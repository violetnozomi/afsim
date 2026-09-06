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

#include "WkfMapLayerDockWidget.hpp"

#include <QColorDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QDrag>
#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QPainter>
#include <QScrollBar>
#include <QTreeWidgetItem>
#include <QWidgetAction>

#include "UtQtGradientBar.hpp"
#include "UtQtGradientMap.hpp"
#include "UtQtUnitLineEdit.hpp"
#include "UtTiff.hpp"
#include "UtoCmeTerrain.hpp"
#include "UtoViewer.hpp"
#include "VaWidget.hpp"
#include "WkfObserver.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::MapLayerDockWidget::MapLayerDockWidget(QWidget*        aParent /*= nullptr*/,
                                            Qt::WindowFlags aFlags /*= Qt::WindowFlags()*/)
   : QDockWidget(aParent, aFlags)
{
   mDelDelegate = ut::make_unique<DeleteButtonDelegate>();
   mUI.setupUi(this);
   mUI.mAddButton->setIcon(QIcon::fromTheme("add"));
   connect(mUI.mAddButton, &QPushButton::clicked, this, &MapLayerDockWidget::CreateLayer);
   connect(mUI.mTreeWidget, &QTreeWidget::itemChanged, this, &MapLayerDockWidget::ItemChanged);
   connect(mUI.mTreeWidget, &QTreeWidget::itemClicked, this, &MapLayerDockWidget::ItemClicked);
   mUI.mTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(mUI.mTreeWidget, &QTreeWidget::customContextMenuRequested, this, &MapLayerDockWidget::ContextMenu);

   vespa::VaWidget* standardWidget = vaEnv.GetStandardWidget();
   if (standardWidget != nullptr)
   {
      // accept drag/drop events from the map window, too.
      connect(standardWidget, &vespa::VaWidget::DragEnter, this, &MapLayerDockWidget::DragEnter);
      connect(standardWidget, &vespa::VaWidget::DragDrop, this, &MapLayerDockWidget::DragDrop);
   }
   else
   {
      mCallbacks.Add(wkf::Observer::StandardWidgetChanged.Connect(&MapLayerDockWidget::StandardWidgetChangedCB, this));
   }

   // if the underlying map changes, we need to reapply all of our layers
   mCallbacks.Add(Observer::StandardMapChanged.Connect(&MapLayerDockWidget::ReapplyLayers, this));

   mUI.mTreeWidget->setItemDelegateForColumn(2, mDelDelegate.get());

   setAcceptDrops(true);

   // map layers dialog is default hidden
   hide();
}

void wkf::MapLayerDockWidget::CreateLayer()
{
   Viewer* v = vaEnv.GetStandardViewer();
   if (nullptr == v)
   {
      QMessageBox::warning(this, "No Viewer", "No map viewer is available.  Map layers may not be added.");
      return;
   }
   else
   {
      UtoCmeTerrain* terrain = dynamic_cast<UtoCmeTerrain*>(&v->GetViewer()->World().Terrain());
      if (terrain == nullptr)
      {
         QMessageBox::warning(this, "Incompatible map type", "The displayed map type does not support map layers.");
      }
      else if (!terrain->SupportsLayers())
      {
         QMessageBox::warning(this, "Incompatible map", "The displayed map does not support map layers.");
      }
      else
      {
         bool      read = false;
         QFileInfo name = QFileDialog::getOpenFileName(this,
                                                       "Open layer source file...",
                                                       QString(),
                                                       "*.shp *.tif *.tiff *.jpg *.jpeg *.png *.bmp");
         if (name.suffix() == "shp")
         {
            read = CreateLayerShape(name, *terrain);
         }
         else if ((name.suffix() == "tif") || (name.suffix() == "tiff"))
         {
            read = CreateLayerTiff(name, *terrain);
         }
         if (!read && name.exists())
         {
            CreateLayerImage(name, *terrain);
         }
      }
   }
   mUI.mTreeWidget->resizeColumnToContents(0);
   mUI.mTreeWidget->resizeColumnToContents(1);
   mUI.mTreeWidget->resizeColumnToContents(2);
}

