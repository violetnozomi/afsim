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

#include "WsfEM_PropagationTypes.hpp"

#include <memory>
#include <sstream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "WsfScenario.hpp"

// Pre-defined object factories.
#include "WsfEM_FastMultipath.hpp"
#include "WsfEM_GroundWavePropagation.hpp"

//! The 'null' (no-effect) model.
class WsfEM_NullPropagation : public WsfEM_Propagation
{
public:
   WsfEM_NullPropagation() = default;
   WsfEM_Propagation* Clone() const override { return new WsfEM_NullPropagation(); }
   double             ComputePropagationFactor(WsfEM_Interaction& aInteraction, WsfEnvironment& aEnvironment) override
   {
      return 0.0;
   }
   bool IsNullModel() const override { return true; }
};


// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfEM_PropagationTypes& WsfEM_PropagationTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetEM_PropagationTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfEM_PropagationTypes& WsfEM_PropagationTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetEM_PropagationTypes();
}

// =================================================================================================
WsfEM_PropagationTypes::WsfEM_PropagationTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfEM_Propagation>(aScenario, cREDEFINITION_ALLOWED, "propagation_model")
{
   mUniqueId = 0;
   AddObjectFactory(WsfEM_FastMultipath::ObjectFactory);
   AddObjectFactory(WsfEM_GroundWavePropagation::ObjectFactory);
}

// =================================================================================================
//! Add an object factory for creating an instance from a fundamental type.
//! The factory should be a static method and should be added only once.
// static
void WsfEM_PropagationTypes::AddObjectFactory(FactoryPtr aFactoryPtr)
{
   mObjectFactoryList.push_back(aFactoryPtr);
}

// =================================================================================================
//! Process a possible reference to a propagation model type.
//! @param aInput    [input]  The input stream.
//! @param aTypeName [output] The name of the referenced propagation model type
//! (valid only if the function return value is true).
//! @returns true if the command was a reference to a propagation model or false if not.
bool WsfEM_PropagationTypes::LoadReference(UtInput& aInput, WsfStringId& aTypeName)
{
   std::string command(aInput.GetCommand());
   if ((command != "propagation") && (command != "propagation_model"))
   {
      return false;
   }

   std::string blockTerminator("end_" + command);
   std::string typeName;
   aInput.ReadValue(typeName);
   if (typeName == "none")
   {
      aTypeName.Clear();
      // 'end_propagation[_model]' is optional after 'none', but it must be the next command.
      aInput.ReadValue(command);
      if (command != blockTerminator)
      {
         aInput.PushBack(command);
      }
   }
   else
   {
      // This is a bit of a hack, but it is provided for backward compatibility.
      //
      // If the requested type is one of the core object types then it is assumed to be and inline
      // definition (terminated by an 'end_propagation[_model]]'). In that case we'll implicitly create
      // a user type and return the generated name of the created type.
      //
      // If it wasn't one of the requested types then this is simply a reference to a user model type
      // which will be resolved when the requesting object is initialized. In this case there is no
      // 'end_propagation[_model]]'.
      //
      // The latter is the preferred form because it allows the user to define a default model that can
      // be overridden.

      std::unique_ptr<WsfEM_Propagation> userTypePtr(CreateInstance(typeName));
      if (userTypePtr == nullptr)
      {
         // The requested type name was not one of the core object types, so simply return the name.
         aTypeName = typeName;
      }
      else
      {
         // Inline definition. Populate with the data up to the 'end_propagation[_model]' command.
         UtInputBlock inputBlock(aInput, blockTerminator);
         inputBlock.ProcessInput(userTypePtr.get());

         // Generate a unique name for the inline definition and add to the list of user-defined types.
         std::ostringstream oss;
         oss << "__propagation:" << ++mUniqueId << "__";
         aTypeName = oss.str(); // Capture and return the generated name
         if (!Add(aTypeName, std::move(userTypePtr)))
         {
            throw UtInput::BadValue(aInput, "Unable to register propagation_model: " + typeName);
         }
      }
   }
   return true;
}

// =================================================================================================
//! Process propagation_model type input.
//! @param aInput The current input stream.
//! @return 'true' if the current command was recognized and processed
//! 'false' if not recognized.
WsfEM_PropagationTypes::LoadResult WsfEM_PropagationTypes::LoadType(UtInput& aInput)
{
   LoadResult  result;
   std::string command(aInput.GetCommand());
   if ((command == "propagation") || (command == "propagation_model"))
   {
      std::string blockTerminator("end_" + command);
      std::string userTypeName;
      std::string baseTypeName;
      aInput.ReadValue(userTypeName);
      aInput.ReadValue(baseTypeName);

      if (baseTypeName == "none")
      {
         // 'end_propagation[_model]' is optional after 'none', but it must be the next command.
         aInput.ReadValue(command);
         if (command != blockTerminator)
         {
            aInput.PushBack(command);
         }
         std::unique_ptr<WsfEM_Propagation> userTypePtr(new WsfEM_NullPropagation());
         result.mObjectTypePtr = userTypePtr.get();
         if (!Add(userTypeName, std::move(userTypePtr)))
         {
            throw UtInput::BadValue(aInput, "Unable to register propagation_model: " + userTypeName);
         }
         result.mIsCommandProcessed = true;
      }
      else
      {
         UtInputBlock                       inputBlock(aInput, blockTerminator);
         std::unique_ptr<WsfEM_Propagation> userTypePtr(CreateInstance(baseTypeName));
         if (userTypePtr == nullptr)
         {
            throw UtInput::BadValue(aInput, "Unknown propagation_model type: " + baseTypeName);
         }
         inputBlock.ProcessInput(userTypePtr.get());
         result.mObjectTypePtr = userTypePtr.get();
         if (!Add(userTypeName, std::move(userTypePtr)))
         {
            throw UtInput::BadValue(aInput, "Unable to register propagation_model: " + userTypeName);
         }
         result.mIsCommandProcessed = true;
      }
   }

   return result;
}


// =================================================================================================
// private
WsfEM_Propagation* WsfEM_PropagationTypes::CreateInstance(const std::string& aTypeName)
{
   WsfEM_Propagation* instancePtr = nullptr;

   auto fli = mObjectFactoryList.begin();
   while ((fli != mObjectFactoryList.end()) && (instancePtr == nullptr))
   {
      FactoryPtr factoryPtr = *fli;
      instancePtr           = (*factoryPtr)(aTypeName);
      ++fli;
   }
   return instancePtr;
}
