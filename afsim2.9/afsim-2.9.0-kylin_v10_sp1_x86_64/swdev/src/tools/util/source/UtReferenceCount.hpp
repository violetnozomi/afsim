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

#ifndef UTREFERENCECOUNT_HPP
#define UTREFERENCECOUNT_HPP

#include "ut_export.h"

#include <atomic>
#include <cassert>

//! Defines a reference count.  Maintains a count of strong references and weak references.
//! Strong references enforce that an object is not deleted.
//! Weak references allow deletion of the referenced object, but enforce that this counter object is not destroyed.
class UT_EXPORT UtReferenceCount
{
public:
   UtReferenceCount(unsigned int aStrongRefCount = 0U, unsigned int aWeakRefCount = 0U)
      : mWeakStrongCount(static_cast<uint64_t>(aWeakRefCount) << 32 | static_cast<uint32_t>(aStrongRefCount))
   {
   }

   void AddWeakRef() { mWeakStrongCount.fetch_add(cWEAK_INC, std::memory_order_relaxed); }

   void AddStrongRef() { mWeakStrongCount.fetch_add(1, std::memory_order_relaxed); }

   //! Decrement the weak reference count.  Return 'true' if the weak reference count reaches zero.
   //! This object is deleted if both counts reach zero.
   bool RemoveWeakRef()
   {
      uint64_t newCount = mWeakStrongCount.fetch_sub(cWEAK_INC) - cWEAK_INC;

      // weak count should never fall below 0
      assert(((newCount + cWEAK_INC) >> 32) > 0U);

      // strong and weak counts are both 0, meaning there no longer exists
      // any handles to this reference counter, so it should be deleted.
      if (newCount == 0U)
      {
         delete this;
      }
      return newCount == 0U;
   }

   //! Decrement the strong reference count.  Return 'true' if the strong reference count reaches zero.
   //! A return value of 'true' indicates the object being reference counted may be deleted.
   //! This object is deleted if both counts reach zero.
   bool RemoveStrongRef()
   {
      uint64_t newCount = mWeakStrongCount.fetch_sub(1) - 1;

      // strong and weak counts are both 0, meaning there no longer exists
      // any handles to this reference counter, so it should be deleted.
      if (newCount == 0U)
      {
         delete this;
      }

      // return true if zero strong refs remain
      return (newCount & cSTRONG_MASK) == 0U;
   }

   //! Returns the number of strong references.
   unsigned int GetStrongCount() const
   {
      return static_cast<unsigned int>(mWeakStrongCount.load(std::memory_order_relaxed) & cSTRONG_MASK);
   }

   //! Returns the number of weak references
   unsigned int GetWeakCount() const
   {
      return static_cast<unsigned int>(mWeakStrongCount.load(std::memory_order_relaxed) >> 32);
   }

   //! Returns true if the referenced object is still valid.
   bool IsValid() const { return GetStrongCount() > 0U; }

private:
   // upper 32 bits = weak count, lower 32 bits = strong count
   std::atomic<uint64_t> mWeakStrongCount;

   // value for incrementing/decrementing the weak count in mWeakStrongCount
   static constexpr uint64_t cWEAK_INC = static_cast<uint64_t>(1) << 32;

   // mask for extracting strong value from mWeakStrongCount
   static constexpr uint64_t cSTRONG_MASK = cWEAK_INC - 1;
};

//! Reference-counted pointer to an object that contains an embedded reference count.
//! The class type T should have the method "UtReferenceCount* GetReferenceCount()"
//! UtReferencePtr is smaller and easier to use than UtSharedPtr, but requires that the contained type has a
//! 'GetReferenceCount()' method.
//! This may be used in conjunction with UtWeakReference.
template<typename T>
class UtReferencePtr
{
   inline void Release()
   {
      if (mPtr != nullptr)
      {
         if (mPtr->GetReferenceCount()->RemoveStrongRef())
            delete mPtr;
         mPtr = nullptr;
      }
   }

public:
   UtReferencePtr()
      : mPtr(nullptr)
   {
   }

