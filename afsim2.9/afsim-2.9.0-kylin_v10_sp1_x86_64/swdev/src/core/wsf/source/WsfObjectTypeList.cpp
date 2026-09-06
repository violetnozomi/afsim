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

#include "WsfObjectTypeList.hpp"

#include <cassert>
#include <memory>
#include <sstream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "WsfComponent.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfDeferredInput.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformPart.hpp"
#include "WsfScenario.hpp"

// =================================================================================================
WsfObjectTypeListBase::WsfObjectTypeListBase(unsigned int aFlags, const std::string& aBlockName, WsfScenario* aScenarioPtr)
   : mTypeMap()
   , mRedefinedTypes()
   , mFlags(aFlags)
   , mBlockName(aBlockName)
   , mCreateSingularTypeFunction(nullptr)
   , mFactoryCreateFunction(nullptr)
   , mFactoryCreateDefault(nullptr)
   , mFactoryPtr(nullptr)
   , mScenarioPtr(aScenarioPtr)
{
}

// =================================================================================================
WsfObjectTypeListBase::~WsfObjectTypeListBase()
{
   Clear();
}

// =================================================================================================
void WsfObjectTypeListBase::Clear()
{
   for (TypeMap::iterator iter = mTypeMap.begin(); iter != mTypeMap.end();)
   {
      if (mCoreTypes.find(iter->first.GetString()) != mCoreTypes.end())
      {
         ++iter; // Don't erase core types
      }
      else
      {
         iter = mTypeMap.erase(iter);
      }
   }

   // Also clear the redefined type list.
   mRedefinedTypes.clear();
}

// =================================================================================================
void WsfObjectTypeListBase::GetTypeIds(TypeIdList& aListOfIds) const
{
   aListOfIds.clear();
   for (const auto& iter : mTypeMap)
   {
      aListOfIds.push_back(iter.first);
   }
}

// =================================================================================================
bool WsfObjectTypeListBase::ProcessInput(UtInput& aInput)
{
   return false;
}

// =================================================================================================
bool WsfObjectTypeListBase::AddP(WsfStringId aId, std::unique_ptr<WsfObject> aDefinitionPtr)
{
   mScenarioPtr->ScenarioChange();

   bool typeAdded = false;
   // The base type of the new type is the type of the object from which is was derived.
   aDefinitionPtr->SetTypeAndBaseType(aId, aDefinitionPtr->GetTypeId());
   TypeMap::iterator tmi = mTypeMap.find(aId);
   if (tmi == mTypeMap.end())
   {
      mTypeMap[aId] = std::move(aDefinitionPtr);
      typeAdded     = true;
   }
   else if (mFlags & cREDEFINITION_ALLOWED)
   {
      // If a type is redefined, the old type is not deleted because somebody *MAY* be using it.
      // This is just for safety because it probably won't happen.
      mRedefinedTypes.push_back(std::move((*tmi).second));
      (*tmi).second = std::move(aDefinitionPtr);
      typeAdded     = true;
      // Throw a redefinition warning if its a derived type (likely a defined user type)
      if ((*tmi).second->GetTypeList().size() > 1)
      {
         const std::string& derivedType(aId.GetString());
         std::string        baseType((*tmi).second->GetBaseType());
         auto               out = ut::log::warning() << "Derived type redefined.";
         out.AddNote() << "Base: " << baseType;
         out.AddNote() << "Derived: " << derivedType;
      }
   }
   return typeAdded;
}

// =================================================================================================
WsfObject* WsfObjectTypeListBase::CloneP(WsfStringId aId) const
{
   WsfObject* instancePtr   = nullptr;
   WsfObject* definitionPtr = FindP(aId);

   if (CanDelayLoad() && (definitionPtr == nullptr) && GetDeferredInput().IsDelayLoading() && !mBlockName.empty())
   {
      GetDeferredInput().Requires(mBlockName, aId.GetString());
   }
   if (definitionPtr != nullptr)
   {
      instancePtr = definitionPtr->Clone();
      instancePtr->SetType(aId);
   }
   return instancePtr;
}

