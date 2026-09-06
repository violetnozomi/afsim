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

#ifndef UT_REFLECT_VISITOR_HPP
#define UT_REFLECT_VISITOR_HPP

#include <map>

#include "UtReflectNode.hpp"

namespace ut
{
namespace reflect
{
template<typename VISITOR_T, typename NODE_T = ut::reflect::Node, typename FN_T = void()>
class Visitor;

//! CRTP template base class for defining visitors.
//! The type can be specialized by base node type, return type, and additional arguments.
//! Once functions have been registered, uses the GetNodeType methods in Node to select the
//!    most appropriate function to call.
//! If no function has been registered for the most derived type,
//!    caches the selected best-fit function.
//! Use @see ut::reflect::Visitor if the visitor itself needs to be modified while visiting.
//!    Otherwise, @see ut::reflect::ConstVisitor can be used.
//! If the node needs to be const, just make the second template argument a const type.
//! Example:
//! @code
//! struct MyVisitor : ut::reflect::Visitor<MyVisitor, const NodeType, int(int)>
//! @endcode
template<typename VISITOR_T, typename NODE_T, typename R, typename... ARGS>
class Visitor<VISITOR_T, NODE_T, R(ARGS...)>
{
   static_assert(std::is_base_of<Node, NODE_T>::value, "NODE_T must derive from Node.");

   template<typename T>
   using ConstCorrectNode = typename std::conditional<std::is_const<NODE_T>::value, const T, T>::type;

   template<typename T>
   using MemberFunction = R (VISITOR_T::*)(ConstCorrectNode<T>&, ARGS...);
   template<typename T>
   using ConstMemberFunction = R (VISITOR_T::*)(ConstCorrectNode<T>&, ARGS...) const;

   template<typename T>
   using FreeFunction = R (*)(ConstCorrectNode<T>&, ARGS...);

   template<typename T>
   using PseudoMemberFunction = R (*)(VISITOR_T&, ConstCorrectNode<T>&, ARGS...);
   template<typename T>
   using ConstPseudoMemberFunction = R (*)(const VISITOR_T&, ConstCorrectNode<T>&, ARGS...);

   using Function = R (Visitor::*)(NODE_T&, ARGS...);

public:
   using BaseVisitor = Visitor<VISITOR_T, NODE_T, R(NODE_T&, ARGS...)>;
   using NodeType    = NODE_T;
   using ReturnType  = R;

   //! Registers a member function of VISITOR_T.
   //! If a function for type T is already registered, replaces it.
   template<typename T, MemberFunction<T> FUNC>
   void Register()
   {
      static_assert(std::is_base_of<Visitor, VISITOR_T>::value, "VISITOR_T must derive from Visitor<VISITOR_T, NODE_T>.");
      static_assert(std::is_base_of<NODE_T, T>::value, "T must derive from NODE_T.");

      mRegistered[typeid(T)] = &Visitor::CallableMemberFunction<T, FUNC>;
      mDerived.clear();
   }

   //! Registers a member function of VISITOR_T.
   //! If a function for type T is already registered, replaces it.
   template<typename T, ConstMemberFunction<T> FUNC>
   void Register()
   {
      static_assert(std::is_base_of<Visitor, VISITOR_T>::value, "VISITOR_T must derive from Visitor<VISITOR_T, NODE_T>.");
      static_assert(std::is_base_of<NODE_T, T>::value, "T must derive from NODE_T.");

      mRegistered[typeid(T)] = &Visitor::CallableConstMemberFunction<T, FUNC>;
      mDerived.clear();
   }

   //! Registers a free function.
   //! If a function for type T is already registered, replaces it.
   template<typename T, FreeFunction<T> FUNC>
   void Register()
   {
      static_assert(std::is_base_of<NODE_T, T>::value, "T must derive from NODE_T.");

      mRegistered[typeid(T)] = &Visitor::CallableFreeFunction<T, FUNC>;
      mDerived.clear();
   }

   //! Registers a pseudo-member function of VISITOR_T.
   //! That is, a free function with a VISITOR_T& as the first argument.
   //! If a function for type T is already registered, replaces it.
   template<typename T, PseudoMemberFunction<T> FUNC>
   void Register()
   {
      static_assert(std::is_base_of<Visitor, VISITOR_T>::value, "VISITOR_T must derive from Visitor<VISITOR_T, NODE_T>.");
      static_assert(std::is_base_of<NODE_T, T>::value, "T must derive from NODE_T.");

      mRegistered[typeid(T)] = &Visitor::CallablePseudoMemberFunction<T, FUNC>;
      mDerived.clear();
   }

   //! Registers a pseudo-member function of VISITOR_T.
   //! That is, a free function with a VISITOR_T& as the first argument.
   //! If a function for type T is already registered, replaces it.
   template<typename T, ConstPseudoMemberFunction<T> FUNC>
   void Register()
   {
      static_assert(std::is_base_of<Visitor, VISITOR_T>::value, "VISITOR_T must derive from Visitor<VISITOR_T, NODE_T>.");
      static_assert(std::is_base_of<NODE_T, T>::value, "T must derive from NODE_T.");

      mRegistered[typeid(T)] = &Visitor::CallableConstPseudoMemberFunction<T, FUNC>;
      mDerived.clear();
   }

