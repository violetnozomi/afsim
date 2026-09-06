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

#include "WsfPlatform.hpp"

#include <algorithm>
#include <cassert>
#include <limits>

#include "UtEllipsoidalEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtScriptContext.hpp"
#include "WsfComm.hpp"
#include "WsfCommandChain.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfComponentFactoryList.hpp"
#include "WsfDateTime.hpp"
#include "WsfDeferredInput.hpp"
#include "WsfEnvironment.hpp"
#include "WsfExchange.hpp"
#include "WsfFuel.hpp"
#include "WsfGroup.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMover.hpp"
#include "WsfMoverObserver.hpp"
#include "WsfNavigationErrors.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfScenario.hpp"
#include "WsfSignatureList.hpp"
#include "WsfSimulation.hpp"
#include "WsfSinglePlatformObserver.hpp"
#include "WsfTerrain.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackList.hpp"
#include "WsfTrackManager.hpp"
#include "WsfUtil.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptEvent.hpp"

// =================================================================================================
WsfPlatform::WsfPlatform(const WsfScenario& aScenario)
   : WsfObject()
   , WsfPlatformComponent()
   , WsfPlatformComponentList()
   , WsfUniqueId()
   , UtEntity()
   , mScenario(aScenario)
   , mFlags(0)
   , mScriptContext(*aScenario.GetScriptContext(), {}, "")
   , mTerrainPtr(new wsf::Terrain(wsf::TerrainInterface::Get(aScenario)))
{
   // Created components will have me as their parent
   SetParentOfComponents(this);

   // Register 'this' in addition to PLATFORM
   mScriptContext.GetContext().RegisterThisPointer("WsfPlatform");
}

// =================================================================================================
//! Copy constructor (for Clone()).
// protected
WsfPlatform::WsfPlatform(const WsfPlatform& aSrc)
   : WsfObject(aSrc)
   , WsfPlatformComponent(aSrc)
   , WsfPlatformComponentList(aSrc)
   , WsfUniqueId(aSrc)
   , UtEntity(aSrc)
   , WsfAuxDataEnabled(aSrc)
   , mScenario(aSrc.mScenario)
   , mSimulationPtr(nullptr)
   , mIndex(0)
   , mSide(aSrc.mSide)
   , mIcon(aSrc.mIcon)
   , mMarking(aSrc.mMarking)
   , mSpatialDomain(aSrc.mSpatialDomain)
   , mOnBrokenAction(aSrc.mOnBrokenAction)
   , mCreationTime(aSrc.mCreationTime)
   , mLastUpdateTime(-1.0)
   , mMasterTrackManagerPtr(nullptr)
   , mSignatureListPtr(nullptr)
   , mMoverPtr(nullptr)
   , mFuelPtr(nullptr)
   , mNavErrorPtr(nullptr)
   , mScriptContext(aSrc.mScriptContext)
   , mTerrainPtr(new wsf::Terrain(nullptr))
   , mCategories(aSrc.mCategories)
   , mGroups(aSrc.mGroups)
   , mNextTrackId(aSrc.mNextTrackId)
   , mHeight(aSrc.mHeight)
   , mLength(aSrc.mLength)
   , mWidth(aSrc.mWidth)
   , mEmptyMass(aSrc.mEmptyMass)
   , mFuelMass(aSrc.mFuelMass)
   , mPayloadMass(aSrc.mPayloadMass)
   , mDamageFactor(aSrc.mDamageFactor)
   , mConcealmentFactor(aSrc.mConcealmentFactor)
   , mAppearanceWord(aSrc.mAppearanceWord)
   , mAppearanceWordScriptMask(aSrc.mAppearanceWordScriptMask)
   , mCapabilitiesWord(aSrc.mCapabilitiesWord)
{
   SetParentOfComponents(this); // Existing and future components have me as their parent

   mFlags             = aSrc.mFlags;
   mHasOnUpdateScript = false;
   mIsInitialized     = false;
   mIsBroken          = false;
   mIsDeleted         = false;
   mUpdateAllowed     = false;
   mUpdateLocked      = false;
   mInitPass          = false;

   // Active objects cannot be cloned (i.e.: those that are part of a running simulation).
   // Cloning was designed to work only for 'type objects' (or constituent parts thereof).
   if (aSrc.GetIndex() != 0)
   {
      throw WsfUncloneableException("WsfUncloneableException: object: " + GetName());
   }

   // Clone any other objects that aren't components...

   if (aSrc.mSignatureListPtr != nullptr)
   {
      mSignatureListPtr = new WsfSignatureList(*aSrc.mSignatureListPtr, this);
   }
}

// =================================================================================================
// virtual
WsfPlatform::~WsfPlatform()
{
   UtReferenceTracked::FreeReference();
   if ((mIndex != 0) && (!IsDeleted()))
   {
      { // RAII block
         auto out = ut::log::error()
                    << "Platform is a simulation member and cannot be deleted by directly invoking the destructor.";
         out.AddNote() << "Platform: " << GetName();
         out.AddNote()
            << "Please use the WsfSimulation::DeletePlatform() method to delete platforms from the simulation.";
      }
      assert((mIndex == 0) || IsDeleted());
   }

   mGroups.LeaveAllGroups();

   // Delete the constituent parts. This must be done BEFORE leaving the destructor because some member
   // variables on which components depend may be deleted before the component list. In particular,
   // antenna objects register as observers of platform updates and will detach their observer when
   // being deleted. But the observer list has already been deleted!

   DeleteAllComponents();

   delete mSignatureListPtr;
   delete mTerrainPtr;
}

// ============================================================================
const int* WsfPlatform::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_PLATFORM, cWSF_COMPONENT_NULL};
   return roles;
}

// ============================================================================
void* WsfPlatform::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_PLATFORM)
   {
      return this;
   }
   return nullptr;
}

// =================================================================================================
const char* WsfPlatform::GetScriptClassName() const
{
   return "WsfPlatform";
}

// =================================================================================================
UtScriptContext* WsfPlatform::GetScriptAccessibleContext() const
{
   return &mScriptContext.GetContext();
}

// =================================================================================================
//! Clone a platform.
//!
//! @return A pointer to the new platform.
//! @note The source of the clone should be a platform that has not yet been added
//! to the simulation (i.e.: it should be a platform 'type object')
// virtual
WsfPlatform* WsfPlatform::Clone() const
{
   return new WsfPlatform(*this);
}

// =================================================================================================
//! May be called prior to Initialize() to bind this platform to a simulation.
//! This allows the platform to be used in a limited fashion without actually being in the simulation.
void WsfPlatform::AssignToSimulation(WsfSimulation* aSimulationPtr)
{
   assert((mSimulationPtr == nullptr) || (mSimulationPtr == aSimulationPtr));
   mSimulationPtr = aSimulationPtr;
   mScriptContext.SetParent(&mSimulationPtr->GetScriptContext());
   mTerrainPtr->Initialize(*aSimulationPtr);
   AssignUniqueId(GetSimulation()); // In WsfUniqueId; (needed for LOS checks)
}

