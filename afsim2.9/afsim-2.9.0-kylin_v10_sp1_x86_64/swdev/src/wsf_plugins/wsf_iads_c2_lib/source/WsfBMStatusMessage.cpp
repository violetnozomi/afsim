// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#include <logger.hpp>
#include "WsfBMStatusMessage.hpp"
#include <UtException.hpp>
#include <WsfPlatform.hpp>
#include <WsfWeapon.hpp>
#include <UtVec3.hpp>
#include <UtNullCreate.hpp>

#include <WsfBMUtils.hpp>
#include "WsfIADSC2ScenarioExtension.hpp"
#include <WsfScriptBMStatusMessageClass.hpp>

//WsfBMStatusMessage::WsfBMStatusMessage (const UtNullCreate& aNull)
//   : WsfMessage (aNull)
//   , WsfBMManagedTypeWrapper (WsfBMStatusMessage::GetTypeId(), std::shared_ptr<il::baseMessage>(new il::combatStatusMessage))
//{
//
//}

//WsfBMStatusMessage::WsfBMStatusMessage(WsfSimulation *simPtr)
WsfBMStatusMessage::WsfBMStatusMessage(std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger)
//: WsfMessage (*simPtr, WsfBMStatusMessage::GetTypeId())
   : WsfMessage(WsfBMStatusMessage::GetTypeId())
   , WsfBMManagedTypeWrapper(WsfBMStatusMessage::GetTypeId(), std::shared_ptr<il::baseMessage>(new il::combatStatusMessage(aGlobalLogger)))
{
}

WsfBMStatusMessage::WsfBMStatusMessage(WsfPlatform* aPlatformPtr)
   : WsfMessage(WsfBMStatusMessage::GetTypeId(), aPlatformPtr)
   , WsfBMManagedTypeWrapper(WsfBMStatusMessage::GetTypeId(), std::shared_ptr<il::baseMessage>(new il::combatStatusMessage(IADSC2ScenarioExtension::GetGlobalLogger(aPlatformPtr->GetScenario()))))
{
}

WsfBMStatusMessage::WsfBMStatusMessage(WsfPlatform* aPlatformPtr, const std::shared_ptr<il::baseMessage>& message)
   : WsfMessage(WsfBMStatusMessage::GetTypeId(), aPlatformPtr)
   , WsfBMManagedTypeWrapper(WsfBMStatusMessage::GetTypeId(), message)
{
   if (message->getMessageType() != il::COMBAT_STATUS_MESSAGE)
   {
      HCL_FATAL_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(aPlatformPtr->GetScenario())) << "Invalid message type, WsfBMStatusMessage set to wrap non-status message (" << message->getMessageType() << std::endl;
      assert(message->getMessageType() == il::COMBAT_STATUS_MESSAGE);
      throw UtException("Invalid message type, WsfBMAssignmentStatusMessage set to wrap non-status message.");
   }
}

WsfBMStatusMessage::WsfBMStatusMessage(const WsfBMStatusMessage& aSrc)
   : WsfMessage(aSrc)
   , WsfBMManagedTypeWrapper(WsfBMStatusMessage::GetTypeId(), std::make_shared<il::combatStatusMessage>(*aSrc.GetRawMessagePtr()))
{
}

WsfBMStatusMessage::~WsfBMStatusMessage()
{
}

WsfBMStatusMessage* WsfBMStatusMessage::Clone() const
{
   return new WsfBMStatusMessage(*this);
}

const char* WsfBMStatusMessage::GetScriptClassName() const
{
   return WsfScriptBMStatusMessageClass::SCRIPT_CLASS_NAME;
}

const char* WsfBMStatusMessage::GetBaseClassName()
{
   return WsfScriptBMStatusMessageClass::BASE_CLASS_NAME;
}

WsfStringId WsfBMStatusMessage::GetTypeId()
{
   return "WsfBMStatusMessage";
}

// Accessor implementations
void WsfBMStatusMessage::SetStatusTime(const double sim_time_s)
{
   GetMutableRawMessagePtr()->setStatusTime(sim_time_s);
}

double WsfBMStatusMessage::GetStatusTime() const
{
   return GetRawMessagePtr()->getStatusTime();
}

void WsfBMStatusMessage::SetPlatform(const WsfPlatform* aPlatformPtr)
{
   GetMutableRawMessagePtr()->setAssetID(WsfBMUtils::GetUnitIDRecordFromPlatform(aPlatformPtr));
}

const WsfPlatform* WsfBMStatusMessage::GetPlatform(const WsfSimulation* simPtr) const
{
   auto nonconst_sim_ptr = const_cast<WsfSimulation*>(simPtr);
   return WsfBMUtils::GetPlatformFromUnitIDRecord(nonconst_sim_ptr, GetRawMessagePtr()->getAssetID());
}

