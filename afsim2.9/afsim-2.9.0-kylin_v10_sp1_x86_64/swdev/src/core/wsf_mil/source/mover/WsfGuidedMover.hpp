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

#ifndef WSFGUIDEDMOVER_HPP
#define WSFGUIDEDMOVER_HPP

#include "wsf_mil_export.h"

#include <vector>

#include "UtCloneablePtr.hpp"
#include "UtECI_Conversion.hpp"
#include "UtInput.hpp"
#include "UtRungeKutta.hpp"
#include "UtTable.hpp"
class WsfAero;
#include "WsfGuidedMoverBase.hpp"
#include "WsfGuidedMoverTestObject.hpp"
#include "WsfPlatform.hpp"

//! A specialized mover that imposes a Newtonian dynamics model upon a platform, from
//! specified mass properties, applied Aerodynamic forces, and via steering commands
//! issued from a WsfGuidanceComputer.
//!
//! This implementation assumes a missile-like attack vehicle, with aerodynamics that influence
//! its flight path.  The guidance commands will be externally pushed into this object by a processor
//! (such as WsfGuidanceComputer).  Termination of flight due to target or ground impact will also be
//! handled by some other means (such as WsfWeaponFuse).
//!
//! The Mover may not be powered, but multiple stages may be specified with different thrust values and
//! discontinuous changes in aerodynamics, mass properties, and thrust.

class WSF_MIL_EXPORT WsfGuidedMover : public WsfGuidedMoverBase
{
public:
   using CurvePtr = UtCloneablePtr<UtTable::Curve>;
   using TablePtr = UtCloneablePtr<UtTable::Table>;

   enum CoordinateFrame
   {
      cCF_WCS,
      cCF_ECI
   };

   WsfGuidedMover(WsfScenario& aScenario, bool aGuidanceOn = true);
   ~WsfGuidedMover() override = default;
   WsfGuidedMover& operator=(const WsfGuidedMover&) = delete;

   //! @name Common framework methods.
   //@{
   WsfMover*   Clone() const override;
   bool        Initialize(double aSimTime) override;
   bool        Initialize2(double aSimTime) override;
   bool        ProcessInput(UtInput& aInput) override;
   const char* GetScriptClassName() const override;
   //@}

   //! @name Common mover methods.
   //@{
   bool GetPropertyDouble(const std::string& aPropertyName, double& aProperty) override;
   //@}

   void GetCommandedAttitude(double& aYaw, double& aPitch, double& aRoll) const;

   // Accessors for divert thrust magnitude.  Needed for certain maneuver calculations.
   double GetDivertThrust() const { return mDivertThrust; }
   double GetDivertFuelMass() const { return mCurState.mDivertFuelMass; }

   // This returns a pointer to the test object. It should only be used in testing.
   WsfGuidedMoverTestObject* GetTestSupportObject() const { return mTestSupportObjectPtr.get(); }

   //! @name Accessors for script methods
   //@{
   double GetCurrentThrust() const { return mCurState.mCurrentThrust; }
   void   GetAeroForcesECS(double aAeroForcesECS[3]) const { UtVec3d::Set(aAeroForcesECS, mLastAeroForcesECS); }
   void   GetThrustForcesECS(double aThrustForcesECS[3]) const
   {
      UtVec3d::Set(aThrustForcesECS, mCurState.mLastThrustForcesECS);
   }
   double GetMaximumLateralAerodynamicForce() const;
   double GetDynamicPressure() const;
   double GetGravitationalAcceleration() const;

   // Get the number of stages
   size_t GetNumStages() const { return mStageList.size(); }

   // Get the empty/fuel/total mass of the current stage (in kg)
   double GetEmptyMass() const;
   double GetFuelMass() const;
   double GetPayloadMass() const;
   double GetTotalMass() const;

   // Get the empty/fuel/total mass of the stage specified by aStageIndex (in kg)
   double GetEmptyMass(size_t aStageIndex) const;
   double GetFuelMass(size_t aStageIndex) const;
   double GetPayloadMass(size_t aStageIndex) const;
   double GetTotalMass(size_t aStageIndex) const;

   // Get the current fuel/total mass (in kg)
   double GetCurrentFuelMass() const { return mCurState.mCurrentFuelMass; }
   double GetCurrentTotalMass() const { return mCurState.mCurrentFuelMass + GetEmptyMass() + GetPayloadMass(); };

