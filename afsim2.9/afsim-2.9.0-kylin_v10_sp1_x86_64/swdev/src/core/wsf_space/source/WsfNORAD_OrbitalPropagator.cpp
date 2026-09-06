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

#include "WsfNORAD_OrbitalPropagator.hpp"

#include <memory>

#include "UtCalendar.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtOrbitalPropagator.hpp"
#include "UtVec3.hpp"
#include "WsfNORAD_PropagatorInverter.hpp"

WsfNORAD_OrbitalPropagator::WsfNORAD_OrbitalPropagator()
   : WsfNonClassicalOrbitalPropagator(ut::make_unique<ut::OrbitalState>(ut::EarthWGS84(),
                                                                        ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                                                        ut::OrbitalState::ReferenceFrame::cTEME))
   , mPropagatedOrbitalState(GetInitialOrbitalState())
{
   for (double& p : params)
   {
      p = 0.0;
   }
}

WsfNORAD_OrbitalPropagator::WsfNORAD_OrbitalPropagator(const WsfNORAD_OrbitalPropagator& aSrc)
   : WsfNonClassicalOrbitalPropagator(aSrc)
   , mPropagatedOrbitalState(aSrc.mPropagatedOrbitalState)
   , tle(nullptr)
   , mSimpleFlag(aSrc.mSimpleFlag)
   , mEphem(aSrc.mEphem)
{
   for (int i = 0; i < N_SAT_PARAMS; ++i)
   {
      params[i] = aSrc.params[i];
   }
   for (int i = 0; i < 3; ++i)
   {
      mPos[i] = aSrc.mPos[i];
      mVel[i] = aSrc.mVel[i];
   }
}

WsfOrbitalPropagatorTypes::FactoryPtr WsfNORAD_OrbitalPropagator::ObjectFactory(WsfScenario& aScenario)
{
   return [&aScenario](const std::string& aTypeName) -> std::unique_ptr<UtOrbitalPropagatorBase>
   {
      if (aTypeName == "WSF_NORAD_PROPAGATOR")
      {
         return ut::make_unique<WsfNORAD_OrbitalPropagator>();
      }
      return nullptr;
   };
}

// virtual
bool WsfNORAD_OrbitalPropagator::Initialize(const UtCalendar& aInitialTime)
{
   bool ok = false;

   if (UtOrbitalPropagatorBase::Initialize(aInitialTime))
   {
      // All TLE data must be copied into the propagated orbital state.
      mPropagatedOrbitalState.Set(GetInitialOrbitalState().GetTwoLineElement());

      // Master tle data must be set in the local initial tle data:
      tle = ut::make_unique<WsfNORAD_Util::tle_t>(GetInitialOrbitalState().GetTwoLineElement());

      int is_deep = SelectEphemeris();

      if (is_deep && (mEphem == 1 || mEphem == 2))
      {
         mEphem += 2; // switch to an SDx
      }
      if (!is_deep && (mEphem == 3 || mEphem == 4))
      {
         mEphem -= 2; // switch to an SGx
      }

      // Calling of NORAD routines
      // Each NORAD routine (SGP, SGP4, SGP8, SDP4, SDP8)
      // will be called in turn with the appropriate TLE set
      switch (mEphem)
      {
      case 0:
         SGP_init();
         break;
      case 1:
         SGP4_init();
         break;
      case 2:
         SGP8_init();
         break;
      case 3:
         SDP4_init();
         break;
      case 4:
         SDP8_init();
         break;
      }

      ok = (InitializeP(aInitialTime)); // Must be called last (See method documentation).
   }
   else
   {
      ut::log::error() << "Initial orbital state does not contain sufficient information to describe the orbit.";
   }

   return ok;
}

// WsfNORAD_OrbitalPropagator does not support maneuvers to change RAAN, and thus does not need this method.
double WsfNORAD_OrbitalPropagator::GetTimeToNorthernIntersection(double   aTargetRAAN,
                                                                 double   aTargetInclination,
                                                                 unsigned aOrbitNum) const
{
   return -1.0;
}

// WsfNORAD_OrbitalPropagator does not support maneuvers to change RAAN, and thus does not need this method.
double WsfNORAD_OrbitalPropagator::GetTimeToSouthernIntersection(double   aTargetRAAN,
                                                                 double   aTargetInclination,
                                                                 unsigned aOrbitNum) const
{
   return -1.0;
}

bool WsfNORAD_OrbitalPropagator::SetInitialOrbitalState(const ut::OrbitalState& aState)
{
   if (!aState.OrbitalElementsValid() || (!aState.GetOrbitalElements().HasMeanElements()) ||
       (aState.GetReferenceFrame() != ut::OrbitalState::ReferenceFrame::cTEME))
   {
      WsfNORAD_PropagatorInverter inv(aState, ut::clone(*this));
      if (inv.SolutionFound())
      {
         WsfNonClassicalOrbitalPropagator::SetInitialOrbitalState(inv.GetOrbitalState());
      }
      return inv.SolutionFound();
   }
   else
   {
      return WsfNonClassicalOrbitalPropagator::SetInitialOrbitalState(aState);
   }
}

// virtual
void WsfNORAD_OrbitalPropagator::Propagate(const UtCalendar& aTime)
{
   double timePastEpochMin = GetTimePastEpoch() / 60.0;

   // update satellite position
   switch (mEphem)
   {
   case 0:
      SGP(timePastEpochMin);
      break;
   case 1:
      SGP4(timePastEpochMin);
      break;
   case 2:
      SGP8(timePastEpochMin);
      break;
   case 3:
      SDP4(timePastEpochMin);
      break;
   case 4:
      SDP8(timePastEpochMin);
      break;
   }
}

// override
void WsfNORAD_OrbitalPropagator::UpdateOrbitalState()
{
   SetOrbitalState(mPropagatedOrbitalState);
}

// Selects the type of ephemeris to be used (SGP*-SDP*)
// private
int WsfNORAD_OrbitalPropagator::SelectEphemeris()
{
   using namespace WsfNORAD_Util;
   double ao, xnodp, delo, a1, del1, r1, temp;
   int    rval;

   // Period > 225 minutes is deep space
   a1    = pow(xke / tle->xno, two_thirds);
   r1    = cos(tle->xincl);
   temp  = ck2 * 1.5 * (r1 * r1 * 3.0 - 1.0) * pow(1.0 - tle->eo * tle->eo, -1.5);
   del1  = temp / (a1 * a1);
   ao    = a1 * (1.0 - del1 * (1. / 3. + del1 * (del1 * 1.654320987654321 + 1.0)));
   delo  = temp / (ao * ao);
   xnodp = tle->xno / (delo + 1.0);

   // Select a deep-space/near-earth ephemeris
   // If the object makes less than 6.4 revolutions around the earth..
   if (UtMath::cTWO_PI / (xnodp * xmnpda) >= (1. / 6.4))
   {
      rval = 1;
   } // yes,  it should be a deep-space (SDPx) ephemeris
   else
   {
      rval = 0;
   } // no,  you can go with an SGPx ephemeris
   return (rval);
}

