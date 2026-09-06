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

#include "WsfEW_Effect.hpp"

#include <limits>
#include <memory>
#include <string>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "WsfArticulatedPart.hpp"
#include "WsfDeferredInput.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEW_EA.hpp"
#include "WsfEW_EP.hpp"
#include "WsfEW_Effects.hpp"
#include "WsfEW_Technique.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

WsfEW_Effect::WsfEW_Effect(const WsfEW_Effect& aSrc)
   : WsfObject(aSrc)
   , mSystemFunctionMask(aSrc.mSystemFunctionMask)
   , mBehavior(aSrc.mBehavior)
   , mCoherencyMask(aSrc.mCoherencyMask)
   , mDebug(aSrc.mDebug)
   , mCurrentTargetId(aSrc.mCurrentTargetId)
   , mCurrentXmtrPlatformId(aSrc.mCurrentXmtrPlatformId)
   , mCurrentRcvrPlatformId(aSrc.mCurrentRcvrPlatformId)
   , mCurrentXmtrUniqueId(aSrc.mCurrentXmtrUniqueId)
   , mCurrentRcvrUniqueId(aSrc.mCurrentRcvrUniqueId)
   , mEffectedSystemId(aSrc.mEffectedSystemId)
   , mEffectingSystemId(aSrc.mEffectingSystemId)
   , mUpdatingEffect(aSrc.mUpdatingEffect)
   , mTechniquePtr(aSrc.mTechniquePtr)
   , mEA_EP_Ptr(nullptr)
   , mProtectionType(aSrc.mProtectionType)
   , mAllowedTargetSet(aSrc.mAllowedTargetSet)
   , mRejectedTargetSet(aSrc.mRejectedTargetSet)
   , mRandom()
{
   auto edmi = aSrc.mEffectDataMap.begin();
   for (; edmi != aSrc.mEffectDataMap.end(); ++edmi)
   {
      mEffectDataMap[edmi->first] = dynamic_cast<WsfEW_Effect::EffectData*>(edmi->second->Clone());
   }
}

// virtual
WsfEW_Effect::~WsfEW_Effect()
{
   auto edmi = mEffectDataMap.begin();
   for (; edmi != mEffectDataMap.end(); ++edmi)
   {
      delete edmi->second;
   }
}

bool WsfEW_Effect::Initialize(WsfSimulation& aSimulation)
{
   bool ok = true;

   mUpdatingEffect = false;

   // Initialize the random number stream.  Each effect has a different seed.
   long int seed = aSimulation.LockRandom().Uniform<long>();
   aSimulation.UnlockRandom();
   mRandom.SetSeed(seed);

   if ((mSystemFunctionMask & cES_UNDEFINED) != 0u)
   {
      if (GetEP_Ptr() != nullptr)
      {
         // Affects a jammer
         mSystemFunctionMask = cES_JAMMER;
      }
      // else if (GetEA_Ptr() != 0)
      //{
      //  Set in the affect itself as it could be a cES_COMM or cES_SENSOR or both.
      //}
   }

   // Check for default and add if it doesn't exist.
   if (!EffectDataExists(nullptr))
   {
      mEffectDataMap[nullptr] = NewEffectData();
   }

   EffectDataMap::const_iterator edmi = mEffectDataMap.begin();
   for (; edmi != mEffectDataMap.end(); ++edmi)
   {
      PropagateEffectDataPtr(edmi->first); // Setting local effect data maps to current base before initializing
      edmi->second->SetEffectPtr(this);
      ok &= edmi->second->Initialize(aSimulation);
   }

   return ok;
}

