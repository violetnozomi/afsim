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
// Updated by Infoscitex, a DCS Company.
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

#include "WsfP6DOF_GuidanceComputer.hpp"

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
#include "UtScriptBasicTypes.hpp"
#include "UtVec3.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfGuidanceProgram.hpp"
#include "WsfGuidedMover.hpp"
#include "WsfGuidedMoverBase.hpp"
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

namespace
{
const double cLARGE_POSITIVE_DOUBLE                    = 1.0E+30;
const double cP6DOF_GUIDANCE_COMPUTER_UNDEFINED_DOUBLE = 1.0E+30;
const double cFROM_LAUNCH_COMPUTER                     = 1.0E+31;

// The tolerance for performing a flight path angle phase change.
const double cFLIGHT_PATH_ANGLE_TOLERANCE = 0.0001 * UtMath::cRAD_PER_DEG;
} // namespace

WsfP6DOF_GuidanceComputer::WsfP6DOF_GuidanceComputer(WsfScenario& aScenario)
   : WsfGuidanceComputer(aScenario)
   , mP6DofMoverPtr(nullptr)
{
}

WsfP6DOF_GuidanceComputer::WsfP6DOF_GuidanceComputer(const WsfP6DOF_GuidanceComputer& aSrc)
   : WsfGuidanceComputer(aSrc)
   , mP6DofMoverPtr(nullptr)
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

WsfProcessor* WsfP6DOF_GuidanceComputer::Clone() const
{
   return new WsfP6DOF_GuidanceComputer(*this);
}

