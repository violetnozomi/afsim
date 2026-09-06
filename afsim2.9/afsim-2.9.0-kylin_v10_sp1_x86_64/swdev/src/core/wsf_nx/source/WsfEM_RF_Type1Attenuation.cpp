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

#include "WsfEM_RF_Type1Attenuation.hpp"

#include <algorithm>

#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfEM_XmtrRcvr.hpp"
#include "WsfEnvironment.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"

//! References
//! 1.  Doerry Armin W., Performance Limits for Synthetic Aperture Radar,
//!     SAND2001-0044, Sandia National Laboratories, Albuquerque, New Mexico, January 2001
//! 2.  Berk A., AndersonG.P., Acharya P.K., Hoke M.L., Chetwynd J.H., Bernstein, L.S., Shettle E.P., Matthew M.W.,
//! Adler-Golden S.M.,
//!     MODTRAN 4, USER'S MANUAL, Space Vehicles Directorate, Air Force Materials Command, Hanscom AFB, MA, 11 February
//!     2003
//! 3.  Liebe, Hans, J., Manabe Takeshi, and Hufford, George, A.,
//!     Millimeter-Wave Attenuation and Delay Rates Due to Fog/Cloud Conditions,
//!     IEEE Transactions of Antennas and Propagation, Vol. 37, NO.12, December 1989
//! 4.  Christian M. Ho, Wang C., Angkasa K., and Gritton K.,
//!     Estimation of Microwave Power Margin Losses Due to Earth's Atmosphere and Weather in the Frequency Range of 3-30
//!     GHz, Prepared for the United States Air Force Spectrum Efficient Technologies for Test and Evaluation Advanced
//!     Range Telemetry Edwards Air Force Base, California, JPL D-27879, January 20, 2004
//! 5.  Kulemin P. Gennadiy, Millimeter Wave Radar Target and Clutter, Artech House, 2003
//! 6.  Goldhirsh, Julius, Attenuation and Backscatter From a Derived Two-Dimensional Duststorm Model
//!     IEEE Transactions of Antennas and Propagation, Vol. 49, No. 12, December 2001
//! 7.  Tomlinson P. G., Model for a Space Radar Clutter, Final Technical Report, RADC-TR-79-166, Rome Air Development
//! Center, June 1979
//! 8.  Nathanson, R. E., Radar Design Principles, McGraw-Hill, NH (1969)
//! 9.  De Wolf, D.A.; Russchenberg, W.J.; Ligthart, L.P.,
//!     Radar Reflection from Clouds:  Gigahertz Backscatter Cross Sections and Doppler Spectra, Antennas and
//!     Propagation, IEEE Transactions on Volumne 48, Issue 2, Feb 2000
//! 10. Long M.W., Radar Reflectivity of Land and Sea, Artech House, 2001
//! 11. Skolnik, M. I., Introduction to Radar Systems, 2nd ed., McGraw-Hill, NY (1980)
//! 12. Lamont V. Blake, Radar Range Performance Analysis, Artech House, 1986
//!
//! Atm:  1
//! Cloud:  2,3,4
//! Rain:  8
//! Dust:  6

