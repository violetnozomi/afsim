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

#include "WsfLOS_Manager.hpp"

#include <algorithm>
#include <iterator>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfApplication.hpp"
#include "WsfArticulatedPart.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"

namespace // anonymous
{

class OptionalLock
{
public:
   OptionalLock(bool aShouldLock, std::recursive_mutex& aMutex)
      : mShouldLock(aShouldLock)
      , mMutex(aMutex)
   {
      if (mShouldLock)
      {
         mMutex.lock();
      }
   }

   ~OptionalLock()
   {
      if (mShouldLock)
      {
         mMutex.unlock();
      }
   }

private:
   bool                  mShouldLock;
   std::recursive_mutex& mMutex;
};

} // namespace

// ****************************************************************************
// Public

WsfLOS_Manager* WsfLOS_Manager::Find(const WsfSimulation& aSimulation)
{
   return static_cast<WsfLOS_Manager*>(aSimulation.FindExtension("los_manager"));
}

// ============================================================================
// Virtual
WsfLOS_Manager::~WsfLOS_Manager()
{
   // Stop the thread even if it still has work in the queue
   if (mMultiThreaded)
   {
      mWorkerThread.WaitUntilWorkDone();
      mWorkerThread.StopWork();
      mWorkerThread.EndThread();
      mWorkerThread.Join();
   }
}

// ============================================================================
//! Initializes the LOS manager; called by WsfSimulation.
//!
//! @return 'true'.
bool WsfLOS_Manager::Initialize()
{
   // Need to know when a platform is deleted from the simulation so unnecessary LOS checks are not performed
   mCallbacks.Add(WsfObserver::PlatformDeleted(&GetSimulation()).Connect(&WsfLOS_Manager::PlatformDeleted, this));

   // Start the worker thread if multi-threaded
   if (mMultiThreaded)
   {
      // Set up the priority queues based on input or default values
      mWorkerThread.Initialize(this);

      // Create the actual thread and start it working
      mWorkerThread.Start();
      mWorkerThread.AssignWork();
   }

   return true;
}

// ============================================================================
//! Process the line_of_sight_manager block.
//!
//! @param aInput    [input]  The input stream.
//!
//! @return 'true' if the command was recognized (and processed) or 'false' if the command was not
//! one recognized by this class.
bool WsfLOS_ManagerExtension::ProcessInput(UtInput& aInput)
{
   return mData.ProcessInput(aInput);
}

// ============================================================================
//! Process the line_of_sight_manager block.
//!
//! @param aInput    [input]  The input stream.
//!
//! @return 'true' if the command was recognized (and processed) or 'false' if the command was not
//! one recognized by this class.
bool WsfLOS_ManagerData::ProcessInput(UtInput& aInput)
{
   bool        ok(false);
   std::string tag = aInput.GetCommand();
   if (tag == "line_of_sight_manager")
   {
      UtInputBlock block(aInput, "end_line_of_sight_manager");
      ok = true;
      while (block.ReadCommand())
      {
         tag = block.GetCommand();
         if (tag == "maximum_location_change")
         {
            aInput.ReadValueOfType(mMaxAllowableLocDelta, UtInput::cLENGTH);
            aInput.ValueGreater(mMaxAllowableLocDelta, 0.0);
         }
         else if (tag == "multi_thread")
         {
            mMultiThreaded = true;
         }
         else if (tag == "multi_threading")
         {
            aInput.ReadValue(mMultiThreaded);
         }
         else if (tag == "number_of_priority_levels")
         {
            aInput.ReadValue(mThreadData.mNumPriorityQueues);
            aInput.ValueGreater(mThreadData.mNumPriorityQueues, 1U);
         }
         else if (tag == "max_steps_in_priority_level")
         {
            aInput.ReadValue(mThreadData.mMaxCountPriorityQueue);
            aInput.ValueGreater(mThreadData.mMaxCountPriorityQueue, 1U);
         }
         else if (tag == "processing_rate")
         {
            aInput.ReadValue(mThreadData.mProcessingRate);
            aInput.ValueGreater(mThreadData.mProcessingRate, 1U);
         }
         else if (tag == "debug")
         {
            mDebugEnabled = true;
         }
         else if (tag == "debug_thread")
         {
            mThreadData.mDebugEnabled = true;
         }
         else if (tag == "vegetation_layer_masking")
         {
            aInput.ReadValue(mVegLayerMaskingEnabled);
         }
         else if (tag == "use_height_for_ground_platforms")
         {
            aInput.ReadValue(mUseHeightForGroundPlatforms);
         }
         else
         {
            ok = false;
         }
      }
   }
   return ok;
}

void WsfLOS_ManagerExtension::SimulationCreated(WsfSimulation& aSimulation)
{
   aSimulation.RegisterExtension(GetExtensionName(), ut::make_unique<WsfLOS_Manager>(mData));
}

// ============================================================================
//! Perform cleanup associated with a platform being deleted from the simulation.
//!
//! @param aSimTime       [input] The current simulation time.
//! @param aPlatformPtr   [input] Pointer to the platform to be deleted.
void WsfLOS_Manager::PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr)
{
   // Save off the simulation time
   mLOS_Time = aSimTime;

   if (mMultiThreaded)
   {
      // NOTE: state data is checked in UpdateStateData function
      // Add the platform ID to the list
      mWorkerThread.mIdsToDelete.insert(aPlatformPtr->GetUniqueId());

      // Add the platform part IDs to the list
      StateMapTypeIterator stateIter;
      for (WsfComponentList::RoleIterator<WsfArticulatedPart> iter(*aPlatformPtr); !iter.AtEnd(); ++iter)
      {
         WsfArticulatedPart* partPtr = (*iter);
         if (partPtr != nullptr)
         {
            if (StateDataExists(partPtr->GetUniqueId(), stateIter))
            {
               mWorkerThread.mIdsToDelete.insert(partPtr->GetUniqueId());
            }
         }
      }
   }
   else
   {
      OptionalLock lock{GetSimulation().MultiThreadingActive(), mMutex};

      // Check for platform entries

      // Clean up the line-of-sight data; may be more than one entry
      DeleteLOS_Data(aPlatformPtr->GetUniqueId(), mLOS_Data);

      // Clean up the state data; should only be one entry
      StateMapTypeIterator stateIter;
      if (StateDataExists(aPlatformPtr->GetUniqueId(), stateIter))
      {
         mStateData.erase(stateIter);
      }

      // Check for platform parts
      for (WsfComponentList::RoleIterator<WsfArticulatedPart> iter(*aPlatformPtr); !iter.AtEnd(); ++iter)
      {
         DeleteLOS_Data(iter->GetUniqueId(), mLOS_Data);
         if (StateDataExists(iter->GetUniqueId(), stateIter))
         {
            mStateData.erase(stateIter);
         }
      }
   }
}

