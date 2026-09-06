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
*   File: WsfBMMOELogger.cpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name: Radiance Technologies
*   Address:         350 Wynn Drive
*                    Huntsville, AL 35805
*
*   Abstract: MOE Logger for the battle managers in ITASE. These will probably
*    be switched over to more expressive events eventually but currently
*    since we're riding on top of the WsfTask systems, we can't necessarily
*    hijack those event logs.
*
*******************************************************************************/

#include <WsfBMCSV_EventOutput.hpp>


#include <iadsLib/assetRecord.hpp>
#include <iadsLib/trackRecord.hpp>
#include <iadsLib/idRecord.hpp>
#include <iadsLib/assignmentContainers.hpp>
#include <iadsLib/util.hpp>
#include <iadsLib/assignmentACKMessage.hpp>
#include <iadsLib/assignmentMessage.hpp>
#include <iadsLib/assessmentRecord.hpp>
#include <iadsLib/sensorRecordInterface.hpp>

#include <WsfBMEventOutput.hpp>
#include <WsfBMCSV_EventOutput.hpp>
#include <WsfBMMOELogger.hpp>
#include <WsfBMUtils.hpp>

#include "WsfCSV_EventOutput.hpp"
#include "WsfEventOutput.hpp"
#include "WsfPlatform.hpp"
#include "WsfWeapon.hpp"
#include "UtScriptTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfBMEvalAssignment.hpp"
#include "WsfIADSC2ScenarioExtension.hpp"
#include "WsfIADSC2UnclassProcRegistration.hpp"

// The MOE Logger constructor ensures the EventOutput and CSVEventOutput are initialized to the correct class.
WsfBMHEFIRMOELogger::WsfBMHEFIRMOELogger(WsfSimulation* aSimulation)
   : il::moeLoggingInterface(IADSC2ScenarioExtension::GetGlobalLogger(aSimulation->GetScenario()))
   , mSimulationPtr(aSimulation)
   , mEventOutput()
   , mCSV_EventOutput()
{
   // If the event_output extension is available, hook in the handlers for our events.
   WsfEventOutput* eventOutputPtr = WsfEventOutput::Find(*aSimulation);
   if (eventOutputPtr != nullptr)
   {
      mEventOutput = std::shared_ptr<WsfBMEventOutput>(new WsfBMEventOutput(*eventOutputPtr, this));
   }

   // If the csv_event_output extension is available, hook in the handlers for our events.
   WsfCSV_EventOutput* csvEventOutputPtr = WsfCSV_EventOutput::Find(*aSimulation);
   if (csvEventOutputPtr != nullptr)
   {
      mCSV_EventOutput = std::shared_ptr<WsfBMCSV_EventOutput>(new WsfBMCSV_EventOutput(*csvEventOutputPtr, this));
   }
}

void WsfBMHEFIRMOELogger::Assignment(const double curr_time, const il::evaluationAssignment* assignment, const std::string assignment_flag)
{
   // we really only use truth here where logging needs it (not provided by the internal model datastructures or would require
   // a lookup on the internal model databases)
   auto assigning_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(mSimulationPtr, assignment->Assignment()->getAssigningID());
   auto assigned_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(mSimulationPtr, assignment->Assignment()->getAssignedID().getID());
   WsfWeapon* assigned_wpn = nullptr;
   if (assigned_plat)
   {
      assigned_wpn = assigned_plat->GetComponentEntry<WsfWeapon>(assignment->Assignment()->getAssignedID().getSubID());
   }

   auto target_plat = mSimulationPtr->GetPlatformByName(WsfStringInt(WsfBMUtils::GetStringId(assignment->Track()->getTargetTruthName())));

   // we use truth logging to log the targets actual location
   double target_lla[3] = { 0 };
   if (target_plat)
   {
      target_plat->GetLocationLLA(target_lla[0], target_lla[1], target_lla[2]);
   }

   HCL_INFO_LOGGER(mGlobalLogger) << "DREVAL,ASSIGNMENT,"
                     << curr_time << ","
                     << assignment_flag << ","
                     << assignment->Assignment()->getAssignTime() << ","
                     << (assigning_plat ? assigning_plat->GetName() : "unknown") << ","
                     << (assigning_plat ? assigning_plat->GetSide() : "unknown") << ","
                     << "N/A" << ","
                     << (assigned_plat ? assigned_plat->GetName() : "unknown") << ","
                     << (assigned_wpn ? assigned_wpn->GetName() : "unknown") << ","
                     << (target_plat ? target_plat->GetName() : "unknown") << ","
                     << (target_plat ? target_plat->GetSide() : "unknown") << ","
                     << il::ShotDoctrineEnumToString(assignment->Assignment()->getShotDoctrine()) << ","
                     << target_lla[0] << ","
                     << target_lla[1] << ","
                     << target_lla[2] << ","
                     << assignment->Track()->getReportingSensorType() << ","
                     << assignment->Assignment()->getReferenceTrackID() << ","
                     << assignment->Assignment()->getLocalTrackID();

   OnAssignment(curr_time, assignment, assignment_flag);
}

