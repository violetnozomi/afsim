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

#include "WkfBM_ModelBase.hpp"

#include <algorithm>
#include <functional>

#include <QAction>
#include <QFont>
#include <QMenu>
#include <QTableView>

#include "UtAlgorithm.hpp"
#include "UtCast.hpp"
#include "UtQtMemory.hpp"
#include "WkfBM_PlatformStatusPrefObject.hpp"
#include "misc/WkfExportHelper.hpp"

static QVariant ColorSettingToVariant(const wkf::bm::ColorSetting& aColor) noexcept
{
   if (aColor)
   {
      return *aColor;
   }
   return QVariant();
}

// ========================================= //
//     wkf::bm::ModelBase::CacheUpdater      //
// ========================================= //

wkf::bm::ModelBase::CacheUpdater::CacheUpdater(ModelBase& aModel) noexcept
   : mModel(aModel)
   , mRow(0)
{
}

void wkf::bm::ModelBase::CacheUpdater::operator()(NodeProxy aNode)
{
   const bool oldVisibility = aNode.GetCachedRow() >= 0;
   const bool newVisibility = aNode.IsDisplayed();

   if (oldVisibility != newVisibility)
   {
      if (newVisibility)
      {
         InsertRow(aNode);
      }
      else
      {
         RemoveRow(aNode);
      }
   }
   else if (newVisibility && aNode.GetCachedRow() != mRow)
   {
      if (aNode.GetCachedRow() < mRow)
      {
         MoveRowForward(aNode);
      }
      else
      {
         // implies aNode.GetCachedRow() > mRow
         MoveRowBackward(aNode);
      }
   }

   if (newVisibility)
   {
      aNode.GetPointer()->mRow = mRow;
      mRow++;
   }
   else
   {
      aNode.GetPointer()->mRow = -1;
   }
}

void wkf::bm::ModelBase::CacheUpdater::InsertRow(NodeProxy aNode)
{
   auto it = mModel.mRows.begin() + mRow;

   emit mModel.beginInsertRows(QModelIndex(), mRow, mRow);
   it = mModel.mRows.insert(it, aNode.GetPointer());
   UpdateNodeRows(it, mModel.mRows.end());
   emit mModel.endInsertRows();
}

void wkf::bm::ModelBase::CacheUpdater::RemoveRow(NodeProxy aNode)
{
   auto it = mModel.mRows.begin() + mRow;

   emit mModel.beginRemoveRows(QModelIndex(), mRow, mRow);
   it = mModel.mRows.erase(it);
   UpdateNodeRows(it, mModel.mRows.end());
   emit mModel.endRemoveRows();
}

void wkf::bm::ModelBase::CacheUpdater::MoveRowForward(NodeProxy aNode)
{
   const auto beginIt = mModel.mRows.begin();
   const auto startIt = beginIt + aNode.GetCachedRow();
   const auto stopIt  = beginIt + mRow;

   emit mModel.beginMoveRows(QModelIndex(), aNode.GetCachedRow(), aNode.GetCachedRow(), QModelIndex(), mRow);

   auto copyOfFirst = *startIt;
   std::move(startIt + 1, stopIt + 1, startIt);
   *stopIt = copyOfFirst;
   UpdateNodeRows(startIt, stopIt + 1);

   emit mModel.endMoveRows();
}

void wkf::bm::ModelBase::CacheUpdater::MoveRowBackward(NodeProxy aNode)
{
   const auto beginIt = mModel.mRows.begin();
   const auto startIt = beginIt + mRow;
   const auto stopIt  = beginIt + aNode.GetCachedRow();

   emit mModel.beginMoveRows(QModelIndex(), aNode.GetCachedRow(), aNode.GetCachedRow(), QModelIndex(), mRow);

   auto copyOfLast = *stopIt;
   std::move_backward(startIt, stopIt, stopIt + 1);
   *startIt = copyOfLast;
   UpdateNodeRows(startIt, stopIt + 1);

   emit mModel.endMoveRows();
}

void wkf::bm::ModelBase::CacheUpdater::UpdateNodeRows(iterator aBegin, iterator aEnd)
{
   for (auto it = aBegin; it != aEnd; it++)
   {
      (*it)->mRow = static_cast<int>(it - mModel.mRows.begin());
   }
}

// ========================================= //
//            wkf::bm::ModelBase             //
// ========================================= //

