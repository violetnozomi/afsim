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

#ifndef WSFTASKCONTROLMESSAGE_HPP
#define WSFTASKCONTROLMESSAGE_HPP

#include "wsf_export.h"

#include <memory>
#include <string>

class UtScriptClass;
class UtScriptTypes;
#include "WsfMessage.hpp"
class WsfPlatform;
class WsfProcessor;
#include "WsfStringId.hpp"

//! A 'control message' is used to change the operating level of a subordinate.
//!
//! This message is sent by a commander to a subordinate to change its 'operating
//! level' with respect to a particlar 'operating condition or status'. An
//! 'operating condition or status' can represent anything such as an alert status
//! and the 'operating level' represents a particular level of alert.
//!
//! @see WsfTaskManager
class WSF_EXPORT WsfTaskControlMessage : public WsfMessage
{
public:
   WsfTaskControlMessage(); // For XIO (de)serialization.
   WsfTaskControlMessage(WsfProcessor* aXmtrPtr, WsfPlatform* aRcvrPtr);

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   WsfMessage* Clone() const override;

   const char* GetScriptClassName() const override;

   static WsfStringId GetTypeId();

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

   //! @name Name methods.
   //@{
   //! Get the name of the operating condition or status whose level is being changed.
   WsfStringId GetName() const { return mName; }

   //! Set the name of the operating condition or status whose level is being changed (ID form)
   void SetName(WsfStringId aName) { mName = aName; }

   //! Set the name of the operating condition or status whose level is being changed (string form)
   void SetName(const std::string& aName) { mName = aName; }
   //@}

   //! @name Level methods.
   //@{
   //! Get the operating level.
   int GetLevel() const { return mLevel; }

   void SetLevel(int aLevel) { mLevel = aLevel; }
   //@}

   //! For XIO (de)serialization.
   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfMessage::Serialize(aBuff);
      aBuff& mAssignerPlatformIndex& mAssignerPlatformName& mAssignerProcessorName& mAssigneePlatformIndex&
         mAssigneePlatformName& mAssigneeProcessorName& mName& mLevel;
   }

protected:
   // Using the default copy constructor.
   // WsfTaskControlMessage(const WsfTaskControlMessage& aSrc);

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

   //! The name of the condition or status whose operating level is being changed.
   WsfStringId mName;

   //! The level to which the condition or status is being changed.
   int mLevel;
};

#endif
