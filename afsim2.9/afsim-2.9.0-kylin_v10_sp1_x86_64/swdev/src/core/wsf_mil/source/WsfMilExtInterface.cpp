// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfMilExtInterface.hpp"

#include <iomanip>
#include <sstream>

#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfExplicitWeaponEffects.hpp"
#include "WsfMil.hpp"
#include "WsfPlatform.hpp"
#include "WsfRF_Jammer.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfWeaponEffectsTypes.hpp"
#include "WsfWeaponTypes.hpp"
#include "ext/WsfExtEmission.hpp"

WsfMilExtInterface::WsfMilExtInterface(WsfMilExtension* aMilExtensionPtr)
   : mDebugWarfarePDU(0)
   , mMilExtensionPtr(aMilExtensionPtr)
   , mExternalWeaponEffectsType()
   , mSimpleKillRange(100.0)
   , mUseSimpleDetonations(true)
   , mExcludedDetonationPlatformTypes()
   , mExcludedDetonationWeaponEffects()
   , sPK_Table()
   , mCategoryToWarheadMap()
   , sTechTypeToJamModeSeq()
   , sJamModeSeqToTechType()
{
}

bool WsfMilExtInterface::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "simple_detonations")
   {
      aInput.ReadValue(mUseSimpleDetonations);
   }
   else if (command == "simple_detonations_exclude")
   {
      std::string objectType;
      aInput.ReadValue(objectType);
      if (objectType == "platform_type")
      {
         aInput.ReadValue(objectType);
         mExcludedDetonationPlatformTypes.insert(objectType);
      }
      else if ((objectType == "weapon_effect") || (objectType == "weapon_effects"))
      {
         aInput.ReadValue(objectType);
         mExcludedDetonationWeaponEffects.insert(objectType);
      }
      else
      {
         throw UtInput::BadValue(aInput, objectType + " is not a valid object for " + command);
      }
   }
   else if (command == "simple_kill_range")
   {
      aInput.ReadValueOfType(mSimpleKillRange, UtInput::cLENGTH);
      aInput.ValueGreater(mSimpleKillRange, 0.0);
   }
   else if (command == "simple_kill_probability") // weapon type, target type
   {
      std::string weaponType;
      std::string targetType;
      aInput.ReadCommand(weaponType);
      aInput.ReadCommand(targetType);
      double pk;
      aInput.ReadValue(pk);
      sPK_Table[WeaponTargetType(weaponType, targetType)] = pk;
   }
   else if (command == "use_simple_detonations") // Old form...
   {
      mUseSimpleDetonations = true;
   }
   else if (command == "debug_warfare_pdu")
   {
      aInput.ReadValue(mDebugWarfarePDU);
   }
   else if (command == "munition_type")
   {
      WsfExtInput::Find(mMilExtensionPtr->GetScenario())->mData.ProcessEntityType(aInput);
   }
   else if (command == "warhead")
   {
      std::string wsfCategory;
      aInput.ReadValue(wsfCategory);
      int disEnum;
      aInput.ReadValue(disEnum);
      mCategoryToWarheadMap.insert(make_pair(wsfCategory, disEnum));
   }
   else if (command == "ew_technique_type")
   {
      ProcessEW_TechniqueType(aInput);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

WsfComponent* WsfMilExtInterface::CloneComponent() const
{
   return new WsfMilExtInterface(*this);
}

WsfStringId WsfMilExtInterface::GetComponentName() const
{
   return UtStringIdLiteral("#mil");
}

const int* WsfMilExtInterface::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_MIL_EXT_INTERFACE, 0};
   return roles;
}

void* WsfMilExtInterface::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_MIL_EXT_INTERFACE)
   {
      return this;
   }
   return nullptr;
}

