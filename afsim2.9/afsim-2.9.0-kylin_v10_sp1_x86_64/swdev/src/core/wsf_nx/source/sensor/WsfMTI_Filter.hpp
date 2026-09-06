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

#ifndef WSFMTI_FILTER_HPP
#define WSFMTI_FILTER_HPP

#include <vector>

class UtInput;

class WsfSensor;
class WsfSensorMode;

class WsfMTI_Filter
{
public:
   enum SignalType
   {
      cST_TARGET           = 1,  // sig_target
      cST_BLADE            = 2,  // sig_blade
      cST_CLUTTER          = 3,  // sig_clutter
      cST_NOISE_JAM        = 4,  // sig_noise_jam
      cST_DECEPT_JAM       = 5,  // sig_decept_jam
      cST_NOISE            = 6,  // sig_noise
      cST_INTERFERENCE     = 7,  // SIG_INTERFERENCE
      cST_RWR_SIGNAL       = 8,  // sig_rwr_signal
      cST_RWR_SENS         = 9,  // sig_rwr_sens
      cST_PROCESSED_TARGET = 10, // sig_processed_target
   };

   using DbleVec     = std::vector<double>;
   using DbleDbleVec = std::vector<DbleVec>;

   WsfMTI_Filter();
   // WsfMTI_Filter(const WsfMTI_Filter& aSrc);
   virtual ~WsfMTI_Filter() = default;

   // static WsfMTI_Filter* ObjectFactory(const std::string& aTypeName);

   virtual WsfMTI_Filter* Clone() const;
   virtual bool
                Initialize(double aConicalScanRate, double aDecayConst, double aIF_Bandwidth, int aN_PRFs, DbleVec aPRFs, double aSigmaC);
   virtual bool ProcessInput(UtInput& aInput);

   double mti_response(double aAzAngle, double aDopplerFrequency, int aFlag, double aSlantRange);
   void   set_mti_data(int     aN_Delay,
                       int     aN_Gate,
                       DbleVec aMinAngle,
                       DbleVec aMaxAngle,
                       DbleVec aMinRange,
                       DbleVec aMaxRange,
                       double  aMinResp_dB,
                       bool    aEffects);

   double get_min_response() { return min_resp_abs; }

private:
   bool init_mti(double aConicalScanRate, double aDecayConst, double aIF_Bandwidth, int aN_PRFs, DbleVec aPRFs, double aSigmaC);

   double resmti(double aFrequency);

   bool is_gated(double aAlphaT, double aRangeT);

   double rommti(double a, double b, double eps);

   double cltmti(double aFrequency);

   void get_clutter_psd_mti(int aN_Prfs, DbleVec& aFrequencies, DbleDbleVec& aPSD, int& aCount);

   void spectral_response_mti(int      aPRF_Index,
                              int      aCount,
                              DbleVec  aFrequencies,
                              DbleVec  aPSD,
                              double&  aResponse,
                              double&  aClutterResponse,
                              DbleVec* aPSDout);

   void set_mti_consts(double aSigmaC, double aDecayConst, double aConicalScanRate);

   bool effects; // mti on / off

   int ndelay; // number mti delays
   int nprfs;  // number of prfs
   int ngate;  // number mti gates

   double average_resp; // mti average resp
   double clutter_resp; // clutter response

   DbleVec max_angle; // degrees
   DbleVec max_range; // km
   DbleVec min_angle; // degrees
   DbleVec min_range; // km

   double min_resp_abs; // absolute
   double min_resp_db;  // dB

   // constants used to compute MTI response
   double const1;
   double const2;
   double conical_scan_rate;
   double decay_const;   // clutter quadratic decay constant
   double sigma3;        // 3 times rms clutter freq spread
   double unambig_range; // m

   // declare a dynamically allocated array
   DbleVec pioprf; // pi over prf
};

#endif
