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

#include "VehicleAeroCore.hpp"

namespace Designer
{

void VehicleAeroCore::ClearAllData()
{
   mCL_AlphaBetaMachTable.Clear();
   mTrimCL_AlphaBetaMachTable.Clear();
   mCLq_AlphaMachTable.Clear();
   mCL_AlphaDotAlphaMachTable.Clear();

   mCd_AlphaBetaMachTable.Clear();
   mTrimCd_AlphaBetaMachTable.Clear();
   mCd_MachTable.Clear();

   mCY_AlphaBetaMachTable.Clear();
   mTrimCY_AlphaBetaMachTable.Clear();
   mCYr_BetaMachTable.Clear();
   mCY_BetaDotBetaMachTable.Clear();

   mCm_AlphaBetaMachTable.Clear();
   mCmq_MachTable.Clear();
   mCmp_MachTable.Clear();
   mCm_AlphaDotMachTable.Clear();

   mCn_AlphaBetaMachTable.Clear();
   mCn_BetaDotMachTable.Clear();
   mCnr_MachTable.Clear();
   mCnp_MachTable.Clear();

   mCl_AlphaBetaMachTable.Clear();
   mClp_MachTable.Clear();
   mCl_AlphaDotMachTable.Clear();
   mCl_BetaDotMachTable.Clear();
   mClr_MachTable.Clear();
   mClq_MachTable.Clear();

   mRollAccelCapability_MachTable.Clear();
   mPitchAccelCapability_MachTable.Clear();
   mYawAccelCapability_MachTable.Clear();
   mRollStabilityFrequency_MachTable.Clear();
   mPitchStabilityFrequency_MachTable.Clear();
   mYawStabilityFrequency_MachTable.Clear();

   mSpeedbrakeDeltaCD_MachTable.Clear();
   mFlapsDeltaCL_MachTable.Clear();
   mFlapsDeltaCD_MachTable.Clear();
   mSpoilersDeltaCL_MachTable.Clear();
   mSpoilersDeltaCD_MachTable.Clear();
}

AeroTable4d* VehicleAeroCore::GetCL_AlphaBetaMachTablePtr()
{
   if (mCL_AlphaBetaMachTable.IsValid())
   {
      return &mCL_AlphaBetaMachTable;
   }

   return nullptr;
}

AeroTable4d* VehicleAeroCore::GetTrimCL_AlphaBetaMachTablePtr()
{
    if (mTrimCL_AlphaBetaMachTable.IsValid())
    {
        return &mTrimCL_AlphaBetaMachTable;
    }

    return nullptr;
}

AeroTable3d* VehicleAeroCore::GetCLq_AlphaMachTablePtr()
{
   if (mCLq_AlphaMachTable.IsValid())
   {
      return &mCLq_AlphaMachTable;
   }

   return nullptr;
}

AeroTable3d* VehicleAeroCore::GetCL_AlphaDotAlphaMachTablePtr()
{
   if (mCL_AlphaDotAlphaMachTable.IsValid())
   {
      return &mCL_AlphaDotAlphaMachTable;
   }

   return nullptr;
}

AeroTable4d* VehicleAeroCore::GetCd_AlphaBetaMachTablePtr()
{
   if (mCd_AlphaBetaMachTable.IsValid())
   {
      return &mCd_AlphaBetaMachTable;
   }

   return nullptr;
}

AeroTable4d* VehicleAeroCore::GetTrimCd_AlphaBetaMachTablePtr()
{
    if (mTrimCd_AlphaBetaMachTable.IsValid())
    {
        return &mTrimCd_AlphaBetaMachTable;
    }

    return nullptr;
}

AeroTable2d* VehicleAeroCore::GetCd_MachTablePtr()
{
   if (mCd_MachTable.IsValid())
   {
      return &mCd_MachTable;
   }

   return nullptr;
}

AeroTable4d* VehicleAeroCore::GetCY_AlphaBetaMachTablePtr()
{
   if (mCY_AlphaBetaMachTable.IsValid())
   {
      return &mCY_AlphaBetaMachTable;
   }

   return nullptr;
}

AeroTable4d* VehicleAeroCore::GetTrimCY_AlphaBetaMachTablePtr()
{
    if (mTrimCY_AlphaBetaMachTable.IsValid())
    {
        return &mTrimCY_AlphaBetaMachTable;
    }

    return nullptr;
}

AeroTable3d* VehicleAeroCore::GetCYr_BetaMachTablePtr()
{
   if (mCYr_BetaMachTable.IsValid())
   {
      return &mCYr_BetaMachTable;
   }

   return nullptr;
}

AeroTable3d* VehicleAeroCore::GetCY_BetaDotBetaMachTablePtr()
{
   if (mCY_BetaDotBetaMachTable.IsValid())
   {
      return &mCY_BetaDotBetaMachTable;
   }

   return nullptr;
}

AeroTable4d* VehicleAeroCore::GetCm_AlphaBetaMachTablePtr()
{
   if (mCm_AlphaBetaMachTable.IsValid())
   {
      return &mCm_AlphaBetaMachTable;
   }

   return nullptr;
}

AeroTable2d* VehicleAeroCore::GetCmq_MachTablePtr()
{
   if (mCmq_MachTable.IsValid())
   {
      return &mCmq_MachTable;
   }

   return nullptr;
}

AeroTable2d* VehicleAeroCore::GetCmp_MachTablePtr()
{
   if (mCmp_MachTable.IsValid())
   {
      return &mCmp_MachTable;
   }

   return nullptr;
}

AeroTable2d* VehicleAeroCore::GetCm_AlphaDotMachTablePtr()
{
   if (mCm_AlphaDotMachTable.IsValid())
   {
      return &mCm_AlphaDotMachTable;
   }

   return nullptr;
}

AeroTable4d* VehicleAeroCore::GetCn_AlphaBetaMachTablePtr()
{
   if (mCn_AlphaBetaMachTable.IsValid())
   {
      return &mCn_AlphaBetaMachTable;
   }

   return nullptr;
}

AeroTable2d* VehicleAeroCore::GetCn_BetaDotMachTablePtr()
{
   if (mCn_BetaDotMachTable.IsValid())
   {
      return &mCn_BetaDotMachTable;
   }

   return nullptr;
}

AeroTable2d* VehicleAeroCore::GetCnr_MachTablePtr()
{
   if (mCnr_MachTable.IsValid())
   {
      return &mCnr_MachTable;
   }

   return nullptr;
}

AeroTable2d* VehicleAeroCore::GetCnp_MachTablePtr()
{
   if (mCnp_MachTable.IsValid())
   {
      return &mCnp_MachTable;
   }

   return nullptr;
}

AeroTable4d* VehicleAeroCore::GetCl_AlphaBetaMachTablePtr()
{
   if (mCl_AlphaBetaMachTable.IsValid())
   {
      return& mCl_AlphaBetaMachTable;
   }

   return nullptr;
}

AeroTable2d* VehicleAeroCore::GetClp_MachTablePtr()
{
   if (mClp_MachTable.IsValid())
   {
      return &mClp_MachTable;
   }

   return nullptr;
}

AeroTable2d* VehicleAeroCore::GetCl_AlphaDotMachTablePtr()
{
   if (mCl_AlphaDotMachTable.IsValid())
   {
      return &mCl_AlphaDotMachTable;
   }

   return nullptr;
}

AeroTable2d* VehicleAeroCore::GetCl_BetaDotMachTablePtr()
{
   if (mCl_BetaDotMachTable.IsValid())
   {
      return &mCl_BetaDotMachTable;
   }

   return nullptr;
}

AeroTable2d* VehicleAeroCore::GetClr_MachTablePtr()
{
   if (mClr_MachTable.IsValid())
   {
      return &mClr_MachTable;
   }

   return nullptr;
}

AeroTable2d* VehicleAeroCore::GetClq_MachTablePtr()
{
   if (mClq_MachTable.IsValid())
   {
      return &mClq_MachTable;
   }

   return nullptr;
}

AeroTable2d* VehicleAeroCore::GetRollAccelCapability_MachTable()
{
   if (mRollAccelCapability_MachTable.IsValid())
   {
      return &mRollAccelCapability_MachTable;
   }

   return nullptr;
}

AeroTable2d* VehicleAeroCore::GetPitchAccelCapability_MachTable()
{
   if (mPitchAccelCapability_MachTable.IsValid())
   {
      return &mPitchAccelCapability_MachTable;
   }

   return nullptr;
}

AeroTable2d* VehicleAeroCore::GetYawAccelCapability_MachTable()
{
   if (mYawAccelCapability_MachTable.IsValid())
   {
      return &mYawAccelCapability_MachTable;
   }

   return nullptr;
}

AeroTable2d* VehicleAeroCore::GetRollStabilityFrequency_MachTable()
{
   if (mRollStabilityFrequency_MachTable.IsValid())
   {
      return &mRollStabilityFrequency_MachTable;
   }

   return nullptr;
}

AeroTable2d* VehicleAeroCore::GetPitchStabilityFrequency_MachTable()
{
   if (mPitchStabilityFrequency_MachTable.IsValid())
   {
      return &mPitchStabilityFrequency_MachTable;
   }

   return nullptr;
}

AeroTable2d* VehicleAeroCore::GetYawStabilityFrequency_MachTable()
{
   if (mYawStabilityFrequency_MachTable.IsValid())
   {
      return &mYawStabilityFrequency_MachTable;
   }

   return nullptr;
}

AeroTable2d* VehicleAeroCore::GetSpeedbrakeDeltaCD_MachTable()
{
   if (mSpeedbrakeDeltaCD_MachTable.IsValid())
   {
      return &mSpeedbrakeDeltaCD_MachTable;
   }

   return nullptr;
}

AeroTable2d* VehicleAeroCore::GetFlapsDeltaCL_MachTable()
{
   if (mFlapsDeltaCL_MachTable.IsValid())
   {
      return &mFlapsDeltaCL_MachTable;
   }

   return nullptr;
}

AeroTable2d* VehicleAeroCore::GetFlapsDeltaCD_MachTable()
{
   if (mFlapsDeltaCD_MachTable.IsValid())
   {
      return &mFlapsDeltaCD_MachTable;
   }

   return nullptr;
}

AeroTable2d* VehicleAeroCore::GetSpoilersDeltaCL_MachTable()
{
   if (mSpoilersDeltaCL_MachTable.IsValid())
   {
      return &mSpoilersDeltaCL_MachTable;
   }

   return nullptr;
}

AeroTable2d* VehicleAeroCore::GetSpoilersDeltaCD_MachTable()
{
   if (mSpoilersDeltaCD_MachTable.IsValid())
   {
      return &mSpoilersDeltaCD_MachTable;
   }

   return nullptr;
}

const std::vector<std::string> VehicleAeroCore::GetEnabledTables()
{
   std::vector<std::string> enabledTableList;
   if (mCL_AlphaBetaMachTable.IsValid())
   {
      enabledTableList.emplace_back(mCL_AlphaBetaMachTable.GetTableName());
   }
   if (mTrimCL_AlphaBetaMachTable.IsValid())
   {
      enabledTableList.emplace_back(mTrimCL_AlphaBetaMachTable.GetTableName());
   }
   if (mCLq_AlphaMachTable.IsValid())
   {
      enabledTableList.emplace_back(mCLq_AlphaMachTable.GetTableName());
   }
   if (mCL_AlphaDotAlphaMachTable.IsValid())
   {
      enabledTableList.emplace_back(mCL_AlphaDotAlphaMachTable.GetTableName());
   }
   if (mCd_AlphaBetaMachTable.IsValid())
   {
      enabledTableList.emplace_back(mCd_AlphaBetaMachTable.GetTableName());
   }
   if (mTrimCd_AlphaBetaMachTable.IsValid())
   {
       enabledTableList.emplace_back(mTrimCd_AlphaBetaMachTable.GetTableName());
   }
   if (mCd_MachTable.IsValid())
   {
      enabledTableList.emplace_back(mCd_MachTable.GetTableName());
   }
   if (mCY_AlphaBetaMachTable.IsValid())
   {
      enabledTableList.emplace_back(mCY_AlphaBetaMachTable.GetTableName());
   }
   if (mTrimCY_AlphaBetaMachTable.IsValid())
   {
       enabledTableList.emplace_back(mTrimCY_AlphaBetaMachTable.GetTableName());
   }
   if (mCYr_BetaMachTable.IsValid())
   {
      enabledTableList.emplace_back(mCYr_BetaMachTable.GetTableName());
   }
   if (mCY_BetaDotBetaMachTable.IsValid())
   {
      enabledTableList.emplace_back(mCY_BetaDotBetaMachTable.GetTableName());
   }
   if (mCm_AlphaBetaMachTable.IsValid())
   {
      enabledTableList.emplace_back(mCm_AlphaBetaMachTable.GetTableName());
   }
   if (mCmq_MachTable.IsValid())
   {
      enabledTableList.emplace_back(mCmq_MachTable.GetTableName());
   }
   if (mCmp_MachTable.IsValid())
   {
      enabledTableList.emplace_back(mCmp_MachTable.GetTableName());
   }
   if (mCm_AlphaDotMachTable.IsValid())
   {
      enabledTableList.emplace_back(mCm_AlphaDotMachTable.GetTableName());
   }
   if (mCn_AlphaBetaMachTable.IsValid())
   {
      enabledTableList.emplace_back(mCn_AlphaBetaMachTable.GetTableName());
   }
   if (mCn_BetaDotMachTable.IsValid())
   {
      enabledTableList.emplace_back(mCn_BetaDotMachTable.GetTableName());
   }
   if (mCnr_MachTable.IsValid())
   {
      enabledTableList.emplace_back(mCnr_MachTable.GetTableName());
   }
   if (mCnp_MachTable.IsValid())
   {
      enabledTableList.emplace_back(mCnp_MachTable.GetTableName());
   }
   if (mCl_AlphaBetaMachTable.IsValid())
   {
      enabledTableList.emplace_back(mCl_AlphaBetaMachTable.GetTableName());
   }
   if (mClp_MachTable.IsValid())
   {
      enabledTableList.emplace_back(mClp_MachTable.GetTableName());
   }
   if (mCl_AlphaDotMachTable.IsValid())
   {
      enabledTableList.emplace_back(mCl_AlphaDotMachTable.GetTableName());
   }
   if (mClr_MachTable.IsValid())
   {
      enabledTableList.emplace_back(mClr_MachTable.GetTableName());
   }
   if (mCl_BetaDotMachTable.IsValid())
   {
      enabledTableList.emplace_back(mCl_BetaDotMachTable.GetTableName());
   }
   if (mClq_MachTable.IsValid())
   {
      enabledTableList.emplace_back(mClq_MachTable.GetTableName());
   }
   if (mRollAccelCapability_MachTable.IsValid())
   {
      enabledTableList.emplace_back(mRollAccelCapability_MachTable.GetTableName());
   }
   if (mPitchAccelCapability_MachTable.IsValid())
   {
      enabledTableList.emplace_back(mPitchAccelCapability_MachTable.GetTableName());
   }
   if (mYawAccelCapability_MachTable.IsValid())
   {
      enabledTableList.emplace_back(mYawAccelCapability_MachTable.GetTableName());
   }
   if (mRollStabilityFrequency_MachTable.IsValid())
   {
      enabledTableList.emplace_back(mRollStabilityFrequency_MachTable.GetTableName());
   }
   if (mPitchStabilityFrequency_MachTable.IsValid())
   {
      enabledTableList.emplace_back(mPitchStabilityFrequency_MachTable.GetTableName());
   }
   if (mYawStabilityFrequency_MachTable.IsValid())
   {
      enabledTableList.emplace_back(mYawStabilityFrequency_MachTable.GetTableName());
   }
   if (mSpeedbrakeDeltaCD_MachTable.IsValid())
   {
      enabledTableList.emplace_back(mSpeedbrakeDeltaCD_MachTable.GetTableName());
   }
   if (mFlapsDeltaCL_MachTable.IsValid())
   {
      enabledTableList.emplace_back(mFlapsDeltaCL_MachTable.GetTableName());
   }
   if (mFlapsDeltaCD_MachTable.IsValid())
   {
      enabledTableList.emplace_back(mFlapsDeltaCD_MachTable.GetTableName());
   }
   if (mSpoilersDeltaCL_MachTable.IsValid())
   {
      enabledTableList.emplace_back(mSpoilersDeltaCL_MachTable.GetTableName());
   }
   if (mSpoilersDeltaCD_MachTable.IsValid())
   {
      enabledTableList.emplace_back(mSpoilersDeltaCD_MachTable.GetTableName());
   }
   return enabledTableList;
}

AeroTableBase* VehicleAeroCore::GetTableByName(QString aTableName)
{
   if (aTableName == "CL vs Alpha Beta Mach")
   {
      return GetCL_AlphaBetaMachTablePtr();
   }
   else if (aTableName == "CL (trimmed) vs Alpha Beta Mach")
   {
      return GetTrimCL_AlphaBetaMachTablePtr();
   }
   else if (aTableName == "Cd vs Alpha Beta Mach")
   {
      return GetCd_AlphaBetaMachTablePtr();
   }
   else if (aTableName == "Cd (trimmed) vs Alpha Beta Mach")
   {
       return GetTrimCd_AlphaBetaMachTablePtr();
   }
   else if (aTableName == "Cd vs Mach")
   {
      return GetCd_MachTablePtr();
   }
   else if (aTableName == "CLq vs Alpha Mach")
   {
      return GetCLq_AlphaMachTablePtr();
   }
   else if (aTableName == "CL_alphadot vs Alpha Mach")
   {
      return GetCL_AlphaDotAlphaMachTablePtr();
   }
   else if (aTableName == "CY vs Alpha Beta Mach")
   {
      return GetCY_AlphaBetaMachTablePtr();
   }
   else if (aTableName == "CY (trimmed) vs Alpha Beta Mach")
   {
       return GetTrimCY_AlphaBetaMachTablePtr();
   }
   else if (aTableName == "CYr vs Beta Mach")
   {
      return GetCYr_BetaMachTablePtr();
   }
   else if (aTableName == "CY_betadot vs Beta Mach")
   {
      return GetCY_BetaDotBetaMachTablePtr();
   }
   else if (aTableName == "Cm vs Alpha Beta Mach")
   {
      return GetCm_AlphaBetaMachTablePtr();
   }
   else if (aTableName == "Cmq vs Mach")
   {
      return GetCmq_MachTablePtr();
   }
   else if (aTableName == "Cmp vs Mach")
   {
      return GetCmp_MachTablePtr();
   }
   else if (aTableName == "Cm_alphadot vs Mach")
   {
      return GetCm_AlphaDotMachTablePtr();
   }
   else if (aTableName == "Cn vs Alpha Beta Mach")
   {
      return GetCn_AlphaBetaMachTablePtr();
   }
   else if (aTableName == "Cn_betadot vs Mach")
   {
      return GetCn_BetaDotMachTablePtr();
   }
   else if (aTableName == "Cnr vs Mach")
   {
      return GetCnr_MachTablePtr();
   }
   else if (aTableName == "Cnp vs Mach")
   {
      return GetCnp_MachTablePtr();
   }
   else if (aTableName == "Cl vs Alpha Beta Mach")
   {
      return  GetCl_AlphaBetaMachTablePtr();
   }
   else if (aTableName == "Clp vs Mach")
   {
      return GetClp_MachTablePtr();
   }
   else if (aTableName == "Cl_alphadot vs Mach")
   {
      return GetCl_AlphaDotMachTablePtr();
   }
   else if (aTableName == "Clr vs Mach")
   {
      return GetClr_MachTablePtr();
   }
   else if (aTableName == "Cl_betadot vs Mach")
   {
      return GetCl_BetaDotMachTablePtr();
   }
   else if (aTableName == "Clq vs Mach")
   {
      return GetClq_MachTablePtr();
   }
   else if (aTableName == "maximum_roll_acceleration vs Mach")
   {
      return GetRollAccelCapability_MachTable();
   }
   else if (aTableName == "maximum_pitch_acceleration vs Mach")
   {
      return GetPitchAccelCapability_MachTable();
   }
   else if (aTableName == "maximum_yaw_acceleration vs Mach")
   {
      return GetYawAccelCapability_MachTable();
   }
   else if (aTableName == "roll_stabilizing_frequency vs Mach")
   {
      return GetRollStabilityFrequency_MachTable();
   }
   else if (aTableName == "alpha_stabilizing_frequency vs Mach")
   {
      return GetPitchStabilityFrequency_MachTable();
   }
   else if (aTableName == "beta_stabilizing_frequency vs Mach")
   {
      return GetYawStabilityFrequency_MachTable();
   }
   else if (aTableName == "speedbrake_dcd vs Mach")
   {
      return GetSpeedbrakeDeltaCD_MachTable();
   }
   else if (aTableName == "flaps_dcl vs Mach")
   {
      return GetFlapsDeltaCL_MachTable();
   }
   else if (aTableName == "flaps_dcd vs Mach")
   {
      return GetFlapsDeltaCD_MachTable();
   }
   else if (aTableName == "spoilers_dcl vs Mach")
   {
      return GetSpoilersDeltaCL_MachTable();
   }
   else if (aTableName == "spoilers_dcd vs Mach")
   {
      return GetSpoilersDeltaCD_MachTable();
   }

   return nullptr;
}

} // namespace Designer
