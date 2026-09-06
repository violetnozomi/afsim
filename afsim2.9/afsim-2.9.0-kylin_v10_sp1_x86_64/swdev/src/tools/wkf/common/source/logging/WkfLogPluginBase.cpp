// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfLogPluginBase.hpp"

#include <QMessageBox>

#include "UtLog.hpp"
#include "UtQtMemory.hpp"
#include "WkfEnvironment.hpp"
#include "WkfLogInteractiveWidget.hpp"
#include "WkfMainWindow.hpp"
#include "WkfObserver.hpp"

constexpr int cMAX_POPUP_LENGTH = 500;
constexpr int cMAX_POPUP_LINES  = 10;

//! Helper for returning the text in a popup.
//! The text will be capped at cMAX_POPUP_LENGTH characters, or cMAX_POPUP_LINES lines, whichever is less.
static QString GetNotificationTextRecurse(const ut::log::Message& aMessage) noexcept
{
   QString retval = QString::fromStdString(aMessage.mData);

   for (const auto& note : aMessage.mNotes)
   {
      retval += '\n';
      retval += ::GetNotificationTextRecurse(note);
   }

   return retval;
}

//! Returns the text in a popup.
//! The text will be capped at cMAX_POPUP_LENGTH characters, or cMAX_POPUP_LINES lines, whichever is less.
//! If the text is cut short, an additional line will be appended indicating that the entire text can be found elsewhere.
static QString GetNotificationText(const ut::log::Message& aMessage)
{
   bool    trimmed = false;
   QString retval  = ::GetNotificationTextRecurse(aMessage);
   if (retval.length() > cMAX_POPUP_LENGTH)
   {
      retval  = retval.left(cMAX_POPUP_LENGTH) + "...";
      trimmed = true;
   }
   if (retval.count('\n') > cMAX_POPUP_LINES)
   {
      int index = 0;
      for (int i = 0; i < cMAX_POPUP_LINES; i++)
      {
         index = retval.indexOf('\n', index + 1);
      }
      retval  = retval.left(index + 1) + "...";
      trimmed = true;
   }
   retval = retval.toHtmlEscaped();
   retval.replace("\n", "<br/>");
   if (trimmed)
   {
      retval += "<br/><br/><i>(See logs for complete text.)</i>";
   }

   return retval;
}

wkf::log::PluginBase::PluginBase(const QString& aPluginName, const size_t aUniqueId)
   : wkf::Plugin(aPluginName, aUniqueId)
   , mNotificationsPrefWidget(ut::qt::make_qt_ptr<wkf::log::NotificationsPrefWidget>())
{
   mCallbacks += wkf::Observer::StandardScenarioRemoved.Connect(&PluginBase::StdScenarioRemovedCB, this);
   connect(mNotificationsPrefWidget->GetPreferenceObject(),
           &NotificationsPrefObject::EffectiveNotificationLevelChanged,
           this,
           &PluginBase::SetNotificationLevel);
   CreateInteractiveWidget();
}

void wkf::log::PluginBase::QueueMessage(const ut::log::Message& aMessage) noexcept
{
   mModel.QueueMessage(aMessage);

   if (aMessage.mTypes.Overlaps(ut::log::Message::PresetWarnings()))
   {
      mNotificationQueue.push(aMessage);
   }
}

void wkf::log::PluginBase::QueueForConsole(const QString& aText)
{
   if (mConsoleWidget)
   {
      mConsoleWidget->QueueForConsole(aText);
   }
}

void wkf::log::PluginBase::ApplyChanges()
{
   mModel.ApplyQueuedMessages();
   if (mConsoleWidget)
   {
      mConsoleWidget->AppendQueueToConsole();
   }
   NextNotification();
}

void wkf::log::PluginBase::Clear() noexcept
{
   mModel.ClearMessages();
   if (mConsoleWidget)
   {
      mConsoleWidget->Reset();
   }
}

QList<wkf::PrefWidget*> wkf::log::PluginBase::GetPreferencesWidgets() const
{
   QList<wkf::PrefWidget*> retval;
   retval << mNotificationsPrefWidget.data();
   if (mConsolePrefWidget)
   {
      retval << mConsolePrefWidget.data();
   }
   return retval;
}

void wkf::log::PluginBase::CreateConsoleWidget()
{
   if (!mConsolePrefWidget)
   {
      mConsolePrefWidget = ut::qt::make_qt_ptr<wkf::log::ConsolePrefWidget>();
   }
   if (!mConsoleWidget)
   {
      auto dockWidget = ut::qt::make_qt_ptr<QDockWidget>();

      mConsoleWidget = ut::qt::make_qt_ptr<wkf::log::ConsoleWidget>();

      dockWidget->setObjectName("LogConsoleOutput");
      dockWidget->setWindowTitle("Console Output");
      dockWidget->setWidget(mConsoleWidget);

      auto mainWindow = wkfEnv.GetMainWindow();
      if (mainWindow)
      {
         mainWindow->addDockWidget(Qt::BottomDockWidgetArea, dockWidget);
         mainWindow->tabifyDockWidget(mInteractiveWidget, dockWidget);
      }
   }
}

