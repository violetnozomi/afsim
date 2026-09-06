// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#include <script/WsfScriptContext.hpp>
#include <script/WsfScriptManager.hpp>
#include <script/WsfScriptDefs.hpp>
#include "WsfSimulation.hpp"

#include "WsfScriptBMWeaponInterface.hpp"
#include "WsfBMWeaponInterface.hpp"
#include "WsfBMAssetRecord.hpp"
#include "WsfBMZoneRecord.hpp"
#include "WsfIADSC2ScenarioExtension.hpp"

WsfScriptBMWeaponInterface::WsfScriptBMWeaponInterface(const std::string& aClassName, UtScriptTypes*     aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfBMWeaponInterface");

   mConstructible = true;
   mCloneable = true;

   AddMethod(ut::make_unique<SetSAMInfo>("SetSAMInfo"));
   AddMethod(ut::make_unique<SetAIInfo>("SetAIInfo"));
   AddMethod(ut::make_unique<GetNumMunitions>("GetNumMunitions"));
   AddMethod(ut::make_unique<GetMaxEngagements>("GetMaxEngagements"));
   AddMethod(ut::make_unique<GetInterceptSpeed>("GetInterceptSpeed"));
   AddMethod(ut::make_unique<GetWsysID>("GetWsysID"));
   AddMethod(ut::make_unique<AddMez>());
   AddMethod(ut::make_unique<AddFez>());
   AddMethod(ut::make_unique<AddJez>());
}

WsfScriptBMWeaponInterface::~WsfScriptBMWeaponInterface()
{
}

void* WsfScriptBMWeaponInterface::Create(const UtScriptContext& aInstance)
{
   WsfBMWeaponInterface* objptr = new WsfBMWeaponInterface();
   return objptr;
}

void* WsfScriptBMWeaponInterface::Clone(void* aObjectPtr)
{
   return reinterpret_cast<WsfBMWeaponInterface*>(aObjectPtr)->Clone();
}

void  WsfScriptBMWeaponInterface::Destroy(void* aObjectPtr)
{
   delete reinterpret_cast<WsfBMWeaponInterface*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMWeaponInterface, WsfBMWeaponInterface, SetSAMInfo, 2, "void", "WsfBMAssetRecord, int")
{
   WsfBMAssetRecord* asset = (WsfBMAssetRecord*)aVarArgs[0].GetPointer()->GetAppObject();
   int weapon_idx = aVarArgs[1].GetInt();

   aObjectPtr->GetRawRecord()->Set(SIMULATION, asset, weapon_idx, il::WEAPON_SAM);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMWeaponInterface, WsfBMWeaponInterface, SetAIInfo, 2, "void", "WsfBMAssetRecord, int")
{
   WsfBMAssetRecord* asset = (WsfBMAssetRecord*)aVarArgs[0].GetPointer()->GetAppObject();
   int weapon_idx = aVarArgs[1].GetInt();

   aObjectPtr->GetRawRecord()->Set(SIMULATION, asset, weapon_idx, il::WEAPON_AI);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMWeaponInterface, WsfBMWeaponInterface, GetNumMunitions, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetRawRecord()->GetNumMunitions());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMWeaponInterface, WsfBMWeaponInterface, GetMaxEngagements, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetRawRecord()->GetMaxEngagements());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMWeaponInterface, WsfBMWeaponInterface, GetInterceptSpeed, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRawRecord()->GetInterceptSpeed());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMWeaponInterface, WsfBMWeaponInterface, GetWsysID, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetRawRecord()->GetWsysID());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMWeaponInterface, WsfBMWeaponInterface, AddMez, 2, "void", "WsfZone, double")
{
   WsfScriptBMWeaponInterface::AttachWeaponZone(aObjectPtr, aVarArgs[0].GetPointer()->GetAppObject(), aVarArgs[1].GetDouble(), il::E_ZONE_TYPE_MEZ, SIMULATION);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMWeaponInterface, WsfBMWeaponInterface, AddFez, 2, "void", "WsfZone, double")
{
   WsfScriptBMWeaponInterface::AttachWeaponZone(aObjectPtr, aVarArgs[0].GetPointer()->GetAppObject(), aVarArgs[1].GetDouble(), il::E_ZONE_TYPE_FEZ, SIMULATION);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMWeaponInterface, WsfBMWeaponInterface, AddJez, 2, "void", "WsfZone, double")
{
   WsfScriptBMWeaponInterface::AttachWeaponZone(aObjectPtr, aVarArgs[0].GetPointer()->GetAppObject(), aVarArgs[1].GetDouble(), il::E_ZONE_TYPE_JEZ, SIMULATION);
}

// Helper functions
void WsfScriptBMWeaponInterface::AttachWeaponZone(WsfBMWeaponInterface* aObjectPtr, void* wsf_zone_obj, il::tZonePriority priority, il::eZoneType type, WsfSimulation* sim_ptr)
{
   // TTD: determine if we need to take ownership here...if so, add ->Clone()
   WsfZoneDefinition* zone_ptr = reinterpret_cast<WsfZoneDefinition*>(wsf_zone_obj);

   std::shared_ptr<WsfBMZoneRecord> zone_ref(new WsfBMZoneRecord(type, il::zoneTypeToName(type, IADSC2ScenarioExtension::GetGlobalLogger(sim_ptr->GetScenario())), priority, zone_ptr, sim_ptr));
   aObjectPtr->GetRawRecord()->AddZone(zone_ref);
}
