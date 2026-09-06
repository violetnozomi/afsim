// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFTARGETPOINTOPTIONS_HPP
#define WSFTARGETPOINTOPTIONS_HPP

#include <sstream>
#include <stdexcept>
#include <type_traits>

#include "UtCloneablePtr.hpp"
#include "UtLog.hpp"
class UtInput;
#include "wsf_space_export.h"

#include "UtOrbitalPropagatorBase.hpp"
#include "WsfLibrationPoint.hpp"
#include "WsfLibrationTargetPoint.hpp"
#include "WsfOrbitalManeuver.hpp"
#include "WsfOrbitalReferenceFrame.hpp"
#include "WsfOrbitalTargetPoint.hpp"
#include "WsfPropagatorTargetPoint.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrackId.hpp"

namespace wsf
{
namespace space
{

//! This class reads in the options for targets for a targeting-class maneuver.
//!
//! The set of targets that are possible to target with a targeting-class maneuver
//! (Target, Intercept, Rendezvous) can be configured with this object. This object
//! will read in from input the target specification, and can create the appropriate
//! wsf::space::OrbitalTargetPoint to represent that target. Targets are specified
//! as both a base target (such as a target platform) and an optional offset from
//! that target.
class WSF_SPACE_EXPORT TargetPointOptions
{
public:
   explicit TargetPointOptions(const WsfTrackId& aLocalTrackId);
   TargetPointOptions(LibrationPoint::System aSystem, LibrationPoint::Point aPoint);
   TargetPointOptions(const UtVec3d& aPosition, const UtVec3d& aVelocity);
   TargetPointOptions(const TargetPointOptions& aOther) = default;
   ~TargetPointOptions()                                = default;

   enum class Type
   {
      cPLATFORM,
      cTRACK,
      cLIBRATION_POINT,
      cPROVIDED_TARGET,
      cKINEMATIC_STATE,
   };

   bool ProcessInput(UtInput& aInput);

   std::unique_ptr<OrbitalTargetPoint> InstantiatePoint(const UtCalendar& aEpoch) const;

   //! Return if this object is configured with a platform target.
   bool HasPlatformTarget() const { return mTargetType == Type::cPLATFORM; }

   //! Return if this object is configured with a track-based target.
   bool HasTrackTarget() const { return mTargetType == Type::cTRACK; }

   //! Return if this object is configured with a libration point target.
   bool HasLibrationPointTarget() const { return mTargetType == Type::cLIBRATION_POINT; }

   //! Return if this object is configured with a provided target point.
   bool HasProvidedTarget() const { return mTargetType == Type::cPROVIDED_TARGET; }

   //! Return if this object is configured with a certain kinematic state as the target.
   bool HasKinematicStateTarget() const { return mTargetType == Type::cKINEMATIC_STATE; }

   //! Return if this objectis configured with a valid target.
   bool IsValid() const;

   //! Return if this object is configured with a position offset.
   bool HasPositionOffset() const { return mHasPositionOffset; }

   //! Return if this object is configured with a velocity offset.
   bool HasVelocityOffset() const { return mHasVelocityOffset; }

   //! Return if this object is configured with a position or velocity offset.
   bool HasKinematicOffset() const { return HasPositionOffset() || HasVelocityOffset(); }

   //! Return the position offset in meters with which this object is configured.
   const UtVec3d& GetPositionOffset() const { return mPositionOffset; }

   //! Return the velocity offset in meters per second with which this object is configured.
   const UtVec3d& GetVelocityOffset() const { return mVelocityOffset; }

   //! Return a string giving the units with which this object's position offset was configured.
   const std::string& GetPositionOffsetUnits() const { return mPositionOffsetUnits; }

   //! Return a string giving the units with which this object's velocity offset was configured.
   const std::string& GetVelocityOffsetUnits() const { return mVelocityOffsetUnits; }

   //! Return the frame in which this object's position offset is specified.
   wsf::space::OrbitalReferenceFrame GetPositionOffsetFrame() const { return mPositionOffsetFrame; }

   //! Return the frame in which this object's velocity offset is specified.
   wsf::space::OrbitalReferenceFrame GetVelocityOffsetFrame() const { return mVelocityOffsetFrame; }

   //! Return the position offset in the units this object was configured with.
   UtVec3d GetPositionOffsetInUnits() const;

   //! Return the velocity offset in the units this object was configured with.
   UtVec3d GetVelocityOffsetInUnits() const;

