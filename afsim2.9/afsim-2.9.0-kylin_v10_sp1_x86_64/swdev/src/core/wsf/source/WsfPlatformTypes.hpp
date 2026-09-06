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

#ifndef WSFPLATFORMTYPES_HPP
#define WSFPLATFORMTYPES_HPP

#include "WsfObjectTypeList.hpp"
#include "WsfPlatform.hpp"

class WsfPlatformTypes : public WsfObjectTypeList<WsfPlatform>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_EXPORT WsfPlatformTypes&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const WsfPlatformTypes& Get(const WsfScenario& aScenario);
   //@}

   WsfPlatformTypes(WsfScenario& aScenario);
   LoadResult LoadType(UtInput& aInput) override;

   bool ProcessInput(UtInput& aInput) override;

   void CompleteLoad() override;
};

#endif
