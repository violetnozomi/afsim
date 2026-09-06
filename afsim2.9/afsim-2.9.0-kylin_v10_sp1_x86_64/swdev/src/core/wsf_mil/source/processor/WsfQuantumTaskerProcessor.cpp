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

#include "WsfQuantumTaskerProcessor.hpp"

#include <iostream>
#include <sstream>

#include "UtDictionary.hpp"
#include "UtLog.hpp"
#include "UtStringUtil.hpp"
#include "WsfAssetPerception.hpp"
#include "WsfComm.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMilComponentRoles.hpp"
#include "WsfPerceptionProcessor.hpp"
#include "WsfPlatform.hpp"
#include "WsfQuantumAllocator.hpp"
#include "WsfQuantumEvaluator.hpp"
#include "WsfQuantumGenerator.hpp"
#include "WsfQuantumTask.hpp"
#include "WsfQuantumTaskerObserver.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfTask.hpp"
#include "WsfTaskAssignMessage.hpp"
#include "WsfTaskObserver.hpp"
#include "WsfTaskStatusMessage.hpp"
#include "WsfTrackManager.hpp"
#include "script/WsfScriptContext.hpp"

WsfQuantumTaskerProcessor::WsfQuantumTaskerProcessor(WsfScenario& aScenario)
   : WsfTaskManager(aScenario, "WsfQuantumTaskerProcessor")
   , WsfQuantumMatrix()
   , mAssetRep(cPLATFORM)
   , mReallocateStrategy(cSTATIC)
   , mIgnoreAllyTracks(false)
   , mAssetsId(0)
   , mTasksId(0)
   , mPrevRejectCount(0)
   , mPerceptionProcessorPtr(nullptr)
   , mRejectedTaskMap()
   , mRejectedTasks()
   , mCompletedTaskTimes()
   , mAllocationMap()
   , mFilterAssignedTasks(false)
   , mFilterRejectedTasks(false)
   , mFilterCompletedTasks(true)
   , mUpdateAssignments(false)
{
}

WsfQuantumTaskerProcessor::WsfQuantumTaskerProcessor(WsfScenario&       aScenario,
                                                     const std::string& aPartClass,
                                                     const std::string& aPartName)
   : WsfTaskManager(aScenario, aPartClass)
   , WsfQuantumMatrix()
   , mAssetRep(cPLATFORM)
   , mReallocateStrategy(cSTATIC)
   , mIgnoreAllyTracks(false)
   , mAssetsId(0)
   , mTasksId(0)
   , mPrevRejectCount(0)
   , mPerceptionProcessorPtr(nullptr)
   , mRejectedTaskMap()
   , mRejectedTasks()
   , mCompletedTaskTimes()
   , mAllocationMap()
   , mFilterAssignedTasks(false)
   , mFilterRejectedTasks(false)
   , mFilterCompletedTasks(true)
   , mUpdateAssignments(false)
{
}


WsfQuantumTaskerProcessor::WsfQuantumTaskerProcessor(const WsfQuantumTaskerProcessor& aSrc)
   : WsfTaskManager(aSrc)
   , WsfQuantumMatrix(aSrc)
   , mAssetRep(aSrc.mAssetRep)
   , mReallocateStrategy(aSrc.mReallocateStrategy)
   , mIgnoreAllyTracks(aSrc.mIgnoreAllyTracks)
   , mAssetsId(aSrc.mAssetsId)
   , mTasksId(aSrc.mTasksId)
   , mPrevRejectCount(aSrc.mPrevRejectCount)
   , mPerceptionProcessorPtr(nullptr)
   , mGeneratorPtr(aSrc.mGeneratorPtr)
   , mEvaluatorPtr(aSrc.mEvaluatorPtr)
   , mAllocatorExtraTasksPtr(aSrc.mAllocatorExtraTasksPtr)
   , mAllocatorExtraAssetsPtr(aSrc.mAllocatorExtraAssetsPtr)
   , mAllocators(aSrc.mAllocators)
   , mRejectedTaskMap(aSrc.mRejectedTaskMap)
   , mRejectedTasks(aSrc.mRejectedTasks)
   , mCompletedTaskTimes(aSrc.mCompletedTaskTimes)
   , mAllocationMap(aSrc.mAllocationMap)
   , mFilterAssignedTasks(aSrc.mFilterAssignedTasks)
   , mFilterRejectedTasks(aSrc.mFilterRejectedTasks)
   , mFilterCompletedTasks(aSrc.mFilterCompletedTasks)
   , mUpdateAssignments(aSrc.mUpdateAssignments)
{
}

WsfQuantumTaskerProcessor::~WsfQuantumTaskerProcessor()
{
   for (auto& task : mTasks)
   {
      delete task;
   }
   mTasks.clear();

   // clean up cloned objects in allocation mapping
   std::map<int, std::set<WsfAssetPerception*>>::iterator it;
   for (it = mAllocationMap.begin(); it != mAllocationMap.end(); ++it)
   {
      std::set<WsfAssetPerception*>::iterator it2;
      for (it2 = it->second.begin(); it2 != it->second.end(); ++it2)
      {
         delete (*it2);
      }
   }
}

// virtual
WsfProcessor* WsfQuantumTaskerProcessor::Clone() const
{
   return new WsfQuantumTaskerProcessor(*this);
}

// virtual
bool WsfQuantumTaskerProcessor::PreInitialize(double aSimTime)
{
   //// Capture the pointer to the perception processor, or create one if it doesn't exist.
   // mPerceptionProcessorPtr = WsfPerceptionProcessor::GetPerceptionProcessor(*GetPlatform());
   return WsfTaskManager::PreInitialize(aSimTime);
}

// virtual
bool WsfQuantumTaskerProcessor::Initialize(double aSimTime)
{
   bool ok = true;
   ok &= WsfTaskManager::Initialize(aSimTime);

   // Capture the pointer to the perception processor, or create one if it doesn't exist.
   mPerceptionProcessorPtr = WsfPerceptionProcessor::GetPerceptionProcessor(*GetPlatform());
   ok &= (mPerceptionProcessorPtr != nullptr);

   WsfScriptContext* aContextPtr = &GetScriptContext();
   ok &= (aContextPtr != nullptr);
   if (ok)
   {
      if (mGeneratorPtr != nullptr)
      {
         mGeneratorPtr->Initialize(aSimTime, aContextPtr);
      }
      if (mEvaluatorPtr != nullptr)
      {
         mEvaluatorPtr->Initialize(aSimTime, aContextPtr);
      }
      for (auto& allocator : mAllocators)
      {
         allocator->Initialize(aSimTime, aContextPtr);
      }
      if (mAllocatorExtraTasksPtr != nullptr)
      {
         mAllocatorExtraTasksPtr->Initialize(aSimTime, aContextPtr);
      }
      if (mAllocatorExtraAssetsPtr != nullptr)
      {
         mAllocatorExtraAssetsPtr->Initialize(aSimTime, aContextPtr);
      }
   }
   return ok;
}

// virtual
bool WsfQuantumTaskerProcessor::Initialize2(double aSimTime)
{
   return WsfTaskManager::Initialize2(aSimTime);
}

