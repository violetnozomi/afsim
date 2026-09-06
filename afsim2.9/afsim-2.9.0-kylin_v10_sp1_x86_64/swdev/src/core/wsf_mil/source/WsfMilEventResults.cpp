// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfMilEventResults.hpp"

#include <ostream>

#include "WsfDirectedEnergyWeapon.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEW_Result.hpp"
#include "WsfEventUtils.hpp"
#include "WsfGuidanceComputer.hpp"
#include "WsfImplicitWeapon.hpp"
#include "WsfMilEventUtils.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfWeaponEngagement.hpp"

namespace wsf
{
namespace event
{

void DirectedEnergyWeaponAbortShot::Print(std::ostream& aStream) const
{
   std::string  additionalInfo;
   WsfPlatform* targetPtr = mEngagementPtr->GetTargetPlatform();
   utils::GetAdditionalDE_Info(mWeaponPtr, targetPtr, additionalInfo, mSettings.PrintSingleLinePerEvent());

   utils::PrintEvent(aStream,
                     mSimTime,
                     mWeaponPtr->GetPlatform()->GetName(),
                     "DIRECTED_ENERGY_WEAPON_ABORT_SHOT",
                     utils::GetTargetName(mEngagementPtr->GetTargetPlatformIndex(), *mWeaponPtr->GetSimulation()),
                     mSettings,
                     additionalInfo);
}

void DirectedEnergyWeaponAbortShot::PrintCSV(std::ostream& aStream) const
{
   std::string  additionalInfo;
   WsfPlatform* targetPtr = mEngagementPtr->GetTargetPlatform();
   utilsCSV::GetAdditionalDE_Info(mWeaponPtr, targetPtr, additionalInfo);

   utilsCSV::PrintEvent(aStream,
                        mSimTime,
                        mWeaponPtr->GetPlatform()->GetName(),
                        "DIRECTED_ENERGY_WEAPON_ABORT_SHOT",
                        utilsCSV::GetTargetName(aStream,
                                                mEngagementPtr->GetTargetPlatformIndex(),
                                                *mWeaponPtr->GetSimulation()),
                        additionalInfo);
}

void DirectedEnergyWeaponBeginShot::Print(std::ostream& aStream) const
{
   std::string  additionalInfo;
   WsfPlatform* targetPtr = mEngagementPtr->GetTargetPlatform();
   utils::GetAdditionalDE_Info(mWeaponPtr, targetPtr, additionalInfo, mSettings.PrintSingleLinePerEvent());

   utils::PrintEvent(aStream,
                     mSimTime,
                     mWeaponPtr->GetPlatform()->GetName(),
                     "DIRECTED_ENERGY_WEAPON_BEGIN_SHOT",
                     utils::GetTargetName(mEngagementPtr->GetTargetPlatformIndex(), *mWeaponPtr->GetSimulation()),
                     mSettings,
                     additionalInfo);
}

void DirectedEnergyWeaponBeginShot::PrintCSV(std::ostream& aStream) const
{
   std::string  additionalInfo;
   WsfPlatform* targetPtr = mEngagementPtr->GetTargetPlatform();
   utilsCSV::GetAdditionalDE_Info(mWeaponPtr, targetPtr, additionalInfo);

   utilsCSV::PrintEvent(aStream,
                        mSimTime,
                        mWeaponPtr->GetPlatform()->GetName(),
                        "DIRECTED_ENERGY_WEAPON_BEGIN_SHOT",
                        utilsCSV::GetTargetName(aStream,
                                                mEngagementPtr->GetTargetPlatformIndex(),
                                                *mWeaponPtr->GetSimulation()),
                        additionalInfo);
}

void DirectedEnergyWeaponEndShot::Print(std::ostream& aStream) const
{
   std::string  additionalInfo;
   WsfPlatform* targetPtr = mEngagementPtr->GetTargetPlatform();
   utils::GetAdditionalDE_Info(mWeaponPtr, targetPtr, additionalInfo, mSettings.PrintSingleLinePerEvent());

   utils::PrintEvent(aStream,
                     mSimTime,
                     mWeaponPtr->GetPlatform()->GetName(),
                     "DIRECTED_ENERGY_WEAPON_END_SHOT",
                     utils::GetTargetName(mEngagementPtr->GetTargetPlatformIndex(), *mWeaponPtr->GetSimulation()),
                     mSettings,
                     additionalInfo);
}

void DirectedEnergyWeaponEndShot::PrintCSV(std::ostream& aStream) const
{
   std::string  additionalInfo;
   WsfPlatform* targetPtr = mEngagementPtr->GetTargetPlatform();
   utilsCSV::GetAdditionalDE_Info(mWeaponPtr, targetPtr, additionalInfo);

   utilsCSV::PrintEvent(aStream,
                        mSimTime,
                        mWeaponPtr->GetPlatform()->GetName(),
                        "DIRECTED_ENERGY_WEAPON_END_SHOT",
                        utilsCSV::GetTargetName(aStream,
                                                mEngagementPtr->GetTargetPlatformIndex(),
                                                *mWeaponPtr->GetSimulation()),
                        additionalInfo);
}

void DirectedEnergyWeaponUpdateShot::Print(std::ostream& aStream) const
{
   std::string  additionalInfo;
   WsfPlatform* targetPtr = mEngagementPtr->GetTargetPlatform();
   utils::GetAdditionalDE_Info(mWeaponPtr, targetPtr, additionalInfo, mSettings.PrintSingleLinePerEvent());

   utils::PrintEvent(aStream,
                     mSimTime,
                     mWeaponPtr->GetPlatform()->GetName(),
                     "DIRECTED_ENERGY_WEAPON_UPDATE_SHOT",
                     utils::GetTargetName(mEngagementPtr->GetTargetPlatformIndex(), *mWeaponPtr->GetSimulation()),
                     mSettings,
                     additionalInfo);
}

void DirectedEnergyWeaponUpdateShot::PrintCSV(std::ostream& aStream) const
{
   std::string  additionalInfo;
   WsfPlatform* targetPtr = mEngagementPtr->GetTargetPlatform();
   utilsCSV::GetAdditionalDE_Info(mWeaponPtr, targetPtr, additionalInfo);

   utilsCSV::PrintEvent(aStream,
                        mSimTime,
                        mWeaponPtr->GetPlatform()->GetName(),
                        "DIRECTED_ENERGY_WEAPON_UPDATE_SHOT",
                        utilsCSV::GetTargetName(aStream,
                                                mEngagementPtr->GetTargetPlatformIndex(),
                                                *mWeaponPtr->GetSimulation()),
                        additionalInfo);
}

void DirectedEnergyWeaponCooldownComplete::Print(std::ostream& aStream) const
{
   utils::PrintEvent(aStream,
                     mSimTime,
                     mWeaponPtr->GetPlatform()->GetName(),
                     "DIRECTED_ENERGY_WEAPON_COOLDOWN_COMPLETE",
                     mWeaponPtr->GetName(),
                     mSettings);
}

void DirectedEnergyWeaponCooldownComplete::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintEvent(aStream,
                        mSimTime,
                        mWeaponPtr->GetPlatform()->GetName(),
                        "DIRECTED_ENERGY_WEAPON_COOLDOWN_COMPLETE",
                        mWeaponPtr->GetName());
}

void ImplicitWeaponBeginEngagement::Print(std::ostream& aStream) const
{
   std::string  additionalInfo;
   WsfPlatform* targetPtr = mEngagementPtr->GetTargetPlatform();

   utils::GetAdditionalImplicitInfo(mWeaponPtr, targetPtr, additionalInfo, mSettings.PrintSingleLinePerEvent());

   utils::PrintEvent(aStream,
                     mSimTime,
                     mWeaponPtr->GetPlatform()->GetName(),
                     "IMPLICIT_WEAPON_BEGIN_ENGAGEMENT",
                     utils::GetTargetName(mEngagementPtr->GetTargetPlatformIndex(), *mWeaponPtr->GetSimulation()),
                     mSettings,
                     additionalInfo);
}

void ImplicitWeaponBeginEngagement::PrintCSV(std::ostream& aStream) const
{
   std::string  additionalInfo;
   WsfPlatform* targetPtr = mEngagementPtr->GetTargetPlatform();
   utilsCSV::GetAdditionalImplicitInfo(mWeaponPtr, targetPtr, additionalInfo);

   utilsCSV::PrintEvent(aStream,
                        mSimTime,
                        mWeaponPtr->GetPlatform()->GetName(),
                        "IMPLICIT_WEAPON_BEGIN_ENGAGEMENT",
                        utilsCSV::GetTargetName(aStream,
                                                mEngagementPtr->GetTargetPlatformIndex(),
                                                *mWeaponPtr->GetSimulation()),
                        additionalInfo);
}

void ImplicitWeaponEndEngagement::Print(std::ostream& aStream) const
{
   std::string  additionalInfo;
   WsfPlatform* targetPtr = mEngagementPtr->GetTargetPlatform();
   utils::GetAdditionalImplicitInfo(mWeaponPtr, targetPtr, additionalInfo, mSettings.PrintSingleLinePerEvent());

   utils::PrintEvent(aStream,
                     mSimTime,
                     mWeaponPtr->GetPlatform()->GetName(),
                     "IMPLICIT_WEAPON_END_ENGAGEMENT",
                     utils::GetTargetName(mEngagementPtr->GetTargetPlatformIndex(), *mWeaponPtr->GetSimulation()),
                     mSettings,
                     additionalInfo);
}

void ImplicitWeaponEndEngagement::PrintCSV(std::ostream& aStream) const
{
   std::string  additionalInfo;
   WsfPlatform* targetPtr = mEngagementPtr->GetTargetPlatform();
   utilsCSV::GetAdditionalImplicitInfo(mWeaponPtr, targetPtr, additionalInfo);

   utilsCSV::PrintEvent(aStream,
                        mSimTime,
                        mWeaponPtr->GetPlatform()->GetName(),
                        "IMPLICIT_WEAPON_END_ENGAGEMENT",
                        utilsCSV::GetTargetName(aStream,
                                                mEngagementPtr->GetTargetPlatformIndex(),
                                                *mWeaponPtr->GetSimulation()),
                        additionalInfo);
}

void JammingAttempt::Print(std::ostream& aStream) const
{
   WsfEW_Result* ewResult = WsfEW_Result::Find(mResult);
#undef GetTimeFormat
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "JAMMING_ATTEMPT " << mXmtrPtr->GetPlatform()->GetName() << ' ' << mRcvrPtr->GetPlatform()->GetName();
   aStream << " Xmtr: " << mXmtrPtr->GetArticulatedPart()->GetName() << " BeamNumber: " << mXmtrPtr->GetIndex();
   aStream << " Rcvr: " << mRcvrPtr->GetArticulatedPart()->GetName()
           << utils::ContinueChar(mSettings.PrintSingleLinePerEvent());
   utils::PrintEM_Interaction(aStream, mResult, mSettings);
   if (mResult.mRcvrNoisePower > 0.0)
   {
      if ((ewResult->mNoiseJammerPower > 0.0) || (ewResult->mPulseJammerPower > 0.0))
      {
         if ((ewResult->mNoiseJammerPower > 0.0) && (ewResult->mPulseJammerPower > 0.0))
         {
            aStream << "  Noise_J/N: "
                    << UtMath::LinearToDB((ewResult->mNoiseJammerPower + ewResult->mPulseJammerPower) /
                                          mResult.mRcvrNoisePower)
                    << " dB";
         }
         else if (ewResult->mNoiseJammerPower > 0.0)
         {
            aStream << "  Noise_J/N: " << UtMath::LinearToDB(ewResult->mNoiseJammerPower / mResult.mRcvrNoisePower)
                    << " dB";
         }
         else if (ewResult->mPulseJammerPower > 0.0)
         {
            aStream << "  Noise_J/N: " << UtMath::LinearToDB(ewResult->mPulseJammerPower / mResult.mRcvrNoisePower)
                    << " dB";
         }
      }
      if (ewResult->mCoherentJammerPower > 0.0)
      {
         aStream << "  Coherent_J/N: " << UtMath::LinearToDB(ewResult->mCoherentJammerPower / mResult.mRcvrNoisePower)
                 << " dB";
      }
   }
   aStream << '\n';
}

void JammingAttempt::PrintCSV(std::ostream& aStream) const
{
   WsfEW_Result* ewResult = WsfEW_Result::Find(mResult);
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "JAMMING_ATTEMPT";
   aStream << ',' << mXmtrPtr->GetPlatform()->GetName();
   aStream << ',' << mRcvrPtr->GetPlatform()->GetName();
   aStream << ',' << mXmtrPtr->GetArticulatedPart()->GetName();
   aStream << ',' << mXmtrPtr->GetIndex();
   aStream << ',' << mRcvrPtr->GetArticulatedPart()->GetName();

   utilsCSV::PrintEM_Interaction(aStream, mResult);
   aStream << ',';
   if (mResult.mRcvrNoisePower > 0.0)
   {
      if ((ewResult->mNoiseJammerPower > 0.0) || (ewResult->mPulseJammerPower > 0.0))
      {
         if ((ewResult->mNoiseJammerPower > 0.0) && (ewResult->mPulseJammerPower > 0.0))
         {
            aStream << UtMath::LinearToDB((ewResult->mNoiseJammerPower + ewResult->mPulseJammerPower) /
                                          mResult.mRcvrNoisePower); // Noise_J/N(dB)
         }
         else if (ewResult->mNoiseJammerPower > 0.0)
         {
            aStream << UtMath::LinearToDB(ewResult->mNoiseJammerPower / mResult.mRcvrNoisePower); // Noise_J/N(dB)
         }
         else if (ewResult->mPulseJammerPower > 0.0)
         {
            aStream << UtMath::LinearToDB(ewResult->mPulseJammerPower / mResult.mRcvrNoisePower); // Noise_J/N(dB)
         }
      }

      aStream << ',';
      if ((ewResult != nullptr) && (ewResult->mCoherentJammerPower > 0.0))
      {
         aStream << UtMath::LinearToDB(ewResult->mCoherentJammerPower / mResult.mRcvrNoisePower); //"  Coherent_J/N:(dB)
      }
   }
   else
   {
      aStream << ',';
   }
   aStream << '\n';
}

void JammingRequestCanceled::Print(std::ostream& aStream) const
{
#undef GetTimeFormat
   // line 1
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "JAMMING_REQUEST_CANCELED " << mWeaponPtr->GetPlatform()->GetName();
   aStream << " Weapon: " << mWeaponPtr->GetName() << utils::ContinueChar(mSettings.PrintSingleLinePerEvent());

   // line 2
   aStream << "  Mode: " << mWeaponPtr->GetCurrentModeName();
   aStream << " Active Requests: " << mWeaponPtr->GetActiveRequestCount();

   // Frequency
   if (mFrequency >= 1.0E+9)
   {
      aStream << " Frequency: " << mFrequency * 1.0E-9 << " GHz";
   }
   else if (mFrequency >= 1.0E+6)
   {
      aStream << " Frequency: " << mFrequency * 1.0E-6 << " MHz";
   }
   else if (mFrequency >= 1.0E+3)
   {
      aStream << " Frequency: " << mFrequency * 1.0E-3 << " KHz";
   }
   else
   {
      aStream << " Frequency: " << mFrequency << " Hz";
   }

   // Bandwidth
   if (mBandwidth >= 1.0E+9)
   {
      aStream << " Bandwidth: " << mBandwidth * 1.0E-9 << " GHz";
   }
   else if (mBandwidth >= 1.0E+6)
   {
      aStream << " Bandwidth: " << mBandwidth * 1.0E-6 << " MHz";
   }
   else if (mBandwidth >= 1.0E+3)
   {
      aStream << " Bandwidth: " << mBandwidth * 1.0E-3 << " KHz";
   }
   else
   {
      aStream << " Bandwidth: " << mBandwidth << " Hz";
   }

   // Target
   WsfStringId targetNameId = mWeaponPtr->GetSimulation()->GetPlatformNameId(mTargetIndex);
   if (targetNameId != 0)
   {
      aStream << " Target: " << targetNameId;
   }

   aStream << '\n';
}

void JammingRequestCanceled::PrintCSV(std::ostream& aStream) const
{
   // line 1
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "JAMMING_REQUEST_CANCELED" << ',' << mWeaponPtr->GetPlatform()->GetName();
   aStream << ',' << mWeaponPtr->GetName();

   // line 2
   aStream << ',' << WsfStringId(mWeaponPtr->GetCurrentModeName());
   aStream << ',' << mWeaponPtr->GetActiveRequestCount();

   // Frequency
   aStream << ',' << mFrequency;

   // Bandwidth
   aStream << ',' << mBandwidth;

   // Target
   WsfStringId targetNameId = mWeaponPtr->GetSimulation()->GetPlatformNameId(mTargetIndex);
   aStream << ',';
   if (targetNameId != 0)
   {
      aStream << targetNameId;
   }

   aStream << '\n';
}

void JammingRequestInitiated::Print(std::ostream& aStream) const
{
   // line 1
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "JAMMING_REQUEST_INITIATED " << mWeaponPtr->GetPlatform()->GetName();
   aStream << " Weapon: " << mWeaponPtr->GetName() << utils::ContinueChar(mSettings.PrintSingleLinePerEvent());

   // line 2
   aStream << "  Mode: " << mWeaponPtr->GetCurrentModeName();
   aStream << " Active Requests: " << mWeaponPtr->GetActiveRequestCount();

   // Frequency
   if (mFrequency >= 1.0E+9)
   {
      aStream << " Frequency: " << mFrequency * 1.0E-9 << " GHz";
   }
   else if (mFrequency >= 1.0E+6)
   {
      aStream << " Frequency: " << mFrequency * 1.0E-6 << " MHz";
   }
   else if (mFrequency >= 1.0E+3)
   {
      aStream << " Frequency: " << mFrequency * 1.0E-3 << " KHz";
   }
   else
   {
      aStream << " Frequency: " << mFrequency << " Hz";
   }

   // Bandwidth
   if (mBandwidth >= 1.0E+9)
   {
      aStream << " Bandwidth: " << mBandwidth * 1.0E-9 << " GHz";
   }
   else if (mBandwidth >= 1.0E+6)
   {
      aStream << " Bandwidth: " << mBandwidth * 1.0E-6 << " MHz";
   }
   else if (mBandwidth >= 1.0E+3)
   {
      aStream << " Bandwidth: " << mBandwidth * 1.0E-3 << " KHz";
   }
   else
   {
      aStream << " Bandwidth: " << mBandwidth << " Hz";
   }

   // Target
   WsfStringId targetNameId = mWeaponPtr->GetSimulation()->GetPlatformNameId(mTargetIndex);
   if (targetNameId != 0)
   {
      aStream << " Target: " << targetNameId;
   }

   aStream << '\n';
}

void JammingRequestInitiated::PrintCSV(std::ostream& aStream) const
{
   // line 1
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "JAMMING_REQUEST_INITIATED";
   aStream << ',' << mWeaponPtr->GetPlatform()->GetName();
   aStream << ',' << mWeaponPtr->GetName();

   // line 2
   aStream << ',' << WsfStringId(mWeaponPtr->GetCurrentModeName());
   aStream << ',' << mWeaponPtr->GetActiveRequestCount();

   // Frequency and Bandwidth
   aStream << ',' << mFrequency;
   aStream << ',' << mBandwidth;

   // Target
   WsfStringId targetNameId = mWeaponPtr->GetSimulation()->GetPlatformNameId(mTargetIndex);
   aStream << ',';
   if (targetNameId != 0)
   {
      aStream << targetNameId;
   }

   aStream << '\n';
}

void JammingRequestUpdated::Print(std::ostream& aStream) const
{
   // line 1
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "JAMMING_REQUEST_UPDATED " << mWeaponPtr->GetPlatform()->GetName();
   aStream << " Weapon: " << mWeaponPtr->GetName() << utils::ContinueChar(mSettings.PrintSingleLinePerEvent());

   // line 2
   aStream << "  Mode: " << mWeaponPtr->GetCurrentModeName();
   aStream << " Active Requests: " << mWeaponPtr->GetActiveRequestCount();

   // Frequency
   if (mFrequency >= 1.0E+9)
   {
      aStream << " Frequency: " << mFrequency * 1.0E-9 << " GHz";
   }
   else if (mFrequency >= 1.0E+6)
   {
      aStream << " Frequency: " << mFrequency * 1.0E-6 << " MHz";
   }
   else if (mFrequency >= 1.0E+3)
   {
      aStream << " Frequency: " << mFrequency * 1.0E-3 << " KHz";
   }
   else
   {
      aStream << " Frequency: " << mFrequency << " Hz";
   }

   // Bandwidth
   if (mBandwidth >= 1.0E+9)
   {
      aStream << " Bandwidth: " << mBandwidth * 1.0E-9 << " GHz";
   }
   else if (mBandwidth >= 1.0E+6)
   {
      aStream << " Bandwidth: " << mBandwidth * 1.0E-6 << " MHz";
   }
   else if (mBandwidth >= 1.0E+3)
   {
      aStream << " Bandwidth: " << mBandwidth * 1.0E-3 << " KHz";
   }
   else
   {
      aStream << " Bandwidth: " << mBandwidth << " Hz";
   }

   // Target
   WsfStringId targetNameId = mWeaponPtr->GetSimulation()->GetPlatformNameId(mTargetIndex);
   if (targetNameId != 0)
   {
      aStream << " Target: " << targetNameId;
   }

   aStream << '\n';
}

void JammingRequestUpdated::PrintCSV(std::ostream& aStream) const
{
   // line 1
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "JAMMING_REQUEST_UPDATED";
   aStream << ',' << mWeaponPtr->GetPlatform()->GetName();
   aStream << ',' << mWeaponPtr->GetName();

   // line 2
   aStream << ',' << WsfStringId(mWeaponPtr->GetCurrentModeName());
   aStream << ',' << mWeaponPtr->GetActiveRequestCount();

   // Frequency and Bandwidth
   aStream << ',' << mFrequency;
   aStream << ',' << mBandwidth;

   // Target
   aStream << ',';
   WsfStringId targetNameId = mWeaponPtr->GetSimulation()->GetPlatformNameId(mTargetIndex);
   if (targetNameId != 0)
   {
      aStream << targetNameId;
   }

   aStream << '\n';
}

void PlatformKilled::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "PLATFORM_KILLED ";
   utils::PrintPlatformEvent(aStream, mPlatformPtr, true, mSettings);
   aStream << '\n';
}