wkf::bm::ModelBase::ModelBase(QStringList aFixedColumns, bool aSpanningColumnZero)
   : QAbstractTableModel()
   , mRootNode(nullptr, QString{})
   , mFixedColumns(std::move(aFixedColumns))
   , mSpanningColumnZero(aSpanningColumnZero)
   , mDefaultExpanding(true)
{
   if (mFixedColumns.size() <= static_cast<int>(aSpanningColumnZero))
   {
      throw std::logic_error("Not enough fixed columns.");
   }
}

void wkf::bm::ModelBase::ConnectToTableView(QTableView* aView)
{
   if (aView)
   {
      aView->setModel(this);
      connect(aView, &QTableView::doubleClicked, this, &ModelBase::DoubleClicked);
   }
}

int wkf::bm::ModelBase::rowCount(const QModelIndex& aParent) const
{
   if (aParent.isValid())
   {
      return 0;
   }
   return static_cast<int>(mRows.size());
}

int wkf::bm::ModelBase::columnCount(const QModelIndex& aParent) const
{
   if (aParent.isValid())
   {
      return 0;
   }
   return FixedColumnDisplayCount() + static_cast<int>(mDataNames.size());
}

QVariant wkf::bm::ModelBase::data(const QModelIndex& aIndex, int aRole) const
{
   if (!aIndex.isValid() || aIndex.row() >= rowCount() || aIndex.column() >= columnCount())
   {
      return QVariant();
   }

   switch (aRole)
   {
   case Qt::DisplayRole:
      return DisplayData(aIndex);
   case Qt::ToolTipRole:
      return ToolTipData(aIndex);
   case Qt::BackgroundRole:
      return ColorData(aIndex);
   case Qt::ForegroundRole:
      return TextColorData(aIndex);
   case Qt::FontRole:
      // Fixed columns have bold font.
      // Data columns have default font.
      if (aIndex.column() < FixedColumnDisplayCount())
      {
         QFont font;
         font.setBold(true);
         return font;
      }
      break;
   }
   return QVariant();
}

QVariant wkf::bm::ModelBase::headerData(int aSection, Qt::Orientation aOrientation, int aRole) const
{
   if (aOrientation == Qt::Orientation::Horizontal)
   {
      switch (aRole)
      {
      case Qt::DisplayRole:
         if (aSection < FixedColumnDisplayCount())
         {
            return mFixedColumns[aSection + mSpanningColumnZero];
         }
         else
         {
            // Guaranteed to be non-negative.
            int dataIndex = aSection - FixedColumnDisplayCount();
            if (dataIndex < mDataNames.size())
            {
               return mDataNames[dataIndex];
            }
         }

         break;
      case Qt::ToolTipRole:
         // TODO: What to do for tool tips?
         break;
      }
   }
   return QVariant();
}

void wkf::bm::ModelBase::sort(int aColumn, Qt::SortOrder aOrder)
{
   mSortingColumn = aColumn;
   mSortAscending = (aOrder == Qt::AscendingOrder);

   UpdateCache();
}

void wkf::bm::ModelBase::UpdateCache()
{
   ForEachNode([this](NodeProxy aNode) { SortNodeChildren(aNode); }, false, true);
   ForEachNode(CacheUpdater{*this}, false);
}

void wkf::bm::ModelBase::Clear()
{
   emit beginResetModel();
   mRootNode.mChildren.clear();
   mRootNode.mData.clear();
   mRootNode.mVisibility = detail::Visibility{};
   mRootNode.mColor.reset();
   mRows.clear();
   emit endResetModel();
}

void wkf::bm::ModelBase::SetHiddenNodes(const Configuration& aConfiguration)
{
   // "black_list" and "white_list" are effectively a QList<QStringList>, where each QStringList is a node index.

   mWhiteListNodes = aConfiguration.mWhiteList;
   mBlackListNodes = aConfiguration.mBlackList;

   ForEachNode(
      [this](NodeProxy aNode)
      {
         aNode.SetHiddenByUser(!InitialVisibility(aNode));
         aNode.SetExpanded(true);
      },
      false);
   UpdateCache();
}

void wkf::bm::ModelBase::GetHiddenNodes(Configuration& aConfiguration) const
{
   aConfiguration.mWhiteList = mWhiteListNodes;
   aConfiguration.mBlackList = mBlackListNodes;
}

