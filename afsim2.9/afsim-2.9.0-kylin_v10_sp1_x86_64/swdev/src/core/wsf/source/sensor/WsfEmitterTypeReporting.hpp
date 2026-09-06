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

#ifndef WSFEMITTERTYPEREPORTING_HPP
#define WSFEMITTERTYPEREPORTING_HPP

#include "wsf_export.h"

#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

class UtInput;
#include "UtRandom.hpp"
class WsfEM_Xmtr;
class WsfObject;
class WsfPlatform;
#include "WsfSensor.hpp"
#include "WsfStringId.hpp"
class WsfTrack;

class WSF_EXPORT WsfEmitterTypeReporting
{
public:
   WsfEmitterTypeReporting();
   WsfEmitterTypeReporting(const WsfEmitterTypeReporting& aSrc);
   WsfEmitterTypeReporting& operator=(const WsfEmitterTypeReporting& aRhs);
   ~WsfEmitterTypeReporting()       = default;

   bool ProcessInput(UtInput& aInput);

   void AddEmitterReportState(double aSimTime, WsfEM_Xmtr* aXmtrPtr);

   void RemoveAllEmitterReportStates();

   void RemoveEmitterReportState(size_t aPlatformIndex, unsigned int aUniqueXmtrPartId);

   void UpdateReportedEmitterType(double           aSimTime,
                                  bool             aSensorReportsType,
                                  WsfSensorResult& aResult,
                                  WsfPlatform*     aTarget,
                                  WsfTrack*        aTrackPtr);

   //! Type definition for the confidence list for reporting detection confidences.
   using ConfidenceList = std::vector<std::pair<double, WsfStringId>>;

   struct ReportingRules
   {
      enum ReportingType
      {
         cTABLE,
         cTRUTH,
         cNOTHING
      };

      double DrawForType(ut::Random& aRandom, WsfStringId& aReportedEmitterType);

      double         mTimeToDeclare;
      double         mTimeToReevaluate;
      ConfidenceList mConfidenceTable;
      ReportingType  mReportingType;
   };

   using ReportingRulesRef = std::shared_ptr<ReportingRules>;
   using ReportingRulesMap = std::map<WsfStringId, ReportingRulesRef>;

   struct ReportState
   {
      unsigned int      mUniqueXmtrPartId;
      double            mReportConfidence;
      WsfStringId       mTruthId;
      WsfStringId       mEmitterDerivedId;
      double            mNextTimeToEvaluate;
      ReportingRulesRef mRules;
   };

private:
   using TargetEmitterPair = std::pair<size_t, unsigned int>;
   using ReportStateMap    = std::map<TargetEmitterPair, ReportState>;

   void ProcessEmitterReportTypes(UtInput& aInput);
   void ProcessTruthTypeInput(UtInput& aInput);

   void AddEmitterReportTypes(UtInput&                aInput,
                              ReportingRulesRef       aReportingRules,
                              bool                    aIsDefault,
                              std::list<WsfStringId>& aTypeList);

   ReportingRulesRef FindReportingRules(const WsfObject& aEmitterType);

   ReportingRulesRef NewReportingRules();

   ReportingRulesMap mEmitterReportingRules;
   ReportingRulesRef mDefaultEmitterRules;
   double            mDefaultTimeToDeclare;
   double            mDefaultTimeToReevaluate;
   bool              mReportingRulesSupplied;

   ReportStateMap mEmitterReportState;
};

#endif
