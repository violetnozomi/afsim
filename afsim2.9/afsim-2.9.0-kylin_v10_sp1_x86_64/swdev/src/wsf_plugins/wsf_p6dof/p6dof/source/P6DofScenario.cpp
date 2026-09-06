// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "P6DofScenario.hpp"

#include <cassert>
#include <iostream>

#include "P6DofAtmosphere.hpp"
#include "P6DofFreezeFlags.hpp"
#include "P6DofGravity.hpp"
#include "P6DofTerrain.hpp"
#include "P6DofTypeManager.hpp"
#include "P6DofWind.hpp"

P6DofScenario::P6DofScenario(P6DofTypeManager* aTypeManager)
   : mTypeManager(aTypeManager)
   , mFreezeFlags(new P6DofFreezeFlags())
   , mAtmosphere(nullptr)
   , mGravity(nullptr)
   , mTerrain(nullptr)
   , mWind(nullptr)
{
}

P6DofScenario::P6DofScenario(const P6DofScenario& aSrc)
   : mTypeManager(aSrc.mTypeManager->Clone())
   , mFreezeFlags(aSrc.mFreezeFlags)
   , mAtmosphere(aSrc.mAtmosphere->Clone())
   , mGravity(aSrc.mGravity->Clone())
   , mTerrain(aSrc.mTerrain->Clone())
   , mWind(aSrc.mWind->Clone())
{
}

P6DofScenario::~P6DofScenario()
{
   delete mFreezeFlags;

   if (mAtmosphere != nullptr)
   {
      delete mAtmosphere;
   }
   if (mGravity != nullptr)
   {
      delete mGravity;
   }
   if (mTerrain != nullptr)
   {
      delete mTerrain;
   }
   if (mWind != nullptr)
   {
      delete mWind;
   }
}

P6DofScenario* P6DofScenario::Clone() const
{
   return new P6DofScenario(*this);
}
