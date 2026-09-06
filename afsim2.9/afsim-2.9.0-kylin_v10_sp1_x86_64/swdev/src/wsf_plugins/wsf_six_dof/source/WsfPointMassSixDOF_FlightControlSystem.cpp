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

#include "WsfPointMassSixDOF_FlightControlSystem.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "WsfPointMassSixDOF_ControlActuator.hpp"
#include "WsfPointMassSixDOF_PilotManager.hpp"
#include "WsfPointMassSixDOF_PilotObject.hpp"
#include "WsfPointMassSixDOF_VehicleData.hpp"
#include "WsfSixDOF_TypeManager.hpp"

wsf::six_dof::PointMassFlightControlSystem::PointMassFlightControlSystem(const PointMassFlightControlSystem& aSrc)
   : Object(aSrc)
   , mLastUpdateSimTime_nanosec(aSrc.mLastUpdateSimTime_nanosec)
   , mStickBackCurvePtr(aSrc.mStickBackCurvePtr)
   , mStickRightCurvePtr(aSrc.mStickRightCurvePtr)
   , mRudderRightCurvePtr(aSrc.mRudderRightCurvePtr)
   , mStickBack(aSrc.mStickBack)
   , mStickRight(aSrc.mStickRight)
   , mRudderRight(aSrc.mRudderRight)
   , mThrottleMil(aSrc.mThrottleMil)
   , mThrottleAb(aSrc.mThrottleAb)
   , mFlaps(aSrc.mFlaps, this)
   , mSpeedbrakes(aSrc.mSpeedbrakes, this)
   , mSpoilers(aSrc.mSpoilers, this)
{
}

double wsf::six_dof::PointMassFlightControlSystem::FindControlPositionForRotationCommand(double aOmegaCommand_rps,
                                                                                         UtTable::Curve aControlCurve)
{
   // Bisection search method
   constexpr double cEPSILON  = 0.01 * UtMath::cRAD_PER_DEG;
   constexpr size_t cMAX_ITER = 20;

   double a     = -1.0;
   double b     = 1.0;
   double guess = 0.0;

   double outputLowerBound = aControlCurve.Lookup(a);
   double outputUpperBound = aControlCurve.Lookup(b);

   double omegaCommand_dps = UtMath::cDEG_PER_RAD * aOmegaCommand_rps;

   if (omegaCommand_dps < outputLowerBound)
   {
      return a;
   }
   if (omegaCommand_dps > outputUpperBound)
   {
      return b;
   }

   double errorLowerBound = omegaCommand_dps - outputLowerBound;
   double errorUpperBound = omegaCommand_dps - outputUpperBound;

   for (size_t i = 0; i < cMAX_ITER; i++)
   {
      // guess = 0.5 * (a + b); // Bisection method
      guess = (a * errorUpperBound - b * errorLowerBound) / (errorUpperBound - errorLowerBound); // False position method

      double outputGuess = aControlCurve.Lookup(guess);

      double errorGuess = omegaCommand_dps - outputGuess;
      if (std::abs(errorGuess) < cEPSILON)
      {
         break;
      }

      if (UtMath::Sign(errorGuess) == UtMath::Sign(errorLowerBound))
      {
         a               = guess;
         errorLowerBound = errorGuess;
      }
      else
      {
         b               = guess;
         errorUpperBound = errorGuess;
      }

      if (UtMath::NearlyEqual(a, b))
      {
         break;
      }
   }

   return guess;
}

wsf::six_dof::PointMassFlightControlSystem* wsf::six_dof::PointMassFlightControlSystem::Clone() const
{
   return new PointMassFlightControlSystem(*this);
}

