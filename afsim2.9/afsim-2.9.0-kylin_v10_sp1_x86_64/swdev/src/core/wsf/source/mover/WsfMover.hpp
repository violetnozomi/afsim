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

#ifndef WSFMOVER_HPP
#define WSFMOVER_HPP

#include "wsf_export.h"

#include "UtVec3.hpp"
#include "WsfAttributeContainer.hpp"
class WsfDraw;
class WsfGeoPoint;
class WsfModeList;
class WsfMoverMode;
#include "WsfPath.hpp"
#include "WsfPlatformPart.hpp"
class WsfRoute;
#include "WsfVariable.hpp"

//! A platform part that moves the platform to which it is attached.
//! A 'mover' is a platform part that is responsible for maintaining the
//! kinematic state (position, orientation, speed, acceleration, etc) of the
//! platform to which it is attached.  It is called by the simulation to effect
//! the movement of platforms within the simulation.
class WSF_EXPORT WsfMover : public WsfPlatformPart
{
public:
	static const char* cTYPE_KIND;

	WsfMover() = delete;
	WsfMover(const WsfScenario& aScenario);
	WsfMover(const WsfMover& aSrc);
	WsfMover& operator=(const WsfMover&) = delete;
	~WsfMover() override;

	//! @name Component infrastructure methods not provided by WsfPlatformPart.
	//@{
	WsfComponent* CloneComponent() const override { return Clone(); }
	int           GetComponentInitializationOrder() const override { return cWSF_INITIALIZE_ORDER_MOVER; }
	const int*    GetComponentRoles() const override;
	void*         QueryInterface(int aRole) override;
	//@}

	//! @name Common infrastructure methods.
	//@{
	//! Clone this object.
	//! @return a pointer to the new object.
	WsfMover* Clone() const override = 0;

	const char* GetScriptClassName() const override;

	virtual void SwapMoverInitiated(double aSimTime, WsfMover* aOldMoverPtr);
	bool         Initialize(double aSimTime) override;
	using WsfPlatformPart::Initialize2;
	bool ProcessInput(UtInput& aInput) override;
	void Update(double aSimTime) override;
	//@}

	//! @name Status methods.
	//@{
	void TurnOff(double aSimTime) override;
	void TurnOn(double aSimTime) override;
	void SetNonOperational(double aSimTime) override;
	void SetOperational(double aSimTime) override;
	void PartBroken(double aSimTime) override;
	void Restore(double aSimTime) override;
	//@}

	virtual WsfSpatialDomain GetSpatialDomain() const;

	virtual bool IsStopped() const;
	virtual bool IsPaused() const;
	virtual bool IsExtrapolating() const;

	virtual void SetHeading(double aHeading);
	virtual void SetPitch(double aPitch);
	virtual void SetRoll(double aRoll);

	virtual void SetOrientationNED(double aHeading, double aPitch, double aRoll);

	//! Informs the mover to use the current platform state
	//  * Used in SetRoute or to move platform
	virtual void ResetPosition(double aSimTime) {}

	//! @name Mover update control
	//@{

	//! Get the simulation time when this mover was last updated.
	//! @return The simulation time the Update method was last called.
	double GetLastUpdateTime() const { return mLastUpdateTime; }

	//! Get the event driven update interval.
	//! @return The update interval (in seconds).
	double GetUpdateInterval() const override { return mUpdateInterval; }

	//! Get the update time tolerance.
	//! The mover will not perform an update if the time since the last update is less
	//! than this value. Some processors need this so they do not
	//! schedule updates that will not do anything.
	//! @return The update time tolerance (in seconds).
	//! @note This value can be zero.
	double GetUpdateTimeTolerance() const { return mUpdateTimeTolerance; }

	//! Set the event driven update interval.
	//! @param aUpdateInterval The new update interval (in seconds).
	void SetUpdateInterval(double aUpdateInterval) override { mUpdateInterval = aUpdateInterval; }

	//! Externally set the Sim Time when the WsfMover was last updated.
	//! This method is not for general use, but was required for the WsfHybridMover.
	//! @param aLastUpdateTime The new update time (in seconds).
	void SetLastUpdateTime(double aLastUpdateTime) { mLastUpdateTime = aLastUpdateTime; }

	virtual bool UpdateAllowed() const;

	bool UpdateWhenBroken() const { return mUpdateWhenBroken; }
	void SetUpdateWhenBroken() { mUpdateWhenBroken = true; }
	void SetNoUpdateWhenBroken() { mUpdateWhenBroken = false; }
	//@}

	//! @name Navigation methods.
	//! These methods provide the interface for external control of the mover.
	//! It is up to each mover implement these as desired. The methods should return 'false' if
	//! the method is not implemented.

	//@{
	virtual const WsfRoute* GetRoute() const;

	virtual const WsfRoute* GetDefaultRoute() const;

	virtual int RoutePointIndex() const { return 0; }

	virtual bool GoToAltitude(double aSimTime, double aAltitude, double aClimbDiveRate, bool aKeepRoute);

	virtual bool ReturnToRoute(double aSimTime);

	virtual bool GoToClosestPoint(double aSimTime);

