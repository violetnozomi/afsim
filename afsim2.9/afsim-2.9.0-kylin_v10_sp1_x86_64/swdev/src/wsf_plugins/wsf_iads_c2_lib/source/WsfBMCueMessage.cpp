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
*   File: WsfBMCueMessage.cpp
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

#include <WsfBMCueMessage.hpp>

#include <WsfPlatform.hpp>
#include <WsfSimulation.hpp>
#include <WsfSensor.hpp>

#include <logger.hpp>
#include <WsfBMUtils.hpp>
#include "WsfIADSC2ScenarioExtension.hpp"
#include <WsfScriptBMCueMessageClass.hpp>

WsfBMCueMessage::WsfBMCueMessage(WsfSimulation* simPtr)
   : WsfMessage(WsfBMCueMessage::GetTypeId())
   , WsfBMManagedTypeWrapper(WsfBMCueMessage::GetTypeId(), std::shared_ptr<il::baseMessage>(new il::cueMessage(IADSC2ScenarioExtension::GetGlobalLogger(simPtr->GetScenario()))))
{
}

WsfBMCueMessage::WsfBMCueMessage(WsfPlatform* plat)
   : WsfMessage(WsfBMCueMessage::GetTypeId(), plat)
   , WsfBMManagedTypeWrapper(WsfBMCueMessage::GetTypeId(), std::shared_ptr<il::baseMessage>(new il::cueMessage(IADSC2ScenarioExtension::GetGlobalLogger(plat->GetScenario()))))
{
}

WsfBMCueMessage::WsfBMCueMessage(WsfPlatform* plat, const std::shared_ptr<il::baseMessage>& message)
   : WsfMessage(WsfBMCueMessage::GetTypeId(), plat)
   , WsfBMManagedTypeWrapper(WsfBMCueMessage::GetTypeId(), message)
{
   if (message->getMessageType() != il::CUE_MESSAGE)
   {
      HCL_FATAL_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(plat->GetScenario())) << "Invalid message type, WsfBMCueMessage set to wrap non-cue message (" << message->getMessageType() << std::endl;
      assert(message->getMessageType() == il::CUE_MESSAGE);
      throw UtException("Invalid message type, WsfBMCueMessage set to wrap non-cue message.");
   }
}

WsfBMCueMessage::WsfBMCueMessage(const WsfBMCueMessage& aSrc)
   : WsfMessage(aSrc)
   , WsfBMManagedTypeWrapper(WsfBMCueMessage::GetTypeId(), std::make_shared<il::cueMessage>(*aSrc.GetRawMessagePtr()))
{
}

WsfBMCueMessage::~WsfBMCueMessage()
{
}

WsfBMCueMessage* WsfBMCueMessage::Clone() const
{
   return new WsfBMCueMessage(*this);
}

const char* WsfBMCueMessage::GetScriptClassName() const
{
   return WsfScriptBMCueMessageClass::SCRIPT_CLASS_NAME;
}

const char* WsfBMCueMessage::GetBaseClassName()
{
   return WsfScriptBMCueMessageClass::BASE_CLASS_NAME;
}

WsfStringId WsfBMCueMessage::GetTypeId()
{
   return "WsfBMCueMessage";
}

void WsfBMCueMessage::SetCueTime(const double sim_time_s)
{
   GetMutableRawMessagePtr()->setCueTime(sim_time_s);
}

double WsfBMCueMessage::GetCueTime() const
{
   return GetRawMessagePtr()->getCueTime();
}

void WsfBMCueMessage::SetReferenceTrackID(const WsfTrackId& track_id)
{
   GetMutableRawMessagePtr()->setReferenceTrackID(WsfBMUtils::ConvertTrackId(track_id));
}

const WsfTrackId WsfBMCueMessage::GetReferenceTrackID() const
{
   return WsfBMUtils::ConvertTrackId(GetRawMessagePtr()->getReferenceTrackID());
}

void WsfBMCueMessage::SetLocalTrackID(const WsfTrackId& track_id)
{
   GetMutableRawMessagePtr()->setLocalTrackID(WsfBMUtils::ConvertTrackId(track_id));
}

const WsfTrackId WsfBMCueMessage::GetLocalTrackID() const
{
   return WsfBMUtils::ConvertTrackId(GetRawMessagePtr()->getLocalTrackID());
}

void WsfBMCueMessage::SetInitiatingPlatform(const WsfPlatform* plat)
{
   GetMutableRawMessagePtr()->setInitiatingID(WsfBMUtils::GetUnitIDRecordFromPlatform(plat));
}

const WsfPlatform* WsfBMCueMessage::GetInitiatingPlatform(WsfSimulation* sim_ptr) const
{
   return WsfBMUtils::GetPlatformFromUnitIDRecord(sim_ptr, GetRawMessagePtr()->getInitiatingID());
}

void WsfBMCueMessage::SetCuedSensor(const WsfPlatform* plat, const WsfSensor* sensor)
{
   GetMutableRawMessagePtr()->setCuedUnitID(WsfBMUtils::GetUnitIDRecordFromPlatform(plat));
   GetMutableRawMessagePtr()->setCuedSensorID(WsfBMUtils::GetSensorID(plat, sensor));
}

const WsfPlatform* WsfBMCueMessage::GetCuedPlatform(WsfSimulation* sim_ptr) const
{
   return WsfBMUtils::GetPlatformFromUnitIDRecord(sim_ptr, GetRawMessagePtr()->getCuedUnitID());
}

const WsfSensor* WsfBMCueMessage::GetCuedSensor(WsfSimulation* sim_ptr) const
{
   return WsfBMUtils::GetSensor(GetCuedPlatform(sim_ptr), GetRawMessagePtr()->getCuedSensorID().getIDString());
}

void WsfBMCueMessage::SetCueReason(const il::cueMessage::CueReason reason)
{
   GetMutableRawMessagePtr()->setCueReason(reason);
}

il::cueMessage::CueReason WsfBMCueMessage::GetCueReason() const
{
   return GetRawMessagePtr()->getCueReason();
}

std::string WsfBMCueMessage::GetCueReasonString() const
{
   return il::cueMessage::ReasonToStr(GetCueReason());
}

void WsfBMCueMessage::Print() const
{
   GetRawMessagePtr()->logSTD();
}
