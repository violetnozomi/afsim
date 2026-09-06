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

#ifndef WSFSCRIPTMESSAGEHANDLER_HPP
#define WSFSCRIPTMESSAGEHANDLER_HPP

#include "wsf_export.h"

#include <vector>

class UtInput;
#include "UtScriptData.hpp"
class UtScript;

class WsfMessage;
class WsfPlatform;
class WsfScriptContext;
#include "WsfStringId.hpp"

//! A script-based message handler.
//!
//! This object provides the ability to define scripts to process messages. It provides
//! the 'on_message' command and also maintains the 'MESSAGE' script variable that refers
//! to the current message being processed. Any variables defined by WsfScriptContext
//! ('PLATFORM', 'PROCESSOR', 'TIME_NOW') are also available.
//!
//! The user of this class is responsible for the following:
//!
//! - Invoking the ProcessInput() method to detect and process the 'on_message' command.
//! - Invoking the Initialize() method during the platform part Initialize() method.
//! - Invoking the ProcessMessage() method during the platform part ProcessMessage() method.

class WSF_EXPORT WsfScriptMessageHandler
{
public:
   WsfScriptMessageHandler() = default;
   WsfScriptMessageHandler(WsfScriptContext& aContext);
   WsfScriptMessageHandler(const WsfScriptMessageHandler& aSrc, WsfScriptContext& aContext);
   virtual ~WsfScriptMessageHandler() = default;

   //! Return true if there is a message handler defined.
   //! @note This is valid only after Initialize() has been called.
   bool HasMessageHandler() const { return mHasMessageHandler; }

   virtual bool Initialize(double aSimTime, WsfPlatform* aPlatformPtr);

   virtual bool ProcessInput(UtInput& aInput);

   virtual bool ProcessMessage(double aSimTime, const WsfMessage& aMessage);

   void SetTimeAndMessage(double aSimTime, const WsfMessage& aMessage);

   //! A 'Handler' represents a 'type' and one or more 'subtypes' that share a common script.
   struct Handler
   {
   public:
      Handler()
         : mType(nullptr)
         , mSubTypes()
         , mScriptIndex(0)
      {
      }

      WsfStringId                       mType;
      std::vector<WsfStringId>          mSubTypes;
      std::vector<UtScript*>::size_type mScriptIndex;
   };

protected:
   WsfPlatform* mPlatformPtr{nullptr};

   //! The context used for compiling and executing scripts.
   WsfScriptContext* mContextPtr{nullptr};

   //! The list of message handlers.
   std::vector<Handler> mHandlers;

   //! The list of script names, indexed by the handlers script index.
   std::vector<WsfStringId> mScriptNames;

   //! The list of scripts, indexed by the handles script index.
   //! This is valid only after Initialize() has been called.
   //! The scripts are owned by the script context.
   std::vector<UtScript*> mScriptPtrs;

   //! The pointer to the explicit 'script void on_message()' if it was defined.
   UtScript* mOnMessageScriptPtr{nullptr};

   //! true if 'on_message' or 'script void on_message()' has been defined.
   bool mHasMessageHandler{false};

   //! The message script variable's memory index.
   UtScriptData* mMessageVarPtr{nullptr};
   int           mMessageVarIndex{-1};
};

#endif