void PlatformKilled::PrintCSV(std::ostream& aStream) const
{
   if (mPlatformPtr->GetDamageFactor() >= 1.0)
   {
      utilsCSV::PrintTime(aStream, mSimTime);
      aStream << ',' << "PLATFORM_KILLED" << ',';
      utilsCSV::PrintPlatformEvent(aStream, mPlatformPtr, true);
      aStream << '\n';
   }
}

void GuidanceComputerPhaseChanged::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "MOVER_GUIDANCE_PHASE_CHANGED " << mComputerPtr->GetPlatform()->GetName() << " to "
           << mComputerPtr->GetCurrentPhase().mPhaseName << '\n';
}

void GuidanceComputerPhaseChanged::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "MOVER_GUIDANCE_PHASE_CHANGED";
   aStream << ',' << mComputerPtr->GetPlatform()->GetName();
   aStream << ',' << mComputerPtr->GetCurrentPhase().mPhaseName;
   aStream << '\n';
}

void WeaponFireAborted::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "WEAPON_FIRE_ABORTED " << mWeaponPtr->GetPlatform()->GetName();
   utils::PrintTrackTargetName(aStream, mTargetTrackPtr, *mWeaponPtr->GetSimulation());
   aStream << " Weapon: " << mWeaponPtr->GetName() << " Target_Track: ";
   if (mTargetTrackPtr != nullptr)
   {
      aStream << mTargetTrackPtr->GetTrackId();
   }
   else
   {
      aStream << "<unknown>";
   }
   aStream << " Quantity: " << mQuantity << utils::ContinueChar(mSettings.PrintSingleLinePerEvent());
   utils::PrintLocationData(aStream, mWeaponPtr->GetPlatform(), mSettings);
   aStream << '\n';
}

