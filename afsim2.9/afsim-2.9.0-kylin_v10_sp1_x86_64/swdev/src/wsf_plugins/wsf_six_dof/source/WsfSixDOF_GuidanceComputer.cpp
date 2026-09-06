// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSixDOF_GuidanceComputer.hpp"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <iomanip>
#include <memory>
#include <sstream>

#include "UtEarth.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLatPos.hpp"
#include "UtLog.hpp"
#include "UtLonPos.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtScriptClass.hpp"
#include "UtVec3.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfGuidanceProgram.hpp"
#include "WsfGuidedMover.hpp"
#include "WsfIntercept.hpp"
#include "WsfLaunchComputer.hpp"
#include "WsfMover.hpp"
#include "WsfPath.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackObserver.hpp"
#include "WsfWeaponObserver.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptProcessorClass.hpp"

wsf::six_dof::GuidanceComputer::GuidanceComputer(WsfScenario& aScenario)
   : WsfGuidanceComputer(aScenario)
{
}

wsf::six_dof::GuidanceComputer::GuidanceComputer(const GuidanceComputer& aSrc)
   : WsfGuidanceComputer(aSrc)
{
}

WsfProcessor* wsf::six_dof::GuidanceComputer::Clone() const
{
   return new GuidanceComputer(*this);
}