bool wkf::MapLayerDockWidget::CreateLayerShape(const QFileInfo& aFilename, UtoCmeTerrain& aTerrain)
{
   RemoveItemsAssociatedWithFile(aFilename);

   QString                               dbfFilename = aFilename.absolutePath() + "/" + aFilename.baseName() + ".dbf";
   std::map<char, std::set<std::string>> preview;
   PreviewDbfFields(dbfFilename.toStdString(), preview);

   auto pc = preview.find('C');
   if (pc != preview.end())
   {
      if (pc->second.find("WSFCOLOR") != pc->second.end())
      {
         if (aTerrain.AddShapeLayer(aFilename.absoluteFilePath().toStdString()))
         {
            QTreeWidgetItem* item = new QTreeWidgetItem(mUI.mTreeWidget);
            item->setIcon(0, QIcon::fromTheme("zone"));
            item->setData(0, cTYPE, cSHAPE);
            item->setData(0, cNAME, aFilename.absoluteFilePath());
            item->setData(0, cFILE, aFilename.absoluteFilePath());
            item->setText(1, aFilename.fileName());
            item->setCheckState(0, Qt::Checked);
            mUI.mTreeWidget->addTopLevelItem(item);
         }
         return true;
      }
   }
   // it would be nice to preview the DBF here and allow the user to color by field, apply height by field, etc...
   QColor c = QColorDialog::getColor(Qt::yellow, this, "Choose color...");
   if (c.isValid())
   {
      if (aTerrain.AddShapeLayer(aFilename.absoluteFilePath().toStdString(), c.redF(), c.greenF(), c.blueF()))
      {
         QTreeWidgetItem* item = new QTreeWidgetItem(mUI.mTreeWidget);
         item->setIcon(0, QIcon::fromTheme("zone"));
         item->setData(0, cTYPE, cSHAPE);
         item->setData(0, cNAME, aFilename.absoluteFilePath());
         item->setData(0, cFILE, aFilename.absoluteFilePath());
         item->setData(0, cCOLOR, c);
         item->setText(1, aFilename.fileName());
         item->setCheckState(0, Qt::Checked);
         mUI.mTreeWidget->addTopLevelItem(item);
      }
   }
   return true;
}

bool wkf::MapLayerDockWidget::CreateLayerTiff(const QFileInfo& aFilename, UtoCmeTerrain& aTerrain)
{
   RemoveItemsAssociatedWithFile(aFilename);
   bool retval = false;
   // it would be nice to preview the tiff header here to allow color gradients for mono 16b and auto-detect srs availability
   UtTiff::PreviewList preview = UtTiff::PreviewHeaderInformation(aFilename.absoluteFilePath().toStdString());
   unsigned int        idx     = 0;
   for (auto&& imageData : preview)
   {
      QString iname = aFilename.absoluteFilePath();
      if (idx > 0)
      {
         iname = QString::number(idx) + ":" + aFilename.absoluteFilePath();
      }
      if (imageData.mIsGeotiff)
      {
         if (imageData.mIsSensorPlot)
         {
            std::map<float, UtColor> stops;
            mGradientMap.GetUtStops(UtQtGradientMap::cDEFAULT_GRADIENT_KEY, stops);
            if (aTerrain.AddSensorPlotImageLayer(aFilename.absoluteFilePath().toStdString(),
                                                 iname.toStdString(),
                                                 imageData.mMinValue,
                                                 imageData.mMaxValue,
                                                 idx,
                                                 stops))
            {
               QTreeWidgetItem* item = new QTreeWidgetItem(mUI.mTreeWidget);
               item->setIcon(0, QIcon::fromTheme("radar"));
               item->setData(0, cTYPE, cSENSORPLOTIMAGE);
               item->setData(0, cNAME, iname);
               item->setData(0, cFILE, aFilename.absoluteFilePath());
               item->setData(0, cMINVALUE, imageData.mMinValue);
               item->setData(0, cMAXVALUE, imageData.mMaxValue);
               item->setData(0, cSUBDATA, idx);
               item->setData(0, cOPACITY, 100);
               item->setData(0, cGRADIENT, UtQtGradientMap::cDEFAULT_GRADIENT_KEY);
               if (!imageData.mDescription.empty())
               {
                  item->setText(1, imageData.mDescription.c_str());
               }
               else
               {
                  item->setText(1, QString::number(idx) + ":" + aFilename.fileName());
               }
               item->setCheckState(0, Qt::Checked);
               mUI.mTreeWidget->addTopLevelItem(item);
            }
         }
         else if (aTerrain.AddImageLayer(aFilename.absoluteFilePath().toStdString()))
         {
            QTreeWidgetItem* item = new QTreeWidgetItem(mUI.mTreeWidget);
            QString          fn   = aFilename.absoluteFilePath();
            QString          f    = aFilename.fileName();
            item->setIcon(0, QIcon::fromTheme("globe"));
            item->setData(0, cTYPE, cGEOIMAGE);
            item->setData(0, cNAME, iname);
            item->setData(0, cFILE, aFilename.absoluteFilePath());
            item->setData(0, cOPACITY, 100);
            item->setText(1, QString::number(idx) + ":" + aFilename.fileName());
            item->setCheckState(0, Qt::Checked);
            mUI.mTreeWidget->addTopLevelItem(item);
         }
         else
         {
            QMessageBox::warning(this,
                                 "Error reading SRS",
                                 "The image has unusable SRS data so it can not be placed automatically.");
         }
         retval = true;
      }
      idx++;
   }
   return retval;
}

