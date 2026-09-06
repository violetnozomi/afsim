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

#ifndef WSFINHERENTCONTRASTTYPES_HPP
#define WSFINHERENTCONTRASTTYPES_HPP

#include "wsf_mil_export.h"

#include "WsfInherentContrast.hpp"
#include "WsfObjectTypeList.hpp"

class WsfInherentContrastTypes : public WsfObjectTypeList<WsfInherentContrast>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_MIL_EXPORT WsfInherentContrastTypes&       Get(WsfScenario& aScenario);
   static WSF_MIL_EXPORT const WsfInherentContrastTypes& Get(const WsfScenario& aScenario);
   //@}

   WsfInherentContrastTypes(WsfScenario& aScenario);

   bool InitializeType(WsfInherentContrast* aTypePtr) override;

   //! @name Object factory methods.
   //@{
   using FactoryPtr = WsfInherentContrast* (*)(const std::string&);
   WSF_MIL_EXPORT void AddObjectFactory(FactoryPtr aFactoryPtr);

   WsfInherentContrast* ObjectFactory(const std::string& aTypeName);
   //@}

private:
   using ObjectFactoryList = std::vector<FactoryPtr>;
   ObjectFactoryList mObjectFactoryList; //!< The list of object factories
};

#endif
