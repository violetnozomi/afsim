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

#ifndef TMProjection_hpp
#define TMProjection_hpp

#include "geodata_export.h"

#include "MapProjection.hpp"

class GEODATA_EXPORT TMProjection : public MapProjection
{
public:
   TMProjection();

   ~TMProjection() override;

   bool ConvertLatLonToXY(double aLat, double aLon, double& aX, double& aY) const override;

   bool ConvertXYToLatLon(double aX, double aY, double& aLat, double& aLon) const override;

   void SetCenter(double aCenterLat, double aCenterLon) override;

   // The following was defined only because I wanted to get the computed
   // radius so I could compare it with some other things I was doing...

   double GetRadius() const { return mRadius; }

private:
   // Radius of Earth (in meters) at center of projection.
   double mRadius;

   // Latitude and longitude (in radians) of the center of projection.
   double mCenterLat;
   double mCenterLon;
};

#endif
