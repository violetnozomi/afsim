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
*   File: WsfBMDisseminateC2.cpp
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
*   Abstract: This is a core class implementation for a HELIOS-like C2 dissemination
*    model. It may be used or extended to meet individual IADS needs.
*
*******************************************************************************/

#include <string>
#include <UtInputBlock.hpp>
#include <UtMemory.hpp>

#include <script/WsfScriptManager.hpp>
#include <WsfComm.hpp>
#include "WsfCommNetworkManager.hpp"
#include "WsfCommUtil.hpp"
#include <WsfCommandChain.hpp>

#include <iadsLib/assetManagerInterface.hpp>
#include <iadsLib/disseminateC2Interface.hpp>
#include "WsfBMDisseminateC2.hpp"
#include <WsfBMPluginUtilities.hpp>
#include <WsfScriptDisseminateC2Class.hpp>
#include <WsfBMUtils.hpp>
#include "WsfIADSC2ScenarioExtension.hpp"

namespace
{
   // Note that we don't distinguish failure return from no-entry returns - we just log an error
   bool ProcessRouteString(const std::string& route_str, il::DisseminateC2Interface::eRoutingKey& key, std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger)
   {
      bool got_route = false;

      if (route_str == "none")
      {
         got_route = false;
      }
      else if (route_str == "subordinate")
      {
         got_route = true;
         key = il::DisseminateC2Interface::E_SUBORDINATE;
      }
      else if (route_str == "peer")
      {
         got_route = true;
         key = il::DisseminateC2Interface::E_PEER;
      }
      else if (route_str == "commander")
      {
         got_route = true;
         key = il::DisseminateC2Interface::E_COMMANDER;
      }
      else if (route_str == "dynamic")
      {
         got_route = true;
         key = il::DisseminateC2Interface::E_DYNAMIC;
      }
      else
      {
         HCL_ERROR_LOGGER(aGlobalLogger) << "Invalid routing table route type: " << route_str;
      }

      return got_route;
   }

   // lifted from WsfTaskManagerBase
   bool PlatformListContains(const WsfCommandChain::PlatformList& aPlatformList,
                             const WsfPlatform*                   aPlatformPtr)
   {
      for (auto pli = aPlatformList.begin(); pli != aPlatformList.end(); ++pli)
      {
         if (aPlatformPtr == (*pli))
         {
            return true;
         }
      }
      return false;
   }
}

WsfBMDisseminateC2::WsfBMDisseminateC2(WsfScenario&       aScenario,
                                       const std::string& aPartClass,
                                       const std::string& aPartVarName,
                                       const std::shared_ptr<il::DisseminateC2Interface>&  dc2_component)
   : WsfScriptProcessor(aScenario, aPartClass, aPartVarName)
   , WsfC2ComponentContainer(dc2_component)
   , WsfScriptOverridableProcessor(false) // by default, asset manager drives everything else
{
   SetType(WsfStringId(WsfScriptDisseminateC2Class::BASE_CLASS_NAME));
}

WsfBMDisseminateC2::WsfBMDisseminateC2(const WsfBMDisseminateC2& aSrc)
   : WsfScriptProcessor(aSrc)
   , WsfC2ComponentContainer(aSrc)
   , WsfScriptOverridableProcessor(aSrc)
   , m_default_dissem_impl(aSrc.m_default_dissem_impl)
{
   m_dc2_component.reset(aSrc.m_dc2_component->Clone());
}

WsfBMDisseminateC2::~WsfBMDisseminateC2()
{
}

WsfProcessor* WsfBMDisseminateC2::Clone() const
{
   return new WsfBMDisseminateC2(*this);
}

