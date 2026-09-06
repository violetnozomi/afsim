// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCyberTrackProcessorEffect.hpp"

#include "UtInput.hpp"
#include "UtMemory.hpp"
#include "WsfCyberEngagement.hpp"
#include "WsfCyberTrackManager.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrackProcessor.hpp"

namespace wsf
{
namespace cyber
{

// ================================================================================================
TrackProcessorEffect::TrackProcessorEffect(WsfScenario& aScenario)
   : TrackManagerEffect(aScenario)
{
}

// ================================================================================================
TrackProcessorEffect* TrackProcessorEffect::Clone() const
{
   return new TrackProcessorEffect(*this);
}

// ================================================================================================
bool TrackProcessorEffect::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   auto command   = aInput.GetCommand();

   if (command == "target_processor")
   {
      std::string processorOrProcessorTypeName;
      aInput.ReadValue(processorOrProcessorTypeName);
      mProcessorTargets.insert(processorOrProcessorTypeName);
   }
   else if (command == "remove_target_processor")
   {
      std::string processorOrProcessorTypeName;
      aInput.ReadValue(processorOrProcessorTypeName);
      auto it = mProcessorTargets.find(processorOrProcessorTypeName);
      if (it != std::end(mProcessorTargets))
      {
         mProcessorTargets.erase(it);
      }
   }
   else if (TrackManagerEffect::ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// ================================================================================================
void TrackProcessorEffect::Attack(double aSimTime, Engagement& aEngagement)
{
   const auto& sim       = aEngagement.GetSimulation();
   auto        victimPtr = sim.GetPlatformByName(aEngagement.GetVictim());
   if (victimPtr)
   {
      InjectTrackManager(aSimTime, aEngagement);

      for (auto managerPtr : mInjectedManagers)
      {
         managerPtr->Activate();
         managerPtr->ExecuteOnEntry(aSimTime);
      }
   }
}

// ================================================================================================
void TrackProcessorEffect::Restore(double aSimTime, Engagement& aEngagement)
{
   const auto& sim       = aEngagement.GetSimulation();
   auto        victimPtr = sim.GetPlatformByName(aEngagement.GetVictim());
   if (victimPtr)
   {
      for (auto managerPtr : mInjectedManagers)
      {
         managerPtr->ExecuteOnExit(aSimTime);
         managerPtr->Deactivate();
      }
   }
}

// ================================================================================================
TrackProcessorEffect::ProcessorVector TrackProcessorEffect::GetValidProcessors(const WsfPlatform& aPlatform) const
{
   TrackProcessorEffect::ProcessorVector returnVec;

   for (WsfComponentList::RoleIterator<WsfProcessor> iter{aPlatform}; !iter.AtEnd(); ++iter)
   {
      auto trackProcessorPtr = dynamic_cast<WsfTrackProcessor*>(*iter);
      if (trackProcessorPtr)
      {
         for (const auto& processorString : mProcessorTargets)
         {
            if ((trackProcessorPtr->GetName() == processorString) || (trackProcessorPtr->IsA_TypeOf(processorString)))
            {
               returnVec.push_back(trackProcessorPtr);
               break;
            }
         }
      }
   }

   return returnVec;
}

// ================================================================================================
TrackProcessorEffect::TrackProcessorEffect(const TrackProcessorEffect& aSrc)
   : TrackManagerEffect(aSrc)
   , mProcessorTargets(aSrc.mProcessorTargets)
   , mInjectedManagers(aSrc.mInjectedManagers)
{
}

// ================================================================================================
void TrackProcessorEffect::InjectTrackManager(double aSimTime, const Engagement& aEngagement)
{
   auto victimPtr = GetValidTarget(aEngagement);
   if (victimPtr)
   {
      auto processors = GetValidProcessors(*victimPtr);
      for (auto processorPtr : processors)
      {
         //! If the processor is using the master track processor, located on the
         //! platform as a component entry, then this effect (for this processor)
         //! is equivalent to the base InjectTrackManager method.
         if (processorPtr->IsMasterTrackManager() && !GetInjectedManager())
         {
            TrackManagerEffect::InjectTrackManager(aSimTime, aEngagement);
            auto targetManagerPtr = GetInjectedManager();
            if (!targetManagerPtr)
            {
               throw UtException("Error in injection of master track manager in cyber track processor effect.");
            }
            mInjectedManagers.push_back(targetManagerPtr);
         }
         else
         {
            //! The target processor is using a non-master, local track manager.
            // auto trackManagerPtr = &processorPtr->GetTrackManager();
            std::unique_ptr<WsfTrackManager> trackManagerPtr(&processorPtr->GetTrackManager());
            auto cyberTrackManager = ut::make_unique<TrackManager>(std::move(trackManagerPtr),
                                                                   GetContext(),
                                                                   GetOnEntry(),
                                                                   GetOnExit(),
                                                                   GetOnTrack(),
                                                                   GetOnRawTrack(),
                                                                   GetOnTrackDrop(),
                                                                   GetTrackPurging(),
                                                                   GetRawTrackPurging(),
                                                                   GetTrackHistoryPurging());

            cyberTrackManager->SetComponentParent(victimPtr);

            //! Immediately notify observers. The callback releases ownership of this object
            //! to the WsfTrackProcessor.
            mInjectedManagers.push_back(cyberTrackManager.get());
            mInjectedManagers.back()->NotifyOfTrackManagerChanged(aSimTime, cyberTrackManager.release());

            if (!mInjectedManagers.back()->Initialize(aSimTime) || !mInjectedManagers.back()->Initialize2(aSimTime))
            {
               throw UtException("Error in initialization of injected cyber track manager.");
            }
         }
      }
   }
}

} // namespace cyber
} // namespace wsf
