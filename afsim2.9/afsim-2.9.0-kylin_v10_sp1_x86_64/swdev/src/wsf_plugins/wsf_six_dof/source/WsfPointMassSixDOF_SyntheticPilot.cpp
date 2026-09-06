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

#include "WsfPointMassSixDOF_SyntheticPilot.hpp"

#include <cstring>
#include <math.h>

#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "WsfPointMassSixDOF_CommonController.hpp"
#include "WsfPointMassSixDOF_Mover.hpp"

wsf::six_dof::PointMassSyntheticPilot* wsf::six_dof::PointMassSyntheticPilot::Clone() const
{
   return new PointMassSyntheticPilot(*this);
}

wsf::six_dof::PointMassSyntheticPilot::PointMassSyntheticPilot(PointMassPilotObject* aPilotObject)
   : PointMassPilotObject(*aPilotObject)
{
}

bool wsf::six_dof::PointMassSyntheticPilot::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "synthetic_pilot")
   {
      UtInputBlock block(aInput, "end_synthetic_pilot");

      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (PointMassPilotObject::ProcessInput(aInput))
         {
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within PointMassSyntheticPilot::ProcessInput().";
            out.AddNote() << "Command: " << localCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   return myCommand;
}

bool wsf::six_dof::PointMassSyntheticPilot::Initialize(int64_t aSimTime_nanosec)
{
   bool result = PointMassPilotObject::Initialize(aSimTime_nanosec);

   return result;
}


void wsf::six_dof::PointMassSyntheticPilot::HoldAltitude()
{
   Mover* vehicle = GetParentVehicle();
   if (vehicle == nullptr)
   {
      return;
   }

   double currentAltitude_ft = vehicle->GetAltitudeMSL_ft();
   SetAutopilotAltitude(currentAltitude_ft);
}

void wsf::six_dof::PointMassSyntheticPilot::HoldVerticalSpeed()
{
   Mover* vehicle = GetParentVehicle();
   if (vehicle == nullptr)
   {
      return;
   }

   double currentVerticalSpeed_fpm = vehicle->GetVertSpeed_fpm();
   SetAutopilotVerticalSpeed(currentVerticalSpeed_fpm);
}

void wsf::six_dof::PointMassSyntheticPilot::HoldPitchAngle()
{
   Mover* vehicle = GetParentVehicle();
   if (vehicle == nullptr)
   {
      return;
   }

   double currentPitchAngle_deg = vehicle->GetPitch_deg();
   this->SetAutopilotPitchAngle(currentPitchAngle_deg);
}

void wsf::six_dof::PointMassSyntheticPilot::HoldBankAngle()
{
   Mover* vehicle = GetParentVehicle();
   if (vehicle == nullptr)
   {
      return;
   }

   double currentRollAngle_deg = vehicle->GetRoll_deg();
   SetAutopilotRollAngle(currentRollAngle_deg);
}

void wsf::six_dof::PointMassSyntheticPilot::HoldHeading()
{
   Mover* vehicle = GetParentVehicle();
   if (vehicle == nullptr)
   {
      return;
   }

   double currentHeading_deg = vehicle->GetHeading_deg();
   SetAutopilotRollHeading(currentHeading_deg);
}

void wsf::six_dof::PointMassSyntheticPilot::HoldSpeedKCAS()
{
   Mover* vehicle = GetParentVehicle();
   if (vehicle == nullptr)
   {
      return;
   }

   double currentSpeed_kias = vehicle->GetKIAS();
   SetAutopilotSpeedKCAS(currentSpeed_kias);
}

void wsf::six_dof::PointMassSyntheticPilot::HoldSpeedKTAS()
{
   Mover* vehicle = GetParentVehicle();
   if (vehicle == nullptr)
   {
      return;
   }

   double currentSpeed_ktas = vehicle->GetKTAS();
   SetAutopilotSpeedKTAS(currentSpeed_ktas);
}

void wsf::six_dof::PointMassSyntheticPilot::HoldSpeedMach()
{
   Mover* vehicle = GetParentVehicle();
   if (vehicle == nullptr)
   {
      return;
   }

   double currentSpeed_mach = vehicle->GetMach();
   SetAutopilotSpeedMach(currentSpeed_mach);
}
