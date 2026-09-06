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

#include "standard_Argo8Model.hpp"

#include <algorithm>
#include <iostream>

standard_Argo8Model::standard_Argo8Model()
  : Argo8Model()
{
}

standard_Argo8Model::standard_Argo8Model(const Argo8Model& aSrc)
  : Argo8Model(aSrc)
{
}

//virtual
standard_Argo8Model::~standard_Argo8Model()
{
}

//virtual
Argo8Model* standard_Argo8Model::Clone() const
{
   return new standard_Argo8Model(*this);
}

//virtual
void standard_Argo8Model::SetModelLibraryName(std::string aLibraryName)
{
      modelName = aLibraryName;
      libraryName = aLibraryName;
}
//virtual
const std::string standard_Argo8Model::GetModelName()
{
   return modelName;
}

//virtual
const std::string standard_Argo8Model::GetLibraryName()
{
   return libraryName;
}

// virtual
bool standard_Argo8Model::Initialize()
{
   return Argo8Model::Initialize();
   std::cout << "Initializing standard" << std::endl;
}

// virtual
void standard_Argo8Model::SetInputs()
{
   // Modifiers
   int instance = GetInstance();
   pProcSetInput(instance, instandard_l_TriggerPress, inTriggerPress);
   pProcSetInput(instance, instandard_TgtSize, intgt_size);
   pProcSetInputArray(instance, instandard_CUE_POS, incue_pos);
   pProcSetInputArray(instance, instandard_CUE_VEL, incue_vel);
   pProcSetInputArray(instance, instandard_LNCHRPOS, inlnch_pos);
   pProcSetInputArray(instance, instandard_LNCHRVEL, inlnch_vel);
   pProcSetInput(instance, instandard_r_phi, inphi);
   pProcSetInput(instance, instandard_r_theta, intheta);
   pProcSetInput(instance, instandard_r_psi, inpsi);
   pProcSetInputArray(instance, instandard_fs2_acceleration, insep_acc);
   pProcSetInputArray(instance, instandard_LNCHRWB, inrot_rates);
   pProcSetInputArray(instance, instandard_X_EMF_TGT, inguidx);
   pProcSetInputArray(instance, instandard_V_EMF_TGT, inguidv);
   pProcSetInputArray(instance, instandard_f_offset, infoffset);
   pProcSetInput(instance, instandard_UPDATE_FLAG, inupdate);
   pProcSetInputArray(instance, instandard_EMF_XTUPDG, inguidx);
   pProcSetInputArray(instance, instandard_EMF_VTUPDG, inguidv);
   pProcSetInput(instance, instandard_l_TgtInFOV, ininFOV);
}

