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

#include "AeroTable4d.hpp"

#include <cmath>
#include <sstream>
#include <stdexcept>

#include "AeroTable2d.hpp"
#include "AeroTable3d.hpp"
#include "UtLog.hpp"

namespace Designer
{

AeroTable4d::AeroTable4d(const std::string& aDependentVarName,
                         const std::string& aPrimaryIndependentVarName,
                         const std::string& aSecondaryIndependentVarName,
                         const std::string& aTertiaryIndependentVarName)
   : mDependentVarName(aDependentVarName)
   , mPrimaryIndependentVarName(aPrimaryIndependentVarName)
   , mSecondaryIndependentVarName(aSecondaryIndependentVarName)
   , mTertiaryIndependentVarName(aTertiaryIndependentVarName)
   , mTableName(aDependentVarName + " vs " + aPrimaryIndependentVarName + " " + aSecondaryIndependentVarName + " " + aTertiaryIndependentVarName)
{
}

AeroTable4d& AeroTable4d::operator=(const AeroTable4d& aRhs)
{
   if (this != &aRhs)
   {
      *this = aRhs;
   }
   return *this;
}

void AeroTable4d::AddSecondaryTable(double aTertiaryIndependentValue)
{
   mBaseData.emplace_back(aTertiaryIndependentValue, AeroTable3d(mDependentVarName, mPrimaryIndependentVarName, mSecondaryIndependentVarName));

   if (!mMaxMinTertiaryIndependentVarInitialized)
   {
      mMaxMinTertiaryIndependentVarInitialized = true;
      mMinTertiaryIndependentVar = aTertiaryIndependentValue;
      mMaxTertiaryIndependentVar = aTertiaryIndependentValue;
   }
   else
   {
      if (aTertiaryIndependentValue < mMinTertiaryIndependentVar)
      {
         mMinTertiaryIndependentVar = aTertiaryIndependentValue;
      }

      if (aTertiaryIndependentValue > mMaxTertiaryIndependentVar)
      {
         mMaxTertiaryIndependentVar = aTertiaryIndependentValue;
      }
   }
}

void AeroTable4d::AddPrimaryTable(double aSecondaryIndependentValue)
{
   if (mBaseData.empty())
   {
      ut::log::error() << "mBaseData lacks a 3d table in AeroTable4d::AddData().";
      return;
   }

   auto& current3dTable = mBaseData.back();
   current3dTable.second.AddTable(aSecondaryIndependentValue);

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

void AeroTable4d::AddData(double aPrimaryIndependentValue, double aDependentVar)
{
   if (mBaseData.empty())
   {
      ut::log::error() << "mBaseData lacks a 3d table in AeroTable4d::AddData().";
      return;
   }

   auto& current3dTable = mBaseData.back();
   current3dTable.second.AddData(aPrimaryIndependentValue, aDependentVar);

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

AeroTable2d* AeroTable4d::GetCurrent2dTable()
{
   AeroTable3d* current3dTable = &(mBaseData.back().second);

   return current3dTable->GetCurrent2dTable();
}

bool AeroTable4d::GetPrimaryBaseData(const double&                           aSecondaryIndependentValue,
                                     const double&                           aTertiaryIndependentValue,
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

   // Check if the tertiary point is below the minimum or above the maximum
   if ((aTertiaryIndependentValue < mMinTertiaryIndependentVar) || (aTertiaryIndependentValue > mMaxTertiaryIndependentVar))
   {
      return false;
   }

   // At this point, we have valid primary, secondary, and tertiary data points

   // Find the lower and upper bounds
   const AeroTable3d* lowerBoundTable = nullptr;
   const AeroTable3d* upperBoundTable = nullptr;
   double lowerBoundValue = 0.0;
   double upperBoundValue = 0.0;
   for (const auto& item : mBaseData)
   {
      if (aTertiaryIndependentValue <= item.first)
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
      // We only have a single bound, so get the 3d table
      if (upperBoundTable && (upperBoundTable->GetPrimaryBaseData(aSecondaryIndependentValue, aDataSet)))
      {
         return true;
      }
      return false;
   }
   else
   {
      // This is the more typical condition, we have two valid bounds

      std::vector<std::pair<double, double>> lowerDataSet;
      if (!lowerBoundTable->GetPrimaryBaseData(aSecondaryIndependentValue, lowerDataSet))
      {
         return false;
      }

      std::vector<std::pair<double, double>> upperDataSet;
      if (!upperBoundTable || (!upperBoundTable->GetPrimaryBaseData(aSecondaryIndependentValue, upperDataSet)))
      {
         return false;
      }

      // Clear the data
      aDataSet.clear();

      // Interpolate between datasets
      double deltaValue = upperBoundValue - lowerBoundValue;
      double fraction = (aTertiaryIndependentValue - lowerBoundValue) / deltaValue;

      // Confirm both sets have the same number of elements
      size_t numLowerElements = lowerDataSet.size();
      size_t numUpperElements = upperDataSet.size();
      if (numLowerElements != numUpperElements)
      {
         auto outStream = ut::log::error() << "Number of elements mismatch in AeroTable4d::GetPrimaryBaseData().";
         outStream.AddNote() << "Table: " << mDependentVarName << " vs " << mPrimaryIndependentVarName << ", " << mSecondaryIndependentVarName << ", " << mTertiaryIndependentVarName;
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

bool AeroTable4d::GetPrimaryModifiedData(const double&                           aSecondaryIndependentValue,
                                         const double&                           aTertiaryIndependentValue,
                                         std::vector<std::pair<double, double>>& aDataSet) const
{
   if (mModifiedData.size() < 2)
   {
      auto outStream = ut::log::error() << "Need at least two data points to be valid in AeroTable4d::GetPrimaryModifiedData().";
      outStream.AddNote() << "Table: " << mDependentVarName << " vs " << mPrimaryIndependentVarName << ", " << mSecondaryIndependentVarName << ", " << mTertiaryIndependentVarName;
      // We need at least two data points to be valid
      return false;
   }

   // Check if the secondary point is below the minimum or above the maximum
   if ((aSecondaryIndependentValue < mMinSecondaryIndependentVar) || (aSecondaryIndependentValue > mMaxSecondaryIndependentVar))
   {
      if (aSecondaryIndependentValue < mMinSecondaryIndependentVar)
      {
         auto outStream = ut::log::error() << "aSecondaryIndependentValue is less than mMinSecondaryIndependentVar in AeroTable4d::GetPrimaryModifiedData().";
         outStream.AddNote() << "Table: " << mDependentVarName << " vs " << mPrimaryIndependentVarName << ", " << mSecondaryIndependentVarName << ", " << mTertiaryIndependentVarName;
         outStream.AddNote() << "aSecondaryIndependentValue: " << aSecondaryIndependentValue;
         outStream.AddNote() << "mMinSecondaryIndependentVar: " << mMinSecondaryIndependentVar;
      }
      if (aSecondaryIndependentValue > mMaxSecondaryIndependentVar)
      {
         auto outStream = ut::log::error() << "aSecondaryIndependentValue is greater than mMaxSecondaryIndependentVar in AeroTable4d::GetPrimaryModifiedData().";
         outStream.AddNote() << "Table: " << mDependentVarName << " vs " << mPrimaryIndependentVarName << ", " << mSecondaryIndependentVarName << ", " << mTertiaryIndependentVarName;
         outStream.AddNote() << "aSecondaryIndependentValue: " << aSecondaryIndependentValue;
         outStream.AddNote() << "mMaxSecondaryIndependentVar: " << mMaxSecondaryIndependentVar;
      }
      return false;
   }

   // Check if the tertiary point is below the minimum or above the maximum
   if ((aTertiaryIndependentValue < mMinTertiaryIndependentVar) || (aTertiaryIndependentValue > mMaxTertiaryIndependentVar))
   {
      if (aTertiaryIndependentValue < mMinTertiaryIndependentVar)
      {
         auto outStream = ut::log::error() << "aTertiaryIndependentValue is less than mMinTertiaryIndependentVar in AeroTable4d::GetPrimaryModifiedData().";
         outStream.AddNote() << "Table: " << mDependentVarName << " vs " << mPrimaryIndependentVarName << ", " << mSecondaryIndependentVarName << ", " << mTertiaryIndependentVarName;
         outStream.AddNote() << "aTertiaryIndependentValue: " << aTertiaryIndependentValue;
         outStream.AddNote() << "mMinTertiaryIndependentVar: " << mMinTertiaryIndependentVar;
      }
      if (aTertiaryIndependentValue > mMaxTertiaryIndependentVar)
      {
         auto outStream = ut::log::error() << "aTertiaryIndependentValue is greater than mMaxTertiaryIndependentVar in AeroTable4d::GetPrimaryModifiedData().";
         outStream.AddNote() << "Table: " << mDependentVarName << " vs " << mPrimaryIndependentVarName << ", " << mSecondaryIndependentVarName << ", " << mTertiaryIndependentVarName;
         outStream.AddNote() << "aTertiaryIndependentValue: " << aTertiaryIndependentValue;
         outStream.AddNote() << "mMaxTertiaryIndependentVar: " << mMaxTertiaryIndependentVar;
      }
      return false;
   }

   // At this point, we have valid primary, secondary, and tertiary data points

   // Find the lower and upper bounds
   const AeroTable3d* lowerBoundTable = nullptr;
   const AeroTable3d* upperBoundTable = nullptr;
   double lowerBoundValue = 0.0;
   double upperBoundValue = 0.0;
   for (const auto& item : mModifiedData)
   {
      if (aTertiaryIndependentValue <= item.first)
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
      // We only have a single bound, so get the 3d table
      if (upperBoundTable && (upperBoundTable->GetPrimaryModifiedData(aSecondaryIndependentValue, aDataSet)))
      {
         return true;
      }

      auto outStream = ut::log::error() << "lowerBoundTable is null in AeroTable4d::GetPrimaryModifiedData().";
      outStream.AddNote() << "Table: " << mDependentVarName << " vs " << mPrimaryIndependentVarName << ", " << mSecondaryIndependentVarName << ", " << mTertiaryIndependentVarName;
      return false;
   }
   else
   {
      // This is the more typical condition, we have two valid bounds

      std::vector<std::pair<double, double>> lowerDataSet;
      if (!lowerBoundTable->GetPrimaryModifiedData(aSecondaryIndependentValue, lowerDataSet))
      {
         auto outStream = ut::log::error() << "Cannot get PrimaryModifiedData for the lower bound table in AeroTable4d::GetPrimaryModifiedData().";
         outStream.AddNote() << "Table: " << mDependentVarName << " vs " << mPrimaryIndependentVarName << ", " << mSecondaryIndependentVarName << ", " << mTertiaryIndependentVarName;
         return false;
      }

      std::vector<std::pair<double, double>> upperDataSet;
      if (!upperBoundTable || (!upperBoundTable->GetPrimaryModifiedData(aSecondaryIndependentValue, upperDataSet)))
      {
         auto outStream = ut::log::error() << "Cannot get PrimaryModifiedData for the upper bound table in AeroTable4d::GetPrimaryModifiedData().";
         outStream.AddNote() << "Table: " << mDependentVarName << " vs " << mPrimaryIndependentVarName << ", " << mSecondaryIndependentVarName << ", " << mTertiaryIndependentVarName;
         return false;
      }

      // Clear the data
      aDataSet.clear();

      // Interpolate between datasets
      double deltaValue = upperBoundValue - lowerBoundValue;
      double fraction = (aTertiaryIndependentValue - lowerBoundValue) / deltaValue;

      // Confirm both sets have the same number of elements
      size_t numLowerElements = lowerDataSet.size();
      size_t numUpperElements = upperDataSet.size();
      if (numLowerElements != numUpperElements)
      {
         auto outStream = ut::log::error() << "Number of elements mismatch in AeroTable4d::GetPrimaryBaseData().";
         outStream.AddNote() << "Table: " << mDependentVarName << " vs " << mPrimaryIndependentVarName << ", " << mSecondaryIndependentVarName << ", " << mTertiaryIndependentVarName;
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

bool AeroTable4d::GetPrimaryBaseDataNearest(const double&                           aSecondaryIndependentValue,
                                            const double&                           aTertiaryIndependentValue,
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

   // Check if the tertiary point is below the minimum or above the maximum
   if ((aTertiaryIndependentValue < mMinTertiaryIndependentVar) || (aTertiaryIndependentValue > mMaxTertiaryIndependentVar))
   {
      return false;
   }

   // At this point, we have valid primary, secondary, and tertiary data points

   // Find the lower and upper bounds
   const AeroTable3d* lowerBoundTable = nullptr;
   const AeroTable3d* upperBoundTable = nullptr;
   double lowerBoundValue = 0.0;
   double upperBoundValue = 0.0;
   for (const auto& item : mBaseData)
   {
      if (aTertiaryIndependentValue <= item.first)
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
      // We only have a single bound, so get the 3d table
      if (upperBoundTable && (upperBoundTable->GetPrimaryBaseDataNearest(aSecondaryIndependentValue, aDataSet)))
      {
         return true;
      }
      return false;
   }
   else
   {
      // This is the more typical condition, we have two valid bounds

      // Clear the data
      aDataSet.clear();

      // Determine which data point is nearer
      double deltaLower = std::abs(aTertiaryIndependentValue - lowerBoundValue);
      double deltaUpper = std::abs(upperBoundValue - aTertiaryIndependentValue);

      if (deltaLower <= deltaUpper)
      {
         return lowerBoundTable->GetPrimaryBaseDataNearest(aSecondaryIndependentValue, aDataSet);
      }
      else
      {
         if (upperBoundTable)
         {
            return upperBoundTable->GetPrimaryBaseDataNearest(aSecondaryIndependentValue, aDataSet);
         }
         return false;
      }
   }

   return false;
}

bool AeroTable4d::GetSecondaryBaseData(const double&                           aPrimaryIndependentValue,
                                       const double&                           aTertiaryIndependentValue,
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

   // Check if the tertiary point is below the minimum or above the maximum
   if ((aTertiaryIndependentValue < mMinTertiaryIndependentVar) || (aTertiaryIndependentValue > mMaxTertiaryIndependentVar))
   {
      return false;
   }

   // At this point, we have valid primary, secondary, and tertiary data points

   // Find the lower and upper bounds
   const AeroTable3d* lowerBoundTable = nullptr;
   const AeroTable3d* upperBoundTable = nullptr;
   double lowerBoundValue = 0.0;
   double upperBoundValue = 0.0;
   for (const auto& item : mBaseData)
   {
      if (aTertiaryIndependentValue <= item.first)
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
      // We only have a single bound, so get the 3d table
      if (upperBoundTable && (upperBoundTable->GetSecondaryBaseData(aPrimaryIndependentValue, aDataSet)))
      {
         return true;
      }
      return false;
   }
   else
   {
      // This is the more typical condition, we have two valid bounds

      std::vector<std::pair<double, double>> lowerDataSet;
      if (!lowerBoundTable->GetSecondaryBaseData(aPrimaryIndependentValue, lowerDataSet))
      {
         return false;
      }

      std::vector<std::pair<double, double>> upperDataSet;
      if (!upperBoundTable || (!upperBoundTable->GetSecondaryBaseData(aPrimaryIndependentValue, upperDataSet)))
      {
         return false;
      }

      // Clear the data
      aDataSet.clear();

      // Interpolate between datasets
      double deltaValue = upperBoundValue - lowerBoundValue;
      double fraction = (aTertiaryIndependentValue - lowerBoundValue) / deltaValue;

      // Confirm both sets have the same number of elements
      size_t numLowerElements = lowerDataSet.size();
      size_t numUpperElements = upperDataSet.size();
      if (numLowerElements != numUpperElements)
      {
         auto outStream = ut::log::error() << "Number of elements mismatch in AeroTable4d::GetPrimaryBaseData().";
         outStream.AddNote() << "Table: " << mDependentVarName << " vs " << mPrimaryIndependentVarName << ", " << mSecondaryIndependentVarName << ", " << mTertiaryIndependentVarName;
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

bool AeroTable4d::GetSecondaryModifiedData(const double&                           aPrimaryIndependentValue,
                                           const double&                           aTertiaryIndependentValue,
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

   // Check if the tertiary point is below the minimum or above the maximum
   if ((aTertiaryIndependentValue < mMinTertiaryIndependentVar) || (aTertiaryIndependentValue > mMaxTertiaryIndependentVar))
   {
      return false;
   }

   // At this point, we have valid primary, secondary, and tertiary data points

   // Find the lower and upper bounds
   const AeroTable3d* lowerBoundTable = nullptr;
   const AeroTable3d* upperBoundTable = nullptr;
   double lowerBoundValue = 0.0;
   double upperBoundValue = 0.0;
   for (const auto& item : mModifiedData)
   {
      if (aTertiaryIndependentValue <= item.first)
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
      // We only have a single bound, so get the 3d table
      if (upperBoundTable && (upperBoundTable->GetSecondaryModifiedData(aPrimaryIndependentValue, aDataSet)))
      {
         return true;
      }
      return false;
   }
   else
   {
      // This is the more typical condition, we have two valid bounds

      std::vector<std::pair<double, double>> lowerDataSet;
      if (!lowerBoundTable->GetSecondaryModifiedData(aPrimaryIndependentValue, lowerDataSet))
      {
         return false;
      }

      std::vector<std::pair<double, double>> upperDataSet;
      if (!upperBoundTable || (!upperBoundTable->GetSecondaryModifiedData(aPrimaryIndependentValue, upperDataSet)))
      {
         return false;
      }

      // Clear the data
      aDataSet.clear();

      // Interpolate between datasets
      double deltaValue = upperBoundValue - lowerBoundValue;
      double fraction = (aTertiaryIndependentValue - lowerBoundValue) / deltaValue;

      // Confirm both sets have the same number of elements
      size_t numLowerElements = lowerDataSet.size();
      size_t numUpperElements = upperDataSet.size();
      if (numLowerElements != numUpperElements)
      {
         auto outStream = ut::log::error() << "Number of elements mismatch in AeroTable4d::GetPrimaryBaseData().";
         outStream.AddNote() << "Table: " << mDependentVarName << " vs " << mPrimaryIndependentVarName << ", " << mSecondaryIndependentVarName << ", " << mTertiaryIndependentVarName;
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

bool AeroTable4d::GetTertiaryBaseData(const double&                           aPrimaryIndependentValue,
                                      const double&                           aSecondaryIndependentValue,
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

   // Check if the secondary point is below the minimum or above the maximum
   if ((aSecondaryIndependentValue < mMinSecondaryIndependentVar) || (aSecondaryIndependentValue > mMaxSecondaryIndependentVar))
   {
      return false;
   }

   // Clear the data
   aDataSet.clear();

   for (auto& item : mBaseData)
   {
      double interpolatedValue = 0.0;
      bool valid = item.second.GetInterpolatedDependentValueBase(aPrimaryIndependentValue, aSecondaryIndependentValue, interpolatedValue);
      if (!valid)
      {
         return false;
      }
      aDataSet.emplace_back(item.first, interpolatedValue);
   }

   return true;
}

bool AeroTable4d::GetTertiaryModifiedData(const double&                           aPrimaryIndependentValue,
                                          const double&                           aSecondaryIndependentValue,
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

   // Check if the secondary point is below the minimum or above the maximum
   if ((aSecondaryIndependentValue < mMinSecondaryIndependentVar) || (aSecondaryIndependentValue > mMaxSecondaryIndependentVar))
   {
      return false;
   }

   // Clear the data
   aDataSet.clear();

   for (auto& item : mModifiedData)
   {
      double interpolatedValue = 0.0;
      bool valid = item.second.GetInterpolatedDependentValueModified(aPrimaryIndependentValue, aSecondaryIndependentValue, interpolatedValue);
      if (!valid)
      {
         return false;
      }
      aDataSet.emplace_back(item.first, interpolatedValue);
   }

   return true;
}

bool AeroTable4d::GetInterpolatedDependentValueBase(const double& aPrimaryIndependentValue,
                                                    const double& aSecondaryIndependentValue,
                                                    const double& aTertiaryIndependentValue,
                                                    double&       aValue) const
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

   // Check if the secondary point is below the minimum or above the maximum
   if ((aSecondaryIndependentValue < mMinSecondaryIndependentVar) || (aSecondaryIndependentValue > mMaxSecondaryIndependentVar))
   {
      return false;
   }

   // Check if the tertiary point is below the minimum or above the maximum
   if ((aTertiaryIndependentValue < mMinTertiaryIndependentVar) || (aTertiaryIndependentValue > mMaxTertiaryIndependentVar))
   {
      return false;
   }

   // At this point, we have valid primary, secondary, and tertiary data points

   // Find the lower and upper bounds
   const AeroTable3d* lowerBoundTable = nullptr;
   const AeroTable3d* upperBoundTable = nullptr;
   double lowerBoundValue = 0.0;
   double upperBoundValue = 0.0;
   for (const auto& item : mBaseData)
   {
      if (aTertiaryIndependentValue <= item.first)
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
      // We only have a single bound, so get the 3d table
      double value = 0.0;
      if (upperBoundTable && (upperBoundTable->GetInterpolatedDependentValueBase(aPrimaryIndependentValue, aSecondaryIndependentValue, value)))
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
      double fraction = (aTertiaryIndependentValue - lowerBoundValue) / deltaValue;

      double lowerValue = 0.0;
      if (!lowerBoundTable->GetInterpolatedDependentValueBase(aPrimaryIndependentValue, aSecondaryIndependentValue, lowerValue))
      {
         return false;
      }

      double upperValue = 0.0;
      if (!upperBoundTable || (!upperBoundTable->GetInterpolatedDependentValueBase(aPrimaryIndependentValue, aSecondaryIndependentValue, upperValue)))
      {
         return false;
      }

      double delta2dValue = upperValue - lowerValue;

      aValue = lowerValue + delta2dValue * fraction;
   }

   return true;
}

bool AeroTable4d::GetInterpolatedDependentValueModified(const double& aPrimaryIndependentValue,
                                                        const double& aSecondaryIndependentValue,
                                                        const double& aTertiaryIndependentValue,
                                                        double&       aValue) const
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

   // Check if the tertiary point is below the minimum or above the maximum
   if ((aTertiaryIndependentValue < mMinTertiaryIndependentVar) || (aTertiaryIndependentValue > mMaxTertiaryIndependentVar))
   {
      return false;
   }

   // At this point, we have valid primary, secondary, and tertiary data points

   // Find the lower and upper bounds
   const AeroTable3d* lowerBoundTable = nullptr;
   const AeroTable3d* upperBoundTable = nullptr;
   double lowerBoundValue = 0.0;
   double upperBoundValue = 0.0;
   for (const auto& item : mModifiedData)
   {
      if (aTertiaryIndependentValue <= item.first)
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
      // We only have a single bound, so get the 3d table
      double value = 0.0;
      if (upperBoundTable && (upperBoundTable->GetInterpolatedDependentValueModified(aPrimaryIndependentValue, aSecondaryIndependentValue, value)))
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
      double fraction = (aTertiaryIndependentValue - lowerBoundValue) / deltaValue;

      double lowerValue = 0.0;
      if (!lowerBoundTable->GetInterpolatedDependentValueModified(aPrimaryIndependentValue, aSecondaryIndependentValue, lowerValue))
      {
         return false;
      }

      double upperValue = 0.0;
      if (!upperBoundTable || (!upperBoundTable->GetInterpolatedDependentValueModified(aPrimaryIndependentValue, aSecondaryIndependentValue, upperValue)))
      {
         return false;
      }

      double delta2dValue = upperValue - lowerValue;

      aValue = lowerValue + delta2dValue * fraction;
   }

   return true;
}

void AeroTable4d::ClearAllData()
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
   mTertiaryGainModifierList.clear();
   mSingleTertiaryGainModifier = 1.0;
   mUseSingleTertiaryGainModifier = true;
   mUseTertiaryGainModifier = false;
   mTertiaryOffsetModifierList.clear();
   mSingleTertiaryOffsetModifier = 0.0;
   mUseSingleTertiaryOffsetModifier = true;
   mUseTertiaryOffsetModifier = false;
   mMinPrimaryIndependentVar = 0.0;
   mMaxPrimaryIndependentVar = 0.0;
   mMaxMinPrimaryIndependentVarInitialized = false;
   mMinSecondaryIndependentVar = 0.0;
   mMaxSecondaryIndependentVar = 0.0;
   mMaxMinSecondaryIndependentVarInitialized = false;
   mMinTertiaryIndependentVar = 0.0;
   mMaxTertiaryIndependentVar = 0.0;
   mMaxMinTertiaryIndependentVarInitialized = false;
   mDirtyDataPresent = false;
}

void AeroTable4d::UpdateModifiedData()
{
   // First, copy the current base data to the modified data
   CopyBaseDataToModifiedData();

   // If there is no dirty data, we're done
   if (!mDirtyDataPresent) { return; }

   // Clear the flag
   mDirtyDataPresent = false;

   // Ensure that the 3d tables know about modifiers and update their data
   for (auto& table : mModifiedData)
   {
      // First, pass along the primary modifiers
      if (mUsePrimaryGainModifier)
      {
         if (mUseSinglePrimaryGainModifier)
         {
            table.second.SetSinglePrimaryGainModifier(mSinglePrimaryGainModifier);
         }
         else
         {
            table.second.SetListBasedPrimaryGainModifier(mPrimaryGainModifierList);
         }
      }
      else
      {
         table.second.ClearPrimaryGainModifier();
      }

      if (mUsePrimaryOffsetModifier)
      {
         if (mUseSinglePrimaryOffsetModifier)
         {
            table.second.SetSinglePrimaryOffsetModifier(mSinglePrimaryOffsetModifier);
         }
         else
         {
            table.second.SetListBasedPrimaryOffsetModifier(mPrimaryOffsetModifierList);
         }
      }
      else
      {
         table.second.ClearPrimaryOffsetModifier();
      }

      // Next, pass along the secondary modifiers
      if (mUseSecondaryGainModifier)
      {
         if (mUseSingleSecondaryGainModifier)
         {
            table.second.SetSingleSecondaryGainModifier(mSingleSecondaryGainModifier);
         }
         else
         {
            table.second.SetListBasedSecondaryGainModifier(mSecondaryGainModifierList);
         }
      }
      else
      {
         table.second.ClearSecondaryGainModifier();
      }

      if (mUseSecondaryOffsetModifier)
      {
         if (mUseSingleSecondaryOffsetModifier)
         {
            table.second.SetSingleSecondaryOffsetModifier(mSingleSecondaryOffsetModifier);
         }
         else
         {
            table.second.SetListBasedSecondaryOffsetModifier(mSecondaryOffsetModifierList);
         }
      }
      else
      {
         table.second.ClearSecondaryOffsetModifier();
      }

      // Now, pass along the tertiary modifiers
      double gainValue = 1.0;
      double offsetValue = 0.0;

      if (mUseTertiaryGainModifier)
      {
         if (mUseSingleTertiaryGainModifier)
         {
            gainValue = mSingleTertiaryGainModifier;
         }
         else
         {
            double value = 0.0;
            if (Interpolate(mTertiaryGainModifierList, table.first, value))
            {
               gainValue = value;
            }
         }
      }

      if (mUseTertiaryOffsetModifier)
      {
         if (mUseSingleTertiaryOffsetModifier)
         {
            offsetValue = mSingleTertiaryOffsetModifier;
         }
         else
         {
            double value = 0.0;
            if (Interpolate(mTertiaryOffsetModifierList, table.first, value))
            {
               offsetValue = value;
            }
         }
      }

      // Apply the gain and offset data, if appropriate

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

void AeroTable4d::SetListBasedPrimaryGainModifier(const std::vector<std::pair<double, double>>& aModifierList)
{
   mDirtyDataPresent              = true;
   mPrimaryGainModifierList      = aModifierList;
   mUseSinglePrimaryGainModifier = false;
   mUsePrimaryGainModifier       = true;
}

void AeroTable4d::SetSinglePrimaryGainModifier(const double& aSingleModifier)
{
   mDirtyDataPresent             = true;
   mSinglePrimaryGainModifier    = aSingleModifier;
   mUseSinglePrimaryGainModifier = true;
   mUsePrimaryGainModifier       = true;
}

void AeroTable4d::ClearPrimaryGainModifier()
{
   mUsePrimaryGainModifier = false;
}

void AeroTable4d::SetListBasedPrimaryOffsetModifier(const std::vector<std::pair<double, double>>& aModifierList)
{
   mDirtyDataPresent               = true;
   mPrimaryOffsetModifierList      = aModifierList;
   mUseSinglePrimaryOffsetModifier = false;
   mUsePrimaryOffsetModifier       = true;
}

void AeroTable4d::SetSinglePrimaryOffsetModifier(const double& aSingleModifier)
{
   mDirtyDataPresent               = true;
   mSinglePrimaryOffsetModifier    = aSingleModifier;
   mUseSinglePrimaryOffsetModifier = true;
   mUsePrimaryOffsetModifier       = true;
}

void AeroTable4d::ClearPrimaryOffsetModifier()
{
   mUsePrimaryOffsetModifier = false;
}

void AeroTable4d::SetListBasedSecondaryGainModifier(const std::vector<std::pair<double, double>>& aModifierList)
{
   mDirtyDataPresent               = true;
   mSecondaryGainModifierList      = aModifierList;
   mUseSingleSecondaryGainModifier = false;
   mUseSecondaryGainModifier       = true;
}

void AeroTable4d::SetSingleSecondaryGainModifier(const double& aSingleModifier)
{
   mDirtyDataPresent               = true;
   mSingleSecondaryGainModifier    = aSingleModifier;
   mUseSingleSecondaryGainModifier = true;
   mUseSecondaryGainModifier       = true;
}

void AeroTable4d::ClearSecondaryGainModifier()
{
   mUseSecondaryGainModifier = false;
}

void AeroTable4d::SetListBasedSecondaryOffsetModifier(const std::vector<std::pair<double, double>>& aModifierList)
{
   mDirtyDataPresent                 = true;
   mSecondaryOffsetModifierList      = aModifierList;
   mUseSingleSecondaryOffsetModifier = false;
   mUseSecondaryOffsetModifier       = true;
}

void AeroTable4d::SetSingleSecondaryOffsetModifier(const double& aSingleModifier)
{
   mDirtyDataPresent                 = true;
   mSingleSecondaryOffsetModifier    = aSingleModifier;
   mUseSingleSecondaryOffsetModifier = true;
   mUseSecondaryOffsetModifier       = true;
}

void AeroTable4d::ClearSecondaryOffsetModifier()
{
   mUseSecondaryOffsetModifier = false;
}

void AeroTable4d::SetListBasedTertiaryGainModifier(const std::vector<std::pair<double, double>>& aModifierList)
{
   mDirtyDataPresent              = true;
   mTertiaryGainModifierList      = aModifierList;
   mUseSingleTertiaryGainModifier = false;
   mUseTertiaryGainModifier       = true;
}

void AeroTable4d::SetSingleTertiaryGainModifier(const double& aSingleModifier)
{
   mDirtyDataPresent              = true;
   mSingleTertiaryGainModifier    = aSingleModifier;
   mUseSingleTertiaryGainModifier = true;
   mUseTertiaryGainModifier       = true;
}

void AeroTable4d::ClearTertiaryGainModifier()
{
   mUseTertiaryGainModifier = false;
}

void AeroTable4d::SetListBasedTertiaryOffsetModifier(const std::vector<std::pair<double, double>>& aModifierList)
{
   mDirtyDataPresent                = true;
   mTertiaryOffsetModifierList      = aModifierList;
   mUseSingleTertiaryOffsetModifier = false;
   mUseTertiaryOffsetModifier       = true;
}

void AeroTable4d::SetSingleTertiaryOffsetModifier(const double& aSingleModifier)
{
   mDirtyDataPresent                = true;
   mSingleTertiaryOffsetModifier    = aSingleModifier;
   mUseSingleTertiaryOffsetModifier = true;
   mUseTertiaryOffsetModifier       = true;
}

void AeroTable4d::ClearTertiaryOffsetModifier()
{
   mUseTertiaryOffsetModifier = false;
}

bool AeroTable4d::MaxMinPrimaryIndependentVariableRange(double& aMinIndependentVar, double& aMaxIndependentVar)
{
   aMinIndependentVar = mMinPrimaryIndependentVar;
   aMaxIndependentVar = mMaxPrimaryIndependentVar;
   return mMaxMinPrimaryIndependentVarInitialized;
}

bool AeroTable4d::MaxMinSecondaryIndependentVariableRange(double& aMinIndependentVar, double& aMaxIndependentVar)
{
   aMinIndependentVar = mMinSecondaryIndependentVar;
   aMaxIndependentVar = mMaxSecondaryIndependentVar;
   return mMaxMinSecondaryIndependentVarInitialized;
}

bool AeroTable4d::MaxMinTertiaryIndependentVariableRange(double& aMinIndependentVar, double& aMaxIndependentVar)
{
   aMinIndependentVar = mMinTertiaryIndependentVar;
   aMaxIndependentVar = mMaxTertiaryIndependentVar;
   return mMaxMinTertiaryIndependentVarInitialized;
}

const std::vector<double> AeroTable4d::GetTertiaryIndepenentValues()
{
   std::vector<double> tertiaryValues;

   // Copy the independent variable values (first item in the pairs in primaryVars)
   std::transform(mBaseData.begin(), mBaseData.end(), std::back_inserter(tertiaryValues),
                  [](const std::pair<double, AeroTable3d>& v) {return v.first; });

   return tertiaryValues;
}

const std::vector<double> AeroTable4d::GetSecondaryIndependentValues(const double& aTertiaryIndependentValue)
{
   std::vector<double> secondaryValues;

   auto secondaryValuesIter = std::find_if(mBaseData.begin(), mBaseData.end(), 
                              [&](const std::pair<double, AeroTable3d>& p) {return p.first == aTertiaryIndependentValue; });

   if (secondaryValuesIter != mBaseData.end())
   {
      secondaryValues = secondaryValuesIter->second.GetSecondaryIndependentValues();
   }

   return secondaryValues;
}

const std::vector<double> AeroTable4d::GetSecondaryIndependentValues()
{
   std::vector<double> secondaryValues;
   for (auto curTertiary : GetTertiaryIndepenentValues())
   {
      for (auto curVal : GetSecondaryIndependentValues(curTertiary))
      {
         if (std::find(secondaryValues.begin(), secondaryValues.end(), curVal) == secondaryValues.end())
         {
            secondaryValues.emplace_back(curVal);
         }
      }
   }
   return secondaryValues;
}

const std::vector<double> AeroTable4d::GetPrimaryIndependentValuesGivenTertiary(const double& aTertiaryValue)
{
   std::vector<double> primaryValues;
   
   auto it = std::find_if(mBaseData.begin(), mBaseData.end(), [aTertiaryValue]
                         (const std::pair<double, AeroTable3d>& element) {return element.first == aTertiaryValue; });
   if (it != mBaseData.end())
   {
      AeroTable3d table = it->second;
      for (auto curPrimary : table.GetPrimaryIndependentValues())
      {
         if (std::find(primaryValues.begin(), primaryValues.end(), curPrimary) == primaryValues.end())
         {
            primaryValues.emplace_back(curPrimary);
         }
      }
   }
   return primaryValues;
}

const std::vector<double> AeroTable4d::GetPrimaryIndependentValues()
{
   std::vector<double> primaryValues;
   auto firstTable = mBaseData.begin();
   AeroTable3d table = firstTable->second;
   for (auto curPrimary : table.GetPrimaryIndependentValues())
   {
      if (std::find(primaryValues.begin(), primaryValues.end(), curPrimary) == primaryValues.end())
      {
         primaryValues.emplace_back(curPrimary);
      }
   }
   return primaryValues;
}

void AeroTable4d::RevertPlot()
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

   mTertiaryGainModifierList.clear();
   mSingleTertiaryGainModifier = 1.0;
   mUseSingleTertiaryGainModifier = true;
   mUseTertiaryGainModifier = false;

   mTertiaryOffsetModifierList.clear();
   mSingleTertiaryOffsetModifier = 0.0;
   mUseSingleTertiaryOffsetModifier = true;
   mUseTertiaryOffsetModifier = false;

   UpdateModifiedData();
}

void AeroTable4d::CopyBaseDataToModifiedData()
{
   mModifiedData.clear();

   for (auto& data : mBaseData)
   {
      data.second.CopyBaseDataToModifiedData();
      std::pair<double, AeroTable3d> tempPair(data.first, data.second);
      mModifiedData.push_back(tempPair);
   }
}

bool AeroTable4d::HasIndependentVarName(const std::string& aVarName) const
{
   return aVarName == GetPrimaryIndependentVarName()
          || aVarName == GetSecondaryIndependentVarName()
          || aVarName == GetTertiaryIndependentVarName();
}

void AeroTable4d::SetListBasedGainModifier(const std::string& aVarName, const std::vector<std::pair<double, double>>& aModifierLine)
{
   if (aVarName == GetPrimaryIndependentVarName())
   {
      SetListBasedPrimaryGainModifier(aModifierLine);
   }
   else if (aVarName == GetSecondaryIndependentVarName())
   {
      SetListBasedSecondaryGainModifier(aModifierLine);
   }
   else if (aVarName == GetTertiaryIndependentVarName())
   {
      SetListBasedTertiaryGainModifier(aModifierLine);
   }
   else
   {
      std::stringstream ss{};
      ss << "ERROR: table '" << GetTableName() << "' has no independent variable '" << aVarName << "'. Unable to set list-based gain modifier.";
      throw std::runtime_error(ss.str());
   }
}

void AeroTable4d::SetSingleGainModifier(const std::string& aVarName, double aModifierValue)
{
   if (aVarName == GetPrimaryIndependentVarName())
   {
      SetSinglePrimaryGainModifier(aModifierValue);
   }
   else if (aVarName == GetSecondaryIndependentVarName())
   {
      SetSingleSecondaryGainModifier(aModifierValue);
   }
   else if (aVarName == GetTertiaryIndependentVarName())
   {
      SetSingleTertiaryGainModifier(aModifierValue);
   }
   else
   {
      std::stringstream ss{};
      ss << "ERROR: table '" << GetTableName() << "' has no independent variable '" << aVarName << "'. Unable to set single gain value.";
      throw std::runtime_error(ss.str());
   }
}

void AeroTable4d::SetListBasedOffsetModifier(const std::string& aVarName, const std::vector<std::pair<double, double>>& aModifierLine)
{
   if (aVarName == GetPrimaryIndependentVarName())
   {
      SetListBasedPrimaryOffsetModifier(aModifierLine);
   }
   else if (aVarName == GetSecondaryIndependentVarName())
   {
      SetListBasedSecondaryOffsetModifier(aModifierLine);
   }
   else if (aVarName == GetTertiaryIndependentVarName())
   {
      SetListBasedTertiaryOffsetModifier(aModifierLine);
   }
   else
   {
      std::stringstream ss{};
      ss << "ERROR: table '" << GetTableName() << "' has no independent variable '" << aVarName << "'. Unable to set list-based offset modifier.";
      throw std::runtime_error(ss.str());
   }
}

void AeroTable4d::SetSingleOffsetModifier(const std::string& aVarName, double aModifierValue)
{
   if (aVarName == GetPrimaryIndependentVarName())
   {
      SetSinglePrimaryOffsetModifier(aModifierValue);
   }
   else if (aVarName == GetSecondaryIndependentVarName())
   {
      SetSingleSecondaryOffsetModifier(aModifierValue);
   }
   else if (aVarName == GetTertiaryIndependentVarName())
   {
      SetSingleTertiaryOffsetModifier(aModifierValue);
   }
   else
   {
      std::stringstream ss{};
      ss << "ERROR: table '" << GetTableName() << "' has no independent variable '" << aVarName << "'. Unable to set single offset value.";
      throw std::runtime_error(ss.str());
   }
}

} // namespace Designer
