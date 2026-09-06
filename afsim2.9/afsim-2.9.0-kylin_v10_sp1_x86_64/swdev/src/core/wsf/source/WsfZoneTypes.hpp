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

#ifndef WSFZONETYPES_HPP
#define WSFZONETYPES_HPP

#include "WsfObjectTypeList.hpp"
#include "WsfZone.hpp"

class WsfZoneTypes : public WsfObjectTypeList<WsfZone>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_EXPORT WsfZoneTypes&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const WsfZoneTypes& Get(const WsfScenario& aScenario);
   //@}

   WsfZoneTypes(WsfScenario& aScenario);
   WsfZone* ProcessZoneSetInput(UtInput& aInput);
   WsfZone* ProcessZoneInput(UtInput& aInput);
   bool     LoadInstance(UtInput& aInput, WsfPlatform* aPlatformPtr);

   void InitializeZones(WsfScenario& aScenario);

   LoadResult LoadType(UtInput& aInput) override;
};

#endif
