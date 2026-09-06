// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef DIS_HPP
#define DIS_HPP

#include "dis_export.h"

#include "DisEntityId.hpp"
#include "DisTypes.hpp"

class DIS_EXPORT Dis
{
public:
   static void Init();
   static void Term();

   static bool IsValid();
   // Accessors

   static DisEnum8   GetProtocolVersion() { return mProtocolVersion; }
   static DisUint8   GetExercise() { return mExercise; }
   static DisUint16  GetApplication() { return mApplication; }
   static DisUint16  GetSite() { return mSite; }
   static DisFloat32 GetHrtBeatTimer() { return mHrtBeatTimer; }
   static DisFloat32 GetHrtBeatMoveTimer() { return mHrtBeatMoveTimer; }
   static DisFloat32 GetHrtBeatDE_FireTimer() { return mHrtBeatDE_FireTimer; }
   static DisFloat32 GetHrtBeatMplier() { return mHrtBeatMplier; }
   static DisFloat32 GetDRAOrientThrsh() { return mDRAOrientThrsh; }
   static DisFloat32 GetDRAPosThrsh() { return mDRAOrientThrsh; }
   static DisFloat32 GetEEAzThrsh() { return GetEE_AZ_THRSH(); }
   static DisFloat32 GetEEElThrsh() { return GetEE_EL_THRSH(); }
   static DisFloat32 GetDE_PrecisionAimingThrsh() { return mDE_PrecisionAimingThrsh; }
   static DisFloat32 GetDE_EnergyThrsh() { return mDE_EnergyThrsh; }
   static DisUint8   GetTargetsInTjFieldDflt() { return GetEE_HIGH_DENSITY_THRSH(); }
   static DisFloat32 GetEE_AZ_THRSH() { return mEE_AZ_THRSH; }
   static DisFloat32 GetEE_EL_THRSH() { return mEE_EL_THRSH; }
   static DisFloat32 GetEE_ERP_THRSH() { return mEE_ERP_THRSH; }
   static DisFloat32 GetEE_FREQ_THRSH() { return mEE_FREQ_THRSH; }
   static DisFloat32 GetEE_FRNG_THRSH() { return mEE_FRNG_THRSH; }
   static DisUint8   GetEE_HIGH_DENSITY_THRSH() { return mEE_HIGH_DENSITY_THRSH; }
   static DisUint8   GetEE_MAX_BEAM_THRSH() { return mEE_MAX_BEAMS_THRSH; }
   static DisFloat32 GetEE_PRF_THRSH() { return mEE_PRF_THRSH; }
   static DisFloat32 GetEE_PW_THRSH() { return mEE_PW_THRSH; }
   static DisUint32  GetMAX_PDU_SIZE_OCTETS() { return mMAX_PDU_SIZE_OCTETS; }

   static unsigned int GetSuppressErrorMask();

   static void SetProtocolVersion(DisEnum8 aProtocolVersion);
   static void SetExercise(DisUint8 aExercise);
   static void SetTargetsInTjFieldDflt(DisUint8 aDefault);
   static void SetApplication(DisUint16 aApplication);
   static void SetSite(DisUint16 aSite);
   static void SetHrtBeatTimer(DisFloat32 aHrtBeatTimer);
   static void SetHrtBeatMoveTimer(DisFloat32 aHrtBeatMoveTimer);
   static void SetHrtBeatDE_FireTimer(DisFloat32 aHrtBeatDE_FireTimer);
   static void SetHrtBeatMplier(DisFloat32 aHrtBeatMplier);
   static void SetDRAOrientThrsh(DisFloat32 aDRAOrientThrsh);
   static void SetDRAPosThrsh(DisFloat32 aDRAOrientThrsh);
   static void SetEEAzThrsh(DisFloat32 aEEAzThrsh);
   static void SetEEElThrsh(DisFloat32 aEEElThrsh);
   static void SetEE_MaxBeamsThrsh(DisUint8 a);
   static void SetDE_PrecisionAimingThrsh(DisFloat32 aDE_PrecisionAimingThrsh);
   static void SetDE_EnergyThrsh(DisFloat32 aDE_EnergyThrsh);

   static void SetEE_AZ_THRSH(DisFloat32 aEE_AZ_THRSH) { mEE_AZ_THRSH = aEE_AZ_THRSH; }
   static void SetEE_EL_THRSH(DisFloat32 aEE_EL_THRSH) { mEE_EL_THRSH = aEE_EL_THRSH; }
   static void SetEE_ERP_THRSH(DisFloat32 aEE_ERP_THRSH) { mEE_ERP_THRSH = aEE_ERP_THRSH; }
   static void SetEE_FREQ_THRSH(DisFloat32 aEE_FREQ_THRSH) { mEE_FREQ_THRSH = aEE_FREQ_THRSH; }
   static void SetEE_FRNG_THRSH(DisFloat32 aEE_FRNG_THRSH) { mEE_FRNG_THRSH = aEE_FRNG_THRSH; }
   static void SetEE_HIGH_DENSITY_THRSH(DisUint8 aEE_HIGH_DENSITY_THRSH)
   {
      mEE_HIGH_DENSITY_THRSH = aEE_HIGH_DENSITY_THRSH;
   }
   static void SetEE_MAX_BEAMS_THRSH(DisUint8 aEE_MAX_BEAMS_THRSH) { mEE_MAX_BEAMS_THRSH = aEE_MAX_BEAMS_THRSH; }
   static void SetEE_PRF_THRSH(DisFloat32 aEE_PRF_THRSH) { mEE_PRF_THRSH = aEE_PRF_THRSH; }
   static void SetEE_PW_THRSH(DisFloat32 aEE_PW_THRSH) { mEE_PW_THRSH = aEE_PW_THRSH; }
   static void SetMAX_PDU_SIZE_OCTETS(DisUint32 aMAX_PDU_SIZE_OCTETS) { mMAX_PDU_SIZE_OCTETS = aMAX_PDU_SIZE_OCTETS; }
   static void SetSuppressErrorMask(unsigned int aSuppressErrorMask);

