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

#ifndef WSFCYBERENGAGEMENT_HPP
#define WSFCYBERENGAGEMENT_HPP

#include "wsf_cyber_export.h"

#include <limits>

#include "UtCloneablePtr.hpp"
#include "UtScriptAccessible.hpp"
#include "UtScriptBasicTypes.hpp"
#include "UtScriptClass.hpp"
#include "WsfCyberRandom.hpp"
class WsfPlatform;
class WsfSimulation;

namespace wsf
{
namespace cyber
{
class Attack;
class Protect;
class Constraint;

//! A class that represents the cyber engagements between two platforms.
class WSF_CYBER_EXPORT Engagement : public UtScriptAccessible
{
public:
   enum CyberAttackFailure
   {
      cATTACK_RANDOM_DRAW,
      cATTACK_IMMUNITY,
      cATTACK_NOT_VULNERABLE,
      cATTACK_INSUFFICIENT_RESOURCES,
      cATTACK_NONE
   };

   enum CyberScanFailure
   {
      cSCAN_IMMUNITY,
      cSCAN_DETECTED,
      cSCAN_NOT_VULNERABLE,
      cSCAN_NONE
   };

   //! No default constructor usage - parameters required for instantiation.
   Engagement(std::string    aAttackingPlatform,
              std::string    aVictimPlatform,
              std::string    aNamedAttack,
              WsfSimulation& aSimulation,
              size_t         aKey);

   virtual ~Engagement();

   Engagement(const Engagement& aSrc) = delete;
   Engagement(Engagement&& aSrc)      = default;
   const Engagement& operator=(const Engagement& aRhs) = delete;

   const char* GetScriptClassName() const override { return "WsfCyberEngagement"; }

   //! @name Public accessors for engagement data
   //@{
   const std::string& GetAttacker() const { return mAttacker; }
   size_t             GetAttackerIndex() const { return mAttackerIndex; }
   const std::string& GetVictim() const { return mVictim; }
   size_t             GetVictimIndex() const { return mVictimIndex; }
   const std::string& GetAttackType() const { return mNamedAttack; }
   size_t             GetKey() const { return mKey; }

   Attack* GetAttack() const { return mAttackPtr; }
   double  GetDuration() const { return mDuration; }
   double  GetAttackStartTime() const { return mTimeAttackStart; }
   double  GetAttackSuccessThreshold() const { return mAttackSuccessThreshold; }
   double  GetAttackDraw() const { return mAttackDraw; }
   double  GetStatusReportThreshold() const { return mStatusReportThreshold; }
   double  GetStatusReportDraw() const { return mStatusReportDraw; }

   //! Returns true if the attack status has been reported.
   //! Returns false if it failed to be reported or hasn't yet been reported.
   bool                            GetStatusReportSuccess() const;
   double                          GetTimeAttackDiscovered() const { return mTimeAttackDetection; }
   double                          GetAttackDetectionThreshold() const { return mAttackDetectionThreshold; }
   double                          GetAttackDetectionDraw() const { return mAttackDetectionDraw; }
   double                          GetAttackAttributionThreshold() const { return mAttackAttributionThreshold; }
   double                          GetAttackAttributionDraw() const { return mAttackAttributionDraw; }
   double                          GetTimeAttackRecovery() const { return mTimeAttackRecovery; }
   bool                            GetRecovery() const { return mRecover; }
   double                          GetScanStartTime() const { return mTimeScanStart; }
   double                          GetScanDetectionThreshold() const { return mScanDetectionThreshold; }
   double                          GetScanDetectionDraw() const { return mScanDetectionDraw; }
   double                          GetScanAttributionThreshold() const { return mScanAttributionThreshold; }
   double                          GetScanAttributionDraw() const { return mScanAttributionDraw; }
   double                          GetScanDelayTime() const { return mTimeScanDelay; }
   double                          GetDeliveryDelayTime() const { return mTimeDeliveryDelay; }
   bool                            GetScanSuccess() const { return mScanSuccess; }
   bool                            GetAttackInProgress() const { return mAttackInProgress; }
   bool                            GetAttackSuccess() const { return mAttackSuccess; }
   const std::vector<std::string>& GetAttackEffects() const { return mAttackEffectTypes; }
   double                          GetAttackDetectionDelayTime() const { return mTimeAttackDetectionDelay; }
   double                          GetAttackRecoveryDelayTime() const { return mTimeAttackRecoveryDelay; }
   double                          GetImmunityThreshold() const { return mImmunityThreshold; }
   double                          GetImmunityDraw() const { return mImmunityDraw; }

