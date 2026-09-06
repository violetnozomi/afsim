// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

// References:
//
// 1) "Initial Orbit Determination Using Multiple Observations", Karimi and Mortari,
//     Celest. Mech. Dyn. Astr. (2011) 109:167-180.
// 2)  Fundamentals of Astrodynamics, Bate, Mueller, and White, pp.198-203.

#include "WsfOrbitDeterminationFusion.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

#include <UtMemory.hpp>

#include "UtCalendar.hpp"
#include "UtDCM.hpp"
#include "UtECI_Conversion.hpp"
#include "UtEarth.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtInput.hpp"
#include "UtInputString.hpp"
#include "UtLambertProblem.hpp"
#include "UtLog.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtMatrix.hpp"
#include "UtMeasurementUtil.hpp"
#include "UtOrbitalElements.hpp"
#include "UtOrbitalPropagator.hpp"
#include "UtRandom.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfCovariance.hpp"
#include "WsfDateTime.hpp"
#include "WsfEnvironment.hpp"
#include "WsfKeplerianOrbitalPropagator.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfOrbitDeterminationKalmanFilter.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfSpaceMover.hpp"
#include "WsfSpaceObserver.hpp"
#include "WsfStringId.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackList.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTypes.hpp"
#include "WsfUtil.hpp"

namespace
{
constexpr double cMAX_ECCENTRICITY = 0.9;
constexpr double cMIN_RANGE        = 30000.0; // approx 30 kilometers altitude
constexpr double cMIN_RADIUS       = UtSphericalEarth::cEARTH_MEAN_RADIUS + cMIN_RANGE;
constexpr double cMAX_RADIUS       = 200000000.0; // beyond furthest relevant bound satellite orbit

//! Helper method to compute the ECI location of a target given range from site, a unit target vector from the
//! site, and the site location.
UtVec3d ComputeLocationECI(double aRange, const UtVec3d& aTargetVec, const UtVec3d& aSiteLoc)
{
   UtVec3d targetLoc;
   targetLoc.Set(aTargetVec);
   targetLoc.Multiply(aRange);
   targetLoc.Add(aSiteLoc);
   return targetLoc;
}

//! Helper method to compute all ECI locations based on ranges, target vectors, and site locations.
void ComputeLocations(const UtMatrixd&            aRanges,
                      const std::vector<UtVec3d>& aTargetVectors,
                      const std::vector<UtVec3d>& aSiteLocations,
                      std::vector<UtVec3d>&       aLocECI)
{
   auto size = static_cast<unsigned int>(aLocECI.size());
   for (unsigned int i = 0; i < size; ++i)
   {
      aLocECI[i] = ComputeLocationECI(aRanges(i), aTargetVectors[i], aSiteLocations[i]);
   }
}

//! Given a radius (distance from center of the earth to target), a target vector from the site location,
//! and the measurement site's ECI location, determine the target's range from the site location.
//! @param aRadius The distance from the center of the earth to the target.
//! @param aSiteLoc The site location for the observation.
//! @param aUnitTargetVec The unit vector from the site to the target.
//! @return The range from site to target.
double ComputeRange(double aRadius, const UtVec3d& aSiteLoc, const UtVec3d& aUnitTargetVec)
{
   double  siteRadius     = aSiteLoc.Magnitude();
   UtVec3d unitVecSiteLoc = aSiteLoc / siteRadius;
   double  sigma          = UtMath::cPI - acos(UtVec3d::DotProduct(unitVecSiteLoc, aUnitTargetVec));
   double  sinSigma       = sin(sigma);
   double  alpha          = asin(std::max(-1.0, std::min(1.0, siteRadius * sinSigma / aRadius)));
   double  gamma          = UtMath::cPI - alpha - sigma;
   double  range          = sin(gamma) * aRadius / sinSigma;
   return range;
}
} // namespace

// =================================================================================================
WsfOrbitDeterminationFusion::WsfOrbitDeterminationFusion(const WsfScenario& aScenario)
   : WsfDefaultFusion(aScenario)
   , mPrototypeFilter(aScenario)
   , mNumberOfAnglesMeasurementsNeeded(5)
   , mLambertConvergenceTolerance(1.0e-12)
   , mRangeErrorFactor(0.05)
   , mAnglesOnlyMaxIterations(200)
   , mAnglesOnlyLinearTolerance(10.0)
   , mDebug(false)
{
}

// =================================================================================================
// virtual
void WsfOrbitDeterminationFusion::ReviewTrack(WsfLocalTrack& aLocalTrack)
{
   ReviewIFF(GetTrackManager(), aLocalTrack);
   ReviewTrackQuality(GetTrackManager(), aLocalTrack);
   aLocalTrack.UpdateSignalList();
}

// =================================================================================================
// static
void WsfOrbitDeterminationFusion::ReviewTrack(WsfTrackManager& aTrackManager, WsfLocalTrack& aLocalTrack)
{
   ReviewIFF(aTrackManager, aLocalTrack);
   ReviewTrackQuality(aTrackManager, aLocalTrack);
   aLocalTrack.UpdateSignalList();
}

bool WsfOrbitDeterminationFusion::ProcessInput(UtInput& aInput)
{
   std::string command;
   aInput.GetCommand(command);
   bool myCommand = true;
   if (command == "number_of_angle_measurements")
   {
      aInput.ReadValue(mNumberOfAnglesMeasurementsNeeded);
      aInput.ValueGreaterOrEqual(mNumberOfAnglesMeasurementsNeeded, 3U);
   }
   else if (command == "lambert_convergence_tolerance")
   {
      aInput.ReadValue(mLambertConvergenceTolerance);
   }
   else if (command == "range_error_factor")
   {
      aInput.ReadValue(mRangeErrorFactor);
      aInput.ValueInClosedRange(mRangeErrorFactor, 1.0e-7, 0.5);
   }
   else if (command == "angles_only_linear_tolerance")
   {
      aInput.ReadValueOfType(mAnglesOnlyLinearTolerance, UtInput::cLENGTH);
   }
   else if (command == "angles_only_maximum_iterations")
   {
      aInput.ReadValue(mAnglesOnlyMaxIterations);
   }
   else if (command == "process_noise_sigmas_XYZ")
   {
      myCommand = mPrototypeFilter.ProcessInput(aInput);
   }
   else if (command == "debug_filter")
   {
      aInput.PushBack("debug");
      aInput.ReadCommand(command);
      mPrototypeFilter.ProcessInput(aInput); // turn on filter debug
   }
   else if (command == "debug")
   {
      mDebug = true;
   }
   else if (mPrototypeFilter.ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = WsfDefaultFusion::ProcessInput(aInput);
   }
   return myCommand;
}

bool WsfOrbitDeterminationFusion::Initialize(WsfTrackManager* aTrackManagerPtr)
{
   SetWeightedAverage(true);
   return WsfFusionStrategy::Initialize(aTrackManagerPtr);
}

