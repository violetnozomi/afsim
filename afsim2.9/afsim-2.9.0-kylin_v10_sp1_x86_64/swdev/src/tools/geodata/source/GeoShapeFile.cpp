// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "GeoShapeFile.hpp"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <vector>

#include "UtCast.hpp"
#include "UtEarth.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

//! Converts a point from the shapefiles CS to geodetic.
//!
//! @param aX Source coordinate system x
//! @param aY Source coordinate system y
//! @param aZ Source coordinate system z
//! @param aLat  Geodetic latitude in degrees.
//! @param aLon  Geodetic longitude in degrees.
//! @param aAlt  Geodetic altitude in meters
namespace
{
void GetLLA(int           aProjectionType,
            unsigned int& aUtmZone,
            bool&         aNorthernHemisphere,
            const double  aX,
            const double  aY,
            const double  aZ,
            double&       aLat,
            double&       aLon,
            double&       aAlt)
{
   if (aProjectionType == GeoShapeFile::ProjectionTypeGeocentric)
   {
      double ecef[3] = {aX, aY, aZ};
      UtEllipsoidalEarth::ConvertECEFToLLA(ecef, aLat, aLon, aAlt);
   }
   else if (aProjectionType == GeoShapeFile::ProjectionTypeUTM)
   {
      UtEllipsoidalEarth::ConvertUTMToLL(aUtmZone, aX, aY, aNorthernHemisphere, aLat, aLon);
      aAlt = aZ;
   }
   else if (aProjectionType == GeoShapeFile::ProjectionTypeGeodetic)
   {
      aLat = aX;
      aLon = aY;
      aAlt = aZ; // TODO this is a place where we need to figure out the LAT/LON ordering problem
   }
}
} // namespace

//! This method returns the value of a shape parameter from the .dbf file.
//!
//! @param aParameterName The parameter to look up.
//! @return The string value of the parameter value.

// public
std::string GeoShapeFile::Shape::QueryParameterValue(const std::string&            aParameterName,
                                                     const std::list<std::string>& aParameterList)
{
   std::list<std::string>::const_iterator valueIt = mParameterValues.begin();

   for (auto&& paramName : aParameterList)
   {
      if (paramName == aParameterName)
      {
         return *valueIt;
      }
      ++valueIt;
   }
   return std::string("0");
}

//! Constructor
//!
//! @param aFileName the filename of the ESRI shapefile.  Leave the extension off.
//! @param aHeightParameter A parameter in the .dbf file used to define the height of the individual shapes. "" is
//! acceptable input.
//! @param aConstHeight A constant height to use for all shapes.  aHeightParameter overrides this value.
//! @param aBaseAltParameter A parameter in the .dbf file used to define the base altitude of the individual shapes. ""
//! is acceptable input.
//! @param aConstBaseAlt A constant base altitude to use for all shapes.  aBaseAltParameter overrides this value.
//! @param aGeocentric Indicates the target coordinate system, if true geocentric, otherwise geodetic.
//! @param aAGL_BaseAltitude If true, agl base altitude will be used, otherwise msl base altitude will be used.
//! @param aElevationCallbackPtr An elevation callback used to vertically offset the base of all read shapes.

// public
GeoShapeFile::GeoShapeFile(const std::string& aFileName, // filename should not include extension .shp'
                           const std::string& aHeightParameter,
                           const double       aConstHeight,
                           const std::string& aBaseAltParameter,
                           const double       aConstBaseAlt,
                           const bool         aGeocentric,
                           const bool         aAGL_BaseAltitude,
                           ElevationCallback* aElevationCallbackPtr)
   : mIsValid(false)
   , mElevationCallbackPtr(aElevationCallbackPtr)
   , mErrorMessage("NO ERROR")
   , mLinesVertCount(0)
{
   std::string projectionFileName;
   std::string shapeFileName;
   std::string databaseFilename;
   std::string indexFileName;

   shapeFileName = aFileName;
   shapeFileName += ".shp";
   databaseFilename = aFileName;
   databaseFilename += ".dbf";
   indexFileName = aFileName;
   indexFileName += ".shx";
   projectionFileName = aFileName;
   projectionFileName += ".prj";

   mProjectionType = GetProjection(projectionFileName.c_str(), mUtmZone, mNorthernHemisphere);
   if (mProjectionType != ProjectionTypeUnknown)
   {
      ReadShapeFile(shapeFileName.c_str(), aConstHeight, aConstBaseAlt);
   }
   else
   {
      mIsValid      = false;
      mErrorMessage = "Could not read shape file because data projection is unknown or unclear or unsupported.";
      auto out      = ut::log::warning() << mErrorMessage;
      out.AddNote()
         << "Currently geocentric, geodetic, or UTM projections specified in the projection file are supported.";
      out.AddNote() << "Projection File: " << projectionFileName;
   }
   LoadShapeParameters(databaseFilename.c_str(), aHeightParameter, aBaseAltParameter);

   ReprojectAndVertOffset(aGeocentric, aAGL_BaseAltitude);
}

GeoShapeFile::GeoShapeFile(int shapeType, int projectionType, const bool aAGL_BaseAltitude, ElevationCallback* aElevationCallbackPtr)
   : mShapeType(shapeType)
   , mProjectionType(projectionType)
   , mElevationCallbackPtr(aElevationCallbackPtr)
   , mLinesVertCount(0)
{
}

// virtual
//! deconstructor
//!

// public
GeoShapeFile::~GeoShapeFile()
{
   delete mElevationCallbackPtr;
}

//! Reads the .prj file, and determines the projection of the data in the .shp file.
//!
//! @param aPrjFileName The .prj filename.
//! @param aZone If the projection is UTM, this will return the zone
//! @param aNorthernHemisphere If the projection is UTM, this will be true when the data is in the northern hemisphere,
//! else south
//! @return An enumeration indicating the projection type.

// private
int GeoShapeFile::GetProjection(const std::string& aPrjFileName, unsigned int& aZone, bool& aNorthernHemisphere)
{
   aZone               = 0;
   aNorthernHemisphere = true;
   int retval          = ProjectionTypeUnknown;

   std::ifstream inputFile(aPrjFileName.c_str());
   inputFile.seekg(0, std::fstream::end);
   std::streamoff fileLen = inputFile.tellg();
   inputFile.seekg(0, std::fstream::beg);

   if (!inputFile)
   {
      auto out = ut::log::error() << "Could not read file.";
      out.AddNote() << "File: " << aPrjFileName;
      return retval;
   }
   std::string str(static_cast<unsigned int>(fileLen), '\0');
   inputFile.read(&str[0], fileLen);

   if (0 == strncmp(str.c_str(), "GEOGCS", 6))
   {
      retval = ProjectionTypeGeodetic;
   }
   else if (0 == strncmp(str.c_str(), "GEOCCS", 6))
   {
      retval = ProjectionTypeGeocentric;
   }
   else if (0 == strncmp(str.c_str(), "PROJCS", 6))
   {
      // try to figure out if it is UTM
      unsigned int place = 6;
      int          depth = 0;
      // read past first ( or [
      while (((str[place] != '[') && (str[place] != '(')) && (place < strlen(str.c_str())))
      {
         ++place;
      }
      depth++;

      // read past name contained in ""
      while ((str[place] != '"') && (place < strlen(str.c_str())))
      {
         ++place;
      }
      ++place;
      while ((str[place] != '"') && (place < strlen(str.c_str())))
      {
         ++place;
      }

      while ((strncmp("SPHEROID", &str[place], 8) != 0) && (place < strlen(str.c_str())))
      {
         ++place;
      }
      place += 8;
      while ((str[place] != '"') && (place < strlen(str.c_str())))
      {
         ++place;
      }
      ++place;
      if (0 != strncmp("WGS84", &str[place], 5))
      {
         std::string datum = "";
         while ((str[place] != '"') && (place < strlen(str.c_str())))
         {
            datum += str[place];
            ++place;
         }

         auto out = ut::log::error() << "Could not read file. Unknown datum.";
         out.AddNote() << "File: " << aPrjFileName;
         out.AddNote() << "Datum: " << datum;
         return retval;
      }

      // read projection, this should be Transverse_Mercator
      while ((strncmp("PROJECTION", &str[place], 10) != 0) && (place < strlen(str.c_str())))
      {
         ++place;
      }
      place += 10;
      while ((str[place] != '"') && (place < strlen(str.c_str())))
      {
         ++place;
      }
      ++place;
      while ((strncmp("Transverse_Mercator", &str[place], 19) != 0) && (place < strlen(str.c_str())))
      {
         ++place;
      }
      if (strncmp("Transverse_Mercator", &str[place], 19) == 0)
      {
         retval = ProjectionTypeUTM;
         place += 19;

         while (place < strlen(str.c_str()))
         {
            while ((strncmp("PARAMETER", &str[place], 9) != 0) && (place < strlen(str.c_str())))
            {
               ++place;
            }
            place += 9;
            while ((str[place] != '"') && (place < strlen(str.c_str())))
            {
               ++place;
            }
            ++place;
            if ((strncmp("False_Northing", &str[place], 14) == 0) || (strncmp("false_northing", &str[place], 14) == 0))
            {
               place += 14;
               while ((str[place] != ',') && (place < strlen(str.c_str())))
               {
                  ++place;
               }
               ++place;
               int fn;
               sscanf(&str[place], "%d", &fn);
               if (fn > 0)
               {
                  aNorthernHemisphere = false;
               }
            }
            else if ((strncmp("Central_Meridian", &str[place], 16) == 0) ||
                     (strncmp("central_meridian", &str[place], 16) == 0))
            {
               place += 16;
               while ((str[place] != ',') && (place < strlen(str.c_str())))
               {
                  ++place;
               }
               ++place;
               int cm;
               sscanf(&str[place], "%d", &cm);
               aZone = (cm + 180) / 6 + 1;
            }
         }
      }
      // else we are done, we don't know what to do with this projection
   }

   inputFile.close();

   return retval;
}

//! This method reads a .shp file.
//!
//! @param aShpFileName The .shp file.
//! @param aConstHeight A constant height to apply to all shapes.
//! @param aConstBaseAlt A constant base altitude to apply to all shapes.
//! @return returns 1 on success, 0 on failure.

