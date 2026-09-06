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

#ifndef WSFRIGIDBODYSIXDOFUTILS_HPP
#define WSFRIGIDBODYSIXDOFUTILS_HPP

#include "wsf_six_dof_export.h"

#include <memory>
#include <string>
#include <vector>

#include "UtVec3dX.hpp"
#include "WsfRigidBodySixDOF_VehicleData.hpp"
#include "WsfSixDOF_Mover.hpp"
#include "WsfSixDOF_Utils.hpp"

namespace wsf
{
namespace six_dof
{
class RigidBodyTableTool;
class RigidBodyMover;

namespace utils
{
WSF_SIX_DOF_EXPORT RigidBodyMover* CastToRigidBody(Mover* aVehicle);

WSF_SIX_DOF_EXPORT void Autotune(RigidBodyMover* aVehicle);
WSF_SIX_DOF_EXPORT void CreateAutopilotConfigFile(const RigidBodyMover* aVehicle, const std::string& aFilename);
WSF_SIX_DOF_EXPORT std::vector<Pid::Type> GetRelevantPids(const RigidBodyMover* aVehicle);

// The throttle input assumes that 0 is idle, 1 is full power without
// augmentation/afterburner (military power), and 2 is full power with
// full augmentation/afterburner (full afterburner).
WSF_SIX_DOF_EXPORT CanFlyAtReturnValue
CanFlyAt(RigidBodyMover* aObject, double aAltitude_ft, double aVelocity_fps, double aThrottleForward, double aFlapsDown);

// The filename is typically "autopilot_support_tables.txt"
WSF_SIX_DOF_EXPORT void CreateAutopilotSupportFile(RigidBodyMover* aObject, const std::string& aFilename);

// The filename is typically "autopilot_support_tables.txt"
WSF_SIX_DOF_EXPORT void CreateAutopilotSupportFileWithMaxMach(RigidBodyMover*    aObject,
                                                              const std::string& aFilename,
                                                              const double&      aMaxMach);

// This provides the processing that is common between CreateAutopilotSupportFile()
// and CreateAutopilotSupportFileWithMaxMach()
WSF_SIX_DOF_EXPORT void CommonCreateAutopilotSupportFile(RigidBodyTableTool& aTool, const std::string& aFilename);

// This is used for the earth radius when mUseSphericalEarth is true
// Note: The AFSIM cEARTH_RADIUS = 6366707.0194937074958 (6366.7 km)
// Typical earth radius is 6371 km -- note the 4.3 km discrepancy
// Distances from points on the surface to the center range from
// 6,353 km to 6,384 km. Several different ways of modeling the
// Earth as a sphere each yield a mean radius of 6,371 km.

WSF_SIX_DOF_EXPORT double GetSphericalEarthRadius_m();
} // namespace utils

class WSF_SIX_DOF_EXPORT RigidBodyMachParamDataTable
{
public:
   RigidBodyMachParamDataTable();
   ~RigidBodyMachParamDataTable();

   RigidBodyMachParamDataTable& operator=(const RigidBodyMachParamDataTable& other) = delete;

   RigidBodyMachParamDataTable* Clone() const;

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
   RigidBodyMachParamDataTable(const RigidBodyMachParamDataTable& aSrc);

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

class WSF_SIX_DOF_EXPORT RigidBodyMachDataTable
{
public:
   RigidBodyMachDataTable();
   ~RigidBodyMachDataTable();

   RigidBodyMachDataTable& operator=(const RigidBodyMachDataTable& other) = delete;

   RigidBodyMachDataTable* Clone() const;

   void CreateNewMachValuePair(double aMach, double aValue);

   double GetValueAtMach(double aMach) const;

   void OutputToString(std::string& aInput, const std::string& aTableName) const;

protected:
   RigidBodyMachDataTable(const RigidBodyMachDataTable& aSrc);

   struct machDataPair
   {
      double mach;
      double value;
   };

   std::vector<machDataPair*> mMachDataList;
};

// -------------------------------------------------------------------------------

class WSF_SIX_DOF_EXPORT RigidBodyTableTool
{
public:
   explicit RigidBodyTableTool(RigidBodyMover* aObject);
   ~RigidBodyTableTool() = default;

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

   RigidBodyMover*                              mObject;
   std::unique_ptr<RigidBodyMachDataTable>      mCLMaxVsMachTable;
   std::unique_ptr<RigidBodyMachDataTable>      mCLMinVsMachTable;
   std::unique_ptr<RigidBodyMachDataTable>      mAlphaMax_degVsMachTable;
   std::unique_ptr<RigidBodyMachDataTable>      mAlphaMin_degVsMachTable;
   std::unique_ptr<RigidBodyMachParamDataTable> mStickForZeroMomentVsMachAlphaTablePtr;            // Mach-Alpha
   std::unique_ptr<RigidBodyMachParamDataTable> mThrustDeltaStickForZeroMomentVsMachAlphaTablePtr; // Mach-Alpha
   std::unique_ptr<RigidBodyMachParamDataTable> mXcgDeltaStickForZeroMomentVsMachAlphaTablePtr;    // Mach-Alpha
   std::unique_ptr<RigidBodyMachParamDataTable> mZcgDeltaStickForZeroMomentVsMachAlphaTablePtr;    // Mach-Alpha
   std::unique_ptr<RigidBodyMachParamDataTable> mCLVsMachAlphaTable;
   std::unique_ptr<RigidBodyMachParamDataTable> mAlphaVsMachCLTable;

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
} // namespace six_dof
} // namespace wsf

#endif
