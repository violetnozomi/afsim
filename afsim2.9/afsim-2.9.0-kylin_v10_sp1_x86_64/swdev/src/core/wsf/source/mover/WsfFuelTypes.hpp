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

#ifndef WSFFUELTYPES_HPP
#define WSFFUELTYPES_HPP

#include "WsfFuel.hpp"
#include "WsfObjectTypeList.hpp"

class WsfFuelTypes : public WsfObjectTypeList<WsfFuel>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_EXPORT WsfFuelTypes&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const WsfFuelTypes& Get(const WsfScenario& aScenario);
   //@}

   WsfFuelTypes(WsfScenario& aScenario);
};

#endif
