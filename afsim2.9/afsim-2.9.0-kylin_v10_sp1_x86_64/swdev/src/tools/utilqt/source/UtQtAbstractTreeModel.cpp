// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "UtQtAbstractTreeModel.hpp"

#include <algorithm>
#include <cassert>

#include "UtCast.hpp"

void UtQtAbstractModel::SetRoot(UtQtAbstractModelItem* aItemPtr)
{
   if (mRootPtr)
   {
      mRootPtr->mModelPtr = nullptr;
      mRootPtr            = nullptr;
   }
   if (aItemPtr)
   {
      mRootPtr             = aItemPtr;
      mRootPtr->mModelPtr  = this;
      mRootPtr->mItemIndex = 0;
   }
}

void UtQtAbstractModel::BeginInsert(UtQtAbstractModelItem& aParent, int aIndex, int aCount)
{
   assert(mChangeMode == 0);
   mChangeMode = 1;
   beginInsertRows(GetRowIndex(aParent), aIndex, aIndex + aCount - 1);
}

void UtQtAbstractModel::EndInsert()
{
   assert(mChangeMode == 1);
   mChangeMode = 0;
   endInsertRows();
}

void UtQtAbstractModel::BeginRemove(UtQtAbstractModelItem& aParent, int aIndex, int aCount)
{
   assert(mChangeMode == 0);
   mChangeMode = 2;
   beginRemoveRows(GetRowIndex(aParent), aIndex, aIndex + aCount - 1);
}

void UtQtAbstractModel::BeginMove(UtQtAbstractModelItem& aSource,
                                  int                    aSourceIndex,
                                  int                    aCount,
                                  UtQtAbstractModelItem& aDestination,
                                  int                    aDestinationIndex)
{
   assert(mChangeMode == 0);
   mChangeMode = 3;
   // rowsAboutToBeMoved(GetRowIndex(aSource), aSourceIndex, aSourceIndex + aCount - 1, GetRowIndex(aDestination), aDestinationIndex);
   beginMoveRows(GetRowIndex(aSource), aSourceIndex, aSourceIndex + aCount - 1, GetRowIndex(aDestination), aDestinationIndex);
}

UtQtAbstractModelItem* UtQtAbstractModel::FindItem(const QModelIndex& idx) const
{
   if (idx.isValid())
   {
      UtQtAbstractModelItem* p = (UtQtAbstractModelItem*)idx.internalPointer();
      assert(p && "Invalid index");
      if (p)
      {
         return ((size_t)idx.row() < p->mItemChildren.size()) ? p->GetAbstractItem(idx.row()) : nullptr;
      }
   }
   else
   {
      return mRootPtr;
   }

   return nullptr;
}

int UtQtAbstractModel::rowCount(const QModelIndex& parent) const
{
   UtQtAbstractModelItem* itemPtr = (UtQtAbstractModelItem*)parent.internalPointer();
   if (itemPtr)
   {
      return itemPtr->rowCount(parent.row());
   }
   else if (!parent.isValid())
   {
      return mRootPtr->rowCount();
   }
   return 1;
}

int UtQtAbstractModel::columnCount(const QModelIndex& parent) const
{
   UtQtAbstractModelItem* itemPtr = (UtQtAbstractModelItem*)parent.internalPointer();
   if (itemPtr)
   {
      return itemPtr->columnCount(parent.row());
   }
   else if (!parent.isValid())
   {
      return 1;
   }
   return 1;
}

QModelIndex UtQtAbstractModel::index(int row, int column, const QModelIndex& parent) const
{
   UtQtAbstractModelItem* itemPtr = FindItem(parent);
   if (!itemPtr)
      return QModelIndex();
   if ((row > itemPtr->rowCount()) || (column > itemPtr->columnCount()))
      return QModelIndex();
   return createIndex(row, column, itemPtr);
}

