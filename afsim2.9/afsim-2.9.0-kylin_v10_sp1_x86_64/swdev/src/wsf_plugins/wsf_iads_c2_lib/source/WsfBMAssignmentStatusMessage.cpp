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
*   File: WsfBMAssignmentStatusMessage.cpp
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

#include <logger.hpp>
#include <WsfBMAssignmentStatusMessage.hpp>
#include "WsfIADSC2ScenarioExtension.hpp"
#include <WsfScriptBMAssignmentStatusClass.hpp>
#include <WsfBMUtils.hpp>

//WsfBMAssignmentStatusMessage::WsfBMAssignmentStatusMessage (const UtNullCreate& aNull)
//   : WsfMessage (aNull)
//   , WsfBMManagedTypeWrapper (WsfBMAssignmentStatusMessage::GetTypeId(), std::shared_ptr<il::baseMessage>(new il::assignmentACKMessage))
//{
//
//}

//WsfBMAssignmentStatusMessage::WsfBMAssignmentStatusMessage (WsfSimulation *simPtr)
WsfBMAssignmentStatusMessage::WsfBMAssignmentStatusMessage(std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger)
//: WsfMessage (*simPtr, WsfBMAssignmentStatusMessage::GetTypeId())
   : WsfMessage(WsfBMAssignmentStatusMessage::GetTypeId())
   , WsfBMManagedTypeWrapper(WsfBMAssignmentStatusMessage::GetTypeId(), std::shared_ptr<il::baseMessage>(new il::assignmentACKMessage(aGlobalLogger)))
{
}

WsfBMAssignmentStatusMessage::WsfBMAssignmentStatusMessage(WsfPlatform* plat)
   : WsfMessage(WsfBMAssignmentStatusMessage::GetTypeId(), plat)
   , WsfBMManagedTypeWrapper(WsfBMAssignmentStatusMessage::GetTypeId(), std::shared_ptr<il::baseMessage>(new il::assignmentACKMessage(IADSC2ScenarioExtension::GetGlobalLogger(plat->GetScenario()))))
{
}

WsfBMAssignmentStatusMessage::WsfBMAssignmentStatusMessage(WsfPlatform* plat, const std::shared_ptr<il::baseMessage>& message)
   : WsfMessage(WsfBMAssignmentStatusMessage::GetTypeId(), plat)
   , WsfBMManagedTypeWrapper(WsfBMAssignmentStatusMessage::GetTypeId(), message)
{
   if (message->getMessageType() != il::ASSIGNMENT_ACK_MESSAGE)
   {
      HCL_FATAL_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(plat->GetScenario())) << "Invalid message type, WsfBMAssignmentStatusMessage set to wrap non-assignment status message (" << message->getMessageType() << std::endl;
      assert(message->getMessageType() == il::ASSIGNMENT_ACK_MESSAGE);
      throw UtException("Invalid message type, WsfBMAssignmentStatusMessage set to wrap non-assignment status message.");
   }
}

WsfBMAssignmentStatusMessage::WsfBMAssignmentStatusMessage(const WsfBMAssignmentStatusMessage& aSrc)
   : WsfMessage(aSrc)
   , WsfBMManagedTypeWrapper(WsfBMAssignmentStatusMessage::GetTypeId(), std::make_shared<il::assignmentACKMessage>(*aSrc.GetRawMessagePtr()))
{
}

WsfBMAssignmentStatusMessage::~WsfBMAssignmentStatusMessage()
{
}

WsfBMAssignmentStatusMessage* WsfBMAssignmentStatusMessage::Clone() const
{
   return new WsfBMAssignmentStatusMessage(*this);
}

const char* WsfBMAssignmentStatusMessage::GetScriptClassName() const
{
   return WsfScriptBMAssignmentStatusClass::SCRIPT_CLASS_NAME;
}

WsfStringId WsfBMAssignmentStatusMessage::GetTypeId()
{
   return "WsfBMAssignmentStatusMessage";
}

// Accessor implementations
void WsfBMAssignmentStatusMessage::SetAckTime(const double sim_time_s)
{
   GetMutableRawMessagePtr()->setAckTime(sim_time_s);
}

double WsfBMAssignmentStatusMessage::GetAckTime() const
{
   return GetRawMessagePtr()->getAckTime();
}

bool WsfBMAssignmentStatusMessage::StatusTimeExists(const il::assignmentACKMessage::eAssignmentStatus status) const
{
   return GetRawMessagePtr()->statusTimeExists(status);
}

double WsfBMAssignmentStatusMessage::GetStatusTime(const il::assignmentACKMessage::eAssignmentStatus status) const
{
   return GetRawMessagePtr()->getStatusTime(status);
}

void WsfBMAssignmentStatusMessage::SetAssignmentReferenceTrackID(const WsfTrackId& track_id)
{
   GetMutableRawMessagePtr()->setAssignmentReferenceTrackID(WsfBMUtils::ConvertTrackId(track_id));
}

WsfTrackId WsfBMAssignmentStatusMessage::GetAssignmentReferenceTrackID() const
{
   return WsfBMUtils::ConvertTrackId(GetRawMessagePtr()->getAssignmentReferenceTrackID());
}

// Does not check plat for nullptr.
void WsfBMAssignmentStatusMessage::SetAssignedWeapon(const WsfPlatform* plat, const WsfWeapon* weapon)
{
   GetMutableRawMessagePtr()->setAssignedID(WsfBMUtils::GetAssignedIDRecord(plat, weapon));
}

