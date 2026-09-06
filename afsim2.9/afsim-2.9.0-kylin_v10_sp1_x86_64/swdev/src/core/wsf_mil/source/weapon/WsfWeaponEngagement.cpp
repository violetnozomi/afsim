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

// =================================================================================================
//                             P R O G R A M M I N G    N O T E S
//
// Terminating a weapon engagement via the Terminate() method is not thread-safe because of the
// numerous observer callbacks and such that occur during weapon effects processing. Because of this,
// if Terminate() is called when multi-threading operations are active (such as during mover updates),
// an event will be scheduled to perform the terminate operation instead of performing it immediately.
// This ensures that the processing occurs while in a state where thread-safety is not a concern.
//
// In theory we should ALWAYS be able to schedule an event to perform the processing, but it is not
// done in non-multi-threading mode because it causes a different event stream, and hence different
// answers.
//
// Because the processing may get deferred, special care is taken to ensure the engagement object does
// not get deleted while the termination event is pending.
// =================================================================================================

#include "WsfWeaponEngagement.hpp"

#include "UtEntity.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtScriptBasicTypes.hpp"
#include "UtScriptVec3.hpp"
#include "UtVec3.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackManager.hpp"
#include "WsfWeapon.hpp"
#include "WsfWeaponEffects.hpp"
#include "WsfWeaponEffectsTypes.hpp"
#include "WsfWeaponPlatformExtension.hpp"
#include "script/WsfScriptContext.hpp"

// =================================================================================================
// static
WsfWeaponEngagement* WsfWeaponEngagement::Find(const WsfPlatform& aPlatform)
{
   return WsfWeaponPlatformExtension::GetWeaponEngagement(aPlatform);
}

// =================================================================================================
//! Assign the weapon engagement to the specified platform.
//! @param aPlatform      [input] The platform (representing the weapon) to whom the assignment is to be made.
//! @param aEngagementPtr [input] Pointer to the engagement object.
//!                       [output] Set to zero if the return value is false. (the object was deleted)
//! @returns 'true' if successful or false if not.
// static
bool WsfWeaponEngagement::Assign(WsfPlatform& aPlatform, WsfWeaponEngagement*& aEngagementPtr)
{
   bool assigned = WsfWeaponPlatformExtension::SetWeaponEngagement(aPlatform, aEngagementPtr);
   // If the engagement was not successfully assigned then SetWeaponEngagement deleted the object.
   // Set the pointer to null so if the caller uses it then they will crash, rather than accessing
   // memory that has been freed.
   if (!assigned)
   {
      aEngagementPtr = nullptr;
   }
   return assigned;
}

// =================================================================================================
//! Constructor for a new weapon engagement without a weapon platform.
//! @param aStartTime                 Simulation time at start of engagement.
//! @param aSimulationPtr             Pointer to the simulation
//! @param aWeaponSystemPtr           Pointer to the firing WsfWeapon system.
//! @param aTargetTrackId             The track ID of the target from the Fire request.
//! @param aIntendedTargetIndex       The intended target (if known) for this engagement.
//! @param aWeaponEffectsType         The type of WsfWeaponEffects which this weapon will have upon its target.
//! @param aIntendedTargetOffsetName  A targeted sub-region of the intended target (e.g., "canopy", "irst", "stabilizer").
//!                                   This sub-region must be recognized and used by the specified weapon effects type.
WsfWeaponEngagement::WsfWeaponEngagement(double            aStartTime,
                                         WsfSimulation*    aSimulationPtr,
                                         WsfWeapon*        aWeaponSystemPtr,
                                         const WsfTrackId& aTargetTrackId,
                                         size_t            aIntendedTargetIndex,
                                         WsfStringId       aWeaponEffectsType,
                                         WsfStringId       aIntendedTargetOffsetName) // = 0
   : mSimulationPtr(aSimulationPtr)
   , mStartTime(aStartTime)
   , mCompletionTime(-1.0)
   , // Not declared
   mSerialNumber(0)
   , mFiringPlatformIndex(0)
   , mWeaponPlatformIndex(0)
   , mIntendedTargetIndex(aIntendedTargetIndex)
   , mIntendedTargetName(nullptr)
   , mIntendedTargetOffsetName(aIntendedTargetOffsetName)
   , mWeaponSystemName(nullptr)
   , mWeaponSystemType(nullptr)
   , mInitialTargetTrackId(aTargetTrackId)
   , mTargetTrackId(aTargetTrackId)
   , mEffectPtr(nullptr)
   , mGeometryResult(cIN_PROGRESS)
   , mExtendedResult()
   , mComputedWeaponIndex(0)
   , mComputedTargetIndex(0)
   , mMissDistance(-1.0)
   , mIsExternallyControlled(false)
   , mIsInitialized(false)
   , mIsComplete(false)
   , mTerminatePending(false)
   , mDestroyPending(false)
   , mInitialWeaponVelocitySet(false)
   , mInitialTargetLocationSet(false)
   , mIFCEnabled(false)
{
   mSerialNumber = aSimulationPtr->NextEngagementSerialNumber();

   UtVec3d::Set(mTargetLocAtLaunchWCS, 0.0);
   UtVec3d::Set(mWeaponLocAtLaunchWCS, 0.0);
   UtVec3d::Set(mWeaponVelAtLaunchWCS, 0.0);
   UtVec3d::Set(mWeaponVelWCS, 0.0);
   UtVec3d::Set(mComputedWeaponLocWCS, 0.0);
   UtVec3d::Set(mComputedTargetLocWCS, 0.0);

   WsfPlatform* targetPtr = aSimulationPtr->GetPlatformByIndex(mIntendedTargetIndex);
   if (targetPtr != nullptr)
   {
      mIntendedTargetName = targetPtr->GetName();
   }

   if (aWeaponSystemPtr != nullptr)
   {
      // Usually valid for both Implicit and Explicit Engagements...
      mWeaponSystemName  = aWeaponSystemPtr->GetNameId();
      mWeaponSystemType  = aWeaponSystemPtr->GetTypeId();
      WsfPlatform* fpPtr = aWeaponSystemPtr->GetPlatform();
      if (fpPtr != nullptr)
      {
         mFiringPlatformIndex = fpPtr->GetIndex();
      }
   }

   SetWeaponEffects(aWeaponEffectsType);
}

