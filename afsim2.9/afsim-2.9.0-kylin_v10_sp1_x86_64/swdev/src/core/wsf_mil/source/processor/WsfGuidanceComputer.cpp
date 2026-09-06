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

// ================================================================================================
//                                       PROGRAMMING NOTES
//
// Prior to 2012, the guidance algorithms were included in this file. At the start of 2012 a
// concept called 'guidance programs' was introduced and the guidance algorithms were moved to
// WsfGuidanceProgram. This class acts as the executive provides following functions:
//
// *) Maintaining the current phase and providing the functions to evaluate switching criteria.
// *) Computing the current state of the weapon and target for passing to the guidance programs.
// *) Invoking the requested programs to get the desired guidance commands to be passed to the mover.
//
// The prime motivation for the refactoring was:
//
// *) The need more for more guidance algorithms and the ability to mix and match.
// *) The ability to add guidance algorithms as separate projects.
// *) Reduce the size of this file. It was going to get really large.
//
// The guidance functions that were formerly present here are now present in the 'LegacyProgram' in
// WsfGuidanceProgram. It will be invoked if no other program is requested.
// ================================================================================================

#include "WsfGuidanceComputer.hpp"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>

#include "UtEarth.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLatPos.hpp"
#include "UtLonPos.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfGuidanceProgram.hpp"
#include "WsfGuidedMover.hpp"
#include "WsfGuidedMoverBase.hpp"
#include "WsfIntercept.hpp"
#include "WsfLaunchComputer.hpp"
#include "WsfLaunchHandoffData.hpp"
#include "WsfMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackObserver.hpp"
#include "WsfWeaponObserver.hpp"
#include "script/WsfScriptContext.hpp"

const double WsfGuidanceComputer::cUNDEFINED_DOUBLE            = 1.0E+30;
const double WsfGuidanceComputer::cFROM_LAUNCH_COMPUTER        = 1.0E+31;
const double WsfGuidanceComputer::cFLIGHT_PATH_ANGLE_TOLERANCE = 0.0001 * UtMath::cRAD_PER_DEG;
const double WsfGuidanceComputer::cLARGE_POSITIVE_DOUBLE       = 1.0E+30;

// ================================================================================================
WsfGuidanceComputer::WsfGuidanceComputer(WsfScenario& aScenario)
   : WsfScriptProcessor(aScenario, "WsfGuidanceComputer", "PROCESSOR")
   , WsfMoverGuidance()
   , mPhaseList()
   , mPhaseTemplate()
   , mExplicitPhaseUsed(false)
   , mImplicitPhaseUsed(false)
   , mInitialPhaseName(nullptr)
   , mPhaseIndex(0)
   , mLegacyProgramPtr(nullptr)
   , mDefinedPrograms()
   , mActivePrograms()
   , mProgramCompleteIndex(0)
   , mTimeConstant(0.0)
   , mGuideToTruth(false)
   , mShowStatus(false)
   , mShowDiagnostics(false)
   , mShowCommands(false)
   , mShowEvaluations(false)
   , mShowGraphics(false)
   , mMoverPtr(nullptr)
   , mAtmosphere(aScenario.GetAtmosphere())
   , mCallbacks()
   , mTerrainPtr(nullptr)
   , mLaunchTime(0.0)
   , mLastUpdateTime(0.0)
   , mPhaseStartTime(0.0)
   , mGuidanceStartTime(0.0)
   , mCommandedFlightPathAngle(cUNDEFINED_DOUBLE)
   , mState()
   , mLastFlightPathAngle(0.0)
   , mLastTgtUpdateTime(cLARGE_POSITIVE_DOUBLE)
   , mTgtStateDefined(false)
   , mLaunchComputerFlightPathAngle(cUNDEFINED_DOUBLE)
   , mRouteIndex(0)
   , mFollowingRoute(false)
   , mWasClosing(false)
   , mAimpointExpireTime(-1.0)
   , mAimpointDrawId(0)
   , mDrawPtr(nullptr)
   , mOnExitScriptActive(false)
{
   UtVec3d::Set(mLaunchLocWCS, 0.0);
   UtVec3d::Set(mLastTgtLocWCS, 0.0);
   UtVec3d::Set(mLastTgtVelWCS, 0.0);
   UtVec3d::Set(mInterceptLocWCS, 0.0);
   UtVec3d::Set(mAimpointLocWCS, 0.0);

   // Provide a default (single) phase in which to place input values.
   mPhaseList.emplace_back();
   mPhaseList.front().mPhaseName = "default";
}

// ================================================================================================
// virtual
WsfGuidanceComputer::~WsfGuidanceComputer()
{
   delete mLegacyProgramPtr;
   for (auto& definedProgram : mDefinedPrograms)
   {
      delete definedProgram;
   }
   delete mTerrainPtr;
   delete mDrawPtr;
}

// ================================================================================================
// protected
WsfGuidanceComputer::WsfGuidanceComputer(const WsfGuidanceComputer& aSrc)
   : WsfScriptProcessor(aSrc)
   , WsfMoverGuidance(aSrc)
   , mPhaseList(aSrc.mPhaseList)
   , mPhaseTemplate(aSrc.mPhaseTemplate)
   , mExplicitPhaseUsed(aSrc.mExplicitPhaseUsed)
   , mImplicitPhaseUsed(aSrc.mImplicitPhaseUsed)
   , mInitialPhaseName(aSrc.mInitialPhaseName)
   , mPhaseIndex(0)
   , mLegacyProgramPtr(nullptr)
   , mDefinedPrograms()
   , mActivePrograms()
   , mProgramCompleteIndex(0)
   , mTimeConstant(aSrc.mTimeConstant)
   , mGuideToTruth(aSrc.mGuideToTruth)
   , mShowStatus(aSrc.mShowStatus)
   , mShowDiagnostics(aSrc.mShowDiagnostics)
   , mShowCommands(aSrc.mShowCommands)
   , mShowEvaluations(aSrc.mShowEvaluations)
   , mShowGraphics(aSrc.mShowGraphics)
   , mMoverPtr(nullptr)
   , mAtmosphere(aSrc.mAtmosphere)
   , mCallbacks()
   , mTerrainPtr(nullptr)
   , mLaunchTime(0.0)
   , mLastUpdateTime(0.0)
   , mPhaseStartTime(0.0)
   , mGuidanceStartTime(0.0)
   , mCommandedFlightPathAngle(aSrc.mCommandedFlightPathAngle)
   , mState()
   , mLastFlightPathAngle(aSrc.mLastFlightPathAngle)
   , mLastTgtUpdateTime(aSrc.mLastTgtUpdateTime)
   , mTgtStateDefined(false)
   , mLaunchComputerFlightPathAngle(aSrc.mLaunchComputerFlightPathAngle)
   , mRouteIndex(0)
   , mFollowingRoute(false)
   , mWasClosing(false)
   , mAimpointExpireTime(aSrc.mAimpointExpireTime)
   , mAimpointDrawId(aSrc.mAimpointDrawId)
   , mDrawPtr(nullptr)
   , mOnExitScriptActive(aSrc.mOnExitScriptActive)
{
   for (auto definedProgram : aSrc.mDefinedPrograms)
   {
      mDefinedPrograms.push_back(definedProgram->Clone());
   }
   UtVec3d::Set(mLaunchLocWCS, aSrc.mLaunchLocWCS);
   UtVec3d::Set(mLastTgtLocWCS, aSrc.mLastTgtLocWCS);
   UtVec3d::Set(mLastTgtVelWCS, aSrc.mLastTgtVelWCS);
   UtVec3d::Set(mInterceptLocWCS, aSrc.mInterceptLocWCS);
   UtVec3d::Set(mAimpointLocWCS, aSrc.mAimpointLocWCS);
}

// ================================================================================================
// virtual
WsfProcessor* WsfGuidanceComputer::Clone() const
{
   return new WsfGuidanceComputer(*this);
}

// ================================================================================================
// virtual
bool WsfGuidanceComputer::Initialize(double aSimTime)
{
   bool ok = true;
   // Can't do anything unless at least one phase has been defined.
   if (mPhaseList.empty())
   {
      ut::log::error() << "No phases have been defined.";
      return false;
   }

   // The base class initialized is called after the basic data structures have been validated.
   // This ensures the script methods called from on_initialize that modify guidance parameters
   // have valid data on which to operate.

   // Set the initial phase name/index (but don't select the phase). Potentially needed by on_initialize scripts...
   if (mInitialPhaseName.IsNull())
   {
      mInitialPhaseName = mPhaseList[0].mPhaseName;
   }
   mPhaseIndex             = 0;
   unsigned int phaseIndex = FindPhase(mInitialPhaseName);
   if (phaseIndex < mPhaseList.size())
   {
      mPhaseIndex = phaseIndex;
   }
   else
   {
      auto out = ut::log::error() << "Invalid initial_phase name.";
      out.AddNote() << "Name: " << mInitialPhaseName;
      ok = false;
   }

   mMoverPtr = dynamic_cast<WsfGuidedMoverBase*>(GetPlatform()->GetMover());
   if (mMoverPtr == nullptr)
   {
      ut::log::warning() << "Mover is of the incorrect type. Turning guidance off.";
      SetInitiallyTurnedOn(false);
   }
   else
   {
      mMoverPtr->SetGuidance(this);
      mCallbacks.Add(mMoverPtr->StageIgnition.Connect(&WsfGuidanceComputer::StageIgnition, this));
      mCallbacks.Add(mMoverPtr->RouteChanged.Connect(&WsfGuidanceComputer::RouteChanged, this));
      mCallbacks.Add(mMoverPtr->StageBurnout.Connect(&WsfGuidanceComputer::StageBurnout, this));
      mCallbacks.Add(mMoverPtr->StageSeparation.Connect(&WsfGuidanceComputer::StageSeparation, this));
      mCallbacks.Add(
         WsfObserver::SensorTrackInitiated(GetSimulation()).Connect(&WsfGuidanceComputer::SensorTrackInitiated, this));
   }

   // For each phase:
   // - Initialize the phase
   // - Validate the existence of every phase specified in a next_phase command,
   // - Validate the program names in the use_program commands.
   size_t phaseListSize = mPhaseList.size();
   for (size_t pi = 0; pi != phaseListSize; ++pi)
   {
      Phase& phase = mPhaseList[pi];
      if (!phase.Initialize(aSimTime, *mContextPtr))
      {
         auto out = ut::log::error() << "Initialization of phase failed.";
         out.AddNote() << "Phase: " << phase.mPhaseName;
         ok = false;
      }
      ok &= ValidateNextPhaseName(phase, phase.mEndOfRoutePhaseName);
      ok &= ValidateNextPhaseName(phase, phase.mBoostCompletePhaseName);
      ok &= ValidateNextPhaseName(phase, phase.mStageIgnitionPhaseName);
      ok &= ValidateNextPhaseName(phase, phase.mStageBurnoutPhaseName);
      ok &= ValidateNextPhaseName(phase, phase.mStageSeparationPhaseName);
      ok &= ValidateNextPhaseName(phase, phase.mSensorTrackInitiatedPhaseName);
      for (size_t i = 0; i < phase.mProgramCompletePhaseName.size(); ++i)
      {
         ok &= ValidateNextPhaseName(phase, phase.mProgramCompletePhaseName[i].second);
      }
      for (size_t pci = 0; pci != phase.mPhaseChangeList.size(); ++pci)
      {
         ok &= ValidateNextPhaseName(phase, phase.mPhaseChangeList[pci].mNextPhaseName);
      }

      for (auto spi = phase.mPrograms.begin(); spi != phase.mPrograms.end(); ++spi)
      {
         if (FindProgram(*spi) == nullptr)
         {
            auto out = ut::log::error() << "Unable to find program referenced by phase.";
            out.AddNote() << "Program: " << *spi;
            out.AddNote() << "Phase: " << phase.mPhaseName;
            ok = false;
         }
      }
   }

   // Now initialize the base class....
   ok &= WsfScriptProcessor::Initialize(aSimTime);
   if (ok)
   {
      if (mLegacyProgramPtr == nullptr)
      {
         mLegacyProgramPtr = WsfGuidanceProgramTypes::Get(GetScenario()).CreateInstance("_LEGACY_PROGRAM_");
      }
      if (mLegacyProgramPtr != nullptr)
      {
         ok &= mLegacyProgramPtr->Initialize(aSimTime, this);
      }

      for (auto& definedProgram : mDefinedPrograms)
      {
         WsfGuidanceProgram& program = *definedProgram;
         if (!program.Initialize(aSimTime, this))
         {
            auto out = ut::log::error() << "Failed to initialize program.";
            out.AddNote() << "Program: " << program.GetName();
            ok = false;
         }
      }

      // If debug is enabled then show_status and show_evaluations are also enabled.
      mShowStatus |= DebugEnabled();
      mShowCommands |= DebugEnabled();
      mShowEvaluations |= DebugEnabled();

      mTerrainPtr = new wsf::Terrain(GetSimulation()->GetTerrainInterface());

      GetPlatform()->GetLocationWCS(mLaunchLocWCS);
      mLaunchTime     = aSimTime;
      mLastUpdateTime = -1.0;

      // Copy the ECI reference angle from the platform so ECI coordinates work if the mover is using them.
      // (this must be done so UpdateWeaponPerception works properly)
      mState.mWpnState.SetECI_ReferenceEpoch(GetPlatform()->GetECI_ReferenceEpoch());

      if (mShowGraphics)
      {
         mDrawPtr        = new WsfDraw(*GetSimulation());
         mAimpointDrawId = mDrawPtr->GetNewId();
      }
   }
   return ok;
}

// ================================================================================================
// virtual
bool WsfGuidanceComputer::Initialize2(double aSimTime)
{
   // Get any launch computer data that may be used from the hand-off track.
   const WsfTrack* trackPtr = GetPlatform()->GetTrackManager().GetCurrentTarget();
   if (trackPtr != nullptr)
   {
      WsfGeoPoint interceptPoint;
      if (WsfLaunchComputer::GetInterceptPoint(*trackPtr, interceptPoint))
      {
         interceptPoint.GetLocationWCS(mInterceptLocWCS);
      }
      WsfLaunchComputer::GetLoftAngle(*trackPtr, mLaunchComputerFlightPathAngle);
   }
   else
   {
      // Try to get necessary data from launch hand-off.
      WsfLaunchHandoffData* handoffPtr = WsfLaunchHandoffData::Find(*GetPlatform());
      if (handoffPtr != nullptr)
      {
         if (handoffPtr->LoftAngleIsValid())
         {
            mLaunchComputerFlightPathAngle = handoffPtr->GetLoftAngle();
         }
         if (handoffPtr->InterceptPointIsValid())
         {
            WsfGeoPoint interceptPoint(handoffPtr->GetInterceptPoint());
            interceptPoint.GetLocationWCS(mInterceptLocWCS);
         }
      }
   }

   bool ok = WsfScriptProcessor::Initialize2(aSimTime);
   return ok;
}

// ================================================================================================
const char* WsfGuidanceComputer::GetScriptClassName() const
{
   return "WsfGuidanceComputer";
}

