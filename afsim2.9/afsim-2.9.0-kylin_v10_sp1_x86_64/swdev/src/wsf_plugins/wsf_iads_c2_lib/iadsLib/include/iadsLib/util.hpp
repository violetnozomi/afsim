// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  @file util.hpp
 *  @author Kenneth R. Sewell III
 *          Kenneth.Sewell@wpafb.af.mil
 *          AFRL/RYWF
 *          2241 Avionics Circle
 *          WPAFB, Ohio 45433-7303
 *          937-656-4899 x3004
 *  @brief Utility functions.
 *
 *  Classification: UNCLASSIFIED
 */
#ifndef IADSLIB_UTIL_HPP
#define IADSLIB_UTIL_HPP

#include <string>
#include <iadsLib/positionRecord.hpp>
#include <iadsLib/enum.hpp>
#include <iadsLib/assessmentRecord.hpp>
#include <iadsLib/assetRecord.hpp>
#include <iadsLib/assignmentMessage.hpp>
#include <iadsLib/trackRecord.hpp>
#include <iadsLib/threatRecordInterface.hpp>
#include <iadsLib/assignmentACKMessage.hpp>
#include <iadsLib/exclusionArray.hpp>

namespace il
{
   #define EXTERNAL_POINTER(TYPE) TYPE *

   /**
      @brief Calculate angular offset from track to asset.
      @param track Track to use as starting point.
      @param asset Asset to use as ending point.
      @returns Angular offset from track to asset in radians.
   */
   IADSLIB_EXPORT double angleOffset(const positionRecord &TrackPosition,
                                     const double &Heading_rads,
                                     const positionRecord &AssetPosition);

   positionRecord
   findPointOfClosestApproach(const positionRecord &CurrentPos,
                              const positionRecord &PredictedPos,
                              const double &HeadingOffsetRads,
                              const double &Velocity,
                              const positionRecord &AssetPos);

   IADSLIB_EXPORT std::string AssetTypeEnumToString(const AssetTypeEnum asset_type);

   IADSLIB_EXPORT bool IsAssessmentValid(const il::assessmentRecord &assessment,
                                         const il::assetMap &assets,
                                         const std::shared_ptr<il::assetRecord> &MyUnit,
                                         std::weak_ptr<logger::GlobalLogger> aGlobalLogger);
   // note: that potential assigned id must be built from the weapon id and parent attached unit id
   IADSLIB_EXPORT bool IsUnitCapableOfAssignment(const il::exclusionArray &exclusion_array,
                                                 const idRecord &potential_assigned_id,
                                                 const idRecord &track_id);

   IADSLIB_EXPORT std::string BoolToString(const bool Value);

   bool IsTrackProjectedInsideZone(const std::vector<std::shared_ptr<zoneRecordInterface>> &zones,
                                   const positionRecord &pos1,
                                   const positionRecord &pos2,
                                   bool true_if_no_zones = true);
   // alternative version of above that returns all zone types the track is projected into - it does not clear the existing flags
   bool IsTrackProjectedInsideZone(const std::vector<std::shared_ptr<zoneRecordInterface>> &zones,
                                   const positionRecord &pos1,
                                   const positionRecord &pos2, tZoneTypeMask &zone_types,
                                   const eZoneType zone_type_if_no_zones,
                                   bool true_if_no_zones = true);

   bool IsTrackProjectedInsideZone(const std::vector<std::shared_ptr<zoneRecordInterface>> &zones,
                                   const positionRecord &pos,
                                   const double heading_rads, bool true_if_no_zones = true);
   bool IsTrackProjectedInsideZone(const std::vector<std::shared_ptr<zoneRecordInterface>> &zones,
                                   const positionRecord &pos,
                                   const double heading_rads, tZoneTypeMask &zone_types,
                                   const eZoneType zone_type_if_no_zones,
                                   bool true_if_no_zones = true);


   // check intermediate unit zones for projection of the track
   bool IsTrackProjectedInsideIntermediateC2Zones(const assetMap &assets,
                                                  const idRecord &cmdr_begin,
                                                  const idRecord &sub_end,
                                                  const positionRecord &position,
                                                  const double heading_rads, bool true_if_no_zones = true);
   bool IsTrackProjectedInsideIntermediateC2Zones(const assetMap &assets,
                                                  const idRecord &cmdr_begin,
                                                  const idRecord &sub_end,
                                                  const positionRecord &position,
                                                  const double heading_rads,
                                                  tZoneTypeMask &zone_types,
                                                  const eZoneType zone_type_if_no_zones,
                                                  bool true_if_no_zones = true);