// private
void WsfNORAD_OrbitalPropagator::PostPropagate()
{
   // Convert position and velocity to mks
   UtVec3d location;
   UtVec3d::Set(location.GetData(), mPos);
   ;
   location.Multiply(1000.0); // km to m
   UtVec3d velocity;
   UtVec3d::Set(velocity.GetData(), mVel);
   velocity.Multiply(1000.0 / 60.0); // km/min to m/sec
   mPropagatedOrbitalState.Set(GetCurrentTime(), ut::OrbitalState::Vector(location, velocity));
   UtOrbitalPropagatorBase::PostPropagate();

   PrintDebug(GetTimePastEpoch());
}

// ***************************************************************
// SG*, SD* function definitions.
// ***************************************************************

#define ao params[0]
#define qo params[1]
#define xlo params[2]
#define d1o params[3]
#define d2o params[4]
#define d3o params[5]
#define d4o params[6]
#define omgdt params[7]
#define xnodot params[8]
#define c5 params[9]
#define c6 params[10]

// ***************************************************************
// SGP function definitions.
// ***************************************************************
// void SGP_init( double *params, const tle_t *tle)
void WsfNORAD_OrbitalPropagator::SGP_init()
{
   using namespace WsfNORAD_Util;
   double c1, c2, c3, c4, r1, cosio, sinio, a1, d1, po, po2no;

   c1     = ck2 * 1.5;
   c2     = ck2 / 4.;
   c3     = ck2 / 2.;
   r1     = ae;
   c4     = xj3 * (r1 * (r1 * r1)) / (ck2 * 4.);
   cosio  = cos(tle->xincl);
   sinio  = sin(tle->xincl);
   a1     = pow(xke / tle->xno, two_thirds);
   d1     = c1 / a1 / a1 * (cosio * 3. * cosio - 1.) / pow(1. - tle->eo * tle->eo, 1.5);
   ao     = a1 * (1. - d1 * .33333333333333331 - d1 * d1 - d1 * 1.654320987654321 * d1 * d1);
   po     = ao * (1. - tle->eo * tle->eo);
   qo     = ao * (1. - tle->eo);
   xlo    = tle->xmo + tle->omegao + tle->xnodeo;
   d1o    = c3 * sinio * sinio;
   d2o    = c2 * (cosio * 7. * cosio - 1.);
   d3o    = c1 * cosio;
   d4o    = d3o * sinio;
   po2no  = tle->xno / (po * po);
   omgdt  = c1 * po2no * (cosio * 5. * cosio - 1.);
   xnodot = d3o * -2. * po2no;
   c5     = c4 * .5 * sinio * (cosio * 5. + 3.) / (cosio + 1.);
   c6     = c4 * sinio;
}


// void WsfNORAD_OrbitalPropagator::SGP( const double tsince, const tle_t *tle, const double *params,
//           double *pos, double *vel)
void WsfNORAD_OrbitalPropagator::SGP(double tsince)
{
   using namespace WsfNORAD_Util;
   double temp, rdot, cosu, sinu, cos2u, sin2u, a, e, p, rr, u, ecose, esine, omgas, cosik, xinck, sinik, axnsl, aynsl,
      sinuk, rvdot, cosuk, coseo1, sineo1, pl, rk, uk, xl, su, ux, uy, uz, vx, vy, vz, pl2, xnodek, cosnok, xnodes, el2,
      eo1, r1, sinnok, xls, xmx, xmy, tem5;

   int i;

   // Update for secular gravity and atmospheric drag
   a = tle->xno + (tle->xndt2o * 2. + tle->xndd6o * 3. * tsince) * tsince;
   a = ao * pow(tle->xno / a, two_thirds);
   e = e6a;
   if (a > qo)
   {
      e = 1. - qo / a;
   }
   p      = a * (1. - e * e);
   xnodes = tle->xnodeo + xnodot * tsince;
   omgas  = tle->omegao + omgdt * tsince;
   r1     = xlo + (tle->xno + omgdt + xnodot + (tle->xndt2o + tle->xndd6o * tsince) * tsince) * tsince;
   xls    = FMod2p(r1);

   // Long period periodics
   axnsl = e * cos(omgas);
   aynsl = e * sin(omgas) - c6 / p;
   r1    = xls - c5 / p * axnsl;
   xl    = FMod2p(r1);

   // Solve Kepler's equation
   r1   = xl - xnodes;
   u    = FMod2p(r1);
   eo1  = u;
   tem5 = 1.;

   i = 0;
   do
   {
      sineo1 = sin(eo1);
      coseo1 = cos(eo1);
      if (fabs(tem5) < e6a)
      {
         break;
      }
      tem5        = 1. - coseo1 * axnsl - sineo1 * aynsl;
      tem5        = (u - aynsl * coseo1 + axnsl * sineo1 - eo1) / tem5;
      double tem2 = fabs(tem5);
      if (tem2 > 1.)
      {
         tem5 = tem2 / tem5;
      }
      eo1 += tem5;
   } while (i++ < 10);

   // Short period preliminary quantities
   ecose = axnsl * coseo1 + aynsl * sineo1;
   esine = axnsl * sineo1 - aynsl * coseo1;
   el2   = axnsl * axnsl + aynsl * aynsl;
   pl    = a * (1. - el2);
   pl2   = pl * pl;
   rr    = a * (1. - ecose);
   rdot  = xke * sqrt(a) / rr * esine;
   rvdot = xke * sqrt(pl) / rr;
   temp  = esine / (sqrt(1. - el2) + 1.);
   sinu  = a / rr * (sineo1 - aynsl - axnsl * temp);
   cosu  = a / rr * (coseo1 - axnsl + aynsl * temp);
   su    = atan2(sinu, cosu);

   // Update for short periodics
   sin2u  = (cosu + cosu) * sinu;
   cos2u  = 1. - 2. * sinu * sinu;
   rk     = rr + d1o / pl * cos2u;
   uk     = su - d2o / pl2 * sin2u;
   xnodek = xnodes + d3o * sin2u / pl2;
   xinck  = tle->xincl + d4o / pl2 * cos2u;

   // Orientation vectors
   sinuk  = sin(uk);
   cosuk  = cos(uk);
   sinnok = sin(xnodek);
   cosnok = cos(xnodek);
   sinik  = sin(xinck);
   cosik  = cos(xinck);
   xmx    = -sinnok * cosik;
   xmy    = cosnok * cosik;
   ux     = xmx * sinuk + cosnok * cosuk;
   uy     = xmy * sinuk + sinnok * cosuk;
   uz     = sinik * sinuk;
   vx     = xmx * cosuk - cosnok * sinuk;
   vy     = xmy * cosuk - sinnok * sinuk;
   vz     = sinik * cosuk;

   // Position and velocity
   mPos[0] = rk * ux * xkmper;
   mPos[1] = rk * uy * xkmper;
   mPos[2] = rk * uz * xkmper;
   // mjm always calculate velocity  if( vel)
   //      {
   mVel[0] = (rdot * ux + rvdot * vx) * xkmper;
   mVel[1] = (rdot * uy + rvdot * vy) * xkmper;
   mVel[2] = (rdot * uz + rvdot * vz) * xkmper;
   //     }

} // SGP

