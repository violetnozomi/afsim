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

#include "WsfDefaultHEL.hpp"

#include <cmath>
#include <sstream>

#include "UtIntegrand.hpp"
#include "UtIntegrate.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfOpticalPath.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

WsfDefaultHEL::WsfDefaultHEL(WsfScenario& aScenario)
   : WsfHighEnergyLaser(aScenario)
   , mAtmosphereModel(2)
   , mHazeModel(1)
{
}

// virtual
bool WsfDefaultHEL::ProcessInput(UtInput& aInput)
{
   std::string command   = aInput.GetCommand();
   bool        myCommand = true;
   if (command == "atmosphere_model")
   {
      aInput.ReadValue(mAtmosphereModel);
      if ((mAtmosphereModel < 1) || (mAtmosphereModel > 6))
      {
         auto out = ut::log::error() << "Invalid atmosphere model number.";
         out.AddNote() << "Actual: " << mAtmosphereModel;
         out.AddNote() << "Expected: 1 <= x <= 6";

         throw(UtInput::BadValue(aInput));
      }
   }
   else if ((command == "haze") || (command == "haze_model"))
   {
      aInput.ReadValue(mHazeModel);
      if ((mHazeModel < 1) || (mHazeModel > 5))
      {
         auto out = ut::log::error() << "Invalid haze model number.";
         out.AddNote() << "Actual: " << mHazeModel;
         out.AddNote() << "Expected: 1 <= x <= 5";
         throw(UtInput::BadValue(aInput));
      }
   }
   else
   {
      myCommand = WsfHighEnergyLaser::ProcessInput(aInput);
   }

   return myCommand;
}

// virtual
void WsfDefaultHEL::Propagate(double aSimTime)
{
   auto out = ut::log::debug();
   if (mWeaponPtr->DebugEnabled())
   {
      WsfPlatform* targetPtr = GetSimulation()->GetPlatformByIndex(mTargetIndex);
      if (targetPtr != nullptr)
      {
         std::ostringstream oss;
         oss << "T: " << aSimTime << " " << mWeaponPtr->GetPlatform()->GetName() << ":Default HEL  "
             << "Firing laser against " << targetPtr->GetName() << ", Range: " << GetTargetRange();

         out << "Default HEL firing laser.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << mWeaponPtr->GetPlatform()->GetName();
         out.AddNote() << "Weapon: " << mWeaponPtr->GetName();
         out.AddNote() << "Target: " << targetPtr->GetName();
         out.AddNote() << "Range: " << GetTargetRange();

         GetWeapon()->GetPlatform()->Comment(aSimTime, oss.str());
      }
   }

   if (!mTargetOccluded)
   {
      PropagateP(aSimTime);
   }
   else if (mWeaponPtr->DebugEnabled())
   {
      out.AddNote() << "Target Occluded. No effect.";
      GetWeapon()->GetPlatform()->Comment(aSimTime, "Target occluded: no effect");
   }
}

