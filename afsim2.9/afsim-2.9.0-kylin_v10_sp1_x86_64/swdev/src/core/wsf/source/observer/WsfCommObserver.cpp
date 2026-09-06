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

#include "WsfCommObserver.hpp"

#include "WsfSimulation.hpp"

WSF_OBSERVER_CALLBACK_DEFINE(Comm, CommFrequencyChanged)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, CommTurnedOff)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, RouterTurnedOff)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, CommTurnedOn)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, RouterTurnedOn)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, CommNonOperational)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, RouterNonOperational)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, CommOperational)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, RouterOperational)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, CommBroken)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, RouterBroken)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, MessageDeliveryAttempt)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, MessageDiscarded)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, MessageHop)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, MessageFailedRouting)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, MessageQueued)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, MessageReceived)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, MessageTransmitted)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, MessageTransmitEnded)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, MessageTransmittedHeartbeat)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, MessageUpdated)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, CommAddedToManager)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, CommRemovedFromManager)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, LinkAddedToManager)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, LinkRemovedFromManager)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, LinkEnabledOnManager)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, LinkDisabledOnManager)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, CommAddedToLocal)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, CommRemovedFromLocal)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, LinkAddedToLocal)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, LinkRemovedFromLocal)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, LinkEnabledOnLocal)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, LinkDisabledOnLocal)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, NetworkAdded)
WSF_OBSERVER_CALLBACK_DEFINE(Comm, NetworkRemoved)
