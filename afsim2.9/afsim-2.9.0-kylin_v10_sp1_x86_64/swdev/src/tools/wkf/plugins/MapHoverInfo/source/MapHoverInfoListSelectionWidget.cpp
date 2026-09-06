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

#include "MapHoverInfoListSelectionWidget.hpp"

MapHoverInfo::ListSelectionWidget::ListSelectionWidget(QWidget* aParent, const QString& aTitle)
   : QGroupBox(aParent)
{
   mUi.setupUi(this);

   setTitle(aTitle);

   // Set button icons and connect to signals.
   mUi.addButton->setIcon(QIcon::fromTheme("left"));
   mUi.removeButton->setIcon(QIcon::fromTheme("right"));
   mUi.upButton->setIcon(QIcon::fromTheme("up"));
   mUi.downButton->setIcon(QIcon::fromTheme("down"));

   connect(mUi.addButton, &QPushButton::clicked, this, &ListSelectionWidget::AddClicked);
   connect(mUi.removeButton, &QPushButton::clicked, this, &ListSelectionWidget::RemoveClicked);
   connect(mUi.upButton, &QPushButton::clicked, this, &ListSelectionWidget::UpClicked);
   connect(mUi.downButton, &QPushButton::clicked, this, &ListSelectionWidget::DownClicked);
}

QStringList MapHoverInfo::ListSelectionWidget::GetAvailableOptions() const
{
   return GetOptions(*mUi.availableListWidget);
}

QStringList MapHoverInfo::ListSelectionWidget::GetSelectedOptions() const
{
   return GetOptions(*mUi.selectedListWidget);
}

void MapHoverInfo::ListSelectionWidget::Clear()
{
   mUi.availableListWidget->clear();
   mUi.selectedListWidget->clear();
}

void MapHoverInfo::ListSelectionWidget::ClearSelected()
{
   mUi.selectedListWidget->clear();
}

void MapHoverInfo::ListSelectionWidget::SetAvailableOptions(const QStringList& aOptions)
{
   mUi.availableListWidget->clear();
   mUi.availableListWidget->addItems(aOptions);
   mUi.availableListWidget->sortItems();

   // Remove options from Selected that no longer exist
   for (int i = 0; i < mUi.selectedListWidget->count();)
   {
      if (!aOptions.contains(mUi.selectedListWidget->item(i)->text()))
      {
         delete mUi.selectedListWidget->takeItem(i);
      }
      else
      {
         i++;
      }
   }
}

void MapHoverInfo::ListSelectionWidget::SetSelectedOptions(const QStringList& aOptions)
{
   mUi.selectedListWidget->clear();

   SelectOptions(aOptions);
}

void MapHoverInfo::ListSelectionWidget::AddOption(const QString& aOption, bool aAddToSelected)
{
   if (mUi.availableListWidget->findItems(aOption, Qt::MatchExactly).isEmpty())
   {
      mUi.availableListWidget->addItem(aOption);
      mUi.availableListWidget->sortItems();
   }

   if (aAddToSelected)
   {
      mUi.selectedListWidget->addItem(aOption);
   }
}

void MapHoverInfo::ListSelectionWidget::AddOptions(const QStringList& aOptions, bool aAddToSelected)
{
   for (const QString& option : aOptions)
   {
      AddOption(option, aAddToSelected);
   }
}

void MapHoverInfo::ListSelectionWidget::SelectOption(const QString& aOption)
{
   if (GetAvailableOptions().contains(aOption))
   {
      mUi.selectedListWidget->addItem(aOption);
   }
}

void MapHoverInfo::ListSelectionWidget::SelectOptions(const QStringList& aOptions)
{
   const QStringList available = GetAvailableOptions();
   for (const QString& option : aOptions)
   {
      if (available.contains(option))
      {
         mUi.selectedListWidget->addItem(option);
      }
   }
}

void MapHoverInfo::ListSelectionWidget::DeselectOption(const QString& aOption)
{
   RemoveOptionFrom(aOption, *mUi.selectedListWidget);
}

void MapHoverInfo::ListSelectionWidget::DeselectOptions(const QStringList& aOptions)
{
   RemoveOptionsFrom(aOptions, *mUi.selectedListWidget);
}

void MapHoverInfo::ListSelectionWidget::RemoveOption(const QString& aOption)
{
   RemoveOptionFrom(aOption, *mUi.availableListWidget);
   RemoveOptionFrom(aOption, *mUi.selectedListWidget);
}

void MapHoverInfo::ListSelectionWidget::RemoveOptions(const QStringList& aOptions)
{
   RemoveOptionsFrom(aOptions, *mUi.availableListWidget);
   RemoveOptionsFrom(aOptions, *mUi.selectedListWidget);
}

// Button Clicked Slots

void MapHoverInfo::ListSelectionWidget::AddClicked()
{
   auto* item = mUi.availableListWidget->currentItem();
   if (item != nullptr)
   {
      mUi.selectedListWidget->addItem(item->text());
   }
}

void MapHoverInfo::ListSelectionWidget::RemoveClicked()
{
   auto* item = mUi.selectedListWidget->currentItem();
   if (item != nullptr)
   {
      delete mUi.selectedListWidget->takeItem(mUi.selectedListWidget->row(item));
   }
}

void MapHoverInfo::ListSelectionWidget::UpClicked()
{
   auto* item = mUi.selectedListWidget->currentItem();
   if (item != nullptr)
   {
      const int row = mUi.selectedListWidget->row(item);
      if (row > 0)
      {
         item = mUi.selectedListWidget->takeItem(row);
         mUi.selectedListWidget->insertItem(row - 1, item);
         mUi.selectedListWidget->setCurrentItem(item);
      }
   }
}

void MapHoverInfo::ListSelectionWidget::DownClicked()
{
   auto* item = mUi.selectedListWidget->currentItem();
   if (item != nullptr)
   {
      const int row = mUi.selectedListWidget->row(item);
      item          = mUi.selectedListWidget->takeItem(row);
      mUi.selectedListWidget->insertItem(row + 1, item);
      mUi.selectedListWidget->setCurrentItem(item);
   }
}

// Private Implementations

QStringList MapHoverInfo::ListSelectionWidget::GetOptions(const QListWidget& aList) const
{
   QStringList retval;

   for (int i = 0; i < aList.count(); i++)
   {
      retval += aList.item(i)->text();
   }

   return retval;
}

void MapHoverInfo::ListSelectionWidget::RemoveOptionFrom(const QString& aOption, QListWidget& aList)
{
   for (int i = 0; i < aList.count();)
   {
      if (aList.item(i)->text() == aOption)
      {
         delete mUi.selectedListWidget->takeItem(i);
      }
      else
      {
         i++;
      }
   }
}

void MapHoverInfo::ListSelectionWidget::RemoveOptionsFrom(const QStringList& aOptions, QListWidget& aList)
{
   for (const QString& option : aOptions)
   {
      RemoveOptionFrom(option, aList);
   }
}
