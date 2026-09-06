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

#include "WsfWeapon.hpp"

#include <algorithm>
#include <cfloat>
#include <iostream>
#include <memory>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfComponentFactoryList.hpp"
#include "WsfComponentListDefines.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEvent.hpp"
#include "WsfLaunchComputer.hpp"
#include "WsfLaunchComputerTypes.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMil.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackManager.hpp"
#include "WsfWeaponEffects.hpp"
#include "WsfWeaponEffectsTypes.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponObserver.hpp"

// static
const double WsfWeapon::cUNDEFINED_DOUBLE = 1.0E+30;

// ================================================================================================
WsfWeapon::WsfWeapon(const WsfScenario& aScenario)
   : WsfArticulatedPart(aScenario, cCOMPONENT_ROLE<WsfWeapon>())
   , mModeListPtr(nullptr)
   , mComponents()
   , mUpdateInterval(0.0)
   , mLaunchedPlatformType(WsfStringId())
   , mWeaponEffectsType(WsfStringId())
   , mQuantityRemaining(0.0)
   , mIsQuantitySpecified(false)
   , mMaximumQuantity(DBL_MAX)
   , mTotalQuantityUsed(0.0)
   , mQuantityCommitted(0.0)
   , mReloadThreshold(0.0)
   , mReloadIncrement(0.0)
   , mReloadInventory(0.0)
   , mReloadTime(WsfRandomVariable::cCONSTANT, 0.0, 0.0, WsfRandomVariable::cNON_NEGATIVE)
   , mFiringDelay(WsfRandomVariable::cCONSTANT, 0.0, 0.0, WsfRandomVariable::cNON_NEGATIVE)
   , mSalvoInterval(WsfRandomVariable::cCONSTANT, 0.0, 0.0, WsfRandomVariable::cNON_NEGATIVE)
   , mFiringInterval(0.0)
   , mTimeLastFired(-DBL_MAX)
   , mMaximumRequestCount(1)
   , mSalvoRequestId(0)
   , mSalvoRequestList()
   , mUnknownTargetRange(0.0)
   , mUnknownTargetAltitude(1.0)
   , mUnknownTargetAltitudeRef(cAR_AGL)
   , mState(cWEAPON_STATE_UNALLOCATED)
   , mAutomaticTargetCueing(true)
   , mCueToPredictedIntercept(false)
   , mIsReloading(false)
   , mInhibitWhileReloading(false)
   , mIFCEnabled(false)
   , mLaunchComputerPtr(nullptr)
   , mWeaponModeList()
{
   WsfPlatformPart::SetInitiallyTurnedOn(true);
   mComponents.SetParentOfComponents(this);
}

// ================================================================================================
//! Copy constructor (for Clone()).
// protected
WsfWeapon::WsfWeapon(const WsfWeapon& aSrc)
   : WsfArticulatedPart(aSrc)
   , mModeListPtr(nullptr)
   , mComponents(aSrc.mComponents)
   , mUpdateInterval(aSrc.mUpdateInterval)
   , mLaunchedPlatformType(aSrc.mLaunchedPlatformType)
   , mWeaponEffectsType(aSrc.mWeaponEffectsType)
   , mQuantityRemaining(aSrc.mQuantityRemaining)
   , mIsQuantitySpecified(aSrc.mIsQuantitySpecified)
   , mMaximumQuantity(aSrc.mMaximumQuantity)
   , mTotalQuantityUsed(aSrc.mTotalQuantityUsed)
   , mQuantityCommitted(aSrc.mQuantityCommitted)
   , mReloadThreshold(aSrc.mReloadThreshold)
   , mReloadIncrement(aSrc.mReloadIncrement)
   , mReloadInventory(aSrc.mReloadInventory)
   , mReloadTime(aSrc.mReloadTime)
   , mFiringDelay(aSrc.mFiringDelay)
   , mSalvoInterval(aSrc.mSalvoInterval)
   , mFiringInterval(aSrc.mFiringInterval)
   , mTimeLastFired(aSrc.mTimeLastFired)
   , mMaximumRequestCount(aSrc.mMaximumRequestCount)
   , mSalvoRequestId(0)
   , mSalvoRequestList()
   , mUnknownTargetRange(aSrc.mUnknownTargetRange)
   , mUnknownTargetAltitude(aSrc.mUnknownTargetAltitude)
   , mUnknownTargetAltitudeRef(aSrc.mUnknownTargetAltitudeRef)
   , mAutomaticTargetCueing(aSrc.mAutomaticTargetCueing)
   , mCueToPredictedIntercept(aSrc.mCueToPredictedIntercept)
   , mIsReloading(aSrc.mIsReloading)
   , mInhibitWhileReloading(aSrc.mInhibitWhileReloading)
   , mIFCEnabled(aSrc.mIFCEnabled)
   , mLaunchComputerPtr(nullptr)
   , mWeaponModeList()
{
   mComponents.SetParentOfComponents(this);
   if (aSrc.mModeListPtr != nullptr)
   {
      mModeListPtr = new WsfModeList(*(aSrc.mModeListPtr));
   }
   if (aSrc.mLaunchComputerPtr != nullptr)
   {
      mLaunchComputerPtr = aSrc.mLaunchComputerPtr->Clone();
   }
}

// ================================================================================================
// virtual
WsfWeapon::~WsfWeapon()
{
   delete mModeListPtr;
   delete mLaunchComputerPtr;
}

// ================================================================================================
const int* WsfWeapon::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_WEAPON,
                               cWSF_COMPONENT_ARTICULATED_PART,
                               cWSF_COMPONENT_PLATFORM_PART,
                               cWSF_COMPONENT_NULL};
   return roles;
}

// ================================================================================================
void* WsfWeapon::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_WEAPON)
   {
      return this;
   }
   return WsfArticulatedPart::QueryInterface(aRole);
}

// ================================================================================================
const char* WsfWeapon::GetScriptClassName() const
{
   return "WsfWeapon";
}

// ================================================================================================
// virtual
bool WsfWeapon::PreInitialize(double aSimTime)
{
   bool ok = WsfArticulatedPart::PreInitialize(aSimTime);

   // Let component factories inject components.
   WsfComponentFactoryList<WsfWeapon>::PreInitialize(GetScenario(), aSimTime, *this);

   for (auto component : mComponents)
   {
      ok &= component->PreInitialize(aSimTime);
   }
   return ok;
}

// ================================================================================================
// virtual
bool WsfWeapon::Initialize(double aSimTime)
{
   bool ok = WsfArticulatedPart::Initialize(aSimTime);

   // Resolve any potential script variable references.
   WsfScriptContext* contextPtr    = &GetPlatform()->GetScriptContext();
   WsfScriptContext& globalContext = GetSimulation()->GetScriptContext();
   ok &= mUpdateInterval.Initialize("update_interval", this, contextPtr, globalContext);
   ok &= mWeaponEffectsType.Initialize("weapon_effects", this, contextPtr, globalContext);
   ok &= mQuantityRemaining.Initialize("quantity", this, contextPtr, globalContext);
   ok &= mLaunchedPlatformType.Initialize("launched_platform_type", this, contextPtr, globalContext);
   ok &= mMaximumQuantity.Initialize("maximum_quantity", this, contextPtr, globalContext);
   ok &= mReloadThreshold.Initialize("reload_threshold", this, contextPtr, globalContext);
   ok &= mReloadIncrement.Initialize("reload_increment", this, contextPtr, globalContext);
   ok &= mReloadInventory.Initialize("reload_inventory", this, contextPtr, globalContext);
   ok &= mReloadTime.Initialize("reload_time", this, contextPtr, *GetSimulation()),
      ok &= mFiringDelay.Initialize("firing_delay", this, contextPtr, *GetSimulation());
   ok &= mSalvoInterval.Initialize("salvo_interval", this, contextPtr, *GetSimulation());
   ok &= mFiringInterval.Initialize("firing_interval", this, contextPtr, globalContext);
   ok &= mUnknownTargetRange.Initialize("unknown_target_range", this, contextPtr, globalContext);
   ok &= mUnknownTargetAltitude.Initialize("unknown_target_altitude", this, contextPtr, globalContext);

   if (mLaunchComputerPtr != nullptr)
   {
      ok &= mLaunchComputerPtr->Initialize(aSimTime, this);
   }

   if (mModeListPtr != nullptr)
   {
      // Initialize the mode list.
      ok &= mModeListPtr->Initialize();

      // Reduce future dynamic casting by extracting derived class mode pointers.
      mModeListPtr->GetDerivedModeList(mWeaponModeList);

      // Initialize the modes.
      for (auto& wpnMode : mWeaponModeList)
      {
         ok &= wpnMode->Initialize(aSimTime, this);
      }
   }

   // Clamp the quantity to whatever the maximum value allowed is.
   SetQuantityRemaining(mQuantityRemaining);
   mInitialStateQuantityRemaining = mQuantityRemaining;

   if (mWeaponEffectsType.GetId() != 0)
   {
      // If a weapon effects type has been specified, then make sure it is a valid type.
      if (WsfWeaponEffectsTypes::Get(GetScenario()).Find(mWeaponEffectsType) == nullptr)
      {
         auto out = ut::log::error() << "Undefined weapon_effects for platform weapon.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Weapon: " << GetName();
         out.AddNote() << "weapon_effects: " << mWeaponEffectsType.GetString();
         ok = false;
      }
   }

   for (auto component : mComponents)
   {
      ok &= component->Initialize(aSimTime);
   }

   return ok;
}

