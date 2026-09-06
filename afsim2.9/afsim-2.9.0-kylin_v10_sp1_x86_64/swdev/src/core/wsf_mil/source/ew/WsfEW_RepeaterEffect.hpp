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

#ifndef WSFEW_REPEATEREFFECT_HPP
#define WSFEW_REPEATEREFFECT_HPP

#include "wsf_mil_export.h"

#include <cfloat>
#include <iosfwd>
#include <map>
#include <vector>

class UtInput;

class WsfEW_EA;
class WsfEW_EP;
#include "WsfEW_RadiusEffect.hpp"
#include "WsfEW_Result.hpp"
#include "WsfEW_Types.hpp"

//! WsfEW_RepeaterEffect models and applies calculated physical power gain effects to the jamming power.
//!
//! Can be associated with an EA or EP technique to increase or decrease the physical jammer power by a
//! factor via automated methods based on received transmitted powers.

class WSF_MIL_EXPORT WsfEW_RepeaterEffect : public WsfEW_RadiusEffect
{
public:
   enum ControlMethod
   {
      cCM_NONE                 = 1,
      cCM_USE_REPEATER         = cCM_NONE << 1,
      cCM_USE_ACTUALS          = cCM_NONE << 2,
      cCM_USE_REPEATER_ACTUALS = cCM_NONE << 3
   };

   enum GainControlType
   {
      cMG_NONE        = 1,
      cMG_DESIRED_RCS = cMG_NONE << 1,
      cMG_MIN_J_TO_N  = cMG_NONE << 2,
      cMG_MIN_J_TO_S  = cMG_NONE << 3,
      cMG_MIN_DETECT  = cMG_NONE << 4,
      cMG_MASKING     = cMG_NONE << 5
   };

   WsfEW_RepeaterEffect();
   WsfEW_RepeaterEffect& operator=(const WsfEW_RepeaterEffect&) = delete;
   ~WsfEW_RepeaterEffect() override                             = default;
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

   //! @name Repeater effect data Accessor/Mutator methods.
   //@{

   ControlMethod GetControlMethod(WsfStringId aSystemTypeId) const;
   void          SetControlMethod(WsfStringId aSystemTypeId, ControlMethod aControlMethod);

   double GetMinimumGainFactor(WsfStringId aSystemTypeId) const;
   double GetMaximumGainFactor(WsfStringId aSystemTypeId) const;
   void   SetGainFactors(WsfStringId aSystemTypeId, double aMinimumGainFactor, double aMaximumGainFactor);

   int GetGainControlTypeMask(WsfStringId aSystemTypeId) const;

   double GetDesiredJtoN(WsfStringId aSystemTypeId) const;
   void   SetDesiredJtoN(WsfStringId aSystemTypeId, double aDesiredJtoN);

   double GetDesiredJtoS(WsfStringId aSystemTypeId) const;
   void   SetDesiredJtoS(WsfStringId aSystemTypeId, double aDesiredJtoS);

   double GetDesiredFT_RCS(WsfStringId aSystemTypeId) const;
   void   SetDesiredFT_RCS(WsfStringId aSystemTypeId, double aDesiredFT_MaskingRCS);

   double GetMinDetectFactor(WsfStringId aSystemTypeId) const;
   void   SetMinDetectFactor(WsfStringId aSystemTypeId, double aMinDetectFactor);

   double GetMaskingFactor(WsfStringId aSystemTypeId) const;
   void   SetMaskingFactor(WsfStringId aSystemTypeId, double aMaskingFactor);

   //@}

protected:
   class WSF_MIL_EXPORT RepeaterEffectData : public WsfEW_RadiusEffect::RadiusData
   {
   public:
      RepeaterEffectData();
      RepeaterEffectData& operator=(const RepeaterEffectData&) = delete;
      ~RepeaterEffectData() override                           = default;

      bool ProcessInput(UtInput& aInput) override;
      bool Initialize(WsfSimulation& aSimulation) override;

      EW_EffectData* Clone() const override;

      void Reset() override;

      ControlMethod mControlMethod;
      double        mMinimumGainFactor;
      double        mMaximumGainFactor;
      int           mGainControlTypeMask;
      double        mDesiredJtoN;
      double        mDesiredJtoS;
      double        mDesiredFT_RCS;
      bool          mExplicitDesiredFT_RCS;
      double        mMaskingFactor;
      double        mMinDetectFactor;

   protected:
      RepeaterEffectData(const RepeaterEffectData& aSrc) = default;

      void AddGainControlType(GainControlType aGainControlType);
      void SetGainControlType(GainControlType aGainControlType);
   };

   WsfEW_RepeaterEffect(const WsfEW_RepeaterEffect& aSrc);

   //! Get the current repeater factor for last call to UpdateEffects(...) or UpdateRepeaterFactor(...).
   double GetRepeaterFactor() { return mRepeaterFactor; }

   void UpdateGainFactors(const WsfEM_Interaction&               aTargetInteraction,
                          const WsfEM_Interaction&               aJammerToTgtInteraction,
                          const ut::optional<WsfEM_Interaction>& aTgtToJammerInteraction,
                          double                                 aCurrentGains[]);

   //! @name Effect Data methods.
   //@{
   EffectData* NewEffectData() override { return new RepeaterEffectData(); }
   EffectData* PropagateEffectDataPtr(WsfStringId aSystemTypeId) override;
   //@}

private:
   using ReapeaterEffectDataMap = std::map<WsfStringId, RepeaterEffectData*>; //!< Local copy of the mEffectDataMap to
                                                                              //!< prevent casting within this class.
   ReapeaterEffectDataMap mRepeaterEffectDataMap; //!< Map of the System Type Id to the data required for this technique.

   double mRepeaterFactor; //!< Current repeater factor for last call to UpdateEffect(...).
   double mMinGainFactor;  //!< Current min gain factor for last call to UpdateEffect(...).
   double mMaxGainFactor;  //!< Current max gain factor for last call to UpdateEffect(...).
};

#endif
