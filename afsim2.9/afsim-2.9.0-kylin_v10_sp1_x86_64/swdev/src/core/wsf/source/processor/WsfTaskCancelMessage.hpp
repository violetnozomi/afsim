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

#ifndef WSFTASKCANCELMESSAGE_HPP
#define WSFTASKCANCELMESSAGE_HPP

#include "wsf_export.h"

#include <memory>
#include <string>

class UtScriptClass;
class UtScriptTypes;
#include "WsfMessage.hpp"
class WsfPlatform;
#include "WsfStringId.hpp"
class WsfTask;
#include "WsfTrackId.hpp"

//! A message that is sent to cancel a task assignment.
//! @see WsfTaskManager
//! @see WsfTaskAssignMessage
class WSF_EXPORT WsfTaskCancelMessage : public WsfMessage
{
public:
   WsfTaskCancelMessage(); // For XIO (de)serialization.
   WsfTaskCancelMessage(WsfPlatform* aPlatformPtr);
   WsfTaskCancelMessage(WsfPlatform* aPlatformPtr, const WsfTask& aTask);

   WsfMessage* Clone() const override;

   const char* GetScriptClassName() const override;

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   static WsfStringId GetTypeId();

   //! Return the assigners task ID.
   unsigned int GetTaskId() const { return mTaskId; }

   //! @name Assigner methods.
   //@{
   WsfPlatform* GetAssigner() const;

   //! Get the platform index of the assigner.
   size_t GetAssignerPlatformIndex() const { return mAssignerPlatformIndex; }

   //! Set the platform index of the assigner
   void SetAssignerPlatformIndex(size_t aIndex) { mAssignerPlatformIndex = aIndex; }

   //! Get the name of the assigner.
   WsfStringId GetAssignerPlatformName() const { return mAssignerPlatformName; }

   //! Get the name of the processor on the assigner.
   WsfStringId GetAssignerProcessorName() const { return mAssignerProcessorName; }
   //@}

   //! @name Assignee methods.
   //@{
   WsfPlatform* GetAssignee() const;

   //! Get the platform index of the assignee.
   size_t GetAssigneePlatformIndex() const { return mAssigneePlatformIndex; }

   //! Set the platform index of the assignee
   void SetAssigneePlatformIndex(size_t aIndex) { mAssigneePlatformIndex = aIndex; }

   //! Get the name of the assignee.
   WsfStringId GetAssigneePlatformName() const { return mAssigneePlatformName; }

   //! Get the name of the processor on the assignee.
   WsfStringId GetAssigneeProcessorName() const { return mAssigneeProcessorName; }
   //@}

   //! @name Track ID methods.
   //@{
   //! Get the track ID to which the message applies.
   const WsfTrackId& GetTrackId() const { return mTrackId; }

   //! Set the track ID to which the message applies.
   void SetTrackId(const WsfTrackId& aTrackId) { mTrackId = aTrackId; }
   //@}

   //! @name Task type methods.
   //@{
   //! Get the task type.
   WsfStringId GetTaskType() const { return mTaskType; }

   //! Set the task type (ID form)
   void SetTaskType(WsfStringId aTaskType) { mTaskType = aTaskType; }

   //! Set the task type (string form)
   void SetTaskType(const std::string& aTaskType) { mTaskType = aTaskType; }
   //@}

   //!
   //! @name Resource methods.
   //! A task may have an associated resource (sensor, processor, etc.)
   //@{
   //! Get the resource name.
   WsfStringId GetResourceName() const { return mResourceName; }

   //! Set the resource (ID form)
   void SetResourceName(WsfStringId aResourceName) { mResourceName = aResourceName; }

   //! Set the resource (string form)
   void SetResourceName(const std::string& aResourceName) { mResourceName = aResourceName; }
   //@}

   //! For XIO (de)serialization.
   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfMessage::Serialize(aBuff);
      aBuff& mTaskId& mAssignerPlatformIndex& mAssignerPlatformName& mAssignerProcessorName& mAssigneePlatformIndex&
         mAssigneePlatformName& mAssigneeProcessorName& mTrackId& mTaskType& mResourceName;
   }

protected:
   // Using the default copy constructor.
   // WsfTaskCancelMessage(const WsfTaskCancelMessage& aSrc);

   //! The assigners task ID.
   unsigned int mTaskId;

   //! The platform index of the assigner.
   size_t mAssignerPlatformIndex;

   //! The name of the assigner.
   WsfStringId mAssignerPlatformName;

   //! The name of the processor on the assigner.
   WsfStringId mAssignerProcessorName;

   //! The platform index of the assignee.
   size_t mAssigneePlatformIndex;

   //! The name of the assignee.
   WsfStringId mAssigneePlatformName;

   //! The name of the processor on the assignee.
   WsfStringId mAssigneeProcessorName;

   //! The track ID of the track associated with the task.
   WsfTrackId mTrackId;

   //! The type of the task.
   WsfStringId mTaskType;

   //! The resource assigned for the task.
   WsfStringId mResourceName;
};

#endif
