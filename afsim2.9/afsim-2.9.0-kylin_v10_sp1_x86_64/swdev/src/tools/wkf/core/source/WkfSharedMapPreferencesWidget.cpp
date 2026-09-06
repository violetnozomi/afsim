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

#include "WkfSharedMapPreferencesWidget.hpp"

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <osgDB/ReadFile>
#include <osgEarth/MapNode>

#include "WkfEnvironment.hpp"
#include "WkfSharedMapPreferencesObject.hpp"


wkf::SharedMapPreferencesWidget::SharedMapPreferencesWidget(QWidget* aParent, Qt::WindowFlags aFlags)
   : wkf::PrefWidgetT<wkf::SharedMapPreferencesObject>(aParent, aFlags)
{
   if (objectName().isEmpty())
   {
      setObjectName("WkfSharedMapPreferencesWidget");
   }
   setWindowTitle("Map Definitions");
   setLayout(new QVBoxLayout(this));

   auto* label     = new QLabel("Preferences on this page are shared among all AFSIM applications", this);
   QFont labelFont = label->font();
   labelFont.setBold(true);
   label->setFont(labelFont);
   layout()->addWidget(label);

   QHBoxLayout* hbox = new QHBoxLayout();
   layout()->addItem(hbox);
   hbox->addWidget(new QLabel("Maps", this));
   QPushButton* newMap = new QPushButton("Add New Map", this);
   connect(newMap, &QPushButton::clicked, this, &SharedMapPreferencesWidget::AddNewMap);
   layout()->addWidget(newMap);
   hbox->addWidget(newMap);

   mMapTreePtr = new QTreeWidget(this);
   layout()->addWidget(mMapTreePtr);

   layout()->addWidget(new QLabel("Profiles", this));

   mProfileTreePtr = new QTreeWidget(this);
   layout()->addWidget(mProfileTreePtr);

   mMapTreePtr->setContextMenuPolicy(Qt::CustomContextMenu);
   mMapTreePtr->setSortingEnabled(false);
   connect(mMapTreePtr, &QWidget::customContextMenuRequested, this, &wkf::SharedMapPreferencesWidget::ContextMenuEvent);

   mProfileTreePtr->setColumnCount(2);
   mProfileTreePtr->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
   mProfileTreePtr->header()->close();
   mProfileTreePtr->clear();
   mMapTreePtr->setColumnCount(1);
   mMapTreePtr->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
   mMapTreePtr->header()->close();
   mMapTreePtr->clear();
}

//! @details this override does a strange thing in the fact that it saves
//! the maps before calling the PrefObject's RestoreDefaults function to
//! restore the default map profiles.
void wkf::SharedMapPreferencesWidget::RestoreDefaults()
{
   // store off the map data
   SharedMapPreferencesData data;
   WritePreferenceData(data);
   mPrefObjectPtr->SetPreferenceData(QVariant::fromValue(data));

   // restore the profiles
   mPrefObjectPtr->RestoreDefaults();
}

void wkf::SharedMapPreferencesWidget::WritePreferenceData(SharedMapPreferencesData& aPrefData)
{
   aPrefData.mMaps.clear();
   for (int i = 0; i < mMapTreePtr->invisibleRootItem()->childCount(); ++i)
   {
      QTreeWidgetItem* map     = mMapTreePtr->invisibleRootItem()->child(i);
      QString          name    = map->data(0, Qt::UserRole + 1).value<QString>();
      auto&            mapData = aPrefData.mMaps[name.toStdString()];

      mapData.mProjection = map->data(0, Qt::UserRole + 2).value<QString>().toStdString();
      mapData.mLatOffset  = map->data(0, Qt::UserRole + 3).toFloat();
      mapData.mLock       = map->data(0, Qt::UserRole + 4).toBool();

      if (map->childCount() > 0)
      {
         mapData.mFile = map->child(0)->text(0).toStdString();
      }
   }
   aPrefData.mProfileMap.clear();
   for (int i = 0; i < mProfileTreePtr->invisibleRootItem()->childCount(); ++i)
   {
      QTreeWidgetItem* profile = mProfileTreePtr->invisibleRootItem()->child(i);
      auto             type =
         static_cast<SharedMapPreferencesData::MapType>(profile->data(0, Qt::UserRole + 1).value<int>()); // profile idx
      QComboBox* combo = dynamic_cast<QComboBox*>(mProfileTreePtr->itemWidget(profile, 1));
      QString    name  = combo->currentText();
      aPrefData.mProfileMap.emplace(type, name.toStdString());
   }
}

