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

#ifndef WSFACOUSTICSIGNATURETYPES_HPP
#define WSFACOUSTICSIGNATURETYPES_HPP

#include "wsf_mil_export.h"

#include "WsfAcousticSignature.hpp"
#include "WsfObjectTypeList.hpp"

class WsfAcousticSignatureTypes : public WsfObjectTypeList<WsfAcousticSignature>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_MIL_EXPORT WsfAcousticSignatureTypes&       Get(WsfScenario& aScenario);
   static WSF_MIL_EXPORT const WsfAcousticSignatureTypes& Get(const WsfScenario& aScenario);
   //@}

   WsfAcousticSignatureTypes(WsfScenario& aScenario);

   bool InitializeType(WsfAcousticSignature* aTypePtr) override;

   //! @name Object factory methods.
   //@{
   using FactoryPtr = WsfAcousticSignature* (*)(const std::string&);
   WSF_MIL_EXPORT void AddObjectFactory(FactoryPtr aFactoryPtr);

   WsfAcousticSignature* ObjectFactory(const std::string& aTypeName);
   //@}

private:
   using ObjectFactoryList = std::vector<FactoryPtr>;
   ObjectFactoryList mObjectFactoryList; //!< The list of object factories
};

#endif
