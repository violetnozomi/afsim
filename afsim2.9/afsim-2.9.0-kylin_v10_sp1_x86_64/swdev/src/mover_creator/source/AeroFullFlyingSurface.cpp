// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "AeroFullFlyingSurface.hpp"
#include "Vehicle.hpp"

namespace Designer
{

double AeroFullFlyingSurface::CombinedControlAndIncidenceAngle_rad() const
{
   if (mInvertVectors)
   {
      return mIncidenceAngle_rad - (mControls.mCurrentControlSurfaceAngle_deg * UtMath::cRAD_PER_DEG);
   }

   return mIncidenceAngle_rad + (mControls.mCurrentControlSurfaceAngle_deg * UtMath::cRAD_PER_DEG);
}

void AeroFullFlyingSurface::SetControlAngle_deg(double aControlAngle_deg)
{
   mControls.mCurrentControlSurfaceAngle_deg = aControlAngle_deg;

   if (mControls.mCurrentControlSurfaceAngle_deg < mControls.mControlSurfaceMinAngle_deg)
   {
      mControls.mCurrentControlSurfaceAngle_deg = mControls.mControlSurfaceMinAngle_deg;
   }
   else if (mControls.mCurrentControlSurfaceAngle_deg > mControls.mControlSurfaceMaxAngle_deg)
   {
      mControls.mCurrentControlSurfaceAngle_deg = mControls.mControlSurfaceMaxAngle_deg;
   }
}

void Designer::AeroFullFlyingSurface::CalcAeroCoeffDelta(const std::string& aControlSurfaceName, 
                                                         double             aControlAngle_deg, 
                                                         double             aAltitude_ft, 
                                                         double             aSpeed_fps, 
                                                         double             aMach, 
                                                         double             aKTAS, 
                                                         double             aKCAS,
                                                         double             aDynamicPressure_psf,
                                                         double             aDensity_slugs_ft3,
                                                         double             aVehicleAlpha_deg, 
                                                         double             aVehicleBeta_deg, 
                                                         double             aVehicleYawRate_dps,
                                                         double             aVehiclePitchRate_dps,
                                                         double             aVehicleRollRate_dps, 
                                                         double&            aDeltaCL, 
                                                         double&            aDeltaCd, 
                                                         double&            aDeltaCY, 
                                                         double&            aDeltaCm, 
                                                         double&            aDeltaCn, 
                                                         double&            aDeltaCl)
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

void AeroFullFlyingSurface::SetAdditionalData(const std::string&    aAirfoilFilename,
                                              bool                  aUseExponentialAngleMapping,
                                              double                aControlSurfaceMinAngle_deg,
                                              double                aControlSurfaceMaxAngle_deg,
                                              double                aActuatorMinRate_dps,
                                              double                aActuatorMaxRate_dps,
                                              double                aActuatorMinAngle_deg,
                                              double                aActuatorMaxAngle_deg)
{
   mControls.mUseExponentialAngleMapping = aUseExponentialAngleMapping;
   mControls.mControlSurfaceMinAngle_deg = aControlSurfaceMinAngle_deg;
   mControls.mControlSurfaceMaxAngle_deg = aControlSurfaceMaxAngle_deg;
   mControls.mActuatorMinRate_dps = aActuatorMinRate_dps;
   mControls.mActuatorMaxRate_dps = aActuatorMaxRate_dps;
   mControls.mActuatorMinAngle_deg = aActuatorMinAngle_deg;
   mControls.mActuatorMaxAngle_deg = aActuatorMaxAngle_deg;

   AeroSurface::SetAdditionalData(aAirfoilFilename, aUseExponentialAngleMapping);
}

} // namespace Designer