// virtual
bool WsfQuantumTaskerProcessor::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string cmd(aInput.GetCommand());
   if (cmd == "generator")
   {
      std::string generatorName;
      aInput.ReadValue(generatorName);
      UtStringUtil::ToLower(generatorName);
      if (generatorName == "custom")
      {
         // create a script defined generator right here
         // TODO - add or do not add to generator factory?
         std::string scriptMethod;
         aInput.ReadValue(scriptMethod);
         auto genPtr = ut::make_unique<WsfQuantumGeneratorScript>(mContextPtr, scriptMethod);
         if (genPtr->Valid())
         {
            mGeneratorPtr = std::move(genPtr);
         }
         else
         {
            throw UtInput::BadValue(aInput, "Invalid quantum generator attached: " + scriptMethod);
         }
      }
      else
      {
         mGeneratorPtr = WsfQuantumGeneratorFactory::Instance()->CreateGenerator(generatorName);
      }
   }
   else if (cmd == "evaluator")
   {
      std::string evaluatorName;
      aInput.ReadValue(evaluatorName);
      UtStringUtil::ToLower(evaluatorName);
      if (evaluatorName == "custom")
      {
         // create a script defined evaluator right here
         // TODO - add or do not add to evaluator factory?
         std::string scriptMethod;
         aInput.ReadValue(scriptMethod);
         auto evalPtr = ut::make_unique<WsfQuantumEvaluatorScript>(mContextPtr, scriptMethod);
         if (evalPtr->Valid())
         {
            mEvaluatorPtr = std::move(evalPtr);
         }
         else
         {
            throw UtInput::BadValue(aInput, "Invalid quantum evaluator attached: " + scriptMethod);
         }
      }
      else
      {
         mEvaluatorPtr = WsfQuantumEvaluatorFactory::Instance()->CreateEvaluator(evaluatorName);
      }
   }
   else if (cmd == "allocator" || cmd == "allocator_extra_tasks" || cmd == "allocator_extra_assets")
   {
      std::string allocatorName;
      aInput.ReadValue(allocatorName);
      UtStringUtil::ToLower(allocatorName);
      std::unique_ptr<WsfQuantumAllocator> allocatorPtr = nullptr;
      if (allocatorName == "custom")
      {
         // create a script defined allocator right here
         // TODO - add or do not add to allocator factory?
         std::string scriptMethod;
         aInput.ReadValue(scriptMethod);
         auto allocPtr = ut::make_unique<WsfQuantumAllocatorScript>(mContextPtr, scriptMethod);
         if (!allocPtr->Valid())
         {
            throw UtInput::BadValue(aInput, "Invalid quantum allocator attached: " + scriptMethod);
         }
         allocatorPtr = std::move(allocPtr);
      }
      else
      {
         allocatorPtr.reset(WsfQuantumAllocatorFactory::Instance()->CreateAllocator(allocatorName));
      }

      // check if the user designated a specific task type for this allocator
      std::string ttype;
      aInput.ReadValue(ttype);
      if (ttype == "type")
      {
         std::string taskTypeName;
         aInput.ReadValue(taskTypeName);
         if (allocatorPtr != nullptr)
         {
            allocatorPtr->SetTaskType(taskTypeName);
         }
      }
      else
      {
         aInput.PushBack(ttype);
      }

      if (allocatorPtr != nullptr)
      {
         if (cmd == "allocator")
         {
            mAllocators.emplace_back(std::move(allocatorPtr));
         }
         else if (cmd == "allocator_extra_tasks")
         {
            mAllocatorExtraTasksPtr = std::move(allocatorPtr);
         }
         else if (cmd == "allocator_extra_assets")
         {
            mAllocatorExtraAssetsPtr = std::move(allocatorPtr);
         }
      }
   }
   else if (cmd == "asset_representation") // "platform" or "systems"
   {
      std::string repType;
      aInput.ReadValue(repType);
      UtStringUtil::ToLower(repType);
      if (repType == "resources")
      {
         mAssetRep = WsfQuantumTaskerProcessor::cRESOURCES;
      }
      else if (repType == "systems")
      {
         mAssetRep = WsfQuantumTaskerProcessor::cSYSTEMS;
      }
      else // if (repType == "platform")
      {
         mAssetRep = WsfQuantumTaskerProcessor::cPLATFORM;
      }
   }
   else if (cmd == "ignore_ally_tracks")
   {
      bool val;
      aInput.ReadValue(val);
      SetIgnoreAllyTracks(val);
   }
   else if (cmd == "reallocation_strategy")
   {
      std::string strat;
      aInput.ReadValue(strat);
      UtStringUtil::ToLower(strat);
      if (strat == "static")
      {
         SetReallocationStrategy(WsfQuantumTaskerProcessor::cSTATIC);
      }
      else if (strat == "dynamic")
      {
         SetReallocationStrategy(WsfQuantumTaskerProcessor::cDYNAMIC);
      }
      else if (strat == "response")
      {
         SetReallocationStrategy(WsfQuantumTaskerProcessor::cRESPONSE);
      }
      else if (strat == "event")
      {
         SetReallocationStrategy(WsfQuantumTaskerProcessor::cEVENT);
      }
   }
   else if (cmd == "update_assignments")
   {
      mUpdateAssignments = true;
   }
   else if (WsfTaskManager::ProcessInput(aInput)) // calls WsfScriptProcessor::ProcessInput()
   {
      // base class processed this input
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// virtual
void WsfQuantumTaskerProcessor::Update(double aSimTime)
{
   WsfScriptProcessor::Update(aSimTime); // TODO : should be first or last?

   mFilterAssignedTasks  = false;
   mFilterRejectedTasks  = false;
   mFilterCompletedTasks = true;

   // mReallocateStrategy
   //  cSTATIC  -  take tasks out of matrix if they were previously assigned &/or rejected
   //  cDYNAMIC -  all tasks & assets can be reallocated each & every update
   //  cRESPONSE-  tasks that were previously assigned & not rejected are taken out of the matrix
   //  cEVENT   -  full reallocation possible if any new tasks, any less assets, or a rejection has taken place
   // note: rejected tasks are taken out of the assigned-tasks list.

   // the matrix columns are <-- assets from the perception processor
   SetMatrixAssets(aSimTime);


   // this map stores all allocations that are to remain true when the update is complete
   std::map<int, std::set<size_t>> allocationMap;
   // the matrix rows are <-- tasks from the generator
   SetMatrixTasks(aSimTime, allocationMap);

   // populate the matrix body with values from the evaluator
   if (mEvaluatorPtr != nullptr)
   {
      std::vector<WsfLocalTrack*>& threats = WsfPerceptionProcessor::GetPlatformPerceivedThreats(*GetPlatform(), aSimTime);
      UpdateEvaluations(aSimTime, *mEvaluatorPtr, threats);
   }


   // use "regular" allocators in order they were listed, use extra task/asset allocators last

   // remove allocated TASKS  each time? YES for now
   // remove allocated ASSETS each time? NO  for now

   // allocate tasks
   if (!mAllocators.empty())
   {
      // allocations = allocatorPtr->MakeAllocations(*(WsfQuantumMatrix*)this);  //matrix
      AllocationList allocations;
      if (!mTasks.empty()) // quick bail out to avoid unnecessary looping
      {
         SetAllocations(allocations);
      }

      // if any tasks are still unassigned, use the extra task allocator (if given) to assign them
      // this will give additional tasks to assets
      if (mAllocatorExtraTasksPtr != nullptr && allocations.size() < mTasks.size())
      {
         AllocateExtraTasks(allocations);
      }

      // if any assets are still unassigned, use the extra asset allocator (if given) to assign them
      // this will give some assets the same tasks that were assigned to others
      if ((!mTasks.empty()) && (mAllocatorExtraAssetsPtr != nullptr) && (allocations.size() < mAssets.size()))
      {
         AllocateExtraAssets(allocations);
      }

      // update the maps, but clean up memory from previous update first

      for (auto& curAlloc : mAllocationMap)
      {
         std::set<WsfAssetPerception*>::iterator it2;
         for (it2 = curAlloc.second.begin(); it2 != curAlloc.second.end(); ++it2)
         {
            delete (*it2);
         }
      }
      mAllocationMap.clear();
      for (auto& allocation : allocations)
      {
         WsfAssetPerception* assetPtr = allocation.first;
         unsigned int        uid      = allocation.second->GetUniqueId();
         allocationMap[uid].insert(assetPtr->mIndex);

         std::pair<std::set<WsfAssetPerception*>::iterator, bool> val = mAllocationMap[uid].insert(assetPtr);
         if (!val.second)
         {
            // already existed, just delete here (wont be deleted later)
            delete assetPtr;
         }
      }

      // cancel any assignments for any task allocations that don't exist anymore
      //                        or for assets that aren't assigned particular tasks anymore
      // this is brute force for now, fix faster later

      auto xtli = mXmtdTaskList.begin();
      while (xtli != mXmtdTaskList.end())
      {
         WsfTask           task           = *xtli;
         std::set<size_t>& assignedAssets = allocationMap[task.GetTaskId()];
         if (assignedAssets.find(task.GetAssigneePlatformIndex()) == assignedAssets.end())
         {
            // we'll modify 'mXmtdTaskList' ourselves & then just send the message
            xtli = mXmtdTaskList.erase(xtli);
            // only sends message, does not modify 'mXmtdTaskList'
            CancelTask(aSimTime, task, true);
         }
         else
         {
            ++xtli;
         }
      }

      // Just send new assignments, for now (do not resend existing assignments)
      //                                    (unless mUpdateAssignments is true)
      // depending on the reallocation strategy, previously sent tasks are removed before the evaluator or allocator
      // look at them if strategy is dynamic or event (with changes) then previously assigned tasks will be present in
      // the allocation

      for (auto& allocation : allocations)
      {
         WsfAssetPerception* assetPtr = allocation.first;
         WsfQuantumTask*     taskPtr  = allocation.second.get();
         ////TODO - check if task resource changed?

         if (!mUpdateAssignments)
         {
            // not updating previous assignments, so check if there are any
            // e.g. if the task is already assigned to this asset, don't resend
            std::set<size_t> platformsIndices = AssigneesForTask(taskPtr);
            if (platformsIndices.find(assetPtr->mIndex) != platformsIndices.end())
            {
               // asset cleaned up later in mAllocationMap
               assetPtr         = nullptr;
               allocation.first = nullptr;
               continue;
            }
         }

         // setup task recipient platform, before sending it out
         WsfPlatform* assigneePlatformPtr = GetSimulation()->GetPlatformByIndex(assetPtr->mIndex);
         if (assigneePlatformPtr != nullptr)
         {
            taskPtr->SetAssignee(assigneePlatformPtr);
         }
         else
         {
            taskPtr->SetAssigneePlatformIndex(assetPtr->mIndex);
         }
         taskPtr->SetAssignerPlatformIndex(GetPlatform()->GetIndex());
         // setup task resource (if asset perception has more than 1 applicable type: only the 1st one found is used)
         for (auto& curSystem : assetPtr->mSystems)
         {
            if (taskPtr->GetResourceType() == ut::safe_cast<int>(curSystem.mSystemKind))
            {
               taskPtr->SetResourceName(curSystem.mSystemName);
               break;
            }
         }

         WsfLocalTrack* threatTrackPtr = mPerceptionProcessorPtr->PerceivedThreat(taskPtr->GetTargetName());
         if (threatTrackPtr != nullptr)
         {
            AssignTask(aSimTime, *(WsfTrack*)threatTrackPtr, *taskPtr);
         }
         else
         {
            WsfTrack fake;
            AssignTask(aSimTime, fake, *taskPtr);
         }
         // asset cleaned up later in mAllocationMap
         assetPtr         = nullptr;
         allocation.first = nullptr;
      }
   }

   WsfObserver::QuantumTaskerUpdate(GetSimulation())(aSimTime, this);
}

// virtual
bool WsfQuantumTaskerProcessor::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   bool processed = WsfTaskManager::ProcessMessage(aSimTime, aMessage);
   if (!processed)
   {
      processed = WsfScriptProcessor::ProcessMessage(aSimTime, aMessage);
   }
   return processed;
}

