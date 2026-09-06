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

#include "RunData.hpp"

#include <sstream>

#include "TextTable.hpp"
#include "TimeBounds.hpp"
#include "TimeScale.hpp"
#include "V1RecordType.hpp"
#include "mapUtil.hpp"
#include "parseUtil.hpp"
#include "statistics.hpp"

namespace artificer
{
void RunData::CollectStats(const artificer::AfperfRecord& aRecord)
{
   const auto recordType = RecordTypeFrom(aRecord);
   VerifyRecordSize(recordType, aRecord);
   const int64_t timestamp = stoll(aRecord[1]);
   mTimestampBounds.Union(timestamp);
   IncrementCount(mRecordCounts, recordType);

   switch (recordType)
   {
   case V1RecordType::MeasurementType:
      CollectMeasurementType(aRecord);
      break;

   case V1RecordType::PauseResume:
   {
      // PauseResume,<end timestamp>,<start timestamp>
      TimeBounds pauseTime;
      pauseTime.Union(stoll(aRecord[1]));
      pauseTime.Union(stoll(aRecord[2]));
      mPauses.push_back(pauseTime);
      break;
   }

   case V1RecordType::RegionAggregate:
   case V1RecordType::RegionPoint:
   case V1RecordType::RegionStart:
   case V1RecordType::RegionStop:
      CollectRegionStats(recordType, timestamp, aRecord);
      break;

   case V1RecordType::RunAggregate:
   case V1RecordType::RunInfo:
   case V1RecordType::RunPoint:
      CollectRunStats(recordType, timestamp, aRecord);
      break;

   case V1RecordType::SectionAggregate:
   case V1RecordType::SectionInfo:
   case V1RecordType::SectionPoint:
   case V1RecordType::SectionStart:
   case V1RecordType::SectionStop:
      CollectSectionStats(recordType, timestamp, aRecord);
      break;
   default:
      // ignore unknown types.
      break;
   }
}

std::string RunData::RenderStatisticsSummary() const
{
   std::stringstream stream;
   if (mTimestampBounds.min.has_value())
   {
      stream << "First event time:   " << TimePointString(mTimestampBounds.min.value_or(0)) << std::endl
             << "Last event time:    " << TimePointString(mTimestampBounds.max.value_or(0)) << std::endl;
   }

   const std::vector<std::pair<TextTable, std::string>> statGroups = {{RecordCountTable(), "Event Counts"},
                                                                      {LongestTimeRegionsTable(), "Region times"},
                                                                      {RegionAggregatesTable(), "Region Aggregates"},
                                                                      {RegionPointsTable(), "Region Points"},
                                                                      {SectionAggregateTable(),
                                                                       "Section aggregate measurements"},
                                                                      //{SectionAggregatesByTypeTable(), "Section
                                                                      // aggregates by type"},
                                                                      {SectionPointsTable(), "Section points"}};

   for (const auto& group : statGroups)
   {
      if (group.first.RowCount() > 0)
      {
         stream << '\n' << group.second << '\n' << group.first;
      }
   }

   return stream.str();
}

void RunData::VerifyRecordSize(const V1RecordType aType, const artificer::AfperfRecord& aRecord)
{
   const size_t minimum = MinimumRecordFields(aType);
   if (aRecord.size() < minimum)
   {
      throw std::runtime_error("Invalid record of type " + ToTypeString(aType) + "; expected " + std::to_string(minimum) +
                               " fields, found " + std::to_string(aRecord.size()) + " fields.");
   }
}

void RunData::CollectRegionStats(const V1RecordType aType, const int64_t aTimestamp, const artificer::AfperfRecord& aRecord)
{
   if (aType == V1RecordType::RegionAggregate)
   {
      // RegionAggregate,<end timestamp>,<start timestamp>,<record id>,<aggregation type>,<measurement type
      // id>,<value>[,<measurement type id>,<value>]
      const TimeBounds   timeBounds(stoll(aRecord[1]), stoll(aRecord[2]));
      const std::string& aggregation = aRecord[4];
      if (!aRecord[3].empty())
      {
         const int64_t                   recordID   = stoll(aRecord[3]);
         const ut::optional<std::string> foundLabel = RegionLabelForID(recordID);
         if (!foundLabel)
         {
            throw std::runtime_error("Unable to map region ID " + std::to_string(recordID) + " to a label");
         }
         mRegionData[*foundLabel].aggregates.push_back({timeBounds, aggregation, GetMeasurements(6, aRecord)});
      }
      else
      {
         mRegionAggregates.push_back({timeBounds, aggregation, GetMeasurements(6, aRecord)});
      }
   }
   else if (aType == V1RecordType::RegionPoint)
   {
      // RegionPoint,<timestamp>,<region id>,<measurement type id>,<value>[,<measurement type id>,<value>]
      const int64_t                   regionID = stoll(aRecord[2]);
      const ut::optional<std::string> label    = RegionLabelForID(regionID);
      if (label.has_value())
      {
         mRegionData[*label].points.push_back({aTimestamp, GetMeasurements(4, aRecord)});
      }
      else
      {
         throw std::runtime_error("The region ID given in a RegionPoint record does not match a region on the stack.");
      }
   }
   else if (aType == V1RecordType::RegionStart)
   {
      // RegionStart,<timestamp>,<run id>,<region id>,<region label>,<tags>
      const int64_t      regionID    = stoll(aRecord[3]);
      const std::string& regionLabel = aRecord[4];
      RegionStackFrame   frame;
      frame.startTime = aTimestamp;
      frame.label     = regionLabel;
      frame.regionID  = regionID;
      mRegionStack.push_back(frame);
      mMaxStackDepth   = std::max(mMaxStackDepth, mRegionStack.size());
      RegionData& data = mRegionData[regionLabel];
      data.timesStarted++;
      data.label = regionLabel;
      data.tags  = ParseNameValuePairs(aRecord[5]);
   }
   else if (aType == V1RecordType::RegionStop)
   {
      // RegionStop,<timestamp>,<region id>
      mRegionData[mRegionStack.back().label].runTimes.push_back(TimeBounds(mRegionStack.back().startTime, aTimestamp));
      mRegionStack.pop_back();
   }
}

void RunData::CollectRunStats(const V1RecordType aType, const int64_t aTimestamp, const artificer::AfperfRecord& aRecord)
{
   if (aType == V1RecordType::RunAggregate)
   {
      // RunAggregate,<end timestamp>,<start timestamp>,<run id>,<aggregation type>,<measurement type
      // id>,<value>[,<measurement type id>,<value>]
      const TimeBounds   timeBounds(stoll(aRecord[1]), stoll(aRecord[2]));
      const std::string& aggregationType = aRecord[4];
      mRunAggregates.push_back({timeBounds, aggregationType, GetMeasurements(6, aRecord)});
   }
   else if (aType == V1RecordType::RunInfo)
   {
      // RunInfo,<start timestamp>,<timestamp units>,<wallclock time>,<afperf format version>,<run id>,<application
      // name>,<application version>,<tags>
      mTimeZero = aTimestamp;
      if (aRecord[2] == "ns")
      {
         mTimeScale = TimeScale::Nanoseconds;
      }
      else if (aRecord[2] == "us")
      {
         mTimeScale = TimeScale::Microseconds;
      }
      else if (aRecord[2] == "ms")
      {
         mTimeScale = TimeScale::Milliseconds;
      }
      else if (aRecord[2] == "s")
      {
         mTimeScale = TimeScale::Seconds;
      }
   }
   else if (aType == V1RecordType::RunPoint)
   {
      // RunPoint,<timestamp>,<run id>,<measurement type id>,<value>
      mRunPoints.push_back({aTimestamp, GetMeasurements(4, aRecord)});
   }
}

void RunData::CollectSectionStats(const V1RecordType aType, const int64_t aTimestamp, const artificer::AfperfRecord& aRecord)
{
   if (aType == V1RecordType::SectionAggregate)
   {
      // SectionAggregate,<end timestamp>,<start timestamp>,<record id>,<section interval id>,<aggregation
      // type>,<measurement type id>,<value>[,<measurement type id>,<value>]
      TimeBounds timeBounds;
      timeBounds.Union(stoll(aRecord[1]));
      timeBounds.Union(stoll(aRecord[2]));
      std::vector<Measurement> measurements    = GetMeasurements(7, aRecord);
      const std::string&       aggregationType = aRecord[5];
      if (!aRecord[3].empty())
      {
         const int64_t recordID     = stoll(aRecord[3]);
         const int64_t intervalID   = stoll(aRecord[4]);
         auto&         intervalData = mSectionData[recordID].intervals[intervalID];
         intervalData.time.Union(timeBounds);
         intervalData.aggregates.push_back({timeBounds, aggregationType, measurements});
         mSectionData[recordID].aggregatesByType[aggregationType].push_back(
            Aggregate{timeBounds, aggregationType, measurements});
      }
      else
      {
         mProcessedSectionAggregates.push_back({timeBounds, aggregationType, std::move(measurements)});
      }
   }
   else if (aType == V1RecordType::SectionInfo)
   {
      // SectionInfo,<timestamp>,<run id>,<section id>,<section label>,<tags>
      const int64_t sectionID = stoll(aRecord[3]);
      SectionData&  data      = mSectionData[sectionID];
      data.label              = aRecord[4];
      data.tags               = ParseNameValuePairs(aRecord[5]);
   }
   else if (aType == V1RecordType::SectionPoint || aType == V1RecordType::SectionStart || aType == V1RecordType::SectionStop)
   {
      // SectionStart,<timestamp>,<section id>,<section interval id>
      // SectionStop,<timestamp>,<section id>,<section interval id>
      // SectionPoint,<timestamp>,<section id>,<section interval id>,<measurement type id>,<value>[,<measurement type id>,<value>]
      const int64_t sectionID  = stoll(aRecord[2]);
      const int64_t intervalID = stoll(aRecord[3]);
      SectionData&  data       = mSectionData[sectionID];
      if (aType == V1RecordType::SectionStop || aType == V1RecordType::SectionStart)
      {
         data.intervals[intervalID].time.Union(aTimestamp);
      }
      else
      {
         SectionPoint point;
         point.interval     = intervalID;
         point.timeStamp    = aTimestamp;
         point.measurements = GetMeasurements(5, aRecord);
         data.points.push_back(std::move(point));
      }
   }
}

int64_t RunData::CurrentRegionDeltaT(const int64_t aEndTime) const
{
   if (mRegionStack.empty())
   {
      throw std::runtime_error(
         "Unable to calculate current region time delta because no there is no matching start event. Ending time = " +
         std::to_string(aEndTime));
   }
   return aEndTime - mRegionStack.back().startTime;
}

ut::optional<std::string> RunData::RegionLabelForID(const int64_t aRegionID)
{
   // Do we need to do better than linear here?
   for (auto frame = mRegionStack.rbegin(); frame != mRegionStack.rend(); ++frame)
   {
      if (frame->regionID == aRegionID)
      {
         return frame->label;
      }
   }
   return ut::optional<std::string>();
}

const std::vector<RunData::Measurement> RunData::GetMeasurements(const size_t                   aFirstValueIndex,
                                                                 const artificer::AfperfRecord& aRecord)
{
   std::vector<Measurement> measurements;
   for (size_t valueIdx = aFirstValueIndex; valueIdx < aRecord.size(); valueIdx += 2)
   {
      const size_t typeIdx = valueIdx - 1;
      Measurement  measurement;
      measurement.type  = stoll(aRecord[typeIdx]);
      measurement.value = stod(aRecord[valueIdx]);
      measurements.push_back(measurement);
   }
   return measurements;
}

void RunData::CollectMeasurementType(const artificer::AfperfRecord& aRecord)
{
   // MeasurementType,<timestamp>,<run id>,<measurement type id>,<name>,<datatype>,<units>,<summary>,<description>
   const MeasurementTypeID type = stoll(aRecord[3]);
   mMeasurementTypes[type]      = {aRecord[4], aRecord[5], aRecord[6], aRecord[7], aRecord[8]};
}

TextTable RunData::RecordCountTable() const
{
   auto RecordCountDescending = [](const RecordCountMap::value_type* a, const RecordCountMap::value_type* b) -> bool
   { return a->second > b->second; };
   const std::vector<const RecordCountMap::value_type*> reorderedData = MapOrderedBy(mRecordCounts, RecordCountDescending);
   TextTable::TableData                                 eventCountRows;
   std::transform(reorderedData.begin(),
                  reorderedData.end(),
                  std::inserter(eventCountRows, eventCountRows.end()),
                  [](const RecordCountMap::value_type* evtCount) -> TextTable::TableRow {
                     return {std::to_string(evtCount->second), ToTypeString(evtCount->first)};
                  });

   return TextTable({TextTable::Column("Count", 15), TextTable::Column("Event", 62)}, eventCountRows);
}

TextTable RunData::LongestTimeRegionsTable() const
{
   const size_t         doubleColWidth = 14;
   TextTable::TableData regionTimeTable;
   for (const auto& regionData : mRegionData)
   {
      std::vector<int64_t> withPauses;
      for (const auto& runTime : regionData.second.runTimes)
      {
         if (runTime.HasValues())
         {
            withPauses.push_back(*runTime.DeltaExcluding(mPauses));
         }
      }
      const auto minmax = std::minmax_element(withPauses.begin(), withPauses.end());
      regionTimeTable.push_back({regionData.second.label,
                                 std::to_string(withPauses.size()),
                                 DurationString(AccumulateToType<int64_t>(withPauses), mTimeScale),
                                 DurationString(*minmax.first, mTimeScale),
                                 DurationString(*minmax.second, mTimeScale),
                                 DurationString(static_cast<int64_t>(Average(withPauses)), mTimeScale),
                                 DurationString(static_cast<int64_t>(StdDev(withPauses)), mTimeScale)});
   }

   return TextTable({TextTable::Column("Label", 40),
                     TextTable::Column("Count", doubleColWidth),
                     TextTable::Column("Total", doubleColWidth),
                     TextTable::Column("Min", doubleColWidth),
                     TextTable::Column("Max", doubleColWidth),
                     TextTable::Column("Avg", doubleColWidth),
                     TextTable::Column("StdDev", doubleColWidth)},
                    regionTimeTable);
}

TextTable RunData::RegionAggregatesTable() const
{
   TextTable::TableData tableData;
   auto                 createAggregateRows = [this, &tableData](const Aggregate&          aggr,
                                                 const MeasurementTypeMap& measurementTypes,
                                                 bool&                     regionFirst,
                                                 const std::string&        regionLabel)
   {
      bool aggregateFirst = true;
      for (const auto& measurement : aggr.measurements)
      {
         const auto foundType = measurementTypes.find(measurement.type);
         tableData.push_back(
            {regionFirst ? regionLabel : "",
             aggregateFirst ? TimePointString(*aggr.time.min) : "",
             aggregateFirst ? TimePointString(*aggr.time.max) : "",
             aggregateFirst ? DurationString((*aggr.time.DeltaExcluding(mPauses)), mTimeScale) : "",
             aggregateFirst ? aggr.aggregateType : "",
             foundType != mMeasurementTypes.end() ? foundType->second.name : std::to_string(measurement.type),
             std::to_string(measurement.value)});
         regionFirst = aggregateFirst = false;
      }
   };

   for (const auto& region : mRegionData)
   {
      bool regionFirst = true;
      for (const auto& aggr : region.second.aggregates)
      {
         createAggregateRows(aggr, mMeasurementTypes, regionFirst, region.second.label);
      }
   }

   bool              regionFirst   = true;
   const std::string noRegionLabel = "<no region>";
   for (const auto& aggr : mRegionAggregates)
   {
      createAggregateRows(aggr, mMeasurementTypes, regionFirst, noRegionLabel);
   }

   return TextTable({TextTable::Column("Region", 40),
                     TextTable::Column("From Time", 12),
                     TextTable::Column("To Time", 12),
                     TextTable::Column("Duration", 12),
                     TextTable::Column("Aggr. Type", 12),
                     TextTable::Column("Measr. Type", 14),
                     TextTable::Column("Measr. Value", 14)},
                    tableData);
}


TextTable RunData::RegionPointsTable() const
{
   TextTable::TableData tableData;
   for (const auto& region : mRegionData)
   {
      bool regionFirst = true;
      for (const auto& points : region.second.points)
      {
         bool pointFirst = true;
         for (const auto& measurement : points.measurements)
         {
            const auto foundType = mMeasurementTypes.find(measurement.type);
            tableData.push_back(
               {regionFirst ? region.second.label : "",
                pointFirst ? TimePointString(points.timeStamp) : "",
                foundType != mMeasurementTypes.end() ? foundType->second.name : std::to_string(measurement.type),
                std::to_string(measurement.value)});
            regionFirst = pointFirst = false;
         }
      }
   }

   return TextTable({TextTable::Column("Region", 40),
                     TextTable::Column("Time", 14),
                     TextTable::Column("Measr. Type", 14),
                     TextTable::Column("Measr. Value", 14)},
                    tableData);
}

TextTable RunData::SectionAggregatesByTypeTable() const
{
   TextTable::TableData sectionTableData;
   for (const auto& section : mSectionData)
   {
      bool sectionFirst = true;
      for (const auto& aggregationType : section.second.aggregatesByType)
      {
         bool typeFirst = true;
         for (const auto& aggregate : aggregationType.second)
         {
            bool aggregateFirst = true;
            for (const auto& measurement : aggregate.measurements)
            {
               const auto foundType = mMeasurementTypes.find(measurement.type);
               sectionTableData.push_back(
                  {sectionFirst ? section.second.label : "",
                   typeFirst ? aggregate.aggregateType : "",
                   aggregateFirst ? TimePointString(*aggregate.time.min) : "",
                   aggregateFirst ? TimePointString(*aggregate.time.max) : "",
                   foundType != mMeasurementTypes.end() ? foundType->second.name : std::to_string(measurement.type),
                   std::to_string(measurement.value)});
               sectionFirst = typeFirst = aggregateFirst = false;
            }
         }
      }
   }
   return TextTable({TextTable::Column("Section", 40),
                     TextTable::Column("Aggr. Type", 12),
                     TextTable::Column("From Time", 12),
                     TextTable::Column("To Time", 12),
                     TextTable::Column("Measr. Type", 14),
                     TextTable::Column("Measr. Value", 14)},
                    sectionTableData);
}

TextTable RunData::SectionAggregateTable() const
{
   TextTable::TableData tableData;
   for (const auto& section : mSectionData)
   {
      bool sectionFirst = true;
      for (const auto& interval : section.second.intervals)
      {
         bool intervalFirst = true;
         for (const auto& aggr : interval.second.aggregates)
         {
            bool aggregateFirst = true;
            for (const auto& measurement : aggr.measurements)
            {
               const auto foundType = mMeasurementTypes.find(measurement.type);
               tableData.push_back(
                  {sectionFirst ? section.second.label : "",
                   intervalFirst ? TimePointString(*interval.second.time.min) : "",
                   intervalFirst ? TimePointString(*interval.second.time.max) : "",
                   aggregateFirst ? TimePointString(*aggr.time.min) : "",
                   aggregateFirst ? TimePointString(*aggr.time.max) : "",
                   aggregateFirst ? aggr.aggregateType : "",
                   foundType != mMeasurementTypes.end() ? foundType->second.name : std::to_string(measurement.type),
                   std::to_string(measurement.value)});
               sectionFirst = intervalFirst = aggregateFirst = false;
            }
         }
      }
   }
   return TextTable({TextTable::Column("Section", 40),
                     TextTable::Column("From Time", 12),
                     TextTable::Column("To Time", 12),
                     TextTable::Column("Aggr From T", 12),
                     TextTable::Column("Aggr To T", 12),
                     TextTable::Column("Aggr. Type", 12),
                     TextTable::Column("Measr. Type", 14),
                     TextTable::Column("Measr. Value", 14)},
                    tableData);
}

TextTable RunData::SectionPointsTable() const
{
   TextTable::TableData sectionTableData;
   for (const auto& section : mSectionData)
   {
      bool sectionFirst = true;
      for (const auto& points : section.second.points)
      {
         bool pointFirst = true;
         for (const auto& measurement : points.measurements)
         {
            const auto foundType = mMeasurementTypes.find(measurement.type);
            sectionTableData.push_back(
               {sectionFirst ? section.second.label : "",
                pointFirst ? TimePointString(points.timeStamp) : "",
                pointFirst ? std::to_string(points.interval) : "",
                foundType != mMeasurementTypes.end() ? foundType->second.name : std::to_string(measurement.type),
                std::to_string(measurement.value)});
            sectionFirst = pointFirst = false;
         }
      }
   }
   return TextTable({TextTable::Column("Section", 40),
                     TextTable::Column("Time", 14),
                     TextTable::Column("IntervalID", 14),
                     TextTable::Column("Measr. Type", 14),
                     TextTable::Column("Measr. Value", 14)},
                    sectionTableData);
}

std::string RunData::TimePointString(int64_t time) const
{
   return std::string("T+") + DurationString(time - mTimeZero, mTimeScale);
}

} // namespace artificer