   IADSLIB_EXPORT bool IsInsideZone(const std::vector<std::shared_ptr<zoneRecordInterface>> &zones,
                                    const positionRecord &pos, bool true_if_no_zones = true);
   // alternative version of above that returns all zone types the track is projected into - it does not clear the existing flags
   IADSLIB_EXPORT bool IsInsideZone(const std::vector<std::shared_ptr<zoneRecordInterface>> &zones,
                                    const positionRecord &pos, tZoneTypeMask &zone_types,
                                    const eZoneType zone_type_if_no_zones,
                                    bool true_if_no_zones = true);

   bool GetTrackTruthNameID(const idRecord &track_id,
                            const trackMap &tracks, std::string &name, idRecord &truthid);

   IADSLIB_EXPORT std::string IFFEnumToString(const eIFFType IFF);

   IADSLIB_EXPORT bool IsThreatAllocatedToZoneType(const std::shared_ptr<threatRecord> &threat,
                                                   const eZoneType type);

   IADSLIB_EXPORT std::shared_ptr<assignmentMessage> GetDelegatedAssignment(const tRankedThreats::value_type &threat);

   IADSLIB_EXPORT std::string ShotDoctrineEnumToString(const eShotDoctrine shot_doctrine);
   IADSLIB_EXPORT bool ShotDoctrineStringToEnum(const std::string &input_str, eShotDoctrine &doctrine, std::weak_ptr<logger::GlobalLogger> aGlobalLogger);
   int GetMissileCountFromDoctrine(const eShotDoctrine doctrine, std::weak_ptr<logger::GlobalLogger> aGlobalLogger);
   int DetermineShotDoctrineDifference(const eShotDoctrine lhs, const eShotDoctrine rhs, std::weak_ptr<logger::GlobalLogger> aGlobalLogger);

   IADSLIB_EXPORT std::string AssignmentStatusToString(const assignmentACKMessage::eAssignmentStatus status);
   assignmentACKMessage::eAssignmentStatus AssignmentStatusFromString(const std::string &status_str);

   IADSLIB_EXPORT idRecord GetAssignedIDFromWeaponRef(const std::shared_ptr<weaponRecord> &weapon);
   idRecord GetAssignedIDFromWeaponIDs(const idRecord &weapon_unit_id, const idRecord &weapon_id);

   std::shared_ptr<positionRecord> GetPropagatedPosition(const positionRecord &TargetPosition,
                                                         const vm::Vector3<double> &EstimatedVelocity,
                                                         const double TimeDelta);

   IADSLIB_EXPORT std::string TakeActionEnumToString(const eTakeAction take_action);
   IADSLIB_EXPORT bool TakeActionStringToEnum(const std::string &input_str, eTakeAction &action, std::weak_ptr<logger::GlobalLogger> aGlobalLogger);
   IADSLIB_EXPORT std::string AssessEngageEnumToString(const eAssessEngage assess);
   IADSLIB_EXPORT bool AssessEngageStringToEnum(const std::string &input_str, eAssessEngage &assess, std::weak_ptr<logger::GlobalLogger> aGlobalLogger);
   IADSLIB_EXPORT std::string RWRPriorityEnumToString(const eRWRResponsePriority priority);
   IADSLIB_EXPORT bool RWRPriorityStringToEnum(const std::string &input_str, eRWRResponsePriority &priority, std::weak_ptr<logger::GlobalLogger> aGlobalLogger);

   // sensor enumerations
   IADSLIB_EXPORT std::string getSensorTypeStringFromEnum(SensorEnum typeEnum, std::weak_ptr<logger::GlobalLogger> aGlobalLogger);
   IADSLIB_EXPORT SensorEnum getSensorTypeEnumFromString(std::string typeString);

   IADSLIB_EXPORT bool CalculateInterceptTime(const double sim_time_s,
                                              const positionRecord &assetPosition,
                                              const trackRecord &targetTrack,
                                              const double interceptSpeed,
                                              double &interceptTime);

   // some sensor helpers
   std::size_t GetTotalAvailableTTRCues(const std::shared_ptr<il::assetRecord> &asset);
}

#endif