   //! We provide a pointer to the protection used, as in some cases, the protection on the platform
   //! is not the valid protection object.
   Protect* GetUsedProtection() { return mProtectTypePtr; }
   //@}

   //! @name Accessor methods for engagement failure status
   //@{
   CyberAttackFailure GetAttackFailureReason() const { return mAttackFailure; }
   CyberScanFailure   GetScanFailureReason() const { return mScanFailure; }
   //@}

   //! @name Mutator methods for engagement failure status
   //! @note These can be set externally (namely by the engagement manager) to
   //! indicate why the engagement object has aborted processing/updating
   //@{
   void SetAttackFailureReason(CyberAttackFailure aAttackFailure) { mAttackFailure = aAttackFailure; }
   void SetScanFailureReason(CyberScanFailure aScanFailure) { mScanFailure = aScanFailure; }
   //@}

   //! @name Mutators for engagement data that may be set during use
   //@{
   void SetAttackStartTime();
   void SetScanStartTime();
   void SetTimeAttackDiscovered();
   void SetTimeAttackRecovery();

   void SetScanSuccess(bool aSuccess) { mScanSuccess = aSuccess; }
   void SetAttackInProgress(bool aInProgress) { mAttackInProgress = aInProgress; }
   void SetAttackSuccess(bool aSuccess) { mAttackSuccess = aSuccess; }
   //@}

   //! @name Immunity Query Method
   //! Provides a wrapper to query the cyber_protect object if it is currently immune to
   //! the provided named attack. This method is available here with the engagement object
   //! to allow for ease of access and to prevent other objects from accessing the cyber_protect
   //! directly
   //! @note Because this call queries the
   bool IsVictimImmune() const;

   //! @name Draw Method
   //! Asks the engagement object to generate a draw for the supplied probability type
   //! The values generated will be stored with the engagement object
   //! A boolean value is return to indicate if the threshold was met
   bool Draw(random::ProbabilityType aProbabilityType);

   //! @name Reset Method
   //! Re-establishes original values on member variables in this object,
   //! with the exception of values that aid subsequent uses of the engagement
   //! object
   virtual void Reset(bool aScanReset = false);

   //! @name Release Attack Constraints Method
   //! Releases all resources consumed by attacker after it has been destroyed.
   void ReleaseAttackerConstraints();

   //! @name Meets Attacker Constraints
   //! Ensures that enough resources are available to the attacker to allow for
   //! the named attack type.
   bool MeetsAttackerConstraints();

   //! @name Make Constraint Reservations
   //! If sufficient resources are available, the method reserves the resources
   //! required for a named attack type by removing the resources from those available
   //! and adding the attack type to any other concurrent attacks.
   bool MakeConstraintReservations();

   //! @name Existing Constraint Reservations
   //! Checks and returns whether any of the platform's cyber resource are reserved.
   bool ExistingConstraintReservations();

   WsfSimulation& GetSimulation() const { return mSimulation; }

private:
   //! @name SetDefiningCyberObject method
   //! This method is called during the construction of the engagement object and provides
   //! the values for the attack and protection object pointers, and sets the mUseProtectionDefinition
   //! value to indicate which object defines the threshold values for the engagement. Upon completion,
   //! these values will be updated in the cyber engagement object via the SetInitialValues method.
   //! @note The attack reference is not stored a local value, as it does not need to be used or retained
   //! beyond construction, unlike the protection object. The attack object is returned for local use ONLY.
   void SetDefiningCyberObject();

   //! @name SetInitialValues method
   //! Upon completion of setting the defining object via SetDefiningCyberObject, this method will use
   //! that object (or other methods in a derived virtual method) to provide values to the engagement
   //! object probability thresholds.
   void SetInitialValues();

   std::string mAttacker{};
   size_t      mAttackerIndex{0U};
   std::string mVictim{};
   size_t      mVictimIndex{0U};
   std::string mNamedAttack{};
   size_t      mKey{0U};

   //! Attack member variables
   double             mTimeAttackStart{std::numeric_limits<double>::max()};
   double             mDuration{-1.0};
   double             mAttackSuccessThreshold{1.0};
   double             mAttackDraw{-1.0};
   double             mCyberResourceUsage{0.0};
   double             mStatusReportThreshold{1.0};
   double             mStatusReportDraw{-1.0};
   double             mTimeAttackDetection{-1.0};
   double             mTimeAttackDetectionDelay{std::numeric_limits<double>::max()};
   double             mAttackDetectionThreshold{0.0};
   double             mAttackDetectionDraw{-1.0};
   double             mAttackAttributionThreshold{0.0};
   double             mAttackAttributionDraw{-1.0};
   double             mTimeAttackRecovery{-1.0};
   double             mTimeAttackRecoveryDelay{std::numeric_limits<double>::max()};
   bool               mRecover{false};
   CyberAttackFailure mAttackFailure{cATTACK_NONE};
   double             mTimeDeliveryDelay{0.0};
   bool               mAttackInProgress{false};
   bool               mAttackSuccess{false};

