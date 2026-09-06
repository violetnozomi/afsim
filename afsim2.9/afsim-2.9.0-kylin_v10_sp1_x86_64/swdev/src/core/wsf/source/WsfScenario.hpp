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

#ifndef WSFSCENARIO_HPP
#define WSFSCENARIO_HPP

#include <functional>
#include <map>
#include <memory>
#include <set>
#include <vector>

#include "UtAtmosphere.hpp"
#include "UtCallback.hpp"
#include "UtCast.hpp"
#include "UtColor.hpp"
class UtInput;
#include "UtScriptEnvironment.hpp"
class UtScriptTypes;
class WsfApplication;
#include "WsfComponentFactory.hpp"
#include "WsfDeferredInput.hpp"
#include "WsfEnvironment.hpp"
#include "WsfExtensionList.hpp"
class WsfGrammarInterface;
#include "WsfIFF_Manager.hpp"
#include "WsfMessageTable.hpp"
class WsfObject;
#include "WsfPathFinder.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformAvailability.hpp"
class WsfScenarioExtension;
class WsfScriptContext;
#include "WsfSignatureList.hpp"
#include "script/WsfScriptManager.hpp"
class WsfSimulation;
#include "WsfSimulationInput.hpp"
#include "WsfStringTable.hpp"
class WsfSystemLog;

// Types classes
class WsfAdvancedBehaviorTreeNodeTypes;
class WsfAeroTypes;
class WsfAntennaPatternTypes;
class WsfBehaviorTreeNodeTypes;
class WsfCallbackTypes;
class WsfCorrelationStrategyTypes;
class WsfEM_AttenuationTypes;
class WsfEM_AttenuationTypes;
class WsfEM_ClutterTypes;
class WsfEM_PropagationTypes;
class WsfFilterTypes;
class WsfFuelTypes;
class WsfFusionStrategyTypes;
class WsfIntersectMeshTypes;
class WsfGroupTypes;
class WsfMaskingPatternTypes;
class WsfMoverTypes;
class WsfNoiseCloudTypes;
class WsfObjectTypeListBase;
class WsfObjectTypeListBaseI;
class WsfPathFinderTypes;
class WsfPlatformTypes;
class WsfProcessorTypes;
class WsfRadarSignatureTypes;
class WsfRouteNetworkTypes;
class WsfRouteTypes;
class WsfSensorErrorModelTypes;
class WsfSensorTypes;
#include "WsfTerrain.hpp"
class WsfThermalSystemTypes;
class WsfThermalSystemComponentTypes;
class WsfTrackReportingStrategyTypes;
class WsfVisualPartTypes;
class WsfZoneTypes;

namespace wsf
{

class TrackExtrapolationStrategyTypes;

namespace comm
{
class Types;
class NetworkTypes;
class ProtocolTypes;

namespace router
{
class Types;
class ProtocolTypes;
} // namespace router

namespace medium
{
class Types;
}
} // namespace comm
} // namespace wsf

//! Contains the data required to create a simulation, and acts as the entry point for input file processing.
class WSF_EXPORT WsfScenario
{
public:
	using ScenarioExtensionMap = std::map<std::string, WsfScenarioExtension*>;
	using ComponentFactoryList = std::vector<std::unique_ptr<WsfComponentFactoryBase>>;
	using InputFunction = std::function<bool(UtInput&)>;

	static WsfScenario& FromInput(UtInput& aInput);

	WsfScenario(WsfApplication& aApplication);
	WsfScenario(const WsfScenario& aSrc) = delete;
	WsfScenario& operator=(const WsfScenario&) = delete;
	virtual ~WsfScenario() = default;

	void SetSimulationInput(WsfSimulationInput* aSimulationInput);

	//! Return a reference to the application to which this scenario is associated.
	WsfApplication& GetApplication() const { return *mApplicationPtr; }

	void LoadFromFile(const std::string& aFileName);
	void LoadFromStream(UtInput& aInput);
	bool ProcessInput(UtInput& aInput);

