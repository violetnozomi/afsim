// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtCentralBody.hpp"

#include <cmath>
#include <string>

#include "UtCalendar.hpp"
#include "UtEarth.hpp"
#include "UtEulerAngles.hpp"
#include "UtInput.hpp"
#include "UtJupiter.hpp"
#include "UtMath.hpp"
#include "UtMoon.hpp"
#include "UtOrbitalElements.hpp"
#include "UtOrbitalState.hpp"
#include "UtSun.hpp"
#include "UtVec3.hpp"

namespace ut
{

// Deprecated.
double CentralBody::ComputeSiderealAngle(const UtCalendar& aTime, double aLongitudeNutation) const
{
   return ComputeSiderealAngle(aTime);
}

//! A simple factory method to create an instance of the named central body.
//! @param aName The name of the CentralBody type to create.
//! @return The desired CentralBody type, or nullptr if aName is not one of the recognized central body types.
std::unique_ptr<CentralBody> CentralBody::Create(const std::string& aName)
{
   std::unique_ptr<CentralBody> centralBodyPtr = nullptr;
   if (aName == EarthWGS72::GetNameId())
   {
      centralBodyPtr = ut::make_unique<EarthWGS72>();
   }
   else if (aName == EarthWGS84::GetNameId())
   {
      centralBodyPtr = ut::make_unique<EarthWGS84>();
   }
   else if (aName == EarthEGM96::GetNameId())
   {
      centralBodyPtr = ut::make_unique<EarthEGM96>();
   }
   else if (aName == Moon::GetNameId())
   {
      centralBodyPtr = ut::make_unique<Moon>();
   }
   else if (aName == Sun::GetNameId())
   {
      centralBodyPtr = ut::make_unique<Sun>();
   }
   else if (aName == Jupiter::GetNameId())
   {
      centralBodyPtr = ut::make_unique<Jupiter>();
   }
   return centralBodyPtr;
}

//! Compute whether Line of Sight between aLocation1 and aLocation2 is blocked by a central body of size aBodyRadius at aBodyLocation.
//! @param aLocation1 The location of the first point to check LOS.
//! @param aLocation2 The location of the second point to check LOS.
//! @param aBodyLocation The location of the central body potentially blocking LOS.
//! @param aBodyRadius The radius of the central body potentially blocking LOS.
//! @note This algorithm is from Vallado, Fundamentals of Astrodynamics and Applications, 4th ed., p. 308 (algorithm 35).
bool CentralBody::LineOfSight(const UtVec3d& aLocation1, const UtVec3d& aLocation2, const UtVec3d& aBodyLocation, double aBodyRadius)
{
   UtVec3d r1M = aLocation1 - aBodyLocation;
   UtVec3d r2M = aLocation2 - aBodyLocation;

   double r1r2Dot = UtVec3d::DotProduct(r1M, r2M);
   double r1MagSq = r1M.MagnitudeSquared();
   double r2MagSq = r2M.MagnitudeSquared();

   double tauMin = (r1MagSq - r1r2Dot) / (r1MagSq + r2MagSq - 2 * r1r2Dot);

   if ((tauMin < 0) || (tauMin > 1))
   {
      return true;
   }

   long double cTauMinSq = (1 - tauMin) * r1MagSq + (r1r2Dot * tauMin);

   bool los = cTauMinSq >= std::pow(aBodyRadius, 2.0);

   return los;
}

//! A non-static call to CentralBody::LineOfSight that assumes the central body has a location of (0,0,0) and radius of
//! the Ellipsoid's semi-major axis.
bool CentralBody::LineOfSight(const UtVec3d& aLocation1, const UtVec3d& aLocation2) const
{
   return LineOfSight(aLocation1, aLocation2, UtVec3d(0.0, 0.0, 0.0), GetEllipsoid().GetSemiMajorAxis());
}

bool CentralBody::ProcessInput(UtInput& aInput)
{
   bool        myCommand = false;
   std::string command   = aInput.GetCommand();
   if (command == "polar_offset_angles")
   {
      aInput.ReadValueOfType(mPolarOffsetAngleX, UtInput::cANGLE);
      aInput.ReadValueOfType(mPolarOffsetAngleY, UtInput::cANGLE);
      myCommand = true;
   }
   return myCommand;
}

double CentralBody::GetSynchronousOrbitSemiMajorAxis() const
{
   return pow(GetGravitationalParameter() / (GetRotationRate() * GetRotationRate()), 1.0 / 3.0);
}

//! Compute the nutation angles in longitude and obliquity.
//! @param aTime The time at which the angles are valid.
//! @param aLongitudeNutation The nutation angle in the longitudinal direction.
//! @param aObliquityNutation The nutation angle in the direction of the obliquity
//! (i.e., in the plane of the pole and orbit plane normal vector).
void CentralBody::ComputeNutationAngles(const UtCalendar& aTime, double& aLongitudeNutation, double& aObliquityNutation) const
{
   aLongitudeNutation = 0.0;
   aObliquityNutation = 0.0;
}

//! Set the polar offset angles to be used in inertial conversion routines.
//! @param aPolarOffsetAngleX The polar offset angle in the WCS x direction.
//! @param aPolarOffsetAngleY The polar offset angle in the WCS y direction.
void CentralBody::SetPolarOffsetAngles(double aPolarOffsetAngleX, double aPolarOffsetAngleY)
{
   mPolarOffsetAngleX = aPolarOffsetAngleX;
   mPolarOffsetAngleY = aPolarOffsetAngleY;
}

//! Return the inertial acceleration of an entity at the specified location, associated with a gravitating central body.
//! @param aLocation The inertial location of the entity with respect to the central body.
//! @note The default implementation provides the result for a spherical, spherically symmetric central body.
//! Future implementations might return results from a full gravity model.
UtVec3d CentralBody::GetAccelerationInertial(const UtVec3d& aLocation) const
{
   double radius = aLocation.Magnitude();
   double accScaleFactor =
      -GetGravitationalParameter() / radius / radius / radius; // ~R^-3 because we multiply by location vector
   UtVec3d acceleration;
   acceleration.Multiply(aLocation, accScaleFactor);
   return acceleration;
}

//! Return the velocity required to escape the gravitational pull of the central body.
double CentralBody::GetEscapeVelocity(const UtVec3d& aLocation) const
{
   return sqrt(2.0 * GetGravitationalParameter() / aLocation.Magnitude());
}

// static
//! Convert from ecliptic coordinates to an equatorial frame by rotating
//! by the mean obliquity of the ecliptic.  The obliquity is calculated for the given
//! epoch by a fit in UtSun::GetMeanObliquity.
OrbitalStateVector CentralBody::ConvertEclipticToEquatorial(const UtCalendar&         aEpoch,
                                                            const OrbitalStateVector& aOrbitalStateVectorEcliptic)
{
   // Need to rotate by the obliquity to go from the ecliptic frame
   // to an earth-offset inertial.
   double obliquity = UtSun::GetMeanObliquity(aEpoch);
   double sino      = sin(obliquity);
   double coso      = cos(obliquity);

   // x-axis unchanged
   UtVec3d locationEquatorial;
   UtVec3d velocityEquatorial;
   auto&   locationEcliptic = aOrbitalStateVectorEcliptic.GetLocation();
   auto&   velocityEcliptic = aOrbitalStateVectorEcliptic.GetVelocity();

   locationEquatorial[0] = locationEcliptic[0];
   locationEquatorial[1] = coso * locationEcliptic[1] - sino * locationEcliptic[2];
   locationEquatorial[2] = sino * locationEcliptic[1] + coso * locationEcliptic[2];

   velocityEquatorial[0] = velocityEcliptic[0];
   velocityEquatorial[1] = coso * velocityEcliptic[1] - sino * velocityEcliptic[2];
   velocityEquatorial[2] = sino * velocityEcliptic[1] + coso * velocityEcliptic[2];

   return OrbitalStateVector(locationEquatorial, velocityEquatorial);
}

// static
//! Convert from ecliptic coordinates to an equatorial frame by rotating
//! by the mean obliquity of the ecliptic.  The obliquity is calculated for the given
//! epoch by a fit in UtSun::GetMeanObliquity.
OrbitalStateVector CentralBody::ConvertEquatorialToEcliptic(const UtCalendar&         aEpoch,
                                                            const OrbitalStateVector& aOrbitalStateVectorEquatorial)
{
   // Need to rotate by the obliquity to go from the ecliptic frame
   // to an earth-offset inertial.
   double obliquity = UtSun::GetMeanObliquity(aEpoch);
   double sino      = sin(obliquity);
   double coso      = cos(obliquity);

   // x-axis unchanged
   UtVec3d locationEcliptic;
   UtVec3d velocityEcliptic;
   auto&   locationEquatorial = aOrbitalStateVectorEquatorial.GetLocation();
   auto&   velocityEquatorial = aOrbitalStateVectorEquatorial.GetVelocity();

   locationEcliptic[0] = locationEquatorial[0];
   locationEcliptic[1] = coso * locationEquatorial[1] + sino * locationEquatorial[2];
   locationEcliptic[2] = -sino * locationEquatorial[1] + coso * locationEquatorial[2];

   velocityEcliptic[0] = velocityEquatorial[0];
   velocityEcliptic[1] = coso * velocityEquatorial[1] + sino * velocityEquatorial[2];
   velocityEcliptic[2] = -sino * velocityEquatorial[1] + coso * velocityEquatorial[2];

   return OrbitalStateVector(locationEcliptic, velocityEcliptic);
}

//! Convert a LLA location to WCS.
//! @note Default is to call the aggregated ellipsoid's conversion method.
void CentralBody::ConvertLLA_ToWCS(double aLat, double aLon, double aAlt, UtVec3d& aWCS) const
{
   mCentralBodyEllipsoidPtr->ConvertLLA_ToWCS(aLat, aLon, aAlt, aWCS);
}

//! Convert a WCS location to LLA.
//! @note Default is to call the aggregated ellipsoid's conversion method.
void CentralBody::ConvertWCS_ToLLA(const UtVec3d& aWCS, double& aLat, double& aLon, double& aAlt) const
{
   mCentralBodyEllipsoidPtr->ConvertWCS_ToLLA(aWCS, aLat, aLon, aAlt);
}

//! Convert a Local location to LLA.
//! @note Default is to call the aggregated ellipsoid's conversion method.
void CentralBody::ConvertLocalToLLA(const UtVec3d& aRefWCS,
                                    const double   aTransWCS[3][3],
                                    const UtVec3d& aLocal,
                                    double&        aLat,
                                    double&        aLon,
                                    double&        aAlt) const
{
   mCentralBodyEllipsoidPtr->ConvertLocalToLLA(aRefWCS, aTransWCS, aLocal, aLat, aLon, aAlt);
}

//! Convert a LLA location to local.
//! @note Default is to call the aggregated ellipsoid's conversion method.
void CentralBody::ConvertLLA_ToLocal(const UtVec3d& aRefWCS,
                                     const double   aTransWCS[3][3],
                                     double         aLat,
                                     double         aLon,
                                     double         aAlt,
                                     UtVec3d&       aLocal) const
{
   mCentralBodyEllipsoidPtr->ConvertLLA_ToLocal(aRefWCS, aTransWCS, aLat, aLon, aAlt, aLocal);
}

// override
UtMat3d CentralBody::ComputeECI_ToCentralPointInertialTransform(const UtCalendar& aEpoch) const
{
   return ut::EulerAngles::ComputeClassicalTransform(UtMath::cPI_OVER_2 + GetNorthPoleRA(aEpoch),
                                                     UtMath::cPI_OVER_2 - GetNorthPoleDec(aEpoch),
                                                     GetSiderealOffsetAngle(aEpoch));
}

// override
UtMat3d CentralBody::ComputePolarMotion() const
{
   UtMat3d polarMotion;
   polarMotion[0] = 1.0;
   polarMotion[2] = -mPolarOffsetAngleX;
   polarMotion[4] = 1.0;
   polarMotion[5] = mPolarOffsetAngleY;
   polarMotion[6] = mPolarOffsetAngleX;
   polarMotion[7] = -mPolarOffsetAngleY;
   polarMotion[8] = 1.0;
   return polarMotion;
}

} // namespace ut
