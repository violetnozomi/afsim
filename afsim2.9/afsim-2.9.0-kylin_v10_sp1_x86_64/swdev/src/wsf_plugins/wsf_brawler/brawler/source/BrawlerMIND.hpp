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

#ifndef BRAWLERMIND_HPP
#define BRAWLERMIND_HPP

#include <array>
#include <string>

#include "BrawlerUtil.hpp"

//! Reads in and provides access to values from the BRAWLER MIND file
//!
//! Member variables names differ from the standard on purpose to match the
//! names used in BRAWLER. Access methods are given a more descriptive name.
//! See section 3.2.5 of the BRAWLER User Manual for more information on values.
class BrawlerMIND
{
public:
   BrawlerMIND()          = default;
   virtual ~BrawlerMIND() = default;

   //! Reads the BRAWLER MIND file format as specified by the
   //! BRAWLER v7.5 User Manual.
   //! @param absolutePathFile Absolute path to the MIND file
   //! @return True if the file is successfully read
   virtual bool LoadMindFile(const std::string& absolutePathFile);

   //! Maximum time in seconds for decision level 3 (maneuver) reconsideration, typically 1.0
   //! Equivalent to agemod(2,3), 'agemod' defined in mindc.fi of BRAWLER v7.5
   double ManeuverReconsiderationMax() { return agemod[2][1]; }

   //! Maximum time in seconds for decision level 7 (weapon employment) reconsideration, typically 1.0
   //! Equivalent to agemod(2,7), 'agemod' defined in mindc.fi of BRAWLER v7.5
   double WeaponReconsiderationMax() { return agemod[6][1]; }

   //! Maneuver look ahead (projection) time (sec). Replaces tproj
   //! See BRAWLER User manual p. 3.2.5-6, read from MIND file
   //! Default value return 3.0 (typical config value)
   double TimeProjection() { return tproj; }

   //! Max Gees to use for ROUTEPOINT flying, (G units). Replaces gmaxm
   double MaxGeesForRouteFlying() { return gmaxm; }

   //! RangeWeapon replaces nominal weapon ranges "rngwpn" from BRAWLER v7.5
   //! See BRAWLER User manual p. 3.2.5-7
   //! The value is read from the MIND input file with typical values of
   //! 25000, 30000, 85000, 3000, 40000 ft for SRM, MRM, LRM, GUN, DEW respectively
   //! @param aType The weapon type to get the range for
   //! @return the nominal range of the weapon in feet
   double RangeWeapon(BRAWLER::WeaponType aType);

   //! SmoothingTimeSeconds replaces "tvpold" from BRAWLER v7.5
   //! See BRAWLER User manual p. 3.2.5-2 and p. 3.2.5-7
   //! The value is read from the MIND input file with typical value of 15
   double SmoothingTimeSeconds() { return tvpold; }

   //! DefaultHostileAssessmentRate replaces "detr0" from BRAWLER v7.5
   //! The value is read from the MIND input file with typical value of 0.02
   double DefaultHostileAssessmentRate() { return detr0; }

   //! MinTimeBetweenHostAssessments replaces "tearly" from BRAWLER v7.5
   //! The value is read from the MIND input file with typical value of 4.99
   double MinTimeBetweenHostAssessments() { return tearly; }

   //! Minimum number of g's at which a pilot decides he is in a slow-flight regime. Replaces 'slogee'
   //! See BRAWLER User manual p. 3.2.5-3, read from MIND file
   //! Default return 3.0 (typical config value)
   double SlowGee() { return slogee; }

   //! Leeway g's used to determine if in a slow-flight regime.  Replaces 'sl_g_leeway'
   //! See BRAWLER User manual p. 3.2.5-3, read from MIND file
   //! Default return 2.0 (typical config value)
   double SlowGeeLeeway() { return sl_g_leeway; }

