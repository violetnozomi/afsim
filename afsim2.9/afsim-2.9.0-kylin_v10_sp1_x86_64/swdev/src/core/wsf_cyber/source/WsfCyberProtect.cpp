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

#include "WsfCyberProtect.hpp"

#include <sstream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtScript.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfCyberAttackTypes.hpp"
#include "WsfCyberEngagement.hpp"
#include "WsfCyberProtectTypes.hpp"
#include "WsfCyberScenarioExtension.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"

namespace wsf
{
namespace cyber
{

namespace
{

//! Component factory to process platform input.
class CyberProtectComponentFactory : public WsfComponentFactory<WsfPlatform>
{
public:
   bool PreInitialize(double, WsfPlatform& aPlatform) override
   {
      // If the platform does not have a cyber_protect component then force a default one on to the platform.
      if (Protect::Find(aPlatform) == nullptr)
      {
         aPlatform.AddComponent(new Protect(GetScenario()));
      }
      return true;
   }

   bool ProcessAddOrEditCommand(UtInput& aInput, WsfPlatform& aPlatform, bool aIsAdding) override
   {
      ProtectTypes& types(ProtectTypes::Get(GetScenario()));
      return types.LoadUnnamedComponent(aInput, aPlatform, aIsAdding, cCOMPONENT_ROLE<Protect>());
   }

   bool ProcessDeleteCommand(UtInput& aInput, WsfPlatform& aPlatform) override
   {
      ProtectTypes& types(ProtectTypes::Get(GetScenario()));
      return types.DeleteUnnamedComponent(aInput, aPlatform, cCOMPONENT_ROLE<Protect>());
   }
};
} // namespace

// =================================================================================================
// static
Protect* Protect::Find(const WsfPlatform& aPlatform)
{
   return static_cast<Protect*>(aPlatform.FindComponentByRole(cCOMPONENT_ROLE<Protect>()));
}

// =================================================================================================
// static
void Protect::RegisterComponentFactory(WsfScenario& aScenario)
{
   // Register the component factory that loads instances onto the platform
   aScenario.RegisterComponentFactory(ut::make_unique<CyberProtectComponentFactory>());
}

// =================================================================================================
Protect::Protect(WsfScenario& aScenario)
   : WsfObject()
   , WsfSimplePlatformComponent(cCOMPONENT_ROLE<Protect>(), "cyber_protect")
   , mScenarioPtr(&aScenario)
   , mPlatformPtr(nullptr)
   , mNamedAttacks(0)
   , mDefaultDefined(false)
   , mImmuneAttacks(0)
   , mContext(*aScenario.GetScriptContext(), "WsfCyberProtect")
{
   mEngagementClassPtr = mContext.GetClass("WsfCyberEngagement");
}

// =================================================================================================
Protect::Protect(const Protect& aSrc)
   : WsfObject(aSrc)
   , WsfSimplePlatformComponent(aSrc)
   , mScenarioPtr(aSrc.mScenarioPtr)
   , mPlatformPtr(aSrc.mPlatformPtr)
   , mNamedAttacks(aSrc.mNamedAttacks)
   , mDefaultDefined(aSrc.mDefaultDefined)
   , mImmuneAttacks(aSrc.mImmuneAttacks)
   , mContext(aSrc.mContext)
{
   mEngagementClassPtr = mContext.GetClass("WsfCyberEngagement");
}

// =================================================================================================
// virtual
Protect* Protect::Clone() const
{
   return new Protect(*this);
}

// =================================================================================================
// virtual
bool Protect::Initialize(double aSimTime)
{
   bool ok = mContext.Initialize(aSimTime, mPlatformPtr->GetScriptContext(), this);

   // Initialize the random variables for named attacks
   for (auto& protect : mNamedAttacks)
   {
      std::string detectionName = "attack_detection_delay_time_" + protect.GetName();
      std::string recoveryName  = "attack_recovery_delay_time_" + protect.GetName();

      ok &= protect.mDetectionDelay.Initialize(detectionName, this, &mContext, *mPlatformPtr->GetSimulation());
      ok &= protect.mRecoveryDelay.Initialize(recoveryName, this, &mContext, *mPlatformPtr->GetSimulation());
   }

   return ok;
}

// =================================================================================================
// virtual
bool Protect::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());

