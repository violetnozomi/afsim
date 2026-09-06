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
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#ifndef WSFPLATFORM_HPP
#define WSFPLATFORM_HPP

#include "wsf_export.h"

#include <string>
#include <vector>

#include "UtEntity.hpp"
class UtInput;
class UtInputBlock;
namespace ut
{
namespace script
{
class Data;
}
} // namespace ut
class WsfArticulatedPart;
#include "WsfAuxDataEnabled.hpp"
#include "WsfCategoryList.hpp"
class WsfCommAddress;
#include "WsfComponent.hpp"
#include "WsfComponentList.hpp"
#include "WsfEvent.hpp"
class WsfFuel;
class WsfGroup;
#include "WsfGroupList.hpp"
class WsfMover;
class WsfNavigationErrors;
#include "WsfObject.hpp"
class WsfSinglePlatformObserver;
class WsfPlatformPart;
#include "WsfRandomVariable.hpp"
class WsfScenario;
#include "script/WsfScriptContext.hpp"
class WsfSignatureList;
class WsfSimulation;
#include "WsfStringId.hpp"
class WsfTrack;
#include "WsfTrackId.hpp"
#include "WsfTrackListFwd.hpp"
class WsfTrackManager;
#include "WsfTypes.hpp"
#include "WsfUniqueId.hpp"

namespace wsf
{
class Terrain;
}

