// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "VaCameraMotion.hpp"

#include "UtVec3.hpp"
#include "VaCameraBase.hpp"
#include "VaObserver.hpp"
#include "VaViewer.hpp"

namespace vespa
{
std::string VaCameraMotion::cMotionTypeStrings[cMAX_MOTION_TYPE] =
   {"geocentric", "tethered", "look-at", "fly-to", "custom", "invalid-motion"};

VaCameraMotion::VaCameraMotion(VaCameraBase* aCameraPtr)
   : mCameraPtr(aCameraPtr)
   , mFovY(45.0)
   , mFovYTimer()
   , mCallbacks()
{
   UtVec3d::Set(mFovYMotion, -1.0);
}

void VaCameraMotion::Initialize(VaCameraMotionData* aDataPtr)
{
   if ((aDataPtr != nullptr) && SupportsMotionData(aDataPtr->mMotionType))
   {
      if (aDataPtr->mFovY >= 0.0)
      {
         mFovY = aDataPtr->mFovY;
         if (mFovY > 180.0)
         {
            mFovY = 180.0;
         }
      }
      UtVec3d::Set(mFovYMotion, aDataPtr->mFovYMotion);
      // Clamp the FovY values to 180.0.
      if (mFovYMotion[0] > 180.0)
      {
         mFovYMotion[0] = 180.0;
      }
      if (mFovYMotion[1] > 180.0)
      {
         mFovYMotion[1] = 180.0;
      }
      mFovYTimer.ResetClock();
   }
}

bool VaCameraMotion::Update(double aTime)
{
   UpdateFovY(aTime);

   return true;
}

void VaCameraMotion::SetSoftLock(bool aState, int aPermissiblies /*= cALL*/, int aBreakConditions /*= cALL*/)
{
   mSoftLock                = aState;
   mSoftLockBreakConditions = aBreakConditions;
   mSoftLockPermissibles    = aPermissiblies;
}

bool VaCameraMotion::HandleSoftLock(InputType aInput)
{
   if (!mSoftLock)
   {
      return true;
   }
   else
   {
      if (aInput & mSoftLockBreakConditions)
      {
         mSoftLock = false;
         return true;
      }
      if (aInput & mSoftLockPermissibles)
      {
         return true;
      }
      else
      {
         return false;
      }
   }
}

void VaCameraMotion::UpdateFovY(double aTime)
{
   double timer = mFovYTimer.GetClock();

   // Make sure the FovY parameters are valid.
   if ((mFovYMotion[0] >= 0.0) &&                           // begin-FovY
       (mFovYMotion[1] >= 0.0) &&                           // end-FovY
       (mFovYMotion[2] > 0.0) && (timer <= mFovYMotion[2])) // duration
   {
      double blend = timer / mFovYMotion[2];
      mFovY        = blend * (mFovYMotion[1] - mFovYMotion[0]) + mFovYMotion[0];
   }
}

double VaCameraMotion::WrapAngleNeg180To180(double aAngle) const
{
   aAngle = fmod(aAngle, 360.0);
   if (aAngle > 180)
   {
      aAngle -= 360.0;
   }
   if (aAngle < -180)
   {
      aAngle += 360.0;
   }
   return aAngle;
}

double VaCameraMotion::WrapAngle0To360(double aAngle) const
{
   aAngle = fmod(aAngle, 360.0);
   if (aAngle < 0)
   {
      aAngle += 360.0;
   }
   return aAngle;
}

} // namespace vespa
