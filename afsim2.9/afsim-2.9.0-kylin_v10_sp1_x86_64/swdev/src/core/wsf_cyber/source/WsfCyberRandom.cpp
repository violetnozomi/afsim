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

#include "WsfCyberRandom.hpp"

#include "UtRandom.hpp"
#include "UtRangeAlgorithm.hpp"
#include "WsfCyberAttackTypes.hpp"
#include "WsfCyberScenarioExtension.hpp"
#include "WsfRandomVariable.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace cyber
{
namespace random
{
namespace
{
// =================================================================================================
ProbabilityFrequency GetAttackProbabilityFrequency(const Attack& aAttack, ProbabilityType aType)
{
   switch (aType)
   {
   case cSCAN_DETECTION:
   {
      return aAttack.GetFrequencyOfScanDetection();
   }
   case cSCAN_ATTRIBUTION:
   {
      return aAttack.GetFrequencyOfScanAttribution();
   }
   case cATTACK_SUCCESS:
   {
      return aAttack.GetFrequencyOfAttackSuccess();
   }
   case cSTATUS_REPORT:
   {
      return aAttack.GetFrequencyOfStatusReport();
   }
   case cATTACK_DETECTION:
   {
      return aAttack.GetFrequencyOfAttackDetection();
   }
   case cATTACK_ATTRIBUTION:
   {
      return aAttack.GetFrequencyOfAttackAttribution();
   }
   case cFUTURE_IMMUNITY:
   {
      return aAttack.GetFrequencyOfFutureImmunity();
   }
   }

   throw UtException("Invalid Probability Type with wsf::cyber::random::RandomManager");
}

// =================================================================================================
WsfRandomVariable& GetAttackRandomVariable(Attack& aAttack, ProbabilityType aType)
{
   switch (aType)
   {
   case cSCAN_DETECTION:
   {
      return aAttack.GetRandomVariableScanDetection();
   }
   case cSCAN_ATTRIBUTION:
   {
      return aAttack.GetRandomVariableScanAttribution();
   }
   case cATTACK_SUCCESS:
   {
      return aAttack.GetRandomVariableAttackSuccess();
   }
   case cSTATUS_REPORT:
   {
      return aAttack.GetRandomVariableStatusReport();
   }
   case cATTACK_DETECTION:
   {
      return aAttack.GetRandomVariableAttackDetection();
   }
   case cATTACK_ATTRIBUTION:
   {
      return aAttack.GetRandomVariableAttackAttribution();
   }
   case cFUTURE_IMMUNITY:
   {
      return aAttack.GetRandomVariableFutureImmunity();
   }
   }

   throw UtException("Invalid Probability Type with wsf::cyber::random::RandomManager");
}

// =================================================================================================
RandomManager::DrawCache& GetDrawCache(RandomManager::AttackDistributionCache& aAttackCache, ProbabilityType aType)
{
   switch (aType)
   {
   case cSCAN_DETECTION:
   {
      return aAttackCache.mScanDetectionCache;
   }
   case cSCAN_ATTRIBUTION:
   {
      return aAttackCache.mScanAttributionCache;
   }
   case cATTACK_SUCCESS:
   {
      return aAttackCache.mAttackSuccessCache;
   }
   case cSTATUS_REPORT:
   {
      return aAttackCache.mStatusReportCache;
   }
   case cATTACK_DETECTION:
   {
      return aAttackCache.mAttackDetectionCache;
   }
   case cATTACK_ATTRIBUTION:
   {
      return aAttackCache.mAttackAttributionCache;
   }
   case cFUTURE_IMMUNITY:
   {
      return aAttackCache.mFutureImmunityCache;
   }
   }

   throw UtException("Invalid Probability Type with wsf::cyber::random::RandomManager");
}

} // namespace

// =================================================================================================
RandomManager::RandomManager(WsfSimulation* aSimulation)
   : mSimulation(aSimulation)
{
}

// =================================================================================================
double RandomManager::Draw(const std::string& aAttackType, const std::string& aTarget, ProbabilityType aType)
{
   //! Get the frequency of the probability of the attack we're drawing for
   ProbabilityFrequency drawFrequency;
   auto&                cyberScenario = ScenarioExtension::Get(mSimulation->GetScenario());
   auto                 attack        = cyberScenario.GetAttackTypes().Find(aAttackType);
   if (!attack)
   {
      return -1.0;
   }

   drawFrequency = GetAttackProbabilityFrequency(*attack, aType);

   switch (drawFrequency)
   {
   case cALWAYS:
   {
      //! If the draw frequency is "cALWAYS", we always do a draw. Ignore caching. Simply serve a value
      return mSimulation->GetRandom().Uniform<double>();
   }
   case cONCE_PER_SIMULATION:
   {
      return pDraw(aAttackType, aType);
   }
   case cONCE_PER_TARGET:
   {
      return pDraw(aAttackType, aTarget, aType);
   }
   case cINTERVAL_PER_SIMULATION:
   {
      return pDraw(aAttackType, aType, GetAttackRandomVariable(*attack, aType));
   }
   case cINTERVAL_PER_TARGET:
   {
      return pDraw(aAttackType, aTarget, aType, GetAttackRandomVariable(*attack, aType));
   }
   case cOTHER:
   {
      // User defined draw method for extensibility.
      return attack->ExtendedDrawDefinition(aTarget);
   }
   default:
   {
      throw UtException("Invalid draw frequency encountered in wsf::cyber::random::RandomManager::Draw");
   }
   }
}

// =================================================================================================
//! cONCE_PER_SIMULATION draw method
double RandomManager::pDraw(const std::string& aAttackType, ProbabilityType aType)
{
   if (contains(GetCacheMap(), aAttackType)) // If the entry exists...
   {
      auto& tempCache = GetDrawCache(mAttackDrawCache[aAttackType], aType);
      if (tempCache.mSimulationDraw >= 0) // and the entry is valid...
      {
         return tempCache.mSimulationDraw; // return the cached entry
      }
      else // the entry is NOT valid - do a draw, update cache, and return the draw
      {
         auto draw                 = mSimulation->GetRandom().Uniform<double>();
         tempCache.mSimulationDraw = draw;
         return draw;
      }
   }
   else // no entry - do a draw, create cache entry, and return the draw
   {
      AttackDistributionCache tempAttackCache;
      mAttackDrawCache[aAttackType] = tempAttackCache;
      auto& tempCache               = GetDrawCache(mAttackDrawCache[aAttackType], aType);
      auto  draw                    = mSimulation->GetRandom().Uniform<double>();
      tempCache.mSimulationDraw     = draw;
      return draw;
   }
}

// =================================================================================================
//! cONCE_PER_TARGET draw method
double RandomManager::pDraw(const std::string& aAttackType, const std::string& aTargetName, ProbabilityType aType)
{
   if (contains(GetCacheMap(), aAttackType))
   {
      auto& tempCache = GetDrawCache(mAttackDrawCache[aAttackType], aType);
      if (contains(tempCache.mTargetDraw, aTargetName))
      {
         if (tempCache.mTargetDraw[aTargetName] >= 0)
         {
            return tempCache.mTargetDraw[aTargetName];
         }
         else
         {
            auto draw                          = mSimulation->GetRandom().Uniform<double>();
            tempCache.mTargetDraw[aTargetName] = draw;
            return draw;
         }
      }
      else
      {
         auto draw                          = mSimulation->GetRandom().Uniform<double>();
         tempCache.mTargetDraw[aTargetName] = draw;
         return draw;
      }
   }
   else
   {
      AttackDistributionCache tempAttackCache;
      mAttackDrawCache[aAttackType]      = tempAttackCache;
      auto& tempCache                    = GetDrawCache(mAttackDrawCache[aAttackType], aType);
      auto  draw                         = mSimulation->GetRandom().Uniform<double>();
      tempCache.mTargetDraw[aTargetName] = draw;
      return draw;
   }
}

// =================================================================================================
//! cINTERVAL_PER_SIMULATION draw method
double RandomManager::pDraw(const std::string& aAttackType, ProbabilityType aType, WsfRandomVariable& aInterval)
{
   if (contains(GetCacheMap(), aAttackType))
   {
      auto& tempCache = GetDrawCache(mAttackDrawCache[aAttackType], aType);
      if (tempCache.mSimulationIntervalDraw.mDraw >= 0)
      {
         auto randomTimeInterval = aInterval.Draw();
         auto elapsedTime        = mSimulation->GetSimTime() - tempCache.mSimulationIntervalDraw.mDrawTime;
         if (elapsedTime >= randomTimeInterval)
         {
            auto draw                                   = mSimulation->GetRandom().Uniform<double>();
            tempCache.mSimulationIntervalDraw.mDrawTime = mSimulation->GetSimTime();
            tempCache.mSimulationIntervalDraw.mDraw     = draw;
            return draw;
         }
         else
         {
            return tempCache.mSimulationIntervalDraw.mDraw;
         }
      }
      else
      {
         auto draw                                   = mSimulation->GetRandom().Uniform<double>();
         tempCache.mSimulationIntervalDraw.mDrawTime = mSimulation->GetSimTime();
         tempCache.mSimulationIntervalDraw.mDraw     = draw;
         return draw;
      }
   }
   else
   {
      AttackDistributionCache tempAttackCache;
      mAttackDrawCache[aAttackType]               = tempAttackCache;
      auto& tempCache                             = GetDrawCache(mAttackDrawCache[aAttackType], aType);
      auto  draw                                  = mSimulation->GetRandom().Uniform<double>();
      tempCache.mSimulationIntervalDraw.mDraw     = draw;
      tempCache.mSimulationIntervalDraw.mDrawTime = mSimulation->GetSimTime();
      return draw;
   }
}

// =================================================================================================
//! cINTERVAL_PER_TARGET draw method
double RandomManager::pDraw(const std::string& aAttackType,
                            const std::string& aTargetName,
                            ProbabilityType    aType,
                            WsfRandomVariable& aInterval)
{
   if (contains(GetCacheMap(), aAttackType))
   {
      auto& tempCache = GetDrawCache(mAttackDrawCache[aAttackType], aType);
      if (contains(tempCache.mTargetIntervalDraw, aTargetName))
      {
         if (tempCache.mTargetIntervalDraw[aTargetName].mDraw >= 0.0)
         {
            auto randomTimeInterval = aInterval.Draw();
            auto elapsedTime        = mSimulation->GetSimTime() - tempCache.mTargetIntervalDraw[aTargetName].mDrawTime;
            if (elapsedTime >= randomTimeInterval)
            {
               auto draw                                            = mSimulation->GetRandom().Uniform<double>();
               tempCache.mTargetIntervalDraw[aTargetName].mDraw     = draw;
               tempCache.mTargetIntervalDraw[aTargetName].mDrawTime = mSimulation->GetSimTime();
               return draw;
            }
            else
            {
               return tempCache.mTargetIntervalDraw[aTargetName].mDraw;
            }
         }
         else
         {
            auto draw                                            = mSimulation->GetRandom().Uniform<double>();
            tempCache.mTargetIntervalDraw[aTargetName].mDraw     = draw;
            tempCache.mTargetIntervalDraw[aTargetName].mDrawTime = mSimulation->GetSimTime();
            return draw;
         }
      }
      else
      {
         DrawData tempData;
         auto     draw                              = mSimulation->GetRandom().Uniform<double>();
         tempData.mDraw                             = draw;
         tempData.mDrawTime                         = mSimulation->GetSimTime();
         tempCache.mTargetIntervalDraw[aTargetName] = tempData;
         return draw;
      }
   }
   else
   {
      AttackDistributionCache tempAttackCache;
      mAttackDrawCache[aAttackType] = tempAttackCache;
      auto&    tempCache            = GetDrawCache(mAttackDrawCache[aAttackType], aType);
      DrawData tempData;
      auto     draw                              = mSimulation->GetRandom().Uniform<double>();
      tempData.mDraw                             = draw;
      tempData.mDrawTime                         = mSimulation->GetSimTime();
      tempCache.mTargetIntervalDraw[aTargetName] = tempData;
      return draw;
   }
}

} // namespace random
} // namespace cyber
} // namespace wsf
