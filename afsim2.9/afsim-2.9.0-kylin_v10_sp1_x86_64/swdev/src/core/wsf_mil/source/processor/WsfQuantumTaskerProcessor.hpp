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

#ifndef WSFQUANTUMTASKERPROCESSOR_HPP
#define WSFQUANTUMTASKERPROCESSOR_HPP

#include <map>
#include <string>
#include <vector>

class WsfAssetPerception;
#include "wsf_mil_export.h"
class WsfPerceptionProcessor;
class WsfPlatform;
class WsfProcessor;
class WsfQuantumAllocator;
class WsfQuantumEvaluator;
class WsfQuantumGenerator;
#include "WsfQuantumMatrix.hpp"
class WsfQuantumTask;
class WsfScriptContext;
#include "WsfQuantumAllocator.hpp"
#include "WsfTaskManager.hpp"
#include "script/WsfScriptProcessor.hpp"

//! WsfQuantumTaskerProcessor is a class for task control.
//! All tasks sent or received by this processor are intended to be compatible with the WsfTaskManager.
//!
//! This class allows the user to choose or even custom define every step of the tasking process
//! that occurs each update.  Some steps are filtered or tailored based on user selected
//! strategies, for example: a task won't be reassigned if using the cSTATIC strategy.
//! The current steps each update in the tasking process are:
//!   - Generation: tasks are generated according to asset & threat perception.
//!   - Evaluation: every theoretical asset-task pairing is given a value.
//!   - Allocation: an allocation algorithm is performed upon the value matrix.
//!   - Strategy: the reallocation strategy determines how various cases are handled:
//!      - tasks re-assignment? when rejected? whenever profitable? never? when perception changes?
//!
//! If a task reassignment is being performed, a cancel message is sent to the old assignee.
//! If a task stops being generated on update, a cancel message is sent to the old assignee.
//!
//! If a task is still generated for a stale track, but the task has been reported as completed, then
//! it will not be reassigned unless the track updates as more current than the completion message.
//!
//! Task assignment messages are only sent on first assign, they are not sent continually
class WSF_MIL_EXPORT WsfQuantumTaskerProcessor : public WsfTaskManager, public WsfQuantumMatrix
{
public:
   enum AssetRepresentationType
   {
      cPLATFORM,
      cSYSTEMS,
      cRESOURCES
   };

   enum ReallocationStrategyType
   {
      cSTATIC,   // never reallocate any tasks
      cDYNAMIC,  // reallocate any time - whenever max profit suggests it
      cRESPONSE, // only reallocate tasks that were rejected/canceled by assets
      cEVENT     // check for reallocation when a new task appears or an asset drops off (change event)
   };

   using AllocationMap = std::map<int, std::set<WsfAssetPerception*>>;

   WsfQuantumTaskerProcessor(WsfScenario& aScenario);
   WsfQuantumTaskerProcessor(WsfScenario& aScenario, const std::string& aPartClass, const std::string& aPartName);
   WsfQuantumTaskerProcessor(const WsfQuantumTaskerProcessor& aSrc);
   WsfQuantumTaskerProcessor& operator=(const WsfQuantumTaskerProcessor&) = delete;
   ~WsfQuantumTaskerProcessor() override;

   //! @name Standard processor function overwrites
   //@{
   WsfProcessor* Clone() const override;
   bool          PreInitialize(double aSimTime) override;
   bool          Initialize(double aSimTime) override;
   bool          Initialize2(double aSimTime) override;
   using WsfScriptProcessor::Initialize2;

   bool ProcessInput(UtInput& aInput) override;
   void Update(double aSimTime) override;
   bool ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;
   bool ReceiveMessage(double aSimTime, const WsfMessage& aMessage) override;
   void PlatformDeleted(WsfPlatform* aPlatformPtr) override;

   const char* GetScriptClassName() const override;
   using WsfScriptProcessor::GetName;
   using WsfScriptProcessor::GetPlatform;
   //@}

