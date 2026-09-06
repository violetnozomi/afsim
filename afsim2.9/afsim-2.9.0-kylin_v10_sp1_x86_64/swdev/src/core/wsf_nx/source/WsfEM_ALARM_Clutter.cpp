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

#include "WsfEM_ALARM_Clutter.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>

#include "TblLookup.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfAntennaPattern.hpp"
#include "WsfArticulatedPart.hpp"
#include "WsfEM_ALARM_Antenna.hpp"
#include "WsfEM_ALARM_Attenuation.hpp"
#include "WsfEM_ALARM_Fortran.hpp"
#include "WsfEM_ALARM_Geometry.hpp"
#include "WsfEM_ALARM_Terrain.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEnvironment.hpp"
#include "WsfMIT_ClutterStrength.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationInput.hpp"
#include "WsfTerrain.hpp"

// =================================================================================================
// Following are a collection of definitions that makes it easier to port FORTRAN to C++
// =================================================================================================

namespace
{
inline double dble(float aValue)
{
   return static_cast<double>(aValue);
}

inline int mod(int aArg1, int aArg2)
{
   return aArg1 % aArg2;
}
} // namespace

// =================================================================================================
// Various constants from ALARM.
// =================================================================================================

namespace
{

//--------------------------------------------------------------------
// Misc constants.
//--------------------------------------------------------------------
double min_db = -370.0; // dB : minimum value for dBs

// ==============================================================================================
// The following is from ALARM 'types_polarization.f90'
// ==============================================================================================

const int ilflc[WsfEM_ALARM_Clutter::max_covers][WsfEM_ALARM_Clutter::max_forms] = {
   {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0},
   {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
   {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
   {1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
   {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
   {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1}};
} // namespace

int                 WsfEM_ALARM_Clutter::aprofile = 0;
bool                WsfEM_ALARM_Clutter::uninit   = true;
std::vector<int>    WsfEM_ALARM_Clutter::iend;
std::vector<int>    WsfEM_ALARM_Clutter::istart;
std::vector<bool>   WsfEM_ALARM_Clutter::visibl;
std::vector<double> WsfEM_ALARM_Clutter::elvmsl;
std::vector<double> WsfEM_ALARM_Clutter::rngter; // 0:
std::vector<double> WsfEM_ALARM_Clutter::tanepp;
std::vector<double> WsfEM_ALARM_Clutter::xprofl; // 0:
std::vector<double> WsfEM_ALARM_Clutter::zprofl; // 0

// =================================================================================================
WsfEM_ALARM_Clutter::WsfEM_ALARM_Clutter()
   : WsfEM_Clutter()
   , // return values are NOT filtered
   map_file()
   , sigmac(0.0)
   , decay_const(0.0)
   , reflectivity(0.0)
   , reflectivity_delta(0.0)
   , max_range(100.0E+3)
   , az_max_angle_deg(0.0)
   , az_angle_incr_deg(0.0)
   , az_max_angle_rad(0.0)
   , az_angle_incr_rad(0.0)
   , cw_clutter_bin(1000.0)
   , // from cw.f90
   land_form(0)
   , statistic_opt(stat_mean)
   , // ALARM doesn't default, but we do for simplicity
   random_seed(1234567)
   , // ALARM doesn't default, but we do for simplicity
   patch_count(0)
   , iseed(1234567)
   , clutter_sw(true)
   , // If this object exists, clutter is enabled!
   map_sw(false)
   , mUseBeamwidthForIncrement(false)
   , mUseMIT_LL_DataTables(true)
   , mPolarizationVertical(false)
   , mWSF_LandCover(WsfEnvironment::cGENERAL)
   , mWSF_LandForm(WsfEnvironment::cLEVEL)
   , mWSF_SeaState(WsfEnvironment::cCALM_GLASSY)
   , mUseSALRAM_DataTables(false)
   , mSimulationPtr(nullptr)
{
}

// =================================================================================================
WsfEM_ALARM_Clutter::WsfEM_ALARM_Clutter(const WsfEM_ALARM_Clutter& aSrc)
   : WsfEM_Clutter(aSrc)
   , map_file(aSrc.map_file)
   , sigmac(aSrc.sigmac)
   , decay_const(aSrc.decay_const)
   , reflectivity(aSrc.reflectivity)
   , reflectivity_delta(aSrc.reflectivity_delta)
   , max_range(aSrc.max_range)
   , az_max_angle_deg(aSrc.az_max_angle_deg)
   , az_angle_incr_deg(aSrc.az_angle_incr_deg)
   , az_max_angle_rad(aSrc.az_max_angle_rad)
   , az_angle_incr_rad(aSrc.az_angle_incr_rad)
   , cw_clutter_bin(aSrc.cw_clutter_bin)
   , land_form(aSrc.land_form)
   , statistic_opt(aSrc.statistic_opt)
   , random_seed(aSrc.random_seed)
   , patch_count(aSrc.patch_count)
   , iseed(aSrc.iseed)
   , clutter_sw(aSrc.clutter_sw)
   , map_sw(aSrc.map_sw)
   , mUseBeamwidthForIncrement(aSrc.mUseBeamwidthForIncrement)
   , mUseMIT_LL_DataTables(aSrc.mUseMIT_LL_DataTables)
   , mPolarizationVertical(aSrc.mPolarizationVertical)
   , mWSF_LandCover(aSrc.mWSF_LandCover)
   , mWSF_LandForm(aSrc.mWSF_LandForm)
   , mWSF_SeaState(aSrc.mWSF_SeaState)
   , mUseSALRAM_DataTables(aSrc.mUseSALRAM_DataTables)
   , mSimulationPtr(aSrc.mSimulationPtr)
{
}

// =================================================================================================
//! Factory method called by WsfEM_ClutterTypes.
// static
WsfEM_Clutter* WsfEM_ALARM_Clutter::ObjectFactory(const std::string& aTypeName)
{
   WsfEM_Clutter* instancePtr = nullptr;
   if ((aTypeName == "WSF_ALARM_CLUTTER") || (aTypeName == "alarm"))
   {
      instancePtr = new WsfEM_ALARM_Clutter();
   }
   return instancePtr;
}

// =================================================================================================
//! Reset all global/static data to default values
// static
void WsfEM_ALARM_Clutter::ResetState()
{
   aprofile = 0;
   uninit   = true;
   iend.clear();
   istart.clear();
   visibl.clear();
   elvmsl.clear();
   rngter.clear();
   tanepp.clear();
   xprofl.clear();
   zprofl.clear();
}

// =================================================================================================
// virtual
WsfEM_Clutter* WsfEM_ALARM_Clutter::Clone() const
{
   return new WsfEM_ALARM_Clutter(*this);
}

// =================================================================================================
// virtual
bool WsfEM_ALARM_Clutter::Initialize(WsfEM_Rcvr* aRcvrPtr)
{
   bool ok        = WsfEM_Clutter::Initialize(aRcvrPtr);
   mSimulationPtr = aRcvrPtr->GetSimulation();
   // From init_clutter

   az_max_angle_rad = az_max_angle_deg * deg2rad;

   if (!map_file.empty())
   {
      // NOT YET SUPPORTED!!!
   }
   else
   {
      //-------------------------------------------------------------------
      // Check to see if the LAND_FORM / LAND_COVER pair is valid.  Also
      // check to see that the clutter option and integer seed are valid
      // for the proper combination of inputs.
      //-------------------------------------------------------------------

      int  land_cover;
      bool water_cover;
      MapEnvironment(aRcvrPtr->GetPlatform(),
                     aRcvrPtr->GetPlatform()->GetScenario().GetEnvironment(),
                     land_cover,
                     land_form,
                     water_cover);

      // BOEING-BEG: Added ability to use MIT-LL/SALRAM data tables (it is more complete)
      if (mUseMIT_LL_DataTables)
      {
      }
      // BOEING-END: Added ability to use MIT-LL/SALRAM data tables (it is more complete)
      else if (ilflc[land_cover - 1][land_form - 1] == 0)
      {
         auto logger = ut::log::warning() << "Clutter coefficient not defined for the selected environment.";
         logger.AddNote() << "No clutter will be computed.";
         logger.AddNote() << "Platform: " << aRcvrPtr->GetPlatform()->GetName();
         logger.AddNote() << "Part: " << aRcvrPtr->GetArticulatedPart()->GetName();
      }

      //-------------------------------------------------------------------
      // Compute the max angle in radians and the number of clutter
      // patches.
      //-------------------------------------------------------------------

      az_angle_incr_rad = az_angle_incr_deg * deg2rad;

      // NOTE-BOEING
      // If the azimuth increment is zero then it only a single mainbeam sample is performed.
      // In that case the width of the sample is the increment. If the increment is also zero
      // the azimuth beamwidth of the antenna is used.
      if (az_max_angle_deg == 0.0)
      {
         patch_count               = 0;
         mUseBeamwidthForIncrement = (az_angle_incr_deg == 0.0);
      }
      else
      {
         if ((az_angle_incr_deg == 0.0) || (az_angle_incr_deg > az_max_angle_deg))
         {
            auto logger = ut::log::error() << "Invalid azimuth_angle_increment.";
            logger.AddNote() << "Provided: " << az_angle_incr_deg << " deg";
            logger.AddNote() << "Expected: 0 deg < azimuth_angle_increment <= " << az_max_angle_deg << " deg";
            return false;
         }

         // C++ NOTE: the addition of the 1.0E-12 while computing the patch count
         // is to make for better agreement with the original version. Our input
         // routines read degrees and convert to radians which we then convert back
         // to degrees. This causes a slight numerical difference.
         patch_count = int(az_max_angle_deg / az_angle_incr_deg + 1.0E-12);
         if (dble(2 * patch_count + 1) * az_angle_incr_deg > 360.0)
         {
            patch_count = patch_count - 1;
         }
      }

      // BOEING-BEG: Added ability to use MIT-LL/SALRAM data tables (it is more complete)
      if (mUseMIT_LL_DataTables)
      {
      }
      // BOEING-END: Added ability to use MIT-LL/SALRAM data tables (it is more complete)
      if (statistic_opt == stat_numerical)
      {
         if ((reflectivity < min_db) || (reflectivity > 0.0))
         {
            auto logger = ut::log::error() << "Invalid reflectivity.";
            logger.AddNote() << "Provided: " << reflectivity << " db";
            logger.AddNote() << "Expected: " << min_db << " db <= reflectivity <= 0 db";
            ok = false;
         }
         if (reflectivity_delta < 0.0)
         {
            auto logger = ut::log::error() << "Invalid reflectivity_delta.";
            logger.AddNote() << "Provided: " << reflectivity_delta << " db";
            logger.AddNote() << "Expected: 0 db <= reflectivity_delta";
            ok = false;
         }
         if ((random_seed < 1000) || (mod(random_seed, 2) == 0))
         {
            auto logger = ut::log::error() << "Invalid random_seed.";
            logger.AddNote() << "Provided: " << random_seed;
            logger.AddNote() << "Expected: large (> 1000), positive, odd integer";
            ok = false;
         }
      }
      else
      {
         if (water_cover && ((statistic_opt < stat_mean) || (statistic_opt > stat_max)))
         {
            ut::log::error() << "The sea clutter statistic must be mean, statistical or maximum.";
            ok = false;
         }
         else if (water_cover)
         {
            // C++NOTE I'm not sure why this is here as it is effectively filtered out in the above
            if ((statistic_opt < stat_mean) || (statistic_opt > stat_min))
            {
               ut::log::error() << "The sea clutter statistics must be mean, statistical, maximum or minimum.";
               ok = false;
            }
            if ((statistic_opt == stat_stat) && ((random_seed < 1000) || (mod(random_seed, 2) == 0)))
            {
               auto logger = ut::log::error() << "Invalid random_seed.";
               logger.AddNote() << "Provided: " << random_seed;
               logger.AddNote() << "Expected: large (> 1000), positive, odd integer";
               ok = false;
            }
         }
      }

      /*call*/ set_random_seed(random_seed);
   }
   return ok;
}

// =================================================================================================
// virtual
bool WsfEM_ALARM_Clutter::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "sigmac")
   {
      aInput.ReadValueOfType(sigmac, UtInput::cFREQUENCY);
   }
   else if (command == "decay_constant")
   {
      // Units are Hz^2
      std::string units;
      aInput.ReadValue(decay_const);
      aInput.ValueGreater(decay_const, 0.0);
      aInput.ReadValue(units);
      std::string::size_type length = units.size();
      if ((length > 1) && (units[length - 1] == '2'))
      {
         --length;
         if ((length > 1) && (units[length - 1] == '^'))
         {
            --length;
         }
         double multiplier = aInput.ConvertValue(1.0, units.substr(0, length), UtInput::cFREQUENCY);
         decay_const *= (multiplier * multiplier);
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "azimuth_angle_limit")
   {
      aInput.ReadValueOfType(az_max_angle_rad, UtInput::cANGLE);
      // NOTE: ALARM does not allow a value of zero, but we do.
      //       This allows us to run a single main-beam only sample.
      aInput.ValueGreaterOrEqual(az_max_angle_rad, 0.0);
      az_max_angle_deg = az_max_angle_rad * UtMath::cDEG_PER_RAD;
   }
   else if (command == "azimuth_angle_increment")
   {
      aInput.ReadValueOfType(az_angle_incr_rad, UtInput::cANGLE);
      // NOTE: ALARM does not allow a value of zero, but we do.
      //       This allows us to run a single main-beam only sample.
      aInput.ValueGreaterOrEqual(az_angle_incr_rad, 0.0);
      az_angle_incr_deg = az_angle_incr_rad * UtMath::cDEG_PER_RAD;
   }
   else if (command == "maximum_range")
   {
      aInput.ReadValueOfType(max_range, UtInput::cLENGTH);
      aInput.ValueGreater(max_range, 0.0);
   }
   else if (command == "reflectivity")
   {
      aInput.ReadValueOfType(reflectivity, UtInput::cRATIO);
      aInput.ValueGreater(reflectivity, 0.0);
      aInput.ValueLessOrEqual(reflectivity, 1.0);
      reflectivity          = UtMath::LinearToDB(reflectivity);
      mUseMIT_LL_DataTables = false;
   }
   else if (command == "reflectivity_delta")
   {
      aInput.ReadValueOfType(reflectivity_delta, UtInput::cRATIO);
      aInput.ValueGreater(reflectivity_delta, 0.0);
      reflectivity_delta    = UtMath::LinearToDB(reflectivity_delta);
      mUseMIT_LL_DataTables = false;
   }
   else if (command == "statistic")
   {
      std::string statistic_str;
      aInput.ReadValue(statistic_str);
      if (statistic_str == "mean")
      {
         statistic_opt = stat_mean;
      }
      else if (statistic_str == "statistical")
      {
         statistic_opt = stat_stat;
      }
      else if (statistic_str == "maximum")
      {
         statistic_opt = stat_max;
      }
      else if (statistic_str == "minimum")
      {
         statistic_opt = stat_min;
      }
      else if (statistic_str == "numerical")
      {
         statistic_opt = stat_numerical;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Bad value for 'statistic': " + statistic_str);
      }
      mUseMIT_LL_DataTables = false;
   }
   else if (command == "random_seed")
   {
      aInput.ReadValue(random_seed);
      if ((random_seed < 1000) || ((random_seed & 1) == 0))
      {
         throw UtInput::BadValue(aInput, "random_seed must be an large positive odd number greater than 1000");
      }
      mUseMIT_LL_DataTables = false;
   }
   else if (command == "cw_clutter_bin") // from cw.f90
   {
      aInput.ReadValueOfType(cw_clutter_bin, UtInput::cLENGTH);
      aInput.ValueGreater(cw_clutter_bin, 0.0);
   }
   else if (command == "use_legacy_data")
   {
      aInput.ReadValue(mUseSALRAM_DataTables);
   }
   else if (command == "use_native_terrain_masking")
   {
      bool useAFSIM_TerrainMasking = false;
      aInput.ReadValue(useAFSIM_TerrainMasking);
      WsfEM_ALARM_Terrain::SetUseAFSIM_TerrainMasking(useAFSIM_TerrainMasking);
   }
   else
   {
      myCommand = WsfEM_Clutter::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
double WsfEM_ALARM_Clutter::ComputeClutterPower(WsfEM_Interaction& aInteraction,
                                                WsfEnvironment&    aEnvironment,
                                                double             aProcessingFactor)
{
   // Make sure the interaction is a two-way interaction.

   WsfEM_Xmtr*  xmtrPtr   = aInteraction.GetTransmitter();
   WsfEM_Rcvr*  rcvrPtr   = aInteraction.GetReceiver();
   WsfPlatform* targetPtr = aInteraction.GetTarget();
   if ((xmtrPtr == nullptr) || (rcvrPtr == nullptr) || (targetPtr == nullptr))
   {
      return 0.0;
   }

   WsfEM_ALARM_Terrain::Initialize(targetPtr->GetTerrain());

   // This model is computationally intensive and should be avoided if possible.
   // If calculations shortcuts are allowed and if the current signal-to-noise is low,
   // then the calculation will be bypassed.

   if (targetPtr->GetScenario().GetSimulationInput().AllowClutterCalculationShortcuts())
   {
      // Only use receiver noise for comparison - this makes the test conservative.
      if (aInteraction.mRcvdPower < aInteraction.mRcvrNoisePower)
      {
         // As a further performance enhancement, if the propagation factor is very low then
         // indicate the terrain masking condition exists. This situation exists when the target
         // is where diffraction is no longer allowing it to be seen. This prevents subsequent
         // beams in a multi-beam radar from having to perform any computations.
         if (aInteraction.mPropagationFactor < 0.001)
         {
            aInteraction.mCheckedStatus |= WsfEM_Interaction::cRCVR_TERRAIN_MASKING;
            aInteraction.mFailedStatus |= WsfEM_Interaction::cRCVR_TERRAIN_MASKING;
         }
         return 0.0;
      }

      if (xmtrPtr->GetPropagationModel() == nullptr)
      {
         // There is no propagation model and the signal is not 'trivial', so there is no chance
         // that diffraction is playing a role in allowing the target to be detected while still
         // being masked. If a masking check has not been performed, do it at this time. Then if
         // the target is masked, this calculation and those for subsequent beams could be bypassed.
         int terrainStatusMask = (WsfEM_Interaction::cRCVR_TERRAIN_MASKING | WsfEM_Interaction::cXMTR_TERRAIN_MASKING);
         if ((aInteraction.mCheckedStatus & terrainStatusMask) == 0)
         {
            aInteraction.MaskedByTerrain();
         }

         if ((aInteraction.mFailedStatus & terrainStatusMask) != 0)
         {
            return 0.0;
         }
      }
   }

   // Map the WSF environment data to ALARM environment data.
   int  land_cover;
   bool water_cover;
   MapEnvironment(xmtrPtr->GetPlatform(), aEnvironment, land_cover, land_form, water_cover);

   // Make sure there is a clutter coefficient for the land_form/land_coverage combination.
   // (see clutter.f90::init_clutter)
   // BOEING-BEG: Added ability to use MIT-LL/SALRAM data tables (it is more complete)
   if (mUseMIT_LL_DataTables)
   {
      mWSF_LandCover = aEnvironment.GetLandCover();
      mWSF_LandForm  = aEnvironment.GetLandFormation();
      mWSF_SeaState  = aEnvironment.GetSeaState();
   }
   // BOEING-END: Added ability to use MIT-LL/SALRAM data tables (it is more complete)
   else if (ilflc[land_cover - 1][land_form - 1] == 0)
   {
      return 0.0;
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

   // NOTE-BOEING
   // If the azimuth increment is zero then it only a single mainbeam sample is performed.
   // In that case the width of the sample is the increment. If the increment is also zero
   // then half the azimuth beamwidth of the receive antenna is used. This will generate
   // about the same result as ALARM if CLUT_AZ_WIDTH = CLUT_ANGLE_INCR = bw/2 is used.
   if (mUseBeamwidthForIncrement)
   {
      double             beamwidth  = 0.0;
      WsfAntennaPattern* patternPtr = rcvrPtr->GetAntennaPattern(xmtrPtr->GetPolarization(), xmtrPtr->GetFrequency());
      if (patternPtr != nullptr)
      {
         beamwidth = patternPtr->GetAzimuthBeamwidth(xmtrPtr->GetFrequency(), 0.0, 0.0);
      }
      if (beamwidth <= 0.0)
      {
         beamwidth = 1.0 * deg2rad;
      }
      az_angle_incr_rad = 0.5 * beamwidth;
      az_angle_incr_deg = az_angle_incr_rad * rad2deg;
   }

   //===========================================================================
   // The following is from radar.f90::init_radar
   //===========================================================================

   double pulse_width = xmtrPtr->GetPulseWidth() * 1.0E+6; // in usec
   double pcr         = xmtrPtr->GetPulseCompressionRatio();
   double frequency   = xmtrPtr->GetFrequency() * 1.0E-6; // in MHz
   double gsubt       = xmtrPtr->GetPeakAntennaGain();
   double gsubr       = xmtrPtr->GetPeakAntennaGain();
   double prfhz       = xmtrPtr->GetPulseRepetitionFrequency();
   double runamb      = 0.0;

   // Initialize some non-cw parameters to stable values.
   if (prfhz <= 0.0)
   {
      pcr         = 1.0;
      runamb      = cw_clutter_bin;
      prfhz       = 0.5 * vlight / runamb;
      pulse_width = 1.0E+6 / prfhz; // * 0.999999d0
   }

   //-------------------------------------------------------------------
   // Convert various input quantities from their original input units
   // to the units suitable for use in the model.
   //-------------------------------------------------------------------

   // tauc = this%pulse_width / this%pcr * 1.0d-06
   // tau = this%pulse_width * 1.0d-06
   double tauc = pulse_width / pcr * 1.0E-6;

   //-------------------------------------------------------------------
   // TAUC - compressed pulse is used for the CTAUO2 and CTAUO4
   // constants which is used for the clutter calculations.  The
   // uncompressed pulse, TAU, is used in the power calculations.
   //-------------------------------------------------------------------

   double ctauo2 = 0.5 * vlight * tauc;
   double ctauo4 = 0.25 * vlight * tauc;

   double rfreq  = frequency * 1.0E+6;
   double txloss = 1.0 / xmtrPtr->GetInternalLoss();
   double syslos = 1.0 / rcvrPtr->GetInternalLoss();

   //-------------------------------------------------------------------
   // Compute the wavelength of the radar.
   //-------------------------------------------------------------------

   double wavelength = vlight / rfreq;

   // Determine unambiguous ranges
   if (prfhz > 0.0)
   {
      double prfrdr = prfhz;
      runamb        = 0.5 * vlight / prfrdr;
   }

   //-------------------------------------------------------------------

   double power  = xmtrPtr->GetPower() * 1.0E+3; // milliwatts
   double pcrfac = pcr;
   double tarcon = power * gsubt * gsubr * txloss * syslos * pow(wavelength, 2) * pcrfac / fpicub;

   //===========================================================================

   mPolarizationVertical = xmtrPtr->GetPolarization() == WsfEM_Types::cPOL_VERTICAL;
   atmosphere atm_data(xmtrPtr);
   double     freqin     = xmtrPtr->GetFrequency() * 1.0E-6; // MHz
   double     radar_proc = aProcessingFactor;
   double     sigclt;
   /*call*/ clutter_signal_comp(ctauo2,
                                ctauo4,
                                freqin,
                                land_cover,
                                radar_proc,
                                slant_range /*ranget*/,
                                rkfact,
                                runamb,
                                rdr_lon /*sitlam*/,
                                rdr_lat /*sitphi*/,
                                tarcon,
                                water_cover,
                                atm_data,
                                rx_ant,
                                tx_ant,
                                sigclt);

   return sigclt * 1.0E-3; // Convert from milliwatts to watts and return
}

// =================================================================================================
void WsfEM_ALARM_Clutter::MapEnvironment(WsfPlatform*    aPlatformPtr,
                                         WsfEnvironment& aEnvironment,
                                         int&            land_cover,
                                         int&            land_form,
                                         bool&           water_cover)
{
   // 'water_cover' is set if the transmitter is on a surface or subsurface vessel.
   WsfSpatialDomain spatialDomain = aPlatformPtr->GetSpatialDomain();
   water_cover = ((spatialDomain == WSF_SPATIAL_DOMAIN_SURFACE) || (spatialDomain == WSF_SPATIAL_DOMAIN_SUBSURFACE));

   land_cover = aEnvironment.GetLandCover();
   switch (land_cover)
   {
   case WsfEnvironment::cGENERAL:
      land_cover = 1; // use urban...
      break;
   case WsfEnvironment::cURBAN:
   case WsfEnvironment::cAGRICULTURAL:
      // no change
      break;
   case WsfEnvironment::cRANGELAND_HERBACEOUS:
   case WsfEnvironment::cRANGELAND_SHRUB:
      land_cover = 3; // rangeland
      break;
   case WsfEnvironment::cFOREST_DECIDUOUS:
   case WsfEnvironment::cFOREST_CONIFEROUS:
   case WsfEnvironment::cFOREST_MIXED:
   case WsfEnvironment::cFOREST_CLEARCUT:
   case WsfEnvironment::cFOREST_BLOCKCUT:
      land_cover = 4; // forest
      break;
   case WsfEnvironment::cWETLAND_FORESTED:
   case WsfEnvironment::cWETLAND_NONFORESTED:
      land_cover = 5; // wetland
      break;
   case WsfEnvironment::cBARREN:
      land_cover = 6;
      break;
   }

   if (water_cover)
   {
      land_cover    = 7; // water
      int sea_state = aEnvironment.GetSeaState();
      sea_state     = std::min(std::max(sea_state, 1), 5);
      land_form     = 9 + sea_state; // [10..14]
   }
   else
   {
      land_form = aEnvironment.GetLandFormation();
      land_form = std::min(std::max(land_form, 1), 9); // [1..9]
   }
}

//---------------------------------------------------------------------
//     Purpose:
//     -------
//     Compute the clutter signal.
//
//     Author:      Mike Sutton  (Division 397)
//     ------
//                    Science Applications
//                  International Corporation
//                            (SAIC)
//
//     Modification Summary:
//     --------------------
//     17-Feb-1999    - Initially generated...
//
//     22-Dec-2000  Added support for the standard deviation of surface
//                  height algorithm. See SPCR 1204. MWS
//     08-Aug-2002  Modified to support radar_proc argument which is
//                  a combination of old eloss and filtr plus new
//                  any new processing.  See SPCR #1255.
//     19-Dec-2003   - Modified to use the new antenna class.
//                     Removed unnecessary variables, ALPHAR, EPSLNR,
//                     HAMMSL, ZTENNA from the argument list.
//                     See SPCR #1310.
//     06-Jul-2004  - Modified to assume that a CW radar clutter is
//                    being computed when the unambiguous range (RUNAMB)
//                    and CTAUO2 are nearly equal.  See SPCR #1307.
//     13-Jan-2006    - Consolidated allocate statements into a single
//                      statement. See SPCR #1357.
//---------------------------------------------------------------------
//!! private

void WsfEM_ALARM_Clutter::clutter_signal_comp(double      ctauo2,
                                              double      ctauo4,
                                              double      freqin,
                                              int         land_cover,
                                              double      radar_proc,
                                              double      ranget,
                                              double      rkfact,
                                              double      runamb,
                                              double      sitlam,
                                              double      sitphi,
                                              double      tarcon,
                                              bool        water_cover,
                                              atmosphere& atm_data,
                                              antenna&    rx_ant,
                                              antenna&    tx_ant,
                                              double&     sigclt)
{
   // use physical_consts, only : pi, twopi
   // use atmosphere_class
   // use antenna_class
   // use terrain_class
   // use error_check_mod, only : fatal_error

   // implicit none

   //-------------------------------------------------------------------
   // Passed parameters
   //-------------------------------------------------------------------

   // type (clutter),      intent(inout) :: this

   // real(8),             intent(in)    :: ctauo2
   // real(8),             intent(in)    :: ctauo4
   // real(8),             intent(in)    :: freqin         // MHz
   // integer,             intent(in)    :: land_cover
   // real(8),             intent(in)    :: radar_proc     // radar processing to clutter
   // real(8),             intent(in)    :: ranget         // meters
   // real(8),             intent(in)    :: rkfact
   // real(8),             intent(in)    :: runamb         // meters
   // real(8),             intent(in)    :: sitlam         // radians
   // real(8),             intent(in)    :: sitphi         // radians
   // real(8),             intent(in)    :: tarcon
   // logical,             intent(in)    :: water_cover
   // type (atmosphere),   intent(in)    :: atm_data
   // type (antenna),      intent(in)    :: rx_ant
   // type (antenna),      intent(in)    :: tx_ant

   // real(8),             intent(inout) :: sigclt         // milli-watts

   //-------------------------------------------------------------------
   // Local variables
   //-------------------------------------------------------------------

   int iaz, i, j, m;
   int jzero;
   int mend;
   int mstart;
   int nazclt;
   int nareas;
   int igatef;
   int igatel;
   int nprofile;

   bool cluttr;
   bool needgf;
   bool needgl;
   bool terrain_sw;
   bool cw_rdr;

   double alphac;
   double atnclt;
   double deltax;
   double deltaz;
   double epslnc;
   double offazc;
   /*NOT-USED double offelc; */
   double plengi;
   double plensm;
   double ratio;
   double rend;
   double rstart;
   double sigmai;
   double sumclt;
   double rminus;
   double rplus;
   double rsubs;
   double rsube;
   double gtbelo;
   double grbelo;
   double dazclr;
   double hammsl_tx;
   double ztenna_tx;
   double offaz_tx;

   const int hunk = 512;

   // moved to header
   // static int                 aprofile = 0;
   // static bool                uninit = true;
   // static std::vector<int>    iend;
   // static std::vector<int>    istart;
   // static std::vector<bool>   visibl;
   // static std::vector<double> elvmsl;
   // static std::vector<double> rngter;     // 0:
   // static std::vector<double> tanepp;
   // static std::vector<double> xprofl;     // 0:
   // static std::vector<double> zprofl;     // 0:
   // BA
   static std::vector<int> lcprofl;
   // BA

   //-------------------------------------------------------------------
   // Initialize SIGCLT, the clutter signal in milliwatts, to zero.
   //-------------------------------------------------------------------

   sigclt = 0.0;

   //-------------------------------------------------------------------
   // Return if the clutter width is less than or equal to zero or if the
   // clutter effects are turned off.
   //-------------------------------------------------------------------

   // NOTE-BOEING: we allow a single azimuth sample... and clutter_sw has no meaning
   //              because if we are here we are enabled.

   // NOT-USED if (! clutter_sw || az_max_angle_rad <= 0.0) return;

   //-------------------------------------------------------------------
   // Determine the clutter signal level.
   //-------------------------------------------------------------------

   alphac = 0.0;

   nazclt     = patch_count;
   dazclr     = az_angle_incr_rad;
   terrain_sw = WsfEM_ALARM_Terrain::get_terrain_sw();
   hammsl_tx  = get_height_msl(tx_ant);
   ztenna_tx  = get_height_agl(tx_ant);
   offaz_tx   = get_az_point_ang(tx_ant);
   cw_rdr     = (fabs(runamb - ctauo2) < 1.e-6);

   //-------------------------------------------------------------------
   // Check to see if more space needs allocated for terrain profile
   // arrays.
   //-------------------------------------------------------------------
   nprofile = WsfEM_ALARM_Terrain::get_nprofile(max_range);
   i        = nprofile;

   if (i > aprofile || uninit)
   {
      uninit = false;
      //----------------------------------------------------------------
      // Allocate arrays.  Memory is allocated in hunks to minimize
      // allocate/deallocate thrashing if nprofile is gradually creeping
      // up.
      //----------------------------------------------------------------

      aprofile = aprofile + (std::abs(i / hunk) + 1) * hunk;

      iend.resize(aprofile + 1);
      istart.resize(aprofile + 1);
      visibl.resize(aprofile + 1);
      elvmsl.resize(aprofile + 1);
      rngter.resize(aprofile + 1);
      tanepp.resize(aprofile + 1);
      xprofl.resize(aprofile + 1);
      zprofl.resize(aprofile + 1);
      // BA
      lcprofl.resize(aprofile + 1);
      // BA

      // allocate(iend(aprofile),     istart(aprofile), &
      //      &   visibl(aprofile),   elvmsl(aprofile),&
      //      &   rngter(0:aprofile), tanepp(aprofile), &
      //      &   xprofl(0:aprofile), zprofl(0:aprofile), stat = j)

      // if (j != 0)
      //    /*call*/ fatal_error('clutter_signal_clut', &
      //         & 'Can''t allocate memory')
      // end if
   } // end if

   //-------------------------------------------------------------------
   // Load a single profile for round smooth earth.
   //-------------------------------------------------------------------
   if (!terrain_sw)
   {
      wsf::Terrain terrain(mSimulationPtr->GetTerrainInterface());

      /*call*/ WsfEM_ALARM_Terrain::visclt(terrain,
                                           mSimulationPtr->GetScenario().GetEnvironment(),
                                           alphac,
                                           hammsl_tx,
                                           nprofile,
                                           rkfact,
                                           sitlam,
                                           sitphi,
                                           elvmsl,
                                           iend,
                                           istart,
                                           nareas,
                                           rngter,
                                           tanepp,
                                           visibl,
                                           xprofl,
                                           zprofl,
                                           lcprofl);
   }

   //-------------------------------------------------------------------
   // Initialize SUMCLT, in milliwatts, the raw clutter sum, to zero.
   // Set RSTART, in meters, the minimum range at which clutter
   // computations will be made, to ZTENNA, the height of the radar
   // antenna.  Determine MSTART, the index of the first ambiguous
   // range cell in which clutter will be considered.
   //-------------------------------------------------------------------

   sumclt = 0.0;
   rstart = ztenna_tx;
   ratio  = (rstart - ranget + ctauo4) / runamb;
   mstart = int(ratio);

   if (dble(mstart) <= ratio)
   {
      mstart = mstart + 1;
   }

   //-------------------------------------------------------------------
   // Determine the clutter contribution along each radial around the
   // radar antenna boresight.
   //-------------------------------------------------------------------

   // loop_iaz: do iaz = -nazclt, nazclt
   /*loop_iaz:*/ for (iaz = -nazclt; iaz <= nazclt; ++iaz)
   {
      //----------------------------------------------------------------
      // Determine OFFAZC, in radians, the off-boresight angle from the
      // radar antenna boresight to the IAZ-th radial.  Determine
      // ALPHAC, the azimuth angle with respect to north [-pi,pi] of the
      // clutter patch.
      //----------------------------------------------------------------

      offazc = dble(iaz) * dazclr;
      alphac = offaz_tx + offazc;

      if (alphac > pi)
      {
         alphac = twopi - alphac;
      }
      else if (alphac < -pi)
      {
         alphac = twopi + alphac;
      } // end_if

      //----------------------------------------------------------------
      // Load a profile for this azimuth.
      //----------------------------------------------------------------
      if (terrain_sw)
      {
         wsf::Terrain terrain(mSimulationPtr->GetTerrainInterface());
         /*call*/ WsfEM_ALARM_Terrain::visclt(terrain,
                                              mSimulationPtr->GetScenario().GetEnvironment(),
                                              alphac,
                                              hammsl_tx,
                                              nprofile,
                                              rkfact,
                                              sitlam,
                                              sitphi,
                                              elvmsl,
                                              iend,
                                              istart,
                                              nareas,
                                              rngter,
                                              tanepp,
                                              visibl,
                                              xprofl,
                                              zprofl,
                                              lcprofl);
      }

      deltax = xprofl[iend[nareas]];
      deltaz = zprofl[iend[nareas]] - hammsl_tx;
      rend   = sqrt(deltax * deltax + deltaz * deltaz);
      ratio  = (rend - ranget - ctauo4) / runamb;
      mend   = int(ratio);

      jzero = 1;

      if (LookSummaryEnabled())
      {
         mLookSummary.mNumRanges = mend - mstart + 1;
      }

      // loop_m: do m = mstart, mend
      /*loop_m:*/ for (m = mstart; m <= mend; ++m)
      {
         rminus = ranget + dble(m) * runamb - ctauo4;
         rplus  = ranget + dble(m) * runamb + ctauo4;
         // for CW make the range cover the whole unambig range cell.
         if (cw_rdr)
         {
            rminus = dble(int(rminus / runamb)) * runamb;
            rplus  = dble(int(rplus / runamb)) * runamb;
         } // end_if

         if (LookSummaryEnabled())
         {
            int rangeIndex                     = m - mstart;
            mLookSummary.mMinRange[rangeIndex] = rminus;
            mLookSummary.mMaxRange[rangeIndex] = rplus;
         }

         // loop_j: do j = jzero, nareas
         for (j = jzero; j <= nareas; ++j)
         {
            igatel = 0; // Prevent Linux compile warning

            cluttr = false;
            needgf = false;
            needgl = false;

            rsubs = rngter[istart[j]];
            rsube = rngter[iend[j]];

            if (rsubs <= rminus)
            {
               if (rsube >= rplus)
               {
                  cluttr = true;
                  needgf = true;
                  needgl = true;
               }
               else if (rsube > rminus)
               {
                  cluttr = true;
                  needgf = true;
                  igatel = iend[j] - 1;
               } // end_if
            }
            else if (rsubs < rplus)
            {
               if (rsube < rplus)
               {
                  cluttr = true;
                  igatef = istart[j];
                  igatel = iend[j] - 1;
               }
               else
               {
                  cluttr = true;
                  needgl = true;
                  igatef = istart[j];
               } // end if
            }
            else
            {
               jzero = j;
               break; // exit loop_j // do loop
            }

            if (cluttr)
            {
               if (needgf)
               {
                  igatef = iend[j];
                  // do i = istart[j], iend[j]
                  for (i = istart[j]; i <= iend[j]; ++i)
                  {
                     if (rngter[i] > rminus)
                     {
                        igatef = i - 1;
                        break; // exit // do loop
                     }         // end if

                  } // end do
               }    // end if


               if (needgl)
               {
                  igatel = istart[j];
                  // do i = iend[j] , istart[j] , -1
                  for (i = iend[j]; i >= istart[j]; --i)
                  {
                     if (rngter[i] <= rplus)
                     {
                        igatel = i;
                        break; // exit // do loop
                     }         // end if

                  } // end do
               }    // end if


               //-------------------------------------------------------
               // Increment through each terrain patch within the
               // clutter cell.
               //-------------------------------------------------------

               plensm = 0.0;

               // loop_i: do i = max(igatef, 1), igatel
               /*loop_i:*/ for (i = std::max(igatef, 1); i <= igatel; ++i)
               {
                  //----------------------------------------------------
                  // Determine the "pulse length" of the I-th terrain
                  // patch, while insuring the sum of the "pulse
                  // length" of all terrain patchs is not greater
                  // than the length of the clutter cell.
                  //----------------------------------------------------

                  deltax = xprofl[i + 1] - xprofl[i];
                  deltaz = zprofl[i + 1] - zprofl[i];
                  plengi = sqrt(deltax * deltax + deltaz * deltaz);

                  plensm = plensm + plengi;

                  if (plensm > ctauo2)
                  {
                     plengi = plengi - (plensm - ctauo2);
                  } // end_if


                  //----------------------------------------------------
                  // Determine the clutter contribution from the
                  // I-th terrain patch, only if its associated
                  // "pulse length" is positive.
                  //----------------------------------------------------

                  if (plengi <= 0.0)
                  {
                     continue;
                  } // cycle;  // to next do loop iteration

                  //----------------------------------------------------
                  // Determine the elevation angle to the I-th
                  // terrain patch.
                  //----------------------------------------------------

                  epslnc = atan(tanepp[i]);

                  //----------------------------------------------------
                  // Determine the radar's transmit and receive
                  // antenna gains in the direction of the I-th
                  // terrain patch.
                  //----------------------------------------------------

                  /*call*/ get_relative_gain(rx_ant, alphac, epslnc, grbelo);
                  /*call*/ get_relative_gain(tx_ant, alphac, epslnc, gtbelo);

                  // BA
                  if (terrain_sw && mUseMIT_LL_DataTables)
                  {
                     mWSF_LandCover = static_cast<WsfEnvironment::LandCover>(
                        lcprofl[i]); // used for MIT-LL calculations in get_reflectivity.
                  }
                  // todo land form
                  // BA

                  sigmai = get_reflectivity(/*this,*/ elvmsl,
                                            freqin,
                                            ztenna_tx,
                                            i,
                                            land_cover,
                                            rkfact,
                                            rngter,
                                            water_cover,
                                            xprofl,
                                            zprofl,
                                            epslnc); // mjm Added elevation angle parameter and land cover parameter

                  //----------------------------------------------------
                  // Determine atmospheric attenuation to the I-th
                  // terrain patch.
                  //----------------------------------------------------

                  atnclt = attenuation(atm_data, epslnc, freqin, rngter[i], rkfact);

                  //----------------------------------------------------
                  // Sum the clutter signal power of the I-th terrain
                  // patch with the previously computed terrain
                  // patches.
                  //----------------------------------------------------

                  sumclt = sumclt + gtbelo * grbelo * plengi * sigmai * atnclt * atnclt / pow(rngter[i], 3);

               } // end do loop_i

            } // end if // cluttr

         } // end do loop_j

      } // end do loop_m

   } // end do loop_iaz

   //-------------------------------------------------------------------
   // Determine the clutter signal level based on the radar constant for
   // targets (including clutter), the azimuth angle increment used to
   // compute clutter, the raw clutter sum, and the radar response to
   // clutter.
   //-------------------------------------------------------------------

   sigclt = tarcon * dazclr * sumclt * radar_proc;
   if (DebugEnabled())
   {
      auto logger = ut::log::debug();
      logger.GetStream().precision(16);
      logger << "Sigclt: " << sigclt / radar_proc;
   }
}

//---------------------------------------------------------------------
//     Purpose:
//     -------
//     Determine the clutter reflectivity
//
//     NOTES:
//     -----
//     THIS is passed with intent(inout) so that the calls to random in
//     MITSIG and CNASIG can change the seed value.
//
//     Author:      Mike Sutton  (Division 397)
//     ------
//                    Science Applications
//                  International Corporation
//                            (SAIC)
//
//     Modification Summary:
//     --------------------
//     16-Jun-1998    - Initially generated...  This routine was
//                      extracted from the former CLUTPU and CLUTPD
//                      routines. MWS
//
//     - Incorporated the NUME clutter reflectivity model.  It is the
//       statistic type of NUMEmerical. MWS
//
//     22-Dec-2000  Added support for the standard deviation of surface
//                  height algorithm. See SPCR 1204. MWS
//---------------------------------------------------------------------
//!! public

double WsfEM_ALARM_Clutter::get_reflectivity(const std::vector<double>& elvmsl,
                                             double                     frequency,
                                             double                     radar_height,
                                             int                        iprofl,
                                             int                        land_cover,
                                             double                     rkfact,
                                             const std::vector<double>& rngter,
                                             bool                       water_cover,
                                             const std::vector<double>& xprofl,
                                             const std::vector<double>& zprofl,
                                             double epslnc) // mjm added elevation angle as parameter
{
   // use physical_consts, only : rezero, halfpi
   // use random_number_mod, only : uniform_random

   // implicit none

   //-------------------------------------------------------------------
   // Passed parameters
   //-------------------------------------------------------------------

   // type (clutter), intent(inout) :: this
   // real(8),        intent(in)    :: elvmsl(*)        // meters
   // real(8),        intent(in)    :: frequency        // MHz
   // real(8),        intent(in)    :: radar_height     // meters MSL
   // integer,        intent(in)    :: iprofl
   // integer,        intent(in)    :: land_cover
   // real(8),        intent(in)    :: rkfact
   // real(8),        intent(in)    :: rngter(0:*)      // meters
   // logical,        intent(in)    :: water_cover
   // real(8),        intent(in)    :: xprofl(0:*)      // meters
   // real(8),        intent(in)    :: zprofl(0:*)      // meters

   //-------------------------------------------------------------------
   // Local variables
   //-------------------------------------------------------------------

   double delhr, twore, graze1, slope, grazei, graze;

   int i;

   double sigmai = 0.0;

   if (!clutter_sw)
   {
      return sigmai;
   }

   //-------------------------------------------------------------------
   // Determine the clutter reflectivity of the I-th
   // terrain patch.
   //-------------------------------------------------------------------

   // BOEING-BEG: Added ability to use MIT-LL/SALRAM data tables (it is more complete)
   if (mUseMIT_LL_DataTables)
   {
      WsfEM_Types::Polarization polarization = WsfEM_Types::cPOL_DEFAULT;
      if (mPolarizationVertical)
      {
         polarization = WsfEM_Types::cPOL_VERTICAL;
      }

      i = iprofl;
      if (!water_cover)
      {
         // Depression angle is simply negative elevation.
         double depressionAngle = -epslnc;
         // double range = rngter[i];
         if (mUseSALRAM_DataTables)
         {
            sigmai = WsfMIT_ClutterStrength::GetLandClutterStrengthLegacy(depressionAngle,
                                                                          mWSF_LandForm,
                                                                          mWSF_LandCover,
                                                                          frequency * 1.0e+6,
                                                                          polarization);
         }
         else
         {
            sigmai = WsfMIT_ClutterStrength::GetLandClutterStrength(depressionAngle,
                                                                    mWSF_LandForm,
                                                                    mWSF_LandCover,
                                                                    frequency * 1.0e+6,
                                                                    polarization);
         }
      }
      else // water cover
      {
         // Compute grazing angle (copied from water_cover code below, with no comments.
         i      = iprofl;
         delhr  = radar_height - elvmsl[i];
         twore  = 2.0 * rkfact * rezero; // twice effective earth radius
         graze1 = asin((delhr / rngter[i]) * (1.0 + delhr / twore) - rngter[i] / twore);
         slope  = (elvmsl[i + 1] - elvmsl[i]) / (xprofl[i + 1] - xprofl[i]);
         grazei = graze1 + atan(slope);
         graze  = std::min(std::max(grazei, 0.0), halfpi);

         sigmai = WsfMIT_ClutterStrength::GetSeaClutterStrength(graze, mWSF_SeaState, frequency * 1.0e+6, polarization);
      }
      sigmai = pow(10.0, sigmai * 0.1);
   }
   // BOEING-END: Added ability to use MIT-LL/SALRAM data tables (it is more complete)
   else if (statistic_opt == stat_numerical)
   {
      sigmai = reflectivity + (dble(uniform_random(0)) * reflectivity_delta);
      sigmai = pow(10.0, sigmai * 0.1);
   }
   else if (!water_cover)
   {
      //----------------------------------------------------------------
      // Land clutter reflectivity will be determined.
      //----------------------------------------------------------------

      sigmai = mitsig(land_cover);
   }
   else // water cover
   {
      //----------------------------------------------------------------
      // Sea clutter reflectivity will be determined.  Determine GRAZE,
      // the grazing angle at the I-th terrain patch.  Compute the
      // intermediate grazing angle that accounts for curvature of the
      // earth and height differences of the radar and terrain, but not
      // for the slope of the terrain.
      //----------------------------------------------------------------

      i     = iprofl;
      delhr = radar_height - elvmsl[i];
      twore = 2.0 * rkfact * rezero; // twice effective earth radius

      graze1 = asin((delhr / rngter[i]) * (1.0 + delhr / twore) - rngter[i] / twore);

      //----------------------------------------------------------------
      // Compute the slope of the terrain being illuminated.  This angle
      // is added to GRAZE1 to give the true grazing angle GRAZEI.
      //----------------------------------------------------------------

      slope = (elvmsl[i + 1] - elvmsl[i]) / (xprofl[i + 1] - xprofl[i]);

      grazei = graze1 + atan(slope);

      //----------------------------------------------------------------
      // Handle the special case where the grazing angle is greater than
      // 90 degrees which may happen because of the slope of the terrain
      // near the radar.  The assumption is that if the grazing angle is
      // greater than 90 degrees, it is set to 90 degrees.  This should
      // have no affect on model accuracy, as the only time this occurs
      // is when the target is right over the radar sight and the
      // terrain directly beneath the radar is the clutter contributer.
      // Terrain this close will be eclipsed by the transmission pulse.
      // Also, limit the grazing angle to positive angles.
      //----------------------------------------------------------------

      graze = std::min(std::max(grazei, 0.0), halfpi);

      sigmai = cnasig(frequency, graze);
   }


   return sigmai;
}

//---------------------------------------------------------------------
//     Purpose:
//     -------
//     Determines the clutter coefficient from data tabulated in CMT
//     Project Memo 'Tabulated Radar Ground Clutter Amplitude
//     Statistics by Terrain Classification', by the Massachusetts
//     Institute of Technology Lincoln Laboratory.
//
//     Three options are currently supplied by this
//     subroutine.  The first option supplies the mean clutter coef-
//     ficient.  The second option determines the clutter coefficient
//     from a statistical distribution.  The third option supplies the
//     maximum clutter coefficient.
//
//
//     Inputs:
//     ------
//     LAND_COVER  -  The land cover index.
//
//                LAND_COVER            Land Cover
//                ------           ------------
//                  1              Urban
//                  2              Agricultural
//                  3              Rangeland
//                  4              Forest
//                  5              Wetland
//                  6              Barren
//                  7              Water
//
//     LAND_FORM   -  The land form and sea state indicator.
//
//                LAND_FORM             Land Form / Sea State
//                -----             ---------------------
//                  1               Level
//                  2               Inclined
//                  3               Undulating
//                  4               Rolling
//                  5               Hummocky
//                  6               Rigid
//                  7               Moderately Steep
//                  8               Steep
//                  9               Broken
//                 10               Sea State 1 (Calm, Rippled)
//                 11               Sea State 2 (Smooth, Wavelets)
//                 12               Sea State 3 (Slight)
//                 13               Sea State 4 (Moderate)
//                 14               Sea State 5+ (Rough+)
//
//
//     STATISTIC_OPT  -  The clutter option indicator.  If STATISTIC_OPT = 1,
//                the mean value of the clutter coefficient is
//                determined.  If STATISTIC_OPT = 2, the clutter
//                coefficient is drawn from a statistical
//                distribution.  If STATISTIC_OPT = 3, worst case clutter
//                is assumed.  STATISTIC_OPT = 4, is an invalid clutter
//                option for terrain.
//
//     ISEED   -  The random number generator seed value
//                (used when STATISTIC_OPT = 2).
//
//
//     Outputs:
//     -------
//     SIGMA   -  The clutter coefficient (square meters /
//                square meters)
//
//
//     Notes:
//     -----
//     THIS is passed with intent(inout) so that the call to random can
//     change the seed value.
//
//     If J = LAND_FORM and K = LAND_COVER then the array SLFLC
//     is as follows:
//
//        SLFLC(1,J,K)  -  The 50 percentile clutter
//                         coefficient value.
//
//        SLFLC(2,J,K)  -  The 90 percentile clutter
//                         coefficient value.
//
//        SLFLC(3,J,K)  -  The 99 percentile clutter
//                         coefficient value.
//
//        SLFLC(4,J,K)  -  The maximum observed value of the
//                         clutter coefficient.
//
//        SLFLC(5,J,K)  -  The mean clutter coefficient.
//
//
//     Author:
//     ------
//     Tom Goodman, Division 397
//     Science Applications International Corporation (SAIC)
//     2109 Air Park Road, S.E.
//     Albuquerque, New Mexico 87106
//     Telephone (505) 247-8787
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
//     - Checking for valid LFORM / LCOVER pairs is now
//       performed in subroutine RFLERR.FOR.  PJH
//
//     - New range of values for LCOVER (7 = Water) and
//       LFORM (10 ~ 14 = Sea State 1 ~ 5+, respectively).
//       This change is in support of the addition of
//       clutter reflectivity values based on sea state.
//       The clutter reflectivity value is determined in
//       subroutine CNASIG.  JCL
//
//     - New clutter option (ICLOPT = 3).  If ICLOPT = 3,
//       the worst case clutter (SLFLC(4,LFORM,LCOVER))is
//       used to determine the coefficient value.  JCL
//
//
//     ALARM 3.2:
//     ---------
//     - RFLCOM common block removed.  Those variables (LCOVER, LFORM,
//       ICLOPT, ISEED) previously accessed through the common block are
//       now passed through the MITSIG argument list.  (SPCR-1057, JCL)
//
//     - Modified subroutine to utilize the "Common" random number
//       function RANDOM.  (SPCR-1050, JCL)
//
//     ALARM 4.x:
//     ---------
//     - Converted to Fortran90 and a function. MWS
//
//     - Incorporated into the clutter class. MWS
//
//     22-Dec-2000  Added support for the standard deviation of surface
//                  height algorithm. See SPCR 1204. MWS
//---------------------------------------------------------------------
//!! private

double WsfEM_ALARM_Clutter::mitsig(int land_cover)
{
   // use random_number_mod, only : uniform_random

   // implicit none

   //-------------------------------------------------------------------
   // Passed Parameters
   //-------------------------------------------------------------------

   // type (clutter), intent(inout) :: this
   // integer,        intent(in)    :: land_cover

   //-------------------------------------------------------------------
   // Local variables
   //-------------------------------------------------------------------

   double dprob, sig1, sig2, sig3, sig4, sig5, sigl, /* slflc(5,9,6),
         slflc1(5,9,3), slflc2(5,9,3),*/
      slope, urn;

   double sigma;

   float rndnum;

   // Indices reverse for C++ and added elements for index 0 to facilitate 1-based indexing.
   static const double slflc[6 + 1][9 + 1][5 + 1] = {{
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, // not used
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, // not used
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, // not used
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, // not used
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, // not used
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, // not used
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, // not used
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, // not used
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, // not used
                                                     },
                                                     {
                                                        // land_cover = 1
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},     // not used
                                                        {0.0, 39.0, 18.0, 5.0, -8.0, 18.0}, // land_form = 1
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},     // land_form = 2
                                                        {0.0, 39.0, 22.0, 8.0, -3.0, 21.0}, // land_form = 3
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},     // land_form = 4
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},     // land_form = 5
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},     // land_form = 6
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},     // land_form = 7
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},     // land_form = 8
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0}      // land_form = 9
                                                     },
                                                     {
                                                        // land_cover = 2
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},      // not used
                                                        {0.0, 46.0, 32.0, 16.0, -5.0, 28.0}, // land_form = 1
                                                        {0.0, 43.0, 26.0, 14.0, 0.0, 27.0},  // land_form = 2
                                                        {0.0, 42.0, 28.0, 18.0, 0.0, 30.0},  // land_form = 3
                                                        {0.0, 37.0, 27.0, 19.0, 1.0, 29.0},  // land_form = 4
                                                        {0.0, 40.0, 24.0, 16.0, 6.0, 28.0},  // land_form = 5
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},      // land_form = 6
                                                        {0.0, 37.0, 21.0, 12.0, 3.0, 24.0},  // land_form = 7
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},      // land_form = 8
                                                        {0.0, 37.0, 23.0, 15.0, 5.0, 26.0}   // land_form = 9
                                                     },
                                                     {
                                                        // land_cover = 3
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},      // not used
                                                        {0.0, 40.0, 25.0, 13.0, 4.0, 27.0},  // land_form = 1
                                                        {0.0, 32.0, 24.0, 19.0, 11.0, 28.0}, // land_form = 2
                                                        {0.0, 47.0, 29.0, 20.0, 5.0, 32.0},  // land_form = 3
                                                        {0.0, 49.0, 30.0, 25.0, 20.0, 35.0}, // land_form = 4
                                                        {0.0, 44.0, 33.0, 25.0, 16.0, 36.0}, // land_form = 5
                                                        {0.0, 35.0, 29.0, 21.0, 18.0, 32.0}, // land_form = 6
                                                        {0.0, 33.0, 26.0, 21.0, 8.0, 29.0},  // land_form = 7
                                                        {0.0, 33.0, 15.0, 3.0, 0.0, 16.0},   // land_form = 8
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0}       // land_form = 9
                                                     },
                                                     {
                                                        // land_cover = 4
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},      // not used
                                                        {0.0, 41.0, 26.0, 19.0, 13.0, 30.0}, // land_form = 1
                                                        {0.0, 32.0, 25.0, 20.0, 9.0, 29.0},  // land_form = 2
                                                        {0.0, 37.0, 26.0, 17.0, -3.0, 28.0}, // land_form = 3
                                                        {0.0, 34.0, 23.0, 18.0, 10.0, 28.0}, // land_form = 4
                                                        {0.0, 44.0, 26.0, 17.0, 4.0, 29.0},  // land_form = 5
                                                        {0.0, 27.0, 16.0, 11.0, 5.0, 21.0},  // land_form = 6
                                                        {0.0, 31.0, 24.0, 19.0, 10.0, 28.0}, // land_form = 7
                                                        {0.0, 24.0, 18.0, 14.0, 9.0, 22.0},  // land_form = 8
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0}       // land_form = 9
                                                     },
                                                     {
                                                        // land_cover = 5
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},      // not used
                                                        {0.0, 42.0, 27.0, 19.0, 7.0, 30.0},  // land_form = 1
                                                        {0.0, 42.0, 30.0, 24.0, 17.0, 34.0}, // land_form = 2
                                                        {0.0, 38.0, 28.0, 24.0, 14.0, 32.0}, // land_form = 3
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},      // land_form = 4
                                                        {0.0, 56.0, 47.0, 37.0, 28.0, 49.0}, // land_form = 5
                                                        {0.0, 56.0, 30.0, 19.0, 12.0, 33.0}, // land_form = 6
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},      // land_form = 7
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},      // land_form = 8
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},      // land_form = 9
                                                     },
                                                     {
                                                        // land_cover = 6
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},      // not used
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},      // land_form = 1
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},      // land_form = 2
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},      // land_form = 3
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},      // land_form = 4
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},      // land_form = 5
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},      // land_form = 6
                                                        {0.0, 33.0, 26.0, 19.0, 16.0, 29.0}, // land_form = 7
                                                        {0.0, 24.0, 18.0, 15.0, 11.0, 22.0}, // land_form = 8
                                                        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0}       // land_form = 9
                                                     }};

