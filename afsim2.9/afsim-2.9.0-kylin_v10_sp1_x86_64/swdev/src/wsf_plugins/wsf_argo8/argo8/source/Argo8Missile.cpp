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

#include "Argo8Missile.hpp"

#include <algorithm>
#include <cmath>
#include <UtMath.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>

#include "Argo8Structs.h"
#include "Argo8Util.hpp"
#include "UtMemory.hpp"

//Model Instances
#include "../models/standard/standard_Argo8Model.hpp"

namespace
{
   using ArgoTypeList = std::map<const std::string, Argo8Model*>;

   //Pointer to the object type list
   std::unique_ptr<ArgoTypeList> sArgoTypeListPtr = nullptr;

   //Return a reference to the object type list
   ArgoTypeList& GetArgoTypeList()
   {
      if (!sArgoTypeListPtr)
      {
         sArgoTypeListPtr = ut::make_unique<ArgoTypeList>();

         // Create 'type' entries for the built-in types.
         sArgoTypeListPtr->insert(std::make_pair("standard", new standard_Argo8Model()));
      }
      return *sArgoTypeListPtr;
   }
}
Argo8Missile::Argo8Missile()
   : mArgoModel(0),
     mLaunchTime(0.0),
     mMissileState(cMS_COMPUTERHOLD),
     mFuzeStateMask(0)
{
}

//! Copy constructor
Argo8Missile::Argo8Missile(const Argo8Missile& aSrc)
   : mArgoModel(0),
     mLaunchTime(0.0),
     mMissileState(aSrc.mMissileState),
     mFuzeStateMask(aSrc.mFuzeStateMask)
{
}

//virtual
Argo8Missile::~Argo8Missile()
{
}

void Argo8Missile::AddMissileModelType(const std::string& aTypeName,
                                        Argo8Model*       aTypePtr)
{
   GetArgoTypeList().insert(make_pair(aTypeName, aTypePtr));
}

Argo8Model* Argo8Missile::CloneMissileModelType(const std::string& aTypeName)
{
   Argo8Model* argoModel(0);
   if (GetArgoTypeList().find(aTypeName) != GetArgoTypeList().end())
   {
      argoModel = GetArgoTypeList().find(aTypeName)->second->Clone();
   }
   return argoModel;
}

Argo8Model* Argo8Missile::FindMissileModelType(const std::string& aTypeName)
{
   Argo8Model* argoModel(0);
   if (GetArgoTypeList().find(aTypeName) != GetArgoTypeList().end())
   {
      argoModel = GetArgoTypeList().find(aTypeName)->second;
   }
   return argoModel;
}

void Argo8Missile::ClearTypes()
{
   ArgoTypeList::iterator itr = GetArgoTypeList().begin();
   while (itr != GetArgoTypeList().end())
   {
      delete itr->second;
      GetArgoTypeList().erase(itr++);
   }
}

void Argo8Missile::CreateTypes()
{
   GetArgoTypeList();
}

void Argo8Missile::ResetTypes()
{
   ClearTypes();
   GetArgoTypeList();
}

bool Argo8Missile::Initialize(const std::string& aMissileModelType,
                              const std::string& aLibraryName)
{
   //
   if ((mArgoModel == 0) ||
       ((mArgoModel != 0) &&
        (mArgoModel->GetModelName() != aMissileModelType)))
   {
      delete mArgoModel;
      mArgoModel = GenerateMissileModel(aMissileModelType);
      mArgoModel->SetModelLibraryName(aLibraryName);
   }

   bool ok = (mArgoModel != 0);


   if (ok && mArgoModel->LoadModel())
   {
      ok &= mArgoModel->Initialize();
   }
   else
   {
      std::cout << "***** ERROR: ARGO model '" << mArgoModel->GetLibraryName()
                << "' library does not exist in current executable directory." << std::endl;
      ok = false;
   }
   return ok;
}

Argo8Model* Argo8Missile::GenerateMissileModel(const std::string aMissileType)
{
   Argo8Model* argoModel(0);
   if (FindMissileModelType(aMissileType) != 0)
   {
      argoModel = CloneMissileModelType(aMissileType);
   }
   else
   {
      std::cout << "***** ERROR: ARGO model type '" << aMissileType << "' not recognized." << std::endl;
   }
   return argoModel;
}

void Argo8Missile::Update(double aSimTime)
{
   mArgoModel->Update(aSimTime);
}

MissileState Argo8Missile::Update(double      aSimTime,
                                   int&        aFuzeStateMask,
                                   std::string aMissileName)
{
   Update(aSimTime);
   ProcessMissileTermination(aSimTime, aFuzeStateMask);
   return GetMissileState();
}

void Argo8Missile::SetupLogging(bool aLoggingEnabled, std::string aFilename)
{
   mArgoModel->SetupLogging(aLoggingEnabled, aFilename);
}

