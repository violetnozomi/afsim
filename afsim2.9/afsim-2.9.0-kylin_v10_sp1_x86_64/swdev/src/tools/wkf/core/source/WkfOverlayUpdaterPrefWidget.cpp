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

#include "WkfOverlayUpdaterPrefWidget.hpp"

#include <QAction>
#include <QBoxLayout>
#include <QColorDialog>
#include <QFontDialog>
#include <QLabel>
#include <QListWidget>
#include <QMenu>
#include <QPushButton>

wkf::OverlayUpdaterPrefWidget::OverlayUpdaterPrefWidget(QWidget* aParent /*= nullptr*/)
   : wkf::PrefWidgetT<wkf::OverlayUpdaterPrefObject>(aParent)
{
   setWindowTitle("Data Displays");
   setLayout(new QVBoxLayout(this));
   layout()->addWidget(new QLabel("Data Types", this));
   mListWidget = new QListWidget(this);
   layout()->addWidget(mListWidget);
   QHBoxLayout* hbox = new QHBoxLayout();
   layout()->addItem(hbox);
   hbox->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
   mEditButton = new QToolButton(this);
   mEditButton->setIcon(QIcon::fromTheme("wrench"));
   mMenu = new QMenu(mEditButton);
   connect(mMenu->addAction("Set Color"), &QAction::triggered, this, &OverlayUpdaterPrefWidget::SetColor);
   connect(mMenu->addAction("Set Style"), &QAction::triggered, this, &OverlayUpdaterPrefWidget::SetStyle);
   mEditButton->setMenu(mMenu);
   mEditButton->setEnabled(false);
   mDeleteButton = new QPushButton(QIcon::fromTheme("delete"), "", this);
   mDeleteButton->setEnabled(false);
   connect(mListWidget, &QListWidget::itemSelectionChanged, this, &OverlayUpdaterPrefWidget::SelectionChanged);
   connect(mDeleteButton, &QPushButton::pressed, this, &OverlayUpdaterPrefWidget::DeleteItem);
   connect(mEditButton, &QToolButton::pressed, mEditButton, &QToolButton::showMenu);
   mListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(mListWidget, &QListWidget::customContextMenuRequested, this, &OverlayUpdaterPrefWidget::ListContextMenu);
   hbox->addWidget(mEditButton);
   hbox->addWidget(mDeleteButton);
}

void wkf::OverlayUpdaterPrefWidget::ReadPreferenceData(const OverlayUpdaterPrefData& aPrefData)
{
   mListWidget->clear();
   mColorSet.clear();
   mFontSet.clear();
   for (auto&& centry : aPrefData.mColorMap)
   {
      QListWidgetItem* item = new QListWidgetItem(centry.first, mListWidget);
      item->setTextColor(centry.second);
      mColorSet.insert(centry.first);
      auto fentry = aPrefData.mFontMap.find(centry.first);
      if (fentry != aPrefData.mFontMap.end())
      {
         item->setFont(fentry->second);
         mFontSet.insert(fentry->first);
      }
   }
   for (auto&& fentry : aPrefData.mFontMap)
   {
      if (mListWidget->findItems(fentry.first, Qt::MatchExactly).empty())
      {
         QListWidgetItem* item = new QListWidgetItem(fentry.first, mListWidget);
         item->setFont(fentry.second);
         mFontSet.insert(fentry.first);
      }
   }
}

void wkf::OverlayUpdaterPrefWidget::WritePreferenceData(OverlayUpdaterPrefData& aPrefData)
{
   aPrefData.mColorMap.clear();
   aPrefData.mFontMap.clear();
   int c = mListWidget->count();
   for (int i = 0; i < c; ++i)
   {
      QListWidgetItem* item = mListWidget->item(i);
      if (mColorSet.find(item->text()) != mColorSet.end())
      {
         aPrefData.mColorMap[item->text()] = item->textColor();
      }
      if (mFontSet.find(item->text()) != mFontSet.end())
      {
         aPrefData.mFontMap[item->text()] = item->font();
      }
   }
}

void wkf::OverlayUpdaterPrefWidget::SelectionChanged()
{
   if (mListWidget->currentItem())
   {
      mEditButton->setEnabled(true);
      mDeleteButton->setEnabled(true);
   }
   else
   {
      mEditButton->setEnabled(false);
      mDeleteButton->setEnabled(false);
   }
}

void wkf::OverlayUpdaterPrefWidget::DeleteItem()
{
   if (mListWidget->currentItem())
   {
      delete mListWidget->currentItem();
   }
}

void wkf::OverlayUpdaterPrefWidget::SetColor()
{
   if (mListWidget->currentItem())
   {
      QString       name         = mListWidget->currentItem()->text();
      const QColor& currentColor = mListWidget->currentItem()->textColor();
      QColor        newColor =
         QColorDialog::getColor(currentColor, this, QString("Choose Color for %1").arg(mListWidget->currentItem()->text()));
      if (newColor != currentColor)
      {
         mListWidget->currentItem()->setTextColor(newColor);
         mColorSet.insert(name);
      }
   }
}

void wkf::OverlayUpdaterPrefWidget::SetStyle()
{
   if (mListWidget->currentItem())
   {
      QString      name        = mListWidget->currentItem()->text();
      const QFont& currentFont = mListWidget->currentItem()->font();
      bool         ok          = false;
      QFont        newFont =
         QFontDialog::getFont(&ok, currentFont, this, QString("Choose Font for %1").arg(mListWidget->currentItem()->text()));
      if (ok)
      {
         mListWidget->currentItem()->setFont(newFont);
         mFontSet.insert(name);
      }
   }
}

void wkf::OverlayUpdaterPrefWidget::ListContextMenu(const QPoint& aPosition)
{
   mMenu->exec(mapToGlobal(aPosition));
}
