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

// ================================================================================================
// ================================================================================================
// PROGRAMMING NOTE: See the programming notes in WsfZone.cpp about the use of Initialize()!!!!
// ================================================================================================
// ================================================================================================

#include "WsfZoneReference.hpp"

#include "UtLog.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfZoneDefinition.hpp"
#include "WsfZoneTypes.hpp"

// =================================================================================================
//! Constructor
//! @param aSharedZoneName The name of the shared zone that will be referenced.
WsfZoneReference::WsfZoneReference(const std::string& aSharedZoneName)
   : WsfZone()
   , mSharedZoneName(aSharedZoneName)
   , mSharedZonePtr(nullptr)
{
}

// =================================================================================================
//! Copy constructor.
WsfZoneReference::WsfZoneReference(const WsfZoneReference& aSrc)
   : WsfZone(aSrc)
   , mSharedZoneName(aSrc.mSharedZoneName)
   , mSharedZonePtr(aSrc.mSharedZonePtr)
{
}


// =================================================================================================
// virtual
WsfZone* WsfZoneReference::Clone() const
{
   return new WsfZoneReference(*this);
}

// =================================================================================================
// virtual
bool WsfZoneReference::PointIsInside(WsfSimulation* aSimulationPtr,
                                     const double   aViewedPointWCS[3],
                                     const double   aEyePointWCS[3],
                                     const double   aLookHeading,
                                     const double   aDeltaDownRange)
{
   return mSharedZonePtr->PointIsInside(aSimulationPtr, aViewedPointWCS, aEyePointWCS, aLookHeading, aDeltaDownRange);
}

// =================================================================================================
//! return the geopoint of the reference point
// virtual
WsfGeoPoint WsfZoneReference::Reference()
{
   return mSharedZonePtr->Reference();
}

// =================================================================================================
//! return the area of the polygon
// virtual
double WsfZoneReference::Area()
{
   return mSharedZonePtr->Area();
}

// =================================================================================================
//! return the geopoint of the centroid
// virtual
WsfGeoPoint WsfZoneReference::Centroid()
{
   return mSharedZonePtr->Centroid();
}

// =================================================================================================
// virtual
void WsfZoneReference::MaxPoint(double& aX, double& aY, double& aAlt)
{
   mSharedZonePtr->MaxPoint(aX, aY, aAlt);
}

// =================================================================================================
// virtual
void WsfZoneReference::MinPoint(double& aX, double& aY, double& aAlt)
{
   mSharedZonePtr->MinPoint(aX, aY, aAlt);
}

// =================================================================================================
//! returns the list of modifiers
// virtual
const std::map<WsfStringId, double>& WsfZoneReference::GetModifierList() const
{
   return mSharedZonePtr->GetModifierList();
}

// =================================================================================================
//! returns the value of the modifier matching the given name
// virtual
double WsfZoneReference::ModifierValue(const std::string& aModifierName) const
{
   return mSharedZonePtr->ModifierValue(aModifierName);
}


// =================================================================================================
//! draw the zone using WsfDraw
// virtual
void WsfZoneReference::DebugDrawZone(WsfDraw& aDraw)
{
   mSharedZonePtr->DebugDrawZone(aDraw);
}

// =================================================================================================
//! draw the centroid using WsfDraw in an x
// virtual
void WsfZoneReference::DebugDrawCentroid(WsfDraw& aDraw)
{
   mSharedZonePtr->DebugDrawCentroid(aDraw);
}

// =================================================================================================
//! draw the bounding box using WsfDraw
// virtual
void WsfZoneReference::DebugDrawBounds(WsfDraw& aDraw)
{
   mSharedZonePtr->DebugDrawBounds(aDraw);
}

// =================================================================================================
// virtual
void WsfZoneReference::Initialize(const WsfScenario& aScenario)
{
   if (mIsInitialized)
   {
      return;
   }
   WsfZone::Initialize(aScenario);

   if (mSharedZonePtr == nullptr)
   {
      mSharedZonePtr = WsfZoneTypes::Get(aScenario).Find(mSharedZoneName);
      if (mSharedZonePtr == nullptr)
      {
         //! An empty zone to be used if the referenced zone does not exist
         WsfZoneDefinition sEmptyZone;

         auto out = ut::log::error() << "Referenced global zone does not exist.";
         out.AddNote() << "Zone: " << mSharedZoneName;
         sEmptyZone.SetShapeType(WsfZoneDefinition::cCIRCULAR);
         sEmptyZone.SetRadialBounds(0.0, 1.0E-30);
         mSharedZonePtr = &sEmptyZone;
      }
   }
}

ut::optional<UtColor> WsfZoneReference::GetFillColor() const
{
   return mSharedZonePtr->GetFillColor();
}

ut::optional<UtColor> WsfZoneReference::GetLineColor() const
{
   return mSharedZonePtr->GetLineColor();
}
