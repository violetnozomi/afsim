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

#ifndef DTEDTILE_HPP
#define DTEDTILE_HPP

#include "geodata_export.h"

// A 'DtedTile' represents a single DTED cell of 1 degree X 1 degree.

#include "GeoElevationTile.hpp"

class GEODATA_EXPORT DtedTile : public GeoElevationTile
{
public:
   DtedTile();

   ~DtedTile() override = default;


   // Load the DTED cell that contains a specified location.  The location
   // is specified in the WGS-84 reference frame using decimal degrees
   // with negative values for west and south.
   //
   // aDirName: the name of a DTED directory tree.  It should contain
   //           a subdirectory 'dted'.
   // aLevel  : the DTED level (0, 1, 2);
   //
   // aStatus = 0 if success.
   //           1 if the file cannot be opened.
   //           2 if the contents appear bad.

   int LoadTile(const char* aDirName, int aLevel, double aLat, double aLon) override;

   // Load the cell from the specified file.
   //
   // aStatus = 0 if success.
   //           1 if the file cannot be opened.
   //           2 if the contents appear bad.

   int LoadTile(const char* aFileName) override;

   // This is a convenience method for allocating tiles.  This
   // is typically passed as an argument to the tile manager.
   // It simply creates a new instance of a DtedTile.

   static DtedTile* TileAllocator();

   void MakeDummyTile(double aSWLat, double aSWLon, double aLatInterval = 1.0, double aLonInterval = 1.0) override;

   static void SetEvaluateChecksum(bool aEvaluateChecksum);

private:
   int LoadCell(const char* aFileName);

   double StrToDeg(const char* aStr, unsigned int aSize);

   // These are defined but not implemented to prevent use.
   DtedTile(const DtedTile& aSrc);
   DtedTile& operator=(const DtedTile& aRhs) = delete;

   static bool sEvaluateChecksum;
};

#endif
