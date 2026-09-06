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

#include "xio/WsfXIO_WeaponComponent.hpp"

#include <iostream>

#include "UtCallbackHolder.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEW_EA.hpp"
#include "WsfEW_EP.hpp"
#include "WsfEW_Types.hpp"
#include "WsfExplicitWeapon.hpp"
#include "WsfMilComponentRoles.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformPart.hpp"
#include "WsfRF_Jammer.hpp"
#include "WsfSimulation.hpp"
#include "WsfTaskResource.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackList.hpp"
#include "WsfTrackManager.hpp"
#include "WsfWeapon.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponTaskManager.hpp"
#include "WsfWeaponTaskResource.hpp"
#include "WsfWeaponTypes.hpp"
#include "observer/WsfWeaponObserver.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_ObjectInfo.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"
#include "xio/WsfXIO_WeaponObjectInfo.hpp"
#include "xio/WsfXIO_WeaponPacketRegistry.hpp"
#include "xio_sim/WsfXIO_Extension.hpp"
#include "xio_sim/WsfXIO_ObjectInfoLoad.hpp"
#include "xio_sim/WsfXIO_ObjectInfoLoadHelpers.hpp"
#include "xio_sim/WsfXIO_PlatformInfoService.hpp"
#include "xio_sim/WsfXIO_Simulation.hpp"
#include "xio_sim/WsfXIO_TaskService.hpp"

// =================================================================================================
// 'info' loaders for weapons (like those found in WsfXIO_ObjectInfoLoad).
// =================================================================================================

namespace
{
//! Load XIO weapon info pertaining the base class WsfWeapon
void Load_Weapon(WsfXIO_Simulation& aXIO_Simulation, WsfXIO_WeaponInfo& aInfo, WsfWeapon& aWeapon)
{
   aInfo.mPartType = cCOMPONENT_ROLE<WsfWeapon>();
   Load_ModedPart(aXIO_Simulation, aInfo, aWeapon); // Load base class
   aInfo.mQuantityRemaining = aWeapon.GetQuantityRemaining();
}

//! Load XIO weapon info pertaining to WsfExplicitWeapon
void Load_ExplicitWeapon(WsfXIO_Simulation& aXIO_Simulation, WsfXIO_ExplicitWeaponInfo& aInfo, WsfExplicitWeapon& aWeapon)
{
   Load_Weapon(aXIO_Simulation, aInfo, aWeapon); // Load base class
   aInfo.mLaunchedPlatformType = aWeapon.GetLaunchedPlatformType();
}

//! Load XIO weapon info pertaining to WsfRF_Jammer
void Load_JammerWeapon(WsfXIO_Simulation& aXIO_Simulation, WsfXIO_JammerWeaponInfo& aInfo, WsfRF_Jammer& aJammer)
{
   Load_Weapon(aXIO_Simulation, aInfo, aJammer); // Load base class
   Load_Transmitters(aXIO_Simulation, aJammer, aInfo.mTransmitters);

   aInfo.mMasterXmtrs.resize(aJammer.GetModeCount());
   for (unsigned int i = 0; i < static_cast<unsigned int>(aInfo.mMasterXmtrs.size()); ++i)
   {
      WsfXIO_EM_XmtrInfo*       xmtrInfoPtr = new WsfXIO_EM_XmtrInfo;
      WsfRF_Jammer::JammerXmtr* xmtrPtr = dynamic_cast<WsfRF_Jammer::JammerMode*>(aJammer.GetModeEntry(i))->mXmtr.get();
      WsfXIO_ObjectInfoLoad::Load_EM_Xmtr(aXIO_Simulation, *xmtrInfoPtr, *xmtrPtr);
      aInfo.mMasterXmtrs[i] = xmtrInfoPtr;
   }
}
} // namespace

