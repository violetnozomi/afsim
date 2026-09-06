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

#include "WsfTaskData.hpp"

#include "WsfAttributeContainer.hpp"

WsfTaskData::WsfTaskData()
   : mTaskId(0)
   , mTrackId()
   , mLocalTrackId()
   , mTaskType(nullptr)
   , mTargetName(nullptr)
   , mAssignerPlatformName(nullptr)
   , mAssignerPlatformIndex(0)
   , mAssignerProcessorName(nullptr)
   , mAssigneePlatformName(nullptr)
   , mAssigneePlatformIndex(0)
   , mAssigneeProcessorName(nullptr)
   , mResourcePtr(new WsfTaskResource(cTASK_RESOURCE_TYPE_SENSOR))
   , mCommName(nullptr)
   , mAssignTime(0.0)
   , mUpdateTime(0.0)
   , mObjectsPending(0)
   , mTaskStatus(nullptr)
   , mTaskSubStatus(nullptr)
{
}

WsfTaskData::WsfTaskData(const WsfTaskData& aSrc)
   : WsfAuxDataEnabled(aSrc)
   , mTaskId(aSrc.mTaskId)
   , mTrackId(aSrc.mTrackId)
   , mLocalTrackId(aSrc.mLocalTrackId)
   , mTaskType(aSrc.mTaskType)
   , mTargetName(aSrc.mTargetName)
   , mAssignerPlatformName(aSrc.mAssignerPlatformName)
   , mAssignerPlatformIndex(aSrc.mAssignerPlatformIndex)
   , mAssignerProcessorName(aSrc.mAssignerProcessorName)
   , mAssigneePlatformName(aSrc.mAssigneePlatformName)
   , mAssigneePlatformIndex(aSrc.mAssigneePlatformIndex)
   , mAssigneeProcessorName(aSrc.mAssigneeProcessorName)
   , mResourcePtr(aSrc.mResourcePtr->Clone())
   , mCommName(aSrc.mCommName)
   , mAssignTime(aSrc.mAssignTime)
   , mUpdateTime(aSrc.mUpdateTime)
   , mObjectsPending(aSrc.mObjectsPending)
   , mTaskStatus(aSrc.mTaskStatus)
   , mTaskSubStatus(aSrc.mTaskSubStatus)
{
}

WsfTaskData::WsfTaskData(const WsfTaskResource& aTaskResource)
   : mTaskId(0)
   , mAssignerPlatformIndex(0)
   , mAssigneePlatformIndex(0)
   , mResourcePtr(aTaskResource.Clone())
   , mAssignTime(0.0)
   , mUpdateTime(0.0)
   , mObjectsPending(0)
   , mTaskStatus(nullptr)
   , mTaskSubStatus(nullptr)
{
}

WsfTaskData& WsfTaskData::operator=(const WsfTaskData& aRhs)
{
   // There probably aren't any cases of self-assignment of tasks, but just in case...
   if (this != &aRhs)
   {
      WsfAuxDataEnabled::operator=(aRhs);
      mTaskId                    = aRhs.mTaskId;
      mTrackId                   = aRhs.mTrackId;
      mLocalTrackId              = aRhs.mLocalTrackId;
      mTaskType                  = aRhs.mTaskType;
      mTargetName                = aRhs.mTargetName;
      mAssignerPlatformName      = aRhs.mAssignerPlatformName;
      mAssignerPlatformIndex     = aRhs.mAssignerPlatformIndex;
      mAssignerProcessorName     = aRhs.mAssignerProcessorName;
      mAssigneePlatformName      = aRhs.mAssigneePlatformName;
      mAssigneePlatformIndex     = aRhs.mAssigneePlatformIndex;
      mAssigneeProcessorName     = aRhs.mAssigneeProcessorName;
      delete mResourcePtr;
      mResourcePtr = nullptr;
      if (aRhs.mResourcePtr != nullptr)
      {
         mResourcePtr = aRhs.mResourcePtr->Clone();
      }
      mCommName       = aRhs.mCommName;
      mAssignTime     = aRhs.mAssignTime;
      mUpdateTime     = aRhs.mUpdateTime;
      mObjectsPending = aRhs.mObjectsPending;
      mTaskStatus     = aRhs.mTaskStatus;
      mTaskSubStatus  = aRhs.mTaskSubStatus;
   }
   return *this;
}
// virtual
WsfTaskData::~WsfTaskData()
{
   UtReferenceTracked::FreeReference();
   delete mResourcePtr;
}
