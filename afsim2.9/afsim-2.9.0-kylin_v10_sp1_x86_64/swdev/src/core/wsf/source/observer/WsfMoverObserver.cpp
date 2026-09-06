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

#include "WsfMoverObserver.hpp"

#include "WsfSimulation.hpp"

WSF_OBSERVER_CALLBACK_DEFINE(Mover, MoverChanged)
WSF_OBSERVER_CALLBACK_DEFINE(Mover, MoverTurnedOff)
WSF_OBSERVER_CALLBACK_DEFINE(Mover, MoverTurnedOn)
WSF_OBSERVER_CALLBACK_DEFINE(Mover, MoverNonOperational)
WSF_OBSERVER_CALLBACK_DEFINE(Mover, MoverOperational)
WSF_OBSERVER_CALLBACK_DEFINE(Mover, MoverBroken)
WSF_OBSERVER_CALLBACK_DEFINE(Mover, MoverBurnedOut)
WSF_OBSERVER_CALLBACK_DEFINE(Mover, MoverStaged)
WSF_OBSERVER_CALLBACK_DEFINE(Mover, MoverUpdated)
WSF_OBSERVER_CALLBACK_DEFINE(Mover, RouteChanged)
WSF_OBSERVER_CALLBACK_DEFINE(Mover, MoverPathEnded)
