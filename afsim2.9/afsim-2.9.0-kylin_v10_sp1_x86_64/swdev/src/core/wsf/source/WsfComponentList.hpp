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

#ifndef WSFCOMPONENTLIST_HPP
#define WSFCOMPONENTLIST_HPP

#include "wsf_export.h"

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

#include "WsfComponent.hpp"

// *************************************************************************************************
// PROGRAMMING NOTE:
//
// Most of the methods within WsfComponent and WsfComponentList include the word 'Component' in the
// name to avoid name conflicts if this class is derived from by another class (e.g.: WsfPlatform).
// Also, some classes derive from both WsfComponent and WsfComponentList (i.e. they are a component,
// but they also support sub-components). Thus, there may be no name conflicts between them.
// *************************************************************************************************

//! The non-templated base class for a component list.
//! This is provided so there is less code duplication. Much of the list manipulation can be done
//! in a generic matter.
//
// PROGRAMMING NOTE: This must be exported so these methods are available to all dynamic libraries.
//
// PROGRAMMING NOTE: The base class should not include the AddComponent method. This is a requirement
//                   to ensure type-safety of derived component lists. Otherwise it would be possible
//                   to inject components of the wrong type into a typed component list.
class WSF_EXPORT WsfComponentList
{
public:
   using NameAndComponent = std::pair<WsfStringId, WsfComponent*>;
   using ComponentList    = std::vector<NameAndComponent>;

   WsfComponentList();
   WsfComponentList(const WsfComponentList& aSrc);
   WsfComponentList(WsfComponentList&&) = default;
   virtual ~WsfComponentList();

   WsfComponentList& operator=(const WsfComponentList& aRhs);
   WsfComponentList& operator=(WsfComponentList&&);

   //! Returns true if the component list has at least one component.
   bool HasComponents() const { return (!mComponentsByName.empty()); }

   void DeleteAllComponents();

   bool DeleteComponent(WsfStringId aName, int aRole);

   unsigned int GetComponentCount(int aRole) const;

   //! @name Function templates to make using a component list simpler and more type-safe.
   //! These methods can be used if the template type registers its component role
   //! using the WSF_DECLARE_COMPONENT_ROLE_TYPE macro. It is a compile-time error to instantiate these templates
   //! with a type that has not registered its component role.
   //! The header file for the class must be available whenever these classes are used.
   //@{

   //! Delete a component with the specified name and role implied by the template argument.
   //! Typical usage:
   //! \code
   //!    platformPtr->DeleteComponent<WsfSensor>(name);
   //! \endcode
   //! @param aName The name of the component to be deleted.
   //! @returns true if the component was deleted or false if the component could not be found.
   template<class T>
   bool DeleteComponent(WsfStringId aName)
   {
      return DeleteComponent(aName, cCOMPONENT_ROLE<T>());
   }

   //! Delete a component with the role implied by the template argument.
   //! This is typically used to delete components that are uniquely identified by role.
   //! Typical usage:
   //! \code
   //!    platformPtr->DeleteComponent<WsfComm>();
   //! \endcode
   //! @returns true if the component was deleted or false if the component could not be found.
   //! @note If used for a component class that is not uniquely identified only by role, the
   //! first component with the specified role will be deleted.
   template<class T>
   bool DeleteComponent()
   {
      bool deleted(false);
      T*   componentPtr = static_cast<T*>(FindComponentByRoleP(cCOMPONENT_ROLE<T>()));
      if (componentPtr != nullptr)
      {
         deleted = DeleteComponent(componentPtr->GetComponentName(), cCOMPONENT_ROLE<T>());
      }
      return deleted;
   }

   //! Get a component with the role implied by the template argument
   //! This method can be used to return the component with the role implied by the template argument.
   //! In general this should be used to retrieve a component where it is known that at most one
   //! instance having the specified role exist with the list. If it is used then it will return
   //! the first entry with the given role.
   //! Typical usage:
   //! \code
   //!    WsfSensor* sensorPtr = platformPtr->GetComponent<WsfSensor>();
   //! \endcode
   template<class T>
   T* GetComponent() const
   {
      return static_cast<T*>(FindComponentByRoleP(cCOMPONENT_ROLE<T>()));
   }

   //! Get a component with the specified name and role implied by the template argument
   //! Typical usage:
   //! \code
   //!    WsfSensor* sensorPtr = platformPtr->GetComponent<WsfSensor>(name);
   //! \endcode
   template<class T>
   T* GetComponent(WsfStringId aName) const
   {
      return static_cast<T*>(FindComponentP(aName, cCOMPONENT_ROLE<T>()));
   }

