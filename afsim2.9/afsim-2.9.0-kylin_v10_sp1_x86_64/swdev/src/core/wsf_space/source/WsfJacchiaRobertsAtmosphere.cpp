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

// This implementation of the Jacchia-Roberts atmosphere is adapted from the
// one provided by GMAT (version R2018a). Please see
// https://software.nasa.gov/software/GSC-17177-1. The file from which this
// class is adapted is <GMAT Distro Dir>/src/base/solarsys/JacchiaRobertsAtmosphere.cpp.
// That implementation bears the following Copyright and License statement:

//------------------------------------------------------------------------------
//                              JacchiaRobertsAtmosphere
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0.
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Waka A. Waktola
// Created: 2004/05/11
//
/**
 * The Jacchia-Roberts atmosphere.
 *
 * @note This is the JR model, ported from the Swingby/Windows source
 */
//------------------------------------------------------------------------------

// For information about the model implemented in this file, see
// Fundamental of Astrodynamics and Applications, Fourth Edition, David A. Vallado,
//    Appendix B, pp. 1001-1010
// and also the references cited in the Jacchia-Roberts section beginning on p. 569.

#include "WsfJacchiaRobertsAtmosphere.hpp"

#include "UtCentralBodyEllipsoid.hpp"
#include "UtInput.hpp"
#include "UtLLAPos.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSun.hpp"

namespace // {anonymous}
{

//! low altitude density in g/m^3
constexpr double cRHO_ZERO = 3.46e-6;

//! Temperature in degrees kelvin at height of 90km
constexpr double cT_ZERO = 183.0;

//! earth gravitational constant m/sec**2
const double cG_ZERO = 9.80665;

//! gas constant (joules/(degK-mole))
const double cGAS_CON = 8.31432;

//! Avogadro's number
constexpr double cN_AVOGADRO = 6.022045e23;

//! Constants for series expansion
constexpr double cCON_C[5] = {-89284375.0, 3542400.0, -52687.5, 340.5, -0.8};

//! Molecular masses of atmospheric constituents in grams/mole
const double cMOL_MASS[6] = {
   28.0134, // Nitrogen
   39.948,  // Argon
   4.0026,  // Helium
   31.9988, // Molecular Oxygen
   15.9994, // Atomic Oxygen
   1.00797  // Hydrogen
};

} // namespace

