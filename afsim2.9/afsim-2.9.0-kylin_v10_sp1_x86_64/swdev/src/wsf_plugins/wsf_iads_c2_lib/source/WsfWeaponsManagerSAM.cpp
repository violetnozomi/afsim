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
*   File: WsfWeaponsManagerSAM.cpp
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

#include <WsfWeaponsManagerSAM.hpp>
#include <WsfScriptWeaponsManagerSAMClass.hpp>
#include <WsfBMPluginUtilities.hpp>
#include <UtMemory.hpp>
#include <iadsLib/weaponsManagerSAM.hpp>
#include <WsfBMMOELogger.hpp>

WsfWeaponsManagerSAM::WsfWeaponsManagerSAM(WsfScenario& aScenario, const std::shared_ptr<il::WeaponsManagerInterface>& wm_component)
   : WsfBMWeaponsManager(aScenario, WsfScriptWeaponsManagerSAMClass::SCRIPT_CLASS_NAME, "PROCESSOR", wm_component)
   , m_engagement_mod(new WsfWMSAMEngagementMod(aScenario, this))
{
   m_wm_component->setWeaponEngagementModule(this->m_engagement_mod);
}

WsfWeaponsManagerSAM::WsfWeaponsManagerSAM(const WsfWeaponsManagerSAM& from)
   : WsfBMWeaponsManager(from)
   , m_engagement_mod(new WsfWMSAMEngagementMod(from.GetScenario(), this))
{
   m_wm_component->setWeaponEngagementModule(this->m_engagement_mod);
}

WsfProcessor* WsfWeaponsManagerSAM::Clone() const
{
#ifdef IC2DEBUG
   HCL_TRACE_LOGGER() << "WsfWeaponsManagerSAM::Clone(before)";
#endif
   return new WsfWeaponsManagerSAM(*this);
}

// Plugin Interface
extern "C" WSF_IADS_C2_LIB_EXPORT void WsfWeaponsManagerSAM::RegisterScriptTypes(WsfApplication* aApplicationPtr)
{
   if (!IsClassTypeRegistered(aApplicationPtr, WsfScriptWeaponsManagerSAMClass::SCRIPT_CLASS_NAME))
   {
      // base class registration
      WsfBMWeaponsManager::RegisterScriptTypes(aApplicationPtr);

      auto script_types = aApplicationPtr->GetScriptTypes();
      script_types->Register(ut::make_unique<WsfScriptWeaponsManagerSAMClass>(WsfScriptWeaponsManagerSAMClass::SCRIPT_CLASS_NAME, script_types));
   }
}

extern "C" WSF_IADS_C2_LIB_EXPORT WsfProcessor* WsfWeaponsManagerSAM::Create(WsfScenario& aScenario)
{
   std::shared_ptr<il::weaponsManagerSAM> wm(new il::weaponsManagerSAM());

#ifdef IC2DEBUG
   HCL_INFO_LOGGER() << "WsfWeaponsManagerSAM::Create(): wm = " << wm;
#endif

   return new WsfWeaponsManagerSAM(aScenario, wm);
}

WSF_IADS_C2_LIB_EXPORT std::unique_ptr<WsfWeaponsManagerSAM> WsfWeaponsManagerSAM::CreateUnique(WsfScenario& aScenario)
{
   return ut::make_unique<WsfWeaponsManagerSAM>(aScenario);
}
// End Plugin Interface

bool WsfWeaponsManagerSAM::ProcessInput(UtInput& aInput)
{
   bool myCommand = false;

   if (WsfBMWeaponsManager::ProcessInput(aInput) ||
       m_engagement_mod->ProcessInput(aInput))
   {
      myCommand = true;
   }

   return myCommand;
}

const char* WsfWeaponsManagerSAM::GetScriptClassName() const
{
   return WsfScriptWeaponsManagerSAMClass::SCRIPT_CLASS_NAME;
}

bool WsfWeaponsManagerSAM::Initialize(double aSimTime)
{
   bool ok = WsfBMWeaponsManager::Initialize(aSimTime);

   ok &= m_engagement_mod->Initialize(aSimTime);

   return ok;
}

bool WsfWeaponsManagerSAM::Initialize2(double aSimTime)
{
   bool ok = WsfBMWeaponsManager::Initialize2(aSimTime);

   ok &= m_engagement_mod->Initialize2(aSimTime);

   return ok;
}

void WsfWeaponsManagerSAM::Update(double aSimTime)
{
   WsfBMWeaponsManager::Update(aSimTime);
}

void WsfWeaponsManagerSAM::SystemicCANTCOAssignmentsForWeapon(WsfStringId name)
{
   auto am = GetCoreAMRef();
   il::moeLoggingInterface* logger = IADSC2SimulationExtension::Find(*GetSimulation())->GetLogger();
   auto assignments = am->getOurAssignments(*logger);

   for (auto it = assignments.begin(); it != assignments.end(); ++it)
   {
      auto assignment = *it;
      if (assignment->getAssignedID().getID() == am->getOwnID().getID() && assignment->getAssignedID().getSubIDString() == name)
      {
         am->cantcoAssignment(*logger, assignment, "Cancel assignment for weapon " + name, true, false);
      }
   }
}
