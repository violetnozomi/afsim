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

#include "WsfEW_EP.hpp"

#include <algorithm>
#include <iostream>
#include <memory>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "WsfArticulatedPart.hpp"
#include "WsfEW_EA.hpp"
#include "WsfEW_EP.hpp"
#include "WsfEW_Effect.hpp"
#include "WsfEW_RcvrComponent.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"

// =================================================================================================
//! Convenience method to get the electronic protect object for the specified receiver.
// static
WsfEW_EA_EP* WsfEW_EP::GetElectronicProtect(const WsfEM_Rcvr& aRcvr)
{
   WsfEW_RcvrComponent* componentPtr = WsfEW_RcvrComponent::Find(aRcvr);
   return (componentPtr != nullptr) ? componentPtr->GetElectronicProtect() : nullptr;
}

// =================================================================================================
//! Convenience method to set the electronic protect object for the specified receiver.
// static
void WsfEW_EP::SetElectronicProtect(WsfEM_Rcvr& aRcvr, WsfEW_EA_EP* aElectronicProtectPtr)
{
   WsfEW_RcvrComponent* componentPtr = WsfEW_RcvrComponent::FindOrCreate(aRcvr);
   componentPtr->SetElectronicProtect(aElectronicProtectPtr);
}

// =================================================================================================
WsfEW_EP::WsfEW_EP()
   : WsfEW_EA_EP()
   , mTechniquePtrMap()
{
}

// =================================================================================================
WsfEW_EP::WsfEW_EP(const WsfEW_EP& aSrc)
   : WsfEW_EA_EP(aSrc)
   , mTechniquePtrMap()
{
   for (const auto& techIter : aSrc.mTechniquePtrMap)
   {
      mTechniquePtrMap[techIter.second->GetNameId()] = dynamic_cast<EP_Technique*>(techIter.second->Clone());
   }
}

// =================================================================================================
// virtual
WsfEW_EP::~WsfEW_EP()
{
   for (auto technique : mTechniques)
   {
      auto techIter = mTechniquePtrMap.find(technique);
      if (techIter != mTechniquePtrMap.end())
      {
         delete techIter->second;
      }
   }
}

// =================================================================================================
// virtual
WsfEW_EA_EP* WsfEW_EP::Clone() const
{
   return new WsfEW_EP(*this);
}

// =================================================================================================
// virtual
bool WsfEW_EP::Initialize(WsfSimulation& aSimulation, WsfEM_XmtrRcvr* aXmtrRcvrPtr)
{
   bool success = true;

   for (unsigned int i = 0; i != mTechniques.size(); ++i)
   {
      WsfEW_Technique* techniquePtr = GetTechnique(mTechniques[i]);
      techniquePtr->SetEA_EP_Ptr(this);
   }

   // NOTE: must have EP pointer variable already set
   // in the techniques before calling
   success &= WsfEW_EA_EP::Initialize(aSimulation, aXmtrRcvrPtr);

   return success;
}

//! Get technique for use based on technique Id
//!
//! @param aTechniqueId The Id of the technique.
//! @return Returns pointer to technique if found and NULL ('0') if not found.
// virtual
WsfEW_Technique* WsfEW_EP::GetTechnique(WsfEW_Types::TechniqueId aTechniqueId) const
{
   EP_Technique* techPtr = nullptr;
   auto          idIter  = mTechniquePtrMap.find(aTechniqueId);
   if (idIter != mTechniquePtrMap.end())
   {
      techPtr = (*idIter).second;
   }
   return techPtr;
}

bool WsfEW_EP::AddTechnique(WsfEW_Technique* aTechniquePtr)
{
   bool                     added(false);
   EP_Technique*            epTechPtr(dynamic_cast<EP_Technique*>(aTechniquePtr));
   WsfEW_Types::TechniqueId techId(aTechniquePtr->GetNameId());
   if ((epTechPtr != nullptr) && (mTechniquePtrMap.find(techId) == mTechniquePtrMap.end()))
   {
      mTechniquePtrMap[(aTechniquePtr->GetNameId())] = epTechPtr;
      added                                          = WsfEW_EA_EP::AddTechnique(epTechPtr);
   }
   return added;
}

