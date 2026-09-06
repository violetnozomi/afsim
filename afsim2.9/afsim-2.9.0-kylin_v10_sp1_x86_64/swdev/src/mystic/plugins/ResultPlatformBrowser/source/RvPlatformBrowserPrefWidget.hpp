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

#ifndef RVPLATFORMBROWSERPREFWIDGET_HPP
#define RVPLATFORMBROWSERPREFWIDGET_HPP

#include "RvPlatformBrowserPrefObject.hpp"
#include "WkfPrefWidget.hpp"
#include "ui_RvPlatformBrowserPrefWidget.h"

namespace RvPlatformBrowser
{
class PrefWidget : public wkf::PrefWidgetT<PrefObject>
{
   Q_OBJECT

public:
   PrefWidget();
   ~PrefWidget() override = default;

private:
   void ReadPreferenceData(const PrefData& aPrefData) override;
   void WritePreferenceData(PrefData& aPrefData) override;

   Ui::RvPlatformBrowserPrefWidget mUi;
};
} // namespace RvPlatformBrowser
#endif
