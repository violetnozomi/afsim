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

#include "HeatmapFields.hpp"

#include <algorithm>
#include <vector>

#include "UtMath.hpp"

namespace CoverageOverlay
{

Patch::Patch(const HeatmapFields& aFields, std::size_t aComponentIndex, std::size_t aLatIndex, std::size_t aLonIndex)
   : mFields(aFields)
   , mComponentIndex{aComponentIndex}
   , mLowLatIndex{aLatIndex}
   , mLowLonIndex{aLonIndex}
{
   SetupIndices();
}

void Patch::SetNumDivisions(std::size_t aNumLatDivisions, std::size_t aNumLonDivisions)
{
   mNumLatDivisions = aNumLatDivisions;
   mNumLonDivisions = aNumLonDivisions;
}

float Patch::GetLatitude(std::size_t aLatIndex, std::size_t aLonIndex) const
{
   return Interpolate(mFields.GetLatitude(mComponentIndex, mLowLatIndex, mLowLonIndex),
                      mFields.GetLatitude(mComponentIndex, mLowLatIndex, mHighLonIndex),
                      mFields.GetLatitude(mComponentIndex, mHighLatIndex, mLowLonIndex),
                      mFields.GetLatitude(mComponentIndex, mHighLatIndex, mHighLonIndex),
                      aLatIndex,
                      aLonIndex);
}

float Patch::GetLongitude(std::size_t aLatIndex, std::size_t aLonIndex) const
{
   float ll = mFields.GetLongitude(mComponentIndex, mLowLatIndex, mLowLonIndex);
   float lr = mFields.GetLongitude(mComponentIndex, mLowLatIndex, mHighLonIndex);
   float ul = mFields.GetLongitude(mComponentIndex, mHighLatIndex, mLowLonIndex);
   float ur = mFields.GetLongitude(mComponentIndex, mHighLatIndex, mHighLonIndex);
   if (ll * lr < 0.0 || ll * ul < 0.0 || ll * ur < 0.0)
   {
      // We cross the coordinate branch, so map into 0->360;
      ll = static_cast<float>(UtMath::NormalizeAngle0_360(ll));
      lr = static_cast<float>(UtMath::NormalizeAngle0_360(lr));
      ul = static_cast<float>(UtMath::NormalizeAngle0_360(ul));
      ur = static_cast<float>(UtMath::NormalizeAngle0_360(ur));
   }
   return Interpolate(ll, lr, ul, ur, aLatIndex, aLonIndex);
}

float Patch::GetAltitude(std::size_t aLatIndex, std::size_t aLonIndex) const
{
   return Interpolate(mFields.GetAltitude(mComponentIndex, mLowLatIndex, mLowLonIndex),
                      mFields.GetAltitude(mComponentIndex, mLowLatIndex, mHighLonIndex),
                      mFields.GetAltitude(mComponentIndex, mHighLatIndex, mLowLonIndex),
                      mFields.GetAltitude(mComponentIndex, mHighLatIndex, mHighLonIndex),
                      aLatIndex,
                      aLonIndex);
}

float Patch::GetField(std::size_t aLatIndex, std::size_t aLonIndex, std::size_t aFieldIndex) const
{
   return Interpolate(mFields.GetField(mComponentIndex, mLowLatIndex, mLowLonIndex, aFieldIndex),
                      mFields.GetField(mComponentIndex, mLowLatIndex, mHighLonIndex, aFieldIndex),
                      mFields.GetField(mComponentIndex, mHighLatIndex, mLowLonIndex, aFieldIndex),
                      mFields.GetField(mComponentIndex, mHighLatIndex, mHighLonIndex, aFieldIndex),
                      aLatIndex,
                      aLonIndex);
}

float Patch::GetDeltaLat() const
{
   std::vector<float> vals{mFields.GetLatitude(mComponentIndex, mLowLatIndex, mLowLonIndex),
                           mFields.GetLatitude(mComponentIndex, mLowLatIndex, mHighLonIndex),
                           mFields.GetLatitude(mComponentIndex, mHighLatIndex, mLowLonIndex),
                           mFields.GetLatitude(mComponentIndex, mHighLatIndex, mHighLonIndex)};
   std::sort(vals.begin(), vals.end());
   return vals[vals.size() - 1] - vals[0];
}

float Patch::GetDeltaLon() const
{
   float ll = mFields.GetLongitude(mComponentIndex, mLowLatIndex, mLowLonIndex);
   float lr = mFields.GetLongitude(mComponentIndex, mLowLatIndex, mHighLonIndex);
   float ul = mFields.GetLongitude(mComponentIndex, mHighLatIndex, mLowLonIndex);
   float ur = mFields.GetLongitude(mComponentIndex, mHighLatIndex, mHighLonIndex);
   if (ll * lr < 0.0 || ll * ul < 0.0 || ll * ur < 0.0)
   {
      // We cross the coordinate branch, so map into 0->360;
      ll = static_cast<float>(UtMath::NormalizeAngle0_360(ll));
      lr = static_cast<float>(UtMath::NormalizeAngle0_360(lr));
      ul = static_cast<float>(UtMath::NormalizeAngle0_360(ul));
      ur = static_cast<float>(UtMath::NormalizeAngle0_360(ur));
   }
   std::vector<float> vals{ll, lr, ul, ur};
   std::sort(vals.begin(), vals.end());
   return vals[vals.size() - 1] - vals[0];
}

void Patch::SetupIndices()
{
   auto latLimit = mFields.GetNumLats(mComponentIndex) - 1;
   auto lonLimit = mFields.GetNumLons(mComponentIndex) - 1;
   if (mLowLatIndex > latLimit)
   {
      mLowLatIndex = latLimit;
   }
   if (mLowLonIndex > lonLimit)
   {
      mLowLonIndex = lonLimit;
   }
   mHighLatIndex = mLowLatIndex + 1;
   mHighLonIndex = mLowLonIndex + 1;
   if (mHighLatIndex > latLimit)
   {
      mHighLatIndex = latLimit;
   }
   if (mHighLonIndex > lonLimit)
   {
      mHighLonIndex = lonLimit;
   }
}

float Patch::Interpolate(float       aValueLL,
                         float       aValueLR,
                         float       aValueUL,
                         float       aValueUR,
                         std::size_t aLatIndex,
                         std::size_t aLonIndex) const
{
   float jFactor = static_cast<float>(aLatIndex) / (mNumLatDivisions - 1);
   float iFactor = static_cast<float>(aLonIndex) / (mNumLonDivisions - 1);

   float llFac = (1.0f - iFactor) * (1.0f - jFactor);
   float lrFac = iFactor * (1.0f - jFactor);
   float ulFac = jFactor * (1.0f - iFactor);
   float urFac = iFactor * jFactor;

   return aValueLL * llFac + aValueLR * lrFac + aValueUL * ulFac + aValueUR * urFac;
}

} // namespace CoverageOverlay