// =================================================================================================
WsfObject* WsfObjectTypeListBase::FindP(WsfStringId aId) const
{
   WsfObject*              definitionPtr = nullptr;
   TypeMap::const_iterator iter          = mTypeMap.find(aId);
   if (iter != mTypeMap.end())
   {
      definitionPtr = (*iter).second.get();
   }
   return definitionPtr;
}

// =================================================================================================
namespace WsfObjectTypeListDetail
{
class LoadTypeP_Later
{
public:
   WsfObjectTypeListBase* mObjectTypeListPtr;

   bool operator()(UtInput& aInput)
   {
      WsfObject* loadedObjectPtr = nullptr;
      return mObjectTypeListPtr->DeferredLoadTypeP(aInput, loadedObjectPtr);
   }
};
} // namespace WsfObjectTypeListDetail

// =================================================================================================
bool WsfObjectTypeListBase::LoadTypeP(UtInput& aInput, WsfObject*& aLoadedObjectPtr)
{
   aLoadedObjectPtr = nullptr;
   if (aInput.GetCommand() == mBlockName)
   {
      UtInputBlock inputBlock(aInput);

      std::string derivedType;

      if (CanDelayLoad() && !GetDeferredInput().IsDelayLoading())
      {
         UtInput::StoredLocation loc = aInput.StoreLocation();

         aInput.ReadValue(derivedType);
         std::string cmd;
         while (inputBlock.ReadCommand(cmd))
         {
         }
         WsfObjectTypeListDetail::LoadTypeP_Later loadLater;
         loadLater.mObjectTypeListPtr = this;
         GetDeferredInput().DelayLoad(mBlockName, derivedType, loc, loadLater);
         return true;
      }

      aInput.ReadValue(derivedType);
      std::string baseType;

      WsfObject* newObjectPtr = nullptr;
      if (mCreateSingularTypeFunction == nullptr)
      {
         aInput.ReadValue(baseType);
         if (mFactoryCreateFunction == nullptr)
         {
            newObjectPtr = CloneP(baseType);
            if (newObjectPtr == nullptr)
            {
               GetDeferredInput().Requires(mBlockName, baseType);
               newObjectPtr = CloneP(baseType);
            }
         }
         else
         {
            newObjectPtr = (*mFactoryCreateFunction)(mFactoryPtr, baseType);
            if (newObjectPtr == nullptr && mFactoryCreateDefault != nullptr)
            {
               newObjectPtr = (*mFactoryCreateDefault)();
               aInput.PushBack(baseType);
            }
            if (newObjectPtr == nullptr)
            {
               std::stringstream ss;
               ss << baseType << " is not a valid " << mBlockName << " type.";
               throw UtInput::BadValue(aInput, ss.str());
            }
         }
      }
      else
      {
         newObjectPtr = (*mCreateSingularTypeFunction)();
         assert(newObjectPtr); // should always return a valid object
         newObjectPtr->SetName(derivedType);
      }

      std::unique_ptr<WsfObject> derivedPtr(newObjectPtr);
      if (derivedPtr != nullptr)
      {
         // If the new object is a component then invoke its PreInput method.
         // See WsfComponent::PreInput for more information.
         WsfComponent* componentPtr = dynamic_cast<WsfComponent*>(derivedPtr.get());
         if (componentPtr != nullptr)
         {
            componentPtr->PreInput();
         }
         inputBlock.ProcessInput(derivedPtr.get());
         if (mFlags & cREDEFINITION_ALLOWED)
         {
            WsfObject* existingDefinitionPtr = FindP(derivedType);
            bool hasFactoryFunction = (mFactoryCreateFunction != nullptr) || (mCreateSingularTypeFunction != nullptr);
            if ((!hasFactoryFunction) && (existingDefinitionPtr != nullptr) &&
                (existingDefinitionPtr->GetTypeList().size() <= 1))
            {
               throw UtInput::BadValue(aInput, derivedType + " cannot be redefined because it is a basic type");
            }
         }
         auto* rawDerivedPtr = derivedPtr.get();
         if (AddP(derivedType, std::move(derivedPtr)))
         {
            if (!InitializeTypeP(rawDerivedPtr))
            {
               // If an error occurs then the entry must be removed from the type list (added by AddP) so it is not
               // deleted by Clear. 'unique_ptr' will cause it to be deleted when the exception is thrown, so we don't
               // want to delete it again in clear.
               mTypeMap.erase(derivedType);
               std::stringstream ss;
               ss << "Initialization failed for " << mBlockName << ": " << derivedType;
               throw UtInput::BadValue(aInput, ss.str());
            }
            aLoadedObjectPtr = rawDerivedPtr;
         }
         else
         {
            std::stringstream errorText;
            errorText << "Duplicate " << mBlockName << " type: " << derivedType;
            throw UtInput::BadValue(aInput, errorText.str());
         }
      }
      else
      {
         std::stringstream errorText;
         errorText << "Unknown " << mBlockName << " type: " << baseType;
         throw UtInput::BadValue(aInput, errorText.str());
      }
   }
   return aLoadedObjectPtr != nullptr;
}