QModelIndex UtQtAbstractModel::parent(const QModelIndex& child) const
{
   if (!child.isValid())
      return QModelIndex();
   UtQtAbstractModelItem* parentPtr = (UtQtAbstractModelItem*)child.internalPointer();
   if (parentPtr != nullptr)
   {
      if (parentPtr->GetAbstractItemParent())
      {
         return createIndex(parentPtr->mItemIndex, 0, parentPtr->GetAbstractItemParent());
      }
   }
   return QModelIndex();
}

QModelIndex UtQtAbstractModel::GetRowIndex(UtQtAbstractModelItem& aRow) const
{
   assert(aRow.mItemIndex >= 0);
   if (&aRow == mRootPtr)
   {
      return QModelIndex();
   }
   else
   {
      return createIndex(aRow.mItemIndex, 0, aRow.GetAbstractItemParent());
   }
}

QModelIndex UtQtAbstractModel::GetIndex(UtQtAbstractModelItem& aItemRow, int aColumn) const
{
   if (&aItemRow == mRootPtr)
   {
      return QModelIndex();
   }
   else
   {
      return createIndex(aItemRow.mItemIndex, aColumn, aItemRow.GetAbstractItemParent());
   }
}
QModelIndex UtQtAbstractModel::GetIndex(UtQtAbstractModelItem& aParent, int aRow, int aColumn) const
{
   return createIndex(aRow, aColumn, &aParent);
}
void UtQtAbstractModel::FakeInsert(UtQtAbstractModelItem& aParent)
{
   BeginInsert(aParent, 0);
   EndInsert();
}

void UtQtAbstractModel::EndRemove()
{
   assert(mChangeMode == 2);
   mChangeMode = 0;
   endRemoveRows();
}

void UtQtAbstractModel::EndMove()
{
   assert(mChangeMode == 3);
   mChangeMode = 0;
   endMoveRows();
}

QVariant UtQtAbstractModel::data(const QModelIndex& index, int role /*= Qt::DisplayRole*/) const
{
   if (index.internalPointer())
   {
      UtQtAbstractModelItem* itemPtr = (UtQtAbstractModelItem*)index.internalPointer();
      return itemPtr->data(index.row(), index.column(), role);
   }
   else
   {
      return mRootPtr->data(index.row(), index.column(), role);
   }
}

bool UtQtAbstractModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
{
   if (index.internalPointer())
   {
      UtQtAbstractModelItem* itemPtr = (UtQtAbstractModelItem*)index.internalPointer();
      return itemPtr->setData(index.row(), index.column(), value, role);
   }
   return false;
}

void UtQtAbstractModel::fetchMore(const QModelIndex& parent)
{
   UtQtAbstractModelItem* itemPtr = (UtQtAbstractModelItem*)parent.internalPointer();
   if (itemPtr)
   {
      itemPtr->fetchMore(parent.row());
   }
   else
   {
      UtQtAbstractModelItem* itemPtr = FindItem(parent);
      if (itemPtr)
      {
         itemPtr->fetchMore();
      }
   }
}

bool UtQtAbstractModel::canFetchMore(const QModelIndex& parent) const
{
   UtQtAbstractModelItem* itemPtr = (UtQtAbstractModelItem*)parent.internalPointer();
   if (itemPtr)
   {
      return itemPtr->canFetchMore(parent.row());
   }
   else
   {
      UtQtAbstractModelItem* itemPtr = FindItem(parent);
      if (itemPtr)
      {
         return itemPtr->canFetchMore();
      }
   }
   return false;
}

Qt::ItemFlags UtQtAbstractModel::flags(const QModelIndex& index) const
{
   if (index.internalPointer())
   {
      UtQtAbstractModelItem* itemPtr = (UtQtAbstractModelItem*)index.internalPointer();
      return itemPtr->flags(index.row(), index.column());
   }
   else
   {
      return mRootPtr->flags(index.row(), index.column());
   }
}

bool UtQtAbstractModel::removeRows(int row, int count, const QModelIndex& parent /*= QModelIndex()*/)
{
   UtQtAbstractModelItem* parentPtr = FindItem(parent);
   if (parentPtr)
   {
      return parentPtr->removeRows(row, count);
   }
   return false;
}

