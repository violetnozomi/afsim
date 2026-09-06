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

#ifndef WSFWEAPONTHREATPROCESSOR_HPP
#define WSFWEAPONTHREATPROCESSOR_HPP

#include "wsf_mil_export.h"

#include <cfloat>
#include <cstring>
#include <list>
#include <map>
#include <sstream>
#include <vector>

#include "UtEntity.hpp"
class UtLineSegment;

#include "WsfLocalTrack.hpp"
class WsfScenario;
class WsfScriptContext;
#include "WsfTrackList.hpp"
#include "script/WsfScriptProcessor.hpp"

//! A script(able) processor that uses tracks to invoke
//! its classifier, and then create a list of weapon threat
//! reports for a given platform (ownship).
//!
class WSF_MIL_EXPORT WsfWeaponThreatProcessor : public WsfScriptProcessor
{
public:
   enum QuerySort
   {
      QUERY_SORT_PK_T0  = 0,
      QUERY_SORT_PT_T0  = 1,
      QUERY_SORT_PK_MAX = 2,
      QUERY_SORT_PT_MAX = 3
   };

   struct PkSearchParameters
   {
      double    mDuration = std::numeric_limits<double>::max();
      QuerySort mQuerySort;
   };

   struct WsfThreatReport
   {
      WsfStringId mTrackNameId;
      WsfTrackId  mTrackId;
      // int         mEntityTypeId;
      WsfStringId mEntityTypeId;
      // int         mWeaponTypeId;
      WsfStringId mWeaponTypeId;
      // probability of entity type
      double mPe;
      // probability of weapons on-board
      double mPw;
      // probability of kill at time zero
      double mPk_t0;
      // probability of kill at time zero
      double mPt_t0;
      // time of max probability of threat
      double mTimePtMax;
      // probability of kill max
      double mPk_max;
      // probability of threat max
      double mPt_max;
      // probability of detection (1.0)
      double mPoD;
   };

   struct ThreatReportComparitor
   {
      ThreatReportComparitor(QuerySort aQuerySort)
         : mQuerySort(aQuerySort)
      {
      }

      bool operator()(const WsfThreatReport& i, const WsfThreatReport& j)
      {
         bool greater = i.mPt_max > j.mPt_max;
         switch (mQuerySort)
         {
         case QUERY_SORT_PK_T0:
            greater = i.mPk_t0 > j.mPk_t0;
            break;
         case QUERY_SORT_PT_T0:
            greater = i.mPt_t0 > j.mPt_t0;
            break;
         case QUERY_SORT_PK_MAX:
            greater = i.mPk_max > j.mPk_max;
            break;
         case QUERY_SORT_PT_MAX:
            greater = i.mPt_max > j.mPt_max;
            break;
         }
         return greater;
      }

      QuerySort mQuerySort;
   };

   using WsfThreatReportList = std::vector<WsfThreatReport>;

   /** The worker interface for the private implementation.
    */
   class ThreatEvaluator
   {
   public:
      ThreatEvaluator(WsfScenario& aScenario);
      ThreatEvaluator(const ThreatEvaluator& aSrc) = default;
      ThreatEvaluator& operator=(const ThreatEvaluator&) = default;
      virtual ~ThreatEvaluator()                         = default;

      virtual ThreatEvaluator* Clone() const                 = 0;
      virtual bool             ProcessInput(UtInput& aInput) = 0;

      PkSearchParameters& GetPkSearchParameters() { return mPkSearchParameters; }

      /** Appends/inserts any threats from this track into the given list.
       * So the caller can iterate over multiple tracks, and keep adding to
       * the list.
       */
      virtual void CreateThreatList(const WsfTrack&      aShooterTrack,
                                    WsfPlatform&         aOwnship,
                                    WsfThreatReportList& aThreatReportList) = 0;

      WsfScenario* GetScenario() { return mScenarioPtr; }

   private:
      PkSearchParameters mPkSearchParameters;
      WsfScenario*       mScenarioPtr;
   };

   WsfWeaponThreatProcessor(WsfScenario& aScenario);
   WsfWeaponThreatProcessor(const WsfWeaponThreatProcessor& aSrc);
   WsfWeaponThreatProcessor& operator=(const WsfWeaponThreatProcessor&) = delete;
   ~WsfWeaponThreatProcessor() override;

   bool ProcessInput(UtInput& aInput) override;

   bool Initialize(double aSimTime) override;

   WsfProcessor* Clone() const override { return new WsfWeaponThreatProcessor(*this); }

   // This override IS critical!!!!!  If not implemented, the scripts calls
   // just fails quietly!!!!!!!!!!!!
   const char* GetScriptClassName() const override { return "WsfWeaponThreatProcessor"; }

   bool ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;

   // This function iterates on the given platforms tracks and finds all threats
   // from them against itself.
   void CreateThreatList(WsfPlatform& aOwnship);

   // This function appends the threat list, with threats from the given track.
   void CreateThreatList(const WsfTrack& aShooterTrack, WsfPlatform& aOwnship, WsfThreatReportList& aThreatReportList);

   // Script focused functions.
   void SetQueryTime(double aQueryTime) { mThreatEvaluator->GetPkSearchParameters().mDuration = aQueryTime; }
   // void SetQueryFanDeg(double aQueryArcDeg)  { mThreatEvaluator->GetPkSearchParameters().mArcFan=
   // UtMath::cRAD_PER_DEG*aQueryArcDeg; }
   void SetQuerySort(QuerySort aQuerySort) { mThreatEvaluator->GetPkSearchParameters().mQuerySort = aQuerySort; }

   /** Running any query clears and also fills this list. Then the script accessor accesses its elements. */
   /*const */ WsfThreatReportList& GetThreatList() { return mThreatReportList; }

private:
   ThreatEvaluator*    mThreatEvaluator;
   WsfThreatReportList mThreatReportList;
};

#endif