// Plugin Interface
extern "C" WSF_IADS_C2_LIB_EXPORT void WsfBMDisseminateC2::RegisterScriptTypes(WsfApplication* aApplicationPtr)
{
   // Register the type if it isn't already
   if (!IsClassTypeRegistered(aApplicationPtr, WsfScriptDisseminateC2Class::SCRIPT_CLASS_NAME))
   {
      RegisterBaseTypes(aApplicationPtr);

      auto script_types = aApplicationPtr->GetScriptTypes();
      script_types->Register(ut::make_unique<WsfScriptDisseminateC2Class>(WsfScriptDisseminateC2Class::SCRIPT_CLASS_NAME, script_types));
   }
}

extern "C" WSF_IADS_C2_LIB_EXPORT WsfProcessor* WsfBMDisseminateC2::Create(WsfScenario& aScenario)
{
   std::shared_ptr<il::DisseminateC2Interface> dc2 = std::shared_ptr<il::DisseminateC2Interface>(new il::DisseminateC2Interface);

   return new WsfBMDisseminateC2(aScenario, WsfScriptDisseminateC2Class::SCRIPT_CLASS_NAME, "PROCESSOR", dc2);
}

WSF_IADS_C2_LIB_EXPORT std::unique_ptr<WsfBMDisseminateC2> WsfBMDisseminateC2::CreateUnique(WsfScenario& aScenario)
{
   return ut::make_unique<WsfBMDisseminateC2>(aScenario);
}

const char* WsfBMDisseminateC2::GetScriptClassName() const
{
   return WsfScriptDisseminateC2Class::SCRIPT_CLASS_NAME;
}

const char* WsfBMDisseminateC2::GetBaseClassName()
{
   return WsfScriptDisseminateC2Class::BASE_CLASS_NAME;
}

bool WsfBMDisseminateC2::ProcessInput(UtInput& aInput)
{
   bool myCommand = false;

   // Attempt to parse scripting for common bm elements
   if (ProcessDisseminateC2Input(aInput) ||
       m_default_dissem_impl.ProcessInput(aInput) ||
       WsfScriptOverridableProcessor::ProcessInput(aInput) ||
       WsfScriptProcessor::ProcessInput(aInput))
   {
      myCommand = true;
   }

   return myCommand;
}