   //! @name Overwrite base message handler.
   //@{
   //! @param aSimTime     :: simulation time
   //! @param aMessage     :: message carrying the task to process
   //! @return 'true' if possible to receive & process
   bool ProcessTaskAssignMessage(double aSimTime, const WsfTaskAssignMessage& aMessage) override;
   bool ProcessTaskStatusMessage(double aSimTime, const WsfTaskStatusMessage& aMessage) override;
   //@}

   //! @name Provide task assignment methods - track not required
   //@{
   //! @param aSimTime     :: simulation time
   //! @param aTask        :: task to assign, assign message will be sent if not already
   //! @return 'true' if possible to assign
   virtual bool AssignTask(double aSimTime, const WsfTask& aTask);
   //! @param aSimTime     :: simulation time
   //! @param aTrack       :: track to be associated with the task
   //! @param aTask        :: task to assign, assign message will be sent if not already
   //! @param aLockTrack   :: parameter ignored in this function overwrite
   //! @return 'true' if possible to assign
   bool AssignTask(double aSimTime, const WsfTrack& aTrack, const WsfTask& aTask, bool aLockTrack = true) override;
   //@}

   //! @name Overwrite for no locking & unlocking of local tracks
   //@{
   //! these function are being overwritten so that the local track
   //! associated with the quantum task is not locked or unlocked
   //! @param aSimTime     :: simulation time
   //! @param aTask        :: task to be purged or correlated
   void PurgeReceivedTask(double aSimTime, WsfTask& aTask) override;
   void PurgeTransmittedTask(double aSimTime, WsfTask& aTask) override;
   void UpdateTaskCorrelation(double aSimTime, WsfTask& aTask) override;
   //@}

   //! @name Allocation Strategy Settings
   //@{
   //! WsfAssetPerception objects (that represent platforms) can be split into
   //! many smaller objects each representing their own platform subsystem
   virtual void SetAssetRepresentation(AssetRepresentationType aType) { mAssetRep = aType; }
   //! See comments for ReallocationStrategyType enum
   virtual void SetReallocationStrategy(ReallocationStrategyType aType) { mReallocateStrategy = aType; }
   //! Sets whether or not ally tracks are passed to the generator (and subsequently tasks on those tracks will be used)
   virtual void SetIgnoreAllyTracks(bool aVal) { mIgnoreAllyTracks = aVal; }
   //@}

   WsfQuantumGenerator* Generator() const { return mGeneratorPtr.get(); }
   WsfQuantumEvaluator* Evaluator() const { return mEvaluatorPtr.get(); }

   //! @name Task assignment queries
   //@{
   //! @return list of tasks received or assigned that match specified parameters
   std::list<WsfTask*> TaskListReceivedOfType(WsfStringId aTaskType);
   std::list<WsfTask*> TaskListReceivedForTrack(const WsfTrackId& aTrackId);
   std::list<WsfTask*> TaskListReceivedOfTypeForTrack(WsfStringId aTaskType, const WsfTrackId& aTrackId);
   std::list<WsfTask*> TaskListReceivedForResourceType(int aResourceType);
   std::list<WsfTask*> TaskListReceivedForResourceName(WsfStringId aResourceName);

   std::list<WsfTask*> TaskListAssignedOfType(WsfStringId aTaskType);
   std::list<WsfTask*> TaskListAssignedForTrack(const WsfTrackId& aTrackId);
   std::list<WsfTask*> TaskListAssignedForResourceType(int aResourceType);
   std::list<WsfTask*> TaskListAssignedForResourceName(WsfStringId aResourceName);
   //@}

   //! @name Task assignee queries
   //@{
   //! @return set of platform indices of task assignees (tasks assigned & sent to them)
   std::set<size_t> AssigneesForTask(WsfQuantumTask* aTaskPtr);

   using WsfTaskManager::AssigneesForTask; // don't hide this

   //! @return set of asset perceptions of task assignees (tasks assigned & sent to them)
   std::set<WsfAssetPerception*> AssetAssigneesForTask(WsfQuantumTask* aTaskPtr);
   //@}