bool WsfEW_Effect::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;

   // Check for default and add if it doesn't exist.
   if (!EffectDataExists(nullptr))
   {
      mEffectDataMap[nullptr] = NewEffectData();
   }

   std::string command(aInput.GetCommand());
   if (command == "effect_name")
   {
      std::string name;
      aInput.ReadValue(name);

      if (GetNameId() == nullptr)
      {
         SetName(name);
      }
      else
      {
         throw UtInput::BadValue(aInput, "The Effect already has a name, " + GetName() + " cannot overwrite with " + name);
      }
   }
   else if (command == "debug")
   {
      mDebug = true;
   }
   else if (command == "system_type_data")
   {
      std::string typeName;
      aInput.ReadValue(typeName);
      WsfStringId typeId;
      if (typeName != "default")
      {
         typeId = typeName;
      }

      UtInputBlock inputBlock(aInput, "end_system_type_data");
      if (!ProcessEffectDataBlock(inputBlock, typeId))
      {
         myCommand = false;
      }
   }
   else if (command == "target_protection_type")
   {
      std::string protectionTypeString;
      aInput.ReadValue(protectionTypeString);
      if (protectionTypeString == "all_targets")
      {
         mProtectionType = cTP_ALL;
      }
      else if (protectionTypeString == "self_protect")
      {
         mProtectionType = cTP_SELF_PROTECT;
      }
      else if (protectionTypeString == "non_self_protect")
      {
         mProtectionType = cTP_NON_SELF_PROTECT;
      }
   }
   else if (command == "allowed_target_set")
   {
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if ((command == "type") || (command == "name"))
         {
            std::string targetNameType;
            aInput.ReadCommand(targetNameType);
            mAllowedTargetSet.insert(targetNameType);
         }
      }
   }
   else if (command == "rejected_target_set")
   {
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if ((command == "type") || (command == "name"))
         {
            std::string targetNameType;
            aInput.ReadCommand(targetNameType);
            mRejectedTargetSet.insert(targetNameType);
         }
      }
   }
   else if (!mEffectDataMap[nullptr]->ProcessInput(aInput)) // Process the 'default' (system type 0) data
   {
      myCommand = false;
   }

   return myCommand;
}

// virtual
void WsfEW_Effect::ProcessUnmitigatedEffect(double                                 aSimTime,
                                            const WsfEM_Interaction&               aTargetInteraction,
                                            const WsfEM_Interaction&               aJammerToTgtInteraction,
                                            const ut::optional<WsfEM_Interaction>& aTgtToJammerInteraction,
                                            WsfEW_Effects&                         aEW_Effects)
{
   if (!mUpdatingEffect)
   {
      WsfEM_Xmtr* jammerXmtrPtr = aJammerToTgtInteraction.GetTransmitter();
      ResetProtectionTypeFlag(jammerXmtrPtr, aTargetInteraction, aEW_Effects);
      if (AllowTarget(jammerXmtrPtr->GetPlatform(), aTargetInteraction.GetTarget()))
      {
         mUpdatingEffect = true;
         UpdateEffects(aSimTime, aTargetInteraction, aJammerToTgtInteraction, aTgtToJammerInteraction, aEW_Effects);
         ApplyUnmitigatedEffect(aEW_Effects);
         mUpdatingEffect = false;
      }
      else if (mDebug)
      {
         WsfEM_Rcvr* radarRcvrPtr = aTargetInteraction.GetReceiver();
         assert(radarRcvrPtr != nullptr);

         // Set some variables as this is the first time they can be set.
         UpdateCurrentIds(radarRcvrPtr, jammerXmtrPtr, aTargetInteraction.GetTarget());

         auto out = ut::log::debug() << "Effect not applied due to target being rejected.";
         PrintUpdateMessage(out, aSimTime);
      }
   }
}

