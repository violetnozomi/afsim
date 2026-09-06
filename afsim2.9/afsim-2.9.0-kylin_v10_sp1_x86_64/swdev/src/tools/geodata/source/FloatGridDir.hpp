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

#ifndef FloatGridDir_hpp
#define FloatGridDir_hpp

#include "geodata_export.h"
// ----------------------------------------------------------------------------
//
// This class represents a directory that contains Grid-Float.  The directory
// is expected to be in the "standard" processed form.
//
// ----------------------------------------------------------------------------

#include <string>

class FloatGridTile;
class GeoTile;
#include "GeoTileMatrix.hpp"

class GEODATA_EXPORT FloatGridDir : public GeoTileMatrix
{
public:
   // 'Open' a 'raw' Grid-Float directory.  The specified name should be a
   // directory whose immediate descendents are the longitudinal
   // subdirectories.
   FloatGridDir(const char* aDirName, double aSWLat, double aSWLon, double aNELat, double aNELon, int& aError);

   // 'Open' a dummy Grid-Float directory.  This is just a placeholder used by the
   // tile manager as a place to hang dummy tiles that for places where no
   // Grid-Float data exists.
   FloatGridDir();

   ~FloatGridDir() override;

   // Return the name of the directory that contains the Grid-Float.
   const std::string& GetDirName() const;

   // Load the specified tile from the directory.  The return value is zero
   // for success and non-zero for failure.
   int LoadTile(FloatGridTile* aTilePtr, double aLat, double aLon);

   void RegisterTile(GeoTile& aTile);

private:
   bool CheckFile(const std::string& aName);

   void GetFileName(const std::string& aDirName, double aLat, double aLon, std::string& aFileName);

   // These are defined but not implemented to prevent use.
   FloatGridDir(const FloatGridDir& aSrc);
   FloatGridDir& operator=(const FloatGridDir& aRhs) = delete;

   std::string mDirName; // The name of the directory containing the
                         // actual Grid-Float subdirectories
};

// Inline methods

inline const std::string& FloatGridDir::GetDirName() const
{
   return mDirName;
}

#endif