// =================================================================================================
//! Destructor.
//! The destructor is protected from public use. Public callers must call Delete().
// protected virtual
WsfWeaponEngagement::~WsfWeaponEngagement()
{
   // There used to be code here to terminate an active engagement, but that caused exceptions if a
   // platform with active engagement was deleted and the simulation was in the process of terminating
   // (either in WsfSimulation::SimulationComplete() or WsfSimulation::~WsfSimulation()).
   //
   // It is the responsibility of the simulation to terminate engagements at the appropriate time.

   delete mEffectPtr;
}

// =================================================================================================
//! Request deletion of the engagement object.
//! This is the public interface to delete an engagement object.
//! @note The object may not be deleted immediately due to internal processing requirements.
// static
void WsfWeaponEngagement::Delete(WsfWeaponEngagement* aEngagementPtr)
{
   // This will delete the engagement if it does not have an outstanding 'Terminate' event or if we are
   // in simulation cleanup.
   //
   // If there is a pending 'Terminate' event then the engagement will be marked 'Destroy Pending'.
   // TerminateEvent::Execute() checks this at the end of its processing and delete the object if it is set.

   if (aEngagementPtr != nullptr)
   {
      aEngagementPtr->mDestroyPending = true;

      if ((!aEngagementPtr->mTerminatePending) || (aEngagementPtr->GetSimulation()->GetState() == WsfSimulation::cCOMPLETE))
      {
         // If the engagement to be deleted has been 'assigned' (i.e.: WsfWeaponEngagment::Assign was called),
         // then the weapon platform owns the engagement and it is responsible for deleting it.
         //
         // NOTE: WsfWeaponPlatform will call us back to delete the object, but by then it will have relinquished
         //       ownership and cause the if check to fail.
         WsfPlatform* weaponPlatformPtr = aEngagementPtr->GetWeaponPlatform();
         if ((weaponPlatformPtr != nullptr) &&
             (WsfWeaponPlatformExtension::GetWeaponEngagement(*weaponPlatformPtr) == aEngagementPtr))
         {
            WsfWeaponPlatformExtension::SetWeaponEngagement(*weaponPlatformPtr, nullptr);
         }
         else
         {
            // Not assigned to a weapon platform, so just delete it.
            delete aEngagementPtr;
         }
      }
   }
}
// =================================================================================================
//! Initialize the engagement with the weapon platform.
//! @param aSimTime            The current simulation time.
//! @param aWeaponPlatformPtr  Pointer to the weapon platform or 0 if the weapon is not a platform.
//! @return 'true' if successfully initialized
bool WsfWeaponEngagement::Initialize(double aSimTime, WsfPlatform* aWeaponPlatformPtr)
{
   bool success = true;

   if (aWeaponPlatformPtr != nullptr)
   {
      mWeaponPlatformIndex = aWeaponPlatformPtr->GetIndex();
   }

   // NOTE: In general, this method is called only once, but it may be called again. In particular,
   // platforms that 'deferred launch' (see WsfMover and WsfExplicitWeapon for a descriptions and use)
   // will be created and initialized but not added to the simulation until they are actually launched.
   // WsfSimulation::AddPlatform() will call this method again, thus the special logic...

   // The first part of can be run more than once.

   mStartTime = aSimTime;

   // Capture the launch location of the target if it hasn't been defined already
   WsfPlatform* platformPtr = GetSimulation()->GetPlatformByIndex(mIntendedTargetIndex);
   if ((!mInitialTargetLocationSet) && (platformPtr != nullptr))
   {
      platformPtr->GetLocationWCS(mTargetLocAtLaunchWCS);
   }

   // Capture the launch location of the weapon (or the firing platform if implicit weapon)
   platformPtr = GetSimulation()->GetPlatformByIndex(mWeaponPlatformIndex);
   if (platformPtr != nullptr)
   {
      platformPtr->GetLocationWCS(mWeaponLocAtLaunchWCS);
      platformPtr->GetVelocityWCS(mWeaponVelAtLaunchWCS);
   }
   else
   {
      platformPtr = GetSimulation()->GetPlatformByIndex(mFiringPlatformIndex);
      if (platformPtr != nullptr)
      {
         platformPtr->GetLocationWCS(mWeaponLocAtLaunchWCS);
         if (!mInitialWeaponVelocitySet)
         {
            platformPtr->GetVelocityWCS(mWeaponVelAtLaunchWCS);
         }
      }
   }

   // The following part can only run once...

   if (!mIsInitialized)
   {
      mIsInitialized = true;
      success        = mEffectPtr->Initialize(mStartTime, this);
   }
   return success;
}

// =================================================================================================
//! An alternative form of Initialize that does not modify the engagement start time.
//! @param aWeaponPlatformPtr  Pointer to the weapon platform or 0 if the weapon is not a platform.
//! @return 'true' if successfully initialized
bool WsfWeaponEngagement::Initialize(WsfPlatform* aWeaponPlatformPtr)
{
   return Initialize(mStartTime, aWeaponPlatformPtr);
}

// =================================================================================================
//! Return the string equivalent of a GeometryResult enum.
// static
bool WsfWeaponEngagement::EnumToString(std::string& aString, GeometryResult aGeometryResult)
{
   bool ok = true;
   switch (aGeometryResult)
   {
   case cIN_PROGRESS:
      aString = "in_progress";
      break;
   case cDUD:
      aString = "dud";
      break;
   case cTARGET_IMPACT:
      aString = "target_impact";
      break;
   case cFAR_AWAY_IN_AIR:
      aString = "far_away_in_air";
      break;
   case cFAR_AWAY_ABOVE_GROUND:
      aString = "far_away_above_ground";
      break;
   case cFAR_AWAY_GROUND_IMPACT:
      aString = "far_away_ground_impact";
      break;
   case cTARGET_PROXIMITY_AIR_BURST:
      aString = "target_proximity_air_burst";
      break;
   case cTARGET_PROXIMITY_ABOVE_GROUND:
      aString = "target_proximity_above_ground";
      break;
   case cTARGET_PROXIMITY_GROUND_IMPACT:
      aString = "target_proximity_ground_impact";
      break;
   default:
      ok = false;
      break;
   }
   return ok;
}

