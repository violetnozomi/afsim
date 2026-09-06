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

#ifndef WSFCALLBACKTYPES_HPP
#define WSFCALLBACKTYPES_HPP

#include "WsfCallback.hpp"
#include "WsfObjectTypeList.hpp"

class WsfCallbackTypes : public WsfObjectTypeList<WsfCallback>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_EXPORT WsfCallbackTypes&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const WsfCallbackTypes& Get(const WsfScenario& aScenario);
   //@}

   WsfCallbackTypes(WsfScenario& aScenario);
   LoadResult LoadType(UtInput& aInput) override;
   bool       LoadInstance(UtInput& aInput, WsfPlatform& aPlatform);
};

#endif
