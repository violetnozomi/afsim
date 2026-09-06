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

#ifndef WSFTBM_MOVER_HPP
#define WSFTBM_MOVER_HPP

#include "wsf_mil_export.h"

#include <vector>

#include "TblLookup.hpp"
#include "UtLog.hpp"
#include "UtRandom.hpp"
#include "WsfMover.hpp"

//! A simple theater/tactical ballistic missile (TBM) mover.
//!
//! The maximum range (and the launch angle needed to reach that range)
//! of the TBM is given by the following input characteristics:
//!
//!     - TBM burn time (sec)
//!     - TBM launch time (sec)
//!     - TBM ballistic coefficient (n/m^2)
//!     - TBM fuel mass fraction
//!     - TBM launch mass (kg)
//!     - TBM specific impulse (sec)
//!     - lofted or depressed trajectory
//!
//! If the desired TBM range is less than the maximum range then the launch
//! angle required to go the desired range is computed.  If the trajectory
//! is lofted the launch angle will be greater than the maximum range launch
//! angle, and if the trajectory is depressed the launch angle will be less
//! than the maximum range launch angle.
//!
//! The thrust phase of the TBM flight is not modeled exactly as a real TBM
//! would fly.  Actual TBMs tend to launch vertically and then use some
//! guidance law during boost to achieve the proper burnout conditions to
//! hit their target.  Each type of TBM would implement this in a different
//! way, but the boost trajectories produced are fairly similar.  In this
//! model the TBM is not launched vertically, but at some angle slightly
//! less.  The direction of thrust is assumed to always be opposite the TBM
//! velocity vector.  Given these conditions, the earth's gravity will cause
//! the TBM to turn.  By selecting the correct launch angle the desired
//! impact range will result.  The trajectories produced are very similar to
//! real ICBM trajectories.
//!
//! Drag is modeled below 200000 meters.  The TBM is assumed to have a
//! constant beta, the coefficient of drag does not vary with Mach number.
//! The air density is modeled using an exponential approximation.

class WSF_MIL_EXPORT WsfTBM_Mover : public WsfMover
{
public:
   WsfTBM_Mover(WsfScenario& aScenario);
   ~WsfTBM_Mover() override = default;
   WsfTBM_Mover& operator=(const WsfTBM_Mover&) = delete;

   WsfMover* Clone() const override;
   bool      Initialize(double aSimTime) override;
   bool      Initialize2(double aSimTime) override;
   bool      ProcessInput(UtInput& aInput) override;
   void      Update(double aSimTime) override;

   static ut::Random& GetRandom();
   static void        ClearRandom();

   WsfSpatialDomain GetSpatialDomain() const override { return WSF_SPATIAL_DOMAIN_AIR; }

   double GetTBMLastUpdateTime() const { return WsfTBM_Mover::mLastUpdateTime; }
   void   SetTBMLastUpdateTime(double aLastUpdateTime) { WsfTBM_Mover::mLastUpdateTime = aLastUpdateTime; }

   void GetLaunchLLA(double& aLat, double& aLon, double& aAlt, bool& aAltAGL);
   void SetLaunchLLA(double aLat, double aLon, double aAlt, bool aAltAGL);

   double GetLaunchTime() const { return mLaunchTime; }
   void   SetLaunchTime(double aLaunchTime) { mLaunchTime = aLaunchTime; }

   unsigned int GetStageIndex() const { return mStageIndex; }
   void         SetStageIndex(unsigned int aStageIndex);
   bool         IsExtrapolating() const override { return mIsBallistic; }

   //! Used to propagate the mover forward in time
   //! for explicit intercept calculations
   std::vector<double> GetState() const;
   void                SetState(const std::vector<double>& aStateVector);

   void GetTerminalLLA(double& aLat, double& aLon, double& aAlt);
   void GetTargetLLA(double& aLat, double& aLon, double& aAlt);
   void SetTargetLLA(double aLat, double aLon, double aAlt);

   //! A stage represents one stage in the vehicle.
   class Stage
   {
   public:
      Stage();

      bool Initialize(double aStageStartTime);
      bool Initialize2();
      bool ProcessInput(UtInput& aInput);

      //! The sum of the initial masses of this and the subsequent stages (kg)
      double RemainingInitialMass() const { return mEmptyMass + mFuelMass + mPayloadMass; }

      //! The sum of the empty (burnout) mass of this and the initial masses of the subsequent stages (kg)
      double RemainingBurnoutMass() const { return mEmptyMass + mPayloadMass; }

      //@(
      //! @name Input values

      //! The ballistic coefficient (N/m^2)
      //! @note This should be the coefficient for the composite vehicle consisting of this
      //! and all subsequent stages.
      double mBallisticCoeff; //!< Beta (N/m^2)

      //! Mass of the THIS stage minus the mass of the fuel on THIS.
      //! @note This does NOT include the mass of the subsequent stages.
      double mEmptyMass;

      //! Mass of just the fuel on THIS stage.
      //! @note This does NOT include the mass of the subsequent stages.
      double mFuelMass;

      //! The fraction of the total mass of THIS stage that is fuel.
      //! @note This does NOT include the mass of the subsequent stages.
      double mFuelMassFraction;

      //! The total mass of the THIS stage (empty mass plus fuel mass).
      //! @note This does NOT include the mass of the subsequent stages.
      double mTotalMass;

