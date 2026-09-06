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

#include "WsfComponentList.hpp"

// ================================================================================================
WsfComponentList::WsfComponentList() {}

// ================================================================================================
WsfComponentList::WsfComponentList(const WsfComponentList& aSrc)
{
   CopyAllP(aSrc, false);
}

// ================================================================================================
WsfComponentList::~WsfComponentList()
{
   DeleteAllP(false);
}

// ================================================================================================
WsfComponentList& WsfComponentList::operator=(const WsfComponentList& aRhs)
{
   if (this != &aRhs)
   {
      DeleteAllP(true);
      CopyAllP(aRhs, true);
   }
   return *this;
}

// ================================================================================================
WsfComponentList& WsfComponentList::operator=(WsfComponentList&& aRhs)
{
   if (this != &aRhs)
   {
      DeleteAllP(true);
      mComponentsByName = std::move(aRhs.mComponentsByName);
   }
   return *this;
}
// ================================================================================================
//! Delete all of the components maintained by the list
void WsfComponentList::DeleteAllComponents()
{
   DeleteAllP(true);
}

// ================================================================================================
//! Delete a single component with the specified name and role from the list.
//! @param aName The name of the component to be deleted.
//! @param aRole The primary role of the component to be deleted.
//! @returns true if successful or false if the component could not be found.
bool WsfComponentList::DeleteComponent(WsfStringId aName, int aRole)
{
   return DeleteComponentP(aName, aRole, true);
}

// ================================================================================================
//! Get the count of components with the specified role.
//! @param aRole The role whose component count is being requested.
//! @returns The count of components who have the specified role.
unsigned int WsfComponentList::GetComponentCount(int aRole) const
{
   unsigned int count = 0;
   for (size_t i = 0; i < mComponentsByName.size(); ++i)
   {
      if (mComponentsByName[i].second->ComponentHasRole(aRole))
      {
         ++count;
      }
   }
   return count;
}

// ================================================================================================
//! A virtual method that allows a derived class to know when a entry is being added to the list.
//! @note This method is not invoked during copy construction because the inheriting class is in an
//! indeterminate state.
// protected virtual
void WsfComponentList::ComponentAdded(WsfComponent* aComponentPtr) {}

// ================================================================================================
//! A virtual method that allows a derived class to know when a entry is being delete from the list.
//! @note This method is not invoked during destruction because the inheriting class is in an
//! indeterminate state.
// protected virtual
void WsfComponentList::ComponentDeleted(WsfComponent* aComponentPtr) {}

// ================================================================================================
//! Base class implementation of copying a component list.
// protected
void WsfComponentList::CopyAllP(const WsfComponentList& aSrc, bool aDoNotify)
{
   for (ComponentList::const_iterator i = aSrc.mComponentsByName.begin(); i != aSrc.mComponentsByName.end(); ++i)
   {
      WsfComponent* clonedComponentPtr = i->second->CloneComponent();
      // Components that do not support clone return 'null'
      if (clonedComponentPtr != nullptr)
      {
         AddComponentP(clonedComponentPtr, aDoNotify);
      }
   }
}

// ================================================================================================
//! Base class implementation of deleting all components in the list.
// protected
void WsfComponentList::DeleteAllP(bool aDoNotify)
{
   // The components are removed from the list and deleted in reverse order. The old code here used
   // to simply iterate over the list front-to-back and delete the components, but it didn't remove
   // the list entries as it went. A component destructor could invoke a method
   // (such as FindByRole, which calls QueryInterfaceT) that accesses the list and refers to an
   // already-deleted component
   while (!mComponentsByName.empty())
   {
      WsfComponent* componentPtr = mComponentsByName.back().second;
      mComponentsByName.pop_back();
      if (aDoNotify)
      {
         ComponentDeleted(componentPtr);
      }
      delete componentPtr;
   }
}

