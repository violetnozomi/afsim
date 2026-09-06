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

#include "WkfTeamVisibilityWidget.hpp"

#include <QCheckBox>
#include <QHeaderView>
#include <QListWidget>
#include <QMessageBox>
#include <QTableWidget>
#include <QTimer>

#include "UtCast.hpp"
#include "VaObserver.hpp"
#include "WkfObserver.hpp"
#include "WkfScenario.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::TeamVisibilityWidget::TeamVisibilityWidget(QWidget* aParent, Qt::WindowFlags f)
   : QWidget(aParent, f)
{
   mUI.setupUi(this);

   connect(wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>(),
           &wkf::TeamVisibilityPrefObject::TeamVisibilityChanged,
           this,
           &TeamVisibilityWidget::PreferencesChanged);
   connect(mUI.listWidget, &QListWidget::itemChanged, this, &TeamVisibilityWidget::VisibilityChanged);

   mCallbacks.Add(wkf::Observer::StandardScenarioRemoved.Connect(&TeamVisibilityWidget::StandardScenarioRemovedCB, this));
   mCallbacks.Add(vespa::VaObserver::EntityAdded.Connect(&TeamVisibilityWidget::PlatformAddedCB, this));
   mCallbacks.Add(vespa::VaObserver::EntityRemoved.Connect(&TeamVisibilityWidget::PlatformDeletedCB, this));
   mCallbacks.Add(wkf::Observer::PlatformPropertiesChanged.Connect(&TeamVisibilityWidget::PlatformChangedSidesCB, this));
}

void wkf::TeamVisibilityWidget::VisibilityChanged(QListWidgetItem* item)
{
   bool currentVisibility =
      wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>()->GetPreferences().mTeamInfo.find(item->text())->second.visible;
   bool newState = item->checkState();

   if (currentVisibility != newState)
   {
      wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>()->SetTeamVisible(item->text(), newState);
   }
}

void wkf::TeamVisibilityWidget::Populate()
{
   auto teamMap = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>()->GetTeamVisibilityMap();
   if (!teamMap.empty())
   {
      mUI.listWidget->clear();

      int row = 0;
      for (const auto& team : teamMap)
      {
         if (mPresentSides.find(team.first) != mPresentSides.end())
         {
            QListWidgetItem* item = new QListWidgetItem();
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            item->setText(team.first);
            if (team.second)
            {
               item->setCheckState(Qt::Checked);
            }
            else
            {
               item->setCheckState(Qt::Unchecked);
            }
            mUI.listWidget->insertItem(row, item);

            ++row;
         }
      }
   }
}

void wkf::TeamVisibilityWidget::Update()
{
   if (mModified)
   {
      Populate();
      mModified = false;
   }
}

void wkf::TeamVisibilityWidget::PreferencesChanged()
{
   Populate();
}

void wkf::TeamVisibilityWidget::StandardScenarioRemovedCB(wkf::Scenario* aScenarioPtr)
{
   mPresentSides.clear();
}

void wkf::TeamVisibilityWidget::PlatformAddedCB(vespa::VaEntity* aEntityPtr)
{
   bool           found  = false;
   wkf::Platform* entity = dynamic_cast<wkf::Platform*>(aEntityPtr);

   if (entity)
   {
      QString side = QString::fromStdString(entity->GetSide());

      if (side.isEmpty())
      {
         side = "default";
         entity->SetSide("default");
      }

      mPlatformSide.emplace(QString::fromStdString(entity->GetName()), side);

      for (auto&& team : wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>()->GetPreferences().mTeamInfo)
      {
         if (side == team.first)
         {
            found = true;
         }
      }

      if (!found)
      {
         wkf::TeamVisibilityPrefData::TeamInfo newTeam;
         wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>()->AddTeam(side, newTeam);
      }

      mPresentSides.emplace(side);
      Populate();
   }
}

void wkf::TeamVisibilityWidget::PlatformDeletedCB(vespa::VaEntity* aEntityPtr)
{
   std::map<QString, QString>::iterator platformSideIter =
      mPlatformSide.find(QString::fromStdString(aEntityPtr->GetName()));
   wkf::Platform* entity = dynamic_cast<wkf::Platform*>(aEntityPtr);

   if (entity)
   {
      if (platformSideIter != mPlatformSide.end())
      {
         mPlatformSide.erase(platformSideIter);
      }

      bool foundSide = false;
      for (auto&& platform : mPlatformSide)
      {
         if (platform.second.toStdString() == entity->GetSide())
         {
            foundSide = true;
         }
      }

      if (!foundSide)
      {
         std::string side = entity->GetSide();

         if (side.empty())
         {
            side = "default";
         }
         std::set<QString>::iterator presentSideIter = mPresentSides.find(QString::fromStdString(side));
         if (presentSideIter != mPresentSides.end())
         {
            mPresentSides.erase(presentSideIter);
         }
      }
      Populate();
   }
}

void wkf::TeamVisibilityWidget::PlatformChangedSidesCB(wkf::Platform* aPlatformPtr)
{
   QString side = QString::fromStdString(aPlatformPtr->GetSide());
   if (side.isEmpty())
   {
      side = "default";
   }

   mPresentSides.emplace(side);
   if (wkfEnv.GetPreferenceObject<TeamVisibilityPrefObject>()->AddTeam(side, {}))
   {
      Populate();
   }

   mPlatformSide[QString::fromStdString(aPlatformPtr->GetName())] = side;

   // This request will clean up teams that have been added to wizard's team list, but have no platforms contained.
   // The situation can happen as teams are changed as part of the development process, or when half formed team
   // names are parsed.
   if (wkfEnv.GetApplicationName() == "wizard")
   {
      RequestTeamCleanup();
   }

   Populate();
}

void wkf::TeamVisibilityWidget::RequestTeamCleanup()
{
   if (!mCleanupRequested)
   {
      QTimer::singleShot(1000, this, [this]() { CleanupTeams(); });
      mCleanupRequested = true;
   }
}

void wkf::TeamVisibilityWidget::CleanupTeams()
{
   for (int row = 0; row < mUI.listWidget->count(); ++row)
   {
      bool found = false;
      for (auto&& platform : mPlatformSide)
      {
         if (platform.second == mUI.listWidget->item(row)->text())
         {
            found = true;
            break;
         }
      }
      if (!found)
      {
         mPresentSides.erase(mUI.listWidget->item(row)->text());
         // remove team if not a default
         wkfEnv.GetPreferenceObject<TeamVisibilityPrefObject>()->RemoveTeam(mUI.listWidget->item(row)->text());
      }
   }
   mCleanupRequested = false;
}