bool wsf::six_dof::GuidanceComputer::Initialize(double aSimTime)
{
   bool ok = true;
   // Can't do anything unless at least one phase has been defined.
   if (mPhaseList.empty())
   {
      ut::log::error() << "No phases have beed defined.";
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
      auto out = ut::log::error() << "Invalid 'initial_phase' name.";
      out.AddNote() << "Name: " << mInitialPhaseName;
      ok = false;
   }

   mVehicleMoverPtr = dynamic_cast<Mover*>(GetPlatform()->GetMover());
   if (mVehicleMoverPtr == nullptr)
   {
      ut::log::warning() << "Mover is of the incorrect type; turning guidance off. A SixDOF Mover is required.";
      SetInitiallyTurnedOn(false);
   }
   else
   {
      mVehicleMoverPtr->SetGuidance(this);
      mCallbacks.Add(mVehicleMoverPtr->RouteChanged.Connect(&GuidanceComputer::RouteChanged, this));
      mCallbacks.Add(mVehicleMoverPtr->StageIgnition.Connect(&GuidanceComputer::StageIgnition, this));
      mCallbacks.Add(mVehicleMoverPtr->StageBurnout.Connect(&GuidanceComputer::StageBurnout, this));
      mCallbacks.Add(mVehicleMoverPtr->StageSeparation.Connect(&GuidanceComputer::StageSeparation, this));

      mCallbacks.Add(WsfObserver::SensorTrackInitiated(GetSimulation())
                        .Connect(&WsfGuidanceComputer::SensorTrackInitiated, static_cast<WsfGuidanceComputer*>(this)));
   }

   // Validate the existence of every phase specified in a next_phase command, and validate the program names in the
   // use_program commands.
   size_t phaseListSize = mPhaseList.size();
   for (size_t pi = 0; pi != phaseListSize; ++pi)
   {
      const Phase& phase = mPhaseList[pi];
      ok &= ValidateNextPhaseName(phase, phase.mEndOfRoutePhaseName);
      ok &= ValidateNextPhaseName(phase, phase.mStageIgnitionPhaseName);
      ok &= ValidateNextPhaseName(phase, phase.mStageBurnoutPhaseName);
      ok &= ValidateNextPhaseName(phase, phase.mStageSeparationPhaseName);
      ok &= ValidateNextPhaseName(phase, phase.mSensorTrackInitiatedPhaseName);
      for (size_t pci = 0; pci != phase.mPhaseChangeList.size(); ++pci)
      {
         ok &= ValidateNextPhaseName(phase, phase.mPhaseChangeList[pci].mNextPhaseName);
      }

      for (std::vector<WsfStringId>::const_iterator spi = phase.mPrograms.begin(); spi != phase.mPrograms.end(); ++spi)
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
   if (!ok)
   {
      return ok;
   }

   if (mLegacyProgramPtr == nullptr)
   {
      mLegacyProgramPtr = WsfGuidanceProgramTypes::Get(GetScenario()).CreateInstance("_LEGACY_PROGRAM_");
   }
   if (mLegacyProgramPtr != nullptr)
   {
      ok &= mLegacyProgramPtr->Initialize(aSimTime, this);
   }

   for (auto& pli : mDefinedPrograms)
   {
      WsfGuidanceProgram& program = *pli;
      if (!program.Initialize(aSimTime, this))
      {
         auto out = ut::log::error() << "Failed to initialize program.";
         out.AddNote() << "Program: " << program.GetName();
         ok = false;
      }
   }

   // If debug is enabled then show_status and show_evaluations are also enabled.
   mShowStatus |= DebugEnabled();
   mShowEvaluations |= DebugEnabled();

   mTerrainPtr = new wsf::Terrain(GetSimulation()->GetTerrainInterface());

   GetPlatform()->GetLocationWCS(mLaunchLocWCS);
   mLaunchTime     = aSimTime;
   mLastUpdateTime = -1.0;

   if (mShowGraphics)
   {
      mDrawPtr        = new WsfDraw(*GetSimulation());
      mAimpointDrawId = mDrawPtr->GetNewId();
   }
   return ok;
}

//! Perform a guidance update.
//! This is called from the mover when a guidance update is desired. It computes the desired values
//! and pushes them back in to the mover.
//! @param aSimTime The current simulation time.
//! @param aCurTime The current internal time within the mover. This can be less than aSimTime.
//! @param aEndTime The internal time to which the mover is trying to advance. This is typically
//!                 one mover integration step beyond aCurTime
// virtual
void wsf::six_dof::GuidanceComputer::UpdateGuidance(double aSimTime, double aCurTime, double aEndTime)
{
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "GuidanceComputer: UpdateGuidance Debug Dump:" << std::fixed << std::setprecision(5);
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Since Launch: " << aCurTime - mLaunchTime << " sec";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "End Time: " << aEndTime;
      out.AddNote() << "Flight Time: " << aEndTime - mLaunchTime;
   }

   // Some movers will call us even when we're not turned on!
   if (!IsTurnedOn())
   {
      mLastUpdateTime = aCurTime;
      //    mVehicleMoverPtr->SetYandZForces(0.0, 0.0);
      mVehicleMoverPtr->SetYandZGLoads(0.0, 0.0);
      mVehicleMoverPtr->SetCommandedSpeed(-1.0);

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

      // Update the programmed attitude control
      mVehicleMoverPtr->SetCommandedYawRate(cmds.mAngleRateCmd[2]);
      mVehicleMoverPtr->SetCommandedPitchRate(cmds.mAngleRateCmd[1]);
      mVehicleMoverPtr->SetCommandedRollRate(cmds.mAngleRateCmd[0]);

      // Apply acceleration limits, convert accelerations to forces and push to the mover
      yAccel = cmds.mAccelCmd[1];
      zAccel = cmds.mAccelCmd[2];

      yAccel = UtMath::Limit(yAccel, static_cast<double>(phase.mMaxGeeCmd));
      zAccel = UtMath::Limit(zAccel, static_cast<double>(phase.mMaxGeeCmd));

      // The guidance above is taken from "standard" AFSIM guidance, which is for 3DOF movers.
      // The SixDOF mover includes pitch and roll in addition to the heading (yaw) of the 3DOF mover.
      // As a result, we need to correct the pitch and yaw commands (just calculated above)
      // to work correctly for the more realistic SixDOF mover.

      // Now determine loading in Y and Z
      mVehicleMoverPtr->SetYandZGLoads(yAccel / UtEarth::cACCEL_OF_GRAVITY, zAccel / UtEarth::cACCEL_OF_GRAVITY);

      // Update command speed/mach if requested.
      if (phase.mCommandedSpeed != cUNDEFINED_DOUBLE)
      {
         if (phase.mCommandedSpeed >= 0.0)
         {
            mVehicleMoverPtr->SetCommandedSpeed(phase.mCommandedSpeed);
         }
         else
         {
            mVehicleMoverPtr->SetCommandedSpeed(-phase.mCommandedSpeed *
                                                mAtmosphere.SonicVelocity(GetPlatform()->GetAltitude()));
         }
      }
      else
      {
         mVehicleMoverPtr->SetCommandedSpeed(-1.0);
      }
   }
   else
   {
      // Guidance is disabled... no commands...
      mVehicleMoverPtr->SetYandZGLoads(0.0, 0.0);
      mVehicleMoverPtr->SetCommandedSpeed(-1.0);
   }

   // Go ballistic without a target!!!
   if (!mTgtStateDefined)
   {
      // Guidance is disabled... no commands...
      mVehicleMoverPtr->SetYandZGLoads(0.0, 0.0);
      mVehicleMoverPtr->SetCommandedSpeed(-1.0);
   }

   mLastUpdateTime = aCurTime;

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Updated SixDOF guidance computer." << std::fixed << std::setprecision(5);
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Since Launch: " << aCurTime - mLaunchTime << " sec";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Ay: " << yAccel / UtEarth::cACCEL_OF_GRAVITY << " g";
      out.AddNote() << "Az: " << zAccel / UtEarth::cACCEL_OF_GRAVITY << " g";
      out.AddNote() << "A: " << std::hypot(yAccel, zAccel) / UtEarth::cACCEL_OF_GRAVITY << " g";
      out << std::setprecision(6);
      out.AddNote() << "End Time: " << aEndTime;
      out.AddNote() << "Flight Time: " << aEndTime - mLaunchTime;
   }
}

