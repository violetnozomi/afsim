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

#ifndef WSFWEAPON_HPP
#define WSFWEAPON_HPP

#include <list>
#include <map>
#include <vector>

class UtInput;
#include "WsfArticulatedPart.hpp"
#include "WsfComponentList.hpp"
class WsfEM_Rcvr;
class WsfEM_Xmtr;
#include "wsf_mil_export.h"

#include "WsfEvent.hpp"
#include "WsfMilComponentRoles.hpp"
class WsfLaunchComputer;
#include "WsfMode.hpp"
#include "WsfModeList.hpp"
#include "WsfRandomVariable.hpp"
#include "WsfStringId.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackId.hpp"
class WsfWeapon;
#include "WsfWeaponComponent.hpp"
class WsfWeaponEngagement;

//! A base class for all types of weapon systems.
//!
//! A weapon is defined to be something that is in some way meant to prevent the
//! operation of some other object (either permanently or temporarily).  The base
//! class only provides an abstract definition of a weapon. It is up to derived
//! classes to provide the actual implementation of the weapon.
//!
//! Weapons come in two basic forms:
//!
//! - Explicit weapons.  These result in the creation of a new platform that causes
//! the intended effect.  (Such as a missile or a bomb that is modeled as a separate
//! platform).
//! - Implicit weapons. These do not result in the creation of a new platform. Directed
//! energy weapons could be considered implicit weapons.  Bombs and missiles could
//! also be modeled implicitly by using geometry and probabilities to determine effect.
//!
//! The state of a weapon engagement is contained within the WsfWeaponEngagement class.
//!
//! The effect of a weapon on a target is modeled by derived classes of the WsfWeaponEffects
//! class.
//!
//! @note Derived classes should invoke the methods ChangeQuantity() and
//! SetTimeLastFired() to ensure external decision makers have the correct data.
class WSF_MIL_EXPORT WsfWeapon : public WsfArticulatedPart
{
public:
   static const double cUNDEFINED_DOUBLE;

   enum WeaponState
   {
      cWEAPON_STATE_UNALLOCATED,
      cWEAPON_STATE_ALLOCATED,
      cWEAPON_STATE_DEALLOCATE,
      cWEAPON_STATE_NACK,
      cWEAPON_STATE_READY
   };

   //! A 'mode' of the weapon.
   class WSF_MIL_EXPORT WsfWeaponMode : public WsfMode
   {
   public:
      WsfWeaponMode(const WsfScenario& aScenario);
      WsfWeaponMode(const WsfWeaponMode& aSrc);
      ~WsfWeaponMode() override = default;
      WsfWeaponMode& operator=(const WsfWeaponMode&) = delete;

      //! Return the pointer to the platform to which the associated weapon is attached.
      WsfPlatform* GetPlatform() const { return mPlatformPtr; }

      //! Return the pointer to the weapon with which this mode is associated.
      WsfWeapon* GetWeapon() const { return mWeaponPtr; }

      //! Create an exact duplicate of this mode.
      WsfMode* Clone() const override = 0;

      virtual bool Initialize(double aSimTime, WsfWeapon* aWeaponPtr);

      bool ProcessInput(UtInput& aInput) override;

      void Deselect(double aSimTime) override = 0;
      void Select(double aSimTime) override   = 0;

      const WsfScenario& GetScenario() const { return mScenario; }

   protected:
      const WsfScenario& mScenario;
      //! Pointer to the platform with which the weapon is associated.
      WsfPlatform* mPlatformPtr;
      //! Pointer to the weapon with which the mode is associated.
      WsfWeapon* mWeaponPtr;
   };

   //! @name Weapon status methods
   static double TimeWeaponLastFiredFor(const WsfPlatform& aPlatform,
                                        const WsfTrackId&  aTrackId,
                                        WsfStringId        aWeaponName = WsfStringId());

   static double TimeWeaponLastTerminatedFor(const WsfPlatform& aPlatform,
                                             const WsfTrackId&  aTrackId,
                                             WsfStringId        aWeaponName = WsfStringId());

