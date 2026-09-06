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

#include "WsfEW_Result.hpp"

#include <algorithm>

#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfEW_Effect.hpp"

// =================================================================================================
WsfEW_Result* WsfEW_Result::Find(const WsfEM_Interaction& aEM_Interaction)
{
   WsfEW_Result* componentPtr = nullptr;
   aEM_Interaction.GetComponents().FindByRole(componentPtr);
   return componentPtr;
}

// =================================================================================================
WsfEW_Result& WsfEW_Result::FindOrCreate(WsfEM_Interaction& aResult)
{
   WsfEW_Result* resultPtr = Find(aResult);
   if (resultPtr == nullptr)
   {
      resultPtr = new WsfEW_Result;
      aResult.AddComponent(resultPtr);
   }
   return *resultPtr;
}

// =================================================================================================
WsfComponent* WsfEW_Result::CloneComponent() const
{
   return new WsfEW_Result(*this);
}

// =================================================================================================
WsfStringId WsfEW_Result::GetComponentName() const
{
   return UtStringIdLiteral("ew_result");
}

// =================================================================================================
const int* WsfEW_Result::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_EW_RESULT, 0};
   return roles;
}

// =================================================================================================
void* WsfEW_Result::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_EW_RESULT)
   {
      return this;
   }
   return nullptr;
}


// =================================================================================================
//! Print the result data to stdout.
// virtual
void WsfEW_Result::Print(ut::log::MessageStream& aMsgStream, WsfEM_Interaction& aEM_Interaction)
{
   auto& out = aMsgStream;
   if ((mNoiseJammerPower > 0.0) || (mPulseJammerPower > 0.0))
   {
      if ((mNoiseJammerPower > 0.0) && (mPulseJammerPower > 0.0))
      {
         out.AddNote() << "Noise_Jammer_Power: " << UtMath::LinearToDB(mNoiseJammerPower + mPulseJammerPower) << " dBw";
      }
      else if (mNoiseJammerPower > 0.0)
      {
         out.AddNote() << "Noise_Jammer_Power: " << UtMath::LinearToDB(mNoiseJammerPower) << " dBw";
      }
      else if (mPulseJammerPower > 0.0)
      {
         out.AddNote() << "Noise_Jammer_Power: " << UtMath::LinearToDB(mPulseJammerPower) << " dBw";
      }
      if (mPulseJammerPower > 0.0)
      {
         out.AddNote() << "Pulse_Jammer_Power: " << UtMath::LinearToDB(mPulseJammerPower) << " dBw";
      }
      if (mCoherentJammerPower > 0.0)
      {
         out.AddNote() << "Coherent_Jammer_Power: " << UtMath::LinearToDB(mCoherentJammerPower) << " dBw";
      }
   }

   if ((aEM_Interaction.mFailedStatus == 0) && (mEW_Effects.mMask != 0))
   {
      // More Printing to be fixed
      mEW_Effects.Print(out);
   }
}

// =================================================================================================
// virtual
void WsfEW_Result::PrintStatus(ut::log::MessageStream& aMsgStream, WsfEM_Interaction& aEM_Interaction)
{
   if (mPerceivesContJamming)
   {
      aMsgStream.AddNote() << "Continuous Jamming Perceived.";
   }
   if (mPerceivesPulseJamming)
   {
      aMsgStream.AddNote() << "Pulse Jamming Perceived.";
   }
   if (mPerceivesCoherentJamming)
   {
      aMsgStream.AddNote() << "Coherent Jamming Perceived.";
   }
}

// =================================================================================================
//! Reset the detection data for a new detection chance.
//!
//! This only resets those values that are absolutely necessary. Values whose
//! validity can be determined by looking at other values are NOT reset.
// virtual
void WsfEW_Result::Reset()
{
   mEW_Effects.Reset();
   mNoiseJammerPower         = 0.0;
   mPulseJammerPower         = 0.0;
   mCoherentJammerPower      = 0.0;
   mPerceivesCoherentJamming = false;
   mPerceivesContJamming     = false;
   mPerceivesPulseJamming    = false;
}

// =================================================================================================
// Called from WsfEventOutput::PrintEM_Interaction.
bool WsfEW_Result::PrintEventOutput(std::ostream& aStream)
{
   if (mEW_Effects.mMask != 0)
   {
      aStream << "  EW_Effects:";
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_UNDEFINED) != 0)
      {
         aStream << " Undefined";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_DROP_TRACK) != 0)
      {
         aStream << " Drop_Track";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_MAINTAIN_TRACK) != 0)
      {
         aStream << " Maintain_Track";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_TRACK_ERROR) != 0)
      {
         aStream << " Track_Error";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_DROP_MESSAGE) != 0)
      {
         aStream << " Drop_Message";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_MAINTAIN_MESSAGE) != 0)
      {
         aStream << " Maintain_Message";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_MESSAGE_ERROR) != 0)
      {
         aStream << " Message_Error";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_JAMMER_POWER_EFFECT) != 0)
      {
         aStream << " Jammer_Power_Effect";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_SIGNAL_POWER_EFFECT) != 0)
      {
         aStream << " Signal_Power_Effect";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_SLC_DEGRADE_EFFECT) != 0)
      {
         aStream << " Jammer_Power_Effect";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_FALSE_TARGET_EFFECT) != 0)
      {
         aStream << " False_Target_Effect";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_PULSE_EFFECT) != 0)
      {
         aStream << " Pulse_Effect";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_AGILITY_EFFECT) != 0)
      {
         aStream << " Agility_Effect";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_FLOODED_BLOCKED) != 0)
      {
         aStream << " Flooded_Blocked";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_REPEATER_EFFECT) != 0)
      {
         aStream << " Repeater_Effect";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_REPEATER_UPDATE) != 0)
      {
         aStream << " Repeater_Update";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_FLOODED_MAINTAINED) != 0)
      {
         aStream << " Flooded_Maintained";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_JAM_STROBE_EFFECT) != 0)
      {
         aStream << " Jam_Strobe_Effect";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_TARGET_REJECT_EFFECT) != 0)
      {
         aStream << " Target_Reject_Effect";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_NEXT_FREE_MASK) != 0)
      {
         aStream << " UNKNOWN";
      }
      return true;
   }
   return false;
}

