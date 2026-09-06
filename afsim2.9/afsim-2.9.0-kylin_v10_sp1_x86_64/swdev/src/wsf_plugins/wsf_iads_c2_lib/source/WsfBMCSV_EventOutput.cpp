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
*   File: WsfBMCSV_EventOutput.cpp
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
*   Abstract:
*
*******************************************************************************/

#include "WsfBMCSV_EventOutput.hpp"

#include <iomanip>
#include <iostream>

#include "UtCallback.hpp"

#include <WsfBMEventResults.hpp>
#include <WsfBMMOELogger.hpp>
#include "WsfBMStatusMessage.hpp"
#include <WsfBMUtils.hpp>

#include <iadsLib/assignmentContainers.hpp>
#include <iadsLib/enum.hpp>
#include <iadsLib/util.hpp>

#include "WsfMessage.hpp"
#include "WsfMil.hpp"
#include "WsfPlatform.hpp"
#include "WsfCSV_EventOutput.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationObserver.hpp"
#include "WsfWeapon.hpp"

#include "WsfIADSC2ScenarioExtension.hpp"


// headers for overriding message received
//#include <WsfCommObserver.hpp> // defines WsfObserver::MessageReceived

WsfBMCSV_EventOutputData::WsfBMCSV_EventOutputData()
{
   WsfCSV_EventOutputData::AddDataTags("BM_ASSIGNMENT_CREATION", { "time<time>", "event<string>", "assigned_platform<string>", "target_platform<string>", "target_platform_side<string>",
                                                                   "assigning_platform<string>", "assigning_platform_side<string>", "assigned_weapon<string>", "shot_doctrine<string>",
                                                                   "reporting_sensor_type<string>", "delegate_or_commanded<string>", "target_lat<lat>", "target_lon<lon>",
                                                                   "target_alt<double>", "assignment_time<time>", "reference_track_ID<int>", "local_track_ID<int>"
                                                                 });
   WsfCSV_EventOutputData::AddDataTags("BM_ASSIGNMENT_STATUS", { "time<time>", "event<string>", "status_type<string>", "assigned_platform<string>", "target_platform<string>",
                                                                 "target_platform_side<string>", "assigning_platform<string>", "assigning_platform_side<string>",
                                                                 "local_track_ID<int>", "assignment_time<time>", "assigned_weapon<string>", "ack_time<time>",
                                                                 "systemic_CANTCO?<string>", "systemic_scope<string>", "assignment_reference_track_ID<int>",
                                                                 "shot_doctrine<string>", "old_shot_doctrine<string>", "old_assigned_weapon<string>", "old_assigned_platform<string>"
                                                               });
   WsfCSV_EventOutputData::AddDataTags("BM_ASSIGNMENT_CANCEL", { "time<time>", "event<string>", "assigned_platform<string>", "target_platform<string>", "target_platform_side<string>",
                                                                 "assigning_platform<string>", "assigning_platform_side<string>", "assigned_weapon<string>",
                                                                 "canceling_platform<string>", "canceling_platform_side<string>",
                                                                 "cancel_reason<string>", "assignment_time<time>", "reference_track_ID<int>", "local_track_ID<int>"
                                                               });
   WsfCSV_EventOutputData::AddDataTags("BM_CANTCO_ASSIGNMENT", { "time<time>", "event<string>", "assignment_platform<string>", "target_platform<string>", "target_platform_side<string>",
                                                                 "assigning_platform<string>", "assigning_platform_side<string>", "assigned_weapon<string>",
                                                                 "reporting_platform<string>", "reporting_platform_side<string>", "reason<string>", "assignment_time<time>",
                                                                 "reference_track_ID<int>", "local_track_ID<int>"
                                                               });
   WsfCSV_EventOutputData::AddDataTags("BM_ALLOCATE", { "time<time>", "event<string>", "allocating_platform<string>", "allocating_platform_side<string>", "target_platform<string>",
                                                        "target_platform_side<string>", "zone_types<string>", "assessed_threat_track_ID<int>"
                                                      });
   WsfCSV_EventOutputData::AddDataTags("BM_SENSOR_CUE", { "time<time>", "event<string>", "sensor_type<string>", "sensor_parent_asset<string>", "parent_asset_side<string>",
                                                          "target_platform<string>", "target_platform_side<string>", "current_unit<string>", "current_unit_side<string>",
                                                          "sensor_to_assign<string>", "master_track_ID<int>", "target_truth_ID<int>"
                                                        });
   WsfCSV_EventOutputData::AddDataTags("BM_SENSOR_DROP_CUE", { "time<time>", "event<string>", "sensor_type<string>", "sensor_parent_asset<string>", "parent_asset_side<string>",
                                                               "target_platform<string>", "target_platform_side<string>", "current_unit<string>", "current_unit_side<string>",
                                                               "sensor_to_assign<string>", "master_track_ID<int>", "target_truth_ID<int>"
                                                             });
   WsfCSV_EventOutputData::AddDataTags("BM_SENSOR_TRACKING", { "time<time>", "event<string>", "sensor_type<string>", "asset_name<string>", "asset_side<string>",
                                                               "target_name<string>", "target_side<string>", "asset_ID<int>", "sensor_ID<int>",
                                                               "tracking_loc_lat<lat>", "tracking_loc_lon<lon>", "tracking_loc-alt<double>",
                                                               "target_lat<lat>", "target_lon<lon>", "target_alt<double>", "local_track_ID<int>", "target_truth_ID<int>"
                                                             });

   // mCSV_EventOutput-PrintLocationDataLLA(tracking_loc.getLatDegs(), tracking_loc.getLonDegs(), tracking_loc.getAltM());
   //mCSV_EventOutput-PrintLocationDataLLA(target_lla[0], target_lla[1], target_lla[2]);

   WsfCSV_EventOutputData::AddDataTags("BM_AI_ADD_BEHAVIOR", { "time<time>", "event<string>", "asset_name<string>", "asset_ID<int>", "asset_side<string>",
                                                               "asset_lat<lat>", "asset_lon<lon>", "asset_alt<double>", "target_truth_name<string>", "target_truth_ID<int>",
                                                               "target_truth_side<string>", "target_lat<lat>", "target_lon<lon>", "target_alt<double>", "range<double>",
                                                               "range_to_zone<double>", "local_track_ID<int>"
                                                             });
   WsfCSV_EventOutputData::AddDataTags("BM_AI_TAKE_ACTION", { "time<time>", "event<string>", "action_type<string>", "asset_name<string>", "asset_ID<string>",
                                                              "asset_side<string>", "asset_lat<lat>", "asset_lon<lon>", "asset_alt<double>", "target_truth_name<string>", "target_truth_ID<string>",
                                                              "target_truth_side<string>", "target_lat<lat>", "target_lon<lon>", "target_alt<double>", "action_end_time<time>",
                                                              "range<double>", "range_to_zone<double>", "local_track_ID<int>"
                                                            });
   WsfCSV_EventOutputData::AddDataTags("BM_AI_EGRESS", { "time<time>", "event<string>", "asset_name<string>", "asset_ID<string>", "asset_side<string>",
                                                         "asset_lat<lat>", "asset_lon<lon>", "asset_alt<double>", "egress_method<string>", "corridor_name<string>"
                                                       });
}

