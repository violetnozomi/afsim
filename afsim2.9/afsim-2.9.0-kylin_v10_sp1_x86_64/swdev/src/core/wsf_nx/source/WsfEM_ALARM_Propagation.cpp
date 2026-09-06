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

// ****************************************************************************
//     This module was designed by Science Applications International
//     Corporation (SAIC) for the US Air Force, for use in the
//     Advanced Low Altitude Radar Model (ALARM), owned by Wright
//     Labs, Analysis and Evaluation Branch (WL/AAWA-1).
//
//     This module is derived from algorithms and code developed for
//     the Air Force by the Massachusetts Institute of Technology's
//     (MIT) Lincoln Laboratory.  Under the terms of the license
//     granted by MIT, the following restriction applies:
//
//     COPYRIGHT (C) 1985,1988 MIT LINCOLN LABORATORY ALL RIGHTS RESERVED
// ****************************************************************************

#include "WsfEM_ALARM_Propagation.hpp"

#include <cmath>
#include <complex>
#include <cstdlib> // for abs(int)

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfEM_ALARM_Antenna.hpp"
#include "WsfEM_ALARM_Fortran.hpp"
#include "WsfEM_ALARM_Geometry.hpp"
#include "WsfEM_ALARM_Terrain.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEnvironment.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationInput.hpp"
#include "WsfTerrain.hpp"

namespace
{
bool mUnitTestPropagation = false; // used only in comparing unit tests of propagation factor vs. alarm
// (do not set true operationally).
} // namespace

namespace
{
// ==============================================================================================
// The following is from ALARM 'physical_constants.f90.
// ==============================================================================================

// symbolic names not permitted in complex parameter statements
std::complex<float> ccoeff(0.86602540F, 0.5F);   // (+sqr3o2,+0.5)
std::complex<float> cforth(1.33333333F, 0.0F);   // (forthr,0.0)
std::complex<float> ctwoth(0.66666667F, 0.0F);   // (twothr,0.0)
std::complex<float> ep1pi3(0.5F, 0.86602540F);   // (+0.5,+sqr3o2)
std::complex<float> em1pi3(0.5F, -0.86602540F);  // (+0.5,-sqr3o2)
std::complex<float> ep2pi3(-0.5F, 0.86602540F);  // (-0.5,+sqr3o2)
std::complex<float> em2pi3(-0.5F, -0.86602540F); // (-0.5,-sqr3o2)

// ==============================================================================================
// The following is from 'SalramPropagation.f90'
// ==============================================================================================

const double sWaveHeight[20] = {0.0, 0.152, 0.457, 0.762, 1.22, 1.82, 3.049, 0.0, 0.0, 0.0,
                                0.0, 0.0,   0.0,   0.0,   0.0,  0.0,  0.0,   0.0, 0.0, 0.0};
} // namespace

// See note in header file...

bool                WsfEM_ALARM_Propagation::uninit   = true;
int                 WsfEM_ALARM_Propagation::aprofile = 0;
std::vector<double> WsfEM_ALARM_Propagation::dratio;
std::vector<double> WsfEM_ALARM_Propagation::elvmsl;
std::vector<int>    WsfEM_ALARM_Propagation::iend;
std::vector<int>    WsfEM_ALARM_Propagation::istart;
std::vector<double> WsfEM_ALARM_Propagation::tanepp;
std::vector<bool>   WsfEM_ALARM_Propagation::visibl;
std::vector<double> WsfEM_ALARM_Propagation::xprofl;
std::vector<double> WsfEM_ALARM_Propagation::zprofl;

int              WsfEM_ALARM_Propagation::indxmx_size = 0;
std::vector<int> WsfEM_ALARM_Propagation::indxmx;

// =================================================================================================
WsfEM_ALARM_Propagation::WsfEM_ALARM_Propagation()
   : WsfEM_Propagation()
   , water_type("sea")
   , soil_moisture(15.0)
   , surface_height(3.0)
   , water_temp(10.0)
   , epsilon_one(6.0)
   , sigma_zero(0.006)
   , roughness(0.0)
   , sea_relaxation(0.0)
   , wind_speed(0.0)
   , diff_sw(true)
   , prop_sw(true)
   , sea_water(true)
   , use_surface_height(false)
   , mUseMIT_LL_DataTables(true)
   , mAllowCalculationShortcuts(true)
   , mWSF_LandCover(0)
   , mWSF_LandForm(0)
   , mWSF_SeaState(0)
   , mSimulationPtr(nullptr)
{
}

// =================================================================================================
WsfEM_ALARM_Propagation::WsfEM_ALARM_Propagation(const WsfEM_ALARM_Propagation& aSrc)
   : WsfEM_Propagation(aSrc)
   , water_type(aSrc.water_type)
   , soil_moisture(aSrc.soil_moisture)
   , surface_height(aSrc.surface_height)
   , water_temp(aSrc.water_temp)
   , epsilon_one(aSrc.epsilon_one)
   , sigma_zero(aSrc.sigma_zero)
   , roughness(aSrc.roughness)
   , sea_relaxation(aSrc.sea_relaxation)
   , wind_speed(aSrc.wind_speed)
   , diff_sw(aSrc.diff_sw)
   , prop_sw(aSrc.prop_sw)
   , sea_water(aSrc.sea_water)
   , use_surface_height(aSrc.use_surface_height)
   , mUseMIT_LL_DataTables(aSrc.mUseMIT_LL_DataTables)
   , mAllowCalculationShortcuts(aSrc.mAllowCalculationShortcuts)
   , mWSF_LandCover(0)
   , mWSF_LandForm(0)
   , mWSF_SeaState(0)
   , mSimulationPtr(aSrc.mSimulationPtr)
{
}

// =================================================================================================
//! Reset all the global/static data to default values.
// static
void WsfEM_ALARM_Propagation::ResetState()
{
   uninit   = true;
   aprofile = 0;
   dratio.clear();
   elvmsl.clear();
   iend.clear();
   istart.clear();
   tanepp.clear();
   visibl.clear();
   xprofl.clear();
   zprofl.clear();

   indxmx_size = 0;
   indxmx.clear();
}

// =================================================================================================
//! Factory method called by WsfEM_PropagationTypes.
// static
WsfEM_Propagation* WsfEM_ALARM_Propagation::ObjectFactory(const std::string& aTypeName)
{
   WsfEM_Propagation* instancePtr = nullptr;
   if ((aTypeName == "WSF_ALARM_PROPAGATION") || (aTypeName == "alarm"))
   {
      instancePtr = new WsfEM_ALARM_Propagation();
   }
   return instancePtr;
}

// =================================================================================================
// virtual
WsfEM_Propagation* WsfEM_ALARM_Propagation::Clone() const
{
   return new WsfEM_ALARM_Propagation(*this);
}

// =================================================================================================
// virtual
double WsfEM_ALARM_Propagation::ComputePropagationFactor(WsfEM_Interaction& aInteraction, WsfEnvironment& aEnvironment)
{
   // Make sure the interaction is a two-way interaction.

   WsfEM_Xmtr*  xmtrPtr   = aInteraction.GetTransmitter();
   WsfEM_Rcvr*  rcvrPtr   = aInteraction.GetReceiver();
   WsfPlatform* targetPtr = aInteraction.GetTarget();
   if ((xmtrPtr == nullptr) || (rcvrPtr == nullptr) || (targetPtr == nullptr))
   {
      return 1.0;
   }

   // This model is computationally intensive and should be avoided if possible.
   // When multi-beam radars use WsfEM_ALARM_Propagation and WsfEM_ALARM_Clutter,
   // the calculations for the 2nd and subsequent beams can be bypassed if the first
   // beam had a target with low S/N and a small propagation factor (-30 db).
   // (see WsfEM_ALARM_Clutter)

   if (mAllowCalculationShortcuts)
   {
      int terrainStatusMask = (WsfEM_Interaction::cRCVR_TERRAIN_MASKING | WsfEM_Interaction::cXMTR_TERRAIN_MASKING);
      if ((aInteraction.mFailedStatus & terrainStatusMask) != 0)
      {
         // Previous beam indicated we are mask and not visible by diffraction.
         // Continue to indicate a small propagation factor...
         return 1.0E-4;
      }
   }

   // Compute the geometry as it is computed in ALARM (geomtr.f90).

   double rdr_lat;
   double rdr_lon;
   double rdr_alt_msl;
   double tgt_lat;
   double tgt_lon;
   double tgt_alt_msl;
   double rkfact;
   double tgt_x;
   double tgt_z;
   double tgt_az;
   double tgt_el;
   double tanept;
   double slant_range;
   double ground_range;

   WsfEM_ALARM_Terrain::Initialize(targetPtr->GetTerrain());

   WsfEM_ALARM_Geometry::ComputeGeometry(xmtrPtr,
                                         targetPtr,
                                         xmtrPtr,
                                         rdr_alt_msl,
                                         rdr_lat,
                                         rdr_lon,
                                         rkfact,
                                         tgt_alt_msl,
                                         tgt_lat,
                                         tgt_lon,
                                         ground_range,
                                         tanept,
                                         slant_range,
                                         tgt_az,
                                         tgt_el,
                                         tgt_x,
                                         tgt_z);

   antenna tx_ant(xmtrPtr, aInteraction, tgt_az, tgt_el, slant_range);
   antenna rx_ant(rcvrPtr, aInteraction, tgt_az, tgt_el, slant_range);

   double pulse_width = xmtrPtr->GetPulseWidth() * 1.0E+6; // in usec
   // BOEING-BEG:
   double frequency = xmtrPtr->GetFrequency();
   if ((rcvrPtr != nullptr) && (rcvrPtr->GetFunction() != WsfEM_Rcvr::cRF_PASSIVE_SENSOR))
   {
      // use receiver frequency in case xmtr is at another center frequency
      frequency = rcvrPtr->GetFrequency();
   }
   double wavelength = UtMath::cLIGHT_SPEED / frequency;
   // double wavelength = UtMath::cLIGHT_SPEED / xmtrPtr->GetFrequency();
   // BOEING-END:
   int polarization = 0;
   switch (xmtrPtr->GetPolarization())
   {
   case WsfEM_Types::cPOL_HORIZONTAL:
      polarization = pol_horizontal;
      break;
   case WsfEM_Types::cPOL_VERTICAL:
      polarization = pol_vertical;
      break;
   case WsfEM_Types::cPOL_SLANT_45:
      polarization = pol_slant_45;
      break;
   case WsfEM_Types::cPOL_SLANT_135:
      polarization = pol_slant_135;
      break;
   case WsfEM_Types::cPOL_LEFT_CIRCULAR:
      polarization = pol_lh_circular;
      break;
   case WsfEM_Types::cPOL_RIGHT_CIRCULAR:
      polarization = pol_rh_circular;
      break;
   default:
      polarization = pol_default;
      break;
   }

   // 'wind_speed' is gotten from the environment.
   wind_speed = aEnvironment.GetWindSpeed();
   wind_speed *= UtMath::cNMPH_PER_MPS; // Convert from m/s to knots as needed by the model

   // 'water_cover' is set if the transmitter is on a surface or subsurface vessel.
   WsfSpatialDomain spatialDomain = xmtrPtr->GetPlatform()->GetSpatialDomain();
   bool water_cover = ((spatialDomain == WSF_SPATIAL_DOMAIN_SURFACE) || (spatialDomain == WSF_SPATIAL_DOMAIN_SUBSURFACE));
   bool masked      = false;

   double deltag = WsfEM_ALARM_Terrain::get_ground_range_incr();

   // BOEING-BEG: Added ability to use MIT-LL/SALRAM data tables (it is more complete)
   if (mUseMIT_LL_DataTables)
   {
      use_surface_height = false;
      mWSF_LandCover     = aEnvironment.GetLandCover();
      mWSF_LandForm      = aEnvironment.GetLandFormation();
      mWSF_SeaState      = aEnvironment.GetSeaState();
      GetLandCoverData(mWSF_LandCover, epsilon_one, sigma_zero);
   }
   // BOEING-END: Added ability to use MIT-LL/SALRAM data tables (it is more complete)

   // use physical_consts, only : pi, onethr

   // implicit none

   //-------------------------------------------------------------------
   // Passed parameters
   //-------------------------------------------------------------------

   // type (propagation),  intent(in)  :: this

   // real(8),             intent(in)  :: deltag           // meters
   // real(8),             intent(in)  :: ground_range     // meters
   // logical,             intent(in)  :: water_cover
   // integer,             intent(in)  :: polarization
   // real(8),             intent(in)  :: pulse_width      // usec
   // real(8),             intent(in)  :: rdr_lat          // radians
   // real(8),             intent(in)  :: rdr_lon          // radians
   // real(8),             intent(in)  :: rkfact
   // type (antenna),      intent(in)  :: rx_ant
   // real(8),             intent(in)  :: slant_range      // meters
   // real(8),             intent(in)  :: tanept
   // real(8),             intent(in)  :: tgt_alt_msl      // meters
   // real(8),             intent(in)  :: tgt_az           // radians
   // real(8),             intent(in)  :: tgt_el           // radians
   // real(8),             intent(in)  :: tgt_lat          // radians
   // real(8),             intent(in)  :: tgt_lon          // radians
   // real(8),             intent(in)  :: tgt_x            // meters
   // real(8),             intent(in)  :: tgt_z            // meters
   // type (antenna),      intent(in)  :: tx_ant
   // real(8),             intent(in)  :: wavelength       // meters

   // complex,             intent(out) :: fto4th
   // logical,             intent(out) :: masked

   //-------------------------------------------------------------------
   // Local variables
   //-------------------------------------------------------------------
   double const3, const4; // NOT-USED , tgt_off_az, tgt_off_el, hammsl_rx, hammsl_tx;

   COMPLEX prop_rx(1.0, 0.0);
   COMPLEX prop_tx(1.0, 0.0);

   //-----------------------------------------------------------------
   // Compute the pattern propagation factor to the fourth power.
   //-----------------------------------------------------------------

   const3 = pow((wavelength / pi), onethr);
   const4 = 0.5 * const3 * const3;

   // Compute the propagation factor to the TX antenna
   /*call*/ laprop(tx_ant,
                   tgt_az,
                   const3,
                   const4,
                   deltag,
                   tgt_el,
                   this->epsilon_one,
                   prop_tx,
                   ground_range,
                   tgt_alt_msl,
                   polarization,
                   water_cover,
                   this->prop_sw,
                   masked,
                   pulse_width,
                   slant_range,
                   rkfact,
                   wavelength,
                   this->roughness,
                   this->sigma_zero,
                   rdr_lon,
                   rdr_lat,
                   tanept,
                   tgt_lon,
                   tgt_lat,
                   this->sea_relaxation,
                   this->wind_speed,
                   tgt_x,
                   tgt_z,
                   this->diff_sw,
                   this->use_surface_height,
                   this->surface_height,
                   this->soil_moisture,
                   this->water_temp,
                   this->sea_water);

   // if (is_same(rx_ant, tx_ant))
   if (!aInteraction.mBistatic)
   {
      // Propagation factor is the same when the antennas are the same
      prop_rx = prop_tx;
   }
   else
   {
      // Compute the propagation factor to the RX antenna
      /*call*/ laprop(rx_ant,
                      tgt_az,
                      const3,
                      const4,
                      deltag,
                      tgt_el,
                      this->epsilon_one,
                      prop_rx,
                      ground_range,
                      tgt_alt_msl,
                      polarization,
                      water_cover,
                      this->prop_sw,
                      masked,
                      pulse_width,
                      slant_range,
                      rkfact,
                      wavelength,
                      this->roughness,
                      this->sigma_zero,
                      rdr_lon,
                      rdr_lat,
                      tanept,
                      tgt_lon,
                      tgt_lat,
                      this->sea_relaxation,
                      this->wind_speed,
                      tgt_x,
                      tgt_z,
                      this->diff_sw,
                      this->use_surface_height,
                      this->surface_height,
                      this->soil_moisture,
                      this->water_temp,
                      this->sea_water);
   }

   COMPLEX fto4th = prop_rx * prop_tx;
   if (DebugEnabled())
   {
      double cabsF4th = cabs(fto4th);
      auto   logger   = ut::log::debug();
      logger.GetStream().precision(16);
      logger << "Pattern propagation factor to the fourth power:";
      logger.AddNote() << "Value: " << fto4th;
      logger.AddNote() << "Magnitude: " << cabsF4th;
      logger.AddNote() << "Magnitude DB: " << UtMath::SafeLinearToDB(cabsF4th) << " db";
   }
   return static_cast<double>(cabs(fto4th));
}

// =================================================================================================
// virtual
bool WsfEM_ALARM_Propagation::Initialize(WsfEM_XmtrRcvr* aXmtrRcvrPtr)
{
   mSimulationPtr = aXmtrRcvrPtr->GetSimulation();
   if (!mSimulationPtr->GetScenario().GetSimulationInput().AllowEM_PropagationCalculationShortcuts())
   {
      // honor the global flag if it is cleared (default is to allow the shortcuts).
      mAllowCalculationShortcuts = false;
   }
   bool ok = WsfEM_Propagation::Initialize(aXmtrRcvrPtr);
   if (mUnitTestPropagation)
   {
      auto logger =
         ut::log::warning() << "ALARM propagation is being used with 'unit_test_propagation' option enabled.";
      logger.AddNote() << "Do not leave this option enabled for normal operation.";
   }
   return ok;
}

