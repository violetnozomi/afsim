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

#ifndef WSFSPACEMOVERBASE_HPP
#define WSFSPACEMOVERBASE_HPP

#include "wsf_space_export.h"

#include <memory>

class UtCalendar;
#include "UtCentralBody.hpp"
#include "UtCloneablePtr.hpp"
#include "UtColor.hpp"
#include "UtOptional.hpp"
class UtOrbitalElements;
#include "UtOrbitalPropagatorBase.hpp"
namespace ut
{
class OrbitalState;
}
class WsfAttitudeController;
class WsfConjunctionInput;
class WsfDraw;
#include "WsfGeoPoint.hpp"
#include "WsfMover.hpp"
class WsfOrbitalEvent;
class WsfOrbitalManeuver;
class WsfOrbitalManeuvering;
class WsfOrbitalMissionSequence;
class WsfScriptContext;
#include "WsfSpaceComponentRoles.hpp"
#include "WsfSpaceOrbitalMissionSimulationContext.hpp"
#include "WsfSpaceVerifyTypes.hpp"
#include "WsfStringId.hpp"

//! WsfSpaceMoverBase is a base class for Space-Based movers
//! (in the WSF project these are WsfSpaceMover and WsfNORAD_SpaceMover).
//! Notably, this class aggregates the orbital propagator object as a
//! UtOrbtialPropagatorBase*.  The way to implement a derived class with
//! a derived mover is to instantiate the derived mover in the constructor, as follows:
//! \code
//! class MyDerivedPropagator : public UtOrbitalPropagagorBase
//! {
//!    ...
//! };
//!
//! class MyDerivedSpaceMover : public WsfSpaceMoverBase
//! {
//!    public:
//!      MyDerivedSpaceMover()
//!         : WsfSpaceMoverBase()
//!      {
//!         mPropagatorPtr = new MyDerivedPropagator();
//!         ...
//!      }
//! \endcode
class WSF_SPACE_EXPORT WsfSpaceMoverBase : public WsfMover
{
public:
   WsfSpaceMoverBase() = delete;
   WsfSpaceMoverBase(const WsfScenario&                       aScenario,
                     std::unique_ptr<UtOrbitalPropagatorBase> aPropagatorPtr,
                     const std::string&                       aScriptClassName = "WsfSpaceMover");
   WsfSpaceMoverBase(const WsfSpaceMoverBase& aSrc);
   WsfSpaceMoverBase& operator=(const WsfSpaceMoverBase&) = delete;
   ~WsfSpaceMoverBase() override;

   //! @name Common framework methods.
   //@{
   WsfMover*  Clone() const override;
   const int* GetComponentRoles() const override;
   void*      QueryInterface(int aRole) override;
   bool       PreInitialize(double aSimTime) override;
   bool       Initialize(double aSimTime) override;
   bool       Initialize2(double aSimTime) override;
   bool       ProcessInput(UtInput& aInput) override;
   void       Update(double aSimTime) override;
   //@}

   //! @name Mostly pass-through methods that are optionally supported by the maneuvering model.
   //@{
   const ut::OrbitalState& GetOrbitalState() const;
   double                  GetAvailableDeltaV() const;
   double                  GetRequiredDuration(double aDeltaV) const;
   double                  GetRequiredDeltaV(double aDuration) const;
   double GetManeuverDuration(const UtCalendar& aEpoch, const WsfOrbitalManeuver& aManeuver, double aDeltaV) const;
   bool   PerformStagingOperation();
   const ut::CentralBody&  GetCentralBody() const;
   const ut::CentralPoint& GetCentralPoint() const;
   bool                    Maneuver(const UtCalendar& aEpoch, const UtVec3d& aCommandedDeltaV, UtVec3d& aActualDeltaV);
   //@}

   virtual bool SetInitialPoint(double aLat, double aLon, double aAlt, double aSpeed, double aHeading);

   //! @name WsfSpaceMoverBase Initial State Accessors
   //@{
   WSF_SPACE_DEPRECATED WsfGeoPoint GetInitialPositionLLA() const;
   UtVec3d                          GetInitialPositionECI() const;
   UtVec3d                          GetInitialPositionWCS() const;
   UtVec3d                          GetInitialVelocityECI() const;
   UtVec3d                          GetInitialVelocityWCS() const;
   //@}
   const char* GetScriptClassName() const override;

   WsfSpatialDomain GetSpatialDomain() const override { return WSF_SPATIAL_DOMAIN_SPACE; }

   // IsExtrapolating here means that no propulsive forces are occurring.
   // While it is true that during Delta-V maneuver this isn't true, it is predominately so.
   bool IsExtrapolating() const override { return true; }

   //! Return a reference to the orbital propagator associated with this space mover.
   const UtOrbitalPropagatorBase& GetPropagator() const { return *mPropagatorPtr; }

   //! Return a reference to the orbital propagator associated with this space mover (non-const version).
   UtOrbitalPropagatorBase& GetPropagator() { return *mPropagatorPtr; }

