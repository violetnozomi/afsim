// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WkfPlatformHistoryPrefWidget_HPP
#define WkfPlatformHistoryPrefWidget_HPP

#include "wkf_common_export.h"

#include <QPointer>
#include <QString>
#include <QWidget>

#include "WkfPlatformHistoryColorWidget.hpp"
#include "WkfPlatformHistoryPrefObject.hpp"
#include "WkfPrefWidget.hpp"
#include "ui_WkfPlatformHistoryPrefWidget.h"

namespace wkf
{
class WKF_COMMON_EXPORT PlatformHistoryPrefWidget : public PrefWidgetT<PlatformHistoryPrefObject>
{
public:
   PlatformHistoryPrefWidget();
   ~PlatformHistoryPrefWidget() override = default;

   QString GetCategoryHint() const override { return "Map"; }

   void AddTraceLineMode(const QString& aModeName, int aModeEnum, PlatformHistoryColorWidget* aModeWidget);

   // Unfortunately, the Behavior trace lines seem to live and die by the ability to immediately apply preferences.
   // This maybe could be avoided if preferences for behavior trace lines did not need to be saved between sessions.
   void Apply() { PrefWidgetT<PlatformHistoryPrefObject>::ApplyChanges(); }

private slots:
   void ChangeTracelineSettings(const QString& aTracelineSetting);

private:
   void                             ReadPreferenceData(const TraceLinesPrefData& aPrefData) override;
   void                             WritePreferenceData(TraceLinesPrefData& aPrefData) override;
   Ui::WkfPlatformHistoryPrefWidget mUi;

   std::map<QString, QPointer<PlatformHistoryColorWidget>> mTracelineSettings;
};
} // namespace wkf
#endif // WkfPlatformHistoryPrefWidget_HPP
