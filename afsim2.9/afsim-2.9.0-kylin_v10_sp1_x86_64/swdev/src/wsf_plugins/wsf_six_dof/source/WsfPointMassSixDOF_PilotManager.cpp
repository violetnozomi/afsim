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

#include "WsfPointMassSixDOF_PilotManager.hpp"

#include <cstring>

#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtRandom.hpp"
#include "WsfPointMassSixDOF_Mover.hpp"

wsf::six_dof::PointMassPilotManager::PointMassPilotManager(const PointMassPilotManager& aSrc)
   : PilotManager(aSrc)
   , mManualPilotSimpleControls(aSrc.mManualPilotSimpleControls)
   , mManualPilotAugmentedControls(aSrc.mManualPilotAugmentedControls)
   , mSyntheticPilot(aSrc.mSyntheticPilot)
   , mSpecialCommonController(aSrc.mSpecialCommonController)
   , mIsDestroyed(aSrc.mIsDestroyed)
{
   if (aSrc.mActivePilot == aSrc.mManualPilotSimpleControls)
   {
      mActivePilot = mManualPilotSimpleControls;
   }
   else if (aSrc.mActivePilot == aSrc.mManualPilotAugmentedControls)
   {
      mActivePilot = mManualPilotAugmentedControls;
   }
   else if (aSrc.mActivePilot == aSrc.mSyntheticPilot)
   {
      mActivePilot = mSyntheticPilot;
   }
}

wsf::six_dof::PointMassPilotManager* wsf::six_dof::PointMassPilotManager::Clone() const
{
   return new PointMassPilotManager(*this);
}

