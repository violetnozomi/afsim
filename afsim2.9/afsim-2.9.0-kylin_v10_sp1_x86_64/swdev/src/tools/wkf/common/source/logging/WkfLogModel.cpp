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

#include "WkfLogModel.hpp"

static bool Overlap(const QStringList& aFirst, const QStringList& aSecond) noexcept
{
   for (const auto& a : aFirst)
   {
      for (const auto& b : aSecond)
      {
         if (a == b)
         {
            return true;
         }
      }
   }

   return false;
}

static int Severity(const ut::log::TypeList& aTypes) noexcept
{
   using ut::log::Message;

   if (aTypes.Contains(Message::Fatal()))
   {
      return 0;
   }
   else if (aTypes.Contains(Message::Error()))
   {
      return 1;
   }
   else if (aTypes.Contains(Message::Warning()))
   {
      return 2;
   }
   else if (aTypes.Contains(Message::Info()))
   {
      return 3;
   }
   else if (aTypes.Contains(Message::Debug()))
   {
      return 4;
   }
   else if (aTypes.Contains(Message::Developer()))
   {
      return 5;
   }
   else
   {
      return 6;
   }
}

wkf::log::MasterModel::MasterModel()
{
   mBaseModel.setColumnCount(1);
   ClearMessages();
}

wkf::log::MasterModel::~MasterModel()
{
   ClearMessages();
}

void wkf::log::MasterModel::QueueMessage(const ut::log::Message& aMessage) noexcept
{
   auto* item = MakeItem(aMessage);
   for (const auto& note : aMessage.mNotes)
   {
      AddMessageTo(note, *item);
   }
   mQueue.append(item);
}

void wkf::log::MasterModel::ApplyQueuedMessages() noexcept
{
   if (!mQueue.empty())
   {
      mBaseModel.invisibleRootItem()->appendRows(mQueue);

      const int initialErrorCount   = mErrorCount;
      const int initialWarningCount = mWarningCount;

      for (const auto* item : mQueue)
      {
         const int severity = item->data(cMSG_SEVERITY_ROLE).toInt();
         if (severity <= 1) // 0 = Fatal, 1 = Error
         {
            mErrorCount++;
         }
         else if (severity == 2) // 2 = Warning
         {
            mWarningCount++;
         }
      }

      if (initialErrorCount != mErrorCount)
      {
         emit ErrorCountChanged(mErrorCount);
      }
      if (initialWarningCount != mWarningCount)
      {
         emit WarningCountChanged(mWarningCount);
      }

      mQueue.clear();
   }
}

void wkf::log::MasterModel::ClearMessages() noexcept
{
   mBaseModel.clear();

   for (auto* msg : mQueue)
   {
      delete msg;
   }
   mQueue.clear();

   mErrorCount   = 0;
   mWarningCount = 0;

   emit ErrorCountChanged(0);
   emit WarningCountChanged(0);
}

QAbstractItemModel* wkf::log::MasterModel::GetModel() noexcept
{
   return &mBaseModel;
}

void wkf::log::MasterModel::AddMessageTo(const ut::log::Message& aMessage, QStandardItem& aParent) noexcept
{
   using ut::log::Message;

   auto* item = MakeItem(aMessage);

   aParent.appendRow(item);

   for (const auto& note : aMessage.mNotes)
   {
      AddMessageTo(note, *item);
   }
}

