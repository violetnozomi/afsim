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

#ifndef EVENTTABLEDIALOG_HPP
#define EVENTTABLEDIALOG_HPP

#include <QDialog>

class     QAbstractItemModel;
class     QListWidget;
class     QListWidgetItem;
#include <QSortFilterProxyModel>
class     QTableView;

class EventFilterModel : public QSortFilterProxyModel
{
public:
   EventFilterModel(QObject*        aParent,
                    int             aColumn,
                    int             aType,
                    const QVariant& aValue);
   ~EventFilterModel() override = default;
   bool filterAcceptsRow(int                source_row,
                         const QModelIndex& source_parent) const override;

   void SetTypeFiltered(bool aState) { mTypeFiltered = aState; }

   QVariant headerData(int             section,
                       Qt::Orientation orientation,
                       int             role) const override;

   void SetComparatorAndValue(int             aComp,
                              const QVariant& aValue) { mType = aComp, mValue = aValue; invalidateFilter(); }
   int GetColumn() const { return mColumn; }
   int GetType() const { return mType; }
   QVariant GetValue() const { return mValue; }
private:

   bool Compare(const QVariant& aLeft,
                int             aCompType,
                const QVariant& aRight) const;
   int mColumn;
   int mType;
   QVariant mValue;
   bool mTypeFiltered;
};

class EventTableDialog : public QWidget
{
public:
   EventTableDialog(QWidget* aParentPtr);
   ~EventTableDialog() override = default;

   void SetModel(QAbstractItemModel* aModelPtr);
   void LoadFilter();
   void LoadFilter(const QString& aFilename);

   enum
   {
      FilterTypeUnset,
      FilterTypeEqual,
      FilterTypeNotEqual,
      FilterTypeGreater,
      FilterTypeLess,
      FilterTypeGreaterOrEqual,
      FilterTypeLessOrEqual,
      FilterTypeContains,
      FilterTypeSide,
      FilterTypeType,
      FilterTypeCategory
   };
private:

   void TableItemContextMenu(const QPoint& aPoint);
   void RuleContextMenu(const QPoint& aPoint);
   void CreateFilter(const QVariant& aData,
                     int             aColumn,
                     int             aFilterType = FilterTypeUnset);
   void ClearRules();
   void RemoveRule(QListWidgetItem* aItem);
   void EditRule(QListWidgetItem* aItem);
   void AddFilterRule(QListWidgetItem* aItem);
   void Export();
   void SaveFilter();

   QSize sizeHint() const override;
   QDialog::DialogCode GetFilter(const int aColumn,
                                 int&      aFilterType,
                                 QVariant& aValue);
   QString GetRuleText(const int       aColumn,
                       const int       aFilterType,
                       const QVariant& aValue);

   QString GetTypeText(int aFilterType);

   QAbstractItemModel* mSourceModelPtr;
   std::map<int, EventFilterModel*> mFilterModels;
   QListWidget* mRuleWidgetPtr;
   QTableView* mTablePtr;
   unsigned int mIdManager;
};

#endif
