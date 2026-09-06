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

#ifndef WSFEM_ALARM_CLUTTER_HPP
#define WSFEM_ALARM_CLUTTER_HPP

#include <string>
#include <vector>

#include "TblLookup.hpp"
#include "WsfEM_ALARM_Antenna.hpp"
#include "WsfEM_ALARM_Attenuation.hpp"
#include "WsfEM_Clutter.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEnvironment.hpp"
class WsfEM_Xmtr;
class WsfEM_XmtrRcvr;

//! Clutter model from ALARM
class WsfEM_ALARM_Clutter : public WsfEM_Clutter
{
public:
   enum
   {
      max_covers = 7
   }; //  land covers (deprecated; move to private)
   enum
   {
      max_forms = 14
   }; //  land forms (deprecated; move to private
   enum
   {
      max_stat_opts = 5
   }; // clutter statistics (deprecated; move to private)

   using antenna    = WsfEM_ALARM_Antenna::antenna;
   using atmosphere = WsfEM_ALARM_Attenuation::atmosphere;

   WsfEM_ALARM_Clutter();
   WsfEM_ALARM_Clutter& operator=(const WsfEM_ALARM_Clutter&) = delete;
   ~WsfEM_ALARM_Clutter() override                            = default;

   static WsfEM_Clutter* ObjectFactory(const std::string& aTypeName);

   static void ResetState();

   WsfEM_Clutter* Clone() const override;

   bool Initialize(WsfEM_Rcvr* aRcvrPtr) override;

   bool ProcessInput(UtInput& aInput) override;

   double ComputeClutterPower(WsfEM_Interaction& aInteraction, WsfEnvironment& aEnvironment, double aProcessingFactor) override;

   double GetSigmaC() const { return sigmac; }
   double GetDecayConstant() const { return decay_const; }

   //! A summary class for the clutter calculations.  Values are provided for
   //! each ambiguous range.
   class LookSummary
   {
   public:
      LookSummary()
         : mIsEnabled(false)
         , mNumRanges(0)
      {
      }

      bool                mIsEnabled;
      unsigned            mNumRanges;
      std::vector<double> mMinRange;
      std::vector<double> mMaxRange;
      std::vector<double> mSurfaceArea;
      std::vector<double> mPowerAtRange;
      std::vector<double> mPowerAtReceiver;
   };

   const LookSummary& GetLookSummary() { return mLookSummary; }
   bool               LookSummaryEnabled() { return mLookSummary.mIsEnabled; }

protected:
   WsfEM_ALARM_Clutter(const WsfEM_ALARM_Clutter& aSrc);

private:
   enum
   {
      stat_mean = 1
   };
   enum
   {
      stat_stat = 2
   };
   enum
   {
      stat_max = 3
   };
   enum
   {
      stat_min = 4
   };
   enum
   {
      stat_numerical = 5
   };

   void MapEnvironment(WsfPlatform* aPlatformPtr, WsfEnvironment& aEnvironment, int& land_cover, int& land_form, bool& water_cover);
   void clutter_signal_comp(double      ctauo2,
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
                            double&     sigclt);

   double get_reflectivity(const std::vector<double>& elvmsl,
                           double                     frequency,
                           double                     radar_height,
                           int                        iprofl,
                           int                        land_cover,
                           double                     rkfact,
                           const std::vector<double>& rngter,
                           bool                       water_cover,
                           const std::vector<double>& xprofl,
                           const std::vector<double>& zprofl,
                           double                     epslnc); // mjm added elevation angle as parameter

   double mitsig(int land_cover);

   double cnasig(double freqin, double graze);

   void set_random_seed(int new_seed) { iseed = new_seed; }

   float uniform_random(int n);

   void randu(int ix, int& iy, float& yfl);

   std::string map_file;
   double      sigmac;             // standard deviation (clutter freq spread)
   double      decay_const;        // quadratic decay const
   double      reflectivity;       // dbsm/dbsm for numerical
   double      reflectivity_delta; // dbsm/dbsm reflectivity delta
   double      max_range;          // meters
   double      az_max_angle_deg;   // degrees
   double      az_angle_incr_deg;  // degrees
   double      az_max_angle_rad;   // radians
   double      az_angle_incr_rad;  // radians
   double      cw_clutter_bin;     // meters (from cw.f90)

   int land_form;
   int statistic_opt;
   int random_seed;
   int patch_count;
   int iseed; // From random_number_mod.f90

   bool clutter_sw;
   bool map_sw;

   bool mUseBeamwidthForIncrement;

   bool                          mUseMIT_LL_DataTables; // true if to use MIT-LL data tables
   bool                          mPolarizationVertical; // true if the transmitter signal is vertically polarized.
   WsfEnvironment::LandCover     mWSF_LandCover;        // land cover from WSF environment
   WsfEnvironment::LandFormation mWSF_LandForm;         // land form from WSF environment
   WsfEnvironment::SeaState      mWSF_SeaState;         // sea state from WSF environment

   bool           mUseSALRAM_DataTables;
   LookSummary    mLookSummary;
   WsfSimulation* mSimulationPtr;
   // Moved here to deal with statics
   static int                 aprofile;
   static bool                uninit;
   static std::vector<int>    iend;
   static std::vector<int>    istart;
   static std::vector<bool>   visibl;
   static std::vector<double> elvmsl;
   static std::vector<double> rngter; // 0:
   static std::vector<double> tanepp;
   static std::vector<double> xprofl; // 0:
   static std::vector<double> zprofl; // 0:
};

#endif
