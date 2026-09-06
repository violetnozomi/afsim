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

#ifndef WSFRADARSIGNATURETYPES_HPP
#define WSFRADARSIGNATURETYPES_HPP

#include "WsfObjectTypeList.hpp"
#include "WsfRadarSignature.hpp"

class WsfRadarSignatureTypes : public WsfObjectTypeList<WsfRadarSignature>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_EXPORT WsfRadarSignatureTypes&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const WsfRadarSignatureTypes& Get(const WsfScenario& aScenario);
   //@}

   WsfRadarSignatureTypes(WsfScenario& aScenario);

   bool InitializeType(WsfRadarSignature* aTypePtr) override;

   //! @name Object factory methods.
   //@{
   typedef WsfRadarSignature* (*FactoryPtr)(const std::string&);
   WSF_EXPORT void AddObjectFactory(FactoryPtr aFactoryPtr);

   WsfRadarSignature* ObjectFactory(const std::string& aTypeName);
   //@}

private:
   typedef std::vector<FactoryPtr> ObjectFactoryList;
   ObjectFactoryList               mObjectFactoryList; //!< The list of object factories
};

#endif