   //! Scan member variables
   double           mTimeScanStart{std::numeric_limits<double>::max()};
   double           mScanDetectionThreshold{0.0};
   double           mScanDetectionDraw{-1.0};
   double           mScanAttributionThreshold{0.0};
   double           mScanAttributionDraw{-1.0};
   CyberScanFailure mScanFailure{cSCAN_NONE};
   double           mTimeScanDelay{0.0};
   bool             mScanSuccess{false};

   //! Other member variables
   double mImmunityThreshold{0.0};
   double mImmunityDraw{-1.0};

   bool mUseProtectDefinition{false};

   WsfSimulation&           mSimulation;
   Protect*                 mProtectTypePtr{nullptr};
   Constraint*              mConstraintPtr{nullptr};
   std::vector<std::string> mAttackEffectTypes{};

   //! A copy of the attack object. The attack object is
   //! simply copied and maintained with an engagement object.
   //! This reduces the need for copying data and provides
   //! better support for user extensions.
   UtCloneablePtr<Attack> mAttackPtr{nullptr};
};


//! The script interface 'class' for WsfCyberEngagement
class WSF_CYBER_EXPORT ScriptEngagement : public UtScriptClass
{
public:
   ScriptEngagement(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);


   //! Accessor methods for general (sim) data
   //@{
   UT_DECLARE_SCRIPT_METHOD(Attacker);
   UT_DECLARE_SCRIPT_METHOD(AttackerPlatform);
   UT_DECLARE_SCRIPT_METHOD(AttackType);
   UT_DECLARE_SCRIPT_METHOD(Victim);
   UT_DECLARE_SCRIPT_METHOD(VictimPlatform);
   //@}

   //! Accessor methods for attack status
   //@{
   UT_DECLARE_SCRIPT_METHOD(TimeAttackInitiated);
   UT_DECLARE_SCRIPT_METHOD(AttackSuccessThreshold);
   UT_DECLARE_SCRIPT_METHOD(AttackSuccessDraw);
   UT_DECLARE_SCRIPT_METHOD(StatusReportThreshold);
   UT_DECLARE_SCRIPT_METHOD(StatusReportDraw);
   UT_DECLARE_SCRIPT_METHOD(TimeAttackDiscovered);
   UT_DECLARE_SCRIPT_METHOD(AttackDetectionThreshold);
   UT_DECLARE_SCRIPT_METHOD(AttackDetectionDraw);
   UT_DECLARE_SCRIPT_METHOD(AttackAttributionThreshold);
   UT_DECLARE_SCRIPT_METHOD(AttackAttributionDraw);
   UT_DECLARE_SCRIPT_METHOD(TimeAttackRecovery);
   UT_DECLARE_SCRIPT_METHOD(Recovery);
   UT_DECLARE_SCRIPT_METHOD(AttackDeliveryDelayTime);
   UT_DECLARE_SCRIPT_METHOD(AttackDetectionDelayTime);
   UT_DECLARE_SCRIPT_METHOD(AttackRecoveryDelayTime);
   UT_DECLARE_SCRIPT_METHOD(AttackSuccess);
   UT_DECLARE_SCRIPT_METHOD(AttackInProgress);
   UT_DECLARE_SCRIPT_METHOD(AttackFailureReason);
   //@}

   //! Accessor methods for scan status
   //@{
   UT_DECLARE_SCRIPT_METHOD(TimeScanInitiated);
   UT_DECLARE_SCRIPT_METHOD(ScanDetectionThreshold);
   UT_DECLARE_SCRIPT_METHOD(ScanDetectionDraw);
   UT_DECLARE_SCRIPT_METHOD(ScanAttributionThreshold);
   UT_DECLARE_SCRIPT_METHOD(ScanAttributionDraw);
   UT_DECLARE_SCRIPT_METHOD(ScanDelayTime);
   UT_DECLARE_SCRIPT_METHOD(ScanSuccess);
   UT_DECLARE_SCRIPT_METHOD(ScanInProgress);
   UT_DECLARE_SCRIPT_METHOD(ScanFailureReason);
   //@}
};

} // namespace cyber

} // namespace wsf

#endif
