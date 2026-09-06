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

#ifndef WSFEW_PULSEEFFECT_HPP
#define WSFEW_PULSEEFFECT_HPP

#include "wsf_mil_export.h"

#include <iosfwd>
#include <map>
#include <vector>

class UtInput;

class WsfEW_EA;
class WsfEW_EP;
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEW_Effect.hpp"
#include "WsfEW_PowerEffect.hpp"


//! WsfEW_PulseEffect models and applies base pulse effects for use by other pulse type effects classes.

class WSF_MIL_EXPORT WsfEW_PulseEffect : public WsfEW_PowerEffect
{
public:
   WsfEW_PulseEffect();
   WsfEW_PulseEffect& operator=(const WsfEW_PulseEffect&) = delete;
   ~WsfEW_PulseEffect() override                          = default;
   WsfEW_Effect* Clone() const override;
   bool          Initialize(WsfSimulation& aSimulation) override;
   bool          ProcessInput(UtInput& aInput) override;

   void UpdateEffects(double                                 aSimTime,
                      const WsfEM_Interaction&               aTargetInteraction,
                      const WsfEM_Interaction&               aJammerToTgtInteraction,
                      const ut::optional<WsfEM_Interaction>& aTgtToJammerInteraction,
                      WsfEW_Effects&                         aEW_Effects,
                      WsfEW_Effect*                          aEW_EffectPtr = nullptr) override;

   //! Set the pulse density factor to be used and reset in inheriting classes via UpdateEffects(..).
   //! @param aPulseDensityFactor The pulse density factor.
   void SetPulseDensityFactor(double aPulseDensityFactor) { mPulseDensityFactor = aPulseDensityFactor; }

   //! Set the current jamming pulse density to be set in inheriting classes via UpdateEffects(..) and used.
   //! @param aJammingPulseDensity The jamming pulse density.
   void SetJammingPulseDensity(double aJammingPulseDensity) { mJammingPulseDensity = aJammingPulseDensity; }

   //! Get the current jamming pulse density.
   //! @return The jamming pulse density, not set if it is <= 0.0.
   double GetJammingPulseDensity() const override { return mJammingPulseDensity; }
   double GetJammingPulseDensity(WsfEM_Xmtr* aXmtrPtr) const override;

   virtual void UpdateJammerPulseDensity(WsfEM_Xmtr* aJammerXmtrPtr, WsfEM_Xmtr* aRadarXmtrPtr);

   //! @name Effect Data methods.
   //@{

   double       GetJammerPulseDensity(WsfStringId aSystemTypeId) const;
   virtual void SetJammerPulseDensity(WsfStringId aSystemTypeId, double aJammerPulseDensity, bool aIsExplicit = false);
   bool         ExplicitJPD(WsfStringId aSystemTypeId);
   //@}

protected:
   class WSF_MIL_EXPORT PulseData : public WsfEW_PowerEffect::PowerData
   {
   public:
      PulseData();
      PulseData& operator=(const PulseData&) = delete;
      ~PulseData() override                  = default;

      bool ProcessInput(UtInput& aInput) override;
      bool Initialize(WsfSimulation& aSimulation) override;

      EW_EffectData* Clone() const override;

      void Reset() override;

      bool   ExplicitJPD() const { return mExplicitJPD; }
      double GetJammerPulseDensity() const { return mJammerPulseDensity; }
      void   SetJammerPulseDensity(double aJammerPulseDensity, bool aIsExplicit = false);

   protected:
      PulseData(const PulseData& aSrc) = default;

   private:
      double mJammerPulseDensity; //!< Pulse density of the applied jamming technique.
      bool   mExplicitJPD;        //!< Jammer pulse density was explicitly set.
   };

   WsfEW_PulseEffect(const WsfEW_PulseEffect& aSrc);

   //! @name Effect Data methods.
   //@{
   EffectData* NewEffectData() override { return new PulseData(); }
   EffectData* PropagateEffectDataPtr(WsfStringId aSystemTypeId) override;
   //@}

private:
   using PulseDataMap =
      std::map<WsfStringId, PulseData*>; //!< Local copy of the mEffectDataMap to prevent casting within this class.
   PulseDataMap mPulseDataMap;           //!< Map of the System Type Id to the data required for this technique.

   //! Pulse density factor modifier  applied by outside methods. Should be reset after each use.
   double mPulseDensityFactor;

   double mJammingPulseDensity; //!< Current jamming pulse density in use for the inheriting effect.
};

#endif
