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
#ifndef WKFTRACKVISIBILITYPREFWIDGET_HPP
#define WKFTRACKVISIBILITYPREFWIDGET_HPP

#include "wkf_common_export.h"

#include "WkfPrefWidget.hpp"
#include "WkfTrackVisibilityPrefObject.hpp"
#include "ui_WkfTrackVisibilityPrefWidget.h"

namespace wkf
{
class WKF_COMMON_EXPORT TrackVisibilityPrefWidget : public PrefWidgetT<TrackVisibilityPrefObject>
{
public:
   TrackVisibilityPrefWidget(QWidget* aParent = nullptr);

   QString GetCategoryHint() const override { return "Visibility"; }

protected:
   Ui::WkfTrackVisibilityPrefWidget mUi;

private:
   void showEvent(QShowEvent* aEvent) override;
   void ReadPreferenceData(const TrackVisibilityPrefData& aPrefData) override;
   void WritePreferenceData(TrackVisibilityPrefData& aPrefData) override;

   void UpdateSelectedTeamCombo();

   QStringList mDeletedTeamNames;
};
} // namespace wkf
#endif
