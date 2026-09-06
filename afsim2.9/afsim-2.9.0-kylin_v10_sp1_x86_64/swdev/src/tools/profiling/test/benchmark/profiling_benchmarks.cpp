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

#include <chrono>
#include <iostream>
#include <list>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <tuple>

#include "ProfilingRegion.hpp"
#include "ProfilingSystem.hpp"
#include "UtBuildInfo.hpp"

#ifdef _WIN32
#include <windows.h>

#include <psapi.h>
#else
#include <sys/resource.h>
#endif

namespace
{
const size_t cTABLEFIELDSIZE = 16uL;

std::string PadStringTo(std::string str, size_t padTo, char padChar = ' ')
{
   const std::size_t padLen = str.size() > padTo ? str.size() - padTo : std::size_t(0);
   return str + std::string(padLen, padChar);
}

std::string BuildHeaderDivider(size_t fieldCount)
{
   const std::string fieldDivider = PadStringTo("", cTABLEFIELDSIZE, '-');
   std::string       headerDivider;
   for (size_t x = 0; x < fieldCount; ++x)
   {
      headerDivider += "+" + fieldDivider;
   }
   headerDivider += "+";
   return headerDivider;
}

std::string BuildHeader(const std::vector<std::string>& fields)
{
   std::stringstream header;
   const std::string headerDivider = BuildHeaderDivider(fields.size());
   header << headerDivider << std::endl;
   for (const auto& name : fields)
   {
      header << "|" << PadStringTo(name, cTABLEFIELDSIZE);
   }
   header << "|" << std::endl << headerDivider << std::endl;
   return header.str();
}

// The memory usage tracking going on here isn't precise enough for what we're trying to do.
// When pages are no longer used they are still reflected in the process's total working set.
// This means if we release a bunch of memory the usage doesn't reflect that and we can't
// get an accurate delta of how much our actual usage went down.
int64_t GetMemoryUsage()
{
#ifdef _WIN32
   PROCESS_MEMORY_COUNTERS memInfo;
   memInfo.cb = sizeof(PROCESS_MEMORY_COUNTERS);
   if (GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, memInfo.cb) == 0)
   {
      throw std::runtime_error(std::string("Unable to retrieve memory status: Windows error code ") +
                               std::to_string(GetLastError()));
   }
   return static_cast<int64_t>(memInfo.WorkingSetSize);
#else
   rusage usage;
   if (getrusage(RUSAGE_SELF, &usage) != 0)
   {
      throw std::runtime_error(std::string("Unable to retrieve memory status: errno = ") + std::to_string(errno));
   }
   return static_cast<std::size_t>(usage.ru_maxrss);
#endif
}

struct MemoryUsageDelta
{
   // Baseline established on construction.
   MemoryUsageDelta()
      : mMemoryUsageBaseline(GetMemoryUsage())
   {
   }

   // Checked on demand, baseline subtracted out.
   int64_t GetDelta() const
   {
      return GetMemoryUsage() - mMemoryUsageBaseline; // in bytes
   }

   int64_t mMemoryUsageBaseline = 0;
};

std::string BenchmarkCoexistingRegions()
{
   std::vector<std::pair<std::size_t, double>> results;
   std::vector<std::string>                    regionLabels;
   while (regionLabels.size() <= 100000uL)
   {
      regionLabels.push_back("Region #" + std::to_string(regionLabels.size() + 1));
   }

   for (std::size_t numLoopIters = 1; numLoopIters < 1000000; numLoopIters *= 10)
   {
      std::list<profiling::ProfilingRegion> regions;
      MemoryUsageDelta                      memoryTest;
      for (const auto& label : regionLabels)
      {
         regions.emplace_back(label.c_str());
      }
      results.push_back(
         std::make_pair(numLoopIters, static_cast<double>(memoryTest.GetDelta()) / static_cast<double>(regions.size())));
   }

   const std::vector<std::string> fields = {"Region Count", "AvgBytes/Region"};
   std::stringstream              resultsLog;
   resultsLog << BuildHeader(fields);
   for (const auto& result : results)
   {
      resultsLog << "|" << PadStringTo(std::to_string(result.first), cTABLEFIELDSIZE) << "|";
      resultsLog << PadStringTo(std::to_string(result.second), cTABLEFIELDSIZE) << "|";
      resultsLog << std::endl;
   }
   resultsLog << BuildHeaderDivider(fields.size()) << std::endl;
   return resultsLog.str();
}

