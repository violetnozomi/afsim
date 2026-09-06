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

#include "WsfPerceptionProcessor.hpp"

#include <algorithm>
#include <iterator>

#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtScriptEnvironment.hpp"
#include "UtVec3.hpp"
#include "WsfAssetMessage.hpp"
#include "WsfAssetPerception.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfCommandChain.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrackList.hpp"
#include "WsfTrackListFwd.hpp"
#include "WsfTrackManager.hpp"
#include "WsfWeapon.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptMessageHandler.hpp"


WsfPerceptionProcessor::WsfPerceptionProcessor(const WsfScenario& aScenario)
   : WsfScriptProcessor(aScenario, "WsfPerceptionProcessor", "PROCESSOR")
   , mTrackManagerPtr(nullptr)
   , mHeatMapPtr(nullptr)
   , mStatusMessageReportInterval(-1.0)
   , mReportingSelfStatus(true)
   , mReportingOthersStatus(false)
   , mPerceiveSelfStatus(false)
   , mThreatUpdateInterval(-1.0)
   , mAssetUpdateInterval(-1.0)
   , mMaxThreatLoad(-1)
   , mMaxAssetLoad(-1)
   , mAssetPerception(TRUTH)
   , mTruthPerceptionMask(0)
   , mLastAssetUpdateTime(-1.0)
   , mLastThreatUpdateTime(-1.0)
   , mPerceivedThreats()
   , mPerceivedAssets()
   , mReceivedAssetStatus()
   , mImportantThreats()
   , mImportantAssets()
   , mAssetFunctionScriptContextPtr(nullptr)
   , mAssetFunctionScriptName()
   , mAssetFunctionScriptPtr(nullptr)
   , mThreatFunctionScriptContext(nullptr)
   , mThreatFunctionScriptName()
   , mThreatFunctionScriptPtr(nullptr)
{
}

WsfPerceptionProcessor::~WsfPerceptionProcessor()
{
   std::map<size_t, WsfAssetPerception*>::iterator it;
   for (it = mReceivedAssetStatus.begin(); it != mReceivedAssetStatus.end(); ++it)
   {
      delete it->second;
   }
   mReceivedAssetStatus.clear();

   std::vector<WsfAssetPerception*>::iterator pt;
   for (pt = mPerceivedAssets.begin(); pt != mPerceivedAssets.end(); ++pt)
   {
      delete (*pt);
   }
   mPerceivedAssets.clear();

   auto tt = mPerceivedThreats.begin();
   while (tt != mPerceivedThreats.end())
   {
      delete (*tt);
      tt = mPerceivedThreats.erase(tt);
   }
   delete mHeatMapPtr;
}

WsfPerceptionProcessor::WsfPerceptionProcessor(const WsfPerceptionProcessor& aSrc)
   : WsfScriptProcessor(aSrc)
   , mTrackManagerPtr(nullptr)
   , mHeatMapPtr(nullptr)
   , mStatusMessageReportInterval(aSrc.mStatusMessageReportInterval)
   , mReportingSelfStatus(aSrc.mReportingSelfStatus)
   , mReportingOthersStatus(aSrc.mReportingOthersStatus)
   , mPerceiveSelfStatus(aSrc.mPerceiveSelfStatus)
   , mThreatUpdateInterval(aSrc.mThreatUpdateInterval)
   , mAssetUpdateInterval(aSrc.mAssetUpdateInterval)
   , mMaxThreatLoad(aSrc.mMaxThreatLoad)
   , mMaxAssetLoad(aSrc.mMaxAssetLoad)
   , mAssetPerception(aSrc.mAssetPerception)
   , mTruthPerceptionMask(aSrc.mTruthPerceptionMask)
   , mLastAssetUpdateTime(aSrc.mLastAssetUpdateTime)
   , mLastThreatUpdateTime(aSrc.mLastThreatUpdateTime)
   , mPerceivedThreats(aSrc.mPerceivedThreats)
   , mPerceivedAssets(aSrc.mPerceivedAssets)
   , mReceivedAssetStatus()
   , mImportantThreats(aSrc.mImportantThreats)
   , mImportantAssets(aSrc.mImportantAssets)
   , mAssetFunctionScriptContextPtr(nullptr)
   , mAssetFunctionScriptName(aSrc.mAssetFunctionScriptName)
   , mAssetFunctionScriptPtr(nullptr)
   , mThreatFunctionScriptContext(nullptr)
   , mThreatFunctionScriptName(aSrc.mThreatFunctionScriptName)
   , mThreatFunctionScriptPtr(nullptr)
{
   mAssetFunctionScriptContextPtr = &GetScriptContext().GetContext();
   mAssetFunctionScriptPtr        = mAssetFunctionScriptContextPtr->FindScript(mAssetFunctionScriptName);
   while (mAssetFunctionScriptPtr == nullptr && mAssetFunctionScriptContextPtr->GetParent() != nullptr)
   {
      mAssetFunctionScriptContextPtr = mAssetFunctionScriptContextPtr->GetParent();
      mAssetFunctionScriptPtr        = mAssetFunctionScriptContextPtr->FindScript(mAssetFunctionScriptName);
   }

   mThreatFunctionScriptContext = &GetScriptContext().GetContext();
   mThreatFunctionScriptPtr     = mThreatFunctionScriptContext->FindScript(mThreatFunctionScriptName);
   while (mThreatFunctionScriptPtr == nullptr && mThreatFunctionScriptContext->GetParent() != nullptr)
   {
      mThreatFunctionScriptContext = mThreatFunctionScriptContext->GetParent();
      mThreatFunctionScriptPtr     = mThreatFunctionScriptContext->FindScript(mThreatFunctionScriptName);
   }

   if (aSrc.mHeatMapPtr != nullptr)
   {
      mHeatMapPtr = new WsfHeatMap(*aSrc.mHeatMapPtr);
   }
}

bool WsfPerceptionProcessor::Initialize(double aSimTime)
{
   bool ok = WsfScriptProcessor::Initialize(aSimTime);

   if (mStatusMessageReportInterval > 0 && HasExternalLinks())
   {
      // setup event to report status to commander
      GetSimulation()->AddEvent(
         ut::make_unique<WsfPerceptionProcessorUpdateEvent>(aSimTime + StatusMessageReportInterval(),
                                                            GetPlatform()->GetIndex(),
                                                            this,
                                                            WsfPerceptionProcessorUpdateEvent::STATUS));
   }

   if (mAssetUpdateInterval > 0)
   {
      GetSimulation()->AddEvent(
         ut::make_unique<WsfPerceptionProcessorUpdateEvent>(aSimTime + mAssetUpdateInterval,
                                                            GetPlatform()->GetIndex(),
                                                            this,
                                                            WsfPerceptionProcessorUpdateEvent::ASSETS));
   }

   if (mThreatUpdateInterval > 0)
   {
      GetSimulation()->AddEvent(
         ut::make_unique<WsfPerceptionProcessorUpdateEvent>(aSimTime + mThreatUpdateInterval,
                                                            GetPlatform()->GetIndex(),
                                                            this,
                                                            WsfPerceptionProcessorUpdateEvent::THREATS));
   }

   mTrackManagerPtr = &(GetPlatform()->GetTrackManager());
   mCallbacks.Add(mTrackManagerPtr->LocalTrackDropped.Connect(&WsfPerceptionProcessor::LocalTrackDropped, this));
   mCallbacks.Add(mTrackManagerPtr->TrackManagerChanged.Connect(&WsfPerceptionProcessor::TrackManagerChanged, this));

   if (mHeatMapPtr != nullptr)
   {
      // Heat map must have Update and Threat Update intervals defined as
      // it operates on perceived threats only. If no values are set
      // give a default of 5 sec.
      if (mThreatUpdateInterval <= 0)
      {
         mThreatUpdateInterval = 5.0;
         GetSimulation()->AddEvent(
            ut::make_unique<WsfPerceptionProcessorUpdateEvent>(aSimTime + mThreatUpdateInterval,
                                                               GetPlatform()->GetIndex(),
                                                               this,
                                                               WsfPerceptionProcessorUpdateEvent::THREATS));
      }
      if (GetUpdateInterval() <= 0.0)
      {
         SetUpdateInterval(5.0);
      }

      // Not checking return value from heat map init as this
      // Perception Processor may not not be using a heat map
      mHeatMapPtr->Initialize(GetPlatform());
   }

   // WsfObserver::PlatformDeleted.Connect();

   return ok;
}