void WsfBMHEFIRMOELogger::AssignmentStatus(const double curr_time, const il::assignmentACKMessage* status, const il::evaluationAssignment* related_assignment)
{
   // this is not actually a HEFIR message since the statuses are expected ot be logged by the issuer of said statuses. But since
   // we're operating with a set of models that we don't necessarily control, do our own logging when we receive these just so
   // we can get better reporting back to the analyst
   std::string status_type = il::AssignmentStatusToString(status->getStatus());

   // we really only use truth here where logging needs it (not provided by the internal model datastructures or would require
   // a lookup on the internal model databases)
   auto assigning_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(mSimulationPtr, related_assignment->Assignment()->getAssigningID());
   auto assigned_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(mSimulationPtr, related_assignment->Assignment()->getAssignedID().getID());
   WsfWeapon* assigned_wpn = nullptr;
   if (assigned_plat)
   {
      assigned_wpn = assigned_plat->GetComponentEntry<WsfWeapon>(related_assignment->Assignment()->getAssignedID().getSubID());
   }
   auto target_plat = mSimulationPtr->GetPlatformByName(WsfStringInt(WsfBMUtils::GetStringId(related_assignment->Track()->getTargetTruthName())));

   il::assignmentACKMessage::eSystemicCantcoScope systemic_scope;
   std::string systemic_scope_str = "none";
   if (status->getSystemicCantcoScope(systemic_scope))
   {
      if (systemic_scope == il::assignmentACKMessage::LOCAL)
      {
         systemic_scope_str = "Local";
      }
      else
      {
         systemic_scope_str = "Global";
      }
   }

   if (status->getStatus() == il::assignmentACKMessage::CHANGED_ASSIGNED_UNIT)
   {
      auto new_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(mSimulationPtr, status->getNewlyAssignedID().getID());
      WsfWeapon* new_wpn = nullptr;
      if (new_plat)
      {
         new_wpn = new_plat->GetComponentEntry<WsfWeapon>(status->getNewlyAssignedID().getSubID());
      }

      HCL_INFO_LOGGER(mGlobalLogger) << "DREVAL,ASSIGNMENT_OVERRIDE,"
                        << curr_time << ","
                        << status_type << ","
                        << status->getAckTime() << ","
                        << related_assignment->Assignment()->getAssignTime() << ","
                        << (assigning_plat ? assigning_plat->GetName() : "unknown") << ","
                        << (assigning_plat ? assigning_plat->GetSide() : "unknown") << ","
                        << (assigned_plat ? assigned_plat->GetName() : "unknown") << ","
                        << (assigned_wpn ? assigned_wpn->GetName() : "unknown") << ","
                        << (new_plat ? new_plat->GetName() : "unknown") << ","
                        << (new_wpn ? new_wpn->GetName() : "unknown") << ","
                        << (target_plat ? target_plat->GetName() : "unknown") << ","
                        << (target_plat ? target_plat->GetSide() : "unknown") << ","
                        << il::BoolToString(status->isSystemicCantco()) << ","
                        << systemic_scope_str << ","
                        << related_assignment->Assignment()->getReferenceTrackID() << ","
                        << related_assignment->Assignment()->getLocalTrackID() << ","
                        << il::ShotDoctrineEnumToString(related_assignment->Assignment()->getShotDoctrine());
   }
   else if (status->getStatus() == il::assignmentACKMessage::UPDATED_SHOT_DOCTRINE)
   {
      HCL_INFO_LOGGER(mGlobalLogger) << "DREVAL,ASSIGNMENT_DOCTRINE_OVERRIDE,"
                        << curr_time << ","
                        << status_type << ","
                        << status->getAckTime() << ","
                        << related_assignment->Assignment()->getAssignTime() << ","
                        << (assigning_plat ? assigning_plat->GetName() : "unknown") << ","
                        << (assigning_plat ? assigning_plat->GetSide() : "unknown") << ","
                        << (assigned_plat ? assigned_plat->GetName() : "unknown") << ","
                        << (assigned_wpn ? assigned_wpn->GetName() : "unknown") << ","
                        << (target_plat ? target_plat->GetName() : "unknown") << ","
                        << (target_plat ? target_plat->GetSide() : "unknown") << ","
                        << il::BoolToString(status->isSystemicCantco()) << ","
                        << systemic_scope_str << ","
                        << related_assignment->Assignment()->getReferenceTrackID() << ","
                        << related_assignment->Assignment()->getLocalTrackID() << ","
                        << il::ShotDoctrineEnumToString(related_assignment->Assignment()->getShotDoctrine()) << ","
                        << il::ShotDoctrineEnumToString(status->getUpdatedShotDoctrine());
   }
   else
   {
      HCL_INFO_LOGGER(mGlobalLogger) << "DREVAL,ASSIGNMENT_STATUS,"
                        << curr_time << ","
                        << status_type << ","
                        << status->getAckTime() << ","
                        << related_assignment->Assignment()->getAssignTime() << ","
                        << (assigning_plat ? assigning_plat->GetName() : "unknown") << ","
                        << (assigning_plat ? assigning_plat->GetSide() : "unknown") << ","
                        << (assigned_plat ? assigned_plat->GetName() : "unknown") << ","
                        << (assigned_wpn ? assigned_wpn->GetName() : "unknown") << ","
                        << (target_plat ? target_plat->GetName() : "unknown") << ","
                        << (target_plat ? target_plat->GetSide() : "unknown") << ","
                        << il::BoolToString(status->isSystemicCantco()) << ","
                        << systemic_scope_str << ","
                        << related_assignment->Assignment()->getReferenceTrackID() << ","
                        << related_assignment->Assignment()->getLocalTrackID() << ","
                        << il::ShotDoctrineEnumToString(related_assignment->Assignment()->getShotDoctrine());
   }

   OnAssignmentStatus(curr_time, status, related_assignment);
}

