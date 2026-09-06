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

#ifndef WKFVISUALEFFECTSPREFERENCESWIDGET_HPP
#define WKFVISUALEFFECTSPREFERENCESWIDGET_HPP

#include "wkf_common_export.h"

#include "WkfPrefWidget.hpp"
#include "WkfVisualEffectsPrefObject.hpp"
#include "ui_WkfVisualEffectsPrefWidget.h"

namespace wkf
{
class WKF_COMMON_EXPORT VisualEffectsPrefWidget : public wkf::PrefWidgetT<VisualEffectsPrefObject>
{
public:
   VisualEffectsPrefWidget();
   ~VisualEffectsPrefWidget() override = default;

   QString GetCategoryHint() const override { return "Map"; }

private:
   void ReadPreferenceData(const VisualEffectsPrefData& aPrefData) override;
   void WritePreferenceData(VisualEffectsPrefData& aPrefData) override;

   Ui::WkfVisualEffectsPrefWidget mUi;
};
} // namespace wkf

#endif // !WKFVISUALEFFECTSPREFERENCESWIDGET_HPP
