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

#include "WsfMover.hpp"

#include <memory>

#include "UtEllipsoidalEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfModeList.hpp"
#include "WsfMoverMode.hpp"
#include "WsfMoverObserver.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"

const char* WsfMover::cTYPE_KIND = "mover";

// ================================================================================================
WsfMover::WsfMover(const WsfScenario& aScenario)
   : WsfPlatformPart(aScenario, cCOMPONENT_ROLE<WsfMover>())
   , mLastUpdateTime(0.0)
   , mUpdateTimeTolerance(1.0E-3)
   , mThreadSafe(true)
   , mMoverModeList()
   , mModeId()
   , mModeListPtr(nullptr)
   , mUpdateInterval(0.0)
   , mPropertiesPtr(nullptr)
   , mUpdateWhenBroken(false)
{
   SetName("mover"); // Must not be changed. Can be referenced by 'internal_link' commands
}

// ================================================================================================
WsfMover::WsfMover(const WsfMover& aSrc)

   : WsfPlatformPart(aSrc)
   , mLastUpdateTime(aSrc.mLastUpdateTime)
   , mUpdateTimeTolerance(aSrc.mUpdateTimeTolerance)
   , mThreadSafe(aSrc.mThreadSafe)
   , mMoverModeList()
   , mModeId(aSrc.mModeId)
   , mModeListPtr(nullptr)
   , mUpdateInterval(aSrc.mUpdateInterval)
   , mPropertiesPtr(nullptr)
   , mUpdateWhenBroken(aSrc.mUpdateWhenBroken)
{
   if (aSrc.mPropertiesPtr != nullptr)
   {
      // Using raw pointer clone here.
      mPropertiesPtr = aSrc.mPropertiesPtr->Clone();
   }
   if (aSrc.mModeListPtr != nullptr)
   {
      mModeListPtr = new WsfModeList(*(aSrc.mModeListPtr));
   }
}

// ================================================================================================
WsfMover::~WsfMover()
{
   delete mPropertiesPtr;
   delete mModeListPtr;
}

// ================================================================================================
void WsfMover::SetModeList(WsfModeList* aModeListPtr)
{
   delete mModeListPtr;
   mModeListPtr = aModeListPtr;
}

// ================================================================================================
//! A mover swap has been initiated (i.e.: a call to WsfPlatform::SwapMover).
//! This is called during the execution of WsfPlatform::SwapMover while the old mover is still the
//! active mover. After this is called the new mover is initialized (Initialize and Initialize2 are
//! called) and then made the active mover. The new mover typically uses this call to establish an
//! environment to be used during subsequent Update calls to blend the old state data with the new
//! state data for some small period of time.
//! @param aSimTime The current simulation time.
//! @param aOldMoverPtr Pointer to the old mover.
//! @note This method is not called if there is no old mover.
/// virtual
void WsfMover::SwapMoverInitiated(double aSimTime, WsfMover* aOldMoverPtr) {}

// ================================================================================================
//! Initialize the kinematic state of the associated platform.
//! Fix the initial location, orientation, speed and acceleration of the attached
//! platform to what it should be at the beginning of its existence.
//! @param aSimTime The current simulation time.
//! @return True if successful.
// virtual
bool WsfMover::Initialize(double aSimTime)
{
   bool              ok            = WsfPlatformPart::Initialize(aSimTime);
   WsfScriptContext& globalContext = GetSimulation()->GetScriptContext();
   WsfScriptContext* contextPtr    = &(GetPlatform()->GetScriptContext());
   ok &= mUpdateInterval.Initialize("update_interval", this, contextPtr, globalContext);
   ok &= mUpdateTimeTolerance.Initialize("update_time_tolerance", this, contextPtr, globalContext);
   mLastUpdateTime = aSimTime;
   InitializeProperties();

   if (mModeListPtr != nullptr)
   {
      // Initialize the mode list.
      ok &= mModeListPtr->Initialize();

      // Reduce future dynamic casting by extracting derived class mode pointers.
      mModeListPtr->GetDerivedModeList(mMoverModeList);

      // Initialize the modes.
      for (unsigned int i = 0; i < mMoverModeList.size(); ++i)
      {
         ok &= mMoverModeList[i]->Initialize(aSimTime, this);
      }
   }

   return ok;
}

