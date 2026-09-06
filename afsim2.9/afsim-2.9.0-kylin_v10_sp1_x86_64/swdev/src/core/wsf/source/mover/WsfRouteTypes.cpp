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

#include "WsfRouteTypes.hpp"

#include <memory>

#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfDeferredInput.hpp"
#include "WsfObjectTypeList.hpp"
#include "WsfRoute.hpp"
#include "WsfScenario.hpp"

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfRouteTypes& WsfRouteTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetRouteTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfRouteTypes& WsfRouteTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetRouteTypes();
}

// =================================================================================================
WsfRouteTypes::WsfRouteTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfRoute>(aScenario, "route")
{
   SetSingularBaseType();
   Add("WSF_ROUTE", ut::make_unique<WsfRoute>());
}

// =================================================================================================
// DEPRECATED as of 2.9
bool WsfRouteTypes::LoadInstance(UtInput& aInput, WsfRoute*& aRoutePtr) const
{
   std::unique_ptr<WsfRoute> routePtr;
   bool                      myCommand = LoadInstance(aInput, routePtr);
   aRoutePtr                           = routePtr.release();
   return myCommand;
}

// =================================================================================================
bool WsfRouteTypes::LoadInstance(UtInput& aInput, std::unique_ptr<WsfRoute>& aRoutePtr) const
{
   bool        myCommand = false;
   std::string command(aInput.GetCommand());

   if (command == "route")
   {
      myCommand = true;
      UtInputBlock inputBlock(aInput);
      aRoutePtr = ut::make_unique<WsfRoute>();
      WsfRoute::ProcessInputBlock(inputBlock, aRoutePtr.get(), true);
   }
   else if (command == "use_route")
   {
      myCommand = true;
      std::string routeName;
      aInput.ReadValue(routeName);
      GetScenario()->GetDeferredInput().Requires("route", routeName);
      aRoutePtr.reset(Clone(routeName));
      if (aRoutePtr == nullptr)
      {
         throw UtInput::BadValue(aInput, "Route does not exist: " + routeName);
      }
   }
   return myCommand;
}
