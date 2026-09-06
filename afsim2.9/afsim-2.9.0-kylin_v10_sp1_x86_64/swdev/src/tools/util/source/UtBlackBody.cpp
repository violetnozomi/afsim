// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtBlackBody.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "UtLog.hpp"
#include "UtMath.hpp"

namespace
{
// c1 - First radiation constant = 2 * pi * h * c^2
const double cC1 = 3.741771790E-16; // W * m^2 (2014 NIST)

// c1L - First radiation constant for spectral radiance = 2 * h * c^2 = c1 / pi
const double cC1L = 1.191042953E-16; // W * m^2 / sr (2014 NIST)

// c2 - Second radiation constant = h * c / k
const double cC2 = 1.4387752E-2; // m * deg-K (2014 NIST)

// k or k_B - Boltzmann constant
const double cK = 1.38064852E-23; // J / deg-K (2014 NIST)

// sigma - Stefan-Boltzmann constant = (2 * pi^5 * k_B^4) / (15 * c^2 * h^3) or (pi^2 /60) / (k^4/(h-bar^3 * c^2))
const double cSIGMA = 5.670367E-8; // W / m^2 / K^4 (2014 NIST)

// These are used by TestRadiance.
//
// IT IS NOT NECESSARY TO WORRY ABOUT MULTI-THREADING!!! During normal execution this may be written by multiple threads
// - but they are never used. They are read only my TestRadiance which is used only in a single-thread.

double sLastX[2];
double sLastSum[2];
int    sLastIteration[2];
double sLastTerm[2];

// These are the descriptions of the wave bands for TestRadiance.
struct BandDescriptor
{
   const char* mName;
   double      mLowerWavelength;
   double      mUpperWavelength;
};

// The band descriptors for the wave bands tested by TestRadiance
const BandDescriptor cBAND_DESC[] = {
   {"uv-c", 100.0E-9, 280.0E-9},
   {"uv-b", 280.0E-9, 315.0E-9},
   {"uv-a", 315.0E-9, 380.0E-9}, // UV A normally ends at 400nm, but we had already defined visual to tart at 380nm
   {"visual", 380.0E-9, 760.0E-9},
   {"ir-short", 1.0E-6, 3.0e-6},
   {"ir-medium", 3.0E-6, 5.0E-6},
   {"ir-long", 8.0E-6, 12.0E-6},
   {"ir-very_long", 15.0E-6, 30.0E-6},
   {"", -1.0, -1.0} // Signifies the end of the table.
};

// =================================================================================================
//! Compute the one-sided integral of the Planck blackbody equation using the Widger-Woodman algorithm.
//! The Widger and Woodman algorithm described in the 'Bulletin American Meteorological Association, Vol. 57, No. 10,
//! October 1976, pp 1217-1219' uses substitution, binomial series expansion and integration by parts to
//! evaluate the one-sided integral (from the input wavelength up to infinity) of the Planck blackbody equation.
//! @param aTemperature Temperature of the black body (deg K)
//! @param aLowerWavelength The lower integration bound. The upper bound is assumed to be infinity.
//! @param aSaveIndex The index for storing internal debugging data.
double Integrate_WW(double aTemperature, double aLowerWavelength, size_t aSaveIndex)
{
   // Informal test show works well for the optical spectrum (1 nm - 1000 um) and for temperatures less than 10,000 K.
   // It begins to degrade as the wavelength and temperature (actually, the product of the two) increase. Since we are
   // only interested in the optical spectrum and temperatures of 6000K or less.
   //
   // Near the end of the paper it defined a substitution that eliminated the first term of the sum. They indicated it
   // required fewer iterations for the same accuracy although experiment found to be more volatile than in the UV
   // region at low temperatures. The radiances are very small here (< 1E-10 w/m^2/sr), so it probably doesn't matter.
   // And at the other regions it just didn't improve performance all that much. All things considered it was decided
   // not use the additional substitution.

   double x   = cC2 / (aTemperature * aLowerWavelength);
   double t1  = x * x * x;
   double t2  = 3.0 * x * x;
   double t3  = 6.0 * x;
   double t4  = 6.0;
   double sum = 0.0;
   double t   = 0.0;
   int    n   = 1;
   while (n < 100)
   {
      double rn     = 1.0 / n;
      double rn2    = rn * rn;
      t             = exp(-n * x) * rn * (t1 + (t2 * rn) + (t3 * rn2) + (t4 * rn * rn2));
      double oldSum = sum;
      sum += t;
      if ((sum == oldSum) || (fabs(t / sum) < 1.0E-9))
      {
         break;
      }
      ++n;
   }

   // It looks like the following would break multi-threading (multiple threads could be saving into the same area
   // simultaneously). It really doesn't matter because the data is only used by TestRadiance for validation purposes,
   // and that runs only in single thread.
   sLastX[aSaveIndex]         = x;
   sLastSum[aSaveIndex]       = sum;
   sLastTerm[aSaveIndex]      = t;
   sLastIteration[aSaveIndex] = n;

   return sum;
}
} // namespace

