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

#ifndef WSFEW_SLC_DEGRADEEFFECT_HPP
#define WSFEW_SLC_DEGRADEEFFECT_HPP

#include "wsf_mil_export.h"

#include <iosfwd>
#include <map>

class UtInput;

#include "WsfEM_Interaction.hpp"
#include "WsfEW_Effect.hpp"
#include "WsfEW_PowerEffect.hpp"
#include "WsfEW_Types.hpp"

class WSF_MIL_EXPORT WsfEW_SLC_DegradeEffect : public WsfEW_PowerEffect
{
public:
   // TODO - not too sure I need this enumeration as I can apply all the factors with
   //   default values without any issues or modification to the data if they do not apply
   enum SLC_DegradeEffect
   {
      cPME_DEGRADE_SLC  = 1,
      cPME_SATURATE_SLC = cPME_DEGRADE_SLC << 1
   };

   WsfEW_SLC_DegradeEffect();
   WsfEW_SLC_DegradeEffect& operator=(const WsfEW_SLC_DegradeEffect&) = delete;
   ~WsfEW_SLC_DegradeEffect() override                                = default;
   WsfEW_Effect* Clone() const override;
   bool          Initialize(WsfSimulation& aSimulation) override;
   bool          ProcessInput(UtInput& aInput) override;

   void ApplyUnmitigatedEffect(WsfEW_Effects& aEW_Effects) const override;

   void ApplyMitigatedEffect(WsfEW_Effect* aEW_EffectPtr, WsfEW_Effects& aEW_Effects) const override;

   //! @name SLC Degradation Accessor/Mutator methods.
   //@{

   double GetSLC_DegradationValue(WsfStringId aSystemTypeId) const;

   int GetSLC_ChannelsSaturated(WsfStringId aSystemTypeId) const;
   //@}

   //! @name Effect Data methods.
   //@{

   //@}


protected:
   class WSF_MIL_EXPORT SLC_DegradeData : public WsfEW_PowerEffect::PowerData
   {
   public:
      SLC_DegradeData();
      SLC_DegradeData& operator=(const SLC_DegradeData&) = delete;
      ~SLC_DegradeData() override                        = default;

      bool ProcessInput(UtInput& aInput) override;
      bool Initialize(WsfSimulation& aSimulation) override;

      EW_EffectData* Clone() const override;

      void Reset() override;

      WsfEW_Effect::ModulationType mModulation;            //!< Signal modulation output type
      SLC_DegradeEffect            mPolModEffect;          //!< Effect that the SLC degrade technique exhibits
      double                       mSLC_DegradationValue;  //!< Degradation value of this effect
      int                          mSLC_ChannelsSaturated; //!< Number of SLC channels occupied by this effect

   protected:
      SLC_DegradeData(const SLC_DegradeData& aSrc) = default;
   };

   WsfEW_SLC_DegradeEffect(const WsfEW_SLC_DegradeEffect& aSrc);

   //! @name Effect Data methods.
   //@{
   EffectData* NewEffectData() override;
   EffectData* PropagateEffectDataPtr(WsfStringId aSystemTypeId) override;
   //@}

private:
   using SLC_DegradeDataMap =
      std::map<WsfStringId, SLC_DegradeData*>; //!< Local copy of the mEffectDataMap to prevent casting within this class.

   // Input variables
   SLC_DegradeDataMap mSLC_DegradeDataMap; //!< Map of the System Type Id to the data required for this technique.
};

#endif
