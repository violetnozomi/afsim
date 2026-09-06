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

#include "WsfUplinkProcessor.hpp"

#include "UtCallback.hpp"
#include "UtMemory.hpp"
#include "WsfComm.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackMessage.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponObserver.hpp"

WsfUplinkProcessor::WsfUplinkProcessor(const WsfScenario& aScenario)
   : WsfScriptProcessor(aScenario, "WsfUplinkProcessor", "PROCESSOR")
   , mMaxSupportedUplinks(0)
   , mUplinkPathList()
   , mTrackUplinkList()
   , mUplinkList()
   , mActiveWeaponList()
   , mLinkToSensorsOnInit(false)
   , mUplinkSendInterval(0.0)
   , mCallbacks()
{
}

WsfUplinkProcessor::WsfUplinkProcessor(const WsfUplinkProcessor& aSrc)
   : WsfScriptProcessor(aSrc)
   , mMaxSupportedUplinks(aSrc.mMaxSupportedUplinks)
   , mUplinkPathList(aSrc.mUplinkPathList)
   , mTrackUplinkList(aSrc.mTrackUplinkList)
   , mUplinkList(aSrc.mUplinkList)
   , mActiveWeaponList(aSrc.mActiveWeaponList)
   , mLinkToSensorsOnInit(aSrc.mLinkToSensorsOnInit)
   , mUplinkSendInterval(aSrc.mUplinkSendInterval)
   , mCallbacks()
{
}

bool WsfUplinkProcessor::ProcessInput(UtInput& aInput)
{
   std::string command(aInput.GetCommand());
   bool        wasProcessed = true;

   if (command == "weapon_uplink_path")
   {
      std::string sensorName;
      std::string commName;
      aInput.ReadValue(sensorName);
      aInput.ReadValue(commName);
      AddUplinkPath(sensorName, commName);
   }
   else if (command == "maximum_weapon_uplinks")
   {
      int temp;
      aInput.ReadValue(temp);
      mMaxSupportedUplinks = (unsigned int)temp;
   }
   else
   {
      wasProcessed = WsfScriptProcessor::ProcessInput(aInput);
   }
   return wasProcessed;
}

// virtual
bool WsfUplinkProcessor::Initialize(double aSimTime)
{
   bool ok = WsfScriptProcessor::Initialize(aSimTime);
   if (ok)
   {
      // setup callbacks
      WsfTrackManager* trackManagerPtr = &(GetPlatform()->GetTrackManager());
      mCallbacks.Add(trackManagerPtr->LocalTrackInitiated.Connect(&WsfUplinkProcessor::LocalTrackInitiated, this)); // uplink track
      mCallbacks.Add(trackManagerPtr->LocalTrackUpdated.Connect(&WsfUplinkProcessor::LocalTrackUpdated, this)); // uplink track
      mCallbacks.Add(trackManagerPtr->RawTrackReceived.Connect(&WsfUplinkProcessor::RawTrackReceived, this)); // uplink track
      mCallbacks.Add(trackManagerPtr->LocalTrackCorrelation.Connect(&WsfUplinkProcessor::LocalTrackCorrelation,
                                                                    this)); // local track correlation
      mCallbacks.Add(WsfObserver::WeaponTerminated(GetSimulation()).Connect(&WsfUplinkProcessor::WeaponTerminated, this));

      if (mLinkToSensorsOnInit)
      {
         // setup internal links from every sensor to this processor (for any track messages that might need processed)
         for (WsfComponentList::RoleIterator<WsfSensor> iter(*GetPlatform()); !iter.AtEnd(); ++iter)
         {
            iter->GetInternalLinks().AddRecipient(this);
         }
      }
   }
   return ok;
}


void WsfUplinkProcessor::AddUplinkPath(const std::string& aSensorName, const std::string& aCommName)
{
   UplinkPath uplinkPath;
   uplinkPath.mSensorName = WsfStringId(aSensorName);
   uplinkPath.mCommName   = WsfStringId(aCommName);
   mUplinkPathList.push_back(uplinkPath);
}

bool WsfUplinkProcessor::UplinkCapable()
{
   if (IsUplinkPathListEmpty())
   {
      return false;
   }
   return true;
}

