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

#ifndef WSFEW_EA_EP_TYPES_HPP
#define WSFEW_EA_EP_TYPES_HPP

#include "WsfEW_EA_EP.hpp"
#include "WsfObjectTypeList.hpp"

class WsfEW_EA_EP_Types : public WsfObjectTypeList<WsfEW_EA_EP>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_MIL_EXPORT WsfEW_EA_EP_Types&       Get(WsfScenario& aScenario);
   static WSF_MIL_EXPORT const WsfEW_EA_EP_Types& Get(const WsfScenario& aScenario);
   //@}

   explicit WsfEW_EA_EP_Types(WsfScenario& aScenario);
};

#endif
