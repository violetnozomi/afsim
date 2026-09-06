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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "WsfScriptSpaceMoverClass.hpp"

#include <memory>
#include <sstream>
#include <stdexcept>

#include "UtCentralBody.hpp"
#include "UtColor.hpp"
#include "UtECI_Conversion.hpp"
#include "UtEarth.hpp"
#include "UtLog.hpp"
#include "UtMat3.hpp"
#include "UtMemory.hpp"
#include "UtOptional.hpp"
#include "UtOrbitalPropagator.hpp"
#include "UtOrbitalState.hpp"
#include "UtQuaternion.hpp"
#include "UtRIC_Frame.hpp"
#include "UtScriptColor.hpp"
#include "UtScriptRef.hpp"
#include "UtScriptVec3.hpp"
#include "UtSphericalEarth.hpp"
#include "UtTwoLineElement.hpp"
#include "UtVec3.hpp"
#include "WsfArticulatedPart.hpp"
#include "WsfAttitudeController.hpp"
#include "WsfDateTime.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfNORAD_OrbitalPropagator.hpp"
#include "WsfNORAD_PropagatorInverter.hpp"
#include "WsfOrbitalEvent.hpp"
#include "WsfOrbitalMissionSequence.hpp"
#include "WsfOrbitalTargeting.hpp"
#include "WsfOrbitalTargetingCost.hpp"
#include "WsfPlatform.hpp"
#include "WsfPropagatorTargetPoint.hpp"
#include "WsfSimulation.hpp"
#include "WsfSpaceMover.hpp"
#include "WsfSpaceMoverBase.hpp"
#include "WsfSpaceOrientation.hpp"
#include "WsfSpaceScriptOrientation.hpp"
#include "WsfTargetPointOptions.hpp"
#include "WsfTrackManager.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptDefs.hpp"
#include "script/WsfScriptGeoPointClass.hpp"

namespace // {anonymous}
{
std::unique_ptr<UtOrbitalPropagatorBase> CreateTrackPropagator(WsfSpaceMoverBase& aSpaceMover, const WsfTrack& aTrack)
{
   UtCalendar targetTime = aSpaceMover.GetSimulation()->GetDateTime().GetCurrentTime(aTrack.GetUpdateTime());

   UtECI_Conversion convert(targetTime, aSpaceMover.GetCentralBody());

   double locTargetWCS[3];
   aTrack.GetLocationWCS(locTargetWCS);
   convert.SetLocationWCS(locTargetWCS);
   UtVec3d locTargetECI = convert.GetLocationECI();

   double velTargetWCS[3];
   aTrack.GetVelocityWCS(velTargetWCS);
   convert.SetVelocityWCS(velTargetWCS);
   UtVec3d velTargetECI = convert.GetVelocityECI();

   auto             trackPropPtr = ut::clone(aSpaceMover.GetPropagator());
   ut::OrbitalState initialState(targetTime,
                                 trackPropPtr->GetOrbitalState().GetCentralBody(),
                                 ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                 ut::OrbitalState::ReferenceFrame::cECI,
                                 ut::OrbitalState::Vector(locTargetECI, velTargetECI));
   trackPropPtr->SetInitialOrbitalState(initialState);
   trackPropPtr->Initialize(targetTime);

   return trackPropPtr;
}

std::unique_ptr<UtOrbitalPropagatorBase> CreateTruthPropagator(WsfSpaceMoverBase& aSpaceMover, const WsfPlatform& aPlatform)
{
   UtCalendar targetTime = aSpaceMover.GetSimulation()->GetDateTime().GetCurrentTime(aPlatform.GetSimTime());

   UtVec3d locTargetECI = UtVec3d(aPlatform.GetLocationECI());
   UtVec3d velTargetECI = UtVec3d(aPlatform.GetVelocityECI());

   auto             platPropPtr = ut::clone(aSpaceMover.GetPropagator());
   ut::OrbitalState initialState(targetTime,
                                 platPropPtr->GetOrbitalState().GetCentralBody(),
                                 ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                 ut::OrbitalState::ReferenceFrame::cECI,
                                 ut::OrbitalState::Vector(locTargetECI, velTargetECI));
   platPropPtr->SetInitialOrbitalState(initialState);
   platPropPtr->Initialize(targetTime);

   return platPropPtr;
}

WsfOrbitalTargeting MakeManeuverTarget(WsfSpaceMoverBase& aSpaceMover, const WsfTrack& aTrack)
{
   UtOrbitalPropagatorBase&          interceptorProp = aSpaceMover.GetPropagator();
   UtCalendar                        interceptorNow  = interceptorProp.GetCurrentTime();
   wsf::space::PropagatorTargetPoint targetPoint{CreateTrackPropagator(aSpaceMover, aTrack)};
   return WsfOrbitalTargeting{interceptorNow, interceptorProp, targetPoint};
}

WsfOrbitalTargeting MakeManeuverTarget(WsfSpaceMoverBase& aSpaceMover, wsf::space::TargetPointOptions& aOptions)
{
   // If this is a case where a target propagator is needed, create that propagator and provide it
   // to aOptions.
   if (aOptions.HasTrackTarget())
   {
      WsfLocalTrack* ltPtr = aSpaceMover.GetPlatform()->GetTrackManager().FindTrack(aOptions.GetLocalTrackId());
      if (ltPtr != nullptr)
      {
         aOptions.SetTargetPropagator(CreateTrackPropagator(aSpaceMover, *ltPtr));
      }
      else
      {
         std::stringstream msg;
         msg << "Unable to find a local track corresponding with local track id: " << aOptions.GetLocalTrackId() << ".";
         throw std::runtime_error{msg.str()};
      }
   }
   else if (aOptions.HasPlatformTarget())
   {
      const WsfPlatform* platformPtr = aSpaceMover.GetSimulation()->GetPlatformByName(aOptions.GetPlatformName());
      if (platformPtr != nullptr)
      {
         aOptions.SetTargetPropagator(CreateTruthPropagator(aSpaceMover, *platformPtr));
      }
      else
      {
         std::stringstream msg;
         msg << "Unable to find platform with name: " << aOptions.GetPlatformName() << ".";
         throw std::runtime_error{msg.str()};
      }
   }

   if (aOptions.RequiresManeuveringPropagator())
   {
      aOptions.SetManeuveringPropagator(ut::clone(aSpaceMover.GetPropagator()));
   }

   UtCalendar targetPointEpoch;
   if (aOptions.GetTargetPropagator())
   {
      targetPointEpoch = aOptions.GetTargetPropagator()->GetCurrentTime();
   }
   // If there is no target propagator, we are in a case where the input argument to InstantiatePoint
   // is not used, so the default constructed targetPointEpoch will habe no effect.
   auto targetPointPtr = aOptions.InstantiatePoint(targetPointEpoch);
   if (targetPointPtr == nullptr)
   {
      throw std::runtime_error{"Unable to create target point."};
   }

   UtOrbitalPropagatorBase& interceptorProp = aSpaceMover.GetPropagator();
   UtCalendar               interceptorNow  = interceptorProp.GetCurrentTime();
   return WsfOrbitalTargeting{interceptorNow, interceptorProp, *targetPointPtr};
}

bool ComputeFixedTimeSolution(const WsfOrbitalTargeting& aTargeting,
                              double                     aTransferTime,
                              bool                       aRendezvous,
                              double&                    aDeltaT,
                              double&                    aDeltaV)
{
   auto result = aTargeting.Solve(aTransferTime, aRendezvous, aDeltaV);
   aDeltaT     = aTransferTime;
   return result.IsSolution();
}

bool ComputeOptimizedSolution(const WsfOrbitalTargeting& aTargeting,
                              double                     aMaxDeltaT,
                              double                     aMaxDeltaV,
                              bool                       aMinimizeDeltaV,
                              bool                       aRendezvous,
                              double&                    aDeltaT,
                              double&                    aDeltaV)
{
   UtLambertProblem::Result result{};
   if (aMinimizeDeltaV)
   {
      static constexpr double cDELTAV_TIME_TOLERANCE = 1.0E-5;
      result = aTargeting.MinimizeDeltaV(aMaxDeltaT, aMaxDeltaV, aRendezvous, cDELTAV_TIME_TOLERANCE, aDeltaT, aDeltaV);
   }
   else
   {
      static constexpr double cDELTAT_TIME_TOLERANCE = 1.0E-2;
      result = aTargeting.MinimizeDeltaT(aMaxDeltaT, aMaxDeltaV, aRendezvous, cDELTAT_TIME_TOLERANCE, aDeltaT, aDeltaV);
   }
   return result.IsSolution();
}

bool ComputeOptimizedSolution(const WsfOrbitalTargeting&        aTargeting,
                              double                            aMaxDeltaT,
                              double                            aMaxDeltaV,
                              wsf::space::OrbitalTargetingCost& aCost,
                              bool                              aRendezvous,
                              double&                           aDeltaT,
                              double&                           aDeltaV)
{
   // This dependence of the tolerance on the result of IsLeastTime is to ensure this will
   // yield the same results as ComputeOptimizedSolution taking a bool to select the optimization
   // choice if the the cost happens to be purely delta-t.
   double tolerance = aCost.IsLeastTime() ? 1.0E-2 : 1.0E-5;
   return aTargeting.MinimizeCost(aCost, aMaxDeltaT, aMaxDeltaV, aRendezvous, tolerance, aDeltaT, aDeltaV).IsSolution();
}

// deprecated
bool PerformTargetingComputation(WsfSpaceMoverBase& aMover,
                                 const WsfTrackId&  aTrackId,
                                 double             aMaxDeltaT,
                                 double             aMaxDeltaV,
                                 bool               aMinimizeDeltaV,
                                 bool               aRendezvous,
                                 double&            aDeltaT,
                                 double&            aDeltaV)
{
   WsfLocalTrack* trackPtr = aMover.GetPlatform()->GetTrackManager().FindTrack(aTrackId);
   const auto     tgt      = MakeManeuverTarget(aMover, *trackPtr);
   return ComputeOptimizedSolution(tgt, aMaxDeltaT, aMaxDeltaV, aMinimizeDeltaV, aRendezvous, aDeltaT, aDeltaV);
}

} // namespace

