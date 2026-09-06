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

#include "WsfEM_AttenuationTypes.hpp"

#include <memory>
#include <sstream>

#include "UtInputBlock.hpp"
#include "WsfScenario.hpp"

// Pre-defined object factories.
#include "WsfEM_BlakeAttenuation.hpp"
#include "WsfEM_ITU_Attenuation.hpp"
#include "WsfEM_SimpleAttenuation.hpp"
#include "WsfTabularAttenuation.hpp"

//! The 'null' (no-effect) model.
class WsfEM_NullAttenuation : public WsfEM_Attenuation
{
public:
   WsfEM_NullAttenuation() {}
   WsfEM_Attenuation* Clone() const override { return new WsfEM_NullAttenuation(); }
   bool               IsNullModel() const override { return true; }

protected:
   double ComputeAttenuationFactorP(double aRange, double aElevation, double aAltitude, double aFrequency) override
   {
      return 1.0;
   }
};

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfEM_AttenuationTypes& WsfEM_AttenuationTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetEM_AttenuationTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfEM_AttenuationTypes& WsfEM_AttenuationTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetEM_AttenuationTypes();
}

// =================================================================================================
WsfEM_AttenuationTypes::WsfEM_AttenuationTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfEM_Attenuation>(aScenario, cREDEFINITION_ALLOWED, "attenuation_model")
{
   mUniqueId = 0;
   AddObjectFactory(WsfEM_BlakeAttenuation::ObjectFactory);
   AddObjectFactory(WsfEM_ITU_Attenuation::GetObjectFactory(aScenario));
   AddObjectFactory(WsfEM_SimpleAttenuation::ObjectFactory);
   AddObjectFactory(WsfTabularAttenuation::ObjectFactory);
}

// =================================================================================================
//! Add an object factory for creating an instance from a fundamental type.
//! The factory should be a static method and should be added only once.
void WsfEM_AttenuationTypes::AddObjectFactory(const Factory& aFactory)
{
   mObjectFactoryList.push_back(aFactory);
}

// =================================================================================================
//! Create a clone of a type object of a given name.
//!
//! The returned object can either be used an instance of the specified type
//! or as a base on which to create a new derived type.
//!
//! @param aTypeName The name of a type object that is to be cloned.
//! @returns A pointer to the clone of the requested type or 0 if the requested type does not exist.
WsfEM_Attenuation* WsfEM_AttenuationTypes::Clone(WsfStringId aTypeName) const
{
   // First try one of the user-defined types.
   WsfEM_Attenuation* attenuationPtr = WsfObjectTypeList<WsfEM_Attenuation>::Clone(aTypeName);
   if (attenuationPtr == nullptr)
   {
      // Not one of the user-defined types, so try one of the core types.
      attenuationPtr = CreateInstance(aTypeName.GetString());
   }
   return attenuationPtr;
}
// =================================================================================================
//! Process a possible reference to a attenuation model type.
//! @param aInput    [input]  The input stream.
//! @param aTypeName [output] The name of the referenced attenuation model type
//! (valid only if the function return value is true).
//! @returns true if the command was a reference to a attenuation model or false if not.
bool WsfEM_AttenuationTypes::LoadReference(UtInput& aInput, WsfStringId& aTypeName)
{
   std::string command(aInput.GetCommand());
   if ((command != "attenuation") && (command != "attenuation_model"))
   {
      return false;
   }

   std::string blockTerminator("end_" + command);
   std::string typeName;
   aInput.ReadValue(typeName);
   if (typeName == "none")
   {
      aTypeName.Clear();
      // 'end_attenuation[_model]' is optional after 'none', but it must be the next command.
      aInput.ReadValue(command);
      if (command != blockTerminator)
      {
         aInput.PushBack(command);
      }
   }
   else
   {
      std::unique_ptr<WsfEM_Attenuation> userTypePtr(CreateInstance(typeName));
      if (userTypePtr.get() == nullptr)
      {
         // The requested type name was not one of the core object types.
         // It is a reference to a user-defined model.
         aTypeName = typeName;
      }
      else if (!userTypePtr->AcceptsInlineBlockInput())
      {
         // The request type was one of the core types, but does not accept inline block input.
         // (This is typically one of the older models like 'blake').
         //
         // Simply return a reference to the core model.
         aTypeName = typeName;
         // 'end_attenuation[_model]' is optional, but it must be the next command.
         aInput.ReadValue(command);
         if (command != blockTerminator)
         {
            aInput.PushBack(command);
         }
      }
      else
      {
         // Inline definition. Populate with the data up to the 'end_attenuation[_model]' command.
         UtInputBlock inputBlock(aInput, blockTerminator);
         inputBlock.ProcessInput(userTypePtr.get());

         // Generate a unique name for the inline definition and add to the list of user-defined types.
         std::ostringstream oss;
         oss << "__attenuation:" << ++mUniqueId << "__";
         aTypeName = oss.str(); // Capture and return the generated name
         if (!Add(aTypeName, std::move(userTypePtr)))
         {
            throw UtInput::BadValue(aInput, "Unable to register attenuation_model: " + typeName);
         }
      }
   }
   return true;
}

// =================================================================================================
//! Process attenuation_model type input.
//! @param aInput The current input stream.
//! @return 'true' if the current command was recognized and processed, 'false' if not recognized.
WsfEM_AttenuationTypes::LoadResult WsfEM_AttenuationTypes::LoadType(UtInput& aInput)
{
   LoadResult  result;
   std::string command(aInput.GetCommand());
   if ((command == "attenuation") || (command == "attenuation_model"))
   {
      std::string blockTerminator("end_" + command);
      std::string userTypeName;
      std::string baseTypeName;
      aInput.ReadValue(userTypeName);
      aInput.ReadValue(baseTypeName);

      if (baseTypeName == "none")
      {
         // 'end_attenuation[_model]' is optional after 'none', but it must be the next command.
         aInput.ReadValue(command);
         if (command != blockTerminator)
         {
            aInput.PushBack(command);
         }
         std::unique_ptr<WsfEM_Attenuation> userTypePtr(new WsfEM_NullAttenuation());
         result.mObjectTypePtr = userTypePtr.get();
         if (!Add(userTypeName, std::move(userTypePtr)))
         {
            throw UtInput::BadValue(aInput, "Unable to register attenuation_model: " + userTypeName);
         }
         result.mIsCommandProcessed = true;
      }
      else
      {
         UtInputBlock                       inputBlock(aInput, blockTerminator);
         std::unique_ptr<WsfEM_Attenuation> userTypePtr(CreateInstance(baseTypeName));
         if (userTypePtr.get() == nullptr)
         {
            throw UtInput::BadValue(aInput, "Unknown attenuation_model type: " + baseTypeName);
         }
         inputBlock.ProcessInput(userTypePtr.get());
         result.mObjectTypePtr = userTypePtr.get();
         if (!Add(userTypeName, std::move(userTypePtr)))
         {
            throw UtInput::BadValue(aInput, "Unable to register attenuation_model: " + userTypeName);
         }
         result.mIsCommandProcessed = true;
      }
   }
   return result;
}

WsfEM_Attenuation* WsfEM_AttenuationTypes::CreateInstance(const std::string& aTypeName) const
{
   WsfEM_Attenuation* instancePtr = nullptr;

   ObjectFactoryList::const_iterator fli = mObjectFactoryList.begin();
   while ((fli != mObjectFactoryList.end()) && (instancePtr == nullptr))
   {
      instancePtr = (*fli)(aTypeName);
      ++fli;
   }
   return instancePtr;
}
