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

#include "WsfMoverTypes.hpp"

#include "UtMemory.hpp"
#include "WsfAirMover.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfGroundMover.hpp"
#include "WsfHybridMover.hpp"
#include "WsfMath3D_Mover.hpp"
#include "WsfOffsetMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfRoadMover.hpp"
#include "WsfRotorcraftMover.hpp"
#include "WsfScenario.hpp"
#include "WsfSurfaceMover.hpp"
#include "WsfTSPI_Mover.hpp"

namespace
{
//! Component factory to process platform input.
class MoverComponentFactory : public WsfComponentFactory<WsfPlatform>
{
public:
   bool ProcessAddOrEditCommand(UtInput& aInput, WsfPlatform& aPlatform, bool aIsAdding) override
   {
      WsfMoverTypes& types(WsfMoverTypes::Get(GetScenario()));
      return types.LoadUnnamedComponent(aInput, aPlatform, aIsAdding, cCOMPONENT_ROLE<WsfMover>());
   }

   bool ProcessDeleteCommand(UtInput& aInput, WsfPlatform& aPlatform) override
   {
      WsfMoverTypes& types(WsfMoverTypes::Get(GetScenario()));
      return types.DeleteUnnamedComponent(aInput, aPlatform, cCOMPONENT_ROLE<WsfMover>());
   }
};
} // namespace

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfMoverTypes& WsfMoverTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetMoverTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfMoverTypes& WsfMoverTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetMoverTypes();
}

// =================================================================================================
WsfMoverTypes::WsfMoverTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfMover>(aScenario, "mover")
{
   aScenario.RegisterComponentFactory(ut::make_unique<MoverComponentFactory>());

#ifndef WSF_BUILD_NO_MOVERS
   // Create 'type' entries for the built-in types.
   Add("WSF_AIR_MOVER", ut::make_unique<WsfAirMover>(aScenario));
   Add("WSF_GROUND_MOVER", ut::make_unique<WsfGroundMover>(aScenario));
   Add("WSF_HYBRID_MOVER", ut::make_unique<WsfHybridMover>(aScenario));
   Add("WSF_KINEMATIC_MOVER", ut::make_unique<WsfMath3D_Mover>(aScenario));
   Add("WSF_OFFSET_MOVER", ut::make_unique<WsfOffsetMover>(aScenario));
   Add("WSF_ROAD_MOVER", ut::make_unique<WsfRoadMover>(aScenario));
   Add("WSF_ROTORCRAFT_MOVER", ut::make_unique<WsfRotorcraftMover>(aScenario));
   Add("WSF_SURFACE_MOVER", ut::make_unique<WsfSurfaceMover>(aScenario));
   Add("WSF_TSPI_MOVER", ut::make_unique<WsfTSPI_Mover>(aScenario));

#endif
}