// virtual
bool WsfQuantumTaskerProcessor::ReceiveMessage(double aSimTime, const WsfMessage& aMessage)
{
   return WsfTaskManager::ReceiveMessage(aSimTime, aMessage);
}

// virtual
void WsfQuantumTaskerProcessor::PlatformDeleted(WsfPlatform* aPlatformPtr)
{
   WsfTaskManager::PlatformDeleted(aPlatformPtr);
}

// virtual
bool WsfQuantumTaskerProcessor::ProcessTaskAssignMessage(double aSimTime, const WsfTaskAssignMessage& aMessage)
{
   // Let the base class handle non-anonymous tasks (with valid tracks)
   if (!aMessage.GetTask().GetTrackId().IsNull())
   {
      return WsfTaskManager::ProcessTaskAssignMessage(aSimTime, aMessage);
   }

   // It is an anonymous task: add it to received list or update it on the list here
   // The WsfTaskManagerBase will not store anonymous tasks, the WsfQuantumTaskerProcessor must
   size_t      senderIndex = 0;
   WsfStringId commName;
   if (!GetMessageSender(aMessage, senderIndex, commName))
   {
      auto out = ut::log::error() << "Task assignment internal error 1.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Processor: " << GetName();
      return true;
   }
   if (commName == nullptr)
   {
      commName = aMessage.GetTask().GetAssignerProcessorName();
   }

   WsfTask task(aMessage.GetTask()); // need a non-const task
   auto    rtli = FindTask(mRcvdTaskList, task);
   if (rtli == mRcvdTaskList.end())
   {
      // Create the task from the assignment.
      task.SetCommName(commName);
      mRcvdTaskList.push_front(task);
      rtli = mRcvdTaskList.begin();
   }
   else
   {
      // copy over all task data (overwrite previous task)
      task.SetCommName(commName);
      *rtli = task;
   }
   WsfTask&        taskRef = *rtli;
   const WsfTrack& track   = aMessage.GetTrack();
   OnTaskAssigned(aSimTime, taskRef, track);
   WsfObserver::TaskAccepted(GetSimulation())(aSimTime, &taskRef, &track);
   TaskAssigned(aSimTime, track, taskRef);

   WsfTaskStatusMessage message(GetScenario().Strings().cACCEPTED, GetPlatform(), taskRef);
   message.SetAcknowledgeId(aMessage.GetSerialNumber());
   SendTaskMessage(aSimTime, message, taskRef.GetAssignerPlatformIndex(), commName, false);
   return true;
}

