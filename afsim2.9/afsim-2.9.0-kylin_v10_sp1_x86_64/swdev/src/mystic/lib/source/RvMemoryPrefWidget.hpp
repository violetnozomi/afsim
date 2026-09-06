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
#ifndef RVMEMORYPREFWIDGET_HPP
#define RVMEMORYPREFWIDGET_HPP

#include "RvMemoryPrefObject.hpp"
#include "WkfPrefWidget.hpp"
#include "ui_RvMemoryPrefWidget.h"

namespace rv
{
class MemoryPrefWidget : public wkf::PrefWidgetT<MemoryPrefObject>
{
public:
   MemoryPrefWidget(QWidget* parent = nullptr);

   QString GetCategoryHint() const override { return "Application"; }

private:
   void showEvent(QShowEvent* aEvent) override;
   void ReadPreferenceData(const MemoryPrefData& aPrefData) override;
   void WritePreferenceData(MemoryPrefData& aPrefData) override;

   Ui::RvMemoryPrefWidget mUi;
};
} // namespace rv
#endif
