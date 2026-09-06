// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFDEFAULTFUSION_HPP
#define WSFDEFAULTFUSION_HPP

#include "wsf_export.h"

#include <memory>

#include "UtEntity.hpp"
class UtScript;
#include "UtScriptData.hpp"
class WsfMeasurement;
#include "WsfCovariance.hpp"
#include "WsfFusionStrategy.hpp"
#include "script/WsfScriptContext.hpp"

//! The wsf standard implementation of WsfFusionStratgey for track fusion.
//!\p
//! This class implements a series of standard algorithms for fusion of similar
//! and complementary information.  The one option for use of these algorithms is
//! whether to use "replacement" or "weighted average" fusion.  These choices are
//! relevant when fusing location-based state data.  For replacement fusion,
//! a track update replaces the location data, whereas for weighted-average fusion,
//! the location data are computed using the "weight" of the inverse covariance matrix.
class WSF_EXPORT WsfDefaultFusion : public WsfFusionStrategy
{
public:
   WsfDefaultFusion(const WsfScenario& aScenario, bool aIsWeightedAverage = false);
   ~WsfDefaultFusion() override = default;
   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(WsfTrackManager* aTrackManagerPtr) override;

   WsfFusionStrategy* Clone() const override { return new WsfDefaultFusion(*this); }

   bool UpdateLocalTrackFromNonLocalTrack(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack) override;
   bool UpdateLocalResultFromNonLocalResult(double                 aSimTime,
                                            WsfLocalSensorResult&  aLocalResult,
                                            const WsfSensorResult& aNonLocalResult) override;

   void        ReviewTrack(WsfLocalTrack& aLocalTrack) override;
   static void ReviewTrack(WsfTrackManager& aTrackManager, WsfLocalTrack& aLocalTrack);

   bool UpdateNonKinematicData(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack) override;

protected:
   WsfDefaultFusion(const WsfDefaultFusion& aSrc);
   WsfDefaultFusion& operator=(const WsfDefaultFusion&) = delete;

   static void ReviewIFF(WsfTrackManager& aTrackManager, WsfLocalTrack& aLocalTrack);

   static void ReviewTrackQuality(WsfTrackManager& aTrackManager, WsfLocalTrack& aLocalTrack);

   bool UpdateIFF_Status(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack) override;

   bool UpdateSide(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack) override;

   bool UpdateType(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack) override;

   bool UpdateSpatialDomain(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack) override;

   bool UpdateTrackQuality(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack) override;

   bool UpdateSignalToNoise(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack) override;

   bool UpdateSignalList(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack) override;

   bool UpdateAuxData(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack) override;

   // Set whether to use weighted average fusion (use in derived classes).
   void SetWeightedAverage(bool aIsWeightedAverage) { mIsWeightedAverage = aIsWeightedAverage; }

   void PerformMirrorTrackUpdate(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack);

   bool PerformStandardTrackUpdate(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack);

private:
   void UpdateLocation(WsfMeasurement&       aLocalMeasurement,
                       const WsfMeasurement& aNonLocalMeasurement,
                       const double          aLocationWCS[3],
                       WsfCovariance*&       aStateCovarianceMatrixPtr,
                       unsigned int          aFusedCount);

   void UpdateTrackStateEstimate(const UtMatrixd&     aStateEstimate,
                                 const WsfCovariance& aStateCovarianceMatrix,
                                 WsfMeasurement&      aMeasurement);

   static void GetEstimateFromTrack(double aSimTime, const WsfMeasurement& aMeasurement, UtMatrixd& aEstimate);

   static void SetEstimateInTrack(UtMatrixd& aEstimate, WsfMeasurement& aMeasurement);

   void FuseRangeBearingWithElevation(const WsfMeasurement& aRangeBearingMeasurement,
                                      const WsfMeasurement& aElevationMeasurement,
                                      bool                  aCreateMeasurementCovariance,
                                      double                aLocationWCS[3],
                                      WsfCovariance*&       aStateCovarianceMatrixPtr);

   void FuseLocationWithRangeBearingElevation(WsfMeasurement&       aFusedLocationMeasurement,
                                              const WsfMeasurement& aRangeBearingElevationMeasurement,
                                              bool                  aCreateMeasurementCovariance,
                                              double                aLocationWCS[3],
                                              WsfCovariance*&       aStateCovarianceMatrixPtr);

   void FuseLocationWith2D_Location(WsfMeasurement&       aFusedLocationMeasurement,
                                    const WsfMeasurement& a2D_LocationMeasurement,
                                    double                aLocationWCS[3]);

   void FuseRangeBearing(WsfMeasurement&       aFusedLocationMeasurement,
                         const WsfMeasurement& aRangeBearingMeasurement,
                         double                aLocationWCS[3]);

   void WeightedAverageFusion1D(double  aMeasurement1,
                                double  aMeasurement2,
                                double  aError1,
                                double  aError2,
                                double& aWeightedMeasurement,
                                double& aWeightedError);

   static bool UpdateHasSufficientTrackQuality(const WsfTrack& aTrackUpdate, WsfLocalTrack& aLocalTrack);

   bool                              mIsWeightedAverage;
   UtEntity                          mReferencePoint;
   std::unique_ptr<WsfScriptContext> mContextPtr;
   UtScript*                         mUpdateTypePtr;
   UtScript*                         mUpdateSidePtr;
   UtScript*                         mUpdateIFF_StatusPtr;
   UtScriptData*                     mLocalTrackVarPtr;
   UtScriptData*                     mNonLocalTrackVarPtr;
};

#endif
