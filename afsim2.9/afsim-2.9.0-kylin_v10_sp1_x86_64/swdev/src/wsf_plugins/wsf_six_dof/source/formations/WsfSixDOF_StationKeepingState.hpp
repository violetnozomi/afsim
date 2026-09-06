// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSF_SIXDOF_STATIONKEEPINGSTATE_HPP
#define WSF_SIXDOF_STATIONKEEPINGSTATE_HPP

#include "WsfSixDOF_FormUpKeepStationManeuver.hpp"
#include "maneuvers/WsfSixDOF_RelativeManeuverState.hpp"

namespace wsf
{
namespace six_dof
{
//! The base type for the station keeping relative maneuver states.
class FormUpStationKeepingManeuverState : public RelativeManeuverState
{
public:
   FormUpStationKeepingManeuverState(const RelativeManeuver::Data& aData, const FormUpKeepStationManeuver& aManeuver)
      : mData(aData)
      , mManeuver(aManeuver)
   {
   }

protected:
   const RelativeManeuver::Data&    mData;     //! Reference to the owning maneuver's data.
   const FormUpKeepStationManeuver& mManeuver; //! Reference to the owning maneuver.
};

//! The form up state provides behavior for the chaser as it approaches its target location.
//!
//! During form up, the chaser will fly toward the target location based on the details of
//! its position in the formation of which it is a member. When the chaser gets close enough
//! to its target location, it will transition into the KeepStationState.
class FormUpState : public FormUpStationKeepingManeuverState
{
public:
   FormUpState(const RelativeManeuver::Data& aData, const FormUpKeepStationManeuver& aManeuver)
      : FormUpStationKeepingManeuverState(aData, aManeuver)
   {
   }

   ~FormUpState() override = default;

   std::unique_ptr<RelativeManeuverState> Evaluate() override;
   double                                 GetEvaluationInterval() const override;

private:
   double ComputeRollAngleDeg(const UtEntity& aEntity) const;
   double ComputeSpeedKTAS() const;
};

//! The keep station state provides behavior for the chases to settle it into its station, and keep it there.
//!
//! During station keeping the chaser will typically execute smaller motions to keep it near its
//! designated station. If the chaser ever fails to maintain station, the maneuver will transition back
//! into FormUpState. If the station for this member platform ever is moving too fast or slow relative
//! to the formation leader (for example, during some turns), the maneuver will transition into
//! PursueState.
class KeepStationState : public FormUpStationKeepingManeuverState
{
public:
   KeepStationState(const RelativeManeuver::Data& aData, const FormUpKeepStationManeuver& aManeuver)
      : FormUpStationKeepingManeuverState(aData, aManeuver)
   {
   }

   ~KeepStationState() override = default;

   std::unique_ptr<RelativeManeuverState> Evaluate() override;
   double                                 GetEvaluationInterval() const override;

private:
   void   ComputeCoordinates(UtVec3d& aDeltaLoc, UtVec3d& aDeltaVel, UtVec3d& aDeltaAcc) const;
   double ComputeRollAngleDeg(const UtVec3d& aDeltaLoc, const UtVec3d& aDeltaVel, const UtVec3d& aDeltaAcc) const;
   double ComputeG_Load(const UtVec3d& aDeltaLoc, const UtVec3d& aDeltaVel) const;
   double ComputeSpeedKTAS(const UtVec3d& aDeltaLoc, const UtVec3d& aDeltaVel, const UtVec3d& aDeltaAcc) const;

   int mExitCount{0}; //! Avoid spurious rapid state switching.
};

//! The pursue state provides behavior for the chaser to get the chaser onto the turn circle of the leader.
//!
//! During the pursue phase, the chaser falls into line behind the formation's leader. This is to avoid
//! excessive differences in speeds for the chaser relative to the leader. Once the motion of the chaser's
//! station returns to being inside the allowed speed ranges, this maneuver will transition back to
//! KeepStationState.
class FormationPursueState : public FormUpStationKeepingManeuverState
{
public:
   FormationPursueState(const RelativeManeuver::Data& aData, const FormUpKeepStationManeuver& aManeuver)
      : FormUpStationKeepingManeuverState(aData, aManeuver)
   {
   }

   std::unique_ptr<RelativeManeuverState> Evaluate() override;
   double                                 GetEvaluationInterval() const override;

   void SetTrailingDistance(double aDistance) { mTrailingDistance = aDistance; }

private:
   double ComputeRollAngle() const;
   double ComputeG_Load() const;
   double ComputeSpeedKTAS() const;

   void ComputeOffsets();

   UtVec3d mDeltaLoc{};              //! Displacement from chaser to target in chaser's ECS frame.
   UtVec3d mDeltaVel{};              //! Relative velocity of target in chaser's ECS frame.
   double  mTargetSpeed{};           //! Speed of the target.
   double  mTrailingDistance{100.0}; //! Distance behind the target on the turn circle to fly to.
   int     mExitCount{0};            //! Avoid spurious rapid state switching.
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_STATIONKEEPINGSTATE_HPP
