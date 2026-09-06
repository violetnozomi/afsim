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

#ifndef UTMATH_HPP
#define UTMATH_HPP

#include "ut_export.h"

#include <cfloat>
#include <cmath>
#include <cstdint>
#include <limits>

#include "UtTypeTraits.hpp"

//! Math constants and utilities.
namespace UtMath
{
//! e
constexpr double cE = 2.718281828459045235360;
//! Pi
constexpr double cPI = 3.141592653589793238460;
//! Pi / 2
constexpr double cPI_OVER_2 = 1.570796326794896619230;
//! Pi / 4
constexpr double cPI_OVER_4 = 0.7853981633974483096156;
//! 2 * Pi
constexpr double cTWO_PI = 6.283185307179586476920;
//! 4 * Pi
constexpr double cFOUR_PI = 12.566370614359172953840;
//! Radians per degree
constexpr double cRAD_PER_DEG = 0.01745329251994329576923; // PI / 180
//! Degrees per radian
constexpr double cDEG_PER_RAD = 57.29577951308232087721; // 180 / PI

//! Feet per meter
constexpr double cFT_PER_M = 3.28083989501312335958; // 1 / 0.3048
//! Meters per feet
constexpr double cM_PER_FT = 0.3048;

//! Meters per nautical mile
constexpr double cM_PER_NM = 1852.0;
//! Meters per mile
constexpr double cM_PER_MI = 1609.344;
//! Feet per nautical mile
constexpr double cFT_PER_NM = 6076.11548556430446194; // 1852 / 0.3048
//! Nautical Miles per feet
constexpr double cNM_PER_FT = 0.000164579;
//! Nautical Miles per meter
constexpr double cNM_PER_M = 0.000539957;
//! Feet per mile
constexpr double cFT_PER_MI = 5280.0;

//! Miles per hour per meters per second.
constexpr double cMPH_PER_MPS = 2.2369362920544;

//! Meters per second per mile per hour.
constexpr double cMPS_PER_MPH = 0.44704;

//! Nautical miles per hour per meters per second.
constexpr double cNMPH_PER_MPS = 1.9438444924406;
//! Nautical miles per hour per feet per second.
constexpr double cNMPH_PER_FPS = 0.592483801295896;

//! Meters per second per nautical miles per hour.
constexpr double cMPS_PER_NMPH = 0.514444444444445;
//! Feet per second per nautical miles per hour.
constexpr double cFPS_PER_NMPH = 1.687809857101196;

//! Pounds (mass) per kilogram
constexpr double cLB_PER_KG = 2.2046226; // inverse = 0.45359237;
//! Pounds (force) force per Newton
constexpr double cLB_PER_NT = 0.22480894; // inverse = 4.4482216;
//! Newtons per pound (force)
constexpr double cNT_PER_LB = 1.0 / cLB_PER_NT;
//! Kilograms per slug
constexpr double cKG_PER_SLUG = 14.5939029; // inverse = 0.068521766
//! Kilograms per pound (force)
constexpr double cKG_PER_LB = 1.0 / cLB_PER_KG;

//! Newton meters per foot pound
constexpr double cNM_PER_FTLB = 0.73756;

//! 0 degrees Celsius in Fahrenheit
constexpr double cZERO_C_IN_F = 32.0;
//! 0 degrees Celsius in kelvin
constexpr double cZERO_C_IN_K = 273.15;
//! 0 degrees Fahrenheit in Kelvin
constexpr double cABS_ZERO_IN_F = -459.67;
//! degree Kelvin per degree Fahrenheit
constexpr double cDEG_F_PER_DEG_K = 9.0 / 5.0; // 1.8
//! degree Fahrenheit per degree Kelvin
constexpr double cDEG_K_PER_DEG_F = 5.0 / 9.0; // 0.555555...

//! pascals per 1 pound-force per square inch
constexpr double cPASCAL_PER_PSI = 6894.75729;
//! pascals per 1 pound-force per square foot
constexpr double cPASCAL_PER_PSF = 47.88020833;
//! pound-force per square foot, per 1 pascal
constexpr double cPSF_PER_PASCAL = 0.020885456;

//! slug per cubic foot, per kilogram per cubic meter
constexpr double cSLUG_FT3_PER_KG_M3 = 0.001940320337;

//! Speed of light: m/s (From NIST/2002 CODATA)
constexpr double cLIGHT_SPEED = 2.99792458E8; // m/s (exact)

//! Gravitational constant: m^3 / kg / s^2 (From NIST/2002 CODATA)
constexpr double cGRAVITATIONAL_CONSTANT = 6.6742E-11; // m^3 / kg / s^2

//! Planck constant: J-s (From NIST/2006 CODATA)
constexpr double cPLANCK_CONSTANT = 6.62606896E-34; // J-s

//! Boltzmann constant: J/deg-K (From NIST/2002 CODATA)
constexpr double cBOLTZMANN_CONSTANT = 1.3806505E-23; // J/deg-K;

//! Euler's (Euler-Mascheroni) constant (From NIST 5.2ii)
constexpr double cEULER_CONSTANT = 0.5772156649;

//! Elementary charge : Coulombs (From NIST/2010 CODATA)
constexpr double cELEMENTARY_CHARGE = 1.602176565E-19; // C

constexpr double cDOUBLE_MIN  = std::numeric_limits<double>::min();
constexpr double cDOUBLE_MAX  = std::numeric_limits<double>::max();
constexpr double cINTEGER_MIN = std::numeric_limits<int>::min();
constexpr double cINTEGER_MAX = std::numeric_limits<int>::max();

//! Performs a cyclic left permutation of the bits of the given integer.
//!
//! As an example, given `1100`, and a shift of 1, this function will return
//! `1001`. Note that this function is incompatible with the C++20
//! implementations currently. We should make it compatible once we move
//! to C++17.
//!
//! @param val: The value to rotate. Must be an unsigned integer type.
//! @param s: The number of bits to rotate by
template<typename INT>
constexpr typename ut::enable_if_t<std::is_unsigned<INT>::value, INT> rotl(INT val, uint32_t s) noexcept
{
   // Necessary to do this because C++11 doesn't allow declarations of variables, even if constexpr.
   using M = std::integral_constant<INT, std::numeric_limits<INT>::digits - 1>;
   return (val << (s & M::value)) | (val >> ((~(s & M::value) + 1) & M::value));
}

//! Interpolate a value from one range to another
//! @param aValue The value to be interpolated
//! @param aValueMin The min value of the original scale
//! @param aValueMax The max value of the original scale
//! @param aRangeMin The min value of the new scale
//! @param aRangeMax The max value of the new scale
UT_EXPORT double Lerp(const double& aValue,
                      const double& aValueMin,
                      const double& aValueMax,
                      const double& aRangeMin,
                      const double& aRangeMax);

UT_EXPORT double TempKtoF(double aTempK);
UT_EXPORT double TempFtoK(double aTempF);

UT_EXPORT bool AngleIsBetween(double aAngle, // radians
                              double aLowerLimit,
                              double aUpperLimit);

UT_EXPORT bool AngleWithinTolerance(double aAngle, // radians
                                    double aTolerance);


//! Limit a value to the [lower_limit,upper_limit].
//! @param aValue The value to be checked.
//! @param aLowerLimit The lower limit of allowable values.
//! @param aUpperLimit The upper limit of allowable values.
//! @returns aValue if in the range [aLowerLimit, aUpperLimit].
//! @returns aLowerLimit if aValue < aLowerLimit.
//! @returns aUpperLimit if aValue > aUpperLimit.
//! @note It is the callers responsibility to ensure aUpperLimit >= aLowerLimit.
template<class T>
T Limit(T aValue, T aLowerLimit, T aUpperLimit)
{
   T value(aValue);
   if (aValue < aLowerLimit)
   {
      value = aLowerLimit;
   }
   else if (aValue > aUpperLimit)
   {
      value = aUpperLimit;
   }
   return value;
}

//! Limit a value to the [-limit,limit].
//! @param aValue The value to be checked.
//! @param aLimit A positive value used to form the endpoints of the range.
//! @returns aValue if in the range [-aLimit, aLimit].
//! @returns -aLimit if aValue < -aLimit.
//! @returns aLimit  if aValue > aLimit.
template<class T>
T Limit(T aValue, T aLimit)
{
   return Limit(aValue, -aLimit, aLimit);
}

UT_EXPORT double LnGamma(int aZ);

//! Return the first value with the same sign as the second value.
//! This is the equivalent of the FORTRAN function SIGN.
//! @param aValue The value to be returned with the same sign as the second argument.
//! @param aReference The value whose sign is to be transferred to the first argument.
//! @returns abs(aValue) if aReference >= 0.
//! @returns -abs(aValue) if aReference < 0.
//! @note The IEEE floating point standard allows a negative 0, which is still zero.
//! If passed as 'aReference', the return value will be positive.
template<class T>
T Sign(T aValue, T aReference)
{
   // Take the absolute value of the first argument. (Note, test must be > 0.0 and not
   // >= 0.0 so a -0 gets converted to a +0).
   T absValue = (aValue > 0) ? (aValue) : (-aValue);
   // Propagate the sign of the second value to the first value.
   return ((aReference >= 0) ? (absValue) : (-absValue));
}

//! Extract the sign of a number.
//! @param aValue The number from which the sign is to be extracted.
//! @returns 1 if aValue >= 0 or -1 if aValue < 0.
template<class T>
T Sign(T aValue)
{
   return (aValue >= 0 ? 1 : -1);
}

//! Returns true if the values are separated by less than the given epsilon.
template<class T>
constexpr bool NearlyEqual(T aValue1, T aValue2, T aEpsilon = std::numeric_limits<T>::epsilon())
{
   // Return true if the values compare equal or if the values are within aEpsilon
   // Note: this must be a single return statement because C++11 requires it for constexpr functions.
   return (aValue1 == aValue2) || (aValue1 < aValue2 ? aValue2 - aValue1 : aValue1 - aValue2) < aEpsilon;
}

template<class T>
constexpr bool NearlyZero(T aValue, T aEpsilon = std::numeric_limits<T>::epsilon())
{
   return NearlyEqual<T>(aValue, 0, aEpsilon);
}

//! Round a float to the nearest integer (away from zero)
//! @param aValue The number to round
//! @returns the nearest integer value away from zero
UT_EXPORT int Round(float aValue);

//! Round a double to the nearest 64-bit integer (away from zero)
//! @param aValue The number to round
//! @returns the nearest 64-bit integer value away from zero
UT_EXPORT long long Round64(double aValue);

//! Convert a linearly scaled value to a decibel scaled value.
//! @param aLinearValue Linear value to be converted.
//! @return Decibel scaled value = 10.0 * log(aValue);
UT_EXPORT double LinearToDB(double aLinearValue);

//! Convert a decibel-scaled value to a linearly-scaled value.
//! @param aDB_Value Decibel value to be converted.
//! @return Linearly-scaled value = 10^(db/10).
UT_EXPORT double DB_ToLinear(double aDB_Value);

UT_EXPORT double SafeLinearToDB(double aLinearValue);

//! Normalizes the angle to be in the range of aMinAngle <= angle < aMinAngle + 360.0
//! Guaranteed O(1) runtime when aMinAngle is in the range of -360.0 to 360.0
//! @param aAngle The angle in degrees.
//! @param aMinAngle The minimum angle of the desired range in degrees.
UT_EXPORT double NormalizeAngleDeg(double aAngle, double aMinAngle);

//! Normalizes sine or cosine results to the range of -1 <= value <= 1.0
//! @param  aSinCos The calculated value of sin or cos to be normalized.
UT_EXPORT double NormalizeSinCosMinusOne_One(double aSinCos);

//! Normalizes the angle to be in the range of 0 <= angle < 360
//! @param aAngle The angle in degrees.
UT_EXPORT double NormalizeAngle0_360(double aAngle);

//! Normalizes the angle to be in the range of -180 <= angle < 180
//! @param aAngle The angle in degrees.
UT_EXPORT double NormalizeAngleMinus180_180(double aAngle);

//! Normalizes the angle to be in the range of aMinAngle <= angle < aMinAngle + 2PI
//! Guaranteed O(1) runtime runtime when aMinAngle is in the range of -2PI to 2PI
//! @param aAngle The angle in radians.
//! @param aMinAngle The minimum angle of the desired range in radians.
UT_EXPORT double NormalizeAngleRad(double aAngle, double aMinAngle);

//! Normalizes the angle to be in the range of 0 <= angle < 2PI
//! @param aAngle The angle in radians.
UT_EXPORT double NormalizeAngle0_TwoPi(double aAngle);

//! Normalizes the angle to be in the range of -PI <= angle < PI
//! @param aAngle The angle in radians.
UT_EXPORT double NormalizeAngleMinusPi_Pi(double aAngle);

//! Normalizes the Az angle to be in the range of -PI <= angle < PI
//! Normalizes the El angle to be in the range of -PI/2 <= angle < PI/2
//! @param aAz The angle in radians. This param may be modified.
//! @param AEl The angle in radians. This param may be modified.
UT_EXPORT void NormalizeAzElAnglesRad(double& aAz, double& aEl);

//! Calculates the bisector of the two angles.
//! The result is normalized to the range of -PI <= angle < PI
//! @param aAngle1 The first angle in radians.
//! @param aAngle2 The second angle in radians.
UT_EXPORT double AngleBisector(double aAngle1, double aAngle2);

//! Returns the difference of two angles
//! @param aAngle1 The first angle in radians.
//! @param aAngle2 The second angle in radians.
UT_EXPORT double AngleDifference(double aAngle1, double aAngle2);

//! Returns the next double after aValue in the direction of aTowards
UT_EXPORT double NextAfter(double aValue, double aTowards);


//! Returns the Error Function of the given value.
UT_EXPORT double ErrorFunction(double aValue);

} // namespace UtMath

#endif
