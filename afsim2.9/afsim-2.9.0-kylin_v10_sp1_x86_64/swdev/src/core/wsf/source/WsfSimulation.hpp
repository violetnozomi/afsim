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

#ifndef WSFSIMULATION_HPP
#define WSFSIMULATION_HPP

#include "wsf_export.h"

#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <vector>

class UtAtmosphere;
class UtInput;
#include "UtMemory.hpp"
#include "UtRandom.hpp"
class UtScriptClass;
#include "UtScriptExecutor.hpp"
class UtScriptTypes;
#include "UtWallClock.hpp"
#include "WsfAdvancedBehaviorObserver.hpp"
class WsfApplication;
#include "WsfBehaviorObserver.hpp"
#include "WsfClockSource.hpp"
#include "WsfCommObserver.hpp"
#include "WsfDateTime.hpp"
#include "WsfDisObserver.hpp"
#include "WsfEM_Manager.hpp"
class WsfEnvironment;
#include "WsfEvent.hpp"
#include "WsfEventManager.hpp"
#include "WsfException.hpp"
#include "WsfExchangeObserver.hpp"
#include "WsfExtensionList.hpp"
#include "WsfFuelObserver.hpp"
#include "WsfGroupManager.hpp"
class WsfIFF_Manager;
class WsfLOS_Manager;
class WsfMover;
#include "WsfMoverObserver.hpp"
#include "WsfMultiThreadManager.hpp"
class WsfPathFinderList;
class WsfPlatform;
#include "WsfPlatformObserver.hpp"
class WsfPlatformPart;
#include "WsfPlatformPartObserver.hpp"
class WsfProcessor;
#include "WsfProcessorObserver.hpp"
class WsfRealTimeClockSource;
class WsfRoute;
class WsfScenario;
#include "WsfScriptStateMachineObserver.hpp"
#include "script/WsfScriptContext.hpp"
class WsfSensor;
#include "WsfSensorObserver.hpp"
class WsfSimulationExtension;
class WsfSimulationInput;
#include "WsfSimulationObserver.hpp"
#include "WsfStringId.hpp"
class WsfSystemLog;
#include "WsfTaskObserver.hpp"
#include "WsfTrackObserver.hpp"
#include "WsfTypes.hpp"
class WsfZone;
#include "WsfZoneAttenuation.hpp"
#include "WsfZoneObserver.hpp"

//! Temporary namespace pre-declaration for NetworkManager
//! Needed until namespacing is implemented
namespace wsf
{
	class TerrainInterface;
	namespace comm
	{
		class Comm;
		class NetworkManager;
	} // namespace comm
} // namespace wsf

/** The main controller for a simulation.

    The 'simulation' object acts as the main controller for a simulation.
    While it is possible to use the framework without a simulation object, the simulation
    object will almost always be required for all but the most trivial of simulations.

    The simulation object provides methods to:

    \li Maintain an event queue
    \li Supervise the addition and deletion of platforms
    \li Turn platform systems on and off in a coordinated manner.
    \li Maintain simulation 'observers'

    Most methods within the class are virtual so a derived class may be used to provide custom
    simulation functionality.
*/

class WSF_EXPORT WsfSimulation
{
public:
	//! An enumerated type specifying the state of the simulation.
	enum State
	{
		cPENDING_INITIALIZE, //!< The simulation has been constructed and is ready for WsfSimulation::Initialize to be called.
		cINITIALIZING,       //!< The WsfSimulation::Initialize method is being called.
		cPENDING_START,      //!< Initialization is complete, ready for WsfSimulation::Start to be called.
		cSTARTING,           //!< The WsfSimulation::Start method is being called.
		cACTIVE,             //!< Start is complete, the simulation is in progress.
		cPENDING_COMPLETE, //!< Simulation processing is complete; waiting on a call to WsfSimulation::Complete
		cCOMPLETE          //!< Simulation is complete.
	};

