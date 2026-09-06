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
*   File: WsfBattleManager.cpp
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
*   Abstract: This is an afsim base class for all battle manager
*     implementation classes to inherit from. It provides necessary common functionality.
*     It's not pure abstract in the sense that we actually instantiate an object of this
*     type purely for type registration purposes with the sole intent of giving AFSIM
*     scripts the ability to up/down cast to and from the inherited types.
*
*******************************************************************************/

#include <iostream>
#include <sstream>

#include <UtInputBlock.hpp>
#include <UtMemory.hpp>
#include <script/WsfScriptManager.hpp>
#include <WsfTrackManager.hpp>

#include "WsfBattleManager.hpp"
#include "WsfScriptBattleManagerClass.hpp"

#include <iadsLib/BattleManagerIface.hpp>
#include <iadsLib/assetManagerInterface.hpp>
#include <iadsLib/NumericUtils.hpp>
#include <WsfBMPluginUtilities.hpp>

#include <WsfBMUtils.hpp>
#include <WsfTaskObserver.hpp>
#include <WsfLocalTrack.hpp>
#include <WsfPlatform.hpp>
#include <WsfAssetManager.hpp>
#include "WsfIADSC2ScenarioExtension.hpp"

WsfProcessor* UninitializedWsfBattleManager(WsfScenario& aScenario, const std::string& calling_context)
{
   std::shared_ptr<il::BattleManagerInterface> bm = std::shared_ptr<il::BattleManagerInterface>(new il::BattleManagerInterface);

#ifdef IC2DEBUG
   HCL_INFO_LOGGER() << calling_context << ": bm = " << bm;
#endif

   return new WsfBattleManager(aScenario, WsfScriptBattleManagerClass::SCRIPT_CLASS_NAME, "PROCESSOR", bm);
}

WsfBattleManager::WsfBattleManager(WsfScenario&       aScenario,
                                   const std::string& aPartClass,
                                   const std::string& aPartVarName,
                                   const std::shared_ptr<il::BattleManagerInterface>& bm_component)
   : WsfScriptProcessor(aScenario, aPartClass, aPartVarName)
   , WsfC2ComponentContainer(bm_component)
   , WsfScriptOverridableProcessor(false) // by default, asset manager drives everything else
   , m_terrain_engine(std::shared_ptr<WsfBMTerrainInterface>(new WsfBMTerrainInterface()))
   , m_intercept_calculator(std::shared_ptr<WsfInterceptCalculator>(new WsfInterceptCalculator()))
{
   SetType(WsfStringId(WsfScriptBattleManagerClass::BASE_CLASS_NAME));

   m_bm_component->setTerrainEngine(m_terrain_engine);
   m_bm_component->SetInterceptCalculator(m_intercept_calculator);
}

// See the notes in the header file about the copy constructor and it's role with
// how AFSIM initialization works
WsfBattleManager::WsfBattleManager(const WsfBattleManager& aSrc)
   : WsfScriptProcessor(aSrc)
   , WsfC2ComponentContainer(aSrc)
   , WsfScriptOverridableProcessor(aSrc)
   , m_default_bm_impl(aSrc.m_default_bm_impl)
   , m_terrain_engine(std::shared_ptr<WsfBMTerrainInterface>(new WsfBMTerrainInterface(*aSrc.m_terrain_engine)))
   , m_intercept_calculator(std::shared_ptr<WsfInterceptCalculator>(new WsfInterceptCalculator()))
{
   m_bm_component.reset(aSrc.m_bm_component->Clone());

   SetupPointerCrossReferences();
}

WsfBattleManager::~WsfBattleManager()
{
}

bool WsfBattleManager::SetSimulation(WsfSimulation* sim_ptr)
{
   return m_terrain_engine->SetSimulation(sim_ptr);
}

WsfProcessor* WsfBattleManager::Clone() const
{
   return new WsfBattleManager(*this);
}

extern "C" WSF_IADS_C2_LIB_EXPORT void WsfBattleManager::RegisterScriptTypes(WsfApplication* aApplicationPtr)
{
   // Register the type if it isn't already
   if (!IsClassTypeRegistered(aApplicationPtr, WsfScriptBattleManagerClass::SCRIPT_CLASS_NAME))
   {
      RegisterBaseTypes(aApplicationPtr);

      auto script_types = aApplicationPtr->GetScriptTypes();
      script_types->Register(ut::make_unique<WsfScriptBattleManagerClass>(WsfScriptBattleManagerClass::SCRIPT_CLASS_NAME, script_types));
   }
}

extern "C" WSF_IADS_C2_LIB_EXPORT WsfProcessor* WsfBattleManager::Create(WsfScenario& aScenario)
{
   return UninitializedWsfBattleManager(aScenario, "WsfBattleManager::Create()");
}

WSF_IADS_C2_LIB_EXPORT std::unique_ptr<WsfBattleManager> WsfBattleManager::CreateUnique(WsfScenario& aScenario)
{
   return ut::make_unique<WsfBattleManager>(aScenario);
}

bool WsfBattleManager::ProcessInput(UtInput& aInput)
{
   bool myCommand = false;

   // Attempt to parse scripting for common bm elements
   if (ProcessAssetManagementInput(aInput) ||
       ProcessBattleManagementInput(aInput) ||
       m_default_bm_impl.ProcessInput(aInput) ||
       WsfScriptOverridableProcessor::ProcessInput(aInput) ||
       WsfScriptProcessor::ProcessInput(aInput))
   {
      myCommand = true;
   }

   return myCommand;
}

const char* WsfBattleManager::GetScriptClassName() const
{
   return WsfScriptBattleManagerClass::SCRIPT_CLASS_NAME;
}

