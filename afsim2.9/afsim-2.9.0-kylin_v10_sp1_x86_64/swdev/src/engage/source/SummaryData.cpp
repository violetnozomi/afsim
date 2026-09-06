// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "SummaryData.hpp"

#include <fstream>
#include <iostream>

#include "InputConfig.hpp"
#include "WsfScenario.hpp"
#include "WsfWeaponEffects.hpp"
#include "WsfWeaponEngagement.hpp"

namespace engage
{

// =================================================================================================
SummaryData::SummaryData()
   : mShotCount(0)
   , mNonZeroPkShotCount(0)
   , mNonZeroPkSum(0.0)
{
}


void SummaryData::EngagementResult(double aSimtime, const WsfWeaponEngagement* aEngagementPtr)
{
   mShotCount++;

   double Pk = aEngagementPtr->GetWeaponEffects()->GetInterceptPk();
   if (Pk > 0.0)
   {
      mNonZeroPkShotCount++;
      mNonZeroPkSum += Pk;
   }
}

} // namespace engage
