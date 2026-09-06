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

#include "WsfSixDOF_PropulsionSystem.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfSixDOF_Mover.hpp"

void wsf::six_dof::PropulsionSystem::SetParentVehicle(Mover* aParentVehicle)
{
   Object::SetParentVehicle(aParentVehicle);

   for (auto& i : mFuelTankMap)
   {
      i.second->SetParentVehicle(aParentVehicle);
   }
}

wsf::six_dof::PropulsionSystem::PropulsionSystem(const PropulsionSystem& aSrc)
   : Object(aSrc)
   , mLastSimTime_nanosec(aSrc.mLastSimTime_nanosec)
   , mPropulsionSystemThrottleLeverPosition(aSrc.mPropulsionSystemThrottleLeverPosition)
   , mThrottleLeverPositionSet(aSrc.mThrottleLeverPositionSet)
   , mEnableThrustVectoring(aSrc.mEnableThrustVectoring)
{
   for (auto& fuelTank : aSrc.mFuelTankMap)
   {
      AddFuelTank(std::unique_ptr<FuelTank>(fuelTank.second->Clone()));
   }

   for (auto& fuelTransfer : aSrc.mFuelTransferList)
   {
      auto fuelXfer = ut::make_unique<FuelTransfer>(*fuelTransfer);
      // Ensure null pointers
      fuelXfer->sourceTank = nullptr;
      fuelXfer->targetTank = nullptr;
      mFuelTransferList.push_back(std::move(fuelXfer));
   }
}

void wsf::six_dof::PropulsionSystem::RemoveInvalidFuelTransfers()
{
   // Remove 'bad' fuel transfers (due to a lack of connected 'path' from tank to
   // propulsion system. This will remove fuel transfers for fuel tanks that have
   // separated from the vehicle, for example).

   auto fuelXferIter = mFuelTransferList.begin();
   while (fuelXferIter != mFuelTransferList.end())
   {
      FuelTank* tgtTank = (*fuelXferIter)->targetTank;
      FuelTank* srcTank = (*fuelXferIter)->sourceTank;

      if ((tgtTank == nullptr) || (srcTank == nullptr) || (!tgtTank->FuelFlowPathIntact(this)) ||
          (!srcTank->FuelFlowPathIntact(this)))
      {
         // The current transfer is invalid (typically from jettisoning the tank), so remove it
         fuelXferIter = mFuelTransferList.erase(fuelXferIter);
      }
      else
      {
         ++fuelXferIter;
      }
   }
}