WsfBMCSV_EventOutput::WsfBMCSV_EventOutput(WsfCSV_EventOutput&   aCSV_EventOutput,
                                           WsfBMHEFIRMOELogger* aMOE_Logger)
   : WsfBMCSV_EventOutputData(),
     mCSV_EventOutput(&aCSV_EventOutput)

{
   //WsfSimulation* simPtr = &(mCSV_EventOutput->GetSimulation());

   // create flag for new event_output type. Prints to file named "BM_EVENTS.ber"
   mCSV_EventOutput->AddEvent("BM_ASSIGNMENT_CREATION", aMOE_Logger->OnAssignment.Connect(&WsfBMCSV_EventOutput::BMAssignment, this));
   mCSV_EventOutput->AddEvent("BM_ASSIGNMENT_STATUS", aMOE_Logger->OnAssignmentStatus.Connect(&WsfBMCSV_EventOutput::AssignmentStatus, this));
   mCSV_EventOutput->AddEvent("BM_ASSIGNMENT_CANCEL", aMOE_Logger->OnAssignmentCancel.Connect(&WsfBMCSV_EventOutput::AssignmentCancel, this));
   mCSV_EventOutput->AddEvent("BM_CANTCO_ASSIGNMENT", aMOE_Logger->OnCANTCOAssignment.Connect(&WsfBMCSV_EventOutput::CANTCOAssignment, this));
   mCSV_EventOutput->AddEvent("BM_ALLOCATE", aMOE_Logger->OnAllocate.Connect(&WsfBMCSV_EventOutput::Allocate, this));

   mCSV_EventOutput->AddEvent("BM_SENSOR_CUE", aMOE_Logger->OnSensorCue.Connect(&WsfBMCSV_EventOutput::SensorCue, this));
   mCSV_EventOutput->AddEvent("BM_SENSOR_DROP_CUE", aMOE_Logger->OnSensorDropCue.Connect(&WsfBMCSV_EventOutput::SensorDropCue, this));
   mCSV_EventOutput->AddEvent("BM_SENSOR_TRACKING", aMOE_Logger->OnSensorTracking.Connect(&WsfBMCSV_EventOutput::SensorTracking, this));
   mCSV_EventOutput->AddEvent("BM_AI_ADD_BEHAVIOR", aMOE_Logger->OnAIAddBehavior.Connect(&WsfBMCSV_EventOutput::AIAddBehavior, this));
   mCSV_EventOutput->AddEvent("BM_AI_TAKE_ACTION", aMOE_Logger->OnAITakeAction.Connect(&WsfBMCSV_EventOutput::AITakeAction, this));
   mCSV_EventOutput->AddEvent("BM_AI_EGRESS", aMOE_Logger->OnAIEgress.Connect(&WsfBMCSV_EventOutput::AIEgress, this));

   // override WsfEventOutput built in callback for MESSAGE_RECEIVED (example)
   //mCSV_EventOutput->AddEvent("MESSAGE_RECEIVED"< WsfObserver::MessageReceived(&aSimulation).Connect(&WsfBMCSV_EventOutput::MessageReceived, this));

   // Register custom event type. This is for MESSAGE_X events only.
   mCSV_EventOutput->RegisterMessagePrinter("WsfBMStatusMessage", std::bind(&WsfBMCSV_EventOutput::PrintBMStatusMessage, this, std::placeholders::_1, std::placeholders::_2));
}