void WsfPerceptionProcessor::Update(double aSimTime)
{
   // perception is event based, see: Initialize()
   // or its polling based, in the case where PERFECT perception is used, e.g. see PerceivedThreats()
   WsfScriptProcessor::Update(aSimTime); // call this for "on_update" script & for behavior tree (if either exist)

   if (mHeatMapPtr != nullptr)
   {
      // Update heat map state
      mHeatMapPtr->Update(aSimTime, GetUpdateInterval(), mPerceivedAssets);
   }
}

bool WsfPerceptionProcessor::ProcessInput(UtInput& aInput)
{
   std::string command(aInput.GetCommand());
   bool        wasProcessed = true;

   if (command == "report_interval")
   {
      aInput.ReadValueOfType(mStatusMessageReportInterval, UtInput::cTIME);
      aInput.ValueGreater(mStatusMessageReportInterval, 0.0);
   }
   else if (command == "reporting_self" || command == "reports_self")
   {
      aInput.ReadValue(mReportingSelfStatus);
   }
   else if (command == "reporting_others" || command == "reports_others")
   {
      aInput.ReadValue(mReportingOthersStatus);
   }
   else if (command == "asset_perception")
   {
      std::string pType;
      aInput.ReadValue(pType);
      if (pType == "status_messages")
      {
         mAssetPerception = STATUS_MESSAGES;
      }
      else if (pType == "truth")
      {
         mAssetPerception = TRUTH;
         std::string setString;
         aInput.ReadValue(
            setString); // colon separated lists of which sets to include in perception (commander:peers:subordinates)
         UtStringUtil::ToLower(setString); // shouldn't mess with the colons???
         std::vector<std::string> sets;
         UtStringUtil::Parse(setString, sets, ':');
         while (!sets.empty())
         {
            std::string set = sets.back();
            if (set == "commander")
            {
               mTruthPerceptionMask |= cCOMMANDER;
            }
            else if (set == "peers")
            {
               mTruthPerceptionMask |= cPEERS;
            }
            else if (set == "subordinates")
            {
               mTruthPerceptionMask |= cSUBORDINATES;
            }
            else if (set == "all_commanders")
            {
               mTruthPerceptionMask |= cALLCOMMANDERS;
            }
            else if (set == "all_subordinates")
            {
               mTruthPerceptionMask |= cALLSUBORDINATES;
            }
            sets.pop_back();
         }
      }
      else
      {
         throw UtInput::BadValue(aInput, "unknown asset_perception: " + pType);
      }
   }
   else if (command == "perceive_self")
   {
      aInput.ReadValue(mPerceiveSelfStatus);
   }
   else if (command == "threat_update_interval")
   {
      aInput.ReadValueOfType(mThreatUpdateInterval, UtInput::cTIME);
      // aInput.ValueGreaterOrEqual(mThreatUpdateInterval, 0.0);
   }
   else if (command == "asset_update_interval")
   {
      aInput.ReadValueOfType(mAssetUpdateInterval, UtInput::cTIME);
      // aInput.ValueGreaterOrEqual(mAssetUpdateInterval, 0.0);
   }
   else if (command == "max_threat_load")
   {
      aInput.ReadValue(mMaxThreatLoad);
   }
   else if (command == "max_asset_load")
   {
      aInput.ReadValue(mMaxAssetLoad);
   }
   else if (command == "threat_importance_function")
   {
      std::string functionName;
      aInput.ReadValue(functionName);
      SetScriptThreatScoringFunction(&GetScriptContext().GetContext(), functionName);
   }
   else if (command == "asset_importance_function")
   {
      std::string functionName;
      aInput.ReadValue(functionName);
      SetScriptAssetScoringFunction(&GetScriptContext().GetContext(), functionName);
   }
   else if (command == "heat_map")
   {
      if (mHeatMapPtr == nullptr)
      {
         mHeatMapPtr = new WsfHeatMap();
      }
      mHeatMapPtr->ProcessInput(aInput);
   }
   else if (WsfScriptProcessor::ProcessInput(aInput))
   {
      // external_link / reports_to
   }
   else
   {
      wasProcessed = false;
   }

   return wasProcessed;
}

// -------------------------------------------------------------------------------------------------
//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
UtScriptClass* WsfPerceptionProcessor::CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
{
   return new WsfScriptPerceptionProcessor(aClassName, aScriptTypesPtr);
}

WsfPerceptionProcessor* WsfPerceptionProcessor::CreatePerceptionProcessor(double aSimTime, WsfPlatform& aPlatform)
{
   WsfPerceptionProcessor* processorPtr = new WsfPerceptionProcessor(aPlatform.GetScenario());
   processorPtr->SetName(aPlatform.GetName() + "_default_perception_processor");

   aPlatform.AddComponent(processorPtr);

   // Must initialize since they may be created post simulation initialization
   processorPtr->PreInitialize(aPlatform.GetSimTime());
   processorPtr->Initialize(aPlatform.GetSimTime());

   return processorPtr;
}

bool WsfPerceptionProcessor::PossibleEnemy(WsfLocalTrack* aTrackPtr)
{
   if (aTrackPtr->SideIdValid())
   {
      if (aTrackPtr->GetSideId() == GetPlatform()->GetSideId())
      {
         return false;
      }
   }
   return true;
}

void WsfPerceptionProcessor::LocalTrackDropped(double aSimTime, const WsfLocalTrack* aLocalTrackPtr)
{
   // remove track from perception
   auto it = mPerceivedThreats.begin();
   for (; it != mPerceivedThreats.end(); ++it)
   {
      if ((*it)->GetTrackId() == aLocalTrackPtr->GetTrackId())
      {
         delete *it;
         mPerceivedThreats.erase(it);

         if (mHeatMapPtr != nullptr)
         {
            // add track to heat map to estimate where it might be
            mHeatMapPtr->LocalTrackDropped(aSimTime, aLocalTrackPtr);
         }
         break;
      }
   }
}

