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

// ----------------------------------------------------------------------------
//
// This class represents provides a 'minimal' interface for accessing NIMA
// Digital Terrain Elevation Data (DTED).  It is intended to be used by
// other classes that implement higher levels of functionality.
//
// Refer to MIL-PRF-89020A, 19 April 1996, for the DTED specifications.
//
// ----------------------------------------------------------------------------

#include "DtedTile.hpp"

#include <cassert>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "DtedDir.hpp"
#include "NimaUtil.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"

bool DtedTile::sEvaluateChecksum = true;

struct UHLRec
{
public:
   char mSentinel[3];
   char mFill1[1];
   char mLonOrigin[8];
   char mLatOrigin[8];
   char mLonInterval[4];
   char mLatInterval[4];
   char mAbsVertAcc[4];
   char mSecurityCode[3];
   char mUniqueRef[12];
   char mLonPoints[4];
   char mLatPoints[4];
   char mMultipleAcc[1];
   char mReserved1[24];
};

std::istream& operator>>(std::istream& aIn, UHLRec& aRecord)
{
   aIn.read(reinterpret_cast<char*>(&aRecord), sizeof(aRecord));
   if ((aIn.gcount() != sizeof(aRecord)) || (::memcmp(aRecord.mSentinel, "UHL", 3) != 0))
   {
      aIn.setstate(std::ios_base::failbit);
   }
   return aIn;
}

struct DSIRec
{
public:
   char mSentinel[3];
   char mSecClass[1];
   char mSecMarking[2];
   char mSecHandling[27];
   char mReserved1[26];
   char mSeriesDesignator[5]; // 'DTEDn'
   char mUniqueRefNum[15];
   char mReserved2[8];
   char mDataEdNum[2];
   char mMatchMergeVer[1];
   char mMaintDate[4];      // 'YYMM'
   char mMatchMergeDate[4]; // 'YYMM'
   char mMaintDesc[4];
   char mProducerCode[8];
   char mReserved3[16];
   char mProductSpec[9];
   char mProductSpecAmend[2];
   char mProductSpecDate[4];
   char mVerticalDatum[3];   // 'MSL'
   char mHorizontalDatum[5]; // 'WGS84'
   char mCollectionSystem[10];
   char mCompilationDate[4];
   char mReserved4[22];
   char mLatOrigin[9];
   char mLonOrigin[10];
   char mSWLat[7];
   char mSWLon[8];
   char mNWLat[7];
   char mNWLon[8];
   char mNELat[7];
   char mNELon[8];
   char mSELat[7];
   char mSELon[8];
   char mOrient[9];
   char mLatInterval[4];
   char mLonInterval[4];
   char mLatPoints[4];
   char mLonPoints[4];
   char mCellComp[2];
   char mReserved5[101];
   char mReserved6[100];
   char mReserved7[156];
};

std::istream& operator>>(std::istream& aIn, DSIRec& aRecord)
{
   aIn.read(reinterpret_cast<char*>(&aRecord), sizeof(aRecord));
   if ((aIn.gcount() != sizeof(aRecord)) || (::memcmp(aRecord.mSentinel, "DSI", 3) != 0))
   {
      aIn.setstate(std::ios_base::failbit);
   }
   return aIn;
}

struct ACCRec
{
public:
   char          mSentinel[3];
   unsigned char mReserved[2697];
};

std::istream& operator>>(std::istream& aIn, ACCRec& aRecord)
{
   aIn.read(reinterpret_cast<char*>(&aRecord), sizeof(aRecord));
   if (aIn.gcount() != sizeof(aRecord) || (::memcmp(aRecord.mSentinel, "ACC", 3) != 0))
   {
      aIn.setstate(std::ios_base::failbit);
   }
   return aIn;
}

DtedTile::DtedTile()
   : GeoElevationTile()
{
   SetDataType(cSHORT_INT);
   SetDataOrderType(cCOLUMN_MAJOR);
}

// Load the information for the cell that contains the specified location.
// aStatus = 0 if success.
//           1 if the file cannot be opened.
//           2 if the contents appear bad.

// Load a cell into the object.  The return value is:
//   0 = success;
//   1 = could not open file;
//   2 = contents do not appear to be a DTED file.
//
// The caller is responsible for calling 'LoadCleanup' to ensure the file
// is closed and the temporary buffer is release.