// =================================================================================================
// virtual
bool WsfOrbitDeterminationFusion::UpdateLocalTrackFromNonLocalTrack(double          aSimTime,
                                                                    WsfLocalTrack&  aLocalTrack,
                                                                    const WsfTrack& aNonLocalTrack)
{
   bool localTrackUpdated = false;

   UtOrbitalPropagatorBase* propPtr    = GetPropagator(aLocalTrack);
   double                   fusionTime = aNonLocalTrack.GetUpdateTime();

   // Does this track have an associated propagator?
   if ((propPtr != nullptr) && (aLocalTrack.GetFilter() != nullptr))
   {
      UtCalendar epochSimTime;
      WsfTrack   measurement = aNonLocalTrack; // todo optimize (copy only needed for bearing-elevation tracks)
      GetSimCalendarTime(fusionTime, epochSimTime);

      if (!IsLocation(measurement) && IsBearingElevation(measurement))
      {
         // Add a pseudo-range based on the existing propagated location.
         // Use the range from the propagator and create a measurement covariance using a large range sigma.
         UtVec3d           locWCS;
         double            range{0.0};
         double            rangeError{0.0};
         UtMeasurementData estimate;
         aLocalTrack.GetFilter()->GetKinematicEstimate(fusionTime, estimate);
         locWCS = estimate.GetLocationWCS();
         UtVec3d rangeVec;
         rangeVec.Subtract(locWCS, measurement.GetOriginatorLocationWCS());
         range = rangeVec.Magnitude();
         UtCovariance predictedCovar;
         measurement.SetRange(range);
         measurement.SetRangeValid(true);
         // if (aLocalTrack.GetFilter()->GetPredictedStateCovariance(fusionTime, predictedCovar))
         //{
         //    // Todo unit test
         //    rangeError = std::max(1.0, ComputeRangeError(measurement, predictedCovar));
         // }
         // else
         //{
         rangeError = range * mRangeErrorFactor;
         //}
         measurement.SetRangeError(rangeError);
         UtCovariance covar(3, 3);
         covar.ComputeMeasurementCovariance(range,
                                            rangeError,
                                            measurement.GetSensorAzimuthError(),
                                            measurement.GetSensorElevationError());
         double WCSToBCSTransform[3][3];
         measurement.GetOriginatorTransformWCS(WCSToBCSTransform);
         // Below, the input transform is always B_To_A_Transform, so A = BCS and B = WCS
         UtCovariance::ConvertFromFrameA_ToFrameB(covar, WCSToBCSTransform);
         covar.Symmetrize();
         covar.CopyTrackData(measurement);
         measurement.SetMeasurementCovariance(fusionTime, covar);
      }

      // Location updates go directly to the filter
      // Which was created during IOD in calls to FuseInitialLocations or FuseInitialAngles.
      if (IsLocation(measurement))
      {
         // Todo Review WsfLocalTrack.UpdateFromMeasurement, as it is not appropriate here.
         // Need to do equivalent of calling base class' method as below (make static?)
         // static_cast<WsfTrack&>(aLocalTrack).UpdateFromMeasurement(aSimTime, measurement);
         WsfMeasurement estimate;
         if (mDebug)
         {
            auto logger = ut::log::debug() << "Attempting to perform OD Update.";
            logger.AddNote() << "Local Track ID(Updating): " << aLocalTrack.GetTrackId();
            logger.AddNote() << "Non Local Track(Incoming): " << aNonLocalTrack.GetTrackId();
            logger.AddNote() << "Non Local Track(Target): " << aNonLocalTrack.GetTargetName();
         }
         aLocalTrack.GetFilter()->Update(fusionTime, measurement, estimate);
         localTrackUpdated = UpdateTrackFromEstimate(fusionTime, aLocalTrack, estimate);
         if (aLocalTrack.GetFilter()->IsStable())
         {
            aLocalTrack.KinematicUpdate(aSimTime);
         }
         else if (mDebug)
         {
            ut::log::debug() << "Filter stabilizing (OD not updated).";
         }
      }
   }
   else if (IsLocation(aNonLocalTrack)) // and no existing propagator; IOD has not yet been performed.
   {
      localTrackUpdated = FuseInitialLocations(fusionTime, aLocalTrack, aNonLocalTrack);
   }
   else if (IsBearingElevation(aNonLocalTrack)) // and no existing propagator; IOD has not yet been performed.
   {
      localTrackUpdated = FuseInitialAngles(fusionTime, aLocalTrack, aNonLocalTrack);
   }

   if (!localTrackUpdated)
   {
      // Perform direct replacement.
      aLocalTrack.ReplacementUpdate(aNonLocalTrack);

      // Aux data is not copied by the direct replacement because we still want to allow the user
      // to have local aux data. This causes aux data to go through our 'fusion' process.
      GetTrackManager().UpdateAuxData(aSimTime, aLocalTrack, aNonLocalTrack);
      localTrackUpdated = true;
   }
   else // track has been updated (location is valid).
   {
      // Set the originating location to be the current location of the host platform.
      // We are about to update a platform based on a perceived time.
      // This time should never be greater than the current simulation time.
      GetPlatform()->Update(aSimTime);
      double reportingLocationWCS[3];
      GetPlatform()->GetLocationWCS(reportingLocationWCS);
      aLocalTrack.SetOriginatorLocationWCS(reportingLocationWCS);
      double WCS_ToNED_Transform[3][3];
      GetPlatform()->GetWCSToNEDTransform(WCS_ToNED_Transform);
      aLocalTrack.SetOriginatorTransformWCS(WCS_ToNED_Transform);

      // Invalidate range, bearing, elevation, measurement covariance.
      aLocalTrack.SetRangeValid(false);
      aLocalTrack.SetBearingValid(false);
      aLocalTrack.SetElevationValid(false);
      aLocalTrack.SetMeasurementCovarianceValid(false);
      aLocalTrack.Update(aSimTime);

      if (mDebug)
      {
         WsfPlatform* platformPtr = GetSimulation()->GetPlatformByIndex(aNonLocalTrack.GetTargetIndex());
         if (platformPtr != nullptr)
         {
            platformPtr->Update(aSimTime);
            UtVec3d locWCS;
            platformPtr->GetLocationWCS(locWCS.GetData());
            UtVec3d diffLoc;
            diffLoc.Subtract(aLocalTrack.GetLocationWCS().GetData(), locWCS.GetData());
            auto logger = ut::log::debug() << "Track-truth location";
            logger.AddNote() << "Difference: " << diffLoc.Magnitude();
         }
      }
   }

   return localTrackUpdated;
}

// Find the speeds at which the satellite would be orbiting based on geometry and time, and
// the speed that it would be orbiting at the guessed radius due to gravitation.
void WsfOrbitDeterminationFusion::ComputeCircularLocationsAndSpeeds(const UtVec3d& aUnitTargetVecECI_1,
                                                                    const UtVec3d& aSiteLocECI_1,
                                                                    const UtVec3d& aUnitTargetVecECI_2,
                                                                    const UtVec3d& aSiteLocECI_2,
                                                                    double         aDt,
                                                                    double         aRadius,
                                                                    UtVec3d&       aLocationECI_1,
                                                                    UtVec3d&       aLocationECI_2,
                                                                    double&        aGeometricSpeed,
                                                                    double&        aGravitationalSpeed)
{
   double  range1         = ComputeRange(aRadius, aSiteLocECI_1, aUnitTargetVecECI_1);
   double  range2         = ComputeRange(aRadius, aSiteLocECI_2, aUnitTargetVecECI_2);
   UtVec3d targetVecECI_1 = range1 * aUnitTargetVecECI_1;
   UtVec3d targetVecECI_2 = range2 * aUnitTargetVecECI_2;
   aLocationECI_1         = targetVecECI_1 + aSiteLocECI_1;
   aLocationECI_2         = targetVecECI_2 + aSiteLocECI_2;
   UtVec3d locECI_1       = aLocationECI_1.GetNormal();
   UtVec3d locECI_2       = aLocationECI_2.GetNormal();
   double  theta          = acos(std::max(std::min(UtVec3d::DotProduct(locECI_1, locECI_2), 1.0), -1.0));
   aGeometricSpeed        = theta * aRadius / aDt;
   // -Then find the speed that it would be orbiting at the guessed radius due to gravitation.
   aGravitationalSpeed = sqrt(GetPlatform()->GetCentralBody().GetGravitationalParameter() / aRadius);
}