// ================================================================================================
//! Perform a guidance update.
//! This is called from the mover when a guidance update is desired. It computes the desired values
//! and pushes them back in to the mover.
//! @param aSimTime The current simulation time.
//! @param aCurTime The current internal time within the mover. This can be less than aSimTime.
//! @param aEndTime The internal time to which the mover is trying to advance. This is typically
//!                 one mover integration step beyond aCurTime
void WsfGuidanceComputer::UpdateGuidance(double aSimTime, double aCurTime,
                                         double aEndTime) // override
{
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Guidance Update:" << std::fixed << std::setprecision(5);
      out.AddNote() << "T = " << aSimTime << " (" << aCurTime - mLaunchTime << ") ";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "End time: " << aEndTime << " (" << aEndTime - mLaunchTime << ")";
   }

   // Some movers will call us even when we're not turned on!
   if (!IsTurnedOn())
   {
      mLastUpdateTime = aCurTime;
      mMoverPtr->SetYandZForces(0.0, 0.0);
      mMoverPtr->SetCommandedSpeed(-1.0);
      mMoverPtr->SetCommandedThrottle(-1.0);
      return;
   }

   // If the time since the last update is too small, ignore the update until enough time has accumulated.
   double deltaTime = aCurTime - mLastUpdateTime;
   if (deltaTime < 1.0E-6)
   {
      return;
   }

   // Update the perceived kinematic state of myself.
   // This must be done before the route following check below as it uses the determined position.
   UpdateWeaponPerception(aCurTime);

   // Iterate until no phase change occurs.
   //
   // Normally only one pass will be made through the loop, but if an "on_update" or a "next_phase"
   // causes a phase change then we need to reevaluate as it may cause a change in the perception
   // of the target.

   unsigned int phaseIndex = static_cast<unsigned int>(mPhaseList.size());
   while (phaseIndex != mPhaseIndex)
   {
      phaseIndex = mPhaseIndex;

      // Execute the 'on_update' script if present.
      mPhaseList[mPhaseIndex].ExecuteOnUpdate(aSimTime, *mContextPtr);
      if (phaseIndex != mPhaseIndex)
      {
         continue; // Phase change due to SelectPhase(name) call within 'on_update'
      }

      // Update the perceived kinematic state of the target.
      UpdateTargetPerception(aCurTime);

      // If following a route, determine if a waypoint change is needed.
      // A phase change will occur if the end-of-route is hit and the phase had a 'next_phase if end_of_route'.
      if (mFollowingRoute)
      {
         CheckForWaypointChange(aSimTime, aCurTime);
         if (phaseIndex != mPhaseIndex)
         {
            continue; // Phase change due to 'next_phase if end_of_route'
         }
      }

      // Evaluate 'next_phase' commands to see if a phase change should occur.
      CheckForPhaseChange(aSimTime, aCurTime);
   }

   // At this point we are now stable in the phase for the current time.

   // Compute forces if guidance is enabled.
   double yAccel = 0.0;
   double zAccel = 0.0;
   if (aCurTime >= mGuidanceStartTime)
   {
      // Update the target vector to which we will guide (which may point towards the real target or the next
      // waypoint). If the vector cannot be determined (no target or waypoint) then the guidance functions
      // involving a target point will not be performed.

      Phase& phase = mPhaseList[mPhaseIndex];

      mState.mSimTime = aSimTime;
      mState.mCurTime = aCurTime;
      mState.mEndTime = aEndTime;
      UpdateAimpoint(aCurTime, phase);

      // Initialize resulting commands.
      Commands cmds;
      UtVec3d::Set(cmds.mAccelCmd, 0.0);
      UtVec3d::Set(cmds.mAngleRateCmd, 0.0);

      // If explicitly specified guidance programs were specified for the current phase, execute only those programs.
      // If that wasn't the case then execute the legacy program so as to remain compatible with guidance computer
      // definitions that were created prior to the existence of phases.
      if (!mActivePrograms.empty())
      {
         auto pli = mActivePrograms.begin();
         while (pli != mActivePrograms.end())
         {
            if ((*pli)->Execute(mState, cmds) == WsfGuidanceProgram::cCOMPLETE)
            {
               // Determine if a 'next_phase' command was specified that tested for completion of this program.
               //
               // Programs that are defined directly within the phase (i.e.: program/end_program instead of use_program)
               // have a generated name. In that case we simply use its type name. Phase::ProcessInput will generate a
               // name '#<type-name> <phase-name> <number>'

               WsfGuidanceProgram* programPtr = *pli;
               WsfStringId         name(programPtr->GetName());
               std::string         nameStr(name);
               std::string         prefixStr = '#' + programPtr->GetType() + ' ' + phase.mPhaseName +
                                       ' '; // See Phase::ProcessInput where it generates the name
               if (nameStr.compare(0, prefixStr.size(), prefixStr) == 0)
               {
                  name = programPtr->GetType(); // directly defined, so use the type - not the generated name
               }

               // If multiple programs complete in the same time step, we want to chose the applicable 'next_phase'
               // command that is defined first by the user (they are concept the one referenced in the first next_phase
               // in order of definition are used.
               //
               // When the first program that has a next_phase completes, the index is of the next_phase is saved. If
               // there are subsequent completions we only have to search the list up to this index (we take the one
               // with the smallest index).
               for (size_t i = 0; i < mProgramCompleteIndex; ++i)
               {
                  if (name == phase.mProgramCompletePhaseName[i].first)
                  {
                     mProgramCompleteIndex = i;
                     break;
                  }
               }

               pli = mActivePrograms.erase(pli); // And now remove it from the active program list...
            }
            else
            {
               ++pli;
            }
         }
      }
      // NOTE: The first check is made because all of the active programs may have completed.  If the phase requested a program
      //       then we still don't execute the legacy program. The legacy program is executed ONLY if the phase requested a program.
      else if (phase.mPrograms.empty() && (mLegacyProgramPtr != nullptr))
      {
         mLegacyProgramPtr->Execute(mState, cmds);
      }

      // Propagate the output from the guidance programs to the mover.

      // Update the programmed attitude control.
      mMoverPtr->SetCommandedYawRate(cmds.mAngleRateCmd[2]);
      mMoverPtr->SetCommandedPitchRate(cmds.mAngleRateCmd[1]);
      mMoverPtr->SetCommandedRollRate(cmds.mAngleRateCmd[0]);

      // Apply acceleration limits, convert accelerations to forces and push to the mover.
      yAccel = cmds.mAccelCmd[1];
      zAccel = cmds.mAccelCmd[2];

      yAccel = UtMath::Limit(yAccel, static_cast<double>(phase.mMaxGeeCmd));
      zAccel = UtMath::Limit(zAccel, static_cast<double>(phase.mMaxGeeCmd));

      double mass = GetPlatform()->GetMass();
      mMoverPtr->SetYandZForces(mass * yAccel, mass * zAccel);

      // Update command speed/Mach if requested.
      if (phase.mCommandedSpeed != cUNDEFINED_DOUBLE)
      {
         if (phase.mCommandedSpeed >= 0.0)
         {
            mMoverPtr->SetCommandedSpeed(phase.mCommandedSpeed);
         }
         else
         {
            mMoverPtr->SetCommandedSpeed(-phase.mCommandedSpeed * mAtmosphere.SonicVelocity(GetPlatform()->GetAltitude()));
         }
      }
      else
      {
         mMoverPtr->SetCommandedSpeed(-1.0);
      }

      mMoverPtr->SetCommandedThrottle(phase.mCommandedThrottle);
   }
   else
   {
      // Guidance is disabled... no commands...
      mMoverPtr->SetYandZForces(0.0, 0.0);
      mMoverPtr->SetCommandedSpeed(-1.0);
      mMoverPtr->SetCommandedThrottle(-1.0);
   }

   mLastUpdateTime = aCurTime;

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Update Guidance:" << std::fixed << std::setprecision(5);
      out.AddNote() << "T = " << aSimTime << " (" << aCurTime - mLaunchTime << ") ";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Acceleration(y): " << yAccel / UtEarth::cACCEL_OF_GRAVITY << " g";
      out.AddNote() << "Acceleration(z): " << zAccel / UtEarth::cACCEL_OF_GRAVITY << " g";
      out.AddNote() << "Acceleration: " << sqrt(yAccel * yAccel + zAccel * zAccel) / UtEarth::cACCEL_OF_GRAVITY << " g";
      out.AddNote() << std::setprecision(6) << "End Time: " << aEndTime << " (" << aEndTime - mLaunchTime << ")";
   }
}

