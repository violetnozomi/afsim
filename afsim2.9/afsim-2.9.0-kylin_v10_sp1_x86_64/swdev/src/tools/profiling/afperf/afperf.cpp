// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "afperf_export.h"

#include <array>
#include <chrono>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <stack>
#include <string>
#include <tuple>
#include <vector>

#include "ProfilingCommon.hpp"
#include "ProfilingCsvUtil.hpp"
#include "ProfilingTimeUtil.hpp"
#include "UtLog.hpp"
#include "UtOptional.hpp"

// Primary AFSIM profiling implementation library.
//
// This also serves as a reference for those writing alternate profiling implementation libraries.

// See ProfilingHooks.hpp for interface details.
// See ProfilingSystem.cpp for the current version of the profiling system.

namespace
{
// Character that separates fields in an output line.
constexpr const char cFIELD_DELIMITER[] = ",";

// Placeholder application name.
//
// This is just a placeholder for now, until we can figure out a way
// to get the actual application name (e.g., "mission")
// without making AFPerf (circularly) depend on the application itself.
constexpr const char cPLACEHOLDER_APPLICATION_NAME[] = "unknown";

// Placeholder application name.
//
// This is just a placeholder for now, until we can figure out a way
// to get the actual application name (e.g., "mission")
// without making AFPerf (circularly) depend on the application itself.
constexpr const char cPLACEHOLDER_APPLICATION_VERSION[] = "0.0.0";

// Placeholder region ID.
//
// The "region ID" field is reserved for later, per request.
constexpr const char cPLACEHOLDER_REGION_ID[] = "0";

// The output file (which may or may not be open).
std::ofstream gOutputFile;

// Output aString, appending a newline, to the preferred output location.
//
// The "preferred output location" is the output file, if it is open.
// Otherwise, this prints nothing nowhere (as if no profiling happened).
//
// C++17 will add std::to_chars, which should speed up event reporting
// by avoiding dynamic memory allocation.
// For now, if someone is looking to optimize this library,
// it would make sense to focus on avoiding string allocations.
// Note that output will never happen on multiple threads
// (as output is not thread safe, and we don't want to add synchronization).
// Thus, it's OK to reuse string storage for output.
// A reasonable optimization would be to generate each output line
// in a string buffer, that is reused and resized only if needed.
void OutputLineToPreferredLocation(const char* aString)
{
   if (gOutputFile)
   {
      gOutputFile << aString << std::endl;
   }
}

using MonotonicClock = std::chrono::steady_clock;
using TimePoint      = std::chrono::time_point<MonotonicClock>;
using Seconds        = std::chrono::duration<double, std::ratio<1>>;

// Converts the given timestamp into a string for the output format.
//
// The resulting string isn't intended for humans to read.
template<class ClockType>
std::string TimestampToString(const std::chrono::time_point<ClockType> aTimestamp)
{
   return std::to_string(aTimestamp.time_since_epoch().count());
}

// The type of an event in this library's output.
enum class EventType
{
   AfPerfVersion,
   MeasurementType,
   PauseResume,
   RegionAggregate,
   RegionPoint,
   RegionStart,
   RegionStop,
   RunAggregate,
   RunInfo,
   RunPoint,
   SectionAggregate,
   SectionInfo,
   SectionPoint,
   SectionStart,
   SectionStop,
};

// Converts the given event type into a string for the output format.
//
// The resulting string isn't intended for humans to read; it's just an enum value.
std::string EventTypeAsString(const EventType aCategoryAndType)
{
   // C++23 will add std::to_underlying; use that to replace the static_cast.
   return std::to_string(static_cast<unsigned int>(aCategoryAndType));
}

// The current run's ID.
//
// AfsimProfilingInitialize initializes this to a new, hopefully unique value.
// The comment above GenerateRunId discusses this.
std::uint64_t gCurrentRunId = 0;

// Generate a new "run ID" for the current run.
//
// This function is not thread safe and thus must not be called
// concurrently by multiple threads.
//
// While it's impossible to promise that the run ID will be unique across runs,
// a sorta-kinda-unique run ID can be useful for distinguishing consecutive runs
// in a single file.  std::random_device makes no promise of nondeterminism
// (and some implementations historically have been deterministic!),
// but it's a good place to start for seeding a random number generator.
//
// The run ID could perhaps be useful for combining results
// from different MPI processes for the same run.
// However, different MPI processes should never attempt
// to interleave their output.  This is because file output is not atomic,
// Windows forbids multiple concurrent writes to the same file,
// and not all clusters have a shared file system.
void GenerateRunId()
{
   // Use a hopefully nondeterministic seed to start the pseudorandom number generator.
   static std::mt19937 randomNumberGenerator{std::random_device{}()};

   std::uniform_int_distribution<uint64_t> distribution(0, std::numeric_limits<uint64_t>::max());
   gCurrentRunId = distribution(randomNumberGenerator);
}

// Get the current run's ID.
std::uint64_t CurrentRunId()
{
   return gCurrentRunId;
}

// Implementation of AfsimProfilingInterfaceVersion.
//
// This exists to avoid the need to forward-declare
// AfsimProfilingInterfaceVersion in RecordAFPerfVersionEvent.
std::uint64_t AfsimProfilingInterfaceVersionImpl()
{
   // Incrementing Epoch version indicates a resetting of versioning system.
   constexpr uint16_t epoch = 0;
   // incrementing MAJOR version indicates when incompatible API changes are made.
   constexpr uint16_t major = 1;
   // incrementing MINOR version indicates when functionality is added in a backwards compatible manner.
   constexpr uint16_t minor = 0;
   // incrementing PATCH version indicates when backwards compatible bug fixes are made.
   constexpr uint16_t patch = 0;

   return profiling::PackVersionInfo(epoch, major, minor, patch);
}

// Record the library's output format version.
// This might not necessarily be the same as the library's version number.
void RecordAFPerfVersionEvent()
{
   constexpr const char formatVersion[] = "# AFPerf v1     ";
   OutputLineToPreferredLocation(formatVersion);
}

// Represents an instance of a profiling region currently "in flight"
// (i.e., having been started, but not yet stopped).
struct Region
{
   std::string mName;
   TimePoint   mStartTime;
};

// Represents all the instances of profiling regions currently "in flight."
std::stack<Region, std::vector<Region>> gRegionStack;

// Record an event upon starting a Region instance.
void RecordRegionStart(const Region& aRegion)
{
   // RegionPoint is for a point event that occurs in a region,
   // not for region start or stop.

   constexpr const char placeholderTags[] = "";
   // clang-format off
   const std::string line =
      EventTypeAsString(EventType::RegionStart) +
      cFIELD_DELIMITER + TimestampToString(aRegion.mStartTime) +
      cFIELD_DELIMITER + std::to_string(CurrentRunId()) +
      cFIELD_DELIMITER + cPLACEHOLDER_REGION_ID +
      cFIELD_DELIMITER + profiling::QuoteCsvFieldIfNeeded(aRegion.mName) +
      cFIELD_DELIMITER + profiling::QuoteCsvFieldIfNeeded(placeholderTags);
   // clang-format on
   OutputLineToPreferredLocation(line.c_str());
}

// Record an event upon stopping a Region instance.
void RecordRegionStop(const Region& aRegion, const TimePoint aStopTime)
{
   // clang-format off
   const std::string line =
      EventTypeAsString(EventType::RegionStop) +
      cFIELD_DELIMITER + TimestampToString(aStopTime) +
      cFIELD_DELIMITER + cPLACEHOLDER_REGION_ID;
   // clang-format on
   OutputLineToPreferredLocation(line.c_str());
}

using MeasurementTypeId = std::size_t;

// Tracks measurement type labels and their corresponding IDs.
class MeasurementTypeTracker
{
public:
   MeasurementTypeId AddAndRecord(const TimePoint aTimestamp,
                                  const char      aName[],
                                  const char      aDatatype[],
                                  const char      aUnit[],
                                  const char      aSummary[],
                                  const char      aDescription[])
   {
      // NOTE: This is not thread safe, but code is not allowed to create
      // new measurement types concurrently on multiple threads.
      const MeasurementTypeId measurementTypeId = [&]()
      {
         auto iter = mMeasurementTypes.find(aName);
         if (iter != mMeasurementTypes.end())
         {
            return iter->second;
         }
         else
         {
            // This code is more explicit than desired, because it must support GCC 4.8,
            // which does not implement aggregate initialization correctly.
            mMeasurementTypes.insert(iter, std::make_pair(std::string(aName), mNextIdToAssign));
            return mNextIdToAssign++;
         }
      }();

      // clang-format off
      const std::string line =
         EventTypeAsString(EventType::MeasurementType) +
         cFIELD_DELIMITER + TimestampToString(aTimestamp) +
         cFIELD_DELIMITER + std::to_string(CurrentRunId()) +
         cFIELD_DELIMITER + std::to_string(measurementTypeId) +
         cFIELD_DELIMITER + profiling::QuoteCsvFieldIfNeeded(aName) +
         cFIELD_DELIMITER + profiling::QuoteCsvFieldIfNeeded(aDatatype) +
         cFIELD_DELIMITER + profiling::QuoteCsvFieldIfNeeded(aUnit) +
         cFIELD_DELIMITER + profiling::QuoteCsvFieldIfNeeded(aSummary) +
         cFIELD_DELIMITER + profiling::QuoteCsvFieldIfNeeded(aDescription);
      // clang-format on
      OutputLineToPreferredLocation(line.c_str());
      return measurementTypeId;
   }

private:
   MeasurementTypeId                        mNextIdToAssign = 0;
   std::map<std::string, MeasurementTypeId> mMeasurementTypes;
};

// The library's single instance of the MeasurementTypeTracker.
MeasurementTypeTracker gMeasurementTypeTracker;

// Add a new measurement type, and record the resulting event in the output.
MeasurementTypeId AddAndRecordMeasurementTypeEvent(const TimePoint aTimestamp,
                                                   const char      aName[],
                                                   const char      aDatatype[],
                                                   const char      aUnit[],
                                                   const char      aSummary[],
                                                   const char      aDescription[])
{
   return gMeasurementTypeTracker.AddAndRecord(aTimestamp, aName, aDatatype, aUnit, aSummary, aDescription);
}

ut::optional<std::string> PathFromFileUri(const std::string& aUri)
{
   std::string scheme = "file:///";
   if (aUri.substr(0, scheme.size()) == scheme)
   {
      return aUri.substr(scheme.size());
   }

   // refuse to accept a file://alpha/beta.bin type path since that implies alpha as the hostname which isn't supported.
   if (aUri.find("://") != std::string::npos)
   {
      // this is a uri that we don't support.  Reject.
      return ut::nullopt;
   }

   scheme = "file:";
   if (aUri.substr(0, scheme.size()) == scheme)
   {
      return aUri.substr(scheme.size());
   }
   // if we didn't find anything we assume 'file:'.
   return aUri;
}

// Initialize the profiling output destination.
//
// aOutputUri: Uniform Resource Indicator (URI)
//    representing where to write profiling output.
//
// First return value is an error code.
// It follows the POSIX convention of zero indicating success.
// If the error code is nonzero,
// then the second return value is a message explaining the error.
std::tuple<std::int32_t, std::string> InitializeOutput(const char* aOutputUri)
{
   enum
   {
      OK = 0,
      parseError,
      openError
   };

   const std::string         pathStr(aOutputUri);
   ut::optional<std::string> filePath = PathFromFileUri(aOutputUri);

   if (!filePath.has_value())
   {
      return std::make_tuple(parseError, std::string("Failed to parse output URI \"") + aOutputUri + "\"");
   }

   const auto appendToEnd = std::ios_base::out | std::ios_base::in | std::ios_base::app;

   // We would like to open a separate stream here and std::swap it to gOutputFile, however gcc 4.8 does not
   // support std::swap on std::ofstream.
   gOutputFile.open(*filePath, appendToEnd);
   if (gOutputFile.good())
   {
      ut::log::debug() << "AFPerf saving profiling information to file: " << *filePath;
      return std::make_tuple(OK, std::string(""));
   }
   return std::make_tuple(openError, std::string("Failed to open output file \"") + *filePath + "\"");
}

// Record the run start event.
//
// aInitializeTime: Time at which the profiling library was initialized.
void RecordRunInfoEvent(const TimePoint aInitializeTime)
{
   constexpr const char placeholderFormatVersion[] = "1.0.0";
   constexpr const char placeholderTag[]           = "";
   // Wall clock time in seconds since the system_clock's epoch.
   // C++20 promises that the system_clock's epoch is the Unix epoch.
   const auto secondsSinceUnixEpoch = std::chrono::duration_cast<std::chrono::duration<double>>(
      std::chrono::system_clock::now() - std::chrono::system_clock::time_point{});

   // clang-format off
   const std::string runStartLine =
      EventTypeAsString(EventType::RunInfo) +
      cFIELD_DELIMITER + TimestampToString(aInitializeTime) +
      cFIELD_DELIMITER + profiling::QuoteCsvFieldIfNeeded(profiling::DurationUnitString(MonotonicClock::period{})) +
      cFIELD_DELIMITER + std::to_string(secondsSinceUnixEpoch.count()) +
      cFIELD_DELIMITER + placeholderFormatVersion +
      cFIELD_DELIMITER + std::to_string(CurrentRunId()) +
      cFIELD_DELIMITER + profiling::QuoteCsvFieldIfNeeded(cPLACEHOLDER_APPLICATION_NAME) +
      cFIELD_DELIMITER + profiling::QuoteCsvFieldIfNeeded(cPLACEHOLDER_APPLICATION_VERSION) +
      cFIELD_DELIMITER + profiling::QuoteCsvFieldIfNeeded(placeholderTag);
   // clang-format on
   OutputLineToPreferredLocation(runStartLine.c_str());
}

// By default, the system starts with two aggregate measurement types:
//
// * total time (sum of MonotonicClock's TimePoint::count() results), and
// * number of outermost nonnested instances.
//
// MeasurementTypeTracker helps us map between each measurement type's label,
// and its unique integer ID.  It also lets us add more measurement types if we wish.
constexpr const char cMEASUREMENT_LABEL_TOTALTIME[] = "Total time";
constexpr const char cMEASUREMENT_LABEL_INSTANCES[] = "Number of outermost nonnested instances";

constexpr const char cMEASUREMENT_DATATYPE_INT32[] = "int32";
constexpr const char cMEASUREMENT_DATATYPE_INT64[] = "int64";

// These IDs will be set when adding their corresponding measurement types
// to the MeasurementTypeTracker.
MeasurementTypeId gTotalTimeMeasurementTypeId       = 0;
MeasurementTypeId gNumInstancesMeasurementTypeLabel = 1;

} // namespace

