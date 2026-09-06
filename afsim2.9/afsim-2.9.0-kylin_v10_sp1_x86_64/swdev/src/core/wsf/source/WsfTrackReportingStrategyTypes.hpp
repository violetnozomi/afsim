// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFTRACKREPORTINGSTRATEGYTYPES_HPP
#define WSFTRACKREPORTINGSTRATEGYTYPES_HPP

#include "WsfObjectTypeList.hpp"
#include "WsfTrackReportingStrategy.hpp"

class WsfTrackReportingStrategyTypes : public WsfObjectTypeList<WsfTrackReportingStrategy>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_EXPORT WsfTrackReportingStrategyTypes&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const WsfTrackReportingStrategyTypes& Get(const WsfScenario& aScenario);
   //@}

   explicit WsfTrackReportingStrategyTypes(WsfScenario& aScenario);
   WSF_EXPORT WsfTrackReportingStrategy* Create(WsfStringId aName, WsfTrackProcessor& aTrackProcessor) const;
};

#endif
