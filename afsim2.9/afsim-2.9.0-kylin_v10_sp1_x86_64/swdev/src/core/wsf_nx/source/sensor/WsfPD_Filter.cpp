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

#include "WsfPD_Filter.hpp"

#include <utility>

#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

namespace
{
const double cPD_FILTER_ROMBERG_EPS = 1.0E-4;
}

// =================================================================================================
WsfPD_Filter::WsfPD_Filter(bool aZVF_Flag /* = false*/, int aMax_PRFs /* = 16*/)
   : even_filter_cnt(true)
   , zero_align(false)
   , zvf_flag(aZVF_Flag)
   , zvf_sw(true)
   , max_prfs(aMax_PRFs)
   , nfilters(0)
   , npsd_lines(0)
   , psd_iprf(0)
   , npoles(7)
   , nprfs(0)
   , slr_abs(0.0)
   , slr_db(0.0)
   , const1(0.0)
   , const2(0.0)
   , sigma3(0.0)
   , decay_const(0.0)
   , conical_scan_rate(0.0)
   , clutter_response()
   , filter_bw()
   , prfhz()
   , max_center_freq()
   , noise_bw()
   , psd_freqs()
   , psd_lines()
   , unambig_range()
   , stop_freqs()
{
   if (zvf_flag)
   {
      nfilters = 1;
   }
}

// =================================================================================================
// virtual
WsfPD_Filter* WsfPD_Filter::Clone() const
{
   return new WsfPD_Filter(*this);
}

// virtual
bool WsfPD_Filter::Initialize(double  aConicalScanRate,
                              double  aDecayConst,
                              double  aIF_Bandwidth,
                              int     aN_PRFs,
                              DbleVec aPRF,
                              double  aPW,
                              double  aSigmaC)
{
   bool ok = true;

   if (nprfs < 1)
   {
      nprfs = aN_PRFs;
   }

   if (nprfs < 1)
   {
      ut::log::error() << "WsfPD_Filter::Initialize: Number of PRFs is 0, should be > 0.";
      return false;
   }
   alloc_data(false);

   if (nfilters < 1)
   {
      ut::log::error() << "WsfPD_Filter::Initialize: Number of Pulse Doppler filters is 0, should be > 0.";
      return false;
   }

   if ((npoles < 1) || (npoles > 7))
   {
      auto logger = ut::log::error()
                    << "WsfPD_Filter::Initialize: Number of filter poles is incorrect, should be between 1 and 7.";
      logger.AddNote() << "Number of filter poles: " << npoles;
      return false;
   }

   if (!(slr_db > 0.0))
   {
      ut::log::error() << "WsfPD_Filter::Initialize: Pulse Doppler filter sidelobe is < 0.0, should be > 0.0.";
      return false;
   }

   if (zvf_flag && zvf_sw)
   {
      PdBank pdBank;
      pdBank.filter_bw = filter_bw;
      pdBank.npoles    = npoles;
      pdBank.slr_db    = slr_db;
      ok &= init_zvf_pd(pdBank, aConicalScanRate, aDecayConst, aIF_Bandwidth, aN_PRFs, aPRF, aPW, aSigmaC /*, beamtxt*/);
   }
   else
   {
      ok &= init_pd(aConicalScanRate, aDecayConst, aIF_Bandwidth, aN_PRFs, aPRF, aPW, aSigmaC, false);
   }

   for (int i = 0; i < nprfs; ++i)
   {
      if (filter_bw[i] <= 0.0)
      {
         ut::log::error() << "WsfPD_Filter::Initialize: Pulse Doppler filter bandwidth is < 0.0, should be > 0.0.";
         return false;
      }
   }

   return ok;
}