bool WsfUplinkProcessor::StartUplinking(size_t aReceiverIndex, size_t aTargetIndex)
{
   WsfPlatform* weaponPlatformPtr = GetSimulation()->GetPlatformByIndex(aReceiverIndex);
   if (weaponPlatformPtr != nullptr)
   {
      WsfTrack*    trackPtr    = nullptr;
      WsfPlatform* tempPlatPtr = GetSimulation()->GetPlatformByIndex(aTargetIndex);
      if (tempPlatPtr != nullptr)
      {
         // create an empty track to hold the target info
         WsfTrack* tempTrackPtr = new WsfTrack;
         tempTrackPtr->SetTargetIndex(tempPlatPtr->GetIndex());
         tempTrackPtr->SetTargetName(tempPlatPtr->GetNameId());
         trackPtr = tempTrackPtr;
         bool val = StartUplinking(weaponPlatformPtr, trackPtr);
         delete tempTrackPtr;
         return val;
      }
      return StartUplinking(weaponPlatformPtr, trackPtr);
   }
   return false;
}

bool WsfUplinkProcessor::StartUplinking(WsfPlatform* aReceiverPtr, WsfTrack* aTargetPtr)
{
   if (aReceiverPtr == nullptr)
   {
      return false;
   }
   if (aTargetPtr == nullptr)
   {
      // find the target of the weapon
      aTargetPtr = (WsfTrack*)aReceiverPtr->GetTrackManager().GetCurrentTarget();
      if (aTargetPtr == nullptr)
      {
         return false;
      }
   }

   if (mMaxSupportedUplinks > 0 && WeaponUplinkCount() >= mMaxSupportedUplinks)
   {
      // don't create any new uplinks, only allow this uplink if the weapon is already being uplinked to
      //(perhaps this new uplink is for an additional track)
      if (!IsUplinkingTo(aReceiverPtr))
      {
         return false;
      }
   }

   // make sure we have an entry in our "active weapon" list for this weapon as well
   bool found = false;
   for (auto awli = mActiveWeaponList.begin(); awli != mActiveWeaponList.end();)
   {
      if (awli->mPlatformIndex == aReceiverPtr->GetIndex() && awli->mTargetIndex == aTargetPtr->GetTargetIndex())
      {
         found = true;
         break;
      }
      else
      {
         ++awli;
      }
   }
   if (!found)
   {
      WsfWeaponEngagement* engagementPtr = WsfWeaponEngagement::Find(*aReceiverPtr);
      ActiveWeapon         activeWeapon;
      activeWeapon.mTaskTrackId      = aTargetPtr->GetTrackId();
      activeWeapon.mTargetIndex      = aTargetPtr->GetTargetIndex();
      activeWeapon.mWeaponSystemName = (engagementPtr != nullptr) ? (engagementPtr->GetWeaponSystemName()) : WsfStringId();
      activeWeapon.mPlatformIndex    = aReceiverPtr->GetIndex();
      activeWeapon.mSerialNumber     = (engagementPtr != nullptr) ? (engagementPtr->GetSerialNumber()) : (0);
      mActiveWeaponList.push_back(activeWeapon);
   }
   for (UplinkPathList::const_iterator upli = mUplinkPathList.begin(); upli != mUplinkPathList.end(); ++upli)
   {
      const UplinkPath& uplinkPath = *upli;
      auto              commPtr    = GetPlatform()->GetComponent<wsf::comm::Comm>(uplinkPath.mCommName);
      if (commPtr != nullptr)
      {
         TrackUplink trackUplink;
         trackUplink.mSensorName  = uplinkPath.mSensorName;
         trackUplink.mTargetIndex = aTargetPtr->GetTargetIndex();
         trackUplink.mPlatformPtr = aReceiverPtr;
         trackUplink.mCommPtr     = commPtr;
         mTrackUplinkList.push_back(trackUplink);

         double time = GetSimulation()->GetSimTime();
         WsfObserver::UplinkInitiated(GetSimulation())(time, GetPlatform(), aReceiverPtr, aTargetPtr);
      }
   }
   // LBM - no uplink paths? this is likely a track manager style uplink, add an entry to the mUplinkList
   if (mUplinkPathList.empty())
   {
      Uplink uplinkData;
      uplinkData.mTaskTrackId          = WsfTrackId();
      uplinkData.mSensorTrackId        = WsfTrackId();
      uplinkData.mUplinkDestinationPtr = aReceiverPtr;
      uplinkData.mCommPtr              = GetUplinkCommToRecipient(GetPlatform(), aReceiverPtr);
      uplinkData.mTrackSource          = nullptr;
      uplinkData.mTaskId               = 0;
      uplinkData.mLastSendTime         = -1.0E20;

      WsfTrackManager& mgr = GetPlatform()->GetTrackManager();
      if (mgr.FindTrack(aTargetPtr->GetTrackId()) == nullptr)
      {
         // find local track id, from target index
         for (unsigned int i = 0; i < mgr.GetTrackCount(); ++i)
         {
            WsfLocalTrack* tPtr = mgr.GetTrackEntry(i);
            if (tPtr->GetTargetIndex() == aTargetPtr->GetTargetIndex())
            {
               uplinkData.mLocalTrackId = tPtr->GetTrackId();
               break;
            }
         }
      }
      else
      {
         uplinkData.mLocalTrackId = aTargetPtr->GetTrackId();
      }

      mUplinkList.push_back(uplinkData);

      double time = GetSimulation()->GetSimTime();
      WsfObserver::UplinkInitiated(GetSimulation())(time, GetPlatform(), aReceiverPtr, aTargetPtr);
   }
   return true;
}

