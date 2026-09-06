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

#ifndef BRAWLERUTIL_HPP
#define BRAWLERUTIL_HPP

#include "UtEntity.hpp"
class UtInput;
#include "brawler_export.h"

#include "UtFortMat3.hpp"
#include "UtMatrix.hpp"
#include "UtVec3.hpp"

//! Namespace to contain BRAWLER unique elements including:
//! constants, enumerations, and utilty functions
namespace BRAWLER
{
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// BRAWLER Constants
double const grav           = 32.17405;
double const pi             = 3.141592654;
double const rad            = pi / 180.0;
double const hafpi          = 1.570796327; // pi/2.0;
double const twopi          = 6.283185308; // pi*2.0;
double const root2          = 1.414213562;
double const ftnmi          = 0.000164578834;
double const eps            = 1.e-35;
double const re             = 20890377.0;
double const xlarge         = 1.e38;
double const degrees2meters = 111194.92664456; // 111194.92664455873734580833886041
double const degrees2feet   = 364812.76313;    // 364812.76313253408783362183046679
double const cFtSec2PerG    = 32.1740;         // TODO - use "grav" instead?

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// BRAWLER enumerations  /// ////////////////////////////////////////////
enum WeaponType
{
   nowepn = 0, // none
   kndsrm = 1, // short range missile
   kndmrm = 2, // medium range missile
   kndlrm = 3, // long range missile
   kndmis = 3, // missile
   kndgun = 4, // guns
   knddew = 5, // directed energy weapon
   kndwpn = 5  // weapon
};
enum ManeuverValueComponentType // used by TunnelVisionMultiplier() to replace "tnl_mult", copied from valhst.fi
{
   i_voffef  = 1,  // offensive
   i_vlospd  = 2,  // low speed recovery
   i_vform   = 3,  // station keeping (formation)
   i_compr   = 4,  // route-point
   i_valdfx  = 5,  // defensive
   i_gaval   = 6,  // ground avoidance
   i_vvec    = 7,  // vector following
   i_villum  = 8,  // illumination (missile guiding)
   i_vmusup  = 9,  // mutual support
   i_valevd  = 10, // missile evasion
   i_vmaim   = 11, // missile aiming
   i_vdisng  = 12, // disengage
   i_sepval  = 13, // separation
   i_vescrt  = 14, // escort bomber
   i_vsamz   = 15, // SAM avoidance
   i_vavbdd  = 16, // bad aspect avoidance
   i_hddval  = 17, // hard-deck
   i_terrval = 18, // terrain
   i_vdew    = 19, // directed energy weapon
   i_vtpt    = 20  // third part targeting
};
enum MissileModeState
{
   NOTASK     = 0, //! 0 = not interested in shot (default)
   INTERCEPT  = 1, //! 1 = interested in shot, get in evenlope (i.e. have a WEAPON_TASK)
   AIMFIRE    = 2, //! 2 = shot imminent/in progress, aim and fire (i.e. weapon in flight)
   ILLUMINATE = 3  //! 3 = illuminate target
};
enum WeaponSeekerType
{
   mxskr  = 4,
   irskr  = 1,
   saskr  = 2,
   acskr  = 3,
   armskr = 4
};
enum InherentBiasFaultTypes
{
   i_ovroff = 1, // OVERLY OFFENSIVE
   i_nmusup = 2, // ATTENTION TO MUTUAL SUPPORT
   i_nospd  = 3  // ATTENTION TO AIRSPEED MAINTENANCE
};
enum JobType
{
   FLIGHT_LEAD  = 1,
   ELEMENT_LEAD = 2,
   WINGMAN      = 3
};
enum MissionType
{
   ROUTE  = 1,
   ESCORT = 2,
   CAP    = 3
};

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// BRAWLER utility functions ////////////////////////////////////////////

//! @name BRAWLER min/max Functions
//! Return the smallest/largest of the values passed in.
//!@{
BRAWLER_EXPORT double amin1(const double a, const double b);
BRAWLER_EXPORT double amax1(const double a, const double b);
BRAWLER_EXPORT double amax1(const double a, const double b, const double c);
//!@}

//! @name BRAWLER sign Functions
//! Return the values of a with sign of b. Equivalent to FORTRAN SIGN()
//!@{
int    sign(const int a, const int b);
double sign(const double a, const double b);
//!@}

//! Returns true if the bit at pos in var is set.
bool btest(const int var, const int pos);
bool lbit(const int word, const int n);

//! @name BRAWLER inverse trigonometric functions
//! Just renames of std acos, asin
double arccos(const double a);
double arcsin(const double a);

//! @param x - 3 vector
//! @return Absolute value of the component of x with the largest absolute value
double avnorm(const UtVec3d& x);

/*not needed yet*/
// lmatch(a,b,c) (std::find(std::begin(a), std::end(a), c) != std::end(a))
// iand(a,b) (a&b)

//! @name BRAWLER Border Functions
//! These are generalized shape functions used throughout brawler to
//! provide shapes used for value functions.
//! These functions replicate BRAWLER v7.5 border.f
//!@{
//! Border is a smooth step function transitioning at 0.
//! The formula used for positive and negative arguments are
//! designed to make border be continuous in first derivative at z=0.
//! Also, border(z,z0) = 1-border(-z,z0).
//! @param z basic independent variable
//! @param z0 width-defining parameter
//! @return ranges from 0 for z < z0 to 1 for z > z0
double border(const double z, const double z0);

//! Bell-shaped curve peaking at origin.
//! @param z basic independent variable
//! @param z0 width-defining parameter
//! @return 1 for z near 0, 0 for abs(z) > z0
double cauchy(const double z, const double z0);

//! Reward is a linear transformation of cauchy with range
//! from -1 to 1 instead of 0 to 1.
//! @param z basic independent variable
//! @param z0 width-defining parameter
//! @return 1 for z near 0, -1 for abs(z) > z0.
double reward(const double z, const double z0);
//!@}

//! Computes separation between two 3-vectors
//! This function replicates BRAWLER v7.5 dist.f
//! @param a  3-vec, one of the two vectors
//! @param b  3-vec, the other vector
//! @return magnitude of a - b
double dist(const UtVec3d& a, const UtVec3d& b);

//! Computes specific energy from vectors
//! This function replicates BRAWLER v7.5 energy.f
//! @param pos  position vector (feet)
//! @param vel  velocity vector (feet/sec)
//! @return specific_energy = alt + 0.5 * v**2 / grav
double energy(const UtVec3d& pos, const UtVec3d& vel);

//! Computes specific energy from quantities
//! This function replicates BRAWLER v7.5 evdwnt.f line 143
//! @see energy
//! @param z  altitude (feet)
//! @param s  speed (feet/sec)
//! @return specific_energy = alt + 0.5 * v**2 / grav
double egy(double z, double s);

//! Routine to construct orientations. Used when a coordinate system must be created.
//! x-axis constructed by unitv; y-axis direction given by
//! crossing bzin with bxin, cros1 assures normalization;
//! finally, z-axis is given by crossing x and y axes. A
//! transpose operation is then performed to make required orientation.
//! This function replicates BRAWLER v7.5 makecs.f
//! @param xVec   3-vec its direction defines orientation's x-axis
//! @param zVec   3-vec its direction, together with xVec, defines
//!               orientation's x-z plane. Ambiguity is resolved by having
//!               dot product of z-axis with zVec positive for makecs
//! @return resultant coordinate system
UtFortMat3d makecs(const UtVec3d& xVec, const UtVec3d& zVec);

//! Routine to construct orientations. Used when a coordinate system must be created.
//! x-axis constructed by unitv; y-axis direction given by
//! crossing bzin with bxin, cros1 assures normalization;
//! finally, z-axis is given by crossing x and y axes. A
//! transpose operation is then performed to make required orientation.
//! This function replicates BRAWLER v7.5 makecs.f
//! @param xVec   3-vec its direction defines orientation's x-axis
//! @param zVec   3-vec its direction, together with xVec, defines
//!               orientation's x-z plane. Ambiguity is resolved by having
//!               dot product of z-axis with zVec negative for makcsi.
//!               makcsi is useful for constructing wind and/or body axes
//!               when zVec is a lift.
//! @return resultant coordinate system
UtFortMat3d makcsi(const UtVec3d& xVec, const UtVec3d& zVec);

//! Equivalent to makecs with bzin=(0,0,1)
//! @see makecs
UtFortMat3d makex(const UtVec3d& cxin);

//! Equiv to makex with cxin assumed zero.
UtFortMat3d makeh(const UtVec3d& cxin);
// makecs_inner(const UtVec3d& bx, const UtVec3d& by);

double headeg(UtVec3d v);

//! Linear ramp function that replicates BRAWLER ramp.f
//! Circumvents issue of whether XLO < XHI, etc. by computing a
//! linear fit to points RAMP(XLO)=0, RAMP(XHI)=1 and truncating
//! range to 0 when result<0 and 1 when result>1.
//! @param xlo  If xlo < xhi then ramp=0 for all xval <= xlo. If xlo>xhi then ramp=0 for all xval >= xlo
//! @param xval Ramp function independent variable
//! @param xhi  If xlo < xhi then ramp=1 for all xval >= xhi. If xlo>xhi then ramp=1 for all xval <= xhi
//! @return 0 for xval outside of range (xlo,xhi) on the xlo side of range.
//!         1 for xval outside of range (xlo,xhi) on the xhi side of range.
//!         Ramp varies linearly from 0 to 1 as xval goes from xlo to xhi.
BRAWLER_EXPORT double ramp(const double xlo, const double xval, const double xhi);

//! Splits a vector into parallel & perpendicular parts
//! This function replicates BRAWLER v7.5 vsplit.f
//! @param  v  - vector to be split
//! @param  x  - vector that defines "parallel" direction
//! @param vl  - part of "v" parallel to "x"
//! @param vt  - part of "v" transverse to "x"
void vsplit(const UtVec3d& v, const UtVec3d& x, UtVec3d& vl, UtVec3d& vt);

//! Forces value to lie in a specified range
//! This function replicates BRAWLER v7.5 xlimit.f
//! @param xreq   real - value to be limited to range
//! @param xmax   real - upper range limit
//! @param xmin   real - lower range limit
//! @return xmin if xreq < xmin, xmax if xreq > xmax, xreq if xmin <= xreq <= xmax
double xlimit(const double xreq, const double xmax, const double xmin);


UtFortMat3d bodyax(const UtFortMat3d& rwe, // returns rbe,  leave in util???
                   double             alpha);

UtFortMat3d NEDToECSRotation(UtEntity& ref);

UtFortMat3d mrotx(UtFortMat3d a, double chi);
//! From atmos.f  calculating DR
double AtmosphereDensityRatio(double altitude);
//! From atmos.f  calculating VS
double SpeedOfSound(double altitude);

//! Replicates ecmsig() with mode for LOS angle sigma only
double LOSangleSigma(const UtFortMat3d& M, const UtMatrixd& R6, double R);

//! Prinat an error message and aborts simulation.
//! This function replicates BRAWLER v7.5 nabort.f
//! @param str the message to display on abort
BRAWLER_EXPORT void nabort(const std::string& str);

//! Does a 1-dimensional interpolation
//! This function replicates BRAWLER v7.5 param1.f
//! @param  i      index of lower bounding table entry
//! @param  xx     interpolation weight of upper bounding entry
//! @param  aray   array dim(int) of (double) - the table
//! @return interpolated value
BRAWLER_EXPORT double param1(int i, double xx, double* aray);

//! Lineraly interpolates a 1-dimensional table with variable tablesize
//! This function replicates BRAWLER v7.5 tbl1.f
//! @param x      real - z(x) is the desired output
//! @param z      array dim(*) of (real) - lookup table
//! @param xval   array dim(*) of (real) - defines values of x for which z
//!               is tabulated.  thus, z(i) = function_z(xval(i))
//!               xval must be monotonically increasing
//! @param nxval  int - number of table points
//! @return interpolated value of z(x), out-of-range x's truncated
BRAWLER_EXPORT double tbl1(double x, double* z, double* xval, int nxval);

//! Interpolates a one-dimensional lookup table.
//! Used for evenly-spaced table case.  Performs work
//! that callers of param1 may need to perform in-line.
//! This function replicates BRAWLER v7.5 tbl1b.f
//! @param x      Real - independent variable
//! @param z      Array dim(nx) of real - tabulated dependent variable.  The
//!               abstract result Z(X) is desired.
//! @param nx     int - Dimension of Z
//! @param xmin   Real - Z(1) corresponds to Z(XMIN)
//! @param xmax   Real - Z(NX) corresponds to Z(XMAX)
//! @return Interpolated Z(X).  Values outside XMIN, XMAX are truncated.
double tbl1b(double x, double* z, int nx, double xmin, double xmax); // TODO

//! Interpolates a 2-dimensional table with variable tablesize.
//! Uses the srch function to bracket x or y between two xval's (or
//! yval's), and then performs a linear interpolation on z.
//! Out of bounds x and y values are truncated.
//! This routine can handle NXVAL or NYVAL = 1.
//! This function replicates BRAWLER v7.5 tbl2.f
//! @param x      real - z(x,y) is the desired output
//! @param y      real - z(x,y) is the desired output
//! @param z      array dim(nxval,nyval) of (real) - lookup table
//! @param xval   array dim(nxval) of (real) - defines values of x for which z
//!               is tabulated. xval must be monotonically increasing
//! @param nxval  int - number of table points for first z-index
//! @param yval   array dim(nyval) of (real) - defines values of y for which z
//!               is tabulated. yval must be monotonically increasing
//! @param nyval  int - number of table points for second z-index
//! @return interpolated value of z(x,y), out-of-range x's truncated
BRAWLER_EXPORT double tbl2(double x, double y, double* z, double* xval, int nxval, double* yval, int nyval);

//! Interpolates a 2-dimensional table with variable tablesize.
//! This function replicates BRAWLER v7.5 tbl2e.f
//! @param x      real - z(x,y) is the desired output
//! @param y      real - z(x,y) is the desired output
//! @param z      array dim(nxval,nyval) of (real) - lookup table
//! @param xval   array dim(nxval) of (real) - defines values of x for which z
//!               is tabulated. xval must be monotonically increasing
//! @param nxval  int - number of table points for first z-index
//! @param yval   array dim(nyval) of (real) - defines values of y for which z
//!               is tabulated. yval must be monotonically increasing
//! @param nyval  int - number of table points for second z-index
//! @return interpolated value of z(x,y), out-of-range x's extrapolated
double tbl2e(double x, double y, double* z, double* xval, int nxval, double* yval, int nyval);

//! Inverse interpolation of a 2-dim table on right index
//! This function replicates BRAWLER v7.5 invtb2.f
//! @param tbl  array dim(nr,nc) of (real) - target of inverse interpolation
//! @param z    desired value of tbl
//! @param nr   number of rows in tbl
//! @param ir   row index (see pr)
//! @param pr   ir and pr define row on which inverse interpolation is
//!             to be performed.  that is, interpolate on "virtual" row of index
//!             ir+pr    ( 0<=pr<=1 ).
//! @param revdir start search from "right" hand edge of the table
//! @param nc     number of columns in tbl
//! @param ic     see pc
//! @param pc     output is (ic,pc) the index and interpolation
//!               constant for which the following holds:
//!               z = tlow + pc*(thigh-tlow)
//!               tlow  = tbl(ir,ic  ) + pr*(tbl(ir+1,ic  )-tbl(ir,ic  ))
//!               thigh = tbl(ir,ic+1) + pr*(tbl(ir+1,ic+1)-tbl(ir,ic+1))
BRAWLER_EXPORT void invtb2(double* tbl, double z, int nr, int ir, double pr, bool revdir, int nc, int& ic, double& pc);

//! Binary search of a double, ascending-sorted array
//! This function replicates BRAWLER v7.5 srch.f
//! @param  x     array dim(*) of (double) - ascending-sorted array
//! @param  nx    size of array to search
//! @param  xint  value to match
//! @param ilow   low end of active search range.
//!               If zero, xint is outside of range [x(1),x(nx)].
//!               Otherwise, the index of the largest element of x that is
//!               less than or equal to xint.
//! @param p      Interpolation constant return value, that is,
//!               xint=x(ilow) + p*(x(ilow+1)-x(ilow))
BRAWLER_EXPORT void srch(double* x, int nx, double xint, int& ilow, double& p);

//! Checks to see if a counter has exceeded a max counter.
//! This is to be used to check on potential infinite loops
//! This function replicates BRAWLER v7.5 maxcnt.f
//! @param count   Number of times through a given loop
//! @param zprnt   If .true. write diagnostic prints
//! @param mcount  Maximum number of times through a loop allowed
//! @param subnam  Name of subroutine calling maxcnt
bool maxcnt(int& count, bool zprnt, int mcount, const std::string& subnam);

double dsxdot(double xdes, double x, double xdot, double xdotmx, double tau);

UtVec3d vorth(const UtVec3d& a, const UtVec3d& b);

UtVec3d rotz(double phi, const UtVec3d& vin);

// replicates aimpt()
BRAWLER_EXPORT bool AimPoint(const UtVec3d& xa, double sa, const UtVec3d& xt, const UtVec3d& vt, double st, UtVec3d& aimp);


BRAWLER_EXPORT void intcpt(const UtVec3d& xa,
                           double         sa,
                           const UtVec3d& xt,
                           const UtVec3d& vt,
                           double         closmn,
                           double         angint,
                           double         hdes,
                           UtVec3d&       desdir,
                           double&        sdes);

void intcpt_inner(const UtVec3d& x_atkr,
                  double         sa,
                  const UtVec3d& x_tgt,
                  const UtVec3d& vt,
                  double         closmn,
                  double         angint,
                  double         hdes,
                  double         vs,
                  const UtVec3d& xlos,
                  double         range,
                  UtVec3d&       desdir,
                  double&        sdes);

BRAWLER_EXPORT bool TryReadCommand(UtInput& aInput, std::string& aCommand);
BRAWLER_EXPORT void ReadValue(UtInput& aInput, int& aValue);
BRAWLER_EXPORT void ReadValue(UtInput& aInput, double& aValue);
} // namespace BRAWLER

#endif