// =================================================================================================
//! Return the GeomtryResult enum from a string.
// static
bool WsfWeaponEngagement::StringToEnum(GeometryResult& aGeometryResult, const std::string& aString)
{
   bool ok = true;
   if (aString == "dud")
   {
      aGeometryResult = cDUD;
   }
   else if (aString == "target_impact")
   {
      aGeometryResult = cTARGET_IMPACT;
   }
   else if (aString == "far_away_in_air")
   {
      aGeometryResult = cFAR_AWAY_IN_AIR;
   }
   else if (aString == "far_away_above_ground")
   {
      aGeometryResult = cFAR_AWAY_ABOVE_GROUND;
   }
   else if (aString == "far_away_ground_impact")
   {
      aGeometryResult = cFAR_AWAY_GROUND_IMPACT;
   }
   else if (aString == "target_proximity_air_burst")
   {
      aGeometryResult = cTARGET_PROXIMITY_AIR_BURST;
   }
   else if (aString == "target_proximity_above_ground")
   {
      aGeometryResult = cTARGET_PROXIMITY_ABOVE_GROUND;
   }
   else if (aString == "target_proximity_ground_impact")
   {
      aGeometryResult = cTARGET_PROXIMITY_GROUND_IMPACT;
   }
   else
   {
      ok = false;
   }
   return ok;
}

// =================================================================================================
//! Return a pointer to the firing platform.
//! @note May be zero if the previously existing firing platform has been destroyed.
WsfPlatform* WsfWeaponEngagement::GetFiringPlatform() const
{
   return GetSimulation()->GetPlatformByIndex(mFiringPlatformIndex);
}

// =================================================================================================
//! Return the name (ID) of the firing platform.
WsfStringId WsfWeaponEngagement::GetFiringPlatformName() const
{
   return GetSimulation()->GetPlatformNameId(mFiringPlatformIndex);
}

// =================================================================================================
//! Set the pointer to the firing platform.
//! This is used when there is not an associated weapon system that is passed on the constructor.
//! It is typically used when the firing platform is an externally controlled entity where we
//! probably aren't explicitly modeling the launching subsystem.
//!
//! @see WsfDisFire and WsfDisDetonation for examples.
void WsfWeaponEngagement::SetFiringPlatform(WsfPlatform* aFiringPlatformPtr)
{
   mFiringPlatformIndex = 0;
   if (aFiringPlatformPtr != nullptr)
   {
      mFiringPlatformIndex = aFiringPlatformPtr->GetIndex();
   }
}

// =================================================================================================
//! Access the weapon system to retrieve other information.
//! @note This method returns 0 if the parent platform has been deleted.
const WsfWeapon* WsfWeaponEngagement::GetWeaponSystem() const
{
   const WsfWeapon* weaponPtr = nullptr;
   WsfPlatform*     fpPtr     = GetFiringPlatform();
   if (fpPtr != nullptr)
   {
      weaponPtr = fpPtr->GetComponent<WsfWeapon>(mWeaponSystemName);
   }
   return weaponPtr;
}

// =================================================================================================
//! Return a pointer to the weapon platform.
//! @note Will be zero for an implicit engagement.
//! @note May be zero if the previously existing weapon platform has been destroyed.
WsfPlatform* WsfWeaponEngagement::GetWeaponPlatform() const
{
   return GetSimulation()->GetPlatformByIndex(mWeaponPlatformIndex);
}

// =================================================================================================
//! Return the name (ID) of the weapon platform.
//! @note This will be zero for an implicit weapon.
WsfStringId WsfWeaponEngagement::GetWeaponPlatformName() const
{
   return GetSimulation()->GetPlatformNameId(mWeaponPlatformIndex);
}

// =================================================================================================
//! Return the track ID of the current target track.
//! @note This will be a null ID if there is no current target.
//! @note This return value is typically different than the value returned by GetInitialTargetTrackId.
//! That value represents the track ID as seen by the firing platform while this value represents the
//! track ID as seen by the weapon platform.
const WsfTrackId& WsfWeaponEngagement::GetTargetTrackId() const
{
   UpdateIntendedTarget();
   return mTargetTrackId;
}

// =================================================================================================
//! Return the platform name of the target.
//! @note This will be zero if there is no specific target.
WsfStringId WsfWeaponEngagement::GetIntendedTargetName() const
{
   UpdateIntendedTarget();
   return mIntendedTargetName;
}

// =================================================================================================
//! Return the platform index of the target.
//! @note This will be zero if there is no specific target or if the target track was not associated
//! with a true platform.
size_t WsfWeaponEngagement::GetTargetPlatformIndex() const
{
   UpdateIntendedTarget();
   return mIntendedTargetIndex;
}

// =================================================================================================
//! Return a pointer to the target platform.
//! @note This will be zero if there is no specific target or if the target is no longer alive.
WsfPlatform* WsfWeaponEngagement::GetTargetPlatform() const
{
   UpdateIntendedTarget();
   return GetSimulation()->GetPlatformByIndex(mIntendedTargetIndex);
}

// =================================================================================================
//! Return the elapsed time for this engagement.
//! The return value will grow from zero while the engagement is in progress,
//! and will be fixed after the engagement is complete.
double WsfWeaponEngagement::GetElapsedTime(double aSimTime) const
{
   double elapsedTime = aSimTime - mStartTime; // Assume engagement is not completed
   if (mCompletionTime >= 0.0)                 // Engagement has been completed
   {
      return mCompletionTime - mStartTime;
   }
   return elapsedTime;
}

// =================================================================================================
//! Set the weapon effects to be used when the engagement is terminated.
//! @param aWeaponEffectsType The name of the weapon effects to be used.
void WsfWeaponEngagement::SetWeaponEffects(WsfStringId aWeaponEffectsType)
{
   const WsfScenario& scenario(mSimulationPtr->GetScenario());
   mEffectPtr = nullptr;
   if (!aWeaponEffectsType.IsNull())
   {
      mEffectPtr = WsfWeaponEffectsTypes::Get(scenario).Clone(aWeaponEffectsType);
      if (mEffectPtr == nullptr)
      {
         auto out = ut::log::warning() << "Undefined weapon_effect.";
         out.AddNote() << "Weapon Effect Type: " << aWeaponEffectsType;
         out.AddNote() << "A default effect will be used.";
      }
   }

   // Create a default effect if one wasn't specified or if the specified effect type does not exist.

   if (mEffectPtr == nullptr)
   {
      // Create a simple default lethality, just to fill the hole
      mEffectPtr = WsfWeaponEffectsTypes::Get(scenario).Clone("#DEFAULT_LETHALITY");
   }

   // If initialization has already been completed then we are responsible for initializing the object.
   // Otherwise we'll just let Initialize() do it.
   if (mIsInitialized)
   {
      if (!mEffectPtr->Initialize(mStartTime, this))
      {
         auto out = ut::log::warning() << "Unable to initialize weapon_effect.";
         out.AddNote() << "Weapon Effect Type: " << aWeaponEffectsType;
      }
   }
}