wkf::bm::NodeProxy wkf::bm::ModelBase::GetInvisibleRootNode(bool aConnected) noexcept
{
   return NodeProxy{&mRootNode, aConnected ? this : nullptr};
}

wkf::bm::NodeProxy wkf::bm::ModelBase::GetNode(const QStringList& aLeafIndex, bool aConnected)
{
   // Check for valid leaf index
   if (aLeafIndex.size() > mFixedColumns.size() || aLeafIndex.empty())
   {
      return NodeProxy{};
   }

   return NodeProxy{GetNodeHelper(aLeafIndex, mRootNode), aConnected ? this : nullptr};
}

wkf::bm::NodeProxy wkf::bm::ModelBase::GetOrAddNode(const QStringList& aLeafIndex, bool aConnected)
{
   // Check for valid leaf index
   if (aLeafIndex.size() > mFixedColumns.size() || aLeafIndex.empty())
   {
      return NodeProxy{};
   }

   return NodeProxy{GetOrAddNodeHelper(aLeafIndex, mRootNode), aConnected ? this : nullptr};
}

wkf::bm::LeafProxy wkf::bm::ModelBase::GetLeaf(const QStringList& aLeafIndex, bool aConnected)
{
   // Check for valid leaf index
   if (aLeafIndex.size() != mFixedColumns.size())
   {
      return LeafProxy{};
   }

   return LeafProxy{GetNodeHelper(aLeafIndex, mRootNode), aConnected ? this : nullptr, mDataNames};
}

wkf::bm::LeafProxy wkf::bm::ModelBase::GetOrAddLeaf(const QStringList& aLeafIndex, bool aConnected)
{
   // Check for valid leaf index
   if (aLeafIndex.size() != mFixedColumns.size())
   {
      return LeafProxy{};
   }

   return LeafProxy{GetOrAddNodeHelper(aLeafIndex, mRootNode), aConnected ? this : nullptr, mDataNames};
}

wkf::bm::NodeProxy wkf::bm::ModelBase::GetNodeAt(int aRow, bool aConnected /* = true */)
{
   if (aRow < 0 || aRow >= rowCount())
   {
      return NodeProxy{};
   }

   return NodeProxy{mRows[aRow], aConnected ? this : nullptr};
}

wkf::bm::LeafProxy wkf::bm::ModelBase::GetLeafAt(int aRow, bool aConnected /* = true */)
{
   if (aRow >= 0 && aRow < rowCount())
   {
      auto node = mRows[aRow];

      if (node->mColumn == mFixedColumns.size() - 1)
      {
         return LeafProxy{node, aConnected ? this : nullptr, mDataNames};
      }
   }

   return LeafProxy{};
}

void wkf::bm::ModelBase::AddDataColumns(const QStringList& aTitles)
{
   if (!aTitles.empty())
   {
      const int startIndex = FixedColumnDisplayCount() + mDataNames.size();
      emit      beginInsertColumns(QModelIndex{}, startIndex, startIndex + aTitles.size() - 1);
      mDataNames.append(aTitles);
      const int count = aTitles.size();
      ForEachLeaf(
         [this, count](LeafProxy aLeaf)
         {
            auto& data = aLeaf.GetPointer()->mData;
            data.insert(data.end(), count, detail::DataNode{});
         },
         false);
      emit endInsertColumns();
   }
}

bool wkf::bm::ModelBase::RemoveDataColumn(int aDataIndex)
{
   if (aDataIndex < 0 || aDataIndex >= mDataNames.size())
   {
      return false;
   }

   if (aDataIndex + FixedColumnDisplayCount() < mSortingColumn)
   {
      mSortingColumn--;
   }
   else if (aDataIndex + FixedColumnDisplayCount() == mSortingColumn)
   {
      mSortingColumn = 0;
   }

   emit beginRemoveColumns(QModelIndex{}, aDataIndex + FixedColumnDisplayCount(), aDataIndex + FixedColumnDisplayCount());
   mDataNames.removeAt(aDataIndex);
   ForEachLeaf(
      [this, aDataIndex](LeafProxy aLeaf)
      {
         auto& data = aLeaf.GetPointer()->mData;
         data.erase(data.begin() + aDataIndex);
      },
      false);

   emit endRemoveColumns();

   return true;
}