void Argo8Missile::SetMissileLaunchTime(double aLaunchTime)
{
}

void Argo8Missile::SetRailData(bool aTriggerPress,
                                double atgt_size,
                                double acue_pos[3],
                                double acue_vel[3],
                                double alnch_pos[3],
                                double alnch_vel[3],
                                double aphi,
                                double atheta,
                                double apsi,
                                double asep_acc[3],
                                double arot_rates[3],
                                double afoffset[3])
{
   RailData iD;
   iD.TriggerPress = aTriggerPress;
   iD.tgt_size = atgt_size;
   UtVec3d::Set(iD.cue_pos, acue_pos);
   UtVec3d::Set(iD.cue_vel, acue_vel);
   UtVec3d::Set(iD.lnch_pos, alnch_pos);
   UtVec3d::Set(iD.lnch_vel, alnch_vel);
   iD.phi = aphi;
   iD.theta = atheta;
   iD.psi = apsi;
   UtVec3d::Set(iD.sep_acc, asep_acc);
   UtVec3d::Set(iD.rot_rates, arot_rates);
   UtVec3d::Set(iD.foffset, afoffset);
   SetRailData(iD);
}
void Argo8Missile::SetRailData(RailData aRailData)
{
   mArgoModel->SetRailData(aRailData);
}
void Argo8Missile::SetUpdateFlag(bool aUpdate)
{
    mArgoModel->SetUpdateFlag(aUpdate);
}

void Argo8Missile::SetDynamicData(double atgt_truthx[3],
                                  double atgt_truthv[3],
                                  bool aupdate,
                                  double aguidx[3],
                                  double aguidv[3],
                                  bool ainFOV)
{
   DynamicData dd;

   dd.update = aupdate;
   dd.inFOV = ainFOV;
   UtVec3d::Set(dd.tgt_truthx, atgt_truthx);
   UtVec3d::Set(dd.tgt_truthv, atgt_truthv);
   UtVec3d::Set(dd.guidx, aguidx);
   UtVec3d::Set(dd.guidv, aguidv);
   SetDynamicData(dd);
}

void Argo8Missile::SetDynamicData(DynamicData aDynamicData)
{
   mArgoModel->SetDynamicData(aDynamicData);
}


