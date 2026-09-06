// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "AeroPanel.hpp"

void Designer::AeroPanel::CalcAeroCoeffDelta(double             aMach,
                                             double             aPartArea_ft2,
                                             bool               aVehicleHasWing,
                                             double             aVehicleReferenceArea_ft2,
                                             double             aVehiclePitchRefLength_ft,
                                             double             aVehicleLateralRefLength_ft,
                                             double             aVehicleAlpha_deg,
                                             double             aVehicleBeta_deg,
                                             double             aDynamicPressure_psf,
                                             double             aAirfoilLiftCurveSlope_perRad,
                                             double             aSurfaceLiftCurveSlope_perRad,
                                             double             aSurfaceMaxPosLiftCoeff,
                                             double             aSurfaceMaxNegLiftCoeff,
                                             double             aMachDragDivergence,
                                             double             aMachDragPeak,
                                             double             aUnflappedCL,
                                             double             aUnflappedCD,
                                             double&            aDeltaCL,
                                             double&            aDeltaCd,
                                             double&            aDeltaCY,
                                             double&            aDeltaCm,
                                             double&            aDeltaCn,
                                             double&            aDeltaCl)
{
   aDeltaCL = 0.0;
   aDeltaCd = 0.0;
   aDeltaCY = 0.0;
   aDeltaCm = 0.0;
   aDeltaCn = 0.0;
   aDeltaCl = 0.0;

   if (std::abs(mControls.mCurrentControlSurfaceAngle_deg) < cEpsilon)
   {
      return;
   }

   if (aVehicleHasWing && (aVehiclePitchRefLength_ft < cEpsilon || aVehicleLateralRefLength_ft < cEpsilon))
   {
      // If we make it here, we've passed in a zero reference length, and we have an issue elsewhere
      return;
   }

   // As a working solution for dragerons/decelerons, we treat each piece independently.
   // (that is, an aircraft will have 4: upper left, lower left, upper right, lower right)
   // When open, each surface contributes its own drag and positive or negative lift.
   // When collapsed, the clamshell acts as a single-flap aileron, and can be treated as a
   // single surface. For simplicity as well as due to table dimension constraints, we
   // assume aileron-style effects for a positive deflection of the bottom surfaces
   // or a negative deflection of the top surfaces, and null effect otherwise.
   // This allows us to treat the control surfaces as ailerons during ordinary
   // flight, and to double the drag effects when deploying the surfaces for deceleration.

   if (mName == "Drageron_Right_Top" || mName == "Drageron_Left_Top")
   {
      if (mControls.mCurrentControlSurfaceAngle_deg > 0.0)
      {
         return;
      }
   }
   if (mName == "Drageron_Right_Bottom" || mName == "Drageron_Left_Bottom")
   {
      if (mControls.mCurrentControlSurfaceAngle_deg < 0.0)
      {
         return;
      }
   }

   // These represent coefficient deltas below the critical Mach number (say, M=0.8)
   double subsonicDeltaCL = 0.0;
   double subsonicDeltaCd = 0.0;
   double subsonicDeltaCm = 0.0;

   // These represent coefficient deltas at the peak-shock-strength Mach number (say, M=1.2)
   double supersonicDeltaCL = 0.0;
   double supersonicDeltaCd = 0.0;
   double supersonicDeltaCm = 0.0;

   // Constant, broadly useful flap properties
   double chordFlapFraction = mChordFractionEnd - mChordFractionStart;
   bool deviceIsLiftSpoiler = mChordFractionEnd < 0.99 || mName.find("Spoiler") != std::string::npos;

   if (deviceIsLiftSpoiler && mControls.mCurrentControlSurfaceAngle_deg < 0.0)
   {
      return;
   }

   double taperRatio = mAeroSurface->GetTipChord_ft() / mAeroSurface->GetRootChord_ft();
   double aspectRatio = mAeroSurface->GetAspectRatio();
   double sweepLeadingEdge_rad = mAeroSurface->GetSweepAngle_deg() * UtMath::cRAD_PER_DEG;

   // This lambda computes sweep angle at different chord stations.
   auto sweepAngle_rad = [taperRatio, aspectRatio, sweepLeadingEdge_rad](double chordFraction)
   {
      double tanSweepAngle = tan(sweepLeadingEdge_rad) - 4.0 / aspectRatio * (chordFraction * (1.0 - taperRatio) / (1.0 + taperRatio));
      return atan(tanSweepAngle);
   };

   double sweepTrailingEdge_rad = sweepAngle_rad(1.0);
   double sweepQuarterChord_rad = sweepAngle_rad(0.25);

   // Overriding the argument Mach numbers with semi-stock values
   // This range is a little narrower than DATCOM's stock range, but should be reasonable
   // This is intended to be a temporary measure, but is stable enough to work indefinitely
   aMachDragDivergence = 0.8;
   aMachDragPeak = 1.02 / cos(sweepLeadingEdge_rad);

   double subsonicQueryMach   = std::min(aMach, aMachDragDivergence);
   double supersonicQueryMach = std::max(aMach, aMachDragPeak);

   // Subsonic data, or transonic data computed at the critical Mach reference point
   if (aMach < aMachDragPeak)
   {
      // Linear lift coefficient deltas
      double wingEfficiency = aSurfaceLiftCurveSlope_perRad / aAirfoilLiftCurveSlope_perRad;

      double delta_cl = 0.0;
      double delta_cl_max = 0.0;
      if (deviceIsLiftSpoiler)
      {
         delta_cl = CalcSubsonicAirfoilLiftCoeffDeltaSpoiler(subsonicQueryMach, aAirfoilLiftCurveSlope_perRad);
      }
      else
      {
         delta_cl = CalcSubsonicAirfoilLiftCoeffDeltaPlainFlap(subsonicQueryMach, wingEfficiency, delta_cl_max);
      }

      double sweepQuarterChord_deg = sweepQuarterChord_rad * UtMath::cDEG_PER_RAD;
      subsonicDeltaCL = CalcSubsonicLiftCoeffDelta(delta_cl, delta_cl_max, sweepQuarterChord_deg, wingEfficiency, aSurfaceMaxPosLiftCoeff, aSurfaceMaxNegLiftCoeff, aUnflappedCL);

      // Drag coefficient deltas
      double deviceLiftEffectiveness = 1.0; // May be a function of chord fraction, data available for slotted flaps
      double inducedDragIncrement = CalcInducedDragCoeffDelta(subsonicQueryMach, aSurfaceLiftCurveSlope_perRad, deviceLiftEffectiveness);
      double profileDragIncrement = CalcProfileDragCoeffDelta();

      subsonicDeltaCd = inducedDragIncrement + profileDragIncrement;
   }

   // Supersonic data, or transonic data computed at the peak-drag Mach reference point
   if (aMach > aMachDragDivergence)
   {
      if (deviceIsLiftSpoiler)
      {
         supersonicDeltaCL = CalcSupersonicLiftCoeffDeltaSpoiler(supersonicQueryMach, UtMath::cDEG_PER_RAD * sweepTrailingEdge_rad, aSurfaceMaxPosLiftCoeff, aSurfaceMaxNegLiftCoeff, aUnflappedCL);
      }
      else
      {
         supersonicDeltaCL = CalcSupersonicLiftCoeffDeltaPlainFlap(supersonicQueryMach, UtMath::cDEG_PER_RAD * sweepTrailingEdge_rad, aSurfaceMaxPosLiftCoeff, aSurfaceMaxNegLiftCoeff, aUnflappedCL);
      }

      double unflappedWaveDrag = CalcEstimatedUnflappedWaveDragCoeffDelta(supersonicQueryMach);
      double waveDragIncrement = CalcWaveDragCoeffDelta(supersonicQueryMach, unflappedWaveDrag);
      supersonicDeltaCd = waveDragIncrement;
   }

   double deltaLiftCoefficient = subsonicDeltaCL;
   double deltaDragCoefficient = subsonicDeltaCd;
   double deltaMomentCoefficient = subsonicDeltaCm;
   if (aMach <= aMachDragDivergence)
   {
      deltaLiftCoefficient = subsonicDeltaCL;
      deltaDragCoefficient = subsonicDeltaCd;
      deltaMomentCoefficient = subsonicDeltaCm;
   }
   else if (aMach > aMachDragDivergence && aMach < aMachDragPeak)
   {
      double transonicDeltaCd = UtMath::Lerp(aMach, aMachDragDivergence, aMachDragPeak, subsonicDeltaCd, supersonicDeltaCd);
      double transonicDeltaCm = UtMath::Lerp(aMach, aMachDragDivergence, aMachDragPeak, subsonicDeltaCm, supersonicDeltaCm);
      double transonicDeltaCL = UtMath::Lerp(aMach, aMachDragDivergence, aMachDragPeak, subsonicDeltaCL, supersonicDeltaCL);

      // Interpolate between between critical and peak compressiblity
      deltaLiftCoefficient = transonicDeltaCL;
      deltaDragCoefficient = transonicDeltaCd;
      deltaMomentCoefficient = transonicDeltaCm;
   }
   else
   {
      deltaLiftCoefficient = supersonicDeltaCL;
      deltaDragCoefficient = supersonicDeltaCd;
      deltaMomentCoefficient = supersonicDeltaCm;
   }



   // Determine the location of center of pressure of incremental load due to flap. 
   // DATCOM Figure 6.1.2.1-35a
   double xcp = -0.25*chordFlapFraction + 0.5;

   // Calculate additional dCm from DATCOM Equation 6.1.2.1-a
   deltaMomentCoefficient += deltaLiftCoefficient * (0.25 - xcp);

   double deltaLift_lbs = deltaLiftCoefficient * aPartArea_ft2 * aDynamicPressure_psf;
   double deltaDrag_lbs = deltaDragCoefficient * aPartArea_ft2 * aDynamicPressure_psf;
   double deltaMoment_ftlbs = deltaMomentCoefficient * aPartArea_ft2 * aDynamicPressure_psf * aVehiclePitchRefLength_ft;

   // Compute and add incidental torques from new forces
   UtVec3dX forcePoint_ft = mAeroSurface->CalcAeroForcePoint_ft(aMach);

   UtVec3dX liftBodyRefVector_lbs = deltaLift_lbs * mAeroSurface->GetLiftForceUnitVector();
   UtVec3dX dragBodyRefVector_lbs = deltaDrag_lbs * mAeroSurface->GetDragForceUnitVector();
   UtVec3dX sideBodyRefVector_lbs = 0.0 * mAeroSurface->GetSideForceUnitVector();
   UtVec3dX momentBodyRef_ftlbs = deltaMoment_ftlbs * mAeroSurface->GetSideForceUnitVector();


   // TODO: The following code is repeated a lot. Put in helper function

   P6DofForceAndMomentsObject liftBodyZeroVector_lbs;
   P6DofForceAndMomentsObject dragBodyZeroVector_lbs;
   P6DofForceAndMomentsObject sideBodyZeroVector_lbs;
   P6DofForceAndMomentsObject momentBodyZero_ftlbs;

   // Set the reference point at the object's aero reference point
   liftBodyZeroVector_lbs.MoveRefPoint_ft(forcePoint_ft);
   dragBodyZeroVector_lbs.MoveRefPoint_ft(forcePoint_ft);
   sideBodyZeroVector_lbs.MoveRefPoint_ft(forcePoint_ft);
   momentBodyZero_ftlbs.MoveRefPoint_ft(forcePoint_ft);

   // Clear F&M data
   liftBodyZeroVector_lbs.ClearForcesAndMoments();
   dragBodyZeroVector_lbs.ClearForcesAndMoments();
   sideBodyZeroVector_lbs.ClearForcesAndMoments();
   momentBodyZero_ftlbs.ClearForcesAndMoments();

   // Add the body-relative data at the reference point
   UtVec3dX zeroVec(0.0, 0.0, 0.0);
   liftBodyZeroVector_lbs.AddForceAndMomentAtReferencePoint(liftBodyRefVector_lbs, zeroVec);
   dragBodyZeroVector_lbs.AddForceAndMomentAtReferencePoint(dragBodyRefVector_lbs, zeroVec);
   sideBodyZeroVector_lbs.AddForceAndMomentAtReferencePoint(sideBodyRefVector_lbs, zeroVec);
   momentBodyZero_ftlbs.AddForceAndMomentAtReferencePoint(zeroVec, momentBodyRef_ftlbs);

   // Move the reference point to the origin as the "zero point, rather than using
   // each object's aero force center

   UtVec3dX zeroPointReference(0.0, 0.0, 0.0);
   liftBodyZeroVector_lbs.MoveRefPoint_ft(zeroPointReference);
   dragBodyZeroVector_lbs.MoveRefPoint_ft(zeroPointReference);
   sideBodyZeroVector_lbs.MoveRefPoint_ft(zeroPointReference);
   momentBodyZero_ftlbs.MoveRefPoint_ft(zeroPointReference);

   // At this point, all F&M are based on a reference point of the origin, as desired

   // Now, we calculate the total moment, including all induced moments from all F&M contributors

   UtVec3dX partialMoment(0.0, 0.0, 0.0);
   UtVec3dX totalMomentBodyZero_ftlbs(0.0, 0.0, 0.0);

   partialMoment.Set(0.0, 0.0, 0.0);
   UtVec3dX tempLiftBodyZeroVector_lbs(0.0, 0.0, 0.0);
   liftBodyZeroVector_lbs.GetForceAndMomentAtCurrentRefPoint(tempLiftBodyZeroVector_lbs, partialMoment);
   totalMomentBodyZero_ftlbs += partialMoment;

   partialMoment.Set(0.0, 0.0, 0.0);
   UtVec3dX tempDragBodyZeroVector_lbs(0.0, 0.0, 0.0);
   dragBodyZeroVector_lbs.GetForceAndMomentAtCurrentRefPoint(tempDragBodyZeroVector_lbs, partialMoment);
   totalMomentBodyZero_ftlbs += partialMoment;

   partialMoment.Set(0.0, 0.0, 0.0);
   UtVec3dX tempSideBodyZeroVector_lbs(0.0, 0.0, 0.0);
   sideBodyZeroVector_lbs.GetForceAndMomentAtCurrentRefPoint(tempSideBodyZeroVector_lbs, partialMoment);
   totalMomentBodyZero_ftlbs += partialMoment;

   UtVec3dX zeroForce(0.0, 0.0, 0.0);
   partialMoment.Set(0.0, 0.0, 0.0);
   momentBodyZero_ftlbs.GetForceAndMomentAtCurrentRefPoint(zeroForce, partialMoment);
   totalMomentBodyZero_ftlbs += partialMoment;

   // We now have the body-relative forces at the zero point. We also have the
   // total moment (in body coordinates) at the zero point.

   // We now need to determine the magnitude of the forces along the primary lift, drag, and
   // side force directions, rather than in body space.

   UtVec3dX liftVectorDirection(0.0, 0.0, -1.0);
   UtVec3dX dragVectorDirection(-1.0, 0.0, 0.0);
   UtVec3dX sideVectorDirection(0.0, 1.0, 0.0);

   // Transform the body relative vectors into the primary reference

   // Note negative beta
   UtDCM dcm(-aVehicleBeta_deg * UtMath::cRAD_PER_DEG, aVehicleAlpha_deg * UtMath::cRAD_PER_DEG, 0.0);

   UtVec3dX tempLiftVectorPrimary_lbs = dcm.InverseTransform(tempLiftBodyZeroVector_lbs);
   UtVec3dX tempDragVectorPrimary_lbs = dcm.InverseTransform(tempDragBodyZeroVector_lbs);
   UtVec3dX tempSideVectorPrimary_lbs = dcm.InverseTransform(tempSideBodyZeroVector_lbs);

   // Get forces along the primary lift vector direction
   double lift_lbs = liftVectorDirection.Dot(tempLiftVectorPrimary_lbs) +
      liftVectorDirection.Dot(tempDragVectorPrimary_lbs) +
      liftVectorDirection.Dot(tempSideVectorPrimary_lbs);

   // Get forces along the primary drag vector direction
   double drag_lbs = dragVectorDirection.Dot(tempLiftVectorPrimary_lbs) +
      dragVectorDirection.Dot(tempDragVectorPrimary_lbs) +
      dragVectorDirection.Dot(tempSideVectorPrimary_lbs);

   // Get forces along the primary side vector direction
   double side_lbs = sideVectorDirection.Dot(tempLiftVectorPrimary_lbs) +
      sideVectorDirection.Dot(tempDragVectorPrimary_lbs) +
      sideVectorDirection.Dot(tempSideVectorPrimary_lbs);

   // Get the total moment (in body coordinates)
   UtVec3dX momentVec_ftlbs = totalMomentBodyZero_ftlbs;

   if (aVehicleHasWing)
   {
      aDeltaCL = lift_lbs / (aVehicleReferenceArea_ft2 * aDynamicPressure_psf);
      aDeltaCd = drag_lbs / (aVehicleReferenceArea_ft2 * aDynamicPressure_psf);
      aDeltaCY = side_lbs / (aVehicleReferenceArea_ft2 * aDynamicPressure_psf);
      aDeltaCl = momentVec_ftlbs.X() / (aVehicleReferenceArea_ft2 * aVehicleLateralRefLength_ft * aDynamicPressure_psf);
      aDeltaCm = momentVec_ftlbs.Y() / (aVehicleReferenceArea_ft2 * aVehiclePitchRefLength_ft * aDynamicPressure_psf);
      aDeltaCn = momentVec_ftlbs.Z() / (aVehicleReferenceArea_ft2 * aVehicleLateralRefLength_ft * aDynamicPressure_psf);
   }
   else
   {
      aDeltaCL = lift_lbs / (aVehicleReferenceArea_ft2 * aDynamicPressure_psf);
      aDeltaCd = drag_lbs / (aVehicleReferenceArea_ft2 * aDynamicPressure_psf);
      aDeltaCY = side_lbs / (aVehicleReferenceArea_ft2 * aDynamicPressure_psf);
      aDeltaCl = momentVec_ftlbs.X() / (aVehicleReferenceArea_ft2 * aDynamicPressure_psf);
      aDeltaCm = momentVec_ftlbs.Y() / (aVehicleReferenceArea_ft2 * aDynamicPressure_psf);
      aDeltaCn = momentVec_ftlbs.Z() / (aVehicleReferenceArea_ft2 * aDynamicPressure_psf);
   }
}

