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

#ifndef WSFSCRIPTPROCESSOR_HPP
#define WSFSCRIPTPROCESSOR_HPP

#include "UtCloneablePtr.hpp"
class UtInput;
class UtScript;
#include "UtScriptData.hpp"
class WsfAdvancedBehaviorTree;
class WsfBehaviorTree;
#include "wsf_export.h"

#include "WsfExternalLinks.hpp"
#include "WsfProcessor.hpp"
class WsfScriptContext;
class WsfScriptMessageHandler;
class WsfScriptStateMachine;

//! WsfScriptProcessor provides a means to execute scripts on simulation updates
//! and message processing. From within an input file the user can specify an "on_update"
//! script (i.e. script void on_update() ... end_script).  During runtime,
//! each call to Update will subsequently call the "on update" script.  The same holds
//! true for the processing of messages, a user can specify a custom script named
//! "on_message" and it will be called during runtime each time ProcessInput
//! is called.  In addition, the following variables will be available to the scripts:
//!
//!     PLATFORM  A reference to the platform that owns the script processor.
//!     PROCESSOR A reference to script processor.
//!     TIME_NOW  The current simulation time in seconds.
//!     MESSAGE   A reference to the message currently being processed (applicable to on_message).

class WSF_EXPORT WsfScriptProcessor : public WsfProcessor
{
public:
   WsfScriptProcessor(const WsfScenario& aScenario);

   WsfScriptProcessor(const WsfScenario& aScenario, WsfStringId aPartClass, const std::string& aPartVarName);

   ~WsfScriptProcessor() override;

   WsfProcessor* Clone() const override;

   bool Initialize(double aSimTime) override;

   bool Initialize2(double aSimTime) override;

   bool ProcessInput(UtInput& aInput) override;

   void Update(double aSimTime) override;

   bool ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;
#undef SendMessage
   void SendMessage(double aSimTime, const WsfMessage& aMessage) override;

   virtual void ExecuteOnMessageCreate(double aSimTime, WsfMessage& aMessage);

   virtual bool ExecuteScript(double aSimTime, const std::string& aScriptName);

   virtual bool ExecuteScript(double                  aSimTime,
                              WsfStringId             aScriptName,
                              UtScriptData&           aScriptRetVal,
                              const UtScriptDataList& aScriptArgs = UtScriptDataList());

   void ExecuteScriptAtTime(double aTime, WsfStringId aScriptName, const UtScriptDataList& aScriptArgs);

   virtual void SuppressMessage(bool aSuppressMessage);

   WsfScriptMessageHandler* GetMessageHandler() { return mMessageHandlerPtr; }

   WsfScriptContext& GetScriptContext() { return *mContextPtr; }
   UtScriptContext*  GetScriptAccessibleContext() const override;
   void              SetPlatform(WsfPlatform* aPlatformPtr) override;

   const WsfBehaviorTree* BehaviorTree() const { return mBehaviorTreePtr; }

   WsfAdvancedBehaviorTree* AdvancedBehaviorTree() const;

   virtual const WsfScriptStateMachine* GetStateMachine() const { return mStateMachinePtr.get(); }

   wsf::ExternalLinks& GetExternalLinks() { return mExternalLinks; }

   std::string State();
   std::string StateAll();
   void        SetState(std::string& stateName);
   void        SetStateAll(std::string& stateNameAll);

protected:
   WsfScriptProcessor(const WsfScriptProcessor& aSrc);

   //! Pointer to the script context.
   WsfScriptContext* mContextPtr;

   //! Pointer to the 'on_message' handler.
   WsfScriptMessageHandler* mMessageHandlerPtr;

   //! Pointer to the 'on_message_create' script.
   UtScript* mOnMessageCreateScriptPtr;

   UtScriptClass* mMessageClassPtr; // = mContextPtr->GetClass("WsfMessage");

   //! The external recipients.
   wsf::ExternalLinks mExternalLinks;

   WsfBehaviorTree* mBehaviorTreePtr;

   ut::CloneablePtr<WsfAdvancedBehaviorTree> mAdvancedBehaviorTreePtr;

   std::unique_ptr<WsfScriptStateMachine> mStateMachinePtr;
   int                                    mStateIndex;

   //! 'true' if the current message should be suppressed.
   //! This is set to 'false' before invoking the message handler for a received message. The user can invoke
   //! the script method 'SuppressMessage' to indicate that it will not be sent to the internal/external links.
   bool mSuppressMessage;
};

#endif
