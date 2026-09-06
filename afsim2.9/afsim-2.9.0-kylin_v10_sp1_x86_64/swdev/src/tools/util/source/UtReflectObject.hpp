// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UT_REFLECT_OBJECT_HPP
#define UT_REFLECT_OBJECT_HPP

#include <algorithm>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "UtMemory.hpp"
#include "UtReflectDerive.hpp"

namespace ut
{
namespace reflect
{
//! ObjectNode represents a collection of sub-nodes.
//! Instead of deriving from this directly, prefer @see ut::reflect::Derive.
//! Use @see ObjectNode::Field to add fields.
template<typename NODE_T = Node>
class ObjectNode : public Derive<ObjectNode<NODE_T>, NODE_T>
{
   static_assert(std::is_base_of<Node, NODE_T>::value, "NODE_T must derive from Node.");

public:
   ObjectNode() = default;

   using ObjectBase = ObjectNode;
   using Derive<ObjectNode<NODE_T>, NODE_T>::Derive;

   ObjectNode(const ObjectNode&) = delete;
   void operator=(const ObjectNode&) = delete;

   ObjectNode(ObjectNode&&) = delete;
   void operator=(ObjectNode&&) = delete;

   //! Returns the number of fields.
   int FieldCount() const noexcept { return static_cast<int>(mFields.size()); }

   //! Returns the field at aIndex.
   //! If aIndex < 0, returns from the end of the list.
   // @{
   NODE_T& GetField(int aIndex)
   {
      if (aIndex < 0)
      {
         return *mFields.at(mFields.size() + aIndex);
      }
      return *mFields.at(aIndex);
   }

   const NODE_T& GetField(int aIndex) const
   {
      if (aIndex < 0)
      {
         return *mFields.at(mFields.size() + aIndex);
      }
      return *mFields.at(aIndex);
   }
   // @}

protected:
   //! Adds a field to the fields list.
   //! If added more than once, unexpected results may occur.
   void AddFieldPtr(NODE_T* aPointer) noexcept { mFields.emplace_back(aPointer); }

   //! Removes a field from the fields list.
   //! Only removes the last instance of aPointer.
   void RemoveFieldPtr(const NODE_T* aPointer) noexcept
   {
      // Because the removed field will almost always be at the end of the list,
      //    we search from back to front.
      auto it = std::find(mFields.rbegin(), mFields.rend(), aPointer);
      if (it != mFields.rend())
      {
         // The -1 is because reverse_iterators store the iterator one-past the element they reference.
         mFields.erase(it.base() - 1);
      }
   }

   //! Can be declared on a derived ObjectNode to add a T field.
   //! @code
   //! struct Derived : Derive<Derived, ObjectNode>
   //! {
   //!    Field<Type> name{ this, ... };
   //! };
   //! @endcode
   template<typename T>
   class Field
   {
      static_assert(std::is_base_of<Node, T>::value, "T must derive from Node.");

   public:
      //! Constructs the T and adds to to aObject.
      template<typename... Args, typename = ut::enable_if_t<std::is_constructible<T, Args...>::value>>
      Field(ObjectNode<NODE_T>* aObject, Args&&... aArgs)
         : mObject(aObject)
         , mValue(std::forward<Args>(aArgs)...)
      {
         if (!mObject)
         {
            throw std::logic_error("Fields must go on an Object.");
         }
         mObject->AddFieldPtr(&mValue);
      }

      Field(const Field&) = delete;
      Field(Field&&)      = delete;

      void operator=(const Field&) = delete;
      void operator=(Field&&) = delete;

      ~Field() noexcept { mObject->RemoveFieldPtr(&mValue); }

      //! Allows access to the field value.
      // @{
      T&       operator*() noexcept { return mValue; }
      const T& operator*() const noexcept { return mValue; }

      T*       operator->() noexcept { return &mValue; }
      const T* operator->() const noexcept { return &mValue; }

      T*       Ptr() noexcept { return &mValue; }
      const T* Ptr() const noexcept { return &mValue; }
      // @}

   private:
      ObjectNode<NODE_T>* mObject;
      T                   mValue;
   };

private:
   std::vector<NODE_T*> mFields;
};
} // namespace reflect
} // namespace ut

#endif
