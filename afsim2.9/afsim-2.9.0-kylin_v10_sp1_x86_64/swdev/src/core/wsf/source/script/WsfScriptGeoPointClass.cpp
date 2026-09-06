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

#include "script/WsfScriptGeoPointClass.hpp"

#include <iomanip>
#include <sstream>

#include "UtEllipsoidalEarth.hpp"
#include "UtLatPos.hpp"
#include "UtLonPos.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtScriptVec3.hpp"
#include "UtUnitTypes.hpp"
#include "WsfEarthGravityModel.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"
#include "WsfTrack.hpp"
#include "WsfWaypoint.hpp"
#include "WsfZone.hpp"
#include "WsfZoneTypes.hpp"
#include "script/WsfScriptContext.hpp"

WsfScriptGeoPointClass::WsfScriptGeoPointClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfGeoPoint");
   mConstructible = true;
   mCloneable     = true;

   // AddMethod(ut::make_unique<ToString>());

   AddStaticMethod(ut::make_unique<Construct>());                    // Construct(double aLat, double aLon, double aAlt)
   AddStaticMethod(ut::make_unique<Construct_2>("Construct"));       // Construct(string aLLA_String)
   AddStaticMethod(ut::make_unique<ConstructWCS_1>("ConstructWCS")); // Construct(double aX, double aY, double aZ)
   AddStaticMethod(ut::make_unique<ConstructWCS_2>("ConstructWCS")); // Construct(Vec3)
   AddStaticMethod(ut::make_unique<StaticTerrainHeight>("TerrainHeight")); // TerrainHeight(double aLat, double aLon);

   AddMethod(ut::make_unique<Latitude>());
   AddMethod(ut::make_unique<Longitude>());
   AddMethod(ut::make_unique<Altitude>());
   AddMethod(ut::make_unique<X>());
   AddMethod(ut::make_unique<Y>());
   AddMethod(ut::make_unique<Z>());
   AddMethod(ut::make_unique<LocationWCS>());
   AddMethod(ut::make_unique<LocationECI>());
   AddMethod(ut::make_unique<LocationMGRS>());
   AddMethod(ut::make_unique<HeightAboveEllipsoid>());
   AddMethod(ut::make_unique<GeoidHeight>());
   AddMethod(ut::make_unique<HeightAboveTerrain>());
   AddMethod(ut::make_unique<TerrainHeight>());

   AddMethod(ut::make_unique<Set_1>("Set"));  // Set(WsfGeoPoint aPoint)
   AddMethod(ut::make_unique<Set_2>("Set"));  // Set(double aLat, double aLon, double aAlt)
   AddMethod(ut::make_unique<Set_3>("Set"));  // Set(WsfPlatform aPlatform)
   AddMethod(ut::make_unique<Set_4>("Set"));  // Set(WsfTrack aTrack)
   AddMethod(ut::make_unique<Set_5>("Set"));  // Set(WsfWaypoint aPoint)
   AddMethod(ut::make_unique<SetWCS>("Set")); // Set(Vec3 aPoint)
   AddMethod(ut::make_unique<SetWCS>());      // Set(Vec3 aPoint)
   AddMethod(ut::make_unique<SetAltitudeAGL>());
   AddMethod(ut::make_unique<SetAltitudeHAE>());

   AddMethod(ut::make_unique<SlantRangeTo>());                // SlantRangeTo(WsfGeoPoint aPoint)
   AddMethod(ut::make_unique<SlantRangeToS>("SlantRangeTo")); // SlantRangeTo(WsfGeoPoint aPoint, bool aUseSpherical)
   AddMethod(ut::make_unique<GroundRangeTo>());               // GroundRangeTo(WsfGeoPoint aPoint);
   AddMethod(ut::make_unique<TrueBearingTo>());               // TrueBearingTo(WsfGeoPoint aPoint);
   AddMethod(ut::make_unique<Offset_1>("Offset")); // Offset(double aHeading, double aDeltaX, double aDeltaY, double aDeltaZ);
   AddMethod(ut::make_unique<Offset_2>("Offset")); // Offset(double aHeading, double aDeltaX, double aDeltaY, double aDeltaZ);
   AddMethod(ut::make_unique<OffsetNED_1>("OffsetNED"));
   AddMethod(ut::make_unique<OffsetNED_2>("OffsetNED"));
   AddMethod(ut::make_unique<OffsetRBE>());
   AddMethod(ut::make_unique<OffsetWCS_1>("OffsetWCS")); // OffsetWCS(double aDeltaX, double aDeltaY, double aDeltaZ);
   AddMethod(ut::make_unique<OffsetWCS_2>("OffsetWCS")); // OffsetWCS(double aDeltaX, double aDeltaY, double aDeltaZ);
   AddMethod(ut::make_unique<Extrapolate>());            // Extrapolate(double aHeading, double aDistance);

   AddMethod(ut::make_unique<WithinZone>());   // WithinZone(string aZoneName)
   AddMethod(ut::make_unique<WithinZoneOf>()); // WithinZoneOf(WsfPlatform aPlatform, string aZoneName)

   AddMethod(ut::make_unique<ApparentTimeNow>());
   AddMethod(ut::make_unique<ApparentTime>());
}

