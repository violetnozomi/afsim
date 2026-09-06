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

#ifndef WSFMASKINGPATTERNTYPES_HPP
#define WSFMASKINGPATTERNTYPES_HPP

#include "wsf_export.h"

#include "WsfMaskingPattern.hpp"
#include "WsfObjectTypeList.hpp"

class WsfMaskingPatternTypes : public WsfObjectTypeList<WsfMaskingPattern>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_EXPORT WsfMaskingPatternTypes&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const WsfMaskingPatternTypes& Get(const WsfScenario& aScenario);
   //@}

   WsfMaskingPatternTypes(WsfScenario& aScenario);
   bool InitializeType(ObjectType* aTypePtr) override;
};

#endif