// =================================================================================================
//! Virtual base method to evaluate an engagement (primarily applies to implicit weapons).
//! Base implementation is to continue the engagement until the target is dead.  The
//! owning implementation may choose to end the engagement earlier, via Terminate().
//! @param aSimTime Current simulated time for engagement evaluation.
// virtual
void WsfWeaponEngagement::Update(double aSimTime)
{
   if (!mIsComplete)
   {
      mEffectPtr->ProcessEffectImplicit(aSimTime);
      WsfWeaponEffects::Result result = mEffectPtr->GetTargetResult();
      if (result == WsfWeaponEffects::cKILLED)
      {
         Terminate(aSimTime);
      }
   }
}

// =================================================================================================
//! Terminate the engagement.
//! If the geometry result indicates that it isn't a dud, then the applicable WsfWeaponEffects effect
//! on platforms in the vicinity of the weapon's lethal area will be applied.
//!
//! @param aSimTime        Current simulated time at engagement termination.
//! @param aGeometryResult A preliminary assessment of the closeness of the hit, based upon geometry.
//! The weapon effects object may override this preliminary assessment with a better one.
// virtual
void WsfWeaponEngagement::Terminate(double aSimTime, GeometryResult aGeometryResult)
{
   // Immediately exit if this has already been called or if the simulation is shutting down.
   if (mIsComplete || (GetSimulation()->GetState() == WsfSimulation::cCOMPLETE))
   {
      return;
   }

   mIsComplete = true;

   // If the completion time has not been declared then use the current simulation time.
   if (mCompletionTime < 0.0)
   {
      mCompletionTime = aSimTime;
   }

   mGeometryResult = aGeometryResult;

   // See PROGRAMMING NOTES at the top of the file.

   if (GetSimulation()->MultiThreadingActive())
   {
      mTerminatePending = true; // Prevent deletion until the event executes
      GetSimulation()->AddEvent(ut::make_unique<TerminateEvent>(aSimTime, this));
   }
   else
   {
      PrivateTerminate(aSimTime);
   }
}

// =================================================================================================
//! Get the location of the target at the time the engagement was terminated.
//! @param aTargetPtr    [input]  The pointer to the target platform.
//! @param [aTargetLocWCS [output] The computed target location as established by SetTargetLocationWCS
//! or the current target location on the computed location was not defined or not appropriate.
void WsfWeaponEngagement::GetTargetLocationWCS(WsfPlatform* aTargetPtr, double aTargetLocWCS[3]) const
{
   if ((aTargetPtr != nullptr) && (aTargetPtr->GetIndex() == mComputedTargetIndex))
   {
      // Use the computed location.
      UtVec3d::Set(aTargetLocWCS, mComputedTargetLocWCS);
   }
   else if (aTargetPtr != nullptr)
   {
      // A computed location has not been set or the incoming platform is not actually the same as the
      // platform for which the computed location applies.

      aTargetPtr->Update(GetSimulation()->GetSimTime()); // Force position update to the current simulation time.
      aTargetPtr->GetLocationWCS(aTargetLocWCS);
      if (mCompletionTime >= 0.0)
      {
         // Extrapolate the platform to the completion time. Note that last update time of the platform
         // could be either before or after the completion time, so the returned location is the location
         // moved moved backward or forward as appropriate.
         double deltaT = mCompletionTime - aTargetPtr->GetLastUpdateTime();
         double velWCS[3];
         aTargetPtr->GetVelocityWCS(velWCS);
         UtVec3d::AddProduct(aTargetLocWCS, aTargetLocWCS, deltaT, velWCS);
      }
   }
   else
   {
      UtVec3d::Set(aTargetLocWCS, 0.0);
   }
}

// =================================================================================================
//! Set the location of the target at the time the engagement is terminated.
//! Objects such as a fuse must often determine the actual location AFTER the
//! weapon has moved passed the point of detonation. This method provides the
//! fuse a method for defining where the target would have been when the fuse
//! would have actually detonated.
//! @param aTargetPtr    [input] Pointer to the target platform.
//! @param aTargetLocWCS [input] The true location of the target at the time of engagement termination.
void WsfWeaponEngagement::SetTargetLocationWCS(WsfPlatform* aTargetPtr, const double aTargetLocWCS[3])
{
   mComputedTargetIndex = 0;
   if (aTargetPtr != nullptr)
   {
      mComputedTargetIndex = aTargetPtr->GetIndex();
      UtVec3d::Set(mComputedTargetLocWCS, aTargetLocWCS);
   }
}

// =================================================================================================
//! Get the location of the weapon at the time the engagement was terminated.
//! @param aWeaponPtr    [input]  The pointer to the weapon platform.
//! @param aWeaponLocWCS [output] The computed weapon location as established by SetWeaponLocationWCS
//! or the current weapon location on the computed location was not defined or not appropriate.
void WsfWeaponEngagement::GetWeaponLocationWCS(WsfPlatform* aWeaponPtr, double aWeaponLocWCS[3]) const
{
   if (aWeaponPtr != nullptr)
   {
      if (aWeaponPtr->GetIndex() == mComputedWeaponIndex)
      {
         // Use the computed location.
         UtVec3d::Set(aWeaponLocWCS, mComputedWeaponLocWCS);
      }
      else
      {
         // A computed location has not been set or the incoming platform is not actually the same as the
         // platform for which the computed location applies.

         aWeaponPtr->Update(GetSimulation()->GetSimTime()); // Force position update to the current simulation time.
         aWeaponPtr->GetLocationWCS(aWeaponLocWCS);
         if (mCompletionTime >= 0.0)
         {
            // Extrapolate the platform to the completion time. Note that last update time of the platform
            // could be either before or after the completion time, so the returned location is the location
            // moved moved backward or forward as appropriate.
            double deltaT = mCompletionTime - aWeaponPtr->GetLastUpdateTime();
            double velWCS[3];
            aWeaponPtr->GetVelocityWCS(velWCS);
            UtVec3d::AddProduct(aWeaponLocWCS, aWeaponLocWCS, deltaT, velWCS);
         }
      }
   }
   else
   {
      UtVec3d::Set(aWeaponLocWCS, 0.0);
   }
}

// =================================================================================================
//! Set the location of the weapon at the time the engagement is terminated.
//! Objects such as a fuse must often determine the actual location AFTER the
//! weapon has moved passed the point of detonation. This method provides the
//! fuse a method for defining where the weapon would have been when the fuse
//! would have actually detonated.
//! @param aWeaponPtr    [input] Pointer to the weapon platform.
//! @param aWeaponLocWCS [input] The true location of the weapon at the time of engagement termination.
void WsfWeaponEngagement::SetWeaponLocationWCS(WsfPlatform* aWeaponPtr, const double aWeaponLocWCS[3])
{
   mComputedWeaponIndex = 0;
   if (aWeaponPtr != nullptr)
   {
      mComputedWeaponIndex = aWeaponPtr->GetIndex();
      UtVec3d::Set(mComputedWeaponLocWCS, aWeaponLocWCS);
   }
}

