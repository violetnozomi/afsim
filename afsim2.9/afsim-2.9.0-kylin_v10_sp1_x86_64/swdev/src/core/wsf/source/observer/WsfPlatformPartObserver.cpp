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

#include "WsfPlatformPartObserver.hpp"

#include "WsfSimulation.hpp"

WSF_OBSERVER_CALLBACK_DEFINE(PlatformPart, VisualPartTurnedOn)
WSF_OBSERVER_CALLBACK_DEFINE(PlatformPart, VisualPartTurnedOff)
WSF_OBSERVER_CALLBACK_DEFINE(PlatformPart, VisualPartNonOperational)
WSF_OBSERVER_CALLBACK_DEFINE(PlatformPart, VisualPartOperational)
WSF_OBSERVER_CALLBACK_DEFINE(PlatformPart, VisualPartBroken)
