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

#include "UtOrbitalPropagator.hpp"

#include <algorithm>
#include <iostream>
#include <memory>

#include "UtEarth.hpp"
#include "UtLambertProblem.hpp"
#include "UtLog.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtTwoLineElement.hpp"
#include "UtVec3.hpp"

UtOrbitalPropagator::UtOrbitalPropagator(std::unique_ptr<ut::OrbitalState> aInitialOrbitalStatePtr)
   : UtOrbitalPropagatorBase(std::move(aInitialOrbitalStatePtr))
{
}

UtOrbitalPropagator& UtOrbitalPropagator::operator=(const UtOrbitalPropagator& aRhs)
{
   if (this != &aRhs)
   {
      UtOrbitalPropagator tmp(aRhs);
      *this = std::move(tmp);
   }
   return *this;
}

// virtual
bool UtOrbitalPropagator::Initialize(const UtCalendar& aTime)
{
   // This propagator must have valid initial orbital elements in order to initialize properly:
   bool ok{false};

   if (UtOrbitalPropagatorBase::Initialize(aTime))
   {
      mPropagatedOrbitalStatePtr = ut::make_unique<ut::OrbitalState>(GetInitialOrbitalState());

      // Make sure the propagated orbital state references the inertial frame
      mPropagatedOrbitalStatePtr->SetReferenceFrame(ut::OrbitalState::ReferenceFrame::cECI);

      // Calculate cached values used for propagation.  This must be done first.
      // Calculate mag of angular momentum
      const UtOrbitalElements& oe              = mPropagatedOrbitalStatePtr->GetOrbitalElements();
      double                   periapsisRadius = oe.GetSemiMajorAxis() * (1 - oe.GetEccentricity());

      // Angular momentum is velocity * at periapsis time the periapsis radius.
      // Velocity from Vis-Viva equation.
      mAngularMomentum = periapsisRadius * sqrt(GetInitialOrbitalState().GetGravitationalParameter() *
                                                (2.0 / periapsisRadius - 1.0 / oe.GetSemiMajorAxis()));

      // Cached values
      mStartMeanAnomaly            = oe.GetMeanAnomaly();
      mSMA_Times_1_Minus_e_Squared = oe.GetSemiMajorAxis() * (1 - oe.GetEccentricity() * oe.GetEccentricity());

      if (mLinearTolerance > 0.0)
      {
         mTolerance = mLinearTolerance / (oe.GetSemiMajorAxis() * (1.0 + oe.GetEccentricity()));
      }

      ok = InitializeP(aTime); // See method documentation.
   }
   else
   {
      ut::log::error() << "Orbital elements do not contain sufficient information to describe the orbit.";
   }

   return ok;
}

//! Set the precession of the orbit in inertial space through application of secular J2 perturbation.
void UtOrbitalPropagator::SetPrecession(bool aSetPrecession)
{
   if (aSetPrecession)
   {
      SetPerturbation(ut::make_unique<J2_Perturbation>());
   }
}

// virtual
void UtOrbitalPropagator::Propagate(const UtCalendar& aEpoch)
{
   UtOrbitalElements oe = mPropagatedOrbitalStatePtr->GetOrbitalElements();

   // Advance mean anomaly and update to current epoch.
   oe.SetMeanAnomaly(
      UtMath::NormalizeAngle0_TwoPi(oe.GetMeanAnomaly() + oe.GetMeanMotion() * aEpoch.GetTimeSince(oe.GetEpoch())));
   oe.SetEpoch(aEpoch);
   mPropagatedOrbitalStatePtr->Set(oe);

   if (GetPerturbation() != nullptr)
   {
      GetPerturbation()->AddPerturbation(*mPropagatedOrbitalStatePtr);
   }

   PropagateTrueAnomalyP();
}

