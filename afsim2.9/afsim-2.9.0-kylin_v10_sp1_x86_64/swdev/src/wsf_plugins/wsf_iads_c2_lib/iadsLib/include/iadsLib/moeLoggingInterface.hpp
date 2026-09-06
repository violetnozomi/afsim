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
*   File: moeLoggingInterface.hpp
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

#ifndef IADSLIB_MOE_LOGGING_INTERFACE_HPP
#define IADSLIB_MOE_LOGGING_INTERFACE_HPP

#include <string>
#include <memory>

#include "iadslib_export.h"
#include "logger.hpp"

// forward declarations
namespace il
{
   class evaluationAssignment;
   class assignmentACKMessage;
   class idRecord;
   class assignmentMessage;
   class trackRecord;
   class assessmentRecord;
   class sensorRecord;
   class assetRecord;
}

namespace il
{

   class IADSLIB_EXPORT moeLoggingInterface
   {
      public:
         moeLoggingInterface(std::weak_ptr<logger::GlobalLogger> aGlobalLogger);
         virtual ~moeLoggingInterface();
         virtual void Assignment(const double curr_time, const evaluationAssignment* assignment, const std::string assignment_flag);
         virtual void AssignmentStatus(const double curr_time, const assignmentACKMessage* status, const evaluationAssignment* related_assignment);
         virtual void AssignmentCancel(const double curr_time, const idRecord* cancelling_unit, const il::evaluationAssignment* active_assignment, const std::string& cancel_reason);
         virtual void CANTCOAssignment(const double curr_time, const idRecord* responding_unit, const assignmentMessage* assignment, const trackRecord* opt_track, const std::string& reason);
         virtual void WILCOAssignment(const double curr_time, const il::assignmentACKMessage* status, const il::evaluationAssignment* related_assignment);
         virtual void HAVCOAssignment(const double curr_time, const il::assignmentACKMessage* status, const il::evaluationAssignment* related_assignment);
         virtual void Allocate(const double curr_time, const il::idRecord* allocating_unit, const assessmentRecord* assessment);

         virtual void SensorCue(const double curr_time, const il::assetRecord* curr_unit, const trackRecord* master_track, const assignmentMessage* assignment, const sensorRecord* sensor_to_assign);
         virtual void SensorDropCue(const double curr_time, const il::assetRecord* curr_unit, const trackRecord* master_track, const assignmentMessage* assignment, const sensorRecord* sensor_to_assign);
         virtual void SensorTracking(const double curr_time, const il::trackRecord* master_track, const il::sensorRecord* assigned_sensor);
         // log extensions allow other plugins to not divulge the details to the main unclass logger but still provide relativley localized logging interfaces
         virtual void LogExtension1(const double curr_time, const void* param1, const void* param2);

         virtual void SensorSearch(const double curr_time, const il::idRecord* sensor, const il::evaluationAssignment* assignment);

         virtual void AIAddBehavior(const double curr_time, const std::string behavior, const il::trackRecord* track, const il::assetRecord* ai_asset, const double range, const double range_to_zone);
         virtual void AITakeAction(const double curr_time, const il::trackRecord* track, const il::assetRecord* ai_asset, const std::string& action_type, const double end_time_s, const double range, const double range_to_zone);
         virtual void AIEgress(const double curr_time, const il::assetRecord* ai_asset, const std::string& egress_method, const std::string& corridor_name, const double speed);
   protected:
      std::weak_ptr<logger::GlobalLogger> mGlobalLogger;
   };

}

#endif
