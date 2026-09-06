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
*   File: WsfBMAssignmentMessage.cpp
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

#include <WsfSimulation.hpp>
#include <WsfPlatform.hpp>
#include <UtException.hpp>
#include <UtNullCreate.hpp>

#include <logger.hpp>
#include <WsfBMAssignmentMessage.hpp>
#include "WsfIADSC2ScenarioExtension.hpp"
#include <WsfScriptBMAssignmentMessageClass.hpp>
#include <WsfBMUtils.hpp>

#include <iadsLib/util.hpp>

//WsfBMAssignmentMessage::WsfBMAssignmentMessage (const UtNullCreate& aNull)
//   : WsfMessage (aNull)
//   , WsfBMManagedTypeWrapper (WsfBMAssignmentMessage::GetTypeId(), std::shared_ptr<il::baseMessage>(new il::assignmentMessage))
//{
//
//}

WsfBMAssignmentMessage::WsfBMAssignmentMessage(WsfSimulation* simPtr)
//: WsfMessage (*simPtr, WsfBMAssignmentMessage::GetTypeId())
   : WsfMessage(WsfBMAssignmentMessage::GetTypeId())
   , WsfBMManagedTypeWrapper(WsfBMAssignmentMessage::GetTypeId(), std::shared_ptr<il::baseMessage>(new il::assignmentMessage(IADSC2ScenarioExtension::GetGlobalLogger(simPtr->GetScenario()))))
{
}

WsfBMAssignmentMessage::WsfBMAssignmentMessage(WsfPlatform* plat)
   : WsfMessage(WsfBMAssignmentMessage::GetTypeId(), plat)
   , WsfBMManagedTypeWrapper(WsfBMAssignmentMessage::GetTypeId(), std::shared_ptr<il::baseMessage>(new il::assignmentMessage(IADSC2ScenarioExtension::GetGlobalLogger(plat->GetScenario()))))
{
}

WsfBMAssignmentMessage::WsfBMAssignmentMessage(WsfPlatform* plat, const std::shared_ptr<il::baseMessage>& message)
   : WsfMessage(WsfBMAssignmentMessage::GetTypeId(), plat)
   , WsfBMManagedTypeWrapper(WsfBMAssignmentMessage::GetTypeId(), message)
{
   if (message->getMessageType() != il::ASSIGNMENT_MESSAGE)
   {
      HCL_FATAL_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(plat->GetScenario())) << "Invalid message type, WsfBMAssignmentMessage set to wrap non-assignment message (" << message->getMessageType() << std::endl;
      assert(message->getMessageType() == il::ASSIGNMENT_MESSAGE);
      throw UtException("Invalid message type, WsfBMAssignmentMessage set to wrap non-assignment message.");
   }
}

WsfBMAssignmentMessage::WsfBMAssignmentMessage(const WsfBMAssignmentMessage& aSrc)
   : WsfMessage(aSrc)
   , WsfBMManagedTypeWrapper(WsfBMAssignmentMessage::GetTypeId(), std::make_shared<il::assignmentMessage>(*aSrc.GetRawMessagePtr()))
{
}

WsfBMAssignmentMessage::~WsfBMAssignmentMessage()
{
}

WsfBMAssignmentMessage* WsfBMAssignmentMessage::Clone() const
{
   return new WsfBMAssignmentMessage(*this);
}

const char* WsfBMAssignmentMessage::GetScriptClassName() const
{
   return WsfScriptBMAssignmentMessageClass::SCRIPT_CLASS_NAME;
}

const char* WsfBMAssignmentMessage::GetBaseClassName()
{
   return WsfScriptBMAssignmentMessageClass::BASE_CLASS_NAME;
}

WsfStringId WsfBMAssignmentMessage::GetTypeId()
{
   return "WsfBMAssignmentMessage";
}

// Accessor implementations
void WsfBMAssignmentMessage::SetAssignmentTime(const double sim_time_s)
{
   GetMutableRawMessagePtr()->setAssignTime(sim_time_s);
}

double WsfBMAssignmentMessage::GetAssignmentTime() const
{
   return GetRawMessagePtr()->getAssignTime();
}

// Does not check plat for nullptr.
void WsfBMAssignmentMessage::SetAssignedWeapon(const WsfPlatform* plat, const WsfWeapon* weapon)
{
   GetMutableRawMessagePtr()->setAssignedID(WsfBMUtils::GetAssignedIDRecord(plat, weapon));
}