void WeaponFireAborted::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "WEAPON_FIRE_ABORTED";
   aStream << ',' << mWeaponPtr->GetPlatform()->GetName();
   utilsCSV::PrintTrackTargetName(aStream, mTargetTrackPtr, *mWeaponPtr->GetSimulation());
   aStream << ',' << mWeaponPtr->GetName();
   aStream << ',';
   if (mTargetTrackPtr != nullptr)
   {
      aStream << mTargetTrackPtr->GetTrackId();
   }
   aStream << ',' << mQuantity;
   utilsCSV::PrintLocationData(aStream, mWeaponPtr->GetPlatform());
   aStream << '\n';
}

void WeaponFireRequested::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "WEAPON_FIRE_REQUESTED " << mWeaponPtr->GetPlatform()->GetName();
   utils::PrintTrackTargetName(aStream, mTargetTrackPtr, *mWeaponPtr->GetSimulation());
   aStream << " Weapon: " << mWeaponPtr->GetName() << " Target_Track: ";
   if (mTargetTrackPtr != nullptr)
   {
      aStream << mTargetTrackPtr->GetTrackId();
   }
   else
   {
      aStream << "<unknown>";
   }
   aStream << " Quantity: " << mQuantity << utils::ContinueChar(mSettings.PrintSingleLinePerEvent());
   utils::PrintLocationData(aStream, mWeaponPtr->GetPlatform(), mSettings);
   aStream << '\n';
}