// private
int GeoShapeFile::ReadShapeFile(const std::string& aShpFileName, const double aConstHeight, const double aConstBaseAlt)
{
   CompatibleFileStream inputfile(aShpFileName.c_str(), std::fstream::in);
   uint32_t             fileCode;
   uint32_t             unused[20];
   uint32_t             fileLength;
   uint32_t             version;
   mIsValid = false;

   if (!(inputfile.Good()))
   {
      auto out = ut::log::error() << "Could not read shapefile.";
      out.AddNote() << "File: " << aShpFileName;

      mErrorMessage = "Could not read shapefile, file not found.";
      return 0;
   }
   inputfile.Read(reinterpret_cast<char*>(&fileCode), 4, CompatibleFileStream::cBIG_ENDIAN);

   if (fileCode != 9994)
   {
      auto out =
         ut::log::error() << "Shapefile is not an ESRI shapefile. File code does not match specification (9994).";
      out.AddNote() << "Shapefile: " << aShpFileName;

      mErrorMessage = "Shapefile is not valid, file code does not match specification (9994).";
      return 0;
   }

   inputfile.Read(reinterpret_cast<char*>(unused), 20, CompatibleFileStream::cNATIVE);
   inputfile.Read(reinterpret_cast<char*>(&fileLength), 4, CompatibleFileStream::cBIG_ENDIAN);
   fileLength *= 2; // original file length is in 16-bit words, I like bytes.
   inputfile.Read(reinterpret_cast<char*>(&version), 4, CompatibleFileStream::cLITTLE_ENDIAN);
   inputfile.Read(reinterpret_cast<char*>(&mShapeType), 4, CompatibleFileStream::cLITTLE_ENDIAN);
   double minx;
   double miny;
   double maxx;
   double maxy;
   double minz;
   double maxz;
   inputfile.Read(reinterpret_cast<char*>(&minx), 8, CompatibleFileStream::cLITTLE_ENDIAN);
   inputfile.Read(reinterpret_cast<char*>(&miny), 8, CompatibleFileStream::cLITTLE_ENDIAN);
   inputfile.Read(reinterpret_cast<char*>(&maxx), 8, CompatibleFileStream::cLITTLE_ENDIAN);
   inputfile.Read(reinterpret_cast<char*>(&maxy), 8, CompatibleFileStream::cLITTLE_ENDIAN);
   inputfile.Read(reinterpret_cast<char*>(&minz), 8, CompatibleFileStream::cLITTLE_ENDIAN);
   inputfile.Read(reinterpret_cast<char*>(&maxz), 8, CompatibleFileStream::cLITTLE_ENDIAN);
   inputfile.Read(reinterpret_cast<char*>(&mMinM), 8, CompatibleFileStream::cLITTLE_ENDIAN);
   inputfile.Read(reinterpret_cast<char*>(&mMaxM), 8, CompatibleFileStream::cLITTLE_ENDIAN);
   GetLLA(mProjectionType, mUtmZone, mNorthernHemisphere, minx, miny, minz, mMinLatFile, mMinLonFile, mMinAltFile);
   GetLLA(mProjectionType, mUtmZone, mNorthernHemisphere, maxx, maxy, maxz, mMaxLatFile, mMaxLonFile, mMaxAltFile);
   mCenterX = 0.5 * (minx + maxx);
   mCenterY = 0.5 * (miny + maxy);
   mCenterZ = 0.5 * (minz + maxz);
   while (!(inputfile.EndOfFile()))
   {
      Shape shape;
      ReadShape(inputfile, shape);
      if ((shape.GetShapeType() != Shape::ShapeTypeNull) && (!inputfile.EndOfFile()))
      {
         shape.SetHeight(aConstHeight);

         if (aConstBaseAlt >= 0.0)
         {
            shape.SetBaseAltitude(aConstBaseAlt);
         }
         mShapeList.push_back(shape);
      }
   }

   inputfile.Close();

   mIsValid = true;

   return 1;
}

//! This will return true if terrain has been successfully loaded.
//!
//! @return true if we have terrain, false otherwise

// public
bool GeoShapeFile::HasElevationCallbackPtr()
{
   if (mElevationCallbackPtr != nullptr)
   {
      return true;
   }
   return false;
}

//! This method read an individual shape.
//!
//! @param aInputFile A stream to read the shape from
//! @param aShape The output shape.

