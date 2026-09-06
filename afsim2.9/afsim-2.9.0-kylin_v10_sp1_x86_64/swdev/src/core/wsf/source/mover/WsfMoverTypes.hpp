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

#ifndef WSFMOVERTYPES_HPP
#define WSFMOVERTYPES_HPP

#include "WsfMover.hpp"
#include "WsfObjectTypeList.hpp"

class WsfMoverTypes : public WsfObjectTypeList<WsfMover>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_EXPORT WsfMoverTypes&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const WsfMoverTypes& Get(const WsfScenario& aScenario);
   //@}

   WsfMoverTypes(WsfScenario& aScenario);
};

#endif
