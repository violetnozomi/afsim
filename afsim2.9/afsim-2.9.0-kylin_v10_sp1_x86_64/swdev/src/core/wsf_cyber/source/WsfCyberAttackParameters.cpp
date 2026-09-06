// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCyberAttackParameters.hpp"

#include "UtCast.hpp"

namespace wsf
{
namespace cyber
{

// =================================================================================================
ScriptAttackParameterObjectClass::ScriptAttackParameterObjectClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfCyberAttackParameterObject");
   mIsScriptAccessible = true;
   mConstructible      = true;
   mCloneable          = true;

   WsfScriptAuxDataUtil::AddAuxDataScriptMethods(*this);
}

// =================================================================================================
void* ScriptAttackParameterObjectClass::Create(const UtScriptContext&)
{
   return new AttackParameterObject(true);
}

// =================================================================================================
void* ScriptAttackParameterObjectClass::Clone(void* aObjectPtr)
{
   return static_cast<AttackParameterObject*>(aObjectPtr)->Clone();
}

// =================================================================================================
void ScriptAttackParameterObjectClass::Destroy(void* aObjectPtr)
{
   delete static_cast<AttackParameterObject*>(aObjectPtr);
}

UT_DEFINE_AUX_DATA_SCRIPT_METHODS(ScriptAttackParameterObjectClass, AttackParameterObject)

// =================================================================================================
std::vector<std::string> AttackParameters::GetEffectNames() const
{
   std::vector<std::string> effects;

   for (const auto& entry : mMap)
   {
      effects.emplace_back(entry.first);
   }

   return effects;
}

// =================================================================================================
const std::vector<AttackParameterObject>* AttackParameters::GetEffectEntries(const std::string& aEffectName) const
{
   auto it = mMap.find(aEffectName);
   if (it != std::end(mMap))
   {
      return &it->second;
   }
   return nullptr;
}

// =================================================================================================
size_t AttackParameters::GetEffectEntryCount(const std::string& aEffectName) const
{
   auto it = mMap.find(aEffectName);
   if (it != std::end(mMap))
   {
      return it->second.size();
   }
   return 0U;
}

// =================================================================================================
AttackParameterObject* AttackParameters::GetEffectEntryByIndex(const std::string& aEffectName, size_t aIndex)
{
   auto it = mMap.find(aEffectName);
   if (it != std::end(mMap) && aIndex < it->second.size())
   {
      return &(it->second[aIndex]);
   }
   return nullptr;
}

// =================================================================================================
void AttackParameters::AddParameter(const AttackParameterObject& aParameter)
{
   std::vector<std::string> attributeNames;

   const auto& auxData = aParameter.GetAuxDataConst();
   auxData.GetAttributeNames(attributeNames);

   if (attributeNames.size() != 1)
   {
      return;
   }

   mMap[attributeNames.front()].push_back(aParameter);
}

// =================================================================================================
ScriptAttackParametersClass::ScriptAttackParametersClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfCyberAttackParameters");
   mIsScriptAccessible = true;
   mConstructible      = true;
   mCloneable          = true;

   AddMethod(ut::make_unique<GetEffectNames>());
   AddMethod(ut::make_unique<GetEffectEntryCount>());
   AddMethod(ut::make_unique<GetEffectEntryByIndex>());
   AddMethod(ut::make_unique<AddParameter>());
}

// =================================================================================================
void* ScriptAttackParametersClass::Create(const UtScriptContext&)
{
   return new AttackParameters();
}

// =================================================================================================
void* ScriptAttackParametersClass::Clone(void* aObjectPtr)
{
   return static_cast<AttackParameters*>(aObjectPtr)->Clone();
}

// =================================================================================================
void ScriptAttackParametersClass::Destroy(void* aObjectPtr)
{
   delete static_cast<AttackParameters*>(aObjectPtr);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptAttackParametersClass, AttackParameters, GetEffectNames, 0, "Array<string>", "")
{
   auto effectNames = aObjectPtr->GetEffectNames();
   auto data        = ut::make_unique<std::vector<UtScriptData>>(effectNames.begin(), effectNames.end());

   aReturnVal.SetPointer(new UtScriptRef(data.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptAttackParametersClass, AttackParameters, GetEffectEntryCount, 1, "int", "string")
{
   const auto& effectName = aVarArgs[0].GetString();
   auto        count      = aObjectPtr->GetEffectEntryCount(effectName);

   aReturnVal.SetInt(ut::cast_to_int(count));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptAttackParametersClass,
                        AttackParameters,
                        GetEffectEntryByIndex,
                        2,
                        "WsfCyberAttackParameterObject",
                        "string, int")
{
   const auto& effectName = aVarArgs[0].GetString();
   auto        index      = ut::cast_to_size_t(aVarArgs[1].GetInt());

   auto parameterPtr = aObjectPtr->GetEffectEntryByIndex(effectName, index);
   aReturnVal.SetPointer(new UtScriptRef(parameterPtr, aReturnClassPtr));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptAttackParametersClass, AttackParameters, AddParameter, 1, "void", "WsfCyberAttackParameterObject")
{
   auto parameterPtr = aVarArgs[0].GetPointer()->GetAppObject<AttackParameterObject>();

   if (parameterPtr)
   {
      aObjectPtr->AddParameter(*parameterPtr);
   }
}

} // namespace cyber
} // namespace wsf
