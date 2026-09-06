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

#ifndef WSFSIMPLECOMPONENT_HPP
#define WSFSIMPLECOMPONENT_HPP

#include "wsf_export.h"

#include "WsfComponent.hpp"
#include "WsfNamed.hpp"

//! A class template for a simple, single-role component that is not a WsfObject.
//! Deriving from this class provides two of the four abstract methods required by WsfComponent.
//! The derived class need only provide CloneComponent and QueryInterface, and if needed, ComponentParentChange (if the
//! component requires notification of being attached to a component list.)
//!
//! Typical uses are for things like 'extension' components (e.g.: WsfDisExtension and WsfDisPlatformComponent,
//! WsfExtInterfaceComponent, WsfTaskManagerComponent) and simple components that don't utilize type-lists
//! (e.g. WsfCommandChain, WsfGeoPoint and WsfTaskManager).
//!
//! This class derives from WsfNamed, which provides the name attribute and the methods GetName(Id)/SetName.
//! This makes it mutually exclusive with things that require WsfObject.
template<typename COMPONENT_TYPE>
class WsfSimpleComponentT : public COMPONENT_TYPE, public WsfNamed
{
   static_assert(std::is_base_of<WsfComponent, COMPONENT_TYPE>::value,
                 "Template argument to WsfSimpleComponentT must be a subclass of WsfComponent");

   using ComponentType = COMPONENT_TYPE;

public:
   //! Empty constructor for serialization.
   WsfSimpleComponentT() = default;

   //! Construct the a role and a name.
   //! @param aRole The role of the component.
   //! @param aName The name of the component.
   WsfSimpleComponentT(int aRole, WsfStringId aName)
      : ComponentType()
      , WsfNamed(aName)
   {
      mRoles[0] = aRole;
      mRoles[1] = 0;
   }

   //! Construct with only a role.
   //! @param aRole The role of the component.
   //! With this form all components in a list must have a unique role.
   // Using 'explicit' prevents casting a WsfStringId to a role.
   explicit WsfSimpleComponentT(int aRole)
      : ComponentType()
      , WsfNamed()
   {
      mRoles[0] = aRole;
      mRoles[1] = 0;
   }

   //! Construct with only a name.
   //! With this form all components in a list must have a unique name.
   WsfSimpleComponentT(WsfStringId aName)
      : ComponentType()
      , WsfNamed(aName)
   {
      mRoles[0] = 0;
      mRoles[1] = 0;
   }

   WsfSimpleComponentT(const WsfSimpleComponentT& aSrc)
      : ComponentType(aSrc)
      , WsfNamed(aSrc)
   {
      mRoles[0] = aSrc.mRoles[0];
      mRoles[1] = aSrc.mRoles[1];
   }

   WsfSimpleComponentT& operator=(const WsfSimpleComponentT& aRhs)
   {
      if (this != &aRhs)
      {
         ComponentType::operator=(aRhs);
         WsfNamed::     operator=(aRhs);
         mRoles[0]              = aRhs.mRoles[0];
         mRoles[1]              = aRhs.mRoles[1];
      }
      return *this;
   }

   ~WsfSimpleComponentT() override = default;

   // *******************************************************************************************
   // PROGRAMMING NOTE: QueryInterface cannot be provided by this class as this will return the
   // wrong pointer if derived class utilizes multiple inheritance and this class is not derived
   // first! For example: The following would NOT work if this class provided QueryInterface:
   //
   // class MyComponent : public OtherBase, public WsfSimpleComponent<Z>
   //
   // The problem is that QueryInterface would return the pointer to the WsfSimpleComponentT<Z>
   // and NOT MyComponent. The addresses are separated by the size of OtherBase!
   // *******************************************************************************************

   //! @name Implementation of those routines that can be provided by this class.
   //@{
   WsfStringId GetComponentName() const override { return GetNameId(); }
   const int*  GetComponentRoles() const override { return mRoles; }
   //@}

protected:
   int mRoles[2];
};

//! A convenient alias template for simple components specified by parent type, when no specialized component
//! is defined or typedef'd
template<class PARENT_TYPE>
using WsfSimpleComponentParentT = WsfSimpleComponentT<WsfComponentT<PARENT_TYPE>>;

//! Explicitly instantiate and export template for simple platform component, and provide convenient alias
template class WSF_EXPORT WsfSimpleComponentT<WsfPlatformComponent>;
using WsfSimplePlatformComponent = WsfSimpleComponentT<WsfPlatformComponent>;

#endif