UtQtAbstractModel::~UtQtAbstractModel()
{
   mRootPtr->RemovingFromModel();
   delete mRootPtr;
}


UtQtAbstractModelItem::UtQtAbstractModelItem()
   : mItemParentPtr()
   , mModelPtr()
   , mItemIndex(-1)
   , mNonItemChildren(0)
{
}

UtQtAbstractModelItem::~UtQtAbstractModelItem()
{
   assert(!IsInModel());
   RemoveAllAbstractItems(true);
}

void UtQtAbstractModelItem::AddAbstractItem(UtQtAbstractModelItem* aItemPtr, int aIndex)
{
   if (aIndex == -1)
   {
      aIndex = ut::cast_to_int(mItemChildren.size());
   }
   assert(aIndex >= 0 && aIndex <= ut::cast_to_int(mItemChildren.size()));

   // assert(mModelPtr);
   bool modelInsertion = IsInModel();
   if (mNonItemChildren > 0)
   {
      --mNonItemChildren;
      modelInsertion = false;
   }
   if (modelInsertion)
   {
      mModelPtr->BeginInsert(*this, aIndex);
   }

   aItemPtr->SetAbstractItemParent(this);
   aItemPtr->mItemIndex = aIndex;
   // aItemPtr->SetModel(mModelPtr);
   bool addedToModel = aItemPtr->BeginSetModel(mModelPtr);
   mItemChildren.insert(mItemChildren.begin() + aIndex, aItemPtr);
   ReIndexChildrenP(aIndex + 1);
   if (modelInsertion)
   {
      mModelPtr->EndInsert();
   }
   if (addedToModel)
   {
      aItemPtr->FinalizeSetModel();
   }
}

UtQtAbstractModelItem* UtQtAbstractModelItem::RemoveAbstractItem(int aIndex)
{
   PreRemoveAbstractItem(aIndex);

   int                    idx     = aIndex;
   UtQtAbstractModelItem* itemPtr = mItemChildren[idx];
   if (IsInModel())
   {
      mModelPtr->BeginRemove(*this, idx);
   }

   mItemChildren.erase(mItemChildren.begin() + idx);
   ReIndexChildrenP(idx);
   itemPtr->SetAbstractItemParent(nullptr);
   itemPtr->mItemIndex = -1;
   itemPtr->mModelPtr  = nullptr;

   if (IsInModel())
   {
      mModelPtr->EndRemove();
   }

   return itemPtr;
}

void UtQtAbstractModelItem::RemoveAbstractItem(UtQtAbstractModelItem* aItemPtr)
{
   assert(aItemPtr->mItemIndex != -1);
   assert(mItemChildren[aItemPtr->mItemIndex] == aItemPtr);
   RemoveAbstractItem(aItemPtr->mItemIndex);
}

