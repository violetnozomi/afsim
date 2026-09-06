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

#ifndef WSFGUIDEDMOVERBASE_HPP
#define WSFGUIDEDMOVERBASE_HPP

#include "wsf_mil_export.h"

#include <fstream>

#include "UtAtmosphere.hpp"
#include "UtCallback.hpp"
#include "UtEntity.hpp"
class UtInput;
#include "UtLog.hpp"
#include "WsfMover.hpp"
class WsfMoverGuidance;
class WsfProcessor;
#include "WsfRoute.hpp"
class WsfWeapon;

//! An abstract base class that defines an interface for guided movers.

class WSF_MIL_EXPORT WsfGuidedMoverBase : public WsfMover
{
public:
   WsfGuidedMoverBase(WsfScenario& aScenario);
   ~WsfGuidedMoverBase() override;
   WsfGuidedMoverBase& operator=(const WsfGuidedMoverBase&) = delete;

   //! @name Common framework methods.
   //@{
   bool Initialize(double aSimTime) override;
   bool Initialize2(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;
   void Update(double aSimTime) override;
   //@}

   //! @name Status (on/off) methods.
   //@{
   void PartBroken(double aSimTime) override;
   //@}

   //! @name Base class mover methods
   //@{
   WsfSpatialDomain GetSpatialDomain() const override { return WSF_SPATIAL_DOMAIN_AIR; }

   const WsfRoute* GetRoute() const override;

   // IsExtrapolating is used to signify that boost phase is complete.
   bool IsExtrapolating() const override { return mBoostComplete; }

   bool IsPaused() const override { return mFMM_Time < 0.0; }

   bool IsStopped() const override { return mFMM_Time < 0.0; }

   bool SetRoute(double aSimTime, const WsfRoute& aRoute) override;

   bool UpdateRoute(double aSimTime, const WsfRoute& aRoute) override;
   //@}

   //! Set the pointer to the mover guidance object.
   //! Ownership is retained by the caller.
   virtual void SetGuidance(WsfMoverGuidance* aGuidancePtr) { mGuidancePtr = aGuidancePtr; }

   //! Get the pointer to the mover guidance object
   virtual WsfMoverGuidance* GetGuidance() const { return mGuidancePtr; }

   //! @name Command functions from the guidance computer.
   //@{
   //! Request desired lateral and vertical aerodynamic forces to be applied in the Body frame.
   //! This is merely a request, as the specified forces may not be attainable (aerodynamically)
   //! at this flight condition.
   virtual void SetYandZForces(double aYForce, double aZForce)
   {
      mYForce = aYForce;
      mZForce = aZForce;
   }

   //! Set the commanded yaw rate.
   virtual void SetCommandedYawRate(double aRate) { mCommandedYawRate = aRate; }

   //! Set the commanded pitch rate.
   virtual void SetCommandedPitchRate(double aRate) { mCommandedPitchRate = aRate; }

   //! Set the commanded roll rate.
   virtual void SetCommandedRollRate(double aRate) { mCommandedRollRate = aRate; }

   //! Set the commanded speed.
   //! @param aCommandedSpeed The commanded speed (m/s). A value less than zero indicates
   //! there is no commanded speed (use the value as determined by the mover).
   virtual void SetCommandedSpeed(double aCommandedSpeed) { mCommandedSpeed = aCommandedSpeed; }

   //! Set the commanded throttle setting.
   //! @param aCommandedThrottle The commanded throttle setting in the range [0..1].  A value less than
   //! zero indicates the mover should revert to the value defined by input.
   virtual void SetCommandedThrottle(double aCommandedThrottle) { mCommandedThrottle = aCommandedThrottle; }

   //! Explicitly define when the engines are to be stopped.
   //! This allows either the launch computer or user to define when the engines are to be stop.
   virtual void SetCommandedEngineStopTime(double aStopTime) { mCommandedEngineStopTime = aStopTime; }

   //! Set the minimum required height above terrain.
   //! This is a very crude mechanism for doing terrain following. When the guidance computer is commanded
   //! to an AGL altitude, it will set this to a 'small' positive non-zero value which the mover will then
   //! use to prevent the platform from crashing into the ground. This is necessary because the guidance
   //! doesn't do look-ahead and thus may not pull up quick enough to avoid an obstacle.
   //! @param aMinimum The required minimum offset from the terrain. If zero then avoidance is not performed.
   virtual void SetMinimumHeightAboveTerrain(double aMinimum) { mMinimumHeightAboveTerrain = aMinimum; }

   //! Currently active Stage for this object.
   virtual size_t CurrentStage() const { return mStageIndex; }

   virtual void EjectStage(double aSimTime, double aPreSeparationCoastTime, double aPreIgnitionCoastTime);
   //@}

   //! @name Callback objects for route changes and stage transitions.
   //! Another object can 'connect' to these objects to receive notification when a stage transition occurs.
   //! The first argument  in the callback is the current simulation time.
   //! The second argument is the actual time of the event. This may less than the current simulation time.
   //@{
   UtCallbackListN<void(double)>         RouteChanged;
   UtCallbackListN<void(double, double)> StageIgnition;
   UtCallbackListN<void(double, double)> StageBurnout;
   UtCallbackListN<void(double, double)> StageSeparation;
   //@}

   void WriteKinematicStatus(ut::log::MessageStream& aStream);

protected:
   enum Phase
   {
      cPHASE_PRE_BURN,  //<! Coasting, after separation of previous stage, before starting engine
      cPHASE_BURN,      //<! Engine running
      cPHASE_POST_BURN, //<! Coasting after expenditure of fuel, before stage separation.
      cPHASE_FAIL       //<! Coasting after a failure (ignition, staging, etc.)
   };

   // Declare default copy constructor (for Clone())
   WsfGuidedMoverBase(const WsfGuidedMoverBase& aSrc);

   //! Advance forward in time.
   //!
   //! This is the main function of this class. The derived class is responsible for moving time forward
   //! and pushing the updated kinematic state into the protected data member 'mExternalState'.
   //!
   //! @param aSimTime The true current simulation time.
   //! @param aBegTime The beginning of the integration interval.
   //! @param aEndTime The ending    of the integration interval. This will be updated if in burn phase
   //!                 and the fuel source becomes empty during this interval.
   //! @param aNextPhase   The current phase. This will be updated if in the burn phase and the fuel source
   //!                 becomes empty during this interval.
   virtual void AdvanceTime(double aSimTime, double aBegTime, double& aEndTime, Phase& aNextPhase) = 0;

   void CallStagingEventScript(double aSimTime, const std::string& aScriptName);

   virtual void InitiateMotion(double aSimTime, double aCurTime);

   virtual bool SeparateStage(double aSimTime, size_t aStageIndex);

   virtual bool IgniteStage(double aSimTime, size_t aStageIndex);

   virtual void InitiateStage(double aSimTime, double aCurTime, size_t aStageIndex);

   virtual void PostUpdate(double aSimTime);

   void PrelaunchAdvanceTime(double aSimTime, double aCurTime);

   void PrelaunchComplete(double aSimTime, double aCurTime);

   void SetInitialState(double aSimTime, double aCurTime, bool aInitiatingMotion);

   virtual void WriteStagingMessage(ut::log::MessageStream& aStream,
                                    double                  aSimTime,
                                    double                  aCurTime,
                                    const std::string&      aStatus,
                                    size_t                  aStageIndex);

   virtual void WriteTrajectoryOutput(double aSimTime, double aCurTime, const double aECS_Forces[3]);

   //! The time when the platform was 'launched' (when the platform was created).
   //! This is NOT the time of first missile motion. That is in mFMM_Time.
   double mLaunchTime;

   //! The following is the entity state into which derived class AdvanceTime method
   //! should push their final kinematic state. The base class Update will use this
   //! to maintain the state of the entity as seen by the simulation. The two may differ
   //! because the derived class may run only on fixed time steps and the base class
   //! must extrapolate from the last timestep to get an intermediate position before
   //! the next timestep.
   UtEntity mExternalState;

   //! The time to which 'mExternalState' is applicable.
   double mLastAdvanceTime;

   //! The timestep for fixed-timestep movement.
   double mIntegrationTimestep;

   size_t mStageIndex; //!< The index of the current stage

   //! The index of the stage, which when the burn completes, signifies the boost phase is complete.
   size_t mBoostCompleteStageIndex;

   //! 'true' if the boost phase has been declared complete.
   bool mBoostComplete;

   WsfRoute mRoute; //!< A route to follow (if guidance supports it).

   //! @name Phase control
   //@{
   Phase mPhase;
   //! The simulation time when the phase started
   double mPhaseStartTime;
   //! The amount of time that must elapse prior to ignition.
   double mPreIgnitionCoastTime;
   //! The amount of tame that must elapse after the motor turns off and stage separation.
   double mPreSeparationCoastTime;
   //@}

   //! The time (relative to launch) when then launch computer has indicated the engines are to shut down.
   double mLaunchComputerBurnTime;

   WsfMoverGuidance* mGuidancePtr;
   double            mYForce;
   double            mZForce;
   double            mCommandedRollRate;
   double            mCommandedPitchRate;
   double            mCommandedYawRate;
   double            mCommandedSpeed;
   double            mCommandedThrottle;

   //! The absolute simulation time when the engines are commanded explicitly commanded to stop.
   //! It can be set either from the launch computer or via explicit script command. If not defined
   //! then the engines will burn until they run out of fuel.
   double mCommandedEngineStopTime;

   //! Override of the pre_separation_coast_time for the NEXT stage (from EjectStage)
   double mCommandedPreSeparationCoastTime;
   //! Override of the pre_ignition_coast_time for the CURRENT stage (from EjectStage)
   double mCommandedPreIgnitionCoastTime;

   //! The minimum height above terrain required for terrain following.
   double mMinimumHeightAboveTerrain;

   bool mShowStatus; //!< show initial conditions on standard output
   bool mShowTrajectory;
   //! If true, sets unguided mover heading to match owning platform's velocity vector, rather than orientation.
   bool mMatchOwningPlatHeading;
   //! If true, check for ground impact directly instead of relying on a 'fuse' processor.
   bool mCheckForGroundImpact;

   UtAtmosphere mAtmosphere;
   double       mSeaLevelPressure;

   //! Information captured at time of 'first missile motion' (needed by trajectory output)
   //@{
   double mFMM_Time;
   double mFMM_LocWCS[3];
   double mFMM_Heading;
   //@}

   std::string   mTimeHistoryPath;
   std::ofstream mTimeHistoryOfs;

   //! The following is true if the weapon to be held on the pad/rail during
   //! the pre-ignition time for the first stage.
   size_t     mLaunchPlatformIndex;
   WsfWeapon* mLaunchWeaponPtr;

private:
   void AdvanceTimeInternal(double aSimTime, double aBegTime, double& aEndTime);
};

#endif
