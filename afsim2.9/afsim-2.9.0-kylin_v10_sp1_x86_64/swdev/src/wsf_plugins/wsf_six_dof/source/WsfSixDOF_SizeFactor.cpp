// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSixDOF_SizeFactor.hpp"

#include "UtMath.hpp"

void wsf::six_dof::SizeFactor::CalculateSizeFactor(double dT_sec)
{
   if (IsEnabled())
   {
      double fourThirdsPi = 1.3333333 * UtMath::cPI;
      if (mSizeFactorVolumeEnabled)
      {
         mCurrentSizeFactorVolume_m3 += mSizeFactorVolumeRate_m3_perSec * dT_sec;
         double tempR3              = mCurrentSizeFactorVolume_m3 / fourThirdsPi;
         mCurrentSizeFactorRadius_m = pow(tempR3, 0.3333333333);
      }
      else if (mSizeFactorAreaEnabled)
      {
         mCurrentSizeFactorArea_m2 += mSizeFactorAreaRate_m2_perSec * dT_sec;
         double tempR2              = mCurrentSizeFactorArea_m2 / UtMath::cPI;
         mCurrentSizeFactorRadius_m = pow(tempR2, 0.5);
      }
      else // mSizeFactorRadiusEnabled
      {
         mCurrentSizeFactorRadius_m += mSizeFactorRadiusRate_m_perSec * dT_sec;
      }

      // Determine (radius-based) size factor and limit it to min/max
      double sizeFactor = mCurrentSizeFactorRadius_m / mRefSizeFactorRadius_m;
      if (sizeFactor < mSizeFactorMin)
      {
         sizeFactor = mSizeFactorMin;
      }
      if (sizeFactor > mSizeFactorMax)
      {
         sizeFactor = mSizeFactorMax;
      }

      // Calculate the 'current' internal values
      mCurrentSizeFactorRadius_m = sizeFactor * mRefSizeFactorRadius_m;
      double r2                  = mCurrentSizeFactorRadius_m * mCurrentSizeFactorRadius_m;
      double r3                  = mCurrentSizeFactorRadius_m * mCurrentSizeFactorRadius_m * mCurrentSizeFactorRadius_m;
      mCurrentSizeFactorArea_m2  = UtMath::cPI * r2;
      mCurrentSizeFactorVolume_m3 = fourThirdsPi * r3;

      // Calculate the normalized values
      mSizeFactorRadius = mCurrentSizeFactorRadius_m / mRefSizeFactorRadius_m;
      mSizeFactorArea   = mCurrentSizeFactorArea_m2 / mRefSizeFactorArea_m2;
      mSizeFactorVolume = mCurrentSizeFactorVolume_m3 / mRefSizeFactorVolume_m3;
   }
}

void wsf::six_dof::SizeFactor::InitializeValues()
{
   // Setup the internal values for size factor
   double fourThirdsPi = 1.3333333 * UtMath::cPI;
   double r2           = mRefSizeFactorRadius_m * mRefSizeFactorRadius_m;
   double r3           = r2 * mRefSizeFactorRadius_m;

   mRefSizeFactorArea_m2       = UtMath::cPI * r2;
   mRefSizeFactorVolume_m3     = fourThirdsPi * r3;
   mCurrentSizeFactorRadius_m  = mRefSizeFactorRadius_m;
   mCurrentSizeFactorArea_m2   = mRefSizeFactorArea_m2;
   mCurrentSizeFactorVolume_m3 = mRefSizeFactorVolume_m3;
}

double wsf::six_dof::SizeFactor::GetSizeFactorRadius() const
{
   return mSizeFactorRadius;
}

void wsf::six_dof::SizeFactor::SetSizeFactorEnabled(bool aEnabled)
{
   mSizeFactorEnabled = aEnabled;
}

void wsf::six_dof::SizeFactor::SetRefSizeFactorRadius_m(double aValue_m)
{
   mRefSizeFactorRadius_m = aValue_m;
}

void wsf::six_dof::SizeFactor::SetSizeFactorMin(double aValue)
{
   mSizeFactorMin = aValue;
}

void wsf::six_dof::SizeFactor::SetSizeFactorMax(double aValue)
{
   mSizeFactorMax = aValue;
}

void wsf::six_dof::SizeFactor::SetSizeFactorVolumeRate_m3_perSec(double aVolumeRate_m3_perSec)
{
   mSizeFactorVolumeRate_m3_perSec = aVolumeRate_m3_perSec;
   mSizeFactorVolumeEnabled        = true;
   mSizeFactorAreaEnabled          = false;
   mSizeFactorRadiusEnabled        = false;
}

void wsf::six_dof::SizeFactor::SetSizeFactorAreaRate_m2_perSec(double aAreaRate_m2_perSec)
{
   mSizeFactorAreaRate_m2_perSec = aAreaRate_m2_perSec;
   mSizeFactorVolumeEnabled      = false;
   mSizeFactorAreaEnabled        = true;
   mSizeFactorRadiusEnabled      = false;
}

void wsf::six_dof::SizeFactor::SetSizeFactorRadiusRate_m_perSec(double aRadiusRate_m_perSec)
{
   mSizeFactorRadiusRate_m_perSec = aRadiusRate_m_perSec;
   mSizeFactorVolumeEnabled       = false;
   mSizeFactorAreaEnabled         = false;
   mSizeFactorRadiusEnabled       = true;
}