// ================================================================================================
// virtual
bool WsfGuidanceComputer::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "guide_to_truth")
   {
      // Must be before implicit phase commands because phase also accepts this
      aInput.ReadValue(mGuideToTruth);
   }
   else if (command == "time_constant") // NO_DOC | IN_DEVELOPMENT
   {
      // Must be before implicit phase commands because phase also accepts this
      aInput.ReadValueOfType(mTimeConstant, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mTimeConstant, 0.0);
   }
   else if (command == "program")
   {
      // Must be before implicit phase commands because phase also accepts this
      std::string nameStr;
      std::string typeStr;
      aInput.ReadValue(nameStr);
      aInput.ReadValue(typeStr);
      UtInputBlock inputBlock(aInput, "end_program");

      // Check for duplicate program name.
      WsfStringId nameId(nameStr);
      for (auto& definedProgram : mDefinedPrograms)
      {
         if (definedProgram->GetNameId() == nameId)
         {
            throw UtInput::BadValue(aInput, "Duplicate guidance program defined: " + nameStr);
         }
      }

      // Allocate and populate a program object of the requested type

      std::unique_ptr<WsfGuidanceProgram> programPtr(WsfGuidanceProgramTypes::Get(GetScenario()).CreateInstance(typeStr));
      if (programPtr == nullptr)
      {
         throw UtInput::BadValue(aInput, "Unknown guidance program type: " + typeStr);
      }
      programPtr->SetName(nameStr);
      while (inputBlock.ReadCommand())
      {
         if (!programPtr->ProcessInput(inputBlock.GetInput()))
         {
            throw UtInput::UnknownCommand(inputBlock.GetInput());
         }
      }
      mDefinedPrograms.push_back(programPtr.release());
   }
   else if ((!mExplicitPhaseUsed) && mPhaseList[0].ProcessInput(aInput, *this, *mContextPtr))
   {
      mImplicitPhaseUsed = true;
   }
   else if ((!mImplicitPhaseUsed) && (command == "phase"))
   {
      if (!mExplicitPhaseUsed)
      {
         // This is the first explicitly named phase specified. Clear out the implicit place holder that
         // was created in the constructor.
         mPhaseList.clear();
      }
      UtInputBlock inputBlock(aInput);

      std::string phaseName;
      aInput.ReadValue(phaseName);
      WsfStringId phaseNameId = phaseName;
      if (FindPhase(phaseNameId) < mPhaseList.size())
      {
         throw UtInput::BadValue(aInput, "Duplicate phase name: " + phaseName);
      }

      mPhaseList.push_back(mPhaseTemplate);
      Phase& phase     = mPhaseList.back();
      phase.mPhaseName = phaseNameId;
      while (inputBlock.ReadCommand())
      {
         if (inputBlock.GetCommand() == "copy")
         {
            std::string srcPhaseName;
            aInput.ReadValue(srcPhaseName);
            unsigned int srcPhaseIndex = FindPhase(srcPhaseName);
            if (srcPhaseIndex >= mPhaseList.size())
            {
               throw UtInput::BadValue(aInput, "Unknown source phase name: " + srcPhaseName);
            }
            else
            {
               phase            = mPhaseList[srcPhaseIndex];
               phase.mPhaseName = phaseName;
            }
         }
         else if (!phase.ProcessInput(aInput, *this, *mContextPtr))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      mExplicitPhaseUsed = true;
   }
   else if (command == "phase_template")
   {
      if (mImplicitPhaseUsed)
      {
         throw UtInput::BadValue(aInput, "'phase_template' cannot be used with an implicitly defined 'phase'");
      }
      else if (mExplicitPhaseUsed && (!mPhaseList.empty()))
      {
         throw UtInput::BadValue(aInput, "'phase_template' cannot be used after a 'phase' has been defined.");
      }
      mExplicitPhaseUsed = true;
      mPhaseList.clear();
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand())
      {
         if (!mPhaseTemplate.ProcessInput(aInput, *this, *mContextPtr))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (command == "edit")
   {
      // TODO - allow editing of programs???
      std::string phaseStr;
      aInput.ReadValue(phaseStr);
      aInput.StringEqual(phaseStr, "phase");

      std::string phaseName;
      aInput.ReadValue(phaseName);
      WsfStringId  phaseNameId = phaseName;
      unsigned int phaseIndex  = FindPhase(phaseNameId);
      if (phaseIndex >= mPhaseList.size())
      {
         throw UtInput::BadValue(aInput, "Non-existent phase name: " + phaseName);
      }
      Phase& phase = mPhaseList[phaseIndex];

      UtInputBlock inputBlock(aInput, "end_phase");
      while (inputBlock.ReadCommand())
      {
         if (!phase.ProcessInput(aInput, *this, *mContextPtr))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (command == "show_status")
   {
      mShowStatus = true;
   }
   else if (command == "show_diagnostics")
   {
      mShowDiagnostics = true;
   }
   else if (command == "show_commands")
   {
      mShowCommands = true;
   }
   else if (command == "show_evaluations")
   {
      mShowEvaluations = true;
   }
   else if (command == "show_graphics")
   {
      mShowGraphics = true;
   }
   else if (mAtmosphere.ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = WsfScriptProcessor::ProcessInput(aInput);
   }
   return myCommand;
}

// ================================================================================================
// virtual
void WsfGuidanceComputer::TurnOn(double aSimTime)
{
   WsfScriptProcessor::TurnOn(aSimTime); // Let the base class do its thing

   mMoverPtr->SetGuidance(this); // Indicate I am performing the guidance
   mLastUpdateTime = aSimTime - 1.0;
   UpdateWeaponPerception(aSimTime);
   UpdateTargetPerception(aSimTime);

   // Select the initial guidance phase. This will default to the first phase if none was specified.
   mPhaseIndex = static_cast<unsigned int>(mPhaseList.size()); // Indicate no phase has been selected.
   SelectPhase(aSimTime, aSimTime, mInitialPhaseName);

   // This should never happen as the initial_phase should have been checked in Initialize and
   // the scriptable SelectPhase method, but we do this just in case.
   if (mPhaseIndex >= static_cast<unsigned int>(mPhaseList.size()))
   {
      mPhaseIndex = 0;
   }
}

// ================================================================================================
// virtual
void WsfGuidanceComputer::TurnOff(double aSimTime)
{
   WsfScriptProcessor::TurnOff(aSimTime); // Let the base class do its thing

   // The WsfSimulation::ProcessPlatformDeathEvent() can delete the Mover pointer in WsfPlatform,
   // then sets it to zero.  So before we use our derived pointer type here, we must access the
   // mover pointer in the Platform.  If it returns zero, do not do what we were going to do.
   if (GetPlatform()->GetMover() == nullptr)
   {
      return;
   }

   // Detach from the mover if I am providing the guidance.
   WsfMoverGuidance* guidancePtr = mMoverPtr->GetGuidance();
   if (this != guidancePtr)
   {
      mMoverPtr->SetGuidance(nullptr);
   }
}

// ================================================================================================
//! A callback from the mover that indicates that the route has changed.
//! @param aSimTime   The current simulation time.
void WsfGuidanceComputer::RouteChanged(double aSimTime)
{
   if (mShowStatus)
   {
      auto out = ut::log::info() << "Detected route change.";
      PrintStatusHeader(aSimTime, aSimTime, out);
   }
   if (mFollowingRoute)
   {
      mWasClosing              = false;
      const WsfRoute* routePtr = mMoverPtr->GetRoute();
      if ((routePtr != nullptr) && (!routePtr->Empty()))
      {
         SelectWaypoint(aSimTime, aSimTime, 0);
      }
      else
      {
         mFollowingRoute = false;
      }
   }
}

// ================================================================================================
//! Called from the script interface select a new phase.
//! @param aPhaseName The name (ID) of the phase to be selected
bool WsfGuidanceComputer::SelectPhase(WsfStringId aPhaseName)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "select_phase", out);
      }
      if (IsTurnedOn())
      {
         double simTime = GetSimTime();
         SelectPhase(simTime, simTime, aPhaseName);
      }
      else
      {
         // If not turned on, use the specified phase as the initial phase
         mPhaseIndex       = phaseIndex;
         mInitialPhaseName = aPhaseName;
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to clear 'commanded_altitude'.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @return true if successful, false if not.
bool WsfGuidanceComputer::ClearCommandedAltitude(WsfStringId aPhaseName)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      mPhaseList[phaseIndex].mCommandedAlt = cUNDEFINED_DOUBLE;
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "clear commanded_altitude", out);
      }
      if (phaseIndex == mPhaseIndex) // Specified phase is the current phase
      {
         UpdateTerrainFollowing(mPhaseList[phaseIndex]);
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to clear 'commanded_flight_path_angle'.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @return true if successful, false if not.
bool WsfGuidanceComputer::ClearCommandedFlightPathAngle(WsfStringId aPhaseName)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      mPhaseList[phaseIndex].mCommandedFlightPathAngle = cUNDEFINED_DOUBLE;
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "clear commanded_flight_path_angle", out);
      }
      if (phaseIndex == mPhaseIndex) // Specified phase is the current phase
      {
         mCommandedFlightPathAngle = cUNDEFINED_DOUBLE;
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to clear 'commanded_speed'.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @return true if successful, false if not.
bool WsfGuidanceComputer::ClearCommandedSpeed(WsfStringId aPhaseName)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      mPhaseList[phaseIndex].mCommandedSpeed = cUNDEFINED_DOUBLE;
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "clear commanded_speed", out);
      }
      if (phaseIndex == mPhaseIndex) // Specified phase is the current phase
      {
         mMoverPtr->SetCommandedSpeed(-1.0);
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to clear 'commanded_throttle'.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @return true if successful, false if not.
bool WsfGuidanceComputer::ClearCommandedThrottle(WsfStringId aPhaseName)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      mPhaseList[phaseIndex].mCommandedThrottle = -1.0;
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "clear commanded_throttle", out);
      }
      if (phaseIndex == mPhaseIndex) // Specified phase is the current phase
      {
         mMoverPtr->SetCommandedThrottle(-1.0);
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to set the value of 'allow_route_following'.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @param aValue     The new value of 'allow_route_following'
//! @return true if successful, false if not.
bool WsfGuidanceComputer::SetAllowRouteFollowing(WsfStringId aPhaseName, bool aValue)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      mPhaseList[phaseIndex].mAllowRouteFollowing = aValue;
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "set allow_route_following", out);
         out.AddNote() << "Value: " << std::boolalpha << mPhaseList[phaseIndex].mAllowRouteFollowing;
      }
      if (phaseIndex == mPhaseIndex) // Specified phase is the current phase
      {
         UpdateRouteFollowing(mPhaseList[phaseIndex]);
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to set the value of 'aimpoint_altitude_offset'.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @param aValue     The new value of 'aimpoint_altitude_offset' (meters)
//! @return true if successful, false if not.
bool WsfGuidanceComputer::SetAimpointAltitudeOffset(WsfStringId aPhaseName, double aValue)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      mPhaseList[phaseIndex].mAimpointAltitudeOffset = aValue;
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "set aimpoint_altitude_offset", out);
         out.AddNote() << "Value: " << mPhaseList[phaseIndex].mAimpointAltitudeOffset << " m";
      }
      if (phaseIndex == mPhaseIndex) // Specified phase is the current phase
      {
         mAimpointExpireTime = -1.0; // Force re-computation of aimpoint if active
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to set the value of 'aimpoint_azimuth_offset'
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @param aValue     The new value of 'aimpoint_azimuth_offset' (radians).
//! @param aDirection The direction of the offset ("left", "right", "either")
//! @return true if successful, false if not.
bool WsfGuidanceComputer::SetAimpointAzimuthOffset(WsfStringId aPhaseName, double aValue, const std::string& aDirection)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      if (aDirection == "left")
      {
         mPhaseList[phaseIndex].mOffsetDirection = cOD_LEFT;
      }
      else if (aDirection == "right")
      {
         mPhaseList[phaseIndex].mOffsetDirection = cOD_RIGHT;
      }
      else if (aDirection == "either")
      {
         mPhaseList[phaseIndex].mOffsetDirection = cOD_EITHER;
      }
      else
      {
         auto out = ut::log::error() << "Invalid value of 'aimpoint_azimuth_offset' direction.";
         out.AddNote() << "Direction: " << aDirection;
         return false;
      }
      mPhaseList[phaseIndex].mAimpointAzimuthOffset = aValue;
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "set aimpoint_azimuth_offset", out);
         out.AddNote() << "Value: " << mPhaseList[phaseIndex].mAimpointAzimuthOffset * UtMath::cDEG_PER_RAD << " deg "
                       << aDirection;
      }
      if (phaseIndex == mPhaseIndex) // Specified phase is the current phase
      {
         mAimpointExpireTime = -1.0; // Force re-computation of aimpoint if active
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to set the value of 'aimpoint_range_offset'.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @param aValue     The new value of 'aimpoint_range_offset' (meters)
//! @return true if successful, false if not.
bool WsfGuidanceComputer::SetAimpointRangeOffset(WsfStringId aPhaseName, double aValue)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      mPhaseList[phaseIndex].mAimpointRangeOffset = aValue;
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "set aimpoint_range_offset", out);
         out.AddNote() << "Value: " << mPhaseList[phaseIndex].mAimpointRangeOffset << " m";
      }
      if (phaseIndex == mPhaseIndex) // Specified phase is the current phase
      {
         mAimpointExpireTime = -1.0; // Force re-computation of aimpoint if active
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to set the value of 'aimpoint_evaluation_interval'.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @param aValue     The new value of 'aimpoint_evaluation_interval' (meters)
//! @return true if successful, false if not.
bool WsfGuidanceComputer::SetAimpointEvaluationInterval(WsfStringId aPhaseName, double aValue)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      mPhaseList[phaseIndex].mAimpointEvaluationInterval = aValue;
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "set aimpoint_evaluation_interval", out);
         out.AddNote() << "Value: " << mPhaseList[phaseIndex].mAimpointEvaluationInterval << " sec";
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to set the value of 'commanded_altitude'.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @param aValue     The new value of 'commanded_altitude' (meters)
//! @param aAltIsAGL  true if the commanded_altitude in AGL.
//! @return true if successful, false if not.
bool WsfGuidanceComputer::SetCommandedAltitude(WsfStringId aPhaseName, double aValue, bool aAltIsAGL)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      mPhaseList[phaseIndex].mCommandedAlt      = aValue;
      mPhaseList[phaseIndex].mCommandedAltIsAGL = aAltIsAGL;
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "set commanded_altitude", out);
         out.AddNote() << "Value: " << mPhaseList[phaseIndex].mCommandedAlt << " m "
                       << (mPhaseList[phaseIndex].mCommandedAltIsAGL ? "agl" : "msl");
      }
      if (phaseIndex == mPhaseIndex) // Specified phase is the current phase
      {
         UpdateTerrainFollowing(mPhaseList[phaseIndex]);
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to set the value of 'commanded_azimuth_offset'.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @param aValue     The new value of 'commanded_azimuth_offset' (radians)
//! @return true if successful, false if not.
bool WsfGuidanceComputer::SetCommandedAzimuthOffset(WsfStringId aPhaseName, double aValue)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      mPhaseList[phaseIndex].mCommandedAzimuthOffset = aValue;
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "set commanded_azimuth_offset", out);
         out.AddNote() << "Value: " << mPhaseList[phaseIndex].mCommandedAzimuthOffset * UtMath::cDEG_PER_RAD << " deg";
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to set the value of 'commanded_flight_path_angle'.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @param aValue     The new value of 'commanded_flight_path_angle' (radians)
//! @return true if successful, false if not.
bool WsfGuidanceComputer::SetCommandedFlightPathAngle(WsfStringId aPhaseName, double aValue)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      mPhaseList[phaseIndex].mCommandedFlightPathAngle = aValue;
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "set commanded_flight_path_angle", out);
         out.AddNote() << "Value: " << aValue * UtMath::cDEG_PER_RAD << " deg";
      }
      if (phaseIndex == mPhaseIndex) // Specified phase is the current phase
      {
         mCommandedFlightPathAngle = aValue;
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to set the value of 'commanded_mach'.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @param aValue     The new value of 'commanded_mach' (unitless)
//! @return true if successful, false if not.
bool WsfGuidanceComputer::SetCommandedMach(WsfStringId aPhaseName, double aValue)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      mPhaseList[phaseIndex].mCommandedSpeed = -aValue;
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "set commanded_mach", out);
         out.AddNote() << "Value: " << -mPhaseList[phaseIndex].mCommandedSpeed;
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to set the value of 'commanded_speed'.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @param aValue     The new value of 'commanded_speed' (meters/second)
//! @return true if successful, false if not.
bool WsfGuidanceComputer::SetCommandedSpeed(WsfStringId aPhaseName, double aValue)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      mPhaseList[phaseIndex].mCommandedSpeed = aValue;
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "set commanded_speed", out);
         out.AddNote() << "Value: " << mPhaseList[phaseIndex].mCommandedSpeed;
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to set the value of 'commanded_throttle'.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @param aValue     The new value of 'commanded_throttle' in the range [0..1]
//! @return true if successful, false if not.
bool WsfGuidanceComputer::SetCommandedThrottle(WsfStringId aPhaseName, double aValue)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      mPhaseList[phaseIndex].mCommandedThrottle = aValue;
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "set commanded_throttle", out);
         out.AddNote() << "Value: " << mPhaseList[phaseIndex].mCommandedThrottle;
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to set g_bias.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @param aValue     The new g_bias (unitless)
//! @return true if successful, false if not.
bool WsfGuidanceComputer::SetGeeBias(WsfStringId aPhaseName, double aValue)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      mPhaseList[phaseIndex].mGeeBiasValue = aValue;
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "set g_bias", out);
         out.AddNote() << "Value: " << mPhaseList[phaseIndex].mGeeBiasValue;
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to set the value of 'guidance_delay'.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @param aValue     The new value of 'guidance_delay' (seconds)
//! @return true if successful, false if not.
bool WsfGuidanceComputer::SetGuidanceDelay(WsfStringId aPhaseName, double aValue)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      mPhaseList[phaseIndex].mGuidanceDelay = aValue;
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "set guidance_delay", out);
         out.AddNote() << "Value: " << mPhaseList[phaseIndex].mGuidanceDelay << " sec";
      }
      if (phaseIndex == mPhaseIndex) // Specified phase is the current phase
      {
         mGuidanceStartTime = mPhaseStartTime + mPhaseList[phaseIndex].mGuidanceDelay;
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to set the value of 'guidance_target'.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @param aValue     The new value of 'guidance_target' (seconds)
//! @return true if successful, false if not.
bool WsfGuidanceComputer::SetGuidanceTarget(WsfStringId aPhaseName, const std::string& aValue)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      if (aValue == "truth")
      {
         mPhaseList[phaseIndex].mGuidanceTarget = cGT_TRUTH;
      }
      else if (aValue == "perception")
      {
         mPhaseList[phaseIndex].mGuidanceTarget = cGT_PERCEPTION;
      }
      else if (aValue == "predicted_intercept")
      {
         mPhaseList[phaseIndex].mGuidanceTarget = cGT_PREDICTED_INTERCEPT;
      }
      else if (aValue == "default")
      {
         mPhaseList[phaseIndex].mGuidanceTarget = cGT_DEFAULT;
      }
      else
      {
         auto out = ut::log::error() << "Invalid value for 'guidance_target'.";
         out.AddNote() << "Value: " << aValue;
         return false;
      }
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "set guidance_target", out);
         out.AddNote() << "Value: " << aValue;
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script interface to command the mover to begin/change a lateral bias trajectory.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @param aValue The lateral bias requested (+right, -left).
//! @return true if successful, false if not.
bool WsfGuidanceComputer::SetLateralGeeBias(WsfStringId aPhaseName, double aValue)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      mPhaseList[phaseIndex].mLateralGeeBiasValue = aValue;
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "set lateral_gee_bias", out);
         out.AddNote() << "Value: " << mPhaseList[phaseIndex].mLateralGeeBiasValue;
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to set the value of 'maximum_ascent_rate'.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @param aValue     The new value of 'maximum_ascent_rate' (meters/second)
//! @return true if successful, false if not.
bool WsfGuidanceComputer::SetMaximumAscentRate(WsfStringId aPhaseName, double aValue)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      mPhaseList[phaseIndex].mMaxAscentRate = aValue;
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "set maximum_ascent_rate", out);
         out.AddNote() << "Value: " << mPhaseList[phaseIndex].mMaxAscentRate << " m/s";
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to set the value of 'maximum_commanded_g'.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @param aValue     The new value of 'maximum_commanded_g' (meters/seconds^2)
//! @return true if successful, false if not.
bool WsfGuidanceComputer::SetMaximumCommandedGees(WsfStringId aPhaseName, double aValue)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      mPhaseList[phaseIndex].mMaxGeeCmd = aValue;
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "set maximum_commanded_g", out);
         out.AddNote() << "Value: " << mPhaseList[phaseIndex].mMaxGeeCmd / UtEarth::cACCEL_OF_GRAVITY << " g";
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to set the 'maximum_descent_rate'.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @param aValue     The new value of 'maximum_descent_rate' (meters/second)
//! @return true if successful, false if not.
bool WsfGuidanceComputer::SetMaximumDescentRate(WsfStringId aPhaseName, double aValue)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      mPhaseList[phaseIndex].mMaxDescentRate = aValue;
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "set maximum_descent_rate", out);
         out.AddNote() << "Value: " << mPhaseList[phaseIndex].mMaxDescentRate << " m/s";
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to set the 'maximum_pitch_angle'.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @param aValue     The new value of 'maximum_pitch_angle' (radians)
//! @return true if successful, false if not.
bool WsfGuidanceComputer::SetMaximumPitchAngle(WsfStringId aPhaseName, double aValue)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      mPhaseList[phaseIndex].mMaxPitchAngle = aValue;
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "set maximum_pitch_angle", out);
         out.AddNote() << "Value: " << mPhaseList[phaseIndex].mMaxPitchAngle * UtMath::cDEG_PER_RAD << " deg";
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to set the value of 'pitch_change_gain'.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @param aValue     The new value for 'pitch_change_gain' (unitless)
//! @return true if successful, false if not.
bool WsfGuidanceComputer::SetPitchChangeGain(WsfStringId aPhaseName, double aValue)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      mPhaseList[phaseIndex].mPitchChangeGain = aValue;
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "set pitch_change_gain", out);
         out.AddNote() << "Value: " << mPhaseList[phaseIndex].mPitchChangeGain;
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to set the value of 'proportional_navigation_gain'.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @param aValue     The new value for 'proportional_navigation_gain' (unitless)
//! @return true if successful, false if not.
bool WsfGuidanceComputer::SetProportionalNavigationGain(WsfStringId aPhaseName, double aValue)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      mPhaseList[phaseIndex].mPN_GainValue = aValue;
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "set proportional_navigation_gain", out);
         out.AddNote() << "Value: " << mPhaseList[phaseIndex].mPN_GainValue;
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to set the value of 'proportional_navigation_limit_angle'.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @param aValue     The new value for 'proportional_navigation_limit_angle' (radians)
//! @return true if successful, false if not.
bool WsfGuidanceComputer::SetProportionalNavigationLimitAngle(WsfStringId aPhaseName, double aValue)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      mPhaseList[phaseIndex].mCosLOS_Offset = cos(aValue);
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "set proportional_navigation_limit_angle", out);
         out.AddNote() << "Value: " << aValue * UtMath::cDEG_PER_RAD << " deg";
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to set the value of 'proportional_navigation_method'.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @param aValue     The new value for 'proportional_navigation_method' (unitless)
//! @return true if successful, false if not.
bool WsfGuidanceComputer::SetProportionalNavigationMethod(WsfStringId aPhaseName, const std::string& aValue)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      if (aValue == "pure")
      {
         mPhaseList[phaseIndex].mPN_Method = Phase::cPN_PURE;
      }
      else if (aValue == "augmented")
      {
         mPhaseList[phaseIndex].mPN_Method = Phase::cPN_AUGMENTED;
      }
      else
      {
         auto out = ut::log::error() << "Unknown 'proportional_navigation_method'.";
         out.AddNote() << "Value: " << aValue;
         return false;
      }
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "set proportional_navigation_method", out);
         out.AddNote() << "Value: " << aValue;
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script to set the value for 'velocity_pursuit_gain'.
//! @param aPhaseName The phase name to be updated, or empty for the current phase.
//! @param aValue     The new value of 'velocity_pursuit_gain' (unitless)
//! @return true if successful, false if not.
bool WsfGuidanceComputer::SetVelocityPursuitGain(WsfStringId aPhaseName, double aValue)
{
   unsigned int phaseIndex;
   bool         ok = BeginExternalCommand(aPhaseName, phaseIndex);
   if (ok)
   {
      mPhaseList[phaseIndex].mVP_GainValue = aValue;
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Guidance Computer Command:";
         BeginExternalCommandEcho(phaseIndex, "set velocity_pursuit_gain", out);
         out.AddNote() << "Value: " << mPhaseList[phaseIndex].mVP_GainValue;
      }
   }
   return ok;
}

// ================================================================================================
//! Called from script interface to command the mover to eject the current stage.
//! @param aPreSeparationCoastTime Allows the caller to override the pre_separation_coast_time for the current stage.
//! If this value is less than zero then the value in the stage definition will be used.
//! @param aPreIgnitionCoastTime Allows the caller to override the pre_ignition_coast_time for the current stage.
//! If this value is less than zero then the value in the stage definition will be used.
//! @return true if successful, false if not.
bool WsfGuidanceComputer::EjectStage(double aPreSeparationCoastTime, double aPreIgnitionCoastTime)
{
   bool   ok(true);
   double simTime(GetSimTime());
   if (mShowCommands)
   {
      auto out = ut::log::info() << "Ejecting stage.";
      PrintStatusHeader(simTime, simTime, out);
   }
   mMoverPtr->EjectStage(simTime, aPreSeparationCoastTime, aPreIgnitionCoastTime);
   return ok;
}

// ================================================================================================
//! Called from script interface to command the mover to stop the engines.
//! @return true if successful, false if not.
bool WsfGuidanceComputer::StopEngines()
{
   bool   ok(true);
   double simTime(GetSimTime());
   if (mShowCommands)
   {
      auto out = ut::log::info() << "Stopping engines.";
      PrintStatusHeader(simTime, simTime, out);
   }
   mMoverPtr->SetCommandedEngineStopTime(simTime);
   return ok;
}

// ================================================================================================
//! Called from script interface to start the specified guidance program.
//! @param aProgramName The name (ID) of the program to be started.
//! @return true if successful, false if not.
bool WsfGuidanceComputer::StartProgram(WsfStringId aProgramName)
{
   bool ok(true);
   if (IsTurnedOn())
   {
      double simTime = GetSimTime();
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Starting program.";
         PrintStatusHeader(simTime, simTime, out);
         out.AddNote() << "Program: " << aProgramName;
      }
      StartProgram(simTime, simTime, aProgramName);
   }
   return ok;
}

// ================================================================================================
//! Called from script interface to stop the specified guidance program.
//! @param aProgramName The name (ID) of the program to be stopped.
//! @return true if successful, false if not.
bool WsfGuidanceComputer::StopProgram(WsfStringId aProgramName)
{
   bool ok(true);
   if (IsTurnedOn())
   {
      double simTime = GetSimTime();
      if (mShowCommands)
      {
         auto out = ut::log::info() << "Stopping program.";
         PrintStatusHeader(simTime, simTime, out);
         out.AddNote() << "Program: " << aProgramName;
      }
      StopProgram(simTime, simTime, aProgramName);
   }
   return ok;
}

// ================================================================================================
//! A callback from the mover that indicates the current stage is igniting.
//! @param aSimTime   The current simulation time.
//! @param aEventTime The actual time the stage ignited. This may be less than the current simulation time.
void WsfGuidanceComputer::StageIgnition(double aSimTime, double aEventTime)
{
   if (mShowStatus)
   {
      auto out = ut::log::info() << "Detected stage ignition.";
      PrintStatusHeader(aSimTime, aEventTime, out);
      out.AddNote() << "Stage: " << mMoverPtr->CurrentStage() + 1;
   }
   if (mShowGraphics)
   {
      UpdateWeaponPerception(aEventTime);
      DrawPoint(0.2, 0.7, 0.2, 8);
   }
   SelectPhase(aSimTime, aEventTime, mPhaseList[mPhaseIndex].mStageIgnitionPhaseName);
}

