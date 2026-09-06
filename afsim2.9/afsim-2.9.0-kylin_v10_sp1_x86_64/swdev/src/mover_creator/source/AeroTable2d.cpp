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

#include "AeroTable2d.hpp"

#include <QJsonObject>
#include <stdexcept>
#include <sstream>

namespace Designer
{

AeroTable2d::AeroTable2d(const std::string& aDependentVarName,
                         const std::string& aIndependentVarName)
   : mDependentVarName(aDependentVarName)
   , mIndependentVarName(aIndependentVarName)
   , mTableName(aDependentVarName + " vs " + aIndependentVarName)
{
}

AeroTable2d& AeroTable2d::operator=(const AeroTable2d& aRhs)
{
   if (this != &aRhs)
   {
      *this = aRhs;
   }
   return *this;
}

void AeroTable2d::AddData(double aIndependentVar, double aDependentVar)
{
   mBaseData.emplace_back(aIndependentVar, aDependentVar);

   if (!mMaxMinIndependentVarInitialized)
   {
      mMaxMinIndependentVarInitialized = true;
      mMinIndependentVar = aIndependentVar;
      mMaxIndependentVar = aIndependentVar;
   }
   else
   {
      if (aIndependentVar < mMinIndependentVar)
      {
         mMinIndependentVar = aIndependentVar;
      }

      if (aIndependentVar > mMaxIndependentVar)
      {
         mMaxIndependentVar = aIndependentVar;
      }
   }
}


bool AeroTable2d::GetBaseData(std::vector<std::pair<double, double>>& aDataset) const
{
   if (mBaseData.empty())
   {
      return false;
   }

   aDataset = mBaseData;
   return true;
}

bool AeroTable2d::GetBaseData(std::vector<std::pair<double, double>>*& aDataset)
{
   if (mBaseData.empty())
   {
      return false;
   }

   aDataset = &mBaseData;
   return true;
}

bool AeroTable2d::GetModifiedData(std::vector<std::pair<double, double>>& aDataset) const
{
   if (mModifiedData.empty())
   {
      return false;
   }

   aDataset = mModifiedData;
   return true;
}

bool AeroTable2d::GetInterpolatedDependentValueBase(const double& aIndependentValue, double& aValue) const
{
   if (!mMaxMinIndependentVarInitialized)
   {
      return false;
   }

   return Interpolate(mBaseData, aIndependentValue, aValue);
}

bool AeroTable2d::GetInterpolatedDependentValueModified(const double& aIndependentValue, double& aValue) const
{
   if (!mMaxMinIndependentVarInitialized)
   {
      return false;
   }

   return Interpolate(mModifiedData, aIndependentValue, aValue);
}

void AeroTable2d::ClearAllData()
{
   mBaseData.clear();
   mModifiedData.clear();
   mGainModifierList.clear();
   mSingleGainModifier = 1.0;
   mUseSingleGainModifier = true;
   mUseGainModifier = false;
   mOffsetModifierList.clear();
   mSingleOffsetModifier = 0.0;
   mUseSingleOffsetModifier = true;
   mUseOffsetModifier = false;
   mExternalGainModifier = 1.0;
   mExternalOffsetModifier = 0.0;
   mUseExternalGainModifier = false;
   mUseExternalOffsetModifier = false;
   mMinIndependentVar = 0.0;
   mMaxIndependentVar = 0.0;
   mMaxMinIndependentVarInitialized = false;
   mDirtyDataPresent = false;
}

void Designer::AeroTable2d::ClearBaseData()
{
   mBaseData.clear();
}

void AeroTable2d::UpdateModifiedData()
{
   // First, copy the current base data to the modified data
   CopyBaseDataToModifiedData();

   // If there is no dirty data, we're done
   if (!mDirtyDataPresent) { return; }

   // Clear the flag
   mDirtyDataPresent = false;

   for (auto& data : mModifiedData)
   {
      double independentVar = data.first;
      double dependentVar   = data.second;

      // Apply offset then gain
      if (mUseOffsetModifier)
      {
         if (mUseSingleOffsetModifier)
         {
            dependentVar += mSingleOffsetModifier;
         }
         else
         {
            double value = 0.0;
            bool valid = Interpolate(mOffsetModifierList, independentVar, value);

            if (valid)
            {
               dependentVar += value;
            }
         }
      }
      if (mUseExternalOffsetModifier)
      {
         dependentVar += mExternalOffsetModifier;
      }

      // Apply gain after offset
      if (mUseGainModifier)
      {
         if (mUseSingleGainModifier)
         {
            dependentVar *= mSingleGainModifier;
         }
         else
         {
            double value = 0.0;
            bool valid = Interpolate(mGainModifierList, independentVar, value);

            if (valid)
            {
               dependentVar *= value;
            }
         }
      }
      if (mUseExternalGainModifier)
      {
         dependentVar *= mExternalGainModifier;
      }

      data.second = dependentVar;
   }
}

void AeroTable2d::SetListBasedGainModifier(const std::vector<std::pair<double, double>>& aModifierList)
{
   mDirtyDataPresent      = true;
   mGainModifierList      = aModifierList;
   mUseSingleGainModifier = false;
   mUseGainModifier       = true;

   mUseSingleOffsetModifier = false;
   mUseOffsetModifier       = false;
}

void AeroTable2d::SetSingleGainModifier(const double& aSingleModifier)
{
   mDirtyDataPresent      = true;
   mSingleGainModifier    = aSingleModifier;
   mUseSingleGainModifier = true;
   mUseGainModifier       = true;

   mUseSingleOffsetModifier = false;
   mUseOffsetModifier       = false;
}

void AeroTable2d::ClearGainModifier()
{
   mUseGainModifier = false;
}

void AeroTable2d::SetListBasedOffsetModifier(const std::vector<std::pair<double, double>>& aModifierList)
{
   mDirtyDataPresent        = true;
   mOffsetModifierList      = aModifierList;
   mUseSingleOffsetModifier = false;
   mUseOffsetModifier       = true;

   mUseSingleGainModifier   = false;
   mUseGainModifier         = false;
}

void AeroTable2d::SetSingleOffsetModifier(const double& aSingleModifier)
{
   mDirtyDataPresent        = true;
   mSingleOffsetModifier    = aSingleModifier;
   mUseSingleOffsetModifier = true;
   mUseOffsetModifier       = true;

   mUseSingleGainModifier = false;
   mUseGainModifier       = false;
}

void AeroTable2d::ClearOffsetModifier()
{
   mUseOffsetModifier = false;
}

void AeroTable2d::SetExternalGainModifier(const double& aModifier)
{
   mDirtyDataPresent        = true;
   mExternalGainModifier    = aModifier;
   mUseExternalGainModifier = true;
}

void AeroTable2d::ClearExternalGainModifier()
{
   mUseExternalGainModifier = false;
}

void AeroTable2d::SetExternalOffsetModifier(const double& aModifier)
{
   mDirtyDataPresent          = true;
   mExternalOffsetModifier    = aModifier;
   mUseExternalOffsetModifier = true;
}

void AeroTable2d::ClearExternalOffsetModifier()
{
   mUseExternalOffsetModifier = false;
}

bool AeroTable2d::MaxMinIndependentVariableRange(double& aMinIndependentVar, double& aMaxIndependentVar)
{
   aMinIndependentVar = mMinIndependentVar;
   aMaxIndependentVar = mMaxIndependentVar;
   return mMaxMinIndependentVarInitialized;
}

bool AeroTable2d::GetDependentVarAtIndex(size_t aIndex, double& aValue)
{
   if (aIndex < mBaseData.size())
   {
      aValue = mBaseData.at(aIndex).second;
      return true;
   }
   return false;
}

bool AeroTable2d::GetIndependentVarAtIndex(size_t aIndex, double& aValue)
{
   if (aIndex < mBaseData.size())
   {
      aValue = mBaseData.at(aIndex).first;
      return true;
   }
   return false;
}

void AeroTable2d::RevertPlot()
{
   mGainModifierList.clear();
   mSingleGainModifier = 1.0;
   mUseSingleGainModifier = true;
   mUseGainModifier = false;

   mOffsetModifierList.clear();
   mSingleOffsetModifier = 0.0;
   mUseSingleOffsetModifier = true;
   mUseOffsetModifier = false;

   mExternalGainModifier = 1.0;
   mExternalOffsetModifier = 0.0;
   mUseExternalGainModifier = false;
   mUseExternalOffsetModifier = false;

   UpdateModifiedData();
}

void AeroTable2d::CopyBaseDataToModifiedData()
{
   mModifiedData.clear();
   mModifiedData = mBaseData;
}

bool AeroTable2d::HasIndependentVarName(const std::string& aVarName) const
{
   return aVarName == GetIndependentVarName();
}

void AeroTable2d::SetListBasedGainModifier(const std::string& aVarName, const std::vector<std::pair<double, double>>& aModifierLine)
{
   if (HasIndependentVarName(aVarName))
   {
      SetListBasedGainModifier(aModifierLine);
   }
}

void AeroTable2d::SetSingleGainModifier(const std::string& aVarName, double aModifierValue)
{
   if (HasIndependentVarName(aVarName))
   {
      SetSingleGainModifier(aModifierValue);
   }
}

void AeroTable2d::SetListBasedOffsetModifier(const std::string& aVarName, const std::vector<std::pair<double, double>>& aModifierLine)
{
   if (HasIndependentVarName(aVarName))
   {
      SetListBasedOffsetModifier(aModifierLine);
   }
}

void AeroTable2d::SetSingleOffsetModifier(const std::string& aVarName, double aModifierValue)
{
   if (HasIndependentVarName(aVarName))
   {
      SetSingleOffsetModifier(aModifierValue);
   }
}

} // namespace Designer
