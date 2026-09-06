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

#include "WkfHUD_PrefWidget.hpp"

#include <QComboBox>
#include <QHeaderView>
#include <QSpinBox>
#include <QTreeWidget>
#include <QVBoxLayout>

#include "WkfHUD_PrefObject.hpp"

wkf::HUD_PrefWidget::HUD_PrefWidget()
   : wkf::PrefWidgetT<HUD_PrefObject>()
{
   setWindowTitle("Head Up View");

   QVBoxLayout* lo = new QVBoxLayout(this);
   lo->setObjectName(QStringLiteral("verticalLayout"));
   QTreeWidget*     vectorTree        = new QTreeWidget(this);
   QTreeWidgetItem* __qtreewidgetitem = new QTreeWidgetItem();
   __qtreewidgetitem->setText(0, QStringLiteral("1"));
   vectorTree->setHeaderItem(__qtreewidgetitem);
   vectorTree->setObjectName(QStringLiteral("vectorTree"));
   vectorTree->header()->setVisible(false);
   vectorTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
   lo->addWidget(vectorTree);
   vectorTree->setColumnCount(2);

   QTreeWidgetItem* resItem = new QTreeWidgetItem(vectorTree->invisibleRootItem(), QStringList("Resolution"));
   mResolution              = new QComboBox(this);
   mResolution->addItem("960x540");
   mResolution->addItem("960x1080");
   mResolution->addItem("960x1200");
   mResolution->addItem("1920x1080");
   mResolution->addItem("1920x1200");
   mResolution->addItem("1920x2160");
   mResolution->addItem("3840x2160");
   vectorTree->setItemWidget(resItem, 1, mResolution);
}

void wkf::HUD_PrefWidget::WritePreferenceData(wkf::HUD_PrefData& aPrefData)
{
   QString      text = mResolution->currentText();
   unsigned int resX = 960;
   unsigned int resY = 540;
   if (text == "960x540")
   {
      resX = 960;
      resY = 540;
   }
   else if (text == "960x1080")
   {
      resX = 960;
      resY = 1080;
   }
   else if (text == "960x1200")
   {
      resX = 960;
      resY = 1200;
   }
   else if (text == "1920x1080")
   {
      resX = 1920;
      resY = 1080;
   }
   else if (text == "1920x1200")
   {
      resX = 1920;
      resY = 1200;
   }
   else if (text == "1920x2160")
   {
      resX = 1920;
      resY = 2160;
   }
   else if (text == "3840x2160")
   {
      resX = 3840;
      resY = 2160;
   }
   aPrefData.mResX = resX;
   aPrefData.mResY = resY;
}

void wkf::HUD_PrefWidget::ReadPreferenceData(const wkf::HUD_PrefData& aPrefData)
{
   if ((aPrefData.mResX == 960) && (aPrefData.mResY == 540))
   {
      mResolution->setCurrentText("960x540");
   }
   else if ((aPrefData.mResX == 960) && (aPrefData.mResY == 1080))
   {
      mResolution->setCurrentText("960x1080");
   }
   else if ((aPrefData.mResX == 960) && (aPrefData.mResY == 1200))
   {
      mResolution->setCurrentText("960x1200");
   }
   else if ((aPrefData.mResX == 1920) && (aPrefData.mResY == 1080))
   {
      mResolution->setCurrentText("1920x1080");
   }
   else if ((aPrefData.mResX == 1920) && (aPrefData.mResY == 1200))
   {
      mResolution->setCurrentText("1920x1200");
   }
   else if ((aPrefData.mResX == 1920) && (aPrefData.mResY == 2160))
   {
      mResolution->setCurrentText("1920x2160");
   }
   else if ((aPrefData.mResX == 3840) && (aPrefData.mResY == 2160))
   {
      mResolution->setCurrentText("3840x2160");
   }
   else
   {
      // Default to a 1080 display, the most common display resolution,
      // but this should never happen under normal operation
      mResolution->setCurrentText("1920x1080");
   }
}
