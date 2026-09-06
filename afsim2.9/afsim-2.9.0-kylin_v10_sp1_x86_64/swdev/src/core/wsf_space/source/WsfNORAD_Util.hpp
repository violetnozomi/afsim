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

#ifndef WSFNORAD_UTIL_HPP
#define WSFNORAD_UTIL_HPP

#include "UtTwoLineElement.hpp"

//(mjm) not using the "Retain Perturbation Values" #ifdef
//#defines moved to WsfNORAD_SpaceMover.hpp as const ints
// in an anonymous namespace.

// if we're retaining the perturbation values at the
// epoch,  it'll cost us another five doubles and an integer.
//#ifdef RETAIN_PERTURBATION_VALUES_AT_EPOCH
//#define DEEP_ARG_T_PARAMS     93
//#else
//#define DEEP_ARG_T_PARAMS     87
//#endif

//#define N_SGP_PARAMS          11
//#define N_SGP4_PARAMS         29
//#define N_SGP8_PARAMS         25
//#define N_SDP4_PARAMS        (9 + DEEP_ARG_T_PARAMS)
//#define N_SDP8_PARAMS        (11 + DEEP_ARG_T_PARAMS)

// 87 or 93 = size of the 'deep_arg_t' structure,  in 8-byte units
// You can use the above constants to minimize the amount of memory used,
//   but if you use the following constant,  you can be assured of having
//   enough memory for any of the five models:

//#define N_SAT_PARAMS         (11 + DEEP_ARG_T_PARAMS)
// static const int N_SAT_PARAMS = (11 + DEEP_ARG_T_PARAMS);

// Common "internal" arguments between deep-space functions;  users of
// the satellite routines shouldn't need to bother with any of this

/** A set of common "utility" structure definitions and functions for the WsfNORAD_SpaceMover class.
   @note This documentation is for reference only.  These structures and functions are, for the most part,
   to be used only by the WsfNORAD_SpaceMover class.
*/

namespace WsfNORAD_Util // included for documentation purposes.
{
//! The tle_t (Two Line Element - type) sructure used by the NORAD utilities.
//! This implementation maps directly to a UtTwoLineElement instance.
class tle_t
{
public:
   explicit tle_t(const UtTwoLineElement& aTLE)
      : epoch(aTLE.GetEpochDate())
      , xndt2o(aTLE.GetMeanMotionFirstDerivative())
      , xndd6o(aTLE.GetMeanMotionSecondDerivative())
      , bstar(aTLE.GetBstarDrag())
      , xincl(aTLE.GetInclination())
      , xnodeo(aTLE.GetRAAN())
      , eo(aTLE.GetEccentricity())
      , omegao(aTLE.GetArgumentOfPeriapsis())
      , xmo(aTLE.GetMeanAnomaly())
      , xno(aTLE.GetMeanMotion())
   {
      xno *= 60.0; // rad / min
   }

   ~tle_t() {}

   double epoch;  //! the Julian date of the TLE epoch (referenced to UTC)
                  //! (in days since 12:00:00 UTC on 1 January 4713 BC; gives t0).
   double xndt2o; //! 1st time derivative of mean motion
   double xndd6o; //! 2nd time derivative of mean motion
   double bstar;  //! "bstar" drag term
   double xincl;  //! inclination
   double xnodeo; //! right ascension of the ascending node (RAAN)
   double eo;     //! eccentricity
   double omegao; //! argument of the periapsis
   double xmo;    //! mean anomaly
   double xno;    //! mean motion

private:
   tle_t();
};

typedef struct
{
   double
      // Common between SGP4 and SDP4:
      aodp,
      cosio, sinio, omgdot, xmdot, xnodot, xnodp,
      // Used by dpinit part of Deep()
      eosq, betao, theta2, sing, cosg, betao2,

      // Used by dpsec and dpper parts of Deep()
      xll, omgadf, xnode, em, xinc, xn, t,

      // 'd####' secular coeffs for 12-hour, e>.5 orbits:
      d2201, d2211, d3210, d3222, d4410, d4422, d5220, d5232, d5421, d5433,

      // formerly static to Deep( ),   but more logically part of this struct:
      atime, del1, del2, del3, e3, ee2, omegaq, pe, pgh, ph, pinc, pl, preep, savtsn, se2, se3, sgh2, sgh3, sgh4, sh2,
      sh3, si2, si3, sl2, sl3, sl4, sse, ssg, ssh, ssi, ssl, thgr, xfact, xgh2, xgh3, xgh4, xh2, xh3, xi2, xi3, xl2,
      xl3, xl4, xlamo, xli, xni, xnq, xqncl, zcosgl, zcoshl, zcosil, zmol, zmos, zsingl, zsinhl, zsinil;

   // Epoch offsets,  described by Rob Matson,  added by BJG,
   // then commented out;  I don't think they really ought to
   // be used...
   //#ifdef RETAIN_PERTURBATION_VALUES_AT_EPOCH
   //    double pe0, pinc0, pl0, pgh0, ph0;
   //    int solar_lunar_init_flag;
   //#endif

   int resonance_flag, synchronous_flag;

} deep_arg_t;

double FMod2p(const double x);

void Deep_dpinit(const tle_t* tle, deep_arg_t* deep_arg);

void Deep_dpsec(const tle_t* tle, deep_arg_t* deep_arg);

void Deep_dpper(deep_arg_t* deep_arg);

void sxpx_posn_vel(const double  xnode,
                   const double  a,
                   const double  e,
                   const double* params,
                   const double  cosio,
                   const double  sinio,
                   const double  xincl,
                   const double  omega,
                   const double  xl,
                   double*       pos,
                   double*       vel);

typedef struct
{
   double coef;
   double coef1;
   double tsi;
   double s4;
   double a3ovk2;
   double eta;
} init_t;

void sxpx_common_init(double* params, const tle_t* tle, init_t* init, deep_arg_t* deep_arg);

// Table of constant values
//#define de2ra    (pi/180.)
//#define pi       3.141592653589793238462643383279502884197
//#define pio2     (pi/2.)
//#define x3pio2   (pi*1.5)
//#define twopi    (pi*2.)

const double e6a        = 1.0E-6;
const double two_thirds = (2. / 3.);
const double xj3        = -2.53881E-6;
const double xkmper     = 6.378135E3;
const double xmnpda     = 1440.;
const double ae         = 1.0;
const double ck2        = 5.413079E-4;

//#ifdef OLD_CONSTANTS
//#define ck4      6.209887E-7
//#define s        1.012229
//#define qoms2t   1.880279E-09
//#define xke      7.43669161E-2
//#else
const double ck4    = 6.2098875E-7;
const double s      = 1.0122292801892716;
const double qoms2t = 1.8802791590152709e-9;
const double xke    = 0.074366916133173408;
//#endif

//#define secday   86400.
const double rho = 1.5696615E-1;
} // namespace WsfNORAD_Util

#endif