// =================================================================================================
//! Get the location of the intended target at the time of launch.
//! @param aTargetLocWCS [output] The true location of the intended at the time of launch.
//! @note The location will be {0,0,0} if there was no intended target tat the time of launch,
//! and the initial location was not set prior to initialization.
void WsfWeaponEngagement::GetTargetLocationAtLaunchWCS(double aTargetLocWCS[3]) const
{
   UtVec3d::Set(aTargetLocWCS, mTargetLocAtLaunchWCS);
}

// =================================================================================================
//! Get the location of the weapon at the time of launch.
//! @param aWeaponLocWCS [output] The true location of the weapon at the time of launch. If the
//! weapon is an implicit weapon then it will be the location of the firing platform.
void WsfWeaponEngagement::GetWeaponLocationAtLaunchWCS(double aWeaponLocWCS[3]) const
{
   UtVec3d::Set(aWeaponLocWCS, mWeaponLocAtLaunchWCS);
}

// =================================================================================================
//! Get the velocity of the weapon at the time of launch.
//! @param aWeaponVelWCS [output] The velocity of the weapon at the time of launch. If the
//! weapon is an implicit weapon then it will either be the velocity of the firing platform
//! (as in the case of a high-energy laser), or an estimate of the velocity that a corresponding
//! explicit weapon would have (as in the case of an indirect fire weapon such as a mortar).
void WsfWeaponEngagement::GetWeaponVelocityAtLaunchWCS(double aWeaponVelWCS[3]) const
{
   UtVec3d::Set(aWeaponVelWCS, mWeaponVelAtLaunchWCS);
}

// =================================================================================================
//! Get the location of the weapon relative to the target.
//! @param aWeaponLocECS [output] The location of the weapon, relative to its target.
//! Intended for use at time of weapon termination.
void WsfWeaponEngagement::GetWeaponLocationECS(double aWeaponLocECS[3]) const
{
   if (GetTargetPlatform() != nullptr) // If there is a target
   {
      // Establish the weapon's location in the World Coordinate System (WCS)
      WsfPlatform* weaponPtr = GetWeaponPlatform();
      double       weaponLocationWCS[3];
      GetWeaponLocationWCS(weaponPtr, weaponLocationWCS);

      // Establish the target's location in the World Coordinate System (WCS)
      WsfPlatform* targetPtr = GetTargetPlatform();
      UtEntity     target(*targetPtr);
      double       targetLocWCS[3];
      GetTargetLocationWCS(targetPtr, targetLocWCS);
      target.SetLocationWCS(targetLocWCS);

      // Return the weapon's location, relative to its target
      target.ConvertWCSToECS(weaponLocationWCS, aWeaponLocECS);
   }
   else // If there is not a target
   {
      // Indicate that the weapon location is invalid
      UtVec3d::Set(aWeaponLocECS, 0.0);
   }
}

// =================================================================================================
// Get the velocity of the weapon relative to the target.
//! @param aWeaponVelECS [output] The velocity of the weapon, relative to its target.
//! Intended for use at time of weapon termination.
void WsfWeaponEngagement::GetWeaponVelocityECS(double aWeaponVelECS[3]) const
{
   if (GetTargetPlatform() != nullptr) // If there is a target
   {
      // Establish the weapon's velocity in the World Coordinate System (WCS)
      // For implicit weapon engagements, there is no weapon platform so use result of GetWeaponVelocityWCS
      double       weaponVelocityWCS[3];
      WsfPlatform* weaponPtr = GetWeaponPlatform();
      if (weaponPtr)
      {
         weaponPtr->GetVelocityWCS(weaponVelocityWCS);
      }
      else
      {
         GetWeaponVelocityWCS(weaponVelocityWCS);
      }

      // Establish the target's velocity in the World Coordinate System (WCS)
      WsfPlatform* targetPtr = GetTargetPlatform();
      double       targetVelocityWCS[3];
      targetPtr->GetVelocityWCS(targetVelocityWCS);

      // Define the weapon's velocity relative to the target's velocity in WCS
      double weaponVelocityRelativeWCS[3];
      UtVec3d::Subtract(weaponVelocityRelativeWCS, weaponVelocityWCS, targetVelocityWCS);

      // Return the weapon's relative velocity in ECS of target
      targetPtr->ConvertWCSVectorToECS(aWeaponVelECS, weaponVelocityRelativeWCS);
   }
   else // If there is not a target
   {
      // Indicate that the weapon location is invalid
      UtVec3d::Set(aWeaponVelECS, 0.0);
   }
}

// =================================================================================================
//! Get the miss distance between the weapon and the target.
//! This will return the first of the following available distances:
//! - If the weapon model provided a miss distance and the distance was provided for the target
//!   under consideration, distance, use the declared miss distance.
//! - If the weapon model provided actual weapon and target locations at PCA and the target
//!   under consideration was the same as the one for which the target location was computed,
//!   use use the distance between the actual locations.
//! - Otherwise use the current distance between the weapon and the target. Note that this may
//!   not be the actual PCA because most weapon models go one timestep past PCA before declaring
//!   intercept.
//! @param aTargetPtr The pointer to the target in question. If 0 then it will use the current target platform.
//! @returns The miss distance in meters.
double WsfWeaponEngagement::GetMissDistance(WsfPlatform* aTargetPtr) const
{
   size_t targetIndex = GetTargetPlatformIndex();
   if (aTargetPtr != nullptr)
   {
      targetIndex = aTargetPtr->GetIndex();
   }

   double missDistance = 1.0e6;
   if ((mMissDistance >= 0.0) && (targetIndex == GetTargetPlatformIndex()))
   {
      missDistance = mMissDistance; // Use the declared miss distance
   }
   else if ((mMissDistance >= 0.0) && (aTargetPtr == nullptr) && // target in question not provided
            (targetIndex == 0))                                  // a target was never declared
   {
      missDistance = mMissDistance; // Use the declared miss distance
   }
   else
   {
      // The weapon did not supply a miss distance, so compute it using what we know.
      WsfPlatform* weaponPtr = GetSimulation()->GetPlatformByIndex(GetWeaponPlatformIndex());
      WsfPlatform* targetPtr = GetSimulation()->GetPlatformByIndex(targetIndex);
      if (targetPtr != nullptr)
      {
         // This will return either the target location at PCA (if provided by the weapon model)
         // or the current location of the target.
         double targetLocWCS[3];
         double weaponLocWCS[3];
         GetTargetLocationWCS(targetPtr, targetLocWCS);
         bool canCompute = false;

         if (weaponPtr != nullptr)
         {
            // This will return either the weapon location at PCA (if provided by the weapon model)
            // or the current location of the weapon.
            GetWeaponLocationWCS(weaponPtr, weaponLocWCS);
            canCompute = true;
         }
         else if (mInitialTargetLocationSet) // and no weapon platform, so this is an implicit weapon engagement.
         {
            // Assume we are at the target location
            UtVec3d::Set(weaponLocWCS, mTargetLocAtLaunchWCS);
            canCompute = true;
         }

         if (canCompute)
         {
            double deltaLocWCS[3];
            UtVec3d::Subtract(deltaLocWCS, targetLocWCS, weaponLocWCS);
            missDistance = UtVec3d::Magnitude(deltaLocWCS);
         }
      }
   }
   return missDistance;
}

