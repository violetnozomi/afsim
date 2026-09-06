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

#ifndef WKF_FILTER_VIEW_HPP
#define WKF_FILTER_VIEW_HPP

#include "wkf_common_export.h"

#include <QAbstractItemModel>

#include "UtMemory.hpp"
#include "WkfFilterItem.hpp"
#include "WkfFilterObject.hpp"
#include "WkfFilterPrefObject.hpp"
#include "misc/WkfExportHelper.hpp"

class QSettings;

namespace WkfFilter
{
//! A view of the filter being used.
//! Uses FilterItem's to manipulate FilterObjects.
class WKF_COMMON_EXPORT FilterView : public QWidget
{
   Q_OBJECT

public:
   explicit FilterView(QWidget* aParent);

   void SetModel(const QAbstractItemModel* aModel);
   void SetupExports(wkf::ExportHelper* aExportHelper, FilterPrefObject* aPrefObject);

   FilterPtr GetFilter() const;

   template<typename T, typename... Args>
   void RegisterRule(Args&&... aArgs)
   {
      static_assert(std::is_base_of<WkfFilter::Rule, T>::value, "Template argument must derive from WkfFilter::Rule.");
      mRules.emplace_back(ut::make_unique<T>(std::forward<Args>(aArgs)...));

      UpdateAllItems();
   }

   //! Note: May emit FilterChanged if any filters change.
   //! Returns true if FilterChanged is emitted.
   bool UpdateAllItems();

   void AddConditionToRoot(int aColumn, const QString& aValue);

signals:
   void FilterChanged();

private slots:
   void TypeChanged(FilterItem* aItem, FilterObject::Type aType);
   void EnableChanged(FilterItem* aItem, bool aEnable);
   void InvertChanged(FilterItem* aItem, bool aInvert);
   void ColumnChanged(FilterItem* aItem, const QString& aName);
   void TestChanged(FilterItem* aItem, const QString& aTest);
   void ValueChanged(FilterItem* aItem, const QString& aValue);
   void AddClicked(FilterItem* aItem);
   void RemoveClicked(FilterItem* aItem);

   void ContextMenu(FilterItem* aItem, const QPoint& aPosition);

private:
   //! Creates and connects the root filter item.
   //! Should not be called outside the constructor.
   void MakeRootItem();

   void ConnectItem(FilterItem* aItem);

   //! Updates the column index and rule list of the FilterItem.
   //! No signals will be emitted.
   //! If this method causes a filter to actually be changed, *aChanged is set to true.
   //! Note: This function will never set *aChanged to false.
   void UpdateItem(FilterObject* aFilter, FilterItem* aItem, bool* aChanged = nullptr);

   //! Loads or saves a single FilterObject hierarchy.
   FilterExport SaveFilter(const FilterObject& aFilter) const;
   void         LoadFilter(const FilterExport& aExport, FilterObject& aFilter);

   //! Return nullptr if data could not be found.
   //! @{
   FilterItem*       FilterToItem(const FilterObject* aFilter) noexcept;
   const FilterItem* FilterToItem(const FilterObject* aFilter) const noexcept;

   FilterObject*       ItemToFilter(const FilterItem* aItem) noexcept;
   const FilterObject* ItemToFilter(const FilterItem* aItem) const noexcept;
   //! @}

   //! Returns the parent of aItem as a non-const pointer.
   FilterObject* GetMutableParent(FilterObject* aItem) noexcept;

   bool IsReachable(const FilterItem* aItem) const noexcept;

   int     ColumnNameToIndex(const QString& aName) const;
   QString ColumnIndexToName(int aIndex) const;

   const Rule* GetRule(const QString& aName) const;
   QStringList GetRuleList(int aColumn) const;

   //! Inserts mSource into the tree as a child of mDestination.
   //! Also creates FilterItems for the new nodes.
   //! If aDestination is nullptr, sets mFilter to aSource and recreates the GUI.
   void Insert(FilterObject* aDestination, FilterPtr aSource);
   void InsertRecursionHelper(FilterObject* aObject);

   //! Returns true if the removed item was reachable.
   bool RemoveFilter(FilterObject* aObject);
   void ClearChildren(FilterObject* aObject);

   //! Create context-menu items.
   //! @{
   void ContextMenuCopy(FilterItem* aItem, QMenu& aMenu);
   void ContextMenuCut(FilterItem* aItem, QMenu& aMenu);
   void ContextMenuPaste(FilterItem* aItem, QMenu& aMenu);
   void ContextMenuAddChild(FilterItem* aItem, QMenu& aMenu);
   void ContextMenuRemove(FilterItem* aItem, QMenu& aMenu);
   void ContextMenuRemoveChildren(FilterItem* aItem, QMenu& aMenu);
   //! @}

   void ImportFilter(QString aName, FilterObject* aFilter);

   const QAbstractItemModel* mModel = nullptr;

   FilterPtr mFilter;
   RuleList  mRules;

   wkf::ExportHelper* mExportHelper = nullptr;
   FilterPrefObject*  mPrefObject   = nullptr;

   FilterPtr   mCopiedFilter;
   FilterItem* mContextMenuItem = nullptr;

   std::vector<std::pair<FilterObject*, FilterItem*>> mDataGuiPairings;
};
} // namespace WkfFilter

#endif
