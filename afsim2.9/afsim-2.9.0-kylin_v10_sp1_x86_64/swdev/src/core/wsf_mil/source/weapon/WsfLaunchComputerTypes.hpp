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

#ifndef WSFLAUNCHCOMPUTERTYPES_HPP
#define WSFLAUNCHCOMPUTERTYPES_HPP

#include "wsf_mil_export.h"

#include "WsfLaunchComputer.hpp"
#include "WsfObjectTypeList.hpp"

class WsfLaunchComputerTypes : public WsfObjectTypeList<WsfLaunchComputer>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_MIL_EXPORT WsfLaunchComputerTypes&       Get(WsfScenario& aScenario);
   static WSF_MIL_EXPORT const WsfLaunchComputerTypes& Get(const WsfScenario& aScenario);
   //@}

   WsfLaunchComputerTypes(WsfScenario& aScenario);
   bool LoadInstance(UtInput& aInput, WsfWeapon* aWeaponPtr, bool aAddInstance) const;
};

#endif
