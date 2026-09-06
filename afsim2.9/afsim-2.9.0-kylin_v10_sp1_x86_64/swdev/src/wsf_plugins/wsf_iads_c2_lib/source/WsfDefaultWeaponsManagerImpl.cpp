// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: WsfDefaultWeaponsManagerImpl.cpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name:	Radiance Technologies
*   Address:			350 Wynn Drive
*   					   Huntsville, AL 35805
*
*   Abstract: This class provides default implementations in C++ for
*    what could be overridden in script for the Weapons Manager base class.
*    The intention was to provide the classical implementation of what was originally
*    done in script so default usage of the processor works without supplemental scripting.
*
*******************************************************************************/

#include <WsfDefaultWeaponsManagerImpl.hpp>

#include <memory>

#include <WsfPlatform.hpp>

#include <WsfBMWeaponsManager.hpp>
#include <WsfAssetManager.hpp>
#include <WsfBMUtils.hpp>
#include <logger.hpp>
#include <iadsLib/weaponsManagerEngageInterface.hpp>
#include <iadsLib/assignmentMessage.hpp>
#include <iadsLib/trackContainers.hpp>
#include <iadsLib/weaponRecordInterface.hpp>
#include <iadsLib/moeLoggingInterface.hpp>
#include <WsfBMMOELogger.hpp>
#include "WsfIADSC2ScenarioExtension.hpp"

WsfDefaultWeaponsManagerImpl::WsfDefaultWeaponsManagerImpl()
{
}

bool WsfDefaultWeaponsManagerImpl::ProcessInput(UtInput& aInput)
{
   return false;
}

void WsfDefaultWeaponsManagerImpl::Initialize(WsfBMWeaponsManager* wm)
{
   HCL_DEBUG_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(wm->GetScenario())) << wm->GetPlatform()->GetName() << "/" << wm->GetName() << ": " << wm->GetScriptClassName() << ": in on_initialize" << std::endl;
}

void WsfDefaultWeaponsManagerImpl::Initialize2(WsfBMWeaponsManager* wm)
{
   HCL_DEBUG_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(wm->GetScenario())) << wm->GetPlatform()->GetName() << "/" << wm->GetName() << ": " << wm->GetScriptClassName() << ": in on_initialize2" << std::endl;

   // locate the other managers
   m_am_ref = WsfBMUtils::FindAttachedAssetManagerRef2(wm->GetPlatform(), true);

   if (!m_am_ref)
   {
      HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(wm->GetScenario())) << "Weapons Manager: could not locate asset manager - all platforms on the C2 network must have an asset manager!" << std::endl;
   }
}

