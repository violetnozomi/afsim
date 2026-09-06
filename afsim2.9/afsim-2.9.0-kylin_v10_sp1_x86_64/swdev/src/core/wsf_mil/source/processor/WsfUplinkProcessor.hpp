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

#ifndef WSFUPLINKPROCESSOR_HPP
#define WSFUPLINKPROCESSOR_HPP

#include <list>

#include "UtCallbackHolder.hpp"
class WsfLocalTrack;
#include "wsf_mil_export.h"
class WsfPlatform;
#include "script/WsfScriptProcessor.hpp"
#include "script/WsfScriptProcessorClass.hpp"
class WsfTrack;
#include "WsfTrackId.hpp"
class WsfWeaponEngagement;

class WSF_MIL_EXPORT WsfUplinkProcessor : public WsfScriptProcessor
{
public:
   static WsfUplinkProcessor* GetUplinkProcessor(WsfPlatform& aPlatform);
   static bool                HasUplinkProcessor(WsfPlatform& aPlatform);

   WsfUplinkProcessor(const WsfScenario& aScenario);
   ~WsfUplinkProcessor() override = default;
   WsfUplinkProcessor* Clone() const override { return new WsfUplinkProcessor(*this); }

   bool ProcessInput(UtInput& aInput) override;

   bool Initialize(double aSimTime) override; // setup callbacks

   bool UplinkCapable();
   void AddUplinkPath(const std::string& aSensorName, const std::string& aCommName);

   void LinkToSensorsOnInit(bool val) { mLinkToSensorsOnInit = val; }

   void SetSendInterval(double aInterval) { mUplinkSendInterval = aInterval; }

   bool StartUplinking(size_t aReceiverIndex, size_t aTargetIndex = 0);

   bool StartUplinking(WsfPlatform* aReceiverPtr, WsfTrack* aTargetPtr);

   bool StopUplinking(size_t aReceiverIndex);

   bool StopUplinking(WsfPlatform* aReceiverPtr);

   bool IsUplinkingTo(WsfPlatform* aReceiverPtr);

   unsigned int WeaponUplinkCount();

   WsfPlatform* UplinkWeaponPlatform(unsigned int aIndex);

   bool ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;

   wsf::comm::Comm* GetUplinkCommForTrack(WsfStringId aSensorName);

   // similar to WsfComm* SelectDevice() except it does not check if either comm is turned on
   wsf::comm::Comm* GetUplinkCommToRecipient(WsfPlatform* aSourcePtr, WsfPlatform* aTargetPtr);

   bool VerifyUplinks();

   bool IsUplinkPathListEmpty() { return mUplinkPathList.empty(); }

   void AddTrackUplink(WsfTrackId aSensorTrackId, WsfPlatform* aPlatformPtr, wsf::comm::Comm* aCommPtr, size_t aTargetIndex);

   void ClearTrackUplinkList() { mTrackUplinkList.clear(); }

   void AddUplink(WsfTrackId       aTaskTrackId,
                  WsfTrackId       aLocalTrackId,
                  WsfTrackId       aSensorTrackId,
                  WsfPlatform*     aUplinkDestinationPtr,
                  wsf::comm::Comm* aCommPtr,
                  WsfStringId      aTrackSource,
                  unsigned int     aTaskId,
                  double           aLastSendTime);

   void RemoveUplink(unsigned int aTaskId);

   void UplinkTrack(double aSimTime, const WsfTrack* aTrackPtr, double aUplinkSendInterval, bool aIsLocalTrack);

   virtual void LocalTrackInitiated(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr);

   virtual void LocalTrackUpdated(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr);

   virtual void RawTrackReceived(double aSimTime, const WsfTrack* aRawTrackPtr);

   virtual void LocalTrackCorrelation(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrackId& aRawTrackId);

   virtual void WeaponTerminated(double aSimTime, const WsfWeaponEngagement* aEngagementPtr);

   void         SetMaxUplinkCount(unsigned int aNumUplinks) { mMaxSupportedUplinks = aNumUplinks; }
   unsigned int GetMaxSupportedUplinks() { return mMaxSupportedUplinks; }