bool WsfP6DOF_GuidanceComputer::Initialize(double aSimTime)
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

   // mMoverPtr = dynamic_cast<WsfGuidedMoverBase*>(GetPlatform()->GetMover());
   // if (mMoverPtr == 0)
   // {
   //    ut::log::warning() << "Mover is of the incorrect type; turning guidance off. A WsfP6DOF_Mover is required.";
   //    SetInitiallyTurnedOn(false);
   // }
   // else
   // {
   //    mMoverPtr->SetGuidance(this);
   //    mCallbacks.Add(mMoverPtr->RouteChanged.Connect(&WsfP6DOF_GuidanceComputer::RouteChanged, this));
   //    mCallbacks.Add(mMoverPtr->StageIgnition.Connect(&WsfP6DOF_GuidanceComputer::StageIgnition, this));
   //    mCallbacks.Add(mMoverPtr->StageBurnout.Connect(&WsfP6DOF_GuidanceComputer::StageBurnout, this));
   //    mCallbacks.Add(mMoverPtr->StageSeparation.Connect(&WsfP6DOF_GuidanceComputer::StageSeparation, this));
   //    mCallbacks.Add(GetSimulation()->GetObserver().SensorTrackInitiated.Connect(&WsfGuidanceComputer::SensorTrackInitiated,
   //    this));
   // }

   mP6DofMoverPtr = dynamic_cast<WsfP6DOF_Mover*>(GetPlatform()->GetMover());
   if (mP6DofMoverPtr == nullptr)
   {
      ut::log::warning() << "Mover is of the incorrect type; turning guidance off. A WsfP6DOF_Mover is required.";
      SetInitiallyTurnedOn(false);
   }
   else
   {
      mP6DofMoverPtr->SetGuidance(this);
      mCallbacks.Add(mP6DofMoverPtr->RouteChanged.Connect(&WsfP6DOF_GuidanceComputer::RouteChanged, this));
      mCallbacks.Add(mP6DofMoverPtr->StageIgnition.Connect(&WsfP6DOF_GuidanceComputer::StageIgnition, this));
      mCallbacks.Add(mP6DofMoverPtr->StageBurnout.Connect(&WsfP6DOF_GuidanceComputer::StageBurnout, this));
      mCallbacks.Add(mP6DofMoverPtr->StageSeparation.Connect(&WsfP6DOF_GuidanceComputer::StageSeparation, this));

      mCallbacks.Add(
         WsfObserver::SensorTrackInitiated(GetSimulation()).Connect(&WsfP6DOF_GuidanceComputer::SensorTrackInitiated, this));
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
void WsfP6DOF_GuidanceComputer::UpdateGuidance(double aSimTime, double aCurTime, double aEndTime)
{
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "WsfP6DOF_GuidanceComputer: UpdateGuidance Debug Dump:" << std::fixed
                                  << std::setprecision(5);
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
      //    mP6DofMoverPtr->SetYandZForces(0.0, 0.0);
      mP6DofMoverPtr->SetYandZGLoads(0.0, 0.0);
      mP6DofMoverPtr->SetCommandedSpeed(-1.0);

      // Mark current target as invalid
      P6DOF::CurrentTargetData currentTarget;
      currentTarget.isValid = false;
      mP6DofMoverPtr->SetTargetData(currentTarget);

      return;
   }

   // If the time since the last update is too small, ignore the update until enough time has accumulated.
   double deltaTime = aCurTime - mLastUpdateTime;
   if (deltaTime < 1.0E-6)
   {
      return;
   }

   // Update the perceived kinematic state of myself.
   UpdateWeaponPerception(aCurTime);

   // Update the perceived kinematic state of the target.
   UpdateTargetPerception(aCurTime);

   // If following a route, determine if a waypoint change is needed.
   //   if (mFollowingRoute)
   //   {
   //      CheckForWaypointChange(aSimTime, aCurTime);
   //   }

   // Determine if a phase change is required.
   CheckForPhaseChange(aSimTime, aCurTime);
   Phase& phase = mPhaseList[mPhaseIndex];

   // Compute forces if guidance is enabled.
   double yAccel = 0.0;
   double zAccel = 0.0;
   if (aCurTime >= mGuidanceStartTime)
   {
      // Update the target vector to which we will guide (which may point towards the real target or the next
      // waypoint). If the vector cannot be determined (no target or waypoint) then the guidance functions
      // involving a target point will not be performed.

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
         for (auto& pli : mActivePrograms)
         {
            pli->Execute(mState, cmds);
         }
      }
      else if (mLegacyProgramPtr != nullptr)
      {
         mLegacyProgramPtr->Execute(mState, cmds);
      }

      // Update the programmed attitude control
      mP6DofMoverPtr->SetCommandedYawRate(cmds.mAngleRateCmd[2]);
      mP6DofMoverPtr->SetCommandedPitchRate(cmds.mAngleRateCmd[1]);
      mP6DofMoverPtr->SetCommandedRollRate(cmds.mAngleRateCmd[0]);

      // Apply acceleration limits, convert accelerations to forces and push to the mover
      yAccel = cmds.mAccelCmd[1];
      zAccel = cmds.mAccelCmd[2];

      // The guidance above is taken from "standard" AFSIM guidance, which is for 3DOF movers.
      // The P6DOF mover includes pitch and roll in addition to the heading (yaw) of the 3DOF mover.
      // As a result, we need to correct the pitch and yaw commands (just calculated above)
      // to work correctly for the more realistic P6DOF mover.

      // First, get the P6DOF object
      P6DofVehicle* object = mP6DofMoverPtr->GetP6DofVehicle();
      if (object == nullptr)
      {
         // No object, so send no commands...
         mP6DofMoverPtr->SetYandZGLoads(0.0, 0.0);
         mP6DofMoverPtr->SetCommandedSpeed(-1.0);
         mLastUpdateTime = aCurTime;
         return;
      }

      // Get attitude in NED
      double hdgNED_rad   = 0;
      double pitchNED_rad = 0;
      double rollNED_rad  = 0;
      object->GetAttitudeNED(hdgNED_rad, pitchNED_rad, rollNED_rad);

      double tgtLoc_WCS[3];
      mState.mTgtState.GetLocationWCS(tgtLoc_WCS);
      double mslLoc_WCS[3];
      mState.mWpnState.GetLocationWCS(mslLoc_WCS);

      UtVec3dX tgtLocVec_WCS(tgtLoc_WCS);
      UtVec3dX mslLocVec_WCS(mslLoc_WCS);
      UtVec3dX deltaLocVec_WCS = tgtLocVec_WCS - mslLocVec_WCS;

      UtVec3dX bodyVec = object->GetKinematicState().CalcBodyVecFromInertialVec(deltaLocVec_WCS);

      UtVec3dX unitBodyVec = bodyVec;
      unitBodyVec.Normalize();

      double az_deg = UtMath::cDEG_PER_RAD * atan2(unitBodyVec.Y(), unitBodyVec.X());
      if (az_deg < -180)
      {
         az_deg += 360.0;
      }
      if (az_deg > 360)
      {
         az_deg -= 360.0;
      }
      double xy     = sqrt(unitBodyVec.X() * unitBodyVec.X() + unitBodyVec.Y() * unitBodyVec.Y());
      double el_deg = UtMath::cDEG_PER_RAD * atan2(-unitBodyVec.Z(), xy);

      UtVec3dX accelTemp(0, yAccel, zAccel);
      UtDCM    baseDCM(hdgNED_rad, 0, 0);

      UtVec3dX unitNED_X(1., 0., 0.);
      UtVec3dX unitNED_Y(0., 1., 0.);
      UtVec3dX unitNED_Z(0., 0., 1.);
      UtDCM    vehicleDCM(hdgNED_rad, pitchNED_rad, rollNED_rad);
      unitNED_X = vehicleDCM.InverseTransform(unitNED_X);
      unitNED_Y = vehicleDCM.InverseTransform(unitNED_Y);
      unitNED_Z = vehicleDCM.InverseTransform(unitNED_Z);

      // Now determine accel in Y and Z
      yAccel = UtMath::Limit(yAccel, static_cast<double>(phase.mMaxGeeCmd));
      zAccel = UtMath::Limit(zAccel, static_cast<double>(phase.mMaxGeeCmd));

      double tgtAz_rad = 0;
      double tgtEl_rad = 0;
      mState.mWpnState.ComputeAzimuthAndElevation(tgtLoc_WCS, tgtAz_rad, tgtEl_rad);

      double dx      = tgtLoc_WCS[0] - mslLoc_WCS[0];
      double dy      = tgtLoc_WCS[1] - mslLoc_WCS[1];
      double dz      = tgtLoc_WCS[2] - mslLoc_WCS[2];
      double range_m = sqrt(dx * dx + dy * dy + dz * dz);

      P6DOF::CurrentTargetData currentTarget;

      WsfTrackManager& trackMgr = GetPlatform()->GetTrackManager();
      const WsfTrack*  trackPtr = trackMgr.GetCurrentTarget();

      std::string targetName;

      if (trackPtr != nullptr)
      {
         WsfStringId id = trackPtr->GetTargetName();
         if (!id.IsNull())
         {
            targetName = trackPtr->GetTargetName().GetString();
         }
      }

      // Set the target name
      currentTarget.targetName                  = targetName;
      currentTarget.targetAziumth_deg           = (float)az_deg;
      currentTarget.targetElevation_deg         = (float)el_deg;
      currentTarget.targetSlantRange_nm         = (float)(range_m * UtMath::cNM_PER_M);
      currentTarget.guidanceYawAccelCommand_g   = (float)(yAccel / 9.81);
      currentTarget.guidancePitchAccelCommand_g = (float)(zAccel / 9.81);
      currentTarget.guidanceGBias_g             = (float)mPhaseList[mPhaseIndex].mGeeBiasValue;
      currentTarget.guidanceMaxG                = (float)(mPhaseList[mPhaseIndex].mMaxGeeCmd / 9.81);
      currentTarget.isValid                     = true;

      if (mCommandedFlightPathAngle != cP6DOF_GUIDANCE_COMPUTER_UNDEFINED_DOUBLE)
      {
         currentTarget.guidanceFlightPathAngle_deg = (float)(mCommandedFlightPathAngle * UtMath::cDEG_PER_RAD);
      }
      else
      {
         currentTarget.guidanceFlightPathAngle_deg = 0;
      }

      currentTarget.autopilotYawGLoadCommand_g   = 0;
      currentTarget.autopilotPitchGLoadCommand_g = 0;
      currentTarget.autopilotRollRateCommand_dps = 0;

      mP6DofMoverPtr->SetTargetData(currentTarget);

      //   "maximum_commanded_g"  -- this is used to  default is 25 g
      //   *************************************************************************************************************
      //    yAccel = UtMath::Limit(yAccel, phase.mMaxGeeCmd);
      //    zAccel = UtMath::Limit(zAccel, phase.mMaxGeeCmd);
      //    double mass = GetPlatform()->GetMass();
      //    mP6DofMoverPtr->SetYandZForces(mass * yAccel, mass * zAccel);
      //    mP6DofMoverPtr->SetYandZForces(yAccel, zAccel);
      mP6DofMoverPtr->SetYandZGLoads(yAccel / 9.81, zAccel / 9.81);

      //    cout << " Guid G-P=" << zAccel/9.81 << " G-Y=" << yAccel/9.81 << endl;
      //    cout << " Guidance P-Rate=" << cmds.mAngleRateCmd[1] * UtMath::cDEG_PER_RAD << endl;

      // Update command speed/mach if requested.
      if (phase.mCommandedSpeed != cP6DOF_GUIDANCE_COMPUTER_UNDEFINED_DOUBLE)
      {
         if (phase.mCommandedSpeed >= 0.0)
         {
            mP6DofMoverPtr->SetCommandedSpeed(phase.mCommandedSpeed);
         }
         else
         {
            mP6DofMoverPtr->SetCommandedSpeed(-phase.mCommandedSpeed *
                                              mAtmosphere.SonicVelocity(GetPlatform()->GetAltitude()));
         }
      }
      else
      {
         mP6DofMoverPtr->SetCommandedSpeed(-1.0);
      }
   }
   else
   {
      // Guidance is disabled... no commands...
      mP6DofMoverPtr->SetYandZGLoads(0.0, 0.0);
      mP6DofMoverPtr->SetCommandedSpeed(-1.0);

      // Mark current target as invalid
      P6DOF::CurrentTargetData currentTarget;
      currentTarget.isValid = false;
      mP6DofMoverPtr->SetTargetData(currentTarget);
   }

   // Go ballistic without a target!!!
   if (!mTgtStateDefined)
   {
      // Guidance is disabled... no commands...
      mP6DofMoverPtr->SetYandZGLoads(0.0, 0.0);
      mP6DofMoverPtr->SetCommandedSpeed(-1.0);

      // Mark current target as invalid
      P6DOF::CurrentTargetData currentTarget;
      currentTarget.isValid = false;
      mP6DofMoverPtr->SetTargetData(currentTarget);
   }

   mLastUpdateTime = aCurTime;

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Updated P6DOF guidance computer." << std::fixed << std::setprecision(5);
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

