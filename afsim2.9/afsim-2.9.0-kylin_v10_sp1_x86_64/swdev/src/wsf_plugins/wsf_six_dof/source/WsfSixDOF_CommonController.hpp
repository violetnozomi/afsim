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

#ifndef WSFSIXDOFCOMMONCONTROLLER_HPP
#define WSFSIXDOFCOMMONCONTROLLER_HPP

#include "wsf_six_dof_export.h"

#include <vector>

#include "UtCloneablePtr.hpp"
class UtInput;
class UtInputBlock;
#include "UtLLAPos.hpp"
#include "UtTable.hpp"
#include "UtVec3dX.hpp"
#include "WsfSixDOF_PID.hpp"
#include "WsfSixDOF_Utils.hpp"
#include "WsfSixDOF_VehicleData.hpp"

namespace wsf
{
namespace six_dof
{
class AutopilotAction;
class Environment;
class Mover;

class WSF_SIX_DOF_EXPORT CommonController
{
public:
   struct sAutopilotControls
   {
      double stickBack           = 0.0;
      double stickRight          = 0.0;
      double rudderRight         = 0.0;
      double throttleMilitary    = 0.0;
      double throttleAfterburner = 0.0;
      double thrustVectorYaw     = 0.0;
      double thrustVectorPitch   = 0.0;
      double thrustVectorRoll    = 0.0;
      double speedBrake          = 0.0;
      double noseWheelSteering   = 0.0;
      double nwsSteering         = 0.0;
      double wheelBrakeLeft      = 0.0;
      double wheelBrakeRight     = 0.0;
   };

   enum eAutopilotTurnDir
   {
      TD_DEFAULT,
      TD_LEFT,
      TD_RIGHT
   };

   struct WaypointNavData
   {
      double turnLeadDist_m    = 0.0;
      double aimHeading_rad    = 0.0;
      double rangeTrack_m      = 0.0;
      double rangeRate_mps     = 0.0;
      double deltaAlt_m        = 0.0;
      double vertSpeed_mps     = 0.0;
      double commandedBank_rad = 0.0;
      bool   executeTurn       = false;
   };

   struct NestedFeedbackLoop
   {
      // PID Middle Loop Timing -- These indicate how many times the inner loop processes before the middle loop
      // processes (typically 10, but could be 5-20)
      ut::optional<int> middleLoopFactor;

      // PID Outer Loop Timing -- These indicate how many times the middle loop processes before the outer loop
      // processes (typically 5-10, but could be 2-20)
      ut::optional<int> outerLoopFactor;

      double GetMiddleLoopInterval_sec()
      {
         int tickCount = middleLoopFactor.value_or(1);
         return tickCount * utils::cDT_RIGID_BODY_SEC;
      }

      double GetOuterLoopInterval_sec()
      {
         int middleLoopTickCount = middleLoopFactor.value_or(1);
         int outerLoopTickCount  = outerLoopFactor.value_or(1);
         int tickCount           = middleLoopTickCount * outerLoopTickCount;
         return tickCount * utils::cDT_RIGID_BODY_SEC;
      }
   };

   CommonController()          = default;
   virtual ~CommonController() = default;

   CommonController& operator=(const CommonController& other) = delete;

   virtual void SetParentVehicle(Mover* aVehicle) = 0;

   virtual void Update(sAutopilotControls& aControls, int64_t aDT_nanosec) = 0;

   // This is called each time step by CommonController. These
   // values can be integrated to determine deltaRoll, for example.
   void AngleDeltas(double aYaw_rad, double aPitch_rad, double aRoll_rad);

   // This is used to reset the integrated yaw angle for delta yaw commands
   void ResetDeltaYawAngle() { mIntegratedDeltaYaw_deg = 0; }

   // This is used to reset the integrated pitch angle for delta yaw commands
   void ResetDeltaPitchAngle() { mIntegratedDeltaPitch_deg = 0; }

   // This is used to reset the integrated roll angle for delta yaw commands
   void ResetDeltaRollAngle() { mIntegratedDeltaRoll_deg = 0; }

   // This is used to reset the integrated yaw and roll angles for delta yaw and roll commands
   void ResetLateralDeltaAngles();

   // This is used to reset the integrated yaw, pitch, and roll angles for delta yaw, pitch, and roll commands
   void ResetAllDeltaAngles();

   // This is used to reset accumulated PID data between evaluations
   void ResetAccumulatedPidData();

   // This is used to reset PID data related to sim-time only
   void ResetAllPidTimings();

   // Returns true when the current waypoint has been achieved
   bool GetWaypointAchieved() const { return mAchievedWaypoint; }

