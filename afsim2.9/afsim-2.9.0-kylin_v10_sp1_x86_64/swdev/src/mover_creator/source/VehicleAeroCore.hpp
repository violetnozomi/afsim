// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef VEHICLE_AERO_CORE_HPP
#define VEHICLE_AERO_CORE_HPP

#include <string>

#include "AeroTable2d.hpp"
#include "AeroTable3d.hpp"
#include "AeroTable4d.hpp"

// VehicleAeroCore provides the core (non-movable) aero for the vehicle.

namespace Designer
{
   class VehicleAeroCore
   {
      public:
         VehicleAeroCore() = default;
         ~VehicleAeroCore() = default;

         // This will clear all tabular data
         void ClearAllData();

         // Each of these functions returns a pointer to the table if it exists -- otherwise returns nullptr
         AeroTable4d* GetCL_AlphaBetaMachTablePtr();
         AeroTable4d* GetTrimCL_AlphaBetaMachTablePtr();
         AeroTable3d* GetCLq_AlphaMachTablePtr();
         AeroTable3d* GetCL_AlphaDotAlphaMachTablePtr();
         AeroTable4d* GetCd_AlphaBetaMachTablePtr();
         AeroTable4d* GetTrimCd_AlphaBetaMachTablePtr();
         AeroTable2d* GetCd_MachTablePtr();
         AeroTable4d* GetCY_AlphaBetaMachTablePtr();
         AeroTable4d* GetTrimCY_AlphaBetaMachTablePtr();
         AeroTable3d* GetCYr_BetaMachTablePtr();
         AeroTable3d* GetCY_BetaDotBetaMachTablePtr();
         AeroTable4d* GetCm_AlphaBetaMachTablePtr();
         AeroTable2d* GetCmq_MachTablePtr();
         AeroTable2d* GetCmp_MachTablePtr();
         AeroTable2d* GetCm_AlphaDotMachTablePtr();
         AeroTable4d* GetCn_AlphaBetaMachTablePtr();
         AeroTable2d* GetCn_BetaDotMachTablePtr();
         AeroTable2d* GetCnr_MachTablePtr();
         AeroTable2d* GetCnp_MachTablePtr();
         AeroTable4d* GetCl_AlphaBetaMachTablePtr();
         AeroTable2d* GetClp_MachTablePtr();
         AeroTable2d* GetCl_AlphaDotMachTablePtr();
         AeroTable2d* GetCl_BetaDotMachTablePtr();
         AeroTable2d* GetClr_MachTablePtr();
         AeroTable2d* GetClq_MachTablePtr();
         AeroTable2d* GetRollAccelCapability_MachTable();
         AeroTable2d* GetPitchAccelCapability_MachTable();
         AeroTable2d* GetYawAccelCapability_MachTable();
         AeroTable2d* GetRollStabilityFrequency_MachTable();
         AeroTable2d* GetPitchStabilityFrequency_MachTable();
         AeroTable2d* GetYawStabilityFrequency_MachTable();
         AeroTable2d* GetSpeedbrakeDeltaCD_MachTable();
         AeroTable2d* GetFlapsDeltaCL_MachTable();
         AeroTable2d* GetFlapsDeltaCD_MachTable();
         AeroTable2d* GetSpoilersDeltaCL_MachTable();
         AeroTable2d* GetSpoilersDeltaCD_MachTable();