	virtual bool GoToLabel(double aSimTime, WsfStringId aLabelId);

	virtual bool GoToLocation(double aSimTime, double aLatitude, double aLongitude, double aAltitude);

	virtual bool GoToSpeed(double aSimTime, double aSpeed, double aLinearAccel, bool aKeepRoute);

	virtual bool TurnToHeading(double aSimTime, double aHeading, double aRadialAccel, WsfPath::TurnDirection aTurnDirection);

	virtual bool TurnToRelativeHeading(double                 aSimTime,
		double                 aHeadingChange,
		double                 aRadialAccel,
		WsfPath::TurnDirection aTurnDirection);

	virtual bool SetRoute(double aSimTime, const WsfRoute& aRoute);

	virtual bool UpdateRoute(double aSimTime, const WsfRoute& aRoute);

	virtual bool UpdateDefaultRoute();

	virtual bool GoToWaypoint(double aSimTime, unsigned int aDestination);

	virtual bool FindAndSetPath(double aSimTime, WsfGeoPoint* aStartPtr, WsfGeoPoint* aEndPtr);

	virtual bool FlyRates(const UtVec3d& aAngularRates, // radians/sec
		const UtVec3d& aLongitudinalAccel)
	{
		return false;
	} // meters/sec^2

	virtual bool FlyHeadingSpeedAltitude(double aHeading,  // deg
		double aSpeed,    // meters/sec
		double aAltitude, // meters
		double aMaxGees,  // Gs
		double aMaxClimb)
	{
		return false;
	} // meters/sec

	virtual bool TurnInPlaneWithSpeed(const UtVec3d& aPlane, // world coordinates (not body coordinates)
		double         aGees,  // Gs
		double         aSpeed)
	{
		return false;
	} // meters/sec

	virtual bool TurnInPlaneWithThrottle(const UtVec3d& aPlane, // world coordinates (not body coordinates)
		double         aGees,  // Gs
		double         aThrottle)
	{
		return false;
	} //[0-3]

	virtual bool FlyVectorWithSpeed(const UtVec3d& aVelocityVector, // world coordinates (not body coordinates)
																	// (magnitude does not matter, just direction)
		double aMaxGees,                // Gs
		double aSpeed)
	{
		return false;
	} // meters/sec

	virtual bool FlyVectorWithThrottle(const UtVec3d& aVelocityVector, // world coordinates (not body coordinates)
																	   // (magnitude does not matter, just direction)
		double aMaxGees,                // Gs
		double aThrottle)
	{
		return false;
	} //[0-3]
	//@}

	virtual bool ExecuteTriggers(double aSimTime, unsigned int aPointIndex);

	virtual bool GetFutureLocationWCS(double aSimTime, double aLocationWCS[3]);

	//! Get the current mode ID of the Mover.  This value is implementation defined,
	//! and may be used to indicate "Stopped", "Idle", "Taxi", "Climb", or any other
	//! state or mode of interest.  Derived WsfMover implementations may modify their
	//! behavior based upon this value.
	WsfStringId  GetModeId() const { return mModeId; }
	virtual void SetMode(WsfStringId aModeName);

	//! Returns true if the mover update is thread safe.
	bool ThreadSafe() const { return mThreadSafe; }
	void SetThreadSafe() { mThreadSafe = true; }
	void SetNotThreadSafe() { mThreadSafe = false; }

	//! @name Property Methods
	//! These methods are used for the defining and accessing named 'properties' (key/value pairs)
	//! that represent capabilities of the mover. The base class maintains a set of 'common' properties,
	//! but a derived class can override the GetProperty method to access class-specific properties.
	//@{

	//! Add any input properties and its default value common properties list.
	virtual void InitializeProperties() {};

	//! Return true if the given property exists in the set of properties.
	virtual bool PropertyExists(const std::string& aPropertyName);

	//! Add an integer property to the common set of properties.
	virtual void RegisterProperty(const std::string& aPropertyName, const int& aProperty);
	//! Add an floating point property to the common set of properties.
	virtual void RegisterProperty(const std::string& aPropertyName, const double& aProperty);
	//! Add an string property to the set of common properties.
	virtual void RegisterProperty(const std::string& aPropertyName, const std::string& aProperty);

	//! Get the value of an integer property from either a derived-class set of properties or
	//! the common set of properties.
	virtual bool GetPropertyInt(const std::string& aPropertyName, int& aProperty);
	//! Get the value of a floating point property from either a derived-class set of properties or
	//! the common set of properties.
	virtual bool GetPropertyDouble(const std::string& aPropertyName, double& aProperty);
	//! Get the value of a string property from either a derived-class set of properties or
	//! the common set of properties.
	virtual bool GetPropertyString(const std::string& aPropertyName, std::string& aProperty);

	//! Get the throttle position that the mover is commanding
	//! A throttle position x such that 1.0 <= x < 2.0 indicates military power
	//! A throttle position x such that x == 2.0 indicates max power
	//! A throttle position x such that 2.0 < x <= 3.0 indicates afterburner
	virtual double GetThrottlePosition() { return 1.0; }

	//@}