// virtual
void WsfEW_Effect::ProcessMitigatedEffect(double                                 aSimTime,
                                          const WsfEM_Interaction&               aTargetInteraction,
                                          const WsfEM_Interaction&               aJammerToTgtInteraction,
                                          const ut::optional<WsfEM_Interaction>& aTgtToJammerInteraction,
                                          WsfEW_Effects&                         aEW_Effects,
                                          WsfEW_Effect*                          aEW_EffectPtr /* = 0*/)
{
   if (!mUpdatingEffect)
   {
      WsfEM_Xmtr* xmtrPtr = aJammerToTgtInteraction.GetTransmitter();
      if ((aEW_EffectPtr == nullptr) || aEW_EffectPtr->AllowTarget(xmtrPtr->GetPlatform(), aTargetInteraction.GetTarget()))
      {
         mUpdatingEffect = true;
         UpdateEffects(aSimTime, aTargetInteraction, aJammerToTgtInteraction, aTgtToJammerInteraction, aEW_Effects, aEW_EffectPtr);
         ApplyMitigatedEffect(aEW_EffectPtr, aEW_Effects);
         mUpdatingEffect = false;
      }
   }
}

//! A convenience function that converts a receiver's function to the appropriate system function.
//!
//! @param aRcvrFunction The receiver's function (i.e. comm, sensor, jammer, etc.).
//! @return Returns the corresponding system type.
// static
WsfEW_Effect::SystemFunction WsfEW_Effect::ConvertToEW_SystemFunction(WsfEM_Rcvr::RcvrFunction aRcvrFunction)
{
   SystemFunction ewFunction = cES_UNDEFINED;

   switch (aRcvrFunction)
   {
   case WsfEM_Rcvr::cRF_COMM:
      ewFunction = cES_COMM;
      break;
   case WsfEM_Rcvr::cRF_SENSOR:
      ewFunction = cES_SENSOR;
      break;
   case WsfEM_Rcvr::cRF_PASSIVE_SENSOR:
      ewFunction = cES_SENSOR;
      break;
   case WsfEM_Rcvr::cRF_INTERFERER:
      ewFunction = cES_JAMMER;
      break;
   default:
      break;
   }
   return ewFunction;
}

//! A convenience function that converts a transmitter's function to the appropriate system function.
//!
//! @param aXmtrFunction The transmitter's function (i.e. comm, sensor, jammer, etc.).
//! @return Returns the corresponding system type.
// static
WsfEW_Effect::SystemFunction WsfEW_Effect::ConvertToEW_SystemFunction(WsfEM_Xmtr::XmtrFunction aXmtrFunction)
{
   SystemFunction ewFunction = cES_UNDEFINED;

   switch (aXmtrFunction)
   {
   case WsfEM_Xmtr::cXF_COMM:
      ewFunction = cES_COMM;
      break;
   case WsfEM_Xmtr::cXF_SENSOR:
      ewFunction = cES_SENSOR;
      break;
   case WsfEM_Xmtr::cXF_INTERFERER:
      ewFunction = cES_JAMMER;
      break;
   default:
      break;
   }
   return ewFunction;
}

