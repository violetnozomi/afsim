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

#ifndef WSFEW_POLMODEFFECT_HPP
#define WSFEW_POLMODEFFECT_HPP

#include "wsf_mil_export.h"

#include <iosfwd>
#include <map>

class UtInput;

#include "WsfEM_Interaction.hpp"
class WsfEM_Rcvr;
class WsfEM_Xmtr;
#include "WsfEW_SLC_DegradeEffect.hpp"
#include "WsfEW_Types.hpp"

class WSF_MIL_EXPORT WsfEW_PolModEffect : public WsfEW_SLC_DegradeEffect
{
public:
   // TODO - not too sure I need this enumeration as I can apply all the factors with
   //   default values without any issues or modification to the data if they do not apply
   enum PolModEffect
   {
      cPME_DEGRADE_SLC  = 1,
      cPME_SATURATE_SLC = cPME_DEGRADE_SLC << 1
   };

   WsfEW_PolModEffect();
   WsfEW_PolModEffect& operator=(const WsfEW_PolModEffect&) = delete;
   ~WsfEW_PolModEffect() override                           = default;
   WsfEW_Effect* Clone() const override;
   bool          Initialize(WsfSimulation& aSimulation) override;
   bool          ProcessInput(UtInput& aInput) override;


   void ApplyUnmitigatedEffect(WsfEW_Effects& aEW_Effects) const override;

   void ApplyMitigatedEffect(WsfEW_Effect* aEW_EffectPtr, WsfEW_Effects& aEW_Effects) const override;

   //! @name Polarization-Modulation Accessor/Mutator methods.
   //@{

   double GetPolarizationSwitchingRate(WsfStringId aSystemTypeId);
   //@}

protected:
   class WSF_MIL_EXPORT PolModData : public WsfEW_SLC_DegradeEffect::SLC_DegradeData
   {
   public:
      PolModData();
      PolModData& operator=(const PolModData&) = delete;
      ~PolModData() override                   = default;

      bool ProcessInput(UtInput& aInput) override;
      bool Initialize(WsfSimulation& aSimulation) override;

      EW_EffectData* Clone() const override;

      void Reset() override;

      //! Switching rate of the polarization modulation (Hz)
      //! default 1/radar_pulsewidth
      double mPolSwitchRate;

   protected:
      PolModData(const PolModData& aSrc) = default;
   };

   WsfEW_PolModEffect(const WsfEW_PolModEffect& aSrc);

   //! @name Effect Data methods.
   //@{
   EffectData* NewEffectData() override;
   EffectData* PropagateEffectDataPtr(WsfStringId aSystemTypeId) override;
   //@}

private:
   using PolModDataMap =
      std::map<WsfStringId, PolModData*>; //!< Local copy of the mEffectDataMap to prevent casting within this class.

   // Input variables
   PolModDataMap mPolModDataMap; //!< Map of the System Type Id to the data required for this technique.
};

#endif