// virtual
void* WsfScriptGeoPointClass::Create(const UtScriptContext& /*aContext*/)
{
   return new WsfGeoPoint();
}

// virtual
void* WsfScriptGeoPointClass::Clone(void* aObjectPtr)
{
   return static_cast<WsfGeoPoint*>(aObjectPtr)->Clone();
}

// virtual
void WsfScriptGeoPointClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfGeoPoint*>(aObjectPtr);
}

std::string WsfScriptGeoPointClass::ToString(void* aObjectPtr) const
{
   std::stringstream ss;
   WsfGeoPoint*      geoPointPtr = (WsfGeoPoint*)aObjectPtr;
   if (geoPointPtr != nullptr)
   {
      ss << UtLatPos(geoPointPtr->GetLat()) << ' ' << UtLonPos(geoPointPtr->GetLon()) << ' ' << std::setprecision(8)
         << geoPointPtr->GetAlt();
   }
   else
   {
      ss << "WsfGeoPoint(null)";
   }
   return ss.str();
}

std::string WsfScriptGeoPointClass::ToStringMGRS(void* aObjectPtr) const
{
   std::stringstream ss;
   WsfGeoPoint*      geoPointPtr = (WsfGeoPoint*)aObjectPtr;
   if (geoPointPtr != nullptr)
   {
      std::string mgrsLoc;
      UtEllipsoidalEarth::ConvertLLToMGRS(geoPointPtr->GetLat(), geoPointPtr->GetLon(), mgrsLoc);
      ss << mgrsLoc << ' ' << std::setprecision(8) << geoPointPtr->GetAlt();
   }
   else
   {
      ss << "WsfGeoPoint(null)";
   }
   return ss.str();
}

#include "script/WsfScriptDefs.hpp"

UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, Latitude, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetLat());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, Longitude, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetLon());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, Altitude, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetAlt());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, X, 0, "double", "")
{
   double locationWCS[3];
   aObjectPtr->GetLocationWCS(locationWCS);
   aReturnVal.SetDouble(locationWCS[0]);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, Y, 0, "double", "")
{
   double locationWCS[3];
   aObjectPtr->GetLocationWCS(locationWCS);
   aReturnVal.SetDouble(locationWCS[1]);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, Z, 0, "double", "")
{
   double locationWCS[3];
   aObjectPtr->GetLocationWCS(locationWCS);
   aReturnVal.SetDouble(locationWCS[2]);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, LocationWCS, 0, "Vec3", "")
{
   double locationWCS[3];
   aObjectPtr->GetLocationWCS(locationWCS);
   aReturnVal.SetPointer(UtScriptVec3::Create(UtVec3d(locationWCS)));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, LocationECI, 0, "Vec3", "")
{
   UtVec3d locationECI;
   aObjectPtr->GetLocationECI(SIMULATION->GetDateTime().GetCurrentTime(TIME_NOW), locationECI.GetData());
   aReturnVal.SetPointer(UtScriptVec3::Create(locationECI));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, LocationMGRS, 0, "string", "")
{
   std::string locationMGRS;
   UtEllipsoidalEarth::ConvertLLToMGRS(aObjectPtr->GetLat(), aObjectPtr->GetLon(), locationMGRS);
   aReturnVal.SetString(locationMGRS);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, HeightAboveTerrain, 0, "double", "")
{
   double lat;
   double lon;
   double alt;
   aObjectPtr->GetLocationLLA(lat, lon, alt);
   wsf::Terrain terrain(wsf::TerrainInterface::Get(*SCENARIO));
   float        terrainHeight;
   terrain.GetElevInterp(lat, lon, terrainHeight);
   aReturnVal.SetDouble(alt - terrainHeight);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, TerrainHeight, 0, "double", "")
{
   double lat;
   double lon;
   double alt;
   aObjectPtr->GetLocationLLA(lat, lon, alt);
   wsf::Terrain terrain(wsf::TerrainInterface::Get(*SCENARIO));
   float        terrainHeight;
   terrain.GetElevInterp(lat, lon, terrainHeight);
   aReturnVal.SetDouble(terrainHeight);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, HeightAboveEllipsoid, 0, "double", "")
{
   double lat, lon, alt;
   aObjectPtr->GetLocationLLA(lat, lon, alt);
   float geoidUndulationCorrection = wsf::EarthGravityModelExtension::Get(*SCENARIO).GetGeoidUndulation(lat, lon);
   // double altHAE = altMSL + lGravityCorrectionForAltMeters;
   aReturnVal.SetDouble(alt + geoidUndulationCorrection);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, GeoidHeight, 0, "double", "")
{
   double lat, lon, alt;
   aObjectPtr->GetLocationLLA(lat, lon, alt);
   float geoidUndulationCorrection = wsf::EarthGravityModelExtension::Get(*SCENARIO).GetGeoidUndulation(lat, lon);
   // Subtract off the gravity correction factor when converting from HAE to MSL.
   //    double altMSL = altHAE - lGravityCorrectionForAltMeters;
   // Add on the gravity correction factor when converting from MSL to HAE.
   //    double altHAE = altMSL + lGravityCorrectionForAltMeters;
   aReturnVal.SetDouble(geoidUndulationCorrection);
}

