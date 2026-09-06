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

#include "WsfEW_EA_EP.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>

#include "UtInput.hpp"
#include "WsfEM_XmtrRcvr.hpp"
#include "WsfEW_Effect.hpp"
#include "WsfEW_Technique.hpp"
#include "WsfEW_TechniqueTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfStringId.hpp"

// Pre-defined types
#include "WsfEW_EA.hpp"
#include "WsfEW_EP.hpp"

// ================================================================================================
WsfEW_EA_EP::WsfEW_EA_EP()
   : WsfObject()
   , mSimulationPtr(nullptr)
   , mTechniques()
   , mTechniquesInUse()
   , mDebug(false)
   , mLinkedXmtrRcvrPtr(nullptr)
{
}

WsfEW_EA_EP::WsfEW_EA_EP(const WsfEW_EA_EP& aSrc)
   : WsfObject(aSrc)
   , mSimulationPtr(nullptr)
   , mTechniques(aSrc.mTechniques)
   , mTechniquesInUse(aSrc.mTechniquesInUse)
   , mDebug(aSrc.mDebug)
   , mLinkedXmtrRcvrPtr(nullptr)
{
}

// virtual
bool WsfEW_EA_EP::Initialize(WsfSimulation& aSimulation, WsfEM_XmtrRcvr* aXmtrRcvrPtr)
{
   bool success       = true;
   mSimulationPtr     = &aSimulation;
   mLinkedXmtrRcvrPtr = aXmtrRcvrPtr;

   // initialize all of the techniques and their associated effects
   for (unsigned int i = 0; i != mTechniques.size(); ++i)
   {
      WsfEW_Technique* techniquePtr = GetTechnique(mTechniques[i]);
      PrivateSetTechnique(mTechniques[i], techniquePtr->IsDefaultOn());

      success &= techniquePtr->Initialize(aSimulation);
   }

   return success;
}

