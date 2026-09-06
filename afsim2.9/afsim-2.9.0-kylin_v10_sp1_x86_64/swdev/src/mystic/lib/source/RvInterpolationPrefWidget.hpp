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
#ifndef RVINTERPOLATIONPREFWIDGET_HPP
#define RVINTERPOLATIONPREFWIDGET_HPP

#include "RvInterpolationPrefObject.hpp"
#include "WkfPrefWidget.hpp"
#include "ui_RvInterpolationPrefWidget.h"

namespace rv
{
class InterpolationPrefWidget : public wkf::PrefWidgetT<InterpolationPrefObject>
{
public:
   InterpolationPrefWidget(QWidget* parent = nullptr);

   QString GetCategoryHint() const override { return "Map"; }

private:
   void showEvent(QShowEvent* aEvent) override;
   void ReadPreferenceData(const InterpolationPrefData& aPrefData) override;
   void WritePreferenceData(InterpolationPrefData& aPrefData) override;

   Ui::RvInterpolationPrefWidget mUi;
};
} // namespace rv
#endif
