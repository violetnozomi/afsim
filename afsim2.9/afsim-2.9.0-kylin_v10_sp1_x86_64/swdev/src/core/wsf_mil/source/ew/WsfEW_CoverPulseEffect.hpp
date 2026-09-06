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

#ifndef WSFEW_COVERPULSEEFFECT_HPP
#define WSFEW_COVERPULSEEFFECT_HPP

#include "wsf_mil_export.h"

#include <map>

class UtInput;
#include "UtMath.hpp"

class WsfEW_EA;
class WsfEW_EP;
#include "WsfEM_Interaction.hpp"
#include "WsfEW_PulseEffect.hpp"
#include "WsfEW_Types.hpp"


//! WsfEW_CoverPulseEffect models and applies J/S limit and probability of application effects to the jammer power.

class WSF_MIL_EXPORT WsfEW_CoverPulseEffect : public WsfEW_PulseEffect
{
public:
   WsfEW_CoverPulseEffect();
   WsfEW_CoverPulseEffect& operator=(const WsfEW_CoverPulseEffect&) = delete;
   ~WsfEW_CoverPulseEffect() override                               = default;
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

   void UpdateJammerPulseDensity(WsfEM_Xmtr* aJammerXmtrPtr, WsfEM_Xmtr* aRadarXmtrPtr) override;

protected:
   class WSF_MIL_EXPORT CoverPulseData : public WsfEW_PulseEffect::PulseData
   {
   public:
      CoverPulseData();
      CoverPulseData& operator=(const CoverPulseData&) = delete;
      ~CoverPulseData() override                       = default;

      bool ProcessInput(UtInput& aInput) override;
      bool Initialize(WsfSimulation& aSimulation) override;

      EW_EffectData* Clone() const override;

      void Reset() override;

      double mCoverProbability; //!< Probability of the effect being applied
      double mRequiredJtoS;     //!< Required J/S for the effect to be applied.

   protected:
      CoverPulseData(const CoverPulseData& aSrc) = default;
   };

   WsfEW_CoverPulseEffect(const WsfEW_CoverPulseEffect& aSrc);

   //! @name Effect Data methods.
   //@{
   double GetCoverProbability(WsfStringId aSystemTypeId) const;
   double GetRequiredJtoS(WsfStringId aSystemTypeId) const;

   EffectData* NewEffectData() override;
   EffectData* PropagateEffectDataPtr(WsfStringId aSystemTypeId) override;
   //@}

private:
   using CoverPulseDataMap = std::map<WsfStringId, CoverPulseData*>;
   CoverPulseDataMap mCoverPulseDataMap; //!< Local copy of the mEffectDataMap to prevent casting within this class.

   double mCoverProbability;   //!< The application probability for the last calculated interaction.
   double mJammingPowerFactor; //!< The jamming power factor for the last calculated interaction.
};

#endif