extern "C" AFPERF_EXPORT std::uint64_t AfsimProfilingInterfaceVersion()
{
   return AfsimProfilingInterfaceVersionImpl();
}

extern "C" AFPERF_EXPORT std::int32_t AfsimProfilingInitialize(const char* aOutputUri)
{
   const auto runStartTime = MonotonicClock::now();
   const auto result       = InitializeOutput(aOutputUri);
   if (std::get<0>(result) != 0)
   {
      ut::log::debug() << "Error while initializing profiling output: " << std::get<1>(result);
      return std::get<0>(result);
   }

   GenerateRunId();
   RecordAFPerfVersionEvent();
   RecordRunInfoEvent(runStartTime);
   {
      constexpr const char summary[]     = "";
      constexpr const char description[] = "";
      gTotalTimeMeasurementTypeId =
         AddAndRecordMeasurementTypeEvent(runStartTime,
                                          cMEASUREMENT_LABEL_TOTALTIME,
                                          cMEASUREMENT_DATATYPE_INT64,
                                          profiling::DurationUnitString(MonotonicClock::period{}).c_str(),
                                          "",
                                          "");
   }
   {
      constexpr const char unit[]        = "count";
      constexpr const char summary[]     = "";
      constexpr const char description[] = "";
      gNumInstancesMeasurementTypeLabel  = AddAndRecordMeasurementTypeEvent(runStartTime,
                                                                           cMEASUREMENT_LABEL_INSTANCES,
                                                                           cMEASUREMENT_DATATYPE_INT32,
                                                                           unit,
                                                                           summary,
                                                                           description);
   }
   return std::get<0>(result);
}

