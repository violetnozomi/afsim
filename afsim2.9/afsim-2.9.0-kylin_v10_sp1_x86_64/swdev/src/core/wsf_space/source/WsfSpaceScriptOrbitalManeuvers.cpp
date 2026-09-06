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

#include "WsfSpaceScriptOrbitalManeuvers.hpp"

#include "UtCalendar.hpp"
#include "UtCast.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "UtScriptRef.hpp"
#include "UtScriptVec3.hpp"
#include "UtUnitTypes.hpp"
#include "WsfDateTime.hpp"
#include "WsfLibrationPoint.hpp"
#include "WsfOrbitalAttitudeChangeEvent.hpp"
#include "WsfOrbitalManeuver.hpp"
#include "WsfOrbitalScriptEvent.hpp"
#include "WsfOrbitalStagingEvent.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfSpaceOrientation.hpp"
#include "WsfSpaceScriptOrbitalManeuverSequence.hpp"
#include "WsfTargetPointOptions.hpp"
#include "WsfTrackManager.hpp"
#include "maneuvers/WsfOrbitalManeuversChangeEccentricity.hpp"
#include "maneuvers/WsfOrbitalManeuversChangeInclination.hpp"
#include "maneuvers/WsfOrbitalManeuversChangeRAAN.hpp"
#include "maneuvers/WsfOrbitalManeuversChangeRAAN_Inclination.hpp"
#include "maneuvers/WsfOrbitalManeuversChangeSemiMajorAxis.hpp"
#include "maneuvers/WsfOrbitalManeuversCircularize.hpp"
#include "maneuvers/WsfOrbitalManeuversCompound.hpp"
#include "maneuvers/WsfOrbitalManeuversDeltaV.hpp"
#include "maneuvers/WsfOrbitalManeuversDrift.hpp"
#include "maneuvers/WsfOrbitalManeuversHohmannTransfer.hpp"
#include "maneuvers/WsfOrbitalManeuversIntercept.hpp"
#include "maneuvers/WsfOrbitalManeuversNaturalMotionCircumnavigation.hpp"
#include "maneuvers/WsfOrbitalManeuversNormal.hpp"
#include "maneuvers/WsfOrbitalManeuversRendezvous.hpp"
#include "maneuvers/WsfOrbitalManeuversTangent.hpp"
#include "maneuvers/WsfOrbitalManeuversTarget.hpp"
#include "maneuvers/WsfOrbitalManeuversTeardrop.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptDefs.hpp"

namespace // {anonymous}
{
std::string ValidateScriptTargetOptionsInputArgument(const wsf::space::TargetPointOptions& aOptions,
                                                     UtScriptContext&                      aContext)
{
   std::string retval{};
   if (aOptions.HasTrackTarget())
   {
      if (aOptions.GetLocalTrackId().IsNull())
      {
         retval = "Provided options specify invalid target track id.";
      }
      WsfPlatform* platPtr = SIMULATION->GetPlatformByName(aOptions.GetLocalTrackId().GetOwningPlatformId());
      if (platPtr == nullptr)
      {
         retval = "Platform referenced by provided target track id does not (no longer) exist.";
      }
      WsfLocalTrack* trackPtr = platPtr->GetTrackManager().FindTrack(aOptions.GetLocalTrackId());
      if (trackPtr == nullptr)
      {
         retval = "Track does not exist for provided target track id";
      }
   }
   else if (aOptions.HasLibrationPointTarget())
   {
      if (!wsf::space::LibrationPoint::ValidLibrationPoint(aOptions.GetLP_System(), aOptions.GetLP_Point()))
      {
         retval = "Invalid provided target libration point.";
      }
   }
   else if (!aOptions.HasKinematicStateTarget())
   {
      retval = "Unknown target selection.";
   }
   return retval;
}

} // namespace

