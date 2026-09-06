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

#include "WsfExplicitWeapon.hpp"

#include <cassert>
#include <cfloat>
#include <memory>
#include <sstream>

#include "UtDCM.hpp"
#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtScriptContext.hpp"
#include "UtScriptData.hpp"
#include "UtScriptDataPack.hpp"
#include "UtVec3.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfCommandChain.hpp"
#include "WsfEvent.hpp"
#include "WsfLaunchComputer.hpp"
#include "WsfLaunchHandoffData.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMover.hpp"
#include "WsfNavigationErrors.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfStatusMessage.hpp"
#include "WsfStringId.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackManager.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponFuse.hpp"
#include "WsfWeaponObserver.hpp"
#include "script/WsfScriptContext.hpp"

namespace
{
//! Return values from ProcessDeferredLaunchEvent
enum
{
   cDL_CONTINUE = 0,
   cDL_LAUNCHED = 1,
   cDL_ABORTED  = 2,
   cDL_ERROR    = 3
};

class DeferredLaunchEvent : public WsfEvent
{
public:
   DeferredLaunchEvent(double             aSimTime,
                       WsfExplicitWeapon* aFiringWeaponPtr,
                       WsfPlatform*       aRealPlatformPtr,
                       WsfPlatform*       aTempPlatformPtr,
                       WsfTrack*          aLaunchTrackPtr);
   ~DeferredLaunchEvent() override;
   EventDisposition Execute() override;

private:
   WsfExplicitWeapon* mFiringWeaponPtr;
   WsfPlatform*       mRealPlatformPtr;
   WsfPlatform*       mTempPlatformPtr;
   WsfTrack*          mLaunchTrackPtr;
   size_t             mFiringPlatformIndex;
};

//! A dummy mover used during deferred launch processing.
class DummyMover : public WsfMover
{
public:
   //! The dummy mover should have the same spatial domain of the mover it's replacing
   DummyMover(const WsfMover* aMoverPtr, const WsfScenario& aScenario)
      : WsfMover(aScenario)
   {
      if (aMoverPtr != nullptr)
      {
         mSpatialDomain = aMoverPtr->GetSpatialDomain();
      }
   }
   WsfMover*        Clone() const override { return new DummyMover(this, GetScenario()); }
   void             Update(double aSimTime) override {}
   WsfSpatialDomain GetSpatialDomain() const override { return mSpatialDomain; }
   WsfSpatialDomain mSpatialDomain;
};
} // namespace

// =================================================================================================
WsfExplicitWeapon::WsfExplicitWeapon(WsfScenario& aScenario)
   : WsfWeapon(aScenario)
   , mContextPtr(new WsfScriptContext(*aScenario.GetScriptContext(), "WsfWeapon", "WEAPON"))
   , mDeferredLaunchDelay(0.0)
   , mDeferredLaunches()
   , mLastIndexLaunched(0)
   , mIdOffset(0)
   , mCommandChains()
   , mIgnoreLaunchPlatformVelocity(false)
   , mRequireInterceptPoint(false)
   , mRequireLoftAngle(false)
   , mWeaponToolsGenMode(false)
{
   UtVec3d::Set(mLaunchDeltaV_PCS, 0.0);
}

// =================================================================================================
//! Copy constructor (for Clone()).
// protected
WsfExplicitWeapon::WsfExplicitWeapon(const WsfExplicitWeapon& aSrc)
   : WsfWeapon(aSrc)
   , mContextPtr(new WsfScriptContext(*aSrc.mContextPtr))
   , mDeferredLaunchDelay(aSrc.mDeferredLaunchDelay)
   , mDeferredLaunches()
   , mLastIndexLaunched(0)
   , mIdOffset(aSrc.mIdOffset)
   , mCommandChains(aSrc.mCommandChains)
   , mIgnoreLaunchPlatformVelocity(aSrc.mIgnoreLaunchPlatformVelocity)
   , mRequireInterceptPoint(aSrc.mRequireInterceptPoint)
   , mRequireLoftAngle(aSrc.mRequireLoftAngle)
   , mWeaponToolsGenMode(aSrc.mWeaponToolsGenMode)
{
   UtVec3d::Set(mLaunchDeltaV_PCS, aSrc.mLaunchDeltaV_PCS);
}

// =================================================================================================
// virtual
WsfExplicitWeapon::~WsfExplicitWeapon()
{
   delete mContextPtr;
}

// =================================================================================================
//! Standard Clone() method.
// virtual
WsfWeapon* WsfExplicitWeapon::Clone() const
{
   return new WsfExplicitWeapon(*this);
}

// =================================================================================================
// virtual
bool WsfExplicitWeapon::Initialize(double aSimTime)
{
   bool ok = WsfWeapon::Initialize(aSimTime);

   // Initialize the script system.
   ok &= mContextPtr->Initialize(aSimTime, GetPlatform(), this);
   if (GetScenario().FindType("platform_type", GetLaunchedPlatformType()) == nullptr)
   {
      auto out = ut::log::error() << "Launched platform type is not defined.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Weapon: " << GetName();
      out.AddNote() << "Launched Platform Type: " << GetLaunchedPlatformType();
      ok = false;
   }

   return ok;
}

// =================================================================================================
// virtual
bool WsfExplicitWeapon::Initialize2(double aSimTime)
{
   bool ok = WsfWeapon::Initialize2(aSimTime);
   ok &= mContextPtr->Initialize2(aSimTime);
   return ok;
}