bool WsfUplinkProcessor::StopUplinking(size_t aReceiverIndex)
{
   int numRemoved = 0;
   // remove any weapon uplinks for this weapon
   for (auto tuli = mTrackUplinkList.begin(); tuli != mTrackUplinkList.end();)
   {
      if (aReceiverIndex == tuli->mPlatformPtr->GetIndex())
      {
         tuli = mTrackUplinkList.erase(tuli);
         ++numRemoved;
      }
      else
      {
         ++tuli;
      }
   }

   for (size_t i = 0; i < mUplinkList.size();)
   {
      Uplink& uplink = mUplinkList[i];
      if (uplink.mUplinkDestinationPtr->GetIndex() == aReceiverIndex)
      {
         mUplinkList.erase(mUplinkList.begin() + i);
         ++numRemoved;
      }
      else
      {
         ++i;
      }
   }

   // if this platform didn't fire the weapon, and isn't supporting it anymore, remove it from the active weapons list
   WsfPlatform* receiverPtr = GetSimulation()->GetPlatformByIndex(aReceiverIndex);

   if (numRemoved > 0)
   {
      double    time           = GetSimulation()->GetSimTime();
      WsfTrack* targetTrackPtr = nullptr;
      if (receiverPtr != nullptr && receiverPtr->GetTrackManager().GetCurrentTarget() != nullptr)
      {
         targetTrackPtr = receiverPtr->GetTrackManager().GetCurrentTarget()->Clone();
      }
      WsfObserver::UplinkDropped(GetSimulation())(time, GetPlatform(), receiverPtr, targetTrackPtr);
      delete targetTrackPtr;
   }

   WsfWeaponEngagement* engagementPtr = (receiverPtr != nullptr) ? WsfWeaponEngagement::Find(*receiverPtr) : nullptr;
   size_t               firingPlatformIndex = (engagementPtr != nullptr) ? engagementPtr->GetFiringPlatformIndex() : 0;
   bool                 drop                = (GetPlatform()->GetIndex() != firingPlatformIndex);
   if (drop)
   {
      // Remove any associated 'active weapon' entry
      for (auto awli = mActiveWeaponList.begin(); awli != mActiveWeaponList.end();)
      {
         if (awli->mPlatformIndex == aReceiverIndex)
         {
            awli = mActiveWeaponList.erase(awli);
         }
         else
         {
            ++awli;
         }
      }
   }
   return true;
}

bool WsfUplinkProcessor::StopUplinking(WsfPlatform* aReceiverPtr)
{
   if (aReceiverPtr == nullptr)
   {
      return false;
   }
   return StopUplinking((int)aReceiverPtr->GetIndex());
}

