// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "AeroLandingGear.hpp"

#include "P6DofUtils.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

namespace Designer
{

AeroLandingGear::AeroLandingGear(const std::string& aGeneratedFromName, Vehicle* aVehicle)
   : AeroObject(aGeneratedFromName, aVehicle)
{
   // Overwrite the AeroObject type
   mAeroTypeString.assign("AeroLandingGear");
}

void AeroLandingGear::SetNormalizedControlValue(double aControlValue)
{
   mCurrentAngle_deg = mMaxAngle_deg * aControlValue;

   if (mCurrentAngle_deg < 0.0)
   {
      mCurrentAngle_deg = 0.0;
   }
   else if (mCurrentAngle_deg > mMaxAngle_deg)
   {
      mCurrentAngle_deg = mMaxAngle_deg;
   }
}

void AeroLandingGear::SetControlAngle_deg(double aControlAngle_deg)
{
   mCurrentAngle_deg = aControlAngle_deg;

   if (mCurrentAngle_deg < 0.0)
   {
      mCurrentAngle_deg = 0.0;
   }
   else if (mCurrentAngle_deg > mMaxAngle_deg)
   {
      mCurrentAngle_deg = mMaxAngle_deg;
   }
}

void AeroLandingGear::CalcForcesAndMoments(double    aDynamicPressure_lbssqft,
                                           double    aMach,
                                           UtVec3dX& aLiftVector_lbs,
                                           UtVec3dX& aDragVector_lbs,
                                           UtVec3dX& aSideVector_lbs,
                                           UtVec3dX& aMoment_ftlbs,
                                           UtVec3dX& aForcePoint_ft)
{
   // No lift, side force, or moment
   aLiftVector_lbs.Set(0.0, 0.0, 0.0);
   aSideVector_lbs.Set(0.0, 0.0, 0.0);
   aMoment_ftlbs.Set(0.0, 0.0, 0.0);

   // Check for retracted gear
   if (GearIsRetracted())
   {
      // Gear is retracted -- no drag
      aDragVector_lbs.Set(0.0, 0.0, 0.0);
      aForcePoint_ft.Set(0.0, 0.0, 0.0);
      return;
   }

   // Calculate the current aero force point, based on Mach
   // This also considers the current angle of the landing gear
   aForcePoint_ft = CalcAeroForcePoint_ft(aMach);

   // Calculate the current alpha/beta
   double alpha_deg = 0.0;
   double beta_deg  = 0.0;
   CalcAlphaBeta(alpha_deg, beta_deg);

   // Verify cross sectional area
   if (mCrossSectionalArea_sqft <= 0.0)
   {
      ut::log::error() << "Non-positive cross sectional area in AeroLandingGear::CalcForcesAndMoments().";
   }

   double alpha_rad = alpha_deg * UtMath::cRAD_PER_DEG;
   double beta_rad  = beta_deg * UtMath::cRAD_PER_DEG;

   // Add in the additional dynamic pressure
   aDynamicPressure_lbssqft += mAdditionalDynamicPressure_psf;

   CalcDragVector(alpha_rad, beta_rad, aDynamicPressure_lbssqft, aMach, aDragVector_lbs);
}

UtVec3dX AeroLandingGear::CalcAeroForcePoint_ft(double aMach) const
{
   return CalcAngleBasedAeroForcePoint_ft(aMach, mCurrentAngle_deg);
}

UtVec3dX AeroLandingGear::CalcAngleBasedAeroForcePoint_ft(double, // aMach,
                                                          double aAngle_deg) const
{
   double angle_rad = aAngle_deg * UtMath::cRAD_PER_DEG;
   double lengthMultiplier = sin(angle_rad);

   double areaStrut_ft2 = mUncompressedLength_ft * mStrutDiam_ft;
   double areaTire_ft2 = mTireDiam_ft * mTireWidth_ft;

   double strutCenterLength_ft = mUncompressedLength_ft * 0.5;
   double tireCenterLength_ft = mUncompressedLength_ft - (mTireDiam_ft * 0.5);

   double combinedLength_ft = lengthMultiplier * ((areaStrut_ft2 * strutCenterLength_ft) + (areaTire_ft2 * tireCenterLength_ft)) / (areaStrut_ft2 + areaTire_ft2);

   UtVec3dX cpPoint_ft(0.0, 0.0, combinedLength_ft);
   cpPoint_ft += mRefPoint_ft;

   return cpPoint_ft;
}

void AeroLandingGear::CalcDragVector(double    aAlpha_rad,
                                     double    aBeta_rad,
                                     double    aDynamicPressure_lbssqft,
                                     double    aMach,
                                     UtVec3dX& aDragVector_lbs)
{
   double CD = CalcDragCoefficient(aAlpha_rad, aBeta_rad, aMach);
   double drag_lbs = aDynamicPressure_lbssqft * mCrossSectionalArea_sqft * CD;
   aDragVector_lbs = drag_lbs * mDragForceUnitVector;
}

double AeroLandingGear::CalcDragCoefficient(double aAlpha_rad, double aBeta_rad, double aMach)
{
   double totalAngle_rad = sqrt(aAlpha_rad * aAlpha_rad + aBeta_rad * aBeta_rad);
   return 0.3 + std::abs(totalAngle_rad);
}

void AeroLandingGear::SetData(const std::string& aName,
                              UtVec3dX           aRefPoint_ft,
                              double             aUncompressedLength_ft,
                              double             aStrutDiam_ft,
                              double             aTireDiam_ft,
                              double             aTireWidth_ft,
                              double             aMaxAngle_deg,
                              double             aCurrentAngle_deg,
                              double             aCdMultiplier,
                              double             aSurfaceArea_ft2,
                              double             aMaxRetractionRate_dps,
                              double             aMaxExtensionRate_dps)
{
   mName = aName;
   mRefPoint_ft = aRefPoint_ft;
   mUncompressedLength_ft = aUncompressedLength_ft;
   mStrutDiam_ft = aStrutDiam_ft;
   mTireDiam_ft = aTireDiam_ft;
   mTireWidth_ft = aTireWidth_ft;
   mMaxAngle_deg = aMaxAngle_deg;
   mCurrentAngle_deg = aCurrentAngle_deg;
   mCdMultiplier = aCdMultiplier;
   mMaxRetractionRate_dps = aMaxRetractionRate_dps;
   mMaxExtensionRate_dps = aMaxExtensionRate_dps;

   double strutArea_ft2 = mUncompressedLength_ft * mStrutDiam_ft;
   double tireArea_ft2 = mTireDiam_ft * mTireWidth_ft;

   mCrossSectionalArea_sqft = strutArea_ft2 + tireArea_ft2;

   mSurfaceArea_ft2 = aSurfaceArea_ft2;

   // Determine the aero center. Only need to consider z.
   double strutAreaTimesDistBelowRefPt = strutArea_ft2 * mUncompressedLength_ft * 0.5;
   double tireAreaTimesDistBelowRefPt = tireArea_ft2 * (mUncompressedLength_ft - (mTireDiam_ft * 0.5));
   double totalAreaTimesDistBelowRefPt = strutAreaTimesDistBelowRefPt + tireAreaTimesDistBelowRefPt;
   double distBelowRefPt = totalAreaTimesDistBelowRefPt / (strutArea_ft2 + tireArea_ft2);

   double z = mRefPoint_ft.Z() + distBelowRefPt;
   mExtendedAeroCenter.Set(0.0, 0.0, z);

   mDCM.Set(0.0, 0.0, 0.0);
}

bool AeroLandingGear::GearIsRetracted()
{
   if (mCurrentAngle_deg <= cEpsilon)
   {
      return true;
   }
   return false;
}

void Designer::AeroLandingGear::CalcGeometryDCM()
{
   mGeometryDCM = UtDCM(cAxesTransformDCM);
}

} // namespace Designer
