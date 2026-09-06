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

#ifndef WSFSCRIPTSPACEMOVERCLASS_HPP
#define WSFSCRIPTSPACEMOVERCLASS_HPP

#include "wsf_space_export.h"

#include "script/WsfScriptMoverClass.hpp"

//! Define script methods for space movers that support maneuvering.
class WSF_SPACE_EXPORT WsfScriptSpaceMoverClass : public WsfScriptMoverClass
{
public:
   WsfScriptSpaceMoverClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(LocationAtTime);
   UT_DECLARE_SCRIPT_METHOD(LocationECI_AtTime);
   UT_DECLARE_SCRIPT_METHOD(VelocityECI_AtTime);
   UT_DECLARE_SCRIPT_METHOD(SemiMajorAxis);
   UT_DECLARE_SCRIPT_METHOD(Eccentricity);
   UT_DECLARE_SCRIPT_METHOD(RAAN);
   UT_DECLARE_SCRIPT_METHOD(ArgumentOfPeriapsis);
   UT_DECLARE_SCRIPT_METHOD(Inclination);
   UT_DECLARE_SCRIPT_METHOD(MeanMotion);
   UT_DECLARE_SCRIPT_METHOD(MeanAnomaly);
   UT_DECLARE_SCRIPT_METHOD(TrueAnomaly);
   UT_DECLARE_SCRIPT_METHOD(SetOrientation_1);
   UT_DECLARE_SCRIPT_METHOD(SetOrientation_2);
   UT_DECLARE_SCRIPT_METHOD(SetOrientation_3);
   UT_DECLARE_SCRIPT_METHOD(SetOrientation_4);
   UT_DECLARE_SCRIPT_METHOD(SetOrientation_5);
   UT_DECLARE_SCRIPT_METHOD(SetOrientation_6);
   UT_DECLARE_SCRIPT_METHOD(GetTimeToConstraint);

   UT_DECLARE_SCRIPT_METHOD(OrbitalPeriod);
   UT_DECLARE_SCRIPT_METHOD(RevolutionsPerDay);
   UT_DECLARE_SCRIPT_METHOD(OrbitalRegime);
   UT_DECLARE_SCRIPT_METHOD(RevolutionNumber_1);
   UT_DECLARE_SCRIPT_METHOD(RevolutionNumber_2);
   UT_DECLARE_SCRIPT_METHOD(Apoapsis);
   UT_DECLARE_SCRIPT_METHOD(ApoapsisRadius);
   UT_DECLARE_SCRIPT_METHOD(ApoapsisAltitude);
   UT_DECLARE_SCRIPT_METHOD(Periapsis);
   UT_DECLARE_SCRIPT_METHOD(PeriapsisRadius);
   UT_DECLARE_SCRIPT_METHOD(PeriapsisAltitude);

   // Initial Orbital Elements Accessor Methods
   UT_DECLARE_SCRIPT_METHOD(InitialEpoch);
   UT_DECLARE_SCRIPT_METHOD(InitialArgumentOfPeriapsis);
   UT_DECLARE_SCRIPT_METHOD(InitialEccentricity);
   UT_DECLARE_SCRIPT_METHOD(InitialInclination);
   UT_DECLARE_SCRIPT_METHOD(InitialMeanAnomaly);
   UT_DECLARE_SCRIPT_METHOD(InitialMeanMotion);
   UT_DECLARE_SCRIPT_METHOD(InitialRAAN);
   UT_DECLARE_SCRIPT_METHOD(InitialSemiMajorAxis);
   UT_DECLARE_SCRIPT_METHOD(InitialTrueAnomaly);

   // Initial State Accessor Methods
   UT_DECLARE_SCRIPT_METHOD(InitialPositionLLA);
   UT_DECLARE_SCRIPT_METHOD(InitialPositionECI);
   UT_DECLARE_SCRIPT_METHOD(InitialPositionWCS);
   UT_DECLARE_SCRIPT_METHOD(InitialVelocityECI);
   UT_DECLARE_SCRIPT_METHOD(InitialVelocityWCS);

   // Initial State Convenience Methods
   UT_DECLARE_SCRIPT_METHOD(InitialHeading);

   // Eclipse methods
   UT_DECLARE_SCRIPT_METHOD(EclipseAnomalies);
   UT_DECLARE_SCRIPT_METHOD(EclipsedState);
   UT_DECLARE_SCRIPT_METHOD(EclipseTimes);

   UT_DECLARE_SCRIPT_METHOD(SetInitialPoint_1);
   UT_DECLARE_SCRIPT_METHOD(SetInitialPoint_2);
   UT_DECLARE_SCRIPT_METHOD(SetInitialPoint_3);
   UT_DECLARE_SCRIPT_METHOD(SetInitialOrbitalState);

