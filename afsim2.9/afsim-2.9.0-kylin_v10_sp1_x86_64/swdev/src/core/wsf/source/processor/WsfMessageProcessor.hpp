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

#ifndef WSFMESSAGEPROCESSOR_HPP
#define WSFMESSAGEPROCESSOR_HPP

#include "wsf_export.h"

#include <string>

#include "WsfExternalLinks.hpp"
#include "WsfInternalLinks.hpp"
#include "WsfRandomVariable.hpp"
#include "WsfTimeDelayQueue.hpp"
#include "WsfVariable.hpp"
#include "script/WsfScriptProcessor.hpp"

//! A processor that implements time-delays and message routing.

class WSF_EXPORT WsfMessageProcessor : public WsfScriptProcessor
{
public:
   WsfMessageProcessor(WsfScenario& aScenario);
   WsfMessageProcessor& operator=(const WsfMessageProcessor&) = delete;
   ~WsfMessageProcessor() override;

   //! @name Framework Methods
   //@{
   WsfProcessor* Clone() const override;

   bool Initialize(double aSimTime) override;

   bool ProcessInput(UtInput& aInput) override;

   bool ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;

   bool ReceiveMessage(double aSimTime, const WsfMessage& aMessage) override;

   void SendMessage(double aSimTime, const WsfMessage& aMessage) override;

   void TurnOff(double aSimTime) override;

   void TurnOn(double aSimTime) override;

   void Update(double aSimTime) override;
   //@}

   bool ProcessAction(double aSimTime, const WsfMessage& aMessage);
   class Action
   {
   public:
      friend class WsfMessageProcessor;

      Action(bool aIsDefaultAction);

      bool Initialize(double aSimTime, WsfMessageProcessor* aProcessorPtr);

      bool ProcessInput(UtInput& aInput, WsfMessageProcessor* aProcessorPtr, const std::string& aNamespace, bool aAllowScript);

      bool              mIsDefaultAction;
      bool              mIgnoreMessage;
      WsfRandomVariable mDelayTime;
      UtScript*         mScriptPtr;
      WsfStringId       mScriptNameId;

      // NOTE: The external and internal links contained in this object are NOT used if the instance
      //       is the action for the 'default_process'. Instead, the code will use the external links
      //       defined in WsfScriptProcessor and the internal links from WsfPlatformPart, so as to
      //       allow external run-time modification to work as expected.

      wsf::ExternalLinks mExternalLinks;
      WsfInternalLinks   mInternalLinks;
   };

   friend class Action;

   class Selector
   {
   public:
      enum
      {
         cCOMMANDER   = 1,
         cPEER        = 2,
         cSUBORDINATE = 4,
         cSELF        = 8
      };

      Selector();

      bool Initialize(double aSimTime, WsfMessageProcessor* aProcessorPtr);

      bool ProcessInput(UtInput& aInput, WsfMessageProcessor* aProcessorPtr, const std::string& aNamespace, int aSelectorIndex);

      bool Matches(double aSimTime, const WsfMessage& aMessage, WsfMessageProcessor* aProcessorPtr) const;

      UtScript*   mScriptPtr;
      WsfStringId mScriptNameId;
      WsfStringId mMessageTypeId;
      WsfStringId mMessageSubTypeId;
      WsfStringId mSensorNameId;
      WsfStringId mSensorTypeId;
      WsfStringId mSensorModeId;
      WsfStringId mSystemNameId;
      int         mSenderMask;
   };

   friend class Selector;

   class Process
   {
   public:
      Process();

      bool Initialize(double aSimTime, WsfMessageProcessor* aProcessorPtr);

      bool ProcessInput(UtInput& aInput, WsfMessageProcessor* aProcessorPtr, const std::string& aNamespace);

      std::list<Selector> mSelectorList;
      Action              mAction;
   };

   friend class Process;

   //! A unit of work within a time delay queue.
   class DelayRequest : public WsfTimeDelayQueue::Request
   {
   public:
      DelayRequest(WsfMessageProcessor* aProcessorPtr, const WsfMessage& aMessage, Action* aActionPtr, double aDelayTime);

      ~DelayRequest() override;

      // Needed for priority queuing
      bool operator<(const DelayRequest& aRhs) const { return mPriority < aRhs.mPriority; }

      bool CompleteRequest(double aSimTime) override;

      WsfMessageProcessor* mProcessorPtr;
      WsfMessage*          mMessagePtr;
      Action*              mActionPtr;

      int mPriority;
   };

   friend class DelayRequest;

protected:
   WsfMessageProcessor(const WsfMessageProcessor& aSrc);

private:
   Action* FindAction(double aSimTime, const WsfMessage& aMessage);

   void ProcessDelayedMessage(double aSimTime, const WsfMessage& aMessage, Action* aActionPtr);

   void ScheduleCompletionEvent(double aSimTime, const WsfMessage& aMessage, Action* aActionPtr, double aDelayTime);

   //! The list of message selectors and associated actions
   std::list<Process> mProcessList;

   //! The action to be applied to messages that are not mentioned in a process block.
   //!
   //! @note: WsfPlatformPart has internal links and WsfScriptProcessor has external links.
   //!        This supersedes them!
   Action mDefaultProcess;

   //! @name routing to be used if the actions for a process do not include routing.
   //@{
   WsfInternalLinks   mDefaultInternalLinks;
   wsf::ExternalLinks mDefaultExternalLinks;
   //@}

   //! The time delay queue used to simulation the time it takes to process a message.
   WsfTimeDelayQueue mDelayQueue;

   //! The index of the MESSAGE script variable.
   UtScriptData* mMessageVarPtr;

   //! Pointer to the action to be used for the next call 'ProcessMessage'.
   //!
   //! This is and optimization used to eliminate the need for 'ProcessMessage' to redetermine the
   //! action needed to be used with the current message.
   Action* mCurrentActionPtr;
};

#endif