// =================================================================================================
//! Copy a serial number (should only be called by WsfExplicitWeapon).
//! @param aRealEngPtr The real engagement whose serial number is to be copied to
//!                    the temporary engagement.
//! @param aTempEngPtr The temporary engagement whose serial number is to be updated.
// static
void WsfWeaponEngagement::CopySerialNumber(const WsfWeaponEngagement* aRealEngPtr, WsfWeaponEngagement* aTempEngPtr)
{
   // The two supplied engagements should have been the last two allocated
   // and should have been allocated such that the last of the two. Make
   // sure this is the case.
   unsigned int lastSerialNumber = aRealEngPtr->GetSimulation()->PreviousEngagementSerialNumber();
   if ((aTempEngPtr->mSerialNumber == lastSerialNumber) && ((aRealEngPtr->mSerialNumber + 1) == lastSerialNumber))
   {
      aTempEngPtr->mSerialNumber = aRealEngPtr->mSerialNumber;
      aRealEngPtr->GetSimulation()->ReclaimPreviousEngagementSerialNumber();
   }
}

// =================================================================================================
//! If the current target is different than the intended target, update the intended target.
void WsfWeaponEngagement::UpdateIntendedTarget() const
{
   WsfPlatform* weaponPtr = GetWeaponPlatform();
   if (weaponPtr != nullptr)
   {
      const WsfTrack* currentTargetTrackPtr = weaponPtr->GetTrackManager().GetCurrentTarget();
      if ((currentTargetTrackPtr != nullptr) && (currentTargetTrackPtr->GetTargetIndex() != 0) &&
          (currentTargetTrackPtr->GetTargetIndex() != mIntendedTargetIndex))
      {
         mIntendedTargetIndex = currentTargetTrackPtr->GetTargetIndex();
         mIntendedTargetName  = currentTargetTrackPtr->GetTargetName();
         mTargetTrackId       = weaponPtr->GetTrackManager().GetCurrentTargetTrackId();
      }
   }
}

// =================================================================================================
//! The private portion of 'Terminate'.
//!
//! This performs the actual logic of 'Terminate'. We are known to be in the serial portion of the
//! the simulation so it is safe to execute code that is not thread-safe.
//!
//! @param aSimTime The current simulation time.
// private
void WsfWeaponEngagement::PrivateTerminate(double aSimTime)
{
   // Calculate weapon effects (and remove any killed platforms).
   if ((mGeometryResult != cIN_PROGRESS) && (mGeometryResult != cDUD))
   {
      if (GetSimulation()->PlatformExists(mWeaponPlatformIndex))
      {
         // Explicit effects.
         mEffectPtr->ProcessEffectExplicit(aSimTime);
      }
      else if (GetSimulation()->PlatformExists(mFiringPlatformIndex))
      {
         // Implicit effects.
         mEffectPtr->ProcessEffectImplicit(aSimTime);
      }
   }

   // let the weapon do it's record keeping
   WsfPlatform* shooterPtr = GetFiringPlatform();
   if (shooterPtr != nullptr)
   {
      WsfWeapon* weaponPtr = shooterPtr->GetComponent<WsfWeapon>(GetWeaponSystemName());
      if (weaponPtr != nullptr)
      {
         weaponPtr->EngagementComplete(aSimTime, this);
      }
   }

   // now terminate
   mEffectPtr->Terminate(aSimTime);
}

// =================================================================================================
//! Set the initial weapon velocity.
//! This is done for implicit weapons that behave like explicit ones, such as
//! a mortar or artillery launcher.
void WsfWeaponEngagement::SetInitialWeaponVelocityWCS(const double aInitialVelWCS[3])
{
   UtVec3d::Set(mWeaponVelAtLaunchWCS, aInitialVelWCS);
   mInitialWeaponVelocitySet = true;
}

// =================================================================================================
//! Set the initial target location.
//! This is done for engagements with no initial intended target
//! a mortar or artillery launcher.
void WsfWeaponEngagement::SetInitialTargetLocationWCS(const double aInitialLocWCS[3])
{
   UtVec3d::Set(mTargetLocAtLaunchWCS, aInitialLocWCS);
   mInitialTargetLocationSet = true;
}

void WsfWeaponEngagement::GetWeaponVelocityWCS(double aWeaponVelWCS[3]) const
{
   UtVec3d::Set(aWeaponVelWCS, mWeaponVelWCS);
}

//! Set the terminal weapon velocity; used if there is no weapon platform in implicit engagements.
// =================================================================================================
void WsfWeaponEngagement::SetWeaponVelocityWCS(const double aTerminalVelocityWCS[3])
{
   UtVec3d::Set(mWeaponVelWCS, aTerminalVelocityWCS);
}

// =================================================================================================
WsfWeaponEngagement::TerminateEvent::TerminateEvent(double aSimTime, WsfWeaponEngagement* aEngagementPtr)
   : mEngagementPtr(aEngagementPtr)
{
}

// =================================================================================================
// virtual
WsfEvent::EventDisposition WsfWeaponEngagement::TerminateEvent::Execute()
{
   // Indicate the termination event is no longer pending.

   mEngagementPtr->mTerminatePending = false;

   // Perform the actual termination processing.

   mEngagementPtr->PrivateTerminate(GetTime());

   // If the owner has logically deleted the object (mDestroyPending is true) then we must delete the
   // object as there is no longer anyone that has a pointer to the object.

   if (mEngagementPtr->mDestroyPending)
   {
      delete mEngagementPtr;
   }

   return cDELETE;
}