void BenchmarkLibrary(bool loadLibrary, ut::optional<std::string> libraryPath)
{
   // clang-format off
   std::cout << "Benchmarking("
             <<
#if defined(_DEBUG) || (defined(__GNU__) && !defined(__OPTIMIZE__))
      "Debug"
#else
      "Release"
#endif
             << " build) library " << (libraryPath.has_value() ? libraryPath.value() : "no library") << "..."
             << std::endl;
   // clang-format on
   std::unique_ptr<profiling::ProfilingSystem> profSystem;

   if (loadLibrary)
   {
      profiling::ProfilingSystemArguments args;
      args.mProfilingOutputDest  = "benchmarking-output.txt";
      args.mProfilingLibraryPath = libraryPath;
      profSystem.reset(new profiling::ProfilingSystem(args));
   }

   const std::chrono::nanoseconds expectedTimePerBenchmark = std::chrono::milliseconds(10);
   std::cout << "Co-existing regions" << std::endl;
   std::cout << BenchmarkCoexistingRegions() << std::endl;
   std::cout << "End of benchmark" << std::endl;
}

/// @brief Generate @c aNumRandomNumbers pseudorandom numbers
///    using the given random seed @c aRandomSeed,
///    and return the total time in seconds that this took.
///
/// We use this function to determine a time unit for durations,
/// based on a number of random numbers large enough to exceed
/// the maximum measured clock resolution by some safety factor.
double TimeGeneratingManyRandomNumbers(const std::size_t aNumRandomNumbers, const std::random_device::result_type aRandomSeed)
{
   std::mt19937                                 engine{aRandomSeed};
   std::uniform_int_distribution<std::uint64_t> distribution;
   std::vector<std::uint64_t>                   randomNumbers(aNumRandomNumbers);

   const auto startTime = std::chrono::steady_clock::now();
   for (auto& randomNumber : randomNumbers)
   {
      randomNumber = distribution(engine);
   }
   const auto stopTime = std::chrono::steady_clock::now();

   const auto totalTime =
      std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(stopTime - startTime);
   return totalTime.count();
}

void GenerateManyRandomNumbersWithProfilingSections(const std::size_t                     aNumRandomNumbers,
                                                    const std::random_device::result_type aRandomSeed,
                                                    const std::uint32_t                   aSectionId)
{
   std::mt19937                                 engine{aRandomSeed};
   std::uniform_int_distribution<std::uint64_t> distribution;
   std::vector<std::uint64_t>                   randomNumbers(aNumRandomNumbers);
   {
      profiling::BeginProfilingSection(aSectionId);
      for (auto& randomNumber : randomNumbers)
      {
         randomNumber = distribution(engine);
      }
      profiling::EndProfilingSection(aSectionId);
   }
}

void GenerateManyRandomNumbersWithProfilingRegions(const std::size_t                     aNumRandomNumbers,
                                                   const std::random_device::result_type aRandomSeed)
{
   std::mt19937                                 engine{aRandomSeed};
   std::uniform_int_distribution<std::uint64_t> distribution;
   std::vector<std::uint64_t>                   randomNumbers(aNumRandomNumbers);
   {
      profiling::ProfilingRegion region("Pseudorandom numbers");
      for (auto& randomNumber : randomNumbers)
      {
         randomNumber = distribution(engine);
      }
   }
}