void WsfDefaultWeaponsManagerImpl::on_update(const double sim_time_s, WsfPlatform* this_plat, WsfBMWeaponsManager* wm)
{
   auto wm_proc = wm->GetCoreWMRef();
   auto am_proc = wm->GetCoreAMRef();

   if (wm_proc->getSelfDefense())
   {
      PerformSelfDefense(sim_time_s, this_plat, wm);
   }

   // foreach assignment
   MapAssignmentArrayFunc
   (*IADSC2SimulationExtension::Find(*this_plat->GetSimulation())->GetLogger(), am_proc->getAssignmentArray(),
    [sim_time_s](il::moeLoggingInterface& aMOELogger, il::evalAssignmentMultimap &assignments, il::evalAssignmentMultimap::iterator &assigned_track_itor, il::evalAssignmentMultimap::iterator &assignment_itor, WsfBMWeaponsManager *wm) -> void
   {
      auto am_proc = wm->GetCoreAMRef();
      auto this_asset = am_proc->getThisAsset();

      std::shared_ptr<il::weaponRecord> assigned_weapon;

      auto weapons = am_proc->getThisAsset()->getWeapons();
      for (auto it = weapons.begin(); it != weapons.end(); ++it)
      {
         auto& weapon = *it;
         il::idRecord this_assigned_id(this_asset->getID().getID(), weapon->GetWeaponID().getID());
         if (assignment_itor->second->getAssignedID() == this_assigned_id)
         {
            assigned_weapon = weapon;
            break;
         }
      }

      // am I the assigned unit?
      if (assigned_weapon)
      {
         // not time to process or already completed?
         if (assignment_itor->second->isAssignmentComplete() ||
             am_proc->getAssignmentDelay() > sim_time_s - assignment_itor->second->getAssignTime())
         {
            ++assignment_itor;
            return;
         }

         // are we not green?
         if (!this_asset->isGreen())
         {
            am_proc->cantcoAssignment(aMOELogger, assignment_itor->second, "No self status (not green)");
            ++assignment_itor;
            return;
         }

         // we've not fired and no more munitions?
         if (assigned_weapon->GetShotsFiredAtTrack(assigned_track_itor->first) +
             assigned_weapon->GetShotsPendingForTrack(assigned_track_itor->first) <= 0 &&
             assigned_weapon->GetTruthTotalMunitions() <= 0)
         {
            am_proc->cantcoAssignment(aMOELogger, assignment_itor->second, "No munitions");
            ++assignment_itor;
            return;
         }
      }

      ++assignment_itor;
   }, wm);

   // foreach assignment
   // foreach munition
   // if munition.qty > 0
   // break

   // if (assigned unit is not me)
   // break
   // assignment_master_track = GetAssignedTrack(Assignment)

   // NOTE: skip WMUtilUpdateLaunchedMissiles because this should be an implementation detail of the weapon itself, not the manager?
   // if (not time to process assignment || assignment completed)
   // continue
   // if (our status != green)
   // CANTCO Assignment
   // continue
   //
   // if (weapon hasn't fired and no more munitions)
   //    CANTCO Assignment
   //
   // NOTE: the next logic block is directly checking for a salvo 2 time and then calling SAM fire. This should not be the role of the assignment management portion of this code. Skipping for now
   // if (no master track found)
   //    CANTCOAssignment
   //
   // NOTE: skip WMUtilReportHAVCOKill since it's just a vestige of the dissemination processor
   // NOTE: same for WMUtilReportHAVCODamage
   // NOTE: same for WMUtilReportHAVCOMiss
   //
   // if (assignment has expired)
   //    CANTCO Assignment
   //
   // NOTE: The next block is all about taking the shot...this will be where we break off between some behaviour abstraction and assignment processing
   // NOTE: any time we cancel or cantco from the WM, we need to notify the underlying behavior abstraction, don't forget this. In fact, we may need to include all processors
   //    from this angle
   //
   // invoke shot behavior
   // need some concept of state check back and forth. Like,
   // if (!behaviors.IsAssignmentInProgress(Assignment))
   //    behaviors.HandleAssignment(Assignment)
   //
   // behaviors.UpdateState()
   // behaviors can directly invoke on the asset manager so hopefully a lot of this code can be seamless
   wm_proc->getWeaponEngagementModule()->Run(sim_time_s);
}

bool WsfDefaultWeaponsManagerImpl::on_message(const double sim_time_s, const WsfMessage& message, WsfBMWeaponsManager* wm)
{
   // if we don't want to block other processors from recieving the message, we must keep this flag false,
   // otherwise nobody else will get a chance to process it. We do this so that other processors have a shot regardless if
   // we process it or not.
   bool block_processing_message = false;

   // all messages get dispatched to the asset manager
   if (m_am_ref)
   {
      m_am_ref->ProcessMessage(sim_time_s, message);
   }
   else
   {
      HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(wm->GetScenario())) << "WsfDefaultWeaponsManagerImpl::on_message(): no asset manager to dispatch message to..." << std::endl;
   }

   return block_processing_message;
}

void WsfDefaultWeaponsManagerImpl::PerformSelfDefense(const double sim_time_s, WsfPlatform* this_plat, WsfBMWeaponsManager* wm)
{
   auto am = wm->GetCoreAMRef();
   auto tracks = am->GetMasterTracksArray();
   for (auto it = tracks.cbegin(); it != tracks.cend(); ++it)
   {
      const auto& track = it->second;
      if (!am->isTrackAssigned(track->getID()))
      {
         if (!am->getThisAsset()->hasOpenAssignments())
         {
            break;
         }

         PerformSelfDefenseProcessing(sim_time_s, this_plat, wm, track);
      }
   }
}

