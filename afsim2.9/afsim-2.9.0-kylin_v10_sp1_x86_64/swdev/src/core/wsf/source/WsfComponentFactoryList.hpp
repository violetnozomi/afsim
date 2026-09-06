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

#ifndef WSFCOMPONENTFACTORYLIST_HPP
#define WSFCOMPONENTFACTORYLIST_HPP

#include <memory>

#include "UtLog.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfScenario.hpp"

//! A templated class to simplify the use of component factories.
//!
//! The component factory list can contain factories that apply to different parent classes. It always
//! contains factories for those components that are part of WsfPlatform, but it may also include factories
//! for other types of parent classes, e.g. WsfProcessor. This class contains static methods which parallel
//! one-for-one the virtual methods in WsfComponentFactory. The static methods iterate over the component
//! factory list and invoke only the factories that pertain to the class of the invoker.
//!
//! For instance, to invoke the PreInitialize method of the factories that pertain only to WsfPlatform,
//! WsfPlatform might do the following:
//!
//! \code
//!    bool ok = WsfComponentFactoryList<WsfPlatform>::PreInitialize(GetScenario(), aSimTime, *this);
//! \endcode
//!
//! While WsfProcessor would do the following:
//!
//! \code
//!    bool ok = WsfComponentFactoryList<WsfProcessor>::PreInitialize(GetScenario(), aSimTime, *this);
//! \endcode
//!
//! The static methods will iterate over the factory list and invoke ONLY those factories that pertain
//! to the invoker.
//!
//! NOTE: This is NOT meant to be included in another header file. It is pretty heavy and requires
//! the user to include the following header files.
//!
//! - \<iostream>
//! - UtInput.hpp
//! - WsfComponentFactory.hpp
//! - WsfScenario.hpp
//! - And at least a forward declaration for the class defined by the template argument PARENT_TYPE.

template<typename PARENT_TYPE>
class WsfComponentFactoryList
{
public:
   typedef std::vector<std::unique_ptr<WsfComponentFactoryBase>> FactoryList;
   typedef WsfComponentFactory<PARENT_TYPE>                      FactoryType;

   class Iterator
   {
   public:
      //! An iterator for iterating over the factories that apply to the class PARENT_TYPE.
      Iterator(const WsfScenario& aScenario)
         : mFactoryList(aScenario.GetComponentFactoryList())
      {
         mIndex      = 0;
         mSize       = mFactoryList.size();
         mParentRole = cCOMPONENT_ROLE<PARENT_TYPE>();

         // If the first factory is defined but doesn't have the desired role, we must advance
         // the iterator to the dereferencing operator will return the first entry with the
         // desired role (if one exists).
         if ((mIndex != mSize) && (mParentRole != mFactoryList[mIndex]->GetParentRole()))
         {
            AdvanceP();
         }
      }

      //! Return the pointer to the factory currently addressed by the iterator.
      //! @returns Pointer to the factory, cast to the proper type.
      FactoryType* operator*() const { return static_cast<FactoryType*>(mFactoryList[mIndex].get()); }

      //! Pre-increment operator.
      //! This must never be called if the iterator is already at the end of the list.
      Iterator& operator++()
      {
         AdvanceP();
         return *this;
      }

      //! Return true if the end-of-list has been reached.
      bool AtEnd() const { return mIndex >= mSize; }

   private:
      //! Advance to the next factory that has the requested parent role.
      void AdvanceP()
      {
         ++mIndex;
         while ((mIndex < mSize) && (mParentRole != mFactoryList[mIndex]->GetParentRole()))
         {
            ++mIndex;
         }
      }

      const FactoryList& mFactoryList;
      size_t             mSize;
      size_t             mIndex;
      int                mParentRole;
   };

   //! @name Iterative calls to the methods in WsfComponentFactory.
   //! See WsfComponentFactory for specific methods.
   //@{

   static void PreInput(const WsfScenario& aScenario, PARENT_TYPE& aParent)
   {
      for (Iterator iter(aScenario); !iter.AtEnd(); ++iter)
      {
         (*iter)->PreInput(aParent);
      }
   }

   static bool ProcessInput(const WsfScenario& aScenario, UtInput& aInput, PARENT_TYPE& aParent)
   {
      for (Iterator iter(aScenario); !iter.AtEnd(); ++iter)
      {
         if ((*iter)->ProcessInput(aInput, aParent))
         {
            return true;
         }
      }
      return false;
   }

   static bool ProcessAddOrEditCommand(const WsfScenario& aScenario, UtInput& aInput, PARENT_TYPE& aParent, bool aIsAdding)
   {
      for (Iterator iter(aScenario); !iter.AtEnd(); ++iter)
      {
         if ((*iter)->ProcessAddOrEditCommand(aInput, aParent, aIsAdding))
         {
            return true;
         }
      }
      return false;
   }

   static void ProcessDeleteCommand(const WsfScenario& aScenario, UtInput& aInput, PARENT_TYPE& aParent)
   {
      std::string componentType(aInput.GetCommand());
      for (Iterator iter(aScenario); !iter.AtEnd(); ++iter)
      {
         if ((*iter)->ProcessDeleteCommand(aInput, aParent))
         {
            return;
         }
      }
      throw UtInput::BadValue(aInput, "Unknown object type to delete: " + componentType);
   }

   static bool PreInitialize(const WsfScenario& aScenario, double aSimTime, PARENT_TYPE& aParent)
   {
      bool ok(true);
      for (Iterator iter(aScenario); !iter.AtEnd(); ++iter)
      {
         if (!(*iter)->PreInitialize(aSimTime, aParent))
         {
            ok       = false;
            auto out = ut::log::error() << "Component factory pre-initialization failed.";
            out.AddNote() << "Component: " << aParent.GetName();
         }
      }
      return ok;
   }
   //@}
};

#endif
