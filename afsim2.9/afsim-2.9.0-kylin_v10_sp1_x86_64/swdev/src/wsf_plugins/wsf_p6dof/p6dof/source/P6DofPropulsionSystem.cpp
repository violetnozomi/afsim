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

#include "P6DofPropulsionSystem.hpp"

#include "P6DofFlightControlSystem.hpp"
#include "P6DofForceAndMomentsObject.hpp"
#include "P6DofFreezeFlags.hpp"
#include "P6DofFuelTank.hpp"
#include "P6DofJetEngineObject.hpp"
#include "P6DofRamjetEngineObject.hpp"
#include "P6DofRocketLiquidPropellantObject.hpp"
#include "P6DofRocketSolidPropellantObject.hpp"
#include "P6DofScenario.hpp"
#include "P6DofTypeManager.hpp"
#include "P6DofUtils.hpp"
#include "P6DofVehicle.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

P6DofPropulsionSystem::P6DofPropulsionSystem(P6DofScenario* aScenario)
   : P6DofObject(aScenario)
   , mPropulsionSystemThrottleLeverPosition(0)
   , mLastSimTime_nanosec(0)
   , mThrottleSettingMIL(nullptr)
   , mThrottleSettingAB(nullptr)
   , mThrottleSettingReverser(nullptr)
   , mThrottleSettingYaw(nullptr)
   , mThrottleSettingPitch(nullptr)
   , mThrottleLeverPositionSet(false)
   , mThrustReverserSet(false)
   , mThrustVectorYawSet(false)
   , mThrustVectorPitchSet(false)
   , mThrustVectoringAngleYaw_deg(0)
   , mThrustVectoringAnglePitch_deg(0)
   , mEnableThrustVectoring(false)
   , mThrustReserverSetting(0)
   , mEnableThrustReverser(false)
   , mMassProperties()
{
}

P6DofPropulsionSystem::~P6DofPropulsionSystem()
{
   for (auto& i : mEngineList)
   {
      delete i;
   }
   for (auto& i : mEngineTypeList)
   {
      delete i;
   }
   for (auto& i : mFuelTankMap)
   {
      delete i.second;
   }
   for (auto& i : mFuelTransferList)
   {
      delete i;
   }
   for (auto& i : mThrustControlInputValueDataList)
   {
      delete i;
   }

   // We do not need perform a specific delete on mThrottleSettingMIL, mThrottleSettingAB,
   // mThrottleSettingReverser, mThrottleSettingYaw, and mThrottleSettingPitch, since
   // they are contained in mThrustControlInputValueDataList, which has already deleted
   // its elements above.
}

P6DofPropulsionSystem* P6DofPropulsionSystem::Clone() const
{
   return new P6DofPropulsionSystem(*this);
}

void P6DofPropulsionSystem::SetParentVehicle(P6DofVehicle* aParentVehicle)
{
   P6DofObject::SetParentVehicle(aParentVehicle);

   for (auto& i : mFuelTankMap)
   {
      i.second->SetParentVehicle(aParentVehicle);
   }

   for (auto& i : mEngineList)
   {
      i->SetParentVehicle(aParentVehicle);
   }
}

P6DofPropulsionSystem::P6DofPropulsionSystem(const P6DofPropulsionSystem& aSrc)
   : P6DofObject(aSrc)
   , mPropulsionSystemThrottleLeverPosition(aSrc.mPropulsionSystemThrottleLeverPosition)
   , mLastSimTime_nanosec(aSrc.mLastSimTime_nanosec)
   , mThrottleSettingMIL(nullptr)      // handled in body
   , mThrottleSettingAB(nullptr)       // handled in body
   , mThrottleSettingReverser(nullptr) // handled in body
   , mThrottleSettingYaw(nullptr)      // handled in body
   , mThrottleSettingPitch(nullptr)    // handled in body
   , mThrottleLeverPositionSet(aSrc.mThrottleLeverPositionSet)
   , mThrustReverserSet(aSrc.mThrustReverserSet)
   , mThrustVectorYawSet(aSrc.mThrustVectorYawSet)
   , mThrustVectorPitchSet(aSrc.mThrustVectorPitchSet)
   , mThrustVectoringAngleYaw_deg(aSrc.mThrustVectoringAngleYaw_deg)
   , mThrustVectoringAnglePitch_deg(aSrc.mThrustVectoringAnglePitch_deg)
   , mEnableThrustVectoring(aSrc.mEnableThrustVectoring)
   , mThrustReserverSetting(aSrc.mThrustReserverSetting)
   , mEnableThrustReverser(aSrc.mEnableThrustReverser)
   , mMassProperties(aSrc.mMassProperties)
{
   // Note: we only deal with engine instances (mEngineList) in Initialize
   // Right here, we deal with engine types (mEngineTypeList)
   for (auto& engineType : aSrc.mEngineTypeList)
   {
      mEngineTypeList.emplace_back(new ThrustProducerTypeInfo(*engineType));
   }

   for (auto& fuelTank : aSrc.mFuelTankMap)
   {
      P6DofFuelTank* newTank = fuelTank.second->Clone();
      newTank->SetPropulsionSystem(this);
      newTank->SetParentVehicle(mParentVehicle);
      mFuelTankMap[newTank->GetName()] = newTank;
   }

   for (auto& fuelTransfer : aSrc.mFuelTransferList)
   {
      FuelTransfer* fuelXfer = new FuelTransfer(*fuelTransfer);
      // Ensure null pointers
      fuelXfer->sourceTank = nullptr;
      fuelXfer->targetTank = nullptr;
      mFuelTransferList.push_back(fuelXfer);
   }

   for (auto& controlInput : aSrc.mThrustControlInputValueDataList)
   {
      P6DofThrustProducerObject::ThrustControlInputValueData* item =
         new P6DofThrustProducerObject::ThrustControlInputValueData(*controlInput);

      if (aSrc.mThrottleSettingMIL == controlInput)
      {
         mThrottleSettingMIL = item;
      }
      if (aSrc.mThrottleSettingAB == controlInput)
      {
         mThrottleSettingAB = item;
      }
      if (aSrc.mThrottleSettingReverser == controlInput)
      {
         mThrottleSettingReverser = item;
      }
      if (aSrc.mThrottleSettingYaw == controlInput)
      {
         mThrottleSettingYaw = item;
      }
      if (aSrc.mThrottleSettingPitch == controlInput)
      {
         mThrottleSettingPitch = item;
      }

      mThrustControlInputValueDataList.push_back(item);
   }
}

