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

#ifndef AERO_TABLE_2D_HPP
#define AERO_TABLE_2D_HPP

#include "AeroTableBase.hpp"

// ****************************************************************************
// AeroTable2d is a "2 dimensional" aero table class.
// ****************************************************************************

namespace Designer
{
   class AeroTable2d : public AeroTableBase
   {
      public:
         AeroTable2d(const std::string& aDependentVarName,
                     const std::string& aIndependentVarName);

         ~AeroTable2d() override = default;

         AeroTable2d& operator= (const AeroTable2d& aRhs);

         AeroTable2d(const AeroTable2d& aSrc) = default;

         bool IsEmpty() override { return mBaseData.empty(); }

         // Add a single data pair to the table
         void AddData(double aIndependentVar, double aDependentVar);

         // Returns a vector of data point pairs for the base data.
         // Returns true if valid.
         bool GetBaseData(std::vector<std::pair<double, double>>& aDataset) const;

         // Returns the pointer to the vector of data point pairs for the base data.
         // Returns true if valid.
         bool GetBaseData(std::vector<std::pair<double, double>>*& aDataset);

         // Returns a vector of data point pairs for the modified data.
         // Returns true if valid.
         bool GetModifiedData(std::vector<std::pair<double, double>>& aDataset) const;

         // Interpolates using the base table with the specified independent value, providing
         // the result in aValue. Clamps to min/max values. Returns true if the result is valid.
         bool GetInterpolatedDependentValueBase(const double& aIndependentValue, double& aValue) const;

         // Interpolates using the modified table with the specified independent value, providing
         // the result in aValue. Clamps to min/max values. Returns true if the result is valid.
         bool GetInterpolatedDependentValueModified(const double& aIndependentValue, double& aValue) const;

         void ClearBaseData();

         // Updates the modified data using the base data and the gain and offset modifiers,
         // if appropriate.
         void UpdateModifiedData() override;

         // Returns the independent var name
         std::string GetIndependentVarName() const { return mIndependentVarName; }

         // Returns the dependent var name
         std::string GetDependentVarName() const { return mDependentVarName; }

         //Returns the table name
         std::string GetTableName() const override { return mTableName; }

         // This sets the list-based gain modifier
         void SetListBasedGainModifier(const std::vector<std::pair<double, double>>& aModifierList);

         // This sets the single gain modifier
         void SetSingleGainModifier(const double& aSingleModifier);

         // Clear the gain modifier. No gain modification will be performed until a new
         // command (SetListBasedGainModifier or SetSingleGainModifier) is called.
         void ClearGainModifier();

         // This sets the list-based offset modifier
         void SetListBasedOffsetModifier(const std::vector<std::pair<double, double>>& aModifierList);

         // This sets the single offset modifier
         void SetSingleOffsetModifier(const double& aSingleModifier);

         // Clear the offset modifier. No offset modification will be performed until a new
         // command (SetListBasedOffsetModifier or SetSingleOffsetModifier) is called.
         void ClearOffsetModifier();

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
         bool MaxMinIndependentVariableRange(double& aMinIndependentVar, double& aMaxIndependentVar);

         // Provides the base data dependent value at the specified index. Returns true if valid.
         bool GetDependentVarAtIndex(size_t aIndex, double& aValue);

         // Provides the base data independent value at the specified index. Returns true if valid.
         bool GetIndependentVarAtIndex(size_t aIndex, double& aValue);

         // Revert the table's modified data back to matching the original data
         void RevertPlot() override;

         // Copies the base data to the modified data
         void CopyBaseDataToModifiedData();

         bool UsingSinglePrimaryGainModifer() const { return mUseSingleGainModifier && mUseGainModifier; }
         bool UsingPrimaryGainModiferList() const { return !mUseSingleGainModifier && mUseGainModifier; }

         bool UsingSinglePrimaryOffsetModifer() const { return mUseSingleOffsetModifier && mUseOffsetModifier; }
         bool UsingPrimaryOffsetModiferList() const { return !mUseSingleOffsetModifier && mUseOffsetModifier; }

         void SetDirty(bool aDirty = true) { mDirtyDataPresent = aDirty; }

         bool HasIndependentVarName(const std::string& aVarName) const override;

         void SetListBasedGainModifier(const std::string& aVarName, const std::vector<std::pair<double, double>>& aModifierLine) override;
         void SetSingleGainModifier(const std::string& aVarName, double aModifierValue) override;
         void SetListBasedOffsetModifier(const std::string& aVarName, const std::vector<std::pair<double, double>>& aModifierLine) override;
         void SetSingleOffsetModifier(const std::string& aVarName, double aModifierValue) override;

      private:

         // This clears all data within the table
         void ClearAllData() override;

         std::vector<std::pair<double, double>> mBaseData;
         std::vector<std::pair<double, double>> mModifiedData;

         std::vector<std::pair<double, double>> mGainModifierList;
         double                                 mSingleGainModifier              = 1.0;
         bool                                   mUseSingleGainModifier           = true;
         bool                                   mUseGainModifier                 = false;

         std::vector<std::pair<double, double>> mOffsetModifierList;
         double                                 mSingleOffsetModifier            = 0.0;
         bool                                   mUseSingleOffsetModifier         = true;
         bool                                   mUseOffsetModifier               = false;

         double                                 mExternalGainModifier            = 1.0;
         double                                 mExternalOffsetModifier          = 0.0;
         bool                                   mUseExternalGainModifier         = false;
         bool                                   mUseExternalOffsetModifier       = false;

         double                                 mMinIndependentVar               = 0.0;
         double                                 mMaxIndependentVar               = 0.0;
         bool                                   mMaxMinIndependentVarInitialized = false;

         bool                                   mDirtyDataPresent                = false;

         std::string                            mDependentVarName;
         std::string                            mIndependentVarName;
         std::string                            mTableName;

   };
}
#endif // ! AERO_TABLE_2D_HPP