// =================================================================================================
WsfXIO_WeaponComponent::WsfXIO_WeaponComponent()
   : WsfXIO_Component(cWSF_COMPONENT_WEAPON_XIO_COMPONENT, "xio_weapon")
   , mPlatformInfoServicePtr(nullptr)
{
   // The XIO interface cannot be referenced at this point because we haven't been added as a component.
}

// =================================================================================================
//! Called when the component is added to the XIO interface
// virtual
void WsfXIO_WeaponComponent::ComponentParentChanged(WsfXIO_Simulation* aXIO_SimulationPtr)
{
   WsfXIO_Interface* interfacePtr = aXIO_SimulationPtr->GetInterface();
   WsfXIO_WeaponPacketRegistry::RegisterPackets(*interfacePtr);

   // Perform functions that are necessary only if the application is a WSF simulation.
   // Must register the packet handlers.
   mCallbacks.Add(interfacePtr->Connect(&WsfXIO_WeaponComponent::HandleFireWeaponRequest, this));
   mCallbacks.Add(interfacePtr->Connect(&WsfXIO_WeaponComponent::HandleEW_TechniqueStateChangeRequest, this));
   mPlatformInfoServicePtr = aXIO_SimulationPtr->GetPlatformInfoService();
}

// =================================================================================================
WsfSimulation& WsfXIO_WeaponComponent::GetSimulation() const
{
   return GetXIO_Simulation()->GetSimulation();
}

// =================================================================================================
double WsfXIO_WeaponComponent::GetSimTime() const
{
   return GetSimulation().GetSimTime();
}

//==================================================================================================
//==================================================================================================
//
// Calls from xio_sim/WsfXIO_PlatformInfoService
//
//==================================================================================================
//==================================================================================================

// =================================================================================================
//! Called from WsfXIO_PlatformInfoService::UpdateCallbacks.
// virtual
void WsfXIO_WeaponComponent::RegisterSimulationObservers(WsfSimulation& aSimulation, UtCallbackHolder& aCallbacks)
{
   aCallbacks += WsfObserver::WeaponTurnedOn(&aSimulation).Connect(&WsfXIO_WeaponComponent::WeaponStateChange, this);
   aCallbacks += WsfObserver::WeaponTurnedOff(&aSimulation).Connect(&WsfXIO_WeaponComponent::WeaponStateChange, this);
   aCallbacks +=
      WsfObserver::WeaponQuantityChanged(&aSimulation).Connect(&WsfXIO_WeaponComponent::WeaponStateChange, this);
   aCallbacks += WsfObserver::WeaponReloadEnded(&aSimulation).Connect(&WsfXIO_WeaponComponent::WeaponStateChange, this);
   aCallbacks += WsfObserver::WeaponFired(&aSimulation).Connect(&WsfXIO_WeaponComponent::WeaponFired, this);
   aCallbacks += WsfObserver::WeaponModeActivated(&aSimulation).Connect(&WsfXIO_WeaponComponent::WeaponModeChange, this);
   aCallbacks += WsfObserver::WeaponModeDeactivated(&aSimulation).Connect(&WsfXIO_WeaponComponent::WeaponModeChange, this);
}

// =================================================================================================
//! Called from WsfXIO_PlatformInfoService::FillPartState.
// virtual
bool WsfXIO_WeaponComponent::FillPartState(const WsfPlatformPart& aPart, WsfXIO_PartStateChangePkt& aPkt)
{
   bool processed(false);
   if (aPkt.mPartType == cCOMPONENT_ROLE<WsfWeapon>())
   {
      processed               = true;
      WsfWeapon& weapon       = ((WsfWeapon&)aPart);
      aPkt.mQuantityRemaining = weapon.GetQuantityRemaining();
      for (size_t i = 0; i < weapon.GetModeCount(); ++i)
      {
         if ((weapon.GetModeEntry(i) != nullptr) && weapon.GetModeEntry(i)->IsSelected())
         {
            aPkt.mSelectedModes.push_back(weapon.GetModeEntry(i)->GetNameId());
         }
      }
      aPkt.mCurrentModeName = weapon.GetCurrentModeName();
   }
   return processed;
}

