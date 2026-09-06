// ****************************************************************************
// CUI//REL TO USA ONLY
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

#ifndef UTILITIES_HPP
#define UTILITIES_HPP

class WsfPlatform;

namespace engage
{

class Utilities
{
public:
   Utilities();

   void SetLocationXYZ(WsfPlatform* aPlatformPtr, double aX, double aY, double aZ) const;

   void SetSpeedAndOrientationXYZ(WsfPlatform* aPlatformPtr, double aSpeed, double aHeading, double aPitch, double aRoll) const;

   bool ConvertXY_ToLL(double aX, double aY, double& aLat, double& aLon) const;

   bool ConvertLL_ToXY(double aLat, double aLon, double& aX, double& aY) const;

   void SetCenter(double aCenterLat, double aCenterLon);

private:
   double mCenterLat;
   double mCenterLon;
   double mRadius;
};

} // namespace engage

#endif
