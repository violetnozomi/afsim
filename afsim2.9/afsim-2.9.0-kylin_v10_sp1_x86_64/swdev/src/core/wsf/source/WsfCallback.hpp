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

#ifndef WSFCALLBACK_HPP
#define WSFCALLBACK_HPP

#include "wsf_export.h"

#include "WsfComponent.hpp"
#include "WsfObject.hpp"
class WsfPlatform;

//! WsfCallback provides the functionality to create callbacks that
//! execute within the context of a platform or one of its processors.
//!
//! This functionality was created prior the existence of the scripting language and is now to be
//! considered deprecated. The only known use is by the air_traffic extension (XWsfAirTraffic).
class WSF_EXPORT WsfCallback : public WsfObject, public WsfPlatformComponent
{
public:
   WsfCallback();
   ~WsfCallback() override;

   //! @name Component infrastructure methods
   //@{
   WsfComponent* CloneComponent() const override { return Clone(); }
   WsfStringId   GetComponentName() const override { return GetNameId(); }
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   //@}

   WsfCallback* Clone() const override = 0;

   virtual bool Execute(double aSimTime, WsfPlatform* aPlatformPtr) = 0;

   // Needed to resolve ambiguity between WsfObject and WsfComponent.
   bool ProcessInput(UtInput& aInput) override { return false; }

   static bool ExecuteAndNotify(double aSimTime, WsfPlatform* aPlatformPtr, WsfStringId aCallbackName);

protected:
   WsfCallback(const WsfCallback& aSrc);

private:
   //! Assignment operator declared but not defined to prevent use.
   WsfCallback& operator=(const WsfCallback& aRhs) = delete;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfCallback, cWSF_COMPONENT_CALLBACK)

#endif
