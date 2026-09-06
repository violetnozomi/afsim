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

#include "WsfEW_EffectManager.hpp"

#include "WsfFalseTarget.hpp"
#include "WsfFalseTargetScreener.hpp"
#include "WsfMil.hpp"

// =================================================================================================
//! Return a pointer to the type list associated with the specified scenario.
WsfEW_EffectManager* WsfEW_EffectManager::Find(WsfSimulation& aSimulation)
{
   auto wsfMilPtr = WsfMilInterface::Find(aSimulation);
   if (wsfMilPtr != nullptr)
   {
      return &wsfMilPtr->GetEW_EffectManager();
   }
   return nullptr;
}

// =================================================================================================
WsfEW_EffectManager::~WsfEW_EffectManager()
{
   for (auto& falseTargetType : mFalseTargetTypes)
   {
      for (auto& screener : mFT_Screeners)
      {
         screener.first->RemoveFalseTargets(falseTargetType.second.get());
      }
   }
}

// =================================================================================================
void WsfEW_EffectManager::AddFalseTargetType(WsfFalseTarget* aFalseTargetType)
{
   auto result = mFalseTargetTypes.emplace(aFalseTargetType->GetName(), std::unique_ptr<WsfFalseTarget>(aFalseTargetType));
   if (!result.second)
   {
      throw UtException("Duplicate False Target Type of " + aFalseTargetType->GetName() + " in WsfEW_EffectManager");
   }
}

// =================================================================================================
WsfFalseTarget* WsfEW_EffectManager::FindFalseTargetType(const std::string& aTypeName) const
{
   auto it = mFalseTargetTypes.find(aTypeName);
   if (it != std::end(mFalseTargetTypes))
   {
      return it->second.get();
   }
   return nullptr;
}

// =================================================================================================
void WsfEW_EffectManager::AddFalseTargetScreener(WsfFalseTarget* aFalseTargetPtr, WsfFalseTargetScreener* aFT_Screener)
{
   if (FindFalseTargetType(aFalseTargetPtr->GetName()) != nullptr)
   {
      mFT_Screeners.emplace_back(aFT_Screener, aFalseTargetPtr);
   }
}

// =================================================================================================
void WsfEW_EffectManager::RemoveFalseTargetScreener(WsfFalseTargetScreener* aFT_Screener)
{
   mFT_Screeners.remove_if([aFT_Screener](const FT_Screeners::value_type& val) { return val.first == aFT_Screener; });
}

// =================================================================================================
void WsfEW_EffectManager::RemoveFalseTargetScreener(WsfFalseTarget* aFalseTargetPtr, WsfFalseTargetScreener* aFT_Screener)
{
   mFT_Screeners.remove(std::make_pair(aFT_Screener, aFalseTargetPtr));
}