//! Get technique for use based on technique index
//!
//! @param aIndex Specifies the index of the technique in the list (0 - size mTechniques-1).
//! @return Returns pointer to technique if found and NULL ('0') if not found.
// virtual
WsfEW_Technique* WsfEW_EP::GetTechnique(unsigned int aIndex) const
{
   EP_Technique* techPtr = nullptr;
   if (aIndex < mTechniques.size())
   {
      auto idIter = mTechniquePtrMap.find(mTechniques[aIndex]);
      if (idIter != mTechniquePtrMap.end())
      {
         techPtr = (*idIter).second;
      }
   }
   return techPtr;
}

//! Selects a technique for use based on technique index for the specified System Type
//!
//! @param aIndex Specifies the index of the technique in the list (0 - size mTechniques-1).
//! @param aSystemFunction Indicates what system function that the effect will be applied to (i.e. Comms, Sensors, etc.).
//! @return Returns true if the technique was successfully selected.
// virtual
bool WsfEW_EP::SelectTechnique(unsigned int aIndex, WsfEW_Effect::SystemFunction aSystemFunction)
{
   if (aIndex < mTechniques.size())
   {
      auto techIter = mTechniquePtrMap.find(mTechniques[aIndex]);
      if (techIter != mTechniquePtrMap.end())
      {
         if (techIter->second->CanApplyTechnique(aSystemFunction))
         {
            if (PrivateSetTechnique(mTechniques[aIndex], true))
            {
               if (mDebug)
               {
                  auto out = ut::log::debug() << "Selected technique for system type.";
                  out.AddNote() << "Technique: " << techIter->second->GetName();
                  out.AddNote() << "System Type: " << aSystemFunction;
               }
            }
            else
            {
               if (mDebug)
               {
                  auto out = ut::log::debug() << "Selected technique for system type already selected.";
                  out.AddNote() << "Technique: " << techIter->second->GetName();
                  out.AddNote() << "System Type: " << aSystemFunction;
               }
            }
         }
         else
         {
            if (mDebug)
            {
               auto out = ut::log::warning() << "Select Technique Failed. Could not apply the technique at index due "
                                                "to an incompatible system type.";
               out.AddNote() << "Technique: " << techIter->second->GetName();
               out.AddNote() << "System Type: " << aSystemFunction;
               out.AddNote() << "Index: " << aIndex;
            }
            return false;
         }
      }
      else
      {
         if (mDebug)
         {
            auto out = ut::log::warning()
                       << "Select Technique Failed. Could not find a valid technique at index for system type.";
            out.AddNote() << "System Type: " << aSystemFunction;
            out.AddNote() << "Index: " << aIndex;
         }
         return false;
      }
   }
   else
   {
      if (mDebug)
      {
         auto out = ut::log::warning() << "Select Technique Failed. Index too large.";
         out.AddNote() << "Index: " << aIndex;
         out.AddNote() << "Max Index: " << mTechniques.size();
      }
      return false;
   }
   return true;
}

