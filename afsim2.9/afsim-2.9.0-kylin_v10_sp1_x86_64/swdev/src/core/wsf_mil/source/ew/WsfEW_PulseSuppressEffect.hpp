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

#ifndef WSFEW_PULSESUPPRESSEFFECT_HPP
#define WSFEW_PULSESUPPRESSEFFECT_HPP

#include "wsf_mil_export.h"

#include <iosfwd>
#include <map>
#include <vector>

class UtInput;

class WsfEW_EA;
class WsfEW_EP;
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEW_PulseEffect.hpp"
#include "WsfObject.hpp"

//! WsfEW_PulseSuppressEffect models and applies pulse suppression effects based on multiple
//! different suppression types.

class WSF_MIL_EXPORT WsfEW_PulseSuppressEffect : public WsfEW_PulseEffect
{
public:
   enum SuppressionType
   {
      cST_NONE        = 1,
      cST_FREQUENCY   = cST_NONE << 1,
      cST_PW          = cST_NONE << 2,
      cST_PRF         = cST_NONE << 3,
      cST_MODULATION  = cST_NONE << 4,
      cST_COHERENT    = cST_NONE << 5,
      cST_NONCOHERENT = cST_NONE << 6,
      cST_PULSE_RATIO = cST_NONE << 7
   };

   WsfEW_PulseSuppressEffect();
   WsfEW_PulseSuppressEffect& operator=(const WsfEW_PulseSuppressEffect&) = delete;
   ~WsfEW_PulseSuppressEffect() override                                  = default;
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

protected:
   WsfEW_PulseSuppressEffect(const WsfEW_PulseSuppressEffect& aSrc) = default;

private:
   struct Limits
   {
      double mLower;
      double mUpper;
      int    mModulationType;

      void Reset()
      {
         mLower          = 0.0;
         mUpper          = 0.0;
         mModulationType = WsfEW_Effect::cEC_NONE;
      }
   };

   //! Type definition for mapping of the suppression type to the suppression limit values.
   using SuppressionMap = std::map<unsigned int, Limits>;

   double GetSuppressionValue(SuppressionType aSuppressType, double aCheckValue);
   double GetSuppressionValue(SuppressionType aSuppressType, ModulationType aModType);

   SuppressionMap mAllowedMap; //! Allow limits map for allowed suppression types.
   SuppressionMap mRejectMap;  //! Reject limits map for suppression types.
   unsigned int   mSuppressTypeMask;
   double         mSuppressGain;
   double         mNoiseSuppressGain;
   double         mCohSuppressGain;
};

#endif
