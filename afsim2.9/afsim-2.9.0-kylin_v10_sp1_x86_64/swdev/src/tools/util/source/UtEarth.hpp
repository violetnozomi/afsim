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

#ifndef UTEARTH_HPP
#define UTEARTH_HPP

#include "ut_export.h"

#include <limits>

#include "UtCentralBody.hpp"
#include "UtCentralBodyEllipsoid.hpp"

namespace ut
{
//! @brief A representation of the Earth as a central body.
//! @note This class is designed to minimize the computationally expensive
//! calls to update the nutation angles.  Typically, the user will cache the nutation matrix, so that ComputeNutation
//! (which in turn computes the nutation angles) only needs to be called when the time
//! difference between the epoch at which the matrix is computed, and the desired coordinate conversion time,
//! exceeds some threshold.  The sidereal angle computation also uses the longitude nutation angle (one of the two
//! nutation angles). Unlike the nutation matrix, the sidereal rotation matrix needs to be recomputed after practically
//! every time interval. Therefore, for performance reasons, ComputeSiderealAngle uses a cached value of the longitude
//! nutation, so it is the user's responsibility to call either ComputeNutation or UpdateNutationAngles before calling
//! ComputeSiderealMotion. (but, typically only called within the above-mentioned time difference threshold).
//! Alternatively, the method ComputeSiderealMotionWithNutationUpdate is provided that will both update the nutation
//! angles and compute the sidereal motion transform.
class UT_EXPORT EarthCentralBody : public CentralBody
{
public:
   explicit EarthCentralBody(const CentralBodyEllipsoid& aEllipsoid)
      : CentralBody(aEllipsoid)
   {
   }

   bool    UsingPrecession() const override { return true; }
   UtMat3d ComputePrecession(const UtCalendar& aTime) const override;
   bool    UsingNutation() const override { return true; }
   UtMat3d ComputeNutation(const UtCalendar& aTime) const override;
   void    UpdateNutationAngles(const UtCalendar& aTime);
   UtMat3d ComputeSiderealMotionWithNutationUpdate(const UtCalendar& aTime);
   void    SetRotationRate(double aRotationRate) override;

   double  GetNorthPoleRA(const UtCalendar&) const override { return 0.0; }
   double  GetNorthPoleDec(const UtCalendar&) const override { return UtMath::cPI_OVER_2; }
   UtVec3d GetLocationECI(const UtCalendar&) const override
   {
      return UtVec3d();
   } // Earth ECI location is always the origin.
   UtVec3d GetVelocityECI(const UtCalendar&) const override { return UtVec3d(); } // Earth ECI velocity is always zero.
   bool    IsEarth() const override { return true; }

   //! @name low-level methods.
   //@{
   double ComputeSiderealAngle(const UtCalendar& aTime) const override;
   double GetObliquity(const UtCalendar& aEpoch) const override;
   void ComputeNutationAngles(const UtCalendar& aTime, double& aLongitudeNutation, double& aObliquityNutation) const override;
   static double GetGAST(const UtCalendar& aEpoch);
   //@}

private:
   mutable double mLongitudeNutation{0.0};
   mutable double mObliquityNutation{0.0};
   mutable double mLastJ2000_UpdateTime{std::numeric_limits<double>::min()};
};

//! Definition for Earth WGS-72 ellipsoid.
//! @note Constants for the WGS-72 ellipsoid retrieved from
//! Department of Defense, World Geodetic System 1972,
//! May 1974, table 4.
class UT_EXPORT EarthEllipsoidWGS72 : public CentralBodyEllipsoid
{
public:
   static constexpr const char* GetNameId() { return "earth_wgs72"; }
   const char*                  GetName() const override { return GetNameId(); }
   CentralBodyEllipsoid*        Clone() const override { return new EarthEllipsoidWGS72(*this); }
   double                       GetSemiMajorAxis() const override { return 6378135.0; } // +- 5 meters
   double                       GetFlatteningReciprocal() const override { return 298.26; }
   double                       GetSemiMinorAxis() const override { return 6356750.5; }
   double                       GetMeanRadius() const override { return 6370998.9; }
   double                       GetFirstEccentricitySquared() const override { return 0.006694317778; }
   double                       GetSecondEccentricitySquared() const override { return 0.006739433689; }
   double                       GetRotationRate() const override { return 7.292115147 - 5; }
   double                       GetGravitationalParameter() const override { return 3.986008e+14; }
};

//! Earth Definition for WGS-72 Ellipsoid.
class UT_EXPORT EarthWGS72 : public EarthCentralBody
{
public:
   EarthWGS72()
      : EarthCentralBody(EarthEllipsoidWGS72())
   {
   }

