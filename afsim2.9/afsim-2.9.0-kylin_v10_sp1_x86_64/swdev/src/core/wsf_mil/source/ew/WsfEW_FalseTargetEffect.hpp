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

#ifndef WSFEW_FALSETARGETEFFECT_HPP
#define WSFEW_FALSETARGETEFFECT_HPP

#include "wsf_mil_export.h"

#include <iosfwd>

class UtInput;

#include "WsfEM_Interaction.hpp"
class WsfEM_Xmtr;
#include "WsfEW_Effect.hpp"
#include "WsfEW_PulseEffect.hpp"
class WsfFalseTarget;

//! WsfEW_FalseTargeEffect models and applies false-target effects.

class WSF_MIL_EXPORT WsfEW_FalseTargetEffect : public WsfEW_PulseEffect
{
public:
   WsfEW_FalseTargetEffect();
   WsfEW_FalseTargetEffect& operator=(const WsfEW_FalseTargetEffect&) = delete;
   ~WsfEW_FalseTargetEffect() override                                = default;

   WsfEW_Effect* Clone() const override;
   bool          Initialize(WsfSimulation& aSimulation) override;
   bool          ProcessInput(UtInput& aInput) override;

   void ApplyUnmitigatedEffect(WsfEW_Effects& aEW_Effects) const override;

   void ApplyMitigatedEffect(WsfEW_Effect* aEW_EffectPtr, WsfEW_Effects& aEW_Effects) const override;

   void UpdateJammerPulseDensity(WsfEM_Xmtr* aJammerXmtrPtr, WsfEM_Xmtr* aRadarXmtrPtr) override;

   virtual bool InitializeFT(const WsfEM_Rcvr& aRadarRcvr, const WsfEM_Xmtr& aRadarXmtr);
   virtual bool UpdateFT(const WsfEM_Rcvr& aRadarRcvr, const WsfEM_Xmtr& aRadarXmtr);

   //! @name Effect Data methods.
   //@{

   WsfFalseTarget* GetFT_Ptr(WsfStringId aSystemTypeId);

   // Override to reset the mExplicitnumberOfFTS flag.
   void SetJammerPulseDensity(WsfStringId aSystemTypeId, double aJammerPulseDensity, bool aIsExplicit = false) override;
   //@}

protected:
   class WSF_MIL_EXPORT FT_Data : public WsfEW_PulseEffect::PulseData
   {
   public:
      FT_Data();
      FT_Data& operator=(const FT_Data&) = delete;
      ~FT_Data() override;

      bool ProcessInput(UtInput& aInput) override;
      bool Initialize(WsfSimulation& aSimulation) override;

      EW_EffectData* Clone() const override;

      void Reset() override;

      WsfFalseTarget* GetFT_Ptr() const { return mFT_Ptr; }
      void            SetFT_Ptr(WsfFalseTarget* aFT_Ptr, bool aDT_IsCopy);

      bool         mAutoGenFTs;
      WsfStringId  mFT_NameId;   //!< Name of the false target object.
      unsigned int mNumberOfFTs; //!< Maximum number of false targets to be injected.
      bool         mExplicitNumberOfFTs;
      bool         mRangeConstrained;
      bool         mExplicitRangeConstraint;
      double       mScanRateMultiplier;
      double       mBlipSpeeds[2];

   protected:
      FT_Data(const FT_Data& aSrc);

   private:
      bool            mFT_IsCopy; //!< Defines whether this false-target effect is a copy or not.
      WsfFalseTarget* mFT_Ptr;    //!< An implementation-defined index to where to find the false target instance.
   };

   WsfEW_FalseTargetEffect(const WsfEW_FalseTargetEffect& aSrc);

   //! @name Effect Data methods.
   //@{
   double       GetScanRateMultiplier(WsfStringId aSystemTypeId) const;
   bool         GetAutoGenFTs(WsfStringId aSystemTypeId) const;
   bool         ExplicitRangeConstraint(WsfStringId aSystemTypeId) const;
   bool         GetRangeConstrained(WsfStringId aSystemTypeId) const;
   double       GetMinBlipSpeed(WsfStringId aSystemTypeId) const;
   double       GetMaxBlipSpeed(WsfStringId aSystemTypeId) const;
   unsigned int GetNumberOfFTs(WsfStringId aSystemTypeId) const;
   bool         ExplicitNumberOfFTs(WsfStringId aSystemTypeId) const;

   EffectData* NewEffectData() override { return new FT_Data(); }
   EffectData* PropagateEffectDataPtr(WsfStringId aSystemTypeId) override;
   //@}

private:
   using FT_DataMap =
      std::map<WsfStringId, FT_Data*>; //!< Local copy of the mEffectDataMap to prevent casting within this class.

   FT_DataMap mFT_DataMap; //!< Map of the System Type Id to the data required for this technique.
};

#endif
