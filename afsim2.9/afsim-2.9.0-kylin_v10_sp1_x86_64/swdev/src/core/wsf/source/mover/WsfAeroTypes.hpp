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

#ifndef WSFAEROTYPES_HPP
#define WSFAEROTYPES_HPP

#include "wsf_export.h"

#include "WsfAero.hpp"
#include "WsfObjectTypeList.hpp"

class WsfAeroTypes : public WsfObjectTypeList<WsfAero>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_EXPORT WsfAeroTypes&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const WsfAeroTypes& Get(const WsfScenario& aScenario);
   //@}

   WsfAeroTypes(WsfScenario& aScenario);
};

#endif
