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

#ifndef GeoShapeFile_hpp
#define GeoShapeFile_hpp

#include "geodata_export.h"

#include <fstream>
#include <iosfwd>
#include <list>
#include <string>

// https://www.esri.com/library/whitepapers/pdfs/shapefile.pdf
class GEODATA_EXPORT GeoShapeFile
{
public:
   // Inherit from ElevationCallback and set it as the elevation callback
   // to project the GeoShape up to an elevation.
   struct ElevationCallback
   {
      virtual ~ElevationCallback() {}
      virtual float operator()(double aLat, double aLon) { return 0.0; }
   };

   enum
   {
      ProjectionTypeUnknown    = 0,
      ProjectionTypeUTM        = 1,
      ProjectionTypeGeodetic   = 2,
      ProjectionTypeGeocentric = 3
   };

   GeoShapeFile(const std::string& aFileName, // do not include extension (i.e. .shp)
                const std::string& aHeightParameter,
                const double       aConstHeight,
                const std::string& aBaseAltParameter,
                const double       aConstBaseAlt,
                const bool         aGeocentric,
                const bool         aAGL_BaseAltitude,
                ElevationCallback* aElevationCallbackPtr = nullptr);

   GeoShapeFile(int                shapeType,
                int                projectionType,
                const bool         aAGL_BaseAltitude,
                ElevationCallback* aElevationCallbackPtr = nullptr);

   ~GeoShapeFile();

   int Write(const std::string& aFileName, const bool aGeocentric = false);


   void GetCenter(double& aX, double& aY, double& aZ) const
   {
      aX = mCenterX;
      aY = mCenterY;
      aZ = mCenterZ;
   }

   int GetShapeType() { return mShapeType; }

   void GetLLABoundingBox(double& aMinLat, double& aMinLon, double& aMinAlt, double& aMaxLat, double& aMaxLon, double& aMaxAlt) const
   {
      aMinLat = mMinLatFile;
      aMinLon = mMinLonFile;
      aMinAlt = mMinAltFile;
      aMaxLat = mMaxLatFile;
      aMaxLon = mMaxLonFile;
      aMaxAlt = mMaxAltFile;
   }

   void SetLLABoundingBox(double aMinLat, double aMinLon, double aMinAlt, double aMaxLat, double aMaxLon, double aMaxAlt)
   {
      mMinLatFile = aMinLat;
      mMinLonFile = aMinLon;
      mMinAltFile = aMinAlt;
      mMaxLatFile = aMaxLat;
      mMaxLonFile = aMaxLon;
      mMaxAltFile = aMaxAlt;
   }

   void SetMValues(double aMinM, double aMaxM)
   {
      mMinM = aMinM;
      mMaxM = aMaxM;
   }

   void SetFileAsValid() { mIsValid = true; }

   void SetCenterValues(double aMinX, double aMinY, double aMaxX, double aMaxY, double aMinZ, double aMaxZ)
   {
      mCenterX = 0.5 * (aMinX + aMaxX);
      mCenterY = 0.5 * (aMinY + aMaxY);
      mCenterZ = 0.5 * (aMinZ + aMaxZ);
   }

   std::list<std::string>& GetParameterList() { return mParameterNames; }

   int GetProjectionType() { return mProjectionType; }

   bool IsValid() { return mIsValid; }

   bool HasElevationCallbackPtr();

   std::string& GetErrorMessage() { return mErrorMessage; }

   int GetLinesVertexCount() { return mLinesVertCount; }

private:
   class CompatibleFileStream;

public:
   struct Position
   {
      Position()
         : x(0.0)
         , y(0.0)
         , z(0.0)
         , m(0.0)
      {
      }

      double x;
      double y;
      double z;
      double m;
   };

   struct Part
   {
      enum
      {
         ShapePatchTypeTriangleStrip = 0,
         ShapePatchTypeTriangleFan   = 1,
         ShapePatchTypeOuterRing     = 2,
         ShapePatchTypeInnerRing     = 3,
         ShapePatchTypeFirstRing     = 4,
         ShapePatchTypeRing          = 5
      };