bool wsf::six_dof::PointMassPilotManager::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "pilot_manager")
   {
      UtInputBlock block(aInput, "end_pilot_manager");

      // We will use a temporary pilot object to read the control_inputs block
      auto tempPilotObjectPtr = ut::make_unique<PointMassPilotObject>();
      tempPilotObjectPtr->ProcessInput(aInput);

      // This is used to store the filename for the common autopilot support file
      std::string tempCommonAutopilotSupportFile;

      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "manual_pilot_simple_controls")
         {
            // We need the temp pilot to clone from, so we cannot proceed without one
            if (mManualPilotSimpleControls)
            {
               {
                  // RAII block
                  auto out =
                     ut::log::error()
                     << "Multiple definitions of simple manual pilots in PointMassPilotManager::ProcessInput().";
                  out.AddNote() << "Location: " << aInput.GetLocation();
               }
               throw UtInput::UnknownCommand(aInput);
            }
            else
            {
               // Clone from the temp pilot and then process input
               mManualPilotSimpleControls = new PointMassManualPilotSimpleControls(tempPilotObjectPtr.get());
               mManualPilotSimpleControls->ProcessInput(aInput);
            }
         }
         else if (localCommand == "manual_pilot_augmented_controls" ||
                  localCommand == "manual_pilot_augmented_stability")
         {
            if (mManualPilotAugmentedControls)
            {
               {
                  // RAII block
                  auto out =
                     ut::log::error()
                     << "Multiple definitions of augmented manual pilots in PointMassPilotManager::ProcessInput().";
                  out.AddNote() << "Location: " << aInput.GetLocation();
               }
               throw UtInput::UnknownCommand(aInput);
            }
            else
            {
               // Clone from the temp pilot and then process input
               mManualPilotAugmentedControls = new PointMassManualPilotAugmentedControls(tempPilotObjectPtr.get());
               mManualPilotAugmentedControls->ProcessInput(aInput);
            }
         }
         else if (localCommand == "synthetic_pilot")
         {
            if (mSyntheticPilot)
            {
               {
                  // RAII block
                  auto out = ut::log::error()
                             << "Multiple definitions of synthetic pilots in PointMassPilotManager::ProcessInput().";
                  out.AddNote() << "Location: " << aInput.GetLocation();
               }
               throw UtInput::UnknownCommand(aInput);
            }
            else
            {
               // Clone from the temp pilot and then process input
               mSyntheticPilot = new PointMassSyntheticPilot(tempPilotObjectPtr.get());
               mSyntheticPilot->ProcessInput(aInput);
            }
         }
         else if (localCommand == "active_pilot")
         {
            std::string name;
            aInput.ReadValue(name);

            if (name == "manual_pilot_simple_controls")
            {
               if (mManualPilotSimpleControls)
               {
                  mActivePilot = mManualPilotSimpleControls;
               }
               else
               {
                  {
                     // RAII block
                     auto out = ut::log::error() << "Invalid 'active_pilot'.";
                     out.AddNote() << "No manual pilot has been defined yet in PointMassPilotManager::ProcessInput().";
                     out.AddNote() << "Requested: " << name;
                     out.AddNote() << "Location: " << aInput.GetLocation();
                  }
                  throw UtInput::UnknownCommand(aInput);
               }
            }
            else if (name == "manual_pilot_augmented_controls")
            {
               if (mManualPilotAugmentedControls != nullptr)
               {
                  mActivePilot = mManualPilotAugmentedControls;
               }
               else
               {
                  {
                     // RAII block
                     auto out = ut::log::error() << "Invalid 'active_pilot'.";
                     out.AddNote() << "No manual pilot has been defined yet in PointMassPilotManager::ProcessInput().";
                     out.AddNote() << "Requested: " << name;
                     out.AddNote() << "Location: " << aInput.GetLocation();
                  }
                  throw UtInput::UnknownCommand(aInput);
               }
            }
            else if (name == "synthetic_pilot")
            {
               if (mSyntheticPilot)
               {
                  mActivePilot = mSyntheticPilot;
               }
               else
               {
                  {
                     // RAII block
                     auto out = ut::log::error() << "Invalid 'active_pilot'.";
                     out.AddNote()
                        << "No synthetic pilot has been defined yet in PointMassPilotManager::ProcessInput().";
                     out.AddNote() << "Requested: " << name;
                     out.AddNote() << "Location: " << aInput.GetLocation();
                  }
                  throw UtInput::UnknownCommand(aInput);
               }
            }
            else
            {
               // Unknown active pilot specified
               {
                  // RAII block
                  auto out = ut::log::error() << "Unknown 'active_pilot' in PointMassPilotManager::ProcessInput().";
                  out.AddNote() << "Requested: " << name;
                  out.AddNote() << "Location: " << aInput.GetLocation();
               }
               throw UtInput::UnknownCommand(aInput);
            }
         }
         else if (localCommand == "common_autopilot_support_file")
         {
            std::string filename;
            aInput.ReadValue(filename);
            filename                       = aInput.LocateFile(filename);
            tempCommonAutopilotSupportFile = filename;
         }
         else
         {
            {
               // RAII block
               auto out = ut::log::error() << "Unrecognized command within PointMassPilotManager::ProcessInput().";
               out.AddNote() << "Command: " << localCommand;
               out.AddNote() << "Location: " << aInput.GetLocation();
            }
            throw UtInput::UnknownCommand(aInput);
         }
      }

      // Read the common autopilot support file into the pilots, if present
      if (!tempCommonAutopilotSupportFile.empty())
      {
         if (mManualPilotSimpleControls)
         {
            PointMassCommonController* autopilot = mManualPilotSimpleControls->GetPointMassCommonController();
            if (autopilot != nullptr)
            {
               autopilot->ReadSupportFile(tempCommonAutopilotSupportFile);
            }
         }

         if (mManualPilotAugmentedControls)
         {
            PointMassCommonController* autopilot = mManualPilotAugmentedControls->GetPointMassCommonController();
            if (autopilot != nullptr)
            {
               autopilot->ReadSupportFile(tempCommonAutopilotSupportFile);
            }
         }

         if (mSyntheticPilot)
         {
            PointMassCommonController* autopilot = mSyntheticPilot->GetPointMassCommonController();
            if (autopilot != nullptr)
            {
               autopilot->ReadSupportFile(tempCommonAutopilotSupportFile);
            }
         }

         // We use an special common controller (autopilot) that is always present, regardless of any
         // other "pilots" above. This provides the ability to always access the common autopilot
         // support file functions.
         if (mSpecialCommonController == nullptr)
         {
            mSpecialCommonController = ut::make_unique<PointMassCommonController>();
         }

         mSpecialCommonController->ReadSupportFile(tempCommonAutopilotSupportFile);
      }

      // Check for an active pilot
      if (mActivePilot == nullptr)
      {
         // We lack an active pilot, so if we have any pilot objects, be sure one of them is "active"

         // Try mSyntheticPilot, mHardwareAutopilot, and mManualPilot.
         if (mSyntheticPilot)
         {
            mActivePilot = mSyntheticPilot;
            auto out     = ut::log::warning() << "Active pilot not defined in PointMassPilotManager::ProcessInput().";
            out.AddNote() << "Using the synthetic pilot as the active pilot.";
            out.AddNote() << "Location: " << aInput.GetLocation();
         }
         else if (mManualPilotSimpleControls)
         {
            mActivePilot = mManualPilotSimpleControls;
            auto out     = ut::log::warning() << "Active pilot not defined in PointMassPilotManager::ProcessInput().";
            out.AddNote() << "Using the manual pilot (simple controls) as the active pilot.";
            out.AddNote() << "Location: " << aInput.GetLocation();
         }
         else if (mManualPilotAugmentedControls)
         {
            mActivePilot = mManualPilotAugmentedControls;
            auto out     = ut::log::warning() << "Active pilot not defined in PointMassPilotManager::ProcessInput().";
            out.AddNote() << "Using the manual pilot (augmented controls) as the active pilot.";
            out.AddNote() << "Location: " << aInput.GetLocation();
         }
         else
         {
            // No pilots available. This is acceptable if the vehicle is unguided/un-piloted.
         }
      }
   }

   return myCommand;
}