// static
unsigned int WsfEW_Effect::ConvertCoherencyIdToMask(WsfStringId aCoherencyId)
{
   unsigned int coherencyMask = cEC_NONE;

   const std::string& typeStr = aCoherencyId;
   if ((typeStr == "all") || (typeStr == "ALL"))
   {
      coherencyMask = cEC_NONE | cEC_NONCOHERENT | cEC_COHERENT | cEC_NONCOHERENT_PULSE | cEC_COHERENT_PULSE;
   }
   else if ((typeStr == "none") || (typeStr == "NONE"))
   {
      coherencyMask = cEC_NONE;
   }
   else if ((typeStr == "noncoherent") || (typeStr == "NONCOHERENT") || (typeStr == "non-coherent") ||
            (typeStr == "NON-COHERENT") || (typeStr == "non_coherent") || (typeStr == "NON_COHERENT") ||
            (typeStr == "noise"))
   {
      coherencyMask = cEC_NONCOHERENT;
   }
   else if ((typeStr == "coherent") || (typeStr == "COHERENT"))
   {
      coherencyMask = cEC_COHERENT;
   }
   else if ((typeStr == "coherent-pulse") || (typeStr == "COHERENT-PULSE") || (typeStr == "coherent_pulse") ||
            (typeStr == "COHERENT_PULSE") || (typeStr == "coherentpulse") || (typeStr == "COHERENTPULSE"))
   {
      coherencyMask = cEC_COHERENT_PULSE;
   }
   else if ((typeStr == "noncoherentpulse") || (typeStr == "NONCOHERENTPULSE") || (typeStr == "non-coherent-pulse") ||
            (typeStr == "NON-COHERENT-PULSE") || (typeStr == "non_coherent_pulse") ||
            (typeStr == "NON_COHERENT_PULSE") || (typeStr == "non-coherent_pulse") || (typeStr == "NON-COHERENT_PULSE") ||
            (typeStr == "non_coherent-pulse") || (typeStr == "NON_COHERENT-PULSE") || (typeStr == "pulse") ||
            (typeStr == "pulse_noise") || (typeStr == "pulsed") || (typeStr == "pulsed_noise"))
   {
      coherencyMask = cEC_NONCOHERENT_PULSE;
   }
   else if ((typeStr == "noise_and_pulse") || (typeStr == "noise_and_pulsed"))
   {
      coherencyMask = cEC_NONCOHERENT | cEC_NONCOHERENT_PULSE;
   }
   else
   {
      auto out = ut::log::warning() << "WsfEW_Effect::ConvertCoherencyIdToMask: Undefined coherency type string.";
      out.AddNote() << "Type String: " << typeStr;
   }
   return coherencyMask;
}

WsfEW_EA* WsfEW_Effect::GetEA_Ptr() const
{
   return dynamic_cast<WsfEW_EA*>(mEA_EP_Ptr);
}

WsfEW_EP* WsfEW_Effect::GetEP_Ptr() const
{
   return dynamic_cast<WsfEW_EP*>(mEA_EP_Ptr);
}

// virtual
void WsfEW_Effect::UpdateEffects(double                                 aSimTime,
                                 const WsfEM_Interaction&               aTargetInteraction,
                                 const WsfEM_Interaction&               aJammerToTgtInteraction,
                                 const ut::optional<WsfEM_Interaction>& aTgtToJammerInteraction,
                                 WsfEW_Effects&                         aEW_Effects,
                                 WsfEW_Effect*                          aEW_EffectPtr /*= 0*/)
{
   WsfEM_Xmtr* xmtrPtr      = aJammerToTgtInteraction.GetTransmitter();
   WsfEM_Rcvr* radarRcvrPtr = aTargetInteraction.GetReceiver();
   assert(radarRcvrPtr != nullptr);

   // Set some variables as this is the first time they can be set.
   UpdateCurrentIds(radarRcvrPtr, xmtrPtr, aTargetInteraction.GetTarget());

   if (mDebug)
   {
      auto out = ut::log::debug() << "Effects updated.";
      PrintUpdateMessage(out, aSimTime, aEW_EffectPtr);
   }
}

//! Remove the specified jammer from the effect and associated classes.
//!
//! @param aXmtrId The unique Id of the jammer transmitter.
// virtual
void WsfEW_Effect::RemoveJammerXmtr(unsigned int aXmtrId) {}

//! Remove the specified jammer from the effect and associated classes.
//!
//! @param aJammerXmtrPtr The pointer to the jammer transmitter.
// virtual
void WsfEW_Effect::RemoveJammerXmtr(WsfEM_Xmtr* aJammerXmtrPtr) {}


void WsfEW_Effect::SetSystemFunctionMask(const std::string& aSystemFunctionId)
{
   if ((aSystemFunctionId == "comm") || (aSystemFunctionId == "COMM"))
   {
      mSystemFunctionMask = cES_COMM;
   }
   else if ((aSystemFunctionId == "sensor") || (aSystemFunctionId == "SENSOR"))
   {
      mSystemFunctionMask = cES_SENSOR;
   }
   else if ((aSystemFunctionId == "comm_sensor") || (aSystemFunctionId == "COMM_SENSOR"))
   {
      mSystemFunctionMask = cES_COMM | cES_SENSOR;
   }
   else if ((aSystemFunctionId == "jammer") || (aSystemFunctionId == "JAMMER"))
   {
      mSystemFunctionMask = cES_JAMMER;
   }
   else
   {
      mSystemFunctionMask = cES_UNDEFINED;
   }
}

