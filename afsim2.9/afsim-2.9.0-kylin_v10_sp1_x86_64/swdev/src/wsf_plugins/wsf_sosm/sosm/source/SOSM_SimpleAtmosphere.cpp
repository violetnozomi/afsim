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

#include "SOSM_SimpleAtmosphere.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "SOSM_Interaction.hpp"
#include "SOSM_Selector.hpp"
#include "UtInput.hpp"

// =================================================================================================
SOSM_SimpleAtmosphere::SOSM_SimpleAtmosphere(const UtAtmosphere& aAtmosphere)
   : SOSM_Atmosphere()
   , mAtmosphericAttenuation(0.0)
   , mRhoSeaLevel(0.0)
   , mAtmosphere(aAtmosphere)
{
}

// =================================================================================================
// virtual
SOSM_Atmosphere* SOSM_SimpleAtmosphere::Clone() const
{
   return new SOSM_SimpleAtmosphere(*this);
}

// =================================================================================================
// virtual
bool SOSM_SimpleAtmosphere::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "atmospheric_attenuation")
   {
      std::string per;
      std::string units;
      aInput.ReadValue(mAtmosphericAttenuation);
      aInput.ValueGreater(mAtmosphericAttenuation, 0.0);
      aInput.ReadValue(per);
      aInput.StringEqual(per, "per");
      aInput.ReadValue(units);
      mAtmosphericAttenuation /= aInput.ConvertValueFrom(1.0, units, UtInput::cLENGTH);
   }
   else if (mAtmosphere.ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = SOSM_Atmosphere::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
void SOSM_SimpleAtmosphere::ComputeAtmosphereData(const SOSM_Selector& aSelector,
                                                  SOSM_Interaction&    aInteraction,
                                                  std::vector<float>&  aBackgroundRadiance,
                                                  std::vector<float>&  aForegroundRadiance,
                                                  std::vector<float>&  aForegroundTransmittance)
{
   aSelector.InitializeOutput(aBackgroundRadiance, 1.0F);
   aSelector.InitializeOutput(aForegroundRadiance, 1.0F);
   aSelector.InitializeOutput(aForegroundTransmittance, 1.0F);
   // TODO
}

// =================================================================================================
//! Get the sample points to be used for fast detection mode table generation.
// virtual
void SOSM_SimpleAtmosphere::GetSamplePoints(std::vector<float>& aAltValues,
                                            std::vector<float>& aElValues,
                                            std::vector<float>& aRangeValues)
{
   // TODO
   aAltValues.clear();
   aElValues.clear();
   aRangeValues.clear();
}

#ifdef TODO
// =================================================================================================
// virtual
void SOSM_SimpleAtmosphere::ComputeBackgroundTransmittance(std::vector<float>&  aTransmittance,
                                                           const SOSM_Selector& aSelector,
                                                           SOSM_Interaction&    aInteraction)
{
   aSelector.InitializeOutput(aTransmittance, 1.0);

   if (mAtmosphericAttenuation > 0.0)
   {
      double targetRange   = 1.0E+6;
      double sensorAlt     = aInteraction.GetSensorAltitude();
      double targetEl      = aInteraction.GetAbsoluteTargetElevation();
      double targetAlt     = sensorAlt + (targetRange * sin(targetEl));
      double groundRange   = targetRange * cos(targetEl);
      double transmittance = ComputeSimpleTransmittance(mAtmosphericAttenuation, sensorAlt, targetAlt, groundRange);
      int    oi            = aSelector.OutputStartIndex();
      for (int i = 0; i < aSelector.Count(); ++i)
      {
         aTransmittance[oi] = transmittance;
         ++oi;
      }
   }
   else
   {
      int oi = aSelector.OutputStartIndex();
      for (int i = 0; i < aSelector.Count(); ++i)
      {
         aTransmittance[oi] = 1.0;
         ++oi;
      }
   }
}
#endif

// =================================================================================================
//! Determine the simplistic transmittance along a path.
//!
//! @param aAttenuationPerMeter [input] The attenuation per meter at sea level.
//! @param aHeightMSL_1         [input] The height MSL of the first  object (meters)
//! @param aHeightMSL_2         [input] The height MSL of the second object (meters)
//! @param aGroundRange         [input] The ground range between the objects (meters)
//!
//! @returns The transmittance in the range [0, 1]
double SOSM_SimpleAtmosphere::ComputeSimpleTransmittance(double aAttenuationPerMeter,
                                                         double aHeightMSL_1,
                                                         double aHeightMSL_2,
                                                         double aGroundRange)
{
   if (mRhoSeaLevel <= 0.0)
   {
      // TODO-USE-PRESSURE mRhoSeaLevel = mAtmosphere.Density(0.0);
      mRhoSeaLevel = mAtmosphere.Pressure(0.0);
   }

   // Sort the heights so that the first point is lower than the second point.

   double zMin = std::max(std::min(aHeightMSL_1, aHeightMSL_2), 0.0);
   double zMax = std::max(std::max(aHeightMSL_1, aHeightMSL_2), 0.0);

   // Determine the elevation angle of the path.

   double dz       = zMax - zMin;
   double dg       = aGroundRange;
   double ds       = sqrt(dg * dg + dz * dz);
   double sinTheta = dz / std::max(1.0, ds);
   // double sinTheta = 0.0;
   // if (ds > 1.0)
   //{
   //    sinTheta = dz / ds;
   // }

   // Integrate through the layers.

   // static const double cDELTA_Z = 1000.0;
   static const double cDELTA_Z      = 200.0;
   double              z2            = zMin;
   double              rho2          = mAtmosphere.Density(z2);
   double              transmittance = 1.0;
   bool                done          = false;
   while (!done)
   {
      // Calculate the altitude at the top of the current layer and get the density.

      double z1   = z2;
      double rho1 = rho2;
      z2          = z1 + cDELTA_Z;
      if (z2 >= zMax)
      {
         z2   = zMax;
         done = true;
      }
      // TODO-USE-PRESSURE rho2 = mAtmosphere.Density(z2);
      rho2 = mAtmosphere.Pressure(z2);

      // Calculate the path length in the current layer.

      dz = z2 - z1;
      ds = aGroundRange; // Assume horizontal path...
      if (sinTheta != 0.0)
      {
         ds = dz / sinTheta;
      }

      // Update the transmittance based on the rate of extinction in the current layer.

      double rhoAvg  = 0.5 * (rho1 + rho2);
      auto   logInfo = ut::log::info() << "Transmittance Update Report";
      logInfo.AddNote() << "z1: " << z1;
      logInfo.AddNote() << "z2: " << z2;
      logInfo.AddNote() << "ds: " << ds;
      logInfo.AddNote() << "rhoAvg: " << rhoAvg;
      logInfo.AddNote() << "rho_sl: " << mRhoSeaLevel;
      logInfo.AddNote() << "ratio: " << rhoAvg / mRhoSeaLevel;
      double c = aAttenuationPerMeter * (rhoAvg / mRhoSeaLevel);
      transmittance *= exp(-c * ds);
   }
   return transmittance;
}

#ifdef TODO
// =================================================================================================
// virtual
void SOSM_SimpleBackground::ComputeRadiance(std::vector<float>& aRadiance,
                                            float               aBinStart,
                                            float               aIncrement,
                                            int                 aCount,
                                            double              aDetectorAlt,
                                            double              aTargetEl)
{
   const double cMEAN_SOLAR_RADIUS = 1.392E+9 * 0.5; // m 6.955E+8;     // m
   // const double cSOLAR_SURFACE_AREA = 6.0877E+18;          // m2 (wiki)
   const double cMEAN_EARTH_RADIUS = 6.371E+6; // m
   // const double cEARTH_SURFACE_AREA = 5.10072E+14;         // m2 (wiki)
   const double cMEAN_SOLAR_DISTACE = 1.496E+11; // m

   if (static_cast<int>(aRadiance.size()) < aCount)
   {
      aRadiance.resize(aCount);
   }

   // Compute the elevation angle of the horizon with respect to the sensor location.

   double r;
   r                = cMEAN_EARTH_RADIUS;
   double horizonEl = -acos(r / (r + std::max(1.0, aDetectorAlt)));
   if (aTargetEl >= horizonEl)
   {
      // Target is above the horizon so use the sky.

      // TODO - what to do if the Sun isn't the radiator??? (such as a night sky).

      // Compute the radiant exitance (M, in w/cm2) of the background.
      // The value is temporarily stored in aRadiance to eliminate the need for another <vector>
      mSky.ComputeRadiantExitance(aRadiance, aBinStart, aIncrement, aCount);

      r                = cMEAN_SOLAR_RADIUS * 1E+2; // cm
      double solarArea = UtMath::cFOUR_PI * r * r;  // cm2

      r                    = cMEAN_EARTH_RADIUS * 1.0E+2; // cm
      double earthDiskArea = UtMath::cTWO_PI * r * r;     // cm2

      r                 = cMEAN_SOLAR_DISTACE * 1.0E+2; // cm
      double omegaEarth = earthDiskArea / (r * r);      // sr

      double oneOverPi = 1.0 / UtMath::cPI;
      for (int i = 0; i < aCount; ++i)
      {
         double exitance = aRadiance[i];

         // Total power radiated by the sun in this bin (W)
         double phi = exitance * solarArea;

         // Amount of that power indicent on the Earth (W)
         double phi_eff = omegaEarth / UtMath::cFOUR_PI * phi;

         // Indicent power density (w/cm2)
         exitance = phi_eff / earthDiskArea;

         // Convert to radiance (L, in w/cm2/sr) and store.
         // For a diffuse emitter, L = M / pi.
         aRadiance[i] = static_cast<float>(exitance * oneOverPi);
         ;
      }
   }
   else
   {
      // Target is below the horizon, so the sensor is looking at the 'ground'.

      // Compute the radiant exitance (M, in w/cm2) of the background.
      // The value is temporarily stored in aRadiance to eliminate the need for another <vector>

      mGround.ComputeRadiantExitance(aRadiance, aBinStart, aIncrement, aCount);

      double oneOverPi = 1.0 / UtMath::cPI;
      for (int i = 0; i < aCount; ++i)
      {
         float exitance = aRadiance[i];

         // Convert to radiance (L, in w/cm2/sr) and store.
         // For a diffuse emitter, L = M / pi.
         aRadiance[i] = static_cast<float>(exitance * oneOverPi);
      }
   }
}
#endif
