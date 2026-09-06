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

#ifndef WKF_LOG_NOTIFICATIONS_PREF_WIDGET_HPP
#define WKF_LOG_NOTIFICATIONS_PREF_WIDGET_HPP

#include "wkf_common_export.h"

#include "WkfLogNotificationsPrefObject.hpp"
#include "WkfPrefWidget.hpp"
#include "ui_WkfLogNotificationsPrefWidget.h"

namespace wkf
{
namespace log
{
constexpr const char* cPREF_WIDGET_TITLE = "Notifications";

class WKF_COMMON_EXPORT NotificationsPrefWidget final : public wkf::PrefWidgetT<NotificationsPrefObject>
{
   Q_OBJECT

public:
   NotificationsPrefWidget() noexcept;
   ~NotificationsPrefWidget() noexcept override = default;

   QString GetCategoryHint() const override;

   void showEvent(QShowEvent* aEvent) override;

private:
   void ReadPreferenceData(const NotificationsPrefData& aPrefData) override;
   void WritePreferenceData(NotificationsPrefData& aPrefData) override;

   Ui::WkfLogNotificationsPrefWidget mUi;
};
} // namespace log
} // namespace wkf

#endif