//! Provide an approximate initial guess of satellite location and velocity based on bearing-elevation measurements.
//! Returned values are the estimated location and velocity of the target at the time of the first measurement.
//! @param aUnitTargetVecECI_1 The first angles-only unit vector from site to target.
//! @param aSiteLocECI_1 The site location of the first observation.
//! @param aUnitTargetVecECI_2 The second angles-only unit vector from site to target.
//! @param aSiteLocECI_2 The site location of the second observation.
//! @param aDt The elapsed time between the two observations.
//! @param aLocationECI_1 (out) Location estimate of the first observation.
//! @param aLocationECI_2 (out) Location estimate of the second observation.
//! @note Algorithm design is as follows:
//! - Bound the solution to just outside the atmosphere to beyond the SMA of the furthest relevant satellite orbit.
//! - Begin Iterating.
//! - Find the speed at which the satellite would be orbiting based on geometry and time (circular orbit approximation).
//! - Find the speed that it would be orbiting at the guessed radius due to gravitation (circular orbit approximation).
//! - Compute change in speed (gravitationally calculated speed minus geometrically calculated speed)to determine
//! whether to increase or decrease the orbit radius.
//!   Negative values indicate the orbit is too close to earth;
//!   Positive values indicate the orbit is too far away from earth.
//! - Compute initial locations and speeds based on a minimum radius, assuming we will walk outward by a specific delta
//! to find the solution.
//! - Initialize the last speed change to be very large and with the same sign as the current speed change.
//! - Bisect in radius after a transition is found to obtain an acceptable result.
//! - If the change in speed is below a threshold
//!    - we have found the solution; break.
//! - else if the speed change is negative,
//!    - If the last speed change is positive, we have found a transition (the solution should be close to it).
//!    - move orbit further in towards earth
//!      - Set the maximum radius to be the current radius
//!      - If the transition has been found
//!         - Set the new radius to be the current radius minus the difference of the maximum and minimum radius,
//!         divided by two.
//!      - else
//!         - Set the new radius to be the current radius minus the delta.
//! - else, in the case that the speed change is positive,
//!    - If the last speed change is negative, we have found a transition (the solution should be close to it).
//!    - move orbit further out away from earth
//!      - Set the minimum radius to be the current radius
//!      - If the transition has been found
//!         - Set the new radius to be the current radius plus the difference of the maximum and minimum radius, divided
//!         by two.
//!      - else
//!         - Set the new radius to be the current radius plus the delta.
//! - Continue until a solution is found or the maximum number of iterations is reached.
bool WsfOrbitDeterminationFusion::AnglesOnlyInitialGuess(const UtVec3d& aUnitTargetVecECI_1,
                                                         const UtVec3d& aSiteLocECI_1,
                                                         const UtVec3d& aUnitTargetVecECI_2,
                                                         const UtVec3d& aSiteLocECI_2,
                                                         double         aDt,
                                                         UtVec3d&       aLocationECI_1,
                                                         UtVec3d&       aLocationECI_2)
{
   unsigned            iterations      = 0;
   static const double cMAX_ITERATIONS = 200;

   if (aDt > 0.0)
   {
      // -Bound the solution to just outside the atmosphere to beyond furthest satellite orbit.
      double minRadius = cMIN_RADIUS;
      double maxRadius = cMAX_RADIUS;
      double radius    = cMIN_RADIUS;

      // Initial linear search step, should take of order the max iterations to walk through the search space,
      // but somewhat greater to allow for a worst case search.
      double deltaRadius = 2.0 * (cMAX_RADIUS - cMIN_RADIUS) / cMAX_ITERATIONS;

      bool   foundTransition    = false;
      double geometricalSpeed   = 0.0;
      double gravitationalSpeed = 0.0;
      double lastDeltaSpeed     = 9.0e99;

      while (iterations < cMAX_ITERATIONS)
      {
         // -Begin Iterating
         // -Limit searching to 200 iterations.
         ComputeCircularLocationsAndSpeeds(aUnitTargetVecECI_1,
                                           aSiteLocECI_1,
                                           aUnitTargetVecECI_2,
                                           aSiteLocECI_2,
                                           aDt,
                                           radius,
                                           aLocationECI_1,
                                           aLocationECI_2,
                                           geometricalSpeed,
                                           gravitationalSpeed);

         // -Compute change in speed ( gravitationally calculated speed minus geometrically calculated speed)
         // to determine whether to increase or decrease the orbit radius.
         double deltaSpeed = gravitationalSpeed - geometricalSpeed;

         // Special case below: if we guessed wrong and should have started from the outer radius, change it here and continue.
         if ((iterations == 0) && (deltaSpeed < 0.0))
         {
            radius = maxRadius;
            ComputeCircularLocationsAndSpeeds(aUnitTargetVecECI_1,
                                              aSiteLocECI_1,
                                              aUnitTargetVecECI_2,
                                              aSiteLocECI_2,
                                              aDt,
                                              radius,
                                              aLocationECI_1,
                                              aLocationECI_2,
                                              geometricalSpeed,
                                              gravitationalSpeed);
            deltaSpeed     = gravitationalSpeed - geometricalSpeed;
            lastDeltaSpeed = -lastDeltaSpeed;
         }

         static const double cSPEED_TOLERANCE = 10.0;

         // -Bisect in radius after a transition is found to obtain an acceptable result.
         if (fabs(deltaSpeed) < cSPEED_TOLERANCE)
         {
            // -If the change in speed is below a threshold, we have found the solution; break.
            break;
         }
         else if (deltaSpeed < 0.0)
         {
            // -else if the speed change is negative, we have found a transition, and the solution may be close to it.
            if (lastDeltaSpeed > 0.0)
            {
               foundTransition = true;
            }
            // -move orbit further in towards earth.
            maxRadius = radius;
            if (foundTransition)
            {
               radius -= (maxRadius - minRadius) / 2.0;
            }
            else
            {
               radius -= deltaRadius;
            }
            lastDeltaSpeed = deltaSpeed;
         }
         else
         {
            // -else, in the case that the speed change is positive,
            // move orbit further out away from earth.
            // -else if the speed change is negative, we have found a transition, and the solution may be close to it.
            if (lastDeltaSpeed < 0.0)
            {
               foundTransition = true;
            }
            minRadius = radius;
            if (foundTransition)
            {
               //   -If we've already found the transition,
               // bound the distance we go out by the maximum distance.
               radius += (maxRadius - minRadius) / 2.0;
            }
            else
            {
               //   -Otherwise, we are not yet bounded by the outer edge of the orbit,
               // so step out a predetermined amount.
               radius += deltaRadius;
            }
            lastDeltaSpeed = deltaSpeed;
         }

         // -Continue until we've found a solution or the maximum number of iterations is reached.
         ++iterations;
      }
   }
   else
   {
      return false;
   }

   if (mDebug && (iterations == cMAX_ITERATIONS))
   {
      ut::log::warning() << "Angles-only Orbit Determination Fusion Initial Guess: Max Iterations.";
   }

   return true;
}

