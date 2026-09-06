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

#ifndef WSFCYBERPROTECT_HPP
#define WSFCYBERPROTECT_HPP

#include "wsf_cyber_export.h"

#include <list>

#include "UtMath.hpp"
class UtScript;
#include "UtScriptContext.hpp"
#include "WsfCyberComponentRoles.hpp"
#include "WsfObject.hpp"
class WsfPlatform;
#include "WsfRandomVariable.hpp"
class WsfScenario;
#include "WsfSimpleComponent.hpp"
#include "script/WsfScriptContext.hpp"

namespace wsf
{
namespace cyber
{
class Engagement;

//! Platform component for cyber warfare.
//!
//! This defines a platform component that implements the modeling of cyber warfare against the platform.
class WSF_CYBER_EXPORT Protect : public WsfObject, public WsfSimplePlatformComponent
{
public:
   static Protect* Find(const WsfPlatform& aPlatform);
   static void     RegisterComponentFactory(WsfScenario& aScenario);

   Protect(WsfScenario& aScenario);
   ~Protect() override = default;

   //! Assignment operator deleted to prevent use.
   Protect& operator=(const Protect& aRhs) = delete;

   //! @name Component infrastructure methods
   //@{
   WsfComponent* CloneComponent() const override { return Clone(); }
   void*         QueryInterface(int aRole) override { return (aRole == mRoles[0]) ? this : nullptr; }
   void          ComponentParentChanged(WsfPlatform* aPlatformPtr) override { mPlatformPtr = aPlatformPtr; }
   //@}

   //! @name Common infrastructure methods.
   //@{
   Protect* Clone() const override;
   bool     Initialize(double aSimTime) override;
   bool     ProcessInput(UtInput& aInput) override;
   //@}

   //! @name DefaultExists method
   //! Returns true if the cyber protect object has a default defined
   bool DefaultExists() const { return mDefaultDefined; }

   //! @name ProtectionExists method
   //! Returns true if a specific named attack provided in the argument has
   //! a corresponding protection entry in the object
   bool ProtectionExists(const std::string& aAttackName) const;

   //! @name Cyber Protect query/inspection methods
   //@{
   double GetProbabilityOfScanDetection(const std::string& aAttackName) const;
   double GetProbabilityOfScanAttribution(const std::string& aAttackName) const;
   double GetProbabilityOfAttackSuccess(const std::string& aAttackName) const;
   double GetProbabilityOfStatusReport(const std::string& aAttackName) const;
   double GetProbabilityOfAttackDetection(const std::string& aAttackName) const;
   double GetProbabilityOfAttackAttribution(const std::string& aAttackName) const;
   double GetProbabilityOfFutureImmunity(const std::string& aAttackName) const;
   double DetectionDelayTime(const std::string& aAttackName) const;
   double RecoveryDelayTime(const std::string& aAttackName) const;
   double LastDetectionDelayTime(const std::string& aAttackName) const;
   double LastRecoveryDelayTime(const std::string& aAttackName) const;
   bool   DoRestore(const std::string& aAttackName) const;
   bool   IsImmune(const std::string& aAttackName) const;
   //@}

   //! @name User-defined script method execution
   //@{
   void ExecuteOnAttackDetection(Engagement& aEngagement, double aSimTime);
   void ExecuteOnAttackRecovery(Engagement& aEngagement, double aSimTime);
   void ExecuteOnScanDetection(Engagement& aEngagement, double aSimTime);
   bool ExecuteIsVulnerable(Engagement& aEngagement, double aSimTime, bool& aWasRun);
   //@}

   //! @name Immunity Mutator
   //! Set the immunity for this component
   void SetImmune(const std::string& aNamedAttack) { mImmuneAttacks.push_back(aNamedAttack); }

   WsfScriptContext& GetScriptContext() { return mContext; }
   UtScriptContext*  GetScriptAccessibleContext() const override { return &mContext.GetContext(); }

protected:
   Protect(const Protect& aSrc);

   //! A container class for storing the specific parameters for each
   //! attack protected against. Placed here as the implementation
   //! shouldn't be visible outside of WsfCyberProtect or its derived
   //! classes
   class ProtectedAttack : public WsfNamed
   {
   public:
      ~ProtectedAttack() override = default;

      bool ProcessInput(UtInput& aInput, WsfScriptContext& aContext);

      //! These are all public, as they should only be accessed within
      //! the WsfCyberProtect class, and an interface is overkill
      double mScanDetection{0.0};
      double mScanAttribution{0.0};
      double mAttackSuccess{1.0};
      double mStatusReport{0.0};
      double mAttackDetection{0.0};
      double mAttackAttribution{0.0};
      double mImmunity{0.0};

      mutable WsfRandomVariable mDetectionDelay{WsfRandomVariable::cCONSTANT, 0, 0, WsfRandomVariable::cNON_NEGATIVE};
      mutable WsfRandomVariable mRecoveryDelay{WsfRandomVariable::cCONSTANT, 0, 0, WsfRandomVariable::cNON_NEGATIVE};

      bool mRestore{false};

      UtScript* mOnAttackDetection{nullptr};
      UtScript* mOnAttackRecovery{nullptr};
      UtScript* mOnScanDetection{nullptr};
      UtScript* mIsVulnerable{nullptr};
   };

   //! @name Definitions to support internal ProtectedAttack container usage
   //@{
   using ProtectedAttackList           = std::vector<ProtectedAttack>;
   using ProtectedAttackListIt         = std::vector<ProtectedAttack>::const_iterator;
   using ProtectedAttackListNonConstIt = std::vector<ProtectedAttack>::iterator;
   //@}

private:
   //! Internal helper method for finding the protection against a specific named attack.
   ProtectedAttackListIt         FindAttackProtection(const std::string& aAttackName) const;
   ProtectedAttackListNonConstIt FindAttackProtection(const std::string& aAttackName);

   //! Pointer to the scenario that owns this instance.
   WsfScenario* mScenarioPtr;

   //! Pointer to the platform that owns this instance.
   WsfPlatform* mPlatformPtr;

   //! A list of named attacks that are protected against.
   ProtectedAttackList mNamedAttacks;

   //! If the default attack protection exists, is set to true
   bool mDefaultDefined;

   //! A list of attacks that this protection provides "immunity" to, i.e. the
   //! probability chance of success is 0.0
   std::vector<std::string> mImmuneAttacks;

   WsfScriptContext mContext;
   UtScriptClass*   mEngagementClassPtr; // = mContext.GetClass("WsfCyberEngagement");
};

} // namespace cyber
} // namespace wsf

WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::cyber::Protect, cWSF_COMPONENT_CYBER_PROTECT)

#endif
