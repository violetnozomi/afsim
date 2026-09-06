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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "InputConfig.hpp"

#include <iostream>

#include "Simulation.hpp"
#include "SimulationExtension.hpp"
#include "UtException.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfScenario.hpp"

namespace engage
{

// =================================================================================================
InputConfig::InputConfig()
   : mThreadCount(0)
   , mTaskCount(0)
   , mVariables()
   , mOutputTemplates()
   , mOutputRates()
   , mRunList()
{
   // Create the default output rate table...
   OutputRate outputRate;
   outputRate.Append(0.0, 0.5);
   outputRate.Append(1000.0, 0.5);
   mOutputRates["default"] = outputRate;
}

// =================================================================================================
// virtual
InputConfig::~InputConfig() {}

// =================================================================================================
//! Given a scenario, find the input data extension.
// static
InputConfig& InputConfig::FindExtension(WsfScenario& aScenario)
{
   WsfScenarioExtension* extensionPtr = aScenario.FindExtension("engage");
   if (extensionPtr == nullptr)
   {
      throw UtException("couldn't find input extension");
   }
   return *dynamic_cast<InputConfig*>(extensionPtr);
}

// =================================================================================================
bool InputConfig::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());
   if (command == "output_rate")
   {
      UtInputBlock inputBlock(aInput);
      OutputRate   outputRate;
      std::string  name;
      aInput.ReadValue(name);
      outputRate.ProcessInputBlock(inputBlock);
      mOutputRates[name] = outputRate;
   }
   else if (command == "output_template")
   {
      // TODO-Add ability to define an output template.
   }
   else if (command == "run")
   {
      if (mRunList.empty())
      {
         mRunList.push_back(RunConfig(*this));
      }
      else
      {
         mRunList.push_back(RunConfig(mRunList.back()));
      }
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (!mRunList.back().ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (command == "thread_count")
   {
      aInput.ReadValue(mThreadCount);
      aInput.ValueGreaterOrEqual(mThreadCount, 1);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
// virtual
bool InputConfig::Complete()
{
   bool ok = true;

   // Compute the number of tasks that need to be executed.
   mTaskCount   = 0;
   int runIndex = 0;
   for (RunList::iterator rli = mRunList.begin(); rli != mRunList.end(); ++rli)
   {
      RunConfig& runConfig = *rli;
      if (runConfig.Complete(runIndex, mTaskCount))
      {
         mTaskCount += runConfig.GetTaskCount();
         runIndex++;
      }
      else
      {
         ok = false;
      }
   }
   std::cout << "Task count=" << mTaskCount << " Run count=" << mRunList.size() << std::endl;

   return ok;
}

// =================================================================================================
// This is called when a simulation object is created and associated with a scenario.
// virtual
void InputConfig::SimulationCreated(WsfSimulation& aSimulation)
{
   aSimulation.RegisterExtension("engage", ut::make_unique<SimulationExtension>());
}

// =================================================================================================
const OutputRate* InputConfig::GetOutputRate(const std::string& aTableName) const
{
   const OutputRate*           ratePtr(nullptr);
   OutputRates::const_iterator ori(mOutputRates.find(aTableName));
   if (ori != mOutputRates.end())
   {
      ratePtr = &((*ori).second);
   }
   return ratePtr;
}

// =================================================================================================
//! Create a 'Task' object for the specified task index.
Task InputConfig::MakeTask(int aTaskIndex) const
{
   int relTaskIndex = aTaskIndex;
   for (RunList::const_iterator rli = mRunList.begin(); rli != mRunList.end(); ++rli)
   {
      const RunConfig& runConfig(*rli);
      if (relTaskIndex < runConfig.GetTaskCount())
      {
         // Found the desired run....
         return runConfig.MakeTask(aTaskIndex);
      }
      relTaskIndex -= runConfig.GetTaskCount();
   }
   std::cout << "***** ERROR: InputConfig::MakeTask could not find run data for task " << aTaskIndex << std::endl;
   throw UtException("InputConfig::MakeTask could not find run data");
}

} // namespace engage
