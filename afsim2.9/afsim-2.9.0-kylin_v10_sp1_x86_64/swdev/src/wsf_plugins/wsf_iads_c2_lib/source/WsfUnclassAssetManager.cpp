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
*   File: WsfUnclassAssetManager.cpp
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

#include "WsfUnclassAssetManager.hpp"
#include "WsfScriptUnclassAssetManagerClass.hpp"
#include <WsfBMPluginUtilities.hpp>
#include <UtMemory.hpp>
#include <iadsLib/assetManagerInterface.hpp>

#include <script/WsfScriptManager.hpp>

#include "WsfIADSC2ScenarioExtension.hpp"

WsfUnclassifiedAssetManager::WsfUnclassifiedAssetManager(WsfScenario& aScenario)
   : WsfAssetManager(aScenario, WsfScriptUnclassAssetManagerClass::SCRIPT_CLASS_NAME, "PROCESSOR", std::make_shared<il::AssetManagerInterface>(IADSC2ScenarioExtension::GetGlobalLogger(aScenario)))
{
}

WsfUnclassifiedAssetManager::WsfUnclassifiedAssetManager(WsfScenario& aScenario, const std::shared_ptr<il::AssetManagerInterface>& am_component)
   : WsfAssetManager(aScenario, WsfScriptUnclassAssetManagerClass::SCRIPT_CLASS_NAME, "PROCESSOR", am_component)
{
}

WsfUnclassifiedAssetManager::WsfUnclassifiedAssetManager(const WsfUnclassifiedAssetManager& from)
   : WsfAssetManager(from)
{
}

WsfProcessor* WsfUnclassifiedAssetManager::Clone() const
{
#ifdef IC2DEBUG
   HCL_TRACE_LOGGER() << "WsfUnclassifiedAssetManager::Clone(before)";
#endif
   return new WsfUnclassifiedAssetManager(*this);
}

// Plugin Interface
extern "C" WSF_IADS_C2_LIB_EXPORT void WsfUnclassifiedAssetManager::RegisterScriptTypes(WsfApplication* aApplicationPtr)
{
   if (!IsClassTypeRegistered(aApplicationPtr, WsfScriptUnclassAssetManagerClass::SCRIPT_CLASS_NAME))
   {
      // base class registration
      WsfAssetManager::RegisterScriptTypes(aApplicationPtr);

      auto script_types = aApplicationPtr->GetScriptTypes();
      script_types->Register(ut::make_unique<WsfScriptUnclassAssetManagerClass>(WsfScriptUnclassAssetManagerClass::SCRIPT_CLASS_NAME, script_types));
   }
}

extern "C" WSF_IADS_C2_LIB_EXPORT WsfProcessor* WsfUnclassifiedAssetManager::Create(WsfScenario& aScenario)
{
   std::shared_ptr<il::AssetManagerInterface> am(new il::AssetManagerInterface(IADSC2ScenarioExtension::GetGlobalLogger(aScenario)));

#ifdef IC2DEBUG
   HCL_INFO_LOGGER() << "WsfUnclassifiedAssetManager::Create(): am = " << am;
#endif

   return new WsfUnclassifiedAssetManager(aScenario, am);
}

WSF_IADS_C2_LIB_EXPORT std::unique_ptr<WsfUnclassifiedAssetManager> WsfUnclassifiedAssetManager::CreateUnique(WsfScenario& aScenario)
{
   return ut::make_unique<WsfUnclassifiedAssetManager>(aScenario);
}

bool WsfUnclassifiedAssetManager::ProcessInput(UtInput& aInput)
{
   bool myCommand = false;

   if (WsfAssetManager::ProcessInput(aInput))
   {
      myCommand = true;
   }

   return myCommand;
}

const char* WsfUnclassifiedAssetManager::GetScriptClassName() const
{
   return WsfScriptUnclassAssetManagerClass::SCRIPT_CLASS_NAME;
}

const char* WsfUnclassifiedAssetManager::GetBaseClassName()
{
   return WsfScriptUnclassAssetManagerClass::BASE_CLASS_NAME;
}
