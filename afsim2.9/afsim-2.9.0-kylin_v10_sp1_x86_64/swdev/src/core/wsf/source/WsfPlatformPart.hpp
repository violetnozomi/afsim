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

#ifndef WSFPLATFORMPART_HPP
#define WSFPLATFORMPART_HPP

#include "wsf_export.h"

#include <mutex>
#include <queue>

#include "WsfAuxDataEnabled.hpp"
#include "WsfCategoryList.hpp"
#include "WsfComponent.hpp"
class WsfGroup;
#include "WsfGroupList.hpp"
#include "WsfInternalLinks.hpp"
#include "WsfObject.hpp"
class WsfPlatform;
class WsfScenario;
class WsfSimulation;
#include "WsfTypes.hpp"
#include "WsfUniqueId.hpp"
#include "WsfVariable.hpp"

//! WsfPlatformPart is the base object for all objects aggregated by platforms.
//!
//! These objects include the following:
//! - WsfComm
//! - WsfFuel
//! - WsfMover
//! - WsfProcessor
//! - WsfSensor
//! - WsfVisualPart
//!
//! This base class provides the abstract interface that subclasses must support:
//! - WsfPlatformPart::Initialize
//! - WsfPlatformPart::ProcessInput
//! - WsfPlatformPart::Update
//!
//! It also provides:
//! - The ability for the parts to be send messages to other parts on the platform.
//! - Support for inclusion within categories (see WsfCategoryList)
//!
//! It enforces that:
//! - Setting damage to 1.0 at least temporarily disables the part:
//! -    a. If Turned On, it is Forced Off.
//! -    b. If Operational, it is set Non-Operational.
//! -    c. If identified as not restorable, damage state latches at 1.0 (broken).
//! -    d. If identified as critical, the containing platform is destroyed.
//! -    e. Observer call-backs are embedded for state transitions.
class WSF_EXPORT WsfPlatformPart : public WsfObject, public WsfPlatformComponent, public WsfUniqueId, public WsfAuxDataEnabled
{
public:
   enum
   {
      cLEVEL_ONE_DEBUG = 0x00000001, //!< General debug.
      cLEVEL_TWO_DEBUG = 0x00000002  //!< Full debug.
   };

   using InternalMessageQueue  = std::queue<WsfMessage*>;
   using RecipientMessageQueue = std::queue<std::pair<WsfPlatformPart*, WsfMessage*>>;

   WsfPlatformPart(const WsfScenario& aScenario, int aPartType);
   WsfPlatformPart& operator=(const WsfPlatformPart& aRhs) = delete;
   ~WsfPlatformPart() override;

   const int* GetComponentRoles() const override;
   void*      QueryInterface(int aRole) override;

   //! @name Component infrastructure methods.
   //! Note that all of the required methods are not defined by this class. They are defined by the
   //! leaf that defines the platform part (e.g. WsfComm, WsfProcessor, etc.).
   //@{
   WsfStringId GetComponentName() const override { return GetNameId(); }
   void        ComponentParentChanged(WsfPlatform* aPlatformPtr) override
   {
      // Set the pointer AND call the SetPlatform. Some classes define SetPlatform so they get control
      // when ever the attachment is made. In particular, some of the script stuff needs to be set
      // as soon as possible so script compiles work. SetPlatform *should* set the pointer, but we do
      // it here just in case a derived method fails to call the base class version as they should.
      SetPlatform(aPlatformPtr);
      mPlatformPtr = aPlatformPtr;
   }
   //@}

   //! @name Common infrastructure methods.
   //@{
   bool         Initialize(double aSimTime) override;
   bool         Initialize2(double aSimTime) override;
   bool         ProcessInput(UtInput& aInput) override;
   virtual void Update(double aSimTime);
   //@}

   //! @name Platform association methods.
   //@{
   //! Get the platform to which this part is attached.
   //! @return Pointer to the containing platform or 0 if the part is not associated with a platform.
   WsfPlatform* GetPlatform() const { return mPlatformPtr; }

   //! Set the platform to which this part is associated.
   //! @param aPlatformPtr Pointer to the containing platform.
   //! @note A part can be associated with only one platform.  It is an error to
   //! associate a part with more than one platform!
   virtual void SetPlatform(WsfPlatform* aPlatformPtr) { mPlatformPtr = aPlatformPtr; }
   //@}

