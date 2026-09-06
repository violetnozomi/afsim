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

#ifndef WSFSCRIPTAUXDATAUTIL_HPP
#define WSFSCRIPTAUXDATAUTIL_HPP

#include <string>

#include "UtException.hpp"
#include "UtMemory.hpp"
#include "UtScriptBasicTypes.hpp" // TODO remove this include
#include "UtScriptClassDefine.hpp"
#include "UtScriptDataPack.hpp" // TODO remove this include
#include "WsfAuxDataEnabled.hpp"

namespace WsfScriptAuxDataUtil
{
template<typename OBJECT_TYPE, typename T>
void GetAuxData(OBJECT_TYPE*                    aObjectPtr,
                UtScriptMethodArgs              aVarArgs,
                UtScriptData&                   aReturnVal,
                UtScriptClass*                  aReturnClassPtr,
                UtScriptClass::InterfaceMethod* aInterfaceMethodPtr)
{
   static_assert(std::is_base_of<WsfAuxDataEnabled, OBJECT_TYPE>::value,
                 "Template type of GetAuxData must derive from WsfAuxDataEnabled");

   if (!aObjectPtr->HasAuxData())
   {
      aReturnVal = UtScriptData(T{});
      return;
   }

   UtAttributeBase* attribute = aObjectPtr->GetAuxDataConst().FindAttribute(aVarArgs[0].GetString());

   if (attribute == nullptr)
   {
      aReturnVal = UtScriptData(T{});
      return;
   }

   T                      value         = T();
   auto                   attributeType = attribute->GetTypeId();
   ut::script::Data::Type valueType     = UtScriptData(value).GetType();
   if (attributeType == UtAttribute<bool>::GetClassTypeId())
   {
      if (valueType == ut::script::Data::Type::cBOOL)
      {
         attribute->Get(value);
      }
      else
      {
         bool returnValueBool;
         attribute->Get(returnValueBool);
         value = UtScriptData(returnValueBool);
      }
   }
   else if (attributeType == UtAttribute<int>::GetClassTypeId())
   {
      if (valueType == ut::script::Data::Type::cINT)
      {
         attribute->Get(value);
      }
      else
      {
         int returnValueInt;
         attribute->Get(returnValueInt);
         value = UtScriptData(returnValueInt);
      }
   }
   else if (attributeType == UtAttribute<double>::GetClassTypeId())
   {
      if (valueType == ut::script::Data::Type::cDOUBLE)
      {
         attribute->Get(value);
      }
      else
      {
         double returnValueDouble;
         attribute->Get(returnValueDouble);
         value = UtScriptData(returnValueDouble);
      }
   }
   else if (attributeType == UtAttribute<std::string>::GetClassTypeId())
   {
      if (valueType == ut::script::Data::Type::cSTRING)
      {
         attribute->Get(value);
      }
      else
      {
         std::string returnValueStr;
         attribute->Get(returnValueStr);
         value = UtScriptData(returnValueStr);
      }
   }
   else
   {
      attribute->Get(value);
   }
   aReturnVal = UtScriptData(value);
}

template<typename OBJECT_TYPE>
void SetAuxData(OBJECT_TYPE*                    aObjectPtr,
                UtScriptMethodArgs              aVarArgs,
                UtScriptData&                   aReturnVal,
                UtScriptClass*                  aReturnClassPtr,
                UtScriptClass::InterfaceMethod* aInterfaceMethodPtr,
                bool                            aSilentFailure = false)
{
   static_assert(std::is_base_of<WsfAuxDataEnabled, OBJECT_TYPE>::value,
                 "Template type of SetAuxData must derive from WsfAuxDataEnabled");

   const std::string&  attributeName = aVarArgs[0].GetString();
   const UtScriptData& value         = aVarArgs[1];
   try
   {
      switch (value.GetType())
      {
      case ut::script::Data::Type::cBOOL:
         aObjectPtr->GetAuxData().Assign(attributeName, value.GetBool());
         break;
      case ut::script::Data::Type::cINT:
         aObjectPtr->GetAuxData().Assign(attributeName, value.GetInt());
         break;
      case ut::script::Data::Type::cDOUBLE:
         aObjectPtr->GetAuxData().Assign(attributeName, value.GetDouble());
         break;
      case ut::script::Data::Type::cSTRING:
         aObjectPtr->GetAuxData().Assign(attributeName, value.GetString());
         break;
      default:
         aObjectPtr->GetAuxData().Assign(attributeName, value);
         break;
      }
   }
   catch (UtException&)
   {
      if (!aSilentFailure)
      {
         UT_SCRIPT_ABORT("AuxData exists, but is not correct type.");
      }
   }
}

template<typename OBJECT_TYPE>
void GetAllAuxDataTypes(OBJECT_TYPE* aObjectPtr, UtScriptData& aReturnVal, UtScriptClass* aReturnClassPtr)
{
   static_assert(std::is_base_of<WsfAuxDataEnabled, OBJECT_TYPE>::value,
                 "Template type of GetAllAuxDataTypes must derive from WsfAuxDataEnabled");

   auto                                      retAuxMapPtr = ut::make_unique<UtScriptMap::Map>();
   const UtAttributeContainer::AttributeMap& attributes   = aObjectPtr->GetAuxDataConst().GetAttributeMap();
   for (const auto& aux : attributes)
   {
      retAuxMapPtr->emplace(UtScriptData{aux.first}, UtScriptData{aux.second->GetType()});
   }

   auto* mapRefPtr = new UtScriptRef(retAuxMapPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE);
   aReturnVal.SetPointer(mapRefPtr);
}

template<class SCRIPT_CLASS>
void AddAuxDataScriptMethods(SCRIPT_CLASS& aScriptClass)
{
   static_assert(std::is_base_of<UtScriptClass, SCRIPT_CLASS>::value,
                 "Template type of AddAuxDataScriptMethods must derive from UtScriptClass");

   // Add each of the aux data method objects to the class.
   aScriptClass.AddMethod(ut::make_unique<typename SCRIPT_CLASS::SetAuxData>());
   aScriptClass.AddMethod(ut::make_unique<typename SCRIPT_CLASS::AuxDataBool>());
   aScriptClass.AddMethod(ut::make_unique<typename SCRIPT_CLASS::AuxDataInt>());
   aScriptClass.AddMethod(ut::make_unique<typename SCRIPT_CLASS::AuxDataDouble>());
   aScriptClass.AddMethod(ut::make_unique<typename SCRIPT_CLASS::AuxDataString>());
   aScriptClass.AddMethod(ut::make_unique<typename SCRIPT_CLASS::AuxDataObject>());
   aScriptClass.AddMethod(ut::make_unique<typename SCRIPT_CLASS::AuxDataExists>());
   aScriptClass.AddMethod(ut::make_unique<typename SCRIPT_CLASS::AuxDataExists>("CheckAuxData"));
   aScriptClass.AddMethod(ut::make_unique<typename SCRIPT_CLASS::DeleteAuxData>());
   aScriptClass.AddMethod(ut::make_unique<typename SCRIPT_CLASS::HasAuxData>());
   aScriptClass.AddMethod(ut::make_unique<typename SCRIPT_CLASS::GetAllAuxDataTypes>());
}
} // namespace WsfScriptAuxDataUtil