#undef ao
#undef qo
#undef xlo
#undef d1o
#undef d2o
#undef d3o
#undef d4o
#undef omgdt
#undef xnodot
#undef c5
#undef c6

// ***************************************************************
// SGP4 function definitions.
// ***************************************************************

#define x3thm1 params[0]
#define x1mth2 params[1]
#define c1 params[2]
#define c4 params[3]
#define xnodcf params[4]
#define t2cof params[5]
#define xlcof params[6]
#define aycof params[7]
#define x7thm1 params[8]
#define p_aodp params[9]
#define p_cosio params[10]
#define p_sinio params[11]
#define p_omgdot params[12]
#define p_xmdot params[13]
#define p_xnodot params[14]
#define p_xnodp params[15]
#define c5 params[16]
#define d2 params[17]
#define d3 params[18]
#define d4 params[19]
#define delmo params[20]
#define p_eta params[21]
#define omgcof params[22]
#define sinmo params[23]
#define t3cof params[24]
#define t4cof params[25]
#define t5cof params[26]
#define xmcof params[27]
#define MINIMAL_E 1.e-9

// void SGP4_init( double *params, const tle_t *tle)
void WsfNORAD_OrbitalPropagator::SGP4_init()
{
   using namespace WsfNORAD_Util;
   deep_arg_t deep_arg;
   init_t     init;
   double     eeta, etasq;

   sxpx_common_init(params, tle.get(), &init, &deep_arg);
   p_aodp   = deep_arg.aodp;
   p_cosio  = deep_arg.cosio;
   p_sinio  = deep_arg.sinio;
   p_omgdot = deep_arg.omgdot;
   p_xmdot  = deep_arg.xmdot;
   p_xnodot = deep_arg.xnodot;
   p_xnodp  = deep_arg.xnodp;
   p_eta    = deep_arg.aodp * tle->eo * init.tsi;
   // p_eta = init.eta;

   eeta = tle->eo * p_eta;
   // For perigee less than 220 kilometers, the "simple" flag is set
   // and the equations are truncated to linear variation in sqrt a
   // and quadratic variation in mean anomaly.  Also, the c3 term,
   // the delta omega term, and the delta m term are dropped.
   if ((p_aodp * (1 - tle->eo) / ae) < (220 / xkmper + ae))
   {
      mSimpleFlag = true;
   }
   else
   {
      const double c1sq = c1 * c1;
      double       temp, c3;

      mSimpleFlag = false;
      delmo       = 1. + p_eta * cos(tle->xmo);
      delmo *= delmo * delmo;
      d2    = 4 * p_aodp * init.tsi * c1sq;
      temp  = d2 * init.tsi * c1 / 3;
      d3    = (17 * p_aodp + init.s4) * temp;
      d4    = 0.5 * temp * p_aodp * init.tsi * (221 * p_aodp + 31 * init.s4) * c1;
      t3cof = d2 + 2 * c1sq;
      t4cof = 0.25 * (3 * d3 + c1 * (12 * d2 + 10 * c1sq));
      t5cof = 0.2 * (3 * d4 + 12 * c1 * d3 + 6 * d2 * d2 + 15 * c1sq * (2 * d2 + c1sq));
      sinmo = sin(tle->xmo);
      c3    = init.coef * init.tsi * init.a3ovk2 * p_xnodp * ae * p_sinio;
      if (tle->eo < MINIMAL_E)
      {
         eeta = MINIMAL_E * MINIMAL_E * p_aodp * init.tsi;
         c3 /= MINIMAL_E;
      }
      else
      {
         c3 /= tle->eo;
      }
      xmcof = -two_thirds * init.coef * tle->bstar * ae / eeta;
      // 17 Aug 2002: added a check for zero eccentricity:
      omgcof = tle->bstar * c3 * cos(tle->omegao);
   } // End of if (isFlagClear(SIMPLE_FLAG))
   etasq = p_eta * p_eta;
   c5    = 2 * init.coef1 * p_aodp * deep_arg.betao2 * (1 + 2.75 * (etasq + eeta) + eeta * etasq);
} // End of SGP4() initialization

// void WsfNORAD_OrbitalPropagator::SGP4( const double tsince, const tle_t *tle, const double *params,
//                                                     double *pos, double *vel)
void WsfNORAD_OrbitalPropagator::SGP4(double tsince)
{
   double a, e, omega, omgadf, tempa, tempe, templ, tsq, xl, xmdf, xmp, xnoddf, xnode;

   // Update for secular gravity and atmospheric drag.
   xmdf   = tle->xmo + p_xmdot * tsince;
   omgadf = tle->omegao + p_omgdot * tsince;
   xnoddf = tle->xnodeo + p_xnodot * tsince;
   omega  = omgadf;
   xmp    = xmdf;
   tsq    = tsince * tsince;
   xnode  = xnoddf + xnodcf * tsq;
   tempa  = 1 - c1 * tsince;
   tempe  = tle->bstar * c4 * tsince;
   templ  = t2cof * tsq;
   if (!mSimpleFlag)
   {
      const double delomg = omgcof * tsince;
      double       delm   = 1. + p_eta * cos(xmdf);
      double       tcube, tfour;

      delm        = xmcof * (delm * delm * delm - delmo);
      double temp = delomg + delm;
      xmp         = xmdf + temp;
      omega       = omgadf - temp;
      tcube       = tsq * tsince;
      tfour       = tsince * tcube;
      tempa       = tempa - d2 * tsq - d3 * tcube - d4 * tfour;
      tempe       = tempe + tle->bstar * c5 * (sin(xmp) - sinmo);
      templ       = templ + t3cof * tcube + tfour * (t4cof + tsince * t5cof);
   }; // End of if (isFlagClear(SIMPLE_FLAG))

   a  = p_aodp * tempa * tempa;
   e  = tle->eo - tempe;
   xl = xmp + omega + xnode + p_xnodp * templ;
   WsfNORAD_Util::sxpx_posn_vel(xnode, a, e, params, p_cosio, p_sinio, tle->xincl, omega, xl, mPos, mVel);
} // SGP4

#undef x3thm1
#undef x1mth2
#undef c1
#undef c4
#undef xnodcf
#undef t2cof
#undef xlcof
#undef aycof
#undef x7thm1
#undef p_aodp
#undef p_cosio
#undef p_sinio
#undef p_omgdot
#undef p_xmdot
#undef p_xnodot
#undef p_xnodp
#undef c5
#undef d2
#undef d3
#undef d4
#undef delmo
#undef p_eta
#undef omgcof
#undef sinmo
#undef t3cof
#undef t4cof
#undef t5cof
#undef xmcof
#undef MINIMAL_E

