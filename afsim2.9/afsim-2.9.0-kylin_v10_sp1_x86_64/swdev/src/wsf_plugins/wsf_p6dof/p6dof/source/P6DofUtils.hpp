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

#ifndef P6DOF_UTILS_H
#define P6DOF_UTILS_H

#include "p6dof_export.h"

#include <memory>
#include <string>
#include <vector>

#include "P6DofVehicleData.hpp"
#include "UtVec3dX.hpp"

class P6DofVehicle;
class P6DofTableTool;

namespace P6DofUtils
{
enum CanFlyAtReturnValue
{
   CAN_NOT_FLY,
   CONTROL_FAIL,
   CAN_FLY
};

P6DOF_EXPORT void Autotune(P6DofVehicle* aVehicle);
P6DOF_EXPORT void CreateAutopilotConfigFile(P6DofVehicle* aVehicle, const std::string& aFilename);
P6DOF_EXPORT std::vector<P6DOF::Pid::Type> GetRelevantPids(P6DofVehicle* aVehicle);
P6DOF_EXPORT const P6DOF::SinglePidValueData& GetSinglePidData(P6DOF::Pid::Type                         aPidType,
                                                               const P6DOF::AutopilotPidGroupValueData& aPidValues);

// The throttle input assumes that 0 is idle, 1 is full power without
// augmentation/afterburner (military power), and 2 is full power with
// full augmentation/afterburner (full afterburner).
P6DOF_EXPORT CanFlyAtReturnValue
CanFlyAt(P6DofVehicle* aObject, double aAltitude_ft, double aVelocity_fps, double aThrottleForward, double aFlapsDown);

// The filename is typically "autopilot_support_tables.txt"
P6DOF_EXPORT void CreateAutopilotSupportFile(P6DofVehicle* aObject, const std::string& aFilename);

// The filename is typically "autopilot_support_tables.txt"
P6DOF_EXPORT void CreateAutopilotSupportFileWithMaxMach(P6DofVehicle*      aObject,
                                                        const std::string& aFilename,
                                                        const double&      aMaxMach);

// This provides the processing that is common between CreateAutopilotSupportFile()
// and CreateAutopilotSupportFileWithMaxMach()
P6DOF_EXPORT void CommonCreateAutopilotSupportFile(P6DofTableTool& aTool, const std::string& aFilename);

P6DOF_EXPORT int64_t TimeToTime(double aSimTime_sec);
P6DOF_EXPORT double  TimeToTime(int64_t aSimTime_nanosec);

// Any delta-T less than or equal to the epsilon value is treated as if it were zero
static constexpr int64_t cEPSILON_SIMTIME_NANOSEC = 100;
static constexpr double  cEPSILON_SIMTIME_SEC     = (0.000000001 * static_cast<double>(cEPSILON_SIMTIME_NANOSEC));

P6DOF_EXPORT void CalcSphericalLatLonAlt(const UtVec3dX& aPos_m, double& aLat_deg, double& aLon_deg, double& aAlt_m);

P6DOF_EXPORT void CalcSphericalVecToNED(const UtVec3dX& aPos, const UtVec3dX& aSphericalVec, UtVec3dX& aNEDVec);

P6DOF_EXPORT void CalcUnitVectors(const UtDCM& aDCM, UtVec3dX& aUnitVecX, UtVec3dX& aUnitVecY, UtVec3dX& aUnitVecZ);

P6DOF_EXPORT void CalcLocalNEDVectors(const UtVec3dX& aPos, UtVec3dX& aLocalNorth, UtVec3dX& aLocalEast, UtVec3dX& aLocalDown);

P6DOF_EXPORT void CalcLocalAngles(const UtVec3dX& aLocalNorth,
                                  const UtVec3dX& aLocalEast,
                                  const UtVec3dX& aLocalDown,
                                  const UtVec3dX& aUnitVecX,
                                  const UtVec3dX& aUnitVecZ,
                                  double&         aLocalYaw_rad,
                                  double&         aLocalPitch_rad,
                                  double&         aLocalRoll_rad);

P6DOF_EXPORT void CalcSphericalWCSAngles(const UtDCM& aDCM, double& aYaw_rad, double& aPitch_rad, double& aRoll_rad);

P6DOF_EXPORT void SphericalGreatCircleHeadingAndDistance(const double aLat,
                                                         const double aLon,
                                                         const double aTargetLat,
                                                         const double aTargetLon,
                                                         double&      aTargetHeading_r,
                                                         double&      aTargetDistance_m);

P6DOF_EXPORT void
CalcSphericalPositionFromLatLonAlt(const double aLat_deg, const double aLon_deg, const double aAlt_m, UtVec3dX& aPos_m);

P6DOF_EXPORT void InertialToBodyRel(const UtDCM& aMatrix, const UtVec3dX& aInertial, UtVec3dX& aBodyRel);

// This is used for the earth radius when mUseSphericalEarth is true
// Note: The AFSIM cEARTH_RADIUS = 6366707.0194937074958 (6366.7 km)
// Typical earth radius is 6371 km -- note the 4.3 km discrepancy
// Distances from points on the surface to the center range from
// 6,353 km to 6,384 km. Several different ways of modeling the
// Earth as a sphere each yield a mean radius of 6,371 km.

P6DOF_EXPORT double GetSphericalEarthRadius_m();

P6DOF_EXPORT void DCM_InertialToBodyRel(const UtDCM& aMatrix, const UtVec3dX& aInertial, UtVec3dX& aBodyRel);

P6DOF_EXPORT void SetSphericalDCM(const double aLat, const double aLon, UtDCM& aDCM);

P6DOF_EXPORT void SetSphericalDCM(const double aLat,
                                  const double aLon,
                                  const double aYaw_rad,
                                  const double aPitch_rad,
                                  const double aRoll_rad,
                                  UtDCM&       aDCM);

P6DOF_EXPORT void SetSphericalDCM(const UtVec3dX& aPos_m,
                                  const double    aHdg_rad,
                                  const double    aPitch_rad,
                                  const double    aRoll_rad,
                                  UtDCM&          aDCM);

P6DOF_EXPORT void
GetSphericalNEDAngles(const UtDCM& aDCM, const UtVec3dX& aPos, double& aHdg_rad, double& aPitch_rad, double& aRoll_rad);

P6DOF_EXPORT UtVec3dX CalcWGS84VecToNED(const UtVec3dX& aWGS84Pos_m, const UtVec3dX& aVecWGS84_m);

P6DOF_EXPORT void CalcSphericalNEDVelToSphericalVel(const UtVec3dX& aPos, const UtVec3dX& aNEDVec, UtVec3dX& aSphericalVec);

P6DOF_EXPORT double CalcSphericalRangeToPoint_m(const UtVec3dX& aPos, const UtVec3dX& aPoint);

P6DOF_EXPORT double CalcSphericalHeadingToPoint_r(const UtVec3dX& aPos, const UtVec3dX& aPoint);

P6DOF_EXPORT void AlphaBetaFromInertialVel(const UtDCM&    aMatrix,
                                           const UtVec3dX& aInertialVel_mps,
                                           double&         aSpeed_fps,
                                           double&         aAlpha_deg,
                                           double&         aBeta_deg);

P6DOF_EXPORT void AlphaBetaFromBodyRelVel(const UtVec3dX& aVel_fps, double& aAlpha_deg, double& aBeta_deg, double& aSpeed_fps);

P6DOF_EXPORT void AlphaBetaFromBodyRelVel(const UtVec3dX& aVel_fps, double& aAlpha_deg, double& aBeta_deg);

P6DOF_EXPORT void GetPositionFromLLA(const double aLat,
                                     const double aLon,
                                     const double aAlt_m,
                                     const bool   aUseSpericalCoords,
                                     UtVec3dX&    aPosition_m);

P6DOF_EXPORT void GetVelocityFromVelNED(const double aNorthVel_mps,
                                        const double aEastVel_mps,
                                        const double aDownVel_mps,
                                        const bool   aUseSpericalCoords,
                                        UtVec3dX&    aPosition_m,
                                        UtVec3dX&    aVelocity_mps);

P6DOF_EXPORT void
GetLLAFromPosition(const UtVec3dX& aPosition_m, const bool aUseSpericalCoords, double& aLat, double& aLon, double& aAlt_m);

// This limits the aThrottle argument between 0 and 1
P6DOF_EXPORT void LimitThrottleNormalized(double& aThrottle);

// This limits the aThrottle argument between 0 and 2
P6DOF_EXPORT void LimitThrottleAfterburner(double& aThrottle);
}; // namespace P6DofUtils