   // Get the current fuel flow rate (in kg/s)
   double GetCurrentFuelFlowRate() const { return mCurState.mCurrentFuelFlowRate; }

   double GetLevelFlightThrust(double aSimTime, double aAltitude_m, double aMach) const;

   double GetLevelFlightDrag(double aAltitude_m, double aMach, double aWeight_N) const;

   double GetLevelFlightFuelBurnRate(double aSimTime, double aAltitude_m, double aSpeed_mps, double aWeight_N) const;


   //@}

   enum IntegrationMethod
   {
      cTRAPEZOIDAL,
      cRK2,
      cRK4
   };

   class Stage
   {
   public:
      enum EngineType
      {
         cNONE,
         cROCKET,
         cJET
      };

      enum ThrustReference
      {
         cDEFAULT,
         cVACUUM,
         cSEA_LEVEL,
         cPAIRED
      };

      enum TurnMethod
      {
         cSKID_TO_TURN,
         cBANK_TO_TURN
      };

      Stage();
      Stage(const Stage& aSrc);
      ~Stage();
      Stage& operator=(const Stage&) = delete;

      bool   CompletePairedValues(WsfGuidedMover* aMoverPtr);
      double ComputeTimeWeightedAverage(const UtTable::Curve& aCurve) const;
      bool   ComputeUnknownValues(WsfGuidedMover* aMoverPtr, int& aRecursionDepth);
      bool   Initialize(double aSimTime, WsfGuidedMover* aMoverPtr, bool aShowStatus);
      bool   ProcessInput(UtInput& aInput);
      void   ShowStatus(WsfGuidedMover* aMoverPtr, ut::log::MessageStream& aMsgStream) const;

      double ComputeSpecificImpulse(double aAmbientPressure, double aSeaLevelPressure) const;
      double DetermineCurrentThrust(double aAmbientPressure, double aSeaLevelPressure, double aPhaseTime);

      //! @name Input Values
      //@{

      //! The type of the engine.
      EngineType mEngineType;

      //! Reference for thrust
      ThrustReference mThrustReference;

      //! Reference for specific impulse
      ThrustReference mImpulseReference;

      //! Mass of the THIS stage minus the mass of the fuel on THIS.
      //! @note This does NOT include the mass of the subsequent stages.
      double mEmptyMass;

      //! Mass of just the fuel on THIS stage.
      //! @note This does NOT include the mass of the subsequent stages.
      double mFuelMass;

      //! The total mass of the THIS stage (empty mass plus fuel mass).
      //! @note This does NOT include the mass of the subsequent stages.
      double mTotalMass;

      //! The thrust (N)
      double mThrust;

      //! The thrust (N) at sea level
      double mThrustSeaLevel;

      //! The thrust (N) in vacuum
      double mThrustVacuum;

      //! Thrust as a function of time
      CurvePtr mThrustTablePtr;

      //! Sea level thrust as a function of time.
      CurvePtr mSeaLevelThrustTablePtr;

      //! Vacuum thrust as a function of time.
      CurvePtr mVacuumThrustTablePtr;

      //! The nozzle exit area (m^2)
      double mNozzleExitArea;

      //! The specific impulse (sec)
      double mSpecificImpulse;

      //! The specific impulse (sec) at sea level
      double mSpecificImpulseSeaLevel;

      //! The specific impulse (sec) in vacuum
      double mSpecificImpulseVacuum;

      //! The rate of fuel expenditure (Kg/Sec)
      double mBurnRate;

      //! The rate of fuel expenditure as a function of time.
      CurvePtr mBurnRateTablePtr;

      //! The throttle setting.
      TablePtr mThrottlePtr;

      //! The engine burn time (sec)
      double mThrustDuration;

      //! Coast time between separation of previous state and ignition of this stage.
      double mPreIgnitionCoastTime;

      //! Coast time from burnout of this stage to separation.
      //! @note This is effectively ignored for the last stage.
      double mPreSeparationCoastTime;

      //! The string ID of the aerodynamic attributes to be used.
      WsfStringId mAeroId;

      //! The pointer to the aerodynamic attributes to be used.
      WsfAero* mAeroPtr;

      //! The airframe time constant
      TablePtr mTimeConstantPtr;

