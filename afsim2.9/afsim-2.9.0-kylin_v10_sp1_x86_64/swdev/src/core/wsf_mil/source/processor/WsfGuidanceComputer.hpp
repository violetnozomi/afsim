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

#ifndef WSFGUIDANCECOMPUTER_HPP
#define WSFGUIDANCECOMPUTER_HPP

#include "wsf_mil_export.h"

#include <list>
#include <vector>

#include "UtAtmosphere.hpp"
#include "UtCallbackHolder.hpp"
#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
class UtScriptClass;
class UtScriptTypes;

#include "WsfDraw.hpp"
class WsfGuidedMoverBase;
class WsfGuidanceProgram;
#include "WsfMoverGuidance.hpp"
class WsfSensor;
#include "script/WsfScriptProcessor.hpp"
class WsfTrack;

namespace wsf
{
class Terrain;
}

//! A class representing a guidance computer for steering/influencing a Mover.
//! The computer will follow proportional homing guidance to null Line-Of-Sight
//! rates to intercept a Track.  The track pointer will be "pushed" into this
//! processor from some external source, extrapolated in time if required.
//! The primary output of the class are constrained steering commands in pitch
//! and yaw, which are pushed into the Mover to influence motion dynamics.

class WSF_MIL_EXPORT WsfGuidanceComputer : public WsfScriptProcessor, public WsfMoverGuidance
{
public:
   friend class WsfGuidanceProgram;
   friend class WsfScriptGuidanceComputer;

   static const double cUNDEFINED_DOUBLE;     //!< A value that indicates a double precision value has not been defined.
   static const double cFROM_LAUNCH_COMPUTER; //!< Indicates 'flight_path_angle from_launch_computer'
   static const double cFLIGHT_PATH_ANGLE_TOLERANCE; //!< The tolerance for performing a flight path angle phase change.
   static const double cLARGE_POSITIVE_DOUBLE;

   WsfGuidanceComputer(WsfScenario& aScenarioPtr);
   ~WsfGuidanceComputer() override;
   WsfGuidanceComputer& operator=(const WsfGuidanceComputer&) = delete;

   //! @name Framework methods
   //@{
   WsfProcessor* Clone() const override;
   bool          Initialize(double aSimTime) override;
   bool          Initialize2(double aSimTime) override;
   bool          ProcessInput(UtInput& aInput) override;
   void          TurnOff(double aSimTime) override;
   void          TurnOn(double aSimTime) override;
   const char*   GetScriptClassName() const override;
   //@}

   //! @name Interface from WsfMoverGuidance
   //@{
   void UpdateGuidance(double aSimTime, double aCurTime, double aEndTime) override;
   //@}

   //! @name Calls from external sources, such as scripts.
   //@{
   bool SelectPhase(WsfStringId aPhaseName);

   bool         ClearCommandedAltitude(WsfStringId aPhaseName);
   bool         ClearCommandedFlightPathAngle(WsfStringId aPhaseName);
   bool         ClearCommandedSpeed(WsfStringId aPhaseName);
   bool         ClearCommandedThrottle(WsfStringId aPhaseName);
   bool         SetAllowRouteFollowing(WsfStringId aPhaseName, bool aValue);
   bool         SetAimpointAltitudeOffset(WsfStringId aPhaseName, double aValue);
   bool         SetAimpointAzimuthOffset(WsfStringId aPhaseName, double aValue, const std::string& aDirection);
   bool         SetAimpointRangeOffset(WsfStringId aPhaseName, double aValue);
   bool         SetAimpointEvaluationInterval(WsfStringId aPhaseName, double aValue);
   bool         SetCommandedAltitude(WsfStringId aPhaseName, double aValue, bool aAltIsAGL);
   bool         SetCommandedAzimuthOffset(WsfStringId aPhaseName, double aValue);
   bool         SetCommandedFlightPathAngle(WsfStringId aPhaseName, double aValue);
   bool         SetCommandedMach(WsfStringId aPhaseName, double aValue);
   bool         SetCommandedSpeed(WsfStringId aPhaseName, double aValue);
   bool         SetCommandedThrottle(WsfStringId aPhaseName, double aValue);
   bool         SetGuidanceDelay(WsfStringId aPhaseName, double aValue);
   bool         SetGuidanceTarget(WsfStringId aPhaseName, const std::string& aValue);
   bool         SetProportionalNavigationGain(WsfStringId aPhaseName, double aValue);
   bool         SetProportionalNavigationLimitAngle(WsfStringId aPhaseName, double aValue);
   bool         SetProportionalNavigationMethod(WsfStringId aPhaseName, const std::string& aValue);
   bool         SetVelocityPursuitGain(WsfStringId aPhaseName, double aValue);
   bool         SetGeeBias(WsfStringId aPhaseName, double aValue);
   virtual bool SetLateralGeeBias(WsfStringId aPhaseName, double aValue);
   bool         SetMaximumCommandedGees(WsfStringId aPhaseName, double aValue);
   bool         SetMaximumPitchAngle(WsfStringId aPhaseName, double aValue);
   bool         SetMaximumAscentRate(WsfStringId aPhaseName, double aValue);
   bool         SetMaximumDescentRate(WsfStringId aPhaseName, double aValue);
   bool         SetPitchChangeGain(WsfStringId aPhaseName, double aValue);
   virtual bool EjectStage(double aPreSeparationCoastTime, double aPreIgnitionCoastTime);
   virtual bool StopEngines();
   bool         StartProgram(WsfStringId aProgramName);
   bool         StopProgram(WsfStringId aProgramName);
   //@}