   //! @name Platform notification methods.
   //! These methods are invoked whenever another platform is added or deleted from the simulation.
   //@{
   virtual void PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr);

   virtual void PlatformDeleted(WsfPlatform* aPlatformPtr);
   //@}

   //! @name Status (on/off) methods.
   //@{
   //! Returns the time at which this system was last turned on or off.
   //! @return The simulation time at which the device was turned on or off.
   double GetLastStatusChangeTime() const { return mLastStatusChangeTime; }

   //! Allows a system to set the time at which the device was initialized
   void SetLastStatusChangeTime(double aSimTime) { mLastStatusChangeTime = aSimTime; }

   //! Returns an indication that this system can be made operational.
   bool CanBeMadeOperational() const
   {
      return (!mIsOperational) && (mDamageFactor < 1.0) && (!mIsExternallyControlled);
   }

   //! Returns an indication that this system can be made non-operational.
   bool CanBeMadeNonOperational() const { return mIsOperational && (!mIsExternallyControlled); }

   //! Returns an indication that this system can be turned on.
   bool CanBeTurnedOn() const { return mIsOperational && (!mIsTurnedOn) && (!mIsExternallyControlled); }

   //! Returns an indication that this system can be turned off.
   bool CanBeTurnedOff() const { return mIsTurnedOn && (!mIsExternallyControlled); }

   //! Is the system currently turned 'on'?
   //! @return true if the device is turned on.
   bool IsTurnedOn() const { return mIsTurnedOn; }

   //! Is the system to be turned 'on' when added to the simulation?
   bool InitiallyTurnedOn() const { return mInitiallyTurnedOn; }

   //! Sets if the system is turned on when added to the simulation.
   void SetInitiallyTurnedOn(bool aInitiallyTurnedOn) { mInitiallyTurnedOn = aInitiallyTurnedOn; }

   virtual void TurnOff(double aSimTime);

   virtual void TurnOn(double aSimTime);

   virtual void SetOperational(double aSimTime);

   virtual void SetNonOperational(double aSimTime);

   virtual void PartBroken(double aSimTime);

   virtual void Restore(double aSimTime);
   //@}

   //! @name Operational status methods.
   //! A part may be defined to be non-operational, in which case it cannot be used (turned on)
   //! until it is made operational. This can be used to deactivate a certain part without having
   //! to remove it from the platform.
   //@{
   //! Is the part operational?
   //! @returns true if the device is operational.
   bool IsOperational() const { return mIsOperational; }

   bool SetOperational(double aSimTime, bool aMakeOperational);

   //! Is the part damaged?
   //! Zero indicates undamaged. One indicates totally damaged. A transition
   //! from positive to zero will trigger the part type becoming non-operational as an observed event.
   //! @returns [0 to 1]
   double GetDamageFactor() const { return mDamageFactor; }

   bool SetDamageFactor(double aSimTime, double aDamageFactor);

   //! Is the part critical?
   //! @returns true if the part is critical. If true and damage = 1.0, the platform will be destroyed.
   bool IsCritical() const { return mIsCritical; }

   void SetCritical(bool aValue) { mIsCritical = aValue; }

   //! Owning platform has issued a broken event.
   virtual void OnPlatformBrokenEvent(double aSimTime);

   //! Is the part permanently broken?
   //! @returns true if the part is broken.
   bool IsBroken() const { return (mDamageFactor == 1.0) && (!mIsRestorable); }

   //@}

   //! @name Inter-part communication methods.
   //@{
   //! Return the internal links for this part.
   WsfInternalLinks& GetInternalLinks() { return mInternalLinks; }

   //! Does this platform part have a communication link to other platform parts?
   virtual bool HasInternalLinks() const { return mInternalLinks.HasLinks(); }

   virtual bool ProcessMessage(double aSimTime, const WsfMessage& aMessage);

   virtual bool ReceiveMessage(double aSimTime, const WsfMessage& aMessage);