void WsfEW_Effect::AddSystemFunctionMask(const std::string& aSystemFunctionId)
{
   if ((aSystemFunctionId == "comm") || (aSystemFunctionId == "COMM"))
   {
      mSystemFunctionMask |= cES_COMM;
   }
   else if ((aSystemFunctionId == "sensor") || (aSystemFunctionId == "SENSOR"))
   {
      mSystemFunctionMask |= cES_SENSOR;
   }
   else if ((aSystemFunctionId == "comm_sensor") || (aSystemFunctionId == "COMM_SENSOR"))
   {
      mSystemFunctionMask |= cES_COMM | cES_SENSOR;
   }
   else if ((aSystemFunctionId == "jammer") || (aSystemFunctionId == "JAMMER"))
   {
      mSystemFunctionMask |= cES_JAMMER;
   }
   else
   {
      mSystemFunctionMask |= cES_UNDEFINED;
   }
}

// virtual
bool WsfEW_Effect::AllowTarget(WsfPlatform* aXmtrPlatform, WsfPlatform* aTargetPlatform) const
{
   if (aTargetPlatform == nullptr)
   {
      return true;
   }

   WsfStringId xmtrPltfrmId    = aXmtrPlatform->GetNameId();
   WsfStringId tgtPltfrmId     = aTargetPlatform->GetNameId();
   WsfStringId tgtPltfrmTypeId = aTargetPlatform->GetTypeId();

   bool allowed(true);
   if (((mProtectionType == cTP_SELF_PROTECT) && (xmtrPltfrmId != tgtPltfrmId)) ||
       ((mProtectionType == cTP_NON_SELF_PROTECT) && (xmtrPltfrmId == tgtPltfrmId)))
   {
      allowed = false;
   }
   else if ((!mRejectedTargetSet.empty()) && ((mRejectedTargetSet.find(tgtPltfrmId) != mRejectedTargetSet.end()) ||
                                              (mRejectedTargetSet.find(tgtPltfrmTypeId) != mRejectedTargetSet.end())))
   {
      allowed = false;
   }
   else if ((!mAllowedTargetSet.empty()) && ((mAllowedTargetSet.find(tgtPltfrmId) == mAllowedTargetSet.end()) &&
                                             (mAllowedTargetSet.find(tgtPltfrmTypeId) == mAllowedTargetSet.end())))
   {
      allowed = false;
   }
   return allowed;
}

