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

#ifndef WSFRIGIDBODYSIXDOFCOMMONCONTROLLER_HPP
#define WSFRIGIDBODYSIXDOFCOMMONCONTROLLER_HPP

#include "WsfRigidBodySixDOF_Utils.hpp"
#include "WsfRigidBodySixDOF_VehicleData.hpp"
#include "WsfSixDOF_CommonController.hpp"
#include "WsfSixDOF_KinematicState.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT RigidBodyCommonController : public CommonController
{
public:
   RigidBodyCommonController() = default;
   explicit RigidBodyCommonController(UtInput& aInput, const std::string& aCommand);
   ~RigidBodyCommonController() = default;

   RigidBodyCommonController& operator=(const RigidBodyCommonController& other) = delete;

   RigidBodyCommonController* Clone() const;

   void ReadSupportFile(const std::string& aFilename);

   void Update(sAutopilotControls& aControls, int64_t aSimTime_nanosec) override;

   void SetParentVehicle(Mover* aVehicle) override { mParentVehiclePtr = utils::CastToRigidBody(aVehicle); }

   // This is used to "enable" taxi mode. When in taxi mode, the vehicle may use different
   // controls. For example, a bank-to-turn aircraft will use rudder pedals to control
   // left/right motion rather than the control stick to control roll left/right.
   void SetTaxiMode(bool aTaxiModeActive);

   // Returns true if taxi mode is active
   bool GetTaxiMode() const;

   void SetDesiredTaxiRadius(double aTaxiRadius_ft);

   // This provides the stickBack position to create a zero moment at the specified mach and alpha.
   // Returns true if data is valid.
   bool GetStickForZeroMomentVsMachAlpha(double aMach, double aAlpha_rad, double aThrottleSetting, double& aStickBack) const;

   // If true, a simple yaw damper should be used
   bool UseSimpleYawDamper() const { return mUseSimpleYawDamper; }

protected:
   RigidBodyCommonController(const RigidBodyCommonController& aSrc);

   void LoadConfigFile(const std::string& aFilename);
   void ProcessInputCommand(UtInput& aInput, const std::string& aCommand) override;

   void ProcessLaternalNavChannelsBankToTurn(double aSimTime) override;
   void ProcessVerticalNavChannelBankToTurn(double aSimTime) override;
   void ProcessSpeedChannelBankToTurn(double aSimTime) override;

   void ProcessLaternalNavChannelsYawToTurn(double aSimTime) override;
   void ProcessVerticalNavChannelYawToTurn(double aSimTime) override;
   void ProcessSpeedChannelYawToTurn(double aSimTime) override;

   // Lateral modes ------------------------------------------------------------------

   void ProcessLateralNavMode_Taxi_Waypoint(double aSimTime);
   void ProcessLateralNavMode_Taxi_Heading(double aSimTime);
   void ProcessLateralNavMode_Taxi_YawRate(double aSimTime);

   void ProcessStandardLateralNavMode_Beta(double aCommandedBeta_deg, double aSimTime) override;

   void ProcessStandardLateralNavMode_TaxiHeading(double aHeading_deg, double aTurnRate_dps, double aSimTime);
   void ProcessStandardLateralNavMode_TaxiYawRate(double aCommandedYawRate_dps, double aSimTime);

   // Vertical modes -----------------------------------------------------------------

   void ProcessStandardVerticalNavMode_Alpha(double aCommandedAlpha_deg, double aSimTime) override;

   void ProcessVerticalNavMode_Taxi(double aSimTime); // (Not yet implemented) TODO

   // Speed modes --------------------------------------------------------------------

   double ProcessSpeedMode_Taxi_Waypoint(double aSimTime);     // (Not yet implemented) TODO
   double ProcessSpeedMode_Taxi_ForwardAccel(double aSimTime); // (Not yet implemented) TODO
   double ProcessSpeedMode_Taxi_KIAS(double aSimTime);         // (Not yet implemented) TODO
   double ProcessSpeedMode_Taxi_KTAS(double aSimTime);         // (Not yet implemented) TODO
   double ProcessStandardSpeedModeTaxi_FPS(double aSpeed_fps, double aSimTime);

   // --------------------------------------------------------------------------------

   PID* GetPID_ByType(Pid::Type aTableType) override;

   // This returns the current aim heading (deg) and turn rate (deg/sec)
   void GetAimHeadingAndTurnRateForTaxiWaypointNav_deg(double aSimTime, double& aAimHeading_deg, double& aTurnRate_dps);

   void SetControllingValueForAllPIDs(double aControllingValue);

   // This is updated in the Update() function so that the state data can be easily
   // used in other functions in this class

private:
   Mover* GetParentVehicle() override;

   RigidBodyMover* mParentVehiclePtr = nullptr;

   double mCombinedWheelBraking = 0.0;
   double mWheelBrakingLeft     = 0.0;
   double mWheelBrakingRight    = 0.0;

   double mExternalGearControlValue          = 0.0;
   double mExternalLftWheelBrakeControlValue = 0.0;
   double mExternalRgtWheelBrakeControlValue = 0.0;

   bool mTaxiModeActive = false;

   double mMinimumTaxiTurnRadius_ft = 50.0;
   double mDesiredTaxiTurnRadius_ft = 50.0;

   UtCloneablePtr<UtTable::Table> mStickForZeroMomentVsMachAlphaTablePtr{nullptr};
   UtCloneablePtr<UtTable::Table> mThrustDeltaStickForZeroMomentVsMachAlphaTablePtr{nullptr};
   UtCloneablePtr<UtTable::Table> mXcgDeltaStickForZeroMomentVsMachAlphaTablePtr{nullptr};
   UtCloneablePtr<UtTable::Table> mZcgDeltaStickForZeroMomentVsMachAlphaTablePtr{nullptr};

   bool mUseSimpleYawDamper = false;
};
} // namespace six_dof
} // namespace wsf

#endif // WSFRIGIDBODYSIXDOFCOMMONCONTROLLER_H
