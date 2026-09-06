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
#ifndef WKFGENERALPREFWIDGET_HPP
#define WKFGENERALPREFWIDGET_HPP

#include "WkfGeneralPrefObject.hpp"
#include "WkfPrefWidget.hpp"
#include "ui_WkfGeneralPrefWidget.h"

namespace wkf
{
class GeneralPrefWidget : public PrefWidgetT<GeneralPrefObject>
{
public:
   GeneralPrefWidget(QWidget* parent = nullptr);

   QString GetCategoryHint() const override { return "Application"; }
   bool    GetCategoryHintPriority() const override { return true; }

private:
   void showEvent(QShowEvent* aEvent) override;
   void ReadPreferenceData(const GeneralPrefData& aPrefData) override;
   void WritePreferenceData(GeneralPrefData& aPrefData) override;

   Ui::WkfGeneralPrefWidget mUi;
};
} // namespace wkf
#endif