// =================================================================================================
// virtual
bool WsfExplicitWeapon::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "launch_delta_v")
   {
      std::string units;
      aInput.ReadValue(mLaunchDeltaV_PCS[0]);
      aInput.ReadValue(mLaunchDeltaV_PCS[1]);
      aInput.ReadValue(mLaunchDeltaV_PCS[2]);
      aInput.ReadValue(units);
      double multiplier = aInput.ConvertValue(1.0, units, UtInput::cSPEED);
      UtVec3d::Multiply(mLaunchDeltaV_PCS, multiplier);
   }
   else if (command == "ignore_launch_platform_velocity")
   {
      aInput.ReadValue(mIgnoreLaunchPlatformVelocity);
   }
   else if ((command == "commander") || (command == "command_chain"))
   {
      std::string commandChainName(WsfCommandChain::GetDefaultName());
      std::string commanderName;
      if (command == "command_chain")
      {
         aInput.ReadValue(commandChainName);
      }
      aInput.ReadValue(commanderName);
      CommandChain commander;
      commander.mCommandChainName = commandChainName;
      commander.mCommanderName    = commanderName;
      mCommandChains.push_back(commander);
   }
   else if (command == "dis_entity_id_offset")
   {
      aInput.ReadValue(mIdOffset);
   }
   else if (command == "require_intercept_point")
   {
      mRequireInterceptPoint = true;
   }
   else if (command == "require_loft_angle")
   {
      mRequireLoftAngle = true;
   }
   else if (command == "deferred_launch_delay") // NOWIKI - TESTING
   {
      // DO NOT DOCUMENT THIS OPTION. It exists simply for exercising the deferred launch capability.
      aInput.ReadValueOfType(mDeferredLaunchDelay, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mDeferredLaunchDelay, 0.0);
   }
   else if (mContextPtr->ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = WsfWeapon::ProcessInput(aInput);
   }

   return myCommand;
}

// =================================================================================================
// virtual
void WsfExplicitWeapon::SetPlatform(WsfPlatform* aPlatformPtr)
{
   WsfWeapon::SetPlatform(aPlatformPtr);
   // Set the context's parent as soon as possible.  This allows scripts compiled later to use the parent's variables.
   mContextPtr->SetParent(&aPlatformPtr->GetScriptContext());
}

// =================================================================================================
// virtual
void WsfExplicitWeapon::TurnOff(double aSimTime)
{
   WsfWeapon::TurnOff(aSimTime);
   mDeferredLaunches.clear();
}

// =================================================================================================
//! Get the velocity vector with which the weapon is ejected from the host platform.
//! The velocity vector is in the PART (or PCS) frame and will be transformed into the
//! host platform frame and then added to the host platform's current velocity to determine
//! the initial velocity vector of the weapon platform.
void WsfExplicitWeapon::GetLaunchDeltaV(double aLaunchDeltaV[3]) const
{
   UtVec3d::Set(aLaunchDeltaV, mLaunchDeltaV_PCS);
}

// =================================================================================================
//! Set the launch velocity relative to the launching part (See GetLaunchDeltaV).
void WsfExplicitWeapon::SetLaunchDeltaV(const double aLaunchDeltaV[3])
{
   UtVec3d::Set(mLaunchDeltaV_PCS, aLaunchDeltaV);
}

