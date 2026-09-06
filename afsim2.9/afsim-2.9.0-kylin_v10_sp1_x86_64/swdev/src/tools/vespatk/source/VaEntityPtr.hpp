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

#ifndef VA_ENTITY_PTR_HPP
#define VA_ENTITY_PTR_HPP

#include <cassert>
#include <memory>

#include "VaCallbackHolder.hpp"
#include "VaEntity.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"

namespace vespa
{
namespace detail
{
struct EntityDeleter
{
   //! Defined at end of file.
   void operator()(VaEntity* aEntity) const;
};

template<typename T>
struct StrongEntityPtrDefinitionHelper
{
   static_assert(std::is_base_of<VaEntity, T>::value, "Template argument must derive from VaEntity.");
   static_assert(!std::is_const<T>::value, "Template argument must not be const.");

   using type = std::unique_ptr<T, EntityDeleter>;
};
} // namespace detail

//! An owning smart pointer to a VaEntity.
//! Deletes contents via the VaEnvironment so that the proper callbacks are called.
template<typename T>
using StrongEntityPtr = typename ::vespa::detail::StrongEntityPtrDefinitionHelper<T>::type;

//! A wrapper around a non-owning pointer to a VaEntity.
//! Automatically sets the pointer to null if the entity is destroyed.
template<typename T>
class EntityPtr
{
   static_assert(std::is_base_of<VaEntity, T>::value, "Template argument must derive from VaEntity.");

public:
   EntityPtr(std::nullptr_t = nullptr) noexcept {}

   template<typename U>
   EntityPtr(const StrongEntityPtr<U>& aOther) noexcept
   {
      static_assert(std::is_convertible<U*, T*>::value, "Cannot perform conversion.");
      Connect(aOther.get());
   }

   template<typename U>
   EntityPtr(const EntityPtr<U>& aOther) noexcept
   {
      static_assert(std::is_convertible<U*, T*>::value, "Cannot perform conversion.");
      Connect(aOther.get());
   }

   template<typename U>
   EntityPtr(U* aPointer) noexcept
   {
      static_assert(std::is_convertible<U*, T*>::value, "Cannot perform conversion.");
      Connect(aPointer);
   }

   EntityPtr<T>& operator=(const EntityPtr<T>& aOther)
   {
      Connect(aOther.get());
      return *this;
   }

   T* operator->() const noexcept
   {
      assert(mPointer != nullptr);
      return mPointer;
   }

   T& operator*() const noexcept
   {
      assert(mPointer != nullptr);
      return *mPointer;
   }

   // Following the naming convention of StrongEntityPtr (aka unique_ptr) for consistency.
   T* get() const noexcept { return mPointer; }

   void reset()
   {
      vespa::detail::EntityDeleter()(mPointer);
      assert(mPointer == nullptr);
   }

   explicit operator bool() const noexcept { return mPointer != nullptr; }

private:
   void Connect(T* aPointer) noexcept
   {
      mCallbacks.Clear();
      mPointer = aPointer;
      if (mPointer != nullptr)
      {
         mCallbacks += VaObserver::EntityDeleted.Connect(&EntityPtr<T>::Disconnect, this, mPointer->GetUniqueId());
      }
   }

   void Disconnect(VaEntity* /*dummy param*/) noexcept
   {
      mCallbacks.Clear();
      mPointer = nullptr;
   }

   // Marked mutable because mPointer can be modified by callbacks in contexts where *this is const.
   // Not sure if this is necessary.
   mutable T*       mPointer = nullptr;
   VaCallbackHolder mCallbacks;
};

template<typename T>
using ConstEntityPtr = EntityPtr<const T>;

// Weak - Weak
template<typename T, typename U>
bool operator==(const EntityPtr<T>& aLeft, const EntityPtr<U>& aRight)
{
   return aLeft.get() == aRight.get();
}

template<typename T, typename U>
bool operator!=(const EntityPtr<T>& aLeft, const EntityPtr<U>& aRight)
{
   return aLeft.get() != aRight.get();
}

// Strong - Weak
template<typename T, typename U>
bool operator==(const StrongEntityPtr<T>& aLeft, const EntityPtr<U>& aRight)
{
   return aLeft.get() == aRight.get();
}

template<typename T, typename U>
bool operator!=(const StrongEntityPtr<T>& aLeft, const EntityPtr<U>& aRight)
{
   return aLeft.get() != aRight.get();
}

// Weak - Strong
template<typename T, typename U>
bool operator==(const EntityPtr<T>& aLeft, const StrongEntityPtr<U>& aRight)
{
   return aLeft.get() == aRight.get();
}

template<typename T, typename U>
bool operator!=(const EntityPtr<T>& aLeft, const StrongEntityPtr<U>& aRight)
{
   return aLeft.get() != aRight.get();
}

// Raw - Weak
template<typename T, typename U>
bool operator==(T* aLeft, const EntityPtr<U>& aRight)
{
   return aLeft == aRight.get();
}

template<typename T, typename U>
bool operator!=(T* aLeft, const EntityPtr<U>& aRight)
{
   return aLeft != aRight.get();
}

// Weak - Raw
template<typename T, typename U>
bool operator==(const EntityPtr<T>& aLeft, U* aRight)
{
   return aLeft.get() == aRight;
}

template<typename T, typename U>
bool operator!=(const EntityPtr<T>& aLeft, U* aRight)
{
   return aLeft.get() != aRight;
}

// nullptr - Weak
template<typename T>
bool operator==(std::nullptr_t, const EntityPtr<T>& aRight)
{
   return nullptr == aRight.get();
}

template<typename T>
bool operator!=(std::nullptr_t, const EntityPtr<T>& aRight)
{
   return nullptr != aRight.get();
}

// Weak - nullptr
template<typename T>
bool operator==(const EntityPtr<T>& aLeft, std::nullptr_t)
{
   return aLeft.get() == nullptr;
}

template<typename T>
bool operator!=(const EntityPtr<T>& aLeft, std::nullptr_t)
{
   return aLeft.get() != nullptr;
}

//! Utility functions for creating VaEntities and derived types
//! {

//! Makes a new T and adds it to aParent using U::AddEntity(T*).
//! See make_entity in WkfScenario.hpp.
template<typename T, typename U, typename... Args>
EntityPtr<T> make_entity_at(U& aParent, Args&&... aArgs)
{
   static_assert(std::is_base_of<VaEntity, T>::value, "T must derive from VaEntity.");
   T* retval = new T(std::forward<Args>(aArgs)...);
   aParent.AddEntity(retval);
   return retval;
}

//! Makes a new T and adds it to aParent using U::AddEntity(T*).
//! See make_strong_entity_ptr in WkfScenario.hpp.
template<typename T, typename U, typename... Args>
StrongEntityPtr<T> make_strong_entity_ptr_at(U& aParent, Args&&... aArgs)
{
   static_assert(std::is_base_of<VaEntity, T>::value, "T must derive from VaEntity.");
   StrongEntityPtr<T> retval(new T(std::forward<Args>(aArgs)...));
   aParent.AddEntity(retval);
   return retval;
}
//! }
} // namespace vespa

inline void vespa::detail::EntityDeleter::operator()(VaEntity* aEntity) const
{
   VaEnvironment::Instance().DeleteEntity(aEntity);
}

#endif
