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

#ifndef AERO_PANEL_HPP
#define AERO_PANEL_HPP

#include "AeroMovableSurface.hpp"

namespace Designer
{
   class AeroPanel
   {
      public:
         explicit AeroPanel(AeroSurface* aParentSurface) { mAeroSurface = aParentSurface; };

         void SetName(const std::string& aName) { mName = aName; }
         const std::string& GetName() const { return mName; }

         double GetControlSurfaceMinAngle_deg() const { return mControls.mControlSurfaceMinAngle_deg; }
         double GetControlSurfaceMaxAngle_deg() const { return mControls.mControlSurfaceMaxAngle_deg; }

         double GetControlAngle_deg() const { return mControls.mCurrentControlSurfaceAngle_deg; }
         void SetControlAngle_deg(double aControlAngle_deg) { mControls.mCurrentControlSurfaceAngle_deg = aControlAngle_deg; }

         void SetChordFractionStart(double aChordFractionStart) { mChordFractionStart = aChordFractionStart; }
         void SetChordFractionEnd(double aChordFractionEnd) { mChordFractionEnd = aChordFractionEnd; }

         void SetSpanFractionStart(double aSpanFractionStart) { mSpanFractionStart = aSpanFractionStart; }
         void SetSpanFractionEnd(double aSpanFractionEnd) { mSpanFractionEnd = aSpanFractionEnd; }

         void CalcAeroCoeffDelta(double             aMach,
                                 double             aPartArea_ft2,
                                 bool               aVehicleHasWing,
                                 double             aVehicleReferenceArea_ft2,
                                 double             aVehiclePitchRefLength_ft,
                                 double             aVehicleLateralRefLength_ft,
                                 double             aVehicleAlpha_deg,
                                 double             aVehicleBeta_deg,
                                 double             aDynamicPressure_psf,
                                 double             airfoilLiftCurveSlope_perDeg,
                                 double             surfaceLiftCurveSlope_perDeg,
                                 double             surfaceMaxPosLiftCoeff,
                                 double             surfaceMaxNegLiftCoeff,
                                 double             aMachDragDivergence,
                                 double             aMachDragPeak,
                                 double             aUnflappedCL,
                                 double             aUnflappedCD,
                                 double&            aDeltaCL,
                                 double&            aDeltaCd,
                                 double&            aDeltaCY,
                                 double&            aDeltaCm,
                                 double&            aDeltaCn,
                                 double&            aDeltaCl);

         void SetAdditionalData(const std::string&    aAirfoilFilename,
                                bool                  aUseExponentialAngleMapping,
                                double                aControlSurfaceMinAngle_deg,
                                double                aControlSurfaceMaxAngle_deg,
                                double                aActuatorMinRate_dps,
                                double                aActuatorMaxRate_dps,
                                double                aActuatorMinAngle_deg,
                                double                aActuatorMaxAngle_deg,
                                double                aChordFractionStart,
                                double                aChordFractionEnd,
                                double                aSpanFractionStart,
                                double                aSpanFractionEnd);

      private:
         std::string mName = "";

         Controls mControls;

         double mChordFractionStart = 0.75;
         double mChordFractionEnd = 1.0;
         double mSpanFractionStart = 0.25;
         double mSpanFractionEnd = 0.75;

         AeroSurface* mAeroSurface;

         double CalcSubsonicAirfoilLiftCoeffDeltaSpoiler(double aMach,
                                                         double aLiftCurveSlopeAirfoil_perDeg);

         double CalcSubsonicAirfoilLiftCoeffDeltaPlainFlap(double  aMach,
                                                           double  aSurfaceLiftEfficiency,
                                                           double& aMaximumLiftCoeffIncrement);

         double CalcSupersonicLiftCoeffDeltaPlainFlap(double aMach,
                                                      double aTrailingEdgeSweep_deg,
                                                      double aMaxPosLiftCoeff,
                                                      double aMaxNegLiftCoeff,
                                                      double aUnflappedLiftCoeff);

         double CalcSupersonicLiftCoeffDeltaSpoiler(double aMach,
                                                    double aTrailingEdgeSweep_deg,
                                                    double aMaxPosLiftCoeff,
                                                    double aMaxNegLiftCoeff,
                                                    double aUnflappedLiftCoeff);

         double CalcSubsonicSpanFactor(double spanFraction);

         double CalcSubsonicLiftCoeffDelta(double aAirfoilLiftCoeffDelta,
                                           double aWingLiftCoeffMaximumDelta,
                                           double aQuarterChordSweepAngle_deg,
                                           double aSurfaceLiftEfficiency,
                                           double aMaxUnflappedPosLiftCoeff,
                                           double aMaxUnflappedNegLiftCoeff,
                                           double aUnflappedLiftCoeff);

         double CalcSpanStationLoadingDelta(double aMach,
                                            double aLiftCurveSlopeSurface_perDeg,
                                            size_t stationIndex,
                                            size_t numberOfStations);

         double CalcInducedDragCoeffDelta(double aMach,
                                          double aLiftCurveSlopeSurface_perDeg,
                                          double deviceEffectivenessFactor);

         double CalcEstimatedUnflappedWaveDragCoeffDelta(double aMach);

         double CalcWaveDragCoeffDelta(double aMach,
                                       double aUndeflectedWaveDragCoeff);

         double CalcProfileDragCoeffDelta();
   };
}

#endif