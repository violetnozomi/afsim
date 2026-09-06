// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************


#include "WkfAngleViewUpdater.hpp"

namespace wkf
{
AngleViewUpdater::AngleViewUpdater(const QString&           aUpdaterName,
                                   const std::string&       aPlatformName,
                                   const vespa::AngleEntry* aAngle)
   : AngleValueUpdater(aUpdaterName, aPlatformName)
   , mAngle{aAngle}
{
}

double AngleViewUpdater::GetValueInDefaultUnits() const
{
   if (mAngle)
   {
      return mAngle->GetAngle();
   }
   return mValue;
}
} // namespace wkf