   //! Return the name of the platform being targeted. Valid only when HasPlatformTarget returns true.
   const std::string& GetPlatformName() const { return mPlatformName; }

   //! Return the track id of the track being targeted. Valid only when HasTrackTarget returns true.
   const WsfTrackId& GetLocalTrackId() const { return mLocalTrackId; }

   //! Return the libration point system being targeted. Valid only when HasLibrationPointTarget returns true.
   wsf::space::LibrationPoint::System GetLP_System() const { return mLP_System; }

   //! Return the libration point being targeted. Valid only when HasLibrationPointTarget returns true.
   wsf::space::LibrationPoint::Point GetLP_Point() const { return mLP_Point; }

   //! Return the position in meters of the kinematic state being targeted. Valid only when HasKinematicStateTarget returns true.
   const UtVec3d& GetKinematicStatePosition() const { return mKinematicStatePosition; }

   //! Return the position of the kinematic state being targeted, in the units with which this object was configured.
   //! Valid only when HasKinematicStateTarget returns true.
   UtVec3d GetKinematicStatePositionInUnits() const;

   //! Return a string giving the units with which this object's kinematic state target position was configured. Valid
   //! only when HasKinematicStateTarget returns true.
   const std::string& GetKinematicStatePositionUnits() const { return mKinematicStatePositionUnits; }

   //! Return the velocity in meters per second of the kinematic state being targeted. Valid only when
   //! HasKinematicStateTarget returns true.
   const UtVec3d& GetKinematicStateVelocity() const { return mKinematicStateVelocity; }

   //! Return the velocity of the kinematic state being targeted, in the units with which this object was configured.
   //! Valid only when HasKinematicStateTarget returns true.
   UtVec3d GetKinematicStateVelocityInUnits() const;

   //! Return a string giving the units with which this object's kinematic state target velocity was configured. Valid
   //! only when HasKinematicStateTarget returns true.
   const std::string& GetKinematicStateVelocityUnits() const { return mKinematicStateVelocityUnits; }

   //! Return the target propagator defined on this object.
   const UtOrbitalPropagatorBase* GetTargetPropagator() const { return mTargetPropagator.get(); }

   //! Set the position offset.
   //!
   //! Set the offset to be the given @p aOffset which is provided with the given
   //! @p aOffsetUnits and in the given @p aOffsetFrame.
   //!
   //! \param aOffset - The vector of components of the offset.
   //! \param aOffsetUnits - The units with which the given @p aOffset are expressed.
   //! \param aOffsetFrame - The frame in which the given @p aOffset are given.
   void SetPositionOffset(const UtVec3d&                    aOffset,
                          const std::string&                aOffsetUnits,
                          wsf::space::OrbitalReferenceFrame aOffsetFrame);

   //! Set the velocity offset.
   //!
   //! Set the offset to be the given @p aOffset which is provided with the given
   //! @p aOffsetUnits and in the given @p aOffsetFrame.
   //!
   //! \param aOffset - The vector of components of the offset.
   //! \param aOffsetUnits - The units with which the given @p aOffset are expressed.
   //! \param aOffsetFrame - The frame in which the given @p aOffset are given.
   void SetVelocityOffset(const UtVec3d&                    aOffset,
                          const std::string&                aOffsetUnits,
                          wsf::space::OrbitalReferenceFrame aOffsetFrame);

   //! Clear the position offset from this object's configuration.
   void ClearPositionOffset();

   //! Clear the velocity offset from this object's configuration.
   void ClearVelocityOffset();

   //! Return if this object has an offset time configured.
   bool HasOffsetTime() const { return mHasOffsetTime; }

   //! Get the offset time configured for this object.
   //! The result is only valid if HasOffsetTime returns true.
   const UtTimeValue& GetOffsetTime() const { return mOffsetTime; }

   //! Set the offset time configured for this object.
   void SetOffsetTime(const UtTimeValue& aTimeOffset);

   //! Clear the offset time from this object's configuration.
   void ClearOffsetTime();

   //! Return if this object has a lag time configured.
   bool HasLagTime() const { return mHasLagTime; }

   //! Get the lag time configured for this object.
   //! The result is only valid if HasLagTime returns true.
   const UtTimeValue& GetLagTime() const { return mLagTime; }

   //! Set the lag time configured for this object.
   void SetLagTime(const UtTimeValue& aLagTime);

   //! Clear the lag time from this object's configuration.
   void ClearLagTime();

