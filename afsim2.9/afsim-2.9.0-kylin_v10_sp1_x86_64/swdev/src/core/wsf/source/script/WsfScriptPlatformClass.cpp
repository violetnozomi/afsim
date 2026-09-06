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

#include "WsfScriptPlatformClass.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <set>
#include <sstream>
#include <string>

#include "UtAtmosphere.hpp"
#include "UtCast.hpp"
#include "UtDCM.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMeasurementUtil.hpp"
#include "UtMemory.hpp"
#include "UtScriptVec3.hpp"
#include "UtTypeInfo.hpp"
#include "UtVec3.hpp"
#include "WsfAirMover.hpp"
#include "WsfCallback.hpp"
#include "WsfCallbackTypes.hpp"
#include "WsfComm.hpp"
#include "WsfCommRouter.hpp"
#include "WsfCommandChain.hpp"
#include "WsfEM_Util.hpp"
#include "WsfExchange.hpp"
#include "WsfFollower.hpp"
#include "WsfFuel.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfGroup.hpp"
#include "WsfGroupList.hpp"
#include "WsfGroupManager.hpp"
#include "WsfHybridMover.hpp"
#include "WsfIntercept.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMover.hpp"
#include "WsfMoverTypes.hpp"
#include "WsfNavigationErrors.hpp"
#include "WsfPathFinder.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformTypes.hpp"
#include "WsfProcessor.hpp"
#include "WsfRoadMover.hpp"
#include "WsfRoute.hpp"
#include "WsfRouteMover.hpp"
#include "WsfRouteNetwork.hpp"
#include "WsfRouteTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfScriptContext.hpp"
#include "WsfScriptManager.hpp"
#include "WsfSensor.hpp"
#include "WsfSignatureList.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTerrain.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackId.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackProcessor.hpp"
#include "WsfUtil.hpp"
#include "WsfVisualPart.hpp"
#include "WsfZone.hpp"
#include "WsfZoneDefinition.hpp"
#include "WsfZoneReference.hpp"
#include "WsfZoneSet.hpp"

namespace
{
const double cHUGE_OFFSET_OR_ALTITUDE = 1.0E+10;

//! Compute the approximate ground range between a platform and another point.
double GroundRange(UtEntity* aThisPtr, const double aOtherLocWCS[3])
{
   double thisLocWCS[3];
   aThisPtr->GetLocationWCS(thisLocWCS);
   return UtMeasurementUtil::GroundRange(thisLocWCS, aOtherLocWCS);
}

//! Compute the slant range between a platform and another point.
double SlantRange(UtEntity* aThisPtr, const double aOtherLocWCS[3], const bool aUseSpherical = false)
{
   double slantRange = 0.0;
   double thisLocWCS[3];
   aThisPtr->GetLocationWCS(thisLocWCS);
   if (aUseSpherical)
   {
      slantRange = UtMeasurementUtil::SlantRange(thisLocWCS, aOtherLocWCS);
   }
   else
   {
      double thisToOtherLocWCS[3];
      UtVec3d::Subtract(thisToOtherLocWCS, thisLocWCS, aOtherLocWCS);
      slantRange = UtVec3d::Magnitude(thisToOtherLocWCS);
   }
   return slantRange;
}

//! Compute the down range between a platform and another point.
//! Positive ahead, negative behind
double DownRange(UtEntity* aThisPtr, const double aOtherLocWCS[3])
{
   double groundRange = GroundRange(aThisPtr, aOtherLocWCS);
   double bearing     = aThisPtr->RelativeBearing(aOtherLocWCS);
   return groundRange * cos(bearing);
}

//! Compute the cross range between a platform and another point.
//! Positive right, negative left
double CrossRange(UtEntity* aThisPtr, const double aOtherLocWCS[3])
{
   double groundRange = GroundRange(aThisPtr, aOtherLocWCS);
   double bearing     = aThisPtr->RelativeBearing(aOtherLocWCS);
   return groundRange * sin(bearing);
}

//! Compute the approximate point of closest approach.
void ClosestApproach(UtEntity* aThisPtr, const double aOtherLocWCS[3], const double aOtherVelWCS[3], double aClosestLocWCS[3])
{
   double thisLocWCS[3];
   aThisPtr->GetLocationWCS(thisLocWCS);
   WsfUtil::ClosestApproachPoint2D(aOtherLocWCS, aOtherVelWCS, thisLocWCS, aClosestLocWCS);
}

//! Compute the difference between the headings of two objects.
double HeadingDifference(UtEntity* aThisPtr, double aOtherHeading)
{
   // Get the heading of this entity.
   double thisHeading;
   double thisPitch;
   double thisRoll;
   aThisPtr->GetOrientationNED(thisHeading, thisPitch, thisRoll);
   return UtMath::NormalizeAngleMinusPi_Pi(aOtherHeading - thisHeading);
}

//! Compute the closing speed.
double ClosingSpeed(UtEntity* aThisPtr, double aThisSpeed, const double aOtherLocWCS[3], const double aOtherVelWCS[3])
{
   // Get the relative location.
   double thisLocWCS[3];
   aThisPtr->GetLocationWCS(thisLocWCS);
   double relLocWCS[3];
   UtVec3d::Subtract(relLocWCS, aOtherLocWCS, thisLocWCS);

   // Get the relative velocity.
   double thisVelWCS[3];
   aThisPtr->GetVelocityWCS(thisVelWCS);
   double speed = UtVec3d::Normalize(thisVelWCS);
   if (speed < 0.1)
   {
      // The current platform doesn't have a speed. We could align it along the 'heading', but a more
      // reasonable approach might be use the line-of-sight to the other object???? That's what we'll do.
      UtVec3d::Set(thisVelWCS, relLocWCS);
      UtVec3d::Normalize(thisVelWCS);
   }
   UtVec3d::Multiply(thisVelWCS, aThisSpeed);
   double relVelWCS[3];
   UtVec3d::Subtract(relVelWCS, aOtherVelWCS, thisVelWCS);

   double closingSpeed = 0.0;
   if (UtVec3d::Normalize(relLocWCS) > 0.0)
   {
      closingSpeed = -UtVec3d::DotProduct(relVelWCS, relLocWCS);
   }
   return closingSpeed;
}

//! Returns a WsfRoute* from a UtScriptData<WsfRoute>
WsfRoute* GetRouteObject(const UtScriptData& aData)
{
   return (WsfRoute*)aData.GetPointer()->GetAppObject();
}

//! Returns a WsfRoute* from a UtScriptData<string>
const WsfRoute* GetNamedRoute(const UtScriptData& aData, WsfMover* aRouteMvrPtr)
{
   const WsfRoute* routePtr = nullptr;
   if (aData.GetString() == "DEFAULT_ROUTE")
   {
      routePtr = aRouteMvrPtr->GetDefaultRoute();
   }
   else if (aRouteMvrPtr != nullptr)
   {
      routePtr = WsfRouteTypes::Get(aRouteMvrPtr->GetScenario()).Find(aData.GetString());
   }
   return routePtr;
}

//! Return a WsfRouteMover* cast to correct type
WsfMover* GetRouteMover(WsfPlatform* aPlatformPtr)
{
   WsfMover*       routeMvrPtr    = aPlatformPtr->GetMover();
   WsfHybridMover* hybridMoverPtr = dynamic_cast<WsfHybridMover*>(aPlatformPtr->GetMover());
   if (hybridMoverPtr != nullptr)
   {
      // Change to a waypoint mover.
      hybridMoverPtr->SetType(WsfHybridMover::cWAYPOINT);
      routeMvrPtr = dynamic_cast<WsfRouteMover*>(hybridMoverPtr->GetCurrent());
   }

   return routeMvrPtr;
}

//! FollowRoute helper method
//! returns true if successful
bool FollowRoute(double aSimTime, WsfMover* aMoverPtr, const WsfRoute* aRoutePtr)
{
   bool success = false;
   if ((aRoutePtr != nullptr) && (aMoverPtr != nullptr))
   {
      if (!aRoutePtr->Empty())
      {
         success = aMoverPtr->UpdateRoute(aSimTime, *aRoutePtr);
      }
   }
   return success;
}

//! SetRoute helper method.  Resets the platform at the beginning of the new route.
//! returns true if successful
bool SetRoute(double aSimTime, WsfPlatform& aPlatform, const WsfRoute& aRoute)
{
   bool      success(false);
   WsfMover* moverPtr = GetRouteMover(&aPlatform);
   if (moverPtr != nullptr)
   {
      success = moverPtr->SetRoute(aSimTime, aRoute);
   }
   return success;
}

void ResetLocation(WsfPlatform* aPlatformPtr, double aSimTime)
{
   WsfMover* moverPtr = aPlatformPtr->GetMover();
   if (moverPtr != nullptr)
   {
      moverPtr->ResetPosition(aSimTime);
   }
}

//! GoToWaypoint helper method
//! returns true if successful
bool GoToWaypoint(double aSimTime, WsfMover* aRouteMvrPtr, int aIndex)
{
   bool         success = false;
   unsigned int size    = 0;
   auto         route   = aRouteMvrPtr->GetRoute();
   if (route)
   {
      size = route->GetSize();
   }
   auto index = ut::safe_cast<unsigned int>(aIndex);
   if (index < size)
   {
      success = aRouteMvrPtr->GoToWaypoint(aSimTime, index);
   }
   return success;
}

//! GoToWaypoint helper method
//! returns true if successful
bool GoToWaypoint(double aSimTime, WsfMover* aRouteMvrPtr, std::string aRouteAction)
{
   bool success = false;
   if (aRouteAction == "CLOSEST_POINT")
   {
      success = aRouteMvrPtr->GoToClosestPoint(aSimTime);
   }
   else
   {
      success = aRouteMvrPtr->GoToLabel(aSimTime, WsfStringId(aRouteAction));
   }
   return success;
}
} // namespace