// ============================================================================
//! Called by WsfFrameStepSimulatio::AdvanceFrame. Updates the state data.
//!
//! @param aSimTime       [input] The current simulation time.
void WsfLOS_Manager::Update(double aSimTime)
{
   // Save off the simulation time
   mLOS_Time = aSimTime;

   // Update the platform data if multi-threaded
   if (mMultiThreaded)
   {
      if (mDebugEnabled)
      {
         auto out = ut::log::debug() << "LOS Manager: Multi-thread update. Pausing.";
         out.AddNote() << "T = " << aSimTime;
      }

      // Pause the thread; keep trying until success; only citical thread work currently causes failure
      while (!mWorkerThread.PauseWork())
      {
      }

      // Wait until thread is available
      mWorkerThread.WaitUntilWorkDone();

      // Thread is NOT processing so ...
      // Update the location data in LOS Manager
      LOS_RequestType tempRequestQueue;
      UpdateStateData(tempRequestQueue);

      if (mDebugEnabled)
      {
         auto out = ut::log::debug() << "LOS Manager: Multi-thread update.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Request Count: " << tempRequestQueue.size();
      }

      // Copy the requests to the worker thread; the request queue identifies only those platforms or parts that have moved
      mWorkerThread.mTime = aSimTime;
      std::copy(tempRequestQueue.begin(), tempRequestQueue.end(), std::back_inserter(mWorkerThread.mRequestQueue));

      // Copy any new LOS data to the worker thread and clear the LOS manager map
      LOS_MapTypeIterator iter    = mLOS_Data.begin();
      LOS_MapTypeIterator iterEnd = mLOS_Data.end();
      for (; iter != iterEnd; ++iter)
      {
         mWorkerThread.SetThreadLOS_Data((*iter).first, (*iter).second);
      }
      mLOS_Data.clear();

      // Remove deleted platforms and parts; if this gets too time consuming may have to do as part of routine thread work
      LOS_Thread::IdSetTypeIterator pIter    = mWorkerThread.mIdsToDelete.begin();
      LOS_Thread::IdSetTypeIterator pIterEnd = mWorkerThread.mIdsToDelete.end();
      for (; pIter != pIterEnd; ++pIter)
      {
         DeleteLOS_Data((*pIter), mWorkerThread.mThreadLOS_Data);
         mWorkerThread.CleanUpPriorityQueues((*pIter));
         if (mDebugEnabled)
         {
            auto out = ut::log::debug() << "LOS Manager: Multi-thread update. Platform deleted.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << *pIter;
         }
      }
      mWorkerThread.mIdsToDelete.clear();

      // Assign work to the thread
      mWorkerThread.AssignWork();
   }
}

// ============================================================================
//! Is the line-of-sight to a target visible.
//!
//! @param aPlatformPtr           [input] Pointer to the source.
//! @param aTargetPtr             [input] Pointer to the target.
//! @param aMaxRange              [input] Distance at which to stop checking. (default = 0.0; no range checking)
//! @param aRadiusScale           [input] Effective earth radius multiplier (default = 1.0)
//!
//! @return 'true' if target is visible or 'false' if not.
bool WsfLOS_Manager::IsTargetVisible(WsfPlatform* aPlatformPtr, WsfPlatform* aTargetPtr, double aMaxRange, double aRadiusScale)
{
   // Default to target NOT visible
   bool isTargetVisible(false);

   // Save off the simulation time
   double simTime(GetSimulation().GetSimTime());
   if (simTime > 0.0)
   {
      mLOS_Time = simTime;
   }

   if (mMultiThreaded)
   {
      // Check in the thread data container first
      LOS_MapTypeIterator iter;
      if (LOS_DataExists(aPlatformPtr->GetUniqueId(), aTargetPtr->GetUniqueId(), mWorkerThread.mThreadLOS_Data, iter))
      {
         isTargetVisible = (*iter).second;

         if (mDebugEnabled)
         {
            auto out = ut::log::debug() << "LOS Manager. LOS " << (isTargetVisible ? "succeeded" : "failed") << ".";
            out.AddNote() << "T = " << mLOS_Time;
            out.AddNote() << "Platform A: " << aPlatformPtr->GetName();
            out.AddNote() << "Platform B: " << aTargetPtr->GetName();
            out.AddNote() << "Source: Thread Cache";
         }
      }
      else
      {
         // Multi-threaded but this is the first check so update the LOS manager
         isTargetVisible = IsTargetVisibleNow(aPlatformPtr, aTargetPtr, aMaxRange, aRadiusScale);
      }
   }
   else
   {
      // NOT multi-threaded
      isTargetVisible = IsTargetVisibleNow(aPlatformPtr, aTargetPtr, aMaxRange, aRadiusScale);
   }

   return isTargetVisible;
}

// ============================================================================
//! Is the line-of-sight to a target visible.
//!
//! @param aAntennaPtr            [input] Pointer to the source.
//! @param aTargetPtr             [input] Pointer to the target.
//! @param aMaxRange              [input] Distance at which to stop checking. (default = 0.0; no range checking)
//! @param aRadiusScale           [input] Effective earth radius multiplier (default = 1.0)
//!
//! @return 'true' if target is visible or 'false' if not.
bool WsfLOS_Manager::IsTargetVisible(WsfEM_Antenna* aAntennaPtr, WsfPlatform* aTargetPtr, double aMaxRange, double aRadiusScale)
{
   // Default to target NOT visible
   bool isTargetVisible(false);

   // Save off the simulation time
   double simTime(GetSimulation().GetSimTime());
   if (simTime > 0.0)
   {
      mLOS_Time = simTime;
   }

   if (mMultiThreaded)
   {
      // Check in the thread data container first
      LOS_MapTypeIterator iter;
      if (LOS_DataExists(aAntennaPtr->GetArticulatedPart()->GetUniqueId(),
                         aTargetPtr->GetUniqueId(),
                         mWorkerThread.mThreadLOS_Data,
                         iter))
      {
         isTargetVisible = (*iter).second;

         if (mDebugEnabled)
         {
            auto out = ut::log::debug() << "LOS Manager. LOS " << (isTargetVisible ? "succeeded" : "failed") << ".";
            out.AddNote() << "T = " << mLOS_Time;
            out.AddNote() << "Platform A: " << aAntennaPtr->GetArticulatedPart()->GetPlatform()->GetName();
            out.AddNote() << "Part A: " << aAntennaPtr->GetArticulatedPart()->GetName();
            out.AddNote() << "Platform B: " << aTargetPtr->GetName();
            out.AddNote() << "Source: Thread Cache";
         }
      }
      else
      {
         // Multi-threaded but this is the first check
         isTargetVisible = IsTargetVisibleNow(aAntennaPtr, aTargetPtr, aMaxRange, aRadiusScale);
      }
   }
   else
   {
      // NOT multi-threaded
      isTargetVisible = IsTargetVisibleNow(aAntennaPtr, aTargetPtr, aMaxRange, aRadiusScale);
   }

   return isTargetVisible;
}

// ============================================================================
//! Is the line-of-sight to a target visible.
//!
//! @param aAntennaPtr            [input] Pointer to the source.
//! @param aTargetPtr             [input] Pointer to the target.
//! @param aMaxRange              [input] Distance at which to stop checking. (default = 0.0; no range checking)
//! @param aRadiusScale           [input] Effective earth radius multiplier (default = 1.0)
//!
//! @return 'true' if target is visible or 'false' if not.
bool WsfLOS_Manager::IsTargetVisible(WsfEM_Antenna* aAntennaPtr, WsfEM_Antenna* aTargetPtr, double aMaxRange, double aRadiusScale)
{
   // Default to target NOT visible
   bool isTargetVisible(false);

   // Save off the simulation time
   double simTime(GetSimulation().GetSimTime());
   if (simTime > 0.0)
   {
      mLOS_Time = simTime;
   }

   if (mMultiThreaded)
   {
      // Check in the thread data container first
      LOS_MapTypeIterator iter;
      if (LOS_DataExists(aAntennaPtr->GetArticulatedPart()->GetUniqueId(),
                         aTargetPtr->GetArticulatedPart()->GetUniqueId(),
                         mWorkerThread.mThreadLOS_Data,
                         iter))
      {
         isTargetVisible = (*iter).second;

         if (mDebugEnabled)
         {
            auto out = ut::log::debug() << "LOS Manager. LOS " << (isTargetVisible ? "succeeded" : "failed") << ".";
            out.AddNote() << "T = " << mLOS_Time;
            out.AddNote() << "Platform A: " << aAntennaPtr->GetArticulatedPart()->GetPlatform()->GetName();
            out.AddNote() << "Part A: " << aAntennaPtr->GetArticulatedPart()->GetName();
            out.AddNote() << "Platform B: " << aTargetPtr->GetArticulatedPart()->GetPlatform()->GetName();
            out.AddNote() << "Part B: " << aTargetPtr->GetArticulatedPart()->GetName();
            out.AddNote() << "Source: Thread Cache";
         }
      }
      else
      {
         // Multi-threaded but this is the first check
         isTargetVisible = IsTargetVisibleNow(aAntennaPtr, aTargetPtr, aMaxRange, aRadiusScale);
      }
   }
   else
   {
      // NOT multi-threaded
      isTargetVisible = IsTargetVisibleNow(aAntennaPtr, aTargetPtr, aMaxRange, aRadiusScale);
   }

   return isTargetVisible;
}