void WeaponFireRequested::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "WEAPON_FIRE_REQUESTED";
   aStream << ',' << mWeaponPtr->GetPlatform()->GetName();
   utilsCSV::PrintTrackTargetName(aStream, mTargetTrackPtr, *mWeaponPtr->GetSimulation());
   aStream << ',' << mWeaponPtr->GetName();
   aStream << ',';
   if (mTargetTrackPtr != nullptr)
   {
      aStream << mTargetTrackPtr->GetTrackId();
   }
   aStream << ',' << mQuantity;
   utilsCSV::PrintLocationData(aStream, mWeaponPtr->GetPlatform());
   aStream << '\n';
}

void WeaponFired::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "WEAPON_FIRED";

   WsfPlatform* firingPtr = nullptr;
   WsfPlatform* weaponPtr = nullptr;
   WsfPlatform* targetPtr = nullptr;

   utils::PrintWE_PlayerData(aStream,
                             mEngagementPtr,
                             nullptr,
                             firingPtr,
                             weaponPtr,
                             targetPtr,
                             *mEngagementPtr->GetSimulation(),
                             mSettings.GetTimeFormat());
   WsfPlatform* launchPtr = firingPtr;
   if (launchPtr == nullptr)
   {
      launchPtr = weaponPtr;
   }
   utils::PrintWE_FiringGeometry(aStream, launchPtr, targetPtr, mSettings);
   if (mTargetTrackPtr != nullptr)
   {
      aStream << utils::ContinueChar(mSettings.PrintSingleLinePerEvent())
              << " Target_Track: " << mTargetTrackPtr->GetTrackId();
      utils::PrintTrackData(aStream, mSimTime, mTargetTrackPtr, *mEngagementPtr->GetSimulation(), mSettings);
   }
   aStream << '\n';
}