void WsfBMHEFIRMOELogger::AssignmentCancel(const double curr_time, const il::idRecord* cancelling_unit, const il::evaluationAssignment* active_assignment, const std::string& cancel_reason)
{
   auto cancelling_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(mSimulationPtr, *cancelling_unit);
   auto assigning_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(mSimulationPtr, active_assignment->Assignment()->getAssigningID());
   auto assigned_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(mSimulationPtr, active_assignment->Assignment()->getAssignedID().getID());
   WsfWeapon* assigned_wpn = nullptr;
   if (assigned_plat)
   {
      assigned_wpn = assigned_plat->GetComponentEntry<WsfWeapon>(active_assignment->Assignment()->getAssignedID().getSubID());
   }
   auto target_plat = mSimulationPtr->GetPlatformByName(WsfStringInt(WsfBMUtils::GetStringId(active_assignment->Track()->getTargetTruthName())));

   HCL_INFO_LOGGER(mGlobalLogger) << "DREVAL,CANCEL,"
                     << curr_time << ","
                     << active_assignment->Assignment()->getAssignTime() << ","
                     << (cancelling_plat ? cancelling_plat->GetName() : "unknown") << ","
                     << (cancelling_plat ? cancelling_plat->GetSide() : "unknown") << ","
                     << (assigning_plat ? assigning_plat->GetName() : "unknown") << ","
                     << (assigning_plat ? assigning_plat->GetSide() : "unknown") << ","
                     << (assigned_plat ? assigned_plat->GetName() : "unknown") << ","
                     << (assigned_wpn ? assigned_wpn->GetName() : "unknown") << ","
                     << (target_plat ? target_plat->GetName() : "unknown") << ","
                     << (target_plat ? target_plat->GetSide() : "unknown") << ","
                     << cancel_reason << ","
                     << active_assignment->Assignment()->getReferenceTrackID() << ","
                     << active_assignment->Assignment()->getLocalTrackID();

   OnAssignmentCancel(curr_time, cancelling_unit, active_assignment, cancel_reason);
}

void WsfBMHEFIRMOELogger::CANTCOAssignment(const double curr_time, const il::idRecord* responding_unit, const il::assignmentMessage* assignment, const il::trackRecord* opt_track, const std::string& reason)
{
   auto reporting_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(mSimulationPtr, *responding_unit);
   auto assigning_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(mSimulationPtr, assignment->getAssigningID());
   auto assigned_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(mSimulationPtr, assignment->getAssignedID().getID());
   WsfWeapon* assigned_wpn = nullptr;
   if (assigned_plat)
   {
      assigned_wpn = assigned_plat->GetComponentEntry<WsfWeapon>(assignment->getAssignedID().getSubID());
   }
   auto target_plat = opt_track ? mSimulationPtr->GetPlatformByName(WsfStringInt(WsfBMUtils::GetStringId(opt_track->getTargetTruthName()))) : nullptr;

   HCL_INFO_LOGGER(mGlobalLogger) << "DREVAL,CANTCO,"
                     << curr_time << ","
                     << assignment->getAssignTime() << ","
                     << (reporting_plat ? reporting_plat->GetName() : "unknown") << ","
                     << (reporting_plat ? reporting_plat->GetSide() : "unknown") << ","
                     << (assigning_plat ? assigning_plat->GetName() : "unknown") << ","
                     << (assigning_plat ? assigning_plat->GetSide() : "unknown") << ","
                     << (assigned_plat ? assigned_plat->GetName() : "unknown") << ","
                     << (assigned_wpn ? assigned_wpn->GetName() : "unknown") << ","
                     << (target_plat ? target_plat->GetName() : "unknown") << ","
                     << (target_plat ? target_plat->GetSide() : "unknown") << ","
                     << reason << ","
                     << assignment->getReferenceTrackID() << ","
                     << assignment->getLocalTrackID();

   OnCANTCOAssignment(curr_time, responding_unit, assignment, opt_track, reason);
}

