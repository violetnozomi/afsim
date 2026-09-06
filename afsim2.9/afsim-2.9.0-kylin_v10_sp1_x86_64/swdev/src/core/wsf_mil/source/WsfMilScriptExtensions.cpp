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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "WsfMilScriptExtensions.hpp"

#include <string>

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtScriptClassDefine.hpp"
#include "WsfAcousticSignature.hpp"
#include "WsfAssetPerception.hpp"
#include "WsfComm.hpp"
#include "WsfCommandChain.hpp"
#include "WsfEW_CommComponent.hpp"
#include "WsfEW_Result.hpp"
#include "WsfEW_SensorComponent.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfInfraredSignature.hpp"
#include "WsfInherentContrast.hpp"
#include "WsfLaserDesignator.hpp"
#include "WsfLaserTracker.hpp"
#include "WsfMil.hpp"
#include "WsfOpticalReflectivity.hpp"
#include "WsfOpticalSignature.hpp"
#include "WsfPerceptionProcessor.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"
#include "WsfTaskManager.hpp"
#include "WsfTaskProcessor.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackId.hpp"
#include "WsfUplinkProcessor.hpp"
#include "WsfWeapon.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponObserver.hpp"
#include "WsfWeaponTask.hpp"
#include "WsfWeaponTaskManager.hpp"
#include "WsfWeaponTaskResource.hpp"
#include "script/WsfScriptDefs.hpp"

//! A helper method to get a track ID from a script argument.
//! This method will return the track ID from a script argument. If the pointer to the track ID is null then
//! a null track ID object will be returned. This allows a script user to pass a track ID reference which has
//! not been populated.
WsfTrackId GetTrackId(const UtScriptData& aScriptArg)
{
   WsfTrackId        trackId;
   const WsfTrackId* trackIdPtr = static_cast<const WsfTrackId*>(aScriptArg.GetPointer()->GetAppObject());
   if (trackIdPtr != nullptr)
   {
      trackId = *trackIdPtr;
   }
   return trackId;
}

//! Determine if a platform has any available weapons to deploy.
//! TODO This needs to be extended to also check to see if the associated tracker (if any) is also available.
bool HasOperationalWeapon(WsfPlatform* aPlatformPtr)
{
   for (WsfComponentList::RoleIterator<WsfWeapon> iter(*aPlatformPtr); !iter.AtEnd(); ++iter)
   {
      if (iter->IsOperational() && (iter->GetQuantityRemaining() > 0.0))
      {
         return true;
      }
   }
   return false;
}

//! Determine if a any subordinate has any weapons that it can deploy.
//! NOTE: This may end up checking the same weapon system twice, but short of keeping a checked-weapon
//! list, there is no way to get around this. This is eventually going to get replaced.
bool HasOperationalSubordinate(WsfPlatform* aCmdrPtr, WsfStringId aSubType, std::set<WsfPlatform*>& aChecked)
{
   // Return immediately if we've already checked (or checking) the subordinates at this level.
   if (aChecked.find(aCmdrPtr) != aChecked.end())
   {
      return false;
   }
   aChecked.insert(aCmdrPtr);

   // Process all the subordinate on every command chains.
   for (WsfComponentList::RoleIterator<WsfCommandChain> iter(*aCmdrPtr); !iter.AtEnd(); ++iter)
   {
      // Process all platforms on the subordinate list
      WsfCommandChain::PlatformList& subList = iter->GetSubordinates();
      for (WsfPlatform* subPtr : subList)
      {
         if ((aSubType == nullptr) || (subPtr->IsA_TypeOf(aSubType)))
         {
            // Check to see if weapons are available are available on this subordinate.
            if (HasOperationalWeapon(subPtr))
            {
               return true;
            }
         }
         if (HasOperationalSubordinate(subPtr, aSubType, aChecked))
         {
            return true;
         }
      }
   }
   return false;
}

namespace WsfCommScriptMethods
{
UT_DEFINE_SCRIPT_METHOD_EXT(wsf::comm::Comm, JammingPerceived, 0, "bool", "")
{
   bool                 jammingPerceived = false;
   WsfEW_CommComponent* componentPtr     = WsfEW_CommComponent::Find(*aObjectPtr);
   if (componentPtr != nullptr)
   {
      jammingPerceived = componentPtr->JammingPerceived();
   }
   aReturnVal.SetBool(jammingPerceived);
}
} // namespace WsfCommScriptMethods

namespace WsfEM_InteractionScriptMethods
{
// double signalTo = <x>.SignalToNoiseClutterJamming();            //dB
UT_DEFINE_SCRIPT_METHOD_EXT(WsfEM_Interaction, SignalToNoiseClutterJamming, 0, "double", "")
{
   WsfEW_Result* ewInteractionPtr = WsfEW_Result::Find(*aObjectPtr);
   double        returnValue      = -300.0;
   if (ewInteractionPtr && (aObjectPtr->mSignalToNoise > 0.0) && (aObjectPtr->mRcvrNoisePower > 0.0))
   {
      double noisePower   = aObjectPtr->mRcvrNoisePower;
      double clutterPower = std::max(aObjectPtr->mClutterPower, 0.0);
      double jammerPower  = std::max(ewInteractionPtr->mNoiseJammerPower + ewInteractionPtr->mPulseJammerPower, 0.0);
      // Signal power used to be aResult.mRcvdPower, but that didn't always contain all the parts that were
      // considered 'signal' (depends on the sensor implementation). The following recovers the signal.
      double signalPower = aObjectPtr->mSignalToNoise * (noisePower + clutterPower + jammerPower);
      returnValue        = UtMath::LinearToDB(signalPower / (noisePower + clutterPower + jammerPower));
   }
   aReturnVal.SetDouble(returnValue);
}

// double noiseJammerPower = <x>.NoiseJammerPower();     // dBw
UT_DEFINE_SCRIPT_METHOD_EXT(WsfEM_Interaction, NoiseJammerPower, 0, "double", "")
{
   WsfEW_Result* ewResultPtr = WsfEW_Result::Find(*aObjectPtr);
   double        power       = -300.0;
   if (ewResultPtr && ((ewResultPtr->mNoiseJammerPower > 0.0) || (ewResultPtr->mPulseJammerPower > 0.0)))
   {
      power = UtMath::LinearToDB(ewResultPtr->mNoiseJammerPower + ewResultPtr->mPulseJammerPower);
   }
   aReturnVal.SetDouble(power);
}

// double pulseJammerPower = <x>.PulseJammerPower();     // dBw
UT_DEFINE_SCRIPT_METHOD_EXT(WsfEM_Interaction, PulseJammerPower, 0, "double", "")
{
   WsfEW_Result* ewResultPtr = WsfEW_Result::Find(*aObjectPtr);
   double        power       = -300.0;
   if (ewResultPtr && ewResultPtr->mPulseJammerPower > 0.0)
   {
      power = UtMath::LinearToDB(ewResultPtr->mPulseJammerPower);
   }
   aReturnVal.SetDouble(power);
}

// double coherentJammerPower = <x>.CoherentJammerPower();     // dBw
UT_DEFINE_SCRIPT_METHOD_EXT(WsfEM_Interaction, CoherentJammerPower, 0, "double", "")
{
   WsfEW_Result* ewResultPtr = WsfEW_Result::Find(*aObjectPtr);
   double        power       = -300.0;
   if (ewResultPtr && ewResultPtr->mCoherentJammerPower > 0.0)
   {
      power = UtMath::LinearToDB(ewResultPtr->mCoherentJammerPower);
   }
   aReturnVal.SetDouble(power);
}

// WsfEW_Effects ew_effects = <x>.EW_Effects();
UT_DEFINE_SCRIPT_METHOD_EXT(WsfEM_Interaction, EW_Effects, 0, "WsfEW_Effects", "")
{
   WsfEW_Result*  ewResultPtr = WsfEW_Result::Find(*aObjectPtr);
   WsfEW_Effects* effectsPtr  = (ewResultPtr != nullptr) ? &(ewResultPtr->mEW_Effects) : nullptr;
   aReturnVal.SetPointer(new UtScriptRef(effectsPtr, aReturnClassPtr));
}

} // namespace WsfEM_InteractionScriptMethods

namespace WsfMoverScriptMethods
{
}

namespace WsfPlatformScriptMethods
{

// ----------------------------------------------------------------------------
//                            Perception methods
// ----------------------------------------------------------------------------

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, PerceptionProcessor, 0, "WsfPerceptionProcessor", "")
{
   WsfPerceptionProcessor* procPtr = WsfPerceptionProcessor::GetPerceptionProcessor(*aObjectPtr);
   aReturnVal.SetPointer(new UtScriptRef(procPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, PerceivedAssets, 0, "Array<WsfAssetPerception>", "")
{
   std::vector<UtScriptData>*        scriptAssets = new std::vector<UtScriptData>();         // this array is returned
   UtScriptClass*                    classPtr     = aReturnClassPtr->GetContainerDataType(); // type of object in array
   std::vector<WsfAssetPerception*>& assets =
      WsfPerceptionProcessor::GetPlatformPerceivedAssets(*aObjectPtr, SIMULATION->GetSimTime());
   for (auto& asset : assets)
   {
      scriptAssets->emplace_back(UtScriptRef::Ref(asset, classPtr));
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptAssets, aReturnClassPtr, UtScriptRef::cMANAGE)); // manage the vector itself,
                                                                                                // but not the members
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, PerceivedThreats, 0, "Array<WsfTrack>", "")
{
   std::vector<UtScriptData>*   scriptTracks = new std::vector<UtScriptData>();           // this array is returned
   UtScriptClass*               classPtr     = aContext.GetTypes()->GetClass("WsfTrack"); // type of object in array
   std::vector<WsfLocalTrack*>& tracks = WsfPerceptionProcessor::GetPlatformPerceivedThreats(*aObjectPtr, TIME_NOW);
   for (auto& track : tracks)
   {
      scriptTracks->push_back(UtScriptData(new UtScriptRef(track, classPtr)));
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptTracks, aReturnClassPtr, UtScriptRef::cMANAGE)); // manage the vector itself,
                                                                                                // but not the members
}

