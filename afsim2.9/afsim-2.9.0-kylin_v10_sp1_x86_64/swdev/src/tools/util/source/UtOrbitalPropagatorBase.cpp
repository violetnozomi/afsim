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
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "UtOrbitalPropagatorBase.hpp"

#include <iomanip>
#include <iostream>
#include <string>

#include "UtAngle.hpp"
#include "UtCalendar.hpp"
#include "UtEarth.hpp"
#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLatPos.hpp"
#include "UtLog.hpp"
#include "UtLonPos.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtOrbitalElements.hpp"
#include "UtTime.hpp"
#include "UtVec3.hpp"

UtOrbitalPropagatorBase::UtOrbitalPropagatorBase()
   : mInitialOrbitalStatePtr(ut::make_unique<ut::OrbitalState>())
{
}

UtOrbitalPropagatorBase::UtOrbitalPropagatorBase(std::unique_ptr<ut::OrbitalState> aInitialOrbitalStatePtr)
   : mInitialOrbitalStatePtr(std::move(aInitialOrbitalStatePtr))
   , mOrbitalStatePtr(mInitialOrbitalStatePtr)
{
}

// virtual
bool UtOrbitalPropagatorBase::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "orbital_state")
   {
      UtInputBlock inputBlock(aInput);
      mInitialOrbitalStatePtr->SetDeclaredWithOrbitalState(true);
      inputBlock.ProcessInput(mInitialOrbitalStatePtr.get());
   }
   else if (mInitialOrbitalStatePtr->ProcessInput(aInput))
   {
   }
   else if (command == "debug_output_stk")
   {
      auto warning = ut::log::warning() << "Command is deprecated.";
      warning.AddNote() << "Command: " << command;
      warning.AddNote() << "Location: " << aInput.GetLocation();
      mDebugOutputSTK = EvaluateDebugInput(aInput);
   }
   else if (command == "debug_output_wsf")
   {
      auto warning = ut::log::warning() << "Command is deprecated.";
      warning.AddNote() << "Command: " << command;
      warning.AddNote() << "Location: " << aInput.GetLocation();
      mDebugOutputWSF = EvaluateDebugInput(aInput);
   }
   else if (command == "debug_output_oe")
   {
      auto warning = ut::log::warning() << "Command is deprecated.";
      warning.AddNote() << "Command: " << command;
      warning.AddNote() << "Location: " << aInput.GetLocation();
      mDebugOutputOE = EvaluateDebugInput(aInput);
   }
   else if (command == "debug_output_xyz")
   {
      auto warning = ut::log::warning() << "Command is deprecated.";
      warning.AddNote() << "Command: " << command;
      warning.AddNote() << "Location: " << aInput.GetLocation();
      mDebugOutputXYZ = EvaluateDebugInput(aInput);
   }
   else if (command == "debug_output_tle")
   {
      auto warning = ut::log::warning() << "Command is deprecated.";
      warning.AddNote() << "Command: " << command;
      warning.AddNote() << "Location: " << aInput.GetLocation();
      mDebugOutputOE = EvaluateDebugInput(aInput);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

//! Return the orbital state associated with this propagator instance.
//! @note Do not store a pointer or reference to the orbital state, as the underlying pointer may be replaced.
//! @note Do not call this method before UtOrbitalPropagatorBase::Initialize or derived class' Initialize.
const ut::OrbitalState& UtOrbitalPropagatorBase::GetOrbitalState() const
{
   return *mOrbitalStatePtr;
}

//! Set the initial orbital state (usually performed during initialization).
//! @note Be careful using this method.  The caller should check that the specific orbital element type is the one used
//! by the propagator.
bool UtOrbitalPropagatorBase::SetInitialOrbitalState(const ut::OrbitalState& aOrbitalState)
{
   mInitialOrbitalStatePtr = ut::clone(aOrbitalState);
   return true;
}

//! Return the central point as a central body.  If the central point is not a ut::CentralBody, an exception will be thrown.
const ut::CentralBody& UtOrbitalPropagatorBase::GetCentralBody() const
{
   return mInitialOrbitalStatePtr->GetCentralPoint().GetAsCentralBody();
}

//! Deprecated version of Update().
void UtOrbitalPropagatorBase::Update(double aTimePastEpoch)
{
   if (mTimePastEpoch != aTimePastEpoch)
   {
      UpdateTime(aTimePastEpoch);
      Propagate(mCurrentTime);
      PostPropagate();
   }
}

//! Template (non-virtual) method, calling UpdateTime and virtual Propagate methods.
void UtOrbitalPropagatorBase::Update(const UtCalendar& aTime)
{
   if (mCurrentTime != aTime)
   {
      UpdateTime(aTime);
      Propagate(aTime);
      PostPropagate();
   }
}

// virtual
//! Update the current time and time past epoch.
//! @param aTime The time to which we should update.
void UtOrbitalPropagatorBase::UpdateTime(const UtCalendar& aTime)
{
   mLastUpdateTime = mCurrentTime;
   mTimePastEpoch += aTime.GetTimeSince(mCurrentTime);
   mCurrentTime = aTime;
}

//! Set the output orbital state from a propagator's internally propagated state.
//! @param aState The propagated state used to update the output orbital state.
void UtOrbitalPropagatorBase::SetOrbitalState(const ut::OrbitalState& aState)
{
   if (mOrbitalStatePtr != nullptr)
   {
      mOrbitalStatePtr->Set(aState);
   }
   else
   {
      mOrbitalStatePtr = ut::clone(aState);
   }
}

//! Update the current time and time past epoch.
//! (deprecated).
void UtOrbitalPropagatorBase::UpdateTime(double aTimePastEpoch)
{
   mLastUpdateTime = mCurrentTime;
   double dt       = aTimePastEpoch - mTimePastEpoch;
   mCurrentTime.AdvanceTimeBy(dt);
   mTimePastEpoch = aTimePastEpoch;
}

// virtual
//! Initialize all propagator-dependent attributes.
//! It is assumed for this method that the initial orbital state has been provided, either in a call
//! to SetInitialOrbitalState, or passed in as a parameter to the constructor.
bool UtOrbitalPropagatorBase::Initialize(const UtCalendar& aEpoch)
{
   bool ok = false;

   if ((mInitialOrbitalStatePtr != nullptr) && (mInitialOrbitalStatePtr->IsValid()))
   {
      if (HyperbolicPropagationAllowed() || (!mInitialOrbitalStatePtr->OrbitIsHyperbolic()))
      {
         if (mInitialOrbitalStatePtr->HasEpoch())
         {
            mCurrentTime = mInitialOrbitalStatePtr->GetEpoch();
         }
         else
         {
            // In some circumstances the initial state epoch must be set here (for example see initial_state_lla input)
            // This is the only place the input parameter (aEpoch) is used (otherwise all necessary data are in the
            // initial orbital state).
            mInitialOrbitalStatePtr->SetEpoch(aEpoch);
            mCurrentTime = aEpoch;
         }
         mLastUpdateTime = mInitialOrbitalStatePtr->GetEpoch();
         mTimePastEpoch  = mCurrentTime.GetTimeSince(mInitialOrbitalStatePtr->GetEpoch());
         ok              = true;
      }
      else
      {
         ut::log::error() << "Non-elliptical propagation not allowed (orbital eccentricity >= 1.0).";
      }

      // mOrbitalStatePtr must be copied from mInitialOrbitalStatePtr,
      // in the case that TLE-specific values are needed (b* drag, mean motion derivatives, etc.)
      mOrbitalStatePtr = ut::clone(*mInitialOrbitalStatePtr);

      // If the output state's reference frame (copied from the input state) was TOD,
      // set it to be ECI in order to prevent unnecessary conversions.
      if (mOrbitalStatePtr->GetReferenceFrame() == ut::OrbitalState::ReferenceFrame::cTRUE_OF_DATE)
      {
         mOrbitalStatePtr->SetReferenceFrame(ut::OrbitalState::ReferenceFrame::cECI);
      }
   }
   else
   {
      ut::log::error() << "You must specify a valid initial orbital state for orbital propagators.";
   }

   return ok;
}

//! Perform initialization based on the provided initial orbital state.
bool UtOrbitalPropagatorBase::Initialize(const ut::OrbitalState& aInitialState)
{
   SetInitialOrbitalState(aInitialState);

   return Initialize(aInitialState.GetEpoch());
}

bool UtOrbitalPropagatorBase::InitializeOrbitalState(const UtCalendar& aEpoch)
{
   if (!mInitialOrbitalStatePtr->IsValid())
   {
      return mInitialOrbitalStatePtr->Initialize(aEpoch);
   }
   return true;
}

//! Perform updates that must occur after Propagate is called.
void UtOrbitalPropagatorBase::PostPropagate()
{
   UpdateOrbitalState();
}

// virtual protected
//! Perform initialization after all input attributes are initialized,
//! such that the output state (position, velocity, acceleration) is valid.
//! @note This method should be called last in each derived class' Initialize method.
bool UtOrbitalPropagatorBase::InitializeP(const UtCalendar& aEpoch)
{
   // Perform an initial propagation to set the kinematic values at epoch.
   bool success = true;

   if (mPerturbationPtr != nullptr)
   {
      // Initialize the perturbation object (which may use the initial state computed above).
      success = mPerturbationPtr->Initialize(*this);
   }

   if (success)
   {
      // Propagate to the current time, if different from the initial epoch.
      UpdateTime(aEpoch);
      Propagate(aEpoch);
      PostPropagate();

      if (mDebugOutputOE)
      {
         auto  out = ut::log::debug() << "UtOrbitalPropagatorBase: Initialization successful." << std::setprecision(10);
         auto& orbitalElementsTOD = GetOrbitalState().GetOrbitalElementsTOD();
         out.AddNote() << "Semi-Major Axis: " << orbitalElementsTOD.GetSemiMajorAxis();
         out.AddNote() << "Eccentricity: " << orbitalElementsTOD.GetEccentricity();
         out.AddNote() << "Inclination: " << orbitalElementsTOD.GetInclination() / UtMath::cRAD_PER_DEG << " deg";
         out.AddNote() << "RAAN: " << orbitalElementsTOD.GetRAAN() / UtMath::cRAD_PER_DEG << " deg";
         out.AddNote() << "Argument of Periapsis: " << orbitalElementsTOD.GetArgumentOfPeriapsis() / UtMath::cRAD_PER_DEG
                       << " deg";
         out.AddNote() << "Mean Anomaly: " << orbitalElementsTOD.GetMeanAnomaly() / UtMath::cRAD_PER_DEG;
      }
   }

   return success;
}

//! Only call this method before Initialize()
void UtOrbitalPropagatorBase::SetPerturbation(std::unique_ptr<Perturbation> aPerturbationPtr)
{
   mPerturbationPtr = std::move(aPerturbationPtr);
}

void UtOrbitalPropagatorBase::PrintDebug(double aTimePastEpoch)
{
   if (mDebugOutputSTK || mDebugOutputWSF || mDebugOutputXYZ)
   {
      double     lat;
      double     lon;
      double     alt;
      UtCalendar currentTime(mInitialOrbitalStatePtr->GetEpoch());
      currentTime.AdvanceTimeBy(aTimePastEpoch);
      UtEntity converter;
      converter.SetECI_ReferenceEpoch(currentTime);
      converter.SetTime(0.0);
      const ut::OrbitalState::Vector& osv = mOrbitalStatePtr->GetOrbitalStateVector();
      converter.SetLocationECI(osv.GetLocation().GetData());
      converter.GetLocationLLA(lat, lon, alt);
      const UtVec3d& location = osv.GetLocation();
      const UtVec3d& velocity = osv.GetVelocity();

      if (mDebugOutputSTK)
      {
         auto out = ut::log::debug() << "Output STK:";
         out.AddNote() << "T = " << UtTime(aTimePastEpoch, UtTime::FmtHMS + 2);
         out.AddNote() << std::setprecision(10) << "LLA: " << lat << ", " << lon << ", " << alt / 1000.0;
         out.AddNote() << std::setprecision(10) << "Velocity: " << velocity[0] << ", " << velocity[1] << ", "
                       << velocity[2];
      }
      if (mDebugOutputXYZ)
      {
         static double updateInterval = 0.0;
         if (aTimePastEpoch >= updateInterval)
         {
            updateInterval += 1800.0; // update every 30 min.
            double simTimeMin = aTimePastEpoch / 60.0;
            auto   out        = ut::log::debug() << "Output XYZ:";
            out.AddNote() << "T = " << currentTime;
            out.AddNote() << "Minutes elapsed: " << simTimeMin;
            out.AddNote() << std::setprecision(10) << "Position (km): " << location[0] / 1000.0 << ", "
                          << location[1] / 1000.0 << ", " << location[2] / 1000.0;
            out.AddNote() << std::setprecision(10) << "Velocity (km/sec.): " << velocity[0] / 1000.0 << ", "
                          << velocity[1] / 1000.0 << ", " << velocity[2] / 1000.0;
         }
      }
      if (mDebugOutputWSF)
      {
         double vmag = velocity.Magnitude();
         auto   out  = ut::log::debug() << "Output WSF:";
         out.AddNote() << "Lat: " << UtLatPos(lat, UtAngle::FmtDMS + 5);
         out.AddNote() << "Lon: " << UtLonPos(lon, UtAngle::FmtDMS + 5);
         out.AddNote() << "Altitude: " << alt / 1000.0 << " km";
         out.AddNote() << "Speed:" << vmag << " m/s";
      }
   }
}

//! Set the polar offsets in the initial orbital state's central body object.
//! @note This method is a special case where a part of the initial state is set separately.
//! This should be called prior to initialize().
void UtOrbitalPropagatorBase::SetInitialCentralPoint(const ut::CentralPoint& aCentralPoint)
{
   mInitialOrbitalStatePtr->SetCentralPoint(aCentralPoint);
}

//! Get the time in seconds from the current time to the next periapsis passage of the satellite.
//! @param aOrbitNum The number of full orbits to include in the computation after the current one.
//!        The default is zero, for which the time of the next passage is returned.
//! @return The time to periapsis passage, or -1.0 if not supported by this propagator type.
//! @note: For example, to compute the time to the sixth upcoming periapsis passage, aOrbitNum would be set to 5.
//! @note: This value has no physical meaning for circular orbits (eccentricity == 0).
double UtOrbitalPropagatorBase::GetTimeToPeriapsisPassage(unsigned aOrbitNum) const
{
   return -1.0;
}

//! Get the time in seconds from the current time to the given ascending node passage.
//! @param aOrbitNum The number of full orbits to include in the computation after the current one.
//!        The default is zero, for which the time of the next passage is returned.
//! @return The time to ascending node passage, or -1.0 if not supported by this propagator type.
//!  If the orbit is equatorial, no solution is possible and 0.0 is returned.
//! @note: For example, to compute the time to the sixth upcoming ascending node passage, aOrbitNum would be set to 5.
//! @note: This value has no physical meaning for circular orbits (eccentricity == 0).
double UtOrbitalPropagatorBase::GetTimeToAscendingNodePassage(unsigned aOrbitNum) const
{
   return -1.0;
}

//! Return the time since the given ascending node passage.
//! @param aOrbitNum The number of full orbits to include in the computation before the current one.
//!        The default is zero, for which the time of the previous passage is returned.
//! @return The time to ascending node passage, or -1.0 if not supported by this propagator type.
//!  If the orbit is equatorial, no solution is possible and 0.0 is returned.
//! @note: For example, to compute the time to the sixth previous ascending node passage, aOrbitNum would be set to 5.
//! @note: This value has no physical meaning for circular orbits (eccentricity == 0).
double UtOrbitalPropagatorBase::GetTimeSinceAscendingNodePassage(unsigned aOrbitNum) const
{
   return -1.0;
}

//! Return the time since the given descending node passage.
//! @param aOrbitNum The number of full orbits to include in the computation before the current one.
//!        The default is zero, for which the time of the previous passage is returned.
//! @return The time to descending node passage, or -1.0 if not supported by this propagator type.
//!  If the orbit is equatorial, no solution is possible and 0.0 is returned.
//! @note: For example, to compute the time to the sixth previous descending node passage, aOrbitNum would be set to 5.
//! @note: This value has no physical meaning for circular orbits (eccentricity == 0).
double UtOrbitalPropagatorBase::GetTimeSinceDescendingNodePassage(unsigned aOrbitNum) const
{
   return -1.0;
}

//! Get the time in seconds from the current time to the next descending node passage.
//! @param aOrbitNum The number of full orbits to include in the computation after the current one.
//!        The default is zero, for which the time of the next passage is returned.
//! @return The time to descending node passage, or -1.0 if not supported by this propagator type.
//!  If the orbit is equatorial, no solution is possible and 0.0 is returned.
//! @note: For example, to compute the time to the sixth upcoming descending node passage, aOrbitNum would be set to 5.
//! @note: This value has no physical meaning for circular orbits (eccentricity == 0).
double UtOrbitalPropagatorBase::GetTimeToDescendingNodePassage(unsigned aOrbitNum) const
{
   return -1.0;
}

//! Given a desired radius for the current orbit, past the ascending node, get the time in seconds before reaching it.
//! @param aRadius The desired radius.
//! @param aTime The time until reaching that radius in the orbit.
//! @param aOrbitNumber The number of full orbits to include in the computation after the current one.
//! The default is zero, for which the time of the next intersection is returned.
//! @return The time until the orbit radius equals the given radius.  If the current orbit does not achieve this radius,
//! 0.0 is returned.
bool UtOrbitalPropagatorBase::GetTimeToAscendingRadius(double aRadius, double& aTime, unsigned aOrbitNumber) const
{
   return false;
}

//! Given a desired radius for the current orbit, past the descending node, get the time in seconds before reaching it.
//! @param aRadius The desired radius.
//! @param aTime The time until reaching that radius in the orbit.
//! @param aOrbitNumber The number of full orbits to include in the computation after the current one.
//! The default is zero, for which the time of the next intersection is returned.
//! @return The time until the orbit radius equals the given radius.  If the current orbit does not achieve this radius,
//! 0.0 is returned.
bool UtOrbitalPropagatorBase::GetTimeToDescendingRadius(double aRadius, double& aTime, unsigned aOrbitNumber) const
{
   return false;
}

//! Define whether an orbit is "circular" (i.e., has nearly zero eccentricity).
bool UtOrbitalPropagatorBase::OrbitIsCircular() const
{
   return mOrbitalStatePtr->OrbitIsCircular();
}

//! Define whether an orbit is "equatorial" (i.e., the inclination is very close to 0 or 180 degrees).
bool UtOrbitalPropagatorBase::OrbitIsEquatorial() const
{
   return mOrbitalStatePtr->OrbitIsEquatorial();
}

//! Return whether the orbit is prograde (orbiting counterclockwise when viewed from above the north pole of the central
//! body; true when the orbital inclination is less than 90 degrees).
bool UtOrbitalPropagatorBase::OrbitIsPrograde() const
{
   return mOrbitalStatePtr->OrbitIsPrograde();
}

//! Return the descriptor associated with this propagator.
const std::string& UtOrbitalPropagatorBase::GetDescriptor() const
{
   return mInitialOrbitalStatePtr->GetOrbitalElements().GetDescriptor();
}

//! Return the designator associated with this propagator.
const std::string& UtOrbitalPropagatorBase::GetDesignator() const
{
   return mInitialOrbitalStatePtr->GetOrbitalElements().GetDesignator();
}

std::unique_ptr<UtOrbitalPropagatorBase> UtOrbitalPropagatorBase::GetManeuveringPropagator() const
{
   auto retval = std::unique_ptr<UtOrbitalPropagatorBase>(Clone());
   // (There is no need to initialize this object as it is currently active and updated to mCurrentTime.)
   return retval;
}

//! Compute a solution to the Lambert problem of providing a kinematic solution of an orbit given two positions and
//! time. Because the positions are provided, this equates to computation of velocity at those points.
//! @param aCentralBody The central body (typically the Earth), whose gravity influences the solution.
//! @param aLocationInertial_1 The first provided satellite location in Inertial coordinates.
//! @param aLocationInertial_2 The second provided satellite location in Inertial coordinates.
//! @param aDt The elapsed time for the satellite to have gone from aLocInertial_1 to aLocInertial_2 (time between
//! observations).
//! @param aIsShortWay Whether the solution should be computed assuming aDt is for the satellite to go "the short way"
//! along the ellipse from one point to the other, or "the long way."
//! @param aAllowHitsEarth True if the solution is allowed to find an orbit that hits the central body.
//! @param aConvergenceTolerance (optional) The numerical tolerance at which the iteration stops and a solution is
//! assumed "good enough."
//! @return A Result instance containing the solution.
UtLambertProblem::Result UtOrbitalPropagatorBase::SolveLambertProblem(const ut::CentralBody& aCentralBody,
                                                                      const UtVec3d&         aLocationInertial_1,
                                                                      const UtVec3d&         aLocationInertial_2,
                                                                      double                 aDt,
                                                                      bool                   aShortWay,
                                                                      bool                   aAllowHitsEarth,
                                                                      double aConvergenceTolerance) const
{
   return UtLambertProblem::Result{aDt, aLocationInertial_1, aLocationInertial_2, aCentralBody.GetEllipsoid()};
}

//! Get the time in seconds from the current time to the next apoapsis passage of the satellite.
//! @param aOrbitNum The number of full orbits to include in the computation after the current one.
//!        The default is zero, for which the time of the next passage is returned.
//! @return The time to apoapsis passage.  If the orbit is circular, zero is returned.
//! @note: For example, to compute the time to the sixth upcoming apoapsis passage, aOrbitNum would be set to 5.
//! @note: This value has no physical meaning for circular orbits (eccentricity == 0).
double UtOrbitalPropagatorBase::GetTimeToApoapsisPassage(unsigned aOrbitNum) const
{
   return -1.0;
}

double UtOrbitalPropagatorBase::GetTimeToNorthernIntersection(double   aTargetRAAN,
                                                              double   aTargetInclination,
                                                              unsigned aOrbitNum) const
{
   return -1.0;
}

double UtOrbitalPropagatorBase::GetTimeToSouthernIntersection(double   aTargetRAAN,
                                                              double   aTargetInclination,
                                                              unsigned aOrbitNum) const
{
   return -1.0;
}

//! A utility method to evaluate deprecated bool input for given keywords.
bool UtOrbitalPropagatorBase::EvaluateDebugInput(UtInput& aInput)
{
   std::string command;
   aInput.ReadCommand(command);
   aInput.PushBack(command);
   bool flag = false;
   try
   {
      aInput.ReadValue(flag);
   }
   catch (std::exception&)
   {
      aInput.PushBack(command);
      flag = true;
   }
   return flag;
}

//! Pass-through method to UtEclipse::GetEclipsedState.
UtEclipse::EclipsedState UtOrbitalPropagatorBase::GetEclipsedState() const
{
   return UtEclipse::GetEclipsedState(mCurrentTime, mOrbitalStatePtr->GetOrbitalStateVectorInertial().GetLocation());
}

//! Pass-through method to UtEclipse::GetEclipseTimes.  The time of
//! evaluation (used to fix the sun's position) is assumed to be mCurrentTime.
bool UtOrbitalPropagatorBase::GetEclipseTimes(double& aTimeToEntry, double& aTimeToExit) const
{
   return UtEclipse::GetEclipseTimes(mCurrentTime, mOrbitalStatePtr->GetOrbitalElementsTOD(), aTimeToEntry, aTimeToExit);
}

//! Pass-through method to UtEclipse::GetEclipseTimes.
//! @param aTime The evaluation time, used to fix the sun's location. used in the eclipse calculations.
//! @param aTimeToEntry The output time of entry of the satellite into Earth's shadow.
//! @param aTimeToExit The output time of exit of the satellite from Earth's shadow.
bool UtOrbitalPropagatorBase::GetEclipseTimes(const UtCalendar& aTime, double& aTimeToEntry, double& aTimeToExit) const
{
   return UtEclipse::GetEclipseTimes(aTime, mOrbitalStatePtr->GetOrbitalElementsTOD(), aTimeToEntry, aTimeToExit);
}

//! Pass-through method to UtEclipse::GetEclipseAnomalies.
//! @param aTime The evaluation time, used to fix the sun's location.
//! @param aEntryAnomaly The true anomaly of the satellite when it enters earth's shadow.
//! @param aExitAnomaly The true anomaly of the satellite when it exits earth's shadow.
bool UtOrbitalPropagatorBase::GetEclipseAnomalies(const UtCalendar& aTime, double& aEntryAnomaly, double& aExitAnomaly) const
{
   return UtEclipse::GetEclipseAnomalies(aTime, mOrbitalStatePtr->GetOrbitalElementsTOD(), aEntryAnomaly, aExitAnomaly);
}

//! Pass-through method to UtEclipse::GetEclipseAnomalies.  The time of
//! evaluation (used to fix the sun's position) is assumed to be mCurrentTime.
//! @param aEntryAnomaly The true anomaly of the satellite when it enters earth's shadow.
//! @param aExitAnomaly The true anomaly of the satellite when it exits earth's shadow.
bool UtOrbitalPropagatorBase::GetEclipseAnomalies(double& aEntryAnomaly, double& aExitAnomaly) const
{
   return UtEclipse::GetEclipseAnomalies(mCurrentTime, mOrbitalStatePtr->GetOrbitalElementsTOD(), aEntryAnomaly, aExitAnomaly);
}
