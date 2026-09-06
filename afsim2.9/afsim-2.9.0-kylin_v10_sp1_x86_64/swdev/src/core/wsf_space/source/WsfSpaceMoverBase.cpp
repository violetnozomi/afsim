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

#include "WsfSpaceMoverBase.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

#include "UtCalendar.hpp"
#include "UtColor.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtOrbitalPropagator.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "UtOrbitalState.hpp"
#include "UtSolarSystem.hpp"
#include "UtSphericalEarth.hpp"
#include "UtTwoLineElement.hpp"
#include "UtVec3.hpp"
#include "WsfAttitudeControllerTypes.hpp"
#include "WsfConjunctionInput.hpp"
#include "WsfDateTime.hpp"
#include "WsfDraw.hpp"
#include "WsfInstantAttitudeController.hpp"
#include "WsfOrbitalManeuver.hpp"
#include "WsfOrbitalManeuvering.hpp"
#include "WsfOrbitalManeuveringTypes.hpp"
#include "WsfOrbitalMissionSequence.hpp"
#include "WsfOrbitalPropagator.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimpleOrbitalManeuvering.hpp"
#include "WsfSimulation.hpp"
#include "WsfSpaceObserver.hpp"
#include "WsfSpaceOrbitalMissionVerificationContext.hpp"
#include "observer/WsfMoverObserver.hpp"
#include "script/WsfScriptContext.hpp"

// static
namespace // anonymous
{
bool sSuppressSpaceMoverBaseWarnings = false;
}

WsfSpaceMoverBase::WsfSpaceMoverBase(const WsfScenario&                       aScenario,
                                     std::unique_ptr<UtOrbitalPropagatorBase> aPropagatorPtr,
                                     const std::string&                       aScriptClassName)
   : WsfMover(aScenario)
   , mPropagatorPtr(move(aPropagatorPtr))
   , mManeuveringPtr(new WsfSimpleOrbitalManeuvering{})
   , mAttitudePtr(new WsfInstantAttitudeController())
   , mConjunctionInputPtr(nullptr)
   , mMissionContext(*this)
   , mSimTimeOfEpoch(0.0)
   , mContextPtr(new WsfScriptContext(*aScenario.GetScriptContext(), aScriptClassName, "SPACE_MOVER"))
{
}

WsfSpaceMoverBase::WsfSpaceMoverBase(const WsfSpaceMoverBase& aSrc)
   : WsfMover(aSrc)
   , mPropagatorPtr(aSrc.mPropagatorPtr)
   , mManeuveringPtr(aSrc.mManeuveringPtr)
   , mAttitudePtr(aSrc.mAttitudePtr)
   , mConjunctionInputPtr(aSrc.mConjunctionInputPtr)
   , mMissionContext(*this, aSrc.mMissionContext)
   , mSimTimeOfEpoch(aSrc.mSimTimeOfEpoch)
   , mContextPtr(new WsfScriptContext(*aSrc.mContextPtr))
   , mOrbitColor(aSrc.mOrbitColor)
{
   // Set update time tolerance as the time required to travel 1 meter at 17000 mi/hr (about 1.3E-4)
   static const double sUpdateTimeTolerance = 1.0 / (17000.0 * 5280.0 * UtMath::cFT_PER_M / 3600.0);
   mUpdateTimeTolerance                     = sUpdateTimeTolerance;
   mAttitudePtr->SetMover(this);
}

WsfSpaceMoverBase::~WsfSpaceMoverBase()
{
   delete mContextPtr;
}

// virtual
WsfMover* WsfSpaceMoverBase::Clone() const
{
   return new WsfSpaceMoverBase(*this);
}

const int* WsfSpaceMoverBase::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_SPACE_MOVER,
                               cWSF_COMPONENT_MOVER,
                               cWSF_COMPONENT_PLATFORM_PART,
                               cWSF_COMPONENT_NULL};
   return roles;
}

void* WsfSpaceMoverBase::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_SPACE_MOVER)
   {
      return this;
   }
   return WsfMover::QueryInterface(aRole);
}