// ***************************************************************
// SDP4 function definitions.
// ***************************************************************

#define x3thm1 params[0]
#define x1mth2 params[1]
#define c1 params[2]
#define c4 params[3]
#define xnodcf params[4]
#define t2cof params[5]
#define xlcof params[6]
#define aycof params[7]
#define x7thm1 params[8]
#define deep_arg ((deep_arg_t*)(params + 9))

// void SDP4_init( double *params, const tle_t *tle)
void WsfNORAD_OrbitalPropagator::SDP4_init()
{
   using namespace WsfNORAD_Util;
   init_t init;

   sxpx_common_init(params, tle.get(), &init, deep_arg);
   deep_arg->sing = sin(tle->omegao);
   deep_arg->cosg = cos(tle->omegao);

   // initialize Deep()
   Deep_dpinit(tle.get(), deep_arg);
   //#ifdef RETAIN_PERTURBATION_VALUES_AT_EPOCH
   //   // initialize lunisolar perturbations:
   //   deep_arg->t = 0.;                            // added 30 Dec 2003
   //   deep_arg->solar_lunar_init_flag = 1;
   //   Deep_dpper( deep_arg);
   //   deep_arg->solar_lunar_init_flag = 0;
   //#endif
} // End of SDP4() initialization

// void SDP4( const double tsince, const tle_t *tle, const double *params,
//                                          double *pos, double *vel)
void WsfNORAD_OrbitalPropagator::SDP4(double tsince)
{
   using namespace WsfNORAD_Util;
   double a, tempa, tempe, templ, tsq, xl, xmam, xmdf, xnoddf;

   // Update for secular gravity and atmospheric drag
   xmdf             = tle->xmo + deep_arg->xmdot * tsince;
   deep_arg->omgadf = tle->omegao + deep_arg->omgdot * tsince;
   xnoddf           = tle->xnodeo + deep_arg->xnodot * tsince;
   tsq              = tsince * tsince;
   deep_arg->xnode  = xnoddf + xnodcf * tsq;
   tempa            = 1 - c1 * tsince;
   tempe            = tle->bstar * c4 * tsince;
   templ            = t2cof * tsq;
   deep_arg->xn     = deep_arg->xnodp;

   // Update for deep-space secular effects
   deep_arg->xll = xmdf;
   deep_arg->t   = tsince;

   Deep_dpsec(tle.get(), deep_arg);

   xmdf         = deep_arg->xll;
   a            = pow(xke / deep_arg->xn, two_thirds) * tempa * tempa;
   deep_arg->em = deep_arg->em - tempe;
   xmam         = xmdf + deep_arg->xnodp * templ;

   // Update for deep-space periodic effects
   deep_arg->xll = xmam;

   Deep_dpper(deep_arg);

   xmam = deep_arg->xll;
   xl   = xmam + deep_arg->omgadf + deep_arg->xnode;
   sxpx_posn_vel(deep_arg->xnode,
                 a,
                 deep_arg->em,
                 params,
                 deep_arg->cosio,
                 deep_arg->sinio,
                 deep_arg->xinc,
                 deep_arg->omgadf,
                 xl,
                 mPos,
                 mVel);
} // SDP4

#undef x3thm1
#undef x1mth2
#undef c1
#undef c4
#undef xnodcf
#undef t2cof
#undef xlcof
#undef aycof
#undef x7thm1
#undef deep_arg

// ***************************************************************
// SGP8 function definitions.
// ***************************************************************

#define a3cof (-(xj3) / ck2 * (ae * ae * ae))
#define cosi params[1]
#define cosio2 params[2]
#define ed params[3]
#define edot params[4]
#define gamma params[5]
#define omgdt params[6]
#define ovgpp params[7]
#define pp params[8]
#define qq params[9]
#define sini params[10]
#define sinio2 params[11]
#define theta2 params[12]
#define tthmun params[13]
#define unm5th params[14]
#define unmth2 params[15]
#define xgdt1 params[16]
#define xhdt1 params[17]
#define xlldot params[18]
#define xmdt1 params[19]
#define xnd params[20]
#define xndt params[21]
#define xnodot params[22]
#define xnodp params[23]

