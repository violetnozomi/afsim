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

#include "P6DofGuidanceAutopilot.hpp"

#include "P6DofCommonController.hpp"
#include "P6DofVehicle.hpp"

P6DofGuidanceAutopilot::P6DofGuidanceAutopilot(P6DofScenario* aScenario)
   : P6DofPilotObject(aScenario)
{
}

P6DofGuidanceAutopilot* P6DofGuidanceAutopilot::Clone() const
{
   return new P6DofGuidanceAutopilot(*this);
}

P6DofGuidanceAutopilot::P6DofGuidanceAutopilot(P6DofPilotObject* aPilotObject)
   : P6DofPilotObject(*aPilotObject)
{
}

void P6DofGuidanceAutopilot::HoldAltitude()
{
   P6DofVehicle* vehicle = GetParentVehicle();
   if (vehicle == nullptr)
   {
      return;
   }

   double currentAltitude_ft = vehicle->GetAltitudeMSL_ft();
   SetAutopilotAltitude(currentAltitude_ft);
}

void P6DofGuidanceAutopilot::HoldVerticalSpeed()
{
   P6DofVehicle* vehicle = GetParentVehicle();
   if (vehicle == nullptr)
   {
      return;
   }

   double currentVerticalSpeed_fpm = vehicle->GetVertSpeed_fpm();
   SetAutopilotVerticalSpeed(currentVerticalSpeed_fpm);
}

void P6DofGuidanceAutopilot::HoldPitchAngle()
{
   P6DofVehicle* vehicle = GetParentVehicle();
   if (vehicle == nullptr)
   {
      return;
   }

   double currentPitchAngle_deg = vehicle->GetPitch_deg();
   this->SetAutopilotPitchAngle(currentPitchAngle_deg);
}

void P6DofGuidanceAutopilot::HoldBankAngle()
{
   P6DofVehicle* vehicle = GetParentVehicle();
   if (vehicle == nullptr)
   {
      return;
   }

   double currentRollAngle_deg = vehicle->GetRoll_deg();
   SetAutopilotRollAngle(currentRollAngle_deg);
}

void P6DofGuidanceAutopilot::HoldHeading()
{
   P6DofVehicle* vehicle = GetParentVehicle();
   if (vehicle == nullptr)
   {
      return;
   }

   double currentHeading_deg = vehicle->GetHeading_deg();
   SetAutopilotRollHeading(currentHeading_deg);
}

void P6DofGuidanceAutopilot::HoldSpeedKCAS()
{
   P6DofVehicle* vehicle = GetParentVehicle();
   if (vehicle == nullptr)
   {
      return;
   }

   double currentSpeed_kias = vehicle->GetKIAS();
   SetAutopilotSpeedKCAS(currentSpeed_kias);
}

void P6DofGuidanceAutopilot::HoldSpeedKTAS()
{
   P6DofVehicle* vehicle = GetParentVehicle();
   if (vehicle == nullptr)
   {
      return;
   }

   double currentSpeed_ktas = vehicle->GetKTAS();
   SetAutopilotSpeedKTAS(currentSpeed_ktas);
}

void P6DofGuidanceAutopilot::HoldSpeedMach()
{
   P6DofVehicle* vehicle = GetParentVehicle();
   if (vehicle == nullptr)
   {
      return;
   }

   double currentSpeed_mach = vehicle->GetMach();
   SetAutopilotSpeedMach(currentSpeed_mach);
}