WsfScriptPlatformClass::WsfScriptPlatformClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : WsfScriptObjectClass(aClassName, aTypesPtr)
{
   SetClassName("WsfPlatform");

   mEqualityComparable = true;
   mLessThanComparable = true;

   // Add each of the method objects to the class.

   AddStaticMethod(ut::make_unique<IsA_TypeOf>());

   AddMethod(ut::make_unique<Index>());
   AddMethod(ut::make_unique<CreationTime>());
   AddMethod(ut::make_unique<SetCreationTime>());
   AddMethod(ut::make_unique<TimeSinceCreation>());
   AddMethod(ut::make_unique<SetSide>());
   AddMethod(ut::make_unique<Side>());
   AddMethod(ut::make_unique<SetIcon>());
   AddMethod(ut::make_unique<Icon>());
   AddMethod(ut::make_unique<Commander_1>("Commander"));
   AddMethod(ut::make_unique<Commander_2>("Commander"));
   AddMethod(ut::make_unique<SetCommander_1>("SetCommander"));
   AddMethod(ut::make_unique<SetCommander_2>("SetCommander"));
   AddMethod(ut::make_unique<CommanderName_1>("CommanderName"));
   AddMethod(ut::make_unique<CommanderName_2>("CommanderName"));
   AddMethod(ut::make_unique<Peers_1>("Peers"));
   AddMethod(ut::make_unique<Peers_2>("Peers"));
   AddMethod(ut::make_unique<Subordinates_1>("Subordinates"));
   AddMethod(ut::make_unique<Subordinates_2>("Subordinates"));
   AddMethod(ut::make_unique<CommandChain>());
   AddMethod(ut::make_unique<CommandChainCount>());
   AddMethod(ut::make_unique<CommandChainEntry>());
   AddMethod(ut::make_unique<SetMoverDebugEnabled>());
   AddMethod(ut::make_unique<Comment_1>("Comment"));
   AddMethod(ut::make_unique<Comment_2>("Comment"));
   AddMethod(ut::make_unique<IsExternallyControlled>());
   AddMethod(ut::make_unique<SpatialDomain>());

   AddMethod(ut::make_unique<WCS_To_ECS_DCM>());
   AddMethod(ut::make_unique<WCS_To_NED_DCM>());
   AddMethod(ut::make_unique<LineOfSightRatesTo>());
   AddMethod(ut::make_unique<Mover>());
   AddMethod(ut::make_unique<Comm>());
   AddMethod(ut::make_unique<CommCount>());
   AddMethod(ut::make_unique<CommEntry>());
   AddMethod(ut::make_unique<Router>());
   AddMethod(ut::make_unique<RouterCount>());
   AddMethod(ut::make_unique<RouterEntry>());

   AddMethod(ut::make_unique<Fuel>());
   AddMethod(ut::make_unique<FuelRemaining>());     // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<Refuel_1>("Refuel"));  // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<Refuel_2>("Refuel"));  // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<FuelBingoQuantity>()); // NO_DOC | DEPRECATED

   AddMethod(ut::make_unique<GPS_Status>());
   AddMethod(ut::make_unique<SetGPS_Status>());
   AddMethod(ut::make_unique<SetPerceivedLocationErrorWCS>());

   AddMethod(ut::make_unique<Processor>());
   AddMethod(ut::make_unique<ProcessorCount>());
   AddMethod(ut::make_unique<ProcessorEntry>());
   AddMethod(ut::make_unique<Sensor>());
   AddMethod(ut::make_unique<SensorCount>());
   AddMethod(ut::make_unique<SensorEntry>());
   AddMethod(ut::make_unique<VisualPart>()); // NO_DOC | FOR_TEST_ONLY

   AddMethod(ut::make_unique<TurnCommOn>());
   AddMethod(ut::make_unique<TurnCommOff>());
   AddMethod(ut::make_unique<TurnProcessorOn>());
   AddMethod(ut::make_unique<TurnProcessorOff>());
   AddMethod(ut::make_unique<TurnSensorOn>());
   AddMethod(ut::make_unique<TurnSensorOff>());
   AddMethod(ut::make_unique<TurnMoverOn>());
   AddMethod(ut::make_unique<TurnMoverOff>());
   AddMethod(ut::make_unique<TurnRouterOn>());
   AddMethod(ut::make_unique<TurnRouterOff>());

   AddMethod(ut::make_unique<TrackManager>());

   AddMethod(ut::make_unique<Length>());
   AddMethod(ut::make_unique<Width>());
   AddMethod(ut::make_unique<Height>());
   AddMethod(ut::make_unique<TotalMass>());
   AddMethod(ut::make_unique<EmptyMass>());
   AddMethod(ut::make_unique<FuelMass>());
   AddMethod(ut::make_unique<PayloadMass>());
   AddMethod(ut::make_unique<ConcealmentFactor>());
   AddMethod(ut::make_unique<SetConcealmentFactor>());
   AddMethod(ut::make_unique<AfterburnerFactor>());     // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<SetAfterburnerFactor>());  // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<ConfigurationState>());    // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<SetConfigurationState>()); // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<Indestructible>());
   AddMethod(ut::make_unique<SetIndestructible>());
   AddMethod(ut::make_unique<SetSignatureState>());
   AddMethod(ut::make_unique<SetDamageFactor>());
   AddMethod(ut::make_unique<DamageFactor>());
   AddMethod(ut::make_unique<SetTrailingEffects>());
   AddMethod(ut::make_unique<Appearance>());
   AddMethod(ut::make_unique<SetAppearance>());
   AddMethod(ut::make_unique<Capabilities>());
   AddMethod(ut::make_unique<SetCapability>());
   AddMethod(ut::make_unique<SetMarking>());

   AddMethod(ut::make_unique<ScriptExists>());
   AddMethod(ut::make_unique<Execute_1>("Execute"));
   AddMethod(ut::make_unique<Execute_2>("Execute"));
   AddMethod(ut::make_unique<ExecuteAtTime_1>("ExecuteAtTime"));
   AddMethod(ut::make_unique<ExecuteAtTime_2>("ExecuteAtTime"));

   AddMethod(ut::make_unique<ExecuteScript>());
   AddMethod(ut::make_unique<ExecuteScriptWithArgs>());
   AddStaticMethod(ut::make_unique<ExecuteGlobalScript>());
   AddMethod(ut::make_unique<GeoPoint>());

   AddMethod(ut::make_unique<MasterTrackList>());
   AddMethod(ut::make_unique<MasterRawTrackList>());
   AddMethod(ut::make_unique<TrackList>());
   AddMethod(ut::make_unique<RawTrackList>());
   AddMethod(ut::make_unique<CurrentTarget>());
   AddMethod(ut::make_unique<SetCurrentTarget>());
   AddMethod(ut::make_unique<HasCurrentTarget>());
   AddMethod(ut::make_unique<ClearCurrentTarget>());
   AddMethod(ut::make_unique<CurrentTargetTrack>());
   AddMethod(ut::make_unique<InterceptLocation2D_1>("InterceptLocation2D")); // InterceptLocation2D(WsfTrack, WsfWaypoint)
   AddMethod(
      ut::make_unique<InterceptLocation2D_2>("InterceptLocation2D")); // InterceptLocation2D(WsfTrack, WsfWaypoint, bool)
   AddMethod(ut::make_unique<InterceptLocation2D_3>("InterceptLocation2D")); // InterceptLocation2D(WsfTrack,
                                                                             // WsfWaypoint, double, double)
   AddMethod(ut::make_unique<InterceptLocation2D_4>("InterceptLocation2D")); // InterceptLocation2D(WsfTrack,
                                                                             // WsfWaypoint, double, double, bool)
   AddMethod(ut::make_unique<InterceptLocation3D_1>("InterceptLocation3D")); // InterceptLocation3D(WsfTrack, WsfWaypoint)
   AddMethod(
      ut::make_unique<InterceptLocation3D_2>("InterceptLocation3D")); // InterceptLocation3D(WsfTrack, WsfWaypoint, bool)
   AddMethod(ut::make_unique<InterceptLocation3D_3>("InterceptLocation3D")); // InterceptLocation3D(WsfTrack,
                                                                             // WsfWaypoint, double, double)
   AddMethod(ut::make_unique<InterceptLocation3D_4>("InterceptLocation3D")); // InterceptLocation3D(WsfTrack,
                                                                             // WsfWaypoint, double, double, bool)
   AddMethod(ut::make_unique<TravelTime>("TravelTime"));

   AddMethod(ut::make_unique<SetLocation_1>("SetLocation"));
   AddMethod(ut::make_unique<SetLocation_2>("SetLocation"));
   AddMethod(ut::make_unique<SetLocationMGRS_1>("SetLocationMGRS"));
   AddMethod(ut::make_unique<SetLocationMGRS_2>("SetLocationMGRS"));
   AddMethod(ut::make_unique<SetLocationECI>("SetLocationECI"));
   AddMethod(ut::make_unique<Location>());
   AddMethod(ut::make_unique<LocationWCS>());
   AddMethod(ut::make_unique<LocationECI>());
   AddMethod(ut::make_unique<LocationTEME>()); // NO_DOC | FOR_TEST_ONLY
   AddMethod(ut::make_unique<LocationMGRS>());
   AddMethod(ut::make_unique<LocationJ2000>());
   AddMethod(ut::make_unique<LocationAsWaypoint>());
   AddMethod(ut::make_unique<Latitude>());
   AddMethod(ut::make_unique<Longitude>());
   AddMethod(ut::make_unique<Altitude>());
   AddMethod(ut::make_unique<HeightAboveTerrain>());
   AddMethod(ut::make_unique<PerceivedLocation>());
   AddMethod(ut::make_unique<PerceivedLocationErrorWCS>());
   AddMethod(ut::make_unique<PerceivedLocationErrorNED>());
   AddMethod(ut::make_unique<FutureLocation>());
   AddMethod(ut::make_unique<Speed>());
   AddMethod(ut::make_unique<GroundSpeed>());
   AddMethod(ut::make_unique<MachNumber>());
   AddMethod(ut::make_unique<Heading>());
   AddMethod(ut::make_unique<Pitch>());
   AddMethod(ut::make_unique<Roll>());
   AddMethod(ut::make_unique<SetHeading>());
   AddMethod(ut::make_unique<SetSpeedAtWaypoint>()); // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<SetPitch>());
   AddMethod(ut::make_unique<SetRoll>());
   AddMethod(ut::make_unique<X>());
   AddMethod(ut::make_unique<Y>());
   AddMethod(ut::make_unique<Z>());
   AddMethod(ut::make_unique<Vx>());
   AddMethod(ut::make_unique<Vy>());
   AddMethod(ut::make_unique<Vz>());
   AddMethod(ut::make_unique<VelocityWCS>());
   AddMethod(ut::make_unique<VelocityNED>());
   AddMethod(ut::make_unique<VelocityECI>());
   AddMethod(ut::make_unique<VelocityTEME>()); // NO_DOC | FOR_TEST_ONLY
   AddMethod(ut::make_unique<VelocityJ2000>());
   AddMethod(ut::make_unique<SetVelocityNED>());
   AddMethod(ut::make_unique<SetVelocityECS>());
   AddMethod(ut::make_unique<AccelerationWCS>());
   AddMethod(ut::make_unique<AccelerationNED>());
   AddMethod(ut::make_unique<AccelerationECI>());
   AddMethod(ut::make_unique<OrientationNED>());
   AddMethod(ut::make_unique<OrientationWCS>());
   AddMethod(ut::make_unique<OrientationECI>());
   AddMethod(ut::make_unique<SetOrientationNED>());
   AddMethod(ut::make_unique<SetOrientationWCS>());
   AddMethod(ut::make_unique<SetOrientationECI>());
   AddMethod(ut::make_unique<GoToPoint_1>("GoToPoint")); // GoToPoint(WsfGeoPoint)
   AddMethod(ut::make_unique<GoToPoint_2>("GoToPoint")); // GoToPoint(string)
   AddMethod(ut::make_unique<GoToPoint_3>("GoToPoint")); // GoToPoint(string, string)
   AddMethod(ut::make_unique<GoToPoint_4>("GoToPoint")); // GoToPoint(WsfGeoPoint, string)
   AddMethod(ut::make_unique<GoToLabel>());
   AddMethod(ut::make_unique<GoToLocation_1>("GoToLocation"));
   AddMethod(ut::make_unique<GoToLocation_2>("GoToLocation"));
   AddMethod(ut::make_unique<GoToLocation_3>("GoToLocation"));
   AddMethod(ut::make_unique<GoToLocation_4>("GoToLocation"));
   AddMethod(ut::make_unique<GoToTrack>());              // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<GoToTrack_2>("GoToTrack")); // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<GoToAltitude_1>("GoToAltitude"));
   AddMethod(ut::make_unique<GoToAltitude_2>("GoToAltitude"));
   AddMethod(ut::make_unique<GoToAltitude_3>("GoToAltitude"));
   AddMethod(ut::make_unique<GoToSpeed_1>("GoToSpeed"));
   AddMethod(ut::make_unique<GoToSpeed_2>("GoToSpeed"));
   AddMethod(ut::make_unique<GoToSpeed_3>("GoToSpeed"));
   AddMethod(ut::make_unique<GoToMachNumber_1>("GoToMachNumber"));
   AddMethod(ut::make_unique<GoToMachNumber_2>("GoToMachNumber"));
   AddMethod(ut::make_unique<TurnToHeading_1>("TurnToHeading"));
   AddMethod(ut::make_unique<TurnToHeading_2>("TurnToHeading"));
   AddMethod(ut::make_unique<TurnToRelativeHeading_1>("TurnToRelativeHeading"));
   AddMethod(ut::make_unique<TurnToRelativeHeading_2>("TurnToRelativeHeading"));
   AddStaticMethod(ut::make_unique<CreateRoute>()); // CreateRoute(string);

   AddMethod(ut::make_unique<GetRoute>("Route"));            // WsfRoute route = Route();
   AddMethod(ut::make_unique<RoutePointIndex>());            // int pointIndex = RoutePointIndex();
   AddMethod(ut::make_unique<FollowRoute_1>("FollowRoute")); // FollowRoute(WsfRoute aRoute);
   AddMethod(ut::make_unique<FollowRoute_2>("FollowRoute")); // FollowRoute(WsfRoute aRoute, string aRouteAction);
   AddMethod(ut::make_unique<FollowRoute_3>("FollowRoute")); // FollowRoute(string aRouteName);
   AddMethod(ut::make_unique<FollowRoute_4>("FollowRoute")); // FollowRoute(string aRouteName, string aRouteAction);
   AddMethod(ut::make_unique<FollowRoute_5>("FollowRoute")); // FollowRoute(WsfRoute aRoute, int aIndex);
   AddMethod(ut::make_unique<FollowRoute_6>("FollowRoute")); // FollowRoute(string aRouteName, int aIndex);
   AddMethod(ut::make_unique<SetRoute_1>("SetRoute"));       // SetRoute(WsfRoute aRoute);
   AddMethod(ut::make_unique<SetRoute_2>("SetRoute"));       // SetRoute(string aRouteName);
   AddMethod(ut::make_unique<DistanceAlongRoute>());         // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<ReturnToRoute>());
   AddMethod(ut::make_unique<FindAndSetPath>()); // NO_DOC | DEPRECATED

   AddMethod(ut::make_unique<FlyRates>());                // FlyRates(Vec3 angularRates, Vec3 axisAccel)
   AddMethod(ut::make_unique<FlyHeadingSpeedAltitude>()); // FlyHeadingSpeedAltitude(double heading, speed, altitude,
                                                          // maxGs, maxClimbRate)
   AddMethod(ut::make_unique<PullGsInPlaneWithSpeed>());  // PullGsInPlaneWithSpeed(Vec3 plane, double Gs, double speed)
   AddMethod(ut::make_unique<PullGsInPlaneWithThrottle>()); // PullGsInPlaneWithThrottle(Vec3 plane, double Gs, double throttle)
   AddMethod(ut::make_unique<FlyVectorWithSpeed>()); // FlyVectorWithSpeed(Vec3 velocityVector, double Gs, double speed)
   AddMethod(ut::make_unique<FlyVectorWithThrottle>()); // FlyVectorWithThrottle(Vec3 velocityVector, double Gs, double throttle)

   AddMethod(ut::make_unique<SetRandomLocation>()); // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<BreakFormation>());    // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<JoinFormation>());     // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<GetTargetSpeed>("TargetSpeed"));
   AddMethod(ut::make_unique<PathFinder>());
   AddMethod(ut::make_unique<Pause>());
   AddMethod(ut::make_unique<Unpause>());

   AddMethod(ut::make_unique<ApparentAspectOf>()); // ApparentAspectOf(WsfGeoPoint, double)

   AddMethod(ut::make_unique<RelativeAzimuthOf>());   // RelativeAzimuthOf(WsfGeoPoint)
   AddMethod(ut::make_unique<RelativeElevationOf>()); // RelativeAzimuthOf(WsfGeoPoint)

   AddMethod(ut::make_unique<TrueBearingTo_1>("TrueBearingTo"));             // TrueBearingTo(WsfTrack)
   AddMethod(ut::make_unique<TrueBearingTo_2>("TrueBearingTo"));             // TrueBearingTo(WsfPlatform)
   AddMethod(ut::make_unique<TrueBearingTo_3>("TrueBearingTo"));             // TrueBearingTo(lat, lon, alt)
   AddMethod(ut::make_unique<TrueBearingTo_4>("TrueBearingTo"));             // TrueBearingTo(WsfGeoPoint)
   AddMethod(ut::make_unique<RelativeBearingTo_1>("RelativeBearingTo"));     // RelativeBearingTo(WsfTrack)
   AddMethod(ut::make_unique<RelativeBearingTo_2>("RelativeBearingTo"));     // RelativeBearingTo(WsfPlatform)
   AddMethod(ut::make_unique<RelativeBearingTo_3>("RelativeBearingTo"));     // RelativeBearingTo(lat, lon, alt)
   AddMethod(ut::make_unique<RelativeBearingTo_4>("RelativeBearingTo"));     // RelativeBearingTo(lat, lon, alt)
   AddMethod(ut::make_unique<SlantRangeTo_1>("SlantRangeTo"));               // SlantRangeTo(WsfTrack)
   AddMethod(ut::make_unique<SlantRangeTo_1s>("SlantRangeTo"));              // SlantRangeTo(WsfTrack, bool)
   AddMethod(ut::make_unique<SlantRangeTo_2>("SlantRangeTo"));               // SlantRangeTo(WsfPlatform)
   AddMethod(ut::make_unique<SlantRangeTo_2s>("SlantRangeTo"));              // SlantRangeTo(WsfPlatform, bool)
   AddMethod(ut::make_unique<SlantRangeTo_3>("SlantRangeTo"));               // SlantRangeTo(lat, lon, alt)
   AddMethod(ut::make_unique<SlantRangeTo_3s>("SlantRangeTo"));              // SlantRangeTo(lat, lon, alt, bool)
   AddMethod(ut::make_unique<SlantRangeTo_4>("SlantRangeTo"));               // SlantRangeTo(WsfGeoPoint)
   AddMethod(ut::make_unique<SlantRangeTo_4s>("SlantRangeTo"));              // SlantRangeTo(WsfGeoPoint, bool)
   AddMethod(ut::make_unique<SlantRangeTo_5>("SlantRangeTo"));               // SlantRangeTo(WsfWaypoint)
   AddMethod(ut::make_unique<SlantRangeTo_5s>("SlantRangeTo"));              // SlantRangeTo(WsfWaypoint, bool)
   AddMethod(ut::make_unique<GroundRangeTo_1>("GroundRangeTo"));             // GroundRangeTo(WsfTrack)
   AddMethod(ut::make_unique<GroundRangeTo_2>("GroundRangeTo"));             // GroundRangeTo(WsfPlatform)
   AddMethod(ut::make_unique<GroundRangeTo_3>("GroundRangeTo"));             // GroundRangeTo(lat, lon, alt)
   AddMethod(ut::make_unique<GroundRangeTo_4>("GroundRangeTo"));             // GroundRangeTo(WsfGeoPoint)
   AddMethod(ut::make_unique<DownRangeTo_1>("DownRangeTo"));                 // DownRangeTo(WsfTrack)
   AddMethod(ut::make_unique<DownRangeTo_2>("DownRangeTo"));                 // DownRangeTo(WsfPlatform)
   AddMethod(ut::make_unique<DownRangeTo_3>("DownRangeTo"));                 // DownRangeTo(lat, lon, alt)
   AddMethod(ut::make_unique<DownRangeTo_4>("DownRangeTo"));                 // DownRangeTo(WsfGeoPoint)
   AddMethod(ut::make_unique<CrossRangeTo_1>("CrossRangeTo"));               // CrossRangeTo(WsfTrack)
   AddMethod(ut::make_unique<CrossRangeTo_2>("CrossRangeTo"));               // CrossRangeTo(WsfPlatform)
   AddMethod(ut::make_unique<CrossRangeTo_3>("CrossRangeTo"));               // CrossRangeTo(lat, lon, alt)
   AddMethod(ut::make_unique<CrossRangeTo_4>("CrossRangeTo"));               // CrossRangeTo(WsfGeoPoint)
   AddMethod(ut::make_unique<ClosestApproachOf_1>("ClosestApproachOf"));     // ClosestApproachOf(WsfTrack)
   AddMethod(ut::make_unique<ClosestApproachOf_2>("ClosestApproachOf"));     // ClosestApproachOf(WsfPlatform)
   AddMethod(ut::make_unique<HeadingDifferenceOf_1>("HeadingDifferenceOf")); // HeadingDifferenceOf(WsfTrack)
   AddMethod(ut::make_unique<HeadingDifferenceOf_2>("HeadingDifferenceOf")); // HeadingDifferenceOf(WsfPlatform)
   AddMethod(ut::make_unique<ClosingSpeedOf_1>("ClosingSpeedOf"));           // ClosingSpeedOf(WsfTrack)
   AddMethod(ut::make_unique<ClosingSpeedOf_2>("ClosingSpeedOf"));           // ClosingSpeedOf(WsfPlatform)
   AddMethod(ut::make_unique<ClosingSpeedOf_3>("ClosingSpeedOf"));           // ClosingSpeedOf(WsfTrack, mySpeed)
   AddMethod(ut::make_unique<ClosingSpeedOf_4>("ClosingSpeedOf"));           // ClosingSpeedOf(WsfPlatform, mySpeed)
   AddMethod(ut::make_unique<WithinFieldOfView_1>("WithinFieldOfView")); // WithinFieldOfView(WsfTrack, mySensorOrInterferer)
   AddMethod(ut::make_unique<WithinFieldOfView_2>("WithinFieldOfView")); // WithinFieldOfView(WsfPlatform, mySensorOrInterferer)
   AddMethod(ut::make_unique<MaskedByTerrain>());

   AddMethod(ut::make_unique<RelativeAltitudeOf>()); // RelativeAltitudeOf(WsfTrack)
   AddMethod(ut::make_unique<RelativeHeadingOf>());  // RelativeHeadingOf(WsfTrack)
   AddMethod(ut::make_unique<RelativeOffsetOf>());   // RelativeOffsetOf(WsfTrack)

   AddMethod(
      ut::make_unique<ExtrapolateGreatCirclePosition>()); // ExtrapolateGreatCirclePosition(double, double, double, double)
   AddMethod(ut::make_unique<GreatCircleHeading>());      // GreatCircleHeading(WsfTrack)
   AddMethod(ut::make_unique<GreatCircleDistance>());     // GreatCircleDistance(WsfTrack)
   AddMethod(ut::make_unique<GreatCircleDestinationTime_1>("GreatCircleDestinationTime")); // GreatCircleDestinationTime(WsfTrack)
   AddMethod(ut::make_unique<GreatCircleDestinationTime_2>("GreatCircleDestinationTime")); // GreatCircleDestinationTime(srcLat,
                                                                                           // srcLon, dstLat, dstLon)

   AddMethod(ut::make_unique<ApparentTime>());

   AddMethod(ut::make_unique<ConvertWCSToECS>()); // NO WIKI | DEPRECATED
   AddMethod(ut::make_unique<ConvertWCSToECS_2>("ConvertWCSToECS"));
   AddMethod(ut::make_unique<ConvertECSToWCS>());
   AddMethod(ut::make_unique<ConvertWCSToECI>());
   AddMethod(ut::make_unique<ConvertECIToWCS>());

   AddMethod(ut::make_unique<WithinZone>());           // WithinZone(string)
   AddMethod(ut::make_unique<WithinZoneOf>());         // WithinZoneOf(WsfPlatform, string)
   AddMethod(ut::make_unique<Zone>());                 // WsfZone zone = Zone(string)
   AddMethod(ut::make_unique<ZoneNames>("ZoneNames")); // Array<string> zones = ZoneNames()
   AddMethod(ut::make_unique<Zones>());                // Array<WsfZone> zones = Zones()

   AddMethod(ut::make_unique<DeletePlatform>());

   WsfScriptAuxDataUtil::AddAuxDataScriptMethods(*this);

   AddMethod(ut::make_unique<MakeTrack>());

   AddMethod(ut::make_unique<CategoryMemberOf>());
   AddMethod(ut::make_unique<AddCategory>());

   AddMethod(ut::make_unique<SwapMover_1>("SwapMover")); // SwapMover(string)
   AddMethod(ut::make_unique<SwapMover_2>("SwapMover")); // SwapMover(WsfMover)

   AddMethod(ut::make_unique<GroupMemberOf_2>("GroupMemberOf")); // GroupMemberOf(string)
   AddMethod(ut::make_unique<GroupMemberOf_3>("GroupMemberOf")); // GroupMemberOf(WsfGroup)
   AddMethod(ut::make_unique<GetGroupByIndex>("Group"));         // WsfGroup Group(int aIndex)
   AddMethod(ut::make_unique<GetGroupByName>("Group"));          // WsfGroup Group(string GroupName)
   AddMethod(ut::make_unique<GroupsByName>());                   // returns Array<string>
   AddMethod(ut::make_unique<Groups>());                         // returns Array<WsfGroup>
   AddMethod(ut::make_unique<GroupJoin>());                      // GroupJoin(int)
   AddMethod(ut::make_unique<GroupLeave>());                     // GroupLeave(int)

   AddMethod(ut::make_unique<PrintComponentInfo>());
   AddMethod(ut::make_unique<GetComponentInfo>());
}

// virtual
bool WsfScriptPlatformClass::EqualTo(void* aLhsPtr, void* aRhsPtr)
{
   // Two platforms are 'equal' if the pointers to the platform pointers are the same.
   return (aLhsPtr == aRhsPtr);
}

// virtual
bool WsfScriptPlatformClass::LessThan(void* aLHS, void* aRHS)
{
   auto lhs = static_cast<WsfPlatform*>(aLHS);
   auto rhs = static_cast<WsfPlatform*>(aRHS);
   // First verify platforms have been assigned to a simulation (unique ids are non-zero)
   assert(lhs->GetUniqueId() != 0 && rhs->GetUniqueId() != 0);
   // Platform ordering is defined by its unique id within the simulation
   return (lhs->GetUniqueId() < rhs->GetUniqueId());
}

// virtual
std::string WsfScriptPlatformClass::ToString(void* aObjectPtr) const
{
   WsfPlatform* platformPtr = nullptr;
   if (aObjectPtr != nullptr)
   {
      // Currently no way to check if platform is valid!
      platformPtr = (WsfPlatform*)aObjectPtr;
   }
   std::stringstream ss;
   ss << "WsfPlatform(";
   if (platformPtr != nullptr)
   {
      ss << "Name: " << platformPtr->GetName() << ", Index: " << platformPtr->GetIndex()
         << ", Type: " << platformPtr->GetType();
   }
   else
   {
      ss << "null";
   }
   ss << ')';
   return ss.str();
}

// virtual
void WsfScriptPlatformClass::OnNewScriptRef(UtScriptRef& aReference)
{
   size_t       platformIndex = 0;
   WsfPlatform* platformPtr   = static_cast<WsfPlatform*>(aReference.GetAppObject());
   if (platformPtr != nullptr)
   {
      platformIndex = platformPtr->GetIndex();
      if (aReference.GetMemManagement() != UtScriptRef::cMANAGE)
      {
         aReference.SetExternalReference(platformPtr->GetReferenceCount());
      }
   }
   // Set the auxiliary data as the platform index.
   aReference.SetAuxData(static_cast<int>(platformIndex));
}