// Move aItemPtr from its current location to a child of this at aIndex
void UtQtAbstractModelItem::MoveAbstractItem(UtQtAbstractModelItem* aItemPtr, int aIndex /*=-1*/)
{
   // Only do special move logic if we are in an item model
   if (!mModelPtr)
   {
      if (aItemPtr->GetAbstractItemParent())
      {
         aItemPtr->GetAbstractItemParent()->RemoveAbstractItem(aItemPtr);
      }
      AddAbstractItem(aItemPtr, aIndex);
      return;
   }

   assert(mModelPtr);

   if (aIndex == -1)
   {
      aIndex = ut::cast_to_int(mItemChildren.size());
   }
   assert(aIndex >= 0 && aIndex <= ut::cast_to_int(mItemChildren.size()));

   int                    oldIndex     = aItemPtr->GetItemIndex();
   UtQtAbstractModelItem* oldParentPtr = aItemPtr->GetAbstractItemParent();
   if (oldParentPtr == this && aIndex == ut::cast_to_int(mItemChildren.size()))
   {
      // adjust the index if the parent is the same and we are moving to the end of the list
      aIndex--;
   }

   // if we are not moving anywhere, abort
   if (oldParentPtr == this && aIndex == oldIndex)
   {
      return;
   }

   PreRemoveAbstractItem(oldIndex);

   QModelIndex sourceIndex;
   if (IsInModel())
   {
      mModelPtr->BeginMove(*oldParentPtr, oldIndex, 1, *this, aIndex);
      sourceIndex = mModelPtr->GetRowIndex(*aItemPtr);
   }

   oldParentPtr->mItemChildren.erase(oldParentPtr->mItemChildren.begin() + oldIndex);
   oldParentPtr->ReIndexChildrenP(oldIndex);
   aItemPtr->SetAbstractItemParent(this);
   aItemPtr->mItemIndex = aIndex;
   // aItemPtr->SetModel(mModelPtr);
   bool addedToModel = aItemPtr->BeginSetModel(mModelPtr);
   mItemChildren.insert(mItemChildren.begin() + aIndex, aItemPtr);
   ReIndexChildrenP(aIndex + 1);

   if (IsInModel())
   {
      mModelPtr->EndMove();
   }
   if (addedToModel)
   {
      aItemPtr->FinalizeSetModel();
   }
}

void UtQtAbstractModelItem::RemoveAllAbstractItems(bool aDelete)
{
   if (mItemChildren.empty())
      return;
   if (IsInModel())
      mModelPtr->BeginRemove(*this, 0, (int)mItemChildren.size());
   for (size_t i = 0; i < mItemChildren.size(); ++i)
   {
      mItemChildren[i]->SetAbstractItemParent(nullptr);
      mItemChildren[i]->mItemIndex = -1;
      mItemChildren[i]->mModelPtr  = nullptr;
      if (aDelete)
      {
         delete mItemChildren[i];
      }
   }
   mItemChildren.clear();
   if (IsInModel())
      mModelPtr->EndRemove();
}

void UtQtAbstractModelItem::ReIndexChildrenP(int aFirstIndex)
{
   for (int i = aFirstIndex; i < (int)mItemChildren.size(); ++i)
   {
      mItemChildren[i]->mItemIndex = i;
   }
}

bool UtQtAbstractModelItem::BeginSetModel(UtQtAbstractModel* aModelPtr)
{
   if (mModelPtr != aModelPtr)
   {
      mModelPtr = aModelPtr;
      if (!mModelPtr)
      {
         return false;
      }
      return true;
   }
   return false;
}

void UtQtAbstractModelItem::FinalizeSetModel()
{
   assert(mModelPtr != nullptr && mItemIndex != -1);
   if (!mItemChildren.empty())
   {
      mModelPtr->beginInsertRows(mModelPtr->GetRowIndex(*this), 0, ut::cast_to_int(mItemChildren.size() - 1));
      for (size_t i = 0; i < mItemChildren.size(); ++i)
      {
         if (!mItemChildren[i]->BeginSetModel(mModelPtr))
         {
            throw;
         }
      }
      mModelPtr->endInsertRows();

      for (size_t i = 0; i < mItemChildren.size(); ++i)
      {
         mItemChildren[i]->FinalizeSetModel();
      }
   }
}

//! Informs Qt that the item's data has changed
void UtQtAbstractModelItem::AbstractItemChanged(int aFirstCol, int aLastCol)
{
   if (mModelPtr)
   {
      mModelPtr->dataChanged(mModelPtr->GetIndex(*this, aFirstCol), mModelPtr->GetIndex(*this, aLastCol));
   }
}

void UtQtAbstractModelItem::AbstractItemChanged(int aFirstRow, int aFirstCol, int aLastRow, int aLastCol)
{
   if (mModelPtr)
   {
      mModelPtr->dataChanged(mModelPtr->GetIndex(*this, aFirstRow, aFirstCol),
                             mModelPtr->GetIndex(*this, aLastRow, aLastCol));
   }
}

