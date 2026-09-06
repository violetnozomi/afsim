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
#include "WkfComboBoxTreeWidgetItem.hpp"

#include <QApplication>
#include <QLineEdit>
#include <QListView>
#include <QStandardItem>
#include <QTreeView>

void wkf::ComboBoxTreeWidgetItem::UnanimatedComboBox::showPopup()
{
   bool previous = qApp->isEffectEnabled(Qt::UI_AnimateCombo);
   qApp->setEffectEnabled(Qt::UI_AnimateCombo, false);
   QComboBox::showPopup();
   qApp->setEffectEnabled(Qt::UI_AnimateCombo, previous);
}

void wkf::ComboBoxTreeWidgetItem::BuildWidget(QTreeWidget* aTreePtr)
{
   if (!mBoxPtr.isNull())
   {
      aTreePtr->removeItemWidget(this, mBoxColumn);
      mBoxPtr->deleteLater();
      mBoxPtr.clear();
   }

   mBoxPtr = new wkf::ComboBoxTreeWidgetItem::UnanimatedComboBox();
   aTreePtr->setItemWidget(this, mBoxColumn, mBoxPtr);
   mBoxPtr->setModel(mModelPtr);

   mBoxPtr->setEditable(true);

   if (mCategorized)
   {
      mBoxPtr->setInsertPolicy(QComboBox::NoInsert);
      auto view = new QTreeView();
      view->setHeaderHidden(true);
      mBoxPtr->setView(view);
   }
   else
   {
      mBoxPtr->setInsertPolicy(QComboBox::InsertAlphabetically);
      mBoxPtr->setView(new QListView());
   }

   setText(1, mText.toLower());
}

void wkf::ComboBoxTreeWidgetItem::setText(int aColumn, const QString& aText)
{
   if (aColumn == mBoxColumn)
   {
      mText = aText;
      if (!mBoxPtr.isNull())
      {
         mBoxPtr->setCurrentText(mText);
      }
   }
   else
   {
      PlatformDataTreeItem::setText(aColumn, aText);
   }
}
