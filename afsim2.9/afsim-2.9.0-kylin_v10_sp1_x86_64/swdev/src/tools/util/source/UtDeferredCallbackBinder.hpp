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

#ifndef UTDEFERREDCALLBACKBINDER_HPP
#define UTDEFERREDCALLBACKBINDER_HPP

/*************************************************************************************************
 * @brief  interface for deferring functions that need to attach to a callback list, but do not
 *         yet have all the data yet bound to the function that it needs to operate on.
 *
 * tparam  DeferredDataParamatersToBind  List of data types that will be bound to a function before adding to a callback list
 * @date 7/13/2017
 * @see   UtDeferredCallbackBinderImpl for more detailed description of purpose and implementation of this interface
 **************************************************************************************************/
template<typename... DeferredDataParamatersToBind>
class UtDeferredCallbackBinder
{
public:
   virtual ~UtDeferredCallbackBinder() = default;

   /*************************************************************************************************
    * @brief   Sets all the data that this interface had been waiting for to complete the bindings of the functions and
    *allow the binded function to be added to the callback list
    * @param   aDeferedData   Variable arguments providing information describing the defered.
    **************************************************************************************************/
   virtual void SetDeferredInfo(DeferredDataParamatersToBind... aDeferedData) = 0;

protected:
   UtDeferredCallbackBinder() = default;
};
#endif
