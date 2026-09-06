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

#ifndef WSFPATHSTATE_HPP
#define WSFPATHSTATE_HPP

#include <cmath>

#include "UtEntity.hpp"

// Represents the state of an entity moving along a WsfPathList
struct WsfPathState
{
   WsfPathState() = default;
   WsfPathState(const WsfPathState& aSrc) { *this = aSrc; }
   WsfPathState(UtEntity& aEntity) { *this = aEntity; }

   WsfPathState& operator=(const WsfPathState& aRhs)
   {
      mLatitude           = aRhs.mLatitude;
      mLongitude          = aRhs.mLongitude;
      mAltitude           = aRhs.mAltitude;
      mOrientationNED[0]  = aRhs.mOrientationNED[0];
      mOrientationNED[1]  = aRhs.mOrientationNED[1];
      mOrientationNED[2]  = aRhs.mOrientationNED[2];
      mVelocityNED[0]     = aRhs.mVelocityNED[0];
      mVelocityNED[1]     = aRhs.mVelocityNED[1];
      mVelocityNED[2]     = aRhs.mVelocityNED[2];
      mAccelerationNED[0] = aRhs.mAccelerationNED[0];
      mAccelerationNED[1] = aRhs.mAccelerationNED[1];
      mAccelerationNED[2] = aRhs.mAccelerationNED[2];
      return *this;
   }

   WsfPathState& operator=(UtEntity& aRhs)
   {
      aRhs.GetLocationLLA(mLatitude, mLongitude, mAltitude);
      aRhs.GetOrientationNED(mOrientationNED[0], mOrientationNED[1], mOrientationNED[2]);
      aRhs.GetVelocityNED(mVelocityNED);
      aRhs.GetAccelerationNED(mAccelerationNED);
      return *this;
   }

   void CopyToEntity(UtEntity& aEntity) const
   {
      aEntity.SetLocationLLA(mLatitude, mLongitude, mAltitude);
      aEntity.SetOrientationNED(mOrientationNED[0], mOrientationNED[1], mOrientationNED[2]);
      aEntity.SetVelocityNED(mVelocityNED);
      aEntity.SetAccelerationNED(mAccelerationNED);
   }

   // Convenience method to allow direct replacement of calls to WsfPlatform::GetLocationLLA().
   void GetLocationLLA(double& aLatitude, double& aLongitude, double& aAltitude) const
   {
      aLatitude  = mLatitude;
      aLongitude = mLongitude;
      aAltitude  = mAltitude;
   }

   double GetSpeed() const
   {
      return sqrt(mVelocityNED[0] * mVelocityNED[0] + mVelocityNED[1] * mVelocityNED[1] +
                  mVelocityNED[2] * mVelocityNED[2]);
   }

   double mLatitude;
   double mLongitude;
   double mAltitude;
   double mOrientationNED[3];
   double mVelocityNED[3];
   double mAccelerationNED[3];
};

#endif