// virtual
bool WsfPerceptionProcessor::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   bool processed = false;
   if (aMessage.GetType() == WsfAssetMessage::GetTypeId())
   {
      const WsfAssetMessage& message            = static_cast<const WsfAssetMessage&>(aMessage);
      WsfAssetPerception*    assetPerceptionPtr = message.GetAsset();
      // save off info into our buffer of received status messages (asset perception)

      // if (!Unimportant(assetPerceptionPtr->mIndex))   //TODO : bother with this check here?
      {
         // don't bother to save off perception if its of THIS platform
         if (assetPerceptionPtr->mIndex != GetPlatform()->GetIndex())
         {
            auto it = mReceivedAssetStatus.find(assetPerceptionPtr->mIndex);
            if (it == mReceivedAssetStatus.end())
            {
               // should add a perception to the map for this guy the first time
               mReceivedAssetStatus[assetPerceptionPtr->mIndex] = assetPerceptionPtr->Clone();
            }
            else if (it->second->mTime < assetPerceptionPtr->mTime)
            {
               // should overwrite the existing perception
               delete mReceivedAssetStatus[assetPerceptionPtr->mIndex];
               mReceivedAssetStatus[assetPerceptionPtr->mIndex] = assetPerceptionPtr->Clone();
            }
         }
      }
      processed = true;
   }

   // Perform any local processing (script on_message callback)
   processed |= mMessageHandlerPtr->ProcessMessage(aSimTime, aMessage);
   return processed;
}

void WsfPerceptionProcessor::TrackManagerChanged(double aSimTime, WsfTrackManager* aNewManagerPtr)
{
   if (aNewManagerPtr != nullptr)
   {
      mTrackManagerPtr = aNewManagerPtr;
   }
}

void WsfPerceptionProcessor::AddExtraCognitiveLoading(double timeDelay)
{
   mExtraCognitiveDelaysThreats.push_back(timeDelay);
   mExtraCognitiveDelaysAssets.push_back(timeDelay);
}

std::vector<WsfAssetPerception*>& WsfPerceptionProcessor::PerceivedAssets(double aSimTime)
{
   if (mAssetUpdateInterval <= 0)
   {
      // no events are updating the 'mPerceivedAssets', we need to fill it each query here
      UpdateAssets(aSimTime);
   }
   return mPerceivedAssets;
}

std::vector<WsfLocalTrack*>& WsfPerceptionProcessor::PerceivedThreats(double aSimTime)
{
   if (mThreatUpdateInterval <= 0)
   {
      // no events are updating the 'mPerceivedThreats', we need to fill it each query here
      UpdateThreats(aSimTime);
   }
   return mPerceivedThreats;
}

WsfAssetPerception* WsfPerceptionProcessor::PerceivedAsset(size_t aPlatformIndex)
{
   for (auto& perceivedAsset : mPerceivedAssets)
   {
      if (perceivedAsset->mIndex == aPlatformIndex)
      {
         return perceivedAsset;
      }
   }
   return nullptr;
}

WsfAssetPerception* WsfPerceptionProcessor::NearestAsset(double aSimTime)
{
   WsfAssetPerception*              nearestAssetPtr = nullptr;
   std::vector<WsfAssetPerception*> assets          = PerceivedAssets(aSimTime);
   if (!assets.empty())
   {
      double minDistance    = 1.0e+200; // Large initial distance (meters)
      double platformWCS[3] = {0};      // Location of the platform this processor is attached to
      double deltaLocWCS[3] = {0};
      GetPlatform()->GetLocationWCS(platformWCS);

      auto it = assets.begin();
      for (; it != assets.end(); ++it)
      {
         UtVec3d::Subtract(deltaLocWCS, platformWCS, (*it)->mLocationWCS);
         // Don't need actual range, avoid sqrt()
         double distanceSquared = UtVec3d::MagnitudeSquared(deltaLocWCS);
         if (distanceSquared < minDistance)
         {
            minDistance     = distanceSquared;
            nearestAssetPtr = *it;
         }
      }
   }
   // return zero/null if no assets
   return nearestAssetPtr;
}

WsfLocalTrack* WsfPerceptionProcessor::PerceivedThreat(WsfStringId targetNameId)
{
   auto it = mPerceivedThreats.begin();
   for (; it != mPerceivedThreats.end(); ++it)
   {
      if ((*it)->GetTargetName() == targetNameId)
      {
         return *it;
      }
   }
   return nullptr;
}


WsfLocalTrack* WsfPerceptionProcessor::PerceivedThreat(WsfTrackId& trackId)
{
   auto it = mPerceivedThreats.begin();
   for (; it != mPerceivedThreats.end(); ++it)
   {
      if ((*it)->GetTrackId() == trackId)
      {
         return *it;
      }
   }
   return nullptr;
}

WsfLocalTrack* WsfPerceptionProcessor::NearestThreat(double aSimTime)
{
   WsfLocalTrack*              nearestTrackPtr = nullptr;
   std::vector<WsfLocalTrack*> threats         = PerceivedThreats(aSimTime);
   if (!threats.empty())
   {
      double minDistance    = 1.0e+200; // Large initial distance (meters)
      double platformWCS[3] = {0};      // Location of the platform this processor is attached to
      double threatWCS[3]   = {0};
      double deltaLocWCS[3] = {0};
      GetPlatform()->GetLocationWCS(platformWCS);

      auto it = threats.begin();
      for (; it != threats.end(); ++it)
      {
         if ((*it)->LocationValid())
         {
            // Track has 3D location, can compute range
            (*it)->GetExtrapolatedLocationWCS(aSimTime, threatWCS);
            UtVec3d::Subtract(deltaLocWCS, platformWCS, threatWCS);
            // Don't need actual range, avoid sqrt()
            double distanceSquared = UtVec3d::MagnitudeSquared(deltaLocWCS);
            if (distanceSquared < minDistance)
            {
               minDistance     = distanceSquared;
               nearestTrackPtr = *it;
            }
         }
      }
   }
   // return zero/null if no threats
   return nearestTrackPtr;
}

void WsfPerceptionProcessor::MarkAssetAsImportant(WsfPlatform& aAsset)
{
   MarkAssetAsImportant(aAsset.GetIndex());
}

void WsfPerceptionProcessor::MarkAssetAsImportant(size_t aIndex)
{
   // add to important list
   mImportantAssets[aIndex] = 1;
   // remove from unimportant list
   mUnimportantAssets.erase(aIndex);
}

void WsfPerceptionProcessor::MarkAssetAsUnimportant(WsfPlatform& aAsset)
{
   MarkAssetAsUnimportant(aAsset.GetIndex());
}

void WsfPerceptionProcessor::MarkAssetAsUnimportant(size_t aIndex)
{
   mUnimportantAssets[aIndex] = 1; // add to unimportant list
   mImportantAssets.erase(aIndex); // remove from important list
}

void WsfPerceptionProcessor::MarkThreatAsImportant(WsfLocalTrack& aThreat)
{
   mImportantThreats[aThreat.GetTargetIndex()] = 1;     // add to important list
   mUnimportantThreats.erase(aThreat.GetTargetIndex()); // remove from unimportant list
}

void WsfPerceptionProcessor::MarkThreatAsUnimportant(WsfLocalTrack& aThreat)
{
   mUnimportantThreats[aThreat.GetTargetIndex()] = 1; // add to unimportant list
   mImportantThreats.erase(aThreat.GetTargetIndex()); // remove from important list
}

void WsfPerceptionProcessor::MarkAssetTypeAsImportant(WsfStringId aType)
{
   mImportantAssetTypes[aType] = 1;
   mUnimportantAssetTypes.erase(aType);
}

void WsfPerceptionProcessor::MarkAssetTypeAsUnimportant(WsfStringId aType)
{
   mUnimportantAssetTypes[aType] = 1;
   mImportantAssetTypes.erase(aType);
}