// =================================================================================================
// virtual
bool WsfPD_Filter::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (zvf_flag)
   {
      if (command == "zvf_num_filter_poles")
      {
         aInput.ReadValue(npoles);
         aInput.ValueInClosedRange(npoles, 1, 7);
      }
      else if (command == "zvf_filter_slr")
      {
         aInput.ReadValueOfType(slr_abs, UtInput::cRATIO);
         slr_db = UtMath::SafeLinearToDB(slr_abs);
         aInput.ValueGreaterOrEqual(slr_db, 0.0);
      }
      else if (command == "zvf_filter_bandwidth")
      {
         // TODO check sizes with prfs in initialize
         int prfNum;
         aInput.ReadValue(prfNum);
         if (static_cast<int>(filter_bw.size() + 1) == prfNum)
         {
            filter_bw.resize(prfNum);
            aInput.ReadValueOfType(filter_bw[prfNum - 1], UtInput::cFREQUENCY);
         }
         else
         {
            throw UtInput::BadValue(aInput, "Filter BW #s must be in increasing order and match the PRF numbers.");
         }
      }
      else if (command == "zvf_switch")
      {
         aInput.ReadValue(zvf_sw);
      }
      else
      {
         myCommand = false;
      }
   }
   else
   {
      if (command == "pd_num_filter_poles")
      {
         aInput.ReadValue(npoles);
         aInput.ValueInClosedRange(npoles, 1, 7);
      }
      else if (command == "pd_filter_slr")
      {
         aInput.ReadValueOfType(slr_abs, UtInput::cRATIO);
         slr_db = UtMath::SafeLinearToDB(slr_abs);
         aInput.ValueGreaterOrEqual(slr_db, 0.0);
      }
      else if (command == "pd_filter_bandwidth")
      {
         // TODO check sizes with prfs in initialize
         int prfNum;
         aInput.ReadValue(prfNum);
         if (static_cast<int>(filter_bw.size() + 1) == prfNum)
         {
            filter_bw.resize(prfNum);
            aInput.ReadValueOfType(filter_bw[prfNum - 1], UtInput::cFREQUENCY);
            aInput.ValueGreaterOrEqual(filter_bw[prfNum - 1], 0.0);
         }
         else
         {
            throw UtInput::BadValue(aInput, "Filter BW #s must be in increasing order and match the PRF numbers.");
         }
      }
      else if (command == "pd_num_filters")
      {
         aInput.ReadValue(nfilters);
         aInput.ValueGreater(nfilters, 0);
      }
      else
      {
         myCommand = false;
      }
   }
   return myCommand;
}

//--------------------------------------------------------------------
// Initialize pulse Doppler object.
//--------------------------------------------------------------------
bool WsfPD_Filter::init_pd(double  aConicalScanRate,
                           double  aDecayConst,
                           double  aIF_Bandwidth,
                           int     aN_PRFs,
                           DbleVec aPRF,
                           double  aPW,
                           double  aSigmaC,
                           bool    aZeroAlign)
{
   bool ok = true;

   zero_align      = aZeroAlign;
   even_filter_cnt = (nfilters % 2 == 0);

   //------------------------------------------------------------------
   // Check to see that the number of prfs specified is less than the
   // maximum.
   //------------------------------------------------------------------
   if (aN_PRFs > max_prfs)
   {
      ut::log::error() << "WsfPD_Filter::Initialize: Error nprfs too big in init_pd.";
      return false;
   }

   //------------------------------------------------------------------
   // Allocate space for the clutter response array
   clutter_response.clear();
   clutter_response.resize(nfilters);
   for (int i = 0; i < nfilters; ++i)
   {
      clutter_response[i].resize(nprfs);
   }

   //------------------------------------------------------------------
   // For a pulsed Doppler recieiver, processing can be either analog
   // or digital.Analog assumes a fixed number of filters, and a fixed
   // bandwidth for each filter(inputs).For digital processing, the
   // number of filters is assumed fixed, however the bandwidth varies
   // according to the prf.
   //------------------------------------------------------------------

   nprfs = aN_PRFs;

   for (int iprf = 0; iprf < nprfs; ++iprf)
   {
      prfhz[iprf]         = aPRF[iprf];
      double duty_cycle   = prfhz[iprf] * aPW;
      unambig_range[iprf] = 0.5 * UtMath::cLIGHT_SPEED / prfhz[iprf];

      if (filter_bw[iprf] <= 0.0)
      {
         filter_bw[iprf] = prfhz[iprf] / static_cast<double>(nfilters);
      }

      // post a one time warning message if any duty cycle is >= 90 % thus
      // mimicking a CW radar.This is to flag old data sets that might
      // be trying to represent CW radars as special case pulse Doppler.
      bool duty_warning(true);
      if (duty_warning && (duty_cycle >= 0.9))
      {
         auto logger = ut::log::warning() << "The duty cycle for this PD radar exceeds 90%.";
         logger.AddNote() << "This may be an old data set trying to model a CW radar.";
         logger.AddNote() << "If so, then use the new CW radar type and its corresponding inputs.";
         duty_warning = false;
      }
   }

   //------------------------------------------------------------------
   // Set constants used for filter response computation
   //------------------------------------------------------------------

   set_pd_consts(aSigmaC, aDecayConst, aConicalScanRate);

   //------------------------------------------------------------------
   // Determine several quantities for use with the Chebychev filter.
   //
   // The XRIGHT formula is from "Theory And Design Of Linear Active
   // Networks", pp 109-114 by Sundaram Natarajan.  Copyright 1987 by
   // Macmillan Publishing Company.
   //------------------------------------------------------------------

   slr_abs       = pow(10.0, (0.1 * slr_db)) - 1.0;
   double sqrte2 = sqrt(slr_abs);
   double xleft  = 1.0;
   double temp   = sqrt((slr_abs - 1.0) / (pow(10, 0.05) - 1.0));
   double xright = log(temp + sqrt(pow(temp, 2) - 1.0));
   // double xright = acosh(sqrt((slr_abs - 1.0) / (pow(10, 0.05) - 1.0)));  // replaced with above two lines
   xright       = cosh(xright / static_cast<double>(npoles));
   double flow  = -15.0 * sigma3;
   double fhigh = 15.0 * sigma3;

   double xmidle;
   for (int iprf = 0; iprf < nprfs; ++iprf)
   {
      psd_iprf = iprf;

      double epslon = 1.0 / filter_bw[iprf];
      double deltax = xright - xleft;

      do
      {
         xmidle        = xleft + 0.5 * deltax;
         double chbmid = chebcf(xmidle, npoles);

         if (chbmid < sqrte2)
         {
            xleft = xmidle;
         }
         else
         {
            xright = xmidle;
         }
         deltax = xright - xleft;
      } while (deltax > epslon);

      stop_freqs[iprf] = 0.5 * xmidle * filter_bw[iprf];

      //----------------------------------------------------------------
      // Numerically integrate the transfer function of a Doppler
      // filter to determine its noise bandwidth.Note: this
      // transfer function is a symetric function.
      //----------------------------------------------------------------

      double freq1(0.0);
      double freq2 = aIF_Bandwidth;
      double center_freq(0.0);
      double stop_freq = stop_freqs[iprf];

      double x       = romchf(freq1, freq2, cPD_FILTER_ROMBERG_EPS, center_freq, stop_freq, cFT_CHFILT);
      noise_bw[iprf] = 2.0 * x;

      // Compute the first center frequency and the maximum center frequency.
      // Assume Doppler filters are symmetric about 0 Doppler.
      double first_center = -static_cast<double>(nfilters) * filter_bw[iprf] / 2.0;

      // if odd number of filters or not zero aligned.When not zero
      // aligned, an even number of filters will not have a filter
      // centered at zero Doppler.
      if (!even_filter_cnt || !zero_align)
      {
         first_center = first_center + filter_bw[iprf] / 2.0;
      }

      max_center_freq[iprf] = first_center + filter_bw[iprf] * static_cast<double>(nfilters - 1);

      //----------------------------------------------------------------
      // Determine the response of each Doppler filter to the
      // clutter's power spectral density.
      //----------------------------------------------------------------

      center_freq = first_center;
      double xcltf;
      for (int ifil = 0; ifil < nfilters; ++ifil)
      {
         if (npsd_lines > 0)
         {
            // when custom PSD data present
            xcltf = romchf(flow, fhigh, cPD_FILTER_ROMBERG_EPS, center_freq, stop_freq, cFT_CLUT_CUSTOM);
         }
         else
         {
            // use standard PSD
            xcltf = romchf(flow, fhigh, cPD_FILTER_ROMBERG_EPS, center_freq, stop_freq, cFT_CLTFPD);
         }
         clutter_response[ifil][iprf] = xcltf;

         center_freq = first_center + filter_bw[iprf] * (ifil + 1);
      }
   }

   return ok;
}