/** Platforms represent a entity within the simulation.

   A platform is basically acts as a container for the 'systems' that
   define how the platform operates.

   The general sequence for creating a platform is as follows:

   <ul>
   <li> Instantiate a WsfPlatform object.
   <li> Set the name of the platform with SetName() (every platform in the
   simulation \b MUST have a unique name).
   <li> Set the side (team/affiliation) of the platform with SetSide().
   <li> Instantiate the appropriate specialization of the WsfMover object and call SetMover().
   <li> Instantiate systems (specializations of WsfComm, WsfProcessor, WsfSensor, etc.) and add
   to the platform using AddComponent().
   </ul>
   \n
   The above sequence can involve hundreds of lines of code to define all the
   appropriated attributes.  It is typically much easier to use the simulation
   input to allow the creation of simulation objects from text files.

   \note
   \n
   Platforms may be 'destroyed' at any time during a simulation.  As such,
   persistent pointers to a platform should not be maintained (except between
   objects that are 'owned' by the platform such as sensors, processors, etc).
   Instead of retaining pointers you should retain a platform index:

   \code
   platformIndex = platformPtr->GetIndex();
   \endcode

   To retrieve the pointer to the platform:

   \code
   platformPtr = GetSimulation()->GetPlatformByIndex(platformIndex);
   \endcode

   The return value will be nullptr if the platform no longer exists.

   @see WsfSimulation
*/
class WSF_EXPORT WsfPlatform :
	public WsfObject,
	public WsfPlatformComponent,
	public WsfPlatformComponentList,
	public WsfUniqueId,
	public UtEntity,
	public WsfAuxDataEnabled
{
public:
	// So WsfSimulation can access protected/private methods that shouldn't be made public.
	friend class WsfSimulation;
	friend class WsfSimulationBase;

	//! Define what should be done when the platform is broken.
	enum OnBrokenAction
	{
		//! Remove the platform from the simulation.
		cON_BROKEN_REMOVE,
		//! Retain the platform, disable all its subsystems, and halt movement.
		cON_BROKEN_DISABLE,
		//! Retain the platform, disable all its subsystems, but allow movement.
		cON_BROKEN_MOVABLE_DISABLED
	};

	WsfPlatform(const WsfScenario& aScenario);
	~WsfPlatform() override;

	//! @name Component infrastructure methods.
	//@{
	WsfStringId   GetComponentName() const override { return GetNameId(); }
	WsfComponent* CloneComponent() const override { return Clone(); }
	const int*    GetComponentRoles() const override;
	void*         QueryInterface(int aRole) override;
	//@}

	//! @name Common framework methods.
	//@{
	const char*      GetScriptClassName() const override;
	UtScriptContext* GetScriptAccessibleContext() const override;

	WsfPlatform* Clone() const override;
	void         AssignToSimulation(WsfSimulation* aSimulationPtr);
	virtual void CompleteLoad(WsfScenario& aScenario);
	bool         ProcessInput(UtInput& aInput) override;
	virtual bool Initialize(WsfSimulation* aSimulationPtr, double aSimTime);
	virtual bool Initialize2(WsfSimulation* aSimulationPtr, double aSimTime);

	//! Update the platform to reflect the state at the specified time.
	//!
	//! This only updates the platform position and fuel utilization.  Updating of attached parts
	//! (sensors, comms, etc.) are left to the simulation executive because the simulation may
	//! required the updates to be performed in a certain order.
	//!
	//! @param aSimTime The current simulation time.
	//!
	//! @note This is a non-virtual method which will first check to see if an update is actually
	//! required. If an update is required then the protected virtual method DoUpdate() will be called.
	void Update(double aSimTime)
	{
		if (mUpdateAllowed && // See DoUpdate for why this check is needed
			(!mUpdateLocked) && (mLastUpdateTime < aSimTime))
		{
			DoUpdate(aSimTime);
		}
	}

	void UpdateMultiThread(double aSimTime)
	{
		if (mUpdateAllowed && // See DoUpdate for why this check is needed
			(mLastUpdateTime < aSimTime))
		{
			DoUpdateMultiThread(aSimTime);
		}
	}
	//@}

	void NotifyUpdate(double aSimTime)
	{
		if (mUpdateAllowed)
		{
			DoNotifyUpdate(aSimTime);
		}
	}

	void ExecuteScript(double aSimTime)
	{
		if (mUpdateAllowed)
		{
			DoExecuteScript(aSimTime);
		}
	}

	void SendQueuedMessages(double aSimTime)
	{
		if (mUpdateAllowed)
		{
			DoSendQueuedMessages(aSimTime);
		}
	}

	//! @name Simulation interface methods.
	//@{

	//! Get the time when the platform was (or is to be) added to the simulation.
	double GetCreationTime() const { return mCreationTime.LastDraw(); }

	bool InitializeCreationTime();

	void SetCreationTime(double aCreationTime);

	//! Return the time of the last call to Update or UpdateFrame.
	double GetLastUpdateTime() const { return mLastUpdateTime; }

	double GetSimTime() const;

	//! Set the update locked flag to prevent updates to the platform.
	//! Currently used by the WsfMultiThreadManager to lock out updates
	//! other than in the multi-thread pool.
	void SetUpdateLocked(bool aUpdateLocked) { mUpdateLocked = aUpdateLocked; }

	//! Is the platform being controlled by something external to this application?
	//! This is typically used to allow the introduction of entities that may be
	//! modeled by some other application that may be connected to us over a network.
	//!
	//! @return 'true' if controlled by another application or 'false' if controlled
	//! by this application.
	bool IsExternallyControlled() const { return mIsExternallyControlled; }

	//! Set the value of the 'IsExternallyControlled' attribute.
	//! Define the platform being controlled by something external to this application.
	//! @param aIsExternallyControlled 'true' if controlled by another application
	//! or 'false' if controlled by this application.  The default value is 'false'.
	//! @see IsExternallyControlled.
	void SetIsExternallyControlled(bool aIsExternallyControlled) { mIsExternallyControlled = aIsExternallyControlled; }

	//! Is the platform being moved by something external to this application?
	//! This is typically used to allow to be driven by some other application that may be connected
	//! to us over a network yet allow this platform to remain local.
	//!
	//! @return 'true' if moved by another application or 'false' if moved by this application.
	bool IsExternallyMoved() const { return mIsExternallyMoved; }

	//! Set the value of the 'IsExternallyMoved' attribute.
	//! Define the platform being moved by something external to this application.
	//! @param aIsExternallyMoved 'true' if moved by another application or 'false'
	//! if moved by this application.  The default is 'false'.
	//! @see IsExternallyMoved.
	void SetIsExternallyMoved(bool aIsExternallyMoved) { mIsExternallyMoved = aIsExternallyMoved; }

	//! Is the platform a shadow of an external entity?
	//! An 'external shadow platform' is a debugging tool that provides a locally controlled platform that mirrors
	//! the movement of another platform that is marked 'IsExternallyControlled'. This local shadow platform may then
	//! be written to the external simulation where a stealth viewer can be used to see that our internal perception
	//! of the platform matches the external entity that we are shadowing.
	//! @return 'true' if this is a shadow platform or 'false' if not.
	//! @note Sensors and certain other objects should test this attribute and not detect or remove shadow entities.
	//! @see WsfDisInterface for an example of shadow entity use.
	bool IsExternalShadow() const { return mIsExternalShadow; }

	//! Set the value of the 'IsExternalShadow' attribute.
	//! @param aIsExternalShadow 'true' if this is a shadow of an externally controlled entity.
	//! @see IsExternalShadow.
	void SetIsExternalShadow(bool aIsExternalShadow) { mIsExternalShadow = aIsExternalShadow; }

	//! Is the platform indestructible?
	//! A platform may be marked as indestructible in order prevent it from being destroyed or removed.
	//! When this value is set 'true', any applied damage accumulates in a logarithmically decaying fashion,
	//! so GetDamageFactor(), rather than returning an actual damage state, reflects an ever decreasing
	//! (but never reaching zero) probability of survival.
	//! @returns 'true' if indestructible or 'false' if not.
	bool IsIndestructible() const { return mIsIndestructible; }

	//! Set the value of the 'IsIndestructible' attribute.
	//! @param aIsIndestructible 'true' if the platform is indestructible or 'false' if not.
	//! @see IsIndestructible.
	void SetIsIndestructible(bool aIsIndestructible) { mIsIndestructible = aIsIndestructible; }

	//! Get the action to occur when the platform is broken.
	OnBrokenAction GetOnBrokenAction() const { return mOnBrokenAction; }

	//! Set the action to occur when the platform is broken.
	void SetOnBrokenAction(OnBrokenAction aOnBrokenAction) { mOnBrokenAction = aOnBrokenAction; }

	//! Process the Broken Event.
	void OnBrokenEvent(double aSimTime);

	//! Is the platform broken?
	//! Depending on the 'on_broken_action', a platform may exist in the simulation but still be broken.
	bool IsBroken() const { return mIsBroken; }

	//! Returns 'true' if the platform has been scheduled for deletion, but hasn't been physically deleted yet.
	bool IsDeleted() const { return mIsDeleted; }

	//! Returns 'true' if the platform has been initialized.
	bool IsInitialized() const { return mIsInitialized; }

	//! Get the unique index of the platform within the simulation.
	//!
	//! The index returned by GetIndex is unique within the simulation. It
	//! will never be reassigned even if the platform is deleted.  This index
	//! can be used to determine if a platform still exists and safely access
	//! the pointer to the platform.  WsfSimulation::GetPlatformByIndex will
	//! return the pointer a platform with a given index or 0 if the platform
	//! no longer exists.
	//! @note This is valid only after the platform has been added to the
	//! simulation (WsfSimulation::AddPlatform) and the simulation has been
	//! initialized (WsfSimulation::Initialize).
	size_t GetIndex() const { return mIndex; }

	//! Set the unique index of the platform within the simulation.
	//! @param aIndex Index of the platform within the simulation.
	//! @note This is an internal method that should only be called by WsfSimulation.
	void SetIndex(size_t aIndex) { mIndex = aIndex; }

	//! If the platform is a member of a simulation, this is the containing simulation
	WsfSimulation* GetSimulation() const { return mSimulationPtr; }

	//! The platform type/instance belongs to this scenario
	const WsfScenario& GetScenario() const { return mScenario; }

	virtual void PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr);

	virtual void PlatformDeleted(WsfPlatform* aPlatformPtr);
	//@}

	//! @name Self-perception methods.
	//! These methods deal with the platforms position of itself. If navigation errors are defined
	//! then the path of the platform can be altered and errors can be introduced into sensor reports.
	//@{
	void GetLocationErrorWCS(double aLocationErrorWCS[3]) const;
	//@}

	//! @name 'side' (team) methods.
	//@{
	//! Get the 'side' (affiliation) to which this platform belongs.
	const std::string& GetSide() const { return mSide.GetString(); }
	//! Set the 'side' (affiliation) to which this platform belongs.
	void SetSide(WsfStringId aSide);
	//! Get the string ID of the side (affiliation) to which this platform belongs.
	WsfStringId GetSideId() const { return mSide; }
	//@}

	//! @name 'icon' methods.
	//@{
	//! Get the name of the icon associated with this platform.
	const std::string& GetIcon() const { return mIcon.GetString(); }
	//! Set the name of the icon associated with this platform.
	void SetIcon(WsfStringId aIcon) { mIcon = aIcon; }
	//! Get the string ID of the name of the icon associated with this platform.
	WsfStringId GetIconId() const { return mIcon; }
	//@}

	//! @name 'marking' methods.
	//@{
	//! Get the marking associated with this platform.
	const std::string& GetMarking() const { return mMarking.GetString(); }
	//! Set the marking associated with this platform.
	void SetMarking(WsfStringId aMarking) { mMarking = aMarking; }
	//! Get the string ID of the marking associated with this platform.
	WsfStringId GetMarkingId() const { return mMarking; }
	//@}

	//! @name Spatial domain methods.
	//! The spatial domain represents the primary domain where the platform exists (land, air, space, etc.)
	//@{
	//! Get the spatial domain of the platform.
	WsfSpatialDomain GetSpatialDomain() const;
	//! Set the spatial domain of the platform.
	void SetSpatialDomain(const WsfSpatialDomain& aSpatialDomain) { mSpatialDomain = aSpatialDomain; }
	//@}

	//! @name Script methods
	//@{
	bool ExecuteScript(double                               aSimTime,
		WsfStringId                          aName,
		UtScriptData&                        aScriptRetVal,
		const std::vector<ut::script::Data>& aScriptArgs);
	bool ExecuteScript(double aSimTime, WsfStringId aName);
	void ExecuteScriptAtTime(double aTime, WsfStringId aName, const std::vector<ut::script::Data>& aScriptArgs);
	bool HasScript(WsfStringId aScriptNameId) const;

	//! Return a reference to the script context.
	WSF_DEPRECATED WsfScriptContext* GetContext() { return &mScriptContext; }
	WsfScriptContext&                GetScriptContext() { return mScriptContext; }
	//@}

	//! @name Component methods that are not implemented in WsfComponentList.
	//@{
	WsfArticulatedPart* GetArticulatedPart(unsigned int aUniqueId) const;
	//! Return a reference to the raw component list.
	const WsfPlatformComponentList& GetComponents() const { return *this; }
	//@}

	//! @name Mover system methods
	//@{
	void SetMover(WsfMover* aMoverPtr);
	bool SwapMover(double aSimTime, WsfMover*& aNewMoverPtr, WsfMover*& aOldMoverPtr);

	//! Return the pointer to the mover object for this platform.
	//! @return A pointer to the mover object or nullptr if a mover has not been defined.
	WsfMover* GetMover() const { return mMoverPtr; }
	//@}

	//! Return the pointer to the Fuel object for this platform.
	//! @return A pointer to the Fuel object or nullptr if a Fuel has not been defined.
	WsfFuel* GetFuel() const { return mFuelPtr; }
	//@}

	//! @name Terrain context methods.
	//@{
	//! Return a reference to the associated terrain object.
	wsf::Terrain& GetTerrain() const { return *mTerrainPtr; }
	double        GetHeightAboveTerrain();
	double        GetTerrainHeight();
	//@}

	//! @name Platform false target methods.
	//! A platform is a false target if it does not represent a physical object.
	//@{
	bool IsFalseTarget() const { return mIsFalseTarget; }
	void SetIsFalseTarget(bool aBoolState) { mIsFalseTarget = aBoolState; }
	//@}

	//! @name Platform mass accounting methods (total, empty, payload, and fuel masses).
	//@{

	//! Get the current mass of platform.
	//! The mass of the platform may be fixed through the input stream, or controlled independently
	//! during run time by internal processes.  (For instance, if the platform contains a WsfFuel object,
	//! the fuel mass may be overwritten during run time.)  The total mass of a platform is the sum of
	//! its empty mass, fuel mass, and payload mass components.  Each mass value defaults to zero.
	//! @return Returns current mass of the platform.
	double GetMass() const { return mEmptyMass + mFuelMass + mPayloadMass; }

	//! Get the empty mass of platform.
	//! Unusable fuel (if any) should be accounted with empty weight.
	//! @return Returns empty mass of the platform.
	double GetEmptyMass() const { return mEmptyMass; }

	//! Get the Payload mass on the platform.
	//! @return Returns Payload mass of the platform.
	double GetPayloadMass() const { return mPayloadMass; }

	//! Get the usable fuel mass on the platform.
	//! Unusable fuel should be accounted with empty weight.
	//! @return Returns fuel mass of the platform.
	double GetFuelMass() const { return mFuelMass; }

	//! Set current fuel for the platform.
	//! (This accessor will primarily be used internally by the WsfFuel classes.)
	void SetFuelMass(double aValue) { mFuelMass = aValue; }

	//! Set Payload mass for the platform.
	void SetPayloadMass(double aValue) { mPayloadMass = aValue; }

	//! Set Empty mass for the platform.
	void SetEmptyMass(double aValue) { mEmptyMass = aValue; }

	//@}

	//! @name Platform appearance and capabilities methods.
	//@{
	//! Return a reference to the 'signature list' object that encapsulates all the signature processing.
	WsfSignatureList& GetSignatureList()
	{
		if (mSignatureListPtr == nullptr)
		{
			CreateSignatureList();
		}
		return *mSignatureListPtr;
	}

	//! Get height of platform.
	//! @return Returns the height of platform.
	double GetHeight() const { return mHeight; }

	//! Get length of platform.
	//! @return Returns the length of platform.
	double GetLength() const { return mLength; }

	//! Get width of platform.
	//! @return Returns width of platform.
	double GetWidth() const { return mWidth; }

	//! Set overall length for the platform.
	void SetLength(double aValue) { mLength = aValue; }

	//! Set overall width for the platform (i.e. wingspan for an aircraft).
	void SetWidth(double aValue) { mWidth = aValue; }

	//! Set overall height for the platform.
	void SetHeight(double aValue) { mHeight = aValue; }

	//! Get the 'appearance' field.
	//! @return The appearance field value.
	//! @see SetAppearance.
	unsigned int GetAppearance(unsigned int aLSB, unsigned int aNumBits) const;
	unsigned int GetAppearance() const { return mAppearanceWord; }

	void SetAppearance(unsigned int aLSB, unsigned int aNumBits, unsigned int aValue, bool aFromScript = false);

	bool ScriptOverride(unsigned int aLSB, unsigned int aNumBits) const;

	//! Get the 'capabilities' word, a bit-field of boolean capabilities.  See WsfExchange.hpp for details.
	unsigned int GetCapabilities() const { return mCapabilitiesWord; }

	//! Set the 'capabilities' word, a bit-field of boolean capabilities.  See WsfExchange.hpp for details.
	void SetCapabilities(unsigned int aCapabilitiesWord);

	//! Get the 'concealment factor'.
	//! @return The concealment factor in the range [0 .. 1]
	//! @see SetConcealmentFactor.
	float GetConcealmentFactor() const { return mConcealmentFactor; }

	void SetConcealmentFactor(float aConcealmentFactor);

	//! Get the 'damage factor'.
	//! @return The damage factor in the range [0 .. 1].
	//! @see SetDamageFactor.
	double GetDamageFactor() const { return mDamageFactor; }

	void SetDamageFactor(double aDamageFactor);

	void Comment(const double aTime, const std::string& aString);

	//@}

	//! @name Platform motion methods.  Generally, these methods should be queried in the order given.
	//@{
	bool IsStopped() const;
	bool IsPaused() const;
	bool IsExtrapolating() const;
	//@}

	//! @name Track management.
	//@{
	const WsfTrackId&  GetNextTrackId();
	WsfLocalTrackList& GetMasterTrackList();
	WsfTrackList&      GetMasterRawTrackList();
	WsfTrackManager&   GetTrackManager();
	//@}

	//! @name Category management.
	//@{
	//! Get the categories to which this platform belongs.
	const WsfCategoryList& GetCategories() const { return mCategories; }
	void                   AddCategory(WsfStringId aCategoryId);
	bool                   IsCategoryMember(WsfStringId aCategoryId) const;
	//@}

	//! @name Group management.
	//@{
	//! Get the Groups to which this platform belongs.
	const WsfGroupList& GetGroups() const { return mGroups; }
	void                JoinGroup(WsfGroup* aGroupPtr);
	void                LeaveGroup(WsfGroup* aGroupPtr);
	bool                IsGroupMember(WsfStringId aGroupId);
	//@}

	//! @name Methods to attach observers to the platform.
	//! Observers receive notification of mover updates and deletion.
	//@{
	void AttachObserver(WsfSinglePlatformObserver* aObserverPtr);
	void DetachObserver(WsfSinglePlatformObserver* aObserverPtr);
	void NotifyDeleted(double aSimTime);
	void NotifyDeleting(double aSimTime);
	//@}

	//! @name Methods used to process input for loading of types and instances.
	//! These are only called by input processing routines and not for general use.
	//@{
	void ProcessInputBlock(UtInputBlock& aInputBlock, bool aLoadingType);
	//@}

	//! Methods only for use by WsfSimulation.
	//@{
	//! Indicate the platform is broken.
	//! @note This is intended to be called only by WsfSimulation extensions.
	void SetBroken() { mIsBroken = true; }

	//! Specifies that the platform is scheduled for deletion.
	//! @note This is intended to be called only by WsfSimulation::DeletePlatform()
	void SetDeleted(bool aDeleted = true) { mIsDeleted = aDeleted; }
	//@}