// ================================================================================================
//! A callback from the mover that indicates the current stage has just burned-out.
//! @param aSimTime   The current simulation time.
//! @param aEventTime The actual time the stage burned-out. This may be less than the current simulation time.
void WsfGuidanceComputer::StageBurnout(double aSimTime, double aEventTime)
{
   if (mShowStatus)
   {
      auto out = ut::log::info() << "Detected stage burnout.";
      PrintStatusHeader(aSimTime, aEventTime, out);
      out.AddNote() << "Stage: " << mMoverPtr->CurrentStage() + 1;
   }
   if (mShowGraphics)
   {
      UpdateWeaponPerception(aEventTime);
      DrawPoint(0.2, 0.7, 0.2, 8);
   }
   // Note: only one of stage_burnout or boost_complete may be specified.

   if (!mPhaseList[mPhaseIndex].mStageBurnoutPhaseName.IsNull())
   {
      SelectPhase(aSimTime, aEventTime, mPhaseList[mPhaseIndex].mStageBurnoutPhaseName);
   }
   else if ((!mPhaseList[mPhaseIndex].mBoostCompletePhaseName.IsNull()) && mMoverPtr->IsExtrapolating())
   {
      // IsExtrapolating means boost complete (see WsfGuidedMoverBase.)
      SelectPhase(aSimTime, aEventTime, mPhaseList[mPhaseIndex].mBoostCompletePhaseName);
   }
}

// ================================================================================================
//! A callback from the mover that indicates the current stage is separating.
//! @param aSimTime   The current simulation time.
//! @param aEventTime The actual time the stage separated. This may be less than the current simulation time.
void WsfGuidanceComputer::StageSeparation(double aSimTime, double aEventTime)
{
   if (mShowStatus)
   {
      auto out = ut::log::info() << "Detected stage separation.";
      PrintStatusHeader(aSimTime, aEventTime, out);
      out.AddNote() << "Stage: " << mMoverPtr->CurrentStage() + 1;
   }
   if (mShowGraphics)
   {
      UpdateWeaponPerception(aEventTime);
      DrawPoint(0.2, 0.7, 0.2, 8);
   }
   SelectPhase(aSimTime, aEventTime, mPhaseList[mPhaseIndex].mStageSeparationPhaseName);
}

// ================================================================================================
//! Return the guidance time constant for the legacy guidance program.
// protected
double WsfGuidanceComputer::GetTimeConstant(const Phase& aPhase) const
{
   double timeConstant = mTimeConstant;
   if (timeConstant <= 0.0)
   {
      timeConstant = aPhase.mTimeConstant;
   }
   return timeConstant;
}

// ================================================================================================
//                            Protected Methods
// ================================================================================================

// ================================================================================================
//! Begin processing for an external command (i.e.: from a script or other external source)
bool WsfGuidanceComputer::BeginExternalCommand(WsfStringId aPhaseName, unsigned int& aPhaseIndex)
{
   bool ok(true);
   if (aPhaseName.IsNull()) // Applies to the current phase
   {
      aPhaseIndex = mPhaseIndex;
   }
   else // Applied to the specified phase
   {
      aPhaseIndex = FindPhase(aPhaseName);
      if (aPhaseIndex >= mPhaseList.size())
      {
         auto out = ut::log::error() << "Invalid phase name specified on a script command.";
         out.AddNote() << "T = " << GetSimTime();
         out.AddNote() << "Phase: " << aPhaseName;
         ok = false;
      }
   }
   return ok;
}

// ================================================================================================
//! If 'show_status' is true, begin the echoing of the external command.
//! @param aPhaseIndex The index of the phase to which the command applies.
//! @param aCommandPtr The command being processed.
//! @param aStream The stream being written to.
//! @returns if the echoing process has started. The caller MUST write any other data to cout, along with an end-of-line.
void WsfGuidanceComputer::BeginExternalCommandEcho(unsigned int            aPhaseIndex,
                                                   const char*             aCommandPtr,
                                                   ut::log::MessageStream& aStream)
{
   double simTime(GetSimTime());
   PrintStatusHeader(simTime, simTime, aStream);
   aStream.AddNote() << "Phase: " << mPhaseList[aPhaseIndex].mPhaseName;
   aStream.AddNote() << "Command: " << aCommandPtr;
}

// ================================================================================================
//! Check for a change in the guidance phase.
void WsfGuidanceComputer::CheckForPhaseChange(double aSimTime, double aEndTime)
{
   Phase& phase = mPhaseList[mPhaseIndex];

   // If a 'next_phase program <name> complete' has been satisfied then it takes precedence over the others

   if (mProgramCompleteIndex < phase.mProgramCompletePhaseName.size())
   {
      WsfStringId phaseName(phase.mProgramCompletePhaseName[mProgramCompleteIndex].second);
      SelectPhase(aSimTime, aEndTime, phaseName);
      return;
   }

   double timeToIntercept  = -1.0;
   double rangeToIntercept = -1.0;
   for (PhaseChange& phaseChange : phase.mPhaseChangeList)
   {
      phaseChange.mValue.Update(*mContextPtr); // Ensure reference value is updated if a script reference
      double value       = 0.0;
      bool   changePhase = false;
      switch (phaseChange.mName)
      {
      case PhaseChange::cPHASE_TIME:
      {
         value       = aEndTime - mPhaseStartTime;
         changePhase = phaseChange.ConditionSatisfied(value);
      }
      break;
      case PhaseChange::cFLIGHT_TIME:
      {
         value       = aEndTime - mLaunchTime;
         changePhase = phaseChange.ConditionSatisfied(value);
      }
      break;
      case PhaseChange::cALTITUDE:
      {
         value       = mState.mWpnState.GetAltitude();
         changePhase = phaseChange.ConditionSatisfied(value);
      }
      break;
      case PhaseChange::cSPEED:
      {
         value       = mState.mWpnState.GetSpeed();
         changePhase = phaseChange.ConditionSatisfied(value);
      }
      break;
      case PhaseChange::cVERTICAL_SPEED:
      {
         double wpnVelNED[3];
         mState.mWpnState.GetVelocityNED(wpnVelNED);
         value       = -wpnVelNED[2]; // + is up, - is down
         changePhase = phaseChange.ConditionSatisfied(value);
      }
      break;
      case PhaseChange::cFLIGHT_PATH_ANGLE:
      {
         value       = mState.mFlightPathAngle;
         changePhase = phaseChange.ConditionSatisfied(value);
      }
      break;
      case PhaseChange::cON_COMMANDED_FLIGHT_PATH_ANGLE:
      {
         if (mCommandedFlightPathAngle != cUNDEFINED_DOUBLE)
         {
            value = mState.mFlightPathAngle;
            // If we're real close to the desired angle or if we cross the desired angle then
            // a phase change is indicated.
            if (fabs(mState.mFlightPathAngle - mCommandedFlightPathAngle) <= cFLIGHT_PATH_ANGLE_TOLERANCE)
            {
               changePhase = true;
            }
            else if ((mState.mFlightPathAngle <= mCommandedFlightPathAngle) &&
                     (mLastFlightPathAngle >= mCommandedFlightPathAngle))
            {
               changePhase = true;
            }
            else if ((mState.mFlightPathAngle >= mCommandedFlightPathAngle) &&
                     (mLastFlightPathAngle <= mCommandedFlightPathAngle))
            {
               changePhase = true;
            }
         }
      }
      break;
      case PhaseChange::cDYNAMIC_PRESSURE:
      {
         double density = mAtmosphere.Density(mState.mWpnState.GetAltitude());
         double speed   = mState.mWpnState.GetSpeed();
         value          = 0.5 * density * speed * speed;
         changePhase    = phaseChange.ConditionSatisfied(value);
      }
      break;
      case PhaseChange::cTARGET_ALTITUDE:
         if (mTgtStateDefined)
         {
            double notUsed;
            mState.mTgtState.GetLocationLLA(notUsed, notUsed, value);
            changePhase = phaseChange.ConditionSatisfied(value);
         }
         break;
      case PhaseChange::cTARGET_SPEED:
         if (mTgtStateDefined)
         {
            value       = mState.mTgtState.GetSpeed();
            changePhase = phaseChange.ConditionSatisfied(value);
         }
         break;
      case PhaseChange::cTARGET_FLIGHT_PATH_ANGLE:
         if (mTgtStateDefined)
         {
            double tgtVelNED[3];
            mState.mTgtState.GetVelocityNED(tgtVelNED);
            double tgtVelNE = sqrt((tgtVelNED[0] * tgtVelNED[0]) + (tgtVelNED[1] * tgtVelNED[1]));
            value           = atan2(-tgtVelNED[2], tgtVelNE);
            changePhase     = phaseChange.ConditionSatisfied(value);
         }
         break;
      case PhaseChange::cCLOSING_SPEED:
         if (mTgtStateDefined)
         {
            double relTgtLocWCS[3];
            mState.mWpnState.GetRelativeLocationWCS(&(mState.mTgtState), relTgtLocWCS);
            if (UtVec3d::Normalize(relTgtLocWCS) > 1.0)
            {
               double wpnVelWCS[3];
               mState.mWpnState.GetVelocityWCS(wpnVelWCS);

               // Compute the relative WCS velocity (V_w - V_t)
               double tgtVelWCS[3];
               mState.mTgtState.GetVelocityWCS(tgtVelWCS);
               double relTgtVelWCS[3];
               UtVec3d::Subtract(relTgtVelWCS, wpnVelWCS, tgtVelWCS);

               // Compute the magnitude of the closing velocity vector along the relative position vector.
               value = UtVec3d::DotProduct(relTgtVelWCS, relTgtLocWCS);
               // Relative position vector has already been normalized.
               // value /= max(UtVec3d::Magnitude(relTgtLocWCS), 1.0);
               changePhase = phaseChange.ConditionSatisfied(value);
            }
         }
         break;
      case PhaseChange::cRANGE_TO_INTERCEPT:
         value = cLARGE_POSITIVE_DOUBLE;
         if (mTgtStateDefined)
         {
            if (timeToIntercept < 0.0)
            {
               ComputeInterceptData(aEndTime, timeToIntercept, rangeToIntercept);
            }
            value       = rangeToIntercept;
            changePhase = phaseChange.ConditionSatisfied(value);
         }
         break;
      case PhaseChange::cTIME_TO_INTERCEPT:
         value = cLARGE_POSITIVE_DOUBLE;
         if (mTgtStateDefined)
         {
            if (timeToIntercept < 0.0)
            {
               ComputeInterceptData(aEndTime, timeToIntercept, rangeToIntercept);
            }
            value       = timeToIntercept;
            changePhase = phaseChange.ConditionSatisfied(value);
         }
         break;
      case PhaseChange::cTARGET_SLANT_RANGE:
         value = cLARGE_POSITIVE_DOUBLE;
         if (mTgtStateDefined)
         {
            double tgtLocNED[3];
            mState.mWpnState.GetRelativeLocationNED(&mState.mTgtState, tgtLocNED);
            value       = UtVec3d::Magnitude(tgtLocNED);
            changePhase = phaseChange.ConditionSatisfied(value);
         }
         break;
      case PhaseChange::cTARGET_GROUND_RANGE:
         value = cLARGE_POSITIVE_DOUBLE;
         if (mTgtStateDefined)
         {
            double tgtLocNED[3];
            mState.mWpnState.GetRelativeLocationNED(&mState.mTgtState, tgtLocNED);
            // TODO - approximate...
            value       = sqrt(tgtLocNED[0] * tgtLocNED[0] + tgtLocNED[1] * tgtLocNED[1]);
            changePhase = phaseChange.ConditionSatisfied(value);
         }
         break;
      case PhaseChange::cTARGET_ELEVATION:
         if (mTgtStateDefined)
         {
            double tgtLocNED[3];
            mState.mWpnState.GetRelativeLocationNED(&mState.mTgtState, tgtLocNED);
            if (UtVec3d::MagnitudeSquared(tgtLocNED) > 1.0)
            {
               value       = ComputeElevation(tgtLocNED);
               changePhase = phaseChange.ConditionSatisfied(value);
            }
         }
         break;
      case PhaseChange::cTARGET_AZIMUTH:
         if (mTgtStateDefined)
         {
            double tgtLocNED[3];
            mState.mWpnState.GetRelativeLocationNED(&mState.mTgtState, tgtLocNED);
            double wpnVelNED[3];
            mState.mWpnState.GetVelocityNED(wpnVelNED);
            tgtLocNED[2] = 0.0;
            wpnVelNED[2] = 0.0;
            if ((UtVec3d::Normalize(tgtLocNED) > 1.0) && (UtVec3d::Normalize(wpnVelNED) > 1.0))
            {
               double cosTheta = UtVec3d::DotProduct(tgtLocNED, wpnVelNED);
               value           = fabs(acos(UtMath::Limit(cosTheta, 1.0)));
               changePhase     = phaseChange.ConditionSatisfied(value);
            }
         }
         break;
      case PhaseChange::cLOS_TARGET_ELEVATION:
         if (mTgtStateDefined)
         {
            double relTgtLocECS[3];
            mState.mWpnState.GetRelativeLocationECS(&mState.mTgtState, relTgtLocECS);
            value = UtMath::cPI_OVER_2;
            if (UtVec3d::MagnitudeSquared(relTgtLocECS) > 1.0)
            {
               value       = ComputeElevation(relTgtLocECS);
               changePhase = phaseChange.ConditionSatisfied(value);
            }
         }
         break;
      case PhaseChange::cLOS_TARGET_AZIMUTH:
         if (mTgtStateDefined)
         {
            double relTgtLocECS[3];
            mState.mWpnState.GetRelativeLocationECS(&mState.mTgtState, relTgtLocECS);
            value = UtMath::cPI;
            if (UtVec3d::MagnitudeSquared(relTgtLocECS) > 1.0)
            {
               value       = fabs(ComputeAzimuth(relTgtLocECS));
               changePhase = phaseChange.ConditionSatisfied(value);
            }
         }
         break;
      case PhaseChange::cLOS_TARGET_ANGLE:
         if (mTgtStateDefined)
         {
            double relTgtLocECS[3];
            mState.mWpnState.GetRelativeLocationECS(&mState.mTgtState, relTgtLocECS);
            double range = UtVec3d::Normalize(relTgtLocECS);
            value        = UtMath::cPI;
            if (range > 1.0)
            {
               // Use definition of dot product to get angle between entity longitudinal (X) axis and
               // the vector from entity to target. Both vectors have been normalized.
               double xAxisVecECS[3] = {1.0, 0.0, 0.0};
               double cosTheta       = UtVec3d::DotProduct(xAxisVecECS, relTgtLocECS);
               value                 = acos(UtMath::Limit(cosTheta, 1.0));
               changePhase           = phaseChange.ConditionSatisfied(value);
            }
         }
         break;
      default:
         auto out = ut::log::error() << "Unknown phase change variable.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Computer: " << GetName();
         continue;
      }

      if (mShowEvaluations)
      {
         std::string variable;
         double      scale          = 1.0;
         double      referenceValue = phaseChange.mValue;
         switch (phaseChange.mName)
         {
         case PhaseChange::cPHASE_TIME:
            variable = "phase_time";
            break;
         case PhaseChange::cFLIGHT_TIME:
            variable = "flight_time";
            break;
         case PhaseChange::cALTITUDE:
            variable = "altitude";
            break;
         case PhaseChange::cSPEED:
            variable = "speed";
            break;
         case PhaseChange::cVERTICAL_SPEED:
            variable = "vertical_speed";
            break;
         case PhaseChange::cFLIGHT_PATH_ANGLE:
            variable = "flight_path_angle";
            scale    = UtMath::cDEG_PER_RAD;
            break;
         case PhaseChange::cON_COMMANDED_FLIGHT_PATH_ANGLE:
            variable       = "on_commanded_flight_path_angle";
            referenceValue = mCommandedFlightPathAngle;
            scale          = UtMath::cDEG_PER_RAD;
            break;
         case PhaseChange::cDYNAMIC_PRESSURE:
            variable = "dynamic_pressure";
            break;
         case PhaseChange::cTARGET_ALTITUDE:
            variable = "target_altitude";
            break;
         case PhaseChange::cTARGET_SPEED:
            variable = "target_speed";
            break;
         case PhaseChange::cTARGET_FLIGHT_PATH_ANGLE:
            variable = "target_flight_path_angle";
            scale    = UtMath::cDEG_PER_RAD;
            break;
         case PhaseChange::cCLOSING_SPEED:
            variable = "closing_speed";
            break;
         case PhaseChange::cTIME_TO_INTERCEPT:
            variable = "time_to_intercept";
            break;
         case PhaseChange::cRANGE_TO_INTERCEPT:
            variable = "range_to_intercept";
            break;
         case PhaseChange::cTARGET_SLANT_RANGE:
            variable = "target_slant_range";
            break;
         case PhaseChange::cTARGET_GROUND_RANGE:
            variable = "target_ground_range";
            break;
         case PhaseChange::cTARGET_ELEVATION:
            variable = "target_elevation";
            scale    = UtMath::cDEG_PER_RAD;
            break;
         case PhaseChange::cTARGET_AZIMUTH:
            variable = "target_azimuth";
            scale    = UtMath::cDEG_PER_RAD;
            break;
         case PhaseChange::cLOS_TARGET_ELEVATION:
            variable = "los_target_elevation";
            scale    = UtMath::cDEG_PER_RAD;
            break;
         case PhaseChange::cLOS_TARGET_AZIMUTH:
            variable = "los_target_azimuth";
            scale    = UtMath::cDEG_PER_RAD;
            break;
         case PhaseChange::cLOS_TARGET_ANGLE:
            variable = "los_target_angle";
            scale    = UtMath::cDEG_PER_RAD;
            break;
         default:
            variable = "??";
            break;
         }

         auto out = ut::log::info() << "Evaluation:";
         out.AddNote() << "T = " << aEndTime << " (" << aEndTime - mLaunchTime << ") ";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();


         std::string condition;
         switch (phaseChange.mCondition)
         {
         case PhaseChange::cLESS:
            condition = "<";
            break;
         case PhaseChange::cLESS_OR_EQUAL:
            condition = "<=";
            break;
         case PhaseChange::cEQUAL:
            condition = "==";
            break;
         case PhaseChange::cNOT_EQUAL:
            condition = "!=";
            break;
         case PhaseChange::cGREATER_OR_EQUAL:
            condition = ">=";
            break;
         case PhaseChange::cGREATER:
            condition = ">";
            break;
         default:
            condition = "??";
            break;
         }
         out.AddNote() << "Test condition: " << variable << " (" << scale * value << ' ' << condition << ' '
                       << scale * referenceValue << ")";
         out.AddNote() << "Result: " << changePhase;
      }
      if (changePhase)
      {
         SelectPhase(aSimTime, aEndTime, phaseChange.mNextPhaseName);
         break;
      }
   }
}