class P6DOF_EXPORT P6DofMachParamDataTable
{
public:
   P6DofMachParamDataTable();
   ~P6DofMachParamDataTable();

   P6DofMachParamDataTable& operator=(const P6DofMachParamDataTable& other) = delete;

   P6DofMachParamDataTable* Clone() const;

   void CreateNewMach(double aMach);
   void AddNewParamDataPair(double aParameter, double aData);

   double GetValueAtMachParam(double aMach, double aParameter) const;

   double GetMinParameterAtMach(double aMach) const;
   double GetMaxParameterAtMach(double aMach) const;

   double GetMinDataAtMach(double aMach) const;
   double GetMaxDataAtMach(double aMach) const;

   // aParameterType typically is "alpha" or "cl"
   // aParameterUnitsString typically is "units deg" or "precision float"
   void OutputToString(std::string&       aInput,
                       const std::string& aTableName,
                       const std::string& aParameterType,
                       const std::string& aParameterUnitsString) const;

protected:
   P6DofMachParamDataTable(const P6DofMachParamDataTable& aSrc);

   struct paramDataPair
   {
      double parameter; // This can be anything (for example, alpha_deg or CL)
      double value;
   };

   struct machParamDataListPair
   {
      double                      mach;
      std::vector<paramDataPair*> paramDataList;
   };

   double GetParameterAtMach(double aMach, bool aGetMin) const;
   double GetMinParameter(const machParamDataListPair* aMachParamDataListPair) const;
   double GetMaxParameter(const machParamDataListPair* aMachParamDataListPair) const;