void wkf::MapLayerDockWidget::CreateLayerImage(const QFileInfo& aFilename, UtoCmeTerrain& aTerrain)
{
   RemoveItemsAssociatedWithFile(aFilename);

   QDialog      constraints;
   QFormLayout* form = new QFormLayout(&constraints);
   constraints.setLayout(form);
   UtQtLatPosLineEdit* minLat = new UtQtLatPosLineEdit(&constraints);
   UtQtLatPosLineEdit* maxLat = new UtQtLatPosLineEdit(&constraints);
   UtQtLonPosLineEdit* minLon = new UtQtLonPosLineEdit(&constraints);
   UtQtLonPosLineEdit* maxLon = new UtQtLonPosLineEdit(&constraints);
   form->addRow("Min Latitude", minLat);
   form->addRow("Max Latitude", maxLat);
   form->addRow("Min Longitude", minLon);
   form->addRow("Max Longitude", maxLon);
   QDialogButtonBox* bb =
      new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &constraints);
   constraints.layout()->addWidget(bb);
   connect(bb, &QDialogButtonBox::accepted, &constraints, &QDialog::accept);
   connect(bb, &QDialogButtonBox::rejected, &constraints, &QDialog::reject);
   int results = constraints.exec();
   if (results == QDialog::Accepted)
   {
      if (aTerrain.AddImageLayer(aFilename.absoluteFilePath().toStdString(),
                                 minLat->GetValue(),
                                 minLon->GetValue(),
                                 maxLat->GetValue(),
                                 maxLon->GetValue()))
      {
         QTreeWidgetItem* item = new QTreeWidgetItem(mUI.mTreeWidget);
         item->setIcon(0, QIcon::fromTheme("globe"));
         item->setData(0, cTYPE, cRAWIMAGE);
         item->setData(0, cNAME, aFilename.absoluteFilePath());
         item->setData(0, cFILE, aFilename.absoluteFilePath());
         item->setData(0, cMINLAT, static_cast<double>(minLat->GetValue()));
         item->setData(0, cMAXLAT, static_cast<double>(maxLat->GetValue()));
         item->setData(0, cMINLON, static_cast<double>(minLon->GetValue()));
         item->setData(0, cMAXLON, static_cast<double>(maxLon->GetValue()));
         item->setData(0, cOPACITY, 100);
         item->setText(1, aFilename.fileName());
         item->setCheckState(0, Qt::Checked);
         mUI.mTreeWidget->addTopLevelItem(item);
      }
   }
}

void wkf::MapLayerDockWidget::ItemChanged(QTreeWidgetItem* aItemPtr)
{
   Viewer* v = vaEnv.GetStandardViewer();
   if (nullptr != v)
   {
      UtoCmeTerrain* terrain = dynamic_cast<UtoCmeTerrain*>(&v->GetViewer()->World().Terrain());
      if ((terrain != nullptr) && (terrain->SupportsLayers()))
      {
         QString   name      = aItemPtr->data(0, cNAME).toString();
         bool      state     = (aItemPtr->checkState(0) == Qt::Checked) ? true : false;
         LayerType layerType = static_cast<LayerType>(aItemPtr->data(0, Qt::UserRole).toInt());
         if ((layerType == cSENSORPLOTIMAGE) && state)
         {
            int count = mUI.mTreeWidget->topLevelItemCount();
            for (int i = 0; i < count; ++i)
            {
               auto item = mUI.mTreeWidget->topLevelItem(i);
               if ((item != aItemPtr) && (item->data(0, cTYPE).toInt() == cSENSORPLOTIMAGE))
               {
                  item->setCheckState(0, Qt::Unchecked);
                  terrain->ShowLayer(item->data(0, cNAME).toString().toStdString(), false);
               }
            }
         }
         terrain->ShowLayer(name.toStdString(), state);
      }
   }
}

