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

#ifndef WSFCYBERATTACK_HPP
#define WSFCYBERATTACK_HPP

#include "wsf_cyber_export.h"

#include <memory>
#include <string>
#include <vector>

#include "WsfObject.hpp"
#include "WsfRandomVariable.hpp"
class WsfSimulation;

namespace wsf
{
namespace cyber
{
class Engagement;

namespace random
{
//! Enumerated frequency of draws that are possible in Cyber.
enum ProbabilityFrequency
{
   cALWAYS,
   cONCE_PER_SIMULATION,
   cONCE_PER_TARGET,
   cINTERVAL_PER_SIMULATION,
   cINTERVAL_PER_TARGET,
   cOTHER
};
} // namespace random

//! A class that defines the effect types that make up a cyber attack.
//!
//! A 'cyber attack' is composed of one or more 'cyber effects' that define what the attack does
//! to the victim. In many cases an attack has only one effect, but this makes it possible to define
//! an attack that is composed of several simple pre-defined effects.
//!
//! @note 'cyber_attack' types are defined in the type list and from then are read-only. At run-time
//! the attacker will mention this attack by name and WsfCyberProtect will locate the object and
//! get the data it needs.
class WSF_CYBER_EXPORT Attack : public WsfObject
{
public:
   Attack()                   = default;
   ~Attack() override         = default;
   Attack(const Attack& aSrc) = default;
   Attack& operator=(const Attack& aRhs) = default;

   //! @name Common infrastructure methods.
   //@{
   Attack* Clone() const override;
   bool    ProcessInput(UtInput& aInput) override;
   //@}

   virtual bool Initialize(double aSimTime, WsfSimulation& aSimulation);

   //! @name Accessors for the default probabilities.
   //@{
   double GetProbabilityOfScanDetection() const { return mProbabilityOfScanDetection; }
   double GetProbabilityOfScanAttribution() const { return mProbabilityOfScanAttribution; }
   double GetProbabilityOfAttackSuccess() const { return mProbabilityOfAttackSuccess; }
   double GetProbabilityOfStatusReport() const { return mProbabilityOfStatusReport; }
   double GetProbabilityOfAttackDetection() const { return mProbabilityOfAttackDetection; }
   double GetProbabilityOfAttackAttribution() const { return mProbabilityOfAttackAttribution; }
   double GetProbabilityOfFutureImmunity() const { return mProbabilityOfFutureImmunity; }
   //@}

   //! @name Accessors for the probability frequencies.
   //@{
   random::ProbabilityFrequency GetFrequencyOfScanDetection() const { return mFrequencyOfScanDetection; }
   random::ProbabilityFrequency GetFrequencyOfScanAttribution() const { return mFrequencyOfScanAttribution; }
   random::ProbabilityFrequency GetFrequencyOfAttackSuccess() const { return mFrequencyOfAttackSuccess; }
   random::ProbabilityFrequency GetFrequencyOfStatusReport() const { return mFrequencyOfStatusReport; }
   random::ProbabilityFrequency GetFrequencyOfAttackDetection() const { return mFrequencyOfAttackDetection; }
   random::ProbabilityFrequency GetFrequencyOfAttackAttribution() const { return mFrequencyOfAttackAttribution; }
   random::ProbabilityFrequency GetFrequencyOfFutureImmunity() const { return mFrequencyOfFutureImmunity; }
   //@}

   //! @name Accessors for the random time intervals.
   //@{
   WsfRandomVariable& GetRandomVariableScanDetection() { return mRandomVariableScanDetection; }
   WsfRandomVariable& GetRandomVariableScanAttribution() { return mRandomVariableScanAttribution; }
   WsfRandomVariable& GetRandomVariableAttackSuccess() { return mRandomVariableAttackSuccess; }
   WsfRandomVariable& GetRandomVariableStatusReport() { return mRandomVariableStatusReport; }
   WsfRandomVariable& GetRandomVariableAttackDetection() { return mRandomVariableAttackDetection; }
   WsfRandomVariable& GetRandomVariableAttackAttribution() { return mRandomVariableAttackAttribution; }
   WsfRandomVariable& GetRandomVariableFutureImmunity() { return mRandomVariableFutureImmunity; }
   //@}

   //! @name Accessors for attack duration requirements.
   //@{
   double GetDuration() const { return mDuration; }
   //@}

   //! @name Accessors for the time delays.
   //@{
   double GetTimeDelayScan() const { return mTimeDelayScan.Draw(); }
   double GetTimeDelayDelivery() const { return mTimeDelayDelivery.Draw(); }
   //@}