// ============================================================================
//! Is the line-of-sight to a target obscured by the terrain.
//!
//! @param aPlatformPtr           [input] Pointer to the source.
//! @param aTargetPtr             [input] Pointer to the target.
//! @param aMaxRange              [input] Distance at which to stop checking. (default = 0.0; no range checking)
//! @param aRadiusScale           [input] Effective earth radius multiplier (default = 1.0)
//!
//! @return 'true' if masked by terrain or 'false' if not.
bool WsfLOS_Manager::MaskedByTerrain(WsfPlatform* aPlatformPtr, WsfPlatform* aTargetPtr, double aMaxRange, double aRadiusScale)
{
   // Get the location of the source
   double srcLat(0.0);
   double srcLon(0.0);
   double srcAlt(0.0);
   aPlatformPtr->GetLocationLLA(srcLat, srcLon, srcAlt);

   if (mUseHeightForGroundPlatforms && (aPlatformPtr->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_LAND))
   {
      srcAlt += aPlatformPtr->GetHeight();
   }

   // Get the location of the target
   double tgtLat(0.0);
   double tgtLon(0.0);
   double tgtAlt(0.0);
   aTargetPtr->GetLocationLLA(tgtLat, tgtLon, tgtAlt);

   if (mUseHeightForGroundPlatforms && (aTargetPtr->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_LAND))
   {
      tgtAlt += aTargetPtr->GetHeight();
   }

   return GetSimulation()
      .GetTerrainInterface()
      ->MaskedByTerrain(srcLat, srcLon, srcAlt, tgtLat, tgtLon, tgtAlt, aMaxRange, aRadiusScale);
}

// ============================================================================
//! Is the line-of-sight to a target obscured by the terrain.
//!
//! @param aAntennaPtr            [input] Pointer to the source.
//! @param aTargetPtr             [input] Pointer to the target.
//! @param aMaxRange              [input] Distance at which to stop checking. (default = 0.0; no range checking)
//! @param aRadiusScale           [input] Effective earth radius multiplier (default = 1.0)
//!
//! @return 'true' if masked by terrain or 'false' if not.
bool WsfLOS_Manager::MaskedByTerrain(WsfEM_Antenna* aAntennaPtr, WsfPlatform* aTargetPtr, double aMaxRange, double aRadiusScale)
{
   // Get the location of the source
   double srcLat(0.0);
   double srcLon(0.0);
   double srcAlt(0.0);
   aAntennaPtr->GetLocationLLA(srcLat, srcLon, srcAlt);

   // Get the location of the target
   double tgtLat(0.0);
   double tgtLon(0.0);
   double tgtAlt(0.0);
   aTargetPtr->GetLocationLLA(tgtLat, tgtLon, tgtAlt);

   if (mUseHeightForGroundPlatforms && (aTargetPtr->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_LAND))
   {
      tgtAlt += aTargetPtr->GetHeight();
   }

   return GetSimulation()
      .GetTerrainInterface()
      ->MaskedByTerrain(srcLat, srcLon, srcAlt, tgtLat, tgtLon, tgtAlt, aMaxRange, aRadiusScale);
}

// ============================================================================
//! Is the line-of-sight to a target obscured by the terrain.
//!
//! @param aAntennaPtr            [input] Pointer to the source.
//! @param aTargetPtr             [input] Pointer to the target.
//! @param aMaxRange              [input] Distance at which to stop checking. (default = 0.0; no range checking)
//! @param aRadiusScale           [input] Effective earth radius multiplier (default = 1.0)
//!
//! @return 'true' if masked by terrain or 'false' if not.
bool WsfLOS_Manager::MaskedByTerrain(WsfEM_Antenna* aAntennaPtr, WsfEM_Antenna* aTargetPtr, double aMaxRange, double aRadiusScale)
{
   // Get the location of the source
   double srcLat(0.0);
   double srcLon(0.0);
   double srcAlt(0.0);
   aAntennaPtr->GetLocationLLA(srcLat, srcLon, srcAlt);

   // Get the location of the target
   double tgtLat(0.0);
   double tgtLon(0.0);
   double tgtAlt(0.0);
   aTargetPtr->GetLocationLLA(tgtLat, tgtLon, tgtAlt);

   return GetSimulation()
      .GetTerrainInterface()
      ->MaskedByTerrain(srcLat, srcLon, srcAlt, tgtLat, tgtLon, tgtAlt, aMaxRange, aRadiusScale);
}

// Vegetation methods

// ============================================================================
//! Is the line-of-sight to a target obscured by the vegetation layer.
//!
//! @param aPlatformPtr           [input] Pointer to the source.
//! @param aTargetPtr             [input] Pointer to the target.
//! @param aMaxRange              [input] Distance at which to stop checking. (default = 0.0; no range checking)
//! @param aRadiusScale           [input] Effective earth radius multiplier (default = 1.0)
//!
//! @return 'true' if masked by terrain or 'false' if not.
bool WsfLOS_Manager::MaskedByVegetation(WsfPlatform* aPlatformPtr, WsfPlatform* aTargetPtr, double aMaxRange, double aRadiusScale)
{
   // Get the location of the source
   double srcLat(0.0);
   double srcLon(0.0);
   double srcAlt(0.0);
   aPlatformPtr->GetLocationLLA(srcLat, srcLon, srcAlt);

   // Get the location of the target
   double tgtLat(0.0);
   double tgtLon(0.0);
   double tgtAlt(0.0);
   aTargetPtr->GetLocationLLA(tgtLat, tgtLon, tgtAlt);

   return GetSimulation()
      .GetTerrainInterface()
      ->MaskedByVegetation(srcLat, srcLon, srcAlt, tgtLat, tgtLon, tgtAlt, aMaxRange, aRadiusScale);
}

// ============================================================================
//! Is the line-of-sight to a target obscured by the vegetation layer.
//!
//! @param aAntennaPtr            [input] Pointer to the source.
//! @param aTargetPtr             [input] Pointer to the target.
//! @param aMaxRange              [input] Distance at which to stop checking. (default = 0.0; no range checking)
//! @param aRadiusScale           [input] Effective earth radius multiplier (default = 1.0)
//!
//! @return 'true' if masked by terrain or 'false' if not.
bool WsfLOS_Manager::MaskedByVegetation(WsfEM_Antenna* aAntennaPtr, WsfPlatform* aTargetPtr, double aMaxRange, double aRadiusScale)
{
   // Get the location of the source
   double srcLat(0.0);
   double srcLon(0.0);
   double srcAlt(0.0);
   aAntennaPtr->GetLocationLLA(srcLat, srcLon, srcAlt);

   // Get the location of the target
   double tgtLat(0.0);
   double tgtLon(0.0);
   double tgtAlt(0.0);
   aTargetPtr->GetLocationLLA(tgtLat, tgtLon, tgtAlt);

   return GetSimulation()
      .GetTerrainInterface()
      ->MaskedByVegetation(srcLat, srcLon, srcAlt, tgtLat, tgtLon, tgtAlt, aMaxRange, aRadiusScale);
}

// ============================================================================
//! Is the line-of-sight to a target obscured by the vegetation layer.
//!
//! @param aAntennaPtr            [input] Pointer to the source.
//! @param aTargetPtr             [input] Pointer to the target.
//! @param aMaxRange              [input] Distance at which to stop checking. (default = 0.0; no range checking)
//! @param aRadiusScale           [input] Effective earth radius multiplier (default = 1.0)
//!
//! @return 'true' if masked by terrain or 'false' if not.
bool WsfLOS_Manager::MaskedByVegetation(WsfEM_Antenna* aAntennaPtr, WsfEM_Antenna* aTargetPtr, double aMaxRange, double aRadiusScale)
{
   // Get the location of the source
   double srcLat(0.0);
   double srcLon(0.0);
   double srcAlt(0.0);
   aAntennaPtr->GetLocationLLA(srcLat, srcLon, srcAlt);

   // Get the location of the target
   double tgtLat(0.0);
   double tgtLon(0.0);
   double tgtAlt(0.0);
   aTargetPtr->GetLocationLLA(tgtLat, tgtLon, tgtAlt);

   return GetSimulation()
      .GetTerrainInterface()
      ->MaskedByVegetation(srcLat, srcLon, srcAlt, tgtLat, tgtLon, tgtAlt, aMaxRange, aRadiusScale);
}

