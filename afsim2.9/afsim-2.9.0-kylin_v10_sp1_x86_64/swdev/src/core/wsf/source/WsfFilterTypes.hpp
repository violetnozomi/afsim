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

#ifndef WSFFILTERTYPES_HPP
#define WSFFILTERTYPES_HPP

#include "wsf_export.h"

#include "WsfFilter.hpp"
#include "WsfObjectTypeList.hpp"

class WsfFilterTypes : public WsfObjectTypeList<WsfFilter>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_EXPORT WsfFilterTypes&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const WsfFilterTypes& Get(const WsfScenario& aScenario);
   //@}

   WsfFilterTypes(WsfScenario& aScenario);

   WSF_EXPORT void Add(const std::string& aName, WsfFilter* aFilter);
   WSF_EXPORT bool LoadInstance(UtInput& aInput, WsfFilter*& aFilterPtr) const;
};

#endif