// =================================================================================================
bool WsfObjectTypeListBase::DeferredLoadTypeP(UtInput& aInput, WsfObject*& aLoadedObjectPtr)
{
   return LoadTypeP(aInput, aLoadedObjectPtr);
}

// =================================================================================================
WsfDeferredInput& WsfObjectTypeListBase::GetDeferredInput() const
{
   if (mScenarioPtr != nullptr)
   {
      return mScenarioPtr->GetDeferredInput();
   }
   throw;
}

// =================================================================================================
//! Process input to delete a component instance from a platform.
//! @param aInput     The current input stream.
//! @param aPlatform  A reference to the platform.
//! @param aRole      The role of the component (for 'edit').
//! @param aIsNamed   'true' if the component is explicitly named, or false if it is unnamed.
//! @returns true if the input was recognized and processed, 'false' if the input was not processed.
//! @throws UtInput::BadValue if the requested component instance does not exist.
bool WsfObjectTypeListBase::DeleteComponentP(UtInput& aInput, WsfPlatform& aPlatform, int aRole, bool aIsNamed)
{
   bool myCommand(false);
   if (aInput.GetCommand() == mBlockName)
   {
      myCommand = true;
      if (aIsNamed)
      {
         // e.g.: delete sensor <name>
         std::string instanceName(mBlockName);
         aInput.ReadValue(instanceName);
         if (!aPlatform.DeleteComponent(instanceName, aRole))
         {
            throw UtInput::BadValue(aInput, mBlockName + " '" + instanceName + "' does not exist");
         }
      }
      else
      {
         // e.g.: delete mover
         if (!aPlatform.DeleteComponent(mBlockName, aRole))
         {
            throw UtInput::BadValue(aInput, mBlockName + " does not exist");
         }
      }
   }
   return myCommand;
}