   //! Return the number of components of a particular component role.
   //! Typical usage:
   //! \code
   //!    unsigned int count = platformPtr->GetComponentCount<WsfSensor>();
   //! \endcode
   template<class T>
   unsigned int GetComponentCount() const
   {
      return GetComponentCount(cCOMPONENT_ROLE<T>());
   }

   //! Return the component with the specified index with the list of components of a component role.
   //! Typical usage:
   //! \code
   //!    unsigned int count = platformPtr->GetComponentCount<WsfSensor>();
   //!    for (unsigned int i = 0; i < count; ++i)
   //!    {
   //!       WsfSensor* sensorPtr = platformPtr->GetComponentEntry<WsfSensor>(i);
   //!       ...
   //!    }
   //! \endcode
   template<class T>
   T* GetComponentEntry(unsigned int aEntry) const
   {
      return static_cast<T*>(GetComponentEntryByRoleP(cCOMPONENT_ROLE<T>(), aEntry));
   }
   //@}

   //! Find a component with the specified role.
   //! This should be used only to access components that are uniquely identified by their role.
   //! Typical usage:
   //! \code
   //!    MyType* objectPtr(0);
   //!    if (platformPtr->FindByRole(objectPtr))
   //!    {
   //!       // objectPtr is safe to use
   //! \endcode
   template<class T>
   bool FindByRole(T*& aPtr) const
   {
      for (const auto& component : mComponentsByName)
      {
         if (component.second->QueryInterfaceT(aPtr))
         {
            return true;
         }
      }
      return false;
   }

   template<class T>
   class RoleRange;

   //! An iterator that provides sequential access to components that support a specific role.
   //! Usage:
   //! \code
   //!  for (WsfComponentList<T>::RoleIterator<WsfProcessor> iter(componentList); ! iter.AtEnd(); ++iter)
   //!  {
   //!     WsfProcessor* proc = *iter;    // this or ...
   //!     iter->MethodOnProcessor(...);  // ... this
   //!     ...
   //!  }
   //! \endcode
   template<class T>
   class RoleIterator
   {
   public:
      RoleIterator(const WsfComponentList& aComponentList)
         : RoleIterator(aComponentList.mComponentsByName.begin(), aComponentList.mComponentsByName.end())
      {
      }

      RoleIterator(const RoleIterator&) = default;
      RoleIterator& operator=(const RoleIterator& aRhs) = default;

      //! Return the pointer to the component.
      T* operator*() const
      {
         T* componentPtr(nullptr);
         mIter->second->QueryInterfaceT(componentPtr);
         return componentPtr;
      }

      //! Return the pointer to the component.
      T* operator->() const
      {
         T* componentPtr(nullptr);
         if (!mIter->second->QueryInterfaceT(componentPtr))
         {
            throw std::out_of_range("Dereference of nullptr");
         }
         return componentPtr;
      }

      //! Compares two iterators for equality
      bool operator==(const RoleIterator& aOther) const { return mIter == aOther.mIter; }

      //! Compares two iterators for inequality
      bool operator!=(const RoleIterator& aOther) const { return mIter != aOther.mIter; }

      //! Prefix increment
      RoleIterator& operator++()
      {
         AdvanceP();
         return *this;
      }

      //! Return true if positioned at the end of the list.
      bool AtEnd() const { return mIter == mEndIter; }

   protected:
      friend class RoleRange<T>;

      // Invariant: aIter and aEndIter must be from the same list.
      RoleIterator(ComponentList::const_iterator aIter, ComponentList::const_iterator aEndIter)
         : mIter(aIter)
         , mEndIter(aEndIter)
      {
         mRole = cCOMPONENT_ROLE<T>();

         if ((mIter != mEndIter) && !mIter->second->ComponentHasRole(mRole))
         {
            AdvanceP();
         }
      }

      void AdvanceP()
      {
         ++mIter;
         while (mIter != mEndIter)
         {
            if (mIter->second->ComponentHasRole(mRole))
            {
               break;
            }
            ++mIter;
         }
      }

      int                           mRole;
      ComponentList::const_iterator mIter;
      ComponentList::const_iterator mEndIter;
   };

   //! An adapter class that lets you loop over every component of a given type using a for-each loop.
   //! In C++20 terminology this would be a range.
   //! \tparam T: The type of the component to loop over.
   template<class T>
   class RoleRange
   {
   public:
      RoleRange(const WsfComponentList& aComponentList)
         : mComponentList(&aComponentList)
      {
      }

