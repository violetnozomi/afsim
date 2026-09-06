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

#ifndef WSFSCRIPTPLATFORMCLASS_HPP
#define WSFSCRIPTPLATFORMCLASS_HPP

#include "wsf_export.h"

#include "WsfScriptAuxDataUtil.hpp"
#include "WsfScriptObjectClass.hpp"

//! WsfScriptPlatformClass is a WsfScriptObjectClass that defines the methods in
//! WsfPlatform.  This makes the methods available for use in script.
class WSF_EXPORT WsfScriptPlatformClass : public WsfScriptObjectClass
{
public:
   WsfScriptPlatformClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);
   ~WsfScriptPlatformClass() override = default;

   bool        EqualTo(void* aLHS, void* aRHS) override;
   bool        LessThan(void* aLHS, void* aRHS) override;
   std::string ToString(void* aObjectPtr) const override;
   void        OnNewScriptRef(UtScriptRef& aReference) override;

   // Each of the exported methods are defined as function objects.

   UT_DECLARE_SCRIPT_METHOD(IsA_TypeOf); // static IsA_TypeOf("derived-type", "base-type")

   UT_DECLARE_SCRIPT_METHOD(Index);
   UT_DECLARE_SCRIPT_METHOD(CreationTime);
   UT_DECLARE_SCRIPT_METHOD(SetCreationTime);
   UT_DECLARE_SCRIPT_METHOD(TimeSinceCreation);
   UT_DECLARE_SCRIPT_METHOD(SetSide);
   UT_DECLARE_SCRIPT_METHOD(Side);
   UT_DECLARE_SCRIPT_METHOD(SetIcon);
   UT_DECLARE_SCRIPT_METHOD(Icon);
   UT_DECLARE_SCRIPT_METHOD(Commander_1);
   UT_DECLARE_SCRIPT_METHOD(Commander_2);
   UT_DECLARE_SCRIPT_METHOD(SetCommander_1);
   UT_DECLARE_SCRIPT_METHOD(SetCommander_2);
   UT_DECLARE_SCRIPT_METHOD(CommanderName_1);
   UT_DECLARE_SCRIPT_METHOD(CommanderName_2);
   UT_DECLARE_SCRIPT_METHOD(Peers_1);
   UT_DECLARE_SCRIPT_METHOD(Peers_2);
   UT_DECLARE_SCRIPT_METHOD(Subordinates_1);
   UT_DECLARE_SCRIPT_METHOD(Subordinates_2);
   UT_DECLARE_SCRIPT_METHOD(CommandChain);
   UT_DECLARE_SCRIPT_METHOD(CommandChainCount);
   UT_DECLARE_SCRIPT_METHOD(CommandChainEntry);
   UT_DECLARE_SCRIPT_METHOD(SetMoverDebugEnabled);
   UT_DECLARE_SCRIPT_METHOD(Comment_1);
   UT_DECLARE_SCRIPT_METHOD(Comment_2);
   UT_DECLARE_SCRIPT_METHOD(IsExternallyControlled);
   UT_DECLARE_SCRIPT_METHOD(SpatialDomain);

   UT_DECLARE_SCRIPT_METHOD(WCS_To_ECS_DCM);
   UT_DECLARE_SCRIPT_METHOD(WCS_To_NED_DCM);
   UT_DECLARE_SCRIPT_METHOD(LineOfSightRatesTo);

   // Subsystem management

   UT_DECLARE_SCRIPT_METHOD(Mover);
   UT_DECLARE_SCRIPT_METHOD(Comm);
   UT_DECLARE_SCRIPT_METHOD(CommCount);
   UT_DECLARE_SCRIPT_METHOD(CommEntry);

   UT_DECLARE_SCRIPT_METHOD(Router);
   UT_DECLARE_SCRIPT_METHOD(RouterCount);
   UT_DECLARE_SCRIPT_METHOD(RouterEntry);

   UT_DECLARE_SCRIPT_METHOD(Fuel);
   UT_DECLARE_SCRIPT_METHOD(FuelRemaining);     // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(Refuel_1);          // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(Refuel_2);          // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(FuelBingoQuantity); // NO_DOC | DEPRECATED

   UT_DECLARE_SCRIPT_METHOD(GPS_Status);
   UT_DECLARE_SCRIPT_METHOD(SetGPS_Status);
   UT_DECLARE_SCRIPT_METHOD(SetPerceivedLocationErrorWCS);

   UT_DECLARE_SCRIPT_METHOD(Processor);
   UT_DECLARE_SCRIPT_METHOD(ProcessorCount);
   UT_DECLARE_SCRIPT_METHOD(ProcessorEntry);
   UT_DECLARE_SCRIPT_METHOD(Sensor);
   UT_DECLARE_SCRIPT_METHOD(SensorCount);
   UT_DECLARE_SCRIPT_METHOD(SensorEntry);
   UT_DECLARE_SCRIPT_METHOD(VisualPart); // NO_DOC | FOR_TEST_ONLY

   UT_DECLARE_SCRIPT_METHOD(TurnCommOn);
   UT_DECLARE_SCRIPT_METHOD(TurnCommOff);
   UT_DECLARE_SCRIPT_METHOD(TurnProcessorOn);
   UT_DECLARE_SCRIPT_METHOD(TurnProcessorOff);
   UT_DECLARE_SCRIPT_METHOD(TurnSensorOn);
   UT_DECLARE_SCRIPT_METHOD(TurnSensorOff);
   UT_DECLARE_SCRIPT_METHOD(TurnMoverOn);
   UT_DECLARE_SCRIPT_METHOD(TurnMoverOff);
   UT_DECLARE_SCRIPT_METHOD(TurnRouterOn);
   UT_DECLARE_SCRIPT_METHOD(TurnRouterOff);

   UT_DECLARE_SCRIPT_METHOD(TrackManager);

   // Appearance methods

   UT_DECLARE_SCRIPT_METHOD(Length);
   UT_DECLARE_SCRIPT_METHOD(Width);
   UT_DECLARE_SCRIPT_METHOD(Height);
   UT_DECLARE_SCRIPT_METHOD(TotalMass);
   UT_DECLARE_SCRIPT_METHOD(EmptyMass);
   UT_DECLARE_SCRIPT_METHOD(FuelMass);
   UT_DECLARE_SCRIPT_METHOD(PayloadMass);
   UT_DECLARE_SCRIPT_METHOD(ConcealmentFactor);
   UT_DECLARE_SCRIPT_METHOD(SetConcealmentFactor);
   UT_DECLARE_SCRIPT_METHOD(AfterburnerFactor);     // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(SetAfterburnerFactor);  // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(ConfigurationState);    // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(SetConfigurationState); // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(Indestructible);
   UT_DECLARE_SCRIPT_METHOD(SetIndestructible);
   UT_DECLARE_SCRIPT_METHOD(SetSignatureState);
   UT_DECLARE_SCRIPT_METHOD(SetDamageFactor);
   UT_DECLARE_SCRIPT_METHOD(DamageFactor);
   UT_DECLARE_SCRIPT_METHOD(SetTrailingEffects);
   UT_DECLARE_SCRIPT_METHOD(Appearance);
   UT_DECLARE_SCRIPT_METHOD(SetAppearance);
   UT_DECLARE_SCRIPT_METHOD(Capabilities);
   UT_DECLARE_SCRIPT_METHOD(SetCapability);
   UT_DECLARE_SCRIPT_METHOD(SetMarking);
   UT_DECLARE_SCRIPT_METHOD(Marking);

   // Script and action methods

   UT_DECLARE_SCRIPT_METHOD(EnterState);
   UT_DECLARE_SCRIPT_METHOD(ExitState);
   UT_DECLARE_SCRIPT_METHOD(InState);

   UT_DECLARE_SCRIPT_METHOD(ScriptExists);
   UT_DECLARE_SCRIPT_METHOD(Execute_1);
   UT_DECLARE_SCRIPT_METHOD(Execute_2);
   UT_DECLARE_SCRIPT_METHOD(ExecuteAtTime_1);
   UT_DECLARE_SCRIPT_METHOD(ExecuteAtTime_2);

   UT_DECLARE_SCRIPT_METHOD(ExecuteScript);
   UT_DECLARE_SCRIPT_METHOD(ExecuteScriptWithArgs);
   UT_DECLARE_SCRIPT_METHOD(ExecuteGlobalScript);

   // Track methods

   UT_DECLARE_SCRIPT_METHOD(MasterTrackList);
   UT_DECLARE_SCRIPT_METHOD(MasterRawTrackList);
   UT_DECLARE_SCRIPT_METHOD(TrackList);
   UT_DECLARE_SCRIPT_METHOD(RawTrackList);
   UT_DECLARE_SCRIPT_METHOD(CurrentTarget);
   UT_DECLARE_SCRIPT_METHOD(SetCurrentTarget);
   UT_DECLARE_SCRIPT_METHOD(HasCurrentTarget);
   UT_DECLARE_SCRIPT_METHOD(ClearCurrentTarget);
   UT_DECLARE_SCRIPT_METHOD(CurrentTargetTrack);
   UT_DECLARE_SCRIPT_METHOD(InterceptLocation2D_1); // InterceptLocation2D(WsfTrack, WsfWaypoint)
   UT_DECLARE_SCRIPT_METHOD(InterceptLocation2D_2); // InterceptLocation2D(WsfTrack, WsfWaypoint, bool)
   UT_DECLARE_SCRIPT_METHOD(InterceptLocation2D_3); // InterceptLocation2D(WsfTrack, WsfWaypoint, double, double)
   UT_DECLARE_SCRIPT_METHOD(InterceptLocation2D_4); // InterceptLocation2D(WsfTrack, WsfWaypoint, double, double, bool)
   UT_DECLARE_SCRIPT_METHOD(InterceptLocation3D_1); // InterceptLocation3D(WsfTrack, WsfWaypoint)
   UT_DECLARE_SCRIPT_METHOD(InterceptLocation3D_2); // InterceptLocation3D(WsfTrack, WsfWaypoint, bool)
   UT_DECLARE_SCRIPT_METHOD(InterceptLocation3D_3); // InterceptLocation3D(WsfTrack, WsfWaypoint, double, double)
   UT_DECLARE_SCRIPT_METHOD(InterceptLocation3D_4); // InterceptLocation2D(WsfTrack, WsfWaypoint, double, double, bool)
   UT_DECLARE_SCRIPT_METHOD(TravelTime);

   // Location and Navigation methods

   UT_DECLARE_SCRIPT_METHOD(SetLocation_1);
   UT_DECLARE_SCRIPT_METHOD(SetLocation_2);
   UT_DECLARE_SCRIPT_METHOD(SetLocationMGRS_1);
   UT_DECLARE_SCRIPT_METHOD(SetLocationMGRS_2);
   UT_DECLARE_SCRIPT_METHOD(SetLocationECI);
   UT_DECLARE_SCRIPT_METHOD(Location);
   UT_DECLARE_SCRIPT_METHOD(LocationWCS);
   UT_DECLARE_SCRIPT_METHOD(LocationECI);
   UT_DECLARE_SCRIPT_METHOD(LocationTEME); // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(LocationJ2000);
   UT_DECLARE_SCRIPT_METHOD(LocationMGRS);
   UT_DECLARE_SCRIPT_METHOD(LocationAsWaypoint);
   UT_DECLARE_SCRIPT_METHOD(Latitude);
   UT_DECLARE_SCRIPT_METHOD(Longitude);
   UT_DECLARE_SCRIPT_METHOD(Altitude);
   UT_DECLARE_SCRIPT_METHOD(HeightAboveTerrain);
   UT_DECLARE_SCRIPT_METHOD(PerceivedLocation);
   UT_DECLARE_SCRIPT_METHOD(PerceivedLocationErrorWCS);
   UT_DECLARE_SCRIPT_METHOD(PerceivedLocationErrorNED);
   UT_DECLARE_SCRIPT_METHOD(FutureLocation);
   UT_DECLARE_SCRIPT_METHOD(Speed);
   UT_DECLARE_SCRIPT_METHOD(GroundSpeed);
   UT_DECLARE_SCRIPT_METHOD(MachNumber);
   UT_DECLARE_SCRIPT_METHOD(Heading);
   UT_DECLARE_SCRIPT_METHOD(Pitch);
   UT_DECLARE_SCRIPT_METHOD(Roll);
   UT_DECLARE_SCRIPT_METHOD(SetHeading);
   UT_DECLARE_SCRIPT_METHOD(SetSpeedAtWaypoint); // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(SetPitch);
   UT_DECLARE_SCRIPT_METHOD(SetRoll);
   UT_DECLARE_SCRIPT_METHOD(X);
   UT_DECLARE_SCRIPT_METHOD(Y);
   UT_DECLARE_SCRIPT_METHOD(Z);
   UT_DECLARE_SCRIPT_METHOD(Vx);
   UT_DECLARE_SCRIPT_METHOD(Vy);
   UT_DECLARE_SCRIPT_METHOD(Vz);
   UT_DECLARE_SCRIPT_METHOD(VelocityWCS);
   UT_DECLARE_SCRIPT_METHOD(VelocityNED);
   UT_DECLARE_SCRIPT_METHOD(VelocityECI);
   UT_DECLARE_SCRIPT_METHOD(VelocityTEME); // NO_DOC | FOR_TEST_ONLY
   UT_DECLARE_SCRIPT_METHOD(VelocityJ2000);
   UT_DECLARE_SCRIPT_METHOD(SetVelocityNED);
   UT_DECLARE_SCRIPT_METHOD(SetVelocityECS);
   UT_DECLARE_SCRIPT_METHOD(AccelerationWCS);
   UT_DECLARE_SCRIPT_METHOD(AccelerationNED);
   UT_DECLARE_SCRIPT_METHOD(AccelerationECI);
   UT_DECLARE_SCRIPT_METHOD(OrientationNED);
   UT_DECLARE_SCRIPT_METHOD(OrientationWCS);
   UT_DECLARE_SCRIPT_METHOD(OrientationECI);
   UT_DECLARE_SCRIPT_METHOD(SetOrientationNED);
   UT_DECLARE_SCRIPT_METHOD(SetOrientationWCS);
   UT_DECLARE_SCRIPT_METHOD(SetOrientationECI);
   UT_DECLARE_SCRIPT_METHOD(GeoPoint);
   UT_DECLARE_SCRIPT_METHOD(GoToPoint_1);
   UT_DECLARE_SCRIPT_METHOD(GoToPoint_2);
   UT_DECLARE_SCRIPT_METHOD(GoToPoint_3);
   UT_DECLARE_SCRIPT_METHOD(GoToPoint_4);
   UT_DECLARE_SCRIPT_METHOD(GoToLabel);
   UT_DECLARE_SCRIPT_METHOD(GoToLocation_1);
   UT_DECLARE_SCRIPT_METHOD(GoToLocation_2);
   UT_DECLARE_SCRIPT_METHOD(GoToLocation_3);
   UT_DECLARE_SCRIPT_METHOD(GoToLocation_4);
   UT_DECLARE_SCRIPT_METHOD(GoToTrack);   // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(GoToTrack_2); // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(GoToAltitude_1);
   UT_DECLARE_SCRIPT_METHOD(GoToAltitude_2);
   UT_DECLARE_SCRIPT_METHOD(GoToAltitude_3);
   UT_DECLARE_SCRIPT_METHOD(GoToSpeed_1);
   UT_DECLARE_SCRIPT_METHOD(GoToSpeed_2);
   UT_DECLARE_SCRIPT_METHOD(GoToSpeed_3);
   UT_DECLARE_SCRIPT_METHOD(GoToMachNumber_1);
   UT_DECLARE_SCRIPT_METHOD(GoToMachNumber_2);
   UT_DECLARE_SCRIPT_METHOD(TurnToHeading_1);
   UT_DECLARE_SCRIPT_METHOD(TurnToHeading_2);
   UT_DECLARE_SCRIPT_METHOD(TurnToRelativeHeading_1);
   UT_DECLARE_SCRIPT_METHOD(TurnToRelativeHeading_2);
   UT_DECLARE_SCRIPT_METHOD(CreateRoute);      // CreateRoute(string)
   UT_DECLARE_SCRIPT_METHOD(FindRouteNetwork); // FindRouteNetwork(name)
   UT_DECLARE_SCRIPT_METHOD(GetRoute);         // Route()
   UT_DECLARE_SCRIPT_METHOD(RoutePointIndex);
   UT_DECLARE_SCRIPT_METHOD(FollowRoute_1);
   UT_DECLARE_SCRIPT_METHOD(FollowRoute_2);
   UT_DECLARE_SCRIPT_METHOD(FollowRoute_3);
   UT_DECLARE_SCRIPT_METHOD(FollowRoute_4);
   UT_DECLARE_SCRIPT_METHOD(FollowRoute_5);
   UT_DECLARE_SCRIPT_METHOD(FollowRoute_6);
   UT_DECLARE_SCRIPT_METHOD(SetRoute_1);
   UT_DECLARE_SCRIPT_METHOD(SetRoute_2);
   UT_DECLARE_SCRIPT_METHOD(DistanceAlongRoute); // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(ReturnToRoute);
   UT_DECLARE_SCRIPT_METHOD(FindAndSetPath); // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(FlyRates);
   UT_DECLARE_SCRIPT_METHOD(FlyHeadingSpeedAltitude);
   UT_DECLARE_SCRIPT_METHOD(PullGsInPlaneWithSpeed);
   UT_DECLARE_SCRIPT_METHOD(PullGsInPlaneWithThrottle);
   UT_DECLARE_SCRIPT_METHOD(FlyVectorWithSpeed);
   UT_DECLARE_SCRIPT_METHOD(FlyVectorWithThrottle);
   UT_DECLARE_SCRIPT_METHOD(SetRandomLocation); // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(SetEndOfPath);
   UT_DECLARE_SCRIPT_METHOD(PathFinder);
   UT_DECLARE_SCRIPT_METHOD(Pause);
   UT_DECLARE_SCRIPT_METHOD(Unpause);
   UT_DECLARE_SCRIPT_METHOD(BreakFormation);
   UT_DECLARE_SCRIPT_METHOD(JoinFormation);
   UT_DECLARE_SCRIPT_METHOD(GetTargetSpeed);

   // Relative geometry methods

   UT_DECLARE_SCRIPT_METHOD(ApparentAspectOf);      // ApparentAspectOf(WsfGepoint, double);
   UT_DECLARE_SCRIPT_METHOD(RelativeAzimuthOf);     // RelativeAzimuthOf(WsfGepoint);
   UT_DECLARE_SCRIPT_METHOD(RelativeElevationOf);   // RelativeElevationOf(WsfGeoPoint);
   UT_DECLARE_SCRIPT_METHOD(TrueBearingTo_1);       // TrueBearingTo(WsfTrack)
   UT_DECLARE_SCRIPT_METHOD(TrueBearingTo_2);       // TrueBearingTo(WsfPlatform)
   UT_DECLARE_SCRIPT_METHOD(TrueBearingTo_3);       // TrueBearingTo(lat, lon, alt)
   UT_DECLARE_SCRIPT_METHOD(TrueBearingTo_4);       // TrueBearingTo(WsfGeoPoint)
   UT_DECLARE_SCRIPT_METHOD(RelativeBearingTo_1);   // RelativeBearingTo(WsfTrack)
   UT_DECLARE_SCRIPT_METHOD(RelativeBearingTo_2);   // RelativeBearingTo(WsfPlatform)
   UT_DECLARE_SCRIPT_METHOD(RelativeBearingTo_3);   // RelativeBearingTo(lat, lon, alt)
   UT_DECLARE_SCRIPT_METHOD(RelativeBearingTo_4);   // RelativeBearingTo(GeoPoint)
   UT_DECLARE_SCRIPT_METHOD(SlantRangeTo_1);        // SlantRangeTo(WsfTrack)
   UT_DECLARE_SCRIPT_METHOD(SlantRangeTo_1s);       // SlantRangeTo(WsfTrack, bool)
   UT_DECLARE_SCRIPT_METHOD(SlantRangeTo_2);        // SlantRangeTo(WsfPlatform)
   UT_DECLARE_SCRIPT_METHOD(SlantRangeTo_2s);       // SlantRangeTo(WsfPlatform, bool)
   UT_DECLARE_SCRIPT_METHOD(SlantRangeTo_3);        // SlantRangeTo(lat, lon, alt)
   UT_DECLARE_SCRIPT_METHOD(SlantRangeTo_3s);       // SlantRangeTo(lat, lon, alt, bool)
   UT_DECLARE_SCRIPT_METHOD(SlantRangeTo_4);        // SlantRangeTo(WsfGeoPoint)
   UT_DECLARE_SCRIPT_METHOD(SlantRangeTo_4s);       // SlantRangeTo(WsfGeoPoint, bool)
   UT_DECLARE_SCRIPT_METHOD(SlantRangeTo_5);        // SlantRangeTo(WsfWaypoint)
   UT_DECLARE_SCRIPT_METHOD(SlantRangeTo_5s);       // SlantRangeTo(WsfWaypoint, bool)
   UT_DECLARE_SCRIPT_METHOD(GroundRangeTo_1);       // GroundRangeTo(WsfTrack)
   UT_DECLARE_SCRIPT_METHOD(GroundRangeTo_2);       // GroundRangeTo(WsfPlatform)
   UT_DECLARE_SCRIPT_METHOD(GroundRangeTo_3);       // GroundRangeTo(lat, lon, alt)
   UT_DECLARE_SCRIPT_METHOD(GroundRangeTo_4);       // GroundRangeTo(WsfGeoPoint)
   UT_DECLARE_SCRIPT_METHOD(DownRangeTo_1);         // DownRangeTo(WsfTrack)
   UT_DECLARE_SCRIPT_METHOD(DownRangeTo_2);         // DownRangeTo(WsfPlatform)
   UT_DECLARE_SCRIPT_METHOD(DownRangeTo_3);         // DownRangeTo(lat, lon, alt)
   UT_DECLARE_SCRIPT_METHOD(DownRangeTo_4);         // DownRangeTo(WsfGeoPoint)
   UT_DECLARE_SCRIPT_METHOD(CrossRangeTo_1);        // CrossRangeTo(WsfTrack)
   UT_DECLARE_SCRIPT_METHOD(CrossRangeTo_2);        // CrossRangeTo(WsfPlatform)
   UT_DECLARE_SCRIPT_METHOD(CrossRangeTo_3);        // CrossRangeTo(lat, lon, alt)
   UT_DECLARE_SCRIPT_METHOD(CrossRangeTo_4);        // CrossRangeTo(WsfGeoPoint)
   UT_DECLARE_SCRIPT_METHOD(ClosestApproachOf_1);   // ClosestApproachOf(WsfTrack)
   UT_DECLARE_SCRIPT_METHOD(ClosestApproachOf_2);   // ClosestApproachOf(WsfPlatform)
   UT_DECLARE_SCRIPT_METHOD(HeadingDifferenceOf_1); // HeadingDifferenceOf(WsfTrack)
   UT_DECLARE_SCRIPT_METHOD(HeadingDifferenceOf_2); // HeadingDifferenceOf(WsfPlatform)
   UT_DECLARE_SCRIPT_METHOD(ClosingSpeedOf_1);      // ClosingSpeedOf(WsfTrack)
   UT_DECLARE_SCRIPT_METHOD(ClosingSpeedOf_2);      // ClosingSpeedOf(WsfPlatform)
   UT_DECLARE_SCRIPT_METHOD(ClosingSpeedOf_3);      // ClosingSpeedOf(WsfTrack, mySpeed)
   UT_DECLARE_SCRIPT_METHOD(ClosingSpeedOf_4);      // ClosingSpeedOf(WsfPlatform, mySpeed)
   UT_DECLARE_SCRIPT_METHOD(WithinFieldOfView_1);   // WithinFieldOfView(WsfTrack, string);
   UT_DECLARE_SCRIPT_METHOD(WithinFieldOfView_2);   // WithinFieldOfView(WsfPlatform, string);
   UT_DECLARE_SCRIPT_METHOD(MaskedByTerrain);

   UT_DECLARE_SCRIPT_METHOD(RelativeHeadingOf);  // RelativeHeadingOf(WsfTrack)
   UT_DECLARE_SCRIPT_METHOD(RelativeOffsetOf);   // RelativeOffsetOf(WsfTrack)
   UT_DECLARE_SCRIPT_METHOD(RelativeAltitudeOf); // RelativeAltitudeOf(WsfTrack);

   UT_DECLARE_SCRIPT_METHOD(ExtrapolateGreatCirclePosition); // ExtrapolateGreatCirclePosition(double, double, double, double)
   UT_DECLARE_SCRIPT_METHOD(GreatCircleHeading);             // GreatCircleHeading(WsfTrack)
   UT_DECLARE_SCRIPT_METHOD(GreatCircleDistance);            // GreatCircleDistance(WsfTrack)
   UT_DECLARE_SCRIPT_METHOD(GreatCircleDestinationTime_1); // GreatCircleDestinationTime(WsfTrack)
   UT_DECLARE_SCRIPT_METHOD(GreatCircleDestinationTime_2); // GreatCircleDestinationTime(double, double, double, double)

   UT_DECLARE_SCRIPT_METHOD(ApparentTime);

   UT_DECLARE_SCRIPT_METHOD(ConvertWCSToECS); // NO WIKI | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(ConvertWCSToECS_2);
   UT_DECLARE_SCRIPT_METHOD(ConvertECSToWCS);
   UT_DECLARE_SCRIPT_METHOD(ConvertECIToWCS);
   UT_DECLARE_SCRIPT_METHOD(ConvertWCSToECI);

   // Zone containment methods

   UT_DECLARE_SCRIPT_METHOD(WithinZone);
   UT_DECLARE_SCRIPT_METHOD(WithinZoneOf);
   UT_DECLARE_SCRIPT_METHOD(Zone);      // WsfZone GetZone(string)
   UT_DECLARE_SCRIPT_METHOD(ZoneNames); // Array<string> ZoneNames()
   UT_DECLARE_SCRIPT_METHOD(Zones);     // Array<WsfZone> Zones()

   UT_DECLARE_SCRIPT_METHOD(DeletePlatform);

   UT_DECLARE_AUX_DATA_SCRIPT_METHODS()

   UT_DECLARE_SCRIPT_METHOD(MakeTrack);
   UT_DECLARE_SCRIPT_METHOD(CategoryMemberOf); // CategoryMemberOf(name)
   UT_DECLARE_SCRIPT_METHOD(AddCategory);

   UT_DECLARE_SCRIPT_METHOD(SwapMover_1); // SwapMover(string)
   UT_DECLARE_SCRIPT_METHOD(SwapMover_2); // SwapMover(WsfMover)

   // Group methods

   UT_DECLARE_SCRIPT_METHOD(GroupMemberOf_1); // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(GroupMemberOf_2); // GroupMemberOf(string)
   UT_DECLARE_SCRIPT_METHOD(GroupMemberOf_3); // GroupMemberOf(WsfGroup)
   UT_DECLARE_SCRIPT_METHOD(GetGroupByIndex); // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(GetGroupByName);  // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(GroupsByName);    // returns Array<string>
   UT_DECLARE_SCRIPT_METHOD(Groups);          // returns Array<WsfGroup>
   UT_DECLARE_SCRIPT_METHOD(GroupJoin);       // GroupJoin(string)
   UT_DECLARE_SCRIPT_METHOD(GroupLeave);      // GroupLeave(string)

   // Component Information Methods

   UT_DECLARE_SCRIPT_METHOD(PrintComponentInfo);
   UT_DECLARE_SCRIPT_METHOD(GetComponentInfo);
};

#endif
