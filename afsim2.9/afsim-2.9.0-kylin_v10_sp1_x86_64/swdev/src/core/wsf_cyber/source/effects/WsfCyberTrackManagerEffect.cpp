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

#include "WsfCyberTrackManagerEffect.hpp"

#include "UtMemory.hpp"
#include "WsfCyberEngagement.hpp"
#include "WsfCyberTrackManager.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace cyber
{

// ================================================================================================
TrackManagerEffect::TrackManagerEffect(WsfScenario& aScenario)
   : Effect()
   , mContextPtr(std::make_shared<WsfScriptContext>(*aScenario.GetScriptContext(), "WsfCyberTrackManagerEffect"))
{
}

// ================================================================================================
TrackManagerEffect* TrackManagerEffect::Clone() const
{
   return new TrackManagerEffect(*this);
}

// ================================================================================================
bool TrackManagerEffect::Initialize(Engagement& aEngagement)
{
   return mContextPtr->Initialize(aEngagement.GetSimulation(), &aEngagement);
}

// ================================================================================================
bool TrackManagerEffect::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   auto command   = aInput.GetCommand();

   if (command == "target_platform")
   {
      std::string platformOrPlatformTypeName;
      aInput.ReadValue(platformOrPlatformTypeName);
      mTargets.insert(platformOrPlatformTypeName);
   }
   else if (command == "remove_target_platform")
   {
      std::string platformOrPlatformTypeName;
      aInput.ReadValue(platformOrPlatformTypeName);
      mTargets.erase(platformOrPlatformTypeName);
   }
   else if (command == "inactive_track_purging")
   {
      aInput.ReadValue(mInactiveTrackPurging);
   }
   else if (command == "inactive_raw_track_purging")
   {
      aInput.ReadValue(mInactiveRawTrackPurging);
   }
   else if (command == "local_track_history_purging")
   {
      aInput.ReadValue(mLocalTrackHistoryPurging);
   }
   else if (command == "script")
   {
      UtScript*   scriptPtr   = mContextPtr->Compile(aInput, "end_script", GetName());
      std::string scriptName  = scriptPtr->GetName();
      bool        validScript = false;

      if (scriptName == "OnEntry")
      {
         validScript = mContextPtr->ValidateScript(scriptPtr, "void", "WsfTrackManager");
         mOnEntry    = scriptPtr;
      }
      else if (scriptName == "OnExit")
      {
         validScript = mContextPtr->ValidateScript(scriptPtr, "void", "WsfTrackManager");
         mOnExit     = scriptPtr;
      }
      else if (scriptName == "OnTrack")
      {
         validScript = mContextPtr->ValidateScript(scriptPtr, "bool", "WsfTrack, WsfTrackManager");
         mOnTrack    = scriptPtr;
      }
      else if (scriptName == "OnRawTrack")
      {
         validScript = mContextPtr->ValidateScript(scriptPtr, "bool", "WsfTrack, WsfTrackManager");
         mOnRawTrack = scriptPtr;
      }
      else if (scriptName == "OnTrackDrop")
      {
         validScript  = mContextPtr->ValidateScript(scriptPtr, "bool", "WsfTrackId, WsfTrackManager");
         mOnTrackDrop = scriptPtr;
      }

      if (!validScript)
      {
         throw UtException(
            "Invalid script return type, argument type, or argument in WSF_CYBER_TRACK_MANAGER_EFFECT::" + GetName() +
            "::" + scriptName);
      }
   }
   else if (Effect::ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// ================================================================================================
void TrackManagerEffect::Attack(double aSimTime, Engagement& aEngagement)
{
   const auto& sim       = aEngagement.GetSimulation();
   auto        victimPtr = sim.GetPlatformByName(aEngagement.GetVictim());
   if (victimPtr)
   {
      InjectTrackManager(aSimTime, aEngagement);

      if (mInjectedManagerPtr)
      {
         mInjectedManagerPtr->Activate();
         mInjectedManagerPtr->ExecuteOnEntry(aSimTime);
      }
   }
}

// ================================================================================================
void TrackManagerEffect::Restore(double aSimTime, Engagement& aEngagement)
{
   const auto& sim       = aEngagement.GetSimulation();
   auto        victimPtr = sim.GetPlatformByName(aEngagement.GetVictim());
   if (victimPtr && mInjectedManagerPtr)
   {
      mInjectedManagerPtr->ExecuteOnExit(aSimTime);
      mInjectedManagerPtr->Deactivate();
   }
}

// ================================================================================================
TrackManagerEffect::TrackManagerEffect(const TrackManagerEffect& aSrc)
   : Effect(aSrc)
   , mContextPtr(std::make_shared<WsfScriptContext>(*aSrc.mContextPtr))
   , mTargets(aSrc.mTargets)
   , mOnEntry(aSrc.mOnEntry)
   , mOnExit(aSrc.mOnExit)
   , mOnTrack(aSrc.mOnTrack)
   , mOnRawTrack(aSrc.mOnRawTrack)
   , mOnTrackDrop(aSrc.mOnTrackDrop)
   , mInjectedManagerPtr(aSrc.mInjectedManagerPtr)
   , mInactiveTrackPurging(aSrc.mInactiveTrackPurging)
   , mInactiveRawTrackPurging(aSrc.mInactiveRawTrackPurging)
   , mLocalTrackHistoryPurging(aSrc.mLocalTrackHistoryPurging)
{
}

// ================================================================================================
void TrackManagerEffect::InjectTrackManager(double aSimTime, const Engagement& aEngagement)
{
   auto victimPtr = GetValidTarget(aEngagement);
   if (victimPtr)
   {
      std::unique_ptr<WsfTrackManager> trackManagerPtr(victimPtr->GetComponent<WsfTrackManager>());
      if (trackManagerPtr)
      {
         victimPtr->RemoveComponent(trackManagerPtr.get());
         trackManagerPtr->SetComponentParent(victimPtr);

         auto cyberTrackManager = ut::make_unique<TrackManager>(std::move(trackManagerPtr),
                                                                mContextPtr,
                                                                mOnEntry,
                                                                mOnExit,
                                                                mOnTrack,
                                                                mOnRawTrack,
                                                                mOnTrackDrop,
                                                                mInactiveTrackPurging,
                                                                mInactiveRawTrackPurging,
                                                                mLocalTrackHistoryPurging);

         mInjectedManagerPtr = cyberTrackManager.get();
         victimPtr->AddComponent(cyberTrackManager.release());
         if (!mInjectedManagerPtr->Initialize(aSimTime) || !mInjectedManagerPtr->Initialize2(aSimTime))
         {
            throw UtException("Error in initialization of injected cyber track manager.");
         }

         //! Notify required processors so they may update references.
         mInjectedManagerPtr->NotifyOfTrackManagerChanged(aSimTime, mInjectedManagerPtr);
      }
   }
}

// ================================================================================================
WsfPlatform* TrackManagerEffect::GetValidTarget(const Engagement& aEngagement)
{
   const auto& sim       = aEngagement.GetSimulation();
   auto        victimPtr = sim.GetPlatformByName(aEngagement.GetVictim());
   if (victimPtr)
   {
      for (const auto& target : mTargets)
      {
         if ((victimPtr->GetName() == target) || victimPtr->IsA_TypeOf(target))
         {
            return victimPtr;
         }
      }
   }

   return nullptr;
}

} // namespace cyber
} // namespace wsf
