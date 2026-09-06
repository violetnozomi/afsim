// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSpaceObserver.hpp"

#include "WsfSimulation.hpp"

#define WSF_SPACE_OBSERVER_CALLBACK_DEFINE(EVENT)                                                    \
   WsfObserver::EVENT##Callback& WsfObserver::EVENT(const WsfSimulation* aSimulationPtr)             \
   {                                                                                                 \
      return static_cast<WsfSpaceObserver*>(aSimulationPtr->FindExtension("space_observer"))->EVENT; \
   }

WSF_SPACE_OBSERVER_CALLBACK_DEFINE(EclipseEntry)
WSF_SPACE_OBSERVER_CALLBACK_DEFINE(EclipseExit)
WSF_SPACE_OBSERVER_CALLBACK_DEFINE(OrbitColorChanged)
WSF_SPACE_OBSERVER_CALLBACK_DEFINE(OrbitDeterminationInitiated)
WSF_SPACE_OBSERVER_CALLBACK_DEFINE(OrbitDeterminationUpdated)
WSF_SPACE_OBSERVER_CALLBACK_DEFINE(OrbitalManeuverInitiated)
WSF_SPACE_OBSERVER_CALLBACK_DEFINE(OrbitalManeuverUpdated)
WSF_SPACE_OBSERVER_CALLBACK_DEFINE(OrbitalManeuverCompleted)
WSF_SPACE_OBSERVER_CALLBACK_DEFINE(OrbitalManeuverCanceled)
WSF_SPACE_OBSERVER_CALLBACK_DEFINE(StagingOperationPerformed)
