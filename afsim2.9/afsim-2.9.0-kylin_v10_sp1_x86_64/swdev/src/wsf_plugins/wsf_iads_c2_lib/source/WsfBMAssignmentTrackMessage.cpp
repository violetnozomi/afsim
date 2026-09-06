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
*   File: WsfBMAssignmentTrackMessage.cpp
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

#include <WsfBMAssignmentTrackMessage.hpp>

#include <WsfLocalTrack.hpp>

#include <WsfBMUtils.hpp>
#include "WsfIADSC2ScenarioExtension.hpp"
#include <logger.hpp>

#include <WsfScriptBMAssignmentTrackMessageClass.hpp>

WsfBMAssignmentTrackMessage::WsfBMAssignmentTrackMessage(WsfSimulation* simPtr)
   : WsfMessage(WsfBMAssignmentTrackMessage::GetTypeId())
   , WsfBMManagedTypeWrapper(WsfBMAssignmentTrackMessage::GetTypeId(), std::shared_ptr<il::baseMessage>(new il::assignmentTrackMessage(IADSC2ScenarioExtension::GetGlobalLogger(simPtr->GetScenario()))))
{
}

WsfBMAssignmentTrackMessage::WsfBMAssignmentTrackMessage(WsfPlatform* plat)
   : WsfMessage(WsfBMAssignmentTrackMessage::GetTypeId(), plat)
   , WsfBMManagedTypeWrapper(WsfBMAssignmentTrackMessage::GetTypeId(), std::shared_ptr<il::baseMessage>(new il::assignmentTrackMessage(IADSC2ScenarioExtension::GetGlobalLogger(plat->GetScenario()))))
{
}

WsfBMAssignmentTrackMessage::WsfBMAssignmentTrackMessage(WsfPlatform* plat, const std::shared_ptr<il::baseMessage>& message)
   : WsfMessage(WsfBMAssignmentTrackMessage::GetTypeId(), plat)
   , WsfBMManagedTypeWrapper(WsfBMAssignmentTrackMessage::GetTypeId(), message)
{
   if (message->getMessageType() != il::ASSIGNMENT_TRACK_MESSAGE)
   {
      HCL_FATAL_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(plat->GetScenario())) << "Invalid message type, WsfBMAssignmentTrackMessage set to wrap non-assignment track message (" << message->getMessageType() << std::endl;
      assert(message->getMessageType() == il::ASSIGNMENT_TRACK_MESSAGE);
      throw UtException("Invalid message type, WsfBMAssignmentTrackMessage set to wrap non-assignment track message.");
   }
}

WsfBMAssignmentTrackMessage::WsfBMAssignmentTrackMessage(const WsfBMAssignmentTrackMessage& aSrc)
   : WsfMessage(aSrc)
   , WsfBMManagedTypeWrapper(WsfBMAssignmentTrackMessage::GetTypeId(), std::make_shared<il::assignmentTrackMessage>(*aSrc.GetRawMessagePtr()))
   , m_track(aSrc.m_track)
{
}

WsfBMAssignmentTrackMessage::~WsfBMAssignmentTrackMessage()
{
}

WsfBMAssignmentTrackMessage* WsfBMAssignmentTrackMessage::Clone() const
{
   return new WsfBMAssignmentTrackMessage(*this);
}

const char* WsfBMAssignmentTrackMessage::GetScriptClassName() const
{
   return WsfScriptBMAssignmentTrackMessageClass::SCRIPT_CLASS_NAME;
}

const char* WsfBMAssignmentTrackMessage::GetBaseClassName()
{
   return WsfScriptBMAssignmentTrackMessageClass::BASE_CLASS_NAME;
}

WsfStringId WsfBMAssignmentTrackMessage::GetTypeId()
{
   return "WsfBMAssignmentTrackMessage";
}

void WsfBMAssignmentTrackMessage::SetTrack(WsfSimulation* sim, const WsfLocalTrack& track)
{
   auto bm_track = WsfBMUtils::ConvertTrack(sim, &track, false);
   auto atm = bm_track.GetRawMessagePtr();
   il::trackRecord tr;
   tr.processMessage(*atm);
   GetMutableRawMessagePtr()->setTrack(tr);

   m_track = track;
}

const WsfTrack& WsfBMAssignmentTrackMessage::GetTrack() const
{
   return m_track;
}

void WsfBMAssignmentTrackMessage::SetAssignedPlatform(const WsfPlatform* plat)
{
   GetMutableRawMessagePtr()->setAssignedUnitID(WsfBMUtils::GetUnitIDRecordFromPlatform(plat));
}

WsfPlatform* WsfBMAssignmentTrackMessage::GetAssignedPlatform(WsfSimulation* sim) const
{
   return WsfBMUtils::GetAssignedPlatformFromAssignedIDRecord(sim, GetRawMessagePtr()->getAssignedUnitID());
}