      //! The angle of attack.
      TablePtr mAngleOfAttackPtr;

      //! The minimum time that thrust vector control can be used (relative to ignition start)
      double mMinTVC_Time;

      //! The maximum time that thrust vector control can be used (relative to ignition start)
      double mMaxTVC_Time;

      //! Maximum TVC angle
      double mMaxTVC_Angle;

      //! Lateral thrust gain for TVC and divert
      double mLateralThrustGain;

      //! The integration timestep.
      double mIntegrationTimestep;

      //! The probability of separation of this stage from subsequent stages
      double mProbabilityOfSeparation;

      //! The probability of ignition of this stage
      double mProbabilityOfIgnition;

      //! The visual method for turns (skid or bank).
      TurnMethod mTurnMethod;

      //! 'true' if the direction of the thrust should be reversed.
      bool mReverseThrust;

      //! 'true' if this is the final stage of 'boost' phase.
      //! Normally the last stage with 'thrust' defines the when boost phase is complete,
      //! but the user can define an earlier stage defines the completion.
      bool mFinalBoostStage;
      //@}

      //! @name Derived values
      //@{
      //! The sum of the initial masses of any subsequent stages (kg)
      //! Consider anything stacked on top of this stage as "payload".
      double mPayloadMass;
      //@}
   };

   //! This class is used by the Runge-Kutta integrator as a representation of the
   //! vector space that gives the kinematic state of the object. This includes the
   //! position and velocity (the variables being integrated). Members of this vector
   //! space also are used for the results of the right hand side computation of the
   //! dynamics. In that use, the components don't represent the position, say, but
   //! rather the time rate of change of the position.
   class KinematicState
   {
   public:
      KinematicState() = default;
      KinematicState(const UtVec3d& aX, const UtVec3d& aY, double aZ)
         : mX{aX}
         , mY{aY}
         , mZ{aZ}
      {
      }

      const UtVec3d& GetPosition() const { return mX; }
      const UtVec3d& GetVelocity() const { return mY; }
      double         GetFuelMass() const { return mZ; }

      const UtVec3d& GetDxDt() const { return mX; }
      const UtVec3d& GetDvDt() const { return mY; }
      double         GetDmDt() const { return mZ; }

      void SetPosition(const UtVec3d& aPosition) { mX = aPosition; }
      void SetVelocity(const UtVec3d& aVelocity) { mY = aVelocity; }
      void SetFuelMass(double aMass) { mZ = aMass; }

      void SetDxDt(const UtVec3d& aDxDt) { mX = aDxDt; }
      void SetDvDt(const UtVec3d& aDvDt) { mY = aDvDt; }
      void SetDmDt(double aDmDt) { mZ = aDmDt; }

      KinematicState operator*(double aScaling) const
      {
         return KinematicState{mX * aScaling, mY * aScaling, mZ * aScaling};
      }

      KinematicState operator+(const KinematicState& aOther) const
      {
         return KinematicState{mX + aOther.mX, mY + aOther.mY, mZ + aOther.mZ};
      }

   private:
      UtVec3d mX{}; //! Represents either the position, or the time rate of change of position.
      UtVec3d mY{}; //! Represents the velocity, or the time rate of change of velocity.
      double  mZ{}; //! Represents the fuel mass, or the time rate of change of the fuel mass.
   };

   //! Entity state data.
   //! The data is in WCS (ECEF) when the coordinate frame is WCS, and ECI when the coordinate frame is ECI.
   //! This is currently in spherical coordinates.
   class State
   {
   public:
      State();
      State(const State& aOther) = default;
      void Print(ut::log::MessageStream& aMsgStream, double aFMM_Time);

      State Advance(double aDeltaT, const KinematicState& aKinematicState) const;

      void           UpdateOrientationData(double aSimTime, double aDeltaTime);
      void           UpdateOtherStateData(double aSimTime);
      void           TakeEulerStep(double                aDeltaTime,
                                   const KinematicState& aDerivatives,
                                   const State&          aInitialState,
                                   bool                  aIntegrateMass = false);
      void           ComputeDerivatives(const double aAppliedForcesECS[3]);
      UtVec3d        ComputeDerivatives(const double aAppliedForcesECS[3]) const;
      void           ComputeGravitationalForce(double aGravitationalForceWCS[3]) const;
      KinematicState GetDerivatives() const;
      KinematicState GetUnpoweredDerivatives() const;