	//! An enumerated type specifying the reason for completion of the simulation.
	enum CompletionReason
	{
		cNONE,             //!< No reason; the simulation is not in a PENDING_COMPLETE or COMPLETE state.
		cEND_TIME_REACHED, //!< The simulation has reached the specified end time.
		cRESET,            //!< A reset was commanded through a call to WsfSimulation::Reset.
		cTERMINATE,        //!< A terminate was commanded through a call to WsfSimulation::Terminate.
		cOTHER             //!< Other reason that can be used by derived classes.
	};

	//! An exception that is thrown when Initialize() fails.
	class WSF_EXPORT InitializeError : public WsfException
	{
	public:
		InitializeError();
	};

	//! An exception that is thrown when Start() fails.
	class WSF_EXPORT StartError : public WsfException
	{
	public:
		StartError();
	};

	//! An exception that is thrown when the user tries to create a simulation
	//! incorrectly.  Usually this occurs when WsfScenario::CompleteLoad() isn't called
	//! prior to creating the simulation.
	class CreateError : public WsfException
	{
	public:
		CreateError();
	};

	//! An internal event scheduled to add a platform at the specified time.

	class WSF_EXPORT AddPlatformEvent : public WsfEvent
	{
	public:
		AddPlatformEvent(double aSimTime, WsfPlatform* aPlatformPtr);

		~AddPlatformEvent() override;

		EventDisposition Execute() override;

	private:
		WsfPlatform* mPlatformPtr;
	};

	WsfSimulation(const WsfScenario& aScenario, unsigned int aRunNumber);
	virtual ~WsfSimulation();
	WsfSimulation(const WsfSimulation& aSrc) = delete;
	WsfSimulation& operator=(const WsfSimulation& aRhs) = delete;

	const WsfScenario& GetScenario() const { return mScenario; }
	WsfApplication&    GetApplication() const;

	//! @name Simulation State and Time management
	//@{
	virtual void Initialize();

	virtual void PrepareSimulation();

	virtual void AddEvent(std::unique_ptr<WsfEvent> aEventPtr);
	// A helper function to construct and queue an event, returning a non-owning reference to the event
	template<typename T, typename... Args>
	T* AddEventT(Args&&... args)
	{
		static_assert(std::is_base_of<WsfEvent, T>::value, "Template type argument must derive from WsfEvent");
		auto eventPtr = ut::make_unique<T>(std::forward<Args>(args)...);
		auto returnPtr = eventPtr.get();
		AddEvent(std::move(eventPtr));
		return returnPtr;
	}

	virtual void AddWallEvent(std::unique_ptr<WsfEvent> aEventPtr);
	// A helper function to construct and queue a wall event, returning a non-owning reference to the event
	template<typename T, typename... Args>
	T* AddWallEventT(Args&&... args)
	{
		static_assert(std::is_base_of<WsfEvent, T>::value, "Template type argument must derive from WsfEvent");
		auto eventPtr = ut::make_unique<T>(std::forward<Args>(args)...);
		auto returnPtr = eventPtr.get();
		AddWallEvent(std::move(eventPtr));
		return returnPtr;
	}

	virtual double AdvanceTime();

	virtual double AdvanceTime(double aSimTime);

	virtual bool ShouldExecute() const;

	virtual void Start();

	virtual void Pause();

	void PauseAndRequestAdvance(double);

	virtual void Resume();

	virtual void Complete(double aSimTime);

	virtual void RequestReset();

	virtual void WaitForAdvanceTime();

	//! Return whether the simulation currently is in the ACTIVE state (see WsfSimulation::State for a
	//! list of simulation states).
	//! @note This method is used to conditionally either advance the simulation time or trigger a call
	//! to WsfSimulation::Complete, as follows:
	//!
	//! \code
	//! // Code Fragment illustrating the IsActive() method in a simulation main loop.
	//! while (sim.IsActive())
	//! {
	//!    sim.AdvanceTime();
	//! }
	//! sim.Complete(sim.GetEndTime());
	//! \endcode
	bool IsActive() const { return (mState == cACTIVE); }

