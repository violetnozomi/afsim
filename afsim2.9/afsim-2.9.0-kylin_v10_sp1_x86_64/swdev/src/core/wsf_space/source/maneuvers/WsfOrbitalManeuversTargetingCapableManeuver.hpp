// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFORBITALMANEUVERTARGETINGCAPABLEMANEUVER_HPP
#define WSFORBITALMANEUVERTARGETINGCAPABLEMANEUVER_HPP

#include <memory>

class UtCalendar;
class UtInput;
class UtTimeValue;
#include "UtVec3.hpp"
#include "WsfLibrationPoint.hpp"
#include "WsfOrbitalManeuver.hpp"
#include "WsfOrbitalReferenceFrame.hpp"
#include "WsfOrbitalTargetPoint.hpp"
#include "WsfTargetPointOptions.hpp"
class WsfTrackId;

namespace WsfOrbitalManeuvers
{
//! A maneuver that uses an OrbitalTargetPoint in determining the effect of the maneuver.
//!
//! This class is not intended to be instantiated, but is instead intended to supply common
//! functionality for maneuvers that require targeting. This class supplies the capability
//! of interpreting a 'target' input block, and will translate that target specification
//! into the needed OrbitalTargetPoint upon initialization of this maneuver.
class TargetingCapableManeuver : public WsfOrbitalManeuver
{
public:
   explicit TargetingCapableManeuver(const WsfScenario& aScenario, const WsfTrackId& aLocalTrackId)
      : WsfOrbitalManeuver{aScenario}
      , mTargetOptions{aLocalTrackId}
   {
   }

   explicit TargetingCapableManeuver(const WsfScenario& aScenario, const wsf::space::TargetPointOptions& aOptions)
      : WsfOrbitalManeuver{aScenario}
      , mTargetOptions{aOptions}
   {
   }

   TargetingCapableManeuver(const TargetingCapableManeuver& aOther)
      : WsfOrbitalManeuver(aOther)
      , mTargetOptions{aOther.mTargetOptions}
   {
   }

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext) override;
   bool ValidateParameterRanges(const std::string& aExecutingPlatformName) const override;

   //! Return the track Id that this maneuver is targeting.
   const WsfTrackId& GetLocalTrackId() const { return mTargetOptions.GetLocalTrackId(); }

   //! Return the name of the platform that this maneuver is targeting.
   WsfStringId GetPlatformName() const { return mTargetOptions.GetPlatformName(); }

   //! Return if a position offset is defined for this maneuver.
   bool HasOffset() const { return mTargetOptions.HasPositionOffset(); }

   //! Return the position offset in meters defined for this maneuver.
   UtVec3d GetOffset() const { return mTargetOptions.GetPositionOffset(); }

   //! Return the units in which the position offset is defined for this maneuver.
   const std::string& GetOffsetUnit() const { return mTargetOptions.GetPositionOffsetUnits(); }

   //! Return the position offset defined for this maneuver using the units specified on setup of this object.
   UtVec3d GetOffsetInUnits() const { return mTargetOptions.GetPositionOffsetInUnits(); }

   //! Return if a velocity offset is defined for this meaneuver.
   bool HasVelocityOffset() const { return mTargetOptions.HasVelocityOffset(); }

   //! Return the velocity offset in meters per second defined for this maneuver.
   UtVec3d GetVelocityOffset() const { return mTargetOptions.GetVelocityOffset(); }

   //! Return the units in which the velocity offset is defined for this maneuver.
   const std::string& GetVelocityOffsetUnit() const { return mTargetOptions.GetVelocityOffsetUnits(); }

   //! Return the velocity offset defined for this maneuver using the units specified on setup of this object.
   UtVec3d GetVelocityOffsetInUnits() const { return mTargetOptions.GetVelocityOffsetInUnits(); }

   //! Return the offset time defined for this maneuver.
   UtTimeValue GetTargetOffsetTime() const { return mTargetOptions.GetOffsetTime(); }

   //! Return the lag time defined for this maneuver.
   UtTimeValue GetTargetLagTime() const { return mTargetOptions.GetLagTime(); }

