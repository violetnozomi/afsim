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

#include "WsfEW_CommEffect.hpp"

#include <algorithm>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfEW_Effects.hpp"

// virtual
WsfEW_Effect* WsfEW_CommEffect::Clone() const
{
   return new WsfEW_CommEffect(*this);
}

// virtual
bool WsfEW_CommEffect::Initialize(WsfSimulation& aSimulation)
{
   bool ok = WsfEW_Effect::Initialize(aSimulation);

   if (GetEA_Ptr() != nullptr)
   {
      mSystemFunctionMask = cES_COMM;
   }

   return ok;
}

// virtual
bool WsfEW_CommEffect::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "bit_error_rate")
   {
      double bitErrorRate;
      aInput.ReadValueOfType(bitErrorRate, UtInput::cDATA_RATE);
      mBitErrorRate = bitErrorRate;
      mBehavior     = cEB_MESSAGE_ERROR;
   }
   else if (command == "message_behavior")
   {
      std::string messageEffect;
      aInput.ReadValue(messageEffect);
      if (messageEffect == "maintain")
      {
         mBehavior = cEB_MAINTAIN_MESSAGE;
      }
      else if (messageEffect == "drop")
      {
         mBehavior = cEB_DROP_MESSAGE;
      }
      else if (messageEffect == "distort")
      {
         mBehavior = cEB_MESSAGE_ERROR;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Specified message_behavior type unknown: " + messageEffect);
      }
   }
   else
   {
      myCommand = WsfEW_Effect::ProcessInput(aInput);
   }

   return myCommand;
}

// virtual
void WsfEW_CommEffect::ApplyUnmitigatedEffect(WsfEW_Effects& aEW_Effects) const
{
   // Indicate the behavior to induce.
   aEW_Effects.mMask |= GetBehavior();

   if (GetBehavior() == cEB_MESSAGE_ERROR)
   {
      aEW_Effects.mEW_Error.mBitErrorRate = std::max(aEW_Effects.mEW_Error.mBitErrorRate, mBitErrorRate);
   }
}

// virtual
void WsfEW_CommEffect::ApplyMitigatedEffect(WsfEW_Effect* aEW_EffectPtr, WsfEW_Effects& aEW_Effects) const

{
   // See what the Mitigation Behavior is and set the Behavior
   if (GetBehavior() == cEB_MAINTAIN_MESSAGE)
   {
      aEW_Effects.mMask |= cEB_MAINTAIN_MESSAGE;
   }
   else if (GetBehavior() == cEB_MESSAGE_ERROR)
   {
      aEW_Effects.mMask |= cEB_MESSAGE_ERROR;
      aEW_Effects.mEW_Error.mBitErrorRate = std::min(aEW_Effects.mEW_Error.mBitErrorRate, mBitErrorRate);
   }
   else if (mDebug)
   {
      auto ou =
         ut::log::debug() << "Undefined or improper Comm Effect behavior in WsfEW_CommEffect::ApplyMitigatedEffect.";
      ou.AddNote() << "Behavior: " << GetBehavior();
   }
}
