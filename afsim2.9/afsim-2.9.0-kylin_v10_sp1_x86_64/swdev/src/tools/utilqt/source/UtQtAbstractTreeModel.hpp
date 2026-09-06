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
#ifndef UTQTABSTRACTTREEMODEL_HPP
#define UTQTABSTRACTTREEMODEL_HPP
#include "utilqt_export.h"

#include <vector>

#include <QAbstractItemModel>

class UtQtAbstractModelItem;

//! Implements a model of a tree where each item in the tree is derived from UtQtAbstractModelItem
class UTILQT_EXPORT UtQtAbstractModel : public QAbstractItemModel
{
public:
   friend class UtQtAbstractModelItem;

   UtQtAbstractModel()
      : mRootPtr()
      , mChangeMode(0)
   {
   }
   ~UtQtAbstractModel() override;

   void reset() { emit dataChanged(QModelIndex(), QModelIndex()); }


   void SetRoot(UtQtAbstractModelItem* aItemPtr);

   UtQtAbstractModelItem* FindItem(const QModelIndex& idx) const;

   int rowCount(const QModelIndex& parent) const override;

   int columnCount(const QModelIndex& parent) const override;

   bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

   QModelIndex index(int row, int column, const QModelIndex& parent) const override;

   QModelIndex parent(const QModelIndex& child) const override;
   QVariant    data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

   bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

   QModelIndex GetRowIndex(UtQtAbstractModelItem& aRow) const;
   QModelIndex GetIndex(UtQtAbstractModelItem& aItemRow, int aColumn) const;
   QModelIndex GetIndex(UtQtAbstractModelItem& aParent, int aRow, int aColumn) const;

   void FakeInsert(UtQtAbstractModelItem& aParent);

   void fetchMore(const QModelIndex& parent) override;
   bool canFetchMore(const QModelIndex& parent) const override;


protected:
   void BeginInsert(UtQtAbstractModelItem& aParent, int aIndex, int aCount = 1);
   void EndInsert();
   void BeginRemove(UtQtAbstractModelItem& aParent, int aIndex, int aCount = 1);
   void EndRemove();
   void BeginMove(UtQtAbstractModelItem& aSource,
                  int                    aSourceIndex,
                  int                    aCount,
                  UtQtAbstractModelItem& aDestination,
                  int                    aDestinationIndex);
   void EndMove();

   Qt::ItemFlags flags(const QModelIndex& index) const override;


   UtQtAbstractModelItem* mRootPtr;
   int                    mChangeMode;
};

//! Objects inheriting this class may be added to a UtQtAbstractModel
class UTILQT_EXPORT UtQtAbstractModelItem : public QObject
{
public:
   friend class UtQtAbstractModel;

   UtQtAbstractModelItem();
   ~UtQtAbstractModelItem() override;

   UtQtAbstractModelItem* GetAbstractItemParent() const { return mItemParentPtr; }

   int GetItemIndex() { return mItemIndex; }
   // Returns the model that contains this item.  May be null
   UtQtAbstractModel* GetModel() { return mModelPtr; }
   // Called by the model.  Should not be called otherwise.
   void                   RemovingFromModel() { mModelPtr = nullptr; }
   UtQtAbstractModelItem* GetAbstractItem(int aIdx) const
   {
      return aIdx < (int)mItemChildren.size() ? mItemChildren[aIdx] : nullptr;
   }
   int GetAbstractItemCount() const { return (int)mItemChildren.size(); }
   int GetNonItemChildCount() const { return mNonItemChildren; }

   void SetNonItemChildCount(int aCount);

   // Warning: These mutators should be used carefully.  Normally these are called by the
   // derived class.
   void                   AddAbstractItem(UtQtAbstractModelItem* aItemPtr, int aIndex = -1);
   void                   RemoveAbstractItem(UtQtAbstractModelItem* aItemPtr);
   UtQtAbstractModelItem* RemoveAbstractItem(int aIndex);
   void                   RemoveAllAbstractItems(bool aDelete = false);

   void MoveAbstractItem(UtQtAbstractModelItem* aItemPtr, int aIndex = -1);

   void AbstractItemChanged(int aFirstCol = 0, int aLastCol = 0);

   void AbstractItemChanged(int aFirstRow, int aFirstCol, int aLastRow, int aLastCol);

   virtual QVariant data(int row, int col, int role)
   {
      UtQtAbstractModelItem* childItem = GetAbstractItem(row);
      if (childItem)
      {
         return childItem->data(col, role);
      }
      return QVariant();
   }