void wkf::SharedMapPreferencesWidget::ReadPreferenceData(const SharedMapPreferencesData& aPrefData)
{
   QTreeWidgetItem* child;
   while ((child = mMapTreePtr->invisibleRootItem()->takeChild(0)) != nullptr)
   {
      delete child;
   }
   for (auto& it : aPrefData.mMaps)
   {
      QString          name = it.first.c_str();
      QTreeWidgetItem* map  = new QTreeWidgetItem(mMapTreePtr->invisibleRootItem(), QStringList(name));
      if (it.second.mLock)
      {
         QFont font = map->font(0);
         font.setItalic(true);
         map->setFont(0, font);
      }
      map->setData(0, Qt::UserRole, cMAP);
      map->setData(0, Qt::UserRole + 1, QString(it.first.c_str()));
      map->setData(0, Qt::UserRole + 2, QString(it.second.mProjection.c_str()));
      map->setData(0, Qt::UserRole + 3, it.second.mLatOffset);
      map->setData(0, Qt::UserRole + 4, it.second.mLock);
      map->setToolTip(0, it.second.mFile.c_str());
      if (!it.second.mFile.empty())
      {
         QTreeWidgetItem* file = new QTreeWidgetItem(map, QStringList(it.second.mFile.c_str()));
         file->setData(0, Qt::UserRole, cMAPFILE);
         file->setHidden(true);
      }
   }
   UpdateProfiles();
}

void wkf::SharedMapPreferencesWidget::UpdateProfiles()
{
   const wkf::SharedMapPreferencesData& prefs = mPrefObjectPtr->GetPreferences();
   QTreeWidgetItem*                     child;
   while ((child = mProfileTreePtr->invisibleRootItem()->takeChild(0)) != nullptr)
   {
      delete child;
   }

   for (auto& it : prefs.mProfileMap)
   {
      wkf::SharedMapPreferencesData::MapType ptype = it.first;
      QString                                map   = it.second.c_str();
      QTreeWidgetItem*                       profile =
         new QTreeWidgetItem(mProfileTreePtr->invisibleRootItem(), QStringList(ProfileTypeLookup(ptype)));
      profile->setData(0, Qt::UserRole + 1, ptype);

      QComboBox* combo = new QComboBox(this);
      mProfileTreePtr->setItemWidget(profile, 1, combo);

      for (int i = 0; i < mMapTreePtr->invisibleRootItem()->childCount(); ++i)
      {
         combo->addItem(mMapTreePtr->invisibleRootItem()->child(i)->text(0));
      }
      combo->setCurrentText(map);
   }
}

QString wkf::SharedMapPreferencesWidget::ProfileTypeLookup(wkf::SharedMapPreferencesData::MapType aType)
{
   switch (aType)
   {
   case (wkf::SharedMapPreferencesData::IMMERSIVE):
      return "Immersive";
      break;
   case (wkf::SharedMapPreferencesData::NAVIGATIONAL):
      return "Navigational";
      break;
   default:
      return "UNKNOWN";
      break;
   }
}

