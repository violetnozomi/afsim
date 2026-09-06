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

#ifndef WSFCORRELATIONSTRATEGYTYPES_HPP
#define WSFCORRELATIONSTRATEGYTYPES_HPP

#include "WsfCorrelationStrategy.hpp"
#include "WsfObjectTypeList.hpp"

class WsfCorrelationStrategyTypes : public WsfObjectTypeList<WsfCorrelationStrategy>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_EXPORT WsfCorrelationStrategyTypes&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const WsfCorrelationStrategyTypes& Get(const WsfScenario& aScenario);
   //@}

   WsfCorrelationStrategyTypes(WsfScenario& aScenario);

   WSF_EXPORT WsfCorrelationStrategy* Create(const std::string& aName) const;

   WSF_EXPORT bool LoadInstance(UtInput& aInput, WsfCorrelationStrategy*& aStrategyPtr) const;

   WSF_EXPORT const std::string& GetDefaultStrategyName() const;
   WSF_EXPORT void               SetDefaultStrategyName(const std::string& aName);

   bool        ProcessInput(UtInput& aInput) override; // To configure the default strategy name.
   std::string mDefaultStrategyName;
};

#endif