// =================================================================================================
//! Calculate the motion state initial conditions of the launched weapon.
//! Currently, this method assumes a 3-degree of freedom flyout, and so
//! sets the resulting NED roll angle to zero.
// protected virtual
void WsfExplicitWeapon::SetLaunchState(double aSimTime, WsfPlatform* aWeaponPlatformPtr, const FireOptions& /*aSettings*/)
{
   // LOCATION

   double weaponLocWCS[3];
   WsfArticulatedPart::UpdatePosition(aSimTime);
   WsfArticulatedPart::GetLocationWCS(weaponLocWCS);
   aWeaponPlatformPtr->SetLocationWCS(weaponLocWCS);

   // VELOCITY

   // Convert the ejection velocity from PCS to WCS.
   double weaponVelWCS[3];
   WsfArticulatedPart::ConvertPCSVectorToWCS(weaponVelWCS, mLaunchDeltaV_PCS);

   if (!mIgnoreLaunchPlatformVelocity)
   {
      // Get the launching platforms velocity in WCS
      double launcherVelWCS[3];
      GetPlatform()->GetVelocityWCS(launcherVelWCS);

      // The resulting weapon velocity is just the vector sum.
      UtVec3d::Add(weaponVelWCS, launcherVelWCS, weaponVelWCS);
   }

   // If the resulting velocity is close to zero then set it to something else.
   if (UtVec3d::Magnitude(weaponVelWCS) < 1.0)
   {
      // Resulting velocity is zero.  We have to give it something or we divide by zero.
      double ejectionVelPCS[3] = {10.0, 0.0, 0.0};
      if (UtVec3d::Magnitude(mLaunchDeltaV_PCS) > 1.0)
      {
         UtVec3d::Set(ejectionVelPCS, mLaunchDeltaV_PCS);
      }
      WsfArticulatedPart::ConvertPCSVectorToWCS(weaponVelWCS, ejectionVelPCS);
   }

   aWeaponPlatformPtr->SetVelocityWCS(weaponVelWCS);

   // ORIENTATION

   // Align using the weapon's orientation on the platform and the firing platform's orientation.
   double psiWCS;
   double thetaWCS;
   double phiWCS;

   GetOrientationWCS(psiWCS, thetaWCS, phiWCS);
   aWeaponPlatformPtr->SetOrientationWCS(psiWCS, thetaWCS, phiWCS);

   // ACCELERATION

   double weaponAclWCS[3] = {0.0, 0.0, 0.0};
   aWeaponPlatformPtr->SetAccelerationWCS(weaponAclWCS);

   // NAVIGATION ERRORS

   WsfNavigationErrors* lauNavErrorPtr = GetPlatform()->GetComponent<WsfNavigationErrors>();
   WsfNavigationErrors* wpnNavErrorPtr = aWeaponPlatformPtr->GetComponent<WsfNavigationErrors>();
   if ((lauNavErrorPtr != nullptr) && (wpnNavErrorPtr != nullptr))
   {
      wpnNavErrorPtr->Handoff(aSimTime, lauNavErrorPtr);
   }

   if (DebugEnabled())
   {
      auto   out = ut::log::debug() << "Weapon state at launch:";
      double lat;
      double lon;
      double alt;
      aWeaponPlatformPtr->GetLocationLLA(lat, lon, alt);
      { // RAII block
         auto loc = out.AddNote() << "Location:";
         loc.AddNote() << "Lat: " << lat;
         loc.AddNote() << "Lon: " << lon;
         loc.AddNote() << "Alt: " << alt;
      }
      double heading;
      double pitch;
      double roll;
      double weaponVelNED[3];
      aWeaponPlatformPtr->GetVelocityNED(weaponVelNED);
      { // RAII block
         auto vel = out.AddNote() << "Velocity:";
         vel.AddNote() << "North: " << weaponVelNED[0];
         vel.AddNote() << "East: " << weaponVelNED[1];
         vel.AddNote() << "Down: " << weaponVelNED[2];
      }
      aWeaponPlatformPtr->GetOrientationNED(heading, pitch, roll);
      { // RAII block
         auto orient = out.AddNote() << "Orientation:";
         orient.AddNote() << "Heading: " << heading * UtMath::cDEG_PER_RAD << " deg";
         orient.AddNote() << "Pitch: " << pitch * UtMath::cDEG_PER_RAD << " deg";
         orient.AddNote() << "Roll: " << roll * UtMath::cDEG_PER_RAD << " deg";
      }
   }

   // Create a launch handoff object and populate with data about the launch platform.
   // This allows the launched platform to have access to all of the data that went in to the above.
   // This is sometimes necessary when the mover on the launched platform needs the data (such as
   // the case a vertically launched rocket for launch-to-orbit).
   WsfLaunchHandoffData& handoffData = WsfLaunchHandoffData::FindOrCreate(*aWeaponPlatformPtr);
   handoffData.SetLaunchPlatformData(nullptr, this);
   handoffData.SetLaunchDeltaV(mLaunchDeltaV_PCS);
   handoffData.SetLaunchComputerData(GetLaunchComputer());
}

// =================================================================================================
// virtual
void WsfExplicitWeapon::AbortSalvo(double aSimTime, const WsfTrackId& aTrackId)
{
   // Invoke the base class to abort any applicable launch requests that are pending.
   WsfWeapon::AbortSalvo(aSimTime, aTrackId);

   // Abort any applicable deferred launch that is in progress.
   //
   // NOTE: All of the data referred to in DeferredLaunch (platforms and track) are owned by the
   //       DeferredLaunchEvent and will be deleted when it is deleted.

   for (size_t i = 0; i < mDeferredLaunches.size();)
   {
      DeferredLaunch dl             = mDeferredLaunches[i];
      WsfTrack*      launchTrackPtr = dl.mLaunchTrackPtr;
      if (aTrackId.IsNull() || ((launchTrackPtr != nullptr) && (launchTrackPtr->GetTrackId() == aTrackId)))
      {
         mDeferredLaunches.erase(mDeferredLaunches.begin() + i);
         WeaponFireAborted(aSimTime, launchTrackPtr, 1);
         WsfObserver::WeaponFireAborted(GetSimulation())(aSimTime, this, launchTrackPtr, 1);
      }
      else
      {
         ++i;
      }
   }
}

// =================================================================================================
WsfWeapon::FireResult WsfExplicitWeapon::Fire(double aSimTime, const FireTarget& aTarget, const FireOptions& aSettings)
{
   WsfPlatform* wpnPlatformPtr = nullptr;

   auto out = ut::log::debug();
   if (DebugEnabled())
   {
      out << "Attempting launch of a weapon:";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Weapon: " << GetName();
      out.AddNote() << "Type: " << GetLaunchedPlatformType();
   }

   // Must have the following valid before a weapon will be launched:
   // 1.  Inventory non-zero.
   // 2.  Enough elapsed time from the last launch.
   // 3.  A non-null cloned weapon platform to launch.

   if (IsReloading() && InhibitWhileReloading())
   {
      if (DebugEnabled())
      {
         out.AddNote() << "Unable to fire because a reload is in progress.";
      }
   }
   else if (GetQuantityRemaining() <= 0.0)
   {
      if (DebugEnabled())
      {
         out.AddNote() << "Unable to fire because no weapon rounds remain.";
      }
   }
   else if (GetTimeLastFired() + GetFiringInterval() > aSimTime)
   {
      if (DebugEnabled())
      {
         out.AddNote() << "Unable to fire because the launch interval has not elapsed.";
      }
   }
   else if (GetDamageFactor() >= 1.0)
   {
      if (DebugEnabled())
      {
         out.AddNote() << "Unable to fire because the DamageFactor is 1.0.";
      }
   }
   else
   {
      wpnPlatformPtr =
         dynamic_cast<WsfPlatform*>(GetScenario().CloneType("platform_type", GetLaunchedPlatformType().GetString()));
      if (wpnPlatformPtr == nullptr)
      {
         if (DebugEnabled())
         {
            auto note = out.AddNote() << "Could not clone a weapon platform of specified type.";
            note.AddNote() << "Platform: " << GetPlatform()->GetName();
            note.AddNote() << "Weapon: " << GetName();
            note.AddNote() << "Launched Platform Type: " << GetLaunchedPlatformType();
         }
      }
   }

   if (wpnPlatformPtr != nullptr)
   {
      return FireP(aSimTime, aTarget, aSettings, wpnPlatformPtr);
   }

   return WsfWeapon::FireResult{};
}

