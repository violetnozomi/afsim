// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSCRIPTORBITALMANEUVERS_HPP
#define WSFSCRIPTORBITALMANEUVERS_HPP

#include "wsf_space_export.h"

#include "script/WsfScriptObjectClass.hpp"

namespace wsf
{

namespace space
{

namespace script
{

//! Define a script class for the WsfOrbitalEvent::Condition object.
//! These objects are primarily meant to be passed as arguments to WsfOrbitalManeuver child "Construct" methods.
class WSF_SPACE_EXPORT OrbitalEventCondition : public WsfScriptObjectClass
{
public:
   OrbitalEventCondition(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~OrbitalEventCondition() override = default;

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   //!@name Script methods documented in the AFSIM Reference Guide:
   //!@name Script methods documented in the AFSIM Reference Guide:
   // Methods that return WsfOrbitalEventCondition
   UT_DECLARE_SCRIPT_METHOD(NONE);
   UT_DECLARE_SCRIPT_METHOD(AT_RELATIVE_TIME);
   UT_DECLARE_SCRIPT_METHOD(AT_PERIAPSIS);
   UT_DECLARE_SCRIPT_METHOD(AT_APOAPSIS);
   UT_DECLARE_SCRIPT_METHOD(AT_ASCENDING_NODE);
   UT_DECLARE_SCRIPT_METHOD(AT_DESCENDING_NODE);
   UT_DECLARE_SCRIPT_METHOD(AT_ASCENDING_RADIUS);
   UT_DECLARE_SCRIPT_METHOD(AT_DESCENDING_RADIUS);
   UT_DECLARE_SCRIPT_METHOD(AT_NORTHERN_INTERSECTION);
   UT_DECLARE_SCRIPT_METHOD(AT_SOUTHERN_INTERSECTION);
   UT_DECLARE_SCRIPT_METHOD(AT_ECLIPSE_ENTRY);
   UT_DECLARE_SCRIPT_METHOD(AT_ECLIPSE_EXIT);

   UT_DECLARE_SCRIPT_METHOD(ConditionType);
   UT_DECLARE_SCRIPT_METHOD(SetOrbitNumber);
};

//! (Deprecated) Same as OrbitalEventCondition, except named differently, for use with orbital maneuvers.
class WSF_SPACE_EXPORT OrbitalManeuverCondition : public OrbitalEventCondition
{
public:
   OrbitalManeuverCondition(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~OrbitalManeuverCondition() override = default;
};

//! A scriptable version of WsfOrbitalEvent.
class WSF_SPACE_EXPORT OrbitalEvent : public WsfScriptObjectClass
{
public:
   OrbitalEvent(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~OrbitalEvent() override = default;

   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   // Methods
   UT_DECLARE_SCRIPT_METHOD(Condition);
   UT_DECLARE_SCRIPT_METHOD(ConditionType);
   UT_DECLARE_SCRIPT_METHOD(EvaluationTime);
   UT_DECLARE_SCRIPT_METHOD(Duration);
   UT_DECLARE_SCRIPT_METHOD(EventType);
   UT_DECLARE_SCRIPT_METHOD(IsComplete);
   UT_DECLARE_SCRIPT_METHOD(IsFinite);
   UT_DECLARE_SCRIPT_METHOD(ManeuverType);
   UT_DECLARE_SCRIPT_METHOD(OrbitNumber);
   UT_DECLARE_SCRIPT_METHOD(Radius);
   UT_DECLARE_SCRIPT_METHOD(RelativeTime);
   UT_DECLARE_SCRIPT_METHOD(SetCondition);
   UT_DECLARE_SCRIPT_METHOD(SetFinite);
   UT_DECLARE_SCRIPT_METHOD(SetDuration);
   UT_DECLARE_SCRIPT_METHOD(SetUpdateInterval);
   UT_DECLARE_SCRIPT_METHOD(StartTime);
};

//! Same as OrbitalEvent, except named differently, for use with orbital maneuvers.
class WSF_SPACE_EXPORT OrbitalManeuver : public OrbitalEvent
{
public:
   OrbitalManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~OrbitalManeuver() override = default;

   // Methods that return optimize option identifiers for the intercept & rendezvous maneuvers.
   UT_DECLARE_SCRIPT_METHOD(EARLIEST_TIME);
   UT_DECLARE_SCRIPT_METHOD(LEAST_DELTA_V);
   UT_DECLARE_SCRIPT_METHOD(NONE);
};

//! A scriptable version of WsfOrbitalManeuvers::CompoundManeuver.
class WSF_SPACE_EXPORT CompoundManeuver : public OrbitalManeuver
{
public:
   CompoundManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~CompoundManeuver() override = default;

