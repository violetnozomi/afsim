// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "effects/WsfCyberMitmMessage.hpp"

#include <memory>

#include "UtMemory.hpp"
#include "WsfMessage.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptDefs.hpp"

namespace wsf
{
namespace cyber
{

// ============================================================================
MitmMessage::MitmMessage(std::unique_ptr<comm::Message> aMessage, double aDelay, bool aDrop)
   : mMessage(std::move(aMessage))
   , mDelay(aDelay)
   , mDrop(aDrop)
{
}

// ============================================================================
MitmMessage* MitmMessage::Clone() const
{
   return new MitmMessage(mMessage ? ut::clone(mMessage) : nullptr, mDelay, mDrop);
}

// ============================================================================
const char* MitmMessage::GetScriptClassName() const
{
   return "WsfCyberMitmMessage";
}

// ============================================================================
comm::Message* MitmMessage::GetMessage() const
{
   return mMessage.get();
}

// ============================================================================
double MitmMessage::GetDelay() const
{
   return mDelay;
}

// ============================================================================
bool MitmMessage::GetDrop() const
{
   return mDrop;
}

// ============================================================================
void MitmMessage::SetMessage(std::unique_ptr<comm::Message> aMessage)
{
   mMessage = std::move(aMessage);
}

// ============================================================================
void MitmMessage::SetDelay(double aDelay)
{
   mDelay = aDelay;
}

// ============================================================================
void MitmMessage::SetDrop(bool aDrop)
{
   mDrop = aDrop;
}

// ============================================================================
ScriptMitmMessage::ScriptMitmMessage(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfMitmMessage");

   // Add each of the method objects to the class.
   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<Message>());
   AddMethod(ut::make_unique<Delay>());
   AddMethod(ut::make_unique<Drop>());
   AddMethod(ut::make_unique<SetMessage>());
   AddMethod(ut::make_unique<SetDelay>());
   AddMethod(ut::make_unique<SetDrop>());
}

// ============================================================================
ScriptMitmMessage::~ScriptMitmMessage() {}

// ============================================================================
void* ScriptMitmMessage::Create(const UtScriptContext&)
{
   return new MitmMessage(nullptr, 0, false);
}

// ============================================================================
void* ScriptMitmMessage::Clone(void* aObjectPtr)
{
   return static_cast<MitmMessage*>(aObjectPtr)->Clone();
}

// ============================================================================
void ScriptMitmMessage::Destroy(void* aObjectPtr)
{
   delete static_cast<MitmMessage*>(aObjectPtr);
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMitmMessage, MitmMessage, Construct, 3, "WsfCyberMitmMessage", "WsfMessage, double, bool")
{
   auto  msgCopy    = ut::make_unique<comm::Message>(ut::clone(aVarArgs[0].GetPointer()->GetAppObject<WsfMessage>()));
   auto* messagePtr = new MitmMessage(std::move(msgCopy), aVarArgs[1].GetDouble(), aVarArgs[2].GetBool());
   aReturnVal.SetPointer(new UtScriptRef(messagePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMitmMessage, MitmMessage, Message, 0, "WsfMessage", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetMessage(), aReturnClassPtr));
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMitmMessage, MitmMessage, Delay, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetDelay());
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMitmMessage, MitmMessage, Drop, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetDrop());
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMitmMessage, MitmMessage, SetMessage, 1, "void", "WsfMessage")
{
   auto msgCopy = ut::make_unique<comm::Message>(ut::clone(aVarArgs[0].GetPointer()->GetAppObject<WsfMessage>()));
   aObjectPtr->SetMessage(std::move(msgCopy));
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMitmMessage, MitmMessage, SetDelay, 1, "void", "double")
{
   aObjectPtr->SetDelay(aVarArgs[0].GetDouble());
}

// ============================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMitmMessage, MitmMessage, SetDrop, 1, "void", "bool")
{
   aObjectPtr->SetDrop(aVarArgs[0].GetBool());
}

} // namespace cyber
} // namespace wsf