double Designer::AeroPanel::CalcSubsonicAirfoilLiftCoeffDeltaSpoiler(double aMach, double aLiftCurveSlopeAirfoil_perRad)
{
   double chordFlapFraction = mChordFractionEnd - mChordFractionStart;

   double controlAngle_deg = mControls.mCurrentControlSurfaceAngle_deg;
   double spoilerLengthFraction = chordFlapFraction * cos(UtMath::cRAD_PER_DEG * controlAngle_deg) + mChordFractionStart;
   double spoilerHeightFraction = chordFlapFraction * sin(UtMath::cRAD_PER_DEG * controlAngle_deg); // + thickness at hinge

   double deltaAlphaFiftyPercentLength = std::max(0.0, 0.4448 + 0.1528 * std::log(spoilerHeightFraction)); // curve fit, from DATCOM Fig. 6.1.1.1-52
   double deltaAlphaEightyPercentLength = std::max(0.0, 0.3561 + 0.0986 * std::log(spoilerHeightFraction));
   double deltaEffectiveAlpha = UtMath::Lerp(spoilerLengthFraction, 0.5, 0.8, deltaAlphaFiftyPercentLength, deltaAlphaEightyPercentLength);

   double deltaLiftCoeffAirfoil = -aLiftCurveSlopeAirfoil_perRad * deltaEffectiveAlpha;
   return deltaLiftCoeffAirfoil;
}