	void CompleteLoad();

	void ConfigureInput(UtInput& aInput);
	bool LoadIsComplete() const { return mLoadIsComplete; }

	//! To be called before modification of the scenario.
	//! If the scenario is in an immutable state, throws an exception
	void ScenarioChange();

	bool LoadPlatformInstance(UtInput& aInput);

	void SimulationCreated(WsfSimulation& aSimulation) const;

	//! @name Access to miscellaneous objects maintained by the scenario.
	//@{
	WsfDeferredInput& GetDeferredInput();

	WsfSystemLog&  GetSystemLog() const;
	UtScriptTypes* GetScriptTypes() const { return mScriptTypesPtr; }

	WsfScriptManager* GetScriptManager() const { return mScriptManagerPtr.get(); }

	WsfScriptContext* GetScriptContext() const;

	WsfEnvironment& GetEnvironment() const { return *mEnvironmentPtr; }

	// Should not be modified by the simulation
	const WsfMessageTable* GetMessageTable() const { return mMessageTablePtr.get(); }

	UtAtmosphere& GetAtmosphere() const { return *mAtmospherePtr; }

	WsfPathFinderList& GetPathFinderList() const { return *mPathFinderListPtr; }

	WsfSimulationInput& GetSimulationInput() const { return *mSimulationInputPtr; }

	//! Valid only after WsfScenario::CompleteLoad()
	wsf::TerrainInterface* GetTerrainInterface() const { return mTerrainInterfacePtr.get(); }

	//! Returns the IFF manager.  The IFF manager is shared by multiple simulations, and must be const after CompleteLoad()
	WsfIFF_Manager* GetIFF_Manager() const { return mIFF_ManagerPtr.get(); }

	const WsfStringTable& Strings() const { return mStringTable; }

	//! Returns the input files given to LoadFromFile()
	const std::vector<std::string>& GetInputFiles() const { return mInputFiles; }

	void AddInputProcessor(const InputFunction& aFunction);

	UtScriptEnvironment& GetScriptEnvironment() const { return *mScriptEnvironmentPtr; }

	//! Return the input object used to read scenario inputs
	UtInput& GetInput() const { return *mInputPtr; }
	//@}

	//! @name Scenario extension registration and query methods.
	//@{
	WsfScenarioExtension* FindExtension(const std::string& aName) const;
	WsfScenarioExtension& GetExtension(const std::string& aName) const;

	void RegisterExtension(const std::string& aName, std::unique_ptr<WsfScenarioExtension> aExtensionPtr);
	void ExtensionDepends(const std::string& aExtensionName, const std::string& aDependsOnExtensionName, bool aRequired);

	const WsfExtensionList& GetExtensions() const { return *mExtensionListPtr; }
	//@}

	//! @name Component factory registration and access.
	//@{
	void                        RegisterComponentFactory(std::unique_ptr<WsfComponentFactoryBase> aFactoryPtr);
	const ComponentFactoryList& GetComponentFactoryList() const { return mComponentFactoryList; }
	//@}

	//! @name Methods to find and clone objects within the type lists.
	//@{
	WsfObject* FindType(const std::string& aTypeKind, WsfStringId aTypeName) const;

	//! A typesafe version of FindType.
	//! @param aTypeName The name of the type object to be located.
	template<typename T>
	T* FindTypeT(const std::string& aTypeName)
	{
		WsfObject* objectPtr = FindType(T::cTYPE_KIND, aTypeName);
		return dynamic_cast<T*>(objectPtr);
	}

	WsfObject* CloneType(const std::string& aTypeKind, WsfStringId aTypeName) const;

	//! A typesafe version of CloneType.
	//! @param aTypeName The name of the type object to be located.
	template<typename T>
	T* CloneTypeT(WsfStringId aTypeName) const
	{
		WsfObject* objectPtr = CloneType(T::cTYPE_KIND, aTypeName);
		return dynamic_cast<T*>(objectPtr);
	}
	//@}

