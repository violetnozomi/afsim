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

#include "TestCoverageGrid.hpp"

#include <algorithm>

TestGrid::TestGrid(const WsfScenario& aScenario, const std::vector<wsf::coverage::Point>& aPoints)
   : Grid(aScenario)
{
   if (aPoints.size())
   {
      mPoints = aPoints;
   }
   SetUsedFlag(); // Do this manually, or this will not initialize.
}

const wsf::coverage::Point* TestGrid::GetPoint(wsf::coverage::PointId aPointId) const
{
   const wsf::coverage::Point* retvalPtr{nullptr};
   auto                        iter = std::find_if(mPoints.begin(),
                            mPoints.end(),
                            [aPointId](const wsf::coverage::Point& aPoint) { return aPoint.GetID() == aPointId; });
   if (iter != mPoints.end())
   {
      retvalPtr = &(*iter);
   }
   return retvalPtr;
}

void TestGrid::Invoke(wsf::coverage::GridAction& aAction) const
{
   for (const auto& point : mPoints)
   {
      aAction.Call(*this, point);
   }
}

wsf::coverage::GridAsset TestGrid::GetAsset(wsf::coverage::PointId aPointId) const
{
   return GetAsset(GetPlatformName(aPointId));
}

wsf::coverage::GridAsset TestGrid::GetAsset(const std::string& aPlatformName) const
{
   wsf::coverage::GridAsset retval{"", wsf::coverage::cINVALID_POINT_ID, ""};
   auto                     iter = mAssets.find(aPlatformName);
   if (iter != mAssets.end())
   {
      retval = iter->second;
   }
   return retval;
}

void TestGrid::HasNullDevice(bool& aAnyNull, bool& aAllNull) const
{
   aAnyNull = true;
   aAllNull = true;
}

bool TestGrid::InitializeP()
{
   if (mPoints.size() == 0)
   {
      mPoints.emplace_back(GetNextAvailableId(), 0.0, 0.0, 0.0);
      mPoints.emplace_back(GetNextAvailableId(), 0.0, 90.0, 0.0);
      mPoints.emplace_back(GetNextAvailableId(), 45.0, 90.0, 0.0);
      mPoints.emplace_back(GetNextAvailableId(), 45.0, 0.0, 0.0);
   }
   for (auto& point : mPoints)
   {
      std::string platName = GetPlatformName(point.GetID());
      mAssets.emplace(platName, wsf::coverage::GridAsset{platName, point.GetID(), "none"});
   }
   return true;
}

std::string TestGrid::GetPlatformName(wsf::coverage::PointId aPointId) const
{
   return GetName() + std::to_string(aPointId);
}