// virtual
bool WsfEW_EA_EP::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (WsfEW_TechniqueTypes::Get(WsfScenario::FromInput(aInput)).LoadInstance(aInput, this))
   {
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

//! Remove the specified jammer transmitter from the associate techniques.
//! @param aXmtrId The unique Id of the transmitter.
// virtual
void WsfEW_EA_EP::RemoveJammerXmtr(unsigned int aXmtrId)
{
   for (unsigned int i = 0; i != mTechniques.size(); ++i)
   {
      WsfEW_Technique* techniquePtr = GetTechnique(mTechniques[i]);
      techniquePtr->RemoveJammerXmtr(aXmtrId);
   }
}

//! Remove the specified jammer transmitter from the associate techniques.
//! @param aXmtrPtr The pointer to the transmitter.
// virtual
void WsfEW_EA_EP::RemoveJammerXmtr(WsfEM_Xmtr* aXmtrPtr)
{
   for (unsigned int i = 0; i != mTechniques.size(); ++i)
   {
      WsfEW_Technique* techniquePtr = GetTechnique(mTechniques[i]);
      techniquePtr->RemoveJammerXmtr(aXmtrPtr);
   }
}

//! This non-const method selects appropriate EP Technique(s) (if available), that will
//! mitigate the applied Electronic Attack Techniques, and then returns the resulting
//! effect.
//!
//! @param aEA_EffectPtr            Pointer to the applied electronic attack effect to mitigate
//! @param aAffectingSystemFunction Indicates the system function the attack was applied to.
//! @param aAffectedSystemFunction  Indicates the system function the attack was applied to.
//! @returns                        The 'effect' resulting from the interchange (can be zero).
// virtual
WsfEW_Technique::EffectPtrVec WsfEW_EA_EP::GetMitigatingEffects(const WsfEW_Effect*          aEA_EffectPtr,
                                                                WsfEW_Effect::SystemFunction aAffectingSystemFunction,
                                                                WsfEW_Effect::SystemFunction aAffectedSystemFunction)
{
   WsfEW_Technique::EffectPtrVec resultingEffects;
   resultingEffects.clear();
   return resultingEffects;
}

//! Reset the techniques in use list to the techniques that are on by default or are externally controlled
//! and currently in use, any changes made to the techniques or associated effects will be kept.
void WsfEW_EA_EP::ResetTechniquesInUseList()
{
   for (unsigned int i = 0; i != mTechniques.size(); ++i)
   {
      WsfEW_Technique* techniquePtr = GetTechnique(mTechniques[i]);
      bool             onOff =
         techniquePtr->IsDefaultOn() || (techniquePtr->IsExternallyControlled() && TechniqueIsInUse(mTechniques[i]));
      PrivateSetTechnique(mTechniques[i], onOff);
   }
}

//! Set the technique state to 'on' or 'off', must be set to 'externally_controlled'.
//!
//! @param aTechniqueId The technique Id for which to change the state.
//! @param aOnOff       The state of the supplied technique to change to, 'true' is on and 'false' is off.
void WsfEW_EA_EP::SetTechnique(WsfEW_Types::TechniqueId aTechniqueId, bool aOnOff)
{
   WsfEW_Technique* techPtr = GetTechnique(aTechniqueId);
   if ((techPtr != nullptr) && techPtr->IsExternallyControlled())
   {
      PrivateSetTechnique(aTechniqueId, aOnOff);
   }
}

WsfEW_Technique* WsfEW_EA_EP::GetTechnique(WsfEW_Types::TechniqueId aTechniqueId) const
{
   ut::log::error() << "WsfEW_EA_EP::GetTechnique base call error.";
   return nullptr;
}

WsfEW_Technique* WsfEW_EA_EP::GetTechnique(unsigned int aIndex) const
{
   ut::log::error() << "WsfEW_EA_EP::GetTechnique base call error.";
   return nullptr;
}

//! Indicates if the specified technique can be applied to the specified system type.
//!
//! @param aId The technique id.
//! @param aSystemFunction The system function.
//! @return Returns true if the technique is valid and can be applied to the system type.
bool WsfEW_EA_EP::CanApplyTechnique(WsfEW_Types::TechniqueId aId, WsfEW_Effect::SystemFunction aSystemFunction) const
{
   bool canApply = false;

   WsfEW_Technique* techPtr = GetTechnique(aId);
   if (techPtr != nullptr)
   {
      canApply = techPtr->CanApplyTechnique(aSystemFunction);
   }

   return canApply;
}

// private
bool WsfEW_EA_EP::PrivateSetTechnique(WsfEW_Types::TechniqueId aTechniqueId, bool aOnOff)
{
   bool inUse = TechniqueIsInUse(aTechniqueId);
   // WsfEW_Technique* techPtr = GetTechnique(aTechniqueId);
   bool changed = false;

   if ((!aOnOff) && inUse)
   {
      // Want "off", but is "on", so remove from "in-use" collection:

      WsfEW_Types::TechniqueIdVec::iterator iter =
         std::find(mTechniquesInUse.begin(), mTechniquesInUse.end(), aTechniqueId);
      if (iter != mTechniquesInUse.end())
      {
         mTechniquesInUse.erase(iter);
         changed = true;
      }
   }
   else if (aOnOff && (!inUse))
   {
      // We want "on", but it is "off", so to turn it on, add it to the "in-use" collection:
      mTechniquesInUse.push_back(aTechniqueId);
      changed = true;
   }
   // else if ((  aOnOff &&   inUse) ||
   //          (! aOnOff && ! inUse))
   //{
   //    // We are happy, do nothing
   // }

   if (changed)
   {
      // The EA technique(s) in use have changed.  Find the new effect.
      // todo      SelectDefaultUnmitigatedEffect();
   }
   return changed;
}

bool WsfEW_EA_EP::AddTechnique(WsfEW_Technique* aTechniquePtr)
{
   WsfEW_Types::TechniqueId techId(aTechniquePtr->GetNameId());
   if (!TechniqueIsAvailable(techId))
   {
      mTechniques.push_back(techId);
      return true;
   }
   return false;
}

//! Check to see if a technique is available for use by technique Id
//!
//! @param aId The technique Id of the technique of interest.
//! @return Returns 'true' if the technique with Id is available, returns 'false otherwise.
bool WsfEW_EA_EP::TechniqueIsAvailable(WsfEW_Types::TechniqueId aId) const
{
   WsfEW_Types::TechniqueIdVec::const_iterator iter = std::find(mTechniques.begin(), mTechniques.end(), aId);
   if (iter != mTechniques.end())
   {
      return true;
   }
   return false;
}

//! Check to see if a technique is in use by technique Id
//!
//! @param aId The technique Id of the technique of interest.
//! @return Returns 'true' if the technique with Id is in use, returns 'false otherwise.
bool WsfEW_EA_EP::TechniqueIsInUse(WsfEW_Types::TechniqueId aId) const
{
   WsfEW_Types::TechniqueIdVec::const_iterator iter = std::find(mTechniquesInUse.begin(), mTechniquesInUse.end(), aId);
   if (iter != mTechniquesInUse.end())
   {
      return true;
   }
   return false;
}

// virtual
bool WsfEW_EA_EP::TechniqueIsExternallyControlled(WsfEW_Types::TechniqueId aId) const
{
   WsfEW_Technique* techPtr = GetTechnique(aId);
   if (techPtr != nullptr)
   {
      return techPtr->IsExternallyControlled();
   }
   return false;
}

//! Selects a technique for use based on technique index
//!
//! @param aIndex Specifies the index of the technique in the list (0 - size mTechniques-1).
//! @return Returns true if the technique was successfully selected.
// virtual
bool WsfEW_EA_EP::SelectTechnique(unsigned int aIndex)
{
   if (aIndex < mTechniques.size())
   {
      WsfEW_Types::TechniqueIdVec::iterator techIter = find(mTechniques.begin(), mTechniques.end(), mTechniques[aIndex]);
      if (techIter != mTechniques.end())
      {
         if (PrivateSetTechnique(mTechniques[aIndex], true))
         {
            if (mDebug)
            {
               auto out = ut::log::debug() << "Selected Technique.";
               out.AddNote() << "Technique: " << mTechniques[aIndex];
            }
         }
         else
         {
            if (mDebug)
            {
               auto out = ut::log::debug() << "Selected Technique already selected.";
               out.AddNote() << "Technique: " << mTechniques[aIndex];
            }
         }
      }
      else
      {
         if (mDebug)
         {
            auto out = ut::log::warning() << "Select Technique Failed: Could not find a valid technique at index.";
            out.AddNote() << "Index: " << aIndex;
         }
         return false;
      }
   }
   else
   {
      if (mDebug)
      {
         auto out = ut::log::warning() << "Select Technique Failed: Index too large.";
         out.AddNote() << "Index: " << aIndex;
         out.AddNote() << "Max Index: " << mTechniques.size();
      }
      return false;
   }
   return true;
}

//! Selects a technique for use based on technique Id
//!
//! @param aId Specifies the Id of the technique in the list.
//! @return Returns true if the technique was successfully selected.
// virtual
bool WsfEW_EA_EP::SelectTechnique(WsfEW_Types::TechniqueId aId)
{
   WsfEW_Types::TechniqueIdVec::iterator techIter = find(mTechniques.begin(), mTechniques.end(), aId);
   if (techIter != mTechniques.end())
   {
      if (PrivateSetTechnique(aId, true))
      {
         if (mDebug)
         {
            auto out = ut::log::debug() << "Selected Technique.";
            out.AddNote() << "Technique: " << aId;
         }
      }
      else
      {
         if (mDebug)
         {
            auto out = ut::log::debug() << "Selected Technique already selected.";
            out.AddNote() << "Technique: " << aId;
         }
      }
   }
   else
   {
      if (mDebug)
      {
         auto out = ut::log::warning() << "Select Technique Failed. Could not find technique.";
         out.AddNote() << "Technique: " << aId;
      }
      return false;
   }
   return true;
}

//! Deselect a technique for use based on technique index
//!
//! @param aIndex Specifies the index of the technique in the list (0 - size mTechniques-1).
//! @return Returns true if the technique was successfully deselected.
// virtual
bool WsfEW_EA_EP::DeselectTechnique(unsigned int aIndex)
{
   if (aIndex < mTechniques.size())
   {
      WsfEW_Types::TechniqueIdVec::iterator techIter = find(mTechniques.begin(), mTechniques.end(), mTechniques[aIndex]);
      if (techIter != mTechniques.end())
      {
         if (PrivateSetTechnique(mTechniques[aIndex], false))
         {
            if (mDebug)
            {
               auto out = ut::log::debug() << "Deselected Technique.";
               out.AddNote() << "Technique: " << mTechniques[aIndex];
            }
         }
         else
         {
            if (mDebug)
            {
               auto out = ut::log::debug() << "Deselected Technique already selected.";
               out.AddNote() << "Technique: " << mTechniques[aIndex];
            }
         }
      }
      else
      {
         if (mDebug)
         {
            auto out = ut::log::warning() << "Deselect Technique Failed. Could not find a valid technique at index.";
            out.AddNote() << "Index: " << aIndex;
         }
         return false;
      }
   }
   else
   {
      if (mDebug)
      {
         auto out = ut::log::warning() << "Deselect Technique Failed. Index too large.";
         out.AddNote() << "Index: " << aIndex;
         out.AddNote() << "Max Index: " << mTechniques.size();
      }
      return false;
   }
   return true;
}

//! Deselect a technique for use based on technique Id
//!
//! @param aId Specifies the Id of the technique in the list.
//! @return Returns true if the technique was successfully deselected.
// virtual
bool WsfEW_EA_EP::DeselectTechnique(WsfEW_Types::TechniqueId aId)
{
   WsfEW_Types::TechniqueIdVec::iterator techIter = find(mTechniques.begin(), mTechniques.end(), aId);
   if (techIter != mTechniques.end())
   {
      if (PrivateSetTechnique(aId, false))
      {
         if (mDebug)
         {
            auto out = ut::log::debug() << "Deselected Technique.";
            out.AddNote() << "Technique: " << aId;
         }
      }
      else
      {
         if (mDebug)
         {
            auto out = ut::log::debug() << "Deselected Technique due to already being selected.";
            out.AddNote() << "Technique: " << aId;
         }
      }
   }
   else
   {
      if (mDebug)
      {
         auto out = ut::log::warning() << "Deselect Technique Failed. Could not find technique.";
         out.AddNote() << "Technique: " << aId;
      }
      return false;
   }
   return true;
}

bool WsfEW_EA_EP::HasFrequencyAgilityEffect() const
{
   for (unsigned int i = 0; i != mTechniques.size(); ++i)
   {
      if (GetTechnique(mTechniques[i])->HasFrequencyAgilityEffect())
      {
         return true;
      }
   }
   return false;
}

bool WsfEW_EA_EP::EffectIsAvailable(WsfStringId aEffectNameId) const
{
   for (unsigned int i = 0; i != mTechniques.size(); ++i)
   {
      if (EffectIsAvailable(mTechniques[i], aEffectNameId))
      {
         return true;
      }
   }
   return false;
}

bool WsfEW_EA_EP::EffectIsAvailable(WsfEW_Types::TechniqueId aTechId, WsfStringId aEffectNameId) const
{
   WsfEW_Technique* techPtr = GetTechnique(aTechId);
   if (techPtr != nullptr)
   {
      return techPtr->EffectIsAvailable(aEffectNameId);
   }
   return false;
}

// virtual
WsfEW_Effect* WsfEW_EA_EP::GetEffect(WsfEW_Types::TechniqueId aId, unsigned int aIndex) const
{
   WsfEW_Effect* effectPtr = nullptr;

   WsfEW_Technique* techPtr = GetTechnique(aId);
   if (techPtr != nullptr)
   {
      effectPtr = techPtr->GetEffect(aIndex);
   }

   return effectPtr;
}

// virtual
WsfEW_Effect* WsfEW_EA_EP::GetEffect(WsfEW_Types::TechniqueId aTechId, WsfStringId aEffectId) const
{
   WsfEW_Effect* effectPtr = nullptr;

   WsfEW_Technique* techPtr = GetTechnique(aTechId);
   if (techPtr != nullptr)
   {
      effectPtr = techPtr->GetEffect(aEffectId);
   }

   return effectPtr;
}

// virtual
const WsfEW_Technique::EffectPtrVec& WsfEW_EA_EP::GetEffects(WsfEW_Types::TechniqueId aId) const
{
   WsfEW_Technique* techPtr = GetTechnique(aId);
   if (techPtr != nullptr)
   {
      return techPtr->GetEffects();
   }

   // return empty vector if technique is not found
   return *(new const WsfEW_Technique::EffectPtrVec);
}

WsfSimulation* WsfEW_EA_EP::GetSimulation() const
{
   return mSimulationPtr;
}