//! Selects a technique for use based on technique Id for the specified System Type
//!
//! @param aId Specifies the Id of the technique in the list.
//! @param aSystemFunction Indicates what system function that the effect will be applied to (i.e. Comms, Sensors, etc.).
//! @return Returns true if the technique was successfully selected.
// virtual
bool WsfEW_EP::SelectTechnique(WsfEW_Types::TechniqueId aId, WsfEW_Effect::SystemFunction aSystemFunction)
{
   auto techIter = mTechniquePtrMap.find(aId);
   if (techIter != mTechniquePtrMap.end())
   {
      if (techIter->second->CanApplyTechnique(aSystemFunction))
      {
         if (PrivateSetTechnique(aId, true))
         {
            if (mDebug)
            {
               auto out = ut::log::debug() << "Selected technique for system type.";
               out.AddNote() << "Technique: " << techIter->second->GetName();
               out.AddNote() << "ID: " << aId;
               out.AddNote() << "System Type: " << aSystemFunction;
            }
         }
         else
         {
            if (mDebug)
            {
               auto out = ut::log::debug() << "Selected technique for system type was already selected.";
               out.AddNote() << "Technique: " << techIter->second->GetName();
               out.AddNote() << "Id:" << aId;
               out.AddNote() << "System Type: " << aSystemFunction;
            }
         }
      }
      else
      {
         if (mDebug)
         {
            auto out = ut::log::warning()
                       << "Select Technique Failed. Could not apply technique due to incompatible system type.";
            out.AddNote() << "Technique: " << techIter->second->GetName();
            out.AddNote() << "Id: " << aId;
            out.AddNote() << "System Type: " << aSystemFunction;
         }
         return false;
      }
   }
   else
   {
      if (mDebug)
      {
         auto out = ut::log::warning() << "Select Technique Failed. Could not find technique for system type.";
         out.AddNote() << "Id: " << aId;
         out.AddNote() << "System Type: " << aSystemFunction;
      }
      return false;
   }

   return true;
}

