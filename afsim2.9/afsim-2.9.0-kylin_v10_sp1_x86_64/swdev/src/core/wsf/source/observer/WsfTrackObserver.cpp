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

#include "WsfTrackObserver.hpp"

#include "WsfSimulation.hpp"

WSF_OBSERVER_CALLBACK_DEFINE(Track, CorrelationStrategyChanged)
WSF_OBSERVER_CALLBACK_DEFINE(Track, FusionStrategyChanged)
WSF_OBSERVER_CALLBACK_DEFINE(Track, LocalTrackCorrelation)
WSF_OBSERVER_CALLBACK_DEFINE(Track, LocalTrackDecorrelation)
WSF_OBSERVER_CALLBACK_DEFINE(Track, LocalTrackDropped)
WSF_OBSERVER_CALLBACK_DEFINE(Track, LocalTrackInitiated)
WSF_OBSERVER_CALLBACK_DEFINE(Track, LocalTrackUpdated)
WSF_OBSERVER_CALLBACK_DEFINE(Track, SensorTrackCoasted)
WSF_OBSERVER_CALLBACK_DEFINE(Track, SensorTrackDropped)
WSF_OBSERVER_CALLBACK_DEFINE(Track, SensorTrackInitiated)
WSF_OBSERVER_CALLBACK_DEFINE(Track, SensorTrackUpdated)