// =================================================================================================
//! Compute the radiance of a black body at a specified temperature within the specified band of wavelengths.
//! This version uses trapezoidal integration to directly integrate the Planck black body equation.
//! @ote Use of BandRadiance is recommended if the temperature and wavelengths meet the conditions for which
//! it is accurate. Use this method only if necessary as it is much slower.
//! @param aTemperature (degK)
//! @param aLowerWavelength of the region of interest (m)
//! @param aUpperWavelength of the region of interest (m)
//! @param aStepSize The integration step size (m). If zero an appropriate step size will be chosen.
//! @returns The radiance (W/m^2/sr)
double ut::BlackBody::BandRadiance_Direct(double aTemperature, double aLowerWavelength, double aUpperWavelength, double aStepSize)
{
   double M_sum      = 0.0;
   double wavelength = aLowerWavelength;
   double M_1        = SpectralRadiance(aTemperature, wavelength);
   while (wavelength < aUpperWavelength)
   {
      double stepSize = aStepSize;
      if (stepSize <= 0.0)
      {
         // Use 1 nm in the visual spectrum, 10 nm in the IR spectrum
         stepSize = 1.0E-8; // 0.01 um
         if (wavelength < 1.0E-6)
         {
            stepSize = 1.0E-9; // 1 nm
         }
      }
      double h   = std::min(stepSize, aUpperWavelength - wavelength);
      double M_2 = SpectralRadiance(aTemperature, wavelength + h);
      M_sum += 0.5 * (M_1 + M_2) * h;
      M_1 = M_2;
      wavelength += stepSize;
   }
   return M_sum;
}

// =================================================================================================
//! Compute the radiance of a black body at a specified temperature within the specified band of wavelengths.
//!
//! This uses the algorithm defined by Widger and Woodman in 'Bulletin American Meteorological Association, Vol. 57, No.
//! 10, October 1976, pp 1217-1219. It uses variable substitution, series expansion and integration by parts to
//! integrate the Planck black body equation over a band of wavelengths.
//!
//! This is much faster than BandRadiance_Direct, but is accurate only for temperatures from 100K to 5800K and
//! wavelengths from 100nm to 1000um - which happily is the primary region of application.
//!
//! @param aLowerWavelength of the region of interest (m)
//! @param aUpperWavelength of the region of interest (m)
//! @param aTemperature (degK)
//! @returns The radiance (W/m^2/sr)
double ut::BlackBody::BandRadiance(double aTemperature, double aLowerWavelength, double aUpperWavelength)
{
   // Compute C1 * T^4 / C2L^4
   double T_OverC2 = aTemperature / cC2;
   double mult     = cC1L * T_OverC2 * T_OverC2 * T_OverC2 * T_OverC2; // C1 * T^4 / C2^4
   return mult * (Integrate_WW(aTemperature, aUpperWavelength, 0) - Integrate_WW(aTemperature, aLowerWavelength, 1));
}

// =================================================================================================
//! Compute the spectral radiance of a blackbody at a specified temperature for a specified wavelength.
//! @param aTemperature The body temperature (deg-K)
//! @param aWavelength  The wavelength of interest (m)
//! @returns The spectral radiance (W/sr/m^2/m).
double ut::BlackBody::SpectralRadiance(double aTemperature, double aWavelength)
{
   // Plank's law for spectral radiance
   double wavelength_2 = aWavelength * aWavelength;
   double t1           = cC1L / (aWavelength * wavelength_2 * wavelength_2);
   double t2           = 1.0 / expm1(cC2 / (aWavelength * aTemperature));
   return t1 * t2;
}

// =================================================================================================
//! Compute the temperature of a blackbody with the specified radiance using the Stefan-Boltzmann law.
//! @param aRadiance The radiance of the body (W/m^2/sr)
//! @returns The blackbody temperature (deg-K)
double ut::BlackBody::Temperature(double aRadiance)
{
   return pow(aRadiance * UtMath::cPI / cSIGMA, 0.25);
}