void WeaponFired::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "WEAPON_FIRED" << ',';

   WsfPlatform* firingPtr = nullptr;
   WsfPlatform* weaponPtr = nullptr;
   WsfPlatform* targetPtr = nullptr;

   utilsCSV::PrintWE_PlayerData(aStream,
                                mEngagementPtr,
                                nullptr,
                                firingPtr,
                                weaponPtr,
                                targetPtr,
                                *mEngagementPtr->GetSimulation());
   WsfPlatform* launchPtr = firingPtr;
   if (launchPtr == nullptr)
   {
      launchPtr = weaponPtr;
   }
   utilsCSV::PrintWE_FiringGeometry(aStream, launchPtr, targetPtr);
   aStream << ',';
   if (mTargetTrackPtr != nullptr)
   {
      aStream << mTargetTrackPtr->GetTrackId();
      utilsCSV::PrintTrackData(aStream, mSimTime, mTargetTrackPtr, *mEngagementPtr->GetSimulation());
   }
   aStream << '\n';
}

void WeaponHit::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "WEAPON_HIT";
   utils::PrintWE_Termination(aStream, mEngagementPtr, mTargetPtr, *mEngagementPtr->GetSimulation(), mSettings);
}

void WeaponHit::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "WEAPON_HIT" << ',';
   utilsCSV::PrintWE_Termination(aStream,
                                 mEngagementPtr,
                                 mTargetPtr,
                                 *mEngagementPtr->GetSimulation(),
                                 mSettings.GetTimeFormat());
}

