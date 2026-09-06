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

#ifndef WSFCOMMMEDIUMFACTORY_HPP
#define WSFCOMMMEDIUMFACTORY_HPP

#include <memory>
#include <unordered_map>

#include "WsfCommMediumContainer.hpp"
#include "WsfCommMediumTypeIdentifier.hpp"
#include "WsfScenario.hpp"

namespace wsf
{
namespace comm
{
namespace medium
{

//! The base factory interface for medium usage.
//! Provides basic required interface for medium factories.
//! Users must use the derived template Factory class for
//! defining a factory for medium implementations.
class FactoryInterface
{
   //! Required for SetScenario()
   friend class FactoryManager;

public:
   using FactoryMap = std::unordered_map<int, std::unique_ptr<FactoryInterface>>;

   FactoryInterface(int aMediumIdentifier)
      : mMediumIdentifier(aMediumIdentifier)
   {
   }
   virtual ~FactoryInterface() = default;

   int GetMediumIdentifier() const { return mMediumIdentifier; }

   virtual void PreInput(Container& aParent) {}

   virtual bool ProcessInput(UtInput& aInput, Container& aParent) { return false; }

   virtual bool ProcessAddOrEditCommand(UtInput& aInput, Container& aParent, bool aIsAdding) { return false; }

   virtual bool ProcessDeleteCommand(UtInput& aInput, Container& aParent) { return false; }

   virtual bool PreInitialize(double aSimTime, Container& aParent) { return true; }

protected:
   WsfScenario& GetScenario() const { return *mScenarioPtr; }

private:
   void SetScenario(WsfScenario& aScenario) { mScenarioPtr = &aScenario; }

   WsfScenario* mScenarioPtr{nullptr};
   int          mMediumIdentifier{0};
};

//! The medium factory class. Automatically fills medium type based on
//! role assignment. Ensures medium factories are derived from medium types.
template<typename MEDIUM_TYPE>
class Factory : public FactoryInterface
{
public:
   static_assert(std::is_base_of<Medium, MEDIUM_TYPE>::value,
                 "FactoryList must be a template of a derived class of wsf::comm::medium::Medium");
   Factory()
      : FactoryInterface(cMEDIUM_ROLE<MEDIUM_TYPE>())
   {
   }
   virtual ~Factory() override = default;
};

//! The factory manager class, which handles the registration of medium factories, and the
//! correct processing of input for such classes based on the type of factory, and the
//! valid types available for the parent class medium container. Since not all mediums
//! may be valid for all object types, this restricts the types of mediums parsed for each
//! parent container.
//!
//! Since this class acts fundamentally different from typical components in runtime instantiation
//! and input parsing, this object is held by the medium typelist itself. Users are required to
//! query the medium typelist for this functionality.
class FactoryManager
{
public:
   using FactoryMap = std::unordered_map<int, std::unique_ptr<FactoryInterface>>;

   FactoryManager(WsfScenario& aScenario)
      : mScenarioPtr(&aScenario)
   {
   }
   virtual ~FactoryManager()                  = default;
   FactoryManager(const FactoryManager& aSrc) = delete;
   FactoryManager(FactoryManager&& aSrc)      = delete;

   //! Template function requires provision of derived type to ensure correct functionality/assignments.
   template<typename MEDIUM_TYPE>
   void AddFactory(std::unique_ptr<Factory<MEDIUM_TYPE>> aFactory)
   {
      auto it = mFactoryMap.find(cMEDIUM_ROLE<MEDIUM_TYPE>());
      if (it != std::end(mFactoryMap))
      {
         throw UtException("Attempting to add a factory to a medium type that is already registered.");
      }

      aFactory->SetScenario(*mScenarioPtr);
      mFactoryMap[cMEDIUM_ROLE<MEDIUM_TYPE>()] = std::move(aFactory);
   }

   bool ProcessInput(UtInput& aInput, Container& aParent) const
   {
      const auto& validMediums = aParent.GetValidMediums();
      for (auto mediumIdentifier : validMediums)
      {
         auto it = mFactoryMap.find(mediumIdentifier);
         if (it != std::end(mFactoryMap))
         {
            if (it->second->ProcessInput(aInput, aParent))
            {
               return true;
            }
         }
      }
      return false;
   }

   bool ProcessAddOrEditCommand(UtInput& aInput, Container& aParent, bool aIsAdding) const
   {
      const auto& validMediums = aParent.GetValidMediums();
      for (auto mediumIdentifier : validMediums)
      {
         auto it = mFactoryMap.find(mediumIdentifier);
         if (it != std::end(mFactoryMap))
         {
            if (it->second->ProcessAddOrEditCommand(aInput, aParent, aIsAdding))
            {
               return true;
            }
         }
      }
      return false;
   }

   bool ProcessDeleteCommand(UtInput& aInput, Container& aParent) const
   {
      auto command = aInput.GetCommand();
      if (command == "medium")
      {
         std::string type;
         aInput.ReadValue(type);
         auto mediumPtr = aParent.GetMedium();

         if (mediumPtr && (mediumPtr->GetType() == type))
         {
            auto mediumIdentifier = mediumPtr->GetMediumIdentifier();
            auto it               = mFactoryMap.find(mediumIdentifier);
            if (it != std::end(mFactoryMap))
            {
               return it->second->ProcessDeleteCommand(aInput, aParent);
            }
         }

         throw UtInput::BadValue(aInput, "Unknown medium type to delete: " + type);
      }
      return false;
   }

private:
   WsfScenario* mScenarioPtr{nullptr};
   FactoryMap   mFactoryMap{};
};

} // namespace medium
} // namespace comm
} // namespace wsf

#endif