// private
bool WsfQuantumTaskerProcessor::ProcessTaskStatusMessage(double aSimTime, const WsfTaskStatusMessage& aMessage)
{
   bool messageProcessed = true;

   // If the message contains an acknowledgment of a message that was sent 'reliably' then
   // indicate that the message was successfully received by the recipient.
   if (aMessage.GetAcknowledgeId() != 0)
   {
      MessageAcknowledged(aMessage.GetAcknowledgeId());
   }

   size_t       assignerIndex = aMessage.GetAssignerPlatformIndex();
   unsigned int taskId        = aMessage.GetTaskId();
   WsfStringId  statusId      = aMessage.GetStatus();
   if ((statusId == GetScenario().Strings().cCOMPLETED) || (statusId == GetScenario().Strings().cACCEPTED) ||
       (statusId == GetScenario().Strings().cREJECTED) || (statusId == GetScenario().Strings().cIN_PROGRESS))
   {
      size_t            assigneeIndex = aMessage.GetAssigneePlatformIndex();
      WsfStringId       assigneeName  = aMessage.GetAssigneePlatformName();
      const WsfTrackId& trackId       = aMessage.GetTrackId();
      WsfStringId       taskType      = aMessage.GetTaskType();
      WsfStringId       resourceName  = aMessage.GetResourceName();
      if (mShowTaskMessages)
      {
         auto out = ut::log::info() << "Received status for task.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Processor: " << GetName();
         out.AddNote() << "Status Id: " << statusId;
         out.AddNote() << "Task: " << trackId;
         out.AddNote() << "Task Type: " << taskType;
         out.AddNote() << "Assignee: " << assigneeName;
         out.AddNote() << "Resource: " << resourceName;
      }

      // Find the task that was sent to the assignee
      WsfTask* taskPtr = nullptr;
      auto     xtli    = FindTask(mXmtdTaskList, taskId, assigneeIndex);
      if (xtli != mXmtdTaskList.end())
      {
         taskPtr = &(*xtli);
      }

      if (taskPtr == nullptr)
      {
         bool showWarning = true;
         if (statusId == GetScenario().Strings().cCOMPLETED)
         {
            showWarning = (!DropFromPurgedTaskList(mPurgedXmtdTaskList, assignerIndex, taskId));
         }
         if (showWarning)
         {
            auto out = ut::log::warning() << "Could not find task.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Processor: " << GetName();
            out.AddNote() << "StatusId: " << statusId;
            out.AddNote() << "Track Id: " << trackId;
            out.AddNote() << "Task Type: " << taskType;
            out.AddNote() << "Assignee: " << assigneeName;
            out.AddNote() << "Resource: " << resourceName;
         }
         return false;
      }

      // TODO - record status on an assignment basis (not task basis).
      //        one task could be assigned to many people!
      taskPtr->SetUpdateTime(aSimTime);
      taskPtr->SetStatus(statusId);
      taskPtr->SetSubStatus(aMessage.GetSubStatus());

      // TODO - send track updates on an assignment basis (not task basis).
      //        one task could be assigned to many people!
      //  If the task has been accepted then start sending periodic track updates.
      if ((statusId == GetScenario().Strings().cACCEPTED) && (mTrackUpdateStrategy == cDEFAULT) &&
          (mTrackUpdateInterval > 0.0))
      {
         GetSimulation()->AddEvent(
            ut::make_unique<WsfTaskManager::TrackUpdateEvent>(aSimTime + mTrackUpdateInterval, this, taskPtr->GetTaskId()));
      }

      // If an assignee has indicated that it has completed the task then we must acknowledge the message.
      if (statusId == GetScenario().Strings().cCOMPLETED)
      {
         WsfTaskStatusMessage message(GetScenario().Strings().cACKNOWLEDGE_COMPLETE, GetPlatform(), *taskPtr);
         message.SetIsFromAssignee(false);
         message.SetAcknowledgeId(aMessage.GetSerialNumber());
         SendTaskMessage(aSimTime, message, taskPtr->GetAssigneePlatformIndex(), taskPtr->GetCommName(), false);
      }

      // Purge the task if no longer needed.
      if ((statusId == GetScenario().Strings().cCOMPLETED) || (statusId == GetScenario().Strings().cREJECTED))
      {
         // NOTE - In order to avoid problems in callbacks, the task is removed from the transmitted task list before
         // proceeding.
         //        A temporary list is used to store the task before deletion.
         TaskList removedTaskList;
         removedTaskList.splice(removedTaskList.end(), mXmtdTaskList, xtli);

         OnTaskCompleted(aSimTime, *taskPtr);

         TaskCompleted(aSimTime, *taskPtr); // protected virtual method (for any derived class processing)

         PurgeTransmittedTask(aSimTime, *taskPtr);
      }
   }
   else if (statusId == GetScenario().Strings().cACKNOWLEDGE_CANCEL)
   {
      DropFromPurgedTaskList(mPurgedXmtdTaskList, assignerIndex, taskId);
   }
   else if (statusId == GetScenario().Strings().cACKNOWLEDGE_COMPLETE)
   {
      DropFromPurgedTaskList(mPurgedRcvdTaskList, assignerIndex, taskId);
   }
   else
   {
      messageProcessed = false;
   }
   return messageProcessed;
}

// virtual
bool WsfQuantumTaskerProcessor::AssignTask(double aSimTime, const WsfTask& aTask)
{
   size_t       assigneeIndex = aTask.GetAssigneePlatformIndex();
   WsfPlatform* assigneePtr   = GetSimulation()->GetPlatformByIndex(assigneeIndex);
   if (assigneePtr == nullptr)
   {
      return false;
   }
   WsfStringId commName = SelectCommDevice(assigneePtr, aTask.GetCommName());
   if (mShowTaskMessages)
   {
      auto out = ut::log::info() << "Assigning task.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Processor: " << GetName();
      out.AddNote() << "Task: " << aTask.GetTaskId();
      out.AddNote() << "Task Type: " << aTask.GetTaskType();
      out.AddNote() << "Assignee: " << assigneePtr->GetName();
      out.AddNote() << "Resource: " << aTask.GetResourceName();
      out.AddNote() << "Comm: " << commName;
   }

   // Create a transmitted task list entry if one doesn't already exist.
   // If one already exists then we just reuse it.
   auto xtli = FindTask(mXmtdTaskList, aTask.GetTaskId(), assigneeIndex);
   if (xtli == mXmtdTaskList.end())
   {
      // Create the transmitted task list entry.
      WsfTask task(aTask);
      ++mNextTaskId;
      task.Initialize(aSimTime, mNextTaskId, this);
      task.SetCommName(commName);

      ////TODO - verify / test this stuff?
      // WsfTaskResource& resource = task.GetResource();
      // WsfStringId assigneeProcessorName;
      // if ((resource.GetType() == cTASK_RESOURCE_TYPE_PROCESSOR) &&
      //     (resource.mNameId != 0) &&
      //     (assigneePtr->GetComponent<WsfProcessor>(resource.mNameId) != 0))
      //{
      //    // The selected resource is another processor (should be a task manager) on either this platform
      //    // or another platform.
      //    assigneeProcessorName = resource.mNameId;

      //   // If the assignment is to this platform then the comm name is the target processor. (See SendTaskMessage).
      //   if (assigneePtr == GetPlatform())
      //   {
      //      task.SetCommName(resource.mNameId);
      //   }
      //}
      // task.SetAssigneeProcessorName(assigneeProcessorName);

      mXmtdTaskList.push_front(task);
      xtli = mXmtdTaskList.begin();
   }
   else // Resend the task...
   {
      WsfTask task(aTask);
      task.Initialize(aSimTime, xtli->GetTaskId(), this);
      task.SetCommName(commName);
      // overwrite the task in our transmitted list (update all the task data)
      *xtli = task;
   }

   // Send the assignment message to the assignee.

   WsfTrack aTrack; // fake empty track
   WsfTask& task = *xtli;

   OnTaskAssignSent(aSimTime, task, aTrack);

   WsfObserver::TaskAssigned(GetSimulation())(aSimTime, &task, &aTrack);

   TaskAssignSent(aSimTime, aTrack, task); // protected virtual method (for any derived class processing)

   WsfTaskAssignMessage message(GetPlatform());
   message.SetTask(task);
   message.SetTrack(aTrack);
   SendTaskMessage(aSimTime, message, task.GetAssigneePlatformIndex(), task.GetCommName(), true);
   mTimeLastTaskAssigned = aSimTime;

   return true;
}

// virtual
bool WsfQuantumTaskerProcessor::AssignTask(double aSimTime, const WsfTrack& aTrack, const WsfTask& aTask, bool aLockTrack)
{
   if (!aTrack.GetTrackId().IsNull())
   {
      return WsfTaskManager::AssignTask(aSimTime, aTrack, aTask, false); // do NOT lock the local track
   }
   return AssignTask(aSimTime, aTask);
}

// =================================================================================================
// protected virtual
bool WsfQuantumTaskerProcessor::AcquireResource(double aSimTime, WsfTask& aTask, const WsfTrack& aTrack)
{
   return true;
}

// =================================================================================================
// protected virtual
bool WsfQuantumTaskerProcessor::ReleaseResource(double aSimTime, WsfTask& aTask)
{
   return true;
}