void standard_Argo8Model::GetOutputs()
{
   // Accessors
   int instance = GetInstance();
   pProcGetOutput(instance, outstandard_EMF_MSLFZD, &outmslfzd);
   pProcGetOutput(instance, outstandard_l_ProxFuzeActivated, &outproxfuze);
   pProcGetOutputArray(instance, outstandard_EMF_XEM_E, outmslx_est);
   pProcGetOutputArray(instance, outstandard_EMF_VEM_E, outmslv_est);
   pProcGetOutputArray(instance, outstandard_EMF_AEM_E, outmsla_est);
   pProcGetOutput(instance, outstandard_r_IMUphi, &outphidot);
   pProcGetOutput(instance, outstandard_r_IMUthe, &outthedot);
   pProcGetOutput(instance, outstandard_r_IMUpsi, &outpsidot);
   pProcGetOutput(instance, outstandard_EMF_FLYTYM, &outflytime);
   pProcGetOutput(instance, outstandard_EMF_MSLPLM, &outmslplume);
   pProcGetOutput(instance, outstandard_EMF_WEIGHM, &outweight);
   pProcGetOutputArray(instance, outstandard_EMF_MSLX, outmslx);
   pProcGetOutputArray(instance, outstandard_EMFMSLV, outmslv);
   pProcGetOutputArray(instance, outstandard_EMF_MSLA, outmsla);
   pProcGetOutputArray(instance, outstandard_fs2_specificForce, outspecforce);
   pProcGetOutput(instance, outstandard_r_phi, &outphi);
   pProcGetOutput(instance, outstandard_r_the, &outthe);
   pProcGetOutput(instance, outstandard_r_psi, &outpsi);
   pProcGetOutput(instance, outstandard_EMF_FMSL, &outmslmach);
   pProcGetOutput(instance, outstandard_EMF_ALPHAM, &outmslalpha);
   pProcGetOutputArray(instance, outstandard_f_TargetPosition, outtgtpos);
   pProcGetOutput(instance, outstandard_l_detached, &outdetached);
   pProcGetOutput(instance, outstandard_l_IgniteMotor, &outignitemotor);
   pProcGetOutput(instance, outstandard_l_safety, &outsafety);
   pProcGetOutput(instance, outstandard_r_PitchGimAng, &outpitch_gimang);
   pProcGetOutput(instance, outstandard_r_YawGimAng, &outyaw_gimang);
   pProcGetOutput(instance, outstandard_l_ActivateSeeker, &outactivate_skr);
   pProcGetOutput(instance, outstandard_r_bepsy, &outbepsy);
   pProcGetOutput(instance, outstandard_r_bepsz, &outbepsz);
   pProcGetOutput(instance, outstandard_fs2_VERTGC, &outvertgc);
   pProcGetOutput(instance, outstandard_fs2_HORGC, &outhorgc);
   pProcGetOutput(instance, outstandard_r_alpha, &outalpha);
   pProcGetOutput(instance, outstandard_r_beta, &outbeta);
   pProcGetOutput(instance, outstandard_rs_alphadot, &outalphadot);
   pProcGetOutput(instance, outstandard_rs_betadot, &outbetadot);
   pProcGetOutput(instance, outstandard_l_selfDestruct, &outselfdestruct);
   pProcGetOutput(instance, outstandard_l_MaxTime, &outmaxtime);
   pProcGetOutput(instance, outstandard_l_FailedLaunch, &outfailedlaunch);
   pProcGetOutput(instance, outstandard_l_LowMslMach, &outlowmslmach);
   pProcGetOutput(instance, outstandard_l_LowGAvail, &outlowgavail);
   pProcGetOutput(instance, outstandard_l_LowClosingVelocity, &outlowclosingvelocity);
   pProcGetOutput(instance, outstandard_l_GimbalLimit, &outgimballimit);
   pProcGetOutput(instance, outstandard_g_GAvail, &outgavail);
   pProcGetOutput(instance, outstandard_lbf_Thrust, &outthrust);
}

//virtual
void standard_Argo8Model::SetRailData(RailData aRailData)
{
    inTriggerPress = Argo8Util::Bool2Double(aRailData.TriggerPress);
    intgt_size     = aRailData.tgt_size;
    inphi          = aRailData.phi;
    intheta        = aRailData.theta;
    inpsi          = aRailData.psi;
    for (int i = 0; i < 3; ++i)
    {
       incue_pos[i]   = aRailData.cue_pos[i];
       incue_vel[i]   = aRailData.cue_vel[i];
       inlnch_pos[i]  = aRailData.lnch_pos[i];
       inlnch_vel[i]  = aRailData.lnch_vel[i];
       insep_acc[i]   = aRailData.sep_acc[i];
       inrot_rates[i] = aRailData.rot_rates[i];
       infoffset[i]   = aRailData.foffset[i];
    }
}
//virtual
void standard_Argo8Model::SetDynamicData(DynamicData aDynamicData)
{
    ininFOV = aDynamicData.inFOV;
    inupdate = aDynamicData.update;
    for (int i = 0; i < 3; ++i)
    {
       intgt_truthx[i] = aDynamicData.tgt_truthx[i];
       intgt_truthv[i] = aDynamicData.tgt_truthv[i];
       inguidx[i] = aDynamicData.guidx[i];
       inguidv[i] = aDynamicData.guidv[i];
    }
}

void standard_Argo8Model::SetUpdateFlag(bool aUpdate)
{
    inupdate = aUpdate;
}