// ============================================================================
// virtual
bool WsfWeapon::Initialize2(double aSimTime)
{
   bool ok = WsfArticulatedPart::Initialize2(aSimTime);

   for (auto component : mComponents)
   {
      ok &= component->Initialize2(aSimTime);
   }
   return ok;
}

// =================================================================================================
// See WsfComponent::PreInput about this.
void WsfWeapon::PreInput()
{
   WsfComponentFactoryList<WsfWeapon>::PreInput(GetScenario(), *this);
}

// ================================================================================================
// virtual
bool WsfWeapon::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   bool        addLaunchComputer = (mLaunchComputerPtr == nullptr) ? true : false;

   if (command == "quantity")
   {
      mQuantityRemaining.ReadValue(aInput);
      mQuantityRemaining.ValueGreaterOrEqual(aInput, 0.0);
      mIsQuantitySpecified = true;
   }
   else if (command == "maximum_quantity")
   {
      mMaximumQuantity.ReadValue(aInput);
      mMaximumQuantity.ValueGreater(aInput, 0.0);
   }
   else if (command == "launched_platform_type")
   {
      mLaunchedPlatformType.ReadValue(aInput);
   }
   else if (command == "firing_delay")
   {
      mFiringDelay.ProcessInput(aInput, UtInput::cTIME);
   }
   else if (command == "salvo_interval")
   {
      mSalvoInterval.ProcessInput(aInput, UtInput::cTIME);
   }
   else if (command == "firing_interval")
   {
      mFiringInterval.ReadValueOfType(aInput, UtInput::cTIME);
      mFiringInterval.ValueGreaterOrEqual(aInput, 0.0);
   }
   else if (command == "update_interval")
   {
      mUpdateInterval.ReadValueOfType(aInput, UtInput::cTIME);
      mUpdateInterval.ValueGreater(aInput, 0.0);
   }
   else if (command == "maximum_request_count")
   {
      aInput.ReadValue(mMaximumRequestCount);
      aInput.ValueGreaterOrEqual(mMaximumRequestCount, 1U);
   }
   else if (command == "unknown_target_range")
   {
      mUnknownTargetRange.ReadValueOfType(aInput, UtInput::cLENGTH);
      mUnknownTargetRange.ValueGreaterOrEqual(aInput, 0.0);
   }
   else if (command == "unknown_target_altitude")
   {
      mUnknownTargetAltitude.ReadValueOfType(aInput, UtInput::cLENGTH);
      std::string reference;
      aInput.ReadValue(reference);
      if (reference == "agl")
      {
         mUnknownTargetAltitudeRef = cAR_AGL;
      }
      else if (reference == "msl")
      {
         mUnknownTargetAltitudeRef = cAR_MSL;
      }
      else if (reference == "relative")
      {
         mUnknownTargetAltitudeRef = cAR_RELATIVE;
      }
      else
      {
         throw UtInput::BadValue(aInput, "altitude reference must be agl, msl or relative");
      }
   }
   else if (command == "automatic_target_cueing")
   {
      aInput.ReadValue(mAutomaticTargetCueing);
   }
   else if (command == "cue_to_predicted_intercept")
   {
      aInput.ReadValue(mCueToPredictedIntercept);
   }
   else if (command == "reload_threshold")
   {
      mReloadThreshold.ReadValue(aInput);
      mReloadThreshold.ValueGreaterOrEqual(aInput, 0.0);
   }
   else if (command == "reload_increment")
   {
      mReloadIncrement.ReadValue(aInput);
      mReloadIncrement.ValueGreaterOrEqual(aInput, 0.0);
   }
   else if (command == "reload_inventory")
   {
      mReloadInventory.ReadValue(aInput);
      mReloadInventory.ValueGreaterOrEqual(aInput, 0.0);
   }
   else if (command == "reload_time")
   {
      mReloadTime.ProcessInput(aInput, UtInput::cTIME);
   }
   else if (command == "inhibit_while_reloading")
   {
      aInput.ReadValue(mInhibitWhileReloading);
   }
   else if (command == "weapon_effects")
   {
      mWeaponEffectsType.ReadValue(aInput);
   }
   else if (command == "fire_inhibit_while_reloading")
   {
      mInhibitWhileReloading = true;
      auto out               = ut::log::warning() << "'fire_inhibit_while_reloading' has been deprecated.";
      out.AddNote() << "Use 'inhibit_while_reloading true'.";
   }
   else if (command == "no_fire_inhibit_while_reloading")
   {
      mInhibitWhileReloading = false;
      auto out               = ut::log::warning() << "'no_fire_inhibit_while_reloading' has been deprecated.";
      out.AddNote() << "Use 'inhibit_while_reloading false'.";
   }
   else if (command == "ifc_enabled")
   {
      aInput.ReadValue(mIFCEnabled);
   }
   else if (WsfLaunchComputerTypes::Get(GetScenario()).LoadInstance(aInput, this, addLaunchComputer))
   {
   }
   else if (WsfAuxDataEnabled::ProcessInput(aInput))
   {
      // NOTE: This needs to stay before the mode list process input check
   }
   else if ((mModeListPtr != nullptr) && mModeListPtr->ProcessInput(aInput))
   {
   }
   else if (WsfArticulatedPart::ProcessInput(aInput))
   {
   }
   else if (mComponents.ProcessComponentInput(aInput))
   {
      // First try components already attached. If the input was not recognized by one of them then
      // try the component factories, which may result in the dynamic creation of a new component.
   }
   else if (WsfComponentFactoryList<WsfWeapon>::ProcessInput(GetScenario(), aInput, *this))
   {
      // If not processed by an attached component try the component factories to see if a component
      // should be dynamically attached.
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// ================================================================================================
//! Update the state of the weapon to the supplied time value.
// virtual
void WsfWeapon::Update(double aSimTime)
{
   if (mLaunchComputerPtr != nullptr)
   {
      mLaunchComputerPtr->Update(aSimTime);
   }
}

// ================================================================================================
//! Turn the weapon off.
//! @param aSimTime The current simulation time.
//! @note  This method should not be invoked directly by application code.
//!        Use the WsfSimulation::TurnWeaponOff method.
// virtual
void WsfWeapon::TurnOff(double aSimTime)
{
   AbortSalvo(aSimTime, WsfTrackId());
   if (mModeListPtr != nullptr)
   {
      mModeListPtr->TurnOff(aSimTime);
   }
   WsfPlatformPart::TurnOff(aSimTime);
   mSalvoRequestList.clear();
   WSF_CALL_VOID_COMPONENTS(TurnOff, aSimTime);
   WsfObserver::WeaponTurnedOff(GetSimulation())(aSimTime, this);
}

// ================================================================================================
//! Turn the weapon on.
//! @param aSimTime The current simulation time.
//! @note  This method should not be invoked directly by application code.
//!        Use the WsfSimulation::TurnWeaponOn method.
// virtual
void WsfWeapon::TurnOn(double aSimTime)
{
   if (mModeListPtr != nullptr)
   {
      mModeListPtr->TurnOn(aSimTime);
   }
   WsfPlatformPart::TurnOn(aSimTime);
   WSF_CALL_VOID_COMPONENTS(TurnOn, aSimTime);
   WsfObserver::WeaponTurnedOn(GetSimulation())(aSimTime, this);
}

// ================================================================================================
//! Set the status to non-Operational.
//! @param aSimTime The current simulation time.
//! @note  This method should not be invoked directly by application code.
// virtual
void WsfWeapon::SetNonOperational(double aSimTime)
{
   WsfPlatformPart::SetNonOperational(aSimTime);
   WsfObserver::WeaponNonOperational(GetSimulation())(aSimTime, this);
}

// ================================================================================================
//! Set the status to Operational.
//! @param aSimTime The current simulation time.
//! @note  This method should not be invoked directly by application code.
// virtual
void WsfWeapon::SetOperational(double aSimTime)
{
   WsfPlatformPart::SetOperational(aSimTime);
   WsfObserver::WeaponOperational(GetSimulation())(aSimTime, this);
}

// ================================================================================================
//! Set the status to Broken.
//! @param aSimTime The current simulation time.
//! @note  This method should not be invoked directly by application code.
// virtual
void WsfWeapon::PartBroken(double aSimTime)
{
   WsfPlatformPart::PartBroken(aSimTime);
   WsfObserver::WeaponKilled(GetSimulation())(aSimTime, this);
   DetermineFirepowerStatus(*GetPlatform());
}

// ================================================================================================
// virtual
void WsfWeapon::Restore(double aSimTime)
{
   WsfPlatformPart::Restore(aSimTime);
   DetermineFirepowerStatus(*GetPlatform());
}

// ================================================================================================
//! Return the number of receivers associated with the weapon.
//!
//! @return The number of receivers associated with the weapon.  This can be zero.
// virtual
size_t WsfWeapon::GetEM_RcvrCount() const
{
   return 0;
}

// ================================================================================================
//! Get the attributes for the receiver with a specified index.
//!
//! @param aIndex The index of the receiver whose attributes are being requested.
//! This must be in the range [0, GetEM_RcvrCount() - 1].
//!
//! @note It is an error to call this method if the weapon does not have any receivers
//! (i.e.: GetEM_RcvrCount() == 0).
// virtual
WsfEM_Rcvr& WsfWeapon::GetEM_Rcvr(size_t aIndex)
{
   // This method should never be called.  But if it is, just return a dummy.
   static WsfEM_Rcvr dummyRcvr(WsfEM_Rcvr::cRF_INTERFERER);
   return dummyRcvr;
}

// ================================================================================================
//! Return the number of transmitters associated with the weapon.
//!
//! @return The number of transmitters associated with the weapon.  This can be zero.
// virtual
size_t WsfWeapon::GetEM_XmtrCount() const
{
   return 0;
}

// ================================================================================================
//! Get the attributes for the transmitter with a specified index.
//!
//! @param aIndex The index of the transmitter whose attributes are being requested.
//! This must be in the range [0, GetEM_XmtrCount() - 1].
//!
//! @note It is an error to call this method if the weapon does not have any transmitters
//! (i.e.: GetEM_XmtrCount() == 0).
// virtual
WsfEM_Xmtr& WsfWeapon::GetEM_Xmtr(size_t aIndex)
{
   // This method should never be called.  But if it is, just return a dummy.
   static WsfEM_Xmtr dummyXmtr(WsfEM_Xmtr::cXF_INTERFERER);
   return dummyXmtr;
}

// ================================================================================================
//! Return the number of transmitters associated with the weapon.
//!
//! @param aModeIndex The index of the mode containing the transmitter.
//! @return The number of transmitters associated with the weapon.  This can be zero.
// virtual
size_t WsfWeapon::GetEM_XmtrCount(size_t aModeIndex) const
{
   return 0;
}

// ================================================================================================
//! Get the attributes for the transmitter with a specified index.
//!
//! @param aModeIndex The index of the mode containing the transmitter.
//! @param aIndex     The index of the transmitter whose attributes are being requested.
//! This must be in the range [0, GetEM_XmtrCount() - 1].
//!
//! @note It is an error to call this method if the weapon does not have any transmitters
//! (i.e.: GetEM_XmtrCount() == 0).
// virtual
WsfEM_Xmtr& WsfWeapon::GetEM_Xmtr(size_t aModeIndex, size_t aIndex)
{
   // This method should never be called.  But if it is, just return a dummy.
   static WsfEM_Xmtr dummyXmtr(WsfEM_Xmtr::cXF_INTERFERER);
   return dummyXmtr;
}

// ================================================================================================
//! Get the attributes for the transmitter on the specified beam and spot index.
//!
//! @param aModeIndex The index of the mode containing the beam with the transmitter.
//! @param aBeamIndex The index of the beam containing the transmitter.
//! @param aSpotIndex The index of the transmitter whose attributes are being requested.
//! This must be in the range [0, GetEM_XmtrCount() - 1] for the specified beam.
//!
//! @note It is an error to call this method if the weapon does not have any transmitters
//! (i.e.: GetEM_XmtrCount() == 0).
// virtual
WsfEM_Xmtr& WsfWeapon::GetEM_Xmtr(size_t aModeIndex, size_t aBeamIndex, size_t aSpotIndex)
{
   // This method should never be called.  But if it is, just return a dummy.
   static WsfEM_Xmtr dummyXmtr(WsfEM_Xmtr::cXF_INTERFERER);
   return dummyXmtr;
}

// ================================================================================================
//! Return the string ID of the string associated with the name of the current mode.
// virtual
WsfStringId WsfWeapon::GetCurrentModeName() const
{
   WsfStringId nameId = WsfModeList::GetDefaultModeNameId();
   if (mModeListPtr != nullptr)
   {
      nameId = mModeListPtr->GetModeNameId(mModeListPtr->GetCurrentMode());
   }
   return nameId;
}

// ================================================================================================
// virtual
WsfWeapon::WsfWeaponMode* WsfWeapon::GetCurrentMode()
{
   WsfWeaponMode* modePtr = nullptr;
   if (mModeListPtr != nullptr)
   {
      modePtr = GetModeEntry(mModeListPtr->GetCurrentMode());
   }
   return modePtr;
}

// ================================================================================================
//! Get the number of modes associated with the weapon.
// virtual
size_t WsfWeapon::GetModeCount()
{
   size_t count = 1;
   if (mModeListPtr != nullptr)
   {
      count = mModeListPtr->GetModeCount();
   }
   return count;
}

// ================================================================================================
// virtual
WsfWeapon::WsfWeaponMode* WsfWeapon::GetModeEntry(size_t aModeIndex)
{
   WsfWeaponMode* modePtr = nullptr;
   if (aModeIndex < mWeaponModeList.size())
   {
      modePtr = mWeaponModeList[aModeIndex];
   }
   return modePtr;
}

// ================================================================================================
//! Return the index of the mode with the specified name (ID).
//! @param aModeNameId The string ID of the name whose index is requested.
//! If this value is zero then 0 will be returned.
//! @returns If 0 was specified for aModeNameId then 0 will be returned.
//! If the mode exists the value will be the index of the mode in the
//! range 0 <= index < GetModeCount(). If the mode does not exist then the value
//! of GetModeCount() will be returned.
// virtual
size_t WsfWeapon::GetModeIndex(WsfStringId aModeNameId)
{
   size_t modeIndex = 0;
   if (aModeNameId != 0)
   {
      modeIndex = mModeListPtr->GetModeByName(aModeNameId);
   }
   return modeIndex;
}

// ================================================================================================
//! Return the string ID of the string associated with the name of the mode with the specified index.
//! @param aModeIndex The index of the desired mode such that 0 <= aModeIndex < GetModeCount().
// virtual
WsfStringId WsfWeapon::GetModeName(size_t aModeIndex)
{
   WsfStringId nameId = WsfModeList::GetDefaultModeNameId();
   if (((mModeListPtr) != nullptr) && (aModeIndex < mModeListPtr->GetModeCount()))
   {
      nameId = mModeListPtr->GetModeNameId(aModeIndex);
   }
   return nameId;
}

// ================================================================================================
//! Deselect the mode given the string ID of the name of the desired mode.
//! @param aSimTime    The current simulation time.
//! @param aModeNameId The string ID of the mode to be deselected.
//! @note This method does nothing unless the weapon is a multi-mode weapon.
//! Deselecting a mode with a single-mode weapon is accomplished by selecting a new mode.
// virtual
void WsfWeapon::DeselectMode(double aSimTime, WsfStringId aModeNameId)
{
   if (((mModeListPtr) != nullptr) && mModeListPtr->CanMultiSelect())
   {
      mModeListPtr->Deselect(aSimTime, aModeNameId);
      size_t modeIndex = mModeListPtr->GetModeByName(aModeNameId);
      if (modeIndex < mModeListPtr->GetModeCount())
      {
         WsfObserver::WeaponModeDeactivated(GetSimulation())(aSimTime, this, mWeaponModeList[modeIndex]);
      }
   }
}

// ================================================================================================
//! Set the weapon mode given the string ID of the name of the desired mode.
//! @param aSimTime    The current simulation time.
//! @param aModeNameId The string ID of the desired mode.
// virtual
void WsfWeapon::SelectMode(double aSimTime, WsfStringId aModeNameId)
{
   if (mModeListPtr != nullptr)
   {
      mModeListPtr->Select(aSimTime, aModeNameId);
      size_t modeIndex = mModeListPtr->GetModeByName(aModeNameId);
      if (modeIndex < mModeListPtr->GetModeCount())
      {
         WsfObserver::WeaponModeActivated(GetSimulation())(aSimTime, this, mWeaponModeList[modeIndex]);
      }
   }
}

// ================================================================================================
//! Set the launch computer pointer.
//! @param aLaunchComputerPtr pointer to the aggregated instance.
void WsfWeapon::SetLaunchComputer(WsfLaunchComputer* aLaunchComputerPtr)
{
   delete mLaunchComputerPtr;
   mLaunchComputerPtr = aLaunchComputerPtr;
}

// ================================================================================================
//! Cue the weapon to a target.
//! @param aSimTime  The current simulation time.
//! @param aTrackPtr The track that represents the target.
//!        If aTrackPtr is 0, will attempt to use WsfPlatform::GetCurrentTarget().
// virtual
void WsfWeapon::CueToTarget(double aSimTime, const WsfTrack* aTrackPtr)
{
   // Cue to the target only if the weapon can slew.
   if (GetSlewMode() != cSLEW_FIXED)
   {
      // If a track wasn't specified then use the current target.
      const WsfTrack* trackPtr = aTrackPtr;
      if (trackPtr == nullptr)
      {
         trackPtr = GetPlatform()->GetTrackManager().GetCurrentTarget();
      }

      if (trackPtr != nullptr)
      {
         double             timeToIntercept = 0.0;
         bool               cueLocIsValid   = false;
         WsfLaunchComputer* computerPtr     = GetLaunchComputer();
         if (mCueToPredictedIntercept && ((computerPtr) != nullptr))
         {
            if (!computerPtr->HaveResultsFor(aSimTime, trackPtr->GetTrackId(), timeToIntercept))
            {
               timeToIntercept = computerPtr->EstimatedTimeToIntercept(aSimTime, *trackPtr, GetFiringDelay());
            }
            if (timeToIntercept < WsfLaunchComputer::cFOREVER)
            {
               if (computerPtr->LauncherBearingIsValid() || computerPtr->LauncherElevationIsValid())
               {
                  // At least one of the absolute launcher angles was specified. Use the specified angle(s), assuming an
                  // angle of of zero if an angle is missing.
                  double bearing   = computerPtr->LauncherBearingIsValid() ? computerPtr->GetLauncherBearing() : 0.0;
                  double elevation = computerPtr->LauncherElevationIsValid() ? computerPtr->GetLauncherElevation() : 0.0;
                  double cueLocNE  = 100000.0 * cos(elevation);
                  double cueLocNED[3] = {cueLocNE * cos(bearing), cueLocNE * sin(bearing), -cueLocNE * sin(elevation)};
                  double cueLocWCS[3];
                  GetPlatform()->ConvertNEDToWCS(cueLocNED, cueLocWCS);
                  SetCuedLocationWCS(cueLocWCS);
                  cueLocIsValid = true;
               }
               else if (computerPtr->InterceptPointIsValid())
               {
                  // Use the launch computers predicted intercept location if provided.
                  double cueLocWCS[3];
                  computerPtr->GetInterceptPoint().GetLocationWCS(cueLocWCS);
                  SetCuedLocationWCS(cueLocWCS);
                  cueLocIsValid = true;
               }
            }
            else
            {
               // Launch computer did not return a solution. Simply cue to the current track location.
               timeToIntercept = 0.0;
            }
         }

         // If the launch computer did not provide an intercept location then derive one by extrapolation.
         if (!cueLocIsValid)
         {
            double cueLocWCS[3];
            if (GetTargetLocationWCS(aSimTime + timeToIntercept, trackPtr, cueLocWCS))
            {
               SetCuedLocationWCS(cueLocWCS);
            }
         }
      }
   }
}

// ================================================================================================
//! Attempt to fire a weapon.
//! WsfPlatform::GetCurrentTarget() will be used as the target if it is defined.
//! @param aSimTime  The current simulation time.
//! @param aTarget   The target of the weapon.
//! @param aSettings Options for firing the weapon.
//!
//! @note This is the only virtual Fire() method, it should be implemented by derived classes.
// virtual
WsfWeapon::FireResult WsfWeapon::Fire(double aSimTime, const FireTarget& aTarget, const FireOptions& aSettings)
{
   return WsfWeapon::FireResult();
}

// ================================================================================================
//! Fire a salvo at the specified target.
//!
//! This method is intended to be used to fire a salvo of one or more discrete rounds subject to the firing_delay
//! and salvo_delay values. It is not intended continuous weapons like lasers.
//!
//! This method only schedules events which will call Fire() at the appropriate times. Even if all the delays are
//! zero, the Fire() for the first round will not be called until the next call to the event dispatcher.
//!
//! @param aSimTime  The current simulation time.
//! @param aTarget   The target of the weapon.
//! @param aSettings Options for firing the weapon.
//! @return 'true' if successfully initiated, 'false' otherwise (salvo already in progress).
// virtual
bool WsfWeapon::FireSalvo(double aSimTime, const FireTarget& aTarget, const SalvoOptions& aSettings)
{
   if ((aSettings.mQuantity <= 0) ||                         // Requested number of weapons to fire is zero or less
       (mSalvoRequestList.size() >= mMaximumRequestCount) || // Already have the maximum number of salvos active
       (mIsReloading && mInhibitWhileReloading))             // Reloading and can't shoot while reloading
   {
      // No observers are called if the request cannot be initiated (it originally called both WeaponFireRequested and
      // WeaponFireAborted, but it caused some issues with the task manager trying to maintain weapon counts.
      // It is up to the caller to do what it wants...
      return false;
   }

   // Add to the list of active requests.
   ++mSalvoRequestId;
   mSalvoRequestList.push_back(ut::make_unique<SalvoRequest>(mSalvoRequestId, aTarget, aSettings));

   // Determine the time when the first shot should actually be fired.
   // This accounts for the necessary time between firing requests and the time it takes to fire the first shot.

   mQuantityCommitted += aSettings.mQuantity; // Indicate the quantity committed but not yet fired.
   double firingTime = std::max(aSimTime, GetTimeLastFired() + std::max(0.0, GetFiringInterval()));
   firingTime += std::max(0.0, GetFiringDelay());
   GetSimulation()->AddEvent(ut::make_unique<SalvoEvent>(firingTime, this, mSalvoRequestId));
   WsfObserver::WeaponFireRequested(GetSimulation())(aSimTime, this, aTarget.mTrackPtr, aSettings.mQuantity);

   // update status
   auto it = mWeaponStatusMap.find(aTarget.mTrackId);
   if (it == mWeaponStatusMap.end())
   {
      // create map entry if doesn't exist yet
      it = mWeaponStatusMap.insert(WeaponStatusVal(aTarget.mTrackId, WeaponStatus())).first;
   }
   it->second.mTimeWeaponLastFired = aSimTime;
   it->second.mTotalSalvosFired += 1;
   it->second.mRoundsPending += std::max(0, aSettings.mQuantity);

   return true;
}

// ================================================================================================
//! Fire the next round of the current salvo.
//! This is an internal method that is invoked via SalvoEvent.
// virtual
void WsfWeapon::ContinueSalvo(double aSimTime, unsigned int aRequestId)
{
   // Find the original request.
   auto rli = mSalvoRequestList.begin();
   while (rli != mSalvoRequestList.end())
   {
      if ((*rli)->mRequestId == aRequestId)
      {
         break;
      }
      ++rli;
   }

   if (rli != mSalvoRequestList.end())
   {
      SalvoRequest*   requestPtr    = rli->get();
      bool            salvoComplete = true;
      int&            quantity      = requestPtr->mSettings.mQuantity;
      const WsfTrack* trackPtr      = requestPtr->mTarget.mTrackPtr;
      mQuantityCommitted            = std::max(0.0, mQuantityCommitted - quantity);
      if (mIsReloading && mInhibitWhileReloading)
      {
         // Reload in progress and firing while reloading is not allowed...
         WeaponFireAborted(aSimTime, trackPtr, quantity);
         WsfObserver::WeaponFireAborted(GetSimulation())(aSimTime, this, trackPtr, quantity);
      }
      else
      {
         if (mAutomaticTargetCueing)
         {
            CueToTarget(aSimTime, trackPtr);
         }
         if (Fire(aSimTime, requestPtr->mTarget, requestPtr->mSettings))
         {
            // It is possible through observer callbacks and such that the request may have been deleted.
            // Therefore we should look it up again...
            for (rli = mSalvoRequestList.begin(); rli != mSalvoRequestList.end(); ++rli)
            {
               if ((*rli)->mRequestId == aRequestId)
               {
                  break;
               }
            }
            if (rli != mSalvoRequestList.end())
            {
               requestPtr = rli->get();
               --(quantity);
               if (quantity > 0)
               {
                  // Continue with the salvo.
                  mQuantityCommitted += quantity;
                  double salvoInterval = requestPtr->mSettings.mSalvoInterval;
                  if (salvoInterval < 0.0)
                  {
                     salvoInterval = GetSalvoInterval();
                  }
                  double firingTime = std::max(aSimTime + std::max(0.0, salvoInterval),
                                               GetTimeLastFired() + std::max(0.0, GetFiringInterval()));
                  GetSimulation()->AddEvent(ut::make_unique<SalvoEvent>(firingTime, this, aRequestId));
                  WsfObserver::WeaponFireRequested(GetSimulation())(aSimTime, this, trackPtr, quantity);
                  salvoComplete = false;
               }
               ProcessReloading(aSimTime);
            }
         }
         else
         {
            WeaponFireAborted(aSimTime, trackPtr, quantity);
            WsfObserver::WeaponFireAborted(GetSimulation())(aSimTime, this, trackPtr, quantity);
         }
      }
      if (salvoComplete && (rli != mSalvoRequestList.end()))
      {
         mSalvoRequestList.erase(rli);
      }
   }
}

// ================================================================================================
//! Abort any pending rounds from a salvo.
//! @param aSimTime The current simulation time.
//! @param aTrackId The track ID of the track that was passed to the FireSalvo request whose salvo is
//!                 to be aborted. If the track ID is Null() then all pending salvos will be aborted.
//! @note This only aborts rounds that have not yet been fired.
// virtual
void WsfWeapon::AbortSalvo(double aSimTime, const WsfTrackId& aTrackId)
{
   for (auto iter = std::begin(mSalvoRequestList); iter != std::end(mSalvoRequestList);)
   {
      if (aTrackId.IsNull() || (aTrackId == (*iter)->mTarget.mTrackId))
      {
         int& quantity      = (*iter)->mSettings.mQuantity;
         mQuantityCommitted = std::max(0.0, mQuantityCommitted - quantity);
         WeaponFireAborted(aSimTime, (*iter)->mTarget.mTrackPtr, quantity);
         WsfObserver::WeaponFireAborted(GetSimulation())(aSimTime, this, (*iter)->mTarget.mTrackPtr, quantity);
         iter = mSalvoRequestList.erase(iter);
      }
      else
      {
         ++iter;
      }
   }
}

// ================================================================================================
// virtual
void WsfWeapon::CeaseFire(double aSimTime)
{
   // Cancel any active salvo requests
   AbortSalvo(aSimTime, WsfTrackId());
}

// ================================================================================================
//! Return the quantity (rounds, bombs, units) remaining.
//!
//! This includes the number of units that have been committed via 'FireSalvo'.
//!
//! @note The quantity is expressed as real value rather than an integer value to provide for
//! weapons that expend themselves in a continuous manner rather than a discrete manner). For
//! example, "quantity" might be a unitless discrete number representing the number of JDAM
//! remaining, or it could mean the (more or less continuous) number of joules available to
//! a high-energy laser (HEL).  The derived class should maintain the quantity as desired and
//! implement this method to return the current remaining quantity.
// virtual
double WsfWeapon::GetQuantityRemaining() const
{
   return std::max(0.0, mQuantityRemaining - mQuantityCommitted);
}

// ================================================================================================
//! Return the quantity that is currently committed.
// virtual
double WsfWeapon::GetQuantityCommitted() const
{
   return mQuantityCommitted;
}

// ================================================================================================
//! Return the maximum quantity that may be 'loaded'.
// virtual
double WsfWeapon::GetMaximumQuantity() const
{
   return mMaximumQuantity;
}

// ================================================================================================
//! Return the cumulative running total of the quantity used (fired).
// virtual
double WsfWeapon::GetTotalQuantityUsed() const
{
   return mTotalQuantityUsed;
}

// ================================================================================================
//! Update the quantity remaining.
//! This method can be used to 'reload' the weapon.
//! @param aQuantity The number of units that will be available for firing.
//! This value will be silently clamped to the range [0.0, GetMaximumQuantity()].
// virtual
void WsfWeapon::SetQuantityRemaining(double aQuantity)
{
   mQuantityRemaining = UtMath::Limit(aQuantity, 0.0, GetMaximumQuantity());

   if (GetSimulation() != nullptr)
   {
      WsfObserver::WeaponQuantityChanged(GetSimulation())(GetSimulation()->GetSimTime(), this);
   }
}

// ================================================================================================
//! Change the quantity of the weapon resource that has been used and is remaining by a delta amount.
//! @note Negative delta values decrement the quantity remaining, while positive values increment
//!       the quantity remaining. The total quantity used will also be modified inversely.
//! @param aDeltaQuantity The change in quantity remaining and (inversely) total quantity used.
void WsfWeapon::ChangeQuantity(double aDeltaQuantity)
{
   double prevQuantityRemaining = mQuantityRemaining;
   SetQuantityRemaining(mQuantityRemaining + aDeltaQuantity);
   mTotalQuantityUsed += (prevQuantityRemaining - mQuantityRemaining);
}

// ================================================================================================
//! Utility method called by the nested CompleteReloadEvent class to trigger the completion
//! of a reload.  (This method has to be made public, but is not to be called directly.)
// public virtual
void WsfWeapon::CompleteReloading(double aSimTime, double aQuantity)
{
   mQuantityRemaining += aQuantity;
   mQuantityRemaining = std::min(mMaximumQuantity, mQuantityRemaining);
   mIsReloading       = false;
   WsfObserver::WeaponReloadEnded(GetSimulation())(aSimTime, this);
   WsfObserver::WeaponQuantityChanged(GetSimulation())(aSimTime, this);
}

// ================================================================================================
//! Attempt to create a target track for hand-off to the weapon.
//!
//! This method attempts to create a track to hand off to the weapon. If a track has been provided
//! and it has a valid location then a clone of the track will be returned. If the track does not
//! have a valid location or if no track was provided AND if the user defined an 'unknown_target_range'
//! then a new track will be created that has a location that is at the indicated range directly
//! in front of the launching platform.
//!
//! @param aSimTime The current simulation time.
//! @param aTrackPtr Pointer to the source track (may be 0).
//! @returns A pointer to the newly created hand-off track. This may be zero!
WsfTrack* WsfWeapon::CreateTargetTrack(double aSimTime, const WsfTrack* aTrackPtr)
{
   double    locWCS[3];
   WsfTrack* trackPtr = nullptr;
   if (aTrackPtr != nullptr)
   {
      // A track was provided. Clone the track. If the track does not result in a valid location
      // and an unknown target range was provide, update the cloned track with the generated location.
      // If the location is valid, update the location to the current time.
      trackPtr              = aTrackPtr->Clone();
      bool hasValidLocation = trackPtr->GetExtrapolatedLocationWCS(aSimTime, locWCS);
      if (hasValidLocation || (mUnknownTargetRange > 0.0))
      {
         if (!hasValidLocation) // and the unknown target range was specified
         {
            // Attempt to get a valid location using this range, which is used GetTargetLocationWCS.
            GetTargetLocationWCS(aSimTime, aTrackPtr, locWCS);

            // Set other flags and values to reasonable defaults.
            trackPtr->Set3D(true);
            trackPtr->SetRangeValid(false);
            trackPtr->SetBearingValid(false);
            trackPtr->SetElevationValid(false);
            trackPtr->SetRangeRateValid(false);
            trackPtr->SetRangeError(0.0);
            trackPtr->SetBearingError(0.0);
            trackPtr->SetElevationError(0.0);
            trackPtr->SetRangeError(0.0);
            trackPtr->SetVelocityValid(false);
         }
         trackPtr->SetUpdateTime(aSimTime);
         trackPtr->SetLocationWCS(locWCS);
         trackPtr->SetWCS_LocationValid(true);
      }
   }
   else if (mUnknownTargetRange > 0.0)
   {
      // No target track was provided, but an unknown target range was provided.
      // Create a track with the generated location.
      trackPtr = new WsfTrack();
      trackPtr->SetPlatformOriginator(GetPlatform());
      trackPtr->SetTrackType(WsfTrack::cPROCESSED);
      trackPtr->SetFusible(true);
      trackPtr->Initialize(aSimTime, GetPlatform()->GetNextTrackId(), *GetSimulation());
      GetPlatform()->GetLocationWCS(locWCS);
      trackPtr->SetOriginatorLocationWCS(locWCS);
      trackPtr->Update(aSimTime);

      GetTargetLocationWCS(aSimTime, nullptr, locWCS);
      trackPtr->SetLocationWCS(locWCS);
      trackPtr->SetWCS_LocationValid(true);
      trackPtr->Set3D(true);
   }
   return trackPtr;
}

// ================================================================================================
//! Return the target location from the track.
//! @param aSimTime  The current simulation time.
//! @param aTrackPtr The pointer to the track from which the location is to be extracted (may be 0)
//! @param aTargetLocWCS The resulting target location. This will be valid only if the return value
//! from this method is 'true'.
//! @returns 'true' if 'aTargetLocWCS' contains valid data.
bool WsfWeapon::GetTargetLocationWCS(double aSimTime, const WsfTrack* aTrackPtr, double aTargetLocWCS[3])
{
   bool   locationValid = false;
   bool   bearingValid  = true;
   double bearing;
   double dummy;
   GetPlatform()->GetOrientationNED(bearing, dummy, dummy);

   // If a track has been provided, see if it has a valid location.

   if (aTrackPtr != nullptr)
   {
      locationValid = aTrackPtr->GetExtrapolatedLocationWCS(aSimTime, aTargetLocWCS);
      if (!locationValid)
      {
         if (aTrackPtr->BearingValid())
         {
            bearing      = aTrackPtr->GetBearing();
            bearingValid = true;
         }
      }
   }

   // If the location was not available from the track and 'unknown_target_range' has been declared
   // create a location at the specified range directly in front of the launching platform.

   if ((!locationValid) && bearingValid && (mUnknownTargetRange > 0.0))
   {
      double wpnLat;
      double wpnLon;
      double wpnAlt;
      GetPlatform()->GetLocationLLA(wpnLat, wpnLon, wpnAlt);
      double tgtLat;
      double tgtLon;
      UtSphericalEarth::ExtrapolateGreatCirclePosition(wpnLat,
                                                       wpnLon,
                                                       bearing * UtMath::cDEG_PER_RAD,
                                                       mUnknownTargetRange,
                                                       tgtLat,
                                                       tgtLon);

      // Select the target altitude.
      double tgtAlt = 0.0;
      if (mUnknownTargetAltitudeRef == cAR_AGL)
      {
         wsf::Terrain terrain(GetSimulation()->GetTerrainInterface());
         float        terrAltMSL;
         terrain.GetElevInterp(tgtLat, tgtLon, terrAltMSL);
         tgtAlt = terrAltMSL + mUnknownTargetAltitude;
      }
      else if (mUnknownTargetAltitudeRef == cAR_MSL)
      {
         tgtAlt = mUnknownTargetAltitude;
      }
      else if (mUnknownTargetAltitudeRef == cAR_RELATIVE)
      {
         tgtAlt = wpnAlt + mUnknownTargetAltitude;
      }
      UtEntity::ConvertLLAToWCS(tgtLat, tgtLon, tgtAlt, aTargetLocWCS);
      locationValid = true;
   }
   return locationValid;
}

// ================================================================================================
// virtual
bool WsfWeapon::WithinFieldOfView(double aSimTime, WsfPlatform* aPlatformPtr)
{
   bool withinFieldOfView(false);

   // Update the location of the platform
   aPlatformPtr->Update(aSimTime);

   size_t rcvrCount = GetEM_RcvrCount();
   for (size_t rcvrIndex = 0; rcvrIndex < rcvrCount; ++rcvrIndex)
   {
      WsfEM_Antenna* antennaPtr = GetEM_Rcvr(rcvrIndex).GetAntenna();
      if (antennaPtr != nullptr)
      {
         double trueThisToTgtAz(0.0);
         double trueThisToTgtEl(0.0);
         double apparentThisToTgtLocWCS[3] = {0.0};
         double apparentThisToTgtAz(0.0);
         double apparentThisToTgtEl(0.0);
         double apparentTgtToThisLocWCS[3] = {0.0};

         if (antennaPtr->WithinFieldOfView(aPlatformPtr,
                                           1.0,
                                           trueThisToTgtAz,
                                           trueThisToTgtEl,
                                           apparentThisToTgtLocWCS,
                                           apparentThisToTgtAz,
                                           apparentThisToTgtEl,
                                           apparentTgtToThisLocWCS))
         {
            withinFieldOfView = true;
            break;
         }
      }
   }
   return withinFieldOfView;
}

// ================================================================================================
// virtual
bool WsfWeapon::WithinFieldOfView(double aSimTime, WsfTrack* aTrackPtr)
{
   bool withinFieldOfView(false);

   size_t rcvrCount = GetEM_RcvrCount();
   for (size_t rcvrIndex = 0; rcvrIndex < rcvrCount; ++rcvrIndex)
   {
      WsfEM_Antenna* antennaPtr = GetEM_Rcvr(rcvrIndex).GetAntenna();
      if (antennaPtr != nullptr)
      {
         // WithinFieldOfView output
         double trueThisToTgtAz(0.0);
         double trueThisToTgtEl(0.0);
         double apparentThisToTgtLocWCS[3] = {0.0};
         double apparentThisToTgtAz(0.0);
         double apparentThisToTgtEl(0.0);
         double apparentTgtToThisLocWCS[3] = {0.0};

         // WithinFieldOfView input
         double tgtLocWCS[3] = {0.0};
         aTrackPtr->GetExtrapolatedLocationWCS(aSimTime, tgtLocWCS);

         if (antennaPtr->WithinFieldOfView(tgtLocWCS,
                                           1.0,
                                           trueThisToTgtAz,
                                           trueThisToTgtEl,
                                           apparentThisToTgtLocWCS,
                                           apparentThisToTgtAz,
                                           apparentThisToTgtEl,
                                           apparentTgtToThisLocWCS))
         {
            withinFieldOfView = true;
            break;
         }
      }
   }
   return withinFieldOfView;
}

// =================================================================================================
//! Method used by the weapon engagement when the weapon platform terminates or the engagement is
//! about to be marked "complete" (which is not the same as "ended").
//! This is not an observer callback but is used to update weapon and engagement status prior to
//! the WsfObserver::WeaponTerminated call.
void WsfWeapon::EngagementComplete(double aSimTime, WsfWeaponEngagement* aEngagementPtr)
{
   const WsfTrackId& targetTrackId = aEngagementPtr->GetInitialTargetTrackId();
   if (targetTrackId.IsNull())
   {
      return;
   }

   auto it = mWeaponStatusMap.find(targetTrackId);
   if (it != mWeaponStatusMap.end())
   {
      // mRoundsActive == mActiveWeaponList.size()
      if (it->second.mRoundsActive > 0)
      {
         it->second.mTimeWeaponLastTerminated = aSimTime;
         it->second.mRoundsActive--;
         it->second.mRoundsCompleted++;
      }
      // erase weapon platform index from active list
      auto at = it->second.mActiveWeaponList.begin();
      for (; at != it->second.mActiveWeaponList.end(); ++at)
      {
         if (at->mSerialNumber == aEngagementPtr->GetSerialNumber())
         {
            // weapon no longer active, remove it ?
            it->second.mActiveWeaponList.erase(at);
            break;
         }
      }
   }
}

// =================================================================================================
//! Return the simulation time of the most recent weapon firing against the specified track.
double WsfWeapon::TimeWeaponLastFiredFor(const WsfTrackId& aTrackId) const
{
   double simTime = -1.0;
   // just use the weapons status for the track
   auto it = mWeaponStatusMap.find(aTrackId);
   if (it != mWeaponStatusMap.end())
   {
      simTime = std::max(simTime, it->second.mTimeWeaponLastFired);
   }
   return simTime;
}

// =================================================================================================
//! Return the simulation time of the most recent weapon termination against the specified track.
double WsfWeapon::TimeWeaponLastTerminatedFor(const WsfTrackId& aTrackId) const
{
   double simTime = -1.0;
   // just use the weapons status for the track
   auto it = mWeaponStatusMap.find(aTrackId);
   if (it != mWeaponStatusMap.end())
   {
      simTime = std::max(simTime, it->second.mTimeWeaponLastTerminated);
   }
   return simTime;
}

// =================================================================================================
//! Return the count of the number of weapons that are pending against the specified track.
//! @param aTrackId The local track ID of the track of interest.
int WsfWeapon::WeaponsPendingFor(const WsfTrackId& aTrackId) const
{
   int weaponCount = 0;
   if (aTrackId.IsNull())
   {
      // count all of the fired/pending weapons (fired at any track)
      auto it = mWeaponStatusMap.begin();
      for (; it != mWeaponStatusMap.end(); ++it)
      {
         weaponCount += std::max(it->second.mRoundsPending, 0);
      }
   }
   else
   {
      // just count the weapons fired/pending at the track
      auto it = mWeaponStatusMap.find(aTrackId);
      if (it != mWeaponStatusMap.end())
      {
         weaponCount += std::max(it->second.mRoundsPending, 0);
      }
   }
   return weaponCount;
}

// =================================================================================================
//! Return the count of the number of weapons that are active against the specified track.
//! @param aTrackId The local track ID of the track of interest.
int WsfWeapon::WeaponsActiveFor(const WsfTrackId& aTrackId) const
{
   int weaponCount = 0;
   if (aTrackId.IsNull())
   {
      // count all of the fired/pending weapons (fired at any track)
      auto it = mWeaponStatusMap.begin();
      for (; it != mWeaponStatusMap.end(); ++it)
      {
         weaponCount += std::max(it->second.mRoundsActive, 0);
      }
   }
   else
   {
      // just count the weapons fired/pending at the track
      auto it = mWeaponStatusMap.find(aTrackId);
      if (it != mWeaponStatusMap.end())
      {
         weaponCount += std::max(it->second.mRoundsActive, 0);
      }
   }
   return weaponCount;
}

// =================================================================================================
//! Returns number of weapons that were fired at the track and are now terminated
int WsfWeapon::RoundsCompleteFor(const WsfTrackId& aTrackId) const
{
   int roundsComplete = 0;
   if (aTrackId.IsNull())
   {
      // add in all of the rounds completed (fired at any track)
      auto it = mWeaponStatusMap.begin();
      for (; it != mWeaponStatusMap.end(); ++it)
      {
         roundsComplete += it->second.mRoundsCompleted;
      }
   }
   else
   {
      // add int just the rounds completed for the track
      auto it = mWeaponStatusMap.find(aTrackId);
      if (it != mWeaponStatusMap.end())
      {
         roundsComplete += it->second.mRoundsCompleted;
      }
   }
   return roundsComplete;
}

// =================================================================================================
//! Return the count of the number of weapons that are active & pending against the specified track.
//! @param aTrackId The local track ID of the track of interest.
int WsfWeapon::WeaponsActiveAndPendingFor(const WsfTrackId& aTrackId) const
{
   return WeaponsPendingFor(aTrackId) + WeaponsActiveFor(aTrackId);
}

// =================================================================================================
//! Return the count of the number of rounds that have been fired at the specified track .
//! @param aTrackId The local track ID of the track of interest.
int WsfWeapon::RoundsFiredAt(const WsfTrackId& aTrackId) const
{
   return WeaponsActiveFor(aTrackId) + RoundsCompleteFor(aTrackId);
}

// =================================================================================================
//! Return the count of the number of salvos that have been fired at the specified track.
//! @param aTrackId The local track ID of the track of interest.
int WsfWeapon::SalvosFiredAt(const WsfTrackId& aTrackId) const
{
   int salvosFired = 0;
   if (aTrackId.IsNull())
   {
      // count all of the salvos (fired at any track)
      auto it = mWeaponStatusMap.begin();
      for (; it != mWeaponStatusMap.end(); ++it)
      {
         salvosFired += it->second.mTotalSalvosFired;
      }
   }
   else
   {
      // just count the salvos fired at the track
      auto it = mWeaponStatusMap.find(aTrackId);
      if (it != mWeaponStatusMap.end())
      {
         salvosFired += it->second.mTotalSalvosFired;
      }
   }
   return salvosFired;
}

// =================================================================================================
//! Returns all platform indices for weapon platforms that were fired from this platform and...
//! ...are still active against the track
std::vector<size_t> WsfWeapon::ActiveWeaponPlatformsFor(const WsfTrackId& aTrackId) const
{
   std::vector<size_t> weaponPlatforms;
   if (aTrackId.IsNull())
   {
      // add in all active weapons (fired at any track)
      auto it = mWeaponStatusMap.begin();
      for (; it != mWeaponStatusMap.end(); ++it)
      {
         auto ait = it->second.mActiveWeaponList.begin();
         while (ait != it->second.mActiveWeaponList.end())
         {
            weaponPlatforms.push_back(ait->mPlatformIndex);
            ++ait;
         }
      }
   }
   else
   {
      // just add in the active weapons fired at the track
      auto it = mWeaponStatusMap.find(aTrackId);
      if (it != mWeaponStatusMap.end())
      {
         auto ait = it->second.mActiveWeaponList.begin();
         while (ait != it->second.mActiveWeaponList.end())
         {
            weaponPlatforms.push_back(ait->mPlatformIndex);
            ++ait;
         }
      }
   }
   return weaponPlatforms;
}

// ================================================================================================
//! After a successful weapon Fire(), this method checks to see if the reload threshold
//! has been reached, and will initiate reloading, as needed, and if possible.
// protected virtual
void WsfWeapon::ProcessReloading(double aSimTime)
{
   // NOTE: 'mQuantityRemaining' is used instead of 'GetQuantityRemaining()' because the latter includes
   // weapons that have been committed (via FireSalvo) or 'deferred launched' (see WsfExplicitWeapon)
   // but not yet actually released (and thus, may be canceled without expending the weapon). Reload
   // should not be started until the true count actually gets to reload threshold.

   if ((!mIsReloading) && (GetReloadInventory() > 0.0) && (mQuantityRemaining <= GetReloadThreshold()))
   {
      double reloadAmount = std::min(mReloadIncrement, mReloadInventory);
      reloadAmount        = std::min(reloadAmount, static_cast<double>(mMaximumQuantity - mQuantityRemaining));
      mReloadInventory -= reloadAmount;
      mIsReloading      = true;
      double reloadTime = mReloadTime.Draw();
      GetSimulation()->AddEvent(ut::make_unique<CompleteReloadEvent>(aSimTime + reloadTime, reloadAmount, this));
      WsfObserver::WeaponReloadStarted(GetSimulation())(aSimTime, this);
   }
}

// ================================================================================================
//! Set the pointer to the 'mode list'.
//! @param aModeListPtr Pointer to the new mode list object. The weapon assumes
//! ownership of the object. Any existing mode list will be deleted.
// protected
void WsfWeapon::SetModeList(WsfModeList* aModeListPtr)
{
   delete mModeListPtr;
   mModeListPtr = aModeListPtr;
}

// ================================================================================================
//! Set the time the weapon the was last fired.
//! The 'last firing time' should represent when the weapon was last known to be firing.
//! In general, a derived class should invoke this method in Fire when it fires the weapon.
//! If the weapon is one where a firing event extends over a period of time (such as a laser)
//! then it should also be called again when the firing event stops. The net effect is that
//! GetLastTimeFired() + GetFiringInterval() would be the next time the weapon could be fired.
//! @param aSimTime The current simulation time.
// protected
void WsfWeapon::SetTimeLastFired(double aSimTime)
{
   mTimeLastFired = aSimTime;
}

// ================================================================================================
// virtual
void WsfWeapon::WeaponFireAborted(double aSimTime, const WsfTrack* aTargetTrackPtr, double aQuantity)
{
   if (aTargetTrackPtr == nullptr)
   {
      return;
   }

   auto it = mWeaponStatusMap.find(aTargetTrackPtr->GetTrackId());
   if (it != mWeaponStatusMap.end())
   {
      it->second.mRoundsPending = std::max(0, it->second.mRoundsPending - static_cast<int>(aQuantity));
   }
}

// ================================================================================================
// Weapon status methods.
// ================================================================================================

// =================================================================================================
double WsfWeapon::TimeWeaponLastFiredFor(const WsfPlatform& aPlatform, const WsfTrackId& aTrackId, WsfStringId aWeaponName)
{
   double timeLastFired = -1.0;
   if (aWeaponName == nullptr)
   {
      // Check all weapons
      for (WsfComponentList::RoleIterator<WsfWeapon> weapons(aPlatform); !weapons.AtEnd(); ++weapons)
      {
         timeLastFired = std::max(timeLastFired, weapons->TimeWeaponLastFiredFor(aTrackId));
      }
   }
   else
   {
      // Only check for weapon of given name
      const WsfWeapon* weaponPtr = aPlatform.GetComponent<WsfWeapon>(aWeaponName);
      if (weaponPtr != nullptr)
      {
         timeLastFired = std::max(timeLastFired, weaponPtr->TimeWeaponLastFiredFor(aTrackId));
      }
   }
   return timeLastFired;
}

// =================================================================================================
double WsfWeapon::TimeWeaponLastTerminatedFor(const WsfPlatform& aPlatform, const WsfTrackId& aTrackId, WsfStringId aWeaponName)
{
   double timeLastTerminated = -1.0;
   if (aWeaponName == nullptr)
   {
      // Check all weapons
      for (WsfComponentList::RoleIterator<WsfWeapon> weapons(aPlatform); !weapons.AtEnd(); ++weapons)
      {
         timeLastTerminated = std::max(timeLastTerminated, weapons->TimeWeaponLastTerminatedFor(aTrackId));
      }
   }
   else
   {
      // Only check for weapon of given name
      const WsfWeapon* weaponPtr = aPlatform.GetComponent<WsfWeapon>(aWeaponName);
      if (weaponPtr != nullptr)
      {
         timeLastTerminated = std::max(timeLastTerminated, weaponPtr->TimeWeaponLastTerminatedFor(aTrackId));
      }
   }
   return timeLastTerminated;
}

// =================================================================================================
//! Returns number of weapons that will be but have not yet been fired yet at the track
//! Typically pending for firing delays or salvo intervals
int WsfWeapon::WeaponsPendingFor(const WsfPlatform& aPlatform, const WsfTrackId& aTrackId, WsfStringId aWeaponName)
{
   int activeWeaponCount = 0;
   if (aWeaponName == nullptr)
   {
      // Check all weapons
      for (WsfComponentList::RoleIterator<WsfWeapon> weapons(aPlatform); !weapons.AtEnd(); ++weapons)
      {
         activeWeaponCount += weapons->WeaponsPendingFor(aTrackId);
      }
   }
   else
   {
      // Only check for weapon of given name
      const WsfWeapon* weaponPtr = aPlatform.GetComponent<WsfWeapon>(aWeaponName);
      if (weaponPtr != nullptr)
      {
         activeWeaponCount += weaponPtr->WeaponsPendingFor(aTrackId);
      }
   }
   return activeWeaponCount;
}

// =================================================================================================
//! Returns number of weapons that have been fired at the track and are still alive
int WsfWeapon::WeaponsActiveFor(const WsfPlatform& aPlatform, const WsfTrackId& aTrackId, WsfStringId aWeaponName)
{
   int activeWeaponCount = 0;
   if (aWeaponName == nullptr)
   {
      // Check all weapons
      for (WsfComponentList::RoleIterator<WsfWeapon> weapons(aPlatform); !weapons.AtEnd(); ++weapons)
      {
         activeWeaponCount += weapons->WeaponsActiveFor(aTrackId);
      }
   }
   else
   {
      // Only check for weapon of given name
      const WsfWeapon* weaponPtr = aPlatform.GetComponent<WsfWeapon>(aWeaponName);
      if (weaponPtr != nullptr)
      {
         activeWeaponCount += weaponPtr->WeaponsActiveFor(aTrackId);
      }
   }
   return activeWeaponCount;
}

// =================================================================================================
//! Returns number of weapons that were fired at the track and are now terminated
int WsfWeapon::RoundsCompleteFor(const WsfPlatform& aPlatform, const WsfTrackId& aTrackId, WsfStringId aWeaponName)
{
   int roundsCompleteCount = 0;
   if (aWeaponName == nullptr)
   {
      // Check all weapons
      for (WsfComponentList::RoleIterator<WsfWeapon> weapons(aPlatform); !weapons.AtEnd(); ++weapons)
      {
         roundsCompleteCount += weapons->RoundsCompleteFor(aTrackId);
      }
   }
   else
   {
      // Only check for weapon of given name
      const WsfWeapon* weaponPtr = aPlatform.GetComponent<WsfWeapon>(aWeaponName);
      if (weaponPtr != nullptr)
      {
         roundsCompleteCount += weaponPtr->RoundsCompleteFor(aTrackId);
      }
   }
   return roundsCompleteCount;
}

// =================================================================================================
//! Equivalent to WeaponsActiveFor() plus WeaponsPendingFor()
int WsfWeapon::WeaponsActiveAndPendingFor(const WsfPlatform& aPlatform, const WsfTrackId& aTrackId, WsfStringId aWeaponName)
{
   int activeWeaponCount = 0;
   if (aWeaponName == nullptr)
   {
      // Check all weapons
      for (WsfComponentList::RoleIterator<WsfWeapon> weapons(aPlatform); !weapons.AtEnd(); ++weapons)
      {
         activeWeaponCount += weapons->WeaponsActiveAndPendingFor(aTrackId);
      }
   }
   else
   {
      // Only check for weapon of given name
      const WsfWeapon* weaponPtr = aPlatform.GetComponent<WsfWeapon>(aWeaponName);
      if (weaponPtr != nullptr)
      {
         activeWeaponCount += weaponPtr->WeaponsActiveAndPendingFor(aTrackId);
      }
   }
   return activeWeaponCount;
}

// =================================================================================================
//! Equivalent to WeaponsActiveFor() plus RoundsCompleteFor()
int WsfWeapon::RoundsFiredAt(const WsfPlatform& aPlatform, const WsfTrackId& aTrackId, WsfStringId aWeaponName)
{
   int roundsFiredCount = 0;
   if (aWeaponName == nullptr)
   {
      // Check all weapons
      for (WsfComponentList::RoleIterator<WsfWeapon> weapons(aPlatform); !weapons.AtEnd(); ++weapons)
      {
         roundsFiredCount += weapons->RoundsFiredAt(aTrackId);
      }
   }
   else
   {
      // Only check for weapon of given name
      const WsfWeapon* weaponPtr = aPlatform.GetComponent<WsfWeapon>(aWeaponName);
      if (weaponPtr != nullptr)
      {
         roundsFiredCount += weaponPtr->RoundsFiredAt(aTrackId);
      }
   }
   return roundsFiredCount;
}

// =================================================================================================
//! Returns number of salvo fire requests requested, and not the actual number of salvos fired
int WsfWeapon::SalvosFiredAt(const WsfPlatform& aPlatform, const WsfTrackId& aTrackId, WsfStringId aWeaponName)
{
   int salvosFiredCount = 0;
   if (aWeaponName == nullptr)
   {
      // Check all weapons
      for (WsfComponentList::RoleIterator<WsfWeapon> weapons(aPlatform); !weapons.AtEnd(); ++weapons)
      {
         salvosFiredCount += weapons->SalvosFiredAt(aTrackId);
      }
   }
   else
   {
      // Only check for weapon of given name
      const WsfWeapon* weaponPtr = aPlatform.GetComponent<WsfWeapon>(aWeaponName);
      if (weaponPtr != nullptr)
      {
         salvosFiredCount += weaponPtr->SalvosFiredAt(aTrackId);
      }
   }
   return salvosFiredCount;
}

// =================================================================================================
//! Returns all platform indices for weapon platforms that were fired from this platform and...
//! ...are still active against the track
std::vector<size_t> WsfWeapon::ActiveWeaponPlatformsFor(const WsfPlatform& aPlatform,
                                                        const WsfTrackId&  aTrackId,
                                                        WsfStringId        aWeaponName)
{
   if (aWeaponName == nullptr)
   {
      // Check all weapons
      std::vector<size_t> activeWeapons;
      for (WsfComponentList::RoleIterator<WsfWeapon> weapons(aPlatform); !weapons.AtEnd(); ++weapons)
      {
         std::vector<size_t> temp = weapons->ActiveWeaponPlatformsFor(aTrackId);
         activeWeapons.insert(activeWeapons.end(), temp.begin(), temp.end());
      }
      return activeWeapons;
   }
   else
   {
      // Only check for weapon of given name
      const WsfWeapon* weaponPtr = aPlatform.GetComponent<WsfWeapon>(aWeaponName);
      if (weaponPtr != nullptr)
      {
         return weaponPtr->ActiveWeaponPlatformsFor(aTrackId);
      }
   }
   return std::vector<size_t>();
}

// =================================================================================================
//! Decide whether to set or clear the Firepower appearance bit.
void WsfWeapon::DetermineFirepowerStatus(WsfPlatform& aPlatform)
{
   if (aPlatform.GetComponentCount<WsfWeapon>() == 0)
   {
      return;
   }

   // bool oneWeaponsIsAlive = false;
   // bool allWeaponsAreDead = true;
   bool oneWeaponIsDead = false;

   for (WsfComponentList::RoleIterator<WsfWeapon> iter(aPlatform); !iter.AtEnd(); ++iter)
   {
      if (iter->GetDamageFactor() == 1.0)
      {
         oneWeaponIsDead = true;
      }
      // else
      //{
      //    allWeaponsAreDead = false;
      //    oneWeaponsIsAlive = true;
      // }
   }

   // Note:  When do we decide we are Firepower Killed?
   // A.  When at least one weapon is dead.
   // B.  When all weapons are dead.
   // Here, we arbitrarily choose (A).
   if (oneWeaponIsDead)
   {
      // Set the Firepower appearance bit to "Kill" (without pulling in any DIS header dependencies):
      aPlatform.SetAppearance(2, 1, 1, false);
   }
   else
   {
      // Set the Firepower appearance bit to "No Kill" (without pulling in any DIS header dependencies):
      aPlatform.SetAppearance(2, 1, 0, false);
   }
}

// ================================================================================================
// Weapon server methods
// ================================================================================================

// ================================================================================================
//! Begin the weapon server allocation sequence.
bool WsfWeapon::AllocateTheWeapon(double aSimTime, WsfTrack* aTrackPtr, int aStoreId)
{
   bool ok = false;
   if (mState == cWEAPON_STATE_READY)
   {
      ok = true;
   }
   WsfObserver::WeaponSelected(GetSimulation())(aSimTime, this, aTrackPtr, aStoreId);
   return ok;
}

//! ==============================================================================================
//! Used primarily by the weapon server to set the state of the weapon during allocation and
//! weapon data setup.  A series of two SetData/Data pdus are sent/received, the first is the
//! Allocation SetData Pdu/Data Acknowledge Pdu, the second is the Umbilical Set Data Pdu/Data
//! Acknowledge Pdu.  When the first is sent/received the weapon is put into the cWPN_ALLOCATE
//! state, when the umbilical data is send/received the weapon is then put into the cWPN_READY
//! state, then a fire pdu can be sent when a fire command is issued.  If a NACK was received
//! back in the first Data pdu or any of the acknowledge Data Pdus were not received, then the
//! weapon would not have been allocated.
void WsfWeapon::SetState(WeaponState aState)
{
   mState = aState;
}

// ================================================================================================
// Nested class SalvoRequest
// ================================================================================================
WsfWeapon::SalvoRequest::SalvoRequest(unsigned int aRequestId, const FireTarget& aTarget, const SalvoOptions& aSettings)
   : mRequestId(aRequestId)
   , mTarget(aTarget.mTrackPtr)
   , mSettings(aSettings)
{
   assert(aSettings.mQuantity > 0);
   if (aTarget.mTrackPtr != nullptr)
   {
      mTarget.mTrackPtr = aTarget.mTrackPtr->Clone();
   }
}

// ================================================================================================
// virtual
WsfWeapon::SalvoRequest::~SalvoRequest()
{
   delete mTarget.mTrackPtr;
}

// ================================================================================================
// Nested class SalvoEvent
// ================================================================================================
WsfWeapon::SalvoEvent::SalvoEvent(double aSimTime, WsfWeapon* aWeaponPtr, unsigned int aRequestId)
   : WsfEvent(aSimTime)
   , mWeaponPtr(aWeaponPtr)
   , mPlatformIndex(0)
   , mRequestId(aRequestId)
{
   assert(aWeaponPtr != nullptr);
   mPlatformIndex = mWeaponPtr->GetPlatform()->GetIndex();
}

// ================================================================================================
// virtual
WsfEvent::EventDisposition WsfWeapon::SalvoEvent::Execute()
{
   WsfPlatform* platformPtr = GetSimulation()->GetPlatformByIndex(mPlatformIndex);
   if ((platformPtr != nullptr) && (mWeaponPtr->IsTurnedOn()))
   {
      mWeaponPtr->ContinueSalvo(GetTime(), mRequestId);
   }
   return cDELETE;
}

// ================================================================================================
// Nested class CompleteReloadEvent
// ================================================================================================
WsfWeapon::CompleteReloadEvent::CompleteReloadEvent(double aCompletionTime, double aReloadedAmount, WsfWeapon* aWeaponPtr)
   : WsfEvent(aCompletionTime)
   , mWeaponPtr(aWeaponPtr)
   , mPlatformIndex(aWeaponPtr->GetPlatform()->GetIndex())
   , mQuantity(aReloadedAmount)
{
}

// ================================================================================================
// virtual
WsfEvent::EventDisposition WsfWeapon::CompleteReloadEvent::Execute()
{
   if (GetSimulation()->PlatformExists(mPlatformIndex))
   {
      mWeaponPtr->CompleteReloading(GetTime(), mQuantity);
   }
   return cDELETE;
}

// ================================================================================================
// Nested class WsfWeaponMode
// ================================================================================================
WsfWeapon::WsfWeaponMode::WsfWeaponMode(const WsfScenario& aScenario)
   : WsfMode()
   , mScenario(aScenario)
   , mPlatformPtr(nullptr)
   , mWeaponPtr(nullptr)
{
}

// ================================================================================================
WsfWeapon::WsfWeaponMode::WsfWeaponMode(const WsfWeaponMode& aSrc)
   : WsfMode(aSrc)
   , mScenario(aSrc.mScenario)
   , mPlatformPtr(nullptr)
   , mWeaponPtr(nullptr)
{
}

// ================================================================================================
bool WsfWeapon::WsfWeaponMode::Initialize(double aSimTime, WsfWeapon* aWeaponPtr)
{
   mWeaponPtr   = aWeaponPtr;
   mPlatformPtr = aWeaponPtr->GetPlatform();
   return true;
}

// ================================================================================================
bool WsfWeapon::WsfWeaponMode::ProcessInput(UtInput& aInput)
{
   return WsfMode::ProcessInput(aInput);
}