// Construct(double aLat, double aLon, double aAlt);
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, Construct, 3, "WsfGeoPoint", "double, double, double")
{
   WsfGeoPoint* pointPtr = new WsfGeoPoint(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble());
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// Construct(double aLat, double aLon, double aAlt);
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, Construct_2, 1, "WsfGeoPoint", "string")
{
   UtLatPos          lat;
   UtLonPos          lon;
   UtLengthValue     alt;
   std::stringstream ss(aVarArgs[0].GetString());
   ss >> lat >> lon >> alt;
   WsfGeoPoint* pointPtr = new WsfGeoPoint(lat, lon, alt);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// Construct(double aX, double aY, double aZ);
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, ConstructWCS_1, 3, "WsfGeoPoint", "double, double, double")
{
   double       locationWCS[3] = {aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble()};
   WsfGeoPoint* pointPtr       = new WsfGeoPoint(locationWCS);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// Construct(Vec3);
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, ConstructWCS_2, 1, "WsfGeoPoint", "Vec3")
{
   UtVec3d*     vec3Ptr  = (UtVec3d*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfGeoPoint* pointPtr = new WsfGeoPoint(vec3Ptr->GetData());
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// double TerrainHeight(double aLat, double aLon)
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, StaticTerrainHeight, 2, "double", "double, double")
{
   double       lat = aVarArgs[0].GetDouble();
   double       lon = aVarArgs[1].GetDouble();
   wsf::Terrain terrain(wsf::TerrainInterface::Get(*SCENARIO));
   float        terrainHeight;
   terrain.GetElevInterp(lat, lon, terrainHeight);
   aReturnVal.SetDouble(terrainHeight);
}

// Set(WsfGeoPoint aPoint)
// Sets this point to the position of the specified point (deep copy).
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, Set_1, 1, "void", "WsfGeoPoint")
{
   WsfGeoPoint* pointPtr = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   double       locationWCS[3];
   pointPtr->GetLocationWCS(locationWCS);
   aObjectPtr->SetLocationWCS(locationWCS);
}

// Set(double aLat, double aLon, double aAlt);
// Sets this point to the specified lat/lon/alt.
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, Set_2, 3, "void", "double, double, double")
{
   aObjectPtr->SetLocationLLA(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble());
}

// Set(WsfPlatform aPlatform)
// Sets this point to the current position of the platform
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, Set_3, 1, "void", "WsfPlatform")
{
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   double       simTime     = TIME_NOW;
   platformPtr->Update(simTime);
   double locationWCS[3];
   platformPtr->GetLocationWCS(locationWCS);
   aObjectPtr->SetLocationWCS(locationWCS);
}

// Set(WsfTrack aTrack)
// Sets this point to the current position of the track
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, Set_4, 1, "void", "WsfTrack")
{
   WsfTrack* trackPtr = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   double    locationWCS[3];
   double    simTime = TIME_NOW;
   if (trackPtr->GetExtrapolatedLocationWCS(simTime, locationWCS))
   {
      aObjectPtr->SetLocationWCS(locationWCS);
   }
}