   // This provides a vector of all PID gain data
   std::vector<PidGainData>* GetAutopilotPidGainData(Pid::Type aTableType);

   // Gets PID data from the specified AutopilotPidGroupValueData
   void GetAutopilotPidValues(AutopilotPidGroupValueData* aData);

   // Get PID data based on PID type. Returns true if data is valid.
   bool GetAutopilotPidGainData(Pid::Type aTableType, size_t& aNumElements, PidGainData aPidGainData[]);

   void SetPidGainData(Pid::Type aPidType, int aNumElements, PidGainData aPidGainData[]);

   bool AddNewPidGainElement(Pid::Type aTableType, PidGainData& aPidGainData);

   void SetControlAugmentationModeActive(bool aControlAugmentationIsActive)
   {
      mControlAugmentationModeActive = aControlAugmentationIsActive;
   }
   bool ControlAugmentationModeIsActive() { return mControlAugmentationModeActive; }

   void SetPitchStabilityAugmentationModeActive(bool aPitchStabilityAugmentationIsActive)
   {
      mPitchStabilityAugmentationModeActive = aPitchStabilityAugmentationIsActive;
   }
   void SetYawStabilityAugmentationModeActive(bool aYawStabilityAugmentationIsActive)
   {
      mYawStabilityAugmentationModeActive = aYawStabilityAugmentationIsActive;
   }
   void SetRollStabilityAugmentationModeActive(bool aRollStabilityAugmentationIsActive)
   {
      mRollStabilityAugmentationModeActive = aRollStabilityAugmentationIsActive;
   }
   bool PitchStabilityAugmentationModeIsActive() { return mPitchStabilityAugmentationModeActive; }
   bool YawStabilityAugmentationModeIsActive() { return mYawStabilityAugmentationModeActive; }
   bool RollStabilityAugmentationModeIsActive() { return mRollStabilityAugmentationModeActive; }

   // This provides the CLMax as a function of Mach. Returns true if data is valid.
   bool GetCLMaxMach(double aMach, double& aCLMax) const;

   // This provides the CLMin as a function of Mach. Returns true if data is valid.
   bool GetCLMinMach(double aMach, double& aCLMin) const;

   // This provides the AlphaMax as a function of Mach. Returns true if data is valid.
   bool GetAlphaMaxMach(double aMach, double& aAlphaMax_deg) const;

   // This provides the AlphaMin as a function of Mach. Returns true if data is valid.
   bool GetAlphaMinMach(double aMach, double& aAlphaMin_deg) const;

   // The autopilot uses a AutopilotAction pointer that points to memory that
   // is created and managed/deleted externally.
   AutopilotAction* GetCurrentActivity() const { return mCurrentActivityPtr; }

   // The autopilot uses a AutopilotAction pointer that points to memory that
   // is created and managed/deleted externally.
   void SetCurrentActivity(AutopilotAction* aAPActivity);

   void GetAutoPilotTurnData(double& aAimHeading_rad,
                             double& aStartTurnHdg_rad,
                             double& aTurnRadius_m,
                             double& aTurnAngle_rad,
                             double& aTurnRefPtLat,
                             double& aTurnRefPtLon,
                             bool&   aTurnCW,
                             bool&   aTurning,
                             double& aTurnCenterLat,
                             double& aTurnCenterLon);

   // This returns the type of control being used, such as variants of BankToTurn or YawToTurn
   Control::Method GetControlMethod() { return mControlMethod; }

   // This returns the current autopilot limits and settings
   const AutopilotLimitsAndSettings& GetCurrentLimitsAndSettings() const { return mCurrentLimitsAndSettings; }

   // This sets the autopilot limits and settings
   void SetCurrentLimitsAndSettings(const AutopilotLimitsAndSettings& aData);

   void RevertLimitsAndSettingsToDefaults() { mCurrentLimitsAndSettings = mDefaultLimitsAndSettings; }

   void SetCurrentMinPitchGLimit(double aMinPitchGLimit);
   void SetCurrentMaxPitchGLimit(double aMaxPitchGLimit);
   void SetCurrentMinAlphaLimit(double aMinAlphaLimit);
   void SetCurrentMaxAlphaLimit(double aMaxAlphaLimit);
   void SetCurrentMinPitchRateLimit(double aMinPitchRateLimit);
   void SetCurrentMaxPitchRateLimit(double aMaxPitchRateLimit);
   void SetCurrentMinVertSpeedLimit(double aMinVertSpeedLimit);
   void SetCurrentMaxVertSpeedLimit(double aMaxVertSpeedLimit);
   void SetCurrentMaxYawGLimit(double aMaxYawGLimit);
   void SetCurrentMaxBetaLimit(double aMaxBetaLimit);
   void SetCurrentMaxYawRateLimit(double aMaxYawRateLimit);
   void SetCurrentMaxRollRateLimit(double aMaxRollRateLimit);
   void SetCurrentMaxBankAngleLimit(double aMaxBankAngleLimit);
   void SetCurrentMinForwardAccelLimit(double aMinForwardAccelLimit);
   void SetCurrentMaxForwardAccelLimit(double aMaxForwardAccelLimit);
   void SetCurrentMaxTaxiSpeedLimit(double aMaxTaxiSpeedLimit);
   void SetCurrentMaxTaxiYawRateLimit(double aMaxTaxiYawRateLimit);

