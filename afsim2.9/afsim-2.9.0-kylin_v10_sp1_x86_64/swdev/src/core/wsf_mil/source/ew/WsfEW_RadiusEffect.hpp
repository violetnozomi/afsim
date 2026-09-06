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

#ifndef WSFEW_RADIUSEFFECT_HPP
#define WSFEW_RADIUSEFFECT_HPP

#include "wsf_mil_export.h"

#include <iosfwd>
#include <map>

class UtInput;

#include "WsfEM_Interaction.hpp"
class WsfEW_EA;
class WsfEW_EP;
#include "WsfEW_Effect.hpp"
#include "WsfEW_Types.hpp"


//! WsfEW_RadiusEffect models and applies delta gain effects to the jammer power.
//!
//! Can be associated with an EA or EP technique to increase or decrease the jammer power by a factor.

class WSF_MIL_EXPORT WsfEW_RadiusEffect : public WsfEW_Effect
{
public:
   //! Enumeration for the type of limitation that is imposed
   //! Defines the target range wrt to the jammer range relative to the receiver
   enum RadiusLimitType
   {
      cJRL_NONE    = 0, //!< No target jammer range limitations defined.
      cJRL_INSIDE  = 1, //!< Limitation for target range INSIDE jammer range wrt receiver.
      cJRL_OUTSIDE = 2, //!< Limitation for target range OUTSIDE jammer range wrt receiver.
      cJRL_BOTH    = 3, //!< Limitation for target range INSIDE & OUTSIDE jammer range wrt to receiver.
      cJRL_EQUAL   = 4  //!< Limitation for target range EQUAL to jammer range wrt receiver.
   };

   using RadiusTypeToValueMap = std::map<RadiusLimitType, double>;

   WsfEW_RadiusEffect();
   WsfEW_RadiusEffect& operator=(const WsfEW_RadiusEffect&) = delete;
   ~WsfEW_RadiusEffect() override                           = default;
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

   //! @name Delta gain effect Accessor/Mutator methods.
   //@{
   int GetRadiusLimitMask(WsfStringId aSystemTypeId) const;

   double GetRadiusFactor(WsfStringId aSystemTypeId) const;
   double GetRadiusFactor(WsfStringId aSystemTypeId, RadiusLimitType aJRL_Type) const;
   //@}

protected:
   class WSF_MIL_EXPORT RadiusData : public WsfEW_Effect::EffectData
   {
   public:
      RadiusData();
      RadiusData& operator=(const RadiusData&) = delete;
      ~RadiusData() override                   = default;

      bool ProcessInput(UtInput& aInput) override;
      bool Initialize(WsfSimulation& aSimulation) override;

      EW_EffectData* Clone() const override;

      void Reset() override;

      // double GetRadiusFactor() const;
      double GetRadiusFactor(RadiusLimitType aJRL_Type) const;
      void   SetRadiusFactor(unsigned int aJRL_Mask, double aRadiusFactor);

      int GetRadiusLimitMask() const { return mRadiusLimitMask; }

   protected:
      RadiusData(const RadiusData& aSrc) = default;

   private:
      int                  mRadiusLimitMask;
      RadiusTypeToValueMap mRadiusFactorMap;
   };

   WsfEW_RadiusEffect(const WsfEW_RadiusEffect& aSrc);

   //! @name Effect Data methods.
   //@{
   EffectData* NewEffectData() override { return new RadiusData(); }
   EffectData* PropagateEffectDataPtr(WsfStringId aSystemTypeId) override;
   //@}

private:
   using RadiusDataMap =
      std::map<WsfStringId, RadiusData*>; //!< Local copy of the mEffectDataMap to prevent casting within this class.

   RadiusDataMap mRadiusDataMap; //!< Map of the System Type Id to the data required for this technique.

   RadiusLimitType mCurrentJRL_Type;
};

#endif