   //! Set the local track Id that this maneuver is targeting.
   void SetLocalTrackId(const WsfTrackId& aTrackId) { mTargetOptions.SetLocalTrackId(aTrackId); }

   //! Set the name of the platform that this maneuver is targeting.
   void SetPlatformName(WsfStringId aPlatformName) { mTargetOptions.SetPlatformName(aPlatformName); }

   //! Set a position offset in meters for this targeting operation.
   void SetOffset(const UtVec3d& aOffset, wsf::space::OrbitalReferenceFrame aFrame)
   {
      mTargetOptions.SetPositionOffset(aOffset, "m", aFrame);
   }

   //! Set a position offset in the given units for this targeting operation.
   void SetOffsetInUnits(const UtVec3d& aOffset, const std::string& aUnit, wsf::space::OrbitalReferenceFrame aFrame)
   {
      mTargetOptions.SetPositionOffset(aOffset, aUnit, aFrame);
   }

   //! Set a velocity offset in meters per second for this targeting operation.
   void SetVelocityOffset(const UtVec3d& aOffset, wsf::space::OrbitalReferenceFrame aFrame)
   {
      mTargetOptions.SetVelocityOffset(aOffset, "m/s", aFrame);
   }

   //! Set a velocity offset in the given users for this targeting operation.
   void SetVelocityOffsetInUnits(const UtVec3d& aOffset, const std::string& aUnit, wsf::space::OrbitalReferenceFrame aFrame)
   {
      mTargetOptions.SetVelocityOffset(aOffset, aUnit, aFrame);
   }

   //! Set an offset time for this targeting operation.
   void SetTargetOffsetTime(const UtTimeValue& aTimeOffset) { mTargetOptions.SetOffsetTime(aTimeOffset); }

   //! Set a lag time for this targeting operation.
   void SetTargetLagTime(const UtTimeValue& aLagTime) { mTargetOptions.SetLagTime(aLagTime); }

   //! Set a libration point target.
   void SetLibrationPoint(wsf::space::LibrationPoint::System aSystem, wsf::space::LibrationPoint::Point aPoint)
   {
      mTargetOptions.SetLibrationPoint(aSystem, aPoint);
   }

   //! Set a kinematic state target.
   void SetKinematicStateTarget(const UtVec3d&     aPositionInertial,
                                const std::string& aPositionUnits,
                                const UtVec3d&     aVelocityInertial,
                                const std::string& aVelocityUnits)
   {
      mTargetOptions.SetKinematicState(aPositionInertial, aPositionUnits, aVelocityInertial, aVelocityUnits);
   }

   //! Set this maneuver to target the provided target point.
   void SetTargetPoint(std::unique_ptr<wsf::space::OrbitalTargetPoint> aTargetPointPtr)
   {
      mTargetOptions.SetTargetPoint(std::move(aTargetPointPtr));
   }

   //! Set the options of this maneuver to the given.
   void SetTargetPointOptions(const wsf::space::TargetPointOptions& aOptions) { mTargetOptions = aOptions; }

   //! Get the targeting options configured on this maneuver.
   const wsf::space::TargetPointOptions& GetTargetPointOptions() const { return mTargetOptions; }

   //! Get the target point that this maneuver will use to provide targeting information.
   //!
   //! If this object has not been initialized, the return value may be null.
   const wsf::space::OrbitalTargetPoint* GetTargetPoint() const { return mTargetPtr.get(); }

   //! Get the propagator representing the target of this maneuver.
   //!
   //! This is either the true propagator of the platform names in this objects options,
   //! or the propagator representing the knowledge contained in the local track specified
   //! as the target of this maneuver.
   const UtOrbitalPropagatorBase* GetTargetPropagator(const wsf::space::OrbitalMissionContext& aContext) const;

   //! Update this objects target point to the given epoch.
   void UpdateTargetPoint(const UtCalendar& aEpoch) const;

private:
   std::unique_ptr<wsf::space::OrbitalTargetPoint> mTargetPtr{nullptr};
   wsf::space::TargetPointOptions                  mTargetOptions;
};

} // namespace WsfOrbitalManeuvers

#endif // WSFORBITALMANEUVERTARGETINGCAPABLEMANEUVER_HPP