bool WsfMilExtInterface::CompleteLoad(WsfScenario& aScenario)
{
   WsfWeaponEffectsTypes& weaponEffectsTypes(WsfWeaponEffectsTypes::Get(aScenario));

   // User may define WSF_EXTERNAL_WEAPON_EFFECT or WSF_DIS_WEAPON_EFFECT
   if (mExternalWeaponEffectsType.empty())
   {
      if (weaponEffectsTypes.Find("WSF_EXTERNAL_WEAPON_EFFECT") != nullptr)
      {
         mExternalWeaponEffectsType = "WSF_EXTERNAL_WEAPON_EFFECT";
      }
      else if (weaponEffectsTypes.Find("WSF_DIS_WEAPON_EFFECT") != nullptr)
      {
         mExternalWeaponEffectsType = "WSF_DIS_WEAPON_EFFECT";
      }
   }

   if (nullptr == weaponEffectsTypes.Find(mExternalWeaponEffectsType))
   {
      mExternalWeaponEffectsType = "WSF_EXTERNAL_WEAPON_EFFECT";
      auto effectPtr             = ut::make_unique<WsfExplicitWeaponEffects>(aScenario);
      effectPtr->SetMaxiumEffectiveRadius(mSimpleKillRange);
      weaponEffectsTypes.Add(mExternalWeaponEffectsType, std::move(effectPtr));
   }

   return true;
}

WsfMilExtInterface* WsfMilExtInterface::Find(const WsfExtInterface& aExt)
{
   WsfMilExtInterface* milPtr = nullptr;
   aExt.GetComponents().FindByRole(milPtr);
   return milPtr;
}

void WsfMilExtInterface::PrintEmitterTypes(WsfScenario& aScenario, std::ostream& aStream)
{
   WsfObjectTypeListBase::TypeIdList weaponTypes;

   WsfWeaponTypes::Get(aScenario).GetTypeIds(weaponTypes);
   int typeCount = (int)weaponTypes.size();
   for (int i = 0; i < typeCount; ++i)
   {
      std::string typeName;
      int         index = i;
      WsfStringId partType;
      partType             = weaponTypes[index];
      typeName             = partType;
      WsfWeapon* weaponPtr = WsfWeaponTypes::Get(aScenario).Find(typeName);
      bool       isEmitter = dynamic_cast<WsfRF_Jammer*>(weaponPtr) != nullptr;
      if (isEmitter)
      {
         uint16_t    emitterType = WsfExtInput::Find(aScenario)->GetEmission()->GetEmitterType(partType);
         WsfStringId partTypeId  = WsfExtInput::Find(aScenario)->GetEmission()->GetPartTypeId(emitterType);
         aStream << std::setw(30) << typeName << std::setw(20) << emitterType << " (" << partTypeId << ")\n";
      }
   }
}

WsfWeapon* WsfMilExtInterface::GetWeapon(WsfPlatform* aPlatformPtr,
                                         WsfStringId  aSensorTypeId,
                                         unsigned int aEmitterNameDIS,
                                         unsigned int aEmitterIdNumberDIS)
{
   // Sensor entry not found; check for a jammer (weapon)
   WsfWeapon* weaponPtr = nullptr;

   for (WsfComponentList::RoleIterator<WsfWeapon> iter(*aPlatformPtr); !iter.AtEnd(); ++iter)
   {
      // Check for the type match
      bool sensorTypeMatches(false);
      if (!aSensorTypeId.IsNull())
      {
         sensorTypeMatches = iter->IsA_TypeOf(aSensorTypeId);
      }

      // Check for emitter name match

      int  auxValue;
      bool emitterNameMatches =
         WsfUtil::GetAuxValue(*iter, "dis_emitter_name", auxValue) && (auxValue == (int)aEmitterNameDIS);

      if (sensorTypeMatches || emitterNameMatches)
      {
         // Now check the emitter ID
         if ((iter->GetAuxDataConst()).AttributeExists("dis_emitter_id"))
         {
            if ((iter->GetAuxDataConst()).GetInt("dis_emitter_id") == (int)aEmitterIdNumberDIS)
            {
               // Found!
               weaponPtr = *iter;
               break;
            }
         }
         else
         {
            // Create the mapping to the DIS emitter ID number
            (iter->GetAuxData()).Assign("dis_emitter_id", (int)aEmitterIdNumberDIS);
            // Found!
            weaponPtr = *iter;
            break;
         }
      }
   }
   return weaponPtr;
}