bool P6DofPropulsionSystem::Initialize(int64_t aSimTime_nanosec)
{
   if (mParentVehicle == nullptr)
   {
      ut::log::error() << "Null Parent Vehicle in P6DofPropulsionSystem::Initialize().";
      return false;
   }

   // Set last time
   mLastSimTime_nanosec = aSimTime_nanosec;

   // Create engine instances from engine type list
   for (const auto& iter : mEngineTypeList)
   {
      P6DofThrustProducerObject* engineType = GetScenario()->GetTypeManager()->GetThrustProducerObjectType(iter->typeName);
      if (engineType == nullptr)
      {
         auto out = ut::log::error() << "Engine type not found in P6DofPropulsionSystem::Initialize().";
         out.AddNote() << "Engine Type: " << iter->typeName;
         return false;
      }

      P6DofThrustProducerObject* newEngine = nullptr;
      if (iter->baseTypeName == "Jet")
      {
         P6DofJetEngineObject* jetEngine = new P6DofJetEngineObject(GetScenario());
         jetEngine->DeriveFrom(dynamic_cast<P6DofJetEngineObject*>(engineType));
         jetEngine->SetParentPropulsionSystem(this);
         jetEngine->SetFuelTank(iter->fuelFeedName);
         newEngine = jetEngine;
      }
      else if (iter->baseTypeName == "SolidRocket")
      {
         P6DofRocketSolidPropellantObject* solidRocket = new P6DofRocketSolidPropellantObject(GetScenario());
         solidRocket->DeriveFrom(dynamic_cast<P6DofRocketSolidPropellantObject*>(engineType));
         solidRocket->SetParentPropulsionSystem(this);
         newEngine = solidRocket;
      }
      else if (iter->baseTypeName == "LiquidRocket")
      {
         P6DofRocketLiquidPropellantObject* liquidRocket = new P6DofRocketLiquidPropellantObject(GetScenario());
         liquidRocket->DeriveFrom(dynamic_cast<P6DofRocketLiquidPropellantObject*>(engineType));
         liquidRocket->SetParentPropulsionSystem(this);
         liquidRocket->SetFuelTank(iter->fuelFeedName);
         newEngine = liquidRocket;
      }
      else if (iter->baseTypeName == "Ramjet")
      {
         P6DofRamjetEngineObject* ramjet = new P6DofRamjetEngineObject(GetScenario());
         ramjet->DeriveFrom(dynamic_cast<P6DofRamjetEngineObject*>(engineType));
         ramjet->SetParentPropulsionSystem(this);
         ramjet->SetFuelTank(iter->fuelFeedName);
         newEngine = ramjet;
      }
      else
      {
         auto out = ut::log::error() << "Engine type not implemented in P6DofPropulsionSystem::Initialize().";
         out.AddNote() << "Engine Type: " << iter->typeName;
         return false;
      }

      newEngine->SetName(iter->instanceName);
      newEngine->SetThrustReferenceOffsetAttitude(iter->relPos, iter->relYPR);
      // Add the engine to the list
      mEngineList.push_back(newEngine);
   }

   // Note: The fuel tank names referenced in engines as the "FuelFeed" should already have the
   // proper name, so there is no need to "adjust" the fuel feel names

   // Note: Fuel tank names were adjusted during the "derive" step of creating a P6DofVehicle instance,
   // so there is no need to change them now (they will have already been adjusted at this point)

   // Note: As this point, fuel transfer names should be unique and the fuel tank names in
   // fuel transfers should already have the proper fuel tank names (no need to do an "adjust").
   // However, there is a need establish the pointers to the tanks and we do that now.

   // Loop through fuel transfers
   for (auto& fuelTransfer : mFuelTransferList)
   {
      std::string    srcTankName = fuelTransfer->sourceTankName;
      P6DofFuelTank* tank        = mParentVehicle->GetFuelTankByName(srcTankName);
      if (tank != nullptr)
      {
         // Set the pointer to the source tank
         fuelTransfer->sourceTank = tank;
      }
      else
      {
         auto out =
            ut::log::error() << "Unable to find source fuel tank for transfer in P6DofPropulsionSystem::Initialize().";
         out.AddNote() << "Fuel Tank: " << srcTankName;
         out.AddNote() << "Transfer: " << fuelTransfer->xferName;
      }

      std::string tgtTankName = fuelTransfer->targetTankName;
      tank                    = mParentVehicle->GetFuelTankByName(tgtTankName);
      if (tank != nullptr)
      {
         // Set the pointer to the target tank
         fuelTransfer->targetTank = tank;
      }
      else
      {
         auto out =
            ut::log::error() << "Unable to find target fuel tank for transfer in P6DofPropulsionSystem::Initialize().";
         out.AddNote() << "Fuel Tank: " << tgtTankName;
         out.AddNote() << "Transfer: " << fuelTransfer->xferName;
      }
   }

   // Now, initialize the various items

   // Initialize fuel tanks
   for (auto& tank : mFuelTankMap)
   {
      tank.second->Initialize(aSimTime_nanosec);
   }

   // Initialize engine(s)
   for (auto& engine : mEngineList)
   {
      engine->Initialize(aSimTime_nanosec);
   }

   // Note that we do not "initialize" the fuel transfers (no need to do so).

   CalculateCurrentMassProperties();

   return true;
}

