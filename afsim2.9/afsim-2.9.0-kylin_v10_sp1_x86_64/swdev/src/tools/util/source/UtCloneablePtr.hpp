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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef UTCLONEABLEPTR_HPP
#define UTCLONEABLEPTR_HPP

#include <cstddef>
#include <memory>

#include "UtMemory.hpp"

namespace ut
{

//! Implementation of a clonable pointer for objects that support the Clone() method.
//!
//! This is a wrapper class that provides a 'smart pointer' to a cloneable object.
//! When a pointer is assigned to an instance of 'cloneable pointer' it assumes ownership of the
//! pointed-to object. If the 'cloneable-pointer' is subsequently copy  or assigned, the target
//! 'cloneable-pointer' will point to a clone of the object referenced by the source. When a
//! 'cloneable-pointer' is deleted it will delete the referenced object.
//!
//! This assumes the referenced class contains a Clone() method that will return a pointer to
//! a new instance of the object that is a clone of the object. If the template argument is a
//! polymorphic type then Clone() is acting as a virtual copy constructor.
//!
//! This is very useful in classes that contain member variables that point to objects
//! that need to be cloned if they are copied.
template<typename T>
class CloneablePtr
{
public:
   //! Default constructor.
   constexpr CloneablePtr() noexcept = default;

   constexpr CloneablePtr(std::nullptr_t) noexcept {}

   explicit CloneablePtr(T* aObjectPtr) noexcept
      : mObjectPtr(aObjectPtr)
   {
   }

   //! If aObjectPtr is non-null then it is assumed to be pointing at an object that supports
   //! the Clone() method. This class assumes ownership of the pointed-to object.
   CloneablePtr(std::unique_ptr<T> aObjectPtr) noexcept
      : mObjectPtr(std::move(aObjectPtr))
   {
   }

   //! construction from a std::unique_ptr of a derived type.
   template<class U, enable_if_t<std::is_base_of<T, U>::value, int> = 0>
   CloneablePtr(std::unique_ptr<U> aUniquePtr) noexcept
      : mObjectPtr(std::move(aUniquePtr))
   {
   }

   //! user-defined default copy constructor.
   //! If the source points to an object then the new pointer will point to a clone of the object.
   CloneablePtr(const CloneablePtr<T>& aSrc)
      : mObjectPtr(nullptr)
   {
      operator=(aSrc);
   }

   //! Copy constructor from derived type.
   template<class U, enable_if_t<std::is_base_of<T, U>::value, int> = 0>
   CloneablePtr(const CloneablePtr<U>& aSrc)
      : mObjectPtr()
   {
      operator=(aSrc);
   }

   //! default move constructor.
   CloneablePtr(CloneablePtr<T>&&) noexcept = default;

   //! Move constructor from derived type.
   template<class U, enable_if_t<std::is_base_of<T, U>::value, int> = 0>
   CloneablePtr(CloneablePtr<U>&& aMov) noexcept
      : mObjectPtr(aMov.release())
   {
   }

   //! Destructor
   ~CloneablePtr() = default;

   //! Checks whether *this owns an object, i.e. whether get() != nullptr.
   explicit operator bool() const noexcept { return mObjectPtr != nullptr; }

   //! user-defined default copy assignment operator.
   CloneablePtr<T>& operator=(const CloneablePtr<T>& aRhs) noexcept
   {
      if (this != &aRhs)
      {
         if (aRhs != nullptr)
         {
            // use operator= to support variety of return types for Clone()
            operator=(aRhs->Clone());
         }
         else
         {
            mObjectPtr = nullptr;
         }
      }
      return *this;
   }

   //! Copy assignment operator using a CloneablePtr of a derived type.
   template<class U, enable_if_t<std::is_base_of<T, U>::value, int> = 0>
   CloneablePtr<T>& operator=(const CloneablePtr<U>& aRhs) noexcept
   {
      if (aRhs != nullptr)
      {
         // use operator= to support variety of return types for Clone()
         operator=(aRhs->Clone());
      }
      else
      {
         mObjectPtr = nullptr;
      }
      return *this;
   }

   //! default move assignment operator.
   CloneablePtr<T>& operator=(CloneablePtr<T>&&) noexcept = default;