void WsfBMHEFIRMOELogger::WILCOAssignment(const double curr_time, const il::assignmentACKMessage* status, const il::evaluationAssignment* related_assignment)
{
   AssignmentStatus(curr_time, status, related_assignment);
}

void WsfBMHEFIRMOELogger::HAVCOAssignment(const double curr_time, const il::assignmentACKMessage* status, const il::evaluationAssignment* related_assignment)
{
   AssignmentStatus(curr_time, status, related_assignment);
}

void WsfBMHEFIRMOELogger::Allocate(const double curr_time, const il::idRecord* allocating_unit, const il::assessmentRecord* assessment)
{
   auto allocating_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(mSimulationPtr, *allocating_unit);
   auto target_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(mSimulationPtr, assessment->getAssessedThreatRecord()->getTrack()->getTargetTruthID());

   std::string zone_types;
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_OTHER) ? il::zoneTypeToName(il::E_ZONE_TYPE_OTHER, mGlobalLogger) : "";
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_AOR) ? "," + il::zoneTypeToName(il::E_ZONE_TYPE_AOR, mGlobalLogger) : "";
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_SUR) ? "," + il::zoneTypeToName(il::E_ZONE_TYPE_SUR, mGlobalLogger) : "";
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_MEZ) ? "," + il::zoneTypeToName(il::E_ZONE_TYPE_MEZ, mGlobalLogger) : "";
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_FEZ) ? "," + il::zoneTypeToName(il::E_ZONE_TYPE_FEZ, mGlobalLogger) : "";
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_JEZ) ? "," + il::zoneTypeToName(il::E_ZONE_TYPE_JEZ, mGlobalLogger) : "";
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_GEZ) ? "," + il::zoneTypeToName(il::E_ZONE_TYPE_GEZ, mGlobalLogger) : "";
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_DA) ? "," + il::zoneTypeToName(il::E_ZONE_TYPE_DA, mGlobalLogger) : "";
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_COR) ? "," + il::zoneTypeToName(il::E_ZONE_TYPE_COR, mGlobalLogger) : "";

   HCL_INFO_LOGGER(mGlobalLogger) << "DREVAL,ALLOCATE,"
                     << curr_time << ","
                     << (allocating_plat ? allocating_plat->GetName() : "unknown") << ","
                     << (allocating_plat ? allocating_plat->GetSide() : "unknown") << ","
                     << zone_types << ","
                     << "not specified" << ","
                     << (target_plat ? target_plat->GetName() : "unknown") << ","
                     << (target_plat ? target_plat->GetSide() : "unknown") << ","
                     << assessment->getAssessedThreatRecord()->getTrack()->getID();

   OnAllocate(curr_time, allocating_unit, assessment);
}

void WsfBMHEFIRMOELogger::SensorCue(const double curr_time, const il::assetRecord* curr_unit, const il::trackRecord* master_track, const il::assignmentMessage* assignment, const il::sensorRecord* sensor_to_assign)
{
   auto target_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(mSimulationPtr, master_track->getTargetTruthID());

   HCL_INFO_LOGGER(mGlobalLogger) << "DREVAL,"
                     << il::getSensorTypeStringFromEnum(sensor_to_assign->GetSensorType(), mGlobalLogger) << "_CUE" << ","
                     << curr_time << ","
                     << curr_unit->getName() << ","
                     << curr_unit->getSide() << ","
                     << sensor_to_assign->GetParentAsset()->getName() << ","
                     << sensor_to_assign->GetParentAsset()->getSide() << ","
                     << sensor_to_assign->GetSensorID().getIDString() << ","
                     << (target_plat ? target_plat->GetName() : "unknown") << ","
                     << master_track->getTargetTruthID().getID() << ","
                     << (target_plat ? target_plat->GetSide() : "unknown") << ","
                     << master_track->getID();

   OnSensorCue(curr_time, curr_unit, master_track, assignment, sensor_to_assign);
}