WsfWeapon::FireResult WsfExplicitWeapon::FireP(double             aSimTime,
                                               const FireTarget&  aTarget,
                                               const FireOptions& aSettings,
                                               WsfPlatform*       aWeaponPlatformPtr)
{
   WsfWeapon::FireResult result;

   // Give the weapon a name...
   // Note: the second part of the name used to be the type of the launched platform, however this
   // didn't guarantee a unique name. If the user had two weapons on the platform and they both had
   // the same launched platform type, the first launch of the second weapon would result in a
   // duplicate platform name.

   std::string weaponName = aSettings.mWeaponPlatformName;
   if (weaponName.empty())
   {
      std::ostringstream oss;
      int                quantityUsed = static_cast<int>(GetTotalQuantityUsed()) + (int)mDeferredLaunches.size();
      oss << GetPlatform()->GetName() << '_' << GetName() << '_' << (quantityUsed + 1);
      weaponName = oss.str();
   }
   aWeaponPlatformPtr->SetName(weaponName);

   // Propagate the side (team) of the launching platform to the weapon
   aWeaponPlatformPtr->SetSide(GetPlatform()->GetSideId());

   // Add the command chains to which the launched platform is a member.
   InitializeCommandChains(aSimTime, aWeaponPlatformPtr);

   // Set the motion state of the Weapon:
   SetLaunchState(aSimTime, aWeaponPlatformPtr, aSettings);

   // A good track to launch toward is either a valid input track or a current
   // target in the launching platform.  A track is not required, however
   // (in particular, a dumb bomb shouldn't require a track).
   const WsfTrack* wpnTgtTrackPtr = aTarget.mTrackPtr;
   if (wpnTgtTrackPtr == nullptr)
   {
      wpnTgtTrackPtr = GetPlatform()->GetTrackManager().GetCurrentTarget();
   }
   std::unique_ptr<WsfTrack> trackPtr(CreateTargetTrack(aSimTime, wpnTgtTrackPtr));

   size_t     targetIndex = 0;
   WsfTrackId targetTrackId;
   bool       haveInterceptPoint = false;
   bool       haveLoftAngle      = false;
   if (trackPtr != nullptr)
   {
      WsfLaunchComputer* computerPtr = GetLaunchComputer();
      if (computerPtr != nullptr)
      {
         computerPtr->CopyResultsToTrack(*trackPtr);
         double      dummy;
         WsfGeoPoint dummyPoint;
         haveInterceptPoint = WsfLaunchComputer::GetInterceptPoint(*trackPtr, dummyPoint);
         haveLoftAngle      = WsfLaunchComputer::GetLoftAngle(*trackPtr, dummy);
      }

      aWeaponPlatformPtr->GetTrackManager().SetCurrentTarget(aSimTime, *trackPtr);
      targetIndex   = trackPtr->GetTargetIndex();
      targetTrackId = trackPtr->GetTrackId();
   }

   // Make sure the launch computer has provided sufficient data.
   if ((mRequireInterceptPoint && (!haveInterceptPoint)) || (mRequireLoftAngle && (!haveLoftAngle)))
   {
      { // RAII block
         auto out = ut::log::error() << "Missing or incomplete launch computer solution - launch suppressed.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Weapon: " << GetName();
      }
      delete aWeaponPlatformPtr;
      return result;
   }

   // If weapon_effects were defined then this is a weapon launch and not just a release of a non-weapon
   // entity (an 'explicit weapon' can be used to drop off stages in a missile, launch aircraft from a
   // base, etc.) If it is really a weapon then we create the engagement structure and attach it to the
   // launched platform for effectiveness calculations when the weapon terminates.

   WsfWeaponEngagement* engagementPtr = nullptr;

   // We are querying the platform for some information it cannot provide unless it has access to the simulation
   aWeaponPlatformPtr->AssignToSimulation(GetSimulation());

   // If no weapon effect was specified for the weapon itself,
   // then try to get one from the weapon platform itself.
   WsfStringId weaponEffectsType(GetWeaponEffectsType());
   if (weaponEffectsType.IsNull())
   {
      weaponEffectsType = WsfWeaponEffects::GetWeaponEffectsType(*aWeaponPlatformPtr);
   }

   if (!weaponEffectsType.IsNull())
   {
      engagementPtr =
         new WsfWeaponEngagement(aSimTime, GetSimulation(), this, targetTrackId, targetIndex, weaponEffectsType);
      if (engagementPtr != nullptr)
      {
         // Push the IFC enable flag onto the weapon engagement; GetIFCEnabled() set through the input file
         engagementPtr->SetIFCEnabled(GetIFCEnabled());
      }

      WsfWeaponEngagement::Assign(*aWeaponPlatformPtr, engagementPtr);
      if (trackPtr != nullptr)
      {
         double targetLocWCS[3];
         if (trackPtr->GetExtrapolatedLocationWCS(aSimTime, targetLocWCS))
         {
            engagementPtr->SetInitialTargetLocationWCS(targetLocWCS);
         }
      }
   }

   // If the on_create script exists, call it.
   UtScript* scriptPtr = mContextPtr->FindScript("on_create");
   if (scriptPtr != nullptr)
   {
      // on_create takes two arguments, the weapon platform and the track it is to fire at.
      UtScriptData       ret;
      UtScriptDataPacker args{mContextPtr->GetTypes()};

      args << aWeaponPlatformPtr << trackPtr.get();
      mContextPtr->ExecuteScript(aSimTime, scriptPtr, ret, args);
   }

   // Add a hook for derived classes to initialize the platform
   WeaponPlatformUserInitialize(aSimTime, aWeaponPlatformPtr, trackPtr.get());

   // Process either a deferred or immediate launch.

   bool useDeferredLaunching = false;
   if ((engagementPtr != nullptr) && (aWeaponPlatformPtr->GetMover() != nullptr))
   {
      useDeferredLaunching =
         (UseDeferredLaunching() /*wpnPlatformPtr->GetMover()->UseDeferredLaunching()*/ || (mDeferredLaunchDelay > 0.0));
   }
   if (useDeferredLaunching)
   {
      mLastIndexLaunched = 0;

      // Create the temporary platform that will represent the weapon platform during
      // the time the weapon is in the deferred launch state. The temporary platform
      // will only have a mover. The real weapon platform cannot be used because it
      // it may contain parts that require the platform index during initialization.

      WsfPlatform* tempPlatformPtr = new WsfPlatform(GetScenario());
      // Copy the initial conditions to the temporary platform
      static_cast<UtEntity&>(*tempPlatformPtr) = static_cast<UtEntity&>(*aWeaponPlatformPtr);
      tempPlatformPtr->SetName(aWeaponPlatformPtr->GetName());
      tempPlatformPtr->SetType(aWeaponPlatformPtr->GetType());
      tempPlatformPtr->SetSide(aWeaponPlatformPtr->GetSide());

      // Transfer the mover from the real weapon platform to the temporary weapon platform.
      // It will be transferred back once the weapon starts moving.

      WsfMover* realMoverPtr;
      WsfMover* tempMoverPtr =
         new DummyMover(aWeaponPlatformPtr->GetMover(), GetScenario()); // SwapMover requires a new mover
      aWeaponPlatformPtr->SwapMover(aSimTime, tempMoverPtr, realMoverPtr);
      tempPlatformPtr->SetMover(realMoverPtr);
      tempPlatformPtr->AssignToSimulation(GetSimulation());

      // Propagate other key pieces that are probably needed by the mover.
      if (trackPtr != nullptr)
      {
         tempPlatformPtr->GetTrackManager().SetCurrentTarget(aSimTime, *trackPtr);
      }
      if (engagementPtr != nullptr)
      {
         // There is currently no facility to transfer the engagement. Make a separate copy...
         WsfWeaponEngagement* tempEngagementPtr =
            new WsfWeaponEngagement(aSimTime, GetSimulation(), this, targetTrackId, targetIndex, weaponEffectsType);
         if (tempEngagementPtr != nullptr)
         {
            // push the IFC enable flag onto the weapon engagement; GetIFCEnabled() set through the input file
            tempEngagementPtr->SetIFCEnabled(GetIFCEnabled());
         }
         // Copy the real serial number to the temp object.
         WsfWeaponEngagement::CopySerialNumber(engagementPtr, tempEngagementPtr);
         WsfWeaponEngagement::Assign(*tempPlatformPtr, tempEngagementPtr);
      }

      if (tempPlatformPtr->Initialize(GetSimulation(), aSimTime))
      {
         // We must have a persistent copy of the track for bookkeeping purposes.
         WsfTrack* copiedTrackPtr = nullptr;
         if (trackPtr != nullptr)
         {
            copiedTrackPtr = trackPtr->Clone();
         }
         GetSimulation()->AddEvent(ut::make_unique<DeferredLaunchEvent>(aSimTime + mDeferredLaunchDelay,
                                                                        this,
                                                                        aWeaponPlatformPtr,
                                                                        tempPlatformPtr,
                                                                        copiedTrackPtr));
         mDeferredLaunches.emplace_back(aWeaponPlatformPtr, tempPlatformPtr, copiedTrackPtr);
         result.mSuccess = true;
      }
      else
      {
         { // RAII block
            auto out = ut::log::error() << "Failed to initialize deferred launch weapon platform.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Weapon: " << GetName();
         }
         delete aWeaponPlatformPtr;
         aWeaponPlatformPtr = nullptr;
         delete tempPlatformPtr;
         tempPlatformPtr = nullptr;
         result.mSuccess = false;
      }
   }
   else if (AddWeaponToSimulation(aSimTime, aWeaponPlatformPtr, trackPtr.get(), aSettings))
   {
      mLastIndexLaunched = aWeaponPlatformPtr->GetIndex();
      result.mSuccess    = true;
   }

   if (result.mSuccess)
   {
      result.mEngagementPtr = engagementPtr;
   }

   return result;
}

