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

#ifndef WSFWEAPONTYPES_HPP
#define WSFWEAPONTYPES_HPP

#include "wsf_mil_export.h"

#include "WsfObjectTypeList.hpp"
#include "WsfWeapon.hpp"

class WsfWeaponTypes : public WsfObjectTypeList<WsfWeapon>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_MIL_EXPORT WsfWeaponTypes&       Get(WsfScenario& aScenario);
   static WSF_MIL_EXPORT const WsfWeaponTypes& Get(const WsfScenario& aScenario);
   //@}

   WsfWeaponTypes(WsfScenario& aScenario);
};

#endif
