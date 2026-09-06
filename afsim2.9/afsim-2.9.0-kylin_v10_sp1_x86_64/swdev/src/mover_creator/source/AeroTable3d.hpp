// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
///// ****************************************************************************
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// This software is provided 'As Is', with no warranties, either express or
// implied, and Infoscitex will not be liable for any damages arising in any way
// from the use of this software.
//
// WARNING: EXPORT CONTROLLED This document contains technical data whose
// export is restricted by the Arms Export Control Act (Title 22, U.S.C.
// Sec 2751 et seq) or Export Administration Act of 1979, as amended, Title 50
// U.S.C.; App. 2401, et seq.  Violators of these export laws are subject to
// severe criminal penalties. (DoD Directive 5230.25)
// ****************************************************************************

#ifndef AERO_TABLE_3D_HPP
#define AERO_TABLE_3D_HPP

#include "AeroTableBase.hpp"

// ****************************************************************************
// AeroTable3d is a "3 dimensional" aero table class.
//
// For a table such as Cd as a function of Alpha, Mach:
//   Cd is the dependent var
//   Alpha is the primary independent var
//   Mach is the secondary independent var
//
// ****************************************************************************

namespace Designer
{
   class AeroTable2d;

   class AeroTable3d : public AeroTableBase
   {
      public:
         AeroTable3d(const std::string& aDependentVarName,
                     const std::string& aPrimaryIndependentVarName,
                     const std::string& aSecondaryIndependentVarName);

         ~AeroTable3d() override = default;;

         AeroTable3d& operator=(const AeroTable3d& aRhs);

         AeroTable3d(const AeroTable3d& aSrc) = default;

         bool IsEmpty() override { return mBaseData.empty(); }

         void AddTable(double aSecondaryIndependentValue);
         void AddData(double aPrimaryIndependentValue, double aDependentVar);

         AeroTable2d* GetCurrent2dTable();

         // Returns a vector of data point pairs for the primary independent variable
         // at the specified secondary independent variable using base data. Returns
         // true if data is valid.
         bool GetPrimaryBaseData(const double&                           aSecondaryIndependentValue,
                                 std::vector<std::pair<double, double>>& aDataSet) const;

         // Returns a vector of data point pairs for the primary independent variable
         // using the tabular data points closest to the specified secondary independent variable
         // using base data. Returns true if data is valid.
         bool GetPrimaryBaseDataNearest(const double&                           aSecondaryIndependentValue,
                                        std::vector<std::pair<double, double>>& aDataSet) const;

         //Get a list of the secondary independent values in the table
         const std::vector<double> GetSecondaryIndependentValues();

         //Get a list of the Primary independent values in the table
         const std::vector<double> GetPrimaryIndependentValues();

         //Get the table name
         std::string GetTableName() const override { return mTableName; }


         // Returns a vector of data point pairs for the primary independent variable
         // at the specified secondary independent variable using modified data. Returns
         // true if data is valid.
         bool GetPrimaryModifiedData(const double&                           aSecondaryIndependentValue,
                                     std::vector<std::pair<double, double>>& aDataSet) const;

         // Returns a vector of data point pairs for the secondary independent variable
         // at the specified primary independent variable using base data. Returns
         // true if data is valid.
         bool GetSecondaryBaseData(const double&                           aPrimaryIndependentValue,
                                   std::vector<std::pair<double, double>>& aDataSet) const;

         // Returns a vector of data point pairs for the secondary independent variable
         // at the specified primary independent variable using modified data. Returns
         // true if data is valid.
         bool GetSecondaryModifiedData(const double&                           aPrimaryIndependentValue,
                                       std::vector<std::pair<double, double>>& aDataSet) const;

         // Interpolates using the base table with the specified primary and secondary independent values,
         // providing the result in aValue. Clamps to min/max values. Returns true if the result is valid.
         bool GetInterpolatedDependentValueBase(const double& aPrimaryIndependentValue,
                                                const double& aSecondaryIndependentValue,
                                                double&       aValue) const;

