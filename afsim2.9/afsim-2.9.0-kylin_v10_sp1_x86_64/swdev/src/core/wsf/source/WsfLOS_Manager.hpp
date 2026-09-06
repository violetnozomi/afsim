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

#ifndef WSFLOSMANAGER_HPP
#define WSFLOSMANAGER_HPP

#include "wsf_export.h"

#include <list>
#include <map>
#include <mutex>
#include <set>

#include "UtCallbackHolder.hpp"
class UtInput;
#include "UtVec3.hpp"

class WsfArticulatedPart;
class WsfEM_Antenna;
class WsfPlatform;
#include "WsfScenarioExtension.hpp"
#include "WsfSimulationExtension.hpp"
#include "WsfThread.hpp"

//! Configuration data for the LOS Manager.
//! These data are broken out in order to provide an efficient way to
//! call ProcessInput from the scenario, then provide these data to
//! the LOSManager instance when created.
struct WSF_EXPORT WsfLOS_ManagerData
{
   WsfLOS_ManagerData();

   struct ThreadData
   {
      ThreadData();

      unsigned int mNumPriorityQueues;     //! Number of priority queues; default 3; minimum 1
      unsigned int mMaxCountPriorityQueue; //! Max count a platform can stay in the current queue
      //!    without being bumped up to a higher priority queue
      //!    default 5; minimum 1
      unsigned int mProcessingRate; //! Number of priority queue entries to process at a time
      bool         mDebugEnabled;
   };

   //! Process the line_of_sight_manager block.
   bool ProcessInput(UtInput& aInput);

   //! Maximum location change allowed to determine if move occurred.
   double mMaxAllowableLocDelta;

   //! Thread behavior
   bool mMultiThreaded;

   //! Debug flag
   bool mDebugEnabled;

   //! Vegetation Layer Masking
   bool mVegLayerMaskingEnabled;

   bool mUseHeightForGroundPlatforms;

   ThreadData mThreadData;
};

//! The scenario extension used to call ProcessInput and provide
//! configuration data to the LOSManager when created in SimulationCreated.
class WSF_EXPORT WsfLOS_ManagerExtension : public WsfScenarioExtension
{
public:
   WsfLOS_ManagerExtension() {}

   void SimulationCreated(WsfSimulation& aSimulation) override;

   bool ProcessInput(UtInput& aInput) override;

private:
   WsfLOS_ManagerData mData;
};

//! The line of sight manager provides a centralized location from which LOS
//! checks are handled.  These checks occur particularly frequently in sensor
//! AttemptToDetect methods.  Through caching LOS checks, particularly for
//! static platforms, the "m on n" computational problem is somewhat mitigated.
//!@note Line of sight manager Works in both a threaded and non-threaded mode.
class WSF_EXPORT WsfLOS_Manager : public WsfSimulationExtension, public WsfLOS_ManagerData
{
public:
   static WsfLOS_Manager* Find(const WsfSimulation& aSimulation);

   WsfLOS_Manager(WsfLOS_ManagerData& aData);
   ~WsfLOS_Manager() override;

   //! Initialize the LOS manager.
   bool Initialize() override;