// =================================================================================================
//! Complete processing of platform inputs.  Called after all inputs have been processed.
//! @param aScenario The scenario containing the platform
// virtual
void WsfPlatform::CompleteLoad(WsfScenario& aScenario)
{
   // Set the altitude if the user specified AGL. We have to wait until now because the terrain may have not
   // yet been defined when the platform was defined in the input file.
   if (mInputAltAGL)
   {
      wsf::Terrain terrain(wsf::TerrainInterface::Get(aScenario));
      float        terrainHeight;
      double       lat, lon, alt;
      GetLocationLLA(lat, lon, alt);
      terrain.GetElevInterp(lat, lon, terrainHeight);
      SetLocationLLA(lat, lon, alt + terrainHeight);
      mInputAltAGL = false;
   }
}

// =================================================================================================
//! Initialize the platform and all its aggregate parts.
//!
//! @param aSimulationPtr Pointer to the simulation
//! @param aSimTime The current simulation time.
//! @return 'true' if successful or 'false' if some sort of error occurred.
//! @note This method is invoked automatically when a object is added to the WsfSimulation object.
// virtual
bool WsfPlatform::Initialize(WsfSimulation* aSimulationPtr, double aSimTime)
{
   bool ok = true;

   AssignToSimulation(aSimulationPtr);

   // NOTE: In general, this method is called only once, but it may be called again. For instance,
   // it is allowed to call Initialize() without yet adding the platform to the simulation. When the
   // platform is finally added to the simulation with WsfSimulation::AddPlatform(), Initialize()
   // will be called again.

   mCreationTime = WsfRandomVariable(aSimTime, WsfRandomVariable::cNON_NEGATIVE);

   double timeDelta = std::max(0.01, 1.01 * GetSimulation()->GetMinimumMoverTimestep());
   mLastUpdateTime  = aSimTime - timeDelta; // Must be slightly less than current simulation time.

   // Done with second-time initialization items
   if (mIsInitialized)
   {
      mUpdateAllowed = true; // Now updates are allowed.
      return true;
   }

   // Set the reference epoch in UtEntity, to support ECI<->WCS coordinate conversion
   SetECI_ReferenceEpoch(GetSimulation()->GetDateTime().GetStartDateAndTime());

   SetCentralPoint(aSimulationPtr->GetEnvironment().GetCentralPoint());

   // Then update it to the given aSimTime.
   SetTime(aSimTime);

   // If the icon was not specified then assume the type instead
   if (mIcon.IsNull())
   {
      mIcon = GetTypeId();
   }

   // Organize any groups that exist
   ok &= mGroups.Initialize(this);

   // Set the 'this' pointing to this...
   mScriptContext.GetContext().SetThisPointer(this);

   // Initialize the script context and resolve any references to script variables
   ok &= mScriptContext.Initialize(aSimTime, this, nullptr);

   // NOTE: Some variables are resolved (in other places) using the global script context rather than
   // here using the local script context to prevent problems with the order in which objects are initialized.
   // The local context cannot be initialized until AFTER certain operations are done, but some variables
   // that may have references need to have their value known BEFORE that is done.
   WsfScriptContext& globalContext = GetSimulation()->GetScriptContext();

   // creation_time must be initialized here even though it is initialized in InitializeCreationTime.
   // InitializeCreationTime is only called for platforms added at T=0. mCreationTime is set to the actual time
   // of creation (see above), This causes the reassigned value to be properly initialized.
   ok &= mCreationTime.Initialize("creation_time", this, nullptr, *GetSimulation());
   ok &= mHeight.Initialize("height", this, &mScriptContext, globalContext);
   ok &= mLength.Initialize("length", this, &mScriptContext, globalContext);
   ok &= mWidth.Initialize("width", this, &mScriptContext, globalContext);
   ok &= mEmptyMass.Initialize("empty_mass", this, &mScriptContext, globalContext);
   ok &= mFuelMass.Initialize("fuel_mass", this, &mScriptContext, globalContext);
   ok &= mPayloadMass.Initialize("payload_mass", this, &mScriptContext, globalContext);

   ok &= GetSignatureList().Initialize(aSimTime);

   // Let the component factories perform pre-initialization.
   // This allows them to add any 'required' components and then have them initialized.
   ok &= WsfComponentFactoryList<WsfPlatform>::PreInitialize(GetScenario(), aSimTime, *this);

   // Let the components perform pre-initialization
   // This allows them to add any 'required' components and then have them initialized.
   std::vector<WsfPlatformComponent*> preComponents(GetComponentsByInitializationOrder());
   for (auto& preComponent : preComponents)
   {
      WsfPlatformComponent* componentPtr(preComponent);
      if (!componentPtr->PreInitialize(aSimTime))
      {
         ok       = false;
         auto out = ut::log::error() << "Platform component failed pre-initialization.";
         out.AddNote() << "Platform: " << GetName();
         out.AddNote() << "Component: " << componentPtr->GetComponentName();
      }
   }

   // Perform phase 1 initialization.
   std::vector<WsfPlatformComponent*> components(GetComponentsByInitializationOrder());
   for (auto& component : components)
   {
      WsfPlatformComponent* componentPtr(component);
      if (!componentPtr->Initialize(aSimTime))
      {
         ok       = false;
         auto out = ut::log::error() << "Platform component failed phase one initialization.";
         out.AddNote() << "Platform: " << GetName();
         out.AddNote() << "Component: " << componentPtr->GetComponentName();
      }
   }

   // Set the concealment factor after the platform spatial domain is set!
   // (Why I don't know, but that comment was in here... JAJ)
   SetConcealmentFactor(mConcealmentFactor);

   mInitPass = ok;
   return ok;
}

// =================================================================================================
//! Post-Initialize the platform and all its aggregate parts.
//!
//! @param aSimulationPtr Pointer to the simulation
//! @param aSimTime The current simulation time.
//! @return 'true' if successful or 'false' if some sort of error occurred.
//! @note This method is invoked automatically when a object is added to the WsfSimulation object.
// virtual
bool WsfPlatform::Initialize2(WsfSimulation* aSimulationPtr, double aSimTime)
{
   // Prevent success of Initialize2 if Initialize not run and successful.
   if (!mInitPass)
   {
      return false;
   }

   bool ok = true;

   std::vector<WsfPlatformComponent*> components(GetComponentsByInitializationOrder());

   ok &= mScriptContext.Initialize2(aSimTime);
   for (auto& component : components)
   {
      WsfComponent* componentPtr(component);
      if (!componentPtr->Initialize2(aSimTime))
      {
         ok       = false;
         auto out = ut::log::error() << "Platform component failed phase two initialization.";
         out.AddNote() << "Platform: " << GetName();
         out.AddNote() << "Component: " << componentPtr->GetComponentName();
      }
   }

   if (!ok)
   {
      return false;
   }

   mHasOnUpdateScript = nullptr != mScriptContext.FindScript("on_update");

   // Marking the platform as 'initialized' cannot be done until EVERYTHING is truly initialized.
   mIsInitialized = true;

   // Update() cannot be allowed until initialization is complete AND the platform is formally part
   // of the simulation (see the early discussion about deferred launch platforms).
   mUpdateAllowed = (mIndex != 0);

   return ok;
}

