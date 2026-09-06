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

#ifndef UTDEFERREDCALLBACKBINDERIMPL_HPP
#define UTDEFERREDCALLBACKBINDERIMPL_HPP

#include <functional>
#include <memory>

#include "UtBinder.hpp"
#include "UtDeferredCallbackBinder.hpp"
#include "UtMemory.hpp"

template<typename... SIG_DATA>
class UtDeferredCallbackBinderImpl;

/*************************************************************************************************
 * @brief   allows a user that does not have all the information required yet to implement a
 *          callback function to defer the implementation of the callback connection once the
 *          information is known.
 *
 *          This is accomplished by creating a function whose paramaters contain both the unknown
 *          data and the paramaters of the callback, in that order, and constructing this object
 *          with the callbacklist that the callback will be created for alongside said function.
 *          Then this object will be passed around / stored away until the necessary information
 *           has been determined, which in turn shall be passed to this object via SetDeferredInfo,
 *           thus allowing for the function to be attached to the callbacklist.
 *
 * tparam  CALLBACKLIST   the type of Callback list that the generated function will be added to.
 *                         Only tested with UtCallbackListN and VaCallbackListN.
 *
 * tparam  DEFERRED_DATA  the list of data types that are being defered for later binding before the provided function
 *can be added to the callbacklist.
 * @date 7/13/2017
 * @see   UtDeferredCallbackBinder
 * @see   SetDeferredInfo
 * @see   MakeDeferredBinder for constructing this object
 **************************************************************************************************/
template<template<typename> class CALLBACKLIST, typename CB_RETURN_TYPE, typename... CALL_BACK_ARGS, typename DEFERRED_DATA>
class UtDeferredCallbackBinderImpl<CALLBACKLIST<CB_RETURN_TYPE(CALL_BACK_ARGS...)>, DEFERRED_DATA> :
   public UtDeferredCallbackBinder<DEFERRED_DATA>
{
public:
   using CallbackFunction = std::function<CB_RETURN_TYPE(DEFERRED_DATA, CALL_BACK_ARGS...)>;
   using CallbackListType = CALLBACKLIST<CB_RETURN_TYPE(CALL_BACK_ARGS...)>;

   /*************************************************************************************************
    * @brief   Constructor.
    * @date 7/13/2017
    * @param   aCallbackList  CallbackList to have aFunc added to once the deferred data has been determined
    * @param   aFunc          The function that needs the deferred data bound to its first N paramaters, before it can
    *match the signature for the callbacklist
    **************************************************************************************************/
   UtDeferredCallbackBinderImpl(CallbackListType* aCallbackList, const CallbackFunction& aFunc)
      : UtDeferredCallbackBinder<DEFERRED_DATA>()
      , mFunc(aFunc)
      , mCallbackList(aCallbackList)
   {
   }

   /*************************************************************************************************
    * @brief   Destructor. releases the generated callback from the callback list
    * @date 7/13/2017
    **************************************************************************************************/
   virtual ~UtDeferredCallbackBinderImpl()
   {
      if (mGeneratedCallback != nullptr)
      {
         delete mGeneratedCallback;
      }
   }

private:
   const CallbackFunction mFunc;

   CallbackListType*                        mCallbackList;
   typename CallbackListType::CallbackType* mGeneratedCallback = nullptr;

   /*************************************************************************************************
    * @brief   Sets all the data that this interface had been waiting for to complete the bindings
    *          of the functions and allow the binded function to be added to the callback list.
    *
    *          Uses the values from aData to bind the deferred paramaters of the member function,
    *          which in turn creates a function signature that matches the necesary signature to be
    *          added to the callbacklist.
    * @param   aData the data values that will be bound to the member function.
    **************************************************************************************************/
   void SetDeferredInfo(DEFERRED_DATA aData) override
   {
      if (mFunc == nullptr) // don't make a callback for an empty function
      {
         return;
      }
      if (mGeneratedCallback != nullptr) // clean up any pre-existing callback generated from a previous call to this function
      {
         delete mGeneratedCallback;
      }
      mGeneratedCallback = mCallbackList->Connect([=](CALL_BACK_ARGS... aArgs) { return mFunc(aData, aArgs...); });
   }
};

