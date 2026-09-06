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

#ifndef WSFPD_FILTER_HPP
#define WSFPD_FILTER_HPP

#include <vector>

class UtInput;

class WsfSensor;
class WsfSensorMode;

class WsfPD_Filter
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

   WsfPD_Filter(bool aZVF_Flag = false, int aMax_PRFs = 16);
   // WsfPD_Filter(const WsfPD_Filter& aSrc) = default;
   virtual ~WsfPD_Filter() = default;

   // static WsfPD_Filter* ObjectFactory(const std::string& aTypeName);

   virtual WsfPD_Filter* Clone() const;
   virtual bool          Initialize(double  aConicalScanRate,
                                    double  aDecayConst,
                                    double  aIF_Bandwidth,
                                    int     aN_PRFs,
                                    DbleVec aPRF,
                                    double  aPW,
                                    double  aSigmaC);
   virtual bool          ProcessInput(UtInput& aInput);

   double pd_response(double aF_Dop, int aFlag, int aPRF_Index);
   void   spectral_response_pd(int     aPRF_Index,
                               int     aCount,
                               DbleVec aFrequencies,
                               DbleVec aPSD,
                               int     aMaxPSD_Index,
                               double& aResponse,
                               double& aClutterResponse);

   void set_pd_data(int aN_Filters, int aN_Poles, int aN_PRFs, double aSLR_dB, DbleVec& aFilterBW);

   double get_stop_freq(int aPRF_Index);
   double noise_bandwidth_pd(int aPRF_Index);
   double filter_bandwidth_pd(int aPRF_Index);


private:
   enum FilterFunc
   {
      cFT_CLTFPD,
      cFT_CLUT_CUSTOM,
      cFT_CHFILT
   };

   struct PdBank
   {
      int     npoles;
      double  slr_db;
      DbleVec filter_bw;
   };

   double romchf(double a, double b, double eps, double center_freq, double stop_freq, FilterFunc aFilterFunc);

   bool init_pd(double  aConicalScanRate,
                double  aDecayConst,
                double  aIF_Bandwidth,
                int     aN_PRFs,
                DbleVec aPRF,
                double  aPW,
                double  aSigmaC,
                bool    aZeroAlign);

   bool init_zvf_pd(PdBank  aPdBank,
                       double  aConicalScanRate,
                       double  aDecayConstant,
                       double  aIF_Bandwidth,
                       int     aN_PRFs,
                       DbleVec aPRF,
                       double  aPW,
                       double  aSigmaC/*,
                       char*   aBeamTxt*/);

   double filt_func(double aFrequency, double aCenterFrequency, double aStopFrequency, FilterFunc aFilterFunc);
   double cltfpd(double aFrequency, double aCenterFrequency, double aStopFrequency);
   double clut_custom(double aFrequency, double aCenterFrequency, double aStopFrequency);
   double chfilt(double aDopplerFrequency, double aCenterFrequency, double aStopFrequency);

   double chebcf(double aX, double aN_Poles);

   void set_pd_consts(double aSigmaC, double aDecayConst, double aConicalScanRate);

   void set_clutter_psd_pd(DbleVec& aFrequencies, DbleDbleVec& aPSD, int aCount, int aN_PRFs);

   void compute_filter_parms_pd(double  aDopplerFrequency,
                                int     aPRF_Index,
                                double& aFrequencyAlias,
                                double& aCenterFrequency,
                                double& aStopFrequency,
                                int&    aN_Filters);

   int position(int aCount, DbleVec& aArray, double aValue);

   void alloc_data(bool aDoDealloc = true);

   bool even_filter_cnt; //! flag to indicate the number of filters is even
   bool zero_align;      //! filter to be centered at zero Doppler
   bool zvf_flag;        //! zero velocity filter implementation flag
   bool zvf_sw;          //! zero velocity filter switch

   int max_prfs;   //! maximum number of prfs
   int nfilters;   //! number pd filters
   int npsd_lines; //! number of custom PSD lines
   int psd_iprf;   //! prf indicator for PSD lines
   int npoles;     //! number pd filt poles
   int nprfs;      //! number of prfs

   double slr_abs; //! slr in absolute
   double slr_db;  //! sidelobe ratio dB

   double const1;
   double const2;
   double sigma3;
   double decay_const;       // from the clutter input environment
   double conical_scan_rate; // radar parameter

   DbleDbleVec clutter_response;

   DbleVec     filter_bw;
   DbleVec     prfhz;
   DbleVec     max_center_freq;
   DbleVec     noise_bw;
   DbleVec     psd_freqs;
   DbleDbleVec psd_lines;
   DbleVec     unambig_range;
   DbleVec     stop_freqs;
};
#endif
