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

#include "WsfQuantumTask.hpp"

#include <string>

#include "UtMemory.hpp"
#include "WsfTask.hpp"
#include "WsfTaskResource.hpp"
#include "WsfTrack.hpp"
#include "WsfWeaponTask.hpp"
#include "WsfWeaponTaskResource.hpp"
#include "script/WsfScriptContext.hpp"

//! If a track is not provided, the unique id of the task can not be automatically generated,
//! then the creator should call SetUniqueId() if the task is to have a unique id.
WsfQuantumTask::WsfQuantumTask(double aPriority, const WsfTaskResource& aResource, WsfTrack* aTrackPtr)
   : WsfTask(aResource)
   , mPriority(aPriority)
   , mUniqueId(0)
{
   if (aTrackPtr != nullptr)
   {
      SetTrackId(aTrackPtr->GetTrackId());
      SetLocalTrackId(aTrackPtr->GetTrackId());
      SetTargetName(aTrackPtr->GetTargetName());
   }
   mTaskId = GetUniqueId();
}

// virtual
WsfQuantumTask* WsfQuantumTask::Clone() const
{
   return new WsfQuantumTask(*this);
}

class WSF_MIL_EXPORT WsfScriptQuantumTaskClass : public WsfScriptTaskClass
{
public:
   WsfScriptQuantumTaskClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(Priority_1);
   UT_DECLARE_SCRIPT_METHOD(Priority_2);
   UT_DECLARE_SCRIPT_METHOD(UniqueId);
   UT_DECLARE_SCRIPT_METHOD(ResourceType); // weapon, sensor, jammer, unknown
   UT_DECLARE_SCRIPT_METHOD(Construct_1);  // WsfQuantumTask.Construct(double priority)
   UT_DECLARE_SCRIPT_METHOD(Construct_2);  // WsfQuantumTask.Construct(double priority, string type)
   UT_DECLARE_SCRIPT_METHOD(Construct_3);  // WsfQuantumTask.Construct(double priority, string type, WsfTrack aTarget)
};

// Script Accessor Class
WsfScriptQuantumTaskClass::WsfScriptQuantumTaskClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptTaskClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfQuantumTask");

   mConstructible = true;
   mCloneable     = true;

   AddMethod(ut::make_unique<Priority_1>("Priority")); // WsfQuantumTask.Priority()
   AddMethod(ut::make_unique<Priority_2>("Priority")); // WsfQuantumTask.Priority(double priority)
   AddMethod(ut::make_unique<UniqueId>());
   AddMethod(ut::make_unique<ResourceType>());                 // weapon, sensor, jammer, unknown
   AddStaticMethod(ut::make_unique<Construct_1>("Construct")); // WsfQuantumTask.Construct(double priority)
   AddStaticMethod(ut::make_unique<Construct_2>("Construct")); // WsfQuantumTask.Construct(double priority, string type)
   AddStaticMethod(ut::make_unique<Construct_3>("Construct")); // WsfQuantumTask.Construct(double priority, string type,
                                                               // WsfTrack aTarget)
}

std::unique_ptr<UtScriptClass> WsfQuantumTask::CreateScriptClass(const std::string& aClassName,
                                                                 UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptQuantumTaskClass>(aClassName, aScriptTypesPtr);
}

void* WsfScriptQuantumTaskClass::Create(const UtScriptContext& /*aContext*/)
{
   return new WsfQuantumTask();
}

void* WsfScriptQuantumTaskClass::Clone(void* aObjectPtr)
{
   return static_cast<WsfQuantumTask*>(aObjectPtr)->Clone();
}


void WsfScriptQuantumTaskClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfQuantumTask*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskClass, WsfQuantumTask, Priority_1, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetPriority());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskClass, WsfQuantumTask, Priority_2, 1, "void", "double")
{
   double priority = aVarArgs[0].GetDouble();
   aObjectPtr->SetPriority(priority);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskClass, WsfQuantumTask, UniqueId, 1, "void", "int")
{
   int uniqueId = aVarArgs[0].GetInt();
   aObjectPtr->SetUniqueId((unsigned int)uniqueId);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskClass, WsfQuantumTask, ResourceType, 0, "string", "")
{
   // sensor, weapon, jammer, unknown
   switch (aObjectPtr->GetResourceType())
   {
   case cTASK_RESOURCE_TYPE_SENSOR:
   {
      aReturnVal.SetString("sensor");
   }
   break;
   case cTASK_RESOURCE_TYPE_WEAPON:
   {
      aReturnVal.SetString("weapon");
   }
   break;
   case cTASK_RESOURCE_TYPE_JAMMER:
   {
      aReturnVal.SetString("jammer");
   }
   break;
   default:
   {
      aReturnVal.SetString("unknown");
   }
   break;
   };
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskClass, WsfQuantumTask, Construct_1, 1, "WsfQuantumTask", "double")
{
   double          priority = aVarArgs[0].GetDouble();
   WsfQuantumTask* taskPtr  = new WsfQuantumTask(priority);
   aReturnVal.SetPointer(new UtScriptRef(taskPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskClass, WsfQuantumTask, Construct_2, 2, "WsfQuantumTask", "double, string")
{
   double          priority = aVarArgs[0].GetDouble();
   std::string     type     = aVarArgs[1].GetString();
   WsfQuantumTask* taskPtr;
   if (type == "weapon" || type == "WEAPON")
   {
      taskPtr = new WsfQuantumTask(priority, WsfWeaponTaskResource());
   }
   else if (type == "sensor" || type == "SENSOR")
   {
      taskPtr = new WsfQuantumTask(priority, WsfTaskResource(cTASK_RESOURCE_TYPE_SENSOR));
   }
   else if (type == "jammer" || type == "JAMMER")
   {
      taskPtr = new WsfQuantumTask(priority, WsfJammerTaskResource());
   }
   else
   {
      taskPtr = new WsfQuantumTask(priority, WsfTaskResource(cTASK_RESOURCE_TYPE_NONE));
   }
   taskPtr->SetTaskType(type);
   aReturnVal.SetPointer(new UtScriptRef(taskPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptQuantumTaskClass, WsfQuantumTask, Construct_3, 3, "WsfQuantumTask", "double, string, WsfTrack")
{
   double          priority = aVarArgs[0].GetDouble();
   std::string     type     = aVarArgs[1].GetString();
   WsfTrack*       trackPtr = static_cast<WsfTrack*>(aVarArgs[2].GetPointer()->GetAppObject());
   WsfQuantumTask* taskPtr;
   if (type == "weapon" || type == "WEAPON")
   {
      taskPtr = new WsfQuantumTask(priority, WsfWeaponTaskResource(), trackPtr);
   }
   else if (type == "sensor" || type == "SENSOR")
   {
      taskPtr = new WsfQuantumTask(priority, WsfTaskResource(cTASK_RESOURCE_TYPE_SENSOR), trackPtr);
   }
   else if (type == "jammer" || type == "JAMMER")
   {
      taskPtr = new WsfQuantumTask(priority, WsfJammerTaskResource(), trackPtr);
   }
   else
   {
      taskPtr = new WsfQuantumTask(priority, WsfTaskResource(cTASK_RESOURCE_TYPE_NONE), trackPtr);
   }
   taskPtr->SetTaskType(type);
   aReturnVal.SetPointer(new UtScriptRef(taskPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}