//! Provide a general least-squares solution for location and velocity with three or more bearing-elevation
//! measurements. Algorithm is based on the paper "Initial Orbit Determination Using Multiple Observations", Karimi and
//! Mortari, Celest. Mech. Dyn. Astr. (2011) 109:167-180.
//! @param aData The set of data to fit; these must contain at least three bearing-elevation measurements.
//! @param aLocECI The output ECI locations corresponding with the input measurements.
//! @param aVelECI The output ECI velocities corresponding with the input measurements.
//! @return Whether the solution converged with the desired accuracy.
bool WsfOrbitDeterminationFusion::AnglesOnlyKinematicSolution(const MeasurementList& aData,
                                                              std::vector<UtVec3d>&  aLocECI,
                                                              std::vector<UtVec3d>&  aVelECI)
{
   auto                 size = static_cast<unsigned int>(aData.size());
   std::vector<UtVec3d> targetVec;
   std::vector<UtVec3d> siteLoc;
   std::vector<UtVec3d> nextLocECI;
   UtMatrixd            rhoVec(size, 1);
   UtMatrixd            lastRhoVec(size, 1);
   UtMatrixd            bestRhoVec(size, 1);
   UtMatrixd            maxRhoVec(size, 1);
   UtMatrixd            minRhoVec(size, 1);
   constexpr double     cINITIAL_DELTA_RHO_VEC_SCALE_INCREMENT = 0.1;
   double               deltaRhoVecScaleIncrement              = cINITIAL_DELTA_RHO_VEC_SCALE_INCREMENT;
   double               deltaRhoVecScale                       = deltaRhoVecScaleIncrement;
   bool                 isConvergent                           = false;
   bool                 isDivergent                            = false;

   // Store all site locations and targetVectors (unit vectors from site locations to target).
   for (size_t i = 0; i < size; ++i)
   {
      targetVec.emplace_back();
      siteLoc.emplace_back();
      nextLocECI.emplace_back();
      ConvertBearingElevation(*aData[i], targetVec[i], siteLoc[i]);
   }

   // Then get initial estimates of target location assuming a circular orbit.
   // This estimate is then used to store initial ECI locations used to compute initial c's and d's.
   for (unsigned int i = 1; i < size; ++i)
   {
      UtVec3d  locECI1;
      UtVec3d  locECI2;
      unsigned i1 = i - 1;
      unsigned i2 = i;
      double   dt = aData[i2]->GetUpdateTime() - aData[i1]->GetUpdateTime();

      AnglesOnlyInitialGuess(targetVec[i1], siteLoc[i1], targetVec[i2], siteLoc[i2], dt, locECI1, locECI2);

      if (i == 1)
      {
         UtVec3d targetVec1 = locECI1 - siteLoc[i1];
         rhoVec(i1)         = targetVec1.Magnitude();
         aLocECI.push_back(locECI1);
         aVelECI.emplace_back();
      }

      UtVec3d targetVec2 = locECI2 - siteLoc[i2];
      rhoVec(i2)         = targetVec2.Magnitude();
      aLocECI.push_back(locECI2);
      aVelECI.emplace_back();
   }

   std::vector<double> c; // c's based on Gauss' f's and g's.
   std::vector<double> d; // d's based on Gauss' f's and g's.
   c.resize(size - 2);
   d.resize(size - 2);

   // start iteration loop here
   unsigned iterations              = 0;
   double   leastMaxDeltaRho        = 1.0e100;
   double   leastMaxDeltaRhoPercent = 1.0e100;
   double   maxDeltaRhoPercent      = leastMaxDeltaRhoPercent;
   bool     isDiverging             = false;
   bestRhoVec                       = rhoVec;
   lastRhoVec                       = rhoVec;

   // We must find velocities, either for output or for high-precision computation of the next iteration of
   // c's and d's (based on Gauss' f's and g's in the referenced paper).
   ComputeVelocities(aData, aLocECI, aVelECI);

   if (mDebug)
   {
      for (size_t i = 0; i < size; ++i)
      {
         PrintAnglesOnlyData(static_cast<const WsfTrack&>(*aData[i]),
                             aLocECI[i],
                             aVelECI[i],
                             rhoVec(static_cast<unsigned int>(i)));
      }
   }

   // Iterate while either the number of iterations is less than the max allowed, or
   // there is an acceptable convergence in the range vector computation (see below; middle test of loop).
   while (iterations < mAnglesOnlyMaxIterations)
   {
      // Compute a set of c's and d's.  These are based on Gauss' f's and g's, computed up to 6th order.
      for (size_t i = 1; i < (size - 1); ++i)
      {
         double delTm = aData[i]->GetUpdateTime() - aData[i - 1]->GetUpdateTime();
         double delTp = aData[i + 1]->GetUpdateTime() - aData[i]->GetUpdateTime();
         double fkm1, gkm1;
         double fkp1, gkp1;
         ComputeLambertf_g(aLocECI[i], aVelECI[i], -delTm, fkm1, gkm1);
         ComputeLambertf_g(aLocECI[i], aVelECI[i], delTp, fkp1, gkp1);
         double denom = (fkm1 * gkp1 - fkp1 * gkm1);
         double ck    = gkp1 / denom;  // equation 4 (ref. 1)
         double dk    = -gkm1 / denom; // equation 4 (ref. 1)
         c[i - 1]     = ck;
         d[i - 1]     = dk;
      }

      UtMatrixd M(3 * (size - 2), size);
      UtMatrixd psi(3 * (size - 2), 1);

      // The following is the representation of equations 19 and 20 (ref. 1):
      for (size_t j = 0; j < (size - 2); ++j)
      {
         for (size_t xyz = 0; xyz < 3; ++xyz)
         {
            auto rowIndex    = static_cast<unsigned int>(j * 3 + xyz);
            psi(rowIndex, 0) = siteLoc[j + 1][xyz] - (c[j] * siteLoc[j][xyz] + d[j] * siteLoc[j + 2][xyz]);
            M(rowIndex, static_cast<unsigned int>(j))     = c[j] * targetVec[j][xyz];
            M(rowIndex, static_cast<unsigned int>(j + 1)) = -targetVec[j + 1][xyz];
            M(rowIndex, static_cast<unsigned int>(j + 2)) = d[j] * targetVec[j + 2][xyz];
         }
      }

      // Now solve for range (eq. 20, ref. 1)

      // Note that the commented block below is equivalent to the method using pseudo-invert.
      // The latter is currently preferred as it is likely faster, but either should work equivalently.

      // 1st method to compute range vector:
      // UtMatrixd Mt;
      // Mt.Transpose(M);
      // UtMatrixd MMt;
      // MMt.Multiply(Mt, M);
      // UtMatrixd MMtInv;
      // MMtInv.Invert(MMt);
      // UtMatrixd Mtpsi;
      // Mtpsi.Multiply(Mt, psi);
      // rhoVec.Multiply(MMtInv, Mtpsi);

      // 2nd method to compute range vector:
      UtMatrixd MInv;
      MInv.PseudoInvert(M);
      rhoVec.Multiply(MInv, psi);

      // Compute provisional new locations based on the new range vector (this may be modified if the solution is diverging).
      ComputeLocations(rhoVec, targetVec, siteLoc, nextLocECI);

      if (iterations >= 1)
      {
         // Determine how well the solution is converging in range by computing the maximum difference from the last iteration.
         double maxDeltaRho = 0.0;
         maxDeltaRhoPercent = 0.0;
         for (unsigned int i = 0; i < size; ++i)
         {
            double deltaRho        = fabs(rhoVec(i) - lastRhoVec(i));
            double deltaRhoPercent = deltaRho / rhoVec(i);
            if (deltaRhoPercent > maxDeltaRhoPercent)
            {
               maxDeltaRho        = deltaRho;
               maxDeltaRhoPercent = deltaRhoPercent;
            }
         }

         // The solution is diverging if the difference in range percent is greater than the least recorded difference.
         // We cannot know if this is occurring until after one iteration.
         isDiverging = (maxDeltaRhoPercent > leastMaxDeltaRhoPercent);

         // If the current maximum difference between the last iteration is the least, save the current "best" range vector.
         if (!isDiverging)
         {
            leastMaxDeltaRho        = maxDeltaRho;
            leastMaxDeltaRhoPercent = maxDeltaRhoPercent;
            bestRhoVec              = rhoVec;
         }

         // Determine if the solution has converged (in which case we will save off the final results and break out of the loop).
         isConvergent = (maxDeltaRho <= mAnglesOnlyLinearTolerance);
      }
      if (isDiverging && (!isDivergent))
      {
         // Bracket the solution for further searches.
         maxRhoVec        = rhoVec;
         minRhoVec        = bestRhoVec;
         rhoVec           = bestRhoVec;
         deltaRhoVecScale = deltaRhoVecScaleIncrement;
      }

      if (!isDiverging)
      {
         constexpr double cDELTA_RHO_PERCENT_THRESHOLD = 0.05; // Arbitrary.
         if (maxDeltaRhoPercent > cDELTA_RHO_PERCENT_THRESHOLD)
         {
            // Average the new solution with the previously computed one
            // (this averaging dampens large oscillations that can occur when iterating far from the actual solution.
            for (unsigned int i = 0; i < size; ++i)
            {
               aLocECI[i].Add(nextLocECI[i]);
               aLocECI[i].Divide(2.0);
            }
            ComputeLocations(rhoVec, targetVec, siteLoc, aLocECI);
         }
         else
         {
            // The solution has converged enough that oscillations are not a problem.
            // The velocities were already computed above, so there is no need to re-compute them here.
            aLocECI = nextLocECI;
         }
         isDivergent = false;
      }
      else // isDiverging == true
      {
         // Perform a bracketed search, starting at the last, best range solution and incrementing to the last
         // calculated solution. Stepping away from a known good solution seems to be preferable to bisection, as it can
         // lead to wild swings in range between iterations. Note that this will fail if initial iterations could not
         // produce good enough solutions that the correct point is not in the min/max limits.
         rhoVec = bestRhoVec + deltaRhoVecScale * (maxRhoVec - minRhoVec);
         ComputeLocations(rhoVec, targetVec, siteLoc, aLocECI);
         isDivergent = true;
         deltaRhoVecScale += deltaRhoVecScaleIncrement;
         if (deltaRhoVecScale >= 1.0)
         {
            // Attempt the search again with greater granularity.
            deltaRhoVecScaleIncrement /= 10.0;
            deltaRhoVecScale = deltaRhoVecScaleIncrement;
         }
      }

      // If we found an acceptable solution, break out of the loop.
      if (isConvergent)
      {
         break;
      }

      ComputeVelocities(aData, aLocECI, aVelECI);
      lastRhoVec = rhoVec;
      iterations += 1;
   }

   if (!isConvergent)
   {
      if (mDebug)
      {
         auto logger = ut::log::debug() << "Max iterations (angles only IOD)";
         logger.AddNote() << "Last range difference in solution convergence:     " << leastMaxDeltaRho << " m";
         logger.AddNote() << "Expected range difference in solution convergence: " << mAnglesOnlyLinearTolerance << " m";
         logger.AddNote() << "Last Computed ECI location: " << aLocECI.back();
      }
      return false;
   }

   return true;
}