   double GetDefaultTurnRollInMultiplier(); // Returns the default turn roll-in multiplier for autopilot turns when
                                            // following waypoints
   double GetDefaultRouteAllowableAngleError_rad(); // Returns the default allowable angle error when flying routes

   // Sets the current allowable angle error when flying routes
   void SetRouteAllowableAngleError_rad(double aValue_rad);

   // Sets the current turn roll-in multiplier for autopilot turns when following waypoints
   void SetCurrentTurnRollInMultiplier(double aValue);

   // Afterburner use is enabled by default, but can be enabled/disabled
   void SetAfterburnerEnabled(bool aEnabled);

   // Afterburner will be used (if enabled) if command exceeds the threshold value
   void SetAfterburnerThreshold(double aValue = 1.0);

   // Speed brake use is enabled by default, but can be enabled/disabled
   void SetSpeedBrakeEnabled(bool aEnabled);

   // Speed brake will be used (if enabled) if command is less than the threshold value
   void SetSpeedBrakeThreshold(double aValue = 0.0);

   bool SetPidGainKP(Pid::Type aTableType, float aControllingValue, float aGainValue);
   bool SetPidGainKI(Pid::Type aTableType, float aControllingValue, float aGainValue);
   bool SetPidGainKD(Pid::Type aTableType, float aControllingValue, float aGainValue);
   bool SetPidAlpha(Pid::Type aTableType, float aControllingValue, float aAlphaValue);
   bool SetPidMaxAccum(Pid::Type aTableType, float aControllingValue, float aMaxAccumValue);

   virtual PID* GetPID_ByType(Pid::Type aTableType) = 0;

   // This provides the effective CL at the specified mach and alpha. Returns true if data is valid.
   bool GetEffectiveCLVsMachAlpha(double aMach, double aAlpha_rad, double& aCL) const;

   // This provides the alpha (in degrees) for the specified mach and CL.
   // It is helpful for determining the alpha for a desired g-load at a specified Mach.
   // Returns true if data is valid.
   bool GetAlphaVsMachCL(double aMach, double aCL, double& aAlpha_deg) const;

protected:
   CommonController(const CommonController& aSrc) = default;

   virtual Mover* GetParentVehicle() = 0;

   void ProcessInput(UtInput& aInput);

   virtual void ProcessInputCommand(UtInput& aInput, const std::string& aCommand) = 0;

   bool ProcessCommonInputCommand(UtInput& aInput, const std::string& aCommand);

   void ProcessPidGroupInputBlock(UtInputBlock& aInputBlock, UtInput& aInput);

   void ProcessLimitsAndSettingsInputBlock(UtInputBlock& aInputBlock);

   // This is the autopilot update function for bank-to-turn vehicles
   void         UpdateBankToTurn(sAutopilotControls& aControls, double aSimTime);
   virtual void ProcessLaternalNavChannelsBankToTurn(double aSimTime) = 0;
   virtual void ProcessVerticalNavChannelBankToTurn(double aSimTime)  = 0;
   virtual void ProcessSpeedChannelBankToTurn(double aSimTime)        = 0;

   // This is the autopilot update function for yaw-to-turn vehicles
   void         UpdateYawToTurn(sAutopilotControls& aControls, double aSimTime);
   virtual void ProcessLaternalNavChannelsYawToTurn(double aSimTime) = 0;
   virtual void ProcessVerticalNavChannelYawToTurn(double aSimTime)  = 0;
   virtual void ProcessSpeedChannelYawToTurn(double aSimTime)        = 0;

   // Lateral modes ------------------------------------------------------------------

