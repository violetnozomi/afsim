// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfUCI.hpp"

#include <string>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"

#include "script/WsfScriptUCI_AMTI_ActivityMessage.hpp"
#include "script/WsfScriptUCI_AMTI_CapabilityMessage.hpp"
#include "script/WsfScriptUCI_AMTI_CommandMessage.hpp"
#include "script/WsfScriptUCI_AMTI_CapabilityTypes.hpp"
#include "script/WsfScriptUCI_AMTI_CommandTypes.hpp"
#include "script/WsfScriptUCI_AMTI_CapabilityStatusMessage.hpp"
#include "script/WsfScriptUCI_AMTI_SettingsCommandMessage.hpp"
#include "script/WsfScriptUCI_CommonTypes.hpp"
#include "script/WsfScriptUCI_ControlStatusMessage.hpp"
#include "script/WsfScriptUCI_ControlTypes.hpp"
#include "script/WsfScriptUCI_DMPI_Message.hpp"
#include "script/WsfScriptUCI_EntityMessage.hpp"
#include "script/WsfScriptUCI_ESM_ActivityMessage.hpp"
#include "script/WsfScriptUCI_ESM_CapabilityMessage.hpp"
#include "script/WsfScriptUCI_ESM_CommandMessage.hpp"
#include "script/WsfScriptUCI_ESM_CapabilityTypes.hpp"
#include "script/WsfScriptUCI_ESM_CommandTypes.hpp"
#include "script/WsfScriptUCI_ESM_CapabilityStatusMessage.hpp"
#include "script/WsfScriptUCI_ESM_SettingsCommandMessage.hpp"
#include "script/WsfScriptUCI_Interface.hpp"
#include "script/WsfScriptUCI_POST_ActivityMessage.hpp"
#include "script/WsfScriptUCI_POST_CapabilityMessage.hpp"
#include "script/WsfScriptUCI_POST_CapabilityStatusMessage.hpp"
#include "script/WsfScriptUCI_POST_CapabilityTypes.hpp"
#include "script/WsfScriptUCI_POST_CommandMessage.hpp"
#include "script/WsfScriptUCI_POST_CommandTypes.hpp"
#include "script/WsfScriptUCI_POST_SettingsCommandMessage.hpp"
#include "script/WsfScriptUCI_ControlRequestMessage.hpp"
#include "script/WsfScriptUCI_ControlRequestStatusMessage.hpp"
#include "script/WsfScriptUCI_ControlTypes.hpp"
//#include "script/WsfScriptUCI_VolumeTypes.hpp"
#include "script/WsfScriptUCI_StrikeActivityMessage.hpp"
#include "script/WsfScriptUCI_StrikeCapabilityMessage.hpp"
#include "script/WsfScriptUCI_StrikeCapabilityStatusMessage.hpp"
#include "script/WsfScriptUCI_StrikeCommandMessage.hpp"
#include "script/WsfScriptUCI_StrikeCommandStatusMessage.hpp"
#include "script/WsfScriptUCI_StrikeCommandTypes.hpp"
#include "script/WsfScriptUCI_SubsystemStatusMessage.hpp"
#include "script/WsfScriptUCI_SystemStatusMessage.hpp"
#include "UtScriptTypes.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfPlugin.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfUCI_ComponentTypes.hpp"
#include "WsfUCI_InterfaceExtension.hpp"

namespace wsf
{

namespace
{
   // The actual application extension
   class ApplicationExtension : public WsfApplicationExtension
   {
      public:
         void AddedToApplication(WsfApplication& aApplication) override;
         void ScenarioCreated(WsfScenario& aScenario) override;
   };

   void ApplicationExtension::AddedToApplication(WsfApplication& aApplication)
   {
   }

   void ApplicationExtension::ScenarioCreated(WsfScenario& aScenario)
   {
      aScenario.RegisterExtension(GetExtensionName(), ut::make_unique<OMS_UCI_Extension>());
   }
}

void WSF_OMS_UCI_EXPORT Register_wsf_uci(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered(OMS_UCI_Extension::GetExtensionName()))
   {
      aApplication.RegisterFeature(OMS_UCI_Extension::GetExtensionName());
      aApplication.RegisterFeature("oms_uci", OMS_UCI_Extension::GetExtensionName());
      aApplication.RegisterExtension(OMS_UCI_Extension::GetExtensionName(), ut::make_unique<ApplicationExtension>());
      aApplication.ExtensionDepends(OMS_UCI_Extension::GetExtensionName(), "wsf_mil", true);
   }
}

OMS_UCI_Extension::~OMS_UCI_Extension()
{
   GetInterface()->Shutdown();
}

