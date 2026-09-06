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

#ifndef WSFINFRAREDSIGNATURETYPES_HPP
#define WSFINFRAREDSIGNATURETYPES_HPP

#include "wsf_mil_export.h"

#include "WsfInfraredSignature.hpp"
#include "WsfObjectTypeList.hpp"

class WsfInfraredSignatureTypes : public WsfObjectTypeList<WsfInfraredSignature>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_MIL_EXPORT WsfInfraredSignatureTypes&       Get(WsfScenario& aScenario);
   static WSF_MIL_EXPORT const WsfInfraredSignatureTypes& Get(const WsfScenario& aScenario);
   //@}

   WsfInfraredSignatureTypes(WsfScenario& aScenario);

   bool InitializeType(WsfInfraredSignature* aTypePtr) override;

   //! @name Object factory methods.
   //@{
   using FactoryPtr = WsfInfraredSignature* (*)(const std::string&);
   WSF_MIL_EXPORT void AddObjectFactory(FactoryPtr aFactoryPtr);

   WsfInfraredSignature* ObjectFactory(const std::string& aTypeName);
   //@}

private:
   using ObjectFactoryList = std::vector<FactoryPtr>;
   ObjectFactoryList mObjectFactoryList; //!< The list of object factories
};

#endif