   static bool ShowError(unsigned int aErrorMask);

   static const unsigned int cIO_ERRORS;
   static const unsigned int cCONNECT_ERRORS;

   // These constants are fixed by the IEEE standard
   static const DisUint16 cALL_APPLIC;
   static const DisUint8  cALL_BEAMS;
   static const DisUint8  cALL_EMITTERS;
   static const DisUint16 cALL_ENTITIES;
   static const DisUint16 cALL_SITES;
   static const DisUint16 cNO_APPLIC;
   static const DisUint8  cNO_BEAM;
   static const DisUint8  cNO_EMITTER;
   static const DisUint16 cNO_ENTITY;
   static const DisUint16 cNO_SITE;
   static const DisUint16 cRQST_ASSIGN_ID;

   static const DisEntityId cD_SPOT_NO_ENTITY;
   static const DisEntityId cENTITY_ID_UNKNOWN;
   static const DisEntityId cNO_SPECIFIC_ENTITY;
   static const DisEntityId cTARGET_ID_UNKNOWN;

private:
   static DisEnum8     mProtocolVersion;
   static DisUint8     mExercise;
   static DisUint16    mApplication;
   static DisUint16    mSite;
   static DisFloat32   mHrtBeatTimer;
   static DisFloat32   mHrtBeatMoveTimer;
   static DisFloat32   mHrtBeatDE_FireTimer;
   static DisFloat32   mHrtBeatMplier;
   static DisFloat32   mDRAOrientThrsh;
   static DisFloat32   mDRAPosThrsh;
   static DisFloat32   mDE_PrecisionAimingThrsh;
   static DisFloat32   mDE_EnergyThrsh;
   static DisFloat32   mEE_AZ_THRSH;
   static DisFloat32   mEE_EL_THRSH;
   static DisFloat32   mEE_ERP_THRSH;
   static DisFloat32   mEE_FREQ_THRSH;
   static DisFloat32   mEE_FRNG_THRSH;
   static DisUint8     mEE_HIGH_DENSITY_THRSH;
   static DisUint8     mEE_MAX_BEAMS_THRSH;
   static DisFloat32   mEE_PRF_THRSH;
   static DisFloat32   mEE_PW_THRSH;
   static DisUint32    mMAX_PDU_SIZE_OCTETS;
   static unsigned int mSuppressErrorMask;
};

// Static
inline void Dis::SetProtocolVersion(DisEnum8 aProtocolVersion)
{
   mProtocolVersion = aProtocolVersion;
}

// Static
inline void Dis::SetExercise(DisUint8 aExercise)
{
   mExercise = aExercise;
}

// Static
inline void Dis::SetApplication(DisUint16 aApplication)
{
   mApplication = aApplication;
}

// Static
inline void Dis::SetSite(DisUint16 aSite)
{
   mSite = aSite;
}

// Static
inline void Dis::SetHrtBeatTimer(DisFloat32 aHrtBeatTimer)
{
   mHrtBeatTimer = aHrtBeatTimer;
}

// Static
inline void Dis::SetHrtBeatMoveTimer(DisFloat32 aHrtBeatMoveTimer)
{
   mHrtBeatMoveTimer = aHrtBeatMoveTimer;
}

// Static
inline void Dis::SetHrtBeatDE_FireTimer(DisFloat32 aHrtBeatDE_FireTimer)
{
   mHrtBeatDE_FireTimer = aHrtBeatDE_FireTimer;
}

// Static
inline void Dis::SetHrtBeatMplier(DisFloat32 aHrtBeatMplier)
{
   mHrtBeatMplier = aHrtBeatMplier;
}

// Static
inline void Dis::SetDRAOrientThrsh(DisFloat32 aDRAOrientThrsh)
{
   mDRAOrientThrsh = aDRAOrientThrsh;
}

// Static
inline void Dis::SetDRAPosThrsh(DisFloat32 aDRAOrientThrsh)
{
   mDRAOrientThrsh = aDRAOrientThrsh;
}

// Static
inline void Dis::SetEEAzThrsh(DisFloat32 aEEAzThrsh)
{
   SetEE_AZ_THRSH(aEEAzThrsh);
}

// Static
inline void Dis::SetEEElThrsh(DisFloat32 aEEElThrsh)
{
   SetEE_EL_THRSH(aEEElThrsh);
}

// Static
inline void Dis::SetDE_PrecisionAimingThrsh(DisFloat32 aDE_PrecisionAimingThrsh)
{
   mDE_PrecisionAimingThrsh = aDE_PrecisionAimingThrsh;
}

// Static
inline void Dis::SetDE_EnergyThrsh(DisFloat32 aDE_EnergyThrsh)
{
   mDE_EnergyThrsh = aDE_EnergyThrsh;
}

// Static
inline void Dis::SetEE_MaxBeamsThrsh(DisUint8 aDefault)
{
   SetEE_MAX_BEAMS_THRSH(aDefault);
}

// Static
inline void Dis::SetTargetsInTjFieldDflt(DisUint8 aDefault)
{
   SetEE_HIGH_DENSITY_THRSH(aDefault);
}

#endif