bool WsfDefaultWeaponsManagerImpl::PerformSelfDefenseProcessing(const double sim_time_s, WsfPlatform* this_plat, WsfBMWeaponsManager* wm, const std::shared_ptr<il::trackRecord>& track)
{
   auto wm_proc = wm->GetCoreWMRef();
   auto am_proc = wm->GetCoreAMRef();
   auto this_asset = am_proc->getThisAsset();

   bool assignment_formed = false;

   if (!(track->getIFF() == il::E_IFF_HOSTILE))
   {
      return assignment_formed;
   }

   // propogate position and check the slant range against the self defense range
   auto slant_range = il::trackRecord::extrapolateTrack(*track, sim_time_s - track->getTime()).distance(this_asset->getPosition());
   if (slant_range <= wm_proc->getSelfDefenseRange())
   {
      if (am_proc->getThisAsset()->getNumOpenAssignments() > 0 && this_asset->hasWeapon())
      {
         // add assignment
         assignment_formed = CreateSelfDefenseAssignment(sim_time_s, this_plat, wm, track);
         if (assignment_formed)
         {
            HCL_DEBUG_LOGGER(am_proc->getGlobalLogger()) << "NOTICE: Self defense engagement created on " << this_asset->getName() << " vs " << track->getTargetTruthName();
         }
      }
   }

   return assignment_formed;
}

bool WsfDefaultWeaponsManagerImpl::CreateSelfDefenseAssignment(const double sim_time_s, WsfPlatform* this_plat, WsfBMWeaponsManager* wm, const std::shared_ptr<il::trackRecord>& track)
{
   auto& track_id = track->getID();
   auto wm_proc = wm->GetCoreWMRef();
   auto am_proc = wm->GetCoreAMRef();

   std::shared_ptr<il::assignmentMessage> newAssignment(new il::assignmentMessage(am_proc->getGlobalLogger()));

   auto selected_weapon = SelectSelfDefenseWeapon(sim_time_s, this_plat, wm, track);
   if (!selected_weapon)
   {
      return false;
   }

   // create assignment based on this assessment
   newAssignment->create(sim_time_s,
                         wm_proc->getSelfDefenseShotDoctrine(),
                         am_proc->getThisAsset()->getID(), // initiating id
                         am_proc->getThisAsset()->getID(), // assigning id
                         track_id,
                         am_proc->getThisAsset()->getID(), // weapon unit id
                         selected_weapon->GetWeaponID(), // weapon id
                         wm_proc->getSelfDefenseShotExpiry(),
                         true,
                         false);

   // push it to the pending assignment array
   il::evaluationTrack evaluation_track(track, am_proc->getGlobalLogger());
   il::evaluationAssignment eval_assignment(evaluation_track, newAssignment);
   am_proc->CreateSelfDefenseAssignment(eval_assignment);

   // log assignment HEFIR
   //this_plat->GetSimulation();
   IADSC2SimulationExtension::Find(*this_plat->GetSimulation())->GetLogger()->Assignment(sim_time_s, &eval_assignment, std::string("SELF-DEFENSE"));

   // update our C2 perception for assignments and estimated munitions
   am_proc->getThisAsset()->updateC2ChainAssignedUnitStatus(newAssignment->getAssignedID(),
                                                            &am_proc->getAssets(),
                                                            il::assetRecord::DECREMENT,
                                                            1, // 1 assignment
                                                            newAssignment->getRequestedMissilesToCommit());

   return true;
}

std::shared_ptr<il::weaponRecord> WsfDefaultWeaponsManagerImpl::SelectSelfDefenseWeapon(const double sim_time_s, WsfPlatform* this_plat, WsfBMWeaponsManager* wm, const std::shared_ptr<il::trackRecord>& track)
{
   auto this_asset = wm->GetCoreAMRef()->getThisAsset();

   auto weapons = this_asset->getWeapons();
   for (auto it = weapons.begin(); it != weapons.end(); ++it)
   {
      auto& weapon = *it;
      if (weapon && weapon->GetTruthTotalMunitions())
      {
         return weapon;
      }
   }

   return nullptr;
}