// virtual
void WsfQuantumTaskerProcessor::PurgeReceivedTask(double aSimTime, WsfTask& aTask)
{
   // this is a tricky function, see comments in WsfTaskManagerBase::PurgeReceivedTask()
   if ((aTask.GetAssignerPlatformIndex() != aTask.GetAssigneePlatformIndex()) && (aTask.GetResourceName() == nullptr))
   {
      // Potentially remove the task from the pending received task list.
      mPendingRcvdTasks.erase(aTask.GetTrackId());

      auto xtli = mXmtdTaskList.begin();
      while (xtli != mXmtdTaskList.end())
      {
         if (aTask.GetTrackId() == (*xtli).GetTrackId())
         {
            // NOTE - In order to avoid problems in callbacks, the task is removed from the transmitted task list before
            // proceeding.
            //        A temporary list is used to store the task before deletion.
            TaskList removedTaskList;
            removedTaskList.splice(removedTaskList.end(), mXmtdTaskList, xtli++);
            CancelTask(aSimTime, removedTaskList.front(), false);
         }
         else
         {
            ++xtli;
         }
      }
   }

   // If this is a 'resource task' (i.e.: one whose resource name is zero) then release the resource.
   ReleaseResource(aSimTime, aTask);

   // edit: do not perform track locking or unlocking, always allow tracks to be purged
   //       that whole section of code was removed in this virtual function

   if (mShowTaskMessages)
   {
      auto out = ut::log::info() << "Purge received task.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Processor: " << GetName();
      out.AddNote() << "Task: " << aTask.GetTrackId();
      out.AddNote() << "Task Type: " << aTask.GetTaskType();
      out.AddNote() << "Assignee: " << aTask.GetAssigneePlatformName();
      out.AddNote() << "Resource: " << aTask.GetResourceName();
      out.AddNote() << "Local Track: " << aTask.GetLocalTrackId();
   }
}

// virtual
void WsfQuantumTaskerProcessor::PurgeTransmittedTask(double aSimTime, WsfTask& aTask)
{
   // do nothing in overwritten method
}

// virtual
void WsfQuantumTaskerProcessor::UpdateTaskCorrelation(double aSimTime, WsfTask& aTask)
{
   // Attempt to locate the local track that is associated with the track that was provided with the task.
   WsfLocalTrack* trackPtr = mTrackManagerPtr->FindCorrelatedTrack(aTask.GetTrackId());
   if (trackPtr != nullptr)
   {
      aTask.SetLocalTrackId(trackPtr->GetTrackId());
      //// Lock the track to prevent it from being purged by the track manager.
      // edit: DO NOT LOCK THE LOCAL TRACK, ALLOW TRACK PURGES
      // mTrackManagerPtr->LockTrack(aSimTime, trackPtr->GetTrackId());
      mPendingRcvdTasks.erase(aTask.GetTrackId());
      TaskCorrelated(aSimTime, aTask);
      if (mShowTaskMessages)
      {
         auto out = ut::log::info() << "Task track/local track correlation:";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Processor: " << GetName();
         out.AddNote() << "Task: " << aTask.GetTrackId();
         out.AddNote() << "Task Type: " << aTask.GetTaskType();
      }
   }
}

// virtual
const char* WsfQuantumTaskerProcessor::GetScriptClassName() const
{
   return "WsfQuantumTaskerProcessor";
}

//! The list is not filtered by the task type if it is zero
std::list<WsfTask*> WsfQuantumTaskerProcessor::TaskListReceivedOfType(WsfStringId aTaskType)
{
   std::list<WsfTask*> list;
   for (WsfTask& task : mRcvdTaskList)
   {
      if ((aTaskType == nullptr) || (aTaskType == task.GetTaskType()))
      {
         list.push_back(&task);
      }
   }
   return list;
}


//! The list is not filtered by the WsfTrackId if it is null
std::list<WsfTask*> WsfQuantumTaskerProcessor::TaskListReceivedForTrack(const WsfTrackId& aTrackId)
{
   std::list<WsfTask*> list;
   for (WsfTask& task : mRcvdTaskList)
   {
      if (aTrackId.IsNull() || (aTrackId == task.GetLocalTrackId()))
      {
         list.push_back(&task);
      }
   }
   return list;
}


//! The list is not filtered by the WsfTrackId if it is null
std::list<WsfTask*> WsfQuantumTaskerProcessor::TaskListReceivedOfTypeForTrack(WsfStringId       aTaskType,
                                                                              const WsfTrackId& aTrackId)
{
   std::list<WsfTask*> list;
   for (WsfTask& task : mRcvdTaskList)
   {
      if ((aTrackId.IsNull() || (aTrackId == task.GetLocalTrackId())) &&
          ((aTaskType == nullptr) || (aTaskType == task.GetTaskType())))
      {
         list.push_back(&task);
      }
   }
   return list;
}


//! The list is not filtered by the ResourceType if it is cNONE
std::list<WsfTask*> WsfQuantumTaskerProcessor::TaskListReceivedForResourceType(int aResourceType)
{
   std::list<WsfTask*> list;
   for (WsfTask& task : mRcvdTaskList)
   {
      if ((aResourceType == cTASK_RESOURCE_TYPE_NONE) || (aResourceType == task.GetResourceType()))
      {
         list.push_back(&task);
      }
   }
   return list;
}


//! The list is not filtered by the resource name if it is zero
std::list<WsfTask*> WsfQuantumTaskerProcessor::TaskListReceivedForResourceName(WsfStringId aResourceName)
{
   std::list<WsfTask*> list;
   for (WsfTask& task : mRcvdTaskList)
   {
      if ((aResourceName == nullptr) || (aResourceName == task.GetResourceName()))
      {
         list.push_back(&task);
      }
   }
   return list;
}


std::list<WsfTask*> WsfQuantumTaskerProcessor::TaskListAssignedOfType(WsfStringId aTaskType)
{
   std::list<WsfTask*> list;
   for (WsfTask& task : mXmtdTaskList)
   {
      if ((aTaskType == nullptr) || (aTaskType == task.GetTaskType()))
      {
         list.push_back(&task);
      }
   }
   return list;
}


std::list<WsfTask*> WsfQuantumTaskerProcessor::TaskListAssignedForTrack(const WsfTrackId& aTrackId)
{
   std::list<WsfTask*> list;
   for (WsfTask& task : mXmtdTaskList)
   {
      if (aTrackId.IsNull() || (aTrackId == task.GetLocalTrackId()))
      {
         list.push_back(&task);
      }
   }
   return list;
}


std::list<WsfTask*> WsfQuantumTaskerProcessor::TaskListAssignedForResourceType(int aResourceType)
{
   std::list<WsfTask*> list;
   for (WsfTask& task : mXmtdTaskList)
   {
      if ((aResourceType == cTASK_RESOURCE_TYPE_NONE) || (aResourceType == task.GetResourceType()))
      {
         list.push_back(&task);
      }
   }
   return list;
}


std::list<WsfTask*> WsfQuantumTaskerProcessor::TaskListAssignedForResourceName(WsfStringId aResourceName)
{
   std::list<WsfTask*> list;
   for (WsfTask& task : mXmtdTaskList)
   {
      if ((aResourceName == nullptr) || (aResourceName == task.GetResourceName()))
      {
         list.push_back(&task);
      }
   }
   return list;
}

//! Returns the set of platform indexes that represent all task assignees
//! The set will be empty if the task is not currently assigned
std::set<size_t> WsfQuantumTaskerProcessor::AssigneesForTask(WsfQuantumTask* aTaskPtr)
{
   std::set<size_t> assigneeIndices;
   for (auto& xmtdTask : mXmtdTaskList)
   {
      if (xmtdTask.GetTaskId() == aTaskPtr->GetTaskId())
      {
         assigneeIndices.insert(xmtdTask.GetAssigneePlatformIndex());
      }
   }
   return assigneeIndices;
}


std::set<WsfAssetPerception*> WsfQuantumTaskerProcessor::AssetAssigneesForTask(WsfQuantumTask* aTaskPtr)
{
   auto it = mAllocationMap.find(aTaskPtr->GetUniqueId()); // GetTaskId() should == UniqueId() for WsfQuantumTask objects
   if (it != mAllocationMap.end())
   {
      return it->second;
   }
   return std::set<WsfAssetPerception*>(); // empty
}


//! Sends a WsfTaskStatusMessage with status cIN_PROGRESS to the task assigner
bool WsfQuantumTaskerProcessor::ReportTaskProgress(double aSimTime, WsfTask& aTask, WsfStringId aSubStatus)
{
   return WsfTaskManager::ReportTaskProgress(aSimTime,
                                             aTask.GetTrackId(),
                                             aTask.GetTaskType(),
                                             aTask.GetResourceName(),
                                             aSubStatus);
}


