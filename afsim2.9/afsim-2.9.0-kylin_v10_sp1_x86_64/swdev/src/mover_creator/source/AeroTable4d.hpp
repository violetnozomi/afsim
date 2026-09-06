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

#ifndef AERO_TABLE_4D_HPP
#define AERO_TABLE_4D_HPP

#include "AeroTableBase.hpp"

// ****************************************************************************
// AeroTable4d is a "4 dimensional" aero table class.
//
// For a table such as Cd as a function of Alpha, Beta, Mach:
//   Cd is the dependent var
//   Alpha is the primary independent var
//   Beta is the secondary independent var
//   Mach is the tertiary independent var
//
// ****************************************************************************

namespace Designer
{
   class AeroTablesUtil;
   class AeroTable2d;
   class AeroTable3d;

   class AeroTable4d : public AeroTableBase
   {
      public:
         AeroTable4d(const std::string& aDependentVarName,
                     const std::string& aPrimaryIndependentVarName,
                     const std::string& aSecondaryIndependentVarName,
                     const std::string& aTertiaryIndependentVarName);

         ~AeroTable4d() override = default;

         AeroTable4d& operator=(const AeroTable4d& aRhs);

         AeroTable4d(const AeroTable4d& aSrc) = default;

         bool IsEmpty() override { return mBaseData.empty(); }

         void AddSecondaryTable(double aTertiaryIndependentValue);
         void AddPrimaryTable(double aSecondaryIndependentValue);
         void AddData(double aPrimaryIndependentValue, double aDependentVar);

         AeroTable2d* GetCurrent2dTable();

         // Returns a vector of data point pairs for the primary independent variable
         // at the specified secondary and tertiary independent variables using base data.
         // Returns true if data is valid.
         bool GetPrimaryBaseData(const double&                           aSecondaryIndependentValue,
                                 const double&                           aTertiaryIndependentValue,
                                 std::vector<std::pair<double, double>>& aDataSet) const;

         // Returns a vector of data point pairs for the primary independent variable
         // at the specified secondary and tertiary independent variables using modified data.
         // Returns true if data is valid.
         bool GetPrimaryModifiedData(const double&                           aSecondaryIndependentValue,
                                     const double&                           aTertiaryIndependentValue,
                                     std::vector<std::pair<double, double>>& aDataSet) const;

         // Returns a vector of data point pairs for the primary independent variable
         // using the tabular data points closest to the specified secondary and tertiary independent variables
         // using base data. Returns true if data is valid.
         bool GetPrimaryBaseDataNearest(const double&                           aSecondaryIndependentValue,
                                        const double&                           aTertiaryIndependentValue,
                                        std::vector<std::pair<double, double>>& aDataSet) const;

         // Returns a vector of data point pairs for the secondary independent variable
         // at the specified primary and tertiary independent variables using base data.
         // Returns true if data is valid.
         bool GetSecondaryBaseData(const double&                           aPrimaryIndependentValue,
                                   const double&                           aTertiaryIndependentValue,
                                   std::vector<std::pair<double, double>>& aDataSet) const;

         // Returns a vector of data point pairs for the secondary independent variable
         // at the specified primary and tertiary independent variables using modified data.
         // Returns true if data is valid.
         bool GetSecondaryModifiedData(const double&                           aPrimaryIndependentValue,
                                       const double&                           aTertiaryIndependentValue,
                                       std::vector<std::pair<double, double>>& aDataSet) const;

         // Returns a vector of data point pairs for the tertiary independent variable
         // at the specified primary and secondary independent variables using base data.
         // Returns true if data is valid.
         bool GetTertiaryBaseData(const double&                           aPrimaryIndependentValue,
                                  const double&                           aSecondaryIndependentValue,
                                  std::vector<std::pair<double, double>>& aDataSet) const;