void WeaponKilled::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "WEAPON_KILLED ", " Weapon: ", mWeaponPtr, mSettings);
}

void WeaponKilled::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "WEAPON_KILLED", "Weapon", mWeaponPtr);
}

void WeaponModeActivated::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "WEAPON_MODE_ACTIVATED " << mWeaponPtr->GetPlatform()->GetName();
   aStream << " Weapon: " << mWeaponPtr->GetName() << " Mode: " << mModePtr->GetName() << '\n';
}

void WeaponModeActivated::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "WEAPON_MODE_ACTIVATED";
   aStream << ',' << mWeaponPtr->GetPlatform()->GetName();
   aStream << ',' << mWeaponPtr->GetName();
   aStream << ',' << mModePtr->GetName();
   aStream << '\n';
}

void WeaponModeDeactivated::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "WEAPON_MODE_DEACTIVATED " << mWeaponPtr->GetPlatform()->GetName();
   aStream << " Weapon: " << mWeaponPtr->GetName() << " Mode: " << mModePtr->GetName() << '\n';
}

void WeaponModeDeactivated::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "WEAPON_MODE_DEACTIVATED";
   aStream << ',' << mWeaponPtr->GetPlatform()->GetName();
   aStream << ',' << mWeaponPtr->GetName();
   aStream << ',' << mModePtr->GetName();
   aStream << '\n';
}