void wkf::MapLayerDockWidget::ItemClicked(QTreeWidgetItem* aItemPtr)
{
   QPoint cur = QCursor::pos();
   cur        = mUI.mTreeWidget->mapFromGlobal(cur);
   int v      = mUI.mTreeWidget->horizontalScrollBar()->value();
   if (cur.x() + v > mUI.mTreeWidget->width() - mUI.mTreeWidget->visualItemRect(aItemPtr).height())
   {
      EraseItemsLayer(*aItemPtr);
      delete aItemPtr;
   }
}

void wkf::MapLayerDockWidget::EraseItemsLayer(const QTreeWidgetItem& aItem)
{
   Viewer* v = vaEnv.GetStandardViewer();
   if (nullptr != v)
   {
      UtoCmeTerrain* terrain = dynamic_cast<UtoCmeTerrain*>(&v->GetViewer()->World().Terrain());
      if ((terrain != nullptr) && (terrain->SupportsLayers()))
      {
         std::string filename = aItem.data(0, cNAME).toString().toStdString();
         terrain->RemoveLayer(filename);
      }
   }
}

// dbase reference: http://web.archive.org/web/20150323061445/http://ulisse.elettra.trieste.it/services/doc/dbase/DBFstruct.htm

void wkf::MapLayerDockWidget::PreviewDbfFields(const std::string& aFile, std::map<char, std::set<std::string>>& aFields)
{
   std::ifstream dbf(aFile, std::ios::binary);
   if (dbf.is_open())
   {
      dbf.seekg(32);
      while ((!dbf.eof()) && (dbf.peek() != 0x0d))
      {
         char fieldName[12];

         dbf.read(fieldName, 11); // read 11 bytes
         fieldName[11] = '\0';    // terminate it so we can use it in a std::string safely
         char c;
         dbf.read(&c, 1);
         aFields[c].insert(fieldName);
         dbf.seekg(20, std::ios_base::cur);
      }
      dbf.close();
   }
}

void wkf::MapLayerDockWidget::AddSensorPlotLayerImage(const QTreeWidgetItem& aItem, UtoCmeTerrain& aTerrain)
{
   QString                  stopsKey = aItem.data(0, cGRADIENT).toString();
   std::map<float, UtColor> stops;
   mGradientMap.GetUtStops(stopsKey, stops);

   std::string filename = aItem.data(0, cFILE).toString().toStdString();
   if (aTerrain.AddSensorPlotImageLayer(filename,
                                        aItem.data(0, cNAME).toString().toStdString(),
                                        aItem.data(0, cMINVALUE).toDouble(),
                                        aItem.data(0, cMAXVALUE).toDouble(),
                                        aItem.data(0, cSUBDATA).toInt(),
                                        stops))
   {
      aTerrain.ShowLayer(aItem.data(0, cNAME).toString().toStdString(), aItem.checkState(0) == Qt::Checked);
      aTerrain.SetImageLayerOpacity(aItem.data(0, cNAME).toString().toStdString(),
                                    aItem.data(0, cOPACITY).toInt() / 100.0f);
   }
}

void wkf::MapLayerDockWidget::dragEnterEvent(QDragEnterEvent* aEvent)
{
   DragEnter(aEvent);
}

void wkf::MapLayerDockWidget::dropEvent(QDropEvent* aEvent)
{
   DragDrop(aEvent);
}

void wkf::MapLayerDockWidget::DragEnter(QDragEnterEvent* aEvent)
{
   bool         ok = false;
   wkf::Viewer* v  = vaEnv.GetStandardViewer();
   if (nullptr != v)
   {
      UtoCmeTerrain* terrain = dynamic_cast<UtoCmeTerrain*>(&v->GetViewer()->World().Terrain());
      if (terrain != nullptr)
      {
         if (aEvent->mimeData()->hasUrls())
         {
            ok        = true;
            auto urls = aEvent->mimeData()->urls();
            for (auto&& url : urls)
            {
               QString str = url.toLocalFile();
               if (!str.endsWith(".shp", Qt::CaseInsensitive) && !str.endsWith(".tif", Qt::CaseInsensitive) &&
                   !str.endsWith(".tiff", Qt::CaseInsensitive) && !str.endsWith(".jpg", Qt::CaseInsensitive) &&
                   !str.endsWith(".jpeg", Qt::CaseInsensitive) && !str.endsWith(".png", Qt::CaseInsensitive) &&
                   !str.endsWith(".bmp", Qt::CaseInsensitive))
               {
                  ok = false;
               }
            }
         }
      }
   }
   if (ok)
   {
      aEvent->setAccepted(aEvent->mimeData()->hasUrls());
      aEvent->setDropAction(Qt::DropAction::LinkAction);
   }
}

