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
*   File: WsfBMEventOutput.cpp
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

#include "WsfBMEventOutput.hpp"

#include <iomanip>
#include <iostream>

#include <WsfBMEventResults.hpp>
#include <WsfBMMOELogger.hpp>
#include <iadsLib/moeLoggingInterface.hpp>
#include <WsfBMStatusMessage.hpp>
#include <WsfBMUtils.hpp>

#include <iadsLib/assetManagerInterface.hpp>
#include <iadsLib/assignmentContainers.hpp>
#include <iadsLib/util.hpp>

#include "WsfEventOutput.hpp"
#include "WsfEventUtils.hpp"
#include "WsfMessage.hpp"
#include "WsfMil.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationObserver.hpp"
#include "WsfWeapon.hpp"

#include "WsfIADSC2ScenarioExtension.hpp"

// headers for overriding message received
//#include <WsfCommObserver.hpp> // defines WsfObserver::MessageReceived

WsfBMEventOutput::WsfBMEventOutput(WsfEventOutput&      aEventOutput,
                                   WsfBMHEFIRMOELogger* aMOE_Logger)
   : mEventOutput(&aEventOutput)
{
   // create flag for new event_output type
   mEventOutput->AddEvent("BM_ASSIGNMENT_CREATION", aMOE_Logger->OnAssignment.Connect(&WsfBMEventOutput::BMAssignment, this));
   mEventOutput->AddEvent("BM_ASSIGNMENT_STATUS", aMOE_Logger->OnAssignmentStatus.Connect(&WsfBMEventOutput::AssignmentStatus, this));
   mEventOutput->AddEvent("BM_ASSIGNMENT_CANCEL", aMOE_Logger->OnAssignmentCancel.Connect(&WsfBMEventOutput::AssignmentCancel, this));
   mEventOutput->AddEvent("BM_CANTCO_ASSIGNMENT", aMOE_Logger->OnCANTCOAssignment.Connect(&WsfBMEventOutput::CANTCOAssignment, this));
   mEventOutput->AddEvent("BM_ALLOCATE", aMOE_Logger->OnAllocate.Connect(&WsfBMEventOutput::Allocate, this));

   mEventOutput->AddEvent("BM_SENSOR_CUE", aMOE_Logger->OnSensorCue.Connect(&WsfBMEventOutput::SensorCue, this));
   mEventOutput->AddEvent("BM_SENSOR_DROP_CUE", aMOE_Logger->OnSensorDropCue.Connect(&WsfBMEventOutput::SensorDropCue, this));
   mEventOutput->AddEvent("BM_SENSOR_TRACKING", aMOE_Logger->OnSensorTracking.Connect(&WsfBMEventOutput::SensorTracking, this));
   mEventOutput->AddEvent("BM_AI_ADD_BEHAVIOR", aMOE_Logger->OnAIAddBehavior.Connect(&WsfBMEventOutput::AIAddBehavior, this));
   mEventOutput->AddEvent("BM_AI_TAKE_ACTION", aMOE_Logger->OnAITakeAction.Connect(&WsfBMEventOutput::AITakeAction, this));
   mEventOutput->AddEvent("BM_AI_EGRESS", aMOE_Logger->OnAIEgress.Connect(&WsfBMEventOutput::AIEgress, this));

   // override WsfEventOutput built in callback for MESSAGE_RECEIVED (example)
   //mEventOutput->AddEvent("MESSAGE_RECEIVED", WsfObserver::MessageReceived(&aSimulation).Connect(&WsfBMEventOutput::MessageReceived, this));
   //mEventOutput->AddEvent("LOCAL_TRACK_INITIATED", WsfObserver::LocalTrackInitiated(&aSimulation).Connect(&WsfBMEventOutput::LocalTrackInitiated, this));

   // Register custom message type. This is for MESSAGE_X events only.
   mEventOutput->RegisterMessagePrinter("WsfBMStatusMessage", std::bind(&WsfBMEventOutput::PrintBMStatusMessage, this, std::placeholders::_1, std::placeholders::_2));
}