   //! Unregisters a function for type T.
   template<typename T>
   void Unregister()
   {
      auto it = mRegistered.find(typeid(T));
      if (it != mRegistered.end())
      {
         mRegistered.erase(it);
         mDerived.clear();
      }
   }

   //! Calls the most appropriate registered function and returns the result.
   R Visit(NodeType& aNode, ARGS... aArgs)
   {
      Function fn = GetFunction(aNode, 0);
      return (this->*fn)(aNode, std::forward<ARGS>(aArgs)...);
   }

   //! If T is a base of aNode, visits aNode as if it were a T.
   //! If T is not a base of aNode, calls Fallback().
   template<typename T>
   R VisitAs(NodeType& aNode, ARGS... aArgs)
   {
      static_assert(std::is_base_of<NodeType, T>::value, "T must derive from NODE_T.");

      for (int i = 0; i < aNode.NodeTypeCount(); i++)
      {
         if (aNode.GetNodeType(i) == typeid(T))
         {
            Function fn = GetFunction(aNode, i);
            return (this->*fn)(aNode, std::forward<ARGS>(aArgs)...);
         }
      }

      return Fallback(aNode, std::forward<ARGS>(aArgs)...);
   }

   //! Function called if no more appropriate function has been registered.
   virtual R Fallback(NodeType& aNode, ARGS... aArgs) = 0;

   //! Unregisters all functions.
   void Reset()
   {
      mRegistered.clear();
      mDerived.clear();
   }

private:
   std::map<std::type_index, Function> mRegistered;
   std::map<std::type_index, Function> mDerived;

   //! Recursive function for selecting and caching a function.
   Function GetFunction(NodeType& aNode, int aIndex)
   {
      if (aIndex >= aNode.NodeTypeCount())
      {
         return &Visitor::Fallback;
      }

      const auto type = aNode.GetNodeType(aIndex);

      auto it = mRegistered.find(type);
      if (it != mRegistered.end())
      {
         return it->second;
      }

      it = mDerived.find(type);
      if (it != mDerived.end())
      {
         return it->second;
      }

      Function fn = GetFunction(aNode, aIndex + 1);
      mDerived.emplace(type, fn);
      return fn;
   }

   template<typename T, MemberFunction<T> FUNC>
   R CallableMemberFunction(NodeType& aNode, ARGS... aArgs)
   {
      static_assert(std::is_base_of<Visitor, VISITOR_T>::value, "VISITOR_T must derive from Visitor<VISITOR_T, NODE_T>.");
      static_assert(std::is_base_of<NODE_T, T>::value, "T must derive from NODE_T.");

      return (static_cast<VISITOR_T&>(*this).*FUNC)(static_cast<ConstCorrectNode<T>&>(aNode), std::forward<ARGS>(aArgs)...);
   }

   template<typename T, ConstMemberFunction<T> FUNC>
   R CallableConstMemberFunction(NodeType& aNode, ARGS... aArgs)
   {
      static_assert(std::is_base_of<Visitor, VISITOR_T>::value, "VISITOR_T must derive from Visitor<VISITOR_T, NODE_T>.");
      static_assert(std::is_base_of<NODE_T, T>::value, "T must derive from NODE_T.");

      return (static_cast<VISITOR_T&>(*this).*FUNC)(static_cast<ConstCorrectNode<T>&>(aNode), std::forward<ARGS>(aArgs)...);
   }

   template<typename T, FreeFunction<T> FUNC>
   R CallableFreeFunction(NodeType& aNode, ARGS... aArgs)
   {
      static_assert(std::is_base_of<NODE_T, T>::value, "T must derive from NODE_T.");

      return FUNC(static_cast<ConstCorrectNode<T>&>(aNode), std::forward<ARGS>(aArgs)...);
   }

   template<typename T, PseudoMemberFunction<T> FUNC>
   R CallablePseudoMemberFunction(NodeType& aNode, ARGS... aArgs)
   {
      static_assert(std::is_base_of<Visitor, VISITOR_T>::value, "VISITOR_T must derive from Visitor<VISITOR_T, NODE_T>.");
      static_assert(std::is_base_of<NODE_T, T>::value, "T must derive from NODE_T.");

      return FUNC(static_cast<VISITOR_T&>(*this), static_cast<ConstCorrectNode<T>&>(aNode), std::forward<ARGS>(aArgs)...);
   }

   template<typename T, ConstPseudoMemberFunction<T> FUNC>
   R CallableConstPseudoMemberFunction(NodeType& aNode, ARGS... aArgs)
   {
      static_assert(std::is_base_of<Visitor, VISITOR_T>::value, "VISITOR_T must derive from Visitor<VISITOR_T, NODE_T>.");
      static_assert(std::is_base_of<NODE_T, T>::value, "T must derive from NODE_T.");

      return FUNC(static_cast<VISITOR_T&>(*this), static_cast<ConstCorrectNode<T>&>(aNode), std::forward<ARGS>(aArgs)...);
   }
};
} // namespace reflect
} // namespace ut

#endif