// Macro for declaring standard aux-data methods on a script class
#define UT_DECLARE_AUX_DATA_SCRIPT_METHODS() \
   UT_DECLARE_SCRIPT_METHOD(SetAuxData);     \
   UT_DECLARE_SCRIPT_METHOD(AuxDataBool);    \
   UT_DECLARE_SCRIPT_METHOD(AuxDataInt);     \
   UT_DECLARE_SCRIPT_METHOD(AuxDataDouble);  \
   UT_DECLARE_SCRIPT_METHOD(AuxDataString);  \
   UT_DECLARE_SCRIPT_METHOD(AuxDataObject);  \
   UT_DECLARE_SCRIPT_METHOD(AuxDataExists);  \
   UT_DECLARE_SCRIPT_METHOD(CheckAuxData);   \
   UT_DECLARE_SCRIPT_METHOD(DeleteAuxData);  \
   UT_DECLARE_SCRIPT_METHOD(HasAuxData);     \
   UT_DECLARE_SCRIPT_METHOD(GetAllAuxDataTypes);

// Macro for defining aux-data methods on a script class
#define UT_DEFINE_AUX_DATA_SCRIPT_METHODS(CLASS, OBJ_TYPE)                                                                        \
   UT_DEFINE_SCRIPT_METHOD(CLASS, OBJ_TYPE, SetAuxData, 2, "void", "string, Object")                                              \
   {                                                                                                                              \
      WsfScriptAuxDataUtil::SetAuxData<OBJ_TYPE>(aObjectPtr, aVarArgs, aReturnVal, aReturnClassPtr, aInterfaceMethodPtr);         \
   }                                                                                                                              \
   UT_DEFINE_SCRIPT_METHOD(CLASS, OBJ_TYPE, AuxDataBool, 1, "bool", "string")                                                     \
   {                                                                                                                              \
      WsfScriptAuxDataUtil::GetAuxData<OBJ_TYPE, bool>(aObjectPtr, aVarArgs, aReturnVal, aReturnClassPtr, aInterfaceMethodPtr);   \
   }                                                                                                                              \
   UT_DEFINE_SCRIPT_METHOD(CLASS, OBJ_TYPE, AuxDataInt, 1, "int", "string")                                                       \
   {                                                                                                                              \
      WsfScriptAuxDataUtil::GetAuxData<OBJ_TYPE, int>(aObjectPtr, aVarArgs, aReturnVal, aReturnClassPtr, aInterfaceMethodPtr);    \
   }                                                                                                                              \
   UT_DEFINE_SCRIPT_METHOD(CLASS, OBJ_TYPE, AuxDataDouble, 1, "double", "string")                                                 \
   {                                                                                                                              \
      WsfScriptAuxDataUtil::GetAuxData<OBJ_TYPE, double>(aObjectPtr, aVarArgs, aReturnVal, aReturnClassPtr, aInterfaceMethodPtr); \
   }                                                                                                                              \
   UT_DEFINE_SCRIPT_METHOD(CLASS, OBJ_TYPE, AuxDataString, 1, "string", "string")                                                 \
   {                                                                                                                              \
      WsfScriptAuxDataUtil::GetAuxData<OBJ_TYPE, std::string>(aObjectPtr,                                                         \
                                                              aVarArgs,                                                           \
                                                              aReturnVal,                                                         \
                                                              aReturnClassPtr,                                                    \
                                                              aInterfaceMethodPtr);                                               \
   }                                                                                                                              \
   UT_DEFINE_SCRIPT_METHOD(CLASS, OBJ_TYPE, AuxDataObject, 1, "Object", "string")                                                 \
   {                                                                                                                              \
      WsfScriptAuxDataUtil::GetAuxData<OBJ_TYPE, UtScriptData>(aObjectPtr,                                                        \
                                                               aVarArgs,                                                          \
                                                               aReturnVal,                                                        \
                                                               aReturnClassPtr,                                                   \
                                                               aInterfaceMethodPtr);                                              \
   }                                                                                                                              \
   UT_DEFINE_SCRIPT_METHOD(CLASS, OBJ_TYPE, AuxDataExists, 1, "bool", "string")                                                   \
   {                                                                                                                              \
      aReturnVal.SetBool(aObjectPtr->GetAuxDataConst().AttributeExists(aVarArgs[0].GetString()));                                 \
   }                                                                                                                              \
   UT_DEFINE_SCRIPT_METHOD(CLASS, OBJ_TYPE, DeleteAuxData, 1, "bool", "string")                                                   \
   {                                                                                                                              \
      aReturnVal.SetBool(aObjectPtr->GetAuxData().Delete(aVarArgs[0].GetString()));                                               \
   }                                                                                                                              \
   UT_DEFINE_SCRIPT_METHOD(CLASS, OBJ_TYPE, HasAuxData, 0, "bool", "")                                                            \
   {                                                                                                                              \
      aReturnVal.SetBool(aObjectPtr->HasAuxData());                                                                               \
   }                                                                                                                              \
   UT_DEFINE_SCRIPT_METHOD(CLASS, OBJ_TYPE, GetAllAuxDataTypes, 0, "Map<string,string>", "")                                      \
   {                                                                                                                              \
      WsfScriptAuxDataUtil::GetAllAuxDataTypes<OBJ_TYPE>(aObjectPtr, aReturnVal, aReturnClassPtr);                                \
   }

#endif