         // Interpolates using the modified table with the specified primary and secondary independent values,
         // providing the result in aValue. Clamps to min/max values. Returns true if the result is valid.
         bool GetInterpolatedDependentValueModified(const double& aPrimaryIndependentValue,
                                                    const double& aSecondaryIndependentValue,
                                                    double&       aValue) const;

         // Updates the modified data using the base data and the gain and offset modifiers,
         // if appropriate.
         void UpdateModifiedData() override;

         // Returns the primary independent var name
         std::string GetPrimaryIndependentVarName() const { return mPrimaryIndependentVarName; }

         // Returns the secondary independent var name
         std::string GetSecondaryIndependentVarName() const { return mSecondaryIndependentVarName; }

         // Returns the dependent var name
         std::string GetDependentVarName() { return mDependentVarName; }

         // This sets the list-based gain modifier
         void SetListBasedPrimaryGainModifier(const std::vector<std::pair<double, double>>& aModifierList);

         // This sets the single gain modifier
         void SetSinglePrimaryGainModifier(const double& aSingleModifier);

         // Clear the gain modifier. No gain modification will be performed until a new
         // command (SetListBasedPrimaryGainModifier or SetSinglePrimaryGainModifier) is called.
         void ClearPrimaryGainModifier();

         // This sets the list-based offset modifier
         void SetListBasedPrimaryOffsetModifier(const std::vector<std::pair<double, double>>& aModifierList);

         // This sets the single offset modifier
         void SetSinglePrimaryOffsetModifier(const double& aSingleModifier);

         // Clear the offset modifier. No offset modification will be performed until a new
         // command (SetListBasedPrimaryOffsetModifier or SetSinglePrimaryOffsetModifier) is called.
         void ClearPrimaryOffsetModifier();

         // This sets the list-based gain modifier
         void SetListBasedSecondaryGainModifier(const std::vector<std::pair<double, double>>& aModifierList);

         // This sets the single gain modifier
         void SetSingleSecondaryGainModifier(const double& aSingleModifier);

         // Clear the gain modifier. No gain modification will be performed until a new
         // command (SetListBasedSecondaryGainModifier or SetSingleSecondaryGainModifier) is called.
         void ClearSecondaryGainModifier();

         // This sets the list-based offset modifier
         void SetListBasedSecondaryOffsetModifier(const std::vector<std::pair<double, double>>& aModifierList);

         // This sets the single offset modifier
         void SetSingleSecondaryOffsetModifier(const double& aSingleModifier);

         // Clear the offset modifier. No offset modification will be performed until a new
         // command (SetListBasedSecondaryOffsetModifier or SetSingleSecondaryOffsetModifier) is called.
         void ClearSecondaryOffsetModifier();

         // This sets the external gain modifier
         void SetExternalGainModifier(const double& aModifier);

         // Clear the external gain modifier. No external gain modification will be performed
         // until a new command (SetListBasedGainModifier or SetListSingleGainModifier) is called.
         void ClearExternalGainModifier();

         // This sets the external offset modifier
         void SetExternalOffsetModifier(const double& aModifier);

         // Clear the external offset modifier. No external offset modification will be performed
         // until a new command (SetListBasedOffsetModifier or SetListSingleOffsetModifier) is called.
         void ClearExternalOffsetModifier();

         // Provides the minimum and maximum values for the independent variable. Returns true
         // if data is initialized and valid.
         bool MaxMinPrimaryIndependentVariableRange(double& aMinIndependentVar, double& aMaxIndependentVar);

         // Provides the minimum and maximum values for the independent variable. Returns true
         // if data is initialized and valid.
         bool MaxMinSecondaryIndependentVariableRange(double& aMinIndependentVar, double& aMaxIndependentVar);

         //revert the table's modified data back to matching the original data
         void RevertPlot() override;

         // Copies the base data to the modified data
         void CopyBaseDataToModifiedData();