	void AddTypeList(std::unique_ptr<WsfObjectTypeListBaseI> aTypeListPtr);

	WsfObjectTypeListBaseI*              GetTypes(const std::string& aTypesKind) const;
	std::vector<WsfObjectTypeListBaseI*> GetTypeLists() const
	{
		std::vector<WsfObjectTypeListBaseI*> result;
		for (auto& typeListPtr : mAllTypeLists)
		{
			result.push_back(typeListPtr.get());
		}
		return result;
	}

	//! @name Access to known type lists.
	//@{
	WsfAdvancedBehaviorTreeNodeTypes& GetAdvancedBehaviorTreeNodeTypes() const
	{
		return *mAdvancedBehaviorTreeNodeTypesPtr;
	}
	WsfAeroTypes&                     GetAeroTypes() const { return *mAeroTypesPtr; }
	WsfAntennaPatternTypes&           GetAntennaPatternTypes() const { return *mAntennaPatternTypesPtr; }
	WsfBehaviorTreeNodeTypes&         GetBehaviorTreeNodeTypes() const { return *mBehaviorTreeNodeTypesPtr; }
	WsfCallbackTypes&                 GetCallbackTypes() const { return *mCallbackTypesPtr; }
	wsf::comm::Types&                 GetCommTypes() const { return *mCommTypesPtr; }
	WsfCorrelationStrategyTypes&      GetCorrelationTypes() const { return *mCorrelationTypesPtr; }
	WsfEM_AttenuationTypes&           GetEM_AttenuationTypes() const { return *mEM_AttenuationTypesPtr; }
	WsfEM_ClutterTypes&               GetEM_ClutterTypes() const { return *mEM_ClutterTypesPtr; }
	WsfEM_PropagationTypes&           GetEM_PropagationTypes() const { return *mEM_PropagationTypesPtr; }
	WsfFilterTypes&                   GetFilterTypes() const { return *mFilterTypesPtr; }
	WsfFuelTypes&                     GetFuelTypes() const { return *mFuelTypesPtr; }
	WsfFusionStrategyTypes&           GetFusionTypes() const { return *mFusionTypesPtr; }
	WsfGroupTypes&                    GetGroupTypes() const { return *mGroupTypesPtr; }
	WsfIntersectMeshTypes&            GetIntersectMeshTypes() const { return *mIntersectMeshTypesPtr; }
	WsfMaskingPatternTypes&           GetMaskingPatternTypes() const { return *mMaskingPatternTypesPtr; }
	wsf::comm::medium::Types&         GetMediumTypes() const { return *mMediumTypesPtr; }
	WsfMoverTypes&                    GetMoverTypes() const { return *mMoverTypesPtr; }
	wsf::comm::NetworkTypes&          GetNetworkTypes() const { return *mNetworkTypesPtr; }
	WsfNoiseCloudTypes&               GetNoiseCloudTypes() const { return *mNoiseCloudTypesPtr; }
	WsfPathFinderTypes&               GetPathFinderTypes() const { return *mPathFinderTypesPtr; }
	WsfPlatformTypes&                 GetPlatformTypes() const { return *mPlatformTypesPtr; }
	WsfProcessorTypes&                GetProcessorTypes() const { return *mProcessorTypesPtr; }
	wsf::comm::ProtocolTypes&         GetProtocolTypes() const { return *mCommProtocolTypesPtr; }
	WsfRadarSignatureTypes&           GetRadarSignatureTypes() const { return *mRadarSignatureTypesPtr; }
	WsfRouteTypes&                    GetRouteTypes() const { return *mRouteTypesPtr; }
	WsfRouteNetworkTypes&             GetRouteNetworkTypes() const { return *mRouteNetworkTypesPtr; }
	wsf::comm::router::Types&         GetRouterTypes() const { return *mRouterTypesPtr; }
	wsf::comm::router::ProtocolTypes& GetRouterProtocolTypes() const { return *mRouterProtocolTypesPtr; }
	WsfSensorErrorModelTypes&         GetSensorErrorModelTypes() const { return *mSensorErrorModelTypesPtr; }
	WsfSensorTypes&                   GetSensorTypes() const { return *mSensorTypesPtr; }
	WsfThermalSystemTypes&            GetThermalSystemTypes() const { return *mThermalSystemTypesPtr; }
	WsfThermalSystemComponentTypes&   GetThermalSystemComponentTypes() const { return *mThermalSystemComponentTypesPtr; }
	wsf::TrackExtrapolationStrategyTypes& GetTrackExtrapolationStrategyTypes() const
	{
		return *mTrackExtrapolationStrategyTypesPtr;
	}
	WsfTrackReportingStrategyTypes& GetTrackReportingStrategyTypes() const { return *mTrackReportingStrategyTypesPtr; }
	WsfVisualPartTypes&             GetVisualPartTypes() const { return *mVisualPartTypesPtr; }
	WsfZoneTypes&                   GetZoneTypes() const { return *mZoneTypesPtr; }
	//@}

