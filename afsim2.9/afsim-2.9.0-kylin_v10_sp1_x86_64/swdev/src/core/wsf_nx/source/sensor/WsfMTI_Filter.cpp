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

#include "WsfMTI_Filter.hpp"

#include <numeric>
#include <utility>

#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

namespace
{
int          max_gates               = 4; //(MXGATE = 4)
const double cMTI_FILTER_ROMBERG_EPS = 1.0E-4;
} // namespace

// =================================================================================================
WsfMTI_Filter::WsfMTI_Filter()
   : effects(true)
   , ndelay(0)
   , nprfs(0)
   , ngate(0)
   , average_resp(0.0)
   , clutter_resp(0.0)
   , max_angle()
   , max_range()
   , min_angle()
   , min_range()
   , min_resp_abs(0.0)
   , min_resp_db(0.0)
   , const1(0.0)
   , const2(0.0)
   , conical_scan_rate(0.0)
   , decay_const(0.0)
   , sigma3(0.0)
   , unambig_range(0.0)
   , pioprf()
{
}

// =================================================================================================
// virtual
WsfMTI_Filter* WsfMTI_Filter::Clone() const
{
   return new WsfMTI_Filter(*this);
}

// virtual
bool WsfMTI_Filter::Initialize(double  aConicalScanRate,
                               double  aDecayConst,
                               double  aIF_Bandwidth,
                               int     aN_PRFs,
                               DbleVec aPRFs,
                               double  aSigmaC)
{
   bool ok = true;

   if (nprfs < 1)
   {
      nprfs = aN_PRFs;
   }

   if (nprfs < 1)
   {
      ut::log::error() << "WsfMTI_Filter::Initialize: Number of PRFs is 0, should be > 0.";
      return false;
   }

   if (ndelay < 1)
   {
      ut::log::error() << "WsfMTI_Filter::Initialize: Number of MTI delays is < 0, should be > 0.";
      return false;
   }

   if (min_resp_db < 0.0)
   {
      ut::log::error() << "WsfMTI_Filter::Initialize: MTI minimum response is < 0 dB, should be > 0 dB.";
      return false;
   }

   if ((ngate < 0) || (ngate > max_gates))
   {
      auto logger = ut::log::error() << "WsfMTI_Filter::Initialize: Number of MTI gates is out of range.";
      logger.AddNote() << "Current MTI Gates: " << ngate;
      logger.AddNote() << "Number of MTI gates should be between 0 and " << max_gates << ".";
      return false;
   }

   for (int i = 0; i < ngate; ++i)
   {
      if (min_range[i] < 0.0)
      {
         auto logger = ut::log::error() << "WsfMTI_Filter::Initialize: Minimum MTI Range is less than 0.0 m.";
         logger.AddNote() << "Range #: " << i + 1;
         logger.AddNote() << "Minimum range: " << min_range[i] << " m";
         return false;
      }
      if (max_range[i] < 0.0)
      {
         auto logger = ut::log::error() << "WsfMTI_Filter::Initialize - Maximum MTI Range m is less than 0.0 m.";
         logger.AddNote() << "Range #: " << i + 1;
         logger.AddNote() << "Maximum range: " << max_range[i] << " m";
         return false;
      }
      if (max_range[i] < min_range[i])
      {
         auto logger =
            ut::log::error() << "WsfMTI_Filter::Initialize - Maximum MTI Range must be greater than Minimum MTI Range.";
         logger.AddNote() << "Range #(max): " << i + 1;
         logger.AddNote() << "Maximum Range: " << max_range[i] << " m";
         logger.AddNote() << "Minimum Range: " << min_range[i] << " m";
         return false;
      }

      if ((min_angle[i] > UtMath::cPI) || (min_angle[i] < -UtMath::cPI))
      {
         auto logger = ut::log::error() << "WsfMTI_Filter::Initialize: Minimum MTI Angle is out of bounds.";
         logger.AddNote() << "Angle #: " << i + 1;
         logger.AddNote() << "Angle: " << UtMath::cDEG_PER_RAD * min_angle[i] << " deg";
         logger.AddNote() << "Expected: -180 deg (-PI rad) <= min_angle <= +180 deg (+PI rad)";
         return false;
      }
      if ((max_angle[i] > UtMath::cPI) || (max_angle[i] < -UtMath::cPI))
      {
         auto logger = ut::log::error() << "WsfMTI_Filter::Initialize: Maximum MTI Angle is out of bounds.";
         logger.AddNote() << "Angle #: " << i + 1;
         logger.AddNote() << "Angle: " << UtMath::cDEG_PER_RAD * max_angle[i] << " deg";
         logger.AddNote() << "Expected: -180 deg (-PI rad) <= max_angle <= +180 deg (+PI rad)";
         return false;
      }
      if (max_angle[i] < min_angle[i])
      {
         auto logger =
            ut::log::error() << "WsfMTI_Filter::Initialize: Maximum MTI Angle must be greater than Minimum MTI Angle.";
         logger.AddNote() << "Angle #: " << i + 1;
         logger.AddNote() << "Angle(max): " << UtMath::cDEG_PER_RAD * max_angle[i] << " deg";
         logger.AddNote() << "Angle(min): " << UtMath::cDEG_PER_RAD * min_angle[i] << " deg";

         return false;
      }
   }

   ok &= init_mti(aConicalScanRate, aDecayConst, aIF_Bandwidth, aN_PRFs, aPRFs, aSigmaC);

   return ok;
}

