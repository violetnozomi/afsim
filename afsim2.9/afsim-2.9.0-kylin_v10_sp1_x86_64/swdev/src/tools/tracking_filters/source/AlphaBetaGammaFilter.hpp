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

#ifndef TRACKING_FILTERS_ALPHABETAGAMMAFILTER_HPP
#define TRACKING_FILTERS_ALPHABETAGAMMAFILTER_HPP

#include "tracking_filters_export.h"

#include "Filter.hpp"
#include "UtEntity.hpp"
class UtInput;
#include "UtMatrix.hpp"
#include "UtVec3.hpp"

namespace TrackingFilters
{
/** A specialization of Filter that implements an alpha-beta-gamma filter. */

class TRACKING_FILTERS_EXPORT AlphaBetaGammaFilter : public Filter
{
public:
   AlphaBetaGammaFilter();
   AlphaBetaGammaFilter(const AlphaBetaGammaFilter& aSrc);
   ~AlphaBetaGammaFilter() override = default;

   Filter* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;

   void Reset(double aSimTime, const UtMatrixd* aInitialStatePtr = nullptr) override;

   void Update(double aSimTime, const UtMeasurementData& aDataIn, UtMeasurementData& aDataOut) override;

   void NoDetectUpdate(double aSimTime, UtMeasurementData& aDataOut) override;

   void GetKinematicEstimate(double aSimTime, UtMeasurementData& aDataOut) override;

   bool IsStable() const override;

   void SetGains(double aAlpha, double aBeta, double aGamma);

protected:
   virtual void InitializeDebug();
   virtual void OutputDebug(double aSimTime, const UtMeasurementData& aDataIn, UtMeasurementData& aDataOut);

private:
   void UpdateStateTransitionMatrix(double dt);

   double    mAlpha;                 //!< The filter parameter alpha
   double    mBeta;                  //!< The filter parameter beta
   double    mGamma;                 //!< The filter parameter gamma
   double    mLastUpdateTime;        //!< The simulation time when Update was last called.
   int       mUpdateCount;           //!< The number times Update has been called.
   double    mFilterFirstPassTime;   //!< Sim Time of the 1st Pass Thru the Filter
   UtMatrixd mStateTransitionMatrix; //!< Target Dynamics Model matrix
   UtMatrixd mLastFilteredState;     //!< 6-Element State Vector from the Last Update
   bool      mDebugFlag;             //!< Debug Control

   double mPMeasLocation[3];  //!< Storage Array for Last time's measured position
   double mP2MeasLocation[3]; //!< Storage Array for Last-Last time's measured position
   double mPFiltVel[3];       //!< Storage Array for Last time's measured velocity
   double mPFiltAcc[3];       //!< Storage Array for Last time's measured acceleration

   double mFilteredLocation[3];     //!< The filtered location from the last call to Update
   double mFilteredVelocity[3];     //!< The filtered velocity from the last call to Update
   double mFilteredAcceleration[3]; //!< The filtered acceleration from the last call to Update

   double mPredictedLocation[3]; //!< The previous/next predicted location.
};
} // namespace TrackingFilters
#endif