// void SGP8_init( double *params, const tle_t *tle)
void WsfNORAD_OrbitalPropagator::SGP8_init()
{
   using namespace WsfNORAD_Util;
   // Recover original mean motion (xnodp) and semimajor
   // axis (aodp) from input elements. Calculate ballistic
   // coefficient (b term) from input b* drag term
   const double a1               = pow(xke / tle->xno, two_thirds);
   const double eosq             = tle->eo * tle->eo;
   const double betao2           = 1. - eosq;
   const double betao            = sqrt(betao2);
   const double b                = tle->bstar * 2. / rho;
   const double sing             = sin(tle->omegao);
   const double cosg             = cos(tle->omegao);
   const double cos2g            = cosg * cosg * 2. - 1.;
   const double half_inclination = tle->xincl * .5;
   double alpha2, ao, aodp, b1, b2, b3, c0, c1, c4, c5, d1, d2, d3, d4, d5, del1, delo, eeta, eta, eta2, pardt1, pardt2,
      pardt4, po, pom2, psim2, r1, theta4, tsi, xndtn;

   cosi   = cos(tle->xincl);
   theta2 = cosi * cosi;
   tthmun = theta2 * 3. - 1.;
   del1   = ck2 * 1.5 * tthmun / (a1 * a1 * betao * betao2);
   ao     = a1 * (1. - del1 * (two_thirds * .5 + del1 * (del1 * 1.654320987654321 + 1.)));
   delo   = ck2 * 1.5 * tthmun / (ao * ao * betao * betao2);
   aodp   = ao / (1. - delo);
   xnodp  = tle->xno / (delo + 1.);

   // Initialization
   po     = aodp * betao2;
   pom2   = 1. / (po * po);
   sini   = sin(tle->xincl);
   sinio2 = sin(half_inclination);
   cosio2 = cos(half_inclination);
   r1     = theta2;
   theta4 = r1 * r1;
   unm5th = 1. - theta2 * 5.;
   unmth2 = 1. - theta2;
   // TODO-REDUNDANT-ASSIGNMENT r1 = ae;
   pardt1 = ck2 * 3. * pom2 * xnodp;
   pardt2 = pardt1 * ck2 * pom2;
   pardt4 = ck4 * 1.25 * pom2 * pom2 * xnodp;
   xmdt1  = pardt1 * .5 * betao * tthmun;
   xgdt1  = pardt1 * -.5 * unm5th;
   xhdt1  = -pardt1 * cosi;
   xlldot = xnodp + xmdt1 + pardt2 * .0625 * betao * (13. - theta2 * 78. + theta4 * 137.);
   omgdt  = xgdt1 + pardt2 * .0625 * (7. - theta2 * 114. + theta4 * 395.) + pardt4 * (3. - theta2 * 36. + theta4 * 49.);
   xnodot = xhdt1 + (pardt2 * .5 * (4. - theta2 * 19.) + pardt4 * 2. * (3. - theta2 * 7.)) * cosi;
   tsi    = 1. / (po - s);
   eta    = tle->eo * s * tsi;
   r1     = eta;
   eta2   = r1 * r1;
   psim2  = (r1 = 1. / (1. - eta2), fabs(r1));
   alpha2 = eosq + 1.;
   eeta   = tle->eo * eta;
   // TODO-REDUNDANT-ASSIGNMENT r1 = cosg;
   d5 = tsi * psim2;
   d1 = d5 / po;
   d2 = eta2 * (eta2 * 4.5 + 36.) + 12.;
   d3 = eta2 * (eta2 * 2.5 + 15.);
   d4 = eta * (eta2 * 3.75 + 5.);
   b1 = ck2 * tthmun;
   b2 = -ck2 * unmth2;
   b3 = a3cof * sini;
   r1 = tsi, r1 *= r1;
   c0 = b * .5 * rho * qoms2t * xnodp * aodp * (r1 * r1) * pow(psim2, 3.5) / sqrt(alpha2);
   r1 = alpha2;
   c1 = xnodp * 1.5 * (r1 * r1) * c0;
   c4 = d1 * d3 * b2;
   c5 = d5 * d4 * b3;
   xndt =
      c1 * (eta2 * (eosq * 34. + 3.) + 2. + eeta * 5. * (eta2 + 4.) + eosq * 8.5 + d1 * d2 * b1 + c4 * cos2g + c5 * sing);
   xndtn = xndt / xnodp;

   // If drag is very small, the isimp flag is set and the
   // equations are truncated to linear variation in mean
   // motion and quadratic variation in mean anomaly
   r1 = xndtn * xmnpda;
   if (fabs(r1) > .00216)
   {
      const double d6     = eta * (eta2 * 22.5 + 30.);
      const double d7     = eta * (eta2 * 12.5 + 5.);
      const double d8     = eta2 * (eta2 + 6.75) + 1.;
      const double d9     = eta * (eosq * 68. + 6.) + tle->eo * (eta2 * 15. + 20.);
      const double d10    = eta * 5. * (eta2 + 4.) + tle->eo * (eta2 * 68. + 17.);
      const double d11    = eta * (eta2 * 18. + 72.);
      const double d12    = eta * (eta2 * 10. + 30.);
      const double d13    = eta2 * 11.25 + 5.;
      const double d20    = two_thirds * .5 * xndtn;
      const double c8     = d1 * d7 * b2;
      const double c9     = d5 * d8 * b3;
      const double tsdtts = aodp * 2. * tsi * (d20 * betao2 + tle->eo * edot);
      const double sin2g  = sing * 2. * cosg;
      double       d1dt, d2dt, d3dt, d4dt, d5dt, temp;
      double       d14, d15, d16, d17, d18, d19, d23, d25, aldtal, psdtps;
      double       c4dt, c5dt, c0dtc0, c1dtc1, rr2;
      double       d1ddt, etddt, tmnddt, tsddts, xnddt, xntrdt;

      mSimpleFlag = false;
      edot        = -c0 * (eta * (eta2 + 4. + eosq * (eta2 * 7. + 15.5)) + tle->eo * (eta2 * 15. + 5.) + d1 * d6 * b1 +
                    c8 * cos2g + c9 * sing);
      aldtal      = tle->eo * edot / alpha2;
      double etdt = (edot + tle->eo * tsdtts) * tsi * s;
      psdtps      = -eta * etdt * psim2;
      c0dtc0      = d20 + tsdtts * 4. - aldtal - psdtps * 7.;
      c1dtc1      = xndtn + aldtal * 4. + c0dtc0;
      d14         = tsdtts - psdtps * 2.;
      d15         = (d20 + tle->eo * edot / betao2) * 2.;
      d1dt        = d1 * (d14 + d15);
      d2dt        = etdt * d11;
      d3dt        = etdt * d12;
      d4dt        = etdt * d13;
      d5dt        = d5 * d14;
      c4dt        = b2 * (d1dt * d3 + d1 * d3dt);
      c5dt        = b3 * (d5dt * d4 + d5 * d4dt);
      d16         = d9 * etdt + d10 * edot + b1 * (d1dt * d2 + d1 * d2dt) + c4dt * cos2g + c5dt * sing +
            xgdt1 * (c5 * cosg - c4 * 2. * sin2g);
      xnddt = c1dtc1 * xndt + c1 * d16;
      double eddot =
         c0dtc0 * edot -
         c0 * ((eta2 * 3. + 4. + eeta * 30. + eosq * (eta2 * 21. + 15.5)) * etdt +
               (eta2 * 15. + 5. + eeta * (eta2 * 14. + 31.)) * edot + b1 * (d1dt * d6 + d1 * etdt * (eta2 * 67.5 + 30.)) +
               b2 * (d1dt * d7 + d1 * etdt * (eta2 * 37.5 + 5.)) * cos2g +
               b3 * (d5dt * d8 + d5 * etdt * eta * (eta2 * 4. + 13.5)) * sing + xgdt1 * (c9 * cosg - c8 * 2. * sin2g));
      r1     = edot;
      d25    = r1 * r1;
      r1     = xndtn;
      d17    = xnddt / xnodp - r1 * r1;
      tsddts = tsdtts * 2. * (tsdtts - d20) +
               aodp * tsi * (two_thirds * betao2 * d17 - d20 * 4. * tle->eo * edot + (d25 + tle->eo * eddot) * 2.);
      etddt = (eddot + edot * 2. * tsdtts) * tsi * s + tsddts * eta;
      r1    = tsdtts;
      d18   = tsddts - r1 * r1;
      r1    = psdtps;
      rr2   = psdtps;
      d19   = -(r1 * r1) / eta2 - eta * etddt * psim2 - rr2 * rr2;
      d23   = etdt * etdt;
      d1ddt = d1dt * (d14 + d15) +
              d1 * (d18 - d19 * 2. + two_thirds * d17 + (alpha2 * d25 / betao2 + tle->eo * eddot) * 2. / betao2);
      r1 = aldtal;
      xntrdt =
         xndt * (two_thirds * 2. * d17 + (d25 + tle->eo * eddot) * 3. / alpha2 - r1 * r1 * 6. + d18 * 4. - d19 * 7.) +
         c1dtc1 * xnddt +
         c1 * (c1dtc1 * d16 + d9 * etddt + d10 * eddot + d23 * (eeta * 30. + 6. + eosq * 68.) +
               etdt * edot * (eta2 * 30. + 40. + eeta * 272.) + d25 * (eta2 * 68. + 17.) +
               b1 * (d1ddt * d2 + d1dt * 2. * d2dt + d1 * (etddt * d11 + d23 * (eta2 * 54. + 72.))) +
               b2 * (d1ddt * d3 + d1dt * 2. * d3dt + d1 * (etddt * d12 + d23 * (eta2 * 30. + 30.))) * cos2g +
               b3 * ((d5dt * d14 + d5 * (d18 - d19 * 2.)) * d4 + d4dt * 2. * d5dt + d5 * (etddt * d13 + eta * 22.5 * d23)) *
                  sing +
               xgdt1 * ((d20 * 7. + tle->eo * 4. * edot / betao2) * (c5 * cosg - c4 * 2. * sin2g) +
                        (c5dt * 2. * cosg - c4dt * 4. * sin2g - xgdt1 * (c5 * sing + c4 * 4. * cos2g))));
      tmnddt = xnddt * 1e9;
      r1     = tmnddt;
      temp   = r1 * r1 - xndt * 1e18 * xntrdt;
      r1     = tmnddt;
      pp     = (temp + r1 * r1) / temp;
      gamma  = -xntrdt / (xnddt * (pp - 2.));
      xnd    = xndt / (pp * gamma);
      qq     = 1. - eddot / (edot * gamma);
      ed     = edot / (qq * gamma);
      ovgpp  = 1. / (gamma * (pp + 1.));
   }
   else
   {
      mSimpleFlag = true;
      edot        = -two_thirds * xndtn * (1. - tle->eo);
   } // End of if (fabs(r1) > .00216)
} // End of SGP8() initialization

