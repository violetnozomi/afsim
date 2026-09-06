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

#include "WsfEW_EffectTypes.hpp"

#include <memory>

#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfDeferredInput.hpp"
#include "WsfMil.hpp"
#include "WsfScenario.hpp"

// Pre-defined types
#include "WsfEW_AgilityEffect.hpp"
#include "WsfEW_CommEffect.hpp"
#include "WsfEW_CoverPulseEffect.hpp"
#include "WsfEW_FalseTargetEffect.hpp"
#include "WsfEW_PolModEffect.hpp"
#include "WsfEW_PowerEffect.hpp"
#include "WsfEW_PulseSuppressEffect.hpp"
#include "WsfEW_RPJ_Effect.hpp"
#include "WsfEW_RadiusEffect.hpp"
#include "WsfEW_RepeaterEffect.hpp"
#include "WsfEW_SLB_Effect.hpp"
#include "WsfEW_SLC_DegradeEffect.hpp"
#include "WsfEW_SLC_Effect.hpp"
#include "WsfEW_SimpleFT_Effect.hpp"
#include "WsfEW_Technique.hpp"
#include "WsfEW_TrackEffect.hpp"

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfEW_EffectTypes& WsfEW_EffectTypes::Get(WsfScenario& aScenario)
{
   return WsfMilExtension::Get(aScenario).GetEW_EffectTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfEW_EffectTypes& WsfEW_EffectTypes::Get(const WsfScenario& aScenario)
{
   return WsfMilExtension::Get(aScenario).GetEW_EffectTypes();
}

// =================================================================================================
WsfEW_EffectTypes::WsfEW_EffectTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfEW_Effect>(aScenario, "electronic_warfare_effect")
{
   Add("WSF_AGILITY_EFFECT", ut::make_unique<WsfEW_AgilityEffect>());
   Add("WSF_COMM_AGILITY_EFFECT", ut::make_unique<WsfEW_AgilityEffect>());
   Add("WSF_COMM_EFFECT", ut::make_unique<WsfEW_CommEffect>());
   Add("WSF_COVER_PULSE_EFFECT", ut::make_unique<WsfEW_CoverPulseEffect>());
   Add("WSF_FALSE_TARGET_EFFECT", ut::make_unique<WsfEW_FalseTargetEffect>());
   Add("WSF_FT_EFFECT", ut::make_unique<WsfEW_FalseTargetEffect>());
   Add("WSF_JAMMER_POWER_EFFECT", ut::make_unique<WsfEW_PowerEffect>());
   Add("WSF_JAMMER_RADIUS_EFFECT", ut::make_unique<WsfEW_RadiusEffect>());
   Add("WSF_POL_MOD_EFFECT", ut::make_unique<WsfEW_PolModEffect>());
   Add("WSF_POWER_EFFECT", ut::make_unique<WsfEW_PowerEffect>());
   Add("WSF_PULSE_EFFECT", ut::make_unique<WsfEW_PulseEffect>());
   Add("WSF_PULSE_SUPPRESS_EFFECT", ut::make_unique<WsfEW_PulseSuppressEffect>());
   Add("WSF_RADAR_AGILITY_EFFECT", ut::make_unique<WsfEW_AgilityEffect>());
   Add("WSF_RADIUS_EFFECT", ut::make_unique<WsfEW_RadiusEffect>());
   Add("WSF_REPEATER_EFFECT", ut::make_unique<WsfEW_RepeaterEffect>());
   Add("WSF_RPJ_EFFECT", ut::make_unique<WsfEW_RPJ_Effect>());
   Add("WSF_SIMPLE_FT_EFFECT", ut::make_unique<WsfEW_SimpleFT_Effect>());
   Add("WSF_SLC_EFFECT", ut::make_unique<WsfEW_SLC_Effect>());
   Add("WSF_SLC_DEGRADE_EFFECT", ut::make_unique<WsfEW_SLC_DegradeEffect>());
   Add("WSF_SLB_EFFECT", ut::make_unique<WsfEW_SLB_Effect>());
   Add("WSF_TRACK_EFFECT", ut::make_unique<WsfEW_TrackEffect>());
}
// =================================================================================================
//! Load an instance of a 'type object' of this class
//! Process the current command if it would create an instance of an 'type object' of this class.
//! @param aInput The input stream.
//! @param aTechniquePtr The object into which the instance is to be attached.
//! @returns true if the command was recognized as one for instantiating an object of this type.
//! @throws UtInput::ExceptionBase (or derived class) if an error was encountered.
bool WsfEW_EffectTypes::LoadInstance(UtInput& aInput, WsfEW_Technique* aTechniquePtr)
{
   bool myCommand = false;
   if (aInput.GetCommand() == "effect")
   {
      myCommand = true;
      UtInputBlock inputBlock(aInput);
      std::string  instanceName;
      aInput.ReadValue(instanceName);
      if (!aTechniquePtr->EffectIsAvailable(instanceName))
      {
         std::string baseType;
         aInput.ReadValue(baseType);
         GetScenario()->GetDeferredInput().MaybeRequires("electronic_warfare_effect", baseType);
         std::unique_ptr<WsfEW_Effect> instancePtr(Clone(baseType));
         if (instancePtr != nullptr)
         {
            instancePtr->SetName(instanceName);
            inputBlock.ProcessInput(instancePtr.get());
            if (aTechniquePtr->AddEffect(instancePtr.get()))
            {
               instancePtr.release();
            }
            else
            {
               throw UtInput::BadValue(aInput, "Duplicate EW effect: " + instanceName);
            }
         }
         else
         {
            throw UtInput::BadValue(aInput, "Unknown or missing EW Effect Type: " + baseType);
         }
      }
      else
      {
         // Edit an existing instance
         WsfEW_Effect* instancePtr = aTechniquePtr->GetEffect(WsfStringId(instanceName));
         if (instancePtr != nullptr)
         {
            std::string baseType;
            aInput.ReadValue(baseType);
            if (Find(baseType) == nullptr)
            {
               aInput.PushBack(baseType);
               inputBlock.ProcessInput(instancePtr);
            }
            else
            {
               throw UtInput::BadValue(aInput, "Redefining an existing effect from type: " + instanceName);
            }
         }
         else
         {
            throw UtInput::BadValue(aInput, "Unknown EW effect: " + instanceName);
         }
      }
   }

   return myCommand;
}
