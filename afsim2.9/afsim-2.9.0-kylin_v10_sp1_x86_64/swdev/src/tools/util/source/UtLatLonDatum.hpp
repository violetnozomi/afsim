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

#ifndef UTLATLONDATUM_HPP
#define UTLATLONDATUM_HPP

#include "ut_export.h"

#include <cstddef> // for size_t

class UtBoundingBox;
#include "UtSpatialDatum.hpp"

/**
   This is a datum containing a Latitude and longitude, meant to be used in conjunction with a UtSpatial-tree.
*/
class UT_EXPORT UtLatLonDatum : public UtSpatialDatum
{
public:
   UtLatLonDatum();
   UtLatLonDatum(unsigned int aId);
   UtLatLonDatum(unsigned int aId, const double& aLat, const double& aLon);
   ~UtLatLonDatum() override = default;
   UtLatLonDatum(const UtLatLonDatum& aSrc);

   void SetLat(double aLat) { mLatLon[0] = aLat; }
   void SetLon(double aLon) { mLatLon[1] = aLon; }

   const double* GetLatLon() { return mLatLon; }

   double GetLat() const { return mLatLon[0]; }
   double GetLon() const { return mLatLon[1]; }

   bool IsPoint() const override { return true; }

   bool IsWhollyContainedIn(const UtBoundingBox& aBox) const override;

   bool IsPartiallyContainedIn(const UtBoundingBox& aBox) const override;

private:
   double mLatLon[2];
};

#endif