// Add custom details to messages of type WSF_BM_STATUS_MESSAGE. Generic info already displayed.
void WsfBMEventOutput::PrintBMStatusMessage(double aSimTime, const WsfMessage& aMessage)
{
   const WsfBMStatusMessage& message = reinterpret_cast<const WsfBMStatusMessage&>(aMessage);
   mEventOutput->StreamRef() << wsf::event::utils::ContinueChar(mEventOutput->GetSettings().PrintSingleLinePerEvent()) << "  Status: ";
   switch (message.GetSystemStatus())
   {
      case il::STATUS_WHITE:
         mEventOutput->StreamRef() << "E_SYS_WHITE";
         break;
      case il::STATUS_YELLOW:
         mEventOutput->StreamRef() << "E_SYS_YELLOW";
         break;
      case il::STATUS_GREEN:
         mEventOutput->StreamRef() << "E_SYS_GREEN";
         break;
      case il::STATUS_RED:
         mEventOutput->StreamRef() << "E_SYS_RED";
         break;
      default:
         mEventOutput->StreamRef() << "UNKNOWN";
         break;
   }
}

// BM_ASSIGNMENT_CREATION callback. EventEntry, PrintTime, EventExit will handle mutex and timestamp output. The rest is up to
//	us. StreamRef gets us access to the .evt file.
void WsfBMEventOutput::BMAssignment(const double curr_time, const il::evaluationAssignment* assignment, const std::string assignment_flag)
{
   WsfSimulation* m_sim_ptr = &mEventOutput->GetSimulation();

   // we really only use truth here where logging needs it (not provided by the internal model datastructures or would require
   // a lookup on the internal model databases)
   auto assigning_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(m_sim_ptr, assignment->Assignment()->getAssigningID());
   auto assigned_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(m_sim_ptr, assignment->Assignment()->getAssignedID().getID());
   WsfWeapon* assigned_wpn = nullptr;
   if (assigned_plat)
   {
      assigned_wpn = assigned_plat->GetComponentEntry<WsfWeapon>(assignment->Assignment()->getAssignedID().getSubID());
   }

   auto target_plat = m_sim_ptr->GetPlatformByName(WsfStringInt(WsfBMUtils::GetStringId(assignment->Track()->getTargetTruthName())));

   // we use truth logging to log the targets actual location
   double target_lla[3] = { 0 };
   if (target_plat)
   {
      target_plat->GetLocationLLA(target_lla[0], target_lla[1], target_lla[2]);
   }

   WsfEventOutput::EventGuard guard(*mEventOutput);
   wsf::event::BMAssignment wsfbmevent(curr_time,
                                       assignment,
                                       assignment_flag,
                                       assigning_plat,
                                       assigned_plat,
                                       target_plat,
                                       assigned_wpn,
                                       target_lla,
                                       mEventOutput->GetSettings());
   wsfbmevent.Print(mEventOutput->StreamRef());
}

void WsfBMEventOutput::AssignmentStatus(const double curr_time, const il::assignmentACKMessage* status, const il::evaluationAssignment* related_assignment)
{
   WsfSimulation* m_sim_ptr = &mEventOutput->GetSimulation();
   // this is not actually a HEFIR message since the statuses are expected ot be logged by the issuer of said statuses. But since
   // we're operating with a set of models that we don't necessarily control, do our own logging when we receive these just so
   // we can get better reporting back to the analyst
   std::string status_type = il::AssignmentStatusToString(status->getStatus());

   // we really only use truth here where logging needs it (not provided by the internal model datastructures or would require
   // a lookup on the internal model databases)
   auto assigning_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(m_sim_ptr, related_assignment->Assignment()->getAssigningID());
   auto assigned_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(m_sim_ptr, related_assignment->Assignment()->getAssignedID().getID());
   WsfWeapon* assigned_wpn = nullptr;
   if (assigned_plat)
   {
      assigned_wpn = assigned_plat->GetComponentEntry<WsfWeapon>(related_assignment->Assignment()->getAssignedID().getSubID());
   }
   auto target_plat = m_sim_ptr->GetPlatformByName(WsfStringInt(WsfBMUtils::GetStringId(related_assignment->Track()->getTargetTruthName())));

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

   WsfEventOutput::EventGuard guard(*mEventOutput);
   wsf::event::AssignmentStatus wsfbmevent(curr_time,
                                           status,
                                           related_assignment,
                                           status_type,
                                           assigning_plat,
                                           assigned_plat,
                                           target_plat,
                                           assigned_wpn,
                                           systemic_scope_str,
                                           mEventOutput->GetSimulation(),
                                           mEventOutput->GetSettings());
   wsfbmevent.Print(mEventOutput->StreamRef());
}