         // Returns a vector of data point pairs for the tertiary independent variable
         // at the specified primary and secondary independent variables using modified data.
         // Returns true if data is valid.
         bool GetTertiaryModifiedData(const double&                           aPrimaryIndependentValue,
                                      const double&                           aSecondaryIndependentValue,
                                      std::vector<std::pair<double, double>>& aDataSet) const;

         // Interpolates using the base table with the specified primary, secondary, and tertiary independent values,
         // providing the result in aValue. Clamps to min/max values. Returns true if the result is valid.
         bool GetInterpolatedDependentValueBase(const double& aPrimaryIndependentValue,
                                                const double& aSecondaryIndependentValue,
                                                const double& aTertiaryIndependentValue,
                                                double&       aValue) const;

         // Interpolates using the modified table with the specified primary, secondary, and tertiary independent values,
         // providing the result in aValue. Clamps to min/max values. Returns true if the result is valid.
         bool GetInterpolatedDependentValueModified(const double& aPrimaryIndependentValue,
                                                    const double& aSecondaryIndependentValue,
                                                    const double& aTertiaryIndependentValue,
                                                    double&       aValue) const;

         // Updates the modified data using the base data and the gain and offset modifiers,
         // if appropriate.
         void UpdateModifiedData() override;

         // Returns the primary independent var name
         std::string GetPrimaryIndependentVarName() const { return mPrimaryIndependentVarName; }

         // Returns the secondary independent var name
         std::string GetSecondaryIndependentVarName() const { return mSecondaryIndependentVarName; }

         // Returns the tertiary independent var name
         std::string GetTertiaryIndependentVarName() const { return mTertiaryIndependentVarName; }

         // Returns the dependent var name
         std::string GetDependentVarName() const { return mDependentVarName; }

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

         // This sets the list-based gain modifier
         void SetListBasedTertiaryGainModifier(const std::vector<std::pair<double, double>>& aModifierList);

         // This sets the single gain modifier
         void SetSingleTertiaryGainModifier(const double& aSingleModifier);

         // Clear the gain modifier. No gain modification will be performed until a new
         // command (SetListBasedTertiaryGainModifier or SetSingleTertiaryGainModifier) is called.
         void ClearTertiaryGainModifier();

         // This sets the list-based offset modifier
         void SetListBasedTertiaryOffsetModifier(const std::vector<std::pair<double, double>>& aModifierList);

         // This sets the single offset modifier
         void SetSingleTertiaryOffsetModifier(const double& aSingleModifier);

         // Clear the offset modifier. No offset modification will be performed until a new
         // command (SetListBasedTertiaryOffsetModifier or SetSingleTertiaryOffsetModifier) is called.
         void ClearTertiaryOffsetModifier();

         // Provides the minimum and maximum values for the independent variable. Returns true
         // if data is initialized and valid.
         bool MaxMinPrimaryIndependentVariableRange(double& aMinIndependentVar, double& aMaxIndependentVar);

         // Provides the minimum and maximum values for the independent variable. Returns true
         // if data is initialized and valid.
         bool MaxMinSecondaryIndependentVariableRange(double& aMinIndependentVar, double& aMaxIndependentVar);

         // Provides the minimum and maximum values for the independent variable. Returns true
         // if data is initialized and valid.
         bool MaxMinTertiaryIndependentVariableRange(double& aMinIndependentVar, double& aMaxIndependentVar);

         //Provides a list of the tertiary independent values
         const std::vector<double> GetTertiaryIndepenentValues();

         //Provides a list of the secondary independent values
         const std::vector<double> GetSecondaryIndependentValues();

         //Provides a list of the secondary independent values at the given tertiary value
         const std::vector<double> GetSecondaryIndependentValues(const double& aTertiaryIndependentValue);

         //Provides a list of the primary independent values at the given tertiary value
         const std::vector<double> GetPrimaryIndependentValuesGivenTertiary(const double& aTertiaryValue);

         const std::vector<double> GetPrimaryIndependentValues();

         std::string GetTableName() const override { return mTableName; }

         //revert the table's modified data back to matching the original data
         void RevertPlot() override;

