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

#include "VaBoundingBox.hpp"

#include <float.h>

#include "UtMath.hpp"

namespace vespa
{
VaBoundingBox::VaBoundingBox()
   : mMinLLA(DBL_MAX, DBL_MAX, DBL_MAX)
   , mMaxLLA(-DBL_MAX, -DBL_MAX, -DBL_MAX)
   , mMeanLLA()
   , mCenterLLA()
   , mMinLLASet(false)
   , mMaxLLASet(false)
{
}

VaBoundingBox::VaBoundingBox(const UtVec3d& aMinLLA, const UtVec3d& aMaxLLA)
   : mMinLLA(aMinLLA)
   , mMaxLLA(aMaxLLA)
   , mMeanLLA()
   , mCenterLLA()
   , mMinLLASet(true)
   , mMaxLLASet(true)
{
   mCenterLLA[0] = 0.0;
   if ((mMaxLLA[0] != -DBL_MAX) && (mMinLLA[0] != DBL_MAX))
   {
      mCenterLLA[0] = 0.5 * (mMaxLLA[0] + mMinLLA[0]);
   }
   mCenterLLA[1] = 0.0;
   if ((mMaxLLA[1] != -DBL_MAX) && (mMinLLA[1] != DBL_MAX))
   {
      // If we need to consider
      if ((fabs(mMaxLLA[1]) > 90.0) && (fabs(mMinLLA[1]) > 90.0))
      {
         double minLon = (mMinLLA[1] < 0.0) ? mMinLLA[1] + 360.0 : mMinLLA[1];
         double maxLon = (mMaxLLA[1] < 0.0) ? mMaxLLA[1] + 360.0 : mMaxLLA[1];
         mCenterLLA[1] = 0.5 * (maxLon + minLon);
         mCenterLLA[1] = UtMath::NormalizeAngleMinus180_180(mCenterLLA[1]);
      }
      else
      {
         mCenterLLA[1] = 0.5 * (mMaxLLA[1] + mMinLLA[1]);
      }
   }
   mCenterLLA[2] = CalculateMeanAltitude();
   mMeanLLA      = mCenterLLA;
}

VaBoundingBox::VaBoundingBox(const UtVec3d& aMinLLA, const UtVec3d& aMaxLLA, const UtVec3d& aMeanLLA)
   : mMinLLA(aMinLLA)
   , mMaxLLA(aMaxLLA)
   , mMeanLLA(aMeanLLA)
   , mCenterLLA()
   , mMinLLASet(true)
   , mMaxLLASet(true)
{
   mCenterLLA[0] = 0.0;
   if ((mMaxLLA[0] != -DBL_MAX) && (mMinLLA[0] != DBL_MAX))
   {
      mCenterLLA[0] = 0.5 * (mMaxLLA[0] + mMinLLA[0]);
   }
   mCenterLLA[1] = 0.0;
   if ((mMaxLLA[1] != -DBL_MAX) && (mMinLLA[1] != DBL_MAX))
   {
      // If we need to consider
      if ((fabs(mMaxLLA[1]) > 90.0) && (fabs(mMinLLA[1]) > 90.0))
      {
         double minLon = (mMinLLA[1] < 0.0) ? mMinLLA[1] + 360.0 : mMinLLA[1];
         double maxLon = (mMaxLLA[1] < 0.0) ? mMaxLLA[1] + 360.0 : mMaxLLA[1];
         mCenterLLA[1] = 0.5 * (maxLon + minLon);
         mCenterLLA[1] = UtMath::NormalizeAngleMinus180_180(mCenterLLA[1]);
      }
      else
      {
         mCenterLLA[1] = 0.5 * (mMaxLLA[1] + mMinLLA[1]);
      }
   }
   mCenterLLA[2] = CalculateMeanAltitude();
}

//! Finalize makes sure that no values still contain DBL_MAX values.
void VaBoundingBox::Finalize()
{
   for (int i = 0; i < 3; ++i)
   {
      if (mMaxLLA[i] == -DBL_MAX && mMinLLA[i] != DBL_MAX)
      {
         mMaxLLA[i] = mMinLLA[i];
      }
      else if (mMaxLLA[i] != -DBL_MAX && mMinLLA[i] == DBL_MAX)
      {
         mMinLLA[i] = mMaxLLA[i];
      }
      else if (mMaxLLA[i] == -DBL_MAX && mMinLLA[i] == DBL_MAX)
      {
         mMaxLLA[i] = 0.0;
         mMinLLA[i] = 0.0;
      }
   }
}

void VaBoundingBox::Grow(const VaBoundingBox& aBoundingBox)
{
   if (aBoundingBox.mMinLLA[0] < mMinLLA[0])
      mMinLLA[0] = aBoundingBox.mMinLLA[0];
   if (aBoundingBox.mMinLLA[1] < mMinLLA[1])
      mMinLLA[1] = aBoundingBox.mMinLLA[1];
   if (aBoundingBox.mMinLLA[2] < mMinLLA[2])
      mMinLLA[2] = aBoundingBox.mMinLLA[2];

   if (aBoundingBox.mMaxLLA[0] > mMaxLLA[0])
      mMaxLLA[0] = aBoundingBox.mMaxLLA[0];
   if (aBoundingBox.mMaxLLA[1] > mMaxLLA[1])
      mMaxLLA[1] = aBoundingBox.mMaxLLA[1];
   if (aBoundingBox.mMaxLLA[2] > mMaxLLA[2])
      mMaxLLA[2] = aBoundingBox.mMaxLLA[2];

   mCenterLLA[0] = 0.0;
   if ((mMaxLLA[0] != -DBL_MAX) && (mMinLLA[0] != DBL_MAX))
   {
      mCenterLLA[0] = 0.5 * (mMaxLLA[0] + mMinLLA[0]);
   }
   mCenterLLA[1] = 0.0;
   if ((mMaxLLA[1] != -DBL_MAX) && (mMinLLA[1] != DBL_MAX))
   {
      // If we need to consider
      if ((fabs(mMaxLLA[1]) > 90.0) && (fabs(mMinLLA[1]) > 90.0))
      {
         double minLon = (mMinLLA[1] < 0.0) ? mMinLLA[1] + 360.0 : mMinLLA[1];
         double maxLon = (mMaxLLA[1] < 0.0) ? mMaxLLA[1] + 360.0 : mMaxLLA[1];
         mCenterLLA[1] = 0.5 * (maxLon + minLon);
         mCenterLLA[1] = UtMath::NormalizeAngleMinus180_180(mCenterLLA[1]);
      }
      else
      {
         mCenterLLA[1] = 0.5 * (mMaxLLA[1] + mMinLLA[1]);
      }
   }
   mCenterLLA[2] = CalculateMeanAltitude();
   mMeanLLA      = mCenterLLA;
}

double VaBoundingBox::CalculateMeanAltitude() const
{
   double meanAltitude = 0.0;
   if (mMaxLLA[2] == -DBL_MAX && mMinLLA[2] != DBL_MAX)
   {
      meanAltitude = mMinLLA[2];
   }
   else if (mMaxLLA[2] != -DBL_MAX && mMinLLA[2] == DBL_MAX)
   {
      meanAltitude = mMaxLLA[2];
   }
   else if (mMaxLLA[2] != -DBL_MAX && mMinLLA[2] != DBL_MAX)
   {
      meanAltitude = 0.5 * (mMaxLLA[2] + mMinLLA[2]);
   }
   return meanAltitude;
}
} // namespace vespa