   static int WeaponsPendingFor(const WsfPlatform& aPlatform,
                                const WsfTrackId&  aTrackId,
                                WsfStringId        aWeaponName = WsfStringId());

   static int WeaponsActiveFor(const WsfPlatform& aPlatform,
                               const WsfTrackId&  aTrackId,
                               WsfStringId        aWeaponName = WsfStringId());

   static int RoundsCompleteFor(const WsfPlatform& aPlatform,
                                const WsfTrackId&  aTrackId,
                                WsfStringId        aWeaponName = WsfStringId());

   static int WeaponsActiveAndPendingFor(const WsfPlatform& aPlatform,
                                         const WsfTrackId&  aTrackId,
                                         WsfStringId        aWeaponName = WsfStringId());

   static int RoundsFiredAt(const WsfPlatform& aPlatform,
                            const WsfTrackId&  aTrackId,
                            WsfStringId        aWeaponName = WsfStringId());

   static int SalvosFiredAt(const WsfPlatform& aPlatform,
                            const WsfTrackId&  aTrackId,
                            WsfStringId        aWeaponName = WsfStringId());

   static std::vector<size_t> ActiveWeaponPlatformsFor(const WsfPlatform& aPlatform,
                                                       const WsfTrackId&  aTrackId,
                                                       WsfStringId        aWeaponName = WsfStringId());

   static void DetermineFirepowerStatus(WsfPlatform& aPlatform);
   //@}

   WsfWeapon(const WsfScenario& aScenario);
   ~WsfWeapon() override;

   //! @name Component list support.
   //! A processor contains a list of components.
   //@{
   using ComponentList = WsfComponentListT<WsfWeaponComponent>;
   ComponentList&       GetComponents() { return mComponents; }
   const ComponentList& GetComponents() const { return mComponents; }
   //@}

   //! @name Component infrastructure methods not provided by WsfPlatformPart.
   //@{
   WsfComponent* CloneComponent() const override { return Clone(); }
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   int           GetComponentInitializationOrder() const override { return cWSF_INITIALIZE_ORDER_WEAPON; }
   //@}

   //! @name Common infrastructure methods.
   //@{
   const char* GetScriptClassName() const override;
   //! Clone this object.
   //! @return a pointer to the new object.
   WsfWeapon* Clone() const override = 0;
   bool       PreInitialize(double aSimTime) override;
   bool       Initialize(double aSimTime) override;
   bool       Initialize2(double aSimTime) override;
   void       PreInput() override;
   bool       ProcessInput(UtInput& aInput) override;
   void       Update(double aSimTime) override;
   //@}

   //! @name Status methods.
   //@{
   void TurnOff(double aSimTime) override;
   void TurnOn(double aSimTime) override;
   void SetNonOperational(double aSimTime) override;
   void SetOperational(double aSimTime) override;
   void PartBroken(double aSimTime) override;
   void Restore(double aSimTime) override;
   //@}

   //! Get the update interval.
   double GetUpdateInterval() const override { return mUpdateInterval; }

   //! @name Receiver/Transmitter access methods.
   //! These methods support accessing the underlying receiver and transmitter components.
   //@{
   virtual size_t      GetEM_RcvrCount() const;
   virtual WsfEM_Rcvr& GetEM_Rcvr(size_t aIndex);

   virtual size_t      GetEM_XmtrCount() const;
   virtual WsfEM_Xmtr& GetEM_Xmtr(size_t aIndex);
   virtual size_t      GetEM_XmtrCount(size_t aModeIndex) const;
   virtual WsfEM_Xmtr& GetEM_Xmtr(size_t aModeIndex, size_t aIndex);
   virtual WsfEM_Xmtr& GetEM_Xmtr(size_t aModeIndex, size_t aBeamIndex, size_t aIndex);
   //@}
   //! @name Mode selection methods.
   //! These methods support the control of a multi-mode weapons (particularly jammers).
   //! @note Not all implementations support multiple modes.
   //@{
   virtual WsfStringId    GetCurrentModeName() const;
   virtual WsfWeaponMode* GetCurrentMode();
   virtual size_t         GetModeCount();
   virtual WsfWeaponMode* GetModeEntry(size_t aModeIndex);
   virtual size_t         GetModeIndex(WsfStringId aModeNameId);
   virtual WsfStringId    GetModeName(size_t aModeIndex);
   virtual void           DeselectMode(double aSimTime, WsfStringId aModeNameId);
   virtual void           SelectMode(double aSimTime, WsfStringId aModeNameId);
   //@}

