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

// An atmospheric attenuation model based on standards from the
// International Telecommunications Union (ITU) Radio Communication Sector.
//
// The following ITU recommendations are used in this model.
//
// 1) "Recommendation ITU-R P.676-9, Attenuation by atmospheric gases".
// 2) "Recommendation ITU-R P.835-4, Reference standard atmospheres".
// 3) "Recommendation ITU-R P.838-3, Specific attenuation model for rain for use in prediction methods".
// 4) "Recommendation ITU-R P.840-4, Attenuation due to clouds and fog".

#include "WsfEM_ITU_Attenuation.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>

// Indicate the atmosphere model from UtAtmosphere is to be used rather than the standard atmosphere
// model from ITU-R P.835-4. The pressure and temperature were nearly identical, but the water
// vapor density calculations in UtAtmosphere were more complete.

#define USE_UTIL_ATMOSPHERE

#include "UtAtmosphere.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Util.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEnvironment.hpp"
#include "WsfScenario.hpp"

namespace
{
struct Factory
{
   WsfScenario*       mScenarioPtr;
   WsfEM_Attenuation* operator()(const std::string& aTypeName) const
   {
      WsfEM_Attenuation* instancePtr = nullptr;
      if ((aTypeName == "WSF_ITU_ATTENUATION") || (aTypeName == "itu"))
      {
         instancePtr = new WsfEM_ITU_Attenuation(mScenarioPtr->GetAtmosphere());
      }
      return instancePtr;
   }
};
} // namespace

std::function<WsfEM_Attenuation*(const std::string&)> WsfEM_ITU_Attenuation::GetObjectFactory(WsfScenario& aScenario)
{
   Factory f;
   f.mScenarioPtr = &aScenario;
   return f;
}

// =================================================================================================
WsfEM_ITU_Attenuation::WsfEM_ITU_Attenuation(const UtAtmosphere& aAtm)
   : WsfEM_Attenuation()
   , mAtmosphere(aAtm)
   , mFrequency(-1.0)
   , mGammaTable()
{
}

// =================================================================================================
WsfEM_ITU_Attenuation::WsfEM_ITU_Attenuation(const WsfEM_ITU_Attenuation& aSrc)
   : WsfEM_Attenuation(aSrc)
   , mAtmosphere(aSrc.mAtmosphere)
   , mFrequency(-1.0)
   , mGammaTable()
{
}

// =================================================================================================
// virtual
WsfEM_Attenuation* WsfEM_ITU_Attenuation::Clone() const
{
   return new WsfEM_ITU_Attenuation(*this);
}

