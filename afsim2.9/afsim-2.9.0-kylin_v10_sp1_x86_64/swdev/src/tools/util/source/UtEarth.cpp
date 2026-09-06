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

#include "UtEarth.hpp"

#include <cmath>

#include "UtCalendar.hpp"
#include "UtLog.hpp"
#include "UtSun.hpp"

// References:
//
// 1) Fundamentals of Astrodynamics and Applications, 4th ed., David A. Vallado
// 2) Naval Observatory Vector Astrometry Software (NOVAS), C Edition C3.1 (31 Mar 11),
//    https://aa.usno.navy.mil/software/novas/novas_c/novasc_info.php
// 3) "Expressions for IAU 2000 Precession Quantities", Capitaine et. al.,
//    Astron. & Astrophys. (2003), 412, pp. 567-586
//
// Additionally, see references cited in the NOVAS routines.

namespace
{
// Declarations of NOVAS routines:
double NOVAS_ee_ct(double aJulianTT_Date);
void   NOVAS_iau2000b(double aJulianTT_Date, double& aDeltaPsi, double& aDeltaEpsilon);

static constexpr double ASEC2RAD = 4.848136811095359935899141e-6;
} // namespace

//! Compute the sidereal angle using a cached value for longitude nutation.
//! @note In order to update this value, call UpdateNutationAngles or ComputeNutation.
//! @param aTime The time at which the sidereal angle is to be computed.
double ut::EarthCentralBody::ComputeSiderealAngle(const UtCalendar& aTime) const
{
   // Using a highly accurate Greenwich Apparent Sidereal Time computation (the one in UtCalendar is less accurate),
   // Compute the "equation of the equinoxes"
   double c_terms       = NOVAS_ee_ct(aTime.GetJulianTT_Date());
   double meanObliquity = UtSun::GetMeanObliquity(aTime);
   double eqeq          = mLongitudeNutation * cos(meanObliquity) + c_terms;

   // Compute the reference earth angle used for sidereal motion computations.
   return (aTime.GetGMST() / UtCalendar::cSEC_IN_DAY * UtMath::cTWO_PI + eqeq);
}

//! Update the cached nutation angles.
//! Because the sidereal angle computation uses the nutation angles,
//! the user may want to call this method prior to ComputeSiderealMotion.
//! Alternatively, calling ComputeNutation will automatically update the nutation angles.
//! @param aTime The time to which the nutation angles are updated.
void ut::EarthCentralBody::UpdateNutationAngles(const UtCalendar& aTime)
{
   if (aTime.GetJ2000_Date() != mLastJ2000_UpdateTime)
   {
      ComputeNutationAngles(aTime, mLongitudeNutation, mObliquityNutation);
      mLastJ2000_UpdateTime = aTime.GetJ2000_Date();
   }
}

//! Compute the rotation matrix accounting for precession.
//! @param aTime The epoch at which the precession matrix is desired.
UtMat3d ut::EarthCentralBody::ComputePrecession(const UtCalendar& aTime) const
{
   double eps0 = 84381.406;
   double t    = aTime.GetJ2000TT_Date() / 36525; // Centuries since J2000
   /*
      Numerical coefficients of psi_a, omega_a, and chi_a, along with
      epsilon_0, the obliquity at J2000.0, are 4-angle formulation from
      Capitaine et al. (2003), eqs. (4), (37), & (39).
   */
   double psia = ((((-0.0000000951 * t + 0.000132851) * t - 0.00114045) * t - 1.0790069) * t + 5038.481507) * t;

   double omegaa = ((((+0.0000003337 * t - 0.000000467) * t - 0.00772503) * t + 0.0512623) * t - 0.025754) * t + eps0;

   double chia = ((((-0.0000000560 * t + 0.000170663) * t - 0.00121197) * t - 2.3814292) * t + 10.556403) * t;

   eps0   = eps0 * ASEC2RAD;
   psia   = psia * ASEC2RAD;
   omegaa = omegaa * ASEC2RAD;
   chia   = chia * ASEC2RAD;

   double sa = sin(eps0);
   double ca = cos(eps0);
   double sb = sin(-psia);
   double cb = cos(-psia);
   double sc = sin(-omegaa);
   double cc = cos(-omegaa);
   double sd = sin(chia);
   double cd = cos(chia);
   /*
      Compute elements of precession rotation matrix equivalent to
      R3(chi_a) R1(-omega_a) R3(-psi_a) R1(epsilon_0).
   */
   UtMat3d precession;
   precession[0] = cd * cb - sb * sd * cc;
   precession[1] = -sd * cb - sb * cd * cc;
   precession[2] = sb * sc;
   precession[3] = cd * sb * ca + sd * cc * cb * ca - sa * sd * sc;
   precession[4] = -sd * sb * ca + cd * cc * cb * ca - sa * cd * sc;
   precession[5] = -sc * cb * ca - sa * cc;
   precession[6] = cd * sb * sa + sd * cc * cb * sa + ca * sd * sc;
   precession[7] = -sd * sb * sa + cd * cc * cb * sa + ca * cd * sc;
   precession[8] = -sc * cb * sa + cc * ca;

   return precession;
}