   //! @name Tunnel Vision MIND Data
   //! Accessors for getting tunnel vision related multipliers.
   //! Replaces BRAWLER "tnl_alpha", "tnl_beta_*" "tnl_tau_*", and "tnl_cntrst"
   //! Low/Med/Hi equate to pilot skill level
   //!@{
   double TunnelAlpha() { return tnl_alpha; }
   double TunnelBetaLow() { return tnl_beta_low; }
   double TunnelBetaMed() { return tnl_beta_med; }
   double TunnelBetaHi() { return tnl_beta_hi; }
   double TunnelTauLow() { return tnl_tau_low; }
   double TunnelTauMed() { return tnl_tau_med; }
   double TunnelTauHi() { return tnl_tau_hi; }
   double TunnelContrast() { return tnl_cntrst; }
   //!@}

private:
   bool mDebug{false}; //!< Controls display of messages when reading MIND file

   //! @name MIND Variables
   //! See section 3.2.5 of the BRAWLER User Manual.
   //!@{
   double age1{2.0};
   double chisig{6.0}; // obsolete
   // Default values used in the constructor are
   // taken from the BRAWLER v7.5 release MIND file
   // tcorr[3];
   std::array<double, 3>                tcorr  = {{10.0, 10.0, 10.0}}; // not in use
   std::array<std::array<double, 2>, 7> agemod = {
      {{{5.0, 20.0}}, {{0.5, 5.0}}, {{0.25, 1.0}}, {{5.0, 20.0}}, {{5.0, 60.0}}, {{5.0, 60.9}}, {{1.0, 1.0}}}};
   double                valsig{0.001};
   double                vcutf{0.6};
   int                   malts{5};
   double                stkpn{5.0};
   double                tmusup{5.0};
   double                trch{20.0}; // not in use
   double                trchw{5.0}; // not in use
   double                wtvmsl{0.5};
   double                tproj{3.0};
   double                agfolo{20.0};
   double                agerad{10.0};
   double                climbm{10000.0};
   double                gmaxm{4.0};
   double                rfxdly{0.2};
   double                wrform{0.1};
   double                wrrfrm{0.5};
   std::array<double, 5> rngwpn = {{25000.0, 30000.0, 85000.0, 3000.0, 40000.0}};
   double                tvpold{15.0};
   double                detr0{0.02};
   double                tearly{4.99};
   double                hdsprd{2.0};
   double                hdpklo{0.05};
   double                hdpkhi{0.10};
   double                hdrkmx{0.5};
   double                hdpsmn{0.8};
   double                splita{45.0};
   double                reqdly{15.0};
   double                orddly{20.1};
   double                chswls{0.3};
   double                chswlo{0.2};
   double dtnear{10.0}; // User Manual says this is an array of 10 values, input file usually only has one (unlabeled)
   double dtudes{5.0};
   double delfac{0.3};
   double cwidth{5.0};
   int    maxac_low{3};
   int    maxac_med{5};
   int    maxac_hi{10};
   double slogee{3.0};
   double sl_g_leeway{2.0};
   double tnl_alpha{0.3};
   double tnl_beta_low{0.6};
   double tnl_beta_med{0.75};
   double tnl_beta_hi{0.9};
   double tnl_tau_low{60.0};
   double tnl_tau_med{30.0};
   double tnl_tau_hi{0.001};
   double tnl_cntrst{2.0};
   double hotmin{1000.0};
   double ft_reject_t{0.02}; // Not documented in User Manaual
   double tau_estab{30.0};
   double mm_est_lvl{20.0};
   double mm_disest_lvl{10.0};            // Not currently used
   double mm_purge_lvl{1.0};              // Not currently used
   std::array<double, 29> obs_est_val = {{// [1] & [8] are different defaults
                                          40.0, 13.2, 40.0, 40.0, 40.0, 40.0, 40.0, 40.0, 13.2, 40.0,
                                          40.0, 40.0, 40.0, 40.0, 40.0, 40.0, 40.0, 40.0, 40.0, 40.0,
                                          40.0, 40.0, 40.0, 40.0, 40.0, 40.0, 40.0, 40.0}};
   //!@}
   /* obs_est_vals:
   0 visual
   1 radar_scan
   2 radar_stt
   3 radar_tws
   4 radar_spot
   5 radar_active
   6 message
   7 inferred
   8 jammed_radar_scan
   9 jammed_radar_stt
   10 jammed_radar_tws
   11 jammed_radar_spot
   12 jammed_radar_active
   13 jammed_message
   14 iff
   15 rhaw
   16 irst
   17 mw
   18 maw
   19 rwr
   20 san
   21 ttt
   22 sss
   23 xav
   24 toc
   25 esm
   26 ext
   27 otd
   28 dld
   */
};

