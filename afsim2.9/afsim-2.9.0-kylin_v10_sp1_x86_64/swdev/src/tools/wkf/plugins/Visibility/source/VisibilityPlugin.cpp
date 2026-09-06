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

#include "VisibilityPlugin.hpp"

#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfObserver.hpp"
#include "WkfScenario.hpp"
#include "WkfVtkEnvironment.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(Visibility::Plugin,
                          "Visibility",
                          "The Visibility dialog provides a list of teams included in the current scenario and allows "
                          "the user to quickly select which teams they want visible.",
                          "all")

Visibility::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : wkf::Plugin(aPluginName, aUniqueId)
{
   wkf::MainWindow* mainWindowPtr = wkfEnv.GetMainWindow();
   mDockWidgetPtr =
      vaEnv.GetFactory()->CreateDockWidget<wkf::VisibilityDockWidget>("VisibilityWidget",
                                                                      wkfEnv.GetMainWindow()->centralWidget());
   mainWindowPtr->addDockWidget(Qt::RightDockWidgetArea, mDockWidgetPtr);
   mDockWidgetPtr->hide();
   wkfEnv.RegisterPlatformVisibilityFilter([this](const wkf::Platform& aPlatform) -> bool
                                           { return this->IsVisible(aPlatform.GetIndex()); });
   mCallbacks.Add(wkf::Observer::StandardScenarioRemoved.Connect([this](wkf::Scenario* aScenarioPtr) { this->ShowAll(); }));
}

void Visibility::Plugin::GuiUpdate()
{
   mDockWidgetPtr->Update();
}

void Visibility::Plugin::BuildViewerContextMenu(QMenu* aMenuPtr, vespa::VaViewer* aViewerPtr)
{
   QMenu* subMenuPtr = new QMenu("Visibility", aMenuPtr);

   QAction* showAllPtr = new QAction(QString("Remove all Filters"), subMenuPtr);
   subMenuPtr->addAction(showAllPtr);
   connect(showAllPtr, &QAction::triggered, this, &Visibility::Plugin::ShowAll);

   if (wkfEnv.GetSelectedPlatforms().size() != 0)
   {
      QAction* hideSelectedPtr   = new QAction(QString("Filter Out Selected Platforms"), subMenuPtr);
      QAction* hideUnselectedPtr = new QAction(QString("Filter Out Unselected Platforms"), subMenuPtr);

      subMenuPtr->addAction(hideSelectedPtr);
      subMenuPtr->addAction(hideUnselectedPtr);

      connect(hideSelectedPtr, &QAction::triggered, this, &Visibility::Plugin::HideSelected);
      connect(hideUnselectedPtr, &QAction::triggered, this, &Visibility::Plugin::HideUnselected);
   }

   aMenuPtr->addMenu(subMenuPtr);
}

void Visibility::Plugin::HideSelected()
{
   for (auto platform : wkfEnv.GetSelectedPlatforms())
   {
      mIsInvisible[platform->GetIndex()] = true;
   }
   emit wkfEnv.PlatformVisibilityChanged();
}

void Visibility::Plugin::HideUnselected()
{
   auto scenarioPtr = vaEnv.GetStandardScenario();
   for (auto platform : scenarioPtr->GetIndexPlatformMap())
   {
      if (!wkfEnv.IsPlatformSelected(platform.second))
      {
         mIsInvisible[platform.first] = true;
      }
   }
   emit wkfEnv.PlatformVisibilityChanged();
}

void Visibility::Plugin::ShowAll()
{
   mIsInvisible.clear();
   emit wkfEnv.PlatformVisibilityChanged();
}

bool Visibility::Plugin::IsVisible(size_t index)
{
   if (mIsInvisible.find(index) == mIsInvisible.end())
   {
      return true;
   }
   return !mIsInvisible[index];
}
