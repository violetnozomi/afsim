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
#ifndef L16_MESSAGESFACTORY_HPP
#define L16_MESSAGESFACTORY_HPP

#include <map>
#include <set>
#include <string>
#include <vector>

class UtCallback;
class UtScriptTypes;
class WsfScenario;

namespace WsfL16
{
namespace Messages
{
class Base;
class ContinuationBase;
class ExtensionBase;
class InitialBase;
class MessageBitInput;
class MessageScriptClass;

//! The JMessageFactory is a static class for creating JMessages
class Factory
{
public:
   Factory();
   ~Factory();

   static void Initialize(UtScriptTypes* aScriptTypes);

   static void ResetState();

   //! Read a JMessage from a short *
   static std::vector<InitialBase*> ReadMessage(const unsigned char* aData);

   //! Read a JMessage from a MessageBitInput
   static std::vector<InitialBase*> ReadMessage(MessageBitInput& aInput, int aNumJ_Words = -1);

   //! Create a new JMessageI with the specified label and sub-label
   static InitialBase* CreateInitialWord(int aLabel, int aSublabel);

   //! Create a new Initial message with all implemented sub-words
   static InitialBase* CreateFullMessage(int aLabel, int aSublabel);

   //! Create a new continuation word, and put it in the initial word aParent
   static ContinuationBase* CreateContinuationWord(InitialBase* aParent, int continuationLabel);

   //! Create a new extension word, and put it in the initial word aParent
   static ExtensionBase* CreateExtensionWord(InitialBase* aParent, int extensionWordIndex);

   static MessageScriptClass* GetMessageScriptClass(int aLabel, int aSubLabel);

   struct MessageInfo
   {
      Messages::InitialBase* mMessagePtr;
      MessageScriptClass*    mScriptClass;
      std::string            mClassName;
   };

   typedef std::map<int, MessageInfo> MessageMap;
   static const MessageMap&           GetMessages() { return mMessages; }

protected:
   static void         InitializeAccessors();
   static void         LoadMessageAccessors(Messages::InitialBase* aMessage);
   static void         LoadWordAccessors(Messages::Base* aWord);
   static InitialBase* NewMessage(int aLabel, int aSubLabel);

   static void RegisterScriptClasses(UtScriptTypes& aTypes);


   static void AddMessage(Messages::InitialBase* aMessage);

   static MessageMap mMessages;

   static std::set<int> mUnhandledJMessages;
   // static UtCallback* mCallbackPtr;
};
} // namespace Messages
} // namespace WsfL16

#endif