void WsfBMHEFIRMOELogger::SensorDropCue(const double curr_time, const il::assetRecord* curr_unit, const il::trackRecord* master_track, const il::assignmentMessage* assignment, const il::sensorRecord* sensor_to_assign)
{
   auto target_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(mSimulationPtr, master_track->getTargetTruthID());

   HCL_INFO_LOGGER(mGlobalLogger) << "DREVAL,"
                     << "DROP_" << il::getSensorTypeStringFromEnum(sensor_to_assign->GetSensorType(), mGlobalLogger) << "_CUE" << ","
                     << curr_time << ","
                     << curr_unit->getName() << ","
                     << curr_unit->getSide() << ","
                     << sensor_to_assign->GetParentAsset()->getName() << ","
                     << sensor_to_assign->GetParentAsset()->getSide() << ","
                     << sensor_to_assign->GetSensorID().getIDString() << ","
                     << (target_plat ? target_plat->GetName() : "unknown") << ","
                     << master_track->getTargetTruthID().getID() << ","
                     << (target_plat ? target_plat->GetSide() : "unknown") << ","
                     << master_track->getID();

   OnSensorDropCue(curr_time, curr_unit, master_track, assignment, sensor_to_assign);
}

void WsfBMHEFIRMOELogger::SensorTracking(const double curr_time, const il::trackRecord* master_track, const il::sensorRecord* assigned_sensor)
{
   auto target_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(mSimulationPtr, master_track->getTargetTruthID());
   double target_lla[3] = { 0 };
   if (target_plat)
   {
      target_plat->GetLocationLLA(target_lla[0], target_lla[1], target_lla[2]);
   }

   auto tracking_loc = assigned_sensor->GetParentAsset()->getPosition();

   HCL_INFO_LOGGER(mGlobalLogger) << "DREVAL,"
                     << il::getSensorTypeStringFromEnum(assigned_sensor->GetSensorType(), mGlobalLogger) << "_TRK" << ","
                     << curr_time << ","
                     << assigned_sensor->GetParentAsset()->getName() << ","
                     << assigned_sensor->GetParentAsset()->getID() << ","
                     << assigned_sensor->GetParentAsset()->getSide() << ","
                     << assigned_sensor->GetSensorID() << ","
                     << tracking_loc.getLatDegs() << ","
                     << tracking_loc.getLonDegs() << ","
                     << tracking_loc.getAltM() << ","
                     << (target_plat ? target_plat->GetName() : "unknown") << ","
                     << master_track->getTargetTruthID().getID() << ","
                     << (target_plat ? target_plat->GetSide() : "unknown") << ","
                     << target_lla[0] << ","
                     << target_lla[1] << ","
                     << target_lla[2] << ","
                     << master_track->getID();

   OnSensorTracking(curr_time, master_track, assigned_sensor);
}

void WsfBMHEFIRMOELogger::AIAddBehavior(const double curr_time, const std::string behavior, const il::trackRecord* master_track, const il::assetRecord* ai_asset, const double range, const double range_to_zone)
{
   auto target = WsfBMUtils::GetPlatformFromUnitIDRecord(mSimulationPtr, master_track->getTargetTruthID());
   double targetLLA[3] = { 0 };
   if (target)
   {
      target->GetLocationLLA(targetLLA[0], targetLLA[1], targetLLA[2]);
   }

   HCL_INFO_LOGGER(mGlobalLogger) << "DREVAL,AI_"
                     << behavior << ","
                     << curr_time << ","
                     << ai_asset->getName() << ","
                     << ai_asset->getID() << ","
                     << ai_asset->getSide() << ","
                     << ai_asset->getPosition().getLatDegs() << ","
                     << ai_asset->getPosition().getLonDegs() << ","
                     << ai_asset->getPosition().getAltM() << ","
                     << master_track->getTargetTruthName() << ","
                     << master_track->getTargetTruthID() << ","
                     << master_track->getTargetTruthSide() << ","
                     << targetLLA[0] << ","
                     << targetLLA[1] << ","
                     << targetLLA[2] << ","
                     << range << ","
                     << range_to_zone << ","
                     << master_track->getID();

   OnAIAddBehavior(curr_time, behavior, master_track, ai_asset, range, range_to_zone);
}

