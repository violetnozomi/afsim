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

#ifndef WSFGROUPTYPES_HPP
#define WSFGROUPTYPES_HPP

#include "WsfGroup.hpp"
#include "WsfObjectTypeList.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformPart.hpp"

class WsfGroupTypes : public WsfObjectTypeList<WsfGroup>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_EXPORT WsfGroupTypes&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const WsfGroupTypes& Get(const WsfScenario& aScenario);
   //@}

   WsfGroupTypes(WsfScenario& aScenario);

   bool LoadType(UtInput& aInput, WsfPlatformPart* aPlatformPartPtr);
   bool LoadType(UtInput& aInput, WsfPlatform* aPlatformPtr);

   LoadResult LoadType(UtInput& aInput) override;

   bool JoinGroupType(const std::string& aGroupType, const std::string& aGroupName, WsfPlatform* aPlatformPtr) const;

   bool JoinGroupType(const std::string& aGroupType, const std::string& aGroupName, WsfPlatformPart* aPlatformPartPtr) const;

private:
};

#endif
