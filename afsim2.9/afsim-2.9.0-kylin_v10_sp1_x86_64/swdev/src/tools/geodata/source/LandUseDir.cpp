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
// This class represents a directory that contains NLCD (National Land
// Coverage Data).
// ----------------------------------------------------------------------------

#include "LandUseDir.hpp"

#include <cmath>
#include <sstream>
#include <stdlib.h>

#include "LandUseTile.hpp"
#include "UtLog.hpp"

// Fully specify a landuse directory
LandUseDir::LandUseDir(const char*         aDirName,
                       double              aSW_Lat,
                       double              aSW_Lon,
                       double              aNE_Lat,
                       double              aNE_Lon,
                       int&                aError,
                       LandUseTile::Format aFormat) // = LandUseTile::cBITMAP)
   : GeoTileMatrix()
   , mDirName(aDirName)
   , mTileFormat(aFormat)
{
   if ((aDirName == nullptr) || (aDirName[0] == '\0'))
   {
      aError = 3; // Null or empty directory
   }
   else
   {
      // Erase the trailing path delimiter if it is present.
      char lastChar = mDirName[mDirName.size() - 1];
      if ((lastChar == '/') || (lastChar == '\\'))
      {
         mDirName.erase(mDirName.size() - 1);
      }

      Initialize(aSW_Lat, aSW_Lon, aNE_Lat, aNE_Lon);

      // Make sure corner directories exist.
      // If so, assume that the other intermediate dirs exist as well.
      std::string swFilename;
      std::string neFilename;
      GetFileName(aSW_Lat, aSW_Lon, swFilename);
      GetFileName(aNE_Lat - GetLatTileDimDeg(), aNE_Lon - GetLonTileDimDeg(), neFilename);
      //      if (CheckFile(swFilename) &&
      //          CheckFile(neFilename))
      //      {
      aError = 0;
      //      }
      //      else
      //      {
      //         aError = -3;
      //      }
   }
}

LandUseDir::LandUseDir(LandUseTile::Format aFormat) // = LandUseTile::cBITMAP)
   : mCellsize(0)
   , mTileFormat(aFormat)
{
   mDirName = "";
   Initialize(-90.0, -180.0, 90.0, 180.0);
}

LandUseDir::~LandUseDir() {}

// Load the specified tile from the directory.  The return value is zero
// for success and non-zero for failure.  If a non-zero value is returned
// then the tile will be loaded with a dummy cell that reflects an
// land usage of zero.

int LandUseDir::LoadTile(LandUseTile* aTilePtr, double aLat, double aLon)
{
   int status = 1;

   if (!mDirName.empty())
   {
      std::string filename;
      GetFileName(aLat, aLon, filename);
      status = aTilePtr->LoadTile(filename.c_str());
   }
   if (status != 0)
   {
      aTilePtr->MakeDummyTile(aLat, aLon);
   }
   aTilePtr->SetTileMatrix(this);
   int row = GetRow(aLat);
   int col = GetCol(aLon);
   aTilePtr->SetTileIndex(row * GetNumCols() + col);

   return status;
}

void LandUseDir::GetFileName(double aLat, double aLon, std::string& aFileName)
{
   char lonName = 'e', latName = 'n';
   int  lat = (int)aLat;
   int  lon = (int)aLon;

   if (aLat < 0.0)
   {
      latName = 's';
      if (aLat != (double)lat) // values is not exactly on the edge between two tiles
      {
         --lat;
      }
      lat = -lat;
   }
   if (aLon < 0.0)
   {
      lonName = 'w';
      if (aLon != (double)lon)
      {
         --lon;
      }
      lon = -lon;
   }

   std::ostringstream oss;
   oss << mDirName << "/" << latName;
   if (std::abs(lat) < 10)
   {
      oss << '0';
   }
   oss << lat << lonName;
   if (std::abs(lon) < 10)
   {
      oss << "00";
   }
   else if (std::abs(lon) < 100)
   {
      oss << '0';
   }
   oss << lon;
   aFileName = oss.str();
}

bool LandUseDir::CheckFile(const std::string& aName) const
{
   bool found = true;

   // test existence of file
   std::string fullname = aName;
   if (mTileFormat == LandUseTile::cBITMAP)
   {
      fullname.append(".ers");
   }
   else
   {
      fullname.append(".asc");
   }

   FILE* fp = fopen(fullname.c_str(), "r");

   if (fp)
   {
      fclose(fp);
   }
   else
   {
      found    = false;
      auto out = ut::log::error() << "File not found.";
      out.AddNote() << "File: " << fullname;
   }

   return found;
}

// private
//  Set the tile format, either 1 degree square bitmap tiles
//  or 0.1 degree square ARC ASCII tiles.
//@note This must not be called after the directory is used to load and store tiles!
void LandUseDir::Initialize(double aSW_Lat, double aSW_Lon, double aNE_Lat, double aNE_Lon)
{
   SetSWLat(aSW_Lat);
   SetSWLon(aSW_Lon);
   SetNELat(aNE_Lat);
   SetNELon(aNE_Lon);

   if (mTileFormat == LandUseTile::cBITMAP)
   {
      SetLatTileDimDeg(1.0);
      SetLonTileDimDeg(1.0);

      SetNumRows((int)(aNE_Lat - aSW_Lat) + 1);
      SetNumCols((int)(aNE_Lon - aSW_Lon) + 1);
   }
   else
   {
      SetLatTileDimDeg(0.1);
      SetLonTileDimDeg(0.1);

      SetNumRows((int)(aNE_Lat - aSW_Lat) * 10 + 1);
      SetNumCols((int)(aNE_Lon - aSW_Lon) * 10 + 1);
   }
}