// Add custom details to messages of type BMStatusMessage. Generic info already displayed. See WsfEventOutput::PrintMessageData for examples.
void WsfBMCSV_EventOutput::PrintBMStatusMessage(double            aSimTime,
                                                const WsfMessage& aMessage)
{
   const WsfBMStatusMessage& message = reinterpret_cast<const WsfBMStatusMessage&>(aMessage);
   switch (message.GetSystemStatus())
   {
      case il::STATUS_WHITE:
         mCSV_EventOutput->StreamRef() << ',' << "WHITE";
         break;
      case il::STATUS_YELLOW:
         mCSV_EventOutput->StreamRef() << ',' << "YELLOW";
         break;
      case il::STATUS_GREEN:
         mCSV_EventOutput->StreamRef() << ',' << "GREEN";
         break;
      case il::STATUS_RED:
         mCSV_EventOutput->StreamRef() << ',' << "RED";
         break;
      default:
         mCSV_EventOutput->StreamRef() << ',' << "UNKNOWN";
         break;
   }
}

// BM_ASSIGNMENT_CREATION callback. EventEntry, PrintTime, EventExit will handle mutex and timestamp output. The rest is up to
// us. StreamRef gets us access to the a .csv file. GetEventIndex grabs file corresponding to event type.
void WsfBMCSV_EventOutput::BMAssignment(const double                    curr_time,
                                        const il::evaluationAssignment* assignment,
                                        const std::string               assignment_flag)
{
   WsfSimulation* simPtr = &mCSV_EventOutput->GetSimulation();
   bool is_delegate = static_cast<bool>(assignment->DelegateAssignment());

   // we really only use truth here where logging needs it (not provided by the internal model data structures or would require
   // a lookup on the internal model databases)
   auto assigning_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(simPtr, assignment->Assignment()->getAssigningID());
   auto assigned_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(simPtr, assignment->Assignment()->getAssignedID().getID());
   WsfWeapon* assigned_wpn = nullptr;
   if (assigned_plat)
   {
      assigned_wpn = assigned_plat->GetComponentEntry<WsfWeapon>(assignment->Assignment()->getAssignedID().getSubID());
   }

   auto target_plat = simPtr->GetPlatformByName(WsfStringInt(WsfBMUtils::GetStringId(assignment->Track()->getTargetTruthName())));

   // we use truth logging to log the targets actual location
   double target_lla[3] = { 0 };
   if (target_plat)
   {
      target_plat->GetLocationLLA(target_lla[0], target_lla[1], target_lla[2]);
   }

   WsfCSV_EventOutput::EventGuard guard(*mCSV_EventOutput);
   wsf::event::BMAssignment wsfbmevent(curr_time,
                                       assignment,
                                       assignment_flag,
                                       assigning_plat,
                                       assigned_plat,
                                       target_plat,
                                       assigned_wpn,
                                       target_lla,
                                       mCSV_EventOutput->GetSettings(),
                                       is_delegate);
   wsfbmevent.PrintCSV(mCSV_EventOutput->StreamRef());
}

