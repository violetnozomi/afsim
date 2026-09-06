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

#include "effects/WsfCyberMitmEffect.hpp"

#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfComm.hpp"
#include "WsfCommLayer.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCyberEngagement.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "effects/WsfCyberMitmLayer.hpp"
#include "script/WsfScriptContext.hpp"

namespace wsf
{

namespace cyber
{
// ============================================================================
MitmEffect::MitmEffect(WsfScenario& aScenario)
   : Effect()
   , mContextPtr{std::make_shared<WsfScriptContext>(*aScenario.GetScriptContext(), "WsfCyberEngagement")}
{
}

// ============================================================================
MitmEffect::MitmEffect(const MitmEffect& aSrc)
   : Effect(aSrc)
   , mContextPtr(std::make_shared<WsfScriptContext>(*aSrc.mContextPtr))
   , mEffectFactory(aSrc.mEffectFactory)
   , mActiveEffects{}
{
}

// ============================================================================
MitmEffect* MitmEffect::Clone() const
{
   return new MitmEffect(*this);
}

// ============================================================================
bool MitmEffect::Initialize(Engagement& aEngagement)
{
   return mContextPtr->Initialize(aEngagement.GetSimulation(), &aEngagement);
}

// ============================================================================
bool MitmEffect::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());
   if (command == "platform_type")
   {
      std::string platformType;
      aInput.ReadValue(platformType);
      // Read effect for type
      mEffectFactory.AddPlatformTypeEffect(platformType, EffectType{aInput, *mContextPtr, platformType});
   }
   else if (command == "platform")
   {
      std::string platform;
      aInput.ReadValue(platform);
      // Read effect to platform
      mEffectFactory.AddPlatformEffect(platform, EffectType{aInput, *mContextPtr, platform});
   }
   else if (command == "default")
   {
      // Default effect
      mEffectFactory.SetDefaultEffect(EffectType{aInput, *mContextPtr, "default"});
   }
   else
   {
      myCommand = Effect::ProcessInput(aInput);
   }

   return myCommand;
}

// ============================================================================
void MitmEffect::Attack(double aSimTime, Engagement& aEngagement)
{
   auto mitmEffect = mEffectFactory.CreateEffect(aEngagement);
   mitmEffect.Attack(aSimTime, aEngagement, mContextPtr);
   mActiveEffects[aEngagement.GetKey()] = mitmEffect;
}

// ============================================================================
void MitmEffect::Restore(double, Engagement& aEngagement)
{
   auto it = mActiveEffects.find(aEngagement.GetKey());
   if (it != mActiveEffects.end())
   {
      it->second.Restore(aEngagement);
      mActiveEffects.erase(it);
   }
}

// ============================================================================
void MitmEffect::EffectType::Attack(double, Engagement& aEngagement, const std::shared_ptr<WsfScriptContext>& aContextPtr)
{
   // Make sure the victim exists.
   auto& sim    = aEngagement.GetSimulation();
   auto  victim = sim.GetPlatformByName(aEngagement.GetVictim());
   if (!victim)
   {
      return;
   }
   // If all of the comms are supposed to be attacked, get the names.
   if (mAllComms)
   {
      auto numComms = victim->GetComponentCount<comm::Comm>();
      for (unsigned int i = 0; i < numComms; ++i)
      {
         auto* comm = victim->GetComponentEntry<comm::Comm>(i);
         if (comm)
         {
            mComms.push_back(comm->GetName());
         }
      }
   }

   // For each comm that this attack is effective for, inject the MITM_Layers.
   // Two layers are added, one for send and one for receive, such that
   // the ordering of the attack effects is retained for both going up and down the stack.
   for (const auto& commName : mComms)
   {
      auto commPtr = victim->GetComponent<comm::Comm>(commName);
      if (commPtr)
      {
         // Create send and receive MITM layers.
         auto mitmLayerSend = ut::make_unique<comm::Layer>(
            comm::Layer::LayerType::cNONE,
            ut::make_unique<MITM_Layer>(aContextPtr,
                                        mOnSend,
                                        true,
                                        mExfiltrate ? ut::optional<size_t>(aEngagement.GetAttackerIndex()) : ut::nullopt)
               .release());
         auto mitmLayerReceive =
            ut::make_unique<comm::Layer>(comm::Layer::LayerType::cNONE,
                                         ut::make_unique<MITM_Layer>(aContextPtr, mOnReceive, false, ut::nullopt).release());

         size_t sendID, receiveID;

         // Determine where to insert the receive layer. If an index is indicated equal to the size, then
         // the receive layer is indicated to be added at the end of the stack.
         auto         insertionIndex = GetInsertionIndex(*commPtr);
         comm::Layer* layerPtr       = nullptr;

         if (insertionIndex == commPtr->GetProtocolStack().Size())
         {
            layerPtr = commPtr->GetProtocolStack().AddLayer(std::move(mitmLayerReceive));
         }
         else
         {
            layerPtr = commPtr->GetProtocolStack().InsertLayer(insertionIndex, std::move(mitmLayerReceive));
         }

         if (layerPtr)
         {
            layerPtr->SetID(sim);
            receiveID = layerPtr->GetUniqueId();
         }
         else
         {
            throw UtException("Unable to add receive layer for wsf::cyber::MitmEffect.");
         }

         // The send layer always goes at the end of the stack.
         layerPtr = commPtr->GetProtocolStack().AddLayer(std::move(mitmLayerSend));
         if (layerPtr)
         {
            layerPtr->SetID(sim);
            sendID = layerPtr->GetUniqueId();
         }
         else
         {
            throw UtException("Unable to add send layer for wsf::cyber::MitmEffect.");
         }

         mAttackedComms.emplace_back(commPtr, sendID, receiveID);
      }
   }
}