void wsf::six_dof::PropulsionSystem::Update(int64_t aSimTime_nanosec)
{
   // Calc delta-t
   int64_t dT_nanosec = aSimTime_nanosec - mLastSimTime_nanosec;

   if (dT_nanosec < 0)
   {
      // Negative time delta, so return
      return;
   }

   double dT_sec = utils::TimeToTime(dT_nanosec);

   // Check for freeze
   if (GetParentVehicle()->GetFreezeFlags()->fuelBurn)
   {
      // Since we're frozen, we do not handle any fuel transfers.
      // Simply set the last time and return
      mLastSimTime_nanosec = aSimTime_nanosec;
      return;
   }

   std::vector<TankMatching> testList;

   RemoveInvalidFuelTransfers();

   for (auto& tankTransfer : mFuelTransferList)
   {
      FuelTank* tgtTank = tankTransfer->targetTank;
      FuelTank* srcTank = tankTransfer->sourceTank;

      // Is the target tank in the test list?
      bool inList = false;
      for (auto& tankMatch : testList)
      {
         if (tankMatch.targetAddTank == tgtTank)
         {
            // This is a match, so add as a source
            FuelTankData data;
            data.sourceTank               = srcTank;
            data.fuelActuallyProvided_lbs = 0.0;
            tankMatch.sourceTankList.push_back(data);
            inList = true;
            break;
         }
      }

      // If not in the list, create it and add it to the test list
      if (!inList)
      {
         TankMatching newItem;
         newItem.targetAddTank = tgtTank;
         FuelTankData data;
         data.sourceTank               = srcTank;
         data.fuelActuallyProvided_lbs = 0.0;
         newItem.sourceTankList.push_back(data);
         testList.push_back(newItem);
      }
   }

   // Now, loop through testList by targetAddTank
   for (auto& xferItem : testList)
   {
      FuelTank* tgtTank = xferItem.targetAddTank;
      if (tgtTank != nullptr)
      {
         // First see how much the target can take
         double   maxTgtXfer_lbs           = tgtTank->GetMaxFuelTransferRate_pps() * dT_sec;
         double   fuelAddRequest_lbs       = maxTgtXfer_lbs * 2.0;
         double   fuelActuallyProvided_lbs = 0.0;
         double   newFuelMass_lbs          = 0.0;
         UtVec3dX cgLocation_ft(0., 0., 0.);

         // This function returns a flag that denotes if it can burn all fuel,
         // but we do not need it in this case -- just calculate
         tgtTank->CalculateFuelTransfer(dT_sec, fuelAddRequest_lbs, fuelActuallyProvided_lbs, newFuelMass_lbs, cgLocation_ft);

         // This is is maximum we could transfer, based on fuel transfer capabilities.
         // However, this may be reduced if the target tank is nearly full (since we can
         // only add fuel until it is full).
         maxTgtXfer_lbs = fuelActuallyProvided_lbs;

         double amountUntilFull_lbs = tgtTank->GetFuelCapacity_lbs() - tgtTank->GetCurrentFuelQuantity_lbs();

         if (maxTgtXfer_lbs < 0.0)
         {
            maxTgtXfer_lbs = 0.0;
         }
         else
         {
            if (amountUntilFull_lbs < maxTgtXfer_lbs)
            {
               maxTgtXfer_lbs = amountUntilFull_lbs;
            }
         }

         if (maxTgtXfer_lbs < 0.0)
         {
            maxTgtXfer_lbs = 0.0;
         }

         double totalXfer_lbs = 0.0;
         // Loop through sources and add up total amount to transfer (if possible)
         for (auto& tankData : xferItem.sourceTankList)
         {
            FuelTank* currentTank = tankData.sourceTank;

            fuelAddRequest_lbs       = currentTank->GetMaxFuelTransferRate_pps() * dT_sec;
            fuelActuallyProvided_lbs = 0.0;
            newFuelMass_lbs          = 0.0;
            cgLocation_ft.Set(0, 0, 0);

            // This function returns a flag that denotes if it can burn all fuel,
            // but we do not need it in this case -- just calculate
            currentTank->CalculateFuelTransfer(dT_sec,
                                               -fuelAddRequest_lbs,
                                               fuelActuallyProvided_lbs,
                                               newFuelMass_lbs,
                                               cgLocation_ft);

            tankData.fuelActuallyProvided_lbs = fuelActuallyProvided_lbs;

            totalXfer_lbs += fuelActuallyProvided_lbs;
         }

         double fraction = 0.0;
         if (fabs(maxTgtXfer_lbs) > DBL_EPSILON)
         {
            fraction = fabs(totalXfer_lbs) / fabs(maxTgtXfer_lbs);

            if (fraction <= 1.0)
            {
               fraction = 1.0;
            }
            else
            {
               fraction = 1.0 / fraction;
            }
         }
         else
         {
            // We cannot transfer to the target, so the fraction is zero
            fraction = 0.0;
         }

         if (fraction > DBL_EPSILON)
         {
            // Now, have the sources add fuel to the target using the fraction
            for (auto& tankData : xferItem.sourceTankList)
            {
               FuelTank* currentTank = tankData.sourceTank;

               fuelAddRequest_lbs       = -tankData.fuelActuallyProvided_lbs * fraction;
               fuelActuallyProvided_lbs = 0.0;
               newFuelMass_lbs          = 0.0;
               cgLocation_ft.Set(0, 0, 0);

               currentTank->UpdateFuelTransfer(dT_sec,
                                               -fuelAddRequest_lbs,
                                               fuelActuallyProvided_lbs,
                                               newFuelMass_lbs,
                                               cgLocation_ft);

               tgtTank->UpdateFuelTransfer(dT_sec, fuelAddRequest_lbs, fuelActuallyProvided_lbs, newFuelMass_lbs, cgLocation_ft);
            }
         }
      }
   }

   // Set the last time
   mLastSimTime_nanosec = aSimTime_nanosec;
}