// =================================================================================================
//! Called from WsfXIO_PlatformInfoService::FindObjectType
// virtual
WsfObject* WsfXIO_WeaponComponent::FindPartType(int aPartType, WsfStringId aPartTypeName)
{
   WsfWeapon* weaponPtr(nullptr);
   if (aPartType == cCOMPONENT_ROLE<WsfWeapon>())
   {
      weaponPtr = WsfWeaponTypes::Get(GetSimulation().GetScenario()).Find(aPartTypeName);
   }
   return weaponPtr;
}

// =================================================================================================
//! Called from WsfXIO_PlatformInfoService::FindObjectType
WsfObject* WsfXIO_WeaponComponent::FindPart(const WsfPlatform& aPlatform, int aPartType, WsfStringId aPartName)
{
   WsfWeapon* weaponPtr(nullptr);
   if (aPartType == cCOMPONENT_ROLE<WsfWeapon>())
   {
      weaponPtr = aPlatform.GetComponent<WsfWeapon>(aPartName);
   }
   return weaponPtr;
}

// =================================================================================================
//! Simulation observer callback
void WsfXIO_WeaponComponent::WeaponStateChange(double aSimTime, WsfWeapon* aWeaponPtr)
{
   mPlatformInfoServicePtr->SendPartState(*aWeaponPtr, cWEAPON_INFO);
}

// =================================================================================================
//! Simulation observer callback
void WsfXIO_WeaponComponent::WeaponModeChange(double aSimTime, WsfWeapon* aWeaponPtr, WsfWeapon::WsfWeaponMode* aModePtr)
{
   mPlatformInfoServicePtr->SendPartState(*aWeaponPtr, cWEAPON_INFO);
}

// =================================================================================================
//! Simulation observer callback
void WsfXIO_WeaponComponent::WeaponFired(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, const WsfTrack* aTrackPtr)
{
   // Quantity remaining isn't decremented until after the fire()
   const WsfWeapon* weaponPtr = aEngagementPtr->GetWeaponSystem();
   if (weaponPtr != nullptr)
   {
      mWeaponsFired.emplace_back(weaponPtr->GetPlatform()->GetIndex(), weaponPtr->GetNameId());
      GetSimulation().AddEvent(ut::make_unique<WsfOneShotEvent>(aSimTime, [this]() { AfterWeaponFired(); }));
   }
}

// =================================================================================================
//! Callback from event scheduled in WeaponFired.
void WsfXIO_WeaponComponent::AfterWeaponFired()
{
   for (const auto& weapon : mWeaponsFired)
   {
      WsfPlatform* platformPtr = GetSimulation().GetPlatformByIndex(weapon.first);
      if (platformPtr != nullptr)
      {
         WsfWeapon* weaponPtr = platformPtr->GetComponent<WsfWeapon>(weapon.second);
         mPlatformInfoServicePtr->SendPartState(*weaponPtr, cWEAPON_INFO);
      }
   }
   mWeaponsFired.clear();
}

// =================================================================================================
void WsfXIO_WeaponComponent::EM_RcvrInfo_Load(WsfXIO_EM_RcvrInfo& aInfo, const WsfEM_Rcvr& aRcvr)
{
   WsfEW_EA_EP* epPtr = WsfEW_EP::GetElectronicProtect(aRcvr);
   if (epPtr != nullptr)
   {
      LoadTechniques(aInfo, epPtr);
   }
}

// =================================================================================================
void WsfXIO_WeaponComponent::EM_XmtrInfo_Load(WsfXIO_EM_XmtrInfo& aInfo, const WsfEM_Xmtr& aXmtr)
{
   WsfEW_EA_EP* eaPtr = WsfEW_EA::GetElectronicAttack(aXmtr);
   if (eaPtr != nullptr)
   {
      LoadTechniques(aInfo, eaPtr);
   }
}

