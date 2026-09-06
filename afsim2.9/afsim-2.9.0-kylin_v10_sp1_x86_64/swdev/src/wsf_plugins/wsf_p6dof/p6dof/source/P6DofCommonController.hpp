// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef P6DOFCOMMONCONTROLLER_H
#define P6DOFCOMMONCONTROLLER_H

#include "p6dof_export.h"

#include <vector>

#include "P6DofKinematicState.hpp"
#include "P6DofPID.hpp"
#include "P6DofVehicleData.hpp"
#include "UtCloneablePtr.hpp"
#include "UtLLAPos.hpp"
#include "UtTable.hpp"
#include "UtVec3dX.hpp"

class P6DofAtmosphere;
class P6DofAutopilotAction;
class P6DofAutopilotWaypointUtility;
class P6DofVehicle;
class UtInput;
class UtInputBlock;

class P6DOF_EXPORT P6DofCommonController
{
public:
   struct sAutopilotControls
   {
      double stickBack;
      double stickRight;
      double rudderRight;
      double throttleMilitary;
      double throttleAfterburner;
      double thrustVectorYaw;
      double thrustVectorPitch;
      double thrustVectorRoll;
      double speedBrake;
      double noseWheelSteering;
      double nwsSteering;
      double wheelBrakeLeft;
      double wheelBrakeRight;
   };

   enum eAutopilotTurnDir
   {
      TD_DEFAULT,
      TD_LEFT,
      TD_RIGHT
   };

   struct WaypointNavData
   {
      double turnLeadDist_m{0.0};
      double aimHeading_rad{0.0};
      double rangeTrack_m{0.0};
      double rangeRate_mps{0.0};
      double deltaAlt_m{0.0};
      double vertSpeed_mps{0.0};
      double commandedBank_rad{0.0};
      bool   executeTurn{false};
   };

   explicit P6DofCommonController(const std::string& aConfigFile);
   explicit P6DofCommonController(UtInput& aInput, const std::string& aCommand);
   virtual ~P6DofCommonController() = default;

   bool UseLegacyBetaControl() { return mUseLegacyBeta; }

   P6DofCommonController& operator=(const P6DofCommonController& other) = delete;

   P6DofCommonController* Clone() const;

   void ReadSupportFile(const std::string& aFilename);

   void Update(P6DofVehicle* aParentObject, P6DofKinematicState& aState, sAutopilotControls& aControls, double aDT);

   // This is called each time step by P6DofPilotController. These PilotController passes these
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

   // The autopilot uses a P6DofAutopilotAction pointer that points to memory that
   // is created and managed/deleted externally.
   P6DofAutopilotAction* GetCurrentActivity() const { return mCurrentActivityPtr; }

   // The autopilot uses a P6DofAutopilotAction pointer that points to memory that
   // is created and managed/deleted externally.
   void SetCurrentActivity(P6DofAutopilotAction* aAPActivity);

   // This is used to "enable" taxi mode. When in taxi mode, the vehicle may use different
   // controls. For example, a bank-to-turn aircraft will use rudder pedals to control
   // left/right motion rather than the control stick to control roll left/right.
   void SetTaxiMode(bool aTaxiModeActive);

   // Returns true is taxi mode is active
   bool GetTaxiMode() const;

   // Returns true when the current waypoint has been achieved
   bool GetWaypointAchieved() { return mAchievedWaypoint; }

   // Returns various turn data parameters
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

   // Returns true when the afterburner use is enabled
   bool GetAfterburnerEnabled();

   // Afterburner use is enabled by default, but can be enabled/disabled
   void SetAfterburnerEnabled(bool aEnabled);

   // Returns the afterburner threshold value
   double GetAfterburnerThreshold();

   // Afterburner will be used (if enabled) if command exceeds the threshold value
   void SetAfterburnerThreshold(double aValue = 1.0);

   // Returns true when the speed brake use is enabled
   bool GetSpeedBrakeEnabled();

   // Speed brake use is enabled by default, but can be enabled/disabled
   void SetSpeedBrakeEnabled(bool aEnabled);

   // Returns the speed brake threshold value
   double GetSpeedBrakeThreshold();