wsf::six_dof::FuelTank* wsf::six_dof::PropulsionSystem::GetFuelTank(const std::string& aName) const
{
   auto iter = mFuelTankMap.find(aName);
   if (iter != mFuelTankMap.end())
   {
      return iter->second;
   }
   return nullptr;
}

bool wsf::six_dof::PropulsionSystem::FuelTransferNameUnique(const std::string& aName) const
{
   for (auto& fuelTransfer : mFuelTransferList)
   {
      if (fuelTransfer->xferName == aName)
      {
         return false;
      }
   }
   return true;
}

bool wsf::six_dof::PropulsionSystem::RemoveFuelTankByName(std::string aName)
{
   auto iter = mFuelTankMap.find(aName);
   if (iter != mFuelTankMap.end())
   {
      mFuelTankMap.erase(iter);
      return true;
   }
   return false;
}

bool wsf::six_dof::PropulsionSystem::AddFuelTank(std::unique_ptr<FuelTank> aFuelTank)
{
   aFuelTank->SetPropulsionSystem(this);
   aFuelTank->SetParentVehicle(GetParentVehicle());

   std::string name   = aFuelTank->GetName();
   mFuelTankMap[name] = std::move(aFuelTank);

   return true;
}

bool wsf::six_dof::PropulsionSystem::AddFuelTransfer(const std::string& aTransferName,
                                                     const std::string& aSourceTankName,
                                                     const std::string& aTargetTankName)
{
   // First, confirm name is unique
   if (!FuelTransferNameUnique(aTransferName))
   {
      auto out =
         ut::log::error()
         << "PropulsionSystem::AddFuelTransfer() is trying to setup a fuel transfer which is not uniquely named.";
      out.AddNote() << "Transfer: " << aTransferName;
      return false;
   }

   auto transfer = ut::make_unique<FuelTransfer>();
   // Ensure null pointers
   transfer->sourceTank     = nullptr;
   transfer->targetTank     = nullptr;
   transfer->xferName       = aTransferName;
   transfer->sourceTankName = aSourceTankName;
   transfer->targetTankName = aTargetTankName;

   mFuelTransferList.push_back(std::move(transfer));

   return true;
}

bool wsf::six_dof::PropulsionSystem::AddFuelTransferAndConnect(const std::string& aTransferName,
                                                               const std::string& aSourceTankName,
                                                               const std::string& aTargetTankName)
{
   // First, confirm name is unique
   if (!FuelTransferNameUnique(aTransferName))
   {
      auto out = ut::log::error() << "PropulsionSystem::AddFuelTransferAndConnect() is trying to setup a fuel transfer "
                                     "which is not uniquely named.";
      out.AddNote() << "Transfer: " << aTransferName;
      return false;
   }

   auto transfer            = ut::make_unique<FuelTransfer>();
   transfer->sourceTank     = nullptr;
   transfer->targetTank     = nullptr;
   transfer->xferName       = aTransferName;
   transfer->sourceTankName = aSourceTankName;
   transfer->targetTankName = aTargetTankName;

   // Now, we need to "connect up" the tanks

   // Find the source tank to populate the pointer
   std::string srcTankName = transfer->sourceTankName;
   FuelTank*   tank        = mParentVehicle->GetFuelTankByName(srcTankName);
   if (tank != nullptr)
   {
      // Set the pointer to the source tank
      transfer->sourceTank = tank;
   }
   else
   {
      auto out = ut::log::error() << "Unable to find source fuel tank for transfer.";
      out.AddNote() << "Fuel Tank: " << srcTankName;
      out.AddNote() << "Transfer: " << transfer->xferName;
   }

   // Find the target tank to populate the pointer
   std::string tgtTankName = transfer->targetTankName;
   tank                    = mParentVehicle->GetFuelTankByName(tgtTankName);
   if (tank != nullptr)
   {
      // Set the pointer to the target tank
      transfer->targetTank = tank;
   }
   else
   {
      auto out = ut::log::error() << "Unable to find target fuel tank for transfer.";
      out.AddNote() << "Fuel Tank: " << tgtTankName;
      out.AddNote() << "Transfer: " << transfer->xferName;
   }

   // Make sure everything is good
   if ((transfer->sourceTank == nullptr) || (transfer->targetTank == nullptr))
   {
      return false;
   }

   mFuelTransferList.push_back(std::move(transfer));
   return true;
}

