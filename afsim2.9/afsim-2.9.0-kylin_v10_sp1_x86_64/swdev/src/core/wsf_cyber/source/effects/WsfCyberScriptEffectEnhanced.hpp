// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSCRIPTCYBEREFFECTENHANCED_HPP
#define WSFSCRIPTCYBEREFFECTENHANCED_HPP

#include <unordered_map>

class UtScript;
#include "WsfCyberAttackParameters.hpp"
#include "WsfCyberEffect.hpp"
#include "WsfNamed.hpp"
class WsfScenario;
#include "script/WsfScriptContext.hpp"

namespace wsf
{
namespace cyber
{
class ScriptEffectEnhanced : public Effect
{
public:
   ScriptEffectEnhanced(WsfScenario& aScenario);
   ScriptEffectEnhanced& operator=(const ScriptEffectEnhanced&) = delete;
   ~ScriptEffectEnhanced() override                             = default;

   //! @name Common infrastructure methods.
   //@{
   Effect* Clone() const override;
   Effect* Clone(const AttackParameters& aParameters) const override;
   bool    Initialize(Engagement& aEngagement) override;
   bool    ProcessInput(UtInput& aInput) override;
   //@}

   //! @name Engagement methods.
   //! These methods are called during the course of the engagement.
   //@{
   void Attack(double aSimTime, Engagement& aEngagement) override;

   void Restore(double aSimTime, Engagement& aEngagement) override;
   //@}

   InputRequirement RequiredInput() const override { return InputRequirement::cOPTIONAL; }

protected:
   ScriptEffectEnhanced(const ScriptEffectEnhanced& aSrc) = default;
   ScriptEffectEnhanced(const ScriptEffectEnhanced& aSrc, const AttackParameters& aParameters);

private:
   class EffectInstance : public WsfNamed
   {
   public:
      EffectInstance() = default;

      void ProcessInput(UtInput& aInput, WsfScriptContext& aContext, bool aDefaultUsage = false);

      void Attack(Engagement* aEngagement, double aSimTime, WsfScriptContext& aContext, AttackParameters& aParameters);

      void Restore(Engagement* aEngagement, double aSimTime, WsfScriptContext& aContext, AttackParameters& aParameters);

   private:
      UtScript* mAttack{nullptr};
      UtScript* mRestore{nullptr};

      static void AttackRestore(Engagement*       aEngagement,
                                double            aSimTime,
                                WsfScriptContext& aContext,
                                AttackParameters& aParameters,
                                UtScript*         aScriptPtr);
   };

   WsfScriptContext                                mContext;
   std::unordered_map<std::string, EffectInstance> mPlatformTypeEffect{};
   std::unordered_map<std::string, EffectInstance> mPlatformEffect{};
   EffectInstance                                  mDefaultEffect{};
   AttackParameters                                mParameters{};

   EffectInstance FindMatch(const Engagement& aEngagement) const;
};

} // namespace cyber
} // namespace wsf

#endif
