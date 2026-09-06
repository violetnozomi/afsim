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

#ifndef WKF_BM_MODEL_BASE_HPP
#define WKF_BM_MODEL_BASE_HPP

#include <QAbstractTableModel>

#include "WkfBM_ModelUtils.hpp"

class QAction;
class QSettings;
class QTableView;

namespace wkf
{
namespace bm
{
struct Configuration;

//! ModelBase allows a QTableView to be a mix between
//!    a tree on the left and a table on the right.
//! Data can be colored to make it easier to interpret at a glance.
//! There are 2 preferred ways to access / modify the data:
//!  * ModelBase can be subclassed to initialize the data cells on creation.
//!  * Member functions returning a proxy object for the data nodes.
//! This class is distinct from PlatformStatusModel so that common functionality
//!    can be reused for a future Base Status display.
class ModelBase : public QAbstractTableModel
{
   //! Callable type that implements an algorithm for use in UpdateCache.
   struct CacheUpdater
   {
      explicit CacheUpdater(ModelBase& aModel) noexcept;

      void operator()(NodeProxy aNode);

      void InsertRow(NodeProxy aNode);
      void RemoveRow(NodeProxy aNode);

      void MoveRowForward(NodeProxy aNode);
      void MoveRowBackward(NodeProxy aNode);

      using iterator = typename std::vector<detail::Node*>::iterator;
      void UpdateNodeRows(iterator aBegin, iterator aEnd);

      ModelBase& mModel;
      int        mRow = 0;
   };

   Q_OBJECT

public:
   //! The first N columns of the model are fixed after construction.
   //! aFixedColumns sets those columns.
   //! Additional columns (data columns) may be added after the fact.
   //! If @param aSpanningColumnZero is true, "column 0" labels will span
   //!    across all columns instead of just their own column.
   //! @note Constructor will throw if there are not enough columns.
   //!       If aSpanningColumnZero is...
   //!          false: requires 1 column
   //!          true:  requires 2 columns
   ModelBase(QStringList aFixedColumns, bool aSpanningColumnZero);

   ~ModelBase() noexcept override = default;

   ModelBase(const ModelBase&) = delete;
   ModelBase(ModelBase&&)      = delete;

   ModelBase& operator=(const ModelBase&) = delete;
   ModelBase& operator=(ModelBase&&) = delete;

   //! Establishes the connections between a QTableView and *this.
   void ConnectToTableView(QTableView* aView);

   //! Override of QAbstractTableModel::rowCount().
   int rowCount(const QModelIndex& aParent = {}) const final;

   //! Override of QAbstractTableModel::columnCount().
   int columnCount(const QModelIndex& aParent = {}) const final;

   //! Override of QAbstractTableModel::data().
   QVariant data(const QModelIndex& aIndex, int aRole = Qt::DisplayRole) const final;

   //! Override of QAbstractTableModel::headerData().
   QVariant headerData(int aSection, Qt::Orientation aOrientation, int aRole = Qt::DisplayRole) const final;

   void sort(int aColumn, Qt::SortOrder aOrder = Qt::AscendingOrder) final;

   //! After making modifications, this should be called to allow those changes to be reflected in the UI.
   void UpdateCache();

   //! Erases all runtime data.
   //! Fixed columns remain.
   //! Forces a cache update.
   void Clear();

   //! Sets mWhiteListNodes and mBlackListNodes.
   //! Applies changes.
   void SetHiddenNodes(const Configuration& aConfiguration);

   //! Sets the white list and black list members of aConfiguration.
   void GetHiddenNodes(Configuration& aConfiguration) const;

   //! Calls aCallable(nodeProxy) for each node in the tree.
   //! aCallable is not called for the invisible root node.
   //! If aConnected is true, changes will be applied automatically.
   //! Otherwise, it is the caller's responsibility to update *this by calling UpdateCache().
   template<typename Callable>
   void ForEachNode(Callable&& aCallable, bool aConnected = true, bool aIncludeInvisibleRoot = false)
   {
      if (aIncludeInvisibleRoot)
      {
         aCallable(GetInvisibleRootNode(aConnected));
      }
      ForEachNodeHelper(mRootNode, aCallable, aConnected);
   }

   //! Calls aCallable(leafProxy) for each leaf in the tree.
   //! If aConnected is true, changes will be applied automatically.
   //! Otherwise, it is the caller's responsibility to update *this by calling UpdateCache().
   template<typename Callable>
   void ForEachLeaf(Callable&& aCallable, bool aConnected = true)
   {
      ForEachLeafHelper(mRootNode, aCallable, aConnected);
   }

