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

#include "WsfTask.hpp"

#include <cassert>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include "UtMemory.hpp"
#include "WsfPlatform.hpp"
#include "WsfProcessor.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptMessageClass.hpp"

void WsfTask::Initialize(double aSimTime, unsigned long aTaskId, WsfProcessor* aAssignerPtr)
{
   mAssignTime = aSimTime;
   mUpdateTime = aSimTime;
   if (mTaskId <= 0)
   {
      mTaskId = aTaskId;
   }
   mAssignerPlatformName  = aAssignerPtr->GetPlatform()->GetNameId();
   mAssignerPlatformIndex = aAssignerPtr->GetPlatform()->GetIndex();
   mAssignerProcessorName = aAssignerPtr->GetNameId();
}

void WsfTask::SetResource(const WsfTaskResource& aResource)
{
   if (mResourcePtr != &aResource)
   {
      delete mResourcePtr;
      mResourcePtr = aResource.Clone();
   }
}

//! Copy the assigner from another task.
//! This is used to transfer an assignment.
void WsfTask::SetAssigner(const WsfTask& aTask)
{
   mAssignerPlatformName  = aTask.mAssignerPlatformName;
   mAssignerPlatformIndex = aTask.mAssignerPlatformIndex;
   mAssignerProcessorName = aTask.mAssignerProcessorName;
}

void WsfTask::SetAssignee(WsfPlatform* aAssigneePtr)
{
   assert(aAssigneePtr != nullptr);
   mAssigneePlatformName  = aAssigneePtr->GetNameId();
   mAssigneePlatformIndex = aAssigneePtr->GetIndex();
   mAssigneeProcessorName = nullptr;
}

//! The script interface 'class' for WsfTaskList
class WSF_EXPORT WsfScriptTaskListClass : public UtScriptClass
{
public:
   WsfScriptTaskListClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   void        Destroy(void* aObjectPtr) override;
   std::string ToString(void* aObjectPtr) const override;

   int          EnumerateSize(void* aObjectPtr) override;
   UtScriptData Enumerate(void* aObjectPtr, int aIndex) override;

   UT_DECLARE_SCRIPT_METHOD(Count);
   UT_DECLARE_SCRIPT_METHOD(Entry);
   UT_DECLARE_SCRIPT_METHOD(GetIterator);
};

using ScriptTaskList = std::vector<UtWeakReference<WsfTask>>;

//! The script interface 'class' for WsfTaskListIterator
class WSF_EXPORT WsfScriptTaskListIteratorClass : public UtScriptClass
{
public:
   WsfScriptTaskListIteratorClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);
   ~WsfScriptTaskListIteratorClass() override;

   void Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(HasNext);
   UT_DECLARE_SCRIPT_METHOD(Next);
   UT_DECLARE_SCRIPT_METHOD(HasPrev);
   UT_DECLARE_SCRIPT_METHOD(Prev);
   UT_DECLARE_SCRIPT_METHOD(Key);
   UT_DECLARE_SCRIPT_METHOD(Data);

   //! Iterator for a Task list.
   class Iterator
   {
   public:
      Iterator(ScriptTaskList* aTaskListPtr);
      virtual ~Iterator();

      virtual bool           HasNext();
      virtual const WsfTask* Next();

      virtual bool           HasPrev();
      virtual const WsfTask* Prev();

      virtual int            Key() const;
      virtual const WsfTask* Data() const;

      bool IteratorValidForAccess() const
      {
         return (mTaskListPtr && mVectorSize == mTaskListPtr->size()) && (mCurrentKey >= 0) &&
                (mCurrentKey < (int)mVectorSize);
      }
      std::string IteratorErrorString() const
      {
         if (mTaskListPtr == nullptr)
         {
            return "Iterator invalid -- List is null";
         }
         if (mVectorSize != mTaskListPtr->size())
         {
            return "Iterator invalid -- List was modified";
         }
         return "Iterator invalid -- out of range";
      }

   private:
      ScriptTaskList* mTaskListPtr;
      int             mCurrentKey;
      size_t          mVectorSize;
   };
};

// ************************************************************************************************

