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

#ifndef WSFGEOPOINT_HPP
#define WSFGEOPOINT_HPP

#include "wsf_export.h"

class UtCalendar;
#include "UtEntity.hpp"
class UtInput;
#include "UtMemory.hpp"
class WsfPlatform;
class WsfScenario;
#include "WsfSimpleComponent.hpp"

//! A representation of a geographical point.
//! A geo point represents a point on the earth.  The geo point interface accommodates both
//! world coordinates (WCS) and (lat, lon, alt) systems.
class WSF_EXPORT WsfGeoPoint : public WsfSimplePlatformComponent
{
public:
   static void RegisterComponentFactory(WsfScenario& aScenario);

   WsfGeoPoint();
   explicit WsfGeoPoint(const double aPointWCS[3]);
   WsfGeoPoint(double aLat, double aLon, double aAlt);
   WsfGeoPoint(const WsfGeoPoint& aSrc);
   WsfGeoPoint& operator   =(const WsfGeoPoint&);
   ~WsfGeoPoint() override = default;

   //! @name Component infrastructure methods
   //@{
   WsfComponent* CloneComponent() const override { return Clone(); }
   void*         QueryInterface(int aRole) override { return (aRole == mRoles[0]) ? this : nullptr; }
   //@}

   virtual WsfGeoPoint* Clone() const { return new WsfGeoPoint(*this); }

   bool ProcessInput(UtInput& aInput) override;

   void GetLocationWCS(double aLocationWCS[3]) const;

   double GetDistanceFrom(double aLocationWCS[3]) const;

   double GetDistanceFrom(const WsfGeoPoint& aRefPoint) const;

   void SetLocationWCS(const double aLocationWCS[3]);

   void GetLocationECI(const UtCalendar& aTime, double aLocationECI[3]) const;

   double GetLat() const;

   double GetLon() const;

   double GetAlt() const;

   void GetLocationLLA(double& aLat, double& aLon, double& aAlt) const;

   void SetLocationLLA(double aLat, double aLon, double aAlt);

   void AddOffset(double aHeading, const double aOffset[3]);

   void AddOffsetNED(const double aOffsetNED[3]);

   void Extrapolate(double aHeading, double aDistance);

   void SetAltitudeMSL(double aAltitude);

   double TrueBearingTo(const WsfGeoPoint& aRhs) const;

   double GroundRangeTo(const WsfGeoPoint& aRhs) const;

   double GetApparentTime(const UtCalendar& aEpoch) const;

   double SlantRangeTo(const WsfGeoPoint& aRhs, const bool aUseSpherical = true) const;

   //! For XIO (de)serialization.
   template<typename AR>
   void Serialize(AR& aAr)
   {
      aAr& mRefPointWCS[0] & mRefPointWCS[1] & mRefPointWCS[2] & mRefLat& mRefLon& mRefAlt;
   }

private:
   UtEntity&              GetReferenceEntity() const;
   void                   UpdateLLA_Location() const;
   void                   UpdateWCS_Location() const;
   const ut::CentralBody& GetCentralBody() const;

   mutable UtVec3d mRefPointWCS;
   mutable double  mRefLat{0.0};
   mutable double  mRefLon{0.0};
   mutable double  mRefAlt{0.0};
   mutable bool    mWCS_LocationValid{false};
   mutable bool    mLLA_LocationValid{true};

   mutable std::unique_ptr<UtEntity>        mEntityPtr{nullptr};
   mutable std::unique_ptr<ut::CentralBody> mCentralBodyPtr{nullptr};
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfGeoPoint, cWSF_COMPONENT_GEOPOINT)

#endif