// ----------------------------------------------------------------------------
//                            Uplink methods
// ----------------------------------------------------------------------------
UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, UplinkProcessor, 0, "WsfUplinkProcessor", "")
{
   WsfUplinkProcessor* procPtr = WsfUplinkProcessor::GetUplinkProcessor(*aObjectPtr);
   aReturnVal.SetPointer(new UtScriptRef(procPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, IsUplinkingTo, 1, "bool", "WsfPlatform")
{
   WsfUplinkProcessor* procPtr     = WsfUplinkProcessor::GetUplinkProcessor(*aObjectPtr);
   WsfPlatform*        platPtr     = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   bool                isUplinking = false;
   if (procPtr != nullptr)
   {
      isUplinking = procPtr->IsUplinkingTo(platPtr);
   }
   aReturnVal.SetBool(isUplinking);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, UplinkCount, 0, "int", "")
{
   WsfUplinkProcessor* procPtr     = WsfUplinkProcessor::GetUplinkProcessor(*aObjectPtr);
   int                 uplinkCount = 0;
   if (procPtr != nullptr)
   {
      uplinkCount = procPtr->WeaponUplinkCount();
   }
   aReturnVal.SetInt(uplinkCount);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, UplinkCapable, 0, "bool", "")
{
   WsfUplinkProcessor* procPtr       = WsfUplinkProcessor::GetUplinkProcessor(*aObjectPtr);
   bool                uplinkCapable = false;
   if (procPtr != nullptr)
   {
      uplinkCapable = procPtr->UplinkCapable();
   }
   aReturnVal.SetBool(uplinkCapable);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, UplinkPlatformEntry, 1, "WsfPlatform", "int")
{
   WsfUplinkProcessor* procPtr    = WsfUplinkProcessor::GetUplinkProcessor(*aObjectPtr);
   unsigned int        index      = (unsigned int)aVarArgs[0].GetInt();
   WsfPlatform*        uplinkPlat = nullptr;
   if (procPtr != nullptr)
   {
      uplinkPlat = procPtr->UplinkWeaponPlatform(index);
   }
   aReturnVal.SetPointer(new UtScriptRef(uplinkPlat, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, StartUplinking_1, 1, "bool", "WsfPlatform")
{
   WsfUplinkProcessor* procPtr = WsfUplinkProcessor::GetUplinkProcessor(*aObjectPtr);
   WsfPlatform*        weapon  = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   bool                started = false;
   if (procPtr != nullptr)
   {
      started = procPtr->StartUplinking(weapon, nullptr);
   }
   aReturnVal.SetBool(started);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, StartUplinking_2, 2, "bool", "WsfPlatform, WsfTrack")
{
   WsfUplinkProcessor* procPtr = WsfUplinkProcessor::GetUplinkProcessor(*aObjectPtr);
   WsfPlatform*        weapon  = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfTrack*           target  = (WsfTrack*)aVarArgs[1].GetPointer()->GetAppObject();
   bool                started = false;
   if (procPtr != nullptr)
   {
      started = procPtr->StartUplinking(weapon, target);
   }
   aReturnVal.SetBool(started);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, StopUplinking, 1, "bool", "WsfPlatform")
{
   WsfUplinkProcessor* procPtr = WsfUplinkProcessor::GetUplinkProcessor(*aObjectPtr);
   bool                stopped = false;
   if (procPtr != nullptr)
   {
      WsfPlatform* weapon = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
      stopped             = procPtr->StopUplinking(weapon);
   }
   aReturnVal.SetBool(stopped);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, MaxSupportedUplinks, 0, "int", "")
{
   WsfUplinkProcessor* procPtr          = WsfUplinkProcessor::GetUplinkProcessor(*aObjectPtr);
   int                 supportedUplinks = 0;
   if (procPtr != nullptr)
   {
      supportedUplinks = procPtr->GetMaxSupportedUplinks();
   }
   aReturnVal.SetInt(supportedUplinks);
}


//! Detonate(string aResult);
//! Detonates the current platform. If not a weapon platform then it just deletes the platform.
//! aResult is one of the detonation codes from WsfWeaponEngagement.
UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, Detonate, 1, "void", "string")

{
   double simTime = TIME_NOW;
   // Get the engagement from the platform
   WsfWeaponEngagement* engagementPtr = WsfWeaponEngagement::Find(*aObjectPtr);
   if (engagementPtr != nullptr)
   {
      WsfWeaponEngagement::GeometryResult result = WsfWeaponEngagement::cDUD;
      if (!WsfWeaponEngagement::StringToEnum(result, aVarArgs[0].GetString()))
      {
         result = WsfWeaponEngagement::cDUD;
      }
      engagementPtr->Terminate(simTime, result);
   }
   else
   {
      // Not a weapon (no engagement data). Just delete the platform
      SIMULATION->DeletePlatform(simTime, aObjectPtr);
   }
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, Weapon, 1, "WsfWeapon", "string")
{
   // Argument 1: string aName
   WsfWeapon* weaponPtr = aObjectPtr->template GetComponent<WsfWeapon>(aVarArgs[0].GetString());
   aReturnVal.SetPointer(UtScriptRef::Ref(weaponPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, WeaponCount, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->template GetComponentCount<WsfWeapon>());
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, WeaponEntry, 1, "WsfWeapon", "int")
{
   // Argument 1: int aEntryIndex
   unsigned   index     = (unsigned)aVarArgs[0].GetInt();
   WsfWeapon* weaponPtr = aObjectPtr->template GetComponentEntry<WsfWeapon>(index);
   if (weaponPtr == nullptr)
   {
      UT_SCRIPT_ABORT("Bad WeaponEntry index");
   }
   aReturnVal.SetPointer(UtScriptRef::Ref(weaponPtr, aReturnClassPtr));
}

// Return the elapsed time since the last weapon firing for the specified task.
// The return value will be less than zero if no weapon has been fired.
UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, TimeSinceWeaponLastFiredFor, 1, "double", "WsfTrackId")
{
   double            timeSince  = -1.0;
   const WsfTrackId* trackIdPtr = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   double            timeFired  = WsfWeapon::TimeWeaponLastFiredFor(*aObjectPtr, *trackIdPtr);
   if (timeFired >= 0.0)
   {
      timeSince = WsfScriptContext::GetTIME_NOW(aContext) - timeFired;
   }
   aReturnVal.SetDouble(timeSince);
}

// Return the elapsed time since the last weapon termination for the specified task.
// The return value will be less than zero if no weapon has been terminated.
UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, TimeSinceWeaponLastTerminatedFor, 1, "double", "WsfTrackId")
{
   double            timeSince      = -1.0;
   const WsfTrackId* trackIdPtr     = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   double            timeTerminated = WsfWeapon::TimeWeaponLastTerminatedFor(*aObjectPtr, *trackIdPtr);
   if (timeTerminated >= 0.0)
   {
      timeSince = WsfScriptContext::GetTIME_NOW(aContext) - timeTerminated;
   }
   aReturnVal.SetDouble(timeSince);
}

//! Returned the number of weapons that are pending for the specified task.
//! int count = WeaponsPendingFor(WsfTrackId aTrackId);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, WeaponsPendingFor, 1, "int", "WsfTrackId")
{
   const WsfTrackId* trackIdPtr = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   aReturnVal.SetInt(WsfWeapon::WeaponsPendingFor(*aObjectPtr, *trackIdPtr));
}

//! Returned the number of weapons that are active for the specified task.
//! int count = WeaponsActiveFor(WsfTrackId aTrackId);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, WeaponsActiveFor, 1, "int", "WsfTrackId")
{
   const WsfTrackId* trackIdPtr = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   aReturnVal.SetInt(WsfWeapon::WeaponsActiveFor(*aObjectPtr, *trackIdPtr));
}

//! Returned the number of weapons that are complete for the specified task.
//! int count = RoundsCompleteFor(WsfTrackId aTrackId);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, RoundsCompleteFor, 1, "int", "WsfTrackId")
{
   const WsfTrackId* trackIdPtr = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   aReturnVal.SetInt(WsfWeapon::RoundsCompleteFor(*aObjectPtr, *trackIdPtr));
}

//! Returned the number of rounds that have been fired for the specified task.
//! int count = RoundsFiredAt(WsfTrackId aTrackId);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, RoundsFiredAt, 1, "int", "WsfTrackId")
{
   const WsfTrackId* trackIdPtr = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   aReturnVal.SetInt(WsfWeapon::RoundsFiredAt(*aObjectPtr, *trackIdPtr));
}

//! Returned the number of salvos that have been fired for the specified task.
//! int count = RoundsFiredAt(WsfTrackId aTrackId);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, SalvosFiredAt, 1, "int", "WsfTrackId")
{
   const WsfTrackId* trackIdPtr = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   aReturnVal.SetInt(WsfWeapon::SalvosFiredAt(*aObjectPtr, *trackIdPtr));
}

//! Returned the number of salvos that have been fired for the specified task.
//! int count = RoundsFiredAt(WsfTrackId aTrackId);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, ActiveWeaponPlatformsFor, 1, "WsfPlatformList", "WsfTrackId")
{
   const WsfTrackId*   trackIdPtr        = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   std::vector<size_t> weaponPlatformIds = WsfWeapon::ActiveWeaponPlatformsFor(*aObjectPtr, *trackIdPtr);

   std::vector<WsfPlatform*>* platformListPtr = new std::vector<WsfPlatform*>();
   for (size_t weaponPlatformId : weaponPlatformIds)
   {
      WsfPlatform* platformPtr = SIMULATION->GetPlatformByIndex(weaponPlatformId);
      if (platformPtr != nullptr)
      {
         platformListPtr->push_back(platformPtr);
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(platformListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, WeaponEngagement, 0, "WsfWeaponEngagement", "")
{
   WsfWeaponEngagement* engagementPtr = WsfWeaponEngagement::Find(*aObjectPtr);
   aReturnVal.SetPointer(new UtScriptRef(engagementPtr, aReturnClassPtr));
}

//! bool hasOpSub = HasOperationalSubordinate(string aType)
//! This emulates the Suppressor SUB-STATUS IS SUB-OP (or SUB-O/A) resource allocation phrase.
UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, HasOperationalSubordinate, 1, "bool", "string") // NO_DOC | SUPPRESSOR
{
   std::set<WsfPlatform*> checked;
   WsfStringId            subType  = WsfStringId(aVarArgs[0].GetString());
   bool                   hasOpSub = ::HasOperationalSubordinate(aObjectPtr, subType, checked);
   aReturnVal.SetBool(hasOpSub);
}

//! WsfGeoPoint centroid = GetSubsCentroid()
//! Returns the centroid of the subordinates on this platform's default command chain
UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, GetSubsCentroid, 0, "WsfGeoPoint", "")
{
   WsfCommandChain::PlatformList* platformListPtr = nullptr;
   WsfCommandChain* chainPtr = aObjectPtr->template GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId());
   if (chainPtr == nullptr)
   {
      WsfGeoPoint centroid;
      aReturnVal.SetPointer(new UtScriptRef(centroid.Clone(), aReturnClassPtr, UtScriptRef::cMANAGE));
      return;
   }

   platformListPtr = &(chainPtr->GetSubordinates());

   WsfCommandChain::PlatformListIter iter = platformListPtr->begin();

   double tot[3] = {0};
   int    c      = 0;
   for (; iter != platformListPtr->end(); ++iter)
   {
      double cur[3] = {0};
      (*iter)->GetLocationWCS(cur);
      tot[0] += cur[0];
      tot[1] += cur[1];
      tot[2] += cur[2];
      ++c;
   }

   if (c > 0)
   {
      tot[0] /= (double)c;
      tot[1] /= (double)c;
      tot[2] /= (double)c;
   }

   WsfGeoPoint centroid(tot);

   aReturnVal.SetPointer(new UtScriptRef(centroid.Clone(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

//! bool withinFieldOfView = WithinFieldOfView(WsfTrack aTrack, string aSensorOrWeapon)
UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, WithinFieldOfView_1, 2, "bool", "WsfTrack, string")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   bool      withinFieldOfView(false);
   WsfTrack* trackPtr = (WsfTrack*)aVarArgs[0].GetPointer()->GetAppObject();
   // First check to see the second argument is an sensor
   WsfSensor* sensorPtr = aObjectPtr->template GetComponent<WsfSensor>(WsfStringId(aVarArgs[1].GetString()));
   if (sensorPtr != nullptr)
   {
      withinFieldOfView = sensorPtr->WithinFieldOfView(simTime, trackPtr);
   }
   else
   {
      // Check to see if this is a weapon
      WsfWeapon* weaponPtr = aObjectPtr->template GetComponent<WsfWeapon>(aVarArgs[1].GetString());
      if (weaponPtr != nullptr)
      {
         withinFieldOfView = weaponPtr->WithinFieldOfView(simTime, trackPtr);
      }
   }
   aReturnVal.SetBool(withinFieldOfView);
}

//! bool withinFieldOfView = WithinFieldOfView(WsfPlatform aPlatform, string aSensorOrWeapon)
UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, WithinFieldOfView_2, 2, "bool", "WsfPlatform, string")
{
   double simTime = TIME_NOW;
   aObjectPtr->Update(simTime); // Ensure source platform position is current
   bool         withinFieldOfView(false);
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   // First check to see the second argument is an sensor
   WsfSensor* sensorPtr = aObjectPtr->template GetComponent<WsfSensor>(WsfStringId(aVarArgs[1].GetString()));
   if (sensorPtr != nullptr)
   {
      withinFieldOfView = sensorPtr->WithinFieldOfView(simTime, platformPtr);
   }
   else
   {
      // Check to see if this is a weapon
      WsfWeapon* weaponPtr = aObjectPtr->template GetComponent<WsfWeapon>(aVarArgs[1].GetString());
      if (weaponPtr != nullptr)
      {
         withinFieldOfView = weaponPtr->WithinFieldOfView(simTime, platformPtr);
      }
   }
   aReturnVal.SetBool(withinFieldOfView);
}

} // namespace WsfPlatformScriptMethods

namespace WsfProcessScriptMethods
{

// NOTE: This method is DEPRECATED
UT_DEFINE_SCRIPT_METHOD_EXT(WsfProcessor, TargetAllocated, 3, "void", "double, WsfPlatform, WsfTrack")
{
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[1].GetPointer()->GetAppObject();
   WsfTrack*    trackPtr    = (WsfTrack*)aVarArgs[2].GetPointer()->GetAppObject();
   WsfObserver::TargetAllocated(SIMULATION)(aVarArgs[0].GetDouble(), platformPtr, trackPtr);
}

// NOTE: This method is DEPRECATED
UT_DEFINE_SCRIPT_METHOD_EXT(WsfProcessor, TargetEngaged, 3, "void", "double, WsfPlatform, WsfTrack")
{
   WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[1].GetPointer()->GetAppObject();
   WsfTrack*    trackPtr    = (WsfTrack*)aVarArgs[2].GetPointer()->GetAppObject();
   WsfObserver::TargetEngaged(SIMULATION)(aVarArgs[0].GetDouble(), platformPtr, trackPtr);
}

} // namespace WsfProcessScriptMethods

namespace WsfSensorScriptMethods
{

UT_DEFINE_SCRIPT_METHOD_EXT(WsfSensor, IsEP_TechniqueActive_1, 2, "bool", "string, string")
{
   // Argument 0: The technique ID.
   // Argument 1: the sensor mode to be used
   bool                   EP_TechniquActive = false;
   WsfEW_SensorComponent* componentPtr      = WsfEW_SensorComponent::Find(*aObjectPtr);
   if (componentPtr != nullptr)
   {
      WsfStringId techniqueId = aVarArgs[0].GetString();
      WsfStringId modeNameId  = aVarArgs[1].GetString();
      double      simTime     = TIME_NOW;
      EP_TechniquActive       = componentPtr->IsEP_TechniqueActive(simTime, techniqueId, modeNameId);
   }
   aReturnVal.SetBool(EP_TechniquActive);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfSensor, IsEP_TechniqueActive_2, 1, "bool", "string")
{
   // Argument 0: The technique ID.
   bool                   EP_TechniquActive = false;
   WsfEW_SensorComponent* componentPtr      = WsfEW_SensorComponent::Find(*aObjectPtr);
   if (componentPtr != nullptr)
   {
      WsfStringId techniqueId = aVarArgs[0].GetString();
      double      simTime     = TIME_NOW;
      EP_TechniquActive       = componentPtr->IsEP_TechniqueActive(simTime, techniqueId, nullptr);
   }
   aReturnVal.SetBool(EP_TechniquActive);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfSensor, SelectEP_Technique_1, 2, "bool", "string, string")
{
   // Argument 0: The technique ID.
   // Argument 1: the sensor mode to be used
   bool                   selectedEP_Technique = false;
   WsfEW_SensorComponent* componentPtr         = WsfEW_SensorComponent::Find(*aObjectPtr);
   if (componentPtr != nullptr)
   {
      WsfStringId techniqueId = aVarArgs[0].GetString();
      WsfStringId modeNameId  = aVarArgs[1].GetString();
      double      simTime     = TIME_NOW;
      selectedEP_Technique    = componentPtr->SelectEP_Technique(simTime, techniqueId, modeNameId);
   }
   aReturnVal.SetBool(selectedEP_Technique);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfSensor, SelectEP_Technique_2, 1, "bool", "string")
{
   // Argument 0: The technique ID.
   bool                   selectedEP_Technique = false;
   WsfEW_SensorComponent* componentPtr         = WsfEW_SensorComponent::Find(*aObjectPtr);
   if (componentPtr != nullptr)
   {
      WsfStringId techniqueId = aVarArgs[0].GetString();
      double      simTime     = TIME_NOW;
      selectedEP_Technique    = componentPtr->SelectEP_Technique(simTime, techniqueId, WsfStringId());
   }
   aReturnVal.SetBool(selectedEP_Technique);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfSensor, DeselectEP_Technique_1, 2, "bool", "string, string")
{
   // Argument 0: The technique ID.
   // Argument 1: the sensor mode to be used
   bool                   selectedEP_Technique = false;
   WsfEW_SensorComponent* componentPtr         = WsfEW_SensorComponent::Find(*aObjectPtr);
   if (componentPtr != nullptr)
   {
      WsfStringId techniqueId = aVarArgs[0].GetString();
      WsfStringId modeNameId  = aVarArgs[1].GetString();
      double      simTime     = TIME_NOW;
      selectedEP_Technique    = componentPtr->DeselectEP_Technique(simTime, techniqueId, modeNameId);
   }
   aReturnVal.SetBool(selectedEP_Technique);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfSensor, DeselectEP_Technique_2, 1, "bool", "string")
{
   // Argument 0: The technique ID.
   bool                   selectedEP_Technique = false;
   WsfEW_SensorComponent* componentPtr         = WsfEW_SensorComponent::Find(*aObjectPtr);
   if (componentPtr != nullptr)
   {
      WsfStringId techniqueId = aVarArgs[0].GetString();
      double      simTime     = TIME_NOW;
      selectedEP_Technique    = componentPtr->DeselectEP_Technique(simTime, techniqueId, nullptr);
   }
   aReturnVal.SetBool(selectedEP_Technique);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfSensor, JammingPerceived_1, 0, "bool", "")
{
   bool                   jammingPerceived = false;
   WsfEW_SensorComponent* componentPtr     = WsfEW_SensorComponent::Find(*aObjectPtr);
   if (componentPtr != nullptr)
   {
      jammingPerceived = componentPtr->JammingPerceived(WsfStringId());
   }
   aReturnVal.SetBool(jammingPerceived);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfSensor, JammingPerceived_2, 1, "bool", "string")
{
   // Argument 0: the sensor mode to be used
   bool                   jammingPerceived = false;
   WsfEW_SensorComponent* componentPtr     = WsfEW_SensorComponent::Find(*aObjectPtr);
   if (componentPtr != nullptr)
   {
      WsfStringId modeNameId(aVarArgs[0].GetString());
      jammingPerceived = componentPtr->JammingPerceived(modeNameId);
   }
   aReturnVal.SetBool(jammingPerceived);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfSensor, ContinuousJammingPerceived_1, 0, "bool", "")
{
   bool                   continuousJammingPerceived = false;
   WsfEW_SensorComponent* componentPtr               = WsfEW_SensorComponent::Find(*aObjectPtr);
   if (componentPtr != nullptr)
   {
      continuousJammingPerceived = componentPtr->ContinuousJammingPerceived(WsfStringId());
   }
   aReturnVal.SetBool(continuousJammingPerceived);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfSensor, ContinuousJammingPerceived_2, 1, "bool", "string")
{
   // Argument 0: the sensor mode to be used
   bool                   continuousJammingPerceived = false;
   WsfEW_SensorComponent* componentPtr               = WsfEW_SensorComponent::Find(*aObjectPtr);
   if (componentPtr != nullptr)
   {
      WsfStringId modeNameId(aVarArgs[0].GetString());
      continuousJammingPerceived = componentPtr->ContinuousJammingPerceived(modeNameId);
   }
   aReturnVal.SetBool(continuousJammingPerceived);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfSensor, PulseJammingPerceived_1, 0, "bool", "")
{
   bool                   pulseJammingPerceived = false;
   WsfEW_SensorComponent* componentPtr          = WsfEW_SensorComponent::Find(*aObjectPtr);
   if (componentPtr != nullptr)
   {
      pulseJammingPerceived = componentPtr->PulseJammingPerceived(WsfStringId());
   }
   aReturnVal.SetBool(pulseJammingPerceived);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfSensor, PulseJammingPerceived_2, 1, "bool", "string")
{
   // Argument 0: the sensor mode to be used
   bool                   pulseJammingPerceived = false;
   WsfEW_SensorComponent* componentPtr          = WsfEW_SensorComponent::Find(*aObjectPtr);
   if (componentPtr != nullptr)
   {
      WsfStringId modeNameId(aVarArgs[0].GetString());
      pulseJammingPerceived = componentPtr->PulseJammingPerceived(modeNameId);
   }
   aReturnVal.SetBool(pulseJammingPerceived);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfSensor, LaserCode_1, 0, "int", "")
{
   // A little hokey, but there are currently only two sensors that support this.
   // If there become more then we should probably create a base class.
   int                 laserCode(0);
   WsfLaserDesignator* ldPtr = dynamic_cast<WsfLaserDesignator*>(aObjectPtr);
   if (ldPtr != nullptr)
   {
      laserCode = ldPtr->GetLaserCode();
   }
   else
   {
      WsfLaserTracker* ltPtr = dynamic_cast<WsfLaserTracker*>(aObjectPtr);
      if (ltPtr != nullptr)
      {
         laserCode = ltPtr->GetLaserCode();
      }
   }
   aReturnVal.SetInt(laserCode);
}

UT_DEFINE_SCRIPT_METHOD_EXT(WsfSensor, LaserCode_2, 1, "void", "int")
{
   // Argument 0: the laser code to be used
   int                 laserCode = aVarArgs[0].GetInt();
   WsfLaserDesignator* ldPtr     = dynamic_cast<WsfLaserDesignator*>(aObjectPtr);
   if (ldPtr != nullptr)
   {
      ldPtr->SetLaserCode(laserCode);
   }
   else
   {
      WsfLaserTracker* ltPtr = dynamic_cast<WsfLaserTracker*>(aObjectPtr);
      if (ltPtr != nullptr)
      {
         ltPtr->SetLaserCode(laserCode);
      }
   }
}

} // namespace WsfSensorScriptMethods

namespace WsfTaskManagerScriptMethods
{
// Return the elapsed time since the last weapon firing for the specified task.
// The return value will be less than zero if no weapon has been fired.
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, TimeSinceWeaponLastFiredFor, 1, "double", "WsfTrackId")
{
   double                timeSince   = -1.0;
   const WsfTrackId*     trackIdPtr  = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfWeaponTaskManager* wpnTaskMngr = WsfWeaponTaskManager::Find(*aObjectPtr);
   double                timeFired   = wpnTaskMngr->TimeWeaponLastFiredFor(*trackIdPtr);
   if (timeFired >= 0.0)
   {
      timeSince = WsfScriptContext::GetTIME_NOW(aContext) - timeFired;
   }
   aReturnVal.SetDouble(timeSince);
}

// Return the elapsed time since the last weapon termination for the specified task.
// The return value will be less than zero if no weapon has been terminated.
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, TimeSinceWeaponLastTerminatedFor, 1, "double", "WsfTrackId")
{
   double                timeSince      = -1.0;
   const WsfTrackId*     trackIdPtr     = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfWeaponTaskManager* wpnTaskMngr    = WsfWeaponTaskManager::Find(*aObjectPtr);
   double                timeTerminated = wpnTaskMngr->TimeWeaponLastTerminatedFor(*trackIdPtr);
   if (timeTerminated >= 0.0)
   {
      timeSince = WsfScriptContext::GetTIME_NOW(aContext) - timeTerminated;
   }
   aReturnVal.SetDouble(timeSince);
}

//! Returned the number of weapons that are active for the specified task.
//! int count = WeaponsActiveFor(WsfTrackId aTrackId);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, WeaponsActiveFor_1, 1, "int", "WsfTrackId")
{
   const WsfTrackId*     trackIdPtr  = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfWeaponTaskManager* wpnTaskMngr = WsfWeaponTaskManager::Find(*aObjectPtr);
   aReturnVal.SetInt(wpnTaskMngr->WeaponsActiveFor(*trackIdPtr));
}

//! Returned the number of weapons that are active for the specified task for the given platform.
//! int count = WeaponsActiveFor(WsfTrackId aTrackId, WsfPlatform aAssignee);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, WeaponsActiveFor_2, 2, "int", "WsfTrackId, WsfPlatform")
{
   const WsfTrackId*     trackIdPtr  = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfPlatform*          assigneePtr = static_cast<WsfPlatform*>(aVarArgs[1].GetPointer()->GetAppObject());
   WsfWeaponTaskManager* wpnTaskMngr = WsfWeaponTaskManager::Find(*aObjectPtr);
   aReturnVal.SetInt(wpnTaskMngr->WeaponsActiveFor(*trackIdPtr, assigneePtr));
}

//! Returned the number of rounds that have been fired for the specified task.
//! int count = RoundsFiredAt(WsfTrackId aTrackId);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, RoundsFiredAt_1, 1, "int", "WsfTrackId")
{
   const WsfTrackId*     trackIdPtr  = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfWeaponTaskManager* wpnTaskMngr = WsfWeaponTaskManager::Find(*aObjectPtr);
   aReturnVal.SetInt(wpnTaskMngr->RoundsFiredAt(*trackIdPtr));
}

//! Returned the number of rounds that have been fired for the specified task for the given platform.
//! int count = RoundsFiredAt(WsfTrackId aTrackId, WsfPlatform aAssignee);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, RoundsFiredAt_2, 2, "int", "WsfTrackId, WsfPlatform")
{
   const WsfTrackId*     trackIdPtr  = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfPlatform*          assigneePtr = static_cast<WsfPlatform*>(aVarArgs[1].GetPointer()->GetAppObject());
   WsfWeaponTaskManager* wpnTaskMngr = WsfWeaponTaskManager::Find(*aObjectPtr);
   aReturnVal.SetInt(wpnTaskMngr->RoundsFiredAt(*trackIdPtr, assigneePtr));
}

//! Returned the number of salvos that have been fired for the specified task.
//! int count = RoundsFiredAt(WsfTrackId aTrackId);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, SalvosFiredAt, 1, "int", "WsfTrackId")
{
   const WsfTrackId*     trackIdPtr  = static_cast<const WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfWeaponTaskManager* wpnTaskMngr = WsfWeaponTaskManager::Find(*aObjectPtr);
   aReturnVal.SetInt(wpnTaskMngr->SalvosFiredAt(*trackIdPtr));
}

//! Fire a local weapon at a track.
//! This is a more obvious form of firing a weapon instead of using AssignTask.
//! bool ok = Fire(WsfTrack aTrack, string aTaskType, WsfWeapon aWeapon, int aQuantity);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, FireLocalWeapon, 4, "bool", "WsfTrack, string, WsfWeapon, int")
{
   // Argument 0: the track
   // Argument 1: the task type
   // Argument 2: the weapon to be used
   // Argument 3: the weapon quantity to be used

   bool                  ok(false);
   WsfWeapon*            weaponPtr = static_cast<WsfWeapon*>(aVarArgs[2].GetPointer()->GetAppObject());
   WsfTrack*             trackPtr  = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfWeaponTaskResource resource;            // resource info
   resource.mNameId = weaponPtr->GetNameId(); // - weapon name
   resource.mCount  = aVarArgs[3].GetInt();   // - weapon count

   WsfPlatform* assigneePtr = weaponPtr->GetPlatform();                // assignee - local
   WsfStringId  commName    = nullptr;                                 // comm     - N/A
   WsfStringId  taskType    = aVarArgs[1].GetString();                 // taskType
   double       simTime     = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                       = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! Fire a remote weapon at a track.
//! This is a more obvious form of firing a weapon instead of using AssignTask.
//! bool ok = Fire(WsfTrack aTrack, string aTaskType, WsfWeapon aWeapon, int aQuantity,
//!                WsfPlatform aAssignee);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, FireRemoteWeapon_1, 5, "bool", "WsfTrack, string, string, int, WsfPlatform")
{
   bool                  ok(false);
   WsfTrack*             trackPtr    = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfPlatform*          assigneePtr = static_cast<WsfPlatform*>(aVarArgs[4].GetPointer()->GetAppObject());
   WsfWeaponTaskResource resource;             // resource info
   resource.mNameId = aVarArgs[2].GetString(); // - weapon name
   resource.mCount  = aVarArgs[3].GetInt();    // - weapon count

   WsfStringId taskType = aVarArgs[1].GetString();                 // taskType - N/A
   WsfStringId commName = nullptr;                                 // comm
   double      simTime  = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                   = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! Fire a remote weapon at a track.
//! This is a more obvious form of firing a weapon instead of using AssignTask.
//! bool ok = Fire(WsfTrack aTrack, string aTaskType, WsfWeapon aWeapon, int aQuantity,
//!                WsfPlatform aAssignee, string aCommName);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, FireRemoteWeapon_2, 6, "bool", "WsfTrack, string, string, int, WsfPlatform, string")
{
   bool                  ok(false);
   WsfTrack*             trackPtr    = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfPlatform*          assigneePtr = static_cast<WsfPlatform*>(aVarArgs[4].GetPointer()->GetAppObject());
   WsfWeaponTaskResource resource;             // resource info
   resource.mNameId = aVarArgs[2].GetString(); // - weapon name
   resource.mCount  = aVarArgs[3].GetInt();    // - weapon count

   WsfStringId commName = aVarArgs[5].GetString();                 // comm
   WsfStringId taskType = aVarArgs[1].GetString();                 // taskType - N/A
   double      simTime  = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                   = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! Abort any inflight weapons or outstanding weapon tasks for the specified track (ID).
//! AbortFiringAt(WsfTrackId aTrackId);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, AbortFiringAt, 1, "void", "WsfTrackId")
{
   double                simTime     = WsfScriptContext::GetTIME_NOW(aContext);
   WsfWeaponTaskManager* wpnTaskMngr = WsfWeaponTaskManager::Find(*aObjectPtr);
   wpnTaskMngr->AbortFiring(simTime, GetTrackId(aVarArgs[0]));
}

//! A simple query to see if this track has been fired on before.
//! bool haveFiredAt = HaveFiredAt(WsfTrackId aTrackId)
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, HaveFiredAt, 1, "bool", "WsfTrackId")
{
   bool                  haveFiredAt = false;
   WsfTrackId*           trackIdPtr  = static_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfWeaponTaskManager* wpnTaskMngr = WsfWeaponTaskManager::Find(*aObjectPtr);
   haveFiredAt                       = (wpnTaskMngr->RoundsFiredAt(*trackIdPtr) > 0);
   aReturnVal.SetBool(haveFiredAt);
}


//! StartUplinking a track to another platform
//! This is a more obvious way of using AssignTask()
//! bool ok = UplinkTrack(WsfTrack aTrackId, string aTaskType, WsfPlatform aDestination);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, StartUplinking_1, 3, "bool", "WsfTrack, string, WsfPlatform")
{
   bool         ok             = false;
   WsfPlatform* assigneePtr    = WsfScriptContext::GetPLATFORM(aContext);
   WsfTrack*    trackPtr       = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfPlatform* destinationPtr = static_cast<WsfPlatform*>(aVarArgs[2].GetPointer()->GetAppObject());
   if (assigneePtr != nullptr && destinationPtr != nullptr)
   {
      WsfUplinkTaskResource resource;

      WsfStringId taskType        = aVarArgs[1].GetString();
      WsfStringId commName        = nullptr;
      double      simTime         = WsfScriptContext::GetTIME_NOW(aContext);
      resource.mUplinkDestination = destinationPtr->GetNameId();
      ok = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   }
   aReturnVal.SetBool(ok);
}

//! StartUplinking a track to another platform
//! This is a more obvious way of using AssignTask()
//! bool ok = UplinkTrack(WsfTrack aTrackId, string aTaskType, WsfPlatform aDestination);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, StartUplinking_2, 4, "bool", "WsfTrack, string, WsfPlatform, WsfPlatform")
{
   bool         ok             = false;
   WsfTrack*    trackPtr       = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfPlatform* destinationPtr = static_cast<WsfPlatform*>(aVarArgs[2].GetPointer()->GetAppObject());
   WsfPlatform* assigneePtr    = static_cast<WsfPlatform*>(aVarArgs[3].GetPointer()->GetAppObject());
   if (assigneePtr != nullptr && destinationPtr != nullptr)
   {
      WsfUplinkTaskResource resource;

      WsfStringId taskType        = aVarArgs[1].GetString();
      WsfStringId commName        = nullptr;
      double      simTime         = WsfScriptContext::GetTIME_NOW(aContext);
      resource.mUplinkDestination = destinationPtr->GetNameId();
      ok = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   }
   aReturnVal.SetBool(ok);
}

//! StartJamming using local jammer at the given frequency and bandwidth.
//! This is a more obvious form of starting jamming instead of using AssignTask.
//! bool ok = StartJamming(WsfTrack aTrack, string aTaskType, WsfWeapon aJammer, double aFrequency, double aBandwidth);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, StartLocalJamming_1, 5, "bool", "WsfTrack, string, WsfWeapon, double, double")
{
   bool                  ok(false);
   WsfWeapon*            jammerPtr = static_cast<WsfWeapon*>(aVarArgs[2].GetPointer()->GetAppObject());
   WsfTrack*             trackPtr  = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfJammerTaskResource resource;                    // resource info
   resource.mNameId        = jammerPtr->GetNameId();  // - jammer name
   resource.mFrequency     = aVarArgs[3].GetDouble(); // - jammer frequency
   resource.mBandwidth     = aVarArgs[4].GetDouble(); // - jammer bandwidth
   resource.mBeamNumber    = 0;                       // - jammer beamNumber N/A - auto-add
   resource.mTechniqueName = nullptr;                 // - jammer technique name - use default techniques

   WsfPlatform* assigneePtr = jammerPtr->GetPlatform();                // assignee - local
   WsfStringId  commName    = nullptr;                                 // comm - N/A
   WsfStringId  taskType    = aVarArgs[1].GetString();                 // taskType
   double       simTime     = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                       = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! StartJamming using local jammer at the given frequency and bandwidth with the specified technique.
//! This is a more obvious form of starting jamming instead of using AssignTask.
//! bool ok = StartJamming(WsfTrack aTrack, string aTaskType, WsfWeapon aJammer, double aFrequency, double aBandwidth
//!                        string aTechniqueName);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, StartLocalJamming_2, 6, "bool", "WsfTrack, string, WsfWeapon, double, double, string")
{
   bool                  ok(false);
   WsfWeapon*            jammerPtr = static_cast<WsfWeapon*>(aVarArgs[2].GetPointer()->GetAppObject());
   WsfTrack*             trackPtr  = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfJammerTaskResource resource;                    // resource info
   resource.mNameId        = jammerPtr->GetNameId();  // - jammer name
   resource.mFrequency     = aVarArgs[3].GetDouble(); // - jammer frequency
   resource.mBandwidth     = aVarArgs[4].GetDouble(); // - jammer bandwidth
   resource.mBeamNumber    = 0;                       // - jammer beamNumber N/A - auto-add
   resource.mTechniqueName = aVarArgs[5].GetString(); // - jammer technique name

   WsfPlatform* assigneePtr = jammerPtr->GetPlatform();                // assignee - local
   WsfStringId  commName    = nullptr;                                 // comm - N/A
   WsfStringId  taskType    = aVarArgs[1].GetString();                 // taskType
   double       simTime     = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                       = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! StartJamming using local jammer at the given frequency and bandwidth on the specified beam.
//! This is a more obvious form of starting jamming instead of using AssignTask.
//! bool ok = StartJamming(WsfTrack aTrack, string aTaskType, WsfWeapon aJammer, double aFrequency, double aBandwidth,
//!                        int aBeamNumber);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, StartLocalJamming_3, 6, "bool", "WsfTrack, string, WsfWeapon, double, double, int")
{
   bool                  ok(false);
   WsfWeapon*            jammerPtr = static_cast<WsfWeapon*>(aVarArgs[2].GetPointer()->GetAppObject());
   WsfTrack*             trackPtr  = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfJammerTaskResource resource;                    // resource info
   resource.mNameId        = jammerPtr->GetNameId();  // - jammer name
   resource.mFrequency     = aVarArgs[3].GetDouble(); // - jammer frequency
   resource.mBandwidth     = aVarArgs[4].GetDouble(); // - jammer bandwidth
   resource.mBeamNumber    = aVarArgs[5].GetInt();    // - jammer beamNumber, 0 - auto-add
   resource.mTechniqueName = nullptr;                 // - jammer technique name - use default techniques

   WsfPlatform* assigneePtr = jammerPtr->GetPlatform();                // assignee - local
   WsfStringId  commName    = nullptr;                                 // comm - N/A
   WsfStringId  taskType    = aVarArgs[1].GetString();                 // taskType
   double       simTime     = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                       = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! StartJamming using local jammer at the given frequency and bandwidth  on the specified beam with specified technique.
//! This is a more obvious form of starting jamming instead of using AssignTask.
//! bool ok = StartJamming(WsfTrack aTrack, string aTaskType, WsfWeapon aJammer, double aFrequency, double aBandwidth,
//!                        int aBeamNumber, string aTechniqueName);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, StartLocalJamming_4, 7, "bool", "WsfTrack, string, WsfWeapon, double, double, int, string")
{
   bool                  ok(false);
   WsfWeapon*            jammerPtr = static_cast<WsfWeapon*>(aVarArgs[2].GetPointer()->GetAppObject());
   WsfTrack*             trackPtr  = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfJammerTaskResource resource;                    // resource info
   resource.mNameId        = jammerPtr->GetNameId();  // - jammer name
   resource.mFrequency     = aVarArgs[3].GetDouble(); // - jammer frequency
   resource.mBandwidth     = aVarArgs[4].GetDouble(); // - jammer bandwidth
   resource.mBeamNumber    = aVarArgs[5].GetInt();    // - jammer beamNumber, 0 - auto-add
   resource.mTechniqueName = aVarArgs[6].GetString(); // - jammer technique name

   WsfPlatform* assigneePtr = jammerPtr->GetPlatform();                // assignee - local
   WsfStringId  commName    = nullptr;                                 // comm - N/A
   WsfStringId  taskType    = aVarArgs[1].GetString();                 // taskType
   double       simTime     = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                       = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! StartJamming using local jammer at the given frequency and bandwidth.
//! This is a more obvious form of starting jamming instead of using AssignTask.
//! bool ok = StartJamming(WsfTrack aTrack, string aTaskType, WsfWeapon aJammer);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, StartLocalJammingTrack_1, 3, "bool", "WsfTrack, string, WsfWeapon")
{
   bool                  ok(false);
   WsfWeapon*            jammerPtr = static_cast<WsfWeapon*>(aVarArgs[2].GetPointer()->GetAppObject());
   WsfTrack*             trackPtr  = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfJammerTaskResource resource;            // resource info
   resource.mNameId = jammerPtr->GetNameId(); // - jammer name
   if (trackPtr->FrequencyValid())
   {
      resource.mFrequency = trackPtr->GetFrequency(); // - jammer frequency
   }
   resource.mBandwidth     = 0.0;     // - jammer bandwidth
   resource.mBeamNumber    = 0;       // - jammer beamNumber N/A - auto-add
   resource.mTechniqueName = nullptr; // - jammer technique name - use default techniques

   WsfPlatform* assigneePtr = jammerPtr->GetPlatform();                // assignee - local
   WsfStringId  commName    = nullptr;                                 // comm - N/A
   WsfStringId  taskType    = aVarArgs[1].GetString();                 // taskType
   double       simTime     = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                       = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! StartJamming using local jammer at the given frequency and bandwidth with the specified technique.
//! This is a more obvious form of starting jamming instead of using AssignTask.
//! bool ok = StartJamming(WsfTrack aTrack, string aTaskType, WsfWeapon aJammer, string aTechniqueName);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, StartLocalJammingTrack_2, 4, "bool", "WsfTrack, string, WsfWeapon, string")
{
   bool                  ok(false);
   WsfWeapon*            jammerPtr = static_cast<WsfWeapon*>(aVarArgs[2].GetPointer()->GetAppObject());
   WsfTrack*             trackPtr  = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfJammerTaskResource resource;            // resource info
   resource.mNameId = jammerPtr->GetNameId(); // - jammer name
   if (trackPtr->FrequencyValid())
   {
      resource.mFrequency = trackPtr->GetFrequency(); // - jammer frequency
   }
   resource.mBandwidth     = 0.0;                     // - jammer bandwidth
   resource.mBeamNumber    = 0;                       // - jammer beamNumber N/A - auto-add
   resource.mTechniqueName = aVarArgs[3].GetString(); // - jammer technique name

   WsfPlatform* assigneePtr = jammerPtr->GetPlatform();                // assignee - local
   WsfStringId  commName    = nullptr;                                 // comm - N/A
   WsfStringId  taskType    = aVarArgs[1].GetString();                 // taskType
   double       simTime     = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                       = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! StartJamming using remote jammer at the given frequency and bandwidth.
//! This is a more obvious form of starting jamming instead of using AssignTask.
//! bool ok = StartJamming(WsfTrack aTrack, string aTaskType, string aResourceName, double aFrequency, double aBandwidth,
//!                        WsfPlatform aAssignee);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, StartRemoteJamming_1, 6, "bool", "WsfTrack, string, string, double, double, WsfPlatform")
{
   bool                  ok(false);
   WsfTrack*             trackPtr    = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfPlatform*          assigneePtr = static_cast<WsfPlatform*>(aVarArgs[5].GetPointer()->GetAppObject());
   WsfJammerTaskResource resource;                    // resource info
   resource.mNameId        = aVarArgs[2].GetString(); // - jammer name
   resource.mFrequency     = aVarArgs[3].GetDouble(); // - jammer frequency
   resource.mBandwidth     = aVarArgs[4].GetDouble(); // - jammer bandwidth
   resource.mBeamNumber    = 0;                       // - jammer beamNumber N/A - auto-add
   resource.mTechniqueName = nullptr;                 // - jammer technique name - use default techniques

   WsfStringId taskType = aVarArgs[1].GetString();                 // taskType
   WsfStringId commName = nullptr;                                 // comm
   double      simTime  = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                   = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! StartJamming using remote jammer at the given frequency and bandwidth.
//! This is a more obvious form of starting jamming instead of using AssignTask.
//! bool ok = StartJamming(WsfTrack aTrack, string aTaskType, string aResourceName, double aFrequency, double aBandwidth,
//!                        string aTechniqueName, WsfPlatform aAssignee);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager,
                            StartRemoteJamming_2,
                            7,
                            "bool",
                            "WsfTrack, string, string, double, double, string, WsfPlatform")
{
   bool                  ok(false);
   WsfTrack*             trackPtr    = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfPlatform*          assigneePtr = static_cast<WsfPlatform*>(aVarArgs[6].GetPointer()->GetAppObject());
   WsfJammerTaskResource resource;                    // resource info
   resource.mNameId        = aVarArgs[2].GetString(); // - jammer name
   resource.mFrequency     = aVarArgs[3].GetDouble(); // - jammer frequency
   resource.mBandwidth     = aVarArgs[4].GetDouble(); // - jammer bandwidth
   resource.mBeamNumber    = 0;                       // - jammer beamNumber N/A - auto-add
   resource.mTechniqueName = aVarArgs[5].GetString(); // - jammer technique name

   WsfStringId taskType = aVarArgs[1].GetString();                 // taskType
   WsfStringId commName = nullptr;                                 // comm
   double      simTime  = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                   = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! StartJamming using remote jammer at the given frequency and bandwidth.
//! This is a more obvious form of starting jamming instead of using AssignTask.
//! bool ok = StartJamming(WsfTrack aTrack, string aTaskType, string aResourceName, double aFrequency, double aBandwidth,
//!                        WsfPlatform aAssignee, string aCommName);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager,
                            StartRemoteJamming_3,
                            7,
                            "bool",
                            "WsfTrack, string, string, double, double, WsfPlatform, string")
{
   bool                  ok(false);
   WsfTrack*             trackPtr    = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfPlatform*          assigneePtr = static_cast<WsfPlatform*>(aVarArgs[5].GetPointer()->GetAppObject());
   WsfJammerTaskResource resource;                    // resource info
   resource.mNameId        = aVarArgs[2].GetString(); // - jammer name
   resource.mFrequency     = aVarArgs[3].GetDouble(); // - jammer frequency
   resource.mBandwidth     = aVarArgs[4].GetDouble(); // - jammer bandwidth
   resource.mBeamNumber    = 0;                       // - jammer beamNumber N/A - auto-add
   resource.mTechniqueName = nullptr;                 // - jammer technique name - use default techniques

   WsfStringId taskType = aVarArgs[1].GetString();                 // taskType
   WsfStringId commName = aVarArgs[6].GetString();                 // comm
   double      simTime  = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                   = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! StartJamming using remote jammer at the given frequency and bandwidth.
//! This is a more obvious form of starting jamming instead of using AssignTask.
//! bool ok = StartJamming(WsfTrack aTrack, string aTaskType, string aResourceName, double aFrequency, double aBandwidth,
//!                        string aTechniqueName, WsfPlatform aAssignee, string aCommName);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager,
                            StartRemoteJamming_4,
                            8,
                            "bool",
                            "WsfTrack, string, string, double, double, string, WsfPlatform, string")
{
   bool                  ok(false);
   WsfTrack*             trackPtr    = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfPlatform*          assigneePtr = static_cast<WsfPlatform*>(aVarArgs[6].GetPointer()->GetAppObject());
   WsfJammerTaskResource resource;                    // resource info
   resource.mNameId        = aVarArgs[2].GetString(); // - jammer name
   resource.mFrequency     = aVarArgs[3].GetDouble(); // - jammer frequency
   resource.mBandwidth     = aVarArgs[4].GetDouble(); // - jammer bandwidth
   resource.mBeamNumber    = 0;                       // - jammer beamNumber N/A - auto-add
   resource.mTechniqueName = aVarArgs[5].GetString(); // - jammer technique name

   WsfStringId taskType = aVarArgs[1].GetString();                 // taskType
   WsfStringId commName = aVarArgs[7].GetString();                 // comm
   double      simTime  = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                   = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! StartJamming using remote jammer at the given frequency and bandwidth.
//! This is a more obvious form of starting jamming instead of using AssignTask.
//! bool ok = StartJamming(WsfTrack aTrack, string aTaskType, string aResourceName, double aFrequency, double aBandwidth,
//!                        int aBeamNumber, WsfPlatform aAssignee);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, StartRemoteJamming_5, 7, "bool", "WsfTrack, string, string, double, double, int, WsfPlatform")
{
   bool                  ok(false);
   WsfTrack*             trackPtr    = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfPlatform*          assigneePtr = static_cast<WsfPlatform*>(aVarArgs[6].GetPointer()->GetAppObject());
   WsfJammerTaskResource resource;                    // resource info
   resource.mNameId        = aVarArgs[2].GetString(); // - jammer name
   resource.mFrequency     = aVarArgs[3].GetDouble(); // - jammer frequency
   resource.mBandwidth     = aVarArgs[4].GetDouble(); // - jammer bandwidth
   resource.mBeamNumber    = aVarArgs[5].GetInt();    // - jammer beamNumber, 0 - auto-add
   resource.mTechniqueName = nullptr;                 // - jammer technique name - use default techniques

   WsfStringId taskType = aVarArgs[1].GetString();                 // taskType
   WsfStringId commName = nullptr;                                 // comm
   double      simTime  = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                   = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! StartJamming using remote jammer at the given frequency and bandwidth.
//! This is a more obvious form of starting jamming instead of using AssignTask.
//! bool ok = StartJamming(WsfTrack aTrack, string aTaskType, string aResourceName, double aFrequency, double aBandwidth,
//!                        int aBeamNumber, string aTechniqueName, WsfPlatform aAssignee);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager,
                            StartRemoteJamming_6,
                            8,
                            "bool",
                            "WsfTrack, string, string, double, double, int, string, WsfPlatform")
{
   bool                  ok(false);
   WsfTrack*             trackPtr    = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfPlatform*          assigneePtr = static_cast<WsfPlatform*>(aVarArgs[7].GetPointer()->GetAppObject());
   WsfJammerTaskResource resource;                    // resource info
   resource.mNameId        = aVarArgs[2].GetString(); // - jammer name
   resource.mFrequency     = aVarArgs[3].GetDouble(); // - jammer frequency
   resource.mBandwidth     = aVarArgs[4].GetDouble(); // - jammer bandwidth
   resource.mBeamNumber    = aVarArgs[5].GetInt();    // - jammer beamNumber, 0 - auto-add
   resource.mTechniqueName = aVarArgs[6].GetString(); // - jammer technique name

   WsfStringId taskType = aVarArgs[1].GetString();                 // taskType
   WsfStringId commName = nullptr;                                 // comm
   double      simTime  = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                   = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! StartJamming using remote jammer at the given frequency and bandwidth.
//! This is a more obvious form of starting jamming instead of using AssignTask.
//! bool ok = StartJamming(WsfTrack aTrack, string aTaskType, string aResourceName, double aFrequency, double aBandwidth,
//!                        int aBeamNumber, WsfPlatform aAssignee, string aCommName);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager,
                            StartRemoteJamming_7,
                            8,
                            "bool",
                            "WsfTrack, string, string, double, double, int, WsfPlatform, string")
{
   bool                  ok(false);
   WsfTrack*             trackPtr    = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfPlatform*          assigneePtr = static_cast<WsfPlatform*>(aVarArgs[6].GetPointer()->GetAppObject());
   WsfJammerTaskResource resource;                    // resource info
   resource.mNameId        = aVarArgs[2].GetString(); // - jammer name
   resource.mFrequency     = aVarArgs[3].GetDouble(); // - jammer frequency
   resource.mBandwidth     = aVarArgs[4].GetDouble(); // - jammer bandwidth
   resource.mBeamNumber    = aVarArgs[5].GetInt();    // - jammer beamNumber, 0 - auto-add
   resource.mTechniqueName = nullptr;                 // - jammer technique name - use default techniques

   WsfStringId taskType = aVarArgs[1].GetString();                 // taskType
   WsfStringId commName = aVarArgs[7].GetString();                 // comm
   double      simTime  = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                   = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! StartJamming using remote jammer at the given frequency and bandwidth.
//! This is a more obvious form of starting jamming instead of using AssignTask.
//! bool ok = StartJamming(WsfTrack aTrack, string aTaskType, string aResourceName, double aFrequency, double aBandwidth,
//!                        int aBeamNumber, string aTechniqueName, WsfPlatform aAssignee, string aCommName);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager,
                            StartRemoteJamming_8,
                            9,
                            "bool",
                            "WsfTrack, string, string, double, double, int, string, WsfPlatform, string")
{
   bool                  ok(false);
   WsfTrack*             trackPtr    = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfPlatform*          assigneePtr = static_cast<WsfPlatform*>(aVarArgs[7].GetPointer()->GetAppObject());
   WsfJammerTaskResource resource;                    // resource info
   resource.mNameId        = aVarArgs[2].GetString(); // - jammer name
   resource.mFrequency     = aVarArgs[3].GetDouble(); // - jammer frequency
   resource.mBandwidth     = aVarArgs[4].GetDouble(); // - jammer bandwidth
   resource.mBeamNumber    = aVarArgs[5].GetInt();    // - jammer beamNumber, 0 - auto-add
   resource.mTechniqueName = aVarArgs[6].GetString(); // - jammer technique name

   WsfStringId taskType = aVarArgs[1].GetString();                 // taskType
   WsfStringId commName = aVarArgs[8].GetString();                 // comm
   double      simTime  = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                   = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}


//! StartJamming using remote jammer at the given frequency and bandwidth.
//! This is a more obvious form of starting jamming instead of using AssignTask.
//! bool ok = StartJamming(WsfTrack aTrack, string aTaskType, string aResourceName, WsfPlatform aAssignee);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, StartRemoteJammingTrack_1, 4, "bool", "WsfTrack, string, string, WsfPlatform")
{
   bool                  ok(false);
   WsfTrack*             trackPtr    = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfPlatform*          assigneePtr = static_cast<WsfPlatform*>(aVarArgs[3].GetPointer()->GetAppObject());
   WsfJammerTaskResource resource;             // resource info
   resource.mNameId = aVarArgs[2].GetString(); // - jammer name
   if (trackPtr->FrequencyValid())
   {
      resource.mFrequency = trackPtr->GetFrequency(); // - jammer frequency
   }
   resource.mBandwidth     = 0.0;     // - jammer bandwidth
   resource.mBeamNumber    = 0;       // - jammer beamNumber N/A - auto-add
   resource.mTechniqueName = nullptr; // - jammer technique name - use default techniques

   WsfStringId taskType = aVarArgs[1].GetString();                 // taskType
   WsfStringId commName = nullptr;                                 // comm
   double      simTime  = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                   = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! StartJamming using remote jammer at the given frequency and bandwidth.
//! This is a more obvious form of starting jamming instead of using AssignTask.
//! bool ok = StartJamming(WsfTrack aTrack, string aTaskType, string aResourceName, string aTechniqueName, WsfPlatform aAssignee);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, StartRemoteJammingTrack_2, 5, "bool", "WsfTrack, string, string, string, WsfPlatform")
{
   bool                  ok(false);
   WsfTrack*             trackPtr    = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfPlatform*          assigneePtr = static_cast<WsfPlatform*>(aVarArgs[4].GetPointer()->GetAppObject());
   WsfJammerTaskResource resource;             // resource info
   resource.mNameId = aVarArgs[2].GetString(); // - jammer name
   if (trackPtr->FrequencyValid())
   {
      resource.mFrequency = trackPtr->GetFrequency(); // - jammer frequency
   }
   resource.mBandwidth     = 0.0;                     // - jammer bandwidth
   resource.mBeamNumber    = 0;                       // - jammer beamNumber N/A - auto-add
   resource.mTechniqueName = aVarArgs[3].GetString(); // - jammer technique name

   WsfStringId taskType = aVarArgs[1].GetString();                 // taskType
   WsfStringId commName = nullptr;                                 // comm
   double      simTime  = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                   = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! StartJamming using remote jammer at the given frequency and bandwidth.
//! This is a more obvious form of starting jamming instead of using AssignTask.
//! bool ok = StartJamming(WsfTrack aTrack, string aTaskType, string aResourceName, WsfPlatform aAssignee, string aCommName);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, StartRemoteJammingTrack_3, 5, "bool", "WsfTrack, string, string, WsfPlatform, string")
{
   bool                  ok(false);
   WsfTrack*             trackPtr    = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfPlatform*          assigneePtr = static_cast<WsfPlatform*>(aVarArgs[3].GetPointer()->GetAppObject());
   WsfJammerTaskResource resource;             // resource info
   resource.mNameId = aVarArgs[2].GetString(); // - jammer name
   if (trackPtr->FrequencyValid())
   {
      resource.mFrequency = trackPtr->GetFrequency(); // - jammer frequency
   }
   resource.mBandwidth     = 0.0;     // - jammer bandwidth
   resource.mBeamNumber    = 0;       // - jammer beamNumber N/A - auto-add
   resource.mTechniqueName = nullptr; // - jammer technique name - use default techniques

   WsfStringId taskType = aVarArgs[1].GetString();                 // taskType
   WsfStringId commName = aVarArgs[4].GetString();                 // comm
   double      simTime  = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                   = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! StartJamming using remote jammer at the given frequency and bandwidth.
//! This is a more obvious form of starting jamming instead of using AssignTask.
//! bool ok = StartJamming(WsfTrack aTrack, string aTaskType, string aResourceName,
//!                        string aTechniqueName, WsfPlatform aAssignee, string aCommName);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, StartRemoteJammingTrack_4, 6, "bool", "WsfTrack, string, string, string, WsfPlatform, string")
{
   bool                  ok(false);
   WsfTrack*             trackPtr    = static_cast<WsfTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfPlatform*          assigneePtr = static_cast<WsfPlatform*>(aVarArgs[4].GetPointer()->GetAppObject());
   WsfJammerTaskResource resource;             // resource info
   resource.mNameId = aVarArgs[2].GetString(); // - jammer name
   if (trackPtr->FrequencyValid())
   {
      resource.mFrequency = trackPtr->GetFrequency(); // - jammer frequency
   }
   resource.mBandwidth     = 0.0;                     // - jammer bandwidth
   resource.mBeamNumber    = 0;                       // - jammer beamNumber N/A - auto-add
   resource.mTechniqueName = aVarArgs[5].GetString(); // - jammer technique name

   WsfStringId taskType = aVarArgs[1].GetString();                 // taskType
   WsfStringId commName = aVarArgs[5].GetString();                 // comm
   double      simTime  = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                   = aObjectPtr->AssignTask(simTime, *trackPtr, taskType, resource, assigneePtr, commName);
   aReturnVal.SetBool(ok);
}

//! StopJamming using local jammer at the given frequency and bandwidth.
//! This is a more obvious form of stopping jamming instead of using CancelTask.
//! bool ok = StopJamming(WsfTrackId aTrackId, string aTaskType, string aResourceType, double aFrequency, double aBandwidth);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, StopLocalJamming_1, 5, "bool", "WsfTrackId, string, string, double, double")
{
   bool                  ok(false);
   WsfTrackId*           trackIdPtr = static_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfJammerTaskResource resource;                 // resource info
   resource.mNameId     = aVarArgs[2].GetString(); // - jammer name
   resource.mFrequency  = aVarArgs[3].GetDouble(); // - jammer frequency
   resource.mBandwidth  = aVarArgs[4].GetDouble(); // - jammer bandwidth
   resource.mBeamNumber = 0;                       // - jammer beamNumber N/A - auto-add

   WsfStringId  taskType      = aVarArgs[1].GetString();                 // taskType
   unsigned int assigneeIndex = 0;                                       // assignee index - N/A
   double       simTime       = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                         = aObjectPtr->CancelTask(simTime, *trackIdPtr, taskType, resource, assigneeIndex);
   aReturnVal.SetBool(ok);
}

//! StopJamming using local jammer at the given frequency and bandwidth.
//! This is a more obvious form of stopping jamming instead of using CancelTask.
//! bool ok = StopJamming(WsfTrackId aTrackId, string aTaskType, string aResourceType, double aFrequency, double
//! aBandwidth, int aBeamNumber);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, StopLocalJamming_2, 6, "bool", "WsfTrackId, string, string, double, double, int")
{
   bool                  ok(false);
   WsfTrackId*           trackIdPtr = static_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfJammerTaskResource resource;                 // resource info
   resource.mNameId     = aVarArgs[2].GetString(); // - jammer name
   resource.mFrequency  = aVarArgs[3].GetDouble(); // - jammer frequency
   resource.mBandwidth  = aVarArgs[4].GetDouble(); // - jammer bandwidth
   resource.mBeamNumber = aVarArgs[5].GetInt();    // - jammer beamNumber, 0 - auto-add

   WsfStringId  taskType      = aVarArgs[1].GetString();                 // taskType
   unsigned int assigneeIndex = 0;                                       // assignee index - N/A
   double       simTime       = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                         = aObjectPtr->CancelTask(simTime, *trackIdPtr, taskType, resource, assigneeIndex);
   aReturnVal.SetBool(ok);
}

//! StopJamming using local jammer at the given frequency and bandwidth.
//! This is a more obvious form of stopping jamming instead of using CancelTask.
//! bool ok = StopJamming(WsfTrackId aTrackId, string aTaskType, string aResourceType);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, StopLocalJammingTrack_1, 3, "bool", "WsfTrackId, string, string")
{
   bool                  ok(false);
   WsfTrackId*           trackIdPtr = static_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfJammerTaskResource resource;                 // resource info
   resource.mNameId     = aVarArgs[2].GetString(); // - jammer name
   resource.mFrequency  = 0.0;                     // - jammer frequency
   resource.mBandwidth  = 0.0;                     // - jammer bandwidth
   resource.mBeamNumber = 0;                       // - jammer beamNumber N/A - auto-add

   WsfStringId  taskType      = aVarArgs[1].GetString();                 // taskType
   unsigned int assigneeIndex = 0;                                       // assignee index - N/A
   double       simTime       = WsfScriptContext::GetTIME_NOW(aContext); // simTime
   ok                         = aObjectPtr->CancelTask(simTime, *trackIdPtr, taskType, resource, assigneeIndex);
   aReturnVal.SetBool(ok);
}

//! StopJamming using remote jammer at the given frequency and bandwidth.
//! This is a more obvious form of stopping jamming instead of using CancelTask.
//! bool ok = StopJamming(WsfPlatform aAssignee, WsfTrackId aTrackId, string aTaskType,
//!                       string aResourceType, double aFrequency, double aBandwidth);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, StopRemoteJamming_1, 6, "bool", "WsfPlatform, WsfTrackId, string, string, double, double")
{
   bool                  ok(false);
   WsfTrackId*           trackIdPtr = static_cast<WsfTrackId*>(aVarArgs[1].GetPointer()->GetAppObject());
   WsfJammerTaskResource resource;                 // resource info
   resource.mNameId     = aVarArgs[3].GetString(); // - jammer name
   resource.mFrequency  = aVarArgs[4].GetDouble(); // - jammer frequency
   resource.mBandwidth  = aVarArgs[5].GetDouble(); // - jammer bandwidth
   resource.mBeamNumber = 0;                       // - jammer beamNumber N/A - auto-add

   WsfStringId  taskType      = aVarArgs[2].GetString();                              // taskType
   unsigned int assigneeIndex = (unsigned int)aVarArgs[0].GetPointer()->GetAuxData(); // assignee index
   double       simTime       = WsfScriptContext::GetTIME_NOW(aContext);              // simTime
   ok                         = aObjectPtr->CancelTask(simTime, *trackIdPtr, taskType, resource, assigneeIndex);
   aReturnVal.SetBool(ok);
}

//! StopJamming using remote jammer at the given frequency and bandwidth.
//! This is a more obvious form of stopping jamming instead of using CancelTask.
//! bool ok = StopJamming(WsfPlatform aAssignee, WsfTrackId aTrackId, string aTaskType,
//!                       string aResourceType, double aFrequency, double aBandwidth);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager,
                            StopRemoteJamming_2,
                            7,
                            "bool",
                            "WsfPlatform, WsfTrackId, string, string, double, double, int")
{
   bool                  ok(false);
   WsfTrackId*           trackIdPtr = static_cast<WsfTrackId*>(aVarArgs[1].GetPointer()->GetAppObject());
   WsfJammerTaskResource resource;                 // resource info
   resource.mNameId     = aVarArgs[3].GetString(); // - jammer name
   resource.mFrequency  = aVarArgs[4].GetDouble(); // - jammer frequency
   resource.mBandwidth  = aVarArgs[5].GetDouble(); // - jammer bandwidth
   resource.mBeamNumber = aVarArgs[6].GetInt();    // - jammer beamNumber, 0 - auto-add

   WsfStringId  taskType      = aVarArgs[2].GetString();                              // taskType
   unsigned int assigneeIndex = (unsigned int)aVarArgs[0].GetPointer()->GetAuxData(); // assignee index
   double       simTime       = WsfScriptContext::GetTIME_NOW(aContext);              // simTime
   ok                         = aObjectPtr->CancelTask(simTime, *trackIdPtr, taskType, resource, assigneeIndex);
   aReturnVal.SetBool(ok);
}

//! StopJamming using remote jammer at the given frequency and bandwidth.
//! This is a more obvious form of stopping jamming instead of using CancelTask.
//! bool ok = StopJamming(WsfPlatform aAssignee, WsfTrackId aTrackId, string aTaskType, string aResourceType);
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTaskManager, StopRemoteJammingTrack_1, 4, "bool", "WsfPlatform, WsfTrackId, string, string")
{
   bool                  ok(false);
   WsfTrackId*           trackIdPtr = static_cast<WsfTrackId*>(aVarArgs[1].GetPointer()->GetAppObject());
   WsfJammerTaskResource resource;                 // resource info
   resource.mNameId     = aVarArgs[3].GetString(); // - jammer name
   resource.mFrequency  = 0.0;                     // - jammer frequency
   resource.mBandwidth  = 0.0;                     // - jammer bandwidth
   resource.mBeamNumber = 0;                       // - jammer beamNumber N/A - auto-add

   WsfStringId  taskType      = aVarArgs[2].GetString();                              // taskType
   unsigned int assigneeIndex = (unsigned int)aVarArgs[0].GetPointer()->GetAuxData(); // assignee index
   double       simTime       = WsfScriptContext::GetTIME_NOW(aContext);              // simTime
   ok                         = aObjectPtr->CancelTask(simTime, *trackIdPtr, taskType, resource, assigneeIndex);
   aReturnVal.SetBool(ok);
}

}; // namespace WsfTaskManagerScriptMethods

namespace WsfTrackScriptMethods
{

// TODO-HACK A track is 'believed alive' if the associated platform is alive (i.e.: still exists).
// TODO-HACK A track without a defined associated platform is defined to be 'alive'.
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTrack, BelievedAlive, 0, "bool", "") // NO_DOC | SUPPRESSOR
{
   bool   believedAlive = true;
   size_t targetIndex   = aObjectPtr->GetTargetIndex();
   if (targetIndex != 0)
   {
      WsfPlatform* targetPtr = SIMULATION->GetPlatformByIndex(targetIndex);
      if ((targetPtr == nullptr) || targetPtr->IsBroken() || targetPtr->IsDeleted())
      {
         believedAlive = false;
      }
   }
   aReturnVal.SetBool(believedAlive);
}

// TODO-HACK A track is 'believed dead' if the associated platform is dead (i.e.: no longer exists).
// TODO-HACK A track without a defined associated platform is defined to be 'alive'.
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTrack, BelievedDead, 0, "bool", "") // NO_DOC | SUPPRESSOR
{
   bool   believedDead = false;
   size_t targetIndex  = aObjectPtr->GetTargetIndex();
   if (targetIndex != 0)
   {
      WsfPlatform* targetPtr = SIMULATION->GetPlatformByIndex(targetIndex);
      if ((targetPtr == nullptr) || targetPtr->IsBroken() || targetPtr->IsDeleted())
      {
         believedDead = true;
      }
   }
   aReturnVal.SetBool(believedDead);
}


//! Uses truth knowledge to report whether or not the target of the track is dead or deleted
UT_DEFINE_SCRIPT_METHOD_EXT(WsfTrack, TargetKilled, 0, "bool", "")
{
   WsfPlatform* platformPtr = SIMULATION->GetPlatformByIndex(aObjectPtr->GetTargetIndex());
   aReturnVal.SetBool(platformPtr == nullptr || platformPtr->GetDamageFactor() == 1 ? true : false);
}
} // namespace WsfTrackScriptMethods

// virtual
bool WsfMilScriptExtensions::AddExtClassMethods(const std::string& aClassName,
                                                const std::string& aBaseName,
                                                UtScriptTypes*     aScriptTypesPtr)
{
   if (aBaseName == aClassName)
   {
      // Should add base classes manually prior to initialization phase
   }
   else if (aBaseName == "WsfComm")
   {
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfCommScriptMethods::JammingPerceived>());
   }
   else if (aBaseName == "WsfPlatform")
   {
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfPlatformScriptMethods::PerceptionProcessor>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfPlatformScriptMethods::PerceivedThreats>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfPlatformScriptMethods::PerceivedAssets>());

      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfPlatformScriptMethods::UplinkProcessor>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfPlatformScriptMethods::IsUplinkingTo>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfPlatformScriptMethods::UplinkCount>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfPlatformScriptMethods::UplinkCapable>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfPlatformScriptMethods::UplinkPlatformEntry>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfPlatformScriptMethods::StartUplinking_1>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfPlatformScriptMethods::StartUplinking_2>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfPlatformScriptMethods::StopUplinking>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfPlatformScriptMethods::MaxSupportedUplinks>());

      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfPlatformScriptMethods::Detonate>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfPlatformScriptMethods::Weapon>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfPlatformScriptMethods::WeaponCount>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfPlatformScriptMethods::WeaponEntry>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfPlatformScriptMethods::TimeSinceWeaponLastFiredFor>());
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfPlatformScriptMethods::TimeSinceWeaponLastTerminatedFor>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfPlatformScriptMethods::WeaponsPendingFor>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfPlatformScriptMethods::WeaponsActiveFor>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfPlatformScriptMethods::RoundsCompleteFor>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfPlatformScriptMethods::RoundsFiredAt>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfPlatformScriptMethods::SalvosFiredAt>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfPlatformScriptMethods::ActiveWeaponPlatformsFor>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfPlatformScriptMethods::WeaponEngagement>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfPlatformScriptMethods::HasOperationalSubordinate>()); // NO_DOC | SUPPRESSOR
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfPlatformScriptMethods::GetSubsCentroid>(
                                         "GetSubsCentroid")); // NO_DOC | DEPRECATED - should not have the 'get'
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfPlatformScriptMethods::GetSubsCentroid>(
                                         "SubordinatesCentroid"));
      // replace WithinFieldOfView to also look at weapons
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfPlatformScriptMethods::WithinFieldOfView_1>(
                                         "WithinFieldOfView")); // WithinFieldOfView(WsfTrack, mySensorOrJammer)
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfPlatformScriptMethods::WithinFieldOfView_2>(
                                         "WithinFieldOfView")); // WithinFieldOfView(WsfPlatform, mySensorOrJammer)
   }
   else if (aBaseName == "WsfProcessor")
   {
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfProcessScriptMethods::TargetAllocated>()); // Deprecated
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfProcessScriptMethods::TargetEngaged>()); // Deprecated
   }
   else if (aBaseName == "WsfSensor")
   {
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfSensorScriptMethods::IsEP_TechniqueActive_1>(
                                         "IsEP_TechniqueActive"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfSensorScriptMethods::IsEP_TechniqueActive_2>(
                                         "IsEP_TechniqueActive"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfSensorScriptMethods::SelectEP_Technique_1>(
                                         "SelectEP_Technique"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfSensorScriptMethods::SelectEP_Technique_2>(
                                         "SelectEP_Technique"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfSensorScriptMethods::DeselectEP_Technique_1>(
                                         "DeselectEP_Technique"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfSensorScriptMethods::DeselectEP_Technique_2>(
                                         "DeselectEP_Technique"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfSensorScriptMethods::JammingPerceived_1>("JammingPerceived"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfSensorScriptMethods::JammingPerceived_2>("JammingPerceived"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfSensorScriptMethods::ContinuousJammingPerceived_1>(
                                         "ContinuousJammingPerceived"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfSensorScriptMethods::ContinuousJammingPerceived_2>(
                                         "ContinuousJammingPerceived"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfSensorScriptMethods::PulseJammingPerceived_1>(
                                         "PulseJammingPerceived"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfSensorScriptMethods::PulseJammingPerceived_2>(
                                         "PulseJammingPerceived"));

      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfSensorScriptMethods::LaserCode_1>("LaserCode"));
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfSensorScriptMethods::LaserCode_2>("LaserCode"));
   }
   else if (aBaseName == "WsfTaskManager")
   {
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::TimeSinceWeaponLastFiredFor>());
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::TimeSinceWeaponLastTerminatedFor>());
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::WeaponsActiveFor_1>(
                                         "WeaponsActiveFor"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::WeaponsActiveFor_2>(
                                         "WeaponsActiveFor"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::RoundsFiredAt_1>("RoundsFiredAt"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::RoundsFiredAt_1>("WeaponsFiredAt"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::RoundsFiredAt_1>("WeaponsFiredFor"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::RoundsFiredAt_2>("RoundsFiredAt"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::RoundsFiredAt_2>("WeaponsFiredAt"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::RoundsFiredAt_2>("WeaponsFiredFor"));
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfTaskManagerScriptMethods::SalvosFiredAt>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfTaskManagerScriptMethods::HaveFiredAt>());

      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfTaskManagerScriptMethods::FireLocalWeapon>("Fire"));
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfTaskManagerScriptMethods::FireLocalWeapon>("FireAt"));
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfTaskManagerScriptMethods::FireRemoteWeapon_1>("Fire"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::FireRemoteWeapon_1>("FireAt"));
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfTaskManagerScriptMethods::FireRemoteWeapon_2>("Fire"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::FireRemoteWeapon_2>("FireAt"));
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfTaskManagerScriptMethods::AbortFiringAt>());

      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::StartUplinking_1>("StartUplinking"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::StartUplinking_2>("StartUplinking"));

      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::StartLocalJamming_1>(
                                         "StartJamming"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::StartLocalJamming_2>(
                                         "StartJamming"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::StartLocalJamming_3>(
                                         "StartJamming"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::StartLocalJamming_4>(
                                         "StartJamming"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::StartLocalJammingTrack_1>(
                                         "StartJamming"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::StartLocalJammingTrack_2>(
                                         "StartJamming"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::StartRemoteJamming_1>(
                                         "StartJamming"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::StartRemoteJamming_2>(
                                         "StartJamming"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::StartRemoteJamming_3>(
                                         "StartJamming"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::StartRemoteJamming_4>(
                                         "StartJamming"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::StartRemoteJamming_5>(
                                         "StartJamming"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::StartRemoteJamming_6>(
                                         "StartJamming"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::StartRemoteJamming_7>(
                                         "StartJamming"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::StartRemoteJamming_8>(
                                         "StartJamming"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::StartRemoteJammingTrack_1>(
                                         "StartJamming"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::StartRemoteJammingTrack_2>(
                                         "StartJamming"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::StartRemoteJammingTrack_3>(
                                         "StartJamming"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::StartRemoteJammingTrack_4>(
                                         "StartJamming"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::StopLocalJamming_1>("StopJamming"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::StopLocalJamming_2>("StopJamming"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::StopLocalJammingTrack_1>(
                                         "StopJamming"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::StopRemoteJamming_1>("StopJamming"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::StopRemoteJamming_2>("StopJamming"));
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfTaskManagerScriptMethods::StopRemoteJammingTrack_1>(
                                         "StopJamming"));
   }
   else if (aBaseName == "WsfTrack")
   {
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfTrackScriptMethods::BelievedAlive>()); // Deprecated
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfTrackScriptMethods::BelievedDead>()); // Deprecated
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfTrackScriptMethods::TargetKilled>());
   }
   else if (aBaseName == "WsfEM_Interaction")
   {
      aScriptTypesPtr->AddClassMethod(aClassName,
                                      ut::make_unique<WsfEM_InteractionScriptMethods::SignalToNoiseClutterJamming>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfEM_InteractionScriptMethods::NoiseJammerPower>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfEM_InteractionScriptMethods::PulseJammerPower>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfEM_InteractionScriptMethods::CoherentJammerPower>());
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<WsfEM_InteractionScriptMethods::EW_Effects>());
   }
   else
   {
      return false;
   }
   return true;
}

void WsfMilScriptExtensions::ExtendScriptClasses(UtScriptTypes& aTypes)
{
   // WsfComm
   {
      aTypes.AddClassMethod("WsfComm", ut::make_unique<WsfCommScriptMethods::JammingPerceived>());
   }

   // WsfPlatform
   {
      using namespace WsfPlatformScriptMethods;
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<PerceptionProcessor>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<PerceivedThreats>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<PerceivedAssets>());

      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<UplinkProcessor>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<IsUplinkingTo>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<UplinkCount>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<UplinkCapable>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<UplinkPlatformEntry>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<StartUplinking_1>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<StartUplinking_2>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<StopUplinking>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<MaxSupportedUplinks>());

      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<Detonate>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<Weapon>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<WeaponCount>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<WeaponEntry>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<TimeSinceWeaponLastFiredFor>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<TimeSinceWeaponLastTerminatedFor>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<WeaponsPendingFor>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<WeaponsActiveFor>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<RoundsCompleteFor>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<RoundsFiredAt>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<SalvosFiredAt>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<ActiveWeaponPlatformsFor>());
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<WeaponEngagement>());
      aTypes.AddClassMethod("WsfPlatform",
                            ut::make_unique<WsfPlatformScriptMethods::HasOperationalSubordinate>()); // NO_DOC | SUPPRESSOR
      aTypes.AddClassMethod("WsfPlatform",
                            ut::make_unique<GetSubsCentroid>("GetSubsCentroid")); // NO_DOC | DEPRECATED
                                                                                  // - should not have the 'get'
      aTypes.AddClassMethod("WsfPlatform", ut::make_unique<GetSubsCentroid>("SubordinatesCentroid"));
      // replace WithinFieldOfView to also look at weapons
      aTypes.AddClassMethod("WsfPlatform",
                            ut::make_unique<WithinFieldOfView_1>("WithinFieldOfView")); // WithinFieldOfView(WsfTrack,
                                                                                        // mySensorOrJammer)
      aTypes.AddClassMethod("WsfPlatform",
                            ut::make_unique<WithinFieldOfView_2>("WithinFieldOfView")); // WithinFieldOfView(WsfPlatform,
                                                                                        // mySensorOrJammer)
   }

   // WsfProcessor
   {
      aTypes.AddClassMethod("WsfProcessor", ut::make_unique<WsfProcessScriptMethods::TargetAllocated>()); // Deprecated
      aTypes.AddClassMethod("WsfProcessor", ut::make_unique<WsfProcessScriptMethods::TargetEngaged>());   // Deprecated
   }

   // WsfSensor
   {
      aTypes.AddClassMethod("WsfSensor",
                            ut::make_unique<WsfSensorScriptMethods::IsEP_TechniqueActive_1>("IsEP_TechniqueActive"));
      aTypes.AddClassMethod("WsfSensor",
                            ut::make_unique<WsfSensorScriptMethods::IsEP_TechniqueActive_2>("IsEP_TechniqueActive"));
      aTypes.AddClassMethod("WsfSensor",
                            ut::make_unique<WsfSensorScriptMethods::SelectEP_Technique_1>("SelectEP_Technique"));
      aTypes.AddClassMethod("WsfSensor",
                            ut::make_unique<WsfSensorScriptMethods::SelectEP_Technique_2>("SelectEP_Technique"));
      aTypes.AddClassMethod("WsfSensor",
                            ut::make_unique<WsfSensorScriptMethods::DeselectEP_Technique_1>("DeselectEP_Technique"));
      aTypes.AddClassMethod("WsfSensor",
                            ut::make_unique<WsfSensorScriptMethods::DeselectEP_Technique_2>("DeselectEP_Technique"));
      aTypes.AddClassMethod("WsfSensor", ut::make_unique<WsfSensorScriptMethods::JammingPerceived_1>("JammingPerceived"));
      aTypes.AddClassMethod("WsfSensor", ut::make_unique<WsfSensorScriptMethods::JammingPerceived_2>("JammingPerceived"));
      aTypes.AddClassMethod("WsfSensor",
                            ut::make_unique<WsfSensorScriptMethods::ContinuousJammingPerceived_1>(
                               "ContinuousJammingPerceived"));
      aTypes.AddClassMethod("WsfSensor",
                            ut::make_unique<WsfSensorScriptMethods::ContinuousJammingPerceived_2>(
                               "ContinuousJammingPerceived"));
      aTypes.AddClassMethod("WsfSensor",
                            ut::make_unique<WsfSensorScriptMethods::PulseJammingPerceived_1>("PulseJammingPerceived"));
      aTypes.AddClassMethod("WsfSensor",
                            ut::make_unique<WsfSensorScriptMethods::PulseJammingPerceived_2>("PulseJammingPerceived"));

      aTypes.AddClassMethod("WsfSensor", ut::make_unique<WsfSensorScriptMethods::LaserCode_1>("LaserCode"));
      aTypes.AddClassMethod("WsfSensor", ut::make_unique<WsfSensorScriptMethods::LaserCode_2>("LaserCode"));
   }

   // WsfTaskManager
   {
      aTypes.AddClassMethod("WsfTaskManager", ut::make_unique<WsfTaskManagerScriptMethods::TimeSinceWeaponLastFiredFor>());
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::TimeSinceWeaponLastTerminatedFor>());
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::WeaponsActiveFor_1>("WeaponsActiveFor"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::WeaponsActiveFor_2>("WeaponsActiveFor"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::RoundsFiredAt_1>("RoundsFiredAt"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::RoundsFiredAt_1>("WeaponsFiredAt"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::RoundsFiredAt_1>("WeaponsFiredFor"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::RoundsFiredAt_2>("RoundsFiredAt"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::RoundsFiredAt_2>("WeaponsFiredAt"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::RoundsFiredAt_2>("WeaponsFiredFor"));
      aTypes.AddClassMethod("WsfTaskManager", ut::make_unique<WsfTaskManagerScriptMethods::SalvosFiredAt>());
      aTypes.AddClassMethod("WsfTaskManager", ut::make_unique<WsfTaskManagerScriptMethods::HaveFiredAt>());

      aTypes.AddClassMethod("WsfTaskManager", ut::make_unique<WsfTaskManagerScriptMethods::FireLocalWeapon>("Fire"));
      aTypes.AddClassMethod("WsfTaskManager", ut::make_unique<WsfTaskManagerScriptMethods::FireLocalWeapon>("FireAt"));
      aTypes.AddClassMethod("WsfTaskManager", ut::make_unique<WsfTaskManagerScriptMethods::FireRemoteWeapon_1>("Fire"));
      aTypes.AddClassMethod("WsfTaskManager", ut::make_unique<WsfTaskManagerScriptMethods::FireRemoteWeapon_1>("FireAt"));
      aTypes.AddClassMethod("WsfTaskManager", ut::make_unique<WsfTaskManagerScriptMethods::FireRemoteWeapon_2>("Fire"));
      aTypes.AddClassMethod("WsfTaskManager", ut::make_unique<WsfTaskManagerScriptMethods::FireRemoteWeapon_2>("FireAt"));
      aTypes.AddClassMethod("WsfTaskManager", ut::make_unique<WsfTaskManagerScriptMethods::AbortFiringAt>());

      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::StartUplinking_1>("StartUplinking"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::StartUplinking_2>("StartUplinking"));

      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::StartLocalJamming_1>("StartJamming"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::StartLocalJamming_2>("StartJamming"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::StartLocalJamming_3>("StartJamming"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::StartLocalJamming_4>("StartJamming"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::StartLocalJammingTrack_1>("StartJamming"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::StartLocalJammingTrack_2>("StartJamming"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::StartRemoteJamming_1>("StartJamming"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::StartRemoteJamming_2>("StartJamming"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::StartRemoteJamming_3>("StartJamming"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::StartRemoteJamming_4>("StartJamming"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::StartRemoteJamming_5>("StartJamming"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::StartRemoteJamming_6>("StartJamming"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::StartRemoteJamming_7>("StartJamming"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::StartRemoteJamming_8>("StartJamming"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::StartRemoteJammingTrack_1>("StartJamming"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::StartRemoteJammingTrack_2>("StartJamming"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::StartRemoteJammingTrack_3>("StartJamming"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::StartRemoteJammingTrack_4>("StartJamming"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::StopLocalJamming_1>("StopJamming"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::StopLocalJamming_2>("StopJamming"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::StopLocalJammingTrack_1>("StopJamming"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::StopRemoteJamming_1>("StopJamming"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::StopRemoteJamming_2>("StopJamming"));
      aTypes.AddClassMethod("WsfTaskManager",
                            ut::make_unique<WsfTaskManagerScriptMethods::StopRemoteJammingTrack_1>("StopJamming"));
   }

   // WsfTrack
   {
      aTypes.AddClassMethod("WsfTrack", ut::make_unique<WsfTrackScriptMethods::BelievedAlive>()); // Deprecated
      aTypes.AddClassMethod("WsfTrack", ut::make_unique<WsfTrackScriptMethods::BelievedDead>());  // Deprecated
      aTypes.AddClassMethod("WsfTrack", ut::make_unique<WsfTrackScriptMethods::TargetKilled>());
   }

   // WsfEM_Interaction
   {
      aTypes.AddClassMethod("WsfEM_Interaction",
                            ut::make_unique<WsfEM_InteractionScriptMethods::SignalToNoiseClutterJamming>());
      aTypes.AddClassMethod("WsfEM_Interaction", ut::make_unique<WsfEM_InteractionScriptMethods::NoiseJammerPower>());
      aTypes.AddClassMethod("WsfEM_Interaction", ut::make_unique<WsfEM_InteractionScriptMethods::PulseJammerPower>());
      aTypes.AddClassMethod("WsfEM_Interaction", ut::make_unique<WsfEM_InteractionScriptMethods::CoherentJammerPower>());
      aTypes.AddClassMethod("WsfEM_Interaction", ut::make_unique<WsfEM_InteractionScriptMethods::EW_Effects>());
   }

   // Register the signatures that are included with the extension.
   WsfAcousticSignature::RegisterScriptMethods(aTypes);
   WsfInherentContrast::RegisterScriptMethods(aTypes);
   WsfInfraredSignature::RegisterScriptMethods(aTypes);
   WsfOpticalReflectivity::RegisterScriptMethods(aTypes);
   WsfOpticalSignature::RegisterScriptMethods(aTypes);

   // Register the task extras that are included in these extension
   WsfWeaponTask::RegisterScriptMethods(aTypes);

   aTypes.RegisterExtension(ut::make_unique<WsfMilScriptExtensions>());
}