extern "C" AFPERF_EXPORT void AfsimProfilingBeginRegion(const char* aRegionName)
{
   const auto startTime = MonotonicClock::now();

   Region region{aRegionName, startTime};
   RecordRegionStart(region);
   gRegionStack.push(std::move(region));
}

extern "C" AFPERF_EXPORT void AfsimProfilingEndRegion()
{
   const auto stopTime = MonotonicClock::now();

   if (gRegionStack.empty())
   {
      ut::log::error() << "Profiling: Attempt to stop a region "
                          "when no region is currently active.  "
                          "This might happen if you use ProfilingRegion at main() scope.  "
                          "Doing that is not supported.";
      return;
   }
   const Region region = gRegionStack.top();
   // We need to pop in order to keep the region stack correct,
   // regardless of whether the code that follows throws.
   gRegionStack.pop();
   RecordRegionStop(region, stopTime);
}

namespace
{
// Representation of a section's collected performance data.
//
// The data are aggregated over all instances of the section.
//
// SectionData is carefully constructed to fit in a typical 64-byte cache line.
// This helps avoid false sharing across threads.
// If you find yourself wanting to expand this struct,
// please be sure to change its alignment to an integer multiple of 64 bytes.
struct alignas(64) SectionData
{
   using Clock     = std::chrono::high_resolution_clock;
   using TimePoint = std::chrono::time_point<Clock>;
   using Duration  = TimePoint::duration;