void wkf::SharedMapPreferencesWidget::ContextMenuEvent(const QPoint& aPoint)
{
   QTreeWidgetItem* item = mMapTreePtr->itemAt(aPoint);
   if (item != nullptr)
   {
      int role = item->data(0, Qt::UserRole).toInt();
      switch (role)
      {
      case (cMAP):
      {
         if (!item->data(0, Qt::UserRole + 4).toBool())
         {
            QMenu   popup(this);
            QString renameMap = "Rename";
            QString removemap = "Remove";
            popup.addAction(renameMap);
            popup.addAction(removemap);
            QAction* action = popup.exec(mMapTreePtr->mapToGlobal(aPoint));
            if (nullptr != action)
            {
               if (action->text() == renameMap)
               {
                  RenameMap(item);
               }
               else if (action->text() == removemap)
               {
                  RemoveMap(item);
               }
            }
         }
         break;
      }
      default:
         break;
      }
   }
}


void wkf::SharedMapPreferencesWidget::AddNewMap()
{
   // ask for the map file
   QString fileName = wkf::getOpenFileName(this, "Open Map File", QString(), "Map Files (*.ive *.osg *.osgb *.earth)");
   if (!fileName.isEmpty())
   {
      osg::Node* file = osgDB::readNodeFile(fileName.toStdString());
      if (file)
      {
         osgEarth::MapNode* mn = dynamic_cast<osgEarth::MapNode*>(file);
         if (mn) // if it is osgearth
         {
            if (mn->getMap())
            {
               std::string sname = mn->getMap()->getName();
               // try to determine the projection parameters.. add a right click rename option
               float lonOffset  = 0.0f;
               bool  geocentric = mn->getMap()->isGeocentric();
               if (!geocentric)
               {
                  const osgEarth::SpatialReference* srs = mn->getMap()->getSRS();
                  if (srs)
                  {
                     QString qwkt = srs->getWKT().c_str();
                     int     idx  = qwkt.indexOf("PARAMETER[\"central_meridian\",");
                     if (idx > 0)
                     {
                        idx += 29;
                        int     endidx = qwkt.indexOf("]", idx);
                        QString substr = qwkt.mid(idx, endidx - idx);
                        lonOffset      = substr.toFloat();
                     }
                  }
               }
               QDialog     dialog(this);
               QFormLayout form(&dialog);
               QLineEdit   nameEdit(&dialog);
               nameEdit.setText(sname.c_str());
               form.addRow(new QLabel("Map Name:"), &nameEdit);

               QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
               form.addRow(&buttonBox);
               QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
               QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

               if (dialog.exec() == QDialog::Accepted)
               {
                  sname = nameEdit.text().toStdString();
                  if (!sname.empty())
                  {
                     QString          name = GetUniqueName(sname.c_str());
                     QTreeWidgetItem* map  = new QTreeWidgetItem(mMapTreePtr->invisibleRootItem(), QStringList(name));
                     map->setData(0, Qt::UserRole, cMAP);
                     map->setData(0, Qt::UserRole + 1, name);
                     if (!geocentric)
                     {
                        map->setData(0, Qt::UserRole + 2, QString("CME_EQR"));
                     }
                     else
                     {
                        map->setData(0, Qt::UserRole + 2, QString("CME_WCS"));
                     }
                     map->setData(0, Qt::UserRole + 3, lonOffset);
                     map->setData(0, Qt::UserRole + 4, false);
                     map->setToolTip(0, fileName);

                     QTreeWidgetItem* file = new QTreeWidgetItem(map, QStringList(fileName));
                     file->setData(0, Qt::UserRole, cMAPFILE);
                     file->setHidden(true);
                  }
               }

               // make it with name, geocentric, and lonOffset
               file = 0;
               UpdateProfiles();
               return;
            }
            else
            {
               QMessageBox::warning(this, "File not usable", "OsgEarth file does not contain a map: " + fileName);
               file = 0;
               return;
            }
         }
         QDialog     dialog(this);
         QFormLayout form(&dialog);
         QLineEdit   nameEdit(&dialog);
         form.addRow(new QLabel("Map Name:"), &nameEdit);
         QComboBox projection(&dialog);
         projection.addItem("Round");
         projection.addItem("Flat");
         projection.setCurrentText("Round");
         form.addRow(new QLabel("Projection:"), &projection);
         QSpinBox offset(&dialog);
         offset.setMinimum(-180);
         offset.setMaximum(180);
         offset.setValue(0);
         form.addRow(new QLabel("Lon. Offset (deg):"), &offset);
         QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
         form.addRow(&buttonBox);
         QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
         QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

         if (dialog.exec() == QDialog::Accepted)
         {
            QString name = nameEdit.text();

            if (!name.isEmpty())
            {
               name = GetUniqueName(name);

               QTreeWidgetItem* map = new QTreeWidgetItem(mMapTreePtr->invisibleRootItem(), QStringList(name));
               map->setData(0, Qt::UserRole, cMAP);
               map->setData(0, Qt::UserRole + 1, name);
               if (projection.currentText() == "Flat")
               {
                  map->setData(0, Qt::UserRole + 2, QString("CME_EQR"));
               }
               else
               {
                  map->setData(0, Qt::UserRole + 2, QString("CME_WCS"));
               }
               map->setData(0, Qt::UserRole + 3, offset.value());
               map->setData(0, Qt::UserRole + 4, false);
               map->setToolTip(0, fileName);

               QTreeWidgetItem* file = new QTreeWidgetItem(map, QStringList(fileName));
               file->setData(0, Qt::UserRole, cMAPFILE);
               file->setHidden(true);
            }
         }
         UpdateProfiles();
         // if no projection parameters, ask the user for that information and make it
         file = 0;
         return;
      }
      QMessageBox::warning(this, "File not usable", "Could not load " + fileName);
   }
}