         // If aCheckIfEmpty is true, the table will only be enabled if the table is non-empty
         void EnableCL_AlphaBetaMachTable()     { mCL_AlphaBetaMachTable.Enable(); }
         void EnableTrimCL_AlphaBetaMachTable() { mTrimCL_AlphaBetaMachTable.Enable(); }
         void EnableCLq_AlphaMachTable()        { mCLq_AlphaMachTable.Enable(); }
         void EnableCL_AlphaDotAlphaMachTable() { mCL_AlphaDotAlphaMachTable.Enable(); }
         void EnableCd_AlphaBetaMachTable()     { mCd_AlphaBetaMachTable.Enable(); }
         void EnableTrimCd_AlphaBetaMachTable() { mTrimCd_AlphaBetaMachTable.Enable(); }
         void EnableCd_MachTable()              { mCd_MachTable.Enable(); }
         void EnableCY_AlphaBetaMachTable()     { mCY_AlphaBetaMachTable.Enable(); }
         void EnableTrimCY_AlphaBetaMachTable() { mTrimCY_AlphaBetaMachTable.Enable(); }
         void EnableCYr_BetaMachTable()         { mCYr_BetaMachTable.Enable(); }
         void EnableCY_BetaDotBetaMachTable()   { mCY_BetaDotBetaMachTable.Enable(); }
         void EnableCm_AlphaBetaMachTable()     { mCm_AlphaBetaMachTable.Enable(); }
         void EnableCmq_MachTable()             { mCmq_MachTable.Enable(); }
         void EnableCmp_MachTable()             { mCmp_MachTable.Enable(); }
         void EnableCm_AlphaDotMachTable()      { mCm_AlphaDotMachTable.Enable(); }
         void EnableCn_AlphaBetaMachTable()     { mCn_AlphaBetaMachTable.Enable(); }
         void EnableCn_BetaDotMachTable()       { mCn_BetaDotMachTable.Enable(); }
         void EnableCnr_MachTable()             { mCnr_MachTable.Enable(); }
         void EnableCnp_MachTable()             { mCnp_MachTable.Enable(); }
         void EnableCl_AlphaBetaMachTable()     { mCl_AlphaBetaMachTable.Enable(); }
         void EnableClp_MachTable()             { mClp_MachTable.Enable(); }
         void EnableCl_AlphaDotMachTable()      { mCl_AlphaDotMachTable.Enable(); }
         void EnableClr_MachTable()             { mClr_MachTable.Enable(); }
         void EnableCl_BetaDotMachTable()       { mCl_BetaDotMachTable.Enable(); }
         void EnableClq_MachTable()             { mClq_MachTable.Enable(); }
         void EnableRollAccelCapability_MachTable()     { mRollAccelCapability_MachTable.Enable(); }
         void EnablePitchAccelCapability_MachTable()    { mPitchAccelCapability_MachTable.Enable(); }
         void EnableYawAccelCapability_MachTable()      { mYawAccelCapability_MachTable.Enable(); }
         void EnableRollStabilityFrequency_MachTable()  { mRollStabilityFrequency_MachTable.Enable(); }
         void EnablePitchStabilityFrequency_MachTable() { mPitchStabilityFrequency_MachTable.Enable(); }
         void EnableYawStabilityFrequency_MachTable()   { mYawStabilityFrequency_MachTable.Enable(); }
         void EnableSpeedbrakeDeltaCD_MachTable()       { mSpeedbrakeDeltaCD_MachTable.Enable(); }
         void EnableFlapsDeltaCL_MachTable()            { mFlapsDeltaCL_MachTable.Enable(); }
         void EnableFlapsDeltaCD_MachTable()            { mFlapsDeltaCD_MachTable.Enable(); }
         void EnableSpoilersDeltaCL_MachTable()         { mSpoilersDeltaCL_MachTable.Enable(); }
         void EnableSpoilersDeltaCD_MachTable()         { mSpoilersDeltaCD_MachTable.Enable(); }