	//! Return the simulation's current state.
	State GetState() const { return mState; }

	//! Return the string representation of the simulation's current state.
	const std::string& GetStateString() const;

	//! Return the string representation of the provided simulation state.
	static const std::string& GetStateString(State aState);

	//! Return the simulation's reason for completion.
	//! If the simulation has not actually completed, the returned value will be cNONE
	CompletionReason GetCompletionReason() const { return mCompletionReason; }

	virtual std::string GetCompletionReasonString() const;

	virtual void RequestTermination();

	//! Set the defined end time of the simulation.
	void SetEndTime(double aEndTime);

	//! Get the last time to which the simulation was updated.
	double GetSimTime() const { return mSimTime; }

	//! Get the defined end time of the simulation.
	double GetEndTime() const { return mEndTime; }

	virtual double GetNextEventTime() const;

	//! Get the last sampled real-time clock time.
	//!
	//! @note This value may not be maintained for non-realtime simulations.
	double GetRealTime() const { return mRealTime; }

	//! Return whether this simulation is running real-time, or a multiple of real-time.
	bool IsRealTime() const { return mIsRealTime; }

	//! Get the current wall-clock time since simulation creation.
	double GetWallTime() const;

	//! Get the amount of time the simulation clock is behind real-time clock.
	//!
	//! A derived class may optionally maintain this value for use as a hint to other simulation components
	//! that the simulation clock has fallen 'sufficiently' behind the real-time clock and that they should
	//! take action to try to reduce the workload. The default value is zero (not running behind).
	double GetTimeBehind() const { return mTimeBehind; }

	//! Return the simulation timestep (if the simulation is time-stepped).
	//! @returns The simulation timestep for time-stepped simulation or 0 for an event driven simulation.
	double GetTimestep() const { return mTimestep; }

	//! Return the scaled and stepped DIS-NRT timestep (if running DIS-NRT).
	//! @returns the current timestep in a scaled-and-stepped simulation.
	double GetSyncTimestep() const { return mSyncTimestep; }

	//! Instructs the simulation to run in a real-time mode.
	virtual void SetRealtime(double aSimTime, bool aIsRealTime) {}

	void SetIsExternallyStarted(bool aIsExternallyStarted);

	//! Return whether the simulation is to be started by an external source.
	bool IsExternallyStarted() const { return mIsExternallyStarted; }

	//! Return whether the simulation multi-threading is currently active.
	bool MultiThreadingActive() const { return mMultiThreadingActive; }

	//! Set whether multi-threading is currently active ('true') or not ('false').
	void SetMultiThreadingActive(bool aMultiThreadingActive) { mMultiThreadingActive = aMultiThreadingActive; }

	//! Return event step simulation indicator
	bool IsEventStepSimulation() const { return mAmAnEventStepSimulation; }

	void SetAmAnEventStepSimulation(bool aFlag) { mAmAnEventStepSimulation = aFlag; }

	void SetClockSource(std::unique_ptr<WsfClockSource> aClockSourcePtr);

	//! Return the current clock source object.
	//! @returns Pointer to the current clock source object.
	const WsfClockSource* GetClockSource() const { return mClockSourcePtr.get(); }

	virtual double GetClockRate() const;

	virtual void SetClockRate(double aClockRate);

	virtual void
		SetTimeParameters(int aTimeScheme, double aSimTime, double aClockRate, double aTimeStep, bool aTimeAdvance = false);
	//@}

	//! @name Platform addition, deletion and query methods.
	//@{
	virtual bool AddPlatform(WsfPlatform* aPlatformPtr);

	virtual bool AddPlatform(double aSimTime, WsfPlatform* aPlatformPtr);

	// void AssignPlatformToSimulation(WsfPlatform* aPlatformPtr);

