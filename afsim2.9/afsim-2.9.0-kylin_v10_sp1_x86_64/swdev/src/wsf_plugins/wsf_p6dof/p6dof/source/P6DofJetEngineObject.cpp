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

#include "P6DofJetEngineObject.hpp"

#include "P6DofForceAndMomentsObject.hpp"
#include "P6DofFreezeFlags.hpp"
#include "P6DofFuelTank.hpp"
#include "P6DofKinematicState.hpp"
#include "P6DofPropulsionSystem.hpp"
#include "P6DofScenario.hpp"
#include "P6DofTypeManager.hpp"
#include "P6DofVehicle.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

P6DofJetEngineObject::P6DofJetEngineObject(P6DofScenario* aScenario)
   : P6DofThrustProducerObject(aScenario)
   , mThrottleLeverPosition(0)
   , mCurrentFuelTank(nullptr)
   , mIdleThrustTable(nullptr)
   , mMilThrustTable(nullptr)
   , mABThrustTable(nullptr)
   , mIdleThrustMachAltTable(nullptr)
   , mMilThrustMachAltTable(nullptr)
   , mABThrustMachAltTable(nullptr)
   , mIdleThrustAltMachTable(nullptr)
   , mMilThrustAltMachTable(nullptr)
   , mABThrustAltMachTable(nullptr)
   , mTSFC_Idle_pph(0)
   , mTSFC_Mil_pph(0)
   , mTSFC_AB_pph(0)
   , mRatedThrustIdle_lbs(0)
   , mRatedThrustMil_lbs(0)
   , mRatedThrustAB_lbs(0)
   , mEffectiveTSFC_Idle_pps(0)
   , mEffectiveTSFC_Mil_pps(0)
   , mEffectiveTSFC_AB_pps(0)
   , mLastThrottleLeverPosition(0)
   , mEnginePercentRPM(0)
   , mNozzlePosition(0)
   , mSpinUpMil_per_sec(0)
   , mSpinDownMil_per_sec(0)
   , mSpinUpAB_per_sec(0)
   , mSpinDownAB_per_sec(0)
   , mSpinUpMilTable(nullptr)
   , mSpinDownMilTable(nullptr)
   , mSpinUpABTable(nullptr)
   , mSpinDownABTable(nullptr)
   , mCurrentFuelBurnRate_pph(0)
   , mThrottleSettingMIL(nullptr)
   , mThrottleSettingAB(nullptr)
   , mThrottleSettingReverser(nullptr)
   , mThrottleSettingYaw(nullptr)
   , mThrottleSettingPitch(nullptr)
   , mThrottleLeverPositionSet(false)
   , mInjectFuel(true)
   , mEngineMaySmoke(false)
   , mEngineSmokesAboveLevel(1.0)
{
   mTypeOfThrustProducer = "Jet";
}

P6DofJetEngineObject::~P6DofJetEngineObject()
{
   // Engine tabular data
   if (mThrottleSettingMIL)
   {
      delete mThrottleSettingMIL;
   }
   if (mThrottleSettingAB)
   {
      delete mThrottleSettingAB;
   }
   if (mThrottleSettingReverser)
   {
      delete mThrottleSettingReverser;
   }
   if (mThrottleSettingYaw)
   {
      delete mThrottleSettingYaw;
   }
   if (mThrottleSettingPitch)
   {
      delete mThrottleSettingPitch;
   }
}

P6DofJetEngineObject::P6DofJetEngineObject(const P6DofJetEngineObject& aSrc)
   : P6DofThrustProducerObject(aSrc)
   , mThrottleLeverPosition(aSrc.mThrottleLeverPosition)
   , mCurrentFuelTank(nullptr)
   , mIdleThrustTable(aSrc.mIdleThrustTable)
   , mMilThrustTable(aSrc.mMilThrustTable)
   , mABThrustTable(aSrc.mABThrustTable)
   , mIdleThrustMachAltTable(aSrc.mIdleThrustMachAltTable)
   , mMilThrustMachAltTable(aSrc.mMilThrustMachAltTable)
   , mABThrustMachAltTable(aSrc.mABThrustMachAltTable)
   , mIdleThrustAltMachTable(aSrc.mIdleThrustAltMachTable)
   , mMilThrustAltMachTable(aSrc.mMilThrustAltMachTable)
   , mABThrustAltMachTable(aSrc.mABThrustAltMachTable)
   , mTSFC_Idle_pph(aSrc.mTSFC_Idle_pph)
   , mTSFC_Mil_pph(aSrc.mTSFC_Mil_pph)
   , mTSFC_AB_pph(aSrc.mTSFC_AB_pph)
   , mRatedThrustIdle_lbs(aSrc.mRatedThrustIdle_lbs)
   , mRatedThrustMil_lbs(aSrc.mRatedThrustMil_lbs)
   , mRatedThrustAB_lbs(aSrc.mRatedThrustAB_lbs)
   , mEffectiveTSFC_Idle_pps(aSrc.mEffectiveTSFC_Idle_pps)
   , mEffectiveTSFC_Mil_pps(aSrc.mEffectiveTSFC_Mil_pps)
   , mEffectiveTSFC_AB_pps(aSrc.mEffectiveTSFC_AB_pps)
   , mLastThrottleLeverPosition(aSrc.mLastThrottleLeverPosition)
   , mEnginePercentRPM(aSrc.mEnginePercentRPM)
   , mNozzlePosition(aSrc.mNozzlePosition)
   , mSpinUpMil_per_sec(aSrc.mSpinUpMil_per_sec)
   , mSpinDownMil_per_sec(aSrc.mSpinDownMil_per_sec)
   , mSpinUpAB_per_sec(aSrc.mSpinUpAB_per_sec)
   , mSpinDownAB_per_sec(aSrc.mSpinDownAB_per_sec)
   , mSpinUpMilTable(aSrc.mSpinUpMilTable)
   , mSpinDownMilTable(aSrc.mSpinDownMilTable)
   , mSpinUpABTable(aSrc.mSpinUpABTable)
   , mSpinDownABTable(aSrc.mSpinDownABTable)
   , mCurrentFuelBurnRate_pph(0)
   , mThrottleSettingMIL(nullptr)
   , mThrottleSettingAB(nullptr)
   , mThrottleSettingReverser(nullptr)
   , mThrottleSettingYaw(nullptr)
   , mThrottleSettingPitch(nullptr)
   , mThrottleLeverPositionSet(aSrc.mThrottleLeverPositionSet)
   , mInjectFuel(aSrc.mInjectFuel)
   , mEngineMaySmoke(aSrc.mEngineMaySmoke)
   , mEngineSmokesAboveLevel(aSrc.mEngineSmokesAboveLevel)
{
   if (aSrc.mThrottleSettingMIL != nullptr)
   {
      mThrottleSettingMIL = new ThrustControlInputValueData(*aSrc.mThrottleSettingMIL);
   }
   if (aSrc.mThrottleSettingAB != nullptr)
   {
      mThrottleSettingAB = new ThrustControlInputValueData(*aSrc.mThrottleSettingAB);
   }
   if (aSrc.mThrottleSettingReverser != nullptr)
   {
      mThrottleSettingReverser = new ThrustControlInputValueData(*aSrc.mThrottleSettingReverser);
   }
   if (aSrc.mThrottleSettingYaw != nullptr)
   {
      mThrottleSettingYaw = new ThrustControlInputValueData(*aSrc.mThrottleSettingYaw);
   }
   if (aSrc.mThrottleSettingPitch != nullptr)
   {
      mThrottleSettingPitch = new ThrustControlInputValueData(*aSrc.mThrottleSettingPitch);
   }
}

