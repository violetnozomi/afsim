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

#include "UtLatLonDatum.hpp"

#include "UtBoundingBox.hpp"

UtLatLonDatum::UtLatLonDatum()
   : UtSpatialDatum()
{
}

UtLatLonDatum::UtLatLonDatum(unsigned int aId, const double& aLat, const double& aLon)
   : UtSpatialDatum(aId)
{
   mLatLon[0] = aLat;
   mLatLon[1] = aLon;
}

UtLatLonDatum::UtLatLonDatum(unsigned int aId)
   : UtSpatialDatum(aId)
{
}

UtLatLonDatum::UtLatLonDatum(const UtLatLonDatum& aSrc)
   : UtSpatialDatum(aSrc)
{
   mLatLon[0] = aSrc.mLatLon[0];
   mLatLon[1] = aSrc.mLatLon[1];
}

bool UtLatLonDatum::IsWhollyContainedIn(const UtBoundingBox& aBox) const
{
   //   const double* least    = aBox.GetLeast();
   //   const double* greatest = aBox.GetGreatest();
   //   return ((mLatLon[0] > least[0]) &&
   //           (mLatLon[1] > least[1]) &&
   //           (mLatLon[0] <= greatest[0]) &&
   //           (mLatLon[1] <= greatest[1]));
   return ((mLatLon[0] > aBox.GetLeast()[0]) && (mLatLon[1] > aBox.GetLeast()[1]) &&
           (mLatLon[0] <= aBox.GetGreatest()[0]) && (mLatLon[1] <= aBox.GetGreatest()[1]));
}

bool UtLatLonDatum::IsPartiallyContainedIn(const UtBoundingBox& aBox) const
{
   return IsWhollyContainedIn(aBox);
}
