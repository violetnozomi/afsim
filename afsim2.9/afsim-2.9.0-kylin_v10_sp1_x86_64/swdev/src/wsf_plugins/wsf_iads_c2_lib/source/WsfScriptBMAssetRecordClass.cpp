// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#include <script/WsfScriptDefs.hpp>

#include "WsfScriptBMAssetRecordClass.hpp"
#include "WsfBMAssetRecord.hpp"
#include "WsfBMWeaponInfoRecord.hpp"
#include "WsfAssetMap.hpp"
#include "WsfBMZoneRecord.hpp"
#include <iadsLib/enum.hpp>
#include <WsfBMUtils.hpp>
#include "WsfIADSC2ScenarioExtension.hpp"

WsfScriptBMAssetRecordClass::WsfScriptBMAssetRecordClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfBMAssetRecord");

   mConstructible = true;
   mCloneable = true;

   AddMethod(ut::make_unique<InitNode>());
   AddMethod(ut::make_unique<AddDirectSubordinate>());
   AddMethod(ut::make_unique<AddPeer>());
   AddMethod(ut::make_unique<AddDirectCommander>());
   AddMethod(ut::make_unique<AddWeaponInfo>());
   AddMethod(ut::make_unique<SetLocationECEF>());
   AddMethod(ut::make_unique<SetVelocityECEF>());
   AddMethod(ut::make_unique<SetDefended>());
   AddMethod(ut::make_unique<AttachDA>());

   AddMethod(ut::make_unique<SetMaxPlatformAssignments>());
   AddMethod(ut::make_unique<SetCurrPlatformAssignments>());
   AddMethod(ut::make_unique<SetPlatformAssignmentDelay>());

   AddMethod(ut::make_unique<SetStatusAgeouts>());
   AddMethod(ut::make_unique<SetSystemStatusRed>());
   AddMethod(ut::make_unique<SetSystemStatusGreen>());
   AddMethod(ut::make_unique<SetSystemStatusWhite>());
   AddMethod(ut::make_unique<SetSystemStatusYellow>());
   AddMethod(ut::make_unique<SetExternallyDefinedSystemStatus>());
   AddMethod(ut::make_unique<GetID>());
   AddMethod(ut::make_unique<SetC2Capable>());
   AddMethod(ut::make_unique<GetC2Capable>());

   AddMethod(ut::make_unique<SetDefaultDefendedPriority>());
}

WsfScriptBMAssetRecordClass::~WsfScriptBMAssetRecordClass()
{
}

void* WsfScriptBMAssetRecordClass::Create(const UtScriptContext& aContext)
{
   WsfBMAssetRecord* obj_ptr = new WsfBMAssetRecord(IADSC2ScenarioExtension::GetGlobalLogger(*WsfScriptContext::GetSCENARIO(aContext)));
   return obj_ptr;
}

void* WsfScriptBMAssetRecordClass::Clone(void* aObjectPtr)
{
   return reinterpret_cast<WsfBMAssetRecord*>(aObjectPtr)->Clone();
}