//! @brief Compute the nutation transform for the Earth.
//! @param aTime The time at which the nutation transform is desired.
//! @return The nutation transform.
//! @note This method computes cached values of the two nutation angles (mLongitudeNutation and mObliquityNutation).
//! Because the sidereal angle computation uses mLongitudeNutation, either this method or
//! ut::EarthCentralBody::UpdateNutationAngles should be called before calling
//! ut::EarthCentralBody::ComputeSiderealMotion.
UtMat3d ut::EarthCentralBody::ComputeNutation(const UtCalendar& aTime) const
{
   if (aTime.GetJ2000_Date() != mLastJ2000_UpdateTime)
   {
      ComputeNutationAngles(aTime, mLongitudeNutation, mObliquityNutation);
      mLastJ2000_UpdateTime = aTime.GetJ2000_Date();
   }

   double eps_bar    = GetObliquity(aTime);
   double eps        = eps_bar + mObliquityNutation;
   double cosDelPsi  = cos(mLongitudeNutation);
   double sinDelPsi  = sin(mLongitudeNutation);
   double sinEps     = sin(eps);
   double cosEps     = cos(eps);
   double sinEps_bar = sin(eps_bar);
   double cosEps_bar = cos(eps_bar);

   UtMat3d nutation;

   // Compute the nutation matrix according to Vallado eq. 3.86
   nutation[0] = cosDelPsi;
   nutation[1] = sinDelPsi * cosEps;
   nutation[2] = sinEps * sinDelPsi;
   nutation[3] = -sinDelPsi * cosEps_bar;
   nutation[4] = cosEps * cosDelPsi * cosEps_bar + sinEps * sinEps_bar;
   nutation[5] = sinEps * cosDelPsi * cosEps_bar - sinEps_bar * cosEps;
   nutation[6] = -sinDelPsi * sinEps_bar;
   nutation[7] = sinEps_bar * cosEps * cosDelPsi - sinEps * cosEps_bar;
   nutation[8] = sinEps * sinEps_bar * cosDelPsi + cosEps * cosEps_bar;

   return nutation;
}

//! @brief Update the nutation angles and compute the sidereal motion in one call.
//! @param aTime The time at which the sidereal motion transform is desired.
//! @return The sidereal motion transform.
UtMat3d ut::EarthCentralBody::ComputeSiderealMotionWithNutationUpdate(const UtCalendar& aTime)
{
   UpdateNutationAngles(aTime);
   return ComputeSiderealMotion(aTime);
}

// override
void ut::EarthCentralBody::SetRotationRate(double aRotationRate)
{
   auto error = ut::log::error()
                << "EarthCentralBody::SetRotationRate: Earth rotation rate is defined by the earth ellipsoid model.";
   error.AddNote() << "Ellipsoid: " << GetName();
}