         // Copies the base data to the modified data
         void CopyBaseDataToModifiedData();

         bool UsingSinglePrimaryGainModifer() const { return mUseSinglePrimaryGainModifier;  }
         bool UsingPrimaryGainModiferList() const { return !mUseSinglePrimaryGainModifier &&  mUsePrimaryGainModifier; }

         bool UsingSinglePrimaryOffsetModifer() const { return mUseSinglePrimaryOffsetModifier; }
         bool UsingPrimaryOffsetModiferList() const { return !mUseSinglePrimaryOffsetModifier &&  mUsePrimaryOffsetModifier; }

         bool UsingSingleSecondaryGainModifer() const { return mUseSingleSecondaryGainModifier; }
         bool UsingSecondaryGainModiferList() const { return !mUseSingleSecondaryGainModifier &&  mUseSecondaryGainModifier; }

         bool UsingSingleSecondaryOffsetModifer() const { return mUseSingleSecondaryOffsetModifier; }
         bool UsingSecondaryOffsetModiferList() const { return !mUseSingleSecondaryOffsetModifier &&  mUseSecondaryOffsetModifier; }

         bool UsingSingleTertiaryGainModifer() const { return mUseSingleTertiaryGainModifier; }
         bool UsingTertiaryGainModiferList() const { return !mUseSingleTertiaryGainModifier &&  mUseTertiaryGainModifier; }

         bool UsingSingleTertiaryOffsetModifer() const { return mUseSingleTertiaryOffsetModifier; }
         bool UsingTertiaryOffsetModiferList() const { return !mUseSingleTertiaryOffsetModifier &&  mUseTertiaryOffsetModifier; }

         bool HasIndependentVarName(const std::string& aVarName) const override;

         void SetListBasedGainModifier(const std::string& aVarName, const std::vector<std::pair<double, double>>& aModifierLine) override;
         void SetSingleGainModifier(const std::string& aVarName, double aModifierValue) override;
         void SetListBasedOffsetModifier(const std::string& aVarName, const std::vector<std::pair<double, double>>& aModifierLine) override;
         void SetSingleOffsetModifier(const std::string& aVarName, double aModifierValue) override;

      private:

         // This clears all data within the tables
         void ClearAllData() override;

         std::vector<std::pair<double, AeroTable3d>> mBaseData;
         std::vector<std::pair<double, AeroTable3d>> mModifiedData;

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

         std::vector<std::pair<double, double>>      mTertiaryGainModifierList;
         double                                      mSingleTertiaryGainModifier               = 1.0;
         bool                                        mUseSingleTertiaryGainModifier            = true;
         bool                                        mUseTertiaryGainModifier                  = false;

         std::vector<std::pair<double, double>>      mTertiaryOffsetModifierList;
         double                                      mSingleTertiaryOffsetModifier             = 0.0;
         bool                                        mUseSingleTertiaryOffsetModifier          = true;
         bool                                        mUseTertiaryOffsetModifier                = false;

         double                                      mMinPrimaryIndependentVar                 = 0.0;
         double                                      mMaxPrimaryIndependentVar                 = 0.0;
         bool                                        mMaxMinPrimaryIndependentVarInitialized   = false;

         double                                      mMinSecondaryIndependentVar               = 0.0;
         double                                      mMaxSecondaryIndependentVar               = 0.0;
         bool                                        mMaxMinSecondaryIndependentVarInitialized = false;

         double                                      mMinTertiaryIndependentVar                = 0.0;
         double                                      mMaxTertiaryIndependentVar                = 0.0;
         bool                                        mMaxMinTertiaryIndependentVarInitialized  = false;

         bool                                        mDirtyDataPresent                         = false;

         std::string                                 mDependentVarName;
         std::string                                 mPrimaryIndependentVarName;
         std::string                                 mSecondaryIndependentVarName;
         std::string                                 mTertiaryIndependentVarName;
         std::string                                 mTableName;
   };
}
#endif // ! AERO_TABLE_4D_HPP