//--------------------------------------------------------------------
// Initialize a zero velocity filter pulse Doppler object.
//--------------------------------------------------------------------
// private
bool WsfPD_Filter::init_zvf_pd(PdBank  aPdBank,
                               double  aConicalScanRate,
                               double  aDecayConstant,
                               double  aIF_Bandwidth,
                               int     aN_PRFs,
                               DbleVec aPRF,
                               double  aPW,
                               double  aSigmaC/*,
                               char*   aBeamTxt*/)
{
   bool ok = true;

   // copy values from the pd_bank if they were not set from the inputs
   if (npoles == 0)
   {
      npoles = aPdBank.npoles;
   }
   if (slr_db <= 0.0)
   {
      slr_db = aPdBank.slr_db;
   }

   // for (size_t i = 0; i < aPdBank.filter_bw.size(); ++i)
   //{
   //    if ((i >= filter_bw.size()) ||
   //        (i < filter_bw.size() && filter_bw[i] <= 0.0))
   //    {
   //       filter_bw[i] = aPdBank.filter_bw[i];
   //    }
   // }

   // TODO AWK Test this section as compared to above section
   for (size_t i = 0; i < filter_bw.size(); ++i)
   {
      if (i < aPdBank.filter_bw.size() && filter_bw[i] <= 0.0)
      {
         filter_bw[i] = aPdBank.filter_bw[i];
      }
   }

   // call error_check(this, beam_txt)
   // call error_stop()

   // call the standard initialization routine
   ok &= init_pd(aConicalScanRate, aDecayConstant, aIF_Bandwidth, aN_PRFs, aPRF, aPW, aSigmaC, true);

   return ok;
}