// =================================================================================================
// virtual
double WsfExplicitWeapon::GetQuantityRemaining() const
{
   // This override of the base class implementation includes considerations for weapons
   // that are in the 'deferred launch state' (i.e. they have been 'fired' but are being held
   // on the rail/pad by the weapon mover.

   return std::max(0.0, WsfWeapon::GetQuantityRemaining() - mDeferredLaunches.size());
}

// =================================================================================================
//! Process a mover update request for a deferred launch.
//! This is called by DeferredLaunchEvent to attempt to move the deferred launch process along.
//
//! @param aSimTime         The current simulation time.
//! @param aRealPlatformPtr The pointer to the 'real' weapon platform.
//! @param aTempPlatformPtr The pointer to the temporary weapon platform.
//!
//! @returns DLE_LAUNCHED   The weapon was successfully launched. The caller should delete
//!                         the temporary platform.
//! @returns DLE_CONTINUE   The deferred launch is still in progress.
//! @returns DLE_ABORT      The deferred launch was aborted (either the missile declared it
//!                         shouldn't be launched or an AbortSalvo had canceled the request).
//!                         The caller should delete both platforms.
//! @returns DLE_ERROR      The launch should have taken place, but the real weapon platform
//!                         could not be added to the simulation. The caller should delete
//!                         only the temporary platform. The real platform has been deleted.
// virtual
int WsfExplicitWeapon::ProcessDeferredLaunchEvent(double aSimTime, WsfPlatform* aRealPlatformPtr, WsfPlatform* aTempPlatformPtr)
{
   int status = cDL_ABORTED; // Assume the launch was aborted or canceled

   // Find the real weapon platform in the deferred launch list...
   // It won't be in the list if it has been canceled.
   DeferredLaunch* deferredLaunchPtr   = nullptr;
   int             deferredLaunchIndex = -1;
   for (size_t i = 0; i < mDeferredLaunches.size(); ++i)
   {
      if (mDeferredLaunches[i].mRealPlatformPtr == aRealPlatformPtr)
      {
         deferredLaunchIndex = (int)i;
         deferredLaunchPtr   = &mDeferredLaunches[i];
         break;
      }
   }
   if (deferredLaunchPtr == nullptr)
   {
      return cDL_ABORTED;
   }
   if (deferredLaunchPtr != nullptr)
   {
      WsfMover* moverPtr = aTempPlatformPtr->GetMover();
      moverPtr->Update(aSimTime);
      if (moverPtr->IsPaused())
      {
         // The weapon is still on the pad/rail. If the damage factor is zero then the
         // launch is still possible, but if it is non-zero then the launch has been aborted.

         status = cDL_CONTINUE; // Assume the launch has not been aborted
         if (aTempPlatformPtr->GetDamageFactor() != 0.0)
         {
            // A damage factor of non-zero indicates the launch was aborted.
            WsfTrack* launchTrackPtr = deferredLaunchPtr->mLaunchTrackPtr;
            WeaponFireAborted(aSimTime, launchTrackPtr, 1);
            WsfObserver::WeaponFireAborted(GetSimulation())(aSimTime, this, launchTrackPtr, 1);
            status = cDL_ABORTED;
         }
      }
      else
      {
         // The weapon has left the rail. Now we can add the real platform to the simulation.

         // Copy the kinematic data from the temporary platform to the real platform.
         // This may be ever so slightly different from the initial conditions because the
         // platform may have started moving.

         static_cast<UtEntity&>(*aRealPlatformPtr) = static_cast<UtEntity&>(*aTempPlatformPtr);

         // The update interval from the real mover that is stored in the temporary platform
         // must be propagated to the temporary mover in the real platform so the simulation
         // executive can schedule mover update events (if needed). Otherwise the executive
         // will schedule the event based on whatever was stored in the temporary mover.
         //
         // The real mover cannot simply be restored to the real platform before adding the real
         // platform to the simulation because that would cause it to be reinitialized.

         double updateInterval = aTempPlatformPtr->GetMover()->GetUpdateInterval();
         aRealPlatformPtr->GetMover()->SetUpdateInterval(updateInterval);

         status                   = cDL_ERROR; // assume unable to add real platform to simulation
         WsfTrack* launchTrackPtr = deferredLaunchPtr->mLaunchTrackPtr;
         if (AddWeaponToSimulation(aSimTime, aRealPlatformPtr, launchTrackPtr, 0))
         {
            status = cDL_LAUNCHED;
            // Now we can swap to real mover from the temporary platform back to the real platform.
            WsfMover* realMoverPtr;
            WsfMover* tempMoverPtr = new DummyMover(nullptr, GetScenario()); // SwapMover requires a new mover
            aTempPlatformPtr->SwapMover(aSimTime, tempMoverPtr, realMoverPtr);
            aRealPlatformPtr->SetMover(realMoverPtr);
            delete tempMoverPtr; // delete the temporary mover that was in the real platform
            GetSimulation()->TurnPartOn(aSimTime, aRealPlatformPtr->GetMover());
         }
      }

      // Delete the deferred launch item if the request isn't to be continued.
      if (status != cDL_CONTINUE)
      {
         mDeferredLaunches.erase(mDeferredLaunches.begin() + deferredLaunchIndex);
         if ((status == cDL_ABORTED) || (status == cDL_ERROR))
         {
            ChangeQuantity(-1.0);
         }
      }
   }
   return status;
}