void WsfEW_Effect::ResetProtectionTypeFlag(WsfEM_Xmtr*              aXmtrPtr,
                                           const WsfEM_Interaction& aTargetInteraction,
                                           WsfEW_Effects&           aEW_Effects)
{
   if (AllowTarget(aXmtrPtr->GetPlatform(), aTargetInteraction.GetTarget()))
   {
      if (((GetCoherencyMask() & cEC_COHERENT) != 0u) || ((GetCoherencyMask() & cEC_COHERENT_PULSE) != 0u))
      {
         if (aEW_Effects.mEW_CoherentJammingEffect.mProtectTarget != 1)
         {
            aEW_Effects.mEW_CoherentJammingEffect.mProtectTarget = 1;
            // if (mDebug)
            //{
            //    cout << "   Coherent Target Protection Flag: Set from 'not set' or 'false' to 'true'." << endl;
            // }
         }
      }

      if (((GetCoherencyMask() & cEC_NONE) != 0u) || ((GetCoherencyMask() & cEC_NONCOHERENT) != 0u))
      {
         if (aEW_Effects.mEW_NoiseJammingEffect.mProtectTarget != 1)
         {
            aEW_Effects.mEW_NoiseJammingEffect.mProtectTarget = 1;
            // if (mDebug)
            //{
            //    cout << "   Noise Target Protection Flag: Set from 'not set' or 'false' to 'true'." << endl;
            // }
         }
      }

      if ((GetCoherencyMask() & cEC_NONCOHERENT_PULSE) != 0u)
      {
         if (aEW_Effects.mEW_PulseJammingEffect.mProtectTarget != 1)
         {
            aEW_Effects.mEW_PulseJammingEffect.mProtectTarget = 1;
            // if (mDebug)
            //{
            //    ut::long::debug << "Pulse Target Protection Flag: Set from 'not set' or 'false' to 'true'.";
            // }
         }
      }
   }
   else
   {
      aEW_Effects.mMask |= cEB_TARGET_REJECT_EFFECT;

      if (((GetCoherencyMask() & cEC_COHERENT) != 0u) || ((GetCoherencyMask() & cEC_COHERENT_PULSE) != 0u))
      {
         if (aEW_Effects.mEW_CoherentJammingEffect.mProtectTarget == -1)
         {
            aEW_Effects.mEW_CoherentJammingEffect.mProtectTarget = 0;
            // if (mDebug)
            //{
            //    ut::log::debug() << "Coherent Target Protection Flag: Set from 'not set' to 'false'.";
            // }
         }
      }

      if (((GetCoherencyMask() & cEC_NONE) != 0u) || ((GetCoherencyMask() & cEC_NONCOHERENT) != 0u))
      {
         if (aEW_Effects.mEW_NoiseJammingEffect.mProtectTarget == -1)
         {
            aEW_Effects.mEW_NoiseJammingEffect.mProtectTarget = 0;
            // if (mDebug)
            //{
            //    ut::log::debug() << "Noise Target Protection Flag: Set from 'not set' to 'false'.";
            // }
         }
      }

      if ((GetCoherencyMask() & cEC_NONCOHERENT_PULSE) != 0u)
      {
         if (aEW_Effects.mEW_PulseJammingEffect.mProtectTarget == -1)
         {
            aEW_Effects.mEW_PulseJammingEffect.mProtectTarget = 0;
            // if (mDebug)
            //{
            //    ut::log::debug() << "Pulse Target Protection Flag: Set from 'not set' to 'false'.";
            // }
         }
      }
   }
}

// virtual
WsfEW_Effect::EffectData& WsfEW_Effect::GetEffectData(WsfStringId aSystemTypeId)
{
   if (EffectDataExists(aSystemTypeId))
   {
      return *mEffectDataMap[aSystemTypeId];
   }
   return *mEffectDataMap[nullptr];
}

bool WsfEW_Effect::ProcessEffectDataBlock(UtInputBlock& aInputBlock, WsfStringId aSystemTypeId)
{
   bool myCommand = true;

   if (!EffectDataExists(aSystemTypeId))
   {
      mEffectDataMap[aSystemTypeId] = NewEffectData();
   }

   while (aInputBlock.ReadCommand())
   {
      if (!mEffectDataMap[aSystemTypeId]->ProcessInput(aInputBlock.GetInput()))
      {
         throw UtInput::UnknownCommand(aInputBlock.GetInput());
      }
   }
   return myCommand;
}

bool WsfEW_Effect::EffectDataExists(WsfStringId aSystemTypeId) const
{
   return (mEffectDataMap.find(aSystemTypeId) != mEffectDataMap.end());
}

void WsfEW_Effect::InsertNewEffectData(WsfStringId aSystemTypeId, WsfEW_Types::EW_EffectData* aEffectPtr)
{
   if (mEffectDataMap.find(aSystemTypeId) != mEffectDataMap.end())
   {
      if (mEffectDataMap[aSystemTypeId] != aEffectPtr)
      {
         delete mEffectDataMap[aSystemTypeId];
      }
   }
   mEffectDataMap[aSystemTypeId] = dynamic_cast<EffectData*>(aEffectPtr);
   PropagateEffectDataPtr(aSystemTypeId);
}