void standard_Argo8Model::GetFlyout(Argo8Output& aOutput)
{
   aOutput.mslfzd = Argo8Util::Double2Bool(outmslfzd);
   aOutput.proxfuze = Argo8Util::Double2Bool(outproxfuze);
   aOutput.phidot  = outphidot; //unused
   aOutput.thedot  = outthedot; //unused
   aOutput.psidot  = outpsidot; //unused
   aOutput.flytime  = outflytime; //unused
   aOutput.mslplume  = Argo8Util::Double2Bool(outmslplume); //unused
   aOutput.weight  = outweight; //unused
   aOutput.phi  = outphi;
   aOutput.the  = outthe;
   aOutput.psi  = outpsi;
   aOutput.mslmach  = outmslmach;  //unused
   aOutput.mslalpha  = outmslalpha;  //unused
   aOutput.detached  = Argo8Util::Double2Bool(outdetached); //unused
   aOutput.ignitemotor  = Argo8Util::Double2Bool(outignitemotor); //unused
   aOutput.safety  = Argo8Util::Double2Bool(outsafety);  //unused
   aOutput.pitch_gimang  = outpitch_gimang; //unused
   aOutput.yaw_gimang  = outyaw_gimang; //unused
   aOutput.activate_skr  = Argo8Util::Double2Bool(outactivate_skr); //unused
   aOutput.bepsy  = Argo8Util::Double2Bool(outbepsy); //unused
   aOutput.bepsz  = Argo8Util::Double2Bool(outbepsz);//unused
   aOutput.vertgc  = outvertgc; //unused
   aOutput.horgc  = outhorgc; //unused
   aOutput.alpha  = outalpha; //unused
   aOutput.beta  = outbeta; //unused
   aOutput.alphadot  = outalphadot; //unused
   aOutput.betadot  = outbetadot; //unused
   aOutput.selfdestruct  = Argo8Util::Double2Bool(outselfdestruct);
   aOutput.maxtime  = Argo8Util::Double2Bool(outmaxtime);
   aOutput.failedlaunch  = Argo8Util::Double2Bool(outfailedlaunch);
   aOutput.lowmslmach  = Argo8Util::Double2Bool(outlowmslmach);
   aOutput.lowgavail  = Argo8Util::Double2Bool(outlowgavail);
   aOutput.lowclosingvelocity  = Argo8Util::Double2Bool(outlowclosingvelocity);
   aOutput.gimballimit  = Argo8Util::Double2Bool(outgimballimit);
   aOutput.gavail  = outgavail;
   aOutput.thrust  = outthrust; //unused
    for (int i = 0; i < 3; ++i)
    {
        aOutput.mslx_est[i] = outmslx_est[i]; //unused
        aOutput.mslv_est[i] = outmslv_est[i]; //unused
        aOutput.msla_est[i] = outmsla_est[i]; //unused
        aOutput.mslx[i] = outmslx[i];
        aOutput.mslv[i] = outmslv[i];
        aOutput.msla[i] = outmsla[i];
        aOutput.specforce[i] = outspecforce[i]; //unused
        aOutput.tgtpos[i] = outtgtpos[i];
    }
}
void standard_Argo8Model::GetSeekerState(bool& aSeeker)
{
    aSeeker = Argo8Util::Double2Bool(outactivate_skr);
}
void standard_Argo8Model::GetFuzeData(FuzeData& aFuzeData)
{
    aFuzeData.mMinCloseVel = Argo8Util::Double2Bool(outlowclosingvelocity);
    aFuzeData.mSelfDestruct = Argo8Util::Double2Bool(outselfdestruct);
    aFuzeData.mProxFuzeActivated = Argo8Util::Double2Bool(outproxfuze);
    aFuzeData.mMslFzd = Argo8Util::Double2Bool(outmslfzd);
    aFuzeData.mMaxTime = Argo8Util::Double2Bool(outmaxtime);
    aFuzeData.mFailedLaunch = Argo8Util::Double2Bool(outfailedlaunch);
    aFuzeData.mLowMslMach = Argo8Util::Double2Bool(outlowmslmach);
    aFuzeData.mGimbalLimit = Argo8Util::Double2Bool(outgimballimit);
    aFuzeData.mDetached = Argo8Util::Double2Bool(outdetached);
    aFuzeData.mLowgAvail = Argo8Util::Double2Bool(outlowgavail);
}


