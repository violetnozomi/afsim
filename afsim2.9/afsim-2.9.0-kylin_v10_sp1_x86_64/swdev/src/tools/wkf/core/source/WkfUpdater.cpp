// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfUpdater.hpp"

wkf::Updater::Updater(const QString& aUpdaterName, const std::string& aPlatformName)
   : mName(aUpdaterName)
   , mPlatformOfInterest(aPlatformName)
{
}

QString wkf::UnitlessValueUpdater::GetValueString() const
{
   return QString::number(GetValueDouble(), mFormat, mPrecision);
}

double wkf::UnitlessValueUpdater::GetValueDouble() const
{
   return mValue;
}