//! Given a set of measurements and pre-computed locations, compute the corresponding velocities using the Lambert solver.
void WsfOrbitDeterminationFusion::ComputeVelocities(const MeasurementList&      aData,
                                                    const std::vector<UtVec3d>& aLocECI,
                                                    std::vector<UtVec3d>&       aVelECI)
{
   auto size = static_cast<unsigned int>(aData.size());
   for (size_t i = 1; i < size; ++i)
   {
      auto                     i1     = i - 1;
      auto                     i2     = i;
      double                   dt     = aData[i2]->GetUpdateTime() - aData[i1]->GetUpdateTime();
      UtLambertProblem::Result result = UtLambertProblem::Universal(aLocECI[i1],
                                                                    aLocECI[i2],
                                                                    dt,
                                                                    GetPlatform()->GetCentralBody().GetEllipsoid(),
                                                                    true,
                                                                    mLambertConvergenceTolerance);
      aVelECI[i1]                     = result.GetInitialVelocity();
      aVelECI[i2]                     = result.GetFinalVelocity();
      if (!result.IsSolution())
      {
         // Compute approximate velocities based on a circular orbit and the Vis-Viva equation.
         // Typically this does not have much of an effect on convergence, but it is technically required to
         // fully solve for the f's and g's in AnglesOnlyKinematicSolution.
         aVelECI[i1] = aLocECI[i2] - aLocECI[i1];
         aVelECI[i1].Normalize();
         double speed = sqrt(GetPlatform()->GetCentralBody().GetGravitationalParameter() / aLocECI[i1].Magnitude());
         aVelECI[i1] *= speed;
         aVelECI[i2] = aVelECI[i1];
      }
   }
}
// Todo unit test
// double WsfOrbitDeterminationFusion::ComputeRangeError(WsfTrack&           aRBE_Measurement,
//                                                      const UtCovariance& aStateCovariance)
//{
//   // Find the angle of rotation.  The idea is to align a rotated coordinate system along the target vector,
//   // so that the x axes of that coordinate system align with that vector,
//   // then compare the length of the vector with with sum of the lengths of the covariance matrices' x-x component.
//   double x[3] = { 1.0, 0.0, 0.0};
//   UtVec3d targetVecWCS;
//   UtMeasurementUtil::LocationWCS(aRBE_Measurement.GetOriginatorLocationWCS().GetData(),
//                                   aRBE_Measurement.GetRange(),
//                                   aRBE_Measurement.GetBearing(),
//                                   aRBE_Measurement.GetElevation(),
//                                   targetVecWCS.GetData());
//   targetVecWCS.Subtract(aRBE_Measurement.GetOriginatorLocationWCS());
//   double angle = acos(std::max(-1.0, std::min(targetVecWCS.DotProduct(x) / targetVecWCS.Magnitude(), 1.0)));
//   UtVec3d axis;
//   axis.CrossProduct(targetVecWCS, x);
//   axis.Normalize();
//   UtDCM rotMatrixDCM(angle, axis.GetData());
//   double rotMatrix[3][3];
//   rotMatrixDCM.Get(rotMatrix);
//   UtCovariance rotStateCovariance = aStateCovariance;
//   UtCovariance::ConvertFromFrameA_ToFrameB(rotStateCovariance, rotMatrix);
//   return sqrt(rotStateCovariance(0, 0));
//}
//! Compute Lambert / Gauss "f" and "g" coefficients
//! (see Bate, Mueller, and White, Fundamentals of Astrodynamics, pp.256-258).
//! This algorithm computes the coefficients up to 6th order in r and v.
//! @param aLocECI The referenced ECI location.
//! @param aVelECI The referenced ECI velocity.
//! @param aDeltaT The time to extrapolate forward in computation of the f's and g's.
//! @param aF (out) The computed "f" value.
//! @param aG (out) The computed "g" value.
void WsfOrbitDeterminationFusion::ComputeLambertf_g(const UtVec3d& aLocECI,
                                                    const UtVec3d& aVelECI,
                                                    double         aDeltaT,
                                                    double&        aF,
                                                    double&        aG)
{
   double rMag  = aLocECI.Magnitude();
   double rDotV = UtVec3d::DotProduct(aLocECI, aVelECI);
   double u     = GetPlatform()->GetCentralBody().GetGravitationalParameter() / rMag / rMag / rMag;
   double p     = rDotV / rMag / rMag;
   double q     = aVelECI.MagnitudeSquared() / rMag / rMag - u;
   double dt2   = aDeltaT * aDeltaT;
   double dt3   = dt2 * aDeltaT;
   double dt4   = dt3 * aDeltaT;
   double dt5   = dt4 * aDeltaT;
   double dt6   = dt5 * aDeltaT;
   double p2    = p * p;
   double q2    = q * q;
   double u2    = u * u;
   double up    = u * p;
   double up2   = u * p2;

   // Equation 5.5-26
   aF = 1.0 - 0.5 * u * dt2 + 0.5 * up * dt3 + 1.0 / 24.0 * u * (u - 15 * p2 + 3 * q) * dt4 +
        0.125 * up * (7.0 * p2 - u - 3.0 * q) * dt5 +
        (105.0 * up2 * (-9.0 * p2 + 6.0 * q + 2.0 * u) - u * (45.0 * q2 + 24.0 * up + u2)) / 720.0 * dt6;

   // Equation 5.5-27
   aG = aDeltaT - 1.0 / 6.0 * u * dt3 + 0.25 * up * dt4 + 1.0 / 120.0 * u * (u - 45.0 * p2 + 9 * q) * dt5 +
        30 * up * (14.0 * p2 - 6.0 * q - u) / 720.0 * dt6;
}

// private
bool WsfOrbitDeterminationFusion::CreateFilterOnTrack(double                   aSimTime,
                                                      WsfLocalTrack&           aLocalTrack,
                                                      UtOrbitalPropagatorBase& aPropagator)
{
   auto kalmanFilterPtr = ut::clone(mPrototypeFilter);

   //! If no propagator was configured in the filter, set it to be the target propagator.
   if (kalmanFilterPtr->UsingDefaultPropagator())
   {
      kalmanFilterPtr->SetStatePropagator(ut::clone(aPropagator));
   }

   UtCalendar epochSimTime;
   GetSimCalendarTime(aSimTime, epochSimTime);

   // Initialize with the rv state of the propagator.
   aPropagator.Update(epochSimTime);
   if (kalmanFilterPtr->Initialize(aSimTime, aPropagator.GetOrbitalState()))
   {
      aLocalTrack.SetFilter(kalmanFilterPtr.release());
      return true;
   }

   return false;
}

//! Given a track with valid bearing and elevation values, determine an ECI unit target vector and ECI site location.
void WsfOrbitDeterminationFusion::ConvertBearingElevation(const UtMeasurementData& aBearingElevationTrack,
                                                          UtVec3d&                 aUnitTargetVecECI,
                                                          UtVec3d&                 aSiteLocationECI)
{
   UtCalendar referenceTime = GetTrackManager().GetSimulation()->GetDateTime().GetStartDateAndTime();
   referenceTime.AdvanceTimeBy(aBearingElevationTrack.GetUpdateTime());
   double locWCS[3];
   aBearingElevationTrack.GetOriginatorLocationWCS(locWCS);
   UtECI_Conversion convert(referenceTime, GetSimulation()->GetEnvironment().GetCentralBody());
   convert.ConvertWCS_ToECI(locWCS, aSiteLocationECI.GetData());
   double  bearing   = aBearingElevationTrack.GetBearing();
   double  elevation = aBearingElevationTrack.GetElevation();
   double  sinB      = sin(bearing);
   double  cosB      = cos(bearing);
   double  sinE      = sin(elevation);
   double  cosE      = cos(elevation);
   double  vecNED[3] = {cosB * cosE, sinB * cosE, -sinE};
   UtVec3d locECI;
   UtVec3d::Multiply(vecNED, 40000.0);
   double transWCS_NED[3][3];
   double lat, lon, alt;
   UtEllipsoidalEarth::ConvertECEFToLLA(locWCS, lat, lon, alt);
   UtEllipsoidalEarth::ComputeNEDTransform(lat, lon, alt, transWCS_NED);

   // This "target location" is purely notional; representing the head of the WCS unit target vector.
   double targetLocWCS[3];
   UtEllipsoidalEarth::ConvertLocalToECEF(locWCS, transWCS_NED, vecNED, targetLocWCS);
   convert.ConvertWCS_ToECI(targetLocWCS, locECI.GetData());

   aUnitTargetVecECI = locECI - aSiteLocationECI;
   aUnitTargetVecECI.Normalize();
}

