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

#include "WsfWeaponTask.hpp"

#include "wsf_mil_export.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>

#include "UtMemory.hpp"
#include "WsfPlatform.hpp"
#include "WsfProcessor.hpp"
#include "WsfSimulation.hpp"
#include "WsfTask.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptMessageClass.hpp"


// =================================================================================================
// Script Interface
// =================================================================================================

namespace
{

UT_DEFINE_SCRIPT_METHOD_EXT(WsfTask, ResourceIsWeapon, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsResourceTypeOf(cTASK_RESOURCE_TYPE_WEAPON));
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfTask, ResourceIsJammer, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsResourceTypeOf(cTASK_RESOURCE_TYPE_JAMMER));
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfTask, ResourceIsUplink, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsResourceTypeOf(cTASK_RESOURCE_TYPE_UPLINK));
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfTask, ResourceCount, 0, "int", "")
{
   int count(0);
   if (aObjectPtr->IsResourceTypeOf(cTASK_RESOURCE_TYPE_WEAPON))
   {
      WsfWeaponTaskResource& resource = (WsfWeaponTaskResource&)aObjectPtr->GetResource();
      count                           = resource.mCount;
   }
   aReturnVal.SetInt(count);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfTask, ResourceFrequency, 0, "double", "")
{
   double frequency(0.0);
   if (aObjectPtr->IsResourceTypeOf(cTASK_RESOURCE_TYPE_JAMMER))
   {
      WsfJammerTaskResource& jammerResource = (WsfJammerTaskResource&)aObjectPtr->GetResource();
      frequency                             = jammerResource.mFrequency;
   }
   aReturnVal.SetDouble(frequency);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfTask, ResourceBandwidth, 0, "double", "")
{
   double bandwidth(0.0);
   if (aObjectPtr->IsResourceTypeOf(cTASK_RESOURCE_TYPE_JAMMER))
   {
      WsfJammerTaskResource& jammerResource = (WsfJammerTaskResource&)aObjectPtr->GetResource();
      bandwidth                             = jammerResource.mBandwidth;
   }
   aReturnVal.SetDouble(bandwidth);
}

} // End anonymous namespace

//! The script interface 'class' for WsfWeaponTask
class WSF_MIL_EXPORT WsfScriptWeaponTaskClass : public WsfScriptTaskClass
{
public:
   WsfScriptWeaponTaskClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(Create);
   UT_DECLARE_SCRIPT_METHOD(Quantity);
   UT_DECLARE_SCRIPT_METHOD(SetQuantity);
};

//! The script interface 'class' for WsfTask
class WSF_MIL_EXPORT WsfScriptJammerTaskClass : public WsfScriptTaskClass
{
public:
   WsfScriptJammerTaskClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(BeamNumber_1);
   UT_DECLARE_SCRIPT_METHOD(BeamNumber_2);
   UT_DECLARE_SCRIPT_METHOD(Bandwidth_1);
   UT_DECLARE_SCRIPT_METHOD(Bandwidth_2);
   UT_DECLARE_SCRIPT_METHOD(Frequency_1);
   UT_DECLARE_SCRIPT_METHOD(Frequency_2);
   UT_DECLARE_SCRIPT_METHOD(Technique_1);
   UT_DECLARE_SCRIPT_METHOD(Technique_2);
};

//! The script interface 'class' for WsfTask
class WSF_MIL_EXPORT WsfScriptUplinkTaskClass : public WsfScriptTaskClass
{
public:
   WsfScriptUplinkTaskClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(Create_1);
   UT_DECLARE_SCRIPT_METHOD(Create_2);
   UT_DECLARE_SCRIPT_METHOD(UplinkDestination_1);
   UT_DECLARE_SCRIPT_METHOD(UplinkDestination_2);
   UT_DECLARE_SCRIPT_METHOD(UplinkComm_1);
   UT_DECLARE_SCRIPT_METHOD(UplinkComm_2);
   UT_DECLARE_SCRIPT_METHOD(UplinkSource_1);
   UT_DECLARE_SCRIPT_METHOD(UplinkSource_2);
};

// ************************************************************************************************

