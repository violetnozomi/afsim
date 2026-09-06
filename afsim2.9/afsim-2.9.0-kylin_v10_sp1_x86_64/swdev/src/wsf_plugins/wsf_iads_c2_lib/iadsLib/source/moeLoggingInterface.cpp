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
*   File: moeLoggingInterface.cpp
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

#include <logger.hpp>

#include <iadsLib/moeLoggingInterface.hpp>
#include <iadsLib/assignmentContainers.hpp>
#include <iadsLib/assignmentACKMessage.hpp>
#include <iadsLib/sensorRecordInterface.hpp>
#include <iadsLib/assetRecord.hpp>

namespace il
{

moeLoggingInterface::moeLoggingInterface(std::weak_ptr<logger::GlobalLogger> aGlobalLogger) :
   mGlobalLogger(aGlobalLogger)
{
   
}

moeLoggingInterface::~moeLoggingInterface()
{

}

void moeLoggingInterface::Assignment(const double curr_time, const evaluationAssignment *assignment, const std::string assignment_flag)
{
   HCL_FATAL_LOGGER(mGlobalLogger) << "moeLoggingInterface::Assignment() unimplemented in base class -- override for your sim framework";
}

void moeLoggingInterface::AssignmentStatus (const double curr_time, const assignmentACKMessage *status, const evaluationAssignment *related_assignment)
{
   HCL_FATAL_LOGGER(mGlobalLogger) << "moeLoggingInterface::AssignmentStatus() unimplemented in base class -- override for your sim framework";
}

void moeLoggingInterface::AssignmentCancel (const double curr_time, const idRecord *cancelling_unit, const il::evaluationAssignment *active_assignment, const std::string &cancel_reason)
{
   HCL_FATAL_LOGGER(mGlobalLogger) << "moeLoggingInterface::AssignmentCancel() unimplemented in base class -- override for your sim framework";
}

void moeLoggingInterface::CANTCOAssignment (const double curr_time, const idRecord *responding_unit, const assignmentMessage *assignment, const trackRecord *opt_track, const std::string &reason)
{
   HCL_FATAL_LOGGER(mGlobalLogger) << "moeLoggingInterface::CANTCO() unimplemented in base class -- override for your sim framework";
}

void moeLoggingInterface::WILCOAssignment (const double curr_time, const il::assignmentACKMessage *status, const il::evaluationAssignment *related_assignment)
{
   HCL_FATAL_LOGGER(mGlobalLogger) << "moeLoggingInterface::WILCO() unimplemented in base class -- override for your sim framework";
}

void moeLoggingInterface::HAVCOAssignment (const double curr_time, const il::assignmentACKMessage *status, const il::evaluationAssignment *related_assignment)
{
   HCL_FATAL_LOGGER(mGlobalLogger) << "moeLoggingInterface::HAVCOAssignment() unimplemented in base class -- override for your sim framework";
}

void moeLoggingInterface::Allocate (const double curr_time, const il::idRecord *allocating_unit, const assessmentRecord *assessment)
{
   HCL_FATAL_LOGGER(mGlobalLogger) << "moeLoggingInterface::Allocate() unimplemented in base class -- override for your sim framework";
}

void moeLoggingInterface::SensorCue (const double curr_time, const il::assetRecord *curr_unit, const trackRecord *master_track, const assignmentMessage *assignment, const sensorRecord *sensor_to_assign)
{
   HCL_FATAL_LOGGER(mGlobalLogger) << "moeLoggingInterface::SensorCue() unimplemented in base class -- override for your sim framework";
}

void moeLoggingInterface::SensorDropCue (const double curr_time, const il::assetRecord *curr_unit, const trackRecord *master_track, const assignmentMessage *assignment, const sensorRecord *sensor_to_assign)
{
   HCL_FATAL_LOGGER(mGlobalLogger) << "moeLoggingInterface::SensorDropCue() unimplemented in base class -- override for your sim framework";
}

void moeLoggingInterface::SensorTracking (const double curr_time, const il::trackRecord *master_track, const il::sensorRecord *assigned_sensor)
{
   HCL_FATAL_LOGGER(mGlobalLogger) << "moeLoggingInterface::SensorTracking() unimplemented in base class -- override for your sim framework";
}

void moeLoggingInterface::LogExtension1 (const double curr_time, const void *param1, const void *param2)
{
   HCL_FATAL_LOGGER(mGlobalLogger) << "moeLoggingInterface::LogExtension1() unimplemented in base class -- override for your sim framework";
}

void moeLoggingInterface::SensorSearch (const double curr_time, const il::idRecord *sensor, const il::evaluationAssignment *assignment)
{
	HCL_FATAL_LOGGER(mGlobalLogger) << "moeLoggingInterface::SensorSearch() unimplemented in base class -- override for your sim framework";
}

void moeLoggingInterface::AIAddBehavior(const double curr_time, const std::string behavior, const il::trackRecord *track, const il::assetRecord *ai_asset, const double range, const double range_to_zone)
{
   HCL_FATAL_LOGGER(mGlobalLogger) << "moeLoggingInterface::AIAddBehavior() unimplemented in base class -- override for your sim framework";
}

void moeLoggingInterface::AITakeAction(const double curr_time, const il::trackRecord *track, const il::assetRecord *ai_asset, const std::string &action_type, const double end_time_s, const double range, const double range_to_zone)
{
   HCL_FATAL_LOGGER(mGlobalLogger) << "moeLoggingInterface::AITakeAction() unimplemented in base class -- override for your sim framework";
}

void moeLoggingInterface::AIEgress(const double curr_time, const il::assetRecord *ai_asset, const std::string &egress_method, const std::string &corridor_name, const double speed)
{
   HCL_FATAL_LOGGER(mGlobalLogger) << "moeLoggingInterface::AIEgress() unimplemented in base class -- override for your sim framework";
}

} // namespace il