// =================================================================================================
// Called from WsfEventOutput::PrintEM_Interaction.
void WsfEW_Result::PrintEventOutput_Powers(std::ostream& aStream)
{
   // if ((mNoiseJammerPower > 0.0) || (mPulseJammerPower > 0.0))
   //{
   //    if ((mNoiseJammerPower > 0.0) && (mPulseJammerPower > 0.0))
   //    {
   //       aStream << " Noise_Jammer_Power: "
   //          << UtMath::LinearToDB(mNoiseJammerPower + mPulseJammerPower) << " dBw";
   //    }
   //    else if (mNoiseJammerPower > 0.0)
   //    {
   //       aStream << " Noise_Jammer_Power: "
   //          << UtMath::LinearToDB(mNoiseJammerPower) << " dBw";
   //    }
   //    else if (mPulseJammerPower > 0.0)
   //    {
   //       aStream << " Noise_Jammer_Power: "
   //          << UtMath::LinearToDB(mPulseJammerPower) << " dBw";
   //    }
   // }
   if (mPulseJammerPower > 0.0)
   {
      aStream << " (Pulse_Jammer_Power: " << UtMath::LinearToDB(mPulseJammerPower) << " dBw)";
   }
   if (mCoherentJammerPower > 0.0)
   {
      aStream << " Coherent_Jammer_Power: " << UtMath::LinearToDB(mCoherentJammerPower) << " dBw";
   }
}

bool WsfEW_Result::PrintCSV_EventOutput(std::ostream& aStream)
{
   std::string ewEffects;
   if (mEW_Effects.mMask > 0)
   {
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_DROP_TRACK) != 0)
      {
         ewEffects += "Drop_Track ";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_MAINTAIN_TRACK) != 0)
      {
         ewEffects += "Maintain_Track ";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_TRACK_ERROR) != 0)
      {
         ewEffects += "Track_Error ";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_FLOODED_BLOCKED) != 0)
      {
         ewEffects += "Flooded_Blocked ";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_FLOODED_MAINTAINED) != 0)
      {
         ewEffects += "Flooded_Maintained ";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_DROP_MESSAGE) != 0)
      {
         ewEffects += "Drop_Message ";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_MAINTAIN_MESSAGE) != 0)
      {
         ewEffects += "Maintain_Message ";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_MESSAGE_ERROR) != 0)
      {
         ewEffects += "Message_Error ";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_JAMMER_POWER_EFFECT) != 0)
      {
         ewEffects += "Jammer_Power_Effect ";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_PULSE_EFFECT) != 0)
      {
         ewEffects += "Pulse_Effect ";
      }
      if ((mEW_Effects.mMask & WsfEW_Effect::cEB_FALSE_TARGET_EFFECT) != 0)
      {
         ewEffects += "False_Target ";
      }
   }
   aStream << ',' << ewEffects;
   return true;
}

// =================================================================================================
// Called from WsfCSV_EventOutput::PrintEM_Interaction.
bool WsfEW_Result::PrintCSV_EventOutput_Powers(std::ostream& aStream)
{
   aStream << ',';
   if ((mNoiseJammerPower > 0.0) || (mPulseJammerPower > 0.0))
   {
      if ((mNoiseJammerPower > 0.0) && (mPulseJammerPower > 0.0))
      {
         aStream << UtMath::LinearToDB(mNoiseJammerPower + mPulseJammerPower); // Noise_JammerPower(dBw)
      }
      else if (mNoiseJammerPower > 0.0)
      {
         aStream << UtMath::LinearToDB(mNoiseJammerPower); // Noise_JammerPower(dBw)
      }
      else if (mPulseJammerPower > 0.0)
      {
         aStream << UtMath::LinearToDB(mPulseJammerPower); // Noise_JammerPower(dBw)
      }
   }

   aStream << ',';
   if (mPulseJammerPower > 0.0)
   {
      aStream << UtMath::LinearToDB(mPulseJammerPower); // Pulse_Jammer_Power dBw
   }

   aStream << ',';
   if (mCoherentJammerPower > 0.0)
   {
      aStream << UtMath::LinearToDB(mCoherentJammerPower); // Coherent_Jammer_Power dBw
   }
   return true;
}