//--------------------------------------------------------------------
// Compute the pulse Doppler filter response.
//--------------------------------------------------------------------
// public
double WsfPD_Filter::pd_response(double aF_Dop, int aFlag, int aPRF_Index)
{
   double pd_response(1.0);

   // check for zero velocity filter off
   if (zvf_flag && !zvf_sw)
   {
      // zvf is off there is no response
      return 0.0;
   }

   // return if signal is pre - processed target type
   if (aFlag == cST_PROCESSED_TARGET)
   {
      return pd_response;
   }

   //----------------------------------------------------------------
   // This is a pulsed Doppler radar, determine the aliased Doppler
   // frequency to the range of + /a PRF / 2.
   //----------------------------------------------------------------

   double fdoprf = aF_Dop - int(aF_Dop / prfhz[aPRF_Index]) * prfhz[aPRF_Index];
   double falias(0.0);
   if (std::abs(fdoprf) > 0.5 * prfhz[aPRF_Index])
   {
      falias = (prfhz[aPRF_Index] - std::abs(fdoprf)) * UtMath::Sign(1.0, fdoprf);
   }
   else // if (std::abs(fdoprf).le. 0.5d0 * this%prfhz(iprf))
   {
      falias = fdoprf;
   }

   //------------------------------------------------------------------
   // Calculate the center frequency of the Doppler filter
   // corresponding to the aliased Doppler frequency.
   //------------------------------------------------------------------

   int    nfil = -1;
   double x    = max_center_freq[aPRF_Index] + filter_bw[aPRF_Index] / 2.0;
   double center_freq;
   if (std::abs(falias) > x && even_filter_cnt && zero_align)
   {
      // even number of filters and target in folded first filter
      center_freq = UtMath::Sign(max_center_freq[aPRF_Index] + filter_bw[aPRF_Index], falias);
      nfil        = 0;
   }
   else
   {
      x = max_center_freq[aPRF_Index] - filter_bw[aPRF_Index] / 2.0;
      if (std::abs(falias) >= x)
      {
         center_freq = UtMath::Sign(max_center_freq[aPRF_Index], falias);
      }
      else
      {
         x = max_center_freq[aPRF_Index] - static_cast<double>(UtMath::Round(static_cast<float>(
                                              (max_center_freq[aPRF_Index] - std::abs(falias)) / filter_bw[aPRF_Index]))) *
                                              filter_bw[aPRF_Index];

         center_freq = UtMath::Sign(x, falias);
      }
   }

   //------------------------------------------------------------------
   // Set the filter's stop frequency for the pulse repetition
   // frequency.
   //------------------------------------------------------------------

   double stop_freq = stop_freqs[aPRF_Index];

   if (aFlag == cST_CLUTTER)
   {
      //----------------------------------------------------------------
      // Determine a the response of the Doppler filter containing the
      // target signal to the clutter's power spectral density.  A bias
      // factor of 0.01 is added to prevent rounding from selecting the
      // wrong filter.The basic equation should alway result in a
      // whole number, but rounding may result in a value slightly less
      // than the number.This would result in the wrong filter being
      // selected.
      //----------------------------------------------------------------
      if (nfil < 0)
      {
         x    = (center_freq + max_center_freq[aPRF_Index]) / filter_bw[aPRF_Index];
         nfil = int(x + 0.01);
         if (zero_align && even_filter_cnt)
         {
            nfil = nfil + 1;
         }
      }

      // ensure that nfil is valid.
      if (nfil < 0 || nfil > nfilters)
      {
         auto logger = ut::log::error() << "pd_response Invalid filter number calculated.";
         logger.AddNote() << "nfil: " << nfil;
         logger.AddNote() << "center_freq: " << center_freq;
         logger.AddNote() << "max_center_freq: " << max_center_freq[aPRF_Index];
         logger.AddNote() << "filter_bw: " << filter_bw[aPRF_Index];
         logger.AddNote() << "iprf: " << aPRF_Index;

         throw UtException("WsfPD_Filter::pd_response");
      }

      pd_response = clutter_response[nfil][aPRF_Index];
   }
   else if (aFlag == cST_NOISE_JAM)
   {
      pd_response = 1.0;
   }
   else
   {
      //----------------------------------------------------------------
      // Determine the response of the Doppler filter at the Doppler
      // shift.
      //----------------------------------------------------------------
      pd_response = chfilt(falias, center_freq, stop_freq);
   }
   return pd_response;
}

