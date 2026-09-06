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

#include "WsfProcessorObserver.hpp"

#include "WsfSimulation.hpp"

WSF_OBSERVER_CALLBACK_DEFINE(Processor, OperatingLevelChanged)
WSF_OBSERVER_CALLBACK_DEFINE(Processor, ProcessorTurnedOff)
WSF_OBSERVER_CALLBACK_DEFINE(Processor, ProcessorTurnedOn)
WSF_OBSERVER_CALLBACK_DEFINE(Processor, ProcessorNonOperational)
WSF_OBSERVER_CALLBACK_DEFINE(Processor, ProcessorOperational)
WSF_OBSERVER_CALLBACK_DEFINE(Processor, ProcessorBroken)
WSF_OBSERVER_CALLBACK_DEFINE(Processor, StateEntry)
WSF_OBSERVER_CALLBACK_DEFINE(Processor, StateExit)