   virtual void ProcessLateralNavMode_Bank(double aSimTime);
   virtual void ProcessLateralNavMode_DeltaRoll(double aSimTime);
   virtual void ProcessLateralNavMode_RollRate(double aSimTime);
   virtual void ProcessLateralNavMode_YawWaypoint(double aSimTime);
   virtual void ProcessLateralNavMode_YawPoint(double aSimTime);
   virtual void ProcessLateralNavMode_YawHeading(double aSimTime);
   virtual void ProcessLateralNavMode_YawRate(double aSimTime);
   virtual void ProcessLateralNavMode_NoControl();
   virtual void ProcessLateralNavMode_NoRollControl();
   virtual void ProcessLateralNavMode_NoYawControl();

   virtual void ProcessLateralNavMode_RollWaypoint(double aSimTime);
   virtual void ProcessLateralNavMode_RollPoint(double aSimTime);
   virtual void ProcessLateralNavMode_RollHeading(double aSimTime);

   virtual void CalcLateralNavMode_RollHeadingCore(double aHeading_deg, double aMaxBankAngle_rad, double aSimTime);
   virtual void ProcessStandardLateralNavMode_Bank(double aBankAngle_deg, double aSimTime);

   virtual void ProcessStandardLateralNavMode_RollHeading(double aHeading_deg, double aSimTime);
   virtual void ProcessStandardLateralNavMode_WaypointRollHeading(double aHeading_deg, double aSimTime);
   virtual void ProcessStandardLateralNavMode_RollRate(double aCommandedRollRate_dps, double aSimTime);

   virtual void ProcessLateralNavMode_Beta(double aSimTime);
   virtual void ProcessLateralNavMode_YawGLoad(double aSimTime);

   virtual void ProcessStandardLateralNavMode_YawHeading(double aHeading_deg, double aSimTime);
   virtual void ProcessStandardLateralNavMode_YawRate(double aCommandedYawRate_dps, double aSimTime);
   virtual void ProcessStandardLateralNavMode_Beta(double aCommandedBeta_deg, double aSimTime);

   // Vertical modes -----------------------------------------------------------------

   virtual void ProcessVerticalNavMode_Waypoint(double aSimTime);
   virtual void ProcessVerticalNavMode_Altitude(double aSimTime);
   virtual void ProcessVerticalNavMode_VertSpeed(double aSimTime);
   virtual void ProcessVerticalNavMode_PitchGLoad(double aSimTime);
   virtual void ProcessVerticalNavMode_PitchAng(double aSimTime);
   virtual void ProcessVerticalNavMode_PitchRate(double aSimTime);
   virtual void ProcessVerticalNavMode_FltPathAng(double aSimTime);
   virtual void ProcessVerticalNavMode_DeltaPitch(double aSimTime);
   virtual void ProcessVerticalNavMode_Point(double aSimTime); // (Not yet implemented) TODO
   virtual void ProcessVerticalNavMode_NoControl(double aSimTime);
   virtual void ProcessVerticalNavMode_Alpha(double aSimTime);

   virtual void ProcessStandardVerticalNavMode_Altitude(double aAltitude_ft, double aSimTime);
   virtual void ProcessStandardVerticalNavMode_VertSpeed(double aCommandedVertSpeed_fpm, double aSimTime);
   virtual void ProcessStandardVerticalNavMode_PitchRate(double aCommandedPitchRate_dps, double aSimTime);
   virtual void ProcessStandardVerticalNavMode_Alpha(double aCommandedAlpha_deg, double aSimTime);

   // Speed modes --------------------------------------------------------------------

   virtual double ProcessSpeedMode_Waypoint(double aSimTime);
   virtual double ProcessSpeedMode_ForwardAccel(double aSimTime);
   virtual double ProcessSpeedMode_KIAS(double aSimTime);
   virtual double ProcessSpeedMode_KTAS(double aSimTime);
   virtual double ProcessSpeedMode_Mach(double aSimTime);
   virtual double ProcessSpeedMode_FPS(double aSimTime);
   virtual double ProcessSpeedMode_Throttle(double aSimTime);
   virtual double ProcessSpeedMode_NoControl();
   virtual double ProcessStandardSpeedMode_FPS(double aSpeed_fps, double aSimTime);
   virtual double ProcessStandardForwardAccel(double aForwardAccel_g, double aSimTime);
   virtual double ProcessStandardSpeedMode_Throttle(double aThrottle, double aSimTime);

   // This is used (internally) to calculate g-limits for both yaw and pitch using
   // alpha/beta limits as well as the baseline g-limits
   void CalcAlphaBetaGLimits();

   // This calculates the current g-bias in terms of g-load and alpha, considering
   // the current pitch and roll angles.
   void CalcGBiasData(double& aGBias_g, double& aGBiasAlpha_deg);

   // This returns the current target heading (deg) when using waypoints
   double GetAimHeadingForWaypointNav_deg(double aSimTime);

   // This returns the current target heading (deg) to the current point
   double GetAimHeadingForPoint_deg();