   //! @name Callbacks from the mover when the route is updated or a staging transition occurs.
   //@{
   virtual void RouteChanged(double aSimTime);
   virtual void StageIgnition(double aSimTime, double aEventTime);
   virtual void StageBurnout(double aSimTime, double aEventTime);
   virtual void StageSeparation(double aSimTime, double aEventTime);
   //@}

   //! @name Simulation callbacks
   //@{
   virtual void SensorTrackInitiated(double aSimTime, WsfSensor* aSensorPtr, const WsfTrack* aTrackPtr);
   //@}

   //! A class that defines a double precision value that can be either:
   //! - a constant
   //! - a reference to a script variable.
   //! @note This class was the predecessor to WsfVariable. We cannot change to use WsfVariable because:
   //! - The input syntax is slightly different.
   //! - This version supports continuous reevaluation of the script variable.
   //! - This only support variables of type double.
   //! @note This class is public because it is (or eventually be) used by WsfGuidanceProgram.
   class WSF_MIL_EXPORT Variable
   {
   public:
      Variable(double aValue = 0.0)
         : mValue(aValue)
         , mScale(1.0)
         , mVarNameId()
      {
      }

      //! @name Operator overloading.
      //! Allows the variable to be used as a normal variable.
      //! @note: This manipulates the constant value. If the variable references a script variable then the
      //         programmer must be sure to use the 'Update' method prior to accessing any of these method.
      //@{
      operator double() const { return mValue; }

      double operator+(double aRhs) const { return mValue + aRhs; }

      double operator-(double aRhs) const { return mValue - aRhs; }

      double operator=(double aRhs)
      {
         mValue = aRhs;
         mScale = 1.0;
         mVarNameId.Clear();
         return mValue;
      }
      //@}

      double Get() const { return mValue; }

      bool HasDefaultValue() const { return false; }

      //! Returns true if the variable is a script reference.
      bool IsReference() const { return !mVarNameId.IsNull(); }

      void ReadValue(UtInput& aInput, WsfScriptContext& aContext);

      void ReadValueOfType(UtInput& aInput, UtInput::ValueType aValueType, WsfScriptContext& aContext);

      void Update(WsfScriptContext& aContext);

      void ValueInClosedRange(UtInput& aInput, double aMinValue, double aMaxValue);

      void ValueGreater(UtInput& aInput, double aMinValue);

      void ValueGreaterOrEqual(UtInput& aInput, double aMinValue);

      void ValueLess(UtInput& aInput, double aMaxValue);

      void ValueLessOrEqual(UtInput& aInput, double aMaxValue);

      //! The current value.
      //! This will be the constant value if mVarNameId.IsNull() or the last value resolved from
      //! a script reference.
      double mValue;

      //! The scale factor used to convert from user supplied units to internal units.
      double mScale;

      //! The name of the script variable from which the value is to be gotten.
      //! If IsNull() then use the constant value in mValue.
      WsfStringId mVarNameId;
   };

   enum GuidanceTarget
   {
      cGT_DEFAULT,
      cGT_TRUTH,
      cGT_PERCEPTION,
      cGT_PREDICTED_INTERCEPT
   };

   enum OffsetDirection
   {
      cOD_LEFT,  //!< Offset so target is on the left side of the weapon
      cOD_RIGHT, //!< Offset so target is on the right side of the weapon
      cOD_EITHER //!< Offset so target on the side that causes least heading change of the weapon
   };