//! Perform two-body propagation, determining the new location, velocity and acceleration of the satellite.
void UtOrbitalPropagator::PropagateTrueAnomalyP()
{
   // The following call to GetTrueAnomaly does the actual propagation.
   const UtOrbitalElements& oe          = mPropagatedOrbitalStatePtr->GetOrbitalElements();
   double                   trueAnomaly = oe.GetTrueAnomaly(mTolerance, mMaxIterations);
   mSinTrueAnomaly                      = sin(trueAnomaly);
   mCosTrueAnomaly                      = cos(trueAnomaly);

   // Compute the radius from the central body (focus) to satellite.
   // (mSMA_Times_1_Minus_e_Squared is the semi-major axis times 1 - the eccentricity squared.)
   mRadius = mSMA_Times_1_Minus_e_Squared / (1 + oe.GetEccentricity() * mCosTrueAnomaly);

   // Compute the position on the ellipse using the computed radius and true anomaly.
   UtVec3d r(mRadius * mCosTrueAnomaly, mRadius * mSinTrueAnomaly, 0.0);

   const UtMat3d& rotMatrix = oe.GetPerifocalToInertialTransform();

   // Rotate using classical Euler angles to produce the correct 3-D vector.
   UtVec3d locInertial;
   rotMatrix.TransposeMultiply(locInertial, r);

   // velocity from Vis-Viva equation:
   mSpeed = sqrt(mPropagatedOrbitalStatePtr->GetGravitationalParameter() * (2.0 / mRadius - 1.0 / oe.GetSemiMajorAxis()));

   double sinTheta = mAngularMomentum / mRadius / mSpeed;
   double theta    = asin(UtMath::NormalizeSinCosMinusOne_One(sinTheta));
   if (oe.GetTrueAnomaly() > UtMath::cPI)
   {
      theta = UtMath::cPI - theta;
   }

   // Compute angle from periapsis for velocity, called gamma.
   double gamma    = theta + oe.GetTrueAnomaly();
   double sinGamma = sin(gamma);
   double cosGamma = cos(gamma);

   // Similar to location calculation, find the vector referenced to the ellipse,
   // then rotate by the classical Euler angles.
   UtVec3d v(mSpeed * cosGamma, mSpeed * sinGamma, 0.0);
   UtVec3d velocity;
   rotMatrix.TransposeMultiply(velocity, v);
}

bool UtOrbitalPropagator::J2_Perturbation::Initialize(UtOrbitalPropagatorBase& aPropagator)
{
   Perturbation::Initialize(aPropagator);
   ComputeJ2Perturbation(aPropagator.GetInitialOrbitalState());
   mInitialized = true;
   return mInitialized;
}

//! Compute the J2 Perturbation terms for
//! the right ascension of the ascending node, argument of periapsis, and mean anomaly.
//! Add these to the orbital elements and set in the propagator.
//! @Note This method is only valid for Earth-based propagation.
void UtOrbitalPropagator::J2_Perturbation::ComputeJ2Perturbation(const ut::OrbitalState& aState)
{
   const double&            ae              = UtEarth::cA;
   const UtOrbitalElements& orbitalElements = aState.GetOrbitalElements();
   double oneMinusESquared = (1 - orbitalElements.GetEccentricity() * orbitalElements.GetEccentricity());
   double sin_i            = sin(orbitalElements.GetInclination());
   double cos_i            = cos(orbitalElements.GetInclination());
   constexpr static double J2{
      0.0010826267}; // Vallado, Fundamentals of Astrodynamics and Applications (4th Edition), table inside back cover.

   // Compute time average of semi-major axis
   double k1     = 1.5 * J2 * ae * ae / orbitalElements.GetSemiMajorAxis() * sin_i * sin_i;
   double smaAve = orbitalElements.GetSemiMajorAxis() -
                   k1 * cos(2.0 * (orbitalElements.GetArgumentOfPeriapsis() + orbitalElements.GetMeanAnomaly()));
   double common = 1.5 * J2 * ae * ae * pow(smaAve, -3.5) * sqrt(aState.GetCentralBody().GetGravitationalParameter());

   mArgPeriapsisDot = common / oneMinusESquared / oneMinusESquared * (2.0 - 2.5 * sin_i * sin_i);
   mRAAN_Dot        = -common / oneMinusESquared / oneMinusESquared * (cos_i);
   mMeanAnomalyDot  = common / pow(oneMinusESquared, 1.5) * (1.0 - 1.5 * sin_i * sin_i);
}

bool UtOrbitalPropagator::J2_Perturbation::AddPerturbation(ut::OrbitalState& aState)
{
   if (mInitialized)
   {
      AdvanceOrbitalElements(aState);
      // TODO increase orbital velocity based on increased mean motion (constant value).
   }
   return mInitialized;
}