         void DisableCL_AlphaBetaMachTable()     { mCL_AlphaBetaMachTable.Disable(); }
         void DisableTrimCL_AlphaBetaMachTable() { mTrimCL_AlphaBetaMachTable.Disable(); }
         void DisableCLq_AlphaMachTable()        { mCLq_AlphaMachTable.Disable(); }
         void DisableCL_AlphaDotAlphaMachTable() { mCL_AlphaDotAlphaMachTable.Disable(); }
         void DisableCd_AlphaBetaMachTable()     { mCd_AlphaBetaMachTable.Disable(); }
         void DisableTrimCd_AlphaBetaMachTable() { mTrimCd_AlphaBetaMachTable.Disable(); }
         void DisableCd_MachTable()              { mCd_MachTable.Disable(); }
         void DisableCY_AlphaBetaMachTable()     { mCY_AlphaBetaMachTable.Disable(); }
         void DisableTrimCY_AlphaBetaMachTable() { mTrimCY_AlphaBetaMachTable.Disable(); }
         void DisableCYr_BetaMachTable()         { mCYr_BetaMachTable.Disable(); }
         void DisableCY_BetaDotBetaMachTable()   { mCY_BetaDotBetaMachTable.Disable(); }
         void DisableCm_AlphaBetaMachTable()     { mCm_AlphaBetaMachTable.Disable(); }
         void DisableCmq_MachTable()             { mCmq_MachTable.Disable(); }
         void DisableCmp_MachTable()             { mCmp_MachTable.Disable(); }
         void DisableCm_AlphaDotMachTable()      { mCm_AlphaDotMachTable.Disable(); }
         void DisableCn_AlphaBetaMachTable()     { mCn_AlphaBetaMachTable.Disable(); }
         void DisableCn_BetaDotMachTable()       { mCn_BetaDotMachTable.Disable(); }
         void DisableCnr_MachTable()             { mCnr_MachTable.Disable(); }
         void DisableCnp_MachTable()             { mCnp_MachTable.Disable(); }
         void DisableCl_AlphaBetaMachTable()     { mCl_AlphaBetaMachTable.Disable(); }
         void DisableClp_MachTable()             { mClp_MachTable.Disable(); }
         void DisableCl_AlphaDotMachTable()      { mCl_AlphaDotMachTable.Disable(); }
         void DisableClr_MachTable()             { mClr_MachTable.Disable(); }
         void DisableCl_BetaDotMachTable()       { mCl_BetaDotMachTable.Disable(); }
         void DisableClq_MachTable()             { mClq_MachTable.Disable(); }
         void DisableRollAccelCapability_MachTable()     { mRollAccelCapability_MachTable.Disable(); }
         void DisablePitchAccelCapability_MachTable()    { mPitchAccelCapability_MachTable.Disable(); }
         void DisableYawAccelCapability_MachTable()      { mYawAccelCapability_MachTable.Disable(); }
         void DisableRollStabilityFrequency_MachTable()  { mRollStabilityFrequency_MachTable.Disable(); }
         void DisablePitchStabilityFrequency_MachTable() { mPitchStabilityFrequency_MachTable.Disable(); }
         void DisableYawStabilityFrequency_MachTable()   { mYawStabilityFrequency_MachTable.Disable(); }
         void DisableSpeedbrakeDeltaCD_MachTable()       { mSpeedbrakeDeltaCD_MachTable.Disable(); }
         void DisableFlapsDeltaCL_MachTable()            { mFlapsDeltaCL_MachTable.Disable(); }
         void DisableFlapsDeltaCD_MachTable()            { mFlapsDeltaCD_MachTable.Disable(); }
         void DisableSpoilersDeltaCL_MachTable()         { mSpoilersDeltaCL_MachTable.Disable(); }
         void DisableSpoilersDeltaCD_MachTable()         { mSpoilersDeltaCD_MachTable.Disable(); }

         const std::vector<std::string> GetEnabledTables();
         AeroTableBase* GetTableByName(QString aTableName);
      private:

         // Note:  All alpha/beta angles in the tables are in radians and all rates are in radians/second

         // Lift
         AeroTable4d   mCL_AlphaBetaMachTable          = AeroTable4d("CL", "Alpha", "Beta", "Mach");
         AeroTable4d   mTrimCL_AlphaBetaMachTable      = AeroTable4d("CL (trimmed)", "Alpha", "Beta", "Mach");
         AeroTable3d   mCLq_AlphaMachTable             = AeroTable3d("CLq", "Alpha", "Mach");
         AeroTable3d   mCL_AlphaDotAlphaMachTable      = AeroTable3d("CL_alphadot", "Alpha", "Mach");

         // Drag
         AeroTable4d   mCd_AlphaBetaMachTable          = AeroTable4d("Cd", "Alpha", "Beta", "Mach");
         AeroTable4d   mTrimCd_AlphaBetaMachTable      = AeroTable4d("Cd (trimmed)", "Alpha", "Beta", "Mach");
         AeroTable2d   mCd_MachTable                   = AeroTable2d("Cd", "Mach"); // used for Guided Mover aero only

         // Side force
         AeroTable4d   mCY_AlphaBetaMachTable          = AeroTable4d("CY", "Alpha", "Beta", "Mach");
         AeroTable4d   mTrimCY_AlphaBetaMachTable      = AeroTable4d("CY (trimmed)", "Alpha", "Beta", "Mach");
         AeroTable3d   mCYr_BetaMachTable              = AeroTable3d("CYr", "Beta", "Mach");
         AeroTable3d   mCY_BetaDotBetaMachTable        = AeroTable3d("CY_betadot", "Beta", "Mach");