         bool UsingSinglePrimaryGainModifer() const { return mUseSinglePrimaryGainModifier; }
         bool UsingPrimaryGainModiferList() const { return !mUseSinglePrimaryGainModifier &&  mUsePrimaryGainModifier; }

         bool UsingSinglePrimaryOffsetModifer() const { return mUseSinglePrimaryOffsetModifier; }
         bool UsingPrimaryOffsetModiferList() const { return !mUseSinglePrimaryOffsetModifier &&  mUsePrimaryOffsetModifier; }

         bool UsingSingleSecondaryGainModifer() const { return mUseSingleSecondaryGainModifier; }
         bool UsingSecondaryGainModiferList() const { return !mUseSingleSecondaryGainModifier &&  mUseSecondaryGainModifier; }

         bool UsingSingleSecondaryOffsetModifer() const { return mUseSingleSecondaryOffsetModifier; }
         bool UsingSecondaryOffsetModiferList() const { return !mUseSingleSecondaryOffsetModifier &&  mUseSecondaryOffsetModifier; }

         bool HasIndependentVarName(const std::string& aVarName) const override;

         void SetListBasedGainModifier(const std::string& aVarName, const std::vector<std::pair<double, double>>& aModifierLine) override;
         void SetSingleGainModifier(const std::string& aVarName, double aModifierValue) override;
         void SetListBasedOffsetModifier(const std::string& aVarName, const std::vector<std::pair<double, double>>& aModifierLine) override;
         void SetSingleOffsetModifier(const std::string& aVarName, double aModifierValue) override;

      private:

         // This clears all data within the tables
         void ClearAllData() override;

         std::vector<std::pair<double, AeroTable2d>> mBaseData;
         std::vector<std::pair<double, AeroTable2d>> mModifiedData;

         std::vector<std::pair<double, double>>      mPrimaryGainModifierList;
         double                                      mSinglePrimaryGainModifier                = 1.0;
         bool                                        mUseSinglePrimaryGainModifier             = true;
         bool                                        mUsePrimaryGainModifier                   = false;

         std::vector<std::pair<double, double>>      mPrimaryOffsetModifierList;
         double                                      mSinglePrimaryOffsetModifier              = 0.0;
         bool                                        mUseSinglePrimaryOffsetModifier           = true;
         bool                                        mUsePrimaryOffsetModifier                 = false;

         std::vector<std::pair<double, double>>      mSecondaryGainModifierList;
         double                                      mSingleSecondaryGainModifier              = 1.0;
         bool                                        mUseSingleSecondaryGainModifier           = true;
         bool                                        mUseSecondaryGainModifier                 = false;

         std::vector<std::pair<double, double>>      mSecondaryOffsetModifierList;
         double                                      mSingleSecondaryOffsetModifier            = 0.0;
         bool                                        mUseSingleSecondaryOffsetModifier         = true;
         bool                                        mUseSecondaryOffsetModifier               = false;

         double                                      mMinPrimaryIndependentVar                 = 0.0;
         double                                      mMaxPrimaryIndependentVar                 = 0.0;
         bool                                        mMaxMinPrimaryIndependentVarInitialized   = false;

         double                                      mMinSecondaryIndependentVar               = 0.0;
         double                                      mMaxSecondaryIndependentVar               = 0.0;
         bool                                        mMaxMinSecondaryIndependentVarInitialized = false;

         double                                      mExternalGainModifier                     = 1.0;
         double                                      mExternalOffsetModifier                   = 0.0;
         bool                                        mUseExternalGainModifier                  = false;
         bool                                        mUseExternalOffsetModifier                = false;

         bool                                        mDirtyDataPresent                         = false;

         std::string                                 mPrimaryIndependentVarName;
         std::string                                 mSecondaryIndependentVarName;
         std::string                                 mDependentVarName;
         std::string                                 mTableName;
   };
}
#endif // ! AERO_TABLE_3D_HPP