      int                 mPartType;
      std::list<Position> mPointList; // a list of points
      int mWinding; // positive is clockwise, negative is counterclockwise, 0 is degenerate or non-polygon
                    // only used for polygon shapes (cw is a positive polygon, ccw is a hole)
   };


   class GEODATA_EXPORT Shape
   {
   public:
      enum
      {
         ShapeTypeNull        = 0,
         ShapeTypePoint       = 1,
         ShapeTypePolyLine    = 3,
         ShapeTypePolygon     = 5,
         ShapeTypeMultiPoint  = 8,
         ShapeTypePointZ      = 11,
         ShapeTypePolyLineZ   = 13,
         ShapeTypePolygonZ    = 15,
         ShapeTypeMultiPointZ = 18,
         ShapeTypePointM      = 21,
         ShapeTypePolyLineM   = 23,
         ShapeTypePolygonM    = 25,
         ShapeTypeMultiPointM = 28,
         ShapeTypeMultiPatch  = 31
      };
      enum
      {
         ShapeComponentX = 0,
         ShapeComponentY = 1,
         ShapeComponentZ = 2,
         ShapeComponentM = 3
      };

      unsigned int           GetShapeFileId() const { return mShapeFileId; }
      unsigned int           GetShapeType() const { return mShapeType; }
      std::list<Part>&       GetPartList() { return mPartList; }
      const std::list<Part>& GetPartList() const { return mPartList; }
      std::string QueryParameterValue(const std::string& aParameterName, const std::list<std::string>& aParameterList);
      double      GetHeight() const { return mHeight; }
      double      GetBaseAltitude() const { return mBaseAltitude; }

      void   SetShapeFileId(unsigned int aId) { mShapeFileId = aId; }
      void   SetShapeType(unsigned int aType) { mShapeType = aType; }
      void   SetBaseAltitude(double aAltitude) { mBaseAltitude = aAltitude; }
      void   SetHeight(double aHeight) { mHeight = aHeight; }
      void   SetBox(const int     aProjectionType,
                    unsigned int& aUtmZone,
                    bool&         aNorthernHemisphere,
                    const double  aMinX,
                    const double  aMinY,
                    const double  aMinZ,
                    const double  aMaxX,
                    const double  aMaxY,
                    const double  aMaxZ);
      void   SetMinMaxXYZ(const double aMinX, const double aMinY, const double aMaxX, const double aMaxY, const double aBaseAltitude);
      double GetCenterLat() const;
      double GetCenterLon() const;

      void GetLLABoundingBox(double& aMinLat, double& aMinLon, double& aMinAlt, double& aMaxLat, double& aMaxLon, double& aMaxAlt) const
      {
         aMinLat = mMinLat;
         aMinLon = mMinLon;
         aMinAlt = mBaseAltitude;
         aMaxLat = mMaxLat;
         aMaxLon = mMaxLon;
         aMaxAlt = mBaseAltitude + mHeight;
      }
      void SwapLatLonOnBoundingBox();

      void WriteRecord(CompatibleFileStream& aOut, bool aGeoCentric, unsigned int aRecordNumber);
      void WritePoint(CompatibleFileStream& aOut, bool aGeocentric, const Position& aPos);

      unsigned int            GetContentLength();
      void                    CalculateBounds(double (&aBounds)[8]);
      std::list<std::string>& GetParameterValues() { return mParameterValues; }

   protected:
      unsigned int    mShapeFileId;
      unsigned int    mShapeType;
      double          mBaseAltitude;
      double          mHeight;
      std::list<Part> mPartList;
      double          mMinLat;
      double          mMaxLat;
      double          mMinLon;
      double          mMaxLon;

      std::list<std::string> mParameterValues;
   };