void WsfBMCSV_EventOutput::AssignmentStatus(const double                    curr_time,
                                            const il::assignmentACKMessage* status,
                                            const il::evaluationAssignment* related_assignment)
{
   WsfSimulation* simPtr = &mCSV_EventOutput->GetSimulation();
   // this is not actually a HEFIR message since the statuses are expected ot be logged by the issuer of said statuses. But since
   // we're operating with a set of models that we don't necessarily control, do our own logging when we receive these just so
   // we can get better reporting back to the analyst
   std::string status_type = il::AssignmentStatusToString(status->getStatus());

   // we really only use truth here where logging needs it (not provided by the internal model datastructures or would require
   // a lookup on the internal model databases)
   auto assigning_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(simPtr, related_assignment->Assignment()->getAssigningID());
   auto assigned_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(simPtr, related_assignment->Assignment()->getAssignedID().getID());
   WsfWeapon* assigned_wpn = nullptr;
   if (assigned_plat)
   {
      assigned_wpn = assigned_plat->GetComponentEntry<WsfWeapon>(related_assignment->Assignment()->getAssignedID().getSubID());
   }
   auto target_plat = simPtr->GetPlatformByName(WsfStringInt(WsfBMUtils::GetStringId(related_assignment->Track()->getTargetTruthName())));

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

   WsfCSV_EventOutput::EventGuard guard(*mCSV_EventOutput);
   wsf::event::AssignmentStatus wsfbmevent(curr_time,
                                           status,
                                           related_assignment,
                                           status_type,
                                           assigning_plat,
                                           assigned_plat,
                                           target_plat,
                                           assigned_wpn,
                                           systemic_scope_str,
                                           mCSV_EventOutput->GetSimulation(),
                                           mCSV_EventOutput->GetSettings());
   wsfbmevent.PrintCSV(mCSV_EventOutput->StreamRef());
}

