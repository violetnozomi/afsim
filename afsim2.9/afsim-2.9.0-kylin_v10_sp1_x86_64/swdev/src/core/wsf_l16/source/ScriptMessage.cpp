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
#include "ScriptMessage.hpp"

#include <iostream>
#include <memory>
#include <sstream>

#include "MessageAccessor.hpp"
#include "Messages.hpp"
#include "UtMemory.hpp"

namespace WsfL16
{
namespace Messages
{

namespace
{
const char* GetAccessorDataType(FieldAccessorType::AccessorType aAccessor)
{
   switch (aAccessor)
   {
   case FieldAccessor::cBOOLEAN:
      return "bool";
   case FieldAccessor::cDOUBLE:
      return "double";
   case FieldAccessor::cENUMERATION:
      return "int";
   case FieldAccessor::cINTEGER:
      return "int";
   case FieldAccessor::cSTRING:
      return "string";
   }
   return "";
}
void SetAccessorByScript(FieldAccessor& aAccessor, const UtScriptData& aScriptData)
{
   FieldAccessor::DataType type = aAccessor.GetType();
   switch (type)
   {
   case FieldAccessor::cBOOLEAN:
      aAccessor.Set(aScriptData.GetBool());
      break;
   case FieldAccessor::cDOUBLE:
      aAccessor.Set(aScriptData.GetDouble());
      break;
   case FieldAccessor::cINTEGER:
   case FieldAccessor::cENUMERATION:
      aAccessor.Set(aScriptData.GetInt());
      break;
   case FieldAccessor::cSTRING:
      aAccessor.Set(aScriptData.GetString());
      break;
   }
}
void GetAccessorByScript(FieldAccessor& aAccessor, UtScriptData& aScriptData)
{
   FieldAccessor::DataType type = aAccessor.GetType();
   switch (type)
   {
   case FieldAccessor::cBOOLEAN:
      aScriptData.SetBool(aAccessor.GetBool());
      break;
   case FieldAccessor::cDOUBLE:
      aScriptData.SetDouble(aAccessor.GetDouble());
      break;
   case FieldAccessor::cINTEGER:
   case FieldAccessor::cENUMERATION:
      aScriptData.SetInt(aAccessor.GetInt());
      break;
   case FieldAccessor::cSTRING:
      aScriptData.SetString(aAccessor.GetString());
      break;
   }
}

class NewInitialMessageInterface : public UtScriptClass::InterfaceMethod
{
public:
   NewInitialMessageInterface(UtScriptClass*         aParentPtr,
                              const std::string&     aName,
                              Messages::InitialBase* aMessage,
                              const std::string&     aMessageName)
      : UtScriptClass::InterfaceMethod(aName, aMessageName)
      , mMessage(aMessage)
   {
   }
   void operator()(UtScriptExecutor*  aExecutorPtr,
                   UtScriptContext&   aContext,
                   const UtScriptRef& aReference,
                   UtScriptMethodArgs aVarArgs,
                   UtScriptData&      aReturnVal) override
   {
      aReturnVal.SetPointer(new UtScriptRef(mMessage->Clone(), mParentPtr));
   }

private:
   Messages::InitialBase* mMessage;
};

class MessageWordFindInterface : public UtScriptClass::InterfaceMethod
{
public:
   MessageWordFindInterface(UtScriptClass*      aParentPtr,
                            const std::string&  aName,
                            MessageScriptClass* aSubWordScriptClass,
                            int                 aWordIndex,
                            bool                aIsContinuation,
                            const std::string&  aMessageName)
      : UtScriptClass::InterfaceMethod(aName, aMessageName)
      , mScriptClass(aSubWordScriptClass)
      , mWordIndex(aWordIndex)
      , mIsContinuation(aIsContinuation)
   {
   }

   void operator()(UtScriptExecutor*  aExecutorPtr,
                   UtScriptContext&   aContext,
                   const UtScriptRef& aReference,
                   UtScriptMethodArgs aVarArgs,
                   UtScriptData&      aReturnVal) override
   {
      Messages::InitialBase* message = (Messages::InitialBase*)aReference.GetAppObject();
      if (!message)
      {
         return;
      }

      void* subMsg = nullptr;
      if (mIsContinuation)
      {
         subMsg = message->FindContinuation(mWordIndex);
      }
      else
      {
         if ((int)message->GetExtensionCount() > mWordIndex)
         {
            subMsg = message->GetExtension(mWordIndex);
         }
      }
      aReturnVal.SetPointer(new UtScriptRef(subMsg, mScriptClass));
   }

private:
   MessageScriptClass* mScriptClass;
   int                 mWordIndex;
   bool                mIsContinuation;
};

class MessageWordAddInterface : public UtScriptClass::InterfaceMethod
{
public:
   MessageWordAddInterface(UtScriptClass*      aParentPtr,
                           const std::string&  aName,
                           MessageScriptClass* aSubWordScriptClass,
                           int                 aWordIndex,
                           bool                aIsContinuation,
                           const std::string&  aMessageName)
      : UtScriptClass::InterfaceMethod(aName, aMessageName)
      , mScriptClass(aSubWordScriptClass)
      , mWordIndex(aWordIndex)
      , mIsContinuation(aIsContinuation)
   {
   }