// ****************************************************************************
// Protected

// Constructor
WsfLOS_Manager::WsfLOS_Manager(WsfLOS_ManagerData& aData)
   : WsfSimulationExtension()
   , WsfLOS_ManagerData(aData)
   , mLOS_Time(0.0)
   , mLOS_Data()
   , mStateData()
   , mCallbacks()
   , mWorkerThread(aData.mThreadData)
   , mMutex()
{
}

// ============================================================================
//! Is the line-of-sight to a target visible.
//!
//! @param aPlatformPtr           [input] Pointer to the source.
//! @param aTargetPtr             [input] Pointer to the target.
//! @param aMaxRange              [input] Distance at which to stop checking. (default = 0.0; no range checking)
//! @param aRadiusScale           [input] Effective earth radius multiplier (default = 1.0)
//!
//! @return 'true' if target is visible or 'false' if not.
bool WsfLOS_Manager::IsTargetVisibleNow(WsfPlatform* aPlatformPtr, WsfPlatform* aTargetPtr, double aMaxRange, double aRadiusScale)
{
   // Default to target NOT visible
   bool isTargetVisible(false);

   // Determine if either entity has moved and get the state
   bool aMoved(Moved(aPlatformPtr));
   bool bMoved(Moved(aTargetPtr));

   // Used in debug messages
   std::string resultStr = "";

   // if either entity has moved beyond the tolerance; then recheck the line-of-sight
   // need to check both for movement since state needs to be updated if movement occurred
   if (aMoved || bMoved || !LOS_DataExists(aPlatformPtr->GetUniqueId(), aTargetPtr->GetUniqueId()))
   {
      // At least one of the entities has moved; check terrain masking
      if (mVegLayerMaskingEnabled)
      {
         isTargetVisible = !MaskedByVegetation(aPlatformPtr, aTargetPtr, aMaxRange, aRadiusScale);
      }
      else
      {
         isTargetVisible = !MaskedByTerrain(aPlatformPtr, aTargetPtr, aMaxRange, aRadiusScale);
      }

      // Update the LOS data in the map
      SetLOS_Data(aPlatformPtr->GetUniqueId(), aTargetPtr->GetUniqueId(), isTargetVisible);

      if (mDebugEnabled)
      {
         resultStr = "Check";
      }
   }
   else
   {
      // Neither entity has moved enough for a LOS re-check; used the saved data from last check
      LOS_MapTypeIterator iter;
      if (LOS_DataExists(aPlatformPtr->GetUniqueId(), aTargetPtr->GetUniqueId(), iter))
      {
         isTargetVisible = (*iter).second;
      }

      if (mDebugEnabled)
      {
         resultStr = "Cache";
      }
   }

   if (mDebugEnabled)
   {
      auto out = ut::log::debug() << "LOS Manager. LOS " << (isTargetVisible ? "succeeded" : "failed") << ".";
      out.AddNote() << "T = " << mLOS_Time;
      out.AddNote() << "Platform A: " << aPlatformPtr->GetName();
      out.AddNote() << "Platform B: " << aTargetPtr->GetName();
      out.AddNote() << "Source: " << resultStr;
   }

   return isTargetVisible;
}

// ============================================================================
//! Is the line-of-sight to a target visible.
//!
//! @param aAntennaPtr            [input] Pointer to the source.
//! @param aTargetPtr             [input] Pointer to the target.
//! @param aMaxRange              [input] Distance at which to stop checking. (default = 0.0; no range checking)
//! @param aRadiusScale           [input] Effective earth radius multiplier (default = 1.0)
//!
//! @return 'true' if target is visible or 'false' if not.
bool WsfLOS_Manager::IsTargetVisibleNow(WsfEM_Antenna* aAntennaPtr, WsfPlatform* aTargetPtr, double aMaxRange, double aRadiusScale)
{
   // Default to target NOT visible
   bool isTargetVisible(false);

   // Determine if either entity has moved and get the state
   bool aMoved(Moved(aAntennaPtr));
   bool bMoved(Moved(aTargetPtr));

   // Used in debug messages
   std::string resultStr = "";

   // if either entity has moved beyond the tolerance; then recheck the line-of-sight
   // need to check both for movement since state needs to be updated if movement occurred
   if (aMoved || bMoved || !LOS_DataExists(aAntennaPtr->GetArticulatedPart()->GetUniqueId(), aTargetPtr->GetUniqueId()))
   {
      // At least one of the entities has moved; check terrain masking
      if (mVegLayerMaskingEnabled)
      {
         isTargetVisible = !MaskedByVegetation(aAntennaPtr, aTargetPtr, aMaxRange, aRadiusScale);
      }
      else
      {
         isTargetVisible = !MaskedByTerrain(aAntennaPtr, aTargetPtr, aMaxRange, aRadiusScale);
      }

      // Update the LOS data in the map
      SetLOS_Data(aAntennaPtr->GetArticulatedPart()->GetUniqueId(), aTargetPtr->GetUniqueId(), isTargetVisible);

      if (mDebugEnabled)
      {
         resultStr = "Check";
      }
   }
   else
   {
      // Neither entity has moved enough for a LOS re-check; used the saved data from last check
      LOS_MapTypeIterator iter;
      if (LOS_DataExists(aAntennaPtr->GetArticulatedPart()->GetUniqueId(), aTargetPtr->GetUniqueId(), iter))
      {
         isTargetVisible = (*iter).second;
      }

      if (mDebugEnabled)
      {
         resultStr = "Cache";
      }
   }

   if (mDebugEnabled)
   {
      auto out = ut::log::debug() << "LOS Manager. LOS " << (isTargetVisible ? "succeeded" : "failed") << ".";
      out.AddNote() << "T = " << mLOS_Time;
      out.AddNote() << "Platform A: " << aAntennaPtr->GetArticulatedPart()->GetPlatform()->GetName();
      out.AddNote() << "Part A: " << aAntennaPtr->GetArticulatedPart()->GetName();
      out.AddNote() << "Platform B: " << aTargetPtr->GetName();
      out.AddNote() << "Source: " << resultStr;
   }

   return isTargetVisible;
}