void WeaponMissed::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "WEAPON_MISSED";
   utils::PrintWE_Termination(aStream, mEngagementPtr, mTargetPtr, *mEngagementPtr->GetSimulation(), mSettings);
}

void WeaponMissed::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "WEAPON_MISSED" << ',';
   utilsCSV::PrintWE_Termination(aStream,
                                 mEngagementPtr,
                                 mTargetPtr,
                                 *mEngagementPtr->GetSimulation(),
                                 mSettings.GetTimeFormat());
}

void WeaponNonOperational::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "WEAPON_NON_OPERATIONAL ", " Weapon: ", mWeaponPtr, mSettings);
}

void WeaponNonOperational::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "WEAPON_NON_OPERATIONAL ", "Weapon", mWeaponPtr);
}

void WeaponOperational::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "WEAPON_OPERATIONAL ", " Weapon: ", mWeaponPtr, mSettings);
}

void WeaponOperational::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "WEAPON_OPERATIONAL", "Weapon", mWeaponPtr);
}

void WeaponReloadStarted::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "WEAPON_RELOAD_STARTED " << mWeaponPtr->GetPlatform()->GetName();
   aStream << " Weapon: " << mWeaponPtr->GetName() << " Quantity: " << mWeaponPtr->GetQuantityRemaining() << '\n';
}

