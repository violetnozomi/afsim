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

#ifndef WSF_SIXDOF_RELATIVEMANEUVER_HPP
#define WSF_SIXDOF_RELATIVEMANEUVER_HPP

#include "wsf_six_dof_export.h"

#include <limits>
#include <memory>
#include <string>

#include "UtEntity.hpp"
#include "UtVec3.hpp"
#include "WsfSixDOF_Maneuver.hpp"
#include "WsfSixDOF_ManeuverTurnCircle.hpp"
#include "WsfSixDOF_RelativeManeuverState.hpp"
#include "WsfWaypoint.hpp"

class WsfPlatform;

namespace wsf
{
namespace six_dof
{
class Mover;

//! The needed kinematic state for relative maneuvers
struct RelativeManeuverKinematics
{
   WsfWaypoint        GetWaypoint() const;
   UtEntity           GetEntity() const;
   ManeuverTurnCircle GetTurnCircle() const;

   UtVec3d mLocWCS{0.0, 0.0, 0.0};
   UtVec3d mVelWCS{0.0, 0.0, 0.0};
   UtVec3d mAccWCS{0.0, 0.0, 0.0};
   UtVec3d mAnglesNED{0.0, 0.0, 0.0};
   double  mG_Load{1.0};
};

//! A maneuver type specialized to motions relative to another platform.
//!
//! Relative maneuvers are implemented as state machines with each state providing the
//! control inputs for the mover that is executing this maneuver. Some relative maneuvers
//! may need to operate in a number of possible states, while some might need only a
//! single state.
class WSF_SIX_DOF_EXPORT RelativeManeuver : public Maneuver
{
public:
   explicit RelativeManeuver(std::unique_ptr<RelativeManeuverState> aInitialStatePtr);
   RelativeManeuver(const RelativeManeuver& aOther);
   ~RelativeManeuver() override = default;

   RelativeManeuver& operator=(const RelativeManeuver& aOther) = delete;

   const char* GetScriptClassName() const override { return "WsfSixDOF_RelativeManeuver"; }

   double Execute(double aSimTime) override;

   //! This method is implemented by subclasses and will update mData as needed for that
   //! subclass. It will return true if the update is successful, or false if there was
   //! some problem.
   virtual bool UpdateData() = 0;

   //! Get the maximum allowed g-load.
   double GetG_LoadMax() const { return mG_LoadMax; }

   //! Set the maximum allowed g-load.
   void SetG_LoadMax(double aG_Load) { mG_LoadMax = aG_Load; }

   //! Get the minimum allowed g-load.
   double GetG_LoadMin() const { return mG_LoadMin; }

   //! Set the minimum allowed g-load.
   void SetG_LoadMin(double aG_Load) { mG_LoadMin = aG_Load; }

   //! Get the maximum allowed speed in meters per second.
   double GetSpeedMpsMax() const { return mSpeedMpsMax; }

   //! Set the maximum allowed speed in meters per second.
   void SetSpeedMpsMax(double aSpeedMps) { mSpeedMpsMax = aSpeedMps; }

   //! Get the minimum allowed speed in meters per second.
   double GetSpeedMpsMin() const { return mSpeedMpsMin; }

   //! Set the minimum allowed speed in meters per second.
   void SetSpeedMpsMin(double aSpeedMps) { mSpeedMpsMin = aSpeedMps; }

   double LimitSpeed(double aSpeedMps) const;
   bool   IsSpeedInsideAllowedRange(double aSpeedMps) const;
   double LimitG_Load(double aG_Load) const;

   //! A minimal set of data needed by relative maneuvers.
   struct Data
   {
      RelativeManeuverKinematics mKinematics;        //! The kinematics of the target.
      WsfPlatform*               mChaserPlatformPtr; //! The chasing platform.
      Mover*                     mChaserMoverPtr;    //! The chasing platform's mover.
      UtVec3d                    mChaserLocWCS;      //! The location of the chaser.
      UtVec3d                    mSeparationWCS;     //! The WCS separation between the chaser and the target.
   };

protected:
   void FinalControls() const;

   Data                                   mData{};
   std::unique_ptr<RelativeManeuverState> mStatePtr;
   double                                 mG_LoadMax{std::numeric_limits<double>::max()};
   double                                 mG_LoadMin{-std::numeric_limits<double>::max()};
   double                                 mSpeedMpsMax{std::numeric_limits<double>::max()};
   double                                 mSpeedMpsMin{0.0};
};
} // namespace six_dof
} // namespace wsf

#endif // WS_SIXDOF_FRELATIVEMANEUVER_HPP
