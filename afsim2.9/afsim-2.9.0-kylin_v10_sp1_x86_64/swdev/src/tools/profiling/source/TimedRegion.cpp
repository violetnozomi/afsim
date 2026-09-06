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

#include "TimedRegion.hpp"

#include <exception>

#include "UtLog.hpp"
#include "UtLogStream.hpp"

namespace profiling
{
TimedRegion::TimedRegion(const char*                                  aRegionLabel,
                         Mode                                         aMode,
                         std::function<void(ut::log::MessageStream&)> aExtraStartingOutputFunc,
                         ut::log::MessageStream                       aOutputStream)
   : mProfilingRegion(nullptr)
   , mRegionLabel(aRegionLabel)
   , mMode(aMode)
   , mExtraStartingOutputFunc(aExtraStartingOutputFunc)
   , mOutputStream(std::move(aOutputStream))
{
   if (mMode == Mode::REGION)
   {
      mProfilingRegion.reset(new ProfilingRegion(aRegionLabel));
      LogStart();
   }
}

TimedRegion::~TimedRegion()
{
   try
   {
      if (mMode == Mode::REGION)
      {
         LogEnd();
      }
   }
   // Exceptions caught here cannot be rethrown.
   catch (const std::exception&)
   {
      /// Do nothing intentionally
   }
}

void TimedRegion::StartSubregion(const std::string& aStartMessage)
{
   if (mMode == Mode::SUBREGION)
   {
      if (mProfilingRegion)
      {
         ut::log::warning() << "Overlapping subregions detected in TimedRegion: " << mRegionLabel;
      }

      mCpuOffset  = mProcessorTime.GetTime();
      mWallOffset = mWallClock.GetClock();
      LogStart(aStartMessage);
      mProfilingRegion.reset(new ProfilingRegion(aStartMessage.c_str()));
   }
   else
   {
      auto err = ut::log::error() << "Attempting to use StartSubregion in TimedRegion: " << mRegionLabel;
      err.AddNote() << "TimedRegion must be in Mode::SUBREGION to use this method";
   }
}

void TimedRegion::StopSubregion(const std::string& aEndMessage)
{
   if (mMode == Mode::SUBREGION)
   {
      LogEnd(aEndMessage);
      mProfilingRegion.reset(nullptr);
   }
   else
   {
      auto err = ut::log::error() << "Attempting to use StopSubregion in TimedRegion: " << mRegionLabel;
      err.AddNote() << "TimedRegion must be in Mode::SUBREGION to use this method";
   }
}

double TimedRegion::GetWallTime()
{
   return mWallClock.GetClock() - mWallOffset;
}

double TimedRegion::GetCpuTime()
{
   return mProcessorTime.GetTime() - mCpuOffset;
}

void TimedRegion::LogStart(const std::string& aStartMessage)
{
   ut::log::newline();
   mOutputStream << (!aStartMessage.empty() ? aStartMessage : mRegionLabel) << '.';
   if (mExtraStartingOutputFunc)
   {
      mExtraStartingOutputFunc(mOutputStream);
   }
   mOutputStream.Send();
}

void TimedRegion::LogEnd(const std::string& aEndMessage)
{
   std::string regionEndMessage;

   // If destructor called in context of an error, change the completion message.
   if (std::current_exception())
   {
      regionEndMessage = "Completion of region failed: " + mRegionLabel;
   }
   else
   {
      regionEndMessage = !aEndMessage.empty() ? aEndMessage : mRegionLabel + " complete.";
   }

   ut::log::newline();
   mOutputStream << regionEndMessage;
   mOutputStream.AddNote() << "Elapsed Wall Clock Time: " << GetWallTime();
   mOutputStream.AddNote() << "Elapsed Processor Time : " << GetCpuTime();
   mOutputStream.Send();
}
} // namespace profiling