	//! @name Monte-Carlo iteration control
	//! The following methods can be used to perform a series of monte-carlo iterations, e.g.:
	//!
	//! \code
	//! for (unsigned int runNumber = simInput.GetInitalRunNumber(); runNumber <= simInput.GetFinalRunNumber();)
	//! {
	//!    WsfSimulation* simPtr = app.CreateSimulation(scenario, options);
	//!    ... run simulation
	//!    delete simPtr;
	//!    runNumber += simInput.GetRunNumberIncrement();
	//! }
	//! \endcode
	//@{

	//! Get the run number of the initial iteration.
	unsigned int GetInitalRunNumber() const { return mInitialRunNumber; }

	//! Get the run number of the final iteration.
	unsigned int GetFinalRunNumber() const { return mFinalRunNumber; }

	//! Get the increment between run numbers.
	unsigned int GetRunNumberIncrement() const { return mRunNumberIncrement; }

	void SetInitialRunNumber(unsigned int aInitialRunNumber);
	void SetFinalRunNumber(unsigned int aFinalRunNumber);
	void SetRunNumberIncrement(unsigned int aRunNumberIncrement);

	//! Get/Set the random seed for this specified run number
	long int GetRandomSeed(unsigned int aRunNumber) const;
	long int GetInitialSeed() const { return mInitialSeed; }
	void     SetInitialSeed(long int aInitialSeed) { mInitialSeed = aInitialSeed; }
	//@}

	const std::string& GetClassificationString() const { return mClassification.first; }
	const UtColor&     GetClassificationColor() const { return mClassification.second; }

	//! @name Input platform methods.
	//@{
	bool AddInputPlatform(std::unique_ptr<WsfPlatform> aPlatformPtr);

	WsfPlatform*       GetInputPlatformByName(WsfStringId aNameId);
	const WsfPlatform* GetInputPlatformByName(WsfStringId aNameId) const;

	unsigned int GetInputPlatformCount() const;

	//! Return a specific entry from the list of input platforms.
	//! @param aEntry The slot of the desired entry.  It must be valid, i.e.: 0 <= aEntry < GetInputPlatformCount().
	//! @return Pointer to the requested input platform.
	WsfPlatform* GetInputPlatformEntry(unsigned int aEntry) const;

	bool PlatformIsAvailable(WsfPlatform* aPlatformPtr, WsfSimulation& aSimulation) const;
	//@}


	//! Return whether the simulation is using quantitative track quality.
	bool UseQuantitativeTrackQuality() const { return mUseQuantitativeTrackQuality; }

	//! @name Prototype WsfSignatureList maintenance.
	//@{
	const WsfSignatureList& GetSignatureListPrototype() const { return *mSignatureListPrototypePtr; }
	WsfSignatureList&       GetSignatureListPrototype() { return *mSignatureListPrototypePtr; }
	//@}