void OMS_UCI_Extension::AddedToScenario()
{
   auto uciTypesPtr = ut::make_unique<wsf::UCI_ComponentTypes>(GetScenario());
   mUCI_ComponentTypesPtr = uciTypesPtr.get();
   GetScenario().AddTypeList(std::move(uciTypesPtr));
   wsf::UCI_ComponentTypes::RegisterComponentFactory(GetScenario());

   // Register simple script types before messages, as argument lists for script message methods require that
   // referred-to simple types be already registered.
   // Common
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_InterfaceClass>("UCI_Interface", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_ActivityIdClass>("UCI_ActivityId", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_CapabilityIdClass>("UCI_CapabilityId", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_SystemIdClass>("UCI_SystemId", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_CommandIdClass>("UCI_CommandId", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_EntityIdClass>("UCI_EntityId", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_CapabilityStatusTypeClass>("UCI_CapabilityStatus", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_CapabilityStateTypeClass>("UCI_CapabilityState", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_ControlTypeClass>("UCI_Control", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_ControlRequestTypeClass>("UCI_ControlRequest", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_MessageHeaderClass>("UCI_MessageHeader", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_LOS_ReferenceTypeClass>("UCI_LOS_Reference", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_LOS_TypeClass>("UCI_LOS", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_ElevationScanStabilizationTypeClass>("UCI_ElevationScanStabilization", GetScenario().GetScriptTypes()));

   //AMTI
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_AirTargetVolumeCommandTypeClass>("UCI_AirTargetVolumeCommand", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_AMTI_CapabilityTypeClass>("UCI_AMTI_Capability", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_AMTI_ActivityCommandTypeClass>("UCI_AMTI_ActivityCommand", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_AMTI_CapabilityCommandTypeClass>("UCI_AMTI_CapabilityCommand", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_AMTI_CommandTypeClass>("UCI_AMTI_Command", GetScenario().GetScriptTypes()));

   //ESM
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_SubCapabilityDetailsTypeClass>("UCI_SubCapabilityDetails", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_SubCapabilitySelectionTypeClass>("UCI_SubCapabilitySelection", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_ESM_CapabilityTypeClass>("UCI_ESM_Capability", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_ESM_ActivityCommandTypeClass>("UCI_ESM_ActivityCommand", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_ESM_CapabilityCommandTypeClass>("UCI_ESM_CapabilityCommand", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_ESM_CommandTypeClass>("UCI_ESM_Command", GetScenario().GetScriptTypes()));

   //POST
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_POST_CapabilityTypeClass>("UCI_POST_Capability", GetScenario().GetScriptTypes()));
   //GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_POST_AirVolumeSensorReferencedTypeClass>("UCI_POST_AirVolumeSensorReferenced", GetScenario().GetScriptTypes())); // place before UCI_Pointing
   //GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_POST_AirTargetVolumeTypeClass>("UCI_POST_AirTargetVolume", GetScenario().GetScriptTypes())); // place before UCI_Pointing
   //GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_POST_AirTargetVolumeCommandTypeClass>("UCI_POST_AirTargetVolumeCommand", GetScenario().GetScriptTypes())); // place before UCI_Pointing
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_PointingTypeClass>("UCI_Pointing", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_POST_ActivityCommandTypeClass>("UCI_POST_ActivityCommand", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_POST_CapabilityCommandTypeClass>("UCI_POST_CapabilityCommand", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_POST_CapabilityStatusTypeClass>("UCI_POST_CapabilityStatus", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_POST_CommandTypeClass>("UCI_POST_Command", GetScenario().GetScriptTypes()));


   //WEAPON
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_StoreCategoryEnumClass>("UCI_StoreCategory", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_DMPI_IdClass>("UCI_DMPI_ID", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_DMPI_MessageClass>("UCI_DMPI_Message", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_DMPI_StatusMessageClass>("UCI_DMPI_StatusMessage", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_StrikeActivityTypeClass>("UCI_StrikeActivity", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_StrikeWeaponCommandTypeClass>("UCI_StrikeWeaponCommand", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_StrikeActivityCommandTypeClass>("UCI_StrikeActivityCommand", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_StrikeCapabilityCommandTypeClass>("UCI_StrikeCapabilityCommand", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_StrikeCommandTypeClass>("UCI_StrikeCommand", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_StrikeCapabilityTypeClass>("UCI_StrikeCapability", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_StrikeCapabilityStatusTypeClass>("UCI_StrikeCapabilityStatus", GetScenario().GetScriptTypes()));

   // Register message types last.
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_AMTI_ActivityMessageClass>("UCI_AMTI_ActivityMessage", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_AMTI_CapabilityMessageClass>("UCI_AMTI_CapabilityMessage", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_AMTI_CapabilityStatusMessageClass>("UCI_AMTI_CapabilityStatusMessage", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_AMTI_CommandMessageClass>("UCI_AMTI_CommandMessage", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_AMTI_SettingsCommandMessageClass>("UCI_AMTI_SettingsCommandMessage", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_ControlStatusMessageClass>("UCI_ControlStatusMessage", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_EntityMessageClass>("UCI_EntityMessage", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_ESM_ActivityMessageClass>("UCI_ESM_ActivityMessage", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_ESM_CapabilityMessageClass>("UCI_ESM_CapabilityMessage", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_ESM_CommandMessageClass>("UCI_ESM_CommandMessage", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_ESM_CapabilityStatusMessageClass>("UCI_ESM_CapabilityStatusMessage", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_ESM_SettingsCommandMessageClass>("UCI_ESM_SettingsCommandMessage", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_POST_ActivityMessageClass>("UCI_POST_ActivityMessage", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_POST_CommandMessageClass>("UCI_POST_CommandMessage", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_POST_CapabilityMessageClass>("UCI_POST_CapabilityMessage", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_POST_CapabilityStatusMessageClass>("UCI_POST_CapabilityStatusMessage", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_POST_SettingsCommandMessageClass>("UCI_POST_SettingsCommandMessage", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_ControlRequestMessageClass>("UCI_ControlRequestMessage", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_ControlRequestStatusMessageClass>("UCI_ControlRequestStatusMessage", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_StrikeActivityMessageClass>("UCI_StrikeActivityMessage", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_StrikeCapabilityMessageClass>("UCI_StrikeCapabilityMessage", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_StrikeCapabilityStatusMessageClass>("UCI_StrikeCapabilityStatusMessage", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_StrikeCommandMessageClass>("UCI_StrikeCommandMessage", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_StrikeCommandStatusMessageClass>("UCI_StrikeCommandStatusMessage", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_SubsystemStatusMessageClass>("UCI_SubsystemStatusMessage", GetScenario().GetScriptTypes()));
   GetScenario().GetScriptTypes()->Register(ut::make_unique<script::UCI_SystemStatusMessageClass>("UCI_SystemStatusMessage", GetScenario().GetScriptTypes()));
}

void OMS_UCI_Extension::SimulationCreated(WsfSimulation& aSimulation)
{
   if (GetInterface()->IsConfigured())
   {
      if (!GetInterface()->Initialize(aSimulation))
      {
         throw UtException("Initialization of UCI_Interface failed.");
      }
      aSimulation.RegisterExtension(UCI_InterfaceExtension::GetExtensionName(), ut::make_unique<wsf::UCI_InterfaceExtension>(*mInterfaceExtensionPtr));
   }
}

bool OMS_UCI_Extension::ProcessInput(UtInput& aInput)
{
   bool myCommand = GetInterface()->ProcessInput(aInput);
   if (!myCommand)
   {
      myCommand = WsfScenarioExtension::ProcessInput(aInput);
   }
   return myCommand;
}

std::shared_ptr<wsf::UCI_Interface> OMS_UCI_Extension::GetInterface()
{
   if (mInterfacePtr == nullptr)
   {
      mInterfacePtr = std::make_shared<UCI_Interface>();
   }
   return mInterfacePtr;
}

// =================================================================================================
//! Return a reference to the UCI scenario extension.
//static
OMS_UCI_Extension& OMS_UCI_Extension::Get(const WsfScenario& aScenario)
{
   WsfScenarioExtension* extensionPtr = aScenario.FindExtension(GetExtensionName());
   return static_cast<OMS_UCI_Extension&>(*extensionPtr);
}

extern "C"
{
   //! This method is called when the plugin is loaded to ensure that the plugin and the executable loading it were built with
   //! the same version of the plugin API.
   UT_PLUGIN_EXPORT void WsfPluginVersion(UtPluginVersion& aVersion)
   {
      aVersion = UtPluginVersion(WSF_PLUGIN_API_MAJOR_VERSION,
                                 WSF_PLUGIN_API_MINOR_VERSION,
                                 WSF_PLUGIN_API_COMPILER_STRING);
   }
   //! This method is called when the plugin is loaded. It must have exactly this signature (name and parameter) to succeed.
   //! As we only have an application reference at load time, one usually registers an application extension, within which the
   //! ScenarioCreated method can be overridden to gain access to a scenario.  If one also needs
   //! access to the simulation, one should also instantiate and register a simulation extension by overriding
   //! the SimulationCreated method in the scenario extension.
   UT_PLUGIN_EXPORT void WsfPluginSetup(WsfApplication& aApplication)
   {
      Register_wsf_uci(aApplication);
   }
}

} // wsf