//! Compensate for secular precession of the orbital elements due to the Earth's oblateness.
//! @Note This method is only valid for Earth-based propagation.
void UtOrbitalPropagator::J2_Perturbation::AdvanceOrbitalElements(ut::OrbitalState& aState)
{
   double dt        = GetPropagator().GetTimePastEpoch() - mLastUpdateTime;
   double deltaRAAN = mRAAN_Dot * dt;

   UtOrbitalElements orbitalElements{aState.GetOrbitalElements()};
   double            RAAN = orbitalElements.GetRAAN() + deltaRAAN;
   RAAN                   = UtMath::NormalizeAngle0_TwoPi(RAAN);
   orbitalElements.SetRAAN(RAAN);

   // Must use get/set with mean anomaly because either the mean or true anomaly may be set.
   // This ensures proper conversion between the two.
   double meanAnomaly = orbitalElements.GetMeanAnomaly() + mMeanAnomalyDot * dt;
   orbitalElements.SetMeanAnomaly(UtMath::NormalizeAngle0_TwoPi(meanAnomaly));

   double argumentOfPeriapsis = orbitalElements.GetArgumentOfPeriapsis() + mArgPeriapsisDot * dt;
   argumentOfPeriapsis        = UtMath::NormalizeAngle0_TwoPi(argumentOfPeriapsis);
   orbitalElements.SetArgumentOfPeriapsis(argumentOfPeriapsis);

   aState.Set(orbitalElements);
   mLastUpdateTime = GetPropagator().GetTimePastEpoch();
}

// override
double UtOrbitalPropagator::GetTimeToPeriapsisPassage(unsigned aOrbitNum) const
{
   double timeToPeriapsisPassage = 0.0;
   if (!OrbitIsCircular())
   {
      timeToPeriapsisPassage = (UtMath::cTWO_PI - GetOrbitalElements().GetMeanAnomaly() + aOrbitNum * UtMath::cTWO_PI) /
                               GetOrbitalElements().GetMeanMotion();
   }
   else
   {
      timeToPeriapsisPassage = aOrbitNum * UtMath::cTWO_PI / GetOrbitalElements().GetMeanMotion();
   }
   return timeToPeriapsisPassage;
}

// override
double UtOrbitalPropagator::GetTimeToAscendingNodePassage(unsigned aOrbitNum) const
{
   double timeToAscendingNodePassage = 0.0;
   if (!OrbitIsEquatorial()) // non-equatorial
   {
      double meanAnomalyOfAscendingNode =
         UtOrbitalElements::ComputeMeanAnomaly(UtMath::cTWO_PI - GetOrbitalElements().GetArgumentOfPeriapsis(),
                                               GetOrbitalElements().GetEccentricity());
      double meanAnomalyToAscendingNode = meanAnomalyOfAscendingNode - GetOrbitalElements().GetMeanAnomaly();
      if (meanAnomalyToAscendingNode < 0.0)
      {
         meanAnomalyToAscendingNode += UtMath::cTWO_PI;
      }
      timeToAscendingNodePassage =
         (meanAnomalyToAscendingNode + aOrbitNum * UtMath::cTWO_PI) / GetOrbitalElements().GetMeanMotion();
   }
   else
   {
      timeToAscendingNodePassage = aOrbitNum * UtMath::cTWO_PI / GetOrbitalElements().GetMeanMotion();
   }
   return timeToAscendingNodePassage;
}

// override
double UtOrbitalPropagator::GetTimeSinceAscendingNodePassage(unsigned aOrbitNum) const
{
   return ((aOrbitNum + 1) * UtMath::cTWO_PI / GetOrbitalElements().GetMeanMotion() - GetTimeToAscendingNodePassage());
}

// override
double UtOrbitalPropagator::GetTimeSinceDescendingNodePassage(unsigned aOrbitNum) const
{
   return ((aOrbitNum + 1) * UtMath::cTWO_PI / GetOrbitalElements().GetMeanMotion() - GetTimeToDescendingNodePassage());
}