   //! A class that defines the conditions for performing a change in guidance phase
   class WSF_MIL_EXPORT PhaseChange
   {
   public:
      //! Name of the condition variable to be tested.
      enum VariableName
      {
         cPHASE_TIME,
         cFLIGHT_TIME,
         cALTITUDE,
         cSPEED,
         cVERTICAL_SPEED,
         cFLIGHT_PATH_ANGLE,
         cON_COMMANDED_FLIGHT_PATH_ANGLE,
         cDYNAMIC_PRESSURE,
         cTARGET_ALTITUDE,
         cTARGET_SPEED,
         cTARGET_FLIGHT_PATH_ANGLE,
         cCLOSING_SPEED,
         cTIME_TO_INTERCEPT,
         cRANGE_TO_INTERCEPT,
         cTARGET_SLANT_RANGE,
         cTARGET_GROUND_RANGE,
         cTARGET_ELEVATION,
         cTARGET_AZIMUTH,
         cLOS_TARGET_ELEVATION,
         cLOS_TARGET_AZIMUTH,
         cLOS_TARGET_ANGLE
      };

      enum Condition
      {
         cLESS,
         cLESS_OR_EQUAL,
         cEQUAL,
         cNOT_EQUAL,
         cGREATER_OR_EQUAL,
         cGREATER
      };

      PhaseChange(WsfStringId aNextPhaseName)
         : mNextPhaseName(aNextPhaseName)
         , mName(cPHASE_TIME)
         , // Just need a value - will get defined when needed
         mCondition(cEQUAL)
         , // Just need a value - will get defined when needed
         mValue()
      {
      }

      bool ConditionSatisfied(double aValue) const;

      void ReadCondition(UtInput& aInput);

      WsfStringId  mNextPhaseName;
      VariableName mName;
      Condition    mCondition;
      Variable     mValue;
   };

   //! A class that defines the guidance parameters for a phase of flight.
   class WSF_MIL_EXPORT Phase
   {
   public:
      enum PN_Method
      {
         cPN_PHASE_DEFAULT, // Used only by WsfGuidanceProgram::InterceptProgram
         cPN_PURE,
         cPN_AUGMENTED
      };

      Phase();

      void ExecuteOnEntry(double aSimTime, WsfScriptContext& aContext);

      void ExecuteOnExit(double aSimTime, WsfScriptContext& aContext);

      void ExecuteOnUpdate(double aSimTime, WsfScriptContext& aContext);

      bool Initialize(double aSimTime, WsfScriptContext& aContext);

      bool ProcessInput(UtInput& aInput, WsfGuidanceComputer& aComputer, WsfScriptContext& aContext);

      void ProcessNextPhase(UtInput& aInput, WsfScriptContext& aContext);

      UtScript*   mOnUpdateScriptPtr;
      WsfStringId mPhaseName;

      // =====================================================================================
      // NOTE: If you add an variables that use 'Variable', make sure you update the Initialize
      //       method to ensure they are added to the dynamic variable list if they contain a
      //       script reference.
      // =====================================================================================

      double mGuidanceDelay;
      double mTimeConstant;
      double mCommandedAlt;
      double mCommandedAzimuthOffset;
      double mCommandedFlightPathAngle;
      // A positive value is commanded speed. A negative value is commanded Mach number.
      double mCommandedSpeed;
      double mCommandedThrottle;

      double mAimpointAltitudeOffset;
      double mAimpointAzimuthOffset;
      double mAimpointRangeOffset;
      double mAimpointEvaluationInterval;

      double mCosLOS_Offset;
      double mPN_GainValue;
      double mVP_GainValue;
      double mGeeBiasValue;
      double mLateralGeeBiasValue; // non-dim, in Gees
      double mMaxGeeCmd;
      double mMaxPitchAngle;
      double mMaxAscentRate;
      double mMaxDescentRate;
      double mPitchChangeGain;

      GuidanceTarget  mGuidanceTarget;
      OffsetDirection mOffsetDirection;
      PN_Method       mPN_Method;

      bool mCommandedAltIsAGL;
      bool mAllowRouteFollowing;

      std::vector<WsfStringId> mPrograms;

      using ProgramAndPhase = std::pair<WsfStringId, WsfStringId>;