void WsfBMCSV_EventOutput::AssignmentCancel(const double                    curr_time,
                                            const il::idRecord*             cancelling_unit,
                                            const il::evaluationAssignment* active_assignment,
                                            const std::string&              cancel_reason)
{
   WsfSimulation* simPtr = &mCSV_EventOutput->GetSimulation();
   auto cancelling_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(simPtr, *cancelling_unit);
   auto assigning_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(simPtr, active_assignment->Assignment()->getAssigningID());
   auto assigned_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(simPtr, active_assignment->Assignment()->getAssignedID().getID());
   WsfWeapon* assigned_wpn = nullptr;
   if (assigned_plat)
   {
      assigned_wpn = assigned_plat->GetComponentEntry<WsfWeapon>(active_assignment->Assignment()->getAssignedID().getSubID());
   }
   auto target_plat = simPtr->GetPlatformByName(WsfStringInt(WsfBMUtils::GetStringId(active_assignment->Track()->getTargetTruthName())));

   WsfCSV_EventOutput::EventGuard guard(*mCSV_EventOutput);
   wsf::event::AssignmentCancel wsfbmevent(curr_time,
                                           cancelling_unit,
                                           active_assignment,
                                           cancel_reason,
                                           cancelling_plat,
                                           assigning_plat,
                                           assigned_plat,
                                           target_plat,
                                           assigned_wpn,
                                           mCSV_EventOutput->GetSettings());
   wsfbmevent.PrintCSV(mCSV_EventOutput->StreamRef());
}

void WsfBMCSV_EventOutput::CANTCOAssignment(const double                 curr_time,
                                            const il::idRecord*          responding_unit,
                                            const il::assignmentMessage* assignment,
                                            const il::trackRecord*       opt_track,
                                            const std::string&           reason)
{
   WsfSimulation* simPtr = &mCSV_EventOutput->GetSimulation();
   auto reporting_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(simPtr, *responding_unit);
   auto assigning_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(simPtr, assignment->getAssigningID());
   auto assigned_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(simPtr, assignment->getAssignedID().getID());
   WsfWeapon* assigned_wpn = nullptr;
   if (assigned_plat)
   {
      assigned_wpn = assigned_plat->GetComponentEntry<WsfWeapon>(assignment->getAssignedID().getSubID());
   }
   auto target_plat = opt_track ? simPtr->GetPlatformByName(WsfStringInt(WsfBMUtils::GetStringId(opt_track->getTargetTruthName()))) : nullptr;

   WsfCSV_EventOutput::EventGuard guard(*mCSV_EventOutput);
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
                                           mCSV_EventOutput->GetSettings());
   wsfbmevent.PrintCSV(mCSV_EventOutput->StreamRef());
}

void WsfBMCSV_EventOutput::Allocate(const double                curr_time,
                                    const il::idRecord*         allocating_unit,
                                    const il::assessmentRecord* assessment)
{
   WsfSimulation* simPtr = &mCSV_EventOutput->GetSimulation();
   auto allocating_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(simPtr, *allocating_unit);
   auto target_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(simPtr, assessment->getAssessedThreatRecord()->getTrack()->getTargetTruthID());

   auto globalLogger = IADSC2ScenarioExtension::GetGlobalLogger(simPtr->GetScenario());

   std::string zone_types;
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_OTHER) ? il::zoneTypeToName(il::E_ZONE_TYPE_OTHER, globalLogger) : "";
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_AOR) ? ',' + il::zoneTypeToName(il::E_ZONE_TYPE_AOR, globalLogger) : "";
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_SUR) ? ',' + il::zoneTypeToName(il::E_ZONE_TYPE_SUR, globalLogger) : "";
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_MEZ) ? ',' + il::zoneTypeToName(il::E_ZONE_TYPE_MEZ, globalLogger) : "";
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_FEZ) ? ',' + il::zoneTypeToName(il::E_ZONE_TYPE_FEZ, globalLogger) : "";
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_JEZ) ? ',' + il::zoneTypeToName(il::E_ZONE_TYPE_JEZ, globalLogger) : "";
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_GEZ) ? ',' + il::zoneTypeToName(il::E_ZONE_TYPE_GEZ, globalLogger) : "";
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_DA) ? ',' + il::zoneTypeToName(il::E_ZONE_TYPE_DA, globalLogger) : "";
   zone_types += (assessment->GetAssessedZoneTypes() & il::E_ZONE_TYPE_COR) ? ',' + il::zoneTypeToName(il::E_ZONE_TYPE_COR, globalLogger) : "";

   WsfCSV_EventOutput::EventGuard guard(*mCSV_EventOutput);
   wsf::event::Allocate wsfbmevent(curr_time,
                                   allocating_plat,
                                   assessment,
                                   target_plat,
                                   zone_types,
                                   mCSV_EventOutput->GetSettings());
   wsfbmevent.PrintCSV(mCSV_EventOutput->StreamRef());
}