// =================================================================================================
// virtual
bool WsfEM_ITU_Attenuation::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "plot")
   {
      // This is an undocumented command (for testing).
      std::string plotType;
      aInput.ReadValue(plotType);
      if (plotType == "gas_figure_1")
      {
         PlotGasFigure1();
      }
      else if (plotType == "gas_figure_2")
      {
         PlotGasFigure2();
      }
      else if (plotType == "gas_figure_3")
      {
         PlotGasFigure3();
      }
      else if (plotType == "cloud_figure_1")
      {
         PlotCloudFigure1();
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "query")
   {
      // This is an undocumented command (for testing).
      double                    frequency    = 1.0E+9;
      WsfEM_Types::Polarization polarization = WsfEM_Types::cPOL_VERTICAL;
      double                    altitude     = 0.0;
      double                    range        = 1000.0;
      double                    elevation    = 0.0;
      UtInputBlock              inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (command == "frequency")
         {
            aInput.ReadValueOfType(frequency, UtInput::cFREQUENCY);
            aInput.ValueGreater(frequency, 0.0);
         }
         else if (command == "polarization")
         {
            std::string polarizationStr;
            aInput.ReadValue(polarizationStr);
            if (!WsfEM_Util::StringToEnum(polarization, polarizationStr))
            {
               throw UtInput::BadValue(aInput);
            }
         }
         else if (command == "range")
         {
            aInput.ReadValueOfType(range, UtInput::cLENGTH);
            aInput.ValueGreaterOrEqual(range, 0.0);
         }
         else if (command == "elevation")
         {
            aInput.ReadValueOfType(elevation, UtInput::cANGLE);
            aInput.ValueInClosedRange(elevation, 0.0, UtMath::cPI_OVER_2);
         }
         else if (command == "altitude")
         {
            aInput.ReadValueOfType(altitude, UtInput::cLENGTH);
            aInput.ValueGreaterOrEqual(altitude, 0.0);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }

      double atten = ComputeAttenuationFactor(range,
                                              elevation,
                                              altitude,
                                              frequency,
                                              polarization,
                                              4.0 / 3.0,
                                              WsfScenario::FromInput(aInput).GetEnvironment());
      auto   out   = ut::log::debug() << "WsfEM_ITU_Attenuation: ProcessInput called.";
      out.AddNote() << "Range: " << range << " m";
      out.AddNote() << "Elevation: " << elevation * UtMath::cDEG_PER_RAD << " deg";
      out.AddNote() << "Altitude: " << altitude << " m";
      out.AddNote() << "Frequency: " << frequency << " Hz";
      out.AddNote() << "Polarization: " << WsfEM_Util::EnumToString(polarization);
      out.AddNote() << "Attenuation: " << UtMath::SafeLinearToDB(atten) << " dB (" << atten << " abs)";
      out.AddNote() << "Specific Attenuation: " << -UtMath::SafeLinearToDB(atten) / (range * 0.001) << " dB/km";
   }
   else if (mAtmosphere.ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = WsfEM_Attenuation::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// This overrides the base class 3-argument implementation.
// virtual
double WsfEM_ITU_Attenuation::ComputeAttenuationFactor(WsfEM_Interaction&          aInteraction,
                                                       WsfEnvironment&             aEnvironment,
                                                       WsfEM_Interaction::Geometry aGeometry)
{
   double range, elevation, altitude;
   GetRangeElevationAltitude(aInteraction, aGeometry, range, elevation, altitude);
   WsfEM_Xmtr* xmtrPtr = aInteraction.GetTransmitter();
   return ComputeAttenuationFactor(range,
                                   elevation,
                                   altitude,
                                   xmtrPtr->GetFrequency(),
                                   xmtrPtr->GetPolarization(),
                                   xmtrPtr->GetEarthRadiusMultiplier(),
                                   aEnvironment);
}

// =================================================================================================
// virtual
double WsfEM_ITU_Attenuation::ComputeAttenuationFactor(double                    aRange,
                                                       double                    aElevation,
                                                       double                    aAltitude,
                                                       double                    aFrequency,
                                                       WsfEM_Types::Polarization aPolarization,
                                                       double                    aEarthRadiusScaleFactor,
                                                       WsfEnvironment&           aEnvironment)
{
   // The model is valid only from 1 GHz to 1000 GHz

   double frequency = UtMath::Limit(aFrequency, 1.0E+9, 1000.0E+9);

   // Regenerate the table of specific attenuation vs. altitude if there is a significant change
   // in the frequency for which the current table was generated.

   if (fabs(frequency - mFrequency) > (0.01 * aFrequency))
   {
      GenerateTable(frequency, aPolarization, aEnvironment);
   }

   // Return a factor of 1 for the trivial cases where the range is small or the starting
   // altitude is above the atmosphere.

   if ((aRange < 1.0) || (aAltitude >= mGammaTable.back().mAltitude))
   {
      return 1.0;
   }

   double elevation = std::min(std::max(aElevation, 0.0), 89.9 * UtMath::cRAD_PER_DEG);
   double altitude  = std::max(aAltitude, 0.0);

   // Find the lower index of the entry such that ALT(i) <= altitude < ALT(i+1).
   // We have already ensured that the altitude is strictly less that the last
   // altitude in the table.

   size_t gtIndex = 0;
   while (altitude > mGammaTable[gtIndex].mAltitude)
   {
      ++gtIndex;
   }
   if (altitude != mGammaTable[gtIndex].mAltitude)
   {
      --gtIndex;
   }

   // side A : The side from the center of the earth to the source point.
   // side B : The side from the center of the earth to the target point.
   // side C : The side from the source point to the target point (the range).
   // angle A: The angle opposite side A.
   // angle B: The angle opposite side B (the elevation angle + 90 degrees).
   // angle C: The angle opposite side C.
   //
   // We'll also be making extensive use of the law of sines:
   //
   //     side A         side B         side C
   //  ------------ = ------------ = ------------
   //  sin(angle A)   sin(angle B)   sin(angle C)

   // NOTE: The provided elevation angle is the 'apparent' elevation angle of the target on the
   // 'unscaled' Earth, which takes into account the effects of atmospheric refraction. Because
   // of this we do NOT use a scaled Earth radius in the following calculations.

   double re        = UtSphericalEarth::cEARTH_RADIUS;
   double sideA     = re + altitude;
   double angleB    = elevation + UtMath::cPI_OVER_2;
   double sinAngleB = sin(angleB);

   // Adjust the starting gamma based on the fact that we may be starting mid-layer.
   double lowerAltitude = mGammaTable[gtIndex].mAltitude;
   double upperAltitude = mGammaTable[gtIndex + 1].mAltitude;
   double f             = (altitude - lowerAltitude) / (upperAltitude - lowerAltitude);
   double lowerGamma    = mGammaTable[gtIndex].mGamma;
   double upperGamma    = mGammaTable[gtIndex + 1].mGamma;
   lowerGamma           = lowerGamma + f * (upperGamma - lowerGamma);

   // Iterate through the layers, accumulating the loss in each layer.

   double atten_dB  = 0.0;
   double range     = 0.0;
   double lastRange = 0.0;
   size_t maxIndex  = mGammaTable.size() - 1;
   while ((range < aRange) && (gtIndex < maxIndex))
   {
      // Use the law of sines to get the angle A.
      double sideB     = re + mGammaTable[gtIndex + 1].mAltitude;
      double sinAngleA = sideA / sideB * sinAngleB;
      double angleA    = asin(sinAngleA);

      // Now that we know two angles (A and B) we can solve for angle C.
      double angleC = UtMath::cPI - angleA - angleB;

      // Use the law of sines again to get side C, the range to the top of the layer.
      double sideC = sideA * sin(angleC) / sinAngleB;
      range        = sideC;

      // If this is the final layer, adjust the range and final gamma to reflect partial penetration.
      upperGamma = mGammaTable[gtIndex + 1].mGamma;
      if (range > aRange)
      {
         f          = (aRange - lastRange) / (range - lastRange);
         upperGamma = lowerGamma + f * (upperGamma - lowerGamma);
         range      = aRange;
      }

      // Accumulate the attenuation.
      atten_dB += ((0.5 * (lowerGamma + upperGamma)) * ((range - lastRange) * 0.001));

      lowerGamma = upperGamma;
      lastRange  = range;
      ++gtIndex;
   }

   // Convert to a linear attenuation factor and return. Note that the result may be zero!
   double atten = pow(10.0, -0.1 * atten_dB);
   return atten;
}

// =================================================================================================
//! For a given frequency, compute the table of specific attenuation as a function of altitude.
// private
void WsfEM_ITU_Attenuation::GenerateTable(double                    aFrequency,
                                          WsfEM_Types::Polarization aPolarization,
                                          WsfEnvironment&           aEnvironment)
{
   Point  point;
   double pressure;
   double temperature;
   double waterVaporDensity;

   // Determine if attenuation due to rain is to be calculated. This can be calculated outside the
   // loop because it is only dependent on frequency, polarization and rain rate.

   double gammaR       = 0.0; // Assume no rain
   double upperRainAlt = 0.0;
   double rainRate     = aEnvironment.GetRainRate();
   if (rainRate > 0.0)
   {
      upperRainAlt = aEnvironment.GetRainUpperLevel();
      gammaR       = ComputeRainSpecificAttenuation(aFrequency, aPolarization, rainRate);

      // If an upper rain altitude was not specified use the lower cloud altitude if it was specified.
      // If it wasn't specified then use 10000.0 m.
      if ((rainRate > 0.0) && (upperRainAlt <= 0.0))
      {
         double notUsed;
         aEnvironment.GetCloudLevel(upperRainAlt, notUsed);
         if (upperRainAlt <= 0.0)
         {
            upperRainAlt = 10000.0;
         }
      }
   }

   // Determine if the contribution of clouds/fog is to be calculated. This is only done if
   // the cloud level and water density within the cloud are defined.

   double lowerCloudAlt;
   double upperCloudAlt;
   aEnvironment.GetCloudLevel(lowerCloudAlt, upperCloudAlt);
   double cloudWaterDensity = aEnvironment.GetCloudWaterDensity();
   if ((cloudWaterDensity == 0.0) || (upperCloudAlt <= lowerCloudAlt))
   {
      lowerCloudAlt     = 0.0;
      upperCloudAlt     = 0.0;
      cloudWaterDensity = 0.0;
   }

   // Compute the upper altitude for which the table will be constructed.

   double maxAlt = std::max(upperRainAlt, upperCloudAlt);
   maxAlt        = std::max(maxAlt, 30000.0);
   maxAlt        = std::min(maxAlt, 100000.0);
   int maxAltInt = 1000 * static_cast<int>(maxAlt / 1000.0);
   if (maxAltInt < maxAlt)
   {
      maxAltInt += 1000;
   }

   mFrequency = aFrequency;
   mGammaTable.clear();
   mGammaTable.reserve(maxAltInt);
   for (int ialt = 0; ialt <= maxAltInt; ialt += 1000)
   {
      point.mAltitude = ialt;
      ComputeAtmosphereData(mAtmosphere, point.mAltitude, pressure, temperature, waterVaporDensity);
      // Exit if we have exceeded the bounds of the atmosphere model.
      if ((pressure <= 0.0) || (temperature <= 0.0))
      {
         break;
      }

      // Compute contribution due to atmospheric gases.
      point.mGamma = ComputeGasSpecificAttenuation(aFrequency, pressure, temperature, waterVaporDensity);

      // Add in contribution due to rain.
      if (point.mAltitude <= upperRainAlt)
      {
         point.mGamma += gammaR;
      }

      // Add in contribution due to clouds/fog.
      if ((cloudWaterDensity > 0.0) && (point.mAltitude >= lowerCloudAlt) && (point.mAltitude <= upperCloudAlt))
      {
         point.mGamma += ComputeCloudSpecificAttenuation(aFrequency, temperature, cloudWaterDensity);
      }
      mGammaTable.push_back(point);
   }
}

// =================================================================================================
// Implementation of ITU Recommendation ITU-R P.676-8, "Attenuation by atmospheric gases".
// =================================================================================================

namespace
{
// Spectroscopic data for oxygen attenuation (Table 1)
//
// NOTE: IF YOU ADD ENTRIES TO THIS TABLE, BE SURE TO MAINTAIN cTABLE_1_INDEX_GT_60_GHZ!!!!

// Index of first entry where f_i is > 60 GHz
enum
{
   cTABLE_1_INDEX_GT_60_GHZ = 19
};

enum
{
   cTABLE_1_SIZE = 44
};
static const double cTABLE_1[cTABLE_1_SIZE][7] = {{50.474238, 0.94, 9.694, 8.90, 0.0, 2.400, 7.900},
                                                  {50.987749, 2.46, 8.694, 9.10, 0.0, 2.200, 7.800},
                                                  {51.503350, 6.08, 7.744, 9.40, 0.0, 1.970, 7.740},
                                                  {52.021410, 14.14, 6.844, 9.70, 0.0, 1.660, 7.640},
                                                  {52.542394, 31.02, 6.004, 9.90, 0.0, 1.360, 7.510},
                                                  {53.066907, 64.10, 5.224, 10.20, 0.0, 1.310, 7.140},
                                                  {53.595749, 124.70, 4.484, 10.50, 0.0, 2.300, 5.840},
                                                  {54.130000, 228.00, 3.814, 10.70, 0.0, 3.350, 4.310},
                                                  {54.671159, 391.80, 3.194, 11.00, 0.0, 3.740, 3.050},
                                                  {55.221367, 631.60, 2.624, 11.30, 0.0, 2.580, 3.390},
                                                  {55.783802, 953.50, 2.119, 11.70, 0.0, -1.660, 7.050},
                                                  {56.264775, 548.90, 0.015, 17.30, 0.0, 3.900, -1.130},
                                                  {56.363389, 1344.00, 1.660, 12.00, 0.0, -2.970, 7.530},
                                                  {56.968206, 1763.00, 1.260, 12.40, 0.0, -4.160, 7.420},
                                                  {57.612484, 2141.00, 0.915, 12.80, 0.0, -6.130, 6.970},
                                                  {58.323877, 2386.00, 0.626, 13.30, 0.0, -2.050, 0.510},
                                                  {58.446590, 1457.00, 0.084, 15.20, 0.0, 7.480, -1.460},
                                                  {59.164207, 2404.00, 0.391, 13.90, 0.0, -7.220, 2.660},
                                                  {59.590983, 2112.00, 0.212, 14.30, 0.0, 7.650, -0.900},
                                                  {60.306061, 2124.00, 0.212, 14.50, 0.0, -7.050, 0.810}, // cTABLE_1_INDEX_GT_60_GHZ
                                                  {60.434776, 2461.00, 0.391, 13.60, 0.0, 6.970, -3.240},
                                                  {61.150560, 2504.00, 0.626, 13.10, 0.0, 1.040, -0.670},
                                                  {61.800154, 2298.00, 0.915, 12.70, 0.0, 5.700, -7.610},
                                                  {62.411215, 1933.00, 1.260, 12.30, 0.0, 3.600, -7.770},
                                                  {62.486260, 1517.00, 0.083, 15.40, 0.0, -4.980, 0.970},
                                                  {62.997977, 1503.00, 1.665, 12.00, 0.0, 2.390, -7.680},
                                                  {63.568518, 1087.00, 2.115, 11.70, 0.0, 1.080, -7.060},
                                                  {64.127767, 733.50, 2.620, 11.30, 0.0, -3.110, -3.320},
                                                  {64.678903, 463.50, 3.195, 11.00, 0.0, -4.210, -2.980},
                                                  {65.224071, 274.80, 3.815, 10.70, 0.0, -3.750, -4.230},
                                                  {65.764772, 153.00, 4.485, 10.50, 0.0, -2.670, -5.750},
                                                  {66.302091, 80.09, 5.225, 10.20, 0.0, -1.680, -7.000},
                                                  {66.836830, 39.46, 6.005, 9.90, 0.0, -1.690, -7.350},
                                                  {67.369598, 18.32, 6.845, 9.70, 0.0, -2.000, -7.440},
                                                  {67.900867, 8.01, 7.745, 9.40, 0.0, -2.280, -7.530},
                                                  {68.431005, 3.30, 8.695, 9.20, 0.0, -2.400, -7.600},
                                                  {68.960311, 1.28, 9.695, 9.00, 0.0, -2.500, -7.650},
                                                  {118.750343, 945.00, 0.009, 16.30, 0.0, -0.360, 0.090},
                                                  {368.498350, 67.90, 0.049, 19.20, 0.6, 0.000, 0.000},
                                                  {424.763124, 638.00, 0.044, 19.30, 0.6, 0.000, 0.000},
                                                  {487.249370, 235.00, 0.049, 19.20, 0.6, 0.000, 0.000},
                                                  {715.393150, 99.60, 0.145, 18.10, 0.6, 0.000, 0.000},
                                                  {773.839675, 671.00, 0.130, 18.20, 0.6, 0.000, 0.000},
                                                  {834.145330, 180.00, 0.147, 18.10, 0.6, 0.000, 0.000}};

// Spectroscopic data for water-vapor attenuation (Table 2)

enum
{
   cTABLE_2_SIZE = 35
};
static const double cTABLE_2[cTABLE_2_SIZE][7] = {
   {22.235080, 0.1130, 2.143, 28.11, 0.69, 4.800, 1.00},        {67.803960, 0.0012, 8.735, 28.58, 0.69, 4.930, 0.82},
   {119.995940, 0.0008, 8.356, 29.48, 0.70, 4.780, 0.79},       {183.310091, 2.4200, 0.668, 30.50, 0.64, 5.300, 0.85},
   {321.225644, 0.0483, 6.181, 23.03, 0.67, 4.690, 0.54},       {325.152919, 1.4990, 1.540, 27.83, 0.68, 4.850, 0.74},
   {336.222601, 0.0011, 9.829, 26.93, 0.69, 4.740, 0.61},       {380.197372, 11.5200, 1.048, 28.73, 0.54, 5.380, 0.89},
   {390.134508, 0.0046, 7.350, 21.52, 0.63, 4.810, 0.55},       {437.346667, 0.0650, 5.050, 18.45, 0.60, 4.230, 0.48},
   {439.150812, 0.9218, 3.596, 21.00, 0.63, 4.290, 0.52},       {443.018295, 0.1976, 5.050, 18.60, 0.60, 4.230, 0.50},
   {448.001075, 10.3200, 1.405, 26.32, 0.66, 4.840, 0.67},      {470.888947, 0.3297, 3.599, 21.52, 0.66, 4.570, 0.65},
   {474.689127, 1.2620, 2.381, 23.55, 0.65, 4.650, 0.64},       {488.491133, 0.2520, 2.853, 26.02, 0.69, 5.040, 0.72},
   {503.568532, 0.0390, 6.733, 16.12, 0.61, 3.980, 0.43},       {504.482692, 0.0130, 6.733, 16.12, 0.61, 4.010, 0.45},
   {547.676440, 9.7010, 0.114, 26.00, 0.70, 4.500, 1.00},       {552.020960, 14.7700, 0.114, 26.00, 0.70, 4.500, 1.00},
   {556.936002, 487.4000, 0.159, 32.10, 0.69, 4.110, 1.00},     {620.700807, 5.0120, 2.200, 24.38, 0.71, 4.680, 0.68},
   {645.866155, 0.0713, 8.580, 18.00, 0.60, 4.000, 0.50},       {658.005280, 0.3022, 7.820, 32.10, 0.69, 4.140, 1.00},
   {752.033227, 239.6000, 0.396, 30.60, 0.68, 4.090, 0.84},     {841.053973, 0.0140, 8.180, 15.90, 0.33, 5.760, 0.45},
   {859.962313, 0.1472, 7.989, 30.60, 0.68, 4.090, 0.84},       {899.306675, 0.0605, 7.917, 29.85, 0.68, 4.530, 0.90},
   {902.616173, 0.0426, 8.432, 28.65, 0.70, 5.100, 0.95},       {906.207325, 0.1876, 5.111, 24.08, 0.70, 4.700, 0.53},
   {916.171582, 8.3400, 1.442, 26.70, 0.70, 4.780, 0.78},       {923.118427, 0.0869, 10.220, 29.00, 0.70, 5.000, 0.80},
   {970.315022, 8.9720, 1.920, 25.50, 0.64, 4.940, 0.67},       {987.926764, 132.1000, 0.258, 29.85, 0.68, 4.550, 0.90},
   {1780.000000, 22300.0000, 0.952, 176.20, 0.50, 30.500, 5.00}};
} // namespace

// =================================================================================================
//! Compute the specific attenuation according to ITU-R P.676 for a given set of conditions.
//! @param aFrequency         The frequency (Hz)
//! @param aPressure          The dry air pressure in (N/m^2 (i.e.: Pa)
//! @param aTemperature       The temperature (deg K)
//! @param aWaterVaporDensity The water vapor density (kg/m^3)
//! @returns The specific attenuation (dB/km).
double WsfEM_ITU_Attenuation::ComputeGasSpecificAttenuation(double aFrequency,
                                                            double aPressure,
                                                            double aTemperature,
                                                            double aWaterVaporDensity)
{
   int i;

   // Convert input arguments from MKS units to those needed by the model.
   double f   = aFrequency * 1.0E-9;           // Hz -> GHz
   f          = UtMath::Limit(f, 1.0, 1000.0); // Model is valid from 1-1000 GHz
   double p   = aPressure * 0.01;              // Pa -> hPa
   double rho = aWaterVaporDensity * 1000.0;   // kg/m^3 -> in g/m^3

   double T         = aTemperature;
   double theta     = 300.0 / T;
   double theta_3   = pow(theta, 3.0);
   double theta_3p5 = pow(theta, 3.5);

   // (Equation 4) Water vapor pressure
   double e = (rho * T) / 216.7;

   // Compute Sum(S(i) * F(i)) for oxygen

   // Comment following equation 2, "(for frequencies, f, above 118.75 GHz, only the
   // oxygen lines above 60 GHz should be included in the summation)".

   int startIndex1 = 0;
   // if (f > 118.75)
   //{
   //    startIndex1 = cTABLE_1_INDEX_GT_60_GHZ;
   // }

   double sum_SF = 0.0;
   for (i = startIndex1; i < cTABLE_1_SIZE; ++i)
   {
      // (Equation 3)
      double a_1 = cTABLE_1[i][1];
      double a_2 = cTABLE_1[i][2];
      double S_i = a_1 * 1.0E-7 * p * theta_3 * exp(a_2 * (1.0 - theta));

      // (Equation 6a)
      double a_3     = cTABLE_1[i][3];
      double a_4     = cTABLE_1[i][4];
      double delta_f = a_3 * 1.0E-4 * ((p * pow(theta, 0.8 - a_4)) + (1.1 * e * theta));

      // (Equation 6b)
      double f_i = cTABLE_1[i][0];
      delta_f    = sqrt((delta_f * delta_f) + 2.25E-6);

      // (Equation 7)
      double a_5   = cTABLE_1[i][5];
      double a_6   = cTABLE_1[i][6];
      double delta = (a_5 + (a_6 * theta)) * 1.0E-4 * (p + e) * pow(theta, 0.8);

      // (Equation 5)
      double f_sum  = f_i + f;
      double f_dif  = f_i - f;
      double numer1 = delta_f - delta * f_dif;
      double denom1 = (f_dif * f_dif) + (delta_f * delta_f);
      double numer2 = delta_f - delta * f_sum;
      double denom2 = (f_sum * f_sum) + (delta_f * delta_f);
      double F_i    = (f / f_i) * ((numer1 / denom1) + (numer2 / denom2));

      // Update cumulative S_i * F_i
      sum_SF += (S_i * F_i);
   }

   // Compute Sum(S(i) * F(i)) for water vapor

   for (i = 0; i < cTABLE_2_SIZE; ++i)
   {
      // (Equation 3)
      double b_1 = cTABLE_2[i][1];
      double b_2 = cTABLE_2[i][2];
      double S_i = b_1 * 1.0E-1 * e * theta_3p5 * exp(b_2 * (1.0 - theta));

      // (Equation 6a)
      double b_3     = cTABLE_2[i][3];
      double b_4     = cTABLE_2[i][4];
      double b_5     = cTABLE_2[i][5];
      double b_6     = cTABLE_2[i][6];
      double delta_f = b_3 * 1.0E-4 * ((p * pow(theta, b_4)) + (b_5 * e * pow(theta, b_6)));

      // (Equation 6b)
      double f_i = cTABLE_2[i][0];
      delta_f    = (0.535 * delta_f) + sqrt((0.217 * delta_f * delta_f) + ((2.1316E-12 * f_i * f_i) / theta));

      // (Equation 7)
      double delta = 0.0;

      // (Equation 5)
      double f_sum  = f_i + f;
      double f_dif  = f_i - f;
      double numer1 = delta_f - delta * f_dif;
      double denom1 = (f_dif * f_dif) + (delta_f * delta_f);
      double numer2 = delta_f - delta * f_sum;
      double denom2 = (f_sum * f_sum) + (delta_f * delta_f);
      double F_i    = (f / f_i) * ((numer1 / denom1) + (numer2 / denom2));

      // Update cumulative S_i * F_i
      sum_SF += (S_i * F_i);
   }

   // (Equation 9) Compute the width parameter for the Debye spectrum - d
   double d = 5.6E-4 * p * pow(theta, 0.8);

   // (Equation 8) Compute the dry continuum contribution - Ndp_D (N-double prime sub D).

   double temp1 = f / d;
   double term1 = 6.14E-5 / (d * (1.0 + temp1 * temp1));
   double term2 = (1.4E-12 * p * pow(theta, 1.5)) / (1.0 + 1.9E-5 * pow(f, 1.5));
   double Ndp_D = f * p * theta * theta * (term1 + term2);

   // (Equation 2) Compute the imaginary part of the frequency-dependent complex refractivity.
   double Ndp = sum_SF + Ndp_D;

   // (Equation 1) And finally, compute the specific gaseous attenuation in dB/km.
   double gamma = 0.1820 * f * Ndp;

   return gamma;
}

// =================================================================================================
// Implementation of ITU Recommendation ITU-R P.838-3, "Specific Attenuation Model for rain for
// use in prediction methods."
// =================================================================================================

namespace
{
// ===== Coefficients for computing 'k'

// a_j, b_j, c_j for k
const double cK[2][4][3] = {
   // a_j, b_j, c_j for k_H (from Table 1)
   {{-5.33980, -0.10008, 1.13098}, {-0.35351, 1.26970, 0.45400}, {-0.23789, 0.86036, 0.15354}, {-0.94158, 0.64552, 0.16817}},
   // a_j, b_j, c_j for K_V (from Table 2)
   {{-3.80595, 0.56934, 0.81061}, {-3.44965, -0.22911, 0.51059}, {-0.39902, 0.73042, 0.11899}, {0.50167, 1.07319, 0.27195}}};

// m_k
const double cM_K[2] = {
   -0.18961, // m_k for k_H (from Table 1)
   -0.16398  // m_k for k_V (from Table 2)
};

// c_k
const double cC_K[2] = {
   0.71147, // c_k for k_H (from Table 1)
   0.63297  // c_k for k_V (from Table 2)
};

// ===== Coefficients for computing 'alpha'

// a_j, b_j, c_j for alpha
const double cA[2][5][3] = {
   // a_j, b_j, c_j for alpha_H (from Table 3)
   {{-0.14318, 1.82442, -0.55187},
    {0.29591, 0.77564, 0.19822},
    {0.32177, 0.62773, 0.13164},
    {-5.37610, -0.96230, 1.47828},
    {16.1721, -3.29980, 3.43990}},
   // a_j, b_j, c_j for alpha_V (from Table 4)
   {{-0.07771, 2.33840, -0.76284},
    {0.56727, 0.95545, 0.54039},
    {-0.20238, 1.14520, 0.26809},
    {-48.2991, 0.791669, 0.116226},
    {48.5833, 0.791459, 0.116479}}};

// m_alpha
const double cM_A[2] = {
   0.67849,  // m_alpha for alpha_H (from Table 3)
   -0.053739 // m_alpha for alpha_V (from Table 4)
};

// c_alpha
const double cC_A[2] = {
   -1.95537, // c_alpha for alpha_H (from Table 3)
   0.83433   // c_alpha for alpha_V (from Table 4)
};
} // namespace

// =================================================================================================
// static
double WsfEM_ITU_Attenuation::ComputeRainSpecificAttenuation(double                    aFrequency,
                                                             WsfEM_Types::Polarization aPolarization,
                                                             double                    aRainRate)
{
   // Convert input arguments from MKS units to those needed by the model.
   double f        = aFrequency * 1.0E-9;           // Hz -> GHz
   f               = UtMath::Limit(f, 1.0, 200.0);  // Model is valid from 1 to 1000 GHz
   double rainRate = aRainRate * (1000.0 * 3600.0); // m/s -> mm/hr

   // Select the polarization index, 0 for horizontal, 1 for vertical.
   // Vertical is used for any polarization other than horizontal.

   int    iPol    = (aPolarization != WsfEM_Types::cPOL_VERTICAL) ? 0 : 1;
   double log10_f = log10(f);

   // (Equation 2)
   double log10_k = 0.0;
   for (size_t j = 0; j < 4; ++j)
   {
      double a_j = cK[iPol][j][0];
      double b_j = cK[iPol][j][1];
      double c_j = cK[iPol][j][2];
      double t   = (log10_f - b_j) / c_j;
      log10_k += a_j * exp(-(t * t));
   }
   double m_k = cM_K[iPol];
   double c_k = cC_K[iPol];
   log10_k += m_k * log10_f + c_k;
   double k = pow(10.0, log10_k);

   // (Equation 3)
   double alpha = 0.0;
   for (size_t j = 0; j < 5; ++j)
   {
      double a_j = cA[iPol][j][0];
      double b_j = cA[iPol][j][1];
      double c_j = cA[iPol][j][2];
      double t   = (log10_f - b_j) / c_j;
      alpha += a_j * exp(-(t * t));
   }
   double m_a = cM_A[iPol];
   double c_a = cC_A[iPol];
   alpha += m_a * log10_f + c_a;

   // (Equation 1)
   double gamma_R = k * pow(rainRate, alpha); // dB/km

   return gamma_R;
}

// =================================================================================================
// Implementation of ITU Recommendation ITU-R P.840-4, "Attenuation due to clouds and fog."
// =================================================================================================
// static
double WsfEM_ITU_Attenuation::ComputeCloudSpecificAttenuation(double aFrequency, double aTemperature, double aWaterDensity)
{
   // Convert input arguments from MKS units to those needed by the model.
   double f            = aFrequency * 1.0E-9;          // Hz -> GHz
   f                   = UtMath::Limit(f, 1.0, 200.0); // Model is valid from 1 to 200 GHz
   double waterDensity = aWaterDensity * 1000.0;       // kg/m^3 -> g/m^3

   // (Equation 9)
   double theta       = 300.0 / aTemperature;
   double thetaMinus1 = theta - 1.0;

   // (Equation 10)
   double f_p = 20.09 - (142.0 * thetaMinus1) + (294.0 * thetaMinus1 * thetaMinus1);

   // (Equation 9)
   double f_s = 590.0 - (1500.0 * thetaMinus1);

   // (Equation 6)
   double epsilon_0 = 77.6 + (103.3 * thetaMinus1);

   // (Equation 7)
   static const double epsilon_1 = 5.48;

   // (Equation 8)
   static const double epsilon_2 = 3.51;

   // Compute temporaries for equations 3 and 4
   double f_over_f_p = f / f_p;
   double temp_f_p   = (epsilon_0 - epsilon_1) / (1.0 + (f_over_f_p * f_over_f_p));
   double f_over_f_s = f / f_s;
   double temp_f_s   = (epsilon_1 - epsilon_2) / (1.0 + (f_over_f_s * f_over_f_s));

   // (Equation 4)
   double epsilon_dp = (f_over_f_p * temp_f_p) + (f_over_f_s * temp_f_s);

   // (Equation 5)
   double epsilon_p = temp_f_p + temp_f_s + epsilon_2;

   // (Equation 3)
   double eta = (2.0 + epsilon_p) / epsilon_dp;

   // (Equation 2)
   double K_l = (0.819 * f) / (epsilon_dp * (1.0 + eta * eta)); // dB/km

   // (Equation 1)
   double gamma_c = K_l * waterDensity; // (dB/km) / (g/m^3)

   return gamma_c;
}

// =================================================================================================
// Implementation of the ITU Standard Atmosphere as defined in ITU-R P.835.
//
// This is almost identical to the US Standard Atmosphere 1976 as implemented in UtAtmosphere
// except that the sea level pressure is slightly different.
// =================================================================================================

namespace
{
inline double Temperature(double h,   // Altitude,
                          double H_i, // Base Altitude for layer
                          double T_i, // Base Temperature layer
                          double L_i) // Gradient for layer
{
   // Equation 1, for temperature (deg K)
   return T_i + L_i * (h - H_i);
}

inline double Pressure(double h, double T, double H_i, double P_i, double T_i, double L_i)
{
   // P[i] = Pressure(H[i], T[i], H[i-1], P[i-1], T[i-1], L[i-1]);
   // Equation 2 or 3, for pressure (hPa)
   double P_h;
   if (L_i != 0.0)
   {
      // P_h = P_i * pow(T_i / (T_i + L_i * (h - H_i)), 34.163 / L_i);
      P_h = P_i * pow(T_i / T, 34.163 / L_i);
   }
   else
   {
      P_h = P_i * exp((-34.163 * (h - H_i)) / T_i);
   }
   return P_h;
}
} // namespace

// =================================================================================================
//! Given an altitude, compute the required atmospheric data.
//!
//! This will compute the required data using either the standard atmosphere model from UtAtmosphere,
//! or using standard atmosphere model from ITU-R P.835-4.
// static
void WsfEM_ITU_Attenuation::ComputeAtmosphereData(UtAtmosphere& aAtmosphere,
                                                  double        aAltitude,
                                                  double&       aPressure,
                                                  double&       aTemperature,
                                                  double&       aWaterVaporDensity)
{
#if defined(USE_UTIL_ATMOSPHERE)
   // UTIL atmosphere

   aPressure          = aAtmosphere.Pressure(aAltitude);
   aTemperature       = aAtmosphere.Temperature(aAltitude);
   aWaterVaporDensity = aAtmosphere.WaterVaporDensity(aAltitude);

#else
   // ITU-R P.835-4 atmosphere

   static const double rho_0 = 7.5; // Equation 7, g/m^2
   static const double H[]   = {0.0, 11.0, 20.0, 32.0, 47.0, 51.0, 71.0, 85.0};
   static const double L[]   = {-6.5, 0.0, 1.0, 2.8, 0.0, -2.8, -2.0, 999.0};
   static double       T[]   = {288.15, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
   static double       P[]   = {1013.25, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

   static bool firstTime = true;
   if (firstTime)
   {
      for (int i = 1; i <= 7; ++i)
      {
         T[i] = Temperature(H[i], H[i - 1], T[i - 1], L[i - 1]);
         P[i] = Pressure(H[i], T[i], H[i - 1], P[i - 1], T[i - 1], L[i - 1]);
      }
      firstTime = false;
   }

   double h = aAltitude * 0.001; // m -> km
   int    i = 0;
   if (h < 11.0)
   {
      h = std::max(h, 0.0);
      i = 0;
   }
   else if (h < 20.0)
   {
      i = 1;
   }
   else if (h < 32.0)
   {
      i = 2;
   }
   else if (h < 47.0)
   {
      i = 3;
   }
   else if (h < 51.0)
   {
      i = 4;
   }
   else if (h < 71.0)
   {
      i = 5;
   }
   else if (h < 85.0)
   {
      i = 6;
   }
   else
   {
      aTemperature       = 0.0;
      aPressure          = 0.0;
      aWaterVaporDensity = 0.0;
      return;
   }

   // Equation 1, for temperature (deg K)
   double T_h = Temperature(h, H[i], T[i], L[i]);

   // Equation 2 or 3, for pressure (hPa)
   double P_h = Pressure(h, T_h, H[i], P[i], T[i], L[i]);

   // Equation 6, for water vapor density. h_0 = 2 km
   double rho_h = rho_0 * exp(-h / 2.0);

   aTemperature       = T_h;
   aPressure          = P_h * 100.0;   // hPa -> Pa
   aWaterVaporDensity = rho_h * 0.001; // g/m^3 -> kg/m^3
#endif
}

// =================================================================================================
// All of the remaining methods are simply for generating GNUPLOT files for comparison with figures
// from the ITU documement.
// =================================================================================================

// =================================================================================================
//! Generate a GNUPLOT data and command file for Figure 1 from ITU-R P.676
//! The output files are gas_figure_1.plt (the data) and gas_figure_1.gnu (the GNUPLOT command file).
// static
void WsfEM_ITU_Attenuation::PlotGasFigure1()
{
   double pressure          = 101300.0;
   double temperature       = 15.0 + 273.15;
   double waterVaporDensity = 7.5 * 0.001; // g/m^3 * (1kg / 1000gm) = kg/m^3

   std::ofstream pltOfs("gas_figure_1.plt");
   pltOfs << "# ITU-R P.676, Figure 1\n";
   pltOfs << "# freq (ghz) gamma(wet) gamma(dry)\n";
   for (int ifreq = 1; ifreq <= 1000; ++ifreq)
   {
      double frequency = ifreq * 1.0E+9;
      double atnWet    = ComputeGasSpecificAttenuation(frequency, pressure, temperature, waterVaporDensity);
      double atnDry    = ComputeGasSpecificAttenuation(frequency, pressure, temperature, 0.0);
      pltOfs << ifreq << '\t' << atnWet << '\t' << atnDry << '\n';
   }

   std::ofstream cmdOfs("gas_figure_1.gnu");
   cmdOfs << "reset\n";
   cmdOfs << "set title \"Specific attenuation due to atmospheric gases\\n"
             "(Standard: 7.5 g/m^3; Dry: 0 g/m^3)\"\n";
   cmdOfs << "set xlabel \"Frequency (GHz)\"\n";
   cmdOfs << "set ylabel \"Specific Attenuation (dB/km)\"\n";
   cmdOfs << "set key outside\n";
   cmdOfs << "set logscale y\n";
   cmdOfs << "set mytics 10\n";
   cmdOfs << "set grid xtics ytics mytics linetype -1,linetype 0\n";
   cmdOfs << "plot \"gas_figure_1.plt\" using 1:2 title \"standard\" with lines linewidth 2,\\\n";
   cmdOfs << "     \"\" using 1:3 title \"dry\" with lines linewidth 2\n";
}

// =================================================================================================
//! Generate a GNUPLOT data and command file for Figure 2 from ITU-R P.676
//! The output files are gas_figure_2.plt (the data) and gas_figure_2.gnu (the GNUPLOT command file).
// static
void WsfEM_ITU_Attenuation::PlotGasFigure2()
{
   UtAtmosphere atmosphere(UtAtmosphere::CreateNewAtmosphereTable());
   double       pressure;
   double       temperature;
   double       waterVaporDensity;

   std::ofstream ofsPlt("gas_figure_2.plt");
   ofsPlt << "# ITU-R P.676, Figure 2\n";
   for (int ialt = 0; ialt <= 20; ialt += 5)
   {
      double altitude = ialt * 1000.0;
      if (ialt != 0)
      {
         ofsPlt << "\n\n";
      }
      ComputeAtmosphereData(atmosphere, altitude, pressure, temperature, waterVaporDensity);
      waterVaporDensity = 0.0;
      ofsPlt << "# Altitude=" << ialt << " km; pressure=" << pressure << " Pa; temperature=" << temperature << " deg K"
             << std::endl;
      for (int ifreq = 5000; ifreq <= 7000; ifreq += 2)
      {
         double freqGHZ   = ifreq * 0.01;
         double frequency = freqGHZ * 1.0E+9;
         double atn       = ComputeGasSpecificAttenuation(frequency, pressure, temperature, waterVaporDensity);
         ofsPlt << freqGHZ << '\t' << atn << std::endl;
      }
   }

   std::ofstream cmdOfs("gas_figure_2.gnu");
   cmdOfs << "reset\n";
   cmdOfs << "set title \"Specific attenuation in the range 50-70 GHz\\n"
             "(0 km, 5 km, 10 km, 15 km and 20 km)\"\n";
   cmdOfs << "set xlabel \"Frequency (GHz)\"\n";
   cmdOfs << "set ylabel \"Specific Attenuation (dB/km)\"\n";
   cmdOfs << "set xtics 2\n";
   cmdOfs << "set logscale y\n";
   cmdOfs << "set mytics 10\n";
   cmdOfs << "set grid xtics ytics mytics linetype -1,linetype 0\n";
   cmdOfs << "plot \"gas_figure_2.plt\" notitle with lines linewidth 2\n";
}

// =================================================================================================
//! Generate a GNUPLOT data and command file for Figure 3 from ITU-R P.676
//! The output files are gas_figure_3.plt (the data) and gas_figure_3.gnu (the GNUPLOT command file).
// static
void WsfEM_ITU_Attenuation::PlotGasFigure3()
{
   UtAtmosphere atmosphere(UtAtmosphere::CreateNewAtmosphereTable());
   double       pressure;
   double       temperature;
   double       waterVaporDensity;

   std::ofstream pltOfs("gas_figure_3.plt");
   pltOfs << "# ITU-R P.676, Figure 3\n";
   pltOfs << "# freq (ghz) gamma(wet) gamma(dry)\n";
   for (int ifreq = 1; ifreq <= 1000; ++ifreq)
   {
      double frequency = ifreq * 1.0E+9;
      double attenDry  = 0.0;
      double attenWet  = 0.0;
      double altitude  = 0.0;
      ComputeAtmosphereData(atmosphere, altitude, pressure, temperature, waterVaporDensity);
      double gammaWet = ComputeGasSpecificAttenuation(frequency, pressure, temperature, waterVaporDensity);
      double gammaDry = ComputeGasSpecificAttenuation(frequency, pressure, temperature, 0.0);
      for (int ialt = 1; ialt < 100; ++ialt)
      {
         double lastAltitude = altitude;
         altitude            = ialt * 1000.0;
         ComputeAtmosphereData(atmosphere, altitude, pressure, temperature, waterVaporDensity);
         if ((pressure <= 0.0) || (temperature <= 0.0))
         {
            break;
         }
         double lastGammaWet = gammaWet;
         double lastGammaDry = gammaDry;
         gammaWet            = ComputeGasSpecificAttenuation(frequency, pressure, temperature, waterVaporDensity);
         gammaDry            = ComputeGasSpecificAttenuation(frequency, pressure, temperature, 0.0);
         double range        = (altitude - lastAltitude);
         attenWet += (0.5 * (lastGammaWet + gammaWet) * range * 0.001);
         attenDry += (0.5 * (lastGammaDry + gammaDry) * range * 0.001);
      }
      pltOfs << ifreq << '\t' << attenWet << '\t' << attenDry << '\n';
   }

   std::ofstream cmdOfs("gas_figure_3.gnu");
   cmdOfs << "reset\n";
   cmdOfs << "set title \"Zenith attenuation due to atmospheric gases\\n"
             "(Standard: 7.5 g/m^3; Dry: 0 g/m^3)\"\n";
   cmdOfs << "set xlabel \"Frequency (GHz)\"\n";
   cmdOfs << "set ylabel \"Zenith Attenuation (dB)\"\n";
   cmdOfs << "set key outside\n";
   cmdOfs << "set logscale y\n";
   cmdOfs << "set mytics 10\n";
   cmdOfs << "set grid xtics ytics mytics linetype -1,linetype 0\n";
   cmdOfs << "plot \"gas_figure_3.plt\" using 1:2 title \"standard\" with lines linewidth 2,\\\n";
   cmdOfs << "     \"\" using 1:3 title \"dry\" with lines linewidth 2\n";
}

// =================================================================================================
//! Generate a GNUPLOT data and command file for Figure 1 from ITU-R P.840.
//! The output files are cloud_figure_1.plt (the data) and cloud_figure_1.gnu (the GNUPLOT command file).
// static
void WsfEM_ITU_Attenuation::PlotCloudFigure1()
{
   double waterDensity  = 1.0 * 0.001;   // g/m^3 * (1kg / 1000gm) = kg/m^3
   double temperature_1 = -8.0 + 273.15; // -8 deg-C
   double temperature_2 = 0.0 + 273.15;  //  0 deg-C
   double temperature_3 = 10.0 + 273.15; // 10 deg-C
   double temperature_4 = 20.0 + 273.15; // 20 deg-C

   std::ofstream pltOfs("cloud_figure_1.plt");
   pltOfs << "# ITU-R P.840, Figure 1\n";
   for (int ifreq = 5; ifreq <= 200; ++ifreq)
   {
      double frequency = ifreq * 1.0E+9;

      // Computing gamma_c for 1 g/m^3 will give us K_l

      double gamma_c_1 = ComputeCloudSpecificAttenuation(frequency, temperature_1, waterDensity);
      double gamma_c_2 = ComputeCloudSpecificAttenuation(frequency, temperature_2, waterDensity);
      double gamma_c_3 = ComputeCloudSpecificAttenuation(frequency, temperature_3, waterDensity);
      double gamma_c_4 = ComputeCloudSpecificAttenuation(frequency, temperature_4, waterDensity);
      pltOfs << ifreq << '\t' << gamma_c_1 << '\t' << gamma_c_2 << '\t' << gamma_c_3 << '\t' << gamma_c_4 << '\n';
   }

   std::ofstream cmdOfs("cloud_figure_1.gnu");
   cmdOfs << "reset\n";
   cmdOfs << "set title \"Specific attenuation due to water droplets at various\\ntemperatures as a function of "
             "frequency\"\n";
   cmdOfs << "set xlabel \"Frequency (GHz)\"\n";
   cmdOfs << "set ylabel \"Specific attenuation coefficient, K_l (dB/km) / (g/m^3)\"\n";
   cmdOfs << "set key outside\n";
   cmdOfs << "set logscale xy\n";
   cmdOfs << "set xtics (5,\"\" 6 1,\"\" 7 1,\"\" 8 1,10,\"\" 15 1,\\\n";
   cmdOfs << "           20,\"\" 30 1,\"\" 40 1,50,\"\" 60 1,\"\" 70 1,\"\" 80 1,\"\" 90 1,100,\"\" 150 1, 200)\n";
   cmdOfs << "set ytics (0.01,\"\" 0.015 1,0.02,\"\" 0.03 1,\"\" 0.04 1,0.05,\"\" 0.06 1,\"\" 0.07 1,\"\" 0.08 1,\"\" "
             "0.09 1,\\\n";
   cmdOfs
      << "           0.1,\"\" 0.15 1,0.2,\"\" 0.3 1,\"\" 0.4 1,0.5,\"\" 0.6,1,\"\" 0.7 1,\"\" 0.8 1,\"\" 0.9 1,\\\n";
   cmdOfs << "           1,\"\" 1.5 1,2,\"\" 3 1,\"\" 4 1,5,\"\" 6 1,\"\" 7 1,\"\" 8 1,\"\" 9 1,10)\n";
   cmdOfs << "set xrange [5:200]\n";
   cmdOfs << "set yrange [0.01:10]\n";
   cmdOfs << "set grid xtics ytics mxtics mytics linetype -1,linetype 0\n";
   cmdOfs << "plot \"cloud_figure_1.plt\" using 1:2 title \"-8 deg C\" with lines linewidth 2,\\\n";
   cmdOfs << "     \"\" using 1:3 title \"0 deg C\" with lines linewidth 2,\\\n";
   cmdOfs << "     \"\" using 1:4 title \"10 deg C\" with lines linewidth 2,\\\n";
   cmdOfs << "     \"\" using 1:5 title \"20 deg C\" with lines linewidth 2\n";
}
