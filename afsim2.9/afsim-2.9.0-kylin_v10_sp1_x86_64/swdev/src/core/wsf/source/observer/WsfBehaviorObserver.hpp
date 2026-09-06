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

#ifndef WSFBEHAVIOROBSERVER_HPP
#define WSFBEHAVIOROBSERVER_HPP

#include "wsf_export.h"

#include "UtCallback.hpp"
#include "WsfBehaviorTree.hpp"
class WsfSimulation;

namespace WsfObserver
{
using BehaviorTreeNodeChildrenCallback = UtCallbackListN<void(double, WsfBehaviorTreeNode*)>;
using BehaviorTreeNodeExecCallback     = UtCallbackListN<void(double, WsfBehaviorTreeNode*)>;

WSF_EXPORT BehaviorTreeNodeChildrenCallback& BehaviorTreeNodeChildren(const WsfSimulation* aSimulationPtr);
WSF_EXPORT BehaviorTreeNodeExecCallback&     BehaviorTreeNodeExec(const WsfSimulation* aSimulationPtr);
} // namespace WsfObserver

//! The implementation of the Behavior observer objects.
//! An instance of this class is maintained by the simulation.
struct WSF_EXPORT WsfBehaviorObserver
{
   //! Provide a callback to query the children of a behavior tree node when the behavior tree is first initialized.
   WsfObserver::BehaviorTreeNodeChildrenCallback BehaviorTreeNodeChildren;

   //! Provide a callback to query the behavior tree node whenever a state change occurs.
   WsfObserver::BehaviorTreeNodeExecCallback BehaviorTreeNodeExec;
};

#endif