//--------------------------------------------------------------------
// Compute the response of a Chebyshev filter to an input signal
// using Romberg integration.
// private
double WsfPD_Filter::romchf(double a, double b, double eps, double center_freq, double stop_freq, FilterFunc aFilterFunc)
{
   const int maxtrp(25);
   double    trzoid[maxtrp] = {0.0};
   double    romchf(0.0);

   //------------------------------------------------------------------
   // If B is less than A, interchange A and B and change the sign
   // of the integral.
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

   trzoid[0] =
      hover2 * (filt_func(ax, center_freq, stop_freq, aFilterFunc) + filt_func(bx, center_freq, stop_freq, aFilterFunc));

   //------------------------------------------------------------------
   // Determine TRZOID(N), the N th trapezoidal approximation for
   // N = 2, 3, ..., MAXTRP.
   //------------------------------------------------------------------

   int nterm = 1;
   for (int n = 1; n < maxtrp; ++n)
   {
      double x    = ax + hover2;
      double sum  = filt_func(x, center_freq, stop_freq, aFilterFunc);
      int    nsum = nterm - 1;

      for (int k = 1; k <= nsum; ++k)
      {
         x   = x + h;
         sum = sum + filt_func(x, center_freq, stop_freq, aFilterFunc);
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
            romchf = trzoid[i - 1] * sign;
            return romchf;
         }
      }

      nterm  = 2 * nterm;
      h      = hover2;
      hover2 = 0.5 * h;
   }

   //------------------------------------------------------------------
   // Entrance to this section of the subroutine indicates the
   // relative error tolerance was not achieved.Either the relative
   // error tolerance must be increased, or the parameter MAXTRP must
   // be increased in the 'PARAMETER' statement at the beginning of
   // the subroutine.
   //------------------------------------------------------------------
   auto logger = ut::log::warning() << "ROMCHF Function";
   logger.AddNote()
      << "ROMCHF(Romberg integration routine) was unable to return an answer with the input relative error tolerance.";
   logger.AddNote() << "The input relative error tolerance must be increased or the parameter MAXTRP in the PARAMETER "
                       "statement must be increased.";
   throw UtException("WsfPD_Filter::romchf");
   return romchf;
}

double WsfPD_Filter::filt_func(double aFrequency, double aCenterFrequency, double aStopFrequency, FilterFunc aFilterFunc)
{
   double value(0.0);
   switch (aFilterFunc)
   {
   case WsfPD_Filter::cFT_CLTFPD:
      value = cltfpd(aFrequency, aCenterFrequency, aStopFrequency);
      break;
   case WsfPD_Filter::cFT_CLUT_CUSTOM:
      value = clut_custom(aFrequency, aCenterFrequency, aStopFrequency);
      break;
   case WsfPD_Filter::cFT_CHFILT:
      value = chfilt(aFrequency, aCenterFrequency, aStopFrequency);
      break;
   default:
      break;
   }
   return value;
}

//--------------------------------------------------------------------
// Calculate the magnitude of a single spectral line of clutter in
// a pulse Doppler filter assuming a Gaussian probability spectral
// density out to 3 sigma with a frequency based decay beyond 3
// sigma.
//--------------------------------------------------------------------
// private
double WsfPD_Filter::cltfpd(double aFrequency, double aCenterFrequency, double aStopFrequency)
{
   double deltaf = aFrequency - conical_scan_rate;
   double delfsq = deltaf * deltaf;

   double ux = chfilt(aFrequency, aCenterFrequency, aStopFrequency);

   double tx(0.0);
   if (std::abs(deltaf) < sigma3)
   {
      tx = const1 * exp(const2 * delfsq);
   }
   else
   {
      tx = std::max(const1 * exp(const2 * delfsq), decay_const / delfsq);
   }

   return tx * ux;
}

//--------------------------------------------------------------------
// Calculate the magnitude of a single spectral line of clutter in
// a pulse Doppler filter using a custom set of spectral lines.
//--------------------------------------------------------------------
// private
double WsfPD_Filter::clut_custom(double aFrequency, double aCenterFrequency, double aStopFrequency)
{
   double deltaf = aFrequency - conical_scan_rate;

   double ux = chfilt(aFrequency, aCenterFrequency, aStopFrequency);

   // if freqs are symmetric about zero to right
   if (psd_freqs[0] == 0.0)
   {
      deltaf = std::abs(deltaf);
   }
   // if freqs are symmetric about zero to left
   if (psd_freqs[npsd_lines] == 0.0)
   {
      deltaf = -std::abs(deltaf);
   }

   double tx(0.0);
   if (deltaf <= psd_freqs[0])
   {
      tx = psd_lines[0][psd_iprf];
   }
   else if (deltaf >= psd_freqs[npsd_lines])
   {
      tx = psd_lines[npsd_lines][psd_iprf];
   }
   else
   {
      int i = position(npsd_lines, psd_freqs, deltaf);
      // interpolate
      double xfact = (deltaf - psd_freqs[i]) / (psd_freqs[i + 1] - psd_freqs[i]);
      tx           = psd_lines[i][psd_iprf] * (1.0 - xfact) + psd_lines[i + 1][psd_iprf] * xfact;
   }
   return tx * ux;
}

