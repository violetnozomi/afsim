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

#include "WsfCyberEventPipe.hpp"

#include "UtPackReflect.hpp"
#include "WsfCyberEngagement.hpp"
#include "WsfCyberEventPipeClasses.hpp"
#include "WsfCyberEventPipeClassesRegister.hpp"
#include "WsfCyberEventPipeSchema.hpp"
#include "WsfCyberObserver.hpp"
#include "WsfEventPipeClasses.hpp"
#include "WsfSimulation.hpp"

namespace
{
//! Finds the platform that owns the cyber engagement
WsfPlatform* GetEngagementOwner(const wsf::cyber::Engagement& aEngagement)
{
   auto& sim = aEngagement.GetSimulation();
   return sim.GetPlatformByIndex(aEngagement.GetAttackerIndex());
}
} // namespace

namespace wsf
{
namespace cyber
{

void EventPipe::RegisterEvents(WsfEventPipeExtension& aEventPipeExtension)
{
   aEventPipeExtension.AddSchema(cWsfCyberEventPipe_SCHEMA);
   aEventPipeExtension.RegisterEventOption("CYBER", true);

   // Register the callback that gets invoked by WsfEventPipeInterface::AddedToSimulation to register
   // our message types with the serializer.
   aEventPipeExtension.AddCallback(aEventPipeExtension.RegisterExtensionMessages.Connect(&EventPipe::RegisterMessages));
}

//! A callback from WsfEventPipeInterface::AddedToSimulation to register our message types with the serializer.
void EventPipe::RegisterMessages(UtPackSerializer* aSerializerPtr)
{
   WsfEventPipe::UtPack_register_all_wsf_cyber_events_types(*aSerializerPtr);
}

EventPipe::EventPipe(WsfEventPipeInterface* aEventPipeInterfacePtr)
   : mSimulationPtr(&aEventPipeInterfacePtr->GetSimulation())
   , mEventPipePtr(aEventPipeInterfacePtr)
{
   mCyberEventId = aEventPipeInterfacePtr->GetEventId("CYBER");
}

void EventPipe::UpdateSubscriptions(const WsfEventPipeOptions& aNewCombinedOptions,
                                    const WsfEventPipeOptions& aPreviousCombinedOptions)
{
   if (aNewCombinedOptions.IsOptionEnabled(mCyberEventId))
   {
      if (!aPreviousCombinedOptions.IsOptionEnabled(mCyberEventId))
      {
         mCyberCallback +=
            WsfObserver::CyberAttackInitiated(mSimulationPtr).Connect(&EventPipe::CyberAttackInitiated, this);
         mCyberCallback +=
            WsfObserver::CyberAttackSucceeded(mSimulationPtr).Connect(&EventPipe::CyberAttackSucceeded, this);
         mCyberCallback += WsfObserver::CyberAttackFailed(mSimulationPtr).Connect(&EventPipe::CyberAttackFailed, this);
         mCyberCallback += WsfObserver::CyberAttackDetected(mSimulationPtr).Connect(&EventPipe::CyberAttackDetected, this);
         mCyberCallback +=
            WsfObserver::CyberAttackAttributed(mSimulationPtr).Connect(&EventPipe::CyberAttackAttributed, this);
         mCyberCallback += WsfObserver::CyberAttackRecovery(mSimulationPtr).Connect(&EventPipe::CyberAttackRecovery, this);
         mCyberCallback += WsfObserver::CyberScanInitiated(mSimulationPtr).Connect(&EventPipe::CyberScanInitiated, this);
         mCyberCallback += WsfObserver::CyberScanSucceeded(mSimulationPtr).Connect(&EventPipe::CyberScanSucceeded, this);
         mCyberCallback += WsfObserver::CyberScanFailed(mSimulationPtr).Connect(&EventPipe::CyberScanFailed, this);
         mCyberCallback += WsfObserver::CyberScanDetected(mSimulationPtr).Connect(&EventPipe::CyberScanDetected, this);
         mCyberCallback += WsfObserver::CyberScanAttributed(mSimulationPtr).Connect(&EventPipe::CyberScanAttributed, this);
      }
   }
   else if (aPreviousCombinedOptions.IsOptionEnabled(mCyberEventId))
   {
      mCyberCallback.Clear();
   }
}

namespace
{
void PopulateCyberBase(WsfEventPipe::CyberBase* aBase, const Engagement& aEngagement)
{
   aBase->ownerIndex(ut::safe_cast<unsigned int>(aEngagement.GetAttackerIndex()));
   aBase->victimIndex(ut::safe_cast<unsigned int>(aEngagement.GetVictimIndex()));
   aBase->attackType(aEngagement.GetAttackType());
}
void PopulateCyberBaseAttackResult(WsfEventPipe::CyberBaseAttackResult* aBase, const Engagement& aEngagement)
{
   PopulateCyberBase(aBase, aEngagement);
   aBase->successOdds().threshold(static_cast<float>(aEngagement.GetAttackSuccessThreshold()));
   aBase->successOdds().draw(static_cast<float>(aEngagement.GetAttackDraw()));
   aBase->reportOdds().threshold(static_cast<float>(aEngagement.GetStatusReportThreshold()));
   aBase->reportOdds().draw(static_cast<float>(aEngagement.GetStatusReportDraw()));
}
void PopulateCyberBaseScanResult(WsfEventPipe::CyberBaseScanResult* aBase, const Engagement& aEngagement)
{
   PopulateCyberBase(aBase, aEngagement);
   aBase->detectOdds().threshold(static_cast<float>(aEngagement.GetScanDetectionThreshold()));
   aBase->detectOdds().draw(static_cast<float>(aEngagement.GetScanDetectionDraw()));
}
} // namespace

void EventPipe::CyberAttackInitiated(double aSimTime, const Engagement& aEngagement)
{
   WsfPlatform* owner = GetEngagementOwner(aEngagement);
   if (owner != nullptr)
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgCyberAttackInitiated>();
      PopulateCyberBase(msgPtr.get(), aEngagement);
      Send(aSimTime, *owner, std::move(msgPtr));
   }
}

void EventPipe::CyberAttackSucceeded(double aSimTime, const Engagement& aEngagement)
{
   WsfPlatform* owner = GetEngagementOwner(aEngagement);
   if (owner != nullptr)
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgCyberAttackSucceeded>();
      PopulateCyberBaseAttackResult(msgPtr.get(), aEngagement);
      msgPtr->detectOdds().threshold(static_cast<float>(aEngagement.GetAttackDetectionThreshold()));
      msgPtr->detectOdds().draw(static_cast<float>(aEngagement.GetAttackDetectionDraw()));

      msgPtr->reportStatus(aEngagement.GetStatusReportSuccess() ? "success" : "failure");
      Send(aSimTime, *owner, std::move(msgPtr));
   }
}