const WsfPlatform* WsfBMAssignmentMessage::GetAssignedPlatform(WsfSimulation* sim_ptr) const
{
   return WsfBMUtils::GetAssignedPlatformFromAssignedIDRecord(sim_ptr, GetRawMessagePtr()->getAssignedID());
}

const WsfWeapon* WsfBMAssignmentMessage::GetAssignedWeapon(WsfSimulation* sim_ptr) const
{
   return WsfBMUtils::GetAssignedWeaponFromAssignedIDRecord(sim_ptr, GetRawMessagePtr()->getAssignedID());
}

void WsfBMAssignmentMessage::SetAssignmentReferenceTrackID(const WsfTrackId& track_id)
{
   GetMutableRawMessagePtr()->setReferenceTrackID(WsfBMUtils::ConvertTrackId(track_id));
}

const WsfTrackId WsfBMAssignmentMessage::GetAssignmentReferenceTrackID() const
{
   return WsfBMUtils::ConvertTrackId(GetRawMessagePtr()->getReferenceTrackID());
}

void WsfBMAssignmentMessage::SetAssignmentLocalTrackID(const WsfTrackId& track_id)
{
   GetMutableRawMessagePtr()->setLocalTrackID(WsfBMUtils::ConvertTrackId(track_id));
}

const WsfTrackId WsfBMAssignmentMessage::GetAssignmentLocalTrackID() const
{
   return WsfBMUtils::ConvertTrackId(GetRawMessagePtr()->getLocalTrackID());
}

void WsfBMAssignmentMessage::SetInitiatingPlatform(const WsfPlatform* plat)
{
   GetMutableRawMessagePtr()->setInitiatingID(WsfBMUtils::GetUnitIDRecordFromPlatform(plat));
}

const WsfPlatform* WsfBMAssignmentMessage::GetInitiatingPlatform(WsfSimulation* sim_ptr) const
{
   return WsfBMUtils::GetPlatformFromUnitIDRecord(sim_ptr, GetRawMessagePtr()->getInitiatingID());
}

void WsfBMAssignmentMessage::SetAssigningPlatform(const WsfPlatform* plat)
{
   GetMutableRawMessagePtr()->setAssigningID(WsfBMUtils::GetUnitIDRecordFromPlatform(plat));
}

const WsfPlatform* WsfBMAssignmentMessage::GetAssigningPlatform(WsfSimulation* sim_ptr) const
{
   return WsfBMUtils::GetPlatformFromUnitIDRecord(sim_ptr, GetRawMessagePtr()->getAssigningID());
}

void WsfBMAssignmentMessage::SetSelfDefenseFlag(const bool is_self_defense)
{
   GetMutableRawMessagePtr()->setSelfDefenseFlag(is_self_defense);
}

bool WsfBMAssignmentMessage::GetSelfDefenseFlag() const
{
   return GetRawMessagePtr()->getSelfDefenseFlag();
}

void WsfBMAssignmentMessage::SetMaxFiringTime(const double sim_time_s)
{
   GetMutableRawMessagePtr()->setMaxFiringTime(sim_time_s);
}

double WsfBMAssignmentMessage::GetMaxFiringTime() const
{
   return GetRawMessagePtr()->getMaxFiringTime();
}

void WsfBMAssignmentMessage::SetAssignmentReason(const il::assignmentMessage::AssignmentReason reason)
{
   GetMutableRawMessagePtr()->setAssignmentReason(reason);
}

il::assignmentMessage::AssignmentReason WsfBMAssignmentMessage::GetAssignmentReason() const
{
   return GetRawMessagePtr()->getAssignmentReason();
}

std::string WsfBMAssignmentMessage::GetAssignmentReasonString() const
{
   return GetRawMessagePtr()->getAssignmentReasonString();
}

void WsfBMAssignmentMessage::SetShotDoctrine(const il::eShotDoctrine shot_doctrine)
{
   GetMutableRawMessagePtr()->setShotDoctrine(shot_doctrine);
}

il::eShotDoctrine WsfBMAssignmentMessage::GetShotDoctrine() const
{
   return GetRawMessagePtr()->getShotDoctrine();
}

std::string WsfBMAssignmentMessage::GetshotDoctrineString() const
{
   return il::ShotDoctrineEnumToString(GetRawMessagePtr()->getShotDoctrine());
}

void WsfBMAssignmentMessage::Print() const
{
   GetRawMessagePtr()->logSTD();
}
