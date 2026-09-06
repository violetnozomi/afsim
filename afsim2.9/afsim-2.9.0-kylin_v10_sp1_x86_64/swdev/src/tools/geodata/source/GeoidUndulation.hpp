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

#ifndef GEOID_UNDULATION_HPP
#define GEOID_UNDULATION_HPP

#include "geodata_export.h"

#include <iosfwd>
#include <string>

#include "TblLookup.hpp"

//! Implements a lookup for geoid fluctuations about the WGS-84 reference ellipsoid.
//! This vertical adjustment (as a function of lat, lon) is applied to the ellipsoid
//! to obtain the zero geoid datum, from which DTED terrain heights are measured.

class GEODATA_EXPORT GeoidUndulation
{
public:
   bool ReadData(const std::string& aFilename, const std::string& aEGM_Type = "EGM96");

   // Obtain the local fluctuation of earth terrain height from the reference Ellipsoid.
   float Get(double aLatitude, double aLongitude) const;

protected:
   bool ReadEGM96Data(std::ifstream& aStream);
   bool ReadEGM2008Data(std::ifstream& aStream);

   TblIndVarE<float> mLatValues; //<! evenly spaced Latitude independent values in degrees
   TblIndVarE<float> mLonValues; //<! evenly spaced Longitude independent values in degrees

   TblDepVar2<float> mHeightValuesM; //<! the function table f(lat, lon)

   mutable TblLookupLE<float> mLatLookUp;
   mutable TblLookupLE<float> mLonLookUp;
};

#endif