double Designer::AeroPanel::CalcSubsonicAirfoilLiftCoeffDeltaPlainFlap(double aMach, double aSurfaceLiftEfficiency, double& aMaximumLiftCoeffIncrement)
{
   double chordFlapFraction = mChordFractionEnd - mChordFractionStart;

   // Increment to maximum 3D lift coefficient, to be used later when applying effects of modified lift curve slope
   double thicknessRatio = std::min(0.20, mAeroSurface->GetThicknessRatio());
   double referenceMaxCL_Increment =
      1.0 +
      1.927e+0 * thicknessRatio +
      -6.47e+1 * pow(thicknessRatio, 2) +
      -4.70e+2 * pow(thicknessRatio, 3) +
      1.091e+4 * pow(thicknessRatio, 4) +
      -3.41e+4 * pow(thicknessRatio, 5); // curve fit, from DATCOM Fig. 6.1.1.3-12a

   double clampedThirtyPercentChordFraction = std::min(0.3, chordFlapFraction);
   double clampedSixtyPercentChordFraction = std::min(0.3, chordFlapFraction);
   double k1 =
      9.541e+0 * clampedThirtyPercentChordFraction +
      -3.45e+1 * pow(clampedThirtyPercentChordFraction, 2) +
      4.828e+1 * pow(clampedThirtyPercentChordFraction, 3); // curve fit, from DATCOM Fig. 6.1.1.3-12b
   double k2 =
      3.4024 * clampedSixtyPercentChordFraction +
      -2.9525 * pow(clampedSixtyPercentChordFraction, 2); // curve fit, from DATCOM Fig. 6.1.1.3-13a

   aMaximumLiftCoeffIncrement = referenceMaxCL_Increment * k1 * k2;

   // Increment to 2D lift curve slope
   double dcl_dDel_theory = 7.1315 * pow(chordFlapFraction, 0.4584); // curve fit, from DATCOM Fig. 6.1.1.1-39a, 0.15 t/c

   double deflectionLiftEfficacyRatioAtReferenceWingEfficiency = 0.3523 + 0.3788 * chordFlapFraction; // curve fit, from DATCOM Fig. 6.1.1.1-39b, 0.15 t/c
   double deflectionLiftEfficacyRatio = 0.0;
   if (aSurfaceLiftEfficiency > 0.7)
   {
      deflectionLiftEfficacyRatio = UtMath::Lerp(aSurfaceLiftEfficiency, 0.7, 1.0, deflectionLiftEfficacyRatioAtReferenceWingEfficiency, 1.0);
   }
   else
   {
      deflectionLiftEfficacyRatio = UtMath::Lerp(aSurfaceLiftEfficiency, 0.7, 0.0, deflectionLiftEfficacyRatioAtReferenceWingEfficiency, 0.0);
   }

   double controlAngle_deg = mControls.mCurrentControlSurfaceAngle_deg;

   double chordFactorFiftyPercentChordFlap = 0.71 + (1.0 - 0.71) * tanh(-6.0 * UtMath::cRAD_PER_DEG * (std::abs(controlAngle_deg) - 20.0)); // curve fit, from DATCOM Fig. 6.1.1.1-40, 0.50 c_f/c
   double chordFactorTenPercentChordFlap = 0.78 + (1.0 - 0.78) * tanh(-4.0 * UtMath::cRAD_PER_DEG * (std::abs(controlAngle_deg) - 30.0)); // curve fit, from DATCOM Fig. 6.1.1.1-40, 0.10 c_f/c
   double chordFactor = UtMath::Lerp(chordFlapFraction, 0.1, 0.5, chordFactorTenPercentChordFlap, chordFactorFiftyPercentChordFlap);

   double controlAngle_rad = controlAngle_deg * UtMath::cRAD_PER_DEG;
   double deltaLiftCurveSlopeAirfoil = controlAngle_rad * deflectionLiftEfficacyRatio * dcl_dDel_theory * chordFactor;
   return deltaLiftCurveSlopeAirfoil;
}

