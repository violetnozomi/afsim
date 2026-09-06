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

#ifndef WSFANTENNAPATTERNTYPES_HPP
#define WSFANTENNAPATTERNTYPES_HPP

#include <vector>

#include "WsfAntennaPattern.hpp"
#include "WsfObjectTypeList.hpp"

//! The list of defined antenna_pattern types.
class WsfAntennaPatternTypes : public WsfObjectTypeList<WsfAntennaPattern>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_EXPORT WsfAntennaPatternTypes&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const WsfAntennaPatternTypes& Get(const WsfScenario& aScenario);
   //@}

   typedef WsfAntennaPattern* (*FactoryPtr)(const std::string&);
   WsfAntennaPatternTypes(WsfScenario& aScenario);
   WSF_EXPORT void AddObjectFactory(FactoryPtr aFactoryPtr);
   bool            InitializeType(WsfAntennaPattern* aObjectPtr) override;

private:
   typedef std::vector<FactoryPtr> ObjectFactoryList;
   ObjectFactoryList               mObjectFactoryList;
};

#endif