bool WsfSpaceMoverBase::PreInitialize(double aSimTime)
{
   if (GetInitialOrbitalState().GetCentralPoint().GetName() == GetPlatform()->GetCentralPoint().GetName())
   {
      // Copy the platform's central body, as it may have been configured with polar offsets or other attributes.
      mPropagatorPtr->SetInitialCentralPoint(GetPlatform()->GetCentralPoint());
   }
   if (GetInitialOrbitalState().HasPlatformCreationEpoch())
   {
      ut::OrbitalState updatedState(mPropagatorPtr->GetInitialOrbitalState());
      UtCalendar       creationEpoch;
      GetSimulation()->GetDateTime().GetCurrentTime(GetPlatform()->GetCreationTime(), creationEpoch);
      updatedState.SetEpoch(creationEpoch);
      mPropagatorPtr->SetInitialOrbitalState(updatedState);
   }
   if (!mTLE_InputAllowed && GetInitialOrbitalState().IsTLEInput() && GetInitialOrbitalState().IsDeclaredWithOrbitalState())
   {
      // TLE input is not allowed for any non-NORAD space mover when initialized with
      // the orbital_state block
      auto logger = ut::log::error() << "Initialized non-NORAD space mover with TLE";
      logger.AddNote() << "Platform: " << GetPlatform()->GetName();
      logger.AddNote() << "Mover: " << GetPlatform()->GetMover()->GetName();
      throw UtException("Initialized non-NORAD space mover with TLE");
   }
   return true;
}

const char* WsfSpaceMoverBase::GetScriptClassName() const
{
   return "WsfSpaceMover";
}

// virtual
bool WsfSpaceMoverBase::Initialize(double aSimTime)
{
   bool ok = (mPropagatorPtr != nullptr);
   if (ok)
   {
      mContextPtr->SetParent(&GetPlatform()->GetScriptContext());
      UtCalendar simStartTime = GetSimulation()->GetDateTime().GetStartDateAndTime();
      UtCalendar initialTime  = GetSimulation()->GetDateTime().GetCurrentTime(aSimTime);
      mPropagatorPtr->InitializeOrbitalState(initialTime);
      if (mConjunctionInputPtr == nullptr) // standard initialization.
      {
         initialTime           = mPropagatorPtr->GetInitialOrbitalState().GetEpoch();
         mSimTimeOfEpoch       = initialTime.GetTimeSince(simStartTime);
         double timeSinceEpoch = -mSimTimeOfEpoch + aSimTime;
         initialTime.AdvanceTimeBy(timeSinceEpoch);
      }
      else // (mConjunctionInputPtr != nullptr)
      {
         mSimTimeOfEpoch               = initialTime.GetTimeSince(simStartTime);
         ut::OrbitalState initialState = GetInitialOrbitalState();
         initialState.SetReferenceFrame(ut::OrbitalState::ReferenceFrame::cECI);
         initialState.SetCoordinateSystem(ut::OrbitalState::CoordinateSystem::cEQUATORIAL);
         mConjunctionInputPtr->Initialize(initialTime, initialState);
         initialState.Set(initialTime,
                          ut::OrbitalState::Vector(mConjunctionInputPtr->InitialLocationECI(),
                                                   mConjunctionInputPtr->InitialVelocityECI()));
         mPropagatorPtr->SetInitialOrbitalState(initialState);
         // NOTE: mConjunctionInputPtr is reset in Initialize2
      }

      mManeuveringPtr->SetName(GetPlatform()->GetName() + "_maneuvering");

      ok = mPropagatorPtr->Initialize(initialTime) && // Propagator initializes with time since epoch, not aSimTime.
           WsfMover::Initialize(aSimTime) && mManeuveringPtr->Initialize(initialTime) &&
           mContextPtr->Initialize(aSimTime, GetPlatform(), this);

      // Some propagators may want to know if they are attached to a space mover. Those classes of
      // propagator derive from wsf::space::OrbitalPropagator. They are notified of the linkage to
      // their owning space mover here.
      try
      {
         auto& wsfProp = dynamic_cast<wsf::space::OrbitalPropagator&>(*mPropagatorPtr);
         wsfProp.SetSpaceMover(this);
      }
      catch (std::bad_cast&)
      {
         // Do nothing.
      }

      if (ok)
      {
         mOrbitalState = GetPropagator().GetOrbitalState();
         // Set the central body if different from the propagator.
         if (mOrbitalState.GetCentralPoint().GetName() != GetPlatform()->GetCentralPoint().GetName())
         {
            mOrbitalState.SetCentralPoint(GetPlatform()->GetCentralPoint());
         }

         UpdatePropagation(initialTime);

         // Set the initial orientation to match that of the propagator
         double psi, theta, phi;
         if (!mAttitudePtr->CalculateOrientation(psi, theta, phi))
         {
            GetPlatform()->GetOrientationECI(psi, theta, phi);
            mAttitudePtr->SetTargetOrientationECI(psi, theta, phi);
         }
         GetPlatform()->SetOrientationECI(psi, theta, phi);
      }
   }

   return ok;
}