   // The number of "open" instances of the section.
   uint64_t mOpenCount{};
   // Among all open instances, the first instance's start time.
   TimePoint mFirstStart{};
   // The time of the last stop (such that there were no more open instances).
   TimePoint mLastStop{};
   // Total duration and total count do not count multiple instances
   // of the section open at the same time.
   // The instances are "coalesced" into one.
   Duration      mTotalTime{};
   std::uint64_t mTotalCount{};
};

/// Map from (unique) section ID to that section's data.
///
/// Holds all the created sections; sections are removed on destruction.
std::map<std::uint32_t, SectionData> gSectionIdToSectionData;

/// Holds a pointer to the data for every section
///    that lives in @a gSectionIdToSectionData.
///
/// Once a section is destroyed, the pointer becomes invalid.
std::vector<SectionData*> gAllSectionsEverCreated;

/// Map from (unique) section ID to (nonunique) section label.
std::map<std::uint32_t, std::string> gSectionIdToSectionLabel;

/// Reserve a section ID and make space for it in the vector of pointers.
std::uint32_t ReserveSectionId()
{
   const std::uint32_t newId = static_cast<std::uint32_t>(gAllSectionsEverCreated.size());
   gAllSectionsEverCreated.push_back(nullptr);
   return newId;
}

// Report an event that describes a created section.
void ReportSectionInfoEvent(const std::uint32_t aSectionId, const char aSectionLabel[])
{
   constexpr const char placeholderTags[] = "";

   const auto timestamp = MonotonicClock::now();

   // clang-format off
   const std::string line =
      EventTypeAsString(EventType::SectionInfo) +
      cFIELD_DELIMITER + TimestampToString(timestamp) +
      cFIELD_DELIMITER + std::to_string(CurrentRunId()) +
      cFIELD_DELIMITER + std::to_string(aSectionId) +
      cFIELD_DELIMITER + profiling::QuoteCsvFieldIfNeeded(aSectionLabel) +
      cFIELD_DELIMITER + profiling::QuoteCsvFieldIfNeeded(placeholderTags);
   // clang-format on
   OutputLineToPreferredLocation(line.c_str());
}

// Report an event that describes a section's aggregated collected performance data.
void ReportSectionAggregateEvent(const std::uint32_t aSectionId, const SectionData& aSectionData)
{
   // The section interval ID string is blank, since we're always aggregating these.
   constexpr const char placeholderSectionIntervalIdStr[] = "";
   constexpr const char placeholderAggregationTypeStr[]   = "total";
   constexpr const char placeholderTags[]                 = "";

   const auto stopTime  = aSectionData.mLastStop;
   const auto startTime = aSectionData.mFirstStart;

   // clang-format off
   const std::string line =
      EventTypeAsString(EventType::SectionAggregate) +
      cFIELD_DELIMITER + TimestampToString(stopTime) +
      cFIELD_DELIMITER + TimestampToString(startTime) +
      cFIELD_DELIMITER + std::to_string(aSectionId) +
      cFIELD_DELIMITER + placeholderSectionIntervalIdStr +
      cFIELD_DELIMITER + placeholderAggregationTypeStr +
      cFIELD_DELIMITER + std::to_string(gTotalTimeMeasurementTypeId) +
      cFIELD_DELIMITER + std::to_string(aSectionData.mTotalTime.count()) +
      cFIELD_DELIMITER + std::to_string(gNumInstancesMeasurementTypeLabel) +
      cFIELD_DELIMITER + std::to_string(aSectionData.mTotalCount);
   // clang-format on
   OutputLineToPreferredLocation(line.c_str());
}

} // namespace