#include "script/WsfScriptDefs.hpp"

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfScriptPlatformClass, IsA_TypeOf, 2, "bool", "string, string")
{
   bool         isA_TypeOf = false;
   WsfPlatform* objectPtr  = WsfPlatformTypes::Get(*SCENARIO).Find(aVarArgs[0].GetString());
   if (objectPtr != nullptr)
   {
      isA_TypeOf = objectPtr->IsA_TypeOf(aVarArgs[1].GetString());
   }
   aReturnVal.SetBool(isA_TypeOf);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Index, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetIndex()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, CreationTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetCreationTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetCreationTime, 1, "void", "double")
{
   aObjectPtr->SetCreationTime(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, TimeSinceCreation, 0, "double", "")
{
   double simTime = TIME_NOW;
   aReturnVal.SetDouble(simTime - aObjectPtr->GetCreationTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetSide, 1, "void", "string")
{
   aObjectPtr->SetSide(aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Side, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetSideId());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetIcon, 1, "void", "string")
{
   aObjectPtr->SetIcon(aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Icon, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetIconId());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Commander_1, 0, "WsfPlatform", "")
{
   WsfPlatform*     platformPtr = nullptr;
   WsfCommandChain* chainPtr = aObjectPtr->template GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId());
   if (chainPtr != nullptr)
   {
      platformPtr = chainPtr->GetCommander();
   }
   aReturnVal.SetPointer(new UtScriptRef(platformPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Commander_2, 1, "WsfPlatform", "string")
{
   WsfPlatform* platformPtr = nullptr;
   // Argument 1: string aCommandChainName (if an empty string then use the default name)
   WsfStringId chainNameId = WsfStringId(aVarArgs[0].GetString());
   if (!chainNameId)
   {
      chainNameId = WsfCommandChain::GetDefaultNameId();
   }
   WsfCommandChain* chainPtr = aObjectPtr->template GetComponent<WsfCommandChain>(chainNameId);
   if (chainPtr != nullptr)
   {
      platformPtr = chainPtr->GetCommander();
   }
   aReturnVal.SetPointer(new UtScriptRef(platformPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetCommander_1, 1, "void", "WsfPlatform")
{
   WsfPlatform*     platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfCommandChain* chainPtr = aObjectPtr->template GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId());
   if (platformPtr != nullptr && chainPtr != nullptr)
   {
      chainPtr->SetCommander(platformPtr);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetCommander_2, 2, "void", "string, WsfPlatform")
{
   // Argument 1: string aCommandChainName (if an empty string then use the default name)
   WsfStringId chainNameId = WsfStringId(aVarArgs[0].GetString());
   if (!chainNameId)
   {
      chainNameId = WsfCommandChain::GetDefaultNameId();
   }
   WsfPlatform*     platformPtr = (WsfPlatform*)aVarArgs[1].GetPointer()->GetAppObject();
   WsfCommandChain* chainPtr    = aObjectPtr->template GetComponent<WsfCommandChain>(chainNameId);
   if (platformPtr != nullptr && chainPtr != nullptr)
   {
      chainPtr->SetCommander(platformPtr);
   }
}

//! string commanderName = CommanderName()
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, CommanderName_1, 0, "string", "")
{
   WsfStringId      nameId;
   WsfCommandChain* chainPtr = aObjectPtr->template GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId());
   if (chainPtr != nullptr)
   {
      nameId = chainPtr->GetCommanderNameId();
   }
   aReturnVal.SetString(nameId);
}

//! string commanderName = CommanderName(string aChainName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, CommanderName_2, 1, "string", "string")
{
   WsfStringId nameId;
   // Argument 1: string aCommandChainName (if an empty string then use the default name)
   WsfStringId chainNameId = WsfStringId(aVarArgs[0].GetString());
   if (!chainNameId)
   {
      chainNameId = WsfCommandChain::GetDefaultNameId();
   }
   WsfCommandChain* chainPtr = aObjectPtr->template GetComponent<WsfCommandChain>(chainNameId);
   if (chainPtr != nullptr)
   {
      nameId = chainPtr->GetCommanderNameId();
   }
   aReturnVal.SetString(nameId);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Peers_1, 0, "WsfPlatformList", "")
{
   WsfCommandChain::PlatformList* platformListPtr = nullptr;
   WsfCommandChain* chainPtr = aObjectPtr->template GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId());
   if (chainPtr != nullptr)
   {
      platformListPtr = &(chainPtr->GetPeers());
   }
   aReturnVal.SetPointer(new UtScriptRef(platformListPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Peers_2, 1, "WsfPlatformList", "string")
{
   WsfCommandChain::PlatformList* platformListPtr = nullptr;
   // Argument 1: string aCommandChainName (if an empty string then use the default name)
   WsfStringId chainNameId = WsfStringId(aVarArgs[0].GetString());
   if (!chainNameId)
   {
      chainNameId = WsfCommandChain::GetDefaultNameId();
   }
   WsfCommandChain* chainPtr = aObjectPtr->template GetComponent<WsfCommandChain>(chainNameId);
   if (chainPtr != nullptr)
   {
      platformListPtr = &(chainPtr->GetPeers());
   }
   aReturnVal.SetPointer(new UtScriptRef(platformListPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Subordinates_1, 0, "WsfPlatformList", "")
{
   WsfCommandChain::PlatformList* platformListPtr = nullptr;
   WsfCommandChain* chainPtr = aObjectPtr->template GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId());
   if (chainPtr != nullptr)
   {
      platformListPtr = &(chainPtr->GetSubordinates());
   }
   aReturnVal.SetPointer(new UtScriptRef(platformListPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Subordinates_2, 1, "WsfPlatformList", "string")
{
   WsfCommandChain::PlatformList* platformListPtr(nullptr);
   // Argument 1: string aCommandChainName (if an empty string then use the default name)
   WsfStringId chainNameId(aVarArgs[0].GetString());
   if (!chainNameId)
   {
      chainNameId = WsfCommandChain::GetDefaultNameId();
   }
   WsfCommandChain* chainPtr = aObjectPtr->template GetComponent<WsfCommandChain>(chainNameId);
   if (chainPtr != nullptr)
   {
      platformListPtr = &(chainPtr->GetSubordinates());
   }
   aReturnVal.SetPointer(new UtScriptRef(platformListPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, CommandChain, 1, "WsfCommandChain", "string")
{
   // Argument 1: string aCommandChainName (if an empty string then use the default name)
   WsfStringId chainNameId(WsfStringId(aVarArgs[0].GetString()));
   if (!chainNameId)
   {
      chainNameId = WsfCommandChain::GetDefaultName();
   }
   WsfCommandChain* chainPtr = aObjectPtr->template GetComponent<WsfCommandChain>(chainNameId);
   aReturnVal.SetPointer(new UtScriptRef(chainPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, CommandChainCount, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->template GetComponentCount<WsfCommandChain>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, CommandChainEntry, 1, "WsfCommandChain", "int")
{
   // Argument 1: int aEntryIndex
   unsigned int     index    = static_cast<unsigned int>(aVarArgs[0].GetInt());
   WsfCommandChain* chainPtr = aObjectPtr->template GetComponentEntry<WsfCommandChain>(index);
   if (chainPtr == nullptr)
   {
      UT_SCRIPT_ABORT("Bad CommandChainEntry index");
   }
   aReturnVal.SetPointer(new UtScriptRef(chainPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetMoverDebugEnabled, 1, "void", "bool")
{
   aObjectPtr->GetMover()->SetDebugEnabled(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Comment_1, 2, "void", "double, string")
{
   aObjectPtr->Comment(aVarArgs[0].GetDouble(), aVarArgs[1].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Comment_2, 1, "void", "string")
{
   aObjectPtr->Comment(TIME_NOW, aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, IsExternallyControlled, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsExternallyControlled());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SpatialDomain, 0, "string", "")
{
   WsfSpatialDomain domain = aObjectPtr->GetSpatialDomain();
   aReturnVal.SetString(WsfTypes::EnumToString(domain));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, WCS_To_ECS_DCM, 0, "DCM", "")
{
   UtDCM* dcmPtr = new UtDCM(aObjectPtr->GetDcmWCStoECS());
   aReturnVal.SetPointer(new UtScriptRef(dcmPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, WCS_To_NED_DCM, 0, "DCM", "")
{
   UtDCM* dcmPtr = new UtDCM(aObjectPtr->GetDcmWCStoNED());
   aReturnVal.SetPointer(new UtScriptRef(dcmPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, LineOfSightRatesTo, 1, "Vec3X", "WsfPlatform")
{
   WsfPlatform* tgtPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   tgtPtr->Update(TIME_NOW);
   aObjectPtr->Update(TIME_NOW);
   UtVec3dX observerLoc, targetLoc, observerVel, targetVel;
   aObjectPtr->GetLocationWCS(observerLoc.GetData());
   aObjectPtr->GetVelocityWCS(observerVel.GetData());
   tgtPtr->GetLocationWCS(targetLoc.GetData());
   tgtPtr->GetVelocityWCS(targetVel.GetData());
   UtVec3dX losrI = UtVec3dX::LineOfSightRates(observerLoc, observerVel, targetLoc, targetVel);
   // Transform from inertial to ECS frame, and also scale up to degrees per second.
   UtVec3dX* losrEcsPtr = new UtVec3dX(aObjectPtr->GetDcmWCStoECS() * losrI * UtMath::cDEG_PER_RAD);
   aReturnVal.SetPointer(new UtScriptRef(losrEcsPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// ----------------------------------------------------------------------------
//                         Sub-system management methods
// ----------------------------------------------------------------------------

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Mover, 0, "WsfMover", "")
{
   aReturnVal.SetPointer(UtScriptRef::Ref(aObjectPtr->GetMover(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Comm, 1, "WsfComm", "string")
{
   // Argument 1: string aCommName
   aReturnVal.SetPointer(
      UtScriptRef::Ref(aObjectPtr->template GetComponent<wsf::comm::Comm>(WsfStringId(aVarArgs[0].GetString())),
                       aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, CommCount, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->template GetComponentCount<wsf::comm::Comm>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, CommEntry, 1, "WsfComm", "int")
{
   // Argument 1: int aEntryIndex
   unsigned commIndex = (unsigned)aVarArgs[0].GetInt();
   if (commIndex >= aObjectPtr->template GetComponentCount<wsf::comm::Comm>())
   {
      UT_SCRIPT_ABORT("Bad CommEntry index");
   }
   wsf::comm::Comm* commPtr = aObjectPtr->template GetComponentEntry<wsf::comm::Comm>(commIndex);
   aReturnVal.SetPointer(UtScriptRef::Ref(commPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Router, 1, "WsfCommRouter", "string")
{
   aReturnVal.SetPointer(
      UtScriptRef::Ref(aObjectPtr->template GetComponent<wsf::comm::router::Router>(WsfStringId(aVarArgs[0].GetString())),
                       aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, RouterCount, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->template GetComponentCount<wsf::comm::router::Router>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, RouterEntry, 1, "WsfCommRouter", "int")
{
   auto routerIndex = ut::safe_cast<unsigned int, int>(aVarArgs[0].GetInt());
   if (routerIndex >= aObjectPtr->template GetComponentCount<wsf::comm::router::Router>())
   {
      UT_SCRIPT_ABORT("Bad RouterEntry index");
   }
   wsf::comm::router::Router* routerPtr = aObjectPtr->template GetComponentEntry<wsf::comm::router::Router>(routerIndex);
   aReturnVal.SetPointer(UtScriptRef::Ref(routerPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Fuel, 0, "WsfFuel", "")
{
   aReturnVal.SetPointer(UtScriptRef::Ref(aObjectPtr->template GetComponent<WsfFuel>(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, FuelRemaining, 0, "double", "") // NO_DOC | DEPRECATED
{
   double   fuelRemaining = std::numeric_limits<double>::max();
   WsfFuel* fuelPtr       = aObjectPtr->template GetComponent<WsfFuel>();
   if (fuelPtr != nullptr)
   {
      fuelRemaining = fuelPtr->GetQuantityRemaining();
   }
   aReturnVal.SetDouble(fuelRemaining);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Refuel_1, 0, "bool", "") // NO_DOC | DEPRECATED
{
   bool     refueled = true;
   WsfFuel* fuelPtr  = aObjectPtr->template GetComponent<WsfFuel>();
   if (fuelPtr != nullptr)
   {
      double simTime = TIME_NOW;
      refueled       = fuelPtr->Refuel(simTime);
   }
   aReturnVal.SetBool(refueled);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Refuel_2, 1, "bool", "double") // NO_DOC | DEPRECATED
{
   bool     refueled = true;
   WsfFuel* fuelPtr  = aObjectPtr->template GetComponent<WsfFuel>();
   if (fuelPtr != nullptr)
   {
      double refuelQuantity = aVarArgs[0].GetDouble();
      double simTime        = TIME_NOW;
      refueled              = fuelPtr->Refuel(simTime, refuelQuantity);
   }
   aReturnVal.SetBool(refueled);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, FuelBingoQuantity, 0, "double", "") // NO_DOC | DEPRECATED
{
   double   fuelBingo = 0;
   WsfFuel* fuelPtr   = aObjectPtr->template GetComponent<WsfFuel>();
   if (fuelPtr != nullptr)
   {
      fuelBingo = fuelPtr->GetBingoQuantity();
   }
   aReturnVal.SetDouble(fuelBingo);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GPS_Status, 0, "int", "")
{
   int                  GPS_Status  = 0;
   WsfNavigationErrors* navErrorPtr = aObjectPtr->template GetComponent<WsfNavigationErrors>();
   if (navErrorPtr != nullptr)
   {
      GPS_Status = navErrorPtr->GetGPS_Status();
   }
   aReturnVal.SetInt(GPS_Status);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetGPS_Status, 1, "bool", "int")
{
   bool                 statusChanged(false);
   WsfNavigationErrors* navErrorPtr = aObjectPtr->template GetComponent<WsfNavigationErrors>();
   if (navErrorPtr != nullptr)
   {
      double simTime = TIME_NOW;
      statusChanged  = navErrorPtr->SetGPS_Status(simTime, WsfNavigationErrors::GetGPS_Status(aVarArgs[0].GetInt()));
   }
   aReturnVal.SetBool(statusChanged);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetPerceivedLocationErrorWCS, 1, "void", "Vec3")
{
   UtVec3d*             vec3Ptr     = (UtVec3d*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfNavigationErrors* navErrorPtr = aObjectPtr->template GetComponent<WsfNavigationErrors>();
   if (navErrorPtr != nullptr)
   {
      navErrorPtr->SetLocationErrorWCS(ut::coords::WCS(*vec3Ptr));
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Processor, 1, "WsfProcessor", "string")
{
   // Argument 1: string aName
   WsfProcessor* processorPtr = aObjectPtr->template GetComponent<WsfProcessor>(WsfStringId(aVarArgs[0].GetString()));
   aReturnVal.SetPointer(UtScriptRef::Ref(processorPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, ProcessorCount, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->template GetComponentCount<WsfProcessor>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, ProcessorEntry, 1, "WsfProcessor", "int")
{
   // Argument 1: int aEntryIndex
   unsigned index = (unsigned)aVarArgs[0].GetInt();
   if (index >= aObjectPtr->template GetComponentCount<WsfProcessor>())
   {
      UT_SCRIPT_ABORT("Bad ProcessorEntry index");
   }
   WsfProcessor* processorPtr = aObjectPtr->template GetComponentEntry<WsfProcessor>(index);
   aReturnVal.SetPointer(UtScriptRef::Ref(processorPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Sensor, 1, "WsfSensor", "string")
{
   // Argument 1: string aName
   WsfSensor* sensorPtr = aObjectPtr->template GetComponent<WsfSensor>(WsfStringId(aVarArgs[0].GetString()));
   aReturnVal.SetPointer(UtScriptRef::Ref(sensorPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SensorCount, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->template GetComponentCount<WsfSensor>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SensorEntry, 1, "WsfSensor", "int")
{
   // Argument 1: int aEntryIndex
   unsigned index = (unsigned)aVarArgs[0].GetInt();
   if (index >= aObjectPtr->template GetComponentCount<WsfSensor>())
   {
      UT_SCRIPT_ABORT("Bad SensorEntry index");
   }
   WsfSensor* sensorPtr = aObjectPtr->template GetComponentEntry<WsfSensor>(index);
   aReturnVal.SetPointer(UtScriptRef::Ref(sensorPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, VisualPart, 1, "WsfVisualPart", "string") // NO_DOC | FOR_TEST_ONLY
{
   // Argument 1: string aName
   WsfVisualPart* visualPartPtr = aObjectPtr->template GetComponent<WsfVisualPart>(aVarArgs[0].GetString());
   aReturnVal.SetPointer(UtScriptRef::Ref(visualPartPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, TurnCommOn, 1, "bool", "string")
{
   // Argument 1: string aName
   bool             ok      = false;
   wsf::comm::Comm* commPtr = aObjectPtr->template GetComponent<wsf::comm::Comm>(aVarArgs[0].GetString());
   if (commPtr != nullptr)
   {
      double simTime = TIME_NOW;
      ok             = SIMULATION->TurnPartOn(simTime, commPtr);
   }
   else
   {
      auto out = ut::log::error() << "Could not find comm.";
      out.AddNote() << "Comm: " << aVarArgs[0].GetString();
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, TurnCommOff, 1, "bool", "string")
{
   // Argument 1: string aName
   bool             ok      = false;
   wsf::comm::Comm* commPtr = aObjectPtr->template GetComponent<wsf::comm::Comm>(aVarArgs[0].GetString());
   if (commPtr != nullptr)
   {
      double simTime = TIME_NOW;
      ok             = SIMULATION->TurnPartOff(simTime, commPtr);
   }
   else
   {
      auto out = ut::log::error() << "Could not find comm.";
      out.AddNote() << "Comm: " << aVarArgs[0].GetString();
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, TurnProcessorOn, 1, "bool", "string")
{
   // Argument 1: string aName
   bool          ok           = false;
   WsfProcessor* processorPtr = aObjectPtr->template GetComponent<WsfProcessor>(aVarArgs[0].GetString());
   if (processorPtr != nullptr)
   {
      double simTime = TIME_NOW;
      ok             = SIMULATION->TurnPartOn(simTime, processorPtr);
   }
   else
   {
      auto out = ut::log::error() << "Could not find processor.";
      out.AddNote() << "Processor: " << aVarArgs[0].GetString();
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, TurnProcessorOff, 1, "bool", "string")
{
   // Argument 1: string aName
   bool          ok           = false;
   WsfProcessor* processorPtr = aObjectPtr->template GetComponent<WsfProcessor>(aVarArgs[0].GetString());
   if (processorPtr != nullptr)
   {
      double simTime = TIME_NOW;
      ok             = SIMULATION->TurnPartOff(simTime, processorPtr);
   }
   else
   {
      auto out = ut::log::error() << "Could not find processor.";
      out.AddNote() << "Processor: " << aVarArgs[0].GetString();
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, TurnMoverOn, 0, "bool", "")
{
   // Argument 1: string aName
   bool      ok       = false;
   WsfMover* MoverPtr = aObjectPtr->GetMover();
   if (MoverPtr != nullptr)
   {
      double simTime = TIME_NOW;
      ok             = SIMULATION->TurnPartOn(simTime, MoverPtr);
   }
   else
   {
      auto out = ut::log::error() << "Could not find mover.";
      out.AddNote() << "Mover: " << aVarArgs[0].GetString();
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, TurnMoverOff, 0, "bool", "")
{
   // Argument 1: string aName
   bool      ok       = false;
   WsfMover* MoverPtr = aObjectPtr->GetMover();
   if (MoverPtr != nullptr)
   {
      double simTime = TIME_NOW;
      ok             = SIMULATION->TurnPartOff(simTime, MoverPtr);
   }
   else
   {
      auto out = ut::log::error() << "Could not find mover.";
      out.AddNote() << "Mover: " << aVarArgs[0].GetString();
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, TurnSensorOn, 1, "bool", "string")
{
   // Argument 1: string aName
   bool       ok        = false;
   WsfSensor* sensorPtr = aObjectPtr->template GetComponent<WsfSensor>(aVarArgs[0].GetString());
   if (sensorPtr != nullptr)
   {
      double simTime = TIME_NOW;
      ok             = SIMULATION->TurnPartOn(simTime, sensorPtr);
   }
   else
   {
      auto out = ut::log::error() << "Could not find sensor.";
      out.AddNote() << "Sensor: " << aVarArgs[0].GetString();
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, TurnSensorOff, 1, "bool", "string")
{
   // Argument 1: string aName
   bool       ok        = false;
   WsfSensor* sensorPtr = aObjectPtr->template GetComponent<WsfSensor>(aVarArgs[0].GetString());
   if (sensorPtr != nullptr)
   {
      double simTime = TIME_NOW;
      ok             = SIMULATION->TurnPartOff(simTime, sensorPtr);
   }
   else
   {
      auto out = ut::log::error() << "Could not find sensor.";
      out.AddNote() << "Sensor: " << aVarArgs[0].GetString();
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, TurnRouterOn, 1, "bool", "string")
{
   // Argument 1: string aName
   bool ok        = false;
   auto routerPtr = aObjectPtr->template GetComponent<wsf::comm::router::Router>(aVarArgs[0].GetString());
   if (routerPtr)
   {
      double simTime = TIME_NOW;
      ok             = SIMULATION->TurnPartOn(simTime, routerPtr);
   }
   else
   {
      auto out = ut::log::error() << "Could not find router.";
      out.AddNote() << "Router: " << aVarArgs[0].GetString();
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, TurnRouterOff, 1, "bool", "string")
{
   // Argument 1: string aName
   bool ok        = false;
   auto routerPtr = aObjectPtr->template GetComponent<wsf::comm::router::Router>(aVarArgs[0].GetString());
   if (routerPtr != nullptr)
   {
      double simTime = TIME_NOW;
      ok             = SIMULATION->TurnPartOff(simTime, routerPtr);
   }
   else
   {
      auto out = ut::log::error() << "Could not find router.";
      out.AddNote() << "Router: " << aVarArgs[0].GetString();
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, TrackManager, 0, "WsfTrackManager", "")
{
   WsfTrackManager& trackManager = aObjectPtr->GetTrackManager();
   aReturnVal.SetPointer(new UtScriptRef(&trackManager, aReturnClassPtr));
}

// ----------------------------------------------------------------------------
//                           Appearance management
// ----------------------------------------------------------------------------

//! Gets the length of the platform
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Length, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetLength());
}

//! Gets the width of the platform
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Width, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetWidth());
}

//! Gets the height of the platform
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Height, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetHeight());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, TotalMass, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMass());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, EmptyMass, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetEmptyMass());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, FuelMass, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetFuelMass());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, PayloadMass, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetPayloadMass());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, AfterburnerFactor, 0, "int", "") // NO_DOC | DEPRECATED
{
   // Don't update the wiki with this function; encourage users to use SetAppearance instead
   aReturnVal.SetInt(aObjectPtr->GetAppearance(16, 1));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetAfterburnerFactor, 1, "void", "int") // NO_DOC | DEPRECATED
{
   // Don't update the wiki with this function; encourage users to use SetAppearance instead
   // Argument 1: int Afterburner factor [0..1].
   // Cast to an unsigned int; leaving script as int
   unsigned int afterburnerValue = static_cast<unsigned int>(aVarArgs[0].GetInt());
   aObjectPtr->SetAppearance(16, 1, afterburnerValue, true);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, ConfigurationState, 0, "int", "") // NO_DOC | DEPRECATED
{
   // Don't update the wiki with this function; encourage users to use SetAppearance instead
   aReturnVal.SetInt(aObjectPtr->GetAppearance(17, 4));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetConfigurationState, 1, "void", "int") // NO_DOC | DEPRECATED
{
   // Don't update the wiki with this function; encourage users to use SetAppearance instead
   // Argument 1: Configuration state integer[0..15]
   int configurationState = aVarArgs[0].GetInt();
   if (configurationState < 0 || configurationState > 15)
   {
      auto out = ut::log::warning() << "Invalid configuration state.";
      out.AddNote() << "Expected an integer in the range [0..15].";
      out.AddNote() << "Provided: " << configurationState;
      out.AddNote() << "Script: WsfScriptPlatformClass::SetConfigurationState";
   }
   else
   {
      aObjectPtr->SetAppearance(17, 4, static_cast<unsigned int>(configurationState), true);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, ConcealmentFactor, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetConcealmentFactor());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetConcealmentFactor, 1, "void", "double")
{
   // Argument 1: double aConcealmentFactor [0..1]
   float concealFactor = static_cast<float>(aVarArgs[0].GetDouble());
   if (concealFactor < 0.0F)
   {
      auto out = ut::log::warning() << "Invalid concealment factor.";
      out.AddNote() << "Expected a value in the range [0..1].";
      out.AddNote() << "Provided: " << concealFactor;
      out.AddNote() << "Using Value: 0";
      out.AddNote() << "Script: WsfScriptPlatformClass::SetConcealmentFactor";

      concealFactor = 0.0F;
   }
   else if (concealFactor > 1.0F)
   {
      auto out = ut::log::warning() << "Invalid concealment factor.";
      out.AddNote() << "Expected a value in the range [0..1].";
      out.AddNote() << "Provided: " << concealFactor;
      out.AddNote() << "Using Value: 1";
      out.AddNote() << "Script: WsfScriptPlatformClass::SetConcealmentFactor";

      concealFactor = 1.0F;
   }
   aObjectPtr->SetConcealmentFactor(concealFactor);

   // Set appearance word; only works for land domain; ignores life forms & sensor/emitters
   WsfMover* moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_LAND)
      {
         aObjectPtr->SetAppearance(19, 1, (concealFactor > 0.99), true);
      }
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Indestructible, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsIndestructible());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetIndestructible, 1, "void", "bool")
{
   aObjectPtr->SetIsIndestructible(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetSignatureState, 1, "void", "string")
{
   WsfStringId stateId = WsfStringId(aVarArgs[0].GetString());
   aObjectPtr->GetSignatureList().SetState(stateId, "all");
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetDamageFactor, 1, "void", "double")
{
   double damagefactor = std::min(1.0, std::max(0.0, aVarArgs[0].GetDouble()));
   aObjectPtr->SetDamageFactor(damagefactor);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, DamageFactor, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetDamageFactor());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetTrailingEffects, 1, "void", "int")
{
   aObjectPtr->SetAppearance(7, 2, aVarArgs[0].GetInt(), true);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Appearance, 0, "int", "")
{
   // Returns an unsigned long
   aReturnVal.SetInt(aObjectPtr->GetAppearance());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetAppearance, 3, "void", "int, int, int")
{
   aObjectPtr->SetAppearance(static_cast<unsigned int>(aVarArgs[0].GetInt()),
                             static_cast<unsigned int>(aVarArgs[1].GetInt()),
                             static_cast<unsigned int>(aVarArgs[2].GetInt()),
                             true);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Capabilities, 0, "int", "")
{
   // Returns an unsigned long
   aReturnVal.SetInt(aObjectPtr->GetCapabilities());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetCapability, 2, "void", "string, bool")
{
   unsigned int                oldCapabilitiesWord = aObjectPtr->GetCapabilities();
   unsigned int                newCapabilitiesWord = oldCapabilitiesWord;
   WsfExchange::CapabilityType capability          = WsfExchange::NameIdToCapability(aVarArgs[0].GetString());
   WsfExchange::SetCapabilityFlag(newCapabilitiesWord, capability, aVarArgs[1].GetBool());
   aObjectPtr->SetCapabilities(newCapabilitiesWord);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetMarking, 1, "void", "string")
{
   aObjectPtr->SetMarking(aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Marking, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetMarking());
}

// ----------------------------------------------------------------------------
//                            Script invocation methods
// ----------------------------------------------------------------------------

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, ScriptExists, 1, "bool", "string")
{
   // Argument 1: script name
   WsfStringId scriptId = WsfStringId(aVarArgs[0].GetString());
   bool        exists   = (aObjectPtr->GetScriptContext().FindScript(scriptId) != nullptr);
   aReturnVal.SetBool(exists);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Execute_1, 1, "Object", "string")
{
   // Argument 1: script name
   UtScriptData     rv(0);
   UtScriptDataList args;
   double           simTime = TIME_NOW;
   if (!aObjectPtr->ExecuteScript(simTime, aVarArgs[0].GetString(), rv, args))
   {
      auto out = ut::log::error() << "Could not execute script.";
      out.AddNote() << "Script: " << aVarArgs[0].GetString();
   }
   aReturnVal = rv;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Execute_2, 2, "Object", "string, Array<Object>")
{
   // Argument 1: script name
   // Argument 2: the argument array.
   UtScriptData      rv(0);
   UtScriptDataList* argsPtr = (UtScriptDataList*)aVarArgs[1].GetPointer()->GetAppObject();
   double            simTime = TIME_NOW;
   if (!aObjectPtr->ExecuteScript(simTime, aVarArgs[0].GetString(), rv, *argsPtr))
   {
      auto out = ut::log::error() << "Could not execute script.";
      out.AddNote() << "Script: " << aVarArgs[0].GetString();
   }
   aReturnVal = rv;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, ExecuteAtTime_1, 2, "bool", "double, string")
{
   // Argument 1: time to execute script
   // Argument 2: script name
   if (aObjectPtr->HasScript(aVarArgs[1].GetString()))
   {
      double time = aVarArgs[0].GetDouble();
      aObjectPtr->ExecuteScriptAtTime(time, aVarArgs[1].GetString(), UtScriptDataList());
      aReturnVal.SetBool(true);
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, ExecuteAtTime_2, 3, "bool", "double, string, Array<Object>")
{
   // Argument 1: time to execute script
   // Argument 2: script name
   // Argument 3: script args
   if (aObjectPtr->HasScript(aVarArgs[1].GetString()))
   {
      double time    = aVarArgs[0].GetDouble();
      auto   argsPtr = aVarArgs[2].GetPointer()->GetAppObject<UtScriptDataList>();
      aObjectPtr->ExecuteScriptAtTime(time, aVarArgs[1].GetString(), *argsPtr);
      aReturnVal.SetBool(true);
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, ExecuteScript, 1, "bool", "string")
{
   // Argument 1: script name
   double simTime = TIME_NOW;
   aReturnVal.SetBool(aObjectPtr->ExecuteScript(simTime, aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, ExecuteScriptWithArgs, 2, "bool", "string, Array<Object>")
{
   // Argument 1: script name
   // Argument 2: the argument array.
   UtScriptDataList* argsPtr = (UtScriptDataList*)aVarArgs[1].GetPointer()->GetAppObject();
   double            simTime = TIME_NOW;
   UtScriptData      ret;
   aReturnVal.SetBool(aObjectPtr->ExecuteScript(simTime, aVarArgs[0].GetString(), ret, *argsPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, ExecuteGlobalScript, 1, "bool", "string")
{
   // Argument 1: script name
   double simTime = TIME_NOW;
   aReturnVal.SetBool(SIMULATION->GetScriptContext().ExecuteScript(simTime, aVarArgs[0].GetString()));
}

// ----------------------------------------------------------------------------
//                            Track methods
// ----------------------------------------------------------------------------

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, MasterTrackList, 0, "WsfLocalTrackList", "")
{
   aReturnVal.SetPointer(new UtScriptRef(&aObjectPtr->GetMasterTrackList(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, MasterRawTrackList, 0, "WsfTrackList", "")
{
   aReturnVal.SetPointer(new UtScriptRef(&aObjectPtr->GetMasterRawTrackList(), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, TrackList, 1, "WsfLocalTrackList", "string")
{
   WsfLocalTrackList* localTrackListPtr = nullptr;
   WsfProcessor*      procPtr           = aObjectPtr->template GetComponent<WsfProcessor>(aVarArgs[0].GetString());
   WsfTrackProcessor* trackProcPtr      = dynamic_cast<WsfTrackProcessor*>(procPtr);
   if (trackProcPtr != nullptr)
   {
      localTrackListPtr = &trackProcPtr->GetTrackManager().GetTrackList();
   }
   aReturnVal.SetPointer(new UtScriptRef(localTrackListPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, RawTrackList, 1, "WsfTrackList", "string")
{
   WsfTrackList*      trackListPtr = nullptr;
   WsfProcessor*      procPtr      = aObjectPtr->template GetComponent<WsfProcessor>(aVarArgs[0].GetString());
   WsfTrackProcessor* trackProcPtr = dynamic_cast<WsfTrackProcessor*>(procPtr);
   if (trackProcPtr != nullptr)
   {
      trackListPtr = &trackProcPtr->GetTrackManager().GetRawTrackList();
   }
   aReturnVal.SetPointer(new UtScriptRef(trackListPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, CurrentTarget, 0, "WsfTrackId", "")
{
   WsfTrackId* trackIdPtr = new WsfTrackId(aObjectPtr->GetTrackManager().GetCurrentTargetTrackId());
   aReturnVal.SetPointer(new UtScriptRef(trackIdPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetCurrentTarget, 1, "WsfTrackId", "WsfTrack")
{
   // Argument 1: the track that is to be declared the 'current target'.
   WsfTrack*   trackPtr   = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double      simTime    = TIME_NOW;
   WsfTrackId* trackIdPtr = new WsfTrackId(aObjectPtr->GetTrackManager().SetCurrentTarget(simTime, *trackPtr));
   aReturnVal.SetPointer(new UtScriptRef(trackIdPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, HasCurrentTarget, 0, "bool", "")
{
   bool hasCurrentTarget(true);
   if (aObjectPtr->GetTrackManager().GetCurrentTargetTrackId().IsNull())
   {
      hasCurrentTarget = false;
   }
   aReturnVal.SetBool(hasCurrentTarget);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, ClearCurrentTarget, 0, "void", "")
{
   aObjectPtr->GetTrackManager().ClearCurrentTarget();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, CurrentTargetTrack, 0, "WsfTrack", "")
{
   WsfTrack* trackPtr = const_cast<WsfTrack*>(aObjectPtr->GetTrackManager().GetCurrentTarget());
   aReturnVal.SetPointer(new UtScriptRef(trackPtr, aReturnClassPtr));
}

// ============================================================================

namespace
{
std::ostream& operator<<(std::ostream& aOut, const double (&aVec)[3])
{
   aOut << ' ' << aVec[0] << ' ' << aVec[1] << ' ' << aVec[2];
   return aOut;
}

//! Helps calculate intercept bearing and altitude (if dimensions >= 3)
//! Assumes constant velocity for both target and ownship
double InterceptHelper(double       aSimTime,
                       WsfPlatform* aOwnshipPtr,
                       double       aOwnshipSpeed,
                       WsfTrack*    aTrackPtr,
                       WsfWaypoint* aWaypointPtr,
                       int          aDimensions,
                       bool         aDebugEnabled)
{
   double timeToIntercept = -1.0;
   if ((aTrackPtr != nullptr) && (aWaypointPtr != nullptr))
   {
      // Get the ownship location in WCS
      double ownLocWCS[3];
      aOwnshipPtr->GetLocationWCS(ownLocWCS);

      // Get target location in WCS
      double tgtLocWCS[3];
      aTrackPtr->GetExtrapolatedLocationWCS(aSimTime, tgtLocWCS);

      // Get target velocity in WCS
      double tgtVelWCS[3];
      aTrackPtr->GetVelocityWCS(tgtVelWCS);

      // Compute the intercept time and location.
      double intLocWCS[3];
      timeToIntercept = WsfIntercept::Intercept(ownLocWCS, aOwnshipSpeed, tgtLocWCS, tgtVelWCS, intLocWCS);

      // Convert the intercept location to spherical coordinates and stuff in the waypoint.
      double intLat;
      double intLon;
      double intAlt;
      WsfPlatform::ConvertWCSToLLA(intLocWCS, intLat, intLon, intAlt);
      aWaypointPtr->SetLat(intLat);
      aWaypointPtr->SetLon(intLon);

      // Also compute the 2D heading to the intercept point as some find it useful.
      double intLocNED[3];
      aOwnshipPtr->ConvertWCSToNED(intLocWCS, intLocNED);
      aWaypointPtr->SetHeading(atan2(intLocNED[1], intLocNED[0]));

      // Set the 'extrapolate' option in the waypoint so if it the last point in the mover then
      // it will continue at the current heading.
      aWaypointPtr->SetEndOfPathOption(WsfPath::cEP_EXTRAPOLATE);

      // For 2D we just let it continue at the current altitude.
      // For 3D we use the intercept altitude IF the track has a valid 3D location or elevation data.
      if (aDimensions >= 3)
      {
         if (aTrackPtr->ElevationValid() || aTrackPtr->LocationValid())
         {
            double tgtLat;
            double tgtLon;
            double tgtAlt;
            WsfPlatform::ConvertWCSToLLA(intLocWCS, tgtLat, tgtLon, tgtAlt);
            aWaypointPtr->SetAlt(tgtAlt);
         }
      }

      // Print debug information.
      if (aDebugEnabled)
      {
         auto out = ut::log::debug() << "Intercept computation.";
         out.AddNote() << "T = " << aSimTime;

         // Print ownership information
         double ownLocLLA[3];
         aOwnshipPtr->GetLocationLLA(ownLocLLA[0], ownLocLLA[1], ownLocLLA[2]);
         double ownVelNED[3];
         aOwnshipPtr->GetVelocityNED(ownVelNED);
         double ownHeading = ::atan2(ownVelNED[1], ownVelNED[0]);

         out.AddNote() << "Interceptor: " << aOwnshipPtr->GetName();
         out.AddNote() << "Interceptor: Location LLA [deg,m]: " << ownLocLLA;
         out.AddNote() << "Interceptor: Velocity NED [m/s]: " << ownVelNED;
         out.AddNote() << "Interceptor: Speed: " << UtVec3d::Magnitude(ownVelNED);
         out.AddNote() << "Interceptor: Requested Speed: " << aOwnshipSpeed;
         out.AddNote() << "Interceptor: Heading [deg]: " << UtMath::NormalizeAngle0_360(ownHeading * UtMath::cDEG_PER_RAD);

         // Print target information.
         double tgtLocLLA[3];
         WsfPlatform::ConvertWCSToLLA(tgtLocWCS, tgtLocLLA[0], tgtLocLLA[1], tgtLocLLA[2]);
         double tgtLocNED[3];
         aOwnshipPtr->ConvertWCSToNED(tgtLocWCS, tgtLocNED);
         double tgtVelNED[3];
         aOwnshipPtr->ConvertWCSVectorToNED(tgtVelNED, tgtVelWCS);
         double tgtHeading = ::atan2(tgtVelNED[1], tgtVelNED[0]);

         out.AddNote() << "Target: " << aTrackPtr->GetTargetName();
         out.AddNote() << "Target: Location LLA [deg,m]: " << tgtLocLLA;
         out.AddNote() << "Target: Location NED [m]: " << tgtLocNED;
         out.AddNote() << "Target: Velocity NED [m/s]: " << tgtVelNED;
         out.AddNote() << "Target: Speed [m/s]: " << UtVec3d::Magnitude(tgtVelWCS);
         out.AddNote() << "Target: Heading [deg]: " << UtMath::NormalizeAngle0_360(tgtHeading * UtMath::cDEG_PER_RAD);

         if (timeToIntercept >= 0)
         {
            double intLocLLA[3];
            WsfPlatform::ConvertWCSToLLA(intLocWCS, intLocLLA[0], intLocLLA[1], intLocLLA[2]);
            out.AddNote() << "Time to Intercept: " << timeToIntercept << " sec";
            out.AddNote() << "Intercept Time: " << aSimTime + timeToIntercept << " sec";
            out.AddNote() << "Intercept Location LLA [deg,m]: " << intLocLLA;
            out.AddNote() << "Intercept Location NED [m]: " << intLocNED;
         }
         else
         {
            out.AddNote() << "Intercept: None";
         }
      }
   }
   return timeToIntercept;
}

} // namespace

//! Returns time to intercept and intercept bearing
//! Assumes constant velocity for both target and ownship
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, InterceptLocation2D_1, 2, "double", "WsfTrack, WsfWaypoint")
{
   // Argument 1: Target track <input>
   // Argument 2: Intercept waypoint <output>

   // Ensure source platform position is current
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime);

   // Extract the function arguments
   WsfTrack*    trackPtr    = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfWaypoint* waypointPtr = (WsfWaypoint*)aVarArgs[1].GetPointer()->GetAppObject();

   double timeToIntercept;
   timeToIntercept = InterceptHelper(simTime, aObjectPtr, aObjectPtr->GetSpeed(), trackPtr, waypointPtr, 2, false);
   aReturnVal.SetDouble(timeToIntercept);
}

//! Returns time to intercept and intercept bearing
//! Assumes constant velocity for both target and ownship
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, InterceptLocation2D_2, 3, "double", "WsfTrack, WsfWaypoint, bool")
{
   // Argument 1: Target track <input>
   // Argument 2: Intercept waypoint <output>
   // Argument 3: Debug flag

   // Ensure source platform position is current
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime);

   // Extract the function arguments
   WsfTrack*    trackPtr     = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfWaypoint* waypointPtr  = (WsfWaypoint*)aVarArgs[1].GetPointer()->GetAppObject();
   bool         debugEnabled = aVarArgs[2].GetBool();

   double timeToIntercept;
   timeToIntercept = InterceptHelper(simTime, aObjectPtr, aObjectPtr->GetSpeed(), trackPtr, waypointPtr, 2, debugEnabled);
   aReturnVal.SetDouble(timeToIntercept);
}

//! Returns time to intercept and intercept bearing
//! Assumes constant velocity for both target and ownship
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, InterceptLocation2D_3, 4, "double", "WsfTrack, WsfWaypoint, double, double")
{
   // Argument 1: Target track <input>
   // Argument 2: Intercept waypoint <output>
   // Argument 3: Speed of ownship
   // Argument 4: Time delay until speed is reached

   // Ensure source platform position is current
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime);

   // Extract the function arguments
   WsfTrack*    trackPtr       = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfWaypoint* waypointPtr    = (WsfWaypoint*)aVarArgs[1].GetPointer()->GetAppObject();
   double       speedOfOwnship = aVarArgs[2].GetDouble();
   double       timeDelay      = aVarArgs[3].GetDouble();

   double timeToIntercept;
   timeToIntercept = InterceptHelper(simTime + timeDelay, aObjectPtr, speedOfOwnship, trackPtr, waypointPtr, 2, false);
   aReturnVal.SetDouble(timeToIntercept);
}

//! Returns time to intercept and intercept bearing
//! Assumes constant velocity for both target and ownship
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass,
                        WsfPlatform,
                        InterceptLocation2D_4,
                        5,
                        "double",
                        "WsfTrack, WsfWaypoint, double, double, bool")
{
   // Argument 1: Target track <input>
   // Argument 2: Intercept waypoint <output>
   // Argument 3: Speed of ownship
   // Argument 4: Time delay until speed is reached

   // Ensure source platform position is current
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime);

   // Extract the function arguments
   WsfTrack*    trackPtr       = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfWaypoint* waypointPtr    = (WsfWaypoint*)aVarArgs[1].GetPointer()->GetAppObject();
   double       speedOfOwnship = aVarArgs[2].GetDouble();
   double       timeDelay      = aVarArgs[3].GetDouble();
   bool         debugEnabled   = aVarArgs[4].GetBool();

   double timeToIntercept;
   timeToIntercept =
      InterceptHelper(simTime + timeDelay, aObjectPtr, speedOfOwnship, trackPtr, waypointPtr, 2, debugEnabled);
   aReturnVal.SetDouble(timeToIntercept);
}

//! Returns time to intercept and intercept bearing and altitude
//! Assumes constant velocity for both target and ownship
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, InterceptLocation3D_1, 2, "double", "WsfTrack, WsfWaypoint")
{
   // Argument 1: Target track <input>
   // Argument 2: Intercept waypoint <output>

   // Ensure source platform position is current
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime);

   // Extract the function arguments
   WsfTrack*    trackPtr    = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfWaypoint* waypointPtr = (WsfWaypoint*)aVarArgs[1].GetPointer()->GetAppObject();

   double timeToIntercept;
   timeToIntercept = InterceptHelper(simTime, aObjectPtr, aObjectPtr->GetSpeed(), trackPtr, waypointPtr, 3, false);
   aReturnVal.SetDouble(timeToIntercept);
}

//! Returns time to intercept and intercept bearing and altitude
//! Assumes constant velocity for both target and ownship
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, InterceptLocation3D_2, 3, "double", "WsfTrack, WsfWaypoint, bool")
{
   // Argument 1: Target track <input>
   // Argument 2: Intercept waypoint <output>

   // Ensure source platform position is current
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime);

   // Extract the function arguments
   WsfTrack*    trackPtr     = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfWaypoint* waypointPtr  = (WsfWaypoint*)aVarArgs[1].GetPointer()->GetAppObject();
   bool         debugEnabled = aVarArgs[2].GetBool();

   double timeToIntercept;
   timeToIntercept = InterceptHelper(simTime, aObjectPtr, aObjectPtr->GetSpeed(), trackPtr, waypointPtr, 3, debugEnabled);
   aReturnVal.SetDouble(timeToIntercept);
}

//! Returns time to intercept and intercept bearing
//! Assumes constant velocity for both target and ownship
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, InterceptLocation3D_3, 4, "double", "WsfTrack, WsfWaypoint, double, double")
{
   // Argument 1: Target track <input>
   // Argument 2: Intercept waypoint <output>
   // Argument 3: Speed of ownship
   // Argument 4: Time delay until speed is reached

   // Ensure source platform position is current
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime);

   // Extract the function arguments
   WsfTrack*    trackPtr       = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfWaypoint* waypointPtr    = (WsfWaypoint*)aVarArgs[1].GetPointer()->GetAppObject();
   double       speedOfOwnship = aVarArgs[2].GetDouble();
   double       timeDelay      = aVarArgs[3].GetDouble();

   double timeToIntercept;
   timeToIntercept = InterceptHelper(simTime + timeDelay, aObjectPtr, speedOfOwnship, trackPtr, waypointPtr, 3, false);
   aReturnVal.SetDouble(timeToIntercept);
}

//! Returns time to intercept and intercept bearing
//! Assumes constant velocity for both target and ownship
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass,
                        WsfPlatform,
                        InterceptLocation3D_4,
                        5,
                        "double",
                        "WsfTrack, WsfWaypoint, double, double, bool")
{
   // Argument 1: Target track <input>
   // Argument 2: Intercept waypoint <output>
   // Argument 3: Speed of ownship
   // Argument 4: Time delay until speed is reached

   // Ensure source platform position is current
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime);

   // Extract the function arguments
   WsfTrack*    trackPtr       = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfWaypoint* waypointPtr    = (WsfWaypoint*)aVarArgs[1].GetPointer()->GetAppObject();
   double       speedOfOwnship = aVarArgs[2].GetDouble();
   double       timeDelay      = aVarArgs[3].GetDouble();
   bool         debugEnabled   = aVarArgs[4].GetBool();

   double timeToIntercept;
   timeToIntercept =
      InterceptHelper(simTime + timeDelay, aObjectPtr, speedOfOwnship, trackPtr, waypointPtr, 3, debugEnabled);
   aReturnVal.SetDouble(timeToIntercept);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, TravelTime, 1, "double", "WsfTrack")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime);

   // Extract the function arguments
   WsfTrack*     trackPtr        = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfRoadMover* moverPtr        = dynamic_cast<WsfRoadMover*>(aObjectPtr->GetMover());
   double        timeToIntercept = std::numeric_limits<double>::max();
   if (moverPtr != nullptr)
   {
      timeToIntercept = moverPtr->GetTravelTime(simTime, trackPtr);
   }
   aReturnVal.SetDouble(timeToIntercept);
}


// ----------------------------------------------------------------------------
//                       Location and Navigation methods
// ----------------------------------------------------------------------------

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetLocation_1, 3, "void", "double, double, double")
{
   aObjectPtr->SetLocationLLA(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble());
   ResetLocation(aObjectPtr, TIME_NOW);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetLocation_2, 1, "void", "WsfGeoPoint")
{
   WsfGeoPoint* geoPointPtr = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   double       locWCS[3];
   geoPointPtr->GetLocationWCS(locWCS);
   aObjectPtr->SetLocationWCS(locWCS);
   ResetLocation(aObjectPtr, TIME_NOW);
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetLocationMGRS_1, 1, "bool", "string")
{
   const std::string& locMGRS = aVarArgs[0].GetString();
   double             lat, lon;
   bool               success = UtEllipsoidalEarth::ConvertMGRSToLL(locMGRS, lat, lon);
   if (success)
   {
      double alt = 0.0;

      if (aObjectPtr->GetTerrain().IsEnabled())
      {
         float        elev;
         wsf::Terrain terrain(aObjectPtr->GetTerrain());
         terrain.GetElevInterp(lat, lon, elev);
         alt = elev;
      }
      aObjectPtr->SetLocationLLA(lat, lon, alt);
      ResetLocation(aObjectPtr, TIME_NOW);
   }
   aReturnVal.SetBool(success);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetLocationMGRS_2, 2, "bool", "string, double")
{
   const std::string& locMGRS = aVarArgs[0].GetString();
   double             lat, lon;
   bool               success = UtEllipsoidalEarth::ConvertMGRSToLL(locMGRS, lat, lon);
   if (success)
   {
      double alt = aVarArgs[1].GetDouble();
      aObjectPtr->SetLocationLLA(lat, lon, alt);
      ResetLocation(aObjectPtr, TIME_NOW);
   }
   aReturnVal.SetBool(success);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetLocationECI, 1, "void", "Vec3")
{
   UtVec3d* vec3Ptr = (UtVec3d*)aVarArgs[0].GetPointer()->GetAppObject();
   aObjectPtr->SetLocationECI(vec3Ptr->GetData());
}

//! Returns a GeoPoint with the location of the platform.
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Location, 0, "WsfGeoPoint", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double locWCS[3];
   aObjectPtr->GetLocationWCS(locWCS);
   WsfGeoPoint* pointPtr = new WsfGeoPoint(locWCS);
   pointPtr->SetComponentParent(aObjectPtr);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

//! Returns a WsfWaypoint with the LLA location of the platform
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, LocationAsWaypoint, 0, "WsfWaypoint", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double latitude;
   double longitude;
   double altitude;
   aObjectPtr->GetLocationLLA(latitude, longitude, altitude);

   WsfWaypoint* locPtr = new WsfWaypoint(latitude, longitude, altitude, 0);
   aReturnVal.SetPointer(new UtScriptRef(locPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Latitude, 0, "double", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double latitude;
   double longitude;
   double altitude;
   aObjectPtr->GetLocationLLA(latitude, longitude, altitude);
   aReturnVal.SetDouble(latitude);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Longitude, 0, "double", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double latitude;
   double longitude;
   double altitude;
   aObjectPtr->GetLocationLLA(latitude, longitude, altitude);
   aReturnVal.SetDouble(longitude);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Altitude, 0, "double", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double latitude;
   double longitude;
   double altitude;
   aObjectPtr->GetLocationLLA(latitude, longitude, altitude);
   aReturnVal.SetDouble(altitude);
}

//! Gets the platform height above the terrain (meters)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, HeightAboveTerrain, 0, "double", "")
{
   double heightAboveTerrain = aObjectPtr->GetHeightAboveTerrain();
   aReturnVal.SetDouble(heightAboveTerrain);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, PerceivedLocation, 0, "WsfGeoPoint", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double locWCS[3];
   aObjectPtr->GetLocationWCS(locWCS);
   double locErrorWCS[3];
   aObjectPtr->GetLocationErrorWCS(locErrorWCS);
   UtVec3d::Add(locWCS, locWCS, locErrorWCS);
   WsfGeoPoint* pointPtr = new WsfGeoPoint(locWCS);
   pointPtr->SetComponentParent(aObjectPtr);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, PerceivedLocationErrorNED, 0, "Vec3", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double errorWCS[3];
   double errorNED[3];
   aObjectPtr->GetLocationErrorWCS(errorWCS);
   aObjectPtr->ConvertWCSVectorToNED(errorNED, errorWCS);
   aReturnVal.SetPointer(UtScriptVec3::Create(errorNED));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, PerceivedLocationErrorWCS, 0, "Vec3", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double errorWCS[3];
   aObjectPtr->GetLocationErrorWCS(errorWCS);
   aReturnVal.SetPointer(UtScriptVec3::Create(errorWCS));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, FutureLocation, 1, "WsfGeoPoint", "double")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfGeoPoint* pointPtr   = nullptr;
   double       futureTime = aVarArgs[0].GetDouble();
   double       futureLocWCS[3];
   WsfMover*    moverPtr = aObjectPtr->GetMover();
   if (moverPtr == nullptr)
   {
      aObjectPtr->GetLocationWCS(futureLocWCS);
      pointPtr = new WsfGeoPoint(futureLocWCS);
      pointPtr->SetComponentParent(aObjectPtr);
   }
   else if (moverPtr->GetFutureLocationWCS(futureTime, futureLocWCS))
   {
      pointPtr = new WsfGeoPoint(futureLocWCS);
      pointPtr->SetComponentParent(aObjectPtr);
   }
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, LocationWCS, 0, "Vec3", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double locWCS[3];
   aObjectPtr->GetLocationWCS(locWCS);
   aReturnVal.SetPointer(UtScriptVec3::Create(locWCS));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, LocationMGRS, 0, "string", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double lat, lon, alt;
   aObjectPtr->GetLocationLLA(lat, lon, alt);
   std::string locMGRS;
   UtEllipsoidalEarth::ConvertLLToMGRS(lat, lon, locMGRS);
   aReturnVal.SetString(locMGRS);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, LocationECI, 0, "Vec3", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double locECI[3];
   aObjectPtr->GetLocationECI(locECI);
   aReturnVal.SetPointer(UtScriptVec3::Create(locECI));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, LocationTEME, 0, "Vec3", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double locTEME[3];
   aObjectPtr->GetLocationTEME(locTEME);
   aReturnVal.SetPointer(UtScriptVec3::Create(locTEME));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, LocationJ2000, 0, "Vec3", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double locECI[3];
   aObjectPtr->GetLocationJ2000(locECI);
   aReturnVal.SetPointer(UtScriptVec3::Create(locECI));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Speed, 0, "double", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   aReturnVal.SetDouble(aObjectPtr->GetSpeed());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GroundSpeed, 0, "double", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double velNED[3];
   aObjectPtr->GetVelocityNED(velNED);
   double groundSpeed = sqrt(velNED[0] * velNED[0] + velNED[1] * velNED[1]);
   aReturnVal.SetDouble(groundSpeed);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, MachNumber, 0, "double", "")
{
   UtAtmosphere atm(SCENARIO->GetAtmosphere());

   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   aReturnVal.SetDouble(aObjectPtr->GetSpeed() / atm.SonicVelocity(aObjectPtr->GetAltitude()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Heading, 0, "double", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double heading;
   double pitch;
   double roll;
   aObjectPtr->GetOrientationNED(heading, pitch, roll);
   aReturnVal.SetDouble(heading * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Pitch, 0, "double", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double heading;
   double pitch;
   double roll;
   aObjectPtr->GetOrientationNED(heading, pitch, roll);
   aReturnVal.SetDouble(pitch * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Roll, 0, "double", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double heading;
   double pitch;
   double roll;
   aObjectPtr->GetOrientationNED(heading, pitch, roll);
   aReturnVal.SetDouble(roll * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetHeading, 1, "void", "double")
{
   double heading, pitch, roll;
   aObjectPtr->GetOrientationNED(heading, pitch, roll);
   if (aObjectPtr->GetMover() == nullptr)
   {
      aObjectPtr->SetOrientationNED(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG, pitch, roll);
   }
   else
   {
      aObjectPtr->GetMover()->SetOrientationNED(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG, pitch, roll);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetSpeedAtWaypoint, 2, "void", "double, int") // NO_DOC | DEPRECATED
{
   double       speed       = aVarArgs[0].GetDouble();
   unsigned int waypointNum = (unsigned int)aVarArgs[1].GetInt();
   WsfMover*    moverPtr    = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      // m/s
      const WsfRoute* routePtr = moverPtr->GetRoute();
      WsfRoute        newRoute;
      if (routePtr != nullptr)
      {
         if ((unsigned int)waypointNum < (routePtr->GetSize()))
         {
            const WsfWaypoint& waypoint       = routePtr->GetWaypointAt(waypointNum);
            WsfWaypoint*       cpyWaypointPtr = waypoint.Clone();
            cpyWaypointPtr->SetSpeed(speed);
            if (routePtr->GetSize() > 1)
            {
               if (waypointNum > 0)
               {
                  // preserve the first part of the route before the waypoint with the new speed
                  routePtr->GetSubroute(0, (waypointNum - 1), newRoute);
               }
               // replace the waypoint that has the new speed
               newRoute.Append(*cpyWaypointPtr);
               if ((waypointNum + 1) < routePtr->GetSize())
               {
                  WsfRoute remainingRoute;
                  // get the remaining route that comes after the selected waypoint
                  routePtr->GetSubroute((waypointNum + 1), (routePtr->GetSize() - 1), remainingRoute);
                  // tack it back onto the new route.
                  for (unsigned int i = 0; i < remainingRoute.GetSize(); ++i)
                  {
                     WsfWaypoint& remainingWaypoints = remainingRoute.GetWaypointAt(i);
                     newRoute.Append(remainingWaypoints);
                  }
               }
            }
            else if (routePtr->GetSize() == 1)
            {
               if (waypointNum == 0)
               {
                  const WsfWaypoint& waypoint1        = routePtr->GetWaypointAt(0);
                  WsfWaypoint*       cpyWaypoint1_Ptr = waypoint1.Clone();
                  cpyWaypoint1_Ptr->SetSpeed(speed);
                  newRoute.Append(*cpyWaypoint1_Ptr);
               }
            }
            moverPtr->SetRoute(TIME_NOW, newRoute);
         }
      }
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetPitch, 1, "void", "double")
{
   // Get the current orientation
   double heading, pitch, roll;
   aObjectPtr->GetOrientationNED(heading, pitch, roll);

   // Get the new pitch value in radians
   double newPitch = aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG;

   // Set the orientation with the new pitch value
   if (aObjectPtr->GetMover() == nullptr)
   {
      aObjectPtr->SetOrientationNED(heading, newPitch, roll);
   }
   else
   {
      aObjectPtr->GetMover()->SetOrientationNED(heading, newPitch, roll);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetRoll, 1, "void", "double")
{
   double heading, pitch, roll;
   aObjectPtr->GetOrientationNED(heading, pitch, roll);
   if (aObjectPtr->GetMover() == nullptr)
   {
      aObjectPtr->SetOrientationNED(heading, pitch, aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   }
   else
   {
      aObjectPtr->SetOrientationNED(heading, pitch, aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, X, 0, "double", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double xyz[3];
   aObjectPtr->GetLocationWCS(xyz);
   aReturnVal.SetDouble(xyz[0]);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Y, 0, "double", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double xyz[3];
   aObjectPtr->GetLocationWCS(xyz);
   aReturnVal.SetDouble(xyz[1]);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Z, 0, "double", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double xyz[3];
   aObjectPtr->GetLocationWCS(xyz);
   aReturnVal.SetDouble(xyz[2]);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Vx, 0, "double", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double xyz[3];
   aObjectPtr->GetVelocityWCS(xyz);
   aReturnVal.SetDouble(xyz[0]);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Vy, 0, "double", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double xyz[3];
   aObjectPtr->GetVelocityWCS(xyz);
   aReturnVal.SetDouble(xyz[1]);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Vz, 0, "double", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double xyz[3];
   aObjectPtr->GetVelocityWCS(xyz);
   aReturnVal.SetDouble(xyz[2]);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, VelocityWCS, 0, "Vec3", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   UtVec3d xyz;
   aObjectPtr->GetVelocityWCS(xyz.GetData());
   aReturnVal.SetPointer(UtScriptVec3::Create(xyz));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, VelocityNED, 0, "Vec3", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   UtVec3d ned;
   aObjectPtr->GetVelocityNED(ned.GetData());
   aReturnVal.SetPointer(UtScriptVec3::Create(ned));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, VelocityECI, 0, "Vec3", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double velECI[3];
   aObjectPtr->GetVelocityECI(velECI);
   aReturnVal.SetPointer(UtScriptVec3::Create(velECI));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, VelocityTEME, 0, "Vec3", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double velTEME[3];
   aObjectPtr->GetVelocityTEME(velTEME);
   aReturnVal.SetPointer(UtScriptVec3::Create(velTEME));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, VelocityJ2000, 0, "Vec3", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime);
   double velJ2000[3];
   aObjectPtr->GetVelocityJ2000(velJ2000);
   aReturnVal.SetPointer(UtScriptVec3::Create(velJ2000));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetVelocityNED, 3, "void", "double, double, double")
{
   double vned[3] = {aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble()};
   aObjectPtr->SetVelocityNED(vned);
   ResetLocation(aObjectPtr, TIME_NOW);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetVelocityECS, 3, "void", "double, double, double")
{
   double vecs[3] = {aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble()};
   aObjectPtr->SetVelocityECS(vecs);
   ResetLocation(aObjectPtr, TIME_NOW);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, AccelerationWCS, 0, "Vec3", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   UtVec3d xyz;
   aObjectPtr->GetAccelerationWCS(xyz.GetData());
   aReturnVal.SetPointer(UtScriptVec3::Create(xyz));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, AccelerationNED, 0, "Vec3", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   UtVec3d ned;
   aObjectPtr->GetAccelerationNED(ned.GetData());
   aReturnVal.SetPointer(UtScriptVec3::Create(ned));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, AccelerationECI, 0, "Vec3", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   UtVec3d eci;
   aObjectPtr->GetAccelerationECI(eci.GetData());
   aReturnVal.SetPointer(UtScriptVec3::Create(eci));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, OrientationNED, 0, "Vec3", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   UtVec3d ned;
   aObjectPtr->GetOrientationNED(ned[0], ned[1], ned[2]);
   ned[0] *= UtMath::cDEG_PER_RAD;
   ned[1] *= UtMath::cDEG_PER_RAD;
   ned[2] *= UtMath::cDEG_PER_RAD;
   aReturnVal.SetPointer(UtScriptVec3::Create(ned));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, OrientationWCS, 0, "Vec3", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   UtVec3d wcs;
   aObjectPtr->GetOrientationWCS(wcs[0], wcs[1], wcs[2]);
   wcs[0] *= UtMath::cDEG_PER_RAD;
   wcs[1] *= UtMath::cDEG_PER_RAD;
   wcs[2] *= UtMath::cDEG_PER_RAD;
   aReturnVal.SetPointer(UtScriptVec3::Create(wcs));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, OrientationECI, 0, "Vec3", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   UtVec3d eci;
   aObjectPtr->GetOrientationECI(eci[0], eci[1], eci[2]);
   eci[0] *= UtMath::cDEG_PER_RAD;
   eci[1] *= UtMath::cDEG_PER_RAD;
   eci[2] *= UtMath::cDEG_PER_RAD;
   aReturnVal.SetPointer(UtScriptVec3::Create(eci));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetOrientationNED, 3, "void", "double, double, double")
{
   double yaw, pitch, roll;
   UtScriptDataUnpack(aVarArgs, yaw, pitch, roll);
   yaw *= UtMath::cRAD_PER_DEG;
   pitch *= UtMath::cRAD_PER_DEG;
   roll *= UtMath::cRAD_PER_DEG;
   aObjectPtr->SetOrientationNED(yaw, pitch, roll);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetOrientationWCS, 3, "void", "double, double, double")
{
   double psi, theta, phi;
   UtScriptDataUnpack(aVarArgs, psi, theta, phi);
   psi *= UtMath::cRAD_PER_DEG;
   theta *= UtMath::cRAD_PER_DEG;
   phi *= UtMath::cRAD_PER_DEG;
   aObjectPtr->SetOrientationWCS(psi, theta, phi);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetOrientationECI, 3, "void", "double, double, double")
{
   double psi, theta, phi;
   UtScriptDataUnpack(aVarArgs, psi, theta, phi);
   psi *= UtMath::cRAD_PER_DEG;
   theta *= UtMath::cRAD_PER_DEG;
   phi *= UtMath::cRAD_PER_DEG;
   aObjectPtr->SetOrientationECI(psi, theta, phi);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GeoPoint, 1, "WsfGeoPoint", "string")
{
   WsfGeoPoint* geoPointPtr = aObjectPtr->template GetComponent<WsfGeoPoint>(aVarArgs[0].GetString());
   aReturnVal.SetPointer(new UtScriptRef(geoPointPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GoToPoint_1, 1, "bool", "WsfGeoPoint")
{
   // Argument 1: geo point
   bool         ok          = false;
   WsfMover*    moverPtr    = aObjectPtr->GetMover();
   WsfGeoPoint* geoPointPtr = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();

   if (moverPtr != nullptr)
   {
      // Create a new route and append the point
      WsfRoute newRoute;
      newRoute.Append(
         WsfWaypoint(geoPointPtr->GetLat(), geoPointPtr->GetLon(), geoPointPtr->GetAlt(), aObjectPtr->GetSpeed()));
      ok = moverPtr->UpdateRoute(SIMULATION->GetSimTime(), newRoute);
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GoToPoint_2, 1, "bool", "string")
{
   // Argument 1: geo point name
   bool         ok(false);
   WsfMover*    moverPtr(aObjectPtr->GetMover());
   WsfGeoPoint* geoPointPtr(aObjectPtr->template GetComponent<WsfGeoPoint>(aVarArgs[0].GetString()));
   if ((moverPtr != nullptr) && (geoPointPtr != nullptr))
   {
      // Create a new route and append the point
      WsfRoute newRoute;
      newRoute.Append(
         WsfWaypoint(geoPointPtr->GetLat(), geoPointPtr->GetLon(), geoPointPtr->GetAlt(), aObjectPtr->GetSpeed()));
      ok = moverPtr->UpdateRoute(SIMULATION->GetSimTime(), newRoute);
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GoToPoint_3, 2, "bool", "string, string")
{
   // Argument 1: geo point name
   bool         ok(false);
   WsfMover*    moverPtr(aObjectPtr->GetMover());
   WsfGeoPoint* geoPointPtr(aObjectPtr->template GetComponent<WsfGeoPoint>(aVarArgs[0].GetString()));
   if ((moverPtr != nullptr) && (geoPointPtr != nullptr))
   {
      // Create a new route and waypoint point
      WsfRoute    newRoute;
      WsfWaypoint newWaypoint(geoPointPtr->GetLat(), geoPointPtr->GetLon(), geoPointPtr->GetAlt(), aObjectPtr->GetSpeed());

      // Get the callback (if it is defined)
      WsfCallback* callbackPtr = WsfCallbackTypes::Get(*SCENARIO).Clone(aVarArgs[1].GetString());
      if (callbackPtr != nullptr)
      {
         newWaypoint.SetCallback(callbackPtr);
      }
      else
      {
         newWaypoint.SetScriptId(WsfStringId(aVarArgs[1].GetString()));
      }

      // Add the waypoint
      newRoute.Append(newWaypoint);
      ok = moverPtr->UpdateRoute(SIMULATION->GetSimTime(), newRoute);
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GoToPoint_4, 2, "bool", "WsfGeoPoint, string")
{
   // Argument 1: geo point name

   bool      ok       = false;
   WsfMover* moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      // Get the geo point
      WsfGeoPoint* geoPointPtr = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();

      // Create a new route and waypoint point
      WsfRoute    newRoute;
      WsfWaypoint newWaypoint(geoPointPtr->GetLat(), geoPointPtr->GetLon(), geoPointPtr->GetAlt(), aObjectPtr->GetSpeed());

      // Get the call back
      WsfCallback* callbackPtr = WsfCallbackTypes::Get(*SCENARIO).Clone(aVarArgs[1].GetString());
      if (callbackPtr != nullptr)
      {
         newWaypoint.SetCallback(callbackPtr);
      }
      else
      {
         newWaypoint.SetScriptId(WsfStringId(aVarArgs[1].GetString()));
      }

      // Add the waypoint
      newRoute.Append(newWaypoint);
      ok = moverPtr->UpdateRoute(SIMULATION->GetSimTime(), newRoute);
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GoToLabel, 1, "bool", "string")
{
   // Argument 1: waypoint label

   bool      ok       = false;
   WsfMover* moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      ok = moverPtr->GoToLabel(SIMULATION->GetSimTime(), WsfStringId(aVarArgs[0].GetString()));
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GoToLocation_1, 3, "bool", "double, double, double")
{
   // Argument 1: latitude
   // Argument 2: longitude
   // Argument 3: altitude

   bool      ok       = false;
   WsfMover* moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      ok = moverPtr->GoToLocation(SIMULATION->GetSimTime(),
                                  aVarArgs[0].GetDouble(),
                                  aVarArgs[1].GetDouble(),
                                  aVarArgs[2].GetDouble());
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GoToLocation_2, 2, "bool", "double, double")
{
   // Argument 1: latitude
   // Argument 2: longitude
   // Altitude will be the current altitude (ignored for a ground/road mover)

   bool      ok       = false;
   WsfMover* moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      double lat, lon, alt;
      moverPtr->GetPlatform()->GetLocationLLA(lat, lon, alt);
      ok = moverPtr->GoToLocation(SIMULATION->GetSimTime(), aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), alt);
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GoToLocation_3, 1, "bool", "WsfWaypoint")
{
   bool         ok       = false;
   WsfWaypoint* pointPtr = (WsfWaypoint*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfMover*    moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      double lat, lon, alt;
      pointPtr->GetLocationLLA(lat, lon, alt);
      ok = moverPtr->GoToLocation(SIMULATION->GetSimTime(), lat, lon, alt);
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GoToLocation_4, 1, "bool", "WsfGeoPoint")
{
   bool         ok       = false;
   WsfGeoPoint* pointPtr = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfMover*    moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      double lat, lon, alt;
      pointPtr->GetLocationLLA(lat, lon, alt);
      ok = moverPtr->GoToLocation(SIMULATION->GetSimTime(), lat, lon, alt);
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GoToTrack, 1, "bool", "WsfTrack") // NO_DOC | DEPRECATED
{
   // Argument 1: Track
   // Altitude will be the current altitude (ignored for a ground/road mover)

   bool          ok       = false;
   WsfRoadMover* moverPtr = dynamic_cast<WsfRoadMover*>(aObjectPtr->GetMover());
   WsfTrack*     trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   if (moverPtr != nullptr)
   {
      double simTime = TIME_NOW;
      ok             = moverPtr->GoToTrack(simTime, trackPtr);
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GoToTrack_2, 2, "bool", "double, double") // NO_DOC | DEPRECATED
{
   // Argument 1: Track
   // Altitude will be the current altitude (ignored for a ground/road mover)

   bool      ok       = false;
   WsfMover* moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      double lat, lon, alt;
      moverPtr->GetPlatform()->GetLocationLLA(lat, lon, alt);
      double simTime = TIME_NOW;
      ok             = moverPtr->GoToLocation(simTime, lat, lon, 0.0);
   }
   aReturnVal.SetBool(ok);
}

// bool ok = GoToAltitude(altitude);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GoToAltitude_1, 1, "bool", "double")
{
   bool      ok       = false;
   WsfMover* moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      double altitude = aVarArgs[0].GetDouble();
      ok              = moverPtr->GoToAltitude(SIMULATION->GetSimTime(), altitude, 0.0, false);
   }
   aReturnVal.SetBool(ok);
}

// bool ok = GoToAltitude(altitude, rateOfChange);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GoToAltitude_2, 2, "bool", "double, double")
{
   bool      ok       = false;
   WsfMover* moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      double altitude   = aVarArgs[0].GetDouble();
      double changeRate = aVarArgs[1].GetDouble();
      ok                = moverPtr->GoToAltitude(SIMULATION->GetSimTime(), altitude, changeRate, false);
   }
   aReturnVal.SetBool(ok);
}

// bool ok = GoToAltitude(altitude, rateOfChange, keepRoute);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GoToAltitude_3, 3, "bool", "double, double, bool")
{
   bool      ok       = false;
   WsfMover* moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      double altitude   = aVarArgs[0].GetDouble();
      double changeRate = aVarArgs[1].GetDouble();
      bool   keepRoute  = aVarArgs[2].GetBool();
      ok                = moverPtr->GoToAltitude(SIMULATION->GetSimTime(), altitude, changeRate, keepRoute);
   }
   aReturnVal.SetBool(ok);
}

// bool ok = GoToSpeed(speed);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GoToSpeed_1, 1, "bool", "double")
{
   bool      ok       = false;
   WsfMover* moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      double speed = aVarArgs[0].GetDouble();
      ok           = moverPtr->GoToSpeed(SIMULATION->GetSimTime(), speed, 0.0, false);
   }
   aReturnVal.SetBool(ok);
}

// bool ok = GoToSpeed(speed, velRateOfChange);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GoToSpeed_2, 2, "bool", "double, double")
{
   bool      ok       = false;
   WsfMover* moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      double speed       = aVarArgs[0].GetDouble();
      double linearAccel = aVarArgs[1].GetDouble();
      ok                 = moverPtr->GoToSpeed(SIMULATION->GetSimTime(), speed, linearAccel, false);
   }
   aReturnVal.SetBool(ok);
}

// bool ok = GoToSpeed(speed, velRateOfChange, keepRoute);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GoToSpeed_3, 3, "bool", "double, double, bool")
{
   bool      ok       = false;
   WsfMover* moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      double speed       = aVarArgs[0].GetDouble();
      double linearAccel = aVarArgs[1].GetDouble();
      bool   keepRoute   = aVarArgs[2].GetBool();
      ok                 = moverPtr->GoToSpeed(SIMULATION->GetSimTime(), speed, linearAccel, keepRoute);
   }
   aReturnVal.SetBool(ok);
}

//! bool ok = GoToMachNumber(double aMachNumber);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GoToMachNumber_1, 1, "bool", "double")
{
   bool      ok       = false;
   WsfMover* moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      UtAtmosphere atm(SCENARIO->GetAtmosphere());
      double       simTime = SIMULATION->GetSimTime();
      aObjectPtr->Update(simTime); // Ensure source platform position is current
      double speed = aVarArgs[0].GetDouble() * atm.SonicVelocity(aObjectPtr->GetAltitude());
      ok           = moverPtr->GoToSpeed(simTime, speed, 0.0, false);
   }
   aReturnVal.SetBool(ok);
}

//! bool ok = GoToMachNumber(double aMachNumber, double aLinearAccel);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GoToMachNumber_2, 2, "bool", "double, double")
{
   bool      ok       = false;
   WsfMover* moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      UtAtmosphere atm(SCENARIO->GetAtmosphere());
      double       simTime = SIMULATION->GetSimTime();
      aObjectPtr->Update(simTime); // Ensure source platform position is current
      double speed       = aVarArgs[0].GetDouble() * atm.SonicVelocity(aObjectPtr->GetAltitude());
      double linearAccel = aVarArgs[1].GetDouble();
      ok                 = moverPtr->GoToSpeed(simTime, speed, linearAccel, false);
   }
   aReturnVal.SetBool(ok);
}

//! bool ok = TurnToHeading(heading)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, TurnToHeading_1, 1, "bool", "double")
{
   bool      ok       = false;
   double    simTime  = TIME_NOW;
   WsfMover* moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      double heading     = aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG; // degrees -> radians
      double radialAccel = 0.0;                                            // use mover default
      ok                 = moverPtr->TurnToHeading(simTime, heading, radialAccel, WsfPath::cTURN_DIR_SHORTEST);

      if (aObjectPtr->GetSpeed() <= 0.0)
      {
         // Mover is NOT moving - Set orientation
         aObjectPtr->Update(simTime); // Ensure source platform position is current
         double tempHeading;
         double pitch;
         double roll;
         aObjectPtr->GetOrientationNED(tempHeading, pitch, roll);
         aObjectPtr->SetOrientationNED(heading, pitch, roll);
         aObjectPtr->Update(simTime); // Update to new orientation
      }
   }
   else
   {
      // No mover - Set orientation
      aObjectPtr->Update(simTime); // Ensure source platform position is current
      double heading;
      double pitch;
      double roll;
      aObjectPtr->GetOrientationNED(heading, pitch, roll);
      aObjectPtr->SetOrientationNED((aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG), pitch, roll);
      aObjectPtr->Update(simTime); // Update to new orientation
   }
   aReturnVal.SetBool(ok);
}

//! bool ok = TurnToHeading(heading, radialAccel)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, TurnToHeading_2, 2, "bool", "double, double")
{
   bool      ok       = false;
   double    simTime  = TIME_NOW;
   WsfMover* moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      double heading     = aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG; // degrees -> radians
      double radialAccel = aVarArgs[1].GetDouble();                        // meters/second^2
      ok                 = moverPtr->TurnToHeading(simTime, heading, radialAccel, WsfPath::cTURN_DIR_SHORTEST);

      if (aObjectPtr->GetSpeed() <= 0.0)
      {
         // Mover is NOT moving - Set orientation
         aObjectPtr->Update(simTime); // Ensure source platform position is current
         double tempHeading;
         double pitch;
         double roll;
         aObjectPtr->GetOrientationNED(tempHeading, pitch, roll);
         aObjectPtr->SetOrientationNED(heading, pitch, roll);
         aObjectPtr->Update(simTime); // Update to new orientation
      }
   }
   else
   {
      // No mover - Set orientation
      aObjectPtr->Update(simTime); // Ensure source platform position is current
      double heading;
      double pitch;
      double roll;
      aObjectPtr->GetOrientationNED(heading, pitch, roll);
      aObjectPtr->SetOrientationNED((aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG), pitch, roll);
      aObjectPtr->Update(simTime); // Update to new orientation
   }
   aReturnVal.SetBool(ok);
}

//! bool ok = TurnToRelativeHeading(heading)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, TurnToRelativeHeading_1, 1, "bool", "double")
{
   bool      ok       = false;
   WsfMover* moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      double heading     = aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG; // degrees -> radians
      double radialAccel = 0.0;                                            // use mover default
      ok = moverPtr->TurnToRelativeHeading(SIMULATION->GetSimTime(), heading, radialAccel, WsfPath::cTURN_DIR_SHORTEST);
   }
   aReturnVal.SetBool(ok);
}

//! bool ok = TurnToRelativeHeading(heading, radialAccel)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, TurnToRelativeHeading_2, 2, "bool", "double, double")
{
   bool      ok       = false;
   WsfMover* moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      double heading     = aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG; // degrees -> radians
      double radialAccel = aVarArgs[1].GetDouble();                        // meters/second^2
      ok = moverPtr->TurnToRelativeHeading(SIMULATION->GetSimTime(), heading, radialAccel, WsfPath::cTURN_DIR_SHORTEST);
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GetRoute, 0, "WsfRoute", "")
{
   const WsfRoute* routePtr = nullptr;
   WsfMover*       moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      routePtr = moverPtr->GetRoute();
   }
   UtScriptRef* routeRefPtr = (routePtr != nullptr) ? routePtr->ConstScriptRef() :
                                                      new UtScriptRef(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE);
   aReturnVal.SetPointer(routeRefPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, RoutePointIndex, 0, "int", "")
{
   int       routePointIndex = 0;
   WsfMover* moverPtr        = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      routePointIndex = moverPtr->RoutePointIndex();
   }
   aReturnVal.SetInt(routePointIndex);
}

//! WsfRoute route = CreateRoute(string aRouteType);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, CreateRoute, 1, "WsfRoute", "string")
{
   WsfRoute* routePtr = WsfRouteTypes::Get(*SCENARIO).Clone(aVarArgs[0].GetString());
   aReturnVal.SetPointer(routePtr->ScriptRefManage());
}

//! bool followingRoute = FollowRoute(WsfRoute aRoute);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, FollowRoute_1, 1, "bool", "WsfRoute")
{
   WsfMover* routeMvrPtr = GetRouteMover(aObjectPtr);
   bool      success     = FollowRoute(TIME_NOW, routeMvrPtr, GetRouteObject(aVarArgs[0]));
   aReturnVal.SetBool(success);
}

//! bool followingRoute = FollowRoute(WsfRoute aRoute);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, FollowRoute_2, 2, "bool", "WsfRoute, string")
{
   WsfMover* routeMvrPtr = GetRouteMover(aObjectPtr);
   bool      success     = FollowRoute(TIME_NOW, routeMvrPtr, GetRouteObject(aVarArgs[0]));
   if (success)
   {
      std::string routeAction = aVarArgs[1].GetString();
      GoToWaypoint(TIME_NOW, routeMvrPtr, routeAction);
   }
   aReturnVal.SetBool(success);
}

//! bool followingRoute = FollowRoute(string aRouteName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, FollowRoute_3, 1, "bool", "string")
{
   WsfMover* routeMvrPtr = GetRouteMover(aObjectPtr);
   bool      success     = FollowRoute(TIME_NOW, routeMvrPtr, GetNamedRoute(aVarArgs[0], routeMvrPtr));
   aReturnVal.SetBool(success);
}

//! bool followingRoute = FollowRoute(string aRouteName, string aRouteAction);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, FollowRoute_4, 2, "bool", "string, string")
{
   WsfMover* routeMvrPtr = GetRouteMover(aObjectPtr);
   bool      success     = FollowRoute(TIME_NOW, routeMvrPtr, GetNamedRoute(aVarArgs[0], routeMvrPtr));
   if (success)
   {
      std::string routeAction = aVarArgs[1].GetString();
      GoToWaypoint(TIME_NOW, routeMvrPtr, routeAction);
   }
   aReturnVal.SetBool(success);
}

//! bool followingRoute = FollowRoute(WsfRoute aRoute, int aIndex);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, FollowRoute_5, 2, "bool", "WsfRoute, int")
{
   WsfMover* routeMvrPtr = GetRouteMover(aObjectPtr);
   bool      success     = FollowRoute(TIME_NOW, routeMvrPtr, GetRouteObject(aVarArgs[0]));
   if (success)
   {
      int routeIndex = aVarArgs[1].GetInt();
      success        = GoToWaypoint(TIME_NOW, routeMvrPtr, routeIndex);
   }
   aReturnVal.SetBool(success);
}

//! bool followingRoute = FollowRoute(string aRouteName, int aIndex);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, FollowRoute_6, 2, "bool", "string, int")
{
   WsfMover* routeMvrPtr = GetRouteMover(aObjectPtr);
   bool      success     = FollowRoute(TIME_NOW, routeMvrPtr, GetNamedRoute(aVarArgs[0], routeMvrPtr));
   if (success)
   {
      int routeIndex = aVarArgs[1].GetInt();
      success        = GoToWaypoint(TIME_NOW, routeMvrPtr, routeIndex);
   }
   aReturnVal.SetBool(success);
}

//! bool followingRoute = SetRoute(WsfRoute aRoute);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetRoute_1, 1, "bool", "WsfRoute")
{
   bool success = SetRoute(TIME_NOW, *aObjectPtr, *GetRouteObject(aVarArgs[0]));
   aReturnVal.SetBool(success);
}

//! bool followingRoute = SetRoute(string aRouteName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetRoute_2, 1, "bool", "string")
{
   bool            success(false);
   WsfMover*       routeMvrPtr = GetRouteMover(aObjectPtr);
   const WsfRoute* routePtr    = GetNamedRoute(aVarArgs[0], routeMvrPtr);
   if (routePtr != nullptr)
   {
      success = SetRoute(TIME_NOW, *aObjectPtr, *routePtr);
   }
   aReturnVal.SetBool(success);
}

//! double distance = DistanceAlongRoute();
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, DistanceAlongRoute, 0, "double", "") // NO_DOC | DEPRECATED
{
   double    distanceAlongRoute = -1.0;
   WsfMover* routeMvrPtr        = GetRouteMover(aObjectPtr);
   if (routeMvrPtr != nullptr)
   {
      WsfRouteMover* moverPtr = dynamic_cast<WsfRouteMover*>(routeMvrPtr);
      if (moverPtr != nullptr)
      {
         int             segment  = moverPtr->GetTargetIndex() - 1;
         const WsfRoute* routePtr = moverPtr->GetRoute();
         if (routePtr != nullptr)
         {
            if (segment >= 0 && (segment + 1) < (int)routePtr->GetSize())
            {
               double lat, lon, alt, distance;
               aObjectPtr->GetLocationLLA(lat, lon, alt);
               const WsfWaypoint& a = (*routePtr)[segment];
               const WsfWaypoint& b = (*routePtr)[segment + 1];
               if (a.GetPointType() & b.GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE)
               {
                  double heading, heading2, distance2;
                  UtSphericalEarth::GreatCircleHeadingAndDistance(a.GetLat(), a.GetLon(), lat, lon, heading, distance);
                  UtSphericalEarth::GreatCircleHeadingAndDistance(lat, lon, b.GetLat(), b.GetLon(), heading2, distance2);
                  double distanceFromPoint = cos((heading - heading2) * UtMath::cRAD_PER_DEG) * distance;
                  distanceAlongRoute       = std::max(0.0, distanceFromPoint) + (double)a.GetDistanceAlongRoute();
               }
            }
            if (distanceAlongRoute == -1.0)
            {
               if (routePtr->GetSize() > 0)
               {
                  int wptIndex       = UtMath::Limit(segment, 0, int(routePtr->GetSize()) - 1);
                  distanceAlongRoute = routePtr->GetWaypointAt(wptIndex).GetDistanceAlongRoute();
               }
            }
         }
      }
   }
   aReturnVal.SetDouble(distanceAlongRoute);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, ReturnToRoute, 0, "bool", "")
{
   bool      ok       = false;
   WsfMover* moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      ok = moverPtr->ReturnToRoute(SIMULATION->GetSimTime());
   }
   aReturnVal.SetBool(ok);
}

//!
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, FindAndSetPath, 2, "bool", "WsfGeoPoint, WsfGeoPoint") // NO_DOC | DEPRECATED
{
   WsfGeoPoint* startPointPtr = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfGeoPoint* endPointPtr   = (WsfGeoPoint*)aVarArgs[1].GetPointer()->GetAppObject();
   bool         success       = false;

   WsfMover* moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      success = moverPtr->FindAndSetPath(SIMULATION->GetSimTime(), startPointPtr, endPointPtr);
   }

   aReturnVal.SetBool(success);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, FlyRates, 2, "bool", "Vec3, Vec3")
{
   bool     ok = false;
   UtVec3d *angularRates, *axisAccel;
   UtScriptDataUnpack(aVarArgs, angularRates, axisAccel);
   WsfMover* moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      ok = moverPtr->FlyRates(*angularRates, *axisAccel);
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, FlyHeadingSpeedAltitude, 5, "bool", "double, double, double, double, double")
{
   bool   ok = false;
   double heading, speed, altitude, maxGs, maxClimbRate;
   UtScriptDataUnpack(aVarArgs, heading, speed, altitude, maxGs, maxClimbRate);
   WsfMover* moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      ok = moverPtr->FlyHeadingSpeedAltitude(heading, speed, altitude, maxGs, maxClimbRate);
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, PullGsInPlaneWithSpeed, 3, "bool", "Vec3, double, double")
{
   bool     ok = false;
   UtVec3d* plane;
   double   Gs, speed;
   UtScriptDataUnpack(aVarArgs, plane, Gs, speed);
   WsfMover* moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      ok = moverPtr->TurnInPlaneWithSpeed(*plane, Gs, speed);
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, PullGsInPlaneWithThrottle, 3, "bool", "Vec3, double, double")
{
   bool     ok = false;
   UtVec3d* plane;
   double   Gs, throttle;
   UtScriptDataUnpack(aVarArgs, plane, Gs, throttle);
   WsfMover* moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      ok = moverPtr->TurnInPlaneWithThrottle(*plane, Gs, throttle);
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, FlyVectorWithSpeed, 3, "bool", "Vec3, double, double")
{
   bool     ok = false;
   UtVec3d* vector;
   double   Gs, speed;
   UtScriptDataUnpack(aVarArgs, vector, Gs, speed);
   WsfMover* moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      ok = moverPtr->FlyVectorWithSpeed(*vector, Gs, speed);
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, FlyVectorWithThrottle, 3, "bool", "Vec3, double, double")
{
   bool     ok = false;
   UtVec3d* vector;
   double   Gs, throttle;
   UtScriptDataUnpack(aVarArgs, vector, Gs, throttle);
   WsfMover* moverPtr = aObjectPtr->GetMover();
   if (moverPtr != nullptr)
   {
      ok = moverPtr->FlyVectorWithThrottle(*vector, Gs, throttle);
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, PathFinder, 0, "WsfPathFinder", "")
{
   WsfRouteMover* moverPtr      = dynamic_cast<WsfRouteMover*>(aObjectPtr->GetMover());
   WsfPathFinder* pathFinderPtr = nullptr;
   if (moverPtr != nullptr)
   {
      pathFinderPtr = (WsfPathFinder*)moverPtr->GetRouteComputer();
   }

   aReturnVal.SetPointer(new UtScriptRef(pathFinderPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Pause, 1, "void", "double")
{
   WsfRouteMover* moverPtr = dynamic_cast<WsfRouteMover*>(aObjectPtr->GetMover());
   if (moverPtr != nullptr)
   {
      double simTime = TIME_NOW;
      moverPtr->Pause(simTime, aVarArgs[0].GetDouble());
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Unpause, 0, "void", "")
{
   WsfRouteMover* moverPtr = dynamic_cast<WsfRouteMover*>(aObjectPtr->GetMover());
   if (moverPtr != nullptr)
   {
      double simTime = TIME_NOW;
      moverPtr->Unpause(simTime, true);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetRandomLocation, 0, "void", "") // NO_DOC | DEPRECATED
{
   std::array<double, 3> lla;
   aObjectPtr->GetLocationLLA(lla[0], lla[1], lla[2]);

   auto           moverPtr  = dynamic_cast<WsfRouteMover*>(aObjectPtr->GetMover());
   WsfPathFinder* finderPtr = nullptr;
   if (moverPtr != nullptr)
   {
      finderPtr = dynamic_cast<WsfPathFinder*>(moverPtr->GetRouteComputer());
   }

   if (finderPtr == nullptr)
   {
      return;
   }

   bool badSpot = true;
   while (badSpot)
   {
      int moveToNode = static_cast<int>(
         SIMULATION->GetScriptRandom().Uniform<long>(0, finderPtr->GetXSize() * finderPtr->GetYSize() - 1));
      WsfGeoPoint           newEndPt = finderPtr->GetNodeLocation(moveToNode);
      std::array<double, 3> endWayPtLLA;
      newEndPt.GetLocationLLA(endWayPtLLA[0], endWayPtLLA[1], endWayPtLLA[2]);
      const WsfPFNode* aNode = finderPtr->GetClosestNode(WsfGeoPoint(endWayPtLLA[0], endWayPtLLA[1], endWayPtLLA[2]));

      if (aNode == nullptr)
      {
         badSpot = true;
      }
      else if (aNode->mWeight == std::numeric_limits<double>::max())
      {
         // if this node is not to be considered, reset the endWayPt and try again...
         badSpot = true;
      }
      else
      {
         badSpot = false;
         std::array<double, 3> newLoc;
         newEndPt.GetLocationLLA(newLoc[0], newLoc[1], newLoc[2]);
         aObjectPtr->SetLocationLLA(newLoc[0], newLoc[1], newLoc[2]);

         WsfRoute aRoute;
         aRoute.Append(WsfWaypoint(newLoc[0], newLoc[1], newLoc[2], aObjectPtr->GetSpeed()));
         aObjectPtr->GetMover()->SetRoute(SIMULATION->GetSimTime(), aRoute);
         return;
      }
   }
}

// allow the end of path behavior to be changed in script
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SetEndOfPath, 1, "void", "string")
{
   std::string argString = aVarArgs[0].GetString();

   WsfPath::EndPath newEOP = WsfPath::cEP_MOVER_DEFAULT;
   if (argString == "default")
   {
      newEOP = WsfPath::cEP_MOVER_DEFAULT;
   }
   else if (argString == "extrapolate")
   {
      newEOP = WsfPath::cEP_EXTRAPOLATE;
   }
   else if (argString == "stop")
   {
      newEOP = WsfPath::cEP_STOP;
   }
   else if (argString == "remove")
   {
      newEOP = WsfPath::cEP_REMOVE;
   }

   WsfRouteMover* moverPtr = dynamic_cast<WsfRouteMover*>(aObjectPtr->GetMover());
   if (moverPtr != nullptr)
   {
      moverPtr->SetEndOfPath(newEOP);
   }
   else
   {
      WsfRouteMover* newMoverPtr = dynamic_cast<WsfRouteMover*>(aObjectPtr->GetMover());
      if (newMoverPtr != nullptr)
      {
         newMoverPtr->SetEndOfPath(newEOP);
      }
   }
}
//! Switches the current platform's mover from a WsfFollower to a WsfRouteMover.
//! This causes the platform to disengage the following logic.
//! Return true if successful.
//! Note: This method only works for platform's that use the WsfHybrid mover.
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, BreakFormation, 0, "bool", "") // NO_DOC | DEPRECATED
{
   bool success = false;
   // Make sure we have a hybrid mover.
   WsfHybridMover* hybridMoverPtr = dynamic_cast<WsfHybridMover*>(aObjectPtr->GetMover());
   if (hybridMoverPtr != nullptr)
   {
      // If the current mover type is a follower mover, change to a waypoint mover.
      if (hybridMoverPtr->GetType() == WsfHybridMover::cFOLLOWER)
      {
         { // RAII block
            auto out = ut::log::info() << "Changing from follower to waypoint mover.";
            out.AddNote() << "Mover: " << aObjectPtr->GetName();
         }
         WsfRoute route;
         // Add our current location to the route, because when we swap movers
         // the first waypoint will be used as our current location.
         double lla[3];
         aObjectPtr->GetLocationLLA(lla[0], lla[1], lla[2]);
         route.Append(WsfWaypoint(lla[0], lla[1], lla[2], aObjectPtr->GetSpeed()));

         // Change to a waypoint mover.
         hybridMoverPtr->SetType(WsfHybridMover::cWAYPOINT);
         success = hybridMoverPtr->GetCurrent()->UpdateRoute(SIMULATION->GetSimTime(), route);
      }
   }
   aReturnVal.SetBool(success);
}

//! Switches the current platform's mover from a WsfRouteMover to a WsfFollower.
//! This causes the platform to continue the following logic.
//! Returns true if successful.
//! Note: This method only works for platform's that use the WsfHybrid mover.
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, JoinFormation, 0, "bool", "") // NO_DOC | DEPRECATED
{
   bool success = false;
   // Make sure we have a hybrid mover.
   WsfHybridMover* hybridMoverPtr = dynamic_cast<WsfHybridMover*>(aObjectPtr->GetMover());
   if (hybridMoverPtr != nullptr)
   {
      // If the current mover type is a waypoint mover, change to a follower mover.
      if (hybridMoverPtr->GetType() == WsfHybridMover::cWAYPOINT)
      {
         { // RAII block
            auto out = ut::log::info() << "Changing from waypoint mover to follower.";
            out.AddNote() << "Mover: " << aObjectPtr->GetName();
         }

         // Change to a follower mover.
         hybridMoverPtr->SetType(WsfHybridMover::cFOLLOWER);
         success = true;
      }
   }
   aReturnVal.SetBool(success);
}

//! Gets the mTargetSpeed of the platform's mover if it's a waypoint mover.
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GetTargetSpeed, 0, "double", "")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current

   double            speed    = 0;
   WsfWaypointMover* moverPtr = dynamic_cast<WsfWaypointMover*>(aObjectPtr->GetMover());
   if (moverPtr != nullptr)
   {
      if (moverPtr->GetTarget().mTargetType & WsfPathTarget::cSPEED)
      {
         speed = moverPtr->GetTarget().mSpeed;
      }
   }

   aReturnVal.SetDouble(speed);
}

// ----------------------------------------------------------------------------
//                         Relative geometry methods
// ----------------------------------------------------------------------------

// double azimuth = ApparentAspectOf(WsfGeoPoint aPoint, double aEarthRadiusScale).
// Returns the aspect (azimuth, elevation) to the point in the entity coordinate
// system for a specified earth radius scale factor.
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, ApparentAspectOf, 2, "Array<double>", "WsfGeoPoint, double")
{
   WsfGeoPoint* pointPtr         = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   double       earthRadiusScale = aVarArgs[1].GetDouble();
   earthRadiusScale = (earthRadiusScale > 0.0) ? earthRadiusScale : (4.0 / 3.0); // Use default if requested

   // Compute the azimuth and elevation of the viewpoint with respect to this platform.
   double pointLocWCS[3];
   pointPtr->GetLocationWCS(pointLocWCS);
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->Update(simTime);
   double thisToPointAz, thisToPointEl;
   WsfEM_Util::ComputeViewerAspect(aObjectPtr, pointLocWCS, earthRadiusScale, thisToPointAz, thisToPointEl);

   auto sdvPtr = ut::make_unique<std::vector<UtScriptData>>();
   sdvPtr->emplace_back(thisToPointAz * UtMath::cDEG_PER_RAD);
   sdvPtr->emplace_back(thisToPointEl * UtMath::cDEG_PER_RAD);
   aReturnVal.SetPointer(new UtScriptRef(sdvPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

// double azimuth = RelativeAzimuthOf(WsfGeoPoint).
// Returns the azimuth of the point in the entity coordinate system.
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, RelativeAzimuthOf, 1, "double", "WsfGeoPoint")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double       otherLocWCS[3];
   WsfGeoPoint* pointPtr = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   pointPtr->GetLocationWCS(otherLocWCS);
   double relLocWCS[3];
   aObjectPtr->GetRelativeLocationWCS(otherLocWCS, relLocWCS);
   double relLocECS[3];
   aObjectPtr->ConvertWCSVectorToECS(relLocECS, relLocWCS);

   // From UtEntity::ComputeAzimuthAndElevation (azimuth part only)
   double x       = relLocECS[0];
   double y       = relLocECS[1];
   double azimuth = atan2(y, x);
   aReturnVal.SetDouble(azimuth * UtMath::cDEG_PER_RAD);
}

// double elevation = RelativeElevationOf(WsfGeoPoint).
// CReturn the elevation of a point in the entity coordinate system.
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, RelativeElevationOf, 1, "double", "WsfGeoPoint")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double       otherLocWCS[3];
   WsfGeoPoint* pointPtr = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   pointPtr->GetLocationWCS(otherLocWCS);
   double relLocWCS[3];
   aObjectPtr->GetRelativeLocationWCS(otherLocWCS, relLocWCS);
   double relLocECS[3];
   aObjectPtr->ConvertWCSVectorToECS(relLocECS, relLocWCS);

   // From UtEntity::ComputeAzimuthAndElevation (elevation part only)
   double x         = relLocECS[0];
   double y         = relLocECS[1];
   double z         = relLocECS[2];
   double elevation = 0.0;
   double xy        = sqrt((x * x) + (y * y));
   if (xy != 0.0)
   {
      elevation = -atan2(z, xy);
   }
   else
   {
      elevation = UtMath::cPI_OVER_2;
      if (z > 0.0)
      {
         elevation = -UtMath::cPI_OVER_2;
      }
   }
   aReturnVal.SetDouble(elevation * UtMath::cDEG_PER_RAD);
}

//! double bearing = TrueBearingTo(WsfTrack aTrack);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, TrueBearingTo_1, 1, "double", "WsfTrack")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double    otherLocWCS[3];
   trackPtr->GetExtrapolatedLocationWCS(simTime, otherLocWCS);
   double bearing = aObjectPtr->TrueBearing(otherLocWCS);
   aReturnVal.SetDouble(bearing * UtMath::cDEG_PER_RAD);
}

//! double bearing = TrueBearingTo(WsfPlatform aPlatform);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, TrueBearingTo_2, 1, "double", "WsfPlatform")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfPlatform* otherPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   otherPtr->Update(simTime); // Ensure the other platforms location is current
   double otherLocWCS[3];
   otherPtr->GetLocationWCS(otherLocWCS);
   double bearing = aObjectPtr->TrueBearing(otherLocWCS);
   aReturnVal.SetDouble(bearing * UtMath::cDEG_PER_RAD);
}

//! double bearing = TrueBearingTo(double aLat, double aLon, double aAlt);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, TrueBearingTo_3, 3, "double", "double, double, double")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double otherLocWCS[3];
   UtEntity::ConvertLLAToWCS(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble(), otherLocWCS);
   double bearing = aObjectPtr->TrueBearing(otherLocWCS);
   aReturnVal.SetDouble(bearing * UtMath::cDEG_PER_RAD);
}

//! double bearing = TrueBearingTo(WsfGeoPoint aPoint);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, TrueBearingTo_4, 1, "double", "WsfGeoPoint")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfGeoPoint* pointPtr = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   double       otherLocWCS[3];
   pointPtr->GetLocationWCS(otherLocWCS);
   double bearing = aObjectPtr->TrueBearing(otherLocWCS);
   aReturnVal.SetDouble(bearing * UtMath::cDEG_PER_RAD);
}

//! double bearing = RelativeBearingTo(WsfTrack aTrack);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, RelativeBearingTo_1, 1, "double", "WsfTrack")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double    otherLocWCS[3];
   trackPtr->GetExtrapolatedLocationWCS(simTime, otherLocWCS);
   double bearing = aObjectPtr->RelativeBearing(otherLocWCS);
   aReturnVal.SetDouble(bearing * UtMath::cDEG_PER_RAD);
}

//! double bearing = RelativeBearingTo(WsfPlatform aPlatform);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, RelativeBearingTo_2, 1, "double", "WsfPlatform")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfPlatform* otherPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   otherPtr->Update(simTime); // Ensure the other platforms location is current
   double otherLocWCS[3];
   otherPtr->GetLocationWCS(otherLocWCS);
   double bearing = aObjectPtr->RelativeBearing(otherLocWCS);
   aReturnVal.SetDouble(bearing * UtMath::cDEG_PER_RAD);
}

//! double bearing = RelativeBearingTo(double aLat, double aLon, double aAlt);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, RelativeBearingTo_3, 3, "double", "double, double, double")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double otherLocWCS[3];
   UtEntity::ConvertLLAToWCS(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble(), otherLocWCS);
   double bearing = aObjectPtr->RelativeBearing(otherLocWCS);
   aReturnVal.SetDouble(bearing * UtMath::cDEG_PER_RAD);
}

//! double bearing = RelativeBearingTo(WsfGeoPoint aPoint);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, RelativeBearingTo_4, 1, "double", "WsfGeoPoint")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfGeoPoint* pointPtr = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   double       otherLocWCS[3];
   pointPtr->GetLocationWCS(otherLocWCS);
   double bearing = aObjectPtr->RelativeBearing(otherLocWCS);
   aReturnVal.SetDouble(bearing * UtMath::cDEG_PER_RAD);
}

//! double distance = SlantRangeTo(WsfTrack aTrack)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SlantRangeTo_1, 1, "double", "WsfTrack")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double    otherLocWCS[3];
   trackPtr->GetExtrapolatedLocationWCS(simTime, otherLocWCS);
   double distance = SlantRange(aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = SlantRangeTo(WsfTrack aTrack, bool aUseSpherical)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SlantRangeTo_1s, 2, "double", "WsfTrack, bool")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfTrack* trackPtr     = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   bool      useSpherical = aVarArgs[1].GetBool();
   double    otherLocWCS[3];
   trackPtr->GetExtrapolatedLocationWCS(simTime, otherLocWCS);
   double distance = SlantRange(aObjectPtr, otherLocWCS, useSpherical);
   aReturnVal.SetDouble(distance);
}

//! double distance = SlantRangeTo(WsfPlatform aPlatform);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SlantRangeTo_2, 1, "double", "WsfPlatform")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   platformPtr->Update(simTime); // Ensure the other platforms location is current
   double otherLocWCS[3];
   platformPtr->GetLocationWCS(otherLocWCS);
   double distance = SlantRange(aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = SlantRangeTo(WsfPlatform aPlatform, bool aUseSpherical);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SlantRangeTo_2s, 2, "double", "WsfPlatform, bool")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfPlatform* platformPtr  = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   bool         useSpherical = aVarArgs[1].GetBool();
   platformPtr->Update(simTime); // Ensure the other platforms location is current
   double otherLocWCS[3];
   platformPtr->GetLocationWCS(otherLocWCS);
   double distance = SlantRange(aObjectPtr, otherLocWCS, useSpherical);
   aReturnVal.SetDouble(distance);
}


//! double distance = SlantRangeTo(double aLat, double aLon, double aAlt);
//! lat/lon in degrees, altitude in meters
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SlantRangeTo_3, 3, "double", "double, double, double")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double otherLocWCS[3];
   UtEntity::ConvertLLAToWCS(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble(), otherLocWCS);
   double distance = SlantRange(aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = SlantRangeTo(double aLat, double aLon, double aAlt, bool aUseSpherical);
//! lat/lon in degrees, altitude in meters
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SlantRangeTo_3s, 4, "double", "double, double, double, bool")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double otherLocWCS[3];
   UtEntity::ConvertLLAToWCS(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble(), otherLocWCS);
   bool   useSpherical = aVarArgs[3].GetBool();
   double distance     = SlantRange(aObjectPtr, otherLocWCS, useSpherical);
   aReturnVal.SetDouble(distance);
}

//! double distance = SlantRangeTo(WsfGeoPoint aPoint)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SlantRangeTo_4, 1, "double", "WsfGeoPoint")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfGeoPoint* pointPtr = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   double       otherLocWCS[3];
   pointPtr->GetLocationWCS(otherLocWCS);
   double distance = SlantRange(aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = SlantRangeTo(WsfGeoPoint aPoint, bool aUseSpherical)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SlantRangeTo_4s, 2, "double", "WsfGeoPoint, bool")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfGeoPoint* pointPtr     = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   bool         useSpherical = aVarArgs[1].GetBool();
   double       otherLocWCS[3];
   pointPtr->GetLocationWCS(otherLocWCS);
   double distance = SlantRange(aObjectPtr, otherLocWCS, useSpherical);
   aReturnVal.SetDouble(distance);
}

//! double distance = SlantRangeTo(WsfWaypoint aPoint)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SlantRangeTo_5, 1, "double", "WsfWaypoint")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfWaypoint* pointPtr = (WsfWaypoint*)aVarArgs[0].GetPointer()->GetAppObject();
   double       lat, lon, alt;
   double       otherLocWCS[3];
   pointPtr->GetLocationLLA(lat, lon, alt);
   UtEntity::ConvertLLAToWCS(lat, lon, alt, otherLocWCS);
   double distance = SlantRange(aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = SlantRangeTo(WsfWaypoint aPoint, bool aUseSpherical)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SlantRangeTo_5s, 2, "double", "WsfWaypoint, bool")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfWaypoint* pointPtr     = static_cast<WsfWaypoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   bool         useSpherical = aVarArgs[1].GetBool();
   double       lat, lon, alt;
   double       otherLocWCS[3];
   pointPtr->GetLocationLLA(lat, lon, alt);
   UtEntity::ConvertLLAToWCS(lat, lon, alt, otherLocWCS);
   double distance = SlantRange(aObjectPtr, otherLocWCS, useSpherical);
   aReturnVal.SetDouble(distance);
}

//! double distance = GroundRangeTo(WsfTrack aTrack)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GroundRangeTo_1, 1, "double", "WsfTrack")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double    otherLocWCS[3];
   trackPtr->GetExtrapolatedLocationWCS(simTime, otherLocWCS);
   double distance = GroundRange(aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = GroundRangeTo(WsfPlatform aPlatform);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GroundRangeTo_2, 1, "double", "WsfPlatform")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   platformPtr->Update(simTime); // Ensure the other platforms location is current
   double otherLocWCS[3];
   platformPtr->GetLocationWCS(otherLocWCS);
   double distance = GroundRange(aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = GroundRangeTo(double aLat, double aLon);
//! lat/lon in degrees, altitude in meters
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GroundRangeTo_3, 3, "double", "double, double, double")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double otherLocWCS[3];
   UtEntity::ConvertLLAToWCS(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble(), otherLocWCS);
   double distance = GroundRange(aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = GroundRangeTo(WsfGeoPoint aPoint)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GroundRangeTo_4, 1, "double", "WsfGeoPoint")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfGeoPoint* pointPtr = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   double       otherLocWCS[3];
   pointPtr->GetLocationWCS(otherLocWCS);
   double distance = GroundRange(aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = DownRangeTo(WsfTrack aTrack)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, DownRangeTo_1, 1, "double", "WsfTrack")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double    otherLocWCS[3];
   trackPtr->GetExtrapolatedLocationWCS(simTime, otherLocWCS);
   double distance = DownRange(aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = DownRangeTo(WsfPlatform aPlatform);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, DownRangeTo_2, 1, "double", "WsfPlatform")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   platformPtr->Update(simTime); // Ensure the other platforms location is current
   double otherLocWCS[3];
   platformPtr->GetLocationWCS(otherLocWCS);
   double distance = DownRange(aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = DownRangeTo(double aLat, double aLon);
//! lat/lon in degrees, altitude in meters
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, DownRangeTo_3, 3, "double", "double, double, double")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double otherLocWCS[3];
   UtEntity::ConvertLLAToWCS(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble(), otherLocWCS);
   double distance = DownRange(aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = DownRangeTo(WsfGeoPoint aPoint)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, DownRangeTo_4, 1, "double", "WsfGeoPoint")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfGeoPoint* pointPtr = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   double       otherLocWCS[3];
   pointPtr->GetLocationWCS(otherLocWCS);
   double distance = DownRange(aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = CrossRangeTo(WsfTrack aTrack)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, CrossRangeTo_1, 1, "double", "WsfTrack")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double    otherLocWCS[3];
   trackPtr->GetExtrapolatedLocationWCS(simTime, otherLocWCS);
   double distance = CrossRange(aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = CrossRangeTo(WsfPlatform aPlatform);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, CrossRangeTo_2, 1, "double", "WsfPlatform")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   platformPtr->Update(simTime); // Ensure the other platforms location is current
   double otherLocWCS[3];
   platformPtr->GetLocationWCS(otherLocWCS);
   double distance = CrossRange(aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = CrossRangeTo(double aLat, double aLon);
//! lat/lon in degrees, altitude in meters
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, CrossRangeTo_3, 3, "double", "double, double, double")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double otherLocWCS[3];
   UtEntity::ConvertLLAToWCS(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble(), otherLocWCS);
   double distance = CrossRange(aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! double distance = CrossRangeTo(WsfGeoPoint aPoint)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, CrossRangeTo_4, 1, "double", "WsfGeoPoint")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfGeoPoint* pointPtr = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   double       otherLocWCS[3];
   pointPtr->GetLocationWCS(otherLocWCS);
   double distance = CrossRange(aObjectPtr, otherLocWCS);
   aReturnVal.SetDouble(distance);
}

//! WsfGeoPoint point = ClosestApproachOf(WsfTrack aTrack)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, ClosestApproachOf_1, 1, "WsfGeoPoint", "WsfTrack")
{
   double pcaLocWCS[3];
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double    otherLocWCS[3];
   double    otherVelWCS[3] = {0.0, 0.0, 0.0};
   trackPtr->GetExtrapolatedLocationWCS(simTime, otherLocWCS);
   if (trackPtr->VelocityValid())
   {
      trackPtr->GetVelocityWCS(otherVelWCS);
   }
   ClosestApproach(aObjectPtr, otherLocWCS, otherVelWCS, pcaLocWCS);
   WsfGeoPoint* gpPtr = new WsfGeoPoint(pcaLocWCS);
   gpPtr->SetComponentParent(aObjectPtr);
   aReturnVal.SetPointer(new UtScriptRef(gpPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

//! WsfGeoPoint point = ClosestApproachOf(WsfPlatform aPlatform);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, ClosestApproachOf_2, 1, "WsfGeoPoint", "WsfPlatform")
{
   double pcaLocWCS[3];
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   platformPtr->Update(simTime); // Ensure the other platforms location is current
   double otherLocWCS[3];
   double otherVelWCS[3];
   platformPtr->GetLocationWCS(otherLocWCS);
   platformPtr->GetVelocityWCS(otherVelWCS);
   ClosestApproach(aObjectPtr, otherLocWCS, otherVelWCS, pcaLocWCS);
   WsfGeoPoint* gpPtr = new WsfGeoPoint(pcaLocWCS);
   gpPtr->SetComponentParent(aObjectPtr);
   aReturnVal.SetPointer(new UtScriptRef(gpPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

//! double headingDifference = HeadingDifferenceOf(WsfTrack aTrack);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, HeadingDifferenceOf_1, 1, "double", "WsfTrack")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double    headingDifference = 0.0;
   WsfTrack* trackPtr          = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   // Determine the heading vector of from the velocity in the track. If the velocity
   // is not valid or if it is zero then we can't determine the heading and a zero
   // crossing angle will be reported.

   if (trackPtr->VelocityValid())
   {
      double otherLocWCS[3];
      double otherVelWCS[3];
      double otherVelNED[3];
      trackPtr->GetExtrapolatedLocationWCS(simTime, otherLocWCS);
      trackPtr->GetVelocityWCS(otherVelWCS);
      UtEntity other;
      other.SetLocationWCS(otherLocWCS);
      other.SetVelocityWCS(otherVelWCS);
      other.GetVelocityNED(otherVelNED);
      if ((otherVelNED[0] != 0.0) && (otherVelNED[1] != 0.0))
      {
         double otherHeading = atan2(otherVelNED[1], otherVelNED[0]);
         headingDifference   = HeadingDifference(aObjectPtr, otherHeading);
      }
   }
   aReturnVal.SetDouble(headingDifference * UtMath::cDEG_PER_RAD);
}

//! double headingDifference = HeadingDifferenceOf(WsfPlatform aPlatform)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, HeadingDifferenceOf_2, 1, "double", "WsfPlatform")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   platformPtr->Update(simTime); // Ensure the other platforms location is current
   double otherHeading;
   double otherPitch;
   double otherRoll;
   platformPtr->GetOrientationNED(otherHeading, otherPitch, otherRoll);
   double headingDifference = HeadingDifference(aObjectPtr, otherHeading);
   aReturnVal.SetDouble(headingDifference * UtMath::cDEG_PER_RAD);
}

//! double closingSpeed = ClosingSpeedOf(WsfTrack aTrack);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, ClosingSpeedOf_1, 1, "double", "WsfTrack")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double    otherLocWCS[3];
   trackPtr->GetExtrapolatedLocationWCS(simTime, otherLocWCS);
   double otherVelWCS[3];
   trackPtr->GetVelocityWCS(otherVelWCS);
   double closingSpeed = ClosingSpeed(aObjectPtr, aObjectPtr->GetSpeed(), otherLocWCS, otherVelWCS);
   aReturnVal.SetDouble(closingSpeed);
}

//! double closingSpeed = ClosingSpeedOf(WsfPlatform aPlatform)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, ClosingSpeedOf_2, 1, "double", "WsfPlatform")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   platformPtr->Update(simTime); // Ensure the other platforms location is current
   double otherLocWCS[3];
   platformPtr->GetLocationWCS(otherLocWCS);
   double otherVelWCS[3];
   platformPtr->GetVelocityWCS(otherVelWCS);
   double closingSpeed = ClosingSpeed(aObjectPtr, aObjectPtr->GetSpeed(), otherLocWCS, otherVelWCS);
   aReturnVal.SetDouble(closingSpeed);
}

//! double closingSpeed = ClosingSpeedOf(WsfTrack aTrack, double aMySpeed);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, ClosingSpeedOf_3, 2, "double", "WsfTrack, double")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double    otherLocWCS[3];
   trackPtr->GetExtrapolatedLocationWCS(simTime, otherLocWCS);
   double otherVelWCS[3];
   trackPtr->GetVelocityWCS(otherVelWCS);
   double closingSpeed = ClosingSpeed(aObjectPtr, aVarArgs[1].GetDouble(), otherLocWCS, otherVelWCS);
   aReturnVal.SetDouble(closingSpeed);
}

//! double closingSpeed = ClosingSpeedOf(WsfPlatform aPlatform, double aMySpeed)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, ClosingSpeedOf_4, 2, "double", "WsfPlatform, double")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   platformPtr->Update(simTime); // Ensure the other platforms location is current
   double otherLocWCS[3];
   platformPtr->GetLocationWCS(otherLocWCS);
   double otherVelWCS[3];
   platformPtr->GetVelocityWCS(otherVelWCS);
   double closingSpeed = ClosingSpeed(aObjectPtr, aVarArgs[1].GetDouble(), otherLocWCS, otherVelWCS);
   aReturnVal.SetDouble(closingSpeed);
}

//! bool withinFieldOfView = WithinFieldOfView(WsfTrack aTrack, string aSensorName)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, WithinFieldOfView_1, 2, "bool", "WsfTrack, string")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   bool      withinFieldOfView(false);
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   // First check to see the second argument is an sensor
   WsfSensor* sensorPtr = aObjectPtr->template GetComponent<WsfSensor>(aVarArgs[1].GetString());
   if (sensorPtr != nullptr)
   {
      withinFieldOfView = sensorPtr->WithinFieldOfView(simTime, trackPtr);
   }
   aReturnVal.SetBool(withinFieldOfView);
}

//! bool withinFieldOfView = WithinFieldOfView(WsfPlatform aPlatform, string aSensorName)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, WithinFieldOfView_2, 2, "bool", "WsfPlatform, string")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   bool         withinFieldOfView(false);
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   // First check to see the second argument is an sensor
   WsfSensor* sensorPtr = aObjectPtr->template GetComponent<WsfSensor>(WsfStringId(aVarArgs[1].GetString()));
   if (sensorPtr != nullptr)
   {
      withinFieldOfView = sensorPtr->WithinFieldOfView(simTime, platformPtr);
   }
   aReturnVal.SetBool(withinFieldOfView);
}

//! bool maskedByTerrain = MaskedByTerrain(WsfGeoPoint aPoint, double aVerticalOffset, double aRadiusScale)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, MaskedByTerrain, 3, "bool", "WsfGeoPoint, double, double")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure position is current

   double lat1;
   double lon1;
   double alt1;
   aObjectPtr->GetLocationLLA(lat1, lon1, alt1);
   double verticalOffset = aVarArgs[1].GetDouble();
   alt1 += verticalOffset;

   double       lat2;
   double       lon2;
   double       alt2;
   WsfGeoPoint* pointPtr = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   pointPtr->GetLocationLLA(lat2, lon2, alt2);

   double radiusScale = aVarArgs[2].GetDouble();
   bool   masked      = true;
   // WsfTerrain::MaskedByTerrain only does its check if terrain is present and enabled. If not,
   // we simply fall back to horizon masking...
   if (aObjectPtr->GetTerrain().IsEnabled() || aObjectPtr->GetTerrain().IsBathymetryEnabled())
   {
      masked = aObjectPtr->GetTerrain().MaskedByTerrain(lat1, lon1, alt1, lat2, lon2, alt2, 0.0, radiusScale);
   }
   else
   {
      masked = WsfEM_Util::MaskedByHorizon(lat1, lon1, alt1, 0.0, lat2, lon2, alt2, 0.0, radiusScale);
   }
   aReturnVal.SetBool(masked);
}

//! REL-TGT-HDG
//!  double RelativeHeadingOf(WsfTrack aTrack)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, RelativeHeadingOf, 1, "double", "WsfTrack")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfTrack* trackPtr        = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double    relativeHeading = 0.0;
   double    trackLocWCS[3];
   if (trackPtr->GetExtrapolatedLocationWCS(simTime, trackLocWCS))
   {
      UtEntity trackEntity;
      trackEntity.SetLocationWCS(trackLocWCS);
      if (trackPtr->VelocityValid())
      {
         double trackVelWCS[3];
         trackPtr->GetVelocityWCS(trackVelWCS);
         if (UtVec3d::MagnitudeSquared(trackVelWCS) > 0.0)
         {
            trackEntity.SetVelocityWCS(trackVelWCS);
            double trackVelNED[3];
            trackEntity.GetVelocityNED(trackVelNED);
            double heading = atan2(trackVelNED[1], trackVelNED[0]);
            trackEntity.SetOrientationNED(heading, 0.0, 0.0);
         }
      }

      // TODO - HACK - REL-TGT-HDG and non-moving tracks.
      // EVALU8 uses the heading from the perceived track to compute REL-TGT-HDG, which is just the truth heading
      // as defined by OBSTEL. The logic above recovers a heading from the track velocity, but if that velocity
      // is zero or non-existent then we don't have a heading. Therefore we go back to the truth platform and
      // get the truth heading. This will still fail if the target no longer exists.

      if (trackEntity.GetSpeed() == 0.0)
      {
         // Cheat and find the truth heading!
         WsfPlatform* platformPtr = SIMULATION->GetPlatformByIndex(trackPtr->GetTargetIndex());
         if (platformPtr != nullptr)
         {
            double heading;
            double pitch;
            double roll;
            platformPtr->GetOrientationNED(heading, pitch, roll);
            trackEntity.SetOrientationNED(heading, pitch, roll);
         }
      }

      double thisLocWCS[3];
      aObjectPtr->GetLocationWCS(thisLocWCS);
      relativeHeading = trackEntity.RelativeBearing(thisLocWCS) * UtMath::cDEG_PER_RAD;

      // Other models sometimes ignores the sign of the heading (doesn't care if it is to the right or left).
      relativeHeading = fabs(relativeHeading);
   }
   aReturnVal.SetDouble(relativeHeading);
}

//! Like 2D-REL-TGT-OFFSET
//! double RelativeOffsetOf(WsfTrack aTrack)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, RelativeOffsetOf, 1, "double", "WsfTrack")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double    relativeOffset = cHUGE_OFFSET_OR_ALTITUDE;
   WsfTrack* trackPtr       = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double    trackLocWCS[3];
   if (trackPtr->GetExtrapolatedLocationWCS(simTime, trackLocWCS))
   {
      // EVALU8 returns 0 if the track does not have a velocity.
      relativeOffset = 0.0;
      if (trackPtr->VelocityValid())
      {
         double trackVelWCS[3];
         trackPtr->GetVelocityWCS(trackVelWCS);
         if (UtVec3d::MagnitudeSquared(trackVelWCS) > 0.0)
         {
            UtEntity trackEntity;
            trackEntity.SetLocationWCS(trackLocWCS);
            trackEntity.SetVelocityWCS(trackVelWCS);
            double trackVelNED[3];
            trackEntity.GetVelocityNED(trackVelNED);
            double heading = atan2(trackVelNED[1], trackVelNED[0]);
            trackEntity.SetOrientationNED(heading, 0.0, 0.0);

            double groundRange = GroundRange(aObjectPtr, trackLocWCS);
            if (groundRange > 0.0)
            {
               double thisLocWCS[3];
               aObjectPtr->GetLocationWCS(thisLocWCS);
               double relativeBearing = trackEntity.RelativeBearing(thisLocWCS);
               // Do this just like EVALU8 to avoid quadrant issues.
               double cosRB   = cos(relativeBearing);
               double sinRB   = sqrt(1.0 - cosRB * cosRB);
               relativeOffset = groundRange * sinRB;
            }
         }
      }
   }
   aReturnVal.SetDouble(relativeOffset);
}

//! Like REL-TGT-ALT (target - platform)
//! double RelativeAltitudeOf(WsfTrack aTrack)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, RelativeAltitudeOf, 1, "double", "WsfTrack")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double    relativeAltitude = cHUGE_OFFSET_OR_ALTITUDE;
   WsfTrack* trackPtr         = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double    targetLocWCS[3];
   if (trackPtr->GetExtrapolatedLocationWCS(simTime, targetLocWCS))
   {
      double targetLat;
      double targetLon;
      double targetAlt;
      UtEntity::ConvertWCSToLLA(targetLocWCS, targetLat, targetLon, targetAlt);
      relativeAltitude = targetAlt - aObjectPtr->GetAltitude();
   }
   aReturnVal.SetDouble(relativeAltitude);
}

//! double GreatCircleHeading(WsfTrack aTrack)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GreatCircleHeading, 1, "double", "WsfTrack")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double    greatCircleHeading = -1.0;
   WsfTrack* trackPtr           = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double    targetLocWCS[3]    = {0.0};
   if (trackPtr->GetExtrapolatedLocationWCS(simTime, targetLocWCS))
   {
      double targetLat(0.0);
      double targetLon(0.0);
      double targetAlt(0.0);
      UtEntity::ConvertWCSToLLA(targetLocWCS, targetLat, targetLon, targetAlt);

      double currentLat(0.0);
      double currentLon(0.0);
      double currentAlt(0.0);
      aObjectPtr->GetLocationLLA(currentLat, currentLon, currentAlt);

      double distance(0.0);
      UtSphericalEarth::GreatCircleHeadingAndDistance(currentLat, currentLon, targetLat, targetLon, greatCircleHeading, distance);
   }
   aReturnVal.SetDouble(greatCircleHeading);
}

//! double GreatCircleDistance(WsfTrack aTrack)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GreatCircleDistance, 1, "double", "WsfTrack")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   double    greatCircleDistance = -1.0;
   WsfTrack* trackPtr            = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double    targetLocWCS[3]     = {0.0};
   if (trackPtr->GetExtrapolatedLocationWCS(simTime, targetLocWCS))
   {
      double targetLat(0.0);
      double targetLon(0.0);
      double targetAlt(0.0);
      UtEntity::ConvertWCSToLLA(targetLocWCS, targetLat, targetLon, targetAlt);

      double currentLat(0.0);
      double currentLon(0.0);
      double currentAlt(0.0);
      aObjectPtr->GetLocationLLA(currentLat, currentLon, currentAlt);

      double heading(0.0);
      UtSphericalEarth::GreatCircleHeadingAndDistance(currentLat, currentLon, targetLat, targetLon, heading, greatCircleDistance);
   }
   aReturnVal.SetDouble(greatCircleDistance);
}

//! double GreatCircleDestinationTime(WsfTrack aTrack)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GreatCircleDestinationTime_1, 1, "double", "WsfTrack")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   WsfTrack* trackPtr       = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   double    platformVel[3] = {0.0};
   aObjectPtr->GetVelocityWCS(platformVel);
   double greatCircleDestinationTime = WsfUtil::TimeToDestination(*aObjectPtr, *trackPtr, platformVel);
   aReturnVal.SetDouble(greatCircleDestinationTime);
}

//! double GreatCircleDestinationTime(double sourceLat, double sourceLon, double destLat, double destLon)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GreatCircleDestinationTime_2, 4, "double", "double, double, double, double")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current

   double sourceLat = aVarArgs[0].GetDouble();
   double sourceLon = aVarArgs[1].GetDouble();
   double destLat   = aVarArgs[2].GetDouble();
   double destLon   = aVarArgs[3].GetDouble();

   // Get great circle heading and distance
   double heading;
   double distance;
   UtSphericalEarth::GreatCircleHeadingAndDistance(sourceLat, sourceLon, destLat, destLon, heading, distance);

   // Get platform velocity
   double platformVel[3] = {0.0};
   aObjectPtr->GetVelocityWCS(platformVel);

   aReturnVal.SetDouble(distance / UtVec3d::Magnitude(platformVel));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, ApparentTime, 0, "double", "")
{
   aObjectPtr->Update(TIME_NOW);
   double wcsLoc[3];
   aObjectPtr->GetLocationWCS(wcsLoc);
   WsfGeoPoint point{wcsLoc};
#undef GetCurrentTime
   auto epoch = SIMULATION->GetDateTime().GetCurrentTime(TIME_NOW);
   aReturnVal.SetDouble(point.GetApparentTime(epoch));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass,
                        WsfPlatform,
                        ConvertWCSToECS,
                        4,
                        "void",
                        "double, double, double, Array<double>") // NO_DOC | DEPRECATED
{
   double               wcs[3] = {aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble(), aVarArgs[2].GetDouble()};
   std::vector<double>* wcsPtr = (std::vector<double>*)aVarArgs[3].GetPointer()->GetAppObject();
   double               ecs[3];
   aObjectPtr->ConvertWCSToECS(wcs, ecs);
   wcsPtr->clear();
   wcsPtr->push_back(ecs[0]);
   wcsPtr->push_back(ecs[1]);
   wcsPtr->push_back(ecs[2]);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, ConvertWCSToECS_2, 1, "Vec3", "Vec3")
{
   UtVec3d* wcsPtr;
   UtVec3d  ecs;
   UtScriptDataUnpack(aVarArgs, wcsPtr);
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   aObjectPtr->ConvertWCSToECS(wcsPtr->GetData(), ecs.GetData());
   aReturnVal = UtScriptData(UtScriptVec3::Create(ecs));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, ConvertECSToWCS, 1, "Vec3", "Vec3")
{
   UtVec3d  wcs;
   UtVec3d* ecsPtr;
   UtScriptDataUnpack(aVarArgs, ecsPtr);
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   aObjectPtr->ConvertECSToWCS(ecsPtr->GetData(), wcs.GetData());
   aReturnVal = UtScriptData(UtScriptVec3::Create(wcs));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, ConvertWCSToECI, 1, "Vec3", "Vec3")
{
   UtVec3d* wcsPtr;
   UtVec3d  eci;
   UtScriptDataUnpack(aVarArgs, wcsPtr);
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   aObjectPtr->ConvertWCSToECI(wcsPtr->GetData(), eci.GetData());
   aReturnVal = UtScriptData(UtScriptVec3::Create(eci));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, ConvertECIToWCS, 1, "Vec3", "Vec3")
{
   UtVec3d  wcs;
   UtVec3d* eciPtr;
   UtScriptDataUnpack(aVarArgs, eciPtr);
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   aObjectPtr->ConvertECIToWCS(eciPtr->GetData(), wcs.GetData());
   aReturnVal = UtScriptData(UtScriptVec3::Create(wcs));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass,
                        WsfPlatform,
                        ExtrapolateGreatCirclePosition,
                        5,
                        "WsfGeoPoint",
                        "double, double, double, double, double")
{
   double lat        = aVarArgs[0].GetDouble();
   double lon        = aVarArgs[1].GetDouble();
   double alt        = aVarArgs[2].GetDouble();
   double heading    = aVarArgs[3].GetDouble();
   double distanceNM = aVarArgs[4].GetDouble() * UtMath::cM_PER_NM;
   double targetLat;
   double targetLon;

   UtSphericalEarth::ExtrapolateGreatCirclePosition(lat, lon, heading, distanceNM, targetLat, targetLon);
   double locWCS[3];
   UtEntity::ConvertLLAToWCS(targetLat, targetLon, alt, locWCS);
   double locWCS2[3];
   UtEllipsoidalEarth::ConvertLLAToECEF(targetLat, targetLon, alt, locWCS2);
   WsfGeoPoint* pointPtr = new WsfGeoPoint(locWCS);
   pointPtr->SetComponentParent(aObjectPtr);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}
// ----------------------------------------------------------------------------
//                         Zone containment methods
// ----------------------------------------------------------------------------

//! Is this platform within the specified zone on this platform?
//! bool withinZone = WithinZone(string aZoneName)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, WithinZone, 1, "bool", "string")
{
   bool     withinZone = false;
   WsfZone* zonePtr    = aObjectPtr->template GetComponent<WsfZone>(aVarArgs[0].GetString());
   if (zonePtr != nullptr)
   {
      double simTime = TIME_NOW;
      aObjectPtr->Update(simTime); // Ensure source platform position is current
      double thisLocWCS[3];
      double heading;
      double pitch;
      double roll;
      aObjectPtr->GetLocationWCS(thisLocWCS);
      aObjectPtr->GetOrientationNED(heading, pitch, roll);
      withinZone = zonePtr->PointIsInside(SIMULATION, thisLocWCS, thisLocWCS, heading);
   }
   aReturnVal.SetBool(withinZone);
}

// =================================================================================================
//! Is this platform within the specified zone of a specified platform
// bool withinZone = WithinZoneOf(WsfPlatform aPlatform, string aZoneName)
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, WithinZoneOf, 2, "bool", "WsfPlatform, string")
{
   bool         withinZone  = false;
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfZone*     zonePtr     = platformPtr->template GetComponent<WsfZone>(aVarArgs[1].GetString());
   if (zonePtr != nullptr)
   {
      double simTime = TIME_NOW;

      // Get the location of the target point.
      double targetLocWCS[3];
      aObjectPtr->Update(simTime); // Ensure source platform position is current
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

// =================================================================================================
//! get the zone object
// WsfZone zone = Zone(zone-name);
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Zone, 1, "WsfZone", "string")
{
   // Get the zone name
   std::string zoneName = aVarArgs[0].GetString();

   // Set the default return value;
   WsfZone* zoneDefinitionPtr = nullptr;

   // Get the zone by name from the platform
   WsfZone* platformZonePtr = aObjectPtr->template GetComponent<WsfZone>(zoneName);
   if (platformZonePtr != nullptr)
   {
      // This zone could be a reference to a global; check for this condiiton
      if (!platformZonePtr->GetGlobalName().IsNull())
      {
         // Check to see if the zone is initialized
         if (!platformZonePtr->IsInitialized())
         {
            platformZonePtr->Initialize(*SCENARIO);
         }
         // Global zone reference
         zoneDefinitionPtr = (dynamic_cast<WsfZoneReference*>(platformZonePtr))->GetReferencedZone();
      }
      else
      {
         // Not a global zone reference - it is either a zone definition or a zone set
         zoneDefinitionPtr = dynamic_cast<WsfZoneDefinition*>(platformZonePtr);
         if (zoneDefinitionPtr == nullptr)
         {
            zoneDefinitionPtr = dynamic_cast<WsfZoneSet*>(platformZonePtr);
         }
      }
   }

   // Return the zone
   if (zoneDefinitionPtr != nullptr)
   {
      aReturnVal.SetPointer(new UtScriptRef(zoneDefinitionPtr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
   }
   else
   {
      auto out = ut::log::warning() << "Could not find zone.";
      out.AddNote() << "Zone: " << aVarArgs[0].GetString();
      aReturnVal.SetPointer(new UtScriptRef(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

// =================================================================================================
//! get the zone set that this platform is storing
// Array<string> zoneNames = aPlatform.ZoneNames()
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, ZoneNames, 0, "Array<string>", "")
{
   std::vector<UtScriptData>* tempListPtr = new std::vector<UtScriptData>();

   for (WsfComponentList::RoleIterator<WsfZone> iter(*aObjectPtr); !iter.AtEnd(); ++iter)
   {
      WsfZone* zonePtr = *iter;
      if (zonePtr != nullptr)
      {
         tempListPtr->push_back(UtScriptData(zonePtr->GetName()));
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(tempListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
//! get the zone set that this platform is storing by object
// Array<WsfZone> zones = aPlatform.Zones()
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Zones, 0, "Array<WsfZone>", "")
{
   std::vector<UtScriptData>* tempListPtr = new std::vector<UtScriptData>();

   UtScriptClass* zoneClassPtr = aContext.GetTypes()->GetClass("WsfZone");

   for (WsfComponentList::RoleIterator<WsfZone> iter(*aObjectPtr); !iter.AtEnd(); ++iter)
   {
      WsfZoneDefinition* zoneDefPtr = dynamic_cast<WsfZoneDefinition*>(*iter);
      if (zoneDefPtr != nullptr)
      {
         tempListPtr->emplace_back(new UtScriptRef(zoneDefPtr->Clone(), zoneClassPtr, UtScriptRef::cMANAGE));
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(tempListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

//! DeletePlatform();
//! Schedules deletion of the requesting platform.
UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptPlatformClass, WsfPlatform, DeletePlatform, 0, "void", "")
{
   if (aObjectPtr != nullptr)
   {
      double simTime = TIME_NOW;
      SIMULATION->DeletePlatform(simTime, aObjectPtr);
   }
}

//! WsfTrack track = MakeTrack();
//! Creates a track that represents the current state of the platform instance.
//! The owner of the track is the issuer of the request.
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, MakeTrack, 0, "WsfTrack", "")
{
   double    simTime  = TIME_NOW;
   WsfTrack* trackPtr = new WsfTrack;
   aObjectPtr->Update(simTime); // Ensure target platform position is current

   // The source (originator) of the track is the platform that issued this call
   WsfPlatform* sourcePtr = WsfScriptContext::GetPLATFORM(aContext);
   if (sourcePtr != nullptr)
   {
      sourcePtr->Update(simTime); // Ensure source platform position is current
   }
   else
   {
      sourcePtr = aObjectPtr; // Just in case...
   }
   WsfUtil::MakeTrack(simTime, *trackPtr, *aObjectPtr, *sourcePtr);

   // Mark the track as 'processed' and 'fusible'. This allows it participate in to be fused with
   // other tracks if passed to a track manager.
   trackPtr->SetTrackType(WsfTrack::cPROCESSED);
   trackPtr->SetFusible(true);

   aReturnVal.SetPointer(new UtScriptRef(trackPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

//! bool isCategoryMemberOf = CategoryMemberOf(string aCategory);
//! Returns true if the platform is a member of the given category.
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, CategoryMemberOf, 1, "bool", "string")
{
   WsfStringId categoryNameId = WsfStringId(aVarArgs[0].GetString());
   aReturnVal.SetBool(aObjectPtr->IsCategoryMember(categoryNameId));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, AddCategory, 1, "void", "string")
{
   aObjectPtr->AddCategory(aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, SwapMover_1, 1, "WsfMover", "string")
{
   std::string moverTypeStr     = aVarArgs[0].GetString();
   WsfMover*   originalMoverPtr = nullptr;
   WsfMover*   newMoverPtr      = WsfMoverTypes::Get(*SCENARIO).Clone(moverTypeStr);
   if (newMoverPtr)
   {
      aObjectPtr->SwapMover(SIMULATION->GetSimTime(), newMoverPtr, originalMoverPtr);
   }
   aReturnVal.SetPointer(new UtScriptRef(originalMoverPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// SwapMover(WsfMover aMover)  aMover may be null.
UT_DEFINE_SCRIPT_METHOD_NO_ARG_CHECK(WsfScriptPlatformClass, WsfPlatform, SwapMover_2, 1, "WsfMover", "WsfMover")
{
   WsfMover* originalMoverPtr = nullptr;
   auto      newMoverPtr      = aVarArgs[0].GetPointer()->GetAppObject<WsfMover>();
   if (newMoverPtr)
   {
      newMoverPtr = newMoverPtr->Clone();
   }
   aObjectPtr->SwapMover(SIMULATION->GetSimTime(), newMoverPtr, originalMoverPtr);
   aReturnVal.SetPointer(new UtScriptRef(originalMoverPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
//! bool isGroupMemberOf = GroupMemberOf(string aGroupName);
//! Returns true if the platform is a member of the given group.
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GroupMemberOf_2, 1, "bool", "string")
{
   WsfStringId groupNameId = WsfStringId(aVarArgs[0].GetString());
   aReturnVal.SetBool(aObjectPtr->IsGroupMember(groupNameId));
}

// =================================================================================================
//! bool isGroupMemberOf = GroupMemberOf(WsfGroup aGroup);
//! Returns true if the platform is a member of the given group.
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GroupMemberOf_3, 1, "bool", "WsfGroup")
{
   bool      isMember(false);
   WsfGroup* groupPtr = (WsfGroup*)aVarArgs[0].GetPointer()->GetAppObject();
   if (groupPtr != nullptr)
   {
      isMember = aObjectPtr->IsGroupMember(groupPtr->GetNameId());
   }
   aReturnVal.SetBool(isMember);
}

// =================================================================================================
//! WsfGroup GroupByIndex(int aIndex); returns the group object that is in the given element,
//!    if the platform is a member
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GetGroupByIndex, 1, "WsfGroup", "int") // NO_DOC | DEPRECATED
{
   WsfGroupList::GroupList tempIdList = aObjectPtr->GetGroups().GetGroupList();

   WsfGroup* groupPtr = SIMULATION->GetGroupManager().GetGroup(tempIdList[aVarArgs[0].GetInt()]);
   if ((groupPtr != nullptr) && groupPtr->IsGroupMember(aObjectPtr->GetIndex()))
   {
      aReturnVal.SetPointer(new UtScriptRef(groupPtr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
      return;
   }
   aReturnVal.SetPointer(new UtScriptRef(nullptr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

// =================================================================================================
//! WsfGroup GroupByName(string aGroupName);
//!   returns the WsfGroup object that matches the given name, if the platform is a member.
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GetGroupByName, 1, "WsfGroup", "string") // NO_DOC | DEPRECATED
{
   WsfGroup* groupPtr = SIMULATION->GetGroupManager().GetGroup(WsfStringId(aVarArgs[0].GetString()));
   if ((groupPtr != nullptr) && groupPtr->IsGroupMember(aObjectPtr->GetIndex()))
   {
      aReturnVal.SetPointer(new UtScriptRef(groupPtr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
      return;
   }

   aReturnVal.SetPointer(new UtScriptRef(nullptr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

// =================================================================================================
//! GroupsByName(); returns a list of group names that this platform is a member of
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GroupsByName, 0, "Array<string>", "")
{
   std::vector<UtScriptData>* resultVecPtr = new std::vector<UtScriptData>();

   WsfGroupList::GroupList tempIdList = aObjectPtr->GetGroups().GetGroupList();
   for (const auto& id : tempIdList)
   {
      resultVecPtr->emplace_back(id);
   }

   aReturnVal.SetPointer(new UtScriptRef(resultVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
//! Groups(); returns a list of group objects that this platform is a member of
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, Groups, 0, "Array<WsfGroup>", "")
{
   std::vector<UtScriptData>* resultVecPtr  = new std::vector<UtScriptData>();
   static UtScriptClass*      groupClassPtr = aContext.GetTypes()->GetClass("WsfGroup");
   WsfGroupList::GroupList    tempIdList    = aObjectPtr->GetGroups().GetGroupList();
   for (auto id : tempIdList)
   {
      WsfGroup* groupPtr = SIMULATION->GetGroupManager().GetGroup(id);
      if (groupPtr != nullptr)
      {
         resultVecPtr->emplace_back(new UtScriptRef(groupPtr, groupClassPtr, UtScriptRef::cDONT_MANAGE));
      }
   }

   aReturnVal.SetPointer(new UtScriptRef(resultVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
//! GroupJoin(int); add the platform to the given group
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GroupJoin, 1, "void", "string")
{
   WsfGroup* groupPtr = SIMULATION->GetGroupManager().GetGroup(WsfStringId(aVarArgs[0].GetString()));
   if (groupPtr != nullptr)
   {
      aObjectPtr->JoinGroup(groupPtr);
   }
}

// =================================================================================================
//! GroupLeave(int); remove the platform from the given group
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GroupLeave, 1, "void", "string")
{
   WsfGroup* groupPtr = SIMULATION->GetGroupManager().GetGroup(WsfStringId(aVarArgs[0].GetString()));
   if (groupPtr != nullptr)
   {
      aObjectPtr->LeaveGroup(groupPtr);
   }
}

// =================================================================================================
//! PrintComponentInfo(); print the list of platform components and their types to ut::log::info
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, PrintComponentInfo, 0, "void", "")
{
   auto out = ut::log::info() << "Component List for Platform: " + aObjectPtr->GetName();
   for (const auto& component : aObjectPtr->GetComponents())
   {
      out.AddNote() << component->GetComponentName() << " (" << ut::TypeNameOf(*component) << ")";
   }
}

// =================================================================================================
//! GetComponentInfo(); Get the list of platform component info and their types
UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, GetComponentInfo, 0, "string", "")
{
   std::string componentList;
   for (const auto& component : aObjectPtr->GetComponents())
   {
      componentList += component->GetComponentName() + " (" + ut::TypeNameOf(*component) + ")\n";
   }
   // Create return value without final newline
   aReturnVal.SetString(componentList.substr(0, componentList.length() - 1));
}

UT_DEFINE_AUX_DATA_SCRIPT_METHODS(WsfScriptPlatformClass, WsfPlatform)
