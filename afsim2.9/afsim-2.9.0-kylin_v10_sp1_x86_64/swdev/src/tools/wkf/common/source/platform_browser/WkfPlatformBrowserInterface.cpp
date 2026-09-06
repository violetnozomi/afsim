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

#include "WkfPlatformBrowserInterface.hpp"

#include <QMenu>
#include <QMutex>
#include <QObject>

#include "VaEntity.hpp"
#include "VaObserver.hpp"
#include "VaViewer.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfPlatform.hpp"
#include "WkfPlatformBrowserDockWidget.hpp"
#include "WkfScenario.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::PlatformBrowserInterface::PlatformBrowserInterface(QWidget* aParent)
   : QObject(aParent)
   , mDockWidget(new PlatformBrowserDockWidget(aParent))
{
   wkf::MainWindow* mainWindowPtr = wkfEnv.GetMainWindow();
   mainWindowPtr->addDockWidget(Qt::LeftDockWidgetArea, mDockWidget);

   connect(&wkfEnv, &Environment::PlatformSelectionChanged, this, &PlatformBrowserInterface::PlatformSelectionChangedHandler);

   connect(mDockWidget,
           &PlatformBrowserDockWidget::ContextMenuRequested,
           this,
           &PlatformBrowserInterface::ContextMenuRequestedHandler);
   connect(mDockWidget,
           &PlatformBrowserDockWidget::SelectedPlatformChanged,
           this,
           &PlatformBrowserInterface::SelectedPlatformChangedHandler);
}

//! Method: ContextMenuRequestedHandler
//! Description: Shows the context menu
//! @param aPoint - the point at which to show the context menu
//! @param aText - what item the menu is for (e.g. platform name)
void wkf::PlatformBrowserInterface::ContextMenuRequestedHandler(const QPoint& aPoint, const QString& aText)
{
   auto* scenarioPtr = vaEnv.GetStandardScenario();
   if (scenarioPtr)
   {
      wkf::Platform* plat = scenarioPtr->FindPlatformByName(aText);
      if (plat)
      {
         QMenu menu;
         wkfEnv.BuildEntityContextMenu(&menu, plat);
         menu.exec(aPoint);
      }
   }
}

void wkf::PlatformBrowserInterface::SelectedPlatformChangedHandler(const QStringList& aNames)
{
   if (mEntitySelectionMutex.tryLock())
   {
      if (aNames.isEmpty())
      {
         // Clear the selections
         wkfEnv.SetPlatformsSelected({});
      }
      else
      {
         wkf::PlatformList selectedList;
         for (const auto& aName : aNames)
         {
            Platform* platform = dynamic_cast<Platform*>(vaEnv.FindEntity(aName.toStdString()));
            if (platform != nullptr)
            {
               selectedList.push_back(platform);
            }
         }
         // Use group selection
         wkfEnv.SetPlatformsSelected(selectedList);
      }
      mEntitySelectionMutex.unlock();
   }
}

void wkf::PlatformBrowserInterface::PlatformSelectionChangedHandler(const PlatformList& aSelectedPlatforms,
                                                                    const PlatformList& aUnselectedPlatforms)
{
   if (mEntitySelectionMutex.tryLock())
   {
      for (const auto& platform : aSelectedPlatforms)
      {
         mDockWidget->SetPlatformSelected(QString::fromStdString(platform->GetName()), true);
      }
      for (const auto& platform : aUnselectedPlatforms)
      {
         mDockWidget->SetPlatformSelected(QString::fromStdString(platform->GetName()), false);
      }
      mEntitySelectionMutex.unlock();
   }
}
