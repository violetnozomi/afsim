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

#include "script/WsfScriptMessageHandler.hpp"

#include <algorithm>
#include <cassert>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtScriptContext.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfMessage.hpp"
#include "WsfPlatform.hpp"
#include "WsfStringId.hpp"
#include "script/WsfScriptContext.hpp"

//! Constructor
//! @param aContext The script context to be used for compiling and executing scripts.
WsfScriptMessageHandler::WsfScriptMessageHandler(WsfScriptContext& aContext)
   : mPlatformPtr(nullptr)
   , mContextPtr(&aContext)
   , mHandlers()
   , mScriptNames()
   , mScriptPtrs()
   , mOnMessageScriptPtr(nullptr)
   , mHasMessageHandler(false)
   , mMessageVarPtr(nullptr)
{
   // Declare the script variable MESSAGE that will point to the message during ProcessMessage().
   mMessageVarPtr = aContext.GetContext().RegisterVariable("MESSAGE", UtStringIdLiteral("WsfMessage"));
}

//! Copy constructor
//! @param aSrc     The source object.
//! @param aContext The script context to be used for compiling and executing scripts.
WsfScriptMessageHandler::WsfScriptMessageHandler(const WsfScriptMessageHandler& aSrc, WsfScriptContext& aContext)
   : mPlatformPtr(nullptr)
   , mContextPtr(&aContext)
   , mHandlers(aSrc.mHandlers)
   , mScriptNames(aSrc.mScriptNames)
   , mScriptPtrs()
   , mOnMessageScriptPtr(nullptr)
   , mHasMessageHandler(false)
   , mMessageVarPtr(nullptr)
{
   mMessageVarPtr = aContext.GetContext().RegisterVariable("MESSAGE", UtStringIdLiteral("WsfMessage"));
}

//! Initialize the message handler object.
//! @param aSimTime     The current simulation time.
//! @param aPlatformPtr The platform with which the message handler is associated.
// virtual
bool WsfScriptMessageHandler::Initialize(double aSimTime, WsfPlatform* aPlatformPtr)
{
   bool ok = true;
   assert(aPlatformPtr != nullptr);
   mPlatformPtr        = aPlatformPtr;
   mOnMessageScriptPtr = mContextPtr->FindScript(UtStringIdLiteral("on_message"));
   mHasMessageHandler  = ((!mHandlers.empty()) || (mOnMessageScriptPtr != nullptr));
   // Resolve the script names into script pointers for more rapid access at execution time.
   mScriptPtrs.resize(mScriptNames.size());
   for (std::vector<int>::size_type i = 0; i < mScriptNames.size(); ++i)
   {
      mScriptPtrs[i] = mContextPtr->FindScript(mScriptNames[i]);
      if (mScriptPtrs[i] == nullptr)
      {
         auto out = ut::log::error() << "WsfScriptMessageHandler unable to resolve script name.";
         out.AddNote() << "Script: " << mScriptNames[i];
         ok = false;
      }
   }
   return ok;
}