   // Speed brake will be used (if enabled) if command is less than the threshold value
   void SetSpeedBrakeThreshold(double aValue = 0.0);

   // Get PID data based on PID type. Returns true if data is valid.
   bool GetAutopilotPidGainData(P6DOF::Pid::Type   aTableType,
                                size_t&            aNumElements,
                                P6DOF::PidGainData aPidGainData[],
                                unsigned char&     aPidFlags);

   // This provides a vector of all PID gain data
   std::vector<P6DOF::PidGainData>* GetAutopilotPidGainData(P6DOF::Pid::Type aTableType, unsigned char& aPidFlags);

   // Gets PID data from the specified AutopilotPidGroupValueData
   void GetAutopilotPidValues(P6DOF::AutopilotPidGroupValueData* aData);

   // This returns the type of control being used, such as variants of BankToTurn or YawToTurn
   P6DOF::Control::Method GetControlMethod();

   // These functions get/set the middle and outer loop rate factors

   unsigned char GetVerticalMiddleLoopFactor() { return mVerticalMiddleLoopFactor; }
   unsigned char GetLateralMiddleLoopFactor() { return mLateralMiddleLoopFactor; }
   unsigned char GetSpeedMiddleLoopFactor() { return mSpeedMiddleLoopFactor; }
   unsigned char GetVerticalOuterLoopFactor() { return mVerticalOuterLoopFactor; }
   unsigned char GetLateralOuterLoopFactor() { return mLateralOuterLoopFactor; }
   unsigned char GetSpeedOuterLoopFactor() { return mSpeedOuterLoopFactor; }

   void SetVerticalMiddleLoopFactor(unsigned char aValue) { mVerticalMiddleLoopFactor = aValue; }
   void SetLateralMiddleLoopFactor(unsigned char aValue) { mLateralMiddleLoopFactor = aValue; }
   void SetSpeedMiddleLoopFactor(unsigned char aValue) { mSpeedMiddleLoopFactor = aValue; }
   void SetVerticalOuterLoopFactor(unsigned char aValue) { mVerticalOuterLoopFactor = aValue; }
   void SetLateralOuterLoopFactor(unsigned char aValue) { mLateralOuterLoopFactor = aValue; }
   void SetSpeedOuterLoopFactor(unsigned char aValue) { mSpeedOuterLoopFactor = aValue; }

   void SetPidGainData(P6DOF::Pid::Type aPidType, int aNumElements, P6DOF::PidGainData aPidGainData[], unsigned char aPidFlags);

   bool AddNewPidGainElement(P6DOF::Pid::Type aTableType, P6DOF::PidGainData& aPidGainData);
   bool SetPidGainKP(P6DOF::Pid::Type aTableType, float aControllingValue, float aGainValue);
   bool SetPidGainKI(P6DOF::Pid::Type aTableType, float aControllingValue, float aGainValue);
   bool SetPidGainKD(P6DOF::Pid::Type aTableType, float aControllingValue, float aGainValue);
   bool SetPidMaxAccum(P6DOF::Pid::Type aTableType, float aControllingValue, float aMaxAccumValue);

   // This reverts autopilot limits and settings to the default values
   void RevertLimitsAndSettingsToDefaults();

   // This sets the autopilot limits and settings
   void SetCurrentLimitsAndSettings(P6DOF::AutopilotLimitsAndSettings& aData);

   // This returns the current autopilot limits and settings
   const P6DOF::AutopilotLimitsAndSettings& GetCurrentLimitsAndSettings() const { return mCurrentLimitsAndSettings; }

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
   void SetCurrentTurnRollInMultiplier(double aValue); // Sets the current turn roll-in multiplier for autopilot turns
                                                       // when following waypoints
   void SetRouteAllowableAngleError_rad(double aValue_rad); // Sets the current allowable angle error when flying routes

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

   void SetDesiredTaxiRadius(double aTaxiRadius_ft);