void wkf::MapLayerDockWidget::DragDrop(QDropEvent* aEvent)
{
   wkf::Viewer* v = vaEnv.GetStandardViewer();
   if (nullptr != v)
   {
      UtoCmeTerrain* terrain = dynamic_cast<UtoCmeTerrain*>(&v->GetViewer()->World().Terrain());
      if (terrain != nullptr)
      {
         if (aEvent->mimeData()->hasUrls())
         {
            auto urls = aEvent->mimeData()->urls();
            for (auto&& url : urls)
            {
               QString str = url.toLocalFile();
               if (str.endsWith(".shp", Qt::CaseInsensitive))
               {
                  CreateLayerShape(str, *terrain);
               }
               else if (str.endsWith(".tif", Qt::CaseInsensitive) || str.endsWith(".tiff", Qt::CaseInsensitive))
               {
                  CreateLayerTiff(str, *terrain);
               }
               else if (str.endsWith(".jpg", Qt::CaseInsensitive) || str.endsWith(".jpeg", Qt::CaseInsensitive) ||
                        str.endsWith(".png", Qt::CaseInsensitive) || str.endsWith(".bmp"))
               {
                  CreateLayerImage(str, *terrain);
               }
            }
         }
      }
   }
}

void wkf::MapLayerDockWidget::StandardWidgetChangedCB(vespa::VaWidget* aWidgetPtr)
{
   if (nullptr != aWidgetPtr)
   {
      connect(aWidgetPtr, &vespa::VaWidget::DragEnter, this, &MapLayerDockWidget::DragEnter);
      connect(aWidgetPtr, &vespa::VaWidget::DragDrop, this, &MapLayerDockWidget::DragDrop);
   }
}

void wkf::MapLayerDockWidget::ReapplyLayers()
{
   Viewer* v = vaEnv.GetStandardViewer();
   if (nullptr != v)
   {
      UtoCmeTerrain* terrain = dynamic_cast<UtoCmeTerrain*>(&v->GetViewer()->World().Terrain());
      if ((terrain != nullptr) && (terrain->SupportsLayers()))
      {
         int count = mUI.mTreeWidget->topLevelItemCount();
         for (int i = 0; i < count; ++i)
         {
            auto item = mUI.mTreeWidget->topLevelItem(i);

            LayerType   layerType = static_cast<LayerType>(item->data(0, Qt::UserRole).toInt());
            std::string filename  = item->data(0, cFILE).toString().toStdString();
            if (layerType == cSHAPE)
            {
               QColor c = item->data(0, cCOLOR).value<QColor>();
               terrain->AddShapeLayer(filename, c.redF(), c.greenF(), c.blueF()); // load it
               terrain->ShowLayer(filename, item->checkState(0) == Qt::Checked);  // show/hide it
            }
            else if (layerType == cGEOIMAGE)
            {
               if (terrain->AddImageLayer(filename)) // load it
               {
                  terrain->ShowLayer(filename, item->checkState(0) == Qt::Checked); // show/hide it
                  terrain->SetImageLayerOpacity(item->data(0, cNAME).toString().toStdString(),
                                                item->data(0, cOPACITY).toInt() / 100.0f);
               }
            }
            else if (layerType == cRAWIMAGE)
            {
               double minLat = item->data(0, cMINLAT).toDouble();
               double maxLat = item->data(0, cMAXLAT).toDouble();
               double minLon = item->data(0, cMINLON).toDouble();
               double maxLon = item->data(0, cMAXLON).toDouble();
               if (terrain->AddImageLayer(filename, minLat, minLon, maxLat, maxLon)) // load it
               {
                  terrain->ShowLayer(filename, item->checkState(0) == Qt::Checked); // show/hide it
                  terrain->SetImageLayerOpacity(item->data(0, cNAME).toString().toStdString(),
                                                item->data(0, cOPACITY).toInt() / 100.0f);
               }
            }
            else if (layerType == cSENSORPLOTIMAGE)
            {
               AddSensorPlotLayerImage(*item, *terrain);
            }
         }
      }
   }
}

