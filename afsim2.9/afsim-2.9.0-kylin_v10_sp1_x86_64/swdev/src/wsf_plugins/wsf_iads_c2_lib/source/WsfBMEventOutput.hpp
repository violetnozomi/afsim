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
*   File: WsfBMEventOutput.hpp
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

#ifndef WSF_BM_EVENTOUTPUT_HPP
#define WSF_BM_EVENTOUTPUT_HPP

#include <memory>
#include <WsfBMCommon.hpp>
//#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"
#include "WsfComm.hpp"
#include "UtPlugin.hpp"

#include "WsfCallback.hpp"
#include "WsfMessage.hpp"
#include "WsfSimulation.hpp"
class    WsfEventOutput;
#include <WsfBMMOELogger.hpp>

namespace il
{
   class evaluationAssignment;
}

//! WsfBMEventOutput writes BM specific events to the event output logfile.
class WSF_IADS_C2_LIB_EXPORT WsfBMEventOutput
{
      friend class WsfBMHEFIRMOELogger;
   protected:
      // need a MOELogger object to link callbacks
      WsfBMEventOutput(WsfEventOutput&      aEventOutput,
                       WsfBMHEFIRMOELogger* aMOE_Logger);	// constructor
      //WsfBMEventOutput(WsfBMEventOutput const&){}; // copy constructor is private
      //WsfBMEventOutput& operator=(WsfBMEventOutput const&){}; // assignment operator is private


      WsfEventOutput* mEventOutput;

   private:
      // MOE logger callbacks
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

      // Registered message print methods
      void PrintBMStatusMessage(double aSimTime, const WsfMessage& aMessage);

      //MESSAGE_RECEIVED event callback
      //void MessageReceived(double aSimTime,  WsfComm* aXmtrPtr, WsfComm* aRcvrPtr, const WsfMessage& aMessage, WsfComm::Result&  aResult);
      //void LocalTrackInitiated(double aSimTime, WsfPlatform* aPlatformPtr, const WsfLocalTrack* aTrackPtr, const WsfTrack* aSourcePtr);
};

#endif
