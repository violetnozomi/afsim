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

#include "AeroTable3d.hpp"

#include <cmath>
#include <QJsonObject>
#include <sstream>
#include <stdexcept>

#include "AeroTable2d.hpp"
#include "UtLog.hpp"

namespace Designer
{

AeroTable3d::AeroTable3d(const std::string& aDependentVarName,
                         const std::string& aPrimaryIndependentVarName,
                         const std::string& aSecondaryIndependentVarName)
   : mPrimaryIndependentVarName(aPrimaryIndependentVarName)
   , mSecondaryIndependentVarName(aSecondaryIndependentVarName)
   , mDependentVarName(aDependentVarName)
   , mTableName(aDependentVarName + " vs " + aPrimaryIndependentVarName + " " + aSecondaryIndependentVarName)
{
}

AeroTable3d& AeroTable3d::operator=(const AeroTable3d& aRhs)
{
   if (this != &aRhs)
   {
      *this = aRhs;
   }
   return *this;
}

void AeroTable3d::AddTable(double aSecondaryIndependentValue)
{
   mBaseData.emplace_back(aSecondaryIndependentValue, AeroTable2d(mDependentVarName, mPrimaryIndependentVarName));

   if (!mMaxMinSecondaryIndependentVarInitialized)
   {
      mMaxMinSecondaryIndependentVarInitialized = true;
      mMinSecondaryIndependentVar = aSecondaryIndependentValue;
      mMaxSecondaryIndependentVar = aSecondaryIndependentValue;
   }
   else
   {
      if (aSecondaryIndependentValue < mMinSecondaryIndependentVar)
      {
         mMinSecondaryIndependentVar = aSecondaryIndependentValue;
      }

      if (aSecondaryIndependentValue > mMaxSecondaryIndependentVar)
      {
         mMaxSecondaryIndependentVar = aSecondaryIndependentValue;
      }
   }
}

void AeroTable3d::AddData(double aPrimaryIndependentValue, double aDependentVar)
{
   if (mBaseData.empty())
   {
      ut::log::error() << "mBaseData lacks a 2d table in AeroTable3d::AddData().";
      return;
   }

   auto& current2dTable = mBaseData.back();
   current2dTable.second.AddData(aPrimaryIndependentValue, aDependentVar);

   if (!mMaxMinPrimaryIndependentVarInitialized)
   {
      mMaxMinPrimaryIndependentVarInitialized = true;
      mMinPrimaryIndependentVar = aPrimaryIndependentValue;
      mMaxPrimaryIndependentVar = aPrimaryIndependentValue;
   }
   else
   {
      if (aPrimaryIndependentValue < mMinPrimaryIndependentVar)
      {
         mMinPrimaryIndependentVar = aPrimaryIndependentValue;
      }

      if (aPrimaryIndependentValue > mMaxPrimaryIndependentVar)
      {
         mMaxPrimaryIndependentVar = aPrimaryIndependentValue;
      }
   }
}

AeroTable2d* AeroTable3d::GetCurrent2dTable()
{
   return &(mBaseData.back().second);
}

bool AeroTable3d::GetPrimaryBaseData(const double&                           aSecondaryIndependentValue,
                                     std::vector<std::pair<double, double>>& aDataSet) const
{
   if (mBaseData.size() < 2)
   {
      // We need at least two data points to be valid
      return false;
   }

   // Check if the secondary point is below the minimum or above the maximum
   if ((aSecondaryIndependentValue < mMinSecondaryIndependentVar) || (aSecondaryIndependentValue > mMaxSecondaryIndependentVar))
   {
      return false;
   }

   // Find the lower and upper bounds for secondary
   const AeroTable2d* lowerBoundTable = nullptr;
   const AeroTable2d* upperBoundTable = nullptr;
   double lowerBoundValue = 0.0;
   double upperBoundValue = 0.0;
   for (const auto& item : mBaseData)
   {
      if (aSecondaryIndependentValue <= item.first)
      {
         // This is the upper bound
         upperBoundTable = &(item.second);
         upperBoundValue = item.first;
         break;
      }

      // Remember the last try
      lowerBoundTable = &(item.second);
      lowerBoundValue = item.first;
   }

   // Check for the special case of a null lower bound
   if (lowerBoundTable == nullptr)
   {
      // We only have a single bound, so get the 2d table
      aDataSet.clear();
      if (upperBoundTable && (upperBoundTable->GetBaseData(aDataSet)))
      {
         return true;
      }
      return false;
   }
   else
   {
      // This is the more typical condition, we have two valid bounds

      std::vector<std::pair<double, double>> lowerDataSet;
      if (!lowerBoundTable->GetBaseData(lowerDataSet))
      {
         return false;
      }

      std::vector<std::pair<double, double>> upperDataSet;
      if (!upperBoundTable || (!upperBoundTable->GetBaseData(upperDataSet)))
      {
         return false;
      }

      // Clear the data
      aDataSet.clear();

      // Interpolate between datasets
      double deltaValue = upperBoundValue - lowerBoundValue;
      double fraction = (aSecondaryIndependentValue - lowerBoundValue) / deltaValue;

      // Confirm both sets have the same number of elements
      size_t numLowerElements = lowerDataSet.size();
      size_t numUpperElements = upperDataSet.size();
      if (numLowerElements != numUpperElements)
      {
         auto outStream = ut::log::error() << "Number of elements mismatch in AeroTable3d::GetPrimaryBaseData().";
         outStream.AddNote() << "Table: " << mDependentVarName << " vs " << mPrimaryIndependentVarName << ", " << mSecondaryIndependentVarName;
         outStream.AddNote() << "numLowerElements: " << numLowerElements;
         outStream.AddNote() << "numUpperElements: " << numUpperElements;
         return false;
      }

      for (size_t index = 0; index < numLowerElements; ++index)
      {
         double independentVarValue = lowerDataSet.at(index).first;
         double lowerValue = lowerDataSet.at(index).second;
         double upperValue = upperDataSet.at(index).second;
         double delta = upperValue - lowerValue;
         double interpolatedValue = lowerValue + delta * fraction;
         aDataSet.emplace_back(independentVarValue, interpolatedValue);
      }
   }

   return true;
}

bool AeroTable3d::GetPrimaryBaseDataNearest(const double&                           aSecondaryIndependentValue,
                                            std::vector<std::pair<double, double>>& aDataSet) const
{
   if (mBaseData.size() < 2)
   {
      // We need at least two data points to be valid
      return false;
   }

   // Check if the secondary point is below the minimum or above the maximum
   if ((aSecondaryIndependentValue < mMinSecondaryIndependentVar) || (aSecondaryIndependentValue > mMaxSecondaryIndependentVar))
   {
      return false;
   }

   // Find the lower and upper bounds
   const AeroTable2d* lowerBoundTable = nullptr;
   const AeroTable2d* upperBoundTable = nullptr;
   double lowerBoundValue = 0.0;
   double upperBoundValue = 0.0;
   for (const auto& item : mBaseData)
   {
      if (aSecondaryIndependentValue <= item.first)
      {
         // This is the upper bound
         upperBoundTable = &(item.second);
         upperBoundValue = item.first;
         break;
      }

      // Remember the last try
      lowerBoundTable = &(item.second);
      lowerBoundValue = item.first;
   }

   // Check for the special case of a null lower bound
   if (lowerBoundTable == nullptr)
   {
      // We only have a single bound, so get the 2d table
      aDataSet.clear();
      if (upperBoundTable && (upperBoundTable->GetBaseData(aDataSet)))
      {
         return true;
      }
      return false;
   }
   else
   {
      // This is the more typical condition, we have two valid bounds

      /*
      std::vector<std::pair<double, double>> lowerDataSet;
      if (!lowerBoundTable->GetBaseData(lowerDataSet))
      {
         return false;
      }

      std::vector<std::pair<double, double>> upperDataSet;
      if (!upperBoundTable->GetBaseData(upperDataSet))
      {
         return false;
      }
      */

      // Clear the data
      aDataSet.clear();

      // Determine which data point is nearer
      double deltaLower = std::abs(aSecondaryIndependentValue - lowerBoundValue);
      double deltaUpper = std::abs(upperBoundValue - aSecondaryIndependentValue);

      if (deltaLower <= deltaUpper)
      {
         return lowerBoundTable->GetBaseData(aDataSet);
      }
      else
      {
         if (upperBoundTable)
         {
            return upperBoundTable->GetBaseData(aDataSet);
         }
         return false;
      }
   }

   return false;
}

bool AeroTable3d::GetPrimaryModifiedData(const double&                           aSecondaryIndependentValue,
                                         std::vector<std::pair<double, double>>& aDataSet) const
{
   if (mModifiedData.size() < 2)
   {
      auto outStream = ut::log::error() << "Need at least two data points to be valid in AeroTable3d::GetPrimaryModifiedData().";
      outStream.AddNote() << "Table: " << mDependentVarName << " vs " << mPrimaryIndependentVarName << ", " << mSecondaryIndependentVarName;
      // We need at least two data points to be valid
      return false;
   }

   // Check if the secondary point is below the minimum or above the maximum
   if ((aSecondaryIndependentValue < mMinSecondaryIndependentVar) || (aSecondaryIndependentValue > mMaxSecondaryIndependentVar))
   {
      if (aSecondaryIndependentValue < mMinSecondaryIndependentVar)
      {
         auto outStream = ut::log::error() << "aSecondaryIndependentValue is less than mMinSecondaryIndependentVar in AeroTable3d::GetPrimaryModifiedData().";
         outStream.AddNote() << "Table: " << mDependentVarName << " vs " << mPrimaryIndependentVarName << ", " << mSecondaryIndependentVarName;
         outStream.AddNote() << "aSecondaryIndependentValue: " << aSecondaryIndependentValue;
         outStream.AddNote() << "mMinSecondaryIndependentVar: " << mMinSecondaryIndependentVar;
      }
      if (aSecondaryIndependentValue > mMaxSecondaryIndependentVar)
      {
         auto outStream = ut::log::error() << "aSecondaryIndependentValue is greater than mMaxSecondaryIndependentVar in AeroTable3d::GetPrimaryModifiedData().";
         outStream.AddNote() << "Table: " << mDependentVarName << " vs " << mPrimaryIndependentVarName << ", " << mSecondaryIndependentVarName;
         outStream.AddNote() << "aSecondaryIndependentValue: " << aSecondaryIndependentValue;
         outStream.AddNote() << "mMaxSecondaryIndependentVar: " << mMaxSecondaryIndependentVar;
      }
      return false;
   }

   // Find the lower and upper bounds for secondary
   const AeroTable2d* lowerBoundTable = nullptr;
   const AeroTable2d* upperBoundTable = nullptr;
   double lowerBoundValue = 0.0;
   double upperBoundValue = 0.0;
   for (const auto& item : mModifiedData)
   {
      if (aSecondaryIndependentValue <= item.first)
      {
         // This is the upper bound
         upperBoundTable = &(item.second);
         upperBoundValue = item.first;
         break;
      }

      // Remember the last try
      lowerBoundTable = &(item.second);
      lowerBoundValue = item.first;
   }

   // Check for the special case of a null lower bound
   if (lowerBoundTable == nullptr)
   {
      // We only have a single bound, so get the 2d table
      aDataSet.clear();
      if (upperBoundTable->GetModifiedData(aDataSet))
      {
         return true;
      }

      auto outStream = ut::log::error() << "lowerBoundTable is null in AeroTable3d::GetPrimaryModifiedData().";
      outStream.AddNote() << "Table: " << mDependentVarName << " vs " << mPrimaryIndependentVarName << ", " << mSecondaryIndependentVarName;
      return false;
   }
   else
   {
      // This is the more typical condition, we have two valid bounds

      std::vector<std::pair<double, double>> lowerDataSet;
      if (!lowerBoundTable->GetModifiedData(lowerDataSet))
      {
         auto outStream = ut::log::error() << "Cannot get ModifiedData for the lower bound table in AeroTable3d::GetPrimaryModifiedData().";
         outStream.AddNote() << "Table: " << mDependentVarName << " vs " << mPrimaryIndependentVarName << ", " << mSecondaryIndependentVarName;
         return false;
      }

      std::vector<std::pair<double, double>> upperDataSet;
      if (!upperBoundTable || (!upperBoundTable->GetModifiedData(upperDataSet)))
      {
         auto outStream = ut::log::error() << "Cannot get ModifiedData for the upper bound table in AeroTable3d::GetPrimaryModifiedData().";
         outStream.AddNote() << "Table: " << mDependentVarName << " vs " << mPrimaryIndependentVarName << ", " << mSecondaryIndependentVarName;
         return false;
      }

      // Clear the data
      aDataSet.clear();

      // Interpolate between datasets
      double deltaValue = upperBoundValue - lowerBoundValue;
      double fraction = (aSecondaryIndependentValue - lowerBoundValue) / deltaValue;

      // Confirm both sets have the same number of elements
      size_t numLowerElements = lowerDataSet.size();
      size_t numUpperElements = upperDataSet.size();
      if (numLowerElements != numUpperElements)
      {
         auto outStream = ut::log::error() << "Number of elements mismatch in AeroTable3d::GetPrimaryModifiedData().";
         outStream.AddNote() << "Table: " << mDependentVarName << " vs " << mPrimaryIndependentVarName << ", " << mSecondaryIndependentVarName;
         outStream.AddNote() << "numLowerElements: " << numLowerElements;
         outStream.AddNote() << "numUpperElements: " << numUpperElements;
         return false;
      }

      for (size_t index = 0; index < numLowerElements; ++index)
      {
         double independentVarValue = lowerDataSet.at(index).first;
         double lowerValue = lowerDataSet.at(index).second;
         double upperValue = upperDataSet.at(index).second;
         double delta = upperValue - lowerValue;
         double interpolatedValue = lowerValue + delta * fraction;
         aDataSet.emplace_back(independentVarValue, interpolatedValue);
      }
   }

   return true;
}

bool AeroTable3d::GetSecondaryBaseData(const double&                           aPrimaryIndependentValue,
                                       std::vector<std::pair<double, double>>& aDataSet) const
{
   if (mBaseData.size() < 2)
   {
      // We need at least two data points to be valid
      return false;
   }

   // Check if the primary point is below the minimum or above the maximum
   if ((aPrimaryIndependentValue < mMinPrimaryIndependentVar) || (aPrimaryIndependentValue > mMaxPrimaryIndependentVar))
   {
      return false;
   }

   // Clear the data
   aDataSet.clear();

   for (auto& item : mBaseData)
   {
      double interpolatedValue = 0.0;
      bool valid = item.second.GetInterpolatedDependentValueBase(aPrimaryIndependentValue, interpolatedValue);
      if (!valid)
      {
         return false;
      }
      aDataSet.emplace_back(item.first, interpolatedValue);
   }

   return true;
}

bool AeroTable3d::GetSecondaryModifiedData(const double&                           aPrimaryIndependentValue,
                                           std::vector<std::pair<double, double>>& aDataSet) const
{
   if (mModifiedData.size() < 2)
   {
      // We need at least two data points to be valid
      return false;
   }

   // Check if the primary point is below the minimum or above the maximum
   if ((aPrimaryIndependentValue < mMinPrimaryIndependentVar) || (aPrimaryIndependentValue > mMaxPrimaryIndependentVar))
   {
      return false;
   }

   // Clear the data
   aDataSet.clear();

   for (auto& item : mModifiedData)
   {
      double interpolatedValue = 0.0;
      bool valid = item.second.GetInterpolatedDependentValueModified(aPrimaryIndependentValue, interpolatedValue);
      if (!valid)
      {
         return false;
      }
      aDataSet.emplace_back(item.first, interpolatedValue);
   }

   return true;
}

bool AeroTable3d::GetInterpolatedDependentValueBase(const double& aPrimaryIndependentValue, const double& aSecondaryIndependentValue, double& aValue) const
{
   if (mBaseData.size() < 2)
   {
      // We need at least two data points to be valid
      ut::log::error() << "mBaseData needs at least two data points in AeroTable3d::GetInterpolatedDependentValueBase().";
      return false;
   }

   // Check if the primary point is below the minimum or above the maximum
   if ((aPrimaryIndependentValue < mMinPrimaryIndependentVar) || (aPrimaryIndependentValue > mMaxPrimaryIndependentVar))
   {
      ut::log::error() << "primary point is below the minimum or above the maximum in "
         << "AeroTable3d::GetInterpolatedDependentValueBase().";
      return false;
   }

   // Check if the secondary point is below the minimum or above the maximum
   if ((aSecondaryIndependentValue < mMinSecondaryIndependentVar) || (aSecondaryIndependentValue > mMaxSecondaryIndependentVar))
   {
      ut::log::error() << "secondary point is below the minimum or above the maximum in "
         << "AeroTable3d::GetInterpolatedDependentValueBase().";
      return false;
   }

   // At this point, we have valid primary and secondary data points

   // Find the lower and upper bounds for secondary
   const AeroTable2d* lowerBoundTable = nullptr;
   const AeroTable2d* upperBoundTable = nullptr;
   double lowerBoundValue = 0.0;
   double upperBoundValue = 0.0;
   for (const auto& item : mBaseData)
   {
      if (aSecondaryIndependentValue <= item.first)
      {
         // This is the upper bound
         upperBoundTable = &(item.second);
         upperBoundValue = item.first;
         break;
      }

      // Remember the last try
      lowerBoundTable = &(item.second);
      lowerBoundValue = item.first;
   }

   // Check for the special case of a null lower bound
   if (lowerBoundTable == nullptr)
   {
      // We only have a single bound, so get the 2d table
      double value = 0.0;
      if (upperBoundTable && (upperBoundTable->GetInterpolatedDependentValueBase(aPrimaryIndependentValue, value)))
      {
         aValue = value;
         return true;
      }
      else
      {
         return false;
      }
   }
   // Check for the special case of a null upper bound
   else if (upperBoundTable == nullptr)
   {
      // We only have a single bound, so get the 2d table
      double value = 0.0;
      if (lowerBoundTable->GetInterpolatedDependentValueBase(aPrimaryIndependentValue, value))
      {
         aValue = value;
         return true;
      }
      else
      {
         return false;
      }
   }
   else
   {
      // This is the more typical condition, we have two valid bounds

      double deltaValue = upperBoundValue - lowerBoundValue;
      double fraction = (aSecondaryIndependentValue - lowerBoundValue) / deltaValue;

      double lowerValue = 0.0;
      if (!lowerBoundTable->GetInterpolatedDependentValueBase(aPrimaryIndependentValue, lowerValue))
      {
         return false;
      }

      double upperValue = 0.0;
      if (!upperBoundTable->GetInterpolatedDependentValueBase(aPrimaryIndependentValue, upperValue))
      {
         return false;
      }

      double delta2dValue = upperValue - lowerValue;

      aValue = lowerValue + delta2dValue * fraction;
   }

   return true;
}

bool AeroTable3d::GetInterpolatedDependentValueModified(const double& aPrimaryIndependentValue, const double& aSecondaryIndependentValue, double& aValue) const
{
   if (mModifiedData.size() < 2)
   {
      // We need at least two data points to be valid
      return false;
   }

   // Check if the primary point is below the minimum or above the maximum
   if ((aPrimaryIndependentValue < mMinPrimaryIndependentVar) || (aPrimaryIndependentValue > mMaxPrimaryIndependentVar))
   {
      return false;
   }

   // Check if the secondary point is below the minimum or above the maximum
   if ((aSecondaryIndependentValue < mMinSecondaryIndependentVar) || (aSecondaryIndependentValue > mMaxSecondaryIndependentVar))
   {
      return false;
   }

   // At this point, we have valid primary and secondary data points

   // Find the lower and upper bounds for secondary
   const AeroTable2d* lowerBoundTable = nullptr;
   const AeroTable2d* upperBoundTable = nullptr;
   double lowerBoundValue = 0.0;
   double upperBoundValue = 0.0;
   for (const auto& item : mModifiedData)
   {
      if (aSecondaryIndependentValue <= item.first)
      {
         // This is the upper bound
         upperBoundTable = &(item.second);
         upperBoundValue = item.first;
         break;
      }

      // Remember the last try
      lowerBoundTable = &(item.second);
      lowerBoundValue = item.first;
   }

   // Check for the special case of a null lower bound
   if (lowerBoundTable == nullptr)
   {
      // We only have a single bound, so get the 2d table
      double value = 0.0;
      if (upperBoundTable && (upperBoundTable->GetInterpolatedDependentValueModified(aPrimaryIndependentValue, value)))
      {
         aValue = value;
         return true;
      }
      else
      {
         return false;
      }
   }
   else
   {
      // This is the more typical condition, we have two valid bounds

      double deltaValue = upperBoundValue - lowerBoundValue;
      double fraction = (aSecondaryIndependentValue - lowerBoundValue) / deltaValue;

      double lowerValue = 0.0;
      if (!lowerBoundTable->GetInterpolatedDependentValueModified(aPrimaryIndependentValue, lowerValue))
      {
         return false;
      }

      double upperValue = 0.0;
      if (!upperBoundTable || (!upperBoundTable->GetInterpolatedDependentValueModified(aPrimaryIndependentValue, upperValue)))
      {
         return false;
      }

      double delta2dValue = upperValue - lowerValue;

      aValue = lowerValue + delta2dValue * fraction;
   }

   return true;
}

void AeroTable3d::ClearAllData()
{
   mBaseData.clear();
   mModifiedData.clear();
   mPrimaryGainModifierList.clear();
   mSinglePrimaryGainModifier = 1.0;
   mUseSinglePrimaryGainModifier = true;
   mUsePrimaryGainModifier = false;
   mPrimaryOffsetModifierList.clear();
   mSinglePrimaryOffsetModifier = 0.0;
   mUseSinglePrimaryOffsetModifier = true;
   mUsePrimaryOffsetModifier = false;
   mSecondaryGainModifierList.clear();
   mSingleSecondaryGainModifier = 1.0;
   mUseSingleSecondaryGainModifier = true;
   mUseSecondaryGainModifier = false;
   mSecondaryOffsetModifierList.clear();
   mSingleSecondaryOffsetModifier = 0.0;
   mUseSingleSecondaryOffsetModifier = true;
   mUseSecondaryOffsetModifier = false;
   mMinPrimaryIndependentVar = 0.0;
   mMaxPrimaryIndependentVar = 0.0;
   mMaxMinPrimaryIndependentVarInitialized = false;
   mMinSecondaryIndependentVar = 0.0;
   mMaxSecondaryIndependentVar = 0.0;
   mMaxMinSecondaryIndependentVarInitialized = false;
   mExternalGainModifier = 1.0;
   mExternalOffsetModifier = 0.0;
   mUseExternalGainModifier = false;
   mUseExternalOffsetModifier = false;
   mDirtyDataPresent = false;
}

void AeroTable3d::UpdateModifiedData()
{
   // First, copy the current base data to the modified data
   CopyBaseDataToModifiedData();

   // If there is no dirty data, we're done
   if (!mDirtyDataPresent) { return; }

   // Clear the flag
   mDirtyDataPresent = false;

   // Ensure that the 2d tables know about modifiers and update their data
   for (auto& table : mModifiedData)
   {
      if (mUsePrimaryGainModifier)
      {
         if (mUseSinglePrimaryGainModifier)
         {
            table.second.SetSingleGainModifier(mSinglePrimaryGainModifier);
         }
         else
         {
            table.second.SetListBasedGainModifier(mPrimaryGainModifierList);
         }
      }
      else
      {
         table.second.ClearGainModifier();
      }

      if (mUsePrimaryOffsetModifier)
      {
         if (mUseSinglePrimaryOffsetModifier)
         {
            table.second.SetSingleOffsetModifier(mSinglePrimaryOffsetModifier);
         }
         else
         {
            table.second.SetListBasedOffsetModifier(mPrimaryOffsetModifierList);
         }
      }
      else
      {
         table.second.ClearOffsetModifier();
      }

      // Determine secondary modifiers
      double gainValue = 1.0;
      double offsetValue = 0.0;

      if (mUseSecondaryGainModifier)
      {
         if (mUseSingleSecondaryGainModifier)
         {
            gainValue = mSingleSecondaryGainModifier;
         }
         else
         {
            double value = 0.0;
            if (Interpolate(mSecondaryGainModifierList, table.first, value))
            {
               gainValue = value;
            }
         }
      }

      if (mUseSecondaryOffsetModifier)
      {
         if (mUseSingleSecondaryOffsetModifier)
         {
            offsetValue = mSingleSecondaryOffsetModifier;
         }
         else
         {
            double value = 0.0;
            if (Interpolate(mSecondaryOffsetModifierList, table.first, value))
            {
               offsetValue = value;
            }
         }
      }

      // Be sure to include external modifications

      if (mUseExternalGainModifier)
      {
         gainValue *= mExternalGainModifier;
      }

      if (mUseExternalOffsetModifier)
      {
         offsetValue += mExternalOffsetModifier;
      }

      // Apply the external data, if appropriate

      if (std::abs(gainValue - 1.0) > mEpsilon)
      {
         table.second.SetExternalGainModifier(gainValue);
      }

      if (std::abs(offsetValue) > mEpsilon)
      {
         table.second.SetExternalOffsetModifier(offsetValue);
      }

      // Update the data
      table.second.UpdateModifiedData();
   }
}

void AeroTable3d::SetListBasedPrimaryGainModifier(const std::vector<std::pair<double, double>>& aModifierList)
{
   mDirtyDataPresent             = true;
   mPrimaryGainModifierList      = aModifierList;
   mUseSinglePrimaryGainModifier = false;
   mUsePrimaryGainModifier       = true;
}

void AeroTable3d::SetSinglePrimaryGainModifier(const double& aSingleModifier)
{
   mDirtyDataPresent             = true;
   mSinglePrimaryGainModifier    = aSingleModifier;
   mUseSinglePrimaryGainModifier = true;
   mUsePrimaryGainModifier       = true;
}

void AeroTable3d::ClearPrimaryGainModifier()
{
   mUsePrimaryGainModifier = false;
}

void AeroTable3d::SetListBasedPrimaryOffsetModifier(const std::vector<std::pair<double, double>>& aModifierList)
{
   mDirtyDataPresent               = true;
   mPrimaryOffsetModifierList      = aModifierList;
   mUseSinglePrimaryOffsetModifier = false;
   mUsePrimaryOffsetModifier       = true;
}

void AeroTable3d::SetSinglePrimaryOffsetModifier(const double& aSingleModifier)
{
   mDirtyDataPresent               = true;
   mSinglePrimaryOffsetModifier    = aSingleModifier;
   mUseSinglePrimaryOffsetModifier = true;
   mUsePrimaryOffsetModifier       = true;
}

void AeroTable3d::ClearPrimaryOffsetModifier()
{
   mUsePrimaryOffsetModifier = false;
}

void AeroTable3d::SetListBasedSecondaryGainModifier(const std::vector<std::pair<double, double>>& aModifierList)
{
   mDirtyDataPresent               = true;
   mSecondaryGainModifierList      = aModifierList;
   mUseSingleSecondaryGainModifier = false;
   mUseSecondaryGainModifier       = true;
}

void AeroTable3d::SetSingleSecondaryGainModifier(const double& aSingleModifier)
{
   mDirtyDataPresent               = true;
   mSingleSecondaryGainModifier    = aSingleModifier;
   mUseSingleSecondaryGainModifier = true;
   mUseSecondaryGainModifier       = true;
}

void AeroTable3d::ClearSecondaryGainModifier()
{
   mUseSecondaryGainModifier = false;
}

void AeroTable3d::SetListBasedSecondaryOffsetModifier(const std::vector<std::pair<double, double>>& aModifierList)
{
   mDirtyDataPresent                 = true;
   mSecondaryOffsetModifierList      = aModifierList;
   mUseSingleSecondaryOffsetModifier = false;
   mUseSecondaryOffsetModifier       = true;
}

void AeroTable3d::SetSingleSecondaryOffsetModifier(const double& aSingleModifier)
{
   mDirtyDataPresent                 = true;
   mSingleSecondaryOffsetModifier    = aSingleModifier;
   mUseSingleSecondaryOffsetModifier = true;
   mUseSecondaryOffsetModifier       = true;
}

void AeroTable3d::ClearSecondaryOffsetModifier()
{
   mUseSecondaryOffsetModifier = false;
}

void AeroTable3d::SetExternalGainModifier(const double& aModifier)
{
   mDirtyDataPresent        = true;
   mExternalGainModifier    = aModifier;
   mUseExternalGainModifier = true;
}

void AeroTable3d::ClearExternalGainModifier()
{
   mUseExternalGainModifier = false;
}

void AeroTable3d::SetExternalOffsetModifier(const double& aModifier)
{
   mDirtyDataPresent          = true;
   mExternalOffsetModifier    = aModifier;
   mUseExternalOffsetModifier = true;
}

void AeroTable3d::ClearExternalOffsetModifier()
{
   mUseExternalOffsetModifier = false;
}

bool AeroTable3d::MaxMinPrimaryIndependentVariableRange(double& aMinIndependentVar, double& aMaxIndependentVar)
{
   aMinIndependentVar = mMinPrimaryIndependentVar;
   aMaxIndependentVar = mMaxPrimaryIndependentVar;
   return mMaxMinPrimaryIndependentVarInitialized;
}

bool AeroTable3d::MaxMinSecondaryIndependentVariableRange(double& aMinIndependentVar, double& aMaxIndependentVar)
{
   aMinIndependentVar = mMinSecondaryIndependentVar;
   aMaxIndependentVar = mMaxSecondaryIndependentVar;
   return mMaxMinSecondaryIndependentVarInitialized;
}

void AeroTable3d::CopyBaseDataToModifiedData()
{
   mModifiedData.clear();

   for (auto& data : mBaseData)
   {
      data.second.CopyBaseDataToModifiedData();
      std::pair<double, AeroTable2d> tempPair(data.first, data.second);
      mModifiedData.push_back(tempPair);
   }
}

const std::vector<double> AeroTable3d::GetSecondaryIndependentValues()
{
   std::vector<double> secondaryIndependentValues;

   // Copy the independent variable values (first item in the pairs in mBaseData)
   std::transform(mBaseData.begin(), mBaseData.end(), std::back_inserter(secondaryIndependentValues), 
                  [](const std::pair<double, AeroTable2d>& v) {return v.first; });

   return secondaryIndependentValues;
}

const std::vector<double> AeroTable3d::GetPrimaryIndependentValues()
{
   std::vector<double> primaryIndependentValues;

   if (mBaseData.size() != 0)
   {
      AeroTable2d curValue = mBaseData[0].second;
      std::vector<std::pair<double, double>> primaryVars;
      curValue.GetBaseData(primaryVars);

      // Copy the independent variable values (first item in the pairs in primaryVars)
      std::transform(primaryVars.begin(), primaryVars.end(), std::back_inserter(primaryIndependentValues),
                     [](const std::pair<double, double>& v) {return v.first; });

   }
   return primaryIndependentValues;
}


void AeroTable3d::RevertPlot()
{
   mPrimaryGainModifierList.clear();
   mSinglePrimaryGainModifier = 1.0;
   mUseSinglePrimaryGainModifier = true;
   mUsePrimaryGainModifier = false;

   mPrimaryOffsetModifierList.clear();
   mSinglePrimaryOffsetModifier = 0.0;
   mUseSinglePrimaryOffsetModifier = true;
   mUsePrimaryOffsetModifier = false;

   mSecondaryGainModifierList.clear();
   mSingleSecondaryGainModifier = 1.0;
   mUseSingleSecondaryGainModifier = true;
   mUseSecondaryGainModifier = false;

   mSecondaryOffsetModifierList.clear();
   mSingleSecondaryOffsetModifier = 0.0;
   mUseSingleSecondaryOffsetModifier = true;
   mUseSecondaryOffsetModifier = false;

   mExternalGainModifier = 1.0;
   mExternalOffsetModifier = 0.0;
   mUseExternalGainModifier = false;
   mUseExternalOffsetModifier = false;

   UpdateModifiedData();
}

bool AeroTable3d::HasIndependentVarName(const std::string& aVarName) const
{
   return aVarName == GetPrimaryIndependentVarName() || aVarName == GetSecondaryIndependentVarName();
}

void AeroTable3d::SetListBasedGainModifier(const std::string& aVarName, const std::vector<std::pair<double, double>>& aModifierLine)
{
   if (aVarName == GetPrimaryIndependentVarName())
   {
      SetListBasedPrimaryGainModifier(aModifierLine);
   }
   else if (aVarName == GetSecondaryIndependentVarName())
   {
      SetListBasedSecondaryGainModifier(aModifierLine);
   }
   else
   {
      std::stringstream ss{};
      ss << "ERROR: table '" << GetTableName() << "' has no independent variable '" << aVarName << "'. Unable to set list-based gain modifier.";
      throw std::runtime_error(ss.str());
   }
}

void AeroTable3d::SetSingleGainModifier(const std::string& aVarName, double aModifierValue)
{
   if (aVarName == GetPrimaryIndependentVarName())
   {
      SetSinglePrimaryGainModifier(aModifierValue);
   }
   else if (aVarName == GetSecondaryIndependentVarName())
   {
      SetSingleSecondaryGainModifier(aModifierValue);
   }
   else
   {
      std::stringstream ss{};
      ss << "ERROR: table '" << GetTableName() << "' has no independent variable '" << aVarName << "'. Unable to set single gain value.";
      throw std::runtime_error(ss.str());
   }
}

void AeroTable3d::SetListBasedOffsetModifier(const std::string& aVarName, const std::vector<std::pair<double, double>>& aModifierLine)
{
   if (aVarName == GetPrimaryIndependentVarName())
   {
      SetListBasedPrimaryOffsetModifier(aModifierLine);
   }
   else if (aVarName == GetSecondaryIndependentVarName())
   {
      SetListBasedSecondaryOffsetModifier(aModifierLine);
   }
   else
   {
      std::stringstream ss{};
      ss << "ERROR: table '" << GetTableName() << "' has no independent variable '" << aVarName << "'. Unable to set list-based offset modifier.";
      throw std::runtime_error(ss.str());
   }
}

void AeroTable3d::SetSingleOffsetModifier(const std::string& aVarName, double aModifierValue)
{
   if (aVarName == GetPrimaryIndependentVarName())
   {
      SetSinglePrimaryOffsetModifier(aModifierValue);
   }
   else if (aVarName == GetSecondaryIndependentVarName())
   {
      SetSingleSecondaryOffsetModifier(aModifierValue);
   }
   else
   {
      std::stringstream ss{};
      ss << "ERROR: table '" << GetTableName() << "' has no independent variable '" << aVarName << "'. Unable to set single offset value.";
      throw std::runtime_error(ss.str());
   }
}

} // namespace Designer
