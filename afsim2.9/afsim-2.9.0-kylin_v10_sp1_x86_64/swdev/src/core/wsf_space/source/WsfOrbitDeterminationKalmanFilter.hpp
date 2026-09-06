// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFORBITDETERMINATIONKALMANFILTER_HPP
#define WSFORBITDETERMINATIONKALMANFILTER_HPP

#include "wsf_space_export.h"

#include <memory>

class UtOrbitalPropagatorBase;
#include "OrbitDeterminationKalmanFilter.hpp"
#include "WsfFilter.hpp"
class WsfScenario;

//! Implement an OrbitDeterminationKalmanFilter as a WsfFilter.
class WSF_SPACE_EXPORT WsfOrbitDeterminationKalmanFilter : public WsfFilter
{
public:
   WsfOrbitDeterminationKalmanFilter(const WsfScenario& aScenario);
   WsfOrbitDeterminationKalmanFilter(const WsfOrbitDeterminationKalmanFilter& aSrc) = default;
   ~WsfOrbitDeterminationKalmanFilter() override                                    = default;

   bool ProcessInput(UtInput& aInput) override;

   WsfOrbitDeterminationKalmanFilter* Clone() const override { return new WsfOrbitDeterminationKalmanFilter(*this); }

   void SetStatePropagator(std::unique_ptr<UtOrbitalPropagatorBase> aStatePropagatorPtr);

   bool Initialize(double aSimTime, const ut::OrbitalState& aState);

   const ut::OrbitalState& GetOrbitalState() const;

   bool Initialize(double aSimTime, WsfSimulation* aSimulationPtr, const UtMatrixd* aInitialStatePtr) override;

   bool HyperbolicOrbitAllowed() const { return GetFilter().HyperbolicOrbitAllowed(); }

   //! Return whether a filter propagator was set during input.
   bool UsingDefaultPropagator() const { return mUsingDefaultPropagator; }

   //! Return the state propagator.  If UsingDefaultPropagator is false, the return value will be non-null;
   //! Otherwise it will be non-null after Initialize is called.
   const UtOrbitalPropagatorBase* GetStatePropagator() const { return GetFilter().GetStatePropagator(); }

private:
   TrackingFilters::OrbitDeterminationKalmanFilter& GetFilter() const;

   const WsfScenario& mScenario;
   bool               mUsingDefaultPropagator{true};
};

#endif
