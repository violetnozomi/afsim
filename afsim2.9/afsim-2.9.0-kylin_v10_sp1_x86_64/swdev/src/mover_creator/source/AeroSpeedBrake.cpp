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

#include "AeroSpeedBrake.hpp"

#include "Vehicle.hpp"
#include "P6DofUtils.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

namespace Designer
{

AeroSpeedBrake::AeroSpeedBrake(const std::string& aGeneratedFromName, Vehicle* aVehicle)
   : AeroObject(aGeneratedFromName, aVehicle)
{
   // Overwrite the AeroObject type
   mAeroTypeString.assign("AeroSpeedBrake");
}

void AeroSpeedBrake::SetNormalizedControlValue(double aControlValue)
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

void AeroSpeedBrake::SetControlAngle_deg(double aControlAngle_deg)
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

void AeroSpeedBrake::CalcForcesAndMoments(double    aDynamicPressure_lbssqft,
                                          double    aMach,
                                          UtVec3dX& aLiftVector_lbs,
                                          UtVec3dX& aDragVector_lbs,
                                          UtVec3dX& aSideVector_lbs,
                                          UtVec3dX& aMoment_ftlbs,
                                          UtVec3dX& aForcePoint_ft)
{
   // Check for retracted
   if (SpeedBrakeIsRetracted())
   {
      // Retracted -- no F&M
      aLiftVector_lbs.Set(0.0, 0.0, 0.0);
      aSideVector_lbs.Set(0.0, 0.0, 0.0);
      aDragVector_lbs.Set(0.0, 0.0, 0.0);
      aMoment_ftlbs.Set(0.0, 0.0, 0.0);
      aForcePoint_ft.Set(0.0, 0.0, 0.0);
      return;
   }

   // Calculate the current aero force point, based on Mach
   // This also considers the current angle of the speed brake
   aForcePoint_ft = CalcAeroForcePoint_ft(aMach);

   // Calculate the current part-local alpha/beta
   double alpha_deg = 0.0;
   double beta_deg  = 0.0;
   CalcAlphaBeta(alpha_deg, beta_deg);

   // Verify cross sectional area
   if (mCrossSectionalArea_sqft <= 0.0)
   {
      ut::log::error() << "Non-positive cross sectional area in AeroSpeedBrake::CalcForcesAndMoments().";
   }

   double alpha_rad = alpha_deg * UtMath::cRAD_PER_DEG;
   double beta_rad  = beta_deg * UtMath::cRAD_PER_DEG;

   // Add in the additional dynamic pressure
   aDynamicPressure_lbssqft += mAdditionalDynamicPressure_psf;

   CalcLiftVector(alpha_rad, beta_rad, aDynamicPressure_lbssqft, aMach, aLiftVector_lbs);
   CalcSideVector(alpha_rad, beta_rad, aDynamicPressure_lbssqft, aMach, aSideVector_lbs);
   CalcDragVector(alpha_rad, beta_rad, aDynamicPressure_lbssqft, aMach, aDragVector_lbs);
   CalcMomentVector(alpha_rad, beta_rad, aDynamicPressure_lbssqft, aMoment_ftlbs);
}

UtVec3dX AeroSpeedBrake::CalcAeroForcePoint_ft(double aMach) const
{
   return CalcAngleBasedAeroForcePoint_ft(aMach, mCurrentAngle_deg);
}

UtVec3dX AeroSpeedBrake::CalcAngleBasedAeroForcePoint_ft(double aMach,
                                                         double aAngle_deg) const
{
   // Pressure center

   double aeroCenter_ft = 0.0;
   double aeroCenterQuarterChord_ft = -mLength_ft * 0.25;
   double aeroCenterHalfChord_ft = -mLength_ft * 0.5;

   // Mach effects logic lifted from AeroSurface
   // This effectively treats our speed brake as a control surface, which may or may not be appropriate
   if (aMach < 0.9)
   {
	   aeroCenter_ft = aeroCenterQuarterChord_ft;
   }
   else if (aMach < 1.9)
   {
	   double fraction = (aMach - 0.9);
	   aeroCenter_ft = aeroCenterQuarterChord_ft;
	   double deltaVec = aeroCenterHalfChord_ft - aeroCenterQuarterChord_ft;
	   deltaVec = deltaVec * fraction;
	   aeroCenter_ft = aeroCenter_ft + deltaVec;
   }
   else
   {
	   aeroCenter_ft = aeroCenterHalfChord_ft;
   }

   // Now, adjust for orientation (roll and deflection angles)
   UtVec3dX aeroCenterVector_ft(aeroCenter_ft, 0.0, 0.0);

   UtDCM deflectionDCM(0.0, aAngle_deg * UtMath::cRAD_PER_DEG, 0.0);
   UtDCM rollDCM(0.0, 0.0, mRoll_rad);
   UtDCM geometryDCM = deflectionDCM * rollDCM;

   aeroCenterVector_ft = geometryDCM.InverseTransform(aeroCenterVector_ft);

   // Finally, add in ref pt offset
   aeroCenterVector_ft += mRefPoint_ft;

   return aeroCenterVector_ft;
}

UtVec3dX AeroSpeedBrake::GetSurfaceNormal(double aAngle_deg)
{
   double angle_rad = aAngle_deg * UtMath::cRAD_PER_DEG;

   UtVec3dX normalVec(mLength_ft * 0.5 * sin(angle_rad), 0.0, -mLength_ft * 0.5 * cos(angle_rad));

   // Now, adjust for orientation (y,p,r)
   UtDCM attitudeDCM(0.0, 0.0, mRoll_rad);
   normalVec = attitudeDCM.InverseTransform(normalVec);

   return normalVec;
}

void AeroSpeedBrake::SetData(const std::string& aName,
                             UtVec3dX           aRefPoint_ft,
                             double             aLength_ft,
                             double             aWidth_ft,
                             double             aRoll_rad,
                             double             aMaxAngle_deg,
                             double             aCurrentAngle_deg,
                             double             aCdMultiplier,
                             double             aSurfaceArea_ft2,
                             double             aMaxRetractionRate_dps,
                             double             aMaxExtensionRate_dps)
{
   mName = aName;
   mRefPoint_ft = aRefPoint_ft;
   mLength_ft = aLength_ft;
   mWidth_ft = aWidth_ft;
   mRoll_rad = aRoll_rad;
   mMaxAngle_deg = aMaxAngle_deg;
   mCurrentAngle_deg = aCurrentAngle_deg;
   mCdMultiplier = aCdMultiplier;
   mCrossSectionalArea_sqft = mLength_ft * mWidth_ft;
   mSurfaceArea_ft2 = aSurfaceArea_ft2;
   mMaxRetractionRate_dps = aMaxRetractionRate_dps;
   mMaxExtensionRate_dps = aMaxExtensionRate_dps;

   mDCM.Set(0.0, 0.0, 0.0);
}

void AeroSpeedBrake::CalcAeroCoeffDelta(const std::string& aControlSurfaceName,
                                        double            aControlAngle_deg,
                                        double            aAltitude_ft,
                                        double            aSpeed_fps,
                                        double            aMach,
                                        double            aKTAS,
                                        double            aKCAS,
                                        double            aDynamicPressure_psf,
                                        double            aDensity_slugs_ft3,
                                        double            aVehicleAlpha_deg,
                                        double            aVehicleBeta_deg,
                                        double            aVehicleYawRate_dps,
                                        double            aVehiclePitchRate_dps,
                                        double            aVehicleRollRate_dps,
                                        double&           aDeltaCL,
                                        double&           aDeltaCd,
                                        double&           aDeltaCY,
                                        double&           aDeltaCm,
                                        double&           aDeltaCn,
                                        double&           aDeltaCl)
{
   double lift_lbs0 = 0.0;
   double drag_lbs0 = 0.0;
   double side_lbs0 = 0.0;
   UtVec3dX moment_ftlbs0 = UtVec3dX();

   double lift_lbs = 0.0;
   double drag_lbs = 0.0;
   double side_lbs = 0.0;
   UtVec3dX moment_ftlbs = UtVec3dX();

   mVehicle->SetVehicleAlphaBeta(aVehicleAlpha_deg, aVehicleBeta_deg);

   // Set the vehicle rates
   mVehicle->SetVehicleYawPitchRollRates(aVehicleYawRate_dps, aVehiclePitchRate_dps, aVehicleRollRate_dps);

   // Set vehicle speed-related data
   mVehicle->SetVehicleSpeedParameters(aSpeed_fps, aMach, aKTAS, aKCAS, aDynamicPressure_psf, aDensity_slugs_ft3);

   SetControlAngle_deg(0.0);
   CalculateAeroAttitudeVectors();
   CalcAeroData(aAltitude_ft, aSpeed_fps, aMach, aKTAS, aKCAS, aDynamicPressure_psf, aDensity_slugs_ft3,
                aVehicleAlpha_deg, aVehicleBeta_deg, aVehicleYawRate_dps, aVehiclePitchRate_dps, aVehicleRollRate_dps,
                lift_lbs0, drag_lbs0, side_lbs0, moment_ftlbs0);

   double CL0 = lift_lbs0 / aDynamicPressure_psf;
   double Cd0 = drag_lbs0 / aDynamicPressure_psf;
   double CY0 = side_lbs0 / aDynamicPressure_psf;
   double Cl0 = moment_ftlbs0.X() / aDynamicPressure_psf;
   double Cm0 = moment_ftlbs0.Y() / aDynamicPressure_psf;
   double Cn0 = moment_ftlbs0.Z() / aDynamicPressure_psf;

   if (mVehicle->GetAero()->UseRefArea())
   {
      double refArea = mVehicle->GetAero()->GetVehicleRefArea_ft2();

      if (refArea > 0)
      {
         CL0 /= refArea;
         Cd0 /= refArea;
         CY0 /= refArea;
         Cm0 /= refArea;
         Cn0 /= refArea;
         Cl0 /= refArea;
      }
      else
      {
         MoverCreatorMainWindow().CreateErrorBox("Vehicle has a zero reference area!");
      }

   }
   else
   {
      double wingArea = mVehicle->GetAero()->GetVehicleWingArea_ft2();
      double wingChord = mVehicle->GetAero()->GetVehicleWingChord_ft();
      double wingSpan = mVehicle->GetAero()->GetVehicleWingSpan_ft();

      if (wingArea > 0 && wingChord > 0 && wingSpan > 0)
      {
         CL0 /= wingArea;
         Cd0 /= wingArea;
         CY0 /= wingArea;
         Cm0 /= (wingArea * wingChord);
         Cn0 /= (wingArea * wingSpan);
         Cl0 /= (wingArea * wingSpan);

      }
      else
      {
         MoverCreatorMainWindow().CreateErrorBox("Vehicle has a zero wing area!");
      }
   }

   SetControlAngle_deg(aControlAngle_deg);
   CalculateAeroAttitudeVectors();
   CalcAeroData(aAltitude_ft, aSpeed_fps, aMach, aKTAS, aKCAS, aDynamicPressure_psf, aDensity_slugs_ft3,
                aVehicleAlpha_deg, aVehicleBeta_deg, aVehicleYawRate_dps, aVehiclePitchRate_dps, aVehicleRollRate_dps,
                lift_lbs, drag_lbs, side_lbs, moment_ftlbs);

   double CL = lift_lbs / aDynamicPressure_psf;
   double Cd = drag_lbs / aDynamicPressure_psf;
   double CY = side_lbs / aDynamicPressure_psf;
   double Cl = moment_ftlbs.X() / aDynamicPressure_psf;
   double Cm = moment_ftlbs.Y() / aDynamicPressure_psf;
   double Cn = moment_ftlbs.Z() / aDynamicPressure_psf;

   if (mVehicle->GetAero()->UseRefArea())
   {
      double refArea = mVehicle->GetAero()->GetVehicleRefArea_ft2();

      if (refArea > 0)
      {
         CL /= refArea;
         Cd /= refArea;
         CY /= refArea;
         Cm /= refArea;
         Cn /= refArea;
         Cl /= refArea;
      }
      else
      {
         MoverCreatorMainWindow().CreateErrorBox("Vehicle has a zero reference area!");
      }

   }
   else
   {
      double wingArea = mVehicle->GetAero()->GetVehicleWingArea_ft2();
      double wingChord = mVehicle->GetAero()->GetVehicleWingChord_ft();
      double wingSpan = mVehicle->GetAero()->GetVehicleWingSpan_ft();

      if (wingArea > 0 && wingChord > 0 && wingSpan > 0)
      {
         CL /= wingArea;
         Cd /= wingArea;
         CY /= wingArea;
         Cm /= (wingArea * wingChord);
         Cn /= (wingArea * wingSpan);
         Cl /= (wingArea * wingSpan);

      }
      else
      {
         MoverCreatorMainWindow().CreateErrorBox("Vehicle has a zero wing area!");
      }
   }

   aDeltaCL = CL - CL0;
   aDeltaCd = Cd - Cd0;
   aDeltaCY = CY - CY0;
   aDeltaCm = Cm - Cm0;
   aDeltaCn = Cn - Cn0;
   aDeltaCl = Cl - Cl0;
}

void AeroSpeedBrake::CalcMomentVector(double    aAlpha_rad,
                                      double    aBeta_rad,
                                      double    aDynamicPressure_lbssqft,
                                      UtVec3dX& aMoment_ftlbs)
{
	double Cm = CalcZeroLiftPitchingMomentCoefficient(aAlpha_rad, aBeta_rad);
	double moment_ftlbs = aDynamicPressure_lbssqft * mCrossSectionalArea_sqft * mLength_ft * Cm;

	// Assume that pitching moment is about the side force vector
	aMoment_ftlbs = moment_ftlbs * mSideForceUnitVector;
}

void AeroSpeedBrake::CalcLiftVector(double    aAlpha_rad,
                                    double    aBeta_rad,
                                    double    aDynamicPressure_lbssqft,
                                    double    aMach,
                                    UtVec3dX& aLiftVector_lbs)
{
   // No effect if retracted
   if (mCurrentAngle_deg <= 0.0)
   {
      aLiftVector_lbs.Set(0.0, 0.0, 0.0);
   }

   double CL = CalcLiftCoefficient(aAlpha_rad, aBeta_rad, aMach);
   double lift_lbs = aDynamicPressure_lbssqft * mCrossSectionalArea_sqft * CL;
   aLiftVector_lbs = lift_lbs * mLiftForceUnitVector;
}

void AeroSpeedBrake::CalcSideVector(double,
                                    double,
                                    double,
                                    double,
                                    UtVec3dX& aSideVector_lbs)
{
   // Side force is regarded as negligible for flat plate speed brakes
   aSideVector_lbs.Set(0.0, 0.0, 0.0);
}

void AeroSpeedBrake::CalcDragVector(double    aAlpha_rad,
                                    double    aBeta_rad,
                                    double    aDynamicPressure_lbssqft,
                                    double    aMach,
                                    UtVec3dX& aDragVector_lbs)
{
   // No effect if retracted
   if (mCurrentAngle_deg <= 0.0)
   {
      aDragVector_lbs.Set(0.0, 0.0, 0.0);
   }

   double CD = CalcDragCoefficient(aAlpha_rad, aBeta_rad, aMach);
   double drag_lbs = aDynamicPressure_lbssqft * mCrossSectionalArea_sqft * CD;
   aDragVector_lbs = drag_lbs * mDragForceUnitVector;
}

double AeroSpeedBrake::CalcZeroLiftPitchingMomentCoefficient(double aAlpha_rad, double aBeta_rad)
{
   // We'll assume stall happens early - around 10 degrees
   // We also assume neutral stability until stall
   // After that point, we have a quarter-chord moment coefficient of about -0.075
   // More realistically, this should be a continuous function of aspect ratio and Mach

   const double stallAngle_rad = 10.0 * UtMath::cRAD_PER_DEG;
	double postStallMomentCoefficient = -0.075;

   if (std::abs(aAlpha_rad) > stallAngle_rad)
   {
      return UtMath::Sign(aAlpha_rad) * postStallMomentCoefficient;
   }
   else
   {
      return 0.0;
   }
}

double AeroSpeedBrake::CalcLiftCoefficient(double aAlpha_rad, double aBeta_rad, double aMach)
{
   // Speed brake lift here is based on thin airfoil theory.

   // We'll assume stall happens relatively early - around 10 degrees -
   // but that it doesn't kill the lift. More realistically, we should see
   // a drop off in lift coefficient as we approach 45 degrees of alpha.

   const double stallAngle_rad = 10.0 * UtMath::cRAD_PER_DEG;
   const double transonicMachBegin = 0.8;
   const double transonicMachEnd = 1.2;

   double aspectRatio = mWidth_ft / mLength_ft;

   double liftCoefficientSlope_perRad = 0.0;
   if (aMach < transonicMachBegin)
   {
      // Subsonic lift curve corrected by Prandtl-Glauert
      // (Nicolai & Carichner, 2010)
      liftCoefficientSlope_perRad = UtMath::cPI_OVER_2 * aspectRatio / sqrt(1.0 - aMach * aMach);
   }
   else if (aMach > transonicMachEnd)
   {
      // Supersonic lift curve from supersonic thin airfoil theory
      // (Nicolai & Carichner, 2010)
      liftCoefficientSlope_perRad = 4.0 / sqrt(aMach * aMach - 1.0);
   }
   else
   {
      // Linear interpolation between subsonic and supersonic
      double liftCoefficientSlopeSubsonic_perRad = UtMath::cPI_OVER_2 * aspectRatio / sqrt(1.0 - transonicMachBegin * transonicMachBegin);
      double liftCoefficientSlopeSupersonic_perRad = 4.0 / sqrt(transonicMachEnd * transonicMachEnd - 1.0);

      liftCoefficientSlope_perRad = liftCoefficientSlopeSubsonic_perRad + (liftCoefficientSlopeSupersonic_perRad - liftCoefficientSlopeSubsonic_perRad) / (transonicMachEnd - transonicMachBegin) * (aMach - transonicMachBegin);
   }

   // It is assumed that the speed brake is affixed to another surface,
   // which we assume eliminates the suction/adverse pressure.
   // We assume that, ordinarily, lift (positive or negative) is generated
   // by equal parts favorable and adverse pressure, so eliminating the
   // adverse pressure cuts the lift in half;
   liftCoefficientSlope_perRad *= 0.5;

   double liftCoefficient = liftCoefficientSlope_perRad * UtMath::Limit(aAlpha_rad, stallAngle_rad);

   double betaFactor = cos(std::abs(aBeta_rad));
   return liftCoefficient * betaFactor;
}

double AeroSpeedBrake::CalcDragCoefficient(double aAlpha_rad, double aBeta_rad, double aMach)
{
   // Drag as a function of beta may depend on the construction of the drag device.
   // Flat plates raised by thin spars will see their drag reduced by increases in beta.
   // For now, we'll assume a simple model of sinusoidal oscillation between 0 and max drag.

   // Maximum subsonic drag coefficient is taken as 1.6, per:
   // Theory and Practice of Aircraft Performance, 2016, Kundu et al, via
   // https://en.wikipedia.org/wiki/Air_brake_(aeronautics)

   const double dragCoefficientSubsonicMaximum = 1.6;

   double dragCoefficientSubsonic = dragCoefficientSubsonicMaximum * 0.5 * (1.0 - cos(2.0 * aAlpha_rad));

   // Wave drag will be phased in through the transonic region, similar to lift
   const double transonicMachBegin = 0.8;
   const double transonicMachEnd = 1.2;

   double waveDragCoefficient = 0.0;
   if (aMach < transonicMachBegin)
   {
      // No wave drag to account for
   }
   else if (aMach > transonicMachEnd)
   {
      // Wave drag from supersonic thin airfoil theory, simplified further for a flat plate
      // (Nicolai & Carichner, 2010)
      double liftCoefficient = 4.0 * aAlpha_rad / sqrt(aMach * aMach - 1.0);
      waveDragCoefficient = std::abs(liftCoefficient) * aAlpha_rad * aAlpha_rad;
   }
   else
   {
      // Linear interpolation between subsonic and supersonic
      double liftCoefficientSupersonic = 4.0 * aAlpha_rad / sqrt(transonicMachEnd * transonicMachEnd - 1.0);
      double waveDragCoefficientSupersonic = std::abs(liftCoefficientSupersonic) * aAlpha_rad * aAlpha_rad;

      waveDragCoefficient = waveDragCoefficientSupersonic / (transonicMachEnd - transonicMachBegin) * (aMach - transonicMachBegin);
   }

   double dragCoefficient = dragCoefficientSubsonic + waveDragCoefficient;

   double betaFactor = cos(std::abs(aBeta_rad));
   return dragCoefficient * betaFactor;
}

bool AeroSpeedBrake::SpeedBrakeIsRetracted()
{
   if (mCurrentAngle_deg <= cEpsilon)
   {
      return true;
   }
   return false;
}

void Designer::AeroSpeedBrake::CalcGeometryDCM()
{
   UtDCM axesDCM(cAxesTransformDCM);
   UtDCM rollDCM(0.0, 0.0, mRoll_rad);

   mGeometryDCM = rollDCM * axesDCM;
}

void Designer::AeroSpeedBrake::CalcAlphaBeta(double& aAlpha_deg, double& aBeta_deg)
{
   UtDCM axesDCM(cAxesTransformDCM);
   UtDCM incidenceDCM(0.0, -mCurrentAngle_deg * UtMath::cRAD_PER_DEG, -mRoll_rad);

   UtDCM geometryDCM = incidenceDCM * axesDCM;

   UtDCM totalDCM = axesDCM.Transpose() * mGeometryDCM.Transpose() * mAlphaBetaDCM * geometryDCM * axesDCM;
   UtVec3dX velocity = totalDCM.GetCol(0);

   P6DofUtils::AlphaBetaFromBodyRelVel(velocity, aAlpha_deg, aBeta_deg);
}

} // namespace Designer