   //! Return the maneuvering model for this space mover.
   const WsfOrbitalManeuvering& GetOrbitalManeuvering() const;

   //! Initialize just the orbital maneuvering model.
   bool InitializeOrbitalManeuvering(const UtCalendar& aEpoch);

   //! Return the current maneuver, if any, being performed by this mover.
   const WsfOrbitalEvent* GetCurrentMissionEvent() const;

   //! Return the current mission sequence.
   //!
   //! In the case that no mission sequence is defined on this mover, this method
   //! will return an empty, but valid, object.
   const WsfOrbitalMissionSequence& GetMissionEvents() const;

   //! Set the MissionEvent sequence using a simulation event that executes at the current simulation time
   //! (necessary because a scripted event can schedule a new sequence, thus potentially deleting itself).
   //! Then, initialize and schedule the first event in the new sequence.
   //! @param aEpoch The current epoch, to be used as the basis for scheduling mission.
   //! @param aMissionEventsPtr The new mission events sequence to be executed.
   void ScheduleMission(const UtCalendar& aEpoch, std::unique_ptr<WsfOrbitalMissionSequence> aMissionEventsPtr);

   //! Cancel the current mission. The current mission event will cease to be executed, even though it may not
   //! have actually achieved its goal. Future events will be deleted, and the parent platform will propagate
   //! using its current state.
   void CancelMission();

   //! Verify a given sequence of MissionEvents.
   //! The sequence of MissionEvents outside of the simulation as a check to make sure that constraints are
   //! being met. If the sequence is not verified, error messages are printed and the simulation will not run
   //! until the errors are fixed. In the case of some orbital events that involve checks on other platforms
   //! (WsfIterceptManeuver, WsfRendezvousManeuver, WsfOrbitalScriptEvent, etc.) the verification will terminate
   //! with the check of the non-verifiable event, and the sequence may subsequently fail to completely execute
   //! during runtime.
   //!@param aEpoch The start epoch at which the MissionEvent sequence is to be verified.
   //!@param aSequence The mission sequence to be verified.
   //!@param aPreVerifyOp The operation to perform before each event is verified.
   //!@param aPostVerifyOp The operation to perform after each event is verified.
   bool VerifyMission(const UtCalendar&                 aEpoch,
                      const WsfOrbitalMissionSequence&  aSequence,
                      wsf::space::verify::PreOperation  aPreVerifyOp  = wsf::space::verify::PreOperation{},
                      wsf::space::verify::PostOperation aPostVerifyOp = wsf::space::verify::PostOperation{}) const;

   //! Verify the initial sequence of MissionEvents in this object.
   //! The sequence of MissionEvents is verified outside of the simulation as a check to make sure that
   //! constraints are being met.  If the sequence is not verified, error messages are printed and the simulation
   //! will not run until the errors are fixed.  In the case of some orbital events that involve checks on other
   //! platforms (WsfIterceptManeuver, WsfRendezvousManeuver, WsfOrbitalScriptEvent, etc.) the verification will
   //! terminate with the check of the non-verifiable event, and the sequence may subsequently fail to completely
   //! execute during runtime.
   //!@param aEpoch The start epoch at which the MissionEvent sequence is to be verified.
   bool VerifyMission(const UtCalendar& aEpoch) const;

   //! Return a reference to the attitude controller associated with this space mover
   WsfAttitudeController& GetAttitudeController() { return *mAttitudePtr; }

   //! Return the relative time since the propagator's epoch time.
   //! @note This is not necessarily the same as the current simulation time.
   double GetSimTimeOfEpoch() const { return mSimTimeOfEpoch; }

   //! Return a reference to the script context.
   WsfScriptContext& GetScriptContext() { return *mContextPtr; }

   void UpdatePropagation(const UtCalendar& aEpoch);

   //! Get the current propagation time.
   const UtCalendar& GetCurrentTime() const { return mPropagatorPtr->GetCurrentTime(); }

   //! Get the interval between entries in an event pipe of this mover's elements.
   double GetElementOutputUpdateInterval() const { return 600.0; }

   //! Return the descriptor associated with this mover.
   const std::string& GetDescriptor() const { return mPropagatorPtr->GetDescriptor(); }

   //! Return the designator associated with this mover.
   const std::string& GetDesignator() const { return mPropagatorPtr->GetDesignator(); }

   const ut::OrbitalState& GetInitialOrbitalState() const;

   double GetInitialHeading() const;

   //! A pass-through method to set the initial orbital state.
   void SetInitialOrbitalState(const ut::OrbitalState& aState) { mPropagatorPtr->SetInitialOrbitalState(aState); }

   bool GetFutureLocationWCS(double aSimTime, double aLocationWCS[3]) final;

   //! A class that encapsulates the trajectory of a platform derived from WsfSpaceMover.
   //! It may be used to predict ahead the locations, times or apogee of entities.
   class SpaceModel : public WsfMover::KinematicModel
   {
   public:
      SpaceModel(double                                   aSimTime,
                 WsfStringId                              aModelTypeId,
                 std::unique_ptr<UtOrbitalPropagatorBase> aPropagatorPtr,
                 WsfDraw*                                 aDrawPtr = nullptr); // If we want to draw the trajectory.

