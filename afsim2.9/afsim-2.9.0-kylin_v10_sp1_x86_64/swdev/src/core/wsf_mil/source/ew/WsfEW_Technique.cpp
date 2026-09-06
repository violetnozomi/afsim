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

#include "WsfEW_Technique.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "WsfDeferredInput.hpp"
#include "WsfEW_AgilityEffect.hpp"
#include "WsfEW_EA.hpp"
#include "WsfEW_EA_EP.hpp"
#include "WsfEW_EP.hpp"
#include "WsfEW_Effect.hpp"
#include "WsfEW_EffectTypes.hpp"
#include "WsfEW_PowerEffect.hpp"
#include "WsfFalseTarget.hpp"
#include "WsfScenario.hpp"

// =================================================================================================
WsfEW_Technique::WsfEW_Technique()
   : WsfObject()
   , mDebug(false)
   , mEffects()
   , mDefaultOn(false)
   , mEA_EP_Ptr(nullptr)
{
}

WsfEW_Technique::WsfEW_Technique(const WsfEW_Technique& aSrc)
   : WsfObject(aSrc)
   , mDebug(aSrc.mDebug)
   , mEffects()
   , mDefaultOn(aSrc.mDefaultOn)
   , mEA_EP_Ptr(nullptr)
{
   for (auto effectPtr : aSrc.mEffects)
   {
      mEffects.push_back(effectPtr->Clone());
   }
}

// virtual
WsfEW_Technique::~WsfEW_Technique()
{
   for (auto& effectPtr : mEffects)
   {
      delete effectPtr;
   }
}

// virtual
WsfEW_Technique* WsfEW_Technique::Clone() const
{
   return new WsfEW_Technique(*this);
}

// NOTE: Must have EA/EP pointer variable already set in the technique before calling
bool WsfEW_Technique::Initialize(WsfSimulation& aSimulation)
{
   // define and initialize return variable
   bool success = true;

   if (mEA_EP_Ptr == nullptr)
   {
      ut::log::error() << "Coding Violation: WsfEW_Technique::Initialize: Must set the EA/EP Pointer variable.";
      return false;
   }

   // Setup and initialize all of the effects associated with this technique.
   for (unsigned int i = 0; i != mEffects.size(); ++i)
   {
      mEffects[i]->SetTechniquePtr(this);
      mEffects[i]->SetEA_EP_Ptr(mEA_EP_Ptr);

      success &= mEffects[i]->Initialize(aSimulation);
   }

   return success;
}

