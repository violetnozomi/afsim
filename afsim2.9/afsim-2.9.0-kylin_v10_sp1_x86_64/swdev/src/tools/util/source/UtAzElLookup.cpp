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

#include "UtAzElLookup.hpp"

#include "UtAzElTable.hpp"
#include "UtException.hpp"

float UtAzElLookup::Lookup(float aAzimuth, float aElevation)
{
   float azimuth = aAzimuth;
   if (mIsSymmetric && (aAzimuth < 0.0F))
   {
      azimuth = -azimuth;
   }

   if (mEqualInterval)
   {
      if (mInterpolate)
      {
         mAzLookupLE.Lookup(mTablePtr->mAzValuesE, azimuth);
         mElLookupLE.Lookup(mTablePtr->mElValuesE, aElevation);
         return TblEvaluate(mTablePtr->mDataValues, mAzLookupLE, mElLookupLE, mInterpolationType);
      }
      else
      {
         mAzLookupE.Lookup(mTablePtr->mAzValuesE, azimuth);
         mElLookupE.Lookup(mTablePtr->mElValuesE, aElevation);
         return TblEvaluate(mTablePtr->mDataValues, mAzLookupE, mElLookupE);
      }
   }
   else
   {
      if (mInterpolate)
      {
         mAzLookupLU.Lookup(mTablePtr->mAzValues, azimuth);
         mElLookupLU.Lookup(mTablePtr->mElValues, aElevation);
         return TblEvaluate(mTablePtr->mDataValues, mAzLookupLU, mElLookupLU, mInterpolationType);
      }
      else
      {
         mAzLookupU.Lookup(mTablePtr->mAzValues, azimuth);
         mElLookupU.Lookup(mTablePtr->mElValues, aElevation);
         return TblEvaluate(mTablePtr->mDataValues, mAzLookupU, mElLookupU);
      }
   }
}

void UtAzElLookup::SetTable(UtAzElTable* aTablePtr)
{
   mTablePtr = aTablePtr;
   mAzLookupU.Reset();
   mElLookupU.Reset();
   mAzLookupE.Reset();
   mElLookupE.Reset();
   mAzLookupLU.Reset();
   mElLookupLU.Reset();
   mAzLookupLE.Reset();
   mElLookupLE.Reset();

   // If the first azimuth value is >= 0 then we assume the table is symmetric.

   mIsSymmetric = false;
   if (mTablePtr->GetContext().IsEqualInterval())
   {
      if (mTablePtr->mAzValuesE.Get(0) >= 0.0F)
      {
         mIsSymmetric = true;
      }
   }
   else
   {
      if (mTablePtr->mAzValues.Get(0) >= 0.0F)
      {
         mIsSymmetric = true;
      }
   }
}

void UtAzElLookup::GetAzimuthSlice(std::map<float, float>& aSlice, float aElevation)
{
   aSlice.clear();
   if (mTablePtr->GetContext().IsEqualInterval())
   {
      if (mInterpolate)
      {
         mElLookupLE.Lookup(mTablePtr->mElValuesE, aElevation);

         for (size_t i = 0; i < mTablePtr->mAzValuesE.GetSize(); ++i)
         {
            float az = mTablePtr->mAzValuesE.Get(i);
            mAzLookupLE.Lookup(mTablePtr->mAzValuesE, az);
            aSlice[az] = TblEvaluate(mTablePtr->mDataValues, mAzLookupLE, mElLookupLE, mInterpolationType);
         }
      }
      else
      {
         mElLookupLE.Lookup(mTablePtr->mElValuesE, aElevation);

         for (size_t i = 0; i < mTablePtr->mAzValuesE.GetSize(); ++i)
         {
            float az = mTablePtr->mAzValuesE.Get(i);
            mAzLookupE.Lookup(mTablePtr->mAzValuesE, az);
            aSlice[az] = TblEvaluate(mTablePtr->mDataValues, mAzLookupE, mElLookupE);
         }
      }
   }
   else
   {
      if (mInterpolate)
      {
         mElLookupLU.Lookup(mTablePtr->mElValues, aElevation);

         for (size_t i = 0; i < mTablePtr->mAzValues.GetSize(); ++i)
         {
            float az = mTablePtr->mAzValues.Get(i);
            mAzLookupLU.Lookup(mTablePtr->mAzValues, az);
            aSlice[az] = TblEvaluate(mTablePtr->mDataValues, mAzLookupLU, mElLookupLU, mInterpolationType);
         }
      }
      else
      {
         mElLookupU.Lookup(mTablePtr->mElValues, aElevation);

         for (size_t i = 0; i < mTablePtr->mAzValues.GetSize(); ++i)
         {
            float az = mTablePtr->mAzValues.Get(i);
            mAzLookupU.Lookup(mTablePtr->mAzValues, az);
            aSlice[az] = TblEvaluate(mTablePtr->mDataValues, mAzLookupU, mElLookupU);
         }
      }
   }
}
