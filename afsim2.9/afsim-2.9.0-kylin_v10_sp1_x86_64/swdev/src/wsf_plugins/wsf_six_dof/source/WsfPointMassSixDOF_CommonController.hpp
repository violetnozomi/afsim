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

#ifndef WSFPOINTMASSSIXDOFCOMMONCONTROLLER_HPP
#define WSFPOINTMASSSIXDOFCOMMONCONTROLLER_HPP

#include <vector>

#include "WsfPointMassSixDOF_Utils.hpp"
#include "WsfSixDOF_CommonController.hpp"
#include "WsfSixDOF_KinematicState.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT PointMassCommonController : public CommonController
{
public:
   PointMassCommonController() = default;
   explicit PointMassCommonController(UtInput& aInput, const std::string& aCommand);
   virtual ~PointMassCommonController() = default;

   PointMassCommonController& operator=(const PointMassCommonController& other) = delete;

   PointMassCommonController* Clone() const;

   void ReadSupportFile(const std::string& aFilename);

   void Update(sAutopilotControls& aControls, int64_t aDT) override;

   void SetParentVehicle(Mover* aVehicle) override { mParentVehiclePtr = utils::CastToPointMass(aVehicle); }

protected:
   PointMassCommonController(const PointMassCommonController& aSrc);

   void LoadConfigFile(const std::string& aFilename);
   void ProcessInputCommand(UtInput& aInput, const std::string& aCommand) override;

   void ProcessLaternalNavChannelsBankToTurn(double aSimTime) override;
   void ProcessVerticalNavChannelBankToTurn(double aSimTime) override;
   void ProcessSpeedChannelBankToTurn(double aSimTime) override;

   void ProcessLaternalNavChannelsYawToTurn(double aSimTime) override;
   void ProcessVerticalNavChannelYawToTurn(double aSimTime) override;
   void ProcessSpeedChannelYawToTurn(double aSimTime) override;

   void ProcessStandardVerticalNavMode_Alpha(double aCommandedAlpha_deg, double aSimTime) override;
   void ProcessStandardLateralNavMode_Beta(double aCommandedBeta_deg, double aSimTime) override;
   void ProcessStandardLateralNavMode_RollRate(double aCommandedRollRate_dps, double aSimTime) override;

   void ProcessLateralNavMode_DeltaRoll(double aSimTime) override;
   void ProcessVerticalNavMode_DeltaPitch(double aSimTime) override;

   PID* GetPID_ByType(Pid::Type aTableType) override;

   void SetControllingValueForAllPIDs(double aControllingValue);

private:
   Mover* GetParentVehicle() override;

   PointMassMover* mParentVehiclePtr = nullptr;
};
} // namespace six_dof
} // namespace wsf

#endif // MRMCOMMONCONTROLLER_H
