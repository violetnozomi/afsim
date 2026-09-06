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

#ifndef WSFPOINTMASSSIXDOFUTILS_HPP
#define WSFPOINTMASSSIXDOFUTILS_HPP

#include "wsf_six_dof_export.h"

#include <memory>
#include <string>
#include <vector>

#include "UtVec3dX.hpp"
#include "WsfPointMassSixDOF_VehicleData.hpp"
#include "WsfSixDOF_Utils.hpp"

namespace wsf
{
namespace six_dof
{
class Mover;
class PointMassMover;
class PointMassTableTool;

namespace utils
{
WSF_SIX_DOF_EXPORT PointMassMover* CastToPointMass(Mover* aVehicle);

WSF_SIX_DOF_EXPORT void CreateAutopilotConfigFile(PointMassMover* aVehicle, const std::string& aFilename);

// The throttle input assumes that 0 is idle, 1 is full power without
// augmentation/afterburner (military power), and 2 is full power with
// full augmentation/afterburner (full afterburner).
WSF_SIX_DOF_EXPORT CanFlyAtReturnValue
CanFlyAt(PointMassMover* aObject, double aAltitude_ft, double aVelocity_fps, double aThrottleForward, double aFlapsDown);

// The filename is typically "autopilot_support_tables.txt"
WSF_SIX_DOF_EXPORT void CreateAutopilotSupportFile(PointMassMover* aObject, const std::string& aFilename);

// The filename is typically "autopilot_support_tables.txt"
WSF_SIX_DOF_EXPORT void CreateAutopilotSupportFileWithMaxMach(PointMassMover*    aObject,
                                                              const std::string& aFilename,
                                                              const double&      aMaxMach);

// This provides the processing that is common between CreateAutopilotSupportFile()
// and CreateAutopilotSupportFileWithMaxMach()
WSF_SIX_DOF_EXPORT void CommonCreateAutopilotSupportFile(PointMassTableTool& aTool, const std::string& aFilename);

} // namespace utils

class WSF_SIX_DOF_EXPORT PointMassMachParamDataTable
{
public:
   PointMassMachParamDataTable();
   ~PointMassMachParamDataTable();

   PointMassMachParamDataTable& operator=(const PointMassMachParamDataTable& other) = delete;

   PointMassMachParamDataTable* Clone() const;

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
   PointMassMachParamDataTable(const PointMassMachParamDataTable& aSrc);

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

class WSF_SIX_DOF_EXPORT PointMassMachDataTable
{
public:
   PointMassMachDataTable();
   ~PointMassMachDataTable();

   PointMassMachDataTable& operator=(const PointMassMachDataTable& other) = delete;

   PointMassMachDataTable* Clone() const;

   void CreateNewMachValuePair(double aMach, double aValue);

   double GetValueAtMach(double aMach) const;

   void OutputToString(std::string& aInput, const std::string& aTableName) const;

protected:
   PointMassMachDataTable(const PointMassMachDataTable& aSrc);

   struct machDataPair
   {
      double mach;
      double value;
   };

   std::vector<machDataPair*> mMachDataList;
};

// -------------------------------------------------------------------------------

class PointMassMover;

class WSF_SIX_DOF_EXPORT PointMassTableTool
{
public:
   explicit PointMassTableTool(PointMassMover* aObject);
   ~PointMassTableTool() = default;

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
   void OutputCLMachAlphaTableToInputString(std::string& aInput) const;

protected:
   double CalcCLFromCLArea(double aCLArea);

   void CreateMachTestList(double aMaxMach);

   void AddAlphaVsMachCL(double aMach, double aCL, double aMinAlpha_deg, double aMaxAlpha_deg, bool& aMachAdded);

   PointMassMover*                              mObject;
   std::unique_ptr<PointMassMachDataTable>      mCLMaxVsMachTable;
   std::unique_ptr<PointMassMachDataTable>      mCLMinVsMachTable;
   std::unique_ptr<PointMassMachDataTable>      mAlphaMax_degVsMachTable;
   std::unique_ptr<PointMassMachDataTable>      mAlphaMin_degVsMachTable;
   std::unique_ptr<PointMassMachParamDataTable> mCLVsMachAlphaTable;
   std::unique_ptr<PointMassMachParamDataTable> mAlphaVsMachCLTable;

   // These lists are used to store effective lists of mach, alpha,
   // and beta values for generating tabular data.
   std::vector<double> mMachTestList;
   bool                mTableListsInitialized;

private:
   void Initialize();
   bool CreateCLMaxAndAlphaMaxTables();
   bool CreateCLMinAndAlphaMinTables();
   bool CreateCLTable();
   bool CreateAlphaVsMachCLTable();
};
} // namespace six_dof
} // namespace wsf

#endif
