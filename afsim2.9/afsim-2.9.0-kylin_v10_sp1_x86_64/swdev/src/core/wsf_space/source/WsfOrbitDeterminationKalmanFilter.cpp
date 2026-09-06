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

#include "WsfOrbitDeterminationKalmanFilter.hpp"

#include <memory>
#include <string>

#include "UtCalendar.hpp"
#include "UtInput.hpp"
#include "WsfDateTime.hpp"
#include "WsfOrbitalPropagatorTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"


WsfOrbitDeterminationKalmanFilter::WsfOrbitDeterminationKalmanFilter(const WsfScenario& aScenario)
   : WsfFilter(new TrackingFilters::OrbitDeterminationKalmanFilter())
   , mScenario(aScenario)
{
}

bool WsfOrbitDeterminationKalmanFilter::ProcessInput(UtInput& aInput)
{
   std::string command   = aInput.GetCommand();
   bool        myCommand = true;
   if (command == "process_noise_model")
   {
      // This command is ignored because a ballistic noise model is only
      // used if the filter is initialized from position-only inputs;
      // the process noise models defined in the KalmanFilter base class are incorrect for orbiting satellites.
   }
   else if (command == "propagator")
   {
      std::unique_ptr<UtOrbitalPropagatorBase> statePropagatorPtr{nullptr};
      if (WsfOrbitalPropagatorTypes::Get(mScenario).LoadInstance(aInput, statePropagatorPtr))
      {
         GetFilter().SetStatePropagator(std::move(statePropagatorPtr));
         mUsingDefaultPropagator = false;
      }
   }
   else
   {
      myCommand = GetFilter().ProcessInput(aInput);
   }

   return myCommand;
}

void WsfOrbitDeterminationKalmanFilter::SetStatePropagator(std::unique_ptr<UtOrbitalPropagatorBase> aStatePropagatorPtr)
{
   GetFilter().SetStatePropagator(std::move(aStatePropagatorPtr));
}

//! Initialize the filter using the specified orbital state.
//! If a prototype propagator is specified in a "propagator" block,
//! that propagator will be cloned and set as the filter's state propagator.
//! The filter is initialized with the provided state.
bool WsfOrbitDeterminationKalmanFilter::Initialize(double aSimTime, const ut::OrbitalState& aState)
{
   return GetFilter().Initialize(aSimTime, aState);
}

const ut::OrbitalState& WsfOrbitDeterminationKalmanFilter::GetOrbitalState() const
{
   return GetFilter().GetOrbitalState();
}

bool WsfOrbitDeterminationKalmanFilter::Initialize(double           aSimTime,
                                                   WsfSimulation*   aSimulationPtr,
                                                   const UtMatrixd* aInitialStatePtr)
{
   UtCalendar epoch = aSimulationPtr->GetDateTime().GetStartDateAndTime();
   epoch.AdvanceTimeBy(aSimTime);
   return GetFilter().Initialize(aSimTime, epoch, aInitialStatePtr);
}

TrackingFilters::OrbitDeterminationKalmanFilter& WsfOrbitDeterminationKalmanFilter::GetFilter() const
{
   return *(static_cast<TrackingFilters::OrbitDeterminationKalmanFilter*>(mFilterPtr));
}
