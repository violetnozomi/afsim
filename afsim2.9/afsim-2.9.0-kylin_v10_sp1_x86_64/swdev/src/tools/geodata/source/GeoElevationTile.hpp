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


#ifndef GEOELEVATIONTILE_HPP
#define GEOELEVATIONTILE_HPP

#include "geodata_export.h"

#include "GeoLatLonGridTile.hpp"

//! A class that supports a rectangular grid of elevation points.
//! DTED and Grid-Float are currently implemented with this class
//! (DTED with short integers and Grid-Float with, well, floats).
//!
//! @note Note that this is not an abstract class.  Elevation tiles are
//! explicitly implemented using a single-dimensional array.
//! This is a choice of speed over flexibility.  The elevation data
//! from these tiles will be accessed very, very often.  It is important
//! to maintain this speed bias.  For example, do not pull the GetElev...
//! methods into parent class virtual methods
//! (~15% performance hit with Win32/VS8).
class GEODATA_EXPORT GeoElevationTile : public GeoLatLonGridTile
{
public:
   GeoElevationTile();

   ~GeoElevationTile() override;

   inline float GetElev(unsigned int aLatIndex, unsigned int aLonIndex) const { return GetValue(aLatIndex, aLonIndex); }

   int GetElevApprox(double aLat, double aLon, float& aElev) { return GetApproxValue(aLat, aLon, aElev); }

   int GetElevInterp(double aLat, double aLon, float& aElev) { return GetInterpValue(aLat, aLon, aElev); }

   //! Load the Dem cell that contains a specified location.  The location
   //! is specified in the WGS-84 reference frame using decimal degrees
   //! with negative values for west and south.
   //! @param aDirName: the name of a Dem directory tree.  It should contain
   //!           a subdirectory 'Dem'.
   //! aParam aLevel  : the Dem level (0, 1, 2);
   //! @return   0 if success.
   //!           1 if the file cannot be opened.
   //!           2 if the contents appear bad.
   virtual int LoadTile(const char* aDirName, int aLevel, double aLat, double aLon) { return 0; }

   //! Load the cell from the specified file.
   //!
   //! @return   0 if success.
   //!           1 if the file cannot be opened.
   //!           2 if the contents appear bad.
   int LoadTile(const char* aFileName) override = 0;

protected:
};

#endif
