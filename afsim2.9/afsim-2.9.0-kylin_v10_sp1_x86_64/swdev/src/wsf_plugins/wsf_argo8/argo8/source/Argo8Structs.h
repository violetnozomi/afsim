// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef ARGO8STRUCTS_HPP
#define ARGO8STRUCTS_HPP
#include "Argo8Util.hpp"


   enum MissileState
   {
      cMS_COMPUTERHOLD,  //! Launch Command not issued
      cMS_ONRAIL,        //! Launch Command issued, still on rail
      cMS_INFLIGHT,      //! Released and in flight, detached and flying
      cMS_TERMINATED     //! Terminated
   };
   enum EngagementState
   {
      cES_DEADSHOOTER = 1,
      cES_DEADTARGET = 2,
      cES_BOTHDEAD = 3,
      cES_BOTHALIVE = 4
   };
   enum SeekerState
   {
      cSS_ACTIVATED      = 1,
      cSS_CUED           = cSS_ACTIVATED << 1,
      cSS_USINGGUIDANCE  = cSS_ACTIVATED << 2,
      cSS_USINGGIMBALCMD = cSS_ACTIVATED << 3,
      cSS_LOCKEDON       = cSS_ACTIVATED << 4
   };

   enum FuzeState
   {
      cFS_DETONATE              = 1,
      cFS_PROXFUZEACTIVATED     = cFS_DETONATE << 1,
      cFS_SELFDESTRUCT          = cFS_DETONATE << 2,
      cFS_FAILURE_GIMBALLIMIT   = cFS_DETONATE << 3,
      cFS_FAILURE_LOWMACH       = cFS_DETONATE << 4,
      //cFS_FAILURE_SEEKERFAILURE = cFS_DETONATE << 5,
      cFS_FAILURE_LOWCLOSINGVEL = cFS_DETONATE << 5,
      cFS_LOWGAVAIL             = cFS_DETONATE << 6,
      cFS_MAXTOFEXCEEDED        = cFS_DETONATE << 7,
      cFS_GROUNDIMPACT          = cFS_DETONATE << 8,
      cFS_TARGETISDEAD          = cFS_DETONATE << 9,
      cFS_FAILEDLAUNCH          = cFS_DETONATE << 10
   };
//Input data
   struct LibraryData {
      LibraryData()
         : ModelName(""),
           LibraryName("")
      {
      }
      std::string ModelName;
      std::string LibraryName;
   };
   struct RailData {
      RailData()
         : TriggerPress(false),
           tgt_size(2.0),
           phi(0.0),
           theta(0.0),
           psi(0.0)
      {
          for (int i = 0; i < 3; ++i)
          {
             cue_pos[i] = 0.0;
             cue_vel[i] = 0.0;
             lnch_pos[i] = 0.0;
             lnch_vel[i] = 0.0;
             sep_acc[i] = 0.0;
             rot_rates[i] = 0.0;
             foffset[i] = 0.0;
          }
      }
      bool TriggerPress;
      double tgt_size;
      double cue_pos[3];
      double cue_vel[3];
      double lnch_pos[3];
      double lnch_vel[3];
      double phi;
      double theta;
      double psi;
      double sep_acc[3];
      double rot_rates[3];
      double foffset[3];
   };
   struct DynamicData {
      DynamicData()
         : update(false),
           inFOV(false)
      {
          for (int i = 0; i < 3; ++i)
          {
             tgt_truthx[i] = 0.0;
             tgt_truthv[i] = 0.0;
             guidx[i] = 0.0;
             guidv[i] = 0.0;
          }
      }
      double tgt_truthx[3];
      double tgt_truthv[3];
      bool update;
      double guidx[3];
      double guidv[3];
      bool inFOV; //0 if seeker is off, 1 if seeker is on
   };
   struct FuzeData {
      FuzeData()
         : mMinCloseVel(false),
           mSelfDestruct(false),
           mProxFuzeActivated(false),
           mMslFzd(false),
           mMaxTime(false),
           mFailedLaunch(false),
           mLowMslMach(false),
           mGimbalLimit(false),
           mLowgAvail(false),
           mDetached(false)
           {
           }
      bool mMinCloseVel;
      bool mSelfDestruct;
      bool mProxFuzeActivated;
      bool mMslFzd;
      bool mMaxTime;
      bool mFailedLaunch;
      bool mLowMslMach;
      bool mGimbalLimit;
      bool mLowgAvail;
      bool mDetached;
   };
//Output Data
   struct Argo8Output {
      Argo8Output()
         : mslfzd(false),
           proxfuze(false),
           phidot(0.0),
           thedot(0.0),
           psidot(0.0),
           flytime(0.0),
           mslplume(false),
           weight(0.0),
           phi(0.0),
           the(0.0),
           psi(0.0),
           mslmach(0.0),
           mslalpha(0.0),
           detached(false),
           ignitemotor(false),
           safety(false),
           pitch_gimang(0.0),
           yaw_gimang(0.0),
           activate_skr(false),
           bepsy(false),
           bepsz(false),
           vertgc(0.0),
           horgc(0.0),
           alpha(0.0),
           beta(0.0),
           alphadot(0.0),
           betadot(0.0),
           selfdestruct(false),
           maxtime(false),
           failedlaunch(false),
           lowmslmach(false),
           lowgavail(false),
           lowclosingvelocity(false),
           gimballimit(false),
           gavail(0.0),
           thrust(0.0)
      {
          for (int i = 0; i < 3; ++i)
          {
             mslx_est[i] = 0.0;
             mslv_est[i] = 0.0;
             msla_est[i] = 0.0;
             mslx[i] = 0.0;
             mslv[i] = 0.0;
             msla[i] = 0.0;
             specforce[i] = 0.0;
             tgtpos[i] = 0.0;
          }
      }
      bool mslfzd;
      bool proxfuze;
      double mslx_est[3];
      double mslv_est[3];
      double msla_est[3];
      double phidot;
      double thedot;
      double psidot;
      double flytime;
      bool mslplume; //if true, engine was buring at end of projection
      double weight;
      double mslx[3];
      double mslv[3];
      double msla[3];
      double specforce[3];
      double phi;
      double the;
      double psi;
      double mslmach;
      double mslalpha;
      double tgtpos[3];
      bool detached; //unused
      bool ignitemotor; //unused
      bool safety;      //unused
      double pitch_gimang;
      double yaw_gimang;
      bool activate_skr; //unused
      bool bepsy; //unused
      bool bepsz; //unused
      double vertgc; //unused
      double horgc; //unused
      double alpha; //unused
      double beta; //unused
      double alphadot; //unused
      double betadot; //unused
      bool selfdestruct;
      bool maxtime;
      bool failedlaunch;
      bool lowmslmach;
      bool lowgavail;
      bool lowclosingvelocity;
      bool gimballimit;
      double gavail;
      double thrust;
   };
#endif
