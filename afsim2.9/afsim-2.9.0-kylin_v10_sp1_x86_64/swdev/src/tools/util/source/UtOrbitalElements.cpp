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

#include "UtOrbitalElements.hpp"

#include <cassert>
#include <iostream>

#include "UtEarth.hpp"
#include "UtEntity.hpp"
#include "UtEulerAngles.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSun.hpp"
#include "UtVec3.hpp"

namespace
{
constexpr double cCIRCULAR_ECCENTRICITY_LIMIT   = 3.0e-7; // eccentricity less than 3.0e-7.
constexpr double cEQUATORIAL_NODAL_VECTOR_LIMIT = 2.0e-2; // nodal vector magnitude is less than 2.0e-2.
} // namespace

//! Constructor specifying only the equinox and gravitational parameter.
//! The orbital elements and epoch must be set separately.
UtOrbitalElements::UtOrbitalElements(const ut::CentralBody& aCentralBody)
   : mCentralBodyPtr(ut::clone(aCentralBody))
{
}

//! Constructor specifying equinox, gravitational parameter, and orbital elements.
//! Epoch must be specified separately.
UtOrbitalElements::UtOrbitalElements(const ut::CentralBody& aCentralBody,
                                     double                 aSemiMajorAxis,
                                     double                 aEccentricity,
                                     double                 aMeanAnomaly,
                                     double                 aInclination,
                                     double                 aRAAN, // Right Ascension of Ascending node.
                                     double                 aArgumentOfPeriapsis)
   : mCentralBodyPtr(ut::clone(aCentralBody))
   , mSemiMajorAxis(aSemiMajorAxis)
   , mEccentricity(aEccentricity)
   , mInclination(aInclination)
   , mRAAN(aRAAN)
   , mArgumentOfPeriapsis(aArgumentOfPeriapsis)
   , mDescriptor()
   , mMeanAnomaly(aMeanAnomaly)
   , mEpoch()
   , mEpochSet(false)
   , mSemiMajorAxisValid(true)
   , mEccentricityValid(true)
   , mMeanAnomalyValid(true)
   , mOrbitSizeElementsPair{OrbitSizeSpecification::cSEMI_MAJOR_AXIS, OrbitSizeSpecification::cECCENTRICITY}
   , mOrbitSizeElements(mOrbitSizeElementsPair.first | mOrbitSizeElementsPair.second)
{
}

//! Constructor specifying equinox, gravitational parameter, orbital elements, and epoch.
//! Nothing else needs to be specified.
UtOrbitalElements::UtOrbitalElements(const UtCalendar&      aEpoch,
                                     const ut::CentralBody& aCentralBody,
                                     double                 aSemiMajorAxis,
                                     double                 aEccentricity,
                                     double                 aMeanAnomaly,
                                     double                 aInclination,
                                     double                 aRAAN, // Right Ascension of Ascending node.
                                     double                 aArgumentOfPeriapsis)
   : mCentralBodyPtr(ut::clone(aCentralBody))
   , mSemiMajorAxis(aSemiMajorAxis)
   , mEccentricity(aEccentricity)
   , mInclination(aInclination)
   , mRAAN(aRAAN)
   , mArgumentOfPeriapsis(aArgumentOfPeriapsis)
   , mDescriptor()
   , mMeanAnomaly(aMeanAnomaly)
   , mEpoch(aEpoch)
   , mEpochSet(true)
   , mSemiMajorAxisValid(true)
   , mEccentricityValid(true)
   , mMeanAnomalyValid(true)
   , mOrbitSizeElementsPair{OrbitSizeSpecification::cSEMI_MAJOR_AXIS, OrbitSizeSpecification::cECCENTRICITY}
   , mOrbitSizeElements(mOrbitSizeElementsPair.first | mOrbitSizeElementsPair.second)
{
}

UtOrbitalElements& UtOrbitalElements::operator=(const UtOrbitalElements& aRhs)
{
   if (this != &aRhs)
   {
      mCentralBodyPtr                    = aRhs.mCentralBodyPtr;
      mSemiMajorAxis                     = aRhs.mSemiMajorAxis;
      mMeanMotion                        = aRhs.mMeanMotion;
      mPeriapsisRadius                   = aRhs.mPeriapsisRadius;
      mApoapsisRadius                    = aRhs.mApoapsisRadius;
      mPeriapsisAltitude                 = aRhs.mPeriapsisAltitude;
      mApoapsisAltitude                  = aRhs.mApoapsisAltitude;
      mEccentricity                      = aRhs.mEccentricity;
      mMeanAnomaly                       = aRhs.mMeanAnomaly;
      mTrueAnomaly                       = aRhs.mTrueAnomaly;
      mInclination                       = aRhs.mInclination;
      mRAAN                              = aRhs.mRAAN;
      mArgumentOfPeriapsis               = aRhs.mArgumentOfPeriapsis;
      mDescriptor                        = aRhs.mDescriptor;
      mNodalVectorMagnitude              = aRhs.mNodalVectorMagnitude;
      mAngularMomentum                   = aRhs.mAngularMomentum;
      mEpoch                             = aRhs.mEpoch;
      mEpochSet                          = aRhs.mEpochSet;
      mAreAveraged                       = aRhs.mAreAveraged;
      mMeanAnomalyValid                  = aRhs.mMeanAnomalyValid;
      mTrueAnomalyValid                  = aRhs.mTrueAnomalyValid;
      mSemiMajorAxisValid                = aRhs.mSemiMajorAxisValid;
      mMeanMotionValid                   = aRhs.mMeanMotionValid;
      mPeriapsisRadiusValid              = aRhs.mPeriapsisRadiusValid;
      mPeriapsisAltitudeValid            = aRhs.mPeriapsisAltitudeValid;
      mApoapsisRadiusValid               = aRhs.mApoapsisRadiusValid;
      mApoapsisAltitudeValid             = aRhs.mApoapsisAltitudeValid;
      mNodalVectorMagnitudeValid         = aRhs.mNodalVectorMagnitudeValid;
      mPerifocalToInertialTransformValid = aRhs.mPerifocalToInertialTransformValid;
      mOrbitSizeElementsPair             = aRhs.mOrbitSizeElementsPair;
      mOrbitSizeElements                 = aRhs.mOrbitSizeElements;
   }
   return *this;
}

