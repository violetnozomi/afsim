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
#ifndef MapHoverInfoPLUGIN_HPP
#define MapHoverInfoPLUGIN_HPP

#include <QMenu>
#include <QProxyStyle>
#include <QTimer>

#include "MapHoverInfoPrefWidget.hpp"
#include "VaCallbackHolder.hpp"
#include "VaViewer.hpp"
#include "VaWidget.hpp"
#include "WkfPlugin.hpp"
#include "WkfUpdater.hpp"

namespace MapHoverInfo
{
class HoverManager : public QObject
{
public:
   HoverManager(vespa::VaWidget* aParent);

   void ClearUpdaters();

   void Enable(bool aState) { mEnabled = aState; }
   bool IsEnabled() { return mEnabled; }

   bool eventFilter(QObject* aWatched, QEvent* aEvent) override;
   void HandleHoverEvent();

private:
   QString GetEntityString(vespa::VaEntity* aEntity, bool& aValid);
   void    GetUpdatersForEntity(const QStringList& aUpdaterNames, vespa::VaEntity* aEntity);

   QString GetPlatformString(wkf::Platform* aPlatform, bool& aValid);
   QString GetNonPlatformString(const wkf::Entity* aEntity, bool& aValid);

   void Revisit();

   using UpdaterList = std::list<std::unique_ptr<wkf::Updater>>;
   using Updaters    = std::map<vespa::VaEntity*, UpdaterList>;
   vespa::VaWidget*  mParent;
   Updaters          mUpdaters;
   bool              mEnabled{true};
   QTimer            mTimer;
   QPoint            mCursor;
   vespa::VaHitEntry mObject;
};

class Plugin : public wkf::Plugin
{
   Q_OBJECT
public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

   QList<wkf::PrefWidget*> GetPreferencesWidgets() const override;

   void BuildViewerContextMenu(QMenu* aMenu, vespa::VaViewer* aViewerPtr) override;

protected:
   void AddHoverToWidget(vespa::VaWidget* aWidget);

private:
   vespa::VaCallbackHolder mCallbacks;
   // Use guarded pointers because objects will be owned by main window
   PluginUiPointer<PrefWidget> mPrefWidgetPtr;
   HoverManager*               mManagerPtr;
};

class HoverStyle : public QProxyStyle
{
   Q_OBJECT
public:
   HoverStyle(QObject* aParent) { setParent(aParent); }

   int styleHint(QStyle::StyleHint   aHint,
                 const QStyleOption* aOption     = nullptr,
                 const QWidget*      aWidget     = nullptr,
                 QStyleHintReturn*   aReturnData = nullptr) const Q_DECL_OVERRIDE
   {
      if (aHint == QStyle::SH_ToolTip_WakeUpDelay)
      {
         return 100;
      }
      return QProxyStyle::styleHint(aHint, aOption, aWidget, aReturnData);
   }
};
} // namespace MapHoverInfo

#endif