QStandardItem* wkf::log::MasterModel::MakeItem(const ut::log::Message& aMessage) const noexcept
{
   using ut::log::Message;

   auto* item = new QStandardItem;

   QStringList types;
   for (const auto& type : aMessage.mTypes)
   {
      types += QString::fromStdString(type);
   }
   item->setData(types, cMSG_TYPE_ROLE);
   item->setData(Severity(aMessage.mTypes), cMSG_SEVERITY_ROLE);
   item->setText(QString::fromStdString(aMessage.mData));
   item->setSizeHint(QSize(0, 24));

   // Set the displayed icon.
   if (aMessage.mTypes.Contains(Message::Fatal()))
   {
      item->setIcon(QIcon::fromTheme("explosion"));
      item->setToolTip("Fatal");
   }
   else if (aMessage.mTypes.Contains(Message::Error()))
   {
      item->setIcon(QIcon::fromTheme("delete"));
      item->setToolTip("Error");
   }
   else if (aMessage.mTypes.Contains(Message::Warning()))
   {
      item->setIcon(QIcon::fromTheme("alert"));
      item->setToolTip("Warning");
   }
   else if (aMessage.mTypes.Contains(Message::Info()))
   {
      // item->setIcon(QIcon::fromTheme("empty"));
      item->setToolTip("Info");
   }
   else if (aMessage.mTypes.Contains(Message::Debug()))
   {
      item->setIcon(QIcon::fromTheme("wrench"));
      item->setToolTip("Debug");
   }
   else if (aMessage.mTypes.Contains(Message::Developer()))
   {
      item->setIcon(QIcon::fromTheme("speed"));
      item->setToolTip("Developer");
   }
   else if (!aMessage.mTypes.empty())
   {
      item->setIcon(QIcon::fromTheme("question"));
      item->setToolTip("Unknown");
   }
   else
   {
      // item->setIcon(QIcon::fromTheme("empty"));
   }

   return item;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

wkf::log::FilterModel::FilterModel(MasterModel& aMaster) noexcept
{
   setSourceModel(aMaster.GetModel());
   sort(0);
}

void wkf::log::FilterModel::AddType(QString aType) noexcept
{
   if (!mSettings.mTypes.contains(aType))
   {
      mSettings.mTypes += std::move(aType);
      invalidateFilter();
   }
}

void wkf::log::FilterModel::RemoveType(const QString& aType) noexcept
{
   if (mSettings.mTypes.removeOne(aType))
   {
      invalidateFilter();
   }
}

void wkf::log::FilterModel::SetSortingMethod(FilterSettings::Sorting aSorting) noexcept
{
   if (mSettings.mSorting != aSorting)
   {
      mSettings.mSorting = aSorting;
      invalidate();
   }
}

void wkf::log::FilterModel::SetSortingReversed(bool aReverse) noexcept
{
   if (mSettings.mReverseSorting != aReverse)
   {
      mSettings.mReverseSorting = aReverse;
      invalidate();
   }
}

void wkf::log::FilterModel::Find(QString aString) noexcept
{
   if (mSettings.mFind != aString)
   {
      mSettings.mFind = std::move(aString);
      invalidateFilter();
   }
}

bool wkf::log::FilterModel::filterAcceptsRow(int aRow, const QModelIndex& aParent) const
{
   if (!mSettings.mFind.isEmpty())
   {
      auto currentIndex = sourceModel()->index(aRow, 0, aParent);

      // Find text in message or parents
      auto itt = currentIndex;
      while (itt.isValid())
      {
         if (itt.data().toString().contains(mSettings.mFind, Qt::CaseInsensitive))
         {
            return true;
         }
         itt = itt.parent();
      }

      // Find text in notes
      return SearchChildren(currentIndex);
   }
   else if (aParent.isValid())
   {
      return true;
   }

   return Overlap(mSettings.mTypes, GetTypes(aRow));
}

bool wkf::log::FilterModel::lessThan(const QModelIndex& aLeft, const QModelIndex& aRight) const
{
   const bool first = aLeft.row() < aRight.row();

   // If is note
   if (aLeft.parent().isValid())
   {
      // This return is NOT affected by reverse.
      return first;
   }

   switch (mSettings.mSorting)
   {
   case FilterSettings::Sorting::Chronological:
      return Reverse(first);
   case FilterSettings::Sorting::Type:
   {
      const int leftSeverity  = GetSeverity(aLeft.row());
      const int rightSeverity = GetSeverity(aRight.row());

      if (leftSeverity != rightSeverity)
      {
         return Reverse(leftSeverity < rightSeverity);
      }
      break;
   }
   case FilterSettings::Sorting::Alphabetical:
   {
      const QString leftText  = GetText(aLeft.row());
      const QString rightText = GetText(aRight.row());

      if (leftText != rightText)
      {
         return Reverse(leftText < rightText);
      }
      break;
   }
   }

   return Reverse(first);
}

bool wkf::log::FilterModel::SearchChildren(const QModelIndex& aParent) const noexcept
{
   const int rows = sourceModel()->rowCount(aParent);

   for (int i = 0; i < rows; i++)
   {
      auto child = sourceModel()->index(i, 0, aParent);
      if (child.data().toString().contains(mSettings.mFind, Qt::CaseInsensitive))
      {
         return true;
      }
      if (SearchChildren(child))
      {
         return true;
      }
   }

   return false;
}

int wkf::log::FilterModel::GetSeverity(int aRow) const noexcept
{
   if (sourceModel())
   {
      return sourceModel()->index(aRow, 0).data(cMSG_SEVERITY_ROLE).toInt();
   }
   return 0;
}

QString wkf::log::FilterModel::GetText(int aRow) const noexcept
{
   if (sourceModel())
   {
      return sourceModel()->index(aRow, 0).data(Qt::DisplayRole).toString();
   }
   return QString();
}

QStringList wkf::log::FilterModel::GetTypes(int aRow) const noexcept
{
   if (sourceModel())
   {
      return sourceModel()->index(aRow, 0).data(cMSG_TYPE_ROLE).toStringList();
   }
   return QStringList();
}

bool wkf::log::FilterModel::Reverse(bool aRetval) const noexcept
{
   return aRetval != mSettings.mReverseSorting;
}
