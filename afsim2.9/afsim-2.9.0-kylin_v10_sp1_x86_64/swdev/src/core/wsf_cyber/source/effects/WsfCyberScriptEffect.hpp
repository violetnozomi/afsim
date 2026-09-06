// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSCRIPTCYBEREFFECT_HPP
#define WSFSCRIPTCYBEREFFECT_HPP

#include <unordered_map>

class UtScript;
#include "WsfCyberEffect.hpp"
#include "WsfNamed.hpp"
class WsfScenario;
#include "script/WsfScriptContext.hpp"

namespace wsf
{
namespace cyber
{

//! NOTE:
//! A new effect, ScriptEffectEnhanced has been provided that extends the capabilities
//! of this script class to allow user provided input during an attack call to
//! be utilized with the script effect. Input is modified from the standard script effect
//! to support that additional variables/parameters within the script context. As such,
//! this script effect type will be maintained to support previous versions of AFSIM
//! until the 3.0 release. At that time, this effect will be deprecated and removed, with
//! the new script effect type assuming the name of this effect.
class ScriptEffect : public Effect
{
public:
   ScriptEffect(WsfScenario& aScenario);
   ScriptEffect& operator=(const ScriptEffect&) = delete;
   ~ScriptEffect() override                     = default;

   //! @name Common infrastructure methods.
   //@{
   Effect* Clone() const override;
   bool    Initialize(Engagement& aEngagement) override;
   bool    ProcessInput(UtInput& aInput) override;
   //@}

   //! @name Engagement methods.
   //! These methods are called during the course of the engagement.
   //@{
   void Attack(double aSimTime, Engagement& aEngagement) override;

   void Restore(double aSimTime, Engagement& aEngagement) override;
   //@}

protected:
   ScriptEffect(const ScriptEffect& aSrc) = default;

private:
   class EffectInstance : public WsfNamed
   {
   public:
      EffectInstance() = default;

      void ProcessInput(UtInput& aInput, WsfScriptContext& aContext, bool aDefaultUsage = false);

      void Attack(Engagement* aEngagement, double aSimTime, WsfScriptContext& aContext);
      void Restore(Engagement* aEngagement, double aSimTime, WsfScriptContext& aContext);

   private:
      UtScript* mAttack{nullptr};
      UtScript* mRestore{nullptr};

      static void AttackRestore(Engagement* aEngagement, double aSimTime, WsfScriptContext& aContext, UtScript* aScriptPtr);
   };

   WsfScriptContext                                mContext;
   std::unordered_map<std::string, EffectInstance> mPlatformTypeEffect{};
   std::unordered_map<std::string, EffectInstance> mPlatformEffect{};
   EffectInstance                                  mDefaultEffect{};

   EffectInstance FindMatch(const Engagement& aEngagement) const;
};

} // namespace cyber
} // namespace wsf

#endif
