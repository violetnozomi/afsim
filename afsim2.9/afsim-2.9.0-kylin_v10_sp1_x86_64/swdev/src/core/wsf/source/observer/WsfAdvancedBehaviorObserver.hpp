// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFADVANCEDBEHAVIOROBSERVER_HPP
#define WSFADVANCEDBEHAVIOROBSERVER_HPP

#include "wsf_export.h"

#include "UtCallback.hpp"
#include "WsfAdvancedBehaviorTree.hpp"
class WsfSimulation;

namespace WsfObserver
{
using AdvancedBehaviorTreeCallback      = UtCallbackListN<void(double, WsfAdvancedBehaviorTree*)>;
using AdvancedBehaviorTreeStateCallback = UtCallbackListN<void(double, WsfAdvancedBehaviorTreeNode*)>;

WSF_EXPORT AdvancedBehaviorTreeCallback&      AdvancedBehaviorTree(const WsfSimulation* aSimulationPtr);
WSF_EXPORT AdvancedBehaviorTreeStateCallback& AdvancedBehaviorTreeState(const WsfSimulation* aSimulationPtr);
} // namespace WsfObserver

struct WSF_EXPORT WsfAdvancedBehaviorObserver
{
   // Initial callback for sending node data on behavior tree initialization.
   WsfObserver::AdvancedBehaviorTreeCallback AdvancedBehaviorTree;

   // Callback for sending the tree's state.
   WsfObserver::AdvancedBehaviorTreeStateCallback AdvancedBehaviorTreeState;
};

#endif