   double GetCurrentAlphaMax_deg() { return mCurrentLimitsAndSettings.alpha_Max; }
   double GetCurrentAlphaMin_deg() { return mCurrentLimitsAndSettings.alpha_Min; }
   double GetCurrentBetaMax_deg() { return mCurrentLimitsAndSettings.beta_Max; }
   double GetCurrentTurnRollInMultiplier(); // Returns the current turn roll-in multiplier for autopilot turns when
                                            // following waypoints
   double GetDefaultTurnRollInMultiplier(); // Returns the default turn roll-in multiplier for autopilot turns when
                                            // following waypoints
   double GetCurrentRouteAllowableAngleError_rad(); // Returns the current allowable angle error when flying routes
   double GetDefaultRouteAllowableAngleError_rad(); // Returns the default allowable angle error when flying routes

   // This provides the CLMax as a function of Mach. Returns true if data is valid.
   bool GetCLMaxMach(double aMach, double& aCLMax) const;

   // This provides the CLMin as a function of Mach. Returns true if data is valid.
   bool GetCLMinMach(double aMach, double& aCLMin) const;

   // This provides the AlphaMax as a function of Mach. Returns true if data is valid.
   bool GetAlphaMaxMach(double aMach, double& aAlphaMax_deg) const;

   // This provides the AlphaMin as a function of Mach. Returns true if data is valid.
   bool GetAlphaMinMach(double aMach, double& aAlphaMin_deg) const;

   // This provides the stickBack position to create a zero moment at the specified mach and alpha.
   // Returns true if data is valid.
   bool GetStickForZeroMomentVsMachAlpha(double aMach, double aAlpha_rad, double aThrottleSetting, double& aStickBack) const;

   // This provides the effective CL at the specified mach and alpha. Returns true if data is valid.
   bool GetEffectiveCLVsMachAlpha(double aMach, double aAlpha_rad, double& aCL) const;

   // This provides the alpha (in degrees) for the specified mach and CL.
   // It is helpful for determining the alpha for a desired g-load at a specified Mach.
   // Returns true if data is valid.
   bool GetAlphaVsMachCL(double aMach, double aCL, double& aAlpha_deg) const;

   // If true, a simple yaw damper should be used
   bool UseSimpleYawDamper() const { return mUseSimpleYawDamper; }

protected:
   P6DofCommonController(const P6DofCommonController& aSrc);

   void LoadConfigFile(const std::string& aFilename);
   void ProcessInput(UtInput& aInput);
   void ProcessInputCommand(UtInput& aInput, const std::string& aCommand);
   void ProcessPidGroupInputBlock(UtInputBlock& aInputBlock, UtInput& aInput);

   // This function processes the data for a single PID. It will provide an error warning if a
   // mixture of tabular and scalar gain data is used.
   void ProcessPidInputBlock(UtInputBlock& aInputBlock, UtInput& aInput, P6DofPID& aPID);

   void ProcessPidGainTableBlock(UtInputBlock&       aInputBlock,
                                 P6DOF::PidGainData& aTableElement,
                                 bool&               aValidControllingValue,
                                 bool&               aValidKpGain,
                                 bool&               aValidKiGain,
                                 bool&               aValidKdGain,
                                 bool&               aValidMaxAccum,
                                 bool&               aValidMaxErrorZero,
                                 bool&               aValidMinErrorZero,
                                 bool&               aValidLowpassAlpha,
                                 bool&               aValidKtAntiWindup,
                                 unsigned char&      aPackedFlags);

   void ProcessLimitsAndSettingsInputBlock(UtInputBlock& aInputBlock);

   // This is the autopilot update function for bank-to-turn vehicles
   void UpdateBankToTurn(sAutopilotControls& aControls, double aDT);

   // This is the autopilot update function for bank-to-turn vehicles
   void UpdateYawToTurn(sAutopilotControls& aControls, double aDT);

   void EnforceControlLimits();
   void EnforceSingleControlLimit(double& aValue, const double& aMinValue, const double& aMaxValue);

   void ProcessLaternalNavChannelsBankToTurn(double aDT);
   void ProcessVerticalNavChannelBankToTurn(double aDT);
   void ProcessSpeedChannelBankToTurn(double aDT);

   void ProcessLaternalNavChannelsYawToTurn(double aDT);
   void ProcessVerticalNavChannelYawToTurn(double aDT);
   void ProcessSpeedChannelYawToTurn(double aDT);