	//! @name Testing of expected/unexpected errors.
	//! These are methods used by applications that generate errors and test to make sure they occur.
	//! Normally all errors are 'unexpected'.
	//@{
	//! Returns true if an unexpected input error was encountered.
	//! Unless input testing is in process (noted by the command 'expect_input_error [true|false]'
	//! in the input stream), all input errors are 'unexpected'.
	bool HadUnexpectedError() const { return mHadUnexpectedError; }
	//! Returns true in an error is expected.
	bool ExpectInputError() const { return mExpectInputError; }
	//@}

	//! Returns the name given to the simulation.
	//! Empty if not specified.
	const std::string& GetSimulationName() const { return mSimulationName; }

private:
	void CreateTypeLists(WsfScenario& aScenario);
	template<class T>
	T* CreateTypeList(WsfScenario& aScenario);

	bool ProcessEnumerateCommand(UtInput& aInput);

protected:
	virtual bool ProcessInputP(UtInput& aInput);
	bool         TypesProcessInput(UtInput& aInput);

	bool ExpectError() const { return mExpectInputError; }
	void ExpectError(bool aExpectError);
	void CheckErrorFailure();

	void LoadFromStreamP(UtInput& aInput);
	void FileOpenCallback(const std::string& aFileName);
	bool ProcessExtensionInput(UtInput& aInput);
	void SortExtensions();

	void   BuildClassificationString();
	size_t FindClassificationLevel(const std::string& aLevel) const;

	friend class ErrorTracker;

	WsfApplication* mApplicationPtr;
	bool            mHadUnexpectedError;
	bool            mTestingForInputError;
	bool            mGotError;
	bool            mExpectInputError;
	bool            mExtensionsInOrder;
	//! When true, indicates that CompleteLoad() has been called.  If this is true,
	//! all data in this class should remain immutable
	bool                                   mLoadIsComplete;
	std::unique_ptr<UtInput>               mInputPtr;
	std::unique_ptr<WsfDeferredInput>      mDeferredInputPtr;
	int                                    mOpenConditionalBlocks;
	WsfStringTable                         mStringTable;
	std::unique_ptr<WsfSimulationInput>    mSimulationInputPtr;
	UtScriptTypes*                         mScriptTypesPtr;
	std::unique_ptr<UtScriptEnvironment>   mScriptEnvironmentPtr;
	std::unique_ptr<WsfScriptManager>      mScriptManagerPtr;
	std::unique_ptr<WsfEnvironment>        mEnvironmentPtr;
	std::unique_ptr<UtAtmosphere>          mAtmospherePtr;
	std::unique_ptr<WsfMessageTable>       mMessageTablePtr;
	std::unique_ptr<WsfPathFinderList>     mPathFinderListPtr;
	std::unique_ptr<wsf::TerrainInterface> mTerrainInterfacePtr;
	std::unique_ptr<WsfIFF_Manager>        mIFF_ManagerPtr;

	std::vector<std::string> mInputFiles;

	std::vector<InputFunction> mInputFunctions;

	bool mUseQuantitativeTrackQuality;

	using ClassificationLevel = std::pair<std::string, UtColor>;
	// This is a vector of pairs (do not map, as the input order of levels needs to be maintained)
	std::vector<ClassificationLevel> mClassificationsList;
	ClassificationLevel              mClassification;
	std::set<std::string>            mTrigraphsSet;
	std::set<std::string>            mCaveatsSet;
	size_t                           mLevelIndex{ ut::npos };

	std::string mSimulationName;

	//! Monte Carlo
	//@{
	//! The first run number to be executed.
	unsigned int mInitialRunNumber;

	//! The final run number to be executed.
	unsigned int mFinalRunNumber;

	//! The increment between the run numbers.
	unsigned int mRunNumberIncrement;

	//! The seed used to generate sRandomSeeds.
	long int mInitialRandomSeed;

	long int mInitialSeed;