void  WsfScriptBMAssetRecordClass::Destroy(void* aObjectPtr)
{
   WsfBMAssetRecord* objPtr = reinterpret_cast<WsfBMAssetRecord*>(aObjectPtr);
   delete objPtr;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssetRecordClass, WsfBMAssetRecord, InitNode, 3, "void", "int, string, string")
{
   aObjectPtr->GetRawRecord()->setID(WsfBMUtils::GetUnitIDRecordFromPlatformIndex(SIMULATION, aVarArgs[0].GetInt()));
   aObjectPtr->GetRawRecord()->setName(aVarArgs[1].GetString());
   aObjectPtr->GetRawRecord()->setSide(aVarArgs[2].GetString());
   aObjectPtr->GetRawRecord()->setType(il::unitTypeRecord(il::C2_OTHER, "Other", INVALID_UNIT_TYPE, "<unset>")); // Default to a non-weapon type
   // TTD: put in support for subtype for the other battle managers
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssetRecordClass, WsfBMAssetRecord, AddDirectSubordinate, 2, "void", "int, WsfBMAssetMap")
{
   WsfAssetMap* asset_map_ptr = reinterpret_cast<WsfAssetMap*>(aVarArgs[1].GetPointer()->GetAppObject());

   aObjectPtr->GetRawRecord()->addDirectSubordinate(WsfBMUtils::GetUnitIDRecordFromPlatformIndex(SIMULATION, aVarArgs[0].GetInt()), *asset_map_ptr->GetRawAssetsPtr());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssetRecordClass, WsfBMAssetRecord, AddPeer, 2, "void", "int, WsfBMAssetMap")
{
   WsfAssetMap* asset_map_ptr = reinterpret_cast<WsfAssetMap*>(aVarArgs[1].GetPointer()->GetAppObject());
   aObjectPtr->GetRawRecord()->addDirectPeer(WsfBMUtils::GetUnitIDRecordFromPlatformIndex(SIMULATION, aVarArgs[0].GetInt()), *asset_map_ptr->GetRawAssetsPtr());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssetRecordClass, WsfBMAssetRecord, AddDirectCommander, 2, "void", "int, WsfBMAssetMap")
{
   WsfAssetMap* asset_map_ptr = reinterpret_cast<WsfAssetMap*>(aVarArgs[1].GetPointer()->GetAppObject());
   aObjectPtr->GetRawRecord()->addDirectCommander(WsfBMUtils::GetUnitIDRecordFromPlatformIndex(SIMULATION, aVarArgs[0].GetInt()), *asset_map_ptr->GetRawAssetsPtr());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssetRecordClass, WsfBMAssetRecord, AddWeaponInfo, 1, "void", "WsfBMWeaponInterface")
{
   WsfBMWeaponInterface* wi_ptr = reinterpret_cast<WsfBMWeaponInterface*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->GetRawRecord()->AddWeapon(wi_ptr->GetRawRecord());

   if (wi_ptr->GetRawRecord()->GetTypeRecord().getType() == il::WEAPON_SAM)
   {
      aObjectPtr->GetRawRecord()->setType(il::unitTypeRecord(il::WEAPON_SAM, "SAM", INVALID_UNIT_TYPE, "<unset>"));
   }
   else if (wi_ptr->GetRawRecord()->GetTypeRecord().getType() == il::WEAPON_AI)
   {
      aObjectPtr->GetRawRecord()->setType(il::unitTypeRecord(il::WEAPON_AI, "AI", INVALID_UNIT_TYPE, "<unset>"));
   }
   else
   {
      aObjectPtr->GetRawRecord()->setType(il::unitTypeRecord(il::WEAPON_OTHER, "Other", INVALID_UNIT_TYPE, "<unset>"));
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssetRecordClass, WsfBMAssetRecord, SetLocationECEF, 3, "void", "double, double, double")
{
   aObjectPtr->GetRawRecord()->setPositionXYZ(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssetRecordClass, WsfBMAssetRecord, SetVelocityECEF, 3, "void", "double, double, double")
{
   aObjectPtr->GetRawRecord()->setVelocityXYZ(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssetRecordClass, WsfBMAssetRecord, SetDefended, 1, "void", "int")
{
   aObjectPtr->GetRawRecord()->setDefended(aVarArgs[0].GetInt());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssetRecordClass, WsfBMAssetRecord, AttachDA, 2, "void", "WsfZone, double")
{
   WsfScriptBMAssetRecordClass::AttachUnitZone(aObjectPtr, aVarArgs[0].GetPointer()->GetAppObject(), aVarArgs[1].GetDouble(), il::E_ZONE_TYPE_DA, SIMULATION);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssetRecordClass, WsfBMAssetRecord, SetMaxPlatformAssignments, 1, "void", "int")
{
   aObjectPtr->GetRawRecord()->setMaxAssignments(aVarArgs[0].GetInt());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssetRecordClass, WsfBMAssetRecord, SetCurrPlatformAssignments, 1, "void", "int")
{
   aObjectPtr->GetRawRecord()->setNumAssignments(aVarArgs[0].GetInt());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssetRecordClass, WsfBMAssetRecord, SetPlatformAssignmentDelay, 1, "void", "int")
{
   aObjectPtr->GetRawRecord()->setAssignmentDelay(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssetRecordClass, WsfBMAssetRecord, SetStatusAgeouts, 2, "void", "double, double")
{
   aObjectPtr->GetRawRecord()->setAgeYellow(aVarArgs[0].GetDouble());
   aObjectPtr->GetRawRecord()->setAgeRed(aVarArgs[1].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssetRecordClass, WsfBMAssetRecord, SetSystemStatusRed, 0, "void", "")
{
   aObjectPtr->GetRawRecord()->setSystemStatus(il::STATUS_RED, true);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssetRecordClass, WsfBMAssetRecord, SetSystemStatusGreen, 0, "void", "")
{
   aObjectPtr->GetRawRecord()->setSystemStatus(il::STATUS_GREEN, true);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssetRecordClass, WsfBMAssetRecord, SetSystemStatusWhite, 0, "void", "")
{
   aObjectPtr->GetRawRecord()->setSystemStatus(il::STATUS_WHITE, true);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssetRecordClass, WsfBMAssetRecord, SetSystemStatusYellow, 0, "void", "")
{
   aObjectPtr->GetRawRecord()->setSystemStatus(il::STATUS_YELLOW, true);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssetRecordClass, WsfBMAssetRecord, SetExternallyDefinedSystemStatus, 1, "void", "bool")
{
   aObjectPtr->GetRawRecord()->setExternallyDefinedSystemStatus(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssetRecordClass, WsfBMAssetRecord, GetID, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetRawRecord()->getID().getID());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssetRecordClass, WsfBMAssetRecord, SetC2Capable, 1, "void", "bool")
{
   aObjectPtr->GetRawRecord()->setC2Capable(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssetRecordClass, WsfBMAssetRecord, GetC2Capable, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetRawRecord()->getC2Capable());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssetRecordClass, WsfBMAssetRecord, SetDefaultDefendedPriority, 1, "void", "double")
{
   aObjectPtr->GetRawRecord()->setDefaultDefendedPriority(aVarArgs[0].GetDouble());
}

// Helper functions
void WsfScriptBMAssetRecordClass::AttachUnitZone(WsfBMAssetRecord* aObjectPtr, void* wsf_zone_obj, il::tZonePriority priority, il::eZoneType type, WsfSimulation* sim_ptr)
{
   // TTD: determine if we need to take ownership here...if so, add ->Clone()
   WsfZoneDefinition* zone_ptr = reinterpret_cast<WsfZoneDefinition*>(wsf_zone_obj);

   std::shared_ptr<WsfBMZoneRecord> zone_ref(new WsfBMZoneRecord(type, il::zoneTypeToName(type, IADSC2ScenarioExtension::GetGlobalLogger(sim_ptr->GetScenario())), priority, zone_ptr, sim_ptr));
   aObjectPtr->GetRawRecord()->AddZone(zone_ref);
}