// private
int DtedTile::LoadCell(const char* aFileName)
{
   std::ifstream dtedFile(aFileName, std::ios_base::in | std::ios_base::binary);
   if (!dtedFile)
   {
      return 1;
   }

   // Read the user header record (UHL: 80 bytes)
   UHLRec uhlBuf;
   if (!(dtedFile >> uhlBuf))
   {
      return 2;
   }

   // Read the dataset identification record (DSI: 648 bytes)
   DSIRec dsiBuf;
   if (!(dtedFile >> dsiBuf))
   {
      return 2;
   }

   mLatPoints   = NimaUtil::StrToInt(uhlBuf.mLatPoints, 4);
   auto tempInt = NimaUtil::StrToInt(uhlBuf.mLatInterval, 4);
   mLatInterval = static_cast<float>(tempInt) / 10.0F;
   mLatInterval /= (60.0F * 60.0F);
   mHalfLatInterval  = 0.5 * mLatInterval;
   mRecipLatInterval = 1.0 / mLatInterval;

   mLonPoints   = NimaUtil::StrToInt(uhlBuf.mLonPoints, 4);
   tempInt      = NimaUtil::StrToInt(uhlBuf.mLonInterval, 4);
   mLonInterval = static_cast<float>(tempInt) / 10.0F;
   mLonInterval /= (60.0F * 60.0F);
   mHalfLonInterval  = 0.5 * mLonInterval;
   mRecipLonInterval = 1.0 / mLonInterval;

   double swLat(StrToDeg(uhlBuf.mLatOrigin, 8));
   double swLon(StrToDeg(uhlBuf.mLonOrigin, 8));
   double neLat(swLat + 1.0);
   double neLon(swLon + 1.0);

   SetSWLat(swLat);
   SetSWLon(swLon);
   SetNELat(neLat);
   SetNELon(neLon);

   /*
   mLatPoints = NimaUtil::StrToInt(dsiBuf.mLatPoints, 4);
   tempInt = NimaUtil::StrToInt(dsiBuf.mLatInterval, 4);
   mLatInterval = static_cast<float>(tempInt) / 10.0F;
   mLatInterval /= (60.0F * 60.0F);
   mHalfLatInterval  = 0.5 * mLatInterval;
   mRecipLatInterval = 1.0 / mLatInterval;

   mLonPoints = NimaUtil::StrToInt(dsiBuf.mLonPoints, 4);
   tempInt = NimaUtil::StrToInt(dsiBuf.mLonInterval, 4);
   mLonInterval = static_cast<float>(tempInt) / 10.0F;
   mLonInterval /= (60.0F * 60.0F);
   mHalfLonInterval  = 0.5 * mLonInterval;
   mRecipLonInterval = 1.0 / mLonInterval;

   SetSWLat(StrToDeg(dsiBuf.mSWLat, 7));
   SetSWLon(StrToDeg(dsiBuf.mSWLon, 8));
   SetNELat(StrToDeg(dsiBuf.mNELat, 7));
   SetNELon(StrToDeg(dsiBuf.mNELon, 8));
   */

   // Read the accuracy record (ACC: 2700 bytes)
   ACCRec accBuf;
   if (!(dtedFile >> accBuf))
   {
      return 2;
   }

   // Allocate a temporary buffer for reading
   auto dtedBuf = ut::make_unique<unsigned char[]>(2 * mLatPoints);

   // Allocate the space for the actual elevation data
   //   mElevData = new short int[mLonPoints * mLatPoints];
   CreateData(mLonPoints * mLatPoints);

   // Read the data.  Each record represents a line of constant longitude.
   // The values are stored from south to north.
   for (unsigned int lonPoint = 0; lonPoint < mLonPoints; ++lonPoint)
   {
      dtedFile.read(reinterpret_cast<char*>(dtedBuf.get()), 8);
      if ((dtedFile.gcount() != 8) || dtedBuf[0] != 0252)
      {
         return 2;
      }

      // Initialize checksum on the header
      unsigned int checksum =
         dtedBuf[0] + dtedBuf[1] + dtedBuf[2] + dtedBuf[3] + dtedBuf[4] + dtedBuf[5] + dtedBuf[6] + dtedBuf[7];

      // We could add more validity checking using the various counts below.
      // 'recCount' starts at 0 and increases by 1.
      // 'latCount' should be zero.
      // 'lonCount' should be equal to 'lonPoint' (which should be the same
      // as 'recCount' because we have one record for each longitude line).

      dtedFile.read(reinterpret_cast<char*>(dtedBuf.get()), 2 * mLatPoints);
      if (dtedFile.gcount() != (2 * mLatPoints))
      {
         return 2;
      }

      // Move data from temporary buffer to the output area.

      unsigned int lonBase = (lonPoint * mLatPoints);
      for (unsigned int latPoint = 0; latPoint < mLatPoints; ++latPoint)
      {
         unsigned int latPointX2 = latPoint * 2;

         // Increment the checksum
         checksum += (dtedBuf[latPointX2] + dtedBuf[latPointX2 + 1]);

         // Assemble the value
         short int tempShort = (dtedBuf[latPointX2] << 8) | dtedBuf[latPointX2 + 1];

         // Negative values are stored as sign and magnitude;
         // not 2's complement!
         if (tempShort < 0)
         {
            tempShort = -1 * (tempShort & 0x7FFF);
         }
         mData.mAsShortInt[lonBase + latPoint] = tempShort;
      }

      // Read and verify the checksum
      dtedFile.read(reinterpret_cast<char*>(dtedBuf.get()), 4);
      if (dtedFile.gcount() != 4)
      {
         return 2;
      }

      if (sEvaluateChecksum)
      {
         unsigned int expectedChecksum = (dtedBuf[0] << 24) | (dtedBuf[1] << 16) | (dtedBuf[2] << 8) | dtedBuf[3];
         if (checksum != expectedChecksum)
         {
            return 2;
         }
      }
   }
   return 0;
}

