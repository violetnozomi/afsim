// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "MessagesFactory.hpp"

#include <iostream>
#include <memory>

#include "AllMessages.hpp"
#include "MessagePacking.hpp"
#include "ScriptMessage.hpp"
#include "UtMemory.hpp"
#include "UtScriptTypes.hpp"
#include "WsfScenario.hpp"
#include "script/WsfScriptManager.hpp"

namespace WsfL16
{
namespace Messages
{
std::map<int, Factory::MessageInfo> Factory::mMessages;
std::set<int>                       Factory::mUnhandledJMessages;

void Factory::Initialize(UtScriptTypes* aScriptTypes)
{
   AddMessage(new J2_0::Initial());
   AddMessage(new J2_2::Initial());
   AddMessage(new J2_3::Initial());
   AddMessage(new J2_4::Initial());
   AddMessage(new J2_5::Initial());
   AddMessage(new J2_6::Initial());
   AddMessage(new J3_0::Initial());
   AddMessage(new J3_1::Initial());
   AddMessage(new J3_2::Initial());
   AddMessage(new J3_3::Initial());
   AddMessage(new J3_4::Initial());
   AddMessage(new J3_5::Initial());
   AddMessage(new J3_6::Initial());
   AddMessage(new J3_7::Initial());
   AddMessage(new J7_0::Initial());
   AddMessage(new J7_1::Initial());
   AddMessage(new J7_3::Initial());
   AddMessage(new J9_0::Initial());
   AddMessage(new J9_1::Initial());
   AddMessage(new J10_2::Initial());
   AddMessage(new J11_0::Initial());
   AddMessage(new J11_1::Initial());
   AddMessage(new J11_2::Initial());
   AddMessage(new J12_0::Initial());
   AddMessage(new J12_1::Initial());
   AddMessage(new J12_2::Initial());
   AddMessage(new J12_3::Initial());
   AddMessage(new J12_4::Initial());
   AddMessage(new J12_5::Initial());
   AddMessage(new J12_6::Initial());
   AddMessage(new J12_7::Initial());
   AddMessage(new J13_2::Initial());
   AddMessage(new J13_3::Initial());
   AddMessage(new J13_4::Initial());
   AddMessage(new J13_5::Initial());
   AddMessage(new J14_0::Initial());
   AddMessage(new J15_0::Initial());
   AddMessage(new J31_7::Initial());

   InitializeAccessors();
   RegisterScriptClasses(*aScriptTypes);
}

// Clear all global/static data so the class can be reinitialized
void Factory::ResetState()
{
   mMessages.clear();
   mUnhandledJMessages.clear();
}

void Factory::InitializeAccessors()
{
   const Factory::MessageMap&          messages = GetMessages();
   Factory::MessageMap::const_iterator i        = messages.begin();
   for (; i != messages.end(); ++i)
   {
      int          id     = i->first;
      int          lbl    = id / 100;
      int          sublbl = id % 100;
      InitialBase* msg    = CreateFullMessage(lbl, sublbl);
      LoadMessageAccessors(msg);
      delete msg;
   }
}

void Factory::LoadMessageAccessors(Messages::InitialBase* aMessage)
{
   LoadWordAccessors(aMessage);
   size_t eWords = aMessage->GetExtensionCount();
   for (size_t e = 0; e < eWords; ++e)
   {
      Messages::Base* msg = aMessage->GetExtension(static_cast<int>(e));
      LoadWordAccessors(msg);
   }
   size_t cWords = aMessage->GetContinuationCount();
   for (size_t i = 0; i < cWords; ++i)
   {
      Messages::ContinuationBase* msg = aMessage->GetContinuation(static_cast<int>(i));
      LoadWordAccessors(msg);
   }
}

void Factory::LoadWordAccessors(Messages::Base* aWord)
{
   MessageAccessorFiller filler;
   filler.ProcessMessage(aWord);
}


void Factory::RegisterScriptClasses(UtScriptTypes& aTypes)
{
   aTypes.Register(ut::make_unique<MessageInitialScriptClass>("WsfTadilJMessageI", &aTypes));
   aTypes.Register(ut::make_unique<MessageExtensionScriptClass>("WsfTadilJMessageE", &aTypes));
   aTypes.Register(ut::make_unique<MessageContinuationScriptClass>("WsfTadilJMessageC", &aTypes));
   for (std::map<int, MessageInfo>::iterator i = mMessages.begin(); i != mMessages.end(); ++i)
   {
      MessageInfo& info = i->second;
      info.mClassName   = info.mMessagePtr->GetClassName();
      auto scriptClass  = ut::make_unique<MessageInitialScriptClass>(info.mClassName, &aTypes, info.mMessagePtr);
      info.mScriptClass = scriptClass.get();
      aTypes.Register(std::move(scriptClass));
   }
}

std::vector<InitialBase*> Factory::ReadMessage(const unsigned char* aData)
{
   MessageBitInput msgReader((const char*)aData);
   return ReadMessage(msgReader);
}

// Reads a message from a buffer.  Constructs any
// sub-words that may be contained.  If there is an error
// ReadMessage() returns 0, otherwise it returns a pointer
// to the new message
std::vector<InitialBase*> Factory::ReadMessage(MessageBitInput& aInput, int aNumJ_Words)
{
   // Create the return value container
   std::vector<InitialBase*> msgs;

   // Read the header; assume there is only one
   Messages::Header header;
   header.Read(aInput);

   // Read all J messages in the PDU; reads one by default
   int numWordsToRead = std::max(aNumJ_Words, 0);

   do
   {
      // Read the word type
      unsigned lWordType = 0u;
      aInput.Read(lWordType, 2);

      // Expect an initial word first
      if (lWordType == Base::cINITIAL)
      {
         bool ok = true;

         unsigned lLabel, lSubLabel, lMessageLength, lExtensionWord = 0;
         aInput.Read(lLabel, 5);
         aInput.Read(lSubLabel, 3);
         aInput.Read(lMessageLength, 3);
         InitialBase* initialBasePtr = NewMessage(lLabel, lSubLabel);

         if (initialBasePtr != nullptr)
         {
            initialBasePtr->GetHeader() = header;
            initialBasePtr->TransferI(aInput);
            aInput.Skip(10); // last 10 bits in the 80 bit boundary

            for (unsigned i = 0; i < lMessageLength; ++i)
            {
               aInput.Read(lWordType, 2);
               if (lWordType == Base::cCONTINUATION)
               {
                  aInput.Read(lLabel, 5);
                  ContinuationBase* newWordPtr = initialBasePtr->AddContinuationWord(lLabel);
                  if (newWordPtr != nullptr)
                  {
                     newWordPtr->TransferI(aInput);
                     aInput.Skip(10); // last 10 bits in the 80 bit boundary
                  }
                  else
                  {
                     ok = false;
                  }
               }
               else if (lWordType == Base::cEXTENSION)
               {
                  ExtensionBase* newWordPtr = initialBasePtr->AddExtensionWord(lExtensionWord++);
                  if (newWordPtr != nullptr)
                  {
                     newWordPtr->TransferI(aInput);
                     aInput.Skip(10); // last 10 bits in the 80 bit boundary
                  }
                  else
                  {
                     ok = false;
                  }
               }
               else
               {
                  int                     jMsgKey = 100 * lLabel + lSubLabel;
                  std::set<int>::iterator iter    = mUnhandledJMessages.find(jMsgKey);
                  if (iter == mUnhandledJMessages.end())
                  {
                     // Allow the message to be printed only once and not over and over
                     auto out = ut::log::error() << "Cannot create sub-word.";
                     out.AddNote() << "Invalid word format #" << lWordType;
                     mUnhandledJMessages.insert(jMsgKey);
                  }
                  ok = false;
                  break;
               }
            }
         }
         else
         {
            int                     jMsgKey = 100 * lLabel + lSubLabel;
            std::set<int>::iterator iter    = mUnhandledJMessages.find(jMsgKey);
            if (iter == mUnhandledJMessages.end())
            {
               // Allow the message to be printed only once and not over and over
               auto logInfo = ut::log::info() << "Unsupported message type.";
               logInfo.AddNote() << "Message type: J" << lLabel << "." << lSubLabel;
               mUnhandledJMessages.insert(jMsgKey);
            }
            ok = false;
         }

         if (ok)
         {
            msgs.push_back(initialBasePtr);

            if (aNumJ_Words < 0)
            {
               // done
               numWordsToRead = 0;
            }
            else
            {
               // Decrement the number of words left to read; includes the initial
               int messageLength = (int)(initialBasePtr->GetMessageLength()) + 1;
               numWordsToRead    = std::max(0, (numWordsToRead - messageLength));
            }
         }
         else
         {
            delete initialBasePtr;
         }
      }
      else
      {
         int                     jMsgKey = (int)lWordType;
         std::set<int>::iterator iter    = mUnhandledJMessages.find(jMsgKey);
         if (iter == mUnhandledJMessages.end())
         {
            // Allow the message to be printed only once and not over and over
            // Found an unexpected word type; we are done
            auto logError = ut::log::error() << "Unexpected word type. Expected an initial word.";
            logError.AddNote() << "Word Type: " << lWordType;
            mUnhandledJMessages.insert(jMsgKey);
         }
         numWordsToRead = 0;
      }
   } while (numWordsToRead > 0);

   return msgs;
}

InitialBase* Factory::CreateInitialWord(int aLabel, int aSubLabel)
{
   return NewMessage(aLabel, aSubLabel);
}

// static
InitialBase* Factory::CreateFullMessage(int aLabel, int aSublabel)
{
   InitialBase* msg = NewMessage(aLabel, aSublabel);
   if (msg)
   {
      for (int i = 0; i < 21; ++i)
      {
         msg->AddExtensionWord(i);
      }
      for (int i = 0; i < 21; ++i)
      {
         msg->AddContinuationWord(i);
      }
   }
   return msg;
}

ContinuationBase* Factory::CreateContinuationWord(InitialBase* aParent, int aContinuationLabel)
{
   return aParent->AddContinuationWord(aContinuationLabel);
}

ExtensionBase* Factory::CreateExtensionWord(InitialBase* aParent, int aExtensionNumber)
{
   return aParent->AddExtensionWord(aExtensionNumber);
}

MessageScriptClass* Factory::GetMessageScriptClass(int aLabel, int aSubLabel)
{
   std::map<int, MessageInfo>::iterator i = mMessages.find(aLabel * 100 + aSubLabel);
   if (i != mMessages.end())
   {
      return i->second.mScriptClass;
   }
   return nullptr;
}

InitialBase* Factory::NewMessage(int aLabel, int aSubLabel)
{
   // combine the label and sub-label into a single integer:
   int                                  wordID = aLabel * 100 + aSubLabel;
   std::map<int, MessageInfo>::iterator i      = mMessages.find(wordID);
   if (i != mMessages.end())
   {
      return i->second.mMessagePtr->Clone();
   }
   return nullptr;
}

void Factory::AddMessage(Messages::InitialBase* aMessage)
{
   MessageInfo info;
   info.mMessagePtr                  = aMessage;
   info.mScriptClass                 = nullptr;
   int label                         = info.mMessagePtr->GetLabel();
   int sublabel                      = info.mMessagePtr->GetSubLabel();
   mMessages[label * 100 + sublabel] = info;
}

} // namespace Messages
} // namespace WsfL16