	// Remove the Platform from the Simulation and cleans up the memory if aDeleteMemory is true
	virtual void DeletePlatform(double aSimTime, WsfPlatform* aPlatformPtr, bool aDeleteMemory = true);

	WsfPlatform* GetPlatformByIndex(size_t aIndex) const;

	WsfPlatform* GetPlatformByName(WsfStringId aNameId) const;

	bool PlatformExists(size_t aIndex) const;

	WsfStringId GetPlatformNameId(size_t aIndex) const;

	WsfStringId GetPlatformTypeId(size_t aIndex) const;
	//@}

	//! @name Platform list iteration methods
	//! The following methods can be used to iterate over the list of
	//! known platforms.  For example:
	//!
	//! \code
	//! for (size_t i = 0; i < simulationPtr->GetPlatformCount(); ++i)
	//! {
	//!    WsfPlatform* platformPtr = simulationPtr->GetPlatformEntry(i);
	//!    ... blah blah blah ...
	//! }
	//! \endcode
	//@{
	size_t GetPlatformCount() const;

	WsfPlatform* GetPlatformEntry(size_t aEntry) const;
	//@}

	//! @name Subsystem status change methods
	//@{
	virtual bool TurnPartOff(double aSimTime, WsfPlatformPart* aPartPtr) { return TurnPartOffP(aSimTime, aPartPtr); }

	virtual bool TurnPartOn(double aSimTime, WsfPlatformPart* aPartPtr) { return TurnPartOnP(aSimTime, aPartPtr); }

	virtual bool SetPartOperational(double aSimTime, WsfPlatformPart* aPartPtr, bool aOperational = true)
	{
		return SetPartOperationalP(aSimTime, aPartPtr, aOperational);
	}

	virtual void SetPartUpdateInterval(double aSimTime, WsfPlatformPart* aPartPtr, double aUpdateInterval);
	//@}

	//! @name Methods that are public, but aren't part of the public interface.
	//@{
	virtual bool PlatformDeleted(double aSimTime, WsfPlatform* aOldPlatformPtr);

	virtual void ProcessRemovePlatformEvent(double aSimTime, WsfPlatform* aPlatformPtr, bool aDeletePlatform);

	virtual void ProcessPlatformBrokenEvent(double aSimTime, WsfPlatform* aPlatformPtr);
	//@}

	//! @name Deprecated methods.
	//@{
	//! Deprecated form of Start();
	virtual void SimulationStarting() { Start(); }
	//! Deprecated form of Complete();
	virtual void SimulationComplete(double aSimTime) { Complete(aSimTime); }
	//@}

	//! Return 'true' if the simulation's real-time constraint is flexible.
	bool IsFlexibleRealtime() const { return mIsFlexibleRealtime; }

	void SetFlexibleRealtime() { mIsFlexibleRealtime = true; }

	//! @name Observer access methods.
	//@{
	WsfAdvancedBehaviorObserver&   GetAdvancedBehaviorObserver() const { return mAdvancedBehaviorObserver; }
	WsfBehaviorObserver&           GetBehaviorObserver() const { return mBehaviorObserver; }
	WsfCommObserver&               GetCommObserver() const { return mCommObserver; }
	WsfDisObserver&                GetDisObserver() const { return mDisObserver; }
	WsfExchangeObserver&           GetExchangeObserver() const { return mExchangeObserver; }
	WsfFuelObserver&               GetFuelObserver() const { return mFuelObserver; }
	WsfMoverObserver&              GetMoverObserver() const { return mMoverObserver; }
	WsfPlatformObserver&           GetPlatformObserver() const { return mPlatformObserver; }
	WsfPlatformPartObserver&       GetPlatformPartObserver() const { return mPlatformPartObserver; }
	WsfProcessorObserver&          GetProcessorObserver() const { return mProcessorObserver; }
	WsfScriptStateMachineObserver& GetScriptStateMachineObserver() const { return mScriptStateMachineObserver; }
	WsfSensorObserver&             GetSensorObserver() const { return mSensorObserver; }
	WsfSimulationObserver&         GetSimulationObserver() const { return mSimulationObserver; }
	WsfTaskObserver&               GetTaskObserver() const { return mTaskObserver; }
	WsfTrackObserver&              GetTrackObserver() const { return mTrackObserver; }
	WsfZoneObserver&               GetZoneObserver() const { return mZoneObserver; }
	//@}