void WsfBMEventOutput::AssignmentCancel(const double curr_time, const il::idRecord* cancelling_unit, const il::evaluationAssignment* active_assignment, const std::string& cancel_reason)
{
   WsfSimulation* m_sim_ptr = &mEventOutput->GetSimulation();
   auto cancelling_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(m_sim_ptr, *cancelling_unit);
   auto assigning_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(m_sim_ptr, active_assignment->Assignment()->getAssigningID());
   auto assigned_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(m_sim_ptr, active_assignment->Assignment()->getAssignedID().getID());
   WsfWeapon* assigned_wpn = nullptr;
   if (assigned_plat)
   {
      assigned_wpn = assigned_plat->GetComponentEntry<WsfWeapon>(active_assignment->Assignment()->getAssignedID().getSubID());
   }
   auto target_plat = m_sim_ptr->GetPlatformByName(WsfStringInt(WsfBMUtils::GetStringId(active_assignment->Track()->getTargetTruthName())));

   WsfEventOutput::EventGuard guard(*mEventOutput);
   wsf::event::AssignmentCancel wsfbmevent(curr_time,
                                           cancelling_unit,
                                           active_assignment,
                                           cancel_reason,
                                           cancelling_plat,
                                           assigning_plat,
                                           assigned_plat,
                                           target_plat,
                                           assigned_wpn,
                                           mEventOutput->GetSettings());
   wsfbmevent.Print(mEventOutput->StreamRef());
}

void WsfBMEventOutput::CANTCOAssignment(const double curr_time, const il::idRecord* responding_unit, const il::assignmentMessage* assignment, const il::trackRecord* opt_track, const std::string& reason)
{
   WsfSimulation* m_sim_ptr = &mEventOutput->GetSimulation();
   auto reporting_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(m_sim_ptr, *responding_unit);
   auto assigning_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(m_sim_ptr, assignment->getAssigningID());
   auto assigned_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(m_sim_ptr, assignment->getAssignedID().getID());
   WsfWeapon* assigned_wpn = nullptr;
   if (assigned_plat)
   {
      assigned_wpn = assigned_plat->GetComponentEntry<WsfWeapon>(assignment->getAssignedID().getSubID());
   }
   auto target_plat = opt_track ? m_sim_ptr->GetPlatformByName(WsfStringInt(WsfBMUtils::GetStringId(opt_track->getTargetTruthName()))) : nullptr;

   WsfEventOutput::EventGuard guard(*mEventOutput);
   wsf::event::CANTCOAssignment wsfbmevent(curr_time,
                                           responding_unit,
                                           assignment,
                                           opt_track,
                                           reason,
                                           assigning_plat,
                                           assigned_plat,
                                           target_plat,
                                           reporting_plat,
                                           assigned_wpn,
                                           mEventOutput->GetSettings());
   wsfbmevent.Print(mEventOutput->StreamRef());
}