void GenerateManyRandomNumbersWithoutProfiling(const std::size_t                     aNumRandomNumbers,
                                               const std::random_device::result_type aRandomSeed)
{
   std::mt19937                                 engine{aRandomSeed};
   std::uniform_int_distribution<std::uint64_t> distribution;
   std::vector<std::uint64_t>                   randomNumbers(aNumRandomNumbers);
   {
      for (auto& randomNumber : randomNumbers)
      {
         randomNumber = distribution(engine);
      }
   }
}

void TestProfilingOverhead(const std::size_t                     aNumTrialsPerLoop,
                           const std::size_t                     aNumLoops,
                           const std::random_device::result_type aRandomSeed)
{
   {
      const auto startTime = std::chrono::steady_clock::now();
      for (std::size_t loop = 0; loop < aNumLoops; ++loop)
      {
         GenerateManyRandomNumbersWithoutProfiling(aNumTrialsPerLoop, aRandomSeed);
      }
      const auto stopTime = std::chrono::steady_clock::now();

      const auto totalTime =
         std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(stopTime - startTime);
      const auto meanTime = totalTime / static_cast<double>(aNumLoops);
      std::cout << "Generating pseudorandom numbers without profiling, warmup pass" << std::endl
                << "  Total time: " << totalTime.count() << std::endl
                << "  Number of (outer) loops: " << aNumLoops << std::endl
                << "  Mean time per (outer) loop iteration: " << meanTime.count() << std::endl;
   }
   {
      const auto startTime = std::chrono::steady_clock::now();
      for (std::size_t loop = 0; loop < aNumLoops; ++loop)
      {
         GenerateManyRandomNumbersWithoutProfiling(aNumTrialsPerLoop, aRandomSeed);
      }
      const auto stopTime = std::chrono::steady_clock::now();

      const auto totalTime =
         std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(stopTime - startTime);
      const auto meanTime = totalTime / static_cast<double>(aNumLoops);
      std::cout << "Generating pseudorandom numbers without profiling, second pass" << std::endl
                << "  Total time: " << totalTime.count() << std::endl
                << "  Number of (outer) loops: " << aNumLoops << std::endl
                << "  Mean time per (outer) loop iteration: " << meanTime.count() << std::endl;
   }
   {
      const auto sectionId = profiling::CreateProfilingSection("Pseudorandom number section");

      const auto startTime = std::chrono::steady_clock::now();
      for (std::size_t loop = 0; loop < aNumLoops; ++loop)
      {
         GenerateManyRandomNumbersWithProfilingSections(aNumTrialsPerLoop, aRandomSeed, sectionId);
      }
      const auto stopTime = std::chrono::steady_clock::now();

      profiling::DestroyProfilingSection(sectionId);

      const auto totalTime =
         std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(stopTime - startTime);
      const auto meanTime = totalTime / static_cast<double>(aNumLoops);
      std::cout << "Generating pseudorandom numbers with profiling sections" << std::endl
                << "  Total time: " << totalTime.count() << std::endl
                << "  Number of (outer) loops: " << aNumLoops << std::endl
                << "  Mean time per (outer) loop iteration: " << meanTime.count() << std::endl;
   }
   {
      const auto startTime = std::chrono::steady_clock::now();
      for (std::size_t loop = 0; loop < aNumLoops; ++loop)
      {
         GenerateManyRandomNumbersWithProfilingRegions(aNumTrialsPerLoop, aRandomSeed);
      }
      const auto stopTime = std::chrono::steady_clock::now();

      const auto totalTime =
         std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(stopTime - startTime);
      const auto meanTime = totalTime / static_cast<double>(aNumLoops);
      std::cout << "Generating pseudorandom numbers with profiling regions" << std::endl
                << "  Total time: " << totalTime.count() << std::endl
                << "  Number of (outer) loops: " << aNumLoops << std::endl
                << "  Mean time per (outer) loop iteration: " << meanTime.count() << std::endl;
   }
}