// ============================================================================
//! Is the line-of-sight to a target visible.
//!
//! @param aAntennaPtr            [input] Pointer to the source.
//! @param aTargetPtr             [input] Pointer to the target.
//! @param aMaxRange              [input] Distance at which to stop checking. (default = 0.0; no range checking)
//! @param aRadiusScale           [input] Effective earth radius multiplier (default = 1.0)
//!
//! @return 'true' if target is visible or 'false' if not.
bool WsfLOS_Manager::IsTargetVisibleNow(WsfEM_Antenna* aAntennaPtr, WsfEM_Antenna* aTargetPtr, double aMaxRange, double aRadiusScale)
{
   // Default to target NOT visible
   bool isTargetVisible(false);

   // Determine if either entity has moved and get the state
   bool aMoved(Moved(aAntennaPtr));
   bool bMoved(Moved(aTargetPtr));

   // Used in debug messages
   std::string resultStr = "";

   // if either entity has moved beyond the tolerance; then recheck the line-of-sight
   // need to check both for movement since state needs to be updated if movement occurred
   if (aMoved || bMoved ||
       !LOS_DataExists(aAntennaPtr->GetArticulatedPart()->GetUniqueId(), aTargetPtr->GetArticulatedPart()->GetUniqueId()))
   {
      // At least one of the entities has moved; check terrain masking
      if (mVegLayerMaskingEnabled)
      {
         isTargetVisible = !MaskedByVegetation(aAntennaPtr, aTargetPtr, aMaxRange, aRadiusScale);
      }
      else
      {
         isTargetVisible = !MaskedByTerrain(aAntennaPtr, aTargetPtr, aMaxRange, aRadiusScale);
      }

      // Update the LOS data in the map
      SetLOS_Data(aAntennaPtr->GetArticulatedPart()->GetUniqueId(),
                  aTargetPtr->GetArticulatedPart()->GetUniqueId(),
                  isTargetVisible);

      if (mDebugEnabled)
      {
         resultStr = "Check";
      }
   }
   else
   {
      // Neither entity has moved enough for a LOS re-check; used the saved data from last check
      LOS_MapTypeIterator iter;
      if (LOS_DataExists(aAntennaPtr->GetArticulatedPart()->GetUniqueId(),
                         aTargetPtr->GetArticulatedPart()->GetUniqueId(),
                         iter))
      {
         isTargetVisible = (*iter).second;
      }

      if (mDebugEnabled)
      {
         resultStr = "Cache";
      }
   }

   if (mDebugEnabled)
   {
      auto out = ut::log::debug() << "LOS Manager. LOS " << (isTargetVisible ? "succeeded" : "failed") << ".";
      out.AddNote() << "T = " << mLOS_Time;
      out.AddNote() << "Platform A: " << aAntennaPtr->GetArticulatedPart()->GetPlatform()->GetName();
      out.AddNote() << "Part A: " << aAntennaPtr->GetArticulatedPart()->GetName();
      out.AddNote() << "Platform B: " << aTargetPtr->GetArticulatedPart()->GetPlatform()->GetName();
      out.AddNote() << "Part B: " << aTargetPtr->GetArticulatedPart()->GetName();
      out.AddNote() << "Source: " << resultStr;
   }

   return isTargetVisible;
}

// ============================================================================
//! Has the entity moved beyond the allowable tolerance.
//!
//! @param aAntennaPtr            [input]  Pointer to the entity.
//!
//! @return 'true' if entity has moved or 'false' if not.
bool WsfLOS_Manager::Moved(WsfEM_Antenna* aAntennaPtr)
{
   return PartMoved(aAntennaPtr->GetArticulatedPart());
}

// ============================================================================
//! Has the entity moved beyond the allowable tolerance.
//!
//! @param aPlatformPtr           [input]  Pointer to the entity.
//!
//! @return 'true' if entity has moved or 'false' if not.
bool WsfLOS_Manager::Moved(WsfPlatform* aPlatformPtr)
{
   return PlatformMoved(aPlatformPtr);
}

// ============================================================================
//! Does line-of-sight data already exist for the two entities.
//!
//! @param aID1           [input] Unique ID of endpoint entity.
//! @param aID2           [input] Unique ID of other endpoint entity.
//!
//! @return 'true' if LOS data exists or 'false' if not.
bool WsfLOS_Manager::LOS_DataExists(unsigned int aID1, unsigned int aID2)
{
   OptionalLock lock{GetSimulation().MultiThreadingActive(), mMutex};
   return (mLOS_Data.find(LOS_Key(aID1, aID2)) != mLOS_Data.end());
}

// ============================================================================
//! Does line-of-sight data already exist for the two entities.
//!
//! @param aID1           [input]  Unique ID of endpoint entity.
//! @param aID2           [input]  Unique ID of other endpoint entity.
//! @param aIter          [output] Iterator pointing to the entry if it exists.
//!
//! @return 'true' if LOS data exists or 'false' if not.
bool WsfLOS_Manager::LOS_DataExists(unsigned int aID1, unsigned int aID2, LOS_MapTypeIterator& aIter)
{
   OptionalLock lock{GetSimulation().MultiThreadingActive(), mMutex};
   aIter = mLOS_Data.find(LOS_Key(aID1, aID2));
   return (aIter != mLOS_Data.end());
}

// ============================================================================
//! Does line-of-sight data already exist for the two entities.
//!
//! @param aID1           [input]  Unique ID of endpoint entity.
//! @param aID2           [input]  Unique ID of other endpoint entity.
//! @param aLOS_Map       [input]  LOS data container to search
//! @param aIter          [output] Iterator pointing to the entry if it exists.
//!
//! @return 'true' if LOS data exists or 'false' if not.
bool WsfLOS_Manager::LOS_DataExists(unsigned int aID1, unsigned int aID2, LOS_MapType& aLOS_Map, LOS_MapTypeIterator& aIter)
{
   OptionalLock lock{GetSimulation().MultiThreadingActive(), mMutex};
   aIter = aLOS_Map.find(LOS_Key(aID1, aID2));
   return (aIter != aLOS_Map.end());
}

// ============================================================================
//! Does state data already exist for the entity.
//!
//! @param aID            [input] Unique ID of entity.
//!
//! @return 'true' if state data exists or 'false' if not.
bool WsfLOS_Manager::StateDataExists(unsigned int aID)
{
   OptionalLock lock{GetSimulation().MultiThreadingActive(), mMutex};
   return (mStateData.find(aID) != mStateData.end());
}

// ============================================================================
//! Does state data already exist for the entity.
//!
//! @param aID            [input]  Unique ID of entity.
//! @param aIter          [output] Iterator pointing to the entry if it exists.
//!
//! @return 'true' if state data exists or 'false' if not.
bool WsfLOS_Manager::StateDataExists(unsigned int aID, StateMapTypeIterator& aIter)
{
   OptionalLock lock{GetSimulation().MultiThreadingActive(), mMutex};
   aIter = mStateData.find(aID);
   return (aIter != mStateData.end());
}

// ============================================================================
//! Set the line-of-sight data.
//!
//! @param aID1           [input]  Unique ID of endpoint entity.
//! @param aID2           [input]  Unique ID of other endpoint entity.
//! @param aVisible       [input]  True if line-of-sight between entities is not masked.
//!
//! @return 'true' if update was successful or 'false' if not.
bool WsfLOS_Manager::SetLOS_Data(unsigned int aID1, unsigned int aID2, bool aVisible)
{
   // Both ID must be greater than 0
   assert((aID1 > 0) && (aID2 > 0));

   LOS_Key newKey(aID1, aID2);

   {
      OptionalLock lock{!mMultiThreaded && GetSimulation().MultiThreadingActive(), mMutex};
      mLOS_Data[newKey] = aVisible;
   }

   return true;
}

// ============================================================================
//! Set the line-of-sight data.
//!
//! @param aID1           [input]  Unique ID of endpoint entity.
//! @param aID2           [input]  Unique ID of other endpoint entity.
//! @param aVisible       [input]  True if line-of-sight between entities is not masked.
//! @param aIter          [output] Iterator pointing to the entry if it exists.
//!
//! @return 'true' if update was successful or 'false' if not.
bool WsfLOS_Manager::SetLOS_Data(unsigned int aID1, unsigned int aID2, bool aVisible, LOS_MapTypeIterator& aIter)
{
   // Both ID must be greater than 0
   assert((aID1 > 0) && (aID2 > 0));

   LOS_Key newKey(aID1, aID2);
   bool    inserted;

   {
      OptionalLock lock{!mMultiThreaded && GetSimulation().MultiThreadingActive(), mMutex};
      std::tie(aIter, inserted) = mLOS_Data.emplace(newKey, aVisible);
   }
   aIter->second = aVisible;

   return true;
}

// ============================================================================
//! Set the state data.
//!
//! @param aID            [input]  Unique ID of entity.
//! @param aNewState      [input]  Current state of entity.
//!
//! @return 'true' if update was successful or 'false' if not.
bool WsfLOS_Manager::SetStateData(unsigned int aID, State& aNewState)
{
   {
      OptionalLock lock{!mMultiThreaded && GetSimulation().MultiThreadingActive(), mMutex};
      mStateData[aID] = aNewState;
   }

   return true;
}