      WsfStringId                  mEndOfRoutePhaseName;
      WsfStringId                  mBoostCompletePhaseName;
      WsfStringId                  mStageIgnitionPhaseName;
      WsfStringId                  mStageBurnoutPhaseName;
      WsfStringId                  mStageSeparationPhaseName;
      WsfStringId                  mSensorTrackInitiatedPhaseName;
      std::vector<ProgramAndPhase> mProgramCompletePhaseName; // for 'next_phase <phase_name> when program <program_name> complete'
      std::vector<PhaseChange> mPhaseChangeList;
   };

   //! 'State' represents the state of the objects (weapon and target).
   //! It is passed to the guidance programs where they are used in evaluation.

   class WSF_MIL_EXPORT State
   {
   public:
      State()
         : mSimTime(0.0)
         , mCurTime(0.0)
         , mEndTime(0.0)
         , mWpnState()
         , mFlightPathAngle(0.0)
         , mWpnHeading(0.0)
         , mTgtState()
         , mAimpointIsValid(false)
         , mAimpointIsTarget(false)
      {
         UtVec3d::Set(mAimRelLocWCS, 0.0);
         UtVec3d::Set(mAimUnitVecECS, 0.0);
      }

      //! The current simulation time. This is the final time to which the mover is trying to achieve. It typically
      //! moves forward in smaller integration intervals. Note that mCurTime < mEndTime <= mSimTime.
      double mSimTime;
      //! The simulation time of the start of the current integration interval.
      double mCurTime;
      //! The simulation time of the end of the current integration interval.
      double mEndTime;

      //! @name The perceived kinematic state of the weapon that the computer is guiding.
      //@{
      UtEntity mWpnState;
      double   mFlightPathAngle; //!< Based on velocity vector, not orientation
      double   mWpnHeading;      //!< Based on velocity vector, not orientation
      //@}

      //! @name The perceived kinematic state of the target to which the computer is guiding.
      //@{
      UtEntity mTgtState;
      //@}

      double mAimRelLocWCS[3];
      double mAimUnitVecECS[3];
      bool   mAimpointIsValid;
      bool   mAimpointIsTarget;
   };

   //! A Commands object is passed to guidance programs, which update it as dictated by the program.
   //! The resulting commands are eventually passed to the mover to be applied.
   class Commands
   {
   public:
      double mAccelCmd[3];     //!< The desired lateral accelerations
      double mAngleRateCmd[3]; //!< The desired rotation rates
   };

   //! @name Called from guidance programs
   //!@{
   bool   ShowStatus() const { return mShowStatus; }
   bool   ShowDiagnostics() const { return mShowDiagnostics; }
   bool   ShowGraphics() const { return mShowGraphics; }
   Phase& GetCurrentPhase() { return mPhaseList[mPhaseIndex]; }
   double GetCommandedFlightPathAngle() const { return mCommandedFlightPathAngle; }
   double GetLastFlightPathAngle() const { return mLastFlightPathAngle; }
   double GetLaunchComputerFlightPathAngle() const { return mLaunchComputerFlightPathAngle; }
   double GetTimeConstant(const Phase& aPhase) const;
   double GetTimeSinceLaunch(double aSimTime) { return aSimTime - mLaunchTime; }
   double GetTimeSincePhaseStart(double aSimTime) { return aSimTime - mPhaseStartTime; }
   double GetLastUpdateTime() const { return mLastUpdateTime; }

   void DrawPoint(double aRedValue, double aGreenValue, double aBlueValue, int aPointSize);

   void PrintStatusHeader(double aSimTime, double aCurTime, ut::log::MessageStream& aStream);
   //@}

protected:
   WsfGuidanceComputer(const WsfGuidanceComputer& aSrc);

   bool BeginExternalCommand(WsfStringId aPhaseName, unsigned int& aPhaseIndex);

   void BeginExternalCommandEcho(unsigned int aPhaseIndex, const char* aCommandPtr, ut::log::MessageStream& aStream);

   virtual void CheckForPhaseChange(double aSimTime, double aEndTime);

   virtual void CheckForWaypointChange(double aSimTime, double aEndTime);

   double ComputeAzimuth(const double aRelLoc[3]) const;

   double ComputeElevation(const double aRelLoc[3]) const;

   void ComputeInterceptData(double aCurTime, double& aTimeToGo, double& aRangeToGo);

   unsigned int FindPhase(WsfStringId aPhaseName) const;

   WsfGuidanceProgram* FindProgram(WsfStringId aProgramName) const;

   double GetSimTime() const;