   //! Returns a proxy to the invisible root node.
   //! If aConnected is true, the returned proxy will alert *this when it causes changes.
   //! Otherwise, it is the caller's responsibility to update *this by calling UpdateCache().
   NodeProxy GetInvisibleRootNode(bool aConnected = true) noexcept;

   //! Returns a proxy to a node.
   //! aNodeIndex cannot contain more terms than a leaf node would have.
   //! If the requested node cannot be found, returns an invalid proxy.
   //! The returned proxy object should be checked for validity before using.
   //! If aConnected is true, the returned proxy will alert *this when it causes changes.
   //! Otherwise, it is the caller's responsibility to update *this by calling UpdateCache().
   NodeProxy GetNode(const QStringList& aNodeIndex, bool aConnected = true);

   //! Returns a proxy to a node.
   //! aNodeIndex cannot contain more terms than a leaf node would have.
   //! If the requested node cannot be found, creates it.
   //! The cache will not be updated automatically.
   //! The returned proxy object should be checked for validity before using.
   //! If aConnected is true, the returned proxy will alert *this when it causes changes.
   //! Otherwise, it is the caller's responsibility to update *this by calling UpdateCache().
   NodeProxy GetOrAddNode(const QStringList& aNodeIndex, bool aConnected = true);

   //! Returns a proxy to a leaf node.
   //! This requires that aLeafIndex has the correct number of terms.
   //! If the requested leaf cannot be found, returns an invalid proxy.
   //! The returned proxy object should be checked for validity before using.
   //! If aConnected is true, the returned proxy will alert *this when it causes changes.
   //! Otherwise, it is the caller's responsibility to update *this by calling UpdateCache().
   LeafProxy GetLeaf(const QStringList& aLeafIndex, bool aConnected = true);

   //! Returns a proxy to a leaf node.
   //! This requires that aLeafIndex has the correct number of terms.
   //! If the requested leaf cannot be found, creates it.
   //! The returned proxy object should be checked for validity before using.
   //! If aConnected is true, the returned proxy will alert *this when it causes changes.
   //! Otherwise, it is the caller's responsibility to update *this by calling UpdateCache().
   LeafProxy GetOrAddLeaf(const QStringList& aLeafIndex, bool aConnected = true);

   //! Returns a proxy to a node.
   //! If aRow is in the range [0, rowCount()), returns a proxy to the node in that row.
   //! Otherwise, returns an invalid proxy.
   //! The returned proxy object should be checked for validity before using.
   //! If aConnected is true, the returned proxy will alert *this when it causes changes.
   //! Otherwise, it is the caller's responsibility to update *this by calling UpdateCache().
   NodeProxy GetNodeAt(int aRow, bool aConnected = true);

   //! Returns a proxy to a leaf.
   //! If aRow is in the range [0, rowCount()) AND the node in that row is a leaf, returns a proxy to it.
   //! Otherwise, returns an invalid proxy.
   //! The returned proxy object should be checked for validity before using.
   //! If aConnected is true, the returned proxy will alert *this when it causes changes.
   //! Otherwise, it is the caller's responsibility to update *this by calling UpdateCache().
   LeafProxy GetLeafAt(int aRow, bool aConnected = true);

   //! Adds new data columns.
   void AddDataColumns(const QStringList& aTitles);

   //! Removes the data column at aDataIndex.
   //! If aDataIndex is out of bounds, does nothing.
   //! Returns true if a column was removed.
   bool RemoveDataColumn(int aDataIndex);

signals:
   //! Emitted when a detail is double clicked.
   //! aDetailIndex is the index of the detail, not the column it's in.
   void DataDoubleClicked(int aDetailIndex);

public slots:
   //! To be connected to QTableView::doubleClicked.
   void DoubleClicked(const QModelIndex& aIndex);

   //! To be called to get context menu items.
   //! By default, creates menu items for
   //!    - Expand All
   //!    - Collapse All
   //!    - Hide
   //!    - Show All
   virtual QList<QAction*> MenuActions(const QModelIndexList& aSelected);

   //! Called by context menu actions.
   //! Pretty much do exactly what they say.
   //! @{
   void ExpandAll();
   void CollapseAll();
   void Hide(const QModelIndexList& aSelection);
   void HideAllExcept(const QModelIndexList& aSelection);
   void ShowAll();
   //! @}

