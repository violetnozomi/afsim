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

#ifndef EquidistantProjection_hpp
#define EquidistantProjection_hpp

#include "geodata_export.h"

#include "MapProjection.hpp"

class GEODATA_EXPORT EquidistantProjection : public MapProjection
{
public:
   EquidistantProjection(double aRadius = 0.0);

   ~EquidistantProjection() override;

   bool ConvertLatLonToXY(double aLat, double aLon, double& aX, double& aY) const override;

   bool ConvertXYToLatLon(double aX, double aY, double& aLat, double& aLon) const override;

   void SetCenter(double aCenterLat, double aCenterLon) override;

private:
   double mRadius;
   double mCenterLat;
   double mCenterLon;
   double mMetersPerDegreeLat;
   double mMetersPerDegreeLon;
};

#endif
