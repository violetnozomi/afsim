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
#include "WkfHDD_PrefWidget.hpp"

#include <QComboBox>
#include <QHeaderView>
#include <QStringListModel>
#include <QTreeWidget>
#include <QVBoxLayout>

#include "WkfHDD_PrefObject.hpp"

HDD::PrefWidget::PrefWidget()
   : wkf::PrefWidgetT<HDD::PrefObject>()
{
   setWindowTitle("Head Down View");

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

   // Resolution choices.
   QTreeWidgetItem* resItem = new QTreeWidgetItem(vectorTree->invisibleRootItem(), QStringList("Resolution"));
   mResolution              = new QComboBox(this);
   mResolution->addItem("960x540");
   mResolution->addItem("960x600");
   mResolution->addItem("1024x512");
   mResolution->addItem("1280x720");
   mResolution->addItem("1920x1080");
   mResolution->addItem("1920x1200");

   // Create our page tree widgets.
   QTreeWidgetItem* page1Item  = new QTreeWidgetItem(vectorTree->invisibleRootItem(), QStringList("Page 1"));
   QTreeWidgetItem* page2Item  = new QTreeWidgetItem(vectorTree->invisibleRootItem(), QStringList("Page 2"));
   QTreeWidgetItem* page3Item  = new QTreeWidgetItem(vectorTree->invisibleRootItem(), QStringList("Page 3"));
   QTreeWidgetItem* page4Item  = new QTreeWidgetItem(vectorTree->invisibleRootItem(), QStringList("Page 4"));
   QTreeWidgetItem* page5Item  = new QTreeWidgetItem(vectorTree->invisibleRootItem(), QStringList("Page 5"));
   QTreeWidgetItem* page6Item  = new QTreeWidgetItem(vectorTree->invisibleRootItem(), QStringList("Page 6"));
   QTreeWidgetItem* page7Item  = new QTreeWidgetItem(vectorTree->invisibleRootItem(), QStringList("Page 7"));
   QTreeWidgetItem* page8Item  = new QTreeWidgetItem(vectorTree->invisibleRootItem(), QStringList("Page 8"));
   QTreeWidgetItem* page9Item  = new QTreeWidgetItem(vectorTree->invisibleRootItem(), QStringList("Page 9"));
   QTreeWidgetItem* page10Item = new QTreeWidgetItem(vectorTree->invisibleRootItem(), QStringList("Page 10"));
   QTreeWidgetItem* page11Item = new QTreeWidgetItem(vectorTree->invisibleRootItem(), QStringList("Page 11"));
   QTreeWidgetItem* page12Item = new QTreeWidgetItem(vectorTree->invisibleRootItem(), QStringList("Page 12"));

   // Page choices.
   mPages[1]                     = new QComboBox(this);
   QStringListModel* stringModel = new QStringListModel();
   mPages[1]->setModel(stringModel);
   mPages[1]->addItem("ASR");
   mPages[1]->addItem("CKLST");
   mPages[1]->addItem("CNI");
   mPages[1]->addItem("DAS");
   mPages[1]->addItem("DIM");
   mPages[1]->addItem("EFI");
   mPages[1]->addItem("ENG");
   mPages[1]->addItem("FCS");
   mPages[1]->addItem("FTA");
   mPages[1]->addItem("FTI");
   mPages[1]->addItem("FUEL");
   mPages[1]->addItem("HUD");
   mPages[1]->addItem("ICAWS");
   mPages[1]->addItem("NAV"); // Will be removed later
   mPages[1]->addItem("SMS");
   mPages[1]->addItem("SACH");
   mPages[1]->addItem("TFLIR");
   mPages[1]->addItem("TSD-1");
   mPages[1]->addItem("TSD-2");
   mPages[1]->addItem("TSD-3");
   mPages[1]->addItem("TWD");
   mPages[1]->addItem("WPN-A");
   mPages[1]->addItem("WPN-S");

   // Set these page options for all twelve combo boxes.
   for (size_t i = 2; i <= 12; i++)
   {
      mPages[i] = new QComboBox(this);
      mPages[i]->addItems(stringModel->stringList());
   }

   // Add the resolution combo box.
   vectorTree->setItemWidget(resItem, 1, mResolution);

   // Add the page combo boxes.
   vectorTree->setItemWidget(page1Item, 1, mPages[1]);
   vectorTree->setItemWidget(page2Item, 1, mPages[2]);
   vectorTree->setItemWidget(page3Item, 1, mPages[3]);
   vectorTree->setItemWidget(page4Item, 1, mPages[4]);
   vectorTree->setItemWidget(page5Item, 1, mPages[5]);
   vectorTree->setItemWidget(page6Item, 1, mPages[6]);
   vectorTree->setItemWidget(page7Item, 1, mPages[7]);
   vectorTree->setItemWidget(page8Item, 1, mPages[8]);
   vectorTree->setItemWidget(page9Item, 1, mPages[9]);
   vectorTree->setItemWidget(page10Item, 1, mPages[10]);
   vectorTree->setItemWidget(page11Item, 1, mPages[11]);
   vectorTree->setItemWidget(page12Item, 1, mPages[12]);
}