bool wsf::six_dof::PointMassPilotManager::Initialize(int64_t aSimTime_nanosec)
{
   if (mManualPilotSimpleControls != nullptr)
   {
      if (!mManualPilotSimpleControls->Initialize(aSimTime_nanosec))
      {
         throw UtException("PointMassPilotManager::Initialize(): Initialize failed for manual pilot simple.");
      }
   }

   if (mManualPilotAugmentedControls != nullptr)
   {
      if (!mManualPilotAugmentedControls->Initialize(aSimTime_nanosec))
      {
         throw UtException("PointMassPilotManager::Initialize(): Initialize failed for manual pilot augmented.");
      }
   }

   if (mSyntheticPilot != nullptr)
   {
      if (!mSyntheticPilot->Initialize(aSimTime_nanosec))
      {
         throw UtException("PointMassPilotManager::Initialize(): Initialize failed for synthetic pilot.");
      }
   }

   return true;
}

void wsf::six_dof::PointMassPilotManager::SetLastSimTime(int64_t aLastSimTime_nanosec)
{
   if (mManualPilotSimpleControls != nullptr)
   {
      mManualPilotSimpleControls->SetLastSimTime(aLastSimTime_nanosec);
   }

   if (mManualPilotAugmentedControls != nullptr)
   {
      mManualPilotAugmentedControls->SetLastSimTime(aLastSimTime_nanosec);
   }

   if (mSyntheticPilot != nullptr)
   {
      mSyntheticPilot->SetLastSimTime(aLastSimTime_nanosec);
   }
}

