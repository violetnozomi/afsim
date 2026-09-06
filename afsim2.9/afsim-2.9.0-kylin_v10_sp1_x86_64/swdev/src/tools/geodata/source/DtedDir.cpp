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
// This class represents a directory that contains DTED.  The directory
// is expected to be in the standard NIMA structure.
//
// Refer to MIL-PRF-89020A, 19 April 1996, for the DTED specifications.
//
// ----------------------------------------------------------------------------

#include "DtedDir.hpp"

#include "DtedTile.hpp"
#include "NimaUtil.hpp"
#include "UtLog.hpp"

struct DmedHdr
{
public:
   char mSWLat[3]; // [N/S]nn
   char mNELat[3]; // [N/S]nn
   char mSWLon[4]; // [E/W]nnn
   char mNELon[4]; // [E/W]nnn
   char mUnused[380];
};

struct DmedCell
{
public:
   char mSWLat[3]; // [N/S]nn
   char mSWLon[4]; // [E/W]nnn
   char mDataEd[2];
   char mMatchMergeVer;
   struct
   {
      char mMinElev[6];
      char mMaxElev[6];
      char mMeanElev[6];
      char mUnused;
      char mStdDev[5];
   } mArea[16];
};


// 'Open' a DTED directory.  The specified name should be a directory
// whose immediate descendents are a 'dmed' file and a 'dted' directory
// that contains the longitudinal subdirectories.

DtedDir::DtedDir(const char* aDirName, int aLevel, int& aError)
   : GeoTileMatrix()
   , mDirName()
   , mLevel(aLevel)
   , mValidDMED(false)
{
   if ((aDirName == nullptr) || (aDirName[0] == '\0'))
   {
      aError = 3; // Null or empty directory
   }
   else if ((aLevel < 0) || (aLevel > 2))
   {
      aError = 4; // Invalid level
   }
   else
   {
      mDirName = aDirName;

      // Erase the trailing path delimiter if it is present.

      char lastChar = mDirName[mDirName.size() - 1];
      if ((lastChar == '/') || (lastChar == '\\'))
      {
         mDirName.erase(mDirName.size() - 1);
      }

      aError = LoadDMEDFile(mDirName.c_str());
      if (aError == 0)
      {
         // Standard NIMA format - 'dted' directory is a subdirectory below.
         mDirName += "/dted";
      }
   }
}

// 'Open' a 'raw' DTED directory.  The specified name should be a
// directory whose immediate descendents are the longitudinal
// subdirectories.

DtedDir::DtedDir(const char* aDirName, int aLevel, int aSWLat, int aSWLon, int aNELat, int aNELon, int& aError)
   : GeoTileMatrix()
   , mLevel(aLevel)
   , mValidDMED(false)
{
   if ((aDirName == nullptr) || (aDirName[0] == '\0'))
   {
      aError = 3; // Null or empty directory
   }
   else if ((aLevel < 0) || (aLevel > 2))
   {
      aError = 4; // Invalid level
   }
   else
   {
      mDirName = aDirName;
      mLevel   = aLevel;

      // Erase the trailing path delimiter if it is present.

      char lastChar = mDirName[mDirName.size() - 1];
      if ((lastChar == '/') || (lastChar == '\\'))
      {
         mDirName.erase(mDirName.size() - 1);
      }

      // Set 'GeoRect' parameters
      SetSWLat(aSWLat);
      SetSWLon(aSWLon);
      SetNELat(aNELat);
      SetNELon(aNELon);

      // Set 'GeoTileMatrix' parameters
      SetNumRows(aNELat - aSWLat);
      SetNumCols(aNELon - aSWLon);
      SetLatTileDimDeg(1.0);
      SetLonTileDimDeg(1.0);

      aError = 0;
   }
}

DtedDir::DtedDir()
{
   mDirName = "";
   mLevel   = 0;

   // Set 'GeoRect' parameters
   SetSWLat(-90.0);
   SetSWLon(-180.0);
   SetNELat(90.0);
   SetNELon(180.0);

   // Set 'GeoTileMatrix' parameters
   SetNumRows(180);
   SetNumCols(360);
   SetLatTileDimDeg(1.0);
   SetLonTileDimDeg(1.0);
}