      bool GetLocationXYZ(double aTime, double aLocXYZ[3]) const override;

      bool GetVelocityXYZ(double aTime, double aVelXYZ[3]) const override;

   protected:
      bool Update(double aSimTime) const;

      std::unique_ptr<UtOrbitalPropagatorBase> mPropagatorPtr;
      mutable UtCalendar                       mUpdateTime;
      mutable double                           mLastUpdateTime;
      mutable UtVec3d                          mLastLocXYZ;
      mutable UtVec3d                          mLastLocWCS;
      mutable UtVec3d                          mLastVelWCS;
   };

   WsfMover::KinematicModel* CreateKinematicModel(double       aSimTime,
                                                  WsfStringId  aModelTypeId,
                                                  const double aLocationWCS[3],
                                                  const double aVelocityWCS[3],
                                                  WsfMover*    aMoverPtr,
                                                  WsfDraw*     aDrawPtr) const override;

   //! Set the mover's target ECI orientation.
   void SetTargetOrientationECI(double aPsi, double aTheta, double aPhi);

   ut::optional<UtColor> GetOrbitColor() const { return mOrbitColor; }
   UtColor               GetOrbitColorValue() const { return mOrbitColor.value_or(UtColor(1, 1, 1)); }
   bool                  OrbitColorSet() const { return mOrbitColor.has_value(); }
   void                  UpdateColor(double aSimTime, const UtColor& aColor);
   void SetOrbitColor(const std::string& aOrbitColor) { SetOrbitColor(UtColor::GetColorMapping(aOrbitColor)); }
   void SetOrbitColor(const UtColor& aOrbitColor);

   bool IsTLEInputAllowed() const noexcept { return mTLE_InputAllowed; }
   void SetTLEInputAllowed(bool aTLE_InputAllowed) noexcept { mTLE_InputAllowed = aTLE_InputAllowed; }

   //! Return the look angle to the given WCS location.
   //! This returns the angle in radians between the nadir direction and the line connecting this space mover
   //! and the given @p aLocationWCS.
   //! @param aLocationWCS The WCS location of the target point.
   //! @returns The look angle from this space mover to the target point, in radians.
   double GetLookAngleTo(const UtVec3d& aLocationWCS) const;

   //! Return the incidence angle for the given WCS location.
   //! This returns the angle in radians between the local vertical direction at @p aLocationWCS and the line
   //! connecting this space mover and the given @p aLocationWCS.
   //! @param aLocationWCS The WCS location of the target point.
   //! @returns The incidence angle in radians.
   double GetIncidenceAngle(const UtVec3d& aLocationWCS) const;

   //! Return the target elevation angle for the given WCS location.
   //! This returns the angle in radians between the local horizontal direction at @p aLocationWCS and the
   //! line connecthing this space mover and the given @p aLocationWCS.
   //! @param aLocationWCS The WCS location of the target point.
   //! @returns The target elevation angle in radians.
   double GetTargetElevationAngle(const UtVec3d& aLocationWCS) const;

   //! Return the squint angle to the given WCS location.
   //! This returns the angle between this mover's WCS velocity vector projected into the local horizontal plane,
   //! and the WCS displacement vector from this mover to the given @p aLocationWCS projected into the local horizontal
   //! plane.
   //! @param aLocationWCS The WCS location of the target point.
   //! @returns The squint angle to the target point, in radians.
   double GetSquintAngleTo(const UtVec3d& aLocationWCS) const;

   //! Return distance in meters from the mover to the central body's horizon.
   //! If the mover's position is inside the mean radius of the central body, this will return -1.
   //! @returns Distance from the mover to the horizon in meters; or -1 if the mover is inside the central body.
   double GetDistanceToHorizon() const;

   //! Return look angle in radians to the central body's horizon.
   //! If the mover's position is inside the mean radius of the central body, this will return -2 pi.
   //! @returns Look angle to the horizon in radians; or -2 pi if the mover is inside the central body.
   double GetLookAngleToHorizon() const;

private:
   bool AddDeltaV(const UtVec3d& aDeltaV_ECI);

   ut::CloneablePtr<UtOrbitalPropagatorBase>   mPropagatorPtr;
   ut::CloneablePtr<WsfOrbitalManeuvering>     mManeuveringPtr;
   ut::CloneablePtr<WsfAttitudeController>     mAttitudePtr;
   ut::CloneablePtr<WsfConjunctionInput>       mConjunctionInputPtr;
   wsf::space::OrbitalMissionSimulationContext mMissionContext;
   double                                      mSimTimeOfEpoch{0.0};
   WsfScriptContext*                           mContextPtr;
   ut::OrbitalState                            mOrbitalState;
   ut::optional<UtColor>                       mOrbitColor;

   bool mTLE_InputAllowed{false};
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfSpaceMoverBase, cWSF_COMPONENT_SPACE_MOVER)

#endif
