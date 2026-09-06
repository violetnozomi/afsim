// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

//! Note: UtBinder.hpp and the STL functional includes must be included outside
//! namespaces by implementing classes, this include method is due to namespaces
//! in different projects.

#ifndef UT_CALLBACK_N_HPP
#define UT_CALLBACK_N_HPP

#include <vector>

#include "UtCallback.hpp"
#include "UtMemory.hpp"

template<typename Signature>
class UtCallbackN;

template<typename R, typename... Args>
class UtCallbackN<R(Args...)> : public UtCallback
{
public:
   using FunctionType = std::function<R(Args...)>;

   UtCallbackN()
      : UtCallback()
   {
   }
   UtCallbackN(const FunctionType& aFunc)
      : mFunc(aFunc)
   {
   }

   R operator()(Args... args) const { return mFunc(args...); }

private:
   FunctionType mFunc;
};

template<typename Signature>
class UtCallbackListN;

//! A callback list.
//! The callback list contains the list of 'subscribers' to the callback list.
template<typename R, typename... Args>
class UtCallbackListN<R(Args...)> : public UtCallbackList
{
   UtCallbackListN<R(Args...)>& operator=(UtCallbackListN<R(Args...)>&) = delete; // No implementation
public:
   using CallbackType = UtCallbackN<R(Args...)>;
   // UtCallbackListN() : UtCallbackList() {}
   // UtCallbackListN(const UtCallbackListN<R(Args...)>& aSrc) : UtCallbackList(aSrc) {}


   //! Invoke operator() for each subscriber.
   //! The base class operator() invokes operator() on each subscriber and does not return
   //! any value. If the return type of the subscribers are non-void then the user must
   //! create a different callback function (with a different name, because you can't override
   //! based simply on return type.
   void operator()(Args... args) const
   {
      for (auto it = mCallbackList.begin(); it != mCallbackList.end();)
      {
         (*static_cast<CallbackType*>(*it++))(args...);
      }
   }

   //! Create a non-member function callback and connect it to the callback list.
   //! @param aFunPtr The pointer to the function to be invoked.
   //! @return The created callback object.
   //! @note The caller assumes ownership of the callback object.
   std::unique_ptr<CallbackType> Connect(const std::function<R(Args...)>& aFunc)
   {
      auto callbackPtr = Create(aFunc);
      Connect(callbackPtr.get());
      return callbackPtr;
   }

   //! Connect a new subscriber.
   //! Connect a new subscriber.
   //! @param aCallbackPtr The callback object to be subscribed.
   //! @return The callback object (same as aCallbackPtr).
   //! @note The caller retains ownership of the callback object.
   CallbackType* Connect(CallbackType* aCallbackPtr)
   {
      ConnectP(aCallbackPtr);
      return aCallbackPtr;
   }

   //! Create a non-member function callback and connect it to the callback list.
   //! @param aFuncPtr The pointer to the function to be invoked.
   //! @return The created callback object.
   //! @note The caller assumes ownership of the callback object.
   std::unique_ptr<CallbackType> Connect(R (*aFuncPtr)(Args...))
   {
      auto callbackPtr = Create(UtStd::Bind(aFuncPtr));
      Connect(callbackPtr.get());
      return callbackPtr;
   }

   //! Create a member function callback and connect it to the callback list.
   //! @param aFuncPtr The pointer to the member function to be invoked.
   //! @param aObjPtr The pointer to the class containing the member function.
   //! @return The created callback object.
   //! @note The caller assumes ownership of the callback object.
   template<typename CT>
   std::unique_ptr<CallbackType> Connect(R (CT::*aFuncPtr)(Args...), CT* aObjPtr)
   {
      auto callbackPtr = Create(UtStd::Bind(aFuncPtr, aObjPtr));
      Connect(callbackPtr.get());
      return callbackPtr;
   }

   void GetCallbacks(std::vector<CallbackType*>& aCallbacks) const
   {
      for (const auto& callback : mCallbackList)
      {
         aCallbacks.push_back(static_cast<CallbackType*>(callback));
      }
   }

   void Merge(UtCallbackListN& aOtherCallbackList) { MergeP(aOtherCallbackList); }

private:
   //! Create a callback object for a std function, but do not connect it to the callback list.
   //! @param aFunc A reference to the function to be invoked.
   //! @return The created callback object.
   //! @note The caller assumes ownership of the callback object.
   std::unique_ptr<CallbackType> Create(const std::function<R(Args...)>& aFunc)
   {
      return ut::make_unique<CallbackType>(aFunc);
   }
};

#endif