void WsfPerceptionProcessor::MarkThreatTypeAsImportant(WsfStringId aType)
{
   mImportantThreatTypes[aType] = 1;
   mUnimportantThreatTypes.erase(aType);
}

void WsfPerceptionProcessor::MarkThreatTypeAsUnimportant(WsfStringId aType)
{
   mUnimportantThreatTypes[aType] = 1;
   mImportantThreatTypes.erase(aType);
}

bool WsfPerceptionProcessor::SetScriptAssetScoringFunction(UtScriptContext* aContextPtr, WsfStringId aFunctionName)
{
   mAssetFunctionScriptName       = aFunctionName;
   mAssetFunctionScriptPtr        = nullptr;
   mAssetFunctionScriptContextPtr = aContextPtr;

   mAssetFunctionScriptPtr = mAssetFunctionScriptContextPtr->FindScript(mAssetFunctionScriptName);
   while (mAssetFunctionScriptPtr == nullptr && mAssetFunctionScriptContextPtr->GetParent() != nullptr)
   {
      mAssetFunctionScriptContextPtr = mAssetFunctionScriptContextPtr->GetParent();
      mAssetFunctionScriptPtr        = mAssetFunctionScriptContextPtr->FindScript(mAssetFunctionScriptName);
   }

   if (mAssetFunctionScriptPtr == nullptr)
   {
      auto out = ut::log::error() << "Unable to find script for asset importance function.";
      out.AddNote() << "Script: " << mAssetFunctionScriptName;
      return false;
   }

   UtScriptEnvironment*    environmentPtr   = mAssetFunctionScriptContextPtr->GetEnvironment();
   const UtScriptFunction* expectedProtoPtr = environmentPtr->GetPrototype("double", "WsfAssetPerception");
   if (expectedProtoPtr == nullptr)
   {
      assert(!"Invalid expected signature.");
   }

   if (expectedProtoPtr != mAssetFunctionScriptPtr->mPrototype)
   {
      auto out = ut::log::error() << "Perception processor asset scoring function received unexpected signature.";
      out.AddNote() << "Received: " << environmentPtr->GetPrototypeDescription(mAssetFunctionScriptPtr->mPrototype);
      out.AddNote() << "Expected: " << environmentPtr->GetPrototypeDescription(expectedProtoPtr);
      mAssetFunctionScriptPtr = nullptr;
      return false;
   }
   return true;
}

bool WsfPerceptionProcessor::SetScriptThreatScoringFunction(UtScriptContext* aContextPtr, WsfStringId aFunctionName)
{
   mThreatFunctionScriptName    = aFunctionName;
   mThreatFunctionScriptPtr     = nullptr;
   mThreatFunctionScriptContext = aContextPtr;

   mThreatFunctionScriptPtr = mThreatFunctionScriptContext->FindScript(mThreatFunctionScriptName);
   while (mThreatFunctionScriptPtr == nullptr && mThreatFunctionScriptContext->GetParent() != nullptr)
   {
      mThreatFunctionScriptContext = mThreatFunctionScriptContext->GetParent();
      mThreatFunctionScriptPtr     = mThreatFunctionScriptContext->FindScript(mThreatFunctionScriptName);
   }

   if (mThreatFunctionScriptPtr == nullptr)
   {
      auto out = ut::log::error() << "Unable to find script for threat importance function.";
      out.AddNote() << "Script: " << mThreatFunctionScriptName;
      return false;
   }

   UtScriptEnvironment*    environmentPtr   = mAssetFunctionScriptContextPtr->GetEnvironment();
   const UtScriptFunction* expectedProtoPtr = environmentPtr->GetPrototype("double", "WsfLocalTrack");
   if (expectedProtoPtr == nullptr)
   {
      assert(!"Invalid expected signature.");
   }

   if (expectedProtoPtr != mThreatFunctionScriptPtr->mPrototype)
   {
      auto out = ut::log::error() << "Perception processor threat scoring function received unexpected signature.";
      out.AddNote() << "Received: " << environmentPtr->GetPrototypeDescription(mThreatFunctionScriptPtr->mPrototype);
      out.AddNote() << "Expected: " << environmentPtr->GetPrototypeDescription(expectedProtoPtr);

      mThreatFunctionScriptPtr = nullptr;
      return false;
   }
   return true;
}

bool WsfPerceptionProcessor::AssetScore(WsfAssetPerception* aAssetPtr, double& score)
{
   if (mAssetFunctionScriptContextPtr == nullptr || mAssetFunctionScriptPtr == nullptr)
   {
      return false;
   }
   UtScriptClass*    aClassPtr = mAssetFunctionScriptContextPtr->GetTypes()->GetClass("WsfAssetPerception");
   UtScriptExecutor* execPtr   = &GetSimulation()->GetScriptExecutor();
   UtScriptDataList  args;
   args.push_back(UtScriptData(new UtScriptRef(aAssetPtr, aClassPtr)));
   UtScriptData retVal;
   mAssetFunctionScriptContextPtr->Execute(execPtr, mAssetFunctionScriptPtr, retVal, args);
   score = retVal.GetDouble();
   return true;
}

bool WsfPerceptionProcessor::ThreatScore(WsfLocalTrack* aThreatPtr, double& score)
{
   if (mThreatFunctionScriptContext == nullptr || mThreatFunctionScriptPtr == nullptr)
   {
      return false;
   }
   // UtScriptClass* aClass = mThreatFunctionScriptContext->GetTypes()->GetClass("WsfTrack");
   UtScriptClass*    aClassPtr = mThreatFunctionScriptContext->GetTypes()->GetClass("WsfLocalTrack");
   UtScriptExecutor* execPtr   = &GetSimulation()->GetScriptExecutor();
   UtScriptDataList  args;
   args.push_back(UtScriptData(new UtScriptRef(aThreatPtr, aClassPtr)));
   UtScriptData retVal;
   mThreatFunctionScriptContext->Execute(execPtr, mThreatFunctionScriptPtr, retVal, args);
   score = retVal.GetDouble();
   return true;
}

void WsfPerceptionProcessor::SetAssetUpdateInterval(double aInterval)
{
   if (mAssetUpdateInterval <= 0 && aInterval > 0)
   {
      // must add an event for updating the asset perception
      GetSimulation()->AddEvent(
         ut::make_unique<WsfPerceptionProcessorUpdateEvent>(GetSimulation()->GetSimTime() + aInterval,
                                                            GetPlatform()->GetIndex(),
                                                            this,
                                                            WsfPerceptionProcessorUpdateEvent::ASSETS));
   }
   mAssetUpdateInterval = aInterval;
}

void WsfPerceptionProcessor::SetThreatUpdateInterval(double aInterval)
{
   if (mThreatUpdateInterval <= 0 && aInterval > 0)
   {
      // must add an event for updating the threat perception
      GetSimulation()->AddEvent(
         ut::make_unique<WsfPerceptionProcessorUpdateEvent>(GetSimulation()->GetSimTime() + aInterval,
                                                            GetPlatform()->GetIndex(),
                                                            this,
                                                            WsfPerceptionProcessorUpdateEvent::THREATS));
   }
   mThreatUpdateInterval = aInterval;
}