double Designer::AeroPanel::CalcSupersonicLiftCoeffDeltaPlainFlap(double aMach, double aTrailingEdgeSweep_deg, double aMaxPosLiftCoeff, double aMaxNegLiftCoeff, double aUnflappedLiftCoeff)
{
   double chordFlapFraction = mChordFractionEnd - mChordFractionStart;
   double spanFlapFraction = mSpanFractionEnd - mSpanFractionStart;

   double trailingEdgeAngle_rad = mAeroSurface->GetThicknessRatio() * mAeroSurface->GetThicknessRatio();

   double machSquared = aMach * aMach;
   double compressibilityFactor = sqrt(machSquared - 1.0);

   double machFactor1 = 2.0 / compressibilityFactor;
   double machFactor2 = ((1.4 + 1) * machSquared * machSquared - 4.0 * (machSquared - 1.0)) / (2.0 * pow(machSquared - 1.0, 2));

   double taperFactor = tan(UtMath::cRAD_PER_DEG * aTrailingEdgeSweep_deg) / compressibilityFactor;
   double dCL_dDeltaReference = (0.07 / (1.0 - std::abs(pow(taperFactor, 4)))) / compressibilityFactor;
   double dCL_dDelta = (1.0 - machFactor2 / machFactor1 * trailingEdgeAngle_rad) * dCL_dDeltaReference * spanFlapFraction * chordFlapFraction;

   dCL_dDelta = std::max(0.0, dCL_dDelta);
   double deltaLiftCoeff = dCL_dDelta * mControls.mCurrentControlSurfaceAngle_deg;

   return deltaLiftCoeff;
}