      const WsfGuidedMover* mMoverPtr{nullptr};
      double                mTime;
      double                mWorld_ToECS_Transform[3][3]; // Either WCS->ECS or ECI->ECS
      double                mLocation[3];
      double                mVelocity[3];
      double                mAcceleration[3];
      double                mAppliedForcesECS[3]; // Does not include gravity
      double                mLastThrustForcesECS[3];
      double                mLastAeroForcesECS[3];
      double                mPsi;
      double                mTheta;
      double                mPhi;
      double                mAltitude;
      double                mSpeed; // Always ECEF speed
      double                mMach;
      double                mMass;
      double                mCurrentThrust;
      double                mCurrentFuelFlowRate;
      double mThrustDurationExtension; //!< The amount of time the thrust duration is extended due to commanded throttle
      double mDivertFuelMass;
      double mCurrentFuelMass;
      CoordinateFrame mCoordinateFrame{CoordinateFrame::cCF_WCS};
      double          mECI_SwitchTime{-1.0E-30}; //!< The time when the WCS->ECI frame switch occurred.
      bool            mLaunchToOrbit{false};     //!< 'true' if the mission is launch-to-orbit
      //! For launch-to-orbit, maintain the inclination as defined by the launch conditions (latitude and launch heading)
      bool mMaintainInclination{false};
   };

   class Ejectable
   {
   public:
      Ejectable()
         : mName()
         , mMass(0.0)
         , mTime(0.0)
         , mAltitude(0.0)
      {
      }
      std::string mName;
      double      mMass;
      double      mTime;
      double      mAltitude;
   };

protected:
   // Declare default copy constructor (for Clone())
   WsfGuidedMover(const WsfGuidedMover& aSrc);

   void AdvanceTime(double aSimTime, double aBegTime, double& aEndTime, Phase& aNextPhase) override;

   void AdvanceTimeSinglestep(double aSimTime, double aBegTime, double& aEndTime, Phase& aNextPhase);

   void AdvanceTimeMultistep(double aSimTime, double aBegTime, double aEndTime, Phase& aNextPhase);

   void InitiateMotion(double aSimTime, double aCurTime) override;

   bool SeparateStage(double aSimTime, size_t aStageIndex) override;

   bool IgniteStage(double aSimTime, size_t aStageIndex) override;

   void InitiateStage(double aSimTime, double aCurTime, size_t aStageIndex) override;

   void PostUpdate(double aSimTime) override;

   void AdvanceTimeLegacyRK2(double aSimTime, State& aState, const double aAppliedForcesECS[3]);

   void AdvanceTimeLegacyRK4(double aSimTime, State& aState, const double aAppliedForcesECS[3]);

   void AdvanceTimeTrapezoidal(double aSimTime, State& aState, const double aAppliedForcesECS[3]);

   //! Take a step using second-order Runge-Kutta.
   struct RK2_StepFunction
   {
      template<typename Callable>
      static State Step(double aSimTime, State& aState, Callable&& aRightHandSide)
      {
         return ut::RK2_Step<State, KinematicState>(aSimTime, aState, aRightHandSide);
      }
   };

   //! Take a step using fourth-order Runge-Kutta.
   struct RK4_StepFunction
   {
      template<typename Callable>
      static State Step(double aSimTime, State& aState, Callable&& aRightHandSide)
      {
         return ut::RK4_Step<State, KinematicState>(aSimTime, aState, aRightHandSide);
      }
   };

   //! This method takes a Runge-Kutta step.
   template<typename StepFunction>
   void AdvanceTimeRK(double aSimTime, double aBegTime, double aEndTime)
   {
      Stage& stage = mStageList[mStageIndex];

      mCurState.mMass     = GetPlatform()->GetMass();
      mCurState.mMoverPtr = this;

      double  dt = aEndTime - aBegTime;
      bool    first{true};
      UtVec3d aeroSave;
      auto    rhs = [this, aSimTime, aBegTime, aEndTime, &stage, &first, &aeroSave](State& aState)
      {
         ComputeFullForces(aSimTime, aBegTime, aEndTime, aState, stage);
         if (first)
         {
            first = false;
            UtVec3d::Set(aeroSave.GetData(), aState.mLastAeroForcesECS);
         }
         KinematicState retval{};
         retval.SetDxDt(aState.mVelocity);
         retval.SetDvDt(aState.mAcceleration);
         retval.SetDmDt(aState.mCurrentFuelFlowRate);
         return retval;
      };

      auto step = StepFunction::Step(dt, mCurState, rhs);
      mCurState = step;
      UtVec3d::Set(mLastAeroForcesECS, aeroSave.GetData());
   }

