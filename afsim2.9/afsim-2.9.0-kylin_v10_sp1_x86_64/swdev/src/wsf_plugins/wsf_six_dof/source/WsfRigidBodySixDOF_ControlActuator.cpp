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

#include "WsfRigidBodySixDOF_ControlActuator.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfRigidBodySixDOF_FlightControlSystem.hpp"
#include "WsfRigidBodySixDOF_Mover.hpp"
#include "WsfRigidBodySixDOF_Utils.hpp"

wsf::six_dof::RigidBodyControlActuator::RigidBodyControlActuator(RigidBodyFlightControlSystem* aFlightControlsParent)
   : mFlightControlsParent(aFlightControlsParent)
{
}

wsf::six_dof::RigidBodyControlActuator::RigidBodyControlActuator(const RigidBodyControlActuator& aSrc,
                                                                 RigidBodyFlightControlSystem*   aFlightControlsParent)
   : mFlightControlsParent(aFlightControlsParent)
   , mMaxPositiveRate_dps(aSrc.mMaxPositiveRate_dps)
   , mMaxNegativeRate_dps(aSrc.mMaxNegativeRate_dps)
   , mMaxAngle_deg(aSrc.mMaxAngle_deg)
   , mMinAngle_deg(aSrc.mMinAngle_deg)
   , mCurrentAngle_deg(aSrc.mCurrentAngle_deg)
   , mCommandedAngle_deg(aSrc.mCommandedAngle_deg)
   , mLastSimTime_nanosec(aSrc.mLastSimTime_nanosec)
   , mLagTimeConstant_sec(aSrc.mLagTimeConstant_sec)
{
}

wsf::six_dof::RigidBodyControlActuator*
wsf::six_dof::RigidBodyControlActuator::Clone(RigidBodyFlightControlSystem* aFlightControlsParent) const
{
   return new RigidBodyControlActuator(*this, aFlightControlsParent);
}

bool wsf::six_dof::RigidBodyControlActuator::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   bool maxAngleValid = false;
   bool minAngleValid = false;

   if (command == "actuator")
   {
      UtInputBlock block(aInput, "end_actuator");
      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "max_positive_rate")
         {
            double value = 0.0;
            aInput.ReadValueOfType(value, UtInput::cANGULAR_RATE);
            value *= UtMath::cDEG_PER_RAD;

            mMaxPositiveRate_dps = value;
         }
         else if (localCommand == "max_negative_rate")
         {
            double value = 0.0;
            aInput.ReadValueOfType(value, UtInput::cANGULAR_RATE);
            value *= UtMath::cDEG_PER_RAD;

            mMaxNegativeRate_dps = value;
         }
         else if (localCommand == "max_angle")
         {
            double value = 0.0;
            aInput.ReadValueOfType(value, UtInput::cANGLE);
            value *= UtMath::cDEG_PER_RAD;
            mMaxAngle_deg = value;
            maxAngleValid = true;
         }
         else if (localCommand == "min_angle")
         {
            double value = 0.0;
            aInput.ReadValueOfType(value, UtInput::cANGLE);
            value *= UtMath::cDEG_PER_RAD;
            mMinAngle_deg = value;
            minAngleValid = true;
         }
         else if (localCommand == "current_angle")
         {
            double value = 0.0;
            aInput.ReadValueOfType(value, UtInput::cANGLE);
            value *= UtMath::cDEG_PER_RAD;
            mCurrentAngle_deg = value;
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within RigidBodyControlActuator::ProcessInput().";
            out.AddNote() << "Command: " << localCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   if (!minAngleValid)
   {
      ut::log::error() << "Actuator lacks a minAngle within RigidBodyControlActuator::ProcessInput().";
      throw UtInput::UnknownCommand(aInput);
   }
   if (!maxAngleValid)
   {
      ut::log::error() << "Actuator lacks a maxAngle within RigidBodyControlActuator::ProcessInput().";
      throw UtInput::UnknownCommand(aInput);
   }

   return myCommand;
}

bool wsf::six_dof::RigidBodyControlActuator::Initialize(int64_t aSimTime_nanosec)
{
   mLastSimTime_nanosec = aSimTime_nanosec;

   return true;
}

void wsf::six_dof::RigidBodyControlActuator::Update(int64_t aSimTime_nanosec, double aCommandedAngle_deg)
{
   // If the "no lag" flag is true, set the current angle and return
   if (mFlightControlsParent->GetParentVehicle()->GetFreezeFlags()->testingNoLag)
   {
      // Set commanded angle
      mCommandedAngle_deg = aCommandedAngle_deg;

      // With no lag, the current angle is the commanded angle
      mCurrentAngle_deg = mCommandedAngle_deg;

      // Enforce limits
      if (mCurrentAngle_deg > mMaxAngle_deg)
      {
         mCurrentAngle_deg = mMaxAngle_deg;
      }
      if (mCurrentAngle_deg < mMinAngle_deg)
      {
         mCurrentAngle_deg = mMinAngle_deg;
      }

      // Set the last time (now)
      mLastSimTime_nanosec = aSimTime_nanosec;

      return;
   }

   int64_t dT_nano = aSimTime_nanosec - mLastSimTime_nanosec;

   if (dT_nano <= 0)
   {
      mLastSimTime_nanosec = aSimTime_nanosec;
      return;
   }

   double dT_sec = utils::TimeToTime(dT_nano);

   mLastSimTime_nanosec = aSimTime_nanosec;

   // Set commanded angle
   mCommandedAngle_deg = aCommandedAngle_deg;

   double desiredDeltaAng_deg = mCommandedAngle_deg - mCurrentAngle_deg;
   if (desiredDeltaAng_deg >= 0)
   {
      // Positive change needed

      double bestNewAng_deg = mCurrentAngle_deg + (mMaxPositiveRate_dps * dT_sec);
      if (bestNewAng_deg >= mCommandedAngle_deg)
      {
         // Able to travel required distance
         mCurrentAngle_deg = mCommandedAngle_deg;
      }
      else
      {
         // Move the max possible
         mCurrentAngle_deg = bestNewAng_deg;
      }
   }
   else
   {
      // Negative change needed

      double bestNewAng_deg = mCurrentAngle_deg + (mMaxNegativeRate_dps * dT_sec);
      if (bestNewAng_deg <= mCommandedAngle_deg)
      {
         // Able to travel required distance
         mCurrentAngle_deg = mCommandedAngle_deg;
      }
      else
      {
         // Move the max possible
         mCurrentAngle_deg = bestNewAng_deg;
      }
   }

   // Limit angle
   if (mCurrentAngle_deg > mMaxAngle_deg)
   {
      mCurrentAngle_deg = mMaxAngle_deg;
   }
   if (mCurrentAngle_deg < mMinAngle_deg)
   {
      mCurrentAngle_deg = mMinAngle_deg;
   }

   return;
}

double wsf::six_dof::RigidBodyControlActuator::UpdateAndGetCurrentAngle_deg(int64_t aSimTime_nanosec,
                                                                            double  aCommandedAngle_deg)
{
   Update(aSimTime_nanosec, aCommandedAngle_deg);
   return mCurrentAngle_deg;
}

double wsf::six_dof::RigidBodyControlActuator::GetCurrentAngle_deg() const
{
   return mCurrentAngle_deg;
}

void wsf::six_dof::RigidBodyControlActuator::SetCurrentAngle_deg(double aAngle_deg)
{
   mCurrentAngle_deg = aAngle_deg;
}