void wkf::bm::ModelBase::DoubleClicked(const QModelIndex& aIndex)
{
   if (!aIndex.isValid() || aIndex.row() >= rowCount() || aIndex.column() >= columnCount())
   {
      return;
   }

   NodeProxy node{mRows[aIndex.row()], this};

   if (node.GetColumn() == 0 && mSpanningColumnZero)
   {
      node.ToggleExpanded();
   }
   else if (aIndex.column() < FixedColumnDisplayCount())
   {
      if (aIndex.column() == node.GetColumn() - mSpanningColumnZero)
      {
         node.ToggleExpanded();
      }
   }
   else
   {
      DataDoubleClicked(aIndex.column() - FixedColumnDisplayCount());
   }
}

// virtual
QList<QAction*> wkf::bm::ModelBase::MenuActions(const QModelIndexList& aSelection)
{
   QList<QAction*> retval;
   retval << ExpandAllAction() << CollapseAllAction();

   for (const auto& index : aSelection)
   {
      const auto& node = *mRows[index.row()];
      if ((node.mColumn == 0 && mSpanningColumnZero) || (index.column() == node.mColumn - mSpanningColumnZero))
      {
         retval << HideAction(aSelection) << HideAllExceptAction(aSelection);
         break;
      }
   }

   retval << ShowAllAction();

   return retval;
}

void wkf::bm::ModelBase::ExpandAll()
{
   ForEachNode([](NodeProxy aNode) { aNode.SetExpanded(true); }, false);
   mDefaultExpanding = true;
   UpdateCache();
}

void wkf::bm::ModelBase::CollapseAll()
{
   ForEachNode([](NodeProxy aNode) { aNode.SetExpanded(false); }, false);
   mDefaultExpanding = false;
   UpdateCache();
}

void wkf::bm::ModelBase::Hide(const QModelIndexList& aSelection)
{
   for (const auto& index : aSelection)
   {
      NodeProxy node{mRows[index.row()], nullptr};
      node.SetHiddenByUser(true);
      mBlackListNodes += node.GetNodeIndex();
   }

   UpdateCache();
}

void wkf::bm::ModelBase::HideAllExcept(const QModelIndexList& aSelection)
{
   HideAllExceptHelper(mRootNode, aSelection);
   UpdateCache();
}

void wkf::bm::ModelBase::ShowAll()
{
   ForEachNode([](NodeProxy aNode) { aNode.SetHiddenByUser(false); }, false);
   mWhiteListNodes.clear();
   mBlackListNodes.clear();
   UpdateCache();
}

int wkf::bm::ModelBase::FixedColumnDisplayCount() const noexcept
{
   return mFixedColumns.size() - mSpanningColumnZero;
}

bool wkf::bm::ModelBase::IsSpanningColumnZero() const noexcept
{
   return mSpanningColumnZero;
}

void wkf::bm::ModelBase::EmitFixedColumnColorChanged(int aRow, int aColumn)
{
   if (!IsSpanningColumnZero())
   {
      auto idx = index(aRow, aColumn);
      emit dataChanged(idx, idx, {Qt::BackgroundRole});
   }
   else if (aColumn == 0)
   {
      auto leftIndex  = index(aRow, 0);
      auto rightIndex = index(aRow, columnCount());
      emit dataChanged(leftIndex, rightIndex, {Qt::BackgroundRole});
   }
   else
   {
      auto idx = index(aRow, aColumn - 1);
      emit dataChanged(idx, idx, {Qt::BackgroundRole});
   }
}

// protected
const QList<QStringList>& wkf::bm::ModelBase::GetWhiteListedNodes() const noexcept
{
   return mWhiteListNodes;
}

// protected
const QList<QStringList>& wkf::bm::ModelBase::GetBlackListedNodes() const noexcept
{
   return mBlackListNodes;
}

// private
QAction* wkf::bm::ModelBase::ExpandAllAction()
{
   auto act = ut::qt::make_qt_ptr<QAction>("Expand All");

   connect(act, &QAction::triggered, this, &ModelBase::ExpandAll);

   return act;
}

// private
QAction* wkf::bm::ModelBase::CollapseAllAction()
{
   auto act = ut::qt::make_qt_ptr<QAction>("Collapse All");

   connect(act, &QAction::triggered, this, &ModelBase::CollapseAll);

   return act;
}

// private
QAction* wkf::bm::ModelBase::HideAction(const QModelIndexList& aSelection)
{
   auto act = ut::qt::make_qt_ptr<QAction>("Hide");

   connect(act, &QAction::triggered, [this, aSelection]() { Hide(aSelection); });

   return act;
}