std::set<WsfPlatform*> WsfPerceptionProcessor::TruthAssets()
{
   // TODO - create this asset list on init once? (edit it as platforms are deleted)
   //        or create the list fresh each time?
   std::set<WsfPlatform*> assets;

   if (mTruthPerceptionMask == 0)
   {
      return assets; // empty
   }

   WsfCommandChain* chainPtr = GetPlatform()->GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId());
   if (chainPtr != nullptr)
   {
      // do subordinates first, to avoid missing out on climbing down the entire command tree
      //(if you would have already happen to have added somebody)
      if ((mTruthPerceptionMask & cALLSUBORDINATES) != 0u)
      {
         std::pair<std::set<WsfPlatform*>::iterator, bool> ret;
         // add all subordinates down the chain, start with first level subordinates
         std::vector<WsfPlatform*> subs;
         subs.insert(subs.end(), chainPtr->GetSubordinates().begin(), chainPtr->GetSubordinates().end());
         for (unsigned int i = 0; i < subs.size(); ++i)
         {
            WsfPlatform* pPtr = subs[i];
            ret               = assets.insert(pPtr);
            if (ret.second == true)
            {
               // haven't added this subordinate yet or climbed down it's chain either
               // keep climbing down the command chain (tree) adding all subordinates
               WsfCommandChain* tempChainPtr = pPtr->GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId());
               if (tempChainPtr != nullptr)
               {
                  subs.insert(subs.end(), tempChainPtr->GetSubordinates().begin(), tempChainPtr->GetSubordinates().end());
               }
            }
         }
      }
      else if ((mTruthPerceptionMask & cSUBORDINATES) != 0u)
      {
         WsfCommandChain::PlatformList           subs = chainPtr->GetSubordinates();
         WsfCommandChain::PlatformList::iterator it;
         for (it = subs.begin(); it != subs.end(); ++it)
         {
            assets.insert(*it);
         }
      }

      // do commanders second
      WsfPlatform* cmdrPtr = chainPtr->GetCommander();
      if ((mTruthPerceptionMask & cALLCOMMANDERS) != 0u)
      {
         while (cmdrPtr != nullptr)
         {
            // save off this commander
            assets.insert(cmdrPtr);
            // keep climbing upwards
            WsfCommandChain* tempChainPtr = cmdrPtr->GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId());
            if (tempChainPtr == nullptr)
            {
               break;
            }
            cmdrPtr = tempChainPtr->GetCommander();
         }
      }
      else if (((mTruthPerceptionMask & cCOMMANDER) != 0u) && cmdrPtr != nullptr)
      {
         assets.insert(cmdrPtr);
      }

      // do peers last, no traversing a command chain here
      if ((mTruthPerceptionMask & cPEERS) != 0u)
      {
         WsfCommandChain::PlatformList           peers = chainPtr->GetPeers();
         WsfCommandChain::PlatformList::iterator it;
         for (it = peers.begin(); it != peers.end(); ++it)
         {
            assets.insert(*it);
         }
      }
   }

   // add in self perception if configured to do so
   if (mPerceiveSelfStatus == true)
   {
      assets.insert(GetPlatform());
   }

   return assets;
}

bool WsfPerceptionProcessor::Important(WsfLocalTrack* aThreatPtr)
{
   if (aThreatPtr == nullptr)
   {
      return false;
   }
   // check if this specific track has been marked as important
   if (mImportantThreats.find(aThreatPtr->GetTargetIndex()) != mImportantThreats.end())
   {
      return true;
   }
   // check if the track's type has been marked as important
   // use perceived type if possible
   WsfStringId tgtType = (aThreatPtr->TypeIdValid()) ? (aThreatPtr->GetTypeId()) : (aThreatPtr->GetTargetType());
   if (mImportantThreatTypes.find(tgtType) != mImportantThreatTypes.end())
   {
      return true;
   }
   return false;
}

bool WsfPerceptionProcessor::Important(WsfPlatform* aAssetPtr)
{
   if (aAssetPtr == nullptr)
   {
      return false;
   }
   // check if this specific platform has been marked as important
   if (mImportantAssets.find(aAssetPtr->GetIndex()) != mImportantAssets.end())
   {
      return true;
   }
   // check if the platform's type has been marked as important
   WsfStringId key = aAssetPtr->GetTypeId();
   if (mImportantAssetTypes.find(key) != mImportantAssetTypes.end())
   {
      return true;
   }
   return false;
}

bool WsfPerceptionProcessor::Important(size_t aPlatformIndex)
{
   return Important(GetSimulation()->GetPlatformByIndex(aPlatformIndex));
}

bool WsfPerceptionProcessor::Unimportant(WsfLocalTrack* aThreatPtr)
{
   if (aThreatPtr == nullptr)
   {
      return false;
   }
   // check if this specific track has been marked as unimportant
   if (mUnimportantThreats.find(aThreatPtr->GetTargetIndex()) != mUnimportantThreats.end())
   {
      return true;
   }
   // check if the track's type has been marked as unimportant
   // use perceived type if possible
   WsfStringId tgtType = (aThreatPtr->TypeIdValid()) ? (aThreatPtr->GetTypeId()) : (aThreatPtr->GetTargetType());
   if (mUnimportantThreatTypes.find(tgtType) != mUnimportantThreatTypes.end())
   {
      return true;
   }
   return false;
}

bool WsfPerceptionProcessor::Unimportant(WsfPlatform* aAssetPtr)
{
   if (aAssetPtr == nullptr)
   {
      return false;
   }
   // check if this specific platform has been marked as unimportant
   if (mUnimportantAssets.find(aAssetPtr->GetIndex()) != mUnimportantAssets.end())
   {
      return true;
   }
   // check if the platform's type has been marked as unimportant
   WsfStringId key = aAssetPtr->GetTypeId();
   if (mUnimportantAssetTypes.find(key) != mUnimportantAssetTypes.end())
   {
      return true;
   }
   return false;
}

bool WsfPerceptionProcessor::Unimportant(size_t aPlatformIndex)
{
   return Unimportant(GetSimulation()->GetPlatformByIndex(aPlatformIndex));
}

double WsfPerceptionProcessor::TryUpdateThreats(double aSimTime)
{
   if (mExtraCognitiveDelaysThreats.empty())
   {
      // perform the update
      UpdateThreats(aSimTime);
      return (aSimTime + mThreatUpdateInterval);
   }
   else
   {
      // delay the update even longer
      double extraDelay = 0.0;
      while (!mExtraCognitiveDelaysThreats.empty())
      {
         extraDelay += mExtraCognitiveDelaysThreats.back();
         mExtraCognitiveDelaysThreats.pop_back();
      }
      return (aSimTime + extraDelay);
   }
}