// ================================================================================================
// virtual
bool WsfMover::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "update_interval")
   {
      mUpdateInterval.ReadValueOfType(aInput, UtInput::cTIME);
      mUpdateInterval.ValueGreater(aInput, 0.0);
   }
   else if (command == "update_time_tolerance")
   {
      mUpdateTimeTolerance.ReadValueOfType(aInput, UtInput::cTIME);
      mUpdateTimeTolerance.ValueGreaterOrEqual(aInput, 0.0);
   }
   else if ((mModeListPtr != nullptr) && mModeListPtr->ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = WsfPlatformPart::ProcessInput(aInput);
   }

   return myCommand;
}

// ================================================================================================
bool WsfMover::GetPropertyInt(const std::string& aPropertyName, int& aProperty)
{
   bool result = false;
   if (PropertyExists(aPropertyName))
   {
      GetProperties().Get(aPropertyName, aProperty);
      result = true;
   }

   return result;
}

// ================================================================================================
bool WsfMover::GetPropertyDouble(const std::string& aPropertyName, double& aProperty)
{
   bool result = false;
   if (PropertyExists(aPropertyName))
   {
      GetProperties().Get(aPropertyName, aProperty);
      result = true;
   }

   return result;
}

// ================================================================================================
bool WsfMover::GetPropertyString(const std::string& aPropertyName, std::string& aProperty)
{
   bool result = false;
   if (PropertyExists(aPropertyName))
   {
      GetProperties().Get(aPropertyName, aProperty);
      result = true;
   }

   return result;
}

// ================================================================================================
//! Request the platform mover to go to a desired NED heading angle (radians).
//! This is merely a request, and the mover may not be able to comply.
// virtual
void WsfMover::SetHeading(double aHeading)
{
   double yaw, pitch, roll;
   GetPlatform()->GetOrientationNED(yaw, pitch, roll);
   GetPlatform()->SetOrientationNED(aHeading, pitch, roll);
}

// ================================================================================================
//! Request the platform mover to go to a desired NED pitch angle (radians).
//! This is merely a request, and the mover may not be able to comply.
// virtual
void WsfMover::SetPitch(double aPitch)
{
   double yaw, pitch, roll;
   GetPlatform()->GetOrientationNED(yaw, pitch, roll);
   GetPlatform()->SetOrientationNED(yaw, aPitch, roll);
}

// ================================================================================================
//! Request the platform mover to go to a desired NED roll (bank) angle (radians).
//! This is merely a request, and the mover may not be able to comply.
// virtual
void WsfMover::SetRoll(double aRoll)
{
   double yaw, pitch, roll;
   GetPlatform()->GetOrientationNED(yaw, pitch, roll);
   GetPlatform()->SetOrientationNED(yaw, pitch, aRoll);
}

// ================================================================================================
//! Request the platform mover to go to a desired NED orientation (radians).
//! This is merely a request, and the mover may not be able to comply fully.
// virtual
void WsfMover::SetOrientationNED(double aHeading, double aPitch, double aRoll)
{
   GetPlatform()->SetOrientationNED(aHeading, aPitch, aRoll);
}

// ================================================================================================
// virtual
void WsfMover::TurnOff(double aSimTime)
{
   WsfPlatformPart::TurnOff(aSimTime);
   WsfObserver::MoverTurnedOff(GetSimulation())(aSimTime, this);
}

// ================================================================================================
// virtual
void WsfMover::TurnOn(double aSimTime)
{
   WsfPlatformPart::TurnOn(aSimTime);
   WsfObserver::MoverTurnedOn(GetSimulation())(aSimTime, this);
}

// ================================================================================================
// virtual
void WsfMover::SetNonOperational(double aSimTime)
{
   WsfPlatformPart::SetNonOperational(aSimTime);
   WsfObserver::MoverNonOperational(GetSimulation())(aSimTime, this);
}

// ================================================================================================
// virtual
void WsfMover::SetOperational(double aSimTime)
{
   WsfPlatformPart::SetOperational(aSimTime);
   WsfObserver::MoverOperational(GetSimulation())(aSimTime, this);
}

// ================================================================================================
//! Set the status to Broken.
//! @param aSimTime The current simulation time.
//! @note  This method should not be invoked directly by application code.
// virtual
void WsfMover::PartBroken(double aSimTime)
{
   // Set the Mobility appearance bit to "Broken" (without pulling in any DIS header dependencies):
   GetPlatform()->SetAppearance(1, 1, 1, false);
   WsfPlatformPart::PartBroken(aSimTime);
   WsfObserver::MoverBroken(GetSimulation())(aSimTime, this);
}