void wkf::log::PluginBase::CreateInteractiveWidget()
{
   if (!mInteractiveWidget)
   {
      mInteractiveWidget = ut::qt::make_qt_ptr<QDockWidget>();
      mInteractiveWidget->setWidget(
         ut::qt::make_qt_ptr<wkf::log::InteractiveWidget>(mModel, ut::log::Message::PresetDebug()));
      mInteractiveWidget->setObjectName("LogInteractiveOutput");
      mInteractiveWidget->setWindowTitle("Interactive Output");

      auto mainWindow = wkfEnv.GetMainWindow();
      if (mainWindow)
      {
         mainWindow->addDockWidget(Qt::BottomDockWidgetArea, mInteractiveWidget);
      }
   }
}

void wkf::log::PluginBase::StdScenarioRemovedCB(wkf::Scenario* /*unused*/)
{
   Clear();
}

void wkf::log::PluginBase::SetNotificationLevel(NotificationLevel aLevel) noexcept
{
   mEffectiveNotificationLevel = aLevel;
}

bool wkf::log::PluginBase::CanCreateNotification(NotificationLevel aLevel) const noexcept
{
   return (mEffectiveNotificationLevel & aLevel) != NotificationLevel::None;
}

void wkf::log::PluginBase::NextNotification()
{
   while (!mNotificationQueue.empty() && !mActiveNotification)
   {
      auto msg = std::move(mNotificationQueue.front());
      mNotificationQueue.pop();

      using ut::log::Message;
      if (CanCreateNotification(NotificationLevel::Fatal) && msg.mTypes.Contains(Message::Fatal()))
      {
         CreateNotification(NotificationLevel::Fatal, Message::Fatal(), msg);
         return;
      }
      else if (CanCreateNotification(NotificationLevel::Error) && msg.mTypes.Contains(Message::Error()))
      {
         CreateNotification(NotificationLevel::Error, Message::Error(), msg);
         return;
      }
      else if (CanCreateNotification(NotificationLevel::Warning) && msg.mTypes.Contains(Message::Warning()))
      {
         CreateNotification(NotificationLevel::Warning, Message::Warning(), msg);
         return;
      }
   }
}

void wkf::log::PluginBase::CreateNotification(NotificationLevel       aLevel,
                                              const std::string&      aTitle,
                                              const ut::log::Message& aMessage) noexcept
{
   mActiveNotification = ut::qt::make_qt_ptr<QMessageBox>(wkfEnv.GetMainWindow());

   mActiveNotification->setModal(false);
   mActiveNotification->setAttribute(Qt::WA_DeleteOnClose);
   auto* btn = mActiveNotification->addButton("Properties...", QMessageBox::ActionRole);
   btn->disconnect(); // prevents the button from closing the dialog.
   connect(btn, &QPushButton::clicked, []() { wkfEnv.GetMainWindow()->ShowPreferencePage(cPREF_WIDGET_TITLE); });

   auto* checkBox = ut::qt::make_qt_ptr<QCheckBox>(mActiveNotification);

   connect(mActiveNotification,
           &QMessageBox::finished,
           [this, aLevel](int aResult) { MessageBoxFinishedCallback(mActiveNotification, aResult, aLevel); });

   mActiveNotification->setStandardButtons(QMessageBox::Ok);
   mActiveNotification->setEscapeButton(QMessageBox::Ok);

   using ut::log::Message;
   if (aTitle == Message::Fatal())
   {
      checkBox->deleteLater();
      checkBox = nullptr;
      mActiveNotification->setIcon(QMessageBox::Icon::Critical);
      mActiveNotification->setWindowTitle("Fatal!");
   }
   else if (aTitle == Message::Error())
   {
      checkBox->setText("Ignore errors for rest of session.");
      mActiveNotification->setIcon(QMessageBox::Icon::Critical);
      mActiveNotification->setWindowTitle("Error!");
   }
   else if (aTitle == Message::Warning())
   {
      checkBox->setText("Ignore warnings for rest of session.");
      mActiveNotification->setIcon(QMessageBox::Icon::Warning);
      mActiveNotification->setWindowTitle("Warning!");
   }
   else
   {
      mActiveNotification->deleteLater();
      mActiveNotification = nullptr;
      return;
   }

   mActiveNotification->setText(::GetNotificationText(aMessage));
   mActiveNotification->setTextFormat(Qt::RichText);
   mActiveNotification->setCheckBox(checkBox);
   mActiveNotification->show();
}

void wkf::log::PluginBase::MessageBoxFinishedCallback(QMessageBox* aMessageBox, int aResult, NotificationLevel aLevel)
{
   QCheckBox* checkBox = aMessageBox->checkBox();
   if (checkBox && checkBox->isChecked())
   {
      mNotificationsPrefWidget->GetPreferenceObject()->SetIgnore(aLevel);
   }
   mActiveNotification->deleteLater();
   mActiveNotification = nullptr;

   NextNotification();
}
