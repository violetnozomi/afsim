// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WkfInteractionPrefWidget.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>

#include "UtQtColorButton.hpp"
#include "WkfInteractionPrefObject.hpp"

wkf::InteractionPrefWidget::InteractionPrefWidget()
   : PrefWidgetT<InteractionPrefObject>()
{
   mUi.setupUi(this);

   QTreeWidgetItem* lw = new QTreeWidgetItem(mUi.treeWidget->invisibleRootItem(), QStringList("Line Width"));
   mLineWidthWidget    = new QSpinBox(this);
   mLineWidthWidget->setRange(1, 8);
   mLineWidthWidget->setValue(3);
   mUi.treeWidget->setItemWidget(lw, 1, mLineWidthWidget);
   QTreeWidgetItem* standard = new QTreeWidgetItem(mUi.treeWidget->invisibleRootItem(), QStringList("Standard"));
   mItemMap[InteractionPrefData::cDETECT]     = new QTreeWidgetItem(standard, QStringList("Detect"));
   mItemMap[InteractionPrefData::cTRACK]      = new QTreeWidgetItem(standard, QStringList("Sensor Track"));
   mItemMap[InteractionPrefData::cLOCALTRACK] = new QTreeWidgetItem(standard, QStringList("Local Track"));
   mItemMap[InteractionPrefData::cFIRE]       = new QTreeWidgetItem(standard, QStringList("Fire"));
   mItemMap[InteractionPrefData::cKILL]       = new QTreeWidgetItem(standard, QStringList("Kill*"));
   mItemMap[InteractionPrefData::cJAM]        = new QTreeWidgetItem(standard, QStringList("Jam Request"));
   mItemMap[InteractionPrefData::cMESSAGE]    = new QTreeWidgetItem(standard, QStringList("Comm*"));
   mItemMap[InteractionPrefData::cTASK]       = new QTreeWidgetItem(standard, QStringList("Task"));
   standard->setExpanded(true);
   QTreeWidgetItem* tt = new QTreeWidgetItem(mUi.treeWidget->invisibleRootItem(), QStringList("Tooltips"));
   mTooltipWidget      = new QComboBox(this);
   mTooltipWidget->addItem("Off", InteractionPrefData::cTT_OFF);
   mTooltipWidget->addItem("Simple", InteractionPrefData::cTT_SIMPLE);
   mTooltipWidget->addItem("Detailed", InteractionPrefData::cTT_DETAILED);
   mUi.treeWidget->setItemWidget(tt, 1, mTooltipWidget);
   QTreeWidgetItem* to = new QTreeWidgetItem(mUi.treeWidget->invisibleRootItem(), QStringList("*Timeout (sec)"));
   mTimeoutWidget      = new QSpinBox(this);
   mTimeoutWidget->setRange(0, 600);
   mTimeoutWidget->setValue(30);
   mUi.treeWidget->setItemWidget(to, 1, mTimeoutWidget);
   QTreeWidgetItem* sl    = new QTreeWidgetItem(mUi.treeWidget->invisibleRootItem(), QStringList("*Permit Stack"));
   mStackingAllowedWidget = new QCheckBox(this);
   mUi.treeWidget->setItemWidget(sl, 1, mStackingAllowedWidget);

   connect(mPrefObjectPtr.get(),
           &InteractionPrefObject::NewInteractionTypeRegistered,
           [this](const QString& aName, const QString& aGroup, const QColor& aColor)
           {
              QTreeWidgetItem* parent = mUi.treeWidget->invisibleRootItem();
              if (!aGroup.isEmpty())
              {
                 QTreeWidgetItem* newparent = nullptr;
                 QTreeWidgetItem* preceding = nullptr;
                 for (int i = 0; i < parent->childCount(); ++i)
                 {
                    QTreeWidgetItem* child = parent->child(i);
                    if (child->text(0) == aGroup)
                    {
                       newparent = child;
                    }
                    else if (child->text(0) == "Standard")
                    {
                       preceding = child;
                    }
                 }
                 if (newparent == nullptr)
                 {
                    if (preceding)
                    {
                       newparent = new QTreeWidgetItem(mUi.treeWidget->invisibleRootItem(), preceding);
                       newparent->setText(0, aGroup);
                    }
                    else
                    {
                       newparent = new QTreeWidgetItem(mUi.treeWidget->invisibleRootItem(), QStringList(aGroup));
                    }
                 }
                 parent = newparent;
              }
              mItemMap[aName] = new QTreeWidgetItem(parent, QStringList(aName));
              SetColor(mItemMap[aName], aColor);
           });
}

void wkf::InteractionPrefWidget::WritePreferenceData(InteractionPrefData& aPrefData)
{
   for (const auto& item : mItemMap)
   {
      aPrefData.mColors[item.first] = GetColor(item.second);
   }
   aPrefData.mTimeout         = (double)(mTimeoutWidget->value());
   aPrefData.mStackingAllowed = mStackingAllowedWidget->isChecked();
   aPrefData.mTooltipMode     = mTooltipWidget->currentData().toInt();
   aPrefData.mLineWidth       = (unsigned int)(mLineWidthWidget->value());
}

void wkf::InteractionPrefWidget::ReadPreferenceData(const InteractionPrefData& aPrefData)
{
   for (auto& item : mItemMap)
   {
      SetColor(item.second, aPrefData.mColors[item.first]);
   }
   mTimeoutWidget->setValue(aPrefData.mTimeout);
   mStackingAllowedWidget->setChecked(aPrefData.mStackingAllowed);
   int index = mTooltipWidget->findData(aPrefData.mTooltipMode);
   if (index != -1)
   {
      mTooltipWidget->setCurrentIndex(index);
   }
   mLineWidthWidget->setValue(aPrefData.mLineWidth);
}

QColor wkf::InteractionPrefWidget::GetColor(QTreeWidgetItem* aItem) const
{
   UtQtColorButton* colorButton = dynamic_cast<UtQtColorButton*>(mUi.treeWidget->itemWidget(aItem, 1));
   if (nullptr != colorButton)
   {
      return colorButton->GetColor();
   }
   else
   {
      return Qt::red;
   }
}

void wkf::InteractionPrefWidget::SetColor(QTreeWidgetItem* aItem, const QColor& aColor)
{
   UtQtColorButton* colorButton = dynamic_cast<UtQtColorButton*>(mUi.treeWidget->itemWidget(aItem, 1));
   if (nullptr == colorButton)
   {
      colorButton = new UtQtColorButton(this);
      mUi.treeWidget->setItemWidget(aItem, 1, colorButton);
   }
   if (nullptr != colorButton)
   {
      colorButton->SetColor(aColor);
   }
}
