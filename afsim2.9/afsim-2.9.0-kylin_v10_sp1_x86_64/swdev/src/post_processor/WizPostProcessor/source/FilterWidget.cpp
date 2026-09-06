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

#include "FilterWidget.hpp"
#include "ui_FilterWidget.h"

PostProcessor::FilterWidget::FilterWidget(QWidget* aParent)
   : QWidget(aParent)
   , mUIPtr(new Ui::FilterWidget)
{
   mUIPtr->setupUi(this);
   mUIPtr->filterComboBox->addItem("Any");
   mUIPtr->treeWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
   mUIPtr->checkAll->setChecked(false);
   Connect();
}

PostProcessor::FilterWidget::~FilterWidget()
{
   delete mUIPtr;
}

void PostProcessor::FilterWidget::SetColumnHeaders(const QStringList& aHeaders)
{
   mUIPtr->treeWidget->setColumnCount(aHeaders.size());
   mUIPtr->treeWidget->setHeaderLabels(aHeaders);
   for (int i = 0; i < aHeaders.size(); ++i)
   {
      mUIPtr->treeWidget->resizeColumnToContents(i);
   }
   mUIPtr->filterComboBox->addItems(aHeaders);
}

void PostProcessor::FilterWidget::Clear()
{
   mUIPtr->treeWidget->clear();
}

void PostProcessor::FilterWidget::AddItem(const QStringList& aItems,
                                          QColor             aColor,
                                          bool               aChecked)
{
   auto* item = new QTreeWidgetItem();
   for (const auto& i : aItems)
   {
      int col = aItems.indexOf(i);
      item->setText(col, i);
      mUIPtr->treeWidget->resizeColumnToContents(col);
   }

   item->setCheckState(0, aChecked ? Qt::Checked : Qt::Unchecked);
   mUIPtr->treeWidget->addTopLevelItem(item);
   SetBackgroundColor(item, aColor);
   FilterChanged();
}

void PostProcessor::FilterWidget::RemoveItem(const QString& aText,
                                             int            aColumn)
{
   QList<QTreeWidgetItem*> items = mUIPtr->treeWidget->findItems(aText, Qt::MatchExactly, aColumn);

   for (auto i : items)
   {
      int index = mUIPtr->treeWidget->indexOfTopLevelItem(i);
      delete mUIPtr->treeWidget->takeTopLevelItem(index);
   }
}

bool PostProcessor::FilterWidget::RemoveItem(const QVector<QPair<QString, int> >& aItems)
{
   bool removeSuccessful = false;
   if (!aItems.empty())
   {
      QList<QTreeWidgetItem*> potentialMatches = mUIPtr->treeWidget->findItems(aItems[0].first, Qt::MatchExactly, aItems[0].second);

      for (auto p : potentialMatches)
      {
         bool allMatch = true;
         for (auto item : aItems) // these all must match
         {
            if (item.second < mUIPtr->treeWidget->columnCount()) // make sure the column exists in the tree
            {
               if (QString::compare(item.first, p->text(item.second)) != 0) // if they don't match
               {
                  allMatch = false;
               }
            }
         }
         if (allMatch)
         {
            int index = mUIPtr->treeWidget->indexOfTopLevelItem(p);
            delete mUIPtr->treeWidget->takeTopLevelItem(index);
            removeSuccessful = true;
         }
      }
   }
   return removeSuccessful;
}


QStringList PostProcessor::FilterWidget::GetCheckedItems(int aColumn)
{
   QStringList checkedItems;
   QTreeWidgetItemIterator iter(mUIPtr->treeWidget);
   while (QTreeWidgetItem* item = (*iter))
   {
      if (aColumn < item->columnCount())
      {
         if (item->checkState(0))
         {
            checkedItems.push_back(item->text(aColumn));
         }
      }
      ++iter;
   }
   return checkedItems;
}

bool PostProcessor::FilterWidget::AllChecked()
{
   bool allChecked = true;
   QTreeWidgetItemIterator iter(mUIPtr->treeWidget);
   while (QTreeWidgetItem* item = (*iter))
   {
      if (item->checkState(0) == Qt::Unchecked)
      {
         allChecked = false;
         break;
      }
      ++iter;
   }
   return allChecked;
}