// ============================================================================
//! Set the state data.
//!
//! @param aID            [input]  Unique ID of entity.
//! @param aNewState      [input]  Current state of entity.
//! @param aIter          [output] Iterator pointing to the entry if it exists.
//!
//! @return 'true' if update was successful or 'false' if not.
bool WsfLOS_Manager::SetStateData(unsigned int aID, State& aNewState, StateMapTypeIterator& aIter)
{
   {
      OptionalLock lock{!mMultiThreaded && GetSimulation().MultiThreadingActive(), mMutex};

      // equivalent to c++17 mStateData.insert_or_assign
      bool inserted;
      std::tie(aIter, inserted) = mStateData.emplace(aID, aNewState);
      if (!inserted)
      {
         aIter->second = aNewState;
      }
   }

   return true;
}

// ============================================================================
//! Update the LOS manager state data.
//!
//! @param aLOS_RequestQueue     [output] The list of platforms or parts that have moved this frame.
void WsfLOS_Manager::UpdateStateData(LOS_RequestType& aLOS_RequestQueue)
{
   // Clear the current request queue
   aLOS_RequestQueue.clear();

   // Loop through all the state data and update
   StateMapTypeIterator iter = mStateData.begin();
   while (iter != mStateData.end())
   {
      // Get the platform pointer; this may be data for a platform or an antenna (has pointer to it's articulated part)
      WsfPlatform* platformPtr = GetSimulation().GetPlatformByIndex((*iter).second.mPlatformIndex);
      if (platformPtr != nullptr)
      {
         bool hasMoved(false);

         unsigned int uniqueID((*iter).first);
         if (platformPtr->GetUniqueId() == uniqueID)
         {
            // This a platform
            hasMoved = PlatformMoveCheck(platformPtr, iter);
         }
         else
         {
            // This is an antenna so get it's articulated part
            WsfArticulatedPart* partPtr = platformPtr->GetArticulatedPart(uniqueID);
            if (partPtr != nullptr)
            {
               hasMoved = PartMoveCheck(partPtr, iter);
            }
         }

         if (hasMoved)
         {
            // Platform or part has moved so add to request queue
            aLOS_RequestQueue.push_back(uniqueID);
         }

         ++iter;
      }
      else
      {
         // Platform does not exist in the simulation
         mStateData.erase(iter++); // Must be postfix increment!!!
      }
   }
}

// ============================================================================
//! Delete all LOS data containing the given ID.
//!
//! @param aID              [input]  The ID to remove.
//! @param aLOS_Map         [input]  The LOS container to process.
void WsfLOS_Manager::DeleteLOS_Data(unsigned int aID, LOS_MapType& aLOS_Map)
{
   // Clean up the line-of-sight data; may be more than one entry
   // NOTE: Use algorithm as defined in Scott Meyer's Effective STL, Item 9.
   LOS_MapTypeIterator losIter = aLOS_Map.begin();
   while (losIter != aLOS_Map.end())
   {
      const LOS_Key& losKey = (*losIter).first;
      if (losKey == aID)
      {
         aLOS_Map.erase(losIter++); // Must be postfix increment!!!
      }
      else
      {
         ++losIter;
      }
   }
}

// ============================================================================
//! Performs a move check on a platform and save the state data.
//!
//! @param aPlatformPtr          [input]  A pointer to the platform.
//!
//! @return 'true' if the platform has moved or 'false' if not.
bool WsfLOS_Manager::PlatformMoved(WsfPlatform* aPlatformPtr)
{
   // Default to true
   bool hasMoved(true);

   StateMapTypeIterator iter;
   if (StateDataExists(aPlatformPtr->GetUniqueId(), iter))
   {
      // Perform a move check on the platform
      hasMoved = PlatformMoveCheck(aPlatformPtr, iter);
   }
   else
   {
      // State data does NOT exist
      State newState;
      newState.mPlatformIndex = aPlatformPtr->GetIndex();
      // no need to set newState.mTime
      aPlatformPtr->GetLocationWCS(newState.mLocationWCS);
      aPlatformPtr->GetLocationLLA(newState.mLLA[0], newState.mLLA[1], newState.mLLA[2]);
      newState.mMoved = true;
      SetStateData(aPlatformPtr->GetUniqueId(), newState);

      if (mDebugEnabled)
      {
         auto out = ut::log::debug() << "LOS Manager: Initial movement check.";
         out.AddNote() << "T = " << mLOS_Time;
         out.AddNote() << "Platform: " << aPlatformPtr->GetName();
      }
   }

   return hasMoved;
}

// ============================================================================
//! Performs a move check on a platform and save the state data.
//!
//! @param aPlatformPtr          [input]  A pointer to the platform.
//! @param aIter                 [input]  Iterator pointing to the entry.
//!
//! @return 'true' if the platform has moved or 'false' if not.
bool WsfLOS_Manager::PlatformMoveCheck(WsfPlatform* aPlatformPtr, StateMapTypeIterator& aIter)
{
   // Default to true
   bool hasMoved(true);

   // Declare here for use in debugging message
   double moveMagSquared(0.0);

   // If the move state is not current
   if ((*aIter).second.mTime < mLOS_Time)
   {
      // Get the current location of the entity
      double currentLocWCS[3] = {0.0};
      aPlatformPtr->GetLocationWCS(currentLocWCS);

      double currentLocLLA[3] = {0.0};
      aPlatformPtr->GetLocationLLA(currentLocLLA[0], currentLocLLA[1], currentLocLLA[2]);

      // Find the relative location in WCS
      double relLocWCS[3] = {0.0};
      UtVec3d::Subtract(relLocWCS, (*aIter).second.mLocationWCS, currentLocWCS);

      moveMagSquared = UtVec3d::MagnitudeSquared(relLocWCS);
      if (moveMagSquared <= (mMaxAllowableLocDelta * mMaxAllowableLocDelta))
      {
         hasMoved = false;
      }
      else
      {
         State newState;
         newState.mPlatformIndex = (*aIter).second.mPlatformIndex;
         newState.mTime          = mLOS_Time;
         UtVec3d::Set(newState.mLocationWCS, currentLocWCS);
         UtVec3d::Set(newState.mLLA, currentLocLLA);
         newState.mMoved = hasMoved;
         SetStateData(aPlatformPtr->GetUniqueId(), newState);
      }
   }
   else
   {
      // Move check has already been performed at the current simulation time
      hasMoved = (*aIter).second.mMoved;
   }

   if (mDebugEnabled && hasMoved)
   {
      auto out = ut::log::debug() << "LOS Manager: Platform has moved.";
      out.AddNote() << "T = " << mLOS_Time;
      out.AddNote() << "Platform: " << aPlatformPtr->GetName();
      out.AddNote() << "Magnitude: " << std::sqrt(moveMagSquared);
      out.AddNote() << "Max Allowed: " << mMaxAllowableLocDelta;
   }

   return hasMoved;
}

// ============================================================================
//! Performs a move check on a platform part and save the state data.
//!
//! @param aPartPtr              [input]  A pointer to the platform.
//!
//! @return 'true' if the platform part has moved or 'false' if not.
bool WsfLOS_Manager::PartMoved(WsfArticulatedPart* aPartPtr)
{
   // Default to true
   bool hasMoved(true);

   StateMapTypeIterator iter;
   if (StateDataExists(aPartPtr->GetUniqueId(), iter))
   {
      // Perform a move check on the platform
      hasMoved = PartMoveCheck(aPartPtr, iter);
   }
   else
   {
      // State data does NOT exist
      State newState;
      newState.mPlatformIndex = aPartPtr->GetPlatform()->GetIndex();
      // no need to set newState.mTime = aSimTime;
      aPartPtr->GetLocationWCS(newState.mLocationWCS);
      aPartPtr->GetLocationLLA(newState.mLLA[0], newState.mLLA[1], newState.mLLA[2]);
      newState.mMoved = true;
      SetStateData(aPartPtr->GetUniqueId(), newState);

      if (mDebugEnabled)
      {
         auto out = ut::log::debug() << "LOS Manager: Initial movement check.";
         out.AddNote() << "T = " << mLOS_Time;
         out.AddNote() << "Platform: " << aPartPtr->GetPlatform()->GetName();
         out.AddNote() << "Part: " << aPartPtr->GetName();
      }
   }

   return hasMoved;
}

