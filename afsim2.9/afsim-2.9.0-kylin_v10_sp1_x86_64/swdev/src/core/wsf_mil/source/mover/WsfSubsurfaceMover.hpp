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

#ifndef WSFSUBSURFACEMOVER_HPP
#define WSFSUBSURFACEMOVER_HPP

#include "wsf_mil_export.h"

#include "WsfWaypointMover.hpp"

//! WsfSubsurfaceMover is a specialization of WsfNewWaypointMover for sea-borne, submersible vehicles
//! It provides a GetDepth() method to retrieve the depth below the surface, and a GetHeightOfSurface() method.
//! By default this mover will stop at the end of its route.
class WSF_MIL_EXPORT WsfSubsurfaceMover : public WsfWaypointMover
{
public:
   WsfSubsurfaceMover(WsfScenario& aScenario);
   ~WsfSubsurfaceMover() override = default;

   //! @name Common framework methods
   //@{
   WsfMover* Clone() const override;
   bool      Initialize(double aSimTime) override;
   //@}

   WsfSpatialDomain GetSpatialDomain() const override { return WSF_SPATIAL_DOMAIN_SUBSURFACE; }

   void PostMoveUpdate(double aSimTime) override;

protected:
   //! Return the Height of Surface of the point directly above the mover.
   double GetHeightOfSurface() const { return 0.0; }

   virtual void Dive();
   virtual void Surface();
};

#endif