   void operator()(UtScriptExecutor*  aExecutorPtr,
                   UtScriptContext&   aContext,
                   const UtScriptRef& aReference,
                   UtScriptMethodArgs aVarArgs,
                   UtScriptData&      aReturnVal) override
   {
      Messages::InitialBase* message = (Messages::InitialBase*)aReference.GetAppObject();
      void*                  subMsg  = nullptr;
      if (mIsContinuation)
      {
         subMsg = message->AddContinuationWord(mWordIndex);
      }
      else
      {
         subMsg = message->AddExtensionWord(mWordIndex);
      }
      aReturnVal.SetPointer(new UtScriptRef(subMsg, mScriptClass));
   }

private:
   MessageScriptClass* mScriptClass;
   int                 mWordIndex;
   bool                mIsContinuation;
};

} // end anonymous namespace


FieldSetterInterface::FieldSetterInterface(UtScriptClass*       aParentPtr,
                                           const std::string&   aName,
                                           MessageAccessorType* aAccessor,
                                           int                  aFieldIndex)
   : UtScriptClass::InterfaceMethod(aName,
                                    "void",
                                    GetAccessorDataType(aAccessor->mFields[aFieldIndex].mAccessorType->mAccessor),
                                    1)
   , mAccessor(aAccessor)
   , mFieldIndex(aFieldIndex)
{
}

// virtual
void FieldSetterInterface::operator()(UtScriptExecutor*  aExecutorPtr,
                                      UtScriptContext&   aContext,
                                      const UtScriptRef& aReference,
                                      UtScriptMethodArgs aVarArgs,
                                      UtScriptData&      aReturnVal)
{
   if (!CheckForCallErrors(aExecutorPtr, aReference, &aVarArgs, aReturnVal))
   {
      return;
   }
   FieldAccessor fieldAccessor = mAccessor->GetField((Messages::Base*)aReference.GetAppObject(), mFieldIndex);
   SetAccessorByScript(fieldAccessor, aVarArgs[0]);
}


FieldGetterInterface::FieldGetterInterface(UtScriptClass*       aParentPtr,
                                           const std::string&   aName,
                                           MessageAccessorType* aAccessor,
                                           int                  aFieldIndex)
   : UtScriptClass::InterfaceMethod(aName, GetAccessorDataType(aAccessor->mFields[aFieldIndex].mAccessorType->mAccessor), "")
   , mAccessor(aAccessor)
   , mFieldIndex(aFieldIndex)
{
}

// virtual
void FieldGetterInterface::operator()(UtScriptExecutor*  aExecutorPtr,
                                      UtScriptContext&   aContext,
                                      const UtScriptRef& aReference,
                                      UtScriptMethodArgs aVarArgs,
                                      UtScriptData&      aReturnVal)
{
   if (!CheckForCallErrors(aExecutorPtr, aReference, &aVarArgs, aReturnVal))
   {
      return;
   }
   FieldAccessor fieldAccessor = mAccessor->GetField((Messages::Base*)aReference.GetAppObject(), mFieldIndex);
   GetAccessorByScript(fieldAccessor, aReturnVal);
}

MessageScriptClass::MessageScriptClass(const std::string& aClassName, UtScriptTypes* aTypesPtr, MessageAccessorType* aAccessor)
   : UtScriptClass(aClassName, aTypesPtr)
   , mAccessor(aAccessor)
{
}

void MessageScriptClass::CreateAccessors(UtScriptTypes* aTypesPtr)
{
   if (mAccessor)
   {
      for (unsigned i = 0; i < mAccessor->mFields.size(); ++i)
      {
         AddMethod(ut::make_unique<FieldGetterInterface>(this, mAccessor->mFields[i].mName, mAccessor, i));
         AddMethod(ut::make_unique<FieldSetterInterface>(this, mAccessor->mFields[i].mName, mAccessor, i));
      }
   }
}

void MessageScriptClass::Destroy(void* aObjectPtr)
{
   delete ((Messages::Base*)aObjectPtr);
}

MessageScriptClass::~MessageScriptClass() {}

MessageInitialScriptClass::MessageInitialScriptClass(const std::string&     aClassName,
                                                     UtScriptTypes*         aTypesPtr,
                                                     Messages::InitialBase* aMessagePrototype /* = 0*/)
   : MessageScriptClass(aClassName, aTypesPtr, MessageAccessorType::Find(aMessagePrototype))
   , mMessagePrototype(aMessagePrototype)
{
   if (mMessagePrototype)
   {
      std::string className = mMessagePrototype->GetClassName();
      mConstructible        = true;
      AddClassName("WsfTadilJMessageI");
      AddClassName(className);
   }
   else
   {
      mConstructible = false;
   }

   AddMethod(ut::make_unique<Print>());
   AddMethod(ut::make_unique<SourceTrackNumber>());

   CreateAccessors(aTypesPtr);

   if (mMessagePrototype)
   {
      AddMethod(ut::make_unique<NewInitialMessageInterface>(this, "Create", mMessagePrototype, GetClassName()));
   }

   for (int i = 0; i < 21 && mMessagePrototype; ++i)
   {
      {
         auto word = mMessagePrototype->CreateContinuationWord(i);
         if (word)
         {
            std::string wordName = word->GetClassName();
            auto scriptClass = ut::make_unique<MessageContinuationScriptClass>(wordName, aTypesPtr, std::move(word));
            mSubWordScriptClasses.push_back(scriptClass.get());
            {
               std::ostringstream ss;
               ss << "AddContinuation" << i;
               AddMethod(ut::make_unique<MessageWordAddInterface>(this, ss.str(), scriptClass.get(), i, true, wordName));
            }
            {
               std::ostringstream ss;
               ss << "FindContinuation" << i;
               AddMethod(ut::make_unique<MessageWordFindInterface>(this, ss.str(), scriptClass.get(), i, true, wordName));
            }
            mSubWordScriptClasses.push_back(scriptClass.get());
            aTypesPtr->Register(std::move(scriptClass));
         }
      }
      {
         auto word = mMessagePrototype->CreateExtensionWord(i);
         if (word)
         {
            std::string wordName = word->GetClassName();
            auto scriptClass     = ut::make_unique<MessageExtensionScriptClass>(wordName, aTypesPtr, std::move(word));
            {
               std::ostringstream ss;
               ss << "AddExtension" << i;
               AddMethod(ut::make_unique<MessageWordAddInterface>(this, ss.str(), scriptClass.get(), i, false, wordName));
            }
            {
               std::ostringstream ss;
               ss << "FindExtension" << i;
               AddMethod(ut::make_unique<MessageWordFindInterface>(this, ss.str(), scriptClass.get(), i, false, wordName));
            }
            mSubWordScriptClasses.push_back(scriptClass.get());
            aTypesPtr->Register(std::move(scriptClass));
         }
      }
   }
}

// virtual
MessageInitialScriptClass::~MessageInitialScriptClass()
{
   delete mMessagePrototype;
}

void* MessageInitialScriptClass::Create(const UtScriptContext& aContext)
{
   return mMessagePrototype->Clone();
}

void* MessageInitialScriptClass::Clone(void* aObjectPtr)
{
   return ((Messages::InitialBase*)aObjectPtr)->Clone();
}

UT_DEFINE_SCRIPT_METHOD(MessageInitialScriptClass, Messages::InitialBase, Print, 0, "void", "")
{
   auto logInfo = ut::log::info() << "Script Message: ";
   aObjectPtr->PrintMessage(logInfo);
}

UT_DEFINE_SCRIPT_METHOD(MessageInitialScriptClass, Messages::InitialBase, SourceTrackNumber, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetHeader().mSourceTrackNumber);
}

MessageContinuationScriptClass::MessageContinuationScriptClass(const std::string&                aClassName,
                                                               UtScriptTypes*                    aTypesPtr,
                                                               std::unique_ptr<ContinuationBase> aMessagePrototype /* = 0*/)
   : MessageScriptClass(aClassName, aTypesPtr, MessageAccessorType::Find(aMessagePrototype.get()))
   , mMessagePrototype(std::move(aMessagePrototype))
{
   AddClassName("WsfTadilJMessageC");
   mConstructible = false;

   if (mMessagePrototype)
   {
      AddClassName(mMessagePrototype->GetClassName());
   }
   else
   {
      mConstructible = false;
   }

   CreateAccessors(aTypesPtr);
}

void* MessageContinuationScriptClass::Create(const UtScriptContext& aContext)
{
   return mMessagePrototype->Clone();
}

void* MessageContinuationScriptClass::Clone(void* aObjectPtr)
{
   return ((Messages::InitialBase*)aObjectPtr)->Clone();
}

MessageExtensionScriptClass::MessageExtensionScriptClass(const std::string&             aClassName,
                                                         UtScriptTypes*                 aTypesPtr,
                                                         std::unique_ptr<ExtensionBase> aMessagePrototype /* = 0*/)
   : MessageScriptClass(aClassName, aTypesPtr, MessageAccessorType::Find(aMessagePrototype.get()))
   , mMessagePrototype(std::move(aMessagePrototype))
{
   AddClassName("WsfTadilJMessageE");

   if (mMessagePrototype)
   {
      AddClassName(mMessagePrototype->GetClassName());
   }
   else
   {
      mConstructible = false;
   }

   CreateAccessors(aTypesPtr);
}

void* MessageExtensionScriptClass::Create(const UtScriptContext& aContext)
{
   return mMessagePrototype->Clone();
}

void* MessageExtensionScriptClass::Clone(void* aObjectPtr)
{
   return ((Messages::InitialBase*)aObjectPtr)->Clone();
}

} // namespace Messages
} // namespace WsfL16