// override
double UtOrbitalPropagator::GetTimeToDescendingNodePassage(unsigned aOrbitNum) const
{
   double timeToDescendingNodePassage = 0.0;
   if (!OrbitIsEquatorial()) // non-equatorial
   {
      double trueAnomalyOfDescendingNode = UtMath::cPI - GetOrbitalElements().GetArgumentOfPeriapsis();
      if (trueAnomalyOfDescendingNode < 0.0)
      {
         trueAnomalyOfDescendingNode += UtMath::cTWO_PI;
      }
      double meanAnomalyOfDescendingNode =
         UtOrbitalElements::ComputeMeanAnomaly(trueAnomalyOfDescendingNode, GetOrbitalElements().GetEccentricity());
      double meanAnomalyToDescendingNode = meanAnomalyOfDescendingNode - GetOrbitalElements().GetMeanAnomaly();
      if (meanAnomalyToDescendingNode < 0.0)
      {
         meanAnomalyToDescendingNode += UtMath::cTWO_PI;
      }
      timeToDescendingNodePassage =
         (meanAnomalyToDescendingNode + aOrbitNum * UtMath::cTWO_PI) / GetOrbitalElements().GetMeanMotion();
   }
   else
   {
      timeToDescendingNodePassage = aOrbitNum * UtMath::cTWO_PI / GetOrbitalElements().GetMeanMotion();
   }
   return timeToDescendingNodePassage;
}

// override
bool UtOrbitalPropagator::GetTimeToAscendingRadius(double aRadius, double& aTime, unsigned aOrbitNumber) const
{
   double arg =
      (GetOrbitalElements().GetSemiMajorAxis() * (1.0 - pow(GetOrbitalElements().GetEccentricity(), 2)) / aRadius - 1.0) /
      GetOrbitalElements().GetEccentricity();
   bool ok = ((arg >= -1.0) && (arg <= 1.0));
   if (ok)
   {
      double trueAnomalyOfAscendingRadius = acos(arg);
      double meanAnomalyOfAscendingRadius =
         UtOrbitalElements::ComputeMeanAnomaly(trueAnomalyOfAscendingRadius, GetOrbitalElements().GetEccentricity());
      double meanAnomalyToAscendingRadius = meanAnomalyOfAscendingRadius - GetOrbitalElements().GetMeanAnomaly();
      if (meanAnomalyToAscendingRadius < 0.0)
      {
         meanAnomalyToAscendingRadius += UtMath::cTWO_PI;
      }
      aTime = (meanAnomalyToAscendingRadius + aOrbitNumber * UtMath::cTWO_PI) / GetOrbitalElements().GetMeanMotion();
   }
   return ok;
}

// override
bool UtOrbitalPropagator::GetTimeToDescendingRadius(double aRadius, double& aTime, unsigned aOrbitNumber) const
{
   double arg =
      (GetOrbitalElements().GetSemiMajorAxis() * (1.0 - pow(GetOrbitalElements().GetEccentricity(), 2)) / aRadius - 1.0) /
      GetOrbitalElements().GetEccentricity();
   bool ok = ((arg >= -1.0) && (arg <= 1.0));
   if (ok)
   {
      double trueAnomalyOfDescendingRadius = UtMath::cTWO_PI - acos(arg);
      double meanAnomalyOfDescendingRadius =
         UtOrbitalElements::ComputeMeanAnomaly(trueAnomalyOfDescendingRadius, GetOrbitalElements().GetEccentricity());
      double meanAnomalyToDescendingRadius = meanAnomalyOfDescendingRadius - GetOrbitalElements().GetMeanAnomaly();
      if (meanAnomalyToDescendingRadius < 0.0)
      {
         meanAnomalyToDescendingRadius += UtMath::cTWO_PI;
      }
      aTime = (meanAnomalyToDescendingRadius + aOrbitNumber * UtMath::cTWO_PI) / GetOrbitalElements().GetMeanMotion();
   }
   return ok;
}

// override
double UtOrbitalPropagator::GetTimeToApoapsisPassage(unsigned aOrbitNum) const
{
   double timeToApoapsisPassage = 0.0;
   if (!OrbitIsCircular())
   {
      if (GetOrbitalElements().GetMeanAnomaly() < UtMath::cPI)
      {
         timeToApoapsisPassage = (UtMath::cPI - GetOrbitalElements().GetMeanAnomaly() + aOrbitNum * UtMath::cTWO_PI) /
                                 GetOrbitalElements().GetMeanMotion();
      }
      else
      {
         // Geometrically it's easier to understand the expanded form:
         // (UtMath::cPI + (UtMath::cTWO_PI -  GetOrbitalElements().GetMeanAnomaly()) + aOrbitNum * UtMath::cTWO_PI)
         timeToApoapsisPassage = (UtMath::cPI + (aOrbitNum + 1) * UtMath::cTWO_PI - GetOrbitalElements().GetMeanAnomaly()) /
                                 GetOrbitalElements().GetMeanMotion();
      }
   }
   else
   {
      timeToApoapsisPassage = aOrbitNum * UtMath::cTWO_PI / GetOrbitalElements().GetMeanMotion();
   }
   return timeToApoapsisPassage;
}

