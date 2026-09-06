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

#include "WsfProcessorTypes.hpp"

#include "UtMemory.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"

// Include files for built-in types
#include "WsfDirectionFinderProcessor.hpp"
#include "WsfExchangeProcessor.hpp"
#include "WsfLinkedProcessor.hpp"
#include "WsfMessageProcessor.hpp"
#include "WsfPerfectTracker.hpp"
#include "WsfTaskProcessor.hpp"
#include "WsfTrackProcessor.hpp"
#include "WsfTrackStateControllerProcessor.hpp"
#include "script/WsfScriptProcessor.hpp"
#include "script/WsfScriptStateMachineProcessor.hpp"

namespace
{
//! Component factory to process platform input.
class ProcessorComponentFactory : public WsfComponentFactory<WsfPlatform>
{
public:
   bool ProcessAddOrEditCommand(UtInput& aInput, WsfPlatform& aPlatform, bool aIsAdding) override
   {
      WsfProcessorTypes& types(WsfProcessorTypes::Get(GetScenario()));
      bool result = types.LoadNamedComponent(aInput, aPlatform, aIsAdding, cCOMPONENT_ROLE<WsfProcessor>());
      if (result && aIsAdding)
      {
         auto procCount = aPlatform.GetComponentCount<WsfProcessor>();
         auto proc      = aPlatform.GetComponentEntryByRole(cCOMPONENT_ROLE<WsfProcessor>(), procCount - 1);
         if (static_cast<WsfProcessor*>(proc)->GetType() == "WSF_TRACK_MANAGER")
         {
            auto out = ut::log::warning() << "WSF_TRACK_MANAGER is deprecated and will be removed in a future release."
                                          << " Use WSF_TRACK_PROCESSOR instead.";
            out.AddNote() << "Location: " << aInput.GetLocation();
         }
      }

      return result;
   }

   bool ProcessDeleteCommand(UtInput& aInput, WsfPlatform& aPlatform) override
   {
      WsfProcessorTypes& types(WsfProcessorTypes::Get(GetScenario()));
      return types.DeleteNamedComponent(aInput, aPlatform, cCOMPONENT_ROLE<WsfProcessor>());
   }
};
} // namespace

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfProcessorTypes& WsfProcessorTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetProcessorTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfProcessorTypes& WsfProcessorTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetProcessorTypes();
}

// =================================================================================================
WsfProcessorTypes::WsfProcessorTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfProcessor>(aScenario, "processor")
{
   aScenario.RegisterComponentFactory(ut::make_unique<ProcessorComponentFactory>());

   // Create 'type' entries for the built-in types.
   AddCoreType("WSF_DELAY_PROCESSOR", ut::make_unique<WsfMessageProcessor>(aScenario)); // For compatibility
   AddCoreType("WSF_DIRECTION_FINDER_PROCESSOR", ut::make_unique<WsfDirectionFinderProcessor>(aScenario));
   AddCoreType("WSF_EXCHANGE_PROCESSOR", ut::make_unique<WsfExchangeProcessor>(aScenario));
   AddCoreType("WSF_LINKED_PROCESSOR", ut::make_unique<WsfLinkedProcessor>(aScenario));
   AddCoreType("WSF_LINKED_SCRIPT_PROCESSOR", ut::make_unique<WsfScriptProcessor>(aScenario)); // For compatibility
   AddCoreType("WSF_MESSAGE_PROCESSOR", ut::make_unique<WsfMessageProcessor>(aScenario));
   AddCoreType("WSF_PERFECT_TRACKER", ut::make_unique<WsfPerfectTracker>(aScenario));
   AddCoreType("WSF_SCRIPT_PROCESSOR", ut::make_unique<WsfScriptProcessor>(aScenario));
   AddCoreType("WSF_STATE_MACHINE", ut::make_unique<WsfScriptStateMachineProcessor>(aScenario));
   AddCoreType("WSF_TASK_PROCESSOR", ut::make_unique<WsfTaskProcessor>(aScenario));
   AddCoreType("WSF_TRACK_MANAGER", ut::make_unique<WsfTrackProcessor>(aScenario)); // For compatibility
   AddCoreType("WSF_TRACK_PROCESSOR", ut::make_unique<WsfTrackProcessor>(aScenario));
   AddCoreType("WSF_TRACK_STATE_CONTROLLER", ut::make_unique<WsfTrackStateControllerProcessor>(aScenario));
}

// =================================================================================================
bool WsfProcessorTypes::DeferredLoadTypeP(UtInput& aInput, WsfObject*& aLoadedObjectPtr)
{
   bool result = LoadTypeP(aInput, aLoadedObjectPtr);
   if (result && aLoadedObjectPtr->GetBaseType() == "WSF_TRACK_MANAGER")
   {
      auto out = ut::log::warning() << "WSF_TRACK_MANAGER is deprecated and will be removed in a future release."
                                    << " Use WSF_TRACK_PROCESSOR instead.";
      out.AddNote() << "Location: " << aInput.GetLocation();
   }
   return result;
}