bool WsfBMDisseminateC2::ProcessDisseminateC2Input(UtInput& aInput)
{
   bool processed = true;

   std::string command = aInput.GetCommand();

   if (command == "routing_style")
   {
      std::string routing_style;
      aInput.ReadValue(routing_style);
      if (routing_style == "next_unit")
      {
         m_dc2_component->setRoutingStyle(il::DisseminateC2Interface::E_NEXT_UNIT);
      }
      else if (routing_style == "next_c2")
      {
         m_dc2_component->setRoutingStyle(il::DisseminateC2Interface::E_NEXT_C2);
      }
      else if (routing_style == "direct")
      {
         m_dc2_component->setRoutingStyle(il::DisseminateC2Interface::E_DIRECT);
      }
      else
      {
         // unknown style
         HCL_WARN_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())) << "Unknown routing style: " << routing_style;
         processed = false;
      }
   }
   else if (command == "routing_table")
   {
      UtInputBlock block(aInput, "end_routing_table");

      while (block.ReadCommand(command) && processed)
      {
         if (command == "track_updates")
         {
            std::string route_type_str;
            il::DisseminateC2Interface::eRoutingKey route_enum;
            block.GetInput().ReadValue(route_type_str);
            if (ProcessRouteString(route_type_str, route_enum, IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())))
            {
               m_dc2_component->AppendTableEntry(il::DisseminateC2Interface::E_TRACK_UPDATE, route_enum);
            }
         }
         else if (command == "assign_track_updates")
         {
            std::string route_type_str;
            il::DisseminateC2Interface::eRoutingKey route_enum;
            block.GetInput().ReadValue(route_type_str);
            if (ProcessRouteString(route_type_str, route_enum, IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())))
            {
               m_dc2_component->AppendTableEntry(il::DisseminateC2Interface::E_TRACKA_UPDATE, route_enum);
            }
         }
         else if (command == "assignments")
         {
            std::string route_type_str;
            il::DisseminateC2Interface::eRoutingKey route_enum;
            block.GetInput().ReadValue(route_type_str);
            if (ProcessRouteString(route_type_str, route_enum, IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())))
            {
               m_dc2_component->AppendTableEntry(il::DisseminateC2Interface::E_ASSIGNMENT, route_enum);
            }
         }
         else if (command == "assignment_status")
         {
            std::string route_type_str;
            il::DisseminateC2Interface::eRoutingKey route_enum;
            block.GetInput().ReadValue(route_type_str);
            if (ProcessRouteString(route_type_str, route_enum, IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())))
            {
               m_dc2_component->AppendTableEntry(il::DisseminateC2Interface::E_ASSIGNMENT_STATUS, route_enum);
            }
         }
         else if (command == "assignment_cancel")
         {
            std::string route_type_str;
            il::DisseminateC2Interface::eRoutingKey route_enum;
            block.GetInput().ReadValue(route_type_str);
            if (ProcessRouteString(route_type_str, route_enum, IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())))
            {
               m_dc2_component->AppendTableEntry(il::DisseminateC2Interface::E_ASSIGNMENT_CANCEL, route_enum);
            }
         }
         else if (command == "sensor_cue")
         {
            std::string route_type_str;
            il::DisseminateC2Interface::eRoutingKey route_enum;
            block.GetInput().ReadValue(route_type_str);
            if (ProcessRouteString(route_type_str, route_enum, IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())))
            {
               m_dc2_component->AppendTableEntry(il::DisseminateC2Interface::E_CUE, route_enum);
            }
         }
         else if (command == "status")
         {
            std::string route_type_str;
            il::DisseminateC2Interface::eRoutingKey route_enum;
            block.GetInput().ReadValue(route_type_str);
            if (ProcessRouteString(route_type_str, route_enum, IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())))
            {
               m_dc2_component->AppendTableEntry(il::DisseminateC2Interface::E_STATUS, route_enum);
            }
         }
         else
         {
            processed = false;
         }
      }
   }
   else // unknown block
   {
      processed = false;
   }

   return processed;
}

bool WsfBMDisseminateC2::Initialize(double aSimTime)
{
   bool success = true;

   if (!OverrideInternalInit())
   {
      m_default_dissem_impl.Initialize(this);
   }

   // base initialization - always happen
   success &= WsfScriptProcessor::Initialize(aSimTime);

   return success;
}

bool WsfBMDisseminateC2::Initialize2(double aSimTime)
{
   bool success = true;

   if (!OverrideInternalInit2())
   {
      if (!SetComponent(WsfBMUtils::FindAttachedAssetManagerRef(this->GetPlatform())))
      {
         HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())) << "A C2 dissemination processor requires a processor of type WSF_ASSET_MANAGER to be employed on the same platform";
         return success &= false;
      }

      m_default_dissem_impl.Initialize2(this);
   }

   // base initialization - always happen
   // run the script processor initialize2 after we've done processor hooking to allow the script to override it.
   success &= WsfScriptProcessor::Initialize2(aSimTime);

   return success;
}

void WsfBMDisseminateC2::Update(double aSimTime)
{
   if (!OverrideInternalOnUpdate() && AllowAutonomousUpdates())
   {
      Run(aSimTime);
   }
   else
   {
      // base class updates always occur
      WsfScriptProcessor::Update(aSimTime);
   }
}

void WsfBMDisseminateC2::Run(const double aSimTime)
{
   m_default_dissem_impl.on_update(aSimTime, GetPlatform(), this);
   // base class updates always occur
   WsfScriptProcessor::Update(aSimTime);
}

bool WsfBMDisseminateC2::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   bool suppress_message = false;

   if (!OverrideInternalOnMessage())
   {
      suppress_message = m_default_dissem_impl.on_message(aSimTime, aMessage, this);
   }

   // base class processing always occur
   if (!suppress_message)
   {
      suppress_message &= WsfScriptProcessor::ProcessMessage(aSimTime, aMessage);
   }

   return suppress_message;
}