// ================================================================================================
//! Determine if a waypoint switch is required.
void WsfGuidanceComputer::CheckForWaypointChange(double aSimTime, double aEndTime)
{
   // Only the horizontal components of position are checked.
   const WsfRoute* routePtr = mMoverPtr->GetRoute();
   if ((routePtr != nullptr) && (mRouteIndex < routePtr->GetSize()))
   {
      const WsfWaypoint& waypt  = routePtr->GetWaypointAt(mRouteIndex);
      double             tgtLat = waypt.GetLat();
      double             tgtLon = waypt.GetLon();
      double             tgtLocWCS[3];
      WsfPlatform::ConvertLLAToWCS(tgtLat, tgtLon, 0.0, tgtLocWCS);
      double relTgtLocNED[3];
      mState.mWpnState.ConvertWCSToNED(tgtLocWCS, relTgtLocNED);
      relTgtLocNED[2] = 0.0; // Only consider the horizontal components

      bool nowClosing = false;
      if (UtVec3d::Normalize(relTgtLocNED) > 1.0)
      {
         double wpnVelNED[3];
         mState.mWpnState.GetVelocityNED(wpnVelNED);
         wpnVelNED[2] = 0.0; // Only consider the horizontal components

         // Compute the magnitude of the closing velocity vector along the relative position vector.
         double closingSpeed = UtVec3d::DotProduct(wpnVelNED, relTgtLocNED);

         // Relative position vector has already been normalized.
         // closingSpeed /= max(UtVec3d::Magnitude(relTgtLocNED), 1.0);
         nowClosing = (closingSpeed > 0.0);
      }
      if (mWasClosing && (!nowClosing))
      {
         // We were closing in the previous time step but now receding - we've passed the waypoint.
         SelectWaypoint(aSimTime, aEndTime, mRouteIndex + 1);

         // Reset the closing flag
         mWasClosing = false;
      }
      else
      {
         mWasClosing = nowClosing;
      }
   }
}

// ================================================================================================
//! A helper for CheckForPhaseChange to compute the azimuth from a relative location vector.
//! The incoming vector is assumed to be non-zero in length.
// protected
double WsfGuidanceComputer::ComputeAzimuth(const double aRelLoc[3]) const
{
   double value = atan2(aRelLoc[1], aRelLoc[0]);
   return value;
}

// ================================================================================================
//! A helper for CheckForPhaseChange to compute the elevation from a relative location vector.
//! The incoming vector is assumed to be non-zero in length.
// protected
double WsfGuidanceComputer::ComputeElevation(const double aRelLoc[3]) const
{
   double value = 0.0;
   double xy    = sqrt((aRelLoc[0] * aRelLoc[0]) + (aRelLoc[1] * aRelLoc[1]));
   if (xy != 0.0)
   {
      value = -atan2(aRelLoc[2], xy);
   }
   else
   {
      value = UtMath::cPI_OVER_2;
      if (aRelLoc[2] > 0.0)
      {
         value = -UtMath::cPI_OVER_2;
      }
   }
   return value;
}

// ================================================================================================
//! A helper for CheckForPhaseChange to compute the time and range to intercept.
// protected
void WsfGuidanceComputer::ComputeInterceptData(double aCurTime, double& aTimeToGo, double& aRangeToGo)
{
   // Assume no intercept.
   aTimeToGo  = cLARGE_POSITIVE_DOUBLE;
   aRangeToGo = cLARGE_POSITIVE_DOUBLE;

   const WsfTrack* targetPtr = GetPlatform()->GetTrackManager().GetCurrentTarget();
   if (targetPtr != nullptr)
   {
      double wpnLocWCS[3];
      double wpnVelWCS[3];
      GetPlatform()->GetLocationWCS(wpnLocWCS);
      GetPlatform()->GetVelocityWCS(wpnVelWCS);
      double wpnSpeed = UtVec3d::Magnitude(wpnVelWCS);
      double tgtLocWCS[3];
      mState.mTgtState.GetLocationWCS(tgtLocWCS);
      double tgtVelWCS[3];
      mState.mTgtState.GetVelocityWCS(tgtVelWCS);

      // Find the minimum-time intercept location.
      double intLocWCS[3];
      double intTime = WsfIntercept::Intercept(wpnLocWCS, wpnSpeed, tgtLocWCS, tgtVelWCS, intLocWCS);
      if (intTime >= 0)
      {
         // Compute the range and time to the intercept location.
         double wpnToIntLocWCS[3];
         UtVec3d::Subtract(wpnToIntLocWCS, intLocWCS, wpnLocWCS);
         aRangeToGo = UtVec3d::Magnitude(wpnToIntLocWCS);
         aTimeToGo  = aRangeToGo / std::max(1.0, wpnSpeed);
      }
   }
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Computer intercept data:";
      out.AddNote() << "T = " << aCurTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Computer: " << GetName();
      out.AddNote() << "TTG: " << aTimeToGo;
      out.AddNote() << "RTG: " << aRangeToGo;
   }
}

// =================================================================================================
void WsfGuidanceComputer::DrawPoint(double aRedValue, double aGreenValue, double aBlueValue, int aPointSize)
{
   double locWCS[3];
   mState.mWpnState.GetLocationWCS(locWCS);
   mDrawPtr->SetId(mDrawPtr->GetNewId());
   mDrawPtr->SetPointSize(aPointSize);
   mDrawPtr->SetDuration(WsfDraw::cFOREVER);
   mDrawPtr->SetColor(aRedValue, aGreenValue, aBlueValue);
   mDrawPtr->BeginPoints();
   mDrawPtr->VertexWCS(locWCS);
   mDrawPtr->VertexWCS(locWCS); // Need two point because of VESPA bug
   mDrawPtr->End();
}

// ================================================================================================
//! Return the index of the phase with the specified name.
//! @param aPhaseName The name (ID) of the phase to be located.
//! @returns The index of the specified phase. If it could not be found then it
//!          will be the size of the phase list.
// protected
unsigned int WsfGuidanceComputer::FindPhase(WsfStringId aPhaseName) const
{
   for (unsigned int phaseIndex = 0; phaseIndex < mPhaseList.size(); ++phaseIndex)
   {
      if (aPhaseName == mPhaseList[phaseIndex].mPhaseName)
      {
         return phaseIndex;
      }
   }
   return static_cast<unsigned int>(mPhaseList.size());
}

// ================================================================================================
//! Return the pointer to the program in the defined program list with the specified name.
//! @param aProgramName The name (ID) of the program to be located.
//! @returns The pointer to the requested program or zero if it could not be found.
// protected
WsfGuidanceProgram* WsfGuidanceComputer::FindProgram(WsfStringId aProgramName) const
{
   for (auto definedProgram : mDefinedPrograms)
   {
      if (definedProgram->GetNameId() == aProgramName)
      {
         return definedProgram;
      }
   }
   return nullptr;
}

// ================================================================================================
// protected
double WsfGuidanceComputer::GetSimTime() const
{
   return (GetSimulation() == nullptr) ? 0.0 : GetSimulation()->GetSimTime();
}

// ================================================================================================
void WsfGuidanceComputer::PrintStatusHeader(double aSimTime, double aCurTime, ut::log::MessageStream& aStream)
{
   auto note = aStream.AddNote() << std::setprecision(5) << "Status:";
   note.AddNote() << "T = " << aSimTime << " (" << aCurTime - mLaunchTime << ") ";
   note.AddNote() << "Platform: " << GetPlatform()->GetName();
   note.AddNote() << "Computer: " << GetName();
}

// ================================================================================================
//! Select the indicated phase as the current phase.
//! @param aSimTime    The current simulation time.
//! @param aChangeTime The time at which the phase change is actually occurring. This may be less than
//!                    the current simulation time.
//! @param aPhaseName  The name (dictionary ID) of the phase that is to become the current phase.
// protected
void WsfGuidanceComputer::SelectPhase(double aSimTime, double aChangeTime, WsfStringId aPhaseName)
{
   // Exit immediately if a phase name was not specified.
   if (aPhaseName.IsNull())
   {
      return;
   }

   unsigned int newPhaseIndex = FindPhase(aPhaseName);
   if (newPhaseIndex >= mPhaseList.size())
   {
      auto out = ut::log::error() << "Requested guidance phase does not exist.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Requested Phase: " << aPhaseName;
      out.AddNote() << "Guidance phase will not be changed.";
      return;
   }

   unsigned int savedPhaseIndex = mPhaseIndex;

   // Execute the 'on_exit' block for the phase we are leaving.
   if ((mPhaseIndex < mPhaseList.size()) && (!mOnExitScriptActive))
   {
      // If the on_exit script executes a SelectPhase command,
      //   1) The on_exit script should not be executed again (the SelectPhase will call this routine!)
      //   2) The command will cause a phase change, so we shouldn't keep going (the new phase has already
      //      been selected.

      mOnExitScriptActive = true;
      mPhaseList[mPhaseIndex].ExecuteOnExit(aSimTime, *mContextPtr);
      mOnExitScriptActive = false;
      if (mPhaseIndex != savedPhaseIndex)
      {
         return;
      }
   }

   // Enter the new phase.
   //
   // From this point on we are official in the new phase (mPhaseIndex is set to the new phase)
   // Anything that causes a phase change from this point on must go through exit processing.

   Phase& phase    = mPhaseList[newPhaseIndex];
   mPhaseIndex     = newPhaseIndex;
   mPhaseStartTime = aChangeTime;

   // Notify subscribers to the phase change callback that the
   WsfObserver::GuidanceComputerPhaseChanged(GetSimulation())(aSimTime, this);

   if (mShowStatus)
   {
      { // RAII block
         auto out = ut::log::debug() << "Guidance computer changed phase.";
         PrintStatusHeader(aSimTime, aChangeTime, out);
         out.AddNote() << "New Phase: " << aPhaseName;
         mMoverPtr->WriteKinematicStatus(out);
      }
      mMoverPtr->GetPlatform()->Comment(aSimTime, "Guidance Change to " + aPhaseName);
   }

   if (mShowGraphics)
   {
      DrawPoint(0.2, 0.7, 0.2, 12);
   }

   // Execute the 'on_entry' and 'on_update' scripts. Either could cause a phase change (a recursive
   // call to this routine that selects yet a different phase). If a recursive call is detected then
   // simply exit so the phase selected by the recursive call is the one that is used.
   //
   // NOTE: We aren't really doing a guidance update here, but 'on_update' could alter script variables
   //       that are used within this method (e.g.: phase.mCommandedSpeed, etc.)

   savedPhaseIndex = mPhaseIndex;
   phase.ExecuteOnEntry(aSimTime, *mContextPtr);
   if (mPhaseIndex != savedPhaseIndex)
   {
      return;
   }

   phase.ExecuteOnUpdate(aSimTime, *mContextPtr);
   if (mPhaseIndex != savedPhaseIndex)
   {
      return;
   }

   mGuidanceStartTime = mPhaseStartTime + phase.mGuidanceDelay;

   if (phase.mCommandedSpeed == cUNDEFINED_DOUBLE)
   {
      mMoverPtr->SetCommandedSpeed(-1.0);
   }

   mCommandedFlightPathAngle = phase.mCommandedFlightPathAngle;
   if (phase.mCommandedFlightPathAngle == cFROM_LAUNCH_COMPUTER)
   {
      mCommandedFlightPathAngle = mLaunchComputerFlightPathAngle;
   }

   UpdateRouteFollowing(phase); // Determine if starting, continuing or stopping route following
   UpdateTerrainFollowing(phase);
   mAimpointExpireTime = -1.0; // Force re-computation of aimpoint if active

   // Activate used programs and deactivated unused programs.

   ProgramList newActivePrograms;
   for (const auto& program : phase.mPrograms)
   {
      WsfGuidanceProgram* programPtr = FindProgram(program);
      if (programPtr != nullptr)
      {
         newActivePrograms.push_back(programPtr);

         // If the program is not currently active, start it up.
         if (std::find(mActivePrograms.begin(), mActivePrograms.end(), programPtr) == mActivePrograms.end())
         {
            programPtr->Start(aSimTime, aChangeTime);
         }
      }
   }
   mActivePrograms       = newActivePrograms;
   mProgramCompleteIndex = phase.mProgramCompletePhaseName.size(); // See comments in the header file...
}

// ================================================================================================
//! Select the indicated waypoint as the current target waypoint.
void WsfGuidanceComputer::SelectWaypoint(double aSimTime, double aChangeTime, unsigned int aNewIndex)
{
   const WsfRoute* routePtr = mMoverPtr->GetRoute();
   if ((routePtr != nullptr) && (!routePtr->Empty()))
   {
      if (aNewIndex < routePtr->GetSize())
      {
         mRouteIndex     = aNewIndex;
         mFollowingRoute = true;
         if (mShowStatus)
         {
            auto out = ut::log::info() << "Changing to waypoint.";
            PrintStatusHeader(aSimTime, aChangeTime, out);
            out.AddNote() << "Waypoint: " << mRouteIndex;
         }
      }
      else
      {
         mFollowingRoute = false;
         if (mShowStatus)
         {
            auto out = ut::log::info() << "End of route encountered.";
            PrintStatusHeader(aSimTime, aChangeTime, out);
         }
         SelectPhase(aSimTime, aChangeTime, mPhaseList[mPhaseIndex].mEndOfRoutePhaseName);
      }
   }
   else
   {
      mFollowingRoute = false;
      auto out        = ut::log::info() << "Waypoint change requested but no route to follow.";
      PrintStatusHeader(aSimTime, aChangeTime, out);
   }
}

// ================================================================================================
// protected
void WsfGuidanceComputer::StartProgram(double aSimTime, double aChangeTime, WsfStringId aProgramName)
{
   // First make sure the program isn't already running.

   for (auto& activeProgram : mActivePrograms)
   {
      if (activeProgram->GetNameId() == aProgramName)
      {
         return;
      }
   }

   WsfGuidanceProgram* programPtr = FindProgram(aProgramName);
   if (programPtr != nullptr)
   {
      mActivePrograms.push_back(programPtr);
      programPtr->Start(aSimTime, aChangeTime);
   }
}

// ================================================================================================
// protected
void WsfGuidanceComputer::StopProgram(double aSimTime, double aChangeTime, WsfStringId aProgramName)
{
   for (auto pli = mActivePrograms.begin(); pli != mActivePrograms.end(); ++pli)
   {
      if ((*pli)->GetNameId() == aProgramName)
      {
         mActivePrograms.erase(pli);
         break;
      }
   }
}

