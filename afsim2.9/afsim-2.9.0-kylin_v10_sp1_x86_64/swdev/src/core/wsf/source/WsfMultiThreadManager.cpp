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

#include "WsfMultiThreadManager.hpp"

#include <algorithm>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtSleep.hpp"
#include "UtWallClock.hpp"
#include "WsfLOS_Manager.hpp"
#include "WsfMover.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationObserver.hpp"

// =================================================================================================
//! Constructor
WsfMultiThreadManager::WsfMultiThreadManager(unsigned int   aNumberOfThreads,
                                             double         aBreakUpdateTime,
                                             bool           aDebugMultiThread,
                                             WsfSimulation* aSimulationPtr)
   : mSimulationPtr(aSimulationPtr)
   , mPlatformQueue()
   , mSensorQueue()
   , mNumberOfThreads(aNumberOfThreads)
   , mThreads(nullptr)
   , mThreadedPlatforms()
   , mNonThreadedPlatforms()
   , mThreadedSensors()
   , mNonThreadedSensors()
   , mBreakUpdateTime(aBreakUpdateTime)
   , mBreakUpdate(false)
   , mDebug(aDebugMultiThread)
   , mMutex()
{
   // Avoiding compiler warning 4355 (VS2010); using 'this' in initializer list
   mThreads = new WsfThreadPool<SimulationUpdateThread, ThreadFactory>(ThreadFactory(this));
}

// =================================================================================================
// virtual
WsfMultiThreadManager::~WsfMultiThreadManager()
{
   delete mThreads;
}

// =================================================================================================
void WsfMultiThreadManager::Initialize()
{
   // Start the thread pool
   mThreads->Start(mNumberOfThreads);

   auto out = ut::log::info() << "Multi-Thread Manager: Multi-threading activated.";
   out.AddNote() << "Worker Threads: " << mNumberOfThreads;
}

// =================================================================================================
void WsfMultiThreadManager::Complete(double aSimTime)
{
   // Wait for all threads to get done with the work
   mThreads->WaitUntilAllWorkDone();

   mThreadedPlatforms.clear();
   mNonThreadedPlatforms.clear();
   mThreadedSensors.clear();
   mNonThreadedSensors.clear();

   // Clear these too, just in case
   while (!mPlatformQueue.empty())
   {
      mPlatformQueue.pop();
   }
   while (!mSensorQueue.empty())
   {
      mSensorQueue.pop();
   }
}

// =================================================================================================
void WsfMultiThreadManager::UpdatePlatforms(double aCurrentFrameTime)
{
   mSimulationPtr->SetMultiThreadingActive(true);

   // For the multi-threaded case, platform updates are broken out.

   size_t i;

   // Update the mover and the fuel; process thread safe first
   for (i = 0; i < mThreadedPlatforms.size(); ++i)
   {
      mPlatformQueue.push(PlatformElement(mThreadedPlatforms[i], aCurrentFrameTime));
   }
   if (!mPlatformQueue.empty())
   {
      mThreads->AssignWork(mPlatformQueue.size());
      mThreads->WaitUntilAllWorkDone();
   }

   mSimulationPtr->SetMultiThreadingActive(false);

   // Notify all simulation observers and execute platform scripts
   WsfPlatform* platformPtr(nullptr);
   for (i = 0; i < mThreadedPlatforms.size(); ++i)
   {
      platformPtr = mSimulationPtr->GetPlatformByIndex(mThreadedPlatforms[i]);
      if (platformPtr != nullptr)
      {
         platformPtr->SendQueuedMessages(aCurrentFrameTime);
         platformPtr->NotifyUpdate(aCurrentFrameTime);
         platformPtr->ExecuteScript(aCurrentFrameTime);
      }
   }

   // Update non-thread safe
   for (i = 0; i < mNonThreadedPlatforms.size(); ++i)
   {
      platformPtr = mSimulationPtr->GetPlatformByIndex(mNonThreadedPlatforms[i]);
      if (platformPtr != nullptr)
      {
         platformPtr->SetUpdateLocked(false);
         platformPtr->Update(aCurrentFrameTime);
         platformPtr->SetUpdateLocked(true);
      }
   }

   WsfObserver::FramePlatformsUpdated(mSimulationPtr)(aCurrentFrameTime);

   // Notify the LOS Manager
   if (mSimulationPtr->GetLOS_Manager())
   {
      mSimulationPtr->GetLOS_Manager()->Update(aCurrentFrameTime);
   }
}