   double GetDataAtMach(double aMach, bool aGetMin) const;
   double GetMinData(const machParamDataListPair* aMachParamDataListPair) const;
   double GetMaxData(const machParamDataListPair* aMachParamDataListPair) const;

   double GetValueAtParam(std::vector<paramDataPair*>& aList, double aParameter) const;

   std::vector<machParamDataListPair*> mMachParamDataList;

   machParamDataListPair* mCurrentMachParamDataPairPtr;
};

// -------------------------------------------------------------------------------

class P6DOF_EXPORT P6DofMachDataTable
{
public:
   P6DofMachDataTable();
   ~P6DofMachDataTable();

   P6DofMachDataTable& operator=(const P6DofMachDataTable& other) = delete;

   P6DofMachDataTable* Clone() const;

   void CreateNewMachValuePair(double aMach, double aValue);

   double GetValueAtMach(double aMach) const;

   void OutputToString(std::string& aInput, const std::string& aTableName) const;

protected:
   P6DofMachDataTable(const P6DofMachDataTable& aSrc);

   struct machDataPair
   {
      double mach;
      double value;
   };

   std::vector<machDataPair*> mMachDataList;
};

// -------------------------------------------------------------------------------

class P6DofVehicle;

class P6DOF_EXPORT P6DofTableTool
{
public:
   explicit P6DofTableTool(P6DofVehicle* aObject);
   ~P6DofTableTool() = default;

   void InitializeTableMachValues(double aMaxMach);

   // Returns true if everything worked properly
   bool CreateAutopilotTables();

   // These functions output tabular data to a string, useful
   // for outputting data to a file
   void OutputCLMaxMachTableToInputString(std::string& aInput) const;
   void OutputCLMinMachTableToInputString(std::string& aInput) const;
   void OutputAlphaMaxMachTableToInputString(std::string& aInput) const;
   void OutputAlphaMinMachTableToInputString(std::string& aInput) const;
   void OutputAlphaMachCLTableToInputString(std::string& aInput) const;
   void OutputStickMachAlphaTablesToInputString(std::string& aInput) const;
   void OutputCLMachAlphaTableToInputString(std::string& aInput) const;

protected:
   double CalcCLFromCLArea(double aCLArea);

   void CreateMachTestList(double aMaxMach);

   bool CalculateStickForZeroMoment(double  aMach,
                                    double  aAlpha_rad,
                                    bool    aIsPowerOn,
                                    double& aThrustProduced_lbs,
                                    double& aStickBack);

   bool CalculateStickForZeroMoment(double aMach, double aAlpha_rad, double& aStickBack);

   void AddStickAndCL(double aMach, double aAlpha_rad, double& aStickBack, bool& aMachAdded, bool aLowPoint, bool aHighPoint);

   bool CalcStickAndCL(double  aMach,
                       double  aAlpha_rad,
                       double& aStickBackBaseline,
                       double& aStickBackThrustShift,
                       double& aStickBackXCmShift,
                       double& aStickBackZCmShift);

   // Here, we pre-compute the stick and CL values. This is redundant, but gives us an
   // avenue to check data points before we add an uncontrollable mach to our table.
   int CountValidStickAndCL(double aMach, std::vector<double> aAlphaVector_deg);

   void AddAlphaVsMachCL(double aMach, double aCL, double aMinAlpha_deg, double aMaxAlpha_deg, bool& aMachAdded);

   P6DofVehicle*                            mObject;
   std::unique_ptr<P6DofMachDataTable>      mCLMaxVsMachTable;
   std::unique_ptr<P6DofMachDataTable>      mCLMinVsMachTable;
   std::unique_ptr<P6DofMachDataTable>      mAlphaMax_degVsMachTable;
   std::unique_ptr<P6DofMachDataTable>      mAlphaMin_degVsMachTable;
   std::unique_ptr<P6DofMachParamDataTable> mStickForZeroMomentVsMachAlphaTablePtr;            // Mach-Alpha
   std::unique_ptr<P6DofMachParamDataTable> mThrustDeltaStickForZeroMomentVsMachAlphaTablePtr; // Mach-Alpha
   std::unique_ptr<P6DofMachParamDataTable> mXcgDeltaStickForZeroMomentVsMachAlphaTablePtr;    // Mach-Alpha
   std::unique_ptr<P6DofMachParamDataTable> mZcgDeltaStickForZeroMomentVsMachAlphaTablePtr;    // Mach-Alpha
   std::unique_ptr<P6DofMachParamDataTable> mCLVsMachAlphaTable;
   std::unique_ptr<P6DofMachParamDataTable> mAlphaVsMachCLTable;

   // These lists are used to store effective lists of mach, alpha,
   // and beta values for generating tabular data.
   std::vector<double> mMachTestList;
   bool                mTableListsInitialized;

private:
   void Initialize();
   bool CreateCLMaxAndAlphaMaxTables();
   bool CreateCLMinAndAlphaMinTables();
   bool CreateStickForZeroMomentTable();
   bool CreateAlphaVsMachCLTable();
};

// -------------------------------------------------------------------------------

#endif