      //! The thrust (N)
      double mThrust;

      //! The specific impulse (sec)
      double mSpecificImpulse;

      //! The engine burn time (sec)
      double mThrustDuration;

      //! Coast time between separation of previous state and ignition of this stage.
      //! \note A pre-burn coast time for the first stage is not supported.
      double mPreIgnitionCoastTime;

      //! Coast time from burnout of this stage to separation.
      //! \note This is effectively ignored for the last stage.
      double mPreSeparationCoastTime;

      //! Zero-lift drag coefficient (C_d0)
      double mCd0;

      //! Reference area
      double mReferenceArea;

      //@}

      //@{
      //! @name Derived values

      //! The time-rate-of-change of the vehicles weight (NOT MASS) while the engine of this stage is burning.
      double mWdot;

      double mPayloadMass;

      //! The surface area times the drag coefficient (m^2)
      double mS_Cd0;

      //! The time relative to launch and state when the stage comes into existence (previous stage separates)
      double mInitialTime;
      double mInitialState[9];

      //! The time relative to launch and state when the engine ignites
      double mIgnitionTime;
      double mIgnitionState[9];

      //! The time relative to launch and state when the engine burns out
      double mBurnOutTime;
      double mBurnOutState[9];

      //! The time relative to launch when the stage separates from the vehicle
      double mSeparationTime;

      //@}
   };

protected:
   WsfTBM_Mover(const WsfTBM_Mover& aSrc);

   bool ProcessCepTable(UtInput& aInput);

   bool SaveCepPoint(bool&                aFormatIsFrozen,
                     std::vector<double>& aNominalRanges,
                     std::vector<double>& aRangeErrors,
                     std::vector<double>& aAzimuthErrors,
                     double               aNominalRange,
                     double               aRangeError,
                     double               aAzimuthError);

   void ComputeTargetError();
   void SetMasses();

   void ComputeImpactRange(double psi, double& rangegr, double& xmid, double& aMaxAlt);

   void ComputeTransitionStateVectors(const double aInitialState[9]);

   void CreateInitialState(double aElevation, double aState[9]);

   void ComputeLaunchAngle(double fltran, double psicrit, double rngmax, double& flttim, double& psi, double& maxAlt, bool& toofar);

   void ComputeMaximumRange(double al, double am, double ah, double tol, double& psicrit, double& rngmax);

   double GetAltitude(const double aState[9]) const;

   double GetSpeed(const double aState[9]) const;

   void CepTableLookup(double aTargetGroundRange, double& aRangeError, double& aAzimuthError);

   void PrintState(const double aState[9], ut::log::MessageStream& aStream) const;

   void ScheduleScriptEvents(double aSimTime);

   void SetTrailingEffects(double aSimTime) const;

   void WriteStateForPlot(double aTime, const double aState[9]);

   void odeint2(double y[9], double x1, double x2);

   void derivs(double x, const double y[9], double dydx[6]) const;

   void rkqc(double y[9], double dydx[6], double& x, double htry, double eps, double yscal[], double& hdid, double& hnext);

   void rk4(double y[9], double dydx[6], double x, double h, double yout[9]);

   //! @brief Used to output data when debugging.
   void OutputDiagnostic(double aSimTime);

   double       mLaunchTime; //!< Simulation time at launch
   Stage        mStage;      //!< The current stage
   unsigned int mStageIndex; //!< The index of the current stage
   double       mState[9];   //!< The current state vector (6 + 3 for accel)

   std::vector<Stage> mStageList;         //!< The list of stages
   bool               mExplicitStageUsed; //!< 'true' if a 'stage' command was used
   bool               mImplicitStageUsed; //!< 'true' if a stage was defined without using 'stage'

   bool   mAzErrorUsed;
   bool   mHaveTarget;
   bool   mIsBallistic;
   double mFixedCEP; //!< Circular Error Probable.  Defaults to zero, with no effect.

   // The launcher position (derived from either the platform or the input file)

   double mLaunchLat;    //!< Launch latitude
   double mLaunchLon;    //!< Launch longitude
   double mLaunchAlt;    //!< Launch altitude
   bool   mLaunchAltAGL; //!< 'true' if AGL altitude was specified in the input file

   // Scenario launch conditions

   bool mIgnoreTarget; //!< true if the target specification should be ignored
   bool mLofted;       //!< true if trajectory is lofted, false if depressed

   double mTargetLat; //!< target latitude (deg)
   double mTargetLon; //!< target longitude (deg)
   double mTargetAlt; //!< terrain height at target location (m)

   double mTerminalLat; //!< terminal latitude (deg)
   double mTerminalLon; //!< terminal longitude (deg)
   double mTerminalAlt; //!< terrain height at terminal location (m)

   bool mShowStatus; //!< show initial conditions on standard output

   int mDebugCallTrigger;
   int mDebugCallCount;

   TblIndVarU<double>  mNomRanges;     //<! Nominal Downrange independent values
   TblDepVar1<double>  mRangeErrors;   //<! Range Error values at different nominal ranges
   TblDepVar1<double>  mAzimuthErrors; //<! Azimuth Error values at different nominal ranges
   TblLookupLU<double> mLookupUtility; //<! Table Lookup utility.

   // PROGRAMMER's NOTE:  TODO --
   // This implementation does not yet use WsfPlatform values for mass accounting purposes.
};

#endif