#undef SendMessage // Avoid conflict with Windows macro
   virtual void SendMessage(double aSimTime, const WsfMessage& aMessage);
   //@}

   //! @name Recipient communication methods.
   //@{
   virtual void SendMessageTo(double aSimTime, const WsfMessage& aMessage, WsfStringId aRecipientId);

   void SendMessageToProcessor(double aSimTime, const WsfMessage& aMessage, WsfStringId aRecipientId);

   virtual void SendQueuedMessages(double aSimTime);
   //@}

   //! @name Category methods.
   //! These methods allow the parts to 'categorized'.
   //@{
   //! Get the categories to which this part belongs.
   const WsfCategoryList& GetCategories() const { return mCategories; }

   //! Adds the provided category to the platform part's category list.
   void AddCategory(WsfStringId aCategoryId) { mCategories.JoinCategory(aCategoryId); }

   //! Is this part a member of the indicated category?
   //! @param aCategoryId String Id of the category to be checked.
   //! @return 'true' if a member of the category.
   bool IsCategoryMember(WsfStringId aCategoryId) const { return mCategories.IsCategoryMember(aCategoryId); }
   //@}

   //! @name Group management.
   //@{
   //! Get the groups to which this platform part belongs.
   const WsfGroupList& GetGroups() const { return mGroups; }
   void                JoinGroup(WsfGroup* aGroupPtr);
   void                LeaveGroup(WsfGroup* aGroupPtr);
   bool                IsGroupMember(WsfStringId aGroupId);
   //@}

   //! @name Event-Driven simulation methods.
   //@{
   //! Get interval at which Update() is called.
   //! This value is set by calling WsfSimulation::GetInstance().SetProcessorUpdateInterval();
   virtual double GetUpdateInterval() const = 0;
   //@}

   //! @name Methods for simulation control of the update interval.
   //! @note These methods are only intended for use by simulation executives (classes derived from WsfSimulation)
   //! and WsfPlatformPartEvent and should not be invoked from any other place. Any other use is at your own risk.
   //{@
   //! Set the time interval between the periodic calls to Update().
   //! A value of less than or equal to zero indicates periodic updates are not to be performed.
   //! @param aUpdateInterval The new update interval.
   //! @note Users should call WsfSimulation::GetInstance().SetProcessorUpdateInterval() to set the interval.
   virtual void SetUpdateInterval(double aUpdateInterval) = 0;

   //! Get the current update event 'epoch'.
   //! The event 'epoch' is used to recognize and ignore obsolete events.
   unsigned int GetUpdateEventEpoch() const { return mUpdateEventEpoch; }

   //! Increment the 'epoch' for the update event.
   //! This should be called whenever the update interval is changed or if the processor is turned off or on.
   void IncrementUpdateEventEpoch() { ++mUpdateEventEpoch; }

   //! Is an 'Update' event active?
   bool UpdateEventActive() const { return mUpdateEventActive; }

   //! Indicate if the 'Update' event is active.
   void SetUpdateEventActive(bool aUpdateEventActive) { mUpdateEventActive = aUpdateEventActive; }
   //@}

   //! @name debugging control
   //@{
   //! Returns 'true' if debugging is enabled for this instance.
   bool DebugEnabled() const { return mDebugEnabled; }

   void SetDebugEnabled(bool aDebugEnabled) { mDebugEnabled = aDebugEnabled; }

   unsigned int GetDebugMask() { return mDebugMask; }
   //@}

   bool IsExternallyControlled() const { return mIsExternallyControlled; }

   virtual void SetExternallyControlled(double aSimTime, bool aIsExternallyControlled);

   int GetPartType() const { return mPartType; }

   const char* GetScriptClassName() const override;

   WsfSimulation* GetSimulation() const;

   const WsfScenario& GetScenario() const { return mScenario; }

protected:
   WsfPlatformPart(const WsfPlatformPart& aSrc);

private:
   //! The platform part type (primary component role)
   int mPartType;

   //! A const reference to the scenario
   const WsfScenario& mScenario;

   //! The pointer to the platform that 'owns' the part.
   WsfPlatform* mPlatformPtr;

   //! The internal links to others that want to receive messages from us.
   WsfInternalLinks mInternalLinks;

   //! The categories to which this part belongs.
   WsfCategoryList mCategories;

   //! The groups to which this part belongs.
   WsfGroupList mGroups;

   //! Time (in seconds) when the system was turned on.
   double mLastStatusChangeTime;

   //! Time to elapse between suffering complete part damage, to an automatic "operational" recovery.
   double mAutomaticRecoveryTime;

   //! Damage factor in the range [0 to 1]; zero is fine, one is non-functional.
   double mDamageFactor;

   //! 'true' if destruction of the part (mDamageFactor = 1.0) results in destruction of the platform.
   bool mIsCritical;

   //! 'true' if currently turned on.
   bool mIsTurnedOn;

   //! true if to be turned on when added to the simulation
   bool mInitiallyTurnedOn;

   //! true if the part is operational.
   bool mIsOperational;

   //! true if the part can become operational again
   //! after once attaining damage state 1.0.
   bool mIsRestorable;

   //! true if debug is enabled.
   bool mDebugEnabled;

   //! true if the platform part is externally controlled
   bool mIsExternallyControlled;

   //! True if an event-driven periodic call to Update() is in progress.
   bool mUpdateEventActive;

   //! A bit mask for turning on debug levels
   unsigned int mDebugMask;

   //! The current update event epoch.
   //! This is used to identify and reject obsolete 'update' events.
   unsigned int mUpdateEventEpoch;

   //! Internal message queue for active multi-threading cases
   InternalMessageQueue mInternalMessageQueue;

   //! Recipient message queue for active multi-threading cases
   RecipientMessageQueue mRecipientMessageQueue;

   std::recursive_mutex mMutex;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfPlatformPart, cWSF_COMPONENT_PLATFORM_PART)

#endif