// =================================================================================================
// virtual
bool WsfMTI_Filter::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "mti_switch")
   {
      aInput.ReadValue(effects);
   }
   else if (command == "mti_num_delays")
   {
      aInput.ReadValue(ndelay);
      aInput.ValueGreaterOrEqual(ndelay, 0);
   }
   else if (command == "mti_min_response")
   {
      aInput.ReadValueOfType(min_resp_abs, UtInput::cRATIO);
      min_resp_db = UtMath::SafeLinearToDB(min_resp_abs);
      aInput.ValueGreaterOrEqual(min_resp_db, 0.0);
   }
   else if (command == "mti_num_gates")
   {
      aInput.ReadValue(ngate);
      aInput.ValueInClosedRange(ngate, 0, max_gates);
   }
   // else if (command == "mti_min_range")
   //{
   //    int j;
   //    aInput.ReadValue(j);
   //    if (((min_range.size() + 1) == j) && (j < max_gates))
   //    {
   //       min_range.resize(j);
   //       aInput.ReadValueOfType(min_range[j - 1], UtInput::cLENGTH);
   //       aInput.ValueGreaterOrEqual(min_range[j - 1], 0.0);
   //    }
   //    else
   //    {
   //       throw UtInput::BadValue(aInput, "Ranges must be in increasing gate # order and match the number or range gates.");
   //    }
   // }
   // else if (command == "mti_max_range")
   //{
   //    int j;
   //    aInput.ReadValue(j);
   //    if (((max_range.size() + 1) == j) && (j < max_gates))
   //    {
   //       max_range.resize(j);
   //       aInput.ReadValueOfType(max_range[j - 1], UtInput::cLENGTH);
   //       aInput.ValueGreaterOrEqual(max_range[j - 1], 0.0);
   //    }
   //    else
   //    {
   //       throw UtInput::BadValue(aInput, "Ranges must be in increasing gate # order and match the number or range gates.");
   //    }
   // }
   else if (command == "mti_range_limits")
   {
      int j;
      aInput.ReadValue(j);
      if ((static_cast<int>(max_range.size() + 1) == j) && (j < max_gates))
      {
         min_range.resize(j);
         aInput.ReadValueOfType(min_range[j - 1], UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(min_range[j - 1], 0.0);
         max_range.resize(j);
         aInput.ReadValueOfType(max_range[j - 1], UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(max_range[j - 1], 0.0);

         aInput.ValueLessOrEqual(min_range[j - 1], max_range[j - 1]);
      }
      else
      {
         throw UtInput::BadValue(aInput, "Ranges must be in increasing gate # order and match the number or range gates.");
      }
   }
   // else if (command == "mti_min_angle")
   //{
   //    int j;
   //    aInput.ReadValue(j);
   //    if (((min_angle.size() + 1) == j) && (j < max_gates))
   //    {
   //       min_angle.resize(j);
   //       aInput.ReadValueOfType(min_angle[j - 1], UtInput::cANGLE);
   //    }
   //    else
   //    {
   //       throw UtInput::BadValue(aInput, "Angles must be in increasing gate # order and match the number or range gates.");
   //    }
   // }
   // else if (command == "mti_max_angle")
   //{
   //    int j;
   //    aInput.ReadValue(j);
   //    if (((max_angle.size() + 1) == j) && (j < max_gates))
   //    {
   //       max_angle.resize(j);
   //       aInput.ReadValueOfType(max_angle[j - 1], UtInput::cANGLE);
   //    }
   //    else
   //    {
   //       throw UtInput::BadValue(aInput, "Angles must be in increasing gate # order and match the number or range gates.");
   //    }
   // }
   else if (command == "mti_angle_limits")
   {
      int j;
      aInput.ReadValue(j);
      if ((static_cast<int>(min_angle.size() + 1) == j) && (j < max_gates))
      {
         min_angle.resize(j);
         aInput.ReadValueOfType(min_angle[j - 1], UtInput::cANGLE);
         aInput.ValueInClosedRange(min_angle[j - 1], -UtMath::cTWO_PI, UtMath::cTWO_PI);
         max_angle.resize(j);
         aInput.ReadValueOfType(max_angle[j - 1], UtInput::cANGLE);
         aInput.ValueInClosedRange(max_angle[j - 1], -UtMath::cTWO_PI, UtMath::cTWO_PI);

         aInput.ValueLessOrEqual(min_angle[j - 1], max_angle[j - 1]);
      }
      else
      {
         throw UtInput::BadValue(aInput, "Angles must be in increasing gate # order and match the number or range gates.");
      }
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

//--------------------------------------------------------------------
// Initialize MTI data.
//--------------------------------------------------------------------
// private
bool WsfMTI_Filter::init_mti(double  aConicalScanRate,
                             double  aDecayConst,
                             double  aIF_Bandwidth, // Hz
                             int     aN_PRFs,
                             DbleVec aPRF, // Hz
                             double  aSigmaC)
{
   bool ok = true;

   //------------------------------------------------------------------
   // Dynamically allocate the pi over prf array
   //------------------------------------------------------------------
   pioprf.clear();
   pioprf.resize(aN_PRFs);

   nprfs = aN_PRFs;
   for (int iprf = 0; iprf < nprfs; ++iprf)
   {
      pioprf[iprf] = UtMath::cPI / aPRF[iprf];
   }

   double prfhz_avg = accumulate(aPRF.begin(), aPRF.end(), 0.0) / static_cast<double>(nprfs);
   unambig_range    = (0.5 * UtMath::cLIGHT_SPEED / prfhz_avg);

   set_mti_consts(aSigmaC, aDecayConst, aConicalScanRate);


   //------------------------------------------------------------------
   // Convert the minimum power response of the MTI system, the floor
   // of the MTI power response, from dB to absolute.
   //------------------------------------------------------------------

   min_resp_abs = pow(10.0, (-min_resp_db * 0.1));

   //------------------------------------------------------------------
   // Determine the average power gain of the MTI system.This
   // determination takes into account that the MTI system frequency
   // response is symmetric about zero frequency across the receiver
   // noise bandwidth.
   //------------------------------------------------------------------

   double fhigh(0.0);
   double flow(0.0);
   average_resp = 1.0;
   if (effects && (ndelay >= 1) && (ngate >= 1))
   {
      fhigh        = aIF_Bandwidth / 2.0;
      double fstep = 10.0;
      double xsum  = 0.0;

      double freq = 0.0;
      do
      {
         xsum = xsum + resmti(freq);
         freq = freq + fstep;
      } while (freq <= fhigh);

      average_resp = xsum * fstep / fhigh;
   }

   // Determine the response of the MTI to clutter.Numerically
   // integrate the transfer function from + / -45 sigma.
   flow  = -15.0 * sigma3;
   fhigh = 15.0 * sigma3;

   clutter_resp = rommti(flow, fhigh, cMTI_FILTER_ROMBERG_EPS);

   return ok;
}

//--------------------------------------------------------------------
// Compute the MTI response.
//
// aFlag can have any of the sig_* values defined in types.
//--------------------------------------------------------------------
// public
double WsfMTI_Filter::mti_response(double aAzAngle,          // radians
                                   double aDopplerFrequency, // Hz
                                   int    aFlag,
                                   double aSlantRange) // meters
{
   double mti_response = 1.0;

   //------------------------------------------------------------------
   // Return if mti is off.
   //------------------------------------------------------------------
   if (!effects)
   {
      return mti_response;
   }

   // return if signal is pre - processed target type
   if (aFlag == cST_PROCESSED_TARGET)
   {
      return mti_response;
   }

   //------------------------------------------------------------------
   // Determine if target is in an MTI gate.
   //------------------------------------------------------------------

   bool mgate = is_gated(aAzAngle, aSlantRange);

   if (!mgate)
   {
      return mti_response;
   }

   if (aFlag == cST_CLUTTER)
   {
      // get the pre - computed clutter response
      mti_response = clutter_resp;
   }
   else if (aFlag == cST_NOISE_JAM || aFlag == cST_NOISE)
   {
      //----------------------------------------------------------------
      // For a noise jamming signal or just noise, the MTI response is
      // just the average.
      //----------------------------------------------------------------
      mti_response = average_resp;
   }
   else
   {
      mti_response = resmti(aDopplerFrequency);
   }

   return mti_response;
}


//--------------------------------------------------------------------
// Determine if the target is within on of the specified
// azimuth / range MTI gates.
//
// Inputs :
// ALPHAT - True bearing of the target relative to the radar site in radians
// RANGET - True range from radar to target in meters
//--------------------------------------------------------------------
// private
bool WsfMTI_Filter::is_gated(double aAlphaT, // radians
                             double aRangeT) // meters
{
   bool is_gated = false;

   //------------------------------------------------------------------
   // If MTI is off, return.
   //------------------------------------------------------------------

   if (!effects)
   {
      return is_gated;
   }

   //------------------------------------------------------------------
   // Initialize values and convert units : RANGE is in m, and ALPHA is
   // in radians
   //------------------------------------------------------------------

   double range = aRangeT;
   double alpha = aAlphaT;

   //------------------------------------------------------------------
   // Convert to 0-2*pi radian headings
   //------------------------------------------------------------------

   // if (alpha < 0.0)
   //{
   //    alpha = 360.0 + alpha;
   // }

   // make target range unambiguous.
   range = range - static_cast<int>(range / unambig_range) * unambig_range;

   //------------------------------------------------------------------
   // Loop through each gate and determine if the target is within in at
   // least one.
   //------------------------------------------------------------------

   for (int i = 0; i < ngate; ++i)
   {
      //----------------------------------------------------------------
      // Check for target within the range gate and then the azimuth
      // gate.
      //----------------------------------------------------------------

      if ((range >= min_range[i]) && (range < max_range[i]))
      {
         //------------------------------------------------------------
         // The target is within the range gate.
         //------------------------------------------------------------

         if (min_angle[i] <= max_angle[i])
         {
            if ((alpha >= min_angle[i]) && (alpha < max_angle[i]))
            {
               //------------------------------------------------------
               // The target is within the azimuth gate
               //------------------------------------------------------
               is_gated = true;
            }
         }
         else
         {
            //----------------------------------------------------------
            // Handle the special case where the MTI azimuth gate
            // straddles North or 0.0 degrees.
            //----------------------------------------------------------

            if ((alpha >= min_angle[i]) || (alpha < max_angle[i]))
            {
               is_gated = true;
            }
         }
      }
   }
   return is_gated;
}

//--------------------------------------------------------------------
// Determine the response of a MTI system to an input signal using
// Romberg integration.
//
// This subroutine no longer assumes the functions
// it is numerically integrating are symmetric, thus
// it can integrate non - symmetric functions.This
// makes it the same as the other numerical
// integration subroutines; ROMFPD and ROMCHF.PJH
//--------------------------------------------------------------------
// private
double WsfMTI_Filter::rommti(double a, double b, double eps)
{
   const int maxtrp(25);
   double    trzoid[maxtrp] = {0.0};
   double    rommti(0.0);

   //------------------------------------------------------------------
   // If B is less than A, interchange A and B and change the sign of
   // the integral.
   //------------------------------------------------------------------
   double ax;
   double bx;
   double sign;
   if (a <= b)
   {
      ax   = a;
      bx   = b;
      sign = 1.0;
   }
   else
   {
      ax   = b;
      bx   = a;
      sign = -1.0;
   }

   double h      = bx - ax;
   double hover2 = 0.5 * h;

   //------------------------------------------------------------------
   // Determine TRZOID[0], the first trapezoidal approximation.
   //------------------------------------------------------------------

   trzoid[0] = hover2 * (cltmti(ax) + cltmti(bx));

   //------------------------------------------------------------------
   // Determine TRZOID(N), the N - th trapezoidal approximation for N =
   // 2, 3, ..., MAXTRP.
   //------------------------------------------------------------------

   int nterm = 1;
   for (int n = 1; n < maxtrp; ++n)
   {
      double x    = ax + hover2;
      double sum  = cltmti(x);
      int    nsum = nterm - 1;

      for (int k = 1; k <= nsum; ++k)
      {
         x   = x + h;
         sum = sum + cltmti(x);
      }

      trzoid[n] = 0.5 * (trzoid[n - 1] + h * sum);

      //----------------------------------------------------------------
      // If the relative error is less than the input relative error
      // tolerance, exit from this routine.Otherwise, perform
      // Richardson's extrapolation to obtain more accurate values.
      //----------------------------------------------------------------

      double d = 1.0;

      for (int i = n; i > 1; --i)
      {
         d             = 4.0 * d;
         double deltat = trzoid[i] - trzoid[i - 1];
         trzoid[i - 1] = trzoid[i] + deltat / (d - 1.0);

         if (std::abs(deltat / trzoid[i]) <= eps)
         {
            rommti = trzoid[i - 1] * sign;
            return rommti; // early exit
         }
      }
      nterm  = 2 * nterm;
      h      = hover2;
      hover2 = 0.5 * h;
   }

   //------------------------------------------------------------------
   // Entrance to this section of the subroutine indicates the relative
   // error tolerance was not achieved.Either the relative error
   // tolerance must be increased, or the parameter MAXTRP must be
   // increased in the 'PARAMETER' statement at the beginning of the
   // subroutine.
   //------------------------------------------------------------------
   auto logger = ut::log::warning() << "ROMMTI Subroutine";
   logger.AddNote()
      << "ROMMTI(Romberg integration routine) was unable to return an answer with the input relative error tolerance.";
   logger.AddNote() << "The input relative error tolerance must be increased or the parameter MAXTRP in the PARAMETER "
                       "statement must be increased.";
   throw std::exception();

   //------------------------------------------------------------------
   // The RETURN statement is located within the 'D0 20' loop above.
   //------------------------------------------------------------------
   return rommti;
}

//--------------------------------------------------------------------
// Calculate the magnitude of a single spectral line of clutter in
// a MTI filter.
//
// - The response of the MTI system is determined
// through the External Function RESMTI.PJH
// 10 - Dec - 2003 - Updated for MTD radar type.Removed the
// limiting of delta frequency squared.The
// limiting is not in the OCD manual equations.
// See SPCR #1306.
//--------------------------------------------------------------------
// private
double WsfMTI_Filter::cltmti(double aFrequency)
{
   //------------------------------------------------------------------
   // Determine the power response of the MTI system at the input
   // frequency.
   //------------------------------------------------------------------

   double ux = resmti(aFrequency);

   //------------------------------------------------------------------
   // Determine the amplitude of PSD at the input frequency.
   //------------------------------------------------------------------

   double deltaf = aFrequency - conical_scan_rate;
   double delfsq = deltaf * deltaf;

   double tx(0.0);
   if (std::abs(deltaf) < sigma3)
   {
      tx = const1 * exp(const2 * delfsq);
   }
   else /*if (std::abs(deltaf) >= sigma3)*/
   {
      tx = std::max(const1 * exp(const2 * delfsq), decay_const / delfsq);
   }

   //------------------------------------------------------------------
   // Determine the clutter response out of the MTI system at the
   // input frequency.
   //------------------------------------------------------------------

   return ux * tx;
}


//--------------------------------------------------------------------
// Determine the response of a MTI system to the input frequency.
// This response is a function of number of delays and the pulse
// repetition frequency(ies), either single or staggered.
//
// Inputs:
// FREQ - The frequency at which the response of the MTI
// system is to be determined.
//
// NDELAY - The number of delays in the MTI system.
//
// NPRFS - The number of pulse repetition frequencies.If
// greater than one, a staggered pulse repetition
// frequency MTI system is assumed.
//
// PIOPRF - Pi divided by the pulse repetition frequency.
//
// MTI_MIN_RESP_ABS - Minimum MTI response.
//
//
// Outputs:
// RESMTI - The response of the MTI system at the input
// frequency.
//
// Reference:
// [1] "MTI And Pulsed Doppler Radar", D.Curtis
// Schleher, Artech House, 1991.  Chapter 6,
// Section 6.4 pages 390 through 403.
//--------------------------------------------------------------------
// private
double WsfMTI_Filter::resmti(double aFrequency)
{
   //------------------------------------------------------------------
   // Determine the power response of the MTI system at the frequency of
   // the input signal for either a single pulse repetition frequency,
   // or staggered pulse repetition frequencies.
   //------------------------------------------------------------------

   double resout(0.0);
   if (ndelay <= 0)
   {
      //----------------------------------------------------------------
      // No delays set the response to one.
      //----------------------------------------------------------------
      resout = 1.0;
   }
   else
   {
      double restmp(0.0);
      if ((nprfs == 1) || (nprfs > 1 && ndelay >= 3))
      {
         //------------------------------------------------------------
         // The MTI system uses either a single pulse repetition
         // frequency or staggered pulse repetition frequencies with
         // three or greater delays.For a single pulse repetition
         // frequency the response of of the MTI system is given by
         // Equation C.30, reference[1], 629.  Reference[1] does not
         // address the response of a MTI system with staggered pulse
         // repetition frequencies for three or greater delays.
         // Therefore, such systems are assumed to use a
         // "block-to-block" stagger.For a block stagger the overall
         // MTI system response is the average of the individual single
         // pulse repetition frequency responses, reference[1] page
         // 390.
         //------------------------------------------------------------

         double sumres = 0.0;

         for (int iprf = 0; iprf < nprfs; ++iprf)
         {
            double x      = aFrequency * pioprf[iprf];
            double resprf = pow((2.0 * sin(x)), (2 * ndelay));
            sumres        = sumres + resprf;
         }
         restmp = sumres / static_cast<double>(nprfs);
      }
      else if (nprfs > 1 && ndelay == 1)
      {
         //------------------------------------------------------------
         // The MTI system uses staggered pulse repetition frequencies
         // and one delay.The response is defined by a corrected
         // version of Equation 6.109, reference[1].The correction
         // accurately accounts for the number of staggered pulse
         // repetition frequencies.
         //------------------------------------------------------------

         double sumres = 0.0;

         for (int iprf = 0; iprf < nprfs; ++iprf)
         {
            double x      = 2.0 * aFrequency * pioprf[iprf];
            double resprf = cos(x);
            sumres        = sumres + resprf;
         }

         restmp = static_cast<double>(nprfs) - sumres;
      }
      else if ((nprfs > 1) && (ndelay == 2))
      {
         //------------------------------------------------------------
         // The MTI system uses staggered pulse repetition frequencies
         // and two delays.The response is defined by a corrected
         // version of Equation 6.118, reference[1].The correction
         // accurately accounts for the trigonometry identify used to
         // solve Equation 6.117.
         //------------------------------------------------------------

         double x = 2.0 * aFrequency;
         double sumres =
            3.0 - 2.0 * cos(x * pioprf[0]) + cos(x * (pioprf[0] + pioprf[nprfs - 1])) - 2.0 * cos(x * pioprf[nprfs - 1]);

         for (int iprf = 1; iprf < nprfs; ++iprf)
         {
            double resprf = 3.0 - 2.0 * cos(x * pioprf[iprf]) + cos(x * (pioprf[iprf] + pioprf[iprf - 1])) -
                            2.0 * cos(x * pioprf[iprf - 1]);
            sumres = sumres + resprf;
         }

         restmp = sumres;
      }

      //----------------------------------------------------------------
      // Limit the power response of the MTI system to the user - supplied
      // minimum.
      //----------------------------------------------------------------

      resout = std::max(restmp, min_resp_abs);
   }
   return resout;
}

//--------------------------------------------------------------------
//------ -
// Set the MTI constants that a based on the standard deviation of
// the Gaussian portion of the clutter power spectral density
// (sigmac).Often this is referred to as the RMS clutter
// frequency spread.
//--------------------------------------------------------------------
// private
void WsfMTI_Filter::set_mti_consts(double aSigmaC, double aDecayConst, double aConicalScanRate)
{
   //------------------------------------------------------------------
   // Determine several terms for use in MTI routines for pulsed radars.
   //------------------------------------------------------------------
   if (aSigmaC > 0.0)
   {
      const1 = 0.39894 / aSigmaC;
      const2 = -0.5 / (aSigmaC * aSigmaC);
      sigma3 = 3.0 * aSigmaC;
   }
   else
   {
      double sigma_default = 10.0;
      const1               = 0.39894 / sigma_default;
      const2               = -0.5 / (sigma_default * sigma_default);
      sigma3               = 3.0 * sigma_default;
   }

   if (aDecayConst > 0.0)
   {
      decay_const = aDecayConst;
   }

   if (aConicalScanRate > 0.0)
   {
      conical_scan_rate = aConicalScanRate;
   }
}

//--------------------------------------------------------------------
// public
void WsfMTI_Filter::set_mti_data(int     aN_Delay,
                                 int     aN_Gate,
                                 DbleVec aMinAngle,
                                 DbleVec aMaxAngle,
                                 DbleVec aMinRange,
                                 DbleVec aMaxRange,
                                 double  aMinResp_dB,
                                 bool    aEffects)
{
   ndelay      = aN_Delay;
   ngate       = aN_Gate;
   effects     = aEffects;
   min_resp_db = aMinResp_dB;

   int igate = std::min(ngate, max_gates);

   min_angle.resize(igate);
   max_angle.resize(igate);
   min_range.resize(igate);
   max_range.resize(igate);

   // for (int i = 0; i < igate; ++i)
   //{
   //    min_angle[i] = min_angle[i];
   //    max_angle[i] = max_angle[i];
   //    min_range[i] = min_range[i];
   //    max_range[i] = max_range[i];
   // }
}

//--------------------------------------------------------------------
//! Compute the clutter power spectral density response for a group
//! of spectral lines that are symmetric about zero frequency.
//!
//! @param aN_Prfs      (input) The index of the mode containing the beam with the transmitter.
//! @param aFrequencies (ouput) (Hz) The frequencies of the clutter PSD output.
//! @param aPSD         (ouput) (unitless) The clutter Power Spectral Density (PSD) output.
//! @param aCount       (ouput) The number of spectral lines to compute.
//--------------------------------------------------------------------
// private
void WsfMTI_Filter::get_clutter_psd_mti(int aN_Prfs, DbleVec& aFrequencies, DbleDbleVec& aPSD, int& aCount)
{
   // TODO double fhigh = ceil(15.0 * this[0]->sigma3);
   double fhigh = ceil(15.0 * sigma3);
   double fincr = 0.5; // this interval should be small enough

   aCount = int((fhigh) / fincr) + 1;

   // allocate memory for the pointers
   aFrequencies.clear();
   aFrequencies.resize(aCount);
   aPSD.clear();
   aPSD.resize(aCount);
   for (int i = 0; i < aCount; ++i)
   {
      aPSD[i].resize(nprfs);
   }


   // compute each spectral line
   double freq = 0.0;
   int    iprf = 0;
   for (int i = 0; i < aCount; ++i)
   {
      aFrequencies[i] = freq;
      // TODO aPSD[i][iprf] = cltmti(this[iprf], freq);
      aPSD[i][iprf] = cltmti(freq);
      freq          = freq + fincr;
   }
   for (iprf = 1; iprf < nprfs; ++iprf)
   {
      for (int i = 0; i < aCount; ++i)
      {
         // TODO aPSD[i][iprf] = cltmti(this[iprf], aFrequencies[i]);
         aPSD[i][iprf] = cltmti(aFrequencies[i]);
      }
   }
}

//--------------------------------------------------------------------
// Compute response to discrete spectral lines
//--------------------------------------------------------------------
// private
void WsfMTI_Filter::spectral_response_mti(int      aPRF_Index,
                                          int      aCount,
                                          DbleVec  aFrequencies,
                                          DbleVec  aPSD,
                                          double&  aResponse,
                                          double&  aClutterResponse,
                                          DbleVec* aPSDout)
{
   // int imaxpsd = aPSD.size() - 1;

   // double fmax = aFrequencies[imaxpsd];

   // sum the filter responses
   double xsum = 0.0;
   if (aPSDout != nullptr)
   {
      aPSDout->resize(aCount);
      for (int i = 0; i < aCount; ++i)
      {
         double x       = resmti(aFrequencies[i]) * aPSD[i];
         aPSDout->at(i) = x;
         xsum           = xsum + x;
      }
   }
   else
   {
      for (int i = 0; i < aCount; ++i)
      {
         double x = resmti(aFrequencies[i]) * aPSD[i];
         xsum     = xsum + x;
      }
   }

   aResponse = xsum;

   // get the pre - computed clutter response
   aClutterResponse = clutter_resp;
}
