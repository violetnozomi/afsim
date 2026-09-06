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

#include "WsfCyberEngagementManager.hpp"

#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfCyberEffectTypes.hpp"
#include "WsfCyberEvent.hpp"
#include "WsfCyberObserver.hpp"
#include "WsfCyberProtect.hpp"
#include "WsfCyberScenarioExtension.hpp"
#include "WsfCyberSimulationExtension.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

namespace
{
size_t GetKey(const std::string& aAttackType, const std::string& aAttacker, const std::string& aVictim)
{
   auto hash = std::hash<std::string>{}(aAttackType);
   hash ^= std::hash<std::string>{}(aAttacker) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
   hash ^= std::hash<std::string>{}(aVictim) + 0x9e3779b9 + (hash << 6) + (hash >> 2);

   return hash;
}
} // namespace

namespace wsf
{
namespace cyber
{
// =================================================================================================
//! Returns a modifiable instance of the cyber engagement manager
EngagementManager& EngagementManager::Get(WsfSimulation& aSimulation)
{
   return SimulationExtension::Get(aSimulation).GetCyberEngagementManager();
}

// =================================================================================================
EngagementManager::EngagementData& EngagementManager::AddEngagement(const std::string& aAttackType,
                                                                    const std::string& aAttacker,
                                                                    const std::string& aVictim,
                                                                    WsfSimulation&     aSimulation)
{
   auto engagementDataPtr = FindEngagementData(aAttackType, aAttacker, aVictim);
   if (engagementDataPtr)
   {
      return *engagementDataPtr;
   }

   auto key = GetKey(aAttackType, aAttacker, aVictim);

   Engagement     engagement(aAttacker, aVictim, aAttackType, aSimulation, key);
   EngagementData engagementData(std::move(engagement));
   auto           it = mEngagements.emplace(key, std::move(engagementData));

   return it.first->second;
}

// =================================================================================================
Engagement* EngagementManager::FindEngagement(const std::string& aAttackType,
                                              const std::string& aAttacker,
                                              const std::string& aVictim)
{
   return FindEngagement(GetKey(aAttackType, aAttacker, aVictim));
}

// =================================================================================================
Engagement* EngagementManager::FindEngagement(size_t aKey)
{
   auto engagementDataPtr = FindEngagementData(aKey);
   if (engagementDataPtr)
   {
      return &engagementDataPtr->GetEngagement();
   }

   return nullptr;
}

// =================================================================================================
EngagementManager::EngagementMap::iterator EngagementManager::FindEngagementByPlatform(const std::string& aName,
                                                                                       bool               aByVictim)
{
   // Faster to check conditional now than multiple times below.
   if (aByVictim)
   {
      return std::find_if(std::begin(mEngagements),
                          std::end(mEngagements),
                          [aName](EngagementMap::value_type& aData)
                          { return aData.second.GetEngagement().GetVictim() == aName; });
   }

   return std::find_if(std::begin(mEngagements),
                       std::end(mEngagements),
                       [aName](EngagementMap::value_type& aData)
                       { return aData.second.GetEngagement().GetAttacker() == aName; });
}

// =================================================================================================
EngagementManager::EngagementData* EngagementManager::FindEngagementData(const std::string& aAttackType,
                                                                         const std::string& aAttacker,
                                                                         const std::string& aVictim)
{
   return FindEngagementData(GetKey(aAttackType, aAttacker, aVictim));
}

// =================================================================================================
EngagementManager::EngagementData* EngagementManager::FindEngagementData(size_t aKey)
{
   auto it = mEngagements.find(aKey);
   if (it != std::end(mEngagements))
   {
      return &it->second;
   }

   return nullptr;
}

// =================================================================================================
bool EngagementManager::CyberAttack(const std::string& aAttackType,
                                    const std::string& aAttacker,
                                    const std::string& aVictim,
                                    WsfSimulation&     aSimulation,
                                    AttackParameters*  aParameters) // = nullptr
{
   //! Check the attack name for validity
   if (!ScenarioExtension::Get(aSimulation.GetScenario()).GetAttackTypeExists(aAttackType))
   {
      return false;
   }

   //! Check that the target platform exists.
   WsfPlatform* aVictimPlat = aSimulation.GetPlatformByName(aVictim);
   if (!aVictimPlat)
   {
      return false;
   }

   auto curEngagementDataPtr = FindEngagementData(aAttackType, aAttacker, aVictim);
   if (curEngagementDataPtr)
   {
      //! An engagement already exists for this attacker/victim/attack type combination.
      //! If an attack isn't in progress, then proceed with this attack.
      //! Otherwise, an attack is still in progress, and this call attempt will fail.
      if (!curEngagementDataPtr->GetEngagement().GetAttackInProgress())
      {
         if (aParameters)
         {
            curEngagementDataPtr->AddParameters(*aParameters);
         }
         CyberAttackInitialize(*curEngagementDataPtr);
         return true;
      }
      //! An attack was previously requested by this attacker, and the time has not elapsed since
      //! the attack has resolved. We terminate any further processing for this request, and wait
      //! for the attack to resolve. We still return true, however, as the request was valid
      //! and ongoing (from the perspective of the attacker)
      return true;
   }
   //! No engagement object. This is a new engagement. Proceed with algorithm.
   curEngagementDataPtr = &AddEngagement(aAttackType, aAttacker, aVictim, aSimulation);

   if (aParameters)
   {
      curEngagementDataPtr->AddParameters(*aParameters);
   }
   CyberAttackInitialize(*curEngagementDataPtr);
   return true;
}

// =================================================================================================
void EngagementManager::CyberAttackInitialize(EngagementData& aEngagementData)
{
   auto& engagement = aEngagementData.GetEngagement();

   engagement.Reset();

   auto& sim = engagement.GetSimulation();
   engagement.SetAttackStartTime();
   engagement.SetAttackInProgress(true);

   //! Reset the attack failure reason from any previous attempts
   engagement.SetAttackFailureReason(Engagement::cATTACK_NONE);

   //! Notify the observer that an attack has begun
   WsfObserver::CyberAttackInitiated (&sim)(sim.GetSimTime(), engagement);
   VisualizationManager::Get(sim).AttackInitiated(engagement);

   // Add the attack time to the attackers constraint component
   auto constraintComponent = Constraint::Find(*(sim.GetPlatformByName(engagement.GetAttacker())));
   constraintComponent->AddAttackTime(engagement.GetAttackType(), sim.GetSimTime());

   if (engagement.GetDeliveryDelayTime() == 0.0)
   {
      //! No delivery delay. Immediately begin the attack
      CyberAttack(aEngagementData);
      return;
   }
   //! Model a delay for the delivery phase. Schedule the event and return to the
   //! attack algorithm upon completion
   auto event = ut::make_unique<Event>((sim.GetSimTime() + engagement.GetDeliveryDelayTime()),
                                       Event::Type::cATTACK_DELAY,
                                       engagement.GetVictim(),
                                       engagement.GetKey());

   SimulationExtension::Get(sim).GetCyberEventManager().AddEvent(std::move(event));
}

// =================================================================================================
void EngagementManager::CyberAttack(EngagementData& aEngagementData)
{
   auto&  engagement = aEngagementData.GetEngagement();
   auto&  sim        = engagement.GetSimulation();
   double simTime    = sim.GetSimTime();

   // Invoke the user defined script "IsVulnerable" here, if defined
   bool     wasRun     = false;
   Protect* protect    = engagement.GetUsedProtection();
   bool     vulnerable = protect->ExecuteIsVulnerable(engagement, simTime, wasRun);
   if (!vulnerable && wasRun)
   {
      // The attack has failed due to a user defined reason
      engagement.SetAttackInProgress(false);
      engagement.SetAttackSuccess(false);
      engagement.SetAttackFailureReason(Engagement::cATTACK_NOT_VULNERABLE);
      WsfObserver::CyberAttackFailed (&sim)(simTime, engagement);
      VisualizationManager::Get(sim).AttackFailed(engagement);

      return;
   }

   //! The target may be outright immune to the attack due to previous attacks.
   //! In this case, the attack fails.
   if (engagement.IsVictimImmune())
   {
      engagement.SetAttackInProgress(false);
      engagement.SetAttackSuccess(false);
      engagement.SetAttackFailureReason(Engagement::cATTACK_IMMUNITY);

      WsfObserver::CyberAttackFailed (&sim)(simTime, engagement);
      VisualizationManager::Get(sim).AttackFailed(engagement);

      return;
   }

   //! The attack may have some resource requirement associated with it.
   //! If the attacker does not have enough resources the attack fails.
   //! If the attacker already allocated resources the attack fails.
   if (engagement.ExistingConstraintReservations() || !engagement.MeetsAttackerConstraints())
   {
      engagement.SetAttackInProgress(false);
      engagement.SetAttackSuccess(false);
      engagement.SetAttackFailureReason(Engagement::cATTACK_INSUFFICIENT_RESOURCES);
      WsfObserver::CyberAttackFailed (&sim)(simTime, engagement);
      VisualizationManager::Get(sim).AttackFailed(engagement);

      return;
   }

   //! The attack is not blocked and may proceed. Determine attack success by
   //! probabilistic draw
   bool attackSuccess = engagement.Draw(random::cATTACK_SUCCESS);

   if (attackSuccess)
   {
      // The resource reservations should only be attempted after the attack succeeds.
      engagement.MakeConstraintReservations();

      //! Mark the attack as completed and successful
      engagement.SetAttackSuccess(true);

      //! Determine if the outcome of the attack is reported to the attacker.
      engagement.Draw(random::cSTATUS_REPORT);

      //! Activate all of the effects associated with this attack
      CyberAttackEffect(aEngagementData);

      //! Mark the attack as completed and successful
      engagement.SetAttackSuccess(true);
      WsfObserver::CyberAttackSucceeded (&sim)(simTime, engagement);
      VisualizationManager::Get(sim).AttackSucceeded(engagement);

      //! Determine if the victim can detect the attack
      bool attackDetected = engagement.Draw(random::cATTACK_DETECTION);
      if (attackDetected)
      {
         // Schedule the continuation of the attack progression after the defined attack detection delay time
         // If the detection delay time is 0.0, just jump straight to the next method
         double attackDelayTime    = engagement.GetAttackDetectionDelayTime();
         double engagementDuration = engagement.GetDuration();
         if (attackDelayTime == 0.0)
         {
            CyberAttackDetectionDelay(aEngagementData);
         }
         else
         {
            // Schedule event in the future after the completion of the attack delay time
            // but only if Detection Delay is less than the duration of the cyber attack.
            // if Detection Delay is greater than the duration, it will recover first and not be detected
            if (engagementDuration > 0.0 && attackDelayTime > engagementDuration)
            {
               // Time to detect attack is greater than Duration, so attack will finish first

               auto event = ut::make_unique<Event>((simTime + engagementDuration),
                                                   Event::Type::cATTACK_RECOVERY_DELAY,
                                                   engagement.GetVictim(),
                                                   engagement.GetKey());

               SimulationExtension::Get(sim).GetCyberEventManager().AddEvent(std::move(event));
            }
            else
            {
               // detects attack before the cyber attack ends

               auto event = ut::make_unique<Event>((simTime + attackDelayTime),
                                                   Event::Type::cATTACK_DETECTION_DELAY,
                                                   engagement.GetVictim(),
                                                   engagement.GetKey());

               SimulationExtension::Get(sim).GetCyberEventManager().AddEvent(std::move(event));
            }
         }
      }
      else // attack not detected - end of attack progression
      {
         // check for duration. If it exists, schedule a recovery. Otherwise, remove the engagement.
         if (engagement.GetDuration() > 0.0)
         {
            // Schedule a delay event for potential recovery actions
            auto event = ut::make_unique<Event>((simTime + engagement.GetDuration()),
                                                Event::Type::cATTACK_RECOVERY_DELAY,
                                                engagement.GetVictim(),
                                                engagement.GetKey());

            SimulationExtension::Get(sim).GetCyberEventManager().AddEvent(std::move(event));
         }
         else
         {
            // No chance of a recovery, so we no longer have use of the instantiated effects for this
            // engagement. Get rid of them at this point
            aEngagementData.RemoveEffects();
            // No further processing or delays. Set end of engagement for reuse.
            engagement.SetAttackInProgress(false);
         }
      }
   }
   else
   {
      engagement.SetAttackSuccess(false);
      engagement.SetAttackInProgress(false);
      engagement.SetAttackFailureReason(Engagement::cATTACK_RANDOM_DRAW);

      //! Determine if the outcome of the attack is reported to the attacker.
      engagement.Draw(random::cSTATUS_REPORT);
      WsfObserver::CyberAttackFailed (&sim)(simTime, engagement);
      VisualizationManager::Get(sim).AttackFailed(engagement);
   }
}

// =================================================================================================
void EngagementManager::CyberAttackEffect(EngagementData& aEngagementData)
{
   auto&  engagement = aEngagementData.GetEngagement();
   auto&  sim        = engagement.GetSimulation();
   double simTime    = sim.GetSimTime();

   //! Add effects and kick off each effect
   const auto& effectList = engagement.GetAttackEffects();
   for (const auto& effect : effectList)
   {
      auto& addedEffect = aEngagementData.AddEffect(effect);
      addedEffect.Attack(simTime, engagement);
   }
}

// =================================================================================================
void EngagementManager::CyberAttackDetectionDelay(EngagementData& aEngagementData)
{
   auto&  engagement = aEngagementData.GetEngagement();
   auto&  sim        = engagement.GetSimulation();
   double simTime    = sim.GetSimTime();

   // Notify the observer that the attack has been detected
   WsfObserver::CyberAttackDetected (&sim)(simTime, engagement);

   // Update detection time
   engagement.SetTimeAttackDiscovered();

   //! Determine if the victim can attribute the attack to the attacking platform
   bool attackAttributed = engagement.Draw(random::cATTACK_ATTRIBUTION);
   if (attackAttributed)
   {
      // Notify the observer that the attack has been attributed
      WsfObserver::CyberAttackAttributed (&sim)(simTime, engagement);
   }

   // Invoke the user defined script "OnAttackDetection" here, if defined
   Protect* protect = engagement.GetUsedProtection();
   protect->ExecuteOnAttackDetection(engagement, simTime);

   double recoveryDelayTime = engagement.GetAttackRecoveryDelayTime();
   if (recoveryDelayTime == 0.0)
   {
      // No delay. Immediately call next attack progression method
      CyberAttackRecoveryDelay(aEngagementData);
   }
   else
   {
      // Schedule a delay event for potential recovery actions
      // Schedule it for attack recovery delay, or end duration, whichever comes first.
      double attackTimeLeft = engagement.GetDuration() - (simTime - engagement.GetAttackStartTime());
      if (engagement.GetDuration() > 0.0 && attackTimeLeft < recoveryDelayTime)
      {
         assert(attackTimeLeft > 0.0);
         auto event = ut::make_unique<Event>((simTime + attackTimeLeft),
                                             Event::Type::cATTACK_RECOVERY_DELAY,
                                             engagement.GetVictim(),
                                             engagement.GetKey());

         SimulationExtension::Get(sim).GetCyberEventManager().AddEvent(std::move(event));
      }
      else
      {
         auto event = ut::make_unique<Event>((simTime + recoveryDelayTime),
                                             Event::Type::cATTACK_RECOVERY_DELAY,
                                             engagement.GetVictim(),
                                             engagement.GetKey());

         SimulationExtension::Get(sim).GetCyberEventManager().AddEvent(std::move(event));
      }
   }
}

// =================================================================================================
void EngagementManager::CyberAttackRecoveryDelay(EngagementData& aEngagementData)
{
   auto&  engagement = aEngagementData.GetEngagement();
   auto&  sim        = engagement.GetSimulation();
   double simTime    = sim.GetSimTime();

   // Invoke the user defined script "OnAttackRecovery" here, if defined
   auto* protect = engagement.GetUsedProtection();
   protect->ExecuteOnAttackRecovery(engagement, simTime);

   if (engagement.GetRecovery() || engagement.GetDuration() > 0.0)
   {
      // Invoke the recovery method for each effect associated with this attack
      const auto& effectList = engagement.GetAttackEffects();
      for (const auto& effect : effectList)
      {
         auto effectPtr = aEngagementData.GetEffect(effect);
         if (effectPtr)
         {
            effectPtr->Restore(simTime, engagement);
         }
         else
         {
            auto logError = ut::log::error() << "Unable to reference cyber effect.";
            logError.AddNote() << "Effect: " << effect;
            throw std::runtime_error("Error in expected GetEffect() during cyber manager attack restore.");
         }
      }

      // Set the time that recovery occurred
      engagement.SetTimeAttackRecovery();

      // After recovery, the associated events with this engagement are no longer needed.
      // Purge them from our maintained effects
      aEngagementData.RemoveEffects();

      // Only check immunity if the duration was larger or equal to the delay time to detect.
      // Also check immunity if the duration was not set.
      if (engagement.GetDuration() <= 0.0 || engagement.GetDuration() >= engagement.GetAttackDetectionDelayTime())
      {
         // Check for future immunity against this attack type
         // Note - this draw call will automatically update the victim platform
         // immunity if the draw is successful
         engagement.Draw(random::cFUTURE_IMMUNITY);
      }

      // Notify the observer that the recovery step has been reached
      WsfObserver::CyberAttackRecovery (&sim)(simTime, engagement);
   }

   // The attack progression has ended for this attack iteration.
   engagement.SetAttackInProgress(false);
}

// =================================================================================================
bool EngagementManager::CyberScan(const std::string& aAttackType,
                                  const std::string& aAttacker,
                                  const std::string& aVictim,
                                  WsfSimulation&     aSimulation)
{
   //! Check the attack name for validity
   if (!ScenarioExtension::Get(aSimulation.GetScenario()).GetAttackTypeExists(aAttackType))
   {
      return false;
   }

   //! Check that the target platform exists.
   WsfPlatform* aVictimPlat = aSimulation.GetPlatformByName(aVictim);
   if (!aVictimPlat)
   {
      return false;
   }

   auto* curEngagementDataPtr = FindEngagementData(aAttackType, aAttacker, aVictim);
   if (curEngagementDataPtr)
   {
      auto& engagement = curEngagementDataPtr->GetEngagement();
      // If any attacks are currently in progress, exit
      if (engagement.GetAttackInProgress())
      {
         return true;
      }

      //! An engagement already exists for this attacker/victim/attack type combination.
      //! If a scan hasn't been attempted, or has and is now expired, then proceed with this scan.
      //! Otherwise, a scan is still in progress, and this call attempt will fail.
      //! Note: Clang warns about the comparison of floating point values here. This is, however, safe
      //! given the use of an internally consistent value for comparison that is not the result of
      //! computation and potential loss of precision.
      bool   neverScanned    = (engagement.GetScanStartTime() == std::numeric_limits<double>::max());
      double elapsedScanTime = aSimulation.GetSimTime() - engagement.GetScanStartTime();
      if (neverScanned || (elapsedScanTime > engagement.GetScanDelayTime()))
      {
         //! No concurrent scan. Proceed with scan
         CyberScanInitialize(*curEngagementDataPtr);
         return true;
      }
      //! A scan was previously requested by this attacker, and the time has not elapsed since
      //! the scan has resolved. We terminate any further processing for this request, and wait
      //! for the scheduled event to resolve. We still return true, however, as the request was valid
      //! and ongoing (from the perspective of the attacker)
      return true;
   }
   //! No engagement exists. Create it, and begin scan
   curEngagementDataPtr = &AddEngagement(aAttackType, aAttacker, aVictim, aSimulation);
   CyberScanInitialize(*curEngagementDataPtr);
   return true;
}

// =================================================================================================
bool EngagementManager::Cancel(size_t aKey)
{
   auto curEngagementDataPtr = FindEngagementData(aKey);
   if (!curEngagementDataPtr)
   {
      return false;
   }

   auto& engagement = curEngagementDataPtr->GetEngagement();
   engagement.SetAttackInProgress(false);
   curEngagementDataPtr->RemoveEffects();

   return true;
}

// =================================================================================================
void EngagementManager::CyberScanInitialize(EngagementData& aEngagementData)
{
   auto& engagement = aEngagementData.GetEngagement();
   auto& sim        = engagement.GetSimulation();
   engagement.Reset(true);

   //! Notify the observer that a scan has begun
   WsfObserver::CyberScanInitiated (&sim)(sim.GetSimTime(), engagement);
   VisualizationManager::Get(sim).ScanInitiated(engagement);
   engagement.SetScanStartTime();

   //! Reset the scan failure reason from any previous attempts
   engagement.SetScanFailureReason(Engagement::cSCAN_NONE);

   if (engagement.GetScanDelayTime() == 0.0)
   {
      //! No delay. Immediately begin the scanning algorithm and record results in the engagement object
      CyberScan(aEngagementData);
   }
   else
   {
      //! A delay is required. Schedule the event with the simulation to execute the scanning algorithm
      //! at the appropriate time
      auto event = ut::make_unique<Event>((sim.GetSimTime() + engagement.GetScanDelayTime()),
                                          Event::Type::cSCAN_DELAY,
                                          engagement.GetVictim(),
                                          engagement.GetKey());

      SimulationExtension::Get(sim).GetCyberEventManager().AddEvent(std::move(event));
   }
}

// =================================================================================================
void EngagementManager::CyberScan(EngagementData& aEngagementData)
{
   auto&  engagement = aEngagementData.GetEngagement();
   auto&  sim        = engagement.GetSimulation();
   double simTime    = sim.GetSimTime();

   // Invoke the user defined script "IsVulnerable" here, if defined
   bool  wasRun     = false;
   auto* protect    = engagement.GetUsedProtection();
   bool  vulnerable = protect->ExecuteIsVulnerable(engagement, simTime, wasRun);
   if (!vulnerable && wasRun)
   {
      // The scan has failed due to a user defined reason
      engagement.SetScanFailureReason(Engagement::cSCAN_NOT_VULNERABLE);
      WsfObserver::CyberScanFailed (&sim)(simTime, engagement);
      VisualizationManager::Get(sim).ScanFailed(engagement);
      return;
   }

   //! Determine if the victim is immune to this attack type
   if (engagement.IsVictimImmune())
   {
      engagement.SetScanFailureReason(Engagement::cSCAN_IMMUNITY);
      WsfObserver::CyberScanFailed (&sim)(simTime, engagement);
      VisualizationManager::Get(sim).ScanFailed(engagement);
      return;
   }

   //! Determine if the victim detects the scan
   bool scanDetected = engagement.Draw(random::cSCAN_DETECTION);
   if (scanDetected)
   {
      //! If the scan is detected, it fails
      engagement.SetScanFailureReason(Engagement::cSCAN_DETECTED);
      WsfObserver::CyberScanFailed (&sim)(simTime, engagement);
      WsfObserver::CyberScanDetected (&sim)(simTime, engagement);

      //! Determine if the victim attributes the attacking platform
      bool scanAttributed = engagement.Draw(random::cSCAN_ATTRIBUTION);
      if (scanAttributed)
      {
         WsfObserver::CyberScanAttributed (&sim)(simTime, engagement);
      }

      VisualizationManager::Get(sim).ScanFailed(engagement);

      // Invoke the user defined script "OnScanDetection" here, if defined
      protect->ExecuteOnScanDetection(engagement, simTime);
      return;
   }

   //! At this point, the scan is now successful.
   engagement.SetScanSuccess(true);
   WsfObserver::CyberScanSucceeded (&sim)(simTime, engagement);
   VisualizationManager::Get(sim).ScanSucceeded(engagement);
}

// =================================================================================================
void EngagementManager::CullVictimEngagements(const std::string& aVictim)
{
   auto it = FindEngagementByPlatform(aVictim, true);
   if (it != std::end(mEngagements))
   {
      mEngagements.erase(it);
   }
}

// =================================================================================================
void EngagementManager::CullAttackerEngagements(const std::string& aAttacker)
{
   auto it = FindEngagementByPlatform(aAttacker, false);
   if (it != std::end(mEngagements))
   {
      mEngagements.erase(it);
   }
}

// =================================================================================================
bool EngagementManager::EngagementExists(const std::string& aAttackType,
                                         const std::string& aAttacker,
                                         const std::string& aVictim) const
{
   return EngagementExists(GetKey(aAttackType, aAttacker, aVictim));
}

// =================================================================================================
bool EngagementManager::EngagementExists(size_t aKey) const
{
   return (mEngagements.find(aKey) != std::end(mEngagements));
}

// =================================================================================================
Effect* EngagementManager::EngagementData::GetEffect(const std::string& aEffectName) const
{
   auto it =
      std::find_if(std::begin(mEngagementEffects),
                   std::end(mEngagementEffects),
                   [aEffectName](const UtCloneablePtr<Effect>& aEffect) { return (aEffect->GetType() == aEffectName); });

   if (it != std::end(mEngagementEffects))
   {
      return it->get();
   }

   return nullptr;
}

// =================================================================================================
Effect& EngagementManager::EngagementData::AddEffect(const std::string& aEffectName)
{
   auto&                  scenario = ScenarioExtension::Get(mEngagement.GetSimulation().GetScenario());
   UtCloneablePtr<Effect> effectPtr(scenario.GetEffectTypes().Clone(aEffectName));
   if (!effectPtr)
   {
      auto out = ut::log::error() << "Attempting to instantiate a non-existent cyber effect.";
      out.AddNote() << "Effect: " << aEffectName;
      throw std::runtime_error("Error in effect instantiation in wsf::cyber::EngagementManager");
   }

   if (effectPtr->RequiredInput() == Effect::InputRequirement::cREQUIRED)
   {
      if (!mParametersValid)
      {
         ut::log::error() << "Illegal attempt to instantiate effect without valid parameters.";
         throw std::runtime_error(
            "Error in effect instantiation requiring valid parameters in wsf::cyber::EngagementManager");
      }

      effectPtr = effectPtr->Clone(mParameters);
   }
   else if (mParametersValid) // optional user data required
   {
      effectPtr = effectPtr->Clone(mParameters);
   }

   mEngagementEffects.push_back(effectPtr);

   // Must initialize AFTER adding to container - because WsfScriptContext has a non-semantically correct
   // copy constructor, we need to avoid copies after it has been initialized. This also extends to why
   // we store this in a std::list as opposed to std::vector, to avoid copy constructor calls due to
   // automatic reallocation of the container when using std::vector.
   mEngagementEffects.back()->Initialize(mEngagement);
   return *mEngagementEffects.back().get();
}

// =================================================================================================
void EngagementManager::EngagementData::RemoveEffect(const std::string& aEffectName)
{
   auto it =
      std::find_if(std::begin(mEngagementEffects),
                   std::end(mEngagementEffects),
                   [aEffectName](const UtCloneablePtr<Effect>& aEffect) { return (aEffect->GetType() == aEffectName); });

   if (it != std::end(mEngagementEffects))
   {
      mEngagementEffects.erase(it);
   }
}

// =================================================================================================
void EngagementManager::EngagementData::AddParameters(const AttackParameters& aParameters)
{
   mParameters      = aParameters;
   mParametersValid = true;
}

// =================================================================================================
void EngagementManager::EngagementData::RemoveParameters()
{
   // Lazy delete
   mParametersValid = false;
}

// =================================================================================================
void EngagementManager::EngagementData::RemoveEffects()
{
   mEngagementEffects.clear();
   RemoveParameters();
}

} // namespace cyber
} // namespace wsf
