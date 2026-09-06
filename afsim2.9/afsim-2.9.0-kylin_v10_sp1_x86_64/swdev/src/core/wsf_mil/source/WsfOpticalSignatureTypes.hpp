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

#ifndef WSFOPTICALSIGNATURETYPES_HPP
#define WSFOPTICALSIGNATURETYPES_HPP

#include "wsf_mil_export.h"

#include "WsfObjectTypeList.hpp"
#include "WsfOpticalSignature.hpp"

class WsfOpticalSignatureTypes : public WsfObjectTypeList<WsfOpticalSignature>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_MIL_EXPORT WsfOpticalSignatureTypes&       Get(WsfScenario& aScenario);
   static WSF_MIL_EXPORT const WsfOpticalSignatureTypes& Get(const WsfScenario& aScenario);
   //@}

   WsfOpticalSignatureTypes(WsfScenario& aScenario);

   bool InitializeType(WsfOpticalSignature* aTypePtr) override;

   //! @name Object factory methods.
   //@{
   using FactoryPtr = WsfOpticalSignature* (*)(const std::string&);
   WSF_MIL_EXPORT void AddObjectFactory(FactoryPtr aFactoryPtr);

   WsfOpticalSignature* ObjectFactory(const std::string& aTypeName);
   //@}

private:
   using ObjectFactoryList = std::vector<FactoryPtr>;
   ObjectFactoryList mObjectFactoryList; //!< The list of object factories
};

#endif
