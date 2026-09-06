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

#ifndef WSFFILTER_HPP
#define WSFFILTER_HPP

#include "wsf_export.h"

#include "Filter.hpp"
#include "UtCovariance.hpp"
#include "WsfObject.hpp"
class WsfSimulation;

//! Implement a filter as an adapter.
class WSF_EXPORT WsfFilter : public WsfObject
{
public:
   WsfFilter() = delete;
   WsfFilter(TrackingFilters::Filter* aFilterPtr);
   WsfFilter(const WsfFilter& aSrc);
   WsfFilter& operator=(const WsfFilter& aRhs);
   ~WsfFilter() override;

   //! Clone this filter.
   //! @return a pointer to the new object.
   WsfFilter* Clone() const override = 0;

   bool ProcessInput(UtInput& aInput) override;

   virtual bool Initialize(double aSimTime, WsfSimulation* aSimulationPtr, const UtMatrixd* aInitialStatePtr = nullptr);

   // Adapted methods (see TrackingFilters::Filter)
   void Reset(double aSimTime, const UtMatrixd* aInitialStatePtr = nullptr);

   void Update(double aSimTime, const UtMeasurementData& aDataIn, UtMeasurementData& aDataOut);

   void NoDetectUpdate(double aSimTime, UtMeasurementData& aDataOut);

   void GetKinematicEstimate(double aSimTime, UtMeasurementData& aDataOut);

   const UtCovariance* GetStateCovariance();

   const UtCovariance* GetResidualCovariance();

   bool GetPredictedStateCovariance(double aSimTime, UtCovariance& aCovariance);

   bool GetPredictedResidualCovariance(double aSimTime, UtCovariance& aCovariance);

   bool GetCurrentMeasurementTrackScore(double& aCurrentScore);

   bool GetAverageTrackScore(double& aAverageScore);

   bool IsStable() const;

   void SetFrameTime(double aFrameTime);

protected:
   TrackingFilters::Filter* mFilterPtr;
};

#endif
