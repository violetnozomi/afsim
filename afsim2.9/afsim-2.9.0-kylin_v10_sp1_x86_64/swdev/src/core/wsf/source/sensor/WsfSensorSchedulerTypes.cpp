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

#include "WsfSensorSchedulerTypes.hpp"

#include <list>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfSensorScheduler.hpp"

namespace
{
//! The list of object factories
std::list<WsfSensorSchedulerTypes::FactoryPtr> sObjectFactoryList;

std::unique_ptr<WsfSensorScheduler> CreateInstance(const std::string& aTypeName)
{
   for (const auto& factoryPtr : sObjectFactoryList)
   {
      auto instancePtr = factoryPtr(aTypeName);
      if (instancePtr)
      {
         return instancePtr;
      }
   }
   return nullptr;
}
} // namespace

// =================================================================================================
//! Process scheduler type input.
//! @param aInput              The current input stream.
//! @param aSensorSchedulerPtr The pointer to the sensor scheduler.
//! @return 'true' if the current command was recognized and processed, 'false' if not recognized.
bool WsfSensorSchedulerTypes::LoadInstance(UtInput& aInput, std::unique_ptr<WsfSensorScheduler>& aSensorSchedulerPtr)
{
   bool        myCommand = false;
   std::string command(aInput.GetCommand());
   if (command == "scheduler")
   {
      myCommand = true;
      std::string schedulerType;
      aInput.ReadValue(schedulerType);
      aSensorSchedulerPtr = CreateInstance(schedulerType);
      if (aSensorSchedulerPtr == nullptr)
      {
         throw UtInput::BadValue(aInput, "Unknown scheduler type: " + schedulerType);
      }

      // To support legacy input format, the next
      // command might be a valid sensor command, or it could be a valid scheduler command.
      // If we don't see the "end_sensor_scheduler" delimiter, and the scheduler's ProcessInput
      // returns false, place the command back on the stack for input on the next pass,
      // where the sensor's ProcessInput might validate it.  If for some reason sensor commands
      // erroneously get placed inside the sensor_scheduler block, the "end_sensor_scheduler"
      // won't be recognized and will throw and error.
      bool blockRequired = false;
      bool doContinue    = true;
      do
      {
         aInput.ReadCommand(command);
         if (command == "end_scheduler")
         {
            doContinue = false;
         }
         else if (aSensorSchedulerPtr->ProcessInput(aInput))
         {
            blockRequired = ((command != "debug_scheduler") && (command != "scan_scheduling"));
         }
         else // potentially a valid sensor input
         {
            doContinue = false;
            if (!blockRequired)
            {
               aInput.PushBack(command); // let the sensor handle it on the next pass
            }
            else
            {
               // The full block was required, and this command was not recognized.
               auto out = ut::log::error() << "Missing block terminator.";
               out.AddNote() << "Expected: end_scheduler";
               throw UtInput::UnknownCommand(aInput);
            }
         }
      } while (doContinue);
   }
   return myCommand;
}

// ================================================================================================
//! Add an object factory for creating an instance from a fundamental type.
//! The factory should be added only once.
void WsfSensorSchedulerTypes::AddObjectFactory(FactoryPtr aFactoryPtr)
{
   sObjectFactoryList.push_back(std::move(aFactoryPtr));
}
