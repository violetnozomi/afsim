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

#ifndef OrthoProjection_hpp
#define OrthoProjection_hpp

#include "geodata_export.h"

#include "MapProjection.hpp"

class GEODATA_EXPORT OrthoProjection : public MapProjection
{
public:
   OrthoProjection();

   ~OrthoProjection() override;

   bool ConvertLatLonToXY(double aLat, double aLon, double& aX, double& aY) const override;

   bool ConvertXYToLatLon(double aX, double aY, double& aLat, double& aLon) const override;

   void SetCenter(double aCenterLat, double aCenterLon) override;

private:
   // Radius of Earth (in meters) at center of projection.
   double mRadius;

   // Latitude and longitude (in radians) of the center of projection.
   double mCenterLat;
   double mCenterLon;

   // Sine and Cosine of the center latitude and longitude.
   double mCosCenterLat;
   double mSinCenterLat;
   double mCosCenterLon;
   double mSinCenterLon;
};

#endif
