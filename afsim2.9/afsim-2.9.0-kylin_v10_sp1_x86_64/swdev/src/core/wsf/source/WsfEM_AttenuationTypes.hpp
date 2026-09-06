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

#ifndef WSFEM_ATTENUATIONTYPES_HPP
#define WSFEM_ATTENUATIONTYPES_HPP

#include <functional>
#include <list>

#include "WsfEM_Attenuation.hpp"
#include "WsfObjectTypeList.hpp"

class WsfEM_AttenuationTypes : public WsfObjectTypeList<WsfEM_Attenuation>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_EXPORT WsfEM_AttenuationTypes&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const WsfEM_AttenuationTypes& Get(const WsfScenario& aScenario);
   //@}

   typedef std::function<WsfEM_Attenuation*(const std::string&)> Factory;

   WsfEM_AttenuationTypes(WsfScenario& aScenario);
   typedef std::list<Factory> ObjectFactoryList;

   WSF_EXPORT void    AddObjectFactory(const Factory& aFactory);
   LoadResult         LoadType(UtInput& aInput) override;
   bool               LoadReference(UtInput& aInput, WsfStringId& aTypeName);
   WsfEM_Attenuation* Clone(WsfStringId aTypeName) const override;

private:
   WsfEM_Attenuation* CreateInstance(const std::string& aTypeName) const;

   // The list of object factories.
   ObjectFactoryList mObjectFactoryList;
   int               mUniqueId;
};

#endif
