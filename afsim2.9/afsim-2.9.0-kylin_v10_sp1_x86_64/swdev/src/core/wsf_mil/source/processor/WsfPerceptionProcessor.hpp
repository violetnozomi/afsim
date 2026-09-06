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

#ifndef WSFPERCEPTIONPROCESSOR_HPP
#define WSFPERCEPTIONPROCESSOR_HPP

#include <map>
#include <set>
#include <string>
#include <vector>

#include "UtCallbackHolder.hpp"
class UtInput;
class UtScriptContext;
class WsfAssetMessage;
#include "WsfAssetPerception.hpp"
class WsfComm;
#include "wsf_mil_export.h"

#include "WsfEvent.hpp"
#include "WsfHeatMap.hpp"
class WsfLocalTrack;
class WsfPlatform;
#include "script/WsfScriptProcessor.hpp"
#include "script/WsfScriptProcessorClass.hpp"
class WsfTrackId;
class WsfTrackManager;

class WSF_MIL_EXPORT WsfPerceptionProcessor : public WsfScriptProcessor
{
public:
   friend class WsfPerceptionProcessorUpdateEvent;

   enum AssetPerceptionType
   {
      TRUTH           = 0,
      STATUS_MESSAGES = 1
   };

   enum RecipientType
   {
      cCOMMANDER       = 1,
      cPEERS           = 2,
      cSUBORDINATES    = 4,
      cALLCOMMANDERS   = 8,
      cALLSUBORDINATES = 16
   };

   static WsfPerceptionProcessor* GetPerceptionProcessor(WsfPlatform& aPlatform);

   //! @name Perception methods.
   //@{
   static std::vector<WsfAssetPerception*>& GetPlatformPerceivedAssets(WsfPlatform& aPlatform,
                                                                       double       aSimTime); // empty if no perception
                                                                                         // processor is on the platform
   static std::vector<WsfLocalTrack*>& GetPlatformPerceivedThreats(WsfPlatform& aPlatform,
                                                                   double aSimTime); // empty if no perception processor
                                                                                     // is on the platform
   //@}


   WsfPerceptionProcessor(const WsfScenario& aScenario);
   WsfPerceptionProcessor& operator=(const WsfPerceptionProcessor&) = delete;
   ~WsfPerceptionProcessor() override;
   WsfPerceptionProcessor* Clone() const override { return new WsfPerceptionProcessor(*this); }