double Designer::AeroPanel::CalcSupersonicLiftCoeffDeltaSpoiler(double aMach, double aTrailingEdgeSweep_deg, double aMaxPosLiftCoeff, double aMaxNegLiftCoeff, double aUnflappedLiftCoeff)
{
   // DATCOM does not offer a prescription for supersonic spoiler effects,
   // so we modify the guidance for supersonic plain flaps

   double chordFlapFraction = 1.0 - mChordFractionStart;
   double spanFlapFraction = mSpanFractionEnd - mSpanFractionStart;

   double trailingEdgeAngle_rad = mAeroSurface->GetThicknessRatio() * mAeroSurface->GetThicknessRatio();

   double machSquared = aMach * aMach;
   double compressibilityFactor = sqrt(machSquared - 1.0);

   double machFactor1 = 2.0 / compressibilityFactor;
   double machFactor2 = ((1.4 + 1) * machSquared * machSquared - 4.0 * (machSquared - 1.0)) / (2.0 * pow(machSquared - 1.0, 2));

   double taperFactor = tan(UtMath::cRAD_PER_DEG * aTrailingEdgeSweep_deg) / compressibilityFactor;
   double dCL_dDeltaReference = (0.07 / (1.0 - std::abs(pow(taperFactor, 4)))) / compressibilityFactor;
   double dCL_dDelta = (1.0 - machFactor2 / machFactor1 * trailingEdgeAngle_rad) * dCL_dDeltaReference * spanFlapFraction * chordFlapFraction;

   dCL_dDelta = std::max(0.0, dCL_dDelta);
   double deltaLiftCoeff = -dCL_dDelta * mControls.mCurrentControlSurfaceAngle_deg;

   return deltaLiftCoeff;
}