   //! @name Launch Computer Set/Get methods.
   //@{
   //! Get the launch computer pointer.
   //! @return The launch computer pointer.
   WsfLaunchComputer* GetLaunchComputer() const { return mLaunchComputerPtr; }

   void SetLaunchComputer(WsfLaunchComputer* aLaunchComputerPtr);
   //@}

   //! @name Weapon Effects methods
   //@{
   //! Get the string ID of the weapon effects to be used.
   WsfStringId GetWeaponEffectsType() const { return mWeaponEffectsType.GetConstReference(); }

   //! Set the string ID of the weapon effects to be used.
   void SetWeaponEffectsType(WsfStringId aWeaponEffect) { mWeaponEffectsType = aWeaponEffect; }
   //@}

   //! A target for a weapon to fire upon
   class FireTarget
   {
   public:
      FireTarget()
         : mTrackPtr(nullptr)
      {
      }
      FireTarget(const WsfTrack* aTrackPtr)
         : mTrackPtr(aTrackPtr)
      {
         if (mTrackPtr != nullptr)
         {
            mTrackId = mTrackPtr->GetTrackId();
         }
      }

      //! A pointer to the track that represents the target.
      //!        If mTrackPtr is null, will attempt to use WsfPlatform::GetCurrentTarget().
      const WsfTrack* mTrackPtr;

      //! The target name against which to fire the weapon.
      std::string mTargetName;

      WsfTrackId mTrackId; // For convenience only...

      //! An string indicating a targeted sub-region of the target
      //! (e.g., "canopy", "irst", "stabilizer"). This offset must be recognized by
      //! and used by the weapon effects type associated with the weapon.
      std::string mTargetOffset;
   };

   //! Weapon firing options
   class FireOptions
   {
   public:
      FireOptions()
         : mWeaponId(0)
      {
      }

      FireOptions(int aWeaponId)
         : mWeaponId(aWeaponId)
      {
      }

      //! The weapon id assigned to an allocated weapon platform.
      int mWeaponId;

      //! Name of the explicit weapon platform.  If empty, the weapon will create a new unique name.
      std::string mWeaponPlatformName;

      //! Name of the weapon component that will be used to fire.
      WsfStringId mWeaponComponentName;
   };

   //! Salvoing options
   class SalvoOptions : public FireOptions
   {
   public:
      SalvoOptions()
         : SalvoOptions(1)
      {
      }

      SalvoOptions(int aQuantity, double aSalvoInterval = -1.0, int aWeaponId = 0)
         : FireOptions(aWeaponId)
         , mQuantity(aQuantity)
         , mSalvoInterval(aSalvoInterval)
      {
      }

      int    mQuantity;      //! The number of rounds to fire
      double mSalvoInterval; //! The time between rounds. If < 0, salvo interval of weapon is used.
   };

   //! Result of WsfWeapon::Fire()
   class FireResult
   {
   public:
      FireResult()
         : mSuccess(false)
         , mEngagementPtr(nullptr)
      {
      }

      FireResult(bool aResult)
         : mSuccess(aResult)
         , mEngagementPtr(nullptr)
      {
      }

      operator bool() const { return mSuccess; }

      //! 'true' if WsfWeapon::Fire() is successful, 'false' otherwise.
      bool mSuccess;
      //! The pointer to the weapon engagement object that was created in response
      //!        WsfWeapn::Fire(). This will be zero if the weapon was not fired (the return value from the method is
      //!       'false') OR if the underlying weapon does not support weapon engagement objects.
      WsfWeaponEngagement* mEngagementPtr;
   };

   //! @name Firing and cueing methods
   //@{
   virtual void CueToTarget(double aSimTime, const WsfTrack* aTrackPtr);

