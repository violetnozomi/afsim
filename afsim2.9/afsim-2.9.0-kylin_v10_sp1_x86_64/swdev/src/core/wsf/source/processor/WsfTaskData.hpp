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

#ifndef WSFTASKDATA_HPP
#define WSFTASKDATA_HPP

#include "wsf_export.h"

#include "UtReferenceTracked.hpp"
#include "WsfAuxDataEnabled.hpp"
#include "WsfTaskResource.hpp"
#include "WsfTrackId.hpp"

class WSF_EXPORT WsfTaskData : public UtReferenceTracked, public WsfAuxDataEnabled
{
public:
   WsfTaskData();
   WsfTaskData(const WsfTaskData& aSrc);
   WsfTaskData(const WsfTaskResource& aTaskResource);
   WsfTaskData& operator=(const WsfTaskData& aRhs);
   virtual ~WsfTaskData();

   // ================================= WARNING: New member variables ===========================
   //
   //   This class is transmitted over the network.  If you add a new member variable, it is
   //   YOUR RESPONSIBILITY to verify that the member is serialized correctly.
   //   Please see the implementation of WsfTaskData::SerializeP in xio/WsfXIO_SerializeTypes.cpp
   //
   // ===========================================================================================

   //! The serial number of the task.
   unsigned int mTaskId;

   //! The track ID of the track that is associated with the task.
   //! This is the ID of the track as submitted in the task assignment request.
   WsfTrackId mTrackId;

   //! The local track ID that is associated with the track ID in the assignment.
   //! For instance, when command CMDR assigns local track CMDR.x to subordinate SUB,
   //! this field will be the local track ID of the local track that was created which
   //! contains CMDR.x as a constituent track.
   WsfTrackId mLocalTrackId;

   //! The task type (string ID).
   WsfStringId mTaskType;

   //! The name of the target (string ID) IF it is known.
   WsfStringId mTargetName;

   //! The name of the assigning platform (string ID).
   WsfStringId mAssignerPlatformName;

   //! The platform index of the assigning platform.
   size_t mAssignerPlatformIndex;

   //! The name of the processor on the assigning platform (string ID).
   WsfStringId mAssignerProcessorName;

   //! The name of the assigned platform (string ID).
   WsfStringId mAssigneePlatformName;

   //! The platform index of the assigned platform.
   size_t mAssigneePlatformIndex;

   //! The name of the processor on the assigned platform (string ID).
   WsfStringId mAssigneeProcessorName;

   //! Attributes of the assigned resource.
   WsfTaskResource* mResourcePtr;

   //! The name of the comm device on which the task assignment was received (string ID).
   //! This is used by the assignee for sending status messages.
   WsfStringId mCommName;

   //! The simulation time when the assignment was made.
   double mAssignTime;

   //! The simulation time when an update was last sent or received.
   double mUpdateTime;

   //! The number of objects that are pending (requested but not yet in operation)
   int mObjectsPending;

   //! Task status
   WsfStringId mTaskStatus;

   //! Task sub status reported by assignee.  (User Defined)
   WsfStringId mTaskSubStatus;

   //! For XIO (de)serialization.
   template<typename T>
   void SerializeP(T& aBuff);
};
#endif
