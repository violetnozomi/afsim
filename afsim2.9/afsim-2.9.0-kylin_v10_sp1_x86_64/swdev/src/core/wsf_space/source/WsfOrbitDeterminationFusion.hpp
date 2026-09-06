// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFORBITDETERMINATIONFUSION_HPP
#define WSFORBITDETERMINATIONFUSION_HPP

#include "wsf_space_export.h"

#include <vector>

class UtCalendar;
#include "UtCallbackHolder.hpp"
class UtECI_Conversion;
class UtOrbitalPropagator;
class UtOrbitalPropagatorBase;

#include "WsfEvent.hpp"
class WsfMeasurement;
#include "WsfDefaultFusion.hpp"
#include "WsfKalmanFilter.hpp"
#include "WsfOrbitDeterminationKalmanFilter.hpp"
#include "WsfTrackId.hpp"
class WsfTrack;

//! An implementation of WsfFusionStratgey for orbit determination from sensor measurements.
class WSF_SPACE_EXPORT WsfOrbitDeterminationFusion : public WsfDefaultFusion
{
public:
   explicit WsfOrbitDeterminationFusion(const WsfScenario& aScenario);
   WsfOrbitDeterminationFusion(const WsfOrbitDeterminationFusion& aSrc) = default;
   WsfOrbitDeterminationFusion& operator=(const WsfOrbitDeterminationFusion&) = delete;
   ~WsfOrbitDeterminationFusion() override                                    = default;

   WsfFusionStrategy* Clone() const override { return new WsfOrbitDeterminationFusion(*this); }
   bool               ProcessInput(UtInput& aInput) override;
   bool               Initialize(WsfTrackManager* aTrackManagerPtr) override;

   bool UpdateLocalTrackFromNonLocalTrack(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack) override;
   bool UpdateLocalResultFromNonLocalResult(double                 aSimTime,
                                            WsfLocalSensorResult&  aLocalResult,
                                            const WsfSensorResult& aNonLocalResult) override
   {
      return false;
   }

   void ReviewTrack(WsfLocalTrack& aLocalTrack) override;

   static void ReviewTrack(WsfTrackManager& aTrackManager, WsfLocalTrack& aLocalTrack);

protected:
   static bool IsBearingElevation(const UtMeasurementData& aTrack);
   static bool IsLocation(const UtMeasurementData& aTrack);

private:
   bool AnglesOnlyInitialGuess(const UtVec3d& aUnitTargetVecECI_1,
                               const UtVec3d& aSiteLocECI_1,
                               const UtVec3d& aUnitTargetVecECI_2,
                               const UtVec3d& aSiteLocECI_2,
                               double         aDt,
                               UtVec3d&       aLocationECI_1,
                               UtVec3d&       aLocationECI_2);

   using MeasurementList = std::vector<const WsfMeasurement*>;

   bool AnglesOnlyKinematicSolution(const MeasurementList& aData,
                                    std::vector<UtVec3d>&  aLocECI,
                                    std::vector<UtVec3d>&  aVelECI);
   void ComputeVelocities(const MeasurementList& aData, const std::vector<UtVec3d>& aLocECI, std::vector<UtVec3d>& aVelECI);

   void ComputeCircularLocationsAndSpeeds(const UtVec3d& aUnitTargetVecECI_1,
                                          const UtVec3d& aSiteLocECI_1,
                                          const UtVec3d& aUnitTargetVecECI_2,
                                          const UtVec3d& aSiteLocECI_2,
                                          double         aDt,
                                          double         aRadius,
                                          UtVec3d&       aLocationECI_1,
                                          UtVec3d&       aLocationECI_2,
                                          double&        aGeometricSpeed,
                                          double&        aGravitationalSpeed);
   void ComputeLambertf_g(const UtVec3d& aLocECI, const UtVec3d& aVelECI, double aDeltaT, double& aF, double& aG);

   bool CreateFilterOnTrack(double aSimTime, WsfLocalTrack& aLocalTrack, UtOrbitalPropagatorBase& aPropagator);

   void ConvertBearingElevation(const UtMeasurementData& aBearingElevationTrack,
                                UtVec3d&                 aUnitTargetVecECI,
                                UtVec3d&                 aSiteLocationECI);

   void ConvertLocation(const UtECI_Conversion& aConversion, const UtMeasurementData& aLocationTrack, UtVec3d& aTargetLocECI);

   MeasurementList GetFusionCandidates(WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack, bool aGetAnglesCandidates);

   bool FuseInitialLocations(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack);

   bool FuseInitialAngles(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack);

   void GetSimCalendarTime(double aSimTime, UtCalendar& aSimCalendarTime);

   bool UpdateTrackFromEstimate(double aSimTime, WsfLocalTrack& aLocalTrack, UtMeasurementData& aEstimate);

   void PrintAnglesOnlyData(const WsfTrack& aTrack,
                            const UtVec3d&  aLocationECI,
                            const UtVec3d&  aVelocityECI,
                            double          aRange) const;

   UtOrbitalPropagatorBase* GetPropagator(const WsfTrack& aTrack);

   UtOrbitalPropagatorBase* AddPropagator(WsfTrack& aTrack);

   const ut::CentralBody& GetCentralBody() const;

   WsfOrbitDeterminationKalmanFilter mPrototypeFilter;
   unsigned                          mNumberOfAnglesMeasurementsNeeded;
   double                            mLambertConvergenceTolerance;
   double                            mRangeErrorFactor;
   unsigned                          mAnglesOnlyMaxIterations;
   double                            mAnglesOnlyLinearTolerance;
   bool                              mDebug;
};

#endif