// =================================================================================================
void WsfMultiThreadManager::UpdateSensors(double aCurrentFrameTime)
{
   mSimulationPtr->SetMultiThreadingActive(true);

   // For the multi-threaded case, sensor updates are broken out.

   unsigned int i;

   // Update the sensor; process thread safe first

   for (i = 0; i < mThreadedSensors.size(); ++i)
   {
      if ((mThreadedSensors[i]->GetNextUpdateTime() > (aCurrentFrameTime + 1.0E-5)))
      {
         continue;
      }

      mSensorQueue.push(SensorElement(mThreadedSensors[i], aCurrentFrameTime, mThreadedSensors[i]->GetNextUpdateTime()));
   }

   mBreakUpdate = false;
   if (!mSensorQueue.empty()) // wake thread pool up only if theres work to be done
   {
      mThreads->AssignWork(mSensorQueue.size());

      // Wait for all threads to get done with the work
      if (mSimulationPtr->IsRealTime())
      {
         bool allDone = mThreads->TryWaitUntilAllWorkDone(mBreakUpdateTime);
         if (!allDone)
         {
            mBreakUpdate = true;
            ut::log::warning() << "Multi-Thread Manager: Skipping out of sensor updates.";
            // Despite signalling early termination with BreakUpdate, we must
            // still wait for the termination to finish to avoid violating
            // multithreading invariants of the still running parallel sensor updates.
            mThreads->WaitUntilAllWorkDone();
         }
      }
      else
      {
         mThreads->WaitUntilAllWorkDone();
      }
   }

   mSimulationPtr->SetMultiThreadingActive(false);

   // Send out the queued message on the sensors
   for (i = 0; i < mThreadedSensors.size(); ++i)
   {
      mThreadedSensors[i]->SendQueuedMessages(aCurrentFrameTime);
   }

   // Update non-thread safe
   if (!mBreakUpdate)
   {
      for (i = 0; i < mNonThreadedSensors.size(); ++i)
      {
         mNonThreadedSensors[i]->Update(aCurrentFrameTime);
      }
   }

   while (!mSensorQueue.empty())
   {
      mSensorQueue.pop();
   }
}

// =================================================================================================
bool WsfMultiThreadManager::PopNextPlatformElement(PlatformElement& aPlatformElement)
{
   bool                                  elementReturned = false;
   std::lock_guard<std::recursive_mutex> lock(mMutex);
   if (!mPlatformQueue.empty())
   {
      aPlatformElement = mPlatformQueue.front();
      mPlatformQueue.pop();
      elementReturned = true;
   }
   return elementReturned;
}

// =================================================================================================
bool WsfMultiThreadManager::PopNextSensorElement(SensorElement& aSensorElement)
{
   bool                                  elementReturned = false;
   std::lock_guard<std::recursive_mutex> lock(mMutex);
   if (!mSensorQueue.empty())
   {
      aSensorElement = mSensorQueue.top();
      mSensorQueue.pop();
      elementReturned = true;
   }
   return elementReturned;
}

// =================================================================================================
void WsfMultiThreadManager::AddPlatform(double aSimTime, WsfPlatform* aPlatformPtr)
{
   // Add the platform the local list.
   size_t platformIndex = aPlatformPtr->GetIndex();

   std::vector<size_t>::iterator threadedIter = find(mThreadedPlatforms.begin(), mThreadedPlatforms.end(), platformIndex);
   std::vector<size_t>::iterator nonThreadedIter =
      find(mNonThreadedPlatforms.begin(), mNonThreadedPlatforms.end(), platformIndex);

   if ((threadedIter == mThreadedPlatforms.end()) && (nonThreadedIter == mNonThreadedPlatforms.end()))
   {
      if ((aPlatformPtr->GetMover() != nullptr) && aPlatformPtr->GetMover()->ThreadSafe())
      {
         mThreadedPlatforms.push_back(platformIndex);
      }
      else
      {
         mNonThreadedPlatforms.push_back(platformIndex);
      }
      aPlatformPtr->SetUpdateLocked(true);
   }
}

