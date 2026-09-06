// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "RvBAT_Blackboard.hpp"

#include <QBoxLayout>

#include "RvEventPipeClasses.hpp"
#include "WkfTrack.hpp"
#include "WkfUnitTypes.hpp"

RvBAT::Blackboard::Blackboard(QWidget* aParentPtr, size_t aWidth)
   : QWidget(aParentPtr)
   , mTitle(this)
   , mTree(this)
   , mSelectedNode(nullptr)
{
   setFixedWidth(static_cast<int>(aWidth));
   // Temp font
   QFont f;
   f.setPointSize(12);

   // Set initial title values
   mTitle.setAlignment(Qt::AlignCenter);
   mTitle.setFont(f);
   mTitle.setText("Blackboard: Shared");

   // Set initial tree values
   mTree.setColumnCount(2);
   QList<QString> headers = {QString("Name"), QString("Value")};
   mTree.setHeaderLabels(QStringList(headers));

   auto layout = new QVBoxLayout;
   layout->setContentsMargins(QMargins(0, 0, 0, 0));
   layout->setSpacing(0);
   layout->addWidget(&mTitle);
   layout->addWidget(&mTree);
   setLayout(layout);
}

void RvBAT::Blackboard::SetBlackboardData(const QVector<rv::BehaviorTreeBlackboardData>& aMsgList)
{
   // Clear the tree
   mTree.clear();

   // For each blackboard data message, loop through and add it to the tree
   for (int i = 0; i < aMsgList.size(); i++)
   {
      QTreeWidgetItem* wBlackboard = nullptr;

      // Only create the shared blackboard widget if there is a shared blackboard name
      if (!aMsgList[i].sharedBlackboard().empty())
      {
         wBlackboard = new QTreeWidgetItem(&mTree);
         wBlackboard->setExpanded(true);
         wBlackboard->setText(0, QString::fromStdString(aMsgList[i].sharedBlackboard()));
         mTree.addTopLevelItem(wBlackboard);
      }

      // Go through each of the dicts and populate the vars
      for (const auto& itr : aMsgList[i].boolDict())
      {
         auto tmp = (wBlackboard != nullptr) ? new QTreeWidgetItem(wBlackboard) : new QTreeWidgetItem(&mTree);
         tmp->setText(0, QString::fromStdString(itr.key()));
         tmp->setText(1, (itr.value() ? "true" : "false"));
      }
      for (const auto& itr : aMsgList[i].intDict())
      {
         auto tmp = (wBlackboard != nullptr) ? new QTreeWidgetItem(wBlackboard) : new QTreeWidgetItem(&mTree);
         tmp->setText(0, QString::fromStdString(itr.key()));
         tmp->setText(1, QString::number(itr.value()));
      }
      for (const auto& itr : aMsgList[i].stringDict())
      {
         auto tmp = (wBlackboard != nullptr) ? new QTreeWidgetItem(wBlackboard) : new QTreeWidgetItem(&mTree);
         tmp->setText(0, QString::fromStdString(itr.key()));
         tmp->setText(1, QString::fromStdString(itr.value()));
      }
      for (const auto& itr : aMsgList[i].doubleDict())
      {
         auto tmp = (wBlackboard != nullptr) ? new QTreeWidgetItem(wBlackboard) : new QTreeWidgetItem(&mTree);
         tmp->setText(0, QString::fromStdString(itr.key()));
         tmp->setText(1, QString::number(itr.value()));
      }
      for (const auto& itr : aMsgList[i].trackDict())
      {
         auto track   = itr.value();
         auto trackId = track.trackId().localTrackNumber();

         auto wParentItem = (wBlackboard != nullptr) ? new QTreeWidgetItem(wBlackboard) : new QTreeWidgetItem(&mTree);
         wParentItem->setText(0, QString::fromStdString(itr.key()));
         wParentItem->setExpanded(true);

         auto wTrackId       = new wkf::UnitlessTreeWidgetItem(wParentItem, "Track Id");
         auto wPositionValid = new wkf::PlatformDataTreeItem(wParentItem, {"Position Valid"});
         auto wLatitude      = new wkf::LatitudeTreeWidgetItem(wParentItem, "Latitude");
         auto wLongitude     = new wkf::LongitudeTreeWidgetItem(wParentItem, "Longitude");
         auto wAltitude      = new wkf::AltitudeTreeWidgetItem(wParentItem, "Altitude");
         auto wVelocityValid = new wkf::PlatformDataTreeItem(wParentItem, {"Velocity Valid"});
         auto wSpeed         = new wkf::SpeedTreeWidgetItem(wParentItem, "Speed");
         auto wHeading       = new wkf::AngleTreeWidgetItem(wParentItem, "Heading");
         auto wSide          = new wkf::PlatformDataTreeItem(wParentItem, {"Side"});
         auto wType          = new wkf::PlatformDataTreeItem(wParentItem, {"Type"});
         auto wSpatialDomain = new wkf::PlatformDataTreeItem(wParentItem, {"Spatial Domain"});
         auto wQuality       = new wkf::UnitlessTreeWidgetItem(wParentItem, "Quality");

         UtEntity         entity;
         const rv::Vec3d& location      = track.locationWCS();
         const rv::Vec3f& velocity      = track.velocityWCS();
         const bool       locationValid = track.locationWCSValid();
         const bool       velocityValid = track.velocityWCSValid();

         const double lwcs[3] = {location.x(), location.y(), location.z()};
         const double vwcs[3] = {velocity.x(), velocity.y(), velocity.z()};
         entity.SetLocationWCS(lwcs);
         entity.SetVelocityWCS(vwcs);
         double vel[3] = {0, 0, 0};
         entity.GetVelocityNED(vel);

         double heading_rad = atan2(vel[1], vel[0]);
         // For display purposes put the valid heading range in [0, 2*PI]
         if (heading_rad < 0)
         {
            heading_rad += UtMath::cTWO_PI;
         }

         double lat, lon, alt_m;
         entity.GetLocationLLA(lat, lon, alt_m);
         const double  speed_mps = entity.GetSpeed();
         const QString domain =
            QString::fromStdString(wkf::SpatialDomainToString(static_cast<wkf::SpatialDomain>(track.spatialDomain())));

         wTrackId->SetValue(trackId);
         wLatitude->SetValue(lat);
         wLongitude->SetValue(lon);
         wAltitude->SetValue(alt_m);
         wPositionValid->setText(1, locationValid ? "true" : "false");
         wVelocityValid->setText(1, velocityValid ? "true" : "false");
         wSpeed->SetValue(speed_mps);
         wHeading->SetValue(heading_rad);
         wSide->setText(1, QString::fromStdString(track.side()));
         wType->setText(1, QString::fromStdString(track.type()));
         wSpatialDomain->setText(1, domain);
         wQuality->SetValue(track.trackQuality());

         wTrackId->SetUnhideable();
         wPositionValid->SetUnhideable();
         wLatitude->SetUnhideable();
         wLongitude->SetUnhideable();
         wAltitude->SetUnhideable();
         wVelocityValid->SetUnhideable();
         wSpeed->SetUnhideable();
         wHeading->SetUnhideable();
         wSide->SetUnhideable();
         wType->SetUnhideable();
         wSpatialDomain->SetUnhideable();
         wQuality->SetUnhideable();
      }
      for (const auto& itr : aMsgList[i].platformDict())
      {
         auto platform = itr.value();

         auto wParentItem = (wBlackboard != nullptr) ? new QTreeWidgetItem(wBlackboard) : new QTreeWidgetItem(&mTree);
         wParentItem->setText(0, QString::fromStdString(itr.key()));
         wParentItem->setExpanded(true);

         auto wName = new wkf::PlatformDataTreeItem(wParentItem, {"Name"});
         auto wSide = new wkf::PlatformDataTreeItem(wParentItem, {"Side"});
         auto wType = new wkf::PlatformDataTreeItem(wParentItem, {"Types"});
         wType->setExpanded(true);

         if (platform.nameValid())
         {
            wName->setText(1, QString::fromStdString(platform.name()));
         }

         if (platform.sideValid())
         {
            wSide->setText(1, QString::fromStdString(platform.side()));
         }

         if (platform.typesValid())
         {
            for (auto& type : platform.types())
            {
               auto tmp = new QTreeWidgetItem(wType);
               tmp->setText(1, QString::fromStdString(type));
            }
         }

         wName->SetUnhideable();
         wSide->SetUnhideable();
         wType->SetUnhideable();
      }
   }

   // Resize the columns to fit the contents
   for (int i = 0; i < mTree.columnCount(); i++)
   {
      mTree.resizeColumnToContents(i);
   }
}

void RvBAT::Blackboard::ClearBlackboardData()
{
   mTree.clear();
}

void RvBAT::Blackboard::SetBlackboardTitle(const QString& aText)
{
   mTitle.setText("Blackboard: " + aText);
}