protected:
	WsfPlatform(const WsfPlatform& aSrc);

	virtual void DoUpdate(double aSimTime);
	virtual void DoUpdateMultiThread(double aSimTime);
	virtual void DoMoverUpdate(double aSimTime);
	virtual void DoNotifyUpdate(double aSimTime);
	virtual void DoExecuteScript(double aSimTime);
	virtual void DoSendQueuedMessages(double aSimTime);

	//! @name Callbacks from WsfComponentList.
	//@{
	void ComponentAdded(WsfComponent* aComponentPtr) override;
	void ComponentDeleted(WsfComponent* aComponentPtr) override;
	//@}

private:
	//! Prevent use of operator= by declaring, but not defining.
	WsfPlatform& operator=(const WsfPlatform& aRhs) = delete;

	//! @name Other private methods
	//@{
	void CreateSignatureList();
	void UpdatePersistentPointers(WsfComponent* aComponentPtr, bool aIsBeingAdded);
	//@}

	const WsfScenario& mScenario;
	WsfSimulation*     mSimulationPtr = nullptr;

	//! The unique index of the platform within the simulation.
	//! This will be zero if the platform isn't yet associated with a simulation.
	size_t mIndex = 0;

	WsfStringId mSide = nullptr;
	WsfStringId mIcon = nullptr;
	WsfStringId mMarking = nullptr;

	//! The user-specified 'spatial domain' in which the platform exists.
	WsfSpatialDomain mSpatialDomain = WSF_SPATIAL_DOMAIN_UNKNOWN;

	OnBrokenAction mOnBrokenAction = cON_BROKEN_REMOVE;

	//! The creation time serves two purposes.
	//! During input, it defines when the platform is to be created.
	//! After initialization it defines when the platform was created.
	WsfRandomVariable mCreationTime = { 0.0, WsfRandomVariable::cNON_NEGATIVE };
	double            mLastUpdateTime = -1.0;

	// Boolean flags are packed into an integer here
	union
	{
		struct
		{
			bool mIsInitialized : 1;
			bool mIsIndestructible : 1;
			bool mIsBroken : 1;
			bool mIsDeleted : 1;
			bool mUpdateAllowed : 1;
			bool mUpdateLocked : 1;
			bool mIsExternallyControlled : 1;
			bool mIsExternallyMoved : 1;
			bool mIsExternalShadow : 1;
			bool mIsFalseTarget : 1;
			bool mInputAltAGL : 1;
			bool mHasOnUpdateScript : 1;
			bool mInitPass : 1;
		};
		unsigned int mFlags;
	};

	mutable WsfTrackManager* mMasterTrackManagerPtr = nullptr; //!< Pointer to the master track manager
	WsfSignatureList*        mSignatureListPtr = nullptr; //!< Pointer to the signatures
	WsfMover*                mMoverPtr = nullptr; //!< Persistent pointer to mover component
	WsfFuel*                 mFuelPtr = nullptr; //!< Persistent pointer to fuel component
	WsfNavigationErrors*     mNavErrorPtr = nullptr; //!< Persistent pointer to navigation error component
	WsfScriptContext         mScriptContext;                   //!< The script context for the platform
	wsf::Terrain*            mTerrainPtr = nullptr;            //!< Terrain context
	WsfCategoryList          mCategories;
	WsfGroupList             mGroups;

	//! The following is used to assign the track ID's for tracks created by this platform.
	WsfTrackId mNextTrackId;

	//! Platform dimensions
	WsfVariable<double> mHeight = 0.0;
	WsfVariable<double> mLength = 0.0;
	WsfVariable<double> mWidth = 0.0;

	//! Platform mass
	WsfVariable<double> mEmptyMass = 0.0; // Platform empty mass (fixed)
	WsfVariable<double> mFuelMass = 0.0; // Platform fuel mass (variable)
	WsfVariable<double> mPayloadMass = 0.0; // Payload mass (variable)

	double mDamageFactor = 0.0;
	float  mConcealmentFactor = 0.0F;

	//! Appearance (provided for local, script, and DIS Entity State Appearance)
	unsigned int mAppearanceWord = 0;
	unsigned int mAppearanceWordScriptMask = 0;
	unsigned int mCapabilitiesWord = 0;

	std::vector<WsfSinglePlatformObserver*> mObservers;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfPlatform, cWSF_COMPONENT_PLATFORM)

#endif
