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

#ifndef WSFTASKOBSERVER_HPP
#define WSFTASKOBSERVER_HPP

#include "wsf_export.h"

#include "UtCallback.hpp"
class WsfSimulation;
#include "WsfTask.hpp"
class WsfTrack;

namespace WsfObserver
{
using TaskAcceptedCallback  = UtCallbackListN<void(double, const WsfTask*, const WsfTrack*)>;
using TaskAssignedCallback  = UtCallbackListN<void(double, const WsfTask*, const WsfTrack*)>;
using TaskCanceledCallback  = UtCallbackListN<void(double, const WsfTask*)>;
using TaskCompletedCallback = UtCallbackListN<void(double, const WsfTask*, WsfStringId)>;

WSF_EXPORT WsfObserver::TaskAcceptedCallback& TaskAccepted(const WsfSimulation* aSimulation);
WSF_EXPORT WsfObserver::TaskAssignedCallback& TaskAssigned(const WsfSimulation* aSimulation);
WSF_EXPORT WsfObserver::TaskCanceledCallback& TaskCanceled(const WsfSimulation* aSimulation);
WSF_EXPORT WsfObserver::TaskCompletedCallback& TaskCompleted(const WsfSimulation* aSimulation);
} // namespace WsfObserver

//! The implementation of the Task observer objects.
//! An instance of this class is maintained by the simulation.
struct WSF_EXPORT WsfTaskObserver
{
   //! A task has been accepted by a subordinate.
   WsfObserver::TaskAcceptedCallback TaskAccepted;

   //! A task has been assigned to a subordinate.
   WsfObserver::TaskAssignedCallback TaskAssigned;

   //! A task assignment has been canceled.
   WsfObserver::TaskCanceledCallback TaskCanceled;

   //! An assigned task has been completed.
   WsfObserver::TaskCompletedCallback TaskCompleted;
};

#endif