// ITAR - START
// ITAR TODO - This functionality was refactored from the WsfMover to meet EAR99 requirements, may need moved again
//  ================================================================================================
//! Should the 'deferred launching' method be used for explicit weapon launching?
//!
//! Some movers contain logic that may delay or abort the launch of a weapon platform.
//! Rather than creating platforms and publishing them to the world, a mover may support the
//! concept of 'deferred launching' whereby the WsfExplicitWeapon and the mover work together
//! to add the platform to the simulation only when the mover has committed the weapon to launch.
//!
//! An example is the (C)SIMS mover, which enforces a launch computer hold if the target position
//! does not meet certain constraints. In theory, a user should not launch a weapon in such a case,
//! but it is almost impossible (and counter-productive) to replicate the SIMS logic in the WSF
//! launch decision making code. We could launch the weapon and let it fail, but this could cause a
//! a lot of extraneous platforms to be created, lots of unnecessary and confusing DIS Fire/Detonate
//! PDUs, and cause us to run out of DIS entity ID's.
//!
//! The deferred launch protocol is as follows:
//! - WsfExplicitWeapon instantiates the platform (but does not add it to the simulation),
//!   initializes it and periodically calls the mover.
//! - The mover returns WsfMover::IsPaused() == true as long as the weapon is not committed.
//! - If the mover wishes to abort the launch, it calls WsfPlatform::SetDamageFactor(<non-zero>).
//!   (WsfMover::IsPaused() should still be returning true).
//! - The mover signals the launch is commited when it returns WsfMover::IsPaused() == false.
//!   WsfExplicitWeapon then adds the platform to the simulation where it will then be published
//!   to the world.
//!
//! @returns true if deferred launching should be used or false if the standard launching
//! protocol should be used.
// virtual
bool WsfExplicitWeapon::UseDeferredLaunching() const
{
   return false;
}
// ITAR - END