double ut::EarthCentralBody::GetObliquity(const UtCalendar& aEpoch) const
{
   static constexpr double cRAD_PER_ARCSECOND = UtMath::cRAD_PER_DEG / 3600.0;
   double                  T                  = aEpoch.GetJ2000_Date() / 36525.0;
   double                  T2                 = T * T;
   double                  T3                 = T2 * T;
   return ((84381.448 - 46.8150 * T - 0.00059 * T2 + 0.001813 * T3) * cRAD_PER_ARCSECOND);
}

//! @brief Compute the nutation angles for use in computing both the nutation and sidereal motion transformations.
//! @param aTime The time at which the nutation angles are desired.
//! @param aLongitudeNutation The output longitude nutation angle.
//! @param aObliquityNutation The output obliquity nutation angle.
//! @note Calling this method is computationally expensive.
void ut::EarthCentralBody::ComputeNutationAngles(const UtCalendar& aTime,
                                                 double&           aLongitudeNutation,
                                                 double&           aObliquityNutation) const
{
   NOVAS_iau2000b(aTime.GetJulianTT_Date(), aLongitudeNutation, aObliquityNutation);
}

//! Compute the Greenwich Apparent Sidereal Time (GAST), using a precise calculation of
//! the equation of the equinoxes (eqeq).
double ut::EarthCentralBody::GetGAST(const UtCalendar& aEpoch)
{
   double julianTT = aEpoch.GetJulianTT_Date();
   double longitudeNutation;
   double obliquityNutation;
   NOVAS_iau2000b(julianTT, longitudeNutation, obliquityNutation);

   // Using a highly accurate Greenwich Apparent Sidereal Time computation (the one in UtCalendar is less accurate),
   // Compute the "equation of the equinoxes"
   double c_terms       = NOVAS_ee_ct(julianTT);
   double meanObliquity = UtSun::GetMeanObliquity(aEpoch);
   double eqeq          = longitudeNutation * cos(meanObliquity) + c_terms;
   return (aEpoch.GetGMST() + eqeq * UtCalendar::cSEC_IN_DAY / UtMath::cTWO_PI);
}