   //! @name Task status response methods
   //@{
   //! will send a WsfTaskStatusMessage to task assigner with proper fields populated
   //! @param aSimTime     :: simulation time
   //! @param aTask        :: task to report on, complete, or reject
   //! @param aSubStatus   :: extra data to be passed with report
   //! @return 'true' if task exists to report on & message sent
   bool ReportTaskProgress(double aSimTime, WsfTask& aTask, WsfStringId aSubStatus);
   bool ReportTaskComplete(double aSimTime, WsfTask& aTask, WsfStringId aSubStatus);
   bool RejectTask(double aSimTime, WsfTask& aTask);
   //! callback for extra processing when a task is marked complete
   void TaskCompleted(double aSimTime, WsfTask& aTask) override;
   //@}

   //! uses the given evaluator to fill out the asset-task pairing values matrix
   //! @param aSimTime     :: simulation time
   //! @param aEvaluator   :: evaluator to use for providing asset-task scoring
   //! @param aPerception  :: list of perception objects to use in evaluation
   void UpdateEvaluations(double aSimTime, WsfQuantumEvaluator& aEvaluator, std::vector<WsfLocalTrack*>& aPerception) override;

   // Disambiguate
   using WsfPlatformPart::GetSimulation;

protected:
   using WsfTaskManager::ReportTaskComplete;
   using WsfTaskManager::ReportTaskProgress;

   bool AcquireResource(double aSimTime, WsfTask& aTask, const WsfTrack& aTrack) override;
   bool ReleaseResource(double aSimTime, WsfTask& aTask) override;

   //! @return 'true' if the aAsset has rejected the aTask
   bool Rejected(WsfAssetPerception* aAssetPtr, WsfQuantumTask* aTaskPtr);
   //! @return 'true' if the aAsset has rejected any tasks
   bool RejectedAnyTasks(WsfAssetPerception* aAssetPtr);
   //! @return 'true' if the aTask has been rejected by any assets
   bool RejectedByAnyAsset(WsfQuantumTask* aTaskPtr);
   //! @return 'true' if the aTask was marked complete after aSimTime
   bool CompletedAfter(WsfQuantumTask* aTaskPtr, double aSimTime);

   void SetMatrixAssets(double aSimTime);
   void SetMatrixTasks(double aSimTime, std::map<int, std::set<size_t>>& aAllocationMap);
   void SetAllocations(AllocationList& aList);
   void AllocateExtraTasks(AllocationList& aList);
   void AllocateExtraAssets(AllocationList& aList);

private:
   TaskList::iterator FindTask(TaskList& aTaskList, WsfTask& aTask);
   using WsfTaskManager::FindTask;

   AssetRepresentationType  mAssetRep;
   ReallocationStrategyType mReallocateStrategy;
   bool                     mIgnoreAllyTracks;
   int                      mAssetsId;
   int                      mTasksId;
   int                      mPrevRejectCount;

   WsfPerceptionProcessor*               mPerceptionProcessorPtr;
   ut::CloneablePtr<WsfQuantumGenerator> mGeneratorPtr;
   ut::CloneablePtr<WsfQuantumEvaluator> mEvaluatorPtr;
   ut::CloneablePtr<WsfQuantumAllocator> mAllocatorExtraTasksPtr;
   ut::CloneablePtr<WsfQuantumAllocator> mAllocatorExtraAssetsPtr;

   std::vector<ut::CloneablePtr<WsfQuantumAllocator>> mAllocators;

   std::map<size_t, std::map<int, int>> mRejectedTaskMap; // map from asset id to vector of rejected task ids
   std::map<int, int>                   mRejectedTasks;   // map from task id to 1

   std::map<int, double> mCompletedTaskTimes; // map from task id to time reported complete (ignore if task creation
                                              // impetus is more up to date???)

   AllocationMap mAllocationMap;

   bool mFilterAssignedTasks;
   bool mFilterRejectedTasks;
   bool mFilterCompletedTasks;
   bool mUpdateAssignments;
};

#endif