void UtQtAbstractModelItem::SetNonItemChildCount(int aCount)
{
   if (mNonItemChildren != aCount)
   {
      if (mNonItemChildren < aCount)
      {
         if (IsInModel())
         {
            mModelPtr->BeginInsert(*this, ut::cast_to_int(mNonItemChildren + mItemChildren.size()), aCount - mNonItemChildren);
            mNonItemChildren = aCount;
            mModelPtr->EndInsert();
         }
      }
      else
      {
         mModelPtr->BeginRemove(*this, ut::cast_to_int(aCount + mItemChildren.size()), mNonItemChildren - aCount);
         mNonItemChildren = aCount;
         mModelPtr->EndRemove();
      }
   }
}


//
// bool UtQtTreeModelItem::removeRows(int row, int count)
//{
//   for (int i = 0; i < count && row < mChildren.size(); ++i)
//   {
//      delete mChildren[row];
//      mChildren.removeAt(row);
//   }
//   for (int i = row; i < mChildren.size(); ++i)
//   {
//      mChildren[i]->rowChanged(i);
//   }
//   return true;
//}
//
// void UtQtTreeModelItem::rowChanged(int aRow)
//{
//   mIndex = aRow;
//}
//
// UtQtAbstractTreeItem* UtQtTreeModelItem::child(int row, int col)
//{
//   if (row < mChildren.size()) return mChildren[row];
//   return 0;
//}
//
// int UtQtTreeModelItem::itemRow() const
//{
//   return mIndex;
//}
//
// void UtQtTreeModelItem::updateModelRows(int aRowCount)
//{
//   if (mRowCount == aRowCount) return;
//   if (mRowCount < aRowCount)
//   {
//      beginInsertRows(mRowCount, aRowCount);
//      mRowCount = aRowCount;
//      endInsertRows();
//   }
//   else if (mRowCount > aRowCount)
//   {
//      beginRemoveRows(aRowCount, mRowCount);
//      mRowCount = aRowCount;
//      endRemoveRows();
//   }
//}
//
// void UtQtTreeModelItem::addChild(UtQtTreeModelItem* aItemPtr)
//{
//   int newIndex = mChildren.size();
//   aItemPtr->mIndex = newIndex;
//   mChildren.push_back(aItemPtr);
//   updateModelRows(std::max(mRowCount, mChildren.size()));
//}
//
// QModelIndex UtQtAbstractTreeItem::itemIndex(int aColumn/*=0*/) const
//{
//   return mModelPtr->createItemIndex(mParentPtr, itemRow(), aColumn);
//}
// QModelIndex UtQtAbstractTreeItem::childIndex(int aRow, int aColumn) const
//{
//   return mModelPtr->createItemIndex(this, aRow, aColumn);
//}
// void UtQtAbstractTreeItem::beginInsertRows(int row, int count)
//{
//   mModelPtr->beginInsertRowsP(this, row, count);
//}
//
// void UtQtAbstractTreeItem::endInsertRows()
//{
//   mModelPtr->endInsertRowsP();
//}
//
// void UtQtAbstractTreeItem::beginRemoveRows(int row, int count)
//{
//   mModelPtr->beginRemoveRowsP(this, row, count);
//}
//
// void UtQtAbstractTreeItem::endRemoveRows()
//{
//   mModelPtr->endRemoveRowsP();
//}
//
// void UtQtAbstractTreeItem::dataChanged(int aRowTop, int aRowBottom, int aColLeft, int aColRight)
//{
//   mModelPtr->dataChangedP(this, aRowTop, aRowBottom, aColLeft, aColRight);
//}
//
// Qt::ItemFlags UtQtAbstractTreeModel::flags(const QModelIndex &index) const
//{
//   UtQtAbstractTreeItem* p = (UtQtAbstractTreeItem*)index.internalPointer();
//   if (p)
//   {
//      return p->flags(index.row(), index.column());
//   }
//   return QAbstractItemModel::flags(index);
//}
//
// bool UtQtAbstractTreeModel::canFetchMore(const QModelIndex &parent) const
//{
//   UtQtAbstractTreeItem* p = (UtQtAbstractTreeItem*)parent.internalPointer();
//   if (p)
//   {
//      return p->canFetchMore(parent.row(), parent.column());
//   }
//   return false;
//}
//
// void UtQtAbstractTreeModel::fetchMore(const QModelIndex &parent)
//{
//   UtQtAbstractTreeItem* p = (UtQtAbstractTreeItem*)parent.internalPointer();
//   if (p)
//   {
//      p->fetchMore(parent.row(), parent.column());
//   }
//}
//
// bool UtQtAbstractTreeModel::removeColumns(int column, int count, const QModelIndex &parent /*= QModelIndex() */)
//{
//   UtQtAbstractTreeItem* p = itemAt(parent);
//   if (p)
//   {
//      return p->removeColumns(column, count);
//   }
//   return false;
//}
//
// bool UtQtAbstractTreeModel::removeRows(int row, int count, const QModelIndex &parent /*= QModelIndex() */)
//{
//   UtQtAbstractTreeItem* p = itemAt(parent);
//   if (p)
//   {
//      return p->removeRows(row, count);
//   }
//   return false;
//}
//
// bool UtQtAbstractTreeModel::insertColumns(int column, int count, const QModelIndex &parent /*= QModelIndex() */)
//{
//   UtQtAbstractTreeItem* p = itemAt(parent);
//   if (p)
//   {
//      return p->insertColumns(column, count);
//   }
//   return false;
//}
//
// bool UtQtAbstractTreeModel::insertRows(int row, int count, const QModelIndex &parent /*= QModelIndex() */)
//{
//   UtQtAbstractTreeItem* p = itemAt(parent);
//   if (p)
//   {
//      return p->insertRows(row, count);
//   }
//   return false;
//}
//
// bool UtQtAbstractTreeModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/)
//{
//   UtQtAbstractTreeItem* p = (UtQtAbstractTreeItem*)index.internalPointer();
//   if (p)
//   {
//      return p->setData(index.row(), index.column(), value, role);
//   }
//   return false;
//}
//
// QVariant UtQtAbstractTreeModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
//{
//   UtQtAbstractTreeItem* p = (UtQtAbstractTreeItem*)index.internalPointer();
//   if (p)
//   {
//      return p->data(index.row(), index.column(), role);
//   }
//   return QVariant();
//}
//
// int UtQtAbstractTreeModel::columnCount(const QModelIndex &parent /*= QModelIndex() */) const
//{
//   UtQtAbstractTreeItem* p = (UtQtAbstractTreeItem*)parent.internalPointer();
//   if (p)
//   {
//      return p->columnCount(parent.row(), parent.column());
//   }
//   return 1;
//}
//
// int UtQtAbstractTreeModel::rowCount(const QModelIndex &parent /*= QModelIndex() */) const
//{
//   UtQtAbstractTreeItem* p = (UtQtAbstractTreeItem*)parent.internalPointer();
//   if (p)
//   {
//      return p->rowCount(parent.row(), parent.column());
//   }
//   return 1;
//}
//
// QModelIndex UtQtAbstractTreeModel::parent(const QModelIndex &child) const
//{
//   UtQtAbstractTreeItem* p = (UtQtAbstractTreeItem*)child.internalPointer();
//   if (p)
//   {
//      return createIndex(p->itemRow(), 0, p->mParentPtr);
//   }
//   return QModelIndex();
//}
//
// QModelIndex UtQtAbstractTreeModel::index(int row, int column, const QModelIndex &parent /*= QModelIndex() */) const
//{
//   UtQtAbstractTreeItem* p = (UtQtAbstractTreeItem*)parent.internalPointer();
//   if (p)
//   {
//      UtQtAbstractTreeItem* c = p->child(row, column);
//      if (c)
//      {
//         return createIndex(row, column, c);
//      }
//   }
//   return QModelIndex();
//}
