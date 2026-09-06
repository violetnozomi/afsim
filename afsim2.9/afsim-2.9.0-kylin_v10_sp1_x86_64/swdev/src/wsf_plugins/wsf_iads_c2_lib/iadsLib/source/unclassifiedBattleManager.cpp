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
*   File: unclassifiedBattleManager.cpp
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
*   Abstract:
*
*******************************************************************************/

#include <cfloat>
#include <iadsLib/unclassifiedBattleManager.hpp>
#include <iadsLib/assetManagerInterface.hpp>
#include <iadsLib/util.hpp>
#include <vclMath/GreatCircle.hpp>
#include <iadsLib/weaponsManagerInterface.hpp>
#include <iadsLib/weaponsManagerAI.hpp>

namespace il
{

unclassifiedBattleManager::unclassifiedBattleManager(std::weak_ptr<logger::GlobalLogger> aGlobalLogger, const std::shared_ptr<terrainInterface> &terrain_interface)
   : BattleManagerInterface(terrain_interface)
   , m_stale_asset_time_s (60)
   , m_shot_doctrine(SHOOT_LOOK_SHOOT)
   , m_max_firing_time_s (360)
   , m_print_valid_units (false)
   , m_print_valid_unit_details (false)
   , m_print_invalid_unit_details (false)
   , m_weapon_table(aGlobalLogger)
   , m_weapon_type_weight (1)
   , m_weapon_subtype_weight (1)
   , m_intercept_rng_weight (1)
   , m_intercept_time_weight (1)
   , m_pk_weight (1)
   , m_workload_weight (1)
{
   m_target_projection_time_s = 0; // override any defaults that are for the unclass AE submodule
}

unclassifiedBattleManager::unclassifiedBattleManager (const unclassifiedBattleManager &from)
   : BattleManagerInterface (from)
   , m_stale_asset_time_s (from.m_stale_asset_time_s)
   , m_shot_doctrine (from.m_shot_doctrine)
   , m_max_firing_time_s (from.m_max_firing_time_s)
   , m_print_valid_units (from.m_print_valid_units)
   , m_print_valid_unit_details (from.m_print_valid_unit_details)
   , m_print_invalid_unit_details (from.m_print_invalid_unit_details)
   , m_weapon_table (from.m_weapon_table)
   , m_weapon_type_weight (from.m_weapon_type_weight)
   , m_weapon_subtype_weight (from.m_weapon_subtype_weight)
   , m_intercept_rng_weight (from.m_intercept_rng_weight)
   , m_intercept_time_weight (from.m_intercept_time_weight)
   , m_pk_weight (from.m_pk_weight)
   , m_workload_weight (from.m_workload_weight)
{

}

BattleManagerInterface *unclassifiedBattleManager::Clone ()
{
   return new unclassifiedBattleManager (*this);
}

bool unclassifiedBattleManager::run(il::moeLoggingInterface& aMOELogger)
{
   if (!m_asset_manager->isInitialized())
      return false;

   if (m_print_valid_units || m_print_valid_unit_details || m_print_invalid_unit_details)
   {
      HCL_DEBUG_LOGGER (m_asset_manager->getGlobalLogger()) << std::setprecision (2) << std::fixed
                          << m_asset_manager->getCurrentTime() << ": "
                          << m_asset_manager->getThisAsset()->getName()
                          << " Assessing Weapon / Track Assignments";

      if (!m_EvaluationTracks.size())
      {
         HCL_DEBUG_LOGGER(m_asset_manager->getGlobalLogger()) << "    No tracks.";
         return false;
      }
   }

   // build the weapon-pass filters
   tReadyWeapons weapon_pass_arrays[NUM_PASSES];
   buildWeaponArrays (weapon_pass_arrays);

   // perform the weapon assessments
   ePasses pass_indices[] = { PASS_1, PASS_2, PASS_3, PASS_4, PASS_5, PASS_6 };
   for (std::size_t pass = 0; pass < sizeof (pass_indices)/sizeof (pass_indices[0]); ++pass)
   {
      auto &weapons = weapon_pass_arrays[pass_indices[pass]];

      if (!weapons.size())
         continue;

      if (m_print_valid_units || m_print_valid_unit_details || m_print_invalid_unit_details)
      {
         if (m_commit_authority)
            HCL_DEBUG_LOGGER(m_asset_manager->getGlobalLogger()) << "Pass " << pass_indices[pass] + 1 << ": "
                            << "Assign master tracks using "
                            << weapons.size() << " weapons. (Evaluation Tracks:"
                            << m_EvaluationTracks.size() << ")";
         else
            HCL_DEBUG_LOGGER(m_asset_manager->getGlobalLogger()) << "Pass " << pass_indices[pass] + 1 << ": "
                               << "Delegate received assignments using "
                               << weapons.size() << " weapons. (Evaluation Tracks:"
                               << m_EvaluationTracks.size() << ")";
      }


      // since we're operating on evaluation tracks, we don't need to distinguish between tracks based off
      // assignments vs. tracks since this has already been done and rolled into the evaluation tracks array.
      tPairingArray pass_pairings; // pairings for unassigned tracks to weapons for this pass
      PerformWeaponPairings (aMOELogger, weapons, m_EvaluationTracks, pass_pairings);

      AssignWeapons (pass_pairings, pass_indices[pass] + 1);

   }

   CANTCOUnallocatedDelegations (aMOELogger, "No weapon preference");

   return true;
}


void unclassifiedBattleManager::runMonitorAssignments(il::moeLoggingInterface& aMOELogger, const double SimTime)
{
   MapAssignmentArrayFunc
      (aMOELogger, m_asset_manager->getAssignmentArray(),
       [SimTime](il::moeLoggingInterface& aMOELogger, evalAssignmentMultimap &assignments, evalAssignmentMultimap::iterator &assigned_track_itor, evalAssignmentMultimap::iterator &assignment_itor, unclassifiedBattleManager *bm_ptr) -> void
       {
          bool cancelled = false;

          if (!assignment_itor->second->isAssignmentComplete ())
          {
             // check for max firing time exceeded
             if (SimTime - assignment_itor->second->getAssignTime() > bm_ptr->getMaxFiringTime() &&
                 assignment_itor->second->getSalvosFired() == 0)
             {
                bm_ptr->getAssetManagerRef()->cancelAssignment (aMOELogger, assignment_itor->second, "Max firing time exceeded");
                cancelled = true;
             }

             // asset manager runs a basic monitor assignments that checks for loss of master track, no need to check it here since
             // these are more specific to the battle manager at hand
          }

          // do some logging
          if (bm_ptr->getPrintValidUnits() || bm_ptr->getPrintValidUnitDetails())
             HCL_DEBUG_LOGGER(bm_ptr->GetAssetManager()->getGlobalLogger()) << "    " << bm_ptr->getAssetManagerRef()->getThisAsset()->getName()
                                << " assessing active Assignment with TAN " << assignment_itor->second.Track()->getID()
                                << ": TGT-" << assignment_itor->second.Track()->getTargetTruthName()
                                << ". Assigned ID " << assignment_itor->second->getAssignedID()
                                << ". Status - "    << (cancelled ? "Max firing time exceeded" : "awaiting prosecution");

          // increment to the next assignment
          ++assignment_itor;
       },
       this);
}


void unclassifiedBattleManager::buildWeaponArrays (tReadyWeapons (&weapon_pass_arrays)[NUM_PASSES])
{
   auto my_asset = m_asset_manager->getThisAsset ();
   auto &assets = m_asset_manager->getAssets();
   HCL_DEBUG_LOGGER(m_asset_manager->getGlobalLogger()) << "\t Building weapon ready list...";
   tReadyWeapons weapon_list;
   std::tie(weapon_list, std::ignore) = my_asset->getReadyWeaponList(assets);

   // run 6-pass filters here for splitting out the weapons list
   for (auto it = weapon_list.begin(); it != weapon_list.end(); ++it)
   {
      const auto &weapon = *it;
      if (pass1Succeeds (weapon))
         weapon_pass_arrays[PASS_1].push_back (weapon);
      else if (pass2Succeeds (weapon))
         weapon_pass_arrays[PASS_2].push_back (weapon);
      else if (pass3Succeeds (weapon))
         weapon_pass_arrays[PASS_3].push_back (weapon);
      else if (pass4Succeeds (weapon))
         weapon_pass_arrays[PASS_4].push_back (weapon);
      else if (pass5Succeeds (weapon))
         weapon_pass_arrays[PASS_5].push_back (weapon);
      else if (pass6Succeeds (weapon))
         weapon_pass_arrays[PASS_6].push_back (weapon);
   }
}

bool unclassifiedBattleManager::pass1Succeeds (const tReadyWeapons::value_type &weapon)
{
   /* WeaponsPass 1: Direct subordinate weapons that utilize a zone */
   return m_asset_manager->getThisAsset ()->isDirectSubordinate (weapon->GetParentAssetID()) &&
          weapon->HasZones();
}

bool unclassifiedBattleManager::pass2Succeeds (const tReadyWeapons::value_type &weapon)
{
   /* WeaponsPass 2: Direct subordinate weapons that do not utilize a zone */
   return m_asset_manager->getThisAsset ()->isDirectSubordinate (weapon->GetParentAssetID()) &&
          !weapon->HasZones();
}

bool unclassifiedBattleManager::pass3Succeeds (const tReadyWeapons::value_type &weapon)
{
   /* WeaponsPass 3: All subordinate weapons that satisfy this condition:
         My direct C2 subordinate, in the chain-of-command to the subordinate
         weapon, must utilize a zone. */
   auto my_asset = m_asset_manager->getThisAsset ();
   auto &assets = m_asset_manager->getAssets();

   auto next_sub = my_asset->findNextSubordinateInChain (weapon->GetParentAsset()->getID(), assets);
   auto next_sub_ptr = assets.GetAsset (next_sub);
   // if we have a valid direct subordinate on the chain of command to the weapon and it has zones, Pass.
   return next_sub_ptr && next_sub_ptr->hasZones() && weapon->HasZones();
}

bool unclassifiedBattleManager::pass4Succeeds (const tReadyWeapons::value_type &weapon)
{
   /* WeaponsPass 4: All subordinate weapons that satisfy this condition:
         Any C2 subordinate, other than direct, in the chain-of-command to
         the subordinate weapon, must utilize a zone. */
   bool any_zones = false;

   auto my_asset = m_asset_manager->getThisAsset ();
   auto &assets = m_asset_manager->getAssets();

   // get first subordinate (not required to utilize zone) down to weapons commander
   auto next_sub = my_asset->findNextSubordinateInChain (weapon->GetParentAsset()->getID(), assets);
   auto next_sub_ptr = assets.GetAsset (next_sub);
   if (next_sub_ptr)
   {
      next_sub = next_sub_ptr->findNextSubordinateInChain (weapon->GetParentAsset()->getID(), assets);
      any_zones = assetRecord::doesAnyChainOfCommandUseZone (assets, next_sub, weapon->GetParentAsset()->getCommanderID());
   }

   if (any_zones && weapon->HasZones())
      return true;

   return false;
}

bool unclassifiedBattleManager::pass5Succeeds (const tReadyWeapons::value_type &weapon)
{
   /* WeaponsPass 5: All subordinate weapons that satisfy these conditions:
      a) NO C2 subordinate, including direct, in the chain-of-command to
      the subordinate weapon, may utilize a zone.
      b) The weapon must utilize a zone. */

   bool any_zones = false;

   auto my_asset = m_asset_manager->getThisAsset ();
   auto &assets = m_asset_manager->getAssets();

   // get first subordinate (not required to utilize zone) down to weapons commander
   auto next_sub = my_asset->findNextSubordinateInChain (weapon->GetParentAsset()->getID(), assets);
   auto next_sub_ptr = assets.GetAsset (next_sub);
   if (next_sub_ptr)
   {
      next_sub = next_sub_ptr->findNextSubordinateInChain (weapon->GetParentAsset()->getID(), assets);
      any_zones = assetRecord::doesAnyChainOfCommandUseZone (assets, next_sub, weapon->GetParentAsset()->getCommanderID());
   }

   if (!any_zones && weapon->HasZones())
      return true;

   return false;
}

bool unclassifiedBattleManager::pass6Succeeds (const tReadyWeapons::value_type &weapon)
{
   /* WeaponsPass 6: All subordinate weapons that satisfy this conditions:
         No zones used in the chain-of-command to the subordinate weapon
         (weapons kinematics check will be performed during assessment). */

   // note: the passes are meant to build on each other 1->6, by this point, based on all the prior checks,
   // the above condition is true, pass the weapon
   return true;
}

void unclassifiedBattleManager::PerformWeaponPairings (il::moeLoggingInterface& aMOELogger, const tReadyWeapons &weapons, evalTrackArray &eval_tracks, tPairingArray &pairings)
{
   const auto assigning_asset = GetAssetManager()->getThisAsset();
   const auto &asset_map = GetAssetManager()->getAssets();

   for (auto it_track = eval_tracks.begin(); it_track != eval_tracks.end(); ++it_track)
   {
      auto &track = *it_track;

      // check that this track isn't already assigned - if it is, skip it. This happens as we iterate through the passes
      // and make inline assignments back to the asset manager
      if (getNumAssignmentsPending (track) > 0)
         continue;

      // get weapon preference record
      auto weapon_row = m_weapon_table.getWeaponTableRow (track.Track(), false);
      if (!weapon_row)
      {
         if (track.DelegateAssignment())
            // there is no matching weapon row and the track was based off a delegate assignment, we must cantco it
            m_asset_manager->cantcoAssignment (aMOELogger, track.DelegateAssignment(), "No weapon preference", true, true);

         continue;
      }

      for (auto it_weapon = weapons.begin(); it_weapon != weapons.end(); ++it_weapon)
      {
         const auto &weapon = *it_weapon;
         auto assignment_delay = il::assetRecord::calculateAssignmentDelays (asset_map, assigning_asset->getID(), GetAssignedIDFromWeaponRef (weapon));

         double expected_execution_delay = 0;
         auto wm = weapon->GetWeaponsManager();
         if (wm)
         {
            expected_execution_delay = wm->getExpectedExecutionDelay();
         }

         std::shared_ptr<WeaponPairing> pairing_rec (new WeaponPairing(weapon, track, weapon_row, getProjectTracksByDelays(), assignment_delay, expected_execution_delay, getTargetProjectionTime(), getTargetProjectionTimeIncrement()));

         if (IsUnitCapableOfAssignment (m_asset_manager->getExclusionArray(), GetAssignedIDFromWeaponRef (weapon), track.GetReferenceTrackID()))
         {
            AssessWeapon (*pairing_rec);
         }
         else
            pairing_rec->setExternalFail ("Unit is excluded");

         // if the pairing has not failed, compute the weapon score here - note: results are cached
         if (!pairing_rec->isFailed())
            pairing_rec->computeWeaponScore (m_weapon_type_weight,    m_weapon_subtype_weight,
                                            m_intercept_rng_weight,  m_intercept_time_weight,
                                            m_pk_weight,             m_workload_weight);

         // add all pairings so logging is simplified - need to be a little careful on the sorting/selection side
         pairings.push_back (pairing_rec);
      }
   }
}

void unclassifiedBattleManager::AssessWeapon (WeaponPairing &pairing)
{
   double time_to_intercept = DBL_MAX;
   positionRecord pred_pt;

   // check for pairing failures (could already be failed)
   if (pairing.isFailed() ||
       pairing.checkWeaponStatusStale (m_asset_manager->getCurrentTime(), m_stale_asset_time_s) ||
       !pairing.checkCanWeaponIntercept (GetAssetManager()->getCurrentTime(), m_xcept_iface, m_terrain_interface, time_to_intercept, pred_pt, m_asset_manager->getGlobalLogger()))
      return;

   // at this point, we're reasonably confident we can intercept the threat successfully, now we need to ensure we are adhering to
   // zone assignments if any exist
   const auto &weapon = pairing.getWeapon();

   // Ignore zones if weapon has a weaponsManagerAI and the Engagement Assessment Methodology
   auto wm = weapon->GetWeaponsManager();
   bool ignoreZones = false;
   if (wm)
   {
      auto wmai = dynamic_cast<weaponsManagerAI*>(wm.get());
      if (wmai)
         ignoreZones = wmai->getAssessEngageVia() == MUNITION_INTERCEPT_PT_IGNORE_ZONE;
   }

   // TODO NH - This code was previously reached because intercept solutions were not correctly checked to ensure they were in zones prior to acceptance. These debug statements should be relocated to be reachable.
   // direct weapon zones
   if (weapon->HasZones() && !ignoreZones)
   {
      if (weapon->GetType().getType() != il::WEAPON_AI && !IsInsideZone (weapon->GetAttachedZones(), pred_pt, false))
         pairing.setExternalFail ("Predicted intercept point not in weapon's zone");
      else if (weapon->GetType().getType() == il::WEAPON_AI &&
               !IsTrackProjectedInsideZone (weapon->GetAttachedZones(), pairing.getTrack().Track()->getPosition(),  pairing.getTrack().Track()->getHeadingRads(), false))
         pairing.setExternalFail ("Track not projected inside weapon zone");
   }

   // intermediate C2 zones (including self and weapon C2 zones (non-weapon zones))
   // note: we don't check this for Air Interceptor weapon types since the intermediate C2 concept is really a land-based concept
   // for SAMs that have intermediate C2 DA zones, we adhere strictly to the zone enforcement here. If the track isn't projected or currently in the zone,
   // fail it.
   if (!pairing.isFailed() && weapon->GetType().getType() != il::WEAPON_AI && assetRecord::doesAnyChainOfCommandUseZone (m_asset_manager->getAssets(), m_asset_manager->getThisAsset()->getID(), weapon->GetParentAsset()->getID()))
   {
      const auto &pred_track_at_launch = pairing.getProjectedTrackAtLaunchTime();
      if (!IsTrackProjectedInsideIntermediateC2Zones (m_asset_manager->getAssets(), m_asset_manager->getThisAsset()->getID(), weapon->GetParentAsset()->getID(), pred_track_at_launch.getPosition(), pred_track_at_launch.getHeadingRads(), false))
        pairing.setExternalFail ("Predicted intercept point not in intermediate C2 zones");
   }

}

void unclassifiedBattleManager::AssignWeapons (tPairingArray &pairings, int pass_number)
{
   static const char *pass_debug_header[] = { "\t*error* invalid pass number",
                                              "\tPASS 1: Direct subordinate weapons with zone.",
                                              "\tPASS 2: Direct subordinate weapons without zone.",
                                              "\tPASS 3: Direct subordinate C2 with zone.",
                                              "\tPASS 4: Intermediate subordinate C2 with zone.",
                                              "\tPASS 5: No C2 Zones. Weapon has zone.",
                                              "\tPASS 6: No Zones. Use weapon kinematics." };

   // print some debug
   if (m_print_valid_units || m_print_valid_unit_details || m_print_invalid_unit_details)
      HCL_DEBUG_LOGGER(m_asset_manager->getGlobalLogger()) << pass_debug_header[pass_number];

   // sort pairings before we compare weapon scores
   PrioritizePairingsGlobally (pairings);

   // the scoring array works like this:
   // track, pairing
   // the tracks are sorted in order of priority and then the weapons are then sorted in order of score within the track row
   weaponScoringMatrix scoring_matrix (pairings);

   for (int rank = 1; rank <= (int)scoring_matrix.GetNumRankedTracks(); ++rank)
   { // for all ranked tracks
      auto &track_pairings = scoring_matrix.GetPairingForTrackRank(rank);
      assert (track_pairings.size() > 0);

      if (m_print_valid_units || m_print_valid_unit_details || m_print_invalid_unit_details)
         HCL_DEBUG_LOGGER(m_asset_manager->getGlobalLogger()) << "\t\tAssessing Track: " << track_pairings[0]->getTrack().Track()->getTargetTruthName() << ". TAN: " << track_pairings[0]->getTrack().Track()->getID() << ". UnitID: " << track_pairings[0]->getTrack().Track()->getTargetTruthID();

      tPairingArray::value_type highest_score;

      for (auto it = track_pairings.begin(); it != track_pairings.end(); ++it)
      {
         const auto &weapon_pairing = *it;
         const auto &eval_track = weapon_pairing->getTrack();
         const auto &track = weapon_pairing->getTrack().Track();
         const auto &weapon = weapon_pairing->getWeapon();

         // check that we haven't already made an assignment decision in this or a previous pass
         if (getNumAssignmentsPending (eval_track))
         {
            HCL_DEBUG_LOGGER(m_asset_manager->getGlobalLogger()) << "Track " << track->getID() << " already has pending assignments, not considering further...";
            continue;
         }

         if (weapon_pairing->isFailed())
         {
            if (m_print_invalid_unit_details)
               HCL_DEBUG_LOGGER(m_asset_manager->getGlobalLogger()) << "\t\t\t" << weapon->GetParentAsset()->getName() << ": weapon id " << weapon->GetWeaponID()
                                  << " - Failed: " << weapon_pairing->getFailedReason();
         }
         else
         { // valid pairing
            if (m_print_valid_units || m_print_valid_unit_details)
            {
               HCL_DEBUG_LOGGER(m_asset_manager->getGlobalLogger()) << "\t\t\t" << weapon->GetParentAsset()->getName() << ": weapon id " << weapon->GetWeaponID()
                                  << " - Weapon is valid";

               if (m_print_valid_unit_details)
               {
                  double pk = 0, tti = 0, xcept_range = 0, max_range = 0, closing_time = 0;
                  positionRecord pred_pt;
                  weapon_pairing->getInterceptCalcs (pk, tti, pred_pt, xcept_range, max_range, closing_time);

                  HCL_DEBUG_LOGGER(m_asset_manager->getGlobalLogger()) << std::fixed << std::setprecision (2)
                                     << "\t\t\t\tNOTE: Ignore RA and PM if not direct sub wpn\n"
                                     << "\t\t\t\tReady Assignments " << weapon->GetParentAsset()->getNumOpenAssignments()
                                     << "    Max Assignments "       << weapon->GetParentAsset()->getMaxAssignments() << std::endl
                                     << "\t\t\t\tPrepped Missiles  " << weapon->GetEstimatedMunitionsPrepped()
                                     << "    Missiles Fired  "       << weapon->GetShotsFiredAtTrack (track->getID()) << std::endl
                                     << "\t\t\t\tWSPT Type         " << weapon_pairing->getTypePreference()
                                     << "    WSPT SubType    "       << weapon_pairing->getSubtypePreference() << std::endl
                                     << "\t\t\t\tZone              " <<  BoolToString (weapon->HasZones())
                                     << "    Projected Zone  "       << BoolToString (weapon->HasZones()) << std::endl
                                     << "\t\t\t\tPK                " << pk << std::endl
                                     << "\t\t\t\tIntercept Time    " << tti << std::endl
                                     << "\t\t\t\tClosing Time      " << closing_time << std::endl
                                     << "\t\t\t\tIntercept Range   " << xcept_range << std::endl
                                     << "\t\t\t\tMax Wpn Range     " << max_range << std::endl
                                     << "\t\t\t\tWeapon Score      " << weapon_pairing->getWeaponScore() << std::endl;
               }

            }

            // this isn't absolutely necessary since our pairings are already sorted by highest score but
            // the HELIOS model does print all assessments if print details is selected and this is where
            // we do it.
            if (!highest_score || weapon_pairing->getWeaponScore() > highest_score->getWeaponScore())
            {
               // ensure the weapon is assignable
               if (CheckWeaponCapableOfAssignment (weapon, eval_track.GetReferenceTrackID()))
                  highest_score = weapon_pairing;
            }
         } // valid pairing

      } // for all weapon pairings

      // if we got a best score, allocate the assignment, if not, it means we didn't have a valid intercept
      if (highest_score)
      {
         const auto &initiating_unit_id = highest_score->getTrack().DelegateAssignment() ? highest_score->getTrack().DelegateAssignment()->getInitiatingID() : m_asset_manager->getMyAsset().getID();
         const auto &assigning_unit_id = m_asset_manager->getThisAsset()->getID();
         const auto &weapon_unit_id = highest_score->getWeapon()->GetParentAssetID();
         const auto &weapon_id = highest_score->getWeapon()->GetWeaponID();
         const auto &evaluation_track = highest_score->getTrack();

         makeAssignment (initiating_unit_id, assigning_unit_id, weapon_unit_id, weapon_id, evaluation_track, m_shot_doctrine, m_max_firing_time_s);
      }

   } // for all ranked tracks
}

void unclassifiedBattleManager::PrioritizePairingsGlobally (tPairingArray &pairings) const
{
   // Sort the weapon pairings based on priority but not weapon score. This is done to sort
   // pairings before weapon assignment and then sub-sorting based on weapon score.
   std::sort (pairings.begin(), pairings.end(), [] (const tPairingArray::value_type &lhs, const tPairingArray::value_type &rhs) -> bool
   {
      // strict weak ordering

      // note: we do not want failed weapon codes in this array, they should not have made it this far
      if (lhs->getTargetPriority() > rhs->getTargetPriority()) // lower is better
         return true;
      if (rhs->getTargetPriority() > lhs->getTargetPriority()) // lower is better
         return false;

      if (lhs->getComputedClosingTime() > rhs->getComputedClosingTime()) // lower is better
         return true;
      if (rhs->getComputedClosingTime() > lhs->getComputedClosingTime()) // lower is better
         return false;

      if (lhs->getTrack().Track()->getID() < rhs->getTrack().Track()->getID())
         return true;
      if (rhs->getTrack().Track()->getID() < lhs->getTrack().Track()->getID())
         return false;

      if (lhs->getWeapon()->GetWeaponID() < rhs->getWeapon()->GetWeaponID())
         return true;

      return false;
   });
}

bool unclassifiedBattleManager::CheckWeaponCapableOfAssignment (const tReadyWeapons::value_type &weapon, const idRecord &track_id) const
{
   bool wpn_available = false;

   // trace logging
   auto preamble = [weapon] () -> std::string
                              { std::stringstream strm_preamble;
                                strm_preamble << "Cannot assign to weapon: " << weapon->GetParentAsset()->getName() << ": "
                                              << weapon->GetType().getTypeStr() << "/" << weapon->GetType().getSubTypeStr()
                                              << " --  ";
                                return strm_preamble.str(); };

   // ensure we have munitions, ready assignments, fire channels, and intermediate C2 open assignments
   // this check is needed here because as we start to make pending assignments, asset perception
   // estimates are updated to account for assignments
   std::string offender_units;
   if (weapon->GetEstimatedAvailableFireChannels() <= 0)
      HCL_TRACE_LOGGER(m_asset_manager->getGlobalLogger()) << preamble() << "No available fire channels";
   else if (weapon->GetEstimatedMunitionsPrepped() <= 0)
      HCL_TRACE_LOGGER(m_asset_manager->getGlobalLogger()) << preamble() << "No estimated munitions prepped";
   else if (weapon->GetParentAsset()->getNumOpenAssignments() <= 0)
      HCL_TRACE_LOGGER(m_asset_manager->getGlobalLogger()) << preamble() << "No open assignments";
   else if (!assetRecord::doesAllChainOfCommandHaveReadyAssignments (m_asset_manager->getAssets(), m_asset_manager->getMyAsset().getID(), weapon->GetParentAssetID(), offender_units))
      HCL_TRACE_LOGGER(m_asset_manager->getGlobalLogger()) << preamble() << "Not all chain of command has ready assignments (" << offender_units << ")";
   else if (m_asset_manager->getExclusionArray().isAssetExcluded (track_id, GetAssignedIDFromWeaponRef (weapon)))
      HCL_TRACE_LOGGER(m_asset_manager->getGlobalLogger()) << preamble() << "Weapon is excluded from engaging track: " << track_id << " systemic exclusion. ";
   else
      wpn_available = true;

   return wpn_available;
}

} // namespace il
