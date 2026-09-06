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

#ifndef WSFWEAPONTASKMANAGER_HPP
#define WSFWEAPONTASKMANAGER_HPP

#include "wsf_mil_export.h"

#include "WsfMilComponentRoles.hpp"
#include "WsfTaskManager.hpp"
#include "WsfTaskProcessor.hpp"
class WsfTrack;
class WsfTrackId;
class WsfWeapon;
class WsfWeaponEngagement;

//! WsfWeaponTaskManager is a class for weapon task control.
//! This class provides weapon uplink capability for sending tracks to weapons.
//! There are a few methods to perform uplinking:
//!   - weapon_uplink_path
//!   - auto_weapon_uplink
//!   - uplink tasks
//! *note: implementation of weapon_uplink_path is completely separate from the other uplink commands.
//!
//! In general it is up to the derived classes to decide what actually needs to be done to perform a
//! task, however some intrinsic capabilities are provided:
//!   - Turn on an acquisition sensor.
//!   - Turn on and initiate a tracking sensor request.
//!   - Cancel a tracking sensor request and turn off the sensor.
//!   - Initiate a weapon release and provide for command guidance uplink.
class WSF_MIL_EXPORT WsfWeaponTaskManager : public WsfTaskManagerComponent
{
public:
   static void                  RegisterComponentFactory(WsfScenario& aScenario);
   static WsfWeaponTaskManager* Find(const WsfProcessor& aParent);
   static WsfWeaponTaskManager* FindOrCreate(WsfProcessor& aParent);

   WsfWeaponTaskManager();
   WsfWeaponTaskManager(const WsfWeaponTaskManager& aSrc);
   WsfWeaponTaskManager& operator=(const WsfWeaponTaskManager&) = delete;
   ~WsfWeaponTaskManager() override                             = default;

   //! @name Required interface from WsfComponent.
   //@{
   WsfComponent* CloneComponent() const override;
   WsfStringId   GetComponentName() const override;
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   //@}

   //! @name Common interface from WsfComponent/WsfProcessorComponent.
   //@{
   bool Initialize(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;
   //@}

   //! @name WsfTaskManagerComponent interface.
   //@{
   void PlatformDeleted(WsfPlatform* aPlatformPtr) override;
   bool AcquireResource(double aSimTime, WsfTask& aTask, const WsfTrack& aTrack, bool& aStatusOk) override;
   bool ReleaseResource(double aSimTime, WsfTask& aTask) override;
   bool AssignTask(double aSimTime, const WsfTrack& aTrack, const WsfTask& aTask, bool aLockTrack = true) override;

   void PurgeReceivedTask(double aSimTime, WsfTask& aTask) override;
   void PurgeTransmittedTask(double aSimTime, WsfTask& aTask) override;

   void TaskCorrelated(double aSimTime, WsfTask& aTask) override;
   void TaskCanceled(double aSimTime, WsfTask& aTask) override;

   void TrackDropped(double aSimTime, const WsfTrackId& aTrackId) override;

   bool ProcessAssociationMessage(double aSimTime, const WsfAssociationMessage& aMessage) override;
   bool ProcessTaskStatusMessage(double aSimTime, const WsfTaskStatusMessage& aMessage) override;
   //@}

   //! @name weapon log data.
   //! These variables are used to keep record of all previously sent weapon tasks
   //! Entries only exist while a track is maintained on the target
   //@{
   using WeaponTaskLogEntry = std::set<std::pair<size_t, WsfStringId>>;
   using WeaponTaskLogMap   = std::map<WsfTrackId, WeaponTaskLogEntry>;
   //@}

   //! @name Command methods.
   //@{
   virtual bool AbortFiring(double aSimTime, const WsfTrackId& aTrackId);
   //@}


   //! @name Simulation observer callbacks
   //@{
   virtual void WeaponFireAborted(double aSimTime, WsfWeapon* aWeaponPtr, const WsfTrack* aTargetTrackPtr, double aQuantity);

   virtual void WeaponFired(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, const WsfTrack* aTargetTrackPtr);

   void WeaponTerminated(double aSimTime, const WsfWeaponEngagement* aEngagementPtr);
   //@}


   //! @name Task manager observer callbacks.
   //@{
   void StartingWeaponTask(double aSimTime, const WsfTask& aTask);
   //@}

   //! @name Query methods
   //@{
   double TimeWeaponLastFiredFor(const WsfTrackId& aTrackId);
   double TimeWeaponLastTerminatedFor(const WsfTrackId& aTrackId);
   int    WeaponsActiveFor(const WsfTrackId& aTrackId);
   int    WeaponsActiveFor(const WsfTrackId& aTrackId, WsfPlatform* aPlatformPtr);
   int    SalvosFiredAt(const WsfTrackId& aTrackId);
   int    RoundsFiredAt(const WsfTrackId& aTrackId);
   int    RoundsFiredAt(const WsfTrackId& aTrackId, WsfPlatform* aPlatformPtr);
   //@}

   //! Check for weapon ready states
   static UtCallbackListN<void(const WsfWeapon*, bool&)> IsWeaponReadyForLaunch;

   //! @name Internal methods.
   //! These methods are not intended for public use.
   //@{
   void UplinkStart(double aSimTime, unsigned int aTaskId);
   //@}

   bool IsAutoUplink() const { return mAutoUplink; }
   // Allow the auto up-link flag to be turned off during the run.
   // BAR/AJM: Bug in auto up-link logic 06/13.  Need it off when running
   // with net logic.
   // Bug: All targets are being up-linked to the weapon and not just the
   //      current target.
   void SetAutoUplink(bool aBool) { mAutoUplink = aBool; }

   WsfStringId GetAutoUplinkPlatformNameId() const { return mAutoUplinkPlatform; }

   void SetAutoUplinkPlatformNameId(WsfStringId aAutoUplinkPlatform) { mAutoUplinkPlatform = aAutoUplinkPlatform; }

private:
   void BeginAutoUplink(double aSimTime, const WsfTrack& aTrack, const WsfTask& aFireTask);

   bool TaskWaitToTransmit(double aSimTime, const WsfTrack& aTrack, const WsfTask& aTask);

   //! Pointer back to the parent task processor.
   WsfTaskManager* mTaskManagerPtr;

   UtCallbackHolder mCallbacks;

   WeaponTaskLogMap mWeaponTaskLog;

   //! List of tasks waiting for some criteria that depends on another transmitted task.
   //! Tasks in this list must match up to a transmitted task by TrackId and TaskType.
   WsfTaskManager::TaskList mWaitingTaskList;

   //! Name of the default uplink source
   WsfStringId mDefaultUplinkSource;

   //! Name of the default comm used to uplink
   WsfStringId mDefaultUplinkComm;

   //! Default delay before uplink begins
   double mDefaultUplinkDelay;

   //! Minimum interval between sending uplink tracks (for task-based uplinking only)
   double mUplinkSendInterval;

   //! When auto-uplink is initiated, the uplink may be provided by another platform
   WsfStringId mAutoUplinkPlatform;

   //! 'true' if an uplink is initiated after a AssignTask with a weapon resource
   bool mAutoUplink;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfWeaponTaskManager, cWSF_COMPONENT_WEAPON_TASK_MANAGER)

#endif
