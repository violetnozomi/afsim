// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  @file assessmentRecord.hpp
 *  @author Kenneth R. Sewell III \n
 *          Kenneth.Sewell@wpafb.af.mil \n
 *          AFRL/RYWF \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004
 *
 *  Classification: UNCLASSIFIED
 *  Derived from gtibmifassessrec.spc 23 Aug 06 - Created. MDMcAllister
 */

#include <stdio.h>
#include <float.h>
#include <iostream>
#include <sstream>
#include <logger.hpp>
#include <iadsLib/assessmentRecord.hpp>
#include <iadsLib/util.hpp>
#include <vclMath/GreatCircle.hpp>
#include <vclMath/ECEFToGeodetic.hpp>
#include <iadsLib/assignmentMessage.hpp>
#include <iadsLib/threatRecordInterface.hpp>
#include <iadsLib/NumericUtils.hpp>
#include <iadsLib/weaponsManagerSAM.hpp>
#include <iadsLib/weaponsManagerAI.hpp>

#define DEBUG_ASSESS 1

namespace il
{

assessmentRecord::assessmentRecord(std::shared_ptr<InterceptCalculatorIface> xcept_iface,
                                   std::weak_ptr<logger::GlobalLogger> aGlobalLogger,
                                   std::shared_ptr<terrainInterface> terrain_interface)
   : m_WeaponExcluded(false)
   , m_ThreatInsideMaxRange(true)
   , m_ThreatOutsideMinRange(true)
   , m_CanInterceptTrack(false)
   , m_IsLOSMasked(false)
   , m_InterceptInsideMaxRange(true)
   , m_InterceptOutsideMinRange(true)
   , m_InterceptInsideZone(false)
   , m_InterceptInsideC2Zone(false)
   , m_ProjectedTime(DBL_MAX)
   , m_InterceptAltitude(DBL_MAX)
   , m_PotentialShots(0)
   , m_DeflectionAngleRads(DBL_MAX)
   , m_DistanceToPCA(DBL_MAX)
   , m_InterceptTime(DBL_MAX)
   , m_InterceptDistance(DBL_MAX)
   , m_InterceptPK(0)
   , m_MaxRange(0)
   , m_AssignmentDelay(0)
   , m_TimeToFireDelay(0)
   , m_AssignmentTime(DBL_MAX)
   , m_KeepOutDA(0)
   , m_Priority(0)
   , m_weapon_zone_types(E_NO_ZONES_SET)
   , m_xcept_iface(xcept_iface)
   , m_terrain_interface(terrain_interface)
   , mGlobalLogger(aGlobalLogger)
{
   // allow nullptr intercept interfaces since AIs may be deployed without a script intercept calculator
   // perform a sanity check for the required interfaces
   //if (!m_xcept_iface)
   //{
   //   HCL_FATAL_LOGGER(aGlobalLogger) << "assessmentRecord(): intercept interface is null -- cannot instantiate any assessments";
   //   throw std::runtime_error ("assessmentRecord(): intercept interface is null -- cannot instantiate any assessments");
   //}

   // allow nullptr terrain interfaces since some may want to check that themselves
}

// sets:
// everything GetEarliestInterceptInZone sets +
// m_Weapon
// m_weapon_zone_types
// m_DeflectionAngleRads
// m_ClosestApproach
// m_DistanceToPCA
// m_CanInterceptTrack
// m_InterceptTime
// m_InterceptDistance
// m_InterceptPoint
// m_ProjectedTime
// m_InterceptPK
// m_MaxRange
// m_AssignmentDelay
// m_TimeToFireDelay
// m_IsLOSMasked
// m_AssignmentTime
// m_InterceptInsideMaxRange
// m_InterceptOutsideMinRange
// m_InterceptInsideZone
void assessmentRecord::AssessSAMEngagement(const double simTime,
                                           const std::shared_ptr<weaponRecord>& weapon,
                                           const trackRecord& track,
                                           const bool use_delays_for_projection,
                                           const double assignment_delay,
                                           const double expected_execution_delay,
                                           const double projection_time_s,
                                           const double projection_incr_s)
{
   if (!m_xcept_iface)
   {
      HCL_FATAL_LOGGER(mGlobalLogger) << "assessmentRecord(): intercept interface is null -- cannot instantiate any assessments";
      throw std::runtime_error("assessmentRecord(): intercept interface is null -- cannot instantiate any assessments");
   }

   m_Weapon = weapon;
   m_weapon_zone_types = E_NO_ZONES_SET;

   auto wm = dynamic_cast<weaponsManagerSAM*>(weapon->GetWeaponsManager().get());
   if (!wm)
   {
      HCL_ERROR_LOGGER(mGlobalLogger) << " assessmentRecord::AssessSAMEngagement(): Weapon " << weapon->GetParentAssetID().getIDString() << ":" << weapon->GetWeaponID().getIDString() << " has no attached weaponsManagerSAM. Unable to assess SAM engagement.";
      return;
   }

   // assignment delay
   m_AssignmentDelay = assignment_delay;
   auto real_assignment_delay = m_AssignmentDelay;
   if (!use_delays_for_projection)
   { m_AssignmentDelay = 0; }

   m_TimeToFireDelay = weapon->GetTimeToFire(); // always do draw in case it modifies underlying RNG stream
   auto real_ttf = m_TimeToFireDelay;
   if (!use_delays_for_projection)
   { m_TimeToFireDelay = 0; }

   m_ExpectedExecutionDelay = expected_execution_delay;
   auto real_execution_delay = m_ExpectedExecutionDelay;
   if (!use_delays_for_projection)
   { m_ExpectedExecutionDelay = 0; }

   // project track forward
   double time_since_collection = simTime - track.getTime();
   HCL_TRACE_LOGGER(mGlobalLogger) << " assessmentRecord::AssessSAMEngagement(): Use delays for projection? " << (use_delays_for_projection ?  "true" : "false")
                      << ", Assignment delay = " << m_AssignmentDelay << ", Real assignment delay = " << real_assignment_delay
                      << ", Time to fire delay = " << m_TimeToFireDelay << " Real ttf = " << real_ttf
                      << ", Expected execution delay = " << m_ExpectedExecutionDelay << " Real execution delay = " << real_execution_delay;

   trackRecord projectedThreat = trackRecord::extrapolateTrack(track, m_TimeToFireDelay + m_AssignmentDelay + m_ExpectedExecutionDelay + time_since_collection);


   // Calculate deflection angle off nose from threat to weapon.
   m_DeflectionAngleRads = std::abs(angleOffset(projectedThreat.getPosition(), projectedThreat.getHeadingRads(), weapon->GetPosition()));

   // Calculate point of closest approach.
   m_ClosestApproach = findPointOfClosestApproach(track.getPosition(),
                                                  projectedThreat.getPosition(),
                                                  m_DeflectionAngleRads,
                                                  track.getVelocity(),
                                                  weapon->GetPosition());


   // Calculate distance from weapon to point of closest approach.
   m_DistanceToPCA = vm::GreatCircle<double>::calculateDistanceInMeters(weapon->GetPosition().getLLA().getLat() * vm::RADIANS_TO_DEGREES,
                                                                        weapon->GetPosition().getLLA().getLon() * vm::RADIANS_TO_DEGREES,
                                                                        m_ClosestApproach.getLLA().getLat() * vm::RADIANS_TO_DEGREES,
                                                                        m_ClosestApproach.getLLA().getLon() * vm::RADIANS_TO_DEGREES);

#if DEBUG_ASSESS
   HCL_TRACE_LOGGER(mGlobalLogger) << "PCA (Lat/Lon): " << m_ClosestApproach.getLatDegs() << ", " << m_ClosestApproach.getLonDegs()
                      << ", Weapon = " << weapon->GetParentAsset()->getName() << ":" << weapon->GetName()
                      << ", Distance to PCA = " << m_DistanceToPCA;
#endif

   // Calculate if we can intercept, and the earliest intercept.
   m_CanInterceptTrack = GetEarliestSAMInterceptInZone(simTime,
                                                       projectedThreat,
                                                       weapon,
                                                       projection_time_s,
                                                       projection_incr_s,
                                                       m_InterceptTime,
                                                       m_InterceptDistance,
                                                       m_InterceptPoint,
                                                       m_ProjectedTime,
                                                       m_InterceptPK,
                                                       m_MaxRange,
                                                       true);

   if (m_CanInterceptTrack && m_terrain_interface)
   {
      m_IsLOSMasked = m_terrain_interface->IsLOSMasked(weapon->GetPosition(), m_InterceptPoint);
      m_CanInterceptTrack = !m_IsLOSMasked;
   }

   // assignment time may diverge from projection time here
   // note: we still incurr the real TTF penalty even if we're not taking into
   // account weapon TTF for track projections
   m_AssignmentTime = simTime + m_ProjectedTime;
   m_ProjectedTime = m_AssignmentTime + real_ttf + real_assignment_delay + real_execution_delay;
   m_InterceptTime += real_assignment_delay + real_ttf + real_execution_delay;

   {
      std::ostringstream minMaxRange;
      if (weapon->HasMinRangeAttribute())
      {
         minMaxRange << weapon->GetMinRange();
      }
      else
      {
         minMaxRange << "unknown";
      }
      minMaxRange << '/';
      if (weapon->HasMaxRangeAttribute())
      {
         minMaxRange << weapon->GetMaxRange();
      }
      else
      {
         minMaxRange << "unknown";
      }
      HCL_TRACE_LOGGER(mGlobalLogger) << "In Assess(): m_InterceptDistance = " << m_InterceptDistance << ", weapon min/max range: " << minMaxRange.str();
   }

   // don't filter if the weapon doesn't broadcast absolute ranges - set to success if the weapon can intercept in this case
   m_InterceptInsideMaxRange =  weapon->HasMaxRangeAttribute() ? (m_InterceptDistance < weapon->GetMaxRange()) : m_CanInterceptTrack;
   m_InterceptOutsideMinRange = weapon->HasMinRangeAttribute() ? (m_InterceptDistance > weapon->GetMinRange()) : m_CanInterceptTrack;
   m_InterceptInsideZone = (m_InterceptInsideMaxRange && m_InterceptOutsideMinRange && m_CanInterceptTrack);
}

// sets:
// everything GetEarliestInterceptInZone sets +
// m_Weapon
// m_weapon_zone_types
// m_DeflectionAngleRads
// m_ClosestApproach
// m_DistanceToTarget
// m_CanInterceptTrack
// m_InterceptTime
// m_InterceptDistance
// m_InterceptPoint
// m_ProjectedTime
// m_InterceptPK
// m_MaxRange
// m_AssignmentDelay
// m_AlertTimeDelay
// m_IsLOSMasked
// m_AssignmentTime
// m_InterceptInsideMaxRange
// m_InterceptOutsideMinRange
// m_InterceptInsideZone
void assessmentRecord::AssessAIEngagement(const double simTime,
                                          const std::shared_ptr<weaponRecord>& weapon,
                                          const trackRecord& track,
                                          const bool use_delays_for_projection,
                                          const double assignment_delay,
                                          const double projection_time_s,
                                          const double projection_incr_s)
{
   m_Weapon = weapon;
   m_weapon_zone_types = E_NO_ZONES_SET;
   std::shared_ptr<assetRecord> asset = weapon->GetParentAsset();

   auto wm = dynamic_cast<weaponsManagerAI*>(weapon->GetWeaponsManager().get());
   if (!wm)
   {
      HCL_ERROR_LOGGER(mGlobalLogger) << " assessmentRecord::AssessAIEngagement(): Weapon " << weapon->GetParentAssetID().getIDString() << ":" << weapon->GetWeaponID().getIDString() << " has no attached weaponsManagerAI. Unable to assess AI engagement.";
      return;
   }

   // assignment delay
   m_AssignmentDelay = assignment_delay;
   auto real_assignment_delay = m_AssignmentDelay;
   if (!use_delays_for_projection)
   { m_AssignmentDelay = 0; }

   double alertDelay = 0;
   if (NumericUtils::LessEqual(asset->getSpeed(), 1.0))
   {
      alertDelay = wm->getAlertTime();
   }

   // project track forward
   double time_since_collection = simTime - track.getTime();
   HCL_TRACE_LOGGER(mGlobalLogger) << " assessmentRecord::AssessAIEngagement(): Use delays for projection? " << (use_delays_for_projection ? "true" : "false")
                      << ", Assignment delay = " << m_AssignmentDelay << ", Real assignment delay = " << real_assignment_delay
                      << ", Alert delay = " << alertDelay;

   trackRecord projectedThreat = trackRecord::extrapolateTrack(track, alertDelay + m_AssignmentDelay + time_since_collection);

   // Calculate distance from weapon to projected threat.
   double distanceToTarget = vm::GreatCircle<double>::calculateDistanceInMeters(weapon->GetPosition().getLatDegs(),
                                                                                weapon->GetPosition().getLonDegs(),
                                                                                projectedThreat.getPosition().getLatDegs(),
                                                                                projectedThreat.getPosition().getLonDegs());

   double interceptSpeed = wm->getInterceptSpeed();

   double projectionTime = NumericUtils::NearlyEqual(projection_time_s, DBL_MAX) ? (distanceToTarget / interceptSpeed) : projection_time_s;
   double projectionIncr = NumericUtils::NearlyEqual(projection_incr_s, DBL_MAX) ? (distanceToTarget / interceptSpeed / 10) : projection_incr_s;


#if DEBUG_ASSESS
   HCL_TRACE_LOGGER(mGlobalLogger) << "Projected threat position (Lat/Lon): " << projectedThreat.getPosition().getLatDegs() << ", " << projectedThreat.getPosition().getLonDegs()
                      << ", Weapon = " << asset->getName() << ":" << weapon->GetName()
                      << ", Distance to projected threat position = " << distanceToTarget;
#endif

   // Calculate if we can intercept, and the earliest intercept.
   m_CanInterceptTrack = GetEarliestAIInterceptInZone(simTime,
                                                      projectedThreat,
                                                      weapon,
                                                      projectionTime,
                                                      projectionIncr,
                                                      m_InterceptTime,
                                                      m_InterceptDistance,
                                                      m_InterceptPoint,
                                                      m_ProjectedTime,
                                                      m_InterceptPK,
                                                      m_MaxRange);

   // assignment time may diverge from projection time here
   // note: we still incurr the real TTF penalty even if we're not taking into
   // account weapon TTF for track projections
   m_AssignmentTime = simTime + m_ProjectedTime;
   m_ProjectedTime = m_AssignmentTime + alertDelay + real_assignment_delay;
   m_InterceptTime += real_assignment_delay + alertDelay;

   {
      std::ostringstream minMaxRange;
      if (asset->HasMinRangeAttribute())
      {
         minMaxRange << asset->GetMinRange();
      }
      else
      {
         minMaxRange << "unknown";
      }
      minMaxRange << '/';
      if (asset->HasMaxRangeAttribute())
      {
         minMaxRange << asset->GetMaxRange();
      }
      else
      {
         minMaxRange << "unknown";
      }
      HCL_TRACE_LOGGER(mGlobalLogger) << "In Assess(): m_InterceptDistance = " << m_InterceptDistance << ", weapon min/max range: " << minMaxRange.str();
   }

   // don't filter if the weapon doesn't broadcast absolute ranges - set to success if the weapon can intercept in this case
   m_InterceptInsideMaxRange = asset->HasMaxRangeAttribute() ? (m_InterceptDistance < asset->GetMaxRange()) : m_CanInterceptTrack;
   m_InterceptOutsideMinRange = asset->HasMinRangeAttribute() ? (m_InterceptDistance > asset->GetMinRange()) : m_CanInterceptTrack;
   m_InterceptInsideZone = (m_InterceptInsideMaxRange && m_InterceptOutsideMinRange && m_CanInterceptTrack);

   m_ClosestApproach = m_InterceptPoint;
   m_DistanceToPCA = vm::GreatCircle<double>::calculateDistanceInMeters(weapon->GetPosition().getLLA().getLat() * vm::RADIANS_TO_DEGREES,
                                                                        weapon->GetPosition().getLLA().getLon() * vm::RADIANS_TO_DEGREES,
                                                                        m_ClosestApproach.getLLA().getLat() * vm::RADIANS_TO_DEGREES,
                                                                        m_ClosestApproach.getLLA().getLon() * vm::RADIANS_TO_DEGREES);
}

// sets:
// Everything AssessEngagement sets +
// m_Threat
// m_WeaponExcluded
// m_PotentialShots
void assessmentRecord::AssessWeapon(const double& simTime,
                                    const std::shared_ptr<threatRecord>& threat,
                                    const std::shared_ptr< weaponRecord >& weapon,
                                    const weaponTable& weaponTable,
                                    const bool use_delays_for_projection,
                                    const double assignment_delay,
                                    const double expected_execution_delay,
                                    const double projected_time_s,
                                    const double projected_time_incr_s)
{
   m_Threat  = threat;

   auto& track = m_Threat->getTrack();

   // Consider weapon systems preferences here...
   const auto& weapon_row = weaponTable.getWeaponTableRow(track, false, false);

   if (!weaponTable.getPriorities(weapon, track, m_WeaponTypePriority, m_WeaponSubtypePriority))
      HCL_WARN_LOGGER(mGlobalLogger) << "Could not find weapon table preference for target: " << track->getTargetTruthName()
                        << ", type: " << track->getType()
                        << ", by weapon: " << weapon->GetParentAsset()->getName() << ":" << weapon->GetName()
                        << ". Assessment cannot be made.";

#if DEBUG_ASSESS
   HCL_TRACE_LOGGER(mGlobalLogger) << "Threat type: " << track->getType()
                      << ", Priorities: " << m_WeaponTypePriority << ", "
                      << m_WeaponSubtypePriority;
#endif

   // Check if weapon is specifically excluded.
   if ((0 == m_WeaponTypePriority) || (0 == m_WeaponSubtypePriority))
   {
      HCL_INFO_LOGGER(mGlobalLogger) << "Weapon " << weapon->GetName() << " is excluded by weapon table for target type: " << track->getType();
      m_WeaponExcluded = true;
      return;
   }

   // Since this is coming from the BM, don't invoke weapon delay. Only the WM's utilize this. Otherwise, we get
   // into longest-delay projection wins counter-intuitive behavior. Non-BM users should call AssessEngagment directly.
   if (weapon->GetType().getType() == il::WEAPON_SAM)
   {
      AssessSAMEngagement(simTime, weapon, *track, use_delays_for_projection, assignment_delay, expected_execution_delay, projected_time_s, projected_time_incr_s);
   }
   else if (weapon->GetType().getType() == il::WEAPON_AI)
   {
      AssessAIEngagement(simTime, weapon, *track, use_delays_for_projection, assignment_delay, projected_time_s, projected_time_incr_s);
   }
   else
   { HCL_ERROR_LOGGER(mGlobalLogger) << "assessmentRecord::AssessWeapon(): Weapon " << weapon->GetParentAssetID() << " " << weapon->GetWeaponID() << " not of type WEAPON_SAM or WEAPON_AI. No suitable assessment found for this weapon type."; }

   // GTIQBAssessEngagementsGetEarliestInterceptInZone - slightly modified with new min pk addition
   if (!m_CanInterceptTrack)
   {
      HCL_TRACE_LOGGER(mGlobalLogger) << "Intercept calculator could not find an intercept solution";
      m_WeaponExcluded = true;
      return;
   }
   else if (weapon_row && !weapon_row->checkMinPK(m_InterceptPK))
   {
      HCL_TRACE_LOGGER(mGlobalLogger) << "Min PK did not pass";
      m_WeaponExcluded = true;
      return;
   }
   else
   {
      m_WeaponExcluded = false;
      m_PotentialShots = 1;
   }

   HCL_TRACE_LOGGER(mGlobalLogger) << "Calculated intercept in zone." << std::endl;

   // Only perform potential shot analysis for SAMs
   if (weapon->GetType().getType() == il::WEAPON_AI)
   { return; }

   // Is threat already inside defended asset zone?

   // Is intercept point inside defended asset zone?

   // Calculate number of shots this weapons system could potentially make.
   /* Do shot count analysis. See how many shots can be fired, with delay
      between fires and still intercept inside zone. */
   double lastInterceptTime = m_InterceptTime;
   double interceptDistance;
   double projectedTime;
   positionRecord interceptPoint;
   int iteration = 1;
   auto projectedThreat = *track;
   HCL_TRACE_LOGGER(mGlobalLogger) << "**** Performing Potential Shot Analysis... ****";
   while (m_PotentialShots < 10)
   {
#if DEBUG_ASSESS
      HCL_TRACE_LOGGER(mGlobalLogger) << iteration++ << ": Last intercept time: " << lastInterceptTime;
#endif
      /* Move the threat forward to the intercept position plus time to launch + TimeBetweenShots. */

      projectedThreat = trackRecord::extrapolateTrack(projectedThreat, lastInterceptTime + weapon->GetTimeToFire() + weapon->GetTimeBetweenShots());

      // Calculate if we can intercept, and the earliest intercept. Instruct the routine not to do projections to current time since we're projecting to future times
      double tmp_est_pk = 0, tmp_max_range = 0; // this loop is just for potential shot analysis, don't need to record this.
      if (GetEarliestSAMInterceptInZone(simTime, projectedThreat, weapon, projected_time_s, projected_time_incr_s, lastInterceptTime, interceptDistance, interceptPoint, projectedTime, tmp_est_pk, tmp_max_range, true))
      {
         ++m_PotentialShots;
      }
      // Cannot intercept inside zone.
      else
      {
         break;
      }
   }

   HCL_TRACE_LOGGER(mGlobalLogger) << "**** Ending Potential Shot Analysis... ****";
}

// dev/custom/src/model/equip/subsy/gtiqbae/gtiqbaeutil.c
/**
   Removed code that returned false if target was moving away.  This
   kind of logic doesn't belong in a low-level function.  Need to discuss
   this with Mike.
   - Ken
*/
// sets:
// m_weapon_zone_types
bool assessmentRecord::GetEarliestSAMInterceptInZone(const double& simTime,
                                                     const trackRecord& track,
                                                     const std::shared_ptr<weaponRecord>& weapon,
                                                     const double projected_time_s,
                                                     const double projected_time_incr_s,
                                                     double& interceptTime,
                                                     double& interceptDistance,
                                                     positionRecord& interceptPoint,
                                                     double& projectedTime,
                                                     double& estPK,
                                                     double& max_range,
                                                     bool   is_already_projected)
{
   /* Find the earliest intercept point, if any, within the weapon's
      engagement zone.

      Outputs:
      InterceptPos  -  Position of intercept within zone
      InterceptTime -  Time of intercept within zone

      Returns:
      FSUCCESS - Intercept Position found
      FFAILURE - Intercept Position not found

      History:
      02 Sep 06 - Created. MDMcAllister
   */
   interceptTime = interceptDistance = projectedTime = 0, estPK = 0;

   // Project Threat forward in time since last collection if it hasn't been already
   double timeSinceCollection = simTime - track.getTime();
   trackRecord predictedTrack = is_already_projected ? track : trackRecord::extrapolateTrack(track, timeSinceCollection);

   // Threat not projected into zone?
   const auto& weapon_zones = weapon->GetAttachedZones();
   // TODO NH - We can handle zones here, but better condition handling is available in higher level functions that call this function. Old behavior had IsTrackProjectedInsideZone automatically returning true in the case of true_if_no_zones being true (this case).
   //if (!IsTrackProjectedInsideZone (weapon_zones,
   //                                 track.getPosition(), predictedTrack.getPosition(),
   //                                 m_weapon_zone_types, GetDefaultWeaponZoneType (weapon),
   //                                 true))
   //{
   //   return false;
   //}

   projectedTime = 0; // time since collection just brings the track up to time now, it doesn't count against projection
   // Loop to find intercept time, within the specified projection time secs.
   while (NumericUtils::LessEqual(projectedTime, projected_time_s))
   {
      // Can intercept threat kinematically?
      if (GetSAMIntercept(predictedTrack, weapon, interceptTime, interceptDistance, interceptPoint, estPK, max_range))
      {
         // TODO NH - Bubble up when an intercept point is found, but is not inside weapon zones.
         //  We are once again handling zones here, but better condition handling (pertaining to output statements) is available in higher level functions that call this function.
         //  Old behavior had IsInsideZone automatically returning true in the case of true_if_no_zones being true (this case).

         // InterceptPos is inside zone or there is no weapon zone.
         if (IsInsideZone(weapon_zones, interceptPoint, m_weapon_zone_types, GetDefaultWeaponZoneType(weapon), true))
         {
            interceptTime += projectedTime;
            return true;
         }
      }

      // Project Threat forward in time so we can try again.
      predictedTrack = trackRecord::extrapolateTrack(predictedTrack, projected_time_incr_s);  // look forward one projection step so the weapon gets a chance to engage

      // Increment projectedTime.
      projectedTime += projected_time_incr_s;
   }

#if DEBUG_ASSESS
   HCL_TRACE_LOGGER(mGlobalLogger) << "************* Unable to intercept in zone. *************";
#endif
   return false;
}

// m_weapon_zone_types
bool assessmentRecord::GetEarliestAIInterceptInZone(const double& simTime,
                                                    const trackRecord& track,
                                                    const std::shared_ptr<weaponRecord>& weapon,
                                                    const double projected_time_s,
                                                    const double projected_time_incr_s,
                                                    double& interceptTime,
                                                    double& interceptDistance,
                                                    positionRecord& interceptPoint,
                                                    double& projectedTime,
                                                    double& estPK,
                                                    double& max_range)
{
   /* Find the earliest intercept point, if any, within the weapon's
   engagement zone.

   Outputs:
   InterceptPos  -  Position of intercept within zone
   InterceptTime -  Time of intercept within zone

   Returns:
   true - Intercept Position found
   false - Intercept Position not found
   */
   interceptTime = interceptDistance = projectedTime = estPK = 0;
   auto predictedTrack = track;

   // Threat not projected into zone?
   const auto& weapon_zones = weapon->GetParentAsset()->GetZones();
   // TODO NH - This can be replaced by a functional check (predictedTrack->getPosition() simply returns the same thing as track.getPosition()), in order to occasionally shortcut some unnecessary processing.
   /*if (!IsTrackProjectedInsideZone(weapon_zones,
      track.getPosition(), predictedTrack->getPosition(),
      m_weapon_zone_types, GetDefaultWeaponZoneType(weapon),
      true))
   {
      return false;
   }*/

   bool ignoreZones = false;
   auto wm = weapon->GetWeaponsManager();
   if (wm)
   {
      auto wmai = dynamic_cast<weaponsManagerAI*>(wm.get());
      if (wmai)
      {
         ignoreZones = wmai->getAssessEngageVia() == MUNITION_INTERCEPT_PT_IGNORE_ZONE;
      }
   }

   // NH - This method of calculating an intercept can cause some strange behavior. The intercept, if found, will be engaged immediately. However, by projecting the threat forward in time,
   //    this method can find an intercept that will result in an intercept point occuring inside a FEZ if pursued after the projection time has passed (if the AI remains stationary during this time),
   //    yet will result in an intercept point occuring outside a FEZ when engaged immediately. As I understand it, this replicates HELIOS behavior. Although an edge case, this could be prevented by
   //    checking the calculated intercept point (or pursue point) flown towards by the AI.

   projectedTime = 0; // time since collection just brings the track up to time now, it doesn't count against projection
   // Loop to find intercept time, within the specified projection time secs.
   while (NumericUtils::LessEqual(projectedTime, projected_time_s))
   {
      // Can intercept threat kinematically?
      if (GetAIIntercept(simTime + projectedTime, predictedTrack, weapon, interceptTime, interceptDistance, interceptPoint, estPK, max_range))
      {
         // InterceptPos is inside FEZ or there is no weapon zone.
         if (IsInsideZone(ignoreZones ? std::vector<std::shared_ptr<zoneRecordInterface>>() : weapon_zones, interceptPoint, m_weapon_zone_types, GetDefaultWeaponZoneType(weapon), true) &&
             (GetDefaultWeaponZoneType(weapon) & m_weapon_zone_types))
         {
            interceptTime += projectedTime;
            return true;
         }
      }

      // Project Threat forward in time so we can try again.
      predictedTrack = trackRecord::extrapolateTrack(predictedTrack, projected_time_incr_s); // look forward one projection step so the weapon gets a chance to engage

      // Increment projectedTime.
      projectedTime += projected_time_incr_s;
   }

#if DEBUG_ASSESS
   HCL_TRACE_LOGGER(mGlobalLogger) << "************* Unable to intercept in zone. *************";
#endif
   return false;
}

bool assessmentRecord::GetSAMIntercept(const trackRecord& track,
                                       const std::shared_ptr<weaponRecord>& weapon,
                                       double& interceptTime,
                                       double& interceptDistance,
                                       positionRecord& interceptPoint,
                                       double& est_pk,
                                       double& max_range)
{
   /* Given Threat and Weapon, determine if an intercept is kinematically
      possible.

      Outputs:
      InterceptPos  -  Position of intercept
      InterceptTime -  Time of intercept

      Returns:
      FSUCCESS - Intercept possible
      FFAILURE - Intercept impossible

      History:
      02 Sep 06 - Created. MDMcAllister
   */

#if 0
   HCL_TRACE_LOGGER(mGlobalLogger) << "Target Position: "
                      << threat.getEvaluatedTrack()->getPredictedPositionXYZ()[0]
                      << ", "
                      << threat.getEvaluatedTrack()->getPredictedPositionXYZ()[1]
                      << ", "
                      << threat.getEvaluatedTrack()->getPredictedPositionXYZ()[2]
                      << std::endl
                      << "Velocity: "
                      << threat.getEvaluatedTrack()->getVelocityVector()[0]
                      << ", "
                      << threat.getEvaluatedTrack()->getVelocityVector()[1]
                      << ", "
                      << threat.getEvaluatedTrack()->getVelocityVector()[2]
                      << std::endl
                      << "Weapon Position: "
                      << weapon.getPositionXYZ()[0]
                      << ", "
                      << weapon.getPositionXYZ()[1]
                      << ", "
                      << weapon.getPositionXYZ()[2]
                      << std::endl;
         << "Average weapon speed: " << weapon.getInterceptSpeed()
         << std::endl;
#endif

   double interceptXYZ[3] = { 0 };
   max_range = 0;
   if (m_xcept_iface->CanIntercept(track, weapon.get(), interceptTime, interceptXYZ, est_pk, max_range, mGlobalLogger))
   {
      interceptPoint.setXYZ(interceptXYZ[0], interceptXYZ[1], interceptXYZ[2]);
      vm::Vector3<double> deltaDist = weapon->GetPosition().getXYZ() - interceptPoint.getXYZ();
      interceptDistance = deltaDist.norm();

#if DEBUG_ASSESS
      HCL_TRACE_LOGGER(mGlobalLogger) << "***** Intercept distance: " << interceptDistance << std::endl
                         << "***** Intercept time: " << interceptTime << std::endl;
#endif
      if ((weapon->HasMaxRangeAttribute() && (weapon->GetMaxRange() < interceptDistance)) ||
          (weapon->HasMinRangeAttribute() && (weapon->GetMinRange() > interceptDistance)))
      {
         return false;
      }
      else
      {
         return true;
      }
   }
   else
   {
#if DEBUG_ASSESS
      HCL_TRACE_LOGGER(mGlobalLogger) << "predictIntercept returned false." << std::endl;
#endif
   }

   return false;
}

bool assessmentRecord::GetAIIntercept(const double simTime,
                                      const trackRecord& track,
                                      const std::shared_ptr<weaponRecord>& weapon,
                                      double& interceptTime,
                                      double& interceptDistance,
                                      positionRecord& interceptPoint,
                                      double& est_pk,
                                      double& max_range)
{
   // Given Threat and Weapon, determine if an intercept is kinematically possible.

   // Outputs:
   // InterceptPoint - Position of intercept
   // InterceptTime - Time of intercept

   // Returns:
   // true - Intercept possible
   // false - Intercept impossible

   auto wm = dynamic_cast<weaponsManagerAI*>(weapon->GetWeaponsManager().get());

   if (wm->getAssessEngageVia() == il::eAssessEngage::COLLISION_INTERCEPT_PT_INSIDE_ZONE)
   {
      max_range = DBL_MAX;

      if (!CalculateInterceptTime(simTime, weapon->GetParentAsset()->getPosition(), track, wm->getInterceptSpeed(), interceptTime))
      {
         return false;
      }
      interceptTime -= simTime; // Adjust for simTime used in CalculateInterceptTime
   }
   else
   {
      // NH - This covers the MUNITION_INTERCEPT cases. This behavior needs to be verified.
      double range = weapon->GetParentAsset()->getPosition().calculateDistanceMeters(track.getPosition());
      max_range = weapon->GetMaxRange();

      // Outside munition range?
      if (NumericUtils::GreaterThan(range, max_range))
      {
         return false;
      }

      // An average speed is what this implementation currently uses;
      double avgSpeed = weapon->GetAvgSpeed();
      // however, HELIOS uses the following rule of thumb to estimate average speed over munition flight
      // double percentMaxRange = range / max_range;
      // double avgSpeed = maxSpeed * (0.35 + (percentMaxRange * 0.5));

      interceptTime = range / avgSpeed;
   }

   interceptPoint = track.getPropagatedPosition(simTime + interceptTime);
   interceptDistance = weapon->GetParentAsset()->getPosition().calculateDistanceMeters(interceptPoint);
   est_pk = weapon->GetEstPK();

   return true;
}

void assessmentRecord::log() const
{
   HCL_INFO_LOGGER(mGlobalLogger) << " ********** "
                     << "Weapon: '"
                     << m_Weapon->GetParentAssetID() << "/" << m_Weapon->GetWeaponID()
                     << "' can intercept threat: '"
                     << m_Threat->getTrack()->getID()
                     << "' in "
                     << m_InterceptTime
                     << " seconds."
                     << " ********** "
                     << std::endl;
}

void assessmentRecord::setWeaponTypePriority(const int priority)
{
   m_WeaponTypePriority = priority;
}

int assessmentRecord::getWeaponTypePriority() const
{
   return m_WeaponTypePriority;
}


void assessmentRecord::setWeaponSubtypePriority(const int priority)
{
   m_WeaponSubtypePriority = priority;
}

int assessmentRecord::getWeaponSubtypePriority() const
{
   return m_WeaponSubtypePriority;
}


void assessmentRecord::setWeaponExcluded(const bool Excluded)
{
   m_WeaponExcluded = Excluded;
}

bool assessmentRecord::isWeaponExcluded() const
{
   return m_WeaponExcluded;
}

void assessmentRecord::setPriority(const tZonePriority& Priority)
{
   m_Priority = Priority;
}

tZonePriority assessmentRecord::getPriority() const
{
   return m_Priority;
}

const std::shared_ptr<assignmentMessage>& assessmentRecord::getExistingAssignment() const
{
   return m_Threat->getEvaluationTrack().DelegateAssignment();
}

const std::shared_ptr<threatRecord>& assessmentRecord::getAssessedThreatRecord() const
{
   return m_Threat;
}

idRecord assessmentRecord::GetAssetID() const
{
   return m_Weapon->GetParentAssetID();
}

tWeaponID assessmentRecord::GetWeaponID() const
{
   return m_Weapon->GetWeaponID();
}


il::eZoneType assessmentRecord::GetDefaultWeaponZoneType(const std::shared_ptr<weaponRecord>& weapon)
{
   il::eZoneType weapon_zone_type = E_ZONE_TYPE_OTHER;

   switch (weapon->GetType().getType())
   {
      case il::WEAPON_SAM:
         weapon_zone_type = E_ZONE_TYPE_MEZ;
         break;
      case il::WEAPON_AI:
         weapon_zone_type = E_ZONE_TYPE_FEZ;
         break;
      case il::WEAPON_OTHER:
         weapon_zone_type = E_ZONE_TYPE_OTHER;
         break;
   }

   return weapon_zone_type;
}

} // namespace il
