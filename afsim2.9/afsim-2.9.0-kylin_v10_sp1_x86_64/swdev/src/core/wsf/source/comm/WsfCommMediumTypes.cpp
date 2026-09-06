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

#include "WsfCommMediumTypes.hpp"

#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfComm.hpp"
#include "WsfCommMediumContainer.hpp"
#include "WsfCommMediumGuided.hpp"
#include "WsfCommMediumUnguided.hpp"
#include "WsfCommRouter.hpp"
#include "WsfDeferredInput.hpp"
#include "WsfScenario.hpp"

namespace wsf
{
namespace comm
{
namespace medium
{

// =================================================================================================
Types& Types::Get(const WsfScenario& aScenario)
{
   return aScenario.GetMediumTypes();
}

// =================================================================================================
Types::Types(WsfScenario& aScenario)
   : WsfObjectTypeList<Medium>(aScenario, "medium")
   , mManager(aScenario)
{
   //! Register component factories for the medium containers.
   aScenario.RegisterComponentFactory(ut::make_unique<ContainerFactory<Comm>>());
   aScenario.RegisterComponentFactory(ut::make_unique<ContainerFactory<router::Router>>());

   //! Register factories for corresponding medium types.
   Guided::RegisterFactory(*this);
   Unguided::RegisterFactory(*this);

   //! Register all unique medium types here.
   AddCoreType("WSF_COMM_MEDIUM_GUIDED", ut::make_unique<Guided>(aScenario));
   AddCoreType("WSF_COMM_MEDIUM_UNGUIDED", ut::make_unique<Unguided>(aScenario));
}

// =================================================================================================
bool Types::AddDefaultMedium(Container& aContainer)
{
   // Adds the indicated default (non-shared) instance of a medium to a container.
   // This only occurs if a container does not already have an assigned medium object.
   // All mediums created via this method are unique instances that are not shared
   // and are clones of core types.
   auto medium = aContainer.GetMedium();
   if (!medium)
   {
      auto defaultMediumType = aContainer.GetDefaultMedium();
      auto coreTypes         = GetCoreTypes();
      auto it                = std::find_if(std::begin(coreTypes),
                             std::end(coreTypes),
                             [&defaultMediumType](Medium* aCoreTypePtr)
                             { return (aCoreTypePtr->GetMediumIdentifier() == defaultMediumType); });

      if (it != std::end(coreTypes))
      {
         auto coreMedium = ut::clone(*it);
         coreMedium->SetName(UtStringIdLiteral("default"));
         aContainer.SetMedium(std::move(coreMedium));
         return true;
      }
   }

   return false;
}

// =================================================================================================
bool Types::LoadMedium(UtInput& aInput, Container& aContainer, bool aIsAdding)
{
   bool myCommand = false;
   if (aInput.GetCommand() == "medium")
   {
      std::string type;
      aInput.ReadValue(type);

      if (aIsAdding)
      {
         // All like-named mediums are shared, such that only one such medium ever exists during
         // the simulation (unless indicated by user input to not share such instances).
         // Check if the medium has been encountered yet, and an instance ready
         // for usage by the simulation. If it has, we don't need to create it.
         std::unique_ptr<WsfObject> clonedMedium(CloneP(type));
         if (!clonedMedium)
         {
            // Deferred input.
            GetDeferredInput().Requires("medium", type);
            clonedMedium.reset(CloneP(type));
         }

         clonedMedium->SetName(UtStringIdLiteral("default"));
         std::unique_ptr<Medium> mediumInstance(dynamic_cast<Medium*>(clonedMedium.release()));
         if (mediumInstance)
         {
            // Ensure that the type specified is allowed.
            if (!aContainer.IsValidMedium(mediumInstance->GetMediumIdentifier()))
            {
               auto out = ut::log::error() << "Cannot add invalid medium type.";
               out.AddNote() << "Medium type: " << type;
               out.AddNote() << "This type is not allowed for this object.";
               throw UtInput::BadValue(aInput, "Invalid medium type: " + type);
            }

            std::shared_ptr<Medium> mediumPtr = nullptr;

            // The user has indicated to NOT share this medium when adding it to
            // container instances. We do not add this type to our shared type list,
            // and simply pass on the cloned copy.
            if (!mediumInstance->UserDirectedSharing())
            {
               mediumPtr = std::move(mediumInstance);
            }

            if (!mediumPtr)
            {
               if (!GetInstance(type))
               {
                  // An instance of this medium hasn't yet been encountered. Store it locally
                  // for later provision.
                  mediumInstance->SetIsShared(true);
                  mInstances.emplace_back(std::move(mediumInstance.release()));
               }

               mediumPtr = GetInstance(type);
            }

            if (!mediumPtr)
            {
               throw UtException("Error attempting to utilize medium instance in wsf::comm::medium::Types.");
            }

            // We don't need to worry about the current instance being held by the medium, if any. This
            // is held by a smart pointer, and will be managed appropriately. However, we do
            // want to warn the user if they are replacing a medium already set in the container.
            auto curMediumPtr = aContainer.GetMedium();
            if (curMediumPtr)
            {
               auto out = ut::log::warning() << "Medium is being replaced by adding a medium.";
               out.AddNote() << "Only one medium is allowed at any time.";
               out.AddNote() << "Existing medium type: " << curMediumPtr->GetType();
               out.AddNote() << "Replacing medium type: " << type;
               out.AddNote() << "Location: " << aInput.GetLocation();
            }
            aContainer.SetMedium(mediumPtr);
         }
         else
         {
            throw UtInput::BadValue(aInput, "Unknown medium type: " + type);
         }
      }

      // Check for editing of this instance.
      auto mediumPtr = aContainer.GetMedium();
      if (mediumPtr)
      {
         UtInputBlock inputBlock(aInput, "end_medium");
         bool         displayWarning = (GetInstance(type).use_count() > 3);
         while (inputBlock.ReadCommand())
         {
            if (displayWarning)
            {
               auto out = ut::log::warning()
                          << "Medium is shared, and edits will apply changes across all mediums of this type.";
               out.AddNote() << "Medium type: " << type;
               out.AddNote() << "Location: " << aInput.GetLocation();
               displayWarning = false;
            }
            if (!mediumPtr->ProcessInput(inputBlock.GetInput()))
            {
               throw UtInput::UnknownCommand(inputBlock.GetInput());
            }
         }
         myCommand = true;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown medium type: " + type);
      }
   }

   return myCommand;
}

// =================================================================================================
bool Types::DeleteMedium(UtInput& aInput, Container& aContainer)
{
   // One medium is allowed per object, and there must be a medium.
   // Because of this, when we delete a medium, we replace it with the default type for this object.
   bool myCommand = false;
   if (aInput.GetCommand() == "medium")
   {
      // To delete a medium, a medium must be specified by the container. The matching
      // type should have already been checked by the factory.
      auto mediumPtr = aContainer.GetMedium();

      if (mediumPtr)
      {
         // Since every container should have a medium, replace this medium
         // with the default medium type. This is a core type that is NOT
         // shared and unique to this instance. The user can change this with
         // subsequent "add" commands.
         auto defaultMediumType = aContainer.GetDefaultMedium();
         auto coreTypes         = GetCoreTypes();
         auto it                = std::find_if(std::begin(coreTypes),
                                std::end(coreTypes),
                                [&defaultMediumType](Medium* aCoreTypePtr)
                                { return (aCoreTypePtr->GetMediumIdentifier() == defaultMediumType); });

         if (it != std::end(coreTypes))
         {
            auto coreMedium = ut::clone(*it);
            coreMedium->SetName(UtStringIdLiteral("default"));
            aContainer.SetMedium(std::move(coreMedium));
            myCommand = true;
         }
      }
   }

   return myCommand;
}

// =================================================================================================
std::shared_ptr<Medium> Types::GetInstance(const std::string& aType) const
{
   auto it =
      std::find_if(std::begin(mInstances),
                   std::end(mInstances),
                   [&aType](const std::shared_ptr<Medium>& aMediumPtr) { return (aMediumPtr->GetType() == aType); });

   if (it != std::end(mInstances))
   {
      return (*it);
   }

   return nullptr;
}

} // namespace medium
} // namespace comm
} // namespace wsf
