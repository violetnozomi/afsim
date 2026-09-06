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

#include "BrawlerMIND.hpp"

#include "UtInput.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

// Bring in BRAWLER constants, enumerations, and util functions
using namespace BRAWLER;

// virtual
bool BrawlerMIND::LoadMindFile(const std::string& absolutePathFile)
{
   UtInput aInput;
   aInput.PushInput(ut::make_unique<UtInputFile>(absolutePathFile));

   std::string command;
   while (aInput.TryReadCommand(command))
   {
      if (command == "AGE1")
      {
         aInput.ReadValue(age1);
      }
      else if (command == "CHISIG")
      {
         aInput.ReadValue(chisig);
      }
      else if (command == "TCORR")
      {
         for (size_t i = 0; i < 3; ++i)
         {
            aInput.ReadValue(tcorr[i]);
         }
         // Also read the two unused VALKEY lines
         std::string temp;
         aInput.ReadLine(temp);
         aInput.ReadLine(temp);
      }
      else if (command == "AGEMOD")
      {
         for (auto& i : agemod)
         {
            for (auto& j : i)
            {
               aInput.ReadValue(j);
            }
         }
      }
      else if (command == "VALSIG")
      {
         aInput.ReadValue(valsig);
      }
      else if (command == "VCUTF")
      {
         aInput.ReadValue(vcutf);
      }
      else if (command == "MALTS")
      {
         aInput.ReadValue(malts);
      }
      else if (command == "STKPN")
      {
         aInput.ReadValue(stkpn);
      }
      else if (command == "TMUSUP")
      {
         aInput.ReadValue(tmusup);
      }
      else if (command == "TRCH")
      {
         aInput.ReadValue(trch);
      }
      else if (command == "TRCHW")
      {
         aInput.ReadValue(trchw);
      }
      else if (command == "WTVMSL")
      {
         aInput.ReadValue(wtvmsl);
      }
      else if (command == "TPROJ")
      {
         aInput.ReadValue(tproj);
      }
      else if (command == "AGFOLO")
      {
         aInput.ReadValue(agfolo);
      }
      else if (command == "AGERAD")
      {
         aInput.ReadValue(agerad);
      }
      else if (command == "CLIMBM")
      {
         aInput.ReadValue(climbm);
      }
      else if (command == "GMAXM")
      {
         aInput.ReadValue(gmaxm);
      }
      else if (command == "RFXDLY")
      {
         aInput.ReadValue(rfxdly);
      }
      else if (command == "WRFORM")
      {
         aInput.ReadValue(wrform);
      }
      else if (command == "WRRFRM")
      {
         aInput.ReadValue(wrrfrm);
      }
      else if (command == "RNGWPN")
      {
         for (size_t i = 0; i < 5; ++i)
         {
            aInput.ReadValue(rngwpn[i]);
         }
      }
      else if (command == "TVPOLD")
      {
         aInput.ReadValue(tvpold);
      }
      else if (command == "DETR0")
      {
         aInput.ReadValue(detr0);
      }
      else if (command == "TEARLY")
      {
         aInput.ReadValue(tearly);
      }
      else if (command == "HDSPRD")
      {
         aInput.ReadValue(hdsprd);
      }
      else if (command == "HDPKLO")
      {
         aInput.ReadValue(hdpklo);
      }
      else if (command == "HDPKHI")
      {
         aInput.ReadValue(hdpkhi);
      }
      else if (command == "HDRKMX")
      {
         aInput.ReadValue(hdrkmx);
      }
      else if (command == "HDPSMN")
      {
         aInput.ReadValue(hdpsmn);
      }
      else if (command == "SPLITA")
      {
         aInput.ReadValue(splita);
      }
      else if (command == "REQDLY")
      {
         aInput.ReadValue(reqdly);
      }
      else if (command == "ORDDLY")
      {
         aInput.ReadValue(orddly);
      }
      else if (command == "CHSWLS")
      {
         aInput.ReadValue(chswls);
      }
      else if (command == "CHSWLO")
      {
         aInput.ReadValue(chswlo);

         // Also read Nearby Update Interval (next, unlabeled line)
         // TODO User Manual states that this is 10 values but all
         // example files only have 1, handle this case in the future?
         aInput.ReadValue(dtnear);
      }
      else if (command == "DTUDES")
      {
         aInput.ReadValue(dtudes);
      }
      else if (command == "DELFAC")
      {
         aInput.ReadValue(delfac);
      }
      else if (command == "CWIDTH")
      {
         aInput.ReadValue(cwidth);
      }
      else if (command == "MAXAC_LOW")
      {
         aInput.ReadValue(maxac_low);
      }
      else if (command == "MAXAC_MED")
      {
         aInput.ReadValue(maxac_med);
      }
      else if (command == "MAXAC_HI")
      {
         std::string input = "";
         if (aInput.TryReadCommand(input) && input == "MACMND")
         {
            // Set maxac_hi to macmnd value from BRAWLER par.fi
            maxac_hi = 20;
         }
         else
         {
            // Not the special MACMND value, push back and
            // try to read as a number
            aInput.PushBack(input);
            aInput.ReadValue(maxac_hi);
         }
      }
      else if (command == "SLOGEE")
      {
         aInput.ReadValue(slogee);
      }
      else if (command == "SL_G_LEEWAY")
      {
         aInput.ReadValue(sl_g_leeway);
      }
      else if (command == "TNL_ALPHA")
      {
         aInput.ReadValue(tnl_alpha);
      }
      else if (command == "TNL_BETA_LOW")
      {
         aInput.ReadValue(tnl_beta_low);
      }
      else if (command == "TNL_BETA_MED")
      {
         aInput.ReadValue(tnl_beta_med);
      }
      else if (command == "TNL_BETA_HI")
      {
         aInput.ReadValue(tnl_beta_hi);
      }
      else if (command == "TNL_TAU_LOW")
      {
         aInput.ReadValue(tnl_tau_low);
      }
      else if (command == "TNL_TAU_MED")
      {
         aInput.ReadValue(tnl_tau_med);
      }
      else if (command == "TNL_TAU_HI")
      {
         aInput.ReadValue(tnl_tau_hi);
      }
      else if (command == "TNL_CNTRST")
      {
         aInput.ReadValue(tnl_cntrst);
      }
      else if (command == "HOTMIN")
      {
         aInput.ReadValue(hotmin);
      }
      else if (command == "FT_REJECT_T")
      {
         aInput.ReadValue(ft_reject_t);
      }
      else if (command == "TAU_ESTAB")
      {
         aInput.ReadValue(tau_estab);
      }
      else if (command == "ESTABLISH")
      {
         aInput.ReadValue(mm_est_lvl);
      }
      else if (command == "DIS-ESTABLISH")
      {
         aInput.ReadValue(mm_disest_lvl);
      }
      else if (command == "PURGE")
      {
         aInput.ReadValue(mm_purge_lvl);
      }
      else if (command == "VISUAL")
      {
         aInput.ReadValue(obs_est_val[0]);
      }
      else if (command == "RADAR")
      {
         // Loop over input commands only taking values
         // immediately after keyword. This ignores comments
         std::string subcommand;
         while (aInput.TryReadCommand(subcommand) && subcommand != "ACTIVE")
         {
            if (subcommand == "SCAN")
            {
               aInput.ReadValue(obs_est_val[1]);
            }
            if (subcommand == "STT")
            {
               aInput.ReadValue(obs_est_val[2]);
            }
            if (subcommand == "TWS")
            {
               aInput.ReadValue(obs_est_val[3]);
            }
            if (subcommand == "SPOT")
            {
               aInput.ReadValue(obs_est_val[4]);
            }
         }
         // Store ACTIVE value
         aInput.ReadValue(obs_est_val[5]);
      }
      else if (command == "MESSAGE")
      {
         aInput.ReadValue(obs_est_val[6]);
      }
      else if (command == "INFERRED")
      {
         aInput.ReadValue(obs_est_val[7]);
      }
      else if (command == "JAMMED_RADAR")
      {
         std::string subcommand;
         while (aInput.TryReadCommand(subcommand) && subcommand != "ACTIVE")
         {
            if (subcommand == "SCAN")
            {
               aInput.ReadValue(obs_est_val[8]);
            }
            if (subcommand == "STT")
            {
               aInput.ReadValue(obs_est_val[9]);
            }
            if (subcommand == "TWS")
            {
               aInput.ReadValue(obs_est_val[10]);
            }
            if (subcommand == "SPOT")
            {
               aInput.ReadValue(obs_est_val[11]);
            }
         }
         // Store last JAMMED_RADAR value (ACTIVE)
         aInput.ReadValue(obs_est_val[12]);
      }
      else if (command == "JAMMED_MESSAGE")
      {
         aInput.ReadValue(obs_est_val[13]);
      }
      else if (command == "IFF")
      {
         aInput.ReadValue(obs_est_val[14]);
      }
      else if (command == "RHAW")
      {
         aInput.ReadValue(obs_est_val[15]);
      }
      else if (command == "IRST")
      {
         aInput.ReadValue(obs_est_val[16]);
      }
      else if (command == "MW")
      {
         aInput.ReadValue(obs_est_val[17]);
      }
      else if (command == "MAW")
      {
         aInput.ReadValue(obs_est_val[18]);
      }
      else if (command == "RWR")
      {
         aInput.ReadValue(obs_est_val[19]);
      }
      else if (command == "SAN")
      {
         aInput.ReadValue(obs_est_val[20]);
      }
      else if (command == "TTT")
      {
         aInput.ReadValue(obs_est_val[21]);
      }
      else if (command == "SSS")
      {
         aInput.ReadValue(obs_est_val[22]);
      }
      else if (command == "XAV")
      {
         aInput.ReadValue(obs_est_val[23]);
      }
      else if (command == "TOC")
      {
         aInput.ReadValue(obs_est_val[24]);
      }
      else if (command == "ESM")
      {
         aInput.ReadValue(obs_est_val[25]);
      }
      else if (command == "EXT")
      {
         aInput.ReadValue(obs_est_val[26]);
      }
      else if (command == "OTD")
      {
         aInput.ReadValue(obs_est_val[27]);
      }
      else if (command == "DLD")
      {
         aInput.ReadValue(obs_est_val[28]);
      }
      else
      {
         // The MIND file can have many comments that are not truly a command
         // or input, but are also not an error. Here we either ignore them or
         // optionally display a message is debug is enabled
         if (mDebug)
         {
            auto logDebug = ut::log::debug() << "Unknown command:";
            logDebug.AddNote() << "Command: " << command;
            logDebug.AddNote() << "MIND file: " << absolutePathFile;
         }
      }
   }
   return true;
}

double BrawlerMIND::RangeWeapon(BRAWLER::WeaponType aType)
{
   switch (aType)
   {
   case kndsrm:
   {
      return rngwpn[0];
   };
   case kndmrm:
   {
      return rngwpn[1];
   };
   case kndlrm: // also kndmis
   {
      return rngwpn[2];
   };
   case kndgun:
   {
      return rngwpn[3];
   };
   case knddew:
   {
      return rngwpn[4];
   };
   default:
   {
      // use short range missile value
      return rngwpn[0];
   };
   }
}
