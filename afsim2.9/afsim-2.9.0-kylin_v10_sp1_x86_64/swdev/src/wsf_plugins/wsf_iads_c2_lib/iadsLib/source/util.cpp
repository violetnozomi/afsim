// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  @file util.cpp
 *  @author Kenneth R. Sewell III \n
 *          Kenneth.Sewell@wpafb.af.mil \n
 *          AFRL/RYWF \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004
 *  @brief Utility functions.
 *
 *  Classification: UNCLASSIFIED
 */

#include <sstream>
#include <cassert>

#include <iadsLib/util.hpp>
#include <logger.hpp>
#include <vclMath/GreatCircle.hpp>
#include <vclMath/GeodeticToECEF.hpp>
#include <vclMath/ECEFToGeodetic.hpp>
#include <iadsLib/assignmentACKMessage.hpp>

namespace il
{

IADSLIB_EXPORT double angleOffset(const positionRecord& TrackPosition, const double& Heading_rads, const positionRecord& AssetPosition)
{
   double bearing = vm::GreatCircle<double>::
                    calculateBearingInRadians(TrackPosition.getLLA().getLat(),
                                              TrackPosition.getLLA().getLon(),
                                              AssetPosition.getLLA().getLat(),
                                              AssetPosition.getLLA().getLon()
                                             );
   bearing -= Heading_rads;
   if (bearing > vm::PI)
   {
      bearing -= vm::TWOPI;
   }
   else if (bearing < -vm::PI)
   {
      bearing += vm::TWOPI;
   }

   return (bearing);
}

/**
   @brief Find point at which track is closest to an asset.
   @param track Moving track.
   @param asset Asset.
   @param closestLatitude Latitude in degrees of point of closest approach.
   @param closestLongitude Longitude in degrees of point of closest approach.
*/
positionRecord findPointOfClosestApproach(const positionRecord& CurrentPos,
                                          const positionRecord& PredictedPos,
                                          const double&         HeadingOffsetRads,
                                          const double&         Velocity,
                                          const positionRecord& AssetPos)
{
   // Is track inbound and moving.
   if ((HeadingOffsetRads < vm::PI_2) && (Velocity > 0.0))
   {
      // Calculate great circle track is traveling on.

      // Convert track's current position to point on unit sphere.
      vm::Vector3<double> trackXYZ;
      vm::geodeticToECEF<double>(CurrentPos.getLLA().getLat(),
                                 CurrentPos.getLLA().getLon(),
                                 0.0,
                                 trackXYZ.getX(),
                                 trackXYZ.getY(),
                                 trackXYZ.getZ()
                                );
      trackXYZ.normalize();

      // Convert track's predicted position to point on unit sphere.
      vm::Vector3<double> trackPredictedXYZ;
      vm::geodeticToECEF<double>(PredictedPos.getLLA().getLat(),
                                 PredictedPos.getLLA().getLon(),
                                 0.0,
                                 trackPredictedXYZ.getX(),
                                 trackPredictedXYZ.getY(),
                                 trackPredictedXYZ.getZ()
                                );
      trackPredictedXYZ.normalize();

      // Convert asset's position to point on unit sphere.
      vm::Vector3<double> assetXYZ;
      vm::geodeticToECEF<double>(AssetPos.getLLA().getLat(),
                                 AssetPos.getLLA().getLon(),
                                 0.0,
                                 assetXYZ.getX(),
                                 assetXYZ.getY(),
                                 assetXYZ.getZ()
                                );
      assetXYZ.normalize();

      // Find the closes point to asset that is on the great circle.
      vm::Vector3<double> closestXYZ =
         vm::GreatCircle<double>::
         projectPointToGreatCircleOnUnitSphere(assetXYZ, trackXYZ,
                                               trackPredictedXYZ);

      // Scale that point from unit sphere up to approx size of earth.
      closestXYZ *= vm::WGS84_SEMI_MAJOR;

      // Convert to Lat/Lon/Alt
      vm::Vector3<double> closestLLA;
      vm::ecefToGeodetic<double>(closestXYZ.getX(),
                                 closestXYZ.getY(),
                                 closestXYZ.getZ(),
                                 closestLLA.getLat(),
                                 closestLLA.getLon(),
                                 closestLLA.getAlt()
                                );
      closestLLA.setAlt(CurrentPos.getLLA().getAlt());

      // Find closest point on great circle to asset.
      positionRecord closestPoint;
      closestPoint.setLLA(closestLLA);
      return closestPoint;
   }
   else
   {
      // Track's current position is the closest point.
      return CurrentPos;
   }
}

IADSLIB_EXPORT std::string AssetTypeEnumToString(const AssetTypeEnum asset_type)
{
   std::string type_str;

   switch (asset_type)
   {
      case WEAPON_OTHER:
         type_str = "Other";
         break;
      case WEAPON_AI:
         type_str = "AI";
         break;
      case WEAPON_SAM:
         type_str = "SAM";
         break;
      case C2_OTHER:
         type_str = "C2 Other";
         break;
      default:
         type_str = "*Error*";
         break;
   }

   return type_str;
}

IADSLIB_EXPORT bool IsAssessmentValid(const il::assessmentRecord& assessment,
                                      const il::assetMap& assets,
                                      const std::shared_ptr<il::assetRecord>& MyUnit,
                                      std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
{
   bool assessment_valid = false;

   if (!assessment.isWeaponExcluded() &&
       assessment.canInterceptTrack() &&
       assessment.isInterceptInsideMaxRange() &&
       assessment.isInterceptOutsideMinRange() &&
       assessment.getInterceptPK() > 0.0 &&
       assessment.isInterceptInsideZone())
   {
      auto weapon = assessment.getWeapon();

      if (weapon &&
          weapon->GetParentAsset()->getNumOpenAssignments() > 0 &&
          weapon->GetEstimatedMunitionsPrepped() > 0 &&
          weapon->GetEstimatedAvailableFireChannels() > 0)
      {
         // Ensure all units in chain of command to the weapon have Ready Assignments.
         std::string offender_units;
         if (!assetRecord::doesAllChainOfCommandHaveReadyAssignments(assets, MyUnit->getID(), weapon->GetParentAssetID(), offender_units))
         {
            HCL_DEBUG_LOGGER(aGlobalLogger) << "Assessment was invalid for track: " << assessment.getTrack()->getID()
                               << " when trying to assign to weapon: " << weapon->GetParentAsset()->getName() << ":" << weapon->GetName()
                               << ". Not all chain of command has ready assignments (" << offender_units << ")";
         }
         else
         {
            assessment_valid = true;
         }
      }
      else
      {
         // report results back since the assessment was valid but the weapon won't allow it
         if (!weapon)
         {
            HCL_ERROR_LOGGER(aGlobalLogger) << "Assessment was invalid for track: " << assessment.getTrack()->getID() << ". Invalid weapon.";
         }
         else
         {
            if (weapon->GetParentAsset()->getNumOpenAssignments() <= 0)
            {
               HCL_DEBUG_LOGGER(aGlobalLogger) << "Assessment was invalid for track: " << assessment.getTrack()->getID() << ". No open assignments on asset: " << weapon->GetParentAsset()->getName();
            }
            else if (weapon->GetEstimatedMunitionsPrepped() <= 0)
            {
               HCL_DEBUG_LOGGER(aGlobalLogger) << "Assessment was invalid for track: " << assessment.getTrack()->getID() << ". No munitions on weapon: " << weapon->GetParentAsset()->getName() << "/" << weapon->GetName();
            }
            else if (weapon->GetEstimatedAvailableFireChannels() <= 0)
            {
               HCL_DEBUG_LOGGER(aGlobalLogger) << "Assessment was invalid for track: " << assessment.getTrack()->getID() << ". No open fire channels on weapon: " << weapon->GetParentAsset()->getName() << "/" << weapon->GetName();
            }
         }
      }
   }
   return assessment_valid;
}

IADSLIB_EXPORT bool IsUnitCapableOfAssignment(const il::exclusionArray& exclusion_array,
                                              const idRecord& weapon,
                                              const idRecord& track_id)
{
   return !exclusion_array.isAssetExcluded(track_id, weapon);
}

IADSLIB_EXPORT std::string BoolToString(const bool Value)
{
   return Value ? "true" : "false";
}

bool IsTrackProjectedInsideZone(const std::vector<std::shared_ptr<zoneRecordInterface>>& zones,
                                const positionRecord& pos1,
                                const positionRecord& pos2,
                                bool true_if_no_zones)
{
   // if no zones, return based on the desired functionality
   if (!zones.size())
   {
      return true_if_no_zones;
   }

   for (auto it = zones.cbegin(); it != zones.cend(); ++it)
   {
      const auto& zone = *it;
      if (zone->isProjectedInsideZone(pos1, pos2))
      {
         return true;
      }
   }

   // if we made it this far, we didn't find a projection into the zone
   return false;
}

// alternative version of above that returns all zone types the track is projected into - it does not clear the existing flags
bool IsTrackProjectedInsideZone(const std::vector<std::shared_ptr<zoneRecordInterface>>& zones,
                                const positionRecord& pos1,
                                const positionRecord& pos2,
                                tZoneTypeMask& zone_types,
                                const eZoneType zone_type_if_no_zones,
                                bool true_if_no_zones)
{
   // if no zones, return based on the desired functionality
   if (!zones.size())
   {
      zone_types |= zone_type_if_no_zones;
      return true_if_no_zones;
   }

   bool is_projected = false;

   for (auto it = zones.cbegin(); it != zones.cend(); ++it)
   {
      const auto& zone = *it;
      if (zone->isProjectedInsideZone(pos1, pos2))
      {
         zone_types |= zone->getTypeEnum();
         is_projected = true;
      }
   }

   return is_projected;
}

bool IsTrackProjectedInsideZone(const std::vector<std::shared_ptr<zoneRecordInterface>>& zones,
                                const positionRecord& pos,
                                const double heading_rads,
                                bool true_if_no_zones)
{
   // if no zones, return based on the desired functionality
   if (!zones.size())
   {
      return true_if_no_zones;
   }

   for (auto it = zones.cbegin(); it != zones.cend(); ++it)
   {
      const auto& zone = *it;
      if (zone->isProjectedInsideZone(pos, heading_rads))
      {
         return true;
      }
   }

   // if we made it this far, we didn't find a projection into the zone
   return false;
}

bool IsTrackProjectedInsideZone(const std::vector<std::shared_ptr<zoneRecordInterface>>& zones,
                                const positionRecord& pos,
                                const double heading_rads,
                                tZoneTypeMask& zone_types,
                                const eZoneType zone_type_if_no_zones,
                                bool true_if_no_zones)
{
   // if no zones, return based on the desired functionality
   if (!zones.size() && true_if_no_zones)
   {
      zone_types |= zone_type_if_no_zones;
      return true;
   }

   bool is_projected = false;

   for (auto it = zones.cbegin(); it != zones.cend(); ++it)
   {
      const auto& zone = *it;
      if (zone->isProjectedInsideZone(pos, heading_rads))
      {
         zone_types |= zone->getTypeEnum();
         is_projected = true;
      }
   }

   return is_projected;
}

bool IsTrackProjectedInsideIntermediateC2Zones(const assetMap& assets,
                                               const idRecord& cmdr_begin,
                                               const idRecord& sub_end,
                                               const positionRecord& position,
                                               const double heading_rads,
                                               bool true_if_no_zones)
{
   bool any_projections = false;

   auto curr_asset_ptr = assets.GetAsset(cmdr_begin);
   while (curr_asset_ptr && !any_projections)
   {
      if (curr_asset_ptr->hasZones() && (IsInsideZone(curr_asset_ptr->GetZones(), position, true_if_no_zones) || IsTrackProjectedInsideZone(curr_asset_ptr->GetZones(), position, heading_rads, true_if_no_zones)))
      {
         any_projections = true;
      }

      auto next_id = curr_asset_ptr->findNextSubordinateInChain(sub_end, assets);
      curr_asset_ptr = assets.GetAsset(next_id);
   }

   return any_projections;
}

bool IsTrackProjectedInsideIntermediateC2Zones(const assetMap& assets,
                                               const idRecord& cmdr_begin,
                                               const idRecord& sub_end,
                                               const positionRecord& position,
                                               const double heading_rads, tZoneTypeMask& zone_types,
                                               const eZoneType zone_type_if_no_zones,
                                               bool true_if_no_zones)
{
   bool any_projections = false;

   auto curr_asset_ptr = assets.GetAsset(cmdr_begin);
   while (curr_asset_ptr && !any_projections)
   {
      if (curr_asset_ptr->hasZones() && (IsInsideZone(curr_asset_ptr->GetZones(), position, zone_types, zone_type_if_no_zones, true_if_no_zones) || IsTrackProjectedInsideZone(curr_asset_ptr->GetZones(), position, heading_rads, zone_types, zone_type_if_no_zones, true_if_no_zones)))
      {
         any_projections = true;
      }

      auto next_id = curr_asset_ptr->findNextSubordinateInChain(sub_end, assets);
      curr_asset_ptr = assets.GetAsset(next_id);
   }

   return any_projections;
}

IADSLIB_EXPORT bool IsInsideZone(const std::vector<std::shared_ptr<zoneRecordInterface>>& zones,
                                 const positionRecord& pos,
                                 bool true_if_no_zones)
{
   // if no zones, return based on the desired functionality
   if (!zones.size())
   {
      return true_if_no_zones;
   }

   for (auto it = zones.cbegin(); it != zones.cend(); ++it)
   {
      const auto& zone = *it;
      if (zone->isInsideZone(pos))
      {
         return true;
      }
   }

   // if we made it this far, we didn't find a projection into the zone
   return false;
}

// alternative version of above that returns all zone types the track is projected into - it does not clear the existing flags
IADSLIB_EXPORT bool IsInsideZone(const std::vector<std::shared_ptr<zoneRecordInterface>>& zones,
                                 const positionRecord& pos,
                                 tZoneTypeMask& zone_types,
                                 const eZoneType zone_type_if_no_zones,
                                 bool true_if_no_zones)
{
   // if no zones, return based on the desired functionality
   if (!zones.size() && true_if_no_zones)
   {
      zone_types |= zone_type_if_no_zones;
      return true;
   }

   bool is_inside = false;

   for (auto it = zones.cbegin(); it != zones.cend(); ++it)
   {
      const auto& zone = *it;
      if (zone->isInsideZone(pos))
      {
         zone_types |= zone->getTypeEnum();
         is_inside = true;
      }
   }

   return is_inside;
}

bool GetTrackTruthNameID(const idRecord& track_id,
                         const trackMap& tracks,
                         std::string& name,
                         idRecord& truthid)
{
   auto trackref = FindTrack(track_id, tracks);
   if (trackref)
   {
      name = trackref->getTargetTruthName();
      truthid = trackref->getTargetTruthID();
      return true;
   }
   name = "Unknown";
   return false;
}

IADSLIB_EXPORT std::string IFFEnumToString(const eIFFType IFF)
{
   std::string iff_str;

   switch (IFF)
   {
      default:
      case E_IFF_UNKNOWN:
         iff_str = "unknown";
         break;

      case E_IFF_NEUTRAL:
         iff_str = "neutral";
         break;

      case E_IFF_FRIENDLY:
         iff_str = "friendly";
         break;

      case E_IFF_HOSTILE:
         iff_str = "hostile";
         break;
   }

   return iff_str;
}

IADSLIB_EXPORT bool IsThreatAllocatedToZoneType(const std::shared_ptr<threatRecord>& threat,
                                                const eZoneType type)
{
   return (threat->getAllocatedZoneTypes() & type) ? true : false;
}

IADSLIB_EXPORT std::shared_ptr<assignmentMessage> GetDelegatedAssignment(const tRankedThreats::value_type& threat)
{
   // if the threat has an original assignment, it was a delegate assignment
   return threat->getEvaluationTrack().DelegateAssignment();
}

IADSLIB_EXPORT std::string ShotDoctrineEnumToString(const eShotDoctrine shot_doctrine)
{
   std::string shot_doctrine_str;

   switch (shot_doctrine)
   {
      case SHOOT_1:
         shot_doctrine_str = "Shoot-1";
         break;
      case SHOOT_2:
         shot_doctrine_str = "Shoot-2";
         break;
      case SHOOT_LOOK_SHOOT:
         shot_doctrine_str = "Shoot-Look-Shoot";
         break;
      default:
         std::stringstream strmerror;
         strmerror << "il::ShotDoctrineEnumToString(): Unknown shot doctrine enum: " << shot_doctrine;
         throw std::runtime_error(strmerror.str());
         break;
   }

   return shot_doctrine_str;
}

IADSLIB_EXPORT bool ShotDoctrineStringToEnum(const std::string& input_str,
                                             eShotDoctrine& doctrine,
                                             std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
{
   bool processed = true;
   if (input_str == "Shoot-1")
   {
      doctrine = il::SHOOT_1;
   }
   else if (input_str == "Shoot-2")
   {
      doctrine = il::SHOOT_2;
   }
   else if (input_str == "Shoot-Look-Shoot")
   {
      doctrine = il::SHOOT_LOOK_SHOOT;
   }
   else
   {
      HCL_FATAL_LOGGER(aGlobalLogger) << "Unrecognized shot doctrine: " << input_str;
      processed = false;
   }

   return processed;
}

IADSLIB_EXPORT std::string TakeActionEnumToString(const eTakeAction take_action)
{
   std::string take_action_str;

   switch (take_action)
   {
      case STRAIGHT:
         take_action_str = "Straight";
         break;
      case BREAK_LEFT:
         take_action_str = "Break-Left";
         break;
      case BREAK_RIGHT:
         take_action_str = "Break-Right";
         break;
      default:
         std::stringstream strmerror;
         strmerror << "il::TakeActionEnumToString(): Unknown take action enum: " << take_action;
         throw std::runtime_error(strmerror.str());
         break;
   }

   return take_action_str;
}

IADSLIB_EXPORT bool TakeActionStringToEnum(const std::string& input_str,
                                           eTakeAction& action,
                                           std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
{
   bool processed = true;
   if (input_str == "Straight")
   {
      action = il::STRAIGHT;
   }
   else if (input_str == "Break-Left")
   {
      action = il::BREAK_LEFT;
   }
   else if (input_str == "Break-Right")
   {
      action = il::BREAK_RIGHT;
   }
   else
   {
      HCL_FATAL_LOGGER(aGlobalLogger) << "Unrecognized take action: " << input_str;
      processed = false;
   }

   return processed;
}

IADSLIB_EXPORT std::string AssessEngageEnumToString(const eAssessEngage assess)
{
   std::string assess_str;

   switch (assess)
   {
      case COLLISION_INTERCEPT_PT_INSIDE_ZONE:
         assess_str = "Collision-Intercept-PT-Inside-Zone";
         break;
      case MUNITION_INTERCEPT_PT_INSIDE_ZONE:
         assess_str = "Munition-Intercept-PT-Inside-Zone";
         break;
      case MUNITION_INTERCEPT_PT_IGNORE_ZONE:
         assess_str = "Munition-Intercept-PT-Ignore-Zone";
         break;
      default:
         std::stringstream strmerror;
         strmerror << "il::AssessEngageEnumToString(): Unknown assess engage enum: " << assess;
         throw std::runtime_error(strmerror.str());
         break;
   }

   return assess_str;
}

IADSLIB_EXPORT bool AssessEngageStringToEnum(const std::string& input_str,
                                             eAssessEngage& assess,
                                             std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
{
   bool processed = true;
   if (input_str == "Collision-Intercept-PT-Inside-Zone")
   {
      assess = il::COLLISION_INTERCEPT_PT_INSIDE_ZONE;
   }
   else if (input_str == "Munition-Intercept-PT-Inside-Zone")
   {
      assess = il::MUNITION_INTERCEPT_PT_INSIDE_ZONE;
   }
   else if (input_str == "Munition-Intercept-PT-Ignore-Zone")
   {
      assess = il::MUNITION_INTERCEPT_PT_IGNORE_ZONE;
   }
   else
   {
      HCL_FATAL_LOGGER(aGlobalLogger) << "Unrecognized engagement assessment: " << input_str;
      processed = false;
   }

   return processed;
}

IADSLIB_EXPORT std::string RWRPriorityEnumToString(const eRWRResponsePriority priority)
{
   std::string priority_str;

   switch (priority)
   {
      case DEFAULT:
         priority_str = "Default";
         break;
      case CHOOSE_CLOSEST_THREAT:
         priority_str = "Choose-Closest-Threat";
         break;
      default:
         std::stringstream strmerror;
         strmerror << "il::RWRPriorityEnumToString(): Unknown RWR response priority enum: " << priority;
         throw std::runtime_error(strmerror.str());
         break;
   }
   return priority_str;
}

IADSLIB_EXPORT bool RWRPriorityStringToEnum(const std::string& input_str,
                                            eRWRResponsePriority& priority,
                                            std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
{
   bool processed = true;
   if (input_str == "Default")
   {
      priority = il::DEFAULT;
   }
   else if (input_str == "Choose-Closest-Threat")
   {
      priority = il::CHOOSE_CLOSEST_THREAT;
   }
   else
   {
      HCL_FATAL_LOGGER(aGlobalLogger) << "Unrecognized RWR response priority: " << input_str;
      processed = false;
   }
   return processed;
}

int GetMissileCountFromDoctrine(const eShotDoctrine doctrine, std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
{
   int missiles_to_commit = 0;
   switch (doctrine)
   {
      case SHOOT_1:
         missiles_to_commit = 1;
         break;
      case SHOOT_2:
      case SHOOT_LOOK_SHOOT:
         missiles_to_commit = 2;
         break;
      default:
         HCL_FATAL_LOGGER(aGlobalLogger) << "Unknown shot doctrine: " << doctrine;
   }

   return missiles_to_commit;
}

// Computes the time when an asset could intercept a track.
bool CalculateInterceptTime(const double sim_time_s, const positionRecord& assetPosition,
                            const trackRecord& targetTrack,
                            const double interceptSpeed,
                            double& interceptTime)
{
   // Inputs:
   // sim_time_s - The current time
   // targetTrack - The trackRecord of the target
   // interceptTime - A double in which the calculated time of intercept will be stored

   // Returns:
   // true - Valid intercept time computed and returned in the output variable InterceptTime.
   // false - No intercept time computed. Usually means AttackerSpeed is not large enough to catch the target.

   // Description of Algorithm:

   // The solution for intercept time assumes

   // 1) the target continues to fly with its current velocity (straight line, current speed)
   // 2) attacker will fly in a straight line at its current speed
   // 3) the attacker can turn immediately in any direction before moving in a straight line towards the intercept point.

   // The solution is found using a triangle with sides

   // 1) the relative position vector (TargetPos - InterceptorPos)
   // 2) a vector of length (magnitude of TargetVel*InterceptTime) in the direction of vector TargetVel
   // 3) a vector of length (AttackerSpeed*InterceptTime)

   // With this triangle, an equation for intercept time (T) can be written using the law of cosines

   // (Si*T)^2 = (St*T)^2 + D^2 - 2*D*St*T*cos(Theta)

   // where
   // Si = Interceptor speed
   // St = Target speed (magnitude of TargetVel)
   // D is the magnitude of the relative position vector (AttackerPos - TargetPos)
   // Theta is the angle between the vectors (AttackerPos - TargetPos) and TargetVel
   //   and making use of the fact that cos(x) = sin (pi/2 - x),
   //   cos(theta) = -(dot product of TargetVel and relative position vector) / (relative position vector * magnitude of TargetVel)

   // A little bit of algebraic manipulation can reduce the solution to

   // T = D * (- Sta +- sqrt(Si^2 - Stn^2))/(Si^2 - St^2)

   // where
   // Sta = the component of the St Along (parallel to) the relative position vector
   // Stn = the component of the St Normal (perpendicular) to the relative position vector

   // This solution presents problems when Si and St are about the same value, and rounding errors will have a large effect on the answer.
   // One also has to pick the right answer, presumably the smallest positive answer.

   // A much more robust solution is found by dividing the original equation by T^2. (T can only be 0 if D = 0). Then solving for SRel (D/T) we get

   // SRel = -Sta +- sqrt (Si**2 - Stn**2)

   // The '+' solution will give the largest SRel and hence the smallest T.

   // The conditions Si < Stn or SRel < 0 indicate that Si is too low to catch the target.

   auto track = targetTrack.getPropagatedPosition(sim_time_s);
   auto targetVelocity = targetTrack.getVelocityVector();

   // Calculate absolute distance between asset and target
   vm::Vector3<double> diff;
   diff.setX(track.getX() - assetPosition.getX());
   diff.setY(track.getY() - assetPosition.getY());
   diff.setZ(track.getZ() - assetPosition.getZ());
   double distance = diff.norm();

   // No distance between target and attacker?
   if (distance <= 0.0001)
   {
      interceptTime = sim_time_s;
      return true;
   }

   // This is a unidimensional vector in the same direction as the position vector from the interceptor to the target.
   double targetParallelVelocity = (targetVelocity.getX() * diff.getX() + targetVelocity.getY() * diff.getY() + targetVelocity.getZ() * diff.getZ()) / distance;

   // This is a scalar representing the component of the target velocity orthogonal to the position vector from the interceptor to the target.
   double targetPerpendicularSpeedSquared = targetVelocity.normSquared() - targetParallelVelocity * targetParallelVelocity;

   // This is a unidimensional vector in the same direction as the position vector from the interceptor to the target.
   double interceptorParallelVelocitySquared = interceptSpeed * interceptSpeed - targetPerpendicularSpeedSquared;

   // Asset intercept speed < target perpendicular velocity?
   if (interceptorParallelVelocitySquared <= 0.0)
   {
      return false;
   }

   // This is the speed of the interceptor relative to the speed of the target in the case that they will intercept.
   double interceptorRelativeSpeed = sqrt(interceptorParallelVelocitySquared) - targetParallelVelocity;

   // If the target is moving away, and the speed of the interceptor is insufficient to catch the target.
   if (interceptorRelativeSpeed <= 0.000001)
   {
      return false;
   }

   interceptTime = sim_time_s + distance / interceptorRelativeSpeed;
   return true;
}

int DetermineShotDoctrineDifference(const eShotDoctrine lhs,
                                    const eShotDoctrine rhs,
                                    std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
{
   return GetMissileCountFromDoctrine(lhs, aGlobalLogger) - GetMissileCountFromDoctrine(rhs, aGlobalLogger);
}

namespace
{
   const char UNACKNOWLEDGED_str[] = "Unacknowledged";
   const char WILCO_str[] = "Wilco";
   const char COMITTED_str[] = "Committed";
   const char CUE_TAR_str[] = "Cue TAR";
   const char CUE_TTR_str[] = "Cue TTR";
   const char TAR_SEARCHING_str[] = "TAR Searching";
   const char TTR_SEARCHING_str[] = "TTR Searching";
   const char TAR_TRACKING_str[] = "TAR Tracking";
   const char TTR_TRACKING_str[] = "TTR Tracking";
   const char FIRING_str[] = "Firing";
   const char MISS_str[] = "Miss";
   const char CHANGED_ASSIGNED_UNIT_str[] = "Changed Assigned Unit";
   const char UPDATED_SHOT_DOCTRINE_str[] = "Updated Shot Doctrine";
   const char KILL_str[] = "Kill";
   const char CANCELLED_str[] = "Cancelled";
   const char HAVCO_SUCCESS_str[] = "Havco Success";
   const char HAVCO_FAILURE_str[] = "Havco Failure";
   const char CANTCO_str[] = "CANTCO";

#define ASSIGNMENT_STATUS_ENUM_TO_STR(status_enum) ( status_enum ## _str )
}

IADSLIB_EXPORT std::string AssignmentStatusToString(const assignmentACKMessage::eAssignmentStatus status)
{
   std::string status_str;

   switch (status)
   {
      case assignmentACKMessage::UNACKNOWLEDGED:
         status_str = ASSIGNMENT_STATUS_ENUM_TO_STR(UNACKNOWLEDGED);
         break;
      case assignmentACKMessage::WILCO:
         status_str = ASSIGNMENT_STATUS_ENUM_TO_STR(WILCO);
         break;
      case assignmentACKMessage::COMITTED:
         status_str = ASSIGNMENT_STATUS_ENUM_TO_STR(COMITTED);
         break;
      case assignmentACKMessage::CUE_TAR:
         status_str = ASSIGNMENT_STATUS_ENUM_TO_STR(CUE_TAR);
         break;
      case assignmentACKMessage::CUE_TTR:
         status_str = ASSIGNMENT_STATUS_ENUM_TO_STR(CUE_TTR);
         break;
      case assignmentACKMessage::TAR_SEARCHING:
         status_str = ASSIGNMENT_STATUS_ENUM_TO_STR(TAR_SEARCHING);
         break;
      case assignmentACKMessage::TTR_SEARCHING:
         status_str = ASSIGNMENT_STATUS_ENUM_TO_STR(TTR_SEARCHING);
         break;
      case assignmentACKMessage::TAR_TRACKING:
         status_str = ASSIGNMENT_STATUS_ENUM_TO_STR(TAR_TRACKING);
         break;
      case assignmentACKMessage::TTR_TRACKING:
         status_str = ASSIGNMENT_STATUS_ENUM_TO_STR(TTR_TRACKING);
         break;
      case assignmentACKMessage::FIRING:
         status_str = ASSIGNMENT_STATUS_ENUM_TO_STR(FIRING);
         break;
      case assignmentACKMessage::MISS:
         status_str = ASSIGNMENT_STATUS_ENUM_TO_STR(MISS);
         break;
      case assignmentACKMessage::CHANGED_ASSIGNED_UNIT:
         status_str = ASSIGNMENT_STATUS_ENUM_TO_STR(CHANGED_ASSIGNED_UNIT);
         break;
      case assignmentACKMessage::UPDATED_SHOT_DOCTRINE:
         status_str = ASSIGNMENT_STATUS_ENUM_TO_STR(UPDATED_SHOT_DOCTRINE);
         break;
      case assignmentACKMessage::KILL:
         status_str = ASSIGNMENT_STATUS_ENUM_TO_STR(KILL);
         break;
      case assignmentACKMessage::CANCELLED:
         status_str = ASSIGNMENT_STATUS_ENUM_TO_STR(CANCELLED);
         break;
      case assignmentACKMessage::HAVCO_SUCCESS:
         status_str = ASSIGNMENT_STATUS_ENUM_TO_STR(HAVCO_SUCCESS);
         break;
      case assignmentACKMessage::HAVCO_FAILURE:
         status_str = ASSIGNMENT_STATUS_ENUM_TO_STR(HAVCO_FAILURE);
         break;
      case assignmentACKMessage::CANTCO:
         status_str = ASSIGNMENT_STATUS_ENUM_TO_STR(CANTCO);
         break;
      default:
         std::stringstream strmerror;
         strmerror << "AssignmentStatusToString(): Invalid assignment status: " << status;
         throw std::runtime_error(strmerror.str());
   }

   return status_str;
}

assignmentACKMessage::eAssignmentStatus AssignmentStatusFromString(const std::string& status_str)
{
   assignmentACKMessage::eAssignmentStatus status;

   if (status_str == ASSIGNMENT_STATUS_ENUM_TO_STR(UNACKNOWLEDGED))
   {
      status = assignmentACKMessage::UNACKNOWLEDGED;
   }
   else if (status_str == ASSIGNMENT_STATUS_ENUM_TO_STR(WILCO))
   {
      status = assignmentACKMessage::WILCO;
   }
   else if (status_str == ASSIGNMENT_STATUS_ENUM_TO_STR(COMITTED))
   {
      status = assignmentACKMessage::COMITTED;
   }
   else if (status_str == ASSIGNMENT_STATUS_ENUM_TO_STR(CUE_TAR))
   {
      status = assignmentACKMessage::CUE_TAR;
   }
   else if (status_str == ASSIGNMENT_STATUS_ENUM_TO_STR(CUE_TTR))
   {
      status = assignmentACKMessage::CUE_TTR;
   }
   else if (status_str == ASSIGNMENT_STATUS_ENUM_TO_STR(TAR_SEARCHING))
   {
      status = assignmentACKMessage::TAR_SEARCHING;
   }
   else if (status_str == ASSIGNMENT_STATUS_ENUM_TO_STR(TTR_SEARCHING))
   {
      status = assignmentACKMessage::TTR_SEARCHING;
   }
   else if (status_str == ASSIGNMENT_STATUS_ENUM_TO_STR(TAR_TRACKING))
   {
      status = assignmentACKMessage::TAR_TRACKING;
   }
   else if (status_str == ASSIGNMENT_STATUS_ENUM_TO_STR(TTR_TRACKING))
   {
      status = assignmentACKMessage::TTR_TRACKING;
   }
   else if (status_str == ASSIGNMENT_STATUS_ENUM_TO_STR(FIRING))
   {
      status = assignmentACKMessage::FIRING;
   }
   else if (status_str == ASSIGNMENT_STATUS_ENUM_TO_STR(MISS))
   {
      status = assignmentACKMessage::MISS;
   }
   else if (status_str == ASSIGNMENT_STATUS_ENUM_TO_STR(CHANGED_ASSIGNED_UNIT))
   {
      status = assignmentACKMessage::CHANGED_ASSIGNED_UNIT;
   }
   else if (status_str == ASSIGNMENT_STATUS_ENUM_TO_STR(UPDATED_SHOT_DOCTRINE))
   {
      status = assignmentACKMessage::UPDATED_SHOT_DOCTRINE;
   }
   else if (status_str == ASSIGNMENT_STATUS_ENUM_TO_STR(KILL))
   {
      status = assignmentACKMessage::KILL;
   }
   else if (status_str == ASSIGNMENT_STATUS_ENUM_TO_STR(CANCELLED))
   {
      status = assignmentACKMessage::CANCELLED;
   }
   else if (status_str == ASSIGNMENT_STATUS_ENUM_TO_STR(HAVCO_SUCCESS))
   {
      status = assignmentACKMessage::HAVCO_SUCCESS;
   }
   else if (status_str == ASSIGNMENT_STATUS_ENUM_TO_STR(HAVCO_FAILURE))
   {
      status = assignmentACKMessage::HAVCO_FAILURE;
   }
   else if (status_str == ASSIGNMENT_STATUS_ENUM_TO_STR(CANTCO))
   {
      status = assignmentACKMessage::CANTCO;
   }
   else
   {
      std::stringstream strmerror;
      strmerror << "AssignmentStatusFromString(): Invalid assignment status: " << status_str;
      throw std::runtime_error(strmerror.str());
   }

   return status;
}

IADSLIB_EXPORT idRecord GetAssignedIDFromWeaponRef(const std::shared_ptr<weaponRecord>& weapon)
{
   // the assigned id will be the parent unit id and the weapon id
   return GetAssignedIDFromWeaponIDs(weapon->GetParentAssetID().getID(), weapon->GetWeaponID().getID());
}

idRecord GetAssignedIDFromWeaponIDs(const idRecord& weapon_unit_id,
                                    const idRecord& weapon_id)
{
   // the assigned id will be the parent unit id and the weapon id
   return idRecord(weapon_unit_id.getID(), weapon_id.getID(), weapon_unit_id.getIDString(), weapon_id.getIDString());
}

std::shared_ptr<positionRecord> GetPropagatedPosition(const positionRecord& TargetPosition,
                                                      const vm::Vector3<double>& EstimatedVelocity,
                                                      const double TimeDelta)
{
   positionRecord* posPtr = new positionRecord();

   std::shared_ptr<positionRecord> propagatedPosition(posPtr);

   propagatedPosition->setXYZ(TargetPosition.getX() + EstimatedVelocity.getX() * TimeDelta,
                              TargetPosition.getY() + EstimatedVelocity.getY() * TimeDelta,
                              TargetPosition.getZ() + EstimatedVelocity.getZ() * TimeDelta);

   return propagatedPosition;
}

std::string getSensorTypeStringFromEnum(SensorEnum typeEnum, std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
{
   std::string typeString = C_UNKNOWN_SENSOR_TYPE_STR;

   switch (typeEnum)
   {
      case SENSOR_EW:
         typeString = C_EW_SENSOR_TYPE_STR;
         break;
      case SENSOR_TAR:
         typeString = C_TAR_SENSOR_TYPE_STR;
         break;
      case SENSOR_TTR:
         typeString = C_TTR_SENSOR_TYPE_STR;
         break;
      case SENSOR_RWR:
         typeString = C_RWR_SENSOR_TYPE_STR;
         break;
      default:
         assert(false);
         HCL_ERROR_LOGGER(aGlobalLogger) << "getSensorTypeStringFromEnum(): Invalid type enum: " << (int)typeEnum;
      case SENSOR_UNKNOWN:
         typeString = C_UNKNOWN_SENSOR_TYPE_STR;
         break;
   }

   return typeString;
}

il::SensorEnum getSensorTypeEnumFromString(std::string typeString)
{
   SensorEnum typeEnum;

   if (typeString == C_EW_SENSOR_TYPE_STR)
   {
      typeEnum = SENSOR_EW;
   }
   else if (typeString == C_TAR_SENSOR_TYPE_STR)
   {
      typeEnum = SENSOR_TAR;
   }
   else if (typeString == C_TTR_SENSOR_TYPE_STR)
   {
      typeEnum = SENSOR_TTR;
   }
   else if (typeString == C_RWR_SENSOR_TYPE_STR)
   {
      typeEnum = SENSOR_RWR;
   }
   else
   {
      typeEnum = SENSOR_UNKNOWN;
   }

   return typeEnum;
}

std::size_t GetTotalAvailableTTRCues(const std::shared_ptr<assetRecord>& asset)
{
   std::size_t num_sensors = 0;
   auto sensors = asset->getSensors();
   for (auto it = sensors.cbegin(); it != sensors.cend(); ++it)
   {
      const auto& sensor = *it;
      if (sensor->IsTTR())
      {
         ++num_sensors;
      }
   }
   return num_sensors;
}

} // namespace il
