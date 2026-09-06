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

#ifndef WSFZONEREFERENCE_HPP
#define WSFZONEREFERENCE_HPP

#include "wsf_export.h"

#include <string>

class WsfGeoPoint;
#include "WsfStringId.hpp"
#include "WsfZone.hpp"

//! A reference (link) to a shared zone.
//!
//! This class provides a mechanism to reference a shared zone.
//! The referenced zone is expected to be static so the pointer is always valid.
//! Any number of instances of this class may go in and
//! out of scope, referring to the same Zone, but not affecting it in any way.
class WSF_EXPORT WsfZoneReference : public WsfZone
{
public:
   WsfZoneReference() = delete;
   WsfZoneReference(const std::string& aSharedZoneName);
   WsfZoneReference& operator=(const WsfZoneReference&) = delete;
   ~WsfZoneReference() override                         = default;

   WsfZone* Clone() const override;

   void Initialize(const WsfScenario& aScenario) override;

   bool PointIsInside(WsfSimulation* aSimulationPtr,
                      const double   aViewedPointWCS[3],
                      const double   aEyePointWCS[3],
                      const double   aLookHeading,
                      const double   aDeltaDownRange = 0.0) override;

   double Area() override;

   WsfGeoPoint Reference() override;
   WsfGeoPoint Centroid() override;

   void MaxPoint(double& aX, double& aY, double& aAlt) override;
   void MinPoint(double& aX, double& aY, double& aAlt) override;

   void DebugDrawZone(WsfDraw& aDraw) override;
   void DebugDrawCentroid(WsfDraw& aDraw) override;
   void DebugDrawBounds(WsfDraw& aDraw) override;

   ut::optional<UtColor> GetFillColor() const override;
   ut::optional<UtColor> GetLineColor() const override;

   //! Get the modifier list for WsfZoneAttenuation.
   const std::map<WsfStringId, double>& GetModifierList() const override;

   //! returns the value of the modifier matching the given name
   double ModifierValue(const std::string& aModifierName) const override;

   WsfStringId GetReferencedZoneName() const { return mSharedZoneName; }
   WsfZone*    GetReferencedZone() const { return mSharedZonePtr; }

protected:
   WsfZoneReference(const WsfZoneReference& aSrc);

private:
   WsfStringId mSharedZoneName;
   WsfZone*    mSharedZonePtr;
};

#endif
