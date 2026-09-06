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

#ifndef WSFSIXDOFUTILS_HPP
#define WSFSIXDOFUTILS_HPP

#include "wsf_six_dof_export.h"

#include <memory>
#include <string>
#include <vector>

#include "UtVec3dX.hpp"
#include "WsfSixDOF_PID.hpp"

namespace wsf
{
namespace six_dof
{
namespace utils
{
enum CanFlyAtReturnValue
{
   CAN_NOT_FLY,
   CONTROL_FAIL,
   CAN_FLY
};

WSF_SIX_DOF_EXPORT const SinglePidValueData& GetSinglePidData(Pid::Type                         aPidType,
                                                              const AutopilotPidGroupValueData& aPidValues);

std::string OutputDoubleString(double aValue, int aWidth, int aPrecision);

WSF_SIX_DOF_EXPORT int64_t TimeToTime(double aSimTime_sec);
WSF_SIX_DOF_EXPORT double  TimeToTime(int64_t aSimTime_nanosec);

WSF_SIX_DOF_EXPORT double NextGuessFromFalsePosition(double aInputA, double aOutputA, double aInputB, double aOutputB);

WSF_SIX_DOF_EXPORT void UpdateFalsePositionBounds(double& aInputA,
                                                  double& aOutputA,
                                                  double& aInputB,
                                                  double& aOutputB,
                                                  double  aInputC,
                                                  double  aOutputC);

// For reference, these are the "natural" timestep sizes for the mover types
static constexpr int64_t cDT_RIGID_BODY_NANOSEC = 10000000;
static constexpr int64_t cDT_POINT_MASS_NANOSEC = 50000000;
static constexpr double  cDT_RIGID_BODY_SEC     = (0.000000001 * static_cast<double>(cDT_RIGID_BODY_NANOSEC));
static constexpr double  cDT_POINT_MASS_SEC     = (0.000000001 * static_cast<double>(cDT_POINT_MASS_NANOSEC));

// Any delta-T less than or equal to the epsilon value is treated as if it were zero
static constexpr int64_t cEPSILON_SIMTIME_NANOSEC = 100;
static constexpr double  cEPSILON_SIMTIME_SEC     = (0.000000001 * static_cast<double>(cEPSILON_SIMTIME_NANOSEC));

// The gravitational acceleration value used to translate between gs and acceleration
static constexpr double cREFERENCE_GRAV_ACCEL_MPS2 = 9.80665;

WSF_SIX_DOF_EXPORT void CalcUnitVectors(const UtDCM& aDCM, UtVec3dX& aUnitVecX, UtVec3dX& aUnitVecY, UtVec3dX& aUnitVecZ);

WSF_SIX_DOF_EXPORT void
CalcLocalNEDVectors(const UtVec3dX& aPos, UtVec3dX& aLocalNorth, UtVec3dX& aLocalEast, UtVec3dX& aLocalDown);

WSF_SIX_DOF_EXPORT void CalcLocalAngles(const UtVec3dX& aLocalNorth,
                                        const UtVec3dX& aLocalEast,
                                        const UtVec3dX& aLocalDown,
                                        const UtVec3dX& aUnitVecX,
                                        const UtVec3dX& aUnitVecZ,
                                        double&         aLocalYaw_rad,
                                        double&         aLocalPitch_rad,
                                        double&         aLocalRoll_rad);

WSF_SIX_DOF_EXPORT void InertialToBodyRel(const UtDCM& aMatrix, const UtVec3dX& aInertial, UtVec3dX& aBodyRel);

WSF_SIX_DOF_EXPORT void DCM_InertialToBodyRel(const UtDCM& aMatrix, const UtVec3dX& aInertial, UtVec3dX& aBodyRel);

WSF_SIX_DOF_EXPORT void AlphaBetaFromInertialVel(const UtDCM&    aMatrix,
                                                 const UtVec3dX& aInertialVel_mps,
                                                 double&         aSpeed_fps,
                                                 double&         aAlpha_deg,
                                                 double&         aBeta_deg);

WSF_SIX_DOF_EXPORT void
AlphaBetaFromBodyRelVel(const UtVec3dX& aVel_fps, double& aAlpha_deg, double& aBeta_deg, double& aSpeed_fps);

WSF_SIX_DOF_EXPORT void AlphaBetaFromBodyRelVel(const UtVec3dX& aVel_fps, double& aAlpha_deg, double& aBeta_deg);

// This limits the aThrottle argument between 0 and 1
WSF_SIX_DOF_EXPORT void LimitThrottleNormalized(double& aThrottle);

// This limits the aThrottle argument between 0 and 2
WSF_SIX_DOF_EXPORT void LimitThrottleAfterburner(double& aThrottle);
}; // namespace utils
} // namespace six_dof
} // namespace wsf

#endif
