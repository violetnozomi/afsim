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

#ifndef DTEDDIR_HPP
#define DTEDDIR_HPP

#include "geodata_export.h"
// ----------------------------------------------------------------------------
//
// This class represents a directory that contains DTED.  The directory
// is expected to be in the standard NIMA structure.
//
// Refer to MIL-PRF-89020A, 19 April 1996, for the DTED specifications.
//
// ----------------------------------------------------------------------------

#include <string>

class DtedTile;
#include "GeoTileMatrix.hpp"

class GEODATA_EXPORT DtedDir : public GeoTileMatrix
{
public:
   // 'Open' a DTED directory.  The specified name should be a directory
   // whose immediate descendants are a 'dmed' file and a 'dted' directory
   // that contains the longitudinal subdirectories.

   DtedDir(const char* aDirName, int aLevel, int& aError);

   // 'Open' a 'raw' DTED directory.  The specified name should be a
   // directory whose immediate descendants are the longitudinal
   // subdirectories.

   DtedDir(const char* aDirName, int aLevel, int aSWLat, int aSWLon, int aNELat, int aNELon, int& aError);

   // 'Open' a dummy DTED directory.  This is just a placeholder used by the
   // tile manager as a place to hang dummy tiles that for places where no
   // DTED data exists.

   DtedDir();

   ~DtedDir() override;

   // Return the name of the directory that contains the DTED.

   const char* GetDirName() const { return mDirName.c_str(); }

   // Return the level of DTED stored in this directory

   int GetLevel() const { return mLevel; }

   // Return 'true' if the directory has valid DMED data.
   bool HasValidDMED() const { return mValidDMED; }

   // Load the specified tile from the directory.  The return value is zero
   // for success and non-zero for failure.

   int LoadTile(DtedTile* aTilePtr, int aRow, int aCol);

private:
   int LoadDMEDFile(const char* aDirName);
   int StrToDeg(const char* aStr, unsigned int aSize);

   std::string mDirName; // The name of the directory containing the
                         // actual DTED subdirectories
   int mLevel;           // DTED Level (0, 1 or 2)

   bool mValidDMED; // Is the DMED data valid?

   // These are defined but not implemented to prevent use.
   DtedDir(const DtedDir& aSrc);
   DtedDir& operator=(const DtedDir& aRhs) = delete;
};

#endif