//! Sends a WsfTaskStatusMessage with status cCOMPLETED to the task assigner
//! (The provided sub-status is typically cSUCCESSFUL or cUNSUCCESSFUL)
//! This removes the task from the received task list
//! All this triggers the TaskCompleted() callback later on
bool WsfQuantumTaskerProcessor::ReportTaskComplete(double aSimTime, WsfTask& aTask, WsfStringId aSubStatus)
{
   return WsfTaskManager::ReportTaskComplete(aSimTime,
                                             aTask.GetTrackId(),
                                             aTask.GetTaskType(),
                                             aTask.GetResourceName(),
                                             aSubStatus);
}


//! Sends a WsfTaskStatusMessage with status cREJECTED to the task assigner
//! Removes the task from the received task list
//! All this triggers the TaskCompleted() callback later on
bool WsfQuantumTaskerProcessor::RejectTask(double aSimTime, WsfTask& aTask)
{
   size_t       assignerIndex = aTask.GetAssignerPlatformIndex();
   WsfPlatform* assignerPtr   = GetSimulation()->GetPlatformByIndex(assignerIndex);
   WsfStringId  commName      = SelectCommDevice(assignerPtr, nullptr);
   // If the resulting comm name is 0 it means the assigner and assignee are on the same platform.
   // Set the comm name to the assigning processor name so status messages get routed correctly (see SendTaskMessage).
   if (commName == nullptr)
   {
      commName = aTask.GetAssignerProcessorName();
   }

   WsfTaskStatusMessage message(GetScenario().Strings().cREJECTED, GetPlatform(), aTask);
   // message.SetAcknowledgeId(aMessage.GetSerialNumber());  //do we need this?  acknowledged serial number?
   SendTaskMessage(aSimTime, message, assignerIndex, commName, false);

   // also needs to be removed from our own received task list
   if (mShowTaskMessages)
   {
      auto out = ut::log::info() << "Rejecting task.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Processor: " << GetName();
      out.AddNote() << "Task: " << aTask.GetTrackId();
      out.AddNote() << "Task Type: " << aTask.GetTaskType();
      out.AddNote() << "Resource: " << aTask.GetResourceName();
   }
   // Find and purge the task from the local task list.
   auto rtli = FindTask(mRcvdTaskList, aTask); // GetPlatform()->GetIndex(), trackId, taskType, resourceName);
   if (rtli != mRcvdTaskList.end())
   {
      WsfTask& task = *rtli;
      // NOTE - In order to avoid problems in callbacks, the task is removed from the received task list before proceeding.
      //        A temporary list is used to store the task before deletion.
      TaskList removedTaskList;
      removedTaskList.splice(removedTaskList.end(), mRcvdTaskList, rtli);
      PurgeReceivedTask(aSimTime, task);
   }
   else if (!DropFromPurgedTaskList(mPurgedRcvdTaskList, assignerIndex, aTask.GetTaskId()))
   {
      auto out = ut::log::error() << "Cannot find task to reject.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Processor: " << GetName();
      out.AddNote() << "Task: " << aTask.GetTrackId();
      out.AddNote() << "Task Type: " << aTask.GetTaskType();
      out.AddNote() << "Resource: " << aTask.GetResourceName();
   }
   return true;
}


//! Callback used to record which tasks are rejected or completed
// virtual
void WsfQuantumTaskerProcessor::TaskCompleted(double aSimTime, WsfTask& aTask)
{
   // int substatus = aTask.GetSubStatus();
   if (GetScenario().Strings().cREJECTED == aTask.GetStatus())
   {
      // it was rejected, store that fact!
      size_t assetId = aTask.GetAssigneePlatformIndex();
      int taskId = aTask.GetTaskId(); // GetTaskId() will be WsfQuantumTask::UniqueId() if the task is a WsfQuantumTask
      mRejectedTaskMap[assetId][taskId] = 1;
      mRejectedTasks[taskId]            = 1;
   }
   else if (GetScenario().Strings().cCOMPLETED == aTask.GetStatus())
   {
      int taskId = static_cast<int>(
         aTask.GetTaskId()); // GetTaskId() will be WsfQuantumTask::UniqueId() if the task is a WsfQuantumTask
      mCompletedTaskTimes[taskId] = aSimTime;
   }

   WsfTaskManager::TaskCompleted(aSimTime, aTask);
}


//! Returns whether or not this asset has ever rejected this task
bool WsfQuantumTaskerProcessor::Rejected(WsfAssetPerception* aAssetPtr, WsfQuantumTask* aTaskPtr)
{
   if (mRejectedTaskMap.find(aAssetPtr->mIndex) != mRejectedTaskMap.end())
   {
      std::map<int, int>& aMap = mRejectedTaskMap[aAssetPtr->mIndex];
      if (aMap.find(aTaskPtr->GetUniqueId()) != aMap.end())
      {
         return true;
      }
   }
   return false;
}


//! Returns whether or not this asset has ever rejected any tasks
bool WsfQuantumTaskerProcessor::RejectedAnyTasks(WsfAssetPerception* aAssetPtr)
{
   return (mRejectedTaskMap.find(aAssetPtr->mIndex) != mRejectedTaskMap.end());
}


//! Returns whether or not this task has ever been rejected
bool WsfQuantumTaskerProcessor::RejectedByAnyAsset(WsfQuantumTask* aTaskPtr)
{
   return (mRejectedTasks.find(aTaskPtr->GetUniqueId()) != mRejectedTasks.end());
}


//! Returns whether or not this task has been reported as completed after the given sim time
bool WsfQuantumTaskerProcessor::CompletedAfter(WsfQuantumTask* aTaskPtr, double aSimTime)
{
   auto it = mCompletedTaskTimes.find(aTaskPtr->GetUniqueId());
   return (it != mCompletedTaskTimes.end() && it->second >= aSimTime);
}


//! This overwrites the WsfQuantumMatrix method so it can filter out any pairings between assets and tasks they have rejected
// virtual
void WsfQuantumTaskerProcessor::UpdateEvaluations(double                       aSimTime,
                                                  WsfQuantumEvaluator&         aEvaluator,
                                                  std::vector<WsfLocalTrack*>& aPerception)
{
   if (mFilterRejectedTasks)
   {
      WsfQuantumMatrix::UpdateEvaluations(aSimTime, aEvaluator, aPerception);
      return;
   }

   size_t rows = mTasks.size();
   size_t cols = mAssets.size();

   for (size_t j = 0; j < cols; ++j)
   {
      if (RejectedAnyTasks(mAssets[j]))
      {
         // have to check each task now
         for (size_t i = 0; i < rows; ++i)
         {
            if (Rejected(mAssets[j], mTasks[i]))
            {
               mValues[i][j]  = 0.0; // TODO - is this sufficient for rejected tasks?
               mProfits[i][j] = 0.0; // TODO - is this sufficient for rejected tasks?
            }
            else
            {
               WsfStringId    targetName = mTasks[i]->GetTargetName();
               WsfLocalTrack* threatPtr  = nullptr;
               for (auto& threatTrack : aPerception)
               {
                  if (threatTrack->GetTargetName() == targetName)
                  {
                     threatPtr = threatTrack;
                     break;
                  }
               }
               mValues[i][j]  = aEvaluator.Evaluate(aSimTime, mTasks[i], mAssets[j], threatPtr);
               mProfits[i][j] = mValues[i][j] * mTasks[i]->GetPriority();
            }
         }
      }
      else
      {
         // go speedily through the task list
         for (unsigned int i = 0; i < rows; ++i)
         {
            WsfStringId    targetName = mTasks[i]->GetTargetName();
            WsfLocalTrack* threatPtr  = nullptr;
            for (auto& threatTrack : aPerception)
            {
               if (threatTrack->GetTargetName() == targetName)
               {
                  threatPtr = threatTrack;
                  break;
               }
            }
            mValues[i][j]  = aEvaluator.Evaluate(aSimTime, mTasks[i], mAssets[j], threatPtr);
            mProfits[i][j] = mValues[i][j] * mTasks[i]->GetPriority();
         }
      }
   }
}