   //! Removes LOS data when a platform is deleted.
   void PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr);

   //! Simulation time has advanced; platforms are updated
   void Update(double aSimTime);

   //! Determine if debugging is enabled.
   bool DebugEnabled() const { return mDebugEnabled; }

   //! Set the debug state.
   //! @param aDebugEnabled New debug state
   void SetDebugEnabled(bool aDebugEnabled) { mDebugEnabled = aDebugEnabled; }

   //! @name LOS check methods
   //@{

   bool IsTargetVisible(WsfPlatform* aPlatformPtr, WsfPlatform* aTargetPtr, double aMaxRange = 0.0, double aRadiusScale = 1.0);

   bool IsTargetVisible(WsfEM_Antenna* aAntennaPtr, WsfPlatform* aTargetPtr, double aMaxRange = 0.0, double aRadiusScale = 1.0);

   bool IsTargetVisible(WsfEM_Antenna* aAntennaPtr,
                        WsfEM_Antenna* aTargetPtr,
                        double         aMaxRange    = 0.0,
                        double         aRadiusScale = 1.0);
   //@}

   //! @name Terrain masking methods
   //@{

   bool MaskedByTerrain(WsfPlatform* aPlatformPtr, WsfPlatform* aTargetPtr, double aMaxRange = 0.0, double aRadiusScale = 1.0);

   bool MaskedByTerrain(WsfEM_Antenna* aAntennaPtr, WsfPlatform* aTargetPtr, double aMaxRange = 0.0, double aRadiusScale = 1.0);

   bool MaskedByTerrain(WsfEM_Antenna* aAntennaPtr,
                        WsfEM_Antenna* aTargetPtr,
                        double         aMaxRange    = 0.0,
                        double         aRadiusScale = 1.0);

   //@}

   //! @name Vegetation masking methods
   //@{

   bool MaskedByVegetation(WsfPlatform* aPlatformPtr,
                           WsfPlatform* aTargetPtr,
                           double       aMaxRange    = 0.0,
                           double       aRadiusScale = 1.0);

   bool MaskedByVegetation(WsfEM_Antenna* aAntennaPtr,
                           WsfPlatform*   aTargetPtr,
                           double         aMaxRange    = 0.0,
                           double         aRadiusScale = 1.0);

   bool MaskedByVegetation(WsfEM_Antenna* aAntennaPtr,
                           WsfEM_Antenna* aTargetPtr,
                           double         aMaxRange    = 0.0,
                           double         aRadiusScale = 1.0);

   //@}

   //! State data used in LOS calculations
   class State
   {
   public:
      State()
         : mPlatformIndex(0)
         , mTime(-1.0)
         , mMoved(true)
      {
         UtVec3d::Set(mLocationWCS, 0.0);
         UtVec3d::Set(mLLA, 0.0);
      };
      virtual ~State(){};

      size_t mPlatformIndex;  // Platform index
      double mTime;           // Sim time of location data update
      double mLocationWCS[3]; // Location in WCS
      double mLLA[3];         // Latitude, longitude, altitude
      bool   mMoved;          // True if the platform has moved
   };

   typedef std::map<unsigned int, State>           StateMapType;
   typedef std::map<unsigned int, State>::iterator StateMapTypeIterator;
   typedef std::pair<unsigned int, State>          StateMapTypePair;

   //! Reference to the LOS manager state data so worker thread can use it
   StateMapType& GetStateData() { return mStateData; };

   //! Line-of-sight key - a combination of the unique IDs of the two entities.
   class LOS_Key
   {
   public:
      LOS_Key(unsigned int aUniqueId1, unsigned int aUniqueId2);
      virtual ~LOS_Key(){};

      bool operator<(const LOS_Key& aRhs) const;
      bool operator==(const LOS_Key& aRhs) const;
      bool operator==(const unsigned int& aRhs) const;
      bool operator!=(const LOS_Key& aRhs) const;

      unsigned int mID1;
      unsigned int mID2;
   };

   typedef std::map<LOS_Key, bool>           LOS_MapType;
   typedef std::map<LOS_Key, bool>::iterator LOS_MapTypeIterator;
   typedef std::pair<LOS_Key, bool>          LOS_MapTypePair;

   typedef std::list<unsigned int>           LOS_RequestType;
   typedef std::list<unsigned int>::iterator LOS_RequestTypeIterator;

   typedef std::map<unsigned int, unsigned int>           PriorityQueueType;
   typedef std::map<unsigned int, unsigned int>::iterator PriorityQueueTypeIterator;
   typedef std::pair<unsigned int, unsigned int>          PriorityQueueTypePair;

