// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "P6DofHardwareAutopilot.hpp"

#include "P6DofCommonController.hpp"
#include "P6DofVehicle.hpp"

P6DofHardwareAutopilot::P6DofHardwareAutopilot(P6DofScenario* aScenario)
   : P6DofPilotObject(aScenario)
{
}

P6DofHardwareAutopilot* P6DofHardwareAutopilot::Clone() const
{
   return new P6DofHardwareAutopilot(*this);
}

P6DofHardwareAutopilot::P6DofHardwareAutopilot(P6DofPilotObject* aPilotObject)
   : P6DofPilotObject(*aPilotObject)
{
}

void P6DofHardwareAutopilot::HoldAltitude()
{
   P6DofVehicle* vehicle = GetParentVehicle();
   if (vehicle == nullptr)
   {
      return;
   }

   double currentAltitude_ft = vehicle->GetAltitudeMSL_ft();
   SetAutopilotAltitude(currentAltitude_ft);
}

void P6DofHardwareAutopilot::HoldVerticalSpeed()
{
   P6DofVehicle* vehicle = GetParentVehicle();
   if (vehicle == nullptr)
   {
      return;
   }

   double currentVerticalSpeed_fpm = vehicle->GetVertSpeed_fpm();
   SetAutopilotVerticalSpeed(currentVerticalSpeed_fpm);
}

void P6DofHardwareAutopilot::HoldPitchAngle()
{
   P6DofVehicle* vehicle = GetParentVehicle();
   if (vehicle == nullptr)
   {
      return;
   }

   double currentPitchAngle_deg = vehicle->GetPitch_deg();
   this->SetAutopilotPitchAngle(currentPitchAngle_deg);
}

void P6DofHardwareAutopilot::HoldBankAngle()
{
   P6DofVehicle* vehicle = GetParentVehicle();
   if (vehicle == nullptr)
   {
      return;
   }

   double currentRollAngle_deg = vehicle->GetRoll_deg();
   SetAutopilotRollAngle(currentRollAngle_deg);
}

void P6DofHardwareAutopilot::HoldHeading()
{
   P6DofVehicle* vehicle = GetParentVehicle();
   if (vehicle == nullptr)
   {
      return;
   }

   double currentHeading_deg = vehicle->GetHeading_deg();
   SetAutopilotRollHeading(currentHeading_deg);
}

void P6DofHardwareAutopilot::HoldSpeedKCAS()
{
   P6DofVehicle* vehicle = GetParentVehicle();
   if (vehicle == nullptr)
   {
      return;
   }

   double currentSpeed_kias = vehicle->GetKIAS();
   SetAutopilotSpeedKCAS(currentSpeed_kias);
}

void P6DofHardwareAutopilot::HoldSpeedKTAS()
{
   P6DofVehicle* vehicle = GetParentVehicle();
   if (vehicle == nullptr)
   {
      return;
   }

   double currentSpeed_ktas = vehicle->GetKTAS();
   SetAutopilotSpeedKTAS(currentSpeed_ktas);
}

void P6DofHardwareAutopilot::HoldSpeedMach()
{
   P6DofVehicle* vehicle = GetParentVehicle();
   if (vehicle == nullptr)
   {
      return;
   }

   double currentSpeed_mach = vehicle->GetMach();
   SetAutopilotSpeedMach(currentSpeed_mach);
}