void WsfBMCSV_EventOutput::SensorCue(const double                 curr_time,
                                     const il::assetRecord*       curr_unit,
                                     const il::trackRecord*       master_track,
                                     const il::assignmentMessage* assignment,
                                     const il::sensorRecord*      sensor_to_assign)
{
   WsfSimulation* simPtr = &mCSV_EventOutput->GetSimulation();
   auto target_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(simPtr, master_track->getTargetTruthID());

   WsfCSV_EventOutput::EventGuard guard(*mCSV_EventOutput);
   wsf::event::SensorCue wsfbmevent(curr_time,
                                    curr_unit,
                                    master_track,
                                    assignment,
                                    sensor_to_assign,
                                    target_plat,
                                    mCSV_EventOutput->GetSimulation(),
                                    mCSV_EventOutput->GetSettings());
   wsfbmevent.PrintCSV(mCSV_EventOutput->StreamRef());
}

void WsfBMCSV_EventOutput::SensorDropCue(const double                 curr_time,
                                         const il::assetRecord*       curr_unit,
                                         const il::trackRecord*       master_track,
                                         const il::assignmentMessage* assignment,
                                         const il::sensorRecord*      sensor_to_assign)
{
   WsfSimulation* simPtr = &mCSV_EventOutput->GetSimulation();
   auto target_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(simPtr,
                                                              master_track->getTargetTruthID());

   WsfCSV_EventOutput::EventGuard guard(*mCSV_EventOutput);
   wsf::event::SensorDropCue wsfbmevent(curr_time,
                                        curr_unit,
                                        master_track,
                                        assignment,
                                        sensor_to_assign,
                                        target_plat,
                                        mCSV_EventOutput->GetSimulation(),
                                        mCSV_EventOutput->GetSettings());
   wsfbmevent.PrintCSV(mCSV_EventOutput->StreamRef());
}

void WsfBMCSV_EventOutput::SensorTracking(const double            curr_time,
                                          const il::trackRecord*  master_track,
                                          const il::sensorRecord* assigned_sensor)
{
   WsfSimulation* simPtr = &mCSV_EventOutput->GetSimulation();
   auto target_plat = WsfBMUtils::GetPlatformFromUnitIDRecord(simPtr,
                                                              master_track->getTargetTruthID());
   double target_lla[3] = { 0 };
   if (target_plat)
   {
      target_plat->GetLocationLLA(target_lla[0], target_lla[1], target_lla[2]);
   }

   auto tracking_loc = assigned_sensor->GetParentAsset()->getPosition();

   WsfCSV_EventOutput::EventGuard guard(*mCSV_EventOutput);
   wsf::event::SensorTracking wsfbmevent(curr_time,
                                         master_track,
                                         assigned_sensor,
                                         tracking_loc,
                                         target_plat,
                                         target_lla,
                                         mCSV_EventOutput->GetSimulation(),
                                         mCSV_EventOutput->GetSettings());
   wsfbmevent.PrintCSV(mCSV_EventOutput->StreamRef());
}