void EventPipe::CyberAttackFailed(double aSimTime, const Engagement& aEngagement)
{
   WsfPlatform* owner = GetEngagementOwner(aEngagement);
   if (owner != nullptr)
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgCyberAttackFailed>();
      PopulateCyberBaseAttackResult(msgPtr.get(), aEngagement);
      if (aEngagement.GetAttackFailureReason() == Engagement::cATTACK_RANDOM_DRAW)
      {
         msgPtr->reportStatus(aEngagement.GetStatusReportSuccess() ? "success" : "failure");
      }
      else
      {
         msgPtr->reportStatus("unknown");
      }

      msgPtr->reason(aEngagement.GetAttackFailureReason());
      Send(aSimTime, *owner, std::move(msgPtr));
   }
}

void EventPipe::CyberAttackDetected(double aSimTime, const Engagement& aEngagement)
{
   WsfPlatform* owner = GetEngagementOwner(aEngagement);
   if (owner != nullptr)
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgCyberAttackDetected>();
      PopulateCyberBase(msgPtr.get(), aEngagement);
      Send(aSimTime, *owner, std::move(msgPtr));
   }
}

void EventPipe::CyberAttackAttributed(double aSimTime, const Engagement& aEngagement)
{
   WsfPlatform* owner = GetEngagementOwner(aEngagement);
   if (owner != nullptr)
   {
      auto msg = ut::make_unique<WsfEventPipe::MsgCyberAttackAttributed>();
      PopulateCyberBase(msg.get(), aEngagement);
      Send(aSimTime, *owner, std::move(msg));
   }
}

void EventPipe::CyberAttackRecovery(double aSimTime, const Engagement& aEngagement)
{
   WsfPlatform* owner = GetEngagementOwner(aEngagement);
   if (owner != nullptr)
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgCyberAttackRecovery>();
      PopulateCyberBase(msgPtr.get(), aEngagement);
      msgPtr->immunityStatus(aEngagement.IsVictimImmune());
      msgPtr->immunityOdds().threshold(static_cast<float>(aEngagement.GetImmunityThreshold()));
      msgPtr->immunityOdds().draw(static_cast<float>(aEngagement.GetImmunityDraw()));
      Send(aSimTime, *owner, std::move(msgPtr));
   }
}

void EventPipe::CyberScanInitiated(double aSimTime, const Engagement& aEngagement)
{
   WsfPlatform* owner = GetEngagementOwner(aEngagement);
   if (owner != nullptr)
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgCyberScanInitiated>();
      PopulateCyberBase(msgPtr.get(), aEngagement);
      Send(aSimTime, *owner, std::move(msgPtr));
   }
}

void EventPipe::CyberScanSucceeded(double aSimTime, const Engagement& aEngagement)
{
   WsfPlatform* owner = GetEngagementOwner(aEngagement);
   if (owner != nullptr)
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgCyberScanSucceeded>();
      PopulateCyberBaseScanResult(msgPtr.get(), aEngagement);
      Send(aSimTime, *owner, std::move(msgPtr));
   }
}

void EventPipe::CyberScanFailed(double aSimTime, const Engagement& aEngagement)
{
   WsfPlatform* owner = GetEngagementOwner(aEngagement);
   if (owner != nullptr)
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgCyberScanFailed>();
      PopulateCyberBaseScanResult(msgPtr.get(), aEngagement);
      msgPtr->reason(aEngagement.GetScanFailureReason());
      Send(aSimTime, *owner, std::move(msgPtr));
   }
}

void EventPipe::CyberScanDetected(double aSimTime, const Engagement& aEngagement)
{
   WsfPlatform* owner = GetEngagementOwner(aEngagement);
   if (owner != nullptr)
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgCyberScanDetected>();
      PopulateCyberBase(msgPtr.get(), aEngagement);
      Send(aSimTime, *owner, std::move(msgPtr));
   }
}

void EventPipe::CyberScanAttributed(double aSimTime, const Engagement& aEngagement)
{
   WsfPlatform* owner = GetEngagementOwner(aEngagement);
   if (owner != nullptr)
   {
      auto msg = ut::make_unique<WsfEventPipe::MsgCyberScanAttributed>();
      PopulateCyberBase(msg.get(), aEngagement);
      Send(aSimTime, *owner, std::move(msg));
   }
}

} // namespace cyber
} // namespace wsf