double Designer::AeroPanel::CalcSubsonicSpanFactor(double spanFraction)
{
   return 1.3767 * spanFraction +
         -0.031 * pow(spanFraction, 2) +
         -0.343 * pow(spanFraction, 3); // curve fit, from DATCOM Fig. 6.1.4.1-15
}

double Designer::AeroPanel::CalcSubsonicLiftCoeffDelta(double aAirfoilLiftCoeffDelta, double aAirfoilLiftCoeffMaximumDelta, double aQuarterChordSweepAngle_deg, double aSurfaceLiftEfficiency, double aMaxUnflappedPosLiftCoeff, double aMaxUnflappedNegLiftCoeff, double aUnflappedLiftCoeff)
{
   // Note: In the future, we might investigate additional effects such as sweep and span fraction.
   //double spanFlapFraction = mSpanFractionEnd - mSpanFractionStart;
   //double cosQuarterChordSweep = cos(aQuarterChordSweepAngle_deg * UtMath::cRAD_PER_DEG);
   //double sweepFactor = (1.0 - 0.08 * cosQuarterChordSweep * cosQuarterChordSweep) * pow(cosQuarterChordSweep, 0.75);

   double chordFlapFraction = mChordFractionEnd - mChordFractionStart;

   double spanFactorFlap = CalcSubsonicSpanFactor(mSpanFractionEnd) - CalcSubsonicSpanFactor(mSpanFractionStart);

   // curve fits, from DATCOM Fig. 6.1.6.1-14
   double liftEffectivenessAirfoil = -sqrt(1.0 - pow((chordFlapFraction - 1.0), 2));
   double liftEffectivenessRatioAtThirtyPercentReduction = 1.4832 - 0.183 * log(mAeroSurface->GetAspectRatio());
   double flapEffectivenessRatio = UtMath::Lerp(liftEffectivenessAirfoil, -0.3, -1.0, liftEffectivenessRatioAtThirtyPercentReduction, 1.0);

   double deltaLiftCoeff = aAirfoilLiftCoeffDelta * aSurfaceLiftEfficiency * flapEffectivenessRatio * spanFactorFlap;

   // We don't account for CL_max or gracefully blend between linear and non-linear control surface effects
   // When we do implement this blend, we'll need to be mindful of the effect on resulting increments to damping coefficients

   return deltaLiftCoeff;
}

