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

#ifndef TRACKING_FILTERS_ALPHABETAFILTER_HPP
#define TRACKING_FILTERS_ALPHABETAFILTER_HPP

#include "tracking_filters_export.h"

#include "Filter.hpp"
#include "UtEntity.hpp"
class UtInput;
#include "UtMatrix.hpp"
class UtMeasurementData;
#include "UtVec3.hpp"

namespace TrackingFilters
{
/** A specialization of Filter that implements an alpha-beta filter. */

class TRACKING_FILTERS_EXPORT AlphaBetaFilter : public Filter
{
public:
   AlphaBetaFilter();
   AlphaBetaFilter(const AlphaBetaFilter& aSrc);
   ~AlphaBetaFilter() override = default;

   Filter* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;

   void Reset(double aSimTime, const UtMatrixd* aInitialStatePtr = nullptr) override;

   void Update(double aSimTime, const UtMeasurementData& aDataIn, UtMeasurementData& aDataOut) override;

   void NoDetectUpdate(double aSimTime, UtMeasurementData& aDataOut) override;

   void GetKinematicEstimate(double aSimTime, UtMeasurementData& aDataOut) override;

   bool IsStable() const override;

   void SetGains(double aAlpha, double aBeta);

protected:
   virtual void InitializeDebug();
   virtual void OutputDebug(double aSimTime, const UtMeasurementData& aDataIn, UtMeasurementData& aDataOut);

   double mAlpha; //!< The filter parameter alpha
   double mBeta;  //!< The filter parameter beta

   int mUpdateCount; //!< The number times Update has been called.

   double mLastUpdateTime; //!< The simulation time when Update was last called.

   double mFilteredLocation[3]; //!< The filtered location from the last call to Update
   double mFilteredVelocity[3]; //!< The filtered velocity from the last call to Update
   double mPFiltVel[3];         //!< The previous filtered velocity

   double mPredictedLocation[3]; //!< The previous/next predicted location.

private:
   void UpdateStateTransitionMatrix(double aDt);

   UtMatrixd mStateTransitionMatrix; //!< Target Dynamics Model Matrix
   UtMatrixd mLastFilteredState;     //!< 6-Element State Vector from the Last Update
   bool      mDebugFlag;             //!< Debug Control Flag
};

} // namespace TrackingFilters

#endif
