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
*   File: WsfScriptBMAuxDataZoneAttachmentClass.cpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name: Radiance Technologies
*   Address:         350 Wynn Drive
*                    Huntsville, AL 35805
*
*   Abstract: Script exports for WsfBMAuxDataZoneAttachment.
*
*******************************************************************************/

#include <string>
#include <sstream>

#include "UtException.hpp"
#include "script/WsfScriptContext.hpp"

#include <WsfScriptBMAuxDataZoneAttachmentClass.hpp>
#include <WsfBMAuxDataZoneAttachment.hpp>
#include <stringUtils.hpp>
#include <iadsLib/zoneRecordInterface.hpp>
#include "WsfIADSC2ScenarioExtension.hpp"

namespace
{
   WsfBMAuxDataZoneAttachment::eAttachmentType AttachmentScopeStringToEnum(const std::string& scope_str)
   {
      if (il::equalsIgnoreCase(scope_str, "platform"))
      {
         return WsfBMAuxDataZoneAttachment::E_PLATFORM;
      }
      else if (il::equalsIgnoreCase(scope_str, "global"))
      {
         return WsfBMAuxDataZoneAttachment::E_GLOBAL;
      }

      std::stringstream strm_except;
      strm_except << "Invalid zone scope string: " << scope_str;

      throw UtException(strm_except.str());

      return WsfBMAuxDataZoneAttachment::E_GLOBAL; // stop warnings
   }

   il::eZoneType ZoneTypeStringToEnum(const std::string& type_str)
   {
      if (il::equalsIgnoreCase(type_str, "OTHER"))
      {
         return il::E_ZONE_TYPE_OTHER;
      }
      else if (il::equalsIgnoreCase(type_str, "AOR"))
      {
         return il::E_ZONE_TYPE_AOR;
      }
      else if (il::equalsIgnoreCase(type_str, "SUR"))
      {
         return il::E_ZONE_TYPE_SUR;
      }
      else if (il::equalsIgnoreCase(type_str, "MEZ"))
      {
         return il::E_ZONE_TYPE_MEZ;
      }
      else if (il::equalsIgnoreCase(type_str, "FEZ"))
      {
         return il::E_ZONE_TYPE_FEZ;
      }
      else if (il::equalsIgnoreCase(type_str, "JEZ"))
      {
         return il::E_ZONE_TYPE_JEZ;
      }
      else if (il::equalsIgnoreCase(type_str, "GEZ"))
      {
         return il::E_ZONE_TYPE_GEZ;
      }
      else if (il::equalsIgnoreCase(type_str, "DA"))
      {
         return il::E_ZONE_TYPE_DA;
      }
      else if (il::equalsIgnoreCase(type_str, "COR"))
      {
         return il::E_ZONE_TYPE_COR;
      }

      std::stringstream strm_except;
      strm_except << "Invalid zone type string: " << type_str;

      throw UtException(strm_except.str());

      return il::E_ZONE_TYPE_COR; // stop warnings
   }
}

WsfScriptBMAuxDataZoneAttachmentClass::WsfScriptBMAuxDataZoneAttachmentClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptObjectClass(aClassName, aScriptTypesPtr)
{
   SetClassName(WSF_BM_AUX_DATA_ZONE_ATTACH_SCRIPT_TYPE_NAME);
   AddClassName("WSF_BM_ZONE_ATTACHMENT");

   mConstructible = true;
   mCloneable = true;

   AddMethod(ut::make_unique<Add>());
   AddMethod(ut::make_unique<GetCount>());
   AddMethod(ut::make_unique<IsNthScopeGlobal>());
   AddMethod(ut::make_unique<IsNthScopePlatform>());
   AddMethod(ut::make_unique<GetNthPriority>());
   AddMethod(ut::make_unique<GetNthZoneName>());
   AddMethod(ut::make_unique<IsNthTypeMEZ>());
   AddMethod(ut::make_unique<IsNthTypeFEZ>());
   AddMethod(ut::make_unique<IsNthTypeJEZ>());
   AddMethod(ut::make_unique<GetNthZoneTypeString>());
}

WsfScriptBMAuxDataZoneAttachmentClass::~WsfScriptBMAuxDataZoneAttachmentClass()
{
}

void* WsfScriptBMAuxDataZoneAttachmentClass::Create(const UtScriptContext& aContext)
{
   WsfBMAuxDataZoneAttachment* objPtr = new WsfBMAuxDataZoneAttachment();
   return objPtr;
}

void* WsfScriptBMAuxDataZoneAttachmentClass::Clone(void* aObjectPtr)
{
   return reinterpret_cast<WsfBMAuxDataZoneAttachment*>(aObjectPtr)->Clone();
}

