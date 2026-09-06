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

#include "VehicleAeroMovable.hpp"

#include "AeroObject.hpp"

namespace Designer
{

VehicleAeroMovable::VehicleAeroMovable(const std::string& aName, AeroObject* aAeroObject)
   : mName(aName)
   , mAeroObject(aAeroObject)
{
}

void VehicleAeroMovable::ClearAllData()
{
   // Clear data
   mCL_AngleAlphaMachTable.Clear();
   mCd_AngleAlphaMachTable.Clear();
   mCd_AngleBetaMachTable.Clear();
   mCd_AngleMachTable.Clear();
   mCY_AngleBetaMachTable.Clear();
   mCm_AngleAlphaMachTable.Clear();
   mCmq_AngleMachTable.Clear();
   mCn_AngleBetaMachTable.Clear();
   mCnr_AngleMachTable.Clear();
   mCl_AngleAlphaBetaTable.Clear();
   mClp_AngleMachTable.Clear();
   mClq_AngleMachTable.Clear();
   mClr_AngleMachTable.Clear();
}

AeroTable4d* VehicleAeroMovable::GetCL_AngleAlphaMachTablePtr()
{
   if (mCL_AngleAlphaMachTable.IsValid())
   {
      return &mCL_AngleAlphaMachTable;
   }

   return nullptr;
}

AeroTable4d* VehicleAeroMovable::GetCd_AngleAlphaMachTablePtr()
{
   if (mCd_AngleAlphaMachTable.IsValid())
   {
      return &mCd_AngleAlphaMachTable;
   }

   return nullptr;
}

AeroTable4d* VehicleAeroMovable::GetCd_AngleBetaMachTablePtr()
{
   if (mCd_AngleBetaMachTable.IsValid())
   {
      return &mCd_AngleBetaMachTable;
   }

   return nullptr;
}

AeroTable3d* VehicleAeroMovable::GetCd_AngleMachTablePtr()
{
   if (mCd_AngleMachTable.IsValid())
   {
      return &mCd_AngleMachTable;
   }

   return nullptr;
}

AeroTable4d* VehicleAeroMovable::GetCY_AngleBetaMachTablePtr()
{
   if (mCY_AngleBetaMachTable.IsValid())
   {
      return &mCY_AngleBetaMachTable;
   }

   return nullptr;
}

AeroTable4d* VehicleAeroMovable::GetCm_AngleAlphaMachTablePtr()
{
   if (mCm_AngleAlphaMachTable.IsValid())
   {
      return &mCm_AngleAlphaMachTable;
   }

   return nullptr;
}

AeroTable3d* VehicleAeroMovable::GetCmq_AngleMachTablePtr()
{
   if (mCmq_AngleMachTable.IsValid())
   {
      return &mCmq_AngleMachTable;
   }

   return nullptr;
}

AeroTable4d* VehicleAeroMovable::GetCn_AngleBetaMachTablePtr()
{
   if (mCn_AngleBetaMachTable.IsValid())
   {
      return &mCn_AngleBetaMachTable;
   }

   return nullptr;
}

AeroTable3d* VehicleAeroMovable::GetCnr_AngleMachTablePtr()
{
   if (mCnr_AngleMachTable.IsValid())
   {
      return &mCnr_AngleMachTable;
   }

   return nullptr;
}

AeroTable4d* VehicleAeroMovable::GetCl_AngleAlphaBetaTablePtr()
{
   if (mCl_AngleAlphaBetaTable.IsValid())
   {
      return &mCl_AngleAlphaBetaTable;
   }

   return nullptr;
}

AeroTable3d* VehicleAeroMovable::GetClp_AngleMachTablePtr()
{
   if (mClp_AngleMachTable.IsValid())
   {
      return &mClp_AngleMachTable;
   }

   return nullptr;
}

AeroTable3d* VehicleAeroMovable::GetClq_AngleMachTablePtr()
{
   if (mClq_AngleMachTable.IsValid())
   {
      return &mClq_AngleMachTable;
   }

   return nullptr;
}

AeroTable3d* VehicleAeroMovable::GetClr_AngleMachTablePtr()
{
   if (mClr_AngleMachTable.IsValid())
   {
      return &mClr_AngleMachTable;
   }

   return nullptr;
}


const std::vector<std::string> VehicleAeroMovable::GetEnabledTables()
{
   std::vector<std::string> enabledTableList;

   // Lift
   if (mCL_AngleAlphaMachTable.IsValid())
   {
      enabledTableList.emplace_back(mCL_AngleAlphaMachTable.GetTableName());
   }

   // Drag
   if (mCd_AngleAlphaMachTable.IsValid())
   {
      enabledTableList.emplace_back(mCd_AngleAlphaMachTable.GetTableName());
   }
   if (mCd_AngleBetaMachTable.IsValid())
   {
      enabledTableList.emplace_back(mCd_AngleBetaMachTable.GetTableName());
   }
   if (mCd_AngleMachTable.IsValid())
   {
      enabledTableList.emplace_back(mCd_AngleMachTable.GetTableName());
   }

   // Side force
   if (mCY_AngleBetaMachTable.IsValid())
   {
      enabledTableList.emplace_back(mCY_AngleBetaMachTable.GetTableName());
   }

   // Pitching moment
   if (mCm_AngleAlphaMachTable.IsValid())
   {
      enabledTableList.emplace_back(mCm_AngleAlphaMachTable.GetTableName());
   }

   if (mCmq_AngleMachTable.IsValid())
   {
      enabledTableList.emplace_back(mCmq_AngleMachTable.GetTableName());
   }

   // Yawing moment
   if (mCn_AngleBetaMachTable.IsValid())
   {
      enabledTableList.emplace_back(mCn_AngleBetaMachTable.GetTableName());
   }

   if (mCnr_AngleMachTable.IsValid())
   {
      enabledTableList.emplace_back(mCnr_AngleMachTable.GetTableName());
   }

   // Rolling moment
   if (mCl_AngleAlphaBetaTable.IsValid())
   {
      enabledTableList.emplace_back(mCl_AngleAlphaBetaTable.GetTableName());
   }

   if (mClp_AngleMachTable.IsValid())
   {
      enabledTableList.emplace_back(mClp_AngleMachTable.GetTableName());
   }

   if (mClq_AngleMachTable.IsValid())
   {
      enabledTableList.emplace_back(mClq_AngleMachTable.GetTableName());
   }

   if (mClr_AngleMachTable.IsValid())
   {
      enabledTableList.emplace_back(mClr_AngleMachTable.GetTableName());
   }
   return enabledTableList;
}


AeroTableBase* VehicleAeroMovable::GetTableByName(QString aTableName)
{

   if (aTableName == "CL vs Angle Alpha Mach")
   {
      return GetCL_AngleAlphaMachTablePtr();
   }
   else if (aTableName == "Cd vs Angle Alpha Mach")
   {
      return GetCd_AngleAlphaMachTablePtr();
   }
   else if (aTableName == "Cd vs Angle Beta Mach")
   {
      return GetCd_AngleBetaMachTablePtr();
   }
   else if (aTableName == "Cd vs Angle Mach")
   {
      return GetCd_AngleMachTablePtr();
   }
   else if (aTableName == "CY vs Angle Beta Mach")
   {
      return GetCY_AngleBetaMachTablePtr();
   }
   else if (aTableName == "Cm vs Angle Alpha Mach")
   {
      return GetCm_AngleAlphaMachTablePtr();
   }
   else if (aTableName == "Cmq vs Angle Mach")
   {
      return GetCmq_AngleMachTablePtr();
   }
   else if (aTableName == "Cn vs Angle Beta Mach")
   {
      return GetCn_AngleBetaMachTablePtr();
   }
   else if (aTableName == "Cnr vs Angle Mach")
   {
      return GetCnr_AngleMachTablePtr();
   }
   else if (aTableName == "Cl vs Angle Alpha Beta")
   {
      return GetCl_AngleAlphaBetaTablePtr();
   }
   else if (aTableName == "Clp vs Angle Mach")
   {
      return GetClp_AngleMachTablePtr();
   }
   else if (aTableName == "Clq vs Angle Mach")
   {
      return GetClq_AngleMachTablePtr();
   }
   else if (aTableName == "Clr vs Angle Mach")
   {
      return GetClr_AngleMachTablePtr();
   }
   return nullptr;
}

} // namespace Designer