// ================================================================================================
//! Add a component to the component list.
//! Attempts to add a component to the list of components. The names of components must be unique
//! within their role.
//! @param aComponentPtr The component to be added.
//! @param aDoNotify     If true, invoke the ComponentAdded() virtual method to inform the derived
//! class that the component was added.
//! @returns true if successful, false if a component of the same name and primary role already
//! exists. If true then this class assumes ownership of the component. If false the caller retains
//! ownership and is responsible for deleting the object.
// protected
bool WsfComponentList::AddComponentP(WsfComponent* aComponentPtr, bool aDoNotify)
{
   if (aComponentPtr == nullptr)
   {
      return false;
   }

   WsfStringId name(aComponentPtr->GetComponentName());
   int         role(aComponentPtr->GetComponentRoles()[0]); // The 'primary' role
   for (size_t i = 0; i < mComponentsByName.size(); ++i)
   {
      if (mComponentsByName[i].first == name)
      {
         WsfComponent* componentPtr(mComponentsByName[i].second);
         if (componentPtr->GetComponentRoles()[0] == role)
         {
            return false; // Error, name and role matches
         }
      }
   }
   mComponentsByName.push_back(NameAndComponent(name, aComponentPtr));
   if (aDoNotify)
   {
      ComponentAdded(aComponentPtr); // Inform the derived class
   }
   return true;
}

// ================================================================================================
// protected
bool WsfComponentList::DeleteComponentP(WsfStringId aName, int aRole, bool aDoNotify)
{
   return RemoveComponentP(aName, aRole, true, aDoNotify);
}

// ================================================================================================
// protected
WsfComponent* WsfComponentList::FindComponentP(WsfStringId aName) const
{
   for (size_t i = 0; i < mComponentsByName.size(); ++i)
   {
      if (mComponentsByName[i].first == aName)
      {
         return mComponentsByName[i].second;
      }
   }
   return nullptr;
}

// ================================================================================================
//! Find the first component with the specified name and role.
//! Remember that components are guaranteed to have unique names only within a primary role.
//! If a non-primary role is specified then multiple components may have the same name and only
//! the first one will be located.
//! @param aName The name of the desired component.
//! @param aRole The role of the desired component.
//! @returns A pointer to the component if found. 0 if the requested component does not exist.
WsfComponent* WsfComponentList::FindComponentP(WsfStringId aName, int aRole) const
{
   for (size_t i = 0; i < mComponentsByName.size(); ++i)
   {
      if (mComponentsByName[i].first == aName)
      {
         WsfComponent* componentPtr(mComponentsByName[i].second);
         if (componentPtr->ComponentHasRole(aRole))
         {
            return componentPtr;
         }
      }
   }
   return nullptr;
}

// ================================================================================================
// protected
WsfComponent* WsfComponentList::FindComponentByRoleP(int aRole) const
{
   for (size_t i = 0; i < mComponentsByName.size(); ++i)
   {
      if (mComponentsByName[i].second->ComponentHasRole(aRole))
      {
         return mComponentsByName[i].second;
      }
   }
   return nullptr;
}

// ================================================================================================
WsfComponent* WsfComponentList::GetComponentEntryByRoleP(int aRole, unsigned int aEntry) const
{
   unsigned int count = 0;
   for (size_t i = 0; i < mComponentsByName.size(); ++i)
   {
      if (mComponentsByName[i].second->ComponentHasRole(aRole))
      {
         if (count == aEntry)
         {
            return mComponentsByName[i].second;
         }
         ++count;
      }
   }
   return nullptr;
}

// ================================================================================================
bool WsfComponentList::RemoveComponentP(WsfStringId aName, int aRole, bool aDelete, bool aDoNotify)
{
   for (size_t i = 0; i < mComponentsByName.size(); ++i)
   {
      if (mComponentsByName[i].first == aName)
      {
         WsfComponent* componentPtr(mComponentsByName[i].second);
         if (componentPtr->ComponentHasRole(aRole))
         {
            mComponentsByName.erase(mComponentsByName.begin() + i);
            if (aDoNotify)
            {
               ComponentDeleted(componentPtr); // Inform the derived class
            }
            if (aDelete)
            {
               delete componentPtr;
            }
            return true;
         }
      }
   }
   return false;
}