   // TLE-related methods
   UT_DECLARE_SCRIPT_METHOD(TwoLineElement);
   UT_DECLARE_SCRIPT_METHOD(SetTwoLineElement);
   UT_DECLARE_SCRIPT_METHOD(InitialTwoLineElement);
   UT_DECLARE_SCRIPT_METHOD(SatelliteNumber);
   UT_DECLARE_SCRIPT_METHOD(Classification);
   UT_DECLARE_SCRIPT_METHOD(BallisticCoefficient);
   UT_DECLARE_SCRIPT_METHOD(BStar);

   // Maneuvering & mission event methods
   UT_DECLARE_SCRIPT_METHOD(CurrentOrbitalEvent);
   UT_DECLARE_SCRIPT_METHOD(ExecuteOrbitalEvent);
   UT_DECLARE_SCRIPT_METHOD(ExecuteMissionSequence);
   UT_DECLARE_SCRIPT_METHOD(CancelMissionSequence);

   UT_DECLARE_SCRIPT_METHOD(CurrentManeuver);
   UT_DECLARE_SCRIPT_METHOD(ExecuteManeuver);
   UT_DECLARE_SCRIPT_METHOD(ExecuteManeuverSequence);

   UT_DECLARE_SCRIPT_METHOD(AvailableDeltaV);
   UT_DECLARE_SCRIPT_METHOD(AvailableManeuverDuration);
   UT_DECLARE_SCRIPT_METHOD(ManeuverDuration);

   // Speculate about possible maneuvers
   UT_DECLARE_SCRIPT_METHOD(ComputeRendezvousDeprecated);
   UT_DECLARE_SCRIPT_METHOD(ComputeInterceptDeprecated);
   UT_DECLARE_SCRIPT_METHOD(ComputeRendezvousTrackOptimized);
   UT_DECLARE_SCRIPT_METHOD(ComputeInterceptTrackOptimized);
   UT_DECLARE_SCRIPT_METHOD(ComputeRendezvousTrackFixed);
   UT_DECLARE_SCRIPT_METHOD(ComputeInterceptTrackFixed);
   UT_DECLARE_SCRIPT_METHOD(ComputeRendezvousTargetPointOptimized);
   UT_DECLARE_SCRIPT_METHOD(ComputeRendezvousTargetPointCost);
   UT_DECLARE_SCRIPT_METHOD(ComputeRendezvousTargetPointFixed);
   UT_DECLARE_SCRIPT_METHOD(ComputeInterceptTargetPointOptimized);
   UT_DECLARE_SCRIPT_METHOD(ComputeInterceptTargetPointCost);
   UT_DECLARE_SCRIPT_METHOD(ComputeInterceptTargetPointFixed);

   // Get the descriptor/designator
   UT_DECLARE_SCRIPT_METHOD(Descriptor);
   UT_DECLARE_SCRIPT_METHOD(Designator);

   // Get/Set the orbit color
   UT_DECLARE_SCRIPT_METHOD(SetOrbitColor);
   UT_DECLARE_SCRIPT_METHOD(GetOrbitColor);

   // RIC Calculation Functions
   UT_DECLARE_SCRIPT_METHOD(RIC_1);
   UT_DECLARE_SCRIPT_METHOD(RIC_2);
   UT_DECLARE_SCRIPT_METHOD(RIC_AtTime_1);
   UT_DECLARE_SCRIPT_METHOD(RIC_AtTime_2);

   // Convenience Methods
   UT_DECLARE_SCRIPT_METHOD(LookAngleTo_Track);
   UT_DECLARE_SCRIPT_METHOD(LookAngleTo_Platform);
   UT_DECLARE_SCRIPT_METHOD(LookAngleTo_LLA);
   UT_DECLARE_SCRIPT_METHOD(LookAngleTo_GeoPoint);
   UT_DECLARE_SCRIPT_METHOD(IncidenceAngle_Track);
   UT_DECLARE_SCRIPT_METHOD(IncidenceAngle_Platform);
   UT_DECLARE_SCRIPT_METHOD(IncidenceAngle_LLA);
   UT_DECLARE_SCRIPT_METHOD(IncidenceAngle_GeoPoint);
   UT_DECLARE_SCRIPT_METHOD(TargetElevationAngle_Track);
   UT_DECLARE_SCRIPT_METHOD(TargetElevationAngle_Platform);
   UT_DECLARE_SCRIPT_METHOD(TargetElevationAngle_LLA);
   UT_DECLARE_SCRIPT_METHOD(TargetElevationAngle_GeoPoint);
   UT_DECLARE_SCRIPT_METHOD(SquintAngleTo_Track);
   UT_DECLARE_SCRIPT_METHOD(SquintAngleTo_Platform);
   UT_DECLARE_SCRIPT_METHOD(SquintAngleTo_LLA);
   UT_DECLARE_SCRIPT_METHOD(SquintAngleTo_GeoPoint);
   UT_DECLARE_SCRIPT_METHOD(DistanceToHorizon);
   UT_DECLARE_SCRIPT_METHOD(LookAngleToHorizon);
};

#endif
