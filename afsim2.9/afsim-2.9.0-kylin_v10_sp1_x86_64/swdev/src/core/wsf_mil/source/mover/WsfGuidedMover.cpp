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

#include "WsfGuidedMover.hpp"

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <cmath>
#include <iomanip>
#include <iostream>

#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLLPos.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtRandom.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfAero.hpp"
#include "WsfAeroTypes.hpp"
#include "WsfAngleOfAttack.hpp"
#include "WsfDateTime.hpp"
#include "WsfGuidanceComputer.hpp"
#include "WsfLaunchComputer.hpp"
#include "WsfLaunchHandoffData.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackManager.hpp"
#include "WsfWeaponEngagement.hpp"

namespace
{
// Independent variable indices for table lookups.
enum
{
   cAI_TIME,
   cAI_ALTITUDE,
   cAI_MACH,
   cAI_SPEED,
   cAI_COUNT // Number of independent variables
};
UtTable::Parameters sGuidedMoverParameters;

// Commanded angles less that this value are just set to zero.
const double cZERO_ANGLE_TOLERANCE = 0.01 * UtMath::cRAD_PER_DEG;

// Predicate for std::remove_if
bool HasBeenEjected(const WsfGuidedMover::Ejectable& aEjectable)
{
   return aEjectable.mMass <= 0.0;
}

// Compute the inclination of an orbit from the current position and velocity (see UtOrbitalElements::Publish)
double ComputeInclination(const double aPosECI[3], const double aVelECI[3])
{
   // 3. Compute the angular momentum vector and its magnitude (same as old algorithm):
   double h[3];
   UtVec3d::CrossProduct(h, aPosECI, aVelECI); //  h = r cross v
   double hMag = UtVec3d::Magnitude(h);
   // 5. Compute the inclination (same as old algorithm):
   double cosi = h[2] / hMag; // cos(inc) = DotProduct(h, z-axis) / (Mag(h) * Mag(z-axis))
   return std::acos(UtMath::NormalizeSinCosMinusOne_One(cosi));
}

} // namespace

// =================================================================================================
WsfGuidedMover::WsfGuidedMover(WsfScenario& aScenario, bool aGuidanceOn /* = true */)
   : WsfGuidedMoverBase(aScenario)
   , mStageList()
   , mExplicitStageUsed(false)
   , mImplicitStageUsed(false)
   , mEjectables()
   , mDivertThrust(0.0)
   , mDivertFuelFlowRate(0.0)
   , mDivertMinAlt(0.0)
   , mDivertMaxAlt(DBL_MAX)
   , mTimeConstantPtr()
   , mDesiredCoordinateFrame(cCF_WCS)
   , mIntegrationMethod(cRK2)
   , mGuidanceOn(aGuidanceOn)
   , mVerifyValues(true)
   , mCommandedYaw(0.0)
   , mCommandedPitch(0.0)
   , mCommandedRoll(0.0)
   , mLastYaw(0.0)
   , mLastPitch(0.0)
   , mLastRoll(0.0)
   , mLastMach(0.0)
   , mLastQ(0.0)
   , mMaxQ(0.0)
   , mCurState()
   , mTempState1()
   , mTempState2()
   , mTempState3()
   , mTempEntity()
   , mTestSupportObjectPtr(new WsfGuidedMoverTestObject(*this))
{
   // Allow mover update even when 'broken' (needed so ignition and separation failure to work)
   SetUpdateWhenBroken();

   // If not yet done, initialize the static list of potential independent variables for table lookups.
   // This doesn't have to be protected by a mutex because this is called during the creation of the scenario type
   // lists, which is single-threaded...
   if (sGuidedMoverParameters.Empty())
   {
      sGuidedMoverParameters.AddRealParameter("time", UtInput::cTIME, UtTable::ValueGE(0.0), cAI_TIME);
      sGuidedMoverParameters.AddRealParameter("altitude", UtInput::cLENGTH, UtTable::NoCheck(), cAI_ALTITUDE);
      sGuidedMoverParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), cAI_MACH);
      sGuidedMoverParameters.AddRealParameter("speed", UtInput::cSPEED, UtTable::ValueGE(0.0), cAI_SPEED);
   }
   for (size_t i = 0; i < cAI_COUNT; ++i)
   {
      mIV_Values[i] = 0.0;
   }
   UtVec3d::Set(mLastAeroForcesECS, 0.0);

   // Provide a default (single) stage in which to place input values.
   mStageList.emplace_back();
}

// =================================================================================================
WsfGuidedMover::WsfGuidedMover(const WsfGuidedMover& aSrc)
   : WsfGuidedMoverBase(aSrc)
   , mStageList(aSrc.mStageList)
   , mExplicitStageUsed(aSrc.mExplicitStageUsed)
   , mImplicitStageUsed(aSrc.mImplicitStageUsed)
   , mEjectables(aSrc.mEjectables)
   , mDivertThrust(aSrc.mDivertThrust)
   , mDivertFuelFlowRate(aSrc.mDivertFuelFlowRate)
   , mDivertMinAlt(aSrc.mDivertMinAlt)
   , mDivertMaxAlt(aSrc.mDivertMaxAlt)
   , mTimeConstantPtr(aSrc.mTimeConstantPtr)
   , mDesiredCoordinateFrame(aSrc.mDesiredCoordinateFrame)
   , mIntegrationMethod(aSrc.mIntegrationMethod)
   , mGuidanceOn(aSrc.mGuidanceOn)
   , mVerifyValues(aSrc.mVerifyValues)
   , mCommandedYaw(aSrc.mCommandedYaw)
   , mCommandedPitch(aSrc.mCommandedPitch)
   , mCommandedRoll(aSrc.mCommandedRoll)
   , mLastYaw(aSrc.mLastYaw)
   , mLastPitch(aSrc.mLastPitch)
   , mLastRoll(aSrc.mLastRoll)
   , mLastMach(aSrc.mLastMach)
   , mLastQ(aSrc.mLastQ)
   , mMaxQ(aSrc.mMaxQ)
   , mCurState(aSrc.mCurState)
   , mTempState1(aSrc.mTempState1)
   , mTempState2(aSrc.mTempState2)
   , mTempState3(aSrc.mTempState3)
   , mTempEntity(aSrc.mTempEntity)
   , mTestSupportObjectPtr(new WsfGuidedMoverTestObject(*this))
   , mECI_ConversionPtr(aSrc.mECI_ConversionPtr)
   , mComputeForcesEachSubstep(aSrc.mComputeForcesEachSubstep)
{
   for (size_t i = 0; i < cAI_COUNT; ++i)
   {
      mIV_Values[i] = aSrc.mIV_Values[i];
   }
   UtVec3d::Set(mLastAeroForcesECS, aSrc.mLastAeroForcesECS);
}

// =================================================================================================
//! Get the ECI conversion object.  You MUST call this method, instead of directly accessing the object.
UtECI_Conversion& WsfGuidedMover::GetECI_Conversion()
{
   if (mECI_ConversionPtr == nullptr)
   {
      mECI_ConversionPtr = ut::make_unique<UtECI_Conversion>(GetSimulation()->GetDateTime().GetStartDateAndTime(),
                                                             GetSimulation()->GetEnvironment().GetCentralBody());
   }
   if (mECI_UpdateTime != mECI_LastUpdateTime)
   {
      mECI_ConversionPtr->SetTime(mECI_UpdateTime);
      mECI_UpdateTime = mECI_LastUpdateTime;
   }
   return *mECI_ConversionPtr;
}

// =================================================================================================
WsfMover* WsfGuidedMover::Clone() const // override
{
   return new WsfGuidedMover(*this);
}

// ============================================================================
const char* WsfGuidedMover::GetScriptClassName() const
{
   return "WsfGuidedMover";
}

// =================================================================================================
bool WsfGuidedMover::Initialize(double aSimTime) // override
{
   bool ok = WsfGuidedMoverBase::Initialize(aSimTime);

   // Make sure all required input values have been supplied.
   size_t stageIndex;
   for (stageIndex = 0; stageIndex < mStageList.size(); ++stageIndex)
   {
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "WsfGuidedMover::Initialize:";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Stage: " << stageIndex + 1;
      }

      Stage& stage = mStageList[stageIndex];
      // If 'integration_timestep' was not provided for the stage, use the common value.
      if (stage.mIntegrationTimestep == 0.0)
      {
         stage.mIntegrationTimestep = mIntegrationTimestep;
      }

      if (!stage.Initialize(aSimTime, this, mShowStatus))
      {
         auto out = ut::log::error() << "Platform failed to initialize stage.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Stage: " << stageIndex + 1;
         auto note = out.AddNote() << "Input or computed values are:";
         stage.ShowStatus(this, note);
         ok = false;
         break;
      }

      // Tell the base class the stage index in which boost is complete.
      if (stage.mThrust > 0.0)
      {
         mBoostCompleteStageIndex = stageIndex;
      }
   }

   if (ok)
   {
      // Initialize the aggregated UtEntity objects
      mTempEntity.SetECI_ReferenceEpoch(GetSimulation()->GetDateTime().GetStartDateAndTime());
      mExternalState.SetECI_ReferenceEpoch(GetSimulation()->GetDateTime().GetStartDateAndTime());

      // Compute the composite vehicle values.  We go through the list in reverse order
      // because the earlier stages need the weight of the later (uppermost) stages.

      for (stageIndex = mStageList.size(); stageIndex != 0; --stageIndex)
      {
         // If not the last (or only) stage then add in the total mass from later stages
         if (stageIndex != mStageList.size())
         {
            Stage& stage       = mStageList[stageIndex - 1];
            stage.mPayloadMass = mStageList[stageIndex].mEmptyMass + mStageList[stageIndex].mFuelMass +
                                 mStageList[stageIndex].mPayloadMass;
         }
      }

      // Check divert thrust data
      if (mDivertThrust > 0.0)
      {
         if (mCurState.mDivertFuelMass == 0.0)
         {
            ut::log::error() << "'divert_fuel_mass' must be specified with 'divert_thrust'.";
            ok = false;
         }
         if (mDivertFuelFlowRate <= 0.0)
         {
            ut::log::error() << "'divert_fuel_flow_rate' must be specified with 'divert_thrust'.";
            ok = false;
         }
      }

      // Get the total ejectable mass, and initialize entries.
      double ejectableMass = 0.0;
      for (auto& ejectable : mEjectables)
      {
         ejectableMass += ejectable.mMass;
         if (ejectable.mTime <= 0.0)
         {
            ejectable.mTime = 1.0E+30;
         }
         if (ejectable.mAltitude <= 0.0)
         {
            ejectable.mAltitude = 1.0E+30;
         }
      }

      if (mShowStatus)
      {
         auto out = ut::log::info() << "Platform's Guided Mover Input Values.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         for (stageIndex = 0; stageIndex < mStageList.size(); ++stageIndex)
         {
            Stage& stage = mStageList[stageIndex];
            auto   note  = out.AddNote() << "Stage: " << stageIndex + 1;
            { // RAII block
               auto statusNote = note.AddNote() << "Guided Mover Status: ";
               stage.ShowStatus(this, statusNote);
            }
            if (mStageList.size() != 1)
            {
               double final = stage.mPayloadMass + stage.mEmptyMass + ejectableMass;
               note.AddNote() << "Total Start Mass: " << std::fixed << std::setprecision(3) << final + stage.mFuelMass
                              << " kg (" << (final + stage.mFuelMass) * UtMath::cLB_PER_KG << " lbm)";
               note.AddNote() << "Init no drag G: " << std::dec << std::setprecision(6)
                              << stage.mThrust / (final + stage.mFuelMass) / UtEarth::cACCEL_OF_GRAVITY << " g";
               note.AddNote() << "Total End Mass: " << std::fixed << std::setprecision(3) << final << " kg ("
                              << (final * UtMath::cLB_PER_KG) << " lbm)";
               note.AddNote() << "Burnout no drag G: " << std::dec << std::setprecision(6)
                              << stage.mThrust / final / UtEarth::cACCEL_OF_GRAVITY << " g";
            }
         }
         if (ejectableMass != 0.0)
         {
            out.AddNote() << "Total Ejectable Mass: " << ejectableMass << " kg";
         }
      }

      if (!mComputeForcesEachSubstep && (mIntegrationMethod == cRK2 || mIntegrationMethod == cRK4))
      {
         auto warn = ut::log::warning() << "WSF_GUIDED_MOVER Runge-Kutta integration without selecting "
                                           "'compute_all_forces_each_substep' is less accurate.";
         warn.AddNote() << "Please see WSF_GUIDED_MOVER documentation.";
      }

      // Set the pre-separation coast time of the last stage to infinity, as there is no next stage
      mStageList.back().mPreSeparationCoastTime = 0.5 * DBL_MAX;
   }

   return ok;
}

// =================================================================================================
bool WsfGuidedMover::Initialize2(double aSimTime) // override
{
   bool ok = WsfGuidedMoverBase::Initialize2(aSimTime);
   return ok;
}

