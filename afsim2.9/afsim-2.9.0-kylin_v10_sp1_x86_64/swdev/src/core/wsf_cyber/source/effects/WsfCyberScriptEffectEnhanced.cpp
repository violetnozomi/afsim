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

#include "effects/WsfCyberScriptEffectEnhanced.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfCyberEngagement.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"

namespace wsf
{
namespace cyber
{

// =================================================================================================
ScriptEffectEnhanced::ScriptEffectEnhanced(WsfScenario& aScenario)
   : Effect()
   ,
   // Note: The scripts execute in the global context. They access the attacker and victim platforms
   //       from the engagement argument.
   mContext(*aScenario.GetScriptContext(), "WsfCyberEngagement")
{
}

// =================================================================================================
ScriptEffectEnhanced::ScriptEffectEnhanced(const ScriptEffectEnhanced& aSrc, const AttackParameters& aParameters)
   : ScriptEffectEnhanced(aSrc)
{
   mParameters = aParameters;
}

// =================================================================================================
Effect* ScriptEffectEnhanced::Clone() const
{
   return new ScriptEffectEnhanced(*this);
}

// =================================================================================================
Effect* ScriptEffectEnhanced::Clone(const AttackParameters& aParameters) const
{
   return new ScriptEffectEnhanced(*this, aParameters);
}

// =================================================================================================
bool ScriptEffectEnhanced::Initialize(Engagement& aEngagement)
{
   auto& sim = aEngagement.GetSimulation();
   return mContext.Initialize(sim, &aEngagement);
}

// =================================================================================================
bool ScriptEffectEnhanced::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());
   if (command == "platform_type")
   {
      std::string platformType;
      aInput.ReadValue(platformType);
      aInput.PushBack(platformType);
      mPlatformTypeEffect[platformType].ProcessInput(aInput, mContext);
   }
   else if (command == "platform")
   {
      std::string platformName;
      aInput.ReadValue(platformName);
      aInput.PushBack(platformName);
      mPlatformEffect[platformName].ProcessInput(aInput, mContext);
   }
   else if (command == "default")
   {
      mDefaultEffect.ProcessInput(aInput, mContext, true);
   }
   else
   {
      myCommand = Effect::ProcessInput(aInput);
   }

   return myCommand;
}

// =================================================================================================
void ScriptEffectEnhanced::Attack(double aSimTime, Engagement& aEngagement)
{
   FindMatch(aEngagement).Attack(&aEngagement, aSimTime, mContext, mParameters);
}

// =================================================================================================
void ScriptEffectEnhanced::Restore(double aSimTime, Engagement& aEngagement)
{
   FindMatch(aEngagement).Restore(&aEngagement, aSimTime, mContext, mParameters);
}

// =================================================================================================
ScriptEffectEnhanced::EffectInstance ScriptEffectEnhanced::FindMatch(const Engagement& aEngagement) const
{
   //! Try to find a matching mapped value by the most specific criteria
   //! 1. Mapped platform name
   //! 2. Mapped platform type
   //! 3. A parent platform_type that matches a platform_type
   //! 3. Default (if it exists)
   const auto& sim    = aEngagement.GetSimulation();
   auto        victim = sim.GetPlatformByName(aEngagement.GetVictim());
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

// =================================================================================================
void ScriptEffectEnhanced::EffectInstance::ProcessInput(UtInput&          aInput,
                                                        WsfScriptContext& aContext,
                                                        bool              aDefaultUsage) // = false
{
   if (aDefaultUsage)
   {
      SetName("default");
   }
   else
   {
      std::string name;
      aInput.ReadValue(name);
      SetName(name);
   }

   std::string  command;
   UtInputBlock inputBlock(aInput);
   while (inputBlock.ReadCommand(command))
   {
      if (command == "script")
      {
         UtScript*   scriptPtr  = aContext.Compile(aInput, "end_script", GetName());
         std::string scriptName = scriptPtr->GetName();

         if (!aContext.ValidateScript(scriptPtr, "void", "WsfCyberEngagement, WsfCyberAttackParameters"))
         {
            throw UtException(
               "Invalid script return type, argument type, or argument in WSF_CYBER_SCRIPT_EFFECT_ENHANCED::" + GetName() +
               "::" + scriptName);
         }
         if (scriptName == GetName() + "::Attack")
         {
            mAttack = scriptPtr;
         }
         else if (scriptName == GetName() + "::Restore")
         {
            mRestore = scriptPtr;
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
}

// =================================================================================================
void ScriptEffectEnhanced::EffectInstance::Attack(Engagement*       aEngagementPtr,
                                                  double            aSimTime,
                                                  WsfScriptContext& aContext,
                                                  AttackParameters& aParameters)
{
   if (mAttack)
   {
      AttackRestore(aEngagementPtr, aSimTime, aContext, aParameters, mAttack);
   }
}

// =================================================================================================
void ScriptEffectEnhanced::EffectInstance::Restore(Engagement*       aEngagementPtr,
                                                   double            aSimTime,
                                                   WsfScriptContext& aContext,
                                                   AttackParameters& aParameters)
{
   if (mRestore)
   {
      AttackRestore(aEngagementPtr, aSimTime, aContext, aParameters, mRestore);
   }
}

// =================================================================================================
void ScriptEffectEnhanced::EffectInstance::AttackRestore(Engagement*       aEngagementPtr,
                                                         double            aSimTime,
                                                         WsfScriptContext& aContext,
                                                         AttackParameters& aParameters,
                                                         UtScript*         aScriptPtr)
{
   UtScriptData       scriptRetVal;
   UtScriptDataPacker scriptArgs{aContext.GetTypes()};
   scriptArgs.Push(aEngagementPtr, "WsfCyberEngagement") << &aParameters;
   aContext.ExecuteScript(aSimTime, aScriptPtr, scriptRetVal, scriptArgs);
}

} // namespace cyber
} // namespace wsf