void wsf::six_dof::PointMassPilotManager::SetParentVehicle(PointMassMover* aParentVehicle)
{
   if (mManualPilotSimpleControls != nullptr)
   {
      mManualPilotSimpleControls->SetParentVehicle(aParentVehicle);
   }

   if (mManualPilotAugmentedControls != nullptr)
   {
      mManualPilotAugmentedControls->SetParentVehicle(aParentVehicle);
   }

   if (mSyntheticPilot != nullptr)
   {
      mSyntheticPilot->SetParentVehicle(aParentVehicle);
   }
}

wsf::six_dof::PilotObject* wsf::six_dof::PointMassPilotManager::GetActivePilot()
{
   return mActivePilot;
}

wsf::six_dof::PilotObject* wsf::six_dof::PointMassPilotManager::GetActiveSimpleManualPilot()
{
   if (mActivePilot != nullptr)
   {
      if (mActivePilot == mManualPilotSimpleControls)
      {
         return mActivePilot;
      }
   }

   return (PointMassPilotObject*)nullptr;
}

wsf::six_dof::PilotObject* wsf::six_dof::PointMassPilotManager::GetActiveAugmentedManualPilot()
{
   if (mActivePilot != nullptr)
   {
      if (mActivePilot == mManualPilotAugmentedControls)
      {
         return mActivePilot;
      }
   }

   return (PointMassPilotObject*)nullptr;
}

wsf::six_dof::PilotObject* wsf::six_dof::PointMassPilotManager::GetActiveSyntheticPilot()
{
   if (mActivePilot != nullptr)
   {
      if (mActivePilot == mSyntheticPilot)
      {
         return mActivePilot;
      }
   }

   return (PointMassPilotObject*)nullptr;
}

void wsf::six_dof::PointMassPilotManager::InputAngleDeltasToPilotObjects(const double& aYaw_rad,
                                                                         const double& aPitch_rad,
                                                                         const double& aRoll_rad)
{
   if (mManualPilotSimpleControls != nullptr)
   {
      mManualPilotSimpleControls->InputAngleDeltas(aYaw_rad, aPitch_rad, aRoll_rad);
   }

   if (mManualPilotAugmentedControls != nullptr)
   {
      mManualPilotAugmentedControls->InputAngleDeltas(aYaw_rad, aPitch_rad, aRoll_rad);
   }

   if (mSyntheticPilot != nullptr)
   {
      mSyntheticPilot->InputAngleDeltas(aYaw_rad, aPitch_rad, aRoll_rad);
   }
}

void wsf::six_dof::PointMassPilotManager::EnableControls(bool aEnabled)
{
   if (mManualPilotSimpleControls != nullptr)
   {
      mManualPilotSimpleControls->EnableControlInputs(aEnabled);
   }

   if (mManualPilotAugmentedControls != nullptr)
   {
      mManualPilotAugmentedControls->EnableControlInputs(aEnabled);
   }

   if (mSyntheticPilot != nullptr)
   {
      mSyntheticPilot->EnableControlInputs(aEnabled);
   }
}

bool wsf::six_dof::PointMassPilotManager::AreControlsEnabled() const
{
   if (mActivePilot == nullptr)
   {
      return false;
   }

   return mActivePilot->ControlsAreEnabled();
}

bool wsf::six_dof::PointMassPilotManager::MakeSimpleManualPilotActive()
{
   if (mIsDestroyed)
   {
      return false;
   }

   if (mManualPilotSimpleControls != nullptr)
   {
      mActivePilot = mManualPilotSimpleControls;
      return true;
   }

   return false;
}

bool wsf::six_dof::PointMassPilotManager::MakeAugmentedManualPilotActive()
{
   if (mIsDestroyed)
   {
      return false;
   }

   if (mManualPilotAugmentedControls != nullptr)
   {
      mActivePilot = mManualPilotAugmentedControls;
      return true;
   }

   return false;
}

bool wsf::six_dof::PointMassPilotManager::MakeSyntheticPilotActive()
{
   if (mIsDestroyed)
   {
      return false;
   }

   if (mSyntheticPilot != nullptr)
   {
      mActivePilot = mSyntheticPilot;
      return true;
   }

   return false;
}

