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

#include "WsfExchangeObserver.hpp"

#include "WsfSimulation.hpp"

WSF_OBSERVER_CALLBACK_DEFINE(Exchange, ExchangeQueried)
WSF_OBSERVER_CALLBACK_DEFINE(Exchange, ExchangeNegotiated)
WSF_OBSERVER_CALLBACK_DEFINE(Exchange, ExchangeRequestFailed)
WSF_OBSERVER_CALLBACK_DEFINE(Exchange, ExchangeCompleted)
WSF_OBSERVER_CALLBACK_DEFINE(Exchange, PlatformCapabilityChanged)
