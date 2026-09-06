// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
//****************************************************************************
// Updated by Infoscitex, a DCS Company
//****************************************************************************

#include "WsfMTT_Interface.hpp"

#include "wsf_mtt_export.h"

#include "MTT.hpp"
#include "MTT_ActiveTrack.hpp"
#include "MTT_CandidateTrack.hpp"
#include "MTT_EmbryonicTrack.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtStringUtil.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfCSV_EventOutput.hpp"
#include "WsfCorrelationStrategy.hpp"
#include "WsfCorrelationStrategyTypes.hpp"
#include "WsfEventOutput.hpp"
#include "WsfEventUtils.hpp"
#include "WsfException.hpp"
#include "WsfFusionStrategy.hpp"
#include "WsfFusionStrategyTypes.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMTT_Correlation.hpp"
#include "WsfMTT_EventResults.hpp"
#include "WsfMTT_Fusion.hpp"
#include "WsfMTT_Observer.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenarioExtension.hpp"
#include "WsfSimulation.hpp"
#include "WsfSystemLog.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackManager.hpp"

namespace
{
class WsfMTT_Extension : public WsfScenarioExtension
{
public:
   WsfMTT_Extension()
      : mData(nullptr)
   {
   }
   ~WsfMTT_Extension() override { delete mData; }

   void AddedToScenario() override
   {
      mData = new WsfMTT_InterfaceData(GetScenario());
      WsfCorrelationStrategyTypes::Get(GetScenario()).Add("mtt", ut::make_unique<WsfMTT_Correlation>());
      WsfFusionStrategyTypes::Get(GetScenario()).Add("mtt", ut::make_unique<WsfMTT_Fusion>(GetScenario()));
   }

   bool ProcessInput(UtInput& aInput) override { return mData->ProcessInput(aInput); }

   void SimulationCreated(WsfSimulation& aSimulation) override
   {
      aSimulation.RegisterExtension("mtt_observer", ut::make_unique<WsfMTT_Observer>());
      aSimulation.RegisterExtension(GetExtensionName(), ut::make_unique<WsfMTT_Interface>(*mData));
   }

   WsfMTT_InterfaceData* mData;
};

void Register_event_output(wsf::event::output::SimulationExtension& aEventOutput)
{
   WsfSimulation* simPtr = &aEventOutput.GetSimulation();

   using namespace wsf::event;
   aEventOutput.AddEvent<ActiveTrackInitiated>("MTT_ACTIVE_TRACK_INITIATED", WsfObserver::ActiveTrackInitiated(simPtr));
   aEventOutput.AddEvent<ActiveTrackDropped>("MTT_ACTIVE_TRACK_DROPPED", WsfObserver::ActiveTrackDropped(simPtr));
   aEventOutput.AddEvent<CandidateTrackInitiated>("MTT_CANDIDATE_TRACK_INITIATED",
                                                  WsfObserver::CandidateTrackInitiated(simPtr));
   aEventOutput.AddEvent<CandidateTrackDropped>("MTT_CANDIDATE_TRACK_DROPPED", WsfObserver::CandidateTrackDropped(simPtr));
   aEventOutput.AddEvent<EmbryonicTrackInitiated>("MTT_EMBRYONIC_TRACK_INITIATED",
                                                  WsfObserver::EmbryonicTrackInitiated(simPtr));
   aEventOutput.AddEvent<EmbryonicTrackDropped>("MTT_EMBRYONIC_TRACK_DROPPED", WsfObserver::EmbryonicTrackDropped(simPtr));
}
} // namespace

void WSF_MTT_EXPORT Register_wsf_mtt(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("wsf_mtt"))
   {
      aApplication.RegisterFeature("mtt", "wsf_mtt"); // Indicate the feature is present
      aApplication.RegisterExtension("wsf_mtt", ut::make_unique<WsfDefaultApplicationExtension<WsfMTT_Extension>>());
   }
}

WsfMTT_InterfaceData::WsfMTT_InterfaceData(WsfScenario& aScenario)
   : mScenarioPtr(&aScenario)
   , mUserReferencePointDefined(false)
   , mReferencePointDefined(false)
   , mReferencePoint()
{
}

WsfMTT_Interface::WsfMTT_Interface(const WsfMTT_InterfaceData& aData)
   : WsfMTT_InterfaceData(aData)
   , mCallbacks()
{
}

// virtual
WsfMTT_Interface::~WsfMTT_Interface()
{
   SupBlock::ResetState();
}