/// @brief Calls std::chrono::steady_clock::now() in a loop until the value changes,
///    repeating that @c aNumTrials times, and returns the maximum change
///    (each as a duration in seconds).
double MeasureMaxClockResolution(const std::size_t aNumTrials)
{
   auto measureResolutionOnce = []()
   {
      std::chrono::steady_clock::time_point::duration resolution{};
      const auto                                      innerStartTime = std::chrono::steady_clock::now();
      auto                                            innerStopTime  = std::chrono::steady_clock::now();
      while (innerStopTime == innerStartTime)
      {
         innerStopTime = std::chrono::steady_clock::now();
      }
      return std::chrono::steady_clock::time_point::duration(innerStopTime - innerStartTime);
   };

   std::chrono::steady_clock::time_point::duration curResolution = measureResolutionOnce();
   auto                                            maxResolution = curResolution;
   for (std::size_t trial = std::size_t(1); trial < aNumTrials; ++trial)
   {
      curResolution = measureResolutionOnce();
      if (curResolution > maxResolution)
      {
         maxResolution = curResolution;
      }
   }

   const double maxResolutionInSeconds =
      std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(maxResolution).count();

   std::cout << "Measuring clock resolution" << std::endl
             << "  Number of trials: " << aNumTrials << std::endl
             << "  Maximum resolution (s): " << maxResolutionInSeconds << std::endl;
   return maxResolutionInSeconds;
}

/// @brief Maximum clock resolution in seconds that we will use for the experiment.
///
/// Using this function ensures that if the system anomalously reports
/// a very large clock resolution, then the benchmark won't run for too long.
///
/// @return If the measured maximum clock resolution (see above) is too large,
///    then a predetermined constant, else the measured maximum clock resolution.
double MaximumClockResolution(const std::size_t numClockTrials)
{
   // Just in case the clock takes a really long time,
   // we cap the max clock resolution by maxPermittedClockRes.
   // 1 millisecond is coarse by modern standards, so we're unlikely to hit this.
   // (Remember that we're measuring wall clock time.)
   constexpr double maxPermittedClockResolution = 1.0e-3;
   const double     maxMeasuredClockResolution  = MeasureMaxClockResolution(numClockTrials);
   const double     maxRes = maxMeasuredClockResolution <= maxPermittedClockResolution ? maxMeasuredClockResolution :
                                                                                         maxPermittedClockResolution;
   std::cout << "  Cap on maximum clock resolution (s): " << maxPermittedClockResolution << std::endl
             << "  Maximum resolution that we will use (s): " << maxRes << std::endl;
   return maxRes;
}

/// @brief The number of loop iterations to use as a standard duration "ruler."
///
/// Each loop iteration generates a pseudorandom number.
///
/// @param aRandomSeed Seed for the pseudorandom number generator.
/// @param maxClockResolution Maximum clock resolution in seconds.
/// @param factorLongerThanMaxClockResolution Factor by which to exceed
///    the maximum measured clock resolution.
///
/// @return The number of loop iterations
///    that exceeds the maximum measured clock resolution
///    by at least a factor of @c factorLongerThanMaxClockResolution.
std::size_t IterationsForStandardTimeUnit(const std::random_device::result_type aRandomSeed,
                                          const double                          maxClockResolution,
                                          const double                          factorLongerThanMaxClockResolution)
{
   // These tests help establish a loop longer than the maximum measured clock resolution
   // by enough that we have confidence in using it as a duration unit.
   constexpr std::size_t minNumRandomNumbers            = 128uL;
   constexpr std::size_t maxNumRandomNumbers            = 16777216uL; // 2^24
   constexpr std::size_t factorIncrease                 = 2uL;
   const double          minAcceptableLoopTimeInSeconds = factorLongerThanMaxClockResolution * maxClockResolution;

   std::cout << "Determining number of loop iterations for standard time unit" << std::endl
             << "  Max clock resolution (s): " << maxClockResolution << std::endl
             << "  Factor longer than max clock resolution: " << factorLongerThanMaxClockResolution << std::endl
             << "  Loop count must make loop take at least this long (s): " << minAcceptableLoopTimeInSeconds
             << std::endl;

   double loopTimeInSeconds = 0.0;
   for (std::size_t numRandomNumbers = minNumRandomNumbers; numRandomNumbers <= maxNumRandomNumbers;
        numRandomNumbers *= factorIncrease)
   {
      loopTimeInSeconds = TimeGeneratingManyRandomNumbers(numRandomNumbers, aRandomSeed);
      if (loopTimeInSeconds >= minAcceptableLoopTimeInSeconds)
      {
         std::cout << "  Loop count success: stopping at count = " << numRandomNumbers << std::endl;
         return numRandomNumbers;
      }
   }
   std::cout << "  Loop count failure: stopping at count = " << maxNumRandomNumbers << std::endl;
   return maxNumRandomNumbers;
}

