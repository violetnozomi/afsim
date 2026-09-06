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

#include "TestMeasure.hpp"

#include "WsfCoverage.hpp"

double TestMeasure::GetMeasuredValue(const wsf::coverage::GridAsset& aGridAsset) const
{
   double      retval{0.0};
   const auto& iter = mData.find(aGridAsset);
   if (iter != mData.cend())
   {
      retval = iter->second;
   }
   return retval;
}

int TestMeasure::NumOutputsOfType(const std::string& aOutputType) const
{
   int retval{0};
   for (std::size_t i = 0; i < GetNumOutput(); ++i)
   {
      if (GetOutput(i)->GetOutputType() == aOutputType)
      {
         ++retval;
      }
   }
   return retval;
}

int TestMeasure::GetNumIntervalStarts(const wsf::coverage::GridAsset& aGridAsset,
                                      const wsf::coverage::FreeAsset& aFreeAsset) const
{
   int  retval{0};
   auto gaIter = mIntervalStarts.find(aGridAsset);
   if (gaIter != mIntervalStarts.end())
   {
      auto faIter = gaIter->second.find(aFreeAsset);
      if (faIter != gaIter->second.end())
      {
         retval = faIter->second;
      }
   }
   return retval;
}

int TestMeasure::GetNumIntervalEnds(const wsf::coverage::GridAsset& aGridAsset,
                                    const wsf::coverage::FreeAsset& aFreeAsset) const
{
   int  retval{0};
   auto gaIter = mIntervalEnds.find(aGridAsset);
   if (gaIter != mIntervalEnds.end())
   {
      auto faIter = gaIter->second.find(aFreeAsset);
      if (faIter != gaIter->second.end())
      {
         retval = faIter->second;
      }
   }
   return retval;
}

int TestMeasure::GetNumIntervalCompletions(const wsf::coverage::GridAsset& aGridAsset,
                                           const wsf::coverage::FreeAsset& aFreeAsset) const
{
   int  retval{0};
   auto gaIter = mIntervalCompletions.find(aGridAsset);
   if (gaIter != mIntervalCompletions.end())
   {
      auto faIter = gaIter->second.find(aFreeAsset);
      if (faIter != gaIter->second.end())
      {
         retval = faIter->second;
      }
   }
   return retval;
}

bool TestMeasure::InitializeP(wsf::coverage::Coverage& aCoverage)
{
   mCallbacks.Add(aCoverage.AccessIntervalStart().Connect(&TestMeasure::OnIntervalStart, this));
   mCallbacks.Add(aCoverage.AccessIntervalEnd().Connect(&TestMeasure::OnIntervalEnd, this));
   mCallbacks.Add(aCoverage.AccessIntervalComplete().Connect(&TestMeasure::OnIntervalComplete, this));
   return true;
}

void TestMeasure::OnIntervalStart(const wsf::coverage::GridAsset& aGridAsset,
                                  const wsf::coverage::FreeAsset& aFreeAsset,
                                  double                          aSimTime)
{
   auto gaIter = mIntervalStarts.find(aGridAsset);
   if (gaIter == mIntervalStarts.end())
   {
      auto result = mIntervalStarts.emplace(aGridAsset, std::map<wsf::coverage::FreeAsset, int>{});
      gaIter      = result.first;
   }

   auto faIter = gaIter->second.find(aFreeAsset);
   if (faIter == gaIter->second.end())
   {
      auto result = gaIter->second.emplace(aFreeAsset, 0);
      faIter      = result.first;
   }

   faIter->second += 1;
}

void TestMeasure::OnIntervalEnd(const wsf::coverage::GridAsset& aGridAsset,
                                const wsf::coverage::FreeAsset& aFreeAsset,
                                double                          aSimTime)
{
   auto gaIter = mIntervalEnds.find(aGridAsset);
   if (gaIter == mIntervalEnds.end())
   {
      auto result = mIntervalEnds.emplace(aGridAsset, std::map<wsf::coverage::FreeAsset, int>{});
      gaIter      = result.first;
   }

   auto faIter = gaIter->second.find(aFreeAsset);
   if (faIter == gaIter->second.end())
   {
      auto result = gaIter->second.emplace(aFreeAsset, 0);
      faIter      = result.first;
   }

   faIter->second += 1;
}

void TestMeasure::OnIntervalComplete(const wsf::coverage::GridAsset&      aGridAsset,
                                     const wsf::coverage::FreeAsset&      aFreeAsset,
                                     const wsf::coverage::AccessInterval& aAccessInterval)
{
   auto gaIter = mIntervalCompletions.find(aGridAsset);
   if (gaIter == mIntervalCompletions.end())
   {
      auto result = mIntervalCompletions.emplace(aGridAsset, std::map<wsf::coverage::FreeAsset, int>{});
      gaIter      = result.first;
   }

   auto faIter = gaIter->second.find(aFreeAsset);
   if (faIter == gaIter->second.end())
   {
      auto result = gaIter->second.emplace(aFreeAsset, 0);
      faIter      = result.first;
   }

   faIter->second += 1;
}