	//! @name Simulation extension methods.
	//@{
	WsfSimulationExtension* FindExtension(const std::string& aName) const;
	WsfSimulationExtension& GetExtension(const std::string& aName) const;
	const WsfExtensionList& GetExtensions() const { return mExtensionList; }

	void RegisterExtension(const std::string& aName, std::unique_ptr<WsfSimulationExtension> aExtensionPtr);
	const std::vector<std::string>& GetExtensionNames() const;
	//@}

	unsigned int AssignUniqueId();

	ut::Random& GetRandom();
	ut::Random& LockRandom();
	void        UnlockRandom();

	ut::Random& GetScriptRandom();
	ut::Random& LockScriptRandom();
	void        UnlockScriptRandom();

	wsf::TerrainInterface*     GetTerrainInterface() const;
	WsfDateTime&               GetDateTime() { return mDateTime; }
	const WsfDateTime&         GetDateTime() const { return mDateTime; }
	WsfGroupManager&           GetGroupManager() { return mGroupManager; }
	wsf::comm::NetworkManager* GetCommNetworkManager() const { return mCommNetworkManagerPtr; }
	WsfMultiThreadManager&     GetMultiThreadManager() { return mMultiThreadManager; }
	WsfPathFinderList&         GetPathFinderList() const { return mPathFinderList; }
	WsfScriptContext&          GetScriptContext() { return mGlobalContext; }
	const WsfScriptContext&    GetScriptContext() const { return mGlobalContext; }
	WsfLOS_Manager*            GetLOS_Manager() const { return mLOS_ManagerPtr; }
	WsfZoneAttenuation&        GetZoneAttenuation() { return mZoneAttenuation; }
	WsfEM_Manager&             GetEM_Manager() { return mEM_Manager; }
	UtScriptExecutor&          GetScriptExecutor() { return mScriptExecutor; }
	UtAtmosphere&              GetAtmosphere() const;
	WsfEnvironment&            GetEnvironment() const;
	WsfSystemLog&              GetSystemLog() const;
	WsfIFF_Manager*            GetIFF_Manager() const;

	unsigned int NextMessageSerialNumber() { return ++mNextMessageSerialNumber; }
	unsigned int NextEngagementSerialNumber() { return ++mNextEngagementSerialNumber; }
	unsigned int PreviousEngagementSerialNumber() const { return mNextEngagementSerialNumber; }
	void         ReclaimPreviousEngagementSerialNumber() { --mNextEngagementSerialNumber; }
	double       CreateMessageDataTag();

	//! Return whether the simulation should multi-thread were appropriate.
	bool MultiThreaded() const { return mMultiThreaded; }

	//! Get the current run number.
	unsigned int GetRunNumber() const { return mRunNumber; }

	unsigned int GetFinalRunNumber() const;

	bool RandomizeFrequency() const;

	bool   UseDefaultFrequency() const;
	double GetMinimumMoverTimestep() const { return mMinimumMoverTimestep; }

	const WsfSimulationInput& GetSimulationInput() const { return mSimulationInput; }

	std::string SubstituteOutputFileVariables(const std::string& aOutputFile);

protected:
	//! @name Platform list maintenance methods.
	//@{
	void AddToPlatformList(WsfPlatform* aPlatformPtr);

	bool DeleteFromPlatformList(WsfPlatform* aPlatformPtr);

	bool PlatformPointerExists(WsfPlatform* aPlatformPtr);

	void ResetPlatformList();