	//! A list of random number seeds indexed by run number.
	std::vector<long int> mRandomSeeds;
	//@}

	//! The list of input platforms.
	std::vector<std::unique_ptr<WsfPlatform>> mInputPlatforms;

	//! The 'platform_availability' of input platforms.
	std::unique_ptr<WsfPlatformAvailability> mPlatformAvailabilityPtr;

	//! @name Known type lists.
	//@{
	WsfAdvancedBehaviorTreeNodeTypes*     mAdvancedBehaviorTreeNodeTypesPtr;
	WsfAeroTypes*                         mAeroTypesPtr;
	WsfAntennaPatternTypes*               mAntennaPatternTypesPtr;
	WsfBehaviorTreeNodeTypes*             mBehaviorTreeNodeTypesPtr;
	WsfCallbackTypes*                     mCallbackTypesPtr;
	wsf::comm::Types*                     mCommTypesPtr;
	WsfCorrelationStrategyTypes*          mCorrelationTypesPtr;
	WsfEM_ClutterTypes*                   mEM_ClutterTypesPtr;
	WsfEM_AttenuationTypes*               mEM_AttenuationTypesPtr;
	WsfEM_PropagationTypes*               mEM_PropagationTypesPtr;
	WsfFilterTypes*                       mFilterTypesPtr;
	WsfFuelTypes*                         mFuelTypesPtr;
	WsfFusionStrategyTypes*               mFusionTypesPtr;
	WsfGroupTypes*                        mGroupTypesPtr;
	WsfIntersectMeshTypes*                mIntersectMeshTypesPtr;
	WsfMaskingPatternTypes*               mMaskingPatternTypesPtr;
	wsf::comm::medium::Types*             mMediumTypesPtr;
	WsfMoverTypes*                        mMoverTypesPtr;
	wsf::comm::NetworkTypes*              mNetworkTypesPtr;
	WsfNoiseCloudTypes*                   mNoiseCloudTypesPtr;
	WsfPathFinderTypes*                   mPathFinderTypesPtr;
	WsfPlatformTypes*                     mPlatformTypesPtr;
	WsfProcessorTypes*                    mProcessorTypesPtr;
	wsf::comm::ProtocolTypes*             mCommProtocolTypesPtr;
	WsfRouteTypes*                        mRouteTypesPtr;
	wsf::comm::router::Types*             mRouterTypesPtr;
	wsf::comm::router::ProtocolTypes*     mRouterProtocolTypesPtr;
	WsfSensorErrorModelTypes*             mSensorErrorModelTypesPtr;
	WsfSensorTypes*                       mSensorTypesPtr;
	WsfRadarSignatureTypes*               mRadarSignatureTypesPtr;
	WsfRouteNetworkTypes*                 mRouteNetworkTypesPtr;
	wsf::TrackExtrapolationStrategyTypes* mTrackExtrapolationStrategyTypesPtr;
	WsfTrackReportingStrategyTypes*       mTrackReportingStrategyTypesPtr;
	WsfThermalSystemTypes*                mThermalSystemTypesPtr;
	WsfThermalSystemComponentTypes*       mThermalSystemComponentTypesPtr;
	WsfVisualPartTypes*                   mVisualPartTypesPtr;
	WsfZoneTypes*                         mZoneTypesPtr;
	//@}

	//! All of the type lists in order of creation
	std::vector<std::unique_ptr<WsfObjectTypeListBaseI>> mAllTypeLists;
	//! All of the type lists by type.
	std::map<std::string, WsfObjectTypeListBaseI*> mTypesListByKind;

	//! Pointer to the prototype signature list.
	std::unique_ptr<WsfSignatureList> mSignatureListPrototypePtr;

	//! The list of scenario extensions attached to the scenario.
	std::unique_ptr<WsfExtensionList> mExtensionListPtr;

	//! This list of component factories attached to the scenario.
	ComponentFactoryList mComponentFactoryList;
};

#endif