bool P6DofPropulsionSystem::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "propulsion_data")
   {
      UtInputBlock block(aInput, "end_propulsion_data");
      while (block.ReadCommand())
      {
         std::string blockCommand;
         blockCommand = block.GetCommand();

         if (blockCommand == "engine")
         {
            ThrustProducerTypeInfo* info = new ThrustProducerTypeInfo;

            // Initialize to zero
            info->relPos.Set(0, 0, 0);
            info->offset_ft = 0;
            info->relYPR.Set(0, 0, 0);

            std::string instanceName;
            std::string engineType;
            aInput.ReadValue(instanceName);
            aInput.ReadValue(engineType);

            // Confirm that the type exists
            if (!GetScenario()->GetTypeManager()->ThrustProducerObjectTypeExists(engineType))
            {
               // Unknown type -- error since we can't derive from an unknown type
               { // RAII Block
                  auto out = ut::log::error() << "P6DofPropulsionSystem is trying to derive an engine from an unknown "
                                                 "type within P6DofPropulsionSystem::ProcessInput().";
                  out.AddNote() << "Base Type: " << engineType;
                  out.AddNote() << "Location: " << aInput.GetLocation();
               }
               delete info;
               throw UtInput::UnknownCommand(aInput);
            }

            info->baseTypeName = GetScenario()->GetTypeManager()->GetEngineBaseType(engineType);

            // Set subBlock to look for proper termination string
            UtInputBlock subBlock(aInput, "end_engine");

            info->typeName     = engineType;
            info->instanceName = instanceName;

            while (subBlock.ReadCommand())
            {
               std::string localCommand;
               localCommand = subBlock.GetCommand();

               if (localCommand == "fuel_feed")
               {
                  std::string str;
                  aInput.ReadValue(str);
                  info->fuelFeedName = str;
               }
               else if (localCommand == "rel_pos_x")
               {
                  double value;
                  aInput.ReadValueOfType(value, UtInput::cLENGTH);

                  // RelPos is in feet
                  double x, y, z;
                  info->relPos.Get(x, y, z);
                  x = value * UtMath::cFT_PER_M;
                  info->relPos.Set(x, y, z);
               }
               else if (localCommand == "rel_pos_y")
               {
                  double value;
                  aInput.ReadValueOfType(value, UtInput::cLENGTH);

                  // RelPos is in feet
                  double x, y, z;
                  info->relPos.Get(x, y, z);
                  y = value * UtMath::cFT_PER_M;
                  info->relPos.Set(x, y, z);
               }
               else if (localCommand == "rel_pos_z")
               {
                  double value;
                  aInput.ReadValueOfType(value, UtInput::cLENGTH);

                  // RelPos is in feet
                  double x, y, z;
                  info->relPos.Get(x, y, z);
                  z = value * UtMath::cFT_PER_M;
                  info->relPos.Set(x, y, z);
               }
               else if (localCommand == "rel_yaw")
               {
                  double value;
                  aInput.ReadValueOfType(value, UtInput::cANGLE);

                  // RelYPR is in radians
                  double y, p, r;
                  info->relYPR.Get(y, p, r);
                  // y = value * UtMath::cRAD_PER_DEG;
                  y = value;
                  info->relYPR.Set(y, p, r);
               }
               else if (localCommand == "rel_pitch")
               {
                  double value;
                  aInput.ReadValueOfType(value, UtInput::cANGLE);

                  // RelYPR is in radians
                  double y, p, r;
                  info->relYPR.Get(y, p, r);
                  // p = value * UtMath::cRAD_PER_DEG;
                  p = value;
                  info->relYPR.Set(y, p, r);
               }
               else if (localCommand == "rel_roll")
               {
                  double value;
                  aInput.ReadValueOfType(value, UtInput::cANGLE);

                  // RelYPR is in radians
                  double y, p, r;
                  info->relYPR.Get(y, p, r);
                  // r = value * UtMath::cRAD_PER_DEG;
                  r = value;
                  info->relYPR.Set(y, p, r);
               }
               else
               {
                  { // RAII Block
                     auto out = ut::log::error()
                                << "Unrecognized command within P6DofPropulsionSystem::ProcessInput().";
                     out.AddNote() << "Command: " << localCommand;
                     out.AddNote() << "Location: " << aInput.GetLocation();
                  }
                  delete info;
                  throw UtInput::UnknownCommand(aInput);
               }
            }

            // Add it to the list
            mEngineTypeList.push_back(info);
         }
         else if (blockCommand == "fuel_tank")
         {
            P6DofFuelTank* tempFuelTank = new P6DofFuelTank(GetScenario());
            tempFuelTank->ProcessInput(aInput);
            tempFuelTank->SetPropulsionSystem(this);
            tempFuelTank->SetParentVehicle(mParentVehicle);
            mFuelTankMap[tempFuelTank->GetName()] = tempFuelTank;
         }
         else if (blockCommand == "fuel_transfer")
         {
            std::string xferName;
            aInput.ReadValue(xferName);

            if (!FuelTransferNameUnique(xferName))
            {
               { // RAII Block
                  auto out = ut::log::error() << "P6DofPropulsionSystem is trying to setup a fuel transfer which is "
                                                 "not uniquely named within P6DofPropulsionSystem::ProcessInput().";
                  out.AddNote() << "Transfer: " << xferName;
                  out.AddNote() << "Location: " << aInput.GetLocation();
               }
               throw UtInput::UnknownCommand(aInput);
            }

            UtInputBlock subBlock(aInput);

            FuelTransfer* tempXfer = new FuelTransfer;
            // Ensure null pointers
            tempXfer->sourceTank = nullptr;
            tempXfer->targetTank = nullptr;
            tempXfer->xferName   = xferName;

            while (subBlock.ReadCommand())
            {
               std::string localCommand;
               localCommand = subBlock.GetCommand();

               if (localCommand == "source_tank")
               {
                  std::string str;
                  aInput.ReadValue(str);
                  tempXfer->sourceTankName = str;
               }
               else if (localCommand == "target_tank")
               {
                  std::string str;
                  aInput.ReadValue(str);
                  tempXfer->targetTankName = str;
               }
               else
               {
                  delete tempXfer;
                  throw UtInput::UnknownCommand(aInput);
               }
            }

            if (!tempXfer->sourceTankName.empty() && !tempXfer->targetTankName.empty())
            {
               mFuelTransferList.push_back(tempXfer);
            }
         }
         else if (blockCommand == "throttle_setting_mil")
         {
            std::string inputName;
            aInput.ReadValue(inputName);

            P6DofThrustProducerObject::ThrustControlInputValueData* item =
               new P6DofThrustProducerObject::ThrustControlInputValueData;
            item->handle = 0;
            item->name   = inputName;
            item->value  = 0;

            mThrottleSettingMIL = item;
            mThrustControlInputValueDataList.push_back(item);
         }
         else if (blockCommand == "throttle_setting_ab")
         {
            std::string inputName;
            aInput.ReadValue(inputName);

            P6DofThrustProducerObject::ThrustControlInputValueData* item =
               new P6DofThrustProducerObject::ThrustControlInputValueData;
            item->handle = 0;
            item->name   = inputName;
            item->value  = 0;

            mThrottleSettingAB = item;
            mThrustControlInputValueDataList.push_back(item);
         }
         else if (blockCommand == "throttle_setting_reverser")
         {
            std::string inputName;
            aInput.ReadValue(inputName);

            P6DofThrustProducerObject::ThrustControlInputValueData* item =
               new P6DofThrustProducerObject::ThrustControlInputValueData;
            item->handle = 0;
            item->name   = inputName;
            item->value  = 0;

            mThrottleSettingReverser = item;
            mThrustControlInputValueDataList.push_back(item);
         }
         else if (blockCommand == "throttle_setting_yaw")
         {
            std::string inputName;
            aInput.ReadValue(inputName);

            P6DofThrustProducerObject::ThrustControlInputValueData* item =
               new P6DofThrustProducerObject::ThrustControlInputValueData;
            item->handle = 0;
            item->name   = inputName;
            item->value  = 0;

            mThrottleSettingYaw = item;
            mThrustControlInputValueDataList.push_back(item);

            // Since this is capable of thrust vectoring (TV), enable TV by default
            mEnableThrustVectoring = true;
         }
         else if (blockCommand == "throttle_setting_pitch")
         {
            std::string inputName;
            aInput.ReadValue(inputName);

            P6DofThrustProducerObject::ThrustControlInputValueData* item =
               new P6DofThrustProducerObject::ThrustControlInputValueData;
            item->handle = 0;
            item->name   = inputName;
            item->value  = 0;

            mThrottleSettingPitch = item;
            mThrustControlInputValueDataList.push_back(item);

            // Since this is capable of thrust vectoring (TV), enable TV by default
            mEnableThrustVectoring = true;
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within P6DofPropulsionSystem::ProcessInput().";
            out.AddNote() << "Command: " << blockCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }

      // ******************** REMOVE FUEL TANK XFERS ******************
      // Loop through fuel tanks to set any cross feeds as needed
      // int numTanks = mFuelTankList.size();
      // for( int n = 0 ; n < numTanks ; ++n )
      // {
      //    FuelTank* tempFuelTank = mFuelTankList.at(n);
      //    std::string xFeedTankName = tempFuelTank->GetCurrentXFeedTankName();
      //    tempFuelTank->SetCurrentXFeedTankByName(xFeedTankName);
      // }
   }

   return myCommand;
}