const WsfPlatform* WsfBMAssignmentStatusMessage::GetAssignedPlatform(WsfSimulation* sim_ptr) const
{
   return WsfBMUtils::GetAssignedPlatformFromAssignedIDRecord(sim_ptr, GetRawMessagePtr()->getAssignedID());
}

const WsfWeapon* WsfBMAssignmentStatusMessage::GetAssignedWeapon(WsfSimulation* sim_ptr) const
{
   return WsfBMUtils::GetAssignedWeaponFromAssignedIDRecord(sim_ptr, GetRawMessagePtr()->getAssignedID());
}

// Does not check plat for nullptr.
void WsfBMAssignmentStatusMessage::SetInitiatingPlatform(const WsfPlatform* plat)
{
   GetMutableRawMessagePtr()->setInitiatingID(WsfBMUtils::GetUnitIDRecordFromPlatform(plat));
}

const WsfPlatform* WsfBMAssignmentStatusMessage::GetInitiatingPlatform(WsfSimulation* sim_ptr) const
{
   return WsfBMUtils::GetPlatformFromUnitIDRecord(sim_ptr, GetRawMessagePtr()->getInitiatingID());
}

// Does not check plat for nullptr.
void WsfBMAssignmentStatusMessage::SetAssigningPlatform(const WsfPlatform* plat)
{
   GetMutableRawMessagePtr()->setAssigningID(WsfBMUtils::GetUnitIDRecordFromPlatform(plat));
}

const WsfPlatform* WsfBMAssignmentStatusMessage::GetAssigningPlatform(WsfSimulation* sim_ptr) const
{
   return WsfBMUtils::GetPlatformFromUnitIDRecord(sim_ptr, GetRawMessagePtr()->getAssigningID());
}

void WsfBMAssignmentStatusMessage::SetAssignmentStatus(const il::assignmentACKMessage::eAssignmentStatus status)
{
   GetMutableRawMessagePtr()->setStatus(status);
}

il::assignmentACKMessage::eAssignmentStatus WsfBMAssignmentStatusMessage::GetAssignmentStatus() const
{
   return GetRawMessagePtr()->getStatus();
}

void WsfBMAssignmentStatusMessage::SetSalvosFired(const int salvos_fired)
{
   GetMutableRawMessagePtr()->setSalvosFired(salvos_fired);
}

int WsfBMAssignmentStatusMessage::GetSalvosFired() const
{
   return GetRawMessagePtr()->getSalvosFired();
}

void WsfBMAssignmentStatusMessage::SetCantcoReason(const std::string& reason)
{
   GetMutableRawMessagePtr()->setCantcoReason(reason);
}

std::string WsfBMAssignmentStatusMessage::GetCantcoReason() const
{
   return GetRawMessagePtr()->getCantoReason();
}

void WsfBMAssignmentStatusMessage::SetSystemicCantcoScope(const il::assignmentACKMessage::eSystemicCantcoScope scope)
{
   GetMutableRawMessagePtr()->setSystemicCantcoScope(scope);
}

bool WsfBMAssignmentStatusMessage::IsSystemicCantco() const
{
   return GetRawMessagePtr()->isSystemicCantco();
}

il::assignmentACKMessage::eSystemicCantcoScope WsfBMAssignmentStatusMessage::GetSystemicCantcoScope() const
{
   il::assignmentACKMessage::eSystemicCantcoScope scope = il::assignmentACKMessage::LOCAL;
   if (GetRawMessagePtr()->isSystemicCantco() && GetRawMessagePtr()->getSystemicCantcoScope(scope))
   {
      return scope;
   }

   HCL_FATAL_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetSimulation()->GetScenario())) << "Call to GetSystemicCantcoScope() when assignment status was not a CANTCO...";

   throw UtException("Call to GetSystemicCantcoScope() when assignment status was not a CANTCO...");
}

// will be null if status is not overridden. Does not check plat for nullptr.
void WsfBMAssignmentStatusMessage::SetOverridingPlatform(const WsfPlatform* plat)
{
   GetMutableRawMessagePtr()->setOverridingID(WsfBMUtils::GetUnitIDRecordFromPlatform(plat));
}

const WsfPlatform* WsfBMAssignmentStatusMessage::GetOverridingPlatform(WsfSimulation* sim_ptr) const
{
   return WsfBMUtils::GetPlatformFromUnitIDRecord(sim_ptr, GetRawMessagePtr()->getOverridingID());
}

// will be null if status is not overridden
// Does not check plat or weapon for nullptr.
void WsfBMAssignmentStatusMessage::SetNewlyAssignedWeapon(const WsfPlatform* plat, const WsfWeapon* weapon)
{
   GetMutableRawMessagePtr()->setNewlyAssignedID(WsfBMUtils::GetAssignedIDRecord(plat, weapon));
}

const WsfPlatform* WsfBMAssignmentStatusMessage::GetNewlyAssignedPlatform(WsfSimulation* sim_ptr) const
{
   return WsfBMUtils::GetAssignedPlatformFromAssignedIDRecord(sim_ptr, GetRawMessagePtr()->getNewlyAssignedID());
}

const WsfWeapon* WsfBMAssignmentStatusMessage::GetNewlyAssignedWeapon(WsfSimulation* sim_ptr) const
{
   return WsfBMUtils::GetAssignedWeaponFromAssignedIDRecord(sim_ptr, GetRawMessagePtr()->getNewlyAssignedID());
}

void WsfBMAssignmentStatusMessage::Print() const
{
   GetRawMessagePtr()->logSTD();
}