void P6DofJetEngineObject::DeriveFrom(const P6DofJetEngineObject* aSrc)
{
   P6DofThrustProducerObject::DeriveFrom(aSrc);

   if (aSrc == nullptr)
   {
      return;
   }

   mIdleThrustTable = aSrc->mIdleThrustTable;
   mMilThrustTable  = aSrc->mMilThrustTable;
   mABThrustTable   = aSrc->mABThrustTable;

   mIdleThrustMachAltTable = aSrc->mIdleThrustMachAltTable;
   mMilThrustMachAltTable  = aSrc->mMilThrustMachAltTable;
   mABThrustMachAltTable   = aSrc->mABThrustMachAltTable;

   mIdleThrustAltMachTable = aSrc->mIdleThrustAltMachTable;
   mMilThrustAltMachTable  = aSrc->mMilThrustAltMachTable;
   mABThrustAltMachTable   = aSrc->mABThrustAltMachTable;

   mThrottleLeverPosition     = aSrc->mThrottleLeverPosition;
   mTSFC_Idle_pph             = aSrc->mTSFC_Idle_pph;
   mTSFC_Mil_pph              = aSrc->mTSFC_Mil_pph;
   mTSFC_AB_pph               = aSrc->mTSFC_AB_pph;
   mRatedThrustIdle_lbs       = aSrc->mRatedThrustIdle_lbs;
   mRatedThrustMil_lbs        = aSrc->mRatedThrustMil_lbs;
   mRatedThrustAB_lbs         = aSrc->mRatedThrustAB_lbs;
   mEffectiveTSFC_Idle_pps    = aSrc->mEffectiveTSFC_Idle_pps;
   mEffectiveTSFC_Mil_pps     = aSrc->mEffectiveTSFC_Mil_pps;
   mEffectiveTSFC_AB_pps      = aSrc->mEffectiveTSFC_AB_pps;
   mLastThrottleLeverPosition = aSrc->mLastThrottleLeverPosition;
   mEnginePercentRPM          = aSrc->mEnginePercentRPM;
   mNozzlePosition            = aSrc->mNozzlePosition;
   mSpinUpMil_per_sec         = aSrc->mSpinUpMil_per_sec;
   mSpinDownMil_per_sec       = aSrc->mSpinDownMil_per_sec;
   mSpinUpAB_per_sec          = aSrc->mSpinUpAB_per_sec;
   mSpinDownAB_per_sec        = aSrc->mSpinDownAB_per_sec;

   mSpinUpMilTable   = aSrc->mSpinUpMilTable;
   mSpinDownMilTable = aSrc->mSpinDownMilTable;
   mSpinUpABTable    = aSrc->mSpinUpABTable;
   mSpinDownABTable  = aSrc->mSpinDownABTable;

   mCurrentFuelBurnRate_pph = 0.0;

   if (mThrottleSettingMIL != nullptr)
   {
      delete mThrottleSettingMIL;
   }
   if (mThrottleSettingAB != nullptr)
   {
      delete mThrottleSettingAB;
   }
   if (mThrottleSettingReverser != nullptr)
   {
      delete mThrottleSettingReverser;
   }
   if (mThrottleSettingYaw != nullptr)
   {
      delete mThrottleSettingYaw;
   }
   if (mThrottleSettingPitch != nullptr)
   {
      delete mThrottleSettingPitch;
   }

   if (aSrc->mThrottleSettingMIL != nullptr)
   {
      mThrottleSettingMIL = new ThrustControlInputValueData(*aSrc->mThrottleSettingMIL);
   }
   if (aSrc->mThrottleSettingAB != nullptr)
   {
      mThrottleSettingAB = new ThrustControlInputValueData(*aSrc->mThrottleSettingAB);
   }
   if (aSrc->mThrottleSettingReverser != nullptr)
   {
      mThrottleSettingReverser = new ThrustControlInputValueData(*aSrc->mThrottleSettingReverser);
   }
   if (aSrc->mThrottleSettingYaw != nullptr)
   {
      mThrottleSettingYaw = new ThrustControlInputValueData(*aSrc->mThrottleSettingYaw);
   }
   if (aSrc->mThrottleSettingPitch != nullptr)
   {
      mThrottleSettingPitch = new ThrustControlInputValueData(*aSrc->mThrottleSettingPitch);
   }

   mThrottleLeverPositionSet = aSrc->mThrottleLeverPositionSet;

   DetermineIfAfterburnerIsPresent();

   mEngineMaySmoke         = aSrc->mEngineMaySmoke;
   mEngineSmokesAboveLevel = aSrc->mEngineSmokesAboveLevel;
}

void P6DofJetEngineObject::SetControlInputHandles(const P6DofFlightControlSystem* aFlightControls)
{
   SetControlInputHandle(aFlightControls, mThrottleSettingMIL);
   SetControlInputHandle(aFlightControls, mThrottleSettingAB);
   SetControlInputHandle(aFlightControls, mThrottleSettingReverser);
   SetControlInputHandle(aFlightControls, mThrottleSettingYaw);
   SetControlInputHandle(aFlightControls, mThrottleSettingPitch);
}

void P6DofJetEngineObject::SetControlInputValues(const P6DofFlightControlSystem* aFlightControls)
{
   SetControlInputValue(aFlightControls, mThrottleSettingMIL);
   SetControlInputValue(aFlightControls, mThrottleSettingAB);
   SetControlInputValue(aFlightControls, mThrottleSettingReverser);
   SetControlInputValue(aFlightControls, mThrottleSettingYaw);
   SetControlInputValue(aFlightControls, mThrottleSettingPitch);
}