// Set(WsfWaypoint aPoint)
// Sets this point to the position of the waypoint
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, Set_5, 1, "void", "WsfWaypoint")
{
   WsfWaypoint* pointPtr = static_cast<WsfWaypoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (pointPtr->GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE)
   {
      double alt = aObjectPtr->GetAlt();
      if (pointPtr->GetAlt() != WsfPath::cDOUBLE_NOT_SET)
      {
         alt = pointPtr->GetAlt();
      }
      aObjectPtr->SetLocationLLA(pointPtr->GetLat(), pointPtr->GetLon(), alt);
   }
}

// Set[WCS](Vec3 aPoint)
// Sets this point to a WCS location
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, SetWCS, 1, "void", "Vec3")
{
   UtVec3d* pointPtr = static_cast<UtVec3d*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetLocationWCS(pointPtr->GetData());
}

// SetAltitudeAGL(double aAltitude)
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, SetAltitudeAGL, 1, "void", "double")
{
   double lat, lon, alt;
   aObjectPtr->GetLocationLLA(lat, lon, alt);
   float        terrainHeight(0.0f);
   wsf::Terrain terrain(wsf::TerrainInterface::Get(*SCENARIO));
   terrain.GetElevInterp(lat, lon, terrainHeight);
   aObjectPtr->SetLocationLLA(lat, lon, aVarArgs[0].GetDouble() + terrainHeight);
}

// SetAltitudeHAE(double aAltitude)
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, SetAltitudeHAE, 1, "void", "double")
{
   double lat, lon, alt;
   aObjectPtr->GetLocationLLA(lat, lon, alt);
   float geoidUndulationCorrection = wsf::EarthGravityModelExtension::Get(*SCENARIO).GetGeoidUndulation(lat, lon);
   // Subtract off the gravity correction factor when converting from HAE to MSL.
   //    double altMSL = altHAE - lGravityCorrectionForAltMeters;
   // Add on the gravity correction factor when converting from MSL to HAE.
   //    double altHAE = altMSL + lGravityCorrectionForAltMeters;
   aObjectPtr->SetLocationLLA(lat, lon, aVarArgs[0].GetDouble() - geoidUndulationCorrection);
}

// double distance = SlantRangeTo(WsfGeoPoint aPoint)
// Compute the slant range from this point to the specified point
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, SlantRangeTo, 1, "double", "WsfGeoPoint")
{
   WsfGeoPoint* pointPtr = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   aReturnVal.SetDouble(aObjectPtr->SlantRangeTo(*pointPtr, false));
}

// double distance = SlantRangeTo(WsfGeoPoint aPoint, bool aUseSpherical)
// Compute the slant range from this point to the specified point
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, SlantRangeToS, 2, "double", "WsfGeoPoint, bool")
{
   WsfGeoPoint* pointPtr     = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   bool         useSpherical = aVarArgs[1].GetBool();
   aReturnVal.SetDouble(aObjectPtr->SlantRangeTo(*pointPtr, useSpherical));
}

// double distance = GroundRangeTo(WsfGeoPoint aPoint)
// Compute the approximate ground range from this point to the specified point
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, GroundRangeTo, 1, "double", "WsfGeoPoint")
{
   WsfGeoPoint* pointPtr = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   aReturnVal.SetDouble(aObjectPtr->GroundRangeTo(*pointPtr));
}

// double bearing = TrueBearingTo(WsfGeoPoint aPoint)
// Compute the true bearing in degrees from this point to the specified point
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, TrueBearingTo, 1, "double", "WsfGeoPoint")
{
   WsfGeoPoint* pointPtr = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   double       bearing  = aObjectPtr->TrueBearingTo(*pointPtr);
   aReturnVal.SetDouble(bearing * UtMath::cDEG_PER_RAD);
}

// Offset(double aHeading, double aDeltaX, double aDeltaY, double aDeltaZ)
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, Offset_1, 4, "void", "double, double, double, double")
{
   double heading   = aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG;
   double offset[3] = {aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble(), aVarArgs[3].GetDouble()};
   aObjectPtr->AddOffset(heading, offset);
}

// Offset(double aHeading, Vec3)
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, Offset_2, 2, "void", "double, Vec3")
{
   double   heading = aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG;
   UtVec3d* vecPtr  = (UtVec3d*)aVarArgs[1].GetPointer()->GetAppObject();
   aObjectPtr->AddOffset(heading, vecPtr->GetData());
}

