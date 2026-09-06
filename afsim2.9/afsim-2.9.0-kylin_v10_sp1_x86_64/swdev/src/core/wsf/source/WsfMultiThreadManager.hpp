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

#ifndef WSFMULTITHREADMANAGER_HPP
#define WSFMULTITHREADMANAGER_HPP

#include "wsf_export.h"

#include <mutex>
#include <queue>
#include <vector>

class UtInput;
#include "WsfMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensor.hpp"
#include "WsfThread.hpp"
#include "WsfThreadPool.hpp"

/**
   A specialization class to handle multi-thread capabilities in the WSF
   core framework via thread pools. Current implementation allows for threaded
   platform (i.e. mover) and sensor updates.
*/

class WSF_EXPORT WsfMultiThreadManager
{
public:
   WsfMultiThreadManager(unsigned int   aNumberOfThreads,
                         double         aBreakUpdateTime,
                         bool           aDebugMultiThread,
                         WsfSimulation* aSimulationPtr);
   virtual ~WsfMultiThreadManager();

   void Initialize();

   void Complete(double aSimTime);

   //! @name Main update methods.
   //@{

   void UpdatePlatforms(double aCurrentTime);
   void UpdateSensors(double aCurrentTime);

   bool BreakUpdate() { return mBreakUpdate; }
   //@}

   //! @name Platform and Sensor methods
   //@{

   void AddPlatform(double aSimTime, WsfPlatform* aPlatformPtr);

   inline void PlatformIntroduced(double aSimTime, WsfPlatform* aPlatformPtr) { AddPlatform(aSimTime, aPlatformPtr); }

   void DeletePlatform(double aSimTime, WsfPlatform* aOldPlatformPtr);

   inline void PlatformDeleted(double aSimTime, WsfPlatform* aOldPlatformPtr)
   {
      DeletePlatform(aSimTime, aOldPlatformPtr);
   }

   void TurnSensorOff(double aSimTime, WsfSensor* aSensorPtr);

   void TurnSensorOn(double aSimTime, WsfSensor* aSensorPtr);
   //@}

protected:
   class PlatformElement
   {
   public:
      PlatformElement()
         : mPlatformIndex(0)
         , mSimTime(0.0)
      {
      }

      PlatformElement(size_t aPlatformIndex, double aSimTime)
         : mPlatformIndex(aPlatformIndex)
         , mSimTime(aSimTime)
      {
      }


      size_t mPlatformIndex; //! Platform to update
      double mSimTime;       //! Simulation time
   };

   class SensorElement
   {
   public:
      SensorElement()
         : mSensorPtr(nullptr)
         , mSimTime(0.0)
         , mNextUpdateTime(0.0)
      {
      }

      SensorElement(WsfSensor* aSensorPtr, double aSimTime, double aNextUpdateTime)
         : mSensorPtr(aSensorPtr)
         , mSimTime(aSimTime)
         , mNextUpdateTime(aNextUpdateTime)
      {
      }

      bool operator<(const SensorElement& aRhs) const { return (mNextUpdateTime > aRhs.mNextUpdateTime); }

      WsfSensor* mSensorPtr;
      double     mSimTime;
      double     mNextUpdateTime;
   };

   //! @name Priority queue methods.
   //@{
   bool PlatformsQueued() { return (!mPlatformQueue.empty()); }
   bool PopNextPlatformElement(PlatformElement& aPlatformElement);

   bool SensorsQueued() { return (!mSensorQueue.empty()); }
   bool PopNextSensorElement(SensorElement& aSensorElement);
   //@}

   //! Implementation of worker thread for updating a platform.
   class SimulationUpdateThread : public WsfThread
   {
   public:
      //! Constructor and destructor
      SimulationUpdateThread(WsfMultiThreadManager* aManagerPtr);
      ~SimulationUpdateThread() override {}

      FunctionType DoWork() override;
      FunctionType Pause() override { return AVAILABLE; }

      //! Returns true if there is no work for thread to perform
      bool NoWork();

      WsfMultiThreadManager* mManagerPtr;
   };

   struct ThreadFactory
   {
   public:
      ThreadFactory(WsfMultiThreadManager* aManagerPtr)
         : mManagerPtr(aManagerPtr)
      {
      }

      WsfMultiThreadManager*  mManagerPtr;
      SimulationUpdateThread* operator()() const;
   };

public:
   friend class SimulationUpdateThread;

private:
   //! Copy Constructor
   WsfMultiThreadManager(const WsfMultiThreadManager& aSrc);

   //! Prevent use of operator= by declaring, but not defining.
   WsfMultiThreadManager& operator=(const WsfMultiThreadManager& aRhs) = delete;


   WsfSimulation*                     mSimulationPtr;
   std::queue<PlatformElement>        mPlatformQueue;
   std::priority_queue<SensorElement> mSensorQueue;

   unsigned int                                          mNumberOfThreads;
   WsfThreadPool<SimulationUpdateThread, ThreadFactory>* mThreads;

   std::vector<size_t> mThreadedPlatforms;
   std::vector<size_t> mNonThreadedPlatforms;

   std::vector<WsfSensor*> mThreadedSensors;
   std::vector<WsfSensor*> mNonThreadedSensors;

   double mBreakUpdateTime;
   bool   mBreakUpdate;

   bool mDebug;

   std::recursive_mutex mMutex;
};

#endif