// private
QAction* wkf::bm::ModelBase::HideAllExceptAction(const QModelIndexList& aSelection)
{
   auto act = ut::qt::make_qt_ptr<QAction>("Hide All Except");

   connect(act, &QAction::triggered, [this, aSelection]() { HideAllExcept(aSelection); });

   return act;
}

// private
QAction* wkf::bm::ModelBase::ShowAllAction()
{
   auto act = ut::qt::make_qt_ptr<QAction>("Show All");

   connect(act, &QAction::triggered, this, &ModelBase::ShowAll);

   return act;
}

// private
QVariant wkf::bm::ModelBase::DisplayData(const QModelIndex& aIndex) const
{
   const detail::Node& node = *mRows[aIndex.row()];

   // Handle fixed columns
   // max() handles spanning column zero
   if (aIndex.column() == std::max(node.mColumn - mSpanningColumnZero, 0))
   {
      return mRows[aIndex.row()]->mTitle;
   }
   // Handle data columns
   if (aIndex.column() >= FixedColumnDisplayCount())
   {
      int dataIndex = aIndex.column() - FixedColumnDisplayCount();

      if (dataIndex < ut::safe_cast<int>(node.mData.size()))
      {
         return node.mData[dataIndex].mValue;
      }
   }

   return QVariant();
}

// private
QVariant wkf::bm::ModelBase::ToolTipData(const QModelIndex& aIndex) const
{
   auto* node = mRows[aIndex.row()];

   if (mSpanningColumnZero && node->mColumn == 0)
   {
      return mFixedColumns[0] + ": " + node->mTitle;
   }

   QString retval;

   const bool onFixedText = aIndex.column() == std::max(node->mColumn - mSpanningColumnZero, 0);
   const bool onData      = aIndex.column() >= FixedColumnDisplayCount() && node->mColumn == mFixedColumns.size() - 1;

   if (onFixedText || onData)
   {
      QStringList nodeIndex = node->GetNodeIndex();

      for (int i = 0; i < nodeIndex.size(); i++)
      {
         if (i > 0)
         {
            retval += "\n";
         }
         retval += mFixedColumns[i] + ": " + nodeIndex[i];
      }

      if (onData)
      {
         int dataIndex = aIndex.column() - FixedColumnDisplayCount();
         if (dataIndex < static_cast<int>(node->mData.size()))
         {
            retval += "\n" + mDataNames[dataIndex] + ": " + node->mData[dataIndex].mValue.toString();
         }
      }
   }
   return retval;
}

// private
QVariant wkf::bm::ModelBase::ColorData(const QModelIndex& aIndex) const
{
   const detail::Node& node = *mRows[aIndex.row()];

   // Handle spanning column zero
   if (mSpanningColumnZero && node.mColumn == 0)
   {
      return ColorSettingToVariant(node.mColor);
   }
   // Handle fixed columns
   else if (aIndex.column() < FixedColumnDisplayCount())
   {
      if (aIndex.column() == node.mColumn - mSpanningColumnZero)
      {
         return ColorSettingToVariant(node.mColor);
      }
   }
   // Handle data columns
   else
   {
      // Guaranteed to be non-negative
      int dataIndex = aIndex.column() - FixedColumnDisplayCount();

      if (dataIndex < static_cast<int>(node.mData.size()))
      {
         // mColor is an optional that needs to be validated first
         return ColorSettingToVariant(node.mData[dataIndex].mColor);
      }
   }
   return QVariant();
}

// private
QVariant wkf::bm::ModelBase::TextColorData(const QModelIndex& aIndex) const
{
   auto background = ColorData(aIndex);
   if (background.isValid())
   {
      QColor color     = background.value<QColor>();
      int    lightness = color.red() + color.green() + color.blue();
      if (lightness < 256)
      {
         return QColor{255, 255, 255};
      }
      else if (lightness > 448)
      {
         return QColor{0, 0, 0};
      }
   }
   return QVariant{};
}

