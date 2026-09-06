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
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#ifndef BRAWLEREVALUATION_HPP
#define BRAWLEREVALUATION_HPP

#include "brawler_export.h"

class BrawlerPlatform;
#include "BrawlerUtil.hpp"
class UtEntity;
#include "UtRandom.hpp"
#include "UtVec3.hpp"

/******************************************************************************
BrawlerEvaluation replicates the brawler alternative evaluation call tree:
   EvaluateAlternative()                     //aeval3
      InitializeRoutePointValues()           //evrtei
      CalcRoutepointValue()                  //evrte
      CalcPrimaryValues()                    //eclin
         Evaluate1v1()                       //e1v1
            GunTracking()                    //gunpar
            SelfEngagementMeasure()          //sem2
               GetBestWeapon()               //getarm
               NominalSelfEngagementMeasure()//sem2f
                  AngleAboveHorizon()        //aohor
            WeaponTimeSelfEngageMeasure()    //wtsem
      CalcVectoredFlightValue()              //evvec
      CalcMissileAimValue()                  //evmaim
         InitMissileAiming()                 //ali38
         WeaponAimingEnvelope()              //envlvl_aim
      CalcIlluminationValue()                //evilum
      CalcEvadeValue()                       //evevd
      CalcLowSpeedRecoveryValue()            //evlosp

NOTE: TODO: add other maneuver value components (disengage, illuminate, etc)
******************************************************************************/
class BRAWLER_EXPORT BrawlerEvaluation
{
public:
   // replicates aeval3() from aeval3.f
   //  Generate an alternative score for the passed in alternative projection state
   static double EvaluateAlternative(BrawlerPlatform& me, UtEntity* tgt);

   // replicates aohor() from aohor.f
   static void
   AngleAboveHorizon(const UtVec3d& xa, const UtVec3d& xt, double& ahor, double& adown, double& ovrhor, bool& masked);

   // replaces:  sem2[] computed in gsem2() - gsem2.f
   static double SelfEngagementMeasure(BrawlerPlatform& me, UtEntity& aRef, UtEntity& aTgt);

   // replicates evevd() from evevd.f
   static void CalcEvadeValue(BrawlerPlatform& me, double& valevd);

protected:
   // replicates evrte() from evrte.f
   static void CalcRoutepointValue(BrawlerPlatform& me, double& compr);

   // replicates evrtei() from evrte.f
   static void InitializeRoutePointValues(BrawlerPlatform& me);

   // replicates eclin() from eclin.f
   static void CalcPrimaryValues(BrawlerPlatform& me, double& valofx, double& valdfs);

   // replicates evvec() from evvec.f
   static void CalcVectoredFlightValue(BrawlerPlatform& me, double& vvec);

   // replicates evmaim() from evmaim.f
   static void CalcMissileAimValue(BrawlerPlatform& me, UtEntity& aTgt, double& vmaim);


   // replicates evilum() from evilum.f
   static void CalcIlluminationValue(BrawlerPlatform& me, UtEntity& aTgt, double& villum);

   // replicates evlosp() from evlosp.f
   static void CalcLowSpeedRecoveryValue(BrawlerPlatform& me, double& vlospd);

   // replicates evbddr() from evbddr.f
   static void CalcBadAspectValue(BrawlerPlatform& me, double& vavbdd);

   // replicates e1v1() from e1v1.f
   static void Evaluate1v1(BrawlerPlatform& me, UtEntity& aTgt, double& valoff, double& valdef);

   // replicates gunpar() from gunpar.f
   static void GunTracking(const UtVec3d& xa,
                           const UtVec3d& va,
                           const UtVec3d& xt,
                           const UtVec3d& vt,
                           const UtVec3d& at,
                           double         sbul,
                           bool&          laimp,
                           UtVec3d&       raimp,
                           UtVec3d&       vaimp,
                           UtVec3d&       waimp,
                           double&        rngap,
                           double&        senow);

   // replaces getarm() from getarm.f
   static void GetBestWeapon(BrawlerPlatform& me, UtEntity& aRef, BRAWLER::WeaponType& kndbst, bool& allasp, bool& lkdown);

   // replicates sem2f() from sem2f.f
   static double NominalSelfEngagementMeasure(double         range,
                                              double         rdot,
                                              double         ofbore,
                                              double         aspect,
                                              double         egysem,
                                              double         spdtgt,
                                              const UtVec3d& xa,
                                              const UtVec3d& xt,
                                              double         rbord,
                                              bool           allasp,
                                              bool           lkdown);

   // replicates wtsem() from wtsem.f
   // weapon time evaluation - A MEASURE OF COMBAT ADVANTAGE SENSITIVE TO
   // HAVING target IN A NOMINAL SHORT-RANGE MISSILE ENVELOPE AND THE ABILITY
   // TO MAINTAIN HIM THERE
   static double WeaponTimeSelfEngageMeasure(const UtVec3d& xa, const UtVec3d& va, const UtVec3d& xt, const UtVec3d& vt);

   // replicates ali38() from alt38.f
   static void InitMissileAiming(BrawlerPlatform& me,
                                 UtEntity&        aTgt,
                                 const UtVec3d&   wap,
                                 double           azap,
                                 double           elap,
                                 double           spdtmp,
                                 double           gimbal_lim);

   // replicates envlvl_aim() from envlvl_aim.f
   static double WeaponAimingEnvelope(BrawlerPlatform& me,
                                      const UtVec3d&   xlosa,
                                      double           rng,
                                      double           rdot,
                                      double           se,
                                      double           aof,
                                      double           rmin,
                                      double           rmax,
                                      double           semax,
                                      double           aoffmx,
                                      bool             trkbl,
                                      double           ovrhor,
                                      double           gimbal_lim,
                                      int              kind);

private:
   // SAVE variables from evrte.f
   static double    sclimp;
   static double    vcnom;
   const static int captac[4];
   const static int rtmnv[4];
   const static int captn[4];

   static double msnmlt;
};

#endif