void WsfPerceptionProcessor::UpdateThreats(double aSimTime)
{
   if (aSimTime > mLastThreatUpdateTime)
   {
      // clear out previous perception
      auto it = mPerceivedThreats.begin();
      while (it != mPerceivedThreats.end())
      {
         delete (*it);
         it = mPerceivedThreats.erase(it);
      }

      // actually update the threats
      WsfLocalTrackList&               threats = GetPlatform()->GetMasterTrackList();
      std::vector<WsfSortedPerception> sortedThreats;

      // first populate perceived threats with important threats
      for (int i = 0; i < (int)threats.GetTrackCount(); ++i)
      {
         WsfLocalTrack* threatPtr = threats.GetTrackEntry(i);
         double         score;
         if (ThreatScore(threatPtr, score))
         {
            // use custom user script for threat scoring (do not use important/unimportant feature)
            if (score > 0)
            {
               sortedThreats.emplace_back(threatPtr, score);
            }
            else
            {
               // skip it - zero or negative user score means threat is ignored
            }
         }
         else if (Important(threatPtr))
         {
            mPerceivedThreats.push_back(dynamic_cast<WsfLocalTrack*>(threatPtr->Clone()));
            if (mMaxThreatLoad > 0 && (int)mPerceivedThreats.size() >= mMaxThreatLoad)
            {
               break;
            }
         }
         else if (Unimportant(threatPtr))
         {
            // skip it
         }
         else
         {
            // put it into sorted list
            sortedThreats.emplace_back(threatPtr, GetPlatform());
         }
      }

      // now fill in the rest of our perceived threats with those that are closest
      if (mMaxThreatLoad < 0 || ((int)mPerceivedThreats.size() < mMaxThreatLoad))
      {
         std::sort(sortedThreats.begin(), sortedThreats.end());
         auto it = sortedThreats.begin();
         for (; it != sortedThreats.end(); ++it)
         {
            mPerceivedThreats.push_back(dynamic_cast<WsfLocalTrack*>(it->mTrackPtr->Clone()));
            if (mMaxThreatLoad > 0 && (int)mPerceivedThreats.size() >= mMaxThreatLoad)
            {
               break;
            }
         }
      }
      mLastThreatUpdateTime = aSimTime;
   }
}

// attempts an update (will be delayed if any extra cognitive delays were introduced
double WsfPerceptionProcessor::TryUpdateAssets(double aSimTime)
{
   if (mExtraCognitiveDelaysAssets.empty())
   {
      // perform the update
      UpdateAssets(aSimTime);

      return (aSimTime + mAssetUpdateInterval);
   }
   else
   {
      // delay the update even longer
      double extraDelay = 0.0;
      while (!mExtraCognitiveDelaysAssets.empty())
      {
         extraDelay += mExtraCognitiveDelaysAssets.back();
         mExtraCognitiveDelaysAssets.pop_back();
      }
      return (aSimTime + extraDelay);
   }
}

void WsfPerceptionProcessor::UpdateAssets(double aSimTime)
{
   if (aSimTime > mLastAssetUpdateTime)
   {
      // clear out previous perception
      for (auto& perceivedAsset : mPerceivedAssets)
      {
         delete perceivedAsset;
      }
      mPerceivedAssets.clear();

      if (mAssetPerception == TRUTH)
      {
         // actually update the assets
         std::set<WsfPlatform*>           assetPlatforms = this->TruthAssets();
         std::vector<WsfAssetPerception*> assets;
         for (auto assetPlatform : assetPlatforms)
         {
            assets.push_back(new WsfAssetPerception(aSimTime, *assetPlatform));
         }
         std::vector<WsfSortedPerception> sortedAssets;

         // first populate perceived assets with important assets
         for (WsfAssetPerception* assetPtr : assets)
         {
            double score;
            if (AssetScore(assetPtr, score))
            {
               // use custom user script for asset scoring (do not use important/unimportant feature)
               if (score > 0)
               {
                  sortedAssets.emplace_back(assetPtr, score);
               }
               else
               {
                  // skip it - zero or negative user score means threat is ignored
                  delete assetPtr;
               }
            }
            else if (Important(assetPtr->mIndex))
            {
               // asset->Update(aSimTime);
               mPerceivedAssets.push_back(assetPtr);
               if (mMaxAssetLoad > 0 && (int)mPerceivedAssets.size() >= mMaxAssetLoad)
               {
                  break;
               }
            }
            else if (Unimportant(assetPtr->mIndex))
            {
               // skip it
               delete assetPtr;
            }
            else
            {
               // assetPtr->Update(aSimTime);
               // put it into sorted list
               sortedAssets.emplace_back(assetPtr, GetPlatform());
            }
         }
         // now fill in the rest of our perceived assets with those that are closest
         if (mMaxAssetLoad < 0 || ((int)mPerceivedAssets.size() < mMaxAssetLoad))
         {
            std::sort(sortedAssets.begin(), sortedAssets.end());
            auto it = sortedAssets.begin();
            for (; it != sortedAssets.end(); ++it)
            {
               // mPerceivedAssets.push_back(new WsfAssetPerception(aSimTime, *(it->mPlatformPtr)));
               mPerceivedAssets.push_back(it->mAssetPtr);
               if (mMaxAssetLoad > 0 && (int)mPerceivedAssets.size() >= mMaxAssetLoad)
               {
                  ++it;
                  break;
               }
            }
            // delete the unused asset perception pointers
            for (; it != sortedAssets.end(); ++it)
            {
               delete it->mAssetPtr;
            }
         }
      }
      else // if (mAssetPerception == STATUS_MESSAGES)
      {
         // keep the list of assets being perceived updated
         std::vector<WsfSortedPerception> sortedAssets;

         // add in self perception if configured to do so
         if (mPerceiveSelfStatus)
         {
            WsfPlatform* selfPtr = GetPlatform();
            auto         it      = mReceivedAssetStatus.find(selfPtr->GetIndex());
            if (it != mReceivedAssetStatus.end())
            {
               // Delete the old entry
               delete it->second;
            }
            // Update with new data
            mReceivedAssetStatus[selfPtr->GetIndex()] = new WsfAssetPerception(aSimTime, *selfPtr);
         }

         // first populate perceived assets with important assets
         for (auto receivedAssetStatus : mReceivedAssetStatus)
         {
            WsfAssetPerception* assetPtr = receivedAssetStatus.second;
            double              score;
            if (AssetScore(assetPtr, score))
            {
               // use custom user script for asset scoring (do not use important/unimportant feature)
               if (score > 0)
               {
                  sortedAssets.emplace_back(assetPtr, score);
               }
               else
               {
                  // skip it - zero or negative user score means threat is ignored
               }
            }
            else if (Important(assetPtr->mIndex))
            {
               mPerceivedAssets.push_back(assetPtr->Clone());
               if (mMaxAssetLoad > 0 && (int)mPerceivedAssets.size() >= mMaxAssetLoad)
               {
                  break;
               }
            }
            else if (Unimportant(assetPtr->mIndex))
            {
               // skip it
            }
            else
            {
               // put it into sorted list
               sortedAssets.emplace_back(assetPtr, GetPlatform());
            }
         }
         // now fill in the rest of our perceived assets with those that are closest
         if (mMaxAssetLoad < 0 || ((int)mPerceivedAssets.size() < mMaxAssetLoad))
         {
            std::sort(sortedAssets.begin(), sortedAssets.end());
            auto it = sortedAssets.begin();
            for (; it != sortedAssets.end(); ++it)
            {
               mPerceivedAssets.push_back(it->mAssetPtr->Clone());
               if (mMaxAssetLoad > 0 && (int)mPerceivedAssets.size() >= mMaxAssetLoad)
               {
                  break;
               }
            }
         }
      }
      mLastAssetUpdateTime = aSimTime;
   }
}

