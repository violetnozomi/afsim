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

#include "WsfProfilingApplicationExtension.hpp"

#include <cstring>
#include <iostream>

#include "ProfilingSystem.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfStandardApplication.hpp"

namespace
{
constexpr const char profilingLibraryPathArg[] = "-profiling-library";
constexpr const char profilingOutputArg[]      = "-profiling-output";
} // namespace

int WsfProfilingApplicationExtension::ProcessCommandLine(WsfStandardApplication::Options& /* aOptions */,
                                                         int   aArgc,
                                                         char* aArgv[])
{
   int numArgsProcessed = 0;

   // The convention is just to read from the front of aArgv,
   // rather than looking through all the entries.
   // WsfStandardApplication may call this method multiple times,
   // once per command-line option.

   const char* const currentArg = aArgv[numArgsProcessed];
   if (std::strcmp(currentArg, profilingLibraryPathArg) == 0)
   {
      ++numArgsProcessed;
      if (numArgsProcessed >= aArgc)
      {
         ut::log::warning() << "Command-line option " << profilingLibraryPathArg << " requires exactly one argument";
      }
      else
      {
         profiling::GlobalProfilingSystemArguments().mProfilingLibraryPath.emplace(aArgv[numArgsProcessed]);
         ++numArgsProcessed;
      }
   }
   else if (std::strcmp(currentArg, profilingOutputArg) == 0)
   {
      ++numArgsProcessed;
      if (numArgsProcessed >= aArgc)
      {
         ut::log::warning() << "Command-line option " << profilingOutputArg << " requires exactly one argument";
      }
      else
      {
         profiling::GlobalProfilingSystemArguments().mProfilingOutputDest.emplace(aArgv[numArgsProcessed]);
         ++numArgsProcessed;
      }
   }

   return numArgsProcessed;
}

void WsfProfilingApplicationExtension::PrintCommandLineArguments() const
{
   constexpr std::size_t maxArgLength              = sizeof(profilingLibraryPathArg) > sizeof(profilingOutputArg) ?
                                                        sizeof(profilingLibraryPathArg) :
                                                        sizeof(profilingOutputArg);
   constexpr std::size_t extraSpaceCountOutputArg  = maxArgLength - sizeof(profilingOutputArg);
   constexpr std::size_t extraSpaceCountLibraryArg = maxArgLength - sizeof(profilingLibraryPathArg);
   constexpr std::size_t maxNumArgSpaces           = maxArgLength - 1; // Exclude count of trailing null

   constexpr std::size_t numIndentSpaces(2);
   constexpr char        indentChar(' ');
   const std::string     argSpaces(maxNumArgSpaces, indentChar);
   const std::string     indent(numIndentSpaces, indentChar);
   const std::string     indentOutputArg(extraSpaceCountOutputArg + numIndentSpaces, indentChar);
   const std::string     indentLibraryArg(extraSpaceCountLibraryArg + numIndentSpaces, indentChar);

   // This uses std::cout explicitly, per current convention in WsfStandardApplication::ShowUsage.
   // clang-format off
   std::cout
      << profilingOutputArg << indentOutputArg << "Destination to which to send profiling output.\n"
      << argSpaces << indent << "For AFPerf (default profiler) this is a file path, which\n"
      << argSpaces << indent << "will be created if it does not exist. Other profiling\n"
      << argSpaces << indent << "libraries may treat the output destination differently.\n"
      << argSpaces << indent << "Profiling is only enabled if this option is provided.\n"
      << profilingLibraryPathArg << indentLibraryArg << "Path of alternate profiling library to load.\n"
      << argSpaces << indent << "If not provided, AFPerf will be used when profiling is enabled.\n";
   // clang-format on
}

/// @brief Register the "profiling" extension with the application
///    so it is available for use.
void WSF_EXPORT Register_profiling(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("profiling"))
   {
      aApplication.RegisterFeature("profiling", "profiling"); // Indicate the feature is present
      aApplication.RegisterExtension("profiling", ut::make_unique<WsfProfilingApplicationExtension>());
   }
}