bool WsfUplinkProcessor::IsUplinkingTo(WsfPlatform* aReceiverPtr)
{
   if (aReceiverPtr == nullptr)
   {
      return false;
   }
   for (auto& uit : mTrackUplinkList)
   {
      size_t uplinkWeaponIndex = uit.mPlatformPtr->GetIndex();
      if (uplinkWeaponIndex == aReceiverPtr->GetIndex())
      {
         return true;
      }
   }
   for (auto& uit : mUplinkList)
   {
      size_t weaponIndex = uit.mUplinkDestinationPtr->GetIndex();
      if (weaponIndex == aReceiverPtr->GetIndex())
      {
         return true;
      }
   }
   return false;
}

unsigned int WsfUplinkProcessor::WeaponUplinkCount()
{
   // count unique weapons being supported
   std::map<size_t, size_t> uniqueWeaponUplinks; // to keep track
   for (auto& uit : mTrackUplinkList)
   {
      size_t weaponIndex               = uit.mPlatformPtr->GetIndex();
      uniqueWeaponUplinks[weaponIndex] = uniqueWeaponUplinks[weaponIndex] + 1;
   }
   // count for both uplink types
   for (auto& uit : mUplinkList)
   {
      size_t weaponIndex               = uit.mUplinkDestinationPtr->GetIndex();
      uniqueWeaponUplinks[weaponIndex] = uniqueWeaponUplinks[weaponIndex] + 1;
   }
   return static_cast<unsigned int>(uniqueWeaponUplinks.size());
}

//! Return the weapon platform at aIndex in the list of the weapons currently being uplinked to
WsfPlatform* WsfUplinkProcessor::UplinkWeaponPlatform(unsigned int aIndex)
{
   // count unique weapons being supported
   std::map<size_t, size_t> uniqueWeaponUplinks; // to keep track
   for (auto& uit : mTrackUplinkList)
   {
      size_t weaponIndex               = uit.mPlatformPtr->GetIndex();
      uniqueWeaponUplinks[weaponIndex] = uniqueWeaponUplinks[weaponIndex] + 1;
      if (uniqueWeaponUplinks.size() > aIndex)
      {
         // just added the weapon at the right "index", return it
         return uit.mPlatformPtr;
      }
   }
   for (auto& uit : mUplinkList)
   {
      size_t weaponIndex               = uit.mUplinkDestinationPtr->GetIndex();
      uniqueWeaponUplinks[weaponIndex] = uniqueWeaponUplinks[weaponIndex] + 1;
      if (uniqueWeaponUplinks.size() > aIndex)
      {
         // just added the weapon at the right "index", return it
         return uit.mUplinkDestinationPtr;
      }
   }
   return nullptr;
}

// virtual
bool WsfUplinkProcessor::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   // WsfStringId messageType = aMessage.GetType();
   if (aMessage.GetType() != WsfTrackMessage::GetTypeId())
   {
      return false;
   }

   // check for uplinks
   const auto&     trackMessage = static_cast<const WsfTrackMessage&>(aMessage);
   const WsfTrack* trackPtr     = trackMessage.GetTrack();
   for (TrackUplinkList::const_iterator tuli = mTrackUplinkList.begin(); tuli != mTrackUplinkList.end();)
   {
      const TrackUplink& uplink = *tuli;
      ++tuli;
      // LBM - one method utilizes mSensorName, another uses mSensorTrackId, support both like this?
      // The WsfTaskManager uplink method based on a TRACK task and a FIRE task uses the mSensorTrackId
      // The WsfTaskManager auto uplink method and the RIPR processor method use the mSensorName
      // if (uplink.mSensorTrackId == trackPtr->GetTrackId())
      if (uplink.mTargetIndex == trackPtr->GetTargetIndex() &&
          (uplink.mSensorName == trackPtr->GetSensorNameId() || uplink.mSensorTrackId == trackPtr->GetTrackId()) &&
          trackMessage.GetOriginatorIndex() == GetPlatform()->GetIndex())
      {
         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "Uplink track to platform via comm.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Processor: " << GetName();
            out.AddNote() << "Track Id: " << trackPtr->GetTrackId();
            out.AddNote() << "Platform: " << uplink.mPlatformPtr->GetName();
            out.AddNote() << "Comm: " << uplink.mCommPtr->GetName();
         }
         for (WsfComponentList::RoleIterator<wsf::comm::Comm> it(*uplink.mPlatformPtr); !it.AtEnd(); ++it)
         {
            auto commRcvrPtr = *it;
            if (commRcvrPtr->IsTurnedOn() && commRcvrPtr->CanReceive())
            {
               auto networkManagerPtr = GetSimulation()->GetCommNetworkManager();
               if (networkManagerPtr->PathExists(uplink.mCommPtr->GetAddress(), commRcvrPtr->GetAddress()))
               {
                  uplink.mCommPtr->Send(aSimTime, ut::clone<WsfMessage>(trackMessage), commRcvrPtr->GetAddress());
                  break;
               }
            }
         }
      }
   }
   return true;
}

