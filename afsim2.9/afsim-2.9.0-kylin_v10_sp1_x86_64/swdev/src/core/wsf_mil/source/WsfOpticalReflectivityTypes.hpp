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

#ifndef WSFOPTICALFREFLECTIVITYTYPES_HPP
#define WSFOPTICALFREFLECTIVITYTYPES_HPP

#include "wsf_mil_export.h"

#include "WsfObjectTypeList.hpp"
#include "WsfOpticalReflectivity.hpp"

class WsfOpticalReflectivityTypes : public WsfObjectTypeList<WsfOpticalReflectivity>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_MIL_EXPORT WsfOpticalReflectivityTypes&       Get(WsfScenario& aScenario);
   static WSF_MIL_EXPORT const WsfOpticalReflectivityTypes& Get(const WsfScenario& aScenario);
   //@}

   WsfOpticalReflectivityTypes(WsfScenario& aScenario);

   bool InitializeType(WsfOpticalReflectivity* aTypePtr) override;

   //! @name Object factory methods.
   //@{
   using FactoryPtr = WsfOpticalReflectivity* (*)(const std::string&);
   WSF_MIL_EXPORT void AddObjectFactory(FactoryPtr aFactoryPtr);

   WsfOpticalReflectivity* ObjectFactory(const std::string& aTypeName);
   //@}

private:
   using ObjectFactoryList = std::vector<FactoryPtr>;
   ObjectFactoryList mObjectFactoryList; //!< The list of object factories
};

#endif