// =================================================================================================
//! Compute the radiance of a blackbody at the specified temperature using the Stefan-Boltzmann law.
//! The Stefan-Boltzmann law integrates Planck's blackbody radiation equation over ALL wavelengths.
//! @param aTemperature The temperature of the blackbody (deg-K)
//! @returns The radiance of the blackbody (w/m^2/sr)
double ut::BlackBody::TotalRadiance(double aTemperature)
{
   double T_2 = aTemperature * aTemperature;
   return (T_2 * T_2) * cSIGMA / UtMath::cPI;
}

// =================================================================================================
//! Execute the black body radiance integration methods for selected temperatures and wavebands and print a report.
//! This was just during develop, but is being kept in case it ever needs to be done again. This simply
//! be called by some other method.
void ut::BlackBody::TestRadiance(std::ostream& aOut)
{
   static const int temps[] = {10, 173, 393, 1000, 5772, -1};

   std::ios::fmtflags savedFlags     = aOut.flags();
   std::streamsize    savedPrecision = aOut.precision();

   for (size_t tempIndex = 0; temps[tempIndex] > 0; ++tempIndex)
   {
      double T = temps[tempIndex];
      ut::log::info() << "Temperature: " << std::fixed << std::setprecision(2) << T;
      for (size_t bandIndex = 0; cBAND_DESC[bandIndex].mLowerWavelength >= 0.0; ++bandIndex)
      {
         const char* name            = cBAND_DESC[bandIndex].mName;
         double      lowerWavelength = cBAND_DESC[bandIndex].mLowerWavelength;
         double      upperWavelength = cBAND_DESC[bandIndex].mUpperWavelength;
         double      L1              = BandRadiance_Direct(T, lowerWavelength, upperWavelength, 0.25E-9);
         double      L2              = BandRadiance(T, lowerWavelength, upperWavelength);
         double      unitScale       = 1.0E+6;
         std::string unitName("um");
         if (upperWavelength < 1.0E-6)
         {
            unitScale = 1.0E+9;
            unitName  = "nm";
         }
         aOut << std::setw(16) << name;
         aOut << "    " << std::fixed << std::setw(7) << std::setprecision(1) << lowerWavelength * unitScale << ' '
              << unitName;
         aOut << "    " << std::fixed << std::setw(7) << std::setprecision(1) << upperWavelength * unitScale << ' '
              << unitName;
         aOut << "    " << std::scientific << std::setw(17) << std::setprecision(10) << L1;
         aOut << "    " << std::scientific << std::setw(17) << std::setprecision(10) << L2;
         // Print data about the one-sided integrals... (see Integrate_WW)
         aOut << std::setprecision(6);
         aOut << " :" << sLastSum[0] << ' ' << sLastX[0] << ' ' << std::setw(3) << sLastIteration[0] << ' '
              << sLastTerm[0];
         aOut << " :" << sLastSum[1] << ' ' << sLastX[1] << ' ' << std::setw(3) << sLastIteration[1] << ' '
              << sLastTerm[1];
         aOut << '\n';
      }
   }
   aOut.flags(savedFlags);
   aOut.precision(savedPrecision);
}

// =================================================================================================
// This is being kept around for potential future development. I believe we can generate very small tables
// that will supply the band radiance with acceptable accuracy without iteration. (JAJ)
#if 0
void Dummy()
{
   std::ofstream codeOfs("band_data.txt");
   std::ofstream plotOfs("band_plot.csv");
   std::vector<double> temps;
   for (int i = 100; i <= 1000; i+= 20)
   {
      temps.push_back(i);
   }
   for (int i = 1000; i <= 6000; i += 20)
   {
      temps.push_back(i);
   }
   for (size_t iTemp = 0; iTemp < temps.size(); ++iTemp)
   {
      double temp = temps[iTemp];
      plotOfs << fixed << static_cast<int>(temp);
      for (size_t iBand = 0; iBand < cBAND_COUNT; ++iBand)
      {
         double r = BlackbodyRadiance2(temp, cBAND_DESC[iBand].mLowerWavelength, cBAND_DESC[iBand].mUpperWavelength);
         if (r < 1.0E-99)
         {
            r = 1.0E-99;
         }
         plotOfs << ',' << scientific << setprecision(6) << r;
      }
      plotOfs << '\n';
   }
#endif
