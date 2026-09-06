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

#include "WkfFilterView.hpp"

#include <QDialog>
#include <QDialogButtonBox>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>

#include "WkfFilterRules.hpp"

namespace WkfFilter
{
FilterView::FilterView(QWidget* aParent)
   : QWidget(aParent)
{
   MakeRootItem();
   setMouseTracking(true);
}

void FilterView::SetModel(const QAbstractItemModel* aModel)
{
   mModel = aModel;
}

void FilterView::SetupExports(wkf::ExportHelper* aExportHelper, FilterPrefObject* aPrefObject)
{
   if (mExportHelper)
   {
      disconnect(mExportHelper);
   }

   mExportHelper = aExportHelper;
   mPrefObject   = aPrefObject;

   if (mExportHelper && mPrefObject)
   {
      connect(mExportHelper,
              &wkf::ExportHelper::WriteState,
              [this](QString aName) { mPrefObject->Export(aName, SaveFilter(*ItemToFilter(mContextMenuItem))); });

      connect(mExportHelper,
              &wkf::ExportHelper::ReadState,
              [this](QString aName) { ImportFilter(aName, ItemToFilter(mContextMenuItem)); });
   }
}

FilterPtr FilterView::GetFilter() const
{
   return mFilter->Clone();
}

bool FilterView::UpdateAllItems()
{
   bool changed = false;

   for (auto pairing : mDataGuiPairings)
   {
      UpdateItem(pairing.first, pairing.second, &changed);
   }

   if (changed)
   {
      emit FilterChanged();
   }

   return changed;
}

void FilterView::AddConditionToRoot(int aColumn, const QString& aValue)
{
   auto* rootFilter = mFilter.get();
   auto* newItem    = FilterToItem(rootFilter)->MakeChild();
   ConnectItem(newItem);
   auto& newFilter = rootFilter->AddChild(GetRule("=="));

   newFilter.mColumn = aColumn;
   newFilter.mValue  = aValue;

   mDataGuiPairings.emplace_back(&newFilter, newItem);
   UpdateItem(&newFilter, newItem);

   if (rootFilter->IsValid())
   {
      emit FilterChanged();
   }
}

void FilterView::TypeChanged(FilterItem* aItem, FilterObject::Type aType)
{
   auto* filter  = ItemToFilter(aItem);
   filter->mType = aType;
   if (filter->IsReachable())
   {
      emit FilterChanged();
   }
}

void FilterView::EnableChanged(FilterItem* aItem, bool aEnable)
{
   auto* filter          = ItemToFilter(aItem);
   bool  reachableBefore = filter->IsReachable();

   filter->mEnabled = aEnable;

   if (reachableBefore || filter->IsReachable())
   {
      emit FilterChanged();
   }
}

void FilterView::InvertChanged(FilterItem* aItem, bool aInvert)
{
   auto* filter      = ItemToFilter(aItem);
   filter->mInverted = aInvert;
   if (filter->IsReachable())
   {
      emit FilterChanged();
   }
}

void FilterView::ColumnChanged(FilterItem* aItem, const QString& aName)
{
   auto* filter      = ItemToFilter(aItem);
   int   columnIndex = ColumnNameToIndex(aName);

   if (filter->mColumn != columnIndex)
   {
      bool reachableBefore = filter->IsReachable();
      filter->mColumn      = columnIndex;
      aItem->SetColumn(aName, filter->mColumn);
      if (reachableBefore || filter->IsReachable())
      {
         emit FilterChanged();
      }
   }
}

void FilterView::TestChanged(FilterItem* aItem, const QString& aTest)
{
   auto* filter = ItemToFilter(aItem);
   if (filter->mRule->Name() != aTest)
   {
      filter->mRule = GetRule(aTest);
      if (filter->IsReachable())
      {
         emit FilterChanged();
      }
   }
}

void FilterView::ValueChanged(FilterItem* aItem, const QString& aValue)
{
   auto* filter = ItemToFilter(aItem);
   if (filter->mValue != aValue)
   {
      filter->mValue = aValue;
      if (filter->IsReachable())
      {
         emit FilterChanged();
      }
   }
}

void FilterView::AddClicked(FilterItem* aItem)
{
   auto* newItem = aItem->MakeChild();
   ConnectItem(newItem);
   auto& newFilter = ItemToFilter(aItem)->AddChild(GetRule("=="));
   mDataGuiPairings.emplace_back(&newFilter, newItem);
   UpdateItem(&newFilter, newItem);

   // No need to emit FilterChanged() because new filters created via the Add button will never be valid.
}

void FilterView::RemoveClicked(FilterItem* aItem)
{
   bool reachable = RemoveFilter(ItemToFilter(aItem));

   if (reachable)
   {
      emit FilterChanged();
   }
}

void FilterView::ContextMenu(FilterItem* aItem, const QPoint& aPosition)
{
   QMenu         menu;
   FilterObject* filter = ItemToFilter(aItem);

   ContextMenuCopy(aItem, menu);
   ContextMenuCut(aItem, menu);
   ContextMenuPaste(aItem, menu);

   menu.addSeparator();

   if (mExportHelper)
   {
      mContextMenuItem = aItem;
      mExportHelper->BuildContextMenu(menu);
      menu.addSeparator();
   }

   if (filter->IsCondition())
   {
      ContextMenuRemove(aItem, menu);
   }
   else
   {
      ContextMenuAddChild(aItem, menu);
      ContextMenuRemove(aItem, menu);
      ContextMenuRemoveChildren(aItem, menu);
   }

   menu.exec(aItem->mapToGlobal(aPosition));
}

void FilterView::MakeRootItem()
{
   auto* newLayout = new QVBoxLayout();
   auto* rootItem  = new FilterItem(true);

   newLayout->addWidget(rootItem);
   setLayout(newLayout);

   ConnectItem(rootItem);
   mFilter        = ut::make_unique<FilterObject>(nullptr);
   mFilter->mType = FilterObject::Type::And;

   mDataGuiPairings.emplace_back(mFilter.get(), rootItem);

   newLayout->setSizeConstraint(QLayout::SetMinimumSize);
}

void FilterView::ConnectItem(FilterItem* aItem)
{
   connect(aItem, &FilterItem::TypeChanged, this, &FilterView::TypeChanged);
   connect(aItem, &FilterItem::EnableChanged, this, &FilterView::EnableChanged);
   connect(aItem, &FilterItem::InvertChanged, this, &FilterView::InvertChanged);
   connect(aItem, &FilterItem::ColumnChanged, this, &FilterView::ColumnChanged);
   connect(aItem, &FilterItem::TestChanged, this, &FilterView::TestChanged);
   connect(aItem, &FilterItem::ValueChanged, this, &FilterView::ValueChanged);
   connect(aItem, &FilterItem::AddClicked, this, &FilterView::AddClicked);
   connect(aItem, &FilterItem::RemoveClicked, this, &FilterView::RemoveClicked);

   connect(aItem, &FilterItem::ContextMenu, this, &FilterView::ContextMenu);
}

void FilterView::UpdateItem(FilterObject* aFilter, FilterItem* aItem, bool* aChanged)
{
   aItem->SetType(aFilter->mType);
   aItem->SetEnabled(aFilter->mEnabled);
   aItem->SetInverted(aFilter->mInverted);

   aItem->SetColumn(ColumnIndexToName(aFilter->mColumn), aFilter->mColumn);
   aItem->SetValue(aFilter->mValue);

   QStringList list = GetRuleList(aFilter->mColumn);
   aItem->SetRules(list, aFilter->GetName());

   if ((aChanged != nullptr) && (aFilter->IsCondition()) && !list.contains(aFilter->GetName()))
   {
      *aChanged      = true;
      aFilter->mRule = GetRule(list.first());
   }
}

FilterExport FilterView::SaveFilter(const FilterObject& aFilter) const
{
   FilterExport retval;

   retval.mType     = aFilter.mType;
   retval.mEnabled  = aFilter.mEnabled;
   retval.mInverted = aFilter.mInverted;
   retval.mRule     = aFilter.GetName();
   retval.mColumn   = aFilter.mColumn;
   retval.mValue    = aFilter.mValue;

   for (int i = 0; i < aFilter.ChildCount(); i++)
   {
      retval.mChildren.emplace_back(SaveFilter(*aFilter.GetChild(i)));
   }

   return retval;
}

void FilterView::LoadFilter(const FilterExport& aExport, FilterObject& aFilter)
{
   aFilter.mType     = aExport.mType;
   aFilter.mEnabled  = aExport.mEnabled;
   aFilter.mInverted = aExport.mInverted;
   aFilter.mRule     = GetRule(aExport.mRule);
   aFilter.mColumn   = aExport.mColumn;
   aFilter.mValue    = aExport.mValue;

   const int count = static_cast<int>(aExport.mChildren.size());
   for (int i = 0; i < count; i++)
   {
      LoadFilter(aExport.mChildren[i], aFilter.AddChild(nullptr));
   }
}

FilterItem* FilterView::FilterToItem(const FilterObject* aFilter) noexcept
{
   for (const auto& pairing : mDataGuiPairings)
   {
      if (pairing.first == aFilter)
      {
         return pairing.second;
      }
   }
   return nullptr;
}

const FilterItem* FilterView::FilterToItem(const FilterObject* aFilter) const noexcept
{
   for (const auto& pairing : mDataGuiPairings)
   {
      if (pairing.first == aFilter)
      {
         return pairing.second;
      }
   }
   return nullptr;
}

FilterObject* FilterView::ItemToFilter(const FilterItem* aItem) noexcept
{
   for (const auto& pairing : mDataGuiPairings)
   {
      if (pairing.second == aItem)
      {
         return pairing.first;
      }
   }
   return nullptr;
}

const FilterObject* FilterView::ItemToFilter(const FilterItem* aItem) const noexcept
{
   for (const auto& pairing : mDataGuiPairings)
   {
      if (pairing.second == aItem)
      {
         return pairing.first;
      }
   }
   return nullptr;
}

bool FilterView::IsReachable(const FilterItem* aItem) const noexcept
{
   auto* filter = ItemToFilter(aItem);

   if (filter != nullptr)
   {
      return filter->IsReachable();
   }
   return false;
}

FilterObject* FilterView::GetMutableParent(FilterObject* aObject) noexcept
{
   // FilterObjects should not be allowed to modify their parents, so they contain a const pointer.
   // The FitlerView has access to non-const pointers to all of its FilterObjects anyways, so this
   // const_cast is not providing access to any "restricted" methods or data.
   return aObject ? const_cast<FilterObject*>(aObject->GetParent()) : nullptr;
}

int FilterView::ColumnNameToIndex(const QString& aName) const
{
   if (mModel == nullptr)
   {
      return -1;
   }

   for (int i = 0; i < mModel->columnCount(); i++)
   {
      if (mModel->headerData(i, Qt::Horizontal) == aName)
      {
         return i;
      }
   }

   return -1;
}

QString FilterView::ColumnIndexToName(int aIndex) const
{
   if (mModel == nullptr || aIndex < 0)
   {
      return QString();
   }

   QString name = mModel->headerData(aIndex, Qt::Horizontal).toString();
   if (name.isEmpty())
   {
      return QString::number(aIndex);
   }
   return name;
}

const Rule* FilterView::GetRule(const QString& aName) const
{
   for (const auto& rule : mRules)
   {
      if (rule->Name() == aName)
      {
         return rule.get();
      }
   }

   return nullptr;
}

QStringList FilterView::GetRuleList(int aColumn) const
{
   if (mModel == nullptr)
   {
      return QStringList();
   }

   QStringList retval;

   for (const auto& rule : mRules)
   {
      if (rule->Predicate(mModel, aColumn))
      {
         retval += rule->Name();
      }
   }

   return retval;
}

void FilterView::Insert(FilterObject* aDestination, FilterPtr aSource)
{
   if (aSource != nullptr)
   {
      auto* source = aSource.get();

      if (aDestination == nullptr)
      {
         // Special case for inserting AS the root item
         ClearChildren(mFilter.get());

         mFilter                        = std::move(aSource);
         mDataGuiPairings.front().first = mFilter.get();

         for (int i = 0; i < mFilter->ChildCount(); i++)
         {
            InsertRecursionHelper(mFilter->GetChild(i));
         }
      }
      else
      {
         aDestination->InsertChild(std::move(aSource));
         InsertRecursionHelper(source);
      }
      if (!UpdateAllItems())
      {
         emit FilterChanged();
      }
   }
}

void FilterView::InsertRecursionHelper(FilterObject* aObject)
{
   // Create FilterItem.
   auto* item = FilterToItem(aObject->GetParent())->MakeChild();
   ConnectItem(item);

   // Add to pairings.
   mDataGuiPairings.emplace_back(aObject, item);

   // Apply to each child.
   for (int i = 0; i < aObject->ChildCount(); i++)
   {
      InsertRecursionHelper(aObject->GetChild(i));
   }
}

bool FilterView::RemoveFilter(FilterObject* aObject)
{
   auto* parentFilter = GetMutableParent(aObject);

   bool reachable = aObject->IsReachable();

   ClearChildren(aObject);

   FilterToItem(parentFilter)->RemoveChild(FilterToItem(aObject));

   parentFilter->RemoveChild(aObject);

   for (auto it = mDataGuiPairings.begin(); it != mDataGuiPairings.end(); it++)
   {
      if (it->first == aObject)
      {
         mDataGuiPairings.erase(it);
         break;
      }
   }

   return reachable;
}

void FilterView::ClearChildren(FilterObject* aObject)
{
   bool needsToEmit = false;

   while (aObject->ChildCount() > 0)
   {
      auto* child = aObject->GetChild(0);
      needsToEmit |= RemoveFilter(child);
   }

   if (needsToEmit)
   {
      emit FilterChanged();
   }
}

void FilterView::ContextMenuCopy(FilterItem* aItem, QMenu& aMenu)
{
   QAction* action = new QAction("Copy");
   connect(action, &QAction::triggered, [this, aItem]() { mCopiedFilter = ItemToFilter(aItem)->Clone(); });

   aMenu.addAction(action);
}

void FilterView::ContextMenuCut(FilterItem* aItem, QMenu& aMenu)
{
   QAction*      action = new QAction("Cut");
   FilterObject* filter = ItemToFilter(aItem);

   connect(action,
           &QAction::triggered,
           [this, aItem]()
           {
              mCopiedFilter = ItemToFilter(aItem)->Clone();
              RemoveClicked(aItem);
           });

   action->setEnabled(filter->GetParent() != nullptr);
   aMenu.addAction(action);
}

void FilterView::ContextMenuPaste(FilterItem* aItem, QMenu& aMenu)
{
   QAction* action = new QAction("Paste");
   connect(action,
           &QAction::triggered,
           [this, aItem]()
           {
              auto* filter = ItemToFilter(aItem);
              if (filter->IsCondition())
              {
                 // Paste as sibling
                 Insert(GetMutableParent(filter), mCopiedFilter->Clone());
              }
              else
              {
                 // Paste as child
                 Insert(filter, mCopiedFilter->Clone());
              }
           });

   action->setEnabled(mCopiedFilter != nullptr);
   aMenu.addAction(action);
}

void FilterView::ContextMenuAddChild(FilterItem* aItem, QMenu& aMenu)
{
   QAction* action = new QAction("Add Child");
   connect(action, &QAction::triggered, [this, aItem]() { AddClicked(aItem); });

   aMenu.addAction(action);
}

void FilterView::ContextMenuRemove(FilterItem* aItem, QMenu& aMenu)
{
   QAction*      action = new QAction("Remove");
   FilterObject* filter = ItemToFilter(aItem);

   connect(action, &QAction::triggered, [this, aItem]() { RemoveClicked(aItem); });

   action->setEnabled(filter->GetParent() != nullptr);
   aMenu.addAction(action);
}

void FilterView::ContextMenuRemoveChildren(FilterItem* aItem, QMenu& aMenu)
{
   QAction* action = new QAction("Remove Children");
   connect(action, &QAction::triggered, [this, aItem]() { mCopiedFilter = ItemToFilter(aItem)->Clone(); });

   aMenu.addAction(action);
}

void FilterView::ImportFilter(QString aName, FilterObject* aFilter)
{
   FilterPtr toImport = ut::make_unique<FilterObject>();
   LoadFilter(mPrefObject->GetExport(aName), *toImport);

   if (mDataGuiPairings.size() == 1 && !toImport->IsCondition())
   {
      if (QMessageBox::question(nullptr, "Replace root?", "Replace the root filter?") == QMessageBox::Yes)
      {
         // Special case for importing AS the root filter.
         Insert(nullptr, std::move(toImport));
      }
      else
      {
         // Import as child
         Insert(aFilter, std::move(toImport));
      }
   }
   else if (aFilter->IsCondition())
   {
      // Import as sibling
      Insert(GetMutableParent(aFilter), std::move(toImport));
   }
   else
   {
      // Import as child
      Insert(aFilter, std::move(toImport));
   }
}
} // namespace WkfFilter