#if 0
   equivalence(slflc1(1, 1, 1), slflc(1, 1, 1))
   equivalence(slflc2(1, 1, 1), slflc(1, 1, 4))

   data   slflc1&
   & / 39.0, 18.0,  5.0, -8.0, 18.0,  0.0,  0.0,  0.0,  0.0,  0.0, &
   &   39.0, 22.0,  8.0, -3.0, 21.0,  0.0,  0.0,  0.0,  0.0,  0.0, &
   &    0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, &
   &    0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, &
   &    0.0,  0.0,  0.0,  0.0,  0.0, 46.0, 32.0, 16.0, -5.0, 28.0, &
   &   43.0, 26.0, 14.0,  0.0, 27.0, 42.0, 28.0, 18.0,  0.0, 30.0, &
   &   37.0, 27.0, 19.0,  1.0, 29.0, 40.0, 24.0, 16.0,  6.0, 28.0, &
   &    0.0,  0.0,  0.0,  0.0,  0.0, 37.0, 21.0, 12.0,  3.0, 24.0, &
   &    0.0,  0.0,  0.0,  0.0,  0.0, 37.0, 23.0, 15.0,  5.0, 26.0, &
   &   40.0, 25.0, 13.0,  4.0, 27.0, 32.0, 24.0, 19.0, 11.0, 28.0, &
   &   47.0, 29.0, 20.0,  5.0, 32.0, 49.0, 30.0, 25.0, 20.0, 35.0, &
   &   44.0, 33.0, 25.0, 16.0, 36.0, 35.0, 29.0, 21.0, 18.0, 32.0, &
   &   33.0, 26.0, 21.0,  8.0, 29.0, 33.0, 15.0,  3.0,  0.0, 16.0, &
   &    0.0,  0.0,  0.0,  0.0,  0.0 /

   data   slflc2&
   & / 41.0, 26.0, 19.0, 13.0, 30.0, &
   &   32.0, 25.0, 20.0,  9.0, 29.0, 37.0, 26.0, 17.0, -3.0, 28.0, &
   &   34.0, 23.0, 18.0, 10.0, 28.0, 44.0, 26.0, 17.0,  4.0, 29.0, &
   &   27.0, 16.0, 11.0,  5.0, 21.0, 31.0, 24.0, 19.0, 10.0, 28.0, &
   &   24.0, 18.0, 14.0,  9.0, 22.0,  0.0,  0.0,  0.0,  0.0,  0.0, &
   &   42.0, 27.0, 19.0,  7.0, 30.0, 42.0, 30.0, 24.0, 17.0, 34.0, &
   &   38.0, 28.0, 24.0, 14.0, 32.0,  0.0,  0.0,  0.0,  0.0,  0.0, &
   &   56.0, 47.0, 37.0, 28.0, 49.0, 56.0, 30.0, 19.0, 12.0, 33.0, &
   &    0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, &
   &    0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, &
   &    0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, &
   &    0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, &
   &    0.0,  0.0,  0.0,  0.0,  0.0, 33.0, 26.0, 19.0, 16.0, 29.0, &
   &   24.0, 18.0, 15.0, 11.0, 22.0,  0.0,  0.0,  0.0,  0.0,  0.0 /
