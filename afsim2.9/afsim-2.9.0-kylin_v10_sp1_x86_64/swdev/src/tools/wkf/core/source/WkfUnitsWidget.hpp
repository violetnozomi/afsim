// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WKFUNITSWIDGET_HPP
#define WKFUNITSWIDGET_HPP

#include "WkfPrefWidget.hpp"
#include "WkfUnitsObject.hpp"
#include "ui_WkfUnitsWidget.h"

namespace wkf
{
class UnitsWidget : public PrefWidgetT<UnitsObject>
{
public:
   explicit UnitsWidget(QWidget* parent = nullptr);

   QString GetCategoryHint() const override { return "Application"; }

private:
   void showEvent(QShowEvent*) override;
   void ReadPreferenceData(const UnitsData& aPrefData) override;
   void WritePreferenceData(UnitsData& aPrefData) override;

   Ui::WkfUnitsWidget mUI;
};
} // namespace wkf
#endif // WKUNITSWIDGET_HPP
