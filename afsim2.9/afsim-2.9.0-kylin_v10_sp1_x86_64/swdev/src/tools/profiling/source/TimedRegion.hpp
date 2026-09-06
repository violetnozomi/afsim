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

#ifndef PROFILINGTIMEDREGION_HPP
#define PROFILINGTIMEDREGION_HPP

#include "profiling_export.h"

#include <functional>
#include <memory>

#include "ProfilingRegion.hpp"
#include "UtLog.hpp"
#include "UtLogStream.hpp"
#include "UtProcessorTime.hpp"
#include "UtWallClock.hpp"

namespace profiling
{
/// Scope guard that records wall clock time elapsed and marks region beginning and end
/// by writing to a given output stream.
/// It owns one internal @a ProfilingRegion which uses the same region label as the given label.
/// It is subject to all caveats and restrictions as @a ProfilingRegion.
///
/// Timing regions may be nested in Mode::REGION. @a TimedRegion instances should never be handled
/// by pointer or passed as arguments to functions.
///
/// Example usage (Mode::REGION):
///@code
/// void InnerFunction() {
///    TimedRegion r("Recording InnerFunction");
///    ComputeOtherStuff();
/// }
/// void OuterFunction() {
///    TimedRegion r("Recording OuterFunction");
///    ComputeStuff();
///    InnerFunction();
/// }
/// @endcode
///
/// This results in the following logged output (with fake time values):
/// Recording OuterFunction.
/// Recording InnerFunction.
/// Recording InnerFunction complete.
///    Elapsed Wall Clock Time: 0.238954
///    Elapsed Processor Time : 0.15625
/// Recording OuterFunction complete.
///    Elapsed Wall Clock Time: 3.92168
///    Elapsed Processor Time : 4.07812
///
///
/// Example usage (Mode::SUBREGION):
///@code
/// void Function() {
///    TimedRegion r("A, B, and C", Mode::SUBREGION);
///
///    r.StartSubregion("starting A and B");
///    DoA();
///    DoB();
///    r.LogCurrentSubregion("finished A and B");
///
///    r.StartSubregion("starting C");
///    DoC();
///    r.LogCurrentSubregion("finished C");
/// }
/// @endcode
///
/// This results in the following logged output (with fake time values):
/// starting A and B.
/// finished A and B.
///    Elapsed Wall Clock Time: 2.248954
///    Elapsed Processor Time : 2.25625
/// starting C.
/// finished C.
///    Elapsed Wall Clock Time: 0.92168
///    Elapsed Processor Time : 1.07812
class PROFILING_EXPORT TimedRegion final
{
public:
   /// In Mode::REGION, TimedRegion must be used as a scope guard. Logging information is automatically produced
   /// on construction and destruction of the TimedRegion object. In Mode::SUBREGION, TimedRegion must be used as
   /// a stopwatch, using @ StartSubregion and @ StopSubregion to produce logging output.
   /// Mode::SUBREGION is required if logging must occur within nested scopes, such as conditional statements,
   /// or if runtime generated information must be included as part of the logged output.
   enum class Mode
   {
      REGION,   //!< Use TimedRegion like scope guard. Regions may be nested.
      SUBREGION //!< Use TimedRegion like stopwatch. Subregions must be disjoint.
   };

   /// @param aRegionLabel Label used when writing to @a outputStream and logs to the loaded profiling library.
   /// @param aMode Flag which sets the behavior of TimedRegion to REGION/SUBREGION logging for its lifespan.
   /// @param aExtraStartingOutputFunc Callable object used for custom output on region initialization.
   /// @param aOutputStream Stream to which to write messages. Use this when a different logging level is needed, such
   /// as ut::log::debug().
   TimedRegion(const char*                                  aRegionLabel,
               Mode                                         aMode                    = Mode::REGION,
               std::function<void(ut::log::MessageStream&)> aExtraStartingOutputFunc = nullptr,
               ut::log::MessageStream                       aOutputStream            = ut::log::info());

   ~TimedRegion();

   /// @brief Logs a start message for a subregion and sets the time offset members with the
   ///    current cpu and wall time. If profiling is enabled, will begin a ProfilingRegion. (Mode::SUBREGION only)
   ///
   /// NOTE: Subregions must be disjointed. Calling this method while a subregion is active will
   /// close the active ProfilingRegion and emit a warning message.
   ///
   /// @param aStartMessage Added to the start output message. If left empty, mRegionLabel
   ///    will be used in its place.
   void StartSubregion(const std::string& aStartMessage = "");

   /// @brief Logs an end message containing cpu and wall time values for a subregion. If
   /// profiling is enabled, will end an active ProfilingRegion. (Mode::SUBREGION only)
   ///
   /// NOTE: It is legal to call this method without an active subregion. The elapsed cpu and wall time
   /// since the last time their offsets were set will be used.
   ///
   /// @param aEndMessage Added to the end output message. If left empty, mRegionLabel + " complete."
   ///    will be used in its place.
   void StopSubregion(const std::string& aEndMessage = "");

   /// @brief Returns the elapsed wall time since the last time its offsets was set.
   double GetWallTime();

   /// @brief Returns the elapsed cpu time since the last time its offsets was set.
   double GetCpuTime();

   TimedRegion()                   = delete;
   TimedRegion(const TimedRegion&) = delete;
   TimedRegion& operator=(const TimedRegion&) = delete;
   TimedRegion(TimedRegion&&)                 = delete;
   TimedRegion& operator=(TimedRegion&&) = delete;

private:
   void LogStart(const std::string& aStartMessage = "");
   void LogEnd(const std::string& aEndMessage = "");

   //! If profiling enabled, captures profiling information for the lifespan of TimedRegion, regardless of Mode.
   std::unique_ptr<ProfilingRegion> mProfilingRegion;
   //! Identifier passed to mProfilingRegion and used for output information.
   std::string mRegionLabel;
   //! Flag (set at construction) to determine if TimedRegion is in REGION or SUBREGION mode
   Mode mMode;
   //! Callback that inserts additional information in the formatted output.
   std::function<void(ut::log::MessageStream&)> mExtraStartingOutputFunc;
   //! Parameterized stream - caller chooses the logging level/destination.
   ut::log::MessageStream mOutputStream;

   UtWallClock     mWallClock;
   UtProcessorTime mProcessorTime;

   //! Cpu time offset which is set when calling @ StartSubregion
   double mCpuOffset = 0.0;
   //! Wall time offset which is set when calling @ StartSubregion
   double mWallOffset = 0.0;
};

} // namespace profiling

#endif // PROFILINGTIMEDREGION_HPP