protected:
   //! @name LOS check methods - Non-threaded
   //@{

   bool IsTargetVisibleNow(WsfPlatform* aPlatformPtr,
                           WsfPlatform* aTargetPtr,
                           double       aMaxRange    = 0.0,
                           double       aRadiusScale = 1.0);

   bool IsTargetVisibleNow(WsfEM_Antenna* aAntennaPtr,
                           WsfPlatform*   aTargetPtr,
                           double         aMaxRange    = 0.0,
                           double         aRadiusScale = 1.0);

   bool IsTargetVisibleNow(WsfEM_Antenna* aAntennaPtr,
                           WsfEM_Antenna* aTargetPtr,
                           double         aMaxRange    = 0.0,
                           double         aRadiusScale = 1.0);

   //@}

   //! Returns true if entity's movement is greater than the move tolerance (default is true).
   bool Moved(WsfEM_Antenna* aAntennaPtr);
   bool Moved(WsfPlatform* aPlatformPtr);

   //! Returns true if LOS data exists for the two entities.
   bool LOS_DataExists(/*input*/ unsigned int aID1, unsigned int aID2);
   bool LOS_DataExists(/*input*/ unsigned int aID1, unsigned int aID2, /*output*/ LOS_MapTypeIterator& aIter);
   bool LOS_DataExists(/*input*/ unsigned int          aID1,
                       unsigned int                    aID2,
                       LOS_MapType&                    aLOS_Map,
                       /*output*/ LOS_MapTypeIterator& aIter);

   //! Returns true is state data exists for the entity.
   bool StateDataExists(/*input*/ unsigned int aID);
   bool StateDataExists(/*input*/ unsigned int aID, /*output*/ StateMapTypeIterator& iter);

   //! Returns true if LOS data was successfully updated.
   bool SetLOS_Data(/*input*/ unsigned int aID1, unsigned int aID2, bool aVisible);
   bool SetLOS_Data(/*input*/ unsigned int aID1, unsigned int aID2, bool aVisible, /*output*/ LOS_MapTypeIterator& aIter);

   //! Returns true if state data was successfully updated.
   bool SetStateData(/*input*/ unsigned int aID, State& aNewState);
   bool SetStateData(/*input*/ unsigned int aID, State& aNewState, /*output*/ StateMapTypeIterator& aIter);

   //! Removes all map entries from the given map with a LOS_Key containing the given ID.
   void DeleteLOS_Data(unsigned int aID, LOS_MapType& aLOS_Map);

   //! Used in multi-threaded mode; updates the platform and part state data
   void UpdateStateData(/*output*/ LOS_RequestType& aLOS_RequestQueue);

   //! @name Platform and part move check methods
   //@{

   bool PlatformMoved(WsfPlatform* aPlatformPtr);
   bool PlatformMoveCheck(WsfPlatform* aPlatformPtr, StateMapTypeIterator& aIter);

   bool PartMoved(WsfArticulatedPart* aPartPtr);
   bool PartMoveCheck(WsfArticulatedPart* aPartPtr, StateMapTypeIterator& aIter);

   //@}

private:
   //! Copy Constructor
   WsfLOS_Manager(const WsfLOS_Manager& aSrc);

   //! Prevent use of operator= by declaring, but not defining.
   WsfLOS_Manager& operator=(const WsfLOS_Manager& aRhs) = delete;

   //! LOS manager implementation of worker thread
   class LOS_Thread : public WsfThread, public WsfLOS_ManagerData::ThreadData
   {
   public:
      //! Constructor and destructor
      LOS_Thread(WsfLOS_ManagerData::ThreadData& aData);
      ~LOS_Thread() override{};

      FunctionType DoWork() override;
      FunctionType Pause() override { return AVAILABLE; };

      //! Initializes the priority queues based on the value in mNumPriorityQueues
      void Initialize(WsfLOS_Manager* aManagerPtr);

      typedef std::set<unsigned int>           IdSetType;
      typedef std::set<unsigned int>::iterator IdSetTypeIterator;

      //! Process all requests in the request queue; results in updated priority queues
      void ProcessRequests();

      //! Processes the priority queue by returning the highest priority requests; returns 0 to mProcessingRate requests
      void ProcessPriorityQueue(/*output*/ IdSetType& aIdSet);

      //! Updates the LOS data for the highest priority requests
      void ProcessThreadLOSData(/*input*/ IdSetType& aIdSet);

      //! Processes a request by entering it into a priority queue
      //! Currently all requests are processed at a given time step i.e. mProcessingRate is not used here
      void ProcessRequest(unsigned int aID);

      //! Removes the entry with the given ID from the priority queues
      void CleanUpPriorityQueues(unsigned int aID);

      //! Set the data in the thread's LOS data container
      bool SetThreadLOS_Data(LOS_Key aLOS_Key, bool aVisible);

      //! Returns true if priority queues are empty
      bool NoWork();

      WsfLOS_Manager* mManagerPtr;
      double          mTime;         //! Sim time of requests
      LOS_RequestType mRequestQueue; //! List of platforms or parts that have moved

      IdSetType mIdsToDelete; //! Platforms and part IDS deleted from simulation

      std::vector<PriorityQueueType> mPriorityQueues;


      LOS_MapType mThreadLOS_Data; //! Thread data; copied to LOS manager at processing rate
   };

   //! LOS simulation time
   double mLOS_Time;

   //! LOS data map; Key = Composite of unique IDs of endpoints; Value = isVisible boolean
   LOS_MapType mLOS_Data;

   //! State of entity when LOS check occurred; Key = unique ID of entity; Value = state vector in WCS
   StateMapType mStateData;

   //! Call back holder
   UtCallbackHolder mCallbacks;

   //! Worker thread
   LOS_Thread mWorkerThread;

   //! Mutex for when running simulation multi-threaded but not the LOS manager
   mutable std::recursive_mutex mMutex;
};

#endif
