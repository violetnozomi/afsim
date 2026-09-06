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
// This class represents a directory that contains DEM data.
// ----------------------------------------------------------------------------

#include "FloatGridDir.hpp"

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <sstream>

#include "FloatGridTile.hpp"
#include "UtLog.hpp"

// 'Open' a 'raw' Dem directory.  The specified name should be a
// directory whose immediate descendents are the longitudinal
// subdirectories.
FloatGridDir::FloatGridDir(const char* aDirName, double aSWLat, double aSWLon, double aNELat, double aNELon, int& aError)
   : GeoTileMatrix()
{
   if ((aDirName == nullptr) || (aDirName[0] == '\0'))
   {
      aError = 3; // Null or empty directory
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

      double swLat = aSWLat;
      double swLon = aSWLon;
      double neLat = aNELat;
      double neLon = aNELon;

      SetSWLat(swLat);
      SetSWLon(swLon);
      SetNELat(neLat);
      SetNELon(neLon);

      // Set 'GeoTileMatrix' parameters
      SetNumRows((int)((neLat - swLat) * 10.0) + 1);
      SetNumCols((int)((neLon - swLon) * 10.0) + 1);
      SetLatTileDimDeg(0.1);
      SetLonTileDimDeg(0.1);

      aError = 0;
   }
}

FloatGridDir::FloatGridDir()
{
   mDirName = "";

   // Set 'GeoRect' parameters
   SetSWLat(-90.0);
   SetSWLon(-180.0);
   SetNELat(90.0);
   SetNELon(180.0);

   // Set 'GeoTileMatrix' parameters
   SetNumRows(1800);
   SetNumCols(3600);
   SetLatTileDimDeg(0.1);
   SetLonTileDimDeg(0.1);
}

FloatGridDir::~FloatGridDir() {}

void FloatGridDir::RegisterTile(GeoTile& aTile)
{
   aTile.SetTileMatrix(this);
   int row = GetRow(aTile.GetSWLat() + (aTile.GetNELat() - aTile.GetSWLat()) / 2.0);
   int col = GetCol(aTile.GetSWLon() + (aTile.GetNELon() - aTile.GetSWLon()) / 2.0);
   aTile.SetTileIndex(row * GetNumCols() + col);
}

// Load the specified tile from the directory.  The return value is zero
// for success and non-zero for failure.  If a non-zero value is returned
// then the tile will be loaded with a dummy cell that reflects an
// elevation of zero.

int FloatGridDir::LoadTile(FloatGridTile* aTilePtr, double aLat, double aLon)
{
   int status = 1;

   if (!mDirName.empty())
   {
      std::string fileName;
      GetFileName(mDirName.c_str(), aLat, aLon, fileName);
      status = aTilePtr->LoadTile(fileName.c_str());
   }
   if (status != 0)
   {
      aTilePtr->MakeDummyTile(aLat, aLon);
   }

   assert(aTilePtr->Contains(aLat, aLon));
   RegisterTile(*aTilePtr);
   return status;
}

void FloatGridDir::GetFileName(const std::string& aTopDir, double aLat, double aLon,
                               std::string& aFileName) // out
{
   std::ostringstream oss;
   char               ns = 'n';

   int latIndex = (int)(aLat * 10.0);
   int lonIndex = (int)(aLon * 10.0);

   int latDeg  = std::abs(latIndex) / 10;
   int latFrac = std::abs(latIndex) % 10;
   int lonDeg  = std::abs(lonIndex) / 10;
   int lonFrac = std::abs(lonIndex) % 10;

   if (latIndex < 0)
   {
      ns = 's';
      latFrac += 1;
      if (latFrac == 10)
      {
         latFrac = 0;
         latDeg += 1;
      }
   }
   char ew = 'e';
   if (lonIndex < 0)
   {
      ew = 'w';
      lonFrac += 1;
      if (lonFrac == 10)
      {
         lonFrac = 0;
         lonDeg += 1;
      }
   }

   oss << aTopDir << "/" << ew << lonDeg << "/" << lonFrac << "/" << ns << latDeg << "/" << latFrac;
   aFileName = oss.str();
}

bool FloatGridDir::CheckFile(const std::string& aName)
{
   bool found = true;

   // test existence of file
   // for now, only test for .flt
   // FILE *fp = fopen(aName.c_str(),"rb");
   std::string fullname = aName + ".flt";
   FILE*       fp       = fopen(fullname.c_str(), "rb");
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
