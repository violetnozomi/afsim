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

#ifndef WSFEW_SLC_EFFECT_HPP
#define WSFEW_SLC_EFFECT_HPP

#include "wsf_mil_export.h"

#include <iosfwd>
#include <map>
#include <vector>

class UtInput;

class WsfAntennaPattern;
class WsfEM_Antenna;
class WsfEM_Rcvr;
class WsfEW_EA;
class WsfEW_EP;
class WsfEW_Effect;
#include "WsfEW_PowerEffect.hpp"

//! WsfEW_SLC_Effect applies Sidelobe Canceler (SLC) effects of radars.

class WSF_MIL_EXPORT WsfEW_SLC_Effect : public WsfEW_PowerEffect
{
public:
   struct CancelationData
   {
      double mCancelationValue;  // The cancellation value for this jammer.
      int    mChannelsCanceled;  // The number of channels this jammer is occupying.
      double mJammerToNoise;     // The calculated JNR (main antenna beam, i.e. NOT auxiliary antenna).
      double mSLC_DegradeFactor; // The degradation factor for this jammer.
      double mPolModSwitchRate;  // The polarization modulation switch rate/frequency for this jammer if applicable.

      void Reset()
      {
         mCancelationValue  = 1.0;
         mChannelsCanceled  = 0;
         mJammerToNoise     = 0.0;
         mSLC_DegradeFactor = 1.0;
         mPolModSwitchRate  = 0.0;
      }
   };

   // Jammer Id to Jammer cancellation data
   using CancelationMap    = std::map<unsigned int, CancelationData>;
   using JNR_To_CR_PairVec = std::vector<std::pair<double, double>>;
   using CancelationTable  = std::map<int, JNR_To_CR_PairVec>;

   WsfEW_SLC_Effect();
   WsfEW_SLC_Effect& operator=(const WsfEW_SLC_Effect&) = delete;
   ~WsfEW_SLC_Effect() override;
   WsfEW_Effect* Clone() const override;
   bool          Initialize(WsfSimulation& aSimulation) override;
   bool          ProcessInput(UtInput& aInput) override;

   enum CancelationMethod
   {
      cNONE,
      cSUM,
      cINCREMENTAL
   };

   enum CancelerState
   {
      cOFF,       //!< canceler off
      cCANCELING, //!< canceler functioning
      cMAINLOBE,  //!< Jammer signal in mainlobe or power in Aux is less than main
      cSTALLED    //!< canceler stalled, no longer canceling
   };

   void ApplyUnmitigatedEffect(WsfEW_Effects& aEW_Effects) const override;

   void ApplyMitigatedEffect(WsfEW_Effect* aEW_EffectPtr, WsfEW_Effects& aEW_Effects) const override;

   void UpdateEffects(double                                 aSimTime,
                      const WsfEM_Interaction&               aTargetInteraction,
                      const WsfEM_Interaction&               aJammerToTgtInteraction,
                      const ut::optional<WsfEM_Interaction>& aTgtToJammerInteraction,
                      WsfEW_Effects&                         aEW_Effects,
                      WsfEW_Effect*                          aEW_EffectPtr = nullptr) override;

   void RemoveJammerXmtr(unsigned int aXmtrId) override;
   void RemoveJammerXmtr(WsfEM_Xmtr* aJammerXmtrPtr) override;

   //! @name External Services access.
   //! These methods support the External Services and are not part of the WSF public interface.
   //@{
   virtual int                     GetNumberCancelers() const { return mNumberCancelers; }
   virtual const CancelationTable& GetCancelationTable() const { return mCancelationTable; }
   virtual double                  GetMainThresholds(unsigned int aIndex) const { return mMainThresholds[aIndex]; }
   //@}

   //! @name Data Accessors
   //@{
   virtual double      GetCancelationLockRatio() const { return mCancelationLockRatio; }
   virtual WsfEM_Rcvr* GetAuxRcvrPtr() { return mAuxRcvrPtr; }
   //@}

protected:
   WsfEW_SLC_Effect(const WsfEW_SLC_Effect& aSrc);


private:
   bool   AddCancelationRatio(int aNumCanceledJammers, double aJammerToNoiseRatio, double aCancelationRatio);
   double GetCancelationRatio(int aNumCanceledJammers, double aJammerToNoiseRatio) const;
   double GetCancelationFactor(int aNumCanceledJammers, double aJammerToNoiseRatio) const;

   // Input data variables
   WsfEM_Rcvr*        mAuxRcvrPtr;
   WsfEM_Antenna*     mAuxAntennaPtr;
   WsfAntennaPattern* mAuxAntennaPatternPtr;

   double mAuxBeamTilt;
   bool   mExplicitAuxBeamTilt;

   double mCancelationLockRatio;
   double mMinimumPulseWidth;
   double mLoopSettlingTime;
   int    mNumberCancelers;

   double mAuxThresholds[2];
   double mMainThresholds[2];

   // bool               mCalculateSaturatedCR;
   CancelationMethod mCancelationMethod;

   CancelationTable mCancelationTable;
   bool             mExplicitCancelationTable;

   // Current/Updated data variables
   CancelerState mCancelerState;
   // double             mAuxChannelJammerPower;
   int            mCanceledJammers;
   CancelationMap mCancelationMap;
};

#endif
