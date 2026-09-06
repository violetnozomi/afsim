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
*   File: WsfBMMOELogger.hpp
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

#ifndef WSF_BM_MOE_LOGGER_HPP
#define WSF_BM_MOE_LOGGER_HPP

#include <UtMemory.hpp>

#include <WsfBMCommon.hpp>
#include <iadsLib/moeLoggingInterface.hpp>

#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptManager.hpp"
#include "script/WsfScriptObserver.hpp"

#include <UtPlugin.hpp>
#include <UtCallback.hpp>
#include "UtScriptDataPack.hpp"
#include "UtScript.hpp"

#include "WsfSimulation.hpp"
#include "WsfSimulationExtension.hpp"

class WsfBMCSV_EventOutput;
class WsfBMEventOutput;
class WsfSimulation;

namespace il
{
   class evaluationAssignment;
}

class WSF_IADS_C2_LIB_EXPORT WsfBMHEFIRMOELogger : public il::moeLoggingInterface
{
   public:
      WsfBMHEFIRMOELogger(WsfSimulation* aSimulation);
      ~WsfBMHEFIRMOELogger() override = default;

      UtCallbackListN<void(const double, const il::evaluationAssignment*, const std::string)> OnAssignment;
      UtCallbackListN<void(const double, const il::assignmentACKMessage*, const il::evaluationAssignment*)> OnAssignmentStatus;
      UtCallbackListN<void(const double, const il::idRecord*, const il::evaluationAssignment*, const std::string&)> OnAssignmentCancel;
      UtCallbackListN<void(const double, const il::idRecord*, const il::assignmentMessage*, const il::trackRecord*, const std::string&)> OnCANTCOAssignment;
      UtCallbackListN<void(const double, const il::idRecord*, const il::assessmentRecord*)> OnAllocate;

      UtCallbackListN<void(const double, const il::assetRecord*, const il::trackRecord*, const il::assignmentMessage*, const il::sensorRecord*)> OnSensorCue;
      UtCallbackListN<void(const double, const il::assetRecord*, const il::trackRecord*, const il::assignmentMessage*, const il::sensorRecord*)> OnSensorDropCue;
      UtCallbackListN<void(const double, const il::trackRecord*, const il::sensorRecord*)> OnSensorTracking;

      UtCallbackListN<void(const double, const std::string, const il::trackRecord*, const il::assetRecord*, const double, const double)> OnAIAddBehavior;
      UtCallbackListN<void(const double, const il::trackRecord*, const il::assetRecord*, const std::string&, const double, const double, const double)> OnAITakeAction;
      UtCallbackListN<void(const double, const il::assetRecord*, const std::string&, const std::string&, const double)> OnAIEgress;

      void Assignment(const double curr_time, const il::evaluationAssignment* assignment, const std::string assignment_flag) override;
      void AssignmentStatus(const double curr_time, const il::assignmentACKMessage* status, const il::evaluationAssignment* related_assignment) override;
      void AssignmentCancel(const double curr_time, const il::idRecord* cancelling_unit, const il::evaluationAssignment* active_assignment, const std::string& cancel_reason) override;
      void CANTCOAssignment(const double curr_time, const il::idRecord* responding_unit, const il::assignmentMessage* assignment, const il::trackRecord* opt_track, const std::string& reason) override;
      void WILCOAssignment(const double curr_time, const il::assignmentACKMessage* status, const il::evaluationAssignment* related_assignment) override;
      void HAVCOAssignment(const double curr_time, const il::assignmentACKMessage* status, const il::evaluationAssignment* related_assignment) override;
      void Allocate(const double curr_time, const il::idRecord* allocating_unit, const il::assessmentRecord* assessment) override;

      void SensorCue(const double curr_time, const il::assetRecord* curr_unit, const il::trackRecord* master_track, const il::assignmentMessage* assignment, const il::sensorRecord* sensor_to_assign) override;
      void SensorDropCue(const double curr_time, const il::assetRecord* curr_unit, const il::trackRecord* master_track, const il::assignmentMessage* assignment, const il::sensorRecord* sensor_to_assign) override;
      void SensorTracking(const double curr_time, const il::trackRecord* master_track, const il::sensorRecord* assigned_sensor) override;

      void AIAddBehavior(const double curr_time, const std::string behavior, const il::trackRecord* target, const il::assetRecord* ai_asset, const double range, const double range_to_zone) override;
      void AITakeAction(const double curr_time, const il::trackRecord* track, const il::assetRecord* ai_asset, const std::string& action_type, const double end_time_s, const double range, const double range_to_zone) override;
      void AIEgress(const double curr_time, const il::assetRecord* ai_asset, const std::string& egress_method, const std::string& corridor_name, const double speed) override;

      void RegisterScriptObservers(WsfSimulation& aSimulation);

   protected:
      WsfBMHEFIRMOELogger(WsfBMHEFIRMOELogger const&) = default;
      WsfBMHEFIRMOELogger& operator=(WsfBMHEFIRMOELogger const&) = delete;

      WsfSimulation*                        mSimulationPtr; // we don't own this, slice

      std::shared_ptr<WsfBMEventOutput>     mEventOutput;
      std::shared_ptr<WsfBMCSV_EventOutput> mCSV_EventOutput;

};

//! We need to define this in here so that we can be used by plugins that want to override
//! the moeLogger.
class WSF_IADS_C2_LIB_EXPORT IADSC2SimulationExtension : public WsfSimulationExtension
{
public:
   IADSC2SimulationExtension();

   void AddedToSimulation() override;

   void SetLogger(std::unique_ptr<il::moeLoggingInterface> aMoeLogger);
   il::moeLoggingInterface* GetLogger() const;

   static IADSC2SimulationExtension* Find(const WsfSimulation& aSimulation);

private:
   std::unique_ptr<il::moeLoggingInterface> mMoeLogger;
};

#endif