// ================================================================================================
// virtual
void WsfMover::Restore(double aSimTime)
{
   WsfPlatformPart::Restore(aSimTime);
   if (GetDamageFactor() < 1.0)
   {
      // Set the Mobility appearance bit to "Not Broken" (without pulling in any DIS header dependencies):
      GetPlatform()->SetAppearance(1, 1, 0, false);
   }
}

// ================================================================================================
//! Update the kinematic state of the associated platform.
//! Update the location, orientation, speed and acceleration of the attached
//! platform to what it should be at the supplied time.
//! @param aSimTime The current simulation time.
//! @note  The derived class MUST update the protected variable mLastUpdateTime
//!        to be the current simulation time.
// virtual
void WsfMover::Update(double aSimTime)
{
   mLastUpdateTime = aSimTime;
}

// ================================================================================================
//! Should the mover update calls by WsfPlatform be allowed?
//! WsfPlatform::Update will invoke this to determine if the mover should be updated as part of the call.
//! Normally a mover will update as long as it is turned on. If the mover at sometime gets marked as 'broken'
//! it gets turned off and by default future mover updates will be suppressed. But some movers still should
//! get updated even if broken (for example, to fall from the sky). If the mover desires, they can suppress
//! this action and let it get called even when broken by calling SetUpdateWhenBroken() in its constructor.
//!
//! A derived class may also override this method to provide custom behavior.
//!
//! @returns true if calls to the Update method should be allowed.
// virtual
bool WsfMover::UpdateAllowed() const
{
   return IsTurnedOn() || (mUpdateWhenBroken && (GetDamageFactor() >= 1.0));
}

// ================================================================================================
//! Return the spatial domain in which the mover is operating.
//!
//! The spatial domain is the domain in which the mover is operating (land, sea, space, etc.).
//! @returns The spatial domain in which the platform is operating. WSF_SPATIAL_DOMAIN_LAND
//! is returned if this method is not overridden by a derived class.
// virtual
WsfSpatialDomain WsfMover::GetSpatialDomain() const
{
   return WSF_SPATIAL_DOMAIN_LAND;
}

// ================================================================================================
//! Return the pointer to the current route.
//! @returns A pointer to the current route or zero if a current route is not defined.
// virtual
const WsfRoute* WsfMover::GetRoute() const
{
   return nullptr;
}

// ================================================================================================
//! Executes scripts, callbacks, and mode changes (if any) for the given route index waypoint.
//! @param aSimTime Current simulated time.
//! @param aPointIndex Route waypoint array index to execute a script or callback for.
//! @returns Indicates whether a script, or callback was executed, or a Mover mode was changed.
//! @note Executing a script or callback may possibly change the current or destination waypoint.
// virtual
bool WsfMover::ExecuteTriggers(double aSimTime, unsigned int aPointIndex)
{
   return false;
}

// ================================================================================================
//! Return the pointer to the default route.
//! Some movers implement the concept of a 'default' route, which is the initial route.
//! @returns A pointer to the default route or zero if a default route is not defined.
// virtual
const WsfRoute* WsfMover::GetDefaultRoute() const
{
   return nullptr;
}

// ================================================================================================
//! Command the mover change its altitude.
//! @param aSimTime       The current simulated time (seconds).
//! @param aAltitude      The desired altitude to attempt to achieve (in meters).
//! @param aClimbDiveRate The desired climb/dive rate (meters/second). If zero then use the mover default.
//! @param aKeepRoute     'true' if the mover should maintain current route.  'false' to extrapolate.
//! @returns true if successful.
// virtual
bool WsfMover::GoToAltitude(double aSimTime, double aAltitude, double aClimbDiveRate, bool aKeepRoute)
{
   return false;
}

// ================================================================================================
//! After going elsewhere, rejoin the route, reverting to the previous waypoint.
//! @param aSimTime The current simulated time (seconds).
//! @returns true if successful.
// virtual
bool WsfMover::ReturnToRoute(double aSimTime)
{
   return false;
}

// ================================================================================================
//! Go to the closest point in the current route.
//! @param aSimTime  The current simulated clock time (seconds).
//! @returns true if successful.
// virtual
bool WsfMover::GoToClosestPoint(double aSimTime)
{
   return false;
}