/* EXAMPLE MIND FILE

AGE1        2.
CHISIG      6.
TCORR       10.       10.       10.  !OBSERVATION CORRELATION TIMES
44444444444444444444444444400000000000000000000000 VALKEY 1-50
00000000000000000000000000000000000000000000000000 VALKEY 51-100
AGEMOD  5.0    20.0
        0.5     5.0
        0.25    1.0
        5.0    20.0
        5.0    60.0
        5.0    60.0
        0.1     1.0
VALSIG   0.001
VCUTF    0.6
MALTS    5
STKPN       5.0
TMUSUP      5.0
TRCH       20.0
TRCHW       5.0
WTVMSL      0.5
TPROJ       3.0
AGFOLO     20.0
AGERAD     10.0
CLIMBM  10000.0
GMAXM       4.0
RFXDLY      0.2 !REFLEX_DELAY - FOR MANEUVER IMPLEMENTATION AFTER DECISION IS
WRFORM      0.1
WRRFRM      0.5
RNGWPN  25000.  30000.  85000.  3000.  40000. !SRM,MRM,LRM,GUN,DEW
TVPOLD     15.
DETR0       0.02
TEARLY      4.99
HDSPRD      2.0
HDPKLO     0.05
HDPKHI     0.10
HDRKMX     0.5
HDPSMN     0.8
SPLITA    45.0
REQDLY    15.0
ORDDLY    20.1
CHSWLS      .3
CHSWLO      .2
10.                      NEARBY UPDATE INTERVAL
DTUDES     5.0
DELFAC     0.3
CWIDTH     5.0
MAXAC_LOW  3
MAXAC_MED  5
MAXAC_HI  MACMND
SLOGEE     3.
SL_G_LEEWAY 2.
TNL_ALPHA    0.3
TNL_BETA_LOW 0.6
TNL_BETA_MED 0.75
TNL_BETA_HI  0.9
TNL_TAU_LOW 60.0
TNL_TAU_MED 30.0
TNL_TAU_HI   0.001
TNL_CNTRST   2.
HOTMIN    1000.       !minimum height over terrain
FT_REJECT_T    0.02   !Time (per false tgt) to reject false targets (sec)
TAU_ESTAB      30.     Establishment value time constant
ESTABLISH      20.     Threshold for establishment
DIS-ESTABLISH  10.     Threshold for disestablishment (currently unused)
PURGE           1.     Threshold for purging  (currently unused)
VISUAL         40.     Value added for a visual observation
RADAR                  Values added for radar obs
   SCAN        13.2    => roughly 2 hits in 20 seconds will establish
   STT         40.
   TWS         40.
   SPOT        40.
   ACTIVE      40.
MESSAGE        40.
INFERRED       40.
JAMMED_RADAR
   SCAN        13.2
   STT         40.
   TWS         40.
   SPOT        40.
   ACTIVE      40.
JAMMED_MESSAGE 40.
IFF            40.
RHAW           40.
IRST           40.
MW             40.
MAW            40.
RWR            40.
SAN            40.
TTT            40.
SSS            40.
XAV            40.
TOC            40.
ESM            40.
EXT            40.
OTD            40.
DLD            40.

*/

#endif