bool PostProcessor::FilterWidget::AllUnchecked()
{
   bool allUnchecked = true;
   QTreeWidgetItemIterator iter(mUIPtr->treeWidget);
   while (QTreeWidgetItem* item = (*iter))
   {
      if (item->checkState(0) == Qt::Checked)
      {
         allUnchecked = false;
         break;
      }
      ++iter;
   }
   if (mUIPtr->treeWidget->topLevelItemCount() == 0)
   {
      allUnchecked = false;
   }
   return allUnchecked;
}

void PostProcessor::FilterWidget::SetChecked(const QString& aName,
                                             bool           aChecked)
{
   for (auto i : mUIPtr->treeWidget->findItems(aName, Qt::MatchExactly))
   {
      i->setCheckState(0, aChecked ? Qt::Checked : Qt::Unchecked);
   }
}

void PostProcessor::FilterWidget::Connect()
{
   connect(mUIPtr->treeWidget, &QTreeWidget::itemChanged, this, &FilterWidget::ItemChanged);
   connect(mUIPtr->filterComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &FilterWidget::FilterChanged);
   connect(mUIPtr->filterMatchCase, &QCheckBox::clicked, this, &FilterWidget::FilterChanged);
   connect(mUIPtr->filterWholeWord, &QCheckBox::clicked, this, &FilterWidget::FilterChanged);
   connect(mUIPtr->filterLineEdit, &QLineEdit::textChanged, this, &FilterWidget::FilterChanged);
   connect(mUIPtr->checkAll, &QCheckBox::clicked, this, &FilterWidget::CheckAll);
}

void PostProcessor::FilterWidget::FilterChanged()
{
   QTreeWidgetItemIterator iter(mUIPtr->treeWidget);

   while (QTreeWidgetItem* item = (*iter))
   {
      item->setHidden(!ItemPassesFilter(item));

      ++iter;
   }

   bool allChecked = true;
   iter = QTreeWidgetItemIterator(mUIPtr->treeWidget);
   while (QTreeWidgetItem* item = (*iter))
   {
      if (!item->isHidden() && !item->checkState(0))
      {
         allChecked = false;
         break;
      }
      ++iter;
   }

   mUIPtr->checkAll->setChecked(allChecked);

}

bool PostProcessor::FilterWidget::ItemPassesFilter(const QTreeWidgetItem* aItem)
{
   bool matchCase = mUIPtr->filterMatchCase->isChecked();
   bool matchWholeWord = mUIPtr->filterWholeWord->isChecked();

   bool visible = false;

   int column = mUIPtr->filterComboBox->currentIndex() - 1; // -1 because combobox has extra "Any" item,

   QString text = mUIPtr->filterLineEdit->text();

   if (text.isEmpty())
   {
      visible = true;
   }
   else
   {
      Qt::CaseSensitivity cs = Qt::CaseInsensitive;
      if (matchCase)
      {
         cs = Qt::CaseSensitive;
      }

      if (column == -1)
      {
         for (int i = 0; i < mUIPtr->treeWidget->columnCount(); ++i)
         {
            if (matchWholeWord)
            {
               if (QString::compare(aItem->text(i), text, cs) == 0)
               {
                  visible = true;
                  break;
               }
            }
            else
            {
               if (aItem->text(i).contains(text, cs))
               {
                  visible = true;
                  break;
               }
            }
         }
      }
      else
      {
         if (matchWholeWord)
         {
            if (QString::compare(aItem->text(column), text, cs) == 0)
            {
               visible = true;
            }
         }
         else
         {
            if (aItem->text(column).contains(text, cs))
            {
               visible = true;
            }
         }
      }
   }

   return visible;

}

void PostProcessor::FilterWidget::CheckAll(bool aChecked)
{
   QTreeWidgetItemIterator iter(mUIPtr->treeWidget);
   while (QTreeWidgetItem* item = (*iter))
   {
      if (!item->isHidden())
      {
         item->setCheckState(0, aChecked ? Qt::Checked : Qt::Unchecked);
      }

      ++iter;
   }
}

void PostProcessor::FilterWidget::SetBackgroundColor(QTreeWidgetItem* aItem,
                                                     QColor           aColor)
{
   for (int i = 0; i < aItem->columnCount(); ++i)
   {
      aItem->setBackground(i, QBrush(aColor));
   }
}

void PostProcessor::FilterWidget::ItemChanged(QTreeWidgetItem* aItem,
                                              int              aColumn)
{
   // if an item changed, and it was the 0 column, this means that the check state changed.
   // We need the filter to determine the check state of the "all checked" check box
   if (aColumn == 0)
   {
      FilterChanged();
   }
}
