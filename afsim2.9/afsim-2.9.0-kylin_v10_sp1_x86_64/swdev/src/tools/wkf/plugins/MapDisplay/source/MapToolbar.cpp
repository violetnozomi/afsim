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

#include "MapToolbar.hpp"

#include <QBuffer>
#include <QFile>
#include <QLayout>
#include <QMenu>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTreeView>
#include <QUrl>

#include "UtQtMemory.hpp"
#include "VaCamera.hpp"
#include "VaWidget.hpp"
#include "WkfAction.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfUnitsObject.hpp"
#include "WkfUtils.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"

Map::ToolBar::ToolBar()
   : QToolBar("Map Toolbar")
{
   setIconSize(QSize(16, 16));
   setObjectName("MapToolBar");
   connect(ut::qt::make_qt_ptr<wkf::Action>(QIcon::fromTheme("find"), "Search", this),
           &QAction::triggered,
           this,
           &ToolBar::GeoSearch);
   mCaptureAction = ut::qt::make_qt_ptr<wkf::Action>(QIcon::fromTheme("add_view"),
                                                     "Capture View",
                                                     this,
                                                     QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_C));
   connect(mCaptureAction, &QAction::triggered, this, &ToolBar::Capture);

   setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, &QToolBar::customContextMenuRequested, this, &ToolBar::ContextMenu);
}

void Map::ToolBar::Clear()
{
   mDataMap.clear();
   for (auto action : actions())
   {
      if (action->text() == cDYNAMIC_BUTTON_TEXT)
      {
         removeAction(action);
      }
   }
}

void Map::ToolBar::Capture()
{
   if (actions().size() < 21)
   {
      QString vlink = wkf::util::ParseLinks(vaEnv.GetStandardViewer()->GetViewLink(), false);
      QPixmap pm    = vaEnv.GetStandardWidget()->grab().scaledToHeight(128, Qt::SmoothTransformation);
      AddCapture(vlink, pm);
   }
   else
   {
      QMessageBox::warning(this, "Map Toolbar-full", "The map toolbar is limited to 20 captures.  Please, right click on an existing capture and select \"Delete Capture\" before trying again.");
   }
}

void Map::ToolBar::AddCapture(const QString& aLink, const QPixmap& aPM)
{
   // create an action with embedded link
   QByteArray data;
   QBuffer    buffer(&data);
   aPM.save(&buffer, "PNG", 100);
   QIcon icon(aPM);

   QAction* action = new QAction(icon, cDYNAMIC_BUTTON_TEXT, this);
   action->setData(QVariant::fromValue<size_t>(mDataMap.size()));
   mDataMap.emplace_back(aLink, aPM);

   QString tooltip = QString("<img src='data:image/png;base64, %0'>").arg(QString(data.toBase64()));
   action->setToolTip(tooltip);
   addAction(action);
   connect(action,
           &QAction::triggered,
           [aLink]()
           {
              QString errmsg;
              wkf::util::FollowLink(QUrl(aLink), errmsg);
           });
}

void Map::ToolBar::ContextMenu(QPoint aPos)
{
   QAction* hitaction = actionAt(aPos);
   if (hitaction != nullptr)
   {
      if (hitaction->text() == cDYNAMIC_BUTTON_TEXT)
      {
         QMenu menu(this);
         menu.addAction("Delete Capture");
         menu.move(mapToGlobal(aPos));
         QAction* action = menu.exec();
         if (action)
         {
            delete hitaction;
         }
      }
   }
}


void Map::ToolBar::LoadSettings(QSettings& aSettings)
{
   Clear();
   int items = aSettings.beginReadArray("MapToolBar");
   for (int i = 0; i < items; ++i)
   {
      aSettings.setArrayIndex(i);
      QString link = aSettings.value("link").toString();
      QPixmap icon = aSettings.value("image").value<QPixmap>();

      AddCapture(link, icon);
   }
   aSettings.endArray();
}

void Map::ToolBar::SaveSettings(QSettings& aSettings)
{
   int i = 0;
   aSettings.beginWriteArray("MapToolBar");
   QList<QAction*> acts = actions();
   for (auto&& act : acts)
   {
      if (act->text() == cDYNAMIC_BUTTON_TEXT)
      {
         aSettings.setArrayIndex(i++);
         size_t idx = act->data().toInt();
         aSettings.setValue("link", mDataMap[idx].first);
         aSettings.setValue("image", mDataMap[idx].second);
      }
   }
   aSettings.endArray();
}