// =================================================================================================
//! Add the weapon platform to the simulation.
//!
//! This routine is called to add the weapon platform for either an immediate launch or
//! for a deferred launch when it has been determined that the weapon has actually been launched.
//!
//! @param aSimTime           The current simulation time.
//! @param aWeaponPlatformPtr Pointer to the weapon platform to be added.
//! @param aLaunchingTrackPtr Pointer to the track being fired against.
//! @param aSettings          Options that Fire was called with.
//!
//! @returns true if successful or false if not.
// protected virtual
bool WsfExplicitWeapon::AddWeaponToSimulation(double             aSimTime,
                                              WsfPlatform*       aWeaponPlatformPtr,
                                              const WsfTrack*    aLaunchingTrackPtr,
                                              const FireOptions& aSettings)
{
   bool success = false;

   // Give the weapon a new dis entity id if id_offset was set as input.
   // Some external simulations need to know what the missile's dis id is before
   // the simulation begins.

   if (mIdOffset > 0)
   {
      if (aSettings.mWeaponId != 0)
      {
         if (GetState() != WsfWeapon::cWEAPON_STATE_READY)
         {
            return false;
         }
         else
         {
            // Reinitialize for next launch, since the pointer to the weapon on this level
            // is for all remaining weapons.
            SetState(WsfWeapon::cWEAPON_STATE_UNALLOCATED);
         }
      }
   }

   WsfWeaponEngagement* weaponEngagementPtr = WsfWeaponEngagement::Find(*aWeaponPlatformPtr);

   WsfObserver::WeaponPlatformPendingAdd(
      GetSimulation())(aSimTime, aWeaponPlatformPtr, weaponEngagementPtr, aLaunchingTrackPtr);

   if (aWeaponPlatformPtr->IsExternallyControlled() || GetSimulation()->AddPlatform(aSimTime, aWeaponPlatformPtr))
   {
      if (weaponEngagementPtr != nullptr)
      {
         // update weapon status
         WsfTrackId targetId;
         if (aLaunchingTrackPtr != nullptr)
         {
            targetId = aLaunchingTrackPtr->GetTrackId();
         }
         auto it = mWeaponStatusMap.find(targetId);
         if (it == mWeaponStatusMap.end())
         {
            // create map entry if doesn't exist yet (WsfWeapon::FireSalvo() was not used, just a direct WsfExplicitWeapon::Fire())
            it = mWeaponStatusMap.emplace(targetId, WeaponStatus()).first;
         }
         if (it->second.mRoundsPending > 0)
         {
            it->second.mRoundsPending--;
         }
         it->second.mRoundsActive++;
         it->second.mTimeWeaponLastFired = aSimTime;
         // save off a new entry for active weapon platforms too
         ActiveWeapon active(weaponEngagementPtr->GetWeaponPlatformIndex(), weaponEngagementPtr->GetSerialNumber());
         it->second.mActiveWeaponList.push_back(active);

         // mRoundsActive == mActiveWeaponList.size()

         WsfObserver::WeaponFired(GetSimulation())(aSimTime, weaponEngagementPtr, aLaunchingTrackPtr);
      }

      // Send the weapon fired status message to interested parties.
      WsfStatusMessage message("WEAPON_FIRED", GetPlatform());
      message.SetSystemNameId(GetNameId());
      if (aLaunchingTrackPtr != nullptr)
      {
         message.SetRequestId(aLaunchingTrackPtr->GetTrackId());
      }
      message.SetPlatform(aWeaponPlatformPtr);
      SendMessage(aSimTime, message);

      ChangeQuantity(-1.0);
      SetTimeLastFired(aSimTime);
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Weapon successfully fired.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Weapon: " << GetName();
         out.AddNote() << "Weapon Name: " << aWeaponPlatformPtr->GetName();
      }
      success = true;

      ProcessReloading(aSimTime);

      if (aWeaponPlatformPtr->IsExternallyControlled())
      {
         // TODO : is this the right place to clean up the weapon platform that isn't added to the sim?
         GetSimulation()->DeletePlatform(aSimTime, aWeaponPlatformPtr);
      }
   }
   else
   {
      { // RAII block
         auto out = ut::log::error() << "Unable to add weapon to the simulation.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Weapon: " << GetName();
         out.AddNote() << "Weapon Name: " << aWeaponPlatformPtr->GetName();
      }
      WeaponFireAborted(aSimTime, aLaunchingTrackPtr, 1);
      WsfObserver::WeaponFireAborted(GetSimulation())(aSimTime, this, aLaunchingTrackPtr, 1);
      delete aWeaponPlatformPtr;
      success = false;
   }
   return success;
}

