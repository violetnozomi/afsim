// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFMILEVENTRESULTS_HPP
#define WSFMILEVENTRESULTS_HPP

#include "wsf_mil_export.h"

#include <ostream>

class WsfDirectedEnergyWeapon;
#include "WsfEventResult.hpp"
class WsfEM_Interaction;
class WsfGuidanceComputer;
class WsfImplicitWeapon;
class WsfEM_Rcvr;
class WsfEM_Xmtr;
class WsfEW_Result;
class WsfPlatform;
class WsfSensor;
class WsfSimulation;
class WsfTrack;
#include "WsfWeapon.hpp"
class WsfWeaponEngagement;

namespace wsf
{
namespace event
{

// ===================================================================================================
class WSF_MIL_EXPORT DirectedEnergyWeaponAbortShot : public Result
{
public:
   static constexpr const char* cNAME = "DIRECTED_ENERGY_WEAPON_ABORT_SHOT";
   DirectedEnergyWeaponAbortShot(double                     aSimTime,
                                 WsfDirectedEnergyWeapon*   aWeaponPtr,
                                 const WsfWeaponEngagement* aEngagementPtr,
                                 Settings                   aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mWeaponPtr(aWeaponPtr)
      , mEngagementPtr(aEngagementPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfDirectedEnergyWeapon* GetWeapon() const { return mWeaponPtr; }
   const WsfWeaponEngagement*     GetEngagement() const { return mEngagementPtr; }

private:
   WsfDirectedEnergyWeapon*   mWeaponPtr;
   const WsfWeaponEngagement* mEngagementPtr;
};

// ===================================================================================================
class WSF_MIL_EXPORT DirectedEnergyWeaponBeginShot : public Result
{
public:
   static constexpr const char* cNAME = "DIRECTED_ENERGY_WEAPON_BEGIN_SHOT";
   DirectedEnergyWeaponBeginShot(double                     aSimTime,
                                 WsfDirectedEnergyWeapon*   aWeaponPtr,
                                 const WsfWeaponEngagement* aEngagementPtr,
                                 Settings                   aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mWeaponPtr(aWeaponPtr)
      , mEngagementPtr(aEngagementPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfDirectedEnergyWeapon* GetWeapon() const { return mWeaponPtr; }
   const WsfWeaponEngagement*     GetEngagement() const { return mEngagementPtr; }

private:
   WsfDirectedEnergyWeapon*   mWeaponPtr;
   const WsfWeaponEngagement* mEngagementPtr;
};

// ===================================================================================================
class WSF_MIL_EXPORT DirectedEnergyWeaponEndShot : public Result
{
public:
   static constexpr const char* cNAME = "DIRECTED_ENERGY_WEAPON_END_SHOT";
   DirectedEnergyWeaponEndShot(double                     aSimTime,
                               WsfDirectedEnergyWeapon*   aWeaponPtr,
                               const WsfWeaponEngagement* aEngagementPtr,
                               Settings                   aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mWeaponPtr(aWeaponPtr)
      , mEngagementPtr(aEngagementPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfDirectedEnergyWeapon* GetWeapon() const { return mWeaponPtr; }
   const WsfWeaponEngagement*     GetEngagement() const { return mEngagementPtr; }

private:
   WsfDirectedEnergyWeapon*   mWeaponPtr;
   const WsfWeaponEngagement* mEngagementPtr;
};

// ===================================================================================================
class WSF_MIL_EXPORT DirectedEnergyWeaponUpdateShot : public Result
{
public:
   static constexpr const char* cNAME = "DIRECTED_ENERGY_WEAPON_UPDATE_SHOT";
   DirectedEnergyWeaponUpdateShot(double                     aSimTime,
                                  WsfDirectedEnergyWeapon*   aWeaponPtr,
                                  const WsfWeaponEngagement* aEngagementPtr,
                                  Settings                   aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mWeaponPtr(aWeaponPtr)
      , mEngagementPtr(aEngagementPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfDirectedEnergyWeapon* GetWeapon() const { return mWeaponPtr; }
   const WsfWeaponEngagement*     GetEngagement() const { return mEngagementPtr; }

private:
   WsfDirectedEnergyWeapon*   mWeaponPtr;
   const WsfWeaponEngagement* mEngagementPtr;
};

// ===================================================================================================
class WSF_MIL_EXPORT DirectedEnergyWeaponCooldownComplete : public Result
{
public:
   static constexpr const char* cNAME = "DIRECTED_ENERGY_WEAPON_COOLDOWN_COMPLETE";
   DirectedEnergyWeaponCooldownComplete(double aSimTime, WsfWeapon* aWeaponPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mWeaponPtr(aWeaponPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfWeapon* GetWeapon() const { return mWeaponPtr; }

private:
   WsfWeapon* mWeaponPtr;
};

// ===================================================================================================
class WSF_MIL_EXPORT ImplicitWeaponBeginEngagement : public Result
{
public:
   static constexpr const char* cNAME = "IMPLICIT_WEAPON_BEGIN_ENGAGEMENT";
   ImplicitWeaponBeginEngagement(double                     aSimTime,
                                 WsfImplicitWeapon*         aWeaponPtr,
                                 const WsfWeaponEngagement* aEngagementPtr,
                                 Settings                   aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mWeaponPtr(aWeaponPtr)
      , mEngagementPtr(aEngagementPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfImplicitWeapon*   GetWeapon() const { return mWeaponPtr; }
   const WsfWeaponEngagement* GetEngagement() const { return mEngagementPtr; }

private:
   WsfImplicitWeapon*         mWeaponPtr;
   const WsfWeaponEngagement* mEngagementPtr;
};

// ===================================================================================================
class WSF_MIL_EXPORT ImplicitWeaponEndEngagement : public Result
{
public:
   static constexpr const char* cNAME = "IMPLICIT_WEAPON_END_ENGAGEMENT";
   ImplicitWeaponEndEngagement(double                     aSimTime,
                               WsfImplicitWeapon*         aWeaponPtr,
                               const WsfWeaponEngagement* aEngagementPtr,
                               Settings                   aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mWeaponPtr(aWeaponPtr)
      , mEngagementPtr(aEngagementPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfImplicitWeapon*   GetWeapon() const { return mWeaponPtr; }
   const WsfWeaponEngagement* GetEngagement() const { return mEngagementPtr; }

private:
   WsfImplicitWeapon*         mWeaponPtr;
   const WsfWeaponEngagement* mEngagementPtr;
};

// ===================================================================================================
class WSF_MIL_EXPORT JammingAttempt : public Result
{
public:
   static constexpr const char* cNAME = "JAMMING_ATTEMPT";
   JammingAttempt(double             aSimTime,
                  WsfEM_Xmtr*        aXmtrPtr,
                  WsfEM_Rcvr*        aRcvrPtr,
                  WsfEM_Interaction& aResult,
                  Settings           aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mXmtrPtr(aXmtrPtr)
      , mRcvrPtr(aRcvrPtr)
      , mResult(aResult)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfEM_Xmtr*  GetXmtr() const { return mXmtrPtr; }
   const WsfEM_Rcvr*  GetRcvr() const { return mRcvrPtr; }
   WsfEM_Interaction& GetInteraction() const { return mResult; }

private:
   WsfEM_Xmtr*        mXmtrPtr;
   WsfEM_Rcvr*        mRcvrPtr;
   WsfEM_Interaction& mResult;
};

// ===================================================================================================
class WSF_MIL_EXPORT JammingRequestCanceled : public Result
{
public:
   static constexpr const char* cNAME = "JAMMING_REQUEST_CANCELED";
   JammingRequestCanceled(double     aSimTime,
                          WsfWeapon* aWeaponPtr,
                          double     aFrequency,
                          double     aBandwidth,
                          size_t     aTargetIndex,
                          Settings   aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mWeaponPtr(aWeaponPtr)
      , mFrequency(aFrequency)
      , mBandwidth(aBandwidth)
      , mTargetIndex(aTargetIndex)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfWeapon* GetWeapon() const { return mWeaponPtr; }
   double           GetFrequency() const { return mFrequency; }
   double           GetBandwidth() const { return mBandwidth; }
   size_t           GetTargetIndex() const { return mTargetIndex; }

private:
   WsfWeapon* mWeaponPtr;
   double     mFrequency;
   double     mBandwidth;
   size_t     mTargetIndex;
};

// ===================================================================================================
class WSF_MIL_EXPORT JammingRequestInitiated : public Result
{
public:
   static constexpr const char* cNAME = "JAMMING_REQUEST_INITIATED";
   JammingRequestInitiated(double      aSimTime,
                           WsfWeapon*  aWeaponPtr,
                           double      aFrequency,
                           double      aBandwidth,
                           WsfStringId aTechniqueId,
                           size_t      aTargetIndex,
                           Settings    aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mWeaponPtr(aWeaponPtr)
      , mFrequency(aFrequency)
      , mBandwidth(aBandwidth)
      , mTechniqueId(aTechniqueId)
      , mTargetIndex(aTargetIndex)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfWeapon* GetWeapon() const { return mWeaponPtr; }
   double           GetFrequency() const { return mFrequency; }
   double           GetBandwidth() const { return mBandwidth; }
   WsfStringId      GetTechniqueId() const { return mTechniqueId; }
   size_t           GetTargetIndex() const { return mTargetIndex; }

private:
   WsfWeapon*  mWeaponPtr;
   double      mFrequency;
   double      mBandwidth;
   WsfStringId mTechniqueId;
   size_t      mTargetIndex;
};

// ===================================================================================================
class WSF_MIL_EXPORT JammingRequestUpdated : public Result
{
public:
   static constexpr const char* cNAME = "JAMMING_REQUEST_UPDATED";
   JammingRequestUpdated(double      aSimTime,
                         WsfWeapon*  aWeaponPtr,
                         double      aFrequency,
                         double      aBandwidth,
                         WsfStringId aTechniqueId,
                         size_t      aTargetIndex,
                         Settings    aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mWeaponPtr(aWeaponPtr)
      , mFrequency(aFrequency)
      , mBandwidth(aBandwidth)
      , mTechniqueId(aTechniqueId)
      , mTargetIndex(aTargetIndex)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfWeapon* GetWeapon() const { return mWeaponPtr; }
   double           GetFrequency() const { return mFrequency; }
   double           GetBandwidth() const { return mBandwidth; }
   WsfStringId      GetTechniqueId() const { return mTechniqueId; }
   size_t           GetTargetIndex() const { return mTargetIndex; }

private:
   WsfWeapon*  mWeaponPtr;
   double      mFrequency;
   double      mBandwidth;
   WsfStringId mTechniqueId;
   size_t      mTargetIndex;
};

// ===================================================================================================
class WSF_MIL_DEPRECATED_EXPORT PlatformKilled : public Result
{
public:
   static constexpr const char* cNAME = "PLATFORM_KILLED";
   PlatformKilled(double aSimTime, WsfPlatform* aPlatformPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), "PLATFORM_KILLED")
      , mPlatformPtr(aPlatformPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfPlatform* GetPlatform() const { return mPlatformPtr; }

private:
   WsfPlatform* mPlatformPtr;
};

// ===================================================================================================
class WSF_MIL_EXPORT GuidanceComputerPhaseChanged : public Result
{
public:
   static constexpr const char* cNAME = "MOVER_GUIDANCE_PHASE_CHANGED";
   GuidanceComputerPhaseChanged(double aSimTime, WsfGuidanceComputer* aComputerPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mComputerPtr(aComputerPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   WsfGuidanceComputer* GetGuidanceComputer() const { return mComputerPtr; }

private:
   WsfGuidanceComputer* mComputerPtr;
};

// ===================================================================================================
class WSF_MIL_EXPORT WeaponFireAborted : public Result
{
public:
   static constexpr const char* cNAME = "WEAPON_FIRE_ABORTED";
   WeaponFireAborted(double          aSimTime,
                     WsfWeapon*      aWeaponPtr,
                     const WsfTrack* aTargetTrackPtr,
                     double          aQuantity,
                     Settings        aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mWeaponPtr(aWeaponPtr)
      , mTargetTrackPtr(aTargetTrackPtr)
      , mQuantity(aQuantity)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfWeapon* GetWeapon() const { return mWeaponPtr; }
   const WsfTrack*  GetTrack() const { return mTargetTrackPtr; }
   double           GetQuantity() const { return mQuantity; }

private:
   WsfWeapon*      mWeaponPtr;
   const WsfTrack* mTargetTrackPtr;
   double          mQuantity;
};

// ===================================================================================================
class WSF_MIL_EXPORT WeaponFireRequested : public Result
{
public:
   static constexpr const char* cNAME = "WEAPON_FIRE_REQUESTED";
   WeaponFireRequested(double          aSimTime,
                       WsfWeapon*      aWeaponPtr,
                       const WsfTrack* aTargetTrackPtr,
                       double          aQuantity,
                       Settings        aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mWeaponPtr(aWeaponPtr)
      , mTargetTrackPtr(aTargetTrackPtr)
      , mQuantity(aQuantity)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfWeapon* GetWeapon() const { return mWeaponPtr; }
   const WsfTrack*  GetTrack() const { return mTargetTrackPtr; }
   double           GetQuantity() const { return mQuantity; }

private:
   WsfWeapon*      mWeaponPtr;
   const WsfTrack* mTargetTrackPtr;
   double          mQuantity;
};

// ===================================================================================================
class WSF_MIL_EXPORT WeaponFired : public Result
{
public:
   static constexpr const char* cNAME = "WEAPON_FIRED";
   WeaponFired(double                     aSimTime,
               const WsfWeaponEngagement* aEngagementPtr,
               const WsfTrack*            aTargetTrackPtr,
               Settings                   aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mEngagementPtr(aEngagementPtr)
      , mTargetTrackPtr(aTargetTrackPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfWeaponEngagement* GetEngagement() const { return mEngagementPtr; }
   const WsfTrack*            GetTrack() const { return mTargetTrackPtr; }

private:
   const WsfWeaponEngagement* mEngagementPtr;
   const WsfTrack*            mTargetTrackPtr;
};

// ===================================================================================================
class WSF_MIL_EXPORT WeaponHit : public Result
{
public:
   static constexpr const char* cNAME = "WEAPON_HIT";
   WeaponHit(double                     aSimTime,
             const WsfWeaponEngagement* aEngagementPtr,
             WsfPlatform*               aTargetPtr,
             Settings                   aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mEngagementPtr(aEngagementPtr)
      , mTargetPtr(aTargetPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfWeaponEngagement* GetEngagement() const { return mEngagementPtr; }
   const WsfPlatform*         GetTarget() const { return mTargetPtr; }

private:
   const WsfWeaponEngagement* mEngagementPtr;
   WsfPlatform*               mTargetPtr;
};

// ===================================================================================================
class WSF_MIL_EXPORT WeaponKilled : public Result
{
public:
   static constexpr const char* cNAME = "WEAPON_KILLED";
   WeaponKilled(double aSimTime, WsfWeapon* aWeaponPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mWeaponPtr(aWeaponPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfWeapon* GetWeapon() const { return mWeaponPtr; }

private:
   WsfWeapon* mWeaponPtr;
};

// ===================================================================================================
class WSF_MIL_EXPORT WeaponModeActivated : public Result
{
public:
   static constexpr const char* cNAME = "WEAPON_MISSED";
   WeaponModeActivated(double                    aSimTime,
                       WsfWeapon*                aWeaponPtr,
                       WsfWeapon::WsfWeaponMode* aModePtr,
                       Settings                  aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mWeaponPtr(aWeaponPtr)
      , mModePtr(aModePtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfWeapon*                GetWeapon() const { return mWeaponPtr; }
   const WsfWeapon::WsfWeaponMode* GetWeaponMode() const { return mModePtr; }

private:
   WsfWeapon*                mWeaponPtr;
   WsfWeapon::WsfWeaponMode* mModePtr;
};

// ===================================================================================================
class WSF_MIL_EXPORT WeaponModeDeactivated : public Result
{
public:
   static constexpr const char* cNAME = "WEAPON_MODE_ACTIVATED";
   WeaponModeDeactivated(double                    aSimTime,
                         WsfWeapon*                aWeaponPtr,
                         WsfWeapon::WsfWeaponMode* aModePtr,
                         Settings                  aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mWeaponPtr(aWeaponPtr)
      , mModePtr(aModePtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfWeapon*                GetWeapon() const { return mWeaponPtr; }
   const WsfWeapon::WsfWeaponMode* GetWeaponMode() const { return mModePtr; }

private:
   WsfWeapon*                mWeaponPtr;
   WsfWeapon::WsfWeaponMode* mModePtr;
};

// ===================================================================================================
class WSF_MIL_EXPORT WeaponMissed : public Result
{
public:
   static constexpr const char* cNAME = "WEAPON_MODE_DEACTIVATED";
   WeaponMissed(double                     aSimTime,
                const WsfWeaponEngagement* aEngagementPtr,
                WsfPlatform*               aTargetPtr,
                Settings                   aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mEngagementPtr(aEngagementPtr)
      , mTargetPtr(aTargetPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfWeaponEngagement* GetEngagement() const { return mEngagementPtr; }
   const WsfPlatform*         GetTarget() const { return mTargetPtr; }

private:
   const WsfWeaponEngagement* mEngagementPtr;
   WsfPlatform*               mTargetPtr;
};

// ===================================================================================================
class WSF_MIL_EXPORT WeaponNonOperational : public Result
{
public:
   static constexpr const char* cNAME = "WEAPON_NON_OPERATIONAL";
   WeaponNonOperational(double aSimTime, WsfWeapon* aWeaponPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mWeaponPtr(aWeaponPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfWeapon* GetWeapon() const { return mWeaponPtr; }

private:
   WsfWeapon* mWeaponPtr;
};

// ===================================================================================================
class WSF_MIL_EXPORT WeaponOperational : public Result
{
public:
   static constexpr const char* cNAME = "WEAPON_OPERATIONAL";
   WeaponOperational(double aSimTime, WsfWeapon* aWeaponPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mWeaponPtr(aWeaponPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfWeapon* GetWeapon() const { return mWeaponPtr; }

private:
   WsfWeapon* mWeaponPtr;
};

// ===================================================================================================
class WSF_MIL_EXPORT WeaponReloadStarted : public Result
{
public:
   static constexpr const char* cNAME = "WEAPON_RELOAD_STARTED";
   WeaponReloadStarted(double aSimTime, WsfWeapon* aWeaponPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mWeaponPtr(aWeaponPtr)
   {
   }

   void PrintCSV(std::ostream& aStream) const override;
   void Print(std::ostream& aStream) const override;

   const WsfWeapon* GetWeapon() const { return mWeaponPtr; }

private:
   WsfWeapon* mWeaponPtr;
};

// ===================================================================================================
class WSF_MIL_EXPORT WeaponReloadEnded : public Result
{
public:
   static constexpr const char* cNAME = "WEAPON_RELOAD_ENDED";
   WeaponReloadEnded(double aSimTime, WsfWeapon* aWeaponPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mWeaponPtr(aWeaponPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfWeapon* GetWeapon() const { return mWeaponPtr; }

private:
   WsfWeapon* mWeaponPtr;
};

// ===================================================================================================
class WSF_MIL_EXPORT WeaponSelected : public Result
{
public:
   static constexpr const char* cNAME = "WEAPON_SELECTED";
   WeaponSelected(double     aSimTime,
                  WsfWeapon* aWeaponPtr,
                  WsfTrack*  aTargetTrackPtr,
                  int        aStoreId,
                  Settings   aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mWeaponPtr(aWeaponPtr)
      , mTargetTrackPtr(aTargetTrackPtr)
      , mStoreId(aStoreId)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfWeapon* GetWeapon() const { return mWeaponPtr; }
   const WsfTrack*  GetTargetTrack() const { return mTargetTrackPtr; }
   int              GetStoreId() const { return mStoreId; }

private:
   WsfWeapon* mWeaponPtr;
   WsfTrack*  mTargetTrackPtr;
   int        mStoreId;
};

// ===================================================================================================
class WSF_MIL_EXPORT WeaponTerminated : public Result
{
public:
   static constexpr const char* cNAME = "WEAPON_TERMINATED";
   WeaponTerminated(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mEngagementPtr(aEngagementPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfWeaponEngagement* GetEngagement() const { return mEngagementPtr; }

private:
   const WsfWeaponEngagement* mEngagementPtr;
};

// ===================================================================================================
class WSF_MIL_EXPORT WeaponTurnedOff : public Result
{
public:
   static constexpr const char* cNAME = "WEAPON_TURNED_OFF";
   WeaponTurnedOff(double aSimTime, WsfWeapon* aWeaponPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mWeaponPtr(aWeaponPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfWeapon* GetWeapon() const { return mWeaponPtr; }

private:
   WsfWeapon* mWeaponPtr;
};

// ===================================================================================================
class WSF_MIL_EXPORT WeaponTurnedOn : public Result
{
public:
   static constexpr const char* cNAME = "WEAPON_TURNED_ON";
   WeaponTurnedOn(double aSimTime, WsfWeapon* aWeaponPtr, Settings aSettings = Settings())
      : Result(aSimTime, std::move(aSettings), cNAME)
      , mWeaponPtr(aWeaponPtr)
   {
   }

   void Print(std::ostream& aStream) const override;
   void PrintCSV(std::ostream& aStream) const override;

   const WsfWeapon* GetWeapon() const { return mWeaponPtr; }

private:
   WsfWeapon* mWeaponPtr;
};

} // namespace event
} // namespace wsf

#endif