double Designer::AeroPanel::CalcSpanStationLoadingDelta(double aMach, double aLiftCurveSlopeSurface_perRad, size_t aStationIndex, size_t aNumberOfStations)
{
   double beta = sqrt(std::abs(aMach * aMach - 1.0));
   double kappa = aLiftCurveSlopeSurface_perRad / UtMath::cTWO_PI;
   double flowFactor = beta * mAeroSurface->GetAspectRatio() / kappa;

   double eta = double(aStationIndex) / double(aNumberOfStations);
   double eta2 = eta * eta;
   double eta3 = eta * eta2;
   double eta4 = eta * eta3;
   double eta5 = eta * eta4;
   double eta6 = eta * eta5;

   double referenceLoadFactorTwentyPercentSemispan =
      0.4269 +
      -0.3115 * eta +
      -1.5991 * eta2 +
      3.3450 * eta3 +
      -1.8648 * eta4;
   double referenceLoadFactorHalfSemispan =
      0.7931 +
      0.1361 * eta +
      -0.7069 * eta2 +
      -1.4219 * eta3 +
      1.2238 * eta4;
   double referenceLoadFactorFullSemispan =
      0.9994 +
      0.1333 * eta +
      -3.4190 * eta2 +
      18.317 * eta3 +
      -46.315 * eta4 +
      51.037 * eta5 +
      -20.752 * eta6;

   double scaleFactorTwentyPercentSemispan = exp(-0.158 * flowFactor);
   double scaleFactorHalfSemispan = exp(-0.142 * flowFactor);
   double scaleFactorFullSemispan = exp(-0.100 * flowFactor);

   double loadFactorTwentyPercentSemispan = scaleFactorTwentyPercentSemispan * referenceLoadFactorTwentyPercentSemispan;
   double loadFactorHalfSemispan = scaleFactorHalfSemispan * referenceLoadFactorHalfSemispan;
   double loadFactorFullSemispan = scaleFactorFullSemispan * referenceLoadFactorFullSemispan;

   double loadFactorInner = 0.0;
   if (mSpanFractionStart < 0.195)
   {
      loadFactorInner = UtMath::Lerp(mSpanFractionStart, 0.0, 0.195, 0.0, loadFactorTwentyPercentSemispan);
   }
   else if (mSpanFractionStart < 0.556)
   {
      loadFactorInner = UtMath::Lerp(mSpanFractionStart, 0.195, 0.556, loadFactorTwentyPercentSemispan, loadFactorHalfSemispan);
   }
   else
   {
      loadFactorInner = UtMath::Lerp(mSpanFractionStart, 0.556, 1.0, loadFactorHalfSemispan, loadFactorFullSemispan);
   }

   double loadFactorOuter = 0.0;
   if (mSpanFractionEnd < 0.195)
   {
      loadFactorOuter = UtMath::Lerp(mSpanFractionEnd, 0.0, 0.195, 0.0, loadFactorTwentyPercentSemispan);
   }
   else if (mSpanFractionEnd < 0.556)
   {
      loadFactorOuter = UtMath::Lerp(mSpanFractionEnd, 0.195, 0.556, loadFactorTwentyPercentSemispan, loadFactorHalfSemispan);
   }
   else
   {
      loadFactorOuter = UtMath::Lerp(mSpanFractionEnd, 0.556, 1.0, loadFactorHalfSemispan, loadFactorFullSemispan);
   }

   double loadFactor = loadFactorOuter - loadFactorInner;
   return loadFactor;
}

double Designer::AeroPanel::CalcInducedDragCoeffDelta(double aMach, double aLiftCurveSlopeSurface_perRad, double aDeviceEffectivenessRatio)
{
   constexpr size_t numberOfSpanStations = 11; // This must be an odd number
   constexpr size_t k = (numberOfSpanStations + 1) / 2;
   double controlAngle_deg = mControls.mCurrentControlSurfaceAngle_deg;

   auto G = [this, aMach, aLiftCurveSlopeSurface_perRad, controlAngle_deg, aDeviceEffectivenessRatio, numberOfSpanStations](size_t n)
   {
      double loadFactor = CalcSpanStationLoadingDelta(aMach, aLiftCurveSlopeSurface_perRad, n, numberOfSpanStations);
      return -loadFactor * aDeviceEffectivenessRatio * UtMath::cRAD_PER_DEG * controlAngle_deg;
   };

   auto del = [](size_t v, size_t n)
   {
      return double(v == n);
   };

   auto phi = [numberOfSpanStations](size_t n)
   {
      return double(n) * UtMath::cPI / (double(numberOfSpanStations) + 1.0);
   };

   auto b = [numberOfSpanStations, phi](size_t v, size_t n)
   {
      if (v == n)
      {
         return (double(numberOfSpanStations) + 1.0) / (4.0 * sin(phi(v)));
      }
      else
      {
         return sin(phi(n)) / pow(cos(phi(n)) - cos(phi(v)), 2) * (double(1 - pow(-1, n - v)) / double(2 * (numberOfSpanStations + 1)));
      }
   };

   auto B = [numberOfSpanStations, k, b](size_t v, size_t n)
   {
      if (n == k)
      {
         return b(v, n);
      }
      else
      {
         return b(v, n) + b(v, numberOfSpanStations + 1 - n);
      }
   };

   double N1 = 0.0;
   {
      double sum = 0.0;
      for (size_t n = 1; n <= k; n++)
      {
         sum += (1.0 - del(n, k)) * B(k, n) * G(n);
      }

      N1 += G(k) * (b(k, k) * G(k) - sum);
   }

   double N2 = 0.0;
   for (size_t v = 1; v < k; v++)
   {
      double sum = 0.0;
      for (size_t n = 1; n <= k; n++)
      {
         sum += (1.0 - del(v, n)) * B(v, n) * G(n);
      }

      N2 += G(v) * (b(v, v) * G(v) - sum) * sin(phi(v));
   }

   double inducedDragIncrement = UtMath::cPI * mAeroSurface->GetAspectRatio() / (double(numberOfSpanStations) + 1.0) * (N1 + 2.0 * N2);
   return inducedDragIncrement;
}