void WsfBMHEFIRMOELogger::AITakeAction(const double curr_time, const il::trackRecord* master_track, const il::assetRecord* ai_asset, const std::string& action_type, const double end_time_s, const double range, const double range_to_zone)
{
   auto target = WsfBMUtils::GetPlatformFromUnitIDRecord(mSimulationPtr, master_track->getTargetTruthID());
   double targetLLA[3] = { 0 };
   if (target)
   {
      target->GetLocationLLA(targetLLA[0], targetLLA[1], targetLLA[2]);
   }

   HCL_INFO_LOGGER(mGlobalLogger) << "DREVAL,AI_TAKE_ACTION" << ","
                     << curr_time << ","
                     << ai_asset->getName() << ","
                     << ai_asset->getID() << ","
                     << ai_asset->getSide() << ","
                     << ai_asset->getPosition().getLatDegs() << ","
                     << ai_asset->getPosition().getLonDegs() << ","
                     << ai_asset->getPosition().getAltM() << ","
                     << master_track->getTargetTruthName() << ","
                     << master_track->getTargetTruthID() << ","
                     << master_track->getTargetTruthSide() << ","
                     << targetLLA[0] << ","
                     << targetLLA[1] << ","
                     << targetLLA[2] << ","
                     << action_type << ","
                     << end_time_s << ","
                     << range << ","
                     << range_to_zone << ","
                     << master_track->getID();

   OnAITakeAction(curr_time, master_track, ai_asset, action_type, end_time_s, range, range_to_zone);
}

void WsfBMHEFIRMOELogger::AIEgress(const double curr_time, const il::assetRecord* ai_asset, const std::string& egress_method, const std::string& corridor_name, const double speed)
{
   HCL_INFO_LOGGER(mGlobalLogger) << "DREVAL,AI_EGRESS" << ","
                     << curr_time << ","
                     << ai_asset->getName() << ","
                     << ai_asset->getID() << ","
                     << ai_asset->getSide() << ","
                     << ai_asset->getPosition().getLatDegs() << ","
                     << ai_asset->getPosition().getLonDegs() << ","
                     << ai_asset->getPosition().getAltM() << ","
                     << egress_method << ","
                     << corridor_name << ","
                     << speed;

   OnAIEgress(curr_time, ai_asset, egress_method, corridor_name, speed);
}