      RoleIterator<T> begin() { return RoleIterator<T>(*mComponentList); }

      RoleIterator<T> end()
      {
         return RoleIterator<T>(mComponentList->mComponentsByName.end(), mComponentList->mComponentsByName.end());
      }

   private:
      const WsfComponentList* mComponentList;
   };

   //! Yields a range that lets you loop over every component of a given type.
   //! Usage:
   //! \code
   //!  for (auto proc: componentList.GetComponentRange<WsfProcessor>())
   //!  {
   //!     proc->MethodOnProcessor(...);  // ... this
   //!     ...
   //!  }
   //! \endcode
   //! \tparam T: The type of the components to loop over.
   template<typename T>
   RoleRange<T> GetComponentRange()
   {
      return RoleRange<T>(*this);
   }

protected:
   virtual void ComponentAdded(WsfComponent* aComponentPtr);
   virtual void ComponentDeleted(WsfComponent* aComponentPtr);

   bool AddComponentP(WsfComponent* aComponentPtr, bool aDoNotify);

   void CopyAllP(const WsfComponentList& aSrc, bool aDoNotify);

   void DeleteAllP(bool aDoNotify);

   bool DeleteComponentP(WsfStringId aName, int aRole, bool aDoNotify);

   WsfComponent* FindComponentP(WsfStringId aName, int aRole) const;

   WsfComponent* FindComponentP(WsfStringId aName) const;

   WsfComponent* FindComponentByRoleP(int aRole) const;

   WsfComponent* GetComponentEntryByRoleP(int aRole, unsigned int aEntry) const;

   bool RemoveComponentP(WsfStringId aName, int aRole, bool aDelete, bool aDoNotify);

   ComponentList mComponentsByName;
};

//! A component list that stores only components of a specified type.
template<class COMPONENT_TYPE>
class WsfComponentListT : public WsfComponentList
{
public:
   using ComponentType = COMPONENT_TYPE;
   using ParentType    = typename COMPONENT_TYPE::ParentType;

   //! @name Iterator support.
   //! The platform list provides a limited capability to iterate sequential over the list.
   //@{
   //! An iterator that provides sequential access to the components in the list.
   //! This is a 'const_iterator'; the list cannot be changed.
   //!
   //! Usage:
   //! \code
   //! for (WsfComponentList<T>::Iterator i = componentList.Begin(); i != componentList.End(); ++i)
   //! {
   //!    T* tPtr = *i;
   //!    ...
   //! }
   //! \endcode
   class Iterator
   {
   public:
      Iterator(const ComponentList::const_iterator& aSrc)
         : mIterator(aSrc)
      {
      }

      //! Prefix increment.
      Iterator& operator++()
      {
         ++mIterator;
         return *this;
      }

      //! Test for operator inequality
      bool operator!=(const Iterator& aRhs) const { return mIterator != aRhs.mIterator; }

      //! Return the pointer to the component.
      ComponentType* operator*() const { return static_cast<ComponentType*>(mIterator->second); }

      //! Return the pointer to the component.
      ComponentType* operator->() const { return static_cast<ComponentType*>(mIterator->second); }

   private:
      ComponentList::const_iterator mIterator;
   };
   //@}

   //! Return an iterator for the beginning of the component list.
   Iterator Begin() const { return Iterator(mComponentsByName.begin()); }

   //! Return an iterator for the end of the component list.
   Iterator End() const { return Iterator(mComponentsByName.end()); }

   WsfComponentListT()
      : WsfComponentList()
      , mParentOfComponentsPtr(nullptr)
   {
   }

   // ===========================================================================================
   // PROGRAMMING NOTE: Do NOT be tempted to put in copy constructor that includes the new parent.
   // In most cases the initializer for this list would be something like: "mComponents(aSrc, this)".
   // and ANYTHING that refers to 'this' (directly or indirectly) is VERY dangerous.
   // ===========================================================================================

   WsfComponentListT(const WsfComponentListT& aSrc)
      : WsfComponentList(aSrc)
      , mParentOfComponentsPtr(nullptr)
   {
   }

   WsfComponentListT(WsfComponentListT&&) = default;

   ~WsfComponentListT() override {}

   WsfComponentListT& operator=(const WsfComponentListT& aRhs)
   {
      if (this != &aRhs)
      {
         DeleteAllP(true);
         CopyAllP(aRhs, false);
         SetParentOfComponents(mParentOfComponentsPtr);
      }
      return *this;
   }

