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

#ifndef WSFEW_SLB_EFFECT_HPP
#define WSFEW_SLB_EFFECT_HPP

#include "wsf_mil_export.h"

#include <iosfwd>
#include <map>
#include <vector>

class UtInput;
class UtInputBlock;

class WsfAntennaPattern;
class WsfEM_Antenna;
#include "WsfEM_Interaction.hpp"
class WsfEM_Rcvr;
class WsfEW_EA;
class WsfEW_EP;
#include "WsfEW_PowerEffect.hpp"

//! WsfEW_SLB_Effect models and applies Sidelobe Blanker (SLB) effects.

class WSF_MIL_EXPORT WsfEW_SLB_Effect : public WsfEW_PowerEffect
{
public:
   using BlankingMap         = std::map<unsigned int, double>;
   using TargetBlankingTable = std::vector<std::pair<double, double>>;

   enum SaturationEffect
   {
      cSE_DUTYCYCLELIMITEFFECT, //!< Apply saturation at a specified duty cycle
      cSE_NOSATURATIONEFFECT    //!< Don't care about saturation effects
   };

   enum TargetBlankingEffect
   {
      cTB_DUTYCYCLEALGORITHM,   //!< Apply target blanking via an algorithm and a uniform random number draw, used by
                                //!< external sources
      cTB_DUTYCYCLEPROBABILITY, //!< Apply target blanking via a uniform probability comparison with the duty-cycle.
      cTB_DUTYCYCLETABLE,       //!< Apply target blanking via an input table and a uniform random number draw
      cTB_NOTARGETBLANKING      //!< Don't care about target blanking effects
   };


   enum BlankerState
   {
      cBLANKING,    //!< Blanker functioning
      cOFF,         //!< Blanker off
      cSTALLED,     //!< Blanker stalled, no longer blanking
      cMAINLOBE,    //!< Jammer signal in mainlobe or power in Aux is less than main
      cPOWERLIMITED //!< Jammer signal is outside the operating power region
   };

   WsfEW_SLB_Effect();
   WsfEW_SLB_Effect& operator=(const WsfEW_SLB_Effect&) = delete;
   ~WsfEW_SLB_Effect() override;
   WsfEW_Effect* Clone() const override;
   bool          Initialize(WsfSimulation& aSimulation) override;
   bool          ProcessInput(UtInput& aInput) override;

   virtual bool ProcessTargetBlankingBlock(UtInputBlock& aInputBlock);
   virtual bool ProcessTargetBlankingInput(UtInput& aInput);

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

   //! @name Data Accessors.
   //@{
   virtual double      GetBlankingThreshold() const { return mBlankingThreshold; }
   virtual WsfEM_Rcvr* GetAuxRcvrPtr() { return mAuxRcvrPtr; }
   //@}

protected:
   WsfEW_SLB_Effect(const WsfEW_SLB_Effect& aSrc);
   bool AddTargetBlankingProbability(double aDutyCycle, double aProbability);

   TargetBlankingEffect mTargetBlankingEffect;
   TargetBlankingTable  mTargetBlankingTable;

private:
   double GetTargetBlankingProbability(double aDutyCycle) const;

   // Input data variables
   WsfEM_Rcvr*        mAuxRcvrPtr;
   WsfEM_Antenna*     mAuxAntennaPtr;
   WsfAntennaPattern* mAuxAntennaPatternPtr;

   double mAuxBeamTilt;
   bool   mExplicitAuxBeamTilt;

   double mAuxThresholds[2];
   double mMainThresholds[2];

   BlankingMap mBlankingMap;
   double      mBlankingThreshold;

   double           mSaturationDutyCycle;
   SaturationEffect mSaturationEffect;

   // Current/Updated data variables
   BlankerState mBlankerState;
   // double             mAuxChannelJammerPower;
   bool mTargetBlanked;
};

#endif
