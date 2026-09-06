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

#ifndef WSFNORAD_SPACEMOVER_HPP
#define WSFNORAD_SPACEMOVER_HPP

#include "wsf_space_export.h"

#include <memory>

#include "WsfNORAD_OrbitalPropagator.hpp"
#include "WsfSpaceMoverBase.hpp"
#include "WsfWaypoint.hpp"

//! WsfNORAD_SpaceMover is a specialization of WsfMover for earth-orbiting satellites.
class WSF_SPACE_EXPORT WsfNORAD_SpaceMover : public WsfSpaceMoverBase
{
public:
   explicit WsfNORAD_SpaceMover(const WsfScenario& aScenario);
   WsfNORAD_SpaceMover(const WsfNORAD_SpaceMover& aSrc);
   WsfNORAD_SpaceMover& operator=(const WsfNORAD_SpaceMover&) = delete;
   ~WsfNORAD_SpaceMover() override                            = default;

   //! @name Common framework methods.
   //@{
   WsfMover* Clone() const override;
   bool      ProcessInput(UtInput& aInput) override;
   bool      Initialize(double aSimTime) override;
   //@}

   bool SetInitialPoint(double aLat, double aLon, double aAlt, double aSpeed, double aHeading) override;

private:
   void ComputeInitialFromWaypoint(const UtCalendar& aEpoch, double aLocECI[3], double aVelECI[3]) const;

   std::unique_ptr<WsfWaypoint> mWaypointPtr;
};

#endif
