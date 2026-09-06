// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef FIRES_MOVER_HPP
#define FIRES_MOVER_HPP

#include <map>

#include "FiresPath.hpp"
#include "FiresTable.hpp"
#include "UtEntity.hpp"
class UtInput;
#include "WsfDraw.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfMover.hpp"

namespace Fires
{
//! The FiresMover (WSF_FIRES_MOVER) provides movement along a ballistic trajectory that assumes a first-order drag
//! (drag proportional to the first power of the velocity). The resulting trajectories are deterministic and
//! characterized by a single parameter. This simplicity allows one to easily hit a location or target with a
//! realistic time-to-impact. Dispersion in azimuth and elevation are also modeled to support realisitc weapon effects.
class FiresMover : public WsfMover, public FiresPath
{
public:
   explicit FiresMover(WsfScenario& aScenario);
   FiresMover(const FiresMover& aSrc);
   ~FiresMover() override;

   FiresMover& operator=(const FiresMover& aSrc) = delete;

   WsfMover* Clone() const override;

   bool Initialize2(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;
   void Update(double aSimTime) override;

   WsfSpatialDomain GetSpatialDomain() const override { return WSF_SPATIAL_DOMAIN_AIR; }

private:
   double mAzimuthErrorExpected;
   double mElevationErrorExpected;

   void ComputeTargetRangeBearingDeltaAlt();
};
}; // namespace Fires

#endif