void HDD::PrefWidget::WritePreferenceData(PrefData& aPrefData)
{
   // Resolution
   QString      text = mResolution->currentText();
   unsigned int resX = 960;
   unsigned int resY = 540;
   if (text == "960x600")
   {
      resX = 960;
      resY = 600;
   }
   else if (text == "1024x512")
   {
      resX = 1024;
      resY = 512;
   }
   else if (text == "1280x720")
   {
      resX = 1280;
      resY = 720;
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
   aPrefData.mResX = resX;
   aPrefData.mResY = resY;

   // Pages
   aPrefData.mPage1  = mPages[1]->currentText();
   aPrefData.mPage2  = mPages[2]->currentText();
   aPrefData.mPage3  = mPages[3]->currentText();
   aPrefData.mPage4  = mPages[4]->currentText();
   aPrefData.mPage5  = mPages[5]->currentText();
   aPrefData.mPage6  = mPages[6]->currentText();
   aPrefData.mPage7  = mPages[7]->currentText();
   aPrefData.mPage8  = mPages[8]->currentText();
   aPrefData.mPage9  = mPages[9]->currentText();
   aPrefData.mPage10 = mPages[10]->currentText();
   aPrefData.mPage11 = mPages[11]->currentText();
   aPrefData.mPage12 = mPages[12]->currentText();
}

void HDD::PrefWidget::ReadPreferenceData(const PrefData& aPrefData)
{
   // Resolution
   if ((aPrefData.mResX == 960) && (aPrefData.mResY == 540))
   {
      mResolution->setCurrentText("960x540");
   }
   else if ((aPrefData.mResX == 960) && (aPrefData.mResY == 600))
   {
      mResolution->setCurrentText("960x600");
   }
   else if ((aPrefData.mResX == 1024) && (aPrefData.mResY == 512))
   {
      mResolution->setCurrentText("1024x512");
   }
   else if ((aPrefData.mResX == 1280) && (aPrefData.mResY == 720))
   {
      mResolution->setCurrentText("1280x720");
   }
   else if ((aPrefData.mResX == 1920) && (aPrefData.mResY == 1080))
   {
      mResolution->setCurrentText("1920x1080");
   }
   else if ((aPrefData.mResX == 1920) && (aPrefData.mResY == 1200))
   {
      mResolution->setCurrentText("1920x1200");
   }

   // Pages
   mPages[1]->setCurrentText(aPrefData.mPage1);
   mPages[2]->setCurrentText(aPrefData.mPage2);
   mPages[3]->setCurrentText(aPrefData.mPage3);
   mPages[4]->setCurrentText(aPrefData.mPage4);
   mPages[5]->setCurrentText(aPrefData.mPage5);
   mPages[6]->setCurrentText(aPrefData.mPage6);
   mPages[7]->setCurrentText(aPrefData.mPage7);
   mPages[8]->setCurrentText(aPrefData.mPage8);
   mPages[9]->setCurrentText(aPrefData.mPage9);
   mPages[10]->setCurrentText(aPrefData.mPage10);
   mPages[11]->setCurrentText(aPrefData.mPage11);
   mPages[12]->setCurrentText(aPrefData.mPage12);
}