//--------------------------------------------------------------------
// Calculate the frequency response of a Chebychev filter.
//
// Inputs :
// DOPFRQ The Doppler frequency(Hz).
//
// STPFRQ The bandstop frequency(Hz) of the Chebychev
// filter.
//
// SLR_ABS A filter factor which is related to the filter
// main lobe to side lobe ratio.
//
// CENTER_FREQ The center frequency(Hz) of the filter
// containing the Doppler frequency.
//
// Outputs:
// The filter response(Absolute units) of the
// Chebychev filter.
//--------------------------------------------------------------------
// private
double WsfPD_Filter::chfilt(double aDopplerFrequency, double aCenterFrequency, double aStopFrequency)
{
   //------------------------------------------------------------------
   // Determine ABSFRQ, the distance from the target Doppler
   // frequency to the center of the filter containing the target
   // signal.
   //------------------------------------------------------------------

   double absfrq = std::abs(aCenterFrequency - aDopplerFrequency);

   //------------------------------------------------------------------
   // If the target Doppler is near enough to the center frequency of
   // the Chebychef filter containing the signal, the filter response
   // may be regarded as one.Otherwise, the filter response must be
   // determined.
   //------------------------------------------------------------------

   double chfilt(1.0);
   if (absfrq > 0.05 * aStopFrequency)
   {
      //----------------------------------------------------------------
      // The target Doppler is far enough from the center frequency of
      // the Chebychef filter that the filter response should not be
      // regarded as one.Determine CHFILT, the filter response.
      //----------------------------------------------------------------

      double x      = aStopFrequency / absfrq;
      double cheb   = chebcf(x, npoles);
      double chbsqr = cheb * cheb;
      chfilt        = 1.0 / (1.0 + slr_abs / chbsqr);
   }
   // else
   //{
   //    //----------------------------------------------------------------
   //    // The target Doppler is near enough to the center frequency of
   //    // the Chebychef filter that the filter response may be regarded
   //    // as one. Set CHFILT to one.
   //    //----------------------------------------------------------------
   //    chfilt = 1.0;
   // }

   return chfilt;
}

//--------------------------------------------------------------------
// Calculate the value of a Chebychev polynomial of degree one
// through seven for use in determining the response of a
// Chebychev filter.
//
// The recursive formulas used may be found in
// 'Numerical Methods,' by Germund Dahlquist and Ake Bjorck,
// Prentice Hall, Inc., Copyright 1974, p. 105.
//
// Inputs:
// X The point at which the Chebychev polynomial
// is to be evaluated.
//
// NPOLES The number of poles in the Chebychev filter.
//
// Outputs:
// CHEBCF The value of the Chebychev polynomial.
//--------------------------------------------------------------------
// private
double WsfPD_Filter::chebcf(double aX, double aN_Poles)
{
   //------------------------------------------------------------------
   // Branch according to NPOLES, the number of poles in the
   // Chebychef filter, or equivalently, the degree of the
   // Chebychef polynomial.
   //------------------------------------------------------------------
   double chebcf(0.0);
   if (aN_Poles == 7)
   {
      //----------------------------------------------------------------
      // Seventh degree Chebychev polynomial.
      //----------------------------------------------------------------
      double xsquar = 4.0 * pow(aX, 2);
      double y      = xsquar - 3.5;
      chebcf        = aX * (xsquar * (pow(y, 2) + 1.75) - 7.0);
   }
   else if (npoles == 6)
   {
      //----------------------------------------------------------------
      // Sixth degree Chebychev polynomial.
      //----------------------------------------------------------------
      double xsquar = 2.0 * pow(aX, 2);
      double y      = 2.0 * xsquar - 3.0;
      chebcf        = xsquar * pow(y, 2) - 1.0;
   }
   else if (npoles == 5)
   {
      //----------------------------------------------------------------
      // Fifth degree Chebychev polynomial.
      //----------------------------------------------------------------
      double y = 4.0 * pow(aX, 2) - 2.5;
      chebcf   = aX * (pow(y, 2) - 1.25);
   }
   else if (npoles == 4)
   {
      //----------------------------------------------------------------
      // Fourth degree Chebychev polynomial.
      //----------------------------------------------------------------
      double xsquar = 2.0 * pow(aX, 2);
      chebcf        = 2.0 * xsquar * (xsquar - 2.0) + 1.0;
   }
   else if (npoles == 3)
   {
      //----------------------------------------------------------------
      // Third degree Chebychev polynomial.
      //----------------------------------------------------------------
      double twox = aX + aX;
      chebcf      = twox * (twox * aX - 1.5);
   }
   else if (npoles == 2)
   {
      //----------------------------------------------------------------
      // Second degree Chebychev polynomial.
      //----------------------------------------------------------------
      chebcf = 2.0 * pow(aX, 2) - 1.0;
   }
   else if (npoles == 1)
   {
      //----------------------------------------------------------------
      // First degree Chebychev polynomial.
      //----------------------------------------------------------------
      chebcf = aX;
   }
   return chebcf;
}

