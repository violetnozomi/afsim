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

#ifndef WSFTASKASSIGNMESSAGE_HPP
#define WSFTASKASSIGNMESSAGE_HPP

#include "wsf_export.h"

#include <memory>
#include <string>

class UtScriptClass;
class UtScriptTypes;
#include "WsfMessage.hpp"
class WsfPlatform;
#include "WsfTask.hpp"
#include "WsfTrack.hpp"

//! A message that is sent to assign a task.
//!
//! This message is sent to initiate a task. The task is can be simple (start tracking a particular object) or
//! vague (execute the kill chain against a target), but the key point is that the task is related to a track
//! is to be done as soon as possible. As implemented it does not provide for a complex mission as it does not
//! provide the ability to specify timing constraints, routes, or multiple tasks.
//!
//! A task is uniquely identified by:
//!
//! - A track ID.
//! - A task type.
//! - A resource. The name of a particular component to be applied to the task.
//!
//! @see WsfTaskManager
//! @see WsfTaskCancelMessage
//! @see WsfTaskStatusMessage

class WSF_EXPORT WsfTaskAssignMessage : public WsfMessage
{
public:
   WsfTaskAssignMessage(); // For XIO (de)serialization.
   WsfTaskAssignMessage(WsfPlatform* aPlatformPtr);
   WsfTaskAssignMessage(const WsfTaskAssignMessage& aSrc);
   WsfTaskAssignMessage& operator=(const WsfTaskAssignMessage&) = delete;

   WsfMessage* Clone() const override;

   const char* GetScriptClassName() const override;

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   static WsfStringId GetTypeId();

   //! @name Task methods.
   //@{
   //! Get the task that was sent (const form).
   const WsfTask& GetTask() const { return mTask; }

   //! Get the task that was sent (non-const form).
   WsfTask& GetTask() { return mTask; }

   void SetTask(const WsfTask& aTask);
   //@}

   //! @name Track methods.
   //@{
   //! Get the track that was sent (const form).
   const WsfTrack& GetTrack() const { return mTrack; }

   //! Get the track that was sent (non-const form).
   WsfTrack& GetTrack() { return mTrack; }

   void SetTrack(const WsfTrack& aTrack);
   //@}

   //! For XIO (de)serialization.
   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfMessage::Serialize(aBuff);
      aBuff&(WsfTaskData&)mTask& mTrack;
   }

protected:
   WsfTask  mTask;
   WsfTrack mTrack;
};

#endif
