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

#include "AeroHingedSurface.hpp"
#include "Vehicle.hpp"

namespace Designer
{

double AeroHingedSurface::CombinedControlAndIncidenceAngle_rad() const
{
   if (mInvertVectors)
   {
      return mIncidenceAngle_rad - (mAeroPanel.GetControlAngle_deg() * UtMath::cRAD_PER_DEG);
   }

   return mIncidenceAngle_rad + (mAeroPanel.GetControlAngle_deg() * UtMath::cRAD_PER_DEG);
}

void AeroHingedSurface::SetControlAngle_deg(double aControlAngle_deg)
{
   mAeroPanel.SetControlAngle_deg(aControlAngle_deg);

   if (mAeroPanel.GetControlAngle_deg() < mAeroPanel.GetControlSurfaceMinAngle_deg())
   {
      mAeroPanel.SetControlAngle_deg(mAeroPanel.GetControlSurfaceMinAngle_deg());
   }
   else if (mAeroPanel.GetControlAngle_deg() > mAeroPanel.GetControlSurfaceMaxAngle_deg())
   {
      mAeroPanel.SetControlAngle_deg(mAeroPanel.GetControlSurfaceMaxAngle_deg());
   }
}

void AeroHingedSurface::CalcAeroCoeffDelta(const std::string& aControlSurfaceName,
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

   mVehicle->SetVehicleAlphaBeta(aVehicleAlpha_deg, aVehicleBeta_deg);

   // Set the vehicle rates
   mVehicle->SetVehicleYawPitchRollRates(aVehicleYawRate_dps, aVehiclePitchRate_dps, aVehicleRollRate_dps);

   // Set vehicle speed-related data
   mVehicle->SetVehicleSpeedParameters(aSpeed_fps, aMach, aKTAS, aKCAS, aDynamicPressure_psf, aDensity_slugs_ft3);

   CalculateAeroAttitudeVectors();
   CalcAeroData(aAltitude_ft, aSpeed_fps, aMach, aKTAS, aKCAS, aDynamicPressure_psf, aDensity_slugs_ft3,
      aVehicleAlpha_deg, aVehicleBeta_deg, aVehicleYawRate_dps, aVehiclePitchRate_dps, aVehicleRollRate_dps,
      lift_lbs0, drag_lbs0, side_lbs0, moment_ftlbs0);

   double vehicleReferenceArea_ft2 = 0.0;
   double vehicleReferenceChord_ft = 0.0;
   double vehicleReferenceSpan_ft  = 0.0;
   bool   vehicleHasWing           = false;
   if (mVehicle->GetAero()->UseRefArea())
   {
      vehicleReferenceArea_ft2 = mVehicle->GetAero()->GetVehicleRefArea_ft2();
   }
   else
   {
      vehicleReferenceArea_ft2 = mVehicle->GetAero()->GetVehicleWingArea_ft2();
      vehicleReferenceChord_ft = mVehicle->GetAero()->GetVehicleWingChord_ft();
      vehicleReferenceSpan_ft  = mVehicle->GetAero()->GetVehicleWingSpan_ft();
      vehicleHasWing = true;
   }

   double CL0 = lift_lbs0 / (aDynamicPressure_psf * vehicleReferenceArea_ft2);
   double Cd0 = drag_lbs0 / (aDynamicPressure_psf * vehicleReferenceArea_ft2);

   double maximumPositiveCL = mAeroData.GetPosLiftPeak(GetSweepAngle_deg()).second;
   double maximumNegativeCL = mAeroData.GetNegLiftPeak(GetSweepAngle_deg()).second;
   double airfoilLiftCurveSlope_perRad = mAeroData.GetPreStallSlope(0.0);
   double wingLiftCurveSlope_perRad = mAeroData.GetPreStallSlope(GetSweepAngle_deg());

   double machDragDivergence = 0.0;
   double machDragPeak = 0.0;
   CalcTransonicMachEndpoints(machDragDivergence, machDragPeak);

   AeroPanel* controlSurface = GetControlSurface();
   controlSurface->SetControlAngle_deg(aControlAngle_deg);

   controlSurface->CalcAeroCoeffDelta(aMach,
                                      mArea_ft2,
                                      vehicleHasWing,
                                      vehicleReferenceArea_ft2,
                                      vehicleReferenceChord_ft,
                                      vehicleReferenceSpan_ft,
                                      aVehicleAlpha_deg,
                                      aVehicleBeta_deg,
                                      aDynamicPressure_psf,
                                      airfoilLiftCurveSlope_perRad,
                                      wingLiftCurveSlope_perRad,
                                      maximumPositiveCL,
                                      maximumNegativeCL,
                                      machDragDivergence,
                                      machDragPeak,
                                      CL0,
                                      Cd0,
                                      aDeltaCL,
                                      aDeltaCd,
                                      aDeltaCY,
                                      aDeltaCm,
                                      aDeltaCn,
                                      aDeltaCl);
}

} // namespace Designer