         // Pitching moments -- These get multiplied by wing area and wing chord
         AeroTable4d   mCm_AlphaBetaMachTable          = AeroTable4d("Cm", "Alpha", "Beta", "Mach");
         AeroTable2d   mCmq_MachTable                  = AeroTable2d("Cmq", "Mach");  // This get multiplied by pitch rate (q, in rad/sec)
         AeroTable2d   mCmp_MachTable                  = AeroTable2d("Cmp", "Mach");  // This get multiplied by roll rate (p, in rad/sec)
         AeroTable2d   mCm_AlphaDotMachTable           = AeroTable2d("Cm_alphadot", "Mach");

         // Yawing moments -- These get multiplied by yaw rate (r, in rad/sec), wing area, and wing span
         AeroTable4d   mCn_AlphaBetaMachTable          = AeroTable4d("Cn", "Alpha", "Beta", "Mach");
         AeroTable2d   mCn_BetaDotMachTable            = AeroTable2d("Cn_betadot", "Mach");
         AeroTable2d   mCnr_MachTable                  = AeroTable2d("Cnr", "Mach"); // This get multiplied by yaw rate (r, in rad/sec)
         AeroTable2d   mCnp_MachTable                  = AeroTable2d("Cnp", "Mach"); // This get multiplied by roll rate (p, in rad/sec)

         // Rolling moments -- These get multiplied by roll rate (p, in rad/sec), wing area, and wing span
         AeroTable4d   mCl_AlphaBetaMachTable          = AeroTable4d("Cl", "Alpha", "Beta", "Mach");
         AeroTable2d   mClp_MachTable                  = AeroTable2d("Clp", "Mach"); // This get multiplied by roll rate (p, in rad/sec)
         AeroTable2d   mCl_AlphaDotMachTable           = AeroTable2d("Cl_alphadot", "Mach");
         AeroTable2d   mCl_BetaDotMachTable            = AeroTable2d("Cl_betadot", "Mach");
         AeroTable2d   mClr_MachTable                  = AeroTable2d("Clr", "Mach"); // This get multiplied by yaw rate (r, in rad/sec)
         AeroTable2d   mClq_MachTable                  = AeroTable2d("Clq", "Mach"); // This get multiplied by pitch rate (q, in rad/sec)

         // Rotation control effects -- These capture the limits of rotation for a point-mass mover
         AeroTable2d   mRollAccelCapability_MachTable  = AeroTable2d("maximum_roll_acceleration", "Mach");
         AeroTable2d   mPitchAccelCapability_MachTable = AeroTable2d("maximum_pitch_acceleration", "Mach");
         AeroTable2d   mYawAccelCapability_MachTable   = AeroTable2d("maximum_yaw_acceleration", "Mach");

         // Rotation stability effects -- These capture the tendency of a point-mass mover to return to or diverge from 0 alpha/beta/roll-rate
         AeroTable2d   mRollStabilityFrequency_MachTable  = AeroTable2d("roll_stabilizing_frequency", "Mach");
         AeroTable2d   mPitchStabilityFrequency_MachTable = AeroTable2d("alpha_stabilizing_frequency", "Mach");
         AeroTable2d   mYawStabilityFrequency_MachTable   = AeroTable2d("beta_stabilizing_frequency", "Mach");

         // Simple device effects -- These represent effects of drag and lift devices at full deployment
         AeroTable2d   mSpeedbrakeDeltaCD_MachTable   = AeroTable2d("speedbrake_dcd", "Mach");
         AeroTable2d   mFlapsDeltaCL_MachTable        = AeroTable2d("flaps_dcl", "Mach");
         AeroTable2d   mFlapsDeltaCD_MachTable        = AeroTable2d("flaps_dcd", "Mach");
         AeroTable2d   mSpoilersDeltaCL_MachTable     = AeroTable2d("spoilers_dcl", "Mach");
         AeroTable2d   mSpoilersDeltaCD_MachTable     = AeroTable2d("spoilers_dcd", "Mach");
   };
}
#endif // !VEHICLE_AERO_CORE_HPP
