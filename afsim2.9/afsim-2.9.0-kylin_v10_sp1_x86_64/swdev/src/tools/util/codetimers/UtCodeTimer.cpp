// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtCodeTimer.hpp"

#include <iomanip>

#include "UtLog.hpp"

UtCodeTimer*         UtCodeTimer::msInstancePtr = nullptr;
UtCodeTimerObserver* UtCodeTimer::msObserverPtr = nullptr;
std::thread::id      UtCodeTimer::msThreadId;

void UtCodeTimer::Initialize(UtCodeTimerObserver* aObserverPtr)
{
   if (!msInstancePtr)
   {
      msThreadId    = std::this_thread::get_id();
      msInstancePtr = new UtCodeTimer();
      if (aObserverPtr)
      {
         msInstancePtr->SetObserver(aObserverPtr);
      }
      else
      {
         msObserverPtr = new UtCodeTimerObserver();
         msInstancePtr->SetObserver(msObserverPtr);
      }
   }
}

void UtCodeTimer::Shutdown()
{
   if (msInstancePtr)
   {
      delete msInstancePtr;
      msInstancePtr = nullptr;
   }
   if (msObserverPtr)
   {
      delete msObserverPtr;
      msObserverPtr = nullptr;
   }
}

UtCodeTimer::UtCodeTimer()
   : mReschedule(false)
   , mObserverPtr(nullptr)
{
   ut::log::warning() << "UtCodeTimers are activated. THIS CODE IS NOT FOR RELEASE!";

   ResetClock();
   mLastTime = std::chrono::high_resolution_clock::now();
   StartPrivate("base");
}

void UtCodeTimer::ResetPrivate()
{
   mLastTime = std::chrono::high_resolution_clock::now();
}

void UtCodeTimer::UpdatePrivate()
{
   if (mActiveNodeStack.size() == 1)
   {
      UtCodeTimerNode* node = mActiveNodeStack.top();
      if (mObserverPtr)
      {
         mObserverPtr->Update(GetClock(), node);
      }

      mLastTime = std::chrono::high_resolution_clock::now();

      // reset the node stack
      mActiveNodeStack.pop(); // clear the node structure
      delete node;
      StartPrivate("base");
   }
   else
   {
      // if we still have active events, we need to wait for them to resolve before reporting
      mReschedule = true;
   }
}

void UtCodeTimer::StartPrivate(const std::string& aTag)
{
   // it would be cool to keep track of times on every thread.
   if (IsMyThread())
   {
      double           startTime = GetClock();
      UtCodeTimerNode* node      = nullptr;
      if (!mActiveNodeStack.empty())
      {
         node = mActiveNodeStack.top()->GetChild(aTag);
      }
      if (!node)
      {
         node = new UtCodeTimerNode(startTime);
         if (!mActiveNodeStack.empty())
         {
            mActiveNodeStack.top()->AddChild(node, aTag);
         }
      }
      else
      {
         node->SetStartTimeAndIncrementInstances(startTime);
      }
      mActiveNodeStack.push(node);
   }
   else
   {
      // we could probably find a way to track multiple threads, but I am not trying to do this, yet so I won't put in the work
      bool inserted = mOffThreadReports.emplace(aTag).second;
      if (inserted)
      {
         auto out = ut::log::developer() << "Attempted to start a timer off thread.";
         out.AddNote() << "Thread: " << aTag;
      }
   }
}

void UtCodeTimer::StopPrivate()
{
   if (IsMyThread())
   {
      UtCodeTimerNode* node = mActiveNodeStack.top();
      mActiveNodeStack.pop();
      node->UpdateTime(GetClock());
      if (mReschedule && (mActiveNodeStack.size() == 1))
      {
         mReschedule = false;
         UpdatePrivate();
      }
   }
}

UtCodeTimer::~UtCodeTimer()
{
   UtCodeTimerNode* baseTimer = nullptr;
   while (!mActiveNodeStack.empty())
   {
      baseTimer = mActiveNodeStack.top();
      mActiveNodeStack.pop();
   }
   if (baseTimer != nullptr)
   {
      delete baseTimer;
   }
   msInstancePtr = nullptr;
}

void UtCodeTimer::Reset()
{
   if (msInstancePtr)
   {
      msInstancePtr->ResetPrivate();
   }
}

// calling this will provide an update on what has happened with the timers since initialization, or the last call to update
void UtCodeTimer::Update()
{
   if (msInstancePtr)
   {
      msInstancePtr->UpdatePrivate();
   }
}

double UtCodeTimer::GetClock() const
{
   std::chrono::high_resolution_clock::time_point t = std::chrono::high_resolution_clock::now();
   return std::chrono::duration_cast<std::chrono::duration<double>>(t - mLastTime).count();
}

void UtCodeTimer::ResetClock()
{
   mLastTime = std::chrono::high_resolution_clock::now();
}

void UtCodeTimer::Start(const std::string& aTag)
{
   if (msInstancePtr)
   {
      msInstancePtr->StartPrivate(aTag);
   }
}

void UtCodeTimer::Stop()
{
   if (msInstancePtr)
   {
      msInstancePtr->StopPrivate();
   }
}

bool UtCodeTimer::IsMyThread() const
{
   return (msThreadId == std::this_thread::get_id());
}

UtCodeTimerNode::~UtCodeTimerNode()
{
   std::map<std::string, UtCodeTimerNode*>::iterator it;
   for (it = mChildMap.begin(); it != mChildMap.end(); ++it)
   {
      delete it->second;
   }
   mChildMap.clear();
}

UtCodeTimerNode* UtCodeTimerNode::GetChild(const std::string& aTag) const
{
   std::map<std::string, UtCodeTimerNode*>::const_iterator it = mChildMap.find(aTag);
   if (it != mChildMap.end())
   {
      return it->second;
   }
   else
   {
      return nullptr;
   }
}

void UtCodeTimerNode::AddChild(UtCodeTimerNode* aChildPtr, const std::string& aTag)
{
   mChildMap[aTag] = aChildPtr;
}

void UtCodeTimerObserver::Update(double aTime, UtCodeTimerNode* aTopLevelNode)
{
   auto out = ut::log::developer() << "UtCodeTimer Report:";
   out.AddNote() << "Time: " << aTime << " seconds";
   UtCodeTimerNode* baseTimer = aTopLevelNode;
   PrintChildReports(baseTimer, aTime, out);
}

void UtCodeTimerObserver::PrintChildReports(UtCodeTimerNode* aNodePtr, double aTime, ut::log::MessageStream& aStream)
{
   for (auto&& it : aNodePtr->GetChildMap())
   {
      auto out = aStream.AddNote() << "Timer " << it.first << ": " << std::setw(12);
      out.AddNote() << "Relative" << 100 * it.second->GetTotalTime() / aTime << "%";
      out.AddNote() << "Absolute: " << it.second->GetTotalTime() << " seconds";
      out.AddNote() << "Instances: " << it.second->GetInstances();
      PrintChildReports(it.second, it.second->GetTotalTime(), out);
   }
}