// virtual
WsfEW_Effect::EffectData* WsfEW_Effect::PropagateEffectDataPtr(WsfStringId aSystemTypeId)
{
   if (!EffectDataExists(aSystemTypeId))
   {
      auto out = ut::log::error() << "Cannot propagate effect data due to invalid base effect data.";
      out.AddNote() << "Effect: " << GetName();
      return nullptr;
   }
   return mEffectDataMap[aSystemTypeId];
}

// virtual
void WsfEW_Effect::UpdateCurrentIds(WsfEM_Rcvr* aRcvrPtr, WsfEM_Xmtr* aXmtrPtr, WsfPlatform* aTargetPtr)
{
   mCurrentRcvrPlatformId = aRcvrPtr->GetPlatform()->GetNameId();
   mCurrentXmtrPlatformId = aXmtrPtr->GetPlatform()->GetNameId();
   mCurrentTargetId       = aTargetPtr != nullptr ? aTargetPtr->GetNameId() : nullptr;
   mCurrentXmtrUniqueId   = aXmtrPtr->GetUniqueId();
   mCurrentRcvrUniqueId   = aRcvrPtr->GetUniqueId();

   mEffectedSystemId  = nullptr;
   mEffectingSystemId = nullptr;
   if ((GetEA_Ptr() != nullptr) && (GetEP_Ptr() == nullptr))
   {
      mEffectedSystemId  = aRcvrPtr->GetArticulatedPart()->GetTypeId();
      mEffectingSystemId = aXmtrPtr->GetArticulatedPart()->GetTypeId();
   }
   else if ((GetEP_Ptr() != nullptr) && (GetEA_Ptr() == nullptr))
   {
      mEffectedSystemId  = aXmtrPtr->GetArticulatedPart()->GetTypeId();
      mEffectingSystemId = aRcvrPtr->GetArticulatedPart()->GetTypeId();
   }
}

// protected virtual
void WsfEW_Effect::PrintUpdateMessage(ut::log::MessageStream& aStream, double aSimTime, WsfEW_Effect* aEW_EffectPtr /* = 0*/)
{
   aStream.AddNote() << "T = " << aSimTime;

   if (GetEP_Ptr() != nullptr)
   {
      aStream.AddNote() << "EP: " << mCurrentRcvrPlatformId << ":" << GetName();
      aStream.AddNote() << "Jammed by: " << mCurrentXmtrPlatformId;
      if (aEW_EffectPtr != nullptr)
      {
         aStream.AddNote() << "Effect: " << aEW_EffectPtr->GetName();
      }
      aStream.AddNote() << "While attempting to detect: " << mCurrentTargetId.GetString();
   }
   else
   {
      aStream.AddNote() << "EA: " << mCurrentXmtrPlatformId << ":" << GetName();
      aStream.AddNote() << "Jamming: " << mCurrentRcvrPlatformId;
      aStream.AddNote() << "While attempting to detect: " << mCurrentTargetId;
   }
}

WsfSimulation* WsfEW_Effect::GetSimulation() const
{
   return mEA_EP_Ptr != nullptr ? mEA_EP_Ptr->GetSimulation() : nullptr;
}


// Embedded Effect data class
WsfEW_Effect::EffectData::EffectData(const EffectData& aSrc)
   : EW_EffectData(aSrc)
   , mEffectPtr(nullptr)
{
}

// virtual
WsfEW_Types::EW_EffectData* WsfEW_Effect::EffectData::Clone() const
{
   return new EffectData(*this);
}

// virtual
bool WsfEW_Effect::EffectData::ProcessInput(UtInput& aInput)
{
   return EW_EffectData::ProcessInput(aInput);
}

// virtual
bool WsfEW_Effect::EffectData::Initialize(WsfSimulation& aSimulation)
{
   return EW_EffectData::Initialize(aSimulation);
}

// virtual
void WsfEW_Effect::EffectData::Reset()
{
   EW_EffectData::Reset();
}
