// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCommMedium.hpp"

#include "UtCast.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "WsfCommEvent.hpp"
#include "WsfCommLayer.hpp"
#include "WsfCommLayerEvent.hpp"
#include "WsfCommMessage.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommObserver.hpp"
#include "WsfCommPhysicalLayer.hpp"
#include "script/WsfScriptDefs.hpp"

namespace wsf
{
namespace comm
{
namespace medium
{

// =================================================================================================
ScriptMediumModeClass::ScriptMediumModeClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfCommMediumMode");
   mConstructible      = false;
   mCloneable          = false;
   mIsScriptAccessible = true;

   AddMethod(ut::make_unique<GetName>());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumModeClass, Mode, GetName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetName());
}

// =================================================================================================
Medium::Medium(WsfScenario& aScenario)
   : mScenario(aScenario)
   , mContextPtr(ut::make_unique<WsfScriptContext>(*aScenario.GetScriptContext(), "WsfCommMedium"))
{
}

// =================================================================================================
Medium::Medium(const Medium& aSrc)
   : WsfObject(aSrc)
   , mModes(aSrc.mModes)
   , mShared(aSrc.mShared)
   , mDebug(aSrc.mDebug)
   , mEventPtr(std::make_shared<int>(0))
   , mChannels(aSrc.mChannels)
   , mModeIndex(aSrc.mModeIndex)
   , mDefaultModeName(aSrc.mDefaultModeName)
   , mScenario(aSrc.mScenario)
   , mSimPtr(aSrc.mSimPtr)
   , mUseSharing(aSrc.mUseSharing)
   , mContextPtr(ut::make_unique<WsfScriptContext>(WsfScriptContext(*(aSrc.mContextPtr))))
   , mModeOnTransmit(aSrc.mModeOnTransmit)
{
}

// =================================================================================================
Medium& Medium::operator=(Medium& aRhs)
{
   if (&aRhs != this)
   {
      WsfObject::operator=(aRhs);
      mModes             = aRhs.mModes;
      mShared            = aRhs.mShared;
      mDebug             = aRhs.mDebug;
      mEventPtr          = std::make_shared<int>(0);
      mChannels          = aRhs.mChannels;
      mModeIndex         = aRhs.mModeIndex;
      mDefaultModeName   = aRhs.mDefaultModeName;
      mUseSharing        = aRhs.mUseSharing;
      mContextPtr        = ut::make_unique<WsfScriptContext>(WsfScriptContext(*(aRhs.mContextPtr)));
   }

   return *this;
}

// =================================================================================================
bool Medium::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   if (command == "channels")
   {
      unsigned int numChannels = 0U;
      aInput.ReadValue(numChannels);
      SetNumChannels(numChannels);
   }
   else if (command == "default_mode_name")
   {
      aInput.ReadValue(mDefaultModeName);
   }
   else if (command == "mode")
   {
      std::string name;
      aInput.ReadValue(name);

      if (!GetMode(name))
      {
         auto mode = CreateDefaultMode();
         mode->SetName(name);
         mModes.emplace_back(std::move(mode));
      }

      auto         modePtr = GetMode(name);
      UtInputBlock modeBlock(aInput, "end_mode");
      while (modeBlock.ReadCommand())
      {
         if (!modePtr->ProcessInput(modeBlock.GetInput()))
         {
            throw UtInput::UnknownCommand(modeBlock.GetInput());
         }
      }
   }
   else if (GetMode("default") && GetMode("default")->ProcessInput(aInput))
   {
   }
   else if (command == "debug")
   {
      mDebug = true;
   }
   else if (command == "use_sharing")
   {
      aInput.ReadValue(mUseSharing);
   }
   else if (command == "script")
   {
      UtScript*   scriptPtr      = mContextPtr->Compile(aInput, "end_script");
      std::string scriptName     = scriptPtr->GetName();
      bool        validSignature = true;

      if (scriptName == "ModeOnTransmit")
      {
         validSignature  = mContextPtr->ValidateScript(scriptPtr, "string", "WsfCommMessage, WsfComm, WsfComm");
         mModeOnTransmit = scriptPtr;
      }
      else
      {
         throw UtException("Unrecognized script method in mode: " + GetType());
      }

      // Throw an exception if the script signature is incorrect
      if (!validSignature)
      {
         throw UtException("Invalid script return type, argument type, or argument count in mode: " + GetType());
      }
   }
   else if (WsfObject::ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =================================================================================================
bool Medium::Initialize(double aSimTime)
{
   bool ok = true;

   if (mModes.empty())
   {
      auto out = ut::log::error() << "No modes defined.";
      out.AddNote() << "Medium: " << GetName();
      return false;
   }

   if (!GetMode(mDefaultModeName))
   {
      auto out = ut::log::error() << "Default mode name does not exist.";
      out.AddNote() << "Mode name: " << mDefaultModeName;
      return false;
   }

   ok &= SetActiveMode(mDefaultModeName);
   ok &= (mSimPtr != nullptr);

   ok &= mContextPtr->Initialize(*GetSimulation(), this);

   return ok;
}

// =================================================================================================
std::pair<size_t, bool> Medium::GetModeIndex(const std::string& aModeName) const
{
   std::pair<size_t, bool> returnPair;
   returnPair.first  = 0U;
   returnPair.second = false;

   auto it = std::find_if(std::begin(mModes),
                          std::end(mModes),
                          [&aModeName](const ut::CloneablePtr<Mode>& aMode) { return aModeName == aMode->GetName(); });

   if (it != std::end(mModes))
   {
      returnPair.first  = ut::safe_cast<size_t>(it - std::begin(mModes));
      returnPair.second = true;
   }

   return returnPair;
}

// =================================================================================================
const std::string& Medium::GetActiveModeName() const
{
   return mModes[mModeIndex]->GetName();
}

// =================================================================================================
size_t Medium::GetCurrentTransmissions() const
{
   size_t transmissions = 0;
   for (const auto& msg : mMessages)
   {
      if (msg.second.IsTransmitting())
      {
         ++transmissions;
      }
   }
   return transmissions;
}

// =================================================================================================
bool Medium::SetActiveMode(const std::string& aModeName)
{
   auto it = std::find_if(std::begin(mModes),
                          std::end(mModes),
                          [&aModeName](const ut::CloneablePtr<Mode>& aMode) { return aModeName == aMode->GetName(); });

   if (it == std::end(mModes))
   {
      return false;
   }

   mModeIndex = ut::safe_cast<size_t, ptrdiff_t>(it - std::begin(mModes));
   return true;
}

// =================================================================================================
bool Medium::SetActiveMode(size_t aIndex)
{
   if (aIndex > (mModes.size() - 1U))
   {
      return false;
   }
   else
   {
      mModeIndex = aIndex;
      return true;
   }
}

// =================================================================================================
void Medium::SetActiveModeDelayed(const std::string& aModeName)
{
   auto simTime = GetSimulation()->GetSimTime();
   GetSimulation()->AddEvent(ut::make_unique<GenericEvent<int>>(simTime, mEventPtr, [=]() { SetActiveMode(aModeName); }));
}

// =================================================================================================
void Medium::SetActiveModeDelayed(size_t aIndex)
{
   auto simTime = GetSimulation()->GetSimTime();
   GetSimulation()->AddEvent(ut::make_unique<GenericEvent<int>>(simTime, mEventPtr, [=]() { SetActiveMode(aIndex); }));
}

// =================================================================================================
MessageStatus* Medium::GetMessageStatus(const Message::Identifier& aIdentifier)
{
   auto it = mMessages.find(aIdentifier);

   if (it != std::end(mMessages))
   {
      return &(it->second);
   }

   return nullptr;
}

// =================================================================================================
MessageStatus* Medium::GetMessageStatusByIndex(size_t aIndex)
{
   size_t index = 0;
   for (auto& msg : mMessages)
   {
      if (index == aIndex)
      {
         return &msg.second;
      }
      ++index;
   }

   return nullptr;
}

// =================================================================================================
Mode* Medium::GetMode(const std::string& aModeName) const
{
   const auto it =
      std::find_if(std::begin(mModes),
                   std::end(mModes),
                   [&aModeName](const ut::CloneablePtr<Mode>& aMode) { return aModeName == aMode->GetName(); });

   if (it != std::end(mModes))
   {
      return it->get();
   }

   return nullptr;
}

// =================================================================================================
Mode* Medium::GetMode(size_t aModeIndex) const
{
   if (aModeIndex < mModes.size())
   {
      return mModes[aModeIndex].get();
   }

   return nullptr;
}

// =================================================================================================
std::string Medium::ExecuteModeOnTransmit(double aSimTime, const Message& aMessage, const Comm& aXmtr, const Comm& aRcvr)
{
   if (mModeOnTransmit)
   {
      UtScriptData       scriptRetVal;
      UtScriptDataPacker scriptArgs(mContextPtr->GetTypes());

      scriptArgs << &aMessage << &aXmtr << &aRcvr;
      mContextPtr->ExecuteScript(aSimTime, mModeOnTransmit, scriptRetVal, scriptArgs);

      return scriptRetVal.GetString();
   }

   return std::string();
}

// =================================================================================================
void Medium::RemoveMessageStatus(const Message::Identifier& aIdentifier)
{
   auto it = mMessages.find(aIdentifier);

   if (it != std::end(mMessages))
   {
      mMessages.erase(it);
   }
}

// =================================================================================================
Mode* Medium::GetModeForTransmission(double aSimTime, Comm& aXmtr, Comm& aRcvr, Message& aMessage)
{
   Mode* modePtr      = nullptr;
   auto  userModeName = ExecuteModeOnTransmit(aSimTime, aMessage, aXmtr, aRcvr);

   if (!userModeName.empty())
   {
      modePtr = GetMode(userModeName);
   }
   else
   {
      // Use the current active mode.
      modePtr = GetMode(GetActiveModeName());
   }

   return modePtr;
}

// =================================================================================================
ScriptMediumClass::ScriptMediumClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptObjectClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfCommMedium");
   mConstructible = false;
   mCloneable     = false;

   AddMethod(ut::make_unique<ModeCount>());
   AddMethod(ut::make_unique<CurrentModeIndex>());
   AddMethod(ut::make_unique<CurrentModeName>());
   AddMethod(ut::make_unique<GetCurrentMode>());
   AddMethod(ut::make_unique<GetMode_1>("GetMode"));
   AddMethod(ut::make_unique<GetMode_2>("GetMode"));
   AddMethod(ut::make_unique<ChannelCount>());

   AddMethod(ut::make_unique<TransmissionCount>());
   AddMethod(ut::make_unique<MessageStatusCount>());
   AddMethod(ut::make_unique<GetMessageStatus>());
   AddMethod(ut::make_unique<GetMessageStatusByIndex>());

   AddMethod(ut::make_unique<IsShared>());

   AddMethod(ut::make_unique<SetMode_1>("SetMode"));
   AddMethod(ut::make_unique<SetMode_2>("SetMode"));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumClass, Medium, ModeCount, 0, "int", "")
{
   aReturnVal.SetInt(ut::cast_to_int(aObjectPtr->GetNumModes()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumClass, Medium, CurrentModeIndex, 0, "int", "")
{
   aReturnVal.SetInt(ut::cast_to_int(aObjectPtr->GetCurrentModeIndex()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumClass, Medium, CurrentModeName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetActiveModeName());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumClass, Medium, GetCurrentMode, 0, "WsfCommMediumMode", "")
{
   auto modePtr = aObjectPtr->GetMode(aObjectPtr->GetCurrentModeIndex());
   aReturnVal.SetPointer(UtScriptRef::Ref(modePtr, aReturnClassPtr));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumClass, Medium, GetMode_1, 1, "WsfCommMediumMode", "int")
{
   auto index   = aVarArgs[0].GetInt();
   auto modePtr = aObjectPtr->GetMode(ut::cast_to_size_t(index));
   aReturnVal.SetPointer(UtScriptRef::Ref(modePtr, aReturnClassPtr));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumClass, Medium, GetMode_2, 1, "WsfCommMediumMode", "string")
{
   auto modeName = aVarArgs[0].GetString();
   auto modePtr  = aObjectPtr->GetMode(modeName);
   aReturnVal.SetPointer(UtScriptRef::Ref(modePtr, aReturnClassPtr));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumClass, Medium, ChannelCount, 0, "int", "")
{
   aReturnVal.SetInt(ut::cast_to_int(aObjectPtr->GetNumChannels()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumClass, Medium, TransmissionCount, 0, "int", "")
{
   aReturnVal.SetInt(ut::cast_to_int(aObjectPtr->GetCurrentTransmissions()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumClass, Medium, MessageStatusCount, 0, "int", "")
{
   aReturnVal.SetInt(ut::cast_to_int(aObjectPtr->GetNumMessageStatuses()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumClass, Medium, GetMessageStatus, 3, "WsfCommMediumMessageStatus", "int, WsfAddress, WsfAddress")
{
   auto           serialNumber     = aVarArgs[0].GetInt();
   auto           senderAddress    = aVarArgs[1].GetPointer()->GetAppObject<Address>();
   auto           receiverAddress  = aVarArgs[2].GetPointer()->GetAppObject<Address>();
   MessageStatus* messageStatusPtr = nullptr;

   if (senderAddress && receiverAddress)
   {
      Message::Identifier ident;
      ident.SetSerialNumber(serialNumber);
      ident.SetSource(*senderAddress);
      ident.SetDestination(*receiverAddress);

      messageStatusPtr = aObjectPtr->GetMessageStatus(ident);
   }

   aReturnVal.SetPointer(UtScriptRef::Ref(messageStatusPtr, aReturnClassPtr));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumClass, Medium, GetMessageStatusByIndex, 1, "WsfCommMediumMessageStatus", "int")
{
   auto index            = aVarArgs[0].GetInt();
   auto messageStatusPtr = aObjectPtr->GetMessageStatusByIndex(ut::cast_to_size_t(index));
   aReturnVal.SetPointer(UtScriptRef::Ref(messageStatusPtr, aReturnClassPtr));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumClass, Medium, IsShared, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsMediumShared());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumClass, Medium, SetMode_1, 1, "bool", "int")
{
   auto index = aVarArgs[0].GetInt();
   aReturnVal.SetBool(aObjectPtr->SetActiveMode(ut::cast_to_size_t(index)));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumClass, Medium, SetMode_2, 1, "bool", "string")
{
   auto modeName = aVarArgs[0].GetString();
   aReturnVal.SetBool(aObjectPtr->SetActiveMode(modeName));
}

} // namespace medium
} // namespace comm
} // namespace wsf
