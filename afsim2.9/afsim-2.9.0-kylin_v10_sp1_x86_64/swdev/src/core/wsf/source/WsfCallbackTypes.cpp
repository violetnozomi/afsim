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

#include "WsfCallbackTypes.hpp"

#include <memory>

#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "script/WsfScriptCallback.hpp"

namespace
{
//! Component factory to process platform input.
class CallbackComponentFactory : public WsfComponentFactory<WsfPlatform>
{
public:
   bool ProcessInput(UtInput& aInput, WsfPlatform& aPlatform) override
   {
      WsfCallbackTypes& types(WsfCallbackTypes::Get(GetScenario()));
      return types.LoadInstance(aInput, aPlatform);
   }
};
} // namespace

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfCallbackTypes& WsfCallbackTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetCallbackTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfCallbackTypes& WsfCallbackTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetCallbackTypes();
}

// =================================================================================================
WsfCallbackTypes::WsfCallbackTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfCallback>(aScenario, "callback")
{
   aScenario.RegisterComponentFactory(ut::make_unique<CallbackComponentFactory>());

   AddCoreType("WSF_SCRIPT_CALLBACK", ut::make_unique<WsfScriptCallback>());
}

// =================================================================================================
//! Create an instance of a 'type object' of this class
//! Process the current command if it would create an instance of an 'type object' of this class.
//! @param aInput The input stream.
//! @param aPlatform The platform to which the instance is to be added.
//! @return 'true' if the command was processed or 'false' if not.
//! @throws UtInput::ExceptionBase (or derived class) if an error was encountered.
bool WsfCallbackTypes::LoadInstance(UtInput& aInput, WsfPlatform& aPlatform)
{
   bool myCommand = false;
   if (aInput.GetCommand() == "callback")
   {
      myCommand = true;
      UtInputBlock inputBlock(aInput);
      std::string  instanceName;
      aInput.ReadValue(instanceName);
      std::string callbackType;
      aInput.ReadValue(callbackType);
      std::unique_ptr<WsfCallback> instancePtr(Clone(callbackType));
      if (instancePtr.get() != nullptr)
      {
         instancePtr->SetName(instanceName);
         inputBlock.ProcessInput(instancePtr.get());
         if (aPlatform.AddComponent(instancePtr.get()))
         {
            instancePtr.release();
         }
         else
         {
            throw UtInput::BadValue(aInput, "Duplicate callback: " + instanceName);
         }
      }
      else
      {
         throw UtInput::BadValue(aInput, "Invalid callback type " + callbackType);
      }
   }
   else if (aInput.GetCommand() == "use_callback")
   {
      myCommand = true;
      UtInputBlock inputBlock(aInput);
      std::string  callbackType;
      aInput.ReadValue(callbackType);
      std::unique_ptr<WsfCallback> instancePtr(Clone(callbackType));
      if (instancePtr.get() != nullptr)
      {
         instancePtr->SetName(callbackType);
         inputBlock.ProcessInput(instancePtr.get());
         if (aPlatform.AddComponent(instancePtr.get()))
         {
            instancePtr.release();
         }
         else
         {
            throw UtInput::BadValue(aInput, "Duplicate callback: " + callbackType);
         }
      }
      else
      {
         throw UtInput::BadValue(aInput, "Invalid callback type " + callbackType);
      }
   }
   return myCommand;
}

// =================================================================================================
//! Load a new 'type object' from an input stream.
//! Process the current command if it is one that defines a new 'type object' of this class.
//! @param aInput The input stream.
//! @return 'true' if the command was processed or 'false' if not.
//! @throws UtInput::ExceptionBase (or derived class) if an error was encountered.
WsfCallbackTypes::LoadResult WsfCallbackTypes::LoadType(UtInput& aInput)
{
   LoadResult result;
   if (aInput.GetCommand() == "callback")
   {
      UtInputBlock inputBlock(aInput);
      std::string  derivedType;
      std::string  baseType;
      aInput.ReadValue(derivedType);
      aInput.ReadValue(baseType);
      std::unique_ptr<WsfCallback> derivedPtr(Clone(baseType));
      if (derivedPtr.get() != nullptr)
      {
         inputBlock.ProcessInput(derivedPtr.get());
         result.mObjectTypePtr = derivedPtr.get();
         if (!Add(derivedType, std::move(derivedPtr)))
         {
            throw UtInput::BadValue(aInput, "Duplicate callback type: " + derivedType);
         }
      }
      else
      {
         throw UtInput::BadValue(aInput, "Invalid callback type " + baseType);
      }
      result.mIsCommandProcessed = true;
   }
   return result;
}
