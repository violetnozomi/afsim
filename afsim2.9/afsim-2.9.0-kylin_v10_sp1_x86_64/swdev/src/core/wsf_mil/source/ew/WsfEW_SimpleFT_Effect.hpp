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

#ifndef WSFEW_SIMPLEFT_EFFECT_HPP
#define WSFEW_SIMPLEFT_EFFECT_HPP

#include "wsf_mil_export.h"

#include <iosfwd>
#include <map>

class UtInput;

#include "WsfEM_Interaction.hpp"
#include "WsfEW_PulseEffect.hpp"

//! WsfEW_SimpleFT_Effect that models and applies Random Pulse Jamming (RPJ) effects.

class WSF_MIL_EXPORT WsfEW_SimpleFT_Effect : public WsfEW_PulseEffect
{
public:
   WsfEW_SimpleFT_Effect();
   WsfEW_SimpleFT_Effect& operator=(const WsfEW_SimpleFT_Effect&) = delete;
   ~WsfEW_SimpleFT_Effect() override                              = default;
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

   //! @name Simple FT Accessor/Mutator methods.
   //@{

   //! Get the calculated number of false targets for the last call to UpdateEffects(..).
   double GetNumberFalseTargets() { return mNumberFalseTargets; }
   //@}

protected:
   class WSF_MIL_EXPORT SimpleFT_Data : public WsfEW_PulseEffect::PulseData
   {
   public:
      SimpleFT_Data();
      SimpleFT_Data& operator=(const SimpleFT_Data&) = delete;
      ~SimpleFT_Data() override                      = default;

      bool ProcessInput(UtInput& aInput) override;
      bool Initialize(WsfSimulation& aSimulation) override;

      EW_EffectData* Clone() const override;

      void Reset() override;

      unsigned int mMaximumNumberFTs;    //!< Maximum number of false targets to be injected.
      unsigned int mMaximumFT_Capacity;  //!< The maximum number of false targets per detection attempt.
      bool         mExplicitFT_Capacity; //!< Maximum false target capacity was explicitly set.
      double       mDetectBW_Ratio;      //!< The ratio of the 3dB beamwidth required for detection.
      bool         mApplyEP_Effects;     //!< Flag to apply EP techniques to number of false targets calculated.
      bool         mUpdateOncePerFrame; //!< Update the false target count only once per frame of a sensor per receiver.
      bool         mCombineBeamCounts;  //!< For multi-beam radars, combine the beam counts as they are processed.
      bool         mUseRandomDraw;      //!< Flag for random draws based on ratios of targets and false targets.

   protected:
      SimpleFT_Data(const SimpleFT_Data& aSrc) = default;
   };

   WsfEW_SimpleFT_Effect(const WsfEW_SimpleFT_Effect& aSrc);

   unsigned int GetMaximumNumberFalseTargets(WsfStringId aSystemTypeId) const;

   //! @name Effect Data methods.
   //@{
   EffectData* NewEffectData() override;
   EffectData* PropagateEffectDataPtr(WsfStringId aSystemTypeId) override;
   //@}

private:
   struct SensorData
   {
      SensorData()
         : mLastUpdateTime(-DBL_MAX)
         , mNumberFalseTargets(0)
         , mFloodedBlocked(false)
      {
      }

      double       mLastUpdateTime;
      unsigned int mNumberFalseTargets;
      bool         mFloodedBlocked;
   };

   bool FloodedBlocked(unsigned int aNumberFalseTargets, unsigned int aMaximumCapacity, bool aRandomDraw);

   using SimpleFT_DataMap =
      std::map<WsfStringId, SimpleFT_Data*>; //!< Local copy of the mEffectDataMap to prevent casting within this class.
   using RcvrPtrDeltaGainVec = std::vector<std::pair<WsfEM_Rcvr*, double>>;
   using SensorDataMap       = std::map<WsfStringId, SensorData>;

   SimpleFT_DataMap mSimpleFT_DataMap; //!< Map of the System Type Id to the data required for this technique.

   // Current data
   SensorDataMap mSensorDataMap;      //!< Stored sensor data for aid in skipping out of calculations once flooded.
   unsigned int  mNumberFalseTargets; //!< Total number of false targets for the last calculated interaction.
   unsigned int  mMaximumFT_Capacity; //!< Maximum false target capacity for the last calculated interaction.
   bool          mFloodedBlocked;     //!< Flooded blocked flag for the last calculated interaction.
   bool          mForceAllow; //!< Forcing the detection (i.e. not blocking) for the last calculated interaction.
};

#endif
