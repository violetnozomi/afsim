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

#ifndef WSFEW_COMMEFFECT_HPP
#define WSFEW_COMMEFFECT_HPP

#include "wsf_mil_export.h"

#include "WsfEW_Effect.hpp"

class UtInput;

//! WsfEW_CommEffect models and applies comm effects (message bit error rate, message drops, etc.).

class WSF_MIL_EXPORT WsfEW_CommEffect : public WsfEW_Effect
{
public:
   WsfEW_CommEffect()                   = default;
   WsfEW_CommEffect(WsfEW_CommEffect&&) = default;
   WsfEW_CommEffect& operator=(const WsfEW_CommEffect&) = delete;
   WsfEW_CommEffect& operator=(WsfEW_CommEffect&&) = default;
   ~WsfEW_CommEffect() override                    = default;

   WsfEW_Effect* Clone() const override;
   bool          Initialize(WsfSimulation& aSimulation) override;
   bool          ProcessInput(UtInput& aInput) override;

   void ApplyUnmitigatedEffect(WsfEW_Effects& aEW_Effects) const override;

   void ApplyMitigatedEffect(WsfEW_Effect* aEW_EffectPtr, WsfEW_Effects& aEW_Effects) const override;

protected:
   WsfEW_CommEffect(const WsfEW_CommEffect& aSrc) = default;

private:
   double mBitErrorRate{0.0};
};
#endif