//! Check for and process message handler commands.
//! @param aInput The input stream.
//! @return 'true' if the command was recognized (and processed) or 'false' if the command was not
//! one recognized by this class.
// virtual
bool WsfScriptMessageHandler::ProcessInput(UtInput& aInput)
{
   bool        myCommand = false;
   std::string command(aInput.GetCommand());
   if (command == "on_message")
   {
      myCommand                 = true;
      bool         typeValid    = true;
      bool         subTypeValid = false;
      bool         defaultValid = true;
      bool         scriptValid  = true;
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (command == "type")
         {
            if (!typeValid)
            {
               throw UtInput::BadValue(aInput, "'subtype' is invalid at this time");
            }
            std::string typeName;
            aInput.ReadValue(typeName);
            Handler handler;
            handler.mType        = typeName;
            handler.mScriptIndex = mScriptNames.size();
            mHandlers.push_back(handler);
            subTypeValid = true;
            scriptValid  = true;
         }
         else if ((command == "subtype") || (command == "sub_type"))
         {
            if (!subTypeValid)
            {
               throw UtInput::BadValue(aInput, "'subtype' is invalid at this time");
            }
            std::string subTypeName;
            aInput.ReadValue(subTypeName);

            // If 'scriptValid' is true it means that the script for the current running set of type/subtypes
            // was compiled. We are now starting a new set of subtypes to which the next script will apply.
            // Therefore, we have to create a new handler.

            if (!scriptValid)
            {
               Handler handler;
               handler.mType        = mHandlers.back().mType;
               handler.mScriptIndex = mScriptNames.size();
               mHandlers.push_back(handler);
            }
            mHandlers.back().mSubTypes.emplace_back(subTypeName);
            scriptValid = true;
         }
         else if (command == "default")
         {
            if (!defaultValid)
            {
               throw UtInput::BadValue(aInput, "'default' is invalid at this time");
            }
            Handler handler;
            handler.mScriptIndex = mScriptNames.size();
            mHandlers.push_back(handler);
            // Once a 'default' command is seen then only a 'script' is valid.
            typeValid    = false;
            subTypeValid = false;
            defaultValid = false;
            scriptValid  = true;
         }
         else if (command == "script")
         {
            if (!scriptValid)
            {
               throw UtInput::BadValue(aInput, "'script' is invalid at this time");
            }
            // Formulate a unique script name.
            std::ostringstream oss;
            oss << "__on_message_" << mScriptNames.size() + 1 << "__";
            std::string scriptName(oss.str());

            UtScript* scriptPtr = mContextPtr->Compile(scriptName, "void", aInput, "end_script", "");
            scriptValid         = false; // We can't have a script immediately follow a script

            // If there was no 'type', 'subtype' or 'default' command then implicitly assume a 'default'
            // command was specified.
            if (mHandlers.empty())
            {
               Handler handler;
               handler.mScriptIndex = mScriptNames.size();
               mHandlers.push_back(handler);
               typeValid    = false;
               subTypeValid = false;
               defaultValid = false;
            }
            mScriptNames.emplace_back(scriptPtr->GetName());
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      // Make sure a 'script' has been associated with the last group of type/subtypes.
      if ((!mHandlers.empty()) && ((mHandlers.back().mScriptIndex + 1) != mScriptNames.size()))
      {
         throw UtInput::BadValue(aInput, "A 'script' must be defined");
      }
   }
   return myCommand;
}

//! Invoke the appropriate message handling script if one is defined.
//! If a script has been defined for the message then:
//! - Set TIME_NOW to the current simulation time.
//! - Set MESSAGE to refer to the message.
//! - Invoke the message handling script.
//! @param aSimTime The current simulation time.
//! @param aMessage The message being processed.
//! @returns 'true' if a message handler was defined for the message or 'false' if not.
// virtual
bool WsfScriptMessageHandler::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   if (!mHasMessageHandler)
   {
      return false;
   }

   mPlatformPtr->Update(aSimTime); // Ensure the platform location is current
   auto savedMessageVarPtr = mMessageVarPtr->GetPointer()->GetAppObject();
   SetTimeAndMessage(aSimTime, aMessage); // Set TIME_NOW and MESSAGE

   bool        messageProcessed = false;
   WsfStringId msgType          = aMessage.GetType();
   WsfStringId msgSubType       = aMessage.GetSubType();
   for (Handler& handler : mHandlers)
   {
      bool typeMatches = (msgType == handler.mType);
      if (!typeMatches)
      {
         // When WSF was first developed, the message type for
         // WsfTrackDropMessage was confusingly called WSF_DROP_TRACK_MESSAGE
         // instead of WSF_TRACK_DROP_MESSAGE. If the user defines
         // processing for one of these types then it will also be added for the other type. This will
         // make any transition transparent.

         if ((handler.mType == UtStringIdLiteral("WSF_TRACK_DROP_MESSAGE")) ||
             (handler.mType == UtStringIdLiteral("WSF_DROP_TRACK_MESSAGE")))
         {
            typeMatches = ((msgType == UtStringIdLiteral("WSF_TRACK_DROP_MESSAGE")) ||
                           (msgType == UtStringIdLiteral("WSF_DROP_TRACK_MESSAGE")));
         }
      }

      if (typeMatches)
      {
         // A handler with no subtypes indicates all subtypes are to be processed.
         // A handler with subtypes indicates only the indicated subtypes are to be processed.
         const std::vector<WsfStringId>& subTypes = handler.mSubTypes;
         if (subTypes.empty() || (std::find(subTypes.begin(), subTypes.end(), msgSubType) != subTypes.end()))
         {
            messageProcessed = true;
         }
      }
      else if (handler.mType == nullptr)
      {
         // 'default' handler processes all types of messages.
         messageProcessed = true;
      }
      if (messageProcessed)
      {
         UtScriptData     scriptRetVal;
         UtScriptDataList scriptArgs;
         mContextPtr->ExecuteScript(mScriptPtrs[handler.mScriptIndex], scriptRetVal, scriptArgs);
         break;
      }
   }

   if ((!messageProcessed) && (mOnMessageScriptPtr != nullptr))
   {
      UtScriptData     scriptRetVal;
      UtScriptDataList scriptArgs;
      mContextPtr->ExecuteScript(mOnMessageScriptPtr, scriptRetVal, scriptArgs);
   }
   mMessageVarPtr->GetPointer()->SetAppObject(savedMessageVarPtr);
   return messageProcessed;
}

//! Set the TIME_NOW and MESSAGE script variables.
//! @param aSimTime The current simulation time.
//! @param aMessage The message that is being processed.
void WsfScriptMessageHandler::SetTimeAndMessage(double aSimTime, const WsfMessage& aMessage)
{
   // Set the MESSAGE variable to point the message.
   mMessageVarPtr->GetPointer()->SetAppObject(const_cast<WsfMessage*>(&aMessage), aMessage.GetReferenceCount());
}
