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

#ifndef WSFTHERMALSYSTEMTYPES_HPP
#define WSFTHERMALSYSTEMTYPES_HPP

#include "wsf_export.h"

#include "WsfObjectTypeList.hpp"
#include "WsfThermalSystem.hpp"

class WsfThermalSystemTypes : public WsfObjectTypeList<WsfThermalSystem>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_EXPORT WsfThermalSystemTypes&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const WsfThermalSystemTypes& Get(const WsfScenario& aScenario);
   //@}

   WsfThermalSystemTypes(WsfScenario& aScenario);
};

#endif
