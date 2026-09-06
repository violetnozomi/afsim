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
*   File: WsfBMCSV_EventOutput.hpp
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

#ifndef WSFBMCSV_EVENTOUTPUT_HPP
#define WSFBMCSV_EVENTOUTPUT_HPP

#include <memory>

#include "wsf_iads_c2_export.h"

#include "UtPlugin.hpp"
#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"
#include "WsfComm.hpp"
#include "WsfMessage.hpp"
class     WsfCSV_EventOutput;
#include "WsfSimulation.hpp"
#include <WsfBMCommon.hpp>
#include <WsfBMMOELogger.hpp>

namespace il
{
   class evaluationAssignment;
}

class WSF_IADS_C2_LIB_EXPORT WsfBMCSV_EventOutputData
{
   public:
      WsfBMCSV_EventOutputData();
};

//! WsfBMCSV_EventOutput writes BM specific events to the event output logfile.
class WSF_IADS_C2_LIB_EXPORT WsfBMCSV_EventOutput : protected WsfBMCSV_EventOutputData
{
      friend class WsfBMHEFIRMOELogger;
   protected:
      //need a MOELogger object to link callbacks
      WsfBMCSV_EventOutput(WsfCSV_EventOutput&   aCSV_EventOutput,
                           WsfBMHEFIRMOELogger* aMOE_Logger);            // constructor
      //WsfBMCSV_EventOutput(WsfBMCSV_EventOutput const&){}; // copy constructor
      //WsfBMCSV_EventOutput& operator=(WsfBMCSV_EventOutput const&){}; //= operator

      WsfCSV_EventOutput* mCSV_EventOutput;

   private:
      //MOE Logger Callback
      void BMAssignment(const double curr_time, const il::evaluationAssignment* assignment, const std::string assignment_flag);

      virtual void AssignmentStatus(const double curr_time, const il::assignmentACKMessage* status, const il::evaluationAssignment* related_assignment);
      virtual void AssignmentCancel(const double curr_time, const il::idRecord* cancelling_unit, const il::evaluationAssignment* active_assignment, const std::string& cancel_reason);
      virtual void CANTCOAssignment(const double curr_time, const il::idRecord* responding_unit, const il::assignmentMessage* assignment, const il::trackRecord* opt_track, const std::string& reason);
      virtual void Allocate(const double curr_time, const il::idRecord* allocating_unit, const il::assessmentRecord* assessment);

      virtual void SensorCue(const double curr_time, const il::assetRecord* curr_unit, const il::trackRecord* master_track, const il::assignmentMessage* assignment, const il::sensorRecord* sensor_to_assign);
      virtual void SensorDropCue(const double curr_time, const il::assetRecord* curr_unit, const il::trackRecord* master_track, const il::assignmentMessage* assignment, const il::sensorRecord* sensor_to_assign);
      virtual void SensorTracking(const double curr_time, const il::trackRecord* master_track, const il::sensorRecord* assigned_sensor);

      virtual void AIAddBehavior(const double curr_time, const std::string behavior, const il::trackRecord* target, const il::assetRecord* ai_asset, const double range, const double range_to_zone);
      virtual void AITakeAction(const double curr_time, const il::trackRecord* track, const il::assetRecord* ai_asset, const std::string& action_type, const double end_time_s, const double range, const double range_to_zone);
      virtual void AIEgress(const double curr_time, const il::assetRecord* ai_asset, const std::string& egress_method, const std::string& corridor_name, const double speed);

      //Registered message print methods
      void PrintBMStatusMessage(double aSimTime, const WsfMessage& aMessage);

      //MESSAGE_RECEIVED event callback
      //void MessageReceived(double aSimTime,  WsfComm* aXmtrPtr, WsfComm* aRcvrPtr, const WsfMessage& aMessage, WsfComm::Result&  aResult);
};

#endif