// void SGP8( const double tsince, const tle_t *tle, const double *params,
//                                           double *pos, double *vel)
void WsfNORAD_OrbitalPropagator::SGP8(double tsince)
{
   using namespace WsfNORAD_Util;
   int    i;
   double am, aovr, axnm, aynm, beta, beta2m, cose, cosos, cs2f2g, csf, csfg, cslamb, di, diwc, dr, ecosf, em, fm, g1,
      g10, g13, g14, g2, g3, g4, g5, omgasm, pm, r1, rdot, rm, rr, rvdot, sine, sinos, sn2f2g, snf, snfg, sni2du,
      snlamb, temp, ux, uy, uz, vx, vy, vz, xlamb, xmam, xn, xnodes, y4, y5, z1, z7, zc2, zc5;

   // Update for secular gravity and atmospheric drag
   r1     = tle->xmo + xlldot * tsince;
   xmam   = FMod2p(r1);
   omgasm = tle->omegao + omgdt * tsince;
   xnodes = tle->xnodeo + xnodot * tsince;
   if (!mSimpleFlag)
   {
      double temp1;

      temp  = 1. - gamma * tsince;
      temp1 = pow(temp, pp);
      xn    = xnodp + xnd * (1. - temp1);
      em    = tle->eo + ed * (1. - pow(temp, qq));
      z1    = xnd * (tsince + ovgpp * (temp * temp1 - 1.));
   }
   else
   {
      xn = xnodp + xndt * tsince;
      em = tle->eo + edot * tsince;
      z1 = xndt * .5 * tsince * tsince;
   } // if(isFlagClear(SIMPLE_FLAG))

   z7   = two_thirds * 3.5 * z1 / xnodp;
   r1   = xmam + z1 + z7 * xmdt1;
   xmam = FMod2p(r1);
   omgasm += z7 * xgdt1;
   xnodes += z7 * xhdt1;

   // Solve Kepler's equation
   zc2 = xmam + em * sin(xmam) * (em * cos(xmam) + 1.);

   i = 0;
   do
   {
      sine        = sin(zc2);
      cose        = cos(zc2);
      zc5         = 1. / (1. - em * cose);
      double cape = (xmam + em * sine - zc2) * zc5 + zc2;
      r1          = cape - zc2;
      if (fabs(r1) <= e6a)
      {
         break;
      }
      zc2 = cape;
   } while (i++ < 10);

   // Short period preliminary quantities
   am     = pow(xke / xn, two_thirds);
   beta2m = 1. - em * em;
   sinos  = sin(omgasm);
   cosos  = cos(omgasm);
   axnm   = em * cosos;
   aynm   = em * sinos;
   pm     = am * beta2m;
   g1     = 1. / pm;
   g2     = ck2 * .5 * g1;
   g3     = g2 * g1;
   beta   = sqrt(beta2m);
   g4     = a3cof * .25 * sini;
   g5     = a3cof * .25 * g1;
   snf    = beta * sine * zc5;
   csf    = (cose - em) * zc5;
   fm     = atan2(snf, csf);
   if (fm < 0.)
   //     fm += pi + pi;
   {
      fm += UtMath::cPI + UtMath::cPI;
   }
   snfg   = snf * cosos + csf * sinos;
   csfg   = csf * cosos - snf * sinos;
   sn2f2g = snfg * 2. * csfg;
   r1     = csfg;
   cs2f2g = r1 * r1 * 2. - 1.;
   ecosf  = em * csf;
   g10    = fm - xmam + em * snf;
   rm     = pm / (ecosf + 1.);
   aovr   = am / rm;
   g13    = xn * aovr;
   g14    = -g13 * aovr;
   dr     = g2 * (unmth2 * cs2f2g - tthmun * 3.) - g4 * snfg;
   diwc   = g3 * 3. * sini * cs2f2g - g5 * aynm;
   di     = diwc * cosi;

   // Update for short period periodics
   sni2du = sinio2 * (g3 * ((1. - theta2 * 7.) * .5 * sn2f2g - unm5th * 3. * g10) - g5 * sini * csfg * (ecosf + 2.)) -
            g5 * .5f * theta2 * axnm / cosio2;
   xlamb = fm + omgasm + xnodes + g3 * ((cosi * 6. + 1. - theta2 * 7.) * .5 * sn2f2g - (unm5th + cosi * 2.) * 3. * g10) +
           g5 * sini * (cosi * axnm / (cosi + 1.) - (ecosf + 2.) * csfg);
   y4    = sinio2 * snfg + csfg * sni2du + snfg * .5 * cosio2 * di;
   y5    = sinio2 * csfg - snfg * sni2du + csfg * .5 * cosio2 * di;
   rr    = rm + dr;
   rdot  = xn * am * em * snf / beta + g14 * (g2 * 2. * unmth2 * sn2f2g + g4 * csfg);
   r1    = am;
   rvdot = xn * (r1 * r1) * beta / rm + g14 * dr + am * g13 * sini * diwc;

   // Orientation vectors
   snlamb = sin(xlamb);
   cslamb = cos(xlamb);
   temp   = (y5 * snlamb - y4 * cslamb) * 2.;
   ux     = y4 * temp + cslamb;
   vx     = y5 * temp - snlamb;
   temp   = (y5 * cslamb + y4 * snlamb) * 2.;
   uy     = -y4 * temp + snlamb;
   vy     = -y5 * temp + cslamb;
   temp   = sqrt(1. - y4 * y4 - y5 * y5) * 2.;
   uz     = y4 * temp;
   vz     = y5 * temp;

   // Position and velocity
   mPos[0] = rr * ux * xkmper;
   mPos[1] = rr * uy * xkmper;
   mPos[2] = rr * uz * xkmper;
   // mjm always calculate velocity  if( vel)
   //      {
   mVel[0] = (rdot * ux + rvdot * vx) * xkmper;
   mVel[1] = (rdot * uy + rvdot * vy) * xkmper;
   mVel[2] = (rdot * uz + rvdot * vz) * xkmper;
   //     }

} // SGP8

