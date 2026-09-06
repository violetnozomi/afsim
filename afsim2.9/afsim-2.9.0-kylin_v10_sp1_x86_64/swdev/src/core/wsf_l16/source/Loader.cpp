// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "Loader.hpp"

#include "wsf_l16_export.h"

#include "ComputerPartTypes.hpp"
#include "ComputerProcessor.hpp"
#include "Interface.hpp"
#include "Messages.hpp"
#include "MessagesFactory.hpp"
#include "ScriptedPartClass.hpp"
#include "UtAtmosphere.hpp"
#include "UtMemory.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfExtension.hpp"
#include "WsfProcessor.hpp"
#include "WsfProcessorTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

void WsfL16_Extension::SimulationCreated(WsfSimulation& aSimulation)
{
   if (mInterfaceSetup.IsRequested())
   {
      aSimulation.RegisterExtension(GetExtensionName(), ut::make_unique<WsfL16::Interface>(mInterfaceSetup));
   }
}

bool WsfL16_Extension::ProcessInput(UtInput& aInput)
{
   return mInterfaceSetup.ProcessInput(aInput);
}

WsfL16_Extension::~WsfL16_Extension() {}

WsfL16_Extension::WsfL16_Extension(WsfScenario& aScenario)
   : mInterfaceSetup(aScenario)
{
   WsfProcessorTypes::Get(aScenario).Add("WSF_LINK16_COMPUTER", ut::make_unique<WsfL16::ComputerProcessor>(aScenario));
   mComputerPartTypes = ut::make_unique<WsfL16::ComputerPartTypes>(aScenario);
}

WsfL16_Extension* WsfL16_Extension::Find(const WsfScenario& aScenario)
{
   return static_cast<WsfL16_Extension*>(aScenario.FindExtension("wsf_l16"));
}

namespace
{
class ApplicationExtension : public WsfApplicationExtension
{
public:
   void ScenarioCreated(WsfScenario& aScenario) override
   {
      aScenario.RegisterExtension(GetExtensionName(), ut::make_unique<WsfL16_Extension>(aScenario));
   }
   void AddedToApplication(WsfApplication& aApplication) override
   {
      WsfL16::Messages::Factory::Initialize(aApplication.GetScriptTypes());
      WsfL16::ScriptedPartClass::RegisterType(*aApplication.GetScriptTypes());
      WsfL16::ComputerProcessor::RegisterType(*aApplication.GetScriptTypes());
   }
   ~ApplicationExtension() override
   {
      WsfL16::Messages::Factory::ResetState();
      WsfL16::Messages::FieldAccessorType::ResetState();
      WsfL16::Messages::MessageAccessorType::ResetState();
   }
};
} // namespace

void WSF_L16_EXPORT Register_wsf_l16(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("wsf_l16"))
   {
      aApplication.RegisterFeature("link-16", "wsf_l16");
      WSF_REGISTER_EXTENSION(aApplication, wsf_mil); // This extension REQUIRES the "wsf_mil" extension
      WSF_REGISTER_EXTENSION(aApplication, wsf_nx);  // This extension REQUIRES the "wsf_nx" extension
      aApplication.RegisterExtension("wsf_l16", ut::make_unique<ApplicationExtension>());
      aApplication.ExtensionDepends("wsf_brawler", "wsf_mil", true);
      aApplication.ExtensionDepends("wsf_brawler", "wsf_nx", true);
   }
}