double Designer::AeroPanel::CalcEstimatedUnflappedWaveDragCoeffDelta(double aMach)
{
   // Note: In the future, we might investigate additional effects such as sweep angle
   // ----------------------------------------------------------------------------------------------
   //double sweepLeadingEdge_rad = mAeroSurface->GetSweepAngle_deg() * UtMath::cRAD_PER_DEG;
   //double cotLeadingEdgeSweep = 1.0 / (tan(sweepLeadingEdge_rad));
   // ----------------------------------------------------------------------------------------------

   // Assuming a biconvex wing with a supersonic leading edge.
   // Corrections can be introduced for blunt-LE and subsonic-LE effects if necessary
   double compressibilityFactor = sqrt(std::abs(aMach * aMach - 1.0));
   double airfoilWaveDragFactor = 16.0 / 3.0;
   return airfoilWaveDragFactor / compressibilityFactor * pow(mAeroSurface->GetThicknessRatio(), 2);
}

double Designer::AeroPanel::CalcWaveDragCoeffDelta(double aMach, double aUndeflectedWaveDragCoeff)
{
   // Note: In the future, we might investigate additional effects such as mach and span fraction.
   // --------------------------------------------------------------------------------------------------------------------------
   //double spanFlapFraction = mSpanFractionEnd - mSpanFractionStart;
   //double areaFlapped_ft2 = GetPlanformArea_ft2() * spanFlapFraction;
   //double machConeAngle = asin(1.0 / aMach);
   //double influencedTipSpan_ft = 2.0 * GetTipChord_ft() * tan(machConeAngle) * cos(UtMath::cRAD_PER_DEG * aSurfaceAlpha_deg);
   //double areaInfluenced_ft2 = GetPlanformArea_ft2() * spanFlapFraction;
   // --------------------------------------------------------------------------------------------------------------------------

   // The DATCOM prescription for this function assumes a flat plate and breaks down
   // at 0 alpha. We use a thickness correction to avoid this singularity.

   double chordFlapFraction = mChordFractionEnd - mChordFractionStart;
   
   double incidenceAngle_rad = UtMath::cRAD_PER_DEG * mAeroSurface->GetIncidenceAngle_deg();
   double controlAngle_rad = UtMath::cRAD_PER_DEG * mControls.mCurrentControlSurfaceAngle_deg;

   if (mName.find("Left") != std::string::npos)
   {
      // Left-side surfaces are given a negated incidence angle, so account for this
      incidenceAngle_rad = -incidenceAngle_rad;
   }

   double referenceAngle = pow(incidenceAngle_rad, 2) + pow(mAeroSurface->GetThicknessRatio(), 2);
   double deflectedAngle = pow(incidenceAngle_rad + controlAngle_rad, 2) + pow(mAeroSurface->GetThicknessRatio(), 2);

   double incidenceAngleFactor = deflectedAngle / referenceAngle;

   double waveDragCoeffIncrementRatio = (1.0 - chordFlapFraction) + chordFlapFraction * incidenceAngleFactor;
   double waveDragCoeffIncrement = aUndeflectedWaveDragCoeff * (waveDragCoeffIncrementRatio - 1.0);
   return waveDragCoeffIncrement;
}

double Designer::AeroPanel::CalcProfileDragCoeffDelta()
{
   double chordFlapFraction = mChordFractionEnd - mChordFractionStart;
   double controlAngleRatio = mControls.mCurrentControlSurfaceAngle_deg / 75.0;

   double profileDragIncrementFullSpan = std::abs(controlAngleRatio) *
                                         (0.7713 * chordFlapFraction +
                                          1.2137 * pow(chordFlapFraction, 2)); // curve fit, from DATCOM Fig. 6.1.7.22

   double spanFactorFlap = CalcSubsonicSpanFactor(mSpanFractionEnd) - CalcSubsonicSpanFactor(mSpanFractionStart);
   double profileDragIncrement = profileDragIncrementFullSpan * spanFactorFlap;

   return profileDragIncrement;
}

void Designer::AeroPanel::SetAdditionalData(const std::string& aAirfoilFilename,
                                            bool               aUseExponentialAngleMapping,
                                            double             aControlSurfaceMinAngle_deg,
                                            double             aControlSurfaceMaxAngle_deg,
                                            double             aActuatorMinRate_dps,
                                            double             aActuatorMaxRate_dps,
                                            double             aActuatorMinAngle_deg,
                                            double             aActuatorMaxAngle_deg,
                                            double             aChordFractionStart,
                                            double             aChordFractionEnd,
                                            double             aSpanFractionStart,
                                            double             aSpanFractionEnd)
{
   mChordFractionStart = aChordFractionStart;
   mChordFractionEnd   = aChordFractionEnd;
   mSpanFractionStart  = aSpanFractionStart;
   mSpanFractionEnd    = aSpanFractionEnd;

   mControls.mUseExponentialAngleMapping = aUseExponentialAngleMapping;
   mControls.mControlSurfaceMinAngle_deg = aControlSurfaceMinAngle_deg;
   mControls.mControlSurfaceMaxAngle_deg = aControlSurfaceMaxAngle_deg;
   mControls.mActuatorMinRate_dps        = aActuatorMinRate_dps;
   mControls.mActuatorMaxRate_dps        = aActuatorMaxRate_dps;
   mControls.mActuatorMinAngle_deg       = aActuatorMinAngle_deg;
   mControls.mActuatorMaxAngle_deg       = aActuatorMaxAngle_deg;

   mAeroSurface->SetAdditionalData(aAirfoilFilename, aUseExponentialAngleMapping);
}
