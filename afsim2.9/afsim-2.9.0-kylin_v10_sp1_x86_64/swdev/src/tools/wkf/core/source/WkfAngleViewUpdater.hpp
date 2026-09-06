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

#ifndef ANGLEVIEWUPDATER_HPP
#define ANGLEVIEWUPDATER_HPP

#include "wkf_export.h"

#include "AngleEntry.hpp"
#include "WkfUnitUpdaters.hpp"

namespace wkf
{
class WKF_EXPORT AngleViewUpdater : public AngleValueUpdater
{
public:
   AngleViewUpdater(const QString& aUpdaterName, const std::string& aPlatformName, const vespa::AngleEntry* aAngle);

protected:
   virtual double GetValueInDefaultUnits() const override;

private:
   const vespa::AngleEntry* mAngle;
};
} // namespace wkf

#endif
