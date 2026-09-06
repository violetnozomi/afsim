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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#ifndef UTATMOSPHERE_HPP
#define UTATMOSPHERE_HPP

#include "ut_export.h"

#include <iosfwd>
#include <string>

class UtAtmosphereData;
class UtInput;
#include "UtReferenceTracked.hpp"

// ==========================================================================
//! Simple implementation of standard and non-standard atmospheres.
//! References: A) U.S. Std Atmosphere Tables, 1976 (Geopotential Alt)
//!             B) MIL-STD-210A Atmospheres (as taken from
//!                Pratt Whitney Aeronautical Vest Pocket Handbook - Aug 81)
//! Note: Temperature, pressure and density are only valid below 84.852 km

class UT_EXPORT UtAtmosphere
{
public:
   friend class UtAtmosphereData;

   static UtAtmosphere CreateNewAtmosphereTable();

   static void AddRefP(UtAtmosphereData* aDataPtr);

   // ===================== start public STATIC ==========================

   enum AtmosphereType
   {
      cDEFAULT      = -1, //!< Simulation-specified Global Default atmosphere provided
      cSTANDARD_DAY = 0,  //!< Standard Day Atmosphere
      cHOT_DAY      = 1,  //!< MIL-STD-210A Hot Day (approximately +15 K)
      cTROPICAL_DAY = 2,  //!< MIL-STD-210A Tropical Day
      cCOLD_DAY     = 3,  //!< MIL-STD-210A Cold Day (approximately -20 K)
      cPOLAR_DAY    = 4,  //!< MIL-STD-210A Polar Day
      //! "Simple" Non-Standard Atmosphere.
      //! Uniform temperature deviation from standard. (Utilizes a single simulation-wide specification of temperature
      //! deviation value.)
      cSIMPLE_DT = 5,
      cCUSTOM    = 6
   };

   //! Air Specific Weight under Standard Sea Level Conditions, kg / m^3:
   static double cSSL_AIR_DENSITY(); // kg / m^3

   //! Speed of sound at standard sea-level conditions m/s:
   static const double cSSL_SONIC_VELOCITY; // m/s

   //! Atmospheric pressure at Standard Sea Level
   static double cSSL_PRESSURE(); // N/m^2 or Pa

   //! Atmospheric temperature at Standard Sea Level
   static double cSSL_TEMP(); // K

   //! Change the default UtAtmosphere type instantiated using a default constructor.
   // TODO_JAJ_REMOVE static void SetGlobalDefaultAtmosphere(AtmosphereType aAtmosphereType);

   //! Change the delta temperature applied to all cSIMPLE_DT atmosphere instances.
   // TODO_JAJ_REMOVE static void SetSimpleDeltaTempK(double aSimpleDeltaTempK);

   //! If a UtAtmosphere is instantiated with a default constructor, this is the type atmosphere modeled.
   // static AtmosphereType GetGlobalDefaultAtmosphere() { return sGlobalDefaultAtmosphere; }

   //! If a UtAtmosphere is instantiated as a non-standard cSIMPLE_DT type,
   //! the temperature deviation is this value, in deg K.  Static to all instances.
   // static double GetSimpleDeltaTempK() { return sSimpleDeltaTempK; }

   //! Hydrostatic contant (1/m)
   static const double cGMR;

   //! Specific heat ratio for dry air (for now, assume constant 0 - 100C)
   static const double cGAMMA;

   static AtmosphereType IntToAtmosphere(int aValue);

   static bool StringToAtmosphere(const std::string& aAtmosphereString, AtmosphereType& aAtmosphereType);

   static std::string AtmosphereTypeName(int aAtmTypeIndex);

   // ===================== end public STATIC ==========================
   UtAtmosphere(const UtAtmosphere& aSrcAtmosphere, AtmosphereType aAtmosphereType = cDEFAULT);

   virtual ~UtAtmosphere();

   bool ProcessInput(UtInput& aInput);
   bool ProcessGlobalInput(UtInput& aInput);

   // Returns the Geopotential Altitude of the atmosphere, given a Pressure Altitude.
   // double GeopotentialAltitude(double aPressureAltitude) const;

   // Returns the Pressure Altitude of the atmosphere, given a Geopotential Altitude.
   // double PressureAltitude(double aGeopotentialAltitude) const;

   //! Returns the Density Altitude of the atmosphere, given a Geopotential Altitude.
   double DensityAltitude(double aGeopotentialAltitude) const;

   //! Returns the temperature (K) of the atmosphere at the given altitude.
   double Temperature(double aAltitude) const;

   //! Returns the ratio of the temperature to standard sea level conditions at the given altitude.
   double TemperatureRatio(double aAltitude) const;

   //! Returns the pressure (N/m^2, Pascals) of the atmosphere at the given altitude.
   double Pressure(double aAltitude) const;

   //! Returns the pressure ratio of the atmosphere to that of standard sea level air.
   double PressureRatio(double aAltitude) const;

