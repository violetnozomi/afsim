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
#ifndef RV_AUX_DATA_UPDATERS_HPP
#define RV_AUX_DATA_UPDATERS_HPP

#include "RvPlatformUpdater.hpp"

namespace rv
{
class ResultPlatform;
}

namespace RvAuxData
{
class Updater : public rv::PlatformUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Aux Data";

   Updater(const std::string& aPlatformName, const std::string& aDatumName);

   QString       GetValueString() const override;
   const QString GetDisplayName() const override;

protected:
   bool     ReadData(rv::ResultPlatform* aPlatform, double aSimTime) override;
   QString  mDatumName;
   QVariant mValue;
};
} // namespace RvAuxData
#endif