//!  Calculate power within the target damage radius
//!@param aPeak  [out]: peak irradiance (W/m^2)
//!@param aEdge  [out]: edge irradiance (W/m^2)
//!@param aSigma [out]: beam divergence (m)
//!@param aPowerEnc [out]: Power Enclosed in target radius (from DynamicSpear PowerEng function).
//!
void WsfDefaultHEL::PropagateP(double aSimTime)
{
   // See HELSim white paper for a discussion of this algorithm
   // First calculate the three contributions to beam spread.
   // First, diffraction spreading (a_d; "a sub d")
   // Ad ~= z * Theta_d
   double theta_d = mBeamQuality * GetWavelength() / (GetApertureDiameter() / 4.0) / UtMath::cTWO_PI;
   double a_d     = GetTargetRange() * theta_d;

   // Next calculate the jitter spread
   static const double sqrtTwo = sqrt(2.0);
   double              a_j     = sqrtTwo * GetTargetRange() * GetJitter(); // todo use a_j^2

   // Now calculate spherical wave rho_0 for the path and turbulence beam spread
   WsfPlatform* targetPtr = GetSimulation()->GetPlatformByIndex(GetTargetIndex());
   if (targetPtr != nullptr)
   {
      double targetLat;
      double targetLon;
      double targetAlt;
      targetPtr->GetLocationLLA(targetLat, targetLon, targetAlt);
      double lat;
      double lon;
      double alt;
      GetWeapon()->GetLocationLLA(lat, lon, alt);

      double a_t = GetTargetRange() * mTurbulenceModel.BeamSpreadAngle(GetTargetRange(), targetAlt, alt, GetWavelength());

      // Find RMS of these three values - 1/e radius of beam
      double spotRadius = sqrt(a_d * a_d + a_j * a_j + a_t * a_t);

      // Now treat the propagation
      // need to optimize this (refactor)
      WsfOpticalPath path(WsfOpticalPathCoefficientTypes::Get(GetSimulation()->GetScenario()),
                          GetWavelength_nm(),
                          mAtmosphereModel,
                          mHazeModel);
      double         attenuation = path.GetAttenuation(GetTargetRange(), targetAlt, alt);

      // Find the peak irradiance
      double cosIncidenceAngle = std::max(1.0e-6, cos(GetIncidenceAngle()));
      double spotArea          = UtMath::cPI * spotRadius * spotRadius / cosIncidenceAngle;

      // double powerAtTarget = GetInitialPower() * attenuation;
      double damageRadius = GetDamageRadius();

      double commonExponential = 1.0 / UtMath::cE;
      if (damageRadius == 0.0)
      {
         // Not using a predefined damage radius.
         // Use the full beam width out to 1/e Gaussian height.
         damageRadius = spotRadius;
      }
      else // Using a defined damage radius.
      {
         commonExponential = exp(-pow(damageRadius / spotRadius, 2));
      }

      double peakIrradiance =
         (GetInitialPower() * attenuation) / spotArea;         // equation 1 in Gebhardt; the cos(IncdenceAngle)
                                                               // is included in the spotArea calculation (above).
      double powerInBucket = GetInitialPower() * attenuation * // Based on a somewhat simplistic assumption to only
                             cosIncidenceAngle *               // reduce the peak irradiance and treat the transverse
                             (1.0 - commonExponential); // and longitudinal dimensions the same when performing the
                                                        // integration.
      double edgeIrradiance = peakIrradiance * commonExponential; // transverse irradiance, no cos(IncidenceAngle) term.

      SetSpotRadius(spotRadius);
      SetEdgeRadius(damageRadius);
      SetPeakIrradiance(peakIrradiance);
      SetAveragePower(powerInBucket);
      SetEdgeIrradiance(edgeIrradiance);

      if (GetDamageRadius() > 0.0)
      {
         SetAverageIrradiance(powerInBucket / (UtMath::cPI * damageRadius * damageRadius));
      }
      else // using spot area, already calculated
      {
         SetAverageIrradiance(powerInBucket / spotArea);
      }

      double dt = aSimTime - mLastUpdateTime;
      SetEnergy(GetEnergy() + GetAveragePower() * dt);
      SetPeakFluence(GetPeakFluence() + peakIrradiance * dt);
      SetEdgeFluence(GetEdgeFluence() + edgeIrradiance * dt);

      if (mWeaponPtr->DebugEnabled())
      {
         std::ostringstream oss;
         oss << " Peak:  " << GetPeakFluence() << std::endl;
         oss << " Sigma: " << spotRadius / sqrtTwo << std::endl;
         oss << " Edge:  " << GetEdgeFluence() << std::endl;
         oss << " Power in " << GetDamageRadius() << " bucket: " << GetAveragePower() << std::endl;
         oss << " Accumulated Energy: " << GetEnergy() / 1000.0 << " kJ." << std::endl;
         oss << " Accumulated Energy Density " << GetEnergyDensity() / 1.0e+7 << " kJ/cm^2.";

         { // RAII block
            auto out = ut::log::debug() << "Default HEL propagated.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Peak: " << GetPeakFluence();
            out.AddNote() << "Sigma: " << spotRadius / sqrtTwo;
            out.AddNote() << "Edge: " << GetEdgeFluence();
            out.AddNote() << "Dmg. Radius: " << GetDamageRadius() << " m";
            out.AddNote() << "Avg. Power: " << GetAveragePower() << " W";
            out.AddNote() << "Acc. Energy: " << GetEnergy() / 1000.0 << " kJ";
            out.AddNote() << "Acc. Energy Density: " << GetEnergyDensity() / 1.0e+7 << " kJ/cm^2";
         }

         GetWeapon()->GetPlatform()->Comment(aSimTime, oss.str());
      }
   }
}