   //! move assignment operator for CloneablePtr of derived type.
   template<class U, enable_if_t<std::is_base_of<T, U>::value, int> = 0>
   CloneablePtr<T>& operator=(CloneablePtr<U>&& aMov) noexcept
   {
      mObjectPtr.reset(aMov.release());
      return *this;
   }

   //! Assignment operator using a pointer to an object.
   //! The previously referenced object is deleted.
   //! The class assumes ownership of the new object.
   template<class U, enable_if_t<std::is_base_of<T, U>::value, int> = 0>
   CloneablePtr<T>& operator=(std::unique_ptr<U> aUniquePtr) noexcept
   {
      mObjectPtr = std::move(aUniquePtr);
      return *this;
   }

   //! Assignment operator using a pointer to an object.
   //! The previously referenced object is deleted.
   //! The class assumes ownership of the new object.
   CloneablePtr<T>& operator=(T* aObjectPtr) noexcept
   {
      reset(aObjectPtr);
      return *this;
   }

   //! assignment to nullptr
   CloneablePtr<T>& operator=(std::nullptr_t) noexcept
   {
      mObjectPtr = nullptr;
      return *this;
   }

   //! @name Modifiers
   //! @{

   //! returns a pointer to the managed object and releases the ownership
   T* release() noexcept { return mObjectPtr.release(); }

   //! replaces the managed object
   void reset(T* aObjectPtr = nullptr) noexcept
   {
      if (get() != aObjectPtr)
      {
         mObjectPtr.reset(aObjectPtr);
      }
   }

   // swaps the managed objects
   void swap(CloneablePtr& aOther) noexcept { mObjectPtr.swap(aOther.mObjectPtr); }
   //! @}

   //! Returns true if the underlying pointer is equal to the specified value.
   //! (Typically used for comparing to zero)
   bool operator==(const T* aRhsPtr) const noexcept { return (get() == aRhsPtr); }

   //! Returns true if the underlying pointer is not equal to the specified value.
   //! (Typically used for comparing to zero)
   bool operator!=(const T* aRhsPtr) const noexcept { return (get() != aRhsPtr); }

   //! Return a reference to the underlying object.
   T& operator*() const { return *mObjectPtr; }

   //! Return a pointer to the underlying object.
   T* operator->() const noexcept { return get(); }

   //! implicit conversion from lvalue CloneablePtr to raw ptr, allowing it to be used as an argument when a pointer is expected.
   operator T*() const& noexcept { return get(); }

   // std::is_same is used here to work around a GCC bug we encountered exclusively during unity builds.
   //! implicit conversion from rvalue CloneablePtr to std::unique_ptr.
   template<class U, enable_if_t<std::is_base_of<U, T>::value || std::is_same<U, T>::value, int> = 0>
   operator std::unique_ptr<U>() && noexcept
   {
      return std::unique_ptr<U>{std::move(mObjectPtr)};
   }

   //! Explicit raw access to the pointer.
   T* Pointer() const noexcept { return get(); }
   T* get() const noexcept { return mObjectPtr.get(); }

   // Operators defined as "hidden friends" to make them only accessible via ADL.
   friend bool operator==(std::nullptr_t, const CloneablePtr<T>& x) noexcept { return x.Pointer() == nullptr; }

   friend bool operator!=(std::nullptr_t, const CloneablePtr<T>& x) noexcept { return !(nullptr == x); }

   friend bool operator==(const CloneablePtr<T>& x, std::nullptr_t) noexcept { return x.Pointer() == nullptr; }

   friend bool operator!=(const CloneablePtr<T>& x, std::nullptr_t) noexcept { return !(x == nullptr); }

private:
   std::unique_ptr<T> mObjectPtr;
};

template<class T>
void swap(CloneablePtr<T>& aLhs, CloneablePtr<T>& aRhs)
{
   aLhs.swap(aRhs);
}

} // end namespace ut

namespace std
{
template<class T>
struct hash<::ut::CloneablePtr<T>>
{
   size_t operator()(const ::ut::CloneablePtr<T>& aSmartPtr) const noexcept
   {
      return ::std::hash<T*>()(aSmartPtr.get());
   }
};
} // namespace std

template<class T>
using UtCloneablePtr = ut::CloneablePtr<T>;

#endif