void wkf::SharedMapPreferencesWidget::RemoveMap(QTreeWidgetItem* aMapItem)
{
   QString text = aMapItem->data(0, Qt::UserRole + 1).toString();
   mMapTreePtr->invisibleRootItem()->removeChild(aMapItem);
   if (mMapTreePtr->invisibleRootItem()->childCount() == 0)
   {
      QString name      = "None";
      QString identname = "    ";
      identname += name;
      QTreeWidgetItem* map  = new QTreeWidgetItem(mMapTreePtr->invisibleRootItem(), QStringList(identname));
      QFont            font = map->font(0);
      font.setItalic(true);
      map->setFont(0, font);
      map->setData(0, Qt::UserRole, cMAP);
      map->setData(0, Qt::UserRole + 1, name);
      map->setData(0, Qt::UserRole + 2, QString("CME_WCS"));
      map->setData(0, Qt::UserRole + 3, 0.0f);
      map->setData(0, Qt::UserRole + 4, true);
   }
   UpdateProfiles();
}

void wkf::SharedMapPreferencesWidget::RenameMap(QTreeWidgetItem* aItem)
{
   QDialog     dialog(this);
   QFormLayout form(&dialog);
   QLineEdit   nameEdit(&dialog);
   nameEdit.setText(aItem->data(0, Qt::UserRole + 1).toString());
   form.addRow(new QLabel("Map Name:"), &nameEdit);

   QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
   form.addRow(&buttonBox);
   QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
   QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

   if (dialog.exec() == QDialog::Accepted)
   {
      QString name = nameEdit.text();
      if (!name.isEmpty())
      {
         name = GetUniqueName(name);
         aItem->setText(0, name);
         aItem->setData(0, Qt::UserRole + 1, name);
      }
   }
   UpdateProfiles();
}

QString wkf::SharedMapPreferencesWidget::GetUniqueName(const QString& aName)
{
   if (!NameIsUnique(aName))
   {
      int     idx  = 0;
      QString name = aName + "_" + QString::number(idx);
      while (!NameIsUnique(name))
      {
         idx++;
         name = aName + "_" + QString::number(idx);
      }
      return name;
   }
   return aName;
}

bool wkf::SharedMapPreferencesWidget::NameIsUnique(const QString& aName)
{
   int mapscount = mMapTreePtr->invisibleRootItem()->childCount();
   for (int i = 0; i < mapscount; ++i)
   {
      if (mMapTreePtr->invisibleRootItem()->child(i)->data(0, Qt::UserRole + 1).toString() == aName)
      {
         return false;
      }
   }
   return true;
}