wsf::comm::Comm* WsfUplinkProcessor::GetUplinkCommForTrack(WsfStringId aSensorName)
{
   wsf::comm::Comm* commPtr = nullptr;
   for (const auto& uplinkPath : mUplinkPathList)
   {
      if (uplinkPath.mSensorName == aSensorName)
      {
         commPtr = GetPlatform()->GetComponent<wsf::comm::Comm>(uplinkPath.mCommName);
         if (commPtr != nullptr)
         {
            break;
         }
      }
   }

   return commPtr;
}

wsf::comm::Comm* WsfUplinkProcessor::GetUplinkCommToRecipient(WsfPlatform* aSourcePtr, WsfPlatform* aTargetPtr)
{
   for (WsfComponentList::RoleIterator<wsf::comm::Comm> sourceIter(*aSourcePtr); !sourceIter.AtEnd(); ++sourceIter)
   {
      for (WsfComponentList::RoleIterator<wsf::comm::Comm> targetIter(*aTargetPtr); !targetIter.AtEnd(); ++targetIter)
      {
         if (sourceIter->GetNetwork() == targetIter->GetNetwork())
         {
            return *sourceIter;
         }
      }
   }
   return nullptr; // No path to target
}


bool WsfUplinkProcessor::VerifyUplinks()
{
   bool ok = true;
   // Ensure the weapon uplink specifications are correct.
   for (const UplinkPath& uplinkPath : mUplinkPathList)
   {
      if ((GetPlatform()->GetComponent<WsfSensor>(uplinkPath.mSensorName) == nullptr) ||
          (GetPlatform()->GetComponent<wsf::comm::Comm>(uplinkPath.mCommName) == nullptr))
      {
         auto out = ut::log::error() << "Unknown sensor or comm specified for uplink_path.";
         out.AddNote() << "Sensor: " << uplinkPath.mSensorName;
         out.AddNote() << "Comm: " << uplinkPath.mCommName;
         ok = false;
      }
   }

   return ok;
}

void WsfUplinkProcessor::AddTrackUplink(WsfTrackId       aSensorTrackId,
                                        WsfPlatform*     aPlatformPtr,
                                        wsf::comm::Comm* aCommPtr,
                                        size_t           aTargetIndex)
{
   TrackUplink trackUplink;
   trackUplink.mSensorTrackId = aSensorTrackId;
   trackUplink.mPlatformPtr   = aPlatformPtr;
   trackUplink.mCommPtr       = aCommPtr;
   trackUplink.mTargetIndex   = aTargetIndex;
   mTrackUplinkList.push_back(trackUplink);


   double time = GetSimulation()->GetSimTime();
   // WsfTrack* targetTrackPtr = GetPlatform()->GetTrackManager().FindTrack(aSensorTrackId);
   WsfTrack* targetTrackPtr = GetPlatform()->GetTrackManager().FindRawTrack(aSensorTrackId);
   WsfObserver::UplinkInitiated(GetSimulation())(time, GetPlatform(), aPlatformPtr, targetTrackPtr);
}