   virtual QVariant      data(int col, int role) { return QVariant(); }
   virtual Qt::ItemFlags flags(int row, int col) const
   {
      UtQtAbstractModelItem* childItem = GetAbstractItem(row);
      if (childItem)
      {
         return childItem->flags(col);
      }
      return Qt::ItemFlags();
   }
   virtual Qt::ItemFlags flags(int col) const { return Qt::ItemFlags(); }
   virtual bool          setData(int row, int col, const QVariant& value, int role)
   {
      UtQtAbstractModelItem* childItem = GetAbstractItem(row);
      if (childItem)
      {
         return childItem->setData(col, value, role);
      }
      return false;
   }
   virtual bool setData(int col, const QVariant& value, int role) { return false; }
   virtual int  rowCount(int aRow) const
   {
      UtQtAbstractModelItem* child = GetAbstractItem(aRow);
      if (child)
      {
         return child->rowCount();
      }
      return 0;
   }
   virtual int rowCount() const { return (int)mItemChildren.size() + mNonItemChildren; }

   virtual int columnCount(int aRow) const
   {
      UtQtAbstractModelItem* child = GetAbstractItem(aRow);
      if (child)
      {
         return child->columnCount();
      }
      return 1;
   }
   virtual int columnCount() const { return 1; }

   virtual void fetchMore() {}
   virtual bool canFetchMore() const { return false; }

   virtual void fetchMore(int aRow)
   {
      UtQtAbstractModelItem* itemPtr = GetAbstractItem(aRow);
      if (itemPtr)
      {
         return itemPtr->fetchMore();
      }
   }
   virtual bool canFetchMore(int aRow) const
   {
      UtQtAbstractModelItem* itemPtr = GetAbstractItem(aRow);
      if (itemPtr)
      {
         return itemPtr->canFetchMore();
      }
      return false;
   }
   virtual bool removeRows(int row, int count) { return false; }

protected:
   virtual void PreRemoveAbstractItem(int aIndex) {}
   virtual void AbstractItemParentChanged() {}
   void         SetAbstractItemParent(UtQtAbstractModelItem* aParent)
   {
      if (mItemParentPtr != aParent)
      {
         mItemParentPtr = aParent;
         AbstractItemParentChanged();
      }
   }

   bool BeginSetModel(UtQtAbstractModel* aModelPtr);

   void FinalizeSetModel();

private:
   void ReIndexChildrenP(int aFirstIndex);
   bool IsInModel() const { return mModelPtr != nullptr; }

   UtQtAbstractModelItem*              mItemParentPtr;
   UtQtAbstractModel*                  mModelPtr;
   int                                 mItemIndex;
   std::vector<UtQtAbstractModelItem*> mItemChildren;
   // Number of rows that are not child items.
   // These immediately follow child item rows
   int mNonItemChildren;
};