   //! Returns the density (kg/m^3) of the atmosphere at the given altitude.
   double Density(double aAltitude) const;

   //! Returns the density ratio SSL at the given altitude, relative to SSL.
   double DensityRatio(double aAltitude) const;

   //! Returns the speed of sound (meters/second) at the given altitude.
   double SonicVelocity(double aAltitude) const;

   //! Returns the relative humidity at an altitude
   double RelativeHumidity(double aAltitude) const;

   //! Returns the dynamic (or absolute) viscosity (kg/(m-s)) at a given altitude.
   //! Based on Sutherland's law.
   double DynamicViscosity(double aAltitude) const;

   //! Returns the kinematic viscosity (m^2/s) at a given altitude.
   //! This is the dynamic viscosity divided by the density.
   double KinematicViscosity(double aAltitude) const { return DynamicViscosity(aAltitude) / Density(aAltitude); }

   //! Returns the water vapor density at an altitude (kg/m^3)
   double WaterVaporDensity(double aAltitude) const;

   //! Returns true if contrails can form at an altitude
   bool IsWithinContrailAltitudeBand(double aAltitude) const;

   //! Returns the lower bound of the contrailing band
   double GetContrailFloor() const;

   //! Returns the upper bound of the contrailing band
   double GetContrailCeiling() const;

   //! Returns the partial derivative change in speed (m/s) per change in
   //! altitude (m) at constant Mach number.
   double dSpeed_dAltitudeAtMach(double aAltitude) const;

   //! Returns the partial derivative change in density ratio per change
   //! in altitude (m).
   double dDensityRatio_dAltitude(double aAltitude) const;
   //! This function accepts an air density value, and returns the standard atmosphere altitude at which
   //! that density occurs.  (This is a utility to help better replicate existing flight test data.)
   double StandardAltitudeForDensity(double aDensity);

   double CalcAirDensity_slug_ft3(double aAltitude_ft) const;

   double CalcAmbientPressure_psf(double aAltitude_ft) const;

   //! This returns the KTAS based on the specified KCAS/KIAS and altitude
   double CalcKtasFromKcas(double aAltitude_m, double aSpeed_kcas) const;

   //! This returns the KCAS/KIAS based on the specified KTAS and altitude
   double CalcKcasFromKtas(double aAltitude_m, double aSpeed_ktas) const;

   //! This returns the KTAS based on the specified mach and altitude
   double CalcKtasFromMach(double aAltitude_m, double aSpeed_mach) const;

   //! This returns the mach based on the specified KTAS and altitude
   double CalcMachFromKtas(double aAltitude_m, double aSpeed_ktas) const;

   //! This returns the KCAS/KIAS based on the specified mach and altitude
   double CalcKcasFromMach(double aAltitude_m, double aSpeed_mach) const;

   //! This returns the mach based on the specified KCAS/KIAS and altitude
   double CalcMachFromKcas(double aAltitude_m, double aSpeed_kcas) const;

   //! This returns the KCAS/KIAS based on the specified speed in fps and altitude
   double CalcKcasFromFps(double aAltitude_m, double aSpeed_fps) const;

   //! This returns the speed in fps based on the specified KCAS/KIAS and altitude
   double CalcFpsFromKcas(double aAltitude_m, double aSpeed_kcas) const;

   //! This returns the mach based on the specified speed in fps and altitude
   double CalcMachFromFps(double aAltitude_m, double aSpeed_fps) const;

   //! This returns the speed in fps based on the specified mach and altitude
   double CalcFpsFromMach(double aAltitude_m, double aSpeed_mach) const;

   //! This returns the speed in fps based on the specified dynamic pressure and altitude
   double CalcFpsFromDynamicPressure(double aAltitude_m, double aPressure_psf) const;

   //! This returns the dynamic pressure in lbs/sq-ft based on the specified speed in fps and altitude
   double CalcDynamicPressure_psf(double aAltitude_m, double aSpeed_fps) const;

   //! This function is needed for backward compatibility.  Deprecated.  Use UtAirspeed.
   static double CalcPressureExported(double aAltitude);
   //! This function is needed for backward compatibility.  Deprecated.  Use UtAirspeed.
   double KnotsEquivalentAirspeed(double aAltitude, double aTrueAirspeed);
   //! This function is needed for backward compatibility.  Deprecated.  Use UtAirspeed.
   double SeaLevelEquivalentAirspeed(double aAltitude, double aTrueAirSpeed);
   //! This function is needed for backward compatibility.  Deprecated.  Use UtAirspeed.
   double TrueAirspeed(double aAltitude, double aEquivalentAirspeed) const;

   AtmosphereType GetAtmosphereType() const;
   std::string    AtmosphereTypeName() const;

   //! Print a tabulation of all available atmospheres to file.
   //! If aMetricValues true, will tabulate in even 500 meter steps, else, 5000 ft steps.
   //! If aPrintRawTables true, then will also print underlying (metric only) interpolation tables.
   void PrintAll(bool aMetricValues = true, bool aPrintRawTables = false);