double WsfPerceptionProcessor::SendStatus(double aSimTime)
{
   if (HasExternalLinks())
   {
      WsfPlatform* platformPtr = GetPlatform();

      if (mReportingSelfStatus)
      {
         WsfAssetMessage myStatusMessage(platformPtr, ut::make_unique<WsfAssetPerception>(aSimTime, *platformPtr));
         mExternalLinks.SendMessage(aSimTime, myStatusMessage);
      }

      if (mReportingOthersStatus)
      {
         // also send all asset status knowledge I have
         for (auto& receivedAssetStatus : mReceivedAssetStatus)
         {
            // TODO: should we store the WsfObject mNameId in an asset perception?
            //(so we can exclude sending his own status back to him)
            //(don't want to do a platform lookup just to find his name each time we send)
            WsfAssetMessage aStatus(platformPtr, ut::clone(receivedAssetStatus.second));
            mExternalLinks.SendMessage(aSimTime, aStatus);
         }
      }
   }
   return aSimTime + mStatusMessageReportInterval;
}

// =================================================================================================
//! Get the pointer to the first attached perception processor.
//! If a perception processor has not been added a new default one will be created and
//! added to the platform.
//! @return a WsfPerceptionProcessor pointer.
WsfPerceptionProcessor* WsfPerceptionProcessor::GetPerceptionProcessor(WsfPlatform& aPlatform)
{
   // Attempt to find the first instance of a WsfPerceptionProcessor.
   WsfPerceptionProcessor* processorPtr(nullptr);
   for (WsfComponentList::RoleIterator<WsfProcessor> iter(aPlatform); !iter.AtEnd(); ++iter)
   {
      processorPtr = dynamic_cast<WsfPerceptionProcessor*>(*iter);
      if (processorPtr != nullptr)
      {
         break;
      }
   }

   if (processorPtr == nullptr)
   {
      // If a perception processor is not found, create a default one.
      // It will not be linked up to anything (empty asset perception).
      // The threat perception will be equivalent to current master tracks.
      processorPtr = CreatePerceptionProcessor(aPlatform.GetSimTime(), aPlatform);
   }
   return processorPtr;
}

std::vector<WsfAssetPerception*>& WsfPerceptionProcessor::GetPlatformPerceivedAssets(WsfPlatform& aPlatform, double aSimTime)
{
   return GetPerceptionProcessor(aPlatform)->PerceivedAssets(aSimTime);
}

std::vector<WsfLocalTrack*>& WsfPerceptionProcessor::GetPlatformPerceivedThreats(WsfPlatform& aPlatform, double aSimTime)
{
   return GetPerceptionProcessor(aPlatform)->PerceivedThreats(aSimTime);
}

WsfPerceptionProcessorUpdateEvent::WsfPerceptionProcessorUpdateEvent(double                  aSimTime,
                                                                     size_t                  aPlatformIndex,
                                                                     WsfPerceptionProcessor* aPercProcPtr,
                                                                     CognitiveUpdateType     aType)
   : WsfEvent(aSimTime)
   , mPlatformIndex(aPlatformIndex)
   , mPercProcPtr(aPercProcPtr)
   , mType(aType)
{
}

WsfEvent::EventDisposition WsfPerceptionProcessorUpdateEvent::Execute()
{
   EventDisposition eventDisposition = cDELETE;
   WsfPlatform*     platformPtr      = GetSimulation()->GetPlatformByIndex(mPlatformIndex);
   if ((platformPtr != nullptr) && /*(! platformPtr->IsDeleted()) &&*/ (mPercProcPtr != nullptr))
   {
      // platform is valid, so perception processor should be too, use it
      if (mType == STATUS)
      {
         if (mPercProcPtr->StatusMessageReportInterval() > 0 && mPercProcPtr->HasExternalLinks())
         {
            SetTime(mPercProcPtr->SendStatus(GetTime()));
            eventDisposition = cRESCHEDULE;
         }
      }
      else if (mType == ASSETS)
      {
         if (mPercProcPtr->AssetUpdateInterval() > 0)
         {
            // update perception != truth
            SetTime(mPercProcPtr->TryUpdateAssets(GetTime()));
            eventDisposition = cRESCHEDULE;
         }
      }
      else // if (mType == THREATS)
      {
         if (mPercProcPtr->ThreatUpdateInterval() > 0)
         {
            // update, perception != truth
            SetTime(mPercProcPtr->TryUpdateThreats(GetTime()));
            eventDisposition = cRESCHEDULE;
         }
      }
   }
   return eventDisposition;
}

