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

#ifndef WSFEW_RPJ_EFFECT_HPP
#define WSFEW_RPJ_EFFECT_HPP

#include "wsf_mil_export.h"

#include <iosfwd>
#include <map>

class UtInput;

#include "WsfEM_Interaction.hpp"
#include "WsfEW_PulseEffect.hpp"

//! WsfEW_RPJ_Effect that models and applies Random Pulse Jamming (RPJ) effects.

class WSF_MIL_EXPORT WsfEW_RPJ_Effect : public WsfEW_PulseEffect
{
public:
   using PD_GainMap        = std::map<double, double>;
   using JNR_PD_GainMapMap = std::map<double, PD_GainMap>;

   using PulseRangeVec = std::vector<double>;

   WsfEW_RPJ_Effect();
   WsfEW_RPJ_Effect& operator=(const WsfEW_RPJ_Effect&) = delete;
   ~WsfEW_RPJ_Effect() override                         = default;
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

   //! @name RPJ Accessor/Mutator methods.
   //@{

   //! Get the calculated JNR for the last call to UpdateEffects(..).
   double GetJammerToNoise() { return mJammerToNoise; }

   //! Get the calculated pulse ranges for the last call to UpdateEffects(..).
   void GetPulseRanges(PulseRangeVec& aPulseRanges) { aPulseRanges = mPulseRangesVec; };

   //! Allows for access to the entered tables to do lookups for different values
   double GetRPJ_Gain(WsfStringId aRadarTypeId, double aJammerToNoise, double aPulseDensity);

   //! Get the calculated gain for the last call to UpdateEffects(..).
   double GetRPJ_Gain() { return mRPJ_Gain; }

   //! Get the calculated pulses in the sample for the last call to UpdateEffects(..).
   double GetPulseDensity() { return mPulseDensity; }
   //@}

protected:
   class WSF_MIL_EXPORT RPJ_Data : public WsfEW_PulseEffect::PulseData
   {
   public:
      RPJ_Data();
      RPJ_Data& operator=(const RPJ_Data&) = delete;
      ~RPJ_Data() override                 = default;

      bool ProcessInput(UtInput& aInput) override;
      bool Initialize(WsfSimulation& aSimulation) override;

      EW_EffectData* Clone() const override;

      void Reset() override;

      double GetRPJ_Gain(double aJammerToNoise, double aPulseDensity) const;

      int    mSamplesPerDetection; //!< Number of samples taken per detection (e.g. FFT sample size).
      double mMinJNR_Threshold;    //!< Minimum JNR that is required to activate this effect.

      //!< JNR  to pulses density to RPJ gain factor table.
      JNR_PD_GainMapMap mJNR_PD_GainMapMap;

   protected:
      RPJ_Data(const RPJ_Data& aSrc) = default;
   };

   WsfEW_RPJ_Effect(const WsfEW_RPJ_Effect& aSrc);

   //! @name Effect Data methods.
   //@{
   EffectData* NewEffectData() override { return new RPJ_Data(); }
   EffectData* PropagateEffectDataPtr(WsfStringId aSystemTypeId) override;

   double            GetMinJNR_Threshold(WsfStringId aSystemTypeId) const;
   int               GetSamplesPerDetection(WsfStringId aSystemTypeId) const;
   JNR_PD_GainMapMap GetJNR_PD_GainMapMap(WsfStringId aSystemTypeId) const;
   //@}

private:
   using RPJ_DataMap =
      std::map<WsfStringId, RPJ_Data*>; //!< Local copy of the mEffectDataMap to prevent casting within this class.

   double GetRPJ_Gain(JNR_PD_GainMapMap& aJNR_PD_GainMapMap, double aJammerToNoise, double aPulseDensity);

   double      mCoherentRatio;   //!< Percent of jammer power that is coherent, remainder is non-coherent.
   RPJ_DataMap mRPJ_DataMap;     //!< Map of the System Type Id to the data required for this technique.
   double      mRPJ_Gain;        //!< The RPJ gain for the last calculated interaction.
   int         mPulsesInSamples; //!< Total number of pulses in the range gates for the last calculated interaction.
   double      mPulseDensity; //!< Inherent pulse-density on the receiver-processor for the last calculated interaction.
   double      mJammerToNoise;    //!< JNR for the last calculated jammer interaction.
   PulseRangeVec mPulseRangesVec; //!< Storage of one beam position's pulse ranges for display purposes.
};

#endif
