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

#ifndef UTCODETIMER_HPP
#define UTCODETIMER_HPP

#include "ut_export.h"

#include <chrono>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <thread>

#include "UtLog.hpp"

class UtCodeTimer;

class UtCodeTimerNode
{
public:
   UtCodeTimerNode(const double aTime)
      : mTime(aTime)
   {
   }
   virtual ~UtCodeTimerNode();
   UtCodeTimerNode* GetChild(const std::string& aTag) const;
   void             AddChild(UtCodeTimerNode* aChildPtr, const std::string& aTag);
   void             SetStartTimeAndIncrementInstances(double aTime)
   {
      mTime = aTime;
      ++mInstances;
   }
   void                                     UpdateTime(double aTime) { mTotalTime += aTime - mTime; }
   double                                   GetTotalTime() { return mTotalTime; }
   unsigned int                             GetInstances() { return mInstances; }
   std::map<std::string, UtCodeTimerNode*>& GetChildMap() { return mChildMap; }

private:
   double                                  mTime;
   double                                  mTotalTime = 0.0;
   unsigned int                            mInstances = 1;
   std::map<std::string, UtCodeTimerNode*> mChildMap;
};

class UT_EXPORT UtCodeTimerObserver
{
public:
   virtual ~UtCodeTimerObserver() = default;
   virtual void Update(double aTime, UtCodeTimerNode* aTopLevelNode);

private:
   void PrintChildReports(UtCodeTimerNode* aNodePtr, double aTime, ut::log::MessageStream& aStream);
};

class UT_EXPORT UtCodeTimer
{
public:
   // initialize and shutdown should wrap any calls to Start, Stop, or ScopeTimers.
   // if an observer is passed in, it is the responsibility of the caller
   static void Initialize(UtCodeTimerObserver* aObserverPtr = nullptr);
   static void Shutdown();

   // every call to start, should have a matching call to stop, otherwise problems will occur
   // considering using scope timers, where appropriate, as they automate this process
   static void Start(const std::string& aTag);
   static void Stop();

   static void Reset();

   // your application will need to make calls to this when it wants to get feedback
   // UtQtCodeTimer, for example ties this to scheduled real-time events on a QTimer
   static void Update();

private:
   bool   IsMyThread() const;
   double GetClock() const;
   void   ResetClock();
   UtCodeTimer();
   virtual ~UtCodeTimer();
   void ResetPrivate();
   void UpdatePrivate();
   void StartPrivate(const std::string& aTag);
   void StopPrivate();
   void SetObserver(UtCodeTimerObserver* aObserverPtr) { mObserverPtr = aObserverPtr; }

   using UtCodeTimerNodeStack = std::stack<UtCodeTimerNode*>;
   UtCodeTimerNodeStack                           mActiveNodeStack;
   bool                                           mReschedule;
   UtCodeTimerObserver*                           mObserverPtr;
   std::chrono::high_resolution_clock::time_point mLastTime;
   std::set<std::string>                          mOffThreadReports;

   static UtCodeTimer*         msInstancePtr;
   static UtCodeTimerObserver* msObserverPtr;

   static std::thread::id msThreadId;
};

class UtScopeTimer
{
public:
   UtScopeTimer(const std::string& aTag) { UtCodeTimer::Start(aTag); }
   ~UtScopeTimer() { UtCodeTimer::Stop(); }
};

#endif