   // Lateral modes ------------------------------------------------------------------

   void ProcessLateralNavMode_RollWaypoint(double aDT);
   void ProcessLateralNavMode_RollPoint(double aDT);
   void ProcessLateralNavMode_RollHeading(double aDT);
   void ProcessLateralNavMode_Bank(double aDT);
   void ProcessLateralNavMode_DeltaRoll(double aDT);
   void ProcessLateralNavMode_RollRate(double aDT);
   void ProcessLateralNavMode_YawWaypoint(double aDT);
   void ProcessLateralNavMode_YawPoint(double aDT);
   void ProcessLateralNavMode_YawHeading(double aDT);
   void ProcessLateralNavMode_YawRate(double aDT);
   void ProcessLateralNavMode_YawGLoad(double aDT);
   void ProcessLateralNavMode_Taxi_Waypoint(double aDT);
   void ProcessLateralNavMode_Taxi_Heading(double aDT);
   void ProcessLateralNavMode_Taxi_YawRate(double aDT);
   void ProcessLateralNavMode_Beta(double aBeta);
   void ProcessLateralNavMode_NoControl();
   void ProcessLateralNavMode_NoRollControl();
   void ProcessLateralNavMode_NoYawControl();

   void CalcLateralNavMode_RollHeadingCore(double aHeading_deg, double aMaxBankAngle_rad, double aDT);
   void ProcessStandardLateralNavMode_RollHeading(double aHeading_deg, double aDT);
   void ProcessStandardLateralNavMode_WaypointRollHeading(double aHeading_deg, double aDT);
   void ProcessStandardLateralNavMode_Bank(double aBankAngle_deg, double aDT);
   void ProcessStandardLateralNavMode_RollRate(double aCommandedRollRate_dps, double aDT);

   void ProcessStandardLateralNavMode_YawHeading(double aHeading_deg, double aDT);
   void ProcessStandardLateralNavMode_YawRate(double aCommandedYawRate_dps, double aDT);
   void ProcessStandardLateralNavMode_Beta(double aCommandedBeta_deg, double aDT);
   void ProcessStandardLateralNavMode_TaxiHeading(double aHeading_deg, double aTurnRate_dps, double aDT);
   void ProcessStandardLateralNavMode_TaxiYawRate(double aCommandedYawRate_dps, double aDT);

   // Vertical modes -----------------------------------------------------------------

   void ProcessVerticalNavMode_Waypoint(double aDT);
   void ProcessVerticalNavMode_Altitude(double aDT);
   void ProcessVerticalNavMode_VertSpeed(double aDT);
   void ProcessVerticalNavMode_PitchGLoad(double aDT);
   void ProcessVerticalNavMode_PitchAng(double aDT);
   void ProcessVerticalNavMode_PitchRate(double aDT);
   void ProcessVerticalNavMode_FltPathAng(double aDT);
   void ProcessVerticalNavMode_DeltaPitch(double aDT);
   void ProcessVerticalNavMode_Point(double aDT); // (Not yet implemented) TODO
   void ProcessVerticalNavMode_Taxi(double aDT);  // (Not yet implemented) TODO
   void ProcessVerticalNavMode_NoControl(double aDT);
   void ProcessVerticalNavMode_Alpha(double aDT);

   void ProcessStandardVerticalNavMode_Altitude(double aAltitude_ft, double aDT);
   void ProcessStandardVerticalNavMode_VertSpeed(double aCommandedVertSpeed_fpm, double aDT);
   void ProcessStandardVerticalNavMode_Alpha(double aCommandedAlpha_deg, double aDT);

   // Speed modes --------------------------------------------------------------------