// =================================================================================================
//! Set up the command chains in the launched weapon.
// protected
void WsfExplicitWeapon::InitializeCommandChains(double aSimTime, WsfPlatform* aPlatformPtr)
{
   // If no commanders are provided then this platform is a subordinate to the launch platform on every
   // command chain on which the launch platform is a member.
   if (mCommandChains.empty())
   {
      for (WsfComponentList::RoleIterator<WsfCommandChain> iter(*GetPlatform()); !iter.AtEnd(); ++iter)
      {
         WsfCommandChain* newChainPtr = new WsfCommandChain(iter->GetNameId(), GetPlatform()->GetNameId());
         if (!aPlatformPtr->AddComponent(newChainPtr))
         {
            delete newChainPtr;
         }
      }
   }
   else
   {
      // Add the explicit command chains as provided by the user.
      for (CommandChain& commandChain : mCommandChains)
      {
         std::string commanderName = commandChain.mCommanderName.GetString();
         if (commanderName == "SELF")
         {
            // Just pass through "SELF"
         }
         else if (commanderName == "LAUNCHER")
         {
            commanderName = GetPlatform()->GetName();
         }
         else if (commanderName == "LAUNCHER_COMMANDER")
         {
            WsfCommandChain* chainPtr = GetPlatform()->GetComponent<WsfCommandChain>(commandChain.mCommandChainName);
            if (chainPtr == nullptr)
            {
               auto out = ut::log::warning()
                          << "Could not locate command chain for LAUNCHER_COMMANDER during weapon launch.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Platform: " << GetPlatform()->GetName();
               out.AddNote() << "Weapon: " << GetName();
               out.AddNote() << "Command Chain: " << commandChain.mCommandChainName;
               continue;
            }
            commanderName = chainPtr->GetCommanderName();
         }
         else
         {
            // Pass through the supplied platform name.
         }

         WsfCommandChain* newChainPtr = new WsfCommandChain(commandChain.mCommandChainName, commanderName);
         if (!aPlatformPtr->AddComponent(newChainPtr))
         {
            delete newChainPtr;
            auto out = ut::log::warning() << "Failed to add command chain to launched weapon.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Weapon: " << GetName();
            out.AddNote() << "Command Chain: " << commandChain.mCommandChainName;
            out.AddNote() << "Commander: " << commandChain.mCommanderName;
         }
      }
   }
}

// =================================================================================================
//! Allow for user initialization of the weapon's platform.
bool WsfExplicitWeapon::WeaponPlatformUserInitialize(double /* aSimTime */,
                                                     WsfPlatform* /* aWeaponPlatformPtr */,
                                                     const WsfTrack* /* aTrackPtr */)
{
   return true;
}

// =================================================================================================
// Nested class DeferredLaunch.
// =================================================================================================

WsfExplicitWeapon::DeferredLaunch::DeferredLaunch(WsfPlatform* aRealPlatformPtr,
                                                  WsfPlatform* aTempPlatformPtr,
                                                  WsfTrack*    aLaunchTrackPtr)
   : mRealPlatformPtr(aRealPlatformPtr)
   , mTempPlatformPtr(aTempPlatformPtr)
   , mLaunchTrackPtr(aLaunchTrackPtr)
{
}

// =================================================================================================
// Private class DeferredLaunchEvent
// =================================================================================================
namespace
{

DeferredLaunchEvent::DeferredLaunchEvent(double             aSimTime,
                                         WsfExplicitWeapon* aFiringWeaponPtr,
                                         WsfPlatform*       aRealPlatformPtr,
                                         WsfPlatform*       aTempPlatformPtr,
                                         WsfTrack*          aLaunchTrackPtr)
   : WsfEvent(aSimTime)
   , mFiringWeaponPtr(aFiringWeaponPtr)
   , mRealPlatformPtr(aRealPlatformPtr)
   , mTempPlatformPtr(aTempPlatformPtr)
   , mLaunchTrackPtr(aLaunchTrackPtr)
   , mFiringPlatformIndex(aFiringWeaponPtr->GetPlatform()->GetIndex())
{
}

// =================================================================================================
// virtual
DeferredLaunchEvent::~DeferredLaunchEvent()
{
   delete mLaunchTrackPtr;
}

// =================================================================================================
// virtual
WsfEvent::EventDisposition DeferredLaunchEvent::Execute()
{
   int          status            = cDL_ABORTED; // Assume failed launch (aborted, canceled, or launcher killed)
   double       simTime           = GetTime();
   WsfPlatform* firingPlatformPtr = GetSimulation()->GetPlatformByIndex(mFiringPlatformIndex);
   if (firingPlatformPtr != nullptr)
   {
      status = mFiringWeaponPtr->ProcessDeferredLaunchEvent(simTime, mRealPlatformPtr, mTempPlatformPtr);
   }

   EventDisposition disposition = cDELETE; // The event is not rescheduled unless cDL_CONTINUE
   switch (status)
   {
   case cDL_CONTINUE:
      // Weapon is still on the rail/pad - reschedule another update.
      disposition = cRESCHEDULE;
      SetTime(simTime + mTempPlatformPtr->GetMover()->GetUpdateInterval());
      break;
   case cDL_LAUNCHED:
      // The weapon was launched. Delete the temporary platform that was used for deferred launching.
      delete mTempPlatformPtr;
      break;
   case cDL_ABORTED:
      // The weapon launch was aborted or canceled, or the launching platform was deleted.
      // Delete both the real and temporary platforms.
      delete mRealPlatformPtr;
      delete mTempPlatformPtr;
      break;
   case cDL_ERROR:
      // The launch should have been successful, but the addition of the real platform failed.
      // The real platform was deleted, but we have to clean up the temporary platform.
      delete mTempPlatformPtr;
      break;
   default:
      break;
   }
   return disposition;
}

} // end namespace