namespace
{
enum
{
   cATM_PARAMS_SIZE = 17
};
const double sAtmParams[cATM_PARAMS_SIZE][3] = {{1.5, 0.106, 0.13005},
                                                {3.0, 0.112, 0.13895},
                                                {5.0, 0.12, 0.15719},
                                                {7.0, 0.852, 0.24552},
                                                {8.0, 0.086, 0.25362},
                                                {9.0, 0.0928, 0.26171},
                                                {10.0, 0.1109, 0.2806},
                                                {11.0, 0.1104, 0.2806},
                                                {12.0, 0.1217, 0.29139},
                                                {13.0, 0.1354, 0.30326},
                                                {14.0, 0.1523, 0.31621},
                                                {15.0, 0.1739, 0.33078},
                                                {16.0, 0.2023, 0.34751},
                                                {17.0, 0.2418, 0.36316},
                                                {18.0, 0.2998, 0.38529},
                                                {35.0, 0.5527, 0.25999},
                                                {94.0, 2.459, 0.32431}};

enum
{
   cCLOUD_PARAMS_SIZE = 22
};
const double sCloudParams[cCLOUD_PARAMS_SIZE][2] = {{1.0, 0.0064},  {2.0, 0.0146},  {3.0, 0.0245},  {4.0, 0.0362},
                                                    {5.0, 0.0496},  {6.0, 0.0646},  {7.0, 0.0812},  {8.0, 0.0993},
                                                    {9.0, 0.119},   {10.0, 0.1402}, {11.0, 0.1627}, {12.0, 0.1867},
                                                    {13.0, 0.2121}, {14.0, 0.2388}, {15.0, 0.2668}, {16.0, 0.296},
                                                    {17.0, 0.3265}, {18.0, 0.3582}, {19.0, 0.3911}, {20.0, 0.4251},
                                                    {35.0, 1.0541}, {94.0, 4.7876}};

enum
{
   cDUST_PARAMS_SIZE = 23
};
const double sDustParams[cDUST_PARAMS_SIZE][3] = {{1.0, 0.039, 0.024},  {2.0, 0.077, 0.049},  {3.0, 0.116, 0.073},
                                                  {4.0, 0.154, 0.097},  {5.0, 0.193, 0.121},  {6.0, 0.232, 0.146},
                                                  {7.0, 0.27, 0.17},    {8.0, 0.309, 0.194},  {9.0, 0.347, 0.218},
                                                  {10.0, 0.386, 0.243}, {11.0, 0.424, 0.267}, {12.0, 0.463, 0.291},
                                                  {13.0, 0.502, 0.315}, {14.0, 0.54, 0.34},   {15.0, 0.579, 0.364},
                                                  {16.0, 0.617, 0.388}, {17.0, 0.656, 0.412}, {18.0, 0.695, 0.437},
                                                  {19.0, 0.733, 0.461}, {20.0, 0.772, 0.485}, {25.0, 0.965, 0.607},
                                                  {30.0, 1.158, 0.728}, {35.0, 1.351, 0.849}};

enum
{
   cRAIN_PARAMS_SIZE = 15
};
const double sRainParams[cRAIN_PARAMS_SIZE][3] = {{1.0, 0.000035, 0.88},
                                                  {2.0, 0.000138, 0.923},
                                                  {3.0, 0.000591, 1.075},
                                                  {6.0, 0.00155, 1.265},
                                                  {7.0, 0.00265, 1.312},
                                                  {8.0, 0.00395, 1.31},
                                                  {10.0, 0.00887, 1.264},
                                                  {12.0, 0.0168, 1.2},
                                                  {15.0, 0.0347, 1.128},
                                                  {20.0, 0.0691, 1.065},
                                                  {25.0, 0.113, 1.03},
                                                  {30.0, 0.167, 1.0},
                                                  {35.0, 0.233, 0.963},
                                                  {40.0, 0.31, 0.929},
                                                  {45.0, 0.393, 0.897}};
} // namespace

// =================================================================================================
WsfEM_RF_Type1Attenuation::WsfEM_RF_Type1Attenuation()
   : mScenarioPtr(nullptr)
{
}

// =================================================================================================
WsfEM_RF_Type1Attenuation::WsfEM_RF_Type1Attenuation(const WsfEM_RF_Type1Attenuation& aSrc)
   : mScenarioPtr(nullptr)
{
}

// =================================================================================================
//! Factory method for WsfEM_Attenuation.
//! This is called by WsfEM_Attenuation::LoadInstance to determine if a model
//! represented by this class is being requested.
// static
WsfEM_Attenuation* WsfEM_RF_Type1Attenuation::ObjectFactory(const std::string& aTypeName)
{
   WsfEM_Attenuation* instancePtr = nullptr;
   if (aTypeName == "rf_type1")
   {
      instancePtr = new WsfEM_RF_Type1Attenuation();
   }
   return instancePtr;
}

// =================================================================================================
WsfEM_Attenuation* WsfEM_RF_Type1Attenuation::Clone() const
{
   return new WsfEM_RF_Type1Attenuation(*this);
}

// =================================================================================================
// protected virtual
double WsfEM_RF_Type1Attenuation::ComputeAttenuationFactorP(double aRange, double aElevation, double aAltitude, double aFrequency)
{
   double atmFactor   = ComputeAtmosphericAttenuation(aRange, aElevation, aFrequency);
   double cloudFactor = ComputeCloudAttenuation(aRange, aElevation, aFrequency);
   double dustFactor  = ComputeDustAttenuation(aRange, aElevation, aFrequency);
   double rainFactor  = ComputeRainAttenuation(aRange, aElevation, aFrequency);
   // cout << " AF=" << atmFactor << " CF=" << cloudFactor << " DF=" << dustFactor << " RF=" << rainFactor << endl;
   return atmFactor * cloudFactor * dustFactor * rainFactor;
}