//! Process common commands using a UtInput object.
//! @param aInput The input object from which the commands are extracted.
bool UtOrbitalElements::ProcessInput(UtInput& aInput)
{
   std::string command   = aInput.GetCommand();
   bool        myCommand = true;

   if (command == "descriptor")
   {
      aInput.ReadValueQuoted(mDescriptor);
   }
   else if (command == "designator")
   {
      aInput.ReadValueQuoted(mDesignator);
   }
   else if (command == "eccentricity")
   {
      double ecc;
      aInput.ReadValue(ecc);
      aInput.ValueGreaterOrEqual(ecc, 0.0);
      SetEccentricity(ecc);
   }
   else if (command == "semi_major_axis")
   {
      double sma;
      aInput.ReadValueOfType(sma, UtInput::cLENGTH);
      aInput.ValueGreater(sma, 0.0);
      SetSemiMajorAxis(sma);
   }
   else if (command == "mean_motion")
   {
      double meanMotion;
      aInput.ReadValueOfType(meanMotion, UtInput::cANGULAR_RATE);
      aInput.ValueGreater(meanMotion, 0.0);
      SetMeanMotion(meanMotion);
   }
   else if ((command == "raan") || (command == "right_ascension_of_ascending_node"))
   {
      aInput.ReadValueOfType(mRAAN, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(mRAAN, 0.0);
      aInput.ValueLess(mRAAN, UtMath::cTWO_PI);
   }
   else if (command == "inclination")
   {
      aInput.ReadValueOfType(mInclination, UtInput::cANGLE);
      aInput.ValueInClosedRange(mInclination, 0.0, UtMath::cPI);
   }
   else if ((command == "mean_anomaly") || (command == "anomaly"))
   {
      double meanAnomaly = 0.0;
      aInput.ReadValueOfType(meanAnomaly, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(meanAnomaly, 0.0);
      aInput.ValueLess(meanAnomaly, UtMath::cTWO_PI);
      SetMeanAnomaly(meanAnomaly);
   }
   else if (command == "true_anomaly")
   {
      double trueAnomaly = 0.0;
      aInput.ReadValueOfType(trueAnomaly, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(trueAnomaly, 0.0);
      aInput.ValueLess(trueAnomaly, UtMath::cTWO_PI);
      SetTrueAnomaly(trueAnomaly);
   }
   else if (command == "argument_of_periapsis")
   {
      aInput.ReadValueOfType(mArgumentOfPeriapsis, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(mArgumentOfPeriapsis, 0.0);
      aInput.ValueLess(mArgumentOfPeriapsis, UtMath::cTWO_PI);
   }
   else if ((command == "revs_per_day") ||      // deprecated form
            (command == "revolutions_per_day")) // in revolutions, not radians.
   {
      double meanMotionRevsPerDay;
      aInput.ReadValue(meanMotionRevsPerDay);
      aInput.ValueGreater(meanMotionRevsPerDay, 0.0);
      SetMeanMotion(meanMotionRevsPerDay / UtCalendar::cSEC_IN_DAY * UtMath::cTWO_PI);
   }
   else if (command == "periapsis_radius")
   {
      double radiusOfPeriapsis;
      aInput.ReadValueOfType(radiusOfPeriapsis, UtInput::cLENGTH);
      aInput.ValueGreater(radiusOfPeriapsis, 0.0);
      SetPeriapsisRadius(radiusOfPeriapsis);
   }
   else if (command == "apoapsis_radius")
   {
      double radiusOfApoapsis;
      aInput.ReadValueOfType(radiusOfApoapsis, UtInput::cLENGTH);
      aInput.ValueGreater(radiusOfApoapsis, 0.0);
      SetApoapsisRadius(radiusOfApoapsis);
   }
   else if (command == "periapsis_altitude")
   {
      double altitudeOfPeriapsis;
      aInput.ReadValueOfType(altitudeOfPeriapsis, UtInput::cLENGTH);
      aInput.ValueGreater(altitudeOfPeriapsis, 0.0);
      SetPeriapsisAltitude(altitudeOfPeriapsis);
   }
   else if (command == "apoapsis_altitude")
   {
      double altitudeOfApoapsis;
      aInput.ReadValueOfType(altitudeOfApoapsis, UtInput::cLENGTH);
      aInput.ValueGreater(altitudeOfApoapsis, 0.0);
      SetApoapsisAltitude(altitudeOfApoapsis);
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// virtual
//! Is there sufficient, valid data to utilize the orbital elements?
bool UtOrbitalElements::IsValid() const
{
   //! If the orbit size (SMA) cannot be determined, there is insufficient data.
   return (mSemiMajorAxisValid || mMeanMotionValid || mPeriapsisRadiusValid || mApoapsisRadiusValid ||
           mPeriapsisAltitudeValid || mApoapsisAltitudeValid);
}

bool UtOrbitalElements::Initialize(const UtCalendar& aEpoch)
{
   if (!mEpochSet)
   {
      mEpoch    = aEpoch;
      mEpochSet = true;
   }
   return mCentralBodyPtr->Initialize(aEpoch) && Initialize();
}

bool UtOrbitalElements::Initialize()
{
   // Check to make sure the orbital elements are not under-constrained.
   mOrbitSizeElements = mOrbitSizeElementsPair.first | mOrbitSizeElementsPair.second;

   // Not enough information to determine orbit size
   if (mOrbitSizeElements < (OrbitSizeSpecification::cSEMI_MAJOR_AXIS))
   {
      // Cannot determine orbit size.
      { // RAII block
         auto out = ut::log::error() << "Cannot determine orbit size. At least one of the following must be specified:";
         out.AddNote() << "revs_per_day";
         out.AddNote() << "revolutions_per_day";
         out.AddNote() << "semi_major_axis";
         out.AddNote() << "periapsis_radius";
         out.AddNote() << "apoapsis_radius";
         out.AddNote() << "periapsis_altitude";
         out.AddNote() << "apoapsis_altitude";
      }
      return false;
   }

   if (!mMeanAnomalyValid && !mTrueAnomalyValid)
   {
      // Mean/True Anomaly not defined... use the defaults (0).
      mTrueAnomalyValid = true;
   }

   return ValidateOrbitalElements();
}

// virtual
//! Explicitly set the individual orbital elements.
void UtOrbitalElements::Set(double aSemiMajorAxis,
                            double aEccentricity,
                            double aStartMeanAnomaly,
                            double aInclination,
                            double aRAAN, // Right Ascension of Ascending node.
                            double aArgumentOfPeriapsis)
{
   SetSemiMajorAxis(aSemiMajorAxis);
   SetEccentricity(aEccentricity);
   SetMeanAnomaly(aStartMeanAnomaly);
   SetInclination(aInclination);
   SetRAAN(aRAAN);
   SetArgumentOfPeriapsis(aArgumentOfPeriapsis);
   mPerifocalToInertialTransformValid = false;
   mNodalVectorMagnitudeValid         = false;
   mMeanAnomalyValid                  = true;
   mTrueAnomalyValid                  = false;
}

// virtual
//! Set the orbital elements such that the resulting orbit is a sun-synchronous
//! orbit at the given altitude and apparent solar hour at the descending node
void UtOrbitalElements::SetSunSynchronous(const UtCalendar& aEpoch, double aSolarHour, double aCircularAltitude, double aJ2)
{
   double meanRadius_m = mCentralBodyPtr->GetMeanRadius();
   double gravParam    = mCentralBodyPtr->GetGravitationalParameter();
   double sma_m        = meanRadius_m + aCircularAltitude;

   double gravParam_km3_day2 = 1.0e-9 * 86400.0 * 86400.0 * gravParam;
   double meanRadius_km      = meanRadius_m * 1.0e-3;
   double sma_km             = sma_m * 1.0e-3;
   double ra, dec;
   UtSun::GetSunLocationRA_Dec(aEpoch, ra, dec);

   // Derived from "Space Mission Engineering: The New SMAD", pages 222-223, 965
   double Kj2  = -1.5 * aJ2 * std::sqrt(gravParam_km3_day2) * meanRadius_km * meanRadius_km;
   double K    = UtMath::cTWO_PI / (365.25636 * Kj2);
   double incl = std::acos(K * std::pow(sma_km, 7.0 / 2.0));
   double raan =
      UtMath::NormalizeAngle0_TwoPi(ra + UtSun::GetApparentSunExcess(aEpoch) + (UtMath::cTWO_PI / 24.0) * aSolarHour);

   SetEpoch(aEpoch);
   SetSemiMajorAxis(sma_m);
   SetEccentricity(0.0);
   SetTrueAnomaly(0.0);
   SetInclination(incl);
   SetRAAN(raan);
   SetArgumentOfPeriapsis(0.0);
   mPerifocalToInertialTransformValid = false;
   mNodalVectorMagnitudeValid         = false;
   mMeanAnomalyValid                  = false;
   mTrueAnomalyValid                  = true;
}

/** Publish a TLE given an updated inertial or quasi-inertial position and velocity.

  @param aPosition The current position in inertial or quasi-inertial coordinates.
  @param aVelocity The current velocity in inertial or quasi-inertial coordinates.
  @param aEpoch  The epoch of the orbit, expressed in either YYDDD.F or YYYYDDD.F format.
*/
void UtOrbitalElements::Publish(const double aPosition[3], const double aVelocity[3], const UtCalendar& aEpoch)
{
   mEpoch    = aEpoch;
   mEpochSet = true;

   // Based on astromechanics notes, ch. 9 (see document for equations):
   //  1. Determine magnitudes of position and velocity vectors:
   double r = UtVec3d::Magnitude(aPosition);
   double v = UtVec3d::Magnitude(aVelocity);

   // 2. Determine the energy equation and semi-major axis (see note about parabolic orbits)
   double u  = GetGravitationalParameter(); // Greek "mu"
   double En = v * v / 2.0 - u / r;         // Energy
   double a  = -u / (2.0 * En);
   SetSemiMajorAxis(a);

   // 3. Compute the angular momentum vector and its magnitude:
   double h[3];
   UtVec3d::CrossProduct(h, aPosition, aVelocity);
   double hMag = UtVec3d::Magnitude(h);

   // 4. Compute the eccentricity vector and its magnitude:
   double e[3];
   double firstTerm[3];
   UtVec3d::Set(firstTerm, aPosition);
   UtVec3d::Multiply(firstTerm, (v * v - u / r));
   double secondTerm[3];
   UtVec3d::Set(secondTerm, aVelocity);
   UtVec3d::Multiply(secondTerm, (UtVec3d::DotProduct(aPosition, aVelocity)));
   UtVec3d::Subtract(e, firstTerm, secondTerm);
   UtVec3d::Divide(e, u);
   SetEccentricity(UtVec3d::Magnitude(e));

   // 5. Compute the inclination:
   double cosi  = h[2] / hMag;
   mInclination = acos(UtMath::NormalizeSinCosMinusOne_One(cosi));

   // 6. Compute nodal vector and its magnitude:
   double n[3]                = {-h[1], h[0], 0.0};
   mNodalVectorMagnitude      = UtVec3d::Magnitude(n);
   mNodalVectorMagnitudeValid = true;

   // 7. Compute longitude of ascending node (RAAN; inclined orbits)
   bool circular   = OrbitIsCircular();
   bool equatorial = OrbitIsEquatorial();

   if (!equatorial) // General cases handled first as they are more common.
   {
      mRAAN = acos(n[0] / mNodalVectorMagnitude);
      if (n[1] < 0.0)
      {
         mRAAN = UtMath::NormalizeAngle0_TwoPi(UtMath::cTWO_PI - mRAAN);
      }
   }
   else // equatorial
   {
      mRAAN = 0.0;
   }

   // 8. Compute argument of periapsis
   if ((!equatorial) && (!circular))
   {
      double cosw          = UtVec3d::DotProduct(n, e) / (mNodalVectorMagnitude * mEccentricity);
      cosw                 = UtMath::NormalizeSinCosMinusOne_One(cosw);
      mArgumentOfPeriapsis = acos(cosw);
      if (e[2] <= 0.0)
      {
         mArgumentOfPeriapsis = UtMath::NormalizeAngle0_TwoPi(UtMath::cTWO_PI - mArgumentOfPeriapsis);
      }
   }
   else if (!circular) // && equatorial
   {
      double cosw          = e[0] / mEccentricity;
      cosw                 = UtMath::NormalizeSinCosMinusOne_One(cosw);
      mArgumentOfPeriapsis = acos(cosw);
      if (mInclination < cEQUATORIAL_NODAL_VECTOR_LIMIT) // equatorial, prograde
      {
         if (e[1] < 0.0)
         {
            mArgumentOfPeriapsis = UtMath::NormalizeAngle0_TwoPi(UtMath::cTWO_PI - mArgumentOfPeriapsis);
         }
      }
      else // equatorial, retrograde (inclination ~180 degrees)
      {
         if (e[1] > 0.0)
         {
            mArgumentOfPeriapsis = UtMath::NormalizeAngle0_TwoPi(UtMath::cTWO_PI - mArgumentOfPeriapsis);
         }
      }
   }
   else // circular, equatorial
   {
      mArgumentOfPeriapsis = 0.0;
   }

   // 9. Determine the true anomaly
   if (!circular)
   {
      double rDotV = UtVec3d::DotProduct(aPosition, aVelocity);

      // Compute the true anomaly.
      double ta = acos(UtMath::NormalizeSinCosMinusOne_One(UtVec3d::DotProduct(e, aPosition) / mEccentricity / r));
      if (rDotV < 0.0)
      {
         ta = UtMath::cTWO_PI - ta;
      }

      SetTrueAnomaly(ta);
   }
   else // circular
   {
      if (!equatorial) // Circular, non-equatorial; Compute the argument of latitude:
      {
         double argLat =
            acos(UtMath::NormalizeSinCosMinusOne_One(UtVec3d::DotProduct(aPosition, n) / r / mNodalVectorMagnitude));
         if (aPosition[2] < 0.0)
         {
            argLat = UtMath::cTWO_PI - argLat;
         }
         SetMeanAnomaly(argLat);
      }
      else // Circular, equatorial; Compute the true longitude:
      {
         double trueLon = acos(UtMath::NormalizeSinCosMinusOne_One(aPosition[0] / r));
         if (aPosition[1] < 0.0)
         {
            trueLon = UtMath::cTWO_PI - trueLon;
         }
         SetMeanAnomaly(trueLon);
      }
   }

   // We likely just changed at least one of inclination, RAAN, and arg. periapsis.
   mPerifocalToInertialTransformValid = false;

   // This is an instantaneous, not averaged, representation of the orbital elements.
   mAreAveraged = false;
}

/** Publish a TLE given an updated inertial or quasi-inertial position and velocity.

  @param aPosition The current position in inertial or quasi-inertial coordinates.
  @param aVelocity The current velocity in inertial or quasi-inertial coordinates.
  @param aEpoch  A UtCalendar object that defines the epoch of the orbit.
*/
void UtOrbitalElements::Publish(const UtVec3d& aPosition, const UtVec3d& aVelocity, const UtCalendar& aEpoch)
{
   Publish(aPosition.GetData(), aVelocity.GetData(), aEpoch);
}

//! Given a point on the orbit ellipse (true anomaly) at a given time,
//! compute the Earth Centered Inertial (ECI) location and velocity.
//! @note Care must be taken to ensure that the Perifocal transform is accurate.
void UtOrbitalElements::ComputeState(UtVec3d& aPosition, UtVec3d& aVelocity) const
{
   // The following call to GetTrueAnomaly does the actual propagation.
   double trueAnomaly    = GetTrueAnomaly();
   double sinTrueAnomaly = sin(trueAnomaly);
   double cosTrueAnomaly = cos(trueAnomaly);

   // Compute the radius from the central body (focus) to satellite.
   // (mSMA_Times_1_Minus_e_Squared is the semi-major axis times 1 - the eccentricity squared.)
   double radius =
      GetSemiMajorAxis() * (1 - GetEccentricity() * GetEccentricity()) / (1 + GetEccentricity() * cosTrueAnomaly);

   // Compute the position on the ellipse using the computed radius and true anomaly.
   UtVec3d r(radius * cosTrueAnomaly, radius * sinTrueAnomaly, 0.0);

   const UtMat3d& rotMatrix = GetPerifocalToInertialTransform();

   // Rotate using classical Euler angles to produce the correct 3-D vector.
   rotMatrix.TransposeMultiply(aPosition, r);

   // velocity from Vis-Viva equation:
   double speed           = sqrt(GetGravitationalParameter() * (2.0 / radius - 1.0 / GetSemiMajorAxis()));
   double periapsisRadius = GetSemiMajorAxis() * (1 - GetEccentricity());

   mAngularMomentum =
      periapsisRadius * sqrt(GetGravitationalParameter() * (2.0 / periapsisRadius - 1.0 / GetSemiMajorAxis()));

   double sinTheta = mAngularMomentum / radius / speed;
   double theta    = asin(UtMath::NormalizeSinCosMinusOne_One(sinTheta));
   if (GetTrueAnomaly() > UtMath::cPI)
   {
      theta = UtMath::cPI - theta;
   }

   // Compute angle from periapsis for velocity, called gamma.
   double gamma    = theta + GetTrueAnomaly();
   double sinGamma = sin(gamma);
   double cosGamma = cos(gamma);

   // Similar to location calculation, find the vector referenced to the ellipse,
   // then rotate by the classical Euler angles.
   UtVec3d v(speed * cosGamma, speed * sinGamma, 0.0);
   rotMatrix.TransposeMultiply(aVelocity, v);
}

UtOrbitalElements UtOrbitalElements::FastForward(const double aSeconds) const
{
   UtOrbitalElements futureElements(*this);
   futureElements.SetMeanAnomaly(GetMeanAnomaly() + aSeconds * GetMeanMotion());
   return futureElements;
}

//! An orbit is defined to be circular if the eccentricity is within some tolerance of 0.
bool UtOrbitalElements::OrbitIsCircular() const
{
   return (GetEccentricity() < cCIRCULAR_ECCENTRICITY_LIMIT);
}

//! An orbit is defined to be prograde if the inclination is equal or below pi/2.
bool UtOrbitalElements::OrbitIsPrograde() const
{
   return (GetInclination() <= UtMath::cPI_OVER_2);
}

//! An orbit is defined to be equatorial if the magnitude of the nodal vector is within
//! a given tolerance; or, if the nodal vector magnitude is not valid, the orbit is considered equatorial only if the
//! inclination is exactly 0 or pi (to double precision).
//!@note The magnitude of the nodal vector is used as the discriminator (when available), because it is more sensitive
//! than
//! the computed inclination.  Alternatively, inclination could be computed with long double precision and compared
//! directly, but this option is not currently available for all supported compilers.
bool UtOrbitalElements::OrbitIsEquatorial() const
{
   if (mNodalVectorMagnitudeValid)
   {
      return (mNodalVectorMagnitude < cEQUATORIAL_NODAL_VECTOR_LIMIT);
   }
   else
   {
      return ((GetInclination() == 0.0) || (GetInclination() == UtMath::cPI));
   }
}

//! Return whether the orbit described by the orbital elements is hyperbolic (or is the limiting parabolic case).
bool UtOrbitalElements::OrbitIsHyperbolic() const
{
   return (mEccentricity >= 1.0);
}

//! Set the desired epoch using a UtCalendar object.
//! @param aEpoch The desired start epoch, as a UtCalendar object.
void UtOrbitalElements::SetEpoch(const UtCalendar& aEpoch)
{
   mEpoch    = aEpoch;
   mEpochSet = true;
}

//! Set the desired epoch.
//! @param aEpoch The desired start epoch
void UtOrbitalElements::SetEpoch(double aEpoch)
{
   mEpoch.SetEpoch(aEpoch);
   mEpochSet = true;
}

//! Set the epoch to the given date and time.
//! @param aYear The year portion of the date.
//! @param aMonth The month portion of the date.
//! @param aDay The day portion of the date.
//! @param aHour The hour portion of the time.
//! @param aMinute The minute portion of the time.
//! @param aSecond The second portion of the time.
void UtOrbitalElements::SetEpoch(int aYear, int aMonth, int aDay, int aHour, int aMinute, double aSecond)
{
   mEpoch.SetDate(aYear, aMonth, aDay);
   mEpoch.SetTime(aHour, aMinute, aSecond);
   mEpochSet = true;
}

//! In some cases, such as were we set individual orbital elements, the epoch will not be set.
//! We want to detect this condition in order that we can set it to the beginning epoch of the simulation.
//! @return Whether an epoch has been set.
bool UtOrbitalElements::HasEpoch() const
{
   return mEpochSet;
}

//! Set the central body, which becomes necessary if it changes during the course of a simulation.
void UtOrbitalElements::SetCentralBody(const ut::CentralBody& aCentralBody)
{
   mCentralBodyPtr = ut::clone(aCentralBody);
}

// static
//! Get the true anomaly, given the mean anomaly.
//! @note This method is static in order to be used as a standalone utility function.
double UtOrbitalElements::ComputeTrueAnomaly(double aMeanAnomaly,
                                             double aEccentricity,
                                             double aTolerance,     //= 1.0e-8,
                                             double aMaxIterations) //= 1000
{
   // This initial guess for the eccentric anomaly is necessary for convergence of the second algorithm below
   // in the case of high eccentricity and small mean anomaly. The guess is more precise as the eccentricity increases.
   double        normMeanAnomaly = UtMath::NormalizeAngle0_TwoPi(aMeanAnomaly);
   double        ea              = normMeanAnomaly;
   const double& ma              = normMeanAnomaly;
   const double& e               = aEccentricity;
   if (aEccentricity < 0.5)
   {
      ea += e * sin(ma);
   }
   else if (aEccentricity < 0.9)
   {
      ea += e * sin(ma) + e * e / 2.0 * sin(2.0 * ma);
   }
   else
   {
      double e2 = e * e;
      double e3 = e2 * e;
      ea += (e - e3 / 8.0) * sin(ma) + e2 / 2.0 * sin(2.0 * ma) + 3.0 * e3 / 8.0 * sin(3.0 * ma);
   }

   unsigned iteration  = 1;
   double   difference = 0.0;

   // Calculation of the eccentric anomaly is iterative,
   // but typically converges within a few cycles.
   while (iteration < aMaxIterations)
   {
      double oldEA = ea;

      // If the eccentricity is small, use the simpler form for iterating to the solution.
      if (aEccentricity < 0.5)
      {
         ea = normMeanAnomaly + aEccentricity * sin(ea);
      }
      else
      {
         // The following is implemented (Meeus, Astronomical Algorithms 2nd ed. p.199) for faster convergence.
         // Also see Vallado (Fundamentals of Astrodynamics and Applications, 4th ed. p. 65).
         double sinEA = sin(ea);
         double cosEA = cos(ea);
         ea += (normMeanAnomaly + aEccentricity * sinEA - ea) / (1 - aEccentricity * cosEA);
      }
      difference = fabs(ea - oldEA);
      if (difference <= aTolerance)
      {
         break;
      }
      ++iteration;
   }

   if (iteration == aMaxIterations)
   {
      auto out = ut::log::warning() << "UtOrbitalElements: Desired convergence not met.";
      out.AddNote() << "Last difference in computed eccentric anomaly was " << difference << ".";
   }

   double cosE        = cos(ea);
   double trueAnomaly = acos((cosE - aEccentricity) / (1 - aEccentricity * cosE));

   // Recover quadrant:
   if (normMeanAnomaly > UtMath::cPI)
   {
      // Due to the acos above, we are in the range of (pi, 0);
      // We should be in the range of (pi, 2*pi).
      trueAnomaly = UtMath::cTWO_PI - trueAnomaly;
   }

   return trueAnomaly;
}

//! Return the orbital regime for the given elements.
//!
//! This will return the orbital regime (LEO, MEO, GEO or HEO) for this
//! set of orbital elements if it can be decided. If the elements do
//! not fit in the criteria specified, then OrbitalRegime::cNO_LABEL
//! will be returned.
//!
//! @return The orbital regime, if any, to which these elements belong.
UtOrbitalElements::OrbitalRegime UtOrbitalElements::GetOrbitalRegime() const
{
   constexpr double cLEO_LIMIT{8371000.0};  // 2000 km of altitude
   constexpr double cMEO_LIMIT{42057000.0}; // 35686 km of altitude
   constexpr double cGEO_LIMIT{42257000.0}; // 35886 km of altitude

   OrbitalRegime retval{OrbitalRegime::cNO_LABEL};
   if (GetEccentricity() >= 0.4)
   {
      retval = OrbitalRegime::cHEO;
   }
   else if (GetApoapsisRadius() <= cLEO_LIMIT)
   {
      retval = OrbitalRegime::cLEO;
   }
   else if (GetApoapsisRadius() < cMEO_LIMIT && GetPeriapsisRadius() > cLEO_LIMIT)
   {
      retval = OrbitalRegime::cMEO;
   }
   else if (GetApoapsisRadius() < cGEO_LIMIT && GetPeriapsisRadius() >= cMEO_LIMIT &&
            GetInclination() < UtMath::cPI_OVER_2 && GetEccentricity() < 0.001)
   {
      retval = OrbitalRegime::cGEO;
   }
   return retval;
}

// static
//! Get the mean anomaly, given the true anomaly.
//! @note This method is static in order to be used as a standalone utility function.
double UtOrbitalElements::ComputeMeanAnomaly(double aTrueAnomaly, double aEccentricity)
{
   double normTrueAnomaly  = UtMath::NormalizeAngle0_TwoPi(aTrueAnomaly);
   double cosT             = cos(normTrueAnomaly);
   double cosE             = (aEccentricity + cosT) / (1 + aEccentricity * cosT);
   double eccentricAnomaly = acos(cosE);
   if (normTrueAnomaly > UtMath::cPI)
   {
      eccentricAnomaly = UtMath::cTWO_PI - eccentricAnomaly; // Quadrant fix
   }
   double sinE = sin(eccentricAnomaly);
   return (eccentricAnomaly - aEccentricity * sinE);
}

//! Get the mean anomaly, assuming a true anomaly.
double UtOrbitalElements::GetMeanAnomaly() const
{
   if (!mMeanAnomalyValid)
   {
      assert(mTrueAnomalyValid);
      mMeanAnomaly      = ComputeMeanAnomaly(mTrueAnomaly, mEccentricity);
      mMeanAnomalyValid = true;
   }
   return mMeanAnomaly;
}

//! Get the true anomaly, assuming a valid mean anomaly.
//! @param aTolerance The allowed difference between the computed and actual value returned.
//! @param aMaxIterations The maximum number of iterations allowed to attain aTolerance.
double UtOrbitalElements::GetTrueAnomaly(double aTolerance,           //= 1.0e-8,
                                         double aMaxIterations) const //= 1000)
{
   if (!mTrueAnomalyValid)
   {
      assert(mMeanAnomalyValid);
      mTrueAnomaly = ComputeTrueAnomaly(mMeanAnomaly, mEccentricity, aTolerance, aMaxIterations);
   }
   return mTrueAnomaly;
}

void UtOrbitalElements::SetTrueAnomaly(double aTrueAnomaly)
{
   mMeanAnomalyValid = false;
   mTrueAnomalyValid = true;
   mTrueAnomaly      = UtMath::NormalizeAngle0_TwoPi(aTrueAnomaly);
}

void UtOrbitalElements::SetMeanAnomaly(double aMeanAnomaly)
{
   mMeanAnomalyValid = true;
   mTrueAnomalyValid = false;
   mMeanAnomaly      = UtMath::NormalizeAngle0_TwoPi(aMeanAnomaly);
}

// static
double UtOrbitalElements::ComputeSemiMajorAxis(double aMeanMotion, double aGravitationalParameter)
{
   static const double cONE_THIRD = 1.0 / 3.0;
   return pow(aGravitationalParameter / aMeanMotion / aMeanMotion, cONE_THIRD);
}

// static
double UtOrbitalElements::ComputeMeanMotion(double aSemiMajorAxis, double aGravitationalParameter)
{
   return sqrt(aGravitationalParameter / (aSemiMajorAxis * aSemiMajorAxis * aSemiMajorAxis));
}

// static
double UtOrbitalElements::ComputePeriapsisRadius(double aPeriapsisAltitude, double aCentralBodyRadius)
{
   return aPeriapsisAltitude + aCentralBodyRadius;
}

// static
double UtOrbitalElements::ComputePeriapsisAltitude(double aPeriapsisRadius, double aCentralBodyRadius)
{
   return aPeriapsisRadius - aCentralBodyRadius;
}

// static
double UtOrbitalElements::ComputeApoapsisRadius(double aApoapsisAltitude, double aCentralBodyRadius)
{
   return aApoapsisAltitude + aCentralBodyRadius;
}

// static
double UtOrbitalElements::ComputeApoapsisAltitude(double aApoapsisRadius, double aCentralBodyRadius)
{
   return aApoapsisRadius - aCentralBodyRadius;
}

void UtOrbitalElements::SetSemiMajorAxis(double aSemiMajorAxis)
{
   mSemiMajorAxis      = aSemiMajorAxis;
   mSemiMajorAxisValid = true;
   mMeanMotionValid    = false;

   SetOrbitSizeSpecification(OrbitSizeSpecification::cSEMI_MAJOR_AXIS);
}

void UtOrbitalElements::SetMeanMotion(double aMeanMotion)
{
   mMeanMotion         = aMeanMotion;
   mSemiMajorAxisValid = false;
   mMeanMotionValid    = true;

   SetOrbitSizeSpecification(OrbitSizeSpecification::cSEMI_MAJOR_AXIS);
}

void UtOrbitalElements::SetEccentricity(double aEccentricity)
{
   mEccentricity      = aEccentricity;
   mEccentricityValid = true;

   SetOrbitSizeSpecification(OrbitSizeSpecification::cECCENTRICITY);
}

void UtOrbitalElements::SetPeriapsisRadius(double aPeriapsisRadius)
{
   mPeriapsisRadius        = aPeriapsisRadius;
   mPeriapsisAltitudeValid = false;
   mPeriapsisRadiusValid   = true;

   SetOrbitSizeSpecification(OrbitSizeSpecification::cPERIAPSIS);
}

void UtOrbitalElements::SetApoapsisRadius(double aApoapsisRadius)
{
   mApoapsisRadius        = aApoapsisRadius;
   mApoapsisAltitudeValid = false;
   mApoapsisRadiusValid   = true;

   SetOrbitSizeSpecification(OrbitSizeSpecification::cAPOAPSIS);
}

void UtOrbitalElements::SetPeriapsisAltitude(double aPeriapsisAltitude)
{
   mPeriapsisAltitude      = aPeriapsisAltitude;
   mPeriapsisRadiusValid   = false;
   mPeriapsisAltitudeValid = true;

   SetOrbitSizeSpecification(OrbitSizeSpecification::cPERIAPSIS);
}

void UtOrbitalElements::SetApoapsisAltitude(double aApoapsisAltitude)
{
   mApoapsisAltitude      = aApoapsisAltitude;
   mApoapsisRadiusValid   = false;
   mApoapsisAltitudeValid = true;

   SetOrbitSizeSpecification(OrbitSizeSpecification::cAPOAPSIS);
}

double UtOrbitalElements::GetSemiMajorAxis() const
{
   if (!mSemiMajorAxisValid)
   {
      if (mMeanMotionValid)
      {
         mSemiMajorAxis = ComputeSemiMajorAxis(mMeanMotion, mCentralBodyPtr->GetGravitationalParameter());
      }
      else if (mOrbitSizeElements == OrbitSizeSpecification::cPERIAPSIS) // circular orbit
      {
         assert(mPeriapsisRadiusValid || mPeriapsisAltitudeValid);
         GetPeriapsisRadius();
         mSemiMajorAxis = mPeriapsisRadius;
      }
      else if (mOrbitSizeElements == OrbitSizeSpecification::cAPOAPSIS) // circular orbit
      {
         assert(mApoapsisRadiusValid || mApoapsisAltitudeValid);
         GetApoapsisRadius();
         mSemiMajorAxis = mApoapsisRadius;
      }
      else if ((mOrbitSizeElements & OrbitSizeSpecification::cECCENTRICITY) != OrbitSizeSpecification::cNULL)
      {
         assert(mEccentricityValid);
         if ((mOrbitSizeElements & OrbitSizeSpecification::cPERIAPSIS) != OrbitSizeSpecification::cNULL)
         {
            assert(mPeriapsisRadiusValid || mPeriapsisAltitudeValid);
            GetPeriapsisRadius();
            mSemiMajorAxis = mPeriapsisRadius / (1.0 - mEccentricity);
         }
         else if ((mOrbitSizeElements & OrbitSizeSpecification::cAPOAPSIS) != OrbitSizeSpecification::cNULL)
         {
            assert(mApoapsisRadiusValid || mApoapsisAltitudeValid);
            GetApoapsisRadius();
            mSemiMajorAxis = mApoapsisRadius / (1.0 + mEccentricity);
         }
      }
      else // periapsis and apoapsis specified
      {
         assert(mPeriapsisRadiusValid || mPeriapsisAltitudeValid);
         assert(mApoapsisRadiusValid || mApoapsisAltitudeValid);
         GetPeriapsisRadius();
         GetApoapsisRadius();
         mSemiMajorAxis = (mPeriapsisRadius + mApoapsisRadius) / 2.0;
      }
      mSemiMajorAxisValid = true;
   }
   return mSemiMajorAxis;
}

double UtOrbitalElements::GetMeanMotion() const
{
   if (!mMeanMotionValid)
   {
      GetSemiMajorAxis();
      mMeanMotion      = ComputeMeanMotion(mSemiMajorAxis, mCentralBodyPtr->GetGravitationalParameter());
      mMeanMotionValid = true;
   }
   return mMeanMotion;
}

double UtOrbitalElements::GetEccentricity() const
{
   if (!mEccentricityValid)
   {
      // Check the case of only one specification
      if ((mOrbitSizeElements == OrbitSizeSpecification::cSEMI_MAJOR_AXIS) ||
          (mOrbitSizeElements == OrbitSizeSpecification::cPERIAPSIS) ||
          (mOrbitSizeElements == OrbitSizeSpecification::cAPOAPSIS))
      {
         mEccentricity = 0.0;
      }
      else if ((mOrbitSizeElements & OrbitSizeSpecification::cSEMI_MAJOR_AXIS) != OrbitSizeSpecification::cNULL)
      {
         assert(mSemiMajorAxisValid || mMeanMotion);
         GetSemiMajorAxis();
         if ((mOrbitSizeElements & OrbitSizeSpecification::cPERIAPSIS) != OrbitSizeSpecification::cNULL)
         {
            assert(mPeriapsisRadiusValid || mPeriapsisAltitude);
            GetPeriapsisRadius();
            mEccentricity = 1.0 - mPeriapsisRadius / mSemiMajorAxis;
         }
         else if ((mOrbitSizeElements & OrbitSizeSpecification::cAPOAPSIS) != OrbitSizeSpecification::cNULL)
         {
            assert(mApoapsisRadiusValid || mApoapsisAltitude);
            GetApoapsisRadius();
            mEccentricity = mApoapsisRadius / mSemiMajorAxis - 1.0;
         }
         else
         {
            mEccentricity = 0.0;
         }
      }
      else // periapsis and apoapsis specified
      {
         assert(mPeriapsisRadiusValid || mPeriapsisAltitudeValid);
         assert(mApoapsisRadiusValid || mApoapsisAltitudeValid);
         GetPeriapsisRadius();
         GetApoapsisRadius();
         mEccentricity = (mApoapsisRadius - mPeriapsisRadius) / (mApoapsisRadius + mPeriapsisRadius);
      }
      mEccentricityValid = true;
   }
   return mEccentricity;
}

//! Return the periapsis distance.
double UtOrbitalElements::GetPeriapsisRadius() const
{
   if (!mPeriapsisRadiusValid)
   {
      if (mPeriapsisAltitudeValid)
      {
         mPeriapsisRadius = ComputePeriapsisRadius(mPeriapsisAltitude, mCentralBodyPtr->GetMeanRadius());
      }
      else if (mOrbitSizeElements == OrbitSizeSpecification::cSEMI_MAJOR_AXIS) // circular orbit
      {
         assert(mSemiMajorAxisValid);
         GetSemiMajorAxis();
         mPeriapsisRadius = mSemiMajorAxis;
      }
      else if (mOrbitSizeElements == OrbitSizeSpecification::cAPOAPSIS) // circular orbit
      {
         assert(mApoapsisRadiusValid || mApoapsisAltitudeValid);
         GetApoapsisRadius();
         mPeriapsisRadius = mApoapsisRadius;
      }
      else if ((mOrbitSizeElements & OrbitSizeSpecification::cECCENTRICITY) != OrbitSizeSpecification::cNULL)
      {
         assert(mEccentricityValid);
         if ((mOrbitSizeElements & OrbitSizeSpecification::cSEMI_MAJOR_AXIS) != OrbitSizeSpecification::cNULL)
         {
            assert(mSemiMajorAxisValid || mMeanMotionValid);
            GetSemiMajorAxis();
            mPeriapsisRadius = mSemiMajorAxis * (1.0 - mEccentricity);
         }
         else if ((mOrbitSizeElements & OrbitSizeSpecification::cAPOAPSIS) != OrbitSizeSpecification::cNULL)
         {
            assert(mApoapsisRadiusValid || mApoapsisAltitudeValid);
            GetApoapsisRadius();
            mPeriapsisRadius = mApoapsisRadius * (1.0 - mEccentricity) / (1.0 + mEccentricity);
         }
      }
      else // semi-major axis and apoapsis
      {
         assert(mSemiMajorAxisValid || mMeanMotionValid);
         assert(mApoapsisRadiusValid || mApoapsisAltitudeValid);
         GetSemiMajorAxis();
         GetApoapsisRadius();
         mPeriapsisRadius = 2.0 * mSemiMajorAxis - mApoapsisRadius;
      }
      mPeriapsisRadiusValid = true;
   }
   return mPeriapsisRadius;
}

//! Return the apoapsis distance.
double UtOrbitalElements::GetApoapsisRadius() const
{
   if (!mApoapsisRadiusValid)
   {
      if (mApoapsisAltitudeValid)
      {
         mApoapsisRadius = ComputeApoapsisRadius(mApoapsisAltitude, mCentralBodyPtr->GetMeanRadius());
      }
      else if (mOrbitSizeElements == OrbitSizeSpecification::cSEMI_MAJOR_AXIS) // circular orbit
      {
         assert(mSemiMajorAxisValid);
         GetSemiMajorAxis();
         mApoapsisRadius = mSemiMajorAxis;
      }
      else if (mOrbitSizeElements == OrbitSizeSpecification::cPERIAPSIS) // circular orbit
      {
         assert(mPeriapsisRadiusValid || mPeriapsisAltitudeValid);
         GetPeriapsisRadius();
         mApoapsisRadius = mPeriapsisRadius;
      }
      else if ((mOrbitSizeElements & OrbitSizeSpecification::cECCENTRICITY) != OrbitSizeSpecification::cNULL)
      {
         assert(mEccentricityValid);
         if ((mOrbitSizeElements & OrbitSizeSpecification::cSEMI_MAJOR_AXIS) != OrbitSizeSpecification::cNULL)
         {
            assert(mSemiMajorAxisValid || mMeanMotionValid);
            GetSemiMajorAxis();
            mApoapsisRadius = mSemiMajorAxis * (1.0 + mEccentricity);
         }
         else if ((mOrbitSizeElements & OrbitSizeSpecification::cAPOAPSIS) != OrbitSizeSpecification::cNULL)
         {
            assert(mApoapsisRadiusValid || mApoapsisAltitudeValid);
            GetApoapsisRadius();
            mApoapsisRadius = mPeriapsisRadius * (1.0 + mEccentricity) / (1.0 - mEccentricity);
         }
      }
      else // semi-major axis and periapsis
      {
         assert(mSemiMajorAxisValid || mMeanMotionValid);
         assert(mPeriapsisRadiusValid || mPeriapsisAltitudeValid);
         GetSemiMajorAxis();
         GetPeriapsisRadius();
         mApoapsisRadius = 2.0 * mSemiMajorAxis - mPeriapsisRadius;
      }
      mApoapsisRadiusValid = true;
   }
   return mApoapsisRadius;
}

double UtOrbitalElements::GetPeriapsisAltitude() const
{
   if (!mPeriapsisAltitudeValid)
   {
      GetPeriapsisRadius();
      mPeriapsisAltitude      = ComputePeriapsisAltitude(mPeriapsisRadius, mCentralBodyPtr->GetMeanRadius());
      mPeriapsisAltitudeValid = true;
   }
   return mPeriapsisAltitude;
}

double UtOrbitalElements::GetApoapsisAltitude() const
{
   if (!mApoapsisAltitudeValid)
   {
      GetApoapsisRadius();
      mApoapsisAltitude      = ComputeApoapsisAltitude(mApoapsisRadius, mCentralBodyPtr->GetMeanRadius());
      mApoapsisAltitudeValid = true;
   }
   return mApoapsisAltitude;
}
//! Return the orbital period.
double UtOrbitalElements::GetPeriod() const
{
   return UtMath::cTWO_PI / GetMeanMotion();
}

//! Return the number of revolutions per day.
double UtOrbitalElements::GetRevolutionsPerDay() const
{
   return UtCalendar::cSEC_IN_DAY / GetPeriod();
}

//! Field 1.12 represents the ephemeris type (i.e., orbital model) used to generate the data.
//! Spacetrack Report Number 3 suggests the following assignments: 1=SGP, 2=SGP4, 3=SDP4, 4=SGP8, 5=SDP8.
//! However, this value is used for internal analysis only. All distributed element sets have a value of zero
//! and are generated using the SGP4/SDP4 orbital model (as appropriate).
//  TODO This can be used to provide the "type 8" ephemeris to be used as input for the SP propagator.
void UtOrbitalElements::SetEphemerisType(unsigned aEphemerisType)
{
   mEphemerisType = aEphemerisType;
}

// static
//! Compute a rotation matrix based on classical Euler angles.
//! Specifically these angles are the RAAN, inclination, and argument of periapsis.
void UtOrbitalElements::ComputePerifocalToInertialTransform(double   aRAAN,
                                                            double   aInclination,
                                                            double   aArgumentOfPeriapsis,
                                                            UtMat3d& aRotationMatrix)
{
   aRotationMatrix = ut::EulerAngles::ComputeClassicalTransform(aRAAN, aInclination, aArgumentOfPeriapsis);
}

void UtOrbitalElements::SetInclination(double aInclination)
{
   mInclination                       = aInclination;
   mPerifocalToInertialTransformValid = false;
   mNodalVectorMagnitudeValid         = false;
}

void UtOrbitalElements::SetRAAN(double aRAAN)
{
   mRAAN                              = aRAAN;
   mPerifocalToInertialTransformValid = false;
   mNodalVectorMagnitudeValid         = false;
}

void UtOrbitalElements::SetArgumentOfPeriapsis(double aArgumentOfPeriapsis)
{
   mArgumentOfPeriapsis               = aArgumentOfPeriapsis;
   mPerifocalToInertialTransformValid = false;
}

double UtOrbitalElements::GetAngularMomentum() const
{
   return mAngularMomentum;
}

const UtMat3d& UtOrbitalElements::GetPerifocalToInertialTransform() const
{
   if (!mPerifocalToInertialTransformValid)
   {
      mPerifocalToInertialTransform =
         ut::EulerAngles::ComputeClassicalTransform(mRAAN, mInclination, mArgumentOfPeriapsis);
      mPerifocalToInertialTransformValid = true;
   }
   return mPerifocalToInertialTransform;
}

void UtOrbitalElements::ResetValidityFlags()
{
   // ------------------------ eccentricity not specified, assume circular orbit ------------------------- //
   if (mOrbitSizeElements == (OrbitSizeSpecification::cSEMI_MAJOR_AXIS))
   {
      mEccentricityValid      = false;
      mPeriapsisRadiusValid   = false;
      mPeriapsisAltitudeValid = false;
      mApoapsisRadiusValid    = false;
      mApoapsisAltitudeValid  = false;
   }
   else if (mOrbitSizeElements == (OrbitSizeSpecification::cPERIAPSIS))
   {
      mEccentricityValid     = false;
      mSemiMajorAxisValid    = false;
      mApoapsisRadiusValid   = false;
      mApoapsisAltitudeValid = false;
   }
   else if (mOrbitSizeElements == (OrbitSizeSpecification::cAPOAPSIS))
   {
      mEccentricityValid      = false;
      mSemiMajorAxisValid     = false;
      mPeriapsisRadiusValid   = false;
      mPeriapsisAltitudeValid = false;
   }
   // ---------------------------------------------------------------------------------------------------- //
   else if (mOrbitSizeElements == (OrbitSizeSpecification::cSEMI_MAJOR_AXIS | OrbitSizeSpecification::cECCENTRICITY))
   {
      mPeriapsisRadiusValid   = false;
      mApoapsisRadiusValid    = false;
      mPeriapsisAltitudeValid = false;
      mApoapsisAltitudeValid  = false;
   }
   else if (mOrbitSizeElements == (OrbitSizeSpecification::cSEMI_MAJOR_AXIS | OrbitSizeSpecification::cPERIAPSIS))
   {
      mEccentricityValid     = false;
      mApoapsisRadiusValid   = false;
      mApoapsisAltitudeValid = false;
   }
   else if (mOrbitSizeElements == (OrbitSizeSpecification::cSEMI_MAJOR_AXIS | OrbitSizeSpecification::cAPOAPSIS))
   {
      mEccentricityValid      = false;
      mPeriapsisRadiusValid   = false;
      mPeriapsisAltitudeValid = false;
   }
   else if (mOrbitSizeElements == (OrbitSizeSpecification::cECCENTRICITY | OrbitSizeSpecification::cPERIAPSIS))
   {
      mSemiMajorAxisValid    = false;
      mApoapsisRadiusValid   = false;
      mApoapsisAltitudeValid = false;
   }
   else if (mOrbitSizeElements == (OrbitSizeSpecification::cECCENTRICITY | OrbitSizeSpecification::cAPOAPSIS))
   {
      mSemiMajorAxisValid     = false;
      mPeriapsisRadiusValid   = false;
      mPeriapsisAltitudeValid = false;
   }
   else // if (mOrbitSizeElements == (OrbitSizeSpecification::cPERIAPSIS | OrbitSizeSpecification::cAPOAPSIS))
   {
      mSemiMajorAxisValid = false;
      mEccentricityValid  = false;
   }
}

bool UtOrbitalElements::ValidateOrbitalElements()
{
   ResetValidityFlags();

   double sma       = 0.0;
   double periapsis = 0.0;
   double apoapsis  = 0.0;
   double radius    = mCentralBodyPtr->GetMeanRadius();

   if ((mOrbitSizeElements & OrbitSizeSpecification::cSEMI_MAJOR_AXIS) != OrbitSizeSpecification::cNULL)
   {
      assert(mSemiMajorAxisValid || mMeanMotionValid);
      if (mSemiMajorAxisValid)
      {
         sma = mSemiMajorAxis;
      }
      else if (mMeanMotionValid)
      {
         sma = ComputeSemiMajorAxis(mMeanMotion, mCentralBodyPtr->GetGravitationalParameter());
      }
      if (sma <= radius)
      {
         auto out = ut::log::error() << "semi_major_axis must be greater than the central_body mean radius";
         out.AddNote() << "semi_major_axis:     " << sma;
         out.AddNote() << "central body radius: " << radius;
         return false;
      }
   }
   if ((mOrbitSizeElements & OrbitSizeSpecification::cPERIAPSIS) != OrbitSizeSpecification::cNULL)
   {
      assert(mPeriapsisRadiusValid || mPeriapsisAltitudeValid);
      if (mPeriapsisRadiusValid)
      {
         periapsis = mPeriapsisRadius;
      }
      else if (mPeriapsisAltitudeValid)
      {
         periapsis = ComputePeriapsisRadius(mPeriapsisAltitude, radius);
      }
      if (periapsis <= radius)
      {
         auto out = ut::log::error() << "periapsis_radius must be greater than the central_body mean radius";
         out.AddNote() << "periapsis_radius:    " << periapsis;
         out.AddNote() << "central body radius: " << radius;
         return false;
      }
   }
   if ((mOrbitSizeElements & OrbitSizeSpecification::cAPOAPSIS) != OrbitSizeSpecification::cNULL)
   {
      assert(mApoapsisRadiusValid || mApoapsisAltitudeValid);
      if (mApoapsisRadiusValid)
      {
         apoapsis = mApoapsisRadius;
      }
      else if (mApoapsisAltitudeValid)
      {
         apoapsis = ComputeApoapsisRadius(mApoapsisAltitude, radius);
      }
      if (apoapsis <= radius)
      {
         auto out = ut::log::error() << "apoapsis_radius must be greater than the central_body mean radius";
         out.AddNote() << "apoapsis_radius:     " << apoapsis;
         out.AddNote() << "central body radius: " << radius;
         return false;
      }
   }

   if (mOrbitSizeElements == (OrbitSizeSpecification::cSEMI_MAJOR_AXIS | OrbitSizeSpecification::cPERIAPSIS))
   {
      if (periapsis > sma)
      {
         auto out = ut::log::error() << "periapsis_radius must be less than or equal to semi_major_axis";
         out.AddNote() << "periapsis_radius: " << periapsis;
         out.AddNote() << "semi_major_axis:  " << sma;
         return false;
      }
   }
   else if (mOrbitSizeElements == (OrbitSizeSpecification::cSEMI_MAJOR_AXIS | OrbitSizeSpecification::cAPOAPSIS))
   {
      if (apoapsis < sma)
      {
         auto out = ut::log::error() << "apoapsis_radius must be greater than or equal to semi_major_axis";
         out.AddNote() << "apoapsis_radius: " << apoapsis;
         out.AddNote() << "semi_major_axis: " << sma;
         return false;
      }
   }
   else if (mOrbitSizeElements == (OrbitSizeSpecification::cPERIAPSIS | OrbitSizeSpecification::cAPOAPSIS))
   {
      if (periapsis > apoapsis)
      {
         auto out = ut::log::error() << "periapsis_radius must be less than or equal to apoapsis_radius";
         out.AddNote() << "periapsis_radius: " << periapsis;
         out.AddNote() << "apoapsis_radius:  " << apoapsis;
         return false;
      }
   }
   return true;
}

void UtOrbitalElements::SetOrbitSizeSpecification(const OrbitSizeSpecification& aOrbitSizeSpecification)
{
   if (mOrbitSizeElementsPair.second != aOrbitSizeSpecification)
   {
      mOrbitSizeElementsPair.first  = mOrbitSizeElementsPair.second;
      mOrbitSizeElementsPair.second = aOrbitSizeSpecification;
   }
   mOrbitSizeElements = mOrbitSizeElementsPair.first | mOrbitSizeElementsPair.second;
}