WsfScriptTaskClass::WsfScriptTaskClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfTask");

   mCloneable = true;

   AddMethod(ut::make_unique<TaskId>());
   AddMethod(ut::make_unique<TrackId>());
   AddMethod(ut::make_unique<LocalTrackId>());
   AddMethod(ut::make_unique<TaskType>());
   AddMethod(ut::make_unique<Assigner>());
   AddMethod(ut::make_unique<AssignerName>());
   AddMethod(ut::make_unique<Assignee>());
   AddMethod(ut::make_unique<AssigneeName>());
   AddMethod(ut::make_unique<ResourceName>());
   AddMethod(ut::make_unique<ResourceIsSensor>());
   AddMethod(ut::make_unique<ResourceIsProcessor>());
   AddMethod(ut::make_unique<ResourceMode>());
   AddMethod(ut::make_unique<Status>());
   AddMethod(ut::make_unique<SubStatus>());
   AddMethod(ut::make_unique<TimeAssigned>());
   AddMethod(ut::make_unique<TimeUpdated>());
   AddMethod(ut::make_unique<TimeSinceAssigned>());
   AddMethod(ut::make_unique<TimeSinceUpdated>());

   AddMethod(ut::make_unique<SetAssignee_1>("SetAssignee"));
   AddMethod(ut::make_unique<SetAssignee_2>("SetAssignee"));
   AddMethod(ut::make_unique<SetCommDevice>());
   AddMethod(ut::make_unique<SetTaskType>());

   WsfScriptAuxDataUtil::AddAuxDataScriptMethods(*this);
}

// virtual
std::string WsfScriptTaskClass::ToString(void* aObjectPtr) const
{
   WsfTask*          taskPtr = static_cast<WsfTask*>(aObjectPtr);
   std::stringstream ss;
   ss << "WsfTask(";
   if (taskPtr != nullptr)
   {
      ss << "id=" << taskPtr->GetTaskId() << ", type=" << (WsfStringId)taskPtr->GetTaskType()
         << ", assignee=" << (WsfStringId)taskPtr->GetAssigneePlatformName()
         << ", assigner=" << (WsfStringId)taskPtr->GetAssignerPlatformName() << ")";
   }
   else
   {
      ss << "null)";
   }
   return ss.str();
}