// =================================================================================================
//! Given the range and elevation, compute the altitude assuming a 4/3 spherical earth.
// private
double WsfEM_RF_Type1Attenuation::ComputeAltitude(double aRange, double aElevation)
{
   double re  = UtSphericalEarth::cEARTH_RADIUS * 4.0 / 3.0;
   double a   = re;
   double b   = aRange;
   double c2  = (a * a) + (b * b) - (2.0 * a * b * cos(UtMath::cPI_OVER_2 + aElevation));
   double alt = sqrt(c2) - re;
   return std::max(alt, 1.0);
}

// =================================================================================================
double WsfEM_RF_Type1Attenuation::ComputeAtmosphericAttenuation(double aRange, double aElevation, double aFrequency)
{
   double frequency = aFrequency * 1.0E-9; // Convert frequency to gHz

   // Determine lower and upper table indices that bound the frequency of interest.

   unsigned int lowerIndex = 0;
   unsigned int upperIndex = 0;
   double       freqInterp = 0.0;
   if (frequency <= sAtmParams[0][0])
   {
      lowerIndex = 0;
      upperIndex = 0;
   }
   else if (frequency >= sAtmParams[cATM_PARAMS_SIZE - 1][0])
   {
      lowerIndex = cATM_PARAMS_SIZE - 1;
      upperIndex = lowerIndex;
   }
   else
   {
      for (upperIndex = 1; upperIndex < cATM_PARAMS_SIZE; ++upperIndex)
      {
         if (frequency < sAtmParams[upperIndex][0])
         {
            break;
         }
      }
      lowerIndex = upperIndex - 1;
      freqInterp = (frequency - sAtmParams[lowerIndex][0]) / (sAtmParams[upperIndex][0] - sAtmParams[lowerIndex][0]);
   }

   // Solve for the approximate altitude difference using a 4/3 round earth.

   double h = ComputeAltitude(aRange, aElevation);
   h *= 0.001; // altitude difference in km

   // alpha1 and alpha2 is the one-way attenuation (dB/km) for the lower and upper frequencies respectively.

   double a      = sAtmParams[lowerIndex][1];
   double b      = sAtmParams[lowerIndex][2];
   double alpha1 = a * (1.0 - exp(-b * h)) / h;
   double alpha2 = alpha1;
   if (lowerIndex != upperIndex)
   {
      a      = sAtmParams[upperIndex][1];
      b      = sAtmParams[upperIndex][2];
      alpha2 = a * (1.0 - exp(-b * h)) / h;
   }

   double twoWayLoss1 = UtMath::DB_ToLinear(alpha1 * (aRange * 0.001));
   double twoWayLoss2 = UtMath::DB_ToLinear(alpha2 * (aRange * 0.001));
   double twoWayLoss  = twoWayLoss1 + freqInterp * (twoWayLoss2 - twoWayLoss1);
   double oneWayLoss  = sqrt(twoWayLoss);
   double factor      = 1.0 / oneWayLoss;
   return factor;
}