void WsfMTT_Interface::AddedToSimulation()
{
   // If the event_output extension is available, hook in the handlers for our events.
   WsfEventOutput* eventOutputPtr = WsfEventOutput::Find(GetSimulation());
   if (eventOutputPtr != nullptr)
   {
      Register_event_output(*eventOutputPtr);
   }

   // If the csv_event_output extension is available, hook in the handlers for our events.
   WsfCSV_EventOutput* csvEventOutputPtr = WsfCSV_EventOutput::Find(GetSimulation());
   if (csvEventOutputPtr != nullptr)
   {
      WsfCSV_EventOutputData::AddDataTags(
         "MTT_ACTIVE_TRACK_INITIATED",
         {"time<time>", "event<string>", "platform<string>", "track_id<int>", "candidate_track_id<int>"});
      WsfCSV_EventOutputData::AddDataTags("MTT_ACTIVE_TRACK_DROPPED",
                                          {"time<time>", "event<string>", "platform<string>", "track_id<int>"});
      WsfCSV_EventOutputData::AddDataTags(
         "MTT_CANDIDATE_TRACK_INITIATED",
         {"time<time>", "event<string>", "platform<string>", "track_id<int>", "embryonic_track_id<int>"});
      WsfCSV_EventOutputData::AddDataTags("MTT_CANDIDATE_TRACK_DROPPED",
                                          {"time<time>", "event<string>", "platform<string>", "track_id<int>"});
      WsfCSV_EventOutputData::AddDataTags("MTT_EMBRYONIC_TRACK_INITIATED",
                                          {"time<time>", "event<string>", "platform<string>", "track_id<int>"});
      WsfCSV_EventOutputData::AddDataTags("MTT_EMBRYONIC_TRACK_DROPPED",
                                          {"time<time>", "event<string>", "platform<string>", "track_id<int>"});
      Register_event_output(*csvEventOutputPtr);
   }
}

bool WsfMTT_Interface::Initialize()
{
   // A reference point is defined if the user defined one. If they didn't define one then
   // it will be set to the location of the first platform that calls the tracker.
   mReferencePointDefined = mUserReferencePointDefined;

   return true;
}

MTT* WsfMTT_Interface::GetMTT(WsfStringId aPlatformId)
{
   auto iter = mTrackers.mPlatformToMTT_Map.find(aPlatformId);
   if (iter != mTrackers.mPlatformToMTT_Map.end())
   {
      return iter->second.get();
   }
   return nullptr;
}

void WsfMTT_Interface::Register(std::shared_ptr<MTT> aMTT_Ptr, WsfPlatform* aPlatformPtr)
{
   mTrackers.mPlatformToMTT_Map.emplace(aPlatformPtr->GetNameId(), aMTT_Ptr);

   // If necessary use this platform as a bootstrap to find the scenario center (potentially, very bad...)
   if (!mReferencePointDefined)
   {
      double lat, lon, alt;
      aPlatformPtr->GetLocationLLA(lat, lon, alt);
      mReferencePoint.SetReferenceLocationLLA(lat, lon, 0.0);
      mReferencePointDefined = true;
      auto logger            = ut::log::info() << "Multi-Target Tracker";
      logger.AddNote() << "Scenario Center Defined to be at LLA: " << lat << " " << lon << " " << alt;
      logger.AddNote()
         << "To change this location, specify a reference point for the MTT in an input file. (See documentation.)";
   }
}

// virtual
bool WsfMTT_InterfaceData::ProcessInput(UtInput& aInput)
{
   bool        processed = false;
   std::string command   = aInput.GetCommand();
   if (command == "mtt_interface")
   {
      processed = true;
      UtInputBlock block(aInput, "end_mtt_interface");
      while (block.ReadCommand())
      {
         command = block.GetCommand();

         if ((command == "reference_point") || (command == "reference_location"))
         {
            double refLat;
            double refLon;
            aInput.ReadValueOfType(refLat, UtInput::cLATITUDE);
            aInput.ReadValueOfType(refLon, UtInput::cLONGITUDE);
            mReferencePoint.SetReferenceLocationLLA(refLat, refLon, 0.0);
            mUserReferencePointDefined = true;
            mReferencePointDefined     = true;
         }
         else
         {
            throw(UtInput::UnknownCommand(aInput));
         }
      }
   }
   return processed;
}

WsfMTT_Interface* WsfMTT_Interface::Find(const WsfSimulation& aSimulation)
{
   return static_cast<WsfMTT_Interface*>(aSimulation.FindExtension("wsf_mtt"));
}