namespace wsf
{
namespace space
{
namespace script
{
OrbitalEvent::OrbitalEvent(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptObjectClass(aClassName, aScriptTypesPtr)
{
   AddClassName("WsfOrbitalEvent");

   AddMethod(ut::make_unique<Condition>());
   AddMethod(ut::make_unique<ConditionType>());
   AddMethod(ut::make_unique<Duration>());
   AddMethod(ut::make_unique<EvaluationTime>());
   AddMethod(ut::make_unique<IsComplete>());
   AddMethod(ut::make_unique<IsFinite>());
   AddMethod(ut::make_unique<EventType>());
   AddMethod(ut::make_unique<ManeuverType>());
   AddMethod(ut::make_unique<OrbitNumber>());
   AddMethod(ut::make_unique<Radius>());
   AddMethod(ut::make_unique<RelativeTime>());
   AddMethod(ut::make_unique<SetCondition>());
   AddMethod(ut::make_unique<SetDuration>());
   AddMethod(ut::make_unique<SetFinite>());
   AddMethod(ut::make_unique<SetUpdateInterval>());
   AddMethod(ut::make_unique<StartTime>());
}

// virtual
void* OrbitalEvent::Clone(void* aObjectPtr)
{
   return static_cast<WsfOrbitalEvent*>(aObjectPtr)->Clone();
}

// virtual
void OrbitalEvent::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfOrbitalEvent*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEvent, WsfOrbitalEvent, IsComplete, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsComplete());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEvent, WsfOrbitalEvent, EventType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetType());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEvent, WsfOrbitalEvent, ManeuverType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetType());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEvent, WsfOrbitalEvent, IsFinite, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsFinite());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEvent, WsfOrbitalEvent, Condition, 0, "WsfOrbitalEventCondition", "")
{
   auto conditionPtr = ut::clone(aObjectPtr->GetCondition());
   aReturnVal.SetPointer(new UtScriptRef(conditionPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEvent, WsfOrbitalEvent, ConditionType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetConditionString());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEvent, WsfOrbitalEvent, SetCondition, 1, "void", "WsfOrbitalEventCondition")
{
   auto conditionPtr = static_cast<wsf::space::OrbitalPropagatorCondition*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetCondition(ut::clone(conditionPtr));
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEvent, WsfOrbitalEvent, RelativeTime, 0, "double", "")
{
   double relativeTime{0.0};
   if (aObjectPtr->GetCondition().GetType() == wsf::space::RelativeTimeCondition::cTYPE)
   {
      const auto& cond = dynamic_cast<const wsf::space::RelativeTimeCondition&>(aObjectPtr->GetCondition());
      relativeTime     = cond.GetOffsetTime().GetAsUnit(UtUnitTime::cSECONDS);
   }
   aReturnVal.SetDouble(relativeTime);
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEvent, WsfOrbitalEvent, OrbitNumber, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetCondition().GetOrbitNumber());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEvent, WsfOrbitalEvent, Duration, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetDuration().GetAsUnit(UtUnitTime::cSECONDS));
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEvent, WsfOrbitalEvent, SetDuration, 1, "void", "double")
{
   double duration = aVarArgs[0].GetDouble();
   if (duration > 0.0)
   {
      aObjectPtr->SetDuration(UtTimeValue{aVarArgs[0].GetDouble(), UtUnitTime::cSECONDS});
   }
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEvent, WsfOrbitalEvent, SetFinite, 1, "void", "bool")
{
   aObjectPtr->SetFinite(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEvent, WsfOrbitalEvent, SetUpdateInterval, 1, "void", "double")
{
   aObjectPtr->SetUpdateInterval(UtTimeValue{aVarArgs[0].GetDouble(), UtUnitTime::cSECONDS});
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEvent, WsfOrbitalEvent, Radius, 0, "double", "")
{
   double radius{0.0};
   try
   {
      const auto& cond = dynamic_cast<const wsf::space::RadiusCondition&>(aObjectPtr->GetCondition());
      radius           = cond.GetRadius();
   }
   catch (std::bad_cast&)
   {
      // Nothing.
   }
   aReturnVal.SetDouble(radius);
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEvent, WsfOrbitalEvent, StartTime, 0, "Calendar", "")
{
   UtCalendar* calendarPtr = new UtCalendar(aObjectPtr->GetStartTime());
   aReturnVal.SetPointer(new UtScriptRef(calendarPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEvent, WsfOrbitalEvent, EvaluationTime, 0, "Calendar", "")
{
   UtCalendar* calendarPtr = new UtCalendar(aObjectPtr->GetEvaluationTime());
   aReturnVal.SetPointer(new UtScriptRef(calendarPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

CompoundManeuver::CompoundManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : OrbitalManeuver(aClassName, aScriptTypesPtr)
{
   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<AddManeuver>());
}

UT_DEFINE_SCRIPT_METHOD(CompoundManeuver, WsfOrbitalManeuvers::CompoundManeuver, Construct, 0, "WsfCompoundManeuver", "")
{
   auto maneuver = ut::make_unique<WsfOrbitalManeuvers::CompoundManeuver>(SIMULATION->GetScenario());
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(CompoundManeuver, WsfOrbitalManeuvers::CompoundManeuver, AddManeuver, 1, "void", "WsfOrbitalManeuver")
{
   auto objPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfOrbitalEvent>();
   aObjectPtr->AddMissionEvent(ut::clone(objPtr));
}

ChangeEccentricity::ChangeEccentricity(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : OrbitalManeuver(aClassName, aScriptTypesPtr)
{
   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(ChangeEccentricity,
                        WsfOrbitalManeuvers::ChangeEccentricity,
                        Construct,
                        2,
                        "WsfChangeEccentricity",
                        "WsfOrbitalEventCondition, double")
{
   auto eccentricity = aVarArgs[1].GetDouble();
   auto maneuver     = ut::make_unique<WsfOrbitalManeuvers::ChangeEccentricity>(*SCENARIO, eccentricity);
   auto conditionPtr = static_cast<wsf::space::OrbitalPropagatorCondition*>(aVarArgs[0].GetPointer()->GetAppObject());
   maneuver->SetCondition(ut::clone(conditionPtr));
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

Circularize::Circularize(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : OrbitalManeuver(aClassName, aScriptTypesPtr)
{
   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(Circularize, WsfOrbitalManeuvers::Circularize, Construct, 1, "WsfCircularize", "WsfOrbitalEventCondition")
{
   auto maneuver     = ut::make_unique<WsfOrbitalManeuvers::Circularize>(*SCENARIO);
   auto conditionPtr = static_cast<wsf::space::OrbitalPropagatorCondition*>(aVarArgs[0].GetPointer()->GetAppObject());
   maneuver->SetCondition(ut::clone(conditionPtr));
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

ChangeInclination::ChangeInclination(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : OrbitalManeuver(aClassName, aScriptTypesPtr)
{
   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(ChangeInclination,
                        WsfOrbitalManeuvers::ChangeInclination,
                        Construct,
                        2,
                        "WsfChangeInclination",
                        "WsfOrbitalEventCondition, double")
{
   auto inclination = aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG;
   if ((inclination < 0.0) || (inclination > UtMath::cPI))
   {
      UT_SCRIPT_ABORT("Invalid inclination value.");
   }
   auto maneuver =
      ut::make_unique<WsfOrbitalManeuvers::ChangeInclination>(*SCENARIO, UtAngleValue{inclination, UtUnitAngle::cRADIANS});
   auto conditionPtr = static_cast<wsf::space::OrbitalPropagatorCondition*>(aVarArgs[0].GetPointer()->GetAppObject());
   maneuver->SetCondition(ut::clone(conditionPtr));
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

ChangeRAAN::ChangeRAAN(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : OrbitalManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfChangeRAAN");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<RAAN>());
}

UT_DEFINE_SCRIPT_METHOD(ChangeRAAN, WsfOrbitalManeuvers::ChangeRAAN, Construct, 2, "WsfChangeRAAN", "WsfOrbitalEventCondition, double")
{
   auto raan = aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG;
   if ((raan < 0.0) || (raan >= UtMath::cTWO_PI))
   {
      UT_SCRIPT_ABORT("Invalid RAAN value.");
   }
   auto maneuver = ut::make_unique<WsfOrbitalManeuvers::ChangeRAAN>(*SCENARIO, UtAngleValue{raan, UtUnitAngle::cRADIANS});
   auto conditionPtr = static_cast<wsf::space::OrbitalPropagatorCondition*>(aVarArgs[0].GetPointer()->GetAppObject());
   maneuver->SetCondition(ut::clone(conditionPtr));
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(ChangeRAAN, WsfOrbitalManeuvers::ChangeRAAN, RAAN, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRAAN().GetAsUnit(UtUnitAngle::cDEGREES));
}

ChangeRAAN_Inclination::ChangeRAAN_Inclination(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : OrbitalManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfChangeRAAN_Inclination");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<RAAN>());
   AddMethod(ut::make_unique<Inclination>());
}

UT_DEFINE_SCRIPT_METHOD(ChangeRAAN_Inclination,
                        WsfOrbitalManeuvers::ChangeRAAN_Inclination,
                        Construct,
                        3,
                        "WsfChangeRAAN_Inclination",
                        "WsfOrbitalEventCondition, double, double")
{
   auto raan = aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG;
   if ((raan < 0.0) || (raan > UtMath::cTWO_PI))
   {
      UT_SCRIPT_ABORT("Invalid RAAN value.");
   }
   auto inclination = aVarArgs[2].GetDouble() * UtMath::cRAD_PER_DEG;
   if ((inclination < 0.0) || (inclination > UtMath::cPI))
   {
      UT_SCRIPT_ABORT("Invalid inclination value.");
   }
   auto maneuver =
      ut::make_unique<WsfOrbitalManeuvers::ChangeRAAN_Inclination>(*SCENARIO,
                                                                   UtAngleValue{raan, UtUnitAngle::cRADIANS},
                                                                   UtAngleValue{inclination, UtUnitAngle::cRADIANS});
   auto conditionPtr = static_cast<wsf::space::OrbitalPropagatorCondition*>(aVarArgs[0].GetPointer()->GetAppObject());
   maneuver->SetCondition(ut::clone(conditionPtr));
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(ChangeRAAN_Inclination, WsfOrbitalManeuvers::ChangeRAAN_Inclination, RAAN, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRAAN().GetAsUnit(UtUnitAngle::cDEGREES));
}

UT_DEFINE_SCRIPT_METHOD(ChangeRAAN_Inclination, WsfOrbitalManeuvers::ChangeRAAN_Inclination, Inclination, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetInclination().GetAsUnit(UtUnitAngle::cDEGREES));
}

DeltaV::DeltaV(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : OrbitalManeuver(aClassName, aScriptTypesPtr)
{
   AddStaticMethod(ut::make_unique<Construct_Deprecated>("Construct")); // DEPRECATED
   AddStaticMethod(ut::make_unique<Construct>("Construct"));
   AddMethod(ut::make_unique<Frame>());
   AddMethod(ut::make_unique<DeltaV_1>("DeltaV"));
}

UT_DEFINE_SCRIPT_METHOD(DeltaV, WsfOrbitalManeuvers::DeltaV, Construct_Deprecated, 2, "WsfDeltaV_Maneuver", "WsfOrbitalEventCondition, Vec3")
{
   auto deltavPtr = static_cast<UtVec3d*>(aVarArgs[1].GetPointer()->GetAppObject());
   auto maneuver =
      ut::make_unique<WsfOrbitalManeuvers::DeltaV>(*SCENARIO, *deltavPtr, wsf::space::OrbitalReferenceFrame::cINERTIAL);
   auto conditionPtr = static_cast<wsf::space::OrbitalPropagatorCondition*>(aVarArgs[0].GetPointer()->GetAppObject());
   maneuver->SetCondition(ut::clone(conditionPtr));
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
   auto msg = ut::log::warning() << "Script method is deprecated.";
   msg.AddNote() << "Deprecated method: WsfDeltaV_Maneuver.Construct(WsfOrbitalEventCondtion, Vec3)";
   msg.AddNote() << "Please see WsfDeltaV_Maneuver.Construct(WsfOrbitalEventCondtion, Vec3, WsfOrbitalReferenceFrame)";
}

UT_DEFINE_SCRIPT_METHOD(DeltaV,
                        WsfOrbitalManeuvers::DeltaV,
                        Construct,
                        3,
                        "WsfDeltaV_Maneuver",
                        "WsfOrbitalEventCondition, Vec3, WsfOrbitalReferenceFrame")
{
   auto conditionPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();
   auto deltavPtr    = aVarArgs[1].GetPointer()->GetAppObject<UtVec3d>();
   auto framePtr     = aVarArgs[2].GetPointer()->GetAppObject<wsf::space::OrbitalReferenceFrame>();
   if (!WsfOrbitalManeuvers::DeltaV::IsFrameSupported(*framePtr))
   {
      UT_SCRIPT_ABORT("Unsupported orbital reference frame for WsfDeltaV_Maneuver.");
   }
   auto maneuver = ut::make_unique<WsfOrbitalManeuvers::DeltaV>(*SCENARIO, *deltavPtr, *framePtr);
   maneuver->SetCondition(ut::clone(conditionPtr));
   aReturnVal.SetPointer(new UtScriptRef{maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

UT_DEFINE_SCRIPT_METHOD(DeltaV, WsfOrbitalManeuvers::DeltaV, Frame, 0, "WsfOrbitalReferenceFrame", "")
{
   auto framePtr = ut::make_unique<wsf::space::OrbitalReferenceFrame>(aObjectPtr->GetDeltaV_Frame());
   aReturnVal.SetPointer(new UtScriptRef{framePtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

UT_DEFINE_SCRIPT_METHOD(DeltaV, WsfOrbitalManeuvers::DeltaV, DeltaV_1, 0, "Vec3", "")
{
   aReturnVal.SetPointer(UtScriptVec3::Create(aObjectPtr->GetConfiguredDeltaV()));
}

Drift::Drift(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : OrbitalManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfDriftManeuver");

   AddStaticMethod(ut::make_unique<Construct_1>("Construct"));
   AddStaticMethod(ut::make_unique<Construct_2>("Construct"));
   AddStaticMethod(ut::make_unique<ConstructCost>("Construct"));

   AddMethod(ut::make_unique<DriftRate>());
   AddMethod(ut::make_unique<SetDriftRate>());
}

UT_DEFINE_SCRIPT_METHOD(Drift,
                        WsfOrbitalManeuvers::Drift,
                        Construct_1,
                        5,
                        "WsfDriftManeuver",
                        "WsfOrbitalEventCondition, int, double, double, double")
{
   // Process arguments
   auto conditionPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();

   auto               optimizeOption = static_cast<WsfOrbitalManeuvers::OptimizeOption>(aVarArgs[1].GetInt());
   UtTimeValue        maxTime{aVarArgs[2].GetDouble(), UtUnitTime::cSECONDS};
   UtSpeedValue       maxDeltaV{aVarArgs[3].GetDouble(), UtUnitSpeed::cMETERS_PER_SECOND};
   UtAngularRateValue driftRate{aVarArgs[4].GetDouble(), UtUnitAngularRate::cDEGREES_PER_SECOND};

   // Create the return value
   auto maneuver = ut::make_unique<WsfOrbitalManeuvers::Drift>(*SCENARIO, optimizeOption, maxTime, maxDeltaV);
   maneuver->SetCondition(ut::clone(conditionPtr));
   maneuver->SetDriftRate(driftRate);
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Drift,
                        WsfOrbitalManeuvers::Drift,
                        ConstructCost,
                        5,
                        "WsfDriftManeuver",
                        "WsfOrbitalEventCondition, WsfOrbitalOptimizationCost, double, double, double")
{
   // Process arguments
   auto conditionPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();

   auto               costPtr = aVarArgs[1].GetPointer()->GetAppObject<wsf::space::OrbitalTargetingCost>();
   UtTimeValue        maxTime{aVarArgs[2].GetDouble(), UtUnitTime::cSECONDS};
   UtSpeedValue       maxDeltaV{aVarArgs[3].GetDouble(), UtUnitSpeed::cMETERS_PER_SECOND};
   UtAngularRateValue driftRate{aVarArgs[4].GetDouble(), UtUnitAngularRate::cDEGREES_PER_SECOND};

   // Create the return value
   auto maneuver = ut::make_unique<WsfOrbitalManeuvers::Drift>(*SCENARIO, *costPtr, maxTime, maxDeltaV);
   maneuver->SetCondition(ut::clone(conditionPtr));
   maneuver->SetDriftRate(driftRate);
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Drift, WsfOrbitalManeuvers::Drift, Construct_2, 3, "WsfDriftManeuver", "WsfOrbitalEventCondition, double, double")
{
   // Process arguments
   auto conditionPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();

   UtTimeValue        deltaTime{aVarArgs[1].GetDouble(), UtUnitTime::cSECONDS};
   UtAngularRateValue driftRate{aVarArgs[2].GetDouble(), UtUnitAngularRate::cDEGREES_PER_SECOND};

   // Create the return value
   auto maneuver = ut::make_unique<WsfOrbitalManeuvers::Drift>(*SCENARIO, WsfOrbitalManeuvers::cOPTIMIZE_NONE, deltaTime);
   maneuver->SetCondition(ut::clone(conditionPtr));
   maneuver->SetDriftRate(driftRate);
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Drift, WsfOrbitalManeuvers::Drift, DriftRate, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetDriftRate().GetAsUnit(UtUnitAngularRate::cDEGREES_PER_SECOND));
}

UT_DEFINE_SCRIPT_METHOD(Drift, WsfOrbitalManeuvers::Drift, SetDriftRate, 1, "void", "double")
{
   aObjectPtr->SetDriftRate(UtAngularRateValue{aVarArgs[0].GetDouble(), UtUnitAngularRate::cDEGREES_PER_SECOND});
}

Tangent::Tangent(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : OrbitalManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfTangentManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(Tangent, WsfOrbitalManeuvers::Tangent, Construct, 2, "WsfTangentManeuver", "WsfOrbitalEventCondition, double")
{
   auto deltaV = aVarArgs[1].GetDouble();
   auto maneuver =
      ut::make_unique<WsfOrbitalManeuvers::Tangent>(*SCENARIO, UtSpeedValue{deltaV, UtUnitSpeed::cMETERS_PER_SECOND});
   auto conditionPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();
   maneuver->SetCondition(ut::clone(conditionPtr));
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

Normal::Normal(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : OrbitalManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfNormalManeuver");

   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(Normal, WsfOrbitalManeuvers::Normal, Construct, 2, "WsfNormalManeuver", "WsfOrbitalEventCondition, double")
{
   auto deltaV = aVarArgs[1].GetDouble();
   auto maneuver =
      ut::make_unique<WsfOrbitalManeuvers::Normal>(*SCENARIO, UtSpeedValue{deltaV, UtUnitSpeed::cMETERS_PER_SECOND});
   auto conditionPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();
   maneuver->SetCondition(ut::clone(conditionPtr));
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

HohmannTransfer::HohmannTransfer(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : OrbitalManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfHohmannTransfer");

   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(HohmannTransfer,
                        WsfOrbitalManeuvers::HohmannTransfer,
                        Construct,
                        2,
                        "WsfHohmannTransfer",
                        "WsfOrbitalEventCondition, double")
{
   auto radius       = aVarArgs[1].GetDouble();
   auto maneuver     = ut::make_unique<WsfOrbitalManeuvers::HohmannTransfer>(SIMULATION->GetScenario(),
                                                                         UtLengthValue{radius, UtUnitLength::cMETERS});
   auto conditionPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();
   maneuver->SetCondition(ut::clone(conditionPtr));
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

ChangeSemiMajorAxis::ChangeSemiMajorAxis(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : OrbitalManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfChangeSemiMajorAxis");

   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(ChangeSemiMajorAxis,
                        WsfOrbitalManeuvers::ChangeSemiMajorAxis,
                        Construct,
                        2,
                        "WsfChangeSemiMajorAxis",
                        "WsfOrbitalEventCondition, double")
{
   auto sma = aVarArgs[1].GetDouble();
   if (sma < 0)
   {
      UT_SCRIPT_ABORT("Invalid final semi-major axis value");
   }
   auto maneuver =
      ut::make_unique<WsfOrbitalManeuvers::ChangeSemiMajorAxis>(*SCENARIO, UtLengthValue{sma, UtUnitLength::cMETERS});
   auto conditionPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();
   maneuver->SetCondition(ut::clone(conditionPtr));
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

MatchVelocity::MatchVelocity(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : OrbitalManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfMatchVelocity");

   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(MatchVelocity,
                        WsfOrbitalManeuvers::MatchVelocity,
                        Construct,
                        2,
                        "WsfMatchVelocity",
                        "WsfOrbitalEventCondition, WsfTrackId")
{
   auto maneuver     = ut::make_unique<WsfOrbitalManeuvers::MatchVelocity>(*SCENARIO);
   auto conditionPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();
   maneuver->SetCondition(ut::clone(conditionPtr));
   auto trackIdPtr = static_cast<WsfTrackId*>(aVarArgs[1].GetPointer()->GetAppObject());
   maneuver->SetLocalTrackId(*trackIdPtr);
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

NaturalMotionCircumnavigation::NaturalMotionCircumnavigation(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : OrbitalManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfNaturalMotionCircumnavigation");

   AddStaticMethod(ut::make_unique<Construct_1>("Construct"));
   AddStaticMethod(ut::make_unique<Construct_2>("Construct"));
   AddStaticMethod(ut::make_unique<ConstructCost>("Construct"));

   AddMethod(ut::make_unique<OrbitSize>());
   AddMethod(ut::make_unique<OrbitPhase>());
   AddMethod(ut::make_unique<OutOfPlaneAmplitude>());
   AddMethod(ut::make_unique<OutOfPlanePhase>());
   AddMethod(ut::make_unique<SetOrbitSize>());
   AddMethod(ut::make_unique<SetOrbitPhase>());
   AddMethod(ut::make_unique<SetOutOfPlaneAmplitude>());
   AddMethod(ut::make_unique<SetOutOfPlanePhase>());
}

UT_DEFINE_SCRIPT_METHOD(NaturalMotionCircumnavigation,
                        WsfOrbitalManeuvers::NaturalMotionCircumnavigation,
                        Construct_1,
                        6,
                        "WsfNaturalMotionCircumnavigation",
                        "WsfOrbitalEventCondition, WsfTargetPoint, int, double, double, double")
{
   // Process arguments
   auto conditionPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();

   auto targetPointPtr = aVarArgs[1].GetPointer()->GetAppObject<wsf::space::TargetPointOptions>();
   if (!targetPointPtr->HasTrackTarget())
   {
      UT_SCRIPT_ABORT("Track targets are the only supported targets for the natural motion cicumnavigation maneuver.");
   }
   if (targetPointPtr->HasPositionOffset() || targetPointPtr->HasVelocityOffset())
   {
      UT_SCRIPT_ABORT("Target offsets are not supported by the natural motion circumnavigation maneuvers.");
   }
   if (targetPointPtr->HasOffsetTime())
   {
      UT_SCRIPT_ABORT("Offset time is not supported by the natural motion circumnavigation maneuver.");
   }
   if (targetPointPtr->HasLagTime())
   {
      UT_SCRIPT_ABORT("Lag time is not supported by the natural motion circumnavigation maneuver.");
   }
   auto error = ValidateScriptTargetOptionsInputArgument(*targetPointPtr, aContext);
   if (!error.empty())
   {
      UT_SCRIPT_ABORT(error);
   }

   auto         optimizeOption = static_cast<WsfOrbitalManeuvers::OptimizeOption>(aVarArgs[2].GetInt());
   UtTimeValue  maxTime{aVarArgs[3].GetDouble(), UtUnitTime::cSECONDS};
   UtSpeedValue maxDeltaV{aVarArgs[4].GetDouble(), UtUnitSpeed::cMETERS_PER_SECOND};

   UtLengthValue orbitSize{aVarArgs[5].GetDouble(), UtUnitLength::cMETERS};
   if (orbitSize <= 0.0)
   {
      UT_SCRIPT_ABORT("Orbit size must be positive.");
   }

   // Create the return value
   auto maneuver = ut::make_unique<WsfOrbitalManeuvers::NaturalMotionCircumnavigation>(*SCENARIO,
                                                                                       *targetPointPtr,
                                                                                       optimizeOption,
                                                                                       maxTime,
                                                                                       maxDeltaV);
   maneuver->SetCondition(ut::clone(conditionPtr));
   maneuver->SetOrbitSize(orbitSize);
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(NaturalMotionCircumnavigation,
                        WsfOrbitalManeuvers::NaturalMotionCircumnavigation,
                        ConstructCost,
                        6,
                        "WsfNaturalMotionCircumnavigation",
                        "WsfOrbitalEventCondition, WsfTargetPoint, WsfOrbitalOptimizationCost, double, double, double")
{
   // Process arguments
   auto conditionPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();

   auto targetPointPtr = aVarArgs[1].GetPointer()->GetAppObject<wsf::space::TargetPointOptions>();
   if (!targetPointPtr->HasTrackTarget())
   {
      UT_SCRIPT_ABORT("Track targets are the only supported targets for the natural motion cicumnavigation maneuver.");
   }
   if (targetPointPtr->HasPositionOffset() || targetPointPtr->HasVelocityOffset())
   {
      UT_SCRIPT_ABORT("Target offsets are not supported by the natural motion circumnavigation maneuvers.");
   }
   if (targetPointPtr->HasOffsetTime())
   {
      UT_SCRIPT_ABORT("Offset time is not supported by the natural motion circumnavigation maneuver.");
   }
   if (targetPointPtr->HasLagTime())
   {
      UT_SCRIPT_ABORT("Lag time is not supported by the natural motion circumnavigation maneuver.");
   }
   auto error = ValidateScriptTargetOptionsInputArgument(*targetPointPtr, aContext);
   if (!error.empty())
   {
      UT_SCRIPT_ABORT(error);
   }

   auto         costPtr = aVarArgs[2].GetPointer()->GetAppObject<wsf::space::OrbitalTargetingCost>();
   UtTimeValue  maxTime{aVarArgs[3].GetDouble(), UtUnitTime::cSECONDS};
   UtSpeedValue maxDeltaV{aVarArgs[4].GetDouble(), UtUnitSpeed::cMETERS_PER_SECOND};

   UtLengthValue orbitSize{aVarArgs[5].GetDouble(), UtUnitLength::cMETERS};
   if (orbitSize <= 0.0)
   {
      UT_SCRIPT_ABORT("Orbit size must be positive.");
   }

   // Create the return value
   auto maneuver = ut::make_unique<WsfOrbitalManeuvers::NaturalMotionCircumnavigation>(*SCENARIO,
                                                                                       *targetPointPtr,
                                                                                       *costPtr,
                                                                                       maxTime,
                                                                                       maxDeltaV);
   maneuver->SetCondition(ut::clone(conditionPtr));
   maneuver->SetOrbitSize(orbitSize);
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(NaturalMotionCircumnavigation,
                        WsfOrbitalManeuvers::NaturalMotionCircumnavigation,
                        Construct_2,
                        4,
                        "WsfNaturalMotionCircumnavigation",
                        "WsfOrbitalEventCondition, WsfTargetPoint, double, double")
{
   // Process arguments
   auto conditionPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();

   auto targetPointPtr = aVarArgs[1].GetPointer()->GetAppObject<wsf::space::TargetPointOptions>();
   if (!targetPointPtr->HasTrackTarget())
   {
      UT_SCRIPT_ABORT("Track targets are the only supported targets for the natural motion cicumnavigation maneuver.");
   }
   if (targetPointPtr->HasPositionOffset() || targetPointPtr->HasVelocityOffset())
   {
      UT_SCRIPT_ABORT("Target offsets are not supported by the natural motion circumnavigation maneuvers.");
   }
   if (targetPointPtr->HasOffsetTime())
   {
      UT_SCRIPT_ABORT("Offset time is not supported by the natural motion circumnavigation maneuver.");
   }
   if (targetPointPtr->HasLagTime())
   {
      UT_SCRIPT_ABORT("Lag time is not supported by the natural motion circumnavigation maneuver.");
   }
   auto error = ValidateScriptTargetOptionsInputArgument(*targetPointPtr, aContext);
   if (!error.empty())
   {
      UT_SCRIPT_ABORT(error);
   }

   UtTimeValue deltaTime{aVarArgs[2].GetDouble(), UtUnitTime::cSECONDS};

   UtLengthValue orbitSize{aVarArgs[3].GetDouble(), UtUnitLength::cMETERS};
   if (orbitSize <= 0.0)
   {
      UT_SCRIPT_ABORT("Orbit size must be positive.");
   }

   // Create the return value
   auto maneuver = ut::make_unique<WsfOrbitalManeuvers::NaturalMotionCircumnavigation>(*SCENARIO,
                                                                                       *targetPointPtr,
                                                                                       WsfOrbitalManeuvers::cOPTIMIZE_NONE,
                                                                                       deltaTime);
   maneuver->SetCondition(ut::clone(conditionPtr));
   maneuver->SetOrbitSize(orbitSize);
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(NaturalMotionCircumnavigation,
                        WsfOrbitalManeuvers::NaturalMotionCircumnavigation,
                        OrbitSize,
                        0,
                        "double",
                        "")
{
   aReturnVal.SetDouble(aObjectPtr->GetOrbitSize().GetAsUnit(UtUnitLength::cMETERS));
}

UT_DEFINE_SCRIPT_METHOD(NaturalMotionCircumnavigation,
                        WsfOrbitalManeuvers::NaturalMotionCircumnavigation,
                        OrbitPhase,
                        0,
                        "double",
                        "")
{
   aReturnVal.SetDouble(aObjectPtr->GetOrbitPhase().GetAsUnit(UtUnitAngle::cDEGREES));
}

UT_DEFINE_SCRIPT_METHOD(NaturalMotionCircumnavigation,
                        WsfOrbitalManeuvers::NaturalMotionCircumnavigation,
                        OutOfPlaneAmplitude,
                        0,
                        "double",
                        "")
{
   aReturnVal.SetDouble(aObjectPtr->GetOutOfPlaneAmplitude().GetAsUnit(UtUnitLength::cMETERS));
}

UT_DEFINE_SCRIPT_METHOD(NaturalMotionCircumnavigation,
                        WsfOrbitalManeuvers::NaturalMotionCircumnavigation,
                        OutOfPlanePhase,
                        0,
                        "double",
                        "")
{
   aReturnVal.SetDouble(aObjectPtr->GetOutOfPlanePhase().GetAsUnit(UtUnitAngle::cDEGREES));
}

UT_DEFINE_SCRIPT_METHOD(NaturalMotionCircumnavigation,
                        WsfOrbitalManeuvers::NaturalMotionCircumnavigation,
                        SetOrbitSize,
                        1,
                        "void",
                        "double")
{
   double size = aVarArgs[0].GetDouble();
   if (size <= 0.0)
   {
      UT_SCRIPT_ABORT("Orbit size must be positive");
   }
   aObjectPtr->SetOrbitSize(UtLengthValue{size, UtUnitLength::cMETERS});
}

UT_DEFINE_SCRIPT_METHOD(NaturalMotionCircumnavigation,
                        WsfOrbitalManeuvers::NaturalMotionCircumnavigation,
                        SetOrbitPhase,
                        1,
                        "void",
                        "double")
{
   double phase = aVarArgs[0].GetDouble();
   aObjectPtr->SetOrbitPhase(UtAngleValue{phase, UtUnitAngle::cDEGREES});
}

UT_DEFINE_SCRIPT_METHOD(NaturalMotionCircumnavigation,
                        WsfOrbitalManeuvers::NaturalMotionCircumnavigation,
                        SetOutOfPlaneAmplitude,
                        1,
                        "void",
                        "double")
{
   double amplitude = aVarArgs[0].GetDouble();
   if (amplitude < 0.0)
   {
      UT_SCRIPT_ABORT("Out-of-plane amplitude must be non-negative.");
   }
   aObjectPtr->SetOutOfPlaneAmplitude(UtLengthValue{amplitude, UtUnitLength::cMETERS});
}

UT_DEFINE_SCRIPT_METHOD(NaturalMotionCircumnavigation,
                        WsfOrbitalManeuvers::NaturalMotionCircumnavigation,
                        SetOutOfPlanePhase,
                        1,
                        "void",
                        "double")
{
   double phase = aVarArgs[0].GetDouble();
   aObjectPtr->SetOutOfPlanePhase(UtAngleValue{phase, UtUnitAngle::cDEGREES});
}

Intercept::Intercept(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : OrbitalManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfInterceptManeuver");

   AddStaticMethod(ut::make_unique<Construct_1>("Construct"));
   AddStaticMethod(ut::make_unique<Construct_2>("Construct"));
   AddStaticMethod(ut::make_unique<ConstructCost>("Construct"));

   AddMethod(ut::make_unique<TargetPoint>());

   AddMethod(ut::make_unique<Tolerance>());
   AddMethod(ut::make_unique<SetTolerance>());
}

UT_DEFINE_SCRIPT_METHOD(Intercept, WsfOrbitalManeuvers::Intercept, TargetPoint, 0, "WsfTargetPoint", "")
{
   auto optionsPtr = ut::make_unique<wsf::space::TargetPointOptions>(aObjectPtr->GetTargetPointOptions());
   aReturnVal.SetPointer(new UtScriptRef{optionsPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

UT_DEFINE_SCRIPT_METHOD(Intercept, WsfOrbitalManeuvers::Intercept, Tolerance, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetTolerance());
}

UT_DEFINE_SCRIPT_METHOD(Intercept, WsfOrbitalManeuvers::Intercept, SetTolerance, 1, "void", "double")
{
   double tol = aVarArgs[0].GetDouble();
   if (tol > 0.0)
   {
      aObjectPtr->SetTolerance(tol);
   }
   else
   {
      UT_SCRIPT_ABORT("Tolerance must be positive.");
   }
}

UT_DEFINE_SCRIPT_METHOD(Intercept,
                        WsfOrbitalManeuvers::Intercept,
                        Construct_1,
                        5,
                        "WsfInterceptManeuver",
                        "WsfOrbitalEventCondition, WsfTargetPoint, int, double, double")
{
   // Process arguments
   auto conditionPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();

   auto targetPointPtr = aVarArgs[1].GetPointer()->GetAppObject<wsf::space::TargetPointOptions>();
   auto error          = ValidateScriptTargetOptionsInputArgument(*targetPointPtr, aContext);
   if (!error.empty())
   {
      UT_SCRIPT_ABORT(error);
   }

   auto         optimizeOption = static_cast<WsfOrbitalManeuvers::OptimizeOption>(aVarArgs[2].GetInt());
   UtTimeValue  maxTime{aVarArgs[3].GetDouble(), UtUnitTime::cSECONDS};
   UtSpeedValue maxDeltaV{aVarArgs[4].GetDouble(), UtUnitSpeed::cMETERS_PER_SECOND};

   // Create the return value
   auto maneuver =
      ut::make_unique<WsfOrbitalManeuvers::Intercept>(*SCENARIO, *targetPointPtr, optimizeOption, maxTime, maxDeltaV);
   maneuver->SetCondition(ut::clone(conditionPtr));
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Intercept,
                        WsfOrbitalManeuvers::Intercept,
                        ConstructCost,
                        5,
                        "WsfInterceptManeuver",
                        "WsfOrbitalEventCondition, WsfTargetPoint, WsfOrbitalOptimizationCost, double, double")
{
   // Process arguments
   auto conditionPtr   = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();
   auto targetPointPtr = aVarArgs[1].GetPointer()->GetAppObject<wsf::space::TargetPointOptions>();
   auto error          = ValidateScriptTargetOptionsInputArgument(*targetPointPtr, aContext);
   if (!error.empty())
   {
      UT_SCRIPT_ABORT(error);
   }
   auto         costPtr = aVarArgs[2].GetPointer()->GetAppObject<wsf::space::OrbitalTargetingCost>();
   UtTimeValue  maxTime{aVarArgs[3].GetDouble(), UtUnitTime::cSECONDS};
   UtSpeedValue maxDeltaV{aVarArgs[4].GetDouble(), UtUnitSpeed::cMETERS_PER_SECOND};

   // Create the return value
   auto maneuver =
      ut::make_unique<WsfOrbitalManeuvers::Intercept>(*SCENARIO, *targetPointPtr, *costPtr, maxTime, maxDeltaV);
   maneuver->SetCondition(ut::clone(conditionPtr));
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Intercept,
                        WsfOrbitalManeuvers::Intercept,
                        Construct_2,
                        3,
                        "WsfInterceptManeuver",
                        "WsfOrbitalEventCondition, WsfTargetPoint, double")
{
   // Process arguments
   auto conditionPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();

   auto targetPointPtr = aVarArgs[1].GetPointer()->GetAppObject<wsf::space::TargetPointOptions>();
   auto error          = ValidateScriptTargetOptionsInputArgument(*targetPointPtr, aContext);
   if (!error.empty())
   {
      UT_SCRIPT_ABORT(error);
   }

   UtTimeValue deltaTime{aVarArgs[2].GetDouble(), UtUnitTime::cSECONDS};

   // Create the return value
   auto maneuver = ut::make_unique<WsfOrbitalManeuvers::Intercept>(*SCENARIO,
                                                                   *targetPointPtr,
                                                                   WsfOrbitalManeuvers::cOPTIMIZE_NONE,
                                                                   deltaTime);
   maneuver->SetCondition(ut::clone(conditionPtr));
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

Target::Target(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : OrbitalManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfTargetManeuver");

   AddStaticMethod(ut::make_unique<Construct_1>("Construct"));
   AddStaticMethod(ut::make_unique<Construct_2>("Construct"));
   AddStaticMethod(ut::make_unique<ConstructCost>("Construct"));

   AddMethod(ut::make_unique<TargetPoint>());

   AddMethod(ut::make_unique<Tolerance>());
   AddMethod(ut::make_unique<SetTolerance>());
}

UT_DEFINE_SCRIPT_METHOD(Target, WsfOrbitalManeuvers::Target, TargetPoint, 0, "WsfTargetPoint", "")
{
   auto optionsPtr = ut::make_unique<wsf::space::TargetPointOptions>(aObjectPtr->GetTargetPointOptions());
   aReturnVal.SetPointer(new UtScriptRef{optionsPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

UT_DEFINE_SCRIPT_METHOD(Target, WsfOrbitalManeuvers::Target, Tolerance, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetTolerance());
}

UT_DEFINE_SCRIPT_METHOD(Target, WsfOrbitalManeuvers::Target, SetTolerance, 1, "void", "double")
{
   double tol = aVarArgs[0].GetDouble();
   if (tol > 0.0)
   {
      aObjectPtr->SetTolerance(tol);
   }
   else
   {
      UT_SCRIPT_ABORT("Tolerance must be positive.");
   }
}

UT_DEFINE_SCRIPT_METHOD(Target,
                        WsfOrbitalManeuvers::Target,
                        Construct_1,
                        5,
                        "WsfTargetManeuver",
                        "WsfOrbitalEventCondition, WsfTargetPoint, int, double, double")
{
   // Process arguments
   auto conditionPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();

   auto targetPointPtr = aVarArgs[1].GetPointer()->GetAppObject<wsf::space::TargetPointOptions>();
   auto error          = ValidateScriptTargetOptionsInputArgument(*targetPointPtr, aContext);
   if (!error.empty())
   {
      UT_SCRIPT_ABORT(error);
   }

   auto         optimizeOption = static_cast<WsfOrbitalManeuvers::OptimizeOption>(aVarArgs[2].GetInt());
   UtTimeValue  maxTime{aVarArgs[3].GetDouble(), UtUnitTime::cSECONDS};
   UtSpeedValue maxDeltaV{aVarArgs[4].GetDouble(), UtUnitSpeed::cMETERS_PER_SECOND};

   // Create the return value
   auto maneuver =
      ut::make_unique<WsfOrbitalManeuvers::Target>(*SCENARIO, *targetPointPtr, optimizeOption, maxTime, maxDeltaV);
   maneuver->SetCondition(ut::clone(conditionPtr));
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Target,
                        WsfOrbitalManeuvers::Target,
                        ConstructCost,
                        5,
                        "WsfTargetManeuver",
                        "WsfOrbitalEventCondition, WsfTargetPoint, WsfOrbitalOptimizationCost, double, double")
{
   // Process arguments
   auto conditionPtr   = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();
   auto targetPointPtr = aVarArgs[1].GetPointer()->GetAppObject<wsf::space::TargetPointOptions>();
   auto error          = ValidateScriptTargetOptionsInputArgument(*targetPointPtr, aContext);
   if (!error.empty())
   {
      UT_SCRIPT_ABORT(error);
   }
   auto         costPtr = aVarArgs[2].GetPointer()->GetAppObject<wsf::space::OrbitalTargetingCost>();
   UtTimeValue  maxTime{aVarArgs[3].GetDouble(), UtUnitTime::cSECONDS};
   UtSpeedValue maxDeltaV{aVarArgs[4].GetDouble(), UtUnitSpeed::cMETERS_PER_SECOND};

   // Create the return value
   auto maneuver = ut::make_unique<WsfOrbitalManeuvers::Target>(*SCENARIO, *targetPointPtr, *costPtr, maxTime, maxDeltaV);
   maneuver->SetCondition(ut::clone(conditionPtr));
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Target,
                        WsfOrbitalManeuvers::Target,
                        Construct_2,
                        3,
                        "WsfTargetManeuver",
                        "WsfOrbitalEventCondition, WsfTargetPoint, double")
{
   // Process arguments
   auto conditionPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();

   auto targetPointPtr = aVarArgs[1].GetPointer()->GetAppObject<wsf::space::TargetPointOptions>();
   auto error          = ValidateScriptTargetOptionsInputArgument(*targetPointPtr, aContext);
   if (!error.empty())
   {
      UT_SCRIPT_ABORT(error);
   }

   UtTimeValue deltaTime{aVarArgs[2].GetDouble(), UtUnitTime::cSECONDS};

   // Create the return value
   auto maneuver =
      ut::make_unique<WsfOrbitalManeuvers::Target>(*SCENARIO, *targetPointPtr, WsfOrbitalManeuvers::cOPTIMIZE_NONE, deltaTime);
   maneuver->SetCondition(ut::clone(conditionPtr));
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

Teardrop::Teardrop(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : OrbitalManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfTeardropManeuver");

   AddStaticMethod(ut::make_unique<Construct_1>("Construct"));
   AddStaticMethod(ut::make_unique<Construct_2>("Construct"));
   AddStaticMethod(ut::make_unique<ConstructCost>("Construct"));

   AddMethod(ut::make_unique<Period>());
   AddMethod(ut::make_unique<RadialOffsetAtPOCA>());
   AddMethod(ut::make_unique<TimeToPOCA>());
   AddMethod(ut::make_unique<Repetitions>());
   AddMethod(ut::make_unique<SetRepetitions>());
}

UT_DEFINE_SCRIPT_METHOD(Teardrop,
                        WsfOrbitalManeuvers::Teardrop,
                        Construct_1,
                        8,
                        "WsfTeardropManeuver",
                        "WsfOrbitalEventCondition, WsfTargetPoint, int, double, double, double, double, double")
{
   // Process arguments
   auto conditionPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();

   auto targetPointPtr = aVarArgs[1].GetPointer()->GetAppObject<wsf::space::TargetPointOptions>();
   if (!targetPointPtr->HasTrackTarget())
   {
      UT_SCRIPT_ABORT("Track targets are the only supported targets for the teardrop maneuver.");
   }
   if (targetPointPtr->HasPositionOffset() || targetPointPtr->HasVelocityOffset())
   {
      UT_SCRIPT_ABORT("Target offsets are not supported by the teardrop maneuver.");
   }
   if (targetPointPtr->HasOffsetTime())
   {
      UT_SCRIPT_ABORT("Offset time is not supported by the teardrop maneuver.");
   }
   if (targetPointPtr->HasLagTime())
   {
      UT_SCRIPT_ABORT("Lag time is not supported by the teardrop maneuver.");
   }
   auto error = ValidateScriptTargetOptionsInputArgument(*targetPointPtr, aContext);
   if (!error.empty())
   {
      UT_SCRIPT_ABORT(error);
   }

   auto         optimizeOption = static_cast<WsfOrbitalManeuvers::OptimizeOption>(aVarArgs[2].GetInt());
   UtTimeValue  maxTime{aVarArgs[3].GetDouble(), UtUnitTime::cSECONDS};
   UtSpeedValue maxDeltaV{aVarArgs[4].GetDouble(), UtUnitSpeed::cMETERS_PER_SECOND};

   UtLengthValue rAtPOCA{aVarArgs[5].GetDouble(), UtUnitLength::cMETERS};
   if (rAtPOCA == 0.0)
   {
      UT_SCRIPT_ABORT("R at POCA must be non-zero.");
   }

   UtTimeValue period{aVarArgs[6].GetDouble(), UtUnitTime::cSECONDS};
   if (period <= 0.0)
   {
      UT_SCRIPT_ABORT("Teardrop period must be positive.");
   }

   UtTimeValue timeToPOCA{aVarArgs[7].GetDouble(), UtUnitTime::cSECONDS};
   if (timeToPOCA <= 0.0)
   {
      UT_SCRIPT_ABORT("Time to POCA for teardrop must be positive.");
   }
   else if (timeToPOCA < period / 2.0)
   {
      UT_SCRIPT_ABORT("Time to POCA must be greater than half the teardrop period.");
   }

   // Create the return value
   auto maneuver =
      ut::make_unique<WsfOrbitalManeuvers::Teardrop>(*SCENARIO, *targetPointPtr, optimizeOption, maxTime, maxDeltaV);
   maneuver->SetCondition(ut::clone(conditionPtr));
   maneuver->SetRadialOffsetAtPOCA(rAtPOCA);
   maneuver->SetTeardropPeriod(period);
   maneuver->SetTimeToPOCA(timeToPOCA);
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(
   Teardrop,
   WsfOrbitalManeuvers::Teardrop,
   ConstructCost,
   8,
   "WsfTeardropManeuver",
   "WsfOrbitalEventCondition, WsfTargetPoint, WsfOrbitalOptimizationCost, double, double, double, double, double")
{
   // Process arguments
   auto conditionPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();

   auto targetPointPtr = aVarArgs[1].GetPointer()->GetAppObject<wsf::space::TargetPointOptions>();
   if (!targetPointPtr->HasTrackTarget())
   {
      UT_SCRIPT_ABORT("Track targets are the only supported targets for the teardrop maneuver.");
   }
   if (targetPointPtr->HasPositionOffset() || targetPointPtr->HasVelocityOffset())
   {
      UT_SCRIPT_ABORT("Target offsets are not supported by the teardrop maneuver.");
   }
   if (targetPointPtr->HasOffsetTime())
   {
      UT_SCRIPT_ABORT("Offset time is not supported by the teardrop maneuver.");
   }
   if (targetPointPtr->HasLagTime())
   {
      UT_SCRIPT_ABORT("Lag time is not supported by the teardrop maneuver.");
   }
   auto error = ValidateScriptTargetOptionsInputArgument(*targetPointPtr, aContext);
   if (!error.empty())
   {
      UT_SCRIPT_ABORT(error);
   }

   auto         costPtr = aVarArgs[2].GetPointer()->GetAppObject<wsf::space::OrbitalTargetingCost>();
   UtTimeValue  maxTime{aVarArgs[3].GetDouble(), UtUnitTime::cSECONDS};
   UtSpeedValue maxDeltaV{aVarArgs[4].GetDouble(), UtUnitSpeed::cMETERS_PER_SECOND};

   UtLengthValue rAtPOCA{aVarArgs[5].GetDouble(), UtUnitLength::cMETERS};
   if (rAtPOCA == 0.0)
   {
      UT_SCRIPT_ABORT("R at POCA must be non-zero.");
   }

   UtTimeValue period{aVarArgs[6].GetDouble(), UtUnitTime::cSECONDS};
   if (period <= 0.0)
   {
      UT_SCRIPT_ABORT("Teardrop period must be positive.");
   }

   UtTimeValue timeToPOCA{aVarArgs[7].GetDouble(), UtUnitTime::cSECONDS};
   if (timeToPOCA <= 0.0)
   {
      UT_SCRIPT_ABORT("Time to POCA for teardrop must be positive.");
   }
   else if (timeToPOCA < period / 2.0)
   {
      UT_SCRIPT_ABORT("Time to POCA must be greater than half the teardrop period.");
   }

   // Create the return value
   auto maneuver =
      ut::make_unique<WsfOrbitalManeuvers::Teardrop>(*SCENARIO, *targetPointPtr, *costPtr, maxTime, maxDeltaV);
   maneuver->SetCondition(ut::clone(conditionPtr));
   maneuver->SetRadialOffsetAtPOCA(rAtPOCA);
   maneuver->SetTeardropPeriod(period);
   maneuver->SetTimeToPOCA(timeToPOCA);
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Teardrop,
                        WsfOrbitalManeuvers::Teardrop,
                        Construct_2,
                        6,
                        "WsfTeardropManeuver",
                        "WsfOrbitalEventCondition, WsfTargetPoint, double, double, double, double")
{
   // Process arguments
   auto conditionPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();

   auto targetPointPtr = aVarArgs[1].GetPointer()->GetAppObject<wsf::space::TargetPointOptions>();
   if (!targetPointPtr->HasTrackTarget())
   {
      UT_SCRIPT_ABORT("Track targets are the only supported targets for the teardrop maneuver.");
   }
   if (targetPointPtr->HasPositionOffset() || targetPointPtr->HasVelocityOffset())
   {
      UT_SCRIPT_ABORT("Target offsets are not supported by the teardrop maneuver.");
   }
   if (targetPointPtr->HasOffsetTime())
   {
      UT_SCRIPT_ABORT("Offset time is not supported by the teardrop maneuver.");
   }
   if (targetPointPtr->HasLagTime())
   {
      UT_SCRIPT_ABORT("Lag time is not supported by the teardrop maneuver.");
   }
   auto error = ValidateScriptTargetOptionsInputArgument(*targetPointPtr, aContext);
   if (!error.empty())
   {
      UT_SCRIPT_ABORT(error);
   }

   UtTimeValue deltaTime{aVarArgs[2].GetDouble(), UtUnitTime::cSECONDS};

   UtLengthValue rAtPOCA{aVarArgs[3].GetDouble(), UtUnitLength::cMETERS};
   if (rAtPOCA == 0.0)
   {
      UT_SCRIPT_ABORT("R at POCA must be non-zero.");
   }

   UtTimeValue period{aVarArgs[4].GetDouble(), UtUnitTime::cSECONDS};
   if (period <= 0.0)
   {
      UT_SCRIPT_ABORT("Teardrop period must be positive.");
   }

   UtTimeValue timeToPOCA{aVarArgs[5].GetDouble(), UtUnitTime::cSECONDS};
   if (timeToPOCA <= 0.0)
   {
      UT_SCRIPT_ABORT("Time to POCA for teardrop must be positive.");
   }
   else if (timeToPOCA < period / 2.0)
   {
      UT_SCRIPT_ABORT("Time to POCA must be greater than half the teardrop period.");
   }

   // Create the return value
   auto maneuver = ut::make_unique<WsfOrbitalManeuvers::Teardrop>(*SCENARIO,
                                                                  *targetPointPtr,
                                                                  WsfOrbitalManeuvers::cOPTIMIZE_NONE,
                                                                  deltaTime);
   maneuver->SetCondition(ut::clone(conditionPtr));
   maneuver->SetRadialOffsetAtPOCA(rAtPOCA);
   maneuver->SetTeardropPeriod(period);
   maneuver->SetTimeToPOCA(timeToPOCA);
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Teardrop, WsfOrbitalManeuvers::Teardrop, Period, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetTeardropPeriod().GetAsUnit(UtUnitTime::cSECONDS));
}

UT_DEFINE_SCRIPT_METHOD(Teardrop, WsfOrbitalManeuvers::Teardrop, RadialOffsetAtPOCA, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetRadialOffsetAtPOCA().GetAsUnit(UtUnitLength::cMETERS));
}

UT_DEFINE_SCRIPT_METHOD(Teardrop, WsfOrbitalManeuvers::Teardrop, TimeToPOCA, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetTimeToPOCA().GetAsUnit(UtUnitTime::cSECONDS));
}

UT_DEFINE_SCRIPT_METHOD(Teardrop, WsfOrbitalManeuvers::Teardrop, Repetitions, 0, "int", "")
{
   aReturnVal.SetInt(ut::safe_cast<int>(aObjectPtr->GetRepetitions()));
}

UT_DEFINE_SCRIPT_METHOD(Teardrop, WsfOrbitalManeuvers::Teardrop, SetRepetitions, 1, "void", "int")
{
   try
   {
      unsigned int reps = ut::safe_cast<unsigned int>(aVarArgs[0].GetInt());
      if (reps < 1)
      {
         UT_SCRIPT_ABORT("Input argument out of range. Repetitions must be at least one.");
      }
      aObjectPtr->SetRepetitions(reps);
   }
   catch (std::out_of_range&)
   {
      UT_SCRIPT_ABORT("Input argument is out of range. Repetitions must be positive and not too large.");
   }
}


Rendezvous::Rendezvous(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : OrbitalManeuver(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfRendezvousManeuver");

   AddStaticMethod(ut::make_unique<Construct_1>("Construct"));
   AddStaticMethod(ut::make_unique<Construct_2>("Construct"));
   AddStaticMethod(ut::make_unique<ConstructCost>("Construct"));

   AddMethod(ut::make_unique<TargetPoint>());

   AddMethod(ut::make_unique<Tolerance>());
   AddMethod(ut::make_unique<SetTolerance>());
}

UT_DEFINE_SCRIPT_METHOD(Rendezvous, WsfOrbitalManeuvers::Rendezvous, TargetPoint, 0, "WsfTargetPoint", "")
{
   auto optionsPtr = ut::make_unique<wsf::space::TargetPointOptions>(aObjectPtr->GetTargetPointOptions());
   aReturnVal.SetPointer(new UtScriptRef{optionsPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

UT_DEFINE_SCRIPT_METHOD(Rendezvous, WsfOrbitalManeuvers::Rendezvous, Tolerance, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetTolerance());
}

UT_DEFINE_SCRIPT_METHOD(Rendezvous, WsfOrbitalManeuvers::Rendezvous, SetTolerance, 1, "void", "double")
{
   double tol = aVarArgs[0].GetDouble();
   if (tol > 0.0)
   {
      aObjectPtr->SetTolerance(tol);
   }
   else
   {
      UT_SCRIPT_ABORT("Tolerance must be positive.");
   }
}

UT_DEFINE_SCRIPT_METHOD(Rendezvous,
                        WsfOrbitalManeuvers::Rendezvous,
                        Construct_1,
                        5,
                        "WsfRendezvousManeuver",
                        "WsfOrbitalEventCondition, WsfTargetPoint, int, double, double")
{
   // Process arguments
   auto conditionPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();

   auto targetPointPtr = aVarArgs[1].GetPointer()->GetAppObject<wsf::space::TargetPointOptions>();
   auto error          = ValidateScriptTargetOptionsInputArgument(*targetPointPtr, aContext);
   if (!error.empty())
   {
      UT_SCRIPT_ABORT(error);
   }

   auto         optimizeOption = static_cast<WsfOrbitalManeuvers::OptimizeOption>(aVarArgs[2].GetInt());
   UtTimeValue  maxTime{aVarArgs[3].GetDouble(), UtUnitTime::cSECONDS};
   UtSpeedValue maxDeltaV{aVarArgs[4].GetDouble(), UtUnitSpeed::cMETERS_PER_SECOND};

   // Create the return value
   auto maneuver =
      ut::make_unique<WsfOrbitalManeuvers::Rendezvous>(*SCENARIO, *targetPointPtr, optimizeOption, maxTime, maxDeltaV);
   maneuver->SetCondition(ut::clone(conditionPtr));
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Rendezvous,
                        WsfOrbitalManeuvers::Rendezvous,
                        ConstructCost,
                        5,
                        "WsfRendezvousManeuver",
                        "WsfOrbitalEventCondition, WsfTargetPoint, WsfOrbitalOptimizationCost, double, double")
{
   // Process arguments
   auto conditionPtr   = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();
   auto targetPointPtr = aVarArgs[1].GetPointer()->GetAppObject<wsf::space::TargetPointOptions>();
   auto error          = ValidateScriptTargetOptionsInputArgument(*targetPointPtr, aContext);
   if (!error.empty())
   {
      UT_SCRIPT_ABORT(error);
   }
   auto         costPtr = aVarArgs[2].GetPointer()->GetAppObject<wsf::space::OrbitalTargetingCost>();
   UtTimeValue  maxTime{aVarArgs[3].GetDouble(), UtUnitTime::cSECONDS};
   UtSpeedValue maxDeltaV{aVarArgs[4].GetDouble(), UtUnitSpeed::cMETERS_PER_SECOND};

   // Create the return value
   auto maneuver =
      ut::make_unique<WsfOrbitalManeuvers::Rendezvous>(*SCENARIO, *targetPointPtr, *costPtr, maxTime, maxDeltaV);
   maneuver->SetCondition(ut::clone(conditionPtr));
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Rendezvous,
                        WsfOrbitalManeuvers::Rendezvous,
                        Construct_2,
                        3,
                        "WsfRendezvousManeuver",
                        "WsfOrbitalEventCondition, WsfTargetPoint, double")
{
   // Process arguments
   auto conditionPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();

   auto targetPointPtr = aVarArgs[1].GetPointer()->GetAppObject<wsf::space::TargetPointOptions>();
   auto error          = ValidateScriptTargetOptionsInputArgument(*targetPointPtr, aContext);
   if (!error.empty())
   {
      UT_SCRIPT_ABORT(error);
   }

   UtTimeValue deltaTime{aVarArgs[2].GetDouble(), UtUnitTime::cSECONDS};

   // Create the return value
   auto maneuver = ut::make_unique<WsfOrbitalManeuvers::Rendezvous>(*SCENARIO,
                                                                    *targetPointPtr,
                                                                    WsfOrbitalManeuvers::cOPTIMIZE_NONE,
                                                                    deltaTime);
   maneuver->SetCondition(ut::clone(conditionPtr));
   aReturnVal.SetPointer(new UtScriptRef(maneuver.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

PerformStaging::PerformStaging(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : OrbitalEvent(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfPerformStaging");

   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(PerformStaging, WsfOrbitalStagingEvent, Construct, 1, "WsfPerformStaging", "WsfOrbitalEventCondition")
{
   auto event        = ut::make_unique<WsfOrbitalStagingEvent>(*SCENARIO);
   auto conditionPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();
   event->SetCondition(ut::clone(conditionPtr));
   aReturnVal.SetPointer(new UtScriptRef(event.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

Scripted::Scripted(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : OrbitalEvent(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfOrbitalScriptEvent");

   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(Scripted,
                        WsfOrbitalScriptEvent,
                        Construct,
                        5,
                        "WsfOrbitalScriptEvent",
                        "WsfOrbitalEventCondition, string, string, string, string")
{
   auto event        = ut::make_unique<WsfOrbitalScriptEvent>(*SCENARIO);
   auto conditionPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();
   event->SetCondition(ut::clone(conditionPtr));
   if (!aVarArgs[1].GetString().empty())
   {
      std::string onInitialize = "on_initialize " + aVarArgs[1].GetString() + "(); end_on_initialize";
      event->SetOnInitializeInput(onInitialize);
   }
   if (!aVarArgs[2].GetString().empty())
   {
      std::string onUpdate = "on_update " + aVarArgs[2].GetString() + "(); end_on_update";
      event->SetOnUpdateInput(onUpdate);
   }
   if (!aVarArgs[3].GetString().empty())
   {
      std::string onComplete = aVarArgs[3].GetString() +
                               "(); end_on_complete"; // Note that this is supposed to be different from the first two.
      event->SetOnCompleteInput(onComplete);
   }
   if (!aVarArgs[4].GetString().empty())
   {
      std::string isComplete = "return " + aVarArgs[4].GetString() +
                               "(); end_is_complete"; // Note that this is supposed to be different from the first two.
      event->SetIsCompleteInput(isComplete);
   }
   aReturnVal.SetPointer(new UtScriptRef(event.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

ChangeAttitude::ChangeAttitude(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : OrbitalEvent(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfChangeAttitude");

   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(ChangeAttitude,
                        WsfOrbitalAttitudeChangeEvent,
                        Construct,
                        2,
                        "WsfChangeAttitude",
                        "WsfOrbitalEventCondition, WsfSpaceOrientation")
{
   auto event        = ut::make_unique<WsfOrbitalAttitudeChangeEvent>(*SCENARIO);
   auto conditionPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::OrbitalPropagatorCondition>();
   event->SetCondition(ut::clone(conditionPtr));
   auto orientationTypePtr = aVarArgs[1].GetPointer()->GetAppObject<wsf::space::Orientation>();
   event->SetOrientation(ut::clone(orientationTypePtr));
   aReturnVal.SetPointer(new UtScriptRef(event.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

OrbitalEventCondition::OrbitalEventCondition(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptObjectClass(aClassName, aScriptTypesPtr)
{
   AddClassName("WsfOrbitalEventCondition");

   AddStaticMethod(ut::make_unique<NONE>());
   AddStaticMethod(ut::make_unique<AT_RELATIVE_TIME>());
   AddStaticMethod(ut::make_unique<AT_PERIAPSIS>());
   AddStaticMethod(ut::make_unique<AT_APOAPSIS>());
   AddStaticMethod(ut::make_unique<AT_ASCENDING_NODE>());
   AddStaticMethod(ut::make_unique<AT_DESCENDING_NODE>());
   AddStaticMethod(ut::make_unique<AT_ASCENDING_RADIUS>());
   AddStaticMethod(ut::make_unique<AT_DESCENDING_RADIUS>());
   AddStaticMethod(ut::make_unique<AT_NORTHERN_INTERSECTION>());
   AddStaticMethod(ut::make_unique<AT_SOUTHERN_INTERSECTION>());
   AddStaticMethod(ut::make_unique<AT_ECLIPSE_ENTRY>());
   AddStaticMethod(ut::make_unique<AT_ECLIPSE_EXIT>());

   AddMethod(ut::make_unique<ConditionType>());
   AddMethod(ut::make_unique<SetOrbitNumber>());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEventCondition, wsf::space::OrbitalPropagatorCondition, NONE, 0, "WsfOrbitalEventCondition", "")
{
   auto conditionPtr = ut::make_unique<wsf::space::NoneCondition>();
   aReturnVal.SetPointer(new UtScriptRef(conditionPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEventCondition,
                        wsf::space::OrbitalPropagatorCondition,
                        AT_RELATIVE_TIME,
                        1,
                        "WsfOrbitalEventCondition",
                        "double")
{
   auto   conditionPtr = ut::make_unique<wsf::space::RelativeTimeCondition>();
   double relativeTime = aVarArgs[0].GetDouble();
   conditionPtr->SetOffsetTime(UtTimeValue{relativeTime, UtUnitTime::cSECONDS});
   aReturnVal.SetPointer(new UtScriptRef(conditionPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEventCondition,
                        wsf::space::OrbitalPropagatorCondition,
                        AT_PERIAPSIS,
                        0,
                        "WsfOrbitalEventCondition",
                        "")
{
   auto conditionPtr = ut::make_unique<wsf::space::PeriapsisCondition>();
   aReturnVal.SetPointer(new UtScriptRef(conditionPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEventCondition,
                        wsf::space::OrbitalPropagatorCondition,
                        AT_APOAPSIS,
                        0,
                        "WsfOrbitalEventCondition",
                        "")
{
   auto conditionPtr = ut::make_unique<wsf::space::ApoapsisCondition>();
   aReturnVal.SetPointer(new UtScriptRef(conditionPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEventCondition,
                        wsf::space::OrbitalPropagatorCondition,
                        AT_ASCENDING_NODE,
                        0,
                        "WsfOrbitalEventCondition",
                        "")
{
   auto conditionPtr = ut::make_unique<wsf::space::AscendingNodeCondition>();
   aReturnVal.SetPointer(new UtScriptRef(conditionPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEventCondition,
                        wsf::space::OrbitalPropagatorCondition,
                        AT_DESCENDING_NODE,
                        0,
                        "WsfOrbitalEventCondition",
                        "")
{
   auto conditionPtr = ut::make_unique<wsf::space::DescendingNodeCondition>();
   aReturnVal.SetPointer(new UtScriptRef(conditionPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEventCondition,
                        wsf::space::OrbitalPropagatorCondition,
                        AT_ASCENDING_RADIUS,
                        1,
                        "WsfOrbitalEventCondition",
                        "double")
{
   auto   conditionPtr = ut::make_unique<wsf::space::AscendingRadiusCondition>();
   double radius       = aVarArgs[0].GetDouble();
   conditionPtr->SetRadius(UtLengthValue{radius, UtUnitLength::cMETERS});
   aReturnVal.SetPointer(new UtScriptRef(conditionPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEventCondition,
                        wsf::space::OrbitalPropagatorCondition,
                        AT_DESCENDING_RADIUS,
                        1,
                        "WsfOrbitalEventCondition",
                        "double")
{
   auto   conditionPtr = ut::make_unique<wsf::space::DescendingRadiusCondition>();
   double radius       = aVarArgs[0].GetDouble();
   conditionPtr->SetRadius(UtLengthValue{radius, UtUnitLength::cMETERS});
   aReturnVal.SetPointer(new UtScriptRef(conditionPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEventCondition,
                        wsf::space::OrbitalPropagatorCondition,
                        AT_NORTHERN_INTERSECTION,
                        0,
                        "WsfOrbitalEventCondition",
                        "")
{
   auto conditionPtr = ut::make_unique<wsf::space::NorthernIntersectionCondition>();
   aReturnVal.SetPointer(new UtScriptRef(conditionPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEventCondition,
                        wsf::space::OrbitalPropagatorCondition,
                        AT_SOUTHERN_INTERSECTION,
                        0,
                        "WsfOrbitalEventCondition",
                        "")
{
   auto conditionPtr = ut::make_unique<wsf::space::SouthernIntersectionCondition>();
   aReturnVal.SetPointer(new UtScriptRef(conditionPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEventCondition,
                        wsf::space::OrbitalPropagatorCondition,
                        AT_ECLIPSE_ENTRY,
                        0,
                        "WsfOrbitalEventCondition",
                        "")
{
   auto conditionPtr = ut::make_unique<wsf::space::EclipseEntryCondition>();
   aReturnVal.SetPointer(new UtScriptRef(conditionPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEventCondition,
                        wsf::space::OrbitalPropagatorCondition,
                        AT_ECLIPSE_EXIT,
                        0,
                        "WsfOrbitalEventCondition",
                        "")
{
   auto conditionPtr = ut::make_unique<wsf::space::EclipseExitCondition>();
   aReturnVal.SetPointer(new UtScriptRef(conditionPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(OrbitalManeuver, WsfOrbitalManeuver, EARLIEST_TIME, 0, "int", "")
{
   aReturnVal.SetInt(WsfOrbitalManeuvers::cOPTIMIZE_TIME);
}

UT_DEFINE_SCRIPT_METHOD(OrbitalManeuver, WsfOrbitalManeuver, LEAST_DELTA_V, 0, "int", "")
{
   aReturnVal.SetInt(WsfOrbitalManeuvers::cOPTIMIZE_DELTA_V);
}

UT_DEFINE_SCRIPT_METHOD(OrbitalManeuver, WsfOrbitalManeuver, NONE, 0, "int", "")
{
   aReturnVal.SetInt(WsfOrbitalManeuvers::cOPTIMIZE_NONE);
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEventCondition, wsf::space::OrbitalPropagatorCondition, ConditionType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetConditionString());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalEventCondition, wsf::space::OrbitalPropagatorCondition, SetOrbitNumber, 1, "void", "int")
{
   int orbitNum = aVarArgs[0].GetInt();
   if (orbitNum > 0)
   {
      aObjectPtr->SetOrbitNumber(orbitNum);
   }
}

void* OrbitalEventCondition::Create(const UtScriptContext& aInstance)
{
   return new wsf::space::NoneCondition{};
}

void* OrbitalEventCondition::Clone(void* aObjectPtr)
{
   return static_cast<wsf::space::OrbitalPropagatorCondition*>(aObjectPtr)->Clone();
}

void OrbitalEventCondition::Destroy(void* aObjectPtr)
{
   delete static_cast<wsf::space::OrbitalPropagatorCondition*>(aObjectPtr);
}

OrbitalManeuverCondition::OrbitalManeuverCondition(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : OrbitalEventCondition(aClassName, aScriptTypesPtr)
{
   SetClassName("OrbitalManeuverCondition");
}

OrbitalManeuver::OrbitalManeuver(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : OrbitalEvent(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfOrbitalManeuver");

   AddStaticMethod(ut::make_unique<EARLIEST_TIME>());
   AddStaticMethod(ut::make_unique<LEAST_DELTA_V>());
}

} // namespace script

} // namespace space

} // namespace wsf