// =================================================================================================
//! The script interface 'class'
class WSF_MIL_EXPORT WsfScriptWeaponEngagementClass : public UtScriptClass
{
public:
   WsfScriptWeaponEngagementClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(StartTime);
   UT_DECLARE_SCRIPT_METHOD(TimeSinceStarted);
   UT_DECLARE_SCRIPT_METHOD(CompletionTime);
   UT_DECLARE_SCRIPT_METHOD(SerialNumber);
   UT_DECLARE_SCRIPT_METHOD(FiringPlatform);
   UT_DECLARE_SCRIPT_METHOD(FiringPlatformName);
   UT_DECLARE_SCRIPT_METHOD(WeaponSystemName);
   UT_DECLARE_SCRIPT_METHOD(WeaponSystemType);
   UT_DECLARE_SCRIPT_METHOD(WeaponPlatform);
   UT_DECLARE_SCRIPT_METHOD(WeaponPlatformName);
   UT_DECLARE_SCRIPT_METHOD(WeaponLocation);
   UT_DECLARE_SCRIPT_METHOD(WeaponLocationAtLaunch);
   UT_DECLARE_SCRIPT_METHOD(WeaponLocationECS);
   UT_DECLARE_SCRIPT_METHOD(WeaponVelocityECS);
   UT_DECLARE_SCRIPT_METHOD(TargetPlatform);
   UT_DECLARE_SCRIPT_METHOD(TargetPlatformName);
   UT_DECLARE_SCRIPT_METHOD(TargetOffsetName);
   UT_DECLARE_SCRIPT_METHOD(TargetLocation_1);
   UT_DECLARE_SCRIPT_METHOD(TargetLocation_2);
   UT_DECLARE_SCRIPT_METHOD(TargetLocationAtLaunch);
   UT_DECLARE_SCRIPT_METHOD(TargetTrackId);
   UT_DECLARE_SCRIPT_METHOD(TargetResult);
   UT_DECLARE_SCRIPT_METHOD(IncidentalResult);
   UT_DECLARE_SCRIPT_METHOD(ExtendedResult);
   UT_DECLARE_SCRIPT_METHOD(MissDistance_1);
   UT_DECLARE_SCRIPT_METHOD(MissDistance_2);
   UT_DECLARE_SCRIPT_METHOD(Pk);
   UT_DECLARE_SCRIPT_METHOD(PkDegrade);
   UT_DECLARE_SCRIPT_METHOD(SetPkDegrade);
   UT_DECLARE_SCRIPT_METHOD(PkDrawn);
   UT_DECLARE_SCRIPT_METHOD(IncidentalDamageAllowed);
   UT_DECLARE_SCRIPT_METHOD(SetIncidentalDamageAllowed);
   UT_DECLARE_SCRIPT_METHOD(SetWeaponEffects);
   UT_DECLARE_SCRIPT_METHOD(Retarget); // NO_DOC | DEPRECATED
};

//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
std::unique_ptr<UtScriptClass> WsfWeaponEngagement::CreateScriptClass(const std::string& aClassName,
                                                                      UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptWeaponEngagementClass>(aClassName, aScriptTypesPtr);
}

WsfScriptWeaponEngagementClass::WsfScriptWeaponEngagementClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfWeaponEngagement");
   mIsScriptAccessible = true;

   AddMethod(ut::make_unique<StartTime>());
   AddMethod(ut::make_unique<TimeSinceStarted>());
   AddMethod(ut::make_unique<CompletionTime>());
   AddMethod(ut::make_unique<SerialNumber>());
   AddMethod(ut::make_unique<FiringPlatform>());
   AddMethod(ut::make_unique<FiringPlatformName>());
   AddMethod(ut::make_unique<WeaponSystemName>());
   AddMethod(ut::make_unique<WeaponSystemType>());
   AddMethod(ut::make_unique<WeaponPlatform>());
   AddMethod(ut::make_unique<WeaponPlatformName>());
   AddMethod(ut::make_unique<WeaponLocation>());
   AddMethod(ut::make_unique<WeaponLocationAtLaunch>());
   AddMethod(ut::make_unique<WeaponLocationECS>());
   AddMethod(ut::make_unique<WeaponVelocityECS>());
   AddMethod(ut::make_unique<TargetPlatform>());
   AddMethod(ut::make_unique<TargetPlatformName>());
   AddMethod(ut::make_unique<TargetOffsetName>());
   AddMethod(ut::make_unique<TargetLocation_1>("TargetLocation"));
   AddMethod(ut::make_unique<TargetLocation_2>("TargetLocation"));
   AddMethod(ut::make_unique<TargetLocationAtLaunch>());
   AddMethod(ut::make_unique<TargetTrackId>());
   AddMethod(ut::make_unique<TargetResult>());
   AddMethod(ut::make_unique<IncidentalResult>());
   AddMethod(ut::make_unique<ExtendedResult>());
   AddMethod(ut::make_unique<MissDistance_1>("MissDistance"));
   AddMethod(ut::make_unique<MissDistance_2>("MissDistance"));
   AddMethod(ut::make_unique<Pk>());
   AddMethod(ut::make_unique<PkDegrade>());
   AddMethod(ut::make_unique<SetPkDegrade>());
   AddMethod(ut::make_unique<PkDrawn>());
   AddMethod(ut::make_unique<IncidentalDamageAllowed>());
   AddMethod(ut::make_unique<SetIncidentalDamageAllowed>());
   AddMethod(ut::make_unique<SetWeaponEffects>());
   AddMethod(ut::make_unique<Retarget>()); // NO_DOC | DEPRECATED
}