#endif

   sigma = 0.0; // Prevent Linux compiler warning

   //-------------------------------------------------------------------
   // Check the clutter coefficient option desired.
   //-------------------------------------------------------------------

   if (statistic_opt == stat_mean)
   {
      //----------------------------------------------------------------
      // Determine the mean clutter coefficient.
      //----------------------------------------------------------------

      sig5  = slflc[land_cover][land_form][5];
      sigma = pow(10.0, -0.1 * sig5);
   }
   else if (statistic_opt == stat_stat)
   {
      //----------------------------------------------------------------
      // Determine the clutter coefficient from a statistical
      // distribution.
      //----------------------------------------------------------------

      sig1 = slflc[land_cover][land_form][1];
      sig2 = slflc[land_cover][land_form][2];
      sig3 = slflc[land_cover][land_form][3];
      sig4 = slflc[land_cover][land_form][4];
      sig5 = slflc[land_cover][land_form][5];

      rndnum = uniform_random(1);
      urn    = dble(rndnum);

      if (urn < 0.01)
      {
         sigl  = sig1 + sig5 - sig4;
         slope = 100.0 * (sig4 - sig3);
         dprob = urn;
      }
      else if (urn < 0.1)
      {
         sigl  = sig1 + sig5 - sig3;
         slope = (sig3 - sig2) / 0.09;
         dprob = urn - 0.01;
      }
      else if (urn < 0.5)
      {
         sigl  = sig1 + sig5 - sig2;
         slope = 2.5 * (sig2 - sig5);
         dprob = urn - 0.1;
      }
      else if (urn < 0.9)
      {
         sigl  = sig1;
         slope = 2.5 * (sig2 - sig1);
         dprob = urn - 0.5;
      }
      else if (urn < 0.99)
      {
         sigl  = sig2;
         slope = (sig3 - sig2) / 0.09;
         dprob = urn - 0.9;
      }
      else
      {
         sigl  = sig3;
         slope = (sig4 - sig3) / 0.99;
         dprob = urn - 0.99;
      }

      sigma = pow(10.0, -0.1 * (sigl + slope * dprob));
   }
   else if (statistic_opt == stat_max)
   {
      //----------------------------------------------------------------
      // Determine the, maximum reflectivity, clutter coefficient.
      //----------------------------------------------------------------

      sig4  = slflc[land_cover][land_form][4];
      sigma = pow(10.0, -0.1 * sig4);
   }

   return sigma; // mitsig = sigma
}

