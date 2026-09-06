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

#ifndef WSFFUSIONSTRATEGYTYPES_HPP
#define WSFFUSIONSTRATEGYTYPES_HPP

#include "wsf_export.h"

#include "WsfFusionStrategy.hpp"

class WsfFusionStrategyTypes : public WsfObjectTypeList<WsfFusionStrategy>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_EXPORT WsfFusionStrategyTypes&       Get(WsfScenario& aScenario);
   static const WSF_EXPORT WsfFusionStrategyTypes& Get(const WsfScenario& aScenario);
   //@}

   WsfFusionStrategyTypes(WsfScenario& aScenario);

   WSF_EXPORT WsfFusionStrategy* Create(const std::string& aName) const;

   WSF_EXPORT bool LoadInstance(UtInput& aInput, WsfFusionStrategy*& aStrategyPtr) const;

   WSF_EXPORT const std::string& GetDefaultStrategyName() const;
   WSF_EXPORT void               SetDefaultStrategyName(const std::string& aName);

   bool ProcessInput(UtInput& aInput) override; // To configure the default strategy name.

   std::string mDefaultStrategyName;
   static bool mCheckFuseEstimates;
};

#endif
