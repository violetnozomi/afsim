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

#ifndef P6DOFATMOSPHERE_H
#define P6DOFATMOSPHERE_H

#include "p6dof_export.h"

#include <string>

#include "UtCloneablePtr.hpp"
#include "UtTable.hpp"

class UtInput;
class UtVec3dX;

//! P6DofAtmosphere provides atmospheric data for Pseudo 6DOF objects.

class P6DOF_EXPORT P6DofAtmosphere
{
public:
   P6DofAtmosphere();
   explicit P6DofAtmosphere(const std::string& aFilename);

   P6DofAtmosphere& operator=(const P6DofAtmosphere& other) = delete;

   P6DofAtmosphere* Clone() const;

   ~P6DofAtmosphere() = default;

   // SpeedOfSoundAtAltitude_fps returns the speed of sound at the specified
   // altitude (ft).
   double SpeedOfSoundAtAltitude_fps(double aAltitude_ft) const;

   // AirDensityAtAltitude returns the density of air at the specified
   // altitude (ft). Density is specified in units of
   // [ ((lbf*sec^2)/ft^4) or slugs/ft^3 ]
   double AirDensityAtAltitude(double aAltitude_ft) const;

   // StaticPressureAtAltitude returns the static pressure at the specified
   // altitude (ft). Static Pressure is specified in units of (lbs-force)/(ft^2)
   double StaticPressureAtAltitude(double aAltitude_ft) const;

   // TemperatureAtAltitude_F returns the temperature at the specified
   // altitude (ft). Temperature is specified in units of degrees Fahrenheit
   double TemperatureAtAltitude_F(double aAltitude_ft) const;

   // TemperatureAtAltitude_R returns the temperature at the specified
   // altitude (ft). Temperature is specified in units of degrees Rankine
   double TemperatureAtAltitude_R(double aAltitude_ft) const;

   // CalcMachAtAltitude returns the Mach for a vehicle at the specified
   // altitude (ft) and speed (ft/sec)
   double CalcMachAtAltitude(double aAltitude_ft, double aSpeed_fps) const;

   // DensityRatioAtAltitude returns the density pressure ratio (often
   // called sigma) for a vehicle at the specified altitude (ft)
   double DensityRatioAtAltitude(double aAltitude_ft) const;

   // StaticPressureRatioAtAltitude returns the static pressure ratio (often
   // called delta) for a vehicle at the specified altitude (ft)
   double StaticPressureRatioAtAltitude(double aAltitude_ft) const;

   // TemperatureRatioAtAltitude returns the temperature ratio (often
   // called theta) for a vehicle at the specified altitude (ft)
   double TemperatureRatioAtAltitude(double aAltitude_ft) const;

   // GetAeroParametersAtAltitude returns several parameters for a vehicle
   // at the specified altitude (ft) and speed (ft/sec). Parameters
   // include air density [ ((lbf*sec^2)/ft^4) or slugs/ft^3 ],
   // dynamic pressure (lbf/sq-ft), static pressure (lbf/sq-ft), and Mach.
   void GetAeroParametersAtAltitude(double  aAltitude_ft,
                                    double  aSpeed_fps,
                                    double& aAirDensity,
                                    double& aDynamicPressure_psf,
                                    double& aStaticPressure_psf,
                                    double& aMach) const;

   // This returns the KTAS based on the specified KCAS/KIAS and altitude
   double CalcKtasFromKcas(double aAltitude_ft, double aSpeed_kcas) const;

   // This returns the KTAS based on the specified mach and altitude
   double CalcKtasFromMach(double aAltitude_ft, double aSpeed_mach) const;

   // This returns the KTAS based on the specified speed in fps
   double CalcKtasFromFps(double aSpeed_fps) const;

   // This returns the KCAS/KIAS based on the specified KTAS and altitude
   double CalcKcasFromKtas(double aAltitude_ft, double aSpeed_ktas) const;

   // This returns the KCAS/KIAS based on the specified mach and altitude
   double CalcKcasFromMach(double aAltitude_ft, double aSpeed_mach) const;

   // This returns the KCAS/KIAS based on the specified speed in fps and altitude
   double CalcKcasFromFps(double aAltitude_ft, double aSpeed_fps) const;

   // This returns the mach based on the specified KCAS/KIAS and altitude
   double CalcMachFromKcas(double aAltitude_ft, double aSpeed_kcas) const;

   // This returns the mach based on the specified KTAS and altitude
   double CalcMachFromKtas(double aAltitude_ft, double aSpeed_ktas) const;

   // This returns the mach based on the specified speed in fps and altitude
   double CalcMachFromFps(double aAltitude_ft, double aSpeed_fps) const;

   // This returns the speed in fps based on the specified KCAS/KIAS and altitude
   double CalcFpsFromKcas(double aAltitude_ft, double aSpeed_kcas) const;

   // This returns the speed in fps based on the specified KTAS
   double CalcFpsFromKtas(double aSpeed_ktas) const;

   // This returns the speed in fps based on the specified mach and altitude
   double CalcFpsFromMach(double aAltitude_ft, double aSpeed_mach) const;

   // This returns the dynamic pressure in lbs/sq-ft based on the specified speed
   // in fps and altitude
   double CalcDynamicPressure_lbft2(double aAltitude_ft, double aSpeed_fps) const;

   // This returns the speed in fps based on the specified altitude and
   // dynamic pressure
   double CalcFpsFromAltitudeDynamicPressure(double aAltitude_ft, double aPressure_lbft2) const;

   // This returns the altitude band where contrailing will occur. Returns true
   // is contrailing conditions exist, false if no contrailing will occur.
   bool ContrailAltitudeBand_ft(double& aLowAltitude_ft, double& aHighAltitude_ft) const;

   // This returns true if the specified altitude is within the contrailing band. If
   // it is not within the band or contrailing conditions do not exist, it returns false.
   bool WithinContrailAltitudeBand(double aAltitude_ft) const;

   bool ProcessInput(UtInput& aInput);

private:
   P6DofAtmosphere(const P6DofAtmosphere& aSrc) = default;

   UtCloneablePtr<UtTable::Curve> mDensityAltitudeTablePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mPressureAltitudeTablePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mSpeedOfSoundAltitudeTablePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mTemperatureAltitudeTablePtr{nullptr};

   bool   mContrailingConditionsExist;
   double mMinContrailingAltitude_ft;
   double mMaxContrailingAltitude_ft;
};

#endif
