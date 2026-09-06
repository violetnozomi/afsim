// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTRUNGEKUTTA_HPP
#define UTRUNGEKUTTA_HPP

namespace ut
{

//! Take an integration step using 2nd order Runge-Kutta.
//!
//! This method steps the system of differential equations represented by the given
//! @p aRightHandSide from the given @p aInitialState forward in time by the
//! given @p aDeltaT. This method requires the specification of two template
//! parameters. Note that while the arguments speak of time as the independent
//! variable, this method is general, and so @p aDeltaT represents that variable,
//! even if the name of the formal argument refers to time.
//!
//! The DynamicState is an object that contains all the needed state
//! information about the dynamics in question. This will contain not only the
//! integration variables, but any supporting data used in the computation of the
//! right hand side of the system dynamics. This type must support an Advance method,
//! taking a taking a time, and a KinematicState giving the results of the right hand
//! side calculation.
//!
//! The DynamicState arugments are passed by non-const reference because some clients
//! might save off intermediate values during the computation of the right hand side.
//! Instead of this routine forcing the DynamicState to apply mutable to members,
//! the approach is to pass a non-const reference.
//!
//! The KinematicState is an object that contains the right hand side values of
//! the system of differential equations modeling the dynamics of interest. This
//! type must support scaling by a double value, and addition, making the type
//! act as a vector space (even if the implementation is not in the form of a
//! traditional vector). The KinematicState is the output of the right hand side
//! function, and is used as input to the DynamicState::Advance method.
//!
//! The right hand side computation for each substep will performed by the provided
//! @p aRightHandSide callable entity. This entity will need to be invokable with
//! the signature KinematicState(DynamicState&).
//!
//! \param aDeltaT        The amount of time to advance the system forward in time.
//! \param aInitialState  The initial state from which the system will be advanced a single step.
//! \param aRightHandSide A callable entity that computes the derivatives of the integration variables.
//! \returns              The state of the system advanced forward a single RK2 step.
template<typename DynamicState, typename KinematicState, typename Callable>
DynamicState RK2_Step(double aDeltaT, DynamicState& aInitialState, Callable&& aRightHandSide)
{
   KinematicState k1 = aRightHandSide(aInitialState);
   DynamicState   x1 = aInitialState.Advance(aDeltaT / 2.0, k1);
   KinematicState k2 = aRightHandSide(x1);
   return aInitialState.Advance(aDeltaT, k2);
}

//! Take an integration step using 4th order Runge-Kutta.
//!
//! This method steps the system of differential equations represented by the given
//! @p aRightHandSide from the given @p aInitialState forward in time by the
//! given @p aDeltaT. This method requires the specification of two template
//! parameters. Note that while the arguments speak of time as the independent
//! variable, this method is general, and so @p aDeltaT represents that variable,
//! even if the name of the formal argument refers to time.
//!
//! The DynamicState is an object that contains all the needed state
//! information about the dynamics in question. This will contain not only the
//! integration variables, but any supporting data used in the computation of the
//! right hand side of the system dynamics. This type must support an Advance method,
//! taking a taking a time, and a KinematicState giving the results of the right hand
//! side calculation.
//!
//! The DynamicState arugments are passed by non-const reference because some clients
//! might save off intermediate values during the computation of the right hand side.
//! Instead of this routine forcing the DynamicState to apply mutable to members,
//! the approach is to pass a non-const reference.
//!
//! The KinematicState is an object that contains the right hand side values of
//! the system of differential equations modeling the dynamics of interest. This
//! type must support scaling by a double value, and addition, making the type
//! act as a vector space (even if the implementation is not in the form of a
//! traditional vector). The KinematicState is the output of the right hand side
//! function, and is used as input to the DynamicState::Advance method.
//!
//! The right hand side computation for each substep will performed by the provided
//! @p aRightHandSide callable entity. This entity will need to be invokable with
//! the signature KinematicState(DynamicState&).
//!
//! \param aDeltaT        The amount of time to advance the system forward in time.
//! \param aInitialState  The initial state from which the system will be advanced a single step.
//! \param aRightHandSide A callable entity that computes the derivatives of the integration variables.
//! \returns              The state of the system advanced forward a single RK4 step.
template<typename DynamicState, typename KinematicState, typename Callable>
DynamicState RK4_Step(double aDeltaT, DynamicState& aInitialState, Callable&& aRightHandSide)
{
   KinematicState k1    = aRightHandSide(aInitialState);
   DynamicState   x1    = aInitialState.Advance(aDeltaT / 2.0, k1);
   KinematicState k2    = aRightHandSide(x1);
   DynamicState   x2    = aInitialState.Advance(aDeltaT / 2.0, k2);
   KinematicState k3    = aRightHandSide(x2);
   DynamicState   x3    = aInitialState.Advance(aDeltaT, k3);
   KinematicState k4    = aRightHandSide(x3);
   KinematicState kFull = k1 * (1.0 / 6.0) + k2 * (2.0 / 6.0) + k3 * (2.0 / 6.0) + k4 * (1.0 / 6.0);
   return aInitialState.Advance(aDeltaT, kFull);
}

} // namespace ut

#endif // UTRUNGEKUTTA_HPP