// ================================================================================================
//! Update the aimpoint location to which we will guide.
//! @returns true if the aimpoint is valid.
// protected
bool WsfGuidanceComputer::UpdateAimpoint(double aCurTime, const Phase& aPhase)
{
   UtVec3d::Set(mState.mAimRelLocWCS, 0.0);
   UtVec3d::Set(mState.mAimUnitVecECS, 0.0);
   mState.mAimpointIsValid  = false; // Assume nothing to guide to...
   mState.mAimpointIsTarget = false; // true if the aimpoint is the target and not a waypoint

   // The aimpoint lat/lon/alt
   double aimLat = 0.0;
   double aimLon = 0.0;
   double aimAlt = 0.0;
   // The 'raw' aimpoint altitude used to draw the aimpoint location.
   // This is generally the target altitude + aimpoint_altitude_offset.
   double rawAimAlt   = 0.0;
   bool   aimAltIsAGL = false;

   // Assume the aimpoint is the intended target.
   if (mTgtStateDefined)
   {
      mState.mAimpointIsValid  = true;
      mState.mAimpointIsTarget = true;
      mState.mTgtState.GetLocationLLA(aimLat, aimLon, aimAlt);
      rawAimAlt = aimAlt + aPhase.mAimpointAltitudeOffset;
      if (aPhase.mCommandedAlt != cUNDEFINED_DOUBLE)
      {
         aimAlt      = aPhase.mCommandedAlt;
         aimAltIsAGL = aPhase.mCommandedAltIsAGL;
      }
      else
      {
         aimAlt += aPhase.mAimpointAltitudeOffset;
      }
   }

   // If route following is enabled AND there is route to follow, then follow it!
   if (mFollowingRoute)
   {
      const WsfRoute* routePtr = mMoverPtr->GetRoute();
      if ((routePtr != nullptr) && (mRouteIndex < routePtr->GetSize()))
      {
         mState.mAimpointIsValid  = true;
         mState.mAimpointIsTarget = false;
         const WsfWaypoint& waypt = routePtr->GetWaypointAt(mRouteIndex);
         aimLat                   = waypt.GetLat();
         aimLon                   = waypt.GetLon();
         // A waypoint altitude (if specified) takes precedence over the phase commanded altitude or the target altitude.
         if (waypt.GetAlt() != WsfPath::cDOUBLE_NOT_SET)
         {
            aimAlt      = waypt.GetAlt();
            aimAltIsAGL = (waypt.GetAltRef() == WsfPath::cALT_REF_AGL);
         }
      }
   }
   else if ((aPhase.mGuidanceTarget == cGT_PREDICTED_INTERCEPT) && (!UtVec3d::Equals(mInterceptLocWCS, 0.0)))
   {
      mState.mAimpointIsValid  = true;
      mState.mAimpointIsTarget = false;
      UtEntity::ConvertWCSToLLA(mInterceptLocWCS, aimLat, aimLon, aimAlt);
      aimAltIsAGL = false;
      rawAimAlt   = aimAlt;
   }

   if (mState.mAimpointIsValid)
   {
      // If the commanded altitude was specified as 'above-ground-level', then adjust the altitude
      // by the height of the terrain UNDER THE WEAPON LOCATION - not the height at the target location.
      // An above-ground-level specification assumes some sort of terrain following.

      if (aimAltIsAGL)
      {
         double wpnLat;
         double wpnLon;
         double notUsed;
         mState.mWpnState.GetLocationLLA(wpnLat, wpnLon, notUsed);
         float terrainHeight;
         mTerrainPtr->GetElevInterp(wpnLat, wpnLon, terrainHeight);
         aimAlt += terrainHeight;
      }

      // If some sort of lateral aimpoint offset has been provided, compute the offset location.
      // The offset is applied ONLY if the aimpoint is the target - not a waypoint.
      //
      // The aimpoint offset is updated 'infrequently' to because the computation is fairly expensive
      // and the locations doesn't change that much if the weapon and target are not maneuvering much.
      // (which should be the case when using this form of guidance).

      if (mState.mAimpointIsTarget && (aPhase.mAimpointRangeOffset != 0.0))
      {
         // If a valid computed aimpoint currently exists then make sure we haven't flown past it.
         // Generally a phase should be set up to recognize this condition, but because of timing and
         // numerical issues it is possible that it will be passed. We don't want to leave it at the
         // current value because that would cause and abrupt 'about-face', and we don't want to create
         // a new aimpoint because it would lead to a situation where the phase change could not be
         // detected because the aimpoint kept moving.
         //
         // So if this situation occurs, we'll simply move the aimpoint slight ahead of the current
         // weapon location, thus keeping the weapon on the current heading and hoping the condition
         // for a the phase change will soon be detected.

         bool aimpointOffsetUpdated = false;
         if (mAimpointExpireTime >= 0.0)
         {
            double aimLocNED[3];
            double wpnVelNED[3];
            mState.mWpnState.GetVelocityNED(wpnVelNED);
            mState.mWpnState.ConvertWCSToNED(mAimpointLocWCS, aimLocNED);
            double dot = UtVec3d::DotProduct(wpnVelNED, aimLocNED);
            if (dot < 0.0)
            {
               // The point is behind us. Only perform the extrapolation if we're reasonable close to the
               // point (1 second). If we're far away this could be a valid point...
               double speed_sq = UtVec3d::MagnitudeSquared(wpnVelNED);
               double dist_sq  = UtVec3d::MagnitudeSquared(aimLocNED);
               double time_sq  = dist_sq / std::max(speed_sq, 0.1);
               if (time_sq < 1.0)
               {
                  UtVec3d::Multiply(aimLocNED, wpnVelNED, 0.1);
                  mState.mWpnState.ConvertNEDToWCS(aimLocNED, mAimpointLocWCS);
                  mAimpointExpireTime   = aCurTime;
                  aimpointOffsetUpdated = true;
                  if (mShowGraphics)
                  {
                     double notUsed;
                     UtEntity::ConvertWCSToLLA(mAimpointLocWCS, notUsed, notUsed, rawAimAlt);
                  }
               }
            }
         } // if (mAimpointExpireTime >= 0.0

         if (aCurTime > mAimpointExpireTime)
         {
            // Compute the new aimpoint location.
            //
            // Target and weapon WCS locations are now on the surface!!!!

            double tgtLocWCS[3];
            double WCS_ToNED_Transform[3][3];
            UtEllipsoidalEarth::ComputeNEDTransform(aimLat, aimLon, 0.0, WCS_ToNED_Transform, tgtLocWCS);

            double wpnLat;
            double wpnLon;
            double notUsed;
            mState.mWpnState.GetLocationLLA(wpnLat, wpnLon, notUsed);
            double wpnLocWCS[3];
            UtEntity::ConvertLLAToWCS(wpnLat, wpnLon, 0.0, wpnLocWCS);

            // Compute the APPROXIMATE ground range and bearing from the target location to the weapon.

            double tgtToWpnLocWCS[3];
            double tgtToWpnLocNED[3];
            UtVec3d::Subtract(tgtToWpnLocWCS, wpnLocWCS, tgtLocWCS);
            UtMat3d::Transform(tgtToWpnLocNED, WCS_ToNED_Transform, tgtToWpnLocWCS);
            double tgtToWpnRange   = UtVec3d::Magnitude(tgtToWpnLocNED);
            double tgtToWpnBearing = atan2(tgtToWpnLocNED[1], tgtToWpnLocNED[0]);

            if (tgtToWpnRange < aPhase.mAimpointRangeOffset)
            {
               // If the weapon gets inside the range offset then the aimpoint is left unchanged. This typically
               // occurs when the aimpoint_azimuth_offset is > 90 degrees. In such cases the solution is ambiguous.

               if (mAimpointExpireTime < 0.0)
               {
                  // No previous aimpoint exists.... use the target location because nothing else can work right now.
                  UtEntity::ConvertLLAToWCS(aimLat, aimLon, aimAlt, mAimpointLocWCS);
               }
               mAimpointExpireTime = aCurTime + aPhase.mAimpointEvaluationInterval;
            }
            else
            {
               double sideA  = aPhase.mAimpointRangeOffset;
               double sideB  = tgtToWpnRange;
               double angleB = UtMath::cPI - aPhase.mAimpointAzimuthOffset;
               double sinA   = (sideA / sideB) * sin(angleB);
               double angleA = asin(sinA);

               // If a neither the left or right aimpoint was specifically chosen,
               // select the one that minimizes the change of weapon heading.

               OffsetDirection offsetDirection = aPhase.mOffsetDirection;
               if (offsetDirection == cOD_EITHER)
               {
                  double wpnToTgtHeading          = UtMath::NormalizeAngleMinusPi_Pi(tgtToWpnBearing + UtMath::cPI);
                  double wpnHeadingForRightOffset = UtMath::NormalizeAngleMinusPi_Pi(wpnToTgtHeading - angleA);
                  double wpnHeadingForLeftOffset  = UtMath::NormalizeAngleMinusPi_Pi(wpnToTgtHeading + angleA);
                  double turnForRightOffset =
                     UtMath::NormalizeAngleMinusPi_Pi(wpnHeadingForRightOffset - mState.mWpnHeading);
                  double turnForLeftOffset =
                     UtMath::NormalizeAngleMinusPi_Pi(wpnHeadingForLeftOffset - mState.mWpnHeading);
                  offsetDirection = (fabs(turnForRightOffset) <= fabs(turnForLeftOffset)) ? cOD_RIGHT : cOD_LEFT;
               }

               // Select the bearing from the target point that goes toward the aimpoint.

               double angleC          = aPhase.mAimpointAzimuthOffset - angleA;
               double tgtToAimBearing = 0.0;
               if (offsetDirection == cOD_RIGHT) // On right side of direct path
               {
                  tgtToAimBearing = UtMath::NormalizeAngleMinusPi_Pi(tgtToWpnBearing + angleC);
               }
               else // On left  side of direct path
               {
                  tgtToAimBearing = UtMath::NormalizeAngleMinusPi_Pi(tgtToWpnBearing - angleC);
               }

               // Compute the location of the aimpoint.

               double aimLocNED[3] = {aPhase.mAimpointRangeOffset * cos(tgtToAimBearing),
                                      aPhase.mAimpointRangeOffset * sin(tgtToAimBearing),
                                      0.0};
               double tgtToAimLocWCS[3];
               UtMat3d::InverseTransform(tgtToAimLocWCS, WCS_ToNED_Transform, aimLocNED);
               double aimLocWCS[3];
               UtVec3d::Add(aimLocWCS, tgtToAimLocWCS, tgtLocWCS);
               UtEntity::ConvertWCSToLLA(aimLocWCS, aimLat, aimLon, notUsed);
               UtEntity::ConvertLLAToWCS(aimLat, aimLon, aimAlt, mAimpointLocWCS);
               mAimpointExpireTime   = aCurTime + aPhase.mAimpointEvaluationInterval;
               aimpointOffsetUpdated = true;
            } // if (tgtToWpnRange < aPhase.mAimpointRangeOffset)
         }    // if (aCurTime > mAimpointExpireTime)

         // Set the current aimpoint as the target location.
         UtEntity::ConvertWCSToLLA(mAimpointLocWCS, aimLat, aimLon, aimAlt);

         if (mShowGraphics && aimpointOffsetUpdated)
         {
            mDrawPtr->SetId(mAimpointDrawId);
            mDrawPtr->Erase(mAimpointDrawId);
            mDrawPtr->SetLineSize(2);
            mDrawPtr->SetColor(0.6, 0.6, 0.6);
            mDrawPtr->BeginPolyline();
            mDrawPtr->VertexLLA(aimLat, aimLon, 0.0);
            mDrawPtr->VertexLLA(aimLat, aimLon, rawAimAlt);
            mDrawPtr->End();
            mDrawPtr->SetPointSize(6);
            mDrawPtr->SetColor(0.6, 0.6, 0.6);
            mDrawPtr->BeginPoints();
            mDrawPtr->VertexLLA(aimLat, aimLon, 0.0);
            mDrawPtr->VertexLLA(aimLat, aimLon, rawAimAlt);
            mDrawPtr->End();
         }
      }

      // Convert the aimpoint location to a weapon-relative WCS position vector.

      double aimLocWCS[3];
      UtEntity::ConvertLLAToWCS(aimLat, aimLon, aimAlt, aimLocWCS);
      double wpnLocWCS[3];
      mState.mWpnState.GetLocationWCS(wpnLocWCS);
      UtVec3d::Subtract(mState.mAimRelLocWCS, aimLocWCS, wpnLocWCS);

      // Convert the aimpoint vector to a wind-relative frame and no body roll.
      mState.mWpnState.ConvertWCSVectorToECS(mState.mAimUnitVecECS, mState.mAimRelLocWCS);
      UtVec3d::Normalize(mState.mAimUnitVecECS);
   }

   return mState.mAimpointIsValid;
}

// ================================================================================================
//! Determine if we are starting, continuing or stopping route following.
//! @param aPhase The current phase.
// protected
void WsfGuidanceComputer::UpdateRouteFollowing(const Phase& aPhase)
{
   bool wasFollowingRoute   = mFollowingRoute;
   mFollowingRoute          = false;
   const WsfRoute* routePtr = mMoverPtr->GetRoute();
   if ((routePtr != nullptr) && !routePtr->Empty())
   {
      if (aPhase.mAllowRouteFollowing)
      {
         if (!wasFollowingRoute)
         {
            // We've just started following the route.
            mRouteIndex     = 0;
            mFollowingRoute = true;
         }
         else
         {
            // Continue following the route (if not already off the end)
            if (mRouteIndex < routePtr->GetSize())
            {
               mFollowingRoute = true;
            }
         }
      }
   }
}

// ================================================================================================
//! Update the perception of the target.
// protected
void WsfGuidanceComputer::UpdateTargetPerception(double aCurTime)
{
   // Refresh perceived target location and velocity, if possible.

   const WsfTrack* trackPtr = GetPlatform()->GetTrackManager().GetCurrentTarget();
   if (trackPtr != nullptr)
   {
      double tgtLocWCS[3]  = {0.0, 0.0, 0.0};
      double tgtVelWCS[3]  = {0.0, 0.0, 0.0};
      double tgtAclWCS[3]  = {0.0, 0.0, 0.0};
      bool   tgtStateValid = false;
      double deltaTime     = aCurTime - mLastTgtUpdateTime;

      // Use truth information if guide_to_truth is true and the true target exists.
      bool guideToTruth = mGuideToTruth;

      // Make sure the phase index is good
      if (mPhaseIndex < static_cast<unsigned int>(mPhaseList.size()))
      {
         Phase& phase = mPhaseList[mPhaseIndex];
         if (phase.mGuidanceTarget == cGT_TRUTH)
         {
            guideToTruth = true;
         }
         else if (phase.mGuidanceTarget == cGT_PERCEPTION)
         {
            guideToTruth = false;
         }
      }
      if (guideToTruth)
      {
         WsfPlatform* truthPtr = GetSimulation()->GetPlatformByIndex(trackPtr->GetTargetIndex());
         if (truthPtr != nullptr)
         {
            // Force a full update of the target platform if the time since last updated is greater
            // that the update time of the mover we are guiding (a tolerance is used to ensure
            // the target gets updated a least at our update rate). If the target is accelerating and
            // we are in endgame, the precision may be 'necessary'.
            double dt = aCurTime - truthPtr->GetLastUpdateTime();
            if (dt >= (0.999 * mMoverPtr->GetUpdateInterval()))
            {
               truthPtr->Update(aCurTime);
            }
            tgtStateValid = true;
            truthPtr->GetLocationWCS(tgtLocWCS);
            truthPtr->GetVelocityWCS(tgtVelWCS);
            truthPtr->GetAccelerationWCS(tgtAclWCS);

            // Extrapolate position forward by the difference between the simulation time and the last platform update time.
            dt = aCurTime - truthPtr->GetLastUpdateTime();
            UtVec3d::AddProduct(tgtLocWCS, tgtLocWCS, dt, tgtVelWCS);
            UtVec3d::AddProduct(tgtLocWCS, tgtLocWCS, 0.5 * dt * dt, tgtAclWCS);
            UtVec3d::AddProduct(tgtVelWCS, tgtVelWCS, dt, tgtAclWCS);
         }
      }

      // Use perceived information if guide_to_truth is false and a perceived location can be derived.
      if ((!tgtStateValid) && trackPtr->GetExtrapolatedLocationWCS(aCurTime, tgtLocWCS))
      {
         tgtStateValid = true;
         UtVec3d::Set(tgtVelWCS, 0.0);
         if (trackPtr->VelocityValid())
         {
            trackPtr->GetVelocityWCS(tgtVelWCS);
         }
         else if (deltaTime > 1.0E-4)
         {
            // Track does not have velocity - construct using current and last known position.
            UtVec3d::Subtract(tgtVelWCS, mLastTgtLocWCS, tgtLocWCS);
            UtVec3d::Multiply(tgtVelWCS, 1.0 / deltaTime);
         }
         UtVec3d::Set(tgtAclWCS, 0.0);
         if (deltaTime > 1.0E-4)
         {
            UtVec3d::Subtract(tgtAclWCS, mLastTgtVelWCS, tgtVelWCS);
            UtVec3d::Multiply(tgtAclWCS, 1.0 / deltaTime);
         }
      }

      if (tgtStateValid)
      {
         mState.mTgtState.SetTime(aCurTime);
         mState.mTgtState.SetLocationWCS(tgtLocWCS);
         mState.mTgtState.SetVelocityWCS(tgtVelWCS);
         mState.mTgtState.SetAccelerationWCS(tgtAclWCS);
         UtVec3d::Set(mLastTgtLocWCS, tgtLocWCS);
         UtVec3d::Set(mLastTgtVelWCS, tgtVelWCS);
         mLastTgtUpdateTime = aCurTime;
         mTgtStateDefined   = true;
      }
      else
      {
         mLastTgtUpdateTime = cLARGE_POSITIVE_DOUBLE;
      }
   }
   else
   {
      mLastTgtUpdateTime = cLARGE_POSITIVE_DOUBLE;
   }
}

// ================================================================================================
//! Update the terrain following status.
//! @param aPhase The current phase.
// protected
void WsfGuidanceComputer::UpdateTerrainFollowing(const Phase& aPhase)
{
   // If commanded to an AGL altitude and terrain is enabled, tell the mover to not let
   // the platform fall below the terrain (crude terrain following).
   double minimumHeightAboveTerrain = 0.0;
   if ((aPhase.mCommandedAlt != cUNDEFINED_DOUBLE) && aPhase.mCommandedAltIsAGL)
   {
      minimumHeightAboveTerrain = 1.0;
   }
   mMoverPtr->SetMinimumHeightAboveTerrain(minimumHeightAboveTerrain);
}

