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

#include "WsfEM_ClutterTypes.hpp"

#include <memory>
#include <sstream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "WsfScenario.hpp"

// Pre-defined object factories
#include "WsfEM_SurfaceClutterTable.hpp"

class WsfEM_NullClutter : public WsfEM_Clutter
{
public:
   WsfEM_NullClutter()
      : WsfEM_Clutter()
   {
   }
   WsfEM_Clutter* Clone() const override { return new WsfEM_NullClutter(); }
   double ComputeClutterPower(WsfEM_Interaction& aInteraction, WsfEnvironment& aEnvironment, double aProcessingFactor) override
   {
      return 0.0;
   }
   bool IsNullModel() const override { return true; }
};

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfEM_ClutterTypes& WsfEM_ClutterTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetEM_ClutterTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfEM_ClutterTypes& WsfEM_ClutterTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetEM_ClutterTypes();
}

// =================================================================================================
WsfEM_ClutterTypes::WsfEM_ClutterTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfEM_Clutter>(aScenario, cREDEFINITION_ALLOWED, "clutter_model")
{
   mUniqueId = 0;
   mObjectFactoryList.push_back(WsfEM_SurfaceClutterTable::ObjectFactory);
}

// =================================================================================================
//! Add an object factory for creating an instance from a fundamental type.
//! The factory should be a static method and should be added only once.
// static
void WsfEM_ClutterTypes::AddObjectFactory(FactoryPtr aFactoryPtr)
{
   mObjectFactoryList.push_back(aFactoryPtr);
}

// =================================================================================================
//! Process a possible reference to a clutter model type.
//! @param aInput    [input]  The input stream.
//! @param aTypeName [output] The name of the referenced clutter model type
//! (valid only if the function return value is true).
//! @returns true if the command was a reference to a clutter model or false if not.
bool WsfEM_ClutterTypes::LoadReference(UtInput& aInput, WsfStringId& aTypeName)
{
   std::string command(aInput.GetCommand());
   if ((command != "clutter") && (command != "clutter_model"))
   {
      return false;
   }

   std::string blockTerminator("end_" + command);
   std::string typeName;
   aInput.ReadValue(typeName);
   if (typeName == "none")
   {
      aTypeName.Clear();
      // 'end_clutter[_model]' is optional after 'none', but it must be the next command.
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
      // definition (terminated by an 'end_clutter[_model]]'). In that case we'll implicitly create
      // a user type and return the generated name of the created type.
      //
      // If it wasn't one of the requested types then this is simply a reference to a user model type
      // which will be resolved when the requesting object is initialized. In this case there is no
      // 'end_clutter[_model]]'.
      //
      // The latter is the preferred form because it allows the user to define a default model that can
      // be overridden.

      std::unique_ptr<WsfEM_Clutter> userTypePtr(CreateInstance(typeName));
      if (userTypePtr.get() == nullptr)
      {
         // The requested type name was not one of the core object types, so simply return the name.
         aTypeName = typeName;
      }
      else
      {
         // Inline definition. Populate with the data up to the 'end_clutter[_model]' command.
         UtInputBlock inputBlock(aInput, blockTerminator);
         inputBlock.ProcessInput(userTypePtr.get());

         // Generate a unique name for the inline definition and add to the list of user-defined types.
         std::ostringstream oss;
         oss << "__clutter:" << ++mUniqueId << "__";
         aTypeName = oss.str(); // Capture and return the generated name
         if (!Add(aTypeName, std::move(userTypePtr)))
         {
            throw UtInput::BadValue(aInput, "Unable to register clutter_model: " + typeName);
         }
      }
   }
   return true;
}

// =================================================================================================
//! Process clutter_model type input.
//! @param aInput The current input stream.
//! @return 'true' if the current command was recognized and processed
//! 'false' if not recognized.
WsfEM_ClutterTypes::LoadResult WsfEM_ClutterTypes::LoadType(UtInput& aInput)
{
   LoadResult  result;
   std::string command(aInput.GetCommand());
   if ((command == "clutter") || (command == "clutter_model"))
   {
      std::string blockTerminator("end_" + command);
      std::string userTypeName;
      std::string baseTypeName;
      aInput.ReadValue(userTypeName);
      aInput.ReadValue(baseTypeName);

      if (baseTypeName == "none")
      {
         // 'end_clutter[_model]' is optional after 'none', but it must be the next command.
         aInput.ReadValue(command);
         if (command != blockTerminator)
         {
            aInput.PushBack(command);
         }
         std::unique_ptr<WsfEM_Clutter> userTypePtr(new WsfEM_NullClutter());
         result.mObjectTypePtr = userTypePtr.get();
         if (!Add(userTypeName, std::move(userTypePtr)))
         {
            throw UtInput::BadValue(aInput, "Unable to register clutter_model: " + userTypeName);
         }
         result.mIsCommandProcessed = true;
      }
      else
      {
         UtInputBlock                   inputBlock(aInput, blockTerminator);
         std::unique_ptr<WsfEM_Clutter> userTypePtr(CreateInstance(baseTypeName));
         if (userTypePtr.get() == nullptr)
         {
            throw UtInput::BadValue(aInput, "Unknown clutter_model type: " + baseTypeName);
         }
         inputBlock.ProcessInput(userTypePtr.get());
         result.mObjectTypePtr = userTypePtr.get();
         if (!Add(userTypeName, std::move(userTypePtr)))
         {
            throw UtInput::BadValue(aInput, "Unable to register clutter_model: " + userTypeName);
         }
         result.mIsCommandProcessed = true;
      }
   }

   return result;
}

// =================================================================================================
WsfEM_Clutter* WsfEM_ClutterTypes::CreateInstance(const std::string& aTypeName)
{
   WsfEM_Clutter* instancePtr = nullptr;
   for (size_t i = 0; i < mObjectFactoryList.size() && !instancePtr; ++i)
   {
      instancePtr = (*mObjectFactoryList[i])(aTypeName);
   }
   return instancePtr;
}