   virtual FireResult Fire(double aSimTime, const FireTarget& aTarget, const FireOptions& aSettings);

   virtual bool FireSalvo(double aSimTime, const FireTarget& aTarget, const SalvoOptions& aSettings);

   virtual void ContinueSalvo(double aSimTime, unsigned int aRequestId);

   virtual void AbortSalvo(double aSimTime, const WsfTrackId& aTrackId);

   virtual void CeaseFire(double aSimTime);

   //! Return the number of 'FireSalvo' calls that are in process.
   virtual unsigned int GetActiveRequestCount() const { return static_cast<unsigned int>(mSalvoRequestList.size()); }

   //! Return the maximum number of 'FireSalvo' calls that may be in process at a time.
   //! For RF jammers, this represents the maximum number of spots per mode.
   virtual unsigned int GetMaximumRequestCount() const { return mMaximumRequestCount; }
   //@}

   //! @name Time methods.
   //@{
   //! Get the simulation time when the weapon was last fired.
   double GetTimeLastFired() const { return mTimeLastFired; }

   //! Get the amount of time that must elapse from the time a request is made until
   //! the first round can be fired.
   //! @note This may be a statistical distribution. Each call causes a new draw and may be zero.
   virtual double GetFiringDelay() { return mFiringDelay.Draw(); }

   //! Set the amount of time that must elapse from the time a request is made until
   //! the first round can be fired.
   //! @note This may be a statistical distribution.
   void SetFiringDelay(const WsfRandomVariable& aFiringDelay) { mFiringDelay = aFiringDelay; }

   //! Get the amount of time that must elapse between rounds in a multi-round salvo.
   //! @note This may be a statistical distribution. Each call causes a new draw and may be zero.
   virtual double GetSalvoInterval() { return mSalvoInterval.Draw(); }

   //! Get the amount of time that must elapse between firings.
   //! Time from the completion of the last request until the start of
   //! the next request (trailing edge to leading edge).
   double GetFiringInterval() const { return mFiringInterval; }

   //! Set the amount of time that must elapse between firings.
   //! Time from the completion of the last request until the start of
   //! the next request (trailing edge to leading edge).
   void SetFiringInterval(double aFiringInterval) { mFiringInterval = aFiringInterval; }
   //@}

   //! @name Launched platform type methods.
   //@{
   //! Get the type of the platform that will be launched.
   WsfStringId GetLaunchedPlatformType() const { return mLaunchedPlatformType; }

   //! Set the type of the platform that will be launched.
   void SetLaunchedPlatformType(WsfStringId aLaunchedPlatformType) { mLaunchedPlatformType = aLaunchedPlatformType; }
   //@}

   //! @name Inventory methods.
   //! The definition of quantity depends on the type of weapon. For some weapons it might be
   //! 'rounds', for others it might be 'Joules'.
   //@{

   //! Get the initial state of the weapon layout.
   //! Return the initial quantities.
   double         GetInitialStateQuantityRemaining() const { return mInitialStateQuantityRemaining; }
   virtual double GetQuantityRemaining() const;
   virtual double GetQuantityCommitted() const;
   virtual double GetMaximumQuantity() const;
   virtual double GetTotalQuantityUsed() const;
   virtual void   SetQuantityRemaining(double aQuantity);
   void           ChangeQuantity(double aDeltaQuantity);
   //@}

   //! @name Reload specification methods.
   //! These methods support the control of weapon inventory replenishment.
   //! For maximum user flexibility, the IsReloading() flag is accessible via script,
   //! the user must determine whether salvo firing will be interrupted during reload.
   //! @note Not all implementations support weapons reloading.
   //@{
   //! Flag indicating that weapon reload is in progress.
   //! If true a weapon reload is in progress and the amount of inventory in 'external' replenishment
   //! storage has been decremented, but the weapon quantity has not yet been incremented.
   bool IsReloading() const { return mIsReloading; }

   //! Flag indicating that when a weapon reload is in progress, any firing will be inhibited.
   bool InhibitWhileReloading() const { return mInhibitWhileReloading; }

   virtual void CompleteReloading(double aSimTime, double aQuantity);