WsfTaskManager::TaskList::iterator WsfQuantumTaskerProcessor::FindTask(TaskList& aTaskList, WsfTask& aTask)
{
   return WsfTaskManager::FindTask(aTaskList,
                                   aTask.GetAssigneePlatformIndex(),
                                   aTask.GetTrackId(),
                                   aTask.GetTaskType(),
                                   aTask.GetResourceName());
}


void WsfQuantumTaskerProcessor::SetMatrixAssets(double aSimTime)
{
   std::vector<WsfAssetPerception*>& assets = WsfPerceptionProcessor::GetPlatformPerceivedAssets(*GetPlatform(), aSimTime);
   if (assets.empty())
   {
      ClearAssets();
      return;
   }
   if (mAssetRep == cRESOURCES)
   {
      // break apart platform perception into a list of resources (each available weapon & sensor slot represented separately)
      std::vector<WsfAssetPerception*> resAssets;
      for (auto& asset : assets)
      {
         for (unsigned int j = 0; j < asset->mSystems.size(); ++j)
         {
            // create a new asset perception for each ready assignment available on an existing system type
            for (unsigned int k = 0; k < asset->mSystems[j].mReadyAssignment; ++k)
            {
               resAssets.push_back(
                  new WsfAssetPerception(*asset, WsfAssetPerception::AssetSystem(asset->mSystems[j], 1, 1.0)));
            }
         }
         resAssets.push_back(
            new WsfAssetPerception(*asset, WsfAssetPerception::AssetSystem())); // an empty system (represents empty
                                                                                // platform, no specific system)
      }
      SetAssets(resAssets); // matrix
   }
   else if (mAssetRep == cSYSTEMS)
   {
      // break apart platform perception into a list of systems (located at platform location)
      std::vector<WsfAssetPerception*> sysAssets;
      for (auto& asset : assets)
      {
         // create a new asset perception for each existing system type
         for (unsigned int j = 0; j < asset->mSystems.size(); ++j)
         {
            sysAssets.push_back(new WsfAssetPerception(*asset, asset->mSystems[j]));
         }
         sysAssets.push_back(
            new WsfAssetPerception(*asset, WsfAssetPerception::AssetSystem())); // an empty system (represents empty
                                                                                // platform, no specific system)
      }
      SetAssets(sysAssets); // matrix
   }
   else // if (mAssetRep == cPLATFORM)
   {
      std::vector<WsfAssetPerception*> platAssets;
      // for (size_t i = 0; i < assets.size(); ++i)
      for (auto& asset : assets)
      {
         // clone the asset perception (represents the whole platform already)
         platAssets.push_back(asset->Clone());
      }
      SetAssets(platAssets); // matrix
   }
}

void WsfQuantumTaskerProcessor::SetMatrixTasks(double aSimTime, std::map<int, std::set<size_t>>& aAllocationMap)
{
   // the matrix rows are <-- tasks from the generator
   if (mGeneratorPtr != nullptr)
   {
      std::vector<WsfLocalTrack*> threats = WsfPerceptionProcessor::GetPlatformPerceivedThreats(*GetPlatform(), aSimTime);
      if (mIgnoreAllyTracks)
      {
         auto threatIter = threats.begin();
         while (threatIter != threats.end())
         {
            if (!mPerceptionProcessorPtr->PossibleEnemy(*threatIter))
            {
               threatIter = threats.erase(threatIter);
            }
            else
            {
               ++threatIter;
            }
         }
      }

      std::vector<WsfQuantumTask*> tasks = mGeneratorPtr->GenerateTasks(aSimTime, threats, Assets());
      for (auto& task : tasks)
      {
         task->SetUniqueId(task->GetUniqueId()); // gives it a unique ID if one does not already exist
      }

      if (mReallocateStrategy == cSTATIC)
      {
         mFilterAssignedTasks = true;
         mFilterRejectedTasks = true;
      }
      else if (mReallocateStrategy == cRESPONSE)
      {
         mFilterAssignedTasks = true;
      }
      else if (mReallocateStrategy == cEVENT)
      {
         // look for changes in the task list
         std::vector<unsigned int> tgtIds;
         for (auto& task : tasks)
         {
            tgtIds.push_back(task->GetUniqueId());
         }
         std::sort(tgtIds.begin(), tgtIds.end());
         std::ostringstream oss;
         const char*        separator = "";
         for (auto tgtId : tgtIds)
         {
            oss << separator << tgtId;
            separator = ",";
         }
         int tasksId = UtDictionary::GetDictionary().GetNumber(oss.str()); // a unique ID based on the task target names

         // look for changes in the asset list
         std::vector<WsfAssetPerception*>& assets = Assets(); // from matrix
         std::vector<size_t>               assetIds;
         for (auto& asset : assets)
         {
            assetIds.push_back(asset->mIndex);
         }

         std::sort(assetIds.begin(), assetIds.end());
         std::ostringstream oss2;
         separator = "";
         for (auto assetId : assetIds)
         {
            oss2 << separator << assetId;
            separator = ",";
         }
         int assetsId = UtDictionary::GetDictionary().GetNumber(oss2.str()); // a unique ID based on the task target names

         int curRejectCount = static_cast<int>(mRejectedTasks.size());
         // if anything changed, don't remove any tasks from the list
         if (tasksId != mTasksId || assetsId != mAssetsId || mPrevRejectCount != curRejectCount)
         {
            mAssetsId        = assetsId;
            mTasksId         = tasksId;
            mPrevRejectCount = curRejectCount;
         }
         else
         {
            mFilterAssignedTasks = true;
            mFilterRejectedTasks = true;
         }
      }

      if (mFilterAssignedTasks)
      {
         // edit the task list to take out tasks that are already assigned
         auto taskIter = tasks.begin();
         while (taskIter != tasks.end())
         {
            bool            taskAllocated = false;
            WsfQuantumTask* curTaskPtr    = *taskIter;
            for (auto& xmtdTask : mXmtdTaskList)
            {
               ////TODO - increase fidelity of checking for identical tasks!!!
               // if (tasks[i]->GetTargetName() == it->GetTargetName() &&
               //     tasks[i]->GetResourceType() == it->GetResourceType())
               //{
               // TODO - test this out more
               if (xmtdTask.GetTaskId() == curTaskPtr->GetTaskId()) //  tasks[i]->GetTaskId())
               {
                  // save the allocation in this map so it remains
                  aAllocationMap[curTaskPtr->GetUniqueId()].insert(xmtdTask.GetAssigneePlatformIndex());
                  taskAllocated = true;

                  // resend the assignment if it is to be updated (perhaps new data is in the task?)
                  if (mUpdateAssignments)
                  {
                     // resend the task assignment message here, using the newly generated task
                     // because perhaps some data has changed, maybe aux data?
                     // populate the generated task data before using it to overwrite the existing sent task
                     curTaskPtr->mAssigneePlatformName = xmtdTask.mAssigneePlatformName;
                     curTaskPtr->SetAssigneePlatformIndex(xmtdTask.GetAssigneePlatformIndex());
                     curTaskPtr->SetAssigneeProcessorName(xmtdTask.GetAssigneeProcessorName());
                     curTaskPtr->SetAssignerPlatformIndex(xmtdTask.GetAssignerPlatformIndex());
                     curTaskPtr->SetResourceName(xmtdTask.GetResourceName());
                     // TODO - should we use it->GetTrackId() instead of the perception processor track?
                     WsfLocalTrack* threatTrackPtr = mPerceptionProcessorPtr->PerceivedThreat(curTaskPtr->GetTargetName());
                     // NOTE: AssignTask() copies the task, we are free to delete it after it is assigned
                     if (threatTrackPtr != nullptr)
                     {
                        AssignTask(aSimTime, *(WsfTrack*)threatTrackPtr, *curTaskPtr);
                     }
                     else
                     {
                        WsfTrack fake;
                        AssignTask(aSimTime, fake, *curTaskPtr);
                     }
                  }
               }
            }
            // delete task from further consideration
            if (taskAllocated)
            {
               delete curTaskPtr; // do not leak this memory, delete the task
               taskIter = tasks.erase(taskIter);
            }
            else
            {
               ++taskIter;
            }
         }
      }

      if (mFilterRejectedTasks)
      {
         auto taskIter = tasks.begin();
         while (taskIter != tasks.end())
         {
            if (RejectedByAnyAsset(*taskIter))
            {
               delete *taskIter; // do not leak this memory, delete the task
               taskIter = tasks.erase(taskIter);
            }
            else
            {
               ++taskIter;
            }
         }
      }

      if (mFilterCompletedTasks)
      {
         //// CAUTION: Next loop cannot be converted to use size_t without more work
         for (int i = static_cast<int>(tasks.size()) - 1; i >= 0; i--)
         {
            //   //find the track for this task so we can figure out if this has track has updated after somebody
            //   reported this task complete
            //   //if the track has a more recent update, then ignore the "complete" that was reported
            //   //if the "complete" report is more recent, then do not re-assign this task
            for (auto& threat : threats)
            {
               if (threat->GetTrackId() == tasks[i]->GetTrackId())
               {
                  if (CompletedAfter(tasks[i], threat->GetUpdateTime()))
                  {
                     delete tasks[i]; // do not leak this memory, delete the task
                     tasks.erase(tasks.begin() + i);
                  }
                  break;
               }
            }
         }
      }
      SetTasks(tasks); // matrix
   }
}

