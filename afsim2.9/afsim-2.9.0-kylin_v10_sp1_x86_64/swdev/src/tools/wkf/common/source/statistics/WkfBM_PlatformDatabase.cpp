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
#include "WkfBM_PlatformDatabase.hpp"

#include <stdexcept>

bool wkf::bm::PlatformPlotData::AliveAt(double aSimTime) const noexcept
{
   return mStartTime <= aSimTime && aSimTime < mEndTime;
}

int wkf::bm::PlatformPlotDatabase::TotalCount(double aSimTime, const QString& aSide, const QString& aType) const noexcept
{
   int retval = 0;
   for (const auto& p : mData)
   {
      const auto& plat = p.second;
      if (plat.AliveAt(aSimTime))
      {
         if (aSide.isEmpty() || plat.mSide == aSide)
         {
            if (aType.isEmpty() || plat.mTypes.contains(aType))
            {
               retval++;
            }
         }
      }
   }
   return retval;
}

int wkf::bm::PlatformPlotDatabase::PlatformCount(double aSimTime, const QString& aSide, const QString& aType) const noexcept
{
   int retval = 0;
   for (const auto& p : mData)
   {
      const auto& plat = p.second;
      if (plat.AliveAt(aSimTime) && !plat.mIsWeapon)
      {
         if (aSide.isEmpty() || plat.mSide == aSide)
         {
            if (aType.isEmpty() || plat.mTypes.contains(aType))
            {
               retval++;
            }
         }
      }
   }
   return retval;
}

int wkf::bm::PlatformPlotDatabase::WeaponCount(double aSimTime, const QString& aSide, const QString& aType) const noexcept
{
   int retval = 0;
   for (const auto& p : mData)
   {
      const auto& plat = p.second;
      if (plat.AliveAt(aSimTime) && plat.mIsWeapon)
      {
         if (aSide.isEmpty() || plat.mSide == aSide)
         {
            if (aType.isEmpty() || plat.mTypes.contains(aType))
            {
               retval++;
            }
         }
      }
   }
   return retval;
}

int wkf::bm::PlatformPlotDatabase::CategoryCount(double         aSimTime,
                                                 const QString& aSide,
                                                 const QString& aType,
                                                 const QString& aCategory) const noexcept
{
   int retval = 0;
   for (const auto& p : mData)
   {
      const auto& plat = p.second;
      if (plat.AliveAt(aSimTime))
      {
         if (aSide.isEmpty() || plat.mSide == aSide)
         {
            if (aType.isEmpty() || plat.mTypes.contains(aType))
            {
               if (aCategory.isEmpty())
               {
                  if (plat.mCategories.empty())
                  {
                     retval++;
                  }
               }
               else if (plat.mCategories.contains(aCategory))
               {
                  retval++;
               }
            }
         }
      }
   }
   return retval;
}

wkf::bm::PlatformPlotData& wkf::bm::PlatformPlotDatabase::AddPlatform(unsigned aIndex)
{
   mSidesChanged      = true;
   mTypesChanged      = true;
   mCategoriesChanged = true;
   return mData[aIndex];
}

wkf::bm::PlatformPlotData& wkf::bm::PlatformPlotDatabase::FindPlatform(unsigned aIndex)
{
   auto it = mData.find(aIndex);
   if (it == mData.end())
   {
      throw std::runtime_error("Cannot find platform.");
   }
   return it->second;
}

void wkf::bm::PlatformPlotDatabase::RemovePlatform(double aSimTime, unsigned aIndex)
{
   auto it = mData.find(aIndex);
   if (it == mData.end())
   {
      throw std::runtime_error("Cannot remove platform that does not exist.");
   }

   it->second.mEndTime = aSimTime;
}

QStringList wkf::bm::PlatformPlotDatabase::GetSides() const noexcept
{
   if (mSidesChanged)
   {
      mSidesChanged = false;
      for (const auto& p : mData)
      {
         const auto& plat = p.second;
         if (!plat.mSide.isEmpty() && !mSides.contains(plat.mSide))
         {
            mSides.append(plat.mSide);
         }
      }

      mSides.sort();
   }

   return mSides;
}

QStringList wkf::bm::PlatformPlotDatabase::GetTypes() const noexcept
{
   if (mTypesChanged)
   {
      mTypesChanged = false;
      for (const auto& p : mData)
      {
         for (const auto& type : p.second.mTypes)
         {
            if (!mTypes.contains(type))
            {
               mTypes.append(type);
            }
         }
      }

      mTypes.sort();
   }

   return mTypes;
}

QStringList wkf::bm::PlatformPlotDatabase::GetCategories() const noexcept
{
   if (mCategoriesChanged)
   {
      mCategoriesChanged = false;
      for (const auto& p : mData)
      {
         for (const auto& category : p.second.mCategories)
         {
            if (!mCategories.contains(category))
            {
               mCategories.append(category);
            }
         }
      }

      mCategories.sort();
   }

   return mCategories;
}

void wkf::bm::PlatformPlotDatabase::AddSides(QStringList aSides) noexcept
{
   for (auto& side : aSides)
   {
      if (!mSides.contains(side))
      {
         mSides.append(std::move(side));
      }
   }
   mSides.sort();
   // Not setting changed flag. mSides already has correct value.
}

void wkf::bm::PlatformPlotDatabase::AddTypes(QStringList aTypes) noexcept
{
   for (auto& type : aTypes)
   {
      if (!mTypes.contains(type))
      {
         mTypes.append(std::move(type));
      }
   }
   mTypes.sort();
   // Not setting changed flag. mTypes already has correct value.
}

void wkf::bm::PlatformPlotDatabase::AddCategories(QStringList aCategories) noexcept
{
   for (auto& category : aCategories)
   {
      if (!mCategories.contains(category))
      {
         mCategories.append(std::move(category));
      }
   }
   mCategories.sort();
   // Not setting changed flag. mCategories already has correct value.
}

void wkf::bm::PlatformPlotDatabase::Clear() noexcept
{
   mData.clear();
   mSides.clear();
   mTypes.clear();
   mCategories.clear();
   // Not setting the changed flags. All 3 lists already have the correct values.
}

bool wkf::bm::PlatformPlotDatabase::Empty() const noexcept
{
   return mData.empty();
}

int wkf::bm::PlatformPlotDatabase::EntryCount() const noexcept
{
   return static_cast<int>(mData.size());
}

wkf::bm::PlatformPlotDatabase::iterator wkf::bm::PlatformPlotDatabase::begin() const noexcept
{
   return mData.begin();
}

wkf::bm::PlatformPlotDatabase::iterator wkf::bm::PlatformPlotDatabase::end() const noexcept
{
   return mData.end();
}