namespace MakeDeferredBinder_pimpl
{
template<typename... args>
struct args_t
{
};

template<typename Tuple, int i>
using get_i = typename std::tuple_element<i, Tuple>::type;

template<typename... args, int... indices>
constexpr args_t<get_i<std::tuple<args...>, indices>...> args_subset(int_sequence<indices...>)
{
   return {};
}

// use the deferred args to specify templat paramaters for a deferred callback binder, and return it as a unique pointer.
template<template<typename> class cb_list, typename R, typename... cb_args, typename... f_args, typename... d_args>
std::unique_ptr<UtDeferredCallbackBinder<d_args...>> CreateDeferredH2(cb_list<R(cb_args...)>*            aList,
                                                                      const std::function<R(f_args...)>& aFunc,
                                                                      args_t<d_args...>)
{
   static_assert(
      std::is_constructible<UtDeferredCallbackBinderImpl<cb_list<R(cb_args...)>, d_args...>, decltype(aList), decltype(aFunc)>::value,
      "Can't create a deferred callback binder because the trailing function "
      "arguments of the provided function did not match the required paramaters "
      "from the callback list. "
      "The provided function must contain both the unknown arguements that are "
      "being deferred for setting later, and the arguements that are passed by "
      "the callback list (in that order).");
   return ut::make_unique<UtDeferredCallbackBinderImpl<cb_list<R(cb_args...)>, d_args...>>(aList, aFunc);
}

// make get the deferred args from the function pass it to a helper for extracting the parameter pack of the stored
// args_t<deferred args...>
template<template<typename> class cb_list, typename R, typename... cb_args, typename... f_args>
auto CreateDeferredH(cb_list<R(cb_args...)>* aList, const std::function<R(f_args...)>& aFunc)
   -> decltype(MakeDeferredBinder_pimpl::CreateDeferredH2(
      aList,
      aFunc,
      MakeDeferredBinder_pimpl::args_subset<f_args...>(make_int_sequence<sizeof...(f_args) - sizeof...(cb_args)>{})))
{
   return MakeDeferredBinder_pimpl::CreateDeferredH2(aList,
                                                     aFunc,
                                                     MakeDeferredBinder_pimpl::args_subset<f_args...>(
                                                        make_int_sequence<sizeof...(f_args) - sizeof...(cb_args)>{}));
}
} // namespace MakeDeferredBinder_pimpl

/*************************************************************************************************
 * @brief   Returns a unique pointer to a UtDeferredCallbackBinder<deferred_data...>. Will auto
 *          deduce necessary template parameters for constructing a UtDeferredCallbackBinderImpl, so
 *          that you don't need to know about the details of the Impl.
 * @param   aListPtrToConnectTo  pointer to the callback list that the function will connect to
 *                               once the deferred data is set.
 * @param   aCallbackFunction    A function whose paramaters start with the deferred_data and end
 *                               with the parameters passed by the callback list.
 * @return  a unique pointer to a UtDeferredCallbackBinder<deferred_data...>. deferred_data is
 *          auto deduced based on signatures of the provided parameters.
 * @date 8/8/2017
 * @see   UtDeferredCallbackBinder for how to use the returned pointer, and why you would want to create it.
 **************************************************************************************************/
template<template<typename> class callbacklist, typename R, typename... cb_args, typename... func_args>
auto MakeDeferredBinder(callbacklist<R(cb_args...)>*          aListPtrToConnectTo,
                        const std::function<R(func_args...)>& aCallbackFunction)
   -> decltype(MakeDeferredBinder_pimpl::CreateDeferredH(aListPtrToConnectTo, aCallbackFunction))
{
   static_assert(sizeof...(cb_args) < sizeof...(func_args),
                 "The provided function must contain both the unknown arguements that are "
                 "being deferred for setting later, and the arguements that are passed by "
                 "the callback list (in that order).");

   return MakeDeferredBinder_pimpl::CreateDeferredH(aListPtrToConnectTo, aCallbackFunction);
}

/*************************************************************************************************
 * @brief   Creates a passthrough function to the passed 'aFunc' whose paramaters are equivalent
 *          to the paramaters of the passed function plus the parameters needed for a function to
 *          connect to the passed callback list.
 *
 *          The purpose of this is to the creation of functions for a deferred callback, where the
 *          only necessary information for the function is the deferred data, which can in turn be
 *          added to any kind of callback list, through the addition of paramaters so as to make it
 *          compatible with the callback list.
 * @param   aCallbackListPtr  The list that we will be using in the instantiation of a deferred
 *                            callback binder.
 * @param   aFunc             The function that we need to change the signature of before adding
 *                            to be compatible with a deferred callback binder that uses
 *                            'aCallbackListPtr'.
 * @return  A function that is compatible, alongside 'aCallbackListPtr' for constructing a
 *          deferred callback binder.
 * @date 8/8/2017
 **************************************************************************************************/
template<template<typename> class Callbacklist, class R, class... CallbackArgs, class... DeferredArgs>
std::function<R(DeferredArgs..., CallbackArgs...)>
AddCallbackArgsToParams(const Callbacklist<R(CallbackArgs...)>*  aCallbackListPtr,
                        const std::function<R(DeferredArgs...)>& aFunc)
{
   if (aFunc == nullptr)
      return {nullptr};

   return [=](DeferredArgs... aDeferred, CallbackArgs...) { return aFunc(aDeferred...); };
}

/*************************************************************************************************
 * @brief   Creates a passthrough function to the passed 'aFunc' that can be added to the passed
 *          callback list.
 * @param   aCallbackListPtr  The callback list that we want the returned functioned to be
 *                            connectable to.
 * @param   aFunc             The void function with no arguements/parameters.
 * @return  A function compatible with connecting to 'aCallbackListPtr'.
 * @date 8/8/2017
 **************************************************************************************************/
template<template<typename> class Callbacklist, class... CallbackArgs>
std::function<void(CallbackArgs...)> AddCallbackArgsToParams(const Callbacklist<void(CallbackArgs...)>* aCallbackListPtr,
                                                             const std::function<void()>&               aFunc)
{
   if (aFunc == nullptr)
      return {nullptr};

   return [=](CallbackArgs...) { return aFunc(); };
}
#endif