void WsfQuantumTaskerProcessor::SetAllocations(AllocationList& aList)
{
   // use a temporary matrix & take out tasks we should not allocate (if a task type was specified)
   WsfQuantumMatrix* copyMatrixPtr = new WsfQuantumMatrix(*this);

   // for each defined allocator
   //    if a task type is specified
   //       perform an allocation pass for all remaining unassigned tasks of that type
   for (auto& curAllocator : mAllocators)
   {
      WsfQuantumMatrix* tempMatrixPtr = copyMatrixPtr; // just point
      // only allocate tasks of a certain type if the allocator has a type specified
      if (curAllocator->HasTaskType())
      {
         tempMatrixPtr = new WsfQuantumMatrix(*copyMatrixPtr); // makes a copy
         for (size_t j = 0; j < tempMatrixPtr->Tasks().size(); ++j)
         {
            if (tempMatrixPtr->Tasks()[j]->mTaskType != curAllocator->TaskType())
            {
               // remove this task from the subset matrix
               tempMatrixPtr->RemoveTask(j);
               --j;
            }
         }
      }

      if (!tempMatrixPtr->Tasks().empty())
      {
         AllocationList            tempAllocs = curAllocator->MakeAllocations(*tempMatrixPtr);
         std::set<WsfQuantumTask*> allocatedTasks;
         for (auto& tempAlloc : tempAllocs)
         {
            allocatedTasks.insert(tempAlloc.second.get());
            aList.emplace_back(tempAlloc.first->Clone(), tempAlloc.second);
         }

         // take out tasks from matrix
         for (size_t i = 0; i < copyMatrixPtr->Tasks().size(); ++i)
         {
            // Tasks have been cloned out of the matrix; can't check pointers
            auto it = std::find_if(std::begin(allocatedTasks),
                                   std::end(allocatedTasks),
                                   [=](WsfQuantumTask* task)
                                   { return task->GetUniqueId() == copyMatrixPtr->Tasks()[i]->GetUniqueId(); });
            if (it != std::end(allocatedTasks))
            {
               // task already allocated, remove it from temp matrix so its not allocated again
               copyMatrixPtr->RemoveTask(i);
               i--;
            }
         }
      }

      if (tempMatrixPtr != copyMatrixPtr)
      {
         // delete it?
         delete tempMatrixPtr;
      }
      // ready to repeat with next allocator?
   }
   delete copyMatrixPtr; // done with temporary matrix for regular allocators
}

void WsfQuantumTaskerProcessor::AllocateExtraTasks(AllocationList& aList)
{
   int numberLastAllocated = 1;
   // pass the unallocated tasks along with all assets into the extra-task allocator

   // must use an entire cloned matrix & remove tasks (along with their values) as they are allocated
   WsfQuantumMatrix              tempMatrix = *(WsfQuantumMatrix*)this; // this makes a copy
   std::vector<WsfQuantumTask*>& tTasks     = tempMatrix.Tasks();

   // use a set to keep track of tasks as they are allocated
   std::set<WsfQuantumTask*> allocatedTasks;
   for (auto& allocation : aList)
   {
      allocatedTasks.insert(allocation.second.get()); //(ait->second);
   }

   // vector returned by all allocation passes by the extra task allocator
   AllocationList tempAllocations;

   // make another allocation pass if anything changed & if some tasks remain unallocated
   while (numberLastAllocated > 0 && mTasks.size() > aList.size())
   {
      // remove tasks from the temp matrix if they are allocated (original allocation in this case),
      for (unsigned int i = 0; i < (unsigned int)tTasks.size(); ++i)
      {
         // Tasks have been cloned out of the matrix; can't check pointers
         auto it = std::find_if(std::begin(allocatedTasks),
                                std::end(allocatedTasks),
                                [=](WsfQuantumTask* task) { return task->GetUniqueId() == tTasks[i]->GetUniqueId(); });
         if (it != std::end(allocatedTasks))
         {
            // task already allocated, remove it from temp matrix so its not allocated again
            tempMatrix.RemoveTask(i);
            i--;
         }
      }

      tempAllocations = mAllocatorExtraTasksPtr->MakeAllocations(tempMatrix);

      numberLastAllocated = (int)tempAllocations.size();

      // add these allocations in with the rest
      for (auto& tmpAllocation : tempAllocations)
      {
         allocatedTasks.insert(tmpAllocation.second.get());
         aList.emplace_back(tmpAllocation.first->Clone(), std::move(tmpAllocation.second));
      }
   }
}


void WsfQuantumTaskerProcessor::AllocateExtraAssets(AllocationList& aList)
{
   int numberLastAllocated = 1;
   // pass the unallocated assets along with all tasks into the extra-asset allocator

   // must use an entire cloned matrix & remove assets (along with their values) as they are allocated
   WsfQuantumMatrix                  tempMatrix = *(WsfQuantumMatrix*)this; // this makes a copy
   std::vector<WsfAssetPerception*>& tAssets    = tempMatrix.Assets();

   // use a set to keep track of assets as they are allocated
   std::vector<std::pair<WsfAssetPerception*, bool>> allocatedAssets;
   for (auto& allocation : aList)
   {
      allocatedAssets.emplace_back(allocation.first, true);
   }

   // vector returned by all allocation passes by the extra task allocator
   AllocationList tempAllocations;

   // make another allocation pass if anything changed & if assets remain unallocated
   while ((numberLastAllocated > 0) && (mAssets.size() > aList.size()))
   {
      // remove tasks from the temp matrix if they are allocated (original allocation in this case),
      for (unsigned int i = 0; i < (unsigned int)tAssets.size(); ++i)
      {
         std::vector<std::pair<WsfAssetPerception*, bool>>::iterator it;
         for (it = allocatedAssets.begin(); it != allocatedAssets.end(); ++it)
         {
            if (it->second && *it->first == *tAssets[i])
            {
               // asset already allocated, remove it from temp matrix so its not allocated again
               tempMatrix.EraseAsset(i);
               i--;
               // set flag so the asset wont be matched again
               it->second = false;
               break;
            }
         }
      }

      tempAllocations = mAllocatorExtraAssetsPtr->MakeAllocations(tempMatrix);

      numberLastAllocated = (int)tempAllocations.size();

      // add these allocations in with the rest
      for (auto& tempAllocation : tempAllocations)
      {
         aList.emplace_back(tempAllocation.first->Clone(), std::move(tempAllocation.second));
         allocatedAssets.emplace_back(tempAllocation.first, true);
      }
   }
}
