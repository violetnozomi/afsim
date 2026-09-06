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

#ifndef WSFSIXDOFENVIRONMENT_HPP
#define WSFSIXDOFENVIRONMENT_HPP

#include "wsf_six_dof_export.h"

#include "UtAtmosphere.hpp"
class UtInput;
#include "UtVec3dX.hpp"
#include "WsfEnvironment.hpp"
#include "WsfScenarioExtension.hpp"
class WsfSimulation;
#include "WsfSixDOF_Terrain.hpp"

namespace wsf
{
namespace six_dof
{
class Environment : public WsfScenarioExtension
{
public:
   Environment()  = default;
   ~Environment() = default;

   // The extension has been added to the scenario. GetScenario() is now callable.
   void AddedToScenario() override;

   // This reads "terrain" and "integrators" input blocks
   bool ProcessInput(UtInput& aInput) override;

   UtVec3dX GetWindNED_mps(double aLatitude, double aLongitude, double aAltitude_m) const
   {
      double heading_rad;
      double speed_mps;
      mEnvironment->GetWind(aLatitude, aLongitude, aAltitude_m, heading_rad, speed_mps);

      return UtVec3dX(heading_rad, 0.0, speed_mps, true);
   }

   double GetTerrainElevation(double aLatitude, double aLongitude) const
   {
      return mTerrain->HeightOfTerrain_m(aLatitude, aLongitude);
   }

   double GetTerrainDataElevationAndData(double    aLatitude,
                                         double    aLongitude,
                                         UtVec3dX& aUpVectorNED,
                                         UtVec3dX& aSpeedVectorNED_mps,
                                         double&   aFrictionMultiplier) const
   {
      return mTerrain->HeightOfTerrain_m(aLatitude, aLongitude, aUpVectorNED, aSpeedVectorNED_mps, aFrictionMultiplier);
   }

   // GetAeroParameters updates several parameters for a vehicle
   // at the specified altitude (ft) and speed (ft/sec). Parameters
   // include air density [ ((lbf*sec^2)/ft^4) or slugs/ft^3 ],
   // dynamic pressure (lbf/sq-ft), and static pressure (lbf/sq-ft).
   void GetAeroParameters(double  aAltitude_ft,
                          double  aSpeed_fps,
                          double& aAirDensity,
                          double& aDynamicPressure_psf,
                          double& aStaticPressure_psf) const;

   double CalcAirDensity_slug_ft3(double aAltitude_ft) const
   {
      return mAtmosphere->CalcAirDensity_slug_ft3(aAltitude_ft);
   }

   double CalcAmbientPressure_psf(double aAltitude_ft) const
   {
      return mAtmosphere->CalcAmbientPressure_psf(aAltitude_ft);
   }

   double CalcKtasFromKcas(double aAltitude_m, double aSpeed_kcas) const
   {
      return mAtmosphere->CalcKtasFromKcas(aAltitude_m, aSpeed_kcas);
   }

   double CalcKcasFromKtas(double aAltitude_m, double aSpeed_ktas) const
   {
      return mAtmosphere->CalcKcasFromKtas(aAltitude_m, aSpeed_ktas);
   }

   double CalcKtasFromMach(double aAltitude_m, double aSpeed_mach) const
   {
      return mAtmosphere->CalcKtasFromMach(aAltitude_m, aSpeed_mach);
   }

   double CalcMachFromKtas(double aAltitude_m, double aSpeed_ktas) const
   {
      return mAtmosphere->CalcMachFromKtas(aAltitude_m, aSpeed_ktas);
   }

   double CalcKcasFromMach(double aAltitude_m, double aSpeed_mach) const
   {
      return mAtmosphere->CalcKcasFromMach(aAltitude_m, aSpeed_mach);
   }

   double CalcMachFromKcas(double aAltitude_m, double aSpeed_kcas) const
   {
      return mAtmosphere->CalcMachFromKcas(aAltitude_m, aSpeed_kcas);
   }

   double CalcKcasFromFps(double aAltitude_m, double aSpeed_fps) const
   {
      return mAtmosphere->CalcKcasFromFps(aAltitude_m, aSpeed_fps);
   }

   double CalcFpsFromKcas(double aAltitude_m, double aSpeed_kcas) const
   {
      return mAtmosphere->CalcFpsFromKcas(aAltitude_m, aSpeed_kcas);
   }

   double CalcMachFromFps(double aAltitude_m, double aSpeed_fps) const
   {
      return mAtmosphere->CalcMachFromFps(aAltitude_m, aSpeed_fps);
   }

   double CalcFpsFromMach(double aAltitude_m, double aSpeed_mach) const
   {
      return mAtmosphere->CalcFpsFromMach(aAltitude_m, aSpeed_mach);
   }

   double CalcFpsFromDynamicPressure(double aAltitude_m, double aPressure_psf) const
   {
      return mAtmosphere->CalcFpsFromDynamicPressure(aAltitude_m, aPressure_psf);
   }

   double CalcDynamicPressure_psf(double aAltitude_m, double aSpeed_fps) const
   {
      return mAtmosphere->CalcDynamicPressure_psf(aAltitude_m, aSpeed_fps);
   }

   // This returns the ratio between ambient air density at altitude and sea-level air density
   double CalcDensityRatio(double aAltitude_m) const { return mAtmosphere->DensityRatio(aAltitude_m); }

   // This returns true if an operating engine can produce contrails at an altitude
   bool IsWithinContrailBand(double aAltitude_m) const
   {
      return mAtmosphere->IsWithinContrailAltitudeBand(aAltitude_m);
   }

protected:
private:
   WsfEnvironment* mEnvironment =
      nullptr; // Used to serve wind data to SixDOF movers; may eventually serve visibility data
   UtAtmosphere*            mAtmosphere = nullptr; // Contains infomation relevant to pressure, density, Mach
   std::unique_ptr<Terrain> mTerrain;              // Augments the WSF terrain interface
};
} // namespace six_dof
} // namespace wsf

#endif