void  WsfScriptBMAuxDataZoneAttachmentClass::Destroy(void* aObjectPtr)
{
   WsfBMAuxDataZoneAttachment* objPtr = reinterpret_cast<WsfBMAuxDataZoneAttachment*>(aObjectPtr);
   delete objPtr;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAuxDataZoneAttachmentClass, WsfBMAuxDataZoneAttachment, Add, 4, "void", "string, double, string, string")
{
   auto attachment_scope = AttachmentScopeStringToEnum(aVarArgs[0].GetString());
   auto priority = aVarArgs[1].GetDouble();
   auto zone_name = aVarArgs[2].GetString();
   auto zone_type = ZoneTypeStringToEnum(aVarArgs[3].GetString());
   aObjectPtr->AddZoneAttachment(attachment_scope, priority, zone_name, zone_type);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAuxDataZoneAttachmentClass, WsfBMAuxDataZoneAttachment, GetCount, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetZoneAttachmentCount()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAuxDataZoneAttachmentClass, WsfBMAuxDataZoneAttachment, IsNthScopeGlobal, 1, "bool", "int")
{
   const std::size_t nth_zone = aVarArgs[0].GetInt();

   WsfBMAuxDataZoneAttachment::eAttachmentType attach_type;
   double priority;
   std::string zone_name;
   il::eZoneType zone_type;
   aObjectPtr->GetNthZoneAttachment(nth_zone, attach_type, priority, zone_name, zone_type);
   aReturnVal.SetBool(attach_type == WsfBMAuxDataZoneAttachment::E_GLOBAL);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAuxDataZoneAttachmentClass, WsfBMAuxDataZoneAttachment, IsNthScopePlatform, 1, "bool", "int")
{
   const std::size_t nth_zone = aVarArgs[0].GetInt();

   WsfBMAuxDataZoneAttachment::eAttachmentType attach_type;
   double priority;
   std::string zone_name;
   il::eZoneType zone_type;
   aObjectPtr->GetNthZoneAttachment(nth_zone, attach_type, priority, zone_name, zone_type);
   aReturnVal.SetBool(attach_type == WsfBMAuxDataZoneAttachment::E_PLATFORM);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAuxDataZoneAttachmentClass, WsfBMAuxDataZoneAttachment, GetNthPriority, 1, "double", "int")
{
   const std::size_t nth_zone = aVarArgs[0].GetInt();

   WsfBMAuxDataZoneAttachment::eAttachmentType attach_type;
   double priority;
   std::string zone_name;
   il::eZoneType zone_type;
   aObjectPtr->GetNthZoneAttachment(nth_zone, attach_type, priority, zone_name, zone_type);
   aReturnVal.SetDouble(priority);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAuxDataZoneAttachmentClass, WsfBMAuxDataZoneAttachment, GetNthZoneName, 1, "string", "int")
{
   const std::size_t nth_zone = aVarArgs[0].GetInt();

   WsfBMAuxDataZoneAttachment::eAttachmentType attach_type;
   double priority;
   std::string zone_name;
   il::eZoneType zone_type;
   aObjectPtr->GetNthZoneAttachment(nth_zone, attach_type, priority, zone_name, zone_type);
   aReturnVal.SetString(zone_name);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAuxDataZoneAttachmentClass, WsfBMAuxDataZoneAttachment, IsNthTypeMEZ, 1, "bool", "int")
{
   const std::size_t nth_zone = aVarArgs[0].GetInt();

   WsfBMAuxDataZoneAttachment::eAttachmentType attach_type;
   double priority;
   std::string zone_name;
   il::eZoneType zone_type;
   aObjectPtr->GetNthZoneAttachment(nth_zone, attach_type, priority, zone_name, zone_type);
   aReturnVal.SetBool(zone_type == il::E_ZONE_TYPE_MEZ);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAuxDataZoneAttachmentClass, WsfBMAuxDataZoneAttachment, IsNthTypeFEZ, 1, "bool", "int")
{
   const std::size_t nth_zone = aVarArgs[0].GetInt();

   WsfBMAuxDataZoneAttachment::eAttachmentType attach_type;
   double priority;
   std::string zone_name;
   il::eZoneType zone_type;
   aObjectPtr->GetNthZoneAttachment(nth_zone, attach_type, priority, zone_name, zone_type);
   aReturnVal.SetBool(zone_type == il::E_ZONE_TYPE_FEZ);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAuxDataZoneAttachmentClass, WsfBMAuxDataZoneAttachment, IsNthTypeJEZ, 1, "bool", "int")
{
   const std::size_t nth_zone = aVarArgs[0].GetInt();

   WsfBMAuxDataZoneAttachment::eAttachmentType attach_type;
   double priority;
   std::string zone_name;
   il::eZoneType zone_type;
   aObjectPtr->GetNthZoneAttachment(nth_zone, attach_type, priority, zone_name, zone_type);
   aReturnVal.SetBool(zone_type == il::E_ZONE_TYPE_JEZ);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAuxDataZoneAttachmentClass, WsfBMAuxDataZoneAttachment, GetNthZoneTypeString, 1, "string", "int")
{
   const std::size_t nth_zone = aVarArgs[0].GetInt();

   WsfBMAuxDataZoneAttachment::eAttachmentType attach_type;
   double priority;
   std::string zone_name;
   il::eZoneType zone_type;
   aObjectPtr->GetNthZoneAttachment(nth_zone, attach_type, priority, zone_name, zone_type);
   aReturnVal.SetString(il::zoneTypeToName(zone_type, IADSC2ScenarioExtension::GetGlobalLogger(*WsfScriptContext::GetSCENARIO(aContext))));
}