void P6DofPropulsionSystem::RemoveInvalidFuelTransfers()
{
   // Remove 'bad' fuel transfers (due to a lack of connected 'path' from tank to
   // propulsion system. This will remove fuel transfers for fuel tanks that have
   // separated from the vehicle, for example).

   auto fuelXferIter = mFuelTransferList.begin();
   while (fuelXferIter != mFuelTransferList.end())
   {
      P6DofFuelTank* tgtTank = (*fuelXferIter)->targetTank;
      P6DofFuelTank* srcTank = (*fuelXferIter)->sourceTank;

      if ((tgtTank == nullptr) || (srcTank == nullptr) || (!tgtTank->FuelFlowPathIntact(this)) ||
          (!srcTank->FuelFlowPathIntact(this)))
      {
         // The current transfer is invalid (typically from jettisoning the tank), so remove it
         delete *fuelXferIter;
         fuelXferIter = mFuelTransferList.erase(fuelXferIter);
      }
      else
      {
         ++fuelXferIter;
      }
   }
}

void P6DofPropulsionSystem::Update(int64_t aSimTime_nanosec)
{
   // Calc delta-t
   int64_t dT_nanosec = aSimTime_nanosec - mLastSimTime_nanosec;

   if (dT_nanosec < 0)
   {
      // Negative time delta, so return
      return;
   }

   double dT_sec = P6DofUtils::TimeToTime(dT_nanosec);

   // Check for freeze
   if (GetScenario()->GetFreezeFlags()->GetMasterFreezeFuelBurn())
   {
      // Since we're frozen, we do not handle any fuel transfers.
      // Simply set the last time and return
      mLastSimTime_nanosec = aSimTime_nanosec;
      return;
   }

   std::vector<tankMatching> testList;

   RemoveInvalidFuelTransfers();

   for (auto& tankTransfer : mFuelTransferList)
   {
      P6DofFuelTank* tgtTank = tankTransfer->targetTank;
      P6DofFuelTank* srcTank = tankTransfer->sourceTank;

      // Is the target tank in the test list?
      bool inList = false;
      for (auto& tankMatch : testList)
      {
         if (tankMatch.targetAddTank == tgtTank)
         {
            // This is a match, so add as a source
            fuelTankData data;
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
         tankMatching newItem;
         newItem.targetAddTank = tgtTank;
         fuelTankData data;
         data.sourceTank               = srcTank;
         data.fuelActuallyProvided_lbs = 0.0;
         newItem.sourceTankList.push_back(data);
         testList.push_back(newItem);
      }
   }

   // Now, loop through testList by targetAddTank
   for (auto& xferItem : testList)
   {
      P6DofFuelTank* tgtTank = xferItem.targetAddTank;
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
            P6DofFuelTank* currentTank = tankData.sourceTank;

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
               P6DofFuelTank* currentTank = tankData.sourceTank;

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

void P6DofPropulsionSystem::CalculatePropulsionFM(double                      aDeltaT_sec,
                                                  double                      aAlt_ft,
                                                  double                      aDynPress_lbsqft,
                                                  double                      aStatPress_lbssqft,
                                                  double                      aSpeed_fps,
                                                  double                      aMach,
                                                  double                      aAlpha_rad,
                                                  double                      aBeta_rad,
                                                  P6DofForceAndMomentsObject& aForceAndMoment,
                                                  double&                     aFuelBurnRate_pps,
                                                  double&                     aFuelBurned_lbs)
{
   PropulsionCalculationsFM(aDeltaT_sec,
                            aAlt_ft,
                            aDynPress_lbsqft,
                            aStatPress_lbssqft,
                            aSpeed_fps,
                            aMach,
                            aAlpha_rad,
                            aBeta_rad,
                            aForceAndMoment,
                            aFuelBurnRate_pps,
                            aFuelBurned_lbs,
                            false);
}

void P6DofPropulsionSystem::UpdatePropulsionFM(double                      aDeltaT_sec,
                                               double                      aAlt_ft,
                                               double                      aDynPress_lbsqft,
                                               double                      aStatPress_lbssqft,
                                               double                      aSpeed_fps,
                                               double                      aMach,
                                               double                      aAlpha_rad,
                                               double                      aBeta_rad,
                                               P6DofForceAndMomentsObject& aForceAndMoment,
                                               double&                     aFuelBurnRate_pps,
                                               double&                     aFuelBurned_lbs)
{
   PropulsionCalculationsFM(aDeltaT_sec,
                            aAlt_ft,
                            aDynPress_lbsqft,
                            aStatPress_lbssqft,
                            aSpeed_fps,
                            aMach,
                            aAlpha_rad,
                            aBeta_rad,
                            aForceAndMoment,
                            aFuelBurnRate_pps,
                            aFuelBurned_lbs,
                            true);
}

void P6DofPropulsionSystem::PropulsionCalculationsFM(double                      aDeltaT_sec,
                                                     double                      aAlt_ft,
                                                     double                      aDynPress_lbsqft,
                                                     double                      aStatPress_lbssqft,
                                                     double                      aSpeed_fps,
                                                     double                      aMach,
                                                     double                      aAlpha_rad,
                                                     double                      aBeta_rad,
                                                     P6DofForceAndMomentsObject& aForceAndMoment,
                                                     double&                     aFuelBurnRate_pps,
                                                     double&                     aFuelBurned_lbs,
                                                     bool                        aUpdateData)
{
   // Zero values
   aForceAndMoment.ClearReferenceAndForceAndMoment();
   aFuelBurnRate_pps = 0.0;
   aFuelBurned_lbs   = 0.0;

   // If the propulsion system throttle was not set directly, check if special inputs
   // are being used, and if so, set engine throttles accordingly
   if (!mThrottleLeverPositionSet)
   {
      if (mThrottleSettingMIL != nullptr)
      {
         double specialThrottleLeverPosition = mThrottleSettingMIL->value;

         if (AfterburnerIsPresent())
         {
            if (mThrottleSettingAB != nullptr)
            {
               const double epsilonVal   = std::numeric_limits<double>::epsilon();
               double       unityEpsilon = 1.0 - epsilonVal;

               // Include AB throttle if we are essentially at full throttle MIL
               if (mThrottleSettingMIL->value > unityEpsilon)
               {
                  specialThrottleLeverPosition += mThrottleSettingAB->value;
               }
            }
         }

         for (auto& i : mEngineList)
         {
            i->SetThrottlePosition(specialThrottleLeverPosition);
         }
      }
   }

   // If the propulsion system mThrustReverserSet was not set directly, check if special inputs
   // are being used, and if so, set engine controls accordingly

   if (!mThrustReverserSet && mThrottleSettingReverser != nullptr)
   {
      if (mEnableThrustReverser)
      {
         // Set reversers to setting
         for (auto& engine : mEngineList)
         {
            engine->SetThrustReverserEnabled(true);
            engine->SetThrustReverserSetting(mThrottleSettingReverser->value);
         }
      }
      else
      {
         // Set reversers to zero
         for (auto& engine : mEngineList)
         {
            engine->SetThrustReverserEnabled(false);
            engine->SetThrustReverserSetting(0);
         }
      }
   }

   // If the propulsion system mThrustVectorYawSet was not set directly, check if special inputs
   // are being used, and if so, set engine controls accordingly
   if (!mThrustVectorYawSet && mThrottleSettingYaw != nullptr)
   {
      if (mEnableThrustVectoring)
      {
         // Set thrust vectoring settings
         for (auto& engine : mEngineList)
         {
            engine->SetThrustVectoringEnabled(true);
            engine->SetThrustVectoringYaw_deg(mThrottleSettingYaw->value);
         }
      }
      else
      {
         // Set thrust vectoring settings to zero
         for (auto& engine : mEngineList)
         {
            engine->SetThrustVectoringEnabled(false);
            engine->SetThrustVectoringYaw_deg(0);
            engine->SetThrustVectoringPitch_deg(0);
         }
      }
   }

   // If the propulsion system mThrustVectorYawSet was not set directly, check if special inputs
   // are being used, and if so, set engine controls accordingly
   if (!mThrustVectorPitchSet && mThrottleSettingPitch != nullptr)
   {
      if (mEnableThrustVectoring)
      {
         // Set reversers to setting
         for (auto& engine : mEngineList)
         {
            engine->SetThrustVectoringEnabled(true);
            engine->SetThrustVectoringPitch_deg(mThrottleSettingPitch->value);
         }
      }
      else
      {
         // Set reversers to zero
         for (auto& engine : mEngineList)
         {
            engine->SetThrustVectoringEnabled(false);
            engine->SetThrustVectoringYaw_deg(0);
            engine->SetThrustVectoringPitch_deg(0);
         }
      }
   }

   // Update/calculate thrust of engines
   for (auto& engine : mEngineList)
   {
      UtVec3dX                   tempForce_lbs(0., 0., 0.);
      UtVec3dX                   tempLocation_ft(0., 0., 0.);
      UtVec3dX                   tempMoment_ftlbs(0., 0., 0.);
      UtVec3dX                   aResultantThrustVector_lbs;
      UtVec3dX                   aResultantThrustMoment_ftlbs;
      P6DofForceAndMomentsObject tempThrustFM;
      double                     tempFuelBurnRate_pps = 0.0;
      double                     tempFuelBurned_lbs   = 0.0;

      if (aUpdateData)
      {
         engine->UpdateThrust(aDeltaT_sec,
                              aAlt_ft,
                              aDynPress_lbsqft,
                              aStatPress_lbssqft,
                              aSpeed_fps,
                              aMach,
                              aAlpha_rad,
                              aBeta_rad,
                              tempThrustFM,
                              tempFuelBurnRate_pps,
                              tempFuelBurned_lbs);
      }
      else
      {
         engine->CalculateThrust(aDeltaT_sec,
                                 aAlt_ft,
                                 aDynPress_lbsqft,
                                 aStatPress_lbssqft,
                                 aSpeed_fps,
                                 aMach,
                                 aAlpha_rad,
                                 aBeta_rad,
                                 tempThrustFM,
                                 tempFuelBurnRate_pps,
                                 tempFuelBurned_lbs);
      }

      // Add F&M
      aForceAndMoment += tempThrustFM;

      // Add fuel burn
      aFuelBurnRate_pps += tempFuelBurnRate_pps;
      aFuelBurned_lbs += tempFuelBurned_lbs;
   }
}

P6DofFuelTank* P6DofPropulsionSystem::GetFuelTank(const std::string& aName) const
{
   auto iter = mFuelTankMap.find(aName);
   if (iter != mFuelTankMap.end())
   {
      return iter->second;
   }
   return nullptr;
}

P6DofThrustProducerObject* P6DofPropulsionSystem::GetThrustProducerObjectByName(const std::string& aName) const
{
   for (auto& i : mEngineList)
   {
      if (i->GetName() == aName)
      {
         return i;
      }
   }
   return nullptr;
}

P6DofThrustProducerObject* P6DofPropulsionSystem::GetThrustProducerByIndex(size_t aIndex) const
{
   if (aIndex < mEngineList.size())
   {
      return mEngineList.at(aIndex);
   }
   return nullptr;
}

void P6DofPropulsionSystem::SetThrottleLeverPosition(double aThrottleLeverPosition)
{
   mPropulsionSystemThrottleLeverPosition = aThrottleLeverPosition;

   // Enforce throttle limits
   if (AfterburnerIsPresent())
   {
      P6DofUtils::LimitThrottleAfterburner(mPropulsionSystemThrottleLeverPosition);
   }
   else
   {
      P6DofUtils::LimitThrottleNormalized(mPropulsionSystemThrottleLeverPosition);
   }

   // Set flag
   mThrottleLeverPositionSet = true;

   // Inform the engines of the current throttle setting
   for (auto& engine : mEngineList)
   {
      engine->SetThrottlePosition(mPropulsionSystemThrottleLeverPosition);
   }
}

void P6DofPropulsionSystem::SetThrustVectoringYaw_deg(double aThrustVectoringAngle_deg)
{
   mThrustVectoringAngleYaw_deg = aThrustVectoringAngle_deg;
   mThrustVectorYawSet          = true;
   for (auto& engine : mEngineList)
   {
      engine->SetThrustVectoringYaw_deg(aThrustVectoringAngle_deg);
   }
}

void P6DofPropulsionSystem::SetThrustVectoringPitch_deg(double aThrustVectoringAngle_deg)
{
   mThrustVectoringAnglePitch_deg = aThrustVectoringAngle_deg;
   mThrustVectorPitchSet          = true;
   for (auto& engine : mEngineList)
   {
      engine->SetThrustVectoringPitch_deg(aThrustVectoringAngle_deg);
   }
}

void P6DofPropulsionSystem::EnableThrustVectoring(bool aEnableThrustVectoring)
{
   mEnableThrustVectoring = aEnableThrustVectoring;
   for (auto& engine : mEngineList)
   {
      engine->SetThrustVectoringEnabled(aEnableThrustVectoring);
   }
}

void P6DofPropulsionSystem::SetThrustReserver(double aThrustReserverSetting)
{
   mThrustReserverSetting = aThrustReserverSetting;
   mThrustReverserSet     = true;
   for (auto& engine : mEngineList)
   {
      engine->SetThrustReverserSetting(aThrustReserverSetting);
   }
}

void P6DofPropulsionSystem::EnableThrustReverser(bool aEnableThrustReverser)
{
   mEnableThrustReverser = aEnableThrustReverser;
   for (auto& engine : mEngineList)
   {
      engine->SetThrustReverserEnabled(aEnableThrustReverser);
   }
}

void P6DofPropulsionSystem::Ignite(int64_t aIgniteTimeInFrame_nanosec)
{
   mPropulsionSystemThrottleLeverPosition = 1.0;
   for (auto& i : mEngineList)
   {
      i->Ignite(aIgniteTimeInFrame_nanosec);
   }
}

void P6DofPropulsionSystem::Shutdown(int64_t aTerminateTime_nanosec)
{
   mPropulsionSystemThrottleLeverPosition = 0.0;
   for (auto& i : mEngineList)
   {
      i->Shutdown(aTerminateTime_nanosec);
   }
}

bool P6DofPropulsionSystem::FuelTransferNameUnique(const std::string& aName) const
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

bool P6DofPropulsionSystem::IsProducingThrust() const
{
   // Loop through engines
   for (auto& engine : mEngineList)
   {
      if (engine->GetForwardThrust_lbs() > 0)
      {
         return true;
      }
   }
   return false;
}

bool P6DofPropulsionSystem::SetFuelFeed(std::string& aEngineName, std::string aFuelTankName)
{
   // Apply to the matched engine
   for (auto& engine : mEngineList)
   {
      if (engine->GetName() == aEngineName)
      {
         // Match found. Select a fuel feed, if appropriate.
         if (engine->GetClassType() == "Jet")
         {
            P6DofJetEngineObject* jet = dynamic_cast<P6DofJetEngineObject*>(engine);
            jet->SetFuelTank(aFuelTankName);
            return true;
         }
         else if (engine->GetClassType() == "Ramjet")
         {
            P6DofRamjetEngineObject* ramjet = dynamic_cast<P6DofRamjetEngineObject*>(engine);
            ramjet->SetFuelTank(aFuelTankName);
            return true;
         }
         else if (engine->GetClassType() == "LiquidRocket")
         {
            P6DofRocketLiquidPropellantObject* rocket = dynamic_cast<P6DofRocketLiquidPropellantObject*>(engine);
            rocket->SetFuelTank(aFuelTankName);
            return true;
         }
      }
   }
   return false;
}

bool P6DofPropulsionSystem::SetFuelFeed(std::string aFuelTankName)
{
   bool result = false;
   // Apply to all engines
   for (auto& engine : mEngineList)
   {
      // Select a fuel feed, if appropriate
      if (engine->GetClassType() == "Jet")
      {
         P6DofJetEngineObject* jet = dynamic_cast<P6DofJetEngineObject*>(engine);
         jet->SetFuelTank(aFuelTankName);
         result = true;
      }
      else if (engine->GetClassType() == "Ramjet")
      {
         P6DofRamjetEngineObject* ramjet = dynamic_cast<P6DofRamjetEngineObject*>(engine);
         ramjet->SetFuelTank(aFuelTankName);
         result = true;
      }
      else if (engine->GetClassType() == "LiquidRocket")
      {
         P6DofRocketLiquidPropellantObject* rocket = dynamic_cast<P6DofRocketLiquidPropellantObject*>(engine);
         rocket->SetFuelTank(aFuelTankName);
         result = true;
      }
   }
   return result;
}

bool P6DofPropulsionSystem::RemoveFuelTankByName(std::string aName)
{
   auto iter = mFuelTankMap.find(aName);
   if (iter != mFuelTankMap.end())
   {
      delete iter->second;
      mFuelTankMap.erase(iter);
      return true;
   }
   return false;
}

bool P6DofPropulsionSystem::AddFuelTransfer(const std::string& aTransferName,
                                            const std::string& aSourceTankName,
                                            const std::string& aTargetTankName)
{
   // First, confirm name is unique
   if (!FuelTransferNameUnique(aTransferName))
   {
      auto out =
         ut::log::error()
         << "P6DofPropulsionSystem::AddFuelTransfer() is trying to setup a fuel transfer which is not uniquely named.";
      out.AddNote() << "Transfer: " << aTransferName;
      return false;
   }

   FuelTransfer* transfer = new FuelTransfer;
   // Ensure null pointers
   transfer->sourceTank     = nullptr;
   transfer->targetTank     = nullptr;
   transfer->xferName       = aTransferName;
   transfer->sourceTankName = aSourceTankName;
   transfer->targetTankName = aTargetTankName;

   mFuelTransferList.push_back(transfer);

   return true;
}

bool P6DofPropulsionSystem::AddFuelTransferAndConnect(const std::string& aTransferName,
                                                      const std::string& aSourceTankName,
                                                      const std::string& aTargetTankName)
{
   // First, confirm name is unique
   if (!FuelTransferNameUnique(aTransferName))
   {
      auto out = ut::log::error() << "P6DofPropulsionSystem::AddFuelTransferAndConnect() is trying to setup a fuel "
                                     "transfer which is not uniquely named.";
      out.AddNote() << "Transfer: " << aTransferName;
      return false;
   }

   FuelTransfer* transfer   = new FuelTransfer;
   transfer->sourceTank     = nullptr;
   transfer->targetTank     = nullptr;
   transfer->xferName       = aTransferName;
   transfer->sourceTankName = aSourceTankName;
   transfer->targetTankName = aTargetTankName;

   // Now, we need to "connect up" the tanks

   // Find the source tank to populate the pointer
   std::string    srcTankName = transfer->sourceTankName;
   P6DofFuelTank* tank        = mParentVehicle->GetFuelTankByName(srcTankName);
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
      delete transfer;
      return false;
   }

   mFuelTransferList.push_back(transfer);
   return true;
}

bool P6DofPropulsionSystem::RemoveFuelTransfer(const std::string& aTransferName)
{
   std::vector<FuelTransfer*>::iterator iter = mFuelTransferList.begin();
   while (iter != mFuelTransferList.end())
   {
      if ((*iter)->xferName == aTransferName)
      {
         delete *iter;
         mFuelTransferList.erase(iter);
         return true;
      }
      ++iter;
   }
   return false;
}

bool P6DofPropulsionSystem::ModifyFuelQuantity(const std::string& aTankName, double aQty_lbs)
{
   P6DofFuelTank* tank = GetFuelTank(aTankName);

   if (tank == nullptr)
   {
      return 0;
   }

   tank->SetCurrentFuelQuantity(aQty_lbs);

   return true;
}

size_t P6DofPropulsionSystem::GetNumThrustProducers() const
{
   return mEngineList.size();
}

size_t P6DofPropulsionSystem::GetNumFuelTanks() const
{
   return mFuelTankMap.size();
}

void P6DofPropulsionSystem::SetControlInputHandles(const P6DofFlightControlSystem* aFlightControlSystem)
{
   if (aFlightControlSystem)
   {
      for (auto& engine : mEngineList)
      {
         if (engine != nullptr) // Is it possible to have a null engine here?
         {
            engine->SetControlInputHandles(aFlightControlSystem);
         }
      }

      for (auto& controlInput : mThrustControlInputValueDataList)
      {
         controlInput->handle = aFlightControlSystem->GetControlValueHandle(controlInput->name);
         if (controlInput->handle == 0)
         {
            auto out = ut::log::warning() << "Unable to get a control value handle for input.";
            out.AddNote() << "Input: " << controlInput->name;
         }
      }
   }
}

void P6DofPropulsionSystem::SetControlInputValues(const P6DofFlightControlSystem* aFlightControlSystem)
{
   if (aFlightControlSystem)
   {
      for (auto& engine : mEngineList)
      {
         if (engine != nullptr) // Is it possible to have a null engine here?
         {
            engine->SetControlInputValues(aFlightControlSystem);
         }
      }

      // Now set data for the propulsions system itself
      for (auto& controlInput : mThrustControlInputValueDataList)
      {
         controlInput->value = aFlightControlSystem->GetControlValue(controlInput->handle);
      }
   }
}

double P6DofPropulsionSystem::GetInternalFuelTankCapacity_lbs() const
{
   double fuelQuantity_lbs = 0;
   for (auto& i : mFuelTankMap)
   {
      fuelQuantity_lbs += i.second->GetFuelCapacity_lbs();
   }

   return fuelQuantity_lbs;
}

double P6DofPropulsionSystem::GetCurrentInternalFuelTankQuantity_lbs() const
{
   double fuelQuantity_lbs = 0;
   for (auto& i : mFuelTankMap)
   {
      fuelQuantity_lbs += i.second->GetCurrentFuelQuantity_lbs();
   }

   return fuelQuantity_lbs;
}

double P6DofPropulsionSystem::AddFuelQuantity_lbs(double aFuelToAdd_lbs)
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

double P6DofPropulsionSystem::FillAllTanks(double aPercentFull)
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

void P6DofPropulsionSystem::EmptyAllTanks()
{
   for (auto& fueltank : mFuelTankMap)
   {
      fueltank.second->SetCurrentFuelQuantity(0.0);
   }
}

void P6DofPropulsionSystem::CalculateCurrentMassProperties()
{
   // Begin by setting the "current" values to the "base" values,
   // which are typically "zero"
   mMassProperties.SetCurrentDataToBaseData();

   // Next, add in fuel mass
   for (auto& i : mFuelTankMap)
   {
      P6DofFuelTank* fuelTank = i.second;
      if (fuelTank != nullptr)
      {
         fuelTank->CalculateCurrentMassProperties();
         mMassProperties += fuelTank->GetMassProperties();
      }
   }

   // Add in propellant mass from thrust producers (such as solid-propellant rockets)
   for (auto& enginePtr : mEngineList)
   {
      if (enginePtr != nullptr)
      {
         P6DofMassProperties* engineMassPtr = enginePtr->GetMassProperties();

         if (engineMassPtr != nullptr)
         {
            enginePtr->CalculateCurrentMassProperties();
            mMassProperties += *engineMassPtr;
         }
      }
   }
}

bool P6DofPropulsionSystem::AnEngineIsOperating(bool aTestSubobjects) const
{
   // Loop through engines. If any are operating, return true.
   for (auto& enginePtr : mEngineList)
   {
      if (enginePtr != nullptr)
      {
         if (enginePtr->EngineOperating())
         {
            return true;
         }
      }
   }

   // Test subobjects, if requested
   if (aTestSubobjects)
   {
      P6DofVehicle* vehicle = GetParentVehicle();
      if (vehicle)
      {
         auto& subObjectList = vehicle->GetSubobjectList();
         for (auto& subObjectPtr : subObjectList)
         {
            if (subObjectPtr != nullptr)
            {
               // Get the propulsion system
               P6DofPropulsionSystem* propulsion = subObjectPtr->GetPropulsionSystem();
               if (propulsion)
               {
                  if (propulsion->AnEngineIsOperating(aTestSubobjects))
                  {
                     return true;
                  }
               }
            }
         }
      }
   }

   return false;
}

bool P6DofPropulsionSystem::AnEngineIsSmoking(bool aTestSubobjects) const
{
   // Loop through engines. If any are smoking, return true.
   for (auto& enginePtr : mEngineList)
   {
      if (enginePtr != nullptr)
      {
         if (enginePtr->EngineSmoking())
         {
            return true;
         }
      }
   }

   // Test subobjects, if requested
   if (aTestSubobjects)
   {
      P6DofVehicle* vehicle = GetParentVehicle();
      if (vehicle)
      {
         auto& subObjectList = vehicle->GetSubobjectList();
         for (auto& subObjectPtr : subObjectList)
         {
            if (subObjectPtr != nullptr)
            {
               // Get the propulsion system
               P6DofPropulsionSystem* propulsion = subObjectPtr->GetPropulsionSystem();
               if (propulsion)
               {
                  if (propulsion->AnEngineIsSmoking(aTestSubobjects))
                  {
                     return true;
                  }
               }
            }
         }
      }
   }

   return false;
}

bool P6DofPropulsionSystem::AnEngineHasAfterburnerOn(bool aTestSubobjects) const
{
   // Loop through engines. If any have an afterburner on, return true.
   for (auto& enginePtr : mEngineList)
   {
      if (enginePtr != nullptr)
      {
         if (enginePtr->AfterburnerOn())
         {
            return true;
         }
      }
   }

   // Test subobjects, if requested
   if (aTestSubobjects)
   {
      P6DofVehicle* vehicle = GetParentVehicle();
      if (vehicle)
      {
         auto& subObjectList = vehicle->GetSubobjectList();
         for (auto& subObjectPtr : subObjectList)
         {
            if (subObjectPtr != nullptr)
            {
               // Get the propulsion system
               P6DofPropulsionSystem* propulsion = subObjectPtr->GetPropulsionSystem();
               if (propulsion)
               {
                  if (propulsion->AnEngineHasAfterburnerOn(aTestSubobjects))
                  {
                     return true;
                  }
               }
            }
         }
      }
   }

   return false;
}

bool P6DofPropulsionSystem::AnEngineIsContrailing(bool aTestSubobjects) const
{
   // Loop through engines. If any are contrailing, return true.
   for (auto& enginePtr : mEngineList)
   {
      if (enginePtr != nullptr)
      {
         if (enginePtr->Contrailing())
         {
            return true;
         }
      }
   }

   // Test subobjects, if requested
   if (aTestSubobjects)
   {
      P6DofVehicle* vehicle = GetParentVehicle();
      if (vehicle)
      {
         auto& subObjectList = vehicle->GetSubobjectList();
         for (auto& subObjectPtr : subObjectList)
         {
            if (subObjectPtr != nullptr)
            {
               // Get the propulsion system
               P6DofPropulsionSystem* propulsion = subObjectPtr->GetPropulsionSystem();
               if (propulsion)
               {
                  if (propulsion->AnEngineIsContrailing(aTestSubobjects))
                  {
                     return true;
                  }
               }
            }
         }
      }
   }

   return false;
}

bool P6DofPropulsionSystem::AnEngineIsEmittingSmokeTrail(bool aTestSubobjects) const
{
   // Loop through engines. If any are emitting a smoke trail, return true.
   for (auto& enginePtr : mEngineList)
   {
      if (enginePtr != nullptr)
      {
         if (enginePtr->ProducingSmokeTrail())
         {
            return true;
         }
      }
   }

   // Test subobjects, if requested
   if (aTestSubobjects)
   {
      P6DofVehicle* vehicle = GetParentVehicle();
      if (vehicle)
      {
         auto& subObjectList = vehicle->GetSubobjectList();
         for (auto& subObjectPtr : subObjectList)
         {
            if (subObjectPtr != nullptr)
            {
               // Get the propulsion system
               P6DofPropulsionSystem* propulsion = subObjectPtr->GetPropulsionSystem();
               if (propulsion)
               {
                  if (propulsion->AnEngineIsEmittingSmokeTrail(aTestSubobjects))
                  {
                     return true;
                  }
               }
            }
         }
      }
   }

   return false;
}

void P6DofPropulsionSystem::MakeAnEngineSmoke(int aEngineIndex)
{
   if (aEngineIndex == 0)
   {
      // Loop through all engines
      for (auto& enginePtr : mEngineList)
      {
         if (enginePtr != nullptr)
         {
            enginePtr->MakeEngineSmoke(true);
         }
      }
   }
   else
   {
      P6DofThrustProducerObject* enginePtr = GetThrustProducerByIndex(aEngineIndex);
      if (enginePtr)
      {
         enginePtr->MakeEngineSmoke(true);
      }
   }
}

bool P6DofPropulsionSystem::AfterburnerIsPresent() const
{
   for (auto& engine : mEngineList)
   {
      if (engine->AfterburnerIsPresent())
      {
         return true;
      }
   }

   return false;
}