// =================================================================================================
// private
double WsfEM_RF_Type1Attenuation::ComputeCloudAttenuation(double aRange, double aElevation, double aFrequency)
{
   double cloudLowerAlt;
   double cloudUpperAlt;
   mScenarioPtr->GetEnvironment().GetCloudLevel(cloudLowerAlt, cloudUpperAlt);
   if (cloudLowerAlt == cloudUpperAlt)
   {
      return 1.0;
   }

   // Check to see if the higher object is below or in the cloud layer. If it is below the cloud layer
   // then there is no attenuation due to clouds. If it is within the cloud layer then the upper bound
   // of the cloud layer is truncated to the altitude of the higher object.

   double alt = ComputeAltitude(aRange, aElevation);
   if (alt < cloudLowerAlt)
   {
      return 1.0;
   }
   cloudUpperAlt = std::min(cloudUpperAlt, alt);

   double frequency = aFrequency * 1.0E-9; // Convert frequency to gHz

   // Determine lower and upper table indices that bound the frequency of interest.

   unsigned int lowerIndex = 0;
   unsigned int upperIndex = 0;
   double       freqInterp = 0.0;
   if (frequency <= sCloudParams[0][0])
   {
      lowerIndex = 0;
      upperIndex = 0;
   }
   else if (frequency >= sCloudParams[cCLOUD_PARAMS_SIZE - 1][0])
   {
      lowerIndex = cCLOUD_PARAMS_SIZE - 1;
      upperIndex = lowerIndex;
   }
   else
   {
      for (upperIndex = 1; upperIndex < cCLOUD_PARAMS_SIZE; ++upperIndex)
      {
         if (frequency < sCloudParams[upperIndex][0])
         {
            break;
         }
      }
      lowerIndex = upperIndex - 1;
      freqInterp = (frequency - sCloudParams[lowerIndex][0]) / (sCloudParams[upperIndex][0] - sCloudParams[lowerIndex][0]);
   }

   // alpha1 and alpha2 is the one-way attenuation (dB/km) for the lower and upper frequencies respectively.

   double alpha1 = sCloudParams[lowerIndex][1]; // C_cloud for lower frequency
   double alpha2 = sCloudParams[upperIndex][1]; // C_cloud for upper frequency

   // Get the path length through the cloud layer

   double D_cloud    = (cloudUpperAlt - cloudLowerAlt); // Cloud layer thickness
   double sinPhi     = sin(aElevation);
   double pathLength = D_cloud / std::max(sinPhi, 0.001);

   // Compute the total path attenuation for the bounding frequencies and interpolate to get the desired value.

   double oneWayLoss1 = UtMath::DB_ToLinear(alpha1 * (pathLength * 0.001)); // Note: change length to km
   double oneWayLoss2 = UtMath::DB_ToLinear(alpha2 * (pathLength * 0.001));
   double oneWayLoss  = oneWayLoss1 + freqInterp * (oneWayLoss2 - oneWayLoss1);
   double factor      = 1.0 / oneWayLoss;
   return factor;
}

// =================================================================================================
//! RF Dust Storm attenuation model.
//! The following assumptions are made...
//! - At least one of the objects is on the ground.
//! - Dust storm is 100 km in horizontal extent.
//! - Dust storm vertical extent is 1 km.
// private
double WsfEM_RF_Type1Attenuation::ComputeDustAttenuation(double aRange, double aElevation, double aFrequency)
{
   // The vertical extent of the dust storm is 1 km and is assumed to be of uniform density
   // throughout the vertical extent.

   static const double cDUST_STORM_HEIGHT = 1000.0;

   double visibilityRange = mScenarioPtr->GetEnvironment().GetDustStormVisibility();
   if (visibilityRange <= 0.0)
   {
      return 1.0;
   }

   double frequency = aFrequency * 1.0E-9; // Convert frequency to ghz

   // Determine lower and upper table indices that bound the frequency of interest.

   unsigned int lowerIndex = 0;
   unsigned int upperIndex = 0;
   double       freqInterp = 0.0;
   if (frequency <= sDustParams[0][0])
   {
      lowerIndex = 0;
      upperIndex = 0;
   }
   else if (frequency >= sDustParams[cDUST_PARAMS_SIZE - 1][0])
   {
      lowerIndex = cDUST_PARAMS_SIZE - 1;
      upperIndex = lowerIndex;
   }
   else
   {
      for (upperIndex = 1; upperIndex < cDUST_PARAMS_SIZE; ++upperIndex)
      {
         if (frequency < sDustParams[upperIndex][0])
         {
            break;
         }
      }
      lowerIndex = upperIndex - 1;
      freqInterp = (frequency - sDustParams[lowerIndex][0]) / (sDustParams[upperIndex][0] - sDustParams[lowerIndex][0]);
   }

   // alpha1 and alpha2 is the one-way attenuation (dB/km) for the lower and upper frequencies respectively.
   // Column 2 (array index 1) is the attenuation (dB/km) for 1km visibility.
   // Column 3 (array index 2) is the attenuation (dB/km) for 5km visibility.
   // If the visibility is outside those values then the value at that limit is used.
   // Intermediate values are interpolated.

   double alpha1;
   double alpha2;
   if (visibilityRange <= 1001.0) // ~1km
   {
      alpha1 = sDustParams[lowerIndex][1];
      alpha2 = sDustParams[upperIndex][1];
   }
   else if (visibilityRange >= 4999.0) // ~ 5km
   {
      alpha1 = sDustParams[lowerIndex][2];
      alpha2 = sDustParams[upperIndex][2];
   }
   else
   {
      double t = (visibilityRange - 1000.0) / (5000.0 - 1000.0);
      alpha1   = sDustParams[lowerIndex][1] + t * (sDustParams[lowerIndex][2] - sDustParams[lowerIndex][1]);
      alpha2   = sDustParams[upperIndex][1] + t * (sDustParams[upperIndex][2] - sDustParams[upperIndex][1]);
   }

   // Only the slant path that is contained in the dust layer (1 km) is considered by this algorithm.
   double pathLength = aRange;
   double alt        = ComputeAltitude(aRange, aElevation);
   if (alt > cDUST_STORM_HEIGHT)
   {
      // Use law of sines to solve for the slant range to the top of the dust layer.
      // Side A = from Earth center to lowest object,  Angle A = angle opposite side A.
      // Side B = from Earth center to higher object, Angle B = angle opposite side B.
      // Side C = slant range.  Angle C = angle opposite side C.
      //
      // This is slightly different than flat-Earth cDUST_STORM_HEIGHT / sin(aElevation).

      // Solve first to get angle A.
      double re     = UtSphericalEarth::cEARTH_RADIUS * 4.0 / 3.0;
      double sideA  = re;
      double sideB  = re + cDUST_STORM_HEIGHT;
      double angleB = aElevation + UtMath::cPI_OVER_2;
      double termB  = sideB / sin(angleB);
      double sinA   = sideA / termB;
      double angleA = asin(sinA);

      // Now derive angle C and then use it to get sideC, the slant range.
      double angleC = UtMath::cPI - angleA - angleB;
      double sideC  = sin(angleC) * termB;
      pathLength    = sideC;
   }

   // Compute the total path attenuation for the bounding frequencies and interpolate to get the desired value.

   double oneWayLoss1 = UtMath::DB_ToLinear(alpha1 * (pathLength * 0.001)); // Note: change length to km
   double oneWayLoss2 = UtMath::DB_ToLinear(alpha2 * (pathLength * 0.001));
   double oneWayLoss  = oneWayLoss1 + freqInterp * (oneWayLoss2 - oneWayLoss1);
   double factor      = 1.0 / oneWayLoss;
   return factor;
}

