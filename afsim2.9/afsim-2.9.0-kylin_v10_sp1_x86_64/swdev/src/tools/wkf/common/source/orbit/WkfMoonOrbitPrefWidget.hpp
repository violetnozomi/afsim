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
#ifndef WKFLUNARORBITPREFWIDGET_HPP
#define WKFLUNARORBITPREFWIDGET_HPP

#include "wkf_common_export.h"

#include "WkfMoonOrbitPrefObject.hpp"
#include "WkfPrefWidget.hpp"
#include "ui_WkfMoonOrbitPrefWidget.h"

class QCheckBox;
class QSpinBox;
class UtQtColorButton;

namespace wkf
{
class WKF_COMMON_EXPORT MoonOrbitPrefWidget : public wkf::PrefWidgetT<MoonOrbitPrefObject>
{
public:
   MoonOrbitPrefWidget();
   ~MoonOrbitPrefWidget() override = default;

   QString GetCategoryHint() const override { return "Space"; }

private:
   void ReadPreferenceData(const MoonOrbitPrefData& aPrefData) override;
   void WritePreferenceData(MoonOrbitPrefData& aPrefData) override;

   Ui::MoonOrbitPrefWidget mUi;
};
} // namespace wkf
#endif