extern "C" AFPERF_EXPORT std::uint32_t AfsimProfilingCreateSection(const char label[])
{
   const std::uint32_t newId = ReserveSectionId();
   // We work around a few different GCC 4.8 issues here
   // that relate to aggregate (curly-brace) initialization.
   // Idiomatic C++11 code would be more concise here.
   gSectionIdToSectionLabel.insert(std::make_pair(newId, std::string(label)));
   SectionData  tmpSection;
   auto         result            = gSectionIdToSectionData.insert(std::make_pair(newId, tmpSection));
   SectionData& section           = result.first->second;
   gAllSectionsEverCreated[newId] = &section;

   ReportSectionInfoEvent(newId, label);
   return newId;
}

extern "C" AFPERF_EXPORT void AfsimProfilingDestroySection(const std::uint32_t sectionId)
{
   auto labelIter = gSectionIdToSectionLabel.find(sectionId);
   auto dataIter  = gSectionIdToSectionData.find(sectionId);
   // We really don't want any bug in profiling to crash the system.
   if (labelIter != gSectionIdToSectionLabel.end() && dataIter != gSectionIdToSectionData.end())
   {
      ReportSectionAggregateEvent(sectionId, dataIter->second);

      // Be sure to erase only _after_ reporting the event.
      gAllSectionsEverCreated[sectionId] = nullptr;
      (void)gSectionIdToSectionLabel.erase(labelIter);
      (void)gSectionIdToSectionData.erase(dataIter);
   }
}