// private
void wkf::bm::ModelBase::SortNodeChildren(NodeProxy aNode)
{
   if (mSortingColumn < 0) // If sorting is disabled
   {
      return;
   }
   else if (mSortingColumn < FixedColumnDisplayCount()) // If sorting one of the fixed columns
   {
      if (aNode.GetColumn() - mSpanningColumnZero == mSortingColumn - 1) // If the node is in the sorting column
      {
         auto sorter = [this](const detail::Node& aLeft, const detail::Node& aRight) noexcept
         {
            if (mSortAscending)
            {
               return aLeft.mTitle < aRight.mTitle;
            }
            else
            {
               return aLeft.mTitle > aRight.mTitle;
            }
         };

         aNode.GetPointer()->mChildren.sort(sorter);
      }
   }
   else if (aNode.GetColumn() == mFixedColumns.size() - 2) // (else) If sorting one of the data columns,
                                                           // and the node is in the last fixed column
   {
      auto sorter = [this](const detail::Node& aLeft, const detail::Node& aRight) noexcept
      {
         int         dataIndex = mSortingColumn - FixedColumnDisplayCount();
         const auto& left      = aLeft.mData[dataIndex].mValue;
         const auto& right     = aRight.mData[dataIndex].mValue;
         if (mSortAscending)
         {
            return left < right;
         }
         else
         {
            return right < left;
         }
      };

      aNode.GetPointer()->mChildren.sort(sorter);
   }
}

// private
wkf::bm::detail::Node* wkf::bm::ModelBase::GetNodeHelper(const QStringList& aNodeIndex, detail::Node& aNode)
{
   // Base case
   if (aNode.mColumn == aNodeIndex.size() - 1)
   {
      return &aNode;
   }

   // Search for next terms
   QString index = aNodeIndex[aNode.mColumn + 1];
   for (auto& child : aNode.mChildren)
   {
      if (child.mTitle == index)
      {
         return GetNodeHelper(aNodeIndex, child);
      }
   }

   // Return null if not found
   return nullptr;
}

// private
wkf::bm::detail::Node* wkf::bm::ModelBase::GetOrAddNodeHelper(const QStringList& aNodeIndex, detail::Node& aNode)
{
   // Base case
   if (aNode.mColumn == aNodeIndex.size() - 1)
   {
      return &aNode;
   }

   // Search for next terms
   QString index = aNodeIndex[aNode.mColumn + 1];
   for (auto& child : aNode.mChildren)
   {
      if (child.mTitle == index)
      {
         return GetOrAddNodeHelper(aNodeIndex, child);
      }
   }

   // Create new node if not found
   if (MakeNewNode(aNodeIndex[aNode.mColumn + 1], &aNode))
   {
      // Call again with the same args
      return GetOrAddNodeHelper(aNodeIndex, aNode);
   }
   return nullptr;
}

// private
bool wkf::bm::ModelBase::HideAllExceptHelper(detail::Node& aNode, const QModelIndexList& aSelection)
{
   for (const auto& index : aSelection)
   {
      if (&aNode == mRows[index.row()])
      {
         mWhiteListNodes += aNode.GetNodeIndex();
         return true;
      }
   }

   bool found = false;
   for (auto& child : aNode.mChildren)
   {
      found |= HideAllExceptHelper(child, aSelection);
   }

   if (!found)
   {
      aNode.mVisibility.SetHiddenByUser(true);
      // mBlackListNodes += aNode.GetNodeIndex();
   }

   return found;
}

// private
bool wkf::bm::ModelBase::MakeNewNode(const QString& aTitle, detail::Node* aParent)
{
   if (aParent)
   {
      detail::Node node{aParent, aTitle};
      NodeProxy    proxy{&node, nullptr};

      proxy.SetExpanded(mDefaultExpanding);
      proxy.SetHiddenByUser(!InitialVisibility(proxy));
      InitializeNode(proxy);

      // Check if new node is a leaf
      if (node.mColumn == mFixedColumns.size() - 1)
      {
         node.mData.resize(mDataNames.size());
         InitializeLeaf(LeafProxy{&node, nullptr, mDataNames});
      }
      aParent->mChildren.emplace_back(std::move(node));
      UpdateCache();

      return true;
   }

   return false;
}

// private
bool wkf::bm::ModelBase::InitialVisibility(const NodeProxy aNode) const
{
   if (mWhiteListNodes.empty())
   {
      return !mBlackListNodes.contains(aNode.GetNodeIndex());
   }
   else
   {
      auto newIndex = aNode.GetNodeIndex();
      for (const auto& index : mWhiteListNodes)
      {
         if (ut::ranges::starts_with(newIndex, index) || ut::ranges::starts_with(index, newIndex))
         {
            return !mBlackListNodes.contains(aNode.GetNodeIndex());
         }
      }

      return false;
   }
}