// ================================================================================================
//! Update the perception of the weapon.
void WsfGuidanceComputer::UpdateWeaponPerception(double aCurTime)
{
   // Propagate the truth location and velocity to the current time.
   // (The truth values *MAY* be slightly out of date, so propagate it forward by 'dt.'.
   // In cases where called directly from UpdateGuidance, 'dt' should be zero.

   bool usingECI_Coords = (GetPlatform()->GetCoordinateFrame() == UtEntity::cCF_INERTIAL);

   double wpnLocWCS[3];
   double wpnVelWCS[3];
   double wpnAclWCS[3];
   double psi;
   double theta;
   double phi;
   GetPlatform()->GetLocationWCS(wpnLocWCS);
   GetPlatform()->GetVelocityWCS(wpnVelWCS);
   GetPlatform()->GetAccelerationWCS(wpnAclWCS);
   GetPlatform()->GetOrientationWCS(psi, theta, phi);

   double dt = aCurTime - mMoverPtr->GetLastUpdateTime();
   UtVec3d::AddProduct(wpnLocWCS, wpnLocWCS, dt, wpnVelWCS);
   UtVec3d::AddProduct(wpnLocWCS, wpnLocWCS, 0.5 * dt * dt, wpnAclWCS);
   UtVec3d::AddProduct(wpnVelWCS, wpnVelWCS, dt, wpnAclWCS);

   mState.mWpnState.SetTime(aCurTime);
   mState.mWpnState.SetLocationWCS(wpnLocWCS);
   mState.mWpnState.SetVelocityWCS(wpnVelWCS);
   mState.mWpnState.SetAccelerationWCS(wpnAclWCS);
   mState.mWpnState.SetOrientationWCS(psi, theta, phi);

   // Calculate the weapon heading and flight path angle in the local tangent plane.

   mLastFlightPathAngle = mState.mFlightPathAngle;
   double wpnVelNED[3];
   mState.mWpnState.GetVelocityNED(wpnVelNED);
   if (usingECI_Coords)
   {
      // If ECI use the orientation directly. The velocity has some initially large
      // lateral components that mess things up.
      double dummy;
      mState.mWpnState.GetOrientationNED(mState.mWpnHeading, mState.mFlightPathAngle, dummy);
   }
   else
   {
      double velNE            = sqrt(wpnVelNED[0] * wpnVelNED[0] + wpnVelNED[1] * wpnVelNED[1]);
      mState.mFlightPathAngle = atan2(-wpnVelNED[2], velNE);
      mState.mWpnHeading      = atan2(wpnVelNED[1], wpnVelNED[0]);
   }

   // Apply navigation errors if they exist. The velocity and acceleration vectors are copied from the
   // extrapolated frame to the perceived frame using NED coordinates rather than WCS coordinates so the
   // directions remain constant in the local tangent frame.

   double locErrorWCS[3];
   GetPlatform()->GetLocationErrorWCS(locErrorWCS);
   if (!UtVec3d::Equals(locErrorWCS, 0.0))
   {
      // Get NED vectors prior to moving the frame. Note that velocity was already retrieved above.
      double wpnAclNED[3];
      mState.mWpnState.GetAccelerationNED(wpnAclNED);

      UtVec3d::Add(wpnLocWCS, wpnLocWCS, locErrorWCS);
      mState.mWpnState.SetLocationWCS(wpnLocWCS);
      mState.mWpnState.SetVelocityNED(wpnVelNED);
      mState.mWpnState.SetAccelerationNED(wpnAclNED);
   }

   // The NED orientation from the host platform is simply copied to the perceived state.
   // (The orientation with respect to the local tangent plane remains constant.)
   //
   // TODO_JAJ needs to be evaluated... leaving it in to make sure nothing breaks in old cases.

   // TODO_JAJ if (!usingECI_Coords)
   {
      double heading;
      double pitch;
      double roll;
      GetPlatform()->GetOrientationNED(heading, pitch, roll);
      mState.mWpnState.SetOrientationNED(heading, pitch, roll);
   }
}

// ================================================================================================
// protected
bool WsfGuidanceComputer::ValidateNextPhaseName(const Phase& aPhase, WsfStringId aNextPhaseName)
{
   bool isValid = true;
   if ((aNextPhaseName != 0) && (FindPhase(aNextPhaseName) >= mPhaseList.size()))
   {
      isValid  = false;
      auto out = ut::log::error() << "Phase has a 'next_phase' command that refers to undefined phase.";
      out.AddNote() << "Phase: " << aPhase.mPhaseName;
      out.AddNote() << "Undefined Phase: " << aNextPhaseName;
   }
   return isValid;
}

// ================================================================================================
//                Definition of nested class 'Variable'.
// ================================================================================================

// ================================================================================================
//! Read a unitless value.
//! @param aInput The input stream.
//! @param aContext The script context for resolving a script variable.
void WsfGuidanceComputer::Variable::ReadValue(UtInput& aInput, WsfScriptContext& aContext)
{
   mValue = 0.0;
   mScale = 1.0;
   mVarNameId.Clear();
   std::string nameOrValue;
   aInput.ReadValue(nameOrValue);
   if ((nameOrValue == "/variable") || // New form consistent with WsfVariable
       (nameOrValue == "variable"))    // Old form consistent with original version
   {
      std::string name;
      aInput.ReadValue(name);
      if (!aContext.GetContext().IsVariableDefined(name))
      {
         throw UtInput::BadValue(aInput, "script variable " + name + " has not been defined");
      }
      mVarNameId = name;
   }
   else
   {
      aInput.PushBack(nameOrValue);
      aInput.ReadValue(mValue);
   }
}

// ================================================================================================
//! Read a value with units
//! @param aInput The input stream.
//! @param aValueType The unit type of the parameter.
//! @param aContext The script context for resolving a script variable.
void WsfGuidanceComputer::Variable::ReadValueOfType(UtInput& aInput, UtInput::ValueType aValueType, WsfScriptContext& aContext)
{
   mValue = 0.0;
   mScale = 1.0;
   mVarNameId.Clear();
   std::string units;
   std::string nameOrValue;
   aInput.ReadValue(nameOrValue);
   if ((nameOrValue == "/variable") || // New form consistent with WsfVariable
       (nameOrValue == "variable"))    // Old form consistent with original version
   {
      std::string name;
      aInput.ReadValue(name);
      mVarNameId = name;
      if (!aContext.GetContext().IsVariableDefined(name))
      {
         throw UtInput::BadValue(aInput, "script variable " + name + " has not been defined");
      }
      if (nameOrValue == "variable") // Original form requires units after name
      {
         aInput.ReadValue(units);
         mScale = aInput.ConvertValue(1.0, units, aValueType);
      }
   }
   else
   {
      aInput.PushBack(nameOrValue);
      aInput.ReadValueOfType(mValue, aValueType);
   }
}

// ================================================================================================
//! Update the value of the variable if it a script reference
//! @param aContext The script context for resolving a script variable.
void WsfGuidanceComputer::Variable::Update(WsfScriptContext& aContext)
{
   if (!mVarNameId.IsNull())
   {
      const UtScriptData* varDataPtr = aContext.GetContext().FindVar(mVarNameId);
      if (varDataPtr != nullptr)
      {
         mValue = varDataPtr->GetDouble() * mScale;
      }
   }
}

// ================================================================================================
//! Interface to UtInput::ValueInClosedRange.
void WsfGuidanceComputer::Variable::ValueInClosedRange(UtInput& aInput, double aMinValue, double aMaxValue)
{
   if ((!IsReference()) || HasDefaultValue())
   {
      aInput.ValueInClosedRange(mValue, aMinValue, aMaxValue);
   }
}

// ================================================================================================
//! Interface to UtInput::ValueGreater.
void WsfGuidanceComputer::Variable::ValueGreater(UtInput& aInput, double aMinValue)
{
   if ((!IsReference()) || HasDefaultValue())
   {
      aInput.ValueGreater(mValue, aMinValue);
   }
}

// ================================================================================================
//! Interface to UtInput::ValueGreaterOrEqual.
void WsfGuidanceComputer::Variable::ValueGreaterOrEqual(UtInput& aInput, double aMinValue)
{
   if ((!IsReference()) || HasDefaultValue())
   {
      aInput.ValueGreaterOrEqual(mValue, aMinValue);
   }
}

// ================================================================================================
//! Interface to UtInput::ValueLess.
void WsfGuidanceComputer::Variable::ValueLess(UtInput& aInput, double aMaxValue)
{
   if ((!IsReference()) || HasDefaultValue())
   {
      aInput.ValueLess(mValue, aMaxValue);
   }
}

// ================================================================================================
//! Interface to UtInput::ValueLessOrEqual.
void WsfGuidanceComputer::Variable::ValueLessOrEqual(UtInput& aInput, double aMaxValue)
{
   if ((!IsReference()) || HasDefaultValue())
   {
      aInput.ValueLessOrEqual(mValue, aMaxValue);
   }
}

// ================================================================================================
//                Definition of nested class 'Phase'.
// ================================================================================================

WsfGuidanceComputer::Phase::Phase()
   : mOnUpdateScriptPtr(nullptr)
   , mPhaseName(nullptr)
   , mGuidanceDelay(0.0)
   , mTimeConstant(0.0)
   , mCommandedAlt(cUNDEFINED_DOUBLE)
   , mCommandedAzimuthOffset(cUNDEFINED_DOUBLE)
   , mCommandedFlightPathAngle(cUNDEFINED_DOUBLE)
   , mCommandedSpeed(cUNDEFINED_DOUBLE)
   , mCommandedThrottle(-1.0)
   , mAimpointAltitudeOffset(0.0)
   , mAimpointAzimuthOffset(0.0)
   , mAimpointRangeOffset(0.0)
   , mAimpointEvaluationInterval(5.0)
   , mCosLOS_Offset(0.866)
   , // acos(30.0 degrees)
   mPN_GainValue(3.0)
   , mVP_GainValue(10.0)
   , mGeeBiasValue(1.0)
   , mLateralGeeBiasValue(0.0)
   , mMaxGeeCmd(25.0 * UtEarth::cACCEL_OF_GRAVITY)
   , mMaxPitchAngle(70.0 * UtMath::cRAD_PER_DEG)
   , mMaxAscentRate(0.0)
   , mMaxDescentRate(0.0)
   , mPitchChangeGain(1.0)
   , mGuidanceTarget(cGT_DEFAULT)
   , mOffsetDirection(cOD_EITHER)
   , mPN_Method(cPN_PURE)
   , mCommandedAltIsAGL(false)
   , mAllowRouteFollowing(false)
   , mPrograms()
   , mEndOfRoutePhaseName()
   , mBoostCompletePhaseName()
   , mStageIgnitionPhaseName()
   , mStageBurnoutPhaseName()
   , mStageSeparationPhaseName()
   , mSensorTrackInitiatedPhaseName()
   , mProgramCompletePhaseName()
   , mPhaseChangeList()
{
}

// ================================================================================================
void WsfGuidanceComputer::Phase::ExecuteOnEntry(double aSimTime, WsfScriptContext& aContext)
{
   UtScript* scriptPtr = aContext.FindScript(mPhaseName.GetString() + "::on_entry");
   if (scriptPtr != nullptr)
   {
      aContext.ExecuteScript(aSimTime, scriptPtr);
   }
}

// ================================================================================================
void WsfGuidanceComputer::Phase::ExecuteOnExit(double aSimTime, WsfScriptContext& aContext)
{
   UtScript* scriptPtr = aContext.FindScript(mPhaseName.GetString() + "::on_exit");
   if (scriptPtr != nullptr)
   {
      aContext.ExecuteScript(aSimTime, scriptPtr);
   }
}

// ================================================================================================
void WsfGuidanceComputer::Phase::ExecuteOnUpdate(double aSimTime, WsfScriptContext& aContext)
{
   if (mOnUpdateScriptPtr != nullptr)
   {
      aContext.ExecuteScript(aSimTime, mOnUpdateScriptPtr);
   }
}

// ================================================================================================
bool WsfGuidanceComputer::Phase::Initialize(double aSimTime, WsfScriptContext& aContext)
{
   bool ok(true);

   // If it exists, capture the pointer to the "on_update" method for this phase.
   mOnUpdateScriptPtr = aContext.FindScript(mPhaseName.GetString() + "::on_update");

   // Make sure aimpoint guidance commands are consistent.
   if (((mAimpointAzimuthOffset == 0.0) && (mAimpointRangeOffset != 0.0)) ||
       ((mAimpointAzimuthOffset != 0.0) && (mAimpointRangeOffset == 0.0)))
   {
      ut::log::error()
         << "'aimpoint_azimuth_offset' and 'aimpoint_range_offset' must both be specified if either is specified.";
      ok = false;
   }
   return ok;
}