// =================================================================================================
//! Add a new events to the script observer that maps our new WsfBMHEFIRMOELogger to a script function signature.
void WsfBMHEFIRMOELogger::RegisterScriptObservers(WsfSimulation& aSimulation)
{
   auto scriptObserver = WsfScriptObserver::Find(aSimulation);
   if (scriptObserver)
   {
      auto onAssignmentScriptPacker = [this](UtScriptDataPacker& scriptArgs,
                                             const il::evaluationAssignment* assignment,
                                             const std::string assignment_flag)
      {
         scriptArgs << mSimulationPtr->GetSimTime();
         scriptArgs.Push(new WsfBMEvalAssignment(*assignment, mGlobalLogger), "WsfBMEvalAssignment", UtScriptRef::cMANAGE);
         scriptArgs << assignment_flag;
      };

      scriptObserver->AddEvent<true>("BM_ASSIGNMENT_CREATION", OnAssignment, "BMAssignmentCreation", 
                                     "double, WsfBMEvalAssignment, string", onAssignmentScriptPacker);

      auto onAssignmentStatusScriptPacker = [this](UtScriptDataPacker& scriptArgs,
                                                   const il::assignmentACKMessage* status,
                                                   const il::evaluationAssignment* related_assignment)
      {
         scriptArgs << mSimulationPtr->GetSimTime();
         scriptArgs.Push(status, "WsfBMAssignmentStatusMessage");
         scriptArgs.Push(related_assignment, "WsfBMEvalAssignment");
      };

      scriptObserver->AddEvent<true>("BM_ASSIGNMENT_STATUS", OnAssignmentStatus, "BMAssignmentStatus",
                                     "double, WsfBMAssignmentStatusMessage, WsfBMEvalAssignment", onAssignmentStatusScriptPacker);

      auto onAssignmentCancelScriptPacker = [this](UtScriptDataPacker& scriptArgs,
                                                   const il::idRecord* cancelling_unit,
                                                   const il::evaluationAssignment* active_assignment,
                                                   const std::string& cancel_reason)
      {
         scriptArgs << mSimulationPtr->GetSimTime();
         // TODO replace GetPlatformByName (a map lookup) with an index lookup on platform ID
         WsfPlatform* master_platform = mSimulationPtr->GetPlatformByName(cancelling_unit->getIDString());
         scriptArgs.Push(master_platform, "WsfPlatform");
         scriptArgs.Push(&active_assignment, "WsfBMEvalAssignment");
         scriptArgs << cancel_reason;
      };

      scriptObserver->AddEvent<true>("BM_ASSIGNMENT_CANCEL", OnAssignmentCancel, "BMAssignmentCancel",
                                     "double, WsfPlatform, WsfBMEvalAssignment, string", onAssignmentCancelScriptPacker);

      auto onCANTCOAssignmentScriptPacker = [this](UtScriptDataPacker& scriptArgs,
                                                   const il::idRecord* responding_unit,
                                                   const il::assignmentMessage* assignment,
                                                   const il::trackRecord* opt_track,
                                                   const std::string& reason)
      {
         scriptArgs << mSimulationPtr->GetSimTime();
         // TODO replace GetPlatformByName (a map lookup) with an index lookup on platform ID
         WsfPlatform* master_platform = mSimulationPtr->GetPlatformByName(responding_unit->getIDString());
         scriptArgs.Push(master_platform, "WsfPlatform");
         scriptArgs.Push(&assignment, "WsfBMAssignmentMessage");
         // TODO replace GetPlatformByName (a map lookup) with an index lookup on platform ID
         WsfPlatform* track_platform = mSimulationPtr->GetPlatformByName(opt_track->getID().getIDString());
         WsfLocalTrack* track = track_platform->GetMasterTrackList().FindTrack(WsfBMUtils::ConvertTrackId(opt_track->getID()));
         scriptArgs.Push(track, "WsfLocalTrack");
         scriptArgs << reason;
      };

      scriptObserver->AddEvent<true>("BM_CANTCO_ASSIGNMENT", OnCANTCOAssignment, "BMCantcoAssignment",
                                     "double, WsfPlatform, WsfBMAssignmentMessage, WsfLocalTrack, string", onCANTCOAssignmentScriptPacker);

      auto onAllocateScriptPacker = [this](UtScriptDataPacker& scriptArgs,
                                           const il::idRecord* allocating_unit,
                                           const il::assessmentRecord* assessment)
      {
         scriptArgs << mSimulationPtr->GetSimTime();
         WsfPlatform* platform = WsfBMUtils::GetPlatformFromUnitIDRecord(mSimulationPtr, *allocating_unit);
         scriptArgs.Push(&platform, "WsfPlatform");
         scriptArgs.Push(&assessment, "WsfBMAssessmentRecord");
      };

      scriptObserver->AddEvent<true>("BM_ALLOCATE", OnAllocate, "BMAllocate",
                                     "double, WsfPlatform, WsfBMAssessmentRecord", onAllocateScriptPacker);

      auto onSensorCueScriptPacker = [this](UtScriptDataPacker& scriptArgs,
                                            const il::assetRecord* curr_unit,
                                            const il::trackRecord* master_track,
                                            const il::assignmentMessage* assignment,
                                            const il::sensorRecord* sensor_to_assign)
      {
         scriptArgs << mSimulationPtr->GetSimTime();
         scriptArgs.Push(curr_unit, "WsfBMAssetRecord");
         // TODO replace GetPlatformByName (a map lookup) with an index lookup on platform ID
         WsfPlatform* master_platform = mSimulationPtr->GetPlatformByName(master_track->getID().getIDString());
         WsfLocalTrack* track = master_platform->GetMasterTrackList().FindTrack(WsfBMUtils::ConvertTrackId(master_track->getID()));
         scriptArgs.Push(track, "WsfLocalTrack");
         scriptArgs.Push(assignment, "WsfBMAssignmentMessage");
         // TODO replace GetPlatformByName (a map lookup) with an index lookup on platform ID
         WsfPlatform* platform = mSimulationPtr->GetPlatformByName(sensor_to_assign->GetParentAssetID().getIDString());
         WsfSensor* sensor = WsfBMUtils::GetSensor(platform, sensor_to_assign->GetSensorID().getID());
         scriptArgs.Push(sensor, "WsfSensor");
      };

      scriptObserver->AddEvent<true>("BM_SENSOR_CUE", OnSensorCue, "BMSensorCue",
                                     "double, WsfBMAssetRecord, WsfLocalTrack, WsfBMAssignmentMessage, WsfSensor", onSensorCueScriptPacker);

      scriptObserver->AddEvent<true>("BM_SENSOR_DROP_CUE", OnSensorDropCue, "BMSensorDropCue",
                                     "double, WsfBMAssetRecord, WsfLocalTrack, WsfBMAssignmentMessage, WsfSensor", onSensorCueScriptPacker);


      auto onSensorTrackingScriptPacker = [this](UtScriptDataPacker& scriptArgs,
                                                 const il::trackRecord* master_track,
                                                 const il::sensorRecord* assigned_sensor)
      {
         scriptArgs << mSimulationPtr->GetSimTime();
         // TODO replace GetPlatformByName (a map lookup) with an index lookup on platform ID
         WsfPlatform* master_platform = mSimulationPtr->GetPlatformByName(master_track->getID().getIDString());
         WsfLocalTrack* track = master_platform->GetMasterTrackList().FindTrack(WsfBMUtils::ConvertTrackId(master_track->getID()));
         scriptArgs.Push(track, "WsfLocalTrack");
         // TODO replace GetPlatformByName (a map lookup) with an index lookup on platform ID
         WsfPlatform* platform = mSimulationPtr->GetPlatformByName(assigned_sensor->GetParentAssetID().getIDString());
         WsfSensor* sensor = WsfBMUtils::GetSensor(platform, assigned_sensor->GetSensorID().getID());
         scriptArgs.Push(sensor, "WsfSensor");
      };

      scriptObserver->AddEvent<true>("BM_SENSOR_TRACKING", OnSensorTracking, "BMSensorTracking",
                                     "double, WsfLocalTrack, WsfSensor", onSensorTrackingScriptPacker);

      auto onAIAddBehaviorScriptPacker = [this](UtScriptDataPacker& scriptArgs,
                                                const std::string behavior,
                                                const il::trackRecord* target,
                                                const il::assetRecord* ai_asset,
                                                const double range,
                                                const double range_to_zone)
      {
         scriptArgs << mSimulationPtr->GetSimTime() << behavior;
         // TODO replace GetPlatformByName (a map lookup) with an index lookup on platform ID
         WsfPlatform* master_platform = mSimulationPtr->GetPlatformByName(target->getID().getIDString());
         WsfLocalTrack* track = master_platform->GetMasterTrackList().FindTrack(WsfBMUtils::ConvertTrackId(target->getID()));
         scriptArgs.Push(track, "WsfLocalTrack");
         scriptArgs.Push(&ai_asset, "WsfBMAssetRecord");
         scriptArgs << range << range_to_zone;
      };

      scriptObserver->AddEvent<true>("BM_AI_ADD_BEHAVIOR", OnAIAddBehavior, "BMAiAddBehavior",
                                     "double, string, WsfLocalTrack, WsfBMAssetRecord, double, double", onAIAddBehaviorScriptPacker);


      auto onAITakeActionScriptPacker = [this](UtScriptDataPacker& scriptArgs,
                                               const il::trackRecord* aTrackRecordPtr,
                                               const il::assetRecord* aAssetRecordPtr,
                                               const std::string& actionType,
                                               const double end_time_s,
                                               const double range,
                                               const double range_to_zone)
      {
         scriptArgs << mSimulationPtr->GetSimTime();
         WsfPlatform* platform = WsfBMUtils::GetPlatformFromUnitIDRecord(mSimulationPtr, aTrackRecordPtr->getSendingSystemID());
         WsfTrackId trackID = WsfBMUtils::ConvertTrackId(aTrackRecordPtr->getID());
         WsfLocalTrack* track = WsfBMUtils::FindCorrelatedTrack(&platform->GetTrackManager(), &trackID);
         scriptArgs.Push(&track, "WsfLocalTrack");
         scriptArgs.Push(&aAssetRecordPtr, "WsfBMAssetRecord");
         scriptArgs << actionType << end_time_s << range << range_to_zone;
      };

      scriptObserver->AddEvent<true>("BM_AI_TAKE_ACTION", OnAITakeAction, "BMAiTakeAction",
                                     "double, WsfLocalTrack, WsfBMAssetRecord, string, double, double, double", onAITakeActionScriptPacker);

      auto onAIEgressScriptPacker = [this](UtScriptDataPacker& scriptArgs,
                                           const il::assetRecord* aAssetRecordPtr,
                                           const std::string& aEgressMethod,
                                           const std::string& aCorridor,
                                           const double aSpeed)
      {
         scriptArgs << mSimulationPtr->GetSimTime();
         scriptArgs.Push(&aAssetRecordPtr, "WsfBMAssetRecord");
         scriptArgs << aEgressMethod << aCorridor << aSpeed;
      };

      scriptObserver->AddEvent<true>("BM_AI_EGRESS", OnAIEgress, "BMAiEgress",
                                     "double, WsfBMAssetRecord, string, string, double", onAIEgressScriptPacker);
   }
}

IADSC2SimulationExtension::IADSC2SimulationExtension() :
   WsfSimulationExtension(),
   mMoeLogger(nullptr)
{}

//virtual
void IADSC2SimulationExtension::AddedToSimulation()
{
   mMoeLogger = ut::make_unique<WsfBMHEFIRMOELogger>(&GetSimulation());
   dynamic_cast<WsfBMHEFIRMOELogger*>(mMoeLogger.get())->RegisterScriptObservers(GetSimulation());
}

void IADSC2SimulationExtension::SetLogger(std::unique_ptr<il::moeLoggingInterface> aMoeLogger)
{
   mMoeLogger = std::move(aMoeLogger);
}

il::moeLoggingInterface* IADSC2SimulationExtension::GetLogger() const
{
   return mMoeLogger.get();
}

//static
IADSC2SimulationExtension* IADSC2SimulationExtension::Find(const WsfSimulation& aSimulation)
{
   return static_cast<IADSC2SimulationExtension*>(aSimulation.FindExtension("wsf_iads_c2"));
}
