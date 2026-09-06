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

#include "WsfIntersectMeshTypes.hpp"

#include <sstream>

#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"

namespace
{
//! Component factory to process platform input.
class IntersectMeshComponentFactory : public WsfComponentFactory<WsfPlatform>
{
public:
   bool ProcessAddOrEditCommand(UtInput& aInput, WsfPlatform& aPlatform, bool aIsAdding) override
   {
      WsfIntersectMeshTypes& types(WsfIntersectMeshTypes::Get(GetScenario()));
      return types.LoadUnnamedComponentWithoutEdit(aInput, aPlatform, aIsAdding, cCOMPONENT_ROLE<WsfIntersectMesh>());
   }

   bool ProcessDeleteCommand(UtInput& aInput, WsfPlatform& aPlatform) override
   {
      WsfIntersectMeshTypes& types(WsfIntersectMeshTypes::Get(GetScenario()));
      return types.DeleteUnnamedComponent(aInput, aPlatform, cCOMPONENT_ROLE<WsfIntersectMesh>());
   }
};
} // namespace

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfIntersectMeshTypes& WsfIntersectMeshTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetIntersectMeshTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfIntersectMeshTypes& WsfIntersectMeshTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetIntersectMeshTypes();
}

// =================================================================================================
WsfIntersectMeshTypes::WsfIntersectMeshTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfIntersectMesh>(aScenario, cREDEFINITION_ALLOWED, "intersect_mesh")
{
   SetSingularBaseType();
   aScenario.RegisterComponentFactory(ut::make_unique<IntersectMeshComponentFactory>()); // Allows for definition inside
   // platform, platform_type blocks.

   Add("WSF_INTERSECT_MESH", ut::make_unique<WsfIntersectMesh>()); // Dummy type "WSF_INTERSECT_MESH" is not
   // explicitly referenced in the input.
}