// =================================================================================================
//! Load EW techniques (EA/EP) from a WSF Xmtr/Rcvr into the and XIO Xmtr/Rcvr
void WsfXIO_WeaponComponent::LoadTechniques(WsfXIO_EM_XmtrRcvrInfo& aInfo, WsfEW_EA_EP* aEW_Ptr)
{
   delete aInfo.mExtraPtr;
   aInfo.mExtraPtr = nullptr;

   const WsfEW_Types::TechniqueIdVec& techs = aEW_Ptr->GetTechniquesAvailable();
   if (!techs.empty())
   {
      WsfXIO_EW_Techniques* techsPtr = new WsfXIO_EW_Techniques;
      aInfo.mExtraPtr                = techsPtr;

      for (auto techId : techs)
      {
         WsfXIO_EW_Technique tech;
         tech.mInUse                = aEW_Ptr->TechniqueIsInUse(techId);
         tech.mExternallyControlled = aEW_Ptr->TechniqueIsExternallyControlled(techId);
         for (auto effect : aEW_Ptr->GetEffects(techId))
         {
            tech.mEffects.push_back(effect->GetNameId());
         }
         techsPtr->mEW_Techniques.emplace(WsfStringId(techId), tech);
      }
   }
}

// =================================================================================================
//! Called from WsfXIO_PlatformPartInfo::Create to process an unrecognized platform part type.
WsfXIO_PlatformPartInfo* WsfXIO_WeaponComponent::CreatePlatformPartInfo(WsfPlatformPart& aPart)
{
   WsfWeapon* weaponPtr = nullptr;
   if (aPart.QueryInterfaceT(weaponPtr))
   {
      WsfExplicitWeapon* exWeaponPtr = dynamic_cast<WsfExplicitWeapon*>(weaponPtr);
      if (exWeaponPtr != nullptr)
      {
         WsfXIO_ExplicitWeaponInfo* infoPtr = new WsfXIO_ExplicitWeaponInfo();
         Load_ExplicitWeapon(*GetXIO_Simulation(), *infoPtr, *exWeaponPtr);
         return infoPtr;
      }
      WsfRF_Jammer* jammerPtr = dynamic_cast<WsfRF_Jammer*>(weaponPtr);
      if (jammerPtr != nullptr)
      {
         WsfXIO_JammerWeaponInfo* infoPtr = new WsfXIO_JammerWeaponInfo();
         Load_JammerWeapon(*GetXIO_Simulation(), *infoPtr, *jammerPtr);
         return infoPtr;
      }

      WsfXIO_WeaponInfo* infoPtr = new WsfXIO_WeaponInfo();
      Load_Weapon(*GetXIO_Simulation(), *infoPtr, *weaponPtr);
      return infoPtr;
   }
   return nullptr;
}