   //! Returns the quantity available in 'external inventory'
   //! This amount will not be consumed during weapon firing, but will be during a reload.
   double GetReloadInventory() const { return mReloadInventory; }

   //! Returns the depleted value of inventory at which a reload is attempted.
   double GetReloadThreshold() const { return mReloadThreshold; }

   //! Flag indicating the (nominal) amount of inventory replenished during a reload event.
   //! The nominal reload increment will be reduced if the 'external inventory' contains less
   //! than this amount, or if the full increment would exceed the maximum capacity of the weapon.
   double GetReloadIncrement() const { return mReloadIncrement; }
   //@}

   //! @name Helpers for working with target data.
   //! Sometimes a weapon needs a track against which to fire and the track has to have location
   //! data. These methods help dealing with cases where the required data may not be available,
   //! but other mechanisms may have been defined that indicate how to construct a location.
   //@{
   WsfTrack* CreateTargetTrack(double aSimTime, const WsfTrack* aTrackPtr);

   bool GetTargetLocationWCS(double aSimTime, const WsfTrack* aTrackPtr, double aTargetLocWCS[3]);
   //@}

   //! @name Target cueing data methods.
   //! Currently used in WsfRF_Jammer::JammerXmtr's functionalities for DIS operations,
   //! other future implementations are possible. The target data is stored in the inheriting classes.
   //@{
   virtual void   SetTargetIndex(int aId, size_t aTargetIndex) {}
   virtual size_t GetTargetIndex(int aId) const { return 0; }

   virtual void SetTargetedLocationWCS(int aId, const double aTargetedLocationWCS[3]) {}
   virtual void GetTargetedLocationWCS(int aId, double aTargetedLocationWCS[3]) const {}

   virtual bool TargetedLocationValid(int aId) const { return false; }

   //@}

   //! @name Field of view methods.
   //! Determines if the platform is within the weapons field of view.
   //@{
   virtual bool WithinFieldOfView(double aSimTime, WsfPlatform* aPlatformPtr);
   virtual bool WithinFieldOfView(double aSimTime, WsfTrack* aTrackPtr);
   //@}

   //! @name Jammer methods.
   //! These methods support jammer tasking.
   //@{

   virtual bool StartJamming(double      aSimTime,
                             double      aFrequency,
                             double      aBandwidth,
                             size_t      aBeamNumber,
                             WsfStringId aTechniqueId,
                             WsfTrackId  aTrackId,
                             size_t      aTargetIndex)
   {
      return false;
   }
   virtual bool StartJammingTrack(double aSimTime, WsfStringId aTechniqueId, const WsfTrack* aTrackPtr)
   {
      return false;
   }

   virtual bool StopJamming(double aSimTime, double aFrequency, double aBandwidth, size_t aBeamNumber, WsfTrackId aTrackId)
   {
      return false;
   }

   virtual bool UpdateJamming(double      aSimTime,
                              size_t      aXmtrIndex,
                              double      aFrequency,
                              double      aBandwidth,
                              WsfStringId aTechniqueId,
                              WsfTrackId  aTrackId,
                              size_t      aTargetIndex)
   {
      return false;
   }

   virtual size_t GetActiveBeamCount() const { return 0; }
   virtual size_t GetMaximumBeamCount() const { return 0; }
   virtual size_t GetActiveSpotCount(size_t aBeamNumber) const { return 0; }
   virtual size_t GetMaximumSpotCount(size_t aBeamNumber) const { return 0; }
   //@}

   //! @name EA methods
   //! These methods support EA jammer tasking.
   //@{
   virtual bool SelectEA_Technique(double      aSimTime,
                                   double      aFrequency,
                                   double      aBandwidth,
                                   size_t      aBeamNumber,
                                   WsfStringId aTechniqueId,
                                   WsfTrackId  aTrackId)
   {
      return false;
   }

   virtual bool DeselectEA_Technique(double      aSimTime,
                                     double      aFrequency,
                                     double      aBandwidth,
                                     size_t      aBeamNumber,
                                     WsfStringId aTechniqueId,
                                     WsfTrackId  aTrackId)
   {
      return false;
   }