#undef a3cof
#undef cosi
#undef cosio2
#undef ed
#undef edot
#undef gamma
#undef omgdt
#undef ovgpp
#undef pp
#undef qq
#undef sini
#undef sinio2
#undef theta2
#undef tthmun
#undef unm5th
#undef unmth2
#undef xgdt1
#undef xhdt1
#undef xlldot
#undef xmdt1
#undef xnd
#undef xndt
#undef xnodot
#undef xnodp


// ***************************************************************
// SDP8 function definitions.
// ***************************************************************

#define tthmun params[0]
#define sinio2 params[1]
#define cosio2 params[2]
#define unm5th params[3]
#define unmth2 params[4]
#define a3cof params[5]
#define xmdt1 params[6]
#define xgdt1 params[7]
#define xhdt1 params[8]
#define xndt params[9]
#define edot params[10]
#define deep_arg ((deep_arg_t*)(params + 11))

// void SDP8_init( double *params, const tle_t *tle)
void WsfNORAD_OrbitalPropagator::SDP8_init()
{
   using namespace WsfNORAD_Util;
   double a1, alpha2, ao, b, b1, b2, b3, c0, c1, c4, c5, cos2g, d1, d2, d3, d4, d5, del1, delo, eeta, eta, eta2, pardt1,
      pardt2, pardt4, po, pom2, psim2, r1, temp, theta4, tsi, xndtn;

   // Recover original mean motion (xnodp) and semimajor axis (aodp)
   // from input elements. Calculate ballistic coefficient
   // (b term) from input b* drag term
   a1               = pow(xke / tle->xno, two_thirds);
   deep_arg->cosio  = cos(tle->xincl);
   deep_arg->theta2 = deep_arg->cosio * deep_arg->cosio;
   tthmun           = deep_arg->theta2 * 3. - 1.;
   deep_arg->eosq   = tle->eo * tle->eo;
   deep_arg->betao2 = 1. - deep_arg->eosq;
   deep_arg->betao  = sqrt(deep_arg->betao2);
   del1             = ck2 * 1.5 * tthmun / (a1 * a1 * deep_arg->betao * deep_arg->betao2);
   ao               = a1 * (1. - del1 * (two_thirds * .5 + del1 * (del1 * 1.654320987654321 + 1.)));
   delo             = ck2 * 1.5 * tthmun / (ao * ao * deep_arg->betao * deep_arg->betao2);
   deep_arg->aodp   = ao / (1. - delo);
   deep_arg->xnodp  = tle->xno / (delo + 1.);
   b                = tle->bstar * 2. / rho;

   // Initialization
   po              = deep_arg->aodp * deep_arg->betao2;
   pom2            = 1. / (po * po);
   deep_arg->sinio = sin(tle->xincl);
   deep_arg->sing  = sin(tle->omegao);
   deep_arg->cosg  = cos(tle->omegao);
   temp            = tle->xincl * .5;
   sinio2          = sin(temp);
   cosio2          = cos(temp);
   r1              = deep_arg->theta2;
   theta4          = r1 * r1;
   unm5th          = 1. - deep_arg->theta2 * 5.;
   unmth2          = 1. - deep_arg->theta2;
   r1              = ae;
   a3cof           = -xj3 / ck2 * (r1 * (r1 * r1));
   pardt1          = ck2 * 3. * pom2 * deep_arg->xnodp;
   pardt2          = pardt1 * ck2 * pom2;
   pardt4          = ck4 * 1.25 * pom2 * pom2 * deep_arg->xnodp;
   xmdt1           = pardt1 * .5 * deep_arg->betao * tthmun;
   xgdt1           = pardt1 * -.5 * unm5th;
   xhdt1           = -pardt1 * deep_arg->cosio;
   deep_arg->xmdot =
      deep_arg->xnodp + xmdt1 + pardt2 * .0625 * deep_arg->betao * (13. - deep_arg->theta2 * 78. + theta4 * 137.);
   deep_arg->omgdot = xgdt1 + pardt2 * .0625 * (7. - deep_arg->theta2 * 114. + theta4 * 395.) +
                      pardt4 * (3. - deep_arg->theta2 * 36. + theta4 * 49.);
   deep_arg->xnodot = xhdt1 + (pardt2 * .5 * (4. - deep_arg->theta2 * 19.) + pardt4 * 2. * (3. - deep_arg->theta2 * 7.)) *
                                 deep_arg->cosio;
   tsi    = 1. / (po - s);
   eta    = tle->eo * s * tsi;
   r1     = eta;
   eta2   = r1 * r1;
   psim2  = (r1 = 1. / (1. - eta2), fabs(r1));
   alpha2 = deep_arg->eosq + 1.;
   eeta   = tle->eo * eta;
   r1     = deep_arg->cosg;
   cos2g  = r1 * r1 * 2. - 1.;
   d5     = tsi * psim2;
   d1     = d5 / po;
   d2     = eta2 * (eta2 * 4.5 + 36.) + 12.;
   d3     = eta2 * (eta2 * 2.5 + 15.);
   d4     = eta * (eta2 * 3.75 + 5.);
   b1     = ck2 * tthmun;
   b2     = -ck2 * unmth2;
   b3     = a3cof * deep_arg->sinio;
   r1     = tsi, r1 *= r1;
   c0     = b * .5 * rho * qoms2t * deep_arg->xnodp * deep_arg->aodp * (r1 * r1) * pow(psim2, 3.5) / sqrt(alpha2);
   r1     = alpha2;
   c1     = deep_arg->xnodp * 1.5 * (r1 * r1) * c0;
   c4     = d1 * d3 * b2;
   c5     = d5 * d4 * b3;
   xndt   = c1 * (eta2 * (deep_arg->eosq * 34. + 3.) + 2. + eeta * 5. * (eta2 + 4.) + deep_arg->eosq * 8.5 +
                d1 * d2 * b1 + c4 * cos2g + c5 * deep_arg->sing);
   xndtn  = xndt / deep_arg->xnodp;
   edot   = -two_thirds * xndtn * (1. - tle->eo);

   // initialize Deep()
   Deep_dpinit(tle.get(), deep_arg);
   //#ifdef RETAIN_PERTURBATION_VALUES_AT_EPOCH
   //   // initialize lunisolar perturbations:
   //   deep_arg->t = 0.;                            // added 30 Dec 2003
   //   deep_arg->solar_lunar_init_flag = 1;
   //   Deep_dpper( deep_arg);
   //   deep_arg->solar_lunar_init_flag = 0;
   //#endif
} // End of SDP8() initialization