   std::list<Shape>& GetShapeList() { return mShapeList; }

private:
   // CompatibleFileStream is a wrapper around fstream that presents an interface for dealing with
   // little-endian/big-endian issues Note that this class also exists within util's UtTiff class.  There wasn't a clean
   // and sensible way to dissect this and given the small amount of code in question it made more sense to keep both.
   class CompatibleFileStream : private std::fstream
   {
   public:
      enum Mode
      {
         cLITTLE_ENDIAN = 0,
         cBIG_ENDIAN    = 1,
         cNATIVE        = 2
      };
      CompatibleFileStream(const std::string aFilename, std::ios_base::openmode aMode);
      ~CompatibleFileStream() override = default;

      bool           IsNativeLittleEndian() const { return mNativeMode == cLITTLE_ENDIAN; }
      bool           Good() const { return good(); }
      bool           EndOfFile() const { return eof(); }
      void           Read(char* aIn, std::streamsize aSize, Mode aReadMode);
      void           Write(const char* aOut, std::streamsize aSize, Mode aWriteMode = cNATIVE);
      void           Close() { close(); }
      std::streampos TellG() { return tellg(); }
      std::streampos TellP() { return tellp(); }
      void           SeekG(std::streampos aPos);
      void           SeekG(std::streampos aOffset, std::ios_base::seekdir aWay);

   private:
      Mode mNativeMode;
   };

   void MakeGeodetic();
   void MakeGeocentric();
   void LoadShapeParameters(const std::string& aDatabaseFileName,
                            const std::string& aHeightParam,
                            const std::string& aBaseAltParam);
   int  GetProjection(const std::string& aPrjFileName, unsigned int& aUtmZone, bool& aNorthernHemisphere);
   void WriteProjectionFile(const std::string& aPrjFileName, const bool aGeocentric);
   void WriteMainFileHeader(CompatibleFileStream& aOut, unsigned int aFileLength, const bool aGeocentric);

   int WriteESRI_DbfFile(const std::string& aDatabaseFilename,
                         const std::string& aHeightParam,
                         const std::string& aBaseAltParam);

   int ReadShapeFile(const std::string& aShpFileName, const double aHeight, const double aBaseAlt);

   void   ReadShape(CompatibleFileStream& aInput, Shape& aShape);
   void   Read2dPoints(CompatibleFileStream& aInput, Part& aPart, const size_t aNumber, const bool aIgnoreLast = false);
   void   ReadPointValues(CompatibleFileStream& aInput,
                          Part&                 aPart,
                          const int             aComponent,
                          const size_t          aNumber,
                          const bool            aIgnoreLast = false);
   void   ReadParts(CompatibleFileStream&    aInput,
                    std::list<unsigned int>& aVector,
                    const unsigned int       aNumber,
                    const unsigned int       aNumberPoints);
   void   CalculatePartWindingOrder(Part& aPart);
   double CrossProduct(const Position& aA, const Position& aB);
   void   ReprojectAndVertOffset(const bool aGeocentric,
                                 const bool aAGL_BaseAltitude); // raise buildings by elevation height and then put all
                                                                // vertices in ECEF if geocentric, else geodetic.

   size_t ComputeShpFileLength();
   size_t ComputeShxFileLength();

   bool                   mIsValid;
   unsigned int           mShapeType;
   double                 mMinLatFile;
   double                 mMinLonFile;
   double                 mMinAltFile;
   double                 mMinM;
   double                 mMaxLatFile;
   double                 mMaxLonFile;
   double                 mMaxAltFile;
   double                 mMaxM;
   double                 mCenterX;
   double                 mCenterY;
   double                 mCenterZ;
   int                    mProjectionType;
   unsigned int           mUtmZone;
   bool                   mNorthernHemisphere;
   ElevationCallback*     mElevationCallbackPtr;
   std::list<Shape>       mShapeList;
   std::list<std::string> mParameterNames; // this correlates with the mParameterValues on the shapes
   std::string            mErrorMessage;
   int                    mLinesVertCount;
};

#endif // GeoShapeFile_hpp