bool WsfSpaceMoverBase::Initialize2(double aSimTime)
{
   if (mConjunctionInputPtr != nullptr)
   {
      UtCalendar epoch = GetSimulation()->GetDateTime().GetStartDateAndTime();
      epoch.AdvanceTimeBy(aSimTime);
      mConjunctionInputPtr->ComputeSolution(*GetSimulation(), epoch, GetPropagator());
      if (mConjunctionInputPtr->SolutionFound())
      {
         ut::OrbitalState initialState = GetInitialOrbitalState();
         initialState.SetReferenceFrame(ut::OrbitalState::ReferenceFrame::cECI);
         initialState.Set(epoch,
                          ut::OrbitalState::Vector(mConjunctionInputPtr->InitialLocationECI(),
                                                   mConjunctionInputPtr->SolutionVelocityECI()));
         mPropagatorPtr->SetInitialOrbitalState(initialState);
         mConjunctionInputPtr.reset(nullptr);
         WsfSpaceMoverBase::Initialize(aSimTime);
      }
      else
      {
         mConjunctionInputPtr.reset(nullptr);
         auto logger = ut::log::warning() << "The input conjunction for specified platform is impossible. Platform "
                                             "will continue on default (circular) orbit.";
         logger.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
   }

   const UtCalendar& epoch = GetPropagator().GetCurrentTime();
   bool              ok    = mContextPtr->Initialize2(aSimTime);

   if (ok)
   {
      // Call Update (this class' method only)
      // to propagate up to "aSimTime."
      WsfSpaceMoverBase::Update(aSimTime);

      // Note: maneuvers are initialized here because we only want to do it once;
      // the act of maneuvering may cause re-initialization!
      ok = VerifyMission(GetPropagator().GetCurrentTime());
      if (ok)
      {
         ok = mMissionContext.Initialize(epoch);
      }
   }
   return ok;
}

// virtual
bool WsfSpaceMoverBase::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   if (mPropagatorPtr->ProcessInput(aInput))
   {
   }
   else if (mMissionContext.ProcessInput(aInput))
   {
   }
   else if (mContextPtr->ProcessInput(aInput))
   {
   }
   else if (command == "suppress_warnings")
   {
      aInput.ReadValue(sSuppressSpaceMoverBaseWarnings);
   }
   else if (command == "attitude_controller")
   {
      UtInputBlock inputBlock(aInput);

      std::string type;
      aInput.ReadValue(type);
      WsfAttitudeController* attitudePtr = WsfAttitudeControllerTypes::Get(GetScenario()).Find(type);
      if (attitudePtr != nullptr)
      {
         mAttitudePtr = ut::clone(attitudePtr);
         mAttitudePtr->SetMover(this);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }

      while (inputBlock.ReadCommand())
      {
         if (!mAttitudePtr->ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (command == "maneuvering")
   {
      std::string type;
      aInput.ReadValue(type);
      WsfOrbitalManeuvering* maneuveringPtr = WsfOrbitalManeuveringTypes::Get(GetScenario()).Find(type);
      if (maneuveringPtr != nullptr)
      {
         mManeuveringPtr = ut::clone(maneuveringPtr);
      }
      UtInputBlock maneuveringBlock(aInput);
      while (maneuveringBlock.ReadCommand())
      {
         myCommand = mManeuveringPtr->ProcessInput(aInput);
         if (!myCommand)
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (command == "conjunction_setup")
   {
      mConjunctionInputPtr = ut::make_unique<WsfConjunctionInput>();
      myCommand            = mConjunctionInputPtr->ProcessInput(aInput);
   }
   else if (command == "orbit_color")
   {
      aInput.ReadValue(mOrbitColor.emplace());
   }
   else if (command == "orbit_color_rgb") // DEPRECATED
   {
      auto warn = ut::log::warning() << "The 'orbit_color_rgb' input has been deprecated.";
      warn.AddNote() << "Please use 'orbit_color' instead.";
      aInput.ReadValue(mOrbitColor.emplace());
   }
   else
   {
      myCommand = WsfMover::ProcessInput(aInput);
   }
   return myCommand;
}

// virtual
void WsfSpaceMoverBase::Update(double aSimTime)
{
   double updateInterval = aSimTime - mLastUpdateTime;
   if (fabs(updateInterval) > mUpdateTimeTolerance)
   {
      GetPlatform()->SetTime(aSimTime);
      UtCalendar epoch = GetSimulation()->GetDateTime().GetStartDateAndTime();
      epoch.AdvanceTimeBy(aSimTime);

      // The following two lines are order dependent, as the mission context may
      // be performing a maneuver that affects the propagator.
      mMissionContext.Update(epoch);
      mContextPtr->Update(aSimTime);
      UpdatePropagation(epoch);
      mAttitudePtr->Update(updateInterval);

      mLastUpdateTime = aSimTime;
   }
}

//! Update the propagation and the associated platform.  This method is usually only called
//! from the Update method; however, in some cases it is called to update the state of the platform instantaneously,
//! especially during execution of an impulsive maneuver.
//!@param aEpoch The time to which the propagation and platform state will be updated.
void WsfSpaceMoverBase::UpdatePropagation(const UtCalendar& aEpoch)
{
   mPropagatorPtr->Update(aEpoch);

   mOrbitalState.Set(mPropagatorPtr->GetOrbitalState());
   auto orbitalStateVector = mOrbitalState.GetOrbitalStateVector(ut::OrbitalState::ReferenceFrame::cECI);

   // Set resulting state in the platform
   GetPlatform()->SetLocationECI(orbitalStateVector.GetLocation().GetData());

   GetPlatform()->SetVelocityECI(orbitalStateVector.GetVelocity().GetData());

   GetPlatform()->SetAccelerationECI(mOrbitalState.GetAccelerationInertial().GetData());

   mPropagatorPtr->PrintDebug(mPropagatorPtr->GetTimePastEpoch());
}

double WsfSpaceMoverBase::GetAvailableDeltaV() const
{
   return mManeuveringPtr->GetAvailableDeltaV();
}

double WsfSpaceMoverBase::GetRequiredDuration(double aDeltaV) const
{
   return mManeuveringPtr->GetRequiredDuration(aDeltaV);
}

double WsfSpaceMoverBase::GetRequiredDeltaV(double aDuration) const
{
   return mManeuveringPtr->GetRequiredDeltaV(aDuration);
}

double WsfSpaceMoverBase::GetManeuverDuration(const UtCalendar& aEpoch, const WsfOrbitalManeuver& aManeuver, double aDeltaV) const
{
   return mManeuveringPtr->GetManeuverDuration(aEpoch, aManeuver, aDeltaV);
}

bool WsfSpaceMoverBase::PerformStagingOperation()
{
   bool staged = mManeuveringPtr->PerformStagingOperation();
   if (staged)
   {
      if (GetSimulation() != nullptr)
      {
         WsfSimulation* simPtr = GetSimulation();
         WsfObserver::MoverStaged(simPtr)(simPtr->GetSimTime(), this);
      }
   }
   return staged;
}

const ut::CentralBody& WsfSpaceMoverBase::GetCentralBody() const
{
   return mPropagatorPtr->GetOrbitalState().GetCentralBody();
}

const ut::CentralPoint& WsfSpaceMoverBase::GetCentralPoint() const
{
   return mPropagatorPtr->GetOrbitalState().GetCentralPoint();
}

bool WsfSpaceMoverBase::Maneuver(const UtCalendar& aEpoch, const UtVec3d& aCommandedDeltaV, UtVec3d& aActualDeltaV)
{
   bool maneuvered = mManeuveringPtr->Maneuver(aEpoch,
                                               static_cast<const WsfOrbitalManeuver&>(*GetCurrentMissionEvent()),
                                               aCommandedDeltaV,
                                               aActualDeltaV);
   if (maneuvered && (aActualDeltaV.MagnitudeSquared() > 0.0))
   {
      AddDeltaV(aActualDeltaV);
   }
   return maneuvered;
}

bool WsfSpaceMoverBase::SetInitialPoint(double aLat, double aLon, double aAlt, double aSpeed, double aHeading)
{
   return false;
}

//! A convenience accessor which calculates a WsfSpaceMoverBase's initial position from its orbital elements.
//! Extended to WsfSpaceMoverScriptClass for exposure to the user.
//! @return Initial Position (Reference Frame: Latitude, Longitude, Altitude))
WsfGeoPoint WsfSpaceMoverBase::GetInitialPositionLLA() const
{
   double lat, lon, alt;
   GetCentralBody().ConvertWCS_ToLLA(GetInitialOrbitalState().GetOrbitalStateVectorWCS().GetLocation(), lat, lon, alt);
   return WsfGeoPoint(lat, lon, alt);
}

//! A convenience accessor to determine a WsfSpaceMoverBase's initial position.
//! Extended to WsfSpaceMoverScriptClass for exposure to the user.
//! @return Initial Position (Reference Frame: Earth Centered Inertial (ECI) / Geocentric Celestial Reference Frame (GCRF))
UtVec3d WsfSpaceMoverBase::GetInitialPositionECI() const
{
   return GetInitialOrbitalState().GetOrbitalStateVectorInertial().GetLocation();
}

//! A convenience accessor to determine a WsfSpaceMoverBase's initial position.
//! Extended to WsfSpaceMoverScriptClass for exposure to the user.
//! @return Initial Position (Reference Frame: World Coordinate System (WCS) / Earth-Centered, Earth-Fixed (ECEF))
UtVec3d WsfSpaceMoverBase::GetInitialPositionWCS() const
{
   return GetInitialOrbitalState().GetOrbitalStateVectorWCS().GetLocation();
}

//! A convenience accessor to determine a WsfSpaceMoverBase's initial velocity.
//! Extended to WsfSpaceMoverScriptClass for exposure to the user.
//! @return Initial Velocity (Reference Frame: Earth Centered Inertial (ECI) / Geocentric Celestial Reference Frame (GCRF))
UtVec3d WsfSpaceMoverBase::GetInitialVelocityECI() const
{
   return GetInitialOrbitalState().GetOrbitalStateVectorInertial().GetVelocity();
}

//! A convenience accessor to determine a WsfSpaceMoverBase's initial velocity.
//! Extended to WsfSpaceMoverScriptClass for exposure to the user.
//! @return Initial Velocity (Reference Frame: World Coordinate System (WCS) / Earth-Centered, Earth-Fixed (ECEF))
UtVec3d WsfSpaceMoverBase::GetInitialVelocityWCS() const
{
   return GetInitialOrbitalState().GetOrbitalStateVectorWCS().GetVelocity();
}

//! Return the initial unpropagated orbital state.
const ut::OrbitalState& WsfSpaceMoverBase::GetInitialOrbitalState() const
{
   return mPropagatorPtr->GetInitialOrbitalState();
}

double WsfSpaceMoverBase::GetInitialHeading() const
{
   UtVec3d velocityWCS = GetInitialOrbitalState().GetOrbitalStateVectorWCS().GetVelocity();

   UtVec3d velocityNED;
   GetPlatform()->ConvertWCSVectorToNED(velocityNED.GetData(), velocityWCS.GetData());

   return std::atan2(velocityNED[1], velocityNED[0]);
}

//! Return the WCS location at some time aSimTime in the future.
bool WsfSpaceMoverBase::GetFutureLocationWCS(double aSimTime, double aLocationWCS[3])
{
   bool retval{false};

   UtCalendar predictionTime = GetSimulation()->GetDateTime().GetCurrentTime(aSimTime);
   UtCalendar lastUpdateTime = GetSimulation()->GetDateTime().GetCurrentTime(mLastUpdateTime);

   // Do not predict into the past.
   if (lastUpdateTime < predictionTime)
   {
      // Predict only if there is no currently executing event, or if the start time of the current event is after the
      // requested time.
      const auto* currentEventPtr = GetCurrentMissionEvent();

      bool shouldPredict;
      if (currentEventPtr->IsComplete())
      {
         // This branch covers a mission sequence that has completed.
         shouldPredict = true;
      }
      else if (currentEventPtr->HasExecuted())
      {
         // This branch covers a currently executing finite maneuver.
         shouldPredict = false;
      }
      else if (currentEventPtr->IsSequence())
      {
         // This branch occurs when the first event of a sequence has not begun.
         auto sequencePtr = static_cast<const WsfOrbitalMissionSequence*>(currentEventPtr);
         if (!sequencePtr->GetSequence().empty())
         {
            // If there are events in the sequence, we need to check that the prediction time
            // is before the start time of the sequence.
            //
            // This does not treat the case where the first event has a start time that is
            // after the prediction time (and not just the condition of the sequence, as is
            // checked here). But there is no way to test the start time of the first event
            // as that will not be set until the event is initialized, which does not happen
            // until the sequence gets past its own condition.
            shouldPredict = predictionTime < currentEventPtr->GetStartTime();
         }
         else
         {
            // If there are no events, it is an empty sequence, and so there is no restriction on
            // the prediction. However, the start time is not set, so this extra branch is needed.
            shouldPredict = true;
         }
      }
      else
      {
         shouldPredict = predictionTime < currentEventPtr->GetStartTime();
      }

      if (shouldPredict)
      {
         // Predict forward
         auto propPtr = ut::clone(GetPropagator());
         propPtr->Initialize(lastUpdateTime);
         propPtr->Update(predictionTime);
         UtVec3d posECI = propPtr->GetOrbitalState().GetOrbitalStateVectorInertial().GetLocation();

         // So that the same ECI conversion is used, with correct precession and nutation, a copy of the
         // entity needs to be made, updated to aSimTime, and then used to convert the ECI position to
         // WCS.
         UtEntity testEntity = *GetPlatform();
         testEntity.SetTime(aSimTime);
         testEntity.SetLocationECI(posECI.GetData());
         testEntity.GetLocationWCS().Get(aLocationWCS);

         retval = true;
      }
   }
   return retval;
}

//! Return the current propagated orbital state.
const ut::OrbitalState& WsfSpaceMoverBase::GetOrbitalState() const
{
   return mPropagatorPtr->GetOrbitalState();
}

const WsfOrbitalManeuvering& WsfSpaceMoverBase::GetOrbitalManeuvering() const
{
   return *mManeuveringPtr;
}

bool WsfSpaceMoverBase::InitializeOrbitalManeuvering(const UtCalendar& aEpoch)
{
   return mManeuveringPtr->Initialize(aEpoch);
}

const WsfOrbitalEvent* WsfSpaceMoverBase::GetCurrentMissionEvent() const
{
   return mMissionContext.GetMissionEvents().GetCurrentMissionEvent();
}

const WsfOrbitalMissionSequence& WsfSpaceMoverBase::GetMissionEvents() const
{
   return mMissionContext.GetMissionEvents();
}

void WsfSpaceMoverBase::ScheduleMission(const UtCalendar&                          aEpoch,
                                        std::unique_ptr<WsfOrbitalMissionSequence> aMissionEventsPtr)
{
   mMissionContext.ScheduleMission(aEpoch, std::move(aMissionEventsPtr));
}

void WsfSpaceMoverBase::CancelMission()
{
   auto emptySequence = ut::make_unique<WsfOrbitalMissionSequence>(GetScenario());
   mMissionContext.ScheduleMission(mMissionContext.GetLastUpdateTime(), move(emptySequence));
}

bool WsfSpaceMoverBase::VerifyMission(const UtCalendar&                 aEpoch,
                                      const WsfOrbitalMissionSequence&  aSequence,
                                      wsf::space::verify::PreOperation  aPreVerifyOp,
                                      wsf::space::verify::PostOperation aPostVerifyOp) const
{
   bool verified = true;
   if (!aSequence.GetSequence().empty())
   {
      wsf::space::OrbitalMissionVerificationContext verifyContext{GetOrbitalManeuvering(),
                                                                  GetPropagator(),
                                                                  aSequence,
                                                                  GetPlatform()->GetName()};
      verifyContext.Initialize(aEpoch);
      verified = verifyContext.VerifyMission();
   }

   return verified;
}

bool WsfSpaceMoverBase::VerifyMission(const UtCalendar& aEpoch) const
{
   return VerifyMission(aEpoch, GetMissionEvents());
}

// override
WsfMover::KinematicModel* WsfSpaceMoverBase::CreateKinematicModel(double       aSimTime,
                                                                  WsfStringId  aModelTypeId,
                                                                  const double aLocationWCS[3],
                                                                  const double aVelocityWCS[3],
                                                                  WsfMover*    aMoverPtr,
                                                                  WsfDraw*     aDrawPtr) const
{
   SpaceModel* spaceModel = nullptr;
   // We currently only deal with Earth-orbiting bodies using equatorial coordinates. It is
   // the only way we get WCS coordinates. This makes sense because the platform is assumed
   // to be launched from Earth.
   auto       propagatorPtr = ut::clone(GetPropagator());
   UtCalendar epoch;
   if (aMoverPtr != nullptr) // Initialize based on the provided mover
   {
      epoch = aMoverPtr->GetSimulation()->GetDateTime().GetStartDateAndTime();
   }
   else
   {
      epoch = GetSimulation()->GetDateTime().GetStartDateAndTime();
   }
   epoch.AdvanceTimeBy(aSimTime);
   if (aMoverPtr != nullptr)
   {
      UtECI_Conversion conv(epoch, aMoverPtr->GetPlatform()->GetCentralPoint());
      conv.SetLocationWCS(aLocationWCS);
      conv.SetVelocityWCS(aVelocityWCS);
      ut::OrbitalState state(epoch,
                             aMoverPtr->GetPlatform()->GetCentralPoint(),
                             ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                             ut::OrbitalState::ReferenceFrame::cECI,
                             ut::OrbitalStateVector(conv.GetLocationECI(), conv.GetVelocityECI()));
      propagatorPtr->Initialize(state);
   }

   propagatorPtr->Update(epoch);
   const auto& orbitalState = propagatorPtr->GetOrbitalState();
   if (orbitalState.GetCoordinateSystem() == ut::OrbitalState::CoordinateSystem::cEQUATORIAL)
   {
      spaceModel = new SpaceModel(aSimTime, aModelTypeId, std::move(propagatorPtr), aDrawPtr);
   }
   return spaceModel;
}

// =================================================================================================
// Nested class SpaceModel.
// =================================================================================================
WsfSpaceMoverBase::SpaceModel::SpaceModel(double                                   aSimTime,
                                          WsfStringId                              aModelTypeId,
                                          std::unique_ptr<UtOrbitalPropagatorBase> aPropagatorPtr,
                                          WsfDraw*                                 aDrawPtr /* = 0 */)
   : WsfMover::KinematicModel(aSimTime, aModelTypeId)
   , mPropagatorPtr(std::move(aPropagatorPtr))
   , mUpdateTime(mPropagatorPtr->GetCurrentTime())
   , mLastUpdateTime(aSimTime)
{
   // The apogee and impact times must be large to indicate that the conditions are not encountered.
   // This must be large so the evaluator will compute a solution.
   mApogeeTime     = 1.0E+30;
   mTimeAtLocation = 1.0E+30;

   mLastLocXYZ.Set(0.0);
   mLastLocWCS.Set(0.0);
   mLastVelWCS.Set(0.0);

   // Draw the trajectory if requested.
   if (aDrawPtr != nullptr)
   {
      // Draw one complete orbit..
      double timePerOrbit  = UtMath::cTWO_PI / mPropagatorPtr->GetOrbitalState().GetOrbitalElements().GetMeanMotion();
      double timeIncrement = 1.0;
      aDrawPtr->BeginPolyline();
      for (double orbitTime = 0.0; orbitTime < timePerOrbit + 0.99; orbitTime += timeIncrement)
      {
         Update(aSimTime + orbitTime);
         aDrawPtr->VertexWCS(mLastLocWCS.GetData());
      }
      aDrawPtr->End();
   }
}

// =================================================================================================
bool WsfSpaceMoverBase::SpaceModel::GetLocationXYZ(double aTime, double aLocXYZ[3]) const
{
   bool valid(false);
   if (Update(aTime))
   {
      UtVec3d::Set(aLocXYZ, mLastLocXYZ.GetData());
      valid = true;
   }
   return valid;
}

// =================================================================================================
bool WsfSpaceMoverBase::SpaceModel::GetVelocityXYZ(double aTime, double aVelXYZ[3]) const
{
   // NOTE: This routine does not need to be particularly efficient and accurate.
   bool valid(false);
   if (Update(aTime))
   {
      // The NED velocity components are assume to be the same in the spherical and ellipsoidal frames.
      double lat, lon, alt;
      double xformWCS_ToNED[3][3];
      UtEllipsoidalEarth::ConvertECEFToLLA(mLastLocWCS.GetData(), lat, lon, alt);
      UtEllipsoidalEarth::ComputeNEDTransform(lat, lon, alt, xformWCS_ToNED);
      double velNED[3];
      UtMat3d::Transform(velNED, xformWCS_ToNED, mLastVelWCS.GetData());
      UtSphericalEarth::ComputeNEDTransform(lat, lon, alt, xformWCS_ToNED);
      UtMat3d::InverseTransform(aVelXYZ, xformWCS_ToNED, velNED);
   }
   return valid;
}

// =================================================================================================
bool WsfSpaceMoverBase::SpaceModel::Update(double aSimTime) const
{
   bool valid(false);
   if ((aSimTime >= 0.0) && (aSimTime < 1.0E+30))
   {
      if (aSimTime != mLastUpdateTime)
      {
         mUpdateTime.AdvanceTimeBy(aSimTime - mLastUpdateTime);
         mPropagatorPtr->Update(mUpdateTime);
         ut::OrbitalState::Vector vectorWCS = mPropagatorPtr->GetOrbitalState().GetOrbitalStateVectorWCS();
         mLastLocWCS                        = vectorWCS.GetLocation();
         mLastVelWCS                        = vectorWCS.GetVelocity();
         UtEllipsoidalEarth::ConvertEllipsoidalToSpherical(mLastLocWCS.GetData(), mLastLocXYZ.GetData());
         UtEllipsoidalEarth::ConvertEllipsoidalToSpherical(mLastLocWCS.GetData(), mLastLocXYZ.GetData());
         mLastUpdateTime = aSimTime;
         valid           = true;
      }
      else
      {
         valid = true;
      }
   }
   return valid;
}

void WsfSpaceMoverBase::SetTargetOrientationECI(double aPsi, double aTheta, double aPhi)
{
   double simTime = GetSimulation()->GetSimTime();
   Update(simTime);
   mAttitudePtr->SetTargetOrientationECI(aPsi, aTheta, aPhi);
}

void WsfSpaceMoverBase::UpdateColor(double aSimTime, const UtColor& aColor)
{
   SetOrbitColor(aColor);
   WsfObserver::OrbitColorChanged(GetSimulation())(aSimTime, this);
}

void WsfSpaceMoverBase::SetOrbitColor(const UtColor& aOrbitColor)
{
   mOrbitColor = aOrbitColor;
}

double WsfSpaceMoverBase::GetLookAngleTo(const UtVec3d& aLocationWCS) const
{
   UtVec3d observer         = GetOrbitalState().GetOrbitalStateVectorWCS().GetLocation();
   UtVec3d targetToObserver = observer - aLocationWCS;
   observer.Normalize();
   targetToObserver.Normalize();
   return std::acos(observer.DotProduct(targetToObserver));
}

double WsfSpaceMoverBase::GetIncidenceAngle(const UtVec3d& aLocationWCS) const
{
   UtVec3d observer         = GetOrbitalState().GetOrbitalStateVectorWCS().GetLocation();
   UtVec3d targetToObserver = observer - aLocationWCS;
   targetToObserver.Normalize();
   return std::acos(targetToObserver.DotProduct(aLocationWCS.GetNormal()));
}

double WsfSpaceMoverBase::GetTargetElevationAngle(const UtVec3d& aLocationWCS) const
{
   return UtMath::cPI_OVER_2 - GetIncidenceAngle(aLocationWCS);
}

double WsfSpaceMoverBase::GetSquintAngleTo(const UtVec3d& aLocationWCS) const
{
   UtVec3d observer = GetOrbitalState().GetOrbitalStateVectorWCS().GetLocation();
   UtVec3d upVec    = observer.GetNormal();

   UtVec3d observerToTarget = aLocationWCS - observer;
   double  ottUp            = observerToTarget.DotProduct(upVec);
   UtVec3d ottUpVec         = upVec;
   ottUpVec.Multiply(ottUp);
   UtVec3d ottProjected = observerToTarget - ottUpVec;
   ottProjected.Normalize();

   UtVec3d velocity = GetOrbitalState().GetOrbitalStateVectorWCS().GetVelocity();
   double  velUp    = velocity.DotProduct(upVec);
   UtVec3d velUpVec = upVec;
   velUpVec.Multiply(velUp);
   UtVec3d velProjected = velocity - velUpVec;
   velProjected.Normalize();

   return std::acos(ottProjected.DotProduct(velProjected));
}

double WsfSpaceMoverBase::GetDistanceToHorizon() const
{
   double retval{-1.0};
   if (GetCentralPoint().IsCentralBody())
   {
      double distFromCenter    = GetOrbitalState().GetOrbitalStateVectorInertial().GetLocation().Magnitude();
      double centralBodyRadius = GetCentralBody().GetMeanRadius();
      if (distFromCenter >= centralBodyRadius)
      {
         retval = std::sqrt(distFromCenter * distFromCenter - centralBodyRadius * centralBodyRadius);
      }
   }
   return retval;
}

double WsfSpaceMoverBase::GetLookAngleToHorizon() const
{
   double retval{-UtMath::cTWO_PI};
   if (GetCentralPoint().IsCentralBody())
   {
      double distFromCenter    = GetOrbitalState().GetOrbitalStateVectorInertial().GetLocation().Magnitude();
      double centralBodyRadius = GetCentralBody().GetMeanRadius();
      if (distFromCenter >= centralBodyRadius)
      {
         retval = std::asin(centralBodyRadius / distFromCenter);
      }
   }
   return retval;
}

bool WsfSpaceMoverBase::AddDeltaV(const UtVec3d& aDeltaV_ECI)
{
   const auto& orbitalState = GetPropagator().GetOrbitalState();
   auto        stateVector  = orbitalState.GetOrbitalStateVectorInertial();
   auto        velocity     = stateVector.GetVelocity();
   velocity.Add(velocity, aDeltaV_ECI);
   bool success =
      mPropagatorPtr->Initialize(ut::OrbitalState(GetPropagator().GetCurrentTime(),
                                                  orbitalState.GetCentralBody(),
                                                  orbitalState.GetCoordinateSystem(),
                                                  ut::OrbitalState::ReferenceFrame::cECI,
                                                  ut::OrbitalState::Vector(stateVector.GetLocation(), velocity)));
   UpdatePropagation(GetPropagator().GetCurrentTime());
   return success;
}
