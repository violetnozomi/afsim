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

#ifndef WkPrefWidgetZoneBrowser_HPP
#define WkPrefWIdgetZoneBrowser_HPP

#include "wkf_common_export.h"

#include "UtQtColorButton.hpp"
#include "WkfPrefWidget.hpp"
#include "WkfZoneBrowserPrefObject.hpp"
#include "ui_WkfZoneBrowserPrefWidget.h"

namespace wkf
{
class WKF_COMMON_EXPORT ZoneBrowserPrefWidget : public wkf::PrefWidgetT<ZoneBrowserPrefObject>
{
public:
   ZoneBrowserPrefWidget();
   ~ZoneBrowserPrefWidget() override = default;

   QString GetCategoryHint() const override { return "Map"; }

private:
   void ReadPreferenceData(const ZoneBrowserPrefData& aPrefData) override;
   void WritePreferenceData(ZoneBrowserPrefData& aPrefData) override;
   void EnableColorButtons(bool aEnable);

   Ui::ZoneBrowserPrefWidget mUi;
   UtQtColorButton*          mLineColorButtonPtr;
   UtQtColorButton*          mFillColorButtonPtr;
};
} // namespace wkf

#endif