   double ProcessSpeedMode_Waypoint(double aDT);
   double ProcessSpeedMode_ForwardAccel(double aDT);
   double ProcessSpeedMode_KIAS(double aDT);
   double ProcessSpeedMode_KTAS(double aDT);
   double ProcessSpeedMode_Mach(double aDT);
   double ProcessSpeedMode_FPS(double aDT);
   double ProcessSpeedMode_Taxi_Waypoint(double aDT);     // (Not yet implemented) TODO
   double ProcessSpeedMode_Taxi_ForwardAccel(double aDT); // (Not yet implemented) TODO
   double ProcessSpeedMode_Taxi_KIAS(double aDT);         // (Not yet implemented) TODO
   double ProcessSpeedMode_Taxi_KTAS(double aDT);         // (Not yet implemented) TODO
   double ProcessSpeedMode_NoControl();
   double ProcessStandardSpeedMode_FPS(double aSpeed_fps, double aDT);
   double ProcessStandardForwardAccel(double aForwardAccel_g, double aDT);
   double ProcessStandardSpeedModeTaxi_FPS(double aSpeed_fps, double aDT);

   // --------------------------------------------------------------------------------

   P6DofPID* GetPID_ByType(P6DOF::Pid::Type aTableType);

   // This returns the current target heading (deg) when using waypoints
   double GetAimHeadingForWaypointNav_deg(double aDT);

   // This returns the current aim heading (deg) and turn rate (deg/sec)
   void GetAimHeadingAndTurnRateForTaxiWaypointNav_deg(double aDT, double& aAimHeading_deg, double& aTurnRate_dps);

   // These functions are use for calculating taxi turn radius and turn rates (yaw rates)
   double CalcTurnRateBasedOnTurnRadiusAndCurrentSpeed_dps(double aTurnRadius_ft);
   double CalcTurnRadiusBasedOnTurnRateAndCurrentSpeed_ft(double aTurnRate_dps);

   // This returns the current target heading (deg) to the current point
   double GetAimHeadingForPoint_deg();

   // This is used (internally) to calculate g-limits for both yaw and pitch using
   // alpha/beta limits as well as the baseline g-limits
   void CalcAlphaBetaGLimits();

   void SetControllingValueForAllPIDs(double aControllingValue);

   // This calculates the current g-bias in terms of g-load and alpha, considering
   // the current pitch and roll angles.
   void CalcGBiasData(double& aGBias_g, double& aGBiasAlpha_deg);

   // The Autopilot uses a P6DofAutopilotActivity pointer that points to memory that
   // is created and managed/deleted externally. This is the command to the autopilot.
   P6DofAutopilotAction* mCurrentActivityPtr;

   // This is updated in the Update() function so that the state data can be easily
   // used in other functions in this class
   P6DofKinematicState mState;

   // This is the output controls from the autopilot
   sAutopilotControls mControlOutputsP6Dof;

   double mCombinedWheelBraking;
   double mWheelBrakingLeft;
   double mWheelBrakingRight;

   bool mAchievedWaypoint;

   double mStartTurnHdg_rad;
   double mTurnRadius_m;
   double mTurnAngle_rad;

   bool     mTurnCW;
   bool     mTurning;
   UtLLAPos mTurnRefPoint;

   double mAimHeading_rad;
   double mTurnCenterLat;
   double mTurnCenterLon;

   double mExternalGearControlValue;
   double mExternalFlapsControlValue;
   double mExternalSpoilersControlValue;
   double mExternalSpeedBrakesControlValue;
   double mExternalAfterburnerControlValue;
   double mExternalLftWheelBrakeControlValue;
   double mExternalRgtWheelBrakeControlValue;

   // PID Middle Loop Timings -- These indicate how many times the inner loop processes
   // before the middle loop processes (typically 10, but could be 5-20)
   unsigned char mVerticalMiddleLoopFactor;
   unsigned char mLateralMiddleLoopFactor;
   unsigned char mSpeedMiddleLoopFactor;

   // PID Outer Loop Timings -- These indicate how many times the middle loop processes
   // before the outer loop processes (typically 5-10, but could be 2-20)
   unsigned char mVerticalOuterLoopFactor;
   unsigned char mLateralOuterLoopFactor;
   unsigned char mSpeedOuterLoopFactor;

   // These count how many times the inner loop processed since the last middle loop update
   unsigned char mVerticalMiddleLoopCount;
   unsigned char mLateralMiddleLoopCount;
   unsigned char mSpeedMiddleLoopCount;

   // These count how many times the middle loop processed since the last outer loop update
   unsigned char mVerticalOuterLoopCount;
   unsigned char mLateralOuterLoopCount;
   unsigned char mSpeedOuterLoopCount;

