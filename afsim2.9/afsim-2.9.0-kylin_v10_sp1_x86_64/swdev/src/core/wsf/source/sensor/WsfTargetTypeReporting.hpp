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

#ifndef WSFTARGETTYPEREPORTING_HPP
#define WSFTARGETTYPEREPORTING_HPP

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

class WSF_EXPORT WsfTargetTypeReporting
{
public:
   WsfTargetTypeReporting();
   WsfTargetTypeReporting(const WsfTargetTypeReporting& aSrc);
   WsfTargetTypeReporting& operator=(const WsfTargetTypeReporting& aSrc) = delete;
   ~WsfTargetTypeReporting()                                             = default;

   bool ProcessInput(UtInput& aInput);

   void AddTargetReportState(double aSimTime, WsfPlatform* aTargetPtr);

   void RemoveAllTargetReportStates();

   void RemoveTargetReportState(size_t aPlatformIndex);

   void UpdateReportedTargetType(double           aSimTime,
                                 bool             aSensorReportsType,
                                 WsfSensorResult& aResult,
                                 WsfPlatform*     aTargetPtr,
                                 WsfTrack*        aTrackPtr);

   //! Type definition for the confidence list for reporting detection confidences.
   using ConfidenceList = std::vector<std::pair<double, WsfStringId>>;
   using EmitterList    = std::vector<std::pair<std::list<WsfStringId>, WsfStringId>>;

   struct ReportingRules
   {
      enum ReportingType
      {
         cEMITTERS,
         cTABLE,
         cTRUTH,
         cNOTHING
      };

      double DrawForType(ut::Random& aRandom, WsfStringId& aReportedTargetType);
      double DrawForType(std::list<WsfStringId>& aEmitterList, WsfStringId& aReportedTargetType);

      double         mTimeToDeclare;
      double         mTimeToReevaluate;
      ConfidenceList mConfidenceTable;
      EmitterList    mEmitterTable;
      ReportingType  mReportingType;
   };

   using ReportingRulesRef = std::shared_ptr<ReportingRules>;
   using ReportingRulesMap = std::map<WsfStringId, ReportingRulesRef>;

   struct ReportState
   {
      unsigned int      mUniqueId;
      double            mReportConfidence;
      WsfStringId       mTruthId;
      WsfStringId       mTargetDerivedId;
      double            mNextTimeToEvaluate;
      ReportingRulesRef mRules;
   };

private:
   using ReportStateMap = std::map<size_t, ReportState>;

   // void ProcessTargetReportTypes(UtInput& aInput);
   // void ProcessTruthTypeInput(UtInput& aInput);

   void AddTargetReportTypes(UtInput&                aInput,
                             ReportingRulesRef       aReportingRules,
                             bool                    aIsDefault,
                             std::list<WsfStringId>& aTypeList);

   ReportingRulesRef FindReportingRules(const WsfObject& aTargetType);

   ReportingRulesRef NewReportingRules();

   ReportingRulesMap mTargetReportingRules;
   ReportingRulesRef mDefaultTargetRules;
   double            mDefaultTimeToDeclare;
   double            mDefaultTimeToReevaluate;
   bool              mReportingRulesSupplied;
   ReportStateMap    mTargetReportState;
};

#endif
