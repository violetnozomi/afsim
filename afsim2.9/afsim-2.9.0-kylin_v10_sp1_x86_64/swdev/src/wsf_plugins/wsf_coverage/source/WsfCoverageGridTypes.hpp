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

#ifndef WSFCOVERAGEGRIDTYPES_HPP
#define WSFCOVERAGEGRIDTYPES_HPP

#include "wsf_coverage_export.h"

#include "WsfCoverageGrid.hpp"
#include "WsfObjectTypeList.hpp"

namespace wsf
{
namespace coverage
{

class GridTypes : public WsfObjectTypeList<Grid>
{
public:
   static WSF_COVERAGE_EXPORT GridTypes& Get(const WsfScenario& aScenario);

   WSF_COVERAGE_EXPORT explicit GridTypes(WsfScenario& aScenario);
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGEGRIDTYPES_HPP
