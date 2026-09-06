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

#ifndef WSFEM_CLUTTERTYPES_HPP
#define WSFEM_CLUTTERTYPES_HPP

#include <vector>

#include "WsfEM_Clutter.hpp"
#include "WsfObjectTypeList.hpp"

class WsfEM_ClutterTypes : public WsfObjectTypeList<WsfEM_Clutter>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_EXPORT WsfEM_ClutterTypes&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const WsfEM_ClutterTypes& Get(const WsfScenario& aScenario);
   //@}

   typedef WsfEM_Clutter* (*FactoryPtr)(const std::string&);

   WsfEM_ClutterTypes(WsfScenario& aScenario);

   LoadResult LoadType(UtInput& aInput) override;

   WSF_EXPORT void AddObjectFactory(FactoryPtr aFactoryPtr);

   WSF_EXPORT bool LoadReference(UtInput& aInput, WsfStringId& aTypeName);

private:
   WsfEM_Clutter* CreateInstance(const std::string& aTypeName);

   // The list of object factories.
   typedef std::vector<FactoryPtr> ObjectFactoryList;
   ObjectFactoryList               mObjectFactoryList;
   int                             mUniqueId;
};

#endif