   CentralBody*                 Clone() const override { return new EarthWGS72(*this); }
   static constexpr const char* GetNameId() { return EarthEllipsoidWGS72::GetNameId(); }
};

//! Definition for Earth WGS-84 ellipsoid.
//! @note Constants for the WGS-84 ellipsoid retrieved from
//! Department of Defense, World Geodetic System 1984 (NIMA TR 8350.2),
//! 3 Jan 2000, tables 3.1, 3.2, and 3.3.
class UT_EXPORT EarthEllipsoidWGS84 : public CentralBodyEllipsoid
{
public:
   static constexpr const char* GetNameId() { return "earth_wgs84"; }
   const char*                  GetName() const override { return GetNameId(); }
   CentralBodyEllipsoid*        Clone() const override { return new EarthEllipsoidWGS84(*this); }
   double                       GetSemiMajorAxis() const override { return 6378137.0; }
   double                       GetFlatteningReciprocal() const override { return 298.257223563; }
   double                       GetSemiMinorAxis() const override { return 6356752.3142; }
   double                       GetMeanRadius() const override { return 6371000.7900; }
   double                       GetFirstEccentricitySquared() const override { return 6.69437999014e-3; }
   double                       GetSecondEccentricitySquared() const override { return 6.73949674228e-3; }
   double                       GetRotationRate() const override { return 7.2921158553e-5; }
   double                       GetGravitationalParameter() const override { return 3.986004418e+14; }
};

//! Earth Definition for WGS84 Ellipsoid.
class UT_EXPORT EarthWGS84 : public EarthCentralBody
{
public:
   EarthWGS84()
      : EarthCentralBody(EarthEllipsoidWGS84())
   {
   }

   CentralBody*                 Clone() const override { return new EarthWGS84(*this); }
   static constexpr const char* GetNameId() { return EarthEllipsoidWGS84::GetNameId(); }
};

//! Definition for Earth EGM-96 ellipsoid.
//! @note The EGM-96 ellipsoid is identical to the WGS-84 ellipsoid.
class UT_EXPORT EarthEllipsoidEGM96 : public EarthEllipsoidWGS84
{
public:
   static constexpr const char* GetNameId() { return "earth_egm96"; }
   const char*                  GetName() const override { return GetNameId(); }
   CentralBodyEllipsoid*        Clone() const override { return new EarthEllipsoidEGM96(*this); }
};

//! Earth Definition for EGM96 Ellipsoid.
class UT_EXPORT EarthEGM96 : public EarthCentralBody
{
public:
   EarthEGM96()
      : EarthCentralBody(EarthEllipsoidEGM96())
   {
   }

   CentralBody*                 Clone() const override { return new EarthEGM96(*this); }
   static constexpr const char* GetNameId() { return EarthEllipsoidEGM96::GetNameId(); }
};

} // namespace ut

//-----------------------------------------------------------------------------
// Legacy UtEarth (Deprecated)
namespace UtEarth
{

constexpr double cACCEL_OF_GRAVITY = 9.80665; // (From NIST)

// WGS-84 defined constants

// Semi-major axis (a) in meters
constexpr double cA = 6378137.0;

// Reciprocal of ellipsoid flattening (1/f)
constexpr double cRF = 298.257223563;

// WGS-84 derived constants

// Semi-minor axis (b) in meters
constexpr double cB = 6356752.3142451794975;

// (First eccentricity squared (e ^ 2)
constexpr double cE_2 = 0.00669437999014131699445;

// Second eccentricity squared (e' ^2)
constexpr double cEP_2 = 0.00673949674227643495307;

// General constants

// Rotation rate (rad/sec)
constexpr double cOMEGA = 7.29211585530e-5; // (From Vallado).
// also see http://en.wikipedia.org/wiki/Sidereal_day; they verify the above to 10 significant figures:
// Given a tropical year of 365.242190402 days from Simon et al.[5] this gives a sidereal day of
// 86,400 x \tfrac{365.242190402}{366.242190402} , or 86,164.09053 seconds (take 2*pi / 86164.09053)
// constexpr double cOMEGA = 7.292115147e-5; // (old value)

// Mass (kg)
constexpr double cMASS = 5.9736e+24; //(kg; from JPL)

// Gravitational Parameter (m^3 / s^2)
constexpr double cGRAVITATIONAL_PARAMETER = 3.986004415e+14; // +- (8; last digit, JPL)
// See Reviews of Modern Physics, 77 (1; http://ssd.jpl.nasa.gov/iau-comm4/iausgnsrpt.ps )
// const double cGRAVITATIONAL_PARAMETER = 3.986005e+14; // (old value)

// Gravitational Parameter (m^3 / s^2)
constexpr double cGRAVITATIONAL_PARAMETER_WGS84 = 3.986004418e+14; // WGS-84 value

// Air Density under Standard Sea Level Conditions, kg / m^3:
// (This is the one to use for calculation of dynamic pressure (q = 1/2 rho V^2) )
constexpr double cSSL_AIR_DENSITY = 1.2250; // kg / m^3

constexpr double cSSL_PRESSURE = 101330.0; // N/m^2 or Pa

constexpr double cSSL_TEMP = 288.15; // K

}; // namespace UtEarth

#endif