extern "C" AFPERF_EXPORT void AfsimProfilingBeginSection(std::uint32_t sectionId)
{
   SectionData& section = *gAllSectionsEverCreated[sectionId];

   // We use mOpenCount, not mTotalCount,
   // to decide when to start counting time for a section.
   // This ensures correct counting of nested instances of the same section.
   // Consider the following example:
   //
   // 1. Start section 42 at time 100.
   // 2. Start section 42 at time 101.
   // 3. Stop section 42 at time 102.
   // 4. Stop section 42 at time 103.
   // 5. Start section 42 at time 200.
   // 6. Stop section 42 at time 201.
   //
   // Step (2) is allowed because we permit nested instances of the same section.
   // Step (4) stops the outermost nested instance.
   //
   // By using mOpenCount == 0 to decide when to start counting,
   // we correctly capture two intervals: [100, 103] and [200, 201],
   // for a correct total time of 4 time units.
   // If we were to use mTotalCount == 0 to decide when to start counting,
   // we would incorrectly get a single time interval [100, 201]
   // and an incorrect total time of 101 time units.

   // NOTE (Hoemmen 2021/12/10, 2022/01/24) We currently ask users
   // to take responsibility for thread synchronization with respect to sections,
   // for example if starting a section on one thread and stopping it on another.
   // If we ever decided to relax that requirement,
   // we would need a (relaxed) atomic fetch_add here.
   // See also the other related comment in the function that stops the section.
   if (section.mOpenCount == uint64_t(0))
   {
      section.mFirstStart = SectionData::Clock::now();
   }
   section.mOpenCount++;
}

extern "C" AFPERF_EXPORT void AfsimProfilingEndSection(std::uint32_t sectionId)
{
   const auto   stopTime = SectionData::Clock::now();
   SectionData& section  = *gAllSectionsEverCreated[sectionId];

   // NOTE (Hoemmen 2021/12/10, 2022/01/24) We currently ask users
   // to take responsibility for thread synchronization with respect to sections,
   // for example if starting a section on one thread and stopping it on another.
   // If we ever decided to relax that requirement,
   // we would need a (acq_rel) atomic fetch_sub here.
   // See also the other related comment in the function that starts the section.
   if (section.mOpenCount == uint64_t(1))
   {
      const SectionData::Duration lastDuration = stopTime - section.mFirstStart;
      section.mLastStop                        = stopTime;
      section.mTotalTime += lastDuration;
      section.mTotalCount++;
   }
   section.mOpenCount--;
}

extern "C" AFPERF_EXPORT void AfsimProfilingFinalize()
{
   ut::log::debug() << "Finalizing AFPerf...";

   if (gOutputFile)
   {
      gOutputFile << std::flush;
      gOutputFile.close();
   }
   {
      decltype(gAllSectionsEverCreated) empty;
      std::swap(empty, gAllSectionsEverCreated);
   }
   ut::log::debug() << "AFPerf profiling finalized.";
}