int DtedTile::LoadTile(const char* aDirName, int aLevel, double aLat, double aLon)
{
   // Determine the integral location of the southwest corner and the
   // hemisphere indications.  Note special processing where values may
   // be less than zero.

   int lat = static_cast<int>(aLat);
   if (lat > aLat)
   {
      --lat;
   }
   char latChar = 'n';
   if (lat < 0)
   {
      lat     = -lat;
      latChar = 's';
   }
   int lon = static_cast<int>(aLon);
   if (lon > aLon)
   {
      --lon;
   }
   char lonChar = 'e';
   if (lon < 0)
   {
      lon     = -lon;
      lonChar = 'w';
   }

   // Form the file name (<dir>/e000/n00.dt1)
   std::ostringstream oss;

   if ((aDirName != nullptr) && (aDirName[0] != 0))
   {
      oss << aDirName;
      char lastDirChar = aDirName[strlen(aDirName) - 1];
      // Append '/' after path name if not included
      if ((lastDirChar != '/') && (lastDirChar != '\\'))
      {
         oss << '/';
      }
   }
   oss << lonChar << std::setfill('0') << std::setw(3) << lon << '/' << latChar << std::setw(2) << lat << ".dt" << aLevel;

   int status = LoadCell(oss.str().c_str());

   // Create a dummy cell if the cell could not be loaded.
   if (status != 0)
   {
      MakeDummyTile(aLat, aLon);
   }
   return status;
}

// Load the information from the specified file.
// aStatus = 0 if success.
//           1 if the file cannot be opened.
//           2 if the contents appear bad.

int DtedTile::LoadTile(const char* aFileName)
{
   int status = LoadCell(aFileName);
   // Create a dummy cell if the cell could not be loaded.
   if (status != 0)
   {
      MakeDummyTile(0.0, 0.0);
   }
   return status;
}

void DtedTile::MakeDummyTile(double aLat, double aLon, double aLatInterval, double aLonInterval)
{
   // Determine the integral location of the southwest corner that
   // contains the specified location.

   int lat = static_cast<int>(aLat);
   if (lat > aLat)
   {
      --lat;
   }
   int lon = static_cast<int>(aLon);
   if (lon > aLon)
   {
      --lon;
   }

   GeoLatLonGridTile::MakeDummyTile(lat, lon, aLatInterval, aLonInterval);
   /*
      mDummyTile = true;

      SetSWLat(lat);
      SetSWLon(lon);
      SetNELat(lat + aLatInterval);
      SetNELon(lon + aLonInterval);
      mLatInterval      = aLatInterval;
      mHalfLatInterval  = 0.5 * mLatInterval;
      mRecipLatInterval = 1.0 / mLatInterval;
      mLonInterval      = aLonInterval;
      mHalfLonInterval  = 0.5 * mLonInterval;
      mRecipLonInterval = 1.0 / mLonInterval;
      mLatPoints        = 2;
      mLonPoints        = 2;

      // Create dummy elevation data.

      CreateData(4);
      for (unsigned int i = 0; i < 4; ++i)
      {
         mData.mAsShortInt[i] = 0;
      }
   */
}

// Convert a string of the form '[D]DDMMSS[NSEW]' to internal form.

// This is a convenience method for allocating tiles.  This
// is typically passed as an argument to the tile manager.
// It simply creates a new instance of a DtedTile.

// static
DtedTile* DtedTile::TileAllocator()
{
   return new DtedTile();
}

// private
double DtedTile::StrToDeg(const char* aStr, unsigned int aSize)
{
   int tempInt = NimaUtil::StrToInt(aStr, aSize - 1);

   int    ddd    = tempInt / 10000;
   int    mm     = (tempInt / 100) % 100;
   int    ss     = tempInt % 100;
   double result = ddd + (mm / 60.0) + (ss / 3600.0);

   if ((aStr[aSize - 1] == 'S') || (aStr[aSize - 1] == 'W'))
   {
      result = -result;
   }
   else if ((aStr[aSize - 1] != 'N') && (aStr[aSize - 1] != 'E'))
   {
      auto out = ut::log::error() << "Unknown direction in DTED.";
      out.AddNote() << "Direction: " << aStr;
   }
   return result;
}

// static
void DtedTile::SetEvaluateChecksum(bool aEvaluateChecksum)
{
   sEvaluateChecksum = aEvaluateChecksum;
}
