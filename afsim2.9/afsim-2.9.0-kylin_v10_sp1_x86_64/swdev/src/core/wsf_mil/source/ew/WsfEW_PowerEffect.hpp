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

#ifndef WSFEW_POWEREFFECT_HPP
#define WSFEW_POWEREFFECT_HPP

#include "wsf_mil_export.h"

#include <iosfwd>
#include <map>
#include <vector>

#include "TblLookup.hpp"
class UtInput;

class WsfEW_EA;
class WsfEW_EP;
#include "WsfEM_Interaction.hpp"
#include "WsfEW_RepeaterEffect.hpp"
#include "WsfEW_Types.hpp"


//! WsfEW_PowerEffect models and applies delta gain effects to the jammer power.
//!
//! Can be associated with an EA or EP technique to increase or decrease the jammer power by a factor.

class WSF_MIL_EXPORT WsfEW_PowerEffect : public WsfEW_RepeaterEffect
{
public:
   WsfEW_PowerEffect();
   WsfEW_PowerEffect& operator=(const WsfEW_PowerEffect&) = delete;
   ~WsfEW_PowerEffect() override                          = default;
   WsfEW_Effect* Clone() const override;
   bool          Initialize(WsfSimulation& aSimulation) override;
   bool          ProcessInput(UtInput& aInput) override;

   void ApplyUnmitigatedEffect(WsfEW_Effects& aEW_Effects) const override;

   void ApplyMitigatedEffect(WsfEW_Effect* aEW_EffectPtr, WsfEW_Effects& aEW_Effects) const override;

   void UpdateEffects(double                                 aSimTime,
                      const WsfEM_Interaction&               aTargetInteraction,
                      const WsfEM_Interaction&               aJammerToTgtInteraction,
                      const ut::optional<WsfEM_Interaction>& aTgtToJammerInteraction,
                      WsfEW_Effects&                         aEW_Effects,
                      WsfEW_Effect*                          aEW_EffectPtr = nullptr) override;

   //! @name Power Effect Data methods.
   //@{

   double GetJammingPowerGain(WsfStringId aSystemTypeId) const;
   void   SetJammingPowerGain(WsfStringId aSystemTypeId, double aJammingPowerGain);

   double GetModulationGain(WsfStringId aSystemTypeId) const;
   void   SetModulationGain(WsfStringId aSystemTypeId, double aModulaitonGain);

   double GetSignalPowerGain(WsfStringId aSystemTypeId) const;
   void   SetSignalPowerGain(WsfStringId aSystemTypeId, double aSignalPowerGain);

   double GetRcvrNoiseGain(WsfStringId aSystemTypeId) const;
   void   SetRcvrNoiseGain(WsfStringId aSystemTypeId, double aRcvrNoiseGain);

   double GetJtoSGain(WsfStringId aSystemTypeId, Coherency aCoherencyType = cEC_NONE) const;
   size_t GetJtoSTableSize(WsfStringId aSystemTypeId) const;

   //@}

protected:
   class WSF_MIL_EXPORT PowerData : public WsfEW_RepeaterEffect::RepeaterEffectData
   {
   public:
      struct JtoXGainTable
      {
         TblIndVarU<double> mJtoX;
         TblDepVar1<double> mGain;
      };

      PowerData();
      PowerData& operator=(const PowerData&) = delete;
      ~PowerData() override                  = default;

      bool ProcessInput(UtInput& aInput) override;
      bool Initialize(WsfSimulation& aSimulation) override;

      EW_EffectData* Clone() const override;

      void Reset() override;

      double        mJammingPowerGain;
      double        mModulationGain;
      JtoXGainTable mJtoSGainTable; //!< Table is modulation gain vs. j-to-s ratio
      double        mSignalPowerGain;
      double        mRcvrNoiseGain;

   protected:
      PowerData(const PowerData& aSrc) = default;
   };

   WsfEW_PowerEffect(const WsfEW_PowerEffect& aSrc);


   //! @name Effect Data methods.
   //@{
   EffectData* NewEffectData() override { return new PowerData(); }
   EffectData* PropagateEffectDataPtr(WsfStringId aSystemTypeId) override; //!< Local copy of the mEffectDataMap to
                                                                           //!< prevent casting within this class.
   //@}

private:
   using PowerDataMap = std::map<WsfStringId, PowerData*>;

   PowerDataMap mPowerDataMap; //!< Map of the System Type Id to the data required for this technique.

   //! The J-to-S ratio for the last calculated interaction.
   //!  [0] - Noise J/S
   //!  [1] - Non-Coherent Pulse J/S
   //!  [2] - Coherent & Coherent Pulse J/S
   double mJammerToSignal[3];
};

#endif
