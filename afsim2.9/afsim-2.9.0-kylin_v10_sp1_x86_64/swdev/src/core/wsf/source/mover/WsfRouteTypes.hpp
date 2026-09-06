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

#ifndef WSFROUTETYPES_HPP
#define WSFROUTETYPES_HPP

#include "WsfObjectTypeList.hpp"
#include "WsfRoute.hpp"

class WsfRouteTypes : public WsfObjectTypeList<WsfRoute>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_EXPORT WsfRouteTypes&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const WsfRouteTypes& Get(const WsfScenario& aScenario);
   //@}

   WsfRouteTypes(WsfScenario& aScenario);

   WSF_DEPRECATED_EXPORT bool LoadInstance(UtInput& aInput, WsfRoute*& aRoutePtr) const;
   WSF_EXPORT bool            LoadInstance(UtInput& aInput, std::unique_ptr<WsfRoute>& aRoutePtr) const;
};

#endif