void Argo8Missile::GetFlyout(bool amslfzd,
                              bool aproxfuze,
                              double amslx_est[3],
                              double amslv_est[3],
                              double amsla_est[3],
                              double aphidot,
                              double athedot,
                              double apsidot,
                              double aflytime,
                              bool amslplume,
                              double aweight,
                              double amslx[3],
                              double amslv[3],
                              double amsla[3],
                              double aspecforce[3],
                              double aphi,
                              double athe,
                              double apsi,
                              double amslmach,
                              double amslalpha,
                              double atgtpos[3],
                              bool adetached,
                              bool aignitemotor,
                              bool asafety,
                              double apitch_gimang,
                              double ayaw_gimang,
                              bool aactivate_skr,
                              bool abepsy,
                              bool abepsz,
                              double avertgc,
                              double ahorgc,
                              double aalpha,
                              double abeta,
                              double aalphadot,
                              double abetadot,
                              bool aselfdestruct,
                              bool amaxtime,
                              bool afailedlaunch,
                              bool alowmslmach,
                              bool alowgavail,
                              bool alowclosingvelocity,
                              bool agimballimit,
                              double agavail,
                              double athrust)
{
   Argo8Output output;
   mArgoModel->GetFlyout(output);
   UtVec3d::Set(output.mslx_est, amslx_est);
   UtVec3d::Set(output.mslv_est, amslv_est);
   UtVec3d::Set(output.msla_est, amsla_est);
   UtVec3d::Set(output.mslx, amslx);
   UtVec3d::Set(output.mslv, amslv);
   UtVec3d::Set(output.msla, amsla);
   UtVec3d::Set(output.specforce, aspecforce);
   UtVec3d::Set(output.tgtpos, atgtpos);
   output.mslfzd = amslfzd;
   output.proxfuze = aproxfuze;
   output.phidot = aphidot;
   output.thedot = athedot;
   output.psidot = apsidot;
   output.flytime = aflytime;
   output.mslplume = amslplume;
   output.weight = aweight;
   output.phi = aphi;
   output.the = athe;
   output.psi = apsi;
   output.mslmach = amslmach;
   output.mslalpha = amslalpha;
   output.detached = adetached;
   output.ignitemotor = aignitemotor;
   output.safety = asafety;
   output.pitch_gimang = apitch_gimang;
   output.yaw_gimang = ayaw_gimang;
   output.activate_skr = aactivate_skr;
   output.bepsy = abepsy;
   output.bepsz = abepsz;
   output.vertgc = avertgc;
   output.horgc = ahorgc;
   output.alpha = aalpha;
   output.beta = abeta;
   output.alphadot = aalphadot;
   output.betadot = abetadot;
   output.selfdestruct = aselfdestruct;
   output.maxtime = amaxtime;
   output.failedlaunch = afailedlaunch;
   output.lowmslmach = alowmslmach;
   output.lowgavail = alowgavail;
   output.lowclosingvelocity = alowclosingvelocity;
   output.gimballimit = agimballimit;
   output.gavail = agavail;
   output.thrust = athrust;
}
void Argo8Missile::GetFlyout(Argo8Output& aOutput)
{
   mArgoModel->GetFlyout(aOutput);
}
void Argo8Missile::GetSeekerState(bool& aSeeker)
{
   mArgoModel->GetSeekerState(aSeeker);
}
double Argo8Missile::GetUpdateInterval()
{
    return mArgoModel->GetUpdateInterval();
}
void Argo8Missile::ProcessMissileTermination(double aSimTime, int& aFuzeStateMask)
{
   if (mMissileState != cMS_TERMINATED)
   {

      FuzeData fuzeData;
      mArgoModel->GetFuzeData(fuzeData);
      //Obtuse code, makes a bit mask with failure code information
      aFuzeStateMask = (fuzeData.mProxFuzeActivated ? cFS_PROXFUZEACTIVATED : 0)
                       | (fuzeData.mSelfDestruct ? cFS_SELFDESTRUCT : 0)
                       | (fuzeData.mMslFzd ? cFS_DETONATE : 0)
                       | (fuzeData.mLowgAvail ? cFS_LOWGAVAIL : 0)
                       | (fuzeData.mGimbalLimit ? cFS_FAILURE_GIMBALLIMIT : 0)
                       | (fuzeData.mMinCloseVel ? cFS_FAILURE_LOWCLOSINGVEL : 0)
                       | (fuzeData.mLowMslMach ? cFS_FAILURE_LOWMACH : 0)
                       | (fuzeData.mMaxTime ? cFS_MAXTOFEXCEEDED : 0)
                       | (fuzeData.mFailedLaunch ? cFS_FAILEDLAUNCH : 0);


      if (((aFuzeStateMask & cFS_DETONATE)              != 0) ||
          ((aFuzeStateMask & cFS_SELFDESTRUCT)          != 0) ||
          ((aFuzeStateMask & cFS_FAILURE_GIMBALLIMIT)   != 0) ||
          ((aFuzeStateMask & cFS_FAILURE_LOWMACH)       != 0) ||
          ((aFuzeStateMask & cFS_FAILURE_LOWCLOSINGVEL) != 0) ||
          ((aFuzeStateMask & cFS_MAXTOFEXCEEDED)        != 0) ||
          ((aFuzeStateMask & cFS_GROUNDIMPACT)          != 0) ||
          ((aFuzeStateMask & cFS_LOWGAVAIL)             != 0) ||
          ((aFuzeStateMask & cFS_FAILEDLAUNCH)          != 0))
      {
         mMissileState = cMS_TERMINATED;
      }
      else if (!fuzeData.mDetached)
      {
         mMissileState = cMS_ONRAIL;
      }
      else
      {
         mMissileState = cMS_INFLIGHT;
      }
   }
}
void Argo8Missile::GetLocationESD(double alocESD[3])
{
   Argo8Output output;
   GetFlyout(output);
   UtVec3d::Set(alocESD, output.mslx);
}
void Argo8Missile::GetgAvail(double& aGavail)
{
   Argo8Output output;
   GetFlyout(output);
   aGavail = output.gavail;
}
void Argo8Missile::GetMissileState(double aSimTime,
                     double alocNED[3],
                     double avelNED[3],
                     double aaccNED[3],
                     double& aPsi,
                     double& aTheta,
                     double& aPhi)
{
   double locESD[3], velESD[3], accESD[3];
   Argo8Output output;
   GetFlyout(output);
   UtVec3d::Set(locESD, output.mslx);
   UtVec3d::Set(alocNED, -locESD[1]*UtMath::cM_PER_FT, locESD[0]*UtMath::cM_PER_FT, locESD[2]*UtMath::cM_PER_FT);
   UtVec3d::Set(velESD, output.mslv);
   UtVec3d::Set(avelNED, -velESD[1]*UtMath::cM_PER_FT, velESD[0]*UtMath::cM_PER_FT, velESD[2]*UtMath::cM_PER_FT);
   UtVec3d::Set(accESD, output.msla);
   UtVec3d::Set(aaccNED, -accESD[1]*UtMath::cM_PER_FT, accESD[0]*UtMath::cM_PER_FT, accESD[2]*UtMath::cM_PER_FT);
   aPhi = output.phi;
   aTheta = output.the;
   aPsi = output.psi + UtMath::cPI/2;
}