   // Scripting support
   UtScriptClass* CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   const char* GetScriptClassName() const override { return "WsfUplinkProcessor"; }

protected:
   WsfUplinkProcessor(const WsfUplinkProcessor& aSrc);
   WsfUplinkProcessor& operator=(const WsfUplinkProcessor&) = delete;

private:
   unsigned int mMaxSupportedUplinks;
   //! The comm device that is to be used for uplinking track data
   struct UplinkPath
   {
      //! The name of the sensor whose track are to be uplinked
      WsfStringId mSensorName;

      //! The name of the comm device over which the tracks are to be uplinked
      WsfStringId mCommName;
   };
   using UplinkPathList     = std::list<UplinkPath>;
   using UplinkPathListIter = std::list<UplinkPath>::iterator;
   UplinkPathList mUplinkPathList;

   //! The track uplink table is used to route tracking sensor updates to the weapon platform.
   //! This table is reconstructed when anything that could require an entry to be created or destroyed
   //! occurs (track creation, weapon firing, platform deletion, etc.)

   struct TrackUplink
   {
      //! The track ID from the from the sensor.
      WsfTrackId mSensorTrackId;

      //! The name of the sensor whose tracks are to be uplinked.
      WsfStringId mSensorName;

      //! The index of the target whose tracks are to be uplinked.
      size_t mTargetIndex;

      //! The recipient weapon platform.
      WsfPlatform* mPlatformPtr;

      //! The comm device over which the message is to be sent.
      wsf::comm::Comm* mCommPtr;
   };
   using TrackUplinkList = std::list<TrackUplink>;
   TrackUplinkList mTrackUplinkList;

   //! The uplink list contains information about tracks currently being uplinked.
   struct Uplink
   {
      //! TrackId of the related task
      WsfTrackId mTaskTrackId;

      //! TrackId of the local track correlated to mTaskTrackId
      WsfTrackId mLocalTrackId;

      //! If non-null, ID of the raw track being uplinked
      WsfTrackId mSensorTrackId;

      //! Platform receiving uplink
      WsfPlatform* mUplinkDestinationPtr;

      //! Comm used to send uplink
      wsf::comm::Comm* mCommPtr;

      //! Name of track source, or null for track-mananger
      WsfStringId mTrackSource;

      //! Task ID of task requesting this uplink
      unsigned int mTaskId;

      //! Last sim-time at which track was sent
      double mLastSendTime;
   };

   using UplinkList = std::vector<Uplink>;
   UplinkList mUplinkList;

   //! A list of active weapons fired in response to weapon fire task.
   //!
   //! An entry is created in response to the WsfObserver::WeaponFired callback.
   //! An entry is removed in response to the WsfObserver::WeaponTerminated callback.
   //!
   //! Note that entries are NOT removed when the associated task is purged.
   struct ActiveWeapon
   {
      //! The track ID of the track that was provided to WsfWeapon::Fire.
      WsfTrackId mTaskTrackId;
      //! The name of the weapon system that was fired. (NOT the name of the launched weapon).
      WsfStringId mWeaponSystemName;
      //! Platform index of the launched weapon platform.
      size_t mPlatformIndex;
      //! The serial number of the weapon engagement
      unsigned int mSerialNumber;
      //! The platform index of the target, not in WsfTaskManager
      size_t mTargetIndex;
   };

   using ActiveWeaponList     = std::list<ActiveWeapon>;
   using ActiveWeaponListIter = std::list<ActiveWeapon>::iterator;

   ActiveWeaponList mActiveWeaponList;

   bool             mLinkToSensorsOnInit;
   double           mUplinkSendInterval;
   UtCallbackHolder mCallbacks;
};


class WSF_MIL_EXPORT WsfScriptUplinkProcessor : public WsfScriptProcessorClass
{
public:
   WsfScriptUplinkProcessor(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   void*            Create(const UtScriptContext& aContext) override;
   void*            Clone(void* aObjectPtr) override;
   void             Destroy(void* aObjectPtr) override;
   UtScriptContext* GetContext(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(StartUplinking_1);
   UT_DECLARE_SCRIPT_METHOD(StartUplinking_2);
   UT_DECLARE_SCRIPT_METHOD(StopUplinking);
   UT_DECLARE_SCRIPT_METHOD(IsUplinkingTo);
   UT_DECLARE_SCRIPT_METHOD(UplinkCapable);
   UT_DECLARE_SCRIPT_METHOD(UplinkCount);
   UT_DECLARE_SCRIPT_METHOD(UplinkPlatformEntry);
   UT_DECLARE_SCRIPT_METHOD(MaxSupportedUplinks);
};

#endif
