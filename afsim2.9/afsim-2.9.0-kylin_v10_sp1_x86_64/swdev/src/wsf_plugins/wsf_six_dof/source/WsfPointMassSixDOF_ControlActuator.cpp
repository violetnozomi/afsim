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

#include "WsfPointMassSixDOF_ControlActuator.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "WsfPointMassSixDOF_FlightControlSystem.hpp"
#include "WsfPointMassSixDOF_Utils.hpp"
#include "WsfSixDOF_TypeManager.hpp"

wsf::six_dof::PointMassControlActuator::PointMassControlActuator(PointMassFlightControlSystem* aFlightControlsParent)
   : mFlightControlsParent(aFlightControlsParent)
{
}

wsf::six_dof::PointMassControlActuator::PointMassControlActuator(const PointMassControlActuator& aSrc,
                                                                 PointMassFlightControlSystem*   aFlightControlsParent)
   : mFlightControlsParent(aFlightControlsParent)
   , mLastSimTime_nanosec(aSrc.mLastSimTime_nanosec)
   , mCurrentSetting(aSrc.mCurrentSetting)
   , mCommandedSetting(aSrc.mCommandedSetting)
   , mLagTimeConstant_sec(aSrc.mLagTimeConstant_sec)
{
}

wsf::six_dof::PointMassControlActuator*
wsf::six_dof::PointMassControlActuator::Clone(PointMassFlightControlSystem* aFlightControlsParent) const
{
   return new PointMassControlActuator(*this, aFlightControlsParent);
}

bool wsf::six_dof::PointMassControlActuator::ProcessInput(UtInputBlock& aInputBlock)
{
   std::string command;
   UtInput&    controlInput = aInputBlock.GetInput();

   while (aInputBlock.ReadCommand(command))
   {
      if (command == "lag_time_constant")
      {
         double value = 0.0;
         controlInput.ReadValueOfType(value, UtInput::cTIME);
         mLagTimeConstant_sec = value;
      }
   }

   return true;
}

bool wsf::six_dof::PointMassControlActuator::Initialize(int64_t aSimTime_nanosec)
{
   mLastSimTime_nanosec = aSimTime_nanosec;

   return true;
}

void wsf::six_dof::PointMassControlActuator::Update(int64_t aSimTime_nanosec, double aCommandedSetting)
{
   // If the "no lag" flag is true, set the current angle and return
   if (mFlightControlsParent->GetParentVehicle()->GetFreezeFlags()->testingNoLag)
   {
      // Set commanded angle
      mCommandedSetting = aCommandedSetting;

      // With no lag, the current angle is the commanded angle
      mCurrentSetting = aCommandedSetting;

      // Enforce limits
      if (mCurrentSetting > 1.0)
      {
         mCurrentSetting = 1.0;
      }
      if (mCurrentSetting < 0.0)
      {
         mCurrentSetting = 0.0;
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

   // Set commanded setting
   mCommandedSetting = aCommandedSetting;

   // Update setting via first-order lag
   mCurrentSetting =
      (mCommandedSetting * dT_sec + mCurrentSetting * mLagTimeConstant_sec) / (mLagTimeConstant_sec + dT_sec);

   // Limit setting to [0,1]
   if (mCurrentSetting > 1.0)
   {
      mCurrentSetting = 1.0;
   }
   if (mCurrentSetting < 0.0)
   {
      mCurrentSetting = 0.0;
   }

   return;
}

double wsf::six_dof::PointMassControlActuator::UpdateAndGetCurrentSetting(int64_t aSimTime_nanosec, double aCommandedSetting)
{
   Update(aSimTime_nanosec, aCommandedSetting);
   return mCurrentSetting;
}

double wsf::six_dof::PointMassControlActuator::GetCurrentSetting() const
{
   return mCurrentSetting;
}

void wsf::six_dof::PointMassControlActuator::SetCurrentSetting(double aSetting)
{
   mCurrentSetting = aSetting;
}
