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

#include "AeroWing.hpp"
#include "Vehicle.hpp"

#include "GeometryTriangle.hpp"
#include "P6DofUtils.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

namespace Designer
{

AeroWing::AeroWing(const std::string& aGeneratedFromName, Vehicle* aVehicle)
   : AeroSurface(aGeneratedFromName, aVehicle)
{
   // Overwrite the AeroSurface type
   mAeroTypeString.assign("AeroWing");
}

void AeroWing::CalcForcesAndMoments(double    aDynamicPressure_lbssqft,
                                    double    aMach,
                                    UtVec3dX& aLiftVector_lbs,
                                    UtVec3dX& aDragVector_lbs,
                                    UtVec3dX& aSideForceVector_lbs,
                                    UtVec3dX& aMoment_ftlbs,
                                    UtVec3dX& aForcePoint_ft)
{
   // CalcLiftVector(aAlpha_deg, aBeta_deg, aDynamicPressure_lbssqft, aMach, aLiftVector_lbs);
   // CalcDragVector(aAlpha_deg, aBeta_deg, aDynamicPressure_lbssqft, aMach, aDragVector_lbs);
   // CalcSideForceVector(aAlpha_deg, aBeta_deg, aDynamicPressure_lbssqft, aMach, aSideForceVector_lbs);
   // CalcMomentVector(aAlpha_deg, aBeta_deg, aDynamicPressure_lbssqft, aMach, aLiftVector_lbs, aDragVector_lbs, aSideForceVector_lbs, aMoment_ftlbs);

   AeroSurface::CalcForcesAndMoments(aDynamicPressure_lbssqft, aMach, aLiftVector_lbs, aDragVector_lbs, aSideForceVector_lbs, aMoment_ftlbs, aForcePoint_ft);
}

UtVec3dX AeroWing::CalcAeroForcePoint_ft(double aMach) const
{
   // Determine the area for a single surface
   double averageChord = 0.5 * (mRootChord_ft + mTipChord_ft);
   double totalArea = mSpan_ft * averageChord;

   // Break the surface into two triangles (or a single triangle, if tip chord is zero)
   GeometryTriangle triangle1;
   GeometryTriangle triangle2;

   UtVec3dX centroid;
   UtVec3dX rootQuarterChordPt;
   UtVec3dX tipQuarterChordPt;

   // Check for the tip being zero length. We'll assume anything less than
   // a billionth of a foot is zero, due to numerical precision
   if (mTipChord_ft < 0.000000001)
   {
      // We have only a single triangle

      UtVec3dX FrontRootPt(0.0, 0.0, 0.0);
      UtVec3dX AftRootPt(-mRootChord_ft, 0.0, 0.0);

      UtVec3dX aftVec = AftRootPt - FrontRootPt;
      double length = aftVec.Magnitude();
      length *= 0.25;
      aftVec.Normalize();
      rootQuarterChordPt = aftVec * length;

      // Determine how far back from the root quarter chord the tip quarter chord exists
      double offsetLength_ft = tan(mSweepAngle_rad) * mSpan_ft;

      UtVec3dX offsetVec(-offsetLength_ft, mSpan_ft, 0.0);
      tipQuarterChordPt = rootQuarterChordPt + offsetVec;

      // Set up the single triangle
      triangle1.SetPoints(FrontRootPt, AftRootPt, tipQuarterChordPt);

      if (!triangle1.CalcCentroid())
      {
         auto outStream = ut::log::error() << "Unable to calculate a centroid for triangle in AeroWing::CalcAeroForcePoint_ft().";
         outStream.AddNote() << "Wing Name: " << mName;
      }

      // Calculate the location of the cg
      centroid = triangle1.GetCentroid_ft();
   }
   else
   {
      // We have two triangles

      UtVec3dX FrontRootPt(0.0, 0.0, 0.0);
      UtVec3dX AftRootPt(-mRootChord_ft, 0.0, 0.0);

      UtVec3dX aftVec = AftRootPt - FrontRootPt;
      double length = aftVec.Magnitude();
      length *= 0.25;
      aftVec.Normalize();
      rootQuarterChordPt = aftVec * length;

      // Determine how far back from the root leading edge the tip leading edge exists
      double offsetLength_ft = tan(mSweepAngle_rad) * mSpan_ft;

      UtVec3dX FrontTipPt(-offsetLength_ft, mSpan_ft, 0.0);
      UtVec3dX tipQuarterChordVec(-mTipChord_ft * 0.25, 0.0, 0.0);
      tipQuarterChordPt = FrontTipPt + tipQuarterChordVec;
      UtVec3dX tipAftVec(-mTipChord_ft, 0.0, 0.0);

      UtVec3dX AftTipPt = FrontTipPt + tipAftVec;

      // Set up the two triangles
      triangle1.SetPoints(FrontRootPt, AftRootPt, FrontTipPt);
      triangle2.SetPoints(AftTipPt, FrontTipPt, AftRootPt);

      double averageChord2   = 0.5 * mTipChord_ft;
      double area2           = mSpan_ft * averageChord2;
      double fractionalArea2 = area2 / totalArea;

      if (!triangle1.CalcCentroid())
      {
         auto outStream = ut::log::error() << "Unable to calculate a centroid for triangle1 in AeroWing::CalcAeroForcePoint_ft().";
         outStream.AddNote() << "Wing Name: " << mName;
      }

      if (!triangle2.CalcCentroid())
      {
         auto outStream = ut::log::error() << "Unable to calculate a centroid for triangle2 in AeroWing::CalcAeroForcePoint_ft().";
         outStream.AddNote() << "Wing Name: " << mName;
      }

      // Calculate the location of the cg
      UtVec3dX cg1 = triangle1.GetCentroid_ft();
      UtVec3dX cg2 = triangle2.GetCentroid_ft();

      UtVec3dX delCg = cg2 - cg1;
      UtVec3dX cg = cg1 + (delCg * fractionalArea2);

      // Calculate the location of the cg
      centroid = cg;
   }

   // Using the centroid, determine the centroid at the quarter-chord point
   UtVec3dX spanVect = tipQuarterChordPt - rootQuarterChordPt;
   double ySpanDist  = tipQuarterChordPt.Y() - rootQuarterChordPt.Y();
   double yFraction  = centroid.Y() / ySpanDist;

   spanVect = spanVect * yFraction;
   UtVec3dX aeroCenterQuarterChord = rootQuarterChordPt + spanVect;
   UtVec3dX aeroCenterHalfChord = centroid;

   // Adjust for root quarter chord
   UtVec3dX rootChordAdjustment(mRootChord_ft * 0.25, 0.0, 0.0);
   aeroCenterQuarterChord += rootChordAdjustment;
   aeroCenterHalfChord += rootChordAdjustment;

   // Mach effects
   UtVec3dX aeroCenter;
   if (aMach < 0.9)
   {
      aeroCenter = aeroCenterQuarterChord;
   }
   else if (aMach < 1.9)
   {
      double fraction = (aMach - 0.9);
      aeroCenter = aeroCenterQuarterChord;
      UtVec3dX deltaVec = aeroCenterHalfChord - aeroCenterQuarterChord;
      deltaVec = deltaVec * fraction;
      aeroCenter = aeroCenter + deltaVec;
   }
   else
   {
      aeroCenter = aeroCenterHalfChord;
   }

   // Now, adjust for orientation (dihedral and incidence)
   UtDCM dihedralDCM(0.0, 0.0, -mDihedralAngle_rad);
   UtDCM incidenceDCM(0.0, mIncidenceAngle_rad, 0.0);
   UtDCM attitudeDCM = incidenceDCM * dihedralDCM;
   aeroCenter = attitudeDCM.InverseTransform(aeroCenter);

   // Finally, add in ref pt offset
   aeroCenter += mRefPoint_ft;

   return aeroCenter;
}


void AeroWing::CalcAeroCoeffDelta(const std::string& aControlSurfaceName,
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

   if (aControlSurfaceName == "Aileron_Right" ||
       aControlSurfaceName == "Aileron_Left")
   {
      AeroPanel* aileron = GetAilerons();
      aileron->SetControlAngle_deg(aControlAngle_deg);

      aileron->CalcAeroCoeffDelta(aMach,
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

   if (aControlSurfaceName == "Drageron_Right_Top" ||
       aControlSurfaceName == "Drageron_Left_Top")
   {
      AeroPanel* drageron = GetDrageronsTop();
      drageron->SetControlAngle_deg(aControlAngle_deg);

      drageron->CalcAeroCoeffDelta(aMach,
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

   if (aControlSurfaceName == "Drageron_Right_Bottom" ||
       aControlSurfaceName == "Drageron_Left_Bottom")
   {
      AeroPanel* drageron = GetDrageronsBottom();
      drageron->SetControlAngle_deg(aControlAngle_deg);

      drageron->CalcAeroCoeffDelta(aMach,
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

   if (aControlSurfaceName == "Spoileron_Right" ||
       aControlSurfaceName == "Spoileron_Left")
   {
      AeroPanel* spoileron = GetSpoilerons();
      spoileron->SetControlAngle_deg(aControlAngle_deg);

      spoileron->CalcAeroCoeffDelta(aMach,
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

   if (aControlSurfaceName == "Spoiler_Right" ||
       aControlSurfaceName == "Spoiler_Left")
   {
      AeroPanel* spoiler = GetSpoilers();
      spoiler->SetControlAngle_deg(aControlAngle_deg);

      spoiler->CalcAeroCoeffDelta(aMach,
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

   if (aControlSurfaceName == "Elevon_Right" ||
       aControlSurfaceName == "Elevon_Left")
   {
      AeroPanel* elevon = GetElevons();
      elevon->SetControlAngle_deg(aControlAngle_deg);

      elevon->CalcAeroCoeffDelta(aMach,
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

}

double AeroWing::GetAspectRatio()
{
   // Note: This can be improved/changed at a later type by using fowler flaps
   return mAspectRatio;
}

double AeroWing::GetPlanformArea_ft2()
{
   // Note: This can be improved/changed at a later type by using fowler flaps
   return mArea_ft2;
}

void AeroWing::SetAileronsData(bool               aIsPresent,
                               const std::string& aName,
                               double             aChordFractionStart,
                               double             aChordFractionEnd,
                               double             aSpanFractionStart,
                               double             aSpanFractionEnd,
                               bool               aUseExponentialAngleMapping,
                               double             aControlSurfaceMinAngle_deg,
                               double             aControlSurfaceMaxAngle_deg,
                               double             aActuatorMinAngle_deg,
                               double             aActuatorMaxAngle_deg,
                               double             aActuatorMinRate_dps,
                               double             aActuatorMaxRate_dps)
{
   mAileronsPresent = aIsPresent;

   mAilerons.SetName(aName);
   mAilerons.SetAdditionalData(mAirfoilFilename,
                               aUseExponentialAngleMapping,
                               aControlSurfaceMinAngle_deg,
                               aControlSurfaceMaxAngle_deg,
                               aActuatorMinRate_dps,
                               aActuatorMaxRate_dps,
                               aActuatorMinAngle_deg,
                               aActuatorMaxAngle_deg,
                               aChordFractionStart,
                               aChordFractionEnd,
                               aSpanFractionStart,
                               aSpanFractionEnd);
}

void AeroWing::SetDrageronsData(bool               aIsPresent,
                                const std::string& aName,
                                double             aChordFractionStart,
                                double             aChordFractionEnd,
                                double             aSpanFractionStart,
                                double             aSpanFractionEnd,
                                bool               aUseExponentialAngleMapping,
                                double             aControlSurfaceMinAngle_deg,
                                double             aControlSurfaceMaxAngle_deg,
                                double             aActuatorMinAngle_deg,
                                double             aActuatorMaxAngle_deg,
                                double             aActuatorMinRate_dps,
                                double             aActuatorMaxRate_dps)
{
   mDrageronsPresent = aIsPresent;

   mDrageronsTop.SetName(aName + "_Top");
   mDrageronsTop.SetAdditionalData(mAirfoilFilename,
                                   aUseExponentialAngleMapping,
                                   aControlSurfaceMinAngle_deg,
                                   aControlSurfaceMaxAngle_deg,
                                   aActuatorMinRate_dps,
                                   aActuatorMaxRate_dps,
                                   aActuatorMinAngle_deg,
                                   aActuatorMaxAngle_deg,
                                   aChordFractionStart,
                                   aChordFractionEnd,
                                   aSpanFractionStart,
                                   aSpanFractionEnd);
   
   mDrageronsBottom.SetName(aName + "_Bottom");
   mDrageronsBottom.SetAdditionalData(mAirfoilFilename,
                                      aUseExponentialAngleMapping,
                                      aControlSurfaceMinAngle_deg,
                                      aControlSurfaceMaxAngle_deg,
                                      aActuatorMinRate_dps,
                                      aActuatorMaxRate_dps,
                                      aActuatorMinAngle_deg,
                                      aActuatorMaxAngle_deg,
                                      aChordFractionStart,
                                      aChordFractionEnd,
                                      aSpanFractionStart,
                                      aSpanFractionEnd);
}

void AeroWing::SetSpoileronsData(bool               aIsPresent,
                                 const std::string& aName,
                                 double             aChordFractionStart,
                                 double             aChordFractionEnd,
                                 double             aSpanFractionStart,
                                 double             aSpanFractionEnd,
                                 bool               aUseExponentialAngleMapping,
                                 double             aControlSurfaceMinAngle_deg,
                                 double             aControlSurfaceMaxAngle_deg,
                                 double             aActuatorMinAngle_deg,
                                 double             aActuatorMaxAngle_deg,
                                 double             aActuatorMinRate_dps,
                                 double             aActuatorMaxRate_dps)
{
   mSpoileronsPresent = aIsPresent;

   mSpoilerons.SetName(aName);
   mSpoilerons.SetAdditionalData(mAirfoilFilename,
                                 aUseExponentialAngleMapping,
                                 aControlSurfaceMinAngle_deg,
                                 aControlSurfaceMaxAngle_deg,
                                 aActuatorMinRate_dps,
                                 aActuatorMaxRate_dps,
                                 aActuatorMinAngle_deg,
                                 aActuatorMaxAngle_deg,
                                 aChordFractionStart,
                                 aChordFractionEnd,
                                 aSpanFractionStart,
                                 aSpanFractionEnd);
}

void AeroWing::SetSpoilersData(bool               aIsPresent,
                               const std::string& aName,
                               double             aChordFractionStart,
                               double             aChordFractionEnd,
                               double             aSpanFractionStart,
                               double             aSpanFractionEnd,
                               bool               aUseExponentialAngleMapping,
                               double             aControlSurfaceMinAngle_deg,
                               double             aControlSurfaceMaxAngle_deg,
                               double             aActuatorMinAngle_deg,
                               double             aActuatorMaxAngle_deg,
                               double             aActuatorMinRate_dps,
                               double             aActuatorMaxRate_dps)
{
   mSpoilersPresent = aIsPresent;

   mSpoilers.SetName(aName);
   mSpoilers.SetAdditionalData(mAirfoilFilename,
                               aUseExponentialAngleMapping,
                               aControlSurfaceMinAngle_deg,
                               aControlSurfaceMaxAngle_deg,
                               aActuatorMinRate_dps,
                               aActuatorMaxRate_dps,
                               aActuatorMinAngle_deg,
                               aActuatorMaxAngle_deg,
                               aChordFractionStart,
                               aChordFractionEnd,
                               aSpanFractionStart,
                               aSpanFractionEnd);
}

void AeroWing::SetElevonsData(bool               aIsPresent,
                              const std::string& aName,
                              double             aChordFractionStart,
                              double             aChordFractionEnd,
                              double             aSpanFractionStart,
                              double             aSpanFractionEnd,
                              bool               aUseExponentialAngleMapping,
                              double             aControlSurfaceMinAngle_deg,
                              double             aControlSurfaceMaxAngle_deg,
                              double             aActuatorMinAngle_deg,
                              double             aActuatorMaxAngle_deg,
                              double             aActuatorMinRate_dps,
                              double             aActuatorMaxRate_dps)
{
   mElevonsPresent = aIsPresent;

   mElevons.SetName(aName);
   mElevons.SetAdditionalData(mAirfoilFilename,
                              aUseExponentialAngleMapping,
                              aControlSurfaceMinAngle_deg,
                              aControlSurfaceMaxAngle_deg,
                              aActuatorMinRate_dps,
                              aActuatorMaxRate_dps,
                              aActuatorMinAngle_deg,
                              aActuatorMaxAngle_deg,
                              aChordFractionStart,
                              aChordFractionEnd,
                              aSpanFractionStart,
                              aSpanFractionEnd);
}

void AeroWing::CalcLiftVector(double    aAlpha_deg,
                              double    aBeta_deg,
                              double    aEffectiveSweep_deg,
                              double    aDynamicPressure_lbssqft,
                              double    aMach,
                              UtVec3dX& aLiftVector_lbs)
{
   // old *****************************************************************
   // // Ensure a zeroed vector
   // aLiftVector_lbs.Set(0.0, 0.0, 0.0);
   //
   // double alpha_rad = aAlpha_deg * UtMath::cRAD_PER_DEG;
   // double beta_rad = aBeta_deg * UtMath::cRAD_PER_DEG;
   //
   // // Calculate lift using alpha and beta
   // double CL = UtMath::cTWO_PI * alpha_rad;
   // // Adjust for beta
   // CL *= cos(beta_rad);
   //
   // double lift_lbs = aDynamicPressure_lbssqft * mArea_ft2 * CL;

   // new *******************************************************************
   // Get unit lift vector
   // aLiftVector_lbs.Set(0.0, 0.0, 1.0);
   double CL = CalcLiftCoefficient(aAlpha_deg, aBeta_deg, aEffectiveSweep_deg, aMach);
   // ApplyLiftCoefficientInterferenceEffects(aAlpha_deg, aMach, CL);
   double lift_lbs = aDynamicPressure_lbssqft * mArea_ft2 * CL;
   aLiftVector_lbs = lift_lbs * mLiftForceUnitVector;
}

void AeroWing::CalcDragVector(double    aAlpha_deg,
                              double    aBeta_deg,
                              double    aEffectiveSweep_deg,
                              double    aDynamicPressure_lbssqft,
                              double    aMach,
                              UtVec3dX& aDragVector_lbs)
{
   // Ensure a zeroed vector or unit drag vector
   // aDragVector_lbs.Set(0.0, 0.0, 0.0);
   // Get unit drag vector
   // aDragVector_lbs.Set(1.0, 0.0, 0.0);

   double CD = CalcDragCoefficient(aAlpha_deg, aBeta_deg, aEffectiveSweep_deg, aMach);
   double drag_lbs = aDynamicPressure_lbssqft * mArea_ft2 * CD;
   aDragVector_lbs = drag_lbs * mDragForceUnitVector;
}

void AeroWing::CalcSideForceVector(double    aAlpha_deg,
                                   double    aBeta_deg,
                                   double    aEffectiveSweep_deg,
                                   double    aDynamicPressure_lbssqft,
                                   double    aMach,
                                   UtVec3dX& aSideForceVector_lbs)
{
   // aSideForceVector_lbs.Set(0.0, 1.0, 0.0);
   double CY = CalcSideForceCoefficient(aAlpha_deg, aBeta_deg, aEffectiveSweep_deg, aMach);
   double sideForce_lbs = aDynamicPressure_lbssqft * mArea_ft2 * CY;
   aSideForceVector_lbs = sideForce_lbs * mSideForceUnitVector;
}

double AeroWing::CalcLiftCoefficient(double aAlpha_deg, double aBeta_deg, double aEffectiveSweep_deg, double aMach)
{
   return AeroSurface::CalcLiftCoefficient(aAlpha_deg, aBeta_deg, aEffectiveSweep_deg, aMach);
}

double AeroWing::CalcDragCoefficient(double aAlpha_deg, double aBeta_deg, double aEffectiveSweep_deg, double aMach)
{
   return AeroSurface::CalcDragCoefficient(aAlpha_deg, aBeta_deg, aEffectiveSweep_deg, aMach);
}

double AeroWing::CalcSideForceCoefficient(double aAlpha_deg, double aBeta_deg, double aEffectiveSweep_deg, double aMach)
{
   return 0.0;
}

} // namespace Designer
