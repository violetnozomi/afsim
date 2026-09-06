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

#include "WsfSimulationObserver.hpp"

#include "WsfSimulation.hpp"

WSF_OBSERVER_CALLBACK_DEFINE(Simulation, AdvanceTime)
WSF_OBSERVER_CALLBACK_DEFINE(Simulation, ApplicationExit)
WSF_OBSERVER_CALLBACK_DEFINE(Simulation, ApplicationInitialize)
WSF_OBSERVER_CALLBACK_DEFINE(Simulation, FrameComplete)
WSF_OBSERVER_CALLBACK_DEFINE(Simulation, FramePlatformsUpdated)
WSF_OBSERVER_CALLBACK_DEFINE(Simulation, FrameStarting)
WSF_OBSERVER_CALLBACK_DEFINE(Simulation, Initialize)
WSF_OBSERVER_CALLBACK_DEFINE(Simulation, RequestTimeAdvance)
WSF_OBSERVER_CALLBACK_DEFINE(Simulation, SimulationClockRateChange)
WSF_OBSERVER_CALLBACK_DEFINE(Simulation, SimulationInitializing)
WSF_OBSERVER_CALLBACK_DEFINE(Simulation, SimulationPendingStart)
WSF_OBSERVER_CALLBACK_DEFINE(Simulation, SimulationStarting)
WSF_OBSERVER_CALLBACK_DEFINE(Simulation, SimulationPausing)
WSF_OBSERVER_CALLBACK_DEFINE(Simulation, SimulationResuming)
WSF_OBSERVER_CALLBACK_DEFINE(Simulation, SimulationComplete)
WSF_OBSERVER_CALLBACK_DEFINE(Simulation, SimulationTimeBehind)