   virtual void SelectPhase(double aSimTime, double aChangeTime, WsfStringId aPhaseName);

   virtual void SelectWaypoint(double aSimTime, double aChangeTime, unsigned int aNewIndex);

   void StartProgram(double aSimTime, double aChangeTime, WsfStringId aProgramName);

   void StopProgram(double aSimTime, double aChangeTime, WsfStringId aProgramName);

   virtual bool UpdateAimpoint(double aCurTime, const Phase& aPhase);

   void UpdateRouteFollowing(const Phase& aPhase);

   virtual void UpdateTargetPerception(double aCurTime);

   void UpdateTerrainFollowing(const Phase& aPhase);

   virtual void UpdateWeaponPerception(double aCurTime);

   bool ValidateNextPhaseName(const Phase& aPhase, WsfStringId aNextPhaseName);

   std::vector<Phase> mPhaseList;         //!< The list of phases
   Phase              mPhaseTemplate;     //!< The template used to construct a phase
   bool               mExplicitPhaseUsed; //!< 'true' if a 'phase' command was used
   bool               mImplicitPhaseUsed; //!< 'true' if a phase was defined without using 'phase'
   WsfStringId        mInitialPhaseName;  //!< The name (ID) of the starting phase.
   unsigned int       mPhaseIndex;        //!< The index of the current phase

   //! @name Guidance programs.
   //@{
   WsfGuidanceProgram* mLegacyProgramPtr;
   using ProgramList = std::list<WsfGuidanceProgram*>;
   ProgramList mDefinedPrograms; //!< The list of defined programs
   ProgramList mActivePrograms;  //!< The list of the currently active programs
   //! This index is used to indicate which 'next_phase' command in the current phase was referenced when a guidance
   //! program signals completion. It is initialized to the size of the list by SelectPhase. This indicates that an
   //! active program with a corresponded next_phase has not yet completed.
   size_t mProgramCompleteIndex;
   //@}

   //! The guidance time constant for intercept navigation by the legacy program.
   double mTimeConstant;

   bool mGuideToTruth;    //!< 'true' if should guide to target truth.
   bool mShowStatus;      //!< 'true' if 'show_status' specified
   bool mShowDiagnostics; //!< 'true' if 'show_diagnostics' specified
   bool mShowCommands;    //!< 'true' if 'show_commands' specified
   bool mShowEvaluations; //!< 'true' if 'show_evaluations' specified.
   bool mShowGraphics;    //!< 'true' if 'show_graphics' is specified

   WsfGuidedMoverBase* mMoverPtr;
   UtAtmosphere        mAtmosphere;
   UtCallbackHolder    mCallbacks;
   wsf::Terrain*       mTerrainPtr;

   double mLaunchTime;               //<! SimTime when the weapon was launched
   double mLaunchLocWCS[3];          //<! Location of weapon at time of launch
   double mLastUpdateTime;           //<! SimTime when last update was performed
   double mPhaseStartTime;           //<! SimTime when the current phase started
   double mGuidanceStartTime;        //<! SimTime when guidance is to start
   double mCommandedFlightPathAngle; //<! The commanded flight path angle

   //! The state of the weapon and target.
   State mState;

   //! @name persistent data between iterations about the weapon and the target.
   //@{
   double mLastFlightPathAngle; //<! The last flight path angle of the weapon
   double mLastTgtUpdateTime;
   double mLastTgtLocWCS[3]; //<! The last location of the target
   double mLastTgtVelWCS[3]; //<! The last velocity of the target

   //! The following will be 'true' if the target state is 'defined'.
   //! The data may be old but it is still defined.
   bool mTgtStateDefined;
   //@}

   //! @name Predicted intercept data from the launch computer.
   //@{
   double mInterceptLocWCS[3];
   double mLaunchComputerFlightPathAngle;
   //@}

   //! @name Route following data.
   //@{
   unsigned int mRouteIndex;
   bool         mFollowingRoute;
   bool         mWasClosing;
   //@}

   //! @name Aimpoint offset data.
   //! If lateral aimpoint offset is provided it is not calculated every pass.
   //@{
   double       mAimpointExpireTime;
   double       mAimpointLocWCS[3];
   unsigned int mAimpointDrawId;
   //@}

   WsfDraw* mDrawPtr; //!< For drawing graphics

   //! The following is used to prevent recursion if a SelectPhase occurs in the user 'on_exit' block.
   bool mOnExitScriptActive;
};

#endif