   //! @name Standard processor function overwrites
   //@{
   bool Initialize(double aSimTime) override;
   void Update(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;

   UtScriptClass* CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   static WsfPerceptionProcessor* CreatePerceptionProcessor(double aSimTime, WsfPlatform& aPlatform);

   const char* GetScriptClassName() const override { return "WsfPerceptionProcessor"; }
   //@}

   //! utility method - if side valid & not my side
   bool PossibleEnemy(WsfLocalTrack* aTrackPtr);

   //! @name Handlers
   //@{
   void LocalTrackDropped(double aSimTime, const WsfLocalTrack* aLocalTrackPtr);

   bool ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;

   void TrackManagerChanged(double aSimTime, WsfTrackManager* aNewManagerPtr);
   //@}

   //! @name Main access methods for class
   //@{
   std::vector<WsfAssetPerception*>& PerceivedAssets(double aSimTime);
   std::vector<WsfLocalTrack*>&      PerceivedThreats(double aSimTime);

   WsfAssetPerception* PerceivedAsset(size_t aPlatformIndex);
   WsfAssetPerception* NearestAsset(double aSimTime);
   WsfLocalTrack*      PerceivedThreat(WsfStringId targetNameId);
   WsfLocalTrack*      PerceivedThreat(WsfTrackId& trackId);
   WsfLocalTrack*      NearestThreat(double aSimTime);
   //@}

   WsfHeatMap* HeatMap() { return mHeatMapPtr; }

   //! @name For filtering assets &/or threats when count is above max load
   //@{
   void MarkAssetAsImportant(WsfPlatform& aAsset);
   void MarkAssetAsImportant(size_t aIndex);
   void MarkAssetAsUnimportant(WsfPlatform& aAsset);
   void MarkAssetAsUnimportant(size_t aIndex);

   void MarkThreatAsImportant(WsfLocalTrack& aThreat);
   void MarkThreatAsUnimportant(WsfLocalTrack& aThreat);

   void MarkAssetTypeAsImportant(WsfStringId aType);
   void MarkAssetTypeAsUnimportant(WsfStringId aType);
   void MarkThreatTypeAsImportant(WsfStringId aType);
   void MarkThreatTypeAsUnimportant(WsfStringId aType);

   bool SetScriptAssetScoringFunction(UtScriptContext* aContextPtr, WsfStringId aFunctionName);
   bool SetScriptThreatScoringFunction(UtScriptContext* aContextPtr, WsfStringId aFunctionName);
   bool AssetScore(WsfAssetPerception* aAssetPtr, double& score);
   bool ThreatScore(WsfLocalTrack* aThreatPtr, double& score);
   //@}

   //! @name Timing & max load for updates
   //@{
   void AddExtraCognitiveLoading(double timeDelay);

   double AssetUpdateInterval() { return mAssetUpdateInterval; }
   void   SetAssetUpdateInterval(double aInterval);

   double ThreatUpdateInterval() { return mThreatUpdateInterval; }
   void   SetThreatUpdateInterval(double aInterval);

   double MaxAssetLoad() { return mMaxAssetLoad; }
   void   SetMaxAssetLoad(int aLoad) { mMaxAssetLoad = aLoad; }

   double MaxThreatLoad() { return mMaxThreatLoad; }
   void   SetMaxThreatLoad(int aLoad) { mMaxThreatLoad = aLoad; }
   //@}

   //! asset status reporting interval
   double StatusMessageReportInterval() { return mStatusMessageReportInterval; }
   //! standard query
   bool HasExternalLinks() { return mExternalLinks.HasLinks(); }

   //! utility method for when asset knowledge is truth data
   std::set<WsfPlatform*> TruthAssets(); // set of assets according to:    mTruthPerceptionMask

protected:
   WsfPerceptionProcessor(const WsfPerceptionProcessor& aSrc);

   double TryUpdateThreats(double aSimTime);
   void   UpdateThreats(double aSimTime);

   double TryUpdateAssets(double aSimTime); // returns appropriate time for next event update of assets
   void   UpdateAssets(double aSimTime);

   double SendStatus(double aSimTime);

   UtCallbackHolder mCallbacks;

   WsfTrackManager* mTrackManagerPtr;

   WsfHeatMap* mHeatMapPtr;

   double mStatusMessageReportInterval;
   bool   mReportingSelfStatus;
   bool   mReportingOthersStatus;
   bool   mPerceiveSelfStatus;
   double mThreatUpdateInterval; // seconds between threat perception updates     (negative value means always updated)
   double mAssetUpdateInterval;  // seconds between asset perception updates      (negative value means always updated)
   int    mMaxThreatLoad;        // max number of threats that can be perceived   (negative value means no limit)
   int    mMaxAssetLoad;         // max number of assets that can be perceived    (negative value means no limit)
   AssetPerceptionType mAssetPerception;
   unsigned int        mTruthPerceptionMask; // WsfComm::  cCOMMANDER | cPEERS | cSUBORDINATES

   double mLastAssetUpdateTime;
   double mLastThreatUpdateTime;

   std::vector<WsfLocalTrack*>      mPerceivedThreats;
   std::vector<WsfAssetPerception*> mPerceivedAssets;

   std::map<size_t, WsfAssetPerception*> mReceivedAssetStatus;

   bool Important(WsfLocalTrack* aThreatPtr);
   bool Important(WsfPlatform* aAssetPtr);
   bool Important(size_t aPlatformIndex);
   bool Unimportant(WsfLocalTrack* aThreatPtr);
   bool Unimportant(WsfPlatform* aAssetPtr);
   bool Unimportant(size_t aPlatformIndex);

   std::map<size_t, int> mImportantThreats;   // should be included in perception (track's target platform index -> 1)
   std::map<size_t, int> mImportantAssets;    // should be included in perception (asset's platform index -> 1)
   std::map<size_t, int> mUnimportantThreats; // should NOT be included in perception (track's target platform index -> 1)
   std::map<size_t, int> mUnimportantAssets; // should NOT be included in perception (asset's platform index -> 1)

   std::map<WsfStringId, int> mImportantThreatTypes;
   std::map<WsfStringId, int> mImportantAssetTypes;
   std::map<WsfStringId, int> mUnimportantThreatTypes;
   std::map<WsfStringId, int> mUnimportantAssetTypes;

   std::vector<double> mExtraCognitiveDelaysThreats;
   std::vector<double> mExtraCognitiveDelaysAssets;

   UtScriptContext* mAssetFunctionScriptContextPtr;
   WsfStringId      mAssetFunctionScriptName;
   UtScript*        mAssetFunctionScriptPtr;
   UtScriptContext* mThreatFunctionScriptContext;
   WsfStringId      mThreatFunctionScriptName;
   UtScript*        mThreatFunctionScriptPtr;
};


// An event to force cognitive model updates
class WSF_MIL_EXPORT WsfPerceptionProcessorUpdateEvent : public WsfEvent
{
public:
   enum CognitiveUpdateType
   {
      THREATS,
      ASSETS,
      STATUS
   };

