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

#ifndef WSFCOMPONENTFACTORY_HPP
#define WSFCOMPONENTFACTORY_HPP

#include <string>

class UtInput;
#include "wsf_export.h"

#include "WsfComponentRoles.hpp"
class WsfScenario;

//! A factory for creating components on an object with a component list..
class WSF_EXPORT WsfComponentFactoryBase
{
   friend class WsfScenario;

public:
   WsfComponentFactoryBase(int aParentRole);
   virtual ~WsfComponentFactoryBase();

   //! Return the parent role of the factory.
   int GetParentRole() const { return mParentRole; }

protected:
   //! Returns a reference to the scenario to which this factory is registered.
   WsfScenario& GetScenario() const { return *mScenarioPtr; }

private:
   //! Called from WsfScenario when the factory is registered.
   void SetScenario(WsfScenario* aScenarioPtr) { mScenarioPtr = aScenarioPtr; }

   WsfScenario* mScenarioPtr;

   //! The role of the parent for which this factory creates components.
   //! For example, processor objects reside on a platform. In that case the parent role of the processor
   //! factory is the platform.
   int mParentRole;
};

//! A template that defines a component factory for a given parent class.
template<typename PARENT_TYPE>
class WsfComponentFactory : public WsfComponentFactoryBase
{
public:
   WsfComponentFactory()
      : WsfComponentFactoryBase(cCOMPONENT_ROLE<PARENT_TYPE>())
   {
   }

   //! Pre-input processing.
   //! @see WsfComponent::PreInput for what this does.
   //! @note Factories must be aware this method could be called more than once for a given parent object.
   virtual void PreInput(PARENT_TYPE& aParent) {}

   //! @name Input processing.
   //! Component factories will override one or more methods depending on their implementation.
   //@{
   //! Process input that may pertain to this component factory.
   //! @param aInput  The input stream.
   //! @param aParent The parent object.
   //! @returns true if the command was recognized and processed, or false if not (pass to the next factory).
   virtual bool ProcessInput(UtInput& aInput, PARENT_TYPE& aParent) { return false; }

   //! Process the implicit or explicit 'add' or 'edit' command.
   //! This is typically used by the component factories whose parent is WsfPlatform.
   //! Others may use this if they follow similar syntax.
   //! @param aInput  The input stream.
   //! @param aParent The parent object.
   //! @param aIsAdding 'true' if the component is being added to the parent or 'false' if an
   //! existing component is to be edited.
   //! @returns true if the command was recognized and processed, or false if not (pass to the next factory).
   virtual bool ProcessAddOrEditCommand(UtInput& aInput, PARENT_TYPE& aParent, bool aIsAdding) { return false; }

   //! Process the explicit component 'delete' command.
   //! This is typically used by component factories whose parent is WsfPlatform.
   //! @param aInput  The input stream.
   //! @param aParent The parent object.
   //! @returns true if the command was recognized and processed, or false if not (pass to the next factory).
   virtual bool ProcessDeleteCommand(UtInput& aInput, PARENT_TYPE& aParent) { return false; }
   //@}

   //! @name Parent Pre-Initialization.
   //@{
   //! Perform pre-initialization function.
   //! This is called by the parents Initialize prior to actually initializing the components on the
   //! parent (i.e.: calling Initialize and Initialize2 for each component). The primary reason for
   //! this method is to allow the factory to check for the existence of a required component and either
   //! add it or cause a failure. The method may examine the component list on the parent but it
   //! must not assume ANYTHING about the actual state of the components.
   //! @param aSimTime The current simulation time.
   //! @param aParent The parent object being initialized.
   //! @returns true if successful or false if unsuccessful (bad input data, etc.)
   virtual bool PreInitialize(double aSimTime, PARENT_TYPE& aParent) { return true; }
   //@}
};

#endif