void wkf::MapLayerDockWidget::ContextMenu(const QPoint& aPoint)
{
   Viewer* v = vaEnv.GetStandardViewer();
   if (nullptr != v)
   {
      UtoCmeTerrain* terrain = dynamic_cast<UtoCmeTerrain*>(&v->GetViewer()->World().Terrain());
      if ((terrain != nullptr) && (terrain->SupportsLayers()))
      {
         QMenu            menu;
         QTreeWidgetItem* itemPtr = mUI.mTreeWidget->itemAt(aPoint);
         if (itemPtr)
         {
            int itemType = itemPtr->data(0, cTYPE).toInt();
            if (itemType == cSHAPE)
            {
               QAction* action = new QAction("Set Color...", &menu);
               connect(action,
                       &QAction::triggered,
                       [this, itemPtr, terrain]()
                       {
                          QColor c =
                             QColorDialog::getColor(itemPtr->data(0, cCOLOR).value<QColor>(), this, "Choose Vector Color");
                          if (c.isValid())
                          {
                             terrain->SetShapeLayerColor(itemPtr->data(0, cFILE).toString().toStdString(),
                                                         c.redF(),
                                                         c.greenF(),
                                                         c.blueF());
                          }
                       });
               menu.addAction(action);
            }
            else if (itemType == cSENSORPLOTIMAGE)
            {
               QAction* rangeAction = new QAction("Set Range...", &menu);
               menu.addAction(rangeAction);
               connect(rangeAction,
                       &QAction::triggered,
                       [this, itemPtr, terrain]()
                       {
                          QDialog      constraints;
                          QFormLayout* form = new QFormLayout(&constraints);
                          constraints.setLayout(form);
                          QDoubleSpinBox* minV     = new QDoubleSpinBox(&constraints);
                          QDoubleSpinBox* maxV     = new QDoubleSpinBox(&constraints);
                          double          minvalue = itemPtr->data(0, cMINVALUE).toDouble();
                          double          maxvalue = itemPtr->data(0, cMAXVALUE).toDouble();
                          minV->setMinimum(-300000);
                          minV->setMaximum(300000);
                          minV->setValue(minvalue);
                          maxV->setMinimum(-300000);
                          maxV->setMaximum(300000);
                          maxV->setValue(maxvalue);
                          form->addRow("Min Value", minV);
                          form->addRow("Max Value", maxV);
                          QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                                      Qt::Horizontal,
                                                                      &constraints);
                          constraints.layout()->addWidget(bb);
                          connect(bb, &QDialogButtonBox::accepted, &constraints, &QDialog::accept);
                          connect(bb, &QDialogButtonBox::rejected, &constraints, &QDialog::reject);
                          int results = constraints.exec();
                          if (results == QDialog::Accepted)
                          {
                             itemPtr->setData(0, cMINVALUE, minV->value());
                             itemPtr->setData(0, cMAXVALUE, maxV->value());
                             std::map<float, UtColor> stops;
                             mGradientMap.GetUtStops(itemPtr->data(0, cGRADIENT).toString(), stops);

                             terrain->SetSensorPlotLayerGradient(itemPtr->data(0, cNAME).toString().toStdString(),
                                                                 minV->value(),
                                                                 maxV->value(),
                                                                 stops);
                          }
                       });
               QMenu* gmenu = new QMenu("Gradients", &menu);
               menu.addMenu(gmenu);
               UtQtGradientAction* defAction =
                  new UtQtGradientAction(gmenu,
                                         mGradientMap.GradientStops(UtQtGradientMap::cDEFAULT_GRADIENT_KEY),
                                         UtQtGradientMap::cDEFAULT_GRADIENT_KEY);
               gmenu->addAction(defAction);
               UtQtGradientAction* fireAction =
                  new UtQtGradientAction(gmenu,
                                         mGradientMap.GradientStops(UtQtGradientMap::cFIRE_GRADIENT_KEY),
                                         UtQtGradientMap::cFIRE_GRADIENT_KEY);
               gmenu->addAction(fireAction);
               UtQtGradientAction* iceAction =
                  new UtQtGradientAction(gmenu,
                                         mGradientMap.GradientStops(UtQtGradientMap::cICE_GRADIENT_KEY),
                                         UtQtGradientMap::cICE_GRADIENT_KEY);
               gmenu->addAction(iceAction);
               UtQtGradientAction* boneAction =
                  new UtQtGradientAction(gmenu,
                                         mGradientMap.GradientStops(UtQtGradientMap::cBONE_GRADIENT_KEY),
                                         UtQtGradientMap::cBONE_GRADIENT_KEY);
               gmenu->addAction(boneAction);
               UtQtGradientAction* hsvAction =
                  new UtQtGradientAction(gmenu,
                                         mGradientMap.GradientStops(UtQtGradientMap::cHSV_GRADIENT_KEY),
                                         UtQtGradientMap::cHSV_GRADIENT_KEY);
               gmenu->addAction(hsvAction);
               UtQtGradientAction* halfAction =
                  new UtQtGradientAction(gmenu,
                                         mGradientMap.GradientStops(UtQtGradientMap::cHALF_GRADIENT_KEY),
                                         UtQtGradientMap::cHALF_GRADIENT_KEY);
               gmenu->addAction(halfAction);
               connect(gmenu,
                       &QMenu::triggered,
                       [this, itemPtr, terrain](QAction* aAction)
                       {
                          UtQtGradientAction* action = dynamic_cast<UtQtGradientAction*>(aAction);
                          if (action)
                          {
                             itemPtr->setData(0, cGRADIENT, action->GetKey());
                             std::map<float, UtColor> stops;
                             mGradientMap.GetUtStops(action->GetKey(), stops);

                             terrain->SetSensorPlotLayerGradient(itemPtr->data(0, cNAME).toString().toStdString(),
                                                                 itemPtr->data(0, cMINVALUE).toDouble(),
                                                                 itemPtr->data(0, cMAXVALUE).toDouble(),
                                                                 stops);
                          }
                       });
            }
            if ((itemType == cSENSORPLOTIMAGE) || (itemType == cGEOIMAGE) || (itemType == cRAWIMAGE))
            {
               QAction* taction = new QAction("Set Opacity...", &menu);
               menu.addAction(taction);
               connect(taction,
                       &QAction::triggered,
                       [this, itemPtr, &menu, terrain]()
                       {
                          QDialog  d(&menu, Qt::Popup);
                          QSlider* slider = new QSlider(&d);
                          slider->setMinimum(0);
                          slider->setMaximum(100);
                          slider->setValue(itemPtr->data(0, cOPACITY).toInt());
                          slider->setOrientation(Qt::Horizontal);
                          connect(slider,
                                  &QSlider::valueChanged,
                                  [this, itemPtr, terrain](int aValue)
                                  {
                                     float       t        = aValue / 100.0f;
                                     std::string filename = itemPtr->data(0, cFILE).toString().toStdString();
                                     terrain->SetImageLayerOpacity(itemPtr->data(0, cNAME).toString().toStdString(), t);
                                  });
                          d.setLayout(new QVBoxLayout());
                          d.layout()->addWidget(slider);
                          d.exec();
                          itemPtr->setData(0, cOPACITY, slider->value());
                       });
            }
            if (!menu.isEmpty())
            {
               menu.exec(mUI.mTreeWidget->mapToGlobal(aPoint));
            }
         }
      }
   }
}

void wkf::MapLayerDockWidget::RemoveItemsAssociatedWithFile(const QFileInfo& aFilename)
{
   std::set<QTreeWidgetItem*> toErase;
   for (int i = 0; i < mUI.mTreeWidget->topLevelItemCount(); ++i)
   {
      QTreeWidgetItem* item = mUI.mTreeWidget->topLevelItem(i);
      if (nullptr != item)
      {
         if (item->data(0, cFILE) == aFilename.absoluteFilePath())
         {
            EraseItemsLayer(*item);
            toErase.insert(item);
         }
      }
   }
   for (auto&& item : toErase)
   {
      delete item;
   }
}

void wkf::DeleteButtonDelegate::paint(QPainter* aPainter, const QStyleOptionViewItem& aOption, const QModelIndex& aIndex) const
{
   QRect r = aOption.rect;
   r.setTop(r.top() + 1);
   r.setHeight(r.height() - 1);
   r.setLeft(r.right() - r.height());
   r.setWidth(r.height());
   aPainter->drawPixmap(r, QIcon::fromTheme("delete").pixmap(r.height(), r.height()));
}
