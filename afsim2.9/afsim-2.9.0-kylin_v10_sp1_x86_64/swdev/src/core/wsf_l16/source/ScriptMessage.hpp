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
#ifndef L16_SCRIPTMESSAGE_HPP
#define L16_SCRIPTMESSAGE_HPP

#include <list>

#include "UtScriptBasicTypes.hpp"
#include "UtScriptClass.hpp"

namespace WsfL16
{
namespace Messages
{
class Base;
class ContinuationBase;
class ExtensionBase;
class InitialBase;
class MessageAccessorType;

class MessageScriptClass : public UtScriptClass
{
public:
   MessageScriptClass(const std::string& aClassName, UtScriptTypes* aTypesPtr, MessageAccessorType* aAccessor);
   ~MessageScriptClass() override;

   void Destroy(void* aObjectPtr) override;

   virtual void CreateAccessors(UtScriptTypes* aTypesPtr);

protected:
   int                  mAddingFieldIndex;
   MessageAccessorType* mAccessor;
};

class MessageInitialScriptClass : public MessageScriptClass
{
public:
   MessageInitialScriptClass(const std::string&     aClassName,
                             UtScriptTypes*         aTypesPtr,
                             Messages::InitialBase* aMessagePrototype = nullptr);
   ~MessageInitialScriptClass() override;
   void* Create(const UtScriptContext& aContext) override;
   void* Clone(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(Print);
   UT_DECLARE_SCRIPT_METHOD(SourceTrackNumber);

private:
   Messages::InitialBase*         mMessagePrototype;
   std::list<MessageScriptClass*> mSubWordScriptClasses;
};

class MessageContinuationScriptClass : public MessageScriptClass
{
public:
   MessageContinuationScriptClass(const std::string&                aClassName,
                                  UtScriptTypes*                    aTypesPtr,
                                  std::unique_ptr<ContinuationBase> aMessagePrototype = nullptr);
   ~MessageContinuationScriptClass() override = default;
   void* Create(const UtScriptContext& aContext) override;
   void* Clone(void* aObjectPtr) override;

private:
   std::unique_ptr<ContinuationBase> mMessagePrototype;
};

class MessageExtensionScriptClass : public MessageScriptClass
{
public:
   MessageExtensionScriptClass(const std::string&             aClassName,
                               UtScriptTypes*                 aTypesPtr,
                               std::unique_ptr<ExtensionBase> aMessagePrototype = nullptr);
   ~MessageExtensionScriptClass() override = default;
   void* Create(const UtScriptContext& aContext) override;
   void* Clone(void* aObjectPtr) override;

private:
   std::unique_ptr<ExtensionBase> mMessagePrototype;
};

class FieldSetterInterface : public UtScriptClass::InterfaceMethod
{
public:
   FieldSetterInterface(UtScriptClass* aParentPtr, const std::string& aName, MessageAccessorType* aAccessor, int aFieldIndex);
   void operator()(UtScriptExecutor*  aExecutorPtr,
                   UtScriptContext&   aContext,
                   const UtScriptRef& aReference,
                   UtScriptMethodArgs aVarArgs,
                   UtScriptData&      aReturnVal) override;

private:
   MessageAccessorType* mAccessor;
   int                  mFieldIndex;
};

class FieldGetterInterface : public UtScriptClass::InterfaceMethod
{
public:
   FieldGetterInterface(UtScriptClass* aParentPtr, const std::string& aName, MessageAccessorType* aAccessor, int aFieldIndex);

   void operator()(UtScriptExecutor*  aExecutorPtr,
                   UtScriptContext&   aContext,
                   const UtScriptRef& aReference,
                   UtScriptMethodArgs aVarArgs,
                   UtScriptData&      aReturnVal) override;

private:
   MessageAccessorType* mAccessor;
   int                  mFieldIndex;
};

} // namespace Messages
} // namespace WsfL16
#endif