//--------------------------------------------------------------------
// Set the pulse Doppler constants that a based on the standard
// deviation of the Gaussian portion of the clutter power spectral
// density(sigmac).Often this is referred to as the RMS clutter
// frequency spread.
//--------------------------------------------------------------------
// private
void WsfPD_Filter::set_pd_consts(double aSigmaC, double aDecayConst, double aConicalScanRate)
{
   //------------------------------------------------------------------
   // Determine several terms for use in pulse Doppler routines for
   // pulsed radars.
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
void WsfPD_Filter::set_pd_data(int aN_Filters, int aN_Poles, int aN_PRFs, double aSLR_dB, DbleVec& aFilterBW)
{
   nfilters = aN_Filters;
   npoles   = aN_Poles;
   slr_db   = aSLR_dB;

   filter_bw.resize(std::min(std::max(nprfs, 1), max_prfs));
   for (int i = 0; i < std::min(std::max(nprfs, 1), max_prfs); ++i)
   {
      filter_bw[i] = aFilterBW[i];
   }
}

//--------------------------------------------------------------------
// public
double WsfPD_Filter::get_stop_freq(int aPRF_Index)
{
   return stop_freqs[aPRF_Index];
}

//--------------------------------------------------------------------
// public
double WsfPD_Filter::noise_bandwidth_pd(int aPRF_Index)
{
   return noise_bw[aPRF_Index];
}

//--------------------------------------------------------------------
// public
double WsfPD_Filter::filter_bandwidth_pd(int aPRF_Index)
{
   return filter_bw[aPRF_Index];
}


//--------------------------------------------------------------------
// Set customized clutter power spectral lines.
//--------------------------------------------------------------------
// private
void WsfPD_Filter::set_clutter_psd_pd(DbleVec& aFrequencies, DbleDbleVec& aPSD, int aCount, int aN_PRFs)
{
   npsd_lines = aCount;

   psd_freqs = aFrequencies;
   psd_lines = aPSD;
}

//--------------------------------------------------------------------
// Locate the position in the increasing, ordered array
// where "value" is between two elements of the array. This
// is accomplished with a bi - sectional search. Values beyond the
// extremes returns the extreme.
//--------------------------------------------------------------------
// private
int WsfPD_Filter::position(int aCount, DbleVec& aArray, double aValue)
{
   int ileft  = 0;
   int iright = aCount - 1;

   // bi - sectional search
   do
   {
      int imiddle = (ileft + iright) / 2;
      if (aValue >= aArray[imiddle])
      {
         ileft = imiddle;
      }
      else
      {
         iright = imiddle;
      }
   } while (iright != ileft + 1);
   return ileft;
}

void WsfPD_Filter::alloc_data(bool aDoDealloc /* = true*/)
{
   if (aDoDealloc)
   {
      filter_bw.clear();
      prfhz.clear();
      max_center_freq.clear();
      noise_bw.clear();
      unambig_range.clear();
      stop_freqs.clear();
   }

   if (nprfs < 1)
   {
      return;
   }

   bool filtResize = false;
   if (static_cast<int>(filter_bw.size()) != nprfs)
   {
      filter_bw.resize(nprfs);
      filtResize = true;
   }
   prfhz.resize(nprfs);
   max_center_freq.resize(nprfs);
   noise_bw.resize(nprfs);
   unambig_range.resize(nprfs);
   stop_freqs.resize(nprfs);

   for (int i = 0; i < nprfs; ++i)
   {
      if (filtResize)
      {
         filter_bw[i] = 0.0;
      }
      prfhz[i]           = 0.0;
      max_center_freq[i] = 0.0;
      noise_bw[i]        = 0.0;
      unambig_range[i]   = 0.0;
      stop_freqs[i]      = 0.0;
   }
}


//--------------------------------------------------------------------
// Compute response to discrete spectral lines
//--------------------------------------------------------------------
// private
void WsfPD_Filter::spectral_response_pd(int     aPRF_Index,
                                        int     aCount,
                                        DbleVec aFrequencies,
                                        DbleVec aPSD,
                                        int     aMaxPSD_Index,
                                        double& aResponse,
                                        double& aClutterResponse)
{
   int    nfil(0);
   double falias(0.0);
   double center_freq(0.0);
   double stop_freq(0.0);
   double x(0.0);
   double xsum = 0.0;

   // check for zero velocity filter off
   if (zvf_flag && !zvf_sw)
   {
      // zvf is off there is no response
      aResponse = 0.0;
      return;
   }

   int i = static_cast<int>(aFrequencies.size()) - 1;
   if ((aMaxPSD_Index < 0) || (aMaxPSD_Index > i))
   {
      auto logger = ut::log::error() << "Invalid IMAXPSD.";
      logger.AddNote() << "Provided: " << aMaxPSD_Index;
      logger.AddNote() << "Expected: 0 <= IMAXPSD <= " << i << " (frequency count)";
      throw UtException("spectral_response_pd");
   }

   // frequency of maximum PSD response
   double fmax = aFrequencies[aMaxPSD_Index];

   // determine the filter containing maximum PSD
   compute_filter_parms_pd(fmax, aPRF_Index, falias, center_freq, stop_freq, nfil);

   ////sum the filter with peak response to all PSD values
   for (i = 0; i < aCount; ++i)
   {
      x    = chfilt(aFrequencies[i], center_freq, stop_freq) * aPSD[i];
      xsum = xsum + x;
   }

   // Sum the max PSD value through the filter's bandwidth for a
   // cumulative response.
   double f = center_freq - filter_bw[aPRF_Index] / 2.0;
   fmax     = f + filter_bw[aPRF_Index];

   do
   {
      x    = chfilt(f, center_freq, stop_freq);
      xsum = xsum + aPSD[aMaxPSD_Index] * x / filter_bw[aPRF_Index];
      f    = f + 1.0;
   } while (f <= fmax);

   // save the summed response
   aResponse = xsum;

   // get the clutter response for the filter with the max PSD
   aClutterResponse = clutter_response[nfil][aPRF_Index];
}


//--------------------------------------------------------------------
// Compute aliased frequency, center frequency, stop frequency
// and filter number that corresponds to FDOP.
//--------------------------------------------------------------------
// private
void WsfPD_Filter::compute_filter_parms_pd(double  aDopplerFrequency,
                                           int     aPRF_Index,
                                           double& aFrequencyAlias,
                                           double& aCenterFrequency,
                                           double& aStopFrequency,
                                           int&    aN_Filters)
{
   //----------------------------------------------------------------
   // This is a pulsed Doppler radar, determine the aliased Doppler
   // frequency to the range of + / -a PRF / 2.
   //----------------------------------------------------------------

   double fdoprf = aDopplerFrequency - int(aDopplerFrequency / prfhz[aPRF_Index]) * prfhz[aPRF_Index];

   if (std::abs(fdoprf) > 0.5 * prfhz[aPRF_Index])
   {
      aFrequencyAlias = (prfhz[aPRF_Index] - std::abs(fdoprf)) * UtMath::Sign(1.0, fdoprf);
   }
   else // if (std::abs(fdoprf) <= 0.5 * prfhz[aPRF_Index])
   {
      aFrequencyAlias = fdoprf;
   }

   //------------------------------------------------------------------
   // Calculate the center frequency of the Doppler filter
   // corresponding to the aliased Doppler frequency.
   //------------------------------------------------------------------

   aN_Filters = -1;
   double x   = max_center_freq[aPRF_Index] + filter_bw[aPRF_Index] / 2.0;
   if (std::abs(aFrequencyAlias) > x && even_filter_cnt && zero_align)
   {
      // even number of filters and target in folded first filter
      aCenterFrequency = UtMath::Sign(max_center_freq[aPRF_Index] + filter_bw[aPRF_Index], aFrequencyAlias);
      aN_Filters       = 1;
   }
   else
   {
      x = max_center_freq[aPRF_Index] - filter_bw[aPRF_Index] / 2.0;
      if (std::abs(aFrequencyAlias) >= x)
      {
         aCenterFrequency = UtMath::Sign(max_center_freq[aPRF_Index], aFrequencyAlias);
      }
      else
      {
         x = max_center_freq[aPRF_Index] -
             static_cast<double>(UtMath::Round(
                static_cast<float>((max_center_freq[aPRF_Index] - std::abs(aFrequencyAlias)) / filter_bw[aPRF_Index]))) *
                filter_bw[aPRF_Index];

         aCenterFrequency = UtMath::Sign(x, aFrequencyAlias);
      }

      x          = (aCenterFrequency + max_center_freq[aPRF_Index]) / filter_bw[aPRF_Index];
      aN_Filters = int(x + 0.01) + 1;
   }

   // ensure that nfil is valid.
   if (aN_Filters < 1 || aN_Filters > nfilters)
   {
      auto logger = ut::log::error() << "Invalid filter number calculated.";
      logger.AddNote() << "nfil: " << aN_Filters;
      logger.AddNote() << "center_freq: " << aCenterFrequency;
      logger.AddNote() << "max_center_freq: " << max_center_freq[aPRF_Index];
      logger.AddNote() << "filter_bw: " << filter_bw[aPRF_Index];
      logger.AddNote() << "iprf: " << aPRF_Index;
      throw UtException("compute_filter_parms_pd");
   }

   //------------------------------------------------------------------
   // Set the filter's stop frequency for the pulse repetition
   // frequency.
   //------------------------------------------------------------------
   aStopFrequency = stop_freqs[aPRF_Index];
}
