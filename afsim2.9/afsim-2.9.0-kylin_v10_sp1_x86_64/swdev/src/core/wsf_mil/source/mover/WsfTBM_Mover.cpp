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

#include "WsfTBM_Mover.hpp"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <string>

#include "UtEarth.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLatPos.hpp"
#include "UtLog.hpp"
#include "UtLonPos.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtRandom.hpp"
#include "UtScriptData.hpp"
#include "UtSphericalEarth.hpp"
#include "UtTime.hpp"
#include "UtVec3.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfStringId.hpp"
#include "WsfTerrain.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackManager.hpp"

//#define DEBUG_ON
#ifdef DEBUG_ON
#define DIAG_ON
#define DO_DEBUG(A) A
#else
#define DO_DEBUG(A) /* A */
#endif

//#define DIAG_ON
#ifdef DIAG_ON
#define DO_DIAG(A) A
#else
#define DO_DIAG(A) /* A */
#endif

namespace
{
// NOTE: The state vector really only needs to have 6 elements. The internal integration routines only
//       use the first 6 elements, but odeint2 has been modified to set the acceleration into the last
//       3 elements.

const unsigned int cSTATE_VECTOR_SIZE = 9;
const double       cNOT_SET           = -1.0;

ut::Random* sRandomPtr = nullptr;

//! Return 'a' with the same sign as 'b'.
inline double sign(double a, double b)
{
   return (b >= 0.0) ? fabs(a) : -fabs(a);
}

//! Copy a state vector
inline void CopyState(double aLHS[cSTATE_VECTOR_SIZE], const double aRHS[cSTATE_VECTOR_SIZE])
{
   for (unsigned int i = 0; i < cSTATE_VECTOR_SIZE; ++i)
   {
      aLHS[i] = aRHS[i];
   }
}
} // namespace

// =================================================================================================
WsfTBM_Mover::WsfTBM_Mover(WsfScenario& aScenario)
   : WsfMover(aScenario)
   , mLaunchTime(0.0)
   , mStage()
   , mStageIndex(0)
   , mStageList()
   , mExplicitStageUsed(false)
   , mImplicitStageUsed(false)
   , mAzErrorUsed(false)
   , mHaveTarget(true)
   , mIsBallistic(false)
   , mFixedCEP(0.0)
   , mLaunchLat(-1.0E+20)
   , mLaunchLon(-1.0E+20)
   , mLaunchAlt(1.0)
   , mLaunchAltAGL(true)
   , mIgnoreTarget(false)
   , mLofted(true)
   , mTargetLat(-1.0E+20)
   , mTargetLon(-1.0E+20)
   , mTargetAlt(-1.0E+20)
   , mTerminalLat(0.0)
   , mTerminalLon(0.0)
   , mTerminalAlt(0.0)
   , mShowStatus(false)
   , mDebugCallTrigger(0)
   , mDebugCallCount(0)
   , mNomRanges()
   , mRangeErrors()
   , mAzimuthErrors()
   , mLookupUtility()
{
   mStageList.emplace_back();
   for (size_t i = 0; i < 9; ++i)
   {
      mState[i] = 0.0;
   }
}

// =================================================================================================
//! Copy constructor (for clone)
// protected
WsfTBM_Mover::WsfTBM_Mover(const WsfTBM_Mover& aSrc)
   : WsfMover(aSrc)
   , mLaunchTime(0.0)
   , mStage()
   , mStageIndex(0)
   , mStageList(aSrc.mStageList)
   , mExplicitStageUsed(aSrc.mExplicitStageUsed)
   , mImplicitStageUsed(aSrc.mImplicitStageUsed)
   , mAzErrorUsed(aSrc.mAzErrorUsed)
   , mHaveTarget(aSrc.mHaveTarget)
   , mIsBallistic(aSrc.mIsBallistic)
   , mFixedCEP(aSrc.mFixedCEP)
   , mLaunchLat(aSrc.mLaunchLat)
   , mLaunchLon(aSrc.mLaunchLon)
   , mLaunchAlt(aSrc.mLaunchAlt)
   , mLaunchAltAGL(aSrc.mLaunchAltAGL)
   , mIgnoreTarget(aSrc.mIgnoreTarget)
   , mLofted(aSrc.mLofted)
   , mTargetLat(aSrc.mTargetLat)
   , mTargetLon(aSrc.mTargetLon)
   , mTargetAlt(aSrc.mTargetAlt)
   , mTerminalLat(aSrc.mTerminalLat)
   , mTerminalLon(aSrc.mTerminalLon)
   , mTerminalAlt(aSrc.mTerminalAlt)
   , mShowStatus(aSrc.mShowStatus)
   , mDebugCallTrigger(aSrc.mDebugCallTrigger)
   , mDebugCallCount(0)
   , mNomRanges(aSrc.mNomRanges)
   , mRangeErrors(aSrc.mRangeErrors)
   , mAzimuthErrors(aSrc.mAzimuthErrors)
   , mLookupUtility(aSrc.mLookupUtility)
{
   for (size_t i = 0; i < 9; ++i)
   {
      mState[i] = 0.0;
   }
}

// =================================================================================================
// virtual
WsfMover* WsfTBM_Mover::Clone() const
{
   return new WsfTBM_Mover(*this);
}

// =================================================================================================
// virtual
bool WsfTBM_Mover::Initialize(double aSimTime)
{
   // Initialize the base class first
   bool ok = WsfMover::Initialize(aSimTime);

   // If debug is enabled then show_status is also enabled.
   mShowStatus |= DebugEnabled();

   // Make sure all required input values have been supplied.

   unsigned int stageIndex;
   double       initialTime = 0.0;
   for (stageIndex = 0; stageIndex < mStageList.size(); ++stageIndex)
   {
      if (!mStageList[stageIndex].Initialize(initialTime))
      {
         auto out = ut::log::error() << "Platform failed to initialize stage.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Stage: " << stageIndex + 1;
         ok = false;
      }
      initialTime = mStageList[stageIndex].mSeparationTime;
   }

   if ((mFixedCEP != 0.0) && (mNomRanges.GetSize() > 0))
   {
      auto out = ut::log::error() << "Platform cannot supply both a 'cep_value' and a 'cep_table'.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      ok = false;
   }

   if (ok)
   {
      // Compute the composite vehicle values.  Note that we go through the list
      // in reverse order because the earlier stages need the weight of later stages.

      // PROGRAMMER's NOTE:  TO DO --
      // This implementation does not yet use WsfPlatform values for mass accounting purposes.

      for (stageIndex = static_cast<unsigned int>(mStageList.size()); stageIndex != 0; --stageIndex)
      {
         Stage& stage = mStageList[stageIndex - 1];

         // If not the last (or only) stage then add in the total mass from later stages
         if (stageIndex != mStageList.size())
         {
            stage.mPayloadMass = mStageList[stageIndex].mEmptyMass + mStageList[stageIndex].mFuelMass +
                                 mStageList[stageIndex].mPayloadMass;
         }

         // Finish up second pass initialization.
         stage.Initialize2();
      }

      // Set the stage separation for the final stage to 'infinity'. This simplifies a lot of loops
      // that look for a stage that contains the requested time.

      mStageList.back().mSeparationTime = DBL_MAX;

      // Set the location of the launcher. This can come from either the platform or the input.

      if ((mLaunchLat < -90.0001) || (mLaunchLon < -180.0001))
      {
         // Use the platform
         GetPlatform()->GetLocationLLA(mLaunchLat, mLaunchLon, mLaunchAlt);
      }
      else
      {
         // Use the input values.
         if (mLaunchAltAGL)
         {
            float terrainHeight;
            GetPlatform()->GetTerrain().GetElevInterp(mLaunchLat, mLaunchLon, terrainHeight);
            mLaunchAlt += terrainHeight;
         }
         GetPlatform()->SetLocationLLA(mLaunchLat, mLaunchLon, mLaunchAlt);
      }
   }
   return ok;
}