std::string WsfBMStatusMessage::GetPlatformName() const
{
   return GetRawMessagePtr()->getAssetID().getIDString();
}

void WsfBMStatusMessage::SetMaxAssignments(const int max_assignments)
{
   GetMutableRawMessagePtr()->setMaxAssignments(max_assignments);
}

int WsfBMStatusMessage::GetMaxAssignments() const
{
   return GetRawMessagePtr()->getMaxAssignments();
}

void WsfBMStatusMessage::SetNumAssignments(const int num_assignments)
{
   GetMutableRawMessagePtr()->setCurrentAssignments(num_assignments);
}

int WsfBMStatusMessage::GetNumAssignments() const
{
   return GetRawMessagePtr()->getCurrentAssignments();
}

void WsfBMStatusMessage::SetWeaponStatus(const WsfPlatform* shooter, const WsfWeapon* weapon)
{
   auto weapon_system_id = WsfBMUtils::GetWeaponID(shooter, weapon);
   auto munitions_ready = (unsigned short)weapon->GetQuantityRemaining();
   auto total_munitions = (unsigned short)weapon->GetQuantityRemaining();
   auto total_fire_channels = weapon->GetMaximumRequestCount();
   auto allocated_fire_channels = weapon->GetActiveRequestCount();

   auto msg_ptr = GetMutableRawMessagePtr();
   il::combatStatusMessage::WeaponSystem wsys(weapon_system_id, munitions_ready, total_munitions, total_fire_channels, allocated_fire_channels);
   msg_ptr->addWeapon(wsys);
}

void WsfBMStatusMessage::SetSystemStatus(const eSystemStatus& status)
{
   il::SYSTEM_STATUS il_status = il::STATUS_RED;
   switch (status)
   {
      case E_SYS_WHITE:
         il_status = il::STATUS_WHITE;
         break;
      case E_SYS_YELLOW:
         il_status = il::STATUS_YELLOW;
         break;
      case E_SYS_GREEN:
         il_status = il::STATUS_GREEN;
         break;
      case E_SYS_RED:
         il_status = il::STATUS_RED;
         break;
      default:
         HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetSimulation()->GetScenario())) << "WsfBMStatusMessage::SetSystemStatus(): Unknown status: " << status << std::endl;
         break;
   }

   GetMutableRawMessagePtr()->setSystemStatus(il_status);
}

WsfBMStatusMessage::eSystemStatus WsfBMStatusMessage::GetSystemStatus() const
{
   eSystemStatus status = E_SYS_RED;
   switch (GetRawMessagePtr()->getSystemStatus())
   {
      case il::STATUS_WHITE:
         status = E_SYS_WHITE;
         break;
      case il::STATUS_YELLOW:
         status = E_SYS_YELLOW;
         break;
      case il::STATUS_GREEN:
         status = E_SYS_GREEN;
         break;
      case il::STATUS_RED:
         status = E_SYS_RED;
         break;
      default:
         HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetSimulation()->GetScenario())) << "WsfBMStatusMessage::GetSystemStatus(): Unknown status: " << GetRawMessagePtr()->getSystemStatus() << std::endl;
         break;
   }

   return status;
}

void WsfBMStatusMessage::SetPosition(const double position_time_s, const WsfGeoPoint& location)
{
   auto msg_ptr = GetMutableRawMessagePtr();

   msg_ptr->setPositionTime(position_time_s);
   msg_ptr->setPosition(location.GetLat(), location.GetLon(), location.GetAlt());
}

bool WsfBMStatusMessage::HasPosition() const
{
   return GetRawMessagePtr()->hasPosition();
}

const WsfGeoPoint WsfBMStatusMessage::GetPosition() const
{
   double lla_ddm[3] = { 0 };
   GetRawMessagePtr()->getPosition(lla_ddm[0], lla_ddm[1], lla_ddm[2]);

   WsfGeoPoint location;
   location.SetLocationLLA(lla_ddm[0], lla_ddm[1], lla_ddm[2]);

   return location;
}

double WsfBMStatusMessage::GetPositionTime() const
{
   return GetRawMessagePtr()->getPositionTime();
}

void WsfBMStatusMessage::SetVelocityECEF(const UtVec3d& vel)
{
   GetMutableRawMessagePtr()->setVelocity(vel[0], vel[1], vel[2]);
}

bool WsfBMStatusMessage::HasVelocity() const
{
   return GetRawMessagePtr()->hasVelocity();
}

const UtVec3d WsfBMStatusMessage::GetVelocityECEF() const
{
   double ecef_vel_ms[3] = { 0 };
   GetRawMessagePtr()->getVelocity(ecef_vel_ms[0], ecef_vel_ms[1], ecef_vel_ms[2]);

   return UtVec3d(ecef_vel_ms);
}