bool WsfP6DOF_GuidanceComputer::ProcessInput(UtInput& aInput)
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
      WsfStringId nameId = nameStr;
      for (ProgramList::const_iterator pli = mDefinedPrograms.begin(); pli != mDefinedPrograms.end(); ++pli)
      {
         if ((*pli)->GetNameId() == nameId)
         {
            throw UtInput::BadValue(aInput, "Duplicate guidance program defined: " + nameStr);
         }
      }

      // Allocate and populate a program object of the requested type

      std::unique_ptr<WsfGuidanceProgram> programPtr(WsfGuidanceProgramTypes::Get(GetScenario()).CreateInstance(typeStr));
      if (programPtr.get() == nullptr)
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

void WsfP6DOF_GuidanceComputer::TurnOn(double aSimTime)
{
   WsfScriptProcessor::TurnOn(aSimTime); // Let the base class do its thing

   mP6DofMoverPtr->SetGuidance(this); // Indicate this is performing the guidance
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

void WsfP6DOF_GuidanceComputer::TurnOff(double aSimTime)
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
   WsfMoverGuidance* guidancePtr = mP6DofMoverPtr->GetGuidance();
   if (this != guidancePtr)
   {
      mP6DofMoverPtr->SetGuidance(nullptr);
   }
}

//! A callback from the mover that indicates that the route has changed.
//! @param aSimTime   The current simulation time.
void WsfP6DOF_GuidanceComputer::RouteChanged(double aSimTime)
{
   if (mShowStatus)
   {
      auto logInfo = ut::log::info() << "WsfP6DOF_GuidanceComputer::RouteChanged: Detected route change.";
      PrintStatusHeader(aSimTime, aSimTime, logInfo);
   }
   if (mFollowingRoute)
   {
      const WsfRoute* routePtr = mP6DofMoverPtr->GetRoute();
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
bool WsfP6DOF_GuidanceComputer::StopEngines()
{
   double simTime(GetSimTime());
   if (mShowStatus)
   {
      auto logInfo = ut::log::info() << " WsfP6DOF_GuidanceComputer::StopEngines: Commanding Engines to Stop";
      PrintStatusHeader(simTime, simTime, logInfo);
   }
   // mP6DofMoverPtr->SetCommandedEngineStopTime(simTime);
   mP6DofMoverPtr->GetP6DofVehicle()->TerminateThrust(P6DofUtils::TimeToTime(simTime));

   return true;
}

//! A callback from the mover that indicates the current stage is igniting.
//! @param aSimTime   The current simulation time.
//! @param aEventTime The actual time the stage ignited. This may be less than the current simulation time.
void WsfP6DOF_GuidanceComputer::StageIgnition(double aSimTime, double aEventTime)
{
   if (mShowStatus)
   {
      auto logInfo = ut::log::info() << "WsfP6DOF_GuidanceComputer::StageIgnition - "
                                     << mP6DofMoverPtr->CurrentStage() + 1;
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
void WsfP6DOF_GuidanceComputer::StageBurnout(double aSimTime, double aEventTime)
{
   if (mShowStatus)
   {
      auto logInfo = ut::log::info() << "WsfP6DOF_GuidanceComputer::StageBurnout: Detected Burnout of Stage - "
                                     << mP6DofMoverPtr->CurrentStage() + 1;
      PrintStatusHeader(aSimTime, aEventTime, logInfo);
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
void WsfP6DOF_GuidanceComputer::StageSeparation(double aSimTime, double aEventTime)
{
   if (mShowStatus)
   {
      auto logInfo = ut::log::info() << "WsfP6DOF_GuidanceComputer::StageSeparation: Detected Separation of Stage - "
                                     << mP6DofMoverPtr->CurrentStage() + 1;
      PrintStatusHeader(aSimTime, aEventTime, logInfo);
   }
   if (mShowGraphics)
   {
      UpdateWeaponPerception(aEventTime);
      DrawPoint(0.2, 0.7, 0.2, 8);
   }
   SelectPhase(aSimTime, aEventTime, mPhaseList[mPhaseIndex].mStageSeparationPhaseName);
}

void WsfP6DOF_GuidanceComputer::CheckForPhaseChange(double aSimTime, double aEndTime)
{
   Phase& phase            = mPhaseList[mPhaseIndex];
   double timeToIntercept  = -1.0;
   double rangeToIntercept = -1.0;
   for (auto& pci : phase.mPhaseChangeList)
   {
      pci.mValue.Update(*mContextPtr); // Ensure reference value is updated if a script reference
      double value       = 0.0;
      bool   changePhase = false;
      switch (pci.mName)
      {
      case PhaseChange::cPHASE_TIME:
      {
         value       = aEndTime - mPhaseStartTime;
         changePhase = pci.ConditionSatisfied(value);
      }
      break;
      case PhaseChange::cFLIGHT_TIME:
      {
         value       = aEndTime - mLaunchTime;
         changePhase = pci.ConditionSatisfied(value);
      }
      break;
      case PhaseChange::cALTITUDE:
      {
         value       = mState.mWpnState.GetAltitude();
         changePhase = pci.ConditionSatisfied(value);
      }
      break;
      case PhaseChange::cSPEED:
      {
         value       = mState.mWpnState.GetSpeed();
         changePhase = pci.ConditionSatisfied(value);
      }
      break;
      case PhaseChange::cFLIGHT_PATH_ANGLE:
      {
         double wpnVelNED[3];
         mState.mWpnState.GetVelocityNED(wpnVelNED);
         double velNE = sqrt(wpnVelNED[0] * wpnVelNED[0] + wpnVelNED[1] * wpnVelNED[1]);
         value        = atan2(-wpnVelNED[2], velNE);
         changePhase  = pci.ConditionSatisfied(value);
      }
      break;
      case PhaseChange::cON_COMMANDED_FLIGHT_PATH_ANGLE:
      {
         if (mCommandedFlightPathAngle != cP6DOF_GUIDANCE_COMPUTER_UNDEFINED_DOUBLE)
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
         changePhase    = pci.ConditionSatisfied(value);
      }
      break;
      case PhaseChange::cTARGET_ALTITUDE:
         if (mTgtStateDefined)
         {
            double notUsed;
            mState.mTgtState.GetLocationLLA(notUsed, notUsed, value);
            changePhase = pci.ConditionSatisfied(value);
         }
         break;
      case PhaseChange::cTARGET_SPEED:
         if (mTgtStateDefined)
         {
            value       = mState.mTgtState.GetSpeed();
            changePhase = pci.ConditionSatisfied(value);
         }
         break;
      case PhaseChange::cTARGET_FLIGHT_PATH_ANGLE:
         if (mTgtStateDefined)
         {
            double tgtVelNED[3];
            mState.mTgtState.GetVelocityNED(tgtVelNED);
            double tgtVelNE = sqrt((tgtVelNED[0] * tgtVelNED[0]) + (tgtVelNED[1] * tgtVelNED[1]));
            value           = atan2(-tgtVelNED[2], tgtVelNE);
            changePhase     = pci.ConditionSatisfied(value);
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
               changePhase = pci.ConditionSatisfied(value);
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
            changePhase = pci.ConditionSatisfied(value);
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
            changePhase = pci.ConditionSatisfied(value);
         }
         break;
      case PhaseChange::cTARGET_SLANT_RANGE:
         value = cLARGE_POSITIVE_DOUBLE;
         if (mTgtStateDefined)
         {
            double tgtLocNED[3];
            mState.mWpnState.GetRelativeLocationNED(&mState.mTgtState, tgtLocNED);
            value       = UtVec3d::Magnitude(tgtLocNED);
            changePhase = pci.ConditionSatisfied(value);
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
            changePhase = pci.ConditionSatisfied(value);
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
               changePhase = pci.ConditionSatisfied(value);
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
               changePhase     = pci.ConditionSatisfied(value);
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
               changePhase = pci.ConditionSatisfied(value);
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
               changePhase = pci.ConditionSatisfied(value);
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
               changePhase           = pci.ConditionSatisfied(value);
            }
         }
         break;
      default:
         auto out = ut::log::error() << "Unknown phase change variable in guidance computer.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Computer: " << GetName();
         out.AddNote() << "Phase Change: " << pci.mName;
         continue;
      }

      if (mShowEvaluations)
      {
         std::string variable;
         double      scale          = 1.0;
         double      referenceValue = pci.mValue;
         switch (pci.mName)
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

         std::string condition;
         switch (pci.mCondition)
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

         auto out = ut::log::info() << "Phase Change Evaluation:";
         out.AddNote() << "T = " << aEndTime;
         out.AddNote() << "Since Launch: " << aEndTime - mLaunchTime << " sec";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Condition: " << variable << "(=" << scale * value << ") " << condition << " "
                       << scale * referenceValue;
         out.AddNote() << "Result: " << std::boolalpha << changePhase;
      }
      if (changePhase)
      {
         SelectPhase(aSimTime, aEndTime, pci.mNextPhaseName);
         break;
      }
   }
}

//! Select the indicated phase as the current phase.
//! @param aSimTime    The current simulation time.
//! @param aChangeTime The time at which the phase change is actually occurring. This may be less than
//!                    the current simulation time.
//! @param aPhaseName  The name (dictionary ID) of the phase that is to become the current phase.
// protected
void WsfP6DOF_GuidanceComputer::SelectPhase(double aSimTime, double aChangeTime, WsfStringId aPhaseName)
{
   // Exit immediately if a phase name was not specified.
   if (aPhaseName.IsNull())
   {
      return;
   }

   // Execute the 'on_exit' block for the phase we are leaving.
   if ((mPhaseIndex < mPhaseList.size()) && (!mOnExitScriptActive))
   {
      UtScript* scriptPtr = mContextPtr->FindScript(mPhaseList[mPhaseIndex].mPhaseName.GetString() + "::on_exit");
      if (scriptPtr != nullptr)
      {
         // If the on_exit script executes a SelectPhase command,
         //   1) The on_exit script should not be executed again (the SelectPhase will call this routine!)
         //   2) The command will cause a phase change, so we shouldn't keep going.

         unsigned int savedPhaseIndex = mPhaseIndex;
         mOnExitScriptActive          = true;
         mContextPtr->ExecuteScript(aSimTime, scriptPtr);
         mOnExitScriptActive = false;
         if (mPhaseIndex != savedPhaseIndex)
         {
            return;
         }
      }
   }

   if (mShowStatus)
   {
      { // RAII block
         auto out = ut::log::info() << "Guidance computer changed phase.";
         PrintStatusHeader(aSimTime, aChangeTime, out);
         out.AddNote() << "New Phase: " << aPhaseName;
         mP6DofMoverPtr->WriteKinematicStatus(out);
      }
      mP6DofMoverPtr->GetPlatform()->Comment(aSimTime, "Guidance Change to " + std::string(aPhaseName));
   }

   if (mShowGraphics)
   {
      DrawPoint(0.2, 0.7, 0.2, 12);
   }

   unsigned int phaseIndex = FindPhase(aPhaseName);
   if (phaseIndex >= mPhaseList.size())
   {
      auto out = ut::log::error() << "Requested guidance phase does not exist.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Phase: " << aPhaseName;
      out.AddNote() << "Guidance will not be changed.";
      return;
   }

   Phase& phase       = mPhaseList[phaseIndex];
   mPhaseIndex        = phaseIndex;
   mPhaseStartTime    = aChangeTime;
   mGuidanceStartTime = mPhaseStartTime + phase.mGuidanceDelay;
   if (phase.mCommandedSpeed == cP6DOF_GUIDANCE_COMPUTER_UNDEFINED_DOUBLE)
   {
      mP6DofMoverPtr->SetCommandedSpeed(-1.0);
   }

   mCommandedFlightPathAngle = phase.mCommandedFlightPathAngle;
   if (phase.mCommandedFlightPathAngle == cFROM_LAUNCH_COMPUTER)
   {
      mCommandedFlightPathAngle = mLaunchComputerFlightPathAngle;
   }

   // If commanded to an AGL altitude and terrain is enabled, tell the mover to not let
   // the platform fall below the terrain (crude terrain following).
   double minimumHeightAboveTerrain = 0.0;
   if ((phase.mCommandedAlt != cP6DOF_GUIDANCE_COMPUTER_UNDEFINED_DOUBLE) && phase.mCommandedAltIsAGL)
   {
      minimumHeightAboveTerrain = 1.0;
   }
   mP6DofMoverPtr->SetMinimumHeightAboveTerrain(minimumHeightAboveTerrain);

   // Cancel any active aim point validity
   mAimpointExpireTime = -1.0;

   // Execute on the 'on-entry' script for the phase we are entering
   UtScript* scriptPtr = mContextPtr->FindScript(mPhaseList[mPhaseIndex].mPhaseName.GetString() + "::on_entry");
   if (scriptPtr != nullptr)
   {
      unsigned int savedPhaseIndex = mPhaseIndex;
      mContextPtr->ExecuteScript(aSimTime, scriptPtr);
      if (mPhaseIndex != savedPhaseIndex)
      {
         return;
      }
   }

   // Generate a callback for interested listeners.
   WsfObserver::GuidanceComputerPhaseChanged(GetSimulation())(aSimTime, this);

   // Determine if we are starting route following, continuing route following or stopping route following.
   bool wasFollowingRoute   = mFollowingRoute;
   mFollowingRoute          = false;
   const WsfRoute* routePtr = mP6DofMoverPtr->GetRoute();
   if ((routePtr != nullptr) && (!routePtr->Empty()))
   {
      if (phase.mAllowRouteFollowing)
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
   mActivePrograms = newActivePrograms;
}

//! Update the aimpoint location to which we will guide.
//! @returns true if the aimpoint is valid.
// protected
bool WsfP6DOF_GuidanceComputer::UpdateAimpoint(double aCurTime, const Phase& aPhase)
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
      if (aPhase.mCommandedAlt != cP6DOF_GUIDANCE_COMPUTER_UNDEFINED_DOUBLE)
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
      const WsfRoute* routePtr = mP6DofMoverPtr->GetRoute();
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
void WsfP6DOF_GuidanceComputer::UpdateTargetPerception(double aCurTime)
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

      trackPtr->GetElevation();
      trackPtr->GetExtrapolatedLocationWCS(aCurTime, tgtLocWCS);
      trackPtr->GetVelocityWCS(tgtVelWCS);

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
            if (dt >= (0.999 * mP6DofMoverPtr->GetUpdateInterval()))
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
      // No target, so show that the state is undefined
      mTgtStateDefined = false;

      mLastTgtUpdateTime = cLARGE_POSITIVE_DOUBLE;
   }
}

void WsfP6DOF_GuidanceComputer::UpdateWeaponPerception(double aCurTime)
{
   // Propagate the truth location and velocity to the current time.
   // (The truth values *MAY* be slightly out of date, so propagate it forward by 'dt.'.
   // In cases where called directly from UpdateGuidance, 'dt' should be zero.

   double wpnLocWCS[3];
   GetPlatform()->GetLocationWCS(wpnLocWCS);
   double wpnVelWCS[3];
   GetPlatform()->GetVelocityWCS(wpnVelWCS);
   double wpnAclWCS[3];
   GetPlatform()->GetAccelerationWCS(wpnAclWCS);

   double dt = aCurTime - mP6DofMoverPtr->GetLastUpdateTime();
   UtVec3d::AddProduct(wpnLocWCS, wpnLocWCS, dt, wpnVelWCS);
   UtVec3d::AddProduct(wpnLocWCS, wpnLocWCS, 0.5 * dt * dt, wpnAclWCS);
   UtVec3d::AddProduct(wpnVelWCS, wpnVelWCS, dt, wpnAclWCS);
   mState.mWpnState.SetLocationWCS(wpnLocWCS);
   mState.mWpnState.SetVelocityWCS(wpnVelWCS);
   mState.mWpnState.SetAccelerationWCS(wpnAclWCS);

   // Calculate the weapon heading and flight path angle in the local tangent plane.

   double wpnVelNED[3];
   mState.mWpnState.GetVelocityNED(wpnVelNED);
   mLastFlightPathAngle    = mState.mFlightPathAngle;
   double velNE            = sqrt(wpnVelNED[0] * wpnVelNED[0] + wpnVelNED[1] * wpnVelNED[1]);
   mState.mFlightPathAngle = atan2(-wpnVelNED[2], velNE);
   mState.mWpnHeading      = atan2(wpnVelNED[1], wpnVelNED[0]);

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

   double heading, pitch, roll;
   GetPlatform()->GetOrientationNED(heading, pitch, roll);

   mState.mWpnState.SetOrientationNED(heading, pitch, roll);
}

//! Callback to indicate that a sensor track has begun.
//! @param aSimTime Simulation time.
//! @param aSensorPtr the sensor producing the track.
//! @param aTrackPtr the Track produced by the sensor.
void WsfP6DOF_GuidanceComputer::SensorTrackInitiated(double     aSimTime,
                                                     WsfSensor* aSensorPtr,
                                                     const WsfTrack*) // aTrackPtr)
{
   if (GetPlatform() == aSensorPtr->GetPlatform())
   {
      SelectPhase(aSimTime, aSimTime, mPhaseList[mPhaseIndex].mSensorTrackInitiatedPhaseName);
   }
}