const char* WsfBattleManager::GetBaseClassName()
{
   return WsfScriptBattleManagerClass::BASE_CLASS_NAME;
}

WsfAssetMap* WsfBattleManager::GetAssetMapPtr()
{
   return new WsfAssetMap(&m_am_component->getAssets(), m_am_component->getGlobalLogger());
}

bool WsfBattleManager::Initialize(double aSimTime)
{
   bool success = true;

   if (!OverrideInternalInit())
   {
      m_default_bm_impl.Initialize(this);
   }

   // this is all considered base class processing
   success &= WsfScriptProcessor::Initialize(aSimTime);
   success &= this->SetSimulation(this->GetPlatform()->GetSimulation());

   success &= m_intercept_calculator->Initialize(this);

   return success;
}

bool WsfBattleManager::Initialize2(double aSimTime)
{
   bool success = true;

   if (!OverrideInternalInit2())
   {
      // hook the locally attached asset manager
      if (!SetComponent(WsfBMUtils::FindAttachedAssetManagerRef(this->GetPlatform())))
      {
         HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())) << "A battle manager requires a processor of type WSF_ASSET_MANAGER to be employed on the same platform";
         return success &= false;
      }

      m_default_bm_impl.Initialize2(this);
   }

   // ensure we call the base class initialize after processor hooking to allow the user to override the asset manager selection if
   // need be
   success &= WsfScriptProcessor::Initialize2(aSimTime);

   return success;
}

void WsfBattleManager::Update(double aSimTime)
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

void WsfBattleManager::Run(const double aSimTime)
{
   m_default_bm_impl.on_update(aSimTime, GetPlatform(), this);
   // base class updates always occur
   WsfScriptProcessor::Update(aSimTime);
}

bool WsfBattleManager::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   if (!OverrideInternalOnMessage())
   {
      m_default_bm_impl.on_message(aSimTime, aMessage, this);
   }

   return WsfScriptProcessor::ProcessMessage(aSimTime, aMessage);
}

bool WsfBattleManager::ReceiveMessage(double aSimTime, const WsfMessage& aMessage)
{
   if (!OverrideInternalReceiveMessage())
   {
      // nothing to do
   }

   return WsfScriptProcessor::ReceiveMessage(aSimTime, aMessage);
}

void WsfBattleManager::SetupPointerCrossReferences()
{
   m_bm_component->setTerrainEngine(m_terrain_engine);
   m_bm_component->SetInterceptCalculator(m_intercept_calculator);
}

bool WsfBattleManager::ProcessAssetManagementInput(UtInput& aInput)
{
   bool processed = true;

   std::string command = aInput.GetCommand();

   if (command == "commit_authority")
   {
      bool commit_authority = false;
      aInput.ReadValue(commit_authority);
      m_bm_component->setCommitAuthority(commit_authority);
   }
   else if (command == "project_tracks_by_delays")
   {
      bool project_by_assign_delays = false;
      aInput.ReadValue(project_by_assign_delays);
      m_bm_component->setProjectTracksByDelays(project_by_assign_delays);
   }
   else if (ProcessIFFSettings(aInput))
   {
      // nothing to do - ProcessIFFSettings dispatches its own settings
   }
   else
   {
      processed = false;
   }

   return processed;
}

bool WsfBattleManager::ProcessIFFSettings(UtInput& aInput)
{
   bool processed = true;

   std::string command = aInput.GetCommand();

   if (command == "engage_iff_permissions")
   {
      UtInputBlock block(aInput, "end_" + command);

      while (block.ReadCommand(command) && processed)
      {
         if (command == "unknowns")
         {
            bool engage = false;
            block.GetInput().ReadValue(engage);
            m_bm_component->addTrackIFFROE(il::E_IFF_UNKNOWN, engage);
         }
         else if (command == "neutrals")
         {
            bool engage = false;
            block.GetInput().ReadValue(engage);
            m_bm_component->addTrackIFFROE(il::E_IFF_NEUTRAL, engage);
         }
         else if (command == "friendlies")
         {
            bool engage = false;
            block.GetInput().ReadValue(engage);
            m_bm_component->addTrackIFFROE(il::E_IFF_FRIENDLY, engage);
         }
         else if (command == "hostiles")
         {
            bool engage = false;
            block.GetInput().ReadValue(engage);
            m_bm_component->addTrackIFFROE(il::E_IFF_HOSTILE, engage);
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

bool WsfBattleManager::ProcessBattleManagementInput(UtInput& aInput)
{
   bool myCommand = false;

   // Attempt to parse scripting for common bm elements
   if (ProcessTargetProjectionInput(aInput))
   {
      myCommand = true;
   }

   return myCommand;
}

bool WsfBattleManager::ProcessTargetProjectionInput(UtInput& aInput)
{
   bool processed = true;

   std::string command = aInput.GetCommand();

   if (command == "project_targets_forward")
   {
      double time_s = 60;
      aInput.ReadValueOfType(time_s, UtInput::cTIME);
      m_bm_component->setTargetProjectionTime(time_s);
   }
   else if (command == "by")
   {
      double time_s = 10;
      aInput.ReadValueOfType(time_s, UtInput::cTIME);

      if (NumericUtils::GreaterThan(time_s, 0.0))
      {
         m_bm_component->setTargetProjectionTimeIncrement(time_s);
      }
      else
      {
         processed = false;
         HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(GetScenario())) << GetName() << ": WsfBattleManager::ProcessTargetProjectionInput(): Target projection increment time must be > 0. Value = " << time_s;
      }
   }
   else // unknown block
   {
      processed = false;
   }

   return processed;
}