void WsfBMCSV_EventOutput::AIAddBehavior(const double           curr_time,
                                         const std::string      behavior,
                                         const il::trackRecord* master_track,
                                         const il::assetRecord* ai_asset,
                                         const double           range,
                                         const double           range_to_zone)
{
   WsfSimulation* simPtr = &mCSV_EventOutput->GetSimulation();
   auto target = WsfBMUtils::GetPlatformFromUnitIDRecord(simPtr,
                                                         master_track->getTargetTruthID());
   auto zones = ai_asset->GetDefendedZones();
   double targetLLA[3] = { 0 };
   if (target)
   {
      target->GetLocationLLA(targetLLA[0], targetLLA[1], targetLLA[2]);
   }

   WsfCSV_EventOutput::EventGuard guard(*mCSV_EventOutput);
   wsf::event::AIAddBehavior wsfbmevent(curr_time,
                                        behavior,
                                        master_track,
                                        ai_asset,
                                        range,
                                        range_to_zone,
                                        targetLLA,
                                        mCSV_EventOutput->GetSettings());
   wsfbmevent.PrintCSV(mCSV_EventOutput->StreamRef());
}

void WsfBMCSV_EventOutput::AITakeAction(const double           curr_time,
                                        const il::trackRecord* master_track,
                                        const il::assetRecord* ai_asset,
                                        const std::string&     action_type,
                                        const double           end_time_s,
                                        const double           range,
                                        const double           range_to_zone)
{
   WsfSimulation* simPtr = &mCSV_EventOutput->GetSimulation();
   auto target = WsfBMUtils::GetPlatformFromUnitIDRecord(simPtr, master_track->getTargetTruthID());
   auto zones = ai_asset->GetDefendedZones();
   double targetLLA[3] = { 0 };
   if (target)
   {
      target->GetLocationLLA(targetLLA[0], targetLLA[1], targetLLA[2]);
   }

   WsfCSV_EventOutput::EventGuard guard(*mCSV_EventOutput);
   wsf::event::AITakeAction wsfbmevent(curr_time,
                                       action_type,
                                       end_time_s,
                                       master_track,
                                       ai_asset,
                                       range,
                                       range_to_zone,
                                       targetLLA,
                                       mCSV_EventOutput->GetSettings());
   wsfbmevent.PrintCSV(mCSV_EventOutput->StreamRef());
}

void WsfBMCSV_EventOutput::AIEgress(const double           curr_time,
                                    const il::assetRecord* ai_asset,
                                    const std::string&     egress_method,
                                    const std::string&     corridor_name,
                                    const double           speed)
{
   WsfCSV_EventOutput::EventGuard guard(*mCSV_EventOutput);
   wsf::event::AIEgress wsfbmevent(curr_time,
                                   ai_asset,
                                   egress_method,
                                   corridor_name,
                                   mCSV_EventOutput->GetSettings());
   wsfbmevent.PrintCSV(mCSV_EventOutput->StreamRef());
}

// Override of MESSAGE_RECEIVED event. We currently have access to all WsfEventOutupt public methods
// but not inherited protected methods. May need to extend if that is needed. (i.e. MessageDataTag)
//void WsfBMCSV_EventOutput::MessageReceived(double            aSimTime,
//                              WsfComm*          aXmtrPtr,
//                              WsfComm*          aRcvrPtr,
//                              const WsfMessage& aMessage,
//                              WsfComm::Result&  aResult)
//{
//   mCSV_EventOutput->EventEntry();
//   mCSV_EventOutput->PrintTime(aSimTime);
//   mCSV_EventOutput->StreamRef() << ',' << "MESSAGE_RECEIVED," << "Override Output.";
//   mCSV_EventOutput->StreamRef() << '\n';
//   mCSV_EventOutput->EventExit();
//}