// override
double UtOrbitalPropagator::GetTimeToNorthernIntersection(double aTargetRAAN, double aTargetInclination, unsigned aOrbitNum) const
{
   const UtOrbitalElements& oe = GetOrbitalElements();
   double                   meanAnomalyOfIntersection1;
   double                   meanAnomalyOfIntersection2;
   double                   timeToIntersect = 0.0;
   if (GetMeanAnomaliesOfIntersection(oe, aTargetRAAN, aTargetInclination, meanAnomalyOfIntersection1, meanAnomalyOfIntersection2))
   {
      double meanAnomalyDiff = meanAnomalyOfIntersection1 - oe.GetMeanAnomaly();
      if (meanAnomalyDiff < 0.0)
      {
         meanAnomalyDiff = UtMath::cTWO_PI + meanAnomalyDiff;
      }
      timeToIntersect = (meanAnomalyDiff + UtMath::cTWO_PI * aOrbitNum) / oe.GetMeanMotion();
   }
   return timeToIntersect;
}

// override
double UtOrbitalPropagator::GetTimeToSouthernIntersection(double aTargetRAAN, double aTargetInclination, unsigned aOrbitNum) const
{
   const UtOrbitalElements& oe = GetOrbitalElements();
   double                   meanAnomalyOfIntersection1;
   double                   meanAnomalyOfIntersection2;
   double                   timeToIntersect = 0.0;
   if (GetMeanAnomaliesOfIntersection(oe, aTargetRAAN, aTargetInclination, meanAnomalyOfIntersection1, meanAnomalyOfIntersection2))
   {
      double meanAnomalyDiff = meanAnomalyOfIntersection2 - oe.GetMeanAnomaly();
      if (meanAnomalyDiff < 0.0)
      {
         meanAnomalyDiff = UtMath::cTWO_PI + meanAnomalyDiff;
      }
      timeToIntersect = (meanAnomalyDiff + UtMath::cTWO_PI * aOrbitNum) / oe.GetMeanMotion();
   }
   return timeToIntersect;
}

