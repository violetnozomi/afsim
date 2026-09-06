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

#ifndef WSFFALSETARGETSCREENERTYPES_HPP
#define WSFFALSETARGETSCREENERTYPES_HPP

#include "WsfFalseTargetScreener.hpp"
#include "WsfObjectTypeList.hpp"

class WsfFalseTargetScreenerTypes : public WsfObjectTypeList<WsfFalseTargetScreener>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_MIL_EXPORT WsfFalseTargetScreenerTypes&       Get(WsfScenario& aScenario);
   static WSF_MIL_EXPORT const WsfFalseTargetScreenerTypes& Get(const WsfScenario& aScenario);
   //@}

   explicit WsfFalseTargetScreenerTypes(WsfScenario& aScenario);
   bool LoadInstance(UtInput& aInput, WsfFalseTargetScreener*& aInstancePtr) const;
};

#endif
