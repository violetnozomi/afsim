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

#include "WsfVisualPartTypes.hpp"

#include "UtMemory.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"

namespace
{
//! Component factory to process platform input.
class VisualPartComponentFactory : public WsfComponentFactory<WsfPlatform>
{
public:
   bool ProcessAddOrEditCommand(UtInput& aInput, WsfPlatform& aPlatform, bool aIsAdding) override
   {
      WsfVisualPartTypes& types(WsfVisualPartTypes::Get(GetScenario()));
      return types.LoadNamedComponent(aInput, aPlatform, aIsAdding, cCOMPONENT_ROLE<WsfVisualPart>());
   }

   bool ProcessDeleteCommand(UtInput& aInput, WsfPlatform& aPlatform) override
   {
      WsfVisualPartTypes& types(WsfVisualPartTypes::Get(GetScenario()));
      return types.DeleteNamedComponent(aInput, aPlatform, cCOMPONENT_ROLE<WsfVisualPart>());
   }
};
} // namespace

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfVisualPartTypes& WsfVisualPartTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetVisualPartTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfVisualPartTypes& WsfVisualPartTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetVisualPartTypes();
}

// =================================================================================================
WsfVisualPartTypes::WsfVisualPartTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfVisualPart>(aScenario, "visual_part")
{
   aScenario.RegisterComponentFactory(ut::make_unique<VisualPartComponentFactory>());

   AddCoreType("WSF_VISUAL_PART", ut::make_unique<WsfVisualPart>(aScenario));
}