void Map::ToolBar::BuildDatabase(const QString&      aFilename,
                                 const QString&      aType,
                                 const QString&      aIcon,
                                 QStandardItemModel& aModel,
                                 double              aTanFOVY)
{
   QFile file(aFilename);
   file.open(QFile::ReadOnly | QFile::Text);
   while (!file.atEnd())
   {
      QString     line = file.readLine();
      QStringList list = line.split(',');
      auto*       item = new QStandardItem(list[0]);
      aModel.appendRow(item);
      double dist = list[3].toDouble();
      QString link = "wkf://view?" + list[1] + "," + list[2] + "," + QString::number(2.0 * dist / aTanFOVY) + ",0,0,\"\"";

      item->setData(link, Qt::UserRole);
      item->setData(aType, Qt::UserRole + 1);
      if (!aIcon.isEmpty())
      {
         item->setIcon(QIcon::fromTheme(aIcon));
      }
   }
   file.close();
}

void Map::ToolBar::GeoSearch()
{
   double tanfovy = 1;
   if (vaEnv.GetStandardViewer())
   {
      tanfovy = std::tan(vaEnv.GetStandardViewer()->GetCamera()->GetFovY() * UtMath::cRAD_PER_DEG);
   }

   if (mGeoSearchDlg)
   {
      mGeoSearchDlg->show();
      mGeoSearchDlg->raise();
   }
   else
   {
      mGeoSearchDlg = ut::qt::make_qt_ptr<QDialog>(this, Qt::Tool);
      mGeoSearchDlg->setWindowTitle("Geo-search");
      auto* layout   = ut::qt::make_qt_ptr<QVBoxLayout>(mGeoSearchDlg);
      auto* filter   = ut::qt::make_qt_ptr<QLineEdit>(mGeoSearchDlg);
      auto* listView = ut::qt::make_qt_ptr<QTreeView>(mGeoSearchDlg);
      layout->addWidget(filter);
      layout->addWidget(listView);

      mGeoSearchDlg->show();
      auto* model       = ut::qt::make_qt_ptr<QStandardItemModel>(listView);
      auto* filterModel = ut::qt::make_qt_ptr<QSortFilterProxyModel>(listView);
      filterModel->setSourceModel(model);
      listView->setModel(filterModel);
      listView->setHeaderHidden(true);
      listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
      listView->setContextMenuPolicy(Qt::CustomContextMenu);

      BuildDatabase(":/countries.csv", "country", "flag", *model, tanfovy);
      BuildDatabase(":/airports.csv", "airport", "airplane", *model, tanfovy);

      filterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
      connect(filter, &QLineEdit::textChanged, filterModel, &QSortFilterProxyModel::setFilterFixedString);
      connect(listView,
              &QTreeView::clicked,
              [model, filterModel](const QModelIndex& aItemIndex)
              {
                 auto srcIndex = filterModel->mapToSource(aItemIndex);
                 if (srcIndex.isValid())
                 {
                    QString errmsg;
                    wkf::util::FollowLink(QUrl(model->itemFromIndex(srcIndex)->data(Qt::UserRole).toString()), errmsg);
                 }
              });
      connect(listView,
              &QTreeView::customContextMenuRequested,
              [this, listView, model, filterModel](const QPoint& aPoint)
              { MenuItemContextMenu(listView, model, filterModel, aPoint); });
   }
}

void Map::ToolBar::MenuItemContextMenu(QTreeView*             aView,
                                       QStandardItemModel*    aModel,
                                       QSortFilterProxyModel* aFilterModel,
                                       const QPoint&          aPoint) const
{
   if (vaEnv.GetStandardViewer())
   {
      auto findex = aView->indexAt(aPoint);
      if (findex.isValid())
      {
         auto index = aFilterModel->mapToSource(findex);
         if (index.isValid())
         {
            auto pos = wkf::util::GetPositionFromUrl(QUrl(aModel->itemFromIndex(index)->data(Qt::UserRole).toString()));
            auto mpp = vaEnv.GetCurrentCursorMetersPerPixel();
            vaEnv.SetCurrentCursorData(pos, mpp);

            QMenu menu;
            wkfEnv.BuildViewerContextMenu(&menu, vaEnv.GetStandardViewer());
            for (auto a : menu.actions())
            {
               if (a->text() != "Add at Location")
               {
                  menu.removeAction(a);
               }
            }
            connect(menu.addAction("Copy Location to Clipboard"),
                    &QAction::triggered,
                    [pos]()
                    {
                       vespa::VaPosition curPos = pos;

                       std::ostringstream oss;
                       curPos.SetLatFormat(wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLatLonFormatting());
                       curPos.SetLonFormat(wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLatLonFormatting());
                       oss << curPos.GetLat() << ' ' << curPos.GetLon();

                       QString text(oss.str().c_str());
                       qApp->clipboard()->setText(text);
                       wkfEnv.GetMainWindow()->ShowStatusMessage(QString("Location %1 copied to clipboard").arg(text),
                                                                 2000);
                    });
            menu.exec(aView->mapToGlobal(aPoint));
         }
      }
   }
}