//---------------------------------------------------------------------
//     Purpose:
//     -------
//     Determines the sea clutter coefficient.
//
//     This subroutine was obtained from the following thesis:
//     IMPROVEMENTS TO THE ADVANCED LOW ALTITUDE RADAR MODEL
//     (ALARM 91), Terry D. Mosher, Naval Postgraduate School,
//     September 1992.  Requests for this document must be referred to
//     Superintendent, Code 043, Naval Postgraduate School, Monterey,
//     CA 93943-5000 via the Defense Technical Information Center,
//     Cameron Station, Alexandria, VA 22304-6145.
//
//     The actual clutter model was obtained from the following
//     report:  CENTER FOR NAVAL ANALYSIS RESEARCH MEMORANDUM 90-139,
//     Radar-Clutter Modeling for Functional Radar Simulations (U),
//     by George V. Cox, August 1990, SECRET.
//
//
//     NOTES:
//     ------
//     THIS is passed with intent(inout) so that the call to random can
//     change the seed value.
//
//     STATISTIC_OPT  -  The clutter option indicator.  If STATISTIC_OPT = 1,
//                the mean value of the clutter coefficient is
//                determined.  If STATISTIC_OPT = 2, the clutter
//                coefficient is drawn from a statistical
//                distribution. If STATISTIC_OPT = 3, worst case clutter
//                is assumed and if STATISTIC_OPT = 4, best case clutter
//                is returned.
//
//     LCOVER  -  The land cover index.
//
//                  LCOVER            Land Cover
//                  ------           ------------
//                    1              Urban
//                    2              Agricultural
//                    3              Rangeland
//                    4              Forest
//                    5              Wetland
//                    6              Barren
//                    7              Water
//
//     LAND_FORM   -  The land form and sea state indicator.
//
//                  LAND_FORM             Land Form / Sea State
//                  -----             ---------------------
//                    1               Level
//                    2               Inclined
//                    3               Undulating
//                    4               Rolling
//                    5               Hummocky
//                    6               Rigid
//                    7               Moderately Steep
//                    8               Steep
//                    9               Broken
//                   10               Sea State 1 (Calm, Rippled)
//                   11               Sea State 2 (Smooth, Wavelets)
//                   12               Sea State 3 (Slight)
//                   13               Sea State 4 (Moderate)
//                   14               Sea State 5+ (Rough+)
//
//
//                 Sea State                   Percentile
//
//                     1                         0 - 65
//                     2                        65 - 85
//                     3                        85 - 90
//                     4                        90 - 95
//                     5+                       95 - 100
//
//
//     Author:
//     ------
//     LT Terry D. Mosher, USN
//     Naval Postgraduate School
//     Monterey, CA
//
//
//     References:
//     ----------
//     [1] Radar-Clutter Modeling for Functional Radar
//         Simulations, Gregory V. Cox, Aug 1990
//         Center for Naval Analysis
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
//     - Subroutine modified for incorporation into
//       ALARM 3.0.  PJH & JCL
//
//     - Subroutine now only calculates sea clutter
//       coefficient based on selected LCOVER and LFORM
//       options.  JCL
//
//     - Subroutine now calls external function RAN.  JCL
//
//
//     ALARM 3.2:
//     ---------
//     - Constants PI, HALFPI and DEGRAD is calculated locally rather
//       than referenced through the CONSTR common block.
//       (SPCR-1057, JCL)
//
//     - Common block CONSTR removed.  (SPCR-1057, JCL)
//
//     - RFLCOM common block removed.  Those variables (LCOVER, LFORM,
//       ICLOPT, ISEED) previously accessed through the common block are
//       now passed through the CNASIG argument list.  (SPCR-1057, JCL)
//
//     - Corrected erroneous clutter reflectivity data in DATA SIGMA on
//       lines 5 and 7.  The value 92 was changed to 82 to match the
//       NPS (Naval Postgraduate School) documentation rather than the
//       source code.  (SPCR-1008, JCL)
//
//     - Corrected minimum offset in the calculation of SIGMAI when
//       the clutter is uniformly distributed between the maximum and
//       minimum values for the given "land" form, grazing angle, and
//       frequency (option:2).  (SPCR-1010, JCL)
//
//     - Modified subroutine to utilize the "Common" random number
//       function RANDOM.  (SPCR-1050, JCL)
//
//     - Made the SIGMAI assignment near the end of routine a double
//     precision constant. MWS
//
//     ALARM 4.0:
//     ---------
//     - Converted to Fortran90 and a function. MWS
//
//     - Incorporated into the clutter class. MWS
//---------------------------------------------------------------------
//!! private

