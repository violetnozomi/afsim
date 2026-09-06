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

#include "WsfSimpleOrbitalManeuvering.hpp"

#include <algorithm>
#include <float.h>
#include <iostream>
#include <sstream>

#include "UtInputString.hpp"
#include "UtMath.hpp"
#include "UtOrbitalElements.hpp"
#include "UtOrbitalPropagator.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "UtTwoLineElement.hpp"
#include "UtVec3.hpp"
#include "WsfDateTime.hpp"
#include "WsfOrbitalManeuver.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfSpaceMover.hpp"
#include "WsfSpaceObserver.hpp"
#include "observer/WsfMoverObserver.hpp"

WsfSimpleOrbitalManeuvering::WsfSimpleOrbitalManeuvering(const WsfSimpleOrbitalManeuvering& aSrc)
   : WsfOrbitalManeuvering(aSrc)
   , mDeltaV(aSrc.mDeltaV)
   , mDeltaV_Rate(aSrc.mDeltaV_Rate)
{
}

// virtual
WsfSimpleOrbitalManeuvering::~WsfSimpleOrbitalManeuvering() {}

// virtual
bool WsfSimpleOrbitalManeuvering::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "delta_v")
   {
      aInput.ReadValueOfType(mDeltaV, UtInput::cSPEED);
   }
   else if (command == "maximum_acceleration")
   {
      aInput.ReadValueOfType(mDeltaV_Rate, UtInput::cACCELERATION);
   }
   else
   {
      myCommand = WsfOrbitalManeuvering::ProcessInput(aInput);
   }
   return myCommand;
}

//! Return how long it will take to execute a maneuver with the specified delta-V.
//!@param aDeltaV The delta-V to expend in the maneuver.
//!@return The time necessary to complete the maneuver.
double WsfSimpleOrbitalManeuvering::GetRequiredDuration(double aDeltaV) const
{
   double duration = 0.0;
   if (mDeltaV_Rate > 0.0)
   {
      duration = aDeltaV / mDeltaV_Rate;
   }
   return duration;
}

//! Return the delta-v necessary to execute a maneuver with the specified duration.
//!@param aDuration The duration for which the equivalent delta-v would  be expended.
//!@return The delta-v that would be expended to complete the maneuver.  If there is
//! insufficient delta-v WsfOrbitalManeuvering::cINFINITE_DELTA_V is returned.
double WsfSimpleOrbitalManeuvering::GetRequiredDeltaV(double aDuration) const
{
   double deltaV = 0.0;
   if (mDeltaV_Rate > 0.0)
   {
      deltaV = mDeltaV_Rate * aDuration;
   }
   return deltaV;
}

//! Return the remaining delta-V available.  The value is allowed to go negative for verification purposes.
//! Any negative value indicates no more delta-v is available.
double WsfSimpleOrbitalManeuvering::GetAvailableDeltaV() const
{
   return mDeltaV;
}

bool WsfSimpleOrbitalManeuvering::ReduceAvailableDeltaV_By(double aDeltaV)
{
   bool succeeded = false;
   if (aDeltaV <= mDeltaV)
   {
      succeeded = true;
      mDeltaV -= aDeltaV;
   }
   return succeeded;
}