// =================================================================================================
// Called from WsfXIO_Simulation::HandleStateChangeRequest
bool WsfXIO_WeaponComponent::HandleStateChangeRequest(double                            aSimTime,
                                                      WsfXIO_PartStateChangeRequestPkt& aPkt,
                                                      WsfPlatform*                      aPlatformPtr,
                                                      WsfEM_Xmtr*&                      aXmtrPtr,
                                                      WsfEM_Rcvr*&                      aRcvrPtr)
{
   bool processed(false);
   if (aPkt.mPartType == cCOMPONENT_ROLE<WsfWeapon>())
   {
      processed            = true;
      aXmtrPtr             = nullptr;
      aRcvrPtr             = nullptr;
      WsfWeapon* weaponPtr = aPlatformPtr->GetComponent<WsfWeapon>(aPkt.mPartName);
      if (weaponPtr != nullptr)
      {
         int commandType    = aPkt.mStateChange & WsfXIO_PartStateChangeRequestPkt::cCOMMAND_MASK;
         int subCommandType = aPkt.mStateChange & ~WsfXIO_PartStateChangeRequestPkt::cCOMMAND_MASK;
         if (aPkt.mSelectMode != 0)
         {
            double simTime = GetSimulation().GetSimTime();
            if ((subCommandType & WsfXIO_PartStateChangeRequestPkt::cDESELECT_MODE) != 0)
            {
               weaponPtr->DeselectMode(simTime, aPkt.mSelectMode);
            }
            else
            {
               weaponPtr->SelectMode(simTime, aPkt.mSelectMode);
            }
         }
         if (commandType == WsfXIO_PartStateChangeRequestPkt::cXMTR_RCVR_COMMAND)
         {
            if (aPkt.mXmtrIndex < (int)weaponPtr->GetEM_XmtrCount())
            {
               aXmtrPtr = &weaponPtr->GetEM_Xmtr(aPkt.mXmtrIndex);
               if ((subCommandType & WsfXIO_PartStateChangeRequestPkt::cCHANGE_RCVR) != 0)
               {
                  aRcvrPtr = aXmtrPtr->GetLinkedReceiver();
               }
            }
         }
      }
   }
   return processed;
}

// =================================================================================================
// Called from WsfTaskService::HandleCommand.
bool WsfXIO_WeaponComponent::HandleTaskCommand(double                 aSimTime,
                                               WsfXIO_TaskCommandPkt& aPkt,
                                               WsfTaskProcessor*      aProcA_Ptr,
                                               WsfTaskProcessor*      aProcB_Ptr,
                                               WsfTrack*              aTrackPtr,
                                               WsfTaskResource*       aResourcePtr)
{
   bool processed(false);
   // TODO_XIO (JAJ) cABORT_FIRE is not ever sent even in WSF. So what is this doing???
   if ((aPkt.mCommandType == cABORT_FIRE) && (aProcA_Ptr != nullptr) && (aTrackPtr != nullptr))
   {
      WsfWeaponTaskManager* weaponTaskMgrPtr = WsfWeaponTaskManager::Find(*aProcA_Ptr);
      if (weaponTaskMgrPtr != nullptr)
      {
         processed = weaponTaskMgrPtr->AbortFiring(aSimTime, aTrackPtr->GetTrackId());
      }
   }
   return processed;
}