bool P6DofJetEngineObject::DetermineIfAfterburnerIsPresent()
{
   mAfterburnerPresent = (mABThrustTable || mABThrustMachAltTable || mABThrustAltMachTable);
   return mAfterburnerPresent;
}

P6DofJetEngineObject* P6DofJetEngineObject::Clone() const
{
   return new P6DofJetEngineObject(*this);
}

bool P6DofJetEngineObject::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   if (command == "jet_engine_type")
   {
      std::string typeName;
      std::string baseType;
      aInput.ReadValue(typeName);
      aInput.ReadValue(baseType);
      SetName(typeName); // Types should have the typeName be their name

      bool derivedFromBase = true;
      if (baseType != "P6DOF_JET_ENGINE")
      {
         derivedFromBase = false;
         // This is not derived from the base type
         // Determine if this base type exists
         if (!GetScenario()->GetTypeManager()->ThrustProducerObjectTypeExists(baseType))
         {
            // Unknown type -- error since we can't derive from an unknown type
            auto out = ut::log::error() << "P6DofJetEngineObject is trying to derive from an unknown type.";
            out.AddNote() << "Base Type: " << baseType;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }

      // We have a known type from which to derive
      // Determine if the name is unique
      if (GetScenario()->GetTypeManager()->ThrustProducerObjectTypeExists(typeName))
      {
         // Non-unique type name -- error since we can't have duplicate names
         auto out = ut::log::error() << "P6DofJetEngineObject is using a duplicate name.";
         out.AddNote() << "Name: " << typeName;
         out.AddNote() << "Location: " << aInput.GetLocation();
         throw UtInput::UnknownCommand(aInput);
      }

      // If derived, clone the data
      if (!derivedFromBase)
      {
         DeriveFrom(
            dynamic_cast<P6DofJetEngineObject*>(GetScenario()->GetTypeManager()->GetThrustProducerObjectType(baseType)));
      }

      // Register the object with the manager
      if (!GetScenario()->GetTypeManager()->RegisterThrustProducerObjectType(this))
      {
         auto out = ut::log::error() << "P6DofJetEngineObject unable to register.";
         out.AddNote() << "Name: " << typeName;
         out.AddNote() << "Location: " << aInput.GetLocation();
         throw UtInput::UnknownCommand(aInput);
      }

      UtInputBlock block(aInput, "end_jet_engine_type");
      while (block.ReadCommand())
      {
         std::string localCommand;
         localCommand = block.GetCommand();
         if (localCommand == "tsfc_idle_pph")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            mTSFC_Idle_pph = value;
         }
         else if (localCommand == "tsfc_mil_pph")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            mTSFC_Mil_pph = value;
         }
         else if (localCommand == "tsfc_ab_pph")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            mTSFC_AB_pph = value;
         }
         else if (localCommand == "rated_thrust_idle")
         {
            double value = 0.0;
            aInput.ReadValueOfType(value, UtInput::cFORCE);
            value *= UtMath::cLB_PER_NT;
            mRatedThrustIdle_lbs = value;
         }
         else if (localCommand == "rated_thrust_mil")
         {
            double value = 0.0;
            aInput.ReadValueOfType(value, UtInput::cFORCE);
            value *= UtMath::cLB_PER_NT;
            mRatedThrustMil_lbs = value;
         }
         else if (localCommand == "rated_thrust_ab")
         {
            double value = 0.0;
            aInput.ReadValueOfType(value, UtInput::cFORCE);
            value *= UtMath::cLB_PER_NT;
            mRatedThrustAB_lbs = value;
         }
         else if (localCommand == "spin_up_mil_per_sec")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            mSpinUpMil_per_sec = value;
         }
         else if (localCommand == "spin_down_mil_per_sec")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            mSpinDownMil_per_sec = value;
         }
         else if (localCommand == "spin_up_ab_per_sec")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            mSpinUpAB_per_sec = value;
         }
         else if (localCommand == "spin_down_ab_per_sec")
         {
            double value = 0.0;
            aInput.ReadValue(value);
            mSpinDownAB_per_sec = value;
         }
         else if (localCommand == "spin_up_table_mil_per_sec")
         {
            mSpinUpMilTable = ut::make_unique<UtTable::Curve>();
            mSpinUpMilTable->ProcessInput(aInput,
                                          UtInput::cNON_DIMENSIONAL,
                                          "throttle_level",
                                          UtTable::NoCheck(),
                                          UtInput::cNON_DIMENSIONAL,
                                          "spin_up_per_sec",
                                          UtTable::NoCheck());
         }
         else if (localCommand == "spin_down_table_mil_per_sec")
         {
            mSpinDownMilTable = ut::make_unique<UtTable::Curve>();
            mSpinDownMilTable->ProcessInput(aInput,
                                            UtInput::cNON_DIMENSIONAL,
                                            "throttle_level",
                                            UtTable::NoCheck(),
                                            UtInput::cNON_DIMENSIONAL,
                                            "spin_down_per_sec",
                                            UtTable::NoCheck());
         }
         else if (localCommand == "spin_up_table_ab_per_sec")
         {
            mSpinUpABTable = ut::make_unique<UtTable::Curve>();
            mSpinUpABTable->ProcessInput(aInput,
                                         UtInput::cNON_DIMENSIONAL,
                                         "throttle_level",
                                         UtTable::NoCheck(),
                                         UtInput::cNON_DIMENSIONAL,
                                         "spin_up_per_sec",
                                         UtTable::NoCheck());
         }
         else if (localCommand == "spin_down_table_ab_per_sec")
         {
            mSpinDownABTable = ut::make_unique<UtTable::Curve>();
            mSpinDownABTable->ProcessInput(aInput,
                                           UtInput::cNON_DIMENSIONAL,
                                           "throttle_level",
                                           UtTable::NoCheck(),
                                           UtInput::cNON_DIMENSIONAL,
                                           "spin_down_per_sec",
                                           UtTable::NoCheck());
         }
         else if (localCommand == "thrust_table_idle")
         {
            mIdleThrustTable = ut::make_unique<UtTable::Curve>();
            mIdleThrustTable->ProcessInput(aInput,
                                           UtInput::cNON_DIMENSIONAL,
                                           "alt",
                                           UtTable::NoCheck(),
                                           UtInput::cNON_DIMENSIONAL,
                                           "thrust",
                                           UtTable::NoCheck());
         }
         else if (localCommand == "thrust_table_mil")
         {
            mMilThrustTable = ut::make_unique<UtTable::Curve>();
            mMilThrustTable->ProcessInput(aInput,
                                          UtInput::cNON_DIMENSIONAL,
                                          "alt",
                                          UtTable::NoCheck(),
                                          UtInput::cNON_DIMENSIONAL,
                                          "thrust",
                                          UtTable::NoCheck());
         }
         else if (localCommand == "thrust_table_ab")
         {
            mABThrustTable = ut::make_unique<UtTable::Curve>();
            mABThrustTable->ProcessInput(aInput,
                                         UtInput::cNON_DIMENSIONAL,
                                         "alt",
                                         UtTable::NoCheck(),
                                         UtInput::cNON_DIMENSIONAL,
                                         "thrust",
                                         UtTable::NoCheck());
         }
         else if (localCommand == "thrust_idle_mach_alt_table")
         {
            UtInputBlock subBlock(aInput, "end_thrust_idle_mach_alt_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("alt", UtInput::cLENGTH, UtTable::NoCheck(), 1);
               mIdleThrustMachAltTable =
                  UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (localCommand == "thrust_mil_mach_alt_table")
         {
            UtInputBlock subBlock(aInput, "end_thrust_mil_mach_alt_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("alt", UtInput::cLENGTH, UtTable::NoCheck(), 1);
               mMilThrustMachAltTable =
                  UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (localCommand == "thrust_ab_mach_alt_table")
         {
            UtInputBlock subBlock(aInput, "end_thrust_ab_mach_alt_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 0);
               sParameters.AddRealParameter("alt", UtInput::cLENGTH, UtTable::NoCheck(), 1);
               mABThrustMachAltTable =
                  UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (localCommand == "thrust_idle_alt_mach_table")
         {
            UtInputBlock subBlock(aInput, "end_thrust_idle_alt_mach_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("alt", UtInput::cLENGTH, UtTable::NoCheck(), 0);
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 1);
               mIdleThrustAltMachTable =
                  UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (localCommand == "thrust_mil_alt_mach_table")
         {
            UtInputBlock subBlock(aInput, "end_thrust_mil_alt_mach_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("alt", UtInput::cLENGTH, UtTable::NoCheck(), 0);
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 1);
               mMilThrustAltMachTable =
                  UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (localCommand == "thrust_ab_alt_mach_table")
         {
            UtInputBlock subBlock(aInput, "end_thrust_ab_alt_mach_table");
            do
            {
               UtTable::Parameters sParameters;
               sParameters.AddRealParameter("alt", UtInput::cLENGTH, UtTable::NoCheck(), 0);
               sParameters.AddRealParameter("mach", UtInput::cNON_DIMENSIONAL, UtTable::ValueGE(0.0), 1);
               mABThrustAltMachTable =
                  UtTable::LoadInstance(aInput, UtInput::cNON_DIMENSIONAL, UtTable::NoCheck(), sParameters);
            } while (subBlock.ReadCommand());
         }
         else if (localCommand == "throttle_setting_mil")
         {
            std::string inputName;
            aInput.ReadValue(inputName);

            if (mThrottleSettingMIL != nullptr)
            {
               delete mThrottleSettingMIL;
            }

            mThrottleSettingMIL         = new ThrustControlInputValueData;
            mThrottleSettingMIL->handle = 0;
            mThrottleSettingMIL->name   = inputName;
            mThrottleSettingMIL->value  = 0;
         }
         else if (localCommand == "throttle_setting_ab")
         {
            std::string inputName;
            aInput.ReadValue(inputName);

            if (mThrottleSettingAB != nullptr)
            {
               delete mThrottleSettingAB;
            }

            mThrottleSettingAB         = new ThrustControlInputValueData;
            mThrottleSettingAB->handle = 0;
            mThrottleSettingAB->name   = inputName;
            mThrottleSettingAB->value  = 0;
         }
         else if (localCommand == "throttle_setting_reverser")
         {
            std::string inputName;
            aInput.ReadValue(inputName);

            if (mThrottleSettingReverser != nullptr)
            {
               delete mThrottleSettingReverser;
            }

            mThrottleSettingReverser         = new ThrustControlInputValueData;
            mThrottleSettingReverser->handle = 0;
            mThrottleSettingReverser->name   = inputName;
            mThrottleSettingReverser->value  = 0;
         }
         else if (localCommand == "throttle_setting_yaw")
         {
            std::string inputName;
            aInput.ReadValue(inputName);

            if (mThrottleSettingYaw != nullptr)
            {
               delete mThrottleSettingYaw;
            }

            mThrottleSettingYaw         = new ThrustControlInputValueData;
            mThrottleSettingYaw->handle = 0;
            mThrottleSettingYaw->name   = inputName;
            mThrottleSettingYaw->value  = 0;

            // Since this is capable of thrust vectoring (TV), enable TV by default
            mThrustVectoringEnabled = true;
         }
         else if (localCommand == "throttle_setting_pitch")
         {
            std::string inputName;
            aInput.ReadValue(inputName);

            if (mThrottleSettingPitch != nullptr)
            {
               delete mThrottleSettingPitch;
            }

            mThrottleSettingPitch         = new ThrustControlInputValueData;
            mThrottleSettingPitch->handle = 0;
            mThrottleSettingPitch->name   = inputName;
            mThrottleSettingPitch->value  = 0;

            // Since this is capable of thrust vectoring (TV), enable TV by default
            mThrustVectoringEnabled = true;
         }
         else if (localCommand == "thrust_offset")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cLENGTH);
            value *= UtMath::cFT_PER_M;
            mThrustOffset_ft = value;
         }
         else if (localCommand == "inop_ref_area")
         {
            double value;
            aInput.ReadValueOfType(value, UtInput::cAREA);
            value *= (UtMath::cFT_PER_M * UtMath::cFT_PER_M);
            mInOpRefArea_ft2  = value;
            mInOpRefAreaValid = true;
         }
         else if (localCommand == "engine_smokes_above_power_setting")
         {
            double value;
            aInput.ReadValue(value);
            if (value >= 0.0 && value <= 1.0)
            {
               mEngineSmokesAboveLevel = value;
               mEngineMaySmoke         = true;
            }
            else
            {
               auto out = ut::log::warning() << "Invalid 'engine_smokes_above_throttle_position' value.";
               out.AddNote() << "Received: " << value;
               out.AddNote() << "Expected: 0.0 <= value <= 1.0";
               out.AddNote() << "Location: " << aInput.GetLocation();
            }
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within P6DofJetEngineObject::ProcessInput().";
            out.AddNote() << "Command: " << localCommand;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }

      // Calculate effective TSFC
      double maxIdleBurn_pph     = mRatedThrustIdle_lbs * mTSFC_Idle_pph;
      double maxMilBurn_pph      = mRatedThrustMil_lbs * mTSFC_Mil_pph;
      double maxABBurn_pph       = mRatedThrustAB_lbs * mTSFC_AB_pph;
      double extraMil_thrust_lbs = mRatedThrustMil_lbs - mRatedThrustIdle_lbs;
      double extraAB_thrust_lbs  = mRatedThrustAB_lbs - mRatedThrustMil_lbs;
      double extraMilBurn_pph    = maxMilBurn_pph - maxIdleBurn_pph;
      double extraABBurn_pph     = maxABBurn_pph - maxMilBurn_pph;
      double effTsfcMil_pph      = extraMilBurn_pph / extraMil_thrust_lbs;
      double effTsfcAb_pph       = extraABBurn_pph / extraAB_thrust_lbs;
      mEffectiveTSFC_Idle_pps    = mTSFC_Idle_pph / 3600.0;
      mEffectiveTSFC_Mil_pps     = effTsfcMil_pph / 3600.0;
      mEffectiveTSFC_AB_pps      = effTsfcAb_pph / 3600.0;

      DetermineIfAfterburnerIsPresent();
   }
   return myCommand;
}

bool P6DofJetEngineObject::Initialize(int64_t) //(int64_t aSimTime_nanosec)
{
   // Calculate effective TSFC
   double maxIdleBurn_pph     = mRatedThrustIdle_lbs * mTSFC_Idle_pph;
   double maxMilBurn_pph      = mRatedThrustMil_lbs * mTSFC_Mil_pph;
   double maxABBurn_pph       = mRatedThrustAB_lbs * mTSFC_AB_pph;
   double extraMil_thrust_lbs = mRatedThrustMil_lbs - mRatedThrustIdle_lbs;
   double extraAB_thrust_lbs  = mRatedThrustAB_lbs - mRatedThrustMil_lbs;
   double extraMilBurn_pph    = maxMilBurn_pph - maxIdleBurn_pph;
   double extraABBurn_pph     = maxABBurn_pph - maxMilBurn_pph;
   double effTsfcMil_pph      = extraMilBurn_pph / extraMil_thrust_lbs;
   double effTsfcAb_pph       = extraABBurn_pph / extraAB_thrust_lbs;
   mEffectiveTSFC_Idle_pps    = mTSFC_Idle_pph / 3600.0;
   mEffectiveTSFC_Mil_pps     = effTsfcMil_pph / 3600.0;
   mEffectiveTSFC_AB_pps      = effTsfcAb_pph / 3600.0;

   DetermineIfAfterburnerIsPresent();
   return true;
}

void P6DofJetEngineObject::CalculateThrust(double                      aDeltaT_sec,
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
   CalculateThrust(aDeltaT_sec,
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

void P6DofJetEngineObject::UpdateThrust(double                      aDeltaT_sec,
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
   CalculateThrust(aDeltaT_sec,
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

void P6DofJetEngineObject::CalculateThrust(double aDeltaT_sec,
                                           double aAlt_ft,
                                           double aDynPress_lbsqft,
                                           double, // aStatPress_lbssqft,
                                           double, // aSpeed_fps,
                                           double aMach,
                                           double, // aAlpha_rad,
                                           double, // aBeta_rad,
                                           P6DofForceAndMomentsObject& aForceAndMoment,
                                           double&                     aFuelBurnRate_pps,
                                           double&                     aFuelBurned_lbs,
                                           bool                        aUpdateData)
{
   // Reset flags (they may be set below)
   mEngineOperating     = false;
   mEngineSmoking       = false;
   mAfterburnerOn       = false;
   mContrailing         = false;
   mProducingSmokeTrail = false; // Turbojets/turbofans do not produce smoke trails

   // Ensure we never divide by 0
   if (aDeltaT_sec < P6DofUtils::cEPSILON_SIMTIME_SEC)
   {
      // Avoid modifying state variables

      aForceAndMoment   = mCurrentThrustFM_lbs_ftlbs;
      aFuelBurnRate_pps = mCurrentFuelBurnRate_pph / 3600.0;
      aFuelBurned_lbs   = 0.0;
      return;
   }

   if (mEngineDamageSmokeActivated)
   {
      mEngineSmoking = true;
   }

   double throttleLeverPosition = 0;
   if (mThrottleLeverPositionSet)
   {
      // Directly set the throttleLeverPosition since it was commanded directly
      throttleLeverPosition = mThrottleLeverPosition;
   }
   else
   {
      if (mThrottleSettingMIL)
      {
         throttleLeverPosition = mThrottleSettingMIL->value;

         if (mAfterburnerPresent)
         {
            if (mThrottleSettingAB)
            {
               if (mThrottleSettingMIL->value > 0.99)
               {
                  throttleLeverPosition += mThrottleSettingAB->value;
               }
            }
         }
      }

      // Stay within limits
      if (mAfterburnerPresent)
      {
         P6DofUtils::LimitThrottleAfterburner(throttleLeverPosition);
      }
      else
      {
         P6DofUtils::LimitThrottleNormalized(throttleLeverPosition);
      }
   }

   double throttle_mil_lever = 0.0;
   double throttle_ab_lever  = 0.0;
   double maxSpinUpMil       = mSpinUpMil_per_sec * aDeltaT_sec;
   double maxSpinDownMil     = mSpinDownMil_per_sec * aDeltaT_sec;
   double maxSpinUpAB        = mSpinUpAB_per_sec * aDeltaT_sec;
   double maxSpinDownAB      = mSpinDownAB_per_sec * aDeltaT_sec;

   // Use tabular spin-up/down tables, if present
   if (mSpinUpMilTable)
   {
      maxSpinUpMil = aDeltaT_sec * mSpinUpMilTable->Lookup(mLastThrottleLeverPosition);
   }
   if (mSpinDownMilTable)
   {
      maxSpinDownMil = aDeltaT_sec * mSpinDownMilTable->Lookup(mLastThrottleLeverPosition);
   }
   if (mSpinUpABTable)
   {
      maxSpinUpAB = aDeltaT_sec * mSpinUpABTable->Lookup(mLastThrottleLeverPosition);
   }
   if (mSpinDownABTable)
   {
      maxSpinDownAB = aDeltaT_sec * mSpinDownABTable->Lookup(mLastThrottleLeverPosition);
   }

   double currentEffectiveThrottleLeverPosition = mLastThrottleLeverPosition;
   double deltaThrottle                         = throttleLeverPosition - currentEffectiveThrottleLeverPosition;

   if (deltaThrottle >= 0.0)
   {
      if (currentEffectiveThrottleLeverPosition > 1.0)
      {
         // In AB
         if (deltaThrottle > maxSpinUpAB)
         {
            deltaThrottle = maxSpinUpAB;
         }
      }
      else
      {
         // In Mil
         if (deltaThrottle > maxSpinUpMil)
         {
            deltaThrottle = maxSpinUpMil;
         }
      }

      if (!mAfterburnerPresent)
      {
         if (deltaThrottle > maxSpinUpMil)
         {
            deltaThrottle = maxSpinUpMil;
         }
      }
   }
   else
   {
      if (currentEffectiveThrottleLeverPosition > 1.0)
      {
         // In AB
         if (deltaThrottle < -maxSpinDownAB)
         {
            deltaThrottle = -maxSpinDownAB;
         }
      }
      else
      {
         // In Mil
         if (deltaThrottle < -maxSpinDownMil)
         {
            deltaThrottle = -maxSpinDownMil;
         }
      }

      if (!mAfterburnerPresent)
      {
         if (deltaThrottle < -maxSpinDownMil)
         {
            deltaThrottle = -maxSpinDownMil;
         }
      }
   }

   currentEffectiveThrottleLeverPosition += deltaThrottle;

   // If no lag, the current throttle is the current lever position (no lag)
   if (GetScenario()->GetFreezeFlags()->GetMasterNoLagTesting())
   {
      currentEffectiveThrottleLeverPosition = throttleLeverPosition;
   }

   // Limit throttle lever limits
   if (mAfterburnerPresent)
   {
      P6DofUtils::LimitThrottleAfterburner(currentEffectiveThrottleLeverPosition);
   }
   else
   {
      P6DofUtils::LimitThrottleNormalized(currentEffectiveThrottleLeverPosition);
   }

   if (currentEffectiveThrottleLeverPosition > 1.0)
   {
      throttle_mil_lever = 1.0;
      throttle_ab_lever  = currentEffectiveThrottleLeverPosition - 1.0;
   }
   else
   {
      throttle_mil_lever = currentEffectiveThrottleLeverPosition;
      throttle_ab_lever  = 0.0;
   }

   if (!mAfterburnerPresent)
   {
      throttle_mil_lever = currentEffectiveThrottleLeverPosition;
      throttle_ab_lever  = 0.0;
      if (throttle_mil_lever > 1.0)
      {
         throttle_mil_lever = 1.0;
      }
   }

   double tempIdleThrust_lbs = 0.0;
   double tempMilThrust_lbs  = 0.0;
   double tempABThrust_lbs   = 0.0;

   // Check for simple tables case
   if (mMilThrustTable)
   {
      if (mIdleThrustTable)
      {
         tempIdleThrust_lbs = mIdleThrustTable->Lookup(aAlt_ft);
      }
      if (mMilThrustTable)
      {
         tempMilThrust_lbs = mMilThrustTable->Lookup(aAlt_ft);
      }
      if (mABThrustTable)
      {
         tempABThrust_lbs = mABThrustTable->Lookup(aAlt_ft);
      }

      // Order here is important, do AB before mil
      tempABThrust_lbs  = tempABThrust_lbs - tempMilThrust_lbs;
      tempMilThrust_lbs = tempMilThrust_lbs - tempIdleThrust_lbs;
   }
   // Check for improved tables case
   else if (mMilThrustMachAltTable || mMilThrustAltMachTable)
   {
      double lookupArgs[2];

      lookupArgs[0] = aMach;
      lookupArgs[1] = aAlt_ft * UtMath::cM_PER_FT;
      if (mIdleThrustMachAltTable)
      {
         tempIdleThrust_lbs = mIdleThrustMachAltTable->Lookup(lookupArgs);
      }
      if (mMilThrustMachAltTable)
      {
         tempMilThrust_lbs = mMilThrustMachAltTable->Lookup(lookupArgs);
      }
      if (mABThrustMachAltTable)
      {
         tempABThrust_lbs = mABThrustMachAltTable->Lookup(lookupArgs);
      }

      lookupArgs[0] = aAlt_ft * UtMath::cM_PER_FT;
      lookupArgs[1] = aMach;
      if (mIdleThrustAltMachTable)
      {
         tempIdleThrust_lbs = mIdleThrustAltMachTable->Lookup(lookupArgs);
      }
      if (mMilThrustAltMachTable)
      {
         tempMilThrust_lbs = mMilThrustAltMachTable->Lookup(lookupArgs);
      }
      if (mABThrustAltMachTable)
      {
         tempABThrust_lbs = mABThrustAltMachTable->Lookup(lookupArgs);
      }

      // Order here is important, do ab before mil
      tempABThrust_lbs  = tempABThrust_lbs - tempMilThrust_lbs;
      tempMilThrust_lbs = tempMilThrust_lbs - tempIdleThrust_lbs;
   }

   // Adjust for throttle
   tempMilThrust_lbs = throttle_mil_lever * tempMilThrust_lbs;
   tempABThrust_lbs  = throttle_ab_lever * tempABThrust_lbs;

   if (!mAfterburnerPresent)
   {
      tempABThrust_lbs = 0.0;
   }

   // Determine thrust
   double thrust_lbs = tempIdleThrust_lbs + tempMilThrust_lbs + tempABThrust_lbs;

   // Determine fuel burn
   double idleComponentFuelBurn = mEffectiveTSFC_Idle_pps * tempIdleThrust_lbs * aDeltaT_sec;
   double milComponentFuelBurn  = mEffectiveTSFC_Mil_pps * tempMilThrust_lbs * aDeltaT_sec;
   double abComponentFuelBurn   = mEffectiveTSFC_AB_pps * tempABThrust_lbs * aDeltaT_sec;

   double fuelBurn_lbs = idleComponentFuelBurn + milComponentFuelBurn + abComponentFuelBurn;

   // Determine fuel burn
   double   fuelBurnRequest_lbs    = fuelBurn_lbs;
   double   fuelActuallyBurned_lbs = 0.0;
   UtVec3dX newFuelCgLocation_ft(0.0, 0.0, 0.0);

   // We use this flag to denote an engine that is "dead". In this case, dead means that the engine
   // cannot produce thrust due to fuel starvation for the *entire* calculation time, due
   // to a lack of any fuel at all.
   bool deadEngine = false;

   // Ensure that we still have a fuel flow path to the current tank
   if (mCurrentFuelTank != nullptr)
   {
      bool flowPathIntact = mCurrentFuelTank->FuelFlowPathIntact(mParentPropulsionSystemPtr);
      if (!flowPathIntact)
      {
         // We do not have a fuel flow path to the current tank. This is often because an
         // external fuel tank has been jettisoned.

         // Since the tank is invalid, set pointer to null, since we no longer can get fuel
         mCurrentFuelTank = nullptr;
      }
   }

   // If we lack a current fuel tank, are not injecting fuel, and/or are not
   // asking to burn fuel (perhaps because we're above the engine's ceiling),
   // the engine is dead
   if ((mCurrentFuelTank == nullptr) || !mInjectFuel || (fuelBurnRequest_lbs <= 0))
   {
      deadEngine = true;
   }

   // Determine if we have enough fuel to burn for the entire time-step
   bool ableToBurnAllFuel = false;
   if (!deadEngine)
   {
      double newFuelMass_lbs = 0.0;
      if (aUpdateData)
      {
         // We call update, rather than calculate
         ableToBurnAllFuel = mCurrentFuelTank->UpdateFuelBurn(aDeltaT_sec,
                                                              fuelBurnRequest_lbs,
                                                              fuelActuallyBurned_lbs,
                                                              newFuelMass_lbs,
                                                              newFuelCgLocation_ft);
      }
      else
      {
         // We call calculate, rather than update
         ableToBurnAllFuel = mCurrentFuelTank->CalculateFuelBurn(aDeltaT_sec,
                                                                 fuelBurnRequest_lbs,
                                                                 fuelActuallyBurned_lbs,
                                                                 newFuelMass_lbs,
                                                                 newFuelCgLocation_ft);
      }
   }

   // Check for dead engine drag
   double deadEngineDrag_lbs = 0;
   if (deadEngine || !ableToBurnAllFuel)
   {
      // The engine is fuel-starved and is not producing thrust. We
      // therefore need to calculate the drag on the dead engine.
      if (mInOpRefAreaValid)
      {
         deadEngineDrag_lbs = mInOpRefArea_ft2 * aDynPress_lbsqft;
      }
      else
      {
         // We do not have a InOpRefArea, so we need some other means
         // of calculating a "dead engine drag". We use a 1 sqft drag
         // reference, so that the drag is merely the dyn pressure.
         deadEngineDrag_lbs = aDynPress_lbsqft;
      }
   }

   // Now, we check for a dead engine, an inability to burn the requested fuel, or a fully
   // operational engine, and handle each case accordingly
   double effectiveThrust_lbs = 0;
   if (deadEngine)
   {
      effectiveThrust_lbs = -deadEngineDrag_lbs;

      // Set various engine flags
      mEngineOperating = false;
      mAfterburnerOn   = false;

      // No smoke -- engine not operating

      // We are not burning fuel
      aFuelBurnRate_pps = 0.0;
      aFuelBurned_lbs   = 0.0;
   }
   else if (!ableToBurnAllFuel)
   {
      // Determine a ratio for burn time to flame-out time
      double burnRatio = fuelActuallyBurned_lbs / fuelBurnRequest_lbs;

      // Calculate the effective thrust
      effectiveThrust_lbs = (thrust_lbs * burnRatio) - deadEngineDrag_lbs * (1 - burnRatio);

      // Set various engine flags
      mEngineOperating = false;
      mAfterburnerOn   = false;

      // No smoke -- engine not operating

      // Set the returned argument values for fuel burn
      aFuelBurnRate_pps = fuelActuallyBurned_lbs / aDeltaT_sec;
      aFuelBurned_lbs   = fuelActuallyBurned_lbs;
   }
   else // everything is fine
   {
      effectiveThrust_lbs = thrust_lbs;

      // Set various engine flags
      mEngineOperating = true;

      if (mAfterburnerPresent && (throttle_ab_lever > 0))
      {
         mAfterburnerOn = true;
      }
      else
      {
         mAfterburnerOn = false;
      }

      // Check for engine smoke
      if (mEngineMaySmoke)
      {
         // Generate smoke if throttle is above smoking level and not in afterburner
         if ((currentEffectiveThrottleLeverPosition > mEngineSmokesAboveLevel) && !mAfterburnerOn)
         {
            mEngineSmoking = true;
         }
      }

      // Set the returned argument values for fuel burn
      aFuelBurnRate_pps = fuelActuallyBurned_lbs / aDeltaT_sec;
      aFuelBurned_lbs   = fuelActuallyBurned_lbs;
   }

   // Standard contrailing effect within contrail altitude band
   P6DofVehicle* vehicle = GetParentVehicle();
   if (vehicle)
   {
      if (mEngineOperating && vehicle->WithinContrailAltitudeBand(aAlt_ft))
      {
         mContrailing = true;
      }
   }

   // Calc the FM
   P6DofForceAndMomentsObject thrustFM;
   CalcThrustProducerFM(effectiveThrust_lbs, thrustFM);

   // Set F&M data
   // aForceAndMoment.MoveRefPoint_ft(mThrustLocation_ft);
   // aForceAndMoment.ClearForcesAndMoments();
   // aForceAndMoment.AddForceAndMomentAtReferencePoint(force_lbs, moment_ftlbs);
   aForceAndMoment = thrustFM;

   // Set member attributes if we are "updating"
   if (aUpdateData)
   {
      mLastThrottleLeverPosition = currentEffectiveThrottleLeverPosition;

      // Limit last throttle lever limits
      if (mAfterburnerPresent)
      {
         P6DofUtils::LimitThrottleAfterburner(mLastThrottleLeverPosition);
      }
      else
      {
         P6DofUtils::LimitThrottleNormalized(mLastThrottleLeverPosition);
      }

      mEnginePercentRPM          = 100.0 * throttle_mil_lever; // TODO - Replace this with a more realistic RPM
      mNozzlePosition            = throttle_ab_lever; // TODO - Replace this with a more realistic nozzle position
      mCurrentThrustFM_lbs_ftlbs = aForceAndMoment;
      mCurrentFuelBurnRate_pph   = aFuelBurnRate_pps * 3600.0;
   }
}

void P6DofJetEngineObject::SetFuelTank(const std::string& aFuelTankName)
{
   mCurrentFuelTank = mParentPropulsionSystemPtr->GetParentVehicle()->GetFuelTankByName(aFuelTankName);

   if (mCurrentFuelTank != nullptr)
   {
      bool flowPathIntact = mCurrentFuelTank->FuelFlowPathIntact(mParentPropulsionSystemPtr);
      if (!flowPathIntact)
      {
         auto out =
            ut::log::error() << "Cannot achieve a fuel flow path to fuel tank in P6DofJetEngineObject::SetFuelTank().";
         out.AddNote() << "Fuel Tank: " << aFuelTankName;
      }
   }

   // Provide warning if no tank is present
   if (mCurrentFuelTank == nullptr)
   {
      auto out = ut::log::error() << "Unknown fuel tank selected in P6DofJetEngineObject::SetFuelTank().";
      out.AddNote() << "Fuel Tank: " << aFuelTankName;
   }
}

P6DofFuelTank* P6DofJetEngineObject::GetFuelTank() const
{
   return mCurrentFuelTank;
}

double P6DofJetEngineObject::GetEnginePercentRPM() const
{
   return mEnginePercentRPM;
}

double P6DofJetEngineObject::GetNozzlePosition() const
{
   return mNozzlePosition;
}

double P6DofJetEngineObject::GetFuelBurnRate_pph() const
{
   return mCurrentFuelBurnRate_pph;
}

double P6DofJetEngineObject::GetMaximumPotentialThrust_lbs(double aAlt_ft,
                                                           double, // aDynPress_lbsqft,
                                                           double, // aStatPress_lbssqft,
                                                           double, // aSpeed_fps,
                                                           double aMach,
                                                           double, // aAlpha_rad,
                                                           double) // aBeta_rad )
{
   double thrust_lbs = 0.0;

   // Check for afterburner - simple
   if (mABThrustTable != nullptr)
   {
      if (mABThrustTable->IsDefined())
      {
         thrust_lbs = mABThrustTable->Lookup(aAlt_ft);
         return thrust_lbs;
      }
   }

   // Check for afterburner - improved
   if (mABThrustMachAltTable != nullptr)
   {
      double lookupArgs[2];
      lookupArgs[0] = aMach;
      lookupArgs[1] = aAlt_ft * UtMath::cM_PER_FT;
      thrust_lbs    = mABThrustMachAltTable->Lookup(lookupArgs);
      return thrust_lbs;
   }

   // Check for afterburner - improved alternative
   if (mABThrustAltMachTable != nullptr)
   {
      double lookupArgs[2];
      lookupArgs[0] = aAlt_ft * UtMath::cM_PER_FT;
      lookupArgs[1] = aMach;
      thrust_lbs    = mABThrustAltMachTable->Lookup(lookupArgs);
      return thrust_lbs;
   }

   // Check for mil power
   if (mMilThrustTable != nullptr)
   {
      if (mMilThrustTable->IsDefined())
      {
         thrust_lbs = mMilThrustTable->Lookup(aAlt_ft);
         return thrust_lbs;
      }
   }

   // Check for mil power - improved
   if (mMilThrustMachAltTable != nullptr)
   {
      double lookupArgs[2];
      lookupArgs[0] = aMach;
      lookupArgs[1] = aAlt_ft * UtMath::cM_PER_FT;
      thrust_lbs    = mMilThrustMachAltTable->Lookup(lookupArgs);
      return thrust_lbs;
   }

   // Check for mil power - improved alternative
   if (mMilThrustAltMachTable != nullptr)
   {
      double lookupArgs[2];
      lookupArgs[0] = aAlt_ft * UtMath::cM_PER_FT;
      lookupArgs[1] = aMach;
      thrust_lbs    = mMilThrustAltMachTable->Lookup(lookupArgs);
      return thrust_lbs;
   }

   // We seem to lack tables, so return zero (the default answer)
   return thrust_lbs;
}

double P6DofJetEngineObject::GetMinimumPotentialThrust_lbs(double aAlt_ft,
                                                           double, // aDynPress_lbsqft,
                                                           double, // aStatPress_lbssqft,
                                                           double, // aSpeed_fps,
                                                           double aMach,
                                                           double, // aAlpha_rad,
                                                           double) // aBeta_rad )
{
   double thrust_lbs = 0.0;

   // Check for idle thrust - simple table
   if (mIdleThrustTable != nullptr)
   {
      if (mIdleThrustTable->IsDefined())
      {
         thrust_lbs = mIdleThrustTable->Lookup(aAlt_ft);
         return thrust_lbs;
      }
   }

   // Check for idle thrust - improved table
   if (mIdleThrustMachAltTable != nullptr)
   {
      double lookupArgs[2];
      lookupArgs[0] = aMach;
      lookupArgs[1] = aAlt_ft * UtMath::cM_PER_FT;
      thrust_lbs    = mIdleThrustMachAltTable->Lookup(lookupArgs);
      return thrust_lbs;
   }

   // Check for idle thrust - improved alternative table
   if (mIdleThrustAltMachTable != nullptr)
   {
      double lookupArgs[2];
      lookupArgs[0] = aAlt_ft * UtMath::cM_PER_FT;
      lookupArgs[1] = aMach;
      thrust_lbs    = mIdleThrustAltMachTable->Lookup(lookupArgs);
      return thrust_lbs;
   }

   // We seem to lack tables, so return zero
   return thrust_lbs;
}

void P6DofJetEngineObject::SetThrottlePosition(double aThrottleLeverPosition)
{
   mThrottleLeverPosition = aThrottleLeverPosition;

   // Enforce throttle limits
   if (mAfterburnerPresent)
   {
      P6DofUtils::LimitThrottleAfterburner(mThrottleLeverPosition);
   }
   else
   {
      P6DofUtils::LimitThrottleNormalized(mThrottleLeverPosition);
   }

   mThrottleLeverPositionSet = true;
}

double P6DofJetEngineObject::GetThrottlePosition() const
{
   if (mThrottleLeverPositionSet)
   {
      return mThrottleLeverPosition;
   }

   return 0.0;
}

void P6DofJetEngineObject::Ignite(int64_t /*aIgniteTimeInFrame_nanosec*/)
{
   if (AfterburnerIsPresent())
   {
      mThrottleLeverPosition    = 2.0;
      mThrottleLeverPositionSet = true;
   }
   else
   {
      mThrottleLeverPosition    = 1.0;
      mThrottleLeverPositionSet = true;
   }

   InjectFuel(true);
}

void P6DofJetEngineObject::Shutdown(int64_t /*aTerminateTime_nanosec = 0*/)
{
   InjectFuel(false);
}
