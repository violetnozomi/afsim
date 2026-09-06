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

#ifndef WSFGROUNDMOVER_HPP
#define WSFGROUNDMOVER_HPP

#include "wsf_export.h"

#include "WsfWaypointMover.hpp"

//! WsfGroundMover is a specialization of WsfWaypointMover for ground vehicles.

class WSF_EXPORT WsfGroundMover : public WsfWaypointMover
{
public:
   WsfGroundMover() = delete;
   WsfGroundMover(const WsfScenario& aScenario);
   ~WsfGroundMover() override;

   WsfMover* Clone() const override;
   bool      ProcessInput(UtInput& aInput) override;

   WsfSpatialDomain GetSpatialDomain() const override { return WSF_SPATIAL_DOMAIN_LAND; }

protected:
   WsfGroundMover(const WsfGroundMover& aSrc);
};

#endif