   //! Set a platform with the given name as the target.
   void SetPlatformName(const std::string& aPlatformName);

   //! Set a track with the given ID as the target.
   void SetLocalTrackId(const WsfTrackId& aTrackId);

   //! Set the given libration point as the target.
   void SetLibrationPoint(wsf::space::LibrationPoint::System aSystem, wsf::space::LibrationPoint::Point aPoint);

   //! Set the given target point as the base target.
   void SetTargetPoint(std::unique_ptr<wsf::space::OrbitalTargetPoint> aTargetPoint);

   //! Set the given position and velocity as the base target.
   void SetKinematicState(const UtVec3d&     aPositionInertial,
                          const std::string& aPositionUnits,
                          const UtVec3d&     aVelocityInertial,
                          const std::string& aVelocityUnits);

   //! Return if these options imply a need for a targeting propagator to successfully instantiate the target point.
   bool RequiresTargetPropagator() const { return mTargetType == Type::cPLATFORM || mTargetType == Type::cTRACK; }

   //! Set the target propagator.
   void SetTargetPropagator(std::unique_ptr<UtOrbitalPropagatorBase> aTargetPropagatorPtr)
   {
      mTargetPropagator = std::move(aTargetPropagatorPtr);
   }

   //! Return if these options imply a need for a maneuvering propagator to successfully instantiate the target point.
   bool RequiresManeuveringPropagator() const { return HasLagTime() || HasOffsetTime(); }

   //! Set the maneuvering propagator.
   void SetManeuveringPropagator(std::unique_ptr<UtOrbitalPropagatorBase> aManeuveringPropagatorPtr)
   {
      mManeuveringPropagator = std::move(aManeuveringPropagatorPtr);
   }

   //! Validate that the configuration has parameters in the allowed range.
   bool ValidateParameterRanges(const std::string& aExecutingPlatformName, const std::string& aManeuverType) const;

   //! Returns if this object supports the given frame for offsets.
   static bool ValidOffsetFrame(wsf::space::OrbitalReferenceFrame aFrame);

private:
   std::unique_ptr<OrbitalTargetPoint> FormInitialTarget(const UtCalendar& aEpoch) const;
   std::unique_ptr<OrbitalTargetPoint> FormOffsetTarget(std::unique_ptr<OrbitalTargetPoint> aPointPtr) const;
   std::unique_ptr<OrbitalTargetPoint> FormKinematicOffsetTarget(std::unique_ptr<OrbitalTargetPoint> aTargetPtr) const;

   Type                               mTargetType{Type::cTRACK};
   UtVec3d                            mPositionOffset{};
   std::string                        mPositionOffsetUnits{};
   wsf::space::OrbitalReferenceFrame  mPositionOffsetFrame{wsf::space::OrbitalReferenceFrame::cUNKNOWN};
   UtVec3d                            mVelocityOffset{};
   std::string                        mVelocityOffsetUnits{};
   wsf::space::OrbitalReferenceFrame  mVelocityOffsetFrame{wsf::space::OrbitalReferenceFrame::cUNKNOWN};
   UtTimeValue                        mOffsetTime{0.0};
   UtTimeValue                        mLagTime{0.0};
   std::string                        mPlatformName{};
   WsfTrackId                         mLocalTrackId{};
   wsf::space::LibrationPoint::System mLP_System{wsf::space::LibrationPoint::System::cUNKNOWN};
   wsf::space::LibrationPoint::Point  mLP_Point{wsf::space::LibrationPoint::Point::cUNKNOWN};
   UtVec3d                            mKinematicStatePosition{};
   std::string                        mKinematicStatePositionUnits{};
   UtVec3d                            mKinematicStateVelocity{};
   std::string                        mKinematicStateVelocityUnits{};
   mutable ut::CloneablePtr<wsf::space::OrbitalTargetPoint> mTargetPoint{nullptr};
   mutable ut::CloneablePtr<UtOrbitalPropagatorBase>        mTargetPropagator{nullptr};
   ut::CloneablePtr<UtOrbitalPropagatorBase>                mManeuveringPropagator{nullptr};
   bool                                                     mHasPositionOffset{false};
   bool                                                     mHasVelocityOffset{false};
   bool                                                     mHasOffsetTime{false};
   bool                                                     mHasLagTime{false};
};

} // namespace space
} // namespace wsf

#endif // WSFTARGETPOINTOPTIONS_HPP