//! Deselect a technique for use based on technique index for the specified System Type
//!
//! @param aIndex Specifies the index of the technique in the list (0 - size mTechniques-1).
//! @param aSystemFunction Indicates what system function that the effect will be applied to (i.e. Comms, Sensors, etc.).
//! @return Returns true if the technique was successfully deselected.
// virtual
bool WsfEW_EP::DeselectTechnique(unsigned int aIndex, WsfEW_Effect::SystemFunction aSystemFunction)
{
   if (aIndex < mTechniques.size())
   {
      auto techIter = mTechniquePtrMap.find(mTechniques[aIndex]);
      if (techIter != mTechniquePtrMap.end())
      {
         if (techIter->second->CanApplyTechnique(aSystemFunction))
         {
            if (PrivateSetTechnique(mTechniques[aIndex], false))
            {
               if (mDebug)
               {
                  auto out = ut::log::debug() << "Deselected technique for system type.";
                  out.AddNote() << "Technique: " << techIter->second->GetName();
                  out.AddNote() << "System Type: " << aSystemFunction;
               }
            }
            else
            {
               if (mDebug)
               {
                  auto out = ut::log::debug() << "Deselected technique for system type already selected.";
                  out.AddNote() << "Technique: " << techIter->second->GetName();
                  out.AddNote() << "System Type: " << aSystemFunction;
               }
            }
         }
         else
         {
            if (mDebug)
            {
               auto out = ut::log::warning() << "Deselect Technique Failed. Could not apply the technique at index due "
                                                "to an incompatible system type.";
               out.AddNote() << "Technique: " << techIter->second->GetName();
               out.AddNote() << "System Type: " << aSystemFunction;
               out.AddNote() << "Index: " << aIndex;
            }
            return false;
         }
      }
      else
      {
         if (mDebug)
         {
            auto out = ut::log::warning() << "Deselect Technique Failed. Could not find a valid technique at index.";
            out.AddNote() << "System Type: " << aSystemFunction;
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

//! Deselect a technique for use based on technique Id for the specified System Type
//!
//! @param aId Specifies the Id of the technique in the list.
//! @param aSystemFunction Indicates what system function that the effect will be applied to (i.e. Comms, Sensors, etc.).
//! @return Returns true if the technique was successfully deselected.
// virtual
bool WsfEW_EP::DeselectTechnique(WsfEW_Types::TechniqueId aId, WsfEW_Effect::SystemFunction aSystemFunction)
{
   auto techIter = mTechniquePtrMap.find(aId);
   if (techIter != mTechniquePtrMap.end())
   {
      if (techIter->second->CanApplyTechnique(aSystemFunction))
      {
         if (PrivateSetTechnique(aId, false))
         {
            if (mDebug)
            {
               auto out = ut::log::debug() << "Deselected technique for system type.";
               out.AddNote() << "Technique: " << techIter->second->GetName();
               out.AddNote() << "ID: " << aId;
               out.AddNote() << "System Type: " << aSystemFunction;
            }
         }
         else
         {
            if (mDebug)
            {
               auto out = ut::log::debug() << "Deselected technique for system type already selected.";
               out.AddNote() << "Technique: " << techIter->second->GetName();
               out.AddNote() << "ID: " << aId;
               out.AddNote() << "System Type: " << aSystemFunction;
            }
         }
      }
      else
      {
         if (mDebug)
         {
            auto out = ut::log::warning()
                       << "Deselect Technique Failed. Could not apply technique due to an incompatible system type.";
            out.AddNote() << "Technique: " << techIter->second->GetName();
            out.AddNote() << "Id: " << aId;
            out.AddNote() << "System Type: " << aSystemFunction;
         }
         return false;
      }
   }
   else
   {
      if (mDebug)
      {
         auto out = ut::log::warning() << "Deselect Technique Failed. Could not find technique for system type.";
         out.AddNote() << "Id: " << aId;
         out.AddNote() << "System Type: " << aSystemFunction;
      }
      return false;
   }

   return true;
}

WsfEW_Types::TechniqueIdVec* WsfEW_EP::GetMitigatedTechniqueIds(WsfEW_Types::TechniqueId aId) const
{
   WsfEW_Types::TechniqueIdVec* mitigatedTechniqueIds = GetTechnique(aId)->GetMitigatedTechniqueIds();
   return mitigatedTechniqueIds;
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
WsfEW_Technique::EffectPtrVec WsfEW_EP::GetMitigatingEffects(const WsfEW_Effect*          aEA_EffectPtr,
                                                             WsfEW_Effect::SystemFunction aAffectingSystemFunction,
                                                             WsfEW_Effect::SystemFunction aAffectedSystemFunction)
{
   WsfEW_Technique::EffectPtrVec resultingEffects;
   resultingEffects.clear();

   ResetTechniquesInUseList();

   const WsfEW_Technique* EA_TechToCounter = aEA_EffectPtr->GetTechniquePtr();
   if (EA_TechToCounter == nullptr)
   {
      return resultingEffects;
   }

   // Make sure this technique can be applied, before we try to match an
   // EP technique to it.
   if (EA_TechToCounter->CanApplyTechnique(aAffectedSystemFunction))
   {
      // Now for each EP Technique we have in our bag of tricks, see if it
      // has one or more effects that will mitigate the applied eaTechnique.
      // If so, turn that EP technique to "on".
      for (WsfEW_Types::TechniqueId possibleEP_TechId : mTechniques)
      {
         if (CanApplyTechnique(possibleEP_TechId, aAffectingSystemFunction))
         {
            WsfEW_Technique*                  possibleEP_TechPtr = GetTechnique(possibleEP_TechId);
            const WsfEW_Types::TechniqueIdVec mitigatedTechniqueFlags =
               *(possibleEP_TechPtr->GetMitigatedTechniqueClassIds());
            const WsfEW_Types::TechniqueIdVec mitigatedTechniqueIds = *(possibleEP_TechPtr->GetMitigatedTechniqueIds());
            // const TechniqueIdVec mitigatedTechniques = *(GetMitigatedTechniqueIds(possibleEP_TechId));

            WsfEW_Types::TechniqueId EA_TechId           = EA_TechToCounter->GetNameId();
            WsfStringId              EA_MitigationFlagId = EA_TechToCounter->GetMitigationClassId();

            if ((find(mitigatedTechniqueFlags.begin(), mitigatedTechniqueFlags.end(), EA_MitigationFlagId) !=
                 mitigatedTechniqueFlags.end()) ||
                (find(mitigatedTechniqueIds.begin(), mitigatedTechniqueIds.end(), EA_TechId) != mitigatedTechniqueIds.end()))
            {
               // If we are not allowed to set the technique to use internally,
               // then this desired technique may or may not actually be "on"
               // right now.
               if (!possibleEP_TechPtr->IsExternallyControlled())
               {
                  PrivateSetTechnique(possibleEP_TechId, true);
               }

               // We have to test to see if it has been set to
               // "on" either internally or externally.
               if (TechniqueIsInUse(possibleEP_TechId))
               {
                  WsfEW_Technique::EffectPtrVec epEffects    = possibleEP_TechPtr->GetEffects();
                  auto                          epEffectIter = epEffects.begin();
                  for (; epEffectIter != epEffects.end(); ++epEffectIter)
                  {
                     // add to list to return
                     resultingEffects.push_back(*epEffectIter);

                     if (mDebug)
                     {
                        auto out = ut::log::debug()
                                   << "WsfEW_EP selects EP Effect based on technique name/flag to mitigate EA Effect.";
                        out.AddNote() << "EP Effect: " << (*epEffectIter)->GetName();
                        out.AddNote() << "EA Effect: " << aEA_EffectPtr->GetName();
                     }
                  }
               }
            }
         }
         else if (mDebug)
         {
            auto out = ut::log::debug() << "WsfEW_EP cannot apply EP Technique due to system type incompatibility.";
            out.AddNote() << "EP Technique: " << possibleEP_TechId;
         }
      }

      if (!resultingEffects.empty())
      {
         // Need to get rid of any possible duplicates
         sort(resultingEffects.begin(), resultingEffects.end());
         auto it = unique(resultingEffects.begin(), resultingEffects.end());
         resultingEffects.resize(it - resultingEffects.begin());
      }
      else if (mDebug)
      {
         auto out = ut::log::debug() << "WsfEW_EP could not find an EP technique effect to mitigate EA Effect.";
         out.AddNote() << "T = " << GetSimulation()->GetSimTime();
         out.AddNote() << "EA Effect: " << aEA_EffectPtr->GetName();
      }
   } // if EA technique can be applied to this system
   else if (mDebug)
   {
      auto out = ut::log::debug() << "WsfEW_EP cannot apply EA Effect due to SystemType incompatibility.";
      out.AddNote() << "EA Effect: " << aEA_EffectPtr->GetName();
   }
   return resultingEffects;
}

// *************************************************************************
WsfEW_EP::EP_Technique::EP_Technique()
   : WsfEW_Technique()
   , mMitigatedTechniqueIds()
   , mMitigatedTechniqueClassIds()
   , mExternallyControlled(false)
{
}

WsfEW_EP::EP_Technique::EP_Technique(const EP_Technique& aSrc)


   = default;

// virtual
WsfEW_Technique* WsfEW_EP::EP_Technique::Clone() const
{
   return new EP_Technique(*this);
}

bool WsfEW_EP::EP_Technique::Initialize(WsfSimulation& aSimulation)
{
   return WsfEW_Technique::Initialize(aSimulation);
}

bool WsfEW_EP::EP_Technique::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "mitigated_techniques")
   {
      UtInputBlock inputBlock(aInput);
      ProcessMitigatedTechniquesBlock(inputBlock);
   }
   else if (command == "mitigated_technique_classes")
   {
      UtInputBlock inputBlock(aInput);
      ProcessMitigatedTechniqueFlagsBlock(inputBlock);
   }
   else if (command == "internally_controlled")
   {
      mExternallyControlled = false;
   }
   else if (command == "externally_controlled")
   {
      mExternallyControlled = true;
   }
   else
   {
      myCommand = WsfEW_Technique::ProcessInput(aInput);
   }
   return myCommand;
}

void WsfEW_EP::EP_Technique::ProcessMitigatedTechniquesBlock(UtInputBlock& aInputBlock)
{
   std::string techniqueName;
   while (aInputBlock.ReadCommand(techniqueName))
   {
      WsfEW_Types::TechniqueId eaTechniqueId = techniqueName;
      mMitigatedTechniqueIds.push_back(eaTechniqueId);
   }
}

void WsfEW_EP::EP_Technique::ProcessMitigatedTechniqueFlagsBlock(UtInputBlock& aInputBlock)
{
   std::string techniqueName;
   while (aInputBlock.ReadCommand(techniqueName))
   {
      WsfEW_Types::TechniqueId eaTechniqueId = techniqueName;
      mMitigatedTechniqueClassIds.push_back(eaTechniqueId);
   }
}
