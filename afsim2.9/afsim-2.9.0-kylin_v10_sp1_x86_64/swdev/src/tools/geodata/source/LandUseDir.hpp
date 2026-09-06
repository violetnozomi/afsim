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

#ifndef LandUseDir_hpp
#define LandUseDir_hpp

#include "geodata_export.h"

// ----------------------------------------------------------------------------
//
// This class represents a directory that contains Land Use data.
// New format is for GeoCover tiles (1x1 degree); all files in one directory
//
// ----------------------------------------------------------------------------
#include <string>

#include "GeoTileMatrix.hpp"
#include "LandUseTile.hpp"

class GEODATA_EXPORT LandUseDir : public GeoTileMatrix
{
public:
   // 'Open' a dummy Land Use directory.  This is just a placeholder used by the
   // tile manager as a place to hang dummy tiles that for places where no
   // Land Use data exists.
   LandUseDir(LandUseTile::Format aFormat = LandUseTile::cBITMAP);

   // 'Open' a 'raw' Land Use directory.
   LandUseDir(const char*         aDirName,
              double              aSWLat,
              double              aSWLon,
              double              aNELat,
              double              aNELon,
              int&                aError,
              LandUseTile::Format aFormat = LandUseTile::cBITMAP);

   ~LandUseDir() override;

   // Return the name of the directory that contains the Grid-Float.
   const char* GetDirName() const;

   // Load the specified tile from the directory.  The return value is zero
   // for success and non-zero for failure.
   int LoadTile(LandUseTile* aTilePtr, double aLat, double aLon);

private:
   bool CheckFile(const std::string& aName) const;
   void Initialize(double aSW_Lat, double aSW_Lon, double aNE_Lat, double aNE_Lon);

   void GetFileName(double aLat, double aLon, std::string& aFileName);

   std::string         mDirName; // The name of the directory containing the tiles
   double              mCellsize;
   LandUseTile::Format mTileFormat;

   // These are defined but not implemented to prevent use.
   LandUseDir(const LandUseDir& aSrc);
   LandUseDir& operator=(const LandUseDir& aRhs) = delete;
};

#endif
