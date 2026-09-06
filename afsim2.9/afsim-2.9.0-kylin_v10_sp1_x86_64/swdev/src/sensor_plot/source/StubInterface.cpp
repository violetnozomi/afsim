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

#include "StubInterface.hpp"

#include <algorithm>
#include <iostream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfComm.hpp"
#include "WsfCommTypes.hpp"
#include "WsfMover.hpp"
#include "WsfMoverTypes.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformTypes.hpp"
#include "WsfProcessor.hpp"
#include "WsfProcessorTypes.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorTypes.hpp"
#include "WsfWeapon.hpp"
#include "WsfWeaponEffects.hpp"
#include "WsfWeaponEffectsTypes.hpp"
#include "WsfWeaponTypes.hpp"

namespace
{
template<class T>
class WsfDummyObject : public T
{
public:
   WsfDummyObject(WsfScenario& aScenario)
      : T(aScenario)
   {
   }
   T*   Clone() const override { return new WsfDummyObject<T>(*this); }
   bool ProcessInput(UtInput& aInput) override { return true; }
};
} // namespace

StubInterface::StubInterface(WsfScenario& aScenario)
{
   WsfMoverTypes::Get(aScenario).Add("WSF_DUMMY_MOVER", ut::make_unique<WsfDummyObject<WsfMover>>(aScenario));
   wsf::comm::Types::Get(aScenario).Add("WSF_DUMMY_COMM", ut::make_unique<WsfDummyObject<wsf::comm::Comm>>(aScenario));
   WsfProcessorTypes::Get(aScenario).Add("WSF_DUMMY_PROCESSOR", ut::make_unique<WsfDummyObject<WsfProcessor>>(aScenario));
   WsfSensorTypes::Get(aScenario).Add("WSF_DUMMY_SENSOR", ut::make_unique<WsfDummyObject<WsfSensor>>(aScenario));
   WsfWeaponTypes::Get(aScenario).Add("WSF_DUMMY_WEAPON", ut::make_unique<WsfDummyObject<WsfWeapon>>(aScenario));
   WsfWeaponEffectsTypes::Get(aScenario).Add("WSF_DUMMY_WEAPON_EFFECTS",
                                             ut::make_unique<WsfDummyObject<WsfWeaponEffects>>(aScenario));
}

// virtual
StubInterface::~StubInterface() {}

// virtual
bool StubInterface::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "ignore_block")
   {
      std::string name;
      aInput.ReadValue(name);
      mIgnoredBlocks.push_back(name);
   }
   else if (command == "ignore_line")
   {
      std::string name;
      aInput.ReadValue(name);
      mIgnoredLines.push_back(name);
   }
   else if (command == "ignore_word")
   {
      std::string name;
      aInput.ReadValue(name);
      mIgnoredWords.push_back(name);
   }
   else if (std::find(mIgnoredBlocks.begin(), mIgnoredBlocks.end(), command) != mIgnoredBlocks.end())
   {
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand()) // Swallow up everything in the block
      {
      }
   }
   else if (std::find(mIgnoredLines.begin(), mIgnoredLines.end(), command) != mIgnoredLines.end())
   {
      std::string line;
      aInput.ReadLine(line, false); // Read and ignore the rest of the current line
   }
   else if (std::find(mIgnoredWords.begin(), mIgnoredWords.end(), command) != mIgnoredWords.end())
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}