void WsfBMEventOutput::Allocate(const double curr_time, const il::idRecord* allocating_unit, const il::assessmentRecord* assessment)
{
   WsfSimulation* m_sim_ptr = &mEventOutput->GetSimulation();
   auto allocating_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(m_sim_ptr, *allocating_unit);
   auto target_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(m_sim_ptr, assessment->getAssessedThreatRecord()->getTrack()->getTargetTruthID());
   auto globalLogger = IADSC2ScenarioExtension::GetGlobalLogger(m_sim_ptr->GetScenario());

   std::string zone_types;
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_OTHER) ? il::zoneTypeToName(il::E_ZONE_TYPE_OTHER, globalLogger) : "";
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_AOR) ? "," + il::zoneTypeToName(il::E_ZONE_TYPE_AOR, globalLogger) : "";
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_SUR) ? "," + il::zoneTypeToName(il::E_ZONE_TYPE_SUR, globalLogger) : "";
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_MEZ) ? "," + il::zoneTypeToName(il::E_ZONE_TYPE_MEZ, globalLogger) : "";
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_FEZ) ? "," + il::zoneTypeToName(il::E_ZONE_TYPE_FEZ, globalLogger) : "";
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_JEZ) ? "," + il::zoneTypeToName(il::E_ZONE_TYPE_JEZ, globalLogger) : "";
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_GEZ) ? "," + il::zoneTypeToName(il::E_ZONE_TYPE_GEZ, globalLogger) : "";
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_DA) ? "," + il::zoneTypeToName(il::E_ZONE_TYPE_DA, globalLogger) : "";
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_COR) ? "," + il::zoneTypeToName(il::E_ZONE_TYPE_COR, globalLogger) : "";

   WsfEventOutput::EventGuard guard(*mEventOutput);
   wsf::event::Allocate wsfbmevent(curr_time,
                                   allocating_plat,
                                   assessment,
                                   target_plat,
                                   zone_types,
                                   mEventOutput->GetSettings());
   wsfbmevent.Print(mEventOutput->StreamRef());
}

void WsfBMEventOutput::SensorCue(const double curr_time, const il::assetRecord* curr_unit, const il::trackRecord* master_track, const il::assignmentMessage* assignment, const il::sensorRecord* sensor_to_assign)
{
   WsfSimulation* m_sim_ptr = &mEventOutput->GetSimulation();
   auto target_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(m_sim_ptr, master_track->getTargetTruthID());

   WsfEventOutput::EventGuard guard(*mEventOutput);
   wsf::event::SensorCue wsfbmevent(curr_time,
                                    curr_unit,
                                    master_track,
                                    assignment,
                                    sensor_to_assign,
                                    target_plat,
                                    mEventOutput->GetSimulation(),
                                    mEventOutput->GetSettings());
   wsfbmevent.Print(mEventOutput->StreamRef());
}

void WsfBMEventOutput::SensorDropCue(const double curr_time, const il::assetRecord* curr_unit, const il::trackRecord* master_track, const il::assignmentMessage* assignment, const il::sensorRecord* sensor_to_assign)
{
   WsfSimulation* m_sim_ptr = &mEventOutput->GetSimulation();
   auto target_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(m_sim_ptr, master_track->getTargetTruthID());

   WsfEventOutput::EventGuard guard(*mEventOutput);
   wsf::event::SensorDropCue wsfbmevent(curr_time,
                                        curr_unit,
                                        master_track,
                                        assignment,
                                        sensor_to_assign,
                                        target_plat,
                                        mEventOutput->GetSimulation(),
                                        mEventOutput->GetSettings());
   wsfbmevent.Print(mEventOutput->StreamRef());
}

