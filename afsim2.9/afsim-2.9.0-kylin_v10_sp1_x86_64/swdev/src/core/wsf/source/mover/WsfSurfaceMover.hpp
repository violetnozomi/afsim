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

#ifndef WSFSURFACEMOVER_HPP
#define WSFSURFACEMOVER_HPP

#include "wsf_export.h"

#include "WsfWaypointMover.hpp"

//! WsfSurfaceMover is a specialization of WsfWaypointMover for sea-borne vehicles.
class WSF_EXPORT WsfSurfaceMover : public WsfWaypointMover
{
public:
   WsfSurfaceMover() = delete;
   WsfSurfaceMover(const WsfScenario& aScenario);
   ~WsfSurfaceMover() override;

   //! @name Common framework methods
   //@{
   WsfMover* Clone() const override;
   bool      ProcessInput(UtInput& aInput) override;
   //@

   WsfSpatialDomain GetSpatialDomain() const override { return WSF_SPATIAL_DOMAIN_SURFACE; }

protected:
   WsfSurfaceMover(const WsfSurfaceMover& aSrc);
};

#endif