void WsfUplinkProcessor::AddUplink(WsfTrackId       aTaskTrackId,
                                   WsfTrackId       aLocalTrackId,
                                   WsfTrackId       aSensorTrackId,
                                   WsfPlatform*     aUplinkDestinationPtr,
                                   wsf::comm::Comm* aCommPtr,
                                   WsfStringId      aTrackSource,
                                   unsigned int     aTaskId,
                                   double           aLastSendTime)
{
   Uplink uplinkData;
   uplinkData.mTaskTrackId          = aTaskTrackId;
   uplinkData.mLocalTrackId         = aLocalTrackId;
   uplinkData.mSensorTrackId        = aSensorTrackId;
   uplinkData.mUplinkDestinationPtr = aUplinkDestinationPtr;
   uplinkData.mCommPtr              = aCommPtr;
   uplinkData.mTrackSource          = aTrackSource;
   uplinkData.mTaskId               = aTaskId;
   uplinkData.mLastSendTime         = aLastSendTime;
   mUplinkList.push_back(uplinkData);

   double    time           = GetSimulation()->GetSimTime();
   WsfTrack* targetTrackPtr = GetPlatform()->GetTrackManager().FindTrack(aLocalTrackId);
   // WsfTrack* targetTrackPtr = GetPlatform()->GetTrackManager().FindRawTrack(aSensorTrackId);
   WsfObserver::UplinkInitiated(GetSimulation())(time, GetPlatform(), aUplinkDestinationPtr, targetTrackPtr);
}

void WsfUplinkProcessor::RemoveUplink(unsigned int aTaskId)
{
   for (size_t i = 0; i < mUplinkList.size();)
   {
      Uplink& uplink = mUplinkList[i];
      if (uplink.mTaskId == aTaskId)
      {
         mUplinkList.erase(mUplinkList.begin() + i);

         double    time           = GetSimulation()->GetSimTime();
         WsfTrack* targetTrackPtr = GetPlatform()->GetTrackManager().FindTrack(uplink.mLocalTrackId);
         WsfObserver::UplinkDropped(GetSimulation())(time, GetPlatform(), uplink.mUplinkDestinationPtr, targetTrackPtr);
      }
      else
      {
         ++i;
      }
   }
}

// =================================================================================================
//! Uplink the supplied track to those who have subscribed to uplinks of the track.
void WsfUplinkProcessor::UplinkTrack(double aSimTime, const WsfTrack* aTrackPtr, double aUplinkSendInterval, bool aIsLocalTrack)
{
   // Uplink the track if necessary
   for (Uplink& uplink : mUplinkList)
   {
      if ((aSimTime - uplink.mLastSendTime) >= aUplinkSendInterval)
      {
         bool doUplink = false;
         if (aIsLocalTrack)
         {
            doUplink = (uplink.mTrackSource == nullptr) && (uplink.mLocalTrackId == aTrackPtr->GetTrackId());
         }
         else
         {
            doUplink = (uplink.mTrackSource != 0) && (uplink.mSensorTrackId == aTrackPtr->GetTrackId());
         }
         if (doUplink && uplink.mCommPtr != nullptr)
         {
            uplink.mLastSendTime = aSimTime;
            for (WsfComponentList::RoleIterator<wsf::comm::Comm> it(*uplink.mUplinkDestinationPtr); !it.AtEnd(); ++it)
            {
               auto commRcvrPtr = *it;
               if (commRcvrPtr->IsTurnedOn() && commRcvrPtr->CanReceive())
               {
                  auto networkMangerPtr = GetSimulation()->GetCommNetworkManager();
                  if (networkMangerPtr->PathExists(uplink.mCommPtr->GetAddress(), commRcvrPtr->GetAddress()))
                  {
                     auto msg = ut::make_unique<WsfTrackMessage>(GetPlatform(), *aTrackPtr);
                     uplink.mCommPtr->Send(aSimTime, std::move(msg), commRcvrPtr->GetAddress());
                     break;
                  }
               }
            }
         }
      }
   }
}

// virtual
void WsfUplinkProcessor::LocalTrackInitiated(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr)
{
   UplinkTrack(aSimTime, aLocalTrackPtr, mUplinkSendInterval, true);
}

// virtual
void WsfUplinkProcessor::LocalTrackUpdated(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr)
{
   UplinkTrack(aSimTime, aLocalTrackPtr, mUplinkSendInterval, true);
}

// virtual
void WsfUplinkProcessor::RawTrackReceived(double aSimTime, const WsfTrack* aRawTrackPtr)
{
   UplinkTrack(aSimTime, aRawTrackPtr, mUplinkSendInterval, false);
}