WsfScriptSpaceMoverClass::WsfScriptSpaceMoverClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptMoverClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSpaceMover");

   AddMethod(ut::make_unique<LocationAtTime>());
   AddMethod(ut::make_unique<LocationECI_AtTime>());
   AddMethod(ut::make_unique<VelocityECI_AtTime>());
   AddMethod(ut::make_unique<SemiMajorAxis>());
   AddMethod(ut::make_unique<Eccentricity>());
   AddMethod(ut::make_unique<Inclination>());
   AddMethod(ut::make_unique<RAAN>());
   AddMethod(ut::make_unique<MeanMotion>());
   AddMethod(ut::make_unique<MeanAnomaly>());
   AddMethod(ut::make_unique<TrueAnomaly>());
   AddMethod(ut::make_unique<ArgumentOfPeriapsis>());
   AddMethod(ut::make_unique<SetOrientation_1>("SetOrientation"));
   AddMethod(ut::make_unique<SetOrientation_2>("SetOrientation"));
   AddMethod(ut::make_unique<SetOrientation_3>("SetOrientation"));
   AddMethod(ut::make_unique<SetOrientation_4>("SetOrientation"));
   AddMethod(ut::make_unique<SetOrientation_5>("SetOrientation"));
   AddMethod(ut::make_unique<SetOrientation_6>("SetOrientation"));
   AddMethod(ut::make_unique<GetTimeToConstraint>());

   AddMethod(ut::make_unique<OrbitalPeriod>());
   AddMethod(ut::make_unique<RevolutionsPerDay>());
   AddMethod(ut::make_unique<OrbitalRegime>());
   AddMethod(ut::make_unique<RevolutionNumber_1>("RevolutionNumber"));
   AddMethod(ut::make_unique<RevolutionNumber_2>("RevolutionNumber"));
   AddMethod(ut::make_unique<Apoapsis>());
   AddMethod(ut::make_unique<ApoapsisRadius>());
   AddMethod(ut::make_unique<ApoapsisAltitude>());
   AddMethod(ut::make_unique<Periapsis>());
   AddMethod(ut::make_unique<PeriapsisRadius>());
   AddMethod(ut::make_unique<PeriapsisAltitude>());


   // Initial Orbital Elements Accessor Methods
   AddMethod(ut::make_unique<InitialEpoch>());
   AddMethod(ut::make_unique<InitialArgumentOfPeriapsis>());
   AddMethod(ut::make_unique<InitialEccentricity>());
   AddMethod(ut::make_unique<InitialInclination>());
   AddMethod(ut::make_unique<InitialMeanAnomaly>());
   AddMethod(ut::make_unique<InitialMeanMotion>());
   AddMethod(ut::make_unique<InitialRAAN>());
   AddMethod(ut::make_unique<InitialSemiMajorAxis>());
   AddMethod(ut::make_unique<InitialTrueAnomaly>());

   // Initial State Accessor Methods
   AddMethod(ut::make_unique<InitialPositionLLA>());
   AddMethod(ut::make_unique<InitialPositionWCS>());
   AddMethod(ut::make_unique<InitialPositionECI>());
   AddMethod(ut::make_unique<InitialVelocityWCS>());
   AddMethod(ut::make_unique<InitialVelocityECI>());

   // Initial State Convenience Methods
   AddMethod(ut::make_unique<InitialHeading>());

   // Eclipse methods
   AddMethod(ut::make_unique<EclipseAnomalies>());
   AddMethod(ut::make_unique<EclipsedState>());
   AddMethod(ut::make_unique<EclipseTimes>());

   AddMethod(ut::make_unique<SetInitialPoint_1>("SetOrbit"));
   AddMethod(ut::make_unique<SetInitialPoint_2>("SetOrbit"));
   AddMethod(ut::make_unique<SetInitialPoint_3>("SetOrbit"));
   AddMethod(ut::make_unique<SetInitialOrbitalState>());

   // TLE-related methods
   AddMethod(ut::make_unique<TwoLineElement>());
   AddMethod(ut::make_unique<SetTwoLineElement>());
   AddMethod(ut::make_unique<InitialTwoLineElement>());
   AddMethod(ut::make_unique<SatelliteNumber>());
   AddMethod(ut::make_unique<Classification>());
   AddMethod(ut::make_unique<BallisticCoefficient>());
   AddMethod(ut::make_unique<BStar>());

   // Maneuvering methods
   AddMethod(ut::make_unique<CurrentManeuver>());
   AddMethod(ut::make_unique<ExecuteManeuver>());
   AddMethod(ut::make_unique<ExecuteManeuverSequence>());

   AddMethod(ut::make_unique<CurrentOrbitalEvent>());
   AddMethod(ut::make_unique<ExecuteOrbitalEvent>());
   AddMethod(ut::make_unique<ExecuteMissionSequence>());
   AddMethod(ut::make_unique<CancelMissionSequence>());

   AddMethod(ut::make_unique<AvailableDeltaV>());
   AddMethod(ut::make_unique<ManeuverDuration>());
   AddMethod(ut::make_unique<AvailableManeuverDuration>());

   // Maneuver speculation methods
   AddMethod(ut::make_unique<ComputeRendezvousDeprecated>("ComputeRendezvous"));
   AddMethod(ut::make_unique<ComputeInterceptDeprecated>("ComputeIntercept"));
   AddMethod(ut::make_unique<ComputeRendezvousTrackOptimized>("ComputeRendezvous"));
   AddMethod(ut::make_unique<ComputeInterceptTrackOptimized>("ComputeIntercept"));
   AddMethod(ut::make_unique<ComputeRendezvousTrackFixed>("ComputeRendezvous"));
   AddMethod(ut::make_unique<ComputeInterceptTrackFixed>("ComputeIntercept"));
   AddMethod(ut::make_unique<ComputeRendezvousTargetPointOptimized>("ComputeRendezvous"));
   AddMethod(ut::make_unique<ComputeRendezvousTargetPointCost>("ComputeRendezvous"));
   AddMethod(ut::make_unique<ComputeRendezvousTargetPointFixed>("ComputeRendezvous"));
   AddMethod(ut::make_unique<ComputeInterceptTargetPointOptimized>("ComputeIntercept"));
   AddMethod(ut::make_unique<ComputeInterceptTargetPointCost>("ComputeIntercept"));
   AddMethod(ut::make_unique<ComputeInterceptTargetPointFixed>("ComputeIntercept"));

   // Get function for descriptor and designator
   AddMethod(ut::make_unique<Descriptor>());
   AddMethod(ut::make_unique<Designator>());

   AddMethod(ut::make_unique<SetOrbitColor>());
   AddMethod(ut::make_unique<GetOrbitColor>());

   // RIC Calculation Functions
   AddMethod(ut::make_unique<RIC_1>("RIC"));
   AddMethod(ut::make_unique<RIC_2>("RIC"));
   AddMethod(ut::make_unique<RIC_AtTime_1>("RIC_AtTime"));
   AddMethod(ut::make_unique<RIC_AtTime_2>("RIC_AtTime"));

   // Convenience Methods
   AddMethod(ut::make_unique<LookAngleTo_Track>("LookAngleTo"));
   AddMethod(ut::make_unique<LookAngleTo_Platform>("LookAngleTo"));
   AddMethod(ut::make_unique<LookAngleTo_LLA>("LookAngleTo"));
   AddMethod(ut::make_unique<LookAngleTo_GeoPoint>("LookAngleTo"));
   AddMethod(ut::make_unique<IncidenceAngle_Track>("IncidenceAngle"));
   AddMethod(ut::make_unique<IncidenceAngle_Platform>("IncidenceAngle"));
   AddMethod(ut::make_unique<IncidenceAngle_LLA>("IncidenceAngle"));
   AddMethod(ut::make_unique<IncidenceAngle_GeoPoint>("IncidenceAngle"));
   AddMethod(ut::make_unique<TargetElevationAngle_Track>("TargetElevationAngle"));
   AddMethod(ut::make_unique<TargetElevationAngle_Platform>("TargetElevationAngle"));
   AddMethod(ut::make_unique<TargetElevationAngle_LLA>("TargetElevationAngle"));
   AddMethod(ut::make_unique<TargetElevationAngle_GeoPoint>("TargetElevationAngle"));
   AddMethod(ut::make_unique<SquintAngleTo_Track>("SquintAngleTo"));
   AddMethod(ut::make_unique<SquintAngleTo_Platform>("SquintAngleTo"));
   AddMethod(ut::make_unique<SquintAngleTo_LLA>("SquintAngleTo"));
   AddMethod(ut::make_unique<SquintAngleTo_GeoPoint>("SquintAngleTo"));
   AddMethod(ut::make_unique<DistanceToHorizon>());
   AddMethod(ut::make_unique<LookAngleToHorizon>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, LocationAtTime, 1, "WsfGeoPoint", "double")
{
   double     timeNow = WsfScriptContext::GetTIME_NOW(aContext);
   double     simTime = aVarArgs[0].GetDouble();
   UtCalendar epoch   = aObjectPtr->GetSimulation()->GetDateTime().GetStartDateAndTime();
   UtCalendar argTime = epoch;
   argTime.AdvanceTimeBy(simTime);
   epoch.AdvanceTimeBy(timeNow);
   UtOrbitalPropagatorBase& prop = aObjectPtr->GetPropagator();
   prop.Update(argTime); // Predict motion to the argument time
   UtVec3d locWCS = prop.GetOrbitalState().GetOrbitalStateVectorWCS().GetLocation();
   prop.Update(epoch); // Return the propagator to the original state
   aReturnVal.SetPointer(new UtScriptRef(new WsfGeoPoint(locWCS.GetData()), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, LocationECI_AtTime, 1, "Vec3", "double")
{
   double     timeNow = WsfScriptContext::GetTIME_NOW(aContext);
   double     simTime = aVarArgs[0].GetDouble();
   UtCalendar epoch   = aObjectPtr->GetSimulation()->GetDateTime().GetStartDateAndTime();
   UtCalendar argTime = epoch;
   argTime.AdvanceTimeBy(simTime);
   epoch.AdvanceTimeBy(timeNow);
   UtOrbitalPropagatorBase& prop = aObjectPtr->GetPropagator();
   prop.Update(argTime); // Predict motion to the argument time
   UtVec3d locECI = prop.GetOrbitalState().GetOrbitalStateVectorInertial().GetLocation();
   prop.Update(epoch); // Return the propagator to the original state
   aReturnVal.SetPointer(UtScriptVec3::Create(locECI));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, VelocityECI_AtTime, 1, "Vec3", "double")
{
   double     timeNow = WsfScriptContext::GetTIME_NOW(aContext);
   double     simTime = aVarArgs[0].GetDouble();
   UtCalendar epoch   = aObjectPtr->GetSimulation()->GetDateTime().GetStartDateAndTime();
   UtCalendar argTime = epoch;
   argTime.AdvanceTimeBy(simTime);
   epoch.AdvanceTimeBy(timeNow);
   UtOrbitalPropagatorBase& prop = aObjectPtr->GetPropagator();
   prop.Update(argTime); // Predict motion to the argument time
   UtVec3d velECI = prop.GetOrbitalState().GetOrbitalStateVectorInertial().GetVelocity();
   prop.Update(epoch); // Return the propagator to the original state
   aReturnVal.SetPointer(UtScriptVec3::Create(velECI));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, SemiMajorAxis, 0, "double", "")
{
   aObjectPtr->Update(WsfScriptContext::GetTIME_NOW(aContext));
   aReturnVal = aObjectPtr->GetOrbitalState().GetOrbitalElements().GetSemiMajorAxis();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, Eccentricity, 0, "double", "")
{
   aObjectPtr->Update(WsfScriptContext::GetTIME_NOW(aContext));
   aReturnVal = aObjectPtr->GetOrbitalState().GetOrbitalElements().GetEccentricity();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, Inclination, 0, "double", "")
{
   aObjectPtr->Update(WsfScriptContext::GetTIME_NOW(aContext));
   aReturnVal = aObjectPtr->GetOrbitalState().GetOrbitalElementsTOD().GetInclination() * UtMath::cDEG_PER_RAD;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, RAAN, 0, "double", "")
{
   aObjectPtr->Update(WsfScriptContext::GetTIME_NOW(aContext));
   aReturnVal = aObjectPtr->GetOrbitalState().GetOrbitalElementsTOD().GetRAAN() * UtMath::cDEG_PER_RAD;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, MeanMotion, 0, "double", "")
{
   aObjectPtr->Update(WsfScriptContext::GetTIME_NOW(aContext));
   aReturnVal = aObjectPtr->GetOrbitalState().GetOrbitalElements().GetMeanMotion() * UtMath::cDEG_PER_RAD;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, MeanAnomaly, 0, "double", "")
{
   aObjectPtr->Update(WsfScriptContext::GetTIME_NOW(aContext));
   aReturnVal = aObjectPtr->GetOrbitalState().GetOrbitalElementsTOD().GetMeanAnomaly() * UtMath::cDEG_PER_RAD;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, TrueAnomaly, 0, "double", "")
{
   aObjectPtr->Update(WsfScriptContext::GetTIME_NOW(aContext));
   aReturnVal = aObjectPtr->GetOrbitalState().GetOrbitalElementsTOD().GetTrueAnomaly() * UtMath::cDEG_PER_RAD;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, ArgumentOfPeriapsis, 0, "double", "")
{
   aObjectPtr->Update(WsfScriptContext::GetTIME_NOW(aContext));
   aReturnVal = aObjectPtr->GetOrbitalState().GetOrbitalElementsTOD().GetArgumentOfPeriapsis() * UtMath::cDEG_PER_RAD;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, SetOrientation_1, 1, "void", "Quaternion")
{
   UtQuaternion* quaternionPtr = static_cast<UtQuaternion*>(aVarArgs[0].GetPointer()->GetAppObject());
   // the quaternion is defined to be an ECI-body transform.
   double Teb[3][3];
   quaternionPtr->Get(Teb);
   double psi, theta, phi;
   UtEntity::ExtractEulerAngles(Teb, psi, theta, phi);
   aObjectPtr->GetAttitudeController().SetOrientation(
      wsf::space::Orientation::Factory(wsf::space::OrientationNone::GetTypeName()));
   aObjectPtr->SetTargetOrientationECI(psi, theta, phi);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, SetOrientation_2, 1, "bool", "string")
{
   std::string                              orientation    = aVarArgs[0].GetString();
   std::unique_ptr<wsf::space::Orientation> orientationPtr = wsf::space::Orientation::Factory(orientation);
   bool                                     success        = (orientationPtr != nullptr);
   if (success)
   {
      aObjectPtr->GetAttitudeController().SetOrientation(std::move(orientationPtr));
   }
   aReturnVal.SetBool(success);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, SetOrientation_3, 1, "void", "WsfGeoPoint")
{
   WsfGeoPoint* gpPtr = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());

   // Construct the ECI rotation matrix from direction cosines.
   UtVec3d xe = aObjectPtr->GetPropagator().GetOrbitalState().GetOrbitalStateVectorWCS().GetLocation();
   UtVec3d ye, ze;
   UtVec3d gpLoc;
   gpPtr->GetLocationWCS(gpLoc.GetData());
   UtVec3d gpVec;
   gpVec.Subtract(gpLoc, xe);
   xe.Multiply(-1.0);

   ze.Set(gpVec);
   ze.Normalize();
   xe.Normalize();

   // For the following cross products, there is a 'rotational' rule:
   // X = Y x Z;  Z = X x Y; Y = Z x X
   ye.CrossProduct(ze, xe);
   ye.Normalize();

   // Final x vector should be orthogonal to y and z
   xe.CrossProduct(ye, ze);

   double transECS_ECEF[3][3];
   for (int i = 0; i < 3; ++i)
   {
      transECS_ECEF[i][0] = xe[i];
      transECS_ECEF[i][1] = ye[i];
      transECS_ECEF[i][2] = ze[i];
   }

   double transECS_ECI[3][3];
   double transECEF_ECI[3][3];
   double transECI_ECEF[3][3];
   aObjectPtr->GetPlatform()->GetECIToWCSTransform(transECI_ECEF);
   UtMat3d::Transpose(transECEF_ECI, transECI_ECEF);
   UtMat3d::Multiply(transECS_ECI, transECEF_ECI, transECS_ECEF);

   // Extract the Euler angles for ECI yaw, pitch, roll.
   // The local +X axis should be in the direction of flight,
   // the local +Y should be out the right side and +Z should be down.
   double theta = -asin(transECS_ECI[2][0]);
   double psi   = atan2(transECS_ECI[1][0], transECS_ECI[0][0]);
   double phi   = atan2(transECS_ECI[2][1], transECS_ECI[2][2]);

   aObjectPtr->GetAttitudeController().SetOrientation(
      wsf::space::Orientation::Factory(wsf::space::OrientationNone::GetTypeName()));
   aObjectPtr->SetTargetOrientationECI(psi, theta, phi);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, SetOrientation_4, 5, "void", "double, double, double, Vec3, Vec3")
{
   double ecsYaw   = aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG;
   double ecsPitch = aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG;
   ;
   double ecsRoll = aVarArgs[2].GetDouble() * UtMath::cRAD_PER_DEG;
   ;
   UtVec3d* xAxisRot = (UtVec3d*)aVarArgs[3].GetPointer()->GetAppObject();
   UtVec3d* zAxisRot = (UtVec3d*)aVarArgs[4].GetPointer()->GetAppObject();
   aObjectPtr->GetAttitudeController().SetOrientation(
      wsf::space::Orientation::Factory(wsf::space::OrientationNone::GetTypeName()));
   aObjectPtr->GetAttitudeController().SetTargetOrientation(ecsYaw, ecsPitch, ecsRoll, *xAxisRot, *zAxisRot);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, SetOrientation_5, 3, "void", "WsfArticulatedPart, Vec3, Vec3")
{
   WsfArticulatedPart* articulatedPartPtr = static_cast<WsfArticulatedPart*>(aVarArgs[0].GetPointer()->GetAppObject());
   UtVec3d*            xAxisRot           = (UtVec3d*)aVarArgs[1].GetPointer()->GetAppObject();
   UtVec3d*            zAxisRot           = (UtVec3d*)aVarArgs[2].GetPointer()->GetAppObject();
   aObjectPtr->GetAttitudeController().SetOrientation(
      wsf::space::Orientation::Factory(wsf::space::OrientationNone::GetTypeName()));
   aObjectPtr->GetAttitudeController().SetTargetOrientation(articulatedPartPtr->GetYaw(),
                                                            articulatedPartPtr->GetPitch(),
                                                            articulatedPartPtr->GetRoll(),
                                                            *xAxisRot,
                                                            *zAxisRot);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, SetOrientation_6, 1, "void", "WsfSpaceOrientation")
{
   auto orientationPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::Orientation>();
   aObjectPtr->GetAttitudeController().SetOrientation(ut::clone(orientationPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, EclipseAnomalies, 0, "Array<double>", "")
{
   std::vector<UtScriptData>* returnDataPtr = new std::vector<UtScriptData>();
   double                     entryAnomaly, exitAnomaly;
   bool                       found = aObjectPtr->GetPropagator().GetEclipseAnomalies(entryAnomaly, exitAnomaly);
   if (found)
   {
      returnDataPtr->emplace_back(entryAnomaly);
      returnDataPtr->emplace_back(exitAnomaly);
   }
   aReturnVal.SetPointer(new UtScriptRef(returnDataPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, EclipseTimes, 0, "Array<double>", "")
{
   std::vector<UtScriptData>* returnDataPtr = new std::vector<UtScriptData>();
   double                     entryTime = -1.0, exitTime = -1.0;
   bool                       found = aObjectPtr->GetPropagator().GetEclipseTimes(entryTime, exitTime);
   if (found)
   {
      double propagatorSimTime =
         aObjectPtr->GetPropagator().GetCurrentTime().GetTimeSince(SIMULATION->GetDateTime().GetStartDateAndTime());
      returnDataPtr->emplace_back(entryTime + propagatorSimTime);
      returnDataPtr->emplace_back(exitTime + propagatorSimTime);
   }
   aReturnVal.SetPointer(new UtScriptRef(returnDataPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, EclipsedState, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetPropagator().GetEclipsedState());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, GetTimeToConstraint, 1, "double", "WsfOrbitalEventCondition")
{
   auto*  condPtr = static_cast<wsf::space::OrbitalPropagatorCondition*>(aVarArgs[0].GetPointer()->GetAppObject());
   double retval{};
   bool   result = condPtr->GetTimeToCondition(aObjectPtr->GetPropagator(), retval);
   if (!result)
   {
      retval = -1.0;
   }
   aReturnVal.SetDouble(retval);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, OrbitalPeriod, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetOrbitalState().GetOrbitalElements().GetPeriod());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, RevolutionsPerDay, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetOrbitalState().GetOrbitalElements().GetRevolutionsPerDay());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, OrbitalRegime, 0, "string", "")
{
   std::string desc{};
   switch (aObjectPtr->GetOrbitalState().GetOrbitalElements().GetOrbitalRegime())
   {
   case UtOrbitalElements::OrbitalRegime::cLEO:
      desc = "LEO";
      break;
   case UtOrbitalElements::OrbitalRegime::cMEO:
      desc = "MEO";
      break;
   case UtOrbitalElements::OrbitalRegime::cGEO:
      desc = "GEO";
      break;
   case UtOrbitalElements::OrbitalRegime::cHEO:
      desc = "HEO";
      break;
   case UtOrbitalElements::OrbitalRegime::cNO_LABEL:
      // NOTE: desc left empty intentionally.
      break;
   }
   aReturnVal.SetString(desc);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, RevolutionNumber_1, 1, "double", "double")
{
   double delta  = TIME_NOW - aVarArgs[0].GetDouble();
   double period = aObjectPtr->GetOrbitalState().GetOrbitalElements().GetPeriod();
   aReturnVal.SetDouble(delta / period);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, RevolutionNumber_2, 1, "double", "Calendar")
{
   UtCalendar* thenPtr = aVarArgs[0].GetPointer()->GetAppObject<UtCalendar>();
   UtCalendar  now     = aObjectPtr->GetSimulation()->GetDateTime().GetStartDateAndTime();
   now.AdvanceTimeBy(TIME_NOW);
   double delta  = now.GetTimeSince(*thenPtr);
   double period = aObjectPtr->GetOrbitalState().GetOrbitalElements().GetPeriod();
   aReturnVal.SetDouble(delta / period);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, Apoapsis, 0, "double", "") // DEPRECATED
{
   { // RAII block
      auto msg = ut::log::warning() << "Method 'Apoapsis' is deprecated.";
      msg.AddNote() << "Use 'ApoapsisRadius' instead.";
   }
   aReturnVal.SetDouble(aObjectPtr->GetOrbitalState().GetOrbitalElements().GetApoapsisRadius());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, ApoapsisRadius, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetOrbitalState().GetOrbitalElements().GetApoapsisRadius());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, ApoapsisAltitude, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetOrbitalState().GetOrbitalElements().GetApoapsisAltitude());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, Periapsis, 0, "double", "") // DEPRECATED
{
   { // RAII block
      auto msg = ut::log::warning() << "Method 'Periapsis' is deprecated.";
      msg.AddNote() << "Use 'PeriapsisRadius' instead.";
   }
   aReturnVal.SetDouble(aObjectPtr->GetOrbitalState().GetOrbitalElements().GetPeriapsisRadius());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, PeriapsisRadius, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetOrbitalState().GetOrbitalElements().GetPeriapsisRadius());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, PeriapsisAltitude, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetOrbitalState().GetOrbitalElements().GetPeriapsisAltitude());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, InitialEpoch, 0, "Calendar", "")
{
   auto retvalPtr = ut::make_unique<UtCalendar>(aObjectPtr->GetInitialOrbitalState().GetEpoch());
   aReturnVal.SetPointer(new UtScriptRef(retvalPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, InitialArgumentOfPeriapsis, 0, "double", "")
{
   double argp = aObjectPtr->GetInitialOrbitalState().GetOrbitalElements().GetArgumentOfPeriapsis() * UtMath::cDEG_PER_RAD;
   aReturnVal.SetDouble(argp);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, InitialEccentricity, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetInitialOrbitalState().GetOrbitalElements().GetEccentricity());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, InitialInclination, 0, "double", "")
{
   double incl = aObjectPtr->GetInitialOrbitalState().GetOrbitalElements().GetInclination() * UtMath::cDEG_PER_RAD;
   aReturnVal.SetDouble(incl);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, InitialMeanAnomaly, 0, "double", "")
{
   double anom = aObjectPtr->GetInitialOrbitalState().GetOrbitalElements().GetMeanAnomaly() * UtMath::cDEG_PER_RAD;
   aReturnVal.SetDouble(anom);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, InitialMeanMotion, 0, "double", "")
{
   double mmot = aObjectPtr->GetInitialOrbitalState().GetOrbitalElements().GetMeanMotion() * UtMath::cDEG_PER_RAD;
   aReturnVal.SetDouble(mmot);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, InitialRAAN, 0, "double", "")
{
   double raan = aObjectPtr->GetInitialOrbitalState().GetOrbitalElements().GetRAAN() * UtMath::cDEG_PER_RAD;
   aReturnVal.SetDouble(raan);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, InitialSemiMajorAxis, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetInitialOrbitalState().GetOrbitalElements().GetSemiMajorAxis());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, InitialTrueAnomaly, 0, "double", "")
{
   double anom = aObjectPtr->GetInitialOrbitalState().GetOrbitalElements().GetTrueAnomaly() * UtMath::cDEG_PER_RAD;
   aReturnVal.SetDouble(anom);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, InitialPositionLLA, 0, "WsfGeoPoint", "")
{
   double lat, lon, alt;
   aObjectPtr->GetCentralBody().ConvertWCS_ToLLA(aObjectPtr->GetInitialPositionWCS(), lat, lon, alt);
   auto pointPtr = ut::make_unique<WsfGeoPoint>(lat, lon, alt);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, InitialPositionECI, 0, "Vec3", "")
{
   aReturnVal.SetPointer(UtScriptVec3::Create(aObjectPtr->GetInitialPositionECI()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, InitialPositionWCS, 0, "Vec3", "")
{
   aReturnVal.SetPointer(UtScriptVec3::Create(aObjectPtr->GetInitialPositionWCS()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, InitialVelocityECI, 0, "Vec3", "")
{
   aReturnVal.SetPointer(UtScriptVec3::Create(aObjectPtr->GetInitialVelocityECI()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, InitialVelocityWCS, 0, "Vec3", "")
{
   aReturnVal.SetPointer(UtScriptVec3::Create(aObjectPtr->GetInitialVelocityWCS()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, InitialHeading, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetInitialHeading() * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, AvailableDeltaV, 0, "double", "")
{
   double deltaV = aObjectPtr->GetAvailableDeltaV();
   aReturnVal.SetDouble(deltaV);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, SetInitialPoint_1, 4, "bool", "double, double, double, double")
{
   double lat      = aVarArgs[0].GetDouble();
   double lon      = aVarArgs[1].GetDouble();
   double alt      = aVarArgs[2].GetDouble();
   double heading  = aVarArgs[3].GetDouble();
   bool   setPoint = aObjectPtr->SetInitialPoint(lat, lon, alt, 0.0, heading);
   if (setPoint)
   {
      WsfPlatform* platformPtr = aObjectPtr->GetPlatform();
      if ((platformPtr != nullptr) && aObjectPtr->GetPlatform()->IsInitialized())
      {
         // Re-initialize the mover at the current time
         aObjectPtr->Initialize(TIME_NOW);
      }
   }
   aReturnVal.SetBool(setPoint);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, SetInitialPoint_2, 2, "void", "Vec3, Vec3")
{
   auto locECIPtr = aVarArgs[0].GetPointer()->GetAppObject<UtVec3d>();
   auto velECIPtr = aVarArgs[1].GetPointer()->GetAppObject<UtVec3d>();

   ut::OrbitalState initialState(SIMULATION->GetDateTime().GetCurrentTime(TIME_NOW),
                                 aObjectPtr->GetInitialOrbitalState().GetCentralBody(),
                                 aObjectPtr->GetInitialOrbitalState().GetCoordinateSystem(),
                                 ut::OrbitalState::ReferenceFrame::cECI,
                                 ut::OrbitalState::Vector(*locECIPtr, *velECIPtr));

   aObjectPtr->SetInitialOrbitalState(initialState);
   WsfPlatform* platformPtr = aObjectPtr->GetPlatform();
   if ((platformPtr != nullptr) && aObjectPtr->GetPlatform()->IsInitialized())
   {
      // Re-initialize the mover at the current time
      aObjectPtr->Initialize(TIME_NOW);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, SetInitialPoint_3, 3, "void", "Vec3, Vec3, Calendar")
{
   auto locECIPtr          = aVarArgs[0].GetPointer()->GetAppObject<UtVec3d>();
   auto velECIPtr          = aVarArgs[1].GetPointer()->GetAppObject<UtVec3d>();
   auto initialCalendarPtr = aVarArgs[2].GetPointer()->GetAppObject<UtCalendar>();

   ut::OrbitalState initialState(*initialCalendarPtr,
                                 aObjectPtr->GetInitialOrbitalState().GetCentralBody(),
                                 aObjectPtr->GetInitialOrbitalState().GetCoordinateSystem(),
                                 ut::OrbitalState::ReferenceFrame::cECI,
                                 ut::OrbitalState::Vector(*locECIPtr, *velECIPtr));
   aObjectPtr->SetInitialOrbitalState(initialState);
   WsfPlatform* platformPtr = aObjectPtr->GetPlatform();
   if ((platformPtr != nullptr) && aObjectPtr->GetPlatform()->IsInitialized())
   {
      // Re-initialize the mover at the current time
      aObjectPtr->Initialize(TIME_NOW);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, TwoLineElement, 0, "string", "")
{
   aObjectPtr->GetPlatform()->Update(WsfScriptContext::GetTIME_NOW(aContext));
   std::ostringstream oss;

   if (!aObjectPtr->IsA_TypeOf("WSF_NORAD_SPACE_MOVER"))
   {
      UtTwoLineElement tle(aObjectPtr->GetPropagator().GetOrbitalState().GetOrbitalElementsTOD());
      oss << tle;
      aReturnVal.SetString(oss.str());
   }
   else
   {
      // Use NORAD propagator inverter
      auto arg = ut::make_unique<WsfNORAD_OrbitalPropagator>(
         dynamic_cast<const WsfNORAD_OrbitalPropagator&>(aObjectPtr->GetPropagator()));
      WsfNORAD_PropagatorInverter inv(aObjectPtr->GetPropagator().GetOrbitalState(), std::move(arg));
      if (inv.SolutionFound())
      {
         oss << inv.GetTwoLineElement();
         aReturnVal.SetString(oss.str());
      }
      else
      {
         auto logger = ut::log::warning() << "Unable to compute mean-value TLE for platform.";
         logger.AddNote() << "Platform: " << aObjectPtr->GetPlatform()->GetName();
      }
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, SetTwoLineElement, 1, "bool", "string")
{
   std::string      tleStr = '\n' + aVarArgs[0].GetString();
   UtTwoLineElement tle;
   UtInput          input;
   input.PushInputString(tleStr);
   try
   {
      tle.ReadOrbitalData(input);
   }
   catch (UtInput::EndOfData&)
   {
      UT_SCRIPT_ABORT("Unable to read two-line element.");
   }
   //! For NORAD space movers, the TLE is TEME referenced; otherwise, TOD-referenced.
   if (aObjectPtr->IsA_TypeOf("WSF_NORAD_SPACE_MOVER"))
   {
      aObjectPtr->SetInitialOrbitalState(
         ut::OrbitalState(ut::OrbitalState::CoordinateSystem::cEQUATORIAL, ut::OrbitalState::ReferenceFrame::cTEME, tle));
   }
   else
   {
      aObjectPtr->SetInitialOrbitalState(ut::OrbitalState(ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                                          ut::OrbitalState::ReferenceFrame::cTRUE_OF_DATE,
                                                          tle));
   }
   aReturnVal.SetBool(aObjectPtr->Initialize(TIME_NOW));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, InitialTwoLineElement, 0, "string", "")
{
   auto               initialOrbitalState = aObjectPtr->GetPropagator().GetInitialOrbitalState();
   std::ostringstream oss;
   oss << initialOrbitalState.GetTwoLineElement();
   aReturnVal.SetString(oss.str());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, SatelliteNumber, 0, "int", "")
{
   auto initialOrbitalState = aObjectPtr->GetPropagator().GetInitialOrbitalState();
   if (initialOrbitalState.TwoLineElementValid() &&
       (initialOrbitalState.GetTwoLineElement().GetEphemerisType() != UtTwoLineElement::cOSCULATING))
   {
      aReturnVal.SetInt(initialOrbitalState.GetTwoLineElement().GetSatelliteNumber());
   }
   else
   {
      aReturnVal.SetInt(-1);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, Classification, 0, "string", "")
{
   auto initialOrbitalState = aObjectPtr->GetPropagator().GetInitialOrbitalState();
   if (initialOrbitalState.TwoLineElementValid() &&
       (initialOrbitalState.GetTwoLineElement().GetEphemerisType() != UtTwoLineElement::cOSCULATING))
   {
      char c = initialOrbitalState.GetTwoLineElement().GetClassification();
      aReturnVal.SetString(std::string(1, c));
   }
   else
   {
      aReturnVal.SetString("");
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, BallisticCoefficient, 0, "double", "")
{
   auto initialOrbitalState = aObjectPtr->GetPropagator().GetInitialOrbitalState();
   if (initialOrbitalState.TwoLineElementValid() &&
       (initialOrbitalState.GetTwoLineElement().GetEphemerisType() != UtTwoLineElement::cOSCULATING))
   {
      aReturnVal.SetDouble(initialOrbitalState.GetTwoLineElement().GetMeanMotionFirstDerivative());
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, BStar, 0, "double", "")
{
   auto initialOrbitalState = aObjectPtr->GetPropagator().GetInitialOrbitalState();
   if (initialOrbitalState.TwoLineElementValid() &&
       (initialOrbitalState.GetTwoLineElement().GetEphemerisType() != UtTwoLineElement::cOSCULATING))
   {
      aReturnVal.SetDouble(initialOrbitalState.GetTwoLineElement().GetBstarDrag());
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, ManeuverDuration, 1, "double", "double")
{
   double deltaV           = aVarArgs[0].GetDouble();
   double maneuverDuration = aObjectPtr->GetRequiredDuration(deltaV);
   aReturnVal.SetDouble(maneuverDuration);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, AvailableManeuverDuration, 0, "double", "")
{
   double maneuverDuration = aObjectPtr->GetRequiredDuration(aObjectPtr->GetAvailableDeltaV());
   aReturnVal.SetDouble(maneuverDuration);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, CurrentManeuver, 0, "WsfOrbitalManeuver", "")
{
   const WsfOrbitalEvent* currentManeuverPtr = aObjectPtr->GetCurrentMissionEvent();
   if (currentManeuverPtr != nullptr)
   {
      aReturnVal.SetPointer(new UtScriptRef(currentManeuverPtr->Clone(), aReturnClassPtr));
   }
   else
   {
      aReturnVal.SetPointer(new UtScriptRef(nullptr, aReturnClassPtr));
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, ExecuteManeuver, 1, "bool", "WsfOrbitalManeuver")
{
   auto maneuver            = aVarArgs[0].GetPointer()->GetAppObject<WsfOrbitalEvent>();
   auto maneuverSequencePtr = ut::make_unique<WsfOrbitalMissionSequence>(*SCENARIO);
   maneuverSequencePtr->AddMissionEvent(ut::clone(maneuver));
   UtCalendar currentTime(SIMULATION->GetDateTime().GetCurrentTime(TIME_NOW));
   maneuverSequencePtr->SetStartTime(currentTime);
   auto verifyResult = aObjectPtr->VerifyMission(currentTime, *maneuverSequencePtr);
   if (verifyResult)
   {
      aObjectPtr->ScheduleMission(currentTime, move(maneuverSequencePtr));
   }
   aReturnVal.SetBool(verifyResult);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, ExecuteManeuverSequence, 1, "bool", "WsfOrbitalMissionSequence")
{
   UtCalendar currentTime(SIMULATION->GetDateTime().GetCurrentTime(WsfScriptContext::GetTIME_NOW(aContext)));
   auto*      maneuverSequencePtr = static_cast<WsfOrbitalMissionSequence*>(aVarArgs[0].GetPointer()->GetAppObject());
   maneuverSequencePtr->SetStartTime(currentTime);
   auto verifyResult = aObjectPtr->VerifyMission(currentTime, *maneuverSequencePtr);
   if (verifyResult)
   {
      aObjectPtr->ScheduleMission(currentTime, move(ut::make_unique<WsfOrbitalMissionSequence>(*maneuverSequencePtr)));
   }
   aReturnVal.SetBool(verifyResult);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, CurrentOrbitalEvent, 0, "WsfOrbitalEvent", "")
{
   const WsfOrbitalEvent* currentOrbitalEventPtr = aObjectPtr->GetCurrentMissionEvent();
   if (currentOrbitalEventPtr != nullptr)
   {
      aReturnVal.SetPointer(new UtScriptRef(currentOrbitalEventPtr->Clone(), aReturnClassPtr));
   }
   else
   {
      aReturnVal.SetPointer(new UtScriptRef(nullptr, aReturnClassPtr));
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, ExecuteOrbitalEvent, 1, "bool", "WsfOrbitalEvent")
{
   auto orbitalEvent            = aVarArgs[0].GetPointer()->GetAppObject<WsfOrbitalEvent>();
   auto orbitalEventSequencePtr = ut::make_unique<WsfOrbitalMissionSequence>(*SCENARIO);
   orbitalEventSequencePtr->AddMissionEvent(ut::clone(orbitalEvent));
   UtCalendar currentTime(SIMULATION->GetDateTime().GetCurrentTime(TIME_NOW));
   orbitalEventSequencePtr->SetStartTime(currentTime);
   auto verifyResult = aObjectPtr->VerifyMission(currentTime, *orbitalEventSequencePtr);
   if (verifyResult)
   {
      aObjectPtr->ScheduleMission(currentTime, move(orbitalEventSequencePtr));
   }
   aReturnVal.SetBool(verifyResult);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, ExecuteMissionSequence, 1, "bool", "WsfOrbitalMissionSequence")
{
   auto*      missionSequencePtr = static_cast<WsfOrbitalMissionSequence*>(aVarArgs[0].GetPointer()->GetAppObject());
   UtCalendar currentTime(SIMULATION->GetDateTime().GetCurrentTime(TIME_NOW));
   missionSequencePtr->SetStartTime(currentTime);
   auto verifyResult = aObjectPtr->VerifyMission(currentTime, *missionSequencePtr);
   if (verifyResult)
   {
      aObjectPtr->ScheduleMission(currentTime, move(ut::make_unique<WsfOrbitalMissionSequence>(*missionSequencePtr)));
   }
   aReturnVal.SetBool(verifyResult);
}

// deprecated
UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass,
                        WsfSpaceMoverBase,
                        ComputeRendezvousDeprecated,
                        4,
                        "Array<double>",
                        "WsfTrackId, double, double, bool")
{
   { // RAII block
      auto msg = ut::log::warning() << "Method 'ComputeRendezvous' accepting a WsfTrackId argument is deprecated.";
      msg.AddNote() << "Please use the form accepting a WsfTargetPoint argument.";
   }

   auto   trackIdPtr     = static_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   double maxDeltaT      = aVarArgs[1].GetDouble();
   double maxDeltaV      = aVarArgs[2].GetDouble();
   bool   minimizeDeltaV = aVarArgs[3].GetBool();

   double deltaT;
   double deltaV;
   aObjectPtr->GetPlatform()->Update(WsfScriptContext::GetTIME_NOW(aContext));
   bool found =
      PerformTargetingComputation(*aObjectPtr, *trackIdPtr, maxDeltaT, maxDeltaV, minimizeDeltaV, true, deltaT, deltaV);

   std::vector<UtScriptData>* returnDataPtr = new std::vector<UtScriptData>();
   if (found)
   {
      returnDataPtr->emplace_back(deltaT);
      returnDataPtr->emplace_back(deltaV);
   }
   aReturnVal.SetPointer(new UtScriptRef(returnDataPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// deprecated
UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass,
                        WsfSpaceMoverBase,
                        ComputeInterceptDeprecated,
                        4,
                        "Array<double>",
                        "WsfTrackId, double, double, bool")
{
   { // RAII block
      auto msg = ut::log::warning() << "Method 'ComputeIntercept' accepting a WsfTrackId argument is deprecated.";
      msg.AddNote() << "Please use the form accepting a WsfTargetPoint argument.";
   }

   auto   trackIdPtr     = static_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   double maxDeltaT      = aVarArgs[1].GetDouble();
   double maxDeltaV      = aVarArgs[2].GetDouble();
   bool   minimizeDeltaV = aVarArgs[3].GetBool();

   double deltaT;
   double deltaV;
   aObjectPtr->GetPlatform()->Update(WsfScriptContext::GetTIME_NOW(aContext));
   bool found =
      PerformTargetingComputation(*aObjectPtr, *trackIdPtr, maxDeltaT, maxDeltaV, minimizeDeltaV, false, deltaT, deltaV);

   std::vector<UtScriptData>* returnDataPtr = new std::vector<UtScriptData>();
   if (found)
   {
      returnDataPtr->emplace_back(deltaT);
      returnDataPtr->emplace_back(deltaV);
   }
   aReturnVal.SetPointer(new UtScriptRef(returnDataPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass,
                        WsfSpaceMoverBase,
                        ComputeRendezvousTrackOptimized,
                        4,
                        "Array<double>",
                        "WsfTrack, double, double, bool")
{
   auto   trackPtr       = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   double maxDeltaT      = aVarArgs[1].GetDouble();
   double maxDeltaV      = aVarArgs[2].GetDouble();
   bool   minimizeDeltaV = aVarArgs[3].GetBool();

   aObjectPtr->GetPlatform()->Update(WsfScriptContext::GetTIME_NOW(aContext));
   const auto tgt = MakeManeuverTarget(*aObjectPtr, *trackPtr);
   double     deltaT;
   double     deltaV;
   bool       found = ComputeOptimizedSolution(tgt, maxDeltaT, maxDeltaV, minimizeDeltaV, true, deltaT, deltaV);

   std::vector<UtScriptData>* returnDataPtr = new std::vector<UtScriptData>();
   if (found)
   {
      returnDataPtr->emplace_back(deltaT);
      returnDataPtr->emplace_back(deltaV);
   }
   aReturnVal.SetPointer(new UtScriptRef(returnDataPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass,
                        WsfSpaceMoverBase,
                        ComputeInterceptTrackOptimized,
                        4,
                        "Array<double>",
                        "WsfTrack, double, double, bool")
{
   auto   trackPtr       = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   double maxDeltaT      = aVarArgs[1].GetDouble();
   double maxDeltaV      = aVarArgs[2].GetDouble();
   bool   minimizeDeltaV = aVarArgs[3].GetBool();

   aObjectPtr->GetPlatform()->Update(WsfScriptContext::GetTIME_NOW(aContext));
   const auto tgt = MakeManeuverTarget(*aObjectPtr, *trackPtr);
   double     deltaT;
   double     deltaV;
   bool       found = ComputeOptimizedSolution(tgt, maxDeltaT, maxDeltaV, minimizeDeltaV, false, deltaT, deltaV);

   std::vector<UtScriptData>* returnDataPtr = new std::vector<UtScriptData>();
   if (found)
   {
      returnDataPtr->emplace_back(deltaT);
      returnDataPtr->emplace_back(deltaV);
   }
   aReturnVal.SetPointer(new UtScriptRef(returnDataPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, ComputeRendezvousTrackFixed, 2, "Array<double>", "WsfTrack, double")
{
   auto   trackPtr    = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   double fixedDeltaT = aVarArgs[1].GetDouble();

   aObjectPtr->GetPlatform()->Update(WsfScriptContext::GetTIME_NOW(aContext));
   const auto tgt = MakeManeuverTarget(*aObjectPtr, *trackPtr);
   double     deltaT;
   double     deltaV;
   bool       found = ComputeFixedTimeSolution(tgt, fixedDeltaT, true, deltaT, deltaV);

   std::vector<UtScriptData>* returnDataPtr = new std::vector<UtScriptData>();
   if (found)
   {
      returnDataPtr->emplace_back(deltaT);
      returnDataPtr->emplace_back(deltaV);
   }
   aReturnVal.SetPointer(new UtScriptRef(returnDataPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, ComputeInterceptTrackFixed, 2, "Array<double>", "WsfTrack, double")
{
   auto   trackPtr    = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   double fixedDeltaT = aVarArgs[1].GetDouble();

   aObjectPtr->GetPlatform()->Update(WsfScriptContext::GetTIME_NOW(aContext));
   const auto tgt = MakeManeuverTarget(*aObjectPtr, *trackPtr);
   double     deltaT;
   double     deltaV;
   bool       found = ComputeFixedTimeSolution(tgt, fixedDeltaT, false, deltaT, deltaV);

   std::vector<UtScriptData>* returnDataPtr = new std::vector<UtScriptData>();
   if (found)
   {
      returnDataPtr->emplace_back(deltaT);
      returnDataPtr->emplace_back(deltaV);
   }
   aReturnVal.SetPointer(new UtScriptRef(returnDataPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass,
                        WsfSpaceMoverBase,
                        ComputeRendezvousTargetPointOptimized,
                        4,
                        "Array<double>",
                        "WsfTargetPoint, double, double, bool")
{
   auto   targetPointPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::TargetPointOptions>();
   double maxDeltaT      = aVarArgs[1].GetDouble();
   double maxDeltaV      = aVarArgs[2].GetDouble();
   bool   minimizeDeltaV = aVarArgs[3].GetBool();

   auto returnDataPtr = new std::vector<UtScriptData>();
   try
   {
      aObjectPtr->GetPlatform()->Update(WsfScriptContext::GetTIME_NOW(aContext));
      const auto tgt = MakeManeuverTarget(*aObjectPtr, *targetPointPtr);
      double     deltaT;
      double     deltaV;
      bool       found = ComputeOptimizedSolution(tgt, maxDeltaT, maxDeltaV, minimizeDeltaV, true, deltaT, deltaV);

      if (found)
      {
         returnDataPtr->emplace_back(deltaT);
         returnDataPtr->emplace_back(deltaV);
      }
   }
   catch (std::runtime_error& err)
   {
      auto msg = ut::log::error() << "Unable to compute rendezvous.";
      msg << err.what();
   }
   aReturnVal.SetPointer(new UtScriptRef(returnDataPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass,
                        WsfSpaceMoverBase,
                        ComputeRendezvousTargetPointCost,
                        4,
                        "Array<double>",
                        "WsfTargetPoint, double, double, WsfOrbitalOptimizationCost")
{
   auto   targetPointPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::TargetPointOptions>();
   double maxDeltaT      = aVarArgs[1].GetDouble();
   double maxDeltaV      = aVarArgs[2].GetDouble();
   auto   costPtr        = aVarArgs[3].GetPointer()->GetAppObject<wsf::space::OrbitalTargetingCost>();

   auto returnDataPtr = new std::vector<UtScriptData>();
   try
   {
      aObjectPtr->GetPlatform()->Update(WsfScriptContext::GetTIME_NOW(aContext));
      const auto tgt = MakeManeuverTarget(*aObjectPtr, *targetPointPtr);
      double     deltaT;
      double     deltaV;
      bool       found = ComputeOptimizedSolution(tgt, maxDeltaT, maxDeltaV, *costPtr, true, deltaT, deltaV);

      if (found)
      {
         returnDataPtr->emplace_back(deltaT);
         returnDataPtr->emplace_back(deltaV);
      }
   }
   catch (std::runtime_error& err)
   {
      auto msg = ut::log::error() << "Unable to compute rendezvous.";
      msg.AddNote() << err.what();
   }
   aReturnVal.SetPointer(new UtScriptRef(returnDataPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass,
                        WsfSpaceMoverBase,
                        ComputeRendezvousTargetPointFixed,
                        2,
                        "Array<double>",
                        "WsfTargetPoint, double")
{
   auto   targetPointPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::TargetPointOptions>();
   double fixedDeltaT    = aVarArgs[1].GetDouble();

   auto returnDataPtr = new std::vector<UtScriptData>();
   try
   {
      aObjectPtr->GetPlatform()->Update(WsfScriptContext::GetTIME_NOW(aContext));
      const auto tgt = MakeManeuverTarget(*aObjectPtr, *targetPointPtr);
      double     deltaT;
      double     deltaV;
      bool       found = ComputeFixedTimeSolution(tgt, fixedDeltaT, true, deltaT, deltaV);

      if (found)
      {
         returnDataPtr->emplace_back(deltaT);
         returnDataPtr->emplace_back(deltaV);
      }
   }
   catch (std::runtime_error& err)
   {
      auto msg = ut::log::error() << "Unable to compute rendezvous.";
      msg << err.what();
   }
   aReturnVal.SetPointer(new UtScriptRef(returnDataPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass,
                        WsfSpaceMoverBase,
                        ComputeInterceptTargetPointOptimized,
                        4,
                        "Array<double>",
                        "WsfTargetPoint, double, double, bool")
{
   auto   targetPointPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::TargetPointOptions>();
   double maxDeltaT      = aVarArgs[1].GetDouble();
   double maxDeltaV      = aVarArgs[2].GetDouble();
   bool   minimizeDeltaV = aVarArgs[3].GetBool();

   auto returnDataPtr = new std::vector<UtScriptData>();
   try
   {
      aObjectPtr->GetPlatform()->Update(WsfScriptContext::GetTIME_NOW(aContext));
      const auto tgt = MakeManeuverTarget(*aObjectPtr, *targetPointPtr);
      double     deltaT;
      double     deltaV;
      bool       found = ComputeOptimizedSolution(tgt, maxDeltaT, maxDeltaV, minimizeDeltaV, false, deltaT, deltaV);

      if (found)
      {
         returnDataPtr->emplace_back(deltaT);
         returnDataPtr->emplace_back(deltaV);
      }
   }
   catch (std::runtime_error& err)
   {
      auto msg = ut::log::error() << "Unable to compute intercept.";
      msg.AddNote() << err.what();
   }
   aReturnVal.SetPointer(new UtScriptRef(returnDataPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass,
                        WsfSpaceMoverBase,
                        ComputeInterceptTargetPointCost,
                        4,
                        "Array<double>",
                        "WsfTargetPoint, double, double, WsfOrbitalOptimizationCost")
{
   auto   targetPointPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::TargetPointOptions>();
   double maxDeltaT      = aVarArgs[1].GetDouble();
   double maxDeltaV      = aVarArgs[2].GetDouble();
   auto   costPtr        = aVarArgs[3].GetPointer()->GetAppObject<wsf::space::OrbitalTargetingCost>();

   auto returnDataPtr = new std::vector<UtScriptData>();
   try
   {
      aObjectPtr->GetPlatform()->Update(WsfScriptContext::GetTIME_NOW(aContext));
      const auto tgt = MakeManeuverTarget(*aObjectPtr, *targetPointPtr);
      double     deltaT;
      double     deltaV;
      bool       found = ComputeOptimizedSolution(tgt, maxDeltaT, maxDeltaV, *costPtr, false, deltaT, deltaV);

      if (found)
      {
         returnDataPtr->emplace_back(deltaT);
         returnDataPtr->emplace_back(deltaV);
      }
   }
   catch (std::runtime_error& err)
   {
      auto msg = ut::log::error() << "Unable to compute intercept.";
      msg.AddNote() << err.what();
   }
   aReturnVal.SetPointer(new UtScriptRef(returnDataPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass,
                        WsfSpaceMoverBase,
                        ComputeInterceptTargetPointFixed,
                        2,
                        "Array<double>",
                        "WsfTargetPoint, double")
{
   auto   targetPointPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::TargetPointOptions>();
   double fixedDeltaT    = aVarArgs[1].GetDouble();

   auto returnDataPtr = new std::vector<UtScriptData>();
   try
   {
      aObjectPtr->GetPlatform()->Update(WsfScriptContext::GetTIME_NOW(aContext));
      const auto tgt = MakeManeuverTarget(*aObjectPtr, *targetPointPtr);
      double     deltaT;
      double     deltaV;
      bool       found = ComputeFixedTimeSolution(tgt, fixedDeltaT, false, deltaT, deltaV);

      if (found)
      {
         returnDataPtr->emplace_back(deltaT);
         returnDataPtr->emplace_back(deltaV);
      }
   }
   catch (std::runtime_error& err)
   {
      auto msg = ut::log::error() << "Unable to compute intercept.";
      msg.AddNote() << err.what();
   }
   aReturnVal.SetPointer(new UtScriptRef(returnDataPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, CancelMissionSequence, 0, "void", "")
{
   aObjectPtr->CancelMission();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, Descriptor, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetDescriptor());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, Designator, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetDesignator());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, SetOrbitColor, 1, "void", "Color")
{
   auto timeNow = WsfScriptContext::GetTIME_NOW(aContext);
   aObjectPtr->UpdateColor(timeNow, *aVarArgs[0].GetPointer()->GetAppObject<UtColor>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, GetOrbitColor, 0, "Color", "")
{
   aReturnVal.SetPointer(UtScriptColor::Create(aObjectPtr->GetOrbitColorValue()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, RIC_1, 1, "Vec3", "WsfPlatform")
{
   double simTime  = WsfScriptContext::GetTIME_NOW(aContext);
   auto   platform = aVarArgs[0].GetPointer()->GetAppObject<WsfPlatform>();
   platform->Update(simTime);

   UtVec3d platformLocECI;
   platform->GetLocationECI(platformLocECI.GetData());

   UtCalendar argCal = aObjectPtr->GetSimulation()->GetDateTime().GetCurrentTime(simTime);

   UtOrbitalPropagatorBase& prop = aObjectPtr->GetPropagator();
   prop.Update(argCal);

   UtVec3d propPos = prop.GetOrbitalState().GetOrbitalStateVectorInertial().GetLocation();
   UtVec3d propVel = prop.GetOrbitalState().GetOrbitalStateVectorInertial().GetVelocity();

   UtVec3d ric = ut::RIC_Frame::CalculateToRIC(propPos, propVel, platformLocECI);
   aReturnVal.SetPointer(UtScriptVec3::Create(ric));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, RIC_2, 1, "Vec3", "WsfTrack")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   auto   track   = aVarArgs[0].GetPointer()->GetAppObject<WsfTrack>();
   track->Update(simTime);

   UtVec3d trackLocWCS;
   track->GetExtrapolatedLocationWCS(simTime, trackLocWCS.GetData());

   auto       dateTime = aObjectPtr->GetSimulation()->GetDateTime();
   UtCalendar startCal = dateTime.GetStartDateAndTime();
   UtCalendar argCal   = dateTime.GetCurrentTime(simTime);

   UtVec3d          trackLocECI;
   UtECI_Conversion converter(startCal, ut::EarthWGS84());
   converter.SetTime(simTime);
   converter.ConvertWCS_ToECI(trackLocWCS.GetData(), trackLocECI.GetData());

   UtOrbitalPropagatorBase& prop = aObjectPtr->GetPropagator();
   prop.Update(argCal);

   UtVec3d propPos = prop.GetOrbitalState().GetOrbitalStateVectorInertial().GetLocation();
   UtVec3d propVel = prop.GetOrbitalState().GetOrbitalStateVectorInertial().GetVelocity();

   UtVec3d ric = ut::RIC_Frame::CalculateToRIC(propPos, propVel, trackLocECI);
   aReturnVal.SetPointer(UtScriptVec3::Create(ric));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, RIC_AtTime_1, 2, "Vec3", "double,WsfPlatform")
{
   double simTime  = aVarArgs[0].GetDouble();
   auto   platform = aVarArgs[1].GetPointer()->GetAppObject<WsfPlatform>();

   UtVec3d               platformLocECI;
   ut::optional<UtVec3d> platformLocWCS;

   auto       dateTime = aObjectPtr->GetSimulation()->GetDateTime();
   UtCalendar argCal   = dateTime.GetCurrentTime(simTime);
   UtCalendar nowCal   = dateTime.GetCurrentTime(WsfScriptContext::GetTIME_NOW(aContext));

   WsfMover* moverPtr = platform->GetMover();
   if (moverPtr == nullptr)
   {
      platformLocWCS = UtVec3d(platform->GetLocationWCS());
   }
   else
   {
      WsfSpaceMoverBase* spaceMoverPtr = dynamic_cast<WsfSpaceMoverBase*>(moverPtr);

      if (spaceMoverPtr != nullptr)
      {
         UtOrbitalPropagatorBase& prop = spaceMoverPtr->GetPropagator();

         prop.Update(argCal);
         platformLocECI = prop.GetOrbitalState().GetOrbitalStateVectorInertial().GetLocation();
         prop.Update(nowCal);
      }
      else if (!moverPtr->GetFutureLocationWCS(simTime, platformLocWCS.emplace().GetData()))
      {
         // If unable to get the future location of the platform, settle for its current location.
         platformLocWCS = UtVec3d(platform->GetLocationWCS());
      }
   }

   if (platformLocWCS)
   {
      UtCalendar       startCal = dateTime.GetStartDateAndTime();
      UtECI_Conversion converter(startCal, aObjectPtr->GetPlatform()->GetCentralBody());
      converter.SetTime(simTime);
      platformLocECI = converter.ConvertWCS_ToECI(platformLocWCS.value());
   }

   UtOrbitalPropagatorBase& prop = aObjectPtr->GetPropagator();
   // Update propagator to selected time
   prop.Update(argCal);

   UtVec3d propPos = prop.GetOrbitalState().GetOrbitalStateVectorInertial().GetLocation();
   UtVec3d propVel = prop.GetOrbitalState().GetOrbitalStateVectorInertial().GetVelocity();

   UtVec3d ric = ut::RIC_Frame::CalculateToRIC(propPos, propVel, platformLocECI);

   // Reset propagator to current sim time
   prop.Update(nowCal);

   aReturnVal.SetPointer(UtScriptVec3::Create(ric));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, RIC_AtTime_2, 2, "Vec3", "double,WsfTrack")
{
   double simTime = aVarArgs[0].GetDouble();
   auto   track   = aVarArgs[1].GetPointer()->GetAppObject<WsfTrack>();

   UtVec3d trackLocWCS;
   track->GetExtrapolatedLocationWCS(simTime, trackLocWCS.GetData());

   auto       dateTime = aObjectPtr->GetSimulation()->GetDateTime();
   UtCalendar startCal = dateTime.GetStartDateAndTime();
   UtCalendar argCal   = dateTime.GetCurrentTime(simTime);
   UtCalendar nowCal   = dateTime.GetCurrentTime(WsfScriptContext::GetTIME_NOW(aContext));

   UtVec3d          trackLocECI;
   UtECI_Conversion converter(startCal, ut::EarthWGS84());
   converter.SetTime(simTime);
   converter.ConvertWCS_ToECI(trackLocWCS.GetData(), trackLocECI.GetData());

   UtOrbitalPropagatorBase& prop = aObjectPtr->GetPropagator();
   prop.Update(argCal);

   UtVec3d propPos = prop.GetOrbitalState().GetOrbitalStateVectorInertial().GetLocation();
   UtVec3d propVel = prop.GetOrbitalState().GetOrbitalStateVectorInertial().GetVelocity();

   UtVec3d ric = ut::RIC_Frame::CalculateToRIC(propPos, propVel, trackLocECI);

   prop.Update(nowCal);

   aReturnVal.SetPointer(UtScriptVec3::Create(ric));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, LookAngleTo_Track, 1, "double", "WsfTrack")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   auto trackPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfTrack>();
   trackPtr->KinematicUpdate(TIME_NOW);
   double retval{-360.0};
   if (trackPtr->LocationValid())
   {
      UtVec3d locWCS;
      trackPtr->GetLocationWCS(locWCS.GetData());
      retval = aObjectPtr->GetLookAngleTo(locWCS) * UtMath::cDEG_PER_RAD;
   }
   else
   {
      auto msg = ut::log::warning() << "Argument to WsfSpaceMover.LookAngleTo is invalid.";
      msg.AddNote() << "Method called on mover for platform `" << aObjectPtr->GetPlatform()->GetName() << "'.";
      msg.AddNote() << "Provided WsfTrack does not have a valid location: " << trackPtr->GetTrackId();
   }
   aReturnVal.SetDouble(retval);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, LookAngleTo_Platform, 1, "double", "WsfPlatform")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   auto platformPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfPlatform>();
   platformPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetLookAngleTo(UtVec3d(platformPtr->GetLocationWCS())) * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, LookAngleTo_LLA, 3, "double", "double, double, double")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   double      lat = aVarArgs[0].GetDouble();
   double      lon = aVarArgs[1].GetDouble();
   double      alt = aVarArgs[2].GetDouble();
   WsfGeoPoint point{lat, lon, alt};
   UtVec3d     locWCS;
   point.GetLocationWCS(locWCS.GetData());
   aReturnVal.SetDouble(aObjectPtr->GetLookAngleTo(locWCS) * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, LookAngleTo_GeoPoint, 1, "double", "WsfGeoPoint")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   auto    geoPointPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfGeoPoint>();
   UtVec3d locWCS;
   geoPointPtr->GetLocationWCS(locWCS.GetData());
   aReturnVal.SetDouble(aObjectPtr->GetLookAngleTo(locWCS) * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, IncidenceAngle_Track, 1, "double", "WsfTrack")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   auto trackPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfTrack>();
   trackPtr->KinematicUpdate(TIME_NOW);
   double retval{-360.0};
   if (trackPtr->LocationValid())
   {
      UtVec3d locWCS;
      trackPtr->GetLocationWCS(locWCS.GetData());
      retval = aObjectPtr->GetIncidenceAngle(locWCS) * UtMath::cDEG_PER_RAD;
   }
   else
   {
      auto msg = ut::log::warning() << "Argument to WsfSpaceMover.IncidenceAngle is invalid.";
      msg.AddNote() << "Method called on mover for platform `" << aObjectPtr->GetPlatform()->GetName() << "'.";
      msg.AddNote() << "Provided WsfTrack does not have a valid location: " << trackPtr->GetTrackId();
   }
   aReturnVal.SetDouble(retval);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, IncidenceAngle_Platform, 1, "double", "WsfPlatform")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   auto platformPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfPlatform>();
   platformPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetIncidenceAngle(UtVec3d(platformPtr->GetLocationWCS())) * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, IncidenceAngle_LLA, 3, "double", "double, double, double")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   double      lat = aVarArgs[0].GetDouble();
   double      lon = aVarArgs[1].GetDouble();
   double      alt = aVarArgs[2].GetDouble();
   WsfGeoPoint point{lat, lon, alt};
   UtVec3d     locWCS;
   point.GetLocationWCS(locWCS.GetData());
   aReturnVal.SetDouble(aObjectPtr->GetIncidenceAngle(locWCS) * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, IncidenceAngle_GeoPoint, 1, "double", "WsfGeoPoint")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   auto    geoPointPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfGeoPoint>();
   UtVec3d locWCS;
   geoPointPtr->GetLocationWCS(locWCS.GetData());
   aReturnVal.SetDouble(aObjectPtr->GetIncidenceAngle(locWCS) * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, TargetElevationAngle_Track, 1, "double", "WsfTrack")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   auto trackPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfTrack>();
   trackPtr->KinematicUpdate(TIME_NOW);
   double retval{-360.0};
   if (trackPtr->LocationValid())
   {
      UtVec3d locWCS;
      trackPtr->GetLocationWCS(locWCS.GetData());
      retval = aObjectPtr->GetTargetElevationAngle(locWCS) * UtMath::cDEG_PER_RAD;
   }
   else
   {
      auto msg = ut::log::warning() << "Argument to WsfSpaceMover.TargetElevationAngle is invalid.";
      msg.AddNote() << "Method called on mover for platform `" << aObjectPtr->GetPlatform()->GetName() << "'.";
      msg.AddNote() << "Provided WsfTrack does not have a valid location: " << trackPtr->GetTrackId();
   }
   aReturnVal.SetDouble(retval);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, TargetElevationAngle_Platform, 1, "double", "WsfPlatform")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   auto platformPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfPlatform>();
   platformPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetTargetElevationAngle(UtVec3d(platformPtr->GetLocationWCS())) * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, TargetElevationAngle_LLA, 3, "double", "double, double, double")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   double      lat = aVarArgs[0].GetDouble();
   double      lon = aVarArgs[1].GetDouble();
   double      alt = aVarArgs[2].GetDouble();
   WsfGeoPoint point{lat, lon, alt};
   UtVec3d     locWCS;
   point.GetLocationWCS(locWCS.GetData());
   aReturnVal.SetDouble(aObjectPtr->GetTargetElevationAngle(locWCS) * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, TargetElevationAngle_GeoPoint, 1, "double", "WsfGeoPoint")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   auto    geoPointPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfGeoPoint>();
   UtVec3d locWCS;
   geoPointPtr->GetLocationWCS(locWCS.GetData());
   aReturnVal.SetDouble(aObjectPtr->GetTargetElevationAngle(locWCS) * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, SquintAngleTo_Track, 1, "double", "WsfTrack")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   auto trackPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfTrack>();
   trackPtr->KinematicUpdate(TIME_NOW);
   double retval{-360.0};
   if (trackPtr->LocationValid())
   {
      UtVec3d locWCS;
      trackPtr->GetLocationWCS(locWCS.GetData());
      retval = aObjectPtr->GetSquintAngleTo(locWCS) * UtMath::cDEG_PER_RAD;
   }
   else
   {
      auto msg = ut::log::warning() << "Argument to WsfSpaceMover.SquintAngleTo is invalid.";
      msg.AddNote() << "Method called on mover for platform `" << aObjectPtr->GetPlatform()->GetName() << "'.";
      msg.AddNote() << "Provided WsfTrack does not have a valid location: " << trackPtr->GetTrackId();
   }
   aReturnVal.SetDouble(retval);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, SquintAngleTo_Platform, 1, "double", "WsfPlatform")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   auto platformPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfPlatform>();
   platformPtr->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetSquintAngleTo(UtVec3d(platformPtr->GetLocationWCS())) * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, SquintAngleTo_LLA, 3, "double", "double, double, double")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   double      lat = aVarArgs[0].GetDouble();
   double      lon = aVarArgs[1].GetDouble();
   double      alt = aVarArgs[2].GetDouble();
   WsfGeoPoint point{lat, lon, alt};
   UtVec3d     locWCS;
   point.GetLocationWCS(locWCS.GetData());
   aReturnVal.SetDouble(aObjectPtr->GetSquintAngleTo(locWCS) * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, SquintAngleTo_GeoPoint, 1, "double", "WsfGeoPoint")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   auto    geoPointPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfGeoPoint>();
   UtVec3d locWCS;
   geoPointPtr->GetLocationWCS(locWCS.GetData());
   aReturnVal.SetDouble(aObjectPtr->GetSquintAngleTo(locWCS) * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, DistanceToHorizon, 0, "double", "")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetDistanceToHorizon());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, LookAngleToHorizon, 0, "double", "")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetLookAngleToHorizon() * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSpaceMoverClass, WsfSpaceMoverBase, SetInitialOrbitalState, 1, "void", "OrbitalState")
{
   auto orbitalState = aVarArgs[0].GetPointer()->GetAppObject<ut::OrbitalState>();
   aObjectPtr->SetInitialOrbitalState(*orbitalState);
}