void wsf::six_dof::PointMassPilotManager::SetDestroyed(double aPitchInputModifier, double aRollInputModifier)
{
   if (mActivePilot == nullptr)
   {
      mIsDestroyed = true;
      return;
   }

   // Take control of the vehicle
   mActivePilot->TakeExternalDirectControl();

   // Disable the autopilot, if it is enabled
   if (mActivePilot->AutopilotIsEnabled())
   {
      mActivePilot->EnableAutopilot(false);
   }

   // Note: The pitch and roll input modifiers are randomly
   // assigned (outside of PointMassPilotManager) and vary between +/-1.0

   // Excessive pitch control can result in "looping", which is
   // unexpected when a vehicle is destroyed. As a result, we limit
   // the pitch control.

   // Use a minimal pitch change 80% of the time. Since aPitchInputModifier
   // varies between +/-1, we can use its value as the 80% check.
   if (fabs(aPitchInputModifier) < 0.8)
   {
      // Limit to a maximum pitch control of only 2% of maximum
      aPitchInputModifier *= 0.02;
   }
   else
   {
      // Limit to a maximum pitch control of 5% of maximum
      aPitchInputModifier *= 0.05;
   }

   // For roll, we want to see some roll to help notice that the vehicle
   // is shot down. So, we'll use 30%-60% roll control for 80% of the
   // cases. So, begin by testing for 80%:
   if (fabs(aRollInputModifier) < 0.8)
   {
      // For these cases, we want a control of 0.3 to 0.8:
      if (aRollInputModifier >= 0.0)
      {
         aRollInputModifier = 0.3 + 0.3 * (aRollInputModifier / 0.8);
      }
      else
      {
         aRollInputModifier = -0.3 + 0.3 * (aRollInputModifier / 0.8);
      }
   }
   else
   {
      // For these cases, which have an unmodified setting of 0.8 to 1.0,
      // we will reduce to a maximum of 10%:
      aRollInputModifier *= 0.1;
   }

   // Get the current stick position
   double stickRightPosition = mActivePilot->GetStickRightControllerPosition();

   // Adjust the stick position
   stickRightPosition += aRollInputModifier;

   // Ensure value is between -1 and +1
   if (stickRightPosition > 1.0)
   {
      stickRightPosition = 1.0;
   }
   if (stickRightPosition < -1.0)
   {
      stickRightPosition = -1.0;
   }

   // Get the current stick position
   double stickAftPosition = mActivePilot->GetStickBackControllerPosition();

   // Adjust the stick position
   stickAftPosition += aPitchInputModifier;

   // Ensure value is between -1 and +1
   if (stickAftPosition > 1.0)
   {
      stickAftPosition = 1.0;
   }
   if (stickAftPosition < -1.0)
   {
      stickAftPosition = -1.0;
   }

   // Kill the engines completely
   mActivePilot->GetParentVehicle()->ShutdownObject();

   // Special: If we have a minimal roll command (less than 10%), we will use a
   // minimal stick forward command (10%) to slowly pitch over...
   if (fabs(stickRightPosition) < 0.1)
   {
      stickAftPosition = -0.1;
   }

   // Set the controls, pulling thew power to idle and extending the speed brakes
   mActivePilot->SetExternalDirectControlData(stickRightPosition, stickAftPosition, 0.0, 1.0, 0.0, 0.0);

   // Now, set various destroyed flags so no further commands can be issued

   mIsDestroyed = true;

   // Inform the active pilot of the destroyed condition
   mActivePilot->SetDestroyed();

   // Inform all other pilot objects of the destroyed condition
   if (mManualPilotSimpleControls)
   {
      mManualPilotSimpleControls->SetDestroyed();
   }
   if (mManualPilotAugmentedControls)
   {
      mManualPilotAugmentedControls->SetDestroyed();
   }
   if (mSyntheticPilot)
   {
      mSyntheticPilot->SetDestroyed();
   }
}