bool wsf::six_dof::PropulsionSystem::RemoveFuelTransfer(const std::string& aTransferName)
{
   auto iter = mFuelTransferList.begin();
   while (iter != mFuelTransferList.end())
   {
      if ((*iter)->xferName == aTransferName)
      {
         mFuelTransferList.erase(iter);
         return true;
      }
      ++iter;
   }
   return false;
}

bool wsf::six_dof::PropulsionSystem::ModifyFuelQuantity(const std::string& aTankName, double aQty_lbs)
{
   FuelTank* tank = GetFuelTank(aTankName);

   if (tank == nullptr)
   {
      return 0;
   }

   tank->SetCurrentFuelQuantity(aQty_lbs);

   return true;
}

double wsf::six_dof::PropulsionSystem::GetInternalFuelTankCapacity_lbs() const
{
   double fuelQuantity_lbs = 0;
   for (auto& i : mFuelTankMap)
   {
      fuelQuantity_lbs += i.second->GetFuelCapacity_lbs();
   }

   return fuelQuantity_lbs;
}

double wsf::six_dof::PropulsionSystem::GetCurrentInternalFuelTankQuantity_lbs() const
{
   double fuelQuantity_lbs = 0;
   for (auto& i : mFuelTankMap)
   {
      fuelQuantity_lbs += i.second->GetCurrentFuelQuantity_lbs();
   }

   return fuelQuantity_lbs;
}

double wsf::six_dof::PropulsionSystem::AddFuelQuantity_lbs(double aFuelToAdd_lbs)
{
   double fuelQuantityAdded_lbs = 0;
   double maxFuel_lbs           = GetInternalFuelTankCapacity_lbs();
   double currentFuel_lbs       = GetCurrentInternalFuelTankQuantity_lbs();
   double maxPossibleAdded_lbs  = maxFuel_lbs - currentFuel_lbs;

   if (maxPossibleAdded_lbs <= aFuelToAdd_lbs)
   {
      // We can take on all of the supplied fuel (there may be extra fuel)

      // Top-off all tanks
      fuelQuantityAdded_lbs = FillAllTanks(100.0);
   }
   else
   {
      // There is not enough fuel to fill all tanks

      double fillPercentage = 100.0 * (currentFuel_lbs + aFuelToAdd_lbs) / maxFuel_lbs;

      if (fillPercentage < 0.0)
      {
         fillPercentage = 0.0;
      }
      if (fillPercentage > 100.0)
      {
         fillPercentage = 100.0;
      }

      // Fill tanks
      fuelQuantityAdded_lbs = FillAllTanks(fillPercentage);
   }

   return fuelQuantityAdded_lbs;
}

double wsf::six_dof::PropulsionSystem::FillAllTanks(double aPercentFull)
{
   double fuelQuantityAdded_lbs = 0;

   for (auto& fueltank : mFuelTankMap)
   {
      double currentFuelQuantity_lbs = fueltank.second->GetCurrentFuelQuantity_lbs();

      double desiredFuelQuantity_lbs = fueltank.second->GetFuelCapacity_lbs() * (aPercentFull / 100.0);

      double fuelToAdd_lbs = desiredFuelQuantity_lbs - currentFuelQuantity_lbs;

      fuelQuantityAdded_lbs += fuelToAdd_lbs;

      fueltank.second->SetCurrentFuelQuantity(desiredFuelQuantity_lbs);
   }

   return fuelQuantityAdded_lbs;
}

void wsf::six_dof::PropulsionSystem::EmptyAllTanks()
{
   for (auto& fueltank : mFuelTankMap)
   {
      fueltank.second->SetCurrentFuelQuantity(0.0);
   }
}

const wsf::six_dof::MassProperties wsf::six_dof::PropulsionSystem::GetMassProperties() const
{
   // Begin by setting the "current" values to the "base" values,
   // which are typically "zero"
   MassProperties massProperties;

   // Next, add in fuel mass
   for (auto& i : mFuelTankMap)
   {
      FuelTank* fuelTank = i.second;
      if (fuelTank != nullptr)
      {
         fuelTank->CalculateCurrentMassProperties();
         massProperties += fuelTank->GetMassProperties();
      }
   }
   return massProperties;
}

size_t wsf::six_dof::PropulsionSystem::GetNumFuelTanks() const
{
   return mFuelTankMap.size();
}