bool WsfBMDisseminateC2::ReceiveMessage(double aSimTime, const WsfMessage& aMessage)
{
   bool success = true;

   if (!OverrideInternalReceiveMessage())
   {
      // nothing to do
   }

   success &= WsfScriptProcessor::ReceiveMessage(aSimTime, aMessage);

   return success;
}

bool WsfBMDisseminateC2::SendMessage(double aSimTime, const WsfMessage& aMessage, const WsfPlatform* destination)
{
   // destination is us?
   if (destination == this->GetPlatform())
   {
      ProcessMessage(aSimTime, aMessage); // we don't get an error code out of this, just a suppress/don't suppress flag. It must handle all error conditions itself
      return true;
   }

   // TTD: fixme for a common base class message that can add some routing info for us aMessage.SetCommName (commName)

   // ensure recipient exists
   if (!destination || !GetSimulation()->GetPlatformByIndex(destination->GetIndex()))
   {
      HCL_WARN_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())) << "***** WARNING: T=" << aSimTime << " Unable to send message from "
                        << GetPlatform()->GetName() << '.' << GetName()
                        << "; recipient no longer exists" << std::endl;
      return false;
   }

   // determine comm device to use
   auto commDevice = SelectCommDevice(destination, WsfStringId(0));
   // check comm device
   if (!commDevice)
   {
      HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())) << "***** ERROR: T=" << aSimTime << " Unable to send message from "
                         << GetPlatform()->GetName() << '.' << GetName()
                         << " to " << destination->GetName()
                         << "; comm device does not exist" << std::endl;
      return false;
   }

   WsfCommandChain* ccPtr = 0;
   //unsigned int ccCount = GetPlatform()->GetCommandChainCount();
   unsigned int ccCount = GetPlatform()->GetComponentCount<WsfCommandChain>();
   for (unsigned int ccIndex = 0; ccIndex < ccCount; ++ccIndex)
   {
      //ccPtr = GetPlatform()->GetCommandChainEntry(ccIndex);
      ccPtr = GetPlatform()->GetComponentEntry<WsfCommandChain>(ccIndex);
      if (destination == ccPtr->GetCommander() ||
          PlatformListContains(ccPtr->GetSubordinates(), destination) ||
          PlatformListContains(ccPtr->GetPeers(), destination))
      {
         break;
      }
      ccPtr = 0;
   }

   if (ccPtr)
   {
      aMessage.SetGroup(ccPtr->GetDefaultNameId());
   }
   else
   {
      aMessage.SetGroup("default");
   }

   for (WsfComponentList::RoleIterator<wsf::comm::Comm> it(*destination); !it.AtEnd(); ++it)
   {
      auto commRcvrPtr = *it;
      if (commRcvrPtr->IsTurnedOn() && commRcvrPtr->CanReceive())
      {
         auto networkManagerPtr = GetSimulation()->GetCommNetworkManager();
         if (networkManagerPtr->PathExists(commDevice->GetAddress(), commRcvrPtr->GetAddress()))
         {
            commDevice->Send(aSimTime, ut::clone(aMessage), commRcvrPtr->GetAddress());
            break;
         }
      }
   }

   return true;
}

// this was lifted from WsfTaskManagerBase::SelectCommDevice
wsf::comm::Comm* WsfBMDisseminateC2::SelectCommDevice(const WsfPlatform* aRecipientPtr, WsfStringId  aCommName)
{
   wsf::comm::Comm* commPtr = nullptr;
   if (aCommName.Empty() &&                 // A comm device was not explicitly specified
       (aRecipientPtr != 0) &&              // ... and a recipient was specified
       (aRecipientPtr != GetPlatform()))    // ... and it isn't the same as the sender
   {
      commPtr = wsf::comm::util::SelectDevice(GetPlatform(), const_cast<WsfPlatform*>(aRecipientPtr));
   }
   return commPtr;
}
