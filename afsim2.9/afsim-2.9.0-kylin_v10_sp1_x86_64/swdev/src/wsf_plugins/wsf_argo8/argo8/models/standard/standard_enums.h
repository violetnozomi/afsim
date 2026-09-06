// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************


#ifndef standard_enums_h
#define standard_enums_h

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

// Define module inputs
typedef enum {
   instandard_l_TriggerPress   = 0,
   instandard_TgtSize          = 1,
   instandard_CUE_POS          = 2,
   instandard_CUE_VEL          = 5,
   instandard_LNCHRPOS         = 8,
   instandard_LNCHRVEL         = 11,
   instandard_r_phi            = 14,
   instandard_r_theta          = 15,
   instandard_r_psi            = 16,
   instandard_fs2_acceleration = 17,
   instandard_LNCHRWB          = 20,
   instandard_X_EMF_TGT        = 23,
   instandard_V_EMF_TGT        = 26,
   instandard_f_offset         = 29,
   instandard_UPDATE_FLAG      = 32,
   instandard_EMF_XTUPDG       = 33,
   instandard_EMF_VTUPDG       = 36,
   instandard_l_TgtInFOV       = 39
} estandardIn;

// Define module outputs
typedef enum {
   outstandard_EMF_MSLFZD           = 0,
   outstandard_l_ProxFuzeActivated  = 1,
   outstandard_EMF_XEM_E            = 2,
   outstandard_EMF_VEM_E            = 5,
   outstandard_EMF_AEM_E            = 8,
   outstandard_r_IMUphi             = 11,
   outstandard_r_IMUthe             = 12,
   outstandard_r_IMUpsi             = 13,
   outstandard_EMF_FLYTYM           = 14,
   outstandard_EMF_MSLPLM           = 15,
   outstandard_EMF_WEIGHM           = 16,
   outstandard_EMF_MSLX             = 17,
   outstandard_EMFMSLV              = 20,
   outstandard_EMF_MSLA             = 23,
   outstandard_fs2_specificForce    = 26,
   outstandard_r_phi                = 29,
   outstandard_r_the                = 30,
   outstandard_r_psi                = 31,
   outstandard_EMF_FMSL             = 32,
   outstandard_EMF_ALPHAM           = 33,
   outstandard_f_TargetPosition     = 34,
   outstandard_l_detached           = 37,
   outstandard_l_IgniteMotor        = 38,
   outstandard_l_safety             = 39,
   outstandard_r_PitchGimAng        = 40,
   outstandard_r_YawGimAng          = 41,
   outstandard_l_ActivateSeeker     = 42,
   outstandard_r_bepsy              = 43,
   outstandard_r_bepsz              = 44,
   outstandard_fs2_VERTGC           = 45,
   outstandard_fs2_HORGC            = 46,
   outstandard_r_alpha              = 47,
   outstandard_r_beta               = 48,
   outstandard_rs_alphadot          = 49,
   outstandard_rs_betadot           = 50,
   outstandard_l_selfDestruct       = 51,
   outstandard_l_MaxTime            = 52,
   outstandard_l_FailedLaunch       = 53,
   outstandard_l_LowMslMach         = 54,
   outstandard_l_LowGAvail          = 55,
   outstandard_l_LowClosingVelocity = 56,
   outstandard_l_GimbalLimit        = 57,
   outstandard_g_GAvail             = 58,
   outstandard_lbf_Thrust           = 59
} estandardOut;

// Define module parameters - none found, place holder
typedef enum {
    emptyParam = 0
} estandardParam;

#define STANDARD_N_IN 40
#define STANDARD_N_OUT 60
#define STANDARD_N_PARAM 0

#endif
// End standard_enums.h