WsfWeapon* WsfMilExtInterface::GetWeapon(const WsfScenario& aScenario,
                                         WsfStringId        aSensorTypeId,
                                         unsigned int       aEmitterNameDIS,
                                         unsigned int       aEmitterIdNumberDIS)
{
   // Set the default return value
   WsfWeapon* weaponPtr = WsfWeaponTypes::Get(aScenario).Clone(aSensorTypeId);
   if (weaponPtr != nullptr)
   {
      // Over-rule the aux data; if it exists
      if ((weaponPtr->GetAuxDataConst()).AttributeExists("dis_emitter_name"))
      {
         (weaponPtr->GetAuxData()).Assign("dis_emitter_name", (int)aEmitterNameDIS);
      }

      // Create the mapping to the DIS emitter ID number
      (weaponPtr->GetAuxData()).Assign("dis_emitter_id", (int)aEmitterIdNumberDIS);
   }

   return weaponPtr;
}

// ============================================================================
//! Adds a weapon to the given platform.
//!
//! @param aSimTime             [input] Current simulation time.
//! @param aPlatformPtr         [input] Pointer to the weapon's platform.
//! @param aSensorTypeId        [input] Sensor type ID.
//! @param aEmitterNameDIS      [input] DIS emitter system - emitter name
//! @param aEmitterIdNumberDIS  [input] DIS emitter system - emitter ID number
//!
//! @return A pointer to the weapon just added.
WsfWeapon* WsfMilExtInterface::AddWeapon(double       aSimTime,
                                         WsfPlatform* aPlatformPtr,
                                         WsfStringId  aSensorTypeId,
                                         unsigned int aEmitterNameDIS,
                                         unsigned int aEmitterIdNumberDIS)
{
   // Set default return value
   WsfWeapon* newWeaponPtr = nullptr;

   // Clone the platform; based on the type
   WsfPlatform* srcPlatformPtr = dynamic_cast<WsfPlatform*>(
      aPlatformPtr->GetSimulation()->GetScenario().FindType("platform_type", aPlatformPtr->GetType()));

   // Get the weapon from the source platform or
   WsfWeapon* srcWeaponPtr = GetWeapon(srcPlatformPtr, aSensorTypeId, aEmitterNameDIS, aEmitterIdNumberDIS);
   if (srcWeaponPtr == nullptr)
   {
      // Weapon is not on the (possibly cloned) platform; use emitter_type info and clone weapon
      srcWeaponPtr = GetWeapon(aPlatformPtr->GetScenario(), aSensorTypeId, aEmitterNameDIS, aEmitterIdNumberDIS);
   }

   if (srcWeaponPtr != nullptr)
   {
      // Clone the weapon
      newWeaponPtr = srcWeaponPtr->Clone();
      if (newWeaponPtr != nullptr)
      {
         // Emitter is a sensor; set the name and add
         std::ostringstream oss;
         oss << aEmitterNameDIS << '_' << aEmitterIdNumberDIS;
         newWeaponPtr->SetName(oss.str());

         if (aPlatformPtr->AddComponent(newWeaponPtr))
         {
            if (!newWeaponPtr->Initialize(aSimTime))
            {
               { // RAII block
                  auto out = ut::log::error() << "Initialization failed for external Weapon.";
                  out.AddNote() << "Platform: " << aPlatformPtr->GetName();
                  out.AddNote() << "Weapon: " << newWeaponPtr->GetName();
               }
               // TODO_CBA this was an error! aPlatformPtr->DeleteSensor(newWeaponPtr->GetNameId());
               aPlatformPtr->DeleteComponent<WsfWeapon>(newWeaponPtr->GetName());
               newWeaponPtr = nullptr;
            }
         }
         else
         {
            // Adding the weapon to the platform failed
            delete newWeaponPtr;
            newWeaponPtr = nullptr;
         }
      }
   }
   return newWeaponPtr;
}

