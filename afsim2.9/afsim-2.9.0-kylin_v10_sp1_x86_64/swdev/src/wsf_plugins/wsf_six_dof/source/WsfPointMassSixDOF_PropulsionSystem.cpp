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

#include "WsfPointMassSixDOF_PropulsionSystem.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtMath.hpp"
#include "WsfPointMassSixDOF_FlightControlSystem.hpp"
#include "WsfPointMassSixDOF_Utils.hpp"
#include "WsfSixDOF_ForceAndRotationObject.hpp"
#include "WsfSixDOF_FuelTank.hpp"
#include "WsfSixDOF_JetEngine.hpp"
#include "WsfSixDOF_RamjetEngine.hpp"
#include "WsfSixDOF_RocketLiquidPropellantEngine.hpp"
#include "WsfSixDOF_RocketSolidPropellantEngine.hpp"
#include "WsfSixDOF_TypeManager.hpp"

wsf::six_dof::PointMassPropulsionSystem* wsf::six_dof::PointMassPropulsionSystem::Clone() const
{
   return new PointMassPropulsionSystem(*this);
}

void wsf::six_dof::PointMassPropulsionSystem::SetParentVehicle(Mover* aParentVehicle)
{
   PropulsionSystem::SetParentVehicle(aParentVehicle);

   for (auto& i : mEngineList)
   {
      i->SetParentVehicle(aParentVehicle);
   }
}

wsf::six_dof::PointMassPropulsionSystem::PointMassPropulsionSystem(const PointMassPropulsionSystem& aSrc)
   : PropulsionSystem(aSrc)
{
   mEngineList.clear();
   for (auto& i : aSrc.mEngineList)
   {
      auto thrustProducer = dynamic_cast<PointMassThrustProducerObject*>(i->Clone());
      if (thrustProducer)
      {
         thrustProducer->SetParentPropulsionSystem(this);

         const FuelTank* srcTank = i->GetEngine()->GetFuelTank();
         const FuelTank* tank    = thrustProducer->GetEngine()->GetFuelTank();
         if (srcTank && !tank)
         {
            thrustProducer->SetFuelTank(srcTank->GetName());
         }

         mEngineList.push_back(std::unique_ptr<PointMassThrustProducerObject>{thrustProducer});
      }
   }
}

bool wsf::six_dof::PointMassPropulsionSystem::Initialize(int64_t aSimTime_nanosec)
{
   if (mParentVehicle == nullptr)
   {
      ut::log::error() << "Null Parent Vehicle in PointMassPropulsionSystem::Initialize";
      return false;
   }

   // Set last time
   mLastSimTime_nanosec = aSimTime_nanosec;

   // Note: The fuel tank names referenced in engines as the "FuelFeed" should already have the
   // proper name, so there is no need to "adjust" the fuel feel names

   // Note: Fuel tank names were adjusted during the "derive" step of creating a PointMassSixDOF_Vehicle instance,
   // so there is no need to change them now (they will have already been adjusted at this point)

   // Note: As this point, fuel transfer names should be unique and the fuel tank names in
   // fuel transfers should already have the proper fuel tank names (no need to do an "adjust").
   // However, there is a need establish the pointers to the tanks and we do that now.

   // Loop through fuel transfers
   for (auto& fuelTransfer : mFuelTransferList)
   {
      std::string srcTankName = fuelTransfer->sourceTankName;
      FuelTank*   tank        = mParentVehicle->GetFuelTankByName(srcTankName);
      if (tank != nullptr)
      {
         // Set the pointer to the source tank
         fuelTransfer->sourceTank = tank;
      }
      else
      {
         auto out = ut::log::error()
                    << "Unable to find source fuel tank for transfer in PointMassPropulsionSystem::Initialize().";
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
         auto out = ut::log::error()
                    << "Unable to find target fuel tank for transfer in PointMassPropulsionSystem::Initialize().";
         out.AddNote() << "Fuel Tank: " << tgtTankName;
         out.AddNote() << "Transfer: " << fuelTransfer->xferName;
      }
   }

   // Now, initialize the various items

   // Initialize engine(s)
   for (auto& engine : mEngineList)
   {
      engine->Initialize(aSimTime_nanosec);
   }

   // Initialize fuel tanks
   for (auto& tank : mFuelTankMap)
   {
      tank.second->Initialize(aSimTime_nanosec);
   }

   // Note that we do not "initialize" the fuel transfers (no need to do so).

   return true;
}

