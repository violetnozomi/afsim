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


#include "WsfCyberEngagement.hpp"

#include "UtMemory.hpp"
#include "WsfCyberAttack.hpp"
#include "WsfCyberAttackTypes.hpp"
#include "WsfCyberConstraint.hpp"
#include "WsfCyberEngagement.hpp"
#include "WsfCyberProtect.hpp"
#include "WsfCyberProtectTypes.hpp"
#include "WsfCyberScenarioExtension.hpp"
#include "WsfCyberSimulationExtension.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

// To avoid linux release error, we must include this macro to indicate a variable is used for assertion
#define _unused(x) ((void)(x))

namespace wsf
{
namespace cyber
{
// =================================================================================================
Engagement::Engagement(std::string    aAttackingPlatform,
                       std::string    aVictimPlatform,
                       std::string    aNamedAttack,
                       WsfSimulation& aSimulation,
                       size_t         aKey)
   : mAttacker(std::move(aAttackingPlatform))
   , mVictim(std::move(aVictimPlatform))
   , mNamedAttack(std::move(aNamedAttack))
   , mKey{aKey}
   , mSimulation(aSimulation)
{
   SetDefiningCyberObject();
   SetInitialValues();

   auto attackerPtr = mSimulation.GetPlatformByName(mAttacker);
   if (attackerPtr)
   {
      mAttackerIndex = attackerPtr->GetIndex();
   }

   auto victimPtr = mSimulation.GetPlatformByName(mVictim);
   if (victimPtr)
   {
      mVictimIndex = victimPtr->GetIndex();
   }
}

bool Engagement::GetStatusReportSuccess() const
{
   if (mStatusReportDraw < 0)
   {
      return false;
   }
   assert(mStatusReportThreshold >= 0);
   return mStatusReportDraw <= mStatusReportThreshold;
}

Engagement::~Engagement()
{
   ReleaseAttackerConstraints();
}

// =================================================================================================
void Engagement::SetDefiningCyberObject()
{
   const auto& cyberScenario = ScenarioExtension::Get(mSimulation.GetScenario());

   //! Get the corresponding WsfCyberProtect object for this engagement
   //! (at the current time. may become a pointer to a parent class type)
   WsfPlatform* victimPtr   = mSimulation.GetPlatformByName(mVictim);
   WsfPlatform* attackerPtr = mSimulation.GetPlatformByName(mAttacker);

   if (!victimPtr)
   {
      throw UtException("WsfCyberEngagement could not find a valid target platform.");
   }

   mProtectTypePtr = victimPtr->GetComponent<Protect>();

   if (!mProtectTypePtr)
   {
      throw UtException("WsfCyberEngagement could not find a valid target platform WsfCyberProtect object.");
   }

   mConstraintPtr = attackerPtr->GetComponent<Constraint>();

   if (!mConstraintPtr)
   {
      throw UtException("WsfCyberEngagement could not find a valid constraint platform WsfCyberConstraint object.");
   }

   //! Get the corresponding WsfCyberAttack object for this engagement
   auto attackPtr = cyberScenario.GetAttackTypes().Find(mNamedAttack);

   if (attackPtr)
   {
      mAttackPtr = ut::clone(attackPtr);
   }
   else
   {
      throw UtException("WsfCyberEngagement could not find a valid WsfCyberAttack.");
   }

   //! The agreed upon algorithm for determining precedence for which object defines the engagement
   //! parameters works in the following method:
   //! 1. If the corresponding attack is named explicitly in the victim protection, use the victim's
   //!    protection parameters
   //! 2. If the corresponding attack is named explicitly in any of the protections inherited from, use that
   //!    protection object's parameters
   //! 3. If a default protection is defined on the victim, use these parameters
   //! 4. If all of the above fails, use the parameters defined on the attack (guaranteed to exist)

   //! Check for case 1, return if found
   if (mProtectTypePtr->ProtectionExists(mNamedAttack))
   {
      mUseProtectDefinition = true;
      return;
   }

   //! Check for case 2, return if found
   auto parentTypes = mProtectTypePtr->GetTypeList();
   for (const auto& parentType : parentTypes)
   {
      auto* curBase = cyberScenario.GetProtectTypes().Find(parentType);
      if (curBase && curBase->ProtectionExists(mNamedAttack))
      {
         mProtectTypePtr       = curBase;
         mUseProtectDefinition = true;
         return;
      }
   }

   //! Check for case 3, return if found
   if (mProtectTypePtr->DefaultExists())
   {
      mUseProtectDefinition = true;
   }

   //! Case 4, all of the above failed, use the attack object
}

// =================================================================================================
void Engagement::SetInitialValues()
{
   if (!mProtectTypePtr)
   {
      throw UtException("Protect object must be a valid reference for WsfCyberEnagement value initialization.");
   }

   if (mUseProtectDefinition)
   {
      mStatusReportThreshold    = mProtectTypePtr->GetProbabilityOfStatusReport(mNamedAttack);
      mAttackDetectionThreshold = mProtectTypePtr->GetProbabilityOfAttackDetection(mNamedAttack);
      mScanDetectionThreshold   = mProtectTypePtr->GetProbabilityOfScanDetection(mNamedAttack);
      mAttackSuccessThreshold   = mProtectTypePtr->GetProbabilityOfAttackSuccess(mNamedAttack);
      mImmunityThreshold        = mProtectTypePtr->GetProbabilityOfFutureImmunity(mNamedAttack);
   }
   else
   {
      mStatusReportThreshold      = mAttackPtr->GetProbabilityOfStatusReport();
      mAttackDetectionThreshold   = mAttackPtr->GetProbabilityOfAttackDetection();
      mAttackAttributionThreshold = mAttackPtr->GetProbabilityOfAttackAttribution();
      mScanDetectionThreshold     = mAttackPtr->GetProbabilityOfScanDetection();
      mScanAttributionThreshold   = mAttackPtr->GetProbabilityOfScanAttribution();
      mAttackSuccessThreshold     = mAttackPtr->GetProbabilityOfAttackSuccess();
      mImmunityThreshold          = mAttackPtr->GetProbabilityOfFutureImmunity();
   }

   mTimeScanDelay            = mAttackPtr->GetTimeDelayScan();
   mTimeDeliveryDelay        = mAttackPtr->GetTimeDelayDelivery();
   mAttackEffectTypes        = mAttackPtr->GetEffects();
   mTimeAttackDetectionDelay = mProtectTypePtr->DetectionDelayTime(mNamedAttack);
   mTimeAttackRecoveryDelay  = mProtectTypePtr->RecoveryDelayTime(mNamedAttack);
   mRecover                  = mProtectTypePtr->DoRestore(mNamedAttack);
   mDuration                 = mAttackPtr->GetDuration();
}

// =================================================================================================
bool Engagement::IsVictimImmune() const
{
   // Pointer validity checked in constructor - only here to ensure software assurance checks pass
   if (mProtectTypePtr)
   {
      return mProtectTypePtr->IsImmune(mNamedAttack);
   }
   return false;
}

// =================================================================================================
void Engagement::SetAttackStartTime()
{
   mTimeAttackStart = mSimulation.GetSimTime();
}

// =================================================================================================
void Engagement::SetScanStartTime()
{
   mTimeScanStart = mSimulation.GetSimTime();
}

// =================================================================================================
void Engagement::SetTimeAttackDiscovered()
{
   mTimeAttackDetection = mSimulation.GetSimTime();
}

// =================================================================================================
void Engagement::SetTimeAttackRecovery()
{
   mTimeAttackRecovery = mSimulation.GetSimTime();
}

// =================================================================================================
bool Engagement::Draw(random::ProbabilityType aProbabilityType)
{
   auto& drawManager = SimulationExtension::Get(mSimulation).GetCyberDrawManager();

   if (aProbabilityType == random::cSCAN_DETECTION)
   {
      mScanDetectionDraw = drawManager.Draw(mNamedAttack, mVictim, aProbabilityType);
      if (mScanDetectionDraw <= mScanDetectionThreshold)
      {
         return true;
      }
      return false;
   }
   else if (aProbabilityType == random::cSCAN_ATTRIBUTION)
   {
      mScanAttributionDraw = drawManager.Draw(mNamedAttack, mVictim, aProbabilityType);
      if (mScanAttributionDraw <= mScanAttributionThreshold)
      {
         return true;
      }
      return false;
   }
   else if (aProbabilityType == random::cATTACK_SUCCESS)
   {
      mAttackDraw = drawManager.Draw(mNamedAttack, mVictim, aProbabilityType);
      if (mAttackDraw <= mAttackSuccessThreshold)
      {
         return true;
      }
      return false;
   }
   else if (aProbabilityType == random::cSTATUS_REPORT)
   {
      mStatusReportDraw = drawManager.Draw(mNamedAttack, mVictim, aProbabilityType);
      if (mStatusReportDraw <= mStatusReportThreshold)
      {
         return true;
      }
      return false;
   }
   else if (aProbabilityType == random::cATTACK_DETECTION)
   {
      mAttackDetectionDraw = drawManager.Draw(mNamedAttack, mVictim, aProbabilityType);
      if (mAttackDetectionDraw <= mAttackDetectionThreshold)
      {
         return true;
      }
      return false;
   }
   else if (aProbabilityType == random::cATTACK_ATTRIBUTION)
   {
      mAttackAttributionDraw = drawManager.Draw(mNamedAttack, mVictim, aProbabilityType);
      if (mAttackAttributionDraw <= mAttackAttributionDraw)
      {
         return true;
      }
      return false;
   }
   else if (aProbabilityType == random::cFUTURE_IMMUNITY)
   {
      mImmunityDraw = drawManager.Draw(mNamedAttack, mVictim, aProbabilityType);
      if (mImmunityDraw <= mImmunityThreshold)
      {
         //! This is a special case. The victim/target has become immune to this attack.
         //! Since multiple engagements exist, this data must be pushed back to the platform component
         //! cyber_protection object itself, so that if other engagements (from potentially other attackers
         //! against this target using the same attack) register the immunity when queried.
         //! NOTE: We cannot naively use the mProtectTypePtr to register the immunity. This may
         //! point to a "parent" cyber_protection, and not the component on the target itself
         auto victimPtr = mSimulation.GetPlatformByName(mVictim);
         if (!victimPtr)
         {
            return false;
         }

         victimPtr->GetComponent<Protect>()->SetImmune(mNamedAttack);
         return true;
      }
      return false;
   }

   // Should never reach this point
   throw UtException("Unexpected conditional branch in WsfCyberEngagment::Draw.");
}

// =================================================================================================
void Engagement::Reset(bool aScanReset) // bool aScanReset = false
{
   if (aScanReset)
   {
      mTimeScanStart       = std::numeric_limits<double>::max();
      mScanDetectionDraw   = -1.0;
      mScanAttributionDraw = -1.0;
      mScanFailure         = cSCAN_NONE;
      mScanSuccess         = false;
   }
   else
   {
      mTimeAttackStart       = std::numeric_limits<double>::max();
      mAttackDraw            = -1.0;
      mStatusReportDraw      = -1.0;
      mTimeAttackDetection   = -1.0;
      mAttackDetectionDraw   = -1.0;
      mAttackAttributionDraw = -1.0;
      mTimeAttackRecovery    = -1.0;
      mAttackFailure         = cATTACK_NONE;
      mAttackSuccess         = false;
      mImmunityDraw          = -1.0;
   }
}

// =================================================================================================
void Engagement::ReleaseAttackerConstraints()
{
   auto platform = mSimulation.GetPlatformByName(mAttacker);

   // If the platform is destroyed there is no need to restore its resources.
   if (platform)
   {
      auto platformCyberConstraint = Constraint::Find(*platform);

      if (platformCyberConstraint)
      {
         // Give the attacking platform back its cyber resources.
         bool valid = platformCyberConstraint->RestoreResources(mCyberResourceUsage);
         assert(valid);
         _unused(valid); // indicate that valid is unused in Release

         mCyberResourceUsage = 0.0;

         platformCyberConstraint->RemoveConcurrentAttack(mNamedAttack, mKey);
      }
   }
}

// =================================================================================================
bool Engagement::MeetsAttackerConstraints()
{
   // Get platform resources
   auto   platformCyberConstraint = Constraint::Find(*(GetSimulation().GetPlatformByName(mAttacker)));
   double attackerResources       = platformCyberConstraint->GetCurrentResources();

   auto cyberScenario = ScenarioExtension::Get(SimulationExtension::Get(GetSimulation()).GetScenario());
   auto attackType    = cyberScenario.GetAttackTypes().Find(mNamedAttack);

   return (attackerResources >= attackType->GetResourceRequirements());
}

// =================================================================================================
bool Engagement::MakeConstraintReservations()
{
   // Get platform resources
   auto   platformCyberConstraint = Constraint::Find(*(GetSimulation().GetPlatformByName(mAttacker)));
   double attackerResources       = platformCyberConstraint->GetCurrentResources();

   auto cyberScenario    = ScenarioExtension::Get(SimulationExtension::Get(GetSimulation()).GetScenario());
   auto attackType       = cyberScenario.GetAttackTypes().Find(mNamedAttack);
   auto resourceRequired = attackType->GetResourceRequirements();

   bool isSufficientResources = attackerResources >= resourceRequired;

   if (isSufficientResources)
   {
      platformCyberConstraint->RemoveResources(resourceRequired);
      mCyberResourceUsage = resourceRequired;

      platformCyberConstraint->AddConcurrentAttack(mNamedAttack, mKey);
   }

   return isSufficientResources;
}

// =================================================================================================
bool Engagement::ExistingConstraintReservations()
{
   return (mCyberResourceUsage > 0.0);
}

// =================================================================================================
ScriptEngagement::ScriptEngagement(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfCyberEngagement");
   mCloneable          = false;
   mIsScriptAccessible = true;

   //! Identifier script methods
   AddMethod(ut::make_unique<Attacker>());
   AddMethod(ut::make_unique<AttackerPlatform>());
   AddMethod(ut::make_unique<AttackType>());
   AddMethod(ut::make_unique<Victim>());
   AddMethod(ut::make_unique<VictimPlatform>());

   //! Attack script methods
   AddMethod(ut::make_unique<TimeAttackInitiated>());
   AddMethod(ut::make_unique<AttackSuccessThreshold>());
   AddMethod(ut::make_unique<AttackSuccessDraw>());
   AddMethod(ut::make_unique<StatusReportThreshold>());
   AddMethod(ut::make_unique<StatusReportDraw>());
   AddMethod(ut::make_unique<TimeAttackDiscovered>());
   AddMethod(ut::make_unique<AttackDetectionThreshold>());
   AddMethod(ut::make_unique<AttackDetectionDraw>());
   AddMethod(ut::make_unique<TimeAttackRecovery>());
   AddMethod(ut::make_unique<Recovery>());
   AddMethod(ut::make_unique<AttackDeliveryDelayTime>());
   AddMethod(ut::make_unique<AttackDetectionDelayTime>());
   AddMethod(ut::make_unique<AttackRecoveryDelayTime>());
   AddMethod(ut::make_unique<AttackSuccess>());
   AddMethod(ut::make_unique<AttackInProgress>());
   AddMethod(ut::make_unique<AttackFailureReason>());

   //! Scan script methods
   AddMethod(ut::make_unique<TimeScanInitiated>());
   AddMethod(ut::make_unique<ScanDetectionThreshold>());
   AddMethod(ut::make_unique<ScanDetectionDraw>());
   AddMethod(ut::make_unique<ScanDelayTime>());
   AddMethod(ut::make_unique<ScanSuccess>());
   AddMethod(ut::make_unique<ScanInProgress>());
   AddMethod(ut::make_unique<ScanFailureReason>());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, Attacker, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetAttacker());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, AttackerPlatform, 0, "WsfPlatform", "")
{
   auto platformPtr = WsfScriptContext::GetSIMULATION(aContext)->GetPlatformByName(aObjectPtr->GetAttacker());
   aReturnVal.SetPointer(UtScriptRef::Ref(platformPtr, aReturnClassPtr));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, AttackType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetAttackType());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, Victim, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetVictim());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, VictimPlatform, 0, "WsfPlatform", "")
{
   auto platformPtr = aObjectPtr->GetSimulation().GetPlatformByName(aObjectPtr->GetVictim());
   aReturnVal.SetPointer(UtScriptRef::Ref(platformPtr, aReturnClassPtr));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, TimeAttackInitiated, 0, "double", "")
{
   auto time = aObjectPtr->GetAttackStartTime();
   aReturnVal.SetDouble(time);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, AttackSuccessThreshold, 0, "double", "")
{
   auto threshold = aObjectPtr->GetAttackSuccessThreshold();
   aReturnVal.SetDouble(threshold);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, AttackSuccessDraw, 0, "double", "")
{
   auto draw = aObjectPtr->GetAttackDraw();
   aReturnVal.SetDouble(draw);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, StatusReportThreshold, 0, "double", "")
{
   auto threshold = aObjectPtr->GetStatusReportThreshold();
   aReturnVal.SetDouble(threshold);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, StatusReportDraw, 0, "double", "")
{
   auto draw = aObjectPtr->GetStatusReportDraw();
   aReturnVal.SetDouble(draw);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, TimeAttackDiscovered, 0, "double", "")
{
   auto time = aObjectPtr->GetTimeAttackDiscovered();
   aReturnVal.SetDouble(time);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, AttackDetectionThreshold, 0, "double", "")
{
   auto threshold = aObjectPtr->GetAttackDetectionThreshold();
   aReturnVal.SetDouble(threshold);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, AttackDetectionDraw, 0, "double", "")
{
   auto draw = aObjectPtr->GetAttackDetectionDraw();
   aReturnVal.SetDouble(draw);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, AttackAttributionThreshold, 0, "double", "")
{
   auto threshold = aObjectPtr->GetAttackAttributionThreshold();
   aReturnVal.SetDouble(threshold);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, AttackAttributionDraw, 0, "double", "")
{
   auto draw = aObjectPtr->GetAttackAttributionDraw();
   aReturnVal.SetDouble(draw);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, TimeAttackRecovery, 0, "double", "")
{
   auto time = aObjectPtr->GetTimeAttackRecovery();
   aReturnVal.SetDouble(time);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, Recovery, 0, "bool", "")
{
   auto recovery = aObjectPtr->GetRecovery();
   aReturnVal.SetBool(recovery);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, TimeScanInitiated, 0, "double", "")
{
   auto time = aObjectPtr->GetScanStartTime();
   aReturnVal.SetDouble(time);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, ScanDetectionThreshold, 0, "double", "")
{
   auto threshold = aObjectPtr->GetScanDetectionThreshold();
   aReturnVal.SetDouble(threshold);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, ScanDetectionDraw, 0, "double", "")
{
   auto draw = aObjectPtr->GetScanDetectionDraw();
   aReturnVal.SetDouble(draw);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, ScanAttributionThreshold, 0, "double", "")
{
   auto threshold = aObjectPtr->GetScanAttributionThreshold();
   aReturnVal.SetDouble(threshold);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, ScanAttributionDraw, 0, "double", "")
{
   auto draw = aObjectPtr->GetScanAttributionDraw();
   aReturnVal.SetDouble(draw);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, ScanDelayTime, 0, "double", "")
{
   auto time = aObjectPtr->GetScanDelayTime();
   aReturnVal.SetDouble(time);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, AttackDeliveryDelayTime, 0, "double", "")
{
   auto time = aObjectPtr->GetDeliveryDelayTime();
   aReturnVal.SetDouble(time);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, AttackDetectionDelayTime, 0, "double", "")
{
   auto time = aObjectPtr->GetAttackDetectionDelayTime();
   aReturnVal.SetDouble(time);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, AttackRecoveryDelayTime, 0, "double", "")
{
   auto time = aObjectPtr->GetAttackRecoveryDelayTime();
   aReturnVal.SetDouble(time);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, ScanSuccess, 0, "bool", "")
{
   auto success = aObjectPtr->GetScanSuccess();
   aReturnVal.SetBool(success);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, AttackSuccess, 0, "bool", "")
{
   auto success = aObjectPtr->GetAttackSuccess();
   aReturnVal.SetBool(success);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, AttackInProgress, 0, "bool", "")
{
   auto success = aObjectPtr->GetAttackInProgress();
   aReturnVal.SetBool(success);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, ScanInProgress, 0, "bool", "")
{
   auto inProgress  = true;
   auto elapsedTime = aObjectPtr->GetSimulation().GetSimTime() - aObjectPtr->GetScanStartTime();
   if (elapsedTime > aObjectPtr->GetScanDelayTime())
   {
      inProgress = false;
   }
   aReturnVal.SetBool(inProgress);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, ScanFailureReason, 0, "int", "")
{
   auto reason = aObjectPtr->GetScanFailureReason();
   aReturnVal.SetInt(reason);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptEngagement, Engagement, AttackFailureReason, 0, "int", "")
{
   auto reason = aObjectPtr->GetAttackFailureReason();
   aReturnVal.SetInt(reason);
}

} // namespace cyber
} // namespace wsf
