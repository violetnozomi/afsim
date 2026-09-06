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

#include "CoverageData.hpp"

#include <algorithm>
#include <limits>
#include <numeric>

#include "UtMath.hpp"

namespace
{

constexpr std::size_t cLATITUDE_INDEX{0};
constexpr std::size_t cLONGITUDE_INDEX{1};
constexpr std::size_t cALTITUDE_INDEX{2};
constexpr std::size_t cFIELD_INDEX_BASE{3};

} // namespace

namespace CoverageOverlay
{

void CoverageData::GetFieldValueRange(std::size_t aFieldIndex, float& aMinValue, float& aMaxValue) const
{
   aMinValue = std::numeric_limits<float>::max();
   aMaxValue = std::numeric_limits<float>::lowest();
   for (auto& component : mComponents)
   {
      aMinValue = std::min(component.GetMinValue(aFieldIndex), aMinValue);
      aMaxValue = std::max(component.GetMaxValue(aFieldIndex), aMaxValue);
   }
}

std::size_t CoverageData::GetNumLats(std::size_t aComponentIndex) const
{
   return mComponents.at(aComponentIndex).GetNumLats();
}

std::size_t CoverageData::GetNumLons(std::size_t aComponentIndex) const
{
   return mComponents.at(aComponentIndex).GetNumLons();
}

float CoverageData::GetLatitude(std::size_t aComponentIndex, std::size_t aLatIndex, std::size_t aLonIndex) const
{
   return mComponents.at(aComponentIndex).GetLatitude(aLatIndex, aLonIndex);
}

float CoverageData::GetLongitude(std::size_t aComponentIndex, std::size_t aLatIndex, std::size_t aLonIndex) const
{
   return mComponents.at(aComponentIndex).GetLongitude(aLatIndex, aLonIndex);
}

float CoverageData::GetAltitude(std::size_t aComponentIndex, std::size_t aLatIndex, std::size_t aLonIndex) const
{
   return mComponents.at(aComponentIndex).GetAltitude(aLatIndex, aLonIndex);
}

float CoverageData::GetField(std::size_t aComponentIndex,
                             std::size_t aLatIndex,
                             std::size_t aLonIndex,
                             std::size_t aFieldIndex) const
{
   return mComponents.at(aComponentIndex).GetField(aLatIndex, aLonIndex, aFieldIndex);
}

CoverageData::Component::Component(std::size_t aNumLats, std::size_t aNumLons)
   : mNumLats{aNumLats}
   , mNumLons{aNumLons}
{
   std::size_t count = mNumLats * mNumLons;
   mValues.reserve(count);
   for (std::size_t i = 0; i < count; ++i)
   {
      mValues.emplace_back(std::vector<float>{});
   }
}

float CoverageData::Component::GetLatitude(std::size_t aLatIndex, std::size_t aLonIndex) const
{
   return GetValue(aLatIndex, aLonIndex, cLATITUDE_INDEX);
}

float CoverageData::Component::GetLongitude(std::size_t aLatIndex, std::size_t aLonIndex) const
{
   return GetValue(aLatIndex, aLonIndex, cLONGITUDE_INDEX);
}

float CoverageData::Component::GetAltitude(std::size_t aLatIndex, std::size_t aLonIndex) const
{
   return GetValue(aLatIndex, aLonIndex, cALTITUDE_INDEX);
}

float CoverageData::Component::GetField(std::size_t aLatIndex, std::size_t aLonIndex, std::size_t aFieldIndex) const
{
   return GetValue(aLatIndex, aLonIndex, aFieldIndex + cFIELD_INDEX_BASE);
}

float CoverageData::Component::GetMinValue(std::size_t aFieldIndex) const
{
   auto fieldLess = [aFieldIndex](float aValue, const std::vector<float>& aFields)
   { return std::min(aValue, aFields[aFieldIndex + cFIELD_INDEX_BASE]); };
   return std::accumulate(mValues.begin(), mValues.end(), std::numeric_limits<float>::max(), fieldLess);
}

float CoverageData::Component::GetMaxValue(std::size_t aFieldIndex) const
{
   auto fieldGreater = [aFieldIndex](float aValue, const std::vector<float>& aFields)
   { return std::max(aValue, aFields[aFieldIndex + cFIELD_INDEX_BASE]); };
   return std::accumulate(mValues.begin(), mValues.end(), std::numeric_limits<float>::lowest(), fieldGreater);
}

void CoverageData::Component::SetValues(std::size_t aLatIndex, std::size_t aLonIndex, const std::vector<float>& aValues)
{
   auto index     = LinearIndex(aLatIndex, aLonIndex);
   mValues[index] = aValues;
}

vespa::VaBoundingBox CoverageData::Component::GetBoundingBox() const
{
   vespa::VaBoundingBox retval{};
   auto                 op = [&retval](const std::vector<float>& aValue)
   {
      retval.mMinLLA[0] = std::min(retval.mMinLLA[0], static_cast<double>(aValue[0]));
      retval.mMinLLA[1] = std::min(retval.mMinLLA[1], static_cast<double>(aValue[1]));
      retval.mMinLLA[2] = std::min(retval.mMinLLA[2], static_cast<double>(aValue[2]));
      retval.mMaxLLA[0] = std::max(retval.mMaxLLA[0], static_cast<double>(aValue[0]));
      retval.mMaxLLA[1] = std::max(retval.mMaxLLA[1], static_cast<double>(aValue[1]));
      retval.mMaxLLA[2] = std::max(retval.mMaxLLA[2], static_cast<double>(aValue[2]));
   };
   std::for_each(mValues.begin(), mValues.end(), op);
   return retval;
}

float CoverageData::Component::GetValue(std::size_t aLatIndex, std::size_t aLonIndex, std::size_t aValueIndex) const
{
   auto idx = LinearIndex(aLatIndex, aLonIndex);
   return mValues[idx][aValueIndex];
}

std::size_t CoverageData::Component::LinearIndex(std::size_t aLatIndex, std::size_t aLonIndex) const
{
   if (static_cast<unsigned int>(aLatIndex) >= mNumLats)
   {
      aLatIndex = mNumLats - 1;
   }

   if (static_cast<unsigned int>(aLonIndex) >= mNumLons)
   {
      aLonIndex = mNumLons - 1;
   }

   return aLonIndex + aLatIndex * mNumLons;
}

void CoverageData::AddComponent(Component&& aComponent)
{
   mComponents.emplace_back(aComponent);
}

vespa::VaBoundingBox CoverageData::GetComponentBoundingBox(std::size_t aComponentIndex) const
{
   return mComponents.at(aComponentIndex).GetBoundingBox();
}

vespa::VaBoundingBox CoverageData::GetBoundingBox() const
{
   vespa::VaBoundingBox retval{};
   for (auto& component : mComponents)
   {
      retval.Grow(component.GetBoundingBox());
   }
   return retval;
}

} // namespace CoverageOverlay
