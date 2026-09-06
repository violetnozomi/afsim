// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "RvMergerThread.hpp"

#include <fstream>
#include <iostream>
#include <limits>

#include "GenUniqueId.hpp"
#include "RvEventPipeClasses.hpp"
#include "RvMergerUtils.hpp"
#include "UtCast.hpp"
#include "UtException.hpp"
#include "UtMemory.hpp"
#include "UtPack.hpp"
#include "UtPackSchema.hpp"

//! Gets the next chronological event from a list of RecordingFile's.
//! If there is no next event to be had, then returns nullptr and aIndex < 0.
//! \param aRecordings INOUT The list of recordings. Only the RecordingFile with the next event is modified.
//! \return The next event.
static std::pair<std::unique_ptr<rv::MsgBase>, int>
GetNextMessage(std::vector<std::unique_ptr<RvMerger::RecordingFileReader>>& aRecordings)
{
   double leastTime = std::numeric_limits<double>::max();
   int    index     = -1;

   for (int i = 0; i < static_cast<int>(aRecordings.size()); i++)
   {
      const rv::MsgBase* next = aRecordings[i]->PeekNext();
      if (next != nullptr && next->simTime() + aRecordings[i]->GetTimeOffset() < leastTime)
      {
         leastTime = next->simTime();
         index     = i;
      }
   }

   if (index >= 0)
   {
      return {aRecordings[index]->TakeNext(), index};
   }

   return {nullptr, -1};
}

RvMerger::MergerThread::MergerThread(QObject* aParent)
   : QThread(aParent)
{
}

RvMerger::MergerThread::~MergerThread() noexcept
{
   Reset();
}

void RvMerger::MergerThread::AddInputFile(std::string aFileName) noexcept
{
   mInputFiles.emplace_back(std::move(aFileName));
}

void RvMerger::MergerThread::SetOutputFile(std::string aFileName) noexcept
{
   mOutputFile = std::move(aFileName);
}

void RvMerger::MergerThread::Reset() noexcept
{
   // Wait for finish
   requestInterruption();
   wait();

   mInputFiles.clear();
   mOutputFile.clear();
}

void RvMerger::MergerThread::run() noexcept
try
{
   ut::SetupThreadErrorHandling();

   if (mInputFiles.empty())
   {
      throw std::runtime_error("No recording files provided.");
   }

   if (mOutputFile.empty())
   {
      throw std::runtime_error("No output file provided.");
   }

   UtPackSerializer serializer;
   InitializeSerializer(mInputFiles[0], serializer);

   // Initialize file input
   auto recordings = InitializeInputs(serializer);

   // Initialize file output
   RecordingFileWriter output(mOutputFile, recordings[0]->GetSchema(), serializer);

   PlatformDatabase platforms(recordings.size());

   // Register all platforms
   for (size_t i = 0; i < recordings.size(); i++)
   {
      while (recordings[i]->PeekNext() && !isInterruptionRequested())
      {
         auto message = recordings[i]->TakeNext();
         if (message->GetMessageId() == rv::MsgPlatformInfo::cMESSAGE_ID)
         {
            AddPlatform(platforms, dynamic_cast<rv::MsgPlatformInfo&>(*message), i);
         }
      }

      recordings[i]->Restart();
   }

   // Process all events
   auto nextEvent = GetNextMessage(recordings);
   while (nextEvent.first && !isInterruptionRequested())
   {
      if (nextEvent.first->simIndex() == 0) // if local message
      {
         AdjustPlatformIndices(platforms, *nextEvent.first, nextEvent.second);

         // Handle time synchronization
         AdjustSimTimes(*nextEvent.first, recordings[nextEvent.second]->GetTimeOffset() - mMinTimeOffset);

         // Set new simIndex
         nextEvent.first->simIndex(nextEvent.second);

         // Output message
         OutputMessage(output, nextEvent.second, *nextEvent.first);
      }

      nextEvent = GetNextMessage(recordings);
   }
}
catch (std::exception& e)
{
   emit RunError(e.what());
}

std::vector<std::unique_ptr<RvMerger::RecordingFileReader>> RvMerger::MergerThread::InitializeInputs(UtPackSerializer& aSerializer)
{
   std::vector<std::unique_ptr<RvMerger::RecordingFileReader>> retval;

   for (const auto& file : mInputFiles)
   {
      retval.emplace_back(ut::make_unique<RecordingFileReader>(file, aSerializer));
   }

   // Test for same schema
   for (const auto& recording : retval)
   {
      // Using recordings[0] as a reference.
      if (!retval[0]->CompareSchema(*recording))
      {
         throw std::runtime_error(
            "Recording schemas do not match. The AFSIM version used to create the recording files must be the same.");
      }
   }

   // Synchronize
   mMinTimeOffset = std::numeric_limits<double>::max();
   for (auto& recording : retval)
   {
      // Using recordings[0] as a reference
      mMinTimeOffset = std::min(mMinTimeOffset, recording->DetermineTimeOffset(retval[0]->GetUniqueId()));
      recording->Restart();
   }

   return retval;
}

void RvMerger::MergerThread::AddPlatform(PlatformDatabase& aPlatforms, const rv::MsgPlatformInfo& aMessage, size_t aSimIndex)
{
   if (aMessage.disIdValid())
   {
      aPlatforms.Add(aSimIndex, aMessage.platformIndex(), aMessage.disId());
   }
   else
   {
      aPlatforms.Add(aSimIndex, aMessage.platformIndex());
   }
}

void RvMerger::MergerThread::AdjustPlatformIndices(PlatformDatabase& aPlatforms, rv::MsgBase& aMessage, size_t aSimIndex)
{
   // Convert platform indices to new numbers.
   size_t i = 0;
   bool   good;

   auto index = aMessage.GetIndex(i, good);
   while (good)
   {
      if (index != 0)
      {
         auto temp = aPlatforms.Find(aSimIndex, index);
         if (temp == 0)
         {
#ifdef _DEBUG
            std::cerr << "Platform index not found:\n   File: " + mInputFiles[aSimIndex] +
                            "\n   Index: " + std::to_string(index) + "\n";
#else
            throw std::runtime_error("Platform index not found:\n   File: " + mInputFiles[aSimIndex] +
                                     "\n   Index: " + std::to_string(index));
#endif
         }
         aMessage.SetIndex(i, temp);
      }

      i++;
      index = aMessage.GetIndex(i, good);
   }
}

void RvMerger::MergerThread::AdjustSimTimes(rv::MsgBase& aMessage, double aOffset)
{
   size_t i = 0;
   bool   good;

   auto time = aMessage.GetTime(i, good);
   while (good)
   {
      aMessage.SetTime(i, time + aOffset);
      i++;
      time = aMessage.GetTime(i, good);
   }
}

void RvMerger::MergerThread::OutputMessage(RecordingFileWriter& aOutput, size_t aRecordingIndex, const rv::MsgBase& aMessage)
{
   // List all events that require special rules.
   switch (aMessage.GetMessageId())
   {
   case rv::MsgScenarioData::cMESSAGE_ID:
   case rv::MsgSetDate::cMESSAGE_ID:
      // only take from reference recording
      if (aRecordingIndex == 0)
      {
         aOutput.Write(aMessage);
      }
      break;
   case rv::MsgXioHeartbeat::cMESSAGE_ID:
      // ignore message
      break;
   default:
      aOutput.Write(aMessage);
   }
}
