// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2022 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef RUNDATA_HPP
#define RUNDATA_HPP

#include <map>

#include "TimeBounds.hpp"
#include "TimeScale.hpp"
#include "V1RecordType.hpp"
#include "parseAfperf.hpp"

namespace artificer
{
class TextTable;

///@brief Collects information about a single run within an afperf file
class RunData
{
public:
   ///@brief Updates this objects statistics to reflect the given record
   void CollectStats(const artificer::AfperfRecord& aRecord);
   ///@brief Reduces this object's statistics to a summarized log for human consumption.
   std::string RenderStatisticsSummary() const;

private:
   ///@brief Throws if the given `aRecord` does not have at least the minimum numbe of values required by `aEventType`
   void VerifyRecordSize(const V1RecordType aEventType, const artificer::AfperfRecord& aRecord);
   ///@brief Updates this object's data for a MeasurementType (type==1) record from `aRecord`
   void CollectMeasurementType(const artificer::AfperfRecord& aRecord);
   ///@brief Updates this object's data to reflect a region record of type `aEventType`, at time `aTimestamp`, and
   /// values in `aRecord`
   void CollectRegionStats(const V1RecordType aEventType, const int64_t aTimestamp, const artificer::AfperfRecord& aRecord);
   ///@brief Update's this object's data to reflect a run record of type `aEventType`, at time `aTimestamp`, and values in `aRecord`
   void CollectRunStats(const V1RecordType aEventType, const int64_t aTimestamp, const artificer::AfperfRecord& aRecord);
   ///@brief Update's this object's data to reflect a section record of type `aEventType`, at time `aTimestamp`, and
   /// values in `aRecord`
   void CollectSectionStats(const V1RecordType aEventType, const int64_t aTimestamp, const artificer::AfperfRecord& aRecord);
   ///@brief Computes the length of time between the start time of the top region stack frame and the given `aEndTime`
   int64_t CurrentRegionDeltaT(const int64_t aEndTime) const;

   ///@ brief Walks the current region stack looking for the region with ID == `aRegionID`.  Returns its label if found.
   ut::optional<std::string> RegionLabelForID(const int64_t aRegionID);

   using MeasurementTypeID = int64_t;
   using Tags              = std::map<std::string, std::string>;

   struct Measurement
   {
      MeasurementTypeID type  = 0;
      double            value = 0.0;
   };
   using Measurements = std::vector<Measurement>;

   ///@brief Parses measurement values from the given `aRecord`, beginning at index `aFirstValueIndex`
   static const std::vector<Measurement> GetMeasurements(const size_t                   aFirstValueIndex,
                                                         const artificer::AfperfRecord& aRecord);

   struct Point
   {
      int64_t      timeStamp;
      Measurements measurements;
   };
   using Points = std::vector<Point>;

   struct Aggregate
   {
      TimeBounds   time;
      std::string  aggregateType;
      Measurements measurements;
   };
   using Aggregates = std::vector<struct Aggregate>;

   struct RegionData
   {
      std::string             label;
      size_t                  timesStarted = 0;
      Tags                    tags;
      Points                  points;
      std::vector<TimeBounds> runTimes;
      Aggregates              aggregates;
   };

   struct RegionStackFrame
   {
      int64_t     startTime = 0;
      std::string label;
      int64_t     regionID = 0;
   };

   struct SectionPoint : public Point
   {
      int64_t interval = 0;
   };
   using SectionPoints = std::vector<SectionPoint>;

   struct SectionIntervalData
   {
      TimeBounds time;
      Aggregates aggregates;
   };

   struct SectionData
   {
      std::string                            label;
      Tags                                   tags;
      SectionPoints                          points;
      std::map<int64_t, SectionIntervalData> intervals;
      std::map<std::string, Aggregates>      aggregatesByType;
   };

   struct MeasurementUnit
   {
      std::string name;
      std::string datatype;
      std::string unit;
      std::string summary;
      std::string description;
   };

   ///@brief Returns a table showing the counts of the types of records logged in the run.
   TextTable RecordCountTable() const;
   ///@brief Returns a table showing the times of the longest regions logged in the run.
   TextTable LongestTimeRegionsTable() const;
   ///@brief Returns a table of region aggregates logged in the run.
   TextTable RegionAggregatesTable() const;
   ///@brief Returns a table of region points logged in the run.
   TextTable RegionPointsTable() const;
   ///@brief Returns a table of section aggregates ordered by their types logged in the run.
   TextTable SectionAggregatesByTypeTable() const;
   ///@brief Returns a table of section aggregates logged in the run.
   TextTable SectionAggregateTable() const;
   ///@brief Returns a table of section points logged in the run.
   TextTable SectionPointsTable() const;
   ///@brief Returns a human-readable string representing `aTime` after epoch
   std::string TimePointString(int64_t aTime) const;

   using RecordCountMap     = std::map<V1RecordType, uint64_t>;
   using RegionDataMap      = std::map<std::string, RegionData>;
   using MeasurementTypeMap = std::map<MeasurementTypeID, MeasurementUnit>;

   MeasurementTypeMap             mMeasurementTypes;
   TimeBounds                     mTimestampBounds;
   RecordCountMap                 mRecordCounts;
   std::vector<RegionStackFrame>  mRegionStack; // back is the top.
   size_t                         mMaxStackDepth = 0;
   RegionDataMap                  mRegionData;
   TimeScale                      mTimeScale = TimeScale::Nanoseconds;
   int64_t                        mTimeZero  = 0;
   std::vector<Point>             mRunPoints;
   std::vector<Aggregate>         mRunAggregates;
   std::vector<Aggregate>         mProcessedSectionAggregates;
   std::vector<Aggregate>         mRegionAggregates;
   std::map<int64_t, SectionData> mSectionData;
   std::vector<TimeBounds>        mPauses;
};

} // namespace artificer

#endif