   // These functions are use for calculating taxi turn radius and turn rates (yaw rates)
   double CalcTurnRateBasedOnTurnRadiusAndCurrentSpeed_dps(double aTurnRadius_ft);
   double CalcTurnRadiusBasedOnTurnRateAndCurrentSpeed_ft(double aTurnRate_dps);

   void EnforceControlLimits();

   void EnforceSingleControlLimit(double& aValue, const double& aMinValue, const double& aMaxValue);

   int64_t mLastUpdateTime_nanosec = 0;

   NestedFeedbackLoop mVerticalControlLoop; // Used for initialization of PIDs
   NestedFeedbackLoop mLateralControlLoop;  // Used for initialization of PIDs
   NestedFeedbackLoop mSpeedControlLoop;    // Used for initialization of PIDs

   WaypointNavData mNavData;

   // This is the output controls from the autopilot
   sAutopilotControls mControlOutputs;

   AutopilotLimitsAndSettings mDefaultLimitsAndSettings;
   AutopilotLimitsAndSettings mCurrentLimitsAndSettings;

   // The Autopilot uses a AutopilotAction pointer that points to memory that
   // is created and managed/deleted externally. This is the command to the autopilot.
   AutopilotAction* mCurrentActivityPtr = nullptr;

   // This is the control method used by the vehicle/autopilot
   Control::Method mControlMethod = Control::Undefined;

   bool mTurning          = false;
   bool mAchievedWaypoint = false;

   double   mStartTurnHdg_rad = 0.0;
   double   mTurnRadius_m     = 0.0;
   double   mTurnAngle_rad    = 0.0;
   bool     mTurnCW           = true;
   UtLLAPos mTurnRefPoint;

   double mAimHeading_rad = 0.0;
   double mTurnCenterLat  = 0.0;
   double mTurnCenterLon  = 0.0;

   double mExternalFlapsControlValue       = 0.0;
   double mExternalSpoilersControlValue    = 0.0;
   double mExternalSpeedBrakesControlValue = 0.0;
   double mExternalAfterburnerControlValue = 0.0;

   double mLastCommandedVertRate_fpm   = 0.0;
   double mLastCommandedAlpha_deg      = 0.0;
   double mLastCommandedYawRate_dps    = 0.0;
   double mLastCommandedBeta_deg       = 0.0;
   double mLastComandedBankAngle_deg   = 0.0;
   double mLastCommandedRollRate_dps   = 0.0;
   double mLastCommandedSpeed_fps      = 0.0;
   double mLastCommandedForwardAccel_g = 0.0;

   PID mAlphaPID;
   PID mVerticalSpeedPID;
   PID mPitchAnglePID;
   PID mPitchRatePID;
   PID mFlightPathAnglePID;
   PID mDeltaPitchPID;
   PID mAltitudePID;
   PID mBetaPID;
   PID mYawRatePID;
   PID mYawHeadingPID;
   PID mTaxiHeadingPID;
   PID mRollRatePID;
   PID mDeltaRollPID;
   PID mBankAnglePID;
   PID mRollHeadingPID;
   PID mForwardAccelPID;
   PID mSpeedPID;
   PID mTaxiForwardAccelPID;
   PID mTaxiSpeedPID;
   PID mTaxiYawRatePID;

   double mLimitedBeta_deg     = 0.0;
   double mLimitedMinAlpha_deg = 0.0;
   double mLimitedMaxAlpha_deg = 0.0;

   double mIntegratedDeltaYaw_deg   = 0.0;
   double mIntegratedDeltaPitch_deg = 0.0;
   double mIntegratedDeltaRoll_deg  = 0.0;

   bool mControlAugmentationModeActive        = false;
   bool mYawStabilityAugmentationModeActive   = false;
   bool mPitchStabilityAugmentationModeActive = false;
   bool mRollStabilityAugmentationModeActive  = false;

   double mCurrentGBias_g        = 1.0;
   double mCurrentGBiasAlpha_deg = 0.0;

   UtCloneablePtr<UtTable::Curve> mCLMaxMachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mCLMinMachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mAlphaMaxMachTablePtr{nullptr};
   UtCloneablePtr<UtTable::Curve> mAlphaMinMachTablePtr{nullptr};

   UtCloneablePtr<UtTable::Table> mEffectiveCLVsMachAlphaTablePtr{nullptr};
   UtCloneablePtr<UtTable::Table> mAlphaVsMachCLTablePtr{nullptr};

   const Environment* mEnvironment = nullptr;
};
} // namespace six_dof
} // namespace wsf

#endif // COMMONAIRCOMMONCONTROLLER_H