// ================================================================================================
//! Go to the point with the specified label in the current route.
//! @param aSimTime  The current simulated clock time (seconds).
//! @param aLabelId The name (string id) of the point.
//! @returns true if successful.
// virtual
bool WsfMover::GoToLabel(double aSimTime, WsfStringId aLabelId)
{
   return false;
}

// ================================================================================================
//! Command the mover to move towards a specified location.
//! @param aSimTime   The current simulated clock time (seconds).
//! @param aLatitude  The desired latitude.
//! @param aLongitude The desired longitude.
//! @param aAltitude  The desired altitude.
//!                      If -1 is used for aAltitude the current altitude is used.
//! @returns true if successful.
// virtual
bool WsfMover::GoToLocation(double aSimTime, double aLatitude, double aLongitude, double aAltitude)
{
   return false;
}

// ================================================================================================
//! Command the mover to change its speed.
//! @param aSimTime     The current simulated clock time (seconds).
//! @param aSpeed       The desired speed (in meters/second).
//! @param aLinearAccel The desired acceleration (meters/second^2). If zero then use the mover default.
//! @param aKeepRoute     'true' if the mover should maintain current route.  'false' to extrapolate.
//! @returns true if successful.
// virtual
bool WsfMover::GoToSpeed(double aSimTime, double aSpeed, double aLinearAccel, bool aKeepRoute)
{
   return false;
}

// ================================================================================================
//! Command the mover to change its heading.
//! @param aSimTime       The current simulated clock time (seconds).
//! @param aHeading       The desired heading (in radians).
//! @param aRadialAccel   The desired radial acceleration (meters/second^2). If zero then use the mover default.
//! @param aTurnDirection The direction in which to turn.
//! @returns true if successful.
// virtual
bool WsfMover::TurnToHeading(double aSimTime, double aHeading, double aRadialAccel, WsfPath::TurnDirection aTurnDirection)
{
   return false;
}

// ================================================================================================
//! Command the mover to change its relative heading.
//! @param aSimTime       The current simulated clock time (seconds).
//! @param aHeadingChange The desired heading change with positive clockwise, negative counter-clockwise. (in radians).
//! @param aRadialAccel   The desired radial acceleration (meters/second^2). If zero then use the mover default.
//! @param aTurnDirection The direction in which to turn.
//! @returns true if successful.
// virtual
bool WsfMover::TurnToRelativeHeading(double                 aSimTime,
                                     double                 aHeadingChange,
                                     double                 aRadialAccel,
                                     WsfPath::TurnDirection aTurnDirection)
{
   return false;
}

// ================================================================================================
//! Set the route.  Replaces any that might have already been set.
//! @param aSimTime   The current simulated clock time (seconds).
//! @param aRoute     The new route.
//! @returns true if successful.
// virtual
bool WsfMover::SetRoute(double aSimTime, const WsfRoute& aRoute)
{
   return false;
}

// ================================================================================================
//! Update the route.
//! @param aSimTime   The current simulated clock time (seconds).
//! @param aRoute     The new route.
//! @returns true if successful.
// virtual
bool WsfMover::UpdateRoute(double aSimTime, const WsfRoute& aRoute)
{
   return false;
}

// ================================================================================================
//! Update the default route to the current route.
//! @returns true if successful.
// virtual
bool WsfMover::UpdateDefaultRoute()
{
   return false;
}

// ================================================================================================
//! Attempt to find a WsfWaypoint index, and go there.
//! @param aSimTime The current simulated clock time (seconds).
//! @param aDestination Waypoint index to find.
//! @returns true if successful.
// virtual
bool WsfMover::GoToWaypoint(double aSimTime, unsigned int aDestination)
{
   return false;
}

// ================================================================================================
// virtual
bool WsfMover::FindAndSetPath(double aSimTime, WsfGeoPoint* aStartPtr, WsfGeoPoint* aEndPtr)
{
   return false;
}

// ================================================================================================
//! Get the anticipated location at some future time
//! @param aSimTime A time in the future for which the location is to be determined.
//! @param aLocationWCS The location at the time specified by aSimTime.
//! @returns false if the future location cannot be determined.
//! @note Not all movers support this method.
// virtual
bool WsfMover::GetFutureLocationWCS(double aSimTime, double aLocationWCS[3])
{
   UtVec3d::Set(aLocationWCS, 0.0);
   return false;
}