void wsf::six_dof::PointMassPropulsionSystem::LinkControls(PointMassFlightControlSystem* aFlightControls)
{
   if (aFlightControls)
   {
      mThrottleSettingMilPtr = aFlightControls->GetThrottleMilPtr();
      mThrottleSettingAbPtr  = aFlightControls->GetThrottleAbPtr();
   }
}

bool wsf::six_dof::PointMassPropulsionSystem::ProcessInput(UtInput& aInput, const TypeManager* aTypeManager)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "propulsion_data")
   {
      // Using vector of pairs instead of std::map so we can move the ThrustProducerObject unique_ptr later
      std::vector<std::pair<std::unique_ptr<PointMassThrustProducerObject>, ThrustProducerInfo>> newThrustProducerMap;

      UtInputBlock block(aInput, "end_propulsion_data");
      while (block.ReadCommand())
      {
         std::string blockCommand;
         blockCommand = block.GetCommand();

         if (blockCommand == "engine")
         {
            std::string instanceName;
            std::string typeName;
            aInput.ReadValue(instanceName);
            aInput.ReadValue(typeName);

            // Confirm that the type exists
            const ThrustProducerObject* engineType = aTypeManager->GetThrustProducerObjectType(typeName);
            if (engineType == nullptr)
            {
               {
                  // RAII Block
                  auto out = ut::log::error() << "Engine type not found in PointMassPropulsionSystem::ProcessInput().";
                  out.AddNote() << "Engine Type: " << typeName;
                  out.AddNote() << "Location: " << aInput.GetLocation();
               }
               throw UtInput::UnknownCommand(aInput);
            }

            std::unique_ptr<PointMassThrustProducerObject> pointMassThrustProducer;

            // Type-check, then build unique_ptr from type
            if (dynamic_cast<const PointMassThrustProducerObject*>(engineType))
            {
               auto thrustProducer             = engineType->Clone();
               auto pointMassThrustProducerRaw = dynamic_cast<PointMassThrustProducerObject*>(thrustProducer);
               pointMassThrustProducer = std::unique_ptr<PointMassThrustProducerObject>(pointMassThrustProducerRaw);
            }
            else
            {
               throw UtInput::UnknownCommand(aInput);
            }

            pointMassThrustProducer->SetName(instanceName);
            pointMassThrustProducer->SetParentVehicle(mParentVehicle);
            pointMassThrustProducer->SetParentPropulsionSystem(this);

            ThrustProducerInfo info;

            // Set subBlock to look for proper termination string
            UtInputBlock subBlock(aInput, "end_engine");
            while (subBlock.ReadCommand())
            {
               std::string localCommand;
               localCommand = subBlock.GetCommand();

               if (localCommand == "fuel_feed")
               {
                  // Attempt to set a tank. If it isn't available yet, store the name until the thrust producer is added
                  aInput.ReadValue(info.fuelFeedName);
               }
               else if (localCommand == "rel_pos_x")
               {
                  double value;
                  aInput.ReadValueOfType(value, UtInput::cLENGTH);

                  // RelPos is in feet
                  double x, y, z;
                  info.relativePosition.Get(x, y, z);
                  x = value * UtMath::cFT_PER_M;
                  info.relativePosition.Set(x, y, z);
               }
               else if (localCommand == "rel_pos_y")
               {
                  double value;
                  aInput.ReadValueOfType(value, UtInput::cLENGTH);

                  // RelPos is in feet
                  double x, y, z;
                  info.relativePosition.Get(x, y, z);
                  y = value * UtMath::cFT_PER_M;
                  info.relativePosition.Set(x, y, z);
               }
               else if (localCommand == "rel_pos_z")
               {
                  double value;
                  aInput.ReadValueOfType(value, UtInput::cLENGTH);

                  // RelPos is in feet
                  double x, y, z;
                  info.relativePosition.Get(x, y, z);
                  z = value * UtMath::cFT_PER_M;
                  info.relativePosition.Set(x, y, z);
               }
               else if (localCommand == "rel_yaw")
               {
                  double value;
                  aInput.ReadValueOfType(value, UtInput::cANGLE);

                  // RelYPR is in radians
                  double y, p, r;
                  info.relativeYawPitchRoll.Get(y, p, r);
                  y = value;
                  info.relativeYawPitchRoll.Set(y, p, r);
               }
               else if (localCommand == "rel_pitch")
               {
                  double value;
                  aInput.ReadValueOfType(value, UtInput::cANGLE);

                  // RelYPR is in radians
                  double y, p, r;
                  info.relativeYawPitchRoll.Get(y, p, r);
                  p = value;
                  info.relativeYawPitchRoll.Set(y, p, r);
               }
               else if (localCommand == "rel_roll")
               {
                  double value;
                  aInput.ReadValueOfType(value, UtInput::cANGLE);

                  // RelYPR is in radians
                  double y, p, r;
                  info.relativeYawPitchRoll.Get(y, p, r);
                  r = value;
                  info.relativeYawPitchRoll.Set(y, p, r);
               }
               else if (pointMassThrustProducer->ProcessTableInput(aInput))
               {
               }
               else
               {
                  {
                     // RAII Block
                     auto out = ut::log::error()
                                << "Unrecognized command within PointMassPropulsionSystem::ProcessInput().";
                     out.AddNote() << "Command: " << localCommand;
                     out.AddNote() << "Location: " << aInput.GetLocation();
                  }
                  throw UtInput::UnknownCommand(aInput);
               }
            }

            std::pair<std::unique_ptr<PointMassThrustProducerObject>, ThrustProducerInfo> thrustProducerData{
               std::move(pointMassThrustProducer),
               info};
            newThrustProducerMap.push_back(std::move(thrustProducerData));
         }
         else if (blockCommand == "fuel_tank")
         {
            auto tempFuelTank = ut::make_unique<FuelTank>();
            tempFuelTank->ProcessInput(aInput);
            AddFuelTank(std::move(tempFuelTank));
         }
         else if (blockCommand == "fuel_transfer")
         {
            std::string xferName;
            aInput.ReadValue(xferName);

            if (!FuelTransferNameUnique(xferName))
            {
               {
                  // RAII Block
                  auto out = ut::log::error()
                             << "PointMassPropulsionSystem is trying to setup a fuel transfer which is not uniquely "
                                "named within PointMassPropulsionSystem::ProcessInput().";
                  out.AddNote() << "Transfer: " << xferName;
                  out.AddNote() << "Location: " << aInput.GetLocation();
               }
               throw UtInput::UnknownCommand(aInput);
            }

            UtInputBlock subBlock(aInput);

            auto tempXfer = ut::make_unique<FuelTransfer>();
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
                  throw UtInput::UnknownCommand(aInput);
               }
            }

            if (!tempXfer->sourceTankName.empty() && !tempXfer->targetTankName.empty())
            {
               mFuelTransferList.push_back(std::move(tempXfer));
            }
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within PointMassPropulsionSystem::ProcessInput().";
            out.AddNote() << "Command: " << blockCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }

      // Add the thrust producer to the list, setting reference placement and assigning a fuel tank if needed
      for (auto& tp : newThrustProducerMap)
      {
         if (!tp.second.fuelFeedName.empty())
         {
            tp.first->SetFuelTank(tp.second.fuelFeedName);
         }
         tp.first->SetThrustReferenceOffsetAttitude(tp.second.relativePosition, tp.second.relativeYawPitchRoll);

         mEngineList.push_back(std::move(tp.first));
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

void wsf::six_dof::PointMassPropulsionSystem::CalculatePropulsionFM(double                  aDeltaT_sec,
                                                                    double                  aAlt_ft,
                                                                    double                  aDynPress_lbsqft,
                                                                    double                  aStatPress_lbssqft,
                                                                    double                  aSpeed_fps,
                                                                    double                  aMach,
                                                                    double                  aAlpha_rad,
                                                                    double                  aBeta_rad,
                                                                    ForceAndRotationObject& aForceAndMoment,
                                                                    double&                 aFuelBurnRate_pps,
                                                                    double&                 aFuelBurned_lbs)
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

void wsf::six_dof::PointMassPropulsionSystem::UpdatePropulsionFM(double                  aDeltaT_sec,
                                                                 double                  aAlt_ft,
                                                                 double                  aDynPress_lbsqft,
                                                                 double                  aStatPress_lbssqft,
                                                                 double                  aSpeed_fps,
                                                                 double                  aMach,
                                                                 double                  aAlpha_rad,
                                                                 double                  aBeta_rad,
                                                                 ForceAndRotationObject& aForceAndMoment,
                                                                 double&                 aFuelBurnRate_pps,
                                                                 double&                 aFuelBurned_lbs)
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

void wsf::six_dof::PointMassPropulsionSystem::PropulsionCalculationsFM(double                  aDeltaT_sec,
                                                                       double                  aAlt_ft,
                                                                       double                  aDynPress_lbsqft,
                                                                       double                  aStatPress_lbssqft,
                                                                       double                  aSpeed_fps,
                                                                       double                  aMach,
                                                                       double                  aAlpha_rad,
                                                                       double                  aBeta_rad,
                                                                       ForceAndRotationObject& aForceAndMoment,
                                                                       double&                 aFuelBurnRate_pps,
                                                                       double&                 aFuelBurned_lbs,
                                                                       bool                    aUpdateData)
{
   // Zero values
   aForceAndMoment.ClearForcesAndRotation();
   aFuelBurnRate_pps = 0.0;
   aFuelBurned_lbs   = 0.0;

   // If the propulsion system throttle was not set directly, check if special inputs
   // are being used, and if so, set engine throttles accordingly
   if (!mThrottleLeverPositionSet)
   {
      if (mThrottleSettingMilPtr != nullptr)
      {
         double milThrottlePosition          = *mThrottleSettingMilPtr;
         double specialThrottleLeverPosition = milThrottlePosition;

         if (AfterburnerIsPresent())
         {
            if (mThrottleSettingAbPtr != nullptr)
            {
               constexpr double epsilonVal   = std::numeric_limits<double>::epsilon();
               constexpr double unityEpsilon = 1.0 - epsilonVal;

               // Include AB throttle if we are essentially at full throttle MIL
               double abThrottlePosition = *mThrottleSettingAbPtr;
               if (milThrottlePosition > unityEpsilon)
               {
                  specialThrottleLeverPosition += abThrottlePosition;
               }
            }
         }

         for (auto& i : mEngineList)
         {
            i->SetThrottlePosition(specialThrottleLeverPosition);
         }
      }
   }

   // Update/calculate thrust of engines
   for (auto& engine : mEngineList)
   {
      UtVec3dX               tempForce_lbs(0., 0., 0.);
      UtVec3dX               tempLocation_ft(0., 0., 0.);
      UtVec3dX               tempMoment_ftlbs(0., 0., 0.);
      UtVec3dX               aResultantThrustVector_lbs;
      UtVec3dX               aResultantThrustMoment_ftlbs;
      ForceAndRotationObject tempThrustFM;
      double                 tempFuelBurnRate_pps = 0.0;
      double                 tempFuelBurned_lbs   = 0.0;

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

wsf::six_dof::ThrustProducerObject*
wsf::six_dof::PointMassPropulsionSystem::GetThrustProducerObjectByName(const std::string& aName) const
{
   for (auto& i : mEngineList)
   {
      if (i->GetName() == aName)
      {
         return i.get();
      }
   }
   return nullptr;
}

wsf::six_dof::ThrustProducerObject* wsf::six_dof::PointMassPropulsionSystem::GetThrustProducerByIndex(size_t aIndex) const
{
   if (aIndex < mEngineList.size())
   {
      return mEngineList.at(aIndex).get();
   }
   return nullptr;
}

void wsf::six_dof::PointMassPropulsionSystem::SetThrottleLeverPosition(double aThrottleLeverPosition)
{
   mPropulsionSystemThrottleLeverPosition = aThrottleLeverPosition;

   // Enforce throttle limits
   if (AfterburnerIsPresent())
   {
      utils::LimitThrottleAfterburner(mPropulsionSystemThrottleLeverPosition);
   }
   else
   {
      utils::LimitThrottleNormalized(mPropulsionSystemThrottleLeverPosition);
   }

   // Set flag
   mThrottleLeverPositionSet = true;

   // Inform the engines of the current throttle setting
   for (auto& engine : mEngineList)
   {
      engine->SetThrottlePosition(mPropulsionSystemThrottleLeverPosition);
   }
}

void wsf::six_dof::PointMassPropulsionSystem::EnableThrustVectoring(bool aEnableThrustVectoring)
{
   mEnableThrustVectoring = aEnableThrustVectoring;
   for (auto& engine : mEngineList)
   {
      engine->SetThrustVectoringEnabled(aEnableThrustVectoring);
   }
}

void wsf::six_dof::PointMassPropulsionSystem::Ignite(int64_t aIgniteTimeInFrame_nanosec)
{
   mPropulsionSystemThrottleLeverPosition = 1.0;
   for (auto& i : mEngineList)
   {
      i->Ignite(aIgniteTimeInFrame_nanosec);
   }
}

void wsf::six_dof::PointMassPropulsionSystem::Shutdown(int64_t aTerminateTime_nanosec)
{
   mPropulsionSystemThrottleLeverPosition = 0.0;
   for (auto& i : mEngineList)
   {
      i->Shutdown(aTerminateTime_nanosec);
   }
}

bool wsf::six_dof::PointMassPropulsionSystem::IsProducingThrust() const
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

bool wsf::six_dof::PointMassPropulsionSystem::SetFuelFeed(std::string& aEngineName, std::string aFuelTankName)
{
   // Apply to the matched engine
   for (auto& engine : mEngineList)
   {
      if (engine->GetName() == aEngineName)
      {
         // Match found. Select a fuel feed, if appropriate.
         return engine->SetFuelTank(aFuelTankName);
      }
   }
   return false;
}

bool wsf::six_dof::PointMassPropulsionSystem::SetFuelFeed(std::string aFuelTankName)
{
   bool result = false;
   // Apply to all engines
   for (auto& engine : mEngineList)
   {
      // Select a fuel feed, if appropriate
      result &= engine->SetFuelTank(aFuelTankName);
   }
   return result;
}

size_t wsf::six_dof::PointMassPropulsionSystem::GetNumThrustProducers() const
{
   return mEngineList.size();
}

bool wsf::six_dof::PointMassPropulsionSystem::AnEngineIsOperating(bool aTestSubobjects) const
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
      if (mParentVehicle)
      {
         auto& subObjectList = mParentVehicle->GetSubobjectList();
         for (auto& subObjectPtr : subObjectList)
         {
            if (subObjectPtr != nullptr)
            {
               // Get the propulsion system
               PropulsionSystem* propulsion = subObjectPtr->GetPropulsionSystem();
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

bool wsf::six_dof::PointMassPropulsionSystem::AnEngineIsSmoking(bool aTestSubobjects) const
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
      if (mParentVehicle)
      {
         auto& subObjectList = mParentVehicle->GetSubobjectList();
         for (auto& subObjectPtr : subObjectList)
         {
            if (subObjectPtr != nullptr)
            {
               // Get the propulsion system
               PropulsionSystem* propulsion = subObjectPtr->GetPropulsionSystem();
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

bool wsf::six_dof::PointMassPropulsionSystem::AnEngineHasAfterburnerOn(bool aTestSubobjects) const
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
      if (mParentVehicle)
      {
         auto& subObjectList = mParentVehicle->GetSubobjectList();
         for (auto& subObjectPtr : subObjectList)
         {
            if (subObjectPtr != nullptr)
            {
               // Get the propulsion system
               PropulsionSystem* propulsion = subObjectPtr->GetPropulsionSystem();
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

bool wsf::six_dof::PointMassPropulsionSystem::AnEngineIsContrailing(bool aTestSubobjects) const
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
      if (mParentVehicle)
      {
         auto& subObjectList = mParentVehicle->GetSubobjectList();
         for (auto& subObjectPtr : subObjectList)
         {
            if (subObjectPtr != nullptr)
            {
               // Get the propulsion system
               PropulsionSystem* propulsion = subObjectPtr->GetPropulsionSystem();
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

bool wsf::six_dof::PointMassPropulsionSystem::AnEngineIsEmittingSmokeTrail(bool aTestSubobjects) const
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
      if (mParentVehicle)
      {
         auto& subObjectList = mParentVehicle->GetSubobjectList();
         for (auto& subObjectPtr : subObjectList)
         {
            if (subObjectPtr != nullptr)
            {
               // Get the propulsion system
               PropulsionSystem* propulsion = subObjectPtr->GetPropulsionSystem();
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

void wsf::six_dof::PointMassPropulsionSystem::MakeAnEngineSmoke(int aEngineIndex)
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
      ThrustProducerObject* enginePtr = GetThrustProducerByIndex(aEngineIndex);
      if (enginePtr)
      {
         enginePtr->MakeEngineSmoke(true);
      }
   }
}

bool wsf::six_dof::PointMassPropulsionSystem::AfterburnerIsPresent() const
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