// =================================================================================================
//! A track manager callback to handle the correlation of a raw track with a local track.
//! @param aSimTime       The current simulation time.
//! @param aLocalTrackPtr The local track.
//! @param aRawTrackId    The track ID of the raw track that became correlated with the local track.
// virtual
void WsfUplinkProcessor::LocalTrackCorrelation(double               aSimTime,
                                               const WsfLocalTrack* aLocalTrackPtr,
                                               const WsfTrackId&    aRawTrackId)
{
   // A raw track has been correlated, check to see if it matches an uplink request.
   for (Uplink& uplink : mUplinkList)
   {
      if ((uplink.mTrackSource != 0) && (uplink.mSensorTrackId.IsNull()) &&
          (uplink.mLocalTrackId == aLocalTrackPtr->GetTrackId()))
      {
         WsfTrack* rawTrackPtr = GetPlatform()->GetTrackManager().FindRawTrack(aRawTrackId);
         if ((rawTrackPtr != nullptr) && (rawTrackPtr->GetSensorNameId() == uplink.mTrackSource))
         {
            uplink.mSensorTrackId = rawTrackPtr->GetTrackId();
         }
      }
   }
}

// virtual
void WsfUplinkProcessor::WeaponTerminated(double aSimTime, const WsfWeaponEngagement* aEngagementPtr)
{
   StopUplinking(aEngagementPtr->GetWeaponPlatformIndex());
}

// =================================================================================================
//! Get the pointer to the first attached uplink processor.
//! If a uplink processor has not been added a new default one will be created and
//! added to the platform.
//! @return a WsfUplinkProcessor pointer.
WsfUplinkProcessor* WsfUplinkProcessor::GetUplinkProcessor(WsfPlatform& aPlatform)
{
   // Attempt to find the first instance of a WsfUplinkProcessor.
   WsfUplinkProcessor* processorPtr(nullptr);
   for (WsfComponentList::RoleIterator<WsfProcessor> iter(aPlatform); !iter.AtEnd(); ++iter)
   {
      processorPtr = dynamic_cast<WsfUplinkProcessor*>(*iter);
      if (processorPtr != nullptr)
      {
         break;
      }
   }

   if (processorPtr == nullptr)
   {
      // If no uplink processor found, create a default one
      // it will not be linked up to anything (empty uplink)
      // NOTE: this must be done before the code below:  InitializeAll<WsfProcessor>(aSimTime, mProcessors,...
      processorPtr = new WsfUplinkProcessor(aPlatform.GetScenario());
      processorPtr->SetName(aPlatform.GetName() + "_default_uplink_processor");
      aPlatform.AddComponent(processorPtr);
      processorPtr->LinkToSensorsOnInit(true);
      if (aPlatform.IsInitialized())
      {
         processorPtr->Initialize(aPlatform.GetSimTime());
      }
   }
   return processorPtr;
}

// =================================================================================================
//! Get the pointer to the first attached uplink processor.
//! If a uplink processor has not been added a new default one will be created and
//! added to the platform.
//! @return a WsfUplinkProcessor pointer.
bool WsfUplinkProcessor::HasUplinkProcessor(WsfPlatform& aPlatform)
{
   // Attempt to find the first instance of a WsfUplinkProcessor.
   WsfUplinkProcessor* processorPtr(nullptr);
   for (WsfComponentList::RoleIterator<WsfProcessor> iter(aPlatform); !iter.AtEnd(); ++iter)
   {
      processorPtr = dynamic_cast<WsfUplinkProcessor*>(*iter);
      if (processorPtr != nullptr)
      {
         break;
      }
   }
   return (processorPtr != nullptr);
}

//
// std::vector<WsfAssetPerception*>& WsfUplinkProcessor::GetPlatformPerceivedAssets(WsfPlatform& aPlatform, double aSimTime)
//{
//   return GetPerceptionProcessor(aPlatform)->PerceivedAssets(aSimTime);
//}
//
// std::vector<WsfLocalTrack*>& WsfUplinkProcessor::GetPlatformPerceivedThreats(WsfPlatform& aPlatform, double aSimTime)
//{
//   return GetPerceptionProcessor(aPlatform)->PerceivedThreats(aSimTime);
//}

// -------------------------------------------------------------------------------------------------
//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
UtScriptClass* WsfUplinkProcessor::CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
{
   return new WsfScriptUplinkProcessor(aClassName, aScriptTypesPtr);
}


#include "script/WsfScriptDefs.hpp"