   WsfComponentListT& operator=(WsfComponentListT&& aRhs)
   {
      if (this != &aRhs)
      {
         this->WsfComponentList::operator=(std::move(aRhs));
         SetParentOfComponents(mParentOfComponentsPtr);
      }
      return *this;
   }

   //! Retrieve the pointer to the parent/owner of the components owned by this list.
   //! @returns The pointer to the parent. This can be the null pointer.
   ParentType* GetParentOfComponents() const { return mParentOfComponentsPtr; }

   //! Set the pointer to the parent/owner of the components owned by this list.
   //!
   //! The only explicit call should be in the body of the copy constructor of the object that
   //! owns the list. It CANNOT be done in the initializer list of the copy constructor because
   //! the owning object is in an indeterminate state. The methods SetComponentParent and
   //! ComponentListUpdated are not guaranteed to be safe, so they must be deferred to the body
   //! of the copy constructor.
   void SetParentOfComponents(ParentType* aComponentParentPtr)
   {
      mParentOfComponentsPtr = aComponentParentPtr;
      for (const auto& component : mComponentsByName)
      {
         ComponentType* componentPtr(static_cast<ComponentType*>(component.second));
         componentPtr->SetComponentParent(mParentOfComponentsPtr);
         // The component isn't actually being added to the list here, but it is logically being
         // added to list because this has been deferred to from the copy construction.
         ComponentAdded(componentPtr);
      }
   }

   bool AddComponent(ComponentType* aComponentPtr)
   {
      bool added = AddComponentP(aComponentPtr, true);
      if (added)
      {
         aComponentPtr->SetComponentParent(mParentOfComponentsPtr);
      }
      return added;
   }

   ComponentType* FindComponent(WsfStringId aName) const { return static_cast<ComponentType*>(FindComponentP(aName)); }

   ComponentType* FindComponent(WsfStringId aName, int aRole) const
   {
      return static_cast<ComponentType*>(FindComponentP(aName, aRole));
   }

   ComponentType* FindComponentByRole(int aRole) const
   {
      return static_cast<ComponentType*>(FindComponentByRoleP(aRole));
   }

   ComponentType* GetComponentEntryByRole(int aRole, unsigned int aEntry) const
   {
      return static_cast<ComponentType*>(GetComponentEntryByRoleP(aRole, aEntry));
   }

   //! Return a vector of components in the order in which they should be initialized.
   std::vector<ComponentType*> GetComponentsByInitializationOrder() const
   {
      std::vector<ComponentType*> components;
      for (const auto& component : mComponentsByName)
      {
         components.push_back(static_cast<ComponentType*>(component.second));
      }

      auto compare = [](const WsfComponent* aLhs, const WsfComponent* aRhs)
      { return aLhs->GetComponentInitializationOrder() < aRhs->GetComponentInitializationOrder(); };
      std::stable_sort(components.begin(), components.end(), compare);
      return components;
   }

   bool RemoveComponent(ComponentType* aComponentPtr)
   {
      bool removed =
         RemoveComponentP(aComponentPtr->GetComponentName(), aComponentPtr->GetComponentRoles()[0], false, true);
      if (removed)
      {
         aComponentPtr->SetComponentParent(nullptr);
      }
      return removed;
   }

   bool ProcessComponentInput(UtInput& aInput)
   {
      bool myCommand{false};
      for (ComponentType* componentPtr : *this)
      {
         if (componentPtr->ProcessInput(aInput))
         {
            myCommand = true;
            break;
         }
      }
      return myCommand;
   }

private:
   //! Points to the object that is the parent of the components pointed to by this list.
   ParentType* mParentOfComponentsPtr;
};

//! A convenience definition for the frequently used component list containing platform components.
using WsfPlatformComponentList = WsfComponentListT<WsfPlatformComponent>;

template<class T>
typename WsfComponentListT<T>::Iterator begin(const WsfComponentListT<T>& aList)
{
   return aList.Begin();
}

template<class T>
typename WsfComponentListT<T>::Iterator end(const WsfComponentListT<T>& aList)
{
   return aList.End();
}

template<class T>
typename WsfComponentListT<T>::Iterator begin(WsfComponentListT<T>& aList)
{
   return aList.Begin();
}

template<class T>
typename WsfComponentT<T>::Iterator end(WsfComponentListT<T>& aList)
{
   return aList.End();
}

#endif