// =================================================================================================
// virtual
bool WsfPlatform::ProcessInput(UtInput& aInput)
{
   WsfTrack* trackPtr(nullptr);

   bool        myCommand(true);
   std::string command(aInput.GetCommand());

   if (command == "side")
   {
      std::string sideString;
      aInput.ReadValue(sideString);
      mSide = WsfStringId(sideString);
   }
   else if (command == "icon")
   {
      std::string iconString;
      aInput.ReadValue(iconString);
      mIcon = WsfStringId(iconString);
   }
   else if (command == "marking")
   {
      std::string markingString;
      aInput.ReadValue(markingString);
      mMarking = WsfStringId(markingString);
   }
   else if (command == "indestructible")
   {
      mIsIndestructible = true;
   }
   else if (command == "destructible")
   {
      mIsIndestructible = false;
   }
   else if (command == "on_broken")
   {
      std::string action;
      aInput.ReadValue(action);
      if (action == "remove")
      {
         mOnBrokenAction = cON_BROKEN_REMOVE;
      }
      else if (action == "disable")
      {
         mOnBrokenAction = cON_BROKEN_DISABLE;
      }
      else if (action == "disabled_but_movable")
      {
         mOnBrokenAction = cON_BROKEN_MOVABLE_DISABLED;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "spatial_domain")
   {
      std::string spatialDomain;
      aInput.ReadValue(spatialDomain);
      if (!WsfTypes::StringToEnum(spatialDomain, mSpatialDomain))
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "position")
   {
      double lat, lon;
      aInput.ReadValueOfType(lat, UtInput::cLATITUDE);
      aInput.ReadValueOfType(lon, UtInput::cLONGITUDE);
      // Needed so WsfEventOutput::PlatformAdded() can see it before Initialize() is called.
      SetLocationLLA(lat, lon, GetAltitude());
   }
   else if (command == "mgrs_coordinate")
   {
      std::string mgrsString;
      aInput.ReadValue(mgrsString);
      double lat, lon;
      if (UtEllipsoidalEarth::ConvertMGRSToLL(mgrsString, lat, lon))
      {
         // Needed so WsfEventOutput::PlatformAdded() can see it before Initialize() is called.
         SetLocationLLA(lat, lon, GetAltitude());
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unable to convert MGRS location: " + mgrsString);
      }
   }
   else if (command == "altitude")
   {
      double alt;
      aInput.ReadValueOfType(alt, UtInput::cLENGTH);
      std::string altRef;
      aInput.ReadCommand(altRef);
      if (altRef == "agl")
      {
         mInputAltAGL = true;
      }
      else if (altRef == "msl")
      {
         mInputAltAGL = false;
      }
      else
      {
         mInputAltAGL = false;
         aInput.PushBack(altRef);
      }

      double lat, lon, oldAlt;
      GetLocationLLA(lat, lon, oldAlt);
      SetLocationLLA(lat, lon, alt);
   }
   else if (command == "heading")
   {
      double yaw, pitch, roll;
      GetOrientationNED(yaw, pitch, roll);
      aInput.ReadValueOfType(yaw, UtInput::cANGLE);
      aInput.ValueInClosedRange(yaw, -UtMath::cTWO_PI, UtMath::cTWO_PI);
      SetOrientationNED(yaw, pitch, roll);
   }
   else if (command == "pitch")
   {
      double yaw, pitch, roll;
      GetOrientationNED(yaw, pitch, roll);
      aInput.ReadValueOfType(pitch, UtInput::cANGLE);
      aInput.ValueInClosedRange(pitch, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      SetOrientationNED(yaw, pitch, roll);
   }
   else if (command == "roll")
   {
      double yaw, pitch, roll;
      GetOrientationNED(yaw, pitch, roll);
      aInput.ReadValueOfType(roll, UtInput::cANGLE);
      aInput.ValueInClosedRange(roll, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      SetOrientationNED(yaw, pitch, roll);
   }
   else if (command == "empty_mass")
   {
      mEmptyMass.ReadValueOfType(aInput, UtInput::cMASS);
      mEmptyMass.ValueGreaterOrEqual(aInput, 0.0);
   }
   else if (command == "fuel_mass")
   {
      mFuelMass.ReadValueOfType(aInput, UtInput::cMASS);
      mFuelMass.ValueGreaterOrEqual(aInput, 0.0);
   }
   else if (command == "payload_mass")
   {
      mPayloadMass.ReadValueOfType(aInput, UtInput::cMASS);
      mPayloadMass.ValueGreaterOrEqual(aInput, 0.0);
   }
   else if (command == "concealment_factor")
   {
      aInput.ReadValue(mConcealmentFactor);
      aInput.ValueGreaterOrEqual(mConcealmentFactor, 0.0F);
      aInput.ValueLessOrEqual(mConcealmentFactor, 1.0F);
   }
   else if ((command == "initial_damage_factor") || (command == "initial_max_damage_factor")) // Old form still allowed
   {
      aInput.ReadValue(mDamageFactor);
      aInput.ValueGreaterOrEqual(mDamageFactor, 0.0);
      aInput.ValueLessOrEqual(mDamageFactor, 1.0);
   }
   else if (command == "height")
   {
      mHeight.ReadValueOfType(aInput, UtInput::cLENGTH);
      mHeight.ValueGreaterOrEqual(aInput, 0.0);
   }
   else if (command == "length")
   {
      mLength.ReadValueOfType(aInput, UtInput::cLENGTH);
      mLength.ValueGreaterOrEqual(aInput, 0.0);
   }
   else if (command == "width")
   {
      mWidth.ReadValueOfType(aInput, UtInput::cLENGTH);
      mWidth.ValueGreaterOrEqual(aInput, 0.0);
   }
   else if (command == "creation_time")
   {
      mCreationTime.ProcessInput(aInput, UtInput::cTIME);
   }
   else if ((command == "nutation_update_interval") || (command == "precession_nutation_update_interval"))
   {
      double nutationUpdateInterval;
      aInput.ReadValueOfType(nutationUpdateInterval, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(nutationUpdateInterval, 0.0);
      SetNutationUpdateInterval(nutationUpdateInterval);

      if (command == "precession_nutation_update_interval")
      {
         auto out =
            ut::log::warning() << "The precession_nutation_update_interval input keyword has been deprecated "
                               << "and will be removed in a future release. Use nutation_update_interval instead.";
         out.AddNote() << "Location: " << aInput.GetLocation();
      }
   }
   else if (GetSignatureList().ProcessInput(aInput))
   {
   }
   else if (WsfAuxDataEnabled::ProcessInput(aInput))
   {
   }
   else if (command == "track_manager")
   {
      UtInputBlock inputBlock(aInput, "end_track_manager");
      while (inputBlock.ReadCommand())
      {
         if (!GetTrackManager().ProcessInput(inputBlock.GetInput()))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (mScriptContext.ProcessInput(aInput))
   {
   }
   else if (mCategories.ProcessInput(aInput))
   {
   }
   else if (mGroups.ProcessInput(aInput, this))
   {
   }
   else if (WsfTrack::LoadTrackInstance(GetScenario(), aInput, this, trackPtr))
   {
      if (trackPtr != nullptr)
      {
         GetTrackManager().AddInitialTrack(*trackPtr);
         delete trackPtr;
      }
   }
   else if (WsfComponentFactoryList<WsfPlatform>::ProcessInput(GetScenario(), aInput, *this))
   {
   }
   else if (WsfObject::ProcessInput(aInput)) // Try the base class
   {
   }
   // --------------------------------------------------------------------------------
   // If command is not recognized yet then pass to mover and fuel objects if defined.
   // No other 'else if' should occur after these statements.
   else if ((mMoverPtr != nullptr) && (mMoverPtr->ProcessInput(aInput)))
   {
   }
   else if ((mFuelPtr != nullptr) && (mFuelPtr->ProcessInput(aInput)))
   {
   }
   // Don't put other 'else if' blocks after this point.
   // --------------------------------------------------------------------------------
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
//! This is called by WsfPlatformTypes.
void WsfPlatform::ProcessInputBlock(UtInputBlock& aInputBlock, bool aLoadingType)
{
   // Set the default add/edit mode for building the constituent systems.
   // - If a 'platform_type' is being processed then the mode is 'add'.
   // - If a 'platform' is being processed then the default is 'edit'.

   bool addInstance = false;
   if (aLoadingType)
   {
      addInstance = true;
   }

   using FactoryListType = WsfComponentFactoryList<WsfPlatform>; // Make things shorter below...
   auto& scenario(GetScenario());

   while (aInputBlock.ReadCommand())
   {
      std::string command(aInputBlock.GetCommand());
      UtInput&    input(aInputBlock.GetInput());

      if (FactoryListType::ProcessAddOrEditCommand(scenario, input, *this, addInstance))
      {
         // Implicit add or edit based on platform (edit) or platform_type (add)
      }
      else if (command == "add")
      {
         // Explicit 'add' command (e.g. "add mover <type>" or "add processor <name> <type>")
         input.ReadCommand(command);
         if (!FactoryListType::ProcessAddOrEditCommand(scenario, input, *this, true))
         {
            throw UtInput::BadValue(input, "add must be followed by mover, comm, sensor, processor, etc... command");
         }
      }
      else if (command == "edit")
      {
         // Explicit 'edit' command (e.g. "edit mover" or "edit processor <name>")
         input.ReadCommand(command);
         if (!FactoryListType::ProcessAddOrEditCommand(scenario, input, *this, false))
         {
            throw UtInput::BadValue(input, "edit must be followed by mover, comm, sensor, processor, etc... command");
         }
      }
      else if (command == "delete")
      {
         // Explicit 'delete' command (e.g. "delete mover" or "delete processor <name>")
         input.ReadCommand(command);
         FactoryListType::ProcessDeleteCommand(scenario, input, *this);
      }
      else if (!ProcessInput(input))
      {
         throw UtInput::UnknownCommand(input);
      }
   }
}

// =================================================================================================
//! Create the signature list.
//! The signature list cannot be created in the constructor because the prototype may not yet be
//! completely initialized when the WSF_PLATFORM platform type is created. The first call to
//! GetSignatureList() will create it by calling this function.
//!
//! Delaying it until this point will ensure the scenario and any scenario extensions will have
//! registered their signature types.
// private
void WsfPlatform::CreateSignatureList()
{
   mSignatureListPtr = new WsfSignatureList(GetScenario().GetSignatureListPrototype(), this);
}

// =================================================================================================
//! Perform the actual update of the platform position.
//!
//! Update() calls this method when it has determined that an update is actually necessary. A derived class
//! may override this, but it is really preferable to simply provide special movers as this routine is also
//! responsible for calling observers, etc.
//!
//! @param aSimTime The current simulation time.
//!
//! @note Update() is often called by script methods to ensure a platforms location is current before
//! performing any operation involving the platform. In some cases these methods are called BEFORE
//! the platform is added to the simulation, so the mover is not initialized.
//! While it does prevent this call from bombing, it doesn't cover every conceivable situation.
//! WsfScriptPlatformClass probably needs to consider other operations (system TurnOn/Off, etc).
//! The check for 'mUpdateAllowed' in Update() ensures we don't get here until we are initialized.
// protected virtual
void WsfPlatform::DoUpdate(double aSimTime)
{
   // Return immediately if the time since last update is less than the minimum mover timestep.
   if ((aSimTime - mLastUpdateTime + 1.0E-9) < GetSimulation()->GetMinimumMoverTimestep())
   {
      return;
   }

   mLastUpdateTime = aSimTime;
   SetTime(aSimTime);         // Update the entity's time to allow valid ECI output.
   DoMoverUpdate(aSimTime);   // Update the mover; to include fuel and nav if present
   DoNotifyUpdate(aSimTime);  // Inform simulation observers of the update
   DoExecuteScript(aSimTime); // Execute scripts on the platform
}

// =================================================================================================
void WsfPlatform::DoUpdateMultiThread(double aSimTime)
{
   // Return immediately if the time since last update is less than the minimum mover timestep.
   if ((aSimTime - mLastUpdateTime) < GetSimulation()->GetMinimumMoverTimestep())
   {
      return;
   }

   mLastUpdateTime = aSimTime;
   DoMoverUpdate(aSimTime);
}

// =================================================================================================
void WsfPlatform::DoMoverUpdate(double aSimTime)
{
   if ((mMoverPtr != nullptr) && mMoverPtr->UpdateAllowed())
   {
      mMoverPtr->Update(aSimTime);
   }
}

// =================================================================================================
void WsfPlatform::DoNotifyUpdate(double aSimTime)
{
   if (mMoverPtr != nullptr)
   {
      assert(GetSimulation());
      WsfObserver::MoverUpdated(GetSimulation())(aSimTime, mMoverPtr); // Inform simulation observers of the update.
   }

   for (auto& observer : mObservers)
   {
      observer->OnPlatformUpdated(aSimTime, this);
   }
}

// =================================================================================================
void WsfPlatform::DoExecuteScript(double aSimTime)
{
   if (mHasOnUpdateScript)
   {
      mScriptContext.ExecuteScript(aSimTime, "on_update");
   }
}

// =================================================================================================
void WsfPlatform::DoSendQueuedMessages(double aSimTime)
{
   if (mMoverPtr != nullptr)
   {
      mMoverPtr->SendQueuedMessages(aSimTime);
      if (mFuelPtr != nullptr)
      {
         mFuelPtr->SendQueuedMessages(aSimTime);
      }
   }
}

// =================================================================================================
//! Initialize the creation time.
//! This is called by WsfSimulation to initialize the creation time of the platform.
//! If the creation time was selected to be drawn from a random number distribution,
//! perform the one and only draw to define the actual creation time.
bool WsfPlatform::InitializeCreationTime()
{
   // NOTE: The script context from the platform CANNOT be used because it hasn't been initialized
   // (this is called BEFORE the platform is initialized.) Therefore, the global context is used.
   bool ok = mCreationTime.Initialize("creation_time", this, nullptr, *GetSimulation());
   if (ok)
   {
      mCreationTime.Draw();
   }
   return ok;
}

// =================================================================================================
//! Set the time when the platform is to be added to the simulation.
//! @param aCreationTime The time when the platform is to be added to the simulation.
//! @note This is only effective for platforms created during simulation initialization.
void WsfPlatform::SetCreationTime(double aCreationTime)
{
   mCreationTime = WsfRandomVariable(aCreationTime, WsfRandomVariable::cNON_NEGATIVE);
}

// =================================================================================================
//! Returns the current simulation time.
//! If the platform has not been added to a simulation, returns 0.0.
double WsfPlatform::GetSimTime() const
{
   return (mSimulationPtr != nullptr) ? mSimulationPtr->GetSimTime() : 0.0;
}

void WsfPlatform::OnBrokenEvent(double aSimTime)
{
   if ((mOnBrokenAction == WsfPlatform::cON_BROKEN_DISABLE) ||
       (mOnBrokenAction == WsfPlatform::cON_BROKEN_MOVABLE_DISABLED))
   {
      if (mOnBrokenAction == WsfPlatform::cON_BROKEN_DISABLE)
      {
         // Stop the platform from moving.
         double zero[3] = {0.0, 0.0, 0.0};
         SetVelocityWCS(zero);
         SetAccelerationWCS(zero);

         // Delete the mover object. These *should* be safe to delete because the any events
         // that are dealing with them first check to see if they exist.

         SetMover(nullptr);
      }

      // Turn all the systems off and make them non-operational (so they can't be turned back on).
      // They can't simply be deleted because there may be events out there that may referencing them.
      //
      // We don't need to worry about scheduling events to turn the systems off because this is already
      // being done in the context of an event (i.e.: PlatformBrokenEvent). If the system is on we simply
      // call 'Turn<system>OffP', which turns the system off and invokes the derived class to let it
      // clean up internal structures.

      for (WsfPlatformComponentList::Iterator it = Begin(); it != End(); ++it)
      {
         WsfPlatformComponent* componentPtr(*it);
         WsfPlatformPart*      partPtr(nullptr);
         if (componentPtr->QueryInterfaceT(partPtr))
         {
            partPtr->OnPlatformBrokenEvent(aSimTime);
         }
      }
   }

   // Inform the observers that the platform is dead.
   if (mIsBroken)
   {
      WsfObserver::PlatformBroken(mSimulationPtr)(aSimTime, this);
   }
}

// =================================================================================================
//! Some other platform has been added to the simulation.
//!
//! This method is called by WsfSimulation whenever another platform has been added to the simulation.
//! This platform can then take whatever action it deems necessary.
//!
//! @param aSimTime     The current simulation time.
//! @param aPlatformPtr Pointer to the platform being added.
// virtual
void WsfPlatform::PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr)
{
   // Notify platform parts and command chains of the new platform.
   WsfPlatformPart* partPtr(nullptr);
   WsfCommandChain* chainPtr(nullptr);
   for (WsfPlatformComponentList::Iterator it = Begin(); it != End(); ++it)
   {
      WsfPlatformComponent* componentPtr(*it);
      if (componentPtr->QueryInterfaceT(partPtr))
      {
         partPtr->PlatformAdded(aSimTime, aPlatformPtr);
      }
      else if (componentPtr->QueryInterfaceT(chainPtr))
      {
         chainPtr->PlatformAdded(aPlatformPtr);
      }
   }
}

// =================================================================================================
//! Some other platform has been deleted from the simulation.
//!
//! WsfSimulation invokes this method whenever another platform has been deleted from the simulation.
//! This platform can then take whatever action it deems necessary.
//!
//! @param aPlatformPtr Pointer to the platform being deleted.
// virtual
void WsfPlatform::PlatformDeleted(WsfPlatform* aPlatformPtr)
{
   if (aPlatformPtr == this)
   {
      UtScript* scriptPtr = mScriptContext.FindScript("on_platform_deleted");
      if (scriptPtr != nullptr)
      {
         mScriptContext.ExecuteScript(GetSimTime(), scriptPtr);
      }
   }
   else
   {
      // Notify platform parts and command chains of the deleted platform.
      WsfPlatformPart* partPtr(nullptr);
      WsfCommandChain* chainPtr(nullptr);
      for (WsfPlatformComponentList::Iterator it = Begin(); it != End(); ++it)
      {
         WsfPlatformComponent* componentPtr(*it);
         if (componentPtr->QueryInterfaceT(partPtr))
         {
            partPtr->PlatformDeleted(aPlatformPtr);
         }
         else if (componentPtr->QueryInterfaceT(chainPtr))
         {
            chainPtr->PlatformDeleted(aPlatformPtr);
         }
      }
   }
}

// =================================================================================================
//! Return the difference between where the platform thinks it is and where it actual is located.
//! This can be used to compute the perceived location as follows:
//! \code
//!    perceived_location = truth_location + location_error;
//! \endcode
//! @param aLocationErrorWCS The difference between the perceived location and the true location.
void WsfPlatform::GetLocationErrorWCS(double aLocationErrorWCS[3]) const
{
   if (mNavErrorPtr == nullptr)
   {
      UtVec3d::Set(aLocationErrorWCS, 0.0);
   }
   else
   {
      auto locationErrorWCS = mNavErrorPtr->GetLocationErrorWCS();
      UtVec3d::Set(aLocationErrorWCS, locationErrorWCS.GetData());
   }
}

void WsfPlatform::SetSide(WsfStringId aSide)
{
   if (IsInitialized())
   {
      auto out = ut::log::warning() << "Unable to change the side of a platform after it has been initialized.";
      out.AddNote() << "Platform: " << GetName();
      out.AddNote() << "Current Side: " << GetSide();
      out.AddNote() << "Ignored Side: " << aSide;
   }
   else
   {
      mSide = aSide;
   }
}

// =================================================================================================
//! Get the spatial domain of the platform.
WsfSpatialDomain WsfPlatform::GetSpatialDomain() const
{
   // The correct spatial domain must be returned as soon as it is known. Some components (clutter,
   // for example), need to know it during their initialization. If the user defines it then that is
   // used, but otherwise the mover value is used. But the mover value, if the mover defines it, is
   // set during phase 1 mover initialization.

   // NOTE: This used to be a simple accessor but the implementation of generic components made it
   //       very kludgey to set it during Initialize.

   WsfSpatialDomain spatialDomain(mSpatialDomain);
   if (mSpatialDomain == WSF_SPATIAL_DOMAIN_UNKNOWN)
   {
      // Spatial domain was not defined explicitly by the user. If there is a mover then get it from there.
      if (mMoverPtr != nullptr)
      {
         spatialDomain = mMoverPtr->GetSpatialDomain();
      }

      // If there wasn't a mover or if the mover didn't define the spatial domain, use 'land'.
      if (spatialDomain == WSF_SPATIAL_DOMAIN_UNKNOWN)
      {
         spatialDomain = WSF_SPATIAL_DOMAIN_LAND;
      }
   }
   return spatialDomain;
}

// =================================================================================================
//! Execute the script with the specified string name ID.
//! @param aSimTime The current simulation time
//! @param aScriptName  The name of the script to be executed.
//! @param aScriptRetVal The return value of the script, which is filled by reference.
//!                      If the return type is void, the return value will be marked as invalid.
//! @param aScriptArgs The list of arguments to pass to the script.
//! @return 'true' if executed or 'false' if the script could not be found.
bool WsfPlatform::ExecuteScript(double                               aSimTime,
                                WsfStringId                          aScriptName,
                                UtScriptData&                        aScriptRetVal,
                                const std::vector<ut::script::Data>& aScriptArgs)
{
   bool ok = true;
   if (!mScriptContext.ExecuteScript(aSimTime, aScriptName, aScriptRetVal, aScriptArgs))
   {
      auto out = ut::log::error() << "Platform unable to invoke script.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetName();
      out.AddNote() << "Script: " << aScriptName;
      ok = false;
   }
   return ok;
}

// =================================================================================================
//! Execute the script with the specified string name ID.
//! @param aSimTime The current simulation time
//! @param aName  The name of the script to be executed.
//! @return 'true' if executed or 'false' if the script could not be found.
bool WsfPlatform::ExecuteScript(double aSimTime, WsfStringId aName)
{
   UtScriptData     retVal;
   UtScriptDataList args;
   return ExecuteScript(aSimTime, aName, retVal, args);
}

// =================================================================================================
//! Execute the script with the specified string name ID at the specified time.
//! @param aTime The time at which to execute the script.
//! @param aName The name of the script to be executed.
//! @param aScriptArgs The list of arguments to pass to the script.
void WsfPlatform::ExecuteScriptAtTime(double aTime, WsfStringId aName, const std::vector<ut::script::Data>& aScriptArgs)
{
   GetSimulation()->AddEvent(ut::make_unique<WsfScriptEvent>(aTime, this, aName, aScriptArgs));
}

// =================================================================================================
//! Returns 'true' if the platform has a script with the specified name.
bool WsfPlatform::HasScript(WsfStringId aScriptNameId) const
{
   return mScriptContext.FindScript(aScriptNameId) != nullptr;
}

// =================================================================================================
//! Returns a pointer to the articulated part with the specified unique ID.
//! This is a convenience method to locate an articulated part with the specified unique Id. This
//! exists because it is called enough to make it useful.
//! @param aUniqueId The unique ID of the articulated part to be located.
//! @returns The pointer to the requested articulated part, or 0 if it could not be found.
WsfArticulatedPart* WsfPlatform::GetArticulatedPart(unsigned int aUniqueId) const
{
   for (WsfComponentList::RoleIterator<WsfArticulatedPart> iter(*this); !iter.AtEnd(); ++iter)
   {
      WsfArticulatedPart* partPtr(*iter);
      if ((partPtr != nullptr) && (partPtr->GetUniqueId() == aUniqueId))
      {
         return partPtr;
      }
   }
   return nullptr;
}

// =================================================================================================
//! A callback from WsfComponentList whenever a component is added to the component list.
// protected virtual
void WsfPlatform::ComponentAdded(WsfComponent* aComponentPtr)
{
   UpdatePersistentPointers(aComponentPtr, true);
}

// =================================================================================================
//! A callback from WsfComponentList when a component is about to be removed from the component list.
// protected virtual
void WsfPlatform::ComponentDeleted(WsfComponent* aComponentPtr)
{
   UpdatePersistentPointers(aComponentPtr, false);
}

// =================================================================================================
//! Update a persistent component pointer as a result of updates to the component list.
//! @param aComponentPtr The component being added or deleted.
//! @param aIsBeingAdded true if the component is being added, false if being deleted.
// private
void WsfPlatform::UpdatePersistentPointers(WsfComponent* aComponentPtr, bool aIsBeingAdded)
{
   if (aComponentPtr->ComponentHasRole(cWSF_COMPONENT_MOVER))
   {
      mMoverPtr = nullptr;
      if (aIsBeingAdded)
      {
         aComponentPtr->QueryInterfaceT(mMoverPtr);
         assert(mMoverPtr != nullptr);
      }
   }
   else if (aComponentPtr->ComponentHasRole(cWSF_COMPONENT_FUEL))
   {
      mFuelPtr = nullptr;
      if (aIsBeingAdded)
      {
         aComponentPtr->QueryInterfaceT(mFuelPtr);
         assert(mFuelPtr != nullptr);
      }
   }
   else if (aComponentPtr->ComponentHasRole(cWSF_COMPONENT_NAVIGATION_ERRORS))
   {
      mNavErrorPtr = nullptr;
      if (aIsBeingAdded)
      {
         aComponentPtr->QueryInterfaceT(mNavErrorPtr);
         assert(mNavErrorPtr != nullptr);
      }
   }
   else if (aComponentPtr->ComponentHasRole(cWSF_COMPONENT_TRACK_MANAGER))
   {
      mMasterTrackManagerPtr = nullptr;
      if (aIsBeingAdded)
      {
         aComponentPtr->QueryInterfaceT(mMasterTrackManagerPtr);
         assert(mMasterTrackManagerPtr != nullptr);
      }
   }
}

// =================================================================================================
//! Attach the mover object for the platform.
//! @param aMoverPtr Pointer to the mover object. The platform assumes ownership of the object.
void WsfPlatform::SetMover(WsfMover* aMoverPtr)
{
   if (mMoverPtr != nullptr)
   {
      DeleteComponent(mMoverPtr->GetNameId(), mMoverPtr->GetComponentRoles()[0]);
   }

   // Maintain order in this logic check
   if ((aMoverPtr != nullptr) && AddComponent(aMoverPtr) && (GetSimulation() != nullptr))
   {
      WsfObserver::MoverChanged(GetSimulation())(this);
   }
}

// =================================================================================================
//! Swap Mover pointers in the Platform.
//! @param aSimTime Current simulated time.
//! @param aNewMoverPtr pointer to be swapped into the platform. The platform assumes ownership.
//! @param aOldMoverPtr pointer to be swapped out of the platform. The platform gives up ownership.
//! @return Bool to indicate whether the swap was performed or not.
bool WsfPlatform::SwapMover(double aSimTime, WsfMover*& aNewMoverPtr, WsfMover*& aOldMoverPtr)
{
   if (IsExternallyControlled() || IsExternallyMoved())
   {
      ut::log::warning() << "SwapMover() called on an external platform (DIS). Mover was not swapped.";
      return false;
   }

   aOldMoverPtr = nullptr;
   if (mMoverPtr != nullptr)
   {
      // Force a position update before swapping. This ensures the new mover gets the state of the
      // platform at the specified time.
      Update(aSimTime);

      // Inform the new mover that a swap is being initiated. This allows the new mover
      // to capture information from the old mover.
      if (aNewMoverPtr != nullptr)
      {
         aNewMoverPtr->SwapMoverInitiated(aSimTime, mMoverPtr);
      }
      aOldMoverPtr = mMoverPtr;
      aOldMoverPtr->SetPlatform(nullptr);
      RemoveComponent(mMoverPtr);
   }

   SetMover(aNewMoverPtr);
   if (aNewMoverPtr != nullptr)
   {
      aNewMoverPtr->SetLastUpdateTime(aSimTime);

      // The mover must be initialized before turn-on.
      aNewMoverPtr->PreInitialize(aSimTime);
      aNewMoverPtr->Initialize(aSimTime);
      aNewMoverPtr->Initialize2(aSimTime);

      if (aNewMoverPtr->InitiallyTurnedOn())
      {
         GetSimulation()->TurnPartOn(aSimTime, aNewMoverPtr);
      }

      // This is done to prevent the caller from deleting the
      // memory he allocated (if any), since WsfPlatform now owns it.
      aNewMoverPtr = nullptr;
   }
   return true;
}

// =================================================================================================
//! Returns the height of the platform above the terrain at the location of the platform.
double WsfPlatform::GetHeightAboveTerrain()
{
   double lat, lon, alt;
   GetLocationLLA(lat, lon, alt);
   float elev;
   mTerrainPtr->GetElevInterp(lat, lon, elev);
   return alt - elev;
}

// =================================================================================================
//! Returns the height of the terrain at the current location of the platform.
double WsfPlatform::GetTerrainHeight()
{
   double lat, lon, alt;
   GetLocationLLA(lat, lon, alt);
   float elev;
   mTerrainPtr->GetElevInterp(lat, lon, elev);
   return elev;
}

// =================================================================================================
//! Gets the appearance
unsigned int WsfPlatform::GetAppearance(unsigned int aLSB, unsigned int aNumBits) const
{
   // Check the inputs
   unsigned int position    = std::min(std::max(0u, aLSB), 32u);
   unsigned int endPosition = std::min(position + std::max(0u, aNumBits), 32u);

   // Generate the mask; shift to the correct position; and
   return ((mAppearanceWord >> position) & WsfUtil::GenerateBitMask(endPosition - position));
}

// =================================================================================================
//! Set the DIS entity 'appearance' word
//!
//! @note Does not allow setting of bits 3-4, 21, or 23. These are set by other means.
void WsfPlatform::SetAppearance(unsigned int aLSB, unsigned int aNumBits, unsigned int aValue, const bool aFromScript)
{
   unsigned int position    = std::min(std::max(0u, aLSB), 32u);
   unsigned int endPosition = std::min(position + std::max(0u, aNumBits), 32u);

   unsigned int protectMask = 10485784u; // bits 3-4, 21 & 23
   unsigned int bitMask     = (WsfUtil::GenerateBitMask(endPosition - position) << position);

   unsigned int oldValue = mAppearanceWord;

   mAppearanceWord &= ~(bitMask | protectMask);              // zero
   mAppearanceWord |= ((aValue << position) & ~protectMask); // fill

   // Check for script over-ride
   if (aFromScript)
   {
      mAppearanceWordScriptMask |= (bitMask & ~protectMask);
   }

   if ((mAppearanceWord != oldValue) && (GetSimulation() != nullptr))
   {
      WsfObserver::PlatformAppearanceChanged(GetSimulation())(GetSimTime(), this, WsfObserver::cAPPEARANCE_WORD);
   }
}

// =================================================================================================
//! Returns true if data in the appearance word has been set from script
bool WsfPlatform::ScriptOverride(unsigned int aLSB, unsigned int aNumBits) const
{
   // Check for script over-ride
   unsigned int position    = std::min(std::max(0u, aLSB), 32u);
   unsigned int endPosition = std::min(position + std::max(0u, aNumBits), 32u);

   // Shift bits by LSB value and with the mask and count the bits on
   return (WsfUtil::CountBitsOn((mAppearanceWordScriptMask >> position) &
                                WsfUtil::GenerateBitMask(endPosition - position)) > 0);
}

// =================================================================================================
// This function is called by the DIS Interface in response to an incoming DIS Entity State PDU.
// We may need to generate a callback to let concerned platforms know of this change.
void WsfPlatform::SetCapabilities(unsigned int aCapabilitiesWord)
{
   if (mCapabilitiesWord != aCapabilitiesWord)
   {
      // This call will generate callbacks to WsfObserver::PlatformCapabilityChanged.
      WsfExchange::ModifyCapabilities(this, mCapabilitiesWord, aCapabilitiesWord);
      mCapabilitiesWord = aCapabilitiesWord;
   }
}

// =================================================================================================
//! Set the 'concealment factor'.
//!
//! The concealment factor is a number in the range [0, 1] and is used to denote how well a platform is
//! VISUALLY concealed.  A value of 0.0 means there is no attempt to hide. A factor of 1.0 means that it
//! is hidden in such a way as to be undetectable (such as in a building or in an underground bunker).
//! Values between the endpoints represent increasing amounts of camouflage.
//!
//! At some point this could be replaced with optical signatures that can be varied by configuration.
//!
//! @param aConcealmentFactor The concealment factor in the range [0 .. 1]
void WsfPlatform::SetConcealmentFactor(float aConcealmentFactor)
{
   float oldValue     = mConcealmentFactor;
   mConcealmentFactor = aConcealmentFactor;
   if ((mConcealmentFactor != oldValue) && (GetSimulation() != nullptr))
   {
      WsfObserver::PlatformAppearanceChanged(GetSimulation())(GetSimTime(), this, WsfObserver::cCONCEALMENT_FACTOR);
   }

   // Set the appearance word; only handles land domain entities
   if (!ScriptOverride(19, 1))
   {
      WsfMover* moverPtr = GetMover();
      if ((moverPtr != nullptr) && (moverPtr->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_LAND))
      {
         SetAppearance(19, 1, (mConcealmentFactor > 0.99));
      }
   }
}

// =================================================================================================
//! Set the 'damage factor'.
//!
//! The damage factor is a value in the range [0 .. 1] and is used to indicate the amount of damage to
//! the platform.  A value of 0 indicates no damage while a value of 1 indicates the platform is 'dead'.
//! @param aDamageFactor Damage factor in the range [0 .. 1]
void WsfPlatform::SetDamageFactor(double aDamageFactor)
{
   if (aDamageFactor != mDamageFactor)
   {
      double oldDamageFactor = mDamageFactor;
      mDamageFactor          = aDamageFactor;

      // If the damage factor has increased, invoke the 'on_platform_damaged' script if it is defined.
      //
      // NOTE: This is a quick fix to address a need to have a scriptable action that is called when
      // a platform that is marked 'indestructible' is damaged. This lets the platform take actions
      // (stop movement, change signature state, etc.). This should be reevaluated when the damage
      // enhancements are completed.

      if (mDamageFactor > oldDamageFactor)
      {
         UtScript* scriptPtr = mScriptContext.FindScript("on_damage_received");
         if (scriptPtr != nullptr)
         {
            mScriptContext.ExecuteScript(GetSimTime(), scriptPtr);
         }
      }

      // Also inform the standard observer interface.
      if ((mDamageFactor != oldDamageFactor) && (GetSimulation() != nullptr))
      {
         WsfObserver::PlatformDamageChanged(GetSimulation())(mLastUpdateTime, this);
      }
   }
}

// =================================================================================================
//! Indicates that the platform is currently at rest, and with zero commanded velocity.
bool WsfPlatform::IsStopped() const
{
   return (mMoverPtr == nullptr) ? true : mMoverPtr->IsStopped();
}

// =================================================================================================
//! Indicates that the platform is currently in "pause" mode; is not moving but may begin
//! moving at some future time.
bool WsfPlatform::IsPaused() const
{
   return (mMoverPtr == nullptr) ? true : mMoverPtr->IsPaused();
}

// =================================================================================================
//! Replaces the bool AtEndOfPath() function.  True indicates that the platform has reached its final
//! path destination, but may or may not be continuing on toward a specified heading, altitude, and
//! speed.  Note that the platform can be sent a new path at which time this will be set back to 'false'.
bool WsfPlatform::IsExtrapolating() const
{
   return (mMoverPtr == nullptr) ? true : mMoverPtr->IsExtrapolating();
}

// =================================================================================================
//! Assign a new track ID to be used a track created by this platform.
//!
//! @return The new track ID
const WsfTrackId& WsfPlatform::GetNextTrackId()
{
   mNextTrackId.SetOwningPlatformId(GetNameId());
   mNextTrackId.IncrementLocalTrackNumber();
   return mNextTrackId;
}

// =================================================================================================
//! Return a pointer to the master track list.
WsfLocalTrackList& WsfPlatform::GetMasterTrackList()
{
   return GetTrackManager().GetTrackList();
}

// =================================================================================================
WsfTrackList& WsfPlatform::GetMasterRawTrackList()
{
   return GetTrackManager().GetRawTrackList();
}

// =================================================================================================
//! Return a pointer to the "master" track manager (i.e., the
//! WsfTrackManager object that owns and maintains the master track list).
WsfTrackManager& WsfPlatform::GetTrackManager()
{
   if (mMasterTrackManagerPtr == nullptr)
   {
      if (!AddComponent(new WsfTrackManager(GetScenario()))) // This should never fail
      {
         throw UtException("Failed to add track manager to platform!");
      }

      if (mIndex != 0)
      {
         // We are creating the track manager after platform initialization.  It must be initialized here.
         double simTime(GetSimulation()->GetSimTime());
         mMasterTrackManagerPtr->Initialize(simTime);
         mMasterTrackManagerPtr->Initialize2(simTime);
      }
   }
   return *mMasterTrackManagerPtr;
}

// =================================================================================================
//! Adds the provided category to the platform's category list.
void WsfPlatform::AddCategory(WsfStringId aCategoryId)
{
   mCategories.JoinCategory(aCategoryId);
}

// =================================================================================================
//! Is this a member of the indicated category?
//! @param aCategoryId String Id of the category to be checked.
//! @return 'true' if a member of the category.
bool WsfPlatform::IsCategoryMember(WsfStringId aCategoryId) const
{
   return mCategories.IsCategoryMember(aCategoryId);
}

// =================================================================================================
//! Join the given group
void WsfPlatform::JoinGroup(WsfGroup* aGroupPtr)
{
   mGroups.JoinGroup(aGroupPtr, GetIndex());
}

// =================================================================================================
//! Leave the given group
void WsfPlatform::LeaveGroup(WsfGroup* aGroupPtr)
{
   mGroups.LeaveGroup(aGroupPtr, GetIndex());
}

// =================================================================================================
//! Is this a member of the indicated group?
//! @param aGroupId String Id of the group to be checked.
//! @return 'true' if a member of the group.
bool WsfPlatform::IsGroupMember(WsfStringId aGroupId)
{
   return mGroups.IsGroupMember(aGroupId);
}

// =================================================================================================
void WsfPlatform::Comment(const double aTime, const std::string& aString)
{
   auto* simulationPtr = GetSimulation();
   if (simulationPtr)
   {
      // Function to inform simulation observers of the comment.
      auto commentFn = [=](WsfPlatform* aPlatformPtr)
      { WsfObserver::Comment(simulationPtr)(aTime, aPlatformPtr, aString); };
      if (aTime <= simulationPtr->GetSimTime())
      {
         // Time is current, invoke function now
         commentFn(this);
      }
      else
      {
         // Time is in future, schedule an event to invoke the function
         size_t index = mIndex; // Capture platform index rather than "this" in case the event outlives the platform
         simulationPtr->AddEvent(ut::make_unique<WsfOneShotEvent>(aTime,
                                                                  [=]()
                                                                  {
                                                                     auto* platformPtr =
                                                                        simulationPtr->GetPlatformByIndex(index);
                                                                     if (platformPtr)
                                                                     {
                                                                        commentFn(platformPtr);
                                                                     }
                                                                  }));
      }
   }
}

// =================================================================================================
void WsfPlatform::AttachObserver(WsfSinglePlatformObserver* aObserverPtr)
{
   auto iter = std::find(mObservers.begin(), mObservers.end(), aObserverPtr);
   if (iter == mObservers.end())
   {
      mObservers.push_back(aObserverPtr);
   }
}

// =================================================================================================
void WsfPlatform::DetachObserver(WsfSinglePlatformObserver* aObserverPtr)
{
   auto iter = std::find(mObservers.begin(), mObservers.end(), aObserverPtr);
   if (iter != mObservers.end())
   {
      mObservers.erase(iter);
   }
}

// =================================================================================================
void WsfPlatform::NotifyDeleted(double aSimTime)
{
   for (auto& observer : mObservers)
   {
      observer->OnPlatformDeleted(aSimTime, this);
   }
}

// =================================================================================================
void WsfPlatform::NotifyDeleting(double aSimTime)
{
   for (auto& observer : mObservers)
   {
      observer->OnPlatformDeleting(aSimTime, this);
   }
}
