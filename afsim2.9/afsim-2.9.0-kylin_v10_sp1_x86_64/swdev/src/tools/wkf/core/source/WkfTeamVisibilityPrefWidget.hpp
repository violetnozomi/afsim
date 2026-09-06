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
#ifndef WKFTEAMVISIBILITYPREFWIDGET_HPP
#define WKFTEAMVISIBILITYPREFWIDGET_HPP

#include "wkf_export.h"

#include "WkfPrefWidget.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"
#include "ui_WkfTeamVisibilityPrefWidget.h"

namespace wkf
{
class WKF_EXPORT TeamVisibilityPrefWidget : public PrefWidgetT<TeamVisibilityPrefObject>
{
public:
   TeamVisibilityPrefWidget(QWidget* aParent = nullptr);

   QString GetCategoryHint() const override { return "Visibility"; }

private:
   bool eventFilter(QObject* aObject, QEvent* aEvent) override;
   void showEvent(QShowEvent* aEvent) override;
   void ReadPreferenceData(const TeamVisibilityPrefData& aPrefData) override;
   void WritePreferenceData(TeamVisibilityPrefData& aPrefData) override;

   void AddNewTeamButtonPressed();

   QStringList mDeletedTeamNames;

   Ui::WkfTeamVisibilityPrefWidget mUi;
};
} // namespace wkf
#endif