	//! @name Deprecated methods
	//! These should not be used in new code. They have been replaced with the property methods.
	//@{
	virtual double TurnRadius(double atSpeed) { return 0.0; } // TODO //returns meters
	//@}

	WsfModeList* GetModeList() const { return mModeListPtr; }

	//! Way to artificially inject an extrapolation event into the simulation.
	//! (Primarily for script-driven TSPI Movers, to inform of a burnout event.)
	virtual void TriggerExtrapolation();

	//! A pure virtual base class encapsulating a kinematic model of a mover.
	//! It may be used to predict ahead the locations, times or apogee of entities.
	class WSF_EXPORT KinematicModel
	{
	public:
		KinematicModel() = delete;
		KinematicModel(const KinematicModel& aSrc) = default;
		virtual ~KinematicModel();

		double GetApogeeTime() const { return mApogeeTime; }
		double GetTimeAtLocation() const { return mTimeAtLocation; }
		double GetStartTime() const { return mStartTime; }

		//! Return the SPHERICAL WCS location of the model at the specified time.
		//! @param aTime The time for which the model position is to be returned.
		//! @param aLocXYZ The spherical WCS location of the model at 'aSimTime'.
		//! @returns true if valid data was returned or false if no data exists for the specified time.
		virtual bool GetLocationXYZ(double aTime, double aLocXYZ[3]) const = 0;

		//! Return the SPHERICAL WCS velocity of the model at the specified time.
		//! @param aTime The time for which the model velocity is to be returned.
		//! @param aVelXYZ The spherical WCS velocity of the model at 'aSimTime'.
		//! @returns true if valid data was returned or false if no data exists for the specified time.
		//! @note This routine does not need to be particularly fast or accurate.
		virtual bool GetVelocityXYZ(double aTime, double aVelXYZ[3]) const = 0;

		static void GetRangeAndAltitude(const double aOriginLocXYZ[3],
			const double aModelLocXYZ[3],
			double&      aRange,
			double&      aAltitude);

	protected:
		KinematicModel(double aSimTime, WsfStringId aModelTypeId);

		double      mStartTime;
		double      mApogeeTime;
		double      mTimeAtLocation;
		WsfStringId mModelTypeId;
	};

	//! Get the kinematic model of this mover if available.
	//! If aMoverPtr is null, "this" has been initialized (i.e., GetPlatform(), etc. will work);
	//! however, if aMoverPtr is non-null, "this" has not been initialized, so that any data needed from the simulation
	//! or scenario should be retrieved using aMoverPtr instead.  In either case, the kinematic model needs to be
	//! extracted without disturbing the state of the platform to which either of "this" or aMoverPtr is attached.
	//! @param aSimTime The current simulation time.
	//! @param aPlatformTypeId The platform type id that is being propagated kinematically.
	//! @param aLocationWCS The location of the platform at aSimTime.
	//! @param aVelocityWCS The velocity of the platform at aSimTime.
	//! @param aMoverPtr If non-null, a valid mover from which "this" should be initialized.
	//! @param aDrawPtr A WsfDraw object that can be used for custom visualization.
	//! @return The kinematic model pointer if available, otherwise nullptr.
	virtual KinematicModel* CreateKinematicModel(double       aSimTime,
		WsfStringId  aPlatformTypeId,
		const double aLocationWCS[3],
		const double aVelocityWCS[3],
		WsfMover*    aMoverPtr,
		WsfDraw*     aDrawPtr) const
	{
		return nullptr;
	}

protected:
	void SetModeList(WsfModeList* aModeListPtr);

	//! Revise the internal time tolerance allowed for Update() sequencing logic.
	void SetUpdateTimeTolerance(double aDt) { mUpdateTimeTolerance = aDt; }

	double mLastUpdateTime; //!< The simulation time when Update was last called.

	//! If the time since the last update is less than or equal to this value then the update does
	//! not need to be performed.
	WsfVariable<double> mUpdateTimeTolerance;

	//! Identifies the mover as thread safe; mover updates can be multi-threaded.
	//! Certain types of derived movers may have dependencies that may not make them thread-safe,
	//!  i.e. formation flyer, hybrid mover
	bool mThreadSafe;

	//! The generic list of modes (not valid until Initialize is called)
	std::vector<WsfMoverMode*> mMoverModeList;

	WsfStringId mModeId;

private:
	WsfModeList* mModeListPtr;

	bool                   HasProperties() const { return mPropertiesPtr != nullptr; }
	WsfAttributeContainer& GetProperties() const;

	//! The number of seconds between interval updates.
	//! An event driven simulation can use this to schedule events to force regular updates of the mover.
	WsfVariable<double> mUpdateInterval;

	//! Any input properties that are accessible from script can be stored in this member variable
	//! NOTE: Each property should be added to InitializeProperties() with its default value
	//!       and also updated in ProcessInput() with the RegisterProperty() function.
	mutable WsfAttributeContainer* mPropertiesPtr;

	//! True if the mover updates should be allowed even if the mover is marked 'broken'.
	bool mUpdateWhenBroken;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfMover, cWSF_COMPONENT_MOVER)

#endif
