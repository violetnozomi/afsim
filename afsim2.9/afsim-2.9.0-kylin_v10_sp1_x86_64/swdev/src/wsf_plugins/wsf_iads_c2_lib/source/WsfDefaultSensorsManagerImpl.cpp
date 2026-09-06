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
*   File: WsfDefaultSensorsManagerImpl.cpp
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
*   Abstract: This class provides default implementations in C++ for
*    what could be overridden in script for the Sensors Manager base class.
*    The intention was to provide the classical implementation of what was originally
*    done in script so default usage of the processor works without supplemental scripting.
*
*******************************************************************************/

#include <WsfDefaultSensorsManagerImpl.hpp>

#include <memory>

#include <WsfPlatform.hpp>

#include <WsfBMSensorsManager.hpp>
#include <WsfAssetManager.hpp>
#include <WsfBMUtils.hpp>
#include <logger.hpp>
#include <iadsLib/assignmentMessage.hpp>
#include <iadsLib/trackContainers.hpp>
#include <iadsLib/sensorRecordInterface.hpp>
#include "WsfIADSC2ScenarioExtension.hpp"

WsfDefaultSensorsManagerImpl::WsfDefaultSensorsManagerImpl()
{
}

bool WsfDefaultSensorsManagerImpl::ProcessInput(UtInput& aInput)
{
   return false;
}

void WsfDefaultSensorsManagerImpl::Initialize(WsfBMSensorsManager* sm)
{
   HCL_DEBUG_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(sm->GetScenario())) << sm->GetPlatform()->GetName() << "/" << sm->GetName() << ": " << sm->GetScriptClassName() << ": in on_initialize" << std::endl;
}

void WsfDefaultSensorsManagerImpl::Initialize2(WsfBMSensorsManager* sm)
{
   HCL_DEBUG_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(sm->GetScenario())) << sm->GetPlatform()->GetName() << "/" << sm->GetName() << ": " << sm->GetScriptClassName() << ": in on_initialize2" << std::endl;

   // locate the other managers
   m_am_ref = WsfBMUtils::FindAttachedAssetManagerRef2(sm->GetPlatform(), true);

   if (!m_am_ref)
   {
      HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(sm->GetScenario())) << "Sensors Manager: could not locate asset manager - all platforms on the C2 network must have an asset manager!" << std::endl;
   }
}

void WsfDefaultSensorsManagerImpl::on_update(const double sim_time_s, WsfPlatform* this_plat, WsfBMSensorsManager* sm)
{
   auto sm_proc = sm->GetCoreSMRef();
   auto am_proc = sm->GetCoreAMRef();

   sm->UpdateSensorCuePerception();
   il::moeLoggingInterface* logger = IADSC2SimulationExtension::Find(*this_plat->GetSimulation())->GetLogger();

   sm_proc->ProcessResponsibleAssignments(*logger, sim_time_s);

   // Loop through the AssignmentArray
   MapAssignmentArrayFunc
   (*logger, am_proc->getAssignmentArray(),
    [sim_time_s](const il::moeLoggingInterface& aMOELogger, il::evalAssignmentMultimap &assignment, il::evalAssignmentMultimap::iterator &assigned_track_iter, il::evalAssignmentMultimap::iterator &assignment_iter, WsfBMSensorsManager *sm) -> void
   {
      auto sm_proc = sm->GetCoreSMRef();

      if (!assignment_iter->second->isAssignmentComplete())
      {
         sm_proc->ProcessAssignment(sim_time_s, assignment_iter->second);
      }

      ++assignment_iter;
   }, sm);

   sm_proc->CreateSensorCues(*logger, sim_time_s); // create new cues for dispatch based off the responsible assignments we may have just created
}

bool WsfDefaultSensorsManagerImpl::on_message(const double sim_time_s, const WsfMessage& message, WsfBMSensorsManager* sm)
{
   // if we don't want to block other processors from receiving the message, we must keep this flag false,
   // otherwise nobody else will get a chance to process it. We do this so that other processors have a shot regardless if
   // we process it or not.
   bool block_processing_message = false;

   // all messages get dispatched to the asset manager
   if (m_am_ref)
   {
      m_am_ref->ProcessMessage(sim_time_s, message);
   }
   else
   {
      HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(sm->GetScenario())) << "WsfDefaultSensorsManagerImpl::on_message(): no asset manager to dispatch message to..." << std::endl;
   }

   return block_processing_message;
}
