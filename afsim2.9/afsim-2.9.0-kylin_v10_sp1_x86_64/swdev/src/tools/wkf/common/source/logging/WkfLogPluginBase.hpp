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
#ifndef WKF_LOG_PLUGIN_BASE_HPP
#define WKF_LOG_PLUGIN_BASE_HPP

#include "wkf_common_export.h"

#include <queue>

#include <QDockWidget>

#include "WkfLogConsolePrefWidget.hpp"
#include "WkfLogConsoleWidget.hpp"
#include "WkfLogModel.hpp"
#include "WkfLogNotificationsPrefWidget.hpp"
#include "WkfPlugin.hpp"

namespace wkf
{
class Scenario;

namespace log
{
class WKF_COMMON_EXPORT PluginBase : public wkf::Plugin
{
   Q_OBJECT
public:
   PluginBase(const QString& aPluginName, const size_t aUniqueId);
   ~PluginBase() noexcept override = default;

   //! Queues a message to the interactive display.
   //! Does not affect the console window.
   //! Message will not be displayed until @see ApplyChanges is called.
   void QueueMessage(const ut::log::Message& aMessage) noexcept;

   //! Queues text to the raw console output.
   //! If console was scrolled to the bottom, stays at the bottom.
   //! Otherwise, stays where it was.
   //! Text will not be displayed until @see ApplyChanges is called.
   void QueueForConsole(const QString& aText);

   //! Applies changes made in QueueMessage and QueueForConsole.
   //! This removes the bottleneck of attempting to apply each change individually.
   void ApplyChanges();

   //! Clears both the interactive display and the console.
   //! Changes take effect immediately. No need to call ApplyChanges.
   void Clear() noexcept;

   QList<wkf::PrefWidget*> GetPreferencesWidgets() const override;

   //! To be called by derived plugin types to create a console dock widget.
   //! Tabifies console widget with interactive widget.
   //! If the console already exists, does nothing.
   void CreateConsoleWidget();

private:
   void CreateInteractiveWidget();

   void StdScenarioRemovedCB(wkf::Scenario* /*unused*/);

   void SetNotificationLevel(NotificationLevel aLevel) noexcept;

   //! Checks whether aLevel is enabled in the notification settings.
   bool CanCreateNotification(NotificationLevel aLevel) const noexcept;

   void NextNotification();

   void CreateNotification(NotificationLevel aLevel, const std::string& aTitle, const ut::log::Message& aMessage) noexcept;

   void MessageBoxFinishedCallback(QMessageBox* aMessageBox, int aResult, NotificationLevel aLevel);

   vespa::VaCallbackHolder mCallbacks;
   //! Stores all of the received messages.
   MasterModel mModel;

   PluginUiPointer<ConsoleWidget>           mConsoleWidget;
   PluginUiPointer<QDockWidget>             mInteractiveWidget;
   PluginUiPointer<NotificationsPrefWidget> mNotificationsPrefWidget;
   PluginUiPointer<ConsolePrefWidget>       mConsolePrefWidget;

   NotificationLevel            mEffectiveNotificationLevel = NotificationLevel::None;
   QMessageBox*                 mActiveNotification         = nullptr;
   std::queue<ut::log::Message> mNotificationQueue;
};
} // namespace log
} // namespace wkf

#endif
