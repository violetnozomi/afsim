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
*   File: weaponPairing.cpp
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
#include <iadsLib/weaponPairing.hpp>
#include <iadsLib/interceptCalculatorInterface.hpp>
#include <iadsLib/terrainInterface.hpp>
#include <iadsLib/assessmentRecord.hpp>
#include <iadsLib/enum.hpp>

namespace il
{

WeaponPairing::WeaponPairing(const tReadyWeapons::value_type& weapon,
                             const evalTrackArray::value_type& track,
                             const std::shared_ptr< weaponTableRow >& best_preference,
                             const bool use_projection_delays,
                             const double assignment_delay,
                             const double expected_execution_delay,
                             const double max_projection_time_s,
                             const double max_projection_time_incr)
   : m_weapon_ref(weapon)
   , m_track_ref(track)
   , m_best_preference(best_preference)
   , m_use_projection_delays(use_projection_delays)
   , m_assignment_delay(assignment_delay)
   , m_expected_execution_delay(expected_execution_delay)
   , m_max_projection_time_s(max_projection_time_s)
   , m_max_projection_time_incr(max_projection_time_incr)
   , m_can_intercept(false)
   , m_estimated_pk(0)
   , m_estimated_tti(DBL_MAX)
   , m_weapon_max_range(DBL_EPSILON)
   , m_intercept_closing_time(DBL_MAX)
   , m_is_failed(false)
   , m_weapon_type_priority(0)
   , m_have_wsubtype_pref_match(false)
   , m_workload(0)
   , m_weapon_score_computed(false)
   , m_weapon_score(0)
{
   weaponTableRow::tWeaponPrefList wsys_pref_list;
   // set up preference since we know everything at this point
   if (m_weapon_ref->GetType().getType() == il::WEAPON_SAM)
   {
      m_weapon_type_priority = m_best_preference->getSAMPreference();
      wsys_pref_list = m_best_preference->getSAMWeaponPreferenceList();
   }
   else if (m_weapon_ref->GetType().getType() == il::WEAPON_AI)
   {
      m_weapon_type_priority = m_best_preference->getAIPreference();
      wsys_pref_list = m_best_preference->getAIWeaponPreferenceList();
   }
   else if (m_weapon_ref->GetType().getType() == il::WEAPON_OTHER)
   {
      m_weapon_type_priority = m_best_preference->getOtherPreference();
      wsys_pref_list = m_best_preference->getOtherWeaponPreferenceList();
   }
   else
   {
      m_failed_reason = "Invalid weapon type";
      m_is_failed = true;
   }

   // check for a valid weapon type priority
   if (!m_is_failed && (m_weapon_type_priority == 0))
   {
      m_failed_reason = "Excluded weapon type (weapon type priority (AI/SAM/Other) == 0)";
      m_is_failed = true;
   }

   // look for a matching weapon system priority that is non-exclusionary (> 0)
   if (!m_is_failed)
   {
      for (auto it = wsys_pref_list.begin(); it != wsys_pref_list.end(); ++it)
      {
         const auto& wsys_pref = *it;
         if (m_weapon_ref->GetType() == wsys_pref.getWeaponSystemType())
         {
            if (wsys_pref.getPriority() <= 0)
            {
               m_failed_reason = "Excluded weapon type (weapon subtype priority == 0)";
               m_is_failed = true;
               return;
            }
            else
            {
               m_wsys_match = wsys_pref;
               m_have_wsubtype_pref_match = true;
            }

            break;
         }
      }
   }

   // check that the weapon has munitions
   if (!m_is_failed && (weapon->GetEstimatedMunitionsPrepped() <= 0))
   {
      m_failed_reason = "No available munitions";
      m_is_failed = true;
   }

   // check that the weapon has fire channels
   if (!m_is_failed && (weapon->GetEstimatedAvailableFireChannels() <= 0))
   {
      m_failed_reason = "No available fire channels";
      m_is_failed = true;
   }

   // check that the parent asset has ready assignments
   if (!m_is_failed && (weapon->GetParentAsset()->getNumOpenAssignments() <= 0))
   {
      m_failed_reason = "No ready assignments";
      m_is_failed = true;
   }

   // compute workload
   if (weapon->GetParentAsset()->getMaxAssignments() > 0)
   {
      m_workload = ((double)weapon->GetParentAsset()->getMaxAssignments() -
                    (double)weapon->GetParentAsset()->getNumOpenAssignments()) /
                   (double)weapon->GetParentAsset()->getMaxAssignments();
   }
}

bool WeaponPairing::isFailed() const
{
   return m_is_failed;
}

std::string WeaponPairing::getFailedReason() const
{
   return m_failed_reason;
}

void WeaponPairing::setExternalFail(const std::string& reason, bool reset_intercept_ability)
{
   m_failed_reason = reason;
   m_is_failed = true;

   if (reset_intercept_ability)
   {
      m_can_intercept = false;
   }
}

bool WeaponPairing::checkWeaponStatusStale(const double sim_time, const double stale_time)
{
   bool is_stale = (sim_time - m_weapon_ref->GetParentAsset()->getStatusTime()) > stale_time;
   if (is_stale)
   {
      m_failed_reason = "Asset is stale";
      m_is_failed = true;
   }

   return is_stale;
}

bool WeaponPairing::checkCanWeaponIntercept(const double sim_time,
                                            std::shared_ptr<InterceptCalculatorIface>& xcept_calc,
                                            const std::shared_ptr<terrainInterface>& terrain_interface,
                                            double& tti, positionRecord& pred_pt,
                                            std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
{
   // interrogate the weapons intercept calculator
   m_can_intercept = canWeaponIntercept(sim_time,
                                        xcept_calc,
                                        nullptr,
                                        *m_track_ref.Track(),
                                        tti,
                                        pred_pt,
                                        m_estimated_pk,
                                        m_weapon_max_range,
                                        aGlobalLogger);
   if (!m_can_intercept)
   {
      m_failed_reason = "Intercept calculator could not find an intercept solution";
      m_is_failed = true;
   }
   else if (!m_best_preference->checkMinPK(m_estimated_pk))
   {
      // our pk doesn't pass
      m_failed_reason = "Min PK did not pass";
      m_is_failed = true;
      // reset the ability to intercept since although we can get there, it's too low of a probability of kill
      m_can_intercept = false;
   }
   else if ((m_weapon_ref->GetType().getType() == il::WEAPON_OTHER || m_weapon_ref->GetType().getType() == il::WEAPON_SAM) &&
            !checkWeaponHasLOS(terrain_interface, m_pred_track_at_launch.getPosition())) // ground weapon and no LOS at launch
   {
      // we won't have LOS at the time of launch
      m_failed_reason = "No line of sight at launch time";
      m_is_failed = true;
      // reset the ability to intercept since although we can get there, we won't be able to see the target at launch
      m_can_intercept = false;
   }


   if (m_can_intercept)
   {
      m_estimated_tti = tti;
      m_pred_intercept_pt = pred_pt;
   }

   return m_can_intercept;
}

double WeaponPairing::computeWeaponScore(const int weapon_type_weight,
                                         const int weapon_subtype_weight,
                                         const int intercept_rng_weight,
                                         const int intercept_time_weight,
                                         const int pk_weight,
                                         const int workload_weight)
{
   m_weapon_score_computed = true;

   // Compute the weapon score from the stored values. The asset score should be
   // scaled from 0.0 - 10.0 where 10.0 is best

   m_weapon_score = 0;
   int numfactors = 0;

   // sanity check
   if (m_is_failed || !m_can_intercept)
   {
      return m_weapon_score;
   }

   // Target Priority //
   // Set the initial score to the target priority.
   m_weapon_score = 11.0 - getTargetPriority();
   ++numfactors;

   // Weapon Type //
   // note that we should never have an invalid weapon type priority because if
   // we didn't have a match then the failed code should be zero and we shouldn't be here
   if (weapon_type_weight > 0)
   {
      m_weapon_score += weapon_type_weight * (11.0 - m_weapon_type_priority);
      numfactors += weapon_type_weight;
   }

   // Weapon Subtype //
   if (m_have_wsubtype_pref_match && weapon_subtype_weight > 0)
   {
      m_weapon_score += weapon_subtype_weight * (11.0 - m_wsys_match.getPriority());
      numfactors += weapon_subtype_weight;
   }

   // Distance Weight //
   auto slant_range = (m_weapon_ref->GetPosition().getXYZ() - m_pred_intercept_pt.getXYZ()).norm();
   if (intercept_rng_weight > 0)
   {
      // compute distance from threat to the predicted intercept position
      m_pred_intercept_pt.getXYZ();

      auto percent_max_range = (m_weapon_max_range - slant_range) / m_weapon_max_range;
      if (slant_range <= m_weapon_max_range && percent_max_range > 0.0)
      {
         m_weapon_score += intercept_rng_weight * percent_max_range * 10.0;
         numfactors += intercept_rng_weight;
      }
   }

   // Time weight //
   auto track_speed = m_track_ref.Track()->getVelocity();
   if (intercept_time_weight > 0 && track_speed > 0.0)
   {
      // Compute estimated Closing Time from target to own unit if the
      // target were to turn and fly directly at my unit.
      m_intercept_closing_time = slant_range / track_speed;
      if (m_intercept_closing_time > 0.0)
      {
         auto percent_intercept_time = (m_intercept_closing_time - m_estimated_tti) / m_intercept_closing_time;
         if (percent_intercept_time > 0.0)
         {
            m_weapon_score += intercept_time_weight * percent_intercept_time * 10.0;
            numfactors += intercept_time_weight;
         }
      }
   }

   // probablity of kill
   if (pk_weight > 0)
   {
      m_weapon_score += pk_weight * m_estimated_pk * 10.0;
      numfactors += pk_weight;
   }

   // workload //
   if (workload_weight > 0)
   {
      m_weapon_score += workload_weight * m_workload * 10.0;
      numfactors += workload_weight;
   }

   // Compute Final Score
   if (numfactors > 0)
   {
      m_weapon_score /= numfactors;
   }

   // score within bounds?
   if (m_weapon_score <= 0)
   {
      m_weapon_score = 0;
   }
   else if (m_weapon_score > 10.0)
   {
      m_weapon_score = 10.0;
   }

   return m_weapon_score;
}

double WeaponPairing::getWeaponScore() const
{
   return m_weapon_score;
}

bool WeaponPairing::getInterceptCalcs(double& est_pk,
                                      double& est_tti,
                                      positionRecord& pred_pt,
                                      double& xcept_range,
                                      double& max_range,
                                      double& closing_time)
{
   if (m_can_intercept)
   {
      est_pk = m_estimated_pk;
      est_tti = m_estimated_tti;
      pred_pt = m_pred_intercept_pt;
      max_range = m_weapon_max_range;
      closing_time = m_intercept_closing_time;
      xcept_range = (m_weapon_ref->GetPosition().getXYZ() - m_pred_intercept_pt.getXYZ()).norm();
   }

   return m_can_intercept;
}


bool WeaponPairing::checkWeaponHasLOS(const std::shared_ptr<terrainInterface>& terrain_interface,
                                      const positionRecord& track_pos)
{
   return !terrain_interface->IsLOSMasked(m_weapon_ref->GetParentAsset()->getPosition(), track_pos);
}

bool WeaponPairing::canWeaponIntercept(const double sim_time,
                                       std::shared_ptr<InterceptCalculatorIface>& xcept_calc,
                                       const std::shared_ptr<terrainInterface>& terrain_interface,
                                       const trackRecord& projected_track,
                                       double& tti,
                                       positionRecord& pred_pt,
                                       double& est_pk,
                                       double& weap_max_range,
                                       std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
{
   il::assessmentRecord assessment(xcept_calc, aGlobalLogger, terrain_interface);

   // Since this is coming from the BM, don't invoke weapon delay. Only the WM's utilize this. Otherwise, we get
   // into longest-delay projection wins counter-intuitive behavior
   if (m_weapon_ref->GetType().getType() == il::WEAPON_SAM)
   {
      assessment.AssessSAMEngagement(sim_time,
                                     m_weapon_ref,
                                     projected_track,
                                     m_use_projection_delays,
                                     m_assignment_delay,
                                     m_expected_execution_delay,
                                     m_max_projection_time_s,
                                     m_max_projection_time_incr);
   }
   else if (m_weapon_ref->GetType().getType() == il::WEAPON_AI)
   {
      assessment.AssessAIEngagement(sim_time,
                                    m_weapon_ref,
                                    projected_track,
                                    m_use_projection_delays,
                                    m_assignment_delay,
                                    m_max_projection_time_s,
                                    m_max_projection_time_incr);
   }

   bool can_intercept = assessment.canInterceptTrack();

   // project track forward by the estimated launch + c2 delay time
   if (can_intercept)
   {
      m_pred_track_at_launch = trackRecord::extrapolateTrack(projected_track, assessment.getProjectedTime() - sim_time);
      tti = assessment.getInterceptTime();
      pred_pt = assessment.getPredictedInterceptPt();
      est_pk = assessment.getInterceptPK();
      weap_max_range = assessment.getMaxRange();
   }

   return can_intercept;
}

} // namespace il
