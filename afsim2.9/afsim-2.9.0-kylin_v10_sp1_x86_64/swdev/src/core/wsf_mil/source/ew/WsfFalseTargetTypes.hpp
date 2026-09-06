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

#ifndef WSFFALSETARGETTYPES_HPP
#define WSFFALSETARGETTYPES_HPP

#include "WsfFalseTarget.hpp"
#include "WsfObjectTypeList.hpp"

class WsfFalseTargetTypes : public WsfObjectTypeList<WsfFalseTarget>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_MIL_EXPORT WsfFalseTargetTypes&       Get(WsfScenario& aScenario);
   static WSF_MIL_EXPORT const WsfFalseTargetTypes& Get(const WsfScenario& aScenario);
   //@}

   explicit WsfFalseTargetTypes(WsfScenario& aScenario);

   bool InitializeType(WsfFalseTarget* aObjectPtr) override;

   bool LoadInstance(UtInput& aInput, WsfFalseTarget*& aInstancePtr);
};

#endif
