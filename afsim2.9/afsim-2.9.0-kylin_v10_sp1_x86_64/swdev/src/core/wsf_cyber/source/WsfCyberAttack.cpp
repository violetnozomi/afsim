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

#include "WsfCyberAttack.hpp"

#include <algorithm>

#include "UtInput.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

namespace
{
wsf::cyber::random::ProbabilityFrequency ReadFrequency(const std::string& aFrequencyString)
{
   if (aFrequencyString == "always")
   {
      return wsf::cyber::random::cALWAYS;
   }

   if (aFrequencyString == "once_per_simulation")
   {
      return wsf::cyber::random::cONCE_PER_SIMULATION;
   }

   if (aFrequencyString == "once_per_target")
   {
      return wsf::cyber::random::cONCE_PER_TARGET;
   }

   if (aFrequencyString == "interval_per_simulation")
   {
      return wsf::cyber::random::cINTERVAL_PER_SIMULATION;
   }

   if (aFrequencyString == "interval_per_target")
   {
      return wsf::cyber::random::cINTERVAL_PER_TARGET;
   }

   // Unsupported in cyber core, but can be used for users wishing to extend.
   return wsf::cyber::random::cOTHER;
}
} // namespace

namespace wsf
{
namespace cyber
{
// =================================================================================================
// virtual
Attack* Attack::Clone() const
{
   return new Attack(*this);
}

// =================================================================================================
// virtual
bool Attack::Initialize(double, WsfSimulation& aSimulation)
{
   bool ok = true;

   ok &= mTimeDelayScan.Initialize("scan_delay_time", nullptr, aSimulation.GetScenario().GetScriptContext(), aSimulation);
   ok &=
      mTimeDelayDelivery.Initialize("delivery_delay_time", nullptr, aSimulation.GetScenario().GetScriptContext(), aSimulation);

   return ok;
}

// =================================================================================================
// virtual
bool Attack::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());
   if (command == "probability_of_scan_detection")
   {
      aInput.ReadValue(mProbabilityOfScanDetection);
      aInput.ValueInClosedRange(mProbabilityOfScanDetection, 0.0, 1.0);
   }
   else if (command == "probability_of_scan_attribution")
   {
      aInput.ReadValue(mProbabilityOfScanAttribution);
      aInput.ValueInClosedRange(mProbabilityOfScanAttribution, 0.0, 1.0);
   }
   else if (command == "probability_of_attack_success")
   {
      aInput.ReadValue(mProbabilityOfAttackSuccess);
      aInput.ValueInClosedRange(mProbabilityOfAttackSuccess, 0.0, 1.0);
   }
   else if (command == "probability_of_status_report")
   {
      aInput.ReadValue(mProbabilityOfStatusReport);
      aInput.ValueInClosedRange(mProbabilityOfStatusReport, 0.0, 1.0);
   }
   else if (command == "probability_of_attack_detection")
   {
      aInput.ReadValue(mProbabilityOfAttackDetection);
      aInput.ValueInClosedRange(mProbabilityOfAttackDetection, 0.0, 1.0);
   }
   else if (command == "probability_of_attack_attribution")
   {
      aInput.ReadValue(mProbabilityOfAttackAttribution);
      aInput.ValueInClosedRange(mProbabilityOfAttackAttribution, 0.0, 1.0);
   }
   else if (command == "probability_of_future_immunity")
   {
      aInput.ReadValue(mProbabilityOfFutureImmunity);
      aInput.ValueInClosedRange(mProbabilityOfFutureImmunity, 0.0, 1.0);
   }
   else if (command == "scan_detection_draw_frequency")
   {
      ProcessFrequencyInput(mRandomVariableScanDetection, mFrequencyOfScanDetection, aInput);
   }
   else if (command == "scan_attribution_draw_frequency")
   {
      ProcessFrequencyInput(mRandomVariableScanAttribution, mFrequencyOfScanAttribution, aInput);
   }
   else if (command == "attack_success_draw_frequency")
   {
      ProcessFrequencyInput(mRandomVariableAttackSuccess, mFrequencyOfAttackSuccess, aInput);
   }
   else if (command == "status_report_draw_frequency")
   {
      ProcessFrequencyInput(mRandomVariableStatusReport, mFrequencyOfStatusReport, aInput);
   }
   else if (command == "attack_detection_draw_frequency")
   {
      ProcessFrequencyInput(mRandomVariableAttackDetection, mFrequencyOfAttackDetection, aInput);
   }
   else if (command == "attack_attribution_draw_frequency")
   {
      ProcessFrequencyInput(mRandomVariableAttackAttribution, mFrequencyOfAttackAttribution, aInput);
   }
   else if (command == "future_immunity_draw_frequency")
   {
      ProcessFrequencyInput(mRandomVariableFutureImmunity, mFrequencyOfFutureImmunity, aInput);
   }
   else if (command == "scan_delay_time")
   {
      mTimeDelayScan.ProcessInput(aInput, UtInput::cTIME);
   }
   else if (command == "delivery_delay_time")
   {
      mTimeDelayDelivery.ProcessInput(aInput, UtInput::cTIME);
   }
   else if (command == "resources_required")
   {
      aInput.ReadValue(mResourcesRequired);
   }
   else if (command == "effect")
   {
      std::string effectType;
      aInput.ReadValue(effectType);
      if (std::find(mEffectTypes.begin(), mEffectTypes.end(), effectType) != mEffectTypes.end())
      {
         throw UtInput::BadValue(aInput, "Duplicate effect specified: " + effectType);
      }

      //! We should check that associated effect types for this attack exist before continuing,
      //! but cannot do so due to the lack of accessibility to the scenario object at this
      //! point. Effects will need to be validated prior to use.
      mEffectTypes.push_back(effectType);
   }
   else if (command == "duration")
   {
      aInput.ReadValueOfType(mDuration, UtInput::cTIME);
   }
   else
   {
      myCommand = WsfObject::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
void Attack::ProcessFrequencyInput(WsfRandomVariable& aRandomVariable, random::ProbabilityFrequency& aFrequency, UtInput& aInput)
{
   std::string frequencyString;
   aInput.ReadValue(frequencyString);
   aFrequency = ReadFrequency(frequencyString);

   if (aFrequency == random::cINTERVAL_PER_SIMULATION || aFrequency == random::cINTERVAL_PER_TARGET)
   {
      aRandomVariable.ProcessInput(aInput, UtInput::cTIME);
   }
   else if (aFrequency == random::cOTHER)
   {
      throw UtInput::BadValue(aInput, "Unsupported draw frequency: " + frequencyString);
   }
}

// =================================================================================================
double Attack::ExtendedDrawDefinition(const std::string&) // = std::string()
{
   // This is an error in core usage.
   throw UtException("Usage of cOTHER Probability Frequency is not supported with base attack objects.");
}

} // namespace cyber
} // namespace wsf