void WsfBMEventOutput::SensorTracking(const double curr_time, const il::trackRecord* master_track, const il::sensorRecord* assigned_sensor)
{
   WsfSimulation* m_sim_ptr = &mEventOutput->GetSimulation();
   auto target_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(m_sim_ptr, master_track->getTargetTruthID());
   double target_lla[3] = { 0 };
   if (target_plat)
   {
      target_plat->GetLocationLLA(target_lla[0], target_lla[1], target_lla[2]);
   }

   auto tracking_loc = assigned_sensor->GetParentAsset()->getPosition();

   WsfEventOutput::EventGuard guard(*mEventOutput);
   wsf::event::SensorTracking wsfbmevent(curr_time,
                                         master_track,
                                         assigned_sensor,
                                         tracking_loc,
                                         target_plat,
                                         target_lla,
                                         mEventOutput->GetSimulation(),
                                         mEventOutput->GetSettings());
   wsfbmevent.Print(mEventOutput->StreamRef());
}

void WsfBMEventOutput::AIAddBehavior(const double curr_time, const std::string behavior, const il::trackRecord* master_track, const il::assetRecord* ai_asset, const double range, const double range_to_zone)
{
   WsfSimulation* m_sim_ptr = &mEventOutput->GetSimulation();
   auto target = WsfBMUtils::GetPlatformFromUnitIDRecord(m_sim_ptr, master_track->getTargetTruthID());
   auto zones = ai_asset->GetDefendedZones();
   double targetLLA[3] = { 0 };
   if (target)
   {
      target->GetLocationLLA(targetLLA[0], targetLLA[1], targetLLA[2]);
   }

   WsfEventOutput::EventGuard guard(*mEventOutput);
   wsf::event::AIAddBehavior wsfbmevent(curr_time,
                                        behavior,
                                        master_track,
                                        ai_asset,
                                        range,
                                        range_to_zone,
                                        targetLLA,
                                        mEventOutput->GetSettings());
   wsfbmevent.Print(mEventOutput->StreamRef());
}

void WsfBMEventOutput::AITakeAction(const double curr_time, const il::trackRecord* master_track, const il::assetRecord* ai_asset, const std::string& action_type, const double end_time_s, const double range, const double range_to_zone)
{
   WsfSimulation* m_sim_ptr = &mEventOutput->GetSimulation();
   auto target = WsfBMUtils::GetPlatformFromUnitIDRecord(m_sim_ptr, master_track->getTargetTruthID());
   auto zones = ai_asset->GetDefendedZones();
   double targetLLA[3] = { 0 };
   if (target)
   {
      target->GetLocationLLA(targetLLA[0], targetLLA[1], targetLLA[2]);
   }

   WsfEventOutput::EventGuard guard(*mEventOutput);
   wsf::event::AITakeAction wsfbmevent(curr_time,
                                       action_type,
                                       end_time_s,
                                       master_track,
                                       ai_asset,
                                       range,
                                       range_to_zone,
                                       targetLLA,
                                       mEventOutput->GetSettings());
   wsfbmevent.Print(mEventOutput->StreamRef());
}

void WsfBMEventOutput::AIEgress(const double curr_time, const il::assetRecord* ai_asset, const std::string& egress_method, const std::string& corridor_name, const double speed)
{
   WsfEventOutput::EventGuard guard(*mEventOutput);
   wsf::event::AIEgress wsfbmevent(curr_time,
                                   ai_asset,
                                   egress_method,
                                   corridor_name,
                                   mEventOutput->GetSettings());
   wsfbmevent.Print(mEventOutput->StreamRef());
}

// Override of MESSAGE_RECEIVED event. We currently have access to all WsfEventOutupt public methods
//	but not inherited protected methods. May need to extend if that is needed. (i.e. MessageDataTag)
//void WsfBMEventOutput::MessageReceived(double            aSimTime,
//	WsfComm*          aXmtrPtr,
//	WsfComm*          aRcvrPtr,
//	const WsfMessage& aMessage,
//	WsfComm::Result&  aResult)
//{
//	mEventOutput->EventEntry();
//	mEventOutput->PrintTime(aSimTime);
//	mEventOutput->StreamRef() << "MESSAGE_RECEIVED " << "Override Output.";
//	mEventOutput->StreamRef() << "\n";
//	mEventOutput->EventExit();
//}