// ================================================================================================
//! Set the current mode ID of the WsfMover.  This value is implementation defined,
//! and may be used to indicate "Stopped", "Idle", "Taxi", "Climb", or any other
//! state or mode of interest.  Derived WsfMover implementations may modify their
//! behavior based upon this value.
// virtual
void WsfMover::SetMode(WsfStringId aModeName)
{
   mModeId = aModeName;
}

// ================================================================================================
//! Return whether the mover (and its attached platform) is stopped.
// virtual
bool WsfMover::IsStopped() const
{
   return false;
}

// ================================================================================================
//! Indicates that the platform is currently in "pause" mode, and is not moving, but may begin
//! moving at some future time.  Note that a "paused" platform may supply a non-zero velocity
//! and/or acceleration, but still not move through space with advancing time.
// virtual
bool WsfMover::IsPaused() const
{
   return false;
}

// ================================================================================================
//! Return whether the mover is "extrapolating".
//! 'Extrapolating' has a slightly different meaning depending on the mover, but it fundamentally
//! means it is moving without any form of external guidance. In the case of a simple waypoint-
//! following mover this means it has reached its final waypoint and has either stopped there
//! or is continuing on at a specified direction, speed, and altitude. For movers that do not follow
//! waypoints it is a little murkier, but it typically means that the motion is occuring without any
//! propulsion. In this latter case it basically means that with a little knowledge an external
//! observer could take the position at any time and use Newton's Laws to estimate future positions.
// virtual
bool WsfMover::IsExtrapolating() const
{
   return false;
}

// ================================================================================================
WsfAttributeContainer& WsfMover::GetProperties() const
{
   if (mPropertiesPtr == nullptr)
   {
      mPropertiesPtr = new WsfAttributeContainer;
   }
   return *mPropertiesPtr;
}

// ================================================================================================
void WsfMover::RegisterProperty(const std::string& aPropertyName, const int& aProperty)
{
   GetProperties().Assign(aPropertyName, aProperty);
}

// ================================================================================================
void WsfMover::RegisterProperty(const std::string& aPropertyName, const double& aProperty)
{
   GetProperties().Assign(aPropertyName, aProperty);
}

// ================================================================================================
void WsfMover::RegisterProperty(const std::string& aPropertyName, const std::string& aProperty)
{
   GetProperties().Assign(aPropertyName, aProperty);
}

// ================================================================================================
bool WsfMover::PropertyExists(const std::string& aPropertyName)
{
   return HasProperties() && GetProperties().AttributeExists(aPropertyName);
}

// ================================================================================================
const char* WsfMover::GetScriptClassName() const
{
   return "WsfMover";
}

// ================================================================================================
// virtual
void WsfMover::TriggerExtrapolation() {}

// ================================================================================================
const int* WsfMover::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_MOVER, cWSF_COMPONENT_PLATFORM_PART, cWSF_COMPONENT_NULL};
   return roles;
}

// ================================================================================================
void* WsfMover::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_MOVER)
   {
      return this;
   }
   return WsfPlatformPart::QueryInterface(aRole);
}

// =================================================================================================
// Nested class 'KinematicModel'
// ================================================================================================
WsfMover::KinematicModel::KinematicModel(double aSimTime, WsfStringId aModelTypeId)
   : mStartTime(aSimTime)
   , mApogeeTime(0.0)
   , mTimeAtLocation(0.0)
   , mModelTypeId(aModelTypeId)
{
}

// ================================================================================================
WsfMover::KinematicModel::~KinematicModel() {}

// ================================================================================================
// static
void WsfMover::KinematicModel::GetRangeAndAltitude(const double aOriginLocXYZ[3],
                                                   const double aEndLocXYZ[3],
                                                   double&      aRange,
                                                   double&      aAltitude)
{
   double endLocMag = UtVec3d::Magnitude(aEndLocXYZ);
   aRange = acos(UtVec3d::DotProduct(aEndLocXYZ, aOriginLocXYZ) / (endLocMag * UtVec3d::Magnitude(aOriginLocXYZ))) *
            UtSphericalEarth::cEARTH_RADIUS;
   aAltitude = endLocMag - UtSphericalEarth::cEARTH_RADIUS;
}
