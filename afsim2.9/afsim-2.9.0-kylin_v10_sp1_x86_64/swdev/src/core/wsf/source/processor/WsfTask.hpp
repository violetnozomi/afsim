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

#ifndef WSFTASK_HPP
#define WSFTASK_HPP

#include "wsf_export.h"

#include "UtScriptAccessible.hpp"
#include "UtScriptBasicTypes.hpp"
#include "UtScriptClass.hpp"
#include "UtScriptTypes.hpp"
class WsfPlatform;
class WsfProcessor;
#include "WsfTaskData.hpp"
#include "script/WsfScriptAuxDataUtil.hpp"

//! A class that defines a task to be performed.
class WSF_EXPORT WsfTask : public UtScriptAccessible, public WsfTaskData
{
public:
   WsfTask() {}
   WsfTask(const WsfTask& aSrc)
      : WsfTaskData(aSrc)
   {
   }
   WsfTask(const WsfTaskResource& aTaskResource)
      : WsfTaskData(aTaskResource)
   {
   }
   WsfTask& operator=(const WsfTask& aRhs)
   {
      WsfTaskData::operator=(aRhs);
      return *this;
   }

   ~WsfTask() override = default;

   bool operator==(const WsfTask& aRhs) const
   {
      return (mTaskId == aRhs.mTaskId) && (mAssignerProcessorName == aRhs.mAssignerProcessorName);
   }

   bool operator!=(const WsfTask& aRhs) const { return !operator==(aRhs); }

   const char* GetScriptClassName() const override { return "WsfTask"; }
   static void RegisterScriptTypes(UtScriptTypes* aScriptTypesPtr);

   // static UtScriptClass* CreateScriptClass(const std::string& aClassName,
   //                                         UtScriptTypes*     aScriptTypesPtr);

   void Initialize(double aSimTime, unsigned long aTaskId, WsfProcessor* aAssignerPtr);

   //! Return the task ID assigned to the task.
   unsigned int GetTaskId() const { return mTaskId; }

   const WsfTrackId& GetTrackId() const { return mTrackId; }
   void              SetTrackId(const WsfTrackId& aTrackId) { mTrackId = aTrackId; }

   const WsfTrackId& GetLocalTrackId() const { return mLocalTrackId; }
   void              SetLocalTrackId(const WsfTrackId& aLocalTrackId) { mLocalTrackId = aLocalTrackId; }

   WsfStringId GetTaskType() const { return mTaskType; }
   void        SetTaskType(WsfStringId aTaskType) { mTaskType = aTaskType; }

   //! Get the name of the target.
   //! The name is the 'truth name' from the track. This may be zero if not correlated with a real platform.
   WsfStringId GetTargetName() const { return mTargetName; }

   //! Set the name of the target.
   void SetTargetName(WsfStringId aTargetName) { mTargetName = aTargetName; }

   //! @name Assigner methods.
   //@{
   void SetAssigner(const WsfTask& aTask);

   WsfStringId GetAssignerPlatformName() const { return mAssignerPlatformName; }
   size_t      GetAssignerPlatformIndex() const { return mAssignerPlatformIndex; }
   WsfStringId GetAssignerProcessorName() const { return mAssignerProcessorName; }
   //@}

   //! @name Assignee methods.
   //@{
   void SetAssignee(WsfPlatform* aAssigneePtr);

   WsfStringId GetAssigneePlatformName() const { return mAssigneePlatformName; }
   size_t      GetAssigneePlatformIndex() const { return mAssigneePlatformIndex; }
   WsfStringId GetAssigneeProcessorName() const { return mAssigneeProcessorName; }
   void        SetAssigneeProcessorName(WsfStringId aAssigneeProcessorName)
   {
      mAssigneeProcessorName = aAssigneeProcessorName;
   }
   //@}


   //! @name Resource methods.
   //@{
   WsfTaskResource&       GetResource() { return *mResourcePtr; }
   const WsfTaskResource& GetResource() const { return *mResourcePtr; }
   void                   SetResource(const WsfTaskResource& aResource);

   WsfStringId GetResourceName() const { return GetResource().mNameId; }
   void        SetResourceName(WsfStringId aResourceName) { GetResource().mNameId = aResourceName; }

   int GetResourceType() const { return GetResource().GetType(); }

   bool IsResourceTypeOf(const int aResourceType) const { return GetResourceType() == aResourceType; };

   WsfStringId GetResourceMode() const { return GetResource().mModeNameId; }
   void        SetResourceMode(WsfStringId aResourceMode) { GetResource().mModeNameId = aResourceMode; }
   //@}

