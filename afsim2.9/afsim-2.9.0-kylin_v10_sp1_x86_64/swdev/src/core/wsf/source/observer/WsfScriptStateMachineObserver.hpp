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

#ifndef WSFSCRIPTSTATEMACHINEOBSERVER_HPP
#define WSFSCRIPTSTATEMACHINEOBSERVER_HPP

#include "wsf_export.h"

#include "UtCallback.hpp"

class WsfScriptStateMachine;
class WsfSimulation;

namespace WsfObserver
{
using StateMachineCallback      = UtCallbackListN<void(double, WsfScriptStateMachine*)>;
using StateMachineStateCallback = UtCallbackListN<void(double, WsfScriptStateMachine*, int, int)>;

WSF_EXPORT StateMachineCallback&      StateMachine(const WsfSimulation* aSimulationPtr);
WSF_EXPORT StateMachineStateCallback& StateMachineState(const WsfSimulation* aSimulationPtr);
} // namespace WsfObserver

struct WSF_EXPORT WsfScriptStateMachineObserver
{
   // Callback for sending the state machine layout upon state machine initialization.
   WsfObserver::StateMachineCallback StateMachine;

   // Callback for sending the state machine's current state.
   WsfObserver::StateMachineStateCallback StateMachineState;
};

#endif // WSFSCRIPTSTATEMACHINEOBSERVER_HPP
