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

#ifndef WSFVISUALPARTTYPES_HPP
#define WSFVISUALPARTTYPES_HPP

#include "WsfObjectTypeList.hpp"
#include "WsfVisualPart.hpp"

class WsfVisualPartTypes : public WsfObjectTypeList<WsfVisualPart>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_EXPORT WsfVisualPartTypes&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const WsfVisualPartTypes& Get(const WsfScenario& aScenario);
   //@}

   explicit WsfVisualPartTypes(WsfScenario& aScenario);
};

#endif
