// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef RELATIVEMANEUVERSTATE_HPP
#define RELATIVEMANEUVERSTATE_HPP

#include <memory>

//! An abstract class defining the interface for the states of a relative maneuver.
class RelativeManeuverState
{
public:
   RelativeManeuverState()          = default;
   virtual ~RelativeManeuverState() = default;

   //! This method provides the details of the individual relative maneuver
   //! state. It is called into from the owning maneuver, and this method should
   //! modify the control state of the associated mover as needed. If the
   //! evaluation determines that this should enter into a new state, that state
   //! is returned.
   virtual std::unique_ptr<RelativeManeuverState> Evaluate() = 0;

   //! This method returns the interval between evaluations for this state.
   virtual double GetEvaluationInterval() const = 0;
};

#endif // RELATIVEMANEUVERSTATE_HPP