   virtual bool SetEA_DeltaGainTechnique(double      aSimTime,
                                         WsfStringId aTechniqueId,
                                         bool        aOnOff,
                                         WsfStringId aEffectId,
                                         double      aJammingDeltaGain_dB,
                                         WsfStringId aSystemTypeId,
                                         WsfStringId aFunctionTypeId)
   {
      return false;
   }

   virtual bool SetEA_DeltaGainTechnique(double      aSimTime,
                                         WsfStringId aTechniqueId,
                                         bool        aOnOff,
                                         WsfStringId aEffectId,
                                         double      aJammingDeltaGain_dB,
                                         WsfStringId aSystemTypeId,
                                         WsfStringId aFunctionTypeId,
                                         double      aFrequency,
                                         double      aBandwidth,
                                         size_t      aBeamNumber,
                                         WsfTrackId  aTrackId)
   {
      return false;
   };
   //@}

   //! @name Methods for simulation control of the update interval.
   //! @note These methods are only intended for use by simulation executives (classes derived from WsfSimulation)
   //! and WsfPlatformPartEvent and should not be invoked from any other place. Any other use is at your own risk.
   //{@
   //! Set the time interval between the periodic calls to Update().
   //! A value of less than or equal to zero indicates periodic updates are not to be performed.
   //! @param aUpdateInterval The new update interval.
   //! @note Users should call WsfSimulation::SetPartUpdateInterval() to set the interval.
   void SetUpdateInterval(double aUpdateInterval) override { mUpdateInterval = aUpdateInterval; }
   //@}

   //! @name Query methods
   //@{
   double              TimeWeaponLastFiredFor(const WsfTrackId& aTrackId) const;
   double              TimeWeaponLastTerminatedFor(const WsfTrackId& aTrackId) const;
   int                 WeaponsPendingFor(const WsfTrackId& aTrackId) const;
   int                 WeaponsActiveFor(const WsfTrackId& aTrackId) const;
   int                 RoundsCompleteFor(const WsfTrackId& aTrackId) const;
   int                 WeaponsActiveAndPendingFor(const WsfTrackId& aTrackId) const;
   int                 RoundsFiredAt(const WsfTrackId& aTrackId) const;
   int                 SalvosFiredAt(const WsfTrackId& aTrackId) const;
   std::vector<size_t> ActiveWeaponPlatformsFor(const WsfTrackId& aTrackId) const;
   //@}

   virtual void EngagementComplete(double aSimTime, WsfWeaponEngagement* aEngagementPtr);

   //! Set to true is IFC is supported
   void SetIFCEnabled(bool aEnabled) { mIFCEnabled = aEnabled; }

   //! Returns 'true' if the platform supports IFC (Integrated Fire Control)
   bool GetIFCEnabled() const { return mIFCEnabled; }

   //! @name Weapon server support.
   //@{
   virtual bool AllocateTheWeapon(double aSimTime, WsfTrack* aTrackPtr, int aStoreId);

   void SetState(WeaponState aState);

   //! Return weapon state.
   WeaponState GetState() const { return mState; }

   //@}

protected:
   WsfWeapon(const WsfWeapon& aSrc);
   WsfWeapon& operator=(const WsfWeapon&) = delete;

   bool IsQuantitySpecified() const { return mIsQuantitySpecified; }

   virtual void ProcessReloading(double aSimTime);

   //! Method used for derived classes to set or clear the IsReloading Flag.
   void SetIsReloading(bool aValue) { mIsReloading = aValue; }

   void SetModeList(WsfModeList* aModeListPtr);

   void SetTimeLastFired(double aTimeLastFired);

   void SetMaximumRequestCount(unsigned int aMaximumRequestCount) { mMaximumRequestCount = aMaximumRequestCount; }

   class WSF_MIL_EXPORT SalvoEvent : public WsfEvent
   {
   public:
      SalvoEvent(double aSimTime, WsfWeapon* aWeaponPtr, unsigned int aRequestId);
      ~SalvoEvent() override = default;
      EventDisposition Execute() override;

   private:
      WsfWeapon*   mWeaponPtr;
      size_t       mPlatformIndex;
      unsigned int mRequestId;
   };