   // These flags indicate if the outer loop should execute during the current frame
   bool mExecuteVerticalOuterLoop;
   bool mExecuteLateralOuterLoop;
   bool mExecuteSpeedOuterLoop;

   // These flags indicate if the middle loop should execute during the current frame
   bool mExecuteVerticalMiddleLoop;
   bool mExecuteLateralMiddleLoop;
   bool mExecuteSpeedMiddleLoop;

   // This is the control method used by the vehicle/autopilot
   P6DOF::Control::Method mControlMethod;

   P6DofVehicle* mParentObjectPtr;

   double mLastCommandedVertRate_fpm;
   double mLastCommandedAlpha_deg;
   double mLastCommandedYawRate_dps;
   double mLastCommandedBeta_deg;
   double mLastComandedBankAngle_deg;
   double mLastCommandedRollRate_dps;
   double mLastCommandedSpeed_fps;
   double mLastCommandedForwardAccel_g;

   P6DofPID mAlphaPID;
   P6DofPID mVertSpeedPID;
   P6DofPID mPitchAnglePID;
   P6DofPID mPitchRatePID;
   P6DofPID mFltpathAnglePID;
   P6DofPID mDeltaPitchPID;
   P6DofPID mAltitudePID;
   P6DofPID mBetaPID;
   P6DofPID mYawRatePID;
   P6DofPID mYawHeadingPID;
   P6DofPID mTaxiHeadingPID;
   P6DofPID mRollRatePID;
   P6DofPID mDeltaRollPID;
   P6DofPID mBankAnglePID;
   P6DofPID mRollHeadingPID;
   P6DofPID mForwardAccelPID;
   P6DofPID mSpeedPID;
   P6DofPID mTaxiForwardAccelPID;
   P6DofPID mTaxiSpeedPID;
   P6DofPID mTaxiYawRatePID;

   P6DOF::AutopilotLimitsAndSettings mDefaultLimitsAndSettings;
   P6DOF::AutopilotLimitsAndSettings mCurrentLimitsAndSettings;

   bool mTaxiModeActive;

   double mLimitedBeta_deg;
   double mLimitedMinAlpha_deg;
   double mLimitedMaxAlpha_deg;

   WaypointNavData mNavData;

   double mIntegratedDeltaYaw_deg;
   double mIntegratedDeltaPitch_deg;
   double mIntegratedDeltaRoll_deg;

   bool mControlAugmentationModeActive;
   bool mPitchStabilityAugmentationModeActive;
   bool mYawStabilityAugmentationModeActive;
   bool mRollStabilityAugmentationModeActive;

   double mMinimumTaxiTurnRadius_ft;
   double mDesiredTaxiTurnRadius_ft;

   double mCurrentGBias_g;
   double mCurrentGBiasAlpha_deg;

   UtCloneablePtr<UtTable::Curve> mCLMaxMachTablePtr;
   UtCloneablePtr<UtTable::Curve> mCLMinMachTablePtr;
   UtCloneablePtr<UtTable::Curve> mAlphaMaxMachTablePtr;
   UtCloneablePtr<UtTable::Curve> mAlphaMinMachTablePtr;

   UtCloneablePtr<UtTable::Table> mStickForZeroMomentVsMachAlphaTablePtr;
   UtCloneablePtr<UtTable::Table> mThrustDeltaStickForZeroMomentVsMachAlphaTablePtr;
   UtCloneablePtr<UtTable::Table> mXcgDeltaStickForZeroMomentVsMachAlphaTablePtr;
   UtCloneablePtr<UtTable::Table> mZcgDeltaStickForZeroMomentVsMachAlphaTablePtr;

   UtCloneablePtr<UtTable::Table> mEffectiveCLVsMachAlphaTablePtr;
   UtCloneablePtr<UtTable::Table> mAlphaVsMachCLTablePtr;

   bool mUseSimpleYawDamper;

   P6DofAtmosphere* mAtmosphere;

   bool mUseLegacyBeta = true;
};

#endif // P6DOFCOMMONCONTROLLER_H