   //! Print a tabulation of atmospheric data versus altitude
   void PrintTableTo(std::ostream& aOutStream, bool aMetricValues = true) const;
   void PrintDataTable(std::ostream& aOutStream);

   UtAtmosphereData* GetDataPtr() const { return mDataPtr; }

protected:
   int    GetAtmosphereIndex() const;
   double Interpolate(double aAltitude, int aBase, int aInterp) const;

   static const double MIN_ALT;  // Minimum altitude in the table
   static const double ALT_INCR; // Increment in meters between adjacent rows

   // ===== Member Functions =====
   // void SelectAtmosphere() const;

private:
   UtAtmosphere(UtAtmosphereData* aDataPtr, AtmosphereType aAtmosphereType);
   // TODO_JAJ_REMOVE UtAtmosphere& operator=(const UtAtmosphere&);

   // ===== Member Variables =====
   int mSelectedAtmosphereType; // Selected atmosphere type.  If cDEFAULT, the default setting from UtAtmosphereData is used
   mutable int       mLastAltIndex; // Data table interpolator lookup offset index.
   UtAtmosphereData* mDataPtr;
#ifdef TEST_UTATMOSPHERE
   friend int main();
#endif
};

//! Private implementation of UtAtmosphere with shared data tables
class UT_EXPORT UtAtmosphereData : public UtReferenceCounted
{
public:
   using AtmosphereType = UtAtmosphere::AtmosphereType;
   friend class UtAtmosphere;
   UtAtmosphereData();

   UtAtmosphere GetAtmosphere();

   bool ProcessInput(UtInput& aInput);

   //! This function accepts an altitude and a non-standard air density.  It then calculates
   //! a "simple" dT value that yields the same altitude and desired air density as supplied.
   //! This function does not (if called by itself) does not change the cSIMPLE_DT to the
   //! desired temperature for you.  (This is a utility to help better match flight test data.)
   bool SimpleDeltaTempKforDensity(double aAltitude, double aNonStandardDensity, double& aChosenDeltaTemperature);

   //! This function accepts an altitude and a non-standard air temperature.  It then calculates
   //! a "simple" dT value that yields the same altitude and desired air temperature as supplied.
   //! This function does not (if called by itself) does not change the cSIMPLE_DT to the
   //! desired temperature for you.  (This is a utility to help better match flight test data.)
   bool SimpleDeltaTempKforTemperature(double aAltitude, double aNonStandardTemperature, double& aChosenDeltaTemperature);

   void FillTables();
   void FillTable(int aAtmIndex);
   void SetAtmosphereType(AtmosphereType aAtmosphereType);
   void SetSimpleDeltaTempK(double aSimpleDeltaT_K);
   void CalcInterpolants();
   int  AtmosphereToInt(AtmosphereType aAtmosphereType);

   double ReverseInterpolation(double aValue, int aAtmTypeIndex, int aBase, int aInterp);

   //! This function accepts an air density value, and returns the standard atmosphere altitude at which
   //! that density occurs.  (This is a utility to help better replicate existing flight test data.)
   double StandardAltitudeForDensity(double aDensity);

private:
   double        CalcNonStdTemperature(double aPressureAltitude, int aAtmIndex);
   static double CalcStdTemperature(double aAltitude);
   static double CalcPressure(double aAltitude, double aTemp = -1.0);
   static double CalcDensity(double aAltitude, double aTemp = -1.0, double aPressure = -1.0);
   static double CalcRelHumidity(double aAltitude, double aTemp = -1.0, double aDensity = -1.0);
   static double CalcWaterVapor(double aAltitude);

   // Returns the saturated vapor density for the atmosphere given an altitude
   static double SaturatedVaporDensity(double aTemp);

   enum
   {
      NUM_ALTS_MAX = 512
   }; // Maximum number of rows allowed in the table
   enum
   {
      NUM_ALTS_STD = 62
   }; // Number of rows in the US Standard Atmosphere table
   enum
   {
      NUM_VALUES = 11
   }; // Number of columns in the table
   enum
   {
      NUM_ATMOSPHERES = 7
   }; // Number of atmospheric types

   enum
   {
      NUM_210_ALTS = 202
   }; // Number of non-standard atmosphere altitudes
   enum
   {
      NUM_210_COLS = 5
   }; // Number of non-standard atmosphere columns (alt, hot, tropical, cold, polar)

   static const double sMS_210A[NUM_210_ALTS][NUM_210_COLS];

   double mTable[NUM_ATMOSPHERES][NUM_ALTS_MAX][NUM_VALUES];

   std::pair<double, double> mContrailingAltitudes_m = {7924.8, 10668.0}; // Bounds for altitude band where contrails
                                                                          // can form, defaulted to 26,000-35,000 ft

   AtmosphereType mDefaultAtmosphere = UtAtmosphere::cSTANDARD_DAY;
   double         mSimpleDeltaTempK  = 10.0;
};

#endif