// OffsetNED(double aDeltaNorth, double aDeltaEast, double aDeltaDown)
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, OffsetNED_1, 3, "void", "double, double, double")
{
   double offsetNED[3] = {aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble()};
   aObjectPtr->AddOffsetNED(offsetNED);
}

// OffsetNED(Vec3 aDeltaNED)
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, OffsetNED_2, 1, "void", "Vec3")
{
   UtVec3d* offsetNED_Ptr = (UtVec3d*)aVarArgs[0].GetPointer()->GetAppObject();
   aObjectPtr->AddOffsetNED(offsetNED_Ptr->GetData());
}

// OffsetRBE(double aRange, double aBearing, double aElevation)
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, OffsetRBE, 3, "void", "double, double, double")
{
   double range        = aVarArgs[0].GetDouble();
   double bearing      = aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG;
   double elevation    = aVarArgs[2].GetDouble() * UtMath::cRAD_PER_DEG;
   double sinBearing   = sin(bearing);
   double cosBearing   = cos(bearing);
   double sinElevation = sin(elevation);
   double cosElevation = cos(elevation);
   double offsetNED[3] = {range * cosBearing * cosElevation, range * sinBearing * cosElevation, -range * sinElevation};
   aObjectPtr->AddOffsetNED(offsetNED);
}

// OffsetWCS(double aDeltaX, double aDeltaY, double aDeltaZ)
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, OffsetWCS_1, 3, "void", "double, double, double")
{
   double offsetWCS[3] = {aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble()};
   double locWCS[3];
   aObjectPtr->GetLocationWCS(locWCS);
   UtVec3d::Add(locWCS, locWCS, offsetWCS);
   aObjectPtr->SetLocationWCS(locWCS);
}

// OffsetWCS(Vec3)
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, OffsetWCS_2, 1, "void", "Vec3")
{
   UtVec3d* vecPtr = (UtVec3d*)aVarArgs[0].GetPointer()->GetAppObject();
   UtVec3d  locWCS;
   aObjectPtr->GetLocationWCS(locWCS.GetData());
   aObjectPtr->SetLocationWCS((locWCS + *vecPtr).GetData());
}

// Extrapolate(double aHeading, double aDistance);
// Extrapolates the point along a great circle heading for a specified distance.
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, Extrapolate, 2, "void", "double, double")
{
   aObjectPtr->Extrapolate(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG, aVarArgs[1].GetDouble());
}

//! Is the track within the specified zone
// bool inside = WithinZoneOf(string aZoneName)
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, WithinZone, 1, "bool", "string")
{
   bool     withinZone = false;
   WsfZone* zonePtr    = WsfZoneTypes::Get(*SCENARIO).Find(aVarArgs[0].GetString());
   if (zonePtr != nullptr)
   {
      double thisLocWCS[3];
      aObjectPtr->GetLocationWCS(thisLocWCS);
      withinZone = zonePtr->PointIsInside(SIMULATION, thisLocWCS, thisLocWCS, 0.0);
   }
   aReturnVal.SetBool(withinZone);
}

// Is this point within the specified zone of a specified platform
// bool inside = <point>.WithinZoneOf(WsfPlatform aPlatform, string aZoneName)
UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, WithinZoneOf, 2, "bool", "WsfPlatform, string")
{
   bool         withinZone  = false;
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfZone*     zonePtr     = platformPtr->GetComponent<WsfZone>(aVarArgs[1].GetString());
   if (zonePtr != nullptr)
   {
      double simTime = TIME_NOW;

      // Get the target location.
      double targetLocWCS[3];
      aObjectPtr->GetLocationWCS(targetLocWCS);

      // Get the location and heading of this platform that contains the zone
      double refLocWCS[3];
      double refHeading;
      double refPitch;
      double refRoll;
      platformPtr->Update(simTime); // Ensure the reference platform position is current
      platformPtr->GetLocationWCS(refLocWCS);
      platformPtr->GetOrientationNED(refHeading, refPitch, refRoll);
      withinZone = zonePtr->PointIsInside(SIMULATION, targetLocWCS, refLocWCS, refHeading);
   }
   aReturnVal.SetBool(withinZone);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, ApparentTimeNow, 0, "double", "")
{
   auto epoch = SIMULATION->GetDateTime().GetCurrentTime(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetApparentTime(epoch));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGeoPointClass, WsfGeoPoint, ApparentTime, 1, "double", "Calendar")
{
   auto* epochPtr = aVarArgs[0].GetPointer()->GetAppObject<UtCalendar>();
   aReturnVal.SetDouble(aObjectPtr->GetApparentTime(*epochPtr));
}