// ============================================================================
//! Performs a move check on a platform part and save the state data.
//!
//! @param aPartPtr              [input]  A pointer to the platform.
//! @param aIter                 [input]  Iterator pointing to the entry.
//!
//! @return 'true' if the platform part has moved or 'false' if not.
bool WsfLOS_Manager::PartMoveCheck(WsfArticulatedPart* aPartPtr, StateMapTypeIterator& aIter)
{
   // Default to true
   bool hasMoved(true);

   // Declare here for use in debugging message
   double moveMagSquared(0.0);

   // If the move state is not current
   if ((*aIter).second.mTime < mLOS_Time)
   {
      // Get the current location of the entity; fill the return value
      double currentLocWCS[3] = {0.0};
      aPartPtr->GetLocationWCS(currentLocWCS);

      double currentLocLLA[3] = {0.0};
      aPartPtr->GetLocationLLA(currentLocLLA[0], currentLocLLA[1], currentLocLLA[2]);

      // Find the relative location in WCS
      double relLocWCS[3] = {0.0};
      UtVec3d::Subtract(relLocWCS, (*aIter).second.mLocationWCS, currentLocWCS);

      moveMagSquared = UtVec3d::MagnitudeSquared(relLocWCS);
      if (moveMagSquared <= (mMaxAllowableLocDelta * mMaxAllowableLocDelta))
      {
         hasMoved = false;
      }
      else
      {
         State newState;
         newState.mPlatformIndex = (*aIter).second.mPlatformIndex;
         newState.mTime          = mLOS_Time;
         UtVec3d::Set(newState.mLocationWCS, currentLocWCS);
         UtVec3d::Set(newState.mLLA, currentLocLLA);
         newState.mMoved = hasMoved;
         SetStateData(aPartPtr->GetUniqueId(), newState);
      }
   }
   else
   {
      // Move check has already been performed at the current simulation time
      hasMoved = (*aIter).second.mMoved;
   }

   if (mDebugEnabled && hasMoved)
   {
      auto out = ut::log::debug() << "LOS Manager: Platform has moved.";
      out.AddNote() << "T = " << mLOS_Time;
      out.AddNote() << "Platform: " << aPartPtr->GetPlatform()->GetName();
      out.AddNote() << "Part: " << aPartPtr->GetName();
      out.AddNote() << "Magnitude: " << std::sqrt(moveMagSquared);
      out.AddNote() << "Max Allowed: " << mMaxAllowableLocDelta;
   }

   return hasMoved;
}


// ****************************************************************************
// Private

// ****************************************************************************
// ****************************************************************************
WsfLOS_Manager::LOS_Key::LOS_Key(unsigned int aUniqueId1, unsigned int aUniqueId2)
{
   // At least one ID must be greater than 0
   assert((aUniqueId1 > 0) || (aUniqueId2 > 0));

   // Allow one ID to be zero; if one ID is zero it will be mID1
   // Put the min ID in the first slot
   mID1 = std::min(aUniqueId1, aUniqueId2);
   mID2 = std::max(aUniqueId1, aUniqueId2);
}

bool WsfLOS_Manager::LOS_Key::operator<(const LOS_Key& aRhs) const
{
   bool isLessThan(false);

   unsigned int minLHS = std::min(mID1, mID2);
   unsigned int minRHS = std::min(aRhs.mID1, aRhs.mID2);

   if (minLHS < minRHS)
   {
      isLessThan = true;
   }
   else if (minLHS == minRHS)
   {
      unsigned int maxLHS = std::max(mID1, mID2);
      unsigned int maxRHS = std::max(aRhs.mID1, aRhs.mID2);
      if (maxLHS < maxRHS)
      {
         isLessThan = true;
      }
   }

   return isLessThan;
}

bool WsfLOS_Manager::LOS_Key::operator==(const LOS_Key& aRhs) const
{
   bool areEqual(false);
   if (((mID1 == aRhs.mID1) && (mID2 == aRhs.mID2)) || ((mID2 == aRhs.mID1) && (mID1 == aRhs.mID2)))
   {
      areEqual = true;
   }

   return areEqual;
}

bool WsfLOS_Manager::LOS_Key::operator==(const unsigned int& aRhs) const
{
   bool areEqual(false);
   if ((mID1 == aRhs) || (mID2 == aRhs))
   {
      areEqual = true;
   }

   return areEqual;
}

bool WsfLOS_Manager::LOS_Key::operator!=(const LOS_Key& aRhs) const
{
   bool areNotEqual(false);
   if (((mID1 == aRhs.mID1) && (mID2 != aRhs.mID2)) || ((mID1 == aRhs.mID2) && (mID2 != aRhs.mID1)))
   {
      areNotEqual = true;
   }

   return areNotEqual;
}

// ****************************************************************************
// ****************************************************************************
//! LOS thread constructor
WsfLOS_Manager::LOS_Thread::LOS_Thread(WsfLOS_ManagerData::ThreadData& aData)
   : WsfThread()
   , WsfLOS_ManagerData::ThreadData(aData)
   , mManagerPtr(nullptr)
   , mTime(0.0)
   , mRequestQueue()
   , mIdsToDelete()
   , mPriorityQueues()
   , mThreadLOS_Data()
{
}

// ============================================================================
//! Performs the thread work.
//!
//! @return Function state of the thread. (See WsfThread)
WsfLOS_Manager::LOS_Thread::FunctionType WsfLOS_Manager::LOS_Thread::DoWork()
{
   // Process all the requests
   ProcessRequests();

   // Get the platforms or parts to process based on the processing rate
   IdSetType idSet;
   ProcessPriorityQueue(idSet);

   // Process the LOS data
   ProcessThreadLOSData(idSet);

   // If there is currently no work in the queues then make the thread available
   if ((GetFunction() == ASSIGNED) && NoWork())
   {
      return AVAILABLE;
   }

   return GetFunction();
}

// ============================================================================
//! Initialized worker thread. Creates priority queues.
void WsfLOS_Manager::LOS_Thread::Initialize(WsfLOS_Manager* aManagerPtr)
{
   mManagerPtr = aManagerPtr;
   // Reserve the space for the queues
   mPriorityQueues.reserve(mNumPriorityQueues);
   for (unsigned int i = 0; i < mNumPriorityQueues; ++i)
   {
      mPriorityQueues.push_back(PriorityQueueType());
   }

   if (mDebugEnabled)
   {
      auto out = ut::log::debug() << "LOS Manager: Thread created.";
      out.AddNote() << "T = " << mTime;
      out.AddNote() << "Priority Queues: " << mNumPriorityQueues;
      out.AddNote() << "Steps: " << mMaxCountPriorityQueue;
      out.AddNote() << "Processing Rate: " << mProcessingRate;
   }
}

// ============================================================================
//! Process all requests in the request queue; results in updated priority queues.
void WsfLOS_Manager::LOS_Thread::ProcessRequests()
{
   if (mDebugEnabled)
   {
      auto out = ut::log::debug() << "LOS Manager Thread: Processing requests.";
      out.AddNote() << "T = " << mTime;
      out.AddNote() << "Request Count: " << mRequestQueue.size();
   }

   while (!mRequestQueue.empty())
   {
      // Process the request queue
      ProcessRequest(*(mRequestQueue.begin()));

      // Remove the processed request
      mRequestQueue.pop_front();
   }
}

// ============================================================================
//! Processes the priority queue by returning the highest priority requests; returns 0 to mProcessingRate requests.
//!
//! @param aIdSet    [input]  A set of unique IDs of moved platforms or platform parts.
void WsfLOS_Manager::LOS_Thread::ProcessPriorityQueue(IdSetType& aIdSet)
{
   PriorityQueueTypeIterator priorityIter;
   unsigned int              queueIndex(0);
   unsigned int              numProcessed(0);
   while ((queueIndex < mNumPriorityQueues) && (numProcessed < mProcessingRate))
   {
      if (!mPriorityQueues[queueIndex].empty())
      {
         // Get the first item in the queue
         priorityIter = mPriorityQueues[queueIndex].begin();
         aIdSet.insert((*priorityIter).first);

         // Remove from the priority queue
         mPriorityQueues[queueIndex].erase(priorityIter);

         // Increment the number of platforms processed
         ++numProcessed;
      }
      else
      {
         ++queueIndex;
      }
   }

   if (mDebugEnabled)
   {
      auto out = ut::log::debug() << "LOS Manager Thread: Requests processed.";
      out.AddNote() << "T = " << mTime;
      out.AddNote() << "Request Count: " << numProcessed;
   }
}