// =================================================================================================
void WsfMultiThreadManager::DeletePlatform(double aSimTime, WsfPlatform* aOldPlatformPtr)
{
   // Remove the platform and associated subsystems from any list we are maintaining.

   size_t platformIndex = aOldPlatformPtr->GetIndex();

   std::vector<size_t>::iterator pli = find(mThreadedPlatforms.begin(), mThreadedPlatforms.end(), platformIndex);
   if (pli != mThreadedPlatforms.end())
   {
      mThreadedPlatforms.erase(pli);
   }
   else
   {
      pli = find(mNonThreadedPlatforms.begin(), mNonThreadedPlatforms.end(), platformIndex);
      if (pli != mNonThreadedPlatforms.end())
      {
         mNonThreadedPlatforms.erase(pli);
      }
   }

   // Make sure other lists are updated. We do this even if the platform wasn't being tracked
   // by us... just to make sure...

   for (WsfComponentList::RoleIterator<WsfSensor> snriter(*aOldPlatformPtr); !snriter.AtEnd(); ++snriter)
   {
      WsfSensor*                        systemPtr = *snriter;
      std::vector<WsfSensor*>::iterator iter      = find(mThreadedSensors.begin(), mThreadedSensors.end(), systemPtr);
      if (iter != mThreadedSensors.end())
      {
         mThreadedSensors.erase(iter);
         continue; // prevent from going on to next check
      }

      iter = find(mNonThreadedSensors.begin(), mNonThreadedSensors.end(), systemPtr);
      if (iter != mNonThreadedSensors.end())
      {
         mNonThreadedSensors.erase(iter);
      }
   }
}

// =================================================================================================
void WsfMultiThreadManager::TurnSensorOff(double aSimTime, WsfSensor* aSensorPtr)
{
   std::vector<WsfSensor*>::iterator iter = find(mThreadedSensors.begin(), mThreadedSensors.end(), aSensorPtr);
   if (iter != mThreadedSensors.end())
   {
      mThreadedSensors.erase(iter);
      return; // prevent from going on to next check
   }

   iter = find(mNonThreadedSensors.begin(), mNonThreadedSensors.end(), aSensorPtr);
   if (iter != mNonThreadedSensors.end())
   {
      mNonThreadedSensors.erase(iter);
   }
}

// =================================================================================================
void WsfMultiThreadManager::TurnSensorOn(double aSimTime, WsfSensor* aSensorPtr)
{
   if (aSensorPtr->IsSlave() || aSensorPtr->IsExternallyControlled())
   {
      return;
   }

   // Add the sensor the local list.
   std::vector<WsfSensor*>::iterator threadedIter = find(mThreadedSensors.begin(), mThreadedSensors.end(), aSensorPtr);
   std::vector<WsfSensor*>::iterator nonThreadedIter =
      find(mNonThreadedSensors.begin(), mNonThreadedSensors.end(), aSensorPtr);
   if ((threadedIter == mThreadedSensors.end()) && (nonThreadedIter == mNonThreadedSensors.end()))
   {
      if (aSensorPtr->ThreadSafe())
      {
         mThreadedSensors.push_back(aSensorPtr);
      }
      else
      {
         mNonThreadedSensors.push_back(aSensorPtr);
      }
   }
}

// =================================================================================================
WsfMultiThreadManager::SimulationUpdateThread::SimulationUpdateThread(WsfMultiThreadManager* aManagerPtr)
   : WsfThread()
   , mManagerPtr(aManagerPtr)
{
}

// =================================================================================================
//! Performs the thread work for updating a platform.
//!
//! @return Function state of the thread. (See WsfThread)
WsfMultiThreadManager::SimulationUpdateThread::FunctionType WsfMultiThreadManager::SimulationUpdateThread::DoWork()
{
   if (mManagerPtr->PlatformsQueued())
   {
      PlatformElement platformElement;
      mManagerPtr->PopNextPlatformElement(platformElement);
      if (platformElement.mPlatformIndex != 0)
      {
         WsfPlatform* platformPtr = mManagerPtr->mSimulationPtr->GetPlatformByIndex(platformElement.mPlatformIndex);
         if (platformPtr != nullptr)
         {
            platformPtr->UpdateMultiThread(platformElement.mSimTime);
         }
         else
         {
            auto out = ut::log::warning() << "Multi-Thread Manager: Platform could not be found.";
            out.AddNote() << "Platform Index: " << platformElement.mPlatformIndex;
         }
      }
   }
   else if (mManagerPtr->SensorsQueued())
   {
      SensorElement sensorElement;
      mManagerPtr->PopNextSensorElement(sensorElement);
      if (sensorElement.mSensorPtr != nullptr)
      {
         sensorElement.mSensorPtr->Update(sensorElement.mSimTime);
      }
   }

   // If there is currently no work in the queue then make the thread available
   if ((GetFunction() == ASSIGNED) && (NoWork() || (mManagerPtr->BreakUpdate())))
   {
      return AVAILABLE;
   }

   return GetFunction();
}

// =================================================================================================
bool WsfMultiThreadManager::SimulationUpdateThread::NoWork()
{
   return (!mManagerPtr->SensorsQueued() && (!mManagerPtr->PlatformsQueued()));
}

// =================================================================================================
WsfMultiThreadManager::SimulationUpdateThread* WsfMultiThreadManager::ThreadFactory::operator()() const
{
   return new SimulationUpdateThread(mManagerPtr);
}