   //! @name Accessor the time delays, with a draw for a new value.
   //@{
   double GetLastTimeDelayScan() const { return mTimeDelayScan; }
   double GetLastTimeDelayDelivery() const { return mTimeDelayDelivery; }
   //@}

   //! @name Accessor for effect types for this attack
   const std::vector<std::string>& GetEffects() const { return mEffectTypes; }

   //! @name Accessors for attack duration requirements.
   double GetResourceRequirements() const { return mResourcesRequired; }

   //! @name ExtendedDrawDefinition
   //! Provides a user implemented draw value determination method for ProbabilityFrequency::cOTHER case.
   //! This is resolved in wsf::cyber::Random, as determination of a whether or not a draw needs to
   //! occur is cached and stored via that object. Access to that functionality is not public, so this
   //! provides a hook that is referred during processing to allow a user to define their own.
   virtual double ExtendedDrawDefinition(const std::string& aTarget = {});

protected:
   //! Utility method for code reuse when parsing frequency input. Virtual to allow user redefinition
   //! to support input parsing of the cOTHER frequency. Within the base implementation, usage of cOTHER
   //! will throw an exception.
   virtual void ProcessFrequencyInput(WsfRandomVariable&            aRandomVariable,
                                      random::ProbabilityFrequency& aFrequency,
                                      UtInput&                      aInput);

private:
   //! @name Default probabilities.
   //! These values will be used as the defaults if the corresponding values the 'attack' sub-block in
   //! the 'cyber_protect' block was not assigned a value by the user.
   //@{
   double mProbabilityOfScanDetection{0.0};
   double mProbabilityOfScanAttribution{0.0};
   double mProbabilityOfAttackSuccess{1.0};
   double mProbabilityOfStatusReport{1.0};
   double mProbabilityOfAttackDetection{0.0};
   double mProbabilityOfAttackAttribution{0.0};
   double mProbabilityOfFutureImmunity{0.0};
   //@}

   //! @name Attack duration
   //! This value is used to keep track of how long a cyber attack lasts against a victim.
   double mDuration{0.0};

   //! @name Attack resource requirements
   double mResourcesRequired{0.0};

   //! @name Timed delays
   //! These member variables correspond to the named delays in time defined on the attack.
   //! @note These are mutable to provide the correct interface while still allowing
   //! draws to occur (which mutate the WsfRandomVariable object).
   //@{
   mutable WsfRandomVariable mTimeDelayScan{WsfRandomVariable::cCONSTANT, 0.0, 0.0, WsfRandomVariable::cNON_NEGATIVE};
   mutable WsfRandomVariable mTimeDelayDelivery{WsfRandomVariable::cCONSTANT, 0.0, 0.0, WsfRandomVariable::cNON_NEGATIVE};
   //@}

   //! @name Draw Frequencies.
   //! The enumerations relate to the frequency in which a draw will be generated for the corresponding
   //! probabilities.
   //@{
   random::ProbabilityFrequency mFrequencyOfScanDetection{random::cALWAYS};
   random::ProbabilityFrequency mFrequencyOfScanAttribution{random::cALWAYS};
   random::ProbabilityFrequency mFrequencyOfAttackSuccess{random::cALWAYS};
   random::ProbabilityFrequency mFrequencyOfStatusReport{random::cALWAYS};
   random::ProbabilityFrequency mFrequencyOfAttackDetection{random::cALWAYS};
   random::ProbabilityFrequency mFrequencyOfAttackAttribution{random::cALWAYS};
   random::ProbabilityFrequency mFrequencyOfFutureImmunity{random::cALWAYS};
   //@}

   //! @name Random Time Values
   //! These objects reference the random time value for a time interval frequency,
   //! if utilized with the interval frequency definitions.
   //! @note These are mutable to provide the correct interface while still allowing
   //! draws to occur (which mutate the WsfRandomVariable object).
   //@{
   mutable WsfRandomVariable mRandomVariableScanDetection{};
   mutable WsfRandomVariable mRandomVariableScanAttribution{};
   mutable WsfRandomVariable mRandomVariableAttackSuccess{};
   mutable WsfRandomVariable mRandomVariableStatusReport{};
   mutable WsfRandomVariable mRandomVariableAttackDetection{};
   mutable WsfRandomVariable mRandomVariableAttackAttribution{};
   mutable WsfRandomVariable mRandomVariableFutureImmunity{};
   //@}

   //! The list of cyber effect type names that define the cyber attack.
   //! This is stored as a list of type names and not pointers to the types because:
   //! - It makes the copy constructor easier.
   //! - The actual effect only needs to be created when actually part of an attack.
   std::vector<std::string> mEffectTypes;
};

} // namespace cyber
} // namespace wsf

#endif