// ============================================================================
//! Updates the LOS data for the highest priority requests.
//!
//! @param aIdSet    [input]  A set of unique IDs of moved platforms or platform parts.
void WsfLOS_Manager::LOS_Thread::ProcessThreadLOSData(IdSetType& aIdSet)
{
   // Get the iterator to the end of the set
   IdSetTypeIterator platformSetIteratorEnd = aIdSet.end();

   // Loop through all the current LOS data entries and update any with at least one moved player or part
   LOS_MapTypeIterator sourceLosIter    = mThreadLOS_Data.begin();
   LOS_MapTypeIterator sourceLosIterEnd = mThreadLOS_Data.end();
   for (; sourceLosIter != sourceLosIterEnd; ++sourceLosIter)
   {
      // Get the LOS key
      const LOS_Key& losKey = (*sourceLosIter).first;

      // See if the platform or part moved
      if ((aIdSet.find(losKey.mID1) != platformSetIteratorEnd) || (aIdSet.find(losKey.mID2) != platformSetIteratorEnd))
      {
         // Found - so process
         StateMapTypeIterator p1 = mManagerPtr->GetStateData().find(losKey.mID1);
         StateMapTypeIterator p2 = mManagerPtr->GetStateData().find(losKey.mID2);

         if ((p1 == mManagerPtr->GetStateData().end()) || (p2 == mManagerPtr->GetStateData().end()))
         {
            ut::log::warning() << "LOS Manager: Inconsistent state data found for ID pair. Skipping.";
            continue;
         }

         bool isTargetVisible =
            !mManagerPtr->GetSimulation().GetTerrainInterface()->MaskedByTerrain((*p1).second.mLLA[0],
                                                                                 (*p1).second.mLLA[1],
                                                                                 (*p1).second.mLLA[2],
                                                                                 (*p2).second.mLLA[0],
                                                                                 (*p2).second.mLLA[1],
                                                                                 (*p2).second.mLLA[2],
                                                                                 0.0);

         // Update the LOS data in the thread
         SetThreadLOS_Data(losKey, isTargetVisible);

         if (mDebugEnabled)
         {
            WsfPlatform* platformPtr1 = mManagerPtr->GetSimulation().GetPlatformByIndex((*p1).second.mPlatformIndex);
            WsfPlatform* platformPtr2 = mManagerPtr->GetSimulation().GetPlatformByIndex((*p2).second.mPlatformIndex);
            if ((platformPtr1 != nullptr) && (platformPtr2 != nullptr))
            {
               auto out = ut::log::debug()
                          << "LOS Manager Thread: LOS " << (isTargetVisible ? "succeeded" : "failed") << ".";
               out.AddNote() << "T = " << mTime;
               out.AddNote() << "Platform A: " << platformPtr1->GetName();

               unsigned int uniqueID((*p1).first);
               if (platformPtr1->GetUniqueId() != uniqueID)
               {
                  WsfArticulatedPart* partPtr = platformPtr1->GetArticulatedPart(uniqueID);
                  if (partPtr != nullptr)
                  {
                     out.AddNote() << "Part A: " << partPtr->GetName();
                  }
               }

               uniqueID = (*p2).first;
               out.AddNote() << "Platform B: " << platformPtr2->GetName();
               if (platformPtr2->GetUniqueId() != uniqueID)
               {
                  WsfArticulatedPart* partPtr = platformPtr2->GetArticulatedPart(uniqueID);
                  if (partPtr != nullptr)
                  {
                     out.AddNote() << "Part B: " << partPtr->GetName();
                  }
               }
               out.AddNote() << "Source: Thread Check";
            }
         }
      }
   }
}

// ============================================================================
//! Processes a request by entering it into a priority queue.
//! Currently all requests are processed at a given time step i.e. mProcessingRate is not used here.
//!
//! @param aID    [input] Unique ID to process.
void WsfLOS_Manager::LOS_Thread::ProcessRequest(unsigned int aID)
{
   // Default to NOT processed
   bool processed(false);

   unsigned int              insertIndex(mNumPriorityQueues - 1);
   PriorityQueueTypeIterator priorityIter;
   for (unsigned int index = 0; index < mNumPriorityQueues; ++index)
   {
      // Look in priority queues
      priorityIter = mPriorityQueues[index].find(aID);
      if (priorityIter != mPriorityQueues[index].end()) // Found
      {
         // If at the highest priority queue i.e. index = 0
         // or the max count in the queue has not been reached
         if ((index == 0) || ((*priorityIter).second < (mMaxCountPriorityQueue - 1)))
         {
            // increment the request count; leave in the current queue
            ++(*priorityIter).second;
            processed = true;
         }
         else
         {
            // erase out of the current queue; insert in higher queue
            mPriorityQueues[index].erase(priorityIter);
            insertIndex = index - 1;
         }
         break;
      }
   }

   // Add to a priority queue given
   if (!processed)
   {
      mPriorityQueues[insertIndex].insert(PriorityQueueTypePair(aID, 1));
   }
}

// ============================================================================
//! Removes the entry with the given ID from the priority queues.
//!
//! @param aID    [input] Unique ID to process.
void WsfLOS_Manager::LOS_Thread::CleanUpPriorityQueues(unsigned int aID)
{
   PriorityQueueTypeIterator priorityIter;
   for (unsigned int index = 0; index < mNumPriorityQueues; ++index)
   {
      // Look in priority queues
      priorityIter = mPriorityQueues[index].find(aID);
      if (priorityIter != mPriorityQueues[index].end()) // Found
      {
         mPriorityQueues[index].erase(priorityIter);
         break;
      }
   }
}

// ============================================================================
//! Set the line-of-sight data.
//!
//! @param aLOS_Key       [input]  LOS key pair.
//! @param aVisible       [input]  True if line-of-sight between entities is not masked.
//!
//! @return 'true' if update was successful or 'false' if not.
bool WsfLOS_Manager::LOS_Thread::SetThreadLOS_Data(LOS_Key aLOS_Key, bool aVisible)
{
   bool dataUpated(false);

   LOS_MapTypeIterator iter = mThreadLOS_Data.find(aLOS_Key);
   if (iter == mThreadLOS_Data.end())
   {
      // Entry is NOT found
      std::pair<LOS_MapTypeIterator, bool> res = mThreadLOS_Data.insert(LOS_MapTypePair(aLOS_Key, aVisible));
      dataUpated                               = res.second;
   }
   else
   {
      // Entry is found
      (*iter).second = aVisible;
      dataUpated     = true;
   }

   return dataUpated;
}

// ============================================================================
//! Checks priority queues for work.
//!
//! @return 'true' if priority queues are empty or 'false' if not.
bool WsfLOS_Manager::LOS_Thread::NoWork()
{
   bool noWork(true);
   for (unsigned int index = 0; index < mNumPriorityQueues; ++index)
   {
      if (!mPriorityQueues[index].empty())
      {
         noWork = false;
         break;
      }
   }

   return noWork;
}

WsfLOS_ManagerData::WsfLOS_ManagerData()
   : mMaxAllowableLocDelta(0.0)
   , mMultiThreaded(false)
   , mDebugEnabled(false)
   , mVegLayerMaskingEnabled(false)
   , mUseHeightForGroundPlatforms(false)
{
}

WsfLOS_ManagerData::ThreadData::ThreadData()
   : mNumPriorityQueues(3)
   , mMaxCountPriorityQueue(5)
   , mProcessingRate(10)
   , mDebugEnabled(false)
{
}
