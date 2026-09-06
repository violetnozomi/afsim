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

#ifndef WSFTASKSTATUSMESSAGE_HPP
#define WSFTASKSTATUSMESSAGE_HPP

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

//! A 'status message' provides status about a task assignment.
//! This message is sent by the recipient of a task assignment to indicate the progress towards performing
//! the task. Messages are generally sent to convey:
//!
//! - acceptance or rejection of the task assignment.
//! - completion of the task.
//! - intermediate progress on the task.
//!
//! Common values of the 'status' are defined by convention in WsfTaskManager, but other values may be
//! defined if a particular application has other requirements.
//!
//! @see WsfTaskManager
//! @see WsfTaskAssignMessage
class WSF_EXPORT WsfTaskStatusMessage : public WsfMessage
{
public:
   WsfTaskStatusMessage(); // For XIO (de)serialization.
   WsfTaskStatusMessage(WsfPlatform* aPlatformPtr);
   WsfTaskStatusMessage(WsfStringId aStatusId, WsfPlatform* aSenderPtr, const WsfTask& aTask);

   WsfMessage* Clone() const override;

   const char* GetScriptClassName() const override;

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   static WsfStringId GetTypeId();

   //! Return the assigners task ID.
   unsigned int GetTaskId() const { return mTaskId; }

   //! @name Message acknowledge Id.
   //! A status message may represent the acknowledgment of a received message.
   //! The value is the message serial number of the message that is being acknowledged.
   //@{
   //! The message serial number of the message that is being acknowledged by this message.
   //! If zero then the message is not acknowledging another message.
   unsigned int GetAcknowledgeId() const { return mAcknowledgeId; }

   //! Set the message serial number of the message that is being acknowledged.
   void SetAcknowledgeId(unsigned int aAcknowledgeId) { mAcknowledgeId = aAcknowledgeId; }
   //@}

   //! @name Assigner methods.
   //@{
   WsfPlatform* GetAssigner() const;

   //! Get the platform index of the assigner.
   size_t GetAssignerPlatformIndex() const { return mAssignerPlatformIndex; }

   //! Set the platform index of the assigner
   void SetAssignerPlatformIndex(size_t aPlatformIndex) { mAssignerPlatformIndex = aPlatformIndex; }

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
   void SetAssigneePlatformIndex(size_t aPlatformIndex) { mAssigneePlatformIndex = aPlatformIndex; }

   //! Get the name of the assignee.
   WsfStringId GetAssigneePlatformName() const { return mAssigneePlatformName; }

   //! Get the name of the processor on the assignee.
   WsfStringId GetAssigneeProcessorName() const { return mAssigneeProcessorName; }
   //@}

   //! @name Status and sub-status methods.
   //@{
   //! Get the status.
   WsfStringId GetStatus() const { return mStatus; }

   //! Set the status (ID form)
   void SetStatus(WsfStringId aStatus) { mStatus = aStatus; }

   //! Set the status (string form)
   void SetStatus(const std::string& aStatus) { mStatus = aStatus; }

   //! Get the sub-status.
   WsfStringId GetSubStatus() const { return mSubStatus; }

   //! Set the sub-status (ID form)
   void SetSubStatus(WsfStringId aSubStatus) { mSubStatus = aSubStatus; }

   //! Set the sub-status (string form)
   void SetSubStatus(const std::string& aSubStatus) { mSubStatus = aSubStatus; }
   //@}

   //! @name Track ID methods.
   //@{
   //! Get the task ID to which the message applies.
   const WsfTrackId& GetTrackId() const { return mTrackId; }

   //! Set the task ID to which the message applies.
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

   //! @name Object index methods.
   //! A task status may be referencing another object.  This is the object platform index.
   //@{
   //! Returns the object platform index
   size_t GetObjectPlatformIndex() const { return mObjectPlatformIndex; }

   //! Sets the object platform index
   void SetObjectPlatformIndex(size_t aIndex) { mObjectPlatformIndex = aIndex; }
   //@}

   //! @name Message source.
   //@{
   //! Returns true if the message was sent by the assignee.
   bool IsFromAssignee() const { return mIsFromAssignee; }

   //! Declare if the message is being sent by the assignee.
   //! This is only used to send the acknowledgment of a task complete message back to the assignee.
   void SetIsFromAssignee(bool aIsFromAssignee) { mIsFromAssignee = aIsFromAssignee; }
   //@}

   //! For XIO (de)serialization.
   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfMessage::Serialize(aBuff);
      aBuff& mAcknowledgeId& mTaskId& mAssignerPlatformIndex& mAssignerPlatformName& mAssignerProcessorName& mAssigneePlatformIndex&
         mAssigneePlatformName& mAssigneeProcessorName& mStatus& mSubStatus& mTrackId& mTaskType& mResourceName& mIsFromAssignee;
   }

protected:
   // Using the default copy constructor.
   // WsfTaskStatusMessage(const WsfTaskStatusMessage& aSrc);

   //! The message serial number of the message that is being acknowledged by this message.
   unsigned int mAcknowledgeId;

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

   //! The status that is being reported.
   WsfStringId mStatus;

   //! The sub-status that is being reported.
   WsfStringId mSubStatus;

   //! The track ID of the track associated with the task.
   WsfTrackId mTrackId;

   //! The type of the task.
   WsfStringId mTaskType;

   //! The resource assigned for the tasked.
   WsfStringId mResourceName;

   //! Index of the object platform
   size_t mObjectPlatformIndex;

   //! True if the message is from the assignee or false if from the assigner.
   //! By default a message is from the assignee. The only time this is from the
   //! assigner is if it is acknowledging the receipt of a task completion.
   bool mIsFromAssignee;
};

#endif