WsfScriptWeaponTaskClass::WsfScriptWeaponTaskClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptTaskClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfWeaponTask");

   AddStaticMethod(ut::make_unique<Create>());

   AddMethod(ut::make_unique<Quantity>());
   AddMethod(ut::make_unique<SetQuantity>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponTaskClass, WsfTask, Create, 2, "WsfWeaponTask", "string, string")
{
   WsfWeaponTaskResource resource;
   resource.mNameId = aVarArgs[1].GetString();
   WsfTask* taskPtr = new WsfTask(resource);
   taskPtr->SetTaskType(aVarArgs[0].GetString());
   aReturnVal.SetPointer(new UtScriptRef(taskPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponTaskClass, WsfTask, SetQuantity, 1, "void", "int")
{
   if (aObjectPtr->IsResourceTypeOf(cTASK_RESOURCE_TYPE_WEAPON))
   {
      WsfWeaponTaskResource& resource = (WsfWeaponTaskResource&)aObjectPtr->GetResource();
      resource.mCount                 = aVarArgs[0].GetInt();
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponTaskClass, WsfTask, Quantity, 0, "int", "")
{
   if (aObjectPtr->IsResourceTypeOf(cTASK_RESOURCE_TYPE_WEAPON))
   {
      WsfWeaponTaskResource resource = (WsfWeaponTaskResource&)aObjectPtr->GetResource();
      aReturnVal.SetInt(resource.mCount);
   }
}

// ************************************************************************************************

WsfScriptJammerTaskClass::WsfScriptJammerTaskClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptTaskClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfJammerTask");

   AddMethod(ut::make_unique<BeamNumber_1>());
   AddMethod(ut::make_unique<BeamNumber_2>());
   AddMethod(ut::make_unique<Bandwidth_1>());
   AddMethod(ut::make_unique<Bandwidth_2>());
   AddMethod(ut::make_unique<Frequency_1>());
   AddMethod(ut::make_unique<Frequency_2>());
   AddMethod(ut::make_unique<Technique_1>());
   AddMethod(ut::make_unique<Technique_2>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptJammerTaskClass, WsfTask, BeamNumber_1, 0, "int", "")
{
   WsfJammerTaskResource& resource = (WsfJammerTaskResource&)aObjectPtr->GetResource();
   aReturnVal.SetInt(resource.mBeamNumber);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptJammerTaskClass, WsfTask, BeamNumber_2, 1, "void", "int")
{
   WsfJammerTaskResource& resource = (WsfJammerTaskResource&)aObjectPtr->GetResource();
   resource.mBeamNumber            = aVarArgs[0].GetInt();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptJammerTaskClass, WsfTask, Bandwidth_1, 0, "double", "")
{
   WsfJammerTaskResource& resource = (WsfJammerTaskResource&)aObjectPtr->GetResource();
   aReturnVal.SetDouble(resource.mBandwidth);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptJammerTaskClass, WsfTask, Bandwidth_2, 1, "void", "double")
{
   WsfJammerTaskResource& resource = (WsfJammerTaskResource&)aObjectPtr->GetResource();
   resource.mBandwidth             = aVarArgs[0].GetDouble();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptJammerTaskClass, WsfTask, Frequency_1, 0, "double", "")
{
   WsfJammerTaskResource& resource = (WsfJammerTaskResource&)aObjectPtr->GetResource();
   aReturnVal.SetDouble(resource.mFrequency);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptJammerTaskClass, WsfTask, Frequency_2, 1, "void", "double")
{
   WsfJammerTaskResource& resource = (WsfJammerTaskResource&)aObjectPtr->GetResource();
   resource.mFrequency             = aVarArgs[0].GetDouble();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptJammerTaskClass, WsfTask, Technique_1, 0, "string", "")
{
   WsfJammerTaskResource& resource = (WsfJammerTaskResource&)aObjectPtr->GetResource();
   aReturnVal.SetString(resource.mTechniqueName);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptJammerTaskClass, WsfTask, Technique_2, 1, "void", "string")
{
   WsfJammerTaskResource& resource = (WsfJammerTaskResource&)aObjectPtr->GetResource();
   resource.mTechniqueName         = aVarArgs[0].GetString();
}

// ************************************************************************************************

WsfScriptUplinkTaskClass::WsfScriptUplinkTaskClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptTaskClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfUplinkTask");

   AddStaticMethod(ut::make_unique<Create_1>("Create"));
   AddStaticMethod(ut::make_unique<Create_2>("Create"));
   AddMethod(ut::make_unique<UplinkDestination_1>("UplinkDestination"));
   AddMethod(ut::make_unique<UplinkDestination_2>("UplinkDestination"));
   AddMethod(ut::make_unique<UplinkComm_1>("UplinkComm"));
   AddMethod(ut::make_unique<UplinkComm_2>("UplinkComm"));
   AddMethod(ut::make_unique<UplinkSource_1>("UplinkSource"));
   AddMethod(ut::make_unique<UplinkSource_2>("UplinkSource"));
}

// Create(string aTaskType)
UT_DEFINE_SCRIPT_METHOD(WsfScriptUplinkTaskClass, WsfTask, Create_1, 1, "WsfUplinkTask", "string")
{
   WsfTask* taskPtr = new WsfTask(WsfUplinkTaskResource());
   taskPtr->SetTaskType(aVarArgs[0].GetString());
   aReturnVal.SetPointer(new UtScriptRef(taskPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// Create(string aTaskType, string aAssignee)
UT_DEFINE_SCRIPT_METHOD(WsfScriptUplinkTaskClass, WsfTask, Create_2, 2, "WsfUplinkTask", "string, string")
{
   WsfTask*     taskPtr     = new WsfTask(WsfUplinkTaskResource());
   WsfPlatform* assigneePtr = WsfScriptContext::GetSIMULATION(aContext)->GetPlatformByName(aVarArgs[1].GetString());
   taskPtr->SetAssignee(assigneePtr);
   taskPtr->SetTaskType(aVarArgs[0].GetString());
   aReturnVal.SetPointer(new UtScriptRef(taskPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptUplinkTaskClass, WsfTask, UplinkDestination_1, 0, "string", "")
{
   WsfUplinkTaskResource& resource = (WsfUplinkTaskResource&)aObjectPtr->GetResource();
   aReturnVal.SetString(resource.mUplinkDestination);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptUplinkTaskClass, WsfTask, UplinkComm_1, 0, "string", "")
{
   WsfUplinkTaskResource& resource = (WsfUplinkTaskResource&)aObjectPtr->GetResource();
   aReturnVal.SetString(resource.mUplinkCommName);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptUplinkTaskClass, WsfTask, UplinkSource_1, 0, "string", "")
{
   WsfUplinkTaskResource& resource = (WsfUplinkTaskResource&)aObjectPtr->GetResource();
   aReturnVal.SetString(resource.mUplinkSource);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptUplinkTaskClass, WsfTask, UplinkDestination_2, 1, "void", "string")
{
   WsfUplinkTaskResource& resource = (WsfUplinkTaskResource&)aObjectPtr->GetResource();
   resource.mUplinkDestination     = aVarArgs[0].GetString();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptUplinkTaskClass, WsfTask, UplinkComm_2, 1, "void", "string")
{
   WsfUplinkTaskResource& resource = (WsfUplinkTaskResource&)aObjectPtr->GetResource();
   resource.mUplinkCommName        = aVarArgs[0].GetString();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptUplinkTaskClass, WsfTask, UplinkSource_2, 1, "void", "string")
{
   WsfUplinkTaskResource& resource = (WsfUplinkTaskResource&)aObjectPtr->GetResource();
   resource.mUplinkSource          = aVarArgs[0].GetString();
}

// ------------------------------------------------------------------------------------------------

//! Register the types ('class' objects) for the scripting systems.
//!
//! This is invoked once by WsfScriptManager to create the 'class' objects that
//! are implemented by this file.
// static
void WsfWeaponTask::RegisterScriptTypes(UtScriptTypes* aScriptTypesPtr)
{
   aScriptTypesPtr->Register(ut::make_unique<WsfScriptWeaponTaskClass>("WsfWeaponTask", aScriptTypesPtr));
   aScriptTypesPtr->Register(ut::make_unique<WsfScriptJammerTaskClass>("WsfJammerTask", aScriptTypesPtr));
   aScriptTypesPtr->Register(ut::make_unique<WsfScriptUplinkTaskClass>("WsfUplinkTask", aScriptTypesPtr));
   aScriptTypesPtr->RegisterExtension(ut::make_unique<WsfWeaponTaskScriptExtensions>());
}

// =================================================================================================
//! Register the script methods associated with this class.
//! @note The WsfPlatform script class must be defined before calling this method.
// static
void WsfWeaponTask::RegisterScriptMethods(UtScriptTypes& aScriptTypes)
{
   aScriptTypes.AddClassMethod("WsfTask", ut::make_unique<ResourceIsWeapon>());
   aScriptTypes.AddClassMethod("WsfTask", ut::make_unique<ResourceIsJammer>());
   aScriptTypes.AddClassMethod("WsfTask", ut::make_unique<ResourceIsUplink>());
   aScriptTypes.AddClassMethod("WsfTask", ut::make_unique<ResourceCount>());
   aScriptTypes.AddClassMethod("WsfTask", ut::make_unique<ResourceFrequency>());
   aScriptTypes.AddClassMethod("WsfTask", ut::make_unique<ResourceBandwidth>());
}

// =================================================================================================

bool WsfWeaponTaskScriptExtensions::AddExtClassMethods(const std::string& aClassName,
                                                       const std::string& aBaseName,
                                                       UtScriptTypes*     aScriptTypesPtr)
{
   if (aBaseName == aClassName)
   {
      // Should add base classes manually prior to initialization phase
   }
   else if (aBaseName == "WsfTask")
   {
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<ResourceIsWeapon>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<ResourceIsJammer>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<ResourceIsUplink>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<ResourceCount>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<ResourceFrequency>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<ResourceBandwidth>());
   }
   else
   {
      return false;
   }
   return true;
}
