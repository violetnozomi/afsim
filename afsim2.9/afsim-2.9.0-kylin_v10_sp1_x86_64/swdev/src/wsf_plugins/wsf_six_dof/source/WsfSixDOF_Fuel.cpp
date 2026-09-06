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

#include "WsfSixDOF_Fuel.hpp"

#include <algorithm>

#include "UtAtmosphere.hpp"
#include "UtException.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"

wsf::six_dof::Fuel::Fuel(WsfScenario& aScenario)
   : WsfFuel(aScenario)
{
}

wsf::six_dof::Fuel::Fuel(const Fuel& aSrc)
   : WsfFuel(aSrc)
{
}

WsfFuel* wsf::six_dof::Fuel::Clone() const
{
   return new Fuel(*this);
}

bool wsf::six_dof::Fuel::Initialize2(double aSimTime)
{
   SetSixDOF_Mover();

   if (mMoverPtr == nullptr)
   {
      throw UtException("ERROR: No SixDOF Mover in Fuel::Initialize2()");
   }

   // Adjust the maximum_quantity based on SixDOF Mover
   double totalFuel_kg = mMoverPtr->GetTotalFuelCapacity();
   WsfFuel::SetMaximumQuantity(totalFuel_kg);

   // Load out fuel based on the initial_quantity. Start by ensuring all tanks are empty.
   mMoverPtr->EmptyAllInternalTanks();
   mMoverPtr->EmptyAllExternalTanks();

   // Next, add the specified amount of fuel (convert to lbs)
   double fuelLoadout_kg = GetInitialQuantity();
   mMoverPtr->AddFuel(fuelLoadout_kg);

   // Call base class
   return WsfFuel::Initialize2(aSimTime);
}

bool wsf::six_dof::Fuel::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   // Note: maximum_quantity, initial_quantity are read by the base class. These will
   // be handled/adjusted in Initialize2.
   if (command == "consumption_rate")
   {
      auto out = ut::log::warning() << "'consumption_rate' is not supported for WSF_SIX_DOF_FUEL and will be ignored.";
      out.AddNote() << "Location: " << aInput.GetLocation();
   }
   else if (command == "maximum_quantity")
   {
      auto out = ut::log::warning() << "'maximum_quantity' is not supported for WSF_SIX_DOF_FUEL and will be ignored.";
      out.AddNote() << "Location: " << aInput.GetLocation();
   }
   else if (command == "mode")
   {
      auto out = ut::log::warning() << "'mode' has no effect for WSF_SIX_DOF_FUEL and will be ignored.";
      out.AddNote() << "Location: " << aInput.GetLocation();
   }
   else if (command == "initial_quantity")
   {
      double initialQuantity = 0.0;
      aInput.ReadValueOfType(initialQuantity, UtInput::cMASS);
      aInput.ValueGreaterOrEqual(initialQuantity, 0.0);
      SetInitialQuantity(initialQuantity);
   }
   else if ((command == "reserve_quantity") || (command == "reserve_fuel"))
   {
      aInput.ReadValueOfType(mReserveQuantity, UtInput::cMASS);
      aInput.ValueGreaterOrEqual(mReserveQuantity, 0.0);
   }
   else if ((command == "bingo_quantity") || (command == "bingo_fuel"))
   {
      aInput.ReadValueOfType(mBingoQuantity, UtInput::cMASS);
      aInput.ValueGreaterOrEqual(mBingoQuantity, 0.0);
   }
   else
   {
      myCommand = WsfFuel::ProcessInput(aInput);
   }
   return myCommand;
}

void wsf::six_dof::Fuel::Update(double aSimTime)
{
   if (mMoverPtr == nullptr)
   {
      throw UtException("ERROR: No SixDOF Mover in Fuel::Update()");
   }

   double lastUpdate_sec = GetLastUpdateTime();
   if (aSimTime > lastUpdate_sec)
   {
      // Update the fuel rates and quantities.
      UpdateSixDOF_FuelData(aSimTime);
      CheckEvents(aSimTime);
      mLastUpdate = aSimTime;
   }
}

double wsf::six_dof::Fuel::GetConsumptionRate()
{
   return mMoverPtr->GetTotalFuelFlow();
}

// For now, this returns the current consumption rate. Later, it will utilize alt and spd.
double wsf::six_dof::Fuel::GetConsumptionRate(const double aAltitude, const double aSpeed)
{
   return GetConsumptionRate();
}

void wsf::six_dof::Fuel::SetMaximumQuantity(double aQuantity)
{
   ut::log::warning() << "SetMaximumQuantity() is not supported for WSF_SIX_DOF_FUEL.";
}

double wsf::six_dof::Fuel::GetQuantityRemaining() const
{
   if (mMoverPtr != nullptr)
   {
      return mMoverPtr->GetTotalFuelRemaining();
   }
   return 0.0;
}

bool wsf::six_dof::Fuel::Refuel(double aSimTime)
{
   if (mMoverPtr != nullptr)
   {
      mMoverPtr->AddFuel(mMoverPtr->GetTotalFuelCapacity());
      return true;
   }
   return false;
}

bool wsf::six_dof::Fuel::Refuel(double aSimTime, double aQuantity)
{
   if (mMoverPtr != nullptr)
   {
      mMoverPtr->AddFuel(aQuantity);
      return true;
   }
   return false;
}

void wsf::six_dof::Fuel::SetSixDOF_Mover()
{
   if (mMoverPtr == nullptr)
   {
      WsfPlatform* platformPtr = GetPlatform();
      if (platformPtr != nullptr)
      {
         WsfMover* mover = platformPtr->GetMover();
         if (mover != nullptr)
         {
            mMoverPtr = dynamic_cast<Mover*>(mover);
         }
      }
   }
}

void wsf::six_dof::Fuel::UpdateSixDOF_FuelData(double aSimTime)
{
   double fuelFlow_kps = 0.0;

   if (mMoverPtr != nullptr)
   {
      fuelFlow_kps = mMoverPtr->GetTotalFuelFlow();
   }

   SetConsumptionRate(fuelFlow_kps);

   double quantity_kg = mMoverPtr->GetTotalFuelRemaining();
   SetQuantityRemaining(quantity_kg);
}

double wsf::six_dof::Fuel::CalcConsumptionRate()
{
   double fuelFlow_kps = 0.0;
   if (mMoverPtr != nullptr)
   {
      fuelFlow_kps = mMoverPtr->GetTotalFuelFlow();
   }
   return fuelFlow_kps;
}

// Note: At the present time, this calculates the fuel consumption under current conditions
double wsf::six_dof::Fuel::CalcConsumptionRateAltitudeSpeed(const double aAltitude, const double aSpeed)
{
   return CalcConsumptionRate();
}