// class UtQtAbstractTreeModel;
//
// class UTQT_EXPORT UtQtAbstractTreeItem
//{
//    public:
//       UtQtAbstractTreeItem() : mModelPtr(0), mParentPtr(0) {}
//       UtQtAbstractTreeModel*  mModelPtr;
//       UtQtAbstractTreeItem*   mParentPtr;
//
//       // Indexing requirements:
//       virtual UtQtAbstractTreeItem* child(int row, int col) = 0;
//       virtual int itemRow() const = 0;
//       virtual void rowChanged(int aRow) = 0;
//
//       // Overriding this allows a single item to produce many rows of data
//       // without adding children
//       virtual QVariant data(int aRow, int aCol, int role)
//       {
//          UtQtAbstractTreeItem* childPtr = child(aRow, aCol);
//          if (childPtr)
//          {
//             return childPtr->data(aCol, role);
//          }
//          return QVariant();
//       }
//
//       virtual QVariant data(int aCol, int role) { return QVariant(); }
//       QModelIndex itemIndex(int aColumn=0) const;
//       QModelIndex childIndex(int aRow, int aColumn=0) const;
//
//       virtual int rowCount(int aRow, int aCol) = 0;
//       virtual int columnCount(int aRow, int aCol) = 0;
//
//       virtual bool setData(int aRow, int aCol, const QVariant& value, int role)
//       {
//          return false;
//       }
//       virtual bool insertRows(int row, int count) { return false; }
//       virtual bool insertColumns(int column, int count) { return false; }
//       virtual bool removeRows(int row, int count) { return false; }
//       virtual bool removeColumns(int column, int count) { return false; }
//       virtual void fetchMore(int row, int column) { }
//       virtual bool canFetchMore(int row, int column) const { return false; }
//       virtual Qt::ItemFlags flags(int row, int column) const { return 0; }
//
//
//       void beginInsertRows(int row, int count);
//       void endInsertRows();
//       void beginRemoveRows(int row, int count);
//       void endRemoveRows();
//
//       void dataChanged(int aRowTop, int aRowBottom, int aColLeft, int aColRight);
// };
//
// class UTQT_EXPORT UtQtTreeModelItem : public UtQtAbstractTreeItem
//{
//    public:
//       UtQtTreeModelItem(UtQtAbstractTreeItem* aParentPtr)
//        : mIndex(0),
//          mRowCount(0)
//       {
//          mParentPtr = aParentPtr;
//          mModelPtr = aParentPtr->mModelPtr;
//       }
//       UtQtTreeModelItem(UtQtAbstractTreeModel* aModelPtr)
//        : mIndex(0),
//          mRowCount(0)
//       {
//          mParentPtr = 0;
//          mModelPtr = aModelPtr;
//       }
//       virtual int itemRow() const;
//
//       virtual UtQtAbstractTreeItem* child(int row, int col);
//       virtual void rowChanged(int aRow);
//       virtual bool removeRows(int row, int count);
//       virtual int  rowCount(int aRow, int aCol)
//       {
//          UtQtTreeModelItem* child = childAt(aRow);
//          if (child)
//          {
//             return child->mRowCount;
//          }
//          return 0;
//       }
//
//       UtQtTreeModelItem* childAt(int aIndex) const
//       {
//          if (aIndex < mChildren.size()) return mChildren[aIndex];
//          return 0;
//       }
//       int treeItemIndex() const { return mIndex; }
//
//       void addChild(UtQtTreeModelItem* aItemPtr);
//
//       void updateModelRows(int aRowCount);
//    protected:
//
//       int                        mIndex;
//       // Number of rows.  Should always be >= mChildren.size()
//       // this is the nubmer of rows reported to Qt via the model interface
//       int                        mRowCount;
//       QList<UtQtTreeModelItem*>  mChildren;
// };
//
//// Item class suitable for the invisible root node
// class UTQT_EXPORT UtQtTreeModelItemRoot : public UtQtTreeModelItem
//{
//    public:
//       UtQtTreeModelItemRoot(UtQtAbstractTreeModel* aModelPtr)
//        : UtQtTreeModelItem(aModelPtr)
//       {
//       }
//       virtual int columnCount(int aRow, int aCol)
//       {
//          return 1;
//       }
//       virtual QVariant data(int aRow, int aCol, int role)
//       {
//          return QVariant();
//       }
// };
//
// class UTQT_EXPORT UtQtAbstractTreeModel : public QAbstractItemModel
//{
//    public:
//       friend class UtQtAbstractTreeItem;
//
//       UtQtAbstractTreeItem*      mRootPtr;
//       UtQtAbstractTreeModel() : mRootPtr(0) {}
//
//       void SetRoot(UtQtAbstractTreeItem* aRootPtr) { mRootPtr = aRootPtr; }
//
//       virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex() ) const;
//
//       virtual QModelIndex parent(const QModelIndex &child) const;
//
//       virtual int rowCount(const QModelIndex &parent = QModelIndex() ) const;
//
//       virtual int columnCount(const QModelIndex &parent = QModelIndex() ) const;
//
//       virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
//
//       virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
//
//       virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex() );
//
//       virtual bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex() );
//
//       virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex() );
//
//       virtual bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex() );
//
//       virtual void fetchMore(const QModelIndex &parent);
//
//       virtual bool canFetchMore(const QModelIndex &parent) const;
//
//       virtual Qt::ItemFlags flags(const QModelIndex &index) const;
//
//       UtQtAbstractTreeItem* itemAt(const QModelIndex& aIndex)
//       {
//          UtQtAbstractTreeItem* p = (UtQtAbstractTreeItem*)aIndex.internalPointer();
//          if (p)
//          {
//             return p->child(aIndex.row(), aIndex.column());
//          }
//          else
//          {
//             return mRootPtr;
//          }
//       }
//
//       QModelIndex createItemIndex(const UtQtAbstractTreeItem* aParentPtr, int aRow, int aCol) const
//       {
//          return createIndex(aRow, aCol, (void*)aParentPtr);
//       }
//    protected:
//       void beginInsertRowsP(UtQtAbstractTreeItem* aItem, int row, int count)
//       {
//          beginInsertRows(aItem->itemIndex(0), row, count);
//       }
//       void endInsertRowsP()
//       {
//          endInsertRows();
//       }
//       void beginRemoveRowsP(UtQtAbstractTreeItem* aItem, int row, int count)
//       {
//          beginRemoveRows(aItem->itemIndex(0), row, count);
//       }
//       void endRemoveRowsP()
//       {
//          endRemoveRows();
//       }
//       void dataChangedP(UtQtAbstractTreeItem* aItem, int aRowTop, int aRowBottom, int aColLeft, int aColRight)
//       {
//          dataChanged(aItem->childIndex(aRowTop, aColLeft), aItem->childIndex(aRowBottom, aColRight));
//       }
// };
//
////////////////////////////////////////////////////////////////////////////
//// Qt's abstract item models are a bit too abstract such that
//// combining them is impossible without adding a mapping overhead.
//// With a few changes, we can make a tree model that can be combined more easily
////    1. The root of the model may be something other than the invalid index
////          (QModelIndex())
////    2. If an item has children, its index must point to an instance
////       of ViAbstractTreeModelEntry.  (items without parents need not)
////////////////////////////////////////////////////////////////////////////
////
//// class UtQtAbstractTreeModel;
////
//// class UtQtAbstractTreeModelEntry
//// {
////    public:
////       virtual ~UtQtAbstractTreeModelEntry() {}
////       UtQtAbstractTreeModel*    mModelPtr;
//// };
////
//// class UtQtAbstractTreeModel : public QAbstractItemModel
//// {
////    public:
////       UtQtAbstractTreeModel() : mExternalRoot(0) {}
////
////
////    protected:
////       bool IsRootIndex(const QModelIndex& aIndex) { return aIndex.internalPointer() == mExternalRoot; }
////       UtQtAbstractTreeModelEntry*  mExternalRoot;
//// };
////
//// class UtQtCombinedTreeModel : public UtQtAbstractTreeModel
//// {
////    public:
////       class Entry : public UtQtAbstractTreeModelEntry
////       {
////          public:
////             QString                 mName;
////             UtQtAbstractTreeModel*  mModelPtr;
////       };
////
////       UtQtCombinedTreeModel()
////       {
////          mRootEntry = new Entry;
////          mRootEntry->mModelPtr = this;
////       }
////
////       void AddSubModel(QString aName,
////                        UtQtAbstractTreeModel* aModelPtr)
////       {
////          Entry* e = new Entry;
////          e->mModelPtr = this;
////          e->mName = aName;
////          mEntries.push_back(e);
////       }
////
////       virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex() ) const
////       {
////          UtQtAbstractTreeModelEntry* parentEntry = parent.internalPointer();
////          if (parentEntry == mExternalRoot)
////          {
////             if (parent.row() < mEntries.size())
////             {
////                Entry* e = mEntries[parent.row()];
////                createIndex(row, column, e);
////             }
////          }
////          else
////          {
////             QModelIndex idx = parentEntry->mModelPtr->index(row, column, parent);
////             return createIndex(idx.row(), idx.column(), idx.internalPointer());
////          }
////          return QModelIndex();
////       }
////
////       virtual QModelIndex parent(const QModelIndex &child) const
////       {
////          UtQtAbstractTreeModelEntry* parentEntry = parent.internalPointer();
////          if (mExternalRoot == parentEntry)
////          {
////             return QModelIndex();
////          }
////          else
////          {
////             convertToLocalIndex(parentEntry->mModelPtr->parent(child));
////          }
////       }
////
////       virtual int rowCount(const QModelIndex &parent = QModelIndex() ) const
////       {
////          if (! parent.isValid()) return 1;
////          if (parent.internalPointer() == mRootEntry)
////          {
////             return mEntries.size();
////          }
////          Entry* parentEntry = parent.internalPointer();
////          QModelIndex idx = parentEntry->mModelPtr->rowCount(parent);
////       }
////
////       virtual int columnCount(const QModelIndex &parent = QModelIndex() ) const
////       {
////          if (! parent.isValid()) return 1;
////       }
////
////       virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
////       {
////          throw std::exception("The method or operation is not implemented.");
////       }
////    protected:
////
////       QModelIndex convertToLocalIndex(const QModelIndex& aIndex)
////       {
////          return createIndex(aIndex.row(), aIndex.column(), aIndex.internalPointer());
////       }
////
////       QList<Entry*>     mEntries;
////       Entry*            mRootEntry;
//// };
//
#endif