	//! @name AddInputPlatforms
	//! Non-public method for adding platforms as part of the standard simulation initialization
	//! routine. During runtime, platforms are added individually using the AddPlatform methods
	//! that are publicly available as part of the WsfSimulation interface.
	bool AddInputPlatforms();
	//@}

	virtual bool TurnPartOffP(double aSimTime, WsfPlatformPart* aPartPtr);

	virtual bool TurnPartOnP(double aSimTime, WsfPlatformPart* aPartPtr);

	virtual bool SetPartOperationalP(double aSimTime, WsfPlatformPart* aPartPtr, bool aOperational);

	virtual void DispatchWallEvents();
	virtual void DispatchSimEvents(double aSimTime);
	virtual void DispatchEvents(double aSimTime);

	virtual bool PlatformInitialized(double aSimTime, WsfPlatform* aNewPlatformPtr);

	virtual bool PlatformIntroduced(double aSimTime, WsfPlatform* aNewPlatformPtr);


	virtual bool SimulationInitialized();

	virtual void CreateClock();

	virtual bool PlatformIsAvailable(WsfPlatform* aPlatformPtr);

	//! EM manager
	WsfEM_Manager mEM_Manager;

	//! Simulation event manager object.
	WsfEventManager mEventManager;

	//! Wall clock-based event manager object.
	WsfEventManager mWallEventManager;

	//! Pointer to the simulation clock source object.
	std::unique_ptr<WsfClockSource> mClockSourcePtr{ nullptr };

	//! If mClockSourcePtr is a WsfRealTimeClockSource, points to mClockSourcePtr,
	//! otherwise it is nullptr.
	WsfRealTimeClockSource* mRealTimeClockSourcePtr{ nullptr };

	//! The last time to which the simulation has been updated (seconds)
	double mSimTime{ 0.0 };

	//! The last sampled real-time clock value (seconds)
	//! See GetRealTime() for more information.
	double mRealTime{ false };

	//! The amount of time the simulation clock is behind the real-time clock.
	double mTimeBehind{ 0.0 };

	//! The timestep or frame time for a fixed timestep simulation.
	//! For event-driven simulations this will be zero (the default value).
	//! For time-stepped simulations, the derived class must set this to their timestep.
	double mTimestep{ 0.0 };

	//! The wall clock time taken to execute the current synchronized time step
	double mSyncAccumulatedTime{ 0.0 };

	//! The current state of the simulation.
	volatile State mState{ cPENDING_INITIALIZE };

	//! The reason for simulation completion.
	volatile CompletionReason mCompletionReason{ cNONE };

	//! True if an external event needs to be processed.
	bool mExternalEventReady{ false };

	//! The current run number being executed.
	unsigned int mRunNumber;

	const WsfSimulationInput& mSimulationInput;

	WsfDateTime mDateTime;

	bool mMultiThreaded;

	double mMinimumMoverTimestep;

	//! The end time of the simulation (seconds)
	double mEndTime{ std::numeric_limits<double>::max() };

	bool mIsRealTime{ false };

private:
	void AssignDefaultName(WsfPlatform* aPlatformPtr);

	bool IntroducePlatform(double aSimTime, WsfPlatform* aNewPlatformPtr);

	void TurnOnSystems(double aSimTime, WsfPlatform* aPlatformPtr);

	void HandlePlatformInitializationFailure(WsfPlatform* aPlatformPtr);

	const WsfScenario& mScenario;

	//! The persistent list of platform name ID's indexed by platform index.
	std::vector<WsfStringId> mPlatformNameIds;

	//! The persistent list of platform type ID's indexed by platform index.
	std::vector<WsfStringId> mPlatformTypeIds;

	//! A real-world clock
	UtWallClock mWallClock;

	//! The length of current time step in a stepped and scaled simulation
	double mSyncTimestep{ 0.0 };

	//! The ratio of simulation time to real world time
	double mClockRate;

	double mNextMessageDataTag{ 1.0 };

	unsigned int mNextMessageSerialNumber{ 0 };