   class WSF_MIL_EXPORT CompleteReloadEvent : public WsfEvent
   {
   public:
      CompleteReloadEvent(double aCompletionTime, double aReloadedAmount, WsfWeapon* aWeaponPtr);
      ~CompleteReloadEvent() override = default;
      EventDisposition Execute() override;

   private:
      WsfWeapon* mWeaponPtr;
      size_t     mPlatformIndex;
      double     mQuantity;
   };

   struct SalvoRequest
   {
   public:
      SalvoRequest(unsigned int aRequestId, const FireTarget& aTarget, const SalvoOptions& aSettings);
      virtual ~SalvoRequest();

      unsigned int mRequestId;
      FireTarget   mTarget;
      SalvoOptions mSettings;
   };

protected:
   //! The pointer to the list of modes (for things like jammers).
   WsfModeList* mModeListPtr;

   double GetReloadInterval() const { return mReloadTime; }

   //! A list of active weapons fired
   //!
   //! An entry is created in the WsfExplicitWeapon::AddWeaponToSimulation (from Fire())
   //! An entry is removed in the WeaponTerminated() callback.
   //!
   struct ActiveWeapon
   {
      ActiveWeapon(size_t weaponPlatIndex, unsigned int serialNum)
         : mPlatformIndex(weaponPlatIndex)
         , mSerialNumber(serialNum)
      {
      }
      //! Platform index of the launched weapon platform.
      size_t mPlatformIndex;
      //! The serial number of the weapon engagement
      unsigned int mSerialNumber;
   };

   //! Persistent track/weapon status data.
   //!
   //! This information represents the on-going and historical data about this weapon.
   //! This information may be needed by user decision making processes.
   //!
   //! An entry exists for each specific track
   //! Multiple fire events on the same track are merged into one entry.
   //!
   struct WeaponStatus
   {
   public:
      WeaponStatus()
         : mTimeWeaponLastFired(-1.0)
         , mTimeWeaponLastTerminated(-1.0)
         , mTotalSalvosFired(0)
         , mRoundsCompleted(0)
         , mRoundsPending(0)
         , mRoundsActive(0)
      {
      }
      //! The simulation time when of the most recent weapon firing event for this task.
      double mTimeWeaponLastFired;
      //! The simulation time when of the most recent weapon termination event for this task.
      double mTimeWeaponLastTerminated;
      //! The number of salvos that have been fired.
      int mTotalSalvosFired;
      //! The total number of weapons that have been fired and terminated for this task.
      int mRoundsCompleted;
      //! The number of weapons that are pending (requested but not yet in flight)
      int mRoundsPending;
      //! The number of weapons that are active (in flight).
      int mRoundsActive;
      //! A list of active weapon platforms
      std::list<ActiveWeapon> mActiveWeaponList;
   };
   using WeaponStatusMap          = std::map<WsfTrackId, WeaponStatus>;
   using WeaponStatusMapIter      = WeaponStatusMap::iterator;
   using WeaponStatusMapConstIter = WeaponStatusMap::const_iterator;
   using WeaponStatusVal          = std::pair<WsfTrackId, WeaponStatus>;
   WeaponStatusMap mWeaponStatusMap;

   virtual void WeaponFireAborted(double aSimTime, const WsfTrack* aTargetTrackPtr, double aQuantity);

private:
   enum AltitudeReference
   {
      cAR_AGL,
      cAR_MSL,
      cAR_RELATIVE
   };

   using SalvoRequestList = std::vector<std::unique_ptr<SalvoRequest>>;

   ComponentList mComponents;

   //! The update interval is time between the periodic calls to Update() by the simulation executive.
   //! If less than or equal to zero then the simulation executive should not make the periodic calls.
   WsfVariable<double> mUpdateInterval;

   //! The current update event epoch.
   //! This is used to identify and reject obsolete 'update' events.
   unsigned int mUpdateEventEpoch;

   //! True if an event-driven periodic call to Update() is in progress.
   bool mUpdateEventActive;