void WeaponReloadStarted::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "WEAPON_RELOAD_STARTED";
   aStream << ',' << mWeaponPtr->GetPlatform()->GetName();
   aStream << ',' << mWeaponPtr->GetName();
   aStream << ',' << mWeaponPtr->GetQuantityRemaining();
   aStream << '\n';
}

void WeaponReloadEnded::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "WEAPON_RELOAD_ENDED " << mWeaponPtr->GetPlatform()->GetName() << " Weapon: " << mWeaponPtr->GetName();
   aStream << " Quantity: " << mWeaponPtr->GetQuantityRemaining() << '\n';
}

void WeaponReloadEnded::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "WEAPON_RELOAD_ENDED";
   aStream << ',' << mWeaponPtr->GetPlatform()->GetName();
   aStream << ',' << mWeaponPtr->GetName();
   aStream << ',' << mWeaponPtr->GetQuantityRemaining();
   aStream << '\n';
}

void WeaponSelected::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "WEAPON_SELECTED " << mWeaponPtr->GetPlatform()->GetName();
   aStream << " Weapon: " << mWeaponPtr->GetName() << utils::ContinueChar(mSettings.PrintSingleLinePerEvent());
   utils::PrintLocationData(aStream, mWeaponPtr->GetPlatform(), mSettings);
   aStream << '\n';
}

void WeaponSelected::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "WEAPON_SELECTED";
   aStream << ',' << mWeaponPtr->GetPlatform()->GetName();
   aStream << ',' << mWeaponPtr->GetName();
   utilsCSV::PrintLocationData(aStream, mWeaponPtr->GetPlatform());
   aStream << '\n';
}

void WeaponTerminated::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "WEAPON_TERMINATED";
   utils::PrintWE_Termination(aStream, mEngagementPtr, nullptr, *mEngagementPtr->GetSimulation(), mSettings);
}

void WeaponTerminated::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "WEAPON_TERMINATED" << ',';
   utilsCSV::PrintWE_Termination(aStream, mEngagementPtr, nullptr, *mEngagementPtr->GetSimulation(), mSettings.GetTimeFormat());
}

void WeaponTurnedOff::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "WEAPON_TURNED_OFF ", " Weapon: ", mWeaponPtr, mSettings);
}

void WeaponTurnedOff::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "WEAPON_TURNED_OFF", "Weapon", mWeaponPtr);
}

void WeaponTurnedOn::Print(std::ostream& aStream) const
{
   utils::PrintSystemOffOnEvent(aStream, mSimTime, "WEAPON_TURNED_ON ", " Weapon: ", mWeaponPtr, mSettings);
}

void WeaponTurnedOn::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintSystemOffOnEvent(aStream, mSimTime, "WEAPON_TURNED_ON", "Weapon", mWeaponPtr);
}

} // namespace event
} // namespace wsf