// =================================================================================================
// private
double WsfEM_RF_Type1Attenuation::ComputeRainAttenuation(double aRange, double aElevation, double aFrequency)
{
   double rainRate = mScenarioPtr->GetEnvironment().GetRainRate();
   if (rainRate <= 0.0)
   {
      return 1.0;
   }

   rainRate *= (1000.0 * 3600.0);          // Convert rain rate to mm/hr
   double frequency = aFrequency * 1.0E-9; // Convert frequency to gHz

   // Determine lower and upper table indices that bound the frequency of interest.

   unsigned int lowerIndex = 0;
   unsigned int upperIndex = 0;
   double       freqInterp = 0.0;
   if (frequency <= sRainParams[0][0])
   {
      lowerIndex = 0;
      upperIndex = 0;
   }
   else if (frequency >= sRainParams[cRAIN_PARAMS_SIZE - 1][0])
   {
      lowerIndex = cRAIN_PARAMS_SIZE - 1;
      upperIndex = lowerIndex;
   }
   else
   {
      for (upperIndex = 1; upperIndex < cRAIN_PARAMS_SIZE; ++upperIndex)
      {
         if (frequency < sRainParams[upperIndex][0])
         {
            break;
         }
      }
      lowerIndex = upperIndex - 1;
      freqInterp = (frequency - sRainParams[lowerIndex][0]) / (sRainParams[upperIndex][0] - sRainParams[lowerIndex][0]);
   }

   // alpha1 and alpha2 is the one-way attenuation (dB/km) for the lower and upper frequencies respectively.

   double a      = sRainParams[lowerIndex][1];
   double b      = sRainParams[lowerIndex][2];
   double alpha1 = a * pow(rainRate, b);
   double alpha2 = alpha1;
   if (upperIndex != lowerIndex)
   {
      b      = sRainParams[upperIndex][2];
      alpha2 = a * pow(rainRate, b);
   }

   // Compute the total path attenuation for the bounding frequencies and interpolate to get the desired value.

   double oneWayLoss1 = UtMath::DB_ToLinear(alpha1 * (aRange * 0.001)); // Note: range in km
   double oneWayLoss2 = UtMath::DB_ToLinear(alpha2 * (aRange * 0.001));
   double oneWayLoss  = oneWayLoss1 + freqInterp * (oneWayLoss2 - oneWayLoss1);
   double factor      = 1.0 / oneWayLoss;
   return factor;
}

bool WsfEM_RF_Type1Attenuation::Initialize(WsfEM_XmtrRcvr* aXmtrRcvrPtr)
{
   mScenarioPtr = &aXmtrRcvrPtr->GetPlatform()->GetScenario();
   return true;
}