//! Convert a WCS location in a track to ECI.  Either The track's location must be valid, or all of range, bearing, and
//! elevation must be valid.
void WsfOrbitDeterminationFusion::ConvertLocation(const UtECI_Conversion&  aConversion,
                                                  const UtMeasurementData& aLocationTrack,
                                                  UtVec3d&                 aTargetLocECI)
{
   UtVec3d locWCS;
   if (aLocationTrack.LocationValid())
   {
      aLocationTrack.GetLocationWCS(locWCS.GetData());
   }
   else
   {
      assert(aLocationTrack.RangeValid() && aLocationTrack.BearingValid() && aLocationTrack.ElevationValid());
      double refLocWCS[3];
      aLocationTrack.GetOriginatorLocationWCS(refLocWCS);
      UtMeasurementUtil::LocationWCS(refLocWCS,
                                     aLocationTrack.GetRange(),
                                     aLocationTrack.GetBearing(),
                                     aLocationTrack.GetElevation(),
                                     locWCS.GetData());
   }
   aTargetLocECI = aConversion.ConvertWCS_ToECI(locWCS);
}

//! Return a list, sorted in time, of the available fusion candidates for a given track.
//! @param aLocalTrack The local track containing the available track history that will be used.
//! @param aNonLocalTrack The Latest update in the nonlocal track.
//! @param aGetAnglesCandidates Whether to check only for valid bearing-elevation data for angles-only measurements, or
//! to check for valid location data.
//! @note Two sensors with the same update rate can report at exactly the same time, causing one of them to be thrown
//! out for bearing-only fusion candidates.
WsfOrbitDeterminationFusion::MeasurementList WsfOrbitDeterminationFusion::GetFusionCandidates(WsfLocalTrack& aLocalTrack,
                                                                                              const WsfTrack& aNonLocalTrack,
                                                                                              bool aGetAnglesCandidates)
{
   // Check the local track history
   // If we have accumulated at least the threshold number of angle measurements, attempt a solution.
   MeasurementList fuseCandidates;
   for (const auto& candidatePtr : aLocalTrack.GetHistory())
   {
      if ((aGetAnglesCandidates && candidatePtr->BearingValid() && candidatePtr->ElevationValid()) ||
          IsLocation(*candidatePtr))
      {
         fuseCandidates.push_back(candidatePtr.get());
      }
   }

   fuseCandidates.push_back(&aNonLocalTrack);

   // Comparison function needed for sorting measurements
   auto timeSortComparison = [](const UtMeasurementData* aLHS, const UtMeasurementData* aRHS)
   { return (aLHS->GetUpdateTime() > aRHS->GetUpdateTime()); };

   // Evaluate these with the most recent observation first.
   std::sort(fuseCandidates.begin(), fuseCandidates.end(), timeSortComparison);

   MeasurementList finalFuseCandidates;

   for (size_t i = 1; i < fuseCandidates.size(); ++i)
   {
      auto                    i1                             = i - 1;
      auto                    i2                             = i;
      auto                    fc1                            = fuseCandidates[i1];
      auto                    fc2                            = fuseCandidates[i2];
      static constexpr double cMIN_TIME_BETWEEN_MEASUREMENTS = 0.01; // second
      if ((fc1->GetUpdateTime() - fc2->GetUpdateTime()) >= cMIN_TIME_BETWEEN_MEASUREMENTS)
      {
         finalFuseCandidates.push_back(fc1);

         if (i == (fuseCandidates.size() - 1))
         {
            finalFuseCandidates.push_back(fc2);
         }
      }
      else if (GetSimulation()->GetRandom().Bernoulli())
      {
         // For tracks with the same detection time, randomly swap so that one sensor's tracks aren't preferentially selected.
         fuseCandidates[i2] = fc1;
         fuseCandidates[i1] = fc2;
      }

      if (aGetAnglesCandidates && (finalFuseCandidates.size() == mNumberOfAnglesMeasurementsNeeded))
      {
         break;
      }
   }

   std::reverse(finalFuseCandidates.begin(), finalFuseCandidates.end());

   if (mDebug)
   {
      auto logger = ut::log::debug() << "Final Fuse Candidates";
      for (size_t i = 0; i < finalFuseCandidates.size(); ++i)
      {
         logger.AddNote() << i << " " << finalFuseCandidates[i]->GetUpdateTime() << " : "
                          << static_cast<const WsfTrack*>(finalFuseCandidates[i])->GetTrackId();
      }
   }

   return finalFuseCandidates;
}

//! Given a set of fusion candidates in track history that have a location (i.e., not just bearing-elevation), fuse them
//! using the Lambert-Universal algorithm. Currently only the first and last measurements are fused.  TODO alternately
//! use the angles-only algorithm but use the location data in the first pass.
//! @param aSimTime The simulation time for fusing observations.
//! @param aLocalTrack The local track for the object whose orbit is being calculated.
//! @param aNonLocalTrack A track update containing the latest information about the target.
bool WsfOrbitDeterminationFusion::FuseInitialLocations(double          aSimTime,
                                                       WsfLocalTrack&  aLocalTrack,
                                                       const WsfTrack& aNonLocalTrack)
{
   // Get a list of all fusion candidates in a time ordered vector format.
   MeasurementList fuseCandidates = GetFusionCandidates(aLocalTrack, aNonLocalTrack, false);

   bool   success = false;
   size_t size    = fuseCandidates.size();
   if (size >= 2)
   {
      if (mDebug)
      {
         auto logger = ut::log::debug() << "Attempting locations IOD";
         logger.AddNote() << "Track: " << aNonLocalTrack.GetTargetName();
      }

      const WsfMeasurement& measurement1 = *fuseCandidates[0];
      const WsfMeasurement& measurement2 = *fuseCandidates[size - 1];

      // Use Lambert algorithm to find a solution for initial velocities
      UtVec3d    locECI1;
      UtVec3d    locECI2;
      UtVec3d    velECI1;
      UtVec3d    velECI2;
      UtCalendar time1;
      UtCalendar time2;
      GetSimCalendarTime(measurement1.GetUpdateTime(), time1);
      GetSimCalendarTime(measurement2.GetUpdateTime(), time2);
      UtECI_Conversion convert(time1, ut::EarthWGS84());
      ConvertLocation(convert, measurement1, locECI1);
      convert.SetEpoch(time2);
      ConvertLocation(convert, measurement2, locECI2);
      double dt = measurement2.GetUpdateTime() - measurement1.GetUpdateTime();
      auto   result =
         UtLambertProblem::Universal(locECI1, locECI2, dt, GetCentralBody().GetEllipsoid(), true, mLambertConvergenceTolerance);
      velECI1 = result.GetInitialVelocity();
      velECI2 = result.GetFinalVelocity();
      success = result.IsSolution();

      if (success)
      {
         if (mDebug)
         {
            auto logger = ut::log::debug() << "Found solution";
            logger.AddNote() << "T = " << aSimTime;
            logger.AddNote() << "Track: " << aNonLocalTrack.GetTargetName();
         }
         UtOrbitalPropagatorBase* propPtr = GetPropagator(aLocalTrack);

         if (propPtr == nullptr)
         {
            propPtr = AddPropagator(aLocalTrack);
         }

         // Use the second point to initialize the propagator.
         ut::OrbitalState initialState(time2,
                                       GetCentralBody(),
                                       ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                       ut::OrbitalState::ReferenceFrame::cECI,
                                       ut::OrbitalState::Vector(locECI2, velECI2));
         success = ((propPtr->HyperbolicPropagationAllowed() || (!initialState.OrbitIsHyperbolic())) &&
                    propPtr->Initialize(initialState) &&
                    CreateFilterOnTrack(measurement2.GetUpdateTime(), aLocalTrack, *propPtr));
         if (success)
         {
            // Set location and velocity information on the track from the converted ECI estimates from the propagator.
            UtCalendar simCalendarTime;
            GetSimCalendarTime(aSimTime, simCalendarTime);
            propPtr->Update(simCalendarTime);
            ut::OrbitalState::Vector osvInertial = propPtr->GetOrbitalState().GetOrbitalStateVectorInertial();
            locECI2                              = osvInertial.GetLocation();
            velECI2                              = osvInertial.GetVelocity();
            convert.SetEpoch(simCalendarTime);
            UtVec3d locWCS;
            UtVec3d velWCS;
            convert.ConvertECI_ToWCS(locECI2.GetData(), locWCS.GetData());
            convert.ConvertVelocityECI_ToWCS(locECI2.GetData(), velECI2.GetData(), velWCS.GetData());
            aLocalTrack.SetLocationWCS(locWCS.GetData());
            aLocalTrack.SetVelocityWCS(velWCS.GetData());

            // Inform observers that IOD is complete.
            WsfObserver::OrbitDeterminationInitiated(GetSimulation())(aSimTime, GetPlatform(), aLocalTrack, *propPtr);

            if (mDebug)
            {
               WsfPlatform* platformPtr = GetSimulation()->GetPlatformByIndex(aNonLocalTrack.GetTargetIndex());
               if (platformPtr != nullptr)
               {
                  platformPtr->Update(aSimTime);
                  UtVec3d locWCS;
                  platformPtr->GetLocationWCS(locWCS.GetData());
                  UtVec3d diffLoc;
                  diffLoc.Subtract(aLocalTrack.GetLocationWCS().GetData(), locWCS.GetData());
                  auto logger = ut::log::debug() << "Track-truth location";
                  logger.AddNote() << "Difference: " << diffLoc.Magnitude();
               }
            }
         }
         else if (mDebug)
         {
            auto logger = ut::log::warning() << "Lambert trial failed; continuing";
            logger.AddNote() << "No solution for " << aNonLocalTrack.GetTargetName();
         }
      }
   }
   return success;
}

