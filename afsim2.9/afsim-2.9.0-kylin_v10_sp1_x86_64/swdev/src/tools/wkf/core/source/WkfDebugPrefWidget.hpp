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

#ifndef WKFDEBUGPREFWIDGET_HPP
#define WKFDEBUGPREFWIDGET_HPP

#include "WkfDebugPrefObject.hpp"
#include "WkfPrefWidget.hpp"
#include "ui_WkfDebugPrefWidget.h"

namespace wkf
{
class WKF_EXPORT DebugPrefWidget : public PrefWidgetT<DebugPrefObject>
{
public:
   DebugPrefWidget(QWidget* parent = nullptr);
   ~DebugPrefWidget() override = default;

private:
   void showEvent(QShowEvent* aEvent) override;
   void ReadPreferenceData(const DebugPrefData& aPrefData) override;
   void WritePreferenceData(DebugPrefData& aPrefData) override;

   Ui::WkfDebugPrefWidget mUi;
};
} // namespace wkf

#endif
