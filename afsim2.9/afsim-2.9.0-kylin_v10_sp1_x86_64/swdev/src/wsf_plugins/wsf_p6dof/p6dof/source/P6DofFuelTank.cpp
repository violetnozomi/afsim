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

#include "P6DofFuelTank.hpp"

#include "P6DofFreezeFlags.hpp"
#include "P6DofObject.hpp"
#include "P6DofPropulsionSystem.hpp"
#include "P6DofScenario.hpp"
#include "P6DofVehicle.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

P6DofFuelTank::P6DofFuelTank(P6DofScenario* aScenario)
   : P6DofObject(aScenario)
   , mPropulsionSystem(nullptr)
   , mMaxFlowRate_pps(0)
   , mMaxFillRate_pps(0)
   , mMaxTransferRate_pps(0)
   , mMaxQuantity_lbs(0)
   , mCurrentQuantity_lbs(0)
   , mCurrentFuelFlow_pps(0)
   , mCurrentFillRate_pps(0)
   , mCurrentTransferRate_pps(0)
   , mTempCurrentFuelFlow_pps(0)
   , mTempCurrentFillRate_pps(0)
   , mTempCurrentTransferRate_pps(0)
   , mCurrentCgLocation_ft(0., 0., 0.)
   , mFullCgLocation_ft(0., 0., 0.)
   , mEmptyCgLocation_ft(0., 0., 0.)
   , mLastSimTime_nanosec(0)
   , mMassProperties()
{
   // These functions will set mCgEmptyToFullVector
   SetFullCgLocation_ft(mFullCgLocation_ft);
   SetEmptyCgLocation_ft(mEmptyCgLocation_ft);
}

P6DofFuelTank::~P6DofFuelTank() {}

P6DofFuelTank* P6DofFuelTank::Clone() const
{
   return new P6DofFuelTank(*this);
}

P6DofFuelTank::P6DofFuelTank(const P6DofFuelTank& aSrc)
   : P6DofObject(aSrc)
   , mPropulsionSystem(aSrc.mPropulsionSystem)
   , mMaxFlowRate_pps(aSrc.mMaxFlowRate_pps)
   , mMaxFillRate_pps(aSrc.mMaxFillRate_pps)
   , mMaxTransferRate_pps(aSrc.mMaxTransferRate_pps)
   , mMaxQuantity_lbs(aSrc.mMaxQuantity_lbs)
   , mCurrentQuantity_lbs(aSrc.mCurrentQuantity_lbs)
   , mCurrentFuelFlow_pps(aSrc.mCurrentFuelFlow_pps)
   , mCurrentFillRate_pps(aSrc.mCurrentFillRate_pps)
   , mCurrentTransferRate_pps(aSrc.mCurrentTransferRate_pps)
   , mTempCurrentFuelFlow_pps(aSrc.mTempCurrentFuelFlow_pps)
   , mTempCurrentFillRate_pps(aSrc.mTempCurrentFillRate_pps)
   , mTempCurrentTransferRate_pps(aSrc.mTempCurrentTransferRate_pps)
   , mCurrentCgLocation_ft(aSrc.mCurrentCgLocation_ft)
   , mFullCgLocation_ft(aSrc.mFullCgLocation_ft)
   , mEmptyCgLocation_ft(aSrc.mEmptyCgLocation_ft)
   , mLastSimTime_nanosec(aSrc.mLastSimTime_nanosec)
   , mMassProperties(aSrc.mMassProperties)
{
   // These functions will set mCgEmptyToFullVector
   SetFullCgLocation_ft(mFullCgLocation_ft);
   SetEmptyCgLocation_ft(mEmptyCgLocation_ft);
}

