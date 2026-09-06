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

#include "TestCoverage.hpp"

void TestCoverage::InjectIntervalStart(const wsf::coverage::GridAsset& aGridAsset,
                                       const wsf::coverage::FreeAsset& aFreeAsset,
                                       double                          aSimTime)
{
   AddIntervalStartToData(aGridAsset, aFreeAsset, aSimTime);
}

void TestCoverage::InjectIntervalEnd(const wsf::coverage::GridAsset& aGridAsset,
                                     const wsf::coverage::FreeAsset& aFreeAsset,
                                     double                          aSimTime)
{
   AddIntervalEndToData(aGridAsset, aFreeAsset, aSimTime);
}

void TestCoverage::InjectInterval(const wsf::coverage::GridAsset& aGridAsset,
                                  const wsf::coverage::FreeAsset& aFreeAsset,
                                  double                          aStartTime,
                                  double                          aEndTime)
{
   InjectIntervalStart(aGridAsset, aFreeAsset, aStartTime);
   InjectIntervalEnd(aGridAsset, aFreeAsset, aEndTime);
}
