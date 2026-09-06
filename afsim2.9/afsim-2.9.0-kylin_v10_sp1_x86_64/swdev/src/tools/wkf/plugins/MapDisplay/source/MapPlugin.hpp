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
#ifndef PLUGINMAPDISPLAY_HPP
#define PLUGINMAPDISPLAY_HPP

#include <QString>
#include <QTimer>

class QMenu;

#include "MapCursorStatusDockWidget.hpp"
#include "MapCursorStatusWidget.hpp"
#include "MapDockWidget.hpp"
#include "MapPrefWidget.hpp"
#include "MapToolbar.hpp"
#include "WkfPlugin.hpp"

namespace wkf
{
class Environment;
}

namespace Map
{
class Plugin : public wkf::Plugin
{
public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

   QList<wkf::Action*>     GetActions() const override;
   QList<wkf::PrefWidget*> GetPreferencesWidgets() const override;
   void                    BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntityPtr) override;

   void           ResetOptionStates() override;
   void           SetPlatformOptionState(int aOptionId, bool aState, wkf::Platform* aPlatformPtr) override;
   Qt::CheckState GetPlatformOptionState(int aOptionId, const wkf::Platform* aPlatformPtr) const override;

   std::unique_ptr<wkf::Updater> GetUpdater(const QString&     aPlatformName,
                                            const QString&     aDatum,
                                            const unsigned int aContext = 0) const override;

   // Load the settings from the specified QSettings file.
   void LoadSettings(QSettings& aSettings) override;
   // Save the settings to the specified QSettings file.
   void SaveSettings(QSettings& aSettings) override;

private slots:
   void PreferencesChanged();

private:
   bool eventFilter(QObject* watched, QEvent* event) override;

   PluginUiPointer<DockWidget>         mDockWidgetPtr;
   PluginUiPointer<PrefWidget>         mMapPrefWidgetPtr;
   PluginUiPointer<CursorStatusWidget> mCursorStatusPtr;
   QPointer<CursorStatusDockWidget>    mCursorDockWidgetPtr;

   vespa::VaCallbackHolder mMapCallbacks;

   // we periodically update to allow the resource manager to provide elevation after the cursor has stopped moving
   QTimer   mTimer;
   ToolBar* mToolBarPtr;
};
} // namespace Map
#endif