WsfScriptUplinkProcessor::WsfScriptUplinkProcessor(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptProcessorClass(aClassName, aScriptTypesPtr)
{
   mConstructible = false;
   SetClassName("WsfUplinkProcessor");

   AddMethod(ut::make_unique<StartUplinking_1>("StartUplinking"));
   AddMethod(ut::make_unique<StartUplinking_2>("StartUplinking"));
   AddMethod(ut::make_unique<StopUplinking>());       // bool StopUplinking(WsfPlatform)
   AddMethod(ut::make_unique<IsUplinkingTo>());       // bool IsUplinkingTo(WsfPlatform)
   AddMethod(ut::make_unique<UplinkCount>());         // int UplinkCount()
   AddMethod(ut::make_unique<UplinkPlatformEntry>()); // WsfPlatform UplinkPlatformEntry(int)
   AddMethod(ut::make_unique<UplinkCapable>());       // bool UplinkCapable()
   AddMethod(ut::make_unique<MaxSupportedUplinks>()); // int MaxSupportedUplinks()
}


// virtual
void* WsfScriptUplinkProcessor::Create(const UtScriptContext& aContext)
{
   return new WsfUplinkProcessor(*WsfScriptContext::GetSCENARIO(aContext));
}

// virtual
void* WsfScriptUplinkProcessor::Clone(void* aObjectPtr)
{
   WsfUplinkProcessor* objPtr = static_cast<WsfUplinkProcessor*>(aObjectPtr);
   assert(objPtr != nullptr);
   return objPtr->Clone();
}

// virtual
void WsfScriptUplinkProcessor::Destroy(void* aObjectPtr)
{
   WsfUplinkProcessor* objPtr = static_cast<WsfUplinkProcessor*>(aObjectPtr);
   delete objPtr;
}

UtScriptContext* WsfScriptUplinkProcessor::GetContext(void* aObjectPtr)
{
   UtScriptContext*    contextPtr   = nullptr;
   WsfUplinkProcessor* processorPtr = (WsfUplinkProcessor*)aObjectPtr;
   if (processorPtr != nullptr)
   {
      contextPtr = &(processorPtr->GetScriptContext().GetContext());
   }
   return contextPtr;
}

//! StartUplinking a track to another platform
//! This is a more obvious way of using AssignTask()
//! bool ok = UplinkTrack(WsfTrack aTrackId, string aTaskType, WsfPlatform aDestination);
UT_DEFINE_SCRIPT_METHOD(WsfScriptUplinkProcessor, WsfUplinkProcessor, StartUplinking_1, 1, "bool", "WsfPlatform")
{
   WsfPlatform* weaponPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetBool(aObjectPtr->StartUplinking(weaponPtr, nullptr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptUplinkProcessor, WsfUplinkProcessor, StartUplinking_2, 2, "bool", "WsfPlatform, WsfTrack")
{
   WsfPlatform* weaponPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfTrack*    targetPtr = (WsfTrack*)aVarArgs[1].GetPointer()->GetAppObject();
   aReturnVal.SetBool(aObjectPtr->StartUplinking(weaponPtr, targetPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptUplinkProcessor, WsfUplinkProcessor, StopUplinking, 1, "bool", "WsfPlatform")
{
   WsfPlatform* weaponPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetBool(aObjectPtr->StopUplinking(weaponPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptUplinkProcessor, WsfUplinkProcessor, IsUplinkingTo, 1, "bool", "WsfPlatform")
{
   WsfPlatform* weaponPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
   aReturnVal.SetBool(aObjectPtr->IsUplinkingTo(weaponPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptUplinkProcessor, WsfUplinkProcessor, UplinkCapable, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->UplinkCapable());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptUplinkProcessor, WsfUplinkProcessor, UplinkCount, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->WeaponUplinkCount());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptUplinkProcessor, WsfUplinkProcessor, UplinkPlatformEntry, 1, "WsfPlatform", "int")
{
   unsigned int index = (unsigned int)aVarArgs[0].GetInt();
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->UplinkWeaponPlatform(index), aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptUplinkProcessor, WsfUplinkProcessor, MaxSupportedUplinks, 0, "int", "")
{
   int supportedUplinks = aObjectPtr->GetMaxSupportedUplinks();
   aReturnVal.SetInt(supportedUplinks);
}
