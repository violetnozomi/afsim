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

#ifndef WSFCOMMMEDIUMCONTAINER_HPP
#define WSFCOMMMEDIUMCONTAINER_HPP

#include "wsf_export.h"

#include <memory>
#include <set>

#include "UtInput.hpp"
#include "WsfCommMedium.hpp"
#include "WsfCommMediumTypeIdentifier.hpp"
#include "WsfComponent.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfComponentRoles.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace comm
{
namespace medium
{

//! A class container for usage of medium objects.
//! Provides the API for usage of the medium object, as well as some
//! convenience methods for using mediums.
//! Mediums are maintained by the type list, and shared pointer instances
//! are provided to containers - thus, the medium held by any given
//! container may be shared between multiple containers.
class WSF_EXPORT Container
{
public:
   enum InputAction : int
   {
      cNONE,
      cADD,
      cEDIT,
      cDELETE
   };

   Container(const WsfScenario& aScenario)
      : mScenario(aScenario)
   {
   }
   Container(const Container& aSrc);
   virtual ~Container() = default;

   Medium* GetMedium() const { return mMedium.get(); }
   void    SetMedium(std::shared_ptr<Medium> aMedium) { mMedium = aMedium; }

   //! In the base container implementation, ProcessInput primarily provides
   //! support for legacy input that was formerly found in physical layer
   //! implementations. This allows for legacy input to still be supported,
   //! while handling it properly using the medium objects without replicating
   //! the functionality.
   virtual bool ProcessInput(UtInput& aInput);

   virtual bool Initialize(double aSimTime);

   //! Required simulation object for subsequent provision to medium instances.
   //! Mediums require a simulation instance for runtime query and modification.
   void SetSimulation(WsfSimulation* aSimPtr) { mSimPtr = aSimPtr; }

   //! A method for processing medium input for the container parent.
   //! An additional parameter is provided to qualify the scope of
   //! the action being taken common to all mediums and akin to
   //! components, being adding, editing, or deleting.
   virtual bool ProcessContainerInput(UtInput& aInput, InputAction aAction = cNONE);

   const std::set<TypeIdentifier>& GetValidMediums() const { return mValidMediums; }

   //! Add a valid medium type for use by the parent of this container.
   //! This is only successful if a medium type has not been forbidden,
   //! either by the parent of the container itself, or one of its derivations.
   void AddValidMedium(TypeIdentifier aValue);

   //! Adds a forbidden medium type for the parent of this container.
   //! This allows the removal of types that may not be valid for this
   //! particular instance.
   void AddForbiddenMedium(TypeIdentifier aValue);

   //! Returns true if the provided TypeIdentifier is valid for this container.
   bool IsValidMedium(TypeIdentifier aValue) const;

   //! Sets/gets the default medium for the parent object. Medium usage does not
   //! require explicit input for backward compatibility, so a default medium
   //! is required.
   //!
   //! The set function is ignored if the TypeIdentifier provided is
   //! a forbidden medium.
   //@{
   void           SetDefaultMedium(TypeIdentifier aValue);
   TypeIdentifier GetDefaultMedium() const { return mDefaultMedium; }
   //@}

   //! A public helper method, indicating to the container that it should
   //! use the default medium type, and acquire a copy if it.
   //! This is only valid if a medium is not currently assigned.
   virtual bool AddDefaultMedium();

   const WsfScenario& GetScenario() const { return mScenario; }

private:
   const WsfScenario&       mScenario;
   WsfSimulation*           mSimPtr{nullptr};
   std::shared_ptr<Medium>  mMedium{nullptr};
   std::set<TypeIdentifier> mValidMediums{};
   std::set<TypeIdentifier> mForbiddenMediums{};
   TypeIdentifier           mDefaultMedium{TypeIdentifier::cNONE};
};

// Forward declare
template<typename PARENT>
class ContainerComponent;

//! A template class for common medium component factory registration.
template<typename PARENT>
class ContainerFactory : public WsfComponentFactory<PARENT>
{
public:
   bool ProcessAddOrEditCommand(UtInput& aInput, PARENT& aParent, bool aIsAdding) override
   {
      // This is a wrapper for medium usage. Redirect input back to the container for proper processing.
      auto containerPtr = wsf::comm::medium::ContainerComponent<PARENT>::Find(aParent);
      if (containerPtr)
      {
         wsf::comm::medium::Container::InputAction action;
         if (aIsAdding)
         {
            action = wsf::comm::medium::Container::cADD;
         }
         else
         {
            action = wsf::comm::medium::Container::cEDIT;
         }

         return containerPtr->ProcessContainerInput(aInput, action);
      }

      return false;
   }

   bool ProcessDeleteCommand(UtInput& aInput, PARENT& aParent) override
   {
      // This is a wrapper for medium usage. Redirect input back to the container for proper processing.
      auto containerPtr = wsf::comm::medium::ContainerComponent<PARENT>::Find(aParent);
      if (containerPtr)
      {
         auto action = wsf::comm::medium::Container::cDELETE;
         return containerPtr->ProcessContainerInput(aInput, action);
      }

      return false;
   }

   bool PreInitialize(double aSimTime, PARENT& aParent) override
   {
      // At the time PreInitialize is called, all input has been parsed.
      // If user input has not resulted in a viable medium added to the container,
      // a default instance is required.
      auto containerPtr = wsf::comm::medium::ContainerComponent<PARENT>::Find(aParent);
      if (containerPtr && !containerPtr->GetMedium())
      {
         return containerPtr->AddDefaultMedium();
      }

      return true;
   }
};

WSF_EXPORT WsfStringId GetContainerComponentName();

//! A templated component type for usage of medium Containers.
//! Use of this template container type allows containers to be attached
//! to parents that maintain a component list, without having to derive
//! or instantiate a container object directly. The container, while a
//! component itself, manages the medium object usage on the parent. The
//! medium itself is NOT a component due to its potentially shared lifetime and
//! incompatibilities due to assumptions in the implementation of components.
//! As such, this container utilizes the traditional factory methods
//! typically utilized by components and their default factories, but most
//! of the usage is a wrapper for the functionality and handling of the
//! medium objects themselves (which is passed to a separate factory
//! for the handling of medium input parsing and other necessary method calls
//! directly related to medium usage).
//!
//! Note that any parent class using this template component container must
//! still explicitly define the specialization for WSF_DECLARE_COMPONENT_ROLE_TYPE.
//! A template factory is provided for component containers.
template<typename PARENT>
class ContainerComponent : public Container, public WsfComponentT<PARENT>
{
public:
   ContainerComponent(const WsfScenario& aScenario)
      : Container(aScenario)
   {
   }

   //! Both WsfComponent and Container contain virtual Initialize methods with the same
   //! signature. This resolves the ambiguity.
   bool Initialize(double aSimTime) override { return Container::Initialize(aSimTime); }

   //! Both WsfComponent and Container contain virtual ProcessInput methods with the same
   //! signature. This resolves the ambiguity.
   bool ProcessInput(UtInput& aInput) override { return Container::ProcessInput(aInput); }

   ~ContainerComponent() override = default;

   //! Static Find and FindOrCreate Methods
   //! Convenience methods for the addition or query for existence of the container within the
   //! parent's component list.
   //@{
   static ContainerComponent<PARENT>* Find(const PARENT& aParent)
   {
      const auto& components   = aParent.GetComponents();
      auto        containerPtr = components.template GetComponent<ContainerComponent<PARENT>>();

      return containerPtr;
   }

   static ContainerComponent<PARENT>* FindOrCreate(PARENT& aParent, const WsfScenario& aScenario)
   {
      auto containerPtr = Find(aParent);
      if (!containerPtr)
      {
         auto  clonedContainer = ut::make_unique<ContainerComponent<PARENT>>(aScenario);
         auto& components      = aParent.GetComponents();
         if (components.AddComponent(clonedContainer.get()))
         {
            containerPtr = clonedContainer.release();
         }
      }
      return containerPtr;
   }
   //@}

   //! Required method implementations for components
   //@{
   WsfComponent* CloneComponent() const override { return new ContainerComponent<PARENT>(*this); }
   WsfStringId   GetComponentName() const override { return GetContainerComponentName(); }
   const int*    GetComponentRoles() const override
   {
      static const int roles[] = {cCOMPONENT_ROLE<ContainerComponent<PARENT>>(), cWSF_COMPONENT_NULL};

      return roles;
   }

   void* QueryInterface(int aRole) override
   {
      if (aRole == cCOMPONENT_ROLE<ContainerComponent<PARENT>>())
      {
         return this;
      }

      return nullptr;
   }
   //@}
};

} // namespace medium
} // namespace comm
} // namespace wsf

#endif