   WsfPerceptionProcessorUpdateEvent(double                  aSimTime,
                                     size_t                  aPlatformIndex,
                                     WsfPerceptionProcessor* aModelPtr,
                                     CognitiveUpdateType     aType);

   EventDisposition Execute() override;

private:
   size_t                  mPlatformIndex;
   WsfPerceptionProcessor* mPercProcPtr;
   CognitiveUpdateType     mType;
};


class WSF_MIL_EXPORT WsfScriptPerceptionProcessor : public WsfScriptProcessorClass
{
public:
   WsfScriptPerceptionProcessor(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   void*            Create(const UtScriptContext& aContext) override;
   void*            Clone(void* aObjectPtr) override;
   void             Destroy(void* aObjectPtr) override;
   UtScriptContext* GetContext(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(PerceivedAssets);
   UT_DECLARE_SCRIPT_METHOD(PerceivedThreats);
   UT_DECLARE_SCRIPT_METHOD(NearestAsset);
   UT_DECLARE_SCRIPT_METHOD(NearestThreat);
   UT_DECLARE_SCRIPT_METHOD(AddExtraCognitiveLoading);

   UT_DECLARE_SCRIPT_METHOD(SetAssetImportant);
   UT_DECLARE_SCRIPT_METHOD(SetAssetImportant_2);
   UT_DECLARE_SCRIPT_METHOD(SetAssetUnimportant);
   UT_DECLARE_SCRIPT_METHOD(SetAssetUnimportant_2);

   UT_DECLARE_SCRIPT_METHOD(SetThreatImportant);
   UT_DECLARE_SCRIPT_METHOD(SetThreatUnimportant);

   UT_DECLARE_SCRIPT_METHOD(SetAssetTypeImportant);
   UT_DECLARE_SCRIPT_METHOD(SetAssetTypeUnimportant);
   UT_DECLARE_SCRIPT_METHOD(SetThreatTypeImportant);
   UT_DECLARE_SCRIPT_METHOD(SetThreatTypeUnimportant);

   UT_DECLARE_SCRIPT_METHOD(GetNearestHeat);
   UT_DECLARE_SCRIPT_METHOD(GetNearestMaxHeat);
   UT_DECLARE_SCRIPT_METHOD(GetHeat);
   UT_DECLARE_SCRIPT_METHOD(GetHeatSourcesCount);

   // TODO : add accessors for threat/asset update interval & max loading???
};


#endif