namespace
{
///*
//   Naval Observatory Vector Astrometry Software (NOVAS)
//   C Edition, Version 3.1
//
//   nutation.c: Nutation models
//
//   U. S. Naval Observatory
//   Astronomical Applications Dept.
//   Washington, DC
//   http://www.usno.navy.mil/USNO/astronomical-applications
//*/
//************************************************************************
// Constants needed by the NOVAS routines:
static constexpr double T0      = 2451545.00000000; // Julian date of epoch J2000.0.
static constexpr double TWOPI   = 6.283185307179586476925287;
static constexpr double ASEC360 = 1296000.0;
//************************************************************************
void NOVAS_fund_args(double t, double a[5])
/*
------------------------------------------------------------------------

   PURPOSE:
      To compute the fundamental arguments (mean elements) of the Sun
      and Moon.

   REFERENCES:
      Simon et al. (1994) Astronomy and Astrophysics 282, 663-683,
         esp. Sections 3.4-3.5.

   INPUT
   ARGUMENTS:
      t (double)
         TDB time in Julian centuries since J2000.0

   OUTPUT
   ARGUMENTS:
      a[5] (double)
         Fundamental arguments, in radians:
          a[0] = l (mean anomaly of the Moon)
          a[1] = l' (mean anomaly of the Sun)
          a[2] = F (mean argument of the latitude of the Moon)
          a[3] = D (mean elongation of the Moon from the Sun)
          a[4] = Omega (mean longitude of the Moon's ascending node);
                 from Simon section 3.4(b.3),
                 precession = 5028.8200 arcsec/cy)

   RETURNED
   VALUE:
      None.

   GLOBALS
   USED:
      ASEC2RAD, ASEC360  novascon.c

   FUNCTIONS
   CALLED:
      fmod               math.h

   VER./DATE/
   PROGRAMMER:
      V1.0/10-97/JAB (USNO/AA)
      V1.1/07-98/JAB (USNO/AA): Place arguments in the range 0-TWOPI
                                radians.
      V1.2/09-03/JAB (USNO/AA): Incorporate function 'norm_ang'.
      V1.3/11-03/JAB (USNO/AA): Update with Simon et al. expressions.
      V1.4/01-06/JAB (USNO/AA): Remove function 'norm_ang'; rewrite for
                                consistency with Fortran.
      V1.5/02-11/WKP (USNO/AA): Clarified a[4] description in prolog.

   NOTES:
      1. This function is the C version of NOVAS Fortran routine
      'funarg'.

------------------------------------------------------------------------
*/
{
   a[0] = fmod(485868.249036 + t * (1717915923.2178 + t * (31.8792 + t * (0.051635 + t * (-0.00024470)))), ASEC360) *
          ASEC2RAD;

   a[1] = fmod(1287104.79305 + t * (129596581.0481 + t * (-0.5532 + t * (0.000136 + t * (-0.00001149)))), ASEC360) *
          ASEC2RAD;

   a[2] = fmod(335779.526232 + t * (1739527262.8478 + t * (-12.7512 + t * (-0.001037 + t * (0.00000417)))), ASEC360) *
          ASEC2RAD;

   a[3] = fmod(1072260.70369 + t * (1602961601.2090 + t * (-6.3706 + t * (0.006593 + t * (-0.00003169)))), ASEC360) *
          ASEC2RAD;

   a[4] = fmod(450160.398036 + t * (-6962890.5431 + t * (7.4722 + t * (0.007702 + t * (-0.00005939)))), ASEC360) * ASEC2RAD;
}

double NOVAS_ee_ct(double aJulianTT_Date)
/*
------------------------------------------------------------------------

   PURPOSE:
      To compute the "complementary terms" of the equation of the
      equinoxes.

   REFERENCES:
      Capitaine, N., Wallace, P.T., and McCarthy, D.D. (2003). Astron. &
         Astrophys. 406, p. 1135-1149. Table 3.
      IERS Conventions (2010), Chapter 5, p. 60, Table 5.2e.
         (Table 5.2e presented in the printed publication is a truncated
         series. The full series, which is used in NOVAS, is available
         on the IERS Conventions Center website in file tab5.2e.txt.)
         ftp://tai.bipm.org/iers/conv2010/chapter5/

   INPUT
   ARGUMENTS:
      jd_high (double)
         High-order part of TT Julian date.
      jd_low (double)
         Low-order part of TT Julian date.
      accuracy (short int)
         Selection for accuracy
            = 0 ... full accuracy
            = 1 ... reduced accuracy

   OUTPUT
   ARGUMENTS:
      None

   RETURNED
   VALUE:
      (double)
         Complementary terms, in radians.

   GLOBALS
   USED:
      T0, ASEC2RAD       novascon.c
      TWOPI              novascon.c

   FUNCTIONS
   CALLED:
      norm_ang           novas.c
      fund_args          novas.c
      fmod               math.h
      sin                math.h
      cos                math.h

   VER./DATE/
   PROGRAMMER:
      V1.0/09-03/JAB (USNO/AA)
      V1.1/12-04/JAB (USNO/AA) Added low-accuracy formula.
      V1.2/01-06/WKP (USNO/AA) Changed 'mode' to 'accuracy'.
      V1.3/11-10/JAB (USNO/AA) Updated reference and notes.
      V1.4/03-11/WKP (USNO/AA) Added braces to 2-D array initialization
                               to quiet gcc warnings.

   NOTES:
      1. The series used in this function was derived from the first
      reference.  This same series was also adopted for use in the IAU's
      Standards of Fundamental Astronomy (SOFA) software (i.e.,
      subroutine eect00.for and function eect00.c).
      2. The low-accuracy series used in this function is a simple
      implementation derived from the first reference, in which terms
      smaller than 2 microarcseconds have been omitted.
      3. This function is based on NOVAS Fortran routine 'eect2000',
      with the low-accuracy formula taken from NOVAS Fortran routine
      'etilt'.

------------------------------------------------------------------------
*/
{
   double fa2[5];
   double t = (aJulianTT_Date - T0) / 36525.0;

   /*
      Low accuracy mode: Terms smaller than 2 microarcseconds omitted.
   */

   NOVAS_fund_args(t, fa2);
   double c_terms =
      2640.96e-6 * sin(fa2[4]) + 63.52e-6 * sin(2.0 * fa2[4]) +
      11.75e-6 * sin(2.0 * fa2[2] - 2.0 * fa2[3] + 3.0 * fa2[4]) + 11.21e-6 * sin(2.0 * fa2[2] - 2.0 * fa2[3] + fa2[4]) -
      4.55e-6 * sin(2.0 * fa2[2] - 2.0 * fa2[3] + 2.0 * fa2[4]) + 2.02e-6 * sin(2.0 * fa2[2] + 3.0 * fa2[4]) +
      1.98e-6 * sin(2.0 * fa2[2] + fa2[4]) - 1.72e-6 * sin(3.0 * fa2[4]) - 0.87e-6 * t * sin(fa2[4]);

   return (c_terms *= ASEC2RAD);
}

void NOVAS_iau2000b(double aJulianTT_Date, double& aDeltaPsi, double& aDeltaEpsilon)
/*
------------------------------------------------------------------------

   PURPOSE:
      To compute the forced nutation of the non-rigid Earth based on
      the IAU 2000B precession/nutation model.

   REFERENCES:
      McCarthy, D. and Luzum, B. (2003). "An Abridged Model of the
         Precession & Nutation of the Celestial Pole," Celestial
         Mechanics and Dynamical Astronomy, Volume 85, Issue 1,
         Jan. 2003, p. 37. (IAU 2000B)
      IERS Conventions (2003), Chapter 5.

   INPUT
   ARGUMENTS:
      jd_high (double)
         High-order part of TT Julian date.
      jd_low (double)
         Low-order part of TT Julian date.

   OUTPUT
   ARGUMENTS:
      *dpsi (double)
         Nutation (luni-solar + planetary) in longitude, in radians.
      *deps (double)
         Nutation (luni-solar + planetary) in obliquity, in radians.

   RETURNED
   VALUE:
      None.

   GLOBALS
   USED:
      T0, ASEC2RAD, TWOPI

   FUNCTIONS
   CALLED:
      fmod      math.h
      sin       math.h
      cos       math.h

   VER./DATE/
   PROGRAMMER:
      V1.0/09-03/JAB (USNO/AA)
      V1.1/12-10/JAB (USNO/AA): Implement static storage class for const
                                arrays.
      V1.2/03-11/WKP (USNO/AA): Added braces to 2-D array initialization
                                to quiet gcc warnings.

   NOTES:
      1. IAU 2000B reproduces the IAU 2000A model to a precision of
      1 milliarcsecond in the interval 1995-2020.

------------------------------------------------------------------------
*/
{
   /*
      Planetary nutation (arcsec).  These fixed terms account for the
      omission of the long-period planetary terms in the truncated model.
   */

   double dpplan = -0.000135;
   double deplan = 0.000388;

   /*
      Luni-Solar argument multipliers:
          L     L'    F     D     Om
   */

   static const short int nals_t[77][5] = {
      {0, 0, 0, 0, 1},   {0, 0, 2, -2, 2}, {0, 0, 2, 0, 2},   {0, 0, 0, 0, 2},   {0, 1, 0, 0, 0},  {0, 1, 2, -2, 2},
      {1, 0, 0, 0, 0},   {0, 0, 2, 0, 1},  {1, 0, 2, 0, 2},   {0, -1, 2, -2, 2}, {0, 0, 2, -2, 1}, {-1, 0, 2, 0, 2},
      {-1, 0, 0, 2, 0},  {1, 0, 0, 0, 1},  {-1, 0, 0, 0, 1},  {-1, 0, 2, 2, 2},  {1, 0, 2, 0, 1},  {-2, 0, 2, 0, 1},
      {0, 0, 0, 2, 0},   {0, 0, 2, 2, 2},  {0, -2, 2, -2, 2}, {-2, 0, 0, 2, 0},  {2, 0, 2, 0, 2},  {1, 0, 2, -2, 2},
      {-1, 0, 2, 0, 1},  {2, 0, 0, 0, 0},  {0, 0, 2, 0, 0},   {0, 1, 0, 0, 1},   {-1, 0, 0, 2, 1}, {0, 2, 2, -2, 2},
      {0, 0, -2, 2, 0},  {1, 0, 0, -2, 1}, {0, -1, 0, 0, 1},  {-1, 0, 2, 2, 1},  {0, 2, 0, 0, 0},  {1, 0, 2, 2, 2},
      {-2, 0, 2, 0, 0},  {0, 1, 2, 0, 2},  {0, 0, 2, 2, 1},   {0, -1, 2, 0, 2},  {0, 0, 0, 2, 1},  {1, 0, 2, -2, 1},
      {2, 0, 2, -2, 2},  {-2, 0, 0, 2, 1}, {2, 0, 2, 0, 1},   {0, -1, 2, -2, 1}, {0, 0, 0, -2, 1}, {-1, -1, 0, 2, 0},
      {2, 0, 0, -2, 1},  {1, 0, 0, 2, 0},  {0, 1, 2, -2, 1},  {1, -1, 0, 0, 0},  {-2, 0, 2, 0, 2}, {3, 0, 2, 0, 2},
      {0, -1, 0, 2, 0},  {1, -1, 2, 0, 2}, {0, 0, 0, 1, 0},   {-1, -1, 2, 2, 2}, {-1, 0, 2, 0, 0}, {0, -1, 2, 2, 2},
      {-2, 0, 0, 0, 1},  {1, 1, 2, 0, 2},  {2, 0, 0, 0, 1},   {-1, 1, 0, 1, 0},  {1, 1, 0, 0, 0},  {1, 0, 2, 0, 0},
      {-1, 0, 2, -2, 1}, {1, 0, 0, 0, 2},  {-1, 0, 0, 1, 0},  {0, 0, 2, 1, 2},   {-1, 0, 2, 4, 2}, {-1, 1, 0, 1, 1},
      {0, -2, 2, -2, 1}, {1, 0, 2, 2, 1},  {-2, 0, 2, 2, 2},  {-1, 0, 0, 0, 2},  {1, 1, 2, -2, 2}};

   /*
      Luni-Solar nutation coefficients, unit 1e-7 arcsec:
      longitude (sin, t*sin, cos), obliquity (cos, t*cos, sin)

      Each row of coefficients in 'cls_t' belongs with the corresponding
      row of fundamental-argument multipliers in 'nals_t'.
   */

   static const double cls_t[77][6] = {{-172064161.0, -174666.0, 33386.0, 92052331.0, 9086.0, 15377.0},
                                       {-13170906.0, -1675.0, -13696.0, 5730336.0, -3015.0, -4587.0},
                                       {-2276413.0, -234.0, 2796.0, 978459.0, -485.0, 1374.0},
                                       {2074554.0, 207.0, -698.0, -897492.0, 470.0, -291.0},
                                       {1475877.0, -3633.0, 11817.0, 73871.0, -184.0, -1924.0},
                                       {-516821.0, 1226.0, -524.0, 224386.0, -677.0, -174.0},
                                       {711159.0, 73.0, -872.0, -6750.0, 0.0, 358.0},
                                       {-387298.0, -367.0, 380.0, 200728.0, 18.0, 318.0},
                                       {-301461.0, -36.0, 816.0, 129025.0, -63.0, 367.0},
                                       {215829.0, -494.0, 111.0, -95929.0, 299.0, 132.0},
                                       {128227.0, 137.0, 181.0, -68982.0, -9.0, 39.0},
                                       {123457.0, 11.0, 19.0, -53311.0, 32.0, -4.0},
                                       {156994.0, 10.0, -168.0, -1235.0, 0.0, 82.0},
                                       {63110.0, 63.0, 27.0, -33228.0, 0.0, -9.0},
                                       {-57976.0, -63.0, -189.0, 31429.0, 0.0, -75.0},
                                       {-59641.0, -11.0, 149.0, 25543.0, -11.0, 66.0},
                                       {-51613.0, -42.0, 129.0, 26366.0, 0.0, 78.0},
                                       {45893.0, 50.0, 31.0, -24236.0, -10.0, 20.0},
                                       {63384.0, 11.0, -150.0, -1220.0, 0.0, 29.0},
                                       {-38571.0, -1.0, 158.0, 16452.0, -11.0, 68.0},
                                       {32481.0, 0.0, 0.0, -13870.0, 0.0, 0.0},
                                       {-47722.0, 0.0, -18.0, 477.0, 0.0, -25.0},
                                       {-31046.0, -1.0, 131.0, 13238.0, -11.0, 59.0},
                                       {28593.0, 0.0, -1.0, -12338.0, 10.0, -3.0},
                                       {20441.0, 21.0, 10.0, -10758.0, 0.0, -3.0},
                                       {29243.0, 0.0, -74.0, -609.0, 0.0, 13.0},
                                       {25887.0, 0.0, -66.0, -550.0, 0.0, 11.0},
                                       {-14053.0, -25.0, 79.0, 8551.0, -2.0, -45.0},
                                       {15164.0, 10.0, 11.0, -8001.0, 0.0, -1.0},
                                       {-15794.0, 72.0, -16.0, 6850.0, -42.0, -5.0},
                                       {21783.0, 0.0, 13.0, -167.0, 0.0, 13.0},
                                       {-12873.0, -10.0, -37.0, 6953.0, 0.0, -14.0},
                                       {-12654.0, 11.0, 63.0, 6415.0, 0.0, 26.0},
                                       {-10204.0, 0.0, 25.0, 5222.0, 0.0, 15.0},
                                       {16707.0, -85.0, -10.0, 168.0, -1.0, 10.0},
                                       {-7691.0, 0.0, 44.0, 3268.0, 0.0, 19.0},
                                       {-11024.0, 0.0, -14.0, 104.0, 0.0, 2.0},
                                       {7566.0, -21.0, -11.0, -3250.0, 0.0, -5.0},
                                       {-6637.0, -11.0, 25.0, 3353.0, 0.0, 14.0},
                                       {-7141.0, 21.0, 8.0, 3070.0, 0.0, 4.0},
                                       {-6302.0, -11.0, 2.0, 3272.0, 0.0, 4.0},
                                       {5800.0, 10.0, 2.0, -3045.0, 0.0, -1.0},
                                       {6443.0, 0.0, -7.0, -2768.0, 0.0, -4.0},
                                       {-5774.0, -11.0, -15.0, 3041.0, 0.0, -5.0},
                                       {-5350.0, 0.0, 21.0, 2695.0, 0.0, 12.0},
                                       {-4752.0, -11.0, -3.0, 2719.0, 0.0, -3.0},
                                       {-4940.0, -11.0, -21.0, 2720.0, 0.0, -9.0},
                                       {7350.0, 0.0, -8.0, -51.0, 0.0, 4.0},
                                       {4065.0, 0.0, 6.0, -2206.0, 0.0, 1.0},
                                       {6579.0, 0.0, -24.0, -199.0, 0.0, 2.0},
                                       {3579.0, 0.0, 5.0, -1900.0, 0.0, 1.0},
                                       {4725.0, 0.0, -6.0, -41.0, 0.0, 3.0},
                                       {-3075.0, 0.0, -2.0, 1313.0, 0.0, -1.0},
                                       {-2904.0, 0.0, 15.0, 1233.0, 0.0, 7.0},
                                       {4348.0, 0.0, -10.0, -81.0, 0.0, 2.0},
                                       {-2878.0, 0.0, 8.0, 1232.0, 0.0, 4.0},
                                       {-4230.0, 0.0, 5.0, -20.0, 0.0, -2.0},
                                       {-2819.0, 0.0, 7.0, 1207.0, 0.0, 3.0},
                                       {-4056.0, 0.0, 5.0, 40.0, 0.0, -2.0},
                                       {-2647.0, 0.0, 11.0, 1129.0, 0.0, 5.0},
                                       {-2294.0, 0.0, -10.0, 1266.0, 0.0, -4.0},
                                       {2481.0, 0.0, -7.0, -1062.0, 0.0, -3.0},
                                       {2179.0, 0.0, -2.0, -1129.0, 0.0, -2.0},
                                       {3276.0, 0.0, 1.0, -9.0, 0.0, 0.0},
                                       {-3389.0, 0.0, 5.0, 35.0, 0.0, -2.0},
                                       {3339.0, 0.0, -13.0, -107.0, 0.0, 1.0},
                                       {-1987.0, 0.0, -6.0, 1073.0, 0.0, -2.0},
                                       {-1981.0, 0.0, 0.0, 854.0, 0.0, 0.0},
                                       {4026.0, 0.0, -353.0, -553.0, 0.0, -139.0},
                                       {1660.0, 0.0, -5.0, -710.0, 0.0, -2.0},
                                       {-1521.0, 0.0, 9.0, 647.0, 0.0, 4.0},
                                       {1314.0, 0.0, 0.0, -700.0, 0.0, 0.0},
                                       {-1283.0, 0.0, 0.0, 672.0, 0.0, 0.0},
                                       {-1331.0, 0.0, 8.0, 663.0, 0.0, 4.0},
                                       {1383.0, 0.0, -2.0, -594.0, 0.0, -2.0},
                                       {1405.0, 0.0, 4.0, -610.0, 0.0, 2.0},
                                       {1290.0, 0.0, 0.0, -556.0, 0.0, 0.0}};

   /*
      Interval between fundamental epoch J2000.0 and given date.
   */

   double t = (aJulianTT_Date - T0) / 36525.0;

   /*
      ** Luni-solar nutation. **

      Fundamental (Delaunay) arguments from Simon et al. (1994),
      in radians.
   */

   /*
      Mean anomaly of the Moon.
   */

   double el = fmod(485868.249036 + t * 1717915923.2178, ASEC360) * ASEC2RAD;

   /*
      Mean anomaly of the Sun.
   */

   double elp = fmod(1287104.79305 + t * 129596581.0481, ASEC360) * ASEC2RAD;

   /*
      Mean argument of the latitude of the Moon.
   */

   double f = fmod(335779.526232 + t * 1739527262.8478, ASEC360) * ASEC2RAD;

   /*
      Mean elongation of the Moon from the Sun.
   */

   double d = fmod(1072260.70369 + t * 1602961601.2090, ASEC360) * ASEC2RAD;

   /*
      Mean longitude of the ascending node of the Moon.
   */

   double om = fmod(450160.398036 - t * 6962890.5431, ASEC360) * ASEC2RAD;

   /*
     Initialize the nutation values.
   */

   double dp = 0.0;
   double de = 0.0;

   /*
     Summation of luni-solar nutation series (in reverse order).
   */
   for (short int i = 76; i >= 0; i--)
   {
      /*
         Argument and functions.
      */

      double arg = fmod((double)nals_t[i][0] * el + (double)nals_t[i][1] * elp + (double)nals_t[i][2] * f +
                           (double)nals_t[i][3] * d + (double)nals_t[i][4] * om,
                        TWOPI);

      double sarg = sin(arg);
      double carg = cos(arg);

      /*
         Term.
      */

      dp += (cls_t[i][0] + cls_t[i][1] * t) * sarg + cls_t[i][2] * carg;
      de += (cls_t[i][3] + cls_t[i][4] * t) * carg + cls_t[i][5] * sarg;
   }

   /*
     Convert from 0.1 microarcsec units to radians.
   */

   double factor = 1.0e-7 * ASEC2RAD;
   double dpsils = dp * factor;
   double depsls = de * factor;

   /*
     ** Planetary nutation. **

     Fixed terms to allow for long-period nutation, in radians.
   */

   double dpsipl = dpplan * ASEC2RAD;
   double depspl = deplan * ASEC2RAD;

   /*
      Total: Add planetary and luni-solar components.
   */

   //*dpsi = dpsipl + dpsils;
   //*deps = depspl + depsls;
   aDeltaPsi     = dpsipl + dpsils;
   aDeltaEpsilon = depspl + depsls;
}

} // namespace