//! Attempt to fuse a number of angles accumulated in track history from a local track.  Include the current
//! measurement.
//! @param aSimTime The current simulation time.
//! @param aLocalTrack The local track to be updated with the fused angles.  This track should contain a track history
//! of at least four measurements in order for fusion to become possible.
//! @param aNonLocalTrack The nonlocal track that also may contain a valid measurement for fusion.
bool WsfOrbitDeterminationFusion::FuseInitialAngles(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack)
{
   // Check the local track history
   // If we have accumulated at least the threshold number of angle measurements, attempt a solution.
   MeasurementList fuseCandidates = GetFusionCandidates(aLocalTrack, aNonLocalTrack, true);

   // location tracks
   bool success = false;

   // todo determine other criteria for fusion (time difference, etc).
   if ((!aLocalTrack.GetAuxDataConst().AttributeExists("propagator")) &&
       fuseCandidates.size() >= mNumberOfAnglesMeasurementsNeeded)
   {
      if (mDebug)
      {
         auto logger = ut::log::debug() << "Attempting angles-only IOD";
         logger.AddNote() << "Track: " << aNonLocalTrack.GetTargetName();
         WsfPlatform* targetPtr = GetSimulation()->GetPlatformByIndex(aNonLocalTrack.GetTargetIndex());
         if (targetPtr != nullptr)
         {
            UtVec3d locECI;
            targetPtr->GetLocationECI(locECI.GetData());
            UtVec3d velECI;
            targetPtr->GetVelocityECI(velECI.GetData());
            logger.AddNote() << "Target Truth Location: " << locECI;
            logger.AddNote() << "Target Truth Velocity: " << velECI;
         }
      }

      std::vector<UtVec3d> locECI_Vec;
      std::vector<UtVec3d> velECI_Vec;
      success = AnglesOnlyKinematicSolution(fuseCandidates, locECI_Vec, velECI_Vec);

      if (success)
      {
         UtVec3d& locECI = locECI_Vec[1];
         UtVec3d& velECI = velECI_Vec[1];

         UtCalendar referenceTime = GetTrackManager().GetSimulation()->GetDateTime().GetStartDateAndTime();
         referenceTime.AdvanceTimeBy(fuseCandidates[1]->GetUpdateTime());
         ut::OrbitalState initialState(referenceTime,
                                       GetCentralBody(),
                                       ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                       ut::OrbitalState::ReferenceFrame::cECI,
                                       ut::OrbitalState::Vector(locECI, velECI));
         if (mDebug)
         {
            auto logger = ut::log::debug() << "Found Solution";
            logger.AddNote() << "Track: " << aNonLocalTrack.GetTargetName();
         }

         // Add a propagator and initialize it.
         UtOrbitalPropagatorBase* propPtr = AddPropagator(aLocalTrack);
         success                          = propPtr->Initialize(initialState) &&
                   CreateFilterOnTrack(fuseCandidates[1]->GetUpdateTime(), aLocalTrack, *propPtr);

         if (success)
         {
            // Set location and velocity information on the track from the converted ECI estimates from the propagator.
            UtVec3d locWCS;
            UtVec3d velWCS;
            GetSimCalendarTime(aSimTime, referenceTime);
            propPtr->Update(referenceTime);
            ut::OrbitalState::Vector osvInertial = propPtr->GetOrbitalState().GetOrbitalStateVectorInertial();
            locECI                               = osvInertial.GetLocation();
            velECI                               = osvInertial.GetVelocity();
            UtECI_Conversion convert(referenceTime, GetSimulation()->GetEnvironment().GetCentralBody());
            locWCS = convert.ConvertECI_ToWCS(locECI);
            velWCS = convert.ConvertVelocityECI_ToWCS(locECI, velECI);
            aLocalTrack.SetLocationWCS(locWCS.GetData());
            aLocalTrack.SetVelocityWCS(velWCS.GetData());
            aLocalTrack.Set3D(true);
            aLocalTrack.SetUpdateTime(aSimTime);

            if (mDebug)
            {
               WsfPlatform* platformPtr = GetSimulation()->GetPlatformByIndex(aNonLocalTrack.GetTargetIndex());
               if (platformPtr != nullptr)
               {
                  platformPtr->Update(aSimTime);
                  UtVec3d locWCS;
                  platformPtr->GetLocationWCS(locWCS.GetData());
                  UtVec3d velWCS;
                  platformPtr->GetVelocityWCS(velWCS.GetData());
                  UtVec3d diffLoc;
                  diffLoc.Subtract(aLocalTrack.GetLocationWCS().GetData(), locWCS.GetData());
                  auto logger = ut::log::debug() << "Track-truth range";
                  logger.AddNote() << "Difference: " << diffLoc.Magnitude();
               }
            }
            // Inform observers that IOD is complete.
            WsfObserver::OrbitDeterminationInitiated(GetSimulation())(aSimTime, GetPlatform(), aLocalTrack, *propPtr);
         }
      }
      if (mDebug && (!success))
      {
         auto logger = ut::log::debug() << "No Solution";
         logger.AddNote() << "Track: " << aNonLocalTrack.GetTargetName();
      }
   }

   return success;
}

bool WsfOrbitDeterminationFusion::IsBearingElevation(const UtMeasurementData& aTrack)
{
   return ((!aTrack.LocationValid()) && (!aTrack.RangeValid()) && (aTrack.BearingValid()) && (aTrack.ElevationValid()));
}

//! Helper method to determine if a track has range and should be processed with the range-based
//! algorithm (Lambert Universal).
bool WsfOrbitDeterminationFusion::IsLocation(const UtMeasurementData& aTrack)
{
   return ((aTrack.LocationValid()) || (aTrack.RangeValid() && aTrack.BearingValid() && aTrack.ElevationValid()));
}