// void SDP8( const double tsince, const tle_t *tle, const double *params,
//                                 double *pos, double *vel)
void WsfNORAD_OrbitalPropagator::SDP8(double tsince)
{
   using namespace WsfNORAD_Util;
   double am, aovr, axnm, aynm, beta, beta2m, cose, cosos, cs2f2g, csf, csfg, cslamb, di, diwc, dr, ecosf, fm, g1, g10,
      g13, g14, g2, g3, g4, g5, pm, r1, rdot, rm, rr, rvdot, sine, sini2, sinos, sn2f2g, snf, snfg, sni2du, snlamb,
      temp, ux, uy, uz, vx, vy, vz, xlamb, xmam, xmamdf, y4, y5, z1, z7, zc2, zc5;
   int i;

   // Update for secular gravity and atmospheric drag
   z1               = xndt * .5 * tsince * tsince;
   z7               = two_thirds * 3.5 * z1 / deep_arg->xnodp;
   xmamdf           = tle->xmo + deep_arg->xmdot * tsince;
   deep_arg->omgadf = tle->omegao + deep_arg->omgdot * tsince + z7 * xgdt1;
   deep_arg->xnode  = tle->xnodeo + deep_arg->xnodot * tsince + z7 * xhdt1;
   deep_arg->xn     = deep_arg->xnodp;

   // Update for deep-space secular effects
   deep_arg->xll = xmamdf;
   deep_arg->t   = tsince;
   Deep_dpsec(tle.get(), deep_arg);
   xmamdf = deep_arg->xll;
   deep_arg->xn += xndt * tsince;
   deep_arg->em += edot * tsince;
   xmam = xmamdf + z1 + z7 * xmdt1;

   // Update for deep-space periodic effects
   deep_arg->xll = xmam;
   Deep_dpper(deep_arg);
   xmam = deep_arg->xll;
   xmam = FMod2p(xmam);

   // Solve Kepler's equation
   zc2 = xmam + deep_arg->em * sin(xmam) * (deep_arg->em * cos(xmam) + 1.);

   i = 0;
   do
   {
      sine        = sin(zc2);
      cose        = cos(zc2);
      zc5         = 1. / (1. - deep_arg->em * cose);
      double cape = (xmam + deep_arg->em * sine - zc2) * zc5 + zc2;
      r1          = cape - zc2;
      if (fabs(r1) <= e6a)
      {
         break;
      }
      zc2 = cape;
   } while (i++ < 10);

   // Short period preliminary quantities
   am     = pow(xke / deep_arg->xn, two_thirds);
   beta2m = 1.f - deep_arg->em * deep_arg->em;
   sinos  = sin(deep_arg->omgadf);
   cosos  = cos(deep_arg->omgadf);
   axnm   = deep_arg->em * cosos;
   aynm   = deep_arg->em * sinos;
   pm     = am * beta2m;
   g1     = 1. / pm;
   g2     = ck2 * .5 * g1;
   g3     = g2 * g1;
   beta   = sqrt(beta2m);
   g4     = a3cof * .25 * deep_arg->sinio;
   g5     = a3cof * .25 * g1;
   snf    = beta * sine * zc5;
   csf    = (cose - deep_arg->em) * zc5;
   fm     = atan2(snf, csf);
   if (fm < 0.)
   //     fm += pi + pi;
   {
      fm += UtMath::cPI + UtMath::cPI;
   }
   snfg   = snf * cosos + csf * sinos;
   csfg   = csf * cosos - snf * sinos;
   sn2f2g = snfg * 2. * csfg;
   r1     = csfg;
   cs2f2g = r1 * r1 * 2. - 1.;
   ecosf  = deep_arg->em * csf;
   g10    = fm - xmam + deep_arg->em * snf;
   rm     = pm / (ecosf + 1.);
   aovr   = am / rm;
   g13    = deep_arg->xn * aovr;
   g14    = -g13 * aovr;
   dr     = g2 * (unmth2 * cs2f2g - tthmun * 3.) - g4 * snfg;
   diwc   = g3 * 3. * deep_arg->sinio * cs2f2g - g5 * aynm;
   di     = diwc * deep_arg->cosio;
   sini2  = sin(deep_arg->xinc * .5);

   // Update for short period periodics
   sni2du = sinio2 * (g3 * ((1. - deep_arg->theta2 * 7.) * .5 * sn2f2g - unm5th * 3. * g10) -
                      g5 * deep_arg->sinio * csfg * (ecosf + 2.)) -
            g5 * .5 * deep_arg->theta2 * axnm / cosio2;
   xlamb = fm + deep_arg->omgadf + deep_arg->xnode +
           g3 * ((deep_arg->cosio * 6. + 1. - deep_arg->theta2 * 7.) * .5 * sn2f2g -
                 (unm5th + deep_arg->cosio * 2.) * 3. * g10) +
           g5 * deep_arg->sinio * (deep_arg->cosio * axnm / (deep_arg->cosio + 1.) - (ecosf + 2.) * csfg);
   y4    = sini2 * snfg + csfg * sni2du + snfg * .5 * cosio2 * di;
   y5    = sini2 * csfg - snfg * sni2du + csfg * .5 * cosio2 * di;
   rr    = rm + dr;
   rdot  = deep_arg->xn * am * deep_arg->em * snf / beta + g14 * (g2 * 2. * unmth2 * sn2f2g + g4 * csfg);
   r1    = am;
   rvdot = deep_arg->xn * (r1 * r1) * beta / rm + g14 * dr + am * g13 * deep_arg->sinio * diwc;

   // Orientation vectors
   snlamb = sin(xlamb);
   cslamb = cos(xlamb);
   temp   = (y5 * snlamb - y4 * cslamb) * 2.;
   ux     = y4 * temp + cslamb;
   vx     = y5 * temp - snlamb;
   temp   = (y5 * cslamb + y4 * snlamb) * 2.;
   uy     = -y4 * temp + snlamb;
   vy     = -y5 * temp + cslamb;
   temp   = sqrt(1. - y4 * y4 - y5 * y5) * 2.;
   uz     = y4 * temp;
   vz     = y5 * temp;

   // Position and velocity
   mPos[0] = rr * ux * xkmper;
   mPos[1] = rr * uy * xkmper;
   mPos[2] = rr * uz * xkmper;
   // mjm always calculate velocity  if( mVel)
   //      {
   mVel[0] = (rdot * ux + rvdot * vx) * xkmper;
   mVel[1] = (rdot * uy + rvdot * vy) * xkmper;
   mVel[2] = (rdot * uz + rvdot * vz) * xkmper;
   //     }

} // SDP8

#undef tthmun
#undef sinio2
#undef cosio2
#undef unm5th
#undef unmth2
#undef a3cof
#undef xmdt1
#undef xgdt1
#undef xhdt1
#undef xndt
#undef edot
#undef deep_arg