WsfScriptPerceptionProcessor::WsfScriptPerceptionProcessor(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptProcessorClass(aClassName, aScriptTypesPtr)
{
   mConstructible = false;
   SetClassName("WsfPerceptionProcessor");

   AddMethod(ut::make_unique<PerceivedAssets>());
   AddMethod(ut::make_unique<PerceivedThreats>());
   AddMethod(ut::make_unique<NearestAsset>());
   AddMethod(ut::make_unique<NearestThreat>());
   AddMethod(ut::make_unique<AddExtraCognitiveLoading>());

   AddMethod(ut::make_unique<SetAssetImportant>("SetAssetImportant"));       // WsfPlatform
   AddMethod(ut::make_unique<SetAssetImportant_2>("SetAssetImportant"));     // WsfAssetPerception
   AddMethod(ut::make_unique<SetAssetUnimportant>("SetAssetUnimportant"));   // WsfPlatform
   AddMethod(ut::make_unique<SetAssetUnimportant_2>("SetAssetUnimportant")); // WsfAssetPerception

   AddMethod(ut::make_unique<SetThreatImportant>());   // WsfLocalTrack
   AddMethod(ut::make_unique<SetThreatUnimportant>()); // WsfLocalTrack

   AddMethod(ut::make_unique<SetAssetTypeImportant>());    // string
   AddMethod(ut::make_unique<SetAssetTypeUnimportant>());  // string
   AddMethod(ut::make_unique<SetThreatTypeImportant>());   // string
   AddMethod(ut::make_unique<SetThreatTypeUnimportant>()); // string

   AddMethod(ut::make_unique<GetNearestHeat>());
   AddMethod(ut::make_unique<GetNearestMaxHeat>());
   AddMethod(ut::make_unique<GetHeat>());
   AddMethod(ut::make_unique<GetHeatSourcesCount>());
}

// virtual
void* WsfScriptPerceptionProcessor::Create(const UtScriptContext& aContext)
{
   return new WsfPerceptionProcessor(*WsfScriptContext::GetSCENARIO(aContext));
}

// virtual
void* WsfScriptPerceptionProcessor::Clone(void* aObjectPtr)
{
   auto objPtr = static_cast<WsfPerceptionProcessor*>(aObjectPtr);
   assert(objPtr != nullptr);
   return objPtr->Clone();
}

// virtual
void WsfScriptPerceptionProcessor::Destroy(void* aObjectPtr)
{
   auto objPtr = static_cast<WsfPerceptionProcessor*>(aObjectPtr);
   delete objPtr;
}

UtScriptContext* WsfScriptPerceptionProcessor::GetContext(void* aObjectPtr)
{
   UtScriptContext* contextPtr   = nullptr;
   auto             processorPtr = static_cast<WsfPerceptionProcessor*>(aObjectPtr);
   if (processorPtr != nullptr)
   {
      contextPtr = &(processorPtr->GetScriptContext().GetContext());
   }
   return contextPtr;
}

#include "script/WsfScriptDefs.hpp"

UT_DEFINE_SCRIPT_METHOD(WsfScriptPerceptionProcessor, WsfPerceptionProcessor, PerceivedAssets, 0, "Array<WsfAssetPerception>", "")
{
   auto           scriptAssetsPtr           = ut::make_unique<std::vector<UtScriptData>>(); // this array is returned
   UtScriptClass* classPtr                  = aReturnClassPtr->GetContainerDataType();      // type of object in array
   std::vector<WsfAssetPerception*>& assets = aObjectPtr->PerceivedAssets(SIMULATION->GetSimTime());
   for (auto& asset : assets)
   {
      scriptAssetsPtr->emplace_back(UtScriptRef::Ref(asset->Clone(), classPtr, UtScriptRef::cMANAGE));
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptAssetsPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPerceptionProcessor, WsfPerceptionProcessor, PerceivedThreats, 0, "Array<WsfTrack>", "")
{
   auto           scriptTracksPtr      = ut::make_unique<std::vector<UtScriptData>>(); // this array is returned
   UtScriptClass* classPtr             = aContext.GetTypes()->GetClass("WsfTrack");    // type of object in array
   std::vector<WsfLocalTrack*>& tracks = aObjectPtr->PerceivedThreats(SIMULATION->GetSimTime());
   for (auto& track : tracks)
   {
      scriptTracksPtr->emplace_back(UtScriptRef::Ref(track->Clone(), classPtr, UtScriptRef::cMANAGE));
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptTracksPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPerceptionProcessor, WsfPerceptionProcessor, NearestAsset, 0, "WsfAssetPerception", "")
{
   WsfAssetPerception* assetPtr = aObjectPtr->NearestAsset(SIMULATION->GetSimTime());
   aReturnVal.SetPointer(new UtScriptRef(assetPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPerceptionProcessor, WsfPerceptionProcessor, NearestThreat, 0, "WsfTrack", "")
{
   auto trackPtr = static_cast<WsfTrack*>(aObjectPtr->NearestThreat(SIMULATION->GetSimTime()));
   aReturnVal.SetPointer(new UtScriptRef(trackPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPerceptionProcessor, WsfPerceptionProcessor, AddExtraCognitiveLoading, 1, "void", "double")
{
   double duration = aVarArgs[0].GetDouble();
   aObjectPtr->AddExtraCognitiveLoading(duration);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPerceptionProcessor, WsfPerceptionProcessor, SetAssetImportant, 1, "void", "WsfPlatform")
{
   auto platformPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfPlatform>();
   aObjectPtr->MarkAssetAsImportant(platformPtr->GetIndex());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPerceptionProcessor, WsfPerceptionProcessor, SetAssetImportant_2, 1, "void", "WsfAssetPerception")
{
   auto assetPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfAssetPerception>();
   aObjectPtr->MarkAssetAsImportant(assetPtr->mIndex);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPerceptionProcessor, WsfPerceptionProcessor, SetAssetUnimportant, 1, "void", "WsfPlatform")
{
   auto platformPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfPlatform>();
   aObjectPtr->MarkAssetAsUnimportant(platformPtr->GetIndex());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPerceptionProcessor, WsfPerceptionProcessor, SetAssetUnimportant_2, 1, "void", "WsfAssetPerception")
{
   auto assetPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfAssetPerception>();
   aObjectPtr->MarkAssetAsUnimportant(assetPtr->mIndex);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPerceptionProcessor, WsfPerceptionProcessor, SetThreatImportant, 1, "void", "WsfLocalTrack")
{
   auto trackPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfLocalTrack>();
   aObjectPtr->MarkThreatAsImportant(*trackPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPerceptionProcessor, WsfPerceptionProcessor, SetThreatUnimportant, 1, "void", "WsfLocalTrack")
{
   auto trackPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfLocalTrack>();
   aObjectPtr->MarkThreatAsUnimportant(*trackPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPerceptionProcessor, WsfPerceptionProcessor, SetAssetTypeImportant, 1, "void", "string")
{
   std::string type = aVarArgs[0].GetString();
   aObjectPtr->MarkAssetTypeAsImportant(type);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPerceptionProcessor, WsfPerceptionProcessor, SetAssetTypeUnimportant, 1, "void", "string")
{
   std::string type = aVarArgs[0].GetString();
   aObjectPtr->MarkAssetTypeAsUnimportant(type);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPerceptionProcessor, WsfPerceptionProcessor, SetThreatTypeImportant, 1, "void", "string")
{
   std::string type = aVarArgs[0].GetString();
   aObjectPtr->MarkThreatTypeAsImportant(type);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPerceptionProcessor, WsfPerceptionProcessor, SetThreatTypeUnimportant, 1, "void", "string")
{
   std::string type = aVarArgs[0].GetString();
   aObjectPtr->MarkThreatTypeAsUnimportant(type);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPerceptionProcessor, WsfPerceptionProcessor, GetNearestHeat, 3, "bool", "WsfGeoPoint, WsfGeoPoint, double")
{
   bool heatFound = false;

   if (aObjectPtr->HeatMap() != nullptr)
   {
      auto   myLocationPtr      = aVarArgs[0].GetPointer()->GetAppObject<WsfGeoPoint>();
      auto   heatLocationPtr    = aVarArgs[1].GetPointer()->GetAppObject<WsfGeoPoint>();
      double requestedHeatValue = aVarArgs[2].GetDouble();
      double heatValue          = std::min(std::max(0.0, requestedHeatValue), 1.0);

      double myLLA[3]      = {0};
      double nearestLLA[3] = {0};
      myLocationPtr->GetLocationLLA(myLLA[0], myLLA[1], myLLA[2]);

      heatFound = aObjectPtr->HeatMap()->GetNearestHeat(myLLA, nearestLLA, heatValue);
      heatLocationPtr->SetLocationLLA(nearestLLA[0], nearestLLA[1], nearestLLA[2]);
   }
   aReturnVal.SetBool(heatFound);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPerceptionProcessor, WsfPerceptionProcessor, GetNearestMaxHeat, 2, "bool", "WsfGeoPoint, WsfGeoPoint")
{
   bool heatFound = false;

   if (aObjectPtr->HeatMap() != nullptr)
   {
      auto myLocationPtr   = aVarArgs[0].GetPointer()->GetAppObject<WsfGeoPoint>();
      auto heatLocationPtr = aVarArgs[1].GetPointer()->GetAppObject<WsfGeoPoint>();

      double myLLA[3]      = {0};
      double nearestLLA[3] = {0};
      myLocationPtr->GetLocationLLA(myLLA[0], myLLA[1], myLLA[2]);

      heatFound = aObjectPtr->HeatMap()->GetNearestMaxHeat(myLLA, nearestLLA);
      heatLocationPtr->SetLocationLLA(nearestLLA[0], nearestLLA[1], nearestLLA[2]);
   }
   aReturnVal.SetBool(heatFound);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPerceptionProcessor, WsfPerceptionProcessor, GetHeat, 1, "double", "WsfGeoPoint")
{
   double heatValue = 0;
   // Get heat value
   if (aObjectPtr->HeatMap() != nullptr)
   {
      // Get LLA from Geopoint
      double LLA[3]      = {0};
      auto   locationPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfGeoPoint>();
      locationPtr->GetLocationLLA(LLA[0], LLA[1], LLA[2]);

      heatValue = aObjectPtr->HeatMap()->GetHeatValue(LLA);
   }
   aReturnVal.SetDouble(heatValue);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptPerceptionProcessor, WsfPerceptionProcessor, GetHeatSourcesCount, 0, "int", "")
{
   int numHeatSources = 0;

   if (aObjectPtr->HeatMap() != nullptr)
   {
      numHeatSources = static_cast<int>(aObjectPtr->HeatMap()->ActiveHeatSources());
   }
   aReturnVal.SetInt(numHeatSources);
}