// ============================================================================
void MitmEffect::EffectType::Restore(Engagement& aEngagement)
{
   // Make sure the victim still exists, if not then the restore isn't necessary and the comm
   // would already be deleted.
   const auto& sim    = aEngagement.GetSimulation();
   auto        victim = sim.GetPlatformByName(aEngagement.GetVictim());
   if (!victim)
   {
      return;
   }

   RemoveMITM_Layers();
}

// ============================================================================
size_t MitmEffect::EffectType::GetInsertionIndex(const comm::Comm& aComm) const
{
   // Attempt to find a physical layer. Normally at index 0, but we will cover edge cases as well.
   auto& protocolStack = aComm.GetProtocolStack();
   auto  stackSize     = protocolStack.Size();
   auto  layerPtr      = protocolStack.GetLayer(comm::Layer::LayerType::cPHYSICAL);

   if (layerPtr)
   {
      // A physical layer exists. Iterate from the start of the stack to the end,
      // and provide the index immediately after we have encountered a physical layer,
      // but then encounter something different.
      auto foundPhysical = false;
      for (size_t i = 0U; i < stackSize; ++i)
      {
         layerPtr = protocolStack.GetLayer(i);
         if (layerPtr->GetLayerType() == comm::Layer::LayerType::cPHYSICAL)
         {
            // Mark the physical layer as found and continue on.
            foundPhysical = true;
         }
         else if (foundPhysical)
         {
            // Not a physical layer, and one was previously found. This is our insertion location.
            return layerPtr->GetCommLayerImp()->GetIndex();
         }
      }

      // Edge case, where only one or more physical layers exist.
      // Or, a custom use case where the physical layer is at the end of the stack.
      // Index is at the end of the stack.
      return stackSize;
   }
   else
   {
      // No physical layer. Insert/add at beginning of stack.
      return 0U;
   }
}

// ============================================================================
void MitmEffect::EffectType::RemoveMITM_Layers()
{
   for (const auto& commData : mAttackedComms)
   {
      if (!commData.mCommPtr->GetProtocolStack().RemoveLayerByID(commData.mSendLayerID))
      {
         throw UtException("MITM Layer erroneous data.");
      }
      if (!commData.mCommPtr->GetProtocolStack().RemoveLayerByID(commData.mReceiveLayerID))
      {
         throw UtException("MITM Layer erroneous data.");
      }
   }

   mAttackedComms.clear();
}

// ============================================================================
MitmEffect::EffectType::EffectType(UtInput& aInput, WsfScriptContext& aContext, const std::string& aName)
   : WsfNamed(aName)
{
   std::string  command;
   UtInputBlock inputBlock(aInput);
   while (inputBlock.ReadCommand(command))
   {
      if (command == "script")
      {
         UtScript*   scriptPtr  = aContext.Compile(aInput, "end_script", GetName());
         std::string scriptName = scriptPtr->GetName();

         if (!aContext.ValidateScript(scriptPtr, "WsfCyberMitmMessage", "WsfMessage"))
         {
            throw UtException("Invalid script return type, argument type, or argument in WSF_CYBER_MITM_EFFECT::" +
                              GetName() + "::" + scriptName);
         }
         if (scriptName == GetName() + "::OnSend")
         {
            mOnSend = scriptPtr;
         }
         else if (scriptName == GetName() + "::OnReceive")
         {
            mOnReceive = scriptPtr;
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      else if (command == "exfiltrate")
      {
         mExfiltrate = aInput.ReadBool();
      }
      else if (command == "target_list")
      {
         UtInputBlock inputBlock2(aInput);
         while (inputBlock2.ReadCommand(command))
         {
            mComms.push_back(command);
         }
      }
      else if (command == "all")
      {
         mAllComms = true;
      }
   }
}

// ============================================================================
MitmEffect::EffectType MitmEffect::EffectTypeFactory::CreateEffect(const Engagement& aEngagement) const
{
   //! Try to find a matching mapped value by the most specific criteria
   //! 1. Mapped platform name
   //! 2. Mapped platform type
   //! 3. A parent platform_type that matches a platform_type
   //! 4. Default (if it exists)
   const auto& sim    = aEngagement.GetSimulation();
   auto*       victim = sim.GetPlatformByName(aEngagement.GetVictim());
   if (victim)
   {
      //! Try to match the platform name to a platform listed with the effect
      auto platIt = mPlatformEffect.find(victim->GetName());
      if (platIt != mPlatformEffect.end())
      {
         return platIt->second;
      }

      for (const auto& type : victim->GetTypeList())
      {
         auto platformTypeIt = mPlatformTypeEffect.find(type);
         if (platformTypeIt != mPlatformTypeEffect.end())
         {
            return platformTypeIt->second;
         }
      }
   }
   return mDefaultEffect;
}

// ============================================================================
void MitmEffect::EffectTypeFactory::AddPlatformTypeEffect(const std::string& aPlatformType, const EffectType& aEffect)
{
   mPlatformTypeEffect[aPlatformType] = aEffect;
}

// ============================================================================
void MitmEffect::EffectTypeFactory::AddPlatformEffect(const std::string& aPlatform, const EffectType& aEffect)
{
   mPlatformEffect[aPlatform] = aEffect;
}

// ============================================================================
void MitmEffect::EffectTypeFactory::SetDefaultEffect(const EffectType& aEffect)
{
   mDefaultEffect = aEffect;
}

} // namespace cyber
} // namespace wsf