bool P6DofFuelTank::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "fuel_tank")
   {
      std::string name;
      aInput.ReadValue(name);
      SetName(name);

      UtInputBlock block(aInput, "end_fuel_tank");
      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();

         if (localCommand == "max_fuel_quantity")
         {
            double mass = 0.0;
            aInput.ReadValueOfType(mass, UtInput::cMASS);
            mMaxQuantity_lbs = mass * UtMath::cLB_PER_KG;
         }
         else if (localCommand == "current_fuel_quantity")
         {
            double mass = 0.0;
            aInput.ReadValueOfType(mass, UtInput::cMASS);
            mCurrentQuantity_lbs  = mass * UtMath::cLB_PER_KG;
            mCurrentCgLocation_ft = CalcCgLocation_ft(mCurrentQuantity_lbs);
         }
         else if (localCommand == "max_flow_rate")
         {
            double massflow_kg_per_sec = 0.0;
            aInput.ReadValueOfType(massflow_kg_per_sec, UtInput::cMASS_TRANSFER);
            mMaxFlowRate_pps = massflow_kg_per_sec;
            mMaxFlowRate_pps *= UtMath::cLB_PER_KG;
         }
         else if (localCommand == "max_fill_rate")
         {
            double massflow_kg_per_sec = 0.0;
            aInput.ReadValueOfType(massflow_kg_per_sec, UtInput::cMASS_TRANSFER);
            mMaxFillRate_pps = massflow_kg_per_sec;
            mMaxFillRate_pps *= UtMath::cLB_PER_KG;
         }
         else if (localCommand == "max_xfer_rate")
         {
            double massflow_kg_per_sec = 0.0;
            aInput.ReadValueOfType(massflow_kg_per_sec, UtInput::cMASS_TRANSFER);
            mMaxTransferRate_pps = massflow_kg_per_sec;
            mMaxTransferRate_pps *= UtMath::cLB_PER_KG;
         }
         else if (localCommand == "cg_full_x")
         {
            double length = 0.0;
            aInput.ReadValueOfType(length, UtInput::cLENGTH);
            double length_ft = length * UtMath::cFT_PER_M;

            double x, y, z;
            mFullCgLocation_ft.Get(x, y, z);
            mFullCgLocation_ft.Set(length_ft, y, z);

            SetFullCgLocation_ft(mFullCgLocation_ft);
            mCurrentCgLocation_ft = CalcCgLocation_ft(mCurrentQuantity_lbs);
         }
         else if (localCommand == "cg_full_y")
         {
            double length = 0.0;
            aInput.ReadValueOfType(length, UtInput::cLENGTH);
            double length_ft = length * UtMath::cFT_PER_M;

            double x, y, z;
            mFullCgLocation_ft.Get(x, y, z);
            mFullCgLocation_ft.Set(x, length_ft, z);

            SetFullCgLocation_ft(mFullCgLocation_ft);
            mCurrentCgLocation_ft = CalcCgLocation_ft(mCurrentQuantity_lbs);
         }
         else if (localCommand == "cg_full_z")
         {
            double length = 0.0;
            aInput.ReadValueOfType(length, UtInput::cLENGTH);
            double length_ft = length * UtMath::cFT_PER_M;

            double x, y, z;
            mFullCgLocation_ft.Get(x, y, z);
            mFullCgLocation_ft.Set(x, y, length_ft);

            SetFullCgLocation_ft(mFullCgLocation_ft);
            mCurrentCgLocation_ft = CalcCgLocation_ft(mCurrentQuantity_lbs);
         }
         else if (localCommand == "cg_empty_x")
         {
            double length = 0.0;
            aInput.ReadValueOfType(length, UtInput::cLENGTH);
            double length_ft = length * UtMath::cFT_PER_M;

            double x, y, z;
            mEmptyCgLocation_ft.Get(x, y, z);
            mEmptyCgLocation_ft.Set(length_ft, y, z);

            SetEmptyCgLocation_ft(mEmptyCgLocation_ft);
            mCurrentCgLocation_ft = CalcCgLocation_ft(mCurrentQuantity_lbs);
         }
         else if (localCommand == "cg_empty_y")
         {
            double length = 0.0;
            aInput.ReadValueOfType(length, UtInput::cLENGTH);
            double length_ft = length * UtMath::cFT_PER_M;

            double x, y, z;
            mEmptyCgLocation_ft.Get(x, y, z);
            mEmptyCgLocation_ft.Set(x, length_ft, z);

            SetEmptyCgLocation_ft(mEmptyCgLocation_ft);
            mCurrentCgLocation_ft = CalcCgLocation_ft(mCurrentQuantity_lbs);
         }
         else if (localCommand == "cg_empty_z")
         {
            double length = 0.0;
            aInput.ReadValueOfType(length, UtInput::cLENGTH);
            double length_ft = length * UtMath::cFT_PER_M;

            double x, y, z;
            mEmptyCgLocation_ft.Get(x, y, z);
            mEmptyCgLocation_ft.Set(x, y, length_ft);

            SetEmptyCgLocation_ft(mEmptyCgLocation_ft);
            mCurrentCgLocation_ft = CalcCgLocation_ft(mCurrentQuantity_lbs);
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within P6DofFuelTank::ProcessInput().";
            out.AddNote() << "Command: " << localCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   return myCommand;
}

bool P6DofFuelTank::Initialize(int64_t aSimTime_nanosec)
{
   // Set last time
   mLastSimTime_nanosec = aSimTime_nanosec;
   CalculateCurrentMassProperties();
   return true;
}

bool P6DofFuelTank::FuelFlowPathIntact(P6DofPropulsionSystem* aSystemDrawingFuel)
{
   // If no parent P6DofPropulsionSystem is specified, we can't flow fuel
   if (mPropulsionSystem == nullptr)
   {
      return false;
   }

   // If the parent P6DofPropulsionSystem is the system drawing fuel, we're all set
   if (mPropulsionSystem == aSystemDrawingFuel)
   {
      return true;
   }

   // If there is no parent vehicle, we cannot find subobjects or parents, so there is no path
   if (mParentVehicle == nullptr)
   {
      return false;
   }

   // Get the P6DofVehicle for the drawing system
   P6DofVehicle* drawingVehicle = aSystemDrawingFuel->GetParentVehicle();

   // If there is no drawing vehicle, there is no path
   if (drawingVehicle == nullptr)
   {
      return false;
   }

   // At this point, we have the vehicle which holds the fuel tank and the vehicle
   // which is drawing fuel and we know they are different vehicles.

   // Next, we check if the drawing vehicle is a subobject of the fuel tank's vehicle
   P6DofVehicle* tempObj = mParentVehicle->GetSubobjectByName(drawingVehicle->GetName());
   if (tempObj == drawingVehicle)
   {
      // A subobject is drawing fuel, so we're connected
      return true;
   }

   // Finally, we determine if the fuel tank's vehicle is a subobject of the drawing vehicle
   tempObj = drawingVehicle->GetSubobjectByName(mParentVehicle->GetName());
   if (tempObj == mParentVehicle)
   {
      // A higher-level (parent) vehicle is drawing fuel, so we're connected
      return true;
   }

   // There is no path
   return false;
}

void P6DofFuelTank::Update(int64_t aSimTime_nanosec)
{
   // Calculate delta-t (not currently used)
   // CleanThisUp int64_t dT_nanosec = aSimTime_nanosec - mLastSimTime_nanosec;
   // CleanThisUp double dT_sec = P6DofKinematicState::TimeToTime(dT_nanosec);

   // Ensure quantities are within limits
   if (mCurrentQuantity_lbs > mMaxQuantity_lbs)
   {
      mCurrentQuantity_lbs = mMaxQuantity_lbs;
   }
   else if (mCurrentQuantity_lbs < 0.0)
   {
      mCurrentQuantity_lbs = 0.0;
   }

   // Set rates
   mCurrentFuelFlow_pps     = mTempCurrentFuelFlow_pps;
   mCurrentFillRate_pps     = mTempCurrentFillRate_pps;
   mCurrentTransferRate_pps = mTempCurrentTransferRate_pps;

   // Reset temp rates
   mTempCurrentFuelFlow_pps     = 0.0;
   mTempCurrentFillRate_pps     = 0.0;
   mTempCurrentTransferRate_pps = 0.0;

   // Set the last time
   mLastSimTime_nanosec = aSimTime_nanosec;
}

bool P6DofFuelTank::CalculateFuelBurn(double    aDeltaT_sec,
                                      double    aFuelBurnRequest_lbs,
                                      double&   aFuelActuallyProvided_lbs,
                                      double&   aNewFuelMass_lbs,
                                      UtVec3dX& aCgLocation_ft)
{
   // Ensure we never divide by 0
   if (aDeltaT_sec < P6DofUtils::cEPSILON_SIMTIME_SEC)
   {
      // A negligible fuel request should be treated as fulfilled;
      // nothing was asked for and nothing was given

      aFuelActuallyProvided_lbs = 0.0;
      aNewFuelMass_lbs          = mCurrentQuantity_lbs;
      aCgLocation_ft            = mCurrentCgLocation_ft;
      return true;
   }

   bool   fullFuelRequestAchieved = false;
   double fuelBurned_lbs          = 0.0;
   double requestedFlowRate_pps   = aFuelBurnRequest_lbs / aDeltaT_sec;

   // First, test for max flow rate
   if (requestedFlowRate_pps > mMaxFlowRate_pps)
   {
      // Exceeded the max flow rate, so will likely not meet the demand

      // Set the max amount of fuel burn, given the dT
      double limitedFuelBurn_lbs = mMaxFlowRate_pps * aDeltaT_sec;

      if (aFuelBurnRequest_lbs > limitedFuelBurn_lbs)
      {
         // Unable to provide requested fuel
         fuelBurned_lbs = limitedFuelBurn_lbs;
      }
      else
      {
         // Somehow we're able to provide the requested fuel?
         fuelBurned_lbs = aFuelBurnRequest_lbs;
      }

      // Now, check for limit by quantity
      double fuelRemaining_lbs = mCurrentQuantity_lbs - fuelBurned_lbs;
      if (fuelRemaining_lbs > 0.0)
      {
         // We have enough fuel
      }
      else
      {
         // Not enough fuel

         // We have a negative value for fuelRemaining_lbs, so add it
         fuelBurned_lbs = aFuelBurnRequest_lbs + fuelRemaining_lbs;
      }
   }
   else
   {
      // Within flow rate limit, now check quantity
      double fuelRemaining_lbs = mCurrentQuantity_lbs - aFuelBurnRequest_lbs;

      if (fuelRemaining_lbs > 0.0)
      {
         // Able to meet fuel qty and flow needs
         fullFuelRequestAchieved = true;
         fuelBurned_lbs          = aFuelBurnRequest_lbs;
      }
      else
      {
         // Able to meet flow requirements, but lack enough fuel

         // We have a negative value for fuelRemaining_lbs, so add it
         fuelBurned_lbs = aFuelBurnRequest_lbs + fuelRemaining_lbs;
      }
   }

   // Return calculated values
   aFuelActuallyProvided_lbs = fuelBurned_lbs;
   aNewFuelMass_lbs          = mCurrentQuantity_lbs - fuelBurned_lbs;
   aCgLocation_ft            = CalcCgLocation_ft(aNewFuelMass_lbs);

   return fullFuelRequestAchieved;
}

bool P6DofFuelTank::UpdateFuelBurn(double    aDeltaT_sec,
                                   double    aFuelBurnRequest_lbs,
                                   double&   aFuelActuallyProvided_lbs,
                                   double&   aNewFuelMass_lbs,
                                   UtVec3dX& aCgLocation_ft)
{
   // Ensure we never divide by 0
   if (aDeltaT_sec < P6DofUtils::cEPSILON_SIMTIME_SEC)
   {
      // A negligible fuel request should be treated as fulfilled;
      // nothing was asked for and nothing was given

      aFuelActuallyProvided_lbs = 0.0;
      aNewFuelMass_lbs          = mCurrentQuantity_lbs;
      aCgLocation_ft            = mCurrentCgLocation_ft;
      return true;
   }

   bool ableToProvideFuel =
      CalculateFuelBurn(aDeltaT_sec, aFuelBurnRequest_lbs, aFuelActuallyProvided_lbs, aNewFuelMass_lbs, aCgLocation_ft);

   // Change flow rate state (cumulative during time step)
   mTempCurrentFuelFlow_pps += aFuelActuallyProvided_lbs / aDeltaT_sec;

   // Don't change fuel state data if frozen, just return
   if (GetScenario()->GetFreezeFlags()->GetMasterFreezeFuelBurn())
   {
      return ableToProvideFuel;
   }

   // Change mass state
   mCurrentQuantity_lbs = aNewFuelMass_lbs;
   if (mCurrentQuantity_lbs < 0.0)
   {
      mCurrentQuantity_lbs = 0.0;
   }

   // Change cg location state
   mCurrentCgLocation_ft = aCgLocation_ft;

   return ableToProvideFuel;
}

bool P6DofFuelTank::CalculateFuelFill(double    aDeltaT_sec,
                                      double    aFuelAddRequest_lbs,
                                      double&   aFuelActuallyProvided_lbs,
                                      double&   aNewFuelMass_lbs,
                                      UtVec3dX& aCgLocation_ft)
{
   // Ensure we never divide by 0
   if (aDeltaT_sec < P6DofUtils::cEPSILON_SIMTIME_SEC)
   {
      // A negligible fuel request should be treated as fulfilled;
      // nothing was asked for and nothing was given

      aFuelActuallyProvided_lbs = 0.0;
      aNewFuelMass_lbs          = mCurrentQuantity_lbs;
      aCgLocation_ft            = mCurrentCgLocation_ft;
      return true;
   }

   bool   fullFuelRequestAchieved = false;
   double fuelTransfer_lbs        = 0.0;
   double requestedInputRate_pps  = aFuelAddRequest_lbs / aDeltaT_sec;

   // First, test for max flow rate
   if (requestedInputRate_pps > mMaxFillRate_pps)
   {
      // Exceeded the max input rate, so will likely not meet the demand

      // Set the max amount of fuel input, given the dT
      double limitedInputAmount_lbs = mMaxFillRate_pps * aDeltaT_sec;

      if (aFuelAddRequest_lbs > limitedInputAmount_lbs)
      {
         // Unable to accept requested fuel transfer
         fuelTransfer_lbs = limitedInputAmount_lbs;
      }
      else
      {
         // Somehow we're able to accept the requested fuel?
         fuelTransfer_lbs = aFuelAddRequest_lbs;
      }

      // Now, check for limit by max quantity
      double fuelAfterAdding_lbs = mCurrentQuantity_lbs + fuelTransfer_lbs;
      if (fuelAfterAdding_lbs < mMaxQuantity_lbs)
      {
         // We can take on the full amount of fuel
      }
      else
      {
         // Too much fuel

         // Determine how much is excess
         double excessFuel_lbs = fuelAfterAdding_lbs - mMaxQuantity_lbs;

         // We can only accept a limited amount of fuel
         fuelTransfer_lbs = aFuelAddRequest_lbs - excessFuel_lbs;
      }
   }
   else
   {
      // Within flow rate limit, now check for limit by max quantity
      double fuelAfterAdding_lbs = mCurrentQuantity_lbs + aFuelAddRequest_lbs;

      if (fuelAfterAdding_lbs < mMaxQuantity_lbs)
      {
         // We can take on the full amount of fuel
         fullFuelRequestAchieved = true;
         fuelTransfer_lbs        = aFuelAddRequest_lbs;
      }
      else
      {
         // Too much fuel

         // Determine how much is excess
         double excessFuel_lbs = fuelAfterAdding_lbs - mMaxQuantity_lbs;

         // We can only accept a limited amount of fuel
         fuelTransfer_lbs = aFuelAddRequest_lbs - excessFuel_lbs;
      }
   }

   // Return calculated values
   aFuelActuallyProvided_lbs = fuelTransfer_lbs;
   aNewFuelMass_lbs          = mCurrentQuantity_lbs + fuelTransfer_lbs;
   aCgLocation_ft            = CalcCgLocation_ft(aNewFuelMass_lbs);

   return fullFuelRequestAchieved;
}


bool P6DofFuelTank::UpdateFuelFill(double    aDeltaT_sec,
                                   double    aFuelAddRequest_lbs,
                                   double&   aFuelActuallyProvided_lbs,
                                   double&   aNewFuelMass_lbs,
                                   UtVec3dX& aCgLocation_ft)
{
   // Ensure we never divide by 0
   if (aDeltaT_sec < P6DofUtils::cEPSILON_SIMTIME_SEC)
   {
      // A negligible fuel request should be treated as fulfilled;
      // nothing was asked for and nothing was given

      aFuelActuallyProvided_lbs = 0.0;
      aNewFuelMass_lbs          = mCurrentQuantity_lbs;
      aCgLocation_ft            = mCurrentCgLocation_ft;
      return true;
   }

   bool ableToProvideFuel =
      CalculateFuelFill(aDeltaT_sec, aFuelAddRequest_lbs, aFuelActuallyProvided_lbs, aNewFuelMass_lbs, aCgLocation_ft);

   // Change fill rate state (cumulative during time step)
   mTempCurrentFillRate_pps += aFuelActuallyProvided_lbs / aDeltaT_sec;

   // Change mass state
   mCurrentQuantity_lbs = aNewFuelMass_lbs;
   if (mCurrentQuantity_lbs > mMaxQuantity_lbs)
   {
      mCurrentQuantity_lbs = mMaxQuantity_lbs;
   }

   // Change cg location state
   mCurrentCgLocation_ft = aCgLocation_ft;

   return ableToProvideFuel;
}

bool P6DofFuelTank::CalculateFuelTransfer(double    aDeltaT_sec,
                                          double    aFuelAddRequest_lbs,
                                          double&   aFuelActuallyProvided_lbs,
                                          double&   aNewFuelMass_lbs,
                                          UtVec3dX& aCgLocation_ft)
{
   // Ensure we never divide by 0
   if (aDeltaT_sec < P6DofUtils::cEPSILON_SIMTIME_SEC)
   {
      // A negligible fuel request should be treated as fulfilled;
      // nothing was asked for and nothing was given

      aFuelActuallyProvided_lbs = 0.0;
      aNewFuelMass_lbs          = mCurrentQuantity_lbs;
      aCgLocation_ft            = mCurrentCgLocation_ft;
      return true;
   }

   bool   fullFuelRequestAchieved = false;
   double fuelTransfer_lbs        = 0.0;
   double requestedInputRate_pps  = aFuelAddRequest_lbs / aDeltaT_sec;

   // With transfers, aFuelAddRequest_lbs can be positive (adding fuel)
   // or negative (removing fuel).

   // First, test for max flow rate
   if (fabs(requestedInputRate_pps) > mMaxTransferRate_pps)
   {
      // Exceeded the max input rate, so may not meet the demand

      // Set the max amount of fuel input, given the dT
      double limitedInputAmount_lbs = mMaxTransferRate_pps * aDeltaT_sec;

      if (fabs(aFuelAddRequest_lbs) > limitedInputAmount_lbs)
      {
         // Unable to accept requested fuel transfer
         if (aFuelAddRequest_lbs >= 0.0)
         {
            fuelTransfer_lbs = limitedInputAmount_lbs;
         }
         else
         {
            fuelTransfer_lbs = -limitedInputAmount_lbs;
         }
      }
      else
      {
         // Somehow we're able to accept the requested fuel?
         fuelTransfer_lbs = aFuelAddRequest_lbs;
      }

      // Now, check for limit by max/min quantity
      double fuelAfterAdding_lbs = mCurrentQuantity_lbs + fuelTransfer_lbs;
      if (fuelAfterAdding_lbs <= mMaxQuantity_lbs)
      {
         // Within max limit, check min limit
         if (fuelAfterAdding_lbs > 0.0)
         {
            // Still have fuel, no additional change needed
         }
         else
         {
            // Ran out of fuel -- check for positive or negative transfer
            if (fuelAfterAdding_lbs >= 0.0)
            {
               // We were adding fuel, so add the negative fuelAfterAdding_lbs
               // to get the actual fuel transfered before running out
               fuelTransfer_lbs = aFuelAddRequest_lbs + fuelAfterAdding_lbs;

               if (fuelTransfer_lbs < 0.0)
               {
                  // Don't go negative, since we're trying to add fuel
                  fuelTransfer_lbs = 0.0;
               }
            }
            else
            {
               // We were giving fuel, so subtract the negative fuelAfterAdding_lbs
               fuelTransfer_lbs = aFuelAddRequest_lbs - fuelAfterAdding_lbs;

               if (fuelTransfer_lbs > 0.0)
               {
                  // Don't go positive, since we're trying to give fuel
                  fuelTransfer_lbs = 0.0;
               }
            }
         }
      }
      else
      {
         // Too much fuel

         // Determine how much is excess
         double excessFuel_lbs = fuelAfterAdding_lbs - mMaxQuantity_lbs;

         // We can only accept a limited amount of fuel
         fuelTransfer_lbs = aFuelAddRequest_lbs - excessFuel_lbs;
      }
   }
   else
   {
      // Within flow rate limit, now check for limit by max/min quantity
      double fuelAfterAdding_lbs = mCurrentQuantity_lbs + aFuelAddRequest_lbs;
      if (fuelAfterAdding_lbs <= mMaxQuantity_lbs)
      {
         // Within max limit, check min limit
         if (fuelAfterAdding_lbs > 0.0)
         {
            // Still have fuel, no additional change needed

            // We can take on the full amount of fuel
            fullFuelRequestAchieved = true;
            fuelTransfer_lbs        = aFuelAddRequest_lbs;
         }
         else
         {
            // Ran out of fuel -- check for positive or negative transfer
            if (aFuelAddRequest_lbs >= 0.0)
            {
               // We were adding fuel, so add the negative fuelAfterAdding_lbs
               // to get the actual fuel transfered before running out
               fuelTransfer_lbs = aFuelAddRequest_lbs + fuelAfterAdding_lbs;

               if (fuelTransfer_lbs < 0.0)
               {
                  // Don't go negative, since we're trying to add fuel
                  fuelTransfer_lbs = 0.0;
               }
            }
            else
            {
               // We were giving all remaining fuel, so subtract the negative mCurrentQuantity_lbs
               // This makes sure it is exactly 0.0 fuel remaining
               fuelTransfer_lbs = -mCurrentQuantity_lbs;
            }
         }
      }
      else
      {
         // Too much fuel

         // Determine how much is excess
         double excessFuel_lbs = fuelAfterAdding_lbs - mMaxQuantity_lbs;

         // We can only accept a limited amount of fuel
         fuelTransfer_lbs = aFuelAddRequest_lbs - excessFuel_lbs;
      }
   }

   // Return calculated values
   aFuelActuallyProvided_lbs = fuelTransfer_lbs;
   aNewFuelMass_lbs          = mCurrentQuantity_lbs + fuelTransfer_lbs;
   aCgLocation_ft            = CalcCgLocation_ft(aNewFuelMass_lbs);

   return fullFuelRequestAchieved;
}


bool P6DofFuelTank::UpdateFuelTransfer(double    aDeltaT_sec,
                                       double    aFuelAddRequest_lbs,
                                       double&   aFuelActuallyProvided_lbs,
                                       double&   aNewFuelMass_lbs,
                                       UtVec3dX& aCgLocation_ft)
{
   // Ensure we never divide by 0
   if (aDeltaT_sec < P6DofUtils::cEPSILON_SIMTIME_SEC)
   {
      // A negligible fuel request should be treated as fulfilled;
      // nothing was asked for and nothing was given

      aFuelActuallyProvided_lbs = 0.0;
      aNewFuelMass_lbs          = mCurrentQuantity_lbs;
      aCgLocation_ft            = mCurrentCgLocation_ft;
      return true;
   }

   bool ableToProvideFuel =
      CalculateFuelTransfer(aDeltaT_sec, aFuelAddRequest_lbs, aFuelActuallyProvided_lbs, aNewFuelMass_lbs, aCgLocation_ft);

   // Change fill rate state (cumulative during time step)
   mTempCurrentTransferRate_pps += aFuelActuallyProvided_lbs / aDeltaT_sec;

   // Change mass state
   mCurrentQuantity_lbs = aNewFuelMass_lbs;
   if (mCurrentQuantity_lbs > mMaxQuantity_lbs)
   {
      mCurrentQuantity_lbs = mMaxQuantity_lbs;
   }
   if (mCurrentQuantity_lbs < 0.0)
   {
      mCurrentQuantity_lbs = 0.0;
   }

   // Change cg location state
   mCurrentCgLocation_ft = aCgLocation_ft;

   return ableToProvideFuel;
}

void P6DofFuelTank::SetMaxFuelQuantity(double aFuelQuantity_lbs)
{
   mMaxQuantity_lbs = aFuelQuantity_lbs;
}

double P6DofFuelTank::GetFuelCapacity_lbs() const
{
   return mMaxQuantity_lbs;
}

double P6DofFuelTank::GetPercentFull() const
{
   if (mMaxQuantity_lbs > 0.0)
   {
      return 100.0 * (mCurrentQuantity_lbs / mMaxQuantity_lbs);
   }

   return 0.0;
}

void P6DofFuelTank::SetCurrentFuelQuantity(double aFuelQuantity_lbs)
{
   mCurrentQuantity_lbs = aFuelQuantity_lbs;
}

double P6DofFuelTank::GetCurrentFuelQuantity_lbs() const
{
   return mCurrentQuantity_lbs;
}

double P6DofFuelTank::GetFuelFraction() const
{
   return mCurrentQuantity_lbs / mMaxQuantity_lbs;
}

double P6DofFuelTank::GetFuelFlow_pps() const
{
   return mCurrentFuelFlow_pps;
}

double P6DofFuelTank::GetFuelFlow_pph() const
{
   return mCurrentFuelFlow_pps * 3600.0;
}

double P6DofFuelTank::GetFuelFillRate_pps() const
{
   return mCurrentFillRate_pps;
}

double P6DofFuelTank::GetFuelFillRate_pph() const
{
   return mCurrentFillRate_pps * 3600.0;
}

double P6DofFuelTank::GetMaxFuelTransferRate_pps() const
{
   return mMaxTransferRate_pps;
}

double P6DofFuelTank::GetFuelTransferRate_pps() const
{
   return mCurrentTransferRate_pps;
}

double P6DofFuelTank::GetFuelTransferRate_pph() const
{
   return mCurrentTransferRate_pps * 3600.0;
}

void P6DofFuelTank::SetFullCgLocation_ft(UtVec3dX& aCgLocation_ft)
{
   mFullCgLocation_ft = aCgLocation_ft;

   mCgEmptyToFullVector = mFullCgLocation_ft - mEmptyCgLocation_ft;
}

void P6DofFuelTank::SetEmptyCgLocation_ft(UtVec3dX& aCgLocation_ft)
{
   mEmptyCgLocation_ft = aCgLocation_ft;

   mCgEmptyToFullVector = mFullCgLocation_ft - mEmptyCgLocation_ft;
}

UtVec3dX P6DofFuelTank::GetCurrentCgLocation_ft() const
{
   return mCurrentCgLocation_ft;
}

UtVec3dX P6DofFuelTank::GetFullCgLocation_ft() const
{
   return mFullCgLocation_ft;
}

UtVec3dX P6DofFuelTank::GetEmptyCgLocation_ft() const
{
   return mEmptyCgLocation_ft;
}

UtVec3dX P6DofFuelTank::CalcCgLocation_ft(double aFuelQuantity_lbs)
{
   double fraction = aFuelQuantity_lbs / mMaxQuantity_lbs;

   UtVec3dX scaledVec = fraction * mCgEmptyToFullVector;

   return mEmptyCgLocation_ft + scaledVec;
}

void P6DofFuelTank::CalculateCurrentMassProperties()
{
   // Clear the mass properties to zero
   mMassProperties.ClearData();

   // Calc the current fuel position
   UtVec3dX fuelPos_ft = CalcCgLocation_ft(mCurrentQuantity_lbs);

   // Add the fuel mass at the fuel location
   mMassProperties.AddMassAtLocation(mCurrentQuantity_lbs, fuelPos_ft);
}