// =================================================================================================
void WsfXIO_WeaponComponent::HandleFireWeaponRequest(WsfXIO_FireWeaponPkt& aPkt)
{
   WsfPlatform* platformPtr = GetSimulation().GetPlatformByIndex(aPkt.mPlatformIndex);
   if (platformPtr != nullptr)
   {
      WsfWeapon* weaponPtr = platformPtr->GetComponent<WsfWeapon>(aPkt.mWeaponNameId);
      if (weaponPtr != nullptr)
      {
         const WsfTrack* trackPtr = aPkt.mTrackPtr;
         if (trackPtr == nullptr)
         {
            trackPtr = platformPtr->GetTrackManager().FindTrack(aPkt.mTrackId);
            if (trackPtr == nullptr)
            {
               trackPtr = platformPtr->GetTrackManager().FindRawTrack(aPkt.mTrackId);
            }
         }

         // If track can't be found using TrackId, use Target Name.
         if (trackPtr == nullptr)
         {
            WsfLocalTrackList& trackList = platformPtr->GetTrackManager().GetTrackList();
            for (unsigned int i = 0; i < trackList.GetTrackCount(); ++i)
            {
               if (trackList.GetTrackEntry(i)->GetTargetName() == aPkt.mTargetNameId)
               {
                  trackPtr = trackList.GetTrackEntry(i);
                  break;
               }
            }
         }

         if (trackPtr != nullptr)
         {
            // Find the task manager on the platform, if one exists
            WsfStringId       procId(nullptr);
            WsfTaskProcessor* taskMgrPtr =
               WsfXIO_TaskService::FindTaskProcessor(GetSimulation(), platformPtr->GetIndex(), procId);

            if (taskMgrPtr != nullptr)
            {
               WsfWeaponTaskManager* weaponTaskMgrPtr = WsfWeaponTaskManager::Find(*taskMgrPtr);
               assert(weaponTaskMgrPtr != nullptr);
               WsfPlatform* uplinkPlatformPtr =
                  GetSimulation().GetPlatformByName(weaponTaskMgrPtr->GetAutoUplinkPlatformNameId());
               WsfWeaponTaskResource resource;
               resource.mNameId = weaponPtr->GetNameId();
               WsfTask fireTask(resource);
               fireTask.SetAssignee(platformPtr);
               fireTask.SetTaskType(WsfStringId("FIRE"));
               if ((uplinkPlatformPtr != nullptr) && weaponTaskMgrPtr->IsAutoUplink())
               {
                  WsfUplinkTaskResource uplink;
                  WsfTask               uplinkTask(uplink);
                  uplinkTask.SetTaskType(WsfStringId("FIRE"));
                  uplinkTask.SetAssignee(uplinkPlatformPtr);
                  taskMgrPtr->AssignTask(GetSimulation().GetSimTime(), *trackPtr, uplinkTask);
               }
               taskMgrPtr->AssignTask(GetSimulation().GetSimTime(), *trackPtr, fireTask);
            }
            else
            {
               if (aPkt.mSalvoSize == 1)
               {
                  weaponPtr->Fire(GetSimulation().GetSimTime(), trackPtr, WsfWeapon::FireOptions());
               }
               else
               {
                  weaponPtr->FireSalvo(GetSimulation().GetSimTime(), trackPtr, aPkt.mSalvoSize);
               }
            }
         }
      }
   }
}

// =================================================================================================
void WsfXIO_WeaponComponent::HandleEW_TechniqueStateChangeRequest(WsfXIO_EW_TechniqueStateChangeRequestPkt& aPkt)
{
   WsfPlatform* platformPtr = GetSimulation().GetPlatformByIndex(aPkt.mPlatformIndex);
   if (platformPtr != nullptr)
   {
      if (aPkt.mTechniqueType == WsfXIO_EW_TechniqueStateChangeRequestPkt::cEP)
      {
         WsfSensor* sensorPtr = platformPtr->GetComponent<WsfSensor>(aPkt.mPartName);
         if ((sensorPtr != nullptr) && aPkt.mXmtrRcvrIndex < (int)sensorPtr->GetEM_RcvrCount())
         {
            WsfEM_Rcvr* rcvrPtr = &sensorPtr->GetEM_Rcvr(aPkt.mXmtrRcvrIndex);
            if (rcvrPtr != nullptr)
            {
               WsfEW_EA_EP* epPtr = WsfEW_EP::GetElectronicProtect(*rcvrPtr);
               if (epPtr != nullptr)
               {
                  epPtr->SetTechnique(aPkt.mTechniqueId, aPkt.mOnOff);
               }
            }
         }
      }
      else // WsfXIO_EW_TechniqueStateChangeRequestPkt::cEA
      {
         WsfWeapon* weaponPtr = platformPtr->GetComponent<WsfWeapon>(aPkt.mPartName);
         if ((weaponPtr != nullptr) && aPkt.mXmtrRcvrIndex < (int)weaponPtr->GetEM_XmtrCount())
         {
            WsfEM_Xmtr* xmtrPtr = &weaponPtr->GetEM_Xmtr(aPkt.mXmtrRcvrIndex);
            if (xmtrPtr != nullptr)
            {
               WsfEW_EA_EP* eaPtr = WsfEW_EA::GetElectronicAttack(*xmtrPtr);
               if (eaPtr != nullptr)
               {
                  eaPtr->SetTechnique(aPkt.mTechniqueId, aPkt.mOnOff);
               }
            }
         }
      }
   }
}