#include "script/WsfScriptDefs.hpp"

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, StartTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetStartTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, TimeSinceStarted, 0, "double", "")
{
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   aReturnVal.SetDouble(simTime - aObjectPtr->GetStartTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, CompletionTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetCompletionTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, SerialNumber, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetSerialNumber()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, FiringPlatform, 0, "WsfPlatform", "")
{
   WsfPlatform* platformPtr = aObjectPtr->GetFiringPlatform();
   aReturnVal.SetPointer(new UtScriptRef(platformPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, FiringPlatformName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetFiringPlatformName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, WeaponSystemName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetWeaponSystemName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, WeaponSystemType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetWeaponSystemType());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, WeaponPlatform, 0, "WsfPlatform", "")
{
   WsfPlatform* platformPtr = aObjectPtr->GetWeaponPlatform();
   aReturnVal.SetPointer(new UtScriptRef(platformPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, WeaponPlatformName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetWeaponPlatformName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, WeaponLocation, 0, "WsfGeoPoint", "")
{
   double       locationWCS[3];
   WsfPlatform* platformPtr = aObjectPtr->GetWeaponPlatform();
   aObjectPtr->GetWeaponLocationWCS(platformPtr, locationWCS);
   WsfGeoPoint* pointPtr = new WsfGeoPoint(locationWCS);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, WeaponLocationAtLaunch, 0, "WsfGeoPoint", "")
{
   double locationWCS[3];
   aObjectPtr->GetWeaponLocationAtLaunchWCS(locationWCS);
   WsfGeoPoint* pointPtr = new WsfGeoPoint(locationWCS);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, WeaponLocationECS, 0, "Vec3", "")
{
   UtVec3d weaponLocationECS;
   aObjectPtr->GetWeaponLocationECS(weaponLocationECS.GetData());
   aReturnVal.SetPointer(UtScriptVec3::Create(weaponLocationECS));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, WeaponVelocityECS, 0, "Vec3", "")
{
   UtVec3d weaponVelocityECS;
   aObjectPtr->GetWeaponVelocityECS(weaponVelocityECS.GetData());
   aReturnVal.SetPointer(UtScriptVec3::Create(weaponVelocityECS));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, TargetPlatform, 0, "WsfPlatform", "")
{
   WsfPlatform* platformPtr = SIMULATION->GetPlatformByIndex(aObjectPtr->GetTargetPlatformIndex());
   aReturnVal.SetPointer(new UtScriptRef(platformPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, TargetPlatformName, 0, "string", "")
{
   aReturnVal.SetString(SIMULATION->GetPlatformNameId(aObjectPtr->GetTargetPlatformIndex()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, TargetOffsetName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetIntendedTargetOffsetName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, TargetLocation_1, 0, "WsfGeoPoint", "")
{
   double       locationWCS[3];
   WsfPlatform* platformPtr = SIMULATION->GetPlatformByIndex(aObjectPtr->GetTargetPlatformIndex());
   aObjectPtr->GetTargetLocationWCS(platformPtr, locationWCS);
   WsfGeoPoint* pointPtr = new WsfGeoPoint(locationWCS);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, TargetLocation_2, 1, "WsfGeoPoint", "WsfPlatform")
{
   double       locationWCS[3] = {0.0, 0.0, 0.0};
   WsfPlatform* platformPtr    = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (platformPtr != nullptr)
   {
      aObjectPtr->GetTargetLocationWCS(platformPtr, locationWCS);
   }
   WsfGeoPoint* pointPtr = new WsfGeoPoint(locationWCS);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, TargetLocationAtLaunch, 0, "WsfGeoPoint", "")
{
   double locationWCS[3];
   aObjectPtr->GetTargetLocationAtLaunchWCS(locationWCS);
   WsfGeoPoint* pointPtr = new WsfGeoPoint(locationWCS);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, TargetTrackId, 0, "WsfTrackId", "")
{
   WsfTrackId  trackId    = aObjectPtr->GetTargetTrackId();
   WsfTrackId* trackIdPtr = new WsfTrackId(trackId);
   aReturnVal.SetPointer(new UtScriptRef(trackIdPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, TargetResult, 0, "string", "")
{
   auto result = aObjectPtr->GetTargetResult();
   aReturnVal.SetString(WsfWeaponEffects::GetResultString(result));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, IncidentalResult, 0, "string", "")
{
   auto result = aObjectPtr->GetIncidentalResult();
   aReturnVal.SetString(WsfWeaponEffects::GetResultString(result));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, ExtendedResult, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetExtendedResult());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, MissDistance_1, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMissDistance(nullptr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, MissDistance_2, 1, "double", "WsfPlatform")
{
   double       missDistance = 0.0;
   WsfPlatform* platformPtr  = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (platformPtr != nullptr)
   {
      missDistance = aObjectPtr->GetMissDistance(platformPtr);
   }
   aReturnVal.SetDouble(missDistance);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, Pk, 0, "double", "")
{
   double            pk         = 0.0;
   WsfWeaponEffects* effectsPtr = aObjectPtr->GetWeaponEffects();
   if (effectsPtr != nullptr)
   {
      pk = effectsPtr->GetPkConstraint();
   }
   aReturnVal.SetDouble(pk);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, PkDegrade, 0, "double", "")
{
   double            pkDegrade  = 0.0;
   WsfWeaponEffects* effectsPtr = aObjectPtr->GetWeaponEffects();
   if (effectsPtr != nullptr)
   {
      pkDegrade = effectsPtr->GetPkDegrade();
   }
   aReturnVal.SetDouble(pkDegrade);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, SetPkDegrade, 1, "void", "double")
{
   WsfWeaponEffects* effectsPtr = aObjectPtr->GetWeaponEffects();
   if (effectsPtr != nullptr)
   {
      effectsPtr->SetPkDegrade(aVarArgs[0].GetDouble());
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, PkDrawn, 0, "double", "")
{
   double            pkDrawn    = 0.0;
   WsfWeaponEffects* effectsPtr = aObjectPtr->GetWeaponEffects();
   if (effectsPtr != nullptr)
   {
      pkDrawn = effectsPtr->GetPkDrawn();
   }
   aReturnVal.SetDouble(pkDrawn);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, IncidentalDamageAllowed, 0, "bool", "")
{
   bool              incidentalDamageAllowed = false;
   WsfWeaponEffects* effectsPtr              = aObjectPtr->GetWeaponEffects();
   if (effectsPtr != nullptr)
   {
      incidentalDamageAllowed = effectsPtr->GetIncidentalDamageAllowed();
   }
   aReturnVal.SetBool(incidentalDamageAllowed);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, SetIncidentalDamageAllowed, 1, "void", "bool")
{
   WsfWeaponEffects* effectsPtr = aObjectPtr->GetWeaponEffects();
   if (effectsPtr != nullptr)
   {
      effectsPtr->SetIncidentalDamageAllowed(aVarArgs[0].GetBool());
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, SetWeaponEffects, 1, "void", "string")
{
   aObjectPtr->SetWeaponEffects(WsfStringId(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponEngagementClass, WsfWeaponEngagement, Retarget, 1, "bool", "WsfPlatform") // NO_DOC
                                                                                                                 // | DEPRECATED
{
   // This function is deprecated as the engagement target is updated with the platform's 'CurrentTarget'
   aReturnVal.SetBool(true);
}
