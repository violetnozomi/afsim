// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

// ****************************************************************************
//     This module was designed by Science Applications International
//     Corporation (SAIC) for the US Air Force, for use in the
//     Advanced Low Altitude Radar Model (ALARM), owned by Wright
//     Labs, Analysis and Evaluation Branch (WL/AAWA-1).
//
//     This module is derived from algorithms and code developed for
//     the Air Force by the Massachusetts Institute of Technology's
//     (MIT) Lincoln Laboratory.  Under the terms of the license
//     granted by MIT, the following restriction applies:
//
//     COPYRIGHT (C) 1985,1988 MIT LINCOLN LABORATORY ALL RIGHTS RESERVED
// ****************************************************************************

#ifndef WSFEM_ALARM_FORTRAN_HPP
#define WSFEM_ALARM_FORTRAN_HPP

#include <cmath>
#include <complex>

#include "UtMath.hpp"
// =================================================================================================
// Following are a collection of definitions that makes it easier to port FORTRAN to C++
// =================================================================================================

inline double dmod(double aValue1, double aValue2)
{
   return aValue1 - (static_cast<int>(aValue1 / aValue2) * aValue2);
}

inline double dsign(double aValue, double aReference)
{
   return UtMath::Sign(aValue, aReference);
}

inline int nint(double aValue)
{
   return (aValue >= 0.0) ? static_cast<int>(aValue + 0.5) : static_cast<int>(aValue - 0.5);
}

inline double dble(int aValue)
{
   return static_cast<double>(aValue);
}

inline float real(double aValue)
{
   return static_cast<float>(aValue);
}

inline float real(int aValue)
{
   return static_cast<float>(aValue);
}

inline std::complex<float> cmplx(float aRealPart, float aImagPart)
{
   return std::complex<float>(aRealPart, aImagPart);
}

inline std::complex<float> cmplx(float aRealPart)
{
   return std::complex<float>(aRealPart, 0.0);
}

inline std::complex<float> cmplx(double aRealPart, double aImagPart)
{
   return std::complex<float>(static_cast<float>(aRealPart), static_cast<float>(aImagPart));
}

inline std::complex<float> cmplx(double aRealPart)
{
   return std::complex<float>(static_cast<float>(aRealPart), 0.0);
}

inline std::complex<float> cmplx(double aRealPart, float aImagPart)
{
   return std::complex<float>(static_cast<float>(aRealPart), aImagPart);
}

inline float aimag(const std::complex<float>& aValue)
{
   return imag(aValue);
}

inline float cabs(const std::complex<float>& aValue)
{
   return std::abs(aValue);
}

inline std::complex<float> cexp(const std::complex<float>& aValue)
{
   return exp(aValue);
}

inline std::complex<float> csqrt(const std::complex<float>& aValue)
{
   return sqrt(aValue);
}

// ==============================================================================================
// The following is from ALARM 'physical_constants.f90.
// ==============================================================================================

//--------------------------------------------------------------------
// From CRC Standard Mathematical Tables and Formula, 30th edition,
// Copyright 1996.
//--------------------------------------------------------------------
const double pi = 3.141592653589793; // pi

//--------------------------------------------------------------------
// NATIONAL IMAGERY AND MAPPING AGENCY TECHNICAL REPORT 8350.2 Third
// Edition
//--------------------------------------------------------------------
const double rezero = 6371008.7714; // m : mean radius of semi-axis of earth

//--------------------------------------------------------------------
// From CRC Standard Mathematical Tables and Formula, 30th edition,
// Copyright 1996.
//--------------------------------------------------------------------
const double vlight = 299792458.0; // m/s : speed of light

// from 19th CRC Standard Mathematical Tables
const double sqrt2 = 1.41421356237309500; // square root of 2

//--------------------------------------------------------------------
// Derived constants
//--------------------------------------------------------------------
const double twopi   = 2.0 * pi;
const double fourpi  = 4.0 * pi;
const double fpicub  = 64.0 * pi * pi * pi;
const double halfpi  = 0.5 * pi;
const double forthpi = 0.25 * pi;
const double onethr  = 1.0 / 3.0;

//--------------------------------------------------------------------
// Misc unit conversions.
//--------------------------------------------------------------------
const double deg2rad = pi / 180.0; // degrees to radians
const double rad2deg = 180.0 / pi; // radians to degrees

// ==============================================================================================
// The following is from ALARM 'propagation_constants.f90'
// ==============================================================================================

//-------------------------------------------------------------------
// Parameters used by the various propagation routines.
//-------------------------------------------------------------------
const double dsqrt3 = 1.7320508075688772; // sqrt(3.0)
const double sqr3o2 = 0.5 * dsqrt3;
const double forthr = 4.0 / 3.0;
const double twothr = 2.0 / 3.0;
const double sr2po4 = 0.25 * 2.5066282746310002; // sqrt(twopi)
const double dsqrpi = 0.5 / 1.7724538509055159;  // sqrt(pi)
const double tsqrpi = 2.0 * 1.7724538509055159;  // sqrt(pi)

// ==============================================================================================
// The following is from ALARM 'types_polarization.f90'
// ==============================================================================================

const int pol_default     = -1;
const int pol_vertical    = 0;
const int pol_horizontal  = 1;
const int pol_slant_45    = 2;
const int pol_slant_135   = 3;
const int pol_rh_circular = 4;
const int pol_lh_circular = 5;

#endif