//! Helper method to get the current absolute time.
//!@param aSimTime The current relative simulation time.
//!@param aSimCalendarTime The current absolute simulation time.
void WsfOrbitDeterminationFusion::GetSimCalendarTime(double aSimTime, UtCalendar& aSimCalendarTime)
{
   aSimCalendarTime = GetTrackManager().GetSimulation()->GetDateTime().GetCurrentTime(aSimTime);
}

bool WsfOrbitDeterminationFusion::UpdateTrackFromEstimate(double             aSimTime,
                                                          WsfLocalTrack&     aLocalTrack,
                                                          UtMeasurementData& aEstimate)
{
   bool localTrackUpdated = false;
   if (aEstimate.LocationValid())
   {
      localTrackUpdated = true;
      UtVec3d trackLocWCS;
      aEstimate.GetLocationWCS(trackLocWCS.GetData());
      aLocalTrack.SetLocationWCS(trackLocWCS.GetData());
      aLocalTrack.Set3D(true);
      aLocalTrack.SetUpdateTime(aSimTime);
      UtOrbitalPropagatorBase* propPtr = GetPropagator(aLocalTrack);
      assert(propPtr != nullptr);

      if (aEstimate.VelocityValid())
      {
         UtVec3d trackVelWCS;
         aEstimate.GetVelocityWCS(trackVelWCS.GetData());
         aLocalTrack.SetVelocityWCS(trackVelWCS.GetData());
         aLocalTrack.SetWCS_VelocityValid(true);
         assert(aEstimate.StateCovarianceValid());
         aLocalTrack.SetStateCovariance(aSimTime, *aEstimate.GetStateCovariance());

         // Update the propagator to reflect the new position and velocity
         UtCalendar simCalendarTime;
         GetSimCalendarTime(aSimTime, simCalendarTime);
         UtVec3d          trackLocECI;
         UtVec3d          trackVelECI;
         UtECI_Conversion convert(simCalendarTime, GetSimulation()->GetEnvironment().GetCentralBody());
         convert.ConvertWCS_ToECI(trackLocWCS.GetData(), trackLocECI.GetData());
         convert.ConvertVelocityWCS_ToECI(trackLocWCS.GetData(), trackVelWCS.GetData(), trackVelECI.GetData());
         ut::OrbitalState newState(simCalendarTime,
                                   GetCentralBody(),
                                   ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                   ut::OrbitalState::ReferenceFrame::cECI,
                                   ut::OrbitalState::Vector(trackLocECI, trackVelECI));
         propPtr->SetInitialOrbitalState(newState);
         if (propPtr->Initialize(simCalendarTime))
         {
            if (mDebug)
            {
               auto logger = ut::log::debug() << "Successful OD update";
               logger.AddNote() << "T = " << aSimTime;
               logger.AddNote() << "Track: " << aLocalTrack.GetTargetName();
            }
         }
         WsfObserver::OrbitDeterminationUpdated(GetSimulation())(aSimTime, GetPlatform(), aLocalTrack, *propPtr);
      }
      else // velocity not valid
      {
         // Get it from the propagator.
         UtCalendar simCalendarTime;
         GetSimCalendarTime(aSimTime, simCalendarTime);
         propPtr->Update(simCalendarTime);
         auto osvWCS = propPtr->GetOrbitalState().GetOrbitalStateVectorWCS();
         aLocalTrack.SetVelocityWCS(osvWCS.GetVelocity().GetData());
      }
   }

   return localTrackUpdated;
}

//! Print diagnostic data about angles-only observations, comparing them with the current target platform data.
//! @note The target platform data are only displayed for the current time, so it will not match older track data well.
void WsfOrbitDeterminationFusion::PrintAnglesOnlyData(const WsfTrack& aTrack,
                                                      const UtVec3d&  aLocationECI,
                                                      const UtVec3d&  aVelocityECI,
                                                      double          aRange) const
{
   WsfPlatform* targetPtr     = GetSimulation()->GetPlatformByIndex(aTrack.GetTargetIndex());
   WsfPlatform* originatorPtr = GetSimulation()->GetPlatformByIndex(aTrack.GetOriginatorIndex());
   if ((targetPtr != nullptr) && (originatorPtr != nullptr))
   {
      double targetLocECI[3];
      targetPtr->GetLocationECI(targetLocECI);
      double targetVelECI[3];
      targetPtr->GetVelocityECI(targetVelECI);
      double originatorLocECI[3];
      originatorPtr->GetLocationECI(originatorLocECI);
      UtVec3d targetVecECI;
      targetVecECI.Subtract(targetLocECI, originatorLocECI);
      auto logger = ut::log::info() << "Diagnostic Data:";
      logger.AddNote() << aTrack.GetTrackId() << " rho, rho(true), r, r(true) " << aRange << ", "
                       << targetVecECI.Magnitude() << ", " << aLocationECI.Magnitude() << ", "
                       << UtVec3d::Magnitude(targetLocECI);
      logger.AddNote() << "locECI: " << aLocationECI[0] << ", " << aLocationECI[1] << ", " << aLocationECI[2];
      logger.AddNote() << "velECI: " << aVelocityECI[0] << ", " << aVelocityECI[1] << ", " << aVelocityECI[2];
      logger.AddNote() << "locECI(true): " << targetLocECI[0] << ", " << targetLocECI[1] << ", " << targetLocECI[2];
      logger.AddNote() << "velECI(true): " << targetVelECI[0] << ", " << targetVelECI[1] << ", " << targetVelECI[2];
   }
}

//! Get an existing propagator on the track.
UtOrbitalPropagatorBase* WsfOrbitDeterminationFusion::GetPropagator(const WsfTrack& aTrack)
{
   UtOrbitalPropagatorBase* propPtr = nullptr;
   if (aTrack.GetAuxDataConst().AttributeExists("propagator"))
   {
      std::shared_ptr<UtOrbitalPropagatorBase> sharedPropPtr = nullptr;
      aTrack.GetAuxDataConst().GetAttribute("propagator").Get(sharedPropPtr);
      propPtr = sharedPropPtr.get();
   }
   return propPtr;
}

//! Add a propagator to a track.
UtOrbitalPropagatorBase* WsfOrbitDeterminationFusion::AddPropagator(WsfTrack& aTrack)
{
   if (aTrack.GetAuxDataConst().AttributeExists("propagator"))
   {
      aTrack.GetAuxData().Delete("propagator");
   }
   auto propPtrAttrPtr = ut::make_unique<UtAttribute<std::shared_ptr<UtOrbitalPropagatorBase>>>();
   auto orbitalState   = ut::make_unique<ut::OrbitalState>(GetCentralBody(),
                                                         ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                                         ut::OrbitalState::ReferenceFrame::cECI);
   std::shared_ptr<UtOrbitalPropagatorBase> sharedPropPtr{nullptr};
   auto targetPlatformPtr = GetSimulation()->GetPlatformByIndex(aTrack.GetTargetIndex());
   if (!mPrototypeFilter.UsingDefaultPropagator())
   {
      // Preferentially use the filter's configured propagator.
      sharedPropPtr = ut::clone(mPrototypeFilter.GetStatePropagator());
      ;
   }
   else if (targetPlatformPtr != nullptr)
   {
      auto smbPtr = dynamic_cast<WsfSpaceMoverBase*>(targetPlatformPtr->GetMover());
      if (smbPtr != nullptr)
      {
         // Use the target propagator.
         sharedPropPtr = ut::clone(smbPtr->GetPropagator());
      }
      else
      {
         // Target mover not a space mover; use a Keplerian propagator.
         sharedPropPtr = std::make_shared<wsf::space::KeplerianOrbitalPropagator>(std::move(orbitalState));
      }
   }
   else
   {
      // Target platform not identified; use a Keplerian propagator.
      sharedPropPtr = std::make_shared<wsf::space::KeplerianOrbitalPropagator>(std::move(orbitalState));
   }
   propPtrAttrPtr->Set(sharedPropPtr);
   propPtrAttrPtr->SetName("propagator");
   aTrack.GetAuxData().AddAttribute(std::move(propPtrAttrPtr));
   assert(aTrack.GetAuxDataConst().AttributeExists("propagator"));
   return sharedPropPtr.get();
}

const ut::CentralBody& WsfOrbitDeterminationFusion::GetCentralBody() const
{
   return GetTrackManager().GetSimulation()->GetEnvironment().GetCentralBody();
}
