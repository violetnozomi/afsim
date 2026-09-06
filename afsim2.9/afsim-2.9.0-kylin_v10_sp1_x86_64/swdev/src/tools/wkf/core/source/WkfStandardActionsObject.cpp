// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfStandardActionsObject.hpp"

#include <QKeySequence>
#include <QMessageBox>

#include "VaEntity.hpp"
#include "VaViewer.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfPlatform.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::StandardActionsObject::StandardActionsObject()
{
   wkf::Action* deleteShortcut = new wkf::Action("Delete Entity", wkfEnv.GetMainWindow(), QKeySequence(Qt::Key_Delete));
   connect(deleteShortcut, &QAction::triggered, this, [&]() { emit wkfEnv.DeleteShortcutTriggered(); });
   mActionList.push_back(deleteShortcut);
   // two separate loops so that "Assign Group" items all get added
   // before the "Select Group" items in the preferences dialog
   for (int k = Qt::Key_0; k <= Qt::Key_9; ++k)
   {
      int          num              = k - Qt::Key_0; // number form (0-9)
      QString      assignActionText = QString::fromStdString("Assign Group ") + QString::number(num);
      wkf::Action* assignShortcut = new wkf::Action(assignActionText, wkfEnv.GetMainWindow(), QKeySequence(Qt::CTRL + k));
      connect(assignShortcut, &QAction::triggered, this, [this, num]() { HandleGroupAssignment(num); });
      mActionList.push_back(assignShortcut);
   }
   for (int k = Qt::Key_0; k <= Qt::Key_9; ++k)
   {
      int          num              = k - Qt::Key_0; // number form (0-9)
      QString      selectActionText = QString::fromStdString("Select Group ") + QString::number(num);
      wkf::Action* selectShortcut   = new wkf::Action(selectActionText, wkfEnv.GetMainWindow(), QKeySequence(k));
      connect(selectShortcut, &QAction::triggered, this, [this, num]() { HandleGroupSelection(num); });
      mActionList.push_back(selectShortcut);
   }
}

wkf::StandardActionsObject::~StandardActionsObject()
{
   for (auto& a : mActionList)
   {
      delete a;
   }
}

void wkf::StandardActionsObject::HandleGroupAssignment(int aIndex)
{
   // Assign the currently selected entities to the action with index aIndex
   QStringList platformList;
   for (auto& p : wkfEnv.GetSelectedPlatforms())
   {
      platformList.push_back(QString::fromStdString(p->GetName()));
   }

   // if the user had nothing selected, remove from the map
   if (platformList.empty())
   {
      mActionMap.remove(aIndex);
      wkfEnv.GetMainWindow()->ShowStatusMessage("Group " + QString::number(aIndex) + " cleared", 3000);
   }
   else
   {
      mActionMap[aIndex] = platformList;
      wkfEnv.GetMainWindow()->ShowStatusMessage("Platforms assigned to group " + QString::number(aIndex), 3000);
   }
}

void wkf::StandardActionsObject::HandleGroupSelection(int aIndex)
{
   // Select the group of entities assigned to the action with index aIndex
   auto item = mActionMap.find(aIndex);
   if (item != mActionMap.end())
   {
      QStringList  selectedEntities = *item;
      PlatformList platforms;
      for (auto& e : selectedEntities)
      {
         auto* platform = dynamic_cast<wkf::Platform*>(vaEnv.FindEntity(e.toStdString()));
         if (platform != nullptr)
         {
            platforms.push_back(platform);
         }
      }
      wkfEnv.SetPlatformsSelected(platforms);
   }
}
