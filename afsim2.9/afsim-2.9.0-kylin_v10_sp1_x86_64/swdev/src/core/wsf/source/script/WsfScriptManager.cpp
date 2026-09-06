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

#include "WsfScriptManager.hpp"

#include <string>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "UtScriptContext.hpp"
#include "UtScriptCore.hpp"
#include "UtScriptStruct.hpp"
#include "UtScriptTypes.hpp"
#include "WsfAdvancedBehaviorTree.hpp"
#include "WsfAdvancedBehaviorTreeNode.hpp"
#include "WsfAssociationMessage.hpp"
#include "WsfBehaviorTreeNode.hpp"
#include "WsfCommGraph.hpp"
#include "WsfCommMedium.hpp"
#include "WsfCommMediumGuided.hpp"
#include "WsfCommMediumMessageStatus.hpp"
#include "WsfCommMediumUnguided.hpp"
#include "WsfCommMessage.hpp"
#include "WsfCommNetwork.hpp"
#include "WsfCommNetworkAdHoc.hpp"
#include "WsfCommNetworkGeneric.hpp"
#include "WsfCommNetworkMesh.hpp"
#include "WsfCommNetworkPointToPoint.hpp"
#include "WsfCommNetworkRing.hpp"
#include "WsfCommNetworkStar.hpp"
#include "WsfCommProtocolIGMP.hpp"
#include "WsfCommProtocolInterface.hpp"
#include "WsfCommProtocolLegacy.hpp"
#include "WsfCommandChain.hpp"
#include "WsfControlMessage.hpp"
#include "WsfCorrelationStrategy.hpp"
#include "WsfDraw.hpp"
#include "WsfEventPipe.hpp"
#include "WsfExchangeProcessor.hpp"
#include "WsfImage.hpp"
#include "WsfImageMessage.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfLocalTrackStatus.hpp"
#include "WsfNavigationMesh.hpp"
#include "WsfPassiveSensor.hpp"
#include "WsfPathFinder.hpp"
#include "WsfRadarSignature.hpp"
#include "WsfRandom.hpp"
#include "WsfRandomVariable.hpp"
#include "WsfRoute.hpp"
#include "WsfRouteFinder.hpp"
#include "WsfRouteNetwork.hpp"
#include "WsfSensorProcessor.hpp"
#include "WsfSensorResult.hpp"
#include "WsfStatusMessage.hpp"
#include "WsfStringId.hpp"
#include "WsfTaskAssignMessage.hpp"
#include "WsfTaskCancelMessage.hpp"
#include "WsfTaskControlMessage.hpp"
#include "WsfTaskStatusMessage.hpp"
#include "WsfTrackDropMessage.hpp"
#include "WsfTrackId.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackMessage.hpp"
#include "WsfTrackNotifyMessage.hpp"
#include "WsfVideoMessage.hpp"
#include "WsfVisualization.hpp"
#include "WsfZoneRouteFinder.hpp"
#include "script/WsfScriptAntennaPatternClass.hpp"
#include "script/WsfScriptCircularFieldOfViewClass.hpp"
#include "script/WsfScriptCommAddress.hpp"
#include "script/WsfScriptCommClass.hpp"
#include "script/WsfScriptCommRouterClass.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptCovarianceClass.hpp"
#include "script/WsfScriptDateTimeClass.hpp"
#include "script/WsfScriptEM_AntennaClass.hpp"
#include "script/WsfScriptEM_AttenuationClass.hpp"
#include "script/WsfScriptEM_InteractionClass.hpp"
#include "script/WsfScriptEM_PropagationClass.hpp"
#include "script/WsfScriptEM_RcvrClass.hpp"
#include "script/WsfScriptEM_XmtrClass.hpp"
#include "script/WsfScriptEM_XmtrRcvrClass.hpp"
#include "script/WsfScriptEquatorialFieldOfViewClass.hpp"
#include "script/WsfScriptExchangeClasses.hpp"
#include "script/WsfScriptFieldOfViewClass.hpp"
#include "script/WsfScriptFuelClass.hpp"
#include "script/WsfScriptGeoPointClass.hpp"
#include "script/WsfScriptGroupClass.hpp"
#include "script/WsfScriptLocalTrackListClass.hpp"
#include "script/WsfScriptMessageClass.hpp"
#include "script/WsfScriptMoverClass.hpp"
#include "script/WsfScriptObjectClass.hpp"
#include "script/WsfScriptPlatformClass.hpp"
#include "script/WsfScriptPlatformListClass.hpp"
#include "script/WsfScriptPolygonalFieldOfViewClass.hpp"
#include "script/WsfScriptProcessorClass.hpp"
#include "script/WsfScriptRectangularFieldOfViewClass.hpp"
#include "script/WsfScriptSensorClass.hpp"
#include "script/WsfScriptSetupClass.hpp"
#include "script/WsfScriptSimulationClass.hpp"
#include "script/WsfScriptStateMachine.hpp"
#include "script/WsfScriptTaskManagerClass.hpp"
#include "script/WsfScriptTaskProcessorClass.hpp"
#include "script/WsfScriptTerrainClass.hpp"
#include "script/WsfScriptTrackClass.hpp"
#include "script/WsfScriptTrackListClass.hpp"
#include "script/WsfScriptTrackProcessorClass.hpp"
#include "script/WsfScriptVisualPartClass.hpp"
#include "script/WsfScriptZoneClass.hpp"

