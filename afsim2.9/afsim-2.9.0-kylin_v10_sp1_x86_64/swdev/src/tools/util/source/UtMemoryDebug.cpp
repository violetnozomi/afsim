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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "UtMemoryDebug.hpp"

#include <iostream>
#include <mutex>

#include "UtCast.hpp"
#include "UtLog.hpp"

namespace
{
static std::vector<int> sTraceCounts;
struct TraceDetail
{
   TraceDetail()
      : mNextId(0)
   {
   }
   size_t           mNextId;
   std::set<size_t> mAliveIds;
};
std::recursive_mutex            sTraceMutex;
static std::vector<TraceDetail> sTraceDetails;
} // namespace

bool UtMemoryDebug::ShowLeakReport()
{
   bool hasLeaks = false;

   for (size_t i = 0; i < sTraceCounts.size(); ++i)
   {
      if (sTraceCounts[i] > 0)
      {
         auto out = ut::log::error() << "UtMemoryDebug Leak Detected:";
         out.AddNote() << "Kind: " << i;
         out.AddNote() << "Remaining: " << sTraceCounts[i];
         hasLeaks = true;
      }
   }
   for (size_t i = 0; i < sTraceDetails.size(); ++i)
   {
      TraceDetail& detail = sTraceDetails[i];
      if (!detail.mAliveIds.empty())
      {
         auto logger = ut::log::error() << "UtMemoryDebug Leak Details:";
         logger.AddNote() << "Kind: " << i;
         auto note = logger.AddNote() << "Remaining Instances:";
         for (size_t id : detail.mAliveIds)
         {
            note.AddNote() << id;
         }
         hasLeaks = true;
      }
   }
   return hasLeaks;
}

void UtMemoryDebug::ShowDeleteError(int aKind)
{
   auto out = ut::log::error() << "UtMemoryDebug detected invalid delete for object type.";
   out.AddNote() << "Object Type: " << aKind;
}

void UtMemoryDebug::AddTrace(int aKind)
{
   if (sTraceCounts.size() <= (size_t)aKind)
   {
      sTraceMutex.lock();
      sTraceCounts.resize((size_t)aKind + 1);
      sTraceMutex.unlock();
   }
   ++sTraceCounts[aKind];
}

void UtMemoryDebug::RemoveTrace(int aKind)
{
   --sTraceCounts[aKind];
}

size_t UtMemoryDebug::AddDetailTrace(int aKind)
{
   if (aKind >= ut::cast_to_int(sTraceDetails.size()))
   {
      sTraceMutex.lock();
      sTraceDetails.resize(ut::cast_to_size_t(aKind + 1));
      sTraceMutex.unlock();
   }

   TraceDetail& detail = sTraceDetails[aKind];
   auto         id     = (detail.mNextId++);
   detail.mAliveIds.insert(id);
   return id;
}

void UtMemoryDebug::RemoveDetailTrace(int aKind, int aId)
{
   if (sTraceDetails[aKind].mAliveIds.erase(aId) == 0)
   {
      ShowDeleteError(aKind);
   }
}
