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

#include "ProfilingTimeUtil.hpp"

#include <ctime>
#include <sstream>

namespace profiling
{
std::string TimeString(const std::chrono::system_clock::time_point now)
{
   // We will need to convert to time_t until C++20.
   // which introduces formatting for clock times.
   const auto clockNow = std::chrono::system_clock::to_time_t(now);

   // GCC < 5 unfortunately does not implement the C++11 feature std::put_time:
   //
   // https://stackoverflow.com/questions/14136833/stdput-time-implementation-status-in-gcc
   //
   // POSIX recommends not using the ctime() function,
   // because it is not thread safe.
   constexpr std::size_t stringBufSize = 128;
   char                  stringBuf[stringBufSize];
   std::ostringstream    os;
   const std::size_t     countWritten = std::strftime(stringBuf, sizeof(stringBuf), "%F %T", std::localtime(&clockNow));
   if (countWritten != 0)
   {
      return {stringBuf};
   }
   else
   {
      return {};
   }
}

} // namespace profiling