   //! Returns the number of displayed columns which are "fixed".
   int FixedColumnDisplayCount() const noexcept;

   //! Returns whether logical column zero spans all visible columns.
   bool IsSpanningColumnZero() const noexcept;

   //! Emits dataChanged for the correct index(es) for a logical row and column.
   void EmitFixedColumnColorChanged(int aRow, int aColumn);

protected:
   //! Called as a part of GetOrAdd*() functions to initialize any node being added.
   //! Note that this is called BEFORE the new node is added to the model.
   virtual void InitializeNode(NodeProxy aNode) = 0;

   //! Called as a part of GetOrAdd*() functions to set the data in newly created leaf nodes.
   //! Note that this is called BEFORE the new node is added to the model.
   //! This is called after InitializeNode().
   virtual void InitializeLeaf(LeafProxy aLeaf) = 0;

   const QList<QStringList>& GetWhiteListedNodes() const noexcept;
   const QList<QStringList>& GetBlackListedNodes() const noexcept;

private slots:
   //! Used by MenuActions() (and derivations) to create context menu actions.
   //! @{
   QAction* ExpandAllAction();
   QAction* CollapseAllAction();
   QAction* HideAction(const QModelIndexList& aSelected);
   QAction* HideAllExceptAction(const QModelIndexList& aSelected);
   QAction* ShowAllAction();
   //! @}

private:
   detail::Node               mRootNode;           //!< The invisible root of the Node tree structure.
   std::vector<detail::Node*> mRows;               //!< The nodes organized into the order listed.
   const QStringList          mFixedColumns;       //!< The fixed column titles.
   QStringList                mDataNames;          //!< The data column titles.
   const bool                 mSpanningColumnZero; //!< If true, the first fixed column spans the table.
   bool                       mDefaultExpanding;   //!< If true, new nodes default to expanding.

   QList<QStringList> mWhiteListNodes;
   QList<QStringList> mBlackListNodes;
   int                mSortingColumn = -1;
   bool               mSortAscending = true;

   //! Used by data() for readability.
   //! @{
   QVariant DisplayData(const QModelIndex& aIndex) const;
   QVariant ToolTipData(const QModelIndex& aIndex) const;
   QVariant ColorData(const QModelIndex& aIndex) const;
   QVariant TextColorData(const QModelIndex& aIndex) const;
   //! @}

   //! Sorts the children of aNode according to the sorting column.
   void SortNodeChildren(NodeProxy aNode);

   //! Recursive function used by ForEachNode().
   template<typename Callable>
   void ForEachNodeHelper(detail::Node& aNode, Callable& aCallable, bool aConnected)
   {
      for (auto& child : aNode.mChildren)
      {
         // The call is performed inside the loop so that
         //    aCallable is not called on the root node.
         aCallable(NodeProxy{&child, (aConnected ? this : nullptr)});
         ForEachNodeHelper(child, aCallable, aConnected);
      }
   }

   //! Recursive function used by ForEachLeaf().
   template<typename Callable>
   void ForEachLeafHelper(detail::Node& aNode, Callable& aCallable, bool aConnected)
   {
      if (aNode.mColumn == mFixedColumns.size() - 1)
      {
         aCallable(LeafProxy{&aNode, (aConnected ? this : nullptr), mDataNames});
      }
      else
      {
         for (auto& child : aNode.mChildren)
         {
            ForEachLeafHelper(child, aCallable, aConnected);
         }
      }
   }

   //! Recursive function used by Get*().
   detail::Node* GetNodeHelper(const QStringList& aLeafIndex, detail::Node& aNode);

   //! Recursive function used by GetOrAdd*().
   detail::Node* GetOrAddNodeHelper(const QStringList& aLeafIndex, detail::Node& aNode);

   //! Recursive function used by HideAllExcept().
   //! Returns true if aNode has any children in aSelection.
   bool HideAllExceptHelper(detail::Node& aNode, const QModelIndexList& aSelection);

   //! Constructs a new node and returns it.
   //! InitializeNode() and InitializeLeaf() are both called before it is returned.
   bool MakeNewNode(const QString& aIndex, detail::Node* aParent);

   //! Used by MakeNewNode() to set the initial visibility of aNode
   //!    based on white list and black list.
   //! This is called BEFORE InitializeNode, so custom code can override this.
   bool InitialVisibility(const NodeProxy aNode) const;
};
} // namespace bm
} // namespace wkf

#endif