// =================================================================================================
// virtual
bool WsfTBM_Mover::Initialize2(double aSimTime)
{
   bool         ok = WsfMover::Initialize2(aSimTime);
   unsigned int stageIndex;

   // Get the location of the target.  This can come from either a track or the input.

   if ((mTargetLat < -90.0001) || (mTargetLon < -180.0001))
   {
      // Target location not specified in input.  Get target location from 'current target'.
      const WsfTrack* targetPtr = GetPlatform()->GetTrackManager().GetCurrentTarget();
      if (targetPtr != nullptr)
      {
         if (targetPtr->LocationValid())
         {
            targetPtr->GetLocationLLA(mTargetLat, mTargetLon, mTargetAlt);
         }
      }
   }

   bool mHaveTarget = true;
   if (mIgnoreTarget || (mTargetLat < -90.0001) || (mTargetLon < -180.0001))
   {
      mHaveTarget = false;
   }
   else if (mTargetAlt < -10000.0)
   {
      // Get the height of the terrain at the target site
      wsf::Terrain terrain(GetPlatform()->GetTerrain()); // Terrain object to get height at launcher and target
      float        terrainHeight;
      terrain.GetElevInterp(mTargetLat, mTargetLon, terrainHeight);
      mTargetAlt = terrainHeight;
   }

   ComputeTargetError();

   double terminalSlantRange = 0.0;
   if (mHaveTarget)
   {
      double launchLocationWCS[3];
      UtEntity::ConvertLLAToWCS(mLaunchLat, mLaunchLon, mLaunchAlt, launchLocationWCS);

      double terminalLocationWCS[3];
      UtEntity::ConvertLLAToWCS(mTerminalLat, mTerminalLon, mTerminalAlt, terminalLocationWCS);

      double dx          = launchLocationWCS[0] - terminalLocationWCS[0];
      double dy          = launchLocationWCS[1] - terminalLocationWCS[1];
      double dz          = launchLocationWCS[2] - terminalLocationWCS[2];
      terminalSlantRange = sqrt(dx * dx + dy * dy + dz * dz);
   }
   if (mShowStatus)
   {
      auto out = ut::log::info() << "Platform TBM Input Values:";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();

      out.AddNote() << "Launch Location(LLA): " << UtLatPos(mLaunchLat) << ", " << UtLonPos(mLaunchLon) << ", "
                    << mLaunchAlt << " m";
      if (mHaveTarget)
      {
         double bearing;
         double terminalGroundRange;
         UtSphericalEarth::GreatCircleHeadingAndDistance(mLaunchLat,
                                                         mLaunchLon,
                                                         mTerminalLat,
                                                         mTerminalLon,
                                                         bearing,
                                                         terminalGroundRange);

         out.AddNote() << "Target Location: " << UtLatPos(mTargetLat) << ", " << UtLonPos(mTargetLon) << ", "
                       << mTargetAlt << " m";
         out.AddNote() << "Terminal Location: " << UtLatPos(mTerminalLat) << ", " << UtLonPos(mTerminalLon) << ", "
                       << mTerminalAlt << " m";
         out.AddNote() << "Ground Range: " << terminalGroundRange * 0.001 << " km"
                       << terminalGroundRange / UtMath::cM_PER_NM << " nm";
         out.AddNote() << "Slant Range: " << terminalSlantRange * 0.001 << " km"
                       << terminalSlantRange / UtMath::cM_PER_NM << " nm";
         out.AddNote() << "Bearing: " << bearing << " deg";
      }
      else
      {
         out.AddNote() << "No target specified. Using initial orientation.";
      }
      for (stageIndex = 0; stageIndex < mStageList.size(); ++stageIndex)
      {
         Stage& stage     = mStageList[stageIndex];
         auto   stageNote = out.AddNote() << "Stage: " << stageIndex + 1;
         stageNote.AddNote() << "Empty Mass: " << stage.mEmptyMass << " kg (" << stage.mEmptyMass * UtMath::cLB_PER_KG
                             << " lb)";
         stageNote.AddNote() << "Fuel Mass: " << stage.mFuelMass << " kg (" << stage.mFuelMass * UtMath::cLB_PER_KG
                             << " lb)";
         stageNote.AddNote() << "Fuel Mass Fraction: " << stage.mFuelMassFraction;
         stageNote.AddNote() << "Total Mass: " << stage.mTotalMass << " kg (" << stage.mTotalMass * UtMath::cLB_PER_KG
                             << " lb)";
         stageNote.AddNote() << "Thrust: " << stage.mThrust << " N ("
                             << stage.mThrust * UtMath::cLB_PER_KG / UtEarth::cACCEL_OF_GRAVITY << " lbf)";

         stageNote.AddNote() << "Specific Impulse: " << stage.mSpecificImpulse << " sec";
         stageNote.AddNote() << "Wdot: " << stage.mWdot;
         stageNote.AddNote() << "Ballistic Coeff: " << stage.mBallisticCoeff << " N/m^2";
         stageNote.AddNote() << "S_Cd0: " << stage.mS_Cd0;
         if (mStageList.size() != 1)
         {
            stageNote.AddNote() << "Remaining Final Mass: " << stage.RemainingBurnoutMass() << " kg";
            stageNote.AddNote() << "Remaining Total Mass: " << stage.RemainingInitialMass() << " kg";
         }
         auto note = stageNote.AddNote() << "Transition Times (relative to launch):";
         note.AddNote() << "Initial: " << stage.mInitialTime << " sec";
         note.AddNote() << "Ignition: " << stage.mIgnitionTime << " sec";
         note.AddNote() << "Burn out: " << stage.mBurnOutTime << " sec";
         note.AddNote() << "Separation: " << stage.mSeparationTime << " sec";
      }
   }

   if (mHaveTarget)
   {
      // Compute maximum range and the launch angle for a maximum range trajectory.

      double psicrit;
      double maxRange;
      double maxAlt;
      double timeOfFlight;
      ComputeMaximumRange(0.0, UtMath::cPI_OVER_4, UtMath::cPI_OVER_2, 1.0E-6, psicrit, maxRange);

      auto out = ut::log::info();
      if (mShowStatus)
      {
         out << "Computed Trajectory Data:";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         double maxRange2;
         ComputeImpactRange(psicrit, maxRange2, timeOfFlight, maxAlt);
         auto note = out.AddNote() << "Computed Max Range:";
         note.AddNote() << "Maximum Range: " << maxRange << " m";
         note.AddNote() << "Launch Elevation: " << psicrit * UtMath::cDEG_PER_RAD << " deg";
         note.AddNote() << "TOF: " << timeOfFlight << " s";
         note.AddNote() << "Max Alt: " << maxAlt << " m";
      }

      // Compute the launch angle required to go the desired range

      double psi = 0.0;
      bool   toofar;
      ComputeLaunchAngle(terminalSlantRange, psicrit, maxRange, timeOfFlight, psi, maxAlt, toofar);
      if (mShowStatus)
      {
         auto note = out.AddNote() << "Computed Final Slant Range:";
         note.AddNote() << "Range: " << terminalSlantRange << " m";
         note.AddNote() << "Launch Elevation: " << psi * UtMath::cDEG_PER_RAD << " deg";
         note.AddNote() << "TOF: " << timeOfFlight << " s";
         note.AddNote() << "Max Alt: " << maxAlt << " m";
      }
      out.Send();

      if (toofar)
      {
         auto logError = ut::log::error() << "Platform TBM Trajectory cannot reach the desired location.";
         logError.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
   }
   else
   {
      // The launcher (e.g.: WsfExplicitWeapon) sets the state of the platform.

      double state[cSTATE_VECTOR_SIZE];
      double locationWCS[3];
      double velocityWCS[3];
      GetPlatform()->GetLocationWCS(locationWCS);
      state[0] = locationWCS[0];
      state[2] = locationWCS[1];
      state[4] = locationWCS[2];
      GetPlatform()->GetVelocityWCS(velocityWCS);
      state[1] = velocityWCS[0];
      state[3] = velocityWCS[1];
      state[5] = velocityWCS[2];
      state[6] = 0.0;
      state[7] = 0.0;
      state[8] = 0.0;
      ComputeTransitionStateVectors(state);
   }

   // Initialize the platform state to the launch conditions

   mLaunchTime     = aSimTime;
   mLastUpdateTime = aSimTime - 0.001;
   mStageIndex     = 0;
   mStage          = mStageList[mStageIndex];
   SetMasses();
   CopyState(mState, mStage.mInitialState);

   double locationWCS[3] = {mState[0], mState[2], mState[4]};
   double velocityWCS[3] = {mState[1], mState[3], mState[5]};
   double accelWCS[3]    = {0.0, 0.0, 0.0};
   GetPlatform()->SetLocationWCS(locationWCS);
   GetPlatform()->SetVelocityWCS(velocityWCS);
   GetPlatform()->SetAccelerationWCS(accelWCS);
   GetPlatform()->SetOrientationNED(0.0, 88.0 * UtMath::cRAD_PER_DEG, 0.0);

   ScheduleScriptEvents(aSimTime);

   if (mShowStatus)
   {
      double velocityNED[3];
      GetPlatform()->GetVelocityNED(velocityNED);
      double speed   = UtVec3d::Normalize(velocityNED);
      double heading = atan2(velocityNED[1], velocityNED[0]);
      double vxy     = sqrt(velocityNED[0] * velocityNED[0] + velocityNED[1] * velocityNED[1]);
      double vz      = velocityNED[2];
      double pitch   = atan2(-vz, vxy);

      // Location in WCS
      double locWCS[3] = {0.0};
      GetPlatform()->GetLocationWCS(locWCS);

      // Velocity in WCS
      double velWCS[3] = {0.0};
      GetPlatform()->GetVelocityWCS(velWCS);

      auto out = ut::log::info() << "Launch State:" << std::setprecision(10);
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Heading: " << heading * UtMath::cDEG_PER_RAD << " deg";
      out.AddNote() << "Pitch: " << pitch * UtMath::cDEG_PER_RAD << " deg";
      out.AddNote() << "Speed: " << speed << " m/s";
      out.AddNote() << "Location WCS: " << locWCS[0] << ", " << locWCS[1] << ", " << locWCS[2];
      out.AddNote() << "Velocity WCS: " << velWCS[0] << ", " << velWCS[1] << ", " << velWCS[2];
      out << std::setprecision(6);

      auto transitionNote = out.AddNote() << "Transition State Data: " << GetPlatform()->GetName();
      for (stageIndex = 0; stageIndex < mStageList.size(); ++stageIndex)
      {
         Stage& stage     = mStageList[stageIndex];
         auto   stageNote = transitionNote.AddNote() << "Stage: " << stageIndex + 1;
         {
            auto note = stageNote.AddNote() << "Ignition T: " << stage.mIgnitionTime;
            PrintState(stage.mIgnitionState, note);
         }
         {
            auto note = stageNote.AddNote() << "Burnout  T: " << stage.mBurnOutTime;
            PrintState(stage.mBurnOutState, note);
         }
         stageNote.AddNote() << "Max Acceleration: "
                             << stage.mThrust / stage.RemainingBurnoutMass() / UtEarth::cACCEL_OF_GRAVITY << " g";
      }
   }

   return ok;
}

// =================================================================================================
void WsfTBM_Mover::SetMasses()
{
   GetPlatform()->SetEmptyMass(mStage.mEmptyMass);
   GetPlatform()->SetFuelMass(mStage.mFuelMass);
   GetPlatform()->SetPayloadMass(mStage.mPayloadMass);
}

// =================================================================================================
void WsfTBM_Mover::CepTableLookup(double aTargetGroundRange, double& aLinearError, double& aAzimuthError)
{
   mLookupUtility.Lookup(mNomRanges, aTargetGroundRange);
   aLinearError = TblEvaluate(mRangeErrors, mLookupUtility);
   if (mAzErrorUsed)
   {
      aAzimuthError = TblEvaluate(mAzimuthErrors, mLookupUtility);
   }
}

// =================================================================================================
void WsfTBM_Mover::ComputeTargetError()
{
   // Have now computed the nominal desired target LLA, however, if a CEP value or
   // CEP Table was supplied, then horizontally offset the target LLA to account
   // for targeting, guidance, or any other accumulated errors.

   if (mHaveTarget)
   {
      // This initial estimate is needed to get a nominal ground range for the Table lookup.
      double initBearingNotUsed;
      double nominalGroundRange;

      UtSphericalEarth::GreatCircleHeadingAndDistance(mLaunchLat,
                                                      mLaunchLon,
                                                      mTargetLat,
                                                      mTargetLon,
                                                      initBearingNotUsed,
                                                      nominalGroundRange);

      if ((mFixedCEP != 0) || (mNomRanges.GetSize() > 0))
      {
         // CEP Offsetting of the target point is required.

         double forwardOffset = 0.0; // near-far miss
         double rightOffset   = 0.0; // right-left miss

         double linearError  = 0.0;
         double azimuthError = 0.0;

         if (mFixedCEP != 0.0)
         {
            // Constant input value for CEP:
            linearError = mFixedCEP;
         }
         else
         {
            // Table lookup required to get CEP versus nominal downrange:
            CepTableLookup(nominalGroundRange, linearError, azimuthError);
         }

         if (mAzErrorUsed)
         {
            // Interpret linearError as a downrange offset,
            // and azimuthError as a crossrange offset.
            // NOTE:  50% Normal Distribution => z = +/- 0.674   (approx)
            rightOffset   = 0.674 * nominalGroundRange * azimuthError * GetRandom().Gaussian();
            forwardOffset = 0.674 * linearError * GetRandom().Gaussian();
         }
         else
         {
            // Azimuth Error is NOT used.  Interpret linearError as
            // a true CEP in both DownRange and CrossRange directions.
            // NOTE:  Rcep = Rsigma / 1.1774
            double heading      = GetRandom().Uniform(0.0, UtMath::cTWO_PI);
            double linearOffset = GetRandom().Rayleigh(linearError * 1.1774);
            forwardOffset       = cos(heading) * linearOffset;
            rightOffset         = sin(heading) * linearOffset;
         }

         // Need to find the (forward, right) compass bearing in the target region.
         double terminalErrorBearingDeg;
         double rangeNotUsed;
         UtSphericalEarth::GreatCircleHeadingAndDistance(mTargetLat,
                                                         mTargetLon,
                                                         mLaunchLat,
                                                         mLaunchLon,
                                                         terminalErrorBearingDeg,
                                                         rangeNotUsed);
         terminalErrorBearingDeg += 180.0;

         UtEntity entity;
         entity.SetLocationLLA(mTargetLat, mTargetLon, mTargetAlt);
         entity.SetOrientationNED(terminalErrorBearingDeg * UtMath::cRAD_PER_DEG, 0.0, 0.0);
         double ecsOffset[] = {forwardOffset, rightOffset, 0.0};
         double terminalLocationWCS[3];
         entity.ConvertECSToWCS(ecsOffset, terminalLocationWCS);
         UtEntity::ConvertWCSToLLA(terminalLocationWCS, mTerminalLat, mTerminalLon, mTerminalAlt);

         // Get the height of the terrain at the terminal site
         wsf::Terrain terrain(GetPlatform()->GetTerrain()); // Terrain object to get height at launcher and target
         float        terrainHeight;
         terrain.GetElevInterp(mTerminalLat, mTerminalLon, terrainHeight);
         mTerminalAlt = terrainHeight;
      }
      else
      {
         // CEP offsetting of the target point is not required.
         mTerminalLat = mTargetLat;
         mTerminalLon = mTargetLon;
         mTerminalAlt = mTargetAlt;
      }
   }
}

// =================================================================================================
// virtual
bool WsfTBM_Mover::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if ((!mExplicitStageUsed) && mStageList[0].ProcessInput(aInput))
   {
      mImplicitStageUsed = true;
   }
   else if ((!mImplicitStageUsed) && (command == "stage"))
   {
      UtInputBlock inputBlock(aInput);

      int stageNumber;
      aInput.ReadValue(stageNumber);
      aInput.ValueInClosedRange(stageNumber, 1, static_cast<int>(mStageList.size()) + 1);
      if (stageNumber > static_cast<int>(mStageList.size()))
      {
         mStageList.emplace_back();
      }
      while (inputBlock.ReadCommand())
      {
         if (!mStageList[stageNumber - 1].ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      mExplicitStageUsed = true;
   }
   else if (command == "trajectory_type")
   {
      std::string trajectoryType;
      aInput.ReadValue(trajectoryType);
      if (trajectoryType == "lofted")
      {
         mLofted = true;
      }
      else if (trajectoryType == "depressed")
      {
         mLofted = false;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "position")
   {
      aInput.ReadValueOfType(mLaunchLat, UtInput::cLATITUDE);
      aInput.ReadValueOfType(mLaunchLon, UtInput::cLONGITUDE);
   }
   else if (command == "altitude")
   {
      aInput.ReadValueOfType(mLaunchAlt, UtInput::cLENGTH);
      std::string altRef;
      aInput.ReadCommand(altRef);
      if (altRef == "agl")
      {
         mLaunchAltAGL = true;
      }
      else if (altRef == "msl")
      {
         mLaunchAltAGL = false;
      }
      else
      {
         aInput.PushBack(altRef);
      }
   }
   else if (command == "target_position")
   {
      aInput.ReadValueOfType(mTargetLat, UtInput::cLATITUDE);
      aInput.ReadValueOfType(mTargetLon, UtInput::cLONGITUDE);
   }
   else if (command == "show_status")
   {
      mShowStatus = true;
   }
   else if (command == "debug_call_trigger")
   {
      aInput.ReadValue(mDebugCallTrigger);
   }
   else if (command == "cep")
   {
      aInput.ReadValueOfType(mFixedCEP, UtInput::cLENGTH);
   }
   else if (ProcessCepTable(aInput))
   {
   }
   else
   {
      myCommand = WsfMover::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
bool WsfTBM_Mover::ProcessCepTable(UtInput& aInput)
{
   bool        myCommand = false;
   std::string command(aInput.GetCommand());

   if (command == "cep_table")
   {
      myCommand = true;

      std::vector<double> nominalRanges;
      std::vector<double> rangeErrors;
      std::vector<double> azimuthErrors;

      mNomRanges.SetValues(nominalRanges); // Clear any existing values ...
      mRangeErrors.Resize(0);              // ...
      mAzimuthErrors.Resize(0);            // ...

      double lastRange      = -FLT_EPSILON; // number slightly less than zero.
      bool   formatIsFrozen = false;

      UtInputBlock inputBlock(aInput);

      double nominalRange = cNOT_SET;
      double rangeError   = cNOT_SET;
      double azimuthError = cNOT_SET;

      while (inputBlock.ReadCommand())
      {
         std::string newCommand(inputBlock.GetCommand());

         if (newCommand == "range")
         {
            if (nominalRange != cNOT_SET)
            {
               // This is the beginning of inputting a new CEP point.
               // so save off the old one.
               if (SaveCepPoint(formatIsFrozen, nominalRanges, rangeErrors, azimuthErrors, nominalRange, rangeError, azimuthError))
               {
                  if (nominalRange <= lastRange)
                  {
                     throw UtInput::BadValue(inputBlock.GetInput(),
                                             "CEP Nominal Range Values must be constantly increasing.");
                  }
                  lastRange = nominalRange;
               }
            }

            aInput.ReadValueOfType(nominalRange, UtInput::cLENGTH);
            aInput.ValueGreaterOrEqual(nominalRange, 0.0);
         }
         else if ((newCommand == "range_error") || (newCommand == "cep"))
         {
            aInput.ReadValueOfType(rangeError, UtInput::cLENGTH);
            aInput.ValueGreaterOrEqual(rangeError, 0.0);
         }
         else if (newCommand == "azimuth_error")
         {
            aInput.ReadValueOfType(azimuthError, UtInput::cANGLE);
            aInput.ValueGreaterOrEqual(azimuthError, 0.0);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }

      if (formatIsFrozen &&
          SaveCepPoint(formatIsFrozen, nominalRanges, rangeErrors, azimuthErrors, nominalRange, rangeError, azimuthError))
      {
         if (nominalRange <= lastRange)
         {
            throw UtInput::BadValue(inputBlock.GetInput(), "CEP Nominal Range Values must be constantly increasing.");
         }
      }

      mNomRanges.SetValues(nominalRanges);
      mRangeErrors.Resize(static_cast<int>(nominalRanges.size()));
      if (mAzErrorUsed)
      {
         mAzimuthErrors.Resize(static_cast<int>(nominalRanges.size()));
      }
      for (unsigned int i = 0; i != static_cast<unsigned int>(mNomRanges.GetSize()); ++i)
      {
         mRangeErrors.Set(rangeErrors[i], i);
         if (mAzErrorUsed)
         {
            mAzimuthErrors.Set(azimuthErrors[i], i);
         }
      }
   }

   return myCommand;
}

// =================================================================================================
bool WsfTBM_Mover::SaveCepPoint(bool&                aFormatIsFrozen,
                                std::vector<double>& aNominalRanges,
                                std::vector<double>& aRangeErrors,
                                std::vector<double>& aAzimuthErrors,
                                double               aNominalRange,
                                double               aRangeError,
                                double               aAzimuthError)
{
   bool saved = false;

   if (aNominalRange != cNOT_SET)
   {
      // This is the beginning of inputting a new CEP point.
      // so save off the old one.
      if (!aFormatIsFrozen)
      {
         aFormatIsFrozen = true;
         mAzErrorUsed    = !(aAzimuthError == cNOT_SET);
      }

      aNominalRanges.push_back(aNominalRange);
      aRangeErrors.push_back(aRangeError);
      if (mAzErrorUsed)
         aAzimuthErrors.push_back(aAzimuthError);
      saved = true;
   }
   return saved;
}

// =================================================================================================
// virtual
void WsfTBM_Mover::Update(double aSimTime)
{
   while ((aSimTime - mLastUpdateTime) > mUpdateTimeTolerance)
   {
      double begTime = mLastUpdateTime - mLaunchTime;
      double endTime = aSimTime - mLaunchTime;
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "WsfTBM_Mover::Update:";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Update DT: " << endTime - begTime;
      }

      // Determine if it is time to switch stages.
      while (endTime > mStageList[mStageIndex].mSeparationTime)
      {
         ++mStageIndex;
         mStage = mStageList[mStageIndex];
         SetMasses();
         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "Platform is switching stage:";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Stage: " << mStageIndex + 1;
         }
      }

      if (endTime < mStage.mIgnitionTime)
      {
         if (begTime < mStage.mInitialTime)
         {
            begTime = mStage.mInitialTime;
            CopyState(mState, mStage.mInitialState);
            if (DebugEnabled() && (mStageIndex > 0))
            {
               auto out = ut::log::debug() << "Platform separation of stage.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Platform: " << GetPlatform()->GetName();
               out.AddNote() << "Stage: " << mStageIndex;
            }
         }
      }
      else if (endTime < mStage.mBurnOutTime)
      {
         if (begTime < mStage.mIgnitionTime)
         {
            begTime = mStage.mIgnitionTime;
            CopyState(mState, mStage.mIgnitionState);
            if (DebugEnabled())
            {
               auto out = ut::log::debug() << "Platform ignition of stage.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Platform: " << GetPlatform()->GetName();
               out.AddNote() << "Stage: " << mStageIndex + 1;
            }
         }
      }
      else
      {
         if (begTime < mStage.mBurnOutTime)
         {
            begTime = mStage.mBurnOutTime;
            CopyState(mState, mStage.mBurnOutState);
            if (DebugEnabled())
            {
               auto out = ut::log::debug() << "Platform burnout of stage.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Platform: " << GetPlatform()->GetName();
               out.AddNote() << "Stage: " << mStageIndex + 1;
            }
            if (mStageIndex == (mStageList.size() - 1))
            {
               // The last stage has burned out.
               // Indicate that we are "coasting" ballistically
               mIsBallistic = true;
            }
         }
      }

      // Integrate the state vector from last time to the current time.

      odeint2(mState, begTime, endTime);
      mLastUpdateTime = endTime + mLaunchTime;
   }

   // Set the position
   double locationWCS[3] = {mState[0], mState[2], mState[4]};
   double velocityWCS[3] = {mState[1], mState[3], mState[5]};
   GetPlatform()->SetLocationWCS(locationWCS);
   GetPlatform()->SetVelocityWCS(velocityWCS);
   double accelerationWCS[3] = {mState[6], mState[7], mState[8]};
   GetPlatform()->SetAccelerationWCS(accelerationWCS);

   // Determine the orientation.  We assume the ECS X axis is aligned with the velocity vector.

   // Compute the WCS->ECS coordinate transform matrix.
   // Remember that the rows are simply the basis vector of the local frame in the WCS frame.

   double transformWCSToECS[3][3];

   // The ECS X axis is simply the velocity vector

   UtVec3d::Set(transformWCSToECS[0], velocityWCS);
   UtVec3d::Normalize(transformWCSToECS[0]);

   // The temporary ECS Z axis is the vector that points from the body to the
   // earth center (just the reverse of the ECEF position vector).
   UtVec3d::Multiply(transformWCSToECS[2], locationWCS, -1.0);
   UtVec3d::Normalize(transformWCSToECS[2]);

   // The ECS Y axis is computed as Z cross X

   UtVec3d::CrossProduct(transformWCSToECS[1], transformWCSToECS[2], transformWCSToECS[0]);
   UtVec3d::Normalize(transformWCSToECS[1]);

   // The ECS Z axis is just X cross Y

   UtVec3d::CrossProduct(transformWCSToECS[2], transformWCSToECS[0], transformWCSToECS[1]);
   UtVec3d::Normalize(transformWCSToECS[2]);

   // Extract the Euler angles

   double psi;
   double theta;
   double phi;
   UtEntity::ExtractEulerAngles(transformWCSToECS, psi, theta, phi);

   // And set the orientation

   GetPlatform()->SetOrientationWCS(psi, theta, phi);

   // Set the appearance
   SetTrailingEffects(aSimTime);

   // At this point we should check for a collision with the ground, but this
   // is left an object like WsfWeaponFuse.  This gives the user more control
   // of detonation.

   DO_DIAG(OutputDiagnostic(aSimTime));

   mLastUpdateTime = aSimTime;
}

// =================================================================================================
// static
ut::Random& WsfTBM_Mover::GetRandom()
{
   if (sRandomPtr == nullptr)
   {
      sRandomPtr = new ut::Random();
   }
   return *sRandomPtr;
}

// =================================================================================================
// static
void WsfTBM_Mover::ClearRandom()
{
   delete sRandomPtr;
   sRandomPtr = nullptr;
}


// =================================================================================================
// virtual
void WsfTBM_Mover::SetTrailingEffects(double aSimTime) const
{
   // If not being over-ridden by script
   if (!GetPlatform()->ScriptOverride(7, 2))
   {
      bool inBoost(true);

      double       endTime(aSimTime - mLaunchTime);
      unsigned int stageListSize(static_cast<unsigned int>(mStageList.size()));
      if (endTime >= mStageList[stageListSize - 1].mBurnOutTime)
      {
         inBoost = false;
      }

      if (inBoost)
      {
         GetPlatform()->SetAppearance(7, 2, 3); // 3 - Large
      }
      else
      {
         GetPlatform()->SetAppearance(7, 2, 0); // 0 - None
      }
   }
}

// =================================================================================================
void WsfTBM_Mover::GetLaunchLLA(double& aLat, double& aLon, double& aAlt, bool& aAltAGL)
{
   aLat    = mLaunchLat;
   aLon    = mLaunchLon;
   aAlt    = mLaunchAlt;
   aAltAGL = mLaunchAltAGL;
}

// =================================================================================================
void WsfTBM_Mover::SetLaunchLLA(double aLat, double aLon, double aAlt, bool aAltAGL)
{
   mLaunchLat    = aLat;
   mLaunchLon    = aLon;
   mLaunchAlt    = aAlt;
   mLaunchAltAGL = aAltAGL;
}

// =================================================================================================
std::vector<double> WsfTBM_Mover::GetState() const
{
   // Put the state into a vector for ease of use
   std::vector<double> returnVector;
   for (unsigned int i = 0; i < cSTATE_VECTOR_SIZE; ++i)
   {
      returnVector.push_back(mState[i]);
   }

   return returnVector;
}

// =================================================================================================
void WsfTBM_Mover::SetState(const std::vector<double>& aStateVector)
{
   // Initialize the vector
   double dState[cSTATE_VECTOR_SIZE] = {0.0};

   // Put the state into a vector for ease of use
   for (unsigned int i = 0; i < cSTATE_VECTOR_SIZE; ++i)
   {
      dState[i] = aStateVector[i];
   }

   // Set the class data
   CopyState(mState, dState);
}

// =================================================================================================
void WsfTBM_Mover::SetStageIndex(unsigned int aStageIndex)
{
   // Default the stage index to 0
   mStageIndex = 0;

   // if passed a valid index
   if (aStageIndex < mStageList.size())
   {
      mStageIndex = aStageIndex;
   }

   // Set the current stage
   mStage = mStageList[mStageIndex];
   SetMasses();
}

// =================================================================================================
void WsfTBM_Mover::GetTerminalLLA(double& aLat, double& aLon, double& aAlt)
{
   aLat = mTerminalLat;
   aLon = mTerminalLon;
   aAlt = mTerminalAlt;
}

// =================================================================================================
void WsfTBM_Mover::GetTargetLLA(double& aLat, double& aLon, double& aAlt)
{
   aLat = mTargetLat;
   aLon = mTargetLon;
   aAlt = mTargetAlt;
}

// =================================================================================================
void WsfTBM_Mover::SetTargetLLA(double aLat, double aLon, double aAlt)
{
   mTargetLat = aLat;
   mTargetLon = aLon;
   mTargetAlt = aAlt;
}

// =================================================================================================
//! Given the launch angle, compute the ground range and time of flight.
//! @param psi     [input]  Launch elevation angle (radians)
//! @param rangegr [output] The ground range to the impact point.
//! @param xmid    [output] The time-of-flight to the impact point.
//! @param aMaxAlt [output] The maximum altitude that was achieved.
// protected
void WsfTBM_Mover::ComputeImpactRange(double psi, double& rangegr, double& xmid, double& aMaxAlt)
{
   double              y[9];
   static const double tol = 1.0; // Tolerance for altitude calculations

   ++mDebugCallCount;
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << " ComputeImpactRange (Enter):";
      out.AddNote() << "Launch Angle: " << psi * UtMath::cDEG_PER_RAD << " deg";
      out.AddNote() << "Call Count: " << mDebugCallCount;
   }

   // Compute the state vector for each flight state
   CreateInitialState(psi, y);
   ComputeTransitionStateVectors(y);

   // This section roughly brackets the time the TBM impacts the earth.
   // NOTE: the TBM may never impact the earth, so we have to prepare for that case!

   double tstp = 100.0; // initial bracket width (sec)
line20:
   mStageIndex = 0;
   mStage      = mStageList[mStageIndex];
   SetMasses();
   CopyState(y, mStage.mInitialState);

   double time           = 0.0;
   int    timindx        = 0;              // time step counter
   double told           = 0.0;            // previous time
   double alt            = GetAltitude(y); // altitude (m)
   double lastAlt        = alt - 1.0;      // must go through the loop once...
   double maxAlt         = -1.0E+23;
   double lastSlantRange = -1.0E23;

   // Loop until the TBM passes below the target altitude on the downward path.
   while ((alt > mTargetAlt) || // loop until TBM hits the earth
          (alt > lastAlt))
   {
      if (mDebugCallCount == mDebugCallTrigger)
      {
         WriteStateForPlot(time, y);
      }

      ++timindx;                                  // next time
      time = static_cast<double>(timindx) * tstp; // compute time

      // Determine if it is time to switch stages.
      while (time > mStageList[mStageIndex].mSeparationTime)
      {
         ++mStageIndex;
         mStage = mStageList[mStageIndex];
         SetMasses();
      }

      // Determine where in the stage phase we're at and if it spans a phase boundary.
      // (i.e.: was the last time during pre-burn coast and this time in burn, etc.)

      if (time < mStage.mIgnitionTime)
      {
         if (told < mStage.mInitialTime)
         {
            told = mStage.mInitialTime;
            CopyState(y, mStage.mInitialState);
         }
      }
      else if (time < mStage.mBurnOutTime)
      {
         if (told < mStage.mIgnitionTime)
         {
            told = mStage.mIgnitionTime;
            CopyState(y, mStage.mIgnitionState);
         }
      }
      else
      {
         if (told < mStage.mBurnOutTime)
         {
            told = mStage.mBurnOutTime;
            CopyState(y, mStage.mBurnOutState);
         }
      }

      // Integrate the TBM state vector from last time to this time

      odeint2(y, told, time);

      // Check to see if we've come close to going to the other side of the earth.
      // If so then we have to abort this pass because we'll never intercept.
      //
      // This also checks to if so much has been given that we're probably escaping.

      double wpnLocWCS[3] = {y[0], y[2], y[4]};
      double wpnRelLocWCS[3];
      GetPlatform()->GetRelativeLocationWCS(wpnLocWCS, wpnRelLocWCS);
      double slantRange = UtVec3d::Magnitude(wpnRelLocWCS);
      if (((slantRange > 10.0E6) && (slantRange < lastSlantRange)) ||
          (slantRange > 3.5E7)) // <- approx 22,300 mi (geosynchronous orbit)
      {
         xmid    = told;
         rangegr = lastSlantRange;
         aMaxAlt = maxAlt;
         if (DebugEnabled())
         {
            auto out  = ut::log::debug() << "Range/altitude limit exceeded.";
            auto note = out.AddNote() << "ComputeImpactRange (Exit):";
            note.AddNote() << "Launch Angle: " << psi * UtMath::cDEG_PER_RAD << " deg";
            note.AddNote() << "Range: " << rangegr;
            note.AddNote() << "Max Alt: " << maxAlt;
            note.AddNote() << "Time: " << xmid;
         }
         return;
      }
      lastSlantRange = slantRange;

      lastAlt = alt;
      alt     = GetAltitude(y); // TBM altitude
      maxAlt  = std::max(alt, maxAlt);
      told    = time; // save old time
   }                  // loop until altitude is less than target altitude
                      //   the value in the variable time is a time
                      //   after the TBM has impacted the earth

   // If the TBM hit the earth on the first time step, reduce the time step by a factor of 10 and try again.

   if (timindx == 1)
   {
      tstp = tstp / 10.0;
      goto line20;
   }

   // If the TBM never reached the altitude of the target, return the last solution

   if (maxAlt < mTargetAlt)
   {
      xmid    = told;
      rangegr = lastSlantRange;
      aMaxAlt = maxAlt;
      if (DebugEnabled())
      {
         auto out  = ut::log::debug() << "Never descended on target.";
         auto note = out.AddNote() << "ComputeImpactRange (Exit):";
         note.AddNote() << "Launch Angle: " << psi * UtMath::cDEG_PER_RAD << " deg";
         note.AddNote() << "Range: " << rangegr;
         note.AddNote() << "Max Alt: " << maxAlt;
         note.AddNote() << "Time: " << xmid;
      }
      return;
   }

   // The TBM impact occurs at a time between time and time - tstp.
   // This section determines the impact time more accurately.

   double xlow  = time - tstp; // low end of impact bracket
   double xhigh = time;        // high end of impact bracket
   alt          = 1.0E23;
   while (fabs(alt - mTargetAlt) > tol)
   {
      xmid = (xlow + xhigh) / 2.0; // mid point of bracket
      time = xmid;

      // Determine the stage of the midpoint.

      mStageIndex           = 0;
      bool stageIncremented = false;
      while (time > mStageList[mStageIndex].mSeparationTime)
      {
         ++mStageIndex;
         stageIncremented = true;
      }
      mStage = mStageList[mStageIndex];
      if (stageIncremented)
      {
         SetMasses();
      }

      // Compute the TBM state vector at the midpoint.

      if (time < mStage.mIgnitionTime)
      {
         told = mStage.mInitialTime;
         CopyState(y, mStage.mInitialState);
      }
      else if (time < mStage.mBurnOutTime)
      {
         told = mStage.mIgnitionTime;
         CopyState(y, mStage.mIgnitionState);
      }
      else
      {
         told = mStage.mBurnOutTime;
         CopyState(y, mStage.mBurnOutState);
      }

      // Do integration to compute TBM state vector at xmid

      odeint2(y, told, xmid);
      alt = GetAltitude(y); // compute TBM altitude

      // If altitude > 0 then xmid represents the new low end of the
      // bracket, otherwise it is the new high end of the bracket.
      // continue until the impact is bracket by tol meters in altitude

      if (alt > mTargetAlt)
      {
         xlow = xmid;
      }
      else
      {
         xhigh = xmid;
      }
   }

   // Compute the impact range from the launch point

   double impactLocWCS[3] = {y[0], y[2], y[4]};
   double relImpactLocWCS[3];
   GetPlatform()->GetRelativeLocationWCS(impactLocWCS, relImpactLocWCS);
   rangegr = UtVec3d::Magnitude(relImpactLocWCS);
   aMaxAlt = maxAlt;
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "ComputeImpactRange (Exit):";
      out.AddNote() << "Launch Angle: " << psi * UtMath::cDEG_PER_RAD << " deg";
      out.AddNote() << "Range: " << rangegr;
      out.AddNote() << "Max Alt: " << maxAlt;
      out.AddNote() << "Time: " << xmid;
   }
}

// =================================================================================================
//! Given an initial state vector, compute the state vectors at the transitions.
// protected
void WsfTBM_Mover::ComputeTransitionStateVectors(const double aInitialState[9])
{
   double currentState[cSTATE_VECTOR_SIZE];
   CopyState(currentState, aInitialState);

   // Determine the TBM state vector at each transition in the flight.

   for (unsigned int stageIndex = 0; stageIndex < mStageList.size(); ++stageIndex)
   {
      Stage& stage = mStageList[stageIndex];
      mStage       = stage; // Also make a copy for the integrator to access things like wdot...
      SetMasses();
      CopyState(stage.mInitialState, currentState);

      // Integrate through the pre-burn coast time (if any).

      if (stage.mIgnitionTime > stage.mInitialTime)
      {
         odeint2(currentState, stage.mInitialTime, stage.mIgnitionTime);
      }
      CopyState(stage.mIgnitionState, currentState);

      // Integrate through the engine burn.

      if (stage.mBurnOutTime > stage.mIgnitionTime)
      {
         odeint2(currentState, stage.mIgnitionTime, stage.mBurnOutTime);
      }
      CopyState(stage.mBurnOutState, currentState);

      // Integrate through the post-burn coast time (if any).

      if (((stageIndex + 1) < mStageList.size()) && (stage.mSeparationTime > stage.mBurnOutTime))
      {
         odeint2(currentState, stage.mBurnOutTime, stage.mSeparationTime);
      }
      if (DebugEnabled())
      {
         auto out  = ut::log::debug() << "State at end of stage.";
         auto note = out.AddNote() << "Stage: " << stageIndex + 1;
         PrintState(currentState, note);
      }
   }
}

// =================================================================================================
//! Create the initial state vector for a launch elevation.
//!
//! Given the WCS positions of the launcher and the target, and the elevation
//! angle of the launcher, compute the initial state vector.
//!
//! @param aElevation [input] launch elevation angle from local horizon. (radians)
//! @param aState     [output] The initial state vector.
// private
void WsfTBM_Mover::CreateInitialState(double aElevation, double aState[9])
{
   double launchLocationWCS[3];
   UtEntity::ConvertLLAToWCS(mLaunchLat, mLaunchLon, mLaunchAlt, launchLocationWCS);
   aState[0] = launchLocationWCS[0];
   aState[2] = launchLocationWCS[1];
   aState[4] = launchLocationWCS[2];

   // Compute the WCS->Local coordinate transform matrix.
   // Remember that the rows are simply the basis vector of the local frame in the WCS frame.

   double transformWCSToECS[3][3];

   // Z axis is in the direction of the position vector to the launch location
   UtVec3d::Set(transformWCSToECS[2], launchLocationWCS);
   UtVec3d::Normalize(transformWCSToECS[2]);

   // The temporary X axis is in the direction of the target
   double targetLocationWCS[3];
   UtEntity::ConvertLLAToWCS(mTerminalLat, mTerminalLon, mTerminalAlt, targetLocationWCS);
   UtVec3d::Subtract(transformWCSToECS[0], targetLocationWCS, launchLocationWCS);
   UtVec3d::Normalize(transformWCSToECS[0]);

   // The Y axis points to the left
   UtVec3d::CrossProduct(transformWCSToECS[1], transformWCSToECS[2], transformWCSToECS[0]);
   UtVec3d::Normalize(transformWCSToECS[1]);

   // X = Y cross Z
   UtVec3d::CrossProduct(transformWCSToECS[0], transformWCSToECS[1], transformWCSToECS[2]);
   UtVec3d::Normalize(transformWCSToECS[0]);

   // Compute the local velocity vector and transform into the WCS frame

   double localVel[3] = {cos(aElevation), 0.0, sin(aElevation)};
   double velocityWCS[3];
   UtMat3d::InverseTransform(velocityWCS, transformWCSToECS, localVel);
   UtVec3d::Multiply(velocityWCS, 10.0); // Must have some small velocity
   aState[1] = velocityWCS[0];
   aState[3] = velocityWCS[1];
   aState[5] = velocityWCS[2];

   // Set initial acceleration to 0 m/s^2
   aState[6] = 0.0;
   aState[7] = 0.0;
   aState[8] = 0.0;
}

// =================================================================================================
//  ****************************************************************
//  this subroutine determines the launch angle required for a given
//  TBM range and launch bearing angle. inputs are the desired TBM range,
//  the TBM maximum range, launch bearing angle,
//  and the launch angle which produces the maximum range trajectory.
//  if a lofted trajectory is chosen the launch angle will be greater
//  than the maximum range angle.  the launch angle will be lower than the
//  maximum range angle for a depressed trajectory.  outputs are the required
//  launch angle, the total TBM flight time, and a flag indicating if
//  this TBM can go the required distance.
//  added launch bearing angle dependency for non-spherical earth model.
//  ****************************************************************

//  Inputs:
//     fltran     desired flight ground range (m)
//     psicrit    launch angle for max range (rad)
//     rngmax     max range (m)
//  Outputs:
//     flttim     total TBM time of flight (sec)
//     psi        required launch angle (rad)
//     toofar     true if desired range > max range

void WsfTBM_Mover::ComputeLaunchAngle(double  fltran,
                                      double  psicrit,
                                      double  rngmax,
                                      double& flttim,
                                      double& psi,
                                      double& maxAlt,
                                      bool&   toofar)

{
   maxAlt = 0.0;
   if (fltran > rngmax)
   {
      // range too great
      toofar = true;
      psi    = psicrit;
      flttim = 0.0;
   }
   else
   {
      // range not too great
      toofar = false;

      double low;           // low end of launch angle bracket (rad)
      double high;          // high end of launch angle bracket (rad)
      double rangegr = 0.0; // ground range (m)
      double tol     = 10.0;
      ; // precision of computed ground range (m)

      //  set the initial launch angle bracket depending on lofted
      //  or depressed trajectory

      if (mLofted)
      {
         low  = psicrit;
         high = UtMath::cPI_OVER_2;
      }
      else
      {
         low  = 0.0;
         high = psicrit;
      }

      //  loop until the achieved and desired ground ranges are
      //  within tol of each other
      double prevPsi = 0.0;
      while (fabs(fltran - rangegr) > tol)
      {
         psi = (low + high) / 2.0; // center of bracket
         //  for small max ranges, it is possible that no calculated range is within 10 meters of the desired range
         //  (even if there theoretically should be), so break if no solution is found
         if (UtMath::NearlyEqual(prevPsi, psi))
         {
            break;
         }
         prevPsi = psi;
         ComputeImpactRange(psi, rangegr, flttim, maxAlt);

         //  reset the bracket limits based on the result

         if ((rangegr - fltran) < 0.)
         {
            if (mLofted)
            {
               high = psi;
            }
            else
            {
               low = psi;
            }
         }
         else
         {
            if (mLofted)
            {
               low = psi;
            }
            else
            {
               high = psi;
            }
         }
      }
   }
}

// =================================================================================================
//  *****************************************************************
//  this subroutine is used to determine the maximum range a TBM with
//  the input characteristics can go.  it also returns the launch angle
//  needed to produce the maximum range trajectory at the given launch
//  bearing angle.  the algorithm
//  used is brent's method and is described in chapter 10 of "numerical
//  recipes", by press, flannery, teukolsky, and vetterling, cambridge
//  university press, 1986.  ax and cx are launch angles which bracket
//  the launch angle which produces the maximum range trajectory.  bx
//  must be between ax and cx.  ax is set to 0 degrees, bx to 45 degrees,
//  and cx to 90 degrees when the routine is called.  the launch angle which
//  produces the maximum range trajectory is returned in xmin (radians).
//  the maximum possible ground range this TBM can cover is returned in ymin
//  (meters).  the subroutine gr is used to determine the ground range
//  the TBM will cover given a launch angle.  the input tol is the
//  approximate precision this routine will find the maximum to.
//  *****************************************************************

void WsfTBM_Mover::ComputeMaximumRange(double ax, double bx, double cx, double tol, double& xmin, double& ymin)
{
   if (DebugEnabled())
   {
      ut::log::debug() << "ComputeMaximumRange: enter";
   }
   double cgold;
   double zeps;
   double a;
   double b;
   double v;
   double w;
   double x;
   double e;
   double fx;
   double xmid;
   double fv;
   double fw;
   double xm;
   double tol1;
   double tol2;
   double r;
   double q;
   double p;
   double etemp;
   double d = 0.0;
   double u;
   double fu;

   int itmax;
   int iter;

   itmax = 100;
   cgold = 0.381966;
   zeps  = 1.0e-10;
   iter  = 0;

   a = ax;
   b = cx;
   v = bx;
   w = v;
   x = v;
   e = 0.0;
   double maxAlt;
   ComputeImpactRange(x, fx, xmid, maxAlt);

   fx = -fx;
   fv = fx;
   fw = fx;
   for (iter = 0; iter < itmax; ++iter)
   {
      xm   = 0.5 * (a + b);
      tol1 = tol * fabs(x) + zeps;
      tol2 = 2. * tol1;
      if (fabs(x - xm) <= (tol2 - .5 * (b - a)))
         goto line3;
      if (fabs(e) > tol1)
      {
         r = (x - w) * (fx - fv);
         q = (x - v) * (fx - fw);
         p = (x - v) * q - (x - w) * r;
         q = 2. * (q - r);
         if (q > 0.)
         {
            p = -p;
         }
         q     = fabs(q);
         etemp = e;
         e     = d;
         if ((fabs(p) >= fabs(.5 * q * etemp)) || (p <= q * (a - x)) || (p >= q * (b - x)))
            goto line1;
         d = p / q;
         u = x + d;
         if (((u - a) < tol2) || ((b - u) < tol2))
         {
            d = sign(tol1, xm - x);
         }
         goto line2;
      }
   line1:
      if (x >= xm)
      {
         e = a - x;
      }
      else
      {
         e = b - x;
      }
      d = cgold * e;
   line2:
      if (fabs(d) >= tol1)
      {
         u = x + d;
      }
      else
      {
         u = x + sign(tol1, d);
      }

      ComputeImpactRange(u, fu, xmid, maxAlt);
      fu = -fu;
      if (fu <= fx)
      {
         if (u >= x)
         {
            a = x;
         }
         else
         {
            b = x;
         }
         v  = w;
         fv = fw;
         w  = x;
         fw = fx;
         x  = u;
         fx = fu;
      }
      else
      {
         if (u < x)
         {
            a = u;
         }
         else
         {
            b = u;
         }
         if ((fu <= fw) || (w == x))
         {
            v  = w;
            fv = fw;
            w  = u;
            fw = fu;
         }
         else if ((fu <= fv) || (v == x) || (v == w))
         {
            v  = u;
            fv = fu;
         }
      }
   }

   { // RAII block
      auto out = ut::log::error() << "Platform exceeded iteration count determining maximum range.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
   }

line3:
   xmin = x;
   ymin = -fx;
   if (DebugEnabled())
   {
      ut::log::debug() << "ComputeMaximumRange: exit";
   }
}

// =================================================================================================
//! Get the geocentric altitude above the ellipsoid for the specified WCS location.
// private
double WsfTBM_Mover::GetAltitude(const double aState[9]) const
{
   static const double cCONST_1 = UtEarth::cA * UtEarth::cA * (1.0 - UtEarth::cE_2);
   double              x        = aState[0];
   double              y        = aState[2];
   double              z        = aState[4];

   // Determine the geocentric distance^2 to the point and the distance^2 in the X-Y plane.
   double rXY_2  = (x * x) + (y * y);
   double rXYZ_2 = rXY_2 + (z * z);

   // Determine the cos^2 of the angle between the X-Y plane and the point
   double cosAngle_2 = rXY_2 / rXYZ_2;

   // Determine the geocentric distance^2 at which the position vector intersects the ellipsoid.
   double rEllipsoid_2 = cCONST_1 / (1.0 - UtEarth::cE_2 * cosAngle_2);

   return sqrt(rXYZ_2) - sqrt(rEllipsoid_2);
}

// =================================================================================================
// private
double WsfTBM_Mover::GetSpeed(const double aState[9]) const
{
   return sqrt(aState[1] * aState[1] + aState[3] * aState[3] + aState[5] * aState[5]);
}

// =================================================================================================
// private
void WsfTBM_Mover::PrintState(const double aState[9], ut::log::MessageStream& aStream) const
{
   double wcs[3];
   double lat;
   double lon;
   double alt;
   wcs[0] = aState[0];
   wcs[1] = aState[2];
   wcs[2] = aState[4];
   UtEntity::ConvertWCSToLLA(wcs, lat, lon, alt);
   aStream.AddNote() << "Altitude: " << alt * 0.001 << " km (" << alt / UtMath::cM_PER_MI << " mi)";
   wcs[0]       = aState[1];
   wcs[1]       = aState[3];
   wcs[2]       = aState[5];
   double speed = UtVec3d::Magnitude(wcs);
   aStream.AddNote() << "Speed: " << speed << " m/s (" << speed * 3600.0 / UtMath::cM_PER_MI << " mph)";
   wcs[0] = aState[6];
   wcs[1] = aState[7];
   wcs[2] = aState[8];
   speed  = UtVec3d::Magnitude(wcs);
   aStream.AddNote() << "Acceleration: " << speed << " m/s2 (" << speed * pow(3600.0 / UtMath::cM_PER_MI, 2.0) << " mph2)";
}

// =================================================================================================
//! Schedule the events for ignition, burnout and separation.
//! We schedule these ahead of time to simplify the logic in Update so it doesn't have to worry
//! about transitioning between multiple phases. It also eliminates the worry about recursive
//! calls that may be triggered by calling a script from with the platform Update method.
// private
void WsfTBM_Mover::ScheduleScriptEvents(double aSimTime)
{
   // Note - if the events times for adjacent events are equal then we add a little
   // bit to it. The event manager doesn't guarantee that two events scheduled for
   // the same time with dispatch in the order scheduled.

   UtScriptDataList scriptArgs;
   UtScriptData     stageNumber(1);
   scriptArgs.push_back(stageNumber);
   std::string stageIgnitionId   = "on_stage_ignition";
   std::string stageBurnoutId    = "on_stage_burnout";
   std::string stageSeparationId = "on_stage_separation";
   double      eventTimeShift    = 1.0E-5;
   double      lastEventTime     = -1.0;
   double      eventTime;
   for (unsigned int stageIndex = 1; stageIndex <= mStageList.size(); ++stageIndex)
   {
      Stage& stage = mStageList[stageIndex - 1];
      scriptArgs[0].SetInt(stageIndex);
      if (stage.mThrustDuration > 0.0)
      {
         if (GetPlatform()->HasScript(stageIgnitionId))
         {
            eventTime = mLaunchTime + stage.mIgnitionTime;
            if (eventTime <= lastEventTime)
            {
               eventTime = lastEventTime + eventTimeShift;
            }
            lastEventTime = eventTime;
            GetPlatform()->ExecuteScriptAtTime(eventTime, stageIgnitionId, scriptArgs);
         }
         if (GetPlatform()->HasScript(stageBurnoutId))
         {
            eventTime = mLaunchTime + stage.mBurnOutTime;
            if (eventTime <= lastEventTime)
            {
               eventTime = lastEventTime + eventTimeShift;
            }
            lastEventTime = eventTime;
            GetPlatform()->ExecuteScriptAtTime(eventTime, stageBurnoutId, scriptArgs);
         }
      }
      if (stageIndex != mStageList.size())
      {
         if (GetPlatform()->HasScript(stageSeparationId))
         {
            eventTime = mLaunchTime + stage.mSeparationTime;
            if (eventTime <= lastEventTime)
            {
               eventTime = lastEventTime + eventTimeShift;
            }
            lastEventTime = eventTime;
            GetPlatform()->ExecuteScriptAtTime(eventTime, stageSeparationId, scriptArgs);
         }
      }
   }
}

// =================================================================================================
void WsfTBM_Mover::WriteStateForPlot(double aTime, const double aState[9])
{
   double locationWCS[3] = {aState[0], aState[2], aState[4]};
   double velocityWCS[3] = {aState[1], aState[3], aState[5]};

   double weaponLat;
   double weaponLon;
   double weaponAlt;
   UtEntity::ConvertWCSToLLA(locationWCS, weaponLat, weaponLon, weaponAlt);
   double relLocationWCS[3];
   GetPlatform()->GetRelativeLocationWCS(locationWCS, relLocationWCS);
   double slantRange  = UtVec3d::Magnitude(relLocationWCS);
   double weaponSpeed = UtVec3d::Magnitude(velocityWCS);
   double groundRange;
   double heading;
   UtSphericalEarth::GreatCircleHeadingAndDistance(mLaunchLat, mLaunchLon, weaponLat, weaponLon, heading, groundRange);

   std::ofstream ofs;
   if (aTime <= 0.0)
   {
      ofs.open("tbm_plot.txt", std::ios::out | std::ios::trunc);
      ofs << "#-time--\t-g_range\t--alt---\t-speed--\t-s_range\n";
   }
   else
   {
      ofs.open("tbm_plot.txt", std::ios::out | std::ios::app);
   }
   // ofs << fixed;  // does't work with G++ 2.9x
   ofs.setf(std::ios::fixed, std::ios::floatfield);
   ofs << std::setw(8) << aTime;
   ofs << '\t' << std::setw(8) << groundRange / UtMath::cM_PER_MI;
   ofs << '\t' << std::setw(8) << weaponAlt / UtMath::cM_PER_MI;
   ofs << '\t' << std::setw(8) << weaponSpeed * 3600.0 / UtMath::cM_PER_MI;
   ofs << '\t' << std::setw(8) << slantRange / UtMath::cM_PER_MI;
   ofs << '\n';
}

// =================================================================================================
// *********************************************************
// This subroutine implements the fourth order runge-kutta
// algorithm with variable step size to integrate differential
// equations to compute a TBM trajectory.
//
// This algorithm is described in section 15.2 of "Numerical Recipes",
// Press, Flannery, Teukolsky, and Vetterling,
// Cambridge University Press 1986.
//
// This routine calls the subroutines rkqc, rk4, and derivs.
// The routine derivs is user supplied and computes the system
// of differential equations which define the TBM trajectory.
//  *********************************************************
void WsfTBM_Mover::odeint2(double y[9], double x1, double x2)
{
   int    i;
   double h1;
   double hdid;
   double hnext;
   double x;
   double h;
   double yscal[6];
   double y2[9];
   double dydx[6];

   static const int    maxstp = 10000;
   static const double zero   = 0.0;
   static const double tiny   = 1.0E-30;
   static const double eps    = 0.00000001;

   // We can sometimes get a bound case where x1 = x2.
   // We can also get a case where x1 and x2 are very nearly equal.
   if (fabs(x1 - x2) < (eps * 0.01))
   {
      return;
   }

   x  = x1;
   h1 = (x2 - x1) / 100.0;
   h  = sign(h1, x2 - x1);

   CopyState(y2, y);

   for (int nstp = 0; nstp < maxstp; ++nstp)
   {
      derivs(x, y2, dydx);
      for (i = 0; i < 6; ++i)
      {
         yscal[i] = fabs(y2[i]) + fabs(h * dydx[i]) + tiny;
      }

      if ((x + h - x2) * (x + h - x1) > zero)
      {
         h = x2 - x;
      }

      rkqc(y2, dydx, x, h, eps, yscal, hdid, hnext);

      if ((x - x2) * (x2 - x1) >= zero)
      {
         y2[6] = dydx[1]; // x double dot
         y2[7] = dydx[3]; // y double dot
         y2[8] = dydx[5]; // z double dot
         CopyState(y, y2);
         return;
      }

      h = hnext;
   }
}

// =================================================================================================
// ********************************************************************
// This subroutine is used by odeint in integrating the TBM trajectory.
// See section 15.2 of "Numerical Recipes", Press, Flannery, Teukolsky,
// and Vetterling, Cambridge University Press 1986 for details.
// ********************************************************************

void WsfTBM_Mover::rk4(double y[9], double dydx[6], double x, double h, double yout[9])
{
   int    i;
   double yt[9];
   double dyt[6];
   double dym[6];

   double hh = h * 0.5;
   double h6 = h / 6.0;
   double xh = x + hh;
   for (i = 0; i < 6; ++i)
   {
      yt[i] = y[i] + hh * dydx[i];
   }
   derivs(xh, yt, dyt);
   for (i = 0; i < 6; ++i)
   {
      yt[i] = y[i] + hh * dyt[i];
   }
   derivs(xh, yt, dym);
   for (i = 0; i < 6; ++i)
   {
      yt[i]  = y[i] + h * dym[i];
      dym[i] = dyt[i] + dym[i];
   }
   derivs(x + h, yt, dyt);
   for (i = 0; i < 6; ++i)
   {
      yout[i] = y[i] + h6 * (dydx[i] + dyt[i] + 2.0 * dym[i]);
   }
}

// =================================================================================================
// ********************************************************************
// This subroutine is used by odeint in integrating the TBM trajectory.
// See section 15.2 of "Numerical Recipes", Press, Flannery, Teukolsky,
// and Vetterling, Cambridge University Press 1986 for details.
// ********************************************************************
void WsfTBM_Mover::rkqc(double y[9], double dydx[6], double& x, double htry, double eps, double yscal[6], double& hdid, double& hnext)
{
   int    i;
   double xsav;
   double h;
   double errmax;
   double ytemp[6];
   double ysav[6];
   double dysav[6];

   static const double fcor    = 1.0 / 15.0;
   static const double cPGROW  = -0.2;
   static const double cPSHRNK = -0.25;
   static const double cSAFETY = 0.9;
   static const double cERRCON = 6.0E-4;

   xsav = x;
   for (i = 0; i < 6; ++i)
   {
      ysav[i]  = y[i];
      dysav[i] = dydx[i];
   }

   h = htry;
   while (true)
   {
      double hh = 0.5 * h;
      rk4(ysav, dysav, xsav, hh, ytemp);
      x = xsav + hh;
      derivs(x, ytemp, dydx);
      rk4(ytemp, dydx, x, hh, y);
      x = xsav + h;
      if (x == xsav)
      {
         { // RAII block
            auto logError = ut::log::error() << "WsfTBM_Mover: Step not significant in rkqc.";
            logError.AddNote() << "T = " << x;
         }
         abort();
      }
      rk4(ysav, dysav, xsav, h, ytemp);

      errmax = 0.0;
      for (i = 0; i < 6; ++i)
      {
         ytemp[i] = y[i] - ytemp[i];
         errmax   = std::max(errmax, fabs(ytemp[i] / yscal[i]));
      }
      errmax = errmax / eps;
      if (errmax <= 1.0)
      {
         break;
      }
      double htemp = cSAFETY * h * pow(errmax, cPSHRNK);
      if (h >= 0.0)
      {
         h = std::max(htemp, 0.1 * h);
      }
      else
      {
         h = std::min(htemp, 0.1 * h);
      }
   }

   hdid = h;
   if (errmax > cERRCON)
   {
      hnext = cSAFETY * h * pow(errmax, cPGROW);
   }
   else
   {
      hnext = 4.0 * h;
   }

   for (i = 0; i < 6; ++i)
   {
      y[i] = y[i] + ytemp[i] * fcor;
   }
}

// =================================================================================================
// ****************************************************************
// This subroutine computes first and second derivatives of the TBM
// state vector in array y with respect to time, at the time x.  These
// are returned in the dydx array.  This information is used by the
// odeint routine to solve for the TBM state vector as a function of
// time.
//  ****************************************************************
void WsfTBM_Mover::derivs(double x, const double y[9], double dydx[6]) const
{
   // Earth's gravitational constant (GM) from NIMA TR8350.2
   static const double gm   = 3986004.418E+8; // m^3/s^2
   static const double rho0 = 1.752;
   static const double hfac = 6700.0;

   // If still boosting then compute acceleration due to thrust and TBM mass
   // Otherwise, acceleration due to thrust=0 and mass equals the empty mass.

   double mass = mStage.RemainingBurnoutMass();
   double at   = 0.0;
   double wdot = mStage.mWdot;
   double scdo = mStage.mS_Cd0;
   if (x < mStage.mIgnitionTime)
   {
      mass = mStage.RemainingInitialMass();
   }
   else if (x < mStage.mBurnOutTime)
   {
      mass = mStage.RemainingInitialMass() + (wdot / UtEarth::cACCEL_OF_GRAVITY) * (x - mStage.mIgnitionTime);
      at   = -mStage.mSpecificImpulse * wdot / mass;
   }

   //  compute the acceleration due to atmospheric drag
   //  set to 0 for altitudes > 200 km
   //  set to the surface value for negative altitudes
   //  uses a exponitial approximation to compute air density
   //  assumes TBM has constant drag coefficient

   double rho = rho0; // Assume below ground
   double alt = GetAltitude(y);
   if (alt > 200000.0)
   {
      rho = 0.0;
   }
   else if (alt > 0.0)
   {
      rho = rho0 * exp(-alt / hfac);
   }

   double speed = sqrt(y[1] * y[1] + y[3] * y[3] + y[5] * y[5]);
   double ad    = -0.5 * rho * (speed * speed) * scdo / mass;
   double temp1 = (at + ad) / speed;

   // Magnitude of the position vector (i.e.: the distance from the center of the earth)
   double posMag = sqrt(y[0] * y[0] + y[2] * y[2] + y[4] * y[4]);
   double gimme  = posMag * posMag * posMag;
   double rqot   = gm / gimme;

   dydx[0] = y[1];                           // x dot
   dydx[1] = (y[1] * temp1) - (y[0] * rqot); // x doubledot

   dydx[2] = y[3];                           // y dot
   dydx[3] = (y[3] * temp1) - (y[2] * rqot); // y doubledot

   dydx[4] = y[5];                           // z dot
   dydx[5] = (y[5] * temp1) - (y[4] * rqot); // z doubledot

   // cout << x << " "  << dydx[0] << " " << dydx[2] << " " << dydx[4] << " "
   //      << dydx[1] << " " << dydx[3] << " " << dydx[5] << endl;
}

// =================================================================================================
#ifdef DIAG_ON
void WsfTBM_Mover::OutputDiagnostic(double aSimTime)
{ // does the following code need updating?
   cout << "T=" << aSimTime;
   cout << " [" << UtTime(aSimTime, UtTime::FmtHMS + 1) << "]";
   cout << " (" << GetPlatform()->GetName() << '.' << "WsfTBM_Mover) ";
   cout << endl;

   // Location in LLA
   double locLLA[3] = {0.0};
   GetPlatform()->GetLocationLLA(locLLA[0], locLLA[1], locLLA[2]);
   cout << "   LLA ="
        << " " << locLLA[0] << " " << locLLA[1] << " " << locLLA[2];
   if (mLaunchAltAGL)
   {
      cout << " AGL";
   }
   cout << endl;

   // Location in ECI
   // double dECI[3] = {0.0};
   // GetPlatform()->GetLocationECI(dECI);
   // cout << "   Location in ECI ="
   //   << " " << dECI[0]
   //   << " " << dECI[1]
   //   << " " << dECI[2]
   //   << endl;

   //// Velocity in ECI
   // double velECI[3] = {0.0};
   // GetPlatform()->GetVelocityECI(velECI);
   // cout << "   Velocity in ECI ="
   //    << " " << velECI[0]
   //    << " " << velECI[1]
   //    << " " << velECI[2]
   //    << endl;


   // Location and velocity in WCS
   double locWCS[3] = {0.0};
   GetPlatform()->GetLocationWCS(locWCS);
   double velWCS[3] = {0.0};
   GetPlatform()->GetVelocityWCS(velWCS);
   cout << "   WCS ="
        << " [" << locWCS[0] << " " << locWCS[1] << " " << locWCS[2] << "]   ["
        << " " << velWCS[0] << " " << velWCS[1] << " " << velWCS[2] << "] " << GetPlatform()->GetSpeed() << endl;

   if (aSimTime <= 0.0)
   {
      // Orientation
      double dOrientation[3] = {0.0};
      GetPlatform()->GetOrientationNED(dOrientation[0], dOrientation[1], dOrientation[2]);

      cout << "   Orientation ="
           << " " << dOrientation[0] * UtMath::cDEG_PER_RAD << " " << dOrientation[1] * UtMath::cDEG_PER_RAD << endl;
   }
}
#endif

// =================================================================================================
// =================================================================================================
WsfTBM_Mover::Stage::Stage()
   : mBallisticCoeff(0.0)
   , mEmptyMass(0.0)
   , mFuelMass(0.0)
   , mFuelMassFraction(0.0)
   , mTotalMass(0.0)
   , mThrust(0.0)
   , mSpecificImpulse(0.0)
   , mThrustDuration(0.0)
   , mPreIgnitionCoastTime(0.0)
   , mPreSeparationCoastTime(0.0)
   , mCd0(0.0)
   , mReferenceArea(0.0)
   , mWdot(0.0)
   , mPayloadMass(0.0)
   , mS_Cd0(0.0)
   , mInitialTime(0.0)
   , mIgnitionTime(0.0)
   , mBurnOutTime(0.0)
   , mSeparationTime(0.0)
{
   for (size_t i = 0; i < 9; ++i)
   {
      mInitialState[i]  = 0.0;
      mIgnitionState[i] = 0.0;
      mBurnOutState[i]  = 0.0;
   }
}

// =================================================================================================
//! Validate and initialize a stage.
//! @returns 'true' if successful, 'false' if unsuccessful.
bool WsfTBM_Mover::Stage::Initialize(double aInitialTime)
{
   bool ok = true;

   if ((mPreIgnitionCoastTime <= 0.0) && (mThrustDuration <= 0.0) && (mPreSeparationCoastTime <= 0.0))
   {
      ut::log::error()
         << "At least one of 'pre_burn_coast_time', 'burn_time', or 'post_burn_coast_time' must be greater than zero.";
      ok = false;
   }

   if (mTotalMass <= 0.0)
   {
      if ((mEmptyMass == 0.0) && (mFuelMass == 0.0))
      {
         ut::log::error() << "If 'total_mass' is not specified then you must specify 'empty_mass' and 'fuel_mass'.";
         ok = false;
      }
      else
      {
         mTotalMass = mEmptyMass + mFuelMass;
      }
   }

   if ((mFuelMass > 0.0) && (mFuelMassFraction > 0.0))
   {
      ut::log::error() << "'fuel_mass' and 'fuel_mass_fraction' cannot both be non-zero.";
      ok = false;
   }
   else if (mFuelMass <= 0.0)
   {
      mFuelMass = mTotalMass * mFuelMassFraction;
   }
   else
   {
      mFuelMassFraction = mFuelMass / mTotalMass;
   }

   if (mFuelMass >= mTotalMass)
   {
      auto out = ut::log::error() << "'fuel mass' must be less than the total mass.";
      out.AddNote() << "Fuel: " << mFuelMass;
      out.AddNote() << "Total: " << mTotalMass;
      ok = false;
   }

   if (mThrustDuration > 0.0)
   {
      if (mFuelMass <= 0.0)
      {
         ut::log::error() << "'fuel_mass' or 'fuel_mass_fraction' must be specified.";
         ok = false;
      }

      if ((mThrust <= 0.0) && (mSpecificImpulse <= 0.0))
      {
         ut::log::error() << "Either 'thrust' or 'specific_impulse' must be specified.";
         ok = false;
      }
      else if ((mThrust > 0.0) && (mSpecificImpulse > 0.0))
      {
         ut::log::error() << "'thrust' and 'specific_impulse' cannot both be specified.";
         ok = false;
      }
   }

   if (ok)
   {
      //! Compute thrust data.
      if (mThrustDuration > 0.0)
      {
         // The time-rate-of-change of the weight (not mass) changes during engine burn
         // mWdot = - mFuelMassFraction * mTotalMass * UtEarth::cACCEL_OF_GRAVITY / mThrustDuration;
         mWdot = -mFuelMass * UtEarth::cACCEL_OF_GRAVITY / mThrustDuration;

         // The mass of the stage without fuel
         if (mEmptyMass <= 0.0)
         {
            mEmptyMass = mTotalMass - mFuelMass;
         }

         if (mSpecificImpulse <= 0.0)
         {
            // Compute specific impulse from thrust
            mSpecificImpulse = -mThrust / mWdot;
         }
         else
         {
            // Compute thrust from specific impulse (not used except for diagnostic messages)
            mThrust = -mSpecificImpulse * mWdot;
         }
      }
      else
      {
         // Only coasting during this stage.
         mWdot = 0.0;
         if (mEmptyMass < 0.0)
         {
            mEmptyMass = mTotalMass;
         }
         mSpecificImpulse = 0.0;
         mThrust          = 0.0;
      }
      if (fabs(mTotalMass - mEmptyMass - mFuelMass) > 1.0)
      {
         ut::log::error() << "'total_mass', 'empty_mass' and 'fuel_mass' are not consistent.";
         ok = false;
      }

      // Compute the time since launch for the events in the life of the stage.

      mInitialTime    = aInitialTime;
      mIgnitionTime   = mInitialTime + mPreIgnitionCoastTime;
      mBurnOutTime    = mIgnitionTime + mThrustDuration;
      mSeparationTime = mBurnOutTime + mPreSeparationCoastTime;
   }
   return ok;
}

// =================================================================================================
//! Perform second pass initialization of a stage.
//! @returns 'true' if successful, 'false' if unsuccessful.
bool WsfTBM_Mover::Stage::Initialize2()
{
   // Surface area times the TBM drag coefficient (m^2)
   mS_Cd0             = 0.0;
   double burnOutMass = RemainingBurnoutMass();
   if (mBallisticCoeff > 0.0)
   {
      mS_Cd0 = burnOutMass * UtEarth::cACCEL_OF_GRAVITY / mBallisticCoeff;
   }
   else if ((mCd0 > 0.0) && (mReferenceArea > 0.0))
   {
      mBallisticCoeff = (burnOutMass * UtEarth::cACCEL_OF_GRAVITY) / (mCd0 * mReferenceArea);
      mS_Cd0          = burnOutMass * UtEarth::cACCEL_OF_GRAVITY / mBallisticCoeff;
   }
   return true;
}

// =================================================================================================
//! Process potential inputs for a stage.
//! @param aInput The input stream.
//! @returns 'true' if the command was a stage related command or 'false' if not.
bool WsfTBM_Mover::Stage::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if ((command == "total_mass") || (command == "launch_mass"))
   {
      aInput.ReadValueOfType(mTotalMass, UtInput::cMASS);
      aInput.ValueGreater(mTotalMass, 0.0);
   }
   else if (command == "empty_mass")
   {
      aInput.ReadValueOfType(mEmptyMass, UtInput::cMASS);
      aInput.ValueGreater(mEmptyMass, 0.0);
   }
   else if (command == "fuel_mass")
   {
      aInput.ReadValueOfType(mFuelMass, UtInput::cMASS);
      aInput.ValueGreater(mFuelMass, 0.0);
   }
   else if (command == "fuel_mass_fraction")
   {
      aInput.ReadValue(mFuelMassFraction);
      aInput.ValueGreater(mFuelMassFraction, 0.0);
      aInput.ValueLess(mFuelMassFraction, 1.0);
   }
   else if (command == "thrust")
   {
      aInput.ReadValueOfType(mThrust, UtInput::cFORCE);
      aInput.ValueGreater(mThrust, 0.0);
   }
   else if (command == "specific_impulse")
   {
      aInput.ReadValueOfType(mSpecificImpulse, UtInput::cTIME);
      aInput.ValueGreater(mSpecificImpulse, 0.0);
   }
   else if (command == "ballistic_coefficient")
   {
      std::string units;
      std::string numeratorStr;
      std::string denominatorStr;
      aInput.ReadValue(mBallisticCoeff);
      aInput.ReadValue(units);
      std::string::size_type slashPos = units.find('/');
      if ((slashPos == std::string::npos) || ((slashPos + 1) >= units.size()))
      {
         throw UtInput::BadValue(aInput);
      }
      numeratorStr             = units.substr(0, slashPos);
      denominatorStr           = units.substr(slashPos + 1, units.size() - slashPos - 1);
      double numeratorFactor   = aInput.ConvertValue(1.0, numeratorStr, UtInput::cFORCE);
      double denominatorFactor = aInput.ConvertValue(1.0, denominatorStr, UtInput::cAREA);
      mBallisticCoeff *= (numeratorFactor / denominatorFactor);
   }
   else if (command == "reference_area")
   {
      aInput.ReadValueOfType(mReferenceArea, UtInput::cAREA);
      aInput.ValueGreater(mReferenceArea, 0.0);
   }
   else if (command == "diameter")
   {
      double diameter;
      aInput.ReadValueOfType(diameter, UtInput::cLENGTH);
      aInput.ValueGreater(diameter, 0.0);
      double radius  = 0.5 * diameter;
      mReferenceArea = UtMath::cPI * radius * radius;
   }
   else if (command == "zero_lift_cd")
   {
      aInput.ReadValue(mCd0);
      aInput.ValueGreaterOrEqual(mCd0, 0.0);
   }
   else if ((command == "thrust_duration") || (command == "burn_time"))
   {
      aInput.ReadValueOfType(mThrustDuration, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mThrustDuration, 0.0);
   }
   else if ((command == "pre_ignition_coast_time") || (command == "pre_burn_coast_time"))
   {
      aInput.ReadValueOfType(mPreIgnitionCoastTime, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mPreIgnitionCoastTime, 0.0);
   }
   else if ((command == "pre_separation_coast_time") || (command == "post_burn_coast_time"))
   {
      aInput.ReadValueOfType(mPreSeparationCoastTime, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mPreSeparationCoastTime, 0.0);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}
