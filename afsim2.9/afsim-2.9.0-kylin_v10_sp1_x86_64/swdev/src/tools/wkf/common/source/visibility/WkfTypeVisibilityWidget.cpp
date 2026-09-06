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
#include "WkfTypeVisibilityWidget.hpp"

#include <QLineEdit>
#include <QListWidget>
#include <QMenu>

#include "WkfEnvironment.hpp"
#include "WkfObserver.hpp"
#include "WkfPlatform.hpp"
#include "WkfTypeVisibilityPrefObject.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::TypeVisibilityWidget::TypeVisibilityWidget(QWidget* parent, Qt::WindowFlags f)
   : QWidget(parent, f)
{
   mUI.setupUi(this);

   // callbacks
   mCallbacks.Add(wkf::Observer::PlatformsAdded.Connect(
      [this](const wkf::PlatformList& aPlatforms)
      {
         for (const auto& platform : aPlatforms)
         {
            if (platform)
            {
               size_t initSize = mTypeSet.size();
               for (const auto& type : platform->GetTypeList())
               {
                  mTypeSet.insert(type);
               }

               // If the set changed, Update the DockWidget
               if (initSize != mTypeSet.size())
               {
                  mModified = true;
               }
            }
         }
      }));

   mCallbacks.Add(wkf::Observer::StandardScenarioRemoved.Connect(
      [this](wkf::Scenario*)
      {
         mTypeSet.clear();
         PopulateDisplay();
      }));

   // Connections
   // Preferences changed, so the display needs to be updated
   wkf::TypeVisibilityPrefObject* tvpo = wkfEnv.GetPreferenceObject<TypeVisibilityPrefObject>();
   if (nullptr != tvpo)
   {
      connect(tvpo, &TypeVisibilityPrefObject::TypeVisibilityChanged, this, &TypeVisibilityWidget::PopulateDisplay);
   }
   // The user is allowed to modify preferences from the dock widget, so there is a need to notify and change preferences
   connect(mUI.mListWidget,
           &QListWidget::itemChanged,
           this,
           [](const QListWidgetItem* aItem)
           {
              wkfEnv.GetPreferenceObject<TypeVisibilityPrefObject>()
                 ->ExternalAddPreference(aItem->text(), (aItem->checkState() == Qt::Checked) ? true : false);
           });
   connect(mUI.mFilterLineEdit, &QLineEdit::textChanged, this, &TypeVisibilityWidget::FilterChanged);
   connect(mUI.mCheckBox,
           &QCheckBox::clicked,
           [&](bool aChecked)
           {
              for (int i = 0; i < mUI.mListWidget->count(); ++i)
              {
                 (aChecked ? mUI.mListWidget->item(i)->setCheckState(Qt::Checked) :
                             mUI.mListWidget->item(i)->setCheckState(Qt::Unchecked));
              }
              FilterChanged(mUI.mFilterLineEdit->text());
           });
   // Open a context menu to select all platforms in the scenario with the specified type: (Action only enabled if the
   // box is already checked)
   mUI.mListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(mUI.mListWidget, &QListWidget::customContextMenuRequested, this, &TypeVisibilityWidget::CreateContextMenu);
}

void wkf::TypeVisibilityWidget::Update()
{
   // We don't want to redraw after every PlatformAdded to avoid redrawing the
   //  dock widget multiple times when multiple platforms are added at same time.
   //  (Such as loading a scenario, or initializing a simulation)
   if (mModified)
   {
      PopulateDisplay();
      mModified = false;
   }
}

void wkf::TypeVisibilityWidget::PopulateDisplay()
{
   bool checkedStateFound   = false;
   bool uncheckedStateFound = false;

   // It's easier to just repopulate the entire display instead of individually finding and changing boxes
   mUI.mListWidget->clear();
   const auto& hiddenTypes = wkfEnv.GetPreferenceObject<TypeVisibilityPrefObject>()->GetPreferences().mHiddenTypes;
   for (const auto& type : mTypeSet)
   {
      QString          platformType = QString::fromStdString(type);
      QListWidgetItem* item         = new QListWidgetItem(platformType);
      item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
      item->setFlags(item->flags() & (~Qt::ItemIsSelectable));
      if (hiddenTypes.contains(platformType))
      {
         item->setCheckState(Qt::Unchecked);
         uncheckedStateFound = true;
      }
      else
      {
         item->setCheckState(Qt::Checked);
         checkedStateFound = true;
      }
      mUI.mListWidget->addItem(item);
   }

   if (checkedStateFound && uncheckedStateFound)
   {
      mUI.mCheckBox->setCheckState(Qt::PartiallyChecked);
   }
   else if (uncheckedStateFound)
   {
      mUI.mCheckBox->setCheckState(Qt::Unchecked);
   }
   else
   {
      mUI.mCheckBox->setCheckState(Qt::Checked);
   }
}

void wkf::TypeVisibilityWidget::CreateContextMenu(const QPoint& aPoint)
{
   QListWidgetItem* selectedItem = mUI.mListWidget->itemAt(aPoint);
   if (selectedItem && !selectedItem->text().isEmpty())
   {
      QMenu         contextMenu;
      const QString cMESSAGE = "Select All with Type: " + selectedItem->text();
      QAction       selectAll(cMESSAGE);
      selectAll.setEnabled(selectedItem->checkState() == Qt::Checked);
      connect(&selectAll,
              &QAction::triggered,
              this,
              [&]()
              {
                 vespa::VaEntityList entityList;
                 vaEnv.GetEntityList(entityList);
                 wkf::PlatformList platformList;
                 for (const auto& platform : entityList)
                 {
                    wkf::Platform* platformPtr = dynamic_cast<wkf::Platform*>(platform);
                    if (platformPtr && platformPtr->IsOfType(selectedItem->text().toStdString()))
                    {
                       platformList.append(platformPtr);
                    }
                 }
                 wkfEnv.SetPlatformsSelected(platformList);
              });
      contextMenu.addAction(&selectAll);
      contextMenu.exec(QCursor::pos());
   }
}

void wkf::TypeVisibilityWidget::FilterChanged(const QString& aText)
{
   for (int i = 0; i < mUI.mListWidget->count(); ++i)
   {
      QListWidgetItem* itemPtr = mUI.mListWidget->item(i);
      itemPtr->setHidden(!itemPtr->text().toLower().contains(aText.toLower()));
   }
}
