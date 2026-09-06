// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "SOSM_Target.hpp"

#include <memory>

#include "SOSM_Interaction.hpp"
#include "SOSM_Manager.hpp"
#include "UtException.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"

// =================================================================================================
//! Constructor
SOSM_Target::SOSM_Target(SOSM_Manager* aManagerPtr)
   : SOSM_CoreBase()
   , mManagerPtr(aManagerPtr)
   , mSharedDataPtr(new SharedData())
   , mAtmosphere(aManagerPtr->GetUtAtmosphere())
   , mMutex()
{
}

// =================================================================================================
//! Copy constructor
SOSM_Target::SOSM_Target(const SOSM_Target& aSrc)
   : SOSM_CoreBase(aSrc)
   , mManagerPtr(aSrc.mManagerPtr)
   , mSharedDataPtr(aSrc.mSharedDataPtr)
   , mAtmosphere(aSrc.mAtmosphere)
   , mMutex()
{
}

// =================================================================================================
// virtual
bool SOSM_Target::Initialize()
{
   return true;
}

// =================================================================================================
// virtual
void SOSM_Target::InputComplete()
{
   // Ensure that at least one state has been defined.
   if (mSharedDataPtr->mStates.empty())
   {
      throw UtException("No target states have been defined");
   }
}

// =================================================================================================
// virtual
bool SOSM_Target::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "state")
   {
      std::string stateName;
      aInput.ReadValue(stateName);

      // Ensure the state hasn't already been defined.
      for (std::vector<StateBase*>::const_iterator sli = mSharedDataPtr->mStates.begin();
           sli != mSharedDataPtr->mStates.end();
           ++sli)
      {
         if (stateName == (*sli)->mStateName)
         {
            throw UtInput::BadValue(aInput, "Duplicate state name: " + stateName);
         }
      }

      // Create and populate the state.
      std::unique_ptr<StateBase> statePtr(CreateState(stateName));
      UtInputBlock               inputBlock(aInput);
      while (inputBlock.ReadCommand())
      {
         if (!statePtr->ProcessInput(aInput, *this))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      statePtr->InputComplete(*this);
      mSharedDataPtr->mStates.push_back(statePtr.release());
   }
   else if (mAtmosphere.ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
//! Select a state using the current target state from an interaction object.
size_t SOSM_Target::SelectState(SOSM_Interaction& aInteraction) const
{
   size_t stateCount = mSharedDataPtr->mStates.size();
   if (stateCount == 0)
   {
      return 0; // Just for safety... InputComplete verifies we have at least one state
   }

   // Select the default state if none found.
   size_t newStateIndex = stateCount - 1;

   float altitude = aInteraction.GetTargetAltitude();
   float speed    = aInteraction.GetTargetSpeed();
   float throttle = aInteraction.GetTargetThrottle();
   mMutex.lock(); // UtAtmosphere is not thread-safe
   float mach = static_cast<float>(speed / mAtmosphere.SonicVelocity(altitude));
   mMutex.unlock();

   for (size_t stateIndex = 0; stateIndex < stateCount; ++stateIndex)
   {
      const StateBase& state = *(mSharedDataPtr->mStates[stateIndex]);

      // select either Mach or speed for the speed comparison.
      float speedSpec = state.mSpeedIsMach ? mach : speed;

      if ((altitude >= state.mMinAltitude) && (altitude <= state.mMaxAltitude) && (speedSpec >= state.mMinSpeed) &&
          (speedSpec <= state.mMaxSpeed) && (throttle >= state.mMinThrottle) && (throttle <= state.mMaxThrottle))
      {
         newStateIndex = stateIndex;
         break;
      }
   }
   aInteraction.mTargetStateIndex = newStateIndex;
   return newStateIndex;
}

// =================================================================================================
SOSM_Target::StateBase::StateBase(const std::string& aStateName)
   : SOSM_SpectralObject()
   , mStateName(aStateName)
   , mMinAltitude(-1.0E+30F)
   , mMaxAltitude(1.0E+30F)
   , mMinSpeed(-1.0E+30F)
   , mMaxSpeed(1.0E+30F)
   , mMinThrottle(0.0F)
   , mMaxThrottle(1.0F)
   , mSpeedIsMach(false)
{
}

// =================================================================================================
// virtual
SOSM_Target::StateBase::~StateBase() {}

// =================================================================================================
void SOSM_Target::StateBase::InputComplete(SOSM_Target& aTarget) {}

// =================================================================================================
bool SOSM_Target::StateBase::ProcessInput(UtInput& aInput, SOSM_Target& aTarget)
{
   std::string command(aInput.GetCommand());
   bool        myCommand = true;
   if (command == "altitude_range")
   {
      aInput.ReadValueOfType(mMinAltitude, UtInput::cLENGTH);
      aInput.ReadValueOfType(mMaxAltitude, UtInput::cLENGTH);
      aInput.ValueGreater(mMaxAltitude, mMinAltitude);
   }
   else if (command == "mach_range")
   {
      aInput.ReadValue(mMinSpeed);
      aInput.ReadValue(mMaxSpeed);
      aInput.ValueGreaterOrEqual(mMinSpeed, 0.0F);
      aInput.ValueGreaterOrEqual(mMaxSpeed, mMinSpeed);
      mSpeedIsMach = true;
   }
   else if (command == "speed_range")
   {
      aInput.ReadValueOfType(mMinSpeed, UtInput::cSPEED);
      aInput.ReadValueOfType(mMaxSpeed, UtInput::cSPEED);
      aInput.ValueGreaterOrEqual(mMinSpeed, 0.0F);
      aInput.ValueGreaterOrEqual(mMaxSpeed, mMinSpeed);
      mSpeedIsMach = false;
   }
   else if (command == "throttle_range")
   {
      aInput.ReadValue(mMinThrottle);
      aInput.ReadValue(mMaxThrottle);
      aInput.ValueGreaterOrEqual(mMinThrottle, 0.0F);
      aInput.ValueInClosedRange(mMaxThrottle, mMinThrottle, 1.0F);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
// virtual
SOSM_Target::SharedData::~SharedData()
{
   for (std::vector<StateBase*>::iterator sli = mStates.begin(); sli != mStates.end(); ++sli)
   {
      delete *sli;
   }
}
