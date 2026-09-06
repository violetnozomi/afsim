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

#include "AeroDish.hpp"

#include "P6DofUtils.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

namespace Designer
{

AeroDish::AeroDish(const std::string& aGeneratedFromName, Vehicle* aVehicle)
   : AeroObject(aGeneratedFromName, aVehicle)
{
   // Overwrite the AeroDish type
   mAeroTypeString.assign("AeroDish");
}

void AeroDish::CalcForcesAndMoments(double    aDynamicPressure_lbssqft,
                                    double    aMach,
                                    UtVec3dX& aLiftVector_lbs,
                                    UtVec3dX& aDragVector_lbs,
                                    UtVec3dX& aSideVector_lbs,
                                    UtVec3dX& aMoment_ftlbs,
                                    UtVec3dX& aForcePoint_ft)
{
   // Calculate the current alpha/beta
   double alpha_deg = 0.0;
   double beta_deg  = 0.0;
   CalcAlphaBeta(alpha_deg, beta_deg);

   // Calculate the current aero force point, based on Mach
   aForcePoint_ft = CalcAeroForcePoint_ft(aMach);

   // Verify cross sectional area
   if (mCrossSectionalArea_sqft <= 0.0)
   {
      ut::log::error() << "Non-positive cross sectional area in AeroDish::CalcForcesAndMoments().";
   }

   double alpha_rad = alpha_deg * UtMath::cRAD_PER_DEG;
   double beta_rad  = beta_deg * UtMath::cRAD_PER_DEG;

   // Add in the additional dynamic pressure
   aDynamicPressure_lbssqft += mAdditionalDynamicPressure_psf;

   CalcLiftVector(alpha_rad, beta_rad, aDynamicPressure_lbssqft, aLiftVector_lbs);
   CalcDragVector(alpha_rad, beta_rad, aDynamicPressure_lbssqft, aMach, aDragVector_lbs);
   CalcSideForceVector(alpha_rad, beta_rad, aDynamicPressure_lbssqft, aSideVector_lbs);
   CalcMomentVector(alpha_rad, beta_rad, aDynamicPressure_lbssqft, aMoment_ftlbs);
}

UtVec3dX AeroDish::CalcAeroForcePoint_ft(double aMach) const
{
   return mRefPoint_ft;
}

void AeroDish::CalcLiftVector(double    aAlpha_rad,
                              double    aBeta_rad,
                              double    aDynamicPressure_lbssqft,
                              UtVec3dX& aLiftVector_lbs)
{
   double CL = CalcLiftCoefficient(aAlpha_rad, aBeta_rad);
   double lift_lbs = aDynamicPressure_lbssqft * mCrossSectionalArea_sqft * CL;
   aLiftVector_lbs = lift_lbs * mLiftForceUnitVector;
}

void AeroDish::CalcDragVector(double    aAlpha_rad,
                              double    aBeta_rad,
                              double    aDynamicPressure_lbssqft,
                              double    aMach,
                              UtVec3dX& aDragVector_lbs)
{
   double CD = CalcDragCoefficient(aAlpha_rad, aBeta_rad, aMach);
   double drag_lbs = aDynamicPressure_lbssqft * mCrossSectionalArea_sqft * CD;
   aDragVector_lbs = drag_lbs * mDragForceUnitVector;
}

void AeroDish::CalcSideForceVector(double    aAlpha_rad,
                                   double    aBeta_rad,
                                   double    aDynamicPressure_lbssqft,
                                   UtVec3dX& aSideForceVector_lbs)
{
   double CY = CalcSideForceCoefficient(aAlpha_rad, aBeta_rad);
   double sideForce_lbs = aDynamicPressure_lbssqft * mCrossSectionalArea_sqft * CY;
   aSideForceVector_lbs = sideForce_lbs * mSideForceUnitVector;
}

void AeroDish::CalcMomentVector(double    aAlpha_rad,
                                double    aBeta_rad,
                                double    aDynamicPressure_lbssqft,
                                UtVec3dX& aMoment_ftlbs)
{
   double Cm = CalcZeroLiftPitchingMoment(aAlpha_rad, aBeta_rad);
   double moment_ftlbs = aDynamicPressure_lbssqft * mCrossSectionalArea_sqft * Cm;

   // Assume that pitching moment is about the side force vector
   aMoment_ftlbs = moment_ftlbs * mSideForceUnitVector;
}

double AeroDish::CalcLiftCoefficient(double aAlpha_rad, double aBeta_rad)
{
   double betaFactor = cos(std::abs(aBeta_rad));
   return 2.0 * aAlpha_rad * betaFactor;
}

double AeroDish::CalcDragCoefficient(double aAlpha_rad, double aBeta_rad, double aMach)
{
   double totalAngle_rad = sqrt(aAlpha_rad * aAlpha_rad + aBeta_rad * aBeta_rad);
   return 0.3 + std::abs(totalAngle_rad);
}

double AeroDish::CalcSideForceCoefficient(double aAlpha_rad, double aBeta_rad)
{
   double alphaFactor = cos(std::abs(aAlpha_rad));
   return 2.0 * aBeta_rad * alphaFactor;
}

double AeroDish::CalcZeroLiftPitchingMoment(double,  //aAlpha_rad,
                                            double)  //aBeta_rad)
{
   return 0.0;
}

void AeroDish::SetData(const std::string& aName,
                       UtVec3dX           aRefPoint_ft,
                       double             aDiameter_ft,
                       double             aThickness_ft,
                       double             aSurfaceArea_ft2)
{
   mName = aName;
   mRefPoint_ft  = aRefPoint_ft;
   mDiameter_ft  = aDiameter_ft;
   mThickness_ft = aThickness_ft;

   // These are always zero
   mYaw_rad   = 0.0;
   mPitch_rad = 0.0;
   mRoll_rad  = 0.0;

   mDCM.Set(mYaw_rad, mPitch_rad, mRoll_rad);

   mCrossSectionalArea_sqft    = UtMath::cPI * (aThickness_ft * 0.5) * (aDiameter_ft * 0.5);  // Use ellipse area
   mSurfaceArea_ft2            = aSurfaceArea_ft2;
   mFinenessRatio              = mDiameter_ft / mThickness_ft;
   mConeRelativeTotalAngle_deg = 2.0 * atan(aThickness_ft / (aDiameter_ft * 0.5)) * UtMath::cDEG_PER_RAD;
}

void Designer::AeroDish::CalcGeometryDCM()
{
   UtDCM axesDCM(cAxesTransformDCM);
   UtDCM yawDCM(-mYaw_rad, 0, 0);
   UtDCM pitchDCM(0, -mPitch_rad, 0);
   UtDCM rollDCM(0, 0, -mRoll_rad);

   mGeometryDCM = yawDCM * pitchDCM * rollDCM * axesDCM;
}

} // namespace Designer
