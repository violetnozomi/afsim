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
#include "MapCursorStatusDockWidget.hpp"

#include <sstream>

#include <QDrag>
#include <QMimeData>

#include "UtAngle.hpp"
#include "VaObserver.hpp"
#include "VaViewer.hpp"
#include "WkfBullseye.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfScenario.hpp"
#include "WkfUnitsObject.hpp"
#include "WkfVtkEnvironment.hpp"

Map::CursorStatusDockWidget::CursorStatusDockWidget(const vespa::VaViewer& aViewer,
                                                    size_t                 aPluginUniqueId,
                                                    QWidget*               parent,
                                                    Qt::WindowFlags        flags)
   : QDockWidget(parent, flags)
   , mViewer(aViewer)
   , mMouseX(-1)
   , mMouseY(-1)
   , mPluginUniqueId(aPluginUniqueId)
{
   mUI.setupUi(this);

   mLatitudeItemPtr  = new QTreeWidgetItem(mUI.treeWidget, {"Latitude", ""});
   mLongitudeItemPtr = new QTreeWidgetItem(mUI.treeWidget, {"Longitude", ""});
   mElevationItemPtr = new QTreeWidgetItem(mUI.treeWidget, {"Elevation", ""});
   mBearingItemPtr   = new QTreeWidgetItem(mUI.treeWidget, {"Bearing (Bullseye)", "n/a"});
   mRangeItemPtr     = new QTreeWidgetItem(mUI.treeWidget, {"Range (Bullseye)", "n/a"});
   mBullseyeItemPtr  = new QTreeWidgetItem(mUI.treeWidget, {"Nearest Bullseye", "none"});

   mUI.treeWidget->addTopLevelItem(mLatitudeItemPtr);
   mUI.treeWidget->addTopLevelItem(mLongitudeItemPtr);
   mUI.treeWidget->addTopLevelItem(mElevationItemPtr);
   mUI.treeWidget->addTopLevelItem(mBearingItemPtr);
   mUI.treeWidget->addTopLevelItem(mRangeItemPtr);
   mUI.treeWidget->addTopLevelItem(mBullseyeItemPtr);

   mLatitudeItemPtr->setData(0, wkf::UPDATER_ROLE, true);
   mLongitudeItemPtr->setData(0, wkf::UPDATER_ROLE, true);
   mElevationItemPtr->setData(0, wkf::UPDATER_ROLE, true);
   mBearingItemPtr->setData(0, wkf::UPDATER_ROLE, true);
   mRangeItemPtr->setData(0, wkf::UPDATER_ROLE, true);
   mBullseyeItemPtr->setData(0, wkf::UPDATER_ROLE, true);

   mUI.treeWidget->resizeColumnToContents(0);

   wkfEnv.GetMainWindow()->AddDockWidgetToViewMenu(this);

   connect(mUI.treeWidget, &wkf::DraggableTreeWidget::DragEvent, this, &CursorStatusDockWidget::DragEvent);
   mCallbacks.Add(vespa::VaObserver::UpdateCursorPosition.Connect(&CursorStatusDockWidget::UpdateCursorPositionCB,
                                                                  this,
                                                                  mViewer.GetUniqueId()));
   mDtedRegistrationId = wkfEnv.GetResourceManager().Register();
}

void Map::CursorStatusDockWidget::UpdateCursorPositionCB(vespa::VaViewer* aViewerPtr, int aMouseX, int aMouseY)
{
   mMouseX = aMouseX;
   mMouseY = aMouseY;
   Update();
}


void Map::CursorStatusDockWidget::Update()
{
   // Make sure we have a valid mouse location.
   if ((mMouseX >= 0) && (mMouseY >= 0))
   {
      QString latitudeText  = "";
      QString longitudeText = "";
      QString elevationText = "no data";
      QString rangeText     = "no bullseye";
      QString bearingText   = "no bullseye";
      QString bullseyeText  = "no bullseye";

      // Format the status line
      UtAngle::SetDefaultFormat(UtAngle::FmtDMS + 3);

      vespa::VaPosition curPos;
      mViewer.ScreenToWorld(mMouseX, mMouseY, curPos);

      curPos.SetLatFormat(wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLatLonFormatting());
      curPos.SetLonFormat(wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLatLonFormatting());

      std::ostringstream latOss;
      latOss << curPos.GetLat();
      latitudeText = QString::fromStdString(latOss.str());

      std::ostringstream lonOss;
      lonOss << curPos.GetLon();
      longitudeText = QString::fromStdString(lonOss.str());

      float alt;
      if (wkfEnv.GetResourceManager().QueryElevation(mDtedRegistrationId, curPos.GetLat(), curPos.GetLon(), alt))
      {
         elevationText = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetFormattedValue(wkf::ValueType::cALTITUDE, alt);
      }

      double      range   = 0;
      double      bearing = 0;
      std::string bullseyeName;
      if (vaEnv.GetStandardScenario() &&
          vaEnv.GetStandardScenario()->GetRangeBearingFromNearestBullseye(curPos, range, bearing, bullseyeName))
      {
         rangeText = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetFormattedValue(wkf::ValueType::cLENGTH, range);
         // bearing needs to be converted from degrees to radians since GetFormattedValue() expects defaults units and
         // radians is the default
         bearingText  = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetFormattedValue(wkf::ValueType::cANGLE,
                                                                                         bearing * UtMath::cRAD_PER_DEG);
         bullseyeText = QString::fromStdString(bullseyeName);
      }

      mLatitudeItemPtr->setText(1, latitudeText);
      mLongitudeItemPtr->setText(1, longitudeText);
      mElevationItemPtr->setText(1, elevationText);

      if (bullseyeText == "no bullseye")
      {
         mRangeItemPtr->setHidden(true);
         mBearingItemPtr->setHidden(true);
         mBullseyeItemPtr->setHidden(true);
      }
      else
      {
         mRangeItemPtr->setHidden(false);
         mBearingItemPtr->setHidden(false);
         mBullseyeItemPtr->setHidden(false);
         mRangeItemPtr->setText(1, rangeText);
         mBearingItemPtr->setText(1, bearingText);
         mBullseyeItemPtr->setText(1, bullseyeText);
      }
   }
}

void Map::CursorStatusDockWidget::DragEvent(QTreeWidgetItem* aItem)
{
   QMimeData*  mimeData = new QMimeData;
   QByteArray  bytes;
   QDataStream out(&bytes, QIODevice::WriteOnly);

   out << static_cast<unsigned int>(mPluginUniqueId);
   out << QString();
   out << aItem->text(0);
   out << aItem->data(0, wkf::UPDATER_CONTEXT).toUInt();
   mimeData->setData(wkf::Updater::cMIME_IDENTIFIER, bytes); // need to give an address to the updater here
   QDrag* drag = new QDrag(this);
   drag->setMimeData(mimeData);
   drag->exec(Qt::DropAction::LinkAction);
   // capture the updater and pass it to whoever receives the drop
}
