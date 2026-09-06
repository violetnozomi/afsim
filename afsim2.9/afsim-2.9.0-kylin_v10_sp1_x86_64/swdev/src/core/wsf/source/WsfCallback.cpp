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

#include "WsfCallback.hpp"

#include "UtLog.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfProcessor.hpp"
#include "WsfSimulation.hpp"

// =================================================================================================
WsfCallback::WsfCallback()
   : WsfObject()
{
}

// =================================================================================================
// protected
WsfCallback::WsfCallback(const WsfCallback& aSrc)
   : WsfObject(aSrc)
{
}

// =================================================================================================
// virtual
WsfCallback::~WsfCallback() {}

// =================================================================================================
// virtual
const int* WsfCallback::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_CALLBACK, 0};
   return roles;
}

// =================================================================================================
// virtual
void* WsfCallback::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_CALLBACK)
   {
      return this;
   }
   return nullptr;
}

// =================================================================================================
//! Primary entry for invoking a callback from other simulation components.
//! This is what is called when another simulation wants to trigger the callback. This routine
//! invokes the Execute method and also invokes things like simulation observers.
//! @param aSimTime The current simulation time.
//! @param aPlatformPtr The platform executing the callback
//! @param aCallbackName The name of the callback to execute
//! @returns The status from the callback Execute method.
// static
bool WsfCallback::ExecuteAndNotify(double aSimTime, WsfPlatform* aPlatformPtr, WsfStringId aCallbackName)
{
   bool         status(false);
   WsfCallback* callbackPtr(aPlatformPtr->GetComponent<WsfCallback>(aCallbackName));
   if (callbackPtr != nullptr)
   {
      status = callbackPtr->Execute(aSimTime, aPlatformPtr);
      WsfObserver::ExecuteCallback(aPlatformPtr->GetSimulation())(aSimTime, aPlatformPtr, *callbackPtr);
      for (WsfComponentList::RoleIterator<WsfProcessor> iter(*aPlatformPtr); !iter.AtEnd(); ++iter)
      {
         iter->ProcessCallback(aSimTime, *callbackPtr);
      }
   }
   else
   {
      auto out = ut::log::error() << "Platform unable to invoke callback.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << aPlatformPtr->GetName();
      out.AddNote() << "Callback: " << aCallbackName;
   }
   return status;
}
