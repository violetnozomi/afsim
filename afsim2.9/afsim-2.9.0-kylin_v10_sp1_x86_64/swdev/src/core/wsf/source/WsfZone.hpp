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

#ifndef WSFZONE_HPP
#define WSFZONE_HPP

#include "wsf_export.h"

#include <map>
#include <vector>

#include "UtColor.hpp"
class UtInput;
class UtInputBlock;
#include "UtOptional.hpp"
#include "WsfAuxDataEnabled.hpp"
#include "WsfComponent.hpp"
class WsfDraw;
class WsfGeoPoint;
#include "WsfObject.hpp"
class WsfPlatform;
class WsfScenario;
class WsfSimulation;

//! An abstract base class for a zone.
//!
//! Zones represent an volume that is formed either by extruding a polygon or
//! other geometric shape in the vertical direction, or specifying a 3-D
//! spherical radius.  The key run-time function for a Zone is the determination
//! of whether a point of interest lies within the Zone or not.  A Zone must be
//! specified relative to some point of reference.  There are two possibilities --
//! relative to:
//! - 1) the Observer (who must specify his location and direction when
//! he makes the function call), or
//! - 2) an internally-specified reference position.  The internally-specified
//! location may be fixed or variable, but since this class will NOT be responsible
//! for tracking the movement of another entity, it is up to the owner of the Zone
//! to maintain currency of the reference values.
//!
//! One Zone may be created referencing another.  This will result in a geometric-
//! ally similar volume, but translated and (heading) rotated to another position.
//! If the original Zone was defined with a (Lat, Lon) polygon, the new Zone will
//! use (X, Y) relative offsets, but will NOT account for earth's curvature,
//! and so may contain vertical distortions as the size grows.
//!
//! A polygonal Zone must be defined in a clockwise fashion, as viewed from above.
//!
//! @note Every Zone must be Initialize()'d by the instantiator before use.
//! has been processed, or after all values have been set via accessors.
//!
//! @see WsfZoneDefinition
//! @see WsfZoneReference
//! @see WsfZoneSet
class WSF_EXPORT WsfZone : public WsfObject, public WsfPlatformComponent, public WsfAuxDataEnabled
{
public:
   static constexpr const char* cTYPE_KIND = "zone";

   static constexpr double cUNSPECIFIED = 1.0E20;
   static constexpr double cHUGE        = 9.9E20;
   static constexpr double cSMALL       = -9.9E20;

   struct Point
   {
      Point(double aX, double aY)
         : mX(aX)
         , mY(aY)
      {
      }

      double mX;
      double mY;
   };

   WsfZone();
   WsfZone& operator=(const WsfZone&) = delete;
   ~WsfZone() override                = default;

   //! @name Component infrastructure methods
   //@{
   WsfComponent* CloneComponent() const override { return Clone(); }
   int           GetComponentInitializationOrder() const override { return cWSF_INITIALIZE_ORDER_ZONE; }
   WsfStringId   GetComponentName() const override { return GetNameId(); }
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   void          ComponentParentChanged(WsfPlatform* aPlatformPtr) override { mPlatformPtr = aPlatformPtr; }
   //@}

   WsfPlatform* GetPlatform() const { return mPlatformPtr; }

   //! The standard Clone() method (virtual copy constructor).
   WsfZone* Clone() const override = 0;

   // Needed to resolve ambiguity between WsfObject and WsfComponent.
   bool ProcessInput(UtInput& aInput) override;

   bool         Initialize(double aSimTime) override;
   virtual void Initialize(const WsfScenario& aScenario);

   virtual bool PointIsInside(WsfSimulation* aSimulationPtr,
                              const double   aViewedPointWCS[3],
                              const double   aEyePointWCS[3],
                              const double   aLookHeading,
                              const double   aDeltaDownRange = 0.0);

   virtual double CheckIntersections(const double aLLA1[3], const double aLLA2[3]) { return 0.0; }

   virtual bool GetLatLonExtrema(double& aSouthernLat, double& aWesternLon, double& aNorthernLat, double& aEasternLon) const;

   //! Set the global model that this zone was cloned from.
   void SetGlobalName(WsfStringId aGlobalName) { mGlobalName = aGlobalName; }

   //! Get the name of the globe template that this zone derives from.
   //! @return The name of the template.  WsfStringId(0) if there was no
   //!         derivation.
   WsfStringId GetGlobalName() const { return mGlobalName; }

   virtual double Area();

   virtual WsfGeoPoint Reference() = 0;
   virtual WsfGeoPoint Centroid();

   virtual void MaxPoint(double& aX, double& aY, double& aAlt);
   virtual void MinPoint(double& aX, double& aY, double& aAlt);

   virtual void DrawBounds(WsfDraw& aDraw) {}
   virtual void DrawCentroid(WsfDraw& aDraw) {}
   virtual void Draw(WsfDraw& aDraw) {}
   virtual void DrawRelative(WsfDraw& aDraw, const double aEyePointWCS[3], const double aLookHeading) {}

   virtual void DebugDrawBounds(WsfDraw& aDraw) {}
   virtual void DebugDrawCentroid(WsfDraw& aDraw) {}
   virtual void DebugDrawZone(WsfDraw& aDraw) {}
   virtual void DebugDrawZoneRelative(WsfDraw& aDraw, const double aEyePointWCS[3], const double aLookHeading) {}

   //! Get the modifier list for WsfZoneAttenuation.
   virtual const std::map<WsfStringId, double>& GetModifierList() const { return mModifierList; }

   //! returns the value of the modifier matching the given name
   virtual double ModifierValue(const std::string& aModifierName) const;

   bool IsInitialized() const { return mIsInitialized; }

   void SetFillColor(const std::string& aColor);
   void SetFillColor(const UtColor& aColor) { mFillColor = aColor; }
   void SetLineColor(const std::string& aColor);
   void SetLineColor(const UtColor& aColor) { mLineColor = aColor; }

   virtual ut::optional<UtColor> GetFillColor() const { return mFillColor; }
   virtual ut::optional<UtColor> GetLineColor() const { return mLineColor; }

protected:
   //! Copy constructor (for Clone()).
   WsfZone(const WsfZone& aSrc);

   WsfPlatform* mPlatformPtr;

   std::map<WsfStringId, double> mModifierList;

   // bounding box coordinates
   double mMinAlt; // Bottom of the Zone (meters)
   double mMaxAlt; // Top    of the Zone (meters)
   double mMinX;
   double mMaxX;
   double mMinY;
   double mMaxY;

   double mArea;
   Point  mCentroid;

   bool mIsInitialized;

private:
   WsfStringId           mGlobalName;
   ut::optional<UtColor> mFillColor;
   ut::optional<UtColor> mLineColor;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfZone, cWSF_COMPONENT_ZONE)

#endif