double WsfEM_ALARM_Clutter::cnasig(double freqin, double graze)
{
   // use physical_consts, only : halfpi, deg2rad
   // use random_number_mod, only : uniform_random

   // implicit none

   //-------------------------------------------------------------------
   // Passed parameters
   //-------------------------------------------------------------------
   // type (clutter), intent(inout) :: this

   // real(8),        intent(in) :: freqin     // MHz
   // real(8),        intent(in) :: graze      // radians

   //-------------------------------------------------------------------
   // Local variables
   //-------------------------------------------------------------------
   double sigmai;

   double factor, perhi, perlo, /* sigma[9][12], */ sigmax, sigmin, sigmn2, sigmnd, sigmx2, sigmxd, urn;

   float rndnum;

   int imax, imin;

   // Indices reverse for C++ and added elements for index 0 to facilitate 1-based indexing.
   static const double sigma[12 + 1][9 + 1] = {{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, // Not used
                                               {0.0, 95.0, 91.0, 86.0, 64.0, 54.0, 42.0, 33.0, 19.0, 4.0},
                                               {0.0, 75.0, 66.0, 50.0, 34.0, 28.0, 23.0, 19.0, 5.0, -7.0},
                                               {0.0, 90.0, 85.0, 80.0, 70.0, 60.0, 50.0, 34.0, 14.0, -1.0},
                                               {0.0, 65.0, 59.0, 40.0, 33.0, 26.0, 21.0, 16.0, 6.0, -15.0},
                                               {0.0, 90.0, 82.0, 73.0, 64.0, 59.0, 46.0, 36.0, 17.0, -2.0},
                                               {0.0, 40.0, 38.0, 35.0, 31.0, 26.0, 20.0, 15.0, 1.0, -11.0},
                                               {0.0, 90.0, 82.0, 73.0, 64.0, 59.0, 46.0, 36.0, 17.0, -2.0},
                                               {0.0, 40.0, 38.0, 35.0, 31.0, 26.0, 20.0, 15.0, 1.0, -11.0},
                                               {0.0, 100.0, 87.0, 74.0, 74.0, 70.0, 45.0, 37.0, 16.0, 2.0},
                                               {0.0, 42.0, 39.0, 31.0, 27.0, 26.0, 22.0, 16.0, 8.0, -11.0},
                                               {0.0, 74.0, 74.0, 74.0, 68.0, 65.0, 38.0, 31.0, 22.0, 99.0},
                                               {0.0, 40.0, 36.0, 28.0, 25.0, 23.0, 21.0, 11.0, 0.0, 99.0}};

   sigmai = 0.0; // Prevent Linux compile warning

   //-------------------------------------------------------------------
   // Note that there are a few situations where there is no clutter
   // data (high angle Ku band for sea (SIGMA)).  The condition of high
   // grazing angles for Ku Band will not cause the program to stop.
   // When the model is run in the contour plot mode, there is a
   // situation when the target is directly above the radar site and the
   // model attempts to compute clutter returns from directly below the
   // site.  This results in grazing angles near 90 degrees.  In fact
   // this is an artificial situation, as the clutter will definitely be
   // eclipsed by the transmission period (most instances the clutter is
   // a few meters away from the radar).  This is recognized as a
   // temporary fix to this particular problem.  Further work may be
   // required to construct a more permanent solution.
   //-------------------------------------------------------------------

   //-------------------------------------------------------------------
   // Check to ensure the frequency and grazing angle are within the
   // range of the CNA model.
   //-------------------------------------------------------------------

   if (((freqin >= 300.0) && (freqin <= 18000.0)) && ((graze >= 0.0) && (graze <= halfpi)))
   {
      imin   = 0;   // Prevent Linux compile warning
      imax   = 0;   // Prevent Linux compile warning
      perlo  = 0.0; // Prevent Linux compile warning
      perhi  = 0.0; // Prevent Linux compile warning
      sigmnd = 0.0; // Prevent Linux compile warning
      sigmxd = 0.0; // Prevent Linux compile warning

      //----------------------------------------------------------------
      // Proceed.  Input parameters are within the CNA model range.
      //----------------------------------------------------------------

      //----------------------------------------------------------------
      // Determine indexes based on frequency.  Note that the SIGMA
      // array is a 9 X 12 array.  There are 9 grazing angles, 12
      // frequency bands (actually there are only 6 frequency bands, but
      // there are 2 sigma values associated with each frequency.
      //----------------------------------------------------------------

      if (freqin >= 300.0 && freqin <= 1000.0)
      {
         imin = 1;
         imax = 2;
      }
      else if (freqin > 1000.0 && freqin <= 2000.0)
      {
         imin = 3;
         imax = 4;
      }
      else if (freqin > 2000.0 && freqin <= 4000.0)
      {
         imin = 5;
         imax = 6;
      }
      else if (freqin > 4000.0 && freqin <= 8000.0)
      {
         imin = 7;
         imax = 8;
      }
      else if (freqin > 8000.0 && freqin <= 12000.0)
      {
         imin = 9;
         imax = 10;
      }
      else if (freqin > 12000.0 && freqin <= 18000.0)
      {
         imin = 11;
         imax = 12;
      }

      if (graze >= 0.0 && graze <= (0.50 * deg2rad))
      {
         //-------------------------------------------------------------
         // Grazing angle is between 0 and 0.5 degrees.
         //-------------------------------------------------------------

         factor = graze / (0.5 * deg2rad);
         sigmnd = sigma[imin][1] - (factor * (sigma[imin][1] - sigma[imin][2]));
         sigmxd = sigma[imax][1] - (factor * (sigma[imax][1] - sigma[imax][2]));
      }
      else if (graze > (0.5 * deg2rad) && graze <= (1.0 * deg2rad))
      {
         //-------------------------------------------------------------
         // Grazing angle is between 0.5 and 1 degrees.
         //-------------------------------------------------------------

         factor = (graze - (0.5 * deg2rad)) / ((1.0 - 0.5) * deg2rad);
         sigmnd = sigma[imin][2] - (factor * (sigma[imin][2] - sigma[imin][3]));
         sigmxd = sigma[imax][2] - (factor * (sigma[imax][2] - sigma[imax][3]));
      }
      else if (graze > (1.0 * deg2rad) && graze <= (5.0 * deg2rad))
      {
         //-------------------------------------------------------------
         // Grazing angle is between 1 and 5 degrees.
         //-------------------------------------------------------------

         factor = (graze - (1.0 * deg2rad)) / ((5.0 - 1.0) * deg2rad);
         sigmnd = sigma[imin][3] - (factor * (sigma[imin][3] - sigma[imin][4]));
         sigmxd = sigma[imax][3] - (factor * (sigma[imax][3] - sigma[imax][4]));
      }
      else if (graze > (5.0 * deg2rad) && graze <= (10.0 * deg2rad))
      {
         //-------------------------------------------------------------
         // Grazing angle is between 5 and 10 degrees.
         //-------------------------------------------------------------

         factor = (graze - (5.0 * deg2rad)) / ((10.0 - 5.0) * deg2rad);
         sigmnd = sigma[imin][4] - (factor * (sigma[imin][4] - sigma[imin][5]));
         sigmxd = sigma[imax][4] - (factor * (sigma[imax][4] - sigma[imax][5]));
      }
      else if (graze > (10.0 * deg2rad) && graze <= (30.0 * deg2rad))
      {
         //-------------------------------------------------------------
         // Grazing angle is between 10 and 30 degrees.
         //-------------------------------------------------------------

         factor = (graze - (10. * deg2rad)) / ((30.0 - 10.0) * deg2rad);
         sigmnd = sigma[imin][5] - (factor * (sigma[imin][5] - sigma[imin][6]));
         sigmxd = sigma[imax][5] - (factor * (sigma[imax][5] - sigma[imax][6]));
      }
      else if (graze > (30.0 * deg2rad) && graze <= (50.0 * deg2rad))
      {
         //-------------------------------------------------------------
         // Grazing angle is between 30 and 50 degrees.
         //-------------------------------------------------------------

         factor = (graze - (30.0 * deg2rad)) / ((50.0 - 30.0) * deg2rad);
         sigmnd = sigma[imin][6] - (factor * (sigma[imin][6] - sigma[imin][7]));
         sigmxd = sigma[imax][6] - (factor * (sigma[imax][6] - sigma[imax][7]));
      }
      else if (graze > (50.0 * deg2rad) && graze <= (70.0 * deg2rad))
      {
         //-------------------------------------------------------------
         // Grazing angle is between 50 and 70 degrees.
         //-------------------------------------------------------------

         factor = (graze - (50.0 * deg2rad)) / ((70.0 - 50.0) * deg2rad);
         sigmnd = sigma[imin][7] - (factor * (sigma[imin][7] - sigma[imin][8]));
         sigmxd = sigma[imax][7] - (factor * (sigma[imax][7] - sigma[imax][8]));
      }
      else if (graze > (70.0 * deg2rad) && graze <= (90.0 * deg2rad))
      {
         //-------------------------------------------------------------
         // Grazing angle is between 70 and 90 degrees.
         //-------------------------------------------------------------

         factor = (graze - (70.0 * deg2rad)) / ((90.0 - 70.0) * deg2rad);
         sigmnd = sigma[imin][8] - (factor * (sigma[imin][8] - sigma[imin][9]));
         sigmxd = sigma[imax][8] - (factor * (sigma[imax][8] - sigma[imax][9]));
      }

      //----------------------------------------------------------------
      // Clutter reflectivity depends on sea state according to the
      // following table from Reference 1.
      //
      // Sea State                   Percentile
      //
      // 1                         0 - 65
      // 2                        65 - 85
      // 3                        85 - 90
      // 4                        90 - 95
      // 5+                       95 - 100
      //----------------------------------------------------------------

      if (land_form == 10)
      {
         perlo = 0.00;
         perhi = 0.65;
      }
      else if (land_form == 11)
      {
         perlo = 0.65;
         perhi = 0.85;
      }
      else if (land_form == 12)
      {
         perlo = 0.85;
         perhi = 0.90;
      }
      else if (land_form == 13)
      {
         perlo = 0.90;
         perhi = 0.95;
      }
      else if (land_form == 14)
      {
         perlo = 0.95;
         perhi = 1.00;
      }

      //----------------------------------------------------------------
      // Convert the clutter reflectivity bounds from dB to absolute
      // units.
      //----------------------------------------------------------------

      sigmin = pow(10.0, -0.1 * sigmnd);
      sigmax = pow(10.0, -0.1 * sigmxd);

      //----------------------------------------------------------------
      // Determine the new clutter reflectivity bounds based on sea
      // state.
      //----------------------------------------------------------------

      sigmx2 = ((sigmax - sigmin) * perhi) + sigmin;
      sigmn2 = ((sigmax - sigmin) * perlo) + sigmin;

      //----------------------------------------------------------------
      // There are four possible values for SIGMAI depending upon the
      // clutter statistics options switch, STATISTIC_OPT.
      //
      // STATISTIC_OPT            Result
      //
      // 1              Mean of sigma returned.
      // 2              Random distribution using the bounding limits
      //                   for SIGMA that have been modified by
      //                   frequency, grazing angle, and land cover.
      // 3              Worst case (maximum) clutter.
      // 4              Best case (minimum) clutter.
      //----------------------------------------------------------------

      if (statistic_opt == stat_mean)
      {
         sigmai = (sigmx2 + sigmn2) / 2.0;
      }
      else if (statistic_opt == stat_stat)
      {
         rndnum = uniform_random(1);
         urn    = dble(rndnum);
         sigmai = ((sigmx2 - sigmn2) * urn) + sigmn2;
      }
      else if (statistic_opt == stat_max)
      {
         sigmai = sigmx2;
      }
      else if (statistic_opt == stat_min)
      {
         sigmai = sigmn2;
      }
   }
   else
   {
      //----------------------------------------------------------------
      // Either the frequency and/or the grazing angle were outside the
      // range for valid data, set the clutter reflectivity to zero.
      //----------------------------------------------------------------

      sigmai = 0.0;
   }

   return sigmai; // cnasig = sigmai
}

// ==============================================================================================
// The following is from ALARM 'random_number_mod.f90.
// ==============================================================================================

//-------------------------------------------------------------------
//     Purpose:
//     -------
//     Computes uniformly distributed random numbers
//     between [-1,1] and [0,1] for N = 0 and 1, respectively.
//
//
//     Inputs:
//     ------
//     N       -  Integer random number mode option.  If (N = 0) then
//                function random returns a uniformly distributed random
//                number between [-1,1].  If (N = 1) then function
//                random returns a uniformly distributed random number
//                between [0,1].
//
//     X1      -  The integer random number generator seed value.
//
//
//     Outputs:
//     -------
//     RANDOM  -  Real random number uniformly distributed between
//                either [-1,1] or [0,1].
//
//
//     Author:
//     ------
//     Steve Swier (ASI)
//
//
//     References:
//     ----------
//     [1] Communications Of The ACM, October 1988, Volume 31, Number 10
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
//     ALARM 3.2:
//     ---------
//     - Modified module for use with ALARM.  (SPCR-1050, JCL)
//
//     - Removed the deprecated arithmetic if-goto contruct and upgraded
//       to Fortran90.  MWS
//
//     - Converted to work with the random number module.  The seed
//       value is now set elsewhere.  MWS
//-------------------------------------------------------------------

// C++NOTE: ALARM had this declared this as real(8) (a double), but always assigned the return
//          value to a real(4) (a float). The return value has been made a 'float' to prevent
//          compile warnings about truncation.
float WsfEM_ALARM_Clutter::uniform_random(int n)
{
   // implicit none

   // integer, intent(in)    :: n

   double random;

   int   z1;
   float aa;
   float a = -1.0;
   float b = 1.0;

   if (n != 0)
   {
      /*call*/ randu(iseed, z1, aa);

      iseed  = z1;
      random = aa;
   }
   else
   {
      /*call*/ randu(iseed, z1, aa);

      iseed  = z1;
      random = a + (b - a) * aa;
   } // end if

   return static_cast<float>(random); // See C++NOTE above
}

//-------------------------------------------------------------------
//     Purpose:
//     -------
//     Computes uniformly distributed random number between [0,1].
//
//     This prime modulus multiplicative linear congruential random
//     number generator is patterned after the minimal standard
//     described in Communications Of The ACM, October 1988, Volume
//     31, Number 10.
//
//     The equation which is used is f(z) = (16807 * z) MOD 2147483647
//
//
//     Inputs:
//     ------
//     IX      -  The integer random number generator seed value.
//
//     IY      -  Integer seed value from previous iteration.
//
//
//     Outputs:
//     -------
//     YFL     -  Real random number uniformly distributed between
//                either [-1,1] or [0,1].
//
//
//     Author:
//     ------
//     Steve Swier (ASI)
//
//
//     References:
//     ----------
//     [1] Communications Of The ACM, October 1988, Volume 31, Number 10
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
//     ALARM 3.2:
//     ---------
//     - Modified module for use with ALARM.  (SPCR-1050, JCL)
//
//     - Upgraded to Fortran90.  MWS
//-------------------------------------------------------------------

void WsfEM_ALARM_Clutter::randu(int ix, int& iy, float& yfl)
{
   // implicit none

   // integer, intent(in)    :: ix
   // integer, intent(inout) :: iy
   // real,    intent(inout) :: yfl

   int low, high, iz, test, seed;

   //-----------------------------------------------------------------
   // The LOW and HIGH variables are used to prevent integer overflow.
   // 127773 is 2147483647 / 16807 2836 is 2147483647 MOD 16807
   //-----------------------------------------------------------------

   seed = ix;
   high = seed / 127773;
   low  = mod(seed, 127773);
   test = 16807 * low - 2836 * high;

   if (test > 0)
   {
      iy = test;
   }
   else
   {
      iy = test + 2147483647;
   } // end if

   //-----------------------------------------------------------------
   // Divide by 256 to bring the number into range for REALS (2**23).
   //-----------------------------------------------------------------

   iz  = iy / 256;
   yfl = real(iz) / 8388608;
}
