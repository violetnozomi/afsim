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

#ifndef WSFROUTENETWORKTYPES_HPP
#define WSFROUTENETWORKTYPES_HPP

#include "WsfObjectTypeList.hpp"
#include "WsfRouteNetwork.hpp"

class WsfRouteNetworkTypes : public WsfObjectTypeList<WsfRouteNetwork>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_EXPORT WsfRouteNetworkTypes&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const WsfRouteNetworkTypes& Get(const WsfScenario& aScenario);
   //@}

   WsfRouteNetworkTypes(WsfScenario& aScenario);
};

#endif