// ================================================================================================
bool WsfGuidanceComputer::Phase::ProcessInput(UtInput& aInput, WsfGuidanceComputer& aComputer, WsfScriptContext& aContext)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "guidance_delay")
   {
      aInput.ReadValueOfType(mGuidanceDelay, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mGuidanceDelay, 0.0);
   }
   else if (command == "time_constant") // NO_DOC | IN_DEVELOPMENT
   {
      aInput.ReadValueOfType(mTimeConstant, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mTimeConstant, 0.0);
   }
   else if (command == "guidance_target")
   {
      std::string guideTo;
      aInput.ReadValue(guideTo);
      if (guideTo == "truth")
      {
         mGuidanceTarget = cGT_TRUTH;
      }
      else if (guideTo == "perception")
      {
         mGuidanceTarget = cGT_PERCEPTION;
      }
      else if (guideTo == "predicted_intercept")
      {
         mGuidanceTarget = cGT_PREDICTED_INTERCEPT;
      }
      else if (guideTo == "default")
      {
         mGuidanceTarget = cGT_DEFAULT;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown value for 'guidance_target'");
      }
   }
   else if (command == "guide_to_truth")
   {
      bool guideToTruth;
      aInput.ReadValue(guideToTruth);
      mGuidanceTarget = guideToTruth ? cGT_TRUTH : cGT_PERCEPTION;
   }
   else if (command == "allow_route_following")
   {
      aInput.ReadValue(mAllowRouteFollowing);
   }
   else if (command == "aimpoint_altitude_offset")
   {
      aInput.ReadValueOfType(mAimpointAltitudeOffset, UtInput::cLENGTH);
   }
   else if (command == "aimpoint_azimuth_offset")
   {
      std::string direction;
      aInput.ReadValueOfType(mAimpointAzimuthOffset, UtInput::cANGLE);
      aInput.ValueInClosedRange(mAimpointAzimuthOffset, 0.0, UtMath::cPI);
      aInput.ReadValue(direction);
      if (direction == "either")
      {
         mOffsetDirection = cOD_EITHER;
      }
      else if (direction == "left")
      {
         mOffsetDirection = cOD_LEFT;
      }
      else if (direction == "right")
      {
         mOffsetDirection = cOD_RIGHT;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown direction for aimpoint_azimuth_offset");
      }
   }
   else if (command == "aimpoint_range_offset")
   {
      aInput.ReadValueOfType(mAimpointRangeOffset, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mAimpointRangeOffset, 0.0);
   }
   else if (command == "aimpoint_evaluation_interval")
   {
      aInput.ReadValueOfType(mAimpointEvaluationInterval, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mAimpointEvaluationInterval, 0.0);
   }
   else if (command == "proportional_navigation_gain")
   {
      aInput.ReadValue(mPN_GainValue);
      aInput.ValueGreaterOrEqual(mPN_GainValue, 0.0); // Allow 0 to cancel
   }
   else if (command == "proportional_navigation_method")
   {
      std::string method;
      aInput.ReadValue(method);
      if (method == "pure")
      {
         mPN_Method = cPN_PURE;
      }
      else if (method == "augmented")
      {
         mPN_Method = cPN_AUGMENTED;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if ((command == "proportional_navigation_limit_angle") || (command == "aspect_angle_for_pro_nav_switch"))
   {
      double angle;
      aInput.ReadValueOfType(angle, UtInput::cANGLE);
      aInput.ValueInClosedRange(angle, 0.0, UtMath::cPI_OVER_2);
      mCosLOS_Offset = cos(angle);
   }
   else if (command == "velocity_pursuit_gain")
   {
      aInput.ReadValue(mVP_GainValue);
      aInput.ValueGreaterOrEqual(mVP_GainValue, 0.0); // Allow 0 to cancel
   }
   else if (command == "g_bias")
   {
      // Positive and negative values are allowed here. A value of zero indicates no g-bias.
      // Negative values are useful when trying to intercept incoming ballistic targets.
      aInput.ReadValue(mGeeBiasValue);
   }
   else if (command == "lateral_g_bias")
   {
      aInput.ReadValue(mLateralGeeBiasValue);
   }
   else if (command == "commanded_altitude")
   {
      std::string altRef;
      aInput.ReadValueOfType(mCommandedAlt, UtInput::cLENGTH);
      // Check for optional 'msl' or 'agl' altitude reference ('msl' assumed if not specified).
      aInput.ReadValue(altRef);
      mCommandedAltIsAGL = false; // Assume 'msl'
      if (altRef == "agl")
      {
         mCommandedAltIsAGL = true;
      }
      else if (altRef != "msl")
      {
         aInput.PushBack(altRef);
      }
   }
   else if (command == "commanded_azimuth_offset")
   {
      aInput.ReadValueOfType(mCommandedAzimuthOffset, UtInput::cANGLE);
      aInput.ValueInClosedRange(mCommandedAzimuthOffset, 0.0, UtMath::cPI);
   }
   else if (command == "commanded_flight_path_angle")
   {
      std::string option;
      aInput.ReadValue(option);
      if (option == "from_launch_computer")
      {
         mCommandedFlightPathAngle = cFROM_LAUNCH_COMPUTER;
      }
      else
      {
         aInput.PushBack(option);
         aInput.ReadValueOfType(mCommandedFlightPathAngle, UtInput::cANGLE);
         aInput.ValueInClosedRange(mCommandedFlightPathAngle, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      }
   }
   else if (command == "commanded_mach")
   {
      double commandedMach;
      aInput.ReadValue(commandedMach);
      aInput.ValueGreaterOrEqual(commandedMach, 0.0);
      mCommandedSpeed = -commandedMach;
   }
   else if (command == "commanded_speed")
   {
      aInput.ReadValueOfType(mCommandedSpeed, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mCommandedSpeed, 0.0);
   }
   else if (command == "commanded_throttle")
   {
      aInput.ReadValue(mCommandedThrottle);
      aInput.ValueInClosedRange(mCommandedThrottle, 0.0, 1.0);
   }
   else if (command == "maximum_commanded_g")
   {
      aInput.ReadValueOfType(mMaxGeeCmd, UtInput::cACCELERATION);
      aInput.ValueGreater(mMaxGeeCmd, 0.0);
   }
   else if (command == "maximum_ascent_rate")
   {
      aInput.ReadValueOfType(mMaxAscentRate, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mMaxAscentRate, 0.0); // 0 = no constraint
   }
   else if (command == "maximum_descent_rate")
   {
      aInput.ReadValueOfType(mMaxDescentRate, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mMaxDescentRate, 0.0); // 0 = no constraint
   }
   else if (command == "maximum_pitch_angle")
   {
      aInput.ReadValueOfType(mMaxPitchAngle, UtInput::cANGLE);
      aInput.ValueGreater(mMaxPitchAngle, 0.0);
   }
   else if (command == "pitch_change_gain")
   {
      aInput.ReadValue(mPitchChangeGain);
      aInput.ValueGreater(mPitchChangeGain, 0.0);
   }
   else if (command == "program")
   {
      std::string typeStr;
      aInput.ReadValue(typeStr);
      UtInputBlock                        inputBlock(aInput, "end_program");
      std::unique_ptr<WsfGuidanceProgram> programPtr(
         WsfGuidanceProgramTypes::Get(aComputer.GetScenario()).CreateInstance(typeStr));
      if (programPtr == nullptr)
      {
         throw UtInput::BadValue(aInput, "Unknown guidance program type: " + typeStr);
      }
      std::ostringstream oss;
      // Generate a name for the program that couldn't be entered manually: '#<type-name> <phase-name> <number>'
      // Note: Don't change this generated name as it is checked for in 'next_phase' commands.
      oss << '#' << typeStr << ' ' << mPhaseName << ' ' << mPrograms.size() + 1;
      programPtr->SetName(oss.str());
      while (inputBlock.ReadCommand())
      {
         if (!programPtr->ProcessInput(inputBlock.GetInput()))
         {
            throw UtInput::UnknownCommand(inputBlock.GetInput());
         }
      }
      mPrograms.push_back(programPtr->GetNameId());
      aComputer.mDefinedPrograms.push_back(programPtr.release());
   }
   else if (command == "use_program")
   {
      std::string nameStr;
      aInput.ReadValue(nameStr);
      WsfStringId nameId = nameStr;
      if (std::find(mPrograms.begin(), mPrograms.end(), nameId) != mPrograms.end())
      {
         throw UtInput::BadValue(aInput, "Duplicate guidance program reference: " + nameStr);
      }
      mPrograms.push_back(nameId);
   }
   else if (command == "next_phase")
   {
      ProcessNextPhase(aInput, aContext);
   }
   else if (command == "on_entry")
   {
      aContext.Compile(command, "void", aInput, "end_on_entry", mPhaseName);
   }
   else if (command == "on_exit")
   {
      aContext.Compile(command, "void", aInput, "end_on_exit", mPhaseName);
   }
   else if (command == "on_update")
   {
      aContext.Compile(command, "void", aInput, "end_on_update", mPhaseName);
   }
   // The following are accepted to allow trivial conversion of WSF_OLD_GUIDANCE_COMPUTER -> WSF_GUIDANCE_COMPUTER
   else if (command == "IMU_drift_rate") // NO_DOC | deprecated
   {
      double notUsed;
      aInput.ReadValueOfType(notUsed, UtInput::cSPEED);
   }
   else if (command == "time_between_GPS_fixes") // NO_DOC | deprecated
   {
      double notUsed;
      aInput.ReadValueOfType(notUsed, UtInput::cTIME);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// ================================================================================================
void WsfGuidanceComputer::Phase::ProcessNextPhase(UtInput& aInput, WsfScriptContext& aContext)
{
   // Read the phase name.
   std::string nextPhaseNameStr;
   aInput.ReadValue(nextPhaseNameStr);
   WsfStringId nextPhaseName = nextPhaseNameStr;

   std::string ifOrWhen;
   aInput.ReadValue(ifOrWhen);
   if ((ifOrWhen != "if") && (ifOrWhen != "when"))
   {
      throw UtInput::BadValue(aInput, "'if' or 'when' must follow phase name");
   }

   std::string criteria;
   aInput.ReadValue(criteria);

   if (criteria == "end_of_route")
   {
      mEndOfRoutePhaseName = nextPhaseName;
   }
   else if (criteria == "boost_complete")
   {
      mBoostCompletePhaseName = nextPhaseName;
   }
   else if (criteria == "stage_ignition")
   {
      mStageIgnitionPhaseName = nextPhaseName;
   }
   else if (criteria == "stage_burnout")
   {
      mStageBurnoutPhaseName = nextPhaseName;
   }
   else if (criteria == "stage_separation")
   {
      mStageSeparationPhaseName = nextPhaseName;
   }
   else if (criteria == "sensor_track_initiated")
   {
      mSensorTrackInitiatedPhaseName = nextPhaseName;
   }
   else if (criteria == "program")
   {
      std::string programName;
      aInput.ReadValue(programName);
      std::string complete;
      aInput.ReadValue(complete);
      aInput.StringEqual(complete, "complete");
      mProgramCompletePhaseName.emplace_back(programName, nextPhaseName);
   }
   else
   {
      // All of the following create a PhaseChange list entry.
      PhaseChange phaseChange(nextPhaseName); // Create the skeleton entry

      if (criteria == "phase_time")
      {
         phaseChange.mName = PhaseChange::cPHASE_TIME;
         phaseChange.ReadCondition(aInput);
         if ((phaseChange.mCondition != PhaseChange::cGREATER) && (phaseChange.mCondition != PhaseChange::cGREATER_OR_EQUAL))
         {
            throw UtInput::BadValue(aInput, "'phase_time' test can only be '>' or '>='");
         }
         phaseChange.mValue.ReadValueOfType(aInput, UtInput::cTIME, aContext);
         phaseChange.mValue.ValueGreater(aInput, 0.0);
      }
      else if (criteria == "flight_time")
      {
         phaseChange.mName = PhaseChange::cFLIGHT_TIME;
         phaseChange.ReadCondition(aInput);
         if ((phaseChange.mCondition != PhaseChange::cGREATER) && (phaseChange.mCondition != PhaseChange::cGREATER_OR_EQUAL))
         {
            throw UtInput::BadValue(aInput, "'flight_time' test can only be '>' or '>='");
         }
         phaseChange.mValue.ReadValueOfType(aInput, UtInput::cTIME, aContext);
         phaseChange.mValue.ValueGreater(aInput, 0.0);
      }
      else if (criteria == "altitude")
      {
         phaseChange.mName = PhaseChange::cALTITUDE;
         phaseChange.ReadCondition(aInput);
         phaseChange.mValue.ReadValueOfType(aInput, UtInput::cLENGTH, aContext);
      }
      else if (criteria == "speed")
      {
         phaseChange.mName = PhaseChange::cSPEED;
         phaseChange.ReadCondition(aInput);
         phaseChange.mValue.ReadValueOfType(aInput, UtInput::cSPEED, aContext);
         phaseChange.mValue.ValueGreaterOrEqual(aInput, 0.0);
      }
      else if (criteria == "vertical_speed")
      {
         phaseChange.mName = PhaseChange::cVERTICAL_SPEED;
         phaseChange.ReadCondition(aInput);
         phaseChange.mValue.ReadValueOfType(aInput, UtInput::cSPEED, aContext);
         phaseChange.mValue.ValueGreaterOrEqual(aInput, 0.0);
      }
      else if (criteria == "flight_path_angle")
      {
         phaseChange.mName = PhaseChange::cFLIGHT_PATH_ANGLE;
         phaseChange.ReadCondition(aInput);
         phaseChange.mValue.ReadValueOfType(aInput, UtInput::cANGLE, aContext);
         phaseChange.mValue.ValueGreater(aInput, -UtMath::cPI_OVER_2);
         phaseChange.mValue.ValueLess(aInput, UtMath::cPI_OVER_2);
      }
      else if (criteria == "on_commanded_flight_path_angle")
      {
         phaseChange.mName = PhaseChange::cON_COMMANDED_FLIGHT_PATH_ANGLE;
      }
      else if (criteria == "dynamic_pressure")
      {
         phaseChange.mName = PhaseChange::cDYNAMIC_PRESSURE;
         phaseChange.ReadCondition(aInput);
         phaseChange.mValue.ReadValueOfType(aInput, UtInput::cPRESSURE, aContext);
         phaseChange.mValue.ValueGreater(aInput, 0.0);
      }
      else if (criteria == "target_altitude")
      {
         phaseChange.mName = PhaseChange::cTARGET_ALTITUDE;
         phaseChange.ReadCondition(aInput);
         phaseChange.mValue.ReadValueOfType(aInput, UtInput::cLENGTH, aContext);
      }
      else if (criteria == "target_speed")
      {
         phaseChange.mName = PhaseChange::cTARGET_SPEED;
         phaseChange.ReadCondition(aInput);
         phaseChange.mValue.ReadValueOfType(aInput, UtInput::cSPEED, aContext);
         phaseChange.mValue.ValueGreaterOrEqual(aInput, 0.0);
      }
      else if (criteria == "target_flight_path_angle")
      {
         phaseChange.mName = PhaseChange::cTARGET_FLIGHT_PATH_ANGLE;
         phaseChange.ReadCondition(aInput);
         phaseChange.mValue.ReadValueOfType(aInput, UtInput::cANGLE, aContext);
         phaseChange.mValue.ValueGreater(aInput, -UtMath::cPI_OVER_2);
         phaseChange.mValue.ValueLess(aInput, UtMath::cPI_OVER_2);
      }
      else if (criteria == "closing_speed")
      {
         phaseChange.mName = PhaseChange::cCLOSING_SPEED;
         phaseChange.ReadCondition(aInput);
         phaseChange.mValue.ReadValueOfType(aInput, UtInput::cSPEED, aContext);
         phaseChange.mValue.ValueGreaterOrEqual(aInput, 0.0);
      }
      else if (criteria == "time_to_intercept")
      {
         phaseChange.mName = PhaseChange::cTIME_TO_INTERCEPT;
         phaseChange.ReadCondition(aInput);
         phaseChange.mValue.ReadValueOfType(aInput, UtInput::cTIME, aContext);
         phaseChange.mValue.ValueGreater(aInput, 0.0);
      }
      else if (criteria == "range_to_intercept")
      {
         phaseChange.mName = PhaseChange::cRANGE_TO_INTERCEPT;
         phaseChange.ReadCondition(aInput);
         phaseChange.mValue.ReadValueOfType(aInput, UtInput::cLENGTH, aContext);
         phaseChange.mValue.ValueGreater(aInput, 0.0);
      }
      else if (criteria == "target_slant_range")
      {
         phaseChange.mName = PhaseChange::cTARGET_SLANT_RANGE;
         phaseChange.ReadCondition(aInput);
         phaseChange.mValue.ReadValueOfType(aInput, UtInput::cLENGTH, aContext);
         phaseChange.mValue.ValueGreater(aInput, 0.0);
      }
      else if (criteria == "target_ground_range")
      {
         phaseChange.mName = PhaseChange::cTARGET_GROUND_RANGE;
         phaseChange.ReadCondition(aInput);
         phaseChange.mValue.ReadValueOfType(aInput, UtInput::cLENGTH, aContext);
         phaseChange.mValue.ValueGreater(aInput, 0.0);
      }
      else if (criteria == "target_elevation")
      {
         phaseChange.mName = PhaseChange::cTARGET_ELEVATION;
         phaseChange.ReadCondition(aInput);
         phaseChange.mValue.ReadValueOfType(aInput, UtInput::cANGLE, aContext);
         phaseChange.mValue.ValueGreater(aInput, -UtMath::cPI_OVER_2);
         phaseChange.mValue.ValueLess(aInput, UtMath::cPI_OVER_2);
      }
      else if (criteria == "target_azimuth")
      {
         phaseChange.mName = PhaseChange::cTARGET_AZIMUTH;
         phaseChange.ReadCondition(aInput);
         phaseChange.mValue.ReadValueOfType(aInput, UtInput::cANGLE, aContext);
         phaseChange.mValue.ValueGreater(aInput, -UtMath::cPI);
         phaseChange.mValue.ValueLess(aInput, UtMath::cPI);
      }
      else if (criteria == "los_target_elevation")
      {
         phaseChange.mName = PhaseChange::cLOS_TARGET_ELEVATION;
         phaseChange.ReadCondition(aInput);
         phaseChange.mValue.ReadValueOfType(aInput, UtInput::cANGLE, aContext);
         phaseChange.mValue.ValueGreater(aInput, -UtMath::cPI_OVER_2);
         phaseChange.mValue.ValueLess(aInput, UtMath::cPI_OVER_2);
      }
      else if (criteria == "los_target_azimuth")
      {
         phaseChange.mName = PhaseChange::cLOS_TARGET_AZIMUTH;
         phaseChange.ReadCondition(aInput);
         phaseChange.mValue.ReadValueOfType(aInput, UtInput::cANGLE, aContext);
         phaseChange.mValue.ValueGreater(aInput, -UtMath::cPI);
         phaseChange.mValue.ValueLess(aInput, UtMath::cPI);
      }
      else if (criteria == "los_target_angle")
      {
         phaseChange.mName = PhaseChange::cLOS_TARGET_ANGLE;
         phaseChange.ReadCondition(aInput);
         phaseChange.mValue.ReadValueOfType(aInput, UtInput::cANGLE, aContext);
         phaseChange.mValue.ValueGreater(aInput, 0.0);
         phaseChange.mValue.ValueLess(aInput, UtMath::cPI);
      }
      else
      {
         throw UtInput::BadValue(aInput, "Invalid criteria for 'next_phase' command: " + criteria);
      }
      mPhaseChangeList.push_back(phaseChange);
   }
}

// ================================================================================================
bool WsfGuidanceComputer::PhaseChange::ConditionSatisfied(double aValue) const
{
   bool satisfied = false;
   switch (mCondition)
   {
   case cLESS:
      satisfied = (aValue < mValue);
      break;
   case cLESS_OR_EQUAL:
      satisfied = (aValue <= mValue);
      break;
   case cEQUAL:
      satisfied = (aValue == mValue);
      break;
   case cNOT_EQUAL:
      satisfied = (aValue != mValue);
      break;
   case cGREATER_OR_EQUAL:
      satisfied = (aValue >= mValue);
      break;
   case cGREATER:
      satisfied = (aValue > mValue);
      break;
   default:
      break; // should not happen... just return false
   }
   return satisfied;
}

// ================================================================================================
void WsfGuidanceComputer::PhaseChange::ReadCondition(UtInput& aInput)
{
   Condition   condition = cEQUAL;
   std::string conditionName;
   aInput.ReadValue(conditionName);
   if (conditionName == "<")
   {
      condition = cLESS;
   }
   else if (conditionName == "<=")
   {
      condition = cLESS_OR_EQUAL;
   }
   else if (conditionName == "==")
   {
      condition = cEQUAL;
   }
   else if (conditionName == "!=")
   {
      condition = cNOT_EQUAL;
   }
   else if (conditionName == ">=")
   {
      condition = cGREATER_OR_EQUAL;
   }
   else if (conditionName == ">")
   {
      condition = cGREATER;
   }
   else
   {
      throw UtInput::BadValue(aInput, "Unknown relational condition: " + conditionName);
   }
   mCondition = condition;
}

// ================================================================================================
//! Callback to indicate that a sensor track has begun.
//! @param aSimTime Simulation time.
//! @param aSensorPtr the sensor producing the track.
//! @param aTrackPtr the Track produced by the sensor.
void WsfGuidanceComputer::SensorTrackInitiated(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr)
{
   if (GetPlatform() == aSensorPtr->GetPlatform())
   {
      SelectPhase(aSimTime, aSimTime, mPhaseList[mPhaseIndex].mSensorTrackInitiatedPhaseName);
   }
}
