// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef RVSCRIPTDATAUPDATER_HPP
#define RVSCRIPTDATAUPDATER_HPP

#include "WkfUpdater.hpp"

namespace RvScriptDataFeed
{
class Updater : public wkf::UnitlessValueUpdater
{
public:
   Updater(const QString& aPlatformName, const QString& aDatum);
   QString GetValueString() const override;
};
} // namespace RvScriptDataFeed

#endif