/// @brief Run benchmarks to measure the overhead of profiling regions and sections.
///
/// @param aEnableProfiling Whether to turn on the profiling system.
///
/// @param aNondefaultLibraryPath If it has a value, then the path to the dynamic library
///    to load as the nondefault profiling library.  Otherwise, load AFPerf,
///    the default profiling library (if @c aEnableProfiling is @c true).
///
/// @param aProfilingOutputPath If @c aEnableProfiling is @c true,
///    then the profiling output path.
void RunBenchmarks(const bool                       aEnableProfiling,
                   const ut::optional<std::string>& aNondefaultLibraryPath,
                   const ut::optional<std::string>& aProfilingOutputPath)
{
   std::unique_ptr<profiling::ProfilingSystem> profSystem;
   if (aEnableProfiling)
   {
      profiling::ProfilingSystemArguments args;
      args.mProfilingOutputDest  = aProfilingOutputPath;
      args.mProfilingLibraryPath = aNondefaultLibraryPath;
      profSystem.reset(new profiling::ProfilingSystem(args));
   }

   // This code takes a data-driven approach to computing a standard time unit.
   // We first compute the min and max clock resolution.
   // The max (worst case) is what we care about.
   // We then figure out how many random numbers we need to compute in a loop
   // so that the loop takes about factorLongerThanMaxClockRes
   // times longer than the max clock resolution.
   constexpr std::size_t numClockTrials(100000);
   constexpr double      factorLongerThanMaxClockResolution = 10.0;
   const double          maxClockResolution                 = MaximumClockResolution(numClockTrials);

   // We use random_device in hopes that the random seed
   // will be completely opaque to compiler optimization.
   // If so, then the loops to be timed cannot possibly
   // be constexpr'd away; they must execute at run time.
   // We don't actually include the random number generation in timing,
   // because random_device is not (necessarily) deterministic.
   // (It might read from /dev/random, for example, meaning that it might do "file" input,
   // with associated unpredictable latency.)
   std::random_device randomDevice;
   const auto         randomSeed = randomDevice();

   // These tests help establish a loop enough longer
   // than the maximum measured clock resolution
   // that we have confidence in using it as a duration "ruler."
   const std::size_t iterationsForStandardTimeUnit =
      IterationsForStandardTimeUnit(randomSeed, maxClockResolution, factorLongerThanMaxClockResolution);
   constexpr std::size_t numRepetitions(1000);
   TestProfilingOverhead(iterationsForStandardTimeUnit, numRepetitions, randomSeed);
   BenchmarkCoexistingRegions();
}

} // namespace

int main(int /* argc */, char** /* argv */)
{
   // The logical next step would be to read these as command-line arguments.
   bool                      enableProfiling = true;
   ut::optional<std::string> libraryPath;
   ut::optional<std::string> profilingOutputPath("benchmarking-output.afperf");

   RunBenchmarks(enableProfiling, libraryPath, profilingOutputPath);
   return 0;
}