bool wsf::six_dof::PointMassFlightControlSystem::ProcessInput(UtInput& aInput)
{
   const std::string& command = aInput.GetCommand();

   if (command == "flight_controls")
   {
      UtInputBlock block(aInput, "end_flight_controls");
      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "stick_back")
         {
            mStickBackCurvePtr = ut::make_unique<UtTable::Curve>();
            mStickBackCurvePtr->ProcessInput(aInput,
                                             UtInput::cNON_DIMENSIONAL,
                                             "stick_input",
                                             UtTable::NoCheck(),
                                             UtInput::cNON_DIMENSIONAL,
                                             "rate_command",
                                             UtTable::NoCheck());
         }
         else if (localCommand == "stick_right")
         {
            mStickRightCurvePtr = ut::make_unique<UtTable::Curve>();
            mStickRightCurvePtr->ProcessInput(aInput,
                                              UtInput::cNON_DIMENSIONAL,
                                              "stick_input",
                                              UtTable::NoCheck(),
                                              UtInput::cNON_DIMENSIONAL,
                                              "rate_command",
                                              UtTable::NoCheck());
         }
         else if (localCommand == "rudder_right")
         {
            mRudderRightCurvePtr = ut::make_unique<UtTable::Curve>();
            mRudderRightCurvePtr->ProcessInput(aInput,
                                               UtInput::cNON_DIMENSIONAL,
                                               "stick_input",
                                               UtTable::NoCheck(),
                                               UtInput::cNON_DIMENSIONAL,
                                               "rate_command",
                                               UtTable::NoCheck());
         }
         else if (localCommand == "speedbrakes")
         {
            UtInputBlock commandBlock(aInput, "end_speedbrakes");
            mSpeedbrakes.ProcessInput(commandBlock);
         }
         else if (localCommand == "flaps")
         {
            UtInputBlock commandBlock(aInput, "end_flaps");
            mFlaps.ProcessInput(commandBlock);
         }
         else if (localCommand == "spoilers")
         {
            UtInputBlock commandBlock(aInput, "end_spoilers");
            mSpoilers.ProcessInput(commandBlock);
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within PointMassFlightControlSystem::ProcessInput().";
            out.AddNote() << "Command: " << localCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   return true;
}

bool wsf::six_dof::PointMassFlightControlSystem::Initialize(int64_t aSimTime_nanosec)
{
   mLastUpdateSimTime_nanosec = aSimTime_nanosec;

   return true;
}

void wsf::six_dof::PointMassFlightControlSystem::Update(int64_t aSimTime_nanosec)
{
   // Check for the "testing" condition (used for feed-forward calculations in the autopilot)
   bool testing = GetParentVehicle()->GetFreezeFlags()->testingNoLag;

   if (aSimTime_nanosec == mLastUpdateSimTime_nanosec)
   {
      if (!testing)
      {
         return;
      }
   }

   // Get the active pilot
   if (mPointMassSixDOF_PilotManager != nullptr)
   {
      mPointMassSixDOF_PilotObjectPtr = mPointMassSixDOF_PilotManager->GetActivePointMassPilot();
   }

   if (mPointMassSixDOF_PilotObjectPtr)
   {
      // First, update the controls provider
      mPointMassSixDOF_PilotObjectPtr->Update(aSimTime_nanosec);

      mSpeedbrakes.Update(aSimTime_nanosec, mPointMassSixDOF_PilotObjectPtr->GetSpeedBrakesControllerPosition());
      mFlaps.Update(aSimTime_nanosec, mPointMassSixDOF_PilotObjectPtr->GetFlapsControllerPosition());
      mSpoilers.Update(aSimTime_nanosec, mPointMassSixDOF_PilotObjectPtr->GetSpoilersControllerPosition());

      mStickBack   = mPointMassSixDOF_PilotObjectPtr->GetStickBackControllerPosition();
      mStickRight  = mPointMassSixDOF_PilotObjectPtr->GetStickRightControllerPosition();
      mRudderRight = mPointMassSixDOF_PilotObjectPtr->GetRudderRightControllerPosition();

      mThrottleMil = mPointMassSixDOF_PilotObjectPtr->GetThrottleMilitaryControllerPosition();
      mThrottleAb  = mPointMassSixDOF_PilotObjectPtr->GetThrottleAfterburnerControllerPosition();
   }

   if (!testing)
   {
      // Set the last time
      mLastUpdateSimTime_nanosec = aSimTime_nanosec;
   }
}

void wsf::six_dof::PointMassFlightControlSystem::SetPilotObject(PointMassPilotObject* aPointMassSixDOF_PilotObject)
{
   mPointMassSixDOF_PilotObjectPtr = aPointMassSixDOF_PilotObject;
}

void wsf::six_dof::PointMassFlightControlSystem::SetPilotManager(PointMassPilotManager* aPointMassSixDOF_PilotManager)
{
   mPointMassSixDOF_PilotManager = aPointMassSixDOF_PilotManager;
}
