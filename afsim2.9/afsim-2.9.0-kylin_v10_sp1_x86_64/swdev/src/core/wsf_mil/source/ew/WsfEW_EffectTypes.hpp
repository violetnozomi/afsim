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

#ifndef WSFEW_EFFECTTYPES_HPP
#define WSFEW_EFFECTTYPES_HPP

#include "WsfEW_Effect.hpp"
#include "WsfObjectTypeList.hpp"
class WsfEW_Technique;

class WsfEW_EffectTypes : public WsfObjectTypeList<WsfEW_Effect>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_MIL_EXPORT WsfEW_EffectTypes&       Get(WsfScenario& aScenario);
   static WSF_MIL_EXPORT const WsfEW_EffectTypes& Get(const WsfScenario& aScenario);
   //@}

   explicit WsfEW_EffectTypes(WsfScenario& aScenario);

   bool LoadInstance(UtInput& aInput, WsfEW_Technique* aTechniquePtr);
};

#endif