// private
void GeoShapeFile::ReadShape(CompatibleFileStream& aInputFile, Shape& aShape)
{
   std::list<unsigned int> partList;
   Part                    part;
   unsigned int            length;
   double                  box[8];
   unsigned int            numPoints;
   unsigned int            numParts;
   unsigned int            shapeFileId;
   unsigned int            shapeType;

   aInputFile.Read(reinterpret_cast<char*>(&shapeFileId), 4, CompatibleFileStream::cBIG_ENDIAN);
   if (aInputFile.EndOfFile())
   {
      aShape.SetShapeType(Shape::ShapeTypeNull);
      return;
   }
   aShape.SetShapeFileId(shapeFileId);
   aInputFile.Read(reinterpret_cast<char*>(&length), 4, CompatibleFileStream::cBIG_ENDIAN);
   length *= 2;

   std::streamoff shapeReadStart = aInputFile.TellG();

   aInputFile.Read(reinterpret_cast<char*>(&shapeType), 4, CompatibleFileStream::cLITTLE_ENDIAN);
   aShape.SetShapeType(shapeType);

   switch (shapeType)
   {
   case Shape::ShapeTypeNull:
   {
      break;
   }
   case Shape::ShapeTypePoint:
   {
      Read2dPoints(aInputFile, part, 1);
      aShape.GetPartList().push_back(part);
      break;
   }
   case Shape::ShapeTypeMultiPoint:
   {
      aInputFile.Read(reinterpret_cast<char*>(&box[0]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[1]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[2]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[3]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aShape.SetBox(mProjectionType, mUtmZone, mNorthernHemisphere, box[0], box[1], 0, box[2], box[3], 0);
      aInputFile.Read(reinterpret_cast<char*>(&numPoints), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      Read2dPoints(aInputFile, part, numPoints);
      aShape.GetPartList().push_back(part);
      break;
   }
   case Shape::ShapeTypePolyLine:
   {
      aInputFile.Read(reinterpret_cast<char*>(&box[0]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[1]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[2]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[3]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aShape.SetBox(mProjectionType, mUtmZone, mNorthernHemisphere, box[0], box[1], 0, box[2], box[3], 0);
      aInputFile.Read(reinterpret_cast<char*>(&numParts), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&numPoints), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      mLinesVertCount += 2 * (numPoints - 1);
      ReadParts(aInputFile, partList, numParts, numPoints);
      for (auto&& p : partList)
      {
         part.mPointList.clear();
         Read2dPoints(aInputFile, part, p);
         aShape.GetPartList().push_back(part);
      }
      break;
   }
   case Shape::ShapeTypePolygon:
   {
      aInputFile.Read(reinterpret_cast<char*>(&box[0]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[1]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[2]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[3]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aShape.SetBox(mProjectionType, mUtmZone, mNorthernHemisphere, box[0], box[1], 0, box[2], box[3], 0);
      aInputFile.Read(reinterpret_cast<char*>(&numParts), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&numPoints), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      mLinesVertCount += 2 * numPoints;
      ReadParts(aInputFile, partList, numParts, numPoints);
      for (auto&& p : partList)
      {
         part.mPointList.clear();
         Read2dPoints(aInputFile, part, p, true);
         CalculatePartWindingOrder(part);
         aShape.GetPartList().push_back(part);
      }
      break;
   }
   case Shape::ShapeTypePointM:
   {
      Read2dPoints(aInputFile, part, 1);
      ReadPointValues(aInputFile, part, Shape::ShapeComponentM, 1);
      aShape.GetPartList().push_back(part);
      break;
   }
   case Shape::ShapeTypeMultiPointM:
   {
      aInputFile.Read(reinterpret_cast<char*>(&box[0]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[1]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[2]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[3]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&numPoints), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      Read2dPoints(aInputFile, part, numPoints);
      if ((unsigned int)(aInputFile.TellG()) - shapeReadStart < length)
      {
         aInputFile.Read(reinterpret_cast<char*>(&box[6]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
         aInputFile.Read(reinterpret_cast<char*>(&box[7]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
         ReadPointValues(aInputFile, part, Shape::ShapeComponentM, numPoints);
      }
      aShape.SetBox(mProjectionType, mUtmZone, mNorthernHemisphere, box[0], box[1], 0, box[2], box[3], 0);
      aShape.GetPartList().push_back(part);
      break;
   }
   case Shape::ShapeTypePolyLineM:
   {
      aInputFile.Read(reinterpret_cast<char*>(&box[0]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[1]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[2]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[3]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&numParts), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&numPoints), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      ReadParts(aInputFile, partList, numParts, numPoints);
      mLinesVertCount += 2 * (numPoints - 1);
      for (auto&& p : partList)
      {
         part.mPointList.clear();
         Read2dPoints(aInputFile, part, p);
         aShape.GetPartList().push_back(part);
      }
      if ((unsigned int)(aInputFile.TellG()) - shapeReadStart < length)
      {
         aInputFile.Read(reinterpret_cast<char*>(&box[6]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
         aInputFile.Read(reinterpret_cast<char*>(&box[7]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
         for (auto&& p : aShape.GetPartList())
         {
            ReadPointValues(aInputFile, p, Shape::ShapeComponentM, p.mPointList.size());
         }
      }
      aShape.SetBox(mProjectionType, mUtmZone, mNorthernHemisphere, box[0], box[1], 0, box[2], box[3], 0);
      break;
   }
   case Shape::ShapeTypePolygonM:
   {
      aInputFile.Read(reinterpret_cast<char*>(&box[0]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[1]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[2]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[3]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&numParts), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&numPoints), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      mLinesVertCount += 2 * numPoints;
      ReadParts(aInputFile, partList, numParts, numPoints);
      for (auto&& p : partList)
      {
         part.mPointList.clear();
         Read2dPoints(aInputFile, part, p, true);
         CalculatePartWindingOrder(part);
         aShape.GetPartList().push_back(part);
      }

      if ((unsigned int)(aInputFile.TellG()) - shapeReadStart < length)
      {
         aInputFile.Read(reinterpret_cast<char*>(&box[6]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
         aInputFile.Read(reinterpret_cast<char*>(&box[7]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
         for (auto&& p : aShape.GetPartList())
         {
            ReadPointValues(aInputFile, p, Shape::ShapeComponentM, p.mPointList.size(), true);
         }
      }
      aShape.SetBox(mProjectionType, mUtmZone, mNorthernHemisphere, box[0], box[1], 0, box[2], box[3], 0);
      break;
   }
   case Shape::ShapeTypePointZ:
   {
      Read2dPoints(aInputFile, part, 1);
      ReadPointValues(aInputFile, part, Shape::ShapeComponentZ, 1);
      ReadPointValues(aInputFile, part, Shape::ShapeComponentM, 1);
      aShape.GetPartList().push_back(part);
      break;
   }
   case Shape::ShapeTypeMultiPointZ:
   {
      aInputFile.Read(reinterpret_cast<char*>(&box[0]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[1]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[2]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[3]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&numPoints), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      Read2dPoints(aInputFile, part, numPoints);
      aInputFile.Read(reinterpret_cast<char*>(&box[4]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[5]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      ReadPointValues(aInputFile, part, Shape::ShapeComponentZ, numPoints);

      if ((unsigned int)(aInputFile.TellG()) - shapeReadStart < length)
      {
         aInputFile.Read(reinterpret_cast<char*>(&box[6]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
         aInputFile.Read(reinterpret_cast<char*>(&box[7]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
         ReadPointValues(aInputFile, part, Shape::ShapeComponentM, numPoints);
      }
      aShape.SetBox(mProjectionType, mUtmZone, mNorthernHemisphere, box[0], box[1], box[4], box[2], box[3], box[5]);
      aShape.GetPartList().push_back(part);
      break;
   }
   case Shape::ShapeTypePolyLineZ:
   {
      aInputFile.Read(reinterpret_cast<char*>(&box[0]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[1]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[2]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[3]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&numParts), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&numPoints), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      ReadParts(aInputFile, partList, numParts, numPoints);
      mLinesVertCount += 2 * (numPoints - 1);
      for (auto&& p : partList)
      {
         part.mPointList.clear();
         Read2dPoints(aInputFile, part, p);
         aShape.GetPartList().push_back(part);
      }
      aInputFile.Read(reinterpret_cast<char*>(&box[4]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[5]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      for (auto&& p : aShape.GetPartList())
      {
         ReadPointValues(aInputFile, p, Shape::ShapeComponentZ, p.mPointList.size());
      }

      if ((unsigned int)(aInputFile.TellG()) - shapeReadStart < length)
      {
         aInputFile.Read(reinterpret_cast<char*>(&box[6]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
         aInputFile.Read(reinterpret_cast<char*>(&box[7]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
         for (auto&& p : aShape.GetPartList())
         {
            ReadPointValues(aInputFile, p, Shape::ShapeComponentM, p.mPointList.size());
         }
      }
      aShape.SetBox(mProjectionType, mUtmZone, mNorthernHemisphere, box[0], box[1], box[4], box[1], box[2], box[5]);
      break;
   }
   case Shape::ShapeTypePolygonZ:
   {
      aInputFile.Read(reinterpret_cast<char*>(&box[0]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[1]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[2]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[3]), 8, CompatibleFileStream::cLITTLE_ENDIAN); // box 4
      aInputFile.Read(reinterpret_cast<char*>(&numParts), 4, CompatibleFileStream::cLITTLE_ENDIAN); // num parts, starts byte 36
      aInputFile.Read(reinterpret_cast<char*>(&numPoints),
                      4,
                      CompatibleFileStream::cLITTLE_ENDIAN); // num points, starts byte 40
      mLinesVertCount += 2 * numPoints;
      ReadParts(aInputFile, partList, numParts, numPoints);
      for (auto&& p : partList)
      {
         part.mPointList.clear();
         Read2dPoints(aInputFile, part, p, true);
         CalculatePartWindingOrder(part);
         aShape.GetPartList().push_back(part);
      }
      aInputFile.Read(reinterpret_cast<char*>(&box[4]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[5]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      for (auto&& p : aShape.GetPartList())
      {
         ReadPointValues(aInputFile, p, Shape::ShapeComponentZ, p.mPointList.size(), true);
      }
      if ((unsigned int)(aInputFile.TellG()) - shapeReadStart < length)
      {
         aInputFile.Read(reinterpret_cast<char*>(&box[6]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
         aInputFile.Read(reinterpret_cast<char*>(&box[7]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
         for (auto&& p : aShape.GetPartList())
         {
            ReadPointValues(aInputFile, p, Shape::ShapeComponentM, p.mPointList.size(), true);
         }
      }
      aShape.SetMinMaxXYZ(box[0], box[1], box[2], box[3], box[5]);
      break;
   }
   case Shape::ShapeTypeMultiPatch:
   {
      aInputFile.Read(reinterpret_cast<char*>(&box[0]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[1]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[2]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[3]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&numParts), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&numPoints), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      mLinesVertCount += 2 * numPoints;
      ReadParts(aInputFile, partList, numParts, numPoints);
      for (size_t i = 0; i < partList.size(); ++i)
      {
         aInputFile.Read(reinterpret_cast<char*>(&part.mPartType), 4, CompatibleFileStream::cLITTLE_ENDIAN);
         aShape.GetPartList().push_back(part);
      }
      auto it = partList.begin();
      for (auto&& p : aShape.GetPartList())
      {
         Read2dPoints(aInputFile, p, *it);
         ++it;
      }
      aInputFile.Read(reinterpret_cast<char*>(&box[4]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&box[5]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      for (auto&& p : aShape.GetPartList())
      {
         ReadPointValues(aInputFile, p, Shape::ShapeComponentZ, p.mPointList.size());
      }
      if ((unsigned int)(aInputFile.TellG()) - shapeReadStart < length)
      {
         aInputFile.Read(reinterpret_cast<char*>(&box[6]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
         aInputFile.Read(reinterpret_cast<char*>(&box[7]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
         for (auto&& p : aShape.GetPartList())
         {
            ReadPointValues(aInputFile, p, Shape::ShapeComponentM, p.mPointList.size());
         }
      }
      aShape.SetBox(mProjectionType, mUtmZone, mNorthernHemisphere, box[0], box[1], box[4], box[2], box[3], box[5]);
      break;
   }
   default:
      mShapeType = Shape::ShapeTypeNull;
      break;
   }
}

//! This reads in a 2d point list
//!
//! @param aInputFile An input stream.
//! @param aPart The part to add the points to.
//! @param aNumber The number of points to read.
//! @param aIgnoreLast Indicates that the last point is ignorable (ESRI standard repeats the last one for loops.)

// private
void GeoShapeFile::Read2dPoints(CompatibleFileStream& aInputFile, Part& aPart, const size_t aNumber, const bool aIgnoreLast)
{
   Position pos;
   for (size_t i = 0; i < aNumber; ++i)
   {
      aInputFile.Read(reinterpret_cast<char*>(&(pos.x)), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aInputFile.Read(reinterpret_cast<char*>(&(pos.y)), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      pos.z = 0;
      pos.m = 0;
      if ((!aIgnoreLast) || (i != aNumber - 1))
      {
         aPart.mPointList.push_back(pos);
      }
   }
}

//! This reads additional point information into a point list.
//!
//! @param aInputFile The input stream.
//! @param aPart The part to add to.
//! @param aComponent The component the data should be added to.
//! @param aNumber The number of components to read.

// private
void GeoShapeFile::ReadPointValues(CompatibleFileStream& aInputFile,
                                   Part&                 aPart,
                                   const int             aComponent,
                                   const size_t          aNumber,
                                   const bool            aIgnoreLast)
{
   std::list<Position>::iterator it = aPart.mPointList.begin();
   double                        v;
   for (size_t i = 0; i < aNumber; ++i)
   {
      aInputFile.Read(reinterpret_cast<char*>(&v), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      if (it != aPart.mPointList.end())
      {
         switch (aComponent)
         {
         case Shape::ShapeComponentX:
            it->x = v;
            break;
         case Shape::ShapeComponentY:
            it->y = v;
            break;
         case Shape::ShapeComponentZ:
            it->z = v;
            break;
         case Shape::ShapeComponentM:
            it->m = v;
            break;
         }
         ++it;
      }
   }
   if (aIgnoreLast)
   {
      aInputFile.Read(reinterpret_cast<char*>(&v), 8, CompatibleFileStream::cLITTLE_ENDIAN);
   }
}

//! Reads in a list of parts.
//!
//! @param aInputFile The input stream.
//! @param aVector A list to read the types into.
//! @param aNumber The number of parts to read.
//! @param aNumberPoints the total number of points in the shape

// private
void GeoShapeFile::ReadParts(CompatibleFileStream&    aInputFile,
                             std::list<unsigned int>& aVector,
                             const unsigned int       aNumber,
                             const unsigned int       aNumberPoints)
{
   unsigned int lastNumber = 0U;
   int          val        = 0;
   for (unsigned int i = 0U; i < aNumber; ++i)
   {
      aInputFile.Read(reinterpret_cast<char*>(&val), 4U, CompatibleFileStream::cLITTLE_ENDIAN);
      if (i != 0U)
      {
         aVector.push_back(val - lastNumber);
         lastNumber = ut::safe_cast<unsigned int, int>(val);
      }
      if (i == aNumber - 1U)
      {
         aVector.push_back(aNumberPoints - lastNumber);
      }
   }
}

//! Calculates the winding order of a part, and notes that in the part.  positive indicates CCW, neg is CW.
//! 0 is degenerate.
//!
//! @param aPart The part to calculate for.

// private
void GeoShapeFile::CalculatePartWindingOrder(Part& aPart)
{
   std::list<Position>::iterator first = aPart.mPointList.begin();
   std::list<Position>::iterator a     = aPart.mPointList.begin();
   std::list<Position>::iterator b     = a;
   ++b;
   double area = 0;

   while (b != aPart.mPointList.end())
   {
      area += CrossProduct(*a, *b);
      ++b;
      ++a;
   }
   area += CrossProduct(*a, *first);

   if (area < 0)
   {
      aPart.mWinding = 1;
   }
   else if (area > 0)
   {
      aPart.mWinding = -1;
   }
   else
   {
      aPart.mWinding = 0;
   }
}

//! A simple cross product of 2D points
//!
//! @param aA point 1
//! @param aB point 2

// private
double GeoShapeFile::CrossProduct(const Position& aA, const Position& aB)
{
   return (aA.x * aB.y) - (aA.y * aB.x);
}

//! Load shape parameters from a .dbf file.
//!
//! @param aDatabaseFilename The name of the .dbf file.
//! @param aHeightParam A string indicating the field that contains the height.
//! @param aBaseAltParam A string indicating the field that contains the base altitude.

// private
void GeoShapeFile::LoadShapeParameters(const std::string& aDatabaseFilename,
                                       const std::string& aHeightParam,
                                       const std::string& aBaseAltParam)
{
   unsigned int             numRecords;
   short                    firstRecordPos;
   short                    recordLength;
   char                     dummy[264];
   std::list<unsigned int>  offsetList;
   std::list<unsigned char> lengthList;

   mParameterNames.clear();

   CompatibleFileStream inputfile(aDatabaseFilename.c_str(), std::fstream::in);
   if (!inputfile.Good())
   {
      return;
   }

   inputfile.Read(dummy, 4, CompatibleFileStream::cLITTLE_ENDIAN);

   inputfile.Read(reinterpret_cast<char*>(&numRecords), 4, CompatibleFileStream::cLITTLE_ENDIAN);
   inputfile.Read(reinterpret_cast<char*>(&firstRecordPos), 2, CompatibleFileStream::cLITTLE_ENDIAN);
   inputfile.Read(reinterpret_cast<char*>(&recordLength), 2, CompatibleFileStream::cLITTLE_ENDIAN);

   inputfile.Read(dummy, 16, CompatibleFileStream::cNATIVE);
   inputfile.Read(dummy, 1, CompatibleFileStream::cNATIVE);
   inputfile.Read(dummy, 1, CompatibleFileStream::cNATIVE);
   inputfile.Read(dummy, 2, CompatibleFileStream::cNATIVE);

   // read fields
   std::string  fieldname;
   char         fieldnamefirstchar;
   char         fieldnamerest[10];
   int          heightIdx    = -1;
   int          baseAltIndex = -1;
   unsigned int i            = 0;
   inputfile.Read(&fieldnamefirstchar, 1, CompatibleFileStream::cNATIVE);
   while (fieldnamefirstchar != 0x0D)
   {
      char          fieldType;
      int           fieldOffset;
      unsigned char fieldLength;
      unsigned char fieldDecimalPlaces;
      unsigned char fieldFlag;
      int           fieldAutoIncrementNext;
      unsigned char fieldAutoIncrementStep;

      inputfile.Read(fieldnamerest, 10, CompatibleFileStream::cNATIVE);
      fieldname = fieldnamefirstchar;
      fieldname.append(fieldnamerest, 10);
      size_t strend = fieldname.find_first_of('\0');
      if (strend != std::string::npos)
      {
         fieldname.resize(strend);
      }
      if (fieldname == aHeightParam)
      {
         heightIdx = i;
      }
      if (fieldname == aBaseAltParam)
      {
         baseAltIndex = i;
      }

      inputfile.Read(&fieldType, 1, CompatibleFileStream::cNATIVE);
      inputfile.Read(reinterpret_cast<char*>(&fieldOffset), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      inputfile.Read(reinterpret_cast<char*>(&fieldLength), 1, CompatibleFileStream::cNATIVE);
      inputfile.Read(reinterpret_cast<char*>(&fieldDecimalPlaces), 1, CompatibleFileStream::cNATIVE);
      inputfile.Read(reinterpret_cast<char*>(&fieldFlag), 1, CompatibleFileStream::cNATIVE);
      inputfile.Read(reinterpret_cast<char*>(&fieldAutoIncrementNext), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      inputfile.Read(reinterpret_cast<char*>(&fieldAutoIncrementStep), 1, CompatibleFileStream::cNATIVE);
      inputfile.Read(reinterpret_cast<char*>(&dummy), 8, CompatibleFileStream::cNATIVE);

      mParameterNames.push_back(fieldname);
      offsetList.push_back(fieldOffset);
      lengthList.push_back(fieldLength);

      inputfile.Read(&fieldnamefirstchar, 1, CompatibleFileStream::cNATIVE);
      ++i;
   }

   unsigned char del;
   double        height;
   double        baseAltitude;
   unsigned int  j;

   for (auto&& shape : mShapeList)
   {
      shape.GetParameterValues().clear();
      inputfile.Read(reinterpret_cast<char*>(&del), 1, CompatibleFileStream::cNATIVE); // I am not sure what to do with
                                                                                       // this delete flag
      j = 0;

      for (auto&& lit : lengthList)
      {
         inputfile.Read(dummy, lit, CompatibleFileStream::cNATIVE);
         dummy[lit] = 0;
         shape.GetParameterValues().emplace_back(dummy);
         if (j == (unsigned int)heightIdx)
         {
            sscanf(dummy, "%lf", &height);
            shape.SetHeight(height);
         }
         if (j == (unsigned int)baseAltIndex)
         {
            sscanf(dummy, "%lf", &baseAltitude);
            shape.SetBaseAltitude(baseAltitude);
         }
         ++j;
      }
   }
   inputfile.Close();
}

void GeoShapeFile::MakeGeodetic()
{
   if (mProjectionType == GeoShapeFile::ProjectionTypeGeodetic)
   {
      bool first = true;
      for (auto&& shape : mShapeList)
      {
         for (auto&& part : shape.GetPartList())
         {
            for (auto&& point : part.mPointList)
            {
               if (first)
               {
                  mMinLonFile = point.x;
                  mMaxLonFile = point.x;
                  mMinLatFile = point.y;
                  mMaxLatFile = point.y;
                  mMinAltFile = point.z;
                  mMaxAltFile = point.z;
                  mMinM       = point.m;
                  mMaxM       = point.m;
                  first       = false;
               }
               else
               {
                  mMinLatFile = std::min(point.y, mMinLatFile);
                  mMaxLatFile = std::max(point.y, mMaxLatFile);
                  mMinLonFile = std::min(point.x, mMinLonFile);
                  mMaxLonFile = std::max(point.x, mMaxLonFile);
                  mMinAltFile = std::min(point.z, mMinAltFile);
                  mMaxAltFile = std::max(point.z, mMaxAltFile);
                  mMinM       = std::min(point.m, mMinM);
                  mMaxM       = std::max(point.m, mMaxM);
               }
            }
         }
      }
      return;
   }

   double in[3];

   if (mProjectionType == ProjectionTypeGeocentric)
   {
      for (auto&& shape : mShapeList)
      {
         for (auto&& part : shape.GetPartList())
         {
            for (auto&& point : part.mPointList)
            {
               in[0] = point.x;
               in[1] = point.y;
               in[2] = point.z;
               UtEllipsoidalEarth::ConvertECEFToLLA(in, point.x, point.y, point.z);
            }
         }
      }
   }


   in[0] = mCenterX;
   in[1] = mCenterY;
   in[2] = mCenterZ;

   if (mProjectionType == GeoShapeFile::ProjectionTypeGeocentric)
   {
      UtEllipsoidalEarth::ConvertECEFToLLA(in, mCenterX, mCenterY, mCenterZ);
   }

   mProjectionType = GeoShapeFile::ProjectionTypeGeodetic;
}

void GeoShapeFile::MakeGeocentric()
{
   double out[3];

   for (auto&& shape : mShapeList)
   {
      switch (shape.GetShapeType())
      {
      case (Shape::ShapeTypePoint):
      case (Shape::ShapeTypePointM):
      {
         shape.SetShapeType(Shape::ShapeTypePointZ);
         break;
      }
      case (Shape::ShapeTypeMultiPoint):
      case (Shape::ShapeTypeMultiPointM):
      {
         shape.SetShapeType(Shape::ShapeTypeMultiPointZ);
         break;
      }
      case (Shape::ShapeTypePolyLine):
      case (Shape::ShapeTypePolyLineM):
      {
         shape.SetShapeType(Shape::ShapeTypePolyLineZ);
         break;
      }
      case (Shape::ShapeTypePolygon):
      case (Shape::ShapeTypePolygonM):
      {
         shape.SetShapeType(Shape::ShapeTypePolygonZ);
         break;
      }
      }
      for (auto&& part : shape.GetPartList())
      {
         for (auto&& point : part.mPointList)
         {
            if (mProjectionType == ProjectionTypeGeodetic)
            {
               UtEllipsoidalEarth::ConvertLLAToECEF(point.y, point.x, point.z, out);

               point.x = out[0];
               point.y = out[1];
               point.z = out[2];
            }
         }
      }
   }

   if (mProjectionType == GeoShapeFile::ProjectionTypeGeodetic)
   {
      UtEllipsoidalEarth::ConvertLLAToECEF(mCenterY, mCenterX, mCenterZ, out);
      mCenterX = out[0];
      mCenterY = out[1];
      mCenterZ = out[2];
   }

   mProjectionType = GeoShapeFile::ProjectionTypeGeocentric;
}

//! Reprojects the data into either Geocentric or Geodetic and offsets using the elevation callback.
//!
//! @param aGeocentric If true, the data wil reproject to geocentric, otherwise geodetic.
//! @param aAGL_BaseAltitude If true, agl base altitude will be applied, otherwise msl.

// private
void GeoShapeFile::ReprojectAndVertOffset(const bool aGeocentric,
                                          const bool aAGL_BaseAltitude) // otherwise geodetic
{
   if ((mProjectionType == GeoShapeFile::ProjectionTypeGeocentric) && (aGeocentric) && (mElevationCallbackPtr == nullptr))
   {
      return; // nothing to be done
   }

   double in[3];
   double out[3];
   double altitudeOffset;

   for (auto&& shape : mShapeList)
   {
      if (mProjectionType == GeoShapeFile::ProjectionTypeGeodetic)
      {
         shape.SwapLatLonOnBoundingBox();
      }

      if ((mElevationCallbackPtr != nullptr) && aAGL_BaseAltitude) // is an agl altitude
      {
         shape.SetBaseAltitude(shape.GetBaseAltitude() +
                               (*mElevationCallbackPtr)(shape.GetCenterLat(), shape.GetCenterLon()));

         altitudeOffset = shape.GetBaseAltitude();
      }
      else // is an msl altitude
      {
         altitudeOffset = shape.GetBaseAltitude();
      }
      for (auto&& part : shape.GetPartList())
      {
         for (auto&& point : part.mPointList)
         {
            in[0] = point.x;
            in[1] = point.y;
            in[2] = point.z;
            if (mProjectionType == ProjectionTypeGeocentric)
            {
               UtEllipsoidalEarth::ConvertECEFToLLA(in, out[0], out[1], out[2]);
               if (aGeocentric)
               {
                  UtEllipsoidalEarth::ConvertLLAToECEF(out[0], out[1], altitudeOffset, in);
                  point.x = in[0];
                  point.y = in[1];
                  point.z = in[2];
               }
               else
               {
                  point.x = out[0];
                  point.y = out[1];
                  point.z = altitudeOffset;
               }
            }
            else if (mProjectionType == GeoShapeFile::ProjectionTypeUTM)
            {
               UtEllipsoidalEarth::ConvertUTMToLL(mUtmZone, in[0], in[1], mNorthernHemisphere, out[0], out[1]);
               if (aGeocentric)
               {
                  UtEllipsoidalEarth::ConvertLLAToECEF(out[0], out[1], altitudeOffset, in);
                  point.x = in[0];
                  point.y = in[1];
                  point.z = in[2];
               }
               else
               {
                  point.x = out[0];
                  point.y = out[1];
                  point.z = altitudeOffset;
               }
            }
            else if (mProjectionType == GeoShapeFile::ProjectionTypeGeodetic)
            {
               if (aGeocentric)
               {
                  UtEllipsoidalEarth::ConvertLLAToECEF(in[1], in[0], altitudeOffset, out);
                  point.x = out[0];
                  point.y = out[1];
                  point.z = out[2];
               }
               else
               {
                  point.x = in[1];
                  point.y = in[0];
                  point.z = altitudeOffset;
               }
            }
         }
      }
   }


   in[0] = mCenterX;
   in[1] = mCenterY;
   in[2] = mCenterZ;
   if (mProjectionType == GeoShapeFile::ProjectionTypeUTM)
   {
      UtEllipsoidalEarth::ConvertUTMToLL(mUtmZone, in[0], in[1], mNorthernHemisphere, out[0], out[1]);
      out[2] = in[2];
      if (aGeocentric)
      {
         UtEllipsoidalEarth::ConvertLLAToECEF(out[0], out[1], out[2], in);
         mCenterX = in[0];
         mCenterY = in[1];
         mCenterZ = in[2];
      }
      else
      {
         mCenterX = out[0];
         mCenterY = out[1];
         mCenterZ = out[2];
      }
   }
   else if (mProjectionType == GeoShapeFile::ProjectionTypeGeodetic)
   {
      if (aGeocentric)
      {
         double minECEF[3];
         double maxECEF[3];
         UtEllipsoidalEarth::ConvertLLAToECEF(mMinLonFile, mMinLatFile, mMinAltFile, minECEF);
         UtEllipsoidalEarth::ConvertLLAToECEF(mMaxLonFile, mMaxLatFile, mMaxAltFile, maxECEF);
         mCenterX = 0.5 * (minECEF[0] + maxECEF[0]);
         mCenterY = 0.5 * (minECEF[1] + maxECEF[1]);
         mCenterZ = 0.5 * (minECEF[2] + maxECEF[2]);
      }
      else
      {
         mCenterX = in[1];
         mCenterY = in[0]; // TODO this is a place where we need to figure out the lat/lon ordering problem
         // fix bounds, too
         double temp;
         temp        = mMinLatFile;
         mMinLatFile = mMinLonFile;
         mMinLonFile = temp;
         temp        = mMaxLatFile;
         mMaxLatFile = mMaxLonFile;
         mMaxLonFile = temp; // TODO this is a place where we need to figure out the lat/lon ordering problem
      }
   }
   else if (mProjectionType == GeoShapeFile::ProjectionTypeGeocentric)
   {
      if (!aGeocentric)
      {
         UtEllipsoidalEarth::ConvertECEFToLLA(in, out[0], out[1], out[2]);
         mCenterX = out[0];
         mCenterY = out[1];
         mCenterZ = out[2];
      }
   }
   if (aGeocentric)
   {
      mProjectionType = GeoShapeFile::ProjectionTypeGeocentric;
   }
   else
   {
      mProjectionType = GeoShapeFile::ProjectionTypeGeodetic;
   }
}

//! Builds a geodetic bounding box from source coordinate system min and max values.
//!
//! @param aMinX Source coordinate system minimum x
//! @param aMinY Source coordinate system minimum y
//! @param aMinZ Source coordinate system minimum z
//! @param aMaxX Source coordinate system maximum x
//! @param aMaxY Source coordinate system maximum y
//! @param aMaxZ Source coordinate system maximum z

// public
void GeoShapeFile::Shape::SetBox(const int     aProjectionType,
                                 unsigned int& aUtmZone,
                                 bool&         aNorthernHemisphere,
                                 const double  aMinX,
                                 const double  aMinY,
                                 const double  aMinZ,
                                 const double  aMaxX,
                                 const double  aMaxY,
                                 const double  aMaxZ)
{
   double lat1 = 0.0;
   double lon1 = 0.0;
   double lat2 = 0.0;
   double lon2 = 0.0;
   double lat3 = 0.0;
   double lon3 = 0.0;
   double lat4 = 0.0;
   double lon4 = 0.0;
   double lat5 = 0.0;
   double lon5 = 0.0;
   double lat6 = 0.0;
   double lon6 = 0.0;
   double lat7 = 0.0;
   double lon7 = 0.0;
   double lat8 = 0.0;
   double lon8 = 0.0;
   double alt  = 0.0;

   GetLLA(aProjectionType, aUtmZone, aNorthernHemisphere, aMinX, aMinY, aMinZ, lat1, lon1, alt);
   GetLLA(aProjectionType, aUtmZone, aNorthernHemisphere, aMaxX, aMaxY, aMinZ, lat2, lon2, alt);
   GetLLA(aProjectionType, aUtmZone, aNorthernHemisphere, aMinX, aMaxY, aMinZ, lat3, lon3, alt);
   GetLLA(aProjectionType, aUtmZone, aNorthernHemisphere, aMaxX, aMinY, aMinZ, lat4, lon4, alt);
   GetLLA(aProjectionType, aUtmZone, aNorthernHemisphere, aMinX, aMinY, aMaxZ, lat5, lon5, alt);
   GetLLA(aProjectionType, aUtmZone, aNorthernHemisphere, aMaxX, aMaxY, aMaxZ, lat6, lon6, alt);
   GetLLA(aProjectionType, aUtmZone, aNorthernHemisphere, aMinX, aMaxY, aMaxZ, lat7, lon7, alt);
   GetLLA(aProjectionType, aUtmZone, aNorthernHemisphere, aMaxX, aMinY, aMaxZ, lat8, lon8, alt);

   // we keep the box in geodetic for simplified comparisons.
   mMinLat = (lat1 < lat2) ? lat1 : lat2;
   mMinLat = (mMinLat < lat3) ? mMinLat : lat3;
   mMinLat = (mMinLat < lat4) ? mMinLat : lat4;
   mMinLat = (mMinLat < lat5) ? mMinLat : lat5;
   mMinLat = (mMinLat < lat6) ? mMinLat : lat6;
   mMinLat = (mMinLat < lat7) ? mMinLat : lat7;
   mMinLat = (mMinLat < lat8) ? mMinLat : lat8;
   mMinLon = (lon1 < lon2) ? lon1 : lon2;
   mMinLon = (mMinLon < lon3) ? mMinLon : lon3;
   mMinLon = (mMinLon < lon4) ? mMinLon : lon4;
   mMinLon = (mMinLon < lon5) ? mMinLon : lon5;
   mMinLon = (mMinLon < lon6) ? mMinLon : lon6;
   mMinLon = (mMinLon < lon7) ? mMinLon : lon7;
   mMinLon = (mMinLon < lon8) ? mMinLon : lon8;
   mMaxLat = (lat1 > lat2) ? lat1 : lat2;
   mMaxLat = (mMaxLat > lat3) ? mMaxLat : lat3;
   mMaxLat = (mMaxLat > lat4) ? mMaxLat : lat4;
   mMaxLat = (mMaxLat > lat5) ? mMaxLat : lat5;
   mMaxLat = (mMaxLat > lat6) ? mMaxLat : lat6;
   mMaxLat = (mMaxLat > lat7) ? mMaxLat : lat7;
   mMaxLat = (mMaxLat > lat8) ? mMaxLat : lat8;
   mMaxLon = (lon1 > lon2) ? lon1 : lon2;
   mMaxLon = (mMaxLon > lon3) ? mMaxLon : lon3;
   mMaxLon = (mMaxLon > lon4) ? mMaxLon : lon4;
   mMaxLon = (mMaxLon > lon5) ? mMaxLon : lon5;
   mMaxLon = (mMaxLon > lon6) ? mMaxLon : lon6;
   mMaxLon = (mMaxLon > lon7) ? mMaxLon : lon7;
   mMaxLon = (mMaxLon > lon8) ? mMaxLon : lon8;
}

void GeoShapeFile::Shape::SetMinMaxXYZ(const double aMinX,
                                       const double aMinY,
                                       const double aMaxX,
                                       const double aMaxY,
                                       const double aBaseAltitude)
{
   mMinLat       = aMinX;
   mMinLon       = aMinY;
   mMaxLat       = aMaxX;
   mMaxLon       = aMaxY;
   mBaseAltitude = aBaseAltitude;
}

void GeoShapeFile::Shape::SwapLatLonOnBoundingBox()
{
   double temp = mMinLat;
   mMinLat     = mMinLon;
   mMinLon     = temp;
   temp        = mMaxLat;
   mMaxLat     = mMaxLon;
   mMaxLon     = temp;
}

//! returns the center latitude of the shapes bounding box
//!
//! @return latitude

// public
double GeoShapeFile::Shape::GetCenterLat() const
{
   return 0.5 * (mMinLat + mMaxLat);
}

//! returns the center longitude of the shapes bounding box
//!
//! @return longitude

// public
double GeoShapeFile::Shape::GetCenterLon() const
{
   return 0.5 * (mMinLon + mMaxLon);
}

unsigned int GeoShapeFile::Shape::GetContentLength()
{
   unsigned int contentLength = 0;
   switch (GetShapeType())
   {
   case (Shape::ShapeTypeNull):
   {
      contentLength += 2; // shape type
      break;
   }
   case (Shape::ShapeTypePoint):
   {
      contentLength += 10; // shape type, x, y
      break;
   }
   case (Shape::ShapeTypeMultiPoint):
   {
      unsigned int          nPoints  = 0;
      const std::list<Part> partList = GetPartList();
      for (auto&& part : partList)
      {
         nPoints += (unsigned int)(part.mPointList.size());
      }
      contentLength += 20 + nPoints * 8; // shape type
      break;
   }
   case (Shape::ShapeTypePolyLine):
   {
      unsigned int           nParts   = 0;
      unsigned int           nPoints  = 0;
      const std::list<Part>& partList = GetPartList();
      nParts                          = (unsigned int)(partList.size());
      for (auto&& part : partList)
      {
         nPoints += (unsigned int)(part.mPointList.size());
      }
      contentLength += 22 + nParts * 2 + nPoints * 8;
      break;
   }
   case (Shape::ShapeTypePolygon):
   {
      unsigned int           nParts   = 0;
      unsigned int           nPoints  = 0;
      const std::list<Part>& partList = GetPartList();
      nParts                          = (unsigned int)(partList.size());
      for (auto&& part : partList)
      {
         nPoints += (unsigned int)(part.mPointList.size());
      }
      contentLength += 22 + nParts * 2 + nPoints * 8; // shape type
      break;
   }
   case (Shape::ShapeTypePointM):
   {
      contentLength += 14; // shape type, x, y, m
      break;
   }
   case (Shape::ShapeTypeMultiPointM):
   {
      unsigned int           nPoints  = 0;
      const std::list<Part>& partList = GetPartList();
      for (auto&& part : partList)
      {
         nPoints += (unsigned int)(part.mPointList.size());
      }
      contentLength += 28 + nPoints * 12; // shape type
      break;
   }
   case (Shape::ShapeTypePolyLineM):
   {
      unsigned int           nParts   = 0;
      unsigned int           nPoints  = 0;
      const std::list<Part>& partList = GetPartList();
      nParts                          = (unsigned int)(partList.size());
      for (auto&& part : partList)
      {
         nPoints += (unsigned int)(part.mPointList.size());
      }
      contentLength += 30 + nParts * 2 + nPoints * 12;
      break;
   }
   case (Shape::ShapeTypePolygonM):
   {
      unsigned int           nParts   = 0;
      unsigned int           nPoints  = 0;
      const std::list<Part>& partList = GetPartList();
      nParts                          = (unsigned int)(partList.size());
      for (auto&& part : partList)
      {
         nPoints += (unsigned int)(part.mPointList.size());
      }
      contentLength += 30 + nParts * 2 + nPoints * 12; // shape type
      break;
   }
   case (Shape::ShapeTypePointZ):
   {
      contentLength += 18; // shape type, x, y, z, m
      break;
   }
   case (Shape::ShapeTypeMultiPointZ):
   {
      unsigned int           nPoints  = 0;
      const std::list<Part>& partList = GetPartList();
      for (auto&& part : partList)
      {
         nPoints += (unsigned int)(part.mPointList.size());
      }
      contentLength += 36 + nPoints * 16; // shape type
      break;
   }
   case (Shape::ShapeTypePolyLineZ):
   {
      unsigned int           nParts   = 0;
      unsigned int           nPoints  = 0;
      const std::list<Part>& partList = GetPartList();
      nParts                          = (unsigned int)(partList.size());
      for (auto&& part : partList)
      {
         nPoints += (unsigned int)(part.mPointList.size());
      }
      contentLength += 38 + nParts * 2 + nPoints * 16;
      break;
   }
   case (Shape::ShapeTypePolygonZ):
   {
      unsigned int           nParts   = 0;
      unsigned int           nPoints  = 0;
      const std::list<Part>& partList = GetPartList();
      nParts                          = (unsigned int)(partList.size());
      for (auto&& part : partList)
      {
         nPoints += (unsigned int)(part.mPointList.size());
      }
      contentLength += 38 + (nParts * 2) + (nPoints * 16); // shape type
      break;
   }
   case (Shape::ShapeTypeMultiPatch):
   {
      unsigned int           nParts   = 0;
      unsigned int           nPoints  = 0;
      const std::list<Part>& partList = GetPartList();
      nParts                          = (unsigned int)(partList.size());
      for (auto&& part : partList)
      {
         nPoints += (unsigned int)(part.mPointList.size());
      }
      contentLength += 38 + nParts * 4 + nPoints * 16; // shape type
      break;
   }
   }
   return contentLength;
}

size_t GeoShapeFile::ComputeShpFileLength()
{
   size_t fileLength = 50; // main file header size

   for (auto&& shape : GetShapeList())
   {
      fileLength += 4; // record header
      fileLength += shape.GetContentLength();
   }

   return fileLength;
}

size_t GeoShapeFile::ComputeShxFileLength()
{
   return 50 + 4 * GetShapeList().size();
}

void GeoShapeFile::Shape::CalculateBounds(double (&aBounds)[8])
{
   aBounds[0] = aBounds[1] = aBounds[4] = aBounds[6] = DBL_MAX;
   aBounds[2] = aBounds[3] = aBounds[5] = aBounds[7] = -DBL_MAX;

   for (auto&& part : GetPartList())
   {
      for (auto&& pos : part.mPointList)
      {
         if (aBounds[0] > pos.x)
         {
            aBounds[0] = pos.x;
         }
         if (aBounds[1] > pos.y)
         {
            aBounds[1] = pos.y;
         }
         if (aBounds[2] < pos.x)
         {
            aBounds[2] = pos.x;
         }
         if (aBounds[3] < pos.y)
         {
            aBounds[3] = pos.y;
         }
         /*if (aBounds[4] > pos.z)
         {
            aBounds[4] = pos.z;
         }*/
         if (aBounds[5] < pos.z)
         {
            aBounds[5] = pos.z;
         }
         /*if (aBounds[6] > pos.m)
         {
            aBounds[6] = pos.m;
         }*/
         if (aBounds[7] < pos.m)
         {
            aBounds[7] = pos.m;
         }

         aBounds[4] = 0.0;
         aBounds[6] = 0.0;
      }
   }
}


void GeoShapeFile::Shape::WritePoint(CompatibleFileStream& aOut, bool aGeocentric, const Position& aPos)
{
   if (aGeocentric)
   {
      aOut.Write(reinterpret_cast<const char*>(&aPos.x), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<const char*>(&aPos.y), 8, CompatibleFileStream::cLITTLE_ENDIAN);
   }
   else
   {
      aOut.Write(reinterpret_cast<const char*>(&aPos.y), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<const char*>(&aPos.x), 8, CompatibleFileStream::cLITTLE_ENDIAN);
   }
}

void GeoShapeFile::Shape::WriteRecord(CompatibleFileStream& aOut,
                                      bool                  aGeoCentric,
                                      unsigned int          aRecordNumber) // true geocentric, else geodetic
{
   // Write content header
   uint32_t recordNumber = aRecordNumber;
   aOut.Write(reinterpret_cast<char*>(&recordNumber), 4, CompatibleFileStream::cBIG_ENDIAN);

   uint32_t contentLength = GetContentLength();
   aOut.Write(reinterpret_cast<char*>(&contentLength), 4, CompatibleFileStream::cBIG_ENDIAN);


   uint32_t shapeType = GetShapeType();

   double box[8];
   CalculateBounds(box); // output minLon, minLat, maxLon, maxLat, minZ, maxZ, minM, maxM in geodetic

   // swap lat/lon values for insertion of x/y geodetic values
   double minLon = box[0];
   double maxLon = box[2];
   box[0]        = box[1];
   box[1]        = minLon;
   box[2]        = box[3];
   box[3]        = maxLon;

   switch (shapeType)
   {
   case (Shape::ShapeTypeNull):
   {
      aOut.Write(reinterpret_cast<char*>(&shapeType), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      break;
   }
   case (Shape::ShapeTypePoint):
   {
      aOut.Write(reinterpret_cast<char*>(&shapeType), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      for (auto&& part : GetPartList())
      {
         for (auto&& pos : part.mPointList)
         {
            WritePoint(aOut, aGeoCentric, pos);
         }
      }
      break;
   }
   case (Shape::ShapeTypeMultiPoint):
   {
      aOut.Write(reinterpret_cast<char*>(&shapeType), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[0]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[1]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[2]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[3]), 8, CompatibleFileStream::cLITTLE_ENDIAN);

      for (auto&& part : GetPartList())
      {
         int numPoints = static_cast<int>(part.mPointList.size());
         aOut.Write(reinterpret_cast<char*>(&(numPoints)), 4, CompatibleFileStream::cLITTLE_ENDIAN);
         for (auto&& pos : part.mPointList)
         {
            WritePoint(aOut, aGeoCentric, pos);
         }
      }
      break;
   }
   case (Shape::ShapeTypePolyLine):
   {
      aOut.Write(reinterpret_cast<char*>(&shapeType), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[0]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[1]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[2]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[3]), 8, CompatibleFileStream::cLITTLE_ENDIAN);

      const std::list<Part>& partList = GetPartList();
      int                    numParts = static_cast<int>(partList.size());
      aOut.Write(reinterpret_cast<char*>(&numParts), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      int numPoints = 0;
      for (auto&& part : partList)
      {
         numPoints += static_cast<int>(part.mPointList.size());
      }
      aOut.Write(reinterpret_cast<char*>(&numPoints), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      numPoints = 0;
      for (auto&& part : partList)
      {
         aOut.Write(reinterpret_cast<char*>(&numPoints), 4, CompatibleFileStream::cLITTLE_ENDIAN);
         numPoints += static_cast<int>(part.mPointList.size());
      }
      for (auto&& part : partList)
      {
         for (auto&& pos : part.mPointList)
         {
            WritePoint(aOut, aGeoCentric, pos);
         }
      }
      break;
   }
   case (Shape::ShapeTypePolygon):
   {
      aOut.Write(reinterpret_cast<char*>(&shapeType), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[0]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[1]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[2]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[3]), 8, CompatibleFileStream::cLITTLE_ENDIAN);

      const std::list<Part>& partList = GetPartList();
      int                    numParts = static_cast<int>(partList.size());
      aOut.Write(reinterpret_cast<char*>(&numParts), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      int numPoints = 0;
      for (auto&& part : partList)
      {
         numPoints += static_cast<int>(part.mPointList.size());
      }
      aOut.Write(reinterpret_cast<char*>(&numPoints), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      numPoints = 0;
      for (auto&& part : partList)
      {
         aOut.Write(reinterpret_cast<char*>(&numPoints), 4, CompatibleFileStream::cLITTLE_ENDIAN);
         numPoints += static_cast<int>(part.mPointList.size());
      }
      for (auto&& part : partList)
      {
         for (auto&& pos : part.mPointList)
         {
            WritePoint(aOut, aGeoCentric, pos);
         }
      }
      break;
   }
   case (Shape::ShapeTypePointM):
   {
      aOut.Write(reinterpret_cast<char*>(&shapeType), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      const std::list<Part>& partList = GetPartList();
      for (auto&& part : partList)
      {
         for (auto&& pos : part.mPointList)
         {
            WritePoint(aOut, aGeoCentric, pos);
            aOut.Write(reinterpret_cast<const char*>(&pos.m), 8, CompatibleFileStream::cLITTLE_ENDIAN);
         }
      }
      break;
   }
   case (Shape::ShapeTypeMultiPointM):
   {
      aOut.Write(reinterpret_cast<char*>(&shapeType), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[0]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[1]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[2]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[3]), 8, CompatibleFileStream::cLITTLE_ENDIAN);

      const std::list<Part>& partList  = GetPartList();
      int                    numPoints = 0;
      for (auto&& part : partList)
      {
         numPoints = static_cast<int>(part.mPointList.size());
      }
      aOut.Write(reinterpret_cast<char*>(&numPoints), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      for (auto&& part : partList)
      {
         for (auto&& pos : part.mPointList)
         {
            WritePoint(aOut, aGeoCentric, pos);
         }
      }
      aOut.Write(reinterpret_cast<char*>(&box[6]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[7]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      for (auto&& part : partList)
      {
         for (auto&& pos : part.mPointList)
         {
            aOut.Write(reinterpret_cast<const char*>(&pos.m), 8, CompatibleFileStream::cLITTLE_ENDIAN);
         }
      }
      break;
   }
   case (Shape::ShapeTypePolyLineM):
   {
      aOut.Write(reinterpret_cast<char*>(&shapeType), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[0]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[1]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[2]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[3]), 8, CompatibleFileStream::cLITTLE_ENDIAN);

      const std::list<Part>& partList = GetPartList();
      int                    numParts = static_cast<int>(partList.size());
      aOut.Write(reinterpret_cast<char*>(&numParts), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      int numPoints = 0;
      for (auto&& part : partList)
      {
         numPoints += static_cast<int>(part.mPointList.size());
      }
      aOut.Write(reinterpret_cast<char*>(&numPoints), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      numPoints = 0;
      for (auto&& part : partList)
      {
         aOut.Write(reinterpret_cast<char*>(&numPoints), 4, CompatibleFileStream::cLITTLE_ENDIAN);
         numPoints += static_cast<int>(part.mPointList.size());
      }
      for (auto&& part : partList)
      {
         for (auto&& pos : part.mPointList)
         {
            WritePoint(aOut, aGeoCentric, pos);
         }
      }
      aOut.Write(reinterpret_cast<char*>(&box[6]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[7]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      for (auto&& part : partList)
      {
         for (auto&& pos : part.mPointList)
         {
            aOut.Write(reinterpret_cast<const char*>(&pos.m), 8, CompatibleFileStream::cLITTLE_ENDIAN);
         }
      }
      break;
   }
   case (Shape::ShapeTypePolygonM):
   {
      aOut.Write(reinterpret_cast<char*>(&shapeType), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[0]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[1]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[2]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[3]), 8, CompatibleFileStream::cLITTLE_ENDIAN);

      const std::list<Part>& partList = GetPartList();
      int                    numParts = static_cast<int>(partList.size());
      aOut.Write(reinterpret_cast<char*>(&numParts), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      int numPoints = 0;
      for (auto&& part : partList)
      {
         numPoints += static_cast<int>(part.mPointList.size());
      }
      aOut.Write(reinterpret_cast<char*>(&numPoints), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      numPoints = 0;
      for (auto&& part : partList)
      {
         aOut.Write(reinterpret_cast<char*>(&numPoints), 4, CompatibleFileStream::cLITTLE_ENDIAN);
         numPoints += static_cast<int>(part.mPointList.size());
      }
      for (auto&& part : partList)
      {
         for (auto&& pos : part.mPointList)
         {
            WritePoint(aOut, aGeoCentric, pos);
         }
      }
      aOut.Write(reinterpret_cast<char*>(&box[6]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[7]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      for (auto&& part : partList)
      {
         for (auto&& pos : part.mPointList)
         {
            aOut.Write(reinterpret_cast<const char*>(&pos.m), 8, CompatibleFileStream::cLITTLE_ENDIAN);
         }
      }
      break;
   }
   case (Shape::ShapeTypePointZ):
   {
      aOut.Write(reinterpret_cast<char*>(&shapeType), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      const std::list<Part>& partList = GetPartList();
      for (auto&& part : partList)
      {
         for (auto&& pos : part.mPointList)
         {
            WritePoint(aOut, aGeoCentric, pos);
            aOut.Write(reinterpret_cast<const char*>(&pos.z), 8, CompatibleFileStream::cLITTLE_ENDIAN);
            aOut.Write(reinterpret_cast<const char*>(&pos.m), 8, CompatibleFileStream::cLITTLE_ENDIAN);
         }
      }
      break;
   }
   case (Shape::ShapeTypeMultiPointZ):
   {
      aOut.Write(reinterpret_cast<char*>(&shapeType), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[0]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[1]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[2]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[3]), 8, CompatibleFileStream::cLITTLE_ENDIAN);

      const std::list<Part>& partList  = GetPartList();
      int                    numPoints = 0;
      for (auto&& part : partList)
      {
         numPoints = static_cast<int>(part.mPointList.size());
      }
      aOut.Write(reinterpret_cast<char*>(&numPoints), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      for (auto&& part : partList)
      {
         for (auto&& pos : part.mPointList)
         {
            WritePoint(aOut, aGeoCentric, pos);
         }
      }
      aOut.Write(reinterpret_cast<char*>(&box[4]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[5]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      for (auto&& part : partList)
      {
         for (auto&& pos : part.mPointList)
         {
            aOut.Write(reinterpret_cast<const char*>(&pos.z), 8, CompatibleFileStream::cLITTLE_ENDIAN);
         }
      }
      aOut.Write(reinterpret_cast<char*>(&box[6]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[7]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      for (auto&& part : partList)
      {
         for (auto&& pos : part.mPointList)
         {
            aOut.Write(reinterpret_cast<const char*>(&pos.m), 8, CompatibleFileStream::cLITTLE_ENDIAN);
         }
      }
      break;
   }
   case (Shape::ShapeTypePolyLineZ):
   {
      aOut.Write(reinterpret_cast<char*>(&shapeType), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[0]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[1]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[2]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[3]), 8, CompatibleFileStream::cLITTLE_ENDIAN);

      const std::list<Part>& partList = GetPartList();
      int                    numParts = static_cast<int>(partList.size());
      aOut.Write(reinterpret_cast<char*>(&numParts), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      int numPoints = 0;
      for (auto&& part : partList)
      {
         numPoints += static_cast<int>(part.mPointList.size());
      }
      aOut.Write(reinterpret_cast<char*>(&numPoints), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      numPoints = 0;
      for (auto&& part : partList)
      {
         aOut.Write(reinterpret_cast<char*>(&numPoints), 4, CompatibleFileStream::cLITTLE_ENDIAN);
         numPoints += static_cast<int>(part.mPointList.size());
      }
      for (auto&& part : partList)
      {
         for (auto&& pos : part.mPointList)
         {
            WritePoint(aOut, aGeoCentric, pos);
         }
      }
      aOut.Write(reinterpret_cast<char*>(&box[4]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[5]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      for (auto&& part : partList)
      {
         for (auto&& pos : part.mPointList)
         {
            aOut.Write(reinterpret_cast<const char*>(&pos.z), 8, CompatibleFileStream::cLITTLE_ENDIAN);
         }
      }
      aOut.Write(reinterpret_cast<char*>(&box[6]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[7]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      for (auto&& part : partList)
      {
         for (auto&& pos : part.mPointList)
         {
            aOut.Write(reinterpret_cast<const char*>(&pos.m), 8, CompatibleFileStream::cLITTLE_ENDIAN);
         }
      }
      break;
   }
   case (Shape::ShapeTypePolygonZ):
   {
      aOut.Write(reinterpret_cast<char*>(&shapeType), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[0]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[1]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[2]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[3]), 8, CompatibleFileStream::cLITTLE_ENDIAN);

      const std::list<Part>& partList = GetPartList();
      int                    numParts = static_cast<int>(partList.size());
      aOut.Write(reinterpret_cast<char*>(&numParts), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      int numPoints = 0;
      for (auto&& part : partList)
      {
         numPoints += static_cast<int>(part.mPointList.size());
      }
      aOut.Write(reinterpret_cast<char*>(&numPoints), 4, CompatibleFileStream::cLITTLE_ENDIAN);
      numPoints = 0;
      for (auto&& part : partList)
      {
         aOut.Write(reinterpret_cast<char*>(&numPoints), 4, CompatibleFileStream::cLITTLE_ENDIAN);
         numPoints += static_cast<int>(part.mPointList.size());
      }
      for (auto&& part : partList)
      {
         for (auto&& pos : part.mPointList)
         {
            WritePoint(aOut, aGeoCentric, pos);
         }
      }
      aOut.Write(reinterpret_cast<char*>(&box[4]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[5]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      for (auto&& part : partList)
      {
         for (auto&& pos : part.mPointList)
         {
            aOut.Write(reinterpret_cast<const char*>(&pos.z), 8, CompatibleFileStream::cLITTLE_ENDIAN);
         }
      }
      aOut.Write(reinterpret_cast<char*>(&box[6]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      aOut.Write(reinterpret_cast<char*>(&box[7]), 8, CompatibleFileStream::cLITTLE_ENDIAN);
      for (auto&& part : partList)
      {
         for (auto&& pos : part.mPointList)
         {
            aOut.Write(reinterpret_cast<const char*>(&pos.m), 8, CompatibleFileStream::cLITTLE_ENDIAN);
         }
      }
      break;
   }
   }
}

int GeoShapeFile::Write(const std::string& aFileName,
                        const bool         aGeocentric) // aFilename is 'aFileName'.shp
{
   if (aGeocentric)
   {
      MakeGeocentric();
   }
   else
   {
      MakeGeodetic();
   }

   std::string baseFileName = aFileName;
   if (baseFileName.find(".shp") == baseFileName.size() - 4)
   {
      baseFileName.erase(baseFileName.size() - 4, 4);
   }

   std::string shpFileName  = baseFileName + ".shp";
   std::string shxFileName  = baseFileName + ".shx";
   std::string projFileName = baseFileName + ".prj";
   std::string dbfFileName  = baseFileName + ".dbf";

   bool geocentric = (mProjectionType == GeoShapeFile::ProjectionTypeGeocentric);
   WriteProjectionFile(projFileName, geocentric);

   CompatibleFileStream shx(shxFileName, std::ofstream::out);
   CompatibleFileStream ofs(shpFileName, std::ofstream::out);
   if (!ofs.Good())
   {
      auto out = ut::log::error() << "Could not open file.";
      out.AddNote() << "File: " << shpFileName;
      return 1;
   }
   if (!shx.Good())
   {
      auto out = ut::log::error() << "Could not open file.";
      out.AddNote() << "File: " << shxFileName;
      return 1;
   }

   // Create the header
   uint32_t fileLength = static_cast<uint32_t>(ComputeShpFileLength());
   WriteMainFileHeader(ofs, fileLength, geocentric);
   fileLength = static_cast<uint32_t>(ComputeShxFileLength());
   WriteMainFileHeader(shx, fileLength, geocentric);

   unsigned int i = 1;
   for (auto&& shape : mShapeList)
   {
      int32_t offset = static_cast<int32_t>(ofs.TellP() / 2);
      shx.Write(reinterpret_cast<char*>(&offset), sizeof(offset), CompatibleFileStream::cBIG_ENDIAN);
      shape.WriteRecord(ofs, geocentric, i);
      int32_t recLength = static_cast<int32_t>(ofs.TellP() / 2);
      recLength -= offset;
      shx.Write(reinterpret_cast<char*>(&recLength), sizeof(recLength), CompatibleFileStream::cBIG_ENDIAN);
      ++i;
   }
   shx.Close();
   ofs.Close();

   int status = WriteESRI_DbfFile(dbfFileName, "HGT", "ELEVATION");
   if (!status)
      return 0;

   return 1;
}

void GeoShapeFile::WriteProjectionFile(const std::string& aPrjFileName, bool aGeocentric)
{
   std::ofstream ofs(aPrjFileName.c_str(), std::fstream::out);
   if (ofs.good())
   {
      if (aGeocentric)
      {
         ofs << R"(GEOCCS["Geocentric Coordinate System",DATUM["WGS84",SPHEROID["WGS84",6378137,298.257223560493]],PRIMEM["Greenwich",0],UNIT["Meter",1.0]])";
         ofs.close();
      }
      else
      {
         ofs << R"(GEOGCS["Geographic Coordinate System",DATUM["WGS84",SPHEROID["WGS84",6378137,298.257223560493]],PRIMEM["Greenwich",0],UNIT["degree",0.0174532925199433]])";
         ofs.close();
      }
   }
}

void GeoShapeFile::WriteMainFileHeader(CompatibleFileStream& aOut, unsigned int aFileLength, const bool aGeocentric)
{
   uint32_t fileCode = 9994;
   aOut.Write(reinterpret_cast<char*>(&fileCode), 4, CompatibleFileStream::cBIG_ENDIAN);

   uint32_t dummy = 0;
   aOut.Write(reinterpret_cast<char*>(&dummy), 4);
   aOut.Write(reinterpret_cast<char*>(&dummy), 4);
   aOut.Write(reinterpret_cast<char*>(&dummy), 4);
   aOut.Write(reinterpret_cast<char*>(&dummy), 4);
   aOut.Write(reinterpret_cast<char*>(&dummy), 4);

   uint32_t fileLength = aFileLength;
   aOut.Write(reinterpret_cast<char*>(&fileLength), 4, CompatibleFileStream::cBIG_ENDIAN);

   uint32_t version = 1000;
   aOut.Write(reinterpret_cast<char*>(&version), 4, CompatibleFileStream::cLITTLE_ENDIAN);

   uint32_t shapeType = GetShapeType();
   aOut.Write(reinterpret_cast<char*>(&shapeType), 4, CompatibleFileStream::cLITTLE_ENDIAN);

   if (!aGeocentric)
   {
      double xmin = mMinLatFile;
      aOut.Write(reinterpret_cast<char*>(&xmin), 8, CompatibleFileStream::cLITTLE_ENDIAN);

      double ymin = mMinLonFile;
      aOut.Write(reinterpret_cast<char*>(&ymin), 8, CompatibleFileStream::cLITTLE_ENDIAN);

      double xmax = mMaxLatFile;
      aOut.Write(reinterpret_cast<char*>(&xmax), 8, CompatibleFileStream::cLITTLE_ENDIAN);

      double ymax = mMaxLonFile;
      aOut.Write(reinterpret_cast<char*>(&ymax), 8, CompatibleFileStream::cLITTLE_ENDIAN);

      double zmin = mMinAltFile;
      aOut.Write(reinterpret_cast<char*>(&zmin), 8, CompatibleFileStream::cLITTLE_ENDIAN);

      double zmax = mMaxAltFile;
      aOut.Write(reinterpret_cast<char*>(&zmax), 8, CompatibleFileStream::cLITTLE_ENDIAN);
   }
   else
   {
      double xmin = -6378137;
      aOut.Write(reinterpret_cast<char*>(&xmin), 8, CompatibleFileStream::cLITTLE_ENDIAN);

      double ymin = -6378137;
      aOut.Write(reinterpret_cast<char*>(&ymin), 8, CompatibleFileStream::cLITTLE_ENDIAN);

      double xmax = 6378137;
      aOut.Write(reinterpret_cast<char*>(&xmax), 8, CompatibleFileStream::cLITTLE_ENDIAN);

      double ymax = 6378137;
      aOut.Write(reinterpret_cast<char*>(&ymax), 8, CompatibleFileStream::cLITTLE_ENDIAN);

      double zmin = -6378137;
      aOut.Write(reinterpret_cast<char*>(&zmin), 8, CompatibleFileStream::cLITTLE_ENDIAN);

      double zmax = 6378137;
      aOut.Write(reinterpret_cast<char*>(&zmax), 8, CompatibleFileStream::cLITTLE_ENDIAN);
   }

   double mmin = mMinM;
   aOut.Write(reinterpret_cast<char*>(&mmin), 8, CompatibleFileStream::cLITTLE_ENDIAN);

   double mmax = mMaxM;
   aOut.Write(reinterpret_cast<char*>(&mmax), 8, CompatibleFileStream::cLITTLE_ENDIAN);
}

int GeoShapeFile::WriteESRI_DbfFile(const std::string& aDatabaseFilename,
                                    const std::string& aHeightParam,
                                    const std::string& aBaseAltParam)
{
   std::ofstream ofs(aDatabaseFilename.c_str(), std::ofstream::binary | std::ofstream::out);
   if (!ofs.good())
   {
      { // RAII block
         auto out = ut::log::fatal() << "Could not open ESRI dbf file to write.";
         out.AddNote() << "File: " << aDatabaseFilename;
      }
      exit(1);
   }
   uint32_t numFields = 2 + static_cast<uint32_t>(mParameterNames.size());

   char ftype = 3;
   ofs.write(&ftype, 1);                   // file type
   std::time_t t     = std::time(nullptr); // get time now
   std::tm*    now   = std::localtime(&t);
   auto        dummy = static_cast<uint32_t>(now->tm_year);
   dummy += (1 + now->tm_mon) * 0x100;
   dummy += now->tm_mday * 0x10000;
   ;
   ofs.write(reinterpret_cast<char*>(&dummy), 3); // year
   dummy                    = 0;
   uint32_t numberOfRecords = static_cast<unsigned int>(mShapeList.size());
   ofs.write(reinterpret_cast<char*>(&numberOfRecords), 4);
   uint32_t firstRecordPosition = 33 + 32 * numFields;
   ofs.write(reinterpret_cast<char*>(&firstRecordPosition), 2);
   uint32_t dbfRecordLength = numFields * 8 + 1; // this assumes a fixed field size of 8
   ofs.write(reinterpret_cast<char*>(&dbfRecordLength), 2);
   // write 16 empty bytes
   ofs.write(reinterpret_cast<char*>(&dummy), 4); // reserved
   ofs.write(reinterpret_cast<char*>(&dummy), 4); // reserved
   ofs.write(reinterpret_cast<char*>(&dummy), 4); // reserved
   ofs.write(reinterpret_cast<char*>(&dummy), 4); // reserved

   ofs.write(reinterpret_cast<char*>(&dummy), 1); // write 1 empty byte... table flag
   ofs.write(reinterpret_cast<char*>(&dummy), 1); // write 1 empty byte... code page mark
   ofs.write(reinterpret_cast<char*>(&dummy), 2); // write 2 empty bytes... reserved

   // write the height field
   std::string heightFld = "HGT";
   heightFld.resize(11);
   ofs.write(heightFld.c_str(), 11); // field one: height
   char fieldType = 'C';
   ofs.write(&fieldType, 1); // field type
   uint32_t fieldOffset = 0;
   ofs.write(reinterpret_cast<char*>(&fieldOffset), 4); // field offset
   uint32_t hgtFldLength = 8;
   ofs.write(reinterpret_cast<char*>(&hgtFldLength), 1); // field length
   uint32_t numberOfDecimalPlaces = 0;
   ofs.write(reinterpret_cast<char*>(&numberOfDecimalPlaces), 1); // number of decimal places

   ofs.write(reinterpret_cast<char*>(&dummy), 1); // field flags

   ofs.write(reinterpret_cast<char*>(&dummy), 4); // autoincrement next value

   ofs.write(reinterpret_cast<char*>(&dummy), 1); // autoincrement step value

   ofs.write(reinterpret_cast<char*>(&dummy), 4); // reserved
   ofs.write(reinterpret_cast<char*>(&dummy), 4); // reserved

   // write the "ELEVATION" field
   std::string elevationFld = "ELEVATION";
   elevationFld.resize(11);
   ofs.write(elevationFld.c_str(), 11);
   // fieldType = 'C';
   ofs.write(&fieldType, 1); // field type
   fieldOffset = 0;
   ofs.write(reinterpret_cast<char*>(&fieldOffset), 4); // field offset
   uint32_t elvFldLength = 8;
   ofs.write(reinterpret_cast<char*>(&elvFldLength), 1); // field length
   numberOfDecimalPlaces = 0;
   ofs.write(reinterpret_cast<char*>(&numberOfDecimalPlaces), 1); // number of decimal places

   ofs.write(reinterpret_cast<char*>(&dummy), 1); // field flags

   ofs.write(reinterpret_cast<char*>(&dummy), 4); // autoincrement next value

   ofs.write(reinterpret_cast<char*>(&dummy), 1); // autoincrement step value

   ofs.write(reinterpret_cast<char*>(&dummy), 4); // reserved
   ofs.write(reinterpret_cast<char*>(&dummy), 4); // reserved

   // additional params
   for (auto& p : mParameterNames)
   {
      std::string name = p;
      name.resize(11);
      ofs.write(name.c_str(), 11);
      fieldType = 'C';
      ofs.write(&fieldType, 1);
      fieldOffset = 0;
      ofs.write(reinterpret_cast<char*>(&fieldOffset), 4); // field offset
      uint32_t fldLength = 8;
      ofs.write(reinterpret_cast<char*>(&fldLength), 1); // field length
      numberOfDecimalPlaces = 0;
      ofs.write(reinterpret_cast<char*>(&numberOfDecimalPlaces), 1);

      ofs.write(reinterpret_cast<char*>(&dummy), 1); // field flags

      ofs.write(reinterpret_cast<char*>(&dummy), 4); // autoincrement next value

      ofs.write(reinterpret_cast<char*>(&dummy), 1); // autoincrement step value

      ofs.write(reinterpret_cast<char*>(&dummy), 4); // reserved
      ofs.write(reinterpret_cast<char*>(&dummy), 4); // reserved
   }

   unsigned char hdrRecordTerminator = 0x0D;
   ofs.write(reinterpret_cast<char*>(&hdrRecordTerminator), 1);

   unsigned char deleteFlag = ' ';

   // WRITE THE ACTUAL VALUES FOR THE FIELDS
   for (auto&& shape : mShapeList)
   {
      ofs.write(reinterpret_cast<char*>(&deleteFlag), 1);
      double      height         = shape.GetHeight();
      std::string heightFldValue = std::to_string(height);
      ofs.write(heightFldValue.c_str(), hgtFldLength);
      double      baseAltitude    = shape.GetBaseAltitude();
      std::string baseAltFldValue = std::to_string(baseAltitude);
      // L: The argument is interpreted as a long double (only applies to floating point specifiers: e, E, f, g and G).
      // f: Decimal floating point.
      ofs.write(baseAltFldValue.c_str(), elvFldLength);
      height       = 0.0;
      baseAltitude = 0.0;

      for (auto& p : shape.GetParameterValues())
      {
         std::string val = p;
         val.resize(8);
         ofs.write(val.c_str(), 8);
      }
   }
   unsigned char terminator = 0x1A;
   ofs.write(reinterpret_cast<char*>(&terminator), 1);
   ofs.close();

   return 1;
}

GeoShapeFile::CompatibleFileStream::CompatibleFileStream(const std::string aFilename, std::ios_base::openmode aMode)
   : std::fstream(aFilename, aMode | std::fstream::binary)
{
   size_t      one    = 0x01;
   const char* numPtr = reinterpret_cast<const char*>(&one);
   mNativeMode        = numPtr[0] == 1 ? cLITTLE_ENDIAN : cBIG_ENDIAN;
}

void GeoShapeFile::CompatibleFileStream::Read(char* aIn, std::streamsize aSize, Mode aReadMode)
{
   if ((aReadMode == cNATIVE) || (aReadMode == mNativeMode))
   {
      read(aIn, aSize);
   }
   else
   {
      std::vector<char> temp(ut::safe_cast<size_t>(aSize));
      read(temp.data(), aSize);
      std::reverse_copy(temp.begin(), temp.end(), aIn);
   }
}

void GeoShapeFile::CompatibleFileStream::Write(const char* aOut, std::streamsize aSize, Mode aWriteMode /*= cNATIVE*/)
{
   if ((aWriteMode == cNATIVE) || (aWriteMode == mNativeMode))
   {
      write(aOut, aSize);
   }
   else
   {
      std::vector<char> temp(ut::safe_cast<size_t>(aSize));
      std::reverse_copy(aOut, aOut + aSize, temp.begin());
      write(temp.data(), aSize);
   }
}

void GeoShapeFile::CompatibleFileStream::SeekG(std::streampos aPos)
{
   seekg(aPos);
}

void GeoShapeFile::CompatibleFileStream::SeekG(std::streampos aOffset, std::ios_base::seekdir aWay)
{
   seekg(aOffset, aWay);
}