//! Given an entity type, select the object type to be used.
WsfStringId WsfMilExtInterface::SelectWeaponType(WsfSimulation& aSimulation, const WsfExtEntityType& aEntityType)
{
   WsfExtEntityType entityType(aEntityType);

   // Try a complete match...
   const std::map<WsfExtEntityType, WsfStringId>& entityTypeToObjectType =
      WsfExtInterface::Find(aSimulation)->GetEntityTypeToObjectMap();
   auto mapIter = entityTypeToObjectType.find(entityType);

   if (mapIter != entityTypeToObjectType.end())
   {
      WsfStringId objectTypeId = (*mapIter).second;

      if (WsfWeaponTypes::Get(mMilExtensionPtr->GetScenario()).Find(objectTypeId) != nullptr)
      {
         return (*mapIter).second;
      }
   }

   // Eliminate 'extra'.
   entityType.mExtra = 0;
   mapIter           = entityTypeToObjectType.find(entityType);
   if (mapIter != entityTypeToObjectType.end())
   {
      WsfStringId objectTypeId = (*mapIter).second;
      if (WsfWeaponTypes::Get(mMilExtensionPtr->GetScenario()).Find(objectTypeId) != nullptr)
      {
         return (*mapIter).second;
      }
   }

   // Eliminate 'specific'.
   entityType.mSpecific = 0;
   mapIter              = entityTypeToObjectType.find(entityType);
   if (mapIter != entityTypeToObjectType.end())
   {
      WsfStringId objectTypeId = (*mapIter).second;
      if (WsfWeaponTypes::Get(mMilExtensionPtr->GetScenario()).Find(objectTypeId) != nullptr)
      {
         return (*mapIter).second;
      }
   }

   // Eliminate 'subcategory'
   entityType.mSubcategory = 0;
   mapIter                 = entityTypeToObjectType.find(entityType);
   if (mapIter != entityTypeToObjectType.end())
   {
      WsfStringId objectTypeId = (*mapIter).second;
      if (WsfWeaponTypes::Get(mMilExtensionPtr->GetScenario()).Find(objectTypeId) != nullptr)
      {
         return (*mapIter).second;
      }
   }

   return WsfStringId();
}

//! Get the warhead enum based on any input category associations.
//! Default is to return 0 == "Other".
int WsfMilExtInterface::GetWarheadEnum(const WsfPlatformPart* aWarheadPtr)
{
   int warheadEnum = 0; //"Other"
   if ((!mCategoryToWarheadMap.empty()) && (aWarheadPtr != nullptr))
   {
      std::map<WsfStringId, int>::const_iterator iter = mCategoryToWarheadMap.begin();
      while (iter != mCategoryToWarheadMap.end())
      {
         if (aWarheadPtr->GetCategories().IsCategoryMember(iter->first))
         {
            warheadEnum = iter->second;
            break;
         }
         ++iter;
      }
   }
   return warheadEnum;
}

void WsfMilExtInterface::ProcessEW_TechniqueType(UtInput& aInput)
{
   std::string techniqueTypeString;
   aInput.ReadValue(techniqueTypeString);

   WsfStringId techniqueId = techniqueTypeString;
   double      modeSeqId;
   aInput.ReadValue(modeSeqId);
   aInput.ValueInClosedRange<double>(modeSeqId, 0, 0xFFFFFFFFu - 1);

   uint32_t modeSeqIdDIS               = static_cast<uint32_t>(modeSeqId);
   sTechTypeToJamModeSeq[techniqueId]  = modeSeqIdDIS;
   sJamModeSeqToTechType[modeSeqIdDIS] = techniqueId;
}

//! Return the DIS jamming mode sequence name that corresponds to given WSF EW technique type Id.
uint32_t WsfMilExtInterface::GetJamModeSequence(WsfStringId aTechTypeId)
{
   uint32_t jamModeName = 0;
   auto     iter        = sTechTypeToJamModeSeq.find(aTechTypeId);
   if (iter != sTechTypeToJamModeSeq.end())
   {
      jamModeName = iter->second;
   }
   return jamModeName;
}

//! Return the WSF technique type Id that corresponds to given DIS jamming mode sequence.
WsfStringId WsfMilExtInterface::GetTechniqueTypeId(uint32_t aJamModeSequence)
{
   WsfStringId techTypeId;
   auto        iter = sJamModeSeqToTechType.find(aJamModeSequence);
   if (iter != sJamModeSeqToTechType.end())
   {
      techTypeId = iter->second;
   }
   return techTypeId;
}