// virtual
bool WsfEW_Technique::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (WsfEW_EffectTypes::Get(WsfScenario::FromInput(aInput)).LoadInstance(aInput, this))
   {
   }
   else if (command == "default_on")
   {
      mDefaultOn = true;
   }
   else if (command == "debug")
   {
      mDebug = true;
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

//! Remove the specified jammer from the associate effects.
//!
//! @param aXmtrId The unique Id of the transmitter.
// virtual
void WsfEW_Technique::RemoveJammerXmtr(unsigned int aXmtrId)
{
   for (unsigned int i = 0; i != mEffects.size(); ++i)
   {
      mEffects[i]->RemoveJammerXmtr(aXmtrId);
   }
}

//! Remove the specified jammer transmitter from the associate effects.
//!
//! @param aXmtrPtr The pointer to the transmitter.
// virtual
void WsfEW_Technique::RemoveJammerXmtr(WsfEM_Xmtr* aXmtrPtr)
{
   for (unsigned int i = 0; i != mEffects.size(); ++i)
   {
      mEffects[i]->RemoveJammerXmtr(aXmtrPtr);
   }
}

WsfEW_EA* WsfEW_Technique::GetEA_Ptr() const
{
   return dynamic_cast<WsfEW_EA*>(mEA_EP_Ptr);
}

WsfEW_EP* WsfEW_Technique::GetEP_Ptr() const
{
   return dynamic_cast<WsfEW_EP*>(mEA_EP_Ptr);
}

// protected
bool WsfEW_Technique::FindEffectIndex(WsfStringId aEffectNameId, unsigned int& aIndex) const
{
   bool found = false;
   for (unsigned int i = 0; i != mEffects.size(); ++i)
   {
      if (mEffects[i]->GetNameId() == aEffectNameId)
      {
         aIndex = i;
         found  = true;
         break;
      }
   }
   return found;
}

//! Indicates if the specified technique can be applied to the specified system type.
//!
//! @param aSystemFunctionMask The system function mask.
//! @return Returns true if the technique can be applied to the system function.
bool WsfEW_Technique::CanApplyTechnique(unsigned int aSystemFunctionMask) const
{
   bool canApply = false;

   for (const WsfEW_Effect* effectPtr : mEffects)
   {
      if (effectPtr != nullptr)
      {
         // Make sure the technique's effect can be applied to
         // specified system.
         if ((effectPtr->GetSystemFunctionMask() & aSystemFunctionMask) != 0u)
         {
            canApply = true;
         }
      }
   }
   return canApply;
}

// virtual
bool WsfEW_Technique::HasFrequencyAgilityEffect() const
{
   for (unsigned int index = 0; index != mEffects.size(); ++index)
   {
      WsfEW_AgilityEffect* agilityEffectPtr = static_cast<WsfEW_AgilityEffect*>(mEffects[index]);
      if (agilityEffectPtr != nullptr)
      {
         if ((agilityEffectPtr->GetAgilityTypeMask() & WsfEW_AgilityEffect::cAT_FREQUENCY) != 0)
         {
            return true;
         }
      }
   }
   return false;
}

bool WsfEW_Technique::AddEffect(WsfEW_Effect* aEffectPtr)
{
   if (!EffectIsAvailable(aEffectPtr->GetNameId()))
   {
      mEffects.push_back(aEffectPtr);
      return true;
   }
   return false;
}

bool WsfEW_Technique::EffectIsAvailable(WsfStringId aEffectNameId) const
{
   bool found = false;
   for (unsigned int i = 0; i != mEffects.size(); ++i)
   {
      if (mEffects[i]->GetNameId() == aEffectNameId)
      {
         found = true;
         break;
      }
   }
   return found;
}

WsfEW_Effect* WsfEW_Technique::GetEffect(unsigned int aIndex) const
{
   WsfEW_Effect* effectPtr = nullptr;
   if (aIndex < mEffects.size())
   {
      effectPtr = mEffects[aIndex];
   }
   return effectPtr;
}

WsfEW_Effect* WsfEW_Technique::GetEffect(WsfStringId aEffectNameId) const
{
   WsfEW_Effect* effectPtr = nullptr;
   for (unsigned int index = 0; index != mEffects.size(); ++index)
   {
      if (mEffects[index]->GetNameId() == aEffectNameId)
      {
         effectPtr = mEffects[index];
         break;
      }
   }
   return effectPtr;
}

// private
void WsfEW_Technique::SelectDefaultUnmitigatedEffects(EffectPtrVec aEffectList, unsigned int aSystemFunctionMask)
{
   aEffectList.clear();
   for (auto& effectPtr : mEffects)
   {
      // Simply use the first effect that supports the provided function.
      if ((effectPtr->GetSystemFunctionMask() & aSystemFunctionMask) != 0u)
      {
         aEffectList.push_back(effectPtr);
         break;
      }
   }
}

//! Gets an effect based on the provided EW function.
//!
//! @param aSystemFunctionMask Indicates what system function(s) that the effect will be applied to (i.e. Comms,
//! Sensors, etc.).
//! @return Returns the appropriate EW effect (0 if none is found).
WsfEW_Effect* WsfEW_Technique::SelectUnmitigatedEffect(unsigned int aSystemFunctionMask)
{
   // It will be difficult to select multiple effects.  The initial implementation
   // is to select the first SINGLE result available, and not try to aggregate
   // multiple effects at all...
   WsfEW_Effect* mUnmitigatedEffectPtr(nullptr);
   for (auto& effectPtr : mEffects)
   {
      // Simply use the first effect that supports the provided function.
      if ((effectPtr->GetSystemFunctionMask() & aSystemFunctionMask) != 0u)
      {
         mUnmitigatedEffectPtr = effectPtr;
         break;
      }
   }
   return mUnmitigatedEffectPtr;
}

//! Gets an effect based on the provided EW function.
//!
//! @param aIndex              The index of the effect of interest.
//! @param aSystemFunctionMask Indicates what system function(s) that the effect will be applied to (i.e. Comms,
//! Sensors, etc.).
//! @return Returns the appropriate EW effect (0 if none is found).
WsfEW_Effect* WsfEW_Technique::SelectUnmitigatedEffect(unsigned int aIndex, unsigned int aSystemFunctionMask)
{
   WsfEW_Effect* mUnmitigatedEffectPtr(nullptr);
   if ((mEffects[aIndex]->GetSystemFunctionMask() & aSystemFunctionMask) != 0u)
   {
      mUnmitigatedEffectPtr = mEffects[aIndex];
   }
   return mUnmitigatedEffectPtr;
}

//! Set a jamming delta gain technique on an active spot using the supplied criteria.
//! @param aSimTime            The current simulation time.
//! @param aEffectId           The assigned effect Id to set.
//! @param aDebug              The debug state  to set for the technique.
//! @param aJammingPowerGain   The assigned jamming gain value.
//! @param aSystemTypeId       The assigned System Type Id of the target types this technique applies to.
//! @param aSystemFunctionId   The assigned System Function Id of the target types this technique applies to.
//! @return The request was successful.
// public
bool WsfEW_Technique::SetDeltaGainEffect(double      aSimTime,
                                         WsfStringId aEffectId,
                                         bool        aDebug,
                                         double      aJammingPowerGain,
                                         WsfStringId aSystemTypeId,
                                         WsfStringId aSystemFunctionId)
{
   // set default return value
   bool setEffect(false);

   WsfEW_PowerEffect* effectPtr(nullptr);
   WsfStringId        id         = GetName() + ":" + aEffectId.GetString();
   auto               effectIter = find(mEffects.begin(), mEffects.end(), GetEffect(id));
   if (effectIter != mEffects.end())
   {
      effectPtr = dynamic_cast<WsfEW_PowerEffect*>(*effectIter);
   }
   else
   {
      effectPtr = new WsfEW_PowerEffect();
      if (effectPtr != nullptr)
      {
         effectPtr->SetName(GetName() + ":" + aEffectId.GetString());
         effectPtr->SetTechniquePtr(this);
         effectPtr->SetEA_EP_Ptr(GetEA_EP_Ptr());

         mEffects.push_back(effectPtr);
      }
   }

   if (effectPtr != nullptr)
   {
      effectPtr->SetDebug(aDebug);
      effectPtr->SetSystemFunctionMask(aSystemFunctionId.GetString());
      effectPtr->SetBehavior(WsfEW_Effect::cEB_JAMMER_POWER_EFFECT);

      setEffect =
         effectPtr->Initialize(*GetEA_EP_Ptr()->GetSimulation()); // have to initialize before setting any of the data

      effectPtr->SetJammingPowerGain(aSystemTypeId, aJammingPowerGain); // set the power gain data field
   }
   else
   {
      if (mDebug)
      {
         auto out = ut::log::warning() << "WsfEW_Technique::SetDeltaGainEffect: Unable to set delta gain technique.";
         out.AddNote() << "Effect: " << aEffectId;
      }
      return false;
   }

   return setEffect;
}