	unsigned int mNextEngagementSerialNumber{ 0 };

	std::recursive_mutex mUniqueIdMutex;
	unsigned int         mUniqueId{ 0 };

	//! Provides the source of randomness for the core simulation models.
	//! All random draws from simulation models should either come directly from this object, or be seeded by this object.
	ut::Random           mRandom;
	std::recursive_mutex mRandomMutex;

	//! Provides the source of randomness in user-defined scripts.
	//! This is separated from the other UtRandom object so that scripts do not pull random numbers
	//! from the same pool, thereby affecting the simulation.
	ut::Random           mScriptRandom;
	std::recursive_mutex mScriptRandomMutex;

	//! Whether an external object starts the simulation.
	bool mIsExternallyStarted{ false };

	//! Current multi-threading indicator flag
	bool mMultiThreadingActive{ false };

	//! Event simulation indicator
	bool mAmAnEventStepSimulation{ true };

	//! If this is true, it indicates the simulation does not need to maintain hard real-time.
	//! This is primarily used to make the simulation advance as expected while debugging.
	bool mIsFlexibleRealtime{ false };

	//! Maps the platform type to the next suffix number for generating a default platform name
	std::map<WsfStringId, int> mNextDefaultNameNumber;

	//! Observer objects are mutable because they can be modified (i.e. changing the list of connected
	//! subscribers) without changing the logical state of the simulation. Therefore a mutable reference
	//! may be returned even for a const WsfSimulation.
	mutable WsfAdvancedBehaviorObserver   mAdvancedBehaviorObserver;
	mutable WsfBehaviorObserver           mBehaviorObserver;
	mutable WsfCommObserver               mCommObserver;
	mutable WsfDisObserver                mDisObserver;
	mutable WsfExchangeObserver           mExchangeObserver;
	mutable WsfFuelObserver               mFuelObserver;
	mutable WsfMoverObserver              mMoverObserver;
	mutable WsfPlatformObserver           mPlatformObserver;
	mutable WsfPlatformPartObserver       mPlatformPartObserver;
	mutable WsfProcessorObserver          mProcessorObserver;
	mutable WsfScriptStateMachineObserver mScriptStateMachineObserver;
	mutable WsfSensorObserver             mSensorObserver;
	mutable WsfSimulationObserver         mSimulationObserver;
	mutable WsfTaskObserver               mTaskObserver;
	mutable WsfTrackObserver              mTrackObserver;
	mutable WsfZoneObserver               mZoneObserver;

	wsf::comm::NetworkManager* mCommNetworkManagerPtr{ nullptr };
	WsfGroupManager            mGroupManager;
	WsfLOS_Manager*            mLOS_ManagerPtr{ nullptr };
	WsfMultiThreadManager      mMultiThreadManager;
	WsfPathFinderList&         mPathFinderList;
	WsfZoneAttenuation         mZoneAttenuation;

	UtScriptExecutor mScriptExecutor;

	//! The global script context.  Contains all top-level scripts and script variables.
	WsfScriptContext mGlobalContext;

	//! The list of platforms that are active in the simulation.
	std::vector<WsfPlatform*> mPlatforms;

	//! The list of platforms indexed by platform index.
	//! Platforms that have been deleted are contain a null pointer.
	std::vector<WsfPlatform*> mPlatformsByIndex;

	//! The list of platforms indexed by platform name(ID).
	std::map<WsfStringId, WsfPlatform*> mPlatformsByName;

	// Records the creation order of extensions
	WsfExtensionList mExtensionList;
};

// Helper macro for observer objects to implement their callback accessors
#define WSF_OBSERVER_CALLBACK_DEFINE(OBSERVER, EVENT)                                    \
   WsfObserver::EVENT##Callback& WsfObserver::EVENT(const WsfSimulation* aSimulationPtr) \
   {                                                                                     \
      return aSimulationPtr->Get##OBSERVER##Observer().EVENT;                            \
   }

#endif
