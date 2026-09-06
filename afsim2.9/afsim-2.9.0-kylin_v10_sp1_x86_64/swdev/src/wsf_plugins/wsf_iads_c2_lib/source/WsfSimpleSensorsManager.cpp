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
*   File: WsfSimpleSensorsManager.cpp
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

#include <WsfSimpleSensorsManagerImpl.hpp>
#include <WsfSimpleSensorsManager.hpp>
#include <WsfBMPluginUtilities.hpp>
#include <WsfScriptSimpleSensorsManagerClass.hpp>
#include <UtMemory.hpp>

WsfSimpleSensorsManager::WsfSimpleSensorsManager(WsfScenario& aScenario,
                                                 const std::shared_ptr<il::SensorsManagerInterface>& sm_component)
   : WsfBMSensorsManager(aScenario, WsfScriptSimpleSensorsManagerClass::SCRIPT_CLASS_NAME, "PROCESSOR", sm_component)
{
}

WsfSimpleSensorsManager::WsfSimpleSensorsManager(WsfScenario& scenario,
                                                 const std::string& partName,
                                                 const std::shared_ptr<il::SensorsManagerInterface>& smInterface)
   : WsfBMSensorsManager(scenario, partName, "PROCESSOR", smInterface)
{
}

WsfSimpleSensorsManager::WsfSimpleSensorsManager(const WsfSimpleSensorsManager& aSrc)
   : WsfBMSensorsManager(aSrc)
{
}

WsfProcessor* WsfSimpleSensorsManager::Clone() const
{
#ifdef IC2DEBUG
   HCL_TRACE_LOGGER() << "WsfSimpleSensorsManager::Clone(before)";
#endif
   return new WsfSimpleSensorsManager(*this);
}

// Plugin Interface
extern "C" WSF_IADS_C2_LIB_EXPORT void WsfSimpleSensorsManager::RegisterScriptTypes(WsfApplication* aApplicationPtr)
{
   // Register the type if it isn't already
   if (!IsClassTypeRegistered(aApplicationPtr, WsfScriptSimpleSensorsManagerClass::SCRIPT_CLASS_NAME))
   {
      RegisterBaseTypes(aApplicationPtr);

      auto script_types = aApplicationPtr->GetScriptTypes();
      script_types->Register(ut::make_unique<WsfScriptSimpleSensorsManagerClass>(WsfScriptSimpleSensorsManagerClass::SCRIPT_CLASS_NAME, script_types));
   }
}

extern "C" WSF_IADS_C2_LIB_EXPORT WsfProcessor* WsfSimpleSensorsManager::Create(WsfScenario& aScenario)
{
   std::shared_ptr<WsfSimpleSensorsManagerImpl> sm(new WsfSimpleSensorsManagerImpl);

#ifdef IC2DEBUG
   HCL_INFO_LOGGER() << "WsfSimpleSensorsManager::Create(): sm = " << sm;
#endif

   return new WsfSimpleSensorsManager(aScenario, sm);
}

WSF_IADS_C2_LIB_EXPORT std::unique_ptr<WsfSimpleSensorsManager> WsfSimpleSensorsManager::CreateUnique(WsfScenario& aScenario)
{
   return ut::make_unique<WsfSimpleSensorsManager>(aScenario);
}

const char* WsfSimpleSensorsManager::GetScriptClassName() const
{
   return WsfScriptSimpleSensorsManagerClass::SCRIPT_CLASS_NAME;
}

bool WsfSimpleSensorsManager::ProcessInput(UtInput& aInput)
{
   bool myCommand = false;

   if (WsfBMSensorsManager::ProcessInput(aInput))
   {
      myCommand = true;
   }

   return myCommand;
}

bool WsfSimpleSensorsManager::Initialize(double aSimTime)
{
   return WsfBMSensorsManager::Initialize(aSimTime);
}

bool WsfSimpleSensorsManager::Initialize2(double aSimTime)
{
   return WsfBMSensorsManager::Initialize2(aSimTime);
}

void WsfSimpleSensorsManager::Update(double aSimTime)
{
   WsfBMSensorsManager::Update(aSimTime);
}