// =================================================================================================
// virtual
bool WsfEM_ALARM_Propagation::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if ((command == "propagation") || (command == "propagation_sw"))
   {
      aInput.ReadValue(prop_sw);
   }
   else if ((command == "diffraction") || (command == "diffraction_sw"))
   {
      aInput.ReadValue(diff_sw);
   }
   else if ((command == "epsilon_one") || (command == "terrain_dielectric_constant")) // For compatibility with SUPPRESSOR
   {
      aInput.ReadValue(epsilon_one);
      aInput.ValueGreaterOrEqual(epsilon_one, 0.0);
      mUseMIT_LL_DataTables = false;
   }
   else if ((command == "sigma_zero") || (command == "terrain_conductivity")) // For compatibility with SUPPRESSOR
   {
      aInput.ReadValue(sigma_zero);
      aInput.ValueGreaterOrEqual(sigma_zero, 0.0);
      mUseMIT_LL_DataTables = false;
   }
   else if ((command == "roughness_factor") ||
            (command == "terrain_scattering_coefficient")) // For compatibility with SUPPRESSOR
   {
      aInput.ReadValue(roughness);
      aInput.ValueGreaterOrEqual(roughness, 0.0);
      use_surface_height    = false;
      mUseMIT_LL_DataTables = false;
   }
   else if (command == "sea_relaxation")
   {
      aInput.ReadValue(sea_relaxation);
      aInput.ValueGreaterOrEqual(sea_relaxation, 0.0);
      mUseMIT_LL_DataTables = false;
   }
   // else if (command == "sea_wind_speed")                 // Gotten from the environment
   //{
   //    aInput.ReadValueOfType(wind_speed, UtInput::cSPEED);
   //    aInput.ValueGreaterOrEqual(wind_speed, 0.0);
   //    wind_speed *= UtMath::cNMPH_PER_MPS;                 // Convert from m/s to knots as needed by the model
   // }
   else if ((command == "water_temp") || (command == "water_temperature"))
   {
      aInput.ReadValueOfType(water_temp, UtInput::cTEMPERATURE);
      aInput.ValueGreater(water_temp, 0.0);
      water_temp -= UtMath::cZERO_C_IN_K; // Convert from deg K to deg C as needed by the model
   }
   else if (command == "soil_moisture")
   {
      aInput.ReadValue(soil_moisture);
      aInput.ValueInClosedRange(soil_moisture, 0.0, 100.0);
      mUseMIT_LL_DataTables = false;
   }
   else if (command == "soil_moisture_fraction") // For compatibility with WsfEM_FastMultipath
   {
      aInput.ReadValue(soil_moisture);
      aInput.ValueInClosedRange(soil_moisture, 0.0, 1.0);
      soil_moisture *= 100.0;
      mUseMIT_LL_DataTables = false;
   }
   else if ((command == "stddev_surface_height") ||
            (command == "surface_roughness")) // For compatibility with WsfEM_FastMultipath
   {
      aInput.ReadValueOfType(surface_height, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(surface_height, 0.0);
      use_surface_height    = true;
      mUseMIT_LL_DataTables = false;
   }
   else if (command == "water_type")
   {
      aInput.ReadValue(water_type);
      if ((water_type != "lake") && (water_type != "sea"))
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "use_native_terrain_masking")
   {
      bool useAFSIM_TerrainMasking = false;
      aInput.ReadValue(useAFSIM_TerrainMasking);
      WsfEM_ALARM_Terrain::SetUseAFSIM_TerrainMasking(useAFSIM_TerrainMasking);
   }
   else if (command == "unit_test_propagation") // for test only; do not document.
   {
      aInput.ReadValue(mUnitTestPropagation);
   }
   else if (command == "use_calculation_shortcuts")
   {
      aInput.ReadValue(mAllowCalculationShortcuts);
   }
   else
   {
      myCommand = WsfEM_Propagation::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// Code from ALARM 'propagation.f90'
// =================================================================================================

//---------------------------------------------------------------------
//     Purpose:
//     -------
//     Compute the pattern propagation factor (F^2)considering multipath
//     and diffraction for a single antenna.  For a traditional radar,
//     this routine needs to be called for both TX and RX.
//
//
//     Inputs:
//     ------
//     ALPHAT  -  The azimuth angle to the target, in radians.
//
//     CONST3  -
//
//     CONST4  -
//
//
//     DELTAG  -  The ground range increment, in meters, between
//                points in the terrain profile between the radar
//                and the target.
//
//     EPSLNT  -  The elevation angle to the target, in radians.
//
//     EPSLN1  -  The dielectric constant of the terrain, land or sea.
//                The mean value is about 10 for land, and about 80
//                for sea.  In tests conducted by MIT/LL over land,
//                a value of 6.0 was used.  (EPSLN1 >= 0.0)
//
//                Ground Type  |  Dielectric Constant
//                -------------|---------------------
//                Very Dry     |          2
//                Very Wet     |          24
//
//                Sea Temperature (degrees C)  |  Dielectric Constant
//                -----------------------------|---------------------
//                             10              |        72.2
//                             20              |        69.1
//
//     GRANGT  -  Ground range to target in meters.
//
//     HTMMSL  - Height of the target above mean sea level in meters.
//
//     IPOLAR  -  The radar antenna polarization indicator.  If
//                IPOLAR = 0, the antenna is vertically polarized.
//                If IPOLAR = 1,  the antenna is horizontally
//                polarized.
//
//     WATER_COVER - flag indicating is land cover is water
//
//     PULWID  -  Radar pulse width, in microseconds.
//
//     RANGET  -  The range from the radar antenna to the target,
//                in meters.
//
//     RLAMDA  -  The radar wavelength, in meters.
//
//     RROUGH  -  The terrain roughness factor or the constant
//                portion of the roughness coefficient when the land
//                cover is water.
//
//     SIGMHO  -  The terrain conductivity in mhos/m, land or sea.
//                SIGMHO varies from approximately 10**-4 to several
//                units of 10**-3.  A mean value is about 10**-3 for
//                land, and about 4 for sea.  In tests conducted by
//                MIT/LL over land, a value of 0.006 was used.
//                (SIGMHO >= 0.0)
//
//                Sea Temperature (degrees C)  |  Conductivity (mhos/m)
//                -----------------------------|-----------------------
//                              10             |          4.0
//                              20             |          5.2
//
//     SITLAM  -  Site longitude in radians.
//
//     SITPHI  -  Site latitude in radians.
//
//     TANEPT  -  The tangent of the elevation angle of the target
//                relative to the radar.
//
//     TARLAM  -  Target longitude in radians.
//
//     TARPHI  -  Target latitude in radians.
//
//     TAURLX  -  Relaxation Constant (times 1 / 10**-12), for use
//                when LCOVER = 7 (water) only.  Used to determine
//                RROUGH for the pattern propagation factor over the
//                sea (TAURLX >= 0.0)
//
//                Sea Temperature (degrees C)  |  Relaxation Constant
//                -----------------------------|---------------------
//                             10              |      12.1 * 10**-12
//                             20              |       9.2 * 10**-12
//
//     ANT_DATA  -  Antenna data structure.
//
//     WNDKNO  -  Wind speed in knots, for use when LCOVER = 7 (water)
//                only.  Used to determine RROUGH for the pattern
//                propagation factor over the sea.  (WNDKNO >= 0.0)
//
//     XTPROF  -  The x-coordinate of the target in an x-z coordinate
//                system whose origin is located on the mean sea level
//                surface of the Earth at the site.  The positive
//                x-axis extends from the origin toward the target and
//                is tangent to the Earth at the radar.  Units are in
//                meters.
//
//     ZPROFL  -  ZPROFL(I), I = 0,1,...,NPROFL, is the z-coordinate,
//                (vertical), in meters, of the I-th point in the
//                terrain profile between the radar and the target
//                in meters.  ZPROFL(0) is the height of the terrain
//                at the radar.
//
//     ZTPROF  -  The z-coordinate of the target in an x-z coordinate
//                system whose origin is located on the mean sea level
//                surface of the Earth at the site.  (Positive z is up)
//                Units are in meters.
//
//     DIFF_SW - Diffraction computation switch.
//
//     USE_SURFACE_HEIGHT - Flag indication if standard deviation of
//                          surface height should be used.
//
//     SURFACE_HEIGHT - The standard deviation of surface height in
//                      meters.
//
//     SOIL_MOISTURE - The soil moisture content percentage.
//
//     WATER_TEMP  - Water temperature in degrees C.
//
//     SEA_WATER  - Flag indicating if the water cover is sea water.
//
//
//     Outputs:
//     -------
//     FSQUARED  -  The complex pattern propagation factor squared (F^2).
//
//
//     Author:
//     ------
//     Science Applications International Corporation (SAIC)
//     2301 Yale Boulevard, SE, Suite E
//     Albuquerque, New Mexico 87106
//     (505) 766-7400
//
//
//     Notice:
//     ------
//     This module was designed by Science Applications International
//     Corporation (SAIC) for the US Air Force, for use in the
//     Advanced Low Altitude Radar Model (ALARM), owned by Wright
//     Labs, Analysis and Evaluation Branch (WL/AAWA-1).
//
//     This module is derived from algorithms and code developed for
//     the Air Force by the Massachusetts Institute of Technology's
//     (MIT) Lincoln Laboratory.  Under the terms of the license
//     granted by MIT, the following restriction applies:
//
//     COPYRIGHT (C) 1985,1988 MIT LINCOLN LABORATORY ALL RIGHTS RESERVED
//
//
//     Modified:
//     --------
//     Division 397
//     SAIC
//     1321 Research Park Drive
//     Dayton, OH 45432
//     (513) 429-6500
//
//     ALARM 3.0:
//     ---------
//     - DOPLER variable omitted from GEOMRT common block.
//       Variable DOPCSR, which includes the conical scan
//       rate, is now used in its place.  JCL
//
//     - Subroutine OFFBOR called to determine the off-
//       boresight angles in azimuth and elevation of an
//       arbitrary point in space with respect to the
//       radar antenna.  JCL
//
//
//     ALARM 3.2:
//     ---------
//     - RGAINS common block removed from subroutine RGAIN to support
//       stand alone receive antenna.  Those variables (AZRMAX, AZRMIN,
//       AZRTAB, ELRMAX, ELRMIN, ELRTAB, IRXTYP, NUMAZR, NUMELR, RGTABL,
//       and RXGMIN) previously accessed through the common block are
//       now passed through the RGAIN argument list.  (SPCR-1056, JCL)
//
//     - Included RGAINS common block so that the AZRMAX, AZRMIN, ELRMAX,
//       ELRMIN, IRXTYP, NUMAZR, NUMELR, RGTABL and RXGMIN variables
//       could be passed to the RGAIN subroutine.  (SPCR-1056, JCL)
//
//     - TGAINS common block removed from subroutine TGAIN to support
//       stand alone transmit antenna.  Those variables (AZTMAX, AZTMIN,
//       AZTTAB, ELTMAX, ELTMIN, ELTTAB, ITXTYP, NUMAZT, NUMELT,TGTABL,
//       and TXGMIN) previously accessed through the common block are
//       now passed through the TGAIN argument list.  (SPCR-1056, JCL)
//
//     - Included TGAINS common block so that the AZTMAX, AZTMIN, ELTMAX,
//       ELTMIN, ITXTYP, NUMAZT, NUMELT, TGTABL and TXGMIN variables
//       could be passed to the TGAIN subroutine.  (SPCR-1056, JCL)
//
//     - Modified calls to MLTPTH subroutine to pass the following
//       additional variables (ALPHAR, ALPHAT, AZTMAX, AZTMIN, AZTTAB,
//       ELTMAX, ELTMIN, ELTTAB, EPSLNR, EPSLNT, EPSLN1, HAMMSL, IEND,
//       IPOLAR, ISTART, ITXTYP, LCOVER, NAREAS, NPROFL, NUMAZT, NUMELT,
//       OFFAZT, OFFELT, PULWID, RANGET, RLAMDA, RROUGH, SIGMHO, TANEPP,
//       TAURLX, TGTABL, TXGMIN, VISIBL, WNDKNO, XPROFL, XTPROF, ZPROFL,
//       ZTPROF) in order to support stand alone multipath.
//       (SPCR-1058, JCL)
//
//     - Included the CONSTR, ENVIRO, RFLCOM, and SITCOM common
//       blocks.  Variables in these common blocks are passed as
//       arguments for the MLTPTH subroutine.  (SPCR-1058, JCL)
//
//
//     ALARM 4.x:
//     ---------
//     - Modified for use with complex pattern propagation factor.  JCL
//
//     15-Oct-1999  Corrected check for for proper allocation
//                  of dynamic arrays.
//
//     15-May-2000 Correct the computation of propagation factor for all
//                 combinations of PROPAGATION_SW, DIFFRACTION_SW, and
//                 TERRAIN_SW. See SPCR #1184.
//
//     22-Dec-2000  Added support for the standard deviation of surface
//                  height algorithm. See SPCR 1204. MWS
//     14-Oct-2002  Removed tab character that was not F90 compliant.
//                  See SPCR #1260.
//     19-Dec-2003   - Modified to use the new antenna class.
//                     Removed unnecessary variables, ALPHAR, EPSLNR,
//                     HAMMSL, OFFAZT, OFFELT, RX_PAT from argument
//                     list.  This routine now only computes F^2 (FSQUARED).
//                     See SPCR #1310.
//     13-Jan-2006    - Consolidated allocate statements into a single
//                      statement. See SPCR #1357.
//---------------------------------------------------------------------

void WsfEM_ALARM_Propagation::laprop(antenna& ant_data,
                                     double   alphat,
                                     double   const3,
                                     double   const4,
                                     double   deltag,
                                     double   epslnt,
                                     double   epsln1,
                                     COMPLEX& fsquared,
                                     double   grangt,
                                     double   htmmsl,
                                     int      ipolar,
                                     bool     water_cover,
                                     bool     lpprop,
                                     bool&    masked,
                                     double   pulwid,
                                     double   ranget,
                                     double   rkfact,
                                     double   rlamda,
                                     double   rrough,
                                     double   sigmho,
                                     double   sitlam,
                                     double   sitphi,
                                     double   tanept,
                                     double   tarlam,
                                     double   tarphi,
                                     double   taurlx,
                                     double   wndkno,
                                     double   xtprof,
                                     double   ztprof,
                                     bool     diff_sw,
                                     bool     use_surface_height,
                                     double   surface_height,
                                     double   soil_moisture,
                                     double   water_temp,
                                     bool     sea_water)
{
   // use propagation_consts
   // use terrain_class
   // use error_check_mod, only : fatal_error

   // implicit none


   //-------------------------------------------------------------------
   // Passed parameters
   //-------------------------------------------------------------------

   // type (antenna), intent(in)  :: ant_data
   // real(8),        intent(in)  :: alphat             ! radians
   // real(8),        intent(in)  :: const3
   // real(8),        intent(in)  :: const4
   // real(8),        intent(in)  :: deltag             ! meters
   // real(8),        intent(in)  :: epsln1
   // real(8),        intent(in)  :: epslnt             ! radians
   // real(8),        intent(in)  :: grangt             ! meters
   // real(8),        intent(in)  :: htmmsl             ! meters
   // real(8),        intent(in)  :: pulwid             ! usec
   // real(8),        intent(in)  :: ranget             ! meters
   // real(8),        intent(in)  :: rkfact
   // real(8),        intent(in)  :: rlamda             ! meters
   // real(8),        intent(in)  :: rrough
   // real(8),        intent(in)  :: sigmho
   // real(8),        intent(in)  :: sitlam             ! radians
   // real(8),        intent(in)  :: sitphi             ! radians
   // real(8),        intent(in)  :: tanept
   // real(8),        intent(in)  :: tarlam             ! radians
   // real(8),        intent(in)  :: tarphi             ! radians
   // real(8),        intent(in)  :: taurlx
   // real(8),        intent(in)  :: wndkno
   // real(8),        intent(in)  :: xtprof
   // real(8),        intent(in)  :: ztprof

   // complex,        intent(out) :: fsquared

   // integer,        intent(in)  :: ipolar

   // logical,        intent(in)  :: lpprop
   // logical,        intent(in)  :: diff_sw
   // logical,        intent(in)  :: water_cover
   // logical,        intent(out) :: masked

   // logical,        intent(in)  :: use_surface_height
   // real(8),        intent(in)  :: surface_height     ! stddev meters
   // real(8),        intent(in)  :: soil_moisture      ! %
   // real(8),        intent(in)  :: water_temp         ! deg C
   // logical,        intent(in)  :: sea_water

   //-------------------------------------------------------------------
   // Local variables
   //-------------------------------------------------------------------

   enum
   {
      hunk = 512
   };
   enum
   {
      mxacol = 1
   };
   enum
   {
      mxazel = 1800
   };
   const double pt1dif = 0.25;
   const double pt2dif = 0.50;
   const double pt1mdf = 0.50;
   const double pt2mdf = 0.75;

   COMPLEX fpprop, fsubm;

   double alfaks, alfamd, frcmin, fsubd, fsubk, fsubs, /*NOT-USED grbelo,*/
      /*NOT-USED gtbelo,*/ hmdzro, tanmax;

   int indxfc, nareas, nprofl /*NOT-USED, i */;

   bool convrg;

   // NOTE-C++ The following have been moved to class static variables
   // logical,              save :: uninit   = true
   // integer,              save :: aprofile = 0
   // real(8), allocatable, save :: dratio(:)
   // real(8), allocatable, save :: elvmsl(:)
   // integer, allocatable, save :: iend(:)
   // integer, allocatable, save :: istart(:)
   // real(8), allocatable, save :: tanepp(:)
   // logical, allocatable, save :: visibl(:)
   // real(8), allocatable, save :: xprofl(:)
   // real(8), allocatable, save :: zprofl(:)

   double hammsl;

   //-------------------------------------------------------------------
   // Allocate profile arrays as necessary.
   //-------------------------------------------------------------------
   nprofl = WsfEM_ALARM_Terrain::get_nprofile(grangt);

   if ((nprofl > aprofile) || uninit)
   {
      uninit = false;

      //----------------------------------------------------------------
      // Allocate arrays.  Memory is allocated in hunks to minimize
      // allocate/deallocate thrashing if nprofl is gradually creeping
      // up.
      //----------------------------------------------------------------
      aprofile = aprofile + (abs(nprofl / hunk) + 1) * hunk;

      // NOTE: Arrays that are start at index 1 will still have element 0 allocated
      //       (but ignored) so as to allow the code to continue 1-based indexing.
      dratio.resize(aprofile + 1); // 0:aprofile
      elvmsl.resize(aprofile + 1); // 1:aprofile
      iend.resize(aprofile + 1);   // 1:aprofile
      istart.resize(aprofile + 1); // 1:aprofile
      tanepp.resize(aprofile + 1); // 1:aprofile
      visibl.resize(aprofile + 1); // 1:aprofile
      xprofl.resize(aprofile + 1); // 0:aprofile
      zprofl.resize(aprofile + 1); // 0:aprofile

      // allocate(dratio(0:aprofile), elvmsl(aprofile), &
      //      &   iend(aprofile),     istart(aprofile), &
      //      &   tanepp(aprofile),   visibl(aprofile),&
      //      &   xprofl(0:aprofile), zprofl(0:aprofile), stat = i)

      // if (i != 0) then
      //    call fatal_error('LAPROP', 'Can't allocate memory')
      // end if
   }

   hammsl = get_height_msl(ant_data);
   wsf::Terrain terrain(mSimulationPtr->GetTerrainInterface());
   /*call*/ WsfEM_ALARM_Terrain::profil(terrain,
                                        mSimulationPtr->GetScenario().GetEnvironment(),
                                        alphat,
                                        epslnt,
                                        hammsl,
                                        htmmsl,
                                        nprofl,
                                        rkfact,
                                        sitlam,
                                        sitphi,
                                        tarlam,
                                        tarphi,
                                        elvmsl,
                                        masked,
                                        tanepp,
                                        tanmax,
                                        visibl,
                                        xprofl,
                                        zprofl);

   if (lpprop)
   {
      //----------------------------------------------------------------
      // The propagation loss is to be determined.  There must be at
      // least one point in the terrain profile between the radar site
      // and the target for the propagation subroutines to be called.
      //----------------------------------------------------------------

      if (nprofl > 0)
      {
         //-------------------------------------------------------------
         // The first step in determining the propagation loss over the
         // path from the radar site to the target is to decide whether
         // to determine the propagation loss from diffraction loss,
         // multipath loss, or a combination of the two.  This decision
         // is based on the terrain clearance of the direct ray between
         // the radar and the target.  Call subroutine FIRST to
         // determine FRCMIN, the minimum value of the ratio of the
         // clearance of the direct ray to the Fresnel clearance at each
         // point in the terrain profile between the radar site and the
         // target.  See pages 4 and 5, reference 1, for more
         // information.
         //-------------------------------------------------------------

         /*call*/ first(dratio, frcmin, hammsl, indxfc, nprofl, rlamda, tanept, xprofl, xtprof, zprofl);

         if (frcmin < pt2mdf && diff_sw)
         {
            //----------------------------------------------------------
            // A portion of the propagation loss is to be determined
            // from diffraction loss.  A second decision must be made
            // whether to determine the diffraction loss from spherical
            // earth diffraction loss, knife-edge diffraction loss, or a
            // combination of the two.  This decision is based on
            // HMDZRO, the ratio of the height of the highest mask or
            // minimum clearance point measured from the best- fit line
            // to the terrain profile to the Fresnel clearance at that
            // point.  Call subroutine SECOND to determine HMDZRO.
            //----------------------------------------------------------

            /*call*/ second(deltag, elvmsl, grangt, hmdzro, indxfc, nprofl, rlamda);

            if (frcmin < pt1mdf)
            {
               //-------------------------------------------------------
               // The propagation loss is to be determined from
               // diffraction loss only.
               //-------------------------------------------------------

               if (hmdzro < pt2dif)
               {
                  //----------------------------------------------------
                  // A portion of the diffraction loss is to be
                  // determined from spherical earth diffraction loss.
                  // Call subroutine SEDIFF to determine FSUBS, the
                  // spherical earth diffraction loss.
                  //----------------------------------------------------

                  /*call*/ sediff(ant_data,
                                  alphat,
                                  const3,
                                  const4,
                                  convrg,
                                  deltag,
                                  dratio,
                                  elvmsl,
                                  epslnt,
                                  fsubs,
                                  grangt,
                                  hammsl,
                                  htmmsl,
                                  indxfc,
                                  nprofl,
                                  rkfact,
                                  rlamda,
                                  xprofl,
                                  zprofl);

                  if (convrg)
                  {
                     if (hmdzro < pt1dif)
                     {
                        //----------------------------------------------
                        // The propagation loss is to be determined from
                        // diffraction loss only.  The diffraction loss
                        // is to be determined from spherical earth
                        // diffraction loss only.  The spherical earth
                        // diffraction subroutine SEDIFF has converged
                        // to a solution.  Set FPPROP, the propagation
                        // loss, to FSUBS, the spherical earth
                        // diffraction loss.
                        //----------------------------------------------

                        fpprop = cmplx(real(fsubs));
                     }
                     else
                     {
                        //----------------------------------------------
                        // The propagation loss is to be determined from
                        // diffraction loss only.  The diffraction loss
                        // is to be determined using a combination of
                        // spherical earth diffraction loss and
                        // knife-edge diffraction loss.  Call subroutine
                        // KEDIFF to determine FSUBK, the knife-edge
                        // diffraction loss.
                        //----------------------------------------------

                        /*call*/ kediff(ant_data,
                                        alphat,
                                        dratio,
                                        epslnt,
                                        fsubk,
                                        hammsl,
                                        indxfc,
                                        nprofl,
                                        rlamda,
                                        xprofl,
                                        xtprof,
                                        zprofl,
                                        ztprof);

                        //----------------------------------------------
                        // Combine FSUBS, the spherical earth
                        // diffraction loss, with FSUBK, the knife-edge
                        // diffraction loss, to determine FPPROP, the
                        // propagation loss.
                        //----------------------------------------------

                        alfaks = (hmdzro - pt1dif) / (pt2dif - pt1dif);
                        fpprop = cmplx(real(alfaks) * real(fsubk) + real(1.0 - alfaks) * real(fsubs));
                     }
                  }
                  else
                  {
                     if (hmdzro < pt1dif)
                     {
                        if (frcmin > 0.0)
                        {
                           //-------------------------------------------
                           // The propagation loss is to be determined
                           // from diffraction loss only.  The
                           // diffraction loss is to be determined from
                           // spherical earth diffraction loss only.
                           // The spherical earth diffraction subroutine
                           // SEDIFF did not converge to a solution.
                           // The target is not masked from the radar.
                           // Set FPPROP, the propagation loss, to
                           // FSUBM, the multipath loss.
                           //-------------------------------------------

                           /*call*/ mltpth(ant_data,
                                           alphat,
                                           epslnt,
                                           epsln1,
                                           fsubm,
                                           hammsl,
                                           iend,
                                           ipolar,
                                           istart,
                                           water_cover,
                                           nareas,
                                           nprofl,
                                           pulwid,
                                           ranget,
                                           rlamda,
                                           rrough,
                                           sigmho,
                                           tanepp,
                                           taurlx,
                                           visibl,
                                           wndkno,
                                           xprofl,
                                           xtprof,
                                           zprofl,
                                           ztprof,
                                           elvmsl,
                                           use_surface_height,
                                           surface_height,
                                           soil_moisture,
                                           water_temp,
                                           sea_water);

                           fpprop = fsubm;
                        }
                        else
                        {
                           //-------------------------------------------
                           // The propagation loss is to be determined
                           // from diffraction loss only.  The
                           // diffraction loss is to be determined from
                           // spherical earth diffraction loss only.
                           // The spherical earth diffraction subroutine
                           // SEDIFF did not converge to a solution.
                           // The target is masked from the radar.  Set
                           // FPPROP, the propagation loss, to FSUBK,
                           // the knife-edge diffraction loss.
                           //-------------------------------------------

                           /*call*/ kediff(ant_data,
                                           alphat,
                                           dratio,
                                           epslnt,
                                           fsubk,
                                           hammsl,
                                           indxfc,
                                           nprofl,
                                           rlamda,
                                           xprofl,
                                           xtprof,
                                           zprofl,
                                           ztprof);

                           fpprop = cmplx(real(fsubk));
                        }
                     }
                     else
                     {
                        //----------------------------------------------
                        // The propagation loss is to be determined from
                        // diffraction loss only.  The diffraction loss
                        // is to be determined using a combination of
                        // spherical earth diffraction loss and
                        // knife-edge diffraction loss.  The spherical
                        // earth diffraction subroutine SEDIFF did not
                        // converge to a solution.  Set FPPROP, the
                        // propagation loss, to FSUBK, the knife-edge
                        // diffraction loss.
                        //----------------------------------------------

                        /*call*/ kediff(ant_data,
                                        alphat,
                                        dratio,
                                        epslnt,
                                        fsubk,
                                        hammsl,
                                        indxfc,
                                        nprofl,
                                        rlamda,
                                        xprofl,
                                        xtprof,
                                        zprofl,
                                        ztprof);

                        fpprop = cmplx(real(fsubk));
                     }
                  }
               }
               else
               {
                  //----------------------------------------------------
                  // The propagation loss is to be determined from
                  // diffraction loss only.  The diffraction loss is to
                  // be determined from knife-edge diffraction loss
                  // only. Set FPPROP, the propagation loss, to FSUBK,
                  // the knife-edge diffraction loss.
                  //----------------------------------------------------

                  /*call*/ kediff(ant_data, alphat, dratio, epslnt, fsubk, hammsl, indxfc, nprofl, rlamda, xprofl, xtprof, zprofl, ztprof);

                  fpprop = cmplx(real(fsubk));

               } // (hmdzro < pt2dif)
            }
            else
            {
               //-------------------------------------------------------
               // The propagation loss is to be determined from
               // multipath loss and diffraction loss.  Call subroutine
               // MLTPTH to determine FSUBM, the multipath loss.
               //-------------------------------------------------------
               /*call*/ mltpth(ant_data,
                               alphat,
                               epslnt,
                               epsln1,
                               fsubm,
                               hammsl,
                               iend,
                               ipolar,
                               istart,
                               water_cover,
                               nareas,
                               nprofl,
                               pulwid,
                               ranget,
                               rlamda,
                               rrough,
                               sigmho,
                               tanepp,
                               taurlx,
                               visibl,
                               wndkno,
                               xprofl,
                               xtprof,
                               zprofl,
                               ztprof,
                               elvmsl,
                               use_surface_height,
                               surface_height,
                               soil_moisture,
                               water_temp,
                               sea_water);

               if (hmdzro < pt2dif)
               {
                  //----------------------------------------------------
                  // A portion of the diffraction loss is to be
                  // determined from spherical earth diffraction loss.
                  // Call subroutine SEDIFF to determine FSUBS, the
                  // spherical earth diffraction loss.
                  //----------------------------------------------------

                  /*call*/ sediff(ant_data,
                                  alphat,
                                  const3,
                                  const4,
                                  convrg,
                                  deltag,
                                  dratio,
                                  elvmsl,
                                  epslnt,
                                  fsubs,
                                  grangt,
                                  hammsl,
                                  htmmsl,
                                  indxfc,
                                  nprofl,
                                  rkfact,
                                  rlamda,
                                  xprofl,
                                  zprofl);

                  if (convrg)
                  {
                     if (hmdzro < pt1dif)
                     {
                        //----------------------------------------------
                        // The propagation loss is to be determined from
                        // multipath loss and diffraction loss.  The
                        // diffraction loss is to be determined from
                        // spherical earth diffraction loss only.  The
                        // spherical earth diffraction subroutine SEDIFF
                        // has converged to a solution.  Set FSUBD, the
                        // diffraction loss, to FSUBS, the spherical
                        // earth diffraction loss.  Combine FSUBD, the
                        // diffraction loss, with FSUBM, the multipath
                        // loss, to determine FPPROP, the propagation
                        // loss.
                        //----------------------------------------------

                        fsubd  = fsubs;
                        alfamd = (frcmin - pt1mdf) / (pt2mdf - pt1mdf);
                        fpprop = (cabs(real(alfamd) * fsubm) + real((1.0 - alfamd) * fsubd)) *
                                 cexp(cmplx(0.0, 1.0) * std::atan2(aimag(fsubm), real(fsubm)));
                     }
                     else
                     {
                        //----------------------------------------------
                        // The diffraction loss is to be determined
                        // using a combination of spherical earth
                        // diffraction loss and knife-edge diffraction
                        // loss.  The spherical earth diffraction
                        // subroutine SEDIFF has converged to a
                        // solution. Call subroutine KEDIFF to determine
                        // FSUBK, the knife-edge diffraction loss.
                        //----------------------------------------------

                        /*call*/ kediff(ant_data,
                                        alphat,
                                        dratio,
                                        epslnt,
                                        fsubk,
                                        hammsl,
                                        indxfc,
                                        nprofl,
                                        rlamda,
                                        xprofl,
                                        xtprof,
                                        zprofl,
                                        ztprof);

                        //----------------------------------------------
                        // Combine FSUBS, the spherical earth
                        // diffraction loss, with FSUBK, the knife-edge
                        // diffraction loss, to determine FSUBD, the
                        // diffraction loss.
                        //----------------------------------------------

                        alfaks = (hmdzro - pt1dif) / (pt2dif - pt1dif);
                        fsubd  = real(alfaks) * fsubk + real(1.0 - alfaks) * fsubs;

                        //----------------------------------------------
                        // Combine FSUBD, the diffraction loss, with
                        // FSUBM, the multipath loss, to determine
                        // FPPROP, the propagation loss.
                        //----------------------------------------------

                        alfamd = (frcmin - pt1mdf) / (pt2mdf - pt1mdf);
                        fpprop = (cabs(real(alfamd) * fsubm) + real((1.0 - alfamd) * fsubd)) *
                                 cexp(cmplx(0.0, 1.0) * std::atan2(aimag(fsubm), real(fsubm)));
                     }
                  }
                  else
                  {
                     if (hmdzro < pt1dif)
                     {
                        //----------------------------------------------
                        // The propagation loss is to be determined from
                        // multipath loss and diffraction loss.  The
                        // diffraction loss is to be determined from
                        // spherical earth diffraction loss only.  The
                        // spherical earth diffraction subroutine SEDIFF
                        // did not converge to a solution.  The target
                        // is not masked from the radar.  Set FPPROP,
                        // the propagation loss, to FSUBM, the multipath
                        // loss.
                        //----------------------------------------------

                        fpprop = fsubm;
                     }
                     else
                     {
                        //----------------------------------------------
                        // The propagation loss is to be determined from
                        // diffraction loss only.  The diffraction loss
                        // is to be determined using a combination of
                        // spherical earth diffraction loss and
                        // knife-edge diffraction loss.  The spherical
                        // earth diffraction subroutine SEDIFF did not
                        // converge to a solution.  Set FSUBD, the
                        // diffraction loss, to FSUBK, the knife-edge
                        // diffraction loss.
                        //----------------------------------------------

                        /*call*/ kediff(ant_data,
                                        alphat,
                                        dratio,
                                        epslnt,
                                        fsubk,
                                        hammsl,
                                        indxfc,
                                        nprofl,
                                        rlamda,
                                        xprofl,
                                        xtprof,
                                        zprofl,
                                        ztprof);

                        fsubd = fsubk;

                        //----------------------------------------------
                        // Combine FSUBD, the diffraction loss, with
                        // FSUBM, the multipath loss, to determine
                        // FPPROP, the propagation loss.
                        //----------------------------------------------

                        alfamd = (frcmin - pt1mdf) / (pt2mdf - pt1mdf);
                        fpprop = (cabs(real(alfamd) * fsubm) + real((1.0 - alfamd) * fsubd)) *
                                 cexp(cmplx(0.0, 1.0) * std::atan2(aimag(fsubm), real(fsubm)));
                     }
                  }
               }
               else
               {
                  //----------------------------------------------------
                  // The propagation loss is to be determined from
                  // multipath loss and diffraction loss.  The
                  // diffraction loss is to be determined from
                  // knife-edge diffraction loss only.  Call subroutine
                  // KEDIFF to determine FSUBK, the knife-edge
                  // diffraction loss.  Set FSUBD, the diffraction loss,
                  // to FSUBK, the knife-edge diffraction loss.
                  //----------------------------------------------------

                  /*call*/ kediff(ant_data, alphat, dratio, epslnt, fsubk, hammsl, indxfc, nprofl, rlamda, xprofl, xtprof, zprofl, ztprof);

                  fsubd = fsubk;

                  //----------------------------------------------------
                  // Combine FSUBD, the diffraction loss, with FSUBM,
                  // the multipath loss, to determine FPPROP, the
                  // propagation loss.
                  //----------------------------------------------------

                  alfamd = (frcmin - pt1mdf) / (pt2mdf - pt1mdf);
                  fpprop = (cabs(real(alfamd) * fsubm) + real((1.0 - alfamd) * fsubd)) *
                           cexp(cmplx(0.0, 1.0) * std::atan2(aimag(fsubm), real(fsubm)));
               }
            }
         }
         else
         {
            if (masked)
            {
               fpprop = cmplx(0.0);
            }
            else
            {
               //----------------------------------------------------------
               // The propagation loss is to be determined from multipath
               // loss.  Set FPPROP, the propagation loss, to FSUBM, the
               // multipath loss.
               //----------------------------------------------------------

               /*call*/ mltpth(ant_data,
                               alphat,
                               epslnt,
                               epsln1,
                               fsubm,
                               hammsl,
                               iend,
                               ipolar,
                               istart,
                               water_cover,
                               nareas,
                               nprofl,
                               pulwid,
                               ranget,
                               rlamda,
                               rrough,
                               sigmho,
                               tanepp,
                               taurlx,
                               visibl,
                               wndkno,
                               xprofl,
                               xtprof,
                               zprofl,
                               ztprof,
                               elvmsl,
                               use_surface_height,
                               surface_height,
                               soil_moisture,
                               water_temp,
                               sea_water);

               fpprop = fsubm;
            }
         }

         fsquared = fpprop * fpprop;
      }
      else
      {
         //-------------------------------------------------------------
         // There are no points in the terrain profile between the radar
         // and the target.  In this case, FPPROP, the propagation loss,
         // is the square root of the antenna gain in the direction of
         // the target.
         //-------------------------------------------------------------

         // F4 propagation factor should not be normalized by antenna gain (CR-106)
         /*call*/ // get_relative_gain(ant_data, alphat, epslnt, gtbelo);

         if (!mUnitTestPropagation)
         {
            fsquared = cmplx(1.0); // cmplx(gtbelo);
         }
         else
         {
            // test original code for duplication with alarm
            double gtbelo;
            /*call*/ get_relative_gain(ant_data, alphat, epslnt, gtbelo);
            fsquared = cmplx(gtbelo);
         }
      } // if (nprofl > 0
   }
   else
   {
      //----------------------------------------------------------------
      // The propagation loss is not to be determined.  In this case, if
      // the target is masked, set FPPROP, the propagation loss, equal
      // to zero.  If the target is not masked, the propagation loss
      // will be that due to the antenna pattern alone.
      //----------------------------------------------------------------

      if (!masked)
      {
         //-------------------------------------------------------------
         // The target is not masked. FPPROP, the propagation loss, is
         // the square root of the antenna gain in the direction of the
         // target.
         //-------------------------------------------------------------

         // F4 propagation factor should not be normalized by antenna gain (CR-106)
         /*call*/ // get_relative_gain(ant_data, alphat, epslnt, gtbelo);

         if (!mUnitTestPropagation)
         {
            fsquared = cmplx(1.0); // cmplx(gtbelo);
         }
         else
         {
            // test original code for duplication with alarm
            double gtbelo;
            /*call*/ get_relative_gain(ant_data, alphat, epslnt, gtbelo);
            fsquared = cmplx(gtbelo);
         }
      }
      else
      {
         //-------------------------------------------------------------
         // The target is masked.  Set FPPROP, the propagation loss,
         // equal to zero.
         //-------------------------------------------------------------

         fsquared = cmplx(0.0);
      }
   } // if (lpprop
}

//---------------------------------------------------------------------
//     Purpose:
//     -------
//     Determines whether the pattern propagation factor is to be
//     determined by a multipath model, a diffraction model, or a
//     combination of multipath model and diffraction model.
//
//     See reference 1, pages 4-5 for a description of the decision
//     process.
//
//
//     Inputs:
//     ------
//     HAMMSL  -  The radar height, in meters above mean sea level.
//
//     NPROFL  -  The number of points in the terrain profile
//                between the radar and the target.
//
//     RLAMDA  -  The radar wavelength, in meters.
//
//     TANEPT  -  The tangent of the elevation angle of the target
//                relative to the radar.
//
//     XPROFL  -  XPROFL(I), I = 0,1,...,NPROFL, is the x-coordinate
//                (horizontal), in meters, of the I-th point in the
//                terrain profile between the radar and the target.
//                XPROFL(0), the x-coordinate of the radar, is equal
//                to zero.
//
//     XTPROF  -  The x-coordinate, in meters, of the target in an
//                x-z coordinate system whose origin is located on
//                the mean sea level surface of the earth at the
//                radar.  The positive x-axis extends from the origin
//                toward the target and is tangent to the earth at
//                the radar.
//
//     ZPROFL  -  ZPROFL(I), I = 0,1,...,NPROFL, is the z-coordinate,
//                (vertical), in meters, of the I-th point in the
//                terrain profile between the radar and the target.
//                ZPROFL(0) is the height of the terrain at the radar.
//
//
//     Outputs:
//     -------
//     DRATIO  -  DRATIO(I), I = 1,2,...,NPROFL, is the ratio of the
//                vertical distance from the I-th terrain point to
//                the line-of-sight between the radar and the target
//                to the Fresnel clearance at the I-th terrain point.
//
//     FRCMIN  -  FRCMIN = min DRATIO(I), I = 1,2,...,NPROFL.  If
//                FRCMIN is less than 0.5 the pattern propagation
//                factor is determined by a diffraction model only.
//                If FRCMIN is greater than or equal to 0.5 and
//                less than 0.75, the pattern propagation factor is
//                determined from by a combination of a diffraction
//                model and a multipath model.  If FRCMIN is greater
//                than or equal to 0.75, the pattern propagation
//                factor is determined by a multipath model only.
//
//     INDXFC  -  The index of that point in the terrain profile
//                between the radar and the target which yields
//                the minimum value of the vector DRATIO; ie,
//                DRATIO(INDXFC) = min DRATIO(I), I = 1,2,...,NPROFL.
//
//
//     NONE.
//
//
//     Author:
//     ------
//     Science Applications International Corporation (SAIC)
//     2301 Yale Boulevard, SE, Suite E
//     Albuquerque, New Mexico 87106
//     (505) 766-7400
//
//
//     Notice:
//     ------
//     This module was designed by Science Applications International
//     Corporation (SAIC) for the US Air Force, for use in the
//     Advanced Low Altitude Radar Model (ALARM), owned by Wright
//     Labs, Analysis and Evaluation Branch (WL/AAWA-1).
//
//     This module is derived from algorithms and code developed for
//     the Air Force by the Massachusetts Institute of Technology's
//     (MIT) Lincoln Laboratory.  Under the terms of the license
//     granted by MIT, the following restriction applies:
//
//     COPYRIGHT (C) 1985,1988 MIT LINCOLN LABORATORY ALL RIGHTS RESERVED
//
//
//     References:
//     ----------
//     [1]  Ayasli, S. and Carlson, M.B.: "SEKE: A Computer Model
//          for Low-Altitude Radar Propagation Over Irregular
//          Terrain,"Project Report CMT-70, Massachusetts Institute
//          of Technology, Lincoln Laboratory, 1 May 1985.
//
//
//     Modified:
//     --------
//     Division 397
//     SAIC
//     1321 Research Park Drive
//     Dayton, OH 45432
//     (513) 429-6500
//
//     ALARM 3.0:
//     ---------
//     - DOPLER variable omitted from GEOMRT common block.
//       Variable DOPCSR, which includes the conical scan
//       rate, is now used in its place.  JCL
//
//---------------------------------------------------------------------

// static
void WsfEM_ALARM_Propagation::first(std::vector<double>&       dratio,
                                    double&                    frcmin,
                                    double                     hammsl,
                                    int&                       indxfc,
                                    int                        nprofl,
                                    double                     rlamda,
                                    double                     tanept,
                                    const std::vector<double>& xprofl,
                                    double                     xtprof,
                                    const std::vector<double>& zprofl)
{
   // implicit none

   //-------------------------------------------------------------------
   // Passed parameters
   //-------------------------------------------------------------------

   // real(8), intent(in)  ::  hammsl, rlamda, tanept, xprofl(0:), &
   //      & xtprof,  zprofl(0:)

   // real(8), intent(out) :: dratio(0:), frcmin

   // integer, intent(in)  :: nprofl
   // integer, intent(out) :: indxfc

   //-------------------------------------------------------------------
   // Local variables
   //-------------------------------------------------------------------

   double delta, delzro, dsub1, dsub2;
   int    i;

   //-------------------------------------------------------------------
   // Determine DSUB1, in meters, the distance from the radar to the
   // first point in the terrain profile, and DSUB2, in meters, the
   // distance from the first point in the terrain profile to the
   // target.  Then determine DELTA, in meters, the vertical distance
   // from the first terrain point to the line-of-sight between the
   // radar and the target, and DELZRO, in meters, the Fresnel clearance
   // at the first terrain point.
   //-------------------------------------------------------------------

   dsub1  = xprofl[1];
   dsub2  = xtprof - dsub1;
   delta  = hammsl + tanept * dsub1 - zprofl[1];
   delzro = sqrt(rlamda * dsub1 * (dsub2 / xtprof));

   //-------------------------------------------------------------------
   // Determine DRATIO(1), the ratio of the vertical distance from the
   // first terrain point to the line-of-sight between the radar and the
   // target to the Fresnel clearance at the first terrain point.  Set
   // FRCMIN, the minimum of these ratios at each terrain point in the
   // profile, to DRATIO(1), and set INDXFC, the index of the point of
   // minimum ratio, to one.  FRCMIN and INDXFC will be updated below.
   //-------------------------------------------------------------------

   dratio[1] = delta / delzro;
   frcmin    = dratio[1];
   indxfc    = 1;

   //-------------------------------------------------------------------
   // Loop through the remaining points in the terrain profile to
   // determine the DRATIO vector and to update the values of FRCMIN and
   // INDXFC.
   //-------------------------------------------------------------------

   for (i = 2; i <= nprofl; ++i)
   {
      //----------------------------------------------------------------
      // Determine DSUB1, in meters, the distance from the radar to the
      // I-th point in the terrain profile, and DSUB2, in meters, the
      // distance from the I-th point in the terrain profile to the
      // target.  Then determine DELTA, in meters, the vertical distance
      // from the I-th terrain point to the line-of-sight between the
      // radar and the target, and DELZRO, in meters, the Fresnel
      // clearance at the I-th terrain point.
      //----------------------------------------------------------------

      dsub1  = xprofl[i];
      dsub2  = xtprof - dsub1;
      delta  = hammsl + tanept * dsub1 - zprofl[i];
      delzro = sqrt(rlamda * dsub1 * (dsub2 / xtprof));

      //----------------------------------------------------------------
      // Determine DRATIO(I), the ratio of the vertical distance from
      // the I-th terrain point to the line-of-sight between the radar
      // and the target to the Fresnel clearance at the I-th terrain
      // point.
      //----------------------------------------------------------------

      dratio[i] = delta / delzro;

      //----------------------------------------------------------------
      // If appropriate, update the values of FRCMIN and INDXFC.
      //----------------------------------------------------------------

      if (dratio[i] < frcmin)
      {
         //-------------------------------------------------------------
         // The I-th terrain point is a better estimate to the point of
         // minimum clearance.  Set FRCMIN equal to DRATIO(I) and set
         // INDXFC equal to I.
         //-------------------------------------------------------------

         frcmin = dratio[i];
         indxfc = i;
      }
   } // for (i
}

//---------------------------------------------------------------------
//     Purpose:
//     -------
//     Determines the ratio of the height of the highest mask or minimum
//     clearance point measured from the best fit line to the terrain
//     profile to the Fresnel clearance at the point.  This ratio is
//     used to determine if the diffraction component of the pattern
//     propagation factor is a function of spherical or knife edge or
//     both.
//
//
//     Inputs:
//     ------
//     DELTAG  -  The ground range increment, in meters, between
//                points in the terrain profile between the radar
//                and the target.
//
//     ELVMSL  -  ELVMSL(I), I = 1,2,...,NPROFL, is the height above
//                mean sea level, in meters, of the I-th point in the
//                terrain profile between the radar and the target.
//
//     GRANGT  -
//
//     INDXFC  -  The index of that point in the terrain profile
//                between the radar and the target which yields
//                the minimum value of the vector DRATIO; ie,
//                DRATIO(INDXFC) = min DRATIO(I), I = 1,2,...,NPROFL.
//
//     NPROFL  -  The number of points in the terrain profile
//                between the radar and the target.
//
//     RLAMDA  -  The radar wavelength, in meters.
//
//
//     Outputs:
//     -------
//     HMDZRO  -
//
//
//     Author:
//     ------
//     Science Applications International Corporation (SAIC)
//     2301 Yale Boulevard, SE, Suite E
//     Albuquerque, New Mexico 87106
//     (505) 766-7400
//
//
//     Notice:
//     ------
//     This module was designed by Science Applications International
//     Corporation (SAIC) for the US Air Force, for use in the
//     Advanced Low Altitude Radar Model (ALARM), owned by Wright
//     Labs, Analysis and Evaluation Branch (WL/AAWA-1).
//
//     This module is derived from algorithms and code developed for
//     the Air Force by the Massachusetts Institute of Technology's
//     (MIT) Lincoln Laboratory.  Under the terms of the license
//     granted by MIT, the following restriction applies:
//
//     COPYRIGHT (C) 1985,1988 MIT LINCOLN LABORATORY ALL RIGHTS RESERVED
//
//
//     Modified:
//     --------
//     Division 397
//     SAIC
//     1321 Research Park Drive
//     Dayton, OH 45432
//     (513) 429-6500
//
//     ALARM 3.0:
//     ---------
//     - DOPLER variable omitted from GEOMRT common block.
//       Variable DOPCSR, which includes the conical scan
//       rate, is now used in its place.  JCL
//
//    ALARM 4
//    -------
//    19-May-2000  - Corrected the elvmsl array declaration.
//                   See  SPCR #1191. MWS
//---------------------------------------------------------------------

// static
void WsfEM_ALARM_Propagation::second(double                     deltag,
                                     const std::vector<double>& elvmsl,
                                     double                     grangt,
                                     double&                    hmdzro,
                                     int                        indxfc,
                                     int                        nprofl,
                                     double                     rlamda)
{
   // implicit none

   //-------------------------------------------------------------------
   // Passed parameters
   //-------------------------------------------------------------------

   // real(8), intent(in)  ::  deltag, elvmsl(*), grangt, rlamda

   // real(8), intent(out) :: hmdzro
   // integer, intent(in)  :: indxfc, nprofl

   //-------------------------------------------------------------------
   // Local variables
   //-------------------------------------------------------------------

   // C++MOD: 'constX' in next line used to be 'const'.
   double aline0, aline1, constX, effhgt, grrtot;

   /*call*/ linfit(aline0, aline1, deltag, elvmsl, nprofl);

   grrtot = indxfc * deltag;
   constX = sqrt(rlamda * grrtot * (grangt - grrtot) / grangt);
   effhgt = elvmsl[indxfc] - (aline0 + aline1 * grrtot);

   hmdzro = effhgt / constX;
}

//---------------------------------------------------------------------
//     Purpose:
//     -------
//     Supports the determination of propagation factor for the SEKE
//     routines.
//
//
//     Inputs:
//     ------
//     DELTAG  -  The ground range increment, in meters, between
//                points in the terrain profile between the radar
//                and the target.
//
//     ELVMSL  -  ELVMSL(I), I = 1,2,...,NPROFL, is the height above
//                mean sea level, in meters, of the I-th point in the
//                terrain profile between the radar and the target.
//
//     NPROFL  -  The number of points in the terrain profile
//                between the radar and the target.
//
//
//     Outputs:
//     -------
//     ALINE0  -
//
//     ALINE1  -
//
//
//     NONE.
//
//
//     Author:
//     ------
//     Science Applications International Corporation (SAIC)
//     2301 Yale Boulevard, SE, Suite E
//     Albuquerque, New Mexico 87106
//     (505) 766-7400
//
//
//     Notice:
//     ------
//     This module was designed by Science Applications International
//     Corporation (SAIC) for the US Air Force, for use in the
//     Advanced Low Altitude Radar Model (ALARM), owned by Wright
//     Labs, Analysis and Evaluation Branch (WL/AAWA-1).
//
//     This module is derived from algorithms and code developed for
//     the Air Force by the Massachusetts Institute of Technology's
//     (MIT) Lincoln Laboratory.  Under the terms of the license
//     granted by MIT, the following restriction applies:
//
//     COPYRIGHT (C) 1985,1988 MIT LINCOLN LABORATORY ALL RIGHTS RESERVED
//
//---------------------------------------------------------------------

// static
void WsfEM_ALARM_Propagation::linfit(double& aline0, double& aline1, double deltag, const std::vector<double>& elvmsl, int nprofl)
{
   // implicit none

   //-------------------------------------------------------------------
   // Passed parameters
   //-------------------------------------------------------------------

   // real(8), intent(out) ::  aline0, aline1

   // real(8), intent(in)  :: deltag, elvmsl(*)

   // integer, intent(in)  ::  nprofl

   //-------------------------------------------------------------------
   // local variables
   //-------------------------------------------------------------------

   double rnprof, sumx, sumx2, sumxy, sumy, x;

   int i;

   if (nprofl > 2)
   {
      //----------------------------------------------------------------
      // Initialize quantities to zero.
      //----------------------------------------------------------------

      sumx  = 0.0;
      sumx2 = 0.0;
      sumxy = 0.0;
      sumy  = 0.0;

      for (i = 1; i <= nprofl; ++i)
      {
         x     = i * deltag;
         sumx  = sumx + x;
         sumx2 = sumx2 + x * x;
         sumxy = sumxy + x * elvmsl[i];
         sumy  = sumy + elvmsl[i];
      }

      rnprof = real(nprofl);

      aline1 = (sumxy - sumx * sumy / rnprof) / (sumx2 - sumx * sumx / rnprof);
      aline0 = (sumy - sumx * aline1) / rnprof;
   }
   else if (nprofl > 1)
   {
      aline1 = (elvmsl[2] - elvmsl[1]) / deltag;
      aline0 = elvmsl[1] - aline1 * deltag;
   }
   else
   {
      aline1 = 0.0;
      aline0 = elvmsl[1];
   }
}

// =================================================================================================
// Code from ALARM 'diffraction.f90'
// =================================================================================================

//---------------------------------------------------------------------
//     Purpose:
//     -------
//     Determines the spherical earth diffraction component of the
//     pattern propagation factor.
//
//
//     Inputs:
//     ------
//     ALPHAT  -  The azimuth angle to the target, in radians.
//
//     CONST3  -
//
//     CONST4  -
//
//     DELTAG  -  The ground range increment, in meters, between
//                points in the terrain profile between the radar
//                and the target.
//
//     DRATIO  -  DRATIO(I), I = 1,2,...,NPROFL, is the ratio of the
//                vertical distance from the I-th terrain point to
//                the line-of-sight between the radar and the target
//                to the Fresnel clearance at the I-th terrain point.
//
//     ELVMSL  -  ELVMSL(I), I = 1,2,...,NPROFL, is the height above
//                mean sea level, in meters, of the I-th point in
//                the terrain profile between the radar and the
//                target.
//
//     EPSLNT  -  The elevation angle to the target, in radians.
//
//     GRANGT  -  Ground range to from radar to target in meters.
//
//     HAMMSL  -  The radar height above mean sea level in meters.
//
//     HTMMSL  -  The target height above mean sea level in meters.
//
//     INDXFC  -  The index of that point in the terrain profile
//                between the radar and the target which yields
//                the minimum value of the vector DRATIO; ie,
//                DRATIO(INDXFC) = min DRATIO(I), I = 1,2,...,NPROFL.
//
//     NPROFL  -  The number of points in the terrain profile
//                between the radar and the target.
//
//     RKFACT  -  Refraction factor, unitless.
//
//     RLAMDA  -  The radar wavelength, in meters.
//
//     XPROFL  -  XPROFL(I), I = 0,1,...,NPROFL, is the x-coordinate
//                (horizontal), in meters, of the I-th point in the
//                terrain profile between the radar and the target.
//                XPROFL(0), the x-coordinate of the radar, is equal
//                to zero.
//
//     ZPROFL  -  ZPROFL(I), I = 0,1,...,NPROFL, is the z-coordinate,
//                (vertical), in meters, of the I-th point in the
//                terrain profile between the radar and the target.
//                ZPROFL(0) is the height of the terrain at the radar.
//
//
//     Outputs:
//     -------
//     COVVRG  -   True if the routine converges to a solution.
//
//     FSUBS   -   The spherical earth diffraction loss.
//
//
//     Author:
//     ------
//     Science Applications International Corporation (SAIC)
//     2301 Yale Boulevard, SE, Suite E
//     Albuquerque, New Mexico 87106
//     (505) 766-7400
//
//
//     Notice:
//     ------
//     This module was designed by Science Applications International
//     Corporation (SAIC) for the US Air Force, for use in the
//     Advanced Low Altitude Radar Model (ALARM), owned by Wright
//     Labs, Analysis and Evaluation Branch (WL/AAWA-1).
//
//     This module is derived from algorithms and code developed for
//     the Air Force by the Massachusetts Institute of Technology's
//     (MIT) Lincoln Laboratory.  Under the terms of the license
//     granted by MIT, the following restriction applies:
//
//     COPYRIGHT (C) 1985,1988 MIT LINCOLN LABORATORY ALL RIGHTS RESERVED
//
//
//     Modified:
//     --------
//     Division 397
//     SAIC
//     1321 Research Park Drive
//     Dayton, OH 45432
//     (513) 429-6500
//
//     ALARM 3.0:
//     ---------
//     - DOPLER variable omitted from GEOMRT common block.
//       Variable DOPCSR, which includes the conical scan
//       rate, is now used in its place.  PJH & JCL
//
//     - Subroutine OFFBOR called to determine the off-
//       boresight angles in azimuth and elevation of an
//       arbitrary point in space with respect to the
//       radar antenna.  PJH & JCL
//
//
//     ALARM 3.2:
//     ---------
//     - TGAINS common block removed from subroutine TGAIN to support
//       stand alone transmit antenna.  Those variables (AZTMAX, AZTMIN,
//       AZTTAB, ELTMAX, ELTMIN, ELTTAB, ITXTYP, NUMAZT, NUMELT,TGTABL,
//       and TXGMIN) previously accessed through the common block are
//       now passed through the TGAIN argument list.  (SPCR-1056, JCL)
//
//     - Included TGAIN common block so that the AZTMAX, AZTMIN, ELTMAX,
//       ELTMIN, ITXTYP, NUMAZT, NUMELT, TGTABL and TXGMIN variables
//       could be passed to the TGAIN subroutine.  (SPCR-1056, JCL)
//
//     Modifications:
//     -------------
//     19-Dec-2003   - Modified to use the new antenna class.
//                     Removed unnecessary variables, ALPHAR, EPSLNR,
//                     OFFAZT from the argument list.  See SPCR #1310.
//---------------------------------------------------------------------

void WsfEM_ALARM_Propagation::sediff(antenna&                   ant_data,
                                     double                     alphat,
                                     double                     const3,
                                     double                     const4,
                                     bool&                      convrg,
                                     double                     deltag,
                                     const std::vector<double>& dratio,
                                     const std::vector<double>& elvmsl,
                                     double                     epslnt,
                                     double&                    fsubs,
                                     double                     grangt,
                                     double                     hammsl,
                                     double                     htmmsl,
                                     int                        indxfc,
                                     int                        nprofl,
                                     double                     rkfact,
                                     double                     rlamda,
                                     const std::vector<double>& xprofl,
                                     const std::vector<double>& zprofl)
{
   // use physical_consts, only : rezero, onethr
   // use propagation_consts

   // implicit none

   //-------------------------------------------------------------------
   // Passed parameters
   //-------------------------------------------------------------------

   // type (antenna), intent(in)  :: ant_data
   // real(8),        intent(in)  :: alphat         ! radians
   // real(8),        intent(in)  :: const3
   // real(8),        intent(in)  :: const4
   // real(8),        intent(in)  :: deltag         ! meters
   // real(8),        intent(in)  :: dratio(0:)
   // real(8),        intent(in)  :: elvmsl(*)      ! meters
   // real(8),        intent(in)  :: epslnt         ! radians
   // real(8),        intent(in)  :: grangt         ! meters
   // real(8),        intent(in)  :: hammsl         ! meters
   // real(8),        intent(in)  :: htmmsl         ! meters
   // real(8),        intent(in)  :: rkfact
   // real(8),        intent(in)  :: rlamda         ! meters
   // real(8),        intent(in)  :: xprofl(0:)     ! meters
   // real(8),        intent(in)  :: zprofl(0:)     ! meters

   // real(8),        intent(out) :: fsubs
   // integer,        intent(in)  :: indxfc
   // integer,        intent(in)  :: nprofl
   // logical,        intent(out) :: convrg

   //-------------------------------------------------------------------
   // Local variables
   //-------------------------------------------------------------------
   enum
   {
      iterations = 35
   };

   COMPLEX eterm, fofxyz, fsubny, fsubnz, psi, tsubj, zeta, znexpy, znexpz;

   double /*airdot(iterations), airzro(iterations),*/ aline0, aline1, apara0, apara1, apara2, atjm1, atsubj, crootr,
      epslnp,
      /*NOT-USED gtilde,*/ hzero, radeff, radinv, rearth, rzero, taneps, xargmt, yargmt, z1eff, z1effh, z2eff, z2effh,
      zargmt;

   int j;

   //-------------------------------------------------------------------

   const double airdot[iterations + 1] = {0.0,        0.70121E0, -0.80311E0, 0.86520E0, -0.91085E0, 0.94733E0,
                                          -0.97792E0, 1.00437E0, -1.02773E0, 1.04872E0, -1.06779E0, 1.08530E0,
                                          -1.10150E0, 1.11659E0, -1.13073E0, 1.14403E0, -1.15660E0, 1.16853E0,
                                          -1.17988E0, 1.19070E0, -1.20106E0, 1.21098E0, -1.22052E0, 1.22970E0,
                                          -1.23854E0, 1.24708E0, -1.25534E0, 1.26334E0, -1.27109E0, 1.27861E0,
                                          -1.28592E0, 1.29302E0, -1.29994E0, 1.30667E0, -1.31324E0, 1.31965E0};

   const double airzro[iterations + 1] = {0.0,
                                          -2.33810741E0,
                                          -4.08794944E0,
                                          -5.52055983E0,
                                          -6.78670809E0,
                                          -7.94413359E0,
                                          -9.02265085E0,
                                          -10.04017434E0,
                                          -11.00852430E0,
                                          -11.93601556E0,
                                          -12.82877675E0,
                                          -13.69148903E0,
                                          -14.52782995E0,
                                          -15.34075514E0,
                                          -16.13268516E0,
                                          -16.90563400E0,
                                          -17.66130011E0,
                                          -18.40113260E0,
                                          -19.12638047E0,
                                          -19.83812989E0,
                                          -20.53733291E0,
                                          -21.22482994E0,
                                          -21.90136760E0,
                                          -22.56761292E0,
                                          -23.22416500E0,
                                          -23.87156446E0,
                                          -24.51030124E0,
                                          -25.14082117E0,
                                          -25.76353140E0,
                                          -26.37880505E0,
                                          -26.98698511E0,
                                          -27.58838781E0,
                                          -28.18330550E0,
                                          -28.77200917E0,
                                          -29.35475056E0,
                                          -29.93176412E0};

   rearth = rkfact * rezero;

   /*call*/ parfit(apara0, apara1, apara2, deltag, elvmsl, nprofl);

   radinv = 1.0 / rearth - 2.0 * apara2 / rkfact;
   radeff = 1.0 / radinv;

   if (radeff > 0.0)
   {
      z1eff = hammsl - apara0;
      z2eff = htmmsl - apara0 - grangt * (apara1 + grangt * apara2);
   }
   else
   {
      /*call*/ linfit(aline0, aline1, deltag, elvmsl, nprofl);

      z1eff  = hammsl - aline0;
      z2eff  = htmmsl - aline0 - grangt * aline1;
      radeff = rearth;
   }

   if (z1eff <= 0.0)
   {
      z1eff = 0.25 * rlamda;
   }
   if (z2eff <= 0.0)
   {
      z2eff = 0.25 * rlamda;
   }

   if (z2eff > z1eff)
   {
      z1effh = z2eff;
      z2effh = z1eff;
   }
   else
   {
      z1effh = z1eff;
      z2effh = z2eff;
   }

   convrg = false;

   crootr = pow(radeff, onethr);
   rzero  = const3 * crootr * crootr;
   hzero  = const4 * crootr;

   xargmt = grangt / rzero;
   yargmt = z2effh / hzero;
   zargmt = z1effh / hzero;

   atjm1  = 10000.0;
   fofxyz = cmplx(0.0, 0.0);

   for (j = 1; j <= iterations; ++j)
   {
      znexpy = cmplx(airzro[j], 0.0) + ep1pi3 * cmplx(yargmt, 0.0);
      fsubny = airy(znexpy) / (ep1pi3 * cmplx(airdot[j], 0.0));
      znexpz = cmplx(airzro[j], 0.0) + ep1pi3 * cmplx(zargmt, 0.0);
      fsubnz = airy(znexpz) / (ep1pi3 * cmplx(airdot[j], 0.0));

      psi    = ctwoth * (znexpy * csqrt(znexpy));
      zeta   = ctwoth * (znexpz * csqrt(znexpz));
      eterm  = ccoeff * cmplx(airzro[j], 0.0) * cmplx(xargmt, 0.0) - psi - zeta;
      tsubj  = fsubny * fsubnz * cexp(eterm);
      atsubj = cabs(tsubj);
      fofxyz = fofxyz + tsubj;

      if (atsubj > 10000.0)
      {
         goto label_20;
      } // early exit

      if (atjm1 < 0.0005 && atsubj < 0.0005)
      {
         if (dratio[indxfc] > 0.0)
         {
            epslnp = epslnt;
         }
         else
         {
            taneps = (zprofl[indxfc] - hammsl) / xprofl[indxfc];
            epslnp = atan(taneps);
         }

         // F4 propagation factor should not be normalized by antenna gain (CR-106)
         /*call*/ // get_relative_gain(ant_data, alphat, epslnp, gtilde);

         if (!mUnitTestPropagation)
         {
            fsubs = /*sqrt(gtilde) **/ tsqrpi * sqrt(xargmt) * cabs(fofxyz);

            double gtilde = 1.0;
            /*call*/ get_relative_gain(ant_data, alphat, epslnp, gtilde);
            if ((sqrt(gtilde) * fsubs) < 2.0)
            {
               convrg = true;
            }
         }
         else // unit test for matching alarm propagation factors.
         {
            double gtilde = 1.0;
            /*call*/ get_relative_gain(ant_data, alphat, epslnp, gtilde);
            fsubs = sqrt(gtilde) * tsqrpi * sqrt(xargmt) * cabs(fofxyz);
            if (gtilde < 2.0)
            {
               convrg = true;
            }
         }
         goto label_20; // early exit
      }

      atjm1 = atsubj;
   } // j

label_20:;
   if (DebugEnabled())
   {
      auto logger = ut::log::debug();
      logger.GetStream().precision(16);
      if (convrg)
      {
         logger << "sediff: fsubs=" << fsubs;
      }
      else
      {
         logger << "sediff: did not converge";
      }
   }
}

//---------------------------------------------------------------------
//     Purpose:
//     -------
//     Determines the knife edge diffraction component of the pattern
//     propagation factor.
//
//
//     Inputs:
//     ------
//     ALPHAT  -  The azimuth angle to the target, in radians.
//
//     DRATIO  -  DRATIO(I), I = 1,2,...,NPROFL, is the ratio of the
//                vertical distance from the I-th terrain point to
//                the line-of-sight between the radar and the target
//                to the Fresnel clearance at the I-th terrain point.
//
//     EPSLNT  -  The elevation angle to the target, in radians.
//
//     HAMMSL  -  The radar height above mean sea level.
//
//     INDXFC  -  The index of that point in the terrain profile
//                between the radar and the target which yields
//                the minimum value of the vector DRATIO; ie,
//                DRATIO(INDXFC) = min DRATIO(I), I = 1,2,...,NPROFL.
//     NPROFL  -  The number of points in the terrain profile
//                between the radar and the target.
//
//     RLAMDA  -  The radar wavelength, in meters.
//
//     XPROFL  -  XPROFL(I), I = 0,1,...,NPROFL, is the x-coordinate
//                (horizontal), in meters, of the I-th point in the
//                terrain profile between the radar and the target.
//                XPROFL(0), the x-coordinate of the radar, is equal
//                to zero.
//
//     XTPROF  -  The x-coordinate of the target in an x-z coordinate
//                system whose origin is located on the mean sea level
//                surface of the Earth at the site.  The positive
//                x-axis extends from the origin toward the target and
//                is tangent to the Earth at the radar.  Units are in
//                meters.
//
//     ZPROFL  -  ZPROFL(I), I = 0,1,...,NPROFL, is the z-coordinate,
//                (vertical), in meters, of the I-th point in the
//                terrain profile between the radar and the target.
//                ZPROFL(0) is the height of the terrain at the radar.
//
//     ZTPROF  -  The z-coordinate of the target in an x-z coordinate
//                system whose origin is located on the mean sea level
//                surface of the Earth at the site.  (Positive z is up)
//                Units are in meters.
//
//     Outputs:
//     -------
//     FSUBK   -  The knife-edge diffraction loss.
//
//
//     Author:
//     ------
//     Science Applications International Corporation (SAIC)
//     2301 Yale Boulevard, SE, Suite E
//     Albuquerque, New Mexico 87106
//     (505) 766-7400
//
//
//     Notice:
//     ------
//     This module was designed by Science Applications International
//     Corporation (SAIC) for the US Air Force, for use in the
//     Advanced Low Altitude Radar Model (ALARM), owned by Wright
//     Labs, Analysis and Evaluation Branch (WL/AAWA-1).
//
//     This module is derived from algorithms and code developed for
//     the Air Force by the Massachusetts Institute of Technology's
//     (MIT) Lincoln Laboratory.  Under the terms of the license
//     granted by MIT, the following restriction applies:
//
//     COPYRIGHT (C) 1985,1988 MIT LINCOLN LABORATORY ALL RIGHTS RESERVED
//
//
//     References:
//     ----------
//     [1]  Beckmann, P. and Spizzichino, A.: "The Scattering of
//          Electromagnetic Waves From Rough Surfaces," Artech
//          House, Inc, Norwood, Massachusetts, 1987.
//
//     Modifications:
//     --------------
//     19-Dec-2003    - Modified to use the new antenna class.
//                      Removed unnecessary variables, ALPHAR, EPSLNR,
//                      OFFAZT from argument list.  See SPCR #1310.
//---------------------------------------------------------------------

void WsfEM_ALARM_Propagation::kediff(antenna&                   ant_data,
                                     double                     alphat,
                                     std::vector<double>&       dratio,
                                     double                     epslnt,
                                     double&                    fsubk,
                                     double                     hammsl,
                                     int                        indxfc,
                                     int                        nprofl,
                                     double                     rlamda,
                                     const std::vector<double>& xprofl,
                                     double                     xtprof,
                                     const std::vector<double>& zprofl,
                                     double                     ztprof)
{
   // use error_check_mod, only : fatal_error

   // implicit none

   //-------------------------------------------------------------------
   // Passed parameters
   //-------------------------------------------------------------------

   // type (antenna), intent(in)  :: ant_data
   // real(8),        intent(in)  ::  alphat         ! radians
   // real(8),        intent(in)  ::  dratio(0:)
   // real(8),        intent(in)  ::  epslnt         ! radians
   // real(8),        intent(in)  ::  hammsl         ! meters
   // real(8),        intent(in)  ::  rlamda         ! meters
   // real(8),        intent(in)  ::  xprofl(0:)     ! meters
   // real(8),        intent(in)  ::  xtprof         ! meters
   // real(8),        intent(in)  ::  zprofl(0:)     ! meters
   // real(8),        intent(in)  ::  ztprof         ! meters

   // real(8),        intent(out) :: fsubk
   // integer,        intent(in)  :: indxfc
   // integer,        intent(in)  :: nprofl

   //-------------------------------------------------------------------
   // Local variables
   //-------------------------------------------------------------------

   enum
   {
      maxres = 10
   };
   enum
   {
      mknife = 2
   };
   enum
   {
      mxfeat = 2
   };
   enum
   {
      hunk = 512
   };


   int i, ileft, imain, iright, isave, j, k, l, nedges, nlocal;

   // moved to header
   // static int indxmx_size = 0;
   // static std::vector<int> indxmx;

   //-------------------------------------------------------------------
   // Allocate memory and initialize
   //-------------------------------------------------------------------
   if (nprofl > indxmx_size)
   {
      //----------------------------------------------------------------
      // Allocate arrays.  Memory is allocated in hunks to minimize
      // allocate/deallocate thrashing if nprofl is gradually creeping
      // up.
      //----------------------------------------------------------------
      indxmx_size = indxmx_size + (abs(nprofl / hunk) + 1) * hunk;

      // NOTE: Arrays that are start at index 1 will still have element 0 allocated
      //       (but ignored) so as to allow the code to continue 1-based indexing.
      indxmx.resize(indxmx_size + 1); // 1:indxmx_size

      // if (i > 0) then
      //    call fatal_error('KEDIFF', 'Can't allocate memory')
      // end if
   }

   indxmx.assign(indxmx_size + 1, 0);

   //-------------------------------------------------------------------
   // Initialize FSUBK, the knife-edge diffraction loss, to one.
   //-------------------------------------------------------------------

   fsubk = 1.0;

   //-------------------------------------------------------------------
   // Initialize NLOCAL, the number of local maximum elevation points,
   // to zero.
   //-------------------------------------------------------------------

   nlocal = 0;

   //-------------------------------------------------------------------
   // A point in the terrain profile is considered to be a local
   // maximum elevation point if MXFEAT points on either side of
   // the point all have elevations less than the point.  Loop
   // through the points in the terrain profile searching for these
   // local maximum elevation points.
   //-------------------------------------------------------------------

   for (i = mxfeat; i <= nprofl - mxfeat; ++i)
   {
      //----------------------------------------------------------------
      // Determine if the I-th point in the terrain profile is a local
      // maximum elevation point.
      //----------------------------------------------------------------

      for (j = 1; j <= mxfeat; ++j)
      {
         //-------------------------------------------------------------
         // Check each of the MXFEAT points on either side of the I-th
         // point in the terrain profile to determine if its elevation is
         // less than the elevation of the I-th point.
         //-------------------------------------------------------------

         if (zprofl[i - j] >= zprofl[i] || zprofl[i + j] >= zprofl[i])
         {
            goto label_20;
         }
      } // j

      //----------------------------------------------------------------
      // The I-th point in the terrain profile is a local maximum
      // elevation point.  Increment NLOCAL, the number of local
      // maximum elevation points.
      //----------------------------------------------------------------

      nlocal = nlocal + 1;

      indxmx[nlocal] = i;

   label_20:;
   } // i

   if (nlocal > 0)
   {
      //----------------------------------------------------------------
      // Order the INDXMX vector such that INDXMX(1) is the index of the
      // local maximum elevation point which has the minimum ratio of
      // the direct ray terrain clearance to the Fresnel clearance and
      // INDXMX(NLOCAL) is the index of the local maximum elevation
      // point which has the maximum ratio of the direct ray terrain
      // clearance to the Fresnel clearance.  INDXMX(1) is the index of
      // the point in the terrain profile which will be taken as the
      // first or main knife-edge.
      //----------------------------------------------------------------

      for (l = 1; l <= nlocal - 1; ++l)
      {
         for (k = l + 1; k <= nlocal; ++k)
         {
            if (dratio[indxmx[k]] < dratio[indxmx[l]])
            {
               //-------------------------------------------------------
               // The ratio of the direct ray terrain clearance to the Fresnel
               // clearance at the INDXMX(K)-th point is less than the same ratio
               // for the INDXMX(L)-th point.  Interchange these two indices.
               //-------------------------------------------------------

               isave     = indxmx[l];
               indxmx[l] = indxmx[k];
               indxmx[k] = isave;
            }
         } // k
      }    // l

      //----------------------------------------------------------------
      // Since NLOCAL is greater than zero, the first or main knife-edge
      // exists, and its index is INDXMX(1).  Initialize NEDGES, the
      // number of knife-edges, to one, and set IMAIN, the index of
      // the first or main knife-edge, to INDXMX(1).  Set ILEFT to zero
      // to indicate that a knife-edge between the radar and the main
      // knife-edge has not been found.  Set IRIGHT to zero to indicate
      // that a knife-edge between the main knife-edge and the target
      // has not been found.
      //----------------------------------------------------------------

      nedges = 1;
      imain  = indxmx[1];
      ileft  = 0;
      iright = 0;

      //----------------------------------------------------------------
      // If MKNIFE, the number of knife-edges to use in estimating
      // the knife-edge diffraction loss, is one, do not search for
      // additional knife-edges.
      //----------------------------------------------------------------

      if (mknife > 1)
      {
         //-------------------------------------------------------------
         // The program is currently set to consider a maximum of three
         // knife-edges; the main knife-edge, the second knife-edge on one
         // side of the main knife-edge, and the third knife-edge on the
         // other side of the main knife-edge.  Loop through the local
         // maximum elevation points attempting to find additional knife-
         // edges on either side of the main knife-edge.
         //-------------------------------------------------------------

         for (l = 2; l <= nlocal; ++l)
         {
            if (indxmx[l] < imain)
            {
               //-------------------------------------------------------
               // The local maximum elevation point of index INDXMX(L)
               // is between the radar and the main knife-edge.  Check
               // ILEFT to determine if a knife-edge has already been
               // located on this side of the main knife-edge.
               //-------------------------------------------------------

               if (ileft == 0)
               {
                  //----------------------------------------------------
                  // A knife-edge has not been located on this side of
                  // the main knife-edge.  Determine if this point is
                  // located at a sufficient distance from the main
                  // knife-edge to qualify as another knife- edge.
                  //----------------------------------------------------

                  if (imain - indxmx[l] > maxres)
                  {
                     //-------------------------------------------------
                     // This local maximum elevation point qualifies as
                     // an additional knife-edge located between the
                     // radar and the main knife-edge.  Set ILEFT to the
                     // index of this local maximum elevation point to
                     // indicate that this knife-edge has been found.
                     // Increment NEDGES, the number of knife-edges
                     // found.
                     //-------------------------------------------------

                     ileft  = indxmx[l];
                     nedges = nedges + 1;
                  }
               }
            }
            else
            {
               //-------------------------------------------------------
               // The local maximum elevation point of index INDXMX(L)
               // is between the main knife-edge and the target.  Check
               // IRIGHT to determine if a knife-edge has already been
               // located on this side of the main knife-edge.
               //-------------------------------------------------------

               if (iright == 0)
               {
                  //----------------------------------------------------
                  // A knife-edge has not been located on this side of
                  // the main knife-edge.  Determine if this point is
                  // located at a sufficient distance from the main
                  // knife-edge to qualify as another knife- edge.
                  //----------------------------------------------------

                  if (indxmx[l] - imain > maxres)
                  {
                     //-------------------------------------------------
                     // This local maximum elevation point qualifies as
                     // an additional knife-edge located between the
                     // main knife-edge and the target.  Set IRIGHT to
                     // the index of this local maximum elevation point
                     // to indicate that this knife-edge has been found.
                     // Increment NEDGES, the number of knife-edges
                     // found.
                     //-------------------------------------------------

                     iright = indxmx[l];
                     nedges = nedges + 1;
                  }
               }
            }

            //----------------------------------------------------------
            // If NEDGES now equals MKNIFE, the number of knife-edges to
            // use in estimating the knife-edge diffraction loss, skip
            // to the end of the subroutine, where the knife-edge
            // diffraction will be determined.
            //----------------------------------------------------------

            if (nedges == mknife)
            {
               goto label_60;
            }
         } // l
      }

   label_60:;

      //----------------------------------------------------------------
      // Determine FSUBK, the knife-edge diffraction loss.
      //----------------------------------------------------------------

      /*call*/ deygou(ant_data, alphat, dratio, epslnt, fsubk, hammsl, ileft, imain, indxfc, iright, rlamda, xprofl, xtprof, zprofl, ztprof);
   }
   if (DebugEnabled())
   {
      auto logger = ut::log::debug() << "Knife-edge diffraction loss:";
      logger.GetStream().precision(16);
      logger.AddNote() << "fsubk: " << fsubk;
   }
}

//---------------------------------------------------------------------
//     Purpose:
//     -------
//     Supports the determination of the pattern propagation factor by
//     the SEKE routines.
//
//
//     Inputs:
//     ------
//     ZARGMT  -
//
//
//
//     Outputs:
//     -------
//     XXXXXX  -  Start
//
//
//     Author:
//     ------
//     Science Applications International Corporation (SAIC)
//     2301 Yale Boulevard, SE, Suite E
//     Albuquerque, New Mexico 87106
//     (505) 766-7400
//
//
//     Notice:
//     -------
//     This module was designed by Science Applications International
//     Corporation (SAIC) for the US Air Force, for use in the
//     Advanced Low Altitude Radar Model (ALARM), owned by Wright
//     Labs, Analysis and Evaluation Branch (WL/AAWA-1).
//
//     This module is derived from algorithms and code developed for
//     the Air Force by the Massachusetts Institute of Technology's
//     (MIT) Lincoln Laboratory.  Under the terms of the license
//     granted by MIT, the following restriction applies:
//
//     COPYRIGHT (C) 1985,1988 MIT LINCOLN LABORATORY ALL RIGHTS RESERVED
//
//---------------------------------------------------------------------

// static
WsfEM_ALARM_Propagation::COMPLEX WsfEM_ALARM_Propagation::airy(const COMPLEX& zargmt)
{
   // use propagation_consts

   // implicit none

   //-------------------------------------------------------------------
   // Passed parameters
   //-------------------------------------------------------------------

   // complex, intent(in) :: zargmt

   //-------------------------------------------------------------------
   // local variables
   //-------------------------------------------------------------------

   double tanarg, x;

   x = real(zargmt);

   if (x < 0.0)
   {
      tanarg = aimag(zargmt) / x;

      if (abs(tanarg) < dsqrt3)
      {
         return /*airy =*/conect(zargmt);
      }
      else
      {
         if (cabs(zargmt) > 4.0)
         {
            return /*airy =*/gaussq(zargmt);
         }
         else
         {
            return /*airy =*/powers(zargmt);
         }
      }
   }
   else
   {
      if (cabs(zargmt) > 2.0)
      {
         return /*airy =*/gaussq(zargmt);
      }
      else
      {
         return /*airy =*/powers(zargmt);
      }
   }
}

//---------------------------------------------------------------------
//     Purpose:
//     -------
//     Supports the determination of the pattern propagation factor by
//     SEKE routines.
//
//
//     Author:
//     ------
//     Science Applications International Corporation (SAIC)
//
//
//     Notice:
//     ------
//     This module was designed by Science Applications International
//     Corporation (SAIC) for the US Air Force, for use in the
//     Advanced Low Altitude Radar Model (ALARM), owned by Wright
//     Labs, Analysis and Evaluation Branch (WL/AAWA-1).
//
//     This module is derived from algorithms and code developed for
//     the Air Force by the Massachusetts Institute of Technology's
//     (MIT) Lincoln Laboratory.  Under the terms of the license
//     granted by MIT, the following restriction applies:
//
//     COPYRIGHT (C) 1985,1988 MIT LINCOLN LABORATORY ALL RIGHTS RESERVED
//
//---------------------------------------------------------------------

// static
WsfEM_ALARM_Propagation::COMPLEX WsfEM_ALARM_Propagation::gaussq(const COMPLEX& zargmt)
{
   // use propagation_consts

   // implicit none

   //-------------------------------------------------------------------
   // Passed parameters
   //-------------------------------------------------------------------

   // complex, intent(in) :: zargmt

   //-------------------------------------------------------------------
   // local variables
   //-------------------------------------------------------------------

   COMPLEX ctemp, sum, zeta;
   int     i;

   const double weight[] = {0.0,
                            2.677084371247434E-14,
                            6.636768688175870E-11,
                            1.758405638619854E-08,
                            1.371239148976848E-06,
                            4.435096659959217E-05,
                            7.155501075431907E-04,
                            6.488956601264211E-03,
                            3.644041585109798E-02,
                            1.439979241604145E-01,
                            8.123114134235980E-01};

   const double zeroes[] = {0.0,
                            1.408308107197377E+01,
                            1.021488548060315E+01,
                            7.441601846833691E+00,
                            5.307094307915284E+00,
                            3.634013504378772E+00,
                            2.331065231384954E+00,
                            1.344797083139945E+00,
                            6.418885840366331E-01,
                            2.010034600905718E-01,
                            8.059435921534400E-03};


   sum  = cmplx(0.0, 0.0);
   zeta = ctwoth * zargmt * csqrt(zargmt);

   for (i = 1; i <= 10; ++i)
   {
      sum = sum + (cmplx(weight[i], 0.0) / (cmplx(1.0, 0.0) + (cmplx(zeroes[i], 0.0) / zeta)));
   }


   ctemp = cmplx(dsqrpi, 0.0) / (csqrt(csqrt(zargmt))) * sum;

   return ctemp;
}

//---------------------------------------------------------------------
//     Purpose:
//     -------
//     Supports computation of the pattern propagation factor by the
//     SEKE routines.
//
//
//     Author:
//     ------
//     Science Applications International Corporation (SAIC)
//     2301 Yale Boulevard, SE, Suite E
//     Albuquerque, New Mexico 87106
//     (505) 766-7400
//
//
//     Notice:
//     ------
//     This module was designed by Science Applications International
//     Corporation (SAIC) for the US Air Force, for use in the
//     Advanced Low Altitude Radar Model (ALARM), owned by Wright
//     Labs, Analysis and Evaluation Branch (WL/AAWA-1).
//
//     This module is derived from algorithms and code developed for
//     the Air Force by the Massachusetts Institute of Technology's
//     (MIT) Lincoln Laboratory.  Under the terms of the license
//     granted by MIT, the following restriction applies:
//
//     COPYRIGHT (C) 1985,1988 MIT LINCOLN LABORATORY ALL RIGHTS RESERVED
//
//---------------------------------------------------------------------

// static
WsfEM_ALARM_Propagation::COMPLEX WsfEM_ALARM_Propagation::conect(const COMPLEX& zargmt)
{
   // use propagation_consts
   // use physical_consts, only : twopi

   // implicit none

   //-------------------------------------------------------------------
   // Passed parameters
   //-------------------------------------------------------------------

   // complex, intent(in) :: zargmt

   //-------------------------------------------------------------------
   // local variables
   //-------------------------------------------------------------------

   COMPLEX airy1, airy2, exparg, zarg1, zarg2, zeta;
   double  x1, x2, xr, yi;

   zarg1 = zargmt * em2pi3;
   x1    = real(zarg1);

   if (x1 < 0.0)
   {
      if (cabs(zarg1) > 4.0)
      {
         airy1 = gaussq(zarg1);
      }
      else
      {
         airy1 = powers(zarg1);
      }
   }
   else
   {
      if (cabs(zarg1) > 2.0)
      {
         airy1 = gaussq(zarg1);
      }
      else
      {
         airy1 = powers(zarg1);
      }
   }

   zarg2 = zargmt * ep2pi3;
   x2    = real(zarg2);

   if (x2 < 0.0)
   {
      if (cabs(zarg2) > 4.0)
      {
         airy2 = gaussq(zarg2);
      }
      else
      {
         airy2 = powers(zarg2);
      }
   }
   else
   {
      if (cabs(zarg2) > 2.0)
      {
         airy2 = gaussq(zarg2);
      }
      else
      {
         airy2 = powers(zarg2);
      }
   }

   exparg = cforth * zargmt * csqrt(zargmt);
   xr     = real(exparg);
   yi     = aimag(exparg);

   if (xr < -64.0)
   {
      xr = -64.0;
   }
   if (yi < -64.0)
   {
      yi = dmod(yi, twopi);
   }

   exparg = cmplx(xr, yi);

   zeta = cexp(exparg);
   return (ep1pi3 * airy1 * zeta + em1pi3 * airy2);
}

//---------------------------------------------------------------------
//     Purpose:
//     -------
//     Supports computation of the pattern propagation factor by the
//     SEKE routines.
//
//     Author:
//     ------
//     Science Applications International Corporation (SAIC)
//
//
//     Notice:
//     ------
//     This module was designed by Science Applications International
//     Corporation (SAIC) for the US Air Force, for use in the
//     Advanced Low Altitude Radar Model (ALARM), owned by Wright
//     Labs, Analysis and Evaluation Branch (WL/AAWA-1).
//
//     This module is derived from algorithms and code developed for
//     the Air Force by the Massachusetts Institute of Technology's
//     (MIT) Lincoln Laboratory.  Under the terms of the license
//     granted by MIT, the following restriction applies:
//
//     COPYRIGHT (C) 1985,1988 MIT LINCOLN LABORATORY ALL RIGHTS RESERVED
//
//---------------------------------------------------------------------

// static
WsfEM_ALARM_Propagation::COMPLEX WsfEM_ALARM_Propagation::powers(const COMPLEX& zargmt)
{
   // use propagation_consts

   // implicit none

   //-------------------------------------------------------------------
   // Passed parameters
   //-------------------------------------------------------------------

   // complex, intent(in) :: zargmt

   //-------------------------------------------------------------------
   // local variables
   //-------------------------------------------------------------------

   COMPLEX fsubnz, gsubnz, hsubnz, zcubed;

   int i, nterms;

   const double gcoeff[] = {0.0,      12.0E0,   42.0E0,   90.0E0,   156.0E0,  240.0E0,  342.0E0,  462.0E0,   600.0E0,
                            756.0E0,  930.0E0,  1122.0E0, 1332.0E0, 1560.0E0, 1806.0E0, 2070.0E0, 2352.0E0,  2652.0E0,
                            2970.0E0, 3306.0E0, 3660.0E0, 4032.0E0, 4422.0E0, 4830.0E0, 5256.0E0, 5700.0E0,  6162.0E0,
                            6642.0E0, 7140.0E0, 7656.0E0, 8190.0E0, 8742.0E0, 9312.0E0, 9900.0E0, 10506.0E0, 11130.0E0};

   const double hcoeff[] = {0.0,      6.0E0,    30.0E0,   72.0E0,   132.0E0,  210.0E0,  306.0E0,  420.0E0,   552.0E0,
                            702.0E0,  870.0E0,  1056.0E0, 1260.0E0, 1482.0E0, 1722.0E0, 1980.0E0, 2256.0E0,  2550.0E0,
                            2862.0E0, 3192.0E0, 3540.0E0, 3906.0E0, 4290.0E0, 4692.0E0, 5112.0E0, 5550.0E0,  6006.0E0,
                            6480.0E0, 6972.0E0, 7482.0E0, 8010.0E0, 8556.0E0, 9120.0E0, 9702.0E0, 10302.0E0, 10920.0E0};

   const double alpha = 0.355028053887817;
   const double beta  = 0.258819403792807;


   nterms = nint(7.0 + 4.0 * cabs(zargmt));

   zcubed = zargmt * zargmt * zargmt;

   hsubnz = cmplx(1.0, 0.0);
   gsubnz = zargmt;
   fsubnz = cmplx(alpha, 0.0) * hsubnz - cmplx(beta, 0.) * gsubnz;

   for (i = 1; i <= nterms; ++i)
   {
      hsubnz = hsubnz / cmplx(hcoeff[i], 0.0) * zcubed;
      gsubnz = gsubnz / cmplx(gcoeff[i], 0.0) * zcubed;
      fsubnz = fsubnz + cmplx(alpha, 0.0) * hsubnz - cmplx(beta, 0.0) * gsubnz;
   }

   return (fsubnz * cexp(ctwoth * zargmt * csqrt(zargmt)));
}

//---------------------------------------------------------------------
//     Purpose:
//     -------
//     Supports computation of the pattern propagation factor by the
//     SEKE routines.
//
//
//     Inputs:
//     ------
//     DELTAG  -  The ground range increment, in meters, between
//                points in the terrain profile between the radar
//                and the target.
//
//     ELVMSL  -  ELVMSL(I), I = 1,2,...,NPROFL, is the height above
//                mean sea level, in meters, of the I-th point in the
//                terrain profile between the radar and the target.
//
//     NPROFL  -  The number of points in the terrain profile
//                between the radar and the target.
//
//
//     Outputs:
//     -------
//     APARA0  -
//
//     APARA1  -
//
//     APARA2  -
//
//
//     Author:
//     ------
//     Science Applications International Corporation (SAIC)
//     2301 Yale Boulevard, SE, Suite E
//     Albuquerque, New Mexico 87106
//     (505) 766-7400
//
//
//     Notice:
//     ------
//     This module was designed by Science Applications International
//     Corporation (SAIC) for the US Air Force, for use in the
//     Advanced Low Altitude Radar Model (ALARM), owned by Wright
//     Labs, Analysis and Evaluation Branch (WL/AAWA-1).
//
//     This module is derived from algorithms and code developed for
//     the Air Force by the Massachusetts Institute of Technology's
//     (MIT) Lincoln Laboratory.  Under the terms of the license
//     granted by MIT, the following restriction applies:
//
//     COPYRIGHT (C) 1985,1988 MIT LINCOLN LABORATORY ALL RIGHTS RESERVED
//
//---------------------------------------------------------------------

// static
void WsfEM_ALARM_Propagation::parfit(double&                    apara0,
                                     double&                    apara1,
                                     double&                    apara2,
                                     double                     deltag,
                                     const std::vector<double>& elvmsl,
                                     int                        nprofl)
{
   // implicit none

   //-------------------------------------------------------------------
   // Passed parameters
   //-------------------------------------------------------------------

   // real(8), intent(out) :: apara0, apara1, apara2
   // real(8), intent(in)  :: deltag, elvmsl(*)

   // integer, intent(in)  :: nprofl

   //-------------------------------------------------------------------
   // local variables
   //-------------------------------------------------------------------

   double c0a0, c0a1, c2a0, c2a1, sumx, sumx2, sumx2y, sumx3, sumx4, sumxy, sumy, x, xforth, xsquar;

   int i;

   if (nprofl > 2)
   {
      //----------------------------------------------------------------
      //     Initialize quantities to zero.
      //----------------------------------------------------------------

      sumx   = 0.0;
      sumx2  = 0.0;
      sumx3  = 0.0;
      sumx4  = 0.0;
      sumx2y = 0.0;
      sumxy  = 0.0;
      sumy   = 0.0;

      for (i = 1; i <= nprofl; ++i)
      {
         x      = i * deltag;
         xsquar = x * x;
         xforth = xsquar * xsquar;
         sumx   = sumx + x;
         sumx2  = sumx2 + xsquar;
         sumx3  = sumx3 + x * xsquar;
         sumx4  = sumx4 + xforth;
         sumx2y = sumx2y + xsquar * elvmsl[i];
         sumxy  = sumxy + x * elvmsl[i];
         sumy   = sumy + elvmsl[i];
      }

      c0a1 = (sumxy - sumx * sumy / real(nprofl)) / (sumx2 - sumx * sumx / real(nprofl));

      c2a1 = (sumx * sumx2 / real(nprofl) - sumx3) / (sumx2 - sumx * sumx / real(nprofl));

      c0a0 = (sumy - sumx * c0a1) / real(nprofl);
      c2a0 = -(sumx * c2a1 + sumx2) / real(nprofl);

      apara2 = (sumx2y - sumx2 * c0a0 - sumx3 * c0a1) / (sumx2 * c2a0 + sumx3 * c2a1 + sumx4);

      apara1 = c0a1 + c2a1 * apara2;
      apara0 = c0a0 + c2a0 * apara2;
   }
   else if (nprofl > 1)
   {
      //----------------------------------------------------------------
      //     NPROFL, the number of points in the terrain profile, is equal
      //     to two.
      //----------------------------------------------------------------

      apara2 = 0.0;
      apara1 = (elvmsl[2] - elvmsl[1]) / deltag;
      apara0 = elvmsl[1] - apara1 * deltag;
   }
   else
   {
      apara2 = 0.0;
      apara1 = 0.0;
      apara0 = elvmsl[1];
   }
}

//---------------------------------------------------------------------
//     Purpose:
//     -------
//     Determines the knife edge diffraction component of the pattern
//     propagation factor as part of the SEKE routines.
//
//     Inputs:
//     ------
//     ALPHAT  -  The azimuth angle to the target, in radians.
//
//     DRATIO  -  DRATIO(I), I = 1,2,...,NPROFL, is the ratio of the
//                vertical distance from the I-th terrain point to
//                the line-of-sight between the radar and the target
//                to the Fresnel clearance at the I-th terrain point.
//
//     EPSLNT  -  The elevation angle to the target, in radians.
//
//     HAMMSL  -  The radar height above mean sea level.
//
//     ILEFT   -
//
//     IMAIN   -
//
//     INDXFC  -  The index of that point in the terrain profile
//                between the radar and the target which yields
//                the minimum value of the vector DRATIO; ie,
//                DRATIO(INDXFC) = min DRATIO(I), I = 1,2,...,NPROFL.
//
//     IRIGHT  -
//
//     RLAMDA  -  The radar wavelength, in meters.
//
//     XPROFL  -  XPROFL(I), I = 0,1,...,NPROFL, is the x-coordinate
//                (horizontal), in meters, of the I-th point in the
//                terrain profile between the radar and the target.
//                XPROFL(0), the x-coordinate of the radar, is equal
//                to zero.
//
//     XTPROF  -  The x-coordinate of the target in an x-z coordinate
//                system whose origin is located on the mean sea level
//                surface of the Earth at the site.  The positive
//                x-axis extends from the origin toward the target and
//                is tangent to the Earth at the radar.  Units are in
//                meters.
//
//     ZPROFL  -  ZPROFL(I), I = 0,1,...,NPROFL, is the z-coordinate,
//                (vertical), in meters, of the I-th point in the
//                terrain profile between the radar and the target
//                in meters.  ZPROFL(0) is the height of the terrain
//                at the radar.
//
//     ZTPROF  -  The z-coordinate of the target in an x-z coordinate
//                system whose origin is located on the mean sea level
//                surface of the Earth at the site.  (Positive z is up)
//                Units are in meters.
//
//     Outputs:
//     -------
//     FSUBK   -  The knife-edge diffraction loss.
//
//
//     Author:
//     ------
//     Science Applications International Corporation (SAIC)
//     2301 Yale Boulevard, SE, Suite E
//     Albuquerque, New Mexico 87106
//     (505) 766-7400
//
//
//     Notice:
//     ------
//     This module was designed by Science Applications International
//     Corporation (SAIC) for the US Air Force, for use in the
//     Advanced Low Altitude Radar Model (ALARM), owned by Wright
//     Labs, Analysis and Evaluation Branch (WL/AAWA-1).
//
//     This module is derived from algorithms and code developed for
//     the Air Force by the Massachusetts Institute of Technology's
//     (MIT) Lincoln Laboratory.  Under the terms of the license
//     granted by MIT, the following restriction applies:
//
//     COPYRIGHT (C) 1985,1988 MIT LINCOLN LABORATORY ALL RIGHTS RESERVED
//
//
//     Modified:
//     --------
//     Division 397
//     SAIC
//     1321 Research Park Drive
//     Dayton, OH 45432
//     (513) 429-6500
//
//     ALARM 3.0:
//     ---------
//     - DOPLER variable omitted from GEOMRT common block.
//       Variable DOPCSR, which includes the conical scan
//       rate, is now used in its place.  PJH & JCL
//
//     - Subroutine OFFBOR called to determine the off-
//       boresight angles in azimuth and elevation of an
//       arbitrary point in space with respect to the
//       radar antenna.  PJH & JCL
//
//
//     ALARM 3.2:
//     ---------
//     - TGAINS common block removed from subroutine TGAIN to support
//       stand alone transmit antenna.  Those variables (AZTMAX, AZTMIN,
//       AZTTAB, ELTMAX, ELTMIN, ELTTAB, ITXTYP, NUMAZT, NUMELT,TGTABL,
//       and TXGMIN) previously accessed through the common block are
//       now passed through the TGAIN argument list.  (SPCR-1056, JCL)
//
//     - Included TGAIN common block so that the AZTMAX, AZTMIN, ELTMAX,
//       ELTMIN, ITXTYP, NUMAZT, NUMELT, TGTABL and TXGMIN variables
//       could be passed to the TGAIN subroutine.  (SPCR-1056, JCL)
//
//     Modifications:
//     --------------
//     19-Dec-2003    - Modified to use the new antenna class.
//                      Removed unnecessary variables, ALPHAR, EPSLNR,
//                      OFFAZT from argument list.  See SPCR #1310.
//---------------------------------------------------------------------

// static
void WsfEM_ALARM_Propagation::deygou(antenna&                   ant_data,
                                     double                     alphat,
                                     const std::vector<double>& dratio,
                                     double                     epslnt,
                                     double&                    fsubk,
                                     double                     hammsl,
                                     int                        ileft,
                                     int                        imain,
                                     int                        indxfc,
                                     int                        iright,
                                     double                     rlamda,
                                     const std::vector<double>& xprofl,
                                     double                     xtprof,
                                     const std::vector<double>& zprofl,
                                     double                     ztprof)
{
   // use physical_consts, only : sqrt2
   // use propagation_consts

   // implicit none

   //-------------------------------------------------------------------
   // Passed parameters
   //-------------------------------------------------------------------

   // type (antenna), intent(in)  :: ant_data
   // real(8),        intent(in)  :: alphat         ! radians
   // real(8),        intent(in)  :: dratio(0:)
   // real(8),        intent(in)  :: epslnt         ! radians
   // real(8),        intent(in)  :: hammsl         ! meters
   // real(8),        intent(in)  :: rlamda         ! meters
   // real(8),        intent(in)  :: xprofl(0:)     ! meters
   // real(8),        intent(in)  :: xtprof         ! meters
   // real(8),        intent(in)  :: zprofl(0:)     ! meters
   // real(8),        intent(in)  :: ztprof         ! meters

   // real(8),        intent(out) :: fsubk

   // integer,        intent(in)  :: ileft
   // integer,        intent(in)  :: imain
   // integer,        intent(in)  :: indxfc
   // integer,        intent(in)  :: iright

   //-------------------------------------------------------------------
   // Local variables
   //-------------------------------------------------------------------

   double d1pd2, delta, deltam, delzro, dsub1, dsub2, epslnp, fcoswi, fleft, fmain, fright, fsinwi,
      /*NOT-USED gtilde,*/ hrayl, hrayr, ratiol, ratiom, ratior, /*ratmax,*/ slope, taneps, w;

   const double ratmax = 100.0;

   //-------------------------------------------------------------------
   // Initialize FMAIN, the diffraction loss contribution from the
   // main knife-edge, to one.  Initialize FLEFT, the diffraction
   // loss contribution from the knife-edge between the radar and
   // the main knife-edge, to one.  Initialize FRIGHT, the diffraction
   // loss contribution from the knife-edge between the main knife-
   // edge and the target, to one.
   //-------------------------------------------------------------------

   fmain  = 1.0;
   fleft  = 1.0;
   fright = 1.0;

   //-------------------------------------------------------------------
   // Initialize DELTAM, in meters, the clearance between the
   // terrain profile and the direct ray to the target at the main
   // knife-edge, to zero.  If the direct ray to the target is masked
   // by the main knife-edge, DELTAM will remain at zero.  Otherwise,
   // DELTAM will be determined later.  Set RATIOM, the ratio of the
   // clearance between the terrain profile and the direct ray to the
   // target to the Fresnel clearance at the main knife-edge, to the
   // value at the main knife-edge from the vector DRATIO.
   //-------------------------------------------------------------------

   deltam = 0.0;
   ratiom = dratio[imain];

   //-------------------------------------------------------------------
   // If the ratio of the clearance between the terrain profile and
   // the direct ray to the target to the Fresnel clearance at the
   // main knife-edge is greater than RATMAX, a user-specified
   // maximum ratio, there is assumed to be no diffraction loss
   // due to the main knife-edge.
   //-------------------------------------------------------------------

   if (ratiom < ratmax)
   {
      //----------------------------------------------------------------
      // Determine FMAIN, the diffraction loss contribution from the
      // main knife-edge.  Subroutine FRESNL determines FCOSWI, the
      // Fresnel cosine integral at W, and FSINWI, the Fresnel sine
      // integral at W.  See reference 2, pages 27 through 33 for more
      // information.
      //----------------------------------------------------------------

      w = sqrt2 * ratiom;
      /*call*/ fresnl(fcoswi, fsinwi, w);

      fmain = sqrt(pow((fcoswi + 0.5), 2) + pow((fsinwi + 0.5), 2)) / sqrt2;

      //----------------------------------------------------------------
      // If the ratio of the clearance between the terrain profile and
      // the direct ray to the target to the Fresnel clearance at the
      // main knife-edge is greater than zero (the direct ray from the
      // radar to the target is not masked by the main knife-edge),
      // determine a new value of DELTAM.
      //----------------------------------------------------------------

      if (ratiom > 0.0)
      {
         //-------------------------------------------------------------
         // Determine DSUB1, in meters, the distance from the radar to
         // the main knife-edge, and DSUB2, in meters, the distance from
         // the main knife-edge to the target.  Then determine DELZRO,
         // in meters, the Fresnel clearance at the main knife-edge, and
         // DELTAM, in meters, the clearance between the terrain profile
         // and the direct ray to the target at the main knife-edge.
         //-------------------------------------------------------------

         dsub1  = xprofl[imain];
         dsub2  = xtprof - dsub1;
         delzro = sqrt(rlamda * dsub1 * (dsub2 / xtprof));
         deltam = delzro * ratiom;
      }
   }

   //-------------------------------------------------------------------
   // Determine if a knife-edge was found between the radar and the
   // main knife-edge.
   //-------------------------------------------------------------------

   if (ileft != 0)
   {
      //----------------------------------------------------------------
      // There is a knife-edge between the radar and the main knife-edge.
      // Determine D1PD2, in meters, the distance from the radar to the
      // main knife-edge, DSUB1, in meters, the distance from the radar
      // to the knife-edge between the radar and the main knife-edge,
      // and DSUB2, in meters, the distance from the knife-edge between
      // the radar and the main knife-edge to the main knife-edge.
      //----------------------------------------------------------------

      d1pd2 = xprofl[imain];
      dsub1 = xprofl[ileft];
      dsub2 = d1pd2 - dsub1;

      //----------------------------------------------------------------
      // Determine SLOPE, the slope of the ray from the radar to the
      // main knife-edge (if DELTAM is equal to zero), or the slope of
      // the direct ray from the radar to the target (if DELTAM is not
      // equal to zero).  Then determine DELTA, in meters, the clearance
      // between the ray and the terrain profile at the knife-edge
      // between the radar and the main knife-edge, DELZRO, in meters,
      // the Fresnel clearance at this knife-edge, and RATIOL, the
      // ratio of the clearance between the terrain profile and this
      // ray to the Fresnel clearance at this knife-edge.
      //----------------------------------------------------------------

      hrayl  = hammsl;
      hrayr  = zprofl[imain] + deltam;
      slope  = (hrayr - hrayl) / d1pd2;
      delta  = hrayl + slope * dsub1 - zprofl[ileft];
      delzro = sqrt(rlamda * dsub1 * (dsub2 / d1pd2));
      ratiol = delta / delzro;

      //----------------------------------------------------------------
      // If the ratio of the clearance between the terrain profile and
      // the direct ray to the target to the Fresnel clearance at the
      // knife-edge between the radar and the main knife-edge is greater
      // than RATMAX, a user-specified maximum ratio, there is assumed
      // to be no diffraction loss due to the knife-edge between the
      // radar and the main knife-edge.
      //----------------------------------------------------------------

      if (ratiol < ratmax)
      {
         //-------------------------------------------------------------
         // Determine FLEFT, the diffraction loss contribution from the
         // knife-edge between the radar and the main knife-edge.
         // Subroutine FRESNL determines FCOSWI, the Fresnel cosine
         // integral at W, and FSINWI, the Fresnel sine integral at W.
         // See reference 2, pages 27 through 33 for more information.
         //-------------------------------------------------------------

         w = sqrt2 * ratiol;
         /*call*/ fresnl(fcoswi, fsinwi, w);

         fleft = sqrt(pow((fcoswi + 0.5), 2) + pow((fsinwi + 0.5), 2)) / sqrt2;
      }
   }

   //-------------------------------------------------------------------
   // Determine if a knife-edge was found between the main knife-edge
   // and the target.
   //-------------------------------------------------------------------

   if (iright != 0)
   {
      //----------------------------------------------------------------
      // There is a knife-edge between the main knife-edge and the
      // target.  Determine D1PD2, in meters, the distance from the
      // main knife-edge to the target, DSUB1, in meters, the distance
      // from the main knife-edge to the knife-edge between the main
      // knife-edge and the target, and DSUB2, in meters, the distance
      // from the knife-edge between the main knife-edge and the target
      // to the target.
      //----------------------------------------------------------------

      d1pd2 = xtprof - xprofl[imain];
      dsub1 = xprofl[iright] - xprofl[imain];
      dsub2 = d1pd2 - dsub1;

      //----------------------------------------------------------------
      // Determine SLOPE, the slope of the ray from the main knife-edge
      // to the target (if DELTAM is equal to zero), or the slope of the
      // direct ray from the radar to the target (if DELTAM is not equal
      // to zero).  Then determine DELTA, in meters, the clearance
      // between the ray and the terrain profile at the knife-edge
      // between the main knife-edge and the target, DELZRO, in meters,
      // the Fresnel clearance at this knife-edge, and RATIOR, the
      // ratio of the clearance between the terrain profile and this
      // ray to the Fresnel clearance at this knife-edge.
      //----------------------------------------------------------------

      hrayl  = zprofl[imain] + deltam;
      hrayr  = ztprof;
      slope  = (hrayr - hrayl) / d1pd2;
      delta  = hrayl + slope * dsub1 - zprofl[iright];
      delzro = sqrt(rlamda * dsub1 * (dsub2 / d1pd2));
      ratior = delta / delzro;

      //----------------------------------------------------------------
      // If the ratio of the clearance between the terrain profile and
      // the direct ray to the target to the Fresnel clearance at the
      // knife-edge between the main knife-edge and the target is greater
      // than RATMAX, a user-specified maximum ratio, there is assumed
      // to be no diffraction loss due to the knife-edge between the
      // main knife-edge and the target.
      //----------------------------------------------------------------

      if (ratior < ratmax)
      {
         //-------------------------------------------------------------
         // Determine FRIGHT, the diffraction loss contribution from the
         // knife-edge between the main knife-edge and the target.
         // Subroutine FRESNL determines FCOSWI, the Fresnel cosine
         // integral at W, and FSINWI, the Fresnel sine integral at W.
         // See reference 2, pages 27 through 33 for more information.
         //-------------------------------------------------------------

         w = sqrt2 * ratior;
         /*call*/ fresnl(fcoswi, fsinwi, w);

         fright = sqrt(pow((fcoswi + 0.5), 2) + pow((fsinwi + 0.5), 2)) / sqrt2;
      }
   }

   if (dratio[indxfc] > 0.0)
   {
      epslnp = epslnt;
   }
   else
   {
      taneps = (zprofl[indxfc] - hammsl) / xprofl[indxfc];
      epslnp = atan(taneps);
   }

   /*call*/ // get_relative_gain(ant_data, alphat, epslnp, gtilde);

   //-------------------------------------------------------------------
   // Determine FSUBK, the knife-edge diffraction loss.
   //-------------------------------------------------------------------
   // F4 propagation factor should not be normalized by antenna gain (CR-106)

   if (!mUnitTestPropagation)
   {
      fsubk = /*sqrt(gtilde) **/ fleft * fmain * fright;
   }
   else
   {
      double gtilde = 1.0;
      /*call*/ get_relative_gain(ant_data, alphat, epslnp, gtilde);
      fsubk = sqrt(gtilde) * fleft * fmain * fright;
   }
}

//---------------------------------------------------------------------
//     Purpose:
//     -------
//     Determines the Fresnel clearance of the radar beam from the
//     terrain in support of determining the pattern propagation factor
//     as part of the SEKE routines.
//
//     Author:
//     ------
//     Science Applications International Corporation (SAIC)
//
//
//     Notice:
//     ------
//     This module was designed by Science Applications International
//     Corporation (SAIC) for the US Air Force, for use in the
//     Advanced Low Altitude Radar Model (ALARM), owned by Wright
//     Labs, Analysis and Evaluation Branch (WL/AAWA-1).
//
//     This module is derived from algorithms and code developed for
//     the Air Force by the Massachusetts Institute of Technology's
//     (MIT) Lincoln Laboratory.  Under the terms of the license
//     granted by MIT, the following restriction applies:
//
//     COPYRIGHT (C) 1985,1988 MIT LINCOLN LABORATORY ALL RIGHTS RESERVED
//
//---------------------------------------------------------------------

// static
void WsfEM_ALARM_Propagation::fresnl(double& cosint, double& sinint, double xargmt)
{
   // use propagation_consts
   // use physical_consts, only : halfpi

   // implicit none


   //-------------------------------------------------------------------
   // Passed parameters
   //-------------------------------------------------------------------

   // real(8), intent(out) :: cosint, sinint
   // real(8), intent(in)  :: xargmt

   //-------------------------------------------------------------------
   // local variables
   //-------------------------------------------------------------------

   double asub1z, asub2z, asub3z, asub4z, constx, consxi, dcosz, dsinz, fourzi, z, zforth;

   int i;

   // Element added for index 0 to enable no change to code that used 1-based indexing

   const double a1sub[] = {0.0,
                           0.3440477900E-01,
                           -0.1502309600E+00,
                           -0.2563904100E-01,
                           0.8506637810E+00,
                           -0.7575241900E-01,
                           -0.3050485660E+01,
                           -0.1689865700E-01,
                           0.6920691902E+01,
                           -0.5763610000E-03,
                           -0.6808568854E+01,
                           -0.1702000000E-05,
                           0.1595769140E+01};

   const double a2sub[] = {0.0,
                           0.1954703100E-01,
                           -0.2161959290E+00,
                           0.7022220160E+00,
                           -0.4033492760E+00,
                           -0.1363729124E+01,
                           -0.1383419470E+00,
                           0.5075161298E+01,
                           -0.9520895000E-02,
                           -0.7780020400E+01,
                           -0.9281000000E-04,
                           0.4255387524E+01,
                           -0.3300000000E-07};

   const double a3sub[] = {0.0,
                           0.2339390000E-03,
                           -0.1217930000E-02,
                           0.2102967000E-02,
                           0.2464200000E-03,
                           -0.6748873000E-02,
                           0.1194880900E-01,
                           -0.9497136000E-02,
                           0.6898920000E-03,
                           0.5770956000E-02,
                           0.3936000000E-05,
                           -0.2493395700E-01,
                           0.0000000000E+00};

   const double a4sub[] = {0.0,
                           0.8383860000E-03,
                           -0.5598515000E-02,
                           0.1649730800E-01,
                           -0.2792895500E-01,
                           0.2906406700E-01,
                           -0.1712291400E-01,
                           0.1903218000E-02,
                           0.4851460000E-02,
                           0.2300600000E-04,
                           -0.9351341000E-02,
                           0.2300000000E-07,
                           0.1994711400E+00};

   //-------------------------------------------------------------------
   // Determine some intermediate functions of XARGMT.
   //-------------------------------------------------------------------

   z     = halfpi * xargmt * xargmt;
   dsinz = sin(z);
   dcosz = cos(z);

   //-------------------------------------------------------------------
   // The magnitude of Z will determine whether the Fresnel sine and
   // cosine integrals are to be approximated by positive or negative
   // exponential power series.
   //-------------------------------------------------------------------

   if (z <= 4.0)
   {
      //----------------------------------------------------------------
      // The Fresnel sine and cosine integrals are to be approximated by
      // positive exponential power series.  Determine some inter-
      // mediate quantities.
      //----------------------------------------------------------------

      zforth = 0.25 * z;
      constx = sr2po4 * xargmt;

      //----------------------------------------------------------------
      // Determine the power series used in the approximation for the
      // Fresnel sine and cosine integrals.  The constant coefficients of
      // the power series are taken from the DATA statements in the upper
      // portion of the subroutine.
      //----------------------------------------------------------------

      asub1z = a1sub[1];
      asub2z = a2sub[1];

      for (i = 2; i <= 12; ++i)
      {
         asub1z = a1sub[i] + zforth * asub1z;
         asub2z = a2sub[i] + zforth * asub2z;
      }

      //----------------------------------------------------------------
      // Determine the Fresnel sine and cosine integrals.
      //----------------------------------------------------------------

      sinint = constx * (asub1z * dsinz - asub2z * dcosz);
      cosint = constx * (asub1z * dcosz + asub2z * dsinz);
   }
   else
   {
      //----------------------------------------------------------------
      // The Fresnel sine and cosine integrals are to be approximated by
      // negative exponential power series.  Determine some inter-
      // mediate quantities.
      //----------------------------------------------------------------

      fourzi = 4.0 / z;
      consxi = 1.59576912160570 / fabs(xargmt);

      //----------------------------------------------------------------
      // Determine the power series used in the approximation for the
      // Fresnel sine and cosine integrals.
      //----------------------------------------------------------------

      asub3z = a3sub[1];
      asub4z = a4sub[1];

      for (i = 2; i <= 12; ++i)
      {
         asub3z = a3sub[i] + fourzi * asub3z;
         asub4z = a4sub[i] + fourzi * asub4z;
      }

      //----------------------------------------------------------------
      // Determine the Fresnel sine and cosine integrals.
      //----------------------------------------------------------------

      sinint = 0.5 + consxi * (asub3z * dsinz - asub4z * dcosz);
      cosint = 0.5 + consxi * (asub3z * dcosz + asub4z * dsinz);
      sinint = UtMath::Sign(sinint, xargmt);
      cosint = UtMath::Sign(cosint, xargmt);
   }
}

// =================================================================================================
// Code from ALARM 'multipath.f90'
// =================================================================================================

//---------------------------------------------------------------------
//     Purpose:
//     -------
//     Determines the multipath component of the pattern propagation
//     factor.
//
//
//
//     Inputs:
//     ------
//     ALPHAT  -  The azimuth angle to the target, in radians.
//
//     EPSLNT  -  The elevation angle to the target, in radians.
//
//     EPSLN1  -  The dielectric constant of the terrain, land or sea.
//                The mean value is about 10 for land, and about 80
//                for sea.  In tests conducted by MIT/LL over land,
//                a value of 6.0 was used.  (EPSLN1 >= 0.0)
//
//                Ground Type  |  Dielectric Constant
//                -------------|---------------------
//                Very Dry     |          2
//                Very Wet     |          24
//
//
//                Sea Temperature (degrees C)  |  Dielectric Constant
//                -----------------------------|---------------------
//                             10              |        72.2
//                             20              |        69.1
//
//     HAMMSL  -  The radar height above mean sea level.
//
//     IEND    -  The vector of ending point indices for the visible
//                areas in the terrain profile.
//
//     IPOLAR  -  The radar antenna polarization indicator.  If
//                IPOLAR = 0, the antenna is vertically polarized.
//
//                If IPOLAR = 1,  the antenna is horizontally
//                polarized.
//
//     ISTART  -  The vector of starting point indices for the visible
//                areas in the terrain profile.
//
//     WATER_COVER  -  flag to indicate propagation over water
//
//     NAREAS  -  The number of visible terrain areas along the
//                terrain profile.
//
//     NPROFL  -  The number of points in the terrain profile
//                between the radar and the target.
//
//     PULWID  -  Radar pulse width, in microseconds.
//
//     RANGET  -  The range from the radar antenna to the target,
//                in meters.
//
//     RLAMDA  -  The radar wavelength, in meters.
//
//     RROUGH_I -  The terrain roughness factor or the constant
//                 portion of the roughness coefficient when the land
//                 cover is water.
//
//     SIGMHO  -  The terrain conductivity in mhos/m, land or sea.
//                SIGMHO varies from approximately 10**-4 to several
//                units of 10**-3.  A mean value is about 10**-3 for
//                land, and about 4 for sea.  In tests conducted by
//                MIT/LL over land, a value of 0.006 was used.
//                (SIGMHO >= 0.0)
//
//                Sea Temperature (degrees C)  |  Conductivity (mhos/m)
//                -----------------------------|-----------------------
//                              10             |          4.0
//                              20             |          5.2
//
//     TANEPP  -  TANEPP(I), I = 1,2,...,NPROFL, is the tangent of
//                the elevation angle from the radar to the I-th
//                point in the terrain profile between the radar
//                and the target.
//
//     TAURLX  -  Relaxation Constant (times 1 / 10**-12), for use
//                water cover only.  Used to determine
//                RROUGH for the pattern propagation factor over the
//                sea (TAURLX >= 0.0)
//
//                Sea Temperature (degrees C)  |  Relaxation Constant
//                -----------------------------|---------------------
//                             10              |      12.1 * 10**-12
//                             20              |       9.2 * 10**-12
//
//     ANT_DATA -  Antenna data structure.
//
//     VISIBL  -  VISIBL(I), I = 0,1,...,NPROFL, logical indicating
//                if the I-th point in the terrain profile is
//                visible from the radar.
//
//     WNDKNO  -  Wind speed in knots, for use when water cover
//                only.  Used to determine RROUGH for the pattern
//                propagation factor over the sea.  (WNDKNO >= 0.0)
//
//     XPROFL  -  XPROFL(I), I = 0,1,...,NPROFL, is the x-coordinate
//                (horizontal), in meters, of the I-th point in the
//                terrain profile between the radar and the target.
//                XPROFL(0), the x-coordinate of the radar, is equal
//                to zero.
//
//     XTPROF  -  The x-coordinate of the target in an x-z coordinate
//                system whose origin is located on the mean sea level
//                surface of the Earth at the site.  The positive
//                x-axis extends from the origin toward the target and
//                is tangent to the Earth at the radar.  Units are in
//                meters.
//
//     ZPROFL  -  ZPROFL(I), I = 0,1,...,NPROFL, is the z-coordinate,
//                (vertical), in meters, of the I-th point in the
//                terrain profile between the radar and the target
//                in meters.  ZPROFL(0) is the height of the terrain
//                at the radar.
//
//     ZTPROF  -  The z-coordinate of the target in an x-z coordinate
//                system whose origin is located on the mean sea level
//                surface of the Earth at the site.  (Positive z is up)
//                Units are in meters.
//
//     ELVMSL   -  Terrain height above mean sea level.
//
//     USE_SURFACE_HEIGHT - Flag indication if standard deviation of
//                         surface height should be used.
//
//     SURFACE_HEIGHT - The standard deviation of surface height in
//                      meters.
//
//     SOIL_MOISTURE - The soil moisture content percentage.
//
//     WATER_TEMP  - Water temperature in degrees C.
//
//     SEA_WATER  - Flag indicating if the water cover is sea water.
//
//     Outputs:
//     -------
//     FSUBM   -  The multipath pattern propagation factor.
//
//
//     Author:
//     ------
//     Science Applications International Corporation (SAIC)
//     2301 Yale Boulevard, SE, Suite E
//     Albuquerque, New Mexico 87106
//     (505) 766-7400
//
//
//     Modified:
//     --------
//     Division 397
//     SAIC
//     1321 Research Park Drive
//     Dayton, OH 45432
//     (513) 429-6500
//
//     ALARM 3.0:
//     ---------
//     - DOPLER variable omitted from GEOMRT common block.
//       Variable DOPCSR, which includes the conical scan
//       rate, is now used in its place.  JCL
//
//     - Subroutine OFFBOR called to determine the off-
//       boresight angles in azimuth and elevation of an
//       arbitrary point in space with respect to the
//       radar antenna.  JCL
//
//     - Check for overlap of the direct and reflected
//       signal returns.  JCL
//
//     - The roughness coefficient is now a function of
//       the grazing angle when the land cover is water
//       (LCOVER = 7).  JCL
//
//
//     ALARM 3.2:
//     ---------
//     - TGAINS common block removed from subroutine TGAIN to support
//       stand alone transmit antenna.  Those variables (AZTMAX, AZTMIN,
//       AZTTAB, ELTMAX, ELTMIN, ELTTAB, ITXTYP, NUMAZT, NUMELT,TGTABL,
//       and TXGMIN) previously accessed through the common block are
//       now passed through the TGAINS argument list.  (SPCR-1056, JCL)
//
//     - Removed the following common blocks to support stand alone
//       multipath; CONSTR, ENVIRO, GEOMRT, LOGICL, PRFILE, RADPAR,
//       RFLCOM, SITCOM, TGAINS.  Variables previously accessed through
//       through these common blocks (ALPHAR, ALPHAT, AZTMAX, AZTMIN,
//       AZTTAB, ELTMAX, ELTMIN, ELTTAB, EPSLNR, EPSLNT, EPSLN1, HAMMSL,
//       IEND, IPOLAR, ISTART, ITXTYP, LCOVER, NAREAS, NPROFL, NUMAZT,
//       NUMELT, OFFAZT, OFFELT, PULWID, RANGET, RLAMDA, RROUGH, SIGMHO,
//       TANEPP, TAURLX, TGTABL, TXGMIN, VISIBL, WNDKNO, XPROFL, XTPROF,
//       ZPROFL, ZTPROF) are now passed through the MLTPTH argument list.
//       (SPCR-1058, JCL)
//
//     - Constants TWOPI and VLIGHT are calculated locally rather than
//       referenced through the CONSTR common block.  (SPCR-1058, JCL)
//
//     - COMPLX common block removed from RFLECT subroutine to support
//       stand alone multipath.  The YSQUAR variable previously accessed
//       through the COMPLX common block is passed through the RFLECT
//       argument list.  Therefore, the variables necessary to compute
//       YSQUAR (EPSLN1, SIGMHO, TAURLX, WNDKNO)are now passed through
//       the MLTPTH argument list.  (SPCR-1058, JCL)
//
//    -  Changed the sign of ARGMNT from positive to negative to denote
//       the phase lag of the reflected wave with respect to the
//       incident wave.  Reference: "Radar Propagation at Low Altitudes",
//       M.L. Meeks, Artech House, Inc. Copyright 1982.  (SPCR-1001, JCL)
//
//    -  Corrected PSLOPE comparison (changed if statement from:
//       IF (PSLOPE .GT. 0.0D0) THEN to: IF (PSLOPE .GE. 0.0D0) THEN)
//       to prevent a divide by zero error which occurred when the slope
//       of the terrain between the radar and the first point in the
//       terrain profile was zero.  (SPCR-1048, JCL)
//
//    -  Added code, which was previously located in subroutine SEKINT,
//       to initialize the normalized admittance of the medium (YSQUAR)
//       and the roughness coefficient (RROUGH) when the land cover
//       is water (LCOVER=7).  (SPCR-1058, JCL)
//
//    -  Added environment parameters:  EPSLN1, SIGMHO, TAURLX, and
//       WNDKNO to the formal arguments for the initialization of the
//       square of the normalized admittance of the medium (YSQUAR).
//       (SPCR-1058, JCL)
//
//    -  Corrected the conversion factor for EPSLNI conversion from
//       mho/m to esu.  The correct factor is
//       ((VLIGHT * 1.0E-8)**2 * 1.0E+9).  (SPCR-1047, JCL)
//
//    - Removed the check for rrough != 0.  This makes it possible to
//      have water cover with a wind speed of zero.  It also makes it
//      possible to have a roughness factor of zero over land.
//      See SPCR #1075. MWS
//
//
//     ALARM 4.x:
//     ---------
//     - Multipath pattern propagation factor type changed from real
//       to complex.  JCL
//
//     22-Dec-2000  Added support for the standard deviation of surface
//                  height algorithm. See SPCR 1204. MWS
//
//     19-Dec-2003   - Modified to use the new antenna class.
//                     Removed unnecessary variables, ALPHAR, EPSLNR,
//                     OFFAZT, OFFELT from argument list.
//                     See SPCR #1310.
//---------------------------------------------------------------------

void WsfEM_ALARM_Propagation::mltpth(antenna&                   ant_data,
                                     double                     alphat,
                                     double                     epslnt,
                                     double                     epsln1,
                                     COMPLEX&                   fsubm,
                                     double                     hammsl,
                                     std::vector<int>&          iend,
                                     int                        ipolar,
                                     std::vector<int>&          istart,
                                     bool                       water_cover,
                                     int&                       nareas,
                                     int                        nprofl,
                                     double                     pulwid,
                                     double                     ranget,
                                     double                     rlamda,
                                     double                     rrough_i,
                                     double                     sigmho,
                                     const std::vector<double>& tanepp,
                                     double                     taurlx,
                                     std::vector<bool>&         visibl,
                                     double                     wndkno,
                                     const std::vector<double>& xprofl,
                                     double                     xtprof,
                                     const std::vector<double>& zprofl,
                                     double                     ztprof,
                                     const std::vector<double>& elvmsl,
                                     bool                       use_surface_height,
                                     double                     surface_height,
                                     double                     soil_moisture,
                                     double                     water_temp,
                                     bool                       sea_water)
{
   // use terrain_class, only : get_terrain_sw

   // use physical_consts, only : vlight, twopi, pi

   // implicit none

   //-------------------------------------------------------------------
   // Passed parameters
   //-------------------------------------------------------------------

   // type (antenna), intent(in)    :: ant_data
   // real(8),        intent(in)    :: alphat             ! radians
   // real(8),        intent(in)    :: epsln1
   // real(8),        intent(in)    :: epslnt
   // complex,        intent(out)   :: fsubm
   // real(8),        intent(in)    :: hammsl             ! meters
   // real(8),        intent(in)    :: pulwid             ! usec
   // real(8),        intent(in)    :: ranget             ! meters
   // real(8),        intent(in)    :: rlamda             ! meters
   // real(8),        intent(in)    :: rrough_i
   // real(8),        intent(in)    :: sigmho
   // real(8),        intent(in)    :: tanepp(*)
   // real(8),        intent(in)    :: taurlx
   // real(8),        intent(in)    :: wndkno             ! knots
   // real(8),        intent(in)    :: xprofl(0:)
   // real(8),        intent(in)    :: xtprof
   // real(8),        intent(in)    :: zprofl(0:)
   // real(8),        intent(in)    :: ztprof
   // real(8),        intent(in)    :: elvmsl(*)          ! meters

   // integer,        intent(inout) :: iend(*)
   // integer,        intent(in)    :: ipolar
   // integer,        intent(inout) :: istart(*)
   // integer,        intent(inout) :: nareas
   // integer,        intent(in)    :: nprofl

   // logical,        intent(inout) :: visibl(*)
   // logical,        intent(in)    :: water_cover

   // logical,        intent(in)    :: use_surface_height
   // real(8),        intent(in)    :: surface_height     ! stddev meters
   // real(8),        intent(in)    :: soil_moisture      ! %
   // real(8),        intent(in)    :: water_temp         ! deg C
   // logical,        intent(in)    :: sea_water


   //-------------------------------------------------------------------
   // Passed parameters
   //-------------------------------------------------------------------

   // type (antenna), intent(in)    :: ant_data
   // real(8),        intent(in)    :: alphat             // radians
   // real(8),        intent(in)    :: epsln1
   // real(8),        intent(in)    :: epslnt
   // complex,        intent(out)   :: fsubm
   // real(8),        intent(in)    :: hammsl             // meters
   // real(8),        intent(in)    :: pulwid             // usec
   // real(8),        intent(in)    :: ranget             // meters
   // real(8),        intent(in)    :: rlamda             // meters
   // real(8),        intent(in)    :: rrough_i
   // real(8),        intent(in)    :: sigmho
   // real(8),        intent(in)    :: tanepp(*)
   // real(8),        intent(in)    :: taurlx
   // real(8),        intent(in)    :: wndkno             // knots
   // real(8),        intent(in)    :: xprofl(0:)
   // real(8),        intent(in)    :: xtprof
   // real(8),        intent(in)    :: zprofl(0:)
   // real(8),        intent(in)    :: ztprof
   // real(8),        intent(in)    :: elvmsl(*)          // meters

   // integer,        intent(inout) :: iend(*)
   // integer,        intent(in)    :: ipolar
   // integer,        intent(inout) :: istart(*)
   // integer,        intent(inout) :: nareas
   // integer,        intent(in)    :: nprofl

   // logical,        intent(inout) :: visibl(*)
   // logical,        intent(in)    :: water_cover

   // logical,        intent(in)    :: use_surface_height
   // real(8),        intent(in)    :: surface_height     // stddev meters
   // real(8),        intent(in)    :: soil_moisture      // %
   // real(8),        intent(in)    :: water_temp         // deg C
   // logical,        intent(in)    :: sea_water


   //-------------------------------------------------------------------
   // local variables
   //-------------------------------------------------------------------
   COMPLEX argmnt, asubs, crflct, rcoeff, ysquar;

   double aminus, bcoeff /*NOT-USED , cabfsm*/, ccoeff, cosgam, cterm, delmax, deltar, deltx1, deltx2, deltz1, deltz2,
      epsln, epsln0, epslni, epslnp, freqhz, fznmax, gamma, gammax, /*NOT-USED gbelod,
                     gbelos,*/
      hsub1, hsub2 /*NOT-USED , offelp*/, pslope, pulsew, radicl, range1, range2, rdelta, rngref, rsmall, sigwav,
      singam, slopel, sloper, slptgl, slptgr, sumwid, tanepi, tangam, tangmx, tantrg, windsp, x, zonwid, rrough;

   int i, ifzmax, ip1, j;

   bool ovrlap, spculr, vislst, terrain_sw;

   // Eliminate g++ compile warnings
   rrough = 0.0;
   ovrlap = false;
   singam = 0.0;
   gammax = 0.0;
   delmax = 0.0;
   fznmax = 0.0;
   gamma  = 0.0;

   //-------------------------------------------------------------------
   // Initialize the PULSEW, the transmitted pulse width in meters.
   //-------------------------------------------------------------------

   freqhz     = vlight / rlamda;
   pulsew     = vlight * pulwid * 1.0E-6 / 2.0E0;
   sumwid     = 0.0;
   terrain_sw = WsfEM_ALARM_Terrain::get_terrain_sw();

   if (use_surface_height)
   {
      //----------------------------------------------------------------
      // Get the complex dielectric constant
      //----------------------------------------------------------------
      if (water_cover)
      {
         ysquar = water_dielectric(freqhz, sea_water, water_temp);
      }
      else
      {
         ysquar = soil_dielectric(freqhz, soil_moisture);
      }
   }
   else if (water_cover)
   {
      //----------------------------------------------------------------
      // The land cover is water, determine the complex dielectric
      // constant for propagation over water.  The terrain conductivity,
      // SIGMHO, is in units of mhos/m but is used in this equation in
      // units of esu.  esu = mhos/m * ((VLIGHT * 1.0E-8)**2 * 1.0E+9)
      //----------------------------------------------------------------

      freqhz = vlight / rlamda;
      epsln0 = 4.9E0;
      x      = twopi * freqhz * taurlx * 1.0E-12;
      epsln  = ((epsln1 - epsln0) / (1.0E0 + pow(x, 2))) + epsln0;
      epslni =
         ((epsln1 - epsln0) * x) / (1.0E0 + pow(x, 2)) + 2.0E0 * sigmho * (pow((vlight * 1.0E-8), 2) * 1.0E9) / freqhz;
      ysquar = cmplx(real(epsln), real(-epslni));

      //----------------------------------------------------------------
      // For propagation over water the roughness coefficient is a
      // function of the grazing angle which will be determined in
      // subroutine MLTPTH.  Initialize the constant portion of the
      // roughness coefficient based on the input wind speed in knots.
      //----------------------------------------------------------------

      windsp = wndkno * (1852.0E0 / 3600.0E0);
      sigwav = 0.25E0 * pow((windsp / 8.67E0), 2.5E0);
      rrough = -2.0E0 * pow((twopi * sigwav / rlamda), 2);
   }
   else
   {
      //----------------------------------------------------------------
      // The land cover is land, determine the complex dielectric
      // constant for propagation over land.
      //----------------------------------------------------------------

      ysquar = cmplx(real(epsln1), real(-60.0E0 * rlamda * sigmho));
      rrough = rrough_i;
   }

   //--------------------------------------------------------------------
   // Set TANGMX, the tangent of the maximum elevation angle from the
   // target to a point in the terrain profile, such that the initial
   // maximum elevation angle is approximately minus ninety degrees.
   //--------------------------------------------------------------------

   tangmx = -1.0E+32;

   //--------------------------------------------------------------------
   // Starting at the target and proceeding toward the radar,
   // determine which points along the terrain profile are visible
   // from the target.  This information, together with the
   // information determined in a previous call to subroutine PROFIL,
   // determines which points along the terrain profile are visible
   // from both the radar and the target.
   //--------------------------------------------------------------------

   for (i = nprofl; i >= 1; --i)
   {
      //-----------------------------------------------------------------
      // Determine TANEPI, the tangent of the elevation angle from
      // the target to the I-th point in the terrain profile.
      //-----------------------------------------------------------------

      tanepi = (zprofl[i] - ztprof) / (xtprof - xprofl[i]);

      if (tanepi > tangmx)
      {
         //--------------------------------------------------------------
         // The elevation angle from the target to the I-th point in
         // the terrain profile is greater than the current maximum
         // elevation angle.  Set TANGMX, the tangent of the maximum
         // elevation angle, to TANEPI, the tangent of the elevation
         // angle from the target to the I-th point in the terrain
         // profile.  VISIBL(I) should maintain its current value.
         // If the I-th point in the terrain profile was visible from
         // the radar, VISIBL(I) = .TRUE. now indicates the I-th
         // point in the terrain profile is visible from both the the
         // radar and the target.  VISIBL(I) = .FALSE. now indicates
         // that the I-th point in the terrain profile is not visible
         // from both the radar and the target.
         //--------------------------------------------------------------

         tangmx = tanepi;
      }
      else
      {
         //--------------------------------------------------------------
         // The I-th point in the terrain profile is not visible from
         // the target.  Set VISIBL(I) to .FALSE. to indicate that
         // the I-th point in the terrain profile is not visible from
         // both the radar and the target.
         //--------------------------------------------------------------

         visibl[i] = false;
      }

   } // for (i

   //--------------------------------------------------------------------
   // A visible area along the terrain profile is a uninterrupted
   // sequence of points, each of which is visible to both the radar
   // and the target.  Initialize NAREAS, the number of such visible
   // areas along the terrain profile, to zero.  Initialize VISLST to
   // .FALSE. to indicate that the previous point in the terrain
   // profile was not visible from both the radar and the target.
   //--------------------------------------------------------------------

   nareas = 0;
   vislst = false;

   //--------------------------------------------------------------------
   // Determine NAREAS, the number of visible areas along the terrain
   // profile.  Also determine ISTART, the vector of starting point
   // indices for the visible areas, and IEND, the vector of ending
   // point indices for the visible areas.
   //--------------------------------------------------------------------

   for (i = 1; i <= nprofl; ++i)
   {
      if (visibl[i])
      {
         //--------------------------------------------------------------
         // The I-th point in the terrain profile is visible from
         // both the radar and the target.
         //--------------------------------------------------------------

         if (!vislst)
         {
            //-----------------------------------------------------------
            // The I-th point in the terrain profile is visible from
            // both the radar and the target, but the previous point
            // in the terrain profile is not visible from both the
            // radar and the target.  Therefore, the I-th point in
            // the terrain profile represents the starting point of a
            // new visible area.  Increment NAREAS, the number of
            // visible areas, and store the index I in the starting
            // point vector ISTART.
            //-----------------------------------------------------------

            nareas         = nareas + 1;
            istart[nareas] = i;
         }
      }
      else
      {
         //--------------------------------------------------------------
         // The I-th point in the terrain profile is not visible from
         // both the radar and the target.
         //--------------------------------------------------------------

         if (vislst)
         {
            //-----------------------------------------------------------
            // The I-th point in the terrain profile is not visible
            // from both the radar and the target, but the previous
            // point in the terrain profile is visible from both the
            // radar and the target.  There- fore, the previous point
            // in the terrain profile is the last point in a visible
            // area.  Store the index of the previous point in the
            // ending point vector IEND.
            //-----------------------------------------------------------

            iend[nareas] = i - 1;
         }
      }

      //-----------------------------------------------------------------
      // Update VISLST to VISIBL(I), the visibility status of the
      // I-th point in the terrain profile.
      //-----------------------------------------------------------------

      vislst = visibl[i];

   } // for (i

   //--------------------------------------------------------------------
   // If the final point (the NPROFL-th point) in the terrain profile
   // is visible from both the radar and the target, the ending point
   // vector IEND will not have been determined for the final visible
   // area.  If this is the case, determine IEND(NAREAS), the ending
   // point for the final visible area.
   //--------------------------------------------------------------------

   if (vislst)
   {
      iend[nareas] = nprofl;
   }

   //--------------------------------------------------------------------
   // Initialize CRFLCT, the portion of the pattern propagation
   // factor due to reflected rays from the terrain, to zero.
   // Initialize SUMWID, the sum of the largest first Fresnel zone
   // extents from each visible area, to zero.
   //--------------------------------------------------------------------

   crflct = cmplx(0.0);
   sumwid = 0.0;

   for (j = 1; j <= nareas; ++j)
   {
      //-----------------------------------------------------------------
      // For each visible area which contains specular reflection
      // points, find the index of the specular reflection point
      // which has the largest first Fresnel zone extent.  Initialize
      // IFZMAX, the index of that specular reflection point, to
      // zero.  Initialize FZNMAX, the first Fresnel zone extent of
      // that specular reflection point, to zero.
      //-----------------------------------------------------------------

      ifzmax = 0;
      fznmax = 0.0;

      ip1 = istart[j];
      i   = ip1 - 1;

      pslope = (zprofl[ip1] - zprofl[i]) / (xprofl[ip1] - xprofl[i]);
      tangam = (pslope - tanepp[ip1]) / (1.0 + pslope * tanepp[ip1]);
      sloper = (pslope + tangam) / (1.0 - pslope * tangam);
      slptgl = (ztprof - zprofl[ip1]) / (xtprof - xprofl[ip1]);

      //-----------------------------------------------------------------
      // For the J-th visible area, find the specular reflection points.
      //-----------------------------------------------------------------

      for (i = istart[j]; i <= iend[j] - 1; ++i)
      {
         ip1 = i + 1;

         //--------------------------------------------------------------
         // Determine PSLOPE, the slope of the terrain between the
         // I-th and I+1 th points in the terrain profile.  Determine
         // TANGAM, the tangent of the angle between the ray
         // connecting the radar to the I-th terrain point and the
         // line between the I-th terrain point and I+1 th terrain
         // point.  Determine SLOPEL, the slope of the ray connecting
         // the radar and the I-th terrain point after it is
         // reflected from the I-th terrain point.  Determine SLPTGR,
         // the slope of the line connecting the I+1 th terrain point
         // and the target.
         //--------------------------------------------------------------

         pslope = (zprofl[ip1] - zprofl[i]) / (xprofl[ip1] - xprofl[i]);
         tangam = (pslope - tanepp[i]) / (1.0 + pslope * tanepp[i]);
         slopel = (pslope + tangam) / (1.0 - pslope * tangam);
         slptgr = (ztprof - zprofl[ip1]) / (xtprof - xprofl[ip1]);

         //--------------------------------------------------------------
         // SLOPEL is the slope of the reflected ray at the I-th
         // terrain point assuming that the terrain slope at the I-th
         // terrain point is given by the slope of the line
         // connecting the I-1 th and the I-th terrain points.
         // SLOPER is the slope of the reflected ray at the I-th
         // terrain point assuming that the terrain slope at the I-th
         // terrain point is given by the slope of the line
         // connecting the I-th and the I+1 th terrain points.
         // SLPTGL is the slope of the line connecting the I-th
         // terrain point and the target.  Set logical variable
         // SPCULR to .FALSE. to indicate that the I-th terrain point
         // has not yet been determined to be a specular reflection
         // point.
         //--------------------------------------------------------------

         spculr = false;

         if ((slopel > slptgl && sloper < slptgl) || (slopel < slptgl && sloper > slptgl))
         {
            //-----------------------------------------------------------
            // The terrain slopes on either side of the I-th terrain
            // point are such that the reflected rays described above
            // lie on opposite sides of the target.  In this case,
            // the I-th terrain point is assumed to be a specular
            // reflection point.  Determine GAMMA, in radians, the
            // grazing angle at the I-th terrain point, assuming that
            // a plane is placed on the I-th terrain point which
            // reflects the transmitted ray toward the target.  Set
            // the logical variable SPCULR to .TRUE. to indicate that
            // the I-th terrain point has been determined to be a
            // specular reflection point.
            //-----------------------------------------------------------

            gamma  = 0.5 * (atan(slptgl) - atan(tanepp[i]));
            spculr = true;
         }
         else
         {
            //-----------------------------------------------------------
            // Determine TANGAM, the tangent of the angle between the
            // ray connecting the radar to the I+1 th terrain point
            // and the line between the I-th terrain point and I+1 th
            // terrain point.  Determine SLOPER, the slope of the ray
            // connecting the radar and the I+1 th terrain point
            // after it is reflected from the I+1 th terrain point.
            //-----------------------------------------------------------

            tangam = (pslope - tanepp[ip1]) / (1.0 + pslope * tanepp[ip1]);
            sloper = (pslope + tangam) / (1.0 - pslope * tangam);

            if (slopel > slptgl && sloper < slptgr)
            {
               //--------------------------------------------------------
               // The target is between the reflected ray from the
               // I-th terrain point and the reflected ray from the
               // I+1 th terrain point.  The specular reflection
               // point lies between these two points.  To save
               // computation time, rather than find this point
               // exactly, the I-th terrain point will be assumed to
               // be the reflection point.  Determine GAMMA, in
               // radians, the grazing angle at the I-th terrain
               // point, assuming that a plane is placed on the I-th
               // terrain point which reflects the transmitted ray
               // toward the target.  Set the logical variable SPCULR
               // to .TRUE. to indicate that the I-th terrain point
               // has been determined to be a specular reflection
               // point.
               //--------------------------------------------------------

               tantrg = (ztprof - zprofl[i]) / (xtprof - xprofl[i]);
               gamma  = 0.5 * (atan(tantrg) - atan(tanepp[i]));
               spculr = true;
            }
         }

         if (spculr)
         {
            //-----------------------------------------------------------
            // The I-th point in the terrain profile has been
            // determined to be a specular point.  Determine SINGAM
            // and COSGAM, the sine and cosine, respectively, of the
            // grazing angle.
            //-----------------------------------------------------------

            singam = sin(gamma);
            cosgam = cos(gamma);

            //-----------------------------------------------------------
            // Determine RANGE1, in meters, the range from the radar
            // to the specular point.
            //-----------------------------------------------------------

            deltz1 = zprofl[i] - hammsl;
            range1 = sqrt(xprofl[i] * xprofl[i] + deltz1 * deltz1);

            //-----------------------------------------------------------
            // Determine RANGE2, in meters, the range from the
            // specular point to the target.
            //-----------------------------------------------------------

            deltx2 = xtprof - xprofl[i];
            deltz2 = ztprof - zprofl[i];
            range2 = sqrt(deltx2 * deltx2 + deltz2 * deltz2);

            //-----------------------------------------------------------
            // Determine RNGREF, in meters, the total reflected range
            // from the radar to the target, and DELTAR, in meters,
            // the path length difference between the direct range to
            // the target and the reflected range to the target.
            //-----------------------------------------------------------

            rngref = range1 + range2;
            deltar = rngref - ranget;

            //-----------------------------------------------------------
            // Check for overlap of the direct and reflected pulses.
            // If the pulses overlap set OVRLAP to true.
            //-----------------------------------------------------------

            if (deltar > pulsew)
            {
               ovrlap = false;
            }
            else if (deltar <= pulsew)
            {
               ovrlap = true;
            }

            //-----------------------------------------------------------
            // Determine ZONWID, in meters, the first Fresnel zone
            // extent of the specular point with respect to a plane
            // passing through the specular point which yields the
            // current grazing angle.
            //-----------------------------------------------------------

            rsmall = rngref * cosgam;
            hsub1  = range1 * singam;
            hsub2  = range2 * singam;
            rdelta = rngref + 0.5 * rlamda;
            aminus = rdelta * rdelta - rsmall * rsmall;
            bcoeff = rsmall * (hsub2 * hsub2 - hsub1 * hsub1 - aminus);
            cterm  = hsub1 * hsub1 + hsub2 * hsub2 - aminus;
            ccoeff = 0.25 * cterm * cterm - hsub1 * hsub1 * (rsmall * rsmall + hsub2 * hsub2);
            radicl = bcoeff * bcoeff + 4.0 * aminus * ccoeff;
            zonwid = sqrt(radicl) / aminus;

            //-----------------------------------------------------------
            // Determine if the first Fresnel zone extent of the
            // current specular point is the largest yet found for
            // this visible area.  Determine if the direct and
            // reflected pulses overlap.
            //-----------------------------------------------------------

            if (zonwid > fznmax && ovrlap)
            {
               //--------------------------------------------------------
               // The direct and reflected pulses overlap.  The first
               // Fresnel zone extent of the current point is the
               // largest yet found for this visible area.  Update
               // IFZMAX, the index of the specular point with the
               // largest first Fresnel zone in this visible area,
               // FZNMAX, in meters, the largest first Fresnel zone
               // extent, GAMMAX, in radians, the grazing angle at
               // this specular point, and DELMAX, in meters, the
               // path length difference at this specular point.
               //--------------------------------------------------------

               ifzmax = i;
               fznmax = zonwid;
               gammax = gamma;
               delmax = deltar;
            }
         }

         slptgl = slptgr;

         // NOT-USED label_30: ;
      } // for i

      if (ifzmax > 0)
      {
         //--------------------------------------------------------------
         // A specular point was found in the current visible area.
         // Update SUMWID, in meters, the sum of the largest first
         // Fresnel zone extents from each visible area.  Call
         // subroutine RFLECT to determine RCOEFF, the complex
         // reflection coefficient at the specular point with the
         // largest first Fresnel zone extent in the current visible
         // area.
         //--------------------------------------------------------------

         sumwid = sumwid + fznmax;

         /*call*/ rflect(gammax, ipolar, rcoeff, ysquar);

         //--------------------------------------------------------------
         // If the land cover is water then the roughness coefficient is
         // a function of the grazing angle.
         //--------------------------------------------------------------

         // BOEING-BEG: Added ability to use MIT-LL/SALRAM data tables (it is more complete)
         if (mUseMIT_LL_DataTables)
         {
            rrough = RoughSurfaceReflection(mWSF_LandForm, water_cover, mWSF_SeaState, rlamda, gammax);
            asubs  = cmplx(real(rrough)) * rcoeff * cmplx(real(fznmax));
         }
         // BOEING-END: Added ability to use MIT-LL/SALRAM data tables (it is more complete)
         else if (water_cover)
         {
            asubs = cmplx(real(exp(rrough * pow(singam, 2)))) * rcoeff * cmplx(real(fznmax));
         }
         else
         {
            if (use_surface_height)
            {
               x = surface_height;
               if (terrain_sw)
               {
                  i = iend[nareas];
                  x = stddev(i, elvmsl);
               }
               asubs = cmplx(exp(-2.0 * pow((twopi * x * singam / rlamda), 2)), 0.0);
               asubs = asubs * rcoeff * cmplx(fznmax, 0.0);
            }
            else
            {
               asubs = cmplx(real(rrough)) * rcoeff * cmplx(real(fznmax));
            }
         }

         epslnp = atan(tanepp[ifzmax]);

         // F4 propagation factor should not be normalized by antenna gain (CR-106)
         /*call*/ // get_relative_gain(ant_data, alphat, epslnp, gbelos);

         if (!mUnitTestPropagation)
         {
            argmnt = cmplx(0.0, real(twopi * delmax / rlamda));
            crflct = crflct + asubs * /*cmplx(real(sqrt(gbelos))) **/
                                 cexp(-argmnt);
         }
         else
         {
            double gbelos = 1.0;
            /*call*/ get_relative_gain(ant_data, alphat, epslnp, gbelos);
            argmnt = cmplx(0.0, real(twopi * delmax / rlamda));
            crflct = crflct + asubs * cmplx(real(sqrt(gbelos))) * cexp(-argmnt);
         }
      }

      // NOT-USED label_40: ;
   } // for (j

   //-------------------------------------------------------------------
   // Determine GBELOD, the normalized antenna gain along the direct ray
   // from the radar to the target.
   //-------------------------------------------------------------------
   // F4 propagation factor should not be normalized by antenna gain (CR-106)
   /*call*/ // get_relative_gain(ant_data, alphat, epslnt, gbelod);

   if (sumwid > 0.0)
   {
      //----------------------------------------------------------------
      // At least one specular point was found in the terrain profile.
      //----------------------------------------------------------------

      if (!mUnitTestPropagation)
      {
         fsubm = /*cmplx(real(sqrt(gbelod))) +*/ cmplx(1.0) + crflct / cmplx(real(sumwid));
      }
      else
      {
         double gbelod = 1.0;
         /*call*/ get_relative_gain(ant_data, alphat, epslnt, gbelod);
         fsubm = cmplx(real(sqrt(gbelod))) + crflct / cmplx(real(sumwid));
      }
   }
   else
   {
      //----------------------------------------------------------------
      // No specular points were found in the terrain profile.  At least
      // temporarily, set FSUBM, the pattern propagation factor, to the
      // square root of the antenna gain in the direction of the target.
      // A search for specular points has not been conducted between the
      // radar and the first point in the terrain profile.  If a
      // specular point can be found between the radar and the first
      // point in the terrain profile, the value of FSUBM will be
      // updated.
      //----------------------------------------------------------------

      spculr = false;
      fsubm  = cmplx(1.0); // cmplx(real(sqrt(gbelod)));

      if (nareas > 0 && istart[1] == 1)
      {
         //-------------------------------------------------------------
         // The first point in the terrain profile is visible from both
         // the radar and the target.  Assume that the terrain point at
         // the base of the radar is also visible from the target.
         // Determine PSLOPE, the slope of the terrain between the radar
         // and the first point in the terrain profile.  Determine
         // TANGAM, the tangent of the angle between the ray connecting
         // the radar to the first terrain point and the line whose
         // slope is PSLOPE.  Determine SLOPER, the slope of the ray
         // connecting the radar and the first terrain point after it is
         // reflected from the first terrain point.  Determine SLPTGR,
         // the slope of the line connecting the first terrain point and
         // the target.
         //-------------------------------------------------------------

         pslope = (zprofl[1] - zprofl[0]) / xprofl[1];
         tangam = (pslope - tanepp[1]) / (1.0 + pslope * tanepp[1]);
         sloper = (pslope + tangam) / (1.0 - pslope * tangam);
         slptgr = (ztprof - zprofl[1]) / (xtprof - xprofl[1]);

         if (sloper < slptgr)
         {
            if (pslope >= 0.0)
            {
               //-------------------------------------------------------
               // The terrain slopes up from the radar site to the first
               // point in the terrain profile and the target is above
               // the reflected ray at the first terrain point.  Set
               // SPCULR to .TRUE. to indicate a specular reflection
               // point exists between the radar site and the first
               // point in the terrain profile.
               //-------------------------------------------------------

               spculr = true;
            }
            else
            {
               //-------------------------------------------------------
               // Determine SLOPEL, the slope of the ray connecting the
               // radar and the base of the radar antenna after it is
               // reflected from the base of the radar antenna.
               // Determine SLPTGL, the slope of the line connecting the
               // base of the antenna and the target.
               //-------------------------------------------------------

               slopel = 0.5 * (pslope - 1.0 / pslope);
               slptgl = (ztprof - zprofl[0]) / xtprof;

               if (slopel > slptgl)
               {
                  //----------------------------------------------------
                  // The terrain slopes down from the radar site to the
                  // first point in the terrain profile and the target
                  // is between the reflected ray at the radar site and
                  // the reflected ray at the first point in the terrain
                  // profile.  Set SPCULR to .TRUE. to indicate a
                  // specular reflection point exists between the radar
                  // site and the first point in the terrain profile.
                  //----------------------------------------------------

                  spculr = true;
               }
            }

            if (spculr)
            {
               //-------------------------------------------------------
               // A specular reflection point lies between the radar
               // site and the first point in the terrain profile.  To
               // save computation time, rather than find this point
               // exactly, the first terrain point will be assumed to be
               // the reflection point.  Determine GAMMA, in radians,
               // the grazing angle at the first terrain point, assuming
               // that a plane is placed on the first terrain point
               // which reflects the transmitted ray toward the target.
               //-------------------------------------------------------

               gamma  = 0.5 * (atan(slptgr) - atan(tanepp[1]));
               singam = sin(gamma);
               cosgam = cos(gamma);

               //-------------------------------------------------------
               // Determine RANGE1, in meters, the range from the radar
               // to the specular point.
               //-------------------------------------------------------

               deltx1 = xprofl[1];
               deltz1 = zprofl[1] - hammsl;
               range1 = sqrt(deltx1 * deltx1 + deltz1 * deltz1);

               //-------------------------------------------------------
               // Determine RANGE2, in meters, the range from the
               // specular point to the target.
               //-------------------------------------------------------

               deltx2 = xtprof - xprofl[1];
               deltz2 = ztprof - zprofl[1];
               range2 = sqrt(deltx2 * deltx2 + deltz2 * deltz2);

               //-------------------------------------------------------
               // Determine RNGREF, in meters, the total reflected range
               // from the radar to the target, and DELTAR, in meters,
               // the path length difference between the direct range to
               // the target and the reflected range to the target.
               //-------------------------------------------------------

               rngref = range1 + range2;
               deltar = rngref - ranget;

               //-------------------------------------------------------
               // Check for overlap of the direct and reflected pulses.
               // If the pulses overlap set OVRLAP to true.
               //-------------------------------------------------------

               if (deltar > pulsew)
               {
                  ovrlap = false;
               }
               else if (deltar <= pulsew)
               {
                  ovrlap = true;
               }

               if (ovrlap)
               {
                  //----------------------------------------------------
                  // Call subroutine RFLECT to determine RCOEFF, the
                  // complex reflection coefficient specular point.
                  //----------------------------------------------------

                  /*call*/ rflect(gamma, ipolar, rcoeff, ysquar);

                  //----------------------------------------------------
                  // If the land cover is water then the roughness
                  // coefficient is a function of the grazing angle.
                  //----------------------------------------------------

                  if (water_cover)
                  {
                     asubs = cmplx(real(exp(rrough * pow(singam, 2)))) * rcoeff * cmplx(real(fznmax));
                  }
                  else
                  {
                     if (use_surface_height)
                     {
                        x = surface_height;
                        if (terrain_sw)
                        {
                           i = iend[nareas];
                           x = stddev(i, elvmsl);
                        }
                        asubs = cmplx(exp(-2.0 * pow((twopi * x * singam / rlamda), 2)), 0.0);
                        asubs = asubs * rcoeff * cmplx(fznmax, 0.0);
                     }
                     else
                     {
                        asubs = cmplx(real(rrough)) * rcoeff * cmplx(real(fznmax));
                     }
                  }

                  epslnp = atan(tanepp[1]);

                  // F4 propagation factor should not be normalized by antenna gain (CR-106)
                  /*call*/ // get_relative_gain(ant_data, alphat, epslnp, gbelos);

                  if (!mUnitTestPropagation)
                  {
                     argmnt = cmplx(0.0, real(twopi * deltar / rlamda));
                     crflct = asubs * /*cmplx(real(sqrt(gbelos))) **/
                              cexp(-argmnt);

                     // F4 propagation factor should not be normalized by antenna gain (CR-106)
                     /*call*/ // get_relative_gain(ant_data, alphat, epslnt, gbelod);

                     fsubm = /*cmplx(real(sqrt(gbelod))) +*/ cmplx(1.0) + crflct;
                  }
                  else
                  {
                     double gbelos = 1.0;
                     /*call*/ get_relative_gain(ant_data, alphat, epslnp, gbelos);
                     argmnt = cmplx(0.0, real(twopi * deltar / rlamda));
                     crflct = asubs * cmplx(real(sqrt(gbelos))) * cexp(-argmnt);

                     double gbelod = 1.0;
                     /*call*/ get_relative_gain(ant_data, alphat, epslnt, gbelod);

                     fsubm = cmplx(real(sqrt(gbelod))) + crflct;
                  }
               }
            }
         }
      }
   }
   if (DebugEnabled())
   {
      auto logger = ut::log::debug();
      logger.GetStream().precision(16);
      logger << "Multipath pattern propagation factor:";
      logger.AddNote() << "Value: " << fsubm;
      logger.AddNote() << "Magnitude: " << cabs(fsubm);
   }
}

//---------------------------------------------------------------------
//     Purpose:
//     -------
//     Determines the complex reflection coefficient of a plane earth.
//
//
//     Inputs:
//     ------
//     GAMMA  -  The grazing angle (the complement of the angle of
//               incidence), in radians.
//
//     IPOLAR  -  The radar antenna polarization indicator.  If
//                IPOLAR = POL_VERTICAL, the antenna is vertically
//                polarized.  If IPOLAR = POL_HORIZONTAL,  the
//                antenna is horizontally polarized.
//
//     YSQUAR  -  The square of the normalized admittance of the
//                medium.  If the relative magnetic permeability of
//                the medium is assumed to be one, YSQUAR is equal to
//                the relative complex dielectric constant of the
//                medium.
//
//
//     Outputs:
//     -------
//     RCOEFF  -  The complex reflection coefficient.
//
//
//     References:
//     ----------
//     [1]  Beckmann, P. and Spizzichino, A.: "The Scattering of
//          Electromagnetic Waves From Rough Surfaces," Artech
//          House, Inc, Norwood, Massachusetts, 1987.
//
//
//     Author:
//     ------
//     Science Applications International Corporation (SAIC)
//     2301 Yale Boulevard, SE, Suite E
//     Albuquerque, New Mexico 87106
//     (505) 766-7400
//
//
//     Modified:
//     --------
//     Division 397
//     SAIC
//     1321 Research Park Drive
//     Dayton, OH 45432
//     (513) 429-6500
//
//
//     ALARM 3.2:
//     ---------
//     - COMPLX and RADPAR common blocks removed from subroutine RFLECT
//       to support stand alone multipath.  Those variables (YSQUAR and
//       IPOLAR) previously accessed through the common block are now
//       passed through the RFLECT argument list.  (SPCR-1058, JCL)
//
//     16-Feb-2005    - Modified to use new polarization types.
//                      Added computation of average value for
//                      polarization type other than vertical and
//                      horizontal.  See SPCR #1329.
//---------------------------------------------------------------------

// static
void WsfEM_ALARM_Propagation::rflect(double gamma, int ipolar, COMPLEX& rcoeff, COMPLEX ysquar)
{
   // use types_polarization, only : pol_vertical, pol_horizontal
   // implicit none

   //-------------------------------------------------------------------
   // Passed parameters
   //-------------------------------------------------------------------

   // complex rcoeff, ysquar

   // real(8) gamma

   // integer ipolar

   //-------------------------------------------------------------------
   // local variables
   //-------------------------------------------------------------------

   COMPLEX cdenom, cnumer, esineg, radicl, hcoeff, vcoeff;

   double cosgam, singam;

   //-------------------------------------------------------------------
   // Determine SINGAM, the sine of the grazing angle, and COSGAM,
   // the cosine of the grazing angle.
   //-------------------------------------------------------------------

   singam = sin(gamma);
   cosgam = cos(gamma);

   //-------------------------------------------------------------------
   // Determine RADICL, a term used in later expressions.
   //-------------------------------------------------------------------

   radicl = csqrt(ysquar - cmplx(real(cosgam * cosgam)));

   //-------------------------------------------------------------------
   // Determine RCOEFF, the complex reflection coefficient.  See
   // reference 1, p. 218-222 for a description of the formulas
   // employed.
   //-------------------------------------------------------------------

   if (ipolar == pol_vertical)
   {
      //----------------------------------------------------------------
      // The antenna is vertically polarized.
      //----------------------------------------------------------------

      esineg = ysquar * cmplx(real(singam));
      cnumer = esineg - radicl;
      cdenom = esineg + radicl;

      rcoeff = cnumer / cdenom;
   }
   else if (ipolar == pol_horizontal)
   {
      //----------------------------------------------------------------
      // The antenna is horizontally polarized.
      //----------------------------------------------------------------

      cnumer = cmplx(real(singam)) - radicl;
      cdenom = cmplx(real(singam)) + radicl;

      rcoeff = cnumer / cdenom;
   }
   else
   {
      //----------------------------------------------------------------
      // The polarization is circular or slant. Use an average of the
      // horizontal and vertical values.  Reference 1 only addresses
      // vertical and horizontal polarizations.
      //----------------------------------------------------------------

      // vertical
      esineg = ysquar * cmplx(real(singam));
      cnumer = esineg - radicl;
      cdenom = esineg + radicl;
      vcoeff = cnumer / cdenom;

      // horizontal
      cnumer = cmplx(real(singam)) - radicl;
      cdenom = cmplx(real(singam)) + radicl;
      hcoeff = cnumer / cdenom;

      // compute average of horizontal and vertical
      rcoeff = (hcoeff + vcoeff) / cmplx(2.0);
   }
}

//---------------------------------------------------------------------
//     Purpose:
//     -------
//     Compute the complex dielectric constant for water covers.  The
//     computation is based on the tables in "Radar Propagation at Low
//     Altitudes" by Meeks.  See Tables 4.2 and 4.3.
//
//     Author:      Mike Sutton  (Division 397)
//     ------
//                    Science Applications
//                  International Corporation
//                            (SAIC)
//
//     Modification Summary:
//     --------------------
//     10-Nov-2000    - Initially generated...
//
//---------------------------------------------------------------------

WsfEM_ALARM_Propagation::COMPLEX WsfEM_ALARM_Propagation::water_dielectric(double freq, bool sea_water, double water_temp)
{
   // use physical_consts, only : vlight

   // implicit none

   //-------------------------------------------------------------------
   // Passed parameters
   //-------------------------------------------------------------------
   // real(8), intent(in) :: freq           // Hz
   // logical, intent(in) :: sea_water
   // real(8), intent(in) :: water_temp     // deg C

   //-------------------------------------------------------------------
   // Local variables
   //-------------------------------------------------------------------
   // NOTE-C++: Added index 0 to allow use of 1-based indexing

   const double freqs[7 + 1] = {0.0, 0.1E9, 1.0E9, 2.0E9, 3.0E9, 4.0E9, 6.0E9, 8.0E9};

   const double temps[3 + 1] = {0.0, 0.0, 10.0, 20.0};

   const double e1_lake[7 + 1][3 + 1] = {{0.0, 0.0, 0.0, 0.0},
                                         {0.0, 85.9, 83.0, 79.1},
                                         {0.0, 84.9, 82.5, 78.8},
                                         {0.0, 82.1, 81.1, 78.1},
                                         {0.0, 77.9, 78.9, 76.9},
                                         {0.0, 72.6, 75.9, 75.3},
                                         {0.0, 61.1, 68.7, 71.0},
                                         {0.0, 50.3, 60.7, 65.9}};

   const double sigma_lake[7 + 1][3 + 1] = {{0.0, 0.0, 0.0, 0.0},
                                            {0.0, 0.38, 0.51, 0.64},
                                            {0.0, 0.87, 0.84, 0.88},
                                            {0.0, 2.30, 1.80, 1.60},
                                            {0.0, 4.40, 3.40, 2.70},
                                            {0.0, 7.00, 5.50, 4.30},
                                            {0.0, 13.0, 11.0, 8.30},
                                            {0.0, 18.0, 16.0, 13.0}};

   const double e1_sea[7 + 1][3 + 1] = {{0.0, 0.0, 0.0, 0.0},
                                        {0.0, 77.8, 75.6, 72.5},
                                        {0.0, 77.0, 75.2, 72.3},
                                        {0.0, 74.6, 74.0, 71.6},
                                        {0.0, 71.0, 72.1, 70.5},
                                        {0.0, 66.5, 69.5, 69.1},
                                        {0.0, 56.5, 63.2, 65.4},
                                        {0.0, 47.0, 56.2, 60.8}};

   const double sigma_sea[7 + 1][3 + 1] = {{0.0, 0.0, 0.0, 0.0},
                                           {0.0, 2.9, 3.8, 4.8},
                                           {0.0, 3.3, 4.1, 5.0},
                                           {0.0, 4.6, 5.0, 5.6},
                                           {0.0, 6.4, 6.4, 6.7},
                                           {0.0, 8.8, 8.2, 8.0},
                                           {0.0, 14.0, 13.0, 12.0},
                                           {0.0, 19.0, 18.0, 16.0}};

   int ifreql, itemp;

   const double(*epsilon1)[3 + 1];
   const double(*sigma)[3 + 1];

   double frqfrc, tempfrc, eps1l, eps1r, epsln1, eps2l, eps2r, epsln2;
   double lambda1, lambda2, x1, x2;

   //-------------------------------------------------------------------
   // NOT-NEEDED COMPLEX epsilon = cmplx(0.0, 0.0)

   if (sea_water)
   {
      epsilon1 = e1_sea;
      sigma    = sigma_sea;
   }
   else
   {
      epsilon1 = e1_lake;
      sigma    = sigma_lake;
   }

   //-------------------------------------------------------------------
   // Determine the index of the left-hand endpoint of the interval
   // containing the frequency.  Also determine the fractional distance
   // of the frequency from the left-hand endpoint of the interval.
   //
   // The frequency is less than or equal to the smallest frequency
   // contained in the table.  Set the index of the left-hand end- point
   // equal to one and the frequency ratio to zero.
   //
   // The frequency is greater than or equal to the largest frequency
   // contained in the table.  Set the index of the left-hand end-
   // point equal to the index of the next to the last point in the
   // table and the frequency ratio to one.
   //----------------------------------------------------------------
   frqfrc = 0.0; // eliminate g++ warning
   if (freq <= freqs[1])
   {
      ifreql = 1;
      frqfrc = 0.0;
   }
   else if (freq >= freqs[7])
   {
      ifreql = 7 - 1;
      frqfrc = 1.0;
   }
   else
   {
      for (ifreql = 1; ifreql <= (7 - 1); ++ifreql)
      {
         if (freq < freqs[ifreql + 1])
         {
            frqfrc = (freq - freqs[ifreql]) / (freqs[ifreql + 1] - freqs[ifreql]);
            break;
         }
      }
   }

   //-------------------------------------------------------------------
   // Determine the index of the left-hand endpoint of the interval
   // containing the water content.  Also determine the fractional
   // distance of the water content from the left-hand endpoint of
   // the interval.
   //-------------------------------------------------------------------
   tempfrc = 0.0; // eliminate g++ warning
   if (water_temp <= temps[1])
   {
      itemp   = 1;
      tempfrc = 0.0;
   }
   else if (water_temp >= temps[3])
   {
      itemp   = 3 - 1;
      tempfrc = 1.0;
   }
   else
   {
      for (itemp = 1; itemp <= (3 - 1); ++itemp)
      {
         if (water_temp < temps[itemp + 1])
         {
            tempfrc = (water_temp - temps[itemp]) / (temps[itemp + 1] - temps[itemp]);
            break;
         }
      }
   }

   //-------------------------------------------------------------------
   // Perform two-dimensional linear interpolation to determine
   // epsln1, the real part of the relative dielectric constant.
   //-------------------------------------------------------------------
   eps1l  = (1.0 - frqfrc) * epsilon1[ifreql][itemp] + frqfrc * epsilon1[ifreql + 1][itemp];
   eps1r  = (1.0 - frqfrc) * epsilon1[ifreql][itemp + 1] + frqfrc * epsilon1[ifreql + 1][itemp + 1];
   epsln1 = (1.0 - tempfrc) * eps1l + tempfrc * eps1r;

   //-------------------------------------------------------------------
   // Perform two-dimensional linear interpolation to determine
   // epsln2, the imaginary part of the relative dielectric
   // constant.
   //-------------------------------------------------------------------
   lambda1 = vlight / freqs[ifreql];
   lambda2 = vlight / freqs[ifreql + 1];
   x1      = 60.0 * lambda1 * sigma[ifreql][itemp];
   x2      = 60.0 * lambda2 * sigma[ifreql + 1][itemp];
   eps2l   = (1.0 - frqfrc) * x1 + frqfrc * x2;
   x1      = 60.0 * lambda1 * sigma[ifreql][itemp + 1];
   x2      = 60.0 * lambda2 * sigma[ifreql + 1][itemp + 1];
   eps2r   = (1.0 - frqfrc) * x1 + frqfrc * x2;
   epsln2  = (1.0 - tempfrc) * eps2l + tempfrc * eps2r;


   return /*epsilon =*/cmplx(real(epsln1), real(epsln2));
}

//---------------------------------------------------------------------
//     Purpose:
//     -------
//     Compute the complex dielectric constant for soil.  The
//     computation is based on the tables in "Radar Propagation at Low
//     Altitudes" by Meeks.  See Table 4.1.
//
//     Author:      Mike Sutton  (Division 397)
//     ------
//                    Science Applications
//                  International Corporation
//                            (SAIC)
//
//     Modification Summary:
//     --------------------
//     10-Nov-2000    - Initially generated...
//
//---------------------------------------------------------------------

WsfEM_ALARM_Propagation::COMPLEX WsfEM_ALARM_Propagation::soil_dielectric(double freq, double soil_moisture)
{
   // implicit none

   //-------------------------------------------------------------------
   // Passed parameters
   //-------------------------------------------------------------------
   // real(8), intent(in) :: freq              // Hz
   // real(8), intent(in) :: soil_moisture     // %

   //-------------------------------------------------------------------
   // Local variables
   //-------------------------------------------------------------------
   // NOTE-C++: Added index 0 to allow use of 1-based indexing

   const double freqs[5 + 1] = {0.0, 0.3E9, 3.0E9, 8.0E9, 14.0E9, 24.0E9};

   const double moistures[4 + 1] = {0.0, 0.003, 0.100, 0.20, 0.30};

   const double epsilon1[5 + 1][4 + 1] = {{0.0, 0.0, 0.0, 0.0, 0.0},
                                          {0.0, 2.9, 6.0, 10.5, 16.7},
                                          {0.0, 2.9, 6.0, 10.5, 16.7},
                                          {0.0, 2.8, 5.8, 10.3, 15.3},
                                          {0.0, 2.8, 5.6, 9.4, 12.6},
                                          {0.0, 2.6, 4.9, 7.7, 9.6}};

   const double epsilon2[5 + 1][4 + 1] = {{0.0, 0.000, 0.000, 0.000, 0.000},
                                          {0.0, 0.071, 0.450, 0.750, 1.200},
                                          {0.0, 0.027, 0.400, 1.100, 2.000},
                                          {0.0, 0.032, 0.870, 2.500, 4.100},
                                          {0.0, 0.035, 1.140, 3.700, 6.300},
                                          {0.0, 0.030, 1.150, 4.800, 8.500}};

   int ifreql, iwater;

   double frqfrc, h2ofrc, eps1l, eps1r, epsln1, eps2l, eps2r, epsln2, moisture;

   //-------------------------------------------------------------------
   // NOT-NEEDED epsilon = cmplx(0.0, 0.0)

   //-------------------------------------------------------------------
   // Determine the index of the left-hand endpoint of the interval
   // containing the frequency.  Also determine the fractional distance
   // of the frequency from the left-hand endpoint of the interval.
   //
   // The frequency is less than or equal to the smallest frequency
   // contained in the table.  Set the index of the left-hand end- point
   // equal to one and the frequency ratio to zero.
   //
   // The frequency is greater than or equal to the largest frequency
   // contained in the table.  Set the index of the left-hand end-
   // point equal to the index of the next to the last point in the
   // table and the frequency ratio to one.
   //----------------------------------------------------------------
   frqfrc = 0.0; // eliminate g++ warning
   if (freq <= freqs[1])
   {
      ifreql = 1;
      frqfrc = 0.0;
   }
   else if (freq >= freqs[5])
   {
      ifreql = 5 - 1;
      frqfrc = 1.0;
   }
   else
   {
      for (ifreql = 1; ifreql <= (5 - 1); ++ifreql)
      {
         if (freq < freqs[ifreql + 1])
         {
            frqfrc = (freq - freqs[ifreql]) / (freqs[ifreql + 1] - freqs[ifreql]);
            break;
         }
      }
   }

   //-------------------------------------------------------------------
   // Determine the index of the left-hand endpoint of the interval
   // containing the water content.  Also determine the fractional
   // distance of the water content from the left-hand endpoint of
   // the interval.
   //-------------------------------------------------------------------
   h2ofrc   = 0.0; // eliminate g++ warning
   moisture = soil_moisture * 1.E-2;
   if (moisture <= moistures[1])
   {
      iwater = 1;
      h2ofrc = 0.0;
   }
   else if (moisture >= moistures[4])
   {
      iwater = 4 - 1;
      h2ofrc = 1.0;
   }
   else
   {
      for (iwater = 1; iwater <= (4 - 1); ++iwater)
      {
         if (moisture < moistures[iwater + 1])
         {
            h2ofrc = (moisture - moistures[iwater]) / (moistures[iwater + 1] - moistures[iwater]);
            break;
         }
      }
   }

   //-------------------------------------------------------------------
   // Perform two-dimensional linear interpolation to determine
   // epsln1, the real part of the relative dielectric constant.
   //-------------------------------------------------------------------
   eps1l  = (1.0 - frqfrc) * epsilon1[ifreql][iwater] + frqfrc * epsilon1[ifreql + 1][iwater];
   eps1r  = (1.0 - frqfrc) * epsilon1[ifreql][iwater + 1] + frqfrc * epsilon1[ifreql + 1][iwater + 1];
   epsln1 = (1.0 - h2ofrc) * eps1l + h2ofrc * eps1r;

   //-------------------------------------------------------------------
   // Perform two-dimensional linear interpolation to determine
   // epsln2, the imaginary part of the relative dielectric
   // constant.
   //-------------------------------------------------------------------
   eps2l  = (1.0 - frqfrc) * epsilon2[ifreql][iwater] + frqfrc * epsilon2[ifreql + 1][iwater];
   eps2r  = (1.0 - frqfrc) * epsilon2[ifreql][iwater + 1] + frqfrc * epsilon2[ifreql + 1][iwater + 1];
   epsln2 = (1.0 - h2ofrc) * eps2l + h2ofrc * eps2r;


   return /*epsilon =*/cmplx(real(epsln1), real(epsln2));
}

//---------------------------------------------------------------------
//     Purpose:
//     -------
//     Determines the standard deviation of the normal distribution.
//
//     Author:      Mike Sutton  (Division 397)
//     ------
//                    Science Applications
//                  International Corporation
//                            (SAIC)
//
//     Modification Summary:
//     --------------------
//     10-Nov-2000    - Initially generated...
//
//---------------------------------------------------------------------

double WsfEM_ALARM_Propagation::stddev(int n, const std::vector<double>& a)
{
   // implicit none

   //-------------------------------------------------------------------
   // Passed arguments
   //-------------------------------------------------------------------

   // integer, intent(in) :: n        ! number of points
   // real(8), intent(in) :: a(n)     ! array points

   //-------------------------------------------------------------------
   // Local variables
   //-------------------------------------------------------------------
   double sums, sumsqr, mean;

   //-------------------------------------------------------------------
   // Initialize
   //-------------------------------------------------------------------
   // stddev = 0.0;
   sums   = 0.0;
   sumsqr = 0.0;
   mean   = 0.0;

   //-------------------------------------------------------------------
   // Loop over the cells in the specified region and convert units as
   // necessary.
   //-------------------------------------------------------------------
   // sums   = sum(a)
   // sumsqr = dot_product(a, a)    // sum of squares short cut
   for (int i = 1; i <= n; ++i)
   {
      sums += a[i];
      sumsqr += (a[i] * a[i]);
   }
   mean = sums / dble(n);
   return /*stddev =*/sqrt(sumsqr / dble(n) - mean * mean);
}

// ================================================================================================
// Start of code taken from SalramClutter.cpp
// ================================================================================================

//! Get dielectric constant and conductivity based on land coverage.
//!
//! @param aLandCover [input] Type of land cover.
//! - 0,  General                   ------ General
//! - 1,  Urban                     ------ Urban
//! - 2,  Agricultural              ------ Open Farmland
//! - 3,  Rangeland-Herbaceous      ------ Grassland
//! - 4,  RangeLand-Shrub and Mixed ------ Grassland
//! - 5,  Forest-Deciduous          ------ Continuous Forest
//! - 6,  Forest-Coniferous         ------ Continuous Forest
//! - 7,  Forest-Mixed              ------ Continuous Forest
//! - 8,  Forest-Clear Cut          ------ Continuous Forest
//! - 9,  Forest-Block Cut          ------ Continuous Forest
//! - 10, WetLand-Forested          ------ Marsh
//! - 11, WetLand-Non-forested      ------ Marsh
//! - 12, Barren                    ------ Desert
//! @param aDielectricConstant [output]
//! @param aConductivity       [output]
//!
//! Data based on MIT Lincoln Lab's report on radar propagation at low
//! altitude and tabulated radar ground clutter, and from Nathanson page 272.
//! Data voids filled by J.A. Hueseman, E245 MDAC-STL.

// private static
void WsfEM_ALARM_Propagation::GetLandCoverData(int aLandCover, double& aDielectricConstant, double& aConductivity)
{
   static const double dielectricConstant[13] = {
      3.0,  // 0
      3.0,  // 1
      10.0, // 2
      10.0, // 3
      10.0, // 4
      14.0, // 5,
      16.0, // 6,
      14.0, // 7,
      12.0, // 8,
      12.0, // 9,
      24.0, // 10,
      24.0, // 11,
      3.0   // 12
   };

   static const double conductivity[13] = {
      0.000075, // 0
      0.000075, // 1
      0.0005,   // 2
      0.0005,   // 3
      0.0005,   // 4
      0.001,    // 5
      0.001,    // 6
      0.001,    // 7
      0.001,    // 8
      0.001,    // 9
      0.005,    // 10
      0.005,    // 11
      0.000075  // 12
   };

   int landCover = aLandCover;
   if ((landCover < 0) || (landCover > 12))
   {
      landCover = 0;
   }

   aDielectricConstant = dielectricConstant[landCover];
   aConductivity       = conductivity[landCover];
}

// ================================================================================================
// Start of code taken from SalramPropagation.cpp
// ================================================================================================

//! Computes rough surface reflection coefficient.
//!
//! Computes 'rho' based on Nathanson (pg 36) with Beard's correction
//! factor from Skolnik (pg 2-40) for large crmet values.

// Was srho
// private static
double WsfEM_ALARM_Propagation::RoughSurfaceReflection(int aLandForm, bool aOnWater, int aSeaState, double aWavelength, double aPsi)
{
   double sigh = 0.0;
   if (aOnWater)
   {
      sigh = 2.0 * sWaveHeight[aSeaState] / 4.0;
   }
   else
   {
      if (aLandForm == 1)
      {
         sigh = 6.10;
      }
      if (aLandForm == 2)
      {
         sigh = 15.24;
      }
      if (aLandForm == 3)
      {
         sigh = 19.81;
      }
      if (aLandForm == 4)
      {
         sigh = 45.72;
      }
      if (aLandForm == 5)
      {
         sigh = 25.91;
      }
      if (aLandForm == 6)
      {
         sigh = 83.82;
      }
      if (aLandForm == 7)
      {
         sigh = 30.48;
      }
      if (aLandForm == 8)
      {
         sigh = 30.48;
      }
      if (aLandForm == 9)
      {
         sigh = 15.24;
      }
   }
   double sinPsi    = sin(aPsi);
   double frequency = vlight / aWavelength;
   double crmet     = (frequency * 1.0E-6) * sigh * sinPsi;
   double rho       = 0.0;
   if ((crmet * 3.28) >= 100.0)
   {
      rho = 0.6674 - (0.0078 * crmet);
   }
   else
   {
      double x = twopi * sigh * sinPsi / aWavelength;
      double y = 2.0 * x * x;
      if (y <= 5.0)
      {
         rho = exp(-y);
      }
   }
   rho = std::max(rho, 0.01);
   return rho;
}