UtScriptTypes* WsfScriptManager::mTypesPtr = nullptr;

//! Register all of the script types that are part of the core.
void WsfScriptManager::RegisterTypes(UtScriptTypes* aTypePtr)
{
   mTypesPtr = aTypePtr;
   mTypesPtr->Register(WsfAssociationMessage::CreateScriptClass("WsfAssociationMessage", mTypesPtr));
   mTypesPtr->Register(WsfCommandChain::CreateScriptClass("WsfCommandChain", mTypesPtr));
   mTypesPtr->Register(WsfControlMessage::CreateScriptClass("WsfControlMessage", mTypesPtr));
   mTypesPtr->Register(WsfEventPipeInterface::CreateScriptClass("WsfEventPipe", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptCorrelationClass>("WsfCorrelator", mTypesPtr));
   mTypesPtr->Register(WsfDraw::CreateScriptClass("WsfDraw", mTypesPtr));
   mTypesPtr->Register(WsfImage::CreateScriptClass("WsfImage", mTypesPtr));
   mTypesPtr->Register(WsfImageMessage::CreateScriptClass("WsfImageMessage", mTypesPtr));
   mTypesPtr->Register(WsfPassiveSensor::CreateScriptClass("WsfPassiveSensor", mTypesPtr));
   mTypesPtr->Register(WsfLocalTrack::CreateScriptClass("WsfLocalTrack", mTypesPtr));
   mTypesPtr->Register(WsfLocalTrackStatus::CreateScriptClass("WsfLocalTrackStatus", mTypesPtr));
   mTypesPtr->Register(WsfRoute::CreateScriptClass("WsfRoute", mTypesPtr));
   mTypesPtr->Register(WsfRouteFinder::CreateScriptClass("WsfRouteFinder", mTypesPtr));
   mTypesPtr->Register(WsfZoneRouteFinder::CreateScriptClass("WsfZoneRouteFinder", mTypesPtr));
   mTypesPtr->Register(WsfRoute::CreateIteratorScriptClass("WsfRouteIterator", mTypesPtr));
   mTypesPtr->Register(WsfRouteNetwork::CreateScriptClass("WsfRouteNetwork", mTypesPtr));
   mTypesPtr->Register(WsfScriptStateMachine::CreateScriptClass("WsfStateMachine", mTypesPtr));
   mTypesPtr->Register(WsfStatusMessage::CreateScriptClass("WsfStatusMessage", mTypesPtr));

   mTypesPtr->Register(WsfTaskAssignMessage::CreateScriptClass("WsfTaskAssignMessage", mTypesPtr));
   mTypesPtr->Register(WsfTaskCancelMessage::CreateScriptClass("WsfTaskCancelMessage", mTypesPtr));
   mTypesPtr->Register(WsfTaskControlMessage::CreateScriptClass("WsfTaskControlMessage", mTypesPtr));
   mTypesPtr->Register(WsfTaskStatusMessage::CreateScriptClass("WsfTaskStatusMessage", mTypesPtr));
   mTypesPtr->Register(WsfTrackDropMessage::CreateScriptClass("WsfTrackDropMessage", mTypesPtr));
   mTypesPtr->Register(WsfTrackId::CreateScriptClass("WsfTrackId", mTypesPtr));
   mTypesPtr->Register(WsfTrackManager::CreateScriptClass("WsfTrackManager", mTypesPtr));
   mTypesPtr->Register(WsfTrackMessage::CreateScriptClass("WsfTrackMessage", mTypesPtr));
   mTypesPtr->Register(WsfTrackNotifyMessage::CreateScriptClass("WsfTrackNotifyMessage", mTypesPtr));
   mTypesPtr->Register(WsfVideoMessage::CreateScriptClass("WsfVideoMessage", mTypesPtr));
   mTypesPtr->Register(WsfVisualization::CreateScriptClass(mTypesPtr));
   mTypesPtr->Register(WsfWaypoint::CreateScriptClass("WsfWaypoint", mTypesPtr));

   mTypesPtr->Register(ut::make_unique<WsfScriptContainerClass>("Container", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptQueryClass>("Query", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptTenderClass>("Tender", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptTransactorClass>("Transactor", mTypesPtr));

   mTypesPtr->Register(ut::make_unique<WsfScriptSetupClass>("WsfScenario", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptArticulatedPartClass>("WsfArticulatedPart", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptBehaviorTreeNodeClass>("WsfBehaviorTreeNode", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptAdvancedBehaviorTreeNodeClass>("WsfAdvancedBehaviorTreeNode", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptAdvancedBehaviorTreeClass>("WsfAdvancedBehaviorTree", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptCommInteractionClass>("WsfCommInteraction", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptCovarianceClass>("WsfCovariance", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptDateTimeClass>("WsfDateTime", mTypesPtr));

   mTypesPtr->Register(ut::make_unique<WsfScriptAntennaPatternClass>("WsfAntennaPattern", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptEM_AntennaClass>("WsfEM_Antenna", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptEM_AttenuationClass>("WsfEM_Attenuation", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptEM_InteractionClass>("WsfEM_Interaction", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptEM_PropagationClass>("WsfEM_Propagation", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptEM_XmtrRcvrClass>("WsfEM_XmtrRcvr", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptEM_RcvrClass>("WsfEM_Rcvr", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptEM_XmtrClass>("WsfEM_Xmtr", mTypesPtr));

   mTypesPtr->Register(ut::make_unique<WsfScriptExchangeProcessorClass>("WsfExchangeProcessor", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptFuelClass>("WsfFuel", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptGeoPointClass>("WsfGeoPoint", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptGroupClass>("WsfGroup", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptLocalTrackListClass>("WsfLocalTrackList", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptLocalTrackListIteratorClass>("WsfLocalTrackListIterator", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptMessageClass>("WsfMessage", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptMoverClass>("WsfMover", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptNavigationMeshClass>("WsfNavigationMesh", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptObjectClass>("WsfObject", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptPathFinderClass>("WsfPathFinder", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptPlatformClass>("WsfPlatform", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptPlatformListClass>("WsfPlatformList", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptPlatformListIteratorClass>("WsfPlatformListIterator", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptPlatformPartClass>("WsfPlatformPart", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptProcessorClass>("WsfProcessor", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptProcessorClass>("WsfTrackStateController", mTypesPtr));

   mTypesPtr->Register(ut::make_unique<WsfScriptSensorClass>("WsfSensor", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptSensorInteractionClass>("WsfSensorInteraction", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptSimulationClass>("WsfSimulation", mTypesPtr));
   WsfTask::RegisterScriptTypes(mTypesPtr);
   mTypesPtr->Register(ut::make_unique<WsfScriptTaskManagerClass>("WsfTaskManager", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptTaskProcessorClass>("WsfTaskProcessor", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptTerrainClass>("WsfTerrain", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptTrackClass>("WsfTrack", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptTrackListClass>("WsfTrackList", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptTrackListIteratorClass>("WsfTrackListIterator", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptTrackProcessorClass>("WsfTrackProcessor", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptVisualPartClass>("WsfVisualPart", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptZoneClass>("WsfZone", mTypesPtr));

   mTypesPtr->Register(ut::make_unique<wsf::script::RandomClass>("WsfRandom", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptRandomVariableClass>("WsfRandomVariable", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptCommClass>("WsfComm", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptCommRouterClass>("WsfCommRouter", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptCommAddressClass>("WsfAddress", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<wsf::comm::ScriptMessageClass>("WsfCommMessage", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<wsf::comm::ScriptNetworkClass>("WsfNetwork", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<wsf::comm::ScriptNetworkAdHocClass>("WsfNetworkAdHoc", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<wsf::comm::ScriptNetworkGenericClass>("WsfNetworkGeneric", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<wsf::comm::ScriptNetworkMeshClass>("WsfNetworkMesh", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<wsf::comm::ScriptNetworkMeshLegacyClass>("WsfNetworkMeshLegacy", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<wsf::comm::ScriptNetworkPointToPointClass>("WsfNetworkP2P", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<wsf::comm::ScriptNetworkRingClass>("WsfNetworkRing", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<wsf::comm::ScriptNetworkDirectedRingClass>("WsfNetworkDirectedRing", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<wsf::comm::ScriptNetworkStarClass>("WsfNetworkStar", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<wsf::comm::ScriptProtocolClass>("WsfCommProtocol", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<wsf::comm::ScriptProtocolClassIGMP>("WsfCommProtocolIGMP", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<wsf::comm::graph::ScriptCommGraphNodeClass>("WsfCommGraphNode", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<wsf::comm::graph::ScriptCommGraphEdgeClass>("WsfCommGraphEdge", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<wsf::comm::graph::ScriptCommGraphClass>("WsfCommGraph", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<wsf::comm::medium::ScriptMediumClass>("WsfCommMedium", mTypesPtr));
   mTypesPtr->Register(
      ut::make_unique<wsf::comm::medium::ScriptMediumMessageStatusClass>("WsfCommMediumMessageStatus", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<wsf::comm::medium::ScriptMediumModeClass>("WsfCommMediumMode", mTypesPtr));
   mTypesPtr->Register(
      ut::make_unique<wsf::comm::medium::ScriptMediumModeGuidedClass>("WsfCommMediumModeGuided", mTypesPtr));
   mTypesPtr->Register(
      ut::make_unique<wsf::comm::medium::ScriptMediumModeUnguidedClass>("WsfCommMediumModeUnguided", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<wsf::comm::router::ScriptProtocolClassLegacy>("WsfCommProtocolLegacy", mTypesPtr));

   mTypesPtr->Register(ut::make_unique<WsfScriptFieldOfViewClass>("WsfFieldOfView", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptCircularFieldOfViewClass>("WsfCircularFieldOfView", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptEquatorialFieldOfViewClass>("WsfEquatorialFieldOfView", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptPolygonalFieldOfViewClass>("WsfPolygonalFieldOfView", mTypesPtr));
   mTypesPtr->Register(ut::make_unique<WsfScriptRectangularFieldOfViewClass>("WsfRectangularFieldOfView", mTypesPtr));

   // Register the signatures that are included with the core.
   // This must be done after the WsfPlatform script class is created because they add to it.
   WsfRadarSignature::RegisterScriptMethods(*mTypesPtr);

   // Register the sensor processor that are included with the core.
   // This must be done after the WsfProcessor script class is created because they add to it.
   WsfSensorProcessor::RegisterScriptMethods(*mTypesPtr);
}

WsfScriptManager::WsfScriptManager(const WsfScriptManager& aSrc)
   : mContextPtr(nullptr)
{
   mContextPtr = new WsfScriptContext(*aSrc.mContextPtr);
}

WsfScriptManager::WsfScriptManager(WsfScenario* aScenarioPtr, UtScriptEnvironment* aScriptEnvironmentPtr)
   : mContextPtr(nullptr)
{
   assert(mTypesPtr != nullptr);
   // This is not done in the initializer list since it relies on mTypesPtr.
   mContextPtr = new WsfScriptContext(aScriptEnvironmentPtr);
   mContextPtr->GetContext().Var("__SCENARIO").GetPointer()->SetAppObject(aScenarioPtr);
}

WsfScriptManager::~WsfScriptManager()
{
   delete mContextPtr;
}

//
// //! Return the 'compile_listing' flag for the global context.
// bool WsfScriptManager::ScriptListingEnabled() const
// {
//    return mContextPtr->ScriptListingEnabled();
// }

//! Return the 'debug_writes' flags for the global context.
bool WsfScriptManager::ScriptWritesEnabled() const
{
   return mContextPtr->GetContext().WritesEnabled();
}

bool WsfScriptManager::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (mContextPtr->ProcessInput(aInput))
   {
   }
   else if (command == "script_interface")
   {
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (mContextPtr->ProcessInput(aInput))
         {
         }
         else if ((command == "silent") || (command == "silent_mode"))
         {
            bool silentModeEnabled;
            aInput.ReadValue(silentModeEnabled);
            mContextPtr->GetContext().SetWritesEnabled(!silentModeEnabled);
         }
         else if ((command == "listing") || (command == "compile_listing"))
         {
            bool scriptListingEnabled;
            aInput.ReadValue(scriptListingEnabled);
            // deprecated
         }
         else if (command == "debug")
         {
            // No change
         }
         else if (command == "debug_writes")
         {
            bool scriptDebugWrites;
            aInput.ReadValue(scriptDebugWrites);
            mContextPtr->GetContext().SetWritesEnabled(scriptDebugWrites);
         }
         else if (command == "no_debug")
         {
            // No change
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (command == "script_abort_on_null_references")
   {
      bool doAbort;
      aInput.ReadValue(doAbort);
      UtScriptClass::InterfaceMethod::SetAbortOnScriptCallErrors(doAbort);
   }
   else if (command == "script_abort_on_assert")
   {
      aInput.ReadValue(UtScriptCore::sAbortOnAssert);
   }
   else if (UtScriptStruct::ProcessInput(aInput, mContextPtr->GetEnvironment()))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

bool WsfScriptManager::Add(std::unique_ptr<UtScriptClass> aClassPtr)
{
   bool ok = true;
   ok &= mTypesPtr->Register(std::move(aClassPtr));
   return ok;
}

//! Execute a globally defined script with the specified string id.
//! @returns true if successful or false if not (script doesn't exist).
bool WsfScriptManager::ExecuteScript(double aSimTime, WsfStringId aScriptId)
{
   return mContextPtr->ExecuteScript(aSimTime, aScriptId);
}

//! Provides access to the types that have been registered with the script manager.
//! @returns a reference to the UtScriptTypes object associated with the script manager.
UtScriptTypes& WsfScriptManager::GetTypes()
{
   return *mTypesPtr;
}

//! Provides access to the script manager's script context.
//! @returns a reference to the WsfScriptContext object associated with the script manager.
WsfScriptContext& WsfScriptManager::GetContext()
{
   return *mContextPtr;
}
