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

#ifndef WSFZONESET_HPP
#define WSFZONESET_HPP

#include "wsf_export.h"

#include <vector>

class GeoIntersectDatabase;
class UtInput;
class WsfDraw;
class WsfGeoPoint;
#include "WsfZone.hpp"

namespace wsf
{
class Terrain;
}

//! An zone that is a set of zones.
//!
//! This zone construct represents the union of a collection of zones. A point
//! is considered to be in the zone if it is in any of the member zones.
class WSF_EXPORT WsfZoneSet : public WsfZone
{
public:
   WsfZoneSet();
   WsfZoneSet& operator=(const WsfZoneSet&) = delete;
   ~WsfZoneSet() override;

   WsfZone* Clone() const override;

   void Initialize(const WsfScenario& aScenario) override;
   bool ProcessInput(UtInput& aInput) override;

   bool PointIsInside(WsfSimulation* aSimulationPtr,
                      const double   aViewedPointWCS[3],
                      const double   aEyePointWCS[3],
                      const double   aLookHeading,
                      const double   aDeltaDownRange = 0.0) override;

   WsfGeoPoint Reference() override;

   WsfZone* GetZoneAtIndex(int aIndex);
   int      GetNumZones() const { return static_cast<int>(mZonePtrs.size()); }
   WsfZone* GetExclusionZoneAtIndex(const int aIndex);
   int      GetNumExclusionZones() const { return static_cast<int>(mExclusionZonePtrs.size()); }

   double CheckIntersections(const double aLLA1[3], const double aLLA2[3]) override;

   void DebugDrawZone(WsfDraw& aDraw) override;
   void DebugDrawCentroid(WsfDraw& aDraw) override;
   void DebugDrawBounds(WsfDraw& aDraw) override;

protected:
   //! Copy constructor (for Clone()).
   WsfZoneSet(const WsfZoneSet& aSrc);

   bool ConstructZoneDatabase();

private:
   void CalculateBoundingBox();
   void CalculateArea();
   void CalculateCentroid();

   std::vector<WsfZone*> mZonePtrs;
   std::vector<WsfZone*> mExclusionZonePtrs;
   GeoIntersectDatabase* mIntersectDBPtr;

   // the following set of variables are used to reconstruct the intersect database as needed
   WsfStringId   mAttenuationFileName;
   bool          mUseDTED;
   WsfStringId   mHeightParameter;
   double        mConstHeight;
   WsfStringId   mBaseAltitudeParameter;
   double        mConstBaseAltitude;
   bool          mGeocentric;
   bool          mBaseAltitiudeIsAGL;
   wsf::Terrain* mTerrainPtr;
};

#endif