DtedDir::~DtedDir() {}

// Load the 'dmed' file.  Returns 0 for success, 1 if the file can't be
// opened or 2 if a read error was encountered.

// private
int DtedDir::LoadDMEDFile(const char* aDirName)
{
   int i;
   int status = 1;
   mValidDMED = false;

   std::string dmedName(aDirName);
   dmedName += "/dmed";
   FILE* dmedFile = ::fopen(dmedName.c_str(), "rb");
   if (dmedFile == nullptr)
   {
      // The open failed.  NIMA's GeoEngine puts a trailing '.' on the name
      // so we'll try it again with that.
      dmedName += ".";
      dmedFile = ::fopen(dmedName.c_str(), "rb");
   }
   if (dmedFile != nullptr)
   {
      int bytesRead;

      // Get the minimum bounding rectangle (MBR)

      DmedHdr hdrRec;
      bytesRead = static_cast<int>(::fread(&hdrRec, 1, sizeof(DmedHdr), dmedFile));
      if (bytesRead != sizeof(DmedHdr))
      {
         ::fclose(dmedFile);
         return 2;
      }

      int swLat    = StrToDeg(hdrRec.mSWLat, 3);
      int swLon    = StrToDeg(hdrRec.mSWLon, 4);
      int neLat    = StrToDeg(hdrRec.mNELat, 3);
      int neLon    = StrToDeg(hdrRec.mNELon, 4);
      int numCells = (neLat - swLat) * (neLon - swLon);

      if ((neLat <= swLat) || (neLon <= swLon))
      {
         ::fclose(dmedFile);
         return 2;
      }

      // Set 'GeoRect' parameters
      SetSWLat(static_cast<double>(swLat));
      SetSWLon(static_cast<double>(swLon));
      SetNELat(static_cast<double>(neLat));
      SetNELon(static_cast<double>(neLon));

      // Set 'GeoTileMatrix' parameters
      SetNumRows(neLat - swLat);
      SetNumCols(neLon - swLon);
      SetLatTileDimDeg(1.0);
      SetLonTileDimDeg(1.0);

      // Read the descriptor for each cell.  Note that the NIMA GeoEngine
      // sometimes failed to return the correct number of records.  In that
      // case we just indicate that we don't have DMED data and continue on.

      mValidDMED = true;
      DmedCell cellRec;
      for (i = 0; i < numCells; ++i)
      {
         bytesRead = static_cast<int>(::fread(&cellRec, 1, sizeof(DmedCell), dmedFile));
         if (bytesRead != sizeof(DmedCell))
         {
            mValidDMED = false;
            break;
         }
      }
      ::fclose(dmedFile);
      status = 0;
   }
   else
   {
      status = 1;
   }
   return status;
}

// Load the specified tile from the directory.  The return value is zero
// for success and non-zero for failure.  If a non-zero value is returned
// then the tile will be loaded with a dummy cell that reflects an
// elevation of zero.

int DtedDir::LoadTile(DtedTile* aTilePtr, int aRow, int aCol)
{
   int status = 1;

   // Use a location that is in the middle of the tile to avoid any
   // possiblity of numerical errors...
   double lat = GetNELat() - aRow - 0.5;
   double lon = GetSWLon() + aCol + 0.5;

   if (!mDirName.empty())
   {
      status = aTilePtr->LoadTile(mDirName.c_str(), mLevel, lat, lon);
   }
   else
   {
      aTilePtr->MakeDummyTile(lat, lon);
   }
   aTilePtr->SetTileMatrix(this);
   aTilePtr->SetTileIndex((aRow * GetNumCols()) + aCol);
   return status;
}

// Convert a string of the form '[NSEW][D]DD' to an integer.
// private
int DtedDir::StrToDeg(const char* aStr, unsigned int aSize)
{
   int value = NimaUtil::StrToInt(aStr + 1, aSize - 1);
   if ((aStr[0] == 'S') || (aStr[0] == 'W'))
   {
      value = -value;
   }
   else if ((aStr[0] != 'N') && (aStr[0] != 'E'))
   {
      auto out = ut::log::error() << "Unknown direction in DMED.";
      out.AddNote() << "Direction: " << aStr;
   }
   return value;
}