   //! @name Communications methods.
   //! For an inter-platform assignment, this is the local name over which task messages are sent.
   //! For an intra-platform assignment, this is the name of the assigning processor.
   //@{
   WsfStringId GetCommName() const { return mCommName; }
   void        SetCommName(WsfStringId aCommName) { mCommName = aCommName; }
   //@}

   //! @name Update time methods.
   //! Set or get the time when a status report was last sent.
   //@{
   void   SetUpdateTime(double aUpdateTime) { mUpdateTime = aUpdateTime; }
   double GetUpdateTime() const { return mUpdateTime; }
   //@}

   //! @name Assignment time methods
   //! Set or get the time when the task was assigned.
   //@{
   void   SetAssignTime(double aAssignTime) { mAssignTime = aAssignTime; }
   double GetAssignTime() const { return mAssignTime; }
   //@}

   //! @name Status methods
   //! Set or get the task status string id.
   //@{
   void        SetStatus(WsfStringId aStatusId) { mTaskStatus = aStatusId; }
   WsfStringId GetStatus() const { return mTaskStatus; }
   //@}

   //! @name SubStatus methods
   //! Set or get the task status string id.
   //@{
   void        SetSubStatus(WsfStringId aSubStatusId) { mTaskSubStatus = aSubStatusId; }
   WsfStringId GetSubStatus() const { return mTaskSubStatus; }
   //@}


   //! @name Object methods.
   //! These methods are used to keep track of objects associated with this track.
   //@{

   //! Return the number of objects that have been assigned but not yet in operation.
   int GetObjectsPending() const { return mObjectsPending; }

   //! Set the number of objects that have been assigned but not yet in operation.
   void SetObjectsPending(int aObjectsPending) { mObjectsPending = aObjectsPending; }
   //@}

   void SetAssignerPlatformIndex(size_t aIndex) { mAssignerPlatformIndex = aIndex; }
   void SetAssigneePlatformIndex(size_t aIndex) { mAssigneePlatformIndex = aIndex; }
};

//! The script interface 'class' for WsfTask
class WSF_EXPORT WsfScriptTaskClass : public UtScriptClass
{
public:
   WsfScriptTaskClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   std::string ToString(void* /*aObjectPtr*/) const override;

   void Destroy(void* aTaskPtr) override { delete (static_cast<WsfTask*>(aTaskPtr)); }

   void* Clone(void* aTaskPtr) override { return new WsfTask(*static_cast<WsfTask*>(aTaskPtr)); }

   void OnNewScriptRef(UtScriptRef& aReference) override;

   UT_DECLARE_SCRIPT_METHOD(Create);

   UT_DECLARE_SCRIPT_METHOD(TaskId);
   UT_DECLARE_SCRIPT_METHOD(TrackId);
   UT_DECLARE_SCRIPT_METHOD(LocalTrackId);
   UT_DECLARE_SCRIPT_METHOD(TaskType);
   UT_DECLARE_SCRIPT_METHOD(Assigner);
   UT_DECLARE_SCRIPT_METHOD(AssignerName);
   UT_DECLARE_SCRIPT_METHOD(Assignee);
   UT_DECLARE_SCRIPT_METHOD(AssigneeName);
   UT_DECLARE_SCRIPT_METHOD(ResourceName);
   UT_DECLARE_SCRIPT_METHOD(ResourceIsSensor);
   UT_DECLARE_SCRIPT_METHOD(ResourceIsProcessor);
   UT_DECLARE_SCRIPT_METHOD(ResourceMode);
   UT_DECLARE_SCRIPT_METHOD(ResourceCount);
   UT_DECLARE_SCRIPT_METHOD(ResourceFrequency);
   UT_DECLARE_SCRIPT_METHOD(ResourceBandwidth);
   UT_DECLARE_SCRIPT_METHOD(Status);
   UT_DECLARE_SCRIPT_METHOD(SubStatus);
   UT_DECLARE_SCRIPT_METHOD(TimeAssigned);
   UT_DECLARE_SCRIPT_METHOD(TimeUpdated);
   UT_DECLARE_SCRIPT_METHOD(TimeSinceAssigned);
   UT_DECLARE_SCRIPT_METHOD(TimeSinceUpdated);

   UT_DECLARE_SCRIPT_METHOD(SetAssignee_1);
   UT_DECLARE_SCRIPT_METHOD(SetAssignee_2);
   UT_DECLARE_SCRIPT_METHOD(SetTaskType);
   UT_DECLARE_SCRIPT_METHOD(SetCommDevice);

   UT_DECLARE_AUX_DATA_SCRIPT_METHODS()
};

#endif