   //! Indicates what type of platform launched by the weapon.
   //! In the case of an explicit weapon, this type will be instantiated when the weapon is fired.
   //! For implicit weapons it may or may not be defined.
   WsfVariable<WsfStringId> mLaunchedPlatformType;

   //! Indicates what type of effect is to be used to assess the effect a weapon has on a target.
   WsfVariable<WsfStringId> mWeaponEffectsType;

   //! Quantity remaining on the weapon itself (in units dictated by the weapon).
   //! (This value is exclusive of the External Replenishment Magazine (ERM) quantity.)
   //! Expressed as a double to allow things like directed energy.
   WsfVariable<double> mQuantityRemaining;

   //! Indicates whether initial quantity is specified by the user via the 'quantity' command.
   //! Allows derived classes to implement a suitable default value.
   bool mIsQuantitySpecified;

   //! Initial state of quantity remaining on the weapon itself (in units dictated by the weapon).
   //! (This value is exclusive of the External Replenishment Magazine (ERM) quantity.)
   //! Expressed as a double to allow things like directed energy.
   double mInitialStateQuantityRemaining;

   //! The maximum quantity loadable onto the weapon itself (in units dictated by the weapon).
   //! (This value is exclusive of the 'external replenishment magazine' quantity.)
   WsfVariable<double> mMaximumQuantity;

   //! The total quantity that has been fired (in units dictated by the weapon).
   double mTotalQuantityUsed;

   //! The quantity committed but not yet fired.
   double mQuantityCommitted;

   //! The depleted value of inventory that initiates a reload from the External Replenishment Magazine (ERM).
   WsfVariable<double> mReloadThreshold;

   //! The incremental quantity of inventory to place back into the weapon (if available)
   //! from the External Replenishment Magazine (ERM).
   WsfVariable<double> mReloadIncrement;

   //! The finite External Replenishment Magazine (ERM) quantity used as a source to reload the weapon.
   //! If this value is set zero (the default), no reloading is carried out.
   WsfVariable<double> mReloadInventory;

   //! The time it takes to transfer an incremental amount of inventory from the
   //! External Replenishment Magazine (ERM) to the weapon.
   WsfRandomVariable mReloadTime;

   //! The time that must elapse between the time a request is made and when the first round is actually fired.
   WsfRandomVariable mFiringDelay;

   //! The time between shots in a salvo.
   WsfRandomVariable mSalvoInterval;

   //! The time that must elapse between fire requests.
   WsfVariable<double> mFiringInterval;

   //! The time when the weapon was last fired.
   double mTimeLastFired;

   //! The maximum number of firing/jamming requests than can occur simultaneously.
   //! For jamming requests, this is dependent on the mode.
   unsigned int mMaximumRequestCount;

   //! The next unique ID for salvo requests.
   unsigned int mSalvoRequestId;

   //! The request IDs for all active salvo requests.
   SalvoRequestList mSalvoRequestList;

   //! The range to be used for creating a target location when otherwise not known.
   //! If the target track was either not specified or did not contain a location, then
   //! this value will be used to construct a target location.
   WsfVariable<double> mUnknownTargetRange;

   //! The altitude associated with an unknown target.
   WsfVariable<double> mUnknownTargetAltitude;

   //! The altitude reference for the altitude for unknown targets.
   AltitudeReference mUnknownTargetAltitudeRef;

   //! Weapon server state
   WeaponState mState;

   //! If true, FireSalvo will cue to the target track (if device cueing is allowed)
   bool mAutomaticTargetCueing;

   //! If true, CueToTarget will cue to the predicted intercept location.
   bool mCueToPredictedIntercept;

   //! Flag indicating that the weapon is unavailable because it is reloading.
   bool mIsReloading;

   //! Flag indicating that weapon firing is inhibited while reload is in progress.
   bool mInhibitWhileReloading;

   //! True if integrated fire control networking is enabled on this weapon
   bool mIFCEnabled;

   WsfLaunchComputer* mLaunchComputerPtr;

   //! The generic list of modes (not valid until Initialize is called)
   std::vector<WsfWeaponMode*> mWeaponModeList;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfWeapon, cWSF_COMPONENT_WEAPON)

#endif