// static
//! Get the two mean anomalies coinciding with orbital intersection for RAAN or combined RAAN-inclination changes.
//!@param aInitialOE The initial set of orbital elements describing the orbit before maneuvering.
//!@param aFinalRAAN The final (post-maneuver) RAAN.
//!@param aFinalInclination The final (post-maneuver) inclination.
//!@param aMA1 The first mean anomaly of the initial orbit at which the maneuver can occur (has northern latitude).
//!@param aMA2 The second mean anomaly of the initial orbit at which the maneuver can occur (has southern latitude).
bool UtOrbitalPropagator::GetMeanAnomaliesOfIntersection(const UtOrbitalElements& aInitialOE,
                                                         double                   aFinalRAAN,
                                                         double                   aFinalInclination,
                                                         double&                  aMA1, // The northern MA
                                                         double&                  aMA2)                  // The southern MA
{
   double cosii          = cos(aInitialOE.GetInclination());
   double sinii          = sin(aInitialOE.GetInclination());
   double cosif          = cos(aFinalInclination);
   double sinif          = sin(aFinalInclination);
   double dRAAN          = UtMath::NormalizeAngleMinusPi_Pi(aFinalRAAN - aInitialOE.GetRAAN());
   double dInc           = aFinalInclination - aInitialOE.GetInclination();
   bool   solutionExists = ((dRAAN != 0.0) || (dInc != 0.0));

   if (solutionExists)
   {
      double cosdRAAN     = cos(dRAAN);
      double cosBA        = cosii * cosif + sinii * sinif * cosdRAAN;
      double argLatitude1 = UtMath::cPI_OVER_2;
      double argLatitude2 = -UtMath::cPI_OVER_2;

      constexpr double sTOLERANCE = 1.0e-14;
      if (!UtMath::NearlyEqual(fabs(cosBA), 1.0, sTOLERANCE))
      {
         // Compute the "Burn angle".
         double BA    = acos(UtMath::Limit(cosBA, 1.0));
         double sinBA = sin(BA);

         // Compute the argument(s) of latitude (in the range 0-2pi) corresponding with the intersection anomalies.
         // Avoid divide by zero in the case of 90 degree inclination:
         static constexpr double sDIVIDE_TOLERANCE = 1.0e-14;
         if (UtMath::NearlyEqual(fabs(cosdRAAN), 1.0, sDIVIDE_TOLERANCE))
         {
            // RAAN changes by 180 degrees.  In this case there is only one degenerate solution at the equator; return
            // it for both results
            argLatitude1 = 0.0;
            argLatitude2 = UtMath::cPI;
         }
         else if (fabs(cosii) > sDIVIDE_TOLERANCE)
         {
            // The acos introduces cosine ambiguity.  The computed argument of latitude is always
            // positive.  To distinguish solutions, note that when dRAAN is negative, the two solutions
            // should be swapped (symmetrical solutions).
            double cosui = UtMath::NormalizeSinCosMinusOne_One((sinif * cosdRAAN - cosBA * sinii) / sinBA / cosii);
            argLatitude1 = acos(UtMath::Limit(cosui, 1.0));
            if (dRAAN > 0.0)
            {
               argLatitude2 = argLatitude1 + UtMath::cPI;
            }
            else
            {
               argLatitude1 = UtMath::cTWO_PI - argLatitude1;
               argLatitude2 = argLatitude1 - UtMath::cPI;
            }
         }
         else
         {
            // Special case of 90 degree inclination angle.
            // Using cotangent form with initial angle 90 degrees (Vallado, p.345).
            // In this case we can use atan2, which does not produce an ambiguity.
            argLatitude1 = atan2(-sin(dRAAN) * sinif, cosif);
            if (argLatitude1 == UtMath::cPI)
            {
               argLatitude1 = 0.0;
               argLatitude2 = UtMath::cPI;
            }
            else if (argLatitude1 < 0.0)
            {
               argLatitude2 = UtMath::cTWO_PI + argLatitude1;
               argLatitude1 += UtMath::cPI;
            }
            else
            {
               argLatitude2 = argLatitude1 + UtMath::cPI;
            }
         }
      }

      double trueAnomalyOfIntersection1 =
         UtMath::NormalizeAngle0_TwoPi(argLatitude1 - aInitialOE.GetArgumentOfPeriapsis());
      double trueAnomalyOfIntersection2 =
         UtMath::NormalizeAngle0_TwoPi(argLatitude2 - aInitialOE.GetArgumentOfPeriapsis());

      // Sort the anomalies such that the first one is in the northern hemisphere (therefore, the second is in the
      // southern hemisphere). This is accomplished by examining the computed arguments of latitude (argument of
      // latitude > 0 is in the northern hemisphere).
      if (argLatitude1 < UtMath::cPI)
      {
         aMA1 = UtOrbitalElements::ComputeMeanAnomaly(trueAnomalyOfIntersection1, aInitialOE.GetEccentricity());
         aMA2 = UtOrbitalElements::ComputeMeanAnomaly(trueAnomalyOfIntersection2, aInitialOE.GetEccentricity());
      }
      else
      {
         aMA2 = UtOrbitalElements::ComputeMeanAnomaly(trueAnomalyOfIntersection1, aInitialOE.GetEccentricity());
         aMA1 = UtOrbitalElements::ComputeMeanAnomaly(trueAnomalyOfIntersection2, aInitialOE.GetEccentricity());
      }
   }
   return solutionExists;
}

UtLambertProblem::Result UtOrbitalPropagator::SolveLambertProblem(const ut::CentralBody& aCentralBody,
                                                                  const UtVec3d&         aLocationInertial_1,
                                                                  const UtVec3d&         aLocationInertial_2,
                                                                  double                 aDt,
                                                                  bool                   aShortWay,
                                                                  bool                   aAllowHitsEarth,
                                                                  double                 aConvergenceTolerance) const
{
   return UtLambertProblem::Universal(aLocationInertial_1,
                                      aLocationInertial_2,
                                      aDt,
                                      aCentralBody.GetEllipsoid(),
                                      aShortWay,
                                      aConvergenceTolerance);
}

void UtOrbitalPropagator::UpdateOrbitalState()
{
   SetOrbitalState(*mPropagatedOrbitalStatePtr);
}