   void ComputeDivertForces(double aDeltaTime,
                            State& aState,
                            double aCommandedForceY,
                            double aCommandedForceZ,
                            double aAppliedForcesECS[3],
                            double aDivertThrustForcesECS[3]) const;

   void ComputeThrustForces(double aSimTime, double aDeltaTime, State& aState, Stage& aStage, double aThrustForceECS[3]) const;

   double GetHorizontalThrust(double       aSimTime,
                              double       aDeltaTime,
                              State&       aState,
                              double       aCommandedThrottle,
                              const double aIV_Values[6],
                              Stage&       aCurrentStage) const;

   void ComputeVectoredThrustForces(double       aSimTime,
                                    double       aCommandedForceY,
                                    double       aCommandedForceZ,
                                    const Stage& aStage,
                                    double       aThrustForceECS[3]) const;

   void ComputeAeroForces(double       aSimTime,
                          double       aDeltaT,
                          const Stage& aStage,
                          const double aThrustForcesECS[3],
                          double       aAeroForcesECS[3]) const;

   void ComputeThrustForCommandedSpeed(double       aDeltaT,
                                       State&       aState,
                                       const Stage& aStage,
                                       const double aAeroForcesECS[3],
                                       double       aThrustForcesECS[3]) const;

   double GetTimeConstant(const Stage& aStage) const;

   void ComputeFullForces(double aSimTime, double aBegTime, double aEndTime, State& aRHS_State, Stage& aRHS_Stage) const;

   void IntegrateEOM(double aDeltaTime, const double aAppliedForcesECS[3], State& aOutState, State& aInState1, State& aInState2);

   void JettisonEjectables(double aSimTime);

   bool SelectCoordinateFrame(const State& aState, CoordinateFrame& aTargetFrame) const;

   void SwitchCoordinateFrame(State& aState, CoordinateFrame aTargetFrame);

   void UpdateExternalState(State& aState);

   std::vector<Stage>             mStageList;         //!< The list of stages
   bool                           mExplicitStageUsed; //!< 'true' if a 'stage' command was used
   bool                           mImplicitStageUsed; //!< 'true' if a stage was defined without using 'stage'
   std::vector<Ejectable>         mEjectables;        //!< The list of eject-able objects
   double                         mDivertThrust;
   double                         mDivertFuelFlowRate;
   double                         mDivertMinAlt;
   double                         mDivertMaxAlt;
   UtCloneablePtr<UtTable::Table> mTimeConstantPtr; //!< airframe time constant
   CoordinateFrame                mDesiredCoordinateFrame;
   IntegrationMethod              mIntegrationMethod;
   bool                           mGuidanceOn;
   bool                           mVerifyValues;
   double                         mCommandedYaw;
   double                         mCommandedPitch;
   double                         mCommandedRoll;
   double                         mLastYaw;
   double                         mLastPitch;
   double                         mLastRoll;
   double                         mLastAeroForcesECS[3];
   double                         mLastMach; //!< For tracking Mach 1 crossing for show_status
   double                         mLastQ;    //!< For tracking MaxQ for show_status
   double                         mMaxQ;
   double                         mIV_Values[6];
   State                          mCurState;   //!< The current entity state
   State                          mTempState1; //!< Temporary state for RK2/RK4 integration
   State                          mTempState2; //!< Temporary state for RK4 integration
   State                          mTempState3; //!< Temporary state for RK4 integration
   UtEntity                       mTempEntity; //!< Temporary entity for WCS<->ECI conversions

   std::unique_ptr<WsfGuidedMoverTestObject> mTestSupportObjectPtr;

   UtECI_Conversion& GetECI_Conversion();

   ut::CloneablePtr<UtECI_Conversion> mECI_ConversionPtr{nullptr};
   double                             mECI_UpdateTime{0.0};
   double                             mECI_LastUpdateTime{0.0};

   bool mComputeForcesEachSubstep{false};
};

#endif