void wsf::six_dof::GuidanceComputer::TurnOn(double aSimTime)
{
   WsfScriptProcessor::TurnOn(aSimTime); // Let the base class do its thing

   mVehicleMoverPtr->SetGuidance(this); // Indicate this is performing the guidance
   mLastUpdateTime = aSimTime - 1.0;
   UpdateWeaponPerception(aSimTime);
   UpdateTargetPerception(aSimTime);

   // Select the initial guidance phase. This will default to the first phase if none was specified
   mPhaseIndex = static_cast<unsigned int>(mPhaseList.size()); // Indicate no phase has been selected
   SelectPhase(aSimTime, aSimTime, mInitialPhaseName);

   // This should never happen as the initial_phase should have been checked in Initialize and
   // the scriptable SelectPhase method, but we do this just in case.
   if (mPhaseIndex >= static_cast<unsigned int>(mPhaseList.size()))
   {
      mPhaseIndex = 0;
   }
}

void wsf::six_dof::GuidanceComputer::TurnOff(double aSimTime)
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
   WsfMoverGuidance* guidancePtr = mVehicleMoverPtr->GetGuidance();
   if (this != guidancePtr)
   {
      mVehicleMoverPtr->SetGuidance(nullptr);
   }
}

//! A callback from the mover that indicates that the route has changed.
//! @param aSimTime   The current simulation time.
void wsf::six_dof::GuidanceComputer::RouteChanged(double aSimTime)
{
   if (mShowStatus)
   {
      auto logInfo = ut::log::info() << "GuidanceComputer::RouteChanged: Detected route change.";
      PrintStatusHeader(aSimTime, aSimTime, logInfo);
   }
   if (mFollowingRoute)
   {
      const WsfRoute* routePtr = mVehicleMoverPtr->GetRoute();
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

//! Called from script interface to command the mover to stop the engines.
bool wsf::six_dof::GuidanceComputer::StopEngines()
{
   double simTime(GetSimTime());
   if (mShowStatus)
   {
      auto logInfo = ut::log::info() << " GuidanceComputer::StopEngines: Commanding Engines to Stop";
      PrintStatusHeader(simTime, simTime, logInfo);
   }
   // mVehicleMoverPtr->SetCommandedEngineStopTime(simTime);
   mVehicleMoverPtr->TerminateThrust(utils::TimeToTime(simTime));

   return true;
}

//! A callback from the mover that indicates the current stage is igniting.
//! @param aSimTime   The current simulation time.
//! @param aEventTime The actual time the stage ignited. This may be less than the current simulation time.
void wsf::six_dof::GuidanceComputer::StageIgnition(double aSimTime, double aEventTime)
{
   if (mShowStatus)
   {
      auto logInfo = ut::log::info() << "GuidanceComputer::StageIgnition - " << mVehicleMoverPtr->CurrentStage() + 1;
      PrintStatusHeader(aSimTime, aEventTime, logInfo);
   }
   if (mShowGraphics)
   {
      UpdateWeaponPerception(aEventTime);
      DrawPoint(0.2, 0.7, 0.2, 8);
   }
   SelectPhase(aSimTime, aEventTime, mPhaseList[mPhaseIndex].mStageIgnitionPhaseName);
}

//! A callback from the mover that indicates the current stage has just burned-out.
//! @param aSimTime   The current simulation time.
//! @param aEventTime The actual time the stage burned-out. This may be less than the current simulation time.
void wsf::six_dof::GuidanceComputer::StageBurnout(double aSimTime, double aEventTime)
{
   if (mShowStatus)
   {
      auto out = ut::log::info() << "GuidanceComputer: Detected stage burnout.";
      PrintStatusHeader(aSimTime, aEventTime, out);
      out.AddNote() << "Stage: " << mVehicleMoverPtr->CurrentStage() + 1;
   }
   if (mShowGraphics)
   {
      UpdateWeaponPerception(aEventTime);
      DrawPoint(0.2, 0.7, 0.2, 8);
   }
   SelectPhase(aSimTime, aEventTime, mPhaseList[mPhaseIndex].mStageBurnoutPhaseName);
}

//! A callback from the mover that indicates the current stage is separating.
//! @param aSimTime   The current simulation time.
//! @param aEventTime The actual time the stage separated. This may be less than the current simulation time.
void wsf::six_dof::GuidanceComputer::StageSeparation(double aSimTime, double aEventTime)
{
   if (mShowStatus)
   {
      auto out = ut::log::info() << "GuidanceComputer: Detected stage separation.";
      PrintStatusHeader(aSimTime, aEventTime, out);
      out.AddNote() << "Stage: " << mVehicleMoverPtr->CurrentStage() + 1;
   }
   if (mShowGraphics)
   {
      UpdateWeaponPerception(aEventTime);
      DrawPoint(0.2, 0.7, 0.2, 8);
   }
   SelectPhase(aSimTime, aEventTime, mPhaseList[mPhaseIndex].mStageSeparationPhaseName);
}

//! Select the indicated phase as the current phase.
//! @param aSimTime    The current simulation time.
//! @param aChangeTime The time at which the phase change is actually occurring. This may be less than
//!                    the current simulation time.
//! @param aPhaseName  The name (dictionary ID) of the phase that is to become the current phase.
// protected
void wsf::six_dof::GuidanceComputer::SelectPhase(double aSimTime, double aChangeTime, WsfStringId aPhaseName)
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

   // Notify subscribers to the phase change callback
   WsfObserver::GuidanceComputerPhaseChanged(GetSimulation())(aSimTime, this);

   if (mShowStatus)
   {
      {
         // RAII block
         auto out = ut::log::debug() << "Guidance computer changed phase.";
         PrintStatusHeader(aSimTime, aChangeTime, out);
         out.AddNote() << "New Phase: " << aPhaseName;
         mVehicleMoverPtr->WriteKinematicStatus(out);
      }
      mVehicleMoverPtr->GetPlatform()->Comment(aSimTime, "Guidance Change to " + aPhaseName);
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
      mVehicleMoverPtr->SetCommandedSpeed(-1.0);
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
   for (auto& program : phase.mPrograms)
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
void wsf::six_dof::GuidanceComputer::SelectWaypoint(double aSimTime, double aChangeTime, unsigned int aNewIndex)
{
   const WsfRoute* routePtr = mVehicleMoverPtr->GetRoute();
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

//! Update the aimpoint location to which we will guide.
//! @returns true if the aimpoint is valid.
// protected
bool wsf::six_dof::GuidanceComputer::UpdateAimpoint(double aCurTime, const Phase& aPhase)
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
      const WsfRoute* routePtr = mVehicleMoverPtr->GetRoute();
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

               // If a neither the left or right aimpoint was specifically choosen,
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

//! Update the perception of the target
void wsf::six_dof::GuidanceComputer::UpdateTargetPerception(double aCurTime)
{
   // Refresh perceived target location and velocity, if possible.

   WsfTrackManager& trackMgr = GetPlatform()->GetTrackManager();
   const WsfTrack*  trackPtr = trackMgr.GetCurrentTarget();

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
            if (dt >= (0.999 * mVehicleMoverPtr->GetUpdateInterval()))
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

void wsf::six_dof::GuidanceComputer::UpdateWeaponPerception(double aCurTime)
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

   double dt = aCurTime - mVehicleMoverPtr->GetLastUpdateTime();
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
//! Update the terrain following status.
//! @param aPhase The current phase.
// protected
void wsf::six_dof::GuidanceComputer::UpdateTerrainFollowing(const Phase& aPhase)
{
   // If commanded to an AGL altitude and terrain is enabled, tell the mover to not let
   // the platform fall below the terrain (crude terrain following).
   double minimumHeightAboveTerrain = 0.0;
   if ((aPhase.mCommandedAlt != cUNDEFINED_DOUBLE) && aPhase.mCommandedAltIsAGL)
   {
      minimumHeightAboveTerrain = 1.0;
   }
   mVehicleMoverPtr->SetMinimumHeightAboveTerrain(minimumHeightAboveTerrain);
}

// ================================================================================================
//! Determine if we are starting, continuing or stopping route following.
//! @param aPhase The current phase.
// protected
void wsf::six_dof::GuidanceComputer::UpdateRouteFollowing(const Phase& aPhase)
{
   bool wasFollowingRoute   = mFollowingRoute;
   mFollowingRoute          = false;
   const WsfRoute* routePtr = mVehicleMoverPtr->GetRoute();
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
//! Determine if a waypoint switch is required.
void wsf::six_dof::GuidanceComputer::CheckForWaypointChange(double aSimTime, double aEndTime)
{
   // Only the horizontal components of position are checked.
   const WsfRoute* routePtr = mVehicleMoverPtr->GetRoute();
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