// =================================================================================================
//! Process input to add or edit a component instance on a platform.
//! This method looks at the current input stream and decides if the input pertains to a platform
//! component of the object type maintained by this type list.
//! @param aInput      The current input stream.
//! @param aPlatform   A reference to the platform.
//! @param aIsAdding   'true' if adding a new component, or 'false' if to edit an existing component.
//! @param aRole       The role of the component.
//! @param aIsNamed    'true' if the component is explicitly named, or false if it is unnamed.
//! @param aIsEditable 'true' if the component can be edited or modified while added to a platform,
//!                    'false' otherwise.
//! @returns true if the input was recognized and processed, 'false' if the input was not processed.
//! @throws UtInput::BadValue if:
//! - aIsAdding is true and the requested base type does not exist.
//! - aIsAdding is true and the requested instance name already exists on the platform.
//! - aIsAdding is false and the requested instance name does not exist on the platform.
//! @throws UtInput::OutOfContext if:
//! - aIsEditable is false, aIsAdding is false, and the input is requesting an edit.
bool WsfObjectTypeListBase::LoadComponentP(UtInput&     aInput,
                                           WsfPlatform& aPlatform,
                                           bool         aIsAdding,
                                           int          aRole,
                                           bool         aIsNamed,
                                           bool         aIsEditable)
{
   bool myCommand(false);
   if (aInput.GetCommand() == mBlockName)
   {
      std::string instanceName(mBlockName);
      // Read the instance name for components that are identified by 'name' on the platform.
      // For components that aren't named we will simply use the block name.
      if (aIsNamed)
      {
         aInput.ReadValue(instanceName);
      }
      if (aIsAdding)
      {
         // Adding a new instance
         // e.g.: [add] sensor <name> <type>               (named component)
         //       [add] mover <type>                       (unnamed component)
         std::string baseType;
         aInput.ReadValue(baseType);
         WsfObject* objectPtr(CloneP(baseType));
         if (objectPtr == nullptr)
         {
            // The base type does not yet exist, but it may be declared later. Indicate the base type is
            // required and use a place holder.
            GetDeferredInput().Requires(mBlockName, baseType);
            objectPtr = CloneP(baseType);
         }
         std::unique_ptr<WsfPlatformComponent> instancePtr(dynamic_cast<WsfPlatformComponent*>(objectPtr));
         if (instancePtr != nullptr)
         {
            // Set the component if it is defined the have one. The component interface only has an
            // interface to get the component name, not set it. Therefore the pointer the WsfObject
            // must be used (which has SetName)
            if (aIsNamed)
            {
               objectPtr->SetName(instanceName);
            }

            // The pointer to the platform within the component MUST be set even though it isn't yet formally
            // added. In particular, script parsing needs this so it can access the platforms script context.
            instancePtr->SetComponentParent(&aPlatform);

            if (aIsEditable)
            {
               // See the documentation for WsfComponent::PreInput as for what the next call accomplishes.
               instancePtr->PreInput();
               UtInputBlock inputBlock(aInput);
               inputBlock.ProcessInput(instancePtr.get());
            }
            if (aPlatform.AddComponent(instancePtr.get()))
            {
               instancePtr.release(); // Prevent deletion of the cloned object
               myCommand = true;
            }
            else
            {
               throw UtInput::BadValue(aInput, "Duplicate " + mBlockName + ": " + instanceName);
            }
         }
         else
         {
            // If the caller MISTAKENLY calls this method for a type list that does not contain objects
            // derived from WsfComponent, then the dynamic cast would fail even though the requested
            // base type exist. This would leak memory, so throw an error if the pointer is non-zero.
            assert(objectPtr == nullptr);
            throw UtInput::BadValue(aInput, "Unknown " + mBlockName + " type: " + baseType);
         }
      }
      else if (aIsEditable)
      {
         // Edit an existing instance
         // e.g.: [edit] sensor <name>                     (named component)
         //       [edit] mover                             (unnamed component)
         WsfComponent* instancePtr = aPlatform.FindComponent(instanceName, aRole);
         if (instancePtr != nullptr)
         {
            UtInputBlock inputBlock(aInput);
            inputBlock.ProcessInput(instancePtr);
            myCommand = true;
         }
         else
         {
            throw UtInput::BadValue(aInput, "Unknown " + mBlockName + ": " + instanceName);
         }
      }
      else
      {
         throw UtInput::OutOfContext(aInput, "Cannot Edit " + mBlockName);
      }
   }
   return myCommand;
}
