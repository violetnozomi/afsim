// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfMTT_Observer.hpp"

#include "WsfSimulation.hpp"

#define WSF_MTT_OBSERVER_CALLBACK_DEFINE(EVENT)                                                   \
   WsfObserver::EVENT##Callback& WsfObserver::EVENT(const WsfSimulation* aSimulationPtr)          \
   {                                                                                              \
      return static_cast<WsfMTT_Observer*>(aSimulationPtr->FindExtension("mtt_observer"))->EVENT; \
   }

WSF_MTT_OBSERVER_CALLBACK_DEFINE(ActiveTrackInitiated)
WSF_MTT_OBSERVER_CALLBACK_DEFINE(ActiveTrackDropped)
WSF_MTT_OBSERVER_CALLBACK_DEFINE(CandidateTrackInitiated)
WSF_MTT_OBSERVER_CALLBACK_DEFINE(CandidateTrackDropped)
WSF_MTT_OBSERVER_CALLBACK_DEFINE(EmbryonicTrackInitiated)
WSF_MTT_OBSERVER_CALLBACK_DEFINE(EmbryonicTrackDropped)