   //!@name Script methods documented in the AFSIM Reference Guide:
   UT_DECLARE_SCRIPT_METHOD(Construct);
   UT_DECLARE_SCRIPT_METHOD(AddManeuver);
};

//! A scriptable version of WsfOrbitalManeuvers::ChangeEccentricity.
class WSF_SPACE_EXPORT ChangeEccentricity : public OrbitalManeuver
{
public:
   ChangeEccentricity(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~ChangeEccentricity() override = default;

   //!@name Script methods documented in the AFSIM Reference Guide:
   UT_DECLARE_SCRIPT_METHOD(Construct);
};

//! A scriptable version of WsfOrbitalManeuvers::Circularize.
class WSF_SPACE_EXPORT Circularize : public OrbitalManeuver
{
public:
   Circularize(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~Circularize() override = default;

   //!@name Script methods documented in the AFSIM Reference Guide:
   UT_DECLARE_SCRIPT_METHOD(Construct);
};

//! A scriptable version of WsfOrbitalManeuvers::ChangeInclination.
class WSF_SPACE_EXPORT ChangeInclination : public OrbitalManeuver
{
public:
   ChangeInclination(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~ChangeInclination() override = default;

   //!@name Script methods documented in the AFSIM Reference Guide:
   UT_DECLARE_SCRIPT_METHOD(Construct);
};

//! A scriptable version of WsfOrbitalManeuvers::ChangeRAAN.
class WSF_SPACE_EXPORT ChangeRAAN : public OrbitalManeuver
{
public:
   ChangeRAAN(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~ChangeRAAN() override = default;

   //!@name Script methods documented in the AFSIM Reference Guide:
   UT_DECLARE_SCRIPT_METHOD(Construct);
   UT_DECLARE_SCRIPT_METHOD(RAAN);
};

//! A scriptable version of WsfOrbitalManeuvers::ChangeRAAN_Inclination.
class WSF_SPACE_EXPORT ChangeRAAN_Inclination : public OrbitalManeuver
{
public:
   ChangeRAAN_Inclination(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~ChangeRAAN_Inclination() override = default;

   //!@name Script methods documented in the AFSIM Reference Guide:
   UT_DECLARE_SCRIPT_METHOD(Construct);
   UT_DECLARE_SCRIPT_METHOD(RAAN);
   UT_DECLARE_SCRIPT_METHOD(Inclination);
};

//! A scriptable version of WsfOrbitalManeuvers::DeltaV.
class WSF_SPACE_EXPORT DeltaV : public OrbitalManeuver
{
public:
   DeltaV(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~DeltaV() override = default;

   //!@name Script methods documented in the AFSIM Reference Guide:
   UT_DECLARE_SCRIPT_METHOD(Construct_Deprecated); // DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(Construct);
   UT_DECLARE_SCRIPT_METHOD(Frame);
   UT_DECLARE_SCRIPT_METHOD(DeltaV_1);
};

class WSF_SPACE_EXPORT Drift : public OrbitalManeuver
{
public:
   Drift(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~Drift() override = default;

   UT_DECLARE_SCRIPT_METHOD(Construct_1);
   UT_DECLARE_SCRIPT_METHOD(Construct_2);
   UT_DECLARE_SCRIPT_METHOD(ConstructCost);

   UT_DECLARE_SCRIPT_METHOD(DriftRate);
   UT_DECLARE_SCRIPT_METHOD(SetDriftRate);
};

//! A scriptable version of WsfOrbitalManeuvers::Tangent.
class WSF_SPACE_EXPORT Tangent : public OrbitalManeuver
{
public:
   Tangent(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~Tangent() override = default;

   //!@name Script methods documented in the AFSIM Reference Guide:
   UT_DECLARE_SCRIPT_METHOD(Construct);
};

//! A scriptable version of WsfOrbitalManeuvers::Normal.
class WSF_SPACE_EXPORT Normal : public OrbitalManeuver
{
public:
   Normal(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~Normal() override = default;

   //!@name Script methods documented in the AFSIM Reference Guide:
   UT_DECLARE_SCRIPT_METHOD(Construct);
};

//! A scriptable version of WsfOrbitalManeuvers::HohmannTransfer.
class WSF_SPACE_EXPORT HohmannTransfer : public OrbitalManeuver
{
public:
   HohmannTransfer(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~HohmannTransfer() override = default;

   //!@name Script methods documented in the AFSIM Reference Guide:
   UT_DECLARE_SCRIPT_METHOD(Construct);
};

//! A scriptable version of WsfOrbitalManeuvers::ChangeSemiMajorAxis.
class WSF_SPACE_EXPORT ChangeSemiMajorAxis : public OrbitalManeuver
{
public:
   ChangeSemiMajorAxis(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~ChangeSemiMajorAxis() override = default;

   //!@name Script methods documented in the AFSIM Reference Guide:
   UT_DECLARE_SCRIPT_METHOD(Construct);
};

//! A scriptable version of WsfOrbitalManeuvers::ChangeSemiMajorAxis.
class WSF_SPACE_EXPORT MatchVelocity : public OrbitalManeuver
{
public:
   MatchVelocity(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~MatchVelocity() override = default;

   //!@name Script methods documented in the AFSIM Reference Guide:
   UT_DECLARE_SCRIPT_METHOD(Construct);
};

class WSF_SPACE_EXPORT NaturalMotionCircumnavigation : public OrbitalManeuver
{
public:
   NaturalMotionCircumnavigation(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~NaturalMotionCircumnavigation() override = default;

   UT_DECLARE_SCRIPT_METHOD(Construct_1);
   UT_DECLARE_SCRIPT_METHOD(Construct_2);
   UT_DECLARE_SCRIPT_METHOD(ConstructCost);

   UT_DECLARE_SCRIPT_METHOD(OrbitSize);
   UT_DECLARE_SCRIPT_METHOD(OrbitPhase);
   UT_DECLARE_SCRIPT_METHOD(OutOfPlaneAmplitude);
   UT_DECLARE_SCRIPT_METHOD(OutOfPlanePhase);

   UT_DECLARE_SCRIPT_METHOD(SetOrbitSize);
   UT_DECLARE_SCRIPT_METHOD(SetOrbitPhase);
   UT_DECLARE_SCRIPT_METHOD(SetOutOfPlaneAmplitude);
   UT_DECLARE_SCRIPT_METHOD(SetOutOfPlanePhase);
};

//! A scriptable version of WsfOrbitalManeuvers::Intercept.
class WSF_SPACE_EXPORT Intercept : public OrbitalManeuver
{
public:
   Intercept(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~Intercept() override = default;

   //!@name Script methods documented in the AFSIM Reference Guide:
   UT_DECLARE_SCRIPT_METHOD(Construct_1);
   UT_DECLARE_SCRIPT_METHOD(Construct_2);
   UT_DECLARE_SCRIPT_METHOD(ConstructCost);

   UT_DECLARE_SCRIPT_METHOD(TargetPoint);

   UT_DECLARE_SCRIPT_METHOD(Tolerance);
   UT_DECLARE_SCRIPT_METHOD(SetTolerance);
};

//! A scriptable version of WsfOrbitalManeuvers::Rendezvous.
class WSF_SPACE_EXPORT Rendezvous : public OrbitalManeuver
{
public:
   Rendezvous(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~Rendezvous() override = default;

   //!@name Script methods documented in the AFSIM Reference Guide:
   UT_DECLARE_SCRIPT_METHOD(Construct_1);
   UT_DECLARE_SCRIPT_METHOD(Construct_2);
   UT_DECLARE_SCRIPT_METHOD(ConstructCost);

   UT_DECLARE_SCRIPT_METHOD(TargetPoint);

   UT_DECLARE_SCRIPT_METHOD(Tolerance);
   UT_DECLARE_SCRIPT_METHOD(SetTolerance);
};

//! A scriptable version of WsfOrbitalManeuvers::Rendezvous.
class WSF_SPACE_EXPORT Target : public OrbitalManeuver
{
public:
   Target(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~Target() override = default;

   //!@name Script methods documented in the AFSIM Reference Guide:
   UT_DECLARE_SCRIPT_METHOD(Construct_1);
   UT_DECLARE_SCRIPT_METHOD(Construct_2);
   UT_DECLARE_SCRIPT_METHOD(ConstructCost);

   UT_DECLARE_SCRIPT_METHOD(TargetPoint);

   UT_DECLARE_SCRIPT_METHOD(Tolerance);
   UT_DECLARE_SCRIPT_METHOD(SetTolerance);
};

//! A scriptable version of WsfOrbitalManeuvers::Teardrop.
class WSF_SPACE_EXPORT Teardrop : public OrbitalManeuver
{
public:
   Teardrop(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~Teardrop() override = default;

   UT_DECLARE_SCRIPT_METHOD(Construct_1);
   UT_DECLARE_SCRIPT_METHOD(Construct_2);
   UT_DECLARE_SCRIPT_METHOD(ConstructCost);

   UT_DECLARE_SCRIPT_METHOD(Period);
   UT_DECLARE_SCRIPT_METHOD(RadialOffsetAtPOCA);
   UT_DECLARE_SCRIPT_METHOD(TimeToPOCA);
   UT_DECLARE_SCRIPT_METHOD(Repetitions);

   UT_DECLARE_SCRIPT_METHOD(SetRepetitions);
};

//! A scriptable version of WsfOrbitalStagingEvent.
class WSF_SPACE_EXPORT PerformStaging : public OrbitalEvent
{
public:
   PerformStaging(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~PerformStaging() override = default;

   //!@name Script methods documented in the AFSIM Reference Guide:
   UT_DECLARE_SCRIPT_METHOD(Construct);
};

//! A scriptable version of WsfOrbitalScriptEvent.
class WSF_SPACE_EXPORT Scripted : public OrbitalEvent
{
public:
   Scripted(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~Scripted() override = default;

   //!@name Script methods documented in the AFSIM Reference Guide:
   UT_DECLARE_SCRIPT_METHOD(Construct);
};

//! A scriptable version of WsfOrbitalAttitudeChangeEvent.
class WSF_SPACE_EXPORT ChangeAttitude : public OrbitalEvent
{
public:
   ChangeAttitude(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~ChangeAttitude() override = default;

   //!@name Script methods documented in the AFSIM Reference Guide:
   UT_DECLARE_SCRIPT_METHOD(Construct);
};

} // namespace script

} // namespace space

} // namespace wsf

#endif