// =================================================================================================
bool WsfGuidedMover::ProcessInput(UtInput& aInput) // override
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (WsfGuidedMoverBase::ProcessInput(aInput))
   {
      // NOTE: Normally the base class is called last to check the current command, but in this case
      // we do it first because 'integration_timestep' is in the base class as well as in the "block".
      // The block inherits the common value at run time if a block value was not provided.
   }
   else if (command == "time_constant") // NO_DOC | IN_DEVELOPMENT
   {
      // Must be before 'stage' as this is also a global command that applies to all stages.
      mTimeConstantPtr = UtTable::LoadInstance(aInput, UtInput::cTIME, UtTable::ValueGT(0.0), sGuidedMoverParameters);
   }
   else if (command == "table_test") // NO_DOC | TESTING
   {
      UtTable::TestQuery(aInput, mTimeConstantPtr, UtInput::cTIME, UtTable::ValueGT(0.0), sGuidedMoverParameters);
   }
   else if ((!mExplicitStageUsed) && mStageList[0].ProcessInput(aInput))
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
   else if (command == "ejectable")
   {
      UtInputBlock inputBlock(aInput);
      std::string  name;
      aInput.ReadValue(name);
      // If an ejectable with the specified name already exists then the commands edit the existing entry.
      std::vector<Ejectable>::iterator eli;
      for (eli = mEjectables.begin(); eli != mEjectables.end(); ++eli)
      {
         if (name == (*eli).mName)
         {
            break;
         }
      }
      if (eli == mEjectables.end())
      {
         mEjectables.emplace_back();
         eli = mEjectables.end();
         --eli;
         (*eli).mName = name;
      }

      while (inputBlock.ReadCommand(command))
      {
         if (command == "mass")
         {
            aInput.ReadValueOfType((*eli).mMass, UtInput::cMASS);
            aInput.ValueGreaterOrEqual((*eli).mMass, 0.0);
         }
         else if (command == "time")
         {
            aInput.ReadValueOfType((*eli).mTime, UtInput::cTIME);
            aInput.ValueGreaterOrEqual((*eli).mTime, 0.0);
         }
         else if (command == "altitude")
         {
            aInput.ReadValueOfType((*eli).mAltitude, UtInput::cLENGTH);
            aInput.ValueGreaterOrEqual((*eli).mAltitude, 0.0);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      // A object with zero-mass is deleted.
      if ((*eli).mMass == 0.0)
      {
         mEjectables.erase(eli);
      }
      else if (((*eli).mTime == 0.0) && ((*eli).mAltitude == 0.0))
      {
         throw UtInput::BadValue(aInput, "An ejection criteria must be specified for an ejectable");
      }
   }
   else if (command == "divert_thrust")
   {
      aInput.ReadValueOfType(mDivertThrust, UtInput::cFORCE);
      aInput.ValueGreaterOrEqual(mDivertThrust, 0.0); // 0 cancels divert thrust
   }
   else if (command == "divert_fuel_mass")
   {
      aInput.ReadValueOfType(mCurState.mDivertFuelMass, UtInput::cMASS);
      aInput.ValueGreaterOrEqual(mCurState.mDivertFuelMass, 0.0); // zero makes it as though wasn't supplied
   }
   else if (command == "divert_fuel_flow_rate")
   {
      aInput.ReadValueOfType(mDivertFuelFlowRate, UtInput::cMASS_TRANSFER);
      aInput.ValueGreaterOrEqual(mDivertFuelFlowRate, 0.0); // zero makes it as though wasn't supplied
   }
   else if (command == "divert_altitude_limits")
   {
      aInput.ReadValueOfType(mDivertMinAlt, UtInput::cLENGTH);
      aInput.ReadValueOfType(mDivertMaxAlt, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mDivertMinAlt, 0.0);
      aInput.ValueGreater(mDivertMaxAlt, mDivertMinAlt);
   }
   else if (command == "coordinate_frame")
   {
      std::string coordinateFrame;
      aInput.ReadValue(coordinateFrame);
      if (coordinateFrame == "wcs")
      {
         mDesiredCoordinateFrame = cCF_WCS;
         // Use 'launch_to_orbit true' AFTER this command if it needed for testing.
         mCurState.mLaunchToOrbit = false;
      }
      else if (coordinateFrame == "eci")
      {
         mDesiredCoordinateFrame  = cCF_ECI;
         mCurState.mLaunchToOrbit = true;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "integration_method")
   {
      std::string method;
      aInput.ReadValue(method);
      if (method == "trapezoidal")
      {
         mIntegrationMethod = cTRAPEZOIDAL;
      }
      else if (method == "rk2")
      {
         mIntegrationMethod = cRK2;
      }
      else if (method == "rk4")
      {
         mIntegrationMethod = cRK4;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Invalid integration_method: " + method);
      }
   }
   else if (command == "allow_inconsistent_mass_values")
   {
      mVerifyValues = false;
   }
   else if (command == "launch_to_orbit") // NO_DOC | TESTING
   {
      // This allows WCS uses to also launch_to_orbit or ECI to be used without launch_to_orbit (testing)
      aInput.ReadValue(mCurState.mLaunchToOrbit);
   }
   else if (command == "maintain_inclination")
   {
      aInput.ReadValue(mCurState.mMaintainInclination);
   }
   else if (command == "compute_all_forces_each_substep")
   {
      aInput.ReadValue(mComputeForcesEachSubstep);
   }
   else
   {
      // See note at start of 'if' block.
      throw UtInput::UnknownCommand(aInput);
   }
   return myCommand;
}

// =================================================================================================
// See documentation in base class header.
bool WsfGuidedMover::GetPropertyDouble(const std::string& aPropertyName,
                                       double&            aProperty) // override
{
   bool propertyValid = false;
   if ((aPropertyName == "available_maneuvering_force") || (aPropertyName == "maximum_aero_force")) // Deprecated - old form
   {
      if ((mStageIndex < mStageList.size()) && (mStageList[mStageIndex].mAeroPtr != nullptr))
      {
         // Get the maximum aero force that could be applied from the last update.
         // This is nominally CLmax * Q * S.
         aProperty     = mStageList[mStageIndex].mAeroPtr->GetMaximumForce();
         propertyValid = true;
      }
   }
   else if (aPropertyName == "delta_v_remaining")
   {
      // This uses the Tsiolkovsy rocket equation (see Wikipedia) to solve for delta_v. It only considers the delta_v
      // for the current stage and assumes the current rate of burn.

      double deltaV_Remaining = 0.0;
      if ((mStageIndex < mStageList.size()) &&
          (mStageIndex == mBoostCompleteStageIndex) && // For now it's useful only on the final boost stage...
          (mCurState.mCurrentFuelFlowRate > 0.0))
      {
         double burnTimeRemaining =
            mCurState.mCurrentFuelMass / mCurState.mCurrentFuelFlowRate; // Assume using all the fuel...
         double thrustDuration = mStageList[mStageIndex].mThrustDuration;
         if (thrustDuration > 0.0) // fixed thrust duration specified - may not use all fuel...
         {
            double burnTimeElapsed = mCurState.mTime - mPhaseStartTime;
            burnTimeRemaining =
               std::min(burnTimeRemaining, thrustDuration + mCurState.mThrustDurationExtension - burnTimeElapsed);
            burnTimeRemaining = std::max(burnTimeRemaining, 0.0);
         }

         // The initial total mass, m_0, is the total mass at this point in time...
         double initialTotalMass =
            GetPlatform()->GetEmptyMass() + GetPlatform()->GetPayloadMass() + mCurState.mCurrentFuelMass;

         // The final total mass, m_f is the initial total mass minus the fuel that would be burned if the engines kept burning.
         double finalTotalMass = initialTotalMass - (burnTimeRemaining * mCurState.mCurrentFuelFlowRate);

         // Thrust and I_sp are related through the definition: Thrust = Isp * g_0 * m_dot
         //                                      alternatively: I_sp = Thrust / (g_0 * m_dot)
         //
         // The exhaust gas velocity is related to I_sp as: v_e = I_sp * g_0
         //
         // Substituting for I_sp, we can get v_e as a function of thrust and m_dot
         //
         // v_e  = I_sp * g_0     = Thrust / (g_0 * m_dot) * g_0    =   Thrust / m_dot

         double exhaustVelocity = mCurState.mCurrentThrust / mCurState.mCurrentFuelFlowRate;
         deltaV_Remaining = exhaustVelocity * log(initialTotalMass / finalTotalMass); // deltaV = v_e * ln(m_0 / m_f);
         // TODO_UNUSED double ve = mStageList[mStageIndex].mSpecificImpulse * UtEarth::cACCEL_OF_GRAVITY;
      }
      aProperty     = deltaV_Remaining;
      propertyValid = true;
   }
   else if (aPropertyName == "burn_time_remaining")
   {
      // Assumes constant rate of burn and that it is currently burning.
      double burnTimeRemaining = 0.0;
      if ((mStageIndex < mStageList.size()) &&
          (mStageIndex == mBoostCompleteStageIndex) && // For now it's useful only on the final boost stage...
          (mCurState.mCurrentFuelFlowRate > 0.0))
      {
         burnTimeRemaining = mCurState.mCurrentFuelMass / mCurState.mCurrentFuelFlowRate; // Assume using all the fuel...
         double thrustDuration = mStageList[mStageIndex].mThrustDuration;
         if (thrustDuration > 0.0) // fixed thrust duration specified - may not use all fuel...
         {
            double burnTimeElapsed = mCurState.mTime - mPhaseStartTime;
            burnTimeRemaining =
               std::min(burnTimeRemaining, thrustDuration + mCurState.mThrustDurationExtension - burnTimeElapsed);
            burnTimeRemaining = std::max(burnTimeRemaining, 0.0);
         }
      }
      aProperty     = burnTimeRemaining;
      propertyValid = true;
   }
   else if (aPropertyName == "fuel_remaining")
   {
      double fuelRemaining = 0.0;
      if ((mStageIndex < mStageList.size()) &&
          (mStageIndex == mBoostCompleteStageIndex) && // For now it's useful only on the final boost stage...
          (mCurState.mCurrentFuelFlowRate > 0.0))
      {
         // Assumes constant rate of burn and that it is currently burning.
         double burnTimeRemaining =
            mCurState.mCurrentFuelMass / mCurState.mCurrentFuelFlowRate; // Assume using all the fuel...
         double thrustDuration = mStageList[mStageIndex].mThrustDuration;
         if (thrustDuration > 0.0) // fixed thrust duration specified - may not use all fuel...
         {
            double burnTimeElapsed = mCurState.mTime - mPhaseStartTime;
            burnTimeRemaining =
               std::min(burnTimeRemaining, thrustDuration + mCurState.mThrustDurationExtension - burnTimeElapsed);
            burnTimeRemaining = std::max(burnTimeRemaining, 0.0);
         }
         // mCurrentFuel mass has all fuel left. We only report what we will use.
         fuelRemaining = burnTimeRemaining * mCurState.mCurrentFuelFlowRate;
      }
      aProperty     = fuelRemaining;
      propertyValid = true;
   }
   else
   {
      propertyValid = WsfGuidedMoverBase::GetPropertyDouble(aPropertyName, aProperty);
   }
   return propertyValid;
}

// =================================================================================================
//! Return the commanded attitude angles.
//! This is called by WsfGuidanceProgram::AttitudeProgram to retrieve the current commanded attitude angles.
//! These are the angles that have been applied by the user command in addition to the standard
//! velocity-vector aligned coordinate system.
//! @param aYaw   The current yaw angle.
//! @param aPitch The current pitch angle.
//! @param aRoll  The current roll angle.
void WsfGuidedMover::GetCommandedAttitude(double& aYaw, double& aPitch, double& aRoll) const
{
   aYaw   = mCommandedYaw;
   aPitch = mCommandedPitch;
   aRoll  = mCommandedRoll;
}

// =================================================================================================
double WsfGuidedMover::GetMaximumLateralAerodynamicForce() const
{
   double force = 0.0;
   if ((mStageIndex < mStageList.size()) && (mStageList[mStageIndex].mAeroPtr != nullptr))
   {
      // Get the maximum aero force that could be applied from the last update.
      // This is nominally CLmax * Q * S.
      force = mStageList[mStageIndex].mAeroPtr->GetMaximumForce();
   }
   return force;
}

// =================================================================================================
double WsfGuidedMover::GetDynamicPressure() const
{
   double rho   = mAtmosphere.Density(GetPlatform()->GetAltitude());
   double speed = GetPlatform()->GetSpeed();
   return 0.5 * rho * speed * speed;
}

// =================================================================================================
double WsfGuidedMover::GetGravitationalAcceleration() const
{
   double locWCS[3];
   GetPlatform()->GetLocationWCS(locWCS);
   double rSquared = UtVec3d::MagnitudeSquared(locWCS);
   double g        = UtEarth::cGRAVITATIONAL_PARAMETER / rSquared;
   return g;
}

double WsfGuidedMover::GetEmptyMass() const
{
   return mStageList.at(mStageIndex).mEmptyMass;
}

double WsfGuidedMover::GetFuelMass() const
{
   return mStageList.at(mStageIndex).mFuelMass;
}

double WsfGuidedMover::GetPayloadMass() const
{
   return mStageList.at(mStageIndex).mPayloadMass;
}

double WsfGuidedMover::GetTotalMass() const
{
   return GetEmptyMass() + GetFuelMass() + GetPayloadMass();
}

double WsfGuidedMover::GetEmptyMass(size_t aStageIndex) const
{
   return mStageList.at(aStageIndex).mEmptyMass;
}

double WsfGuidedMover::GetFuelMass(size_t aStageIndex) const
{
   return mStageList.at(aStageIndex).mFuelMass;
}

double WsfGuidedMover::GetPayloadMass(size_t aStageIndex) const
{
   return mStageList.at(aStageIndex).mPayloadMass;
}

double WsfGuidedMover::GetTotalMass(size_t aStageIndex) const
{
   return GetEmptyMass(aStageIndex) + GetFuelMass(aStageIndex) + GetPayloadMass(aStageIndex);
}

// =================================================================================================
double WsfGuidedMover::GetLevelFlightThrust(double aSimTime, double aAltitude_m, double aMach) const
{
   WsfGuidedMover::State state;
   state.mAltitude = aAltitude_m;
   state.mMach     = aMach;

   double ivValues[6];
   ivValues[cAI_MACH] = aMach;

   // Copy, because we do not want to mutate mStageList
   Stage stage = mStageList.at(mStageIndex);

   // Note: GetHorizontalThrust takes several parameters by reference and mutates them.
   // We don't need the result of the mutated parameters. We only need the returned value (thrust)
   // In other words, we DO NOT want to mutate mStageList
   return GetHorizontalThrust(aSimTime, GetUpdateInterval(), state, -1.0, ivValues, stage);
}

double WsfGuidedMover::GetLevelFlightDrag(double aAltitude_m, double aMach, double aWeight_N) const
{
   return mStageList[mStageIndex].mAeroPtr->CalculateDrag(aAltitude_m, aMach, aWeight_N);
}

double WsfGuidedMover::GetLevelFlightFuelBurnRate(double aSimTime, double aAltitude_m, double aSpeed_mps, double aWeight_N) const
{
   double mach             = aSpeed_mps / mAtmosphere.SonicVelocity(aAltitude_m);
   double requiredThrust_N = GetLevelFlightDrag(aAltitude_m, mach, aWeight_N);

   if (requiredThrust_N > GetLevelFlightThrust(aSimTime, aAltitude_m, mach))
   {
      return 0.0;
   }

   // Determine the ambient pressure
   double isp_s = mStageList[mStageIndex].ComputeSpecificImpulse(mAtmosphere.Pressure(aAltitude_m), mSeaLevelPressure);

   return requiredThrust_N / (GetGravitationalAcceleration() * isp_s);
}

// =================================================================================================
// See documentation in base class header.
// protected
void WsfGuidedMover::AdvanceTime(double aSimTime, double aBegTime, double& aEndTime,
                                 Phase& aNextPhase) // override
{
   if (mIntegrationMethod != cTRAPEZOIDAL && mComputeForcesEachSubstep)
   {
      AdvanceTimeMultistep(aSimTime, aBegTime, aEndTime, aNextPhase);
   }
   else
   {
      AdvanceTimeSinglestep(aSimTime, aBegTime, aEndTime, aNextPhase);
   }

   double deltaT = aEndTime - aBegTime;

   // Accumulate any commanded attitude rotations
   if ((mCommandedYawRate != 0.0) || (mCommandedPitchRate != 0.0) || (mCommandedRollRate != 0.0))
   {
      // Update the commanded angles and keep them normalized.
      mCommandedYaw   = UtMath::NormalizeAngleMinusPi_Pi(mCommandedYaw + mCommandedYawRate * deltaT);
      mCommandedPitch = UtMath::Limit(mCommandedPitch + mCommandedPitchRate * deltaT, UtMath::cPI_OVER_2);
      mCommandedRoll  = UtMath::NormalizeAngleMinusPi_Pi(mCommandedRoll + mCommandedRollRate * deltaT);
      // If the commanded angles are close to zero, set them to zero. If all are zero this will avoid unnecessary work in PostUpdate
      if (fabs(mCommandedYaw) <= cZERO_ANGLE_TOLERANCE)
      {
         mCommandedYaw = 0.0;
      }
      if (fabs(mCommandedPitch) < cZERO_ANGLE_TOLERANCE)
      {
         mCommandedPitch = 0.0;
      }
      if (fabs(mCommandedRoll) < cZERO_ANGLE_TOLERANCE)
      {
         mCommandedRoll = 0.0;
      }
   }

   // Push results into the external state.
   UpdateExternalState(mCurState);

   // Update the remaining mass at the END of the timestep.
   GetPlatform()->SetFuelMass(mCurState.mCurrentFuelMass);

   // Determine if there is are any ejectable objects to be jettisoned.
   if (!mEjectables.empty()) // Masses exist that could be ejected
   {
      JettisonEjectables(aEndTime);
   }

   WriteTrajectoryOutput(aSimTime, aEndTime, mCurState.mAppliedForcesECS);

   // If 'show_status', check for Mach=1 crossing and MaxQ.
   // (mostly used for comparing orbital launch vehicles against published data)
   if (mShowStatus && (mDesiredCoordinateFrame == cCF_ECI))
   {
      // Check for Mach=1 transition.
      if ((mLastMach < 1.0) && (mCurState.mMach >= 1.0))
      {
         auto out = ut::log::info() << "Comparing orbital launch vehicles against published data." << std::fixed;
         out.AddNote() << "T = " << std::setprecision(5) << aSimTime;
         out.AddNote() << "Since Launch: " << std::setprecision(5) << aSimTime - mLaunchTime << " sec";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Mach: " << mCurState.mMach;
         out.AddNote() << "Altitude: " << mCurState.mAltitude << " m";
      }
      mLastMach = mCurState.mMach;

      // Check for maximum dynamic pressure.
      // Note that this method may find more than one maximum as it looks for a decreasing change.
      // We didn't want to wait until the end to report it...

      double rho = mAtmosphere.Density(mCurState.mAltitude);
      double q   = 0.5 * rho * mCurState.mSpeed * mCurState.mSpeed;
      if (q < mLastQ) // Q is decreasing
      {
         if (q > mMaxQ) // Found a new maximum.
         {
            mMaxQ    = q;
            auto out = ut::log::info() << "Check for maximum dynamic pressure report: " << std::fixed;
            out.AddNote() << "T = " << std::setprecision(5) << aSimTime;
            out.AddNote() << "Since Launch: " << std::setprecision(5) << aSimTime - mLaunchTime << " sec";
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << std::setprecision(1) << "MaxQ: " << mMaxQ << " Pa";
            out.AddNote() << std::setprecision(1) << "Altitude: " << mCurState.mAltitude << " m";
         }
      }
      mLastQ = q;
   }
}

void WsfGuidedMover::AdvanceTimeSinglestep(double aSimTime, double aBegTime, double& aEndTime, Phase& aNextPhase)
{
   double deltaT = aEndTime - aBegTime;

   // Update the guidance. mYForce and mZForce will be updated with the commanded forces.
   if (mGuidancePtr != nullptr)
   {
      // The second argument should be the time for by the current state of the weapon (aBegTime),
      // not the time to which we are trying to advance (aEndTime).
      mGuidancePtr->UpdateGuidance(aSimTime, aBegTime, aEndTime);
   }

   // Set the values of independent variables that may potential involved in table lookups.
   mIV_Values[cAI_TIME]     = aEndTime - mPhaseStartTime;
   mIV_Values[cAI_ALTITUDE] = mCurState.mAltitude;
   mIV_Values[cAI_MACH]     = mCurState.mMach;
   mIV_Values[cAI_SPEED]    = mCurState.mSpeed;

   const Stage& stage = mStageList[mStageIndex];

   // Determine the thrust forces without any vectoring applied.
   double thrustForcesECS[3];
   ComputeThrustForces(aBegTime, deltaT, mCurState, mStageList[mStageIndex], thrustForcesECS);

   // Determine if thrust vectoring can be employed to satisfy some (or all) of the commanded forces.
   ComputeVectoredThrustForces(aEndTime, mYForce, mZForce, mStageList[mStageIndex], thrustForcesECS);

   // Determine the aerodynamics forces (drag and lateral forces to satisfy commanded forces.)
   double aeroForcesECS[3] = {0.0, 0.0, 0.0};
   ComputeAeroForces(aSimTime, deltaT, mStageList[mStageIndex], thrustForcesECS, aeroForcesECS);
   UtVec3d::Set(mLastAeroForcesECS, aeroForcesECS);

   // If a speed was commanded, compute the required thrust. This must be done after computing the
   // aerodynamic forces because that is where the drag is computed.
   ComputeThrustForCommandedSpeed(deltaT, mCurState, mStageList[mStageIndex], aeroForcesECS, thrustForcesECS);

   // Applied forces do NOT include gravity...
   UtVec3d::Add(mCurState.mAppliedForcesECS, thrustForcesECS, aeroForcesECS);

   // If the commanded forces have not been achieved through aerodynamic and thrust vectoring forces,
   // determine if divert thrusters can achieve the residual...
   double divertThrustForcesECS[3];
   ComputeDivertForces(deltaT, mCurState, mYForce, mZForce, mCurState.mAppliedForcesECS, divertThrustForcesECS);

   // Total thrust forces (for scripting)
   UtVec3d::Add(mCurState.mLastThrustForcesECS, thrustForcesECS, divertThrustForcesECS);

   // Update the fuel consumed.
   if (mPhase == cPHASE_BURN)
   {
      double fuelUsed = mCurState.mCurrentFuelFlowRate * deltaT;
      if (fuelUsed <= mCurState.mCurrentFuelMass)
      {
         mCurState.mCurrentFuelMass -= fuelUsed;
      }
      else if (mCurState.mCurrentFuelFlowRate > 0.0)
      {
         // The fuel will be expended during this timestep, so limit the timestep to only expend what is left.
         deltaT                     = mCurState.mCurrentFuelMass / mCurState.mCurrentFuelFlowRate;
         mCurState.mCurrentFuelMass = 0.0;
         aEndTime                   = aBegTime + deltaT;
      }

      // Determine if it is time to advance to the post-burn phase. This occurs either when all fuel is
      // consumed or the declared burn time has elapsed.
      if ((mCurState.mCurrentFuelMass <= 0.0) ||
          ((stage.mThrustDuration > 0.0) &&
           ((aEndTime - mPhaseStartTime) > (stage.mThrustDuration + mCurState.mThrustDurationExtension))))
      {
         aNextPhase = cPHASE_POST_BURN;
      }
   }
   mCurState.mMass = GetPlatform()->GetMass();

   switch (mIntegrationMethod)
   {
   case cTRAPEZOIDAL:
      AdvanceTimeTrapezoidal(aEndTime, mCurState, mCurState.mAppliedForcesECS);
      break;
   case cRK2:
      AdvanceTimeLegacyRK2(aEndTime, mCurState, mCurState.mAppliedForcesECS);
      break;
   case cRK4:
      AdvanceTimeLegacyRK4(aEndTime, mCurState, mCurState.mAppliedForcesECS);
      break;
   default:
      throw std::runtime_error{"Incompatible integration method."};
      break;
   }
}

void WsfGuidedMover::AdvanceTimeMultistep(double aSimTime, double aBegTime, double aEndTime, Phase& aNextPhase)
{
   // Update the guidance. mYForce and mZForce will be updated with the commanded forces.
   if (mGuidancePtr != nullptr)
   {
      // The second argument should be the time for by the current state of the weapon (aBegTime),
      // not the time to which we are trying to advance (aEndTime).
      mGuidancePtr->UpdateGuidance(aSimTime, aBegTime, aEndTime);
   }

   // Set the values of independent variables that may potential involved in table lookups.
   mIV_Values[cAI_TIME]     = aEndTime - mPhaseStartTime;
   mIV_Values[cAI_ALTITUDE] = mCurState.mAltitude;
   mIV_Values[cAI_MACH]     = mCurState.mMach;
   mIV_Values[cAI_SPEED]    = mCurState.mSpeed;

   switch (mIntegrationMethod)
   {
   case cRK2:
      AdvanceTimeRK<RK2_StepFunction>(aSimTime, aBegTime, aEndTime);
      break;
   case cRK4:
      AdvanceTimeRK<RK4_StepFunction>(aSimTime, aBegTime, aEndTime);
      break;
   case cTRAPEZOIDAL: // Intentional fallthrough
   default:           //
      throw std::runtime_error{"Incompatible integration method."};
      break;
   }

   // Determine if it is time to advance to the post-burn phase. This occurs either when all fuel is
   // consumed or the declared burn time has elapsed.
   if ((mCurState.mCurrentFuelMass <= 0.0) ||
       ((mStageList[mStageIndex].mThrustDuration > 0.0) &&
        ((aEndTime - mPhaseStartTime) > (mStageList[mStageIndex].mThrustDuration + mCurState.mThrustDurationExtension))))
   {
      aNextPhase = cPHASE_POST_BURN;
   }
}

// ================================================================================================
// See WsfGuidedMoverBase for documentation
// protected
void WsfGuidedMover::InitiateMotion(double aSimTime,
                                    double aCurTime) // override
{
   WsfLaunchHandoffData* handoffPtr = WsfLaunchHandoffData::Find(*GetPlatform());

   mCurState.mTime     = aCurTime;
   mCurState.mAltitude = GetPlatform()->GetAltitude();
   mCurState.mSpeed    = GetPlatform()->GetSpeed();

   // Get any launch computer data that is provided in the track.
   bool            interceptPointValid = false;
   WsfGeoPoint     interceptPoint;
   const WsfTrack* trackPtr = GetPlatform()->GetTrackManager().GetCurrentTarget();
   if (trackPtr != nullptr)
   {
      interceptPointValid = WsfLaunchComputer::GetInterceptPoint(*trackPtr, interceptPoint);
   }

   double lat;
   double lon;
   double alt;
   double wpnLocWCS[3];
   GetPlatform()->GetLocationLLA(lat, lon, alt);
   UtSphericalEarth::ConvertLLAToECEF(lat, lon, alt, wpnLocWCS);
   UtVec3d::Set(mCurState.mLocation, wpnLocWCS);

   // Recover the original tilt angle of the launcher.
   double tiltAngle = 0.0;
   if (mCurState.mSpeed != 0.0)
   {
      double wpnVelNED[3];
      GetPlatform()->GetVelocityNED(wpnVelNED);
      double wpnVelNE = sqrt(wpnVelNED[0] * wpnVelNED[0] + wpnVelNED[1] * wpnVelNED[1]);
      tiltAngle       = atan2(-wpnVelNED[2], wpnVelNE);
   }
   else
   {
      // Align along the ECS X body vector
      double ecsVec[3] = {1.0, 0.0, 0.0};
      double NEDToECS_Transform[3][3];
      GetPlatform()->GetNEDToECSTransform(NEDToECS_Transform);
      double nedVec[3];
      UtMat3d::InverseTransform(nedVec, NEDToECS_Transform, ecsVec);
      tiltAngle = asin(-nedVec[2] / UtVec3d::Magnitude(nedVec));
   }

   // Compute the WCS->ECS coordinate transform matrix.
   // Remember that the rows are simply the basis vector of the local frame in the WCS frame.

   // The ECS Z axis points to the center of the earth.
   UtVec3d::Normalize(mCurState.mWorld_ToECS_Transform[2], wpnLocWCS);
   UtVec3d::Multiply(mCurState.mWorld_ToECS_Transform[2], -1.0);

   // The temporary ECS X axis points at the target. This will be reformed later.
   double tgtLocWCS[3];
   if (interceptPointValid)
   {
      interceptPoint.GetLocationLLA(lat, lon, alt);
      UtSphericalEarth::ConvertLLAToECEF(lat, lon, alt, tgtLocWCS);
   }
   else
   {
      double heading;
      // This mover lacks an intercept point, so determine whether velocity or
      // orientation should be used during separation
      if (mMatchOwningPlatHeading && !UtMath::NearlyZero(GetPlatform()->GetSpeed()))
      {
         // Use velocity to determine the projected heading
         auto velocity = GetPlatform()->GetVelocityNED();
         heading       = std::atan2(velocity[1], velocity[0]);
      }
      else
      {
         // Use orientation to determine the projected heading
         double pitch;
         double roll;
         GetPlatform()->GetOrientationNED(heading, pitch, roll);
      }

      // This is a 3dof mover, so we need to do a great circle extrapolation.
      UtSphericalEarth::ExtrapolateGreatCirclePosition(lat, lon, heading * UtMath::cDEG_PER_RAD, 100000.0, lat, lon);
      UtSphericalEarth::ConvertLLAToECEF(lat, lon, 0.0, tgtLocWCS);
   }

   UtVec3d::Subtract(mCurState.mWorld_ToECS_Transform[0], tgtLocWCS, wpnLocWCS);
   UtVec3d::Normalize(mCurState.mWorld_ToECS_Transform[0]);

   // The ECS Y axis is just Z cross X
   UtVec3d::CrossProduct(mCurState.mWorld_ToECS_Transform[1],
                         mCurState.mWorld_ToECS_Transform[2],
                         mCurState.mWorld_ToECS_Transform[0]);
   UtVec3d::Normalize(mCurState.mWorld_ToECS_Transform[1]);

   // The ECS X axis is just Y cross Z. This puts the X axis into the plane formed by the
   // the points at the center of the earth, the launch location and the target location.
   UtVec3d::CrossProduct(mCurState.mWorld_ToECS_Transform[0],
                         mCurState.mWorld_ToECS_Transform[1],
                         mCurState.mWorld_ToECS_Transform[2]);
   UtVec3d::Normalize(mCurState.mWorld_ToECS_Transform[0]);

   // Rotate the coordinate frame about the Y axis by the launcher tilt to get the direction
   // of the initial velocity vector.
   double rotateY[3][3];
   UtEntity::ComputeRotationalTransform(0.0, tiltAngle, 0.0, rotateY);
   double tempTransform[3][3];
   UtMat3d::Multiply(tempTransform, rotateY, mCurState.mWorld_ToECS_Transform);
   UtMat3d::Set(mCurState.mWorld_ToECS_Transform, tempTransform);

   // The velocity vector is just the ECS X axis times the speed
   UtVec3d::Multiply(mCurState.mVelocity, mCurState.mWorld_ToECS_Transform[0], GetPlatform()->GetSpeed());

   // Get the launch hand-off data and use it to initialize.
   //
   // NOTE: This isn't valid when the pre_ignition_coast_time is non-zero and the launch is from a
   // moving platform, which simulates the situation where an air-launched missile is still captive
   // (attached to the firing platform) for a short period after the 'Fire' command is issued.
   // In such as case the GetLaunchPlatformXXX methods don't reflect the current data.
   //
   // TODO(akrisby): _JAJ This is currently only used for 'launch-to-orbit'. Since this is a new capability
   // and it may affect existing results, I want to limit the scope to where it is actually needed.
   // When I am less time-constrained I will go back and look at how this can be merged, as this
   // has some useful benefit for ALL launches.

   if ((handoffPtr != nullptr) && mCurState.mLaunchToOrbit)
   {
      // If we are performing a true launch-to-orbit (initial altitude close to the ground and velocity is
      // small) then reset the velocity vector using just the delta V specified. WsfExplicitWeapon messes
      // with it if it too small.
      if (mCurState.mLaunchToOrbit && (mCurState.mAltitude < 10000.0) && (mCurState.mSpeed < 100.0))
      {
         // Use the delta V specified. WsfExplictWeapon does some messing with it if it is too small
         // and we really don't want that...
         double launchDeltaV[3];
         handoffPtr->GetLaunchDeltaV(launchDeltaV);
         double launchSpeed = UtVec3d::Magnitude(launchDeltaV);
         UtVec3d::Multiply(mCurState.mVelocity, mCurState.mWorld_ToECS_Transform[0], launchSpeed);
      }
   }

   // Extract the Euler angles from the WCS-to-ECS transformation matrix and set the WCS orientation
   UtEntity::ExtractEulerAngles(mCurState.mWorld_ToECS_Transform, mCurState.mPsi, mCurState.mTheta, mCurState.mPhi);
   GetPlatform()->SetOrientationWCS(mCurState.mPsi, mCurState.mTheta, mCurState.mPhi);

   // Update other state data a necessary.
   mCurState.mAltitude = UtVec3d::Magnitude(mCurState.mLocation) - UtSphericalEarth::cEARTH_RADIUS;
   mCurState.mSpeed    = UtVec3d::Magnitude(mCurState.mVelocity);
   mCurState.mMach     = mCurState.mSpeed / mAtmosphere.SonicVelocity(mCurState.mAltitude);

   // 'mCurState' has been built assuming WCS/ECEF coordinate system.
   // If ECI coordinates have been selected, and it is 'acceptable' then it must be converted to ECI.
   if (mDesiredCoordinateFrame == cCF_ECI)
   {
      CoordinateFrame targetFrame;
      if (SelectCoordinateFrame(mCurState, targetFrame))
      {
         SwitchCoordinateFrame(mCurState, targetFrame);
         mLastMach = 1.1;     // Disable Mach 1 crossing message
         mMaxQ     = 1.0E+30; // Disable maxQ status message
      }
   }
   UpdateExternalState(mCurState);

   // Capture reference angle for inertial frame/fixed frame velocity conversions.
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Motion Starting";
      mCurState.Print(out, mFMM_Time);
   }

   // If 'maintain_inclination true' is applicable, write out the inclination based on initial conditions.
   if (mShowStatus && mCurState.mLaunchToOrbit && mCurState.mMaintainInclination)
   {
      UtEntity::ConvertWCSToLLA(mFMM_LocWCS, lat, lon, alt);
      double cosLat = std::cos(lat * UtMath::cRAD_PER_DEG);
      double inclination =
         std::acos(cosLat * sin(mFMM_Heading)); // cos(inclination) = cos(launch_latitude) * sin(launch_heading)

      auto out = ut::log::info() << "Platform orbit inclination based on Launch Conditions." << std::fixed;
      out.AddNote() << "T = " << std::setprecision(5) << mCurState.mTime;
      out.AddNote() << "Since Launch: " << mCurState.mTime - mLaunchTime << " sec";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Launch Conditions: " << inclination * UtMath::cDEG_PER_RAD << " deg";
   }
}

// =================================================================================================
bool WsfGuidedMover::SeparateStage(double aSimTime,
                                   size_t aStageIndex) // override
{
   bool separated = ((mStageList[aStageIndex].mProbabilityOfSeparation == 1.0) ||
                     (GetSimulation()->GetRandom().Bernoulli(mStageList[aStageIndex].mProbabilityOfSeparation)));
   if (!separated)
   {
      SetDamageFactor(aSimTime, 1.0); // Initiates a call to PartBroken.
   }
   return separated;
}

// =================================================================================================
bool WsfGuidedMover::IgniteStage(double aSimTime,
                                 size_t aStageIndex) // override
{
   bool ignited = ((mStageList[aStageIndex].mProbabilityOfIgnition == 1.0) ||
                   (GetSimulation()->GetRandom().Bernoulli(mStageList[aStageIndex].mProbabilityOfIgnition)));
   if (!ignited)
   {
      SetDamageFactor(aSimTime, 1.0); // Initiates a call to PartBroken.
   }
   return ignited;
}

// =================================================================================================
void WsfGuidedMover::InitiateStage(double aSimTime, double aCurTime,
                                   size_t aStageIndex) // override
{
   if (aStageIndex >= mStageList.size())
   {
      { // RAII block
         auto out = ut::log::error() << "Platform attempting to use stage index that is too large.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Stage: " << aStageIndex + 1;
         out.AddNote() << "Max Stage: " << mStageList.size();
      }
      abort();
   }

   mStageIndex  = aStageIndex;
   Stage& stage = mStageList[mStageIndex];
   if (mShowStatus)
   {
      auto out = ut::log::info() << "Staging Report:";
      WriteStagingMessage(out, aSimTime, aCurTime, "initiation", aStageIndex);
   }

   mPhase                             = cPHASE_PRE_BURN;
   mPhaseStartTime                    = aCurTime;
   mPreIgnitionCoastTime              = stage.mPreIgnitionCoastTime;
   mPreSeparationCoastTime            = stage.mPreSeparationCoastTime;
   mIntegrationTimestep               = stage.mIntegrationTimestep;
   mCurState.mCurrentFuelMass         = stage.mFuelMass;
   mCurState.mThrustDurationExtension = 0.0;

   GetPlatform()->SetEmptyMass(stage.mEmptyMass);
   GetPlatform()->SetFuelMass(stage.mFuelMass);
   // Payload mass is the sum of the total mass of every stage above this one AND all the eject-able masses.
   double payloadMass = stage.mPayloadMass; // The sum of the 'total_mass' for stages above this one
   for (auto& ejectable : mEjectables)
   {
      payloadMass += ejectable.mMass;
   }
   GetPlatform()->SetPayloadMass(payloadMass);

   // This allows the user to declare that boost phase is complete even if subsequent stages have thrust.
   if (stage.mFinalBoostStage)
   {
      mBoostCompleteStageIndex = mStageIndex;
   }

   if (mShowStatus)
   {
      auto out = ut::log::info() << "Platform Data Report:";
      out.AddNote() << "T = " << std::fixed << std::setprecision(5) << aSimTime;
      out.AddNote() << "Since Launch: " << aCurTime - mLaunchTime << " sec";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();

      auto note = out.AddNote() << "Mass: " << std::setprecision(2) << GetPlatform()->GetMass() << " kg";
      note.AddNote() << "Payload: " << GetPlatform()->GetPayloadMass() << " kg";
      note.AddNote() << "Empty: " << GetPlatform()->GetEmptyMass() << " kg";
      note.AddNote() << "Fuel: " << GetPlatform()->GetFuelMass() << " kg";
   }
}

// ================================================================================================
//! Perform any post update actions to the platform.
//! This is called by the WsfGuidedMoverBase::Update just prior to exiting. The reason this exists is that
//! mExternalState (the state pushed to the platform prior to calling guidance and aero) must not have any
//! extra rotations other that provided by the standard orientation-to-velocity-vector (WsfGuidanceComputer
//! assumes this!.) Because we allow extra orientations through angle_of_attack table, bank_to_turn and
//! explicit attitude control programs, these must be applied just prior to exiting update.
//!
//! @param aSimTime The current simulation time.
void WsfGuidedMover::PostUpdate(double aSimTime) // override
{
   static const double cTIME_CONSTANT = 0.25; // time constant for aoa/bank angle changes

   Stage& stage = mStageList[mStageIndex];
   double dt    = aSimTime - mLastUpdateTime;

   double yaw   = mCommandedYaw;
   double pitch = mCommandedPitch;
   double roll  = mCommandedRoll;

   // If an angle-of-attack table was defined it overrides any command pitch
   if (stage.mAngleOfAttackPtr != nullptr)
   {
      pitch = stage.mAngleOfAttackPtr->Lookup(mIV_Values);
      // The new pitch angle is blended with the old to prevent jumps.
      double f = exp(-dt / cTIME_CONSTANT);
      pitch    = (mLastPitch * f) + ((1.0 - f) * pitch); // Xnew = (Xold * f) + ((1 - f) * Xnew)
   }

   // If bank_to_turn was specified it overrides any commanded roll.
   if (stage.mTurnMethod == Stage::cBANK_TO_TURN)
   {
      double yAclECS = mLastAeroForcesECS[1] / GetPlatform()->GetMass();
      roll           = yAclECS / UtEarth::cACCEL_OF_GRAVITY;
      // The accelerations can be large and can generate large roll angles. Since this is just basically eye-candy
      // to roll angle is limited to +/- 90 degrees.
      roll = UtMath::Limit(roll, UtMath::cPI_OVER_2);
      // The new roll angle is blended with the old to avoid jumps
      double f = exp(-dt / cTIME_CONSTANT);
      roll     = (mLastRoll * f) + ((1.0 - f) * roll); // Xnew = (Xold * f) + ((1 - f) * Xnew)
   }

   // Apply an attitude changes as required
   if ((yaw != 0.0) || (pitch != 0.0) || (roll != 0.0))
   {
      double attitudeTransform[3][3];
      double curWCS_ToECS_Transform[3][3];
      double newWCS_ToECS_Transform[3][3];
      double psi;
      double theta;
      double phi;

      UtEntity::ComputeRotationalTransform(yaw, pitch, roll, attitudeTransform);
      GetPlatform()->GetWCSToECSTransform(curWCS_ToECS_Transform);
      UtMat3d::Multiply(newWCS_ToECS_Transform, attitudeTransform, curWCS_ToECS_Transform);
      UtEntity::ExtractEulerAngles(newWCS_ToECS_Transform, psi, theta, phi);
      GetPlatform()->SetOrientationWCS(psi, theta, phi);
   }
   mLastYaw   = yaw;
   mLastPitch = pitch;
   mLastRoll  = roll;
}

// =================================================================================================
// protected
void WsfGuidedMover::AdvanceTimeLegacyRK2(double aSimTime, State& aState, const double aAppliedForcesECS[3])
{
   double dt  = aSimTime - aState.mTime;
   double hdt = 0.5 * dt;
   if (dt < 1.0E-10)
   {
      return;
   }

   // Using the velocity and acceleration from the current state, integrate forward
   // one half time step to get an intermediate state.
   mTempState1 = aState;
   IntegrateEOM(hdt, aAppliedForcesECS, mTempState1, aState, aState);

   // Using the velocity and acceleration from the intermediate state, integrate forward
   // the full time step to get the new state.
   IntegrateEOM(dt, aAppliedForcesECS, aState, aState, mTempState1);
   aState.mTime = aSimTime;
}

// =================================================================================================
// protected
void WsfGuidedMover::AdvanceTimeLegacyRK4(double aSimTime, State& aState, const double aAppliedForcesECS[3])
{
   double dt  = aSimTime - aState.mTime;
   double hdt = 0.5 * dt;
   if (dt < 1.0E-10)
   {
      return;
   }

   // Using the velocity and acceleration from the current state, start with the current state and
   // integrate forward one half time step to get 'state 1'
   mTempState1 = aState;
   IntegrateEOM(hdt, aAppliedForcesECS, mTempState1, aState, aState);

   // Using the velocity and acceleration from the 'state 1', start with the current state and
   // integrate forward one half time step to get 'state 2'.
   mTempState2 = aState;
   IntegrateEOM(hdt, aAppliedForcesECS, mTempState2, aState, mTempState1);

   // Using the velocity and acceleration from the 'state 2', start with the current state and
   // integrate forward a full time step to get 'state 3'.
   mTempState3 = aState;
   IntegrateEOM(dt, aAppliedForcesECS, mTempState3, aState, mTempState2);

   mTempState3.ComputeDerivatives(aAppliedForcesECS);

   // And now merge them all together to get the final integrated state.
   double dtOver6 = dt / 6.0;
   for (int i = 0; i < 3; ++i)
   {
      aState.mLocation[i] += dtOver6 * (aState.mVelocity[i] + mTempState1.mVelocity[i] * 2.0 +
                                        mTempState2.mVelocity[i] * 2.0 + mTempState3.mVelocity[i]);
      double deltaVelWCS = dtOver6 * (aState.mAcceleration[i] + mTempState1.mAcceleration[i] * 2.0 +
                                      mTempState2.mAcceleration[i] * 2.0 + mTempState3.mAcceleration[i]); // dv = a * dt
      aState.mVelocity[i] += deltaVelWCS; // v' = v + a * dt
   }

   aState.mMoverPtr = this;
   aState.UpdateOtherStateData(aSimTime);
}

// =================================================================================================
// protected
void WsfGuidedMover::AdvanceTimeTrapezoidal(double aSimTime, State& aState, const double aAppliedForcesECS[3])
{
   double dt  = aSimTime - aState.mTime;
   double hdt = 0.5 * dt;
   if (dt < 1.0E-10)
   {
      return;
   }
   // Save any state data prior to integration that is needed for orientation updates by rotation.

   // Compute the acceleration at the current location
   aState.ComputeDerivatives(aAppliedForcesECS);

   // Compute the new velocity.
   double lastVelWCS[3];
   UtVec3d::Set(lastVelWCS, aState.mVelocity);
   double deltaVelWCS[3];
   UtVec3d::Multiply(deltaVelWCS, aState.mAcceleration, dt);      // dv = a * dt;
   UtVec3d::Add(aState.mVelocity, aState.mVelocity, deltaVelWCS); // v' = v + a * dt

   // Compute the position increment and update the position.
   double deltaLocWCS[3];
   deltaLocWCS[0] = (aState.mVelocity[0] + lastVelWCS[0]) * hdt;
   deltaLocWCS[1] = (aState.mVelocity[1] + lastVelWCS[1]) * hdt;
   deltaLocWCS[2] = (aState.mVelocity[2] + lastVelWCS[2]) * hdt;
   UtVec3d::Add(aState.mLocation, aState.mLocation, deltaLocWCS);

   // Update altitude, speed and orientation data.
   aState.mMoverPtr = this;
   aState.UpdateOtherStateData(aSimTime);
}

// =================================================================================================
// protected
void WsfGuidedMover::State::ComputeDerivatives(const double aAppliedForcesECS[3])
{
   // Convert the ECS forces to WCS
   double forceVecWCS[3];
   UtMat3d::InverseTransform(forceVecWCS, mWorld_ToECS_Transform, aAppliedForcesECS);

   // Compute the acceleration due to all forces but gravity.
   double newAclWCS[3];
   UtVec3d::Multiply(newAclWCS, forceVecWCS, 1.0 / mMass);

   // Add in the acceleration due to gravity.
   double gravVecWCS[3];
   ComputeGravitationalForce(gravVecWCS);
   UtVec3d::Add(mAcceleration, newAclWCS, gravVecWCS);
}

// =================================================================================================
// protected
void WsfGuidedMover::ComputeDivertForces(double aDeltaTime,
                                         State& aState,
                                         double aCommandedForceY,
                                         double aCommandedForceZ,
                                         double aAppliedForcesECS[3],
                                         double aDivertThrustForcesECS[3]) const
{
   UtVec3d::Set(aDivertThrustForcesECS, 0.0);
   if ((mDivertThrust <= 0.0) || (aState.mDivertFuelMass <= 0.0))
   {
      return;
   }

   if ((aState.mAltitude < mDivertMinAlt) || (aState.mAltitude > mDivertMaxAlt))
   {
      return;
   }

   // Compute the additional divert force desired in each direction and the total divert force desired.
   double divertForceY = std::max(fabs(aCommandedForceY) - fabs(aAppliedForcesECS[1]), 0.0);
   if (aCommandedForceY < 0.0)
   {
      divertForceY = -divertForceY;
   }
   double divertForceZ = std::max(fabs(aCommandedForceZ) - fabs(aAppliedForcesECS[2]), 0.0);
   if (aCommandedForceZ < 0.0)
   {
      divertForceZ = -divertForceZ;
   }
   double divertForce = sqrt((divertForceY * divertForceY) + (divertForceZ * divertForceZ));

   // If divert force is needed, apply it up to the maximum divert force allowed.
   // If less than the maximum is allowed, assume the thruster is continuously variable and that
   // the fuel flow rate is proportional to the thrust.

   if (divertForce > 0.0)
   {
      double fuelFlowRate = mDivertFuelFlowRate;
      if (divertForce > mDivertThrust)
      {
         divertForceY = (divertForceY / divertForce) * mDivertThrust;
         divertForceZ = (divertForceZ / divertForce) * mDivertThrust;
      }
      else
      {
         fuelFlowRate *= (divertForce / mDivertThrust);
      }
      UtVec3d::Set(aDivertThrustForcesECS, 0.0, divertForceY, divertForceZ);
      aAppliedForcesECS[1] += divertForceY;
      aAppliedForcesECS[2] += divertForceZ;
      aState.mDivertFuelMass -= fuelFlowRate * aDeltaTime;
      aState.mDivertFuelMass = std::max(aState.mDivertFuelMass, 0.0);
   }
}

// =================================================================================================
// protected
void WsfGuidedMover::ComputeThrustForces(double aSimTime,
                                         double aDeltaTime,
                                         State& aState,
                                         Stage& aStage,
                                         double aThrustForceECS[3]) const
{
   aState.mCurrentThrust       = 0.0;
   aState.mCurrentFuelFlowRate = 0.0;

   if (mPhase == cPHASE_BURN)
   {
      aState.mCurrentThrust = GetHorizontalThrust(aSimTime, aDeltaTime, aState, mCommandedThrottle, mIV_Values, aStage);
   }

   UtVec3d::Set(aThrustForceECS, aState.mCurrentThrust, 0.0, 0.0);
}

double WsfGuidedMover::GetHorizontalThrust(double       aSimTime,
                                           double       aDeltaTime,
                                           State&       aState,
                                           double       aCommandedThrottle,
                                           const double aIV_Values[6],
                                           Stage&       aCurrentStage) const
{
   aState.mCurrentFuelFlowRate = 0.0;

   double phaseTime = aSimTime - mPhaseStartTime;

   // Determine the ambient pressure
   double localPressure = mAtmosphere.Pressure(aState.mAltitude);

   double thrust = aCurrentStage.DetermineCurrentThrust(localPressure, mSeaLevelPressure, phaseTime);

   // Determine the fuel flow rate
   if (aCurrentStage.mBurnRateTablePtr != nullptr)
   {
      aState.mCurrentFuelFlowRate = aCurrentStage.mBurnRateTablePtr->Lookup(phaseTime);
   }
   else
   {
      // Calculate the rate from the thrust and specific impulse.
      aCurrentStage.mSpecificImpulse = aCurrentStage.ComputeSpecificImpulse(localPressure, mSeaLevelPressure);
      if (aCurrentStage.mSpecificImpulse > 0.0)
      {
         aState.mCurrentFuelFlowRate = thrust / (aCurrentStage.mSpecificImpulse * UtEarth::cACCEL_OF_GRAVITY);
      }
   }

   // If either of the resulting thrust or fuel flow rate values is zero, set them both to zero.
   if ((thrust == 0.0) || (aState.mCurrentFuelFlowRate == 0.0))
   {
      thrust                      = 0.0;
      aState.mCurrentFuelFlowRate = 0.0;
   }
   else if (aCurrentStage.mReverseThrust)
   {
      thrust = -thrust;
   }

   // Apply the throttle factor if present. This assumes a linear relationship between thrust and fuel flow.
   double throttleFactor = 1.0;
   if (aCommandedThrottle >= 0.0)
   {
      throttleFactor = aCommandedThrottle;
      // When a commanded throttle is being employed, increase the thrust duration to account for reduced throttle.
      // Otherwise the user would have to modify their rocket definition artificially to use external throttle command...
      aState.mThrustDurationExtension += (1.0 - throttleFactor) * aDeltaTime;
   }
   else if (aCurrentStage.mThrottlePtr != nullptr)
   {
      throttleFactor = aCurrentStage.mThrottlePtr->Lookup(aIV_Values);
   }
   thrust *= throttleFactor;
   aState.mCurrentFuelFlowRate *= throttleFactor;

   return thrust;
}

// =================================================================================================
// protected
void WsfGuidedMover::ComputeVectoredThrustForces(double       aSimTime,
                                                 double       aCommandedForceY,
                                                 double       aCommandedForceZ,
                                                 const Stage& aStage,
                                                 double       aThrustForceECS[3]) const
{
   if (aStage.mMaxTVC_Angle == 0.0)
   {
      return; // Return immediately if thrust vectoring not defined
   }

   double phaseTime = aSimTime - mPhaseStartTime;
   if ((phaseTime < aStage.mMinTVC_Time) || (phaseTime > aStage.mMaxTVC_Time))
   {
      return; // Return immediately if not within the time window
   }

   double commandedForce = sqrt(aCommandedForceY * aCommandedForceY + aCommandedForceZ * aCommandedForceZ);
   if (commandedForce > 0.0)
   {
      // This is a 3-DOF point-mass model where changes in direction/orientation are performed by pushing the mass
      // around. In real life (6-DOF, non-point-mass) thrust-vectoring acts at the end of a lever arm in order to alter
      // the orientation. The latter is much more efficient and requires less thrust vectoring to achieve the same
      // change in direction. We need to capture this effect because otherwise we 'steal' to much of the thrust to
      // change direction, or worse, we don't have enough force to change direction.
      //
      // 'thrust_vectoring_effectiveness' provides some measure of the improvement between 3-DOF and 6-DOF.

      double maxEffectiveTVC_Force = aThrustForceECS[0] * std::sin(aStage.mMaxTVC_Angle) * aStage.mLateralThrustGain;
      // Assume thrust vectoring can completely satisfy the commanded force requirements.
      double appliedTVC_Force = commandedForce;
      double TVC_Factor       = 1.0;
      if (commandedForce > maxEffectiveTVC_Force)
      {
         // Thrust vectoring can only partially satisfy the commanded force requirements.
         appliedTVC_Force = maxEffectiveTVC_Force;
         TVC_Factor       = maxEffectiveTVC_Force / commandedForce;
      }
      // Note: In general the magnitude of the thrust will not change as a result of vectoring,
      // only the direction. If, however, 'lateral_thrust_gain' is employed then that statement
      // will not be true (See comments about 'lateral_thrust_gain' earlier in this routine.).
      // If 'lateral_thrust_gain' is employed then the axial thrust will be correct and the
      // lateral thrust components will be increased, resulting in an apparent increase of the
      // the thrust magnitude. This increase does NOTs cause an increase in fuel consumption,
      // however, because that was already computed based on the input thrust.
      appliedTVC_Force /= aStage.mLateralThrustGain;
      aThrustForceECS[1] = aCommandedForceY * TVC_Factor;
      aThrustForceECS[2] = aCommandedForceZ * TVC_Factor;
      aThrustForceECS[0] = sqrt(aThrustForceECS[0] * aThrustForceECS[0] - appliedTVC_Force * appliedTVC_Force);
   }
}

void WsfGuidedMover::ComputeAeroForces(double       aSimTime,
                                       double       aDeltaT,
                                       const Stage& aStage,
                                       const double aThrustForcesECS[3],
                                       double       aAeroForcesECS[3]) const
{
   if (aStage.mAeroPtr != nullptr)
   {
      double requiredForceY = 0.0;
      double requiredForceZ = 0.0;
      if (mGuidanceOn)
      {
         // Decrement the commanded guidance forces by the amount being satisfied by thrust vectoring.
         requiredForceY = mYForce - aThrustForcesECS[1];
         requiredForceZ = mZForce - aThrustForcesECS[2];
      }

      aStage.mAeroPtr->Update(aSimTime, requiredForceY, requiredForceZ, 0.0);
      aStage.mAeroPtr->GetECSForces(aAeroForcesECS);

      // Apply the missile time constant if supplied.
      //
      // Note that the time constant is applied only to the resulting aerodynamic forces.
      // It is NOT applied to thrust vectoring or divert thrust as they probably have different time constants.

      double timeConstant = GetTimeConstant(aStage);
      if (timeConstant > 0.0)
      {
         double dragForce = aAeroForcesECS[0]; // Drag is not affected by the time constant
         double f         = exp(-aDeltaT / timeConstant);
         // Xnew = (Xold * f) + ((1 - f) * Xcmd)
         UtVec3d::Multiply(aAeroForcesECS, 1.0 - f);                                 // (1 - f) * Xcmd ...
         UtVec3d::AddProduct(aAeroForcesECS, aAeroForcesECS, f, mLastAeroForcesECS); // ... + (Xold * f)
         aAeroForcesECS[0] = dragForce;
      }
   }
}

void WsfGuidedMover::ComputeThrustForCommandedSpeed(double       aDeltaT,
                                                    State&       aState,
                                                    const Stage& aStage,
                                                    const double aAeroForcesECS[3],
                                                    double       aThrustForcesECS[3]) const
{
   // This is very kludgey... it assumes instantaneous throttle response over a wide range - not likely =)
   if ((mCommandedSpeed >= 0.0) && (aDeltaT > 0.0))
   {
      // a = dv/dt
      double speed = mCurState.mSpeed;
      double accel = (mCommandedSpeed - speed) / aDeltaT;
      // TODO(akrisby): Need to limit acceleration.
      // F = T + D = ma, so T = ma - D.
      double neededThrust = (mCurState.mMass * accel) - aAeroForcesECS[0];

      // Note: This only affects the axial thrust component. Any lateral thrust components due to
      //       thrust vectoring are not changed.
      aThrustForcesECS[0] = std::min(aThrustForcesECS[0], std::max(0.0, neededThrust));

      // Update the fuel flow rate.
      aState.mCurrentThrust       = UtVec3d::Magnitude(aThrustForcesECS);
      aState.mCurrentFuelFlowRate = mCurState.mCurrentThrust / (aStage.mSpecificImpulse * UtEarth::cACCEL_OF_GRAVITY);
   }
}

void WsfGuidedMover::State::ComputeGravitationalForce(double aGravitationalForceWCS[3]) const
{
   UtVec3d::Set(aGravitationalForceWCS, mLocation);
   double r = UtVec3d::Magnitude(aGravitationalForceWCS);
   double g = UtEarth::cGRAVITATIONAL_PARAMETER / (r * r);
   UtVec3d::Multiply(aGravitationalForceWCS, -g / r);
}

// =================================================================================================
//! Get the aerodynamic time constant.
// protected
double WsfGuidedMover::GetTimeConstant(const Stage& aStage) const
{
   double timeConstant = 0.0;
   if (aStage.mTimeConstantPtr != nullptr) // Use the stage-specific time constant
   {
      timeConstant = aStage.mTimeConstantPtr->Lookup(mIV_Values);
   }
   else if (mTimeConstantPtr != nullptr) // Use the 'global' time constant
   {
      timeConstant = mTimeConstantPtr->Lookup(mIV_Values);
   }
   return timeConstant;
}

void WsfGuidedMover::ComputeFullForces(double aSimTime, double aBegTime, double aEndTime, State& aRHS_State, Stage& aRHS_Stage) const
{
   double deltaT = aEndTime - aBegTime;

   // Determine the thrust forces without any vectoring applied.
   double thrustForcesECS[3];
   ComputeThrustForces(aBegTime, deltaT, aRHS_State, aRHS_Stage, thrustForcesECS);

   // Determine if thrust vectoring can be employed to satisfy some (or all) of the commanded forces.
   ComputeVectoredThrustForces(aEndTime, mYForce, mZForce, aRHS_Stage, thrustForcesECS);

   // Determine the aerodynamics forces (drag and lateral forces to satisfy commanded forces.)
   double aeroForcesECS[3] = {0.0, 0.0, 0.0};
   ComputeAeroForces(aSimTime, deltaT, aRHS_Stage, thrustForcesECS, aeroForcesECS);
   UtVec3d::Set(aRHS_State.mLastAeroForcesECS, aeroForcesECS);

   // If a speed was commanded, compute the required thrust. This must be done after computing the
   // aerodynamic forces because that is where the drag is computed.
   ComputeThrustForCommandedSpeed(deltaT, aRHS_State, aRHS_Stage, aeroForcesECS, thrustForcesECS);

   // Applied forces do NOT include gravity...
   UtVec3d::Add(aRHS_State.mAppliedForcesECS, thrustForcesECS, aeroForcesECS);

   // If the commanded forces have not been achieved through aerodynamic and thrust vectoring forces,
   // determine if divert thrusters can achieve the residual...
   double divertThrustForcesECS[3];
   ComputeDivertForces(deltaT, aRHS_State, mYForce, mZForce, aRHS_State.mAppliedForcesECS, divertThrustForcesECS);

   // Total thrust forces (for scripting)
   UtVec3d::Add(aRHS_State.mLastThrustForcesECS, thrustForcesECS, divertThrustForcesECS);

   // Convert the ECS applied forces into WCS acceleration, and add the gravitational acceleration, setting
   // the acceleration of the provided state.
   aRHS_State.ComputeDerivatives(aRHS_State.mAppliedForcesECS);
}

void WsfGuidedMover::State::TakeEulerStep(double                aDeltaTime,
                                          const KinematicState& aDerivatives,
                                          const State&          aInitialState,
                                          bool                  aIntegrateMass)
{
   // Advance from InitialState by the supplied derivatives
   UtVec3d::AddProduct(mLocation, aInitialState.mLocation, aDeltaTime, aDerivatives.GetDxDt().GetData());

   double deltaVelWCS[3];
   UtVec3d::Multiply(deltaVelWCS, aDerivatives.GetDvDt().GetData(), aDeltaTime);
   UtVec3d::Add(mVelocity, aInitialState.mVelocity, deltaVelWCS);

   if (aIntegrateMass)
   {
      mCurrentFuelMass = std::max(0.0, mCurrentFuelMass - aDerivatives.GetDmDt() * aDeltaTime);
   }

   UpdateOtherStateData(aInitialState.mTime + aDeltaTime);
}

// =================================================================================================
// protected
void WsfGuidedMover::IntegrateEOM(double       aDeltaTime,
                                  const double aAppliedForcesECS[3],
                                  State&       aOutState,
                                  State&       aInState1,
                                  State&       aInState2)
{
   // Compute the derivatives at state 2.
   aInState2.ComputeDerivatives(aAppliedForcesECS);

   // Advance from state 1 to the new state using the velocity and accelerations at state 2.
   aOutState.mMoverPtr = this;
   aOutState.TakeEulerStep(aDeltaTime, aInState2.GetDerivatives(), aInState1);
}

// =================================================================================================
//! Jettison eject-ables that meet the criteria for ejection.
// protected
void WsfGuidedMover::JettisonEjectables(double aSimTime)
{
   double relTime          = aSimTime - mLaunchTime;
   bool   ejectionOccurred = false;

   for (Ejectable& ejectable : mEjectables)
   {
      if (ejectable.mMass > 0.0)
      {
         if ((relTime >= ejectable.mTime) || (mCurState.mAltitude > ejectable.mAltitude))
         {
            // Remove this eject-able mass from the payload mass.
            double payloadMass = GetPlatform()->GetPayloadMass();
            payloadMass -= ejectable.mMass;
            GetPlatform()->SetPayloadMass(payloadMass);
            if (mShowStatus)
            {
               auto out = ut::log::info() << "Platform jettisoning ejectable.";
               out.AddNote() << "T = " << std::fixed << std::setprecision(5) << aSimTime;
               out.AddNote() << "Since Launch: " << std::fixed << std::setprecision(5) << aSimTime - mLaunchTime;
               out.AddNote() << "Jettison: " << ejectable.mName;
               out.AddNote() << "Mass: " << ejectable.mMass << " kg";
               WriteKinematicStatus(out);
            }
            ejectable.mMass  = 0.0; // Indicate no longer used
            ejectionOccurred = true;
         }
      }
   }
   if (ejectionOccurred)
   {
      auto newEnd = std::remove_if(mEjectables.begin(), mEjectables.end(), HasBeenEjected);
      mEjectables.erase(newEnd, mEjectables.end());
   }
}

// =================================================================================================
// Select what should be the coordinate frame if desired coordinate frame is ECI (this doesn't actually change the data).
//
// Coordinate frame selection is encapsulated here so it can be used from both InitiateMotion and UpdateExternalFrame.
//
// @returns true if a frame switch should occur.
// protected
bool WsfGuidedMover::SelectCoordinateFrame(const State& aState, CoordinateFrame& aTargetFrame) const
{
   // If ECI coordinates are requested we want to use WCS coordinates when still in the atmosphere and switch to ECI
   // once we get out of the atmosphere. If we come back in the atmosphere we will switch back to WCS, but the threshold
   // is made a little lower so as to avoid bouncing between WCS and ECI.
   //
   // The use of WCS at lower altitudes is very critical because there is a large lateral component in the ECI velocity
   // and orienting to the velocity vector would give VERY unusual results. (See UpdateOrientationData).

   static const double cMIN_ALTITUDE_FOR_ECI = 100000.0; // Approximate upper bound of our atmosphere tables.

   aTargetFrame = aState.mCoordinateFrame;
   if (mDesiredCoordinateFrame == cCF_ECI)
   {
      // If the user requested ECI coordinates, WCS is used during the initial phase of flight or if the
      // platform reenters and becomes 'Earth bound' again.
      if (aState.mCoordinateFrame == cCF_ECI)
      {
         if (aState.mAltitude < (0.9 * cMIN_ALTITUDE_FOR_ECI))
         {
            aTargetFrame = cCF_WCS;
            return true;
         }
      }
      else if (aState.mCoordinateFrame == cCF_WCS)
      {
         // See if the criteria is met for switching to ECI from WCS.
         if (aState.mAltitude >= cMIN_ALTITUDE_FOR_ECI)
         {
            aTargetFrame = cCF_ECI;
            return true;
         }
      }
   }
   return false; // No switch necessary.
}

// =================================================================================================
// protected
void WsfGuidedMover::SwitchCoordinateFrame(State& aState, CoordinateFrame aTargetFrame)
{
   // The assumes the data in the supplied state is in the coordinate frame opposite the target frame.

   UtVec3d tempLoc;
   UtVec3d tempVel;
   UtVec3d tempAcl;
   mECI_UpdateTime = aState.mTime;

   auto& eciConv = GetECI_Conversion();
   if (aTargetFrame == cCF_ECI)
   {
      // Convert WCS to TOD
      eciConv.SetLocationWCS(aState.mLocation);
      eciConv.SetVelocityWCS(aState.mVelocity);
      eciConv.SetAccelerationWCS(aState.mAcceleration);
      tempLoc = eciConv.GetLocationTOD();
      tempVel = eciConv.GetVelocityTOD();
      tempAcl = eciConv.GetAccelerationTOD();
      double ECI_ToWCS_Transform[3][3];
      eciConv.GetECI_ToWCS_Transform(ECI_ToWCS_Transform);
      double ECI_ToECS_Transform[3][3];
      UtMat3d::Multiply(ECI_ToECS_Transform, aState.mWorld_ToECS_Transform, ECI_ToWCS_Transform);
      UtEntity::ExtractEulerAngles(ECI_ToECS_Transform, aState.mPsi, aState.mTheta, aState.mPhi);
      aState.mSpeed = UtVec3d::Magnitude(aState.mVelocity); // Speed is always the true WCS speed
   }
   else
   {
      // Convert TOD to WCS
      eciConv.SetLocationECI(aState.mLocation); // using same variable for both ECI and WCS?
      eciConv.SetVelocityECI(aState.mVelocity);
      eciConv.SetAccelerationECI(aState.mAcceleration);
      tempLoc = eciConv.GetLocationWCS();
      tempVel = eciConv.GetVelocityWCS();
      tempAcl = eciConv.GetAccelerationWCS();
      double WCS_ToECI_Transform[3][3];
      eciConv.GetWCS_ToECI_Transform(WCS_ToECI_Transform);
      double WCS_ToECS_Transform[3][3];
      // actually aState.mWorld_ToECS_Transform should be ECI_ToECS.
      //                                          // ECI_ToECS
      UtMat3d::Multiply(WCS_ToECS_Transform, aState.mWorld_ToECS_Transform, WCS_ToECI_Transform);
      UtEntity::ExtractEulerAngles(WCS_ToECS_Transform, aState.mPsi, aState.mTheta, aState.mPhi);
      aState.mSpeed = tempVel.Magnitude(); // Speed is always the true WCS speed
   }
   UtVec3d::Set(aState.mLocation, tempLoc.GetData());
   UtVec3d::Set(aState.mVelocity, tempVel.GetData());
   UtVec3d::Set(aState.mAcceleration, tempAcl.GetData());
   UtEntity::ComputeRotationalTransform(aState.mPsi, aState.mTheta, aState.mPhi, aState.mWorld_ToECS_Transform);

   // If the target frame is ECI and 'launch_to_orbit' and 'maintain_inclination' are true, remove any cross-track velocity
   // component introduced by the conversion of the WCS velocity to ECI. A cross-track component will exist whenever the
   // initial launch heading is something other than pure east/west. This will cause a very slight discontinuity in the
   // total velocity (worst case is about 0.8 for near north/south launch headings for high inclinations.)
   if ((aTargetFrame == cCF_ECI) && aState.mLaunchToOrbit && aState.mMaintainInclination)
   {
      // Convert velocity to ECS and zap the cross-track (Y) velocity.
      double oldSpeed = UtVec3d::Magnitude(aState.mVelocity);
      double velECS[3];
      UtMat3d::Transform(velECS, aState.mWorld_ToECS_Transform, aState.mVelocity);
      velECS[1] = 0.0;
      UtMat3d::InverseTransform(aState.mVelocity, aState.mWorld_ToECS_Transform, velECS);
      if (mShowStatus)
      {
         double deltaSpeed  = oldSpeed - UtVec3d::Magnitude(aState.mVelocity);
         double inclination = ComputeInclination(aState.mLocation, aState.mVelocity);

         auto out = ut::log::info() << "Platform removed cross track velocity to maintain inclination.";
         out.AddNote() << "T = " << std::fixed << std::setprecision(5) << aState.mTime;
         out.AddNote() << std::fixed << std::setprecision(5) << "Since Launch: " << aState.mTime - mLaunchTime << " sec";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << std::fixed << std::setprecision(2) << "Inclination: " << inclination * UtMath::cDEG_PER_RAD
                       << " deg";
         out.AddNote() << std::fixed << std::setprecision(2) << "Speed Delta: " << deltaSpeed << "m/s ("
                       << 100.0 * deltaSpeed / oldSpeed << "%)";
      }
   }
   aState.mCoordinateFrame = aTargetFrame;
}

// =================================================================================================
//! Propagate the internal spherical WCS values to ellipsoidal values that are used externally.
// protected
void WsfGuidedMover::UpdateExternalState(State& aState)
{
   double lat;
   double lon;
   double alt;
   double WCS_ToNED_Transform[3][3];
   double refLocWCS[3];
   double velNED[3];
   double aclNED[3];
   double psiNED;
   double thetaNED;
   double phiNED;

   // Determine if a coordinate frame switch is needed (WCS->ECI or ECI->WCS)
   bool switchedCoordinateFrame(false);
   if (mDesiredCoordinateFrame == cCF_ECI)
   {
      CoordinateFrame targetFrame;
      if (SelectCoordinateFrame(aState, targetFrame))
      {
         SwitchCoordinateFrame(aState, targetFrame);
         aState.mECI_SwitchTime  = aState.mTime;
         switchedCoordinateFrame = true;
      }
   }

   // We are going from a spherical to an ellipsoidal system. The position is converted from spherical
   // Cartesian to LLA for setting the ellipsoid position.  All other values are converted to spherical
   // NED and used to set NED values in the ellipsoidal frame.
   //
   // Note that if ECI is being used, the internal state is ECI but is called 'WCS'.

   UtSphericalEarth::ConvertECEFToLLA(aState.mLocation, lat, lon, alt);
   UtSphericalEarth::ComputeNEDTransform(lat, lon, alt, WCS_ToNED_Transform, refLocWCS);
   UtMat3d::Transform(velNED, WCS_ToNED_Transform, aState.mVelocity);
   UtMat3d::Transform(aclNED, WCS_ToNED_Transform, aState.mAcceleration);

   // Recovering the NED orientation is a little more difficult.
   // The following is patterned after UtEntity::UpdateNEDToECSTransform.

   double NED_ToWCS_Transform[3][3];
   UtMat3d::Transpose(NED_ToWCS_Transform, WCS_ToNED_Transform);
   double NED_ToECS_Transform[3][3];
   UtMat3d::Multiply(NED_ToECS_Transform, aState.mWorld_ToECS_Transform, NED_ToWCS_Transform);

   // ... and now we can get the orientation angles

   UtEntity::ExtractEulerAngles(NED_ToECS_Transform, psiNED, thetaNED, phiNED);

   if (aState.mCoordinateFrame == cCF_ECI)
   {
      // Use a temporary entity to convert from LLA/NED to ECI. The converted
      // values are then used to set ECI in the external state.

      double locTOD[3];
      double velTOD[3];
      double aclTOD[3];
      double oriECI[3];

      // TODO MJM This is likely wrong (re-examine).
      mTempEntity.SetLocationLLA(lat, lon, alt);
      mTempEntity.GetLocationWCS(locTOD);
      mTempEntity.SetVelocityNED(velNED);
      mTempEntity.GetVelocityWCS(velTOD);
      mTempEntity.SetAccelerationNED(aclNED);
      mTempEntity.GetAccelerationWCS(aclTOD);
      mTempEntity.SetOrientationNED(psiNED, thetaNED, phiNED);
      mTempEntity.GetOrientationWCS(oriECI[0], oriECI[1], oriECI[2]);
      mExternalState.SetLocationTOD(locTOD);
      mExternalState.SetVelocityTOD(velTOD);
      mExternalState.SetAccelerationTOD(aclTOD);
      mExternalState.SetOrientationECI(oriECI[0], oriECI[1], oriECI[2]);
   }
   else
   {
      mExternalState.SetLocationLLA(lat, lon, alt);
      mExternalState.SetVelocityNED(velNED);
      mExternalState.SetAccelerationNED(aclNED);
      mExternalState.SetOrientationNED(psiNED, thetaNED, phiNED);
   }

   if (switchedCoordinateFrame && mShowStatus)
   {
      {
         auto out = ut::log::info() << "Platform switched coordinate frame.";
         out.AddNote() << "T = " << std::fixed << std::setprecision(5) << aState.mTime;
         out.AddNote() << "Since Launch: " << aState.mTime - mLaunchTime << " sec";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Coordinate Frame: " << ((aState.mCoordinateFrame == cCF_WCS) ? "WCS" : "ECI");
      }
   }
}

// =================================================================================================
// protected
void WsfGuidedMover::State::UpdateOrientationData(double aSimTime, double aDeltaTime)
{
   // Update the orientation by aligning the X-axis with the velocity vector and the Y- and Z-axes
   // such that there is no side-slip or roll. (Canopy up and wings level)

   // Remember mVelocity is the WCS velocity if using the WCS coordinate frame and the ECI velocity if using the ECI
   // coordinate frame. And remember the magnitude is not important - we are only interested in the direction.

   double velVec[3];
   UtVec3d::Set(velVec, mVelocity);
   if (mCoordinateFrame == cCF_ECI)
   {
      // When switching from WCS to ECI there is a difference in direction in the WCS and ECI velocity vectors due to
      // the addition of the components due to Earth's rotation. Because we align the X axis to the velocity vector,
      // this would show as a slight discontinuity. Instead we blend the WCS and ECI velocity unit vectors over a period
      // of several seconds when forming the X axis.

      static const double cBLEND_TIME = 10.0;
      if (mTime < mECI_SwitchTime + cBLEND_TIME)
      {
         double fracECI = (mTime - mECI_SwitchTime) / cBLEND_TIME;
         fracECI        = UtMath::Limit(fracECI, 0.0, 1.0);

         double vecECI[3];
         UtVec3d::Normalize(vecECI, mVelocity);

         // Compute the velocity vector due to Earth's rotation.
         double vxRot = -mLocation[1] * UtEarth::cOMEGA;
         double vyRot = mLocation[0] * UtEarth::cOMEGA;
         double rotVelECI[3];
         UtVec3d::Set(rotVelECI, vxRot, vyRot, 0.0);

         // If 'maintain_inclination true' was specified, remove the out-of-orbit-plane components. If this isn't done
         // it causes the inclination angle to drift.
         if (mMaintainInclination && mLaunchToOrbit)
         {
            double rotVelECS[3];
            // This should be                     ECI_ToECS
            UtMat3d::Transform(rotVelECS, mWorld_ToECS_Transform, rotVelECI);
            rotVelECS[1] = 0.0;
            // This should be                            ECI_ToECS
            UtMat3d::InverseTransform(rotVelECI, mWorld_ToECS_Transform, rotVelECS);
         }

         // Subtract the Earth rotation velocity vector from the ECI velocity. If 'maintain_inclination' is false then
         // this is just the WCS velocity rotated back to the ECI position.
         double vecWCS_In_ECI[3];
         UtVec3d::Subtract(vecWCS_In_ECI, mVelocity, rotVelECI);
         UtVec3d::Magnitude(vecWCS_In_ECI);

         // Now blend the two together
         UtVec3d::Multiply(vecECI, fracECI);
         UtVec3d::Multiply(vecWCS_In_ECI, 1.0 - fracECI);
         UtVec3d::Add(velVec, vecECI, vecWCS_In_ECI);
         UtVec3d::Normalize(velVec);
      }
   }

   if (UtVec3d::Equals(velVec, 0.0))
   {
      // Use the last X axis as the velocity vector...
      UtVec3d::Set(velVec, mWorld_ToECS_Transform[0]);
   }

   // The rows of the WCS->ECS coordinate transform matrix are simply the basis vectors of the local frame in the WCS frame.

   // The ECS X axis is simply the normalized velocity vector.
   UtVec3d::Normalize(mWorld_ToECS_Transform[0], velVec);

   // 'locUnitVec' points from the object to the center of the Earth.
   double locUnitVec[3];
   UtVec3d::Multiply(locUnitVec, mLocation, -1.0); // Points to center of the Earth
   UtVec3d::Normalize(locUnitVec);

   // If vehicle is moving (nearly) vertical, the velocity vector and the position vector are
   // (nearly) co-linear. Normally the new Z axis is formed from the negated position vector and the
   // new Y axis is formed as Z cross X (the later being the new velocity vector). Unfortunately,
   // when X and Z are nearly collinear their cross product becomes very erratic and the orientation
   // goes crazy.

   double dot = UtVec3d::DotProduct(mWorld_ToECS_Transform[0], locUnitVec);
   if (fabs(dot) > 0.999999)
   {
      // Use an alternative formulation for (nearly) vertical flight.

      double velUnitVec[3];
      UtVec3d::Set(velUnitVec, mWorld_ToECS_Transform[0]);
      if (dot < 0.0)
      {
         UtVec3d::Multiply(locUnitVec, locUnitVec, -1.0);
      }

      // Form a temporary Y from the last Z axis and the location vector.
      UtVec3d::CrossProduct(mWorld_ToECS_Transform[1], mWorld_ToECS_Transform[2], locUnitVec);
      UtVec3d::Normalize(mWorld_ToECS_Transform[1]);

      // Formulate an updated Z using the location vector and the Y axis computed above
      UtVec3d::CrossProduct(mWorld_ToECS_Transform[2], locUnitVec, mWorld_ToECS_Transform[1]);
      UtVec3d::Normalize(mWorld_ToECS_Transform[2]);

      UtVec3d::Set(mWorld_ToECS_Transform[0], locUnitVec);
   }
   else
   {
      // Accept the normalized/negated position vector as the proposed Z-axis.
      UtVec3d::Set(mWorld_ToECS_Transform[2], locUnitVec);

      // The ECS Y axis is computed as Z cross X
      UtVec3d::CrossProduct(mWorld_ToECS_Transform[1], mWorld_ToECS_Transform[2], mWorld_ToECS_Transform[0]);
      UtVec3d::Normalize(mWorld_ToECS_Transform[1]);

      // The ECS Z axis is just X cross Y
      UtVec3d::CrossProduct(mWorld_ToECS_Transform[2], mWorld_ToECS_Transform[0], mWorld_ToECS_Transform[1]);
      UtVec3d::Normalize(mWorld_ToECS_Transform[2]);
   }

   UtEntity::ExtractEulerAngles(mWorld_ToECS_Transform, mPsi, mTheta, mPhi);
}

// =================================================================================================
// protected
void WsfGuidedMover::State::UpdateOtherStateData(double aSimTime)
{
   double deltaTime = aSimTime - mTime;
   mTime            = aSimTime;

   // Update the WCS->ECS transform and use it to get the current Euler angles.

   UpdateOrientationData(aSimTime, deltaTime);

   // Compute the altitude and speed for atmospheric data.

   // The speed is always the true WCS speed even when ECI coordinates are being used.
   // It is really only used for in-atmosphere calculations.
   if (mCoordinateFrame == cCF_WCS)
   {
      mSpeed = UtVec3d::Magnitude(mVelocity);
   }
   else
   {
      double velWCS[3];
      mMoverPtr->GetPlatform()->ConvertVelocityECIToWCS(mLocation, mVelocity, velWCS);
      mSpeed = UtVec3d::Magnitude(velWCS);
   }
   mAltitude = UtVec3d::Magnitude(mLocation) - UtSphericalEarth::cEARTH_RADIUS;
   mMach     = mSpeed / mMoverPtr->mAtmosphere.SonicVelocity(mAltitude);

   if (mMoverPtr->DebugEnabled())
   {
      auto out = ut::log::debug() << "Updating WsfGuidedMover state data.";
      Print(out, mMoverPtr->mFMM_Time);
   }
}

// =================================================================================================
WsfGuidedMover::Stage::Stage()
   : mEngineType(cROCKET)
   , mThrustReference(cDEFAULT)
   , mImpulseReference(cDEFAULT)
   , mEmptyMass(0.0)
   , mFuelMass(0.0)
   , mTotalMass(0.0)
   , mThrust(0.0)
   , mThrustSeaLevel(0.0)
   , mThrustVacuum(0.0)
   , mThrustTablePtr(nullptr)
   , mSeaLevelThrustTablePtr(nullptr)
   , mVacuumThrustTablePtr(nullptr)
   , mNozzleExitArea(0.0)
   , mSpecificImpulse(0.0)
   , mSpecificImpulseSeaLevel(0.0)
   , mSpecificImpulseVacuum(0.0)
   , mBurnRate(0.0)
   , mBurnRateTablePtr(nullptr)
   , mThrottlePtr(nullptr)
   , mThrustDuration(0.0)
   , mPreIgnitionCoastTime(0.0)
   , mPreSeparationCoastTime(0.0)
   , mAeroId(nullptr)
   , mAeroPtr(nullptr)
   , mTimeConstantPtr(nullptr)
   , mAngleOfAttackPtr(nullptr)
   , mMinTVC_Time(0.0)
   , mMaxTVC_Time(DBL_MAX)
   , mMaxTVC_Angle(0.0)
   , mLateralThrustGain(1.0)
   , mIntegrationTimestep(0.0)
   , mProbabilityOfSeparation(1.0)
   , mProbabilityOfIgnition(1.0)
   , mTurnMethod(cSKID_TO_TURN)
   , mReverseThrust(false)
   , mFinalBoostStage(false)
   , mPayloadMass(0.0)
{
}

// =================================================================================================
WsfGuidedMover::Stage::Stage(const Stage& aSrc)
   : mEngineType(aSrc.mEngineType)
   , mThrustReference(aSrc.mThrustReference)
   , mImpulseReference(aSrc.mImpulseReference)
   , mEmptyMass(aSrc.mEmptyMass)
   , mFuelMass(aSrc.mFuelMass)
   , mTotalMass(aSrc.mTotalMass)
   , mThrust(aSrc.mThrust)
   , mThrustSeaLevel(aSrc.mThrustSeaLevel)
   , mThrustVacuum(aSrc.mThrustVacuum)
   , mThrustTablePtr(aSrc.mThrustTablePtr)
   , mSeaLevelThrustTablePtr(aSrc.mSeaLevelThrustTablePtr)
   , mVacuumThrustTablePtr(aSrc.mVacuumThrustTablePtr)
   , mNozzleExitArea(aSrc.mNozzleExitArea)
   , mSpecificImpulse(aSrc.mSpecificImpulse)
   , mSpecificImpulseSeaLevel(aSrc.mSpecificImpulseSeaLevel)
   , mSpecificImpulseVacuum(aSrc.mSpecificImpulseVacuum)
   , mBurnRate(aSrc.mBurnRate)
   , mBurnRateTablePtr(aSrc.mBurnRateTablePtr)
   , mThrottlePtr(aSrc.mThrottlePtr)
   , mThrustDuration(aSrc.mThrustDuration)
   , mPreIgnitionCoastTime(aSrc.mPreIgnitionCoastTime)
   , mPreSeparationCoastTime(aSrc.mPreSeparationCoastTime)
   , mAeroId(aSrc.mAeroId)
   , mAeroPtr(nullptr)
   , mTimeConstantPtr(aSrc.mTimeConstantPtr)
   , mAngleOfAttackPtr(aSrc.mAngleOfAttackPtr)
   , mMinTVC_Time(aSrc.mMinTVC_Time)
   , mMaxTVC_Time(aSrc.mMaxTVC_Time)
   , mMaxTVC_Angle(aSrc.mMaxTVC_Angle)
   , mLateralThrustGain(aSrc.mLateralThrustGain)
   , mIntegrationTimestep(aSrc.mIntegrationTimestep)
   , mProbabilityOfSeparation(aSrc.mProbabilityOfSeparation)
   , mProbabilityOfIgnition(aSrc.mProbabilityOfIgnition)
   , mTurnMethod(aSrc.mTurnMethod)
   , mReverseThrust(aSrc.mReverseThrust)
   , mFinalBoostStage(aSrc.mFinalBoostStage)
   , mPayloadMass(aSrc.mPayloadMass)
{
}

// =================================================================================================
WsfGuidedMover::Stage::~Stage()
{
   delete mAeroPtr;
}

// =================================================================================================
//! Complete paired values.
//! If scalar reference values of thrust or specific impulse are employed this routine will attempt to
//! derive any missing values and verify the consistency of known values. This routine may be
//! called multiple times, but at the end, if ANY reference values were defined then ALL of them
//! should be known and the reference for both pairs should be 'cPAIRED'.
//!
//! This routine does nothing if thrust tables are employed.
//!
//! @param aMoverPtr Pointer to the parent mover.
//! @returns true if successful, false if not.
bool WsfGuidedMover::Stage::CompletePairedValues(WsfGuidedMover* aMoverPtr)
{
   bool ok = true;

   double Psl = aMoverPtr->mAtmosphere.Pressure(0.0);

   // If nozzle_exit_area was not specified do the following:
   // - If three of the four of the referenced thrust/specific_impulse values was given
   //   compute the missing value using ratios.
   // - If the thrust reference pair is defined or derived, compute nozzle_exit_area.

   if (mNozzleExitArea == 0.0)
   {
      if ((mThrustReference == cPAIRED) && (mImpulseReference == cVACUUM))
      {
         mSpecificImpulseSeaLevel = mSpecificImpulseVacuum * (mThrustSeaLevel / mThrustVacuum);
         mImpulseReference        = cPAIRED;
      }
      else if ((mThrustReference == cPAIRED) && (mImpulseReference == cSEA_LEVEL))
      {
         mSpecificImpulseVacuum = mSpecificImpulseSeaLevel * (mThrustVacuum / mThrustSeaLevel);
         mImpulseReference      = cPAIRED;
      }
      else if ((mThrustReference == cVACUUM) && (mImpulseReference == cPAIRED))
      {
         mThrustSeaLevel  = mThrustVacuum * (mSpecificImpulseSeaLevel / mSpecificImpulseVacuum);
         mThrustReference = cPAIRED;
      }
      else if ((mThrustReference == cSEA_LEVEL) && (mImpulseReference == cPAIRED))
      {
         mThrustVacuum    = mThrustSeaLevel * (mSpecificImpulseVacuum / mSpecificImpulseSeaLevel);
         mThrustReference = cPAIRED;
      }

      // If the thrust reference pair was specified compute nozzle_exit_area.
      if (mThrustReference == cPAIRED)
      {
         mNozzleExitArea = (mThrustVacuum - mThrustSeaLevel) / Psl;
      }
   }

   if (mNozzleExitArea != 0.0)
   {
      // If only one of the thrust reference values is given derive the other.
      if ((mThrustReference == cVACUUM) && (mThrustSeaLevel == 0.0))
      {
         mThrustSeaLevel = mThrustVacuum - mNozzleExitArea * Psl;
      }
      else if ((mThrustReference == cSEA_LEVEL) && (mThrustVacuum == 0.0))
      {
         mThrustVacuum = mThrustSeaLevel + mNozzleExitArea * Psl;
      }
      else if (mThrustReference == cPAIRED)
      {
         // Ensure the exit area computed from the paired values is consistent with the explicit value.
         double derivedExitArea = (mThrustVacuum - mThrustSeaLevel) / Psl;
         if (std::abs(derivedExitArea - mNozzleExitArea) / mNozzleExitArea > 0.01)
         {
            auto out = ut::log::error() << "The input value of 'nozzle_exit_area' is not consistent with the value of "
                                           "the derived from 'vacuum_thrust' and 'sea_level_thrust'.";
            out.AddNote() << "Input: " << mNozzleExitArea << " m^2";
            out.AddNote() << "Derived: " << derivedExitArea << " m^2";
            out.AddNote() << "One of the thrust values should be deleted or adjusted.";
            ok = false;
         }
      }

      // If only one of the specific_impulse reference values is given derive the other.
      // This assumes that the respective thrust reference value was either computed or derived.
      if ((mImpulseReference == cVACUUM) && (mSpecificImpulseSeaLevel == 0.0) && (mThrustVacuum != 0.0))
      {
         double f = (mThrustVacuum - mNozzleExitArea * Psl) / mThrustVacuum;
         if (f > 0.0)
         {
            mSpecificImpulseSeaLevel = mSpecificImpulseVacuum * f;
         }
         else
         {
            ut::log::error() << "Unable to calculate 'sea_level_specific_impulse'.";
            ok = false;
         }
      }
      else if ((mImpulseReference == cSEA_LEVEL) && (mSpecificImpulseVacuum == 0.0) && (mThrustSeaLevel != 0.0))
      {
         double f = (mThrustSeaLevel + mNozzleExitArea * Psl) / mThrustSeaLevel;
         if (f > 0.0)
         {
            mSpecificImpulseVacuum = mSpecificImpulseSeaLevel * f;
         }
         else
         {
            ut::log::error() << "Unable to calculate 'vacuum_specific_impulse'.";
            ok = false;
         }
      }
      else if (mImpulseReference == cPAIRED)
      {
         // TODO(akrisby): _JAJ what sort of checks here???
      }
   }

   // If both thrust and specific impulse were specified using paired references, ensure they
   // have the same ratio.
   if ((mThrustReference == cPAIRED) && (mImpulseReference == cPAIRED))
   {
      double thrustRatio          = mThrustVacuum / mThrustSeaLevel;
      double specificImpulseRatio = mSpecificImpulseVacuum / mSpecificImpulseSeaLevel;
      if (std::abs(thrustRatio - specificImpulseRatio) > 0.01)
      {
         auto out = ut::log::error() << "Ratios computed are not equal and should be!";
         out.AddNote() << "(Vacuum Thrust) / (Sea Level Thrust): " << thrustRatio;
         out.AddNote() << "(Vacuum Specified Impulse) / (Sea Level Specified Impulse): " << specificImpulseRatio;
         out.AddNote() << "The solution is to either correct one of the thrust or specific_impulse values or to remove "
                          "one of them and specify the correct nozzle exit area.";
         ok = false;
      }
   }

   return ok;
}

// =================================================================================================
//! Compute the time-weighted average of a function represented by the supplied curve.
//! It assumes the independent variable is 'time'.
double WsfGuidedMover::Stage::ComputeTimeWeightedAverage(const UtTable::Curve& aCurve) const
{
   // Trapezoidal integration
   std::vector<double> iv;
   std::vector<double> dv;
   double              sum = 0.0;
   aCurve.GetVariable("*", iv); // "*" means the one and only independent variable, regardless of name
   aCurve.GetVariable("", dv);  // "" for the name means the dependent variable
   for (size_t i = 1; i < iv.size(); ++i)
   {
      sum += (0.5 * (dv[i] + dv[i - 1])) * (iv[i] - iv[i - 1]);
   }
   // The denominator should ALWAYS be non-zero.
   return sum / (iv.back() - iv.front());
}

// =================================================================================================
//! Compute unknown variables.
//! This routine recursively calls itself attempting to determine as many unknown variables as possible.
//! It is first called from Stage::Initialize with only the input values known.
bool WsfGuidedMover::Stage::ComputeUnknownValues(WsfGuidedMover* aMoverPtr, int& aRecursionDepth)
{
   // First derive any parts of paired references that can be determined from the current state.
   // This is done upon entry so this routine has the most up-to-date state.
   if (!CompletePairedValues(aMoverPtr))
   {
      return false;
   }

   bool recompute = false;

   // See if any mass related values can be computed.

   if ((mEmptyMass == 0.0) && (mFuelMass != 0.0) && (mTotalMass != 0.0))
   {
      mEmptyMass = mTotalMass - mFuelMass;
      recompute  = true;
   }
   else if ((mEmptyMass != 0.0) && (mFuelMass == 0.0) && (mTotalMass != 0.0))
   {
      mFuelMass = mTotalMass - mEmptyMass;
      recompute = true;
   }
   else if ((mEmptyMass != 0.0) && (mFuelMass != 0.0) && (mTotalMass == 0.0))
   {
      mTotalMass = mEmptyMass + mFuelMass;
      recompute  = true;
   }
   else if ((mFuelMass == 0.0) && (mBurnRate != 0.0) && (mThrustDuration != 0.0))
   {
      mFuelMass = mBurnRate * mThrustDuration;
      recompute = true;
   }

   // Now check the propulsion related values need computing.

   if ((mThrust == 0.0) && (mBurnRate != 0.0) && (mSpecificImpulse != 0.0))
   {
      // The derived thrust will have the same reference as the specific impulse
      mThrustReference = mImpulseReference;
      switch (mImpulseReference)
      {
      case cVACUUM:
         mThrustVacuum = mSpecificImpulseVacuum * mBurnRate * UtEarth::cACCEL_OF_GRAVITY;
         mThrust       = mThrustVacuum;
         break;
      case cSEA_LEVEL:
         mThrustSeaLevel = mSpecificImpulseSeaLevel * mBurnRate * UtEarth::cACCEL_OF_GRAVITY;
         mThrust         = mThrustSeaLevel;
         break;
      case cPAIRED:
         mThrustVacuum   = mSpecificImpulseVacuum * mBurnRate * UtEarth::cACCEL_OF_GRAVITY;
         mThrustSeaLevel = mSpecificImpulseSeaLevel * mBurnRate * UtEarth::cACCEL_OF_GRAVITY;
         mThrust         = mThrustVacuum;
         break;
      default:
         mThrust = mSpecificImpulse * mBurnRate * UtEarth::cACCEL_OF_GRAVITY;
         break;
      }
      recompute = true;
   }
   else if ((mThrust != 0.0) && (mBurnRate == 0.0) && (mSpecificImpulse != 0.0))
   {
      if ((mThrustReference == cDEFAULT) && (mImpulseReference == cDEFAULT))
      {
         mBurnRate = mThrust / mSpecificImpulse / UtEarth::cACCEL_OF_GRAVITY;
      }
      else if ((mThrustVacuum > 0.0) && (mSpecificImpulseVacuum > 0.0))
      {
         mBurnRate = mThrustVacuum / mSpecificImpulseVacuum / UtEarth::cACCEL_OF_GRAVITY;
      }
      else if ((mThrustSeaLevel > 0.0) && (mSpecificImpulseSeaLevel > 0.0))
      {
         mBurnRate = mThrustSeaLevel / mSpecificImpulseSeaLevel / UtEarth::cACCEL_OF_GRAVITY;
      }
      else
      {
         ut::log::warning() << "Potentially erroneous 'burn_rate' calculated.";
         mBurnRate = mThrust / mSpecificImpulse / UtEarth::cACCEL_OF_GRAVITY;
      }
      recompute = true;
   }
   else if ((mThrust != 0.0) && (mBurnRate != 0.0) && (mSpecificImpulse == 0.0))
   {
      // The derived specific_impulse will have the same reference as the thrust
      mImpulseReference = mThrustReference;
      switch (mThrustReference)
      {
      case cVACUUM:
         mSpecificImpulseVacuum = mThrustVacuum / mBurnRate / UtEarth::cACCEL_OF_GRAVITY;
         mSpecificImpulse       = mSpecificImpulseVacuum;
         break;
      case cSEA_LEVEL:
         mSpecificImpulseSeaLevel = mThrustSeaLevel / mBurnRate / UtEarth::cACCEL_OF_GRAVITY;
         mSpecificImpulse         = mSpecificImpulseSeaLevel;
         break;
      case cPAIRED:
         mSpecificImpulseVacuum   = mThrustVacuum / mBurnRate / UtEarth::cACCEL_OF_GRAVITY;
         mSpecificImpulseSeaLevel = mThrustSeaLevel / mBurnRate / UtEarth::cACCEL_OF_GRAVITY;
         mSpecificImpulse         = mSpecificImpulseVacuum;
         break;
      default:
         mSpecificImpulse = mThrust / mBurnRate / UtEarth::cACCEL_OF_GRAVITY;
         break;
      }
      recompute = true;
   }
   else if ((mFuelMass != 0.0) && (mBurnRate == 0.0) && (mThrustDuration != 0.0))
   {
      mBurnRate = mFuelMass / mThrustDuration;
      recompute = true;
   }
   else if ((mFuelMass != 0.0) && (mBurnRate != 0.0) && (mThrustDuration == 0.0))
   {
      mThrustDuration = mFuelMass / mBurnRate;
      recompute       = true;
   }

   // Recursively call again if a value was computed.
   // The recursion depth is limited to prevent infinite recursion if input values would cause it...
   bool ok = true;
   if (recompute && (aRecursionDepth <= 10))
   {
      ++aRecursionDepth;
      ok = ComputeUnknownValues(aMoverPtr, aRecursionDepth);
      --aRecursionDepth;
   }
   return ok;
}

// =================================================================================================
//! Initialize the stage.
//! @param aSimTime     The current simulation time.
//! @param aMoverPtr    The pointer to the mover.
//! @param aShowStatus  If 'show_status' was enabled for the mover.
//! @returns true if successful, false if not.
bool WsfGuidedMover::Stage::Initialize(double aSimTime, WsfGuidedMover* aMoverPtr, bool aShowStatus)
{
   bool ok = true;

   // If tables were given for thrust or burn rate, set the scalar value to the time-weighted average value
   // for subsequent calculations.

   if (mThrustTablePtr != nullptr)
   {
      mThrust = ComputeTimeWeightedAverage(*mThrustTablePtr);
   }
   if (mBurnRateTablePtr != nullptr)
   {
      mBurnRate = ComputeTimeWeightedAverage(*mBurnRateTablePtr);
   }

   // For vacuum and sea level tables we use the first entry instead of the average if paired thrust values are given.
   // They should be consistent in order to compute the effective nozzle area...
   if (mVacuumThrustTablePtr != nullptr)
   {
      if (mThrustReference == cPAIRED)
      {
         double maxValue;
         mVacuumThrustTablePtr->GetVariableLimits("", mThrustVacuum, maxValue); // "" means the dependent variable
      }
      else
      {
         mThrustVacuum = ComputeTimeWeightedAverage(*mVacuumThrustTablePtr);
      }
      mThrust = mThrustVacuum;
   }
   if (mSeaLevelThrustTablePtr != nullptr)
   {
      if (mThrustReference == cPAIRED)
      {
         double maxValue;
         mSeaLevelThrustTablePtr->GetVariableLimits("", mThrustSeaLevel, maxValue); // "" means the depended variable
      }
      else
      {
         mThrustSeaLevel = ComputeTimeWeightedAverage(*mSeaLevelThrustTablePtr);
      }
      mThrust = mThrustSeaLevel;
   }

   // If the scalar thrust duration was not defined but a thrust or burn rate tables was defined, use the maximum
   // from whatever time-based tables are specified.
   // time value first from burn rate table, or from the thrust table.
   if (mThrustDuration == 0.0)
   {
      double minValue;
      double maxValue;
      if (mBurnRateTablePtr != nullptr)
      {
         mBurnRateTablePtr->GetVariableLimits(minValue, maxValue);
         mThrustDuration = std::max(mThrustDuration, maxValue);
      }
      if (mThrustTablePtr != nullptr)
      {
         mThrustTablePtr->GetVariableLimits(minValue, maxValue);
         mThrustDuration = std::max(mThrustDuration, maxValue);
      }
      if (mVacuumThrustTablePtr != nullptr)
      {
         mVacuumThrustTablePtr->GetVariableLimits(minValue, maxValue);
         mThrustDuration = std::max(mThrustDuration, maxValue);
      }
      if (mSeaLevelThrustTablePtr != nullptr)
      {
         mSeaLevelThrustTablePtr->GetVariableLimits(minValue, maxValue);
         mThrustDuration = std::max(mThrustDuration, maxValue);
      }
      if ((mThrottlePtr != nullptr) && mThrottlePtr->GetVariableLimits("time", minValue, maxValue))
      {
         mThrustDuration = std::max(mThrustDuration, maxValue);
      }
   }

   // Notes:
   // 1. Use the user's input values when specified, rather than calculated ones.
   // 2. Flag a value that seems to not add up with a "false" boolean return (this value MAY be ignored).
   // 3. If #1 forces some calculated mass value negative, set mTotalMass to -1.0 (this is a show-stopping failure).
   // 4. If user input is not sufficient to come up with proper weights, ditto.

   const double cMASS_TOLERANCE = 2.0;

   // First, consider the trivial NO PROPULSION case:
   if ((mThrust == 0.0) && (mBurnRate == 0.0) && (mSpecificImpulse == 0.0) &&
       ((mEmptyMass == 0.0) || (mEmptyMass == mTotalMass)) && (mFuelMass == 0.0) && (mThrustDuration == 0.0))
   {
      // This used to abort if total_mass == 0.0, but this was removed because massless stages are
      // used to implement coasting with a change of aero (e.g.: ramjet cuts out and causes more drag)
      mEmptyMass = mTotalMass;
   }
   else
   {
      // Derive unknown values from known values. Sorry for the nastiness, but this recursive process
      // gives the user a lot of flexibility regarding which type of data is used.
      int recursionLevel = 0;
      ok &= ComputeUnknownValues(aMoverPtr, recursionLevel);
      ok &= CompletePairedValues(aMoverPtr);

      // Ensure nozzle_exit_area was specified if it is needed. If scalar pairs are used everything
      // should be cPAIRED and the nozzle_exit_area should have been computed.
      if (mNozzleExitArea == 0.0)
      {
         if ((mThrustReference == cVACUUM) || (mThrustReference == cSEA_LEVEL))
         {
            ut::log::error() << "'nozzle_exit_area' must be specified for 'vacuum_thrust' or 'sea_level_thrust'.";
            ok = false;
         }
      }

      // Make sure all values have been specified or computed and are self-consistent.

      if ((mEmptyMass <= 0.0) || (mFuelMass <= 0.0) || (mTotalMass <= 0.0))
      {
         ut::log::error() << "Unable to determine 'empty_mass', 'fuel_mass', or 'total_mass'.";
         ok = false;
      }
      else if ((mEmptyMass >= mTotalMass) || (mFuelMass >= mTotalMass) ||
               (fabs(mTotalMass - mEmptyMass - mFuelMass) > cMASS_TOLERANCE))
      {
         ut::log::error() << "Inconsistent 'total_mass', 'empty_mass', and 'fuel_mass'.";
         ok = false;
      }

      if ((mThrust <= 0.0) || (mBurnRate <= 0.0) || (mSpecificImpulse <= 0.0) || (mThrustDuration <= 0.0))
      {
         ut::log::error() << "Unable to determine 'thrust', 'burn_rate', or 'specific_impulse'.";
         ok = false;
      }
      else if ((mThrustTablePtr != nullptr) || (mSeaLevelThrustTablePtr != nullptr) ||
               (mVacuumThrustTablePtr != nullptr) || (mBurnRateTablePtr != nullptr) || (mThrottlePtr != nullptr))
      {
         // If the user employed thrust_table, sea_level_thrust_table, vacuum_thrust_table, burn_rate_table or
         // throttle_table, we cannot reasonably verify they are consistent. This is especially true if the user is
         // providing scripted throttle control.
      }
      else
      {
         double thrust;
         if ((mThrustVacuum != mThrustSeaLevel))
         {
            thrust = mSpecificImpulseVacuum * mBurnRate * UtEarth::cACCEL_OF_GRAVITY;
            if (std::abs(thrust - mThrustVacuum) / mThrustVacuum > 0.02)
            {
               ut::log::error() << "Inconsistent 'vacuum_thrust', 'vacuum_specific_impulse', and 'burn_rate'.";
               ok = false;
            }
            thrust = mSpecificImpulseSeaLevel * mBurnRate * UtEarth::cACCEL_OF_GRAVITY;
            if (std::abs(thrust - mThrustSeaLevel) / mThrustSeaLevel > 0.02)
            {
               ut::log::error() << "Inconsistent 'sea_level_thrust', 'sea_level_specific_impulse', and 'burn_rate'.";
               ok = false;
            }
         }
         else
         {
            thrust = mSpecificImpulse * mBurnRate * UtEarth::cACCEL_OF_GRAVITY;
            if ((fabs(thrust - mThrust) / mThrust) > 0.02)
            {
               ut::log::error() << "Inconsistent 'thrust', 'burn_rate', and 'specific_impulse'.";
               ok = false;
            }
         }

         // Ensure that the fuel consumed does not exceed the fuel available.
         double fuelConsumed = mBurnRate * mThrustDuration;
         if (fuelConsumed > (mFuelMass + cMASS_TOLERANCE))
         {
            ut::log::error() << "Fuel consumed is greater than 'fuel_mass'.";
            ok = false;
         }
      }
   }

   // Initialize the aerodynamic properties

   if (!mAeroId.IsNull())
   {
      if (mAeroId != "none")
      {
         mAeroPtr = WsfAeroTypes::Get(aMoverPtr->GetScenario()).Clone(mAeroId);
         if (mAeroPtr == nullptr)
         {
            ut::log::error() << "'aero' type: " << mAeroId;
            ok = false;
         }
         else if (!mAeroPtr->Initialize(aSimTime, aMoverPtr->GetPlatform()))
         {
            ok = false;
         }
      }
   }
   else
   {
      ut::log::error() << "'aero <aero-type>' or 'aero none' must be specified.";
      ok = false;
   }
   return ok;
}

// =================================================================================================
bool WsfGuidedMover::Stage::ProcessInput(UtInput& aInput)
{
   // NOTE: In many places values of zero are allowed even though it may not make perfect sense.  This is used when
   // creating a derived type
   //       that wants to override or disable something. Setting it to zero makes it appears as though it was never
   //       defined in the base type.

   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "aero")
   {
      std::string aeroName;
      aInput.ReadValue(aeroName);
      mAeroId = aeroName;
   }
   else if ((command == "total_mass") || (command == "launch_mass") || // left for backward compatibility
            (command == "mass") ||                                     // left for backward compatibility
            (command == "initial_mass"))                               // left for backward compatibility
   {
      // Note that input of Mass or Weight is required, but not both.
      // One will overwrite the other...
      aInput.ReadValueOfType(mTotalMass, UtInput::cMASS);
      aInput.ValueGreaterOrEqual(mTotalMass, 0.0); // Zero is allow to implement coasting stages
   }
   else if (command == "weight")
   {
      // Note that input of Mass or Weight is required, but not both!
      // One will overwrite the other...
      double weight;
      aInput.ReadValueOfType(weight, UtInput::cFORCE);
      aInput.ValueGreaterOrEqual(weight, 0.0); // Zero is allow to implement coasting stages
      mTotalMass = weight / UtEarth::cACCEL_OF_GRAVITY;
   }
   else if (command == "empty_mass")
   {
      aInput.ReadValueOfType(mEmptyMass, UtInput::cMASS);
      aInput.ValueGreaterOrEqual(mEmptyMass, 0.0);
   }
   else if (command == "fuel_mass")
   {
      aInput.ReadValueOfType(mFuelMass, UtInput::cMASS);
      aInput.ValueGreaterOrEqual(mFuelMass, 0.0);
   }
   else if (command == "thrust")
   {
      mThrustTablePtr         = nullptr;
      mVacuumThrustTablePtr   = nullptr;
      mSeaLevelThrustTablePtr = nullptr;
      aInput.ReadValueOfType(mThrust, UtInput::cFORCE);
      aInput.ValueGreaterOrEqual(mThrust, 0.0); // zero makes it as though wasn't supplied
      mThrustReference = cDEFAULT;
      mThrustVacuum    = 0.0;
      mThrustSeaLevel  = 0.0;
   }
   else if (command == "vacuum_thrust")
   {
      mThrustTablePtr       = nullptr;
      mVacuumThrustTablePtr = nullptr;
      aInput.ReadValueOfType(mThrust, UtInput::cFORCE);
      aInput.ValueGreaterOrEqual(mThrust, 0.0); // zero makes it as though wasn't supplied
      mThrustVacuum = mThrust;
      if (mThrustReference == cSEA_LEVEL)
      {
         mThrustReference = cPAIRED;
      }
      else
      {
         mThrustReference = cVACUUM;
      }
   }
   else if (command == "sea_level_thrust")
   {
      mThrustTablePtr         = nullptr;
      mSeaLevelThrustTablePtr = nullptr;
      aInput.ReadValueOfType(mThrust, UtInput::cFORCE);
      aInput.ValueGreaterOrEqual(mThrust, 0.0); // zero makes it as though wasn't supplied
      mThrustSeaLevel = mThrust;
      if (mThrustReference == cVACUUM)
      {
         mThrustReference = cPAIRED;
      }
      else
      {
         mThrustReference = cSEA_LEVEL;
      }
   }
   else if (command == "thrust_table")
   {
      mSeaLevelThrustTablePtr = nullptr;
      mVacuumThrustTablePtr   = nullptr;
      mThrustReference        = cDEFAULT;
      mThrustTablePtr =
         UtTable::LoadCurve(aInput, "time", UtInput::cTIME, "", UtTable::ValueGE(0.0), UtInput::cFORCE, "", UtTable::ValueGE(0.0));
      mThrust         = 0.0;
      mThrustVacuum   = 0.0;
      mThrustSeaLevel = 0.0;
   }
   else if (command == "vacuum_thrust_table")
   {
      mThrustTablePtr = nullptr;
      mVacuumThrustTablePtr =
         UtTable::LoadCurve(aInput, "time", UtInput::cTIME, "", UtTable::ValueGE(0.0), UtInput::cFORCE, "", UtTable::ValueGE(0.0));
      if (mThrustReference == cSEA_LEVEL)
      {
         mThrustReference = cPAIRED;
      }
      else
      {
         mThrustReference = cVACUUM;
      }
      mThrust         = 0.0;
      mThrustVacuum   = 0.0;
      mThrustSeaLevel = 0.0;
   }
   else if (command == "sea_level_thrust_table")
   {
      mThrustTablePtr = nullptr;
      mSeaLevelThrustTablePtr =
         UtTable::LoadCurve(aInput, "time", UtInput::cTIME, "", UtTable::ValueGE(0.0), UtInput::cFORCE, "", UtTable::ValueGE(0.0));
      if (mThrustReference == cVACUUM)
      {
         mThrustReference = cPAIRED;
      }
      else
      {
         mThrustReference = cSEA_LEVEL;
      }
      mThrust         = 0.0;
      mThrustVacuum   = 0.0;
      mThrustSeaLevel = 0.0;
   }
   else if (command == "nozzle_exit_area")
   {
      aInput.ReadValueOfType(mNozzleExitArea, UtInput::cAREA);
      aInput.ValueGreaterOrEqual(mNozzleExitArea, 0.0); // zero makes it as though wasn't supplied
   }
   else if (command == "specific_impulse")
   {
      aInput.ReadValueOfType(mSpecificImpulse, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mSpecificImpulse, 0.0); // zero makes it as though wasn't supplied
      mSpecificImpulseVacuum   = 0.0;
      mSpecificImpulseSeaLevel = 0.0;
   }
   else if (command == "vacuum_specific_impulse")
   {
      aInput.ReadValueOfType(mSpecificImpulse, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mSpecificImpulse, 0.0); // zero makes it as though wasn't supplied
      mSpecificImpulseVacuum = mSpecificImpulse;
      if (mImpulseReference == cSEA_LEVEL)
      {
         mImpulseReference = cPAIRED;
         mSpecificImpulse  = mSpecificImpulseVacuum;
      }
      else
      {
         mImpulseReference = cVACUUM;
      }
   }
   else if (command == "sea_level_specific_impulse")
   {
      aInput.ReadValueOfType(mSpecificImpulse, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mSpecificImpulse, 0.0); // zero makes it as though wasn't supplied
      mSpecificImpulseSeaLevel = mSpecificImpulse;
      if (mImpulseReference == cVACUUM)
      {
         mImpulseReference = cPAIRED;
         mSpecificImpulse  = mSpecificImpulseVacuum;
      }
      else
      {
         mImpulseReference = cSEA_LEVEL;
      }
   }
   else if (command == "burn_rate")
   {
      aInput.ReadValueOfType(mBurnRate, UtInput::cMASS_TRANSFER);
      aInput.ValueGreaterOrEqual(mBurnRate, 0.0); // zero makes it as though wasn't supplied
   }
   else if (command == "burn_rate_table")
   {
      mBurnRateTablePtr = UtTable::LoadCurve(aInput,
                                             "time",
                                             UtInput::cTIME,
                                             "",
                                             UtTable::ValueGE(0.0),
                                             UtInput::cMASS_TRANSFER,
                                             "",
                                             UtTable::ValueGE(0.0));
   }
   else if ((command == "throttle") || (command == "throttle_table"))
   {
      mThrottlePtr =
         UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), sGuidedMoverParameters);
   }
   else if ((command == "thrust_duration") || (command == "burn_time"))
   {
      aInput.ReadValueOfType(mThrustDuration, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mThrustDuration, 0.0); // zero makes it as though wasn't supplied
   }
   else if ((command == "pre_ignition_coast_time") || (command == "pre_burn_coast_time") ||
            (command == "thrust_delay")) // for backward compatibility
   {
      aInput.ReadValueOfType(mPreIgnitionCoastTime, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mPreIgnitionCoastTime, 0.0); // zero makes it as though wasn't supplied
   }
   else if ((command == "pre_separation_coast_time") || (command == "post_burn_coast_time") ||
            (command == "staging_delay")) // for backward compatibility
   {
      aInput.ReadValueOfType(mPreSeparationCoastTime, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mPreSeparationCoastTime, 0.0); // zero makes it as though wasn't supplied
   }
   else if ((command == "thrust_vectoring_angle_limit") || (command == "max_tvc_angle")) // for backward compatibility
   {
      aInput.ReadValueOfType(mMaxTVC_Angle, UtInput::cANGLE);
      aInput.ValueInClosedRange(mMaxTVC_Angle, 0.0, UtMath::cPI_OVER_2);
   }
   else if (command == "thrust_vectoring_time_limits")
   {
      aInput.ReadValueOfType(mMinTVC_Time, UtInput::cTIME);
      aInput.ReadValueOfType(mMaxTVC_Time, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mMinTVC_Time, 0.0);
      aInput.ValueGreater(mMaxTVC_Time, mMinTVC_Time);
   }
   else if (command == "max_tvc_time") // For backward compatibility
   {
      aInput.ReadValueOfType(mMaxTVC_Time, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mMaxTVC_Time, 0.0); // zero makes it as though wasn't supplied
      mMinTVC_Time = 0.0;
   }
   else if (command == "lateral_thrust_gain")
   {
      aInput.ReadValue(mLateralThrustGain);
      aInput.ValueGreater(mLateralThrustGain, 0.0);
   }
   else if (command == "engine_type")
   {
      std::string engineType;
      aInput.ReadValue(engineType);
      if (engineType == "rocket")
      {
         mEngineType = cROCKET;
      }
      else if ((engineType == "turbojet") || (engineType == "turbofan") || (engineType == "ramjet"))
      {
         mEngineType = cJET;
      }
      else if (engineType == "none")
      {
         mEngineType = cNONE;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Invalid value for 'engine_type': " + engineType);
      }
   }
   else if (command == "integration_timestep")
   {
      aInput.ReadValueOfType(mIntegrationTimestep, UtInput::cTIME);
      aInput.ValueGreater(mIntegrationTimestep, 0.0);
   }
   else if (command == "time_constant") // NO_DOC | IN_DEVELOPMENT
   {
      mTimeConstantPtr = UtTable::LoadInstance(aInput, UtInput::cTIME, UtTable::ValueGT(0.0), sGuidedMoverParameters);
   }
   else if (command == "separation_failure_probability")
   {
      aInput.ReadValue(mProbabilityOfSeparation);
      aInput.ValueInClosedRange(mProbabilityOfSeparation, 0.0, 1.0);
      // The probability of successful separation is 1 - the probability of separation failure.
      mProbabilityOfSeparation = 1.0 - mProbabilityOfSeparation;
   }
   else if (command == "ignition_failure_probability")
   {
      aInput.ReadValue(mProbabilityOfIgnition);
      aInput.ValueInClosedRange(mProbabilityOfIgnition, 0.0, 1.0);
      // The probability of successful ignition is 1 - the probability of ignition failure.
      mProbabilityOfIgnition = 1.0 - mProbabilityOfIgnition;
   }
   else if ((command == "angle_of_attack") || (command == "angle_of_attack_table"))
   {
      mAngleOfAttackPtr = UtTable::LoadInstance(aInput,
                                                UtInput::cANGLE,
                                                UtTable::ValueGT_LT(-UtMath::cPI_OVER_2, UtMath::cPI_OVER_2),
                                                sGuidedMoverParameters);
   }
   else if (command == "skid_to_turn")
   {
      mTurnMethod = cSKID_TO_TURN;
   }
   else if (command == "bank_to_turn")
   {
      mTurnMethod = cBANK_TO_TURN;
   }
   else if (command == "reverse_thrust")
   {
      mReverseThrust = true;
   }
   else if (command == "final_boost_stage")
   {
      mFinalBoostStage = true;
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
void WsfGuidedMover::Stage::ShowStatus(WsfGuidedMover* aMoverPtr, ut::log::MessageStream& aMsgStream) const
{
   aMsgStream << std::fixed << std::setprecision(3);
   aMsgStream.AddNote() << "Empty Mass: " << mEmptyMass << " kg (" << mEmptyMass * UtMath::cLB_PER_KG << " lbm)";
   aMsgStream.AddNote() << "Fuel Mass: " << mFuelMass << " kg (" << mFuelMass * UtMath::cLB_PER_KG << " lbm)";
   aMsgStream.AddNote() << "Total Mass: " << mTotalMass << " kg (" << mTotalMass * UtMath::cLB_PER_KG << " lbm)";
   if (mThrustReference == cVACUUM)
   {
      aMsgStream.AddNote() << "Thrust (VAC): " << mThrustVacuum << " N ("
                           << mThrustVacuum * UtMath::cLB_PER_KG / UtEarth::cACCEL_OF_GRAVITY << " lbf) ("
                           << mThrustVacuum / UtEarth::cACCEL_OF_GRAVITY << " kgf)";
      aMsgStream.AddNote() << "Thrust (SL): " << mThrustSeaLevel << " N ("
                           << mThrustSeaLevel * UtMath::cLB_PER_KG / UtEarth::cACCEL_OF_GRAVITY << " lbf) ("
                           << mThrustSeaLevel / UtEarth::cACCEL_OF_GRAVITY << " kgf) (derived)";
   }
   else if (mThrustReference == cSEA_LEVEL)
   {
      aMsgStream.AddNote() << "Thrust (VAC): " << mThrustVacuum << " N ("
                           << mThrustVacuum * UtMath::cLB_PER_KG / UtEarth::cACCEL_OF_GRAVITY << " lbf) ("
                           << mThrustVacuum / UtEarth::cACCEL_OF_GRAVITY << " kgf) (derived)";
      aMsgStream.AddNote() << "Thrust (SL): " << mThrustSeaLevel << " N ("
                           << mThrustSeaLevel * UtMath::cLB_PER_KG / UtEarth::cACCEL_OF_GRAVITY << " lbf) ("
                           << mThrustSeaLevel / UtEarth::cACCEL_OF_GRAVITY << " kgf)";
   }
   else if (mThrustReference == cPAIRED)
   {
      aMsgStream.AddNote() << "Thrust (VAC): " << mThrustVacuum << " N ("
                           << mThrustVacuum * UtMath::cLB_PER_KG / UtEarth::cACCEL_OF_GRAVITY << " lbf) ("
                           << mThrustVacuum / UtEarth::cACCEL_OF_GRAVITY << " kgf)";
      aMsgStream.AddNote() << "Thrust (SL): " << mThrustSeaLevel << " N ("
                           << mThrustSeaLevel * UtMath::cLB_PER_KG / UtEarth::cACCEL_OF_GRAVITY << " lbf) ("
                           << mThrustSeaLevel / UtEarth::cACCEL_OF_GRAVITY << " kgf)";
   }
   else
   {
      aMsgStream.AddNote() << "Thrust: " << mThrust << " N (" << mThrust * UtMath::cLB_PER_KG / UtEarth::cACCEL_OF_GRAVITY
                           << " lbf) (" << mThrust / UtEarth::cACCEL_OF_GRAVITY << " kgf)";
   }
   if (mImpulseReference == cVACUUM)
   {
      aMsgStream.AddNote() << "Specific Impulse (VAC): " << mSpecificImpulseVacuum << " sec";
      aMsgStream.AddNote() << "Specific Impulse (SL) : " << mSpecificImpulseSeaLevel << " sec (derived)";
   }
   else if (mImpulseReference == cSEA_LEVEL)
   {
      aMsgStream.AddNote() << "Specific Impulse (VAC): " << mSpecificImpulseVacuum << " sec (derived)";
      aMsgStream.AddNote() << "Specific Impulse (SL) : " << mSpecificImpulseSeaLevel << " sec";
   }
   else if (mImpulseReference == cPAIRED)
   {
      aMsgStream.AddNote() << "Specific Impulse (VAC): " << mSpecificImpulseVacuum << " sec";
      aMsgStream.AddNote() << "Specific Impulse (SL) : " << mSpecificImpulseSeaLevel << " sec";
   }
   else
   {
      aMsgStream.AddNote() << "Specific Impulse: " << mSpecificImpulse << " sec";
   }
   if (mNozzleExitArea != 0.0)
   {
      aMsgStream.AddNote() << "Nozzle Exit Area: " << mNozzleExitArea << " m2";
   }
   aMsgStream.AddNote() << "Burn Rate: " << mBurnRate << " kg/sec (" << mBurnRate * UtMath::cLB_PER_KG << " lbm/sec)";
   aMsgStream.AddNote() << "Thrust Duration: " << mThrustDuration << " sec";
   double fuelUsed = mThrustDuration * mBurnRate;
   aMsgStream.AddNote() << "Fuel Used: " << fuelUsed << " kg (" << fuelUsed * UtMath::cLB_PER_KG << " lbm)";
}

double WsfGuidedMover::Stage::ComputeSpecificImpulse(double aAmbientPressure, double aSeaLevelPressure) const
{
   double specificImpulse = 0.0;
   double fraction        = 1.0; // Set to unity

   switch (mImpulseReference)
   {
   case Stage::cVACUUM:
      if (mThrust > 0.0)
      {
         fraction        = (mThrust - (mNozzleExitArea * aAmbientPressure)) / mThrust;
         fraction        = std::max(fraction, 0.0);
         specificImpulse = mSpecificImpulseVacuum * fraction;
      }
      else
      {
         specificImpulse = mSpecificImpulseVacuum;
      }
      break;
   case Stage::cSEA_LEVEL:
      if (mThrust > 0.0)
      {
         fraction        = (mThrust + ((aSeaLevelPressure - aAmbientPressure) * mNozzleExitArea)) / mThrust;
         fraction        = std::max(fraction, 0.0);
         specificImpulse = mSpecificImpulseSeaLevel * fraction;
      }
      else
      {
         specificImpulse = mSpecificImpulseSeaLevel;
      }
      break;
   case Stage::cPAIRED:
      specificImpulse = mSpecificImpulseVacuum -
                        (mSpecificImpulseVacuum - mSpecificImpulseSeaLevel) * (aAmbientPressure / aSeaLevelPressure);
      break;
   default:
      specificImpulse = mSpecificImpulse;
      break;
   }

   // Ensure a positive value
   specificImpulse = std::max(specificImpulse, 0.0);

   return specificImpulse;
}

double WsfGuidedMover::Stage::DetermineCurrentThrust(double aAmbientPressure, double aSeaLevelPressure, double aPhaseTime)
{
   // Determine the current thrust, based on thrust reference type
   //
   // NOTE: if a thrust value is gotten via table lookup, its value is pushed back into the respective scalar value back
   //       in the stage definition for use by ComputeSpecificImpulse.
   double thrust{0.0};
   switch (mThrustReference)
   {
   case Stage::cVACUUM:
   {
      if (mVacuumThrustTablePtr != nullptr)
      {
         mThrustVacuum = mVacuumThrustTablePtr->Lookup(aPhaseTime);
      }
      mThrust = mThrustVacuum;
      thrust  = mThrustVacuum - (mNozzleExitArea * aAmbientPressure);
      thrust  = std::max(thrust, 0.0);
   }
   break;
   case Stage::cSEA_LEVEL:
   {
      if (mSeaLevelThrustTablePtr != nullptr)
      {
         mThrustSeaLevel = mSeaLevelThrustTablePtr->Lookup(aPhaseTime);
      }
      mThrust = mThrustSeaLevel;
      thrust  = mThrustSeaLevel + ((aSeaLevelPressure - aAmbientPressure) * mNozzleExitArea);
      thrust  = std::max(thrust, 0.0);
   }
   break;
   case Stage::cPAIRED:
   {
      if (mVacuumThrustTablePtr != nullptr)
      {
         mThrustVacuum = mVacuumThrustTablePtr->Lookup(aPhaseTime);
      }
      if (mSeaLevelThrustTablePtr != nullptr)
      {
         mThrustSeaLevel = mSeaLevelThrustTablePtr->Lookup(aPhaseTime);
      }
      thrust = mThrustVacuum - (mThrustVacuum - mThrustSeaLevel) * (aAmbientPressure / aSeaLevelPressure);
      thrust = std::max(thrust, 0.0);
   }
   break;
   default: // Stage::cDEFAULT
   {
      if (mThrustTablePtr != nullptr)
      {
         mThrust = mThrustTablePtr->Lookup(aPhaseTime);
      }
      thrust = mThrust;
   }
   break;
   }
   return thrust;
}

// =================================================================================================
// Nested class State
// =================================================================================================

// =================================================================================================
WsfGuidedMover::State::State()
   : mTime(0.0)
   , mPsi(0.0)
   , mTheta(0.0)
   , mPhi(0.0)
   , mAltitude(0.0)
   , mSpeed(0.0)
   , mMach(0.0)
   , mMass(0.0)
   , mCurrentThrust(0.0)
   , mCurrentFuelFlowRate(0.0)
   , mThrustDurationExtension(0.0)
   , mDivertFuelMass(0.0)
   , mCurrentFuelMass(0.0)
{
   UtMat3d::Set(mWorld_ToECS_Transform, 0.0);
   UtVec3d::Set(mLocation, 0.0);
   UtVec3d::Set(mVelocity, 0.0);
   UtVec3d::Set(mAcceleration, 0.0);
   UtVec3d::Set(mAppliedForcesECS, 0.0);
   UtVec3d::Set(mLastThrustForcesECS, 0.0);
   UtVec3d::Set(mLastAeroForcesECS, 0.0);
}

void WsfGuidedMover::State::Print(ut::log::MessageStream& aMsgStream, double aFMM_Time)
{
   double lat;
   double lon;
   double alt;
   double WCS_ToNED_Transform[3][3];
   UtSphericalEarth::ConvertECEFToLLA(mLocation, lat, lon, alt);
   UtSphericalEarth::ComputeNEDTransform(lat, lon, alt, WCS_ToNED_Transform);

   double NED_ToWCS_Transform[3][3];
   UtMat3d::Transpose(NED_ToWCS_Transform, WCS_ToNED_Transform);
   double NED_ToECS_Transform[3][3];
   UtMat3d::Multiply(NED_ToECS_Transform, mWorld_ToECS_Transform, NED_ToWCS_Transform);
   double psiNED, thetaNED, phiNED;
   UtEntity::ExtractEulerAngles(NED_ToECS_Transform, psiNED, thetaNED, phiNED);

   aMsgStream << std::fixed;
   aMsgStream.AddNote() << "T = " << std::setprecision(10) << mTime << ", " << mTime - aFMM_Time;
   aMsgStream.AddNote() << std::setprecision(15) << "LocWCS: " << UtVec3d(mLocation) << " m";
   aMsgStream.AddNote() << "Lat: " << lat;
   aMsgStream.AddNote() << "Lon: " << lat;
   aMsgStream.AddNote() << "Alt: " << alt << " m";
   aMsgStream.AddNote() << "VelWCS: " << std::setprecision(15) << UtVec3d(mVelocity) << " m/s";
   aMsgStream.AddNote() << "Vel: " << UtVec3d::Magnitude(mVelocity) << " m/s";
   aMsgStream.AddNote() << "AclWCS: " << std::setprecision(15) << UtVec3d(mAcceleration) << " m/s^2";
   aMsgStream.AddNote() << "Acl: " << UtVec3d::Magnitude(mAcceleration) << " m/s^2";
   { // RAII block
      auto wcsecs = aMsgStream.AddNote() << "" << std::setprecision(15);
      wcsecs.AddNote() << "X: " << std::setprecision(15) << UtVec3d(mWorld_ToECS_Transform[0]);
      wcsecs.AddNote() << "Y: " << std::setprecision(15) << UtVec3d(mWorld_ToECS_Transform[1]);
      wcsecs.AddNote() << "Z: " << std::setprecision(15) << UtVec3d(mWorld_ToECS_Transform[2]);
   }
   aMsgStream.AddNote() << "WCS-Psi: " << std::setprecision(15) << mPsi * UtMath::cDEG_PER_RAD << " deg";
   aMsgStream.AddNote() << "Theta: " << std::setprecision(15) << mTheta * UtMath::cDEG_PER_RAD << " deg";
   aMsgStream.AddNote() << "Phi: " << std::setprecision(15) << mPhi * UtMath::cDEG_PER_RAD << " deg";
   { // RAII block
      auto nedecs = aMsgStream.AddNote() << "NED->ECS:" << std::setprecision(15);
      nedecs.AddNote() << "X: " << UtVec3d(NED_ToECS_Transform[0]);
      nedecs.AddNote() << "Y: " << UtVec3d(NED_ToECS_Transform[1]);
      nedecs.AddNote() << "Z: " << UtVec3d(NED_ToECS_Transform[2]);
   }
   aMsgStream.AddNote() << "NED-Psi: " << std::setprecision(15) << psiNED * UtMath::cDEG_PER_RAD << " deg";
   aMsgStream.AddNote() << "Theta: " << std::setprecision(15) << thetaNED * UtMath::cDEG_PER_RAD << " deg";
   aMsgStream.AddNote() << "Phi: " << std::setprecision(15) << phiNED * UtMath::cDEG_PER_RAD << " deg";
}

WsfGuidedMover::State WsfGuidedMover::State::Advance(double aDeltaT, const KinematicState& aKinematicState) const
{
   State retval = *this;

   // Use the full step delta t to work out if a coast phase is needed.
   double poweredDeltaT = aDeltaT;
   if (mMoverPtr->mPhase == cPHASE_BURN)
   {
      double fuelUsed = mCurrentFuelFlowRate * aDeltaT;
      if (fuelUsed > mCurrentFuelMass && mCurrentFuelFlowRate > 0.0)
      {
         // The fuel will be expended during this timestep, so limit the timestep to only expend what is left.
         poweredDeltaT = mCurrentFuelMass / mCurrentFuelFlowRate;
      }
   }

   double coastDeltaT = aDeltaT - poweredDeltaT;

   // Do powered portion of the timestep
   retval.TakeEulerStep(poweredDeltaT, aKinematicState, *this, true);

   // Do unpowered portion of timestep (if needed)
   if (mMoverPtr->mPhase == cPHASE_BURN && coastDeltaT > 0.0)
   {
      retval.TakeEulerStep(coastDeltaT, GetUnpoweredDerivatives(), *this, true);
   }

   return retval;
}

WsfGuidedMover::KinematicState WsfGuidedMover::State::GetDerivatives() const
{
   return KinematicState{mVelocity, mAcceleration, mCurrentFuelFlowRate};
}

WsfGuidedMover::KinematicState WsfGuidedMover::State::GetUnpoweredDerivatives() const
{
   double appForce[3];
   UtVec3d::Subtract(appForce, mAppliedForcesECS, mLastThrustForcesECS);
   auto acceleration = ComputeDerivatives(mAppliedForcesECS);
   return KinematicState{mVelocity, acceleration, 0.0};
}

UtVec3d WsfGuidedMover::State::ComputeDerivatives(const double aAppliedForcesECS[3]) const
{
   // Convert the ECS forces to WCS
   double forceVecWCS[3];
   UtMat3d::InverseTransform(forceVecWCS, mWorld_ToECS_Transform, aAppliedForcesECS);

   // Compute the acceleration due to all forces but gravity.
   double newAclWCS[3];
   UtVec3d::Multiply(newAclWCS, forceVecWCS, 1.0 / mMass);

   // Add in the acceleration due to gravity.
   double gravVecWCS[3];
   ComputeGravitationalForce(gravVecWCS);

   UtVec3d retval;
   UtVec3d::Add(retval.GetData(), newAclWCS, gravVecWCS);

   return retval;
}
