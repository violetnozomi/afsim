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

#ifndef WSFMTT_OBSERVER_HPP
#define WSFMTT_OBSERVER_HPP

#include "wsf_mtt_export.h"

class MTT_ActiveTrack;
class MTT_CandidateTrack;
class MTT_EmbryonicTrack;

#include "UtCallback.hpp"
#include "WsfSimulationExtension.hpp"
class WsfTrackManager;

namespace WsfObserver
{
using ActiveTrackInitiatedCallback =
   UtCallbackListN<void(double, WsfTrackManager*, MTT_ActiveTrack*, MTT_CandidateTrack*)>;
using ActiveTrackDroppedCallback = UtCallbackListN<void(double, WsfTrackManager*, MTT_ActiveTrack*)>;
using CandidateTrackInitiatedCallback =
   UtCallbackListN<void(double, WsfTrackManager*, MTT_CandidateTrack*, MTT_EmbryonicTrack*)>;
using CandidateTrackDroppedCallback   = UtCallbackListN<void(double, WsfTrackManager*, MTT_CandidateTrack*)>;
using EmbryonicTrackInitiatedCallback = UtCallbackListN<void(double, WsfTrackManager*, MTT_EmbryonicTrack*)>;
using EmbryonicTrackDroppedCallback   = UtCallbackListN<void(double, WsfTrackManager*, MTT_EmbryonicTrack*)>;

WSF_MTT_EXPORT ActiveTrackInitiatedCallback&    ActiveTrackInitiated(const WsfSimulation* aSimulationPtr);
WSF_MTT_EXPORT ActiveTrackDroppedCallback&      ActiveTrackDropped(const WsfSimulation* aSimulationPtr);
WSF_MTT_EXPORT CandidateTrackInitiatedCallback& CandidateTrackInitiated(const WsfSimulation* aSimulationPtr);
WSF_MTT_EXPORT CandidateTrackDroppedCallback&   CandidateTrackDropped(const WsfSimulation* aSimulationPtr);
WSF_MTT_EXPORT EmbryonicTrackInitiatedCallback& EmbryonicTrackInitiated(const WsfSimulation* aSimulationPtr);
WSF_MTT_EXPORT EmbryonicTrackDroppedCallback&   EmbryonicTrackDropped(const WsfSimulation* aSimulationPtr);
} // namespace WsfObserver

class WsfMTT_Observer : public WsfSimulationExtension
{
public:
   WsfObserver::ActiveTrackInitiatedCallback    ActiveTrackInitiated;
   WsfObserver::ActiveTrackDroppedCallback      ActiveTrackDropped;
   WsfObserver::CandidateTrackInitiatedCallback CandidateTrackInitiated;
   WsfObserver::CandidateTrackDroppedCallback   CandidateTrackDropped;
   WsfObserver::EmbryonicTrackInitiatedCallback EmbryonicTrackInitiated;
   WsfObserver::EmbryonicTrackDroppedCallback   EmbryonicTrackDropped;
};

#endif