   // Construct a temporary ProtectedAttack object in case it is used
   ProtectedAttack tempProtectedAttack;

   if (tempProtectedAttack.ProcessInput(aInput, mContext))
   {
      if (tempProtectedAttack.GetName() == "default_attack")
      {
         mDefaultDefined = true;
      }
      mNamedAttacks.push_back(tempProtectedAttack);
   }
   else if (mContext.ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = WsfObject::ProcessInput(aInput);
   }
   return myCommand;
}

bool Protect::ProtectedAttack::ProcessInput(UtInput& aInput, WsfScriptContext& aContext)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());

   if (command == "attack_response")
   {
      std::string attackName;
      aInput.ReadValue(attackName);
      SetName(attackName);

      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (command == "probability_of_scan_detection")
         {
            aInput.ReadValue(mScanDetection);
            aInput.ValueInClosedRange(mScanDetection, 0.0, 1.0);
         }
         else if (command == "probability_of_scan_attribution")
         {
            aInput.ReadValue(mScanAttribution);
            aInput.ValueInClosedRange(mScanAttribution, 0.0, 1.0);
         }
         else if (command == "probability_of_attack_success")
         {
            aInput.ReadValue(mAttackSuccess);
            aInput.ValueInClosedRange(mAttackSuccess, 0.0, 1.0);
         }
         else if (command == "probability_of_status_report")
         {
            aInput.ReadValue(mStatusReport);
            aInput.ValueInClosedRange(mStatusReport, 0.0, 1.0);
         }
         else if (command == "probability_of_attack_detection")
         {
            aInput.ReadValue(mAttackDetection);
            aInput.ValueInClosedRange(mAttackDetection, 0.0, 1.0);
         }
         else if (command == "probability_of_attack_attribution")
         {
            aInput.ReadValue(mAttackAttribution);
            aInput.ValueInClosedRange(mAttackAttribution, 0.0, 1.0);
         }
         else if (command == "probability_of_future_immunity")
         {
            aInput.ReadValue(mImmunity);
            aInput.ValueInClosedRange(mImmunity, 0.0, 1.0);
         }
         else if (command == "attack_detection_delay_time")
         {
            mDetectionDelay.ProcessInput(aInput, UtInput::cTIME);
         }
         else if (command == "attack_recovery_delay_time")
         {
            mRecoveryDelay.ProcessInput(aInput, UtInput::cTIME);
         }
         else if (command == "restore")
         {
            mRestore = true;
         }
         else if (command == "script")
         {
            UtScript* scriptPtr = aContext.Compile(aInput, "end_script", GetName());

            std::string scriptName     = scriptPtr->GetName();
            bool        validSignature = true;

            if (scriptName == GetName() + "::" + "OnScanDetection")
            {
               validSignature   = aContext.ValidateScript(scriptPtr, "void", "WsfCyberEngagement");
               mOnScanDetection = scriptPtr;
            }
            else if (scriptName == GetName() + "::" + "OnAttackDetection")
            {
               validSignature     = aContext.ValidateScript(scriptPtr, "void", "WsfCyberEngagement");
               mOnAttackDetection = scriptPtr;
            }
            else if (scriptName == GetName() + "::" + "OnAttackRecovery")
            {
               validSignature    = aContext.ValidateScript(scriptPtr, "void", "WsfCyberEngagement");
               mOnAttackRecovery = scriptPtr;
            }
            else if (scriptName == GetName() + "::" + "IsVulnerable")
            {
               validSignature = aContext.ValidateScript(scriptPtr, "bool", "WsfCyberEngagement");
               mIsVulnerable  = scriptPtr;
            }
            else
            {
               // Only allow script method definitions for the above types. Anything
               // else will throw an exception
               throw UtException("Unrecognized script method in " + GetName());
            }

            // Throw an exception if the script signature is incorrect
            if (!validSignature)
            {
               throw UtException(
                  "Invalid script return type, argument type, or argument count in CyberProtect attack_response " +
                  GetName());
            }
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
Protect::ProtectedAttackListIt Protect::FindAttackProtection(const std::string& aAttackName) const
{
   return std::find_if(std::begin(mNamedAttacks),
                       std::end(mNamedAttacks),
                       [aAttackName](const ProtectedAttack& aSrc) { return (aAttackName == aSrc.GetName()); });
}

// =================================================================================================
Protect::ProtectedAttackListNonConstIt Protect::FindAttackProtection(const std::string& aAttackName)
{
   return std::find_if(std::begin(mNamedAttacks),
                       std::end(mNamedAttacks),
                       [aAttackName](const ProtectedAttack& aSrc) { return (aAttackName == aSrc.GetName()); });
}

// =================================================================================================
bool Protect::ProtectionExists(const std::string& aAttackName) const
{
   auto protectIt = FindAttackProtection(aAttackName);
   if (protectIt == mNamedAttacks.end())
   {
      return false;
   }

   return true;
}

// =================================================================================================
double Protect::GetProbabilityOfScanDetection(const std::string& aAttackName) const
{
   auto protectIt = FindAttackProtection(aAttackName);
   if (protectIt == mNamedAttacks.end())
   {
      return -1.0;
   }

   return protectIt->mScanDetection;
}

// =================================================================================================
double Protect::GetProbabilityOfScanAttribution(const std::string& aAttackName) const
{
   auto protectIt = FindAttackProtection(aAttackName);
   if (protectIt == mNamedAttacks.end())
   {
      return -1.0;
   }

   return protectIt->mScanAttribution;
}

// =================================================================================================
double Protect::GetProbabilityOfAttackSuccess(const std::string& aAttackName) const
{
   auto protectIt = FindAttackProtection(aAttackName);
   if (protectIt == mNamedAttacks.end())
   {
      return -1.0;
   }

   return protectIt->mAttackSuccess;
}

// =================================================================================================
double Protect::GetProbabilityOfStatusReport(const std::string& aAttackName) const
{
   auto protectIt = FindAttackProtection(aAttackName);
   if (protectIt == mNamedAttacks.end())
   {
      return -1.0;
   }

   return protectIt->mStatusReport;
}

// =================================================================================================
double Protect::GetProbabilityOfAttackDetection(const std::string& aAttackName) const
{
   auto protectIt = FindAttackProtection(aAttackName);
   if (protectIt == mNamedAttacks.end())
   {
      return -1.0;
   }

   return protectIt->mAttackDetection;
}

// =================================================================================================
double Protect::GetProbabilityOfAttackAttribution(const std::string& aAttackName) const
{
   auto protectIt = FindAttackProtection(aAttackName);
   if (protectIt == mNamedAttacks.end())
   {
      return -1.0;
   }

   return protectIt->mAttackAttribution;
}

// =================================================================================================
double Protect::GetProbabilityOfFutureImmunity(const std::string& aAttackName) const
{
   auto protectIt = FindAttackProtection(aAttackName);
   if (protectIt == mNamedAttacks.end())
   {
      return -1.0;
   }

   return protectIt->mImmunity;
}

// =================================================================================================
double Protect::DetectionDelayTime(const std::string& aAttackName) const
{
   auto protectIt = FindAttackProtection(aAttackName);
   if (protectIt == mNamedAttacks.end())
   {
      return 0.0; // Default delay is none
   }

   return protectIt->mDetectionDelay.Draw();
}

// =================================================================================================
double Protect::RecoveryDelayTime(const std::string& aAttackName) const
{
   auto protectIt = FindAttackProtection(aAttackName);
   if (protectIt == mNamedAttacks.end())
   {
      return 0.0; // Default delay is none
   }

   return protectIt->mRecoveryDelay.Draw();
}

// =================================================================================================
double Protect::LastDetectionDelayTime(const std::string& aAttackName) const
{
   auto protectIt = FindAttackProtection(aAttackName);
   if (protectIt == mNamedAttacks.end())
   {
      return -1.0;
   }

   return protectIt->mDetectionDelay;
}

// =================================================================================================
double Protect::LastRecoveryDelayTime(const std::string& aAttackName) const
{
   auto protectIt = FindAttackProtection(aAttackName);
   if (protectIt == mNamedAttacks.end())
   {
      return -1.0;
   }

   return protectIt->mRecoveryDelay;
}

// =================================================================================================
bool Protect::DoRestore(const std::string& aAttackName) const
{
   auto protectIt = FindAttackProtection(aAttackName);
   if (protectIt == mNamedAttacks.end())
   {
      return false;
   }

   return protectIt->mRestore;
}

// =================================================================================================
bool Protect::IsImmune(const std::string& aAttackName) const
{
   auto it = std::find(mImmuneAttacks.begin(), mImmuneAttacks.end(), aAttackName);
   if (it != mImmuneAttacks.end())
   {
      return true;
   }

   return false;
}

// =================================================================================================
void Protect::ExecuteOnAttackDetection(Engagement& aEngagement, double aSimTime)
{
   auto protectIt = FindAttackProtection(aEngagement.GetAttackType());
   if (protectIt == mNamedAttacks.end() || !protectIt->mOnAttackDetection)
   {
      return;
   }

   UtScriptData       scriptRetVal;
   UtScriptDataPacker scriptArgs{mScenarioPtr->GetScriptTypes()};

   scriptArgs.Push(&aEngagement, "WsfCyberEngagement");
   mContext.ExecuteScript(aSimTime, protectIt->mOnAttackDetection, scriptRetVal, scriptArgs);
}

// =================================================================================================
void Protect::ExecuteOnAttackRecovery(Engagement& aEngagement, double aSimTime)
{
   auto protectIt = FindAttackProtection(aEngagement.GetAttackType());
   if (protectIt == mNamedAttacks.end() || !protectIt->mOnAttackRecovery)
   {
      return;
   }

   UtScriptData       scriptRetVal;
   UtScriptDataPacker scriptArgs{mScenarioPtr->GetScriptTypes()};

   scriptArgs.Push(&aEngagement, "WsfCyberEngagement");
   mContext.ExecuteScript(aSimTime, protectIt->mOnAttackRecovery, scriptRetVal, scriptArgs);
}

// =================================================================================================
void Protect::ExecuteOnScanDetection(Engagement& aEngagement, double aSimTime)
{
   auto protectIt = FindAttackProtection(aEngagement.GetAttackType());
   if (protectIt == mNamedAttacks.end() || !protectIt->mOnScanDetection)
   {
      return;
   }

   UtScriptData       scriptRetVal;
   UtScriptDataPacker scriptArgs{mScenarioPtr->GetScriptTypes()};

   scriptArgs.Push(&aEngagement, "WsfCyberEngagement");
   mContext.ExecuteScript(aSimTime, protectIt->mOnScanDetection, scriptRetVal, scriptArgs);
}

// =================================================================================================
bool Protect::ExecuteIsVulnerable(Engagement& aEngagement, double aSimTime, bool& aWasRun)
{
   // Unknown value for this argument on entry. Set to true until known otherwise
   aWasRun = true;

   auto protectIt = FindAttackProtection(aEngagement.GetAttackType());
   if (protectIt == mNamedAttacks.end() || !protectIt->mIsVulnerable)
   {
      aWasRun = false;
      return false;
   }

   UtScriptData       scriptRetVal;
   UtScriptDataPacker scriptArgs{mScenarioPtr->GetScriptTypes()};

   scriptArgs.Push(&aEngagement, "WsfCyberEngagement");
   mContext.ExecuteScript(aSimTime, protectIt->mIsVulnerable, scriptRetVal, scriptArgs);

   return scriptRetVal.GetBool();
}

} // namespace cyber
} // namespace wsf
