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

#include "script/WsfScriptEvent.hpp"

#include <algorithm>

#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptManager.hpp"
#include "script/WsfScriptProcessor.hpp"

// =================================================================================================
//! Create an event to run a script at a specified time from a specified platform context.
//! @param aSimTime     The simulation time when the script is to be executed.
//! @param aPlatformPtr The platform containing the context containing the script to execute.
//! @param aScriptName  The name (ID) of the script to execute.
//! @param aScriptArgs  The script arguments.
WsfScriptEvent::WsfScriptEvent(double                  aSimTime,
                               WsfPlatform*            aPlatformPtr,
                               WsfStringId             aScriptName,
                               const UtScriptDataList& aScriptArgs)
   : WsfEvent()
   , mPlatformIndex(aPlatformPtr->GetIndex())
   , mProcessorNameId(nullptr)
   , mScriptName(aScriptName)
   , mScriptArgs(aScriptArgs)
{
   double simTime = std::max(aSimTime, aPlatformPtr->GetSimulation()->GetSimTime()); // Don't allow scheduling in the past
   SetTime(simTime);
}

// =================================================================================================
//! Create an event to run a script at a specified time from a specified processor context.
//! @param aSimTime      The simulation time when the script is to be executed.
//! @param aProcessorPtr The platform containing the context containing the script to execute.
//! @param aScriptName   The name (ID) of the script to execute.
//! @param aScriptArgs   The script arguments.
WsfScriptEvent::WsfScriptEvent(double                  aSimTime,
                               WsfScriptProcessor*     aProcessorPtr,
                               WsfStringId             aScriptName,
                               const UtScriptDataList& aScriptArgs)
   : WsfEvent()
   , mPlatformIndex(aProcessorPtr->GetPlatform()->GetIndex())
   , mProcessorNameId(aProcessorPtr->GetNameId())
   , mScriptName(aScriptName)
   , mScriptArgs(aScriptArgs)
{
   double simTime = std::max(aSimTime, aProcessorPtr->GetSimulation()->GetSimTime()); // Don't allow scheduling in the past
   SetTime(simTime);
}

// =================================================================================================
//! Create an event to run a script at a specified in the simulation context.
WsfScriptEvent::WsfScriptEvent(double                  aSimTime,
                               WsfSimulation*          aSimulationPtr,
                               WsfStringId             aScriptName,
                               const UtScriptDataList& aScriptArgs)
   : WsfEvent()
   , mPlatformIndex(0)
   , mProcessorNameId(nullptr)
   , mScriptName(aScriptName)
   , mScriptArgs(aScriptArgs)
{
   double simTime = std::max(aSimTime, aSimulationPtr->GetSimTime()); // Don't allow scheduling in the past
   SetTime(simTime);
}

// =================================================================================================
// virtual
WsfEvent::EventDisposition WsfScriptEvent::Execute()
{
   UtScriptData returnValue;
   if (mPlatformIndex != 0)
   {
      // Execute the script in a platform or processor context
      WsfPlatform* platformPtr = GetSimulation()->GetPlatformByIndex(mPlatformIndex);
      if (platformPtr != nullptr)
      {
         if (mProcessorNameId == nullptr)
         {
            // Execute the script in the platform context
            platformPtr->ExecuteScript(GetTime(), mScriptName, returnValue, mScriptArgs);
         }
         else
         {
            // Execute the script in the processor context
            WsfScriptProcessor* processorPtr =
               static_cast<WsfScriptProcessor*>(platformPtr->GetComponent<WsfProcessor>(mProcessorNameId));
            if (processorPtr != nullptr)
            {
               processorPtr->ExecuteScript(GetTime(), mScriptName, returnValue, mScriptArgs);
            }
         }
      }
   }
   else
   {
      // Execute the script in the simulation context
      GetSimulation()->GetScriptContext().ExecuteScript(GetTime(), mScriptName, returnValue, mScriptArgs);
   }
   return cDELETE;
}
