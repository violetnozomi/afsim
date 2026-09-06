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

#ifndef WSFSCRIPTGEOPOINTCLASS_HPP
#define WSFSCRIPTGEOPOINTCLASS_HPP

#include "wsf_export.h"

#include "UtScriptBasicTypes.hpp"


//! Script methods for WsfGeoPoint
class WSF_EXPORT WsfScriptGeoPointClass : public UtScriptClass
{
public:
   WsfScriptGeoPointClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   std::string ToString(void* aObjectPtr) const override;
   std::string ToStringMGRS(void* aObjectPtr) const;

   UT_DECLARE_SCRIPT_METHOD(Construct);      // Construct(double aLat, double aLon, double aAlt)
   UT_DECLARE_SCRIPT_METHOD(Construct_2);    // Construct(string aLLA_String)
   UT_DECLARE_SCRIPT_METHOD(ConstructWCS_1); // Construct(double aX, double aY, double aZ)
   UT_DECLARE_SCRIPT_METHOD(ConstructWCS_2); // Construct(Vec3)
   UT_DECLARE_SCRIPT_METHOD(StaticTerrainHeight);

   UT_DECLARE_SCRIPT_METHOD(Latitude);
   UT_DECLARE_SCRIPT_METHOD(Longitude);
   UT_DECLARE_SCRIPT_METHOD(Altitude);
   UT_DECLARE_SCRIPT_METHOD(X);
   UT_DECLARE_SCRIPT_METHOD(Y);
   UT_DECLARE_SCRIPT_METHOD(Z);
   UT_DECLARE_SCRIPT_METHOD(LocationWCS);
   UT_DECLARE_SCRIPT_METHOD(LocationECI);
   UT_DECLARE_SCRIPT_METHOD(LocationMGRS);
   UT_DECLARE_SCRIPT_METHOD(HeightAboveEllipsoid);
   UT_DECLARE_SCRIPT_METHOD(GeoidHeight);
   UT_DECLARE_SCRIPT_METHOD(HeightAboveTerrain);
   UT_DECLARE_SCRIPT_METHOD(TerrainHeight);

   UT_DECLARE_SCRIPT_METHOD(Set_1);  // Set(double aLat, double aLon, double aAlt);
   UT_DECLARE_SCRIPT_METHOD(Set_2);  // Set(WsfGeoPoint aPoint);
   UT_DECLARE_SCRIPT_METHOD(Set_3);  // Set(WsfPlatform aPlatform)
   UT_DECLARE_SCRIPT_METHOD(Set_4);  // Set(WsfTrack aTrack)
   UT_DECLARE_SCRIPT_METHOD(Set_5);  // Set(WsfWaypoint aPoint)
   UT_DECLARE_SCRIPT_METHOD(SetWCS); // Set(Vec3)
   UT_DECLARE_SCRIPT_METHOD(SetAltitudeAGL);
   UT_DECLARE_SCRIPT_METHOD(SetAltitudeHAE);

   UT_DECLARE_SCRIPT_METHOD(SlantRangeTo);  // SlantRangeTo(WsfGeoPoint aPoint)
   UT_DECLARE_SCRIPT_METHOD(SlantRangeToS); // SlantRangeTo(WsfGeoPoint aPoint, bool aUseSpherical)
   UT_DECLARE_SCRIPT_METHOD(GroundRangeTo); // GroundRangeTo(WsfGeoPoint aPoint)
   UT_DECLARE_SCRIPT_METHOD(TrueBearingTo); // TrueBearingTo(WsfGeoPoint aPoint)
   UT_DECLARE_SCRIPT_METHOD(Offset_1);      // Offset(double aHeading, double aDeltaX, double aDeltaY, double aDeltaZ);
   UT_DECLARE_SCRIPT_METHOD(Offset_2);      // Offset(double aHeading, Vec3);
   UT_DECLARE_SCRIPT_METHOD(OffsetNED_1);   // OffsetNED(double aNorth, double aEast, double aDown);
   UT_DECLARE_SCRIPT_METHOD(OffsetNED_2);   // OffsetNED(Vec3 aDeltaNED);
   UT_DECLARE_SCRIPT_METHOD(OffsetRBE);     // OffsetRBE(double aRange, double aBearing, double aElevation);
   UT_DECLARE_SCRIPT_METHOD(OffsetWCS_1);   // OffsetWCS(double aDeltaX, double aDeltaY, double aDeltaZ);
   UT_DECLARE_SCRIPT_METHOD(OffsetWCS_2);   // OffsetWCS(Vec3 aDeltaXYZ);
   UT_DECLARE_SCRIPT_METHOD(Extrapolate);   // Extrapolate(double aHeading, double aDistance);

   UT_DECLARE_SCRIPT_METHOD(WithinZone);   // WithinZone(string aZoneName)
   UT_DECLARE_SCRIPT_METHOD(WithinZoneOf); // WithinZoneOf(WsfPlatform aPlatform, string aZoneName)

   UT_DECLARE_SCRIPT_METHOD(ApparentTimeNow);
   UT_DECLARE_SCRIPT_METHOD(ApparentTime);
};

#endif
