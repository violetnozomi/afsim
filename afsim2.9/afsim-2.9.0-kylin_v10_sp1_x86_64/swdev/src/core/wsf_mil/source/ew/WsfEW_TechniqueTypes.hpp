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

#ifndef WSFEW_TECHNIQUETYPES_HPP
#define WSFEW_TECHNIQUETYPES_HPP

#include "WsfEW_Technique.hpp"
#include "WsfObjectTypeList.hpp"

class WsfEW_TechniqueTypes : public WsfObjectTypeList<WsfEW_Technique>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_MIL_EXPORT WsfEW_TechniqueTypes&       Get(WsfScenario& aScenario);
   static WSF_MIL_EXPORT const WsfEW_TechniqueTypes& Get(const WsfScenario& aScenario);
   //@}

   explicit WsfEW_TechniqueTypes(WsfScenario& aScenario);

   WSF_MIL_EXPORT bool LoadInstance(UtInput& aInput, WsfEW_EA_EP* aEA_EP_Ptr) const;
};

#endif