   UtReferencePtr(T* aPtr)
      : mPtr(aPtr)
   {
      if (mPtr != nullptr)
      {
         mPtr->GetReferenceCount()->AddStrongRef();
      }
   }

   UtReferencePtr(const UtReferencePtr& aRhs)
      : mPtr(aRhs.mPtr)
   {
      if (mPtr != nullptr)
      {
         mPtr->GetReferenceCount()->AddStrongRef();
      }
   }

   UtReferencePtr& operator=(T* aRhs)
   {
      if (mPtr != aRhs)
      {
         Release();
         mPtr = aRhs;
         if (mPtr != nullptr)
         {
            mPtr->GetReferenceCount()->AddStrongRef();
         }
      }
      return *this;
   }

   UtReferencePtr& operator=(const UtReferencePtr& aRhs)
   {
      if (mPtr != aRhs.mPtr)
      {
         Release();
         mPtr = aRhs.mPtr;
         if (mPtr != nullptr)
         {
            mPtr->GetReferenceCount()->AddStrongRef();
         }
      }
      return *this;
   }

   ~UtReferencePtr() { Release(); }

   T* Pointer() const { return mPtr; }

   T& Get() const { return *mPtr; }

   T& operator*() const { return *mPtr; }

   bool IsValid() const { return mPtr != nullptr; }
        operator bool() const { return IsValid(); }

private:
   T* mPtr;
};

template<typename T>
class UtWeakReference
{
public:
   UtWeakReference()
      : mPtr()
      , mRefCountPtr()
   {
   }

   explicit UtWeakReference(T* aReferencePtr)
      : mPtr(aReferencePtr)
      , mRefCountPtr(nullptr)
   {
      if (mPtr != nullptr)
      {
         mRefCountPtr = mPtr->GetReferenceCount();
         mRefCountPtr->AddWeakRef();
      }
   }

   UtWeakReference(const UtWeakReference<T>& aRhs)
      : mPtr(aRhs.mPtr)
      , mRefCountPtr(aRhs.mRefCountPtr)
   {
      if (mRefCountPtr != nullptr)
      {
         mRefCountPtr->AddWeakRef();
      }
   }

   ~UtWeakReference() { ReleaseReference(); }

   //! Equivalent to SetReference(aPtr, aPtr->GetReferenceCount())
   template<typename S>
   UtWeakReference<T>& operator=(S* aPtr)
   {
      if (aPtr != nullptr)
      {
         SetReference(aPtr, aPtr->GetReferenceCount());
      }
      else
      {
         SetReference(nullptr, nullptr);
      }
      return *this;
   }

   UtWeakReference<T>& operator=(const UtWeakReference<T>& aRhs)
   {
      if (this != &aRhs)
      {
         SetRef(aRhs.mPtr, aRhs.mRefCountPtr);
      }
      return *this;
   }

   void SetReference(T* aPtr, UtReferenceCount* aRefCountPtr)
   {
      if (aPtr != mPtr)
      {
         SetRef(aPtr, aRefCountPtr);
      }
   }

   T* Get() const { return ((mRefCountPtr != nullptr) && mRefCountPtr->IsValid()) ? mPtr : nullptr; }

   T* operator->() const
   {
      assert((mRefCountPtr != nullptr) && mRefCountPtr->IsValid());
      return mPtr;
   }

   bool IsValid() const { return ((mRefCountPtr != nullptr) && mRefCountPtr->IsValid()); }

private:
   void ReleaseReference()
   {
      if (mRefCountPtr != nullptr)
      {
         mRefCountPtr->RemoveWeakRef();
         mRefCountPtr = nullptr;
      }
      mPtr = nullptr;
   }

   void SetRef(T* aPtr, UtReferenceCount* aCount)
   {
      ReleaseReference();
      mPtr = aPtr;
      if (mPtr != nullptr)
      {
         mRefCountPtr = aCount;
         mRefCountPtr->AddWeakRef();
      }
   }

   T*                mPtr;
   UtReferenceCount* mRefCountPtr;
};

#endif