// virtual
void WsfScriptTaskClass::OnNewScriptRef(UtScriptRef& aReference)
{
   unsigned int taskid = 0;
   if (aReference.GetMemManagement() != UtScriptRef::cMANAGE)
   {
      WsfTask* taskPtr = static_cast<WsfTask*>(aReference.GetAppObject());
      if (taskPtr != nullptr)
      {
         aReference.SetExternalReference(taskPtr->GetReferenceCount());
         taskid = taskPtr->GetTaskId();
      }
   }
   aReference.SetAuxData(taskid);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskClass, WsfTask, TaskId, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetTaskId());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskClass, WsfTask, TrackId, 0, "WsfTrackId", "")
{
   WsfTrackId* trackIdPtr = new WsfTrackId(aObjectPtr->GetTrackId());
   aReturnVal.SetPointer(new UtScriptRef(trackIdPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskClass, WsfTask, LocalTrackId, 0, "WsfTrackId", "")
{
   WsfTrackId* trackIdPtr = new WsfTrackId(aObjectPtr->GetLocalTrackId());
   aReturnVal.SetPointer(new UtScriptRef(trackIdPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskClass, WsfTask, TaskType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetTaskType().GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskClass, WsfTask, Assigner, 0, "WsfPlatform", "")
{
   WsfPlatform* platformPtr =
      WsfScriptContext::GetSIMULATION(aContext)->GetPlatformByIndex(aObjectPtr->GetAssignerPlatformIndex());
   aReturnVal.SetPointer(new UtScriptRef(platformPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskClass, WsfTask, AssignerName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetAssignerPlatformName().GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskClass, WsfTask, Assignee, 0, "WsfPlatform", "")
{
   WsfPlatform* platformPtr =
      WsfScriptContext::GetSIMULATION(aContext)->GetPlatformByIndex(aObjectPtr->GetAssigneePlatformIndex());
   aReturnVal.SetPointer(new UtScriptRef(platformPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskClass, WsfTask, AssigneeName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetAssigneePlatformName().GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskClass, WsfTask, ResourceName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetResourceName().GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskClass, WsfTask, ResourceIsSensor, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsResourceTypeOf(cTASK_RESOURCE_TYPE_SENSOR));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskClass, WsfTask, ResourceIsProcessor, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsResourceTypeOf(cTASK_RESOURCE_TYPE_PROCESSOR));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskClass, WsfTask, ResourceMode, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetResourceMode().GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskClass, WsfTask, Status, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetStatus().GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskClass, WsfTask, SubStatus, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetSubStatus().GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskClass, WsfTask, TimeAssigned, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetAssignTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskClass, WsfTask, TimeUpdated, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetUpdateTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskClass, WsfTask, TimeSinceAssigned, 0, "double", "")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aReturnVal.SetDouble(simTime - aObjectPtr->GetAssignTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskClass, WsfTask, TimeSinceUpdated, 0, "double", "")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aReturnVal.SetDouble(simTime - aObjectPtr->GetUpdateTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskClass, WsfTask, SetAssignee_1, 1, "void", "WsfPlatform")
{
   WsfPlatform* assigneePtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetAssignee(assigneePtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskClass, WsfTask, SetAssignee_2, 1, "void", "string")
{
   WsfPlatform* assigneePtr =
      WsfScriptContext::GetSIMULATION(aContext)->GetPlatformByName(WsfStringId(aVarArgs[0].GetString()));
   if (assigneePtr != nullptr)
   {
      aObjectPtr->SetAssignee(assigneePtr);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskClass, WsfTask, SetTaskType, 1, "void", "string")
{
   aObjectPtr->SetTaskType(WsfStringId(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskClass, WsfTask, SetCommDevice, 1, "void", "string")
{
   aObjectPtr->SetCommName(WsfStringId(aVarArgs[0].GetString()));
}

UT_DEFINE_AUX_DATA_SCRIPT_METHODS(WsfScriptTaskClass, WsfTask)

// ************************************************************************************************

WsfScriptTaskListClass::WsfScriptTaskListClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfTaskList");

   // Set the key and data types of this container.
   SetContainerKeyTypeId(GetTypes()->GetTypeId("int"));
   SetContainerDataTypeId(GetTypes()->GetTypeId("WsfTask"));

   mContainer = true;

   AddMethod(ut::make_unique<Count>());
   AddMethod(ut::make_unique<Entry>());
   AddMethod(ut::make_unique<GetIterator>());
}

// virtual
void WsfScriptTaskListClass::Destroy(void* aObjectPtr)
{
   delete static_cast<std::vector<UtWeakReference<WsfTask>>*>(aObjectPtr);
}

// virtual
std::string WsfScriptTaskListClass::ToString(void* aObjectPtr) const
{
   ScriptTaskList*   taskListPtr = (ScriptTaskList*)aObjectPtr;
   std::stringstream ss;
   UtScriptClass*    taskClassPtr = GetTypes()->GetClass("WsfTask");
   if (taskListPtr != nullptr)
   {
      ss << "{ ";
      for (size_t i = 0; i < taskListPtr->size(); ++i)
      {
         if (i != 0)
         {
            ss << ", ";
         }
         ss << taskClassPtr->ToString(taskListPtr->at(i).Get());
      }
      ss << " }";
   }
   else
   {
      ss << "WsfTaskList(null)";
   }
   return ss.str();
}

int WsfScriptTaskListClass::EnumerateSize(void* aObjectPtr)
{
   ScriptTaskList* taskListPtr = (ScriptTaskList*)aObjectPtr;
   if (taskListPtr != nullptr)
   {
      return (int)taskListPtr->size();
   }
   return 0;
}

UtScriptData WsfScriptTaskListClass::Enumerate(void* aObjectPtr, int aIndex)
{
   ScriptTaskList* taskListPtr = (ScriptTaskList*)aObjectPtr;
   if ((taskListPtr != nullptr) && (aIndex < (int)taskListPtr->size()))
   {
      WsfTask* taskPtr = taskListPtr->at(aIndex).Get();
      if (taskPtr != nullptr)
      {
         UtScriptClass* classPtr = GetTypes()->GetClass("WsfTask");
         return UtScriptData(new UtScriptRef(taskPtr, classPtr, taskPtr->GetReferenceCount()));
      }
   }
   return UtScriptData();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskListClass, ScriptTaskList, Count, 0, "int", "")
{
   int count = static_cast<int>(aObjectPtr->size());
   aReturnVal.SetInt(count);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskListClass, ScriptTaskList, Entry, 1, "WsfTask", "int")
{
   WsfTask* taskPtr = nullptr;
   int      index   = aVarArgs[0].GetInt();
   if ((index >= 0) && (index < static_cast<int>(aObjectPtr->size())))
   {
      taskPtr = const_cast<WsfTask*>((*aObjectPtr)[index].Get());
   }
   aReturnVal.SetPointer(new UtScriptRef(taskPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskListClass, ScriptTaskList, GetIterator, 0, "WsfTaskListIterator", "")
{
   // Create the iterator.
   void* iterPtr = new WsfScriptTaskListIteratorClass::Iterator(aObjectPtr);

   // Create a return object that manages the iterator memory we just created.
   aReturnVal.SetPointer(new UtScriptRef(iterPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// ************************************************************************************************

WsfScriptTaskListIteratorClass::WsfScriptTaskListIteratorClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddMethod(ut::make_unique<HasNext>());
   AddMethod(ut::make_unique<Next>());
   AddMethod(ut::make_unique<HasPrev>());
   AddMethod(ut::make_unique<Prev>());
   AddMethod(ut::make_unique<Key>());
   AddMethod(ut::make_unique<Data>());
}

// virtual
WsfScriptTaskListIteratorClass::~WsfScriptTaskListIteratorClass() {}

// virtual
void WsfScriptTaskListIteratorClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfScriptTaskListIteratorClass::Iterator*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskListIteratorClass, WsfScriptTaskListIteratorClass::Iterator, HasNext, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->HasNext());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskListIteratorClass, WsfScriptTaskListIteratorClass::Iterator, Next, 0, "WsfTask", "")
{
   WsfTask* taskPtr = const_cast<WsfTask*>(aObjectPtr->Next());
   if (!aObjectPtr->IteratorValidForAccess())
   {
      UT_SCRIPT_ABORT(aObjectPtr->IteratorErrorString());
   }
   aReturnVal.SetPointer(new UtScriptRef(taskPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskListIteratorClass, WsfScriptTaskListIteratorClass::Iterator, HasPrev, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->HasPrev());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskListIteratorClass, WsfScriptTaskListIteratorClass::Iterator, Prev, 0, "WsfTask", "")
{
   WsfTask* taskPtr = const_cast<WsfTask*>(aObjectPtr->Prev());
   if (!aObjectPtr->IteratorValidForAccess())
   {
      UT_SCRIPT_ABORT(aObjectPtr->IteratorErrorString());
   }
   aReturnVal.SetPointer(new UtScriptRef(taskPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskListIteratorClass, WsfScriptTaskListIteratorClass::Iterator, Key, 0, "int", "")
{
   if (!aObjectPtr->IteratorValidForAccess())
   {
      UT_SCRIPT_ABORT(aObjectPtr->IteratorErrorString());
   }
   aReturnVal.SetInt(aObjectPtr->Key());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTaskListIteratorClass, WsfScriptTaskListIteratorClass::Iterator, Data, 0, "WsfTask", "")
{
   if (!aObjectPtr->IteratorValidForAccess())
   {
      UT_SCRIPT_ABORT(aObjectPtr->IteratorErrorString());
   }
   WsfTask* taskPtr = const_cast<WsfTask*>(aObjectPtr->Data());
   aReturnVal.SetPointer(new UtScriptRef(taskPtr, aReturnClassPtr));
}

WsfScriptTaskListIteratorClass::Iterator::Iterator(ScriptTaskList* aTaskListPtr)
   : mTaskListPtr(aTaskListPtr)
   , mCurrentKey(-1)
   , mVectorSize((aTaskListPtr != nullptr) ? aTaskListPtr->size() : 0)
{
}

// virtual
WsfScriptTaskListIteratorClass::Iterator::~Iterator() {}

// virtual
bool WsfScriptTaskListIteratorClass::Iterator::HasNext()
{
   return (mCurrentKey + 1 < (int)mVectorSize);
}

// virtual
const WsfTask* WsfScriptTaskListIteratorClass::Iterator::Next()
{
   if (mCurrentKey + 1 < (int)mVectorSize)
   {
      ++mCurrentKey;
      return Data();
   }
   return nullptr;
}

// virtual
bool WsfScriptTaskListIteratorClass::Iterator::HasPrev()
{
   return (mCurrentKey > 0);
}

// virtual
const WsfTask* WsfScriptTaskListIteratorClass::Iterator::Prev()
{
   if (mCurrentKey > 0)
   {
      --mCurrentKey;
      return Data();
   }
   return nullptr;
}

// virtual
int WsfScriptTaskListIteratorClass::Iterator::Key() const
{
   return mCurrentKey;
}

// virtual
const WsfTask* WsfScriptTaskListIteratorClass::Iterator::Data() const
{
   if (IteratorValidForAccess())
   {
      return ((*mTaskListPtr)[mCurrentKey]).Get();
   }
   return nullptr;
}

// ------------------------------------------------------------------------------------------------

//! Register the types ('class' objects) for the scripting systems.
//!
//! This is invoked once by WsfScriptManager to create the 'class' objects that
//! are implemented by this file.
// static
void WsfTask::RegisterScriptTypes(UtScriptTypes* aScriptTypesPtr)
{
   aScriptTypesPtr->Register(ut::make_unique<WsfScriptTaskClass>("WsfTask", aScriptTypesPtr));
   aScriptTypesPtr->Register(ut::make_unique<WsfScriptTaskListClass>("WsfTaskList", aScriptTypesPtr));
   aScriptTypesPtr->Register(ut::make_unique<WsfScriptTaskListIteratorClass>("WsfTaskListIterator", aScriptTypesPtr));
}
