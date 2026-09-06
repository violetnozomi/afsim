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

#include "WsfTaskObserver.hpp"

#include "WsfSimulation.hpp"

WSF_OBSERVER_CALLBACK_DEFINE(Task, TaskAccepted)
WSF_OBSERVER_CALLBACK_DEFINE(Task, TaskAssigned)
WSF_OBSERVER_CALLBACK_DEFINE(Task, TaskCanceled)
WSF_OBSERVER_CALLBACK_DEFINE(Task, TaskCompleted)