namespace wsf
{
namespace space
{

JacchiaRobertsAtmosphere::JacchiaRobertsAtmosphere()
   : Atmosphere()
{
   SetType(cTYPE);
}

bool JacchiaRobertsAtmosphere::ProcessInput(UtInput& aInput)
{
   bool        myCommand{true};
   std::string command;
   aInput.GetCommand(command);
   if (command == "solar_flux")
   {
      aInput.ReadValue(mF107);
      aInput.ValueGreater(mF107, 0.0);
   }
   else if (command == "average_solar_flux")
   {
      aInput.ReadValue(mF107a);
      aInput.ValueGreater(mF107a, 0.0);
   }
   else if (command == "geomagnetic_index")
   {
      aInput.ReadValue(mKp);
      aInput.ValueInClosedRange(mKp, 0.0, 9.0);
   }
   else
   {
      myCommand = Atmosphere::ProcessInput(aInput);
   }
   return myCommand;
}

double JacchiaRobertsAtmosphere::GetDensity(const UtCalendar& aEpoch, const UtLLAPos& aLLA) const
{
   double height_km = aLLA.GetAlt() / 1000.0;

   // For now, JR is turned off below 100 km altitude
   if (height_km <= 100.0)
   {
      if (!mLowAltWarned)
      {
         auto msg =
            ut::log::warning() << "The Jacchia-Roberts atmosphere model is intended for altitudes above 100 km.";
         msg.AddNote() << "Resulting density will be a simplified model.";
         mLowAltWarned = true;
      }
   }

   //  For heights in the air, use JacchiaRoberts to calculate the density.
   //  For heights at or below the surface of the Earth, use the constant
   //  value used for heights below 90 KM.
   double density{cRHO_ZERO};
   if (height_km > 0.0)
   {
      // Output density in units of g/m^3
      density = JacchiaRoberts(aLLA, aEpoch);
   }
   // Convert to kg/m^3
   return 1.0e3 * density;
}

double JacchiaRobertsAtmosphere::GetPolarRadiusKm() const
{
   return mCentralBodyPtr->GetEllipsoid().GetSemiMinorAxis() / 1000.0;
}

double JacchiaRobertsAtmosphere::GetPolarRadiusKmSquared() const
{
   double polar = GetPolarRadiusKm();
   return polar * polar;
}

double JacchiaRobertsAtmosphere::JacchiaRoberts(const UtLLAPos& aLLA, const UtCalendar& aEpoch) const
{
   mX_Temp = 379.0 + 3.24 * mF107a + 1.3 * (mF107 - mF107a);

   // Compute declination of the sun
   double sunRA;
   double sunDec;
   UtSun::GetSunLocationRA_Dec(aEpoch, sunRA, sunDec);
   double solarLon = UtSun::GetSunLongitude(aEpoch);

   // Compute height dependent density
   double height_km = aLLA.GetAlt() / 1000.0;
   double density;
   if (height_km <= 90.0)
   {
      density = cRHO_ZERO;
   }
   else if (height_km < 100.0)
   {
      double temperature = Exotherm(aLLA, sunDec, solarLon);
      density            = Rho100(height_km, temperature);
   }
   else if (height_km <= 125.0)
   {
      double temperature = Exotherm(aLLA, sunDec, solarLon);
      density            = Rho125(height_km, temperature);
   }
   else if (height_km <= 2500.0)
   {
      UtLLAPos lla500{aLLA};
      lla500.SetAlt(500000.0);
      double t_500       = Exotherm(lla500, sunDec, solarLon);
      double temperature = Exotherm(aLLA, sunDec, solarLon);

      density = RhoHigh(height_km, temperature, t_500, sunDec, aLLA.GetLat() * UtMath::cRAD_PER_DEG);
   }
   else
   {
      density = 0.0;
   }

   // NOTE: This does not include the partial second offset as in GMAT's A.1 time system, but the risk here is
   // very slight, given that we are looking at effects varying on the timescale of hours to days.
   double a1Time = aEpoch.GetJulianUTC_Date() + aEpoch.GetDeltaAT();
   return density * RhoCorrection(height_km, a1Time, aLLA.GetLat() * UtMath::cRAD_PER_DEG);
}

double JacchiaRobertsAtmosphere::Exotherm(const UtLLAPos& aLLA, double aSunDec, double aSolarLon) const
{
   constexpr double cCON_L[5] = {0.1031445e5, 0.2341230e1, 0.1579202e-2, -0.1252487e-5, 0.2462708e-9};

   double exotemp;

   // Compute hour angle of the sun.
   // NOTE: This is computed differently to how GMAT does this. The results were identical in most
   // cases, and differed by ~10^-14 otherwise.
   double hourAngle = aLLA.GetLon() * UtMath::cRAD_PER_DEG - aSolarLon;

   // Compute sun and spacecraft position dependent part of temperature
   double theta = 0.5 * fabs(aLLA.GetLat() * UtMath::cRAD_PER_DEG + aSunDec);
   double eta   = 0.5 * fabs(aLLA.GetLat() * UtMath::cRAD_PER_DEG - aSunDec);
   double tau   = hourAngle - 0.64577182325 + 0.10471975512 * sin(hourAngle + 0.75049157836);
   if (tau < -UtMath::cPI)
   {
      tau += 2 * UtMath::cPI;
   }
   else if (tau > UtMath::cPI)
   {
      tau -= 2 * UtMath::cPI;
   }
   double th22  = pow(sin(theta), 2.2);
   double t1    = mX_Temp * (1.0 + 0.3 * (th22 + pow(cos(0.5 * tau), 3.0) * (pow(cos(eta), 2.2) - th22)));
   double expkp = exp(mKp);

   // Compute the infinity based on altitude
   double alt_km = aLLA.GetAlt() / 1000.0;
   if (alt_km < 200.0)
   {
      mTinfinity = t1 + 14.0 * mKp + 0.02 * expkp;
   }
   else
   {
      mTinfinity = t1 + 28.0 * mKp + 0.03 * expkp;
   }

   mTx = 371.6678 + 0.0518806 * mTinfinity - 294.3505 * exp(-0.00216222 * mTinfinity);

   // If the spacecraft altitude is below 125 km then
   if (alt_km < 125.0)
   {
      // Compute height dependent polynomial
      mSum = cCON_C[4];
      for (int i = 3; i >= 0; i--)
      {
         mSum = cCON_C[i] + mSum * alt_km;
      }
      // Compute temperature
      exotemp = mTx + (mTx - cT_ZERO) * mSum / 1.500625e6;
   }
   // else if spacecraft altitude is above 125 km then
   else if (alt_km > 125.0)
   {
      // Compute temperature dependent polynomial
      mSum = cCON_L[4];
      for (int i = 3; i >= 0; i--)
      {
         mSum = cCON_L[i] + mSum * mTinfinity;
      }
      // Compute temperature
      exotemp = mTinfinity - (mTinfinity - mTx) * exp(-(mTx - cT_ZERO) / (mTinfinity - mTx) * (alt_km - 125.0) / 35.0 *
                                                      mSum / (GetPolarRadiusKm() + alt_km));
   }
   else
   {
      exotemp = mTx;
   }

   if (alt_km <= 125.0)
   {
      double        cStar[5];
      double        aux[4][2];
      constexpr int na = 5;

      // Obtain coefficients of polynomial for auxiliary quantities
      // required for heights less than 125 km
      cStar[0] = cCON_C[0] + 1500625.0 * mTx / (mTx - cT_ZERO);
      for (int i = 1; i <= 4; i++)
      {
         cStar[i] = cCON_C[i];
      }

      aux[0][0] = 125.0; // get 1st real root
      aux[0][1] = 0.0;
      Roots(cStar, na, aux, 1);
      mRoot1 = aux[0][0];

      DeflatePolynomial(cStar, na, mRoot1, cStar);

      aux[0][0] = 200.0; // get 2nd real root
      aux[0][1] = 0.0;
      Roots(cStar, na - 1, aux, 1);
      mRoot2 = aux[0][0];

      DeflatePolynomial(cStar, na - 1, mRoot2, cStar);

      aux[0][0] = 10.0; // get remaining roots
      aux[0][1] = 125.0;
      Roots(cStar, na - 2, aux, 1);
      mX_Root = aux[0][0];
      mY_Root = fabs(aux[0][1]);
   }

   return exotemp;
}

double JacchiaRobertsAtmosphere::Rho100(double aHeightKm, double aTemperature) const
{
   constexpr double cM_ZERO   = 28.82678;
   constexpr double cM_CON[7] = {
      -435093.363387, // of M(z) function
      28275.5646391,  // (1/km)
      -765.33466108,  // (1/km**2)
      11.043387545,   // (1/km**3)
      -0.08958790995, // (1/km**4)
      0.00038737586,  // (1/km**5)
      -0.000000697444 // (1/km**6)
   };
   constexpr double cS_CON[6] = {
      3144902516.672729,   // of S(z) function
      -123774885.4832917,  // (1/km)
      1816141.096520398,   // (1/km**2)
      -11403.31079489267,  // (1/km**3)
      24.36498612105595,   // (1/km**4)
      0.008957502869707995 // (1/km**5)
   };
   constexpr double cS_BETA[6] = {
      -52864482.17910969, // of S(z) function - temperature part
      -16632.50847336828, // (1/km)
      -1.308252378125,    // (1/km**2)
      0.0,                // (1/km**3)
      0.0,                // (1/km**4)
      0.0                 // (1/km**5)
   };

   // Compute M(z) polynomial
   double m_poly = cM_CON[6];
   for (int i = 5; i >= 0; i--)
   {
      m_poly = m_poly * aHeightKm + cM_CON[i];
   }

   // Compute temperature dependent coefficients
   double b[6];
   for (int i = 0; i <= 5; ++i)
   {
      b[i] = cS_CON[i] + cS_BETA[i] * mTx / (mTx - cT_ZERO);
   }

   // Compute functions of auxiliary temperature values
   double roots_2        = mX_Root * mX_Root + mY_Root * mY_Root;
   double polarRadiusKm  = GetPolarRadiusKm();
   double polarRadiusKm2 = GetPolarRadiusKmSquared();
   double x_star = -2.0 * mRoot1 * mRoot2 * polarRadiusKm * (polarRadiusKm2 + 2.0 * polarRadiusKm * mX_Root + roots_2);
   double v =
      (polarRadiusKm + mRoot1) * (polarRadiusKm + mRoot2) * (polarRadiusKm2 + 2.0 * polarRadiusKm * mX_Root + roots_2);
   double u[2];
   u[0] = (mRoot1 - mRoot2) * (mRoot1 + polarRadiusKm) * (mRoot1 + polarRadiusKm) *
          (mRoot1 * mRoot1 - 2.0 * mRoot1 * mX_Root + roots_2);
   u[1] = (mRoot1 - mRoot2) * (mRoot2 + polarRadiusKm) * (mRoot2 + polarRadiusKm) *
          (mRoot2 * mRoot2 - 2.0 * mRoot2 * mX_Root + roots_2);
   double w[2];
   w[0] = mRoot1 * mRoot2 * polarRadiusKm * (polarRadiusKm + mRoot1) * (polarRadiusKm + roots_2 / mRoot1);
   w[1] = mRoot1 * mRoot2 * polarRadiusKm * (polarRadiusKm + mRoot2) * (polarRadiusKm + roots_2 / mRoot2);

   // Compute S(z) polynomial for z = mRoot1
   double s_poly = b[5];
   for (int i = 4; i >= 0; --i)
   {
      s_poly = s_poly * mRoot1 + b[i];
   }
   // Compute power of second quantity in f1 function
   double p2 = s_poly / u[0];

   // Compute S(z) polynomial for z = mRoot2
   s_poly = b[5];
   for (int i = 4; i >= 0; --i)
   {
      s_poly = s_poly * mRoot2 + b[i];
   }
   // Compute power of third quantity in f1 function
   double p3 = -s_poly / u[1];

   // Compute S(z) polynomial for z = negative earth radius
   s_poly = b[5];
   for (int i = 4; i >= 0; --i)
   {
      s_poly = -s_poly * polarRadiusKm + b[i];
   }
   // Compute p5 factor in f2 function
   double p5 = s_poly / v;

   // Compute power of fourth quantity in f1 function
   double p4 =
      (b[0] - mRoot1 * mRoot2 * polarRadiusKm2 * (b[4] + b[5] * (2.0 * mX_Root + mRoot1 + mRoot2 - polarRadiusKm)) +
       w[0] * p2 + w[1] * p3 - mRoot1 * mRoot2 * b[5] * polarRadiusKm * roots_2 +
       mRoot1 * mRoot2 * (polarRadiusKm2 - roots_2) * p5) /
      x_star;

   // Compute power of first quantity in f1 function
   double p1 = b[5] - 2 * p4 - p3 - p2;

   // Compute p6 factor in f2 function
   double p6 = b[4] + b[5] * (2.0 * mX_Root + mRoot1 + mRoot2 - polarRadiusKm) - p5 -
               2.0 * (mX_Root + polarRadiusKm) * p4 - (mRoot2 + polarRadiusKm) * p3 - (mRoot1 + polarRadiusKm) * p2;

   // Compute natural log of f1 function
   double log_f1 =
      p1 * log((aHeightKm + polarRadiusKm) / (90.0 + polarRadiusKm)) +
      p2 * log((aHeightKm - mRoot1) / (90.0 - mRoot1)) + p3 * log((aHeightKm - mRoot2) / (90.0 - mRoot2)) +
      p4 * log((aHeightKm * aHeightKm - 2.0 * mX_Root * aHeightKm + roots_2) / (8100.0 - 180.0 * mX_Root + roots_2));

   // Compute f2 function
   double f2 =
      (aHeightKm - 90.0) * (cM_CON[6] + p5 / ((aHeightKm + polarRadiusKm) * (90.0 + polarRadiusKm))) +
      p6 * atan(mY_Root * (aHeightKm - 90.0) / (mY_Root * mY_Root + (aHeightKm - mX_Root) * (90.0 - mX_Root))) / mY_Root;
   // Roberts eq 13(b) looks like this:
   // f2 = (aHeight - 90.0) * (1500625.0 * mCbPolarSquared / cCON_C[4] * cM_CON[6] +
   //       p5/((aHeight + mCbPolarRadius)*(90.0 + mCbPolarRadius)))
   //       + p6 * atan(mY_Root * (aHeight - 90.0)/(
   //       mY_Root*mY_Root + (aHeight - mX_Root)*(90.0 - mX_Root))) / mY_Root;

   // Compute f1 power
   // Old code (and GTDS):
   //   factor_k = -1500625.0*cG_ZERO*mCbPolarSquared/(cGAS_CON*cCON_C[4]*(mTx-cT_ZERO));
   // Replaced by Vallado's (3rd Ed, p 951):
   double factor_k = -cG_ZERO / (cGAS_CON * (mTx - cT_ZERO)); // Vallado p 951

   return cRHO_ZERO * cT_ZERO * m_poly * exp(factor_k * (log_f1 + f2)) / (cM_ZERO * aTemperature);
}

double JacchiaRobertsAtmosphere::Rho125(double aHeightKm, double aTemperature) const
{
   constexpr double cOMEGA       = -0.94585589;
   constexpr double cZETA_CON[7] = {
      0.1985549e-10,
      -0.1833490e-14, // (1/deg)
      0.1711735e-17,  // (1/deg**2)
      -0.1021474e-20, // (1/deg**3)
      0.3727894e-24,  // (1/deg**4)
      -0.7734110e-28, // (1/deg**5)
      0.7026942e-32   // (1/deg**6)
   };
   constexpr double cNUM_DENS[5] = {
      0.78110,    // Nitrogen
      0.93432e-2, // Argon
      0.61471e-5, // Helium
      0.161778,   // Molecular Oxygen
      0.95544e-1  // Atomic Oxygen
   };

   // Compute base density polynomial
   double rho_prime = cZETA_CON[6];
   for (int i = 5; i >= 0; --i)
   {
      rho_prime = rho_prime * mTinfinity + cZETA_CON[i];
   }

   // Compute base temperature
   double t_100 = mTx + cOMEGA * (mTx - cT_ZERO);

   // Compute functions of auxiliary temperature values
   double roots_2        = mX_Root * mX_Root + mY_Root * mY_Root;
   double polarRadiusKm  = GetPolarRadiusKm();
   double polarRadiusKm2 = GetPolarRadiusKmSquared();
   double x_star = -2.0 * mRoot1 * mRoot2 * polarRadiusKm * (polarRadiusKm2 + 2.0 * polarRadiusKm * mX_Root + roots_2);
   double v =
      (polarRadiusKm + mRoot1) * (polarRadiusKm + mRoot2) * (polarRadiusKm2 + 2.0 * polarRadiusKm * mX_Root + roots_2);
   double u[2];
   u[0] = (mRoot1 - mRoot2) * (mRoot1 + polarRadiusKm) * (mRoot1 + polarRadiusKm) *
          (mRoot1 * mRoot1 - 2.0 * mRoot1 * mX_Root + roots_2);
   u[1] = (mRoot1 - mRoot2) * (mRoot2 + polarRadiusKm) * (mRoot2 + polarRadiusKm) *
          (mRoot2 * mRoot2 - 2.0 * mRoot2 * mX_Root + roots_2);
   double w[2];
   w[0] = mRoot1 * mRoot2 * polarRadiusKm * (polarRadiusKm + mRoot1) * (polarRadiusKm + roots_2 / mRoot1);
   w[1] = mRoot1 * mRoot2 * polarRadiusKm * (polarRadiusKm + mRoot2) * (polarRadiusKm + roots_2 / mRoot2);

   // Compute power of second quantity in f3 function
   double q2 = 1.0 / u[0];

   // Compute power of third quantity in f3 function
   double q3 = -1.0 / u[1];

   // Compute q5 factor in f4 function
   double q5 = 1.0 / v;

   // Compute power of fourth quantity in f3 function
   double q4 = (1.0 + w[0] * q2 + w[1] * q3 + mRoot1 * mRoot2 * (polarRadiusKm2 - roots_2) * q5) / x_star;

   // Compute power of first quantity in f3 function
   double q1 = -2 * q4 - q3 - q2;

   // Compute q6 factor in f4 function
   double q6 = -q5 - 2.0 * (mX_Root + polarRadiusKm) * q4 - (mRoot2 + polarRadiusKm) * q3 - (mRoot1 + polarRadiusKm) * q2;

   // Compute log of f3 function
   double log_f3 =
      q1 * log((aHeightKm + polarRadiusKm) / (100.0 + polarRadiusKm)) +
      q2 * log((aHeightKm - mRoot1) / (100.0 - mRoot1)) + q3 * log((aHeightKm - mRoot2) / (100.0 - mRoot2)) +
      q4 * log((aHeightKm * aHeightKm - 2.0 * mX_Root * aHeightKm + roots_2) / (1.0e4 - 200.0 * mX_Root + roots_2));

   // Compute f4 function
   double f4 =
      (aHeightKm - 100.0) * q5 / ((aHeightKm + polarRadiusKm) * (100.0 + polarRadiusKm)) +
      q6 * atan(mY_Root * (aHeightKm - 100.0) / (mY_Root * mY_Root + (aHeightKm - mX_Root) * (100.0 - mX_Root))) / mY_Root;

   // Compute f3 power
   double factor_k = -1500625.0 * cG_ZERO * polarRadiusKm2 / (cGAS_CON * cCON_C[4] * (mTx - cT_ZERO));

   // Compute mass-dependent sum
   double rho_sum = 0.0;
   for (int i = 0; i <= 4; ++i)
   {
      double rhoi = cMOL_MASS[i] * cNUM_DENS[i] * exp(cMOL_MASS[i] * factor_k * (f4 + log_f3));

      if (i == 2)
      {
         rhoi *= pow(t_100 / aTemperature, -0.38);
      }
      rho_sum += rhoi;
   }

   return rho_sum * rho_prime * t_100 / aTemperature;
}

double JacchiaRobertsAtmosphere::RhoCorrection(double aHeightKm, double aA1_Time, double aGeoLat) const
{
   // Compute geomagnetic activity correction
   double geoCor;
   if (aHeightKm < 200.0)
   {
      geoCor = 0.012 * mKp + 0.000012 * exp(mKp);
   }
   else
   {
      geoCor = 0.0;
   }

   // Compute semiannual variation correction
   double f         = (5.876e-7 * pow(aHeightKm, 2.331) + 0.06328) * exp(-0.002868 * aHeightKm);
   double day_58    = (aA1_Time - 6204.5) / 365.2422;
   double tausa     = day_58 + 0.09544 * (pow(0.5 * (1.0 + sin(UtMath::cTWO_PI * day_58 + 6.035)), 1.65) - 0.5);
   double alpha     = sin(4.0 * UtMath::cPI * tausa + 4.259);
   double g         = 0.02835 + (0.3817 + 0.17829 * sin(UtMath::cTWO_PI * tausa + 4.137)) * alpha;
   double semianCor = f * g;

   // Compute seasonal latitudinal variation
   double sinLat  = sin(aGeoLat);
   double etaLat  = sin(UtMath::cTWO_PI * day_58 + 1.72) * sinLat * fabs(sinLat);
   double slatCor = 0.014 * (aHeightKm - 90.0) * etaLat * exp(-0.0013 * (aHeightKm - 90.0) * (aHeightKm - 90.0));

   return pow(10.0, geoCor + semianCor + slatCor);
}

double JacchiaRobertsAtmosphere::RhoHigh(double aHeightKm, double aTemperature, double aT_500, double aSunDec, double aGeoLat) const
{
   constexpr double cCON_DEN[5][7] = {{
                                         // Nitrogen
                                         0.1093155e2,
                                         0.1186783e-2,   // (1/deg)
                                         -0.1677341e-5,  // (1/deg**2)
                                         0.1420228e-8,   // (1/deg**3)
                                         -0.7139785e-12, // (1/deg**4)
                                         0.1969715e-15,  // (1/deg**5)
                                         -0.2296182e-19  // (1/deg**6)
                                      },
                                      {
                                         // Argon
                                         0.8049405e1,
                                         0.2382822e-2,   // (1/deg)
                                         -0.3391366e-5,  // (1/deg**2)
                                         0.2909714e-8,   // (1/deg**3)
                                         -0.1481702e-11, // (1/deg**4)
                                         0.4127600e-15,  // (1/deg**5)
                                         -0.4837461e-19  // (1/deg**6)
                                      },
                                      {
                                         // Helium
                                         0.7646886e1,
                                         -0.4383486e-3,  // (1/deg)
                                         0.4694319e-6,   // (1/deg**2)
                                         -0.2894886e-9,  // (1/deg**3)
                                         0.9451989e-13,  // (1/deg**4)
                                         -0.1270838e-16, // (1/deg**5)
                                         0.0             // (1/deg**6)
                                      },
                                      {
                                         // Molecular Oxygen
                                         0.9924237e1,
                                         0.1600311e-2,   // (1/deg)
                                         -0.2274761e-5,  // (1/deg**2)
                                         0.1938454e-8,   // (1/deg**3)
                                         -0.9782183e-12, // (1/deg**4)
                                         0.2698450e-15,  // (1/deg**5)
                                         -0.3131808e-19  // (1/deg**6)
                                      },
                                      {
                                         // Atomic Oxygen
                                         0.1097083e2,
                                         0.6118742e-4,   // (1/deg)
                                         -0.1165003e-6,  // (1/deg**2)
                                         0.9239354e-10,  // (1/deg**3)
                                         -0.3490739e-13, // (1/deg**4)
                                         0.5116298e-17,  // (1/deg**5)
                                         0.0             // (1/deg**6)
                                      }};

   double di{0.0};
   double rho_out{0.0};
   for (int i = 0; i <= 5; ++i)
   {
      // Compute constituent density sum for this atmospheric component
      if (i <= 4) // (skip 5 => hydrogen)
      {
         double log_di = cCON_DEN[i][6];
         for (int j = 5; j >= 0; --j)
         {
            log_di = log_di * mTinfinity + cCON_DEN[i][j];
         }
         di = pow(10.0, log_di) / cN_AVOGADRO;
      }

      // Compute second exponent in density expression for this component
      double polar125 = GetPolarRadiusKm() + 125.0;
      double gamma    = 35.0 * cMOL_MASS[i] * cG_ZERO * GetPolarRadiusKmSquared() * (mTinfinity - mTx) /
                     (cGAS_CON * mSum * mTinfinity * (mTx - cT_ZERO) * polar125);

      // Compute first exponent in density expression for this component
      double exp1 = 1.0 + gamma;

      // A factor which is non-unity only for helium
      double f = 1.0;

      // Compute corrections for helium
      if (i == 2)
      {
         exp1 -= 0.38;
         f = 4.9914 * fabs(aSunDec) *
             (pow(sin(0.25 * UtMath::cPI - 0.5 * aGeoLat * aSunDec / fabs(aSunDec)), 3) - 0.35355) / UtMath::cPI;
         f = pow(10.0, f);
      }

      // Add corrections to rho_out, skip hydrogen unless above 500 km
      if (aHeightKm > 500.0 && i == 5)
      {
         double r = cMOL_MASS[5] * pow(10.0, 73.13 - (39.4 - 5.5 * log10(aT_500)) * log10(aT_500)) *
                    pow(aT_500 / aTemperature, exp1) * pow((mTinfinity - aTemperature) / (mTinfinity - aT_500), gamma) /
                    cN_AVOGADRO;
         rho_out += r;
      }
      else if (i <= 4)
      {
         double r = f * cMOL_MASS[i] * di * pow(mTx / aTemperature, exp1) *
                    pow((mTinfinity - aTemperature) / (mTinfinity - mTx), gamma);
         rho_out += r;
      }
   }
   return rho_out;
}

void JacchiaRobertsAtmosphere::Roots(double aA[], int aNa, double aCroots[][2], int aIrl)
{
   double z[2];
   double zs[2];
   double cb[2];
   double cc[2];
   double dif;

   int ir = 0; // ir counts roots solved to this point
   int n1 = aNa - 1;
   int n2 = n1 - 1;

   // While the number of roots solved to this point (ir) has not exceeded
   // the number passed to this function (irl), perform the following
   // iterations.

   while (ir < aIrl)
   {
      z[0] = aCroots[ir][0]; // z is the currently iterated value
      z[1] = aCroots[ir][1]; // of the root

      do                 // Iterate while the convergence criterion
      {                  // is greater than the defined limit
         cb[0] = aA[n1]; // double component of complex number
         cb[1] = 0;      // Imaginary component of complex number
         cc[0] = aA[n1]; // double component of complex number
         cc[1] = 0;      // Imaginary component of complex number

         // cc and cb are the values of the polynomial (P(z))
         // and its' derivative (P'(z)).

         for (int i = 0; i <= n2; ++i)
         {
            int    j    = n2 - i;
            double temp = (z[0] * cb[0] - z[1] * cb[1]) + aA[j];
            cb[1]       = z[0] * cb[1] + z[1] * cb[0];
            cb[0]       = temp;
            if (j != 0)
            {
               temp  = (z[0] * cc[0] - z[1] * cc[1]) + cb[0];
               cc[1] = (z[0] * cc[1] + z[1] * cc[0]) + cb[1];
               cc[0] = temp;
            }
         }
         zs[0] = z[0]; // Result of last iteration saved in zs
         zs[1] = z[1];

         // Newton's Method: Z(I) = Z(I-1) - P(Z(I-1)) / P'(Z(I-1))
         // tends towards a root of the polynomial P(Z).

         double denom = cc[0] * cc[0] + cc[1] * cc[1];
         z[0] -= ((cb[0] * cc[0] + cb[1] * cc[1]) / denom);
         z[1] += ((cb[0] * cc[1] - cb[1] * cc[0]) / denom);

         // The convergence criterion (dif) is the sum of the relative
         // changes in the real and imaginary parts of the root (z).

         dif = fabs((zs[0] - z[0]) / zs[0]);
         if (zs[1] != 0.0E0)
         {
            dif += fabs((zs[1] - z[1]) / zs[1]);
         }
      } while (dif > 1.0E-14); // If convergence criterion is too large
                               // iterate again

      // Convergence criterion is complete.  Store the answer in the
      // output array 'croots' and start on the next root.

      aCroots[ir][0] = z[0];
      aCroots[ir][1] = z[1];
      ++ir; // Increment roots solved counter
   }
}

void JacchiaRobertsAtmosphere::DeflatePolynomial(double aC[], int aN, double aRoot, double aCnew[])
{
   double sum = aC[aN - 1];
   for (int i = aN - 2; i >= 0; --i)
   {
      double save = aC[i];
      aCnew[i]    = sum;
      sum         = save + sum * aRoot;
   }
}

} // namespace space
} // namespace wsf
