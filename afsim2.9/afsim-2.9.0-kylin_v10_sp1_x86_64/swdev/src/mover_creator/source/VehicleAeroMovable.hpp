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

#ifndef VEHICLE_AERO_MOVABLE_HPP
#define VEHICLE_AERO_MOVABLE_HPP

#include <string>

#include "AeroTable2d.hpp"
#include "AeroTable3d.hpp"
#include "AeroTable4d.hpp"

// VehicleAeroMovable provides the core (non-movable) aero for the vehicle.

namespace Designer
{
   class AeroObject;
   class VehicleAero;

   class VehicleAeroMovable
   {
      public:
         explicit VehicleAeroMovable(const std::string& aName, AeroObject* aAeroObject);
         ~VehicleAeroMovable() = default;

         std::string GetName()       { return mName; }
         AeroObject* GetAeroObject() { return mAeroObject; }

         // This will clear all tabular data
         void ClearAllData();

         // Each of these functions returns a pointer to the table if it exists -- otherwise returns nullptr
         AeroTable4d* GetCL_AngleAlphaMachTablePtr();
         AeroTable4d* GetCd_AngleAlphaMachTablePtr();
         AeroTable4d* GetCd_AngleBetaMachTablePtr();
         AeroTable3d* GetCd_AngleMachTablePtr();
         AeroTable4d* GetCY_AngleBetaMachTablePtr();
         AeroTable4d* GetCm_AngleAlphaMachTablePtr();
         AeroTable3d* GetCmq_AngleMachTablePtr();
         AeroTable4d* GetCn_AngleBetaMachTablePtr();
         AeroTable3d* GetCnr_AngleMachTablePtr();
         AeroTable4d* GetCl_AngleAlphaBetaTablePtr();
         AeroTable3d* GetClp_AngleMachTablePtr();
         AeroTable3d* GetClq_AngleMachTablePtr();
         AeroTable3d* GetClr_AngleMachTablePtr();

         // If aCheckIfEmpty is true, the table will only be enabled if the table is non-empty
         void EnableCL_AngleAlphaMachTablePtr() { mCL_AngleAlphaMachTable.Enable(); }
         void EnableCd_AngleAlphaMachTablePtr() { mCd_AngleAlphaMachTable.Enable(); }
         void EnableCd_AngleBetaMachTablePtr()  { mCd_AngleBetaMachTable.Enable(); }
         void EnableCd_AngleMachTablePtr()      { mCd_AngleMachTable.Enable(); }
         void EnableCY_AngleBetaMachTablePtr()  { mCY_AngleBetaMachTable.Enable(); }
         void EnableCm_AngleAlphaMachTablePtr() { mCm_AngleAlphaMachTable.Enable(); }
         void EnableCmq_AngleMachTablePtr()     { mCmq_AngleMachTable.Enable(); }
         void EnableCn_AngleBetaMachTablePtr()  { mCn_AngleBetaMachTable.Enable(); }
         void EnableCnr_AngleMachTablePtr()     { mCnr_AngleMachTable.Enable(); }
         void EnableCl_AngleAlphaBetaTablePtr() { mCl_AngleAlphaBetaTable.Enable(); }
         void EnableClp_AngleMachTablePtr()     { mClp_AngleMachTable.Enable(); }
         void EnableClq_AngleMachTablePtr()     { mClq_AngleMachTable.Enable(); }
         void EnableClr_AngleMachTablePtr()     { mClr_AngleMachTable.Enable(); }

         void DisableCL_AngleAlphaMachTablePtr() { mCL_AngleAlphaMachTable.Disable(); }
         void DisableCd_AngleAlphaMachTablePtr() { mCd_AngleAlphaMachTable.Disable(); }
         void DisableCd_AngleBetaMachTablePtr()  { mCd_AngleBetaMachTable.Disable(); }
         void DisableCd_AngleMachTablePtr()      { mCd_AngleMachTable.Disable(); }
         void DisableCY_AngleBetaMachTablePtr()  { mCY_AngleBetaMachTable.Disable(); }
         void DisableCm_AngleAlphaMachTablePtr() { mCm_AngleAlphaMachTable.Disable(); }
         void DisableCmq_AngleMachTablePtr()     { mCmq_AngleMachTable.Disable(); }
         void DisableCn_AngleBetaMachTablePtr()  { mCn_AngleBetaMachTable.Disable(); }
         void DisableCnr_AngleMachTablePtr()     { mCnr_AngleMachTable.Disable(); }
         void DisableCl_AngleAlphaBetaTablePtr() { mCl_AngleAlphaBetaTable.Disable(); }
         void DisableClp_AngleMachTablePtr()     { mClp_AngleMachTable.Disable(); }
         void DisableClq_AngleMachTablePtr()     { mClq_AngleMachTable.Disable(); }
         void DisableClr_AngleMachTablePtr()     { mClr_AngleMachTable.Disable(); }

         //Returns a list of all of the currently enabled tables for this object
         const std::vector<std::string> GetEnabledTables();

         //returns the table given the table name
         AeroTableBase* GetTableByName(QString aTableName);

      private:
         // Lift
         AeroTable4d mCL_AngleAlphaMachTable   = AeroTable4d(std::string("CL"), std::string("Angle"), std::string("Alpha"), std::string("Mach"));

         // Drag - Only one of these should be used
         AeroTable4d mCd_AngleAlphaMachTable   = AeroTable4d(std::string("Cd"), std::string("Angle"), std::string("Alpha"), std::string("Mach"));
         AeroTable4d mCd_AngleBetaMachTable    = AeroTable4d(std::string("Cd"), std::string("Angle"), std::string("Beta"),  std::string("Mach"));
         AeroTable3d mCd_AngleMachTable        = AeroTable3d(std::string("Cd"), std::string("Angle"), std::string("Mach"));

         // Side force
         AeroTable4d mCY_AngleBetaMachTable    = AeroTable4d(std::string("CY"), std::string("Angle"), std::string("Beta"), std::string("Mach"));

         // Pitching moments
         AeroTable4d mCm_AngleAlphaMachTable   = AeroTable4d(std::string("Cm"), std::string("Angle"), std::string("Alpha"), std::string("Mach"));
         AeroTable3d mCmq_AngleMachTable       = AeroTable3d(std::string("Cmq"), std::string("Angle"), std::string("Mach"));

         // Yawing moments
         AeroTable4d mCn_AngleBetaMachTable    = AeroTable4d(std::string("Cn"), std::string("Angle"), std::string("Beta"), std::string("Mach"));
         AeroTable3d mCnr_AngleMachTable       = AeroTable3d(std::string("Cnr"), std::string("Angle"), std::string("Mach"));

         // Rolling moments
         AeroTable4d mCl_AngleAlphaBetaTable   = AeroTable4d(std::string("Cl"), std::string("Angle"), std::string("Alpha"), std::string("Beta"));
         AeroTable3d mClp_AngleMachTable       = AeroTable3d(std::string("Clp"), std::string("Angle"), std::string("Mach"));
         AeroTable3d mClq_AngleMachTable       = AeroTable3d(std::string("Clq"), std::string("Angle"), std::string("Mach"));
         AeroTable3d mClr_AngleMachTable       = AeroTable3d(std::string("Clr"), std::string("Angle"), std::string("Mach"));

         std::string mName                     = "";
         AeroObject* mAeroObject               = nullptr;

   };
}
#endif // !VEHICLE_AERO_MOVABLE_HPP
