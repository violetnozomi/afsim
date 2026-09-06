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

#include "EventTableDialog.hpp"

#include <QAbstractItemModel>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QSortFilterProxyModel>
#include <QSplitter>
#include <QStandardItemModel>
#include <QTableView>
#include <QTextStream>
#include <QVBoxLayout>

#include "EventModel.hpp"

EventTableDialog::EventTableDialog(QWidget* aParentPtr)
   : QWidget(aParentPtr)
   , mIdManager(0)
   , mSourceModelPtr(nullptr)
{
   setLayout(new QVBoxLayout(this));
   QSplitter* splitter = new QSplitter(this);
   splitter->setOrientation(Qt::Vertical);
   setWindowTitle("Events");

   mRuleWidgetPtr = new QListWidget(this);
   mRuleWidgetPtr->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(mRuleWidgetPtr, &QWidget::customContextMenuRequested, this, &EventTableDialog::RuleContextMenu);

   QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
   sizePolicy1.setHorizontalStretch(0);
   sizePolicy1.setVerticalStretch(1);
   sizePolicy1.setHeightForWidth(mRuleWidgetPtr->sizePolicy().hasHeightForWidth());
   mRuleWidgetPtr->setSizePolicy(sizePolicy1);

   splitter->addWidget(mRuleWidgetPtr);
   mTablePtr = new QTableView(this);
   mTablePtr->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(mTablePtr, &QWidget::customContextMenuRequested, this, &EventTableDialog::TableItemContextMenu);

   QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
   sizePolicy2.setHorizontalStretch(0);
   sizePolicy2.setVerticalStretch(3);
   sizePolicy2.setHeightForWidth(mTablePtr->sizePolicy().hasHeightForWidth());
   mTablePtr->setSizePolicy(sizePolicy2);

   splitter->addWidget(mTablePtr);
   layout()->addWidget(splitter);
   resize(QSize(1000, 700));
}

void EventTableDialog::SetModel(QAbstractItemModel* aModelPtr)
{
   mSourceModelPtr = aModelPtr;
   mTablePtr->setModel(aModelPtr);
}

void EventTableDialog::LoadFilter()
{
   QString filename = QFileDialog::getOpenFileName(this, "Load Filter", QString(), "*.evt_filter");
   if (!filename.isEmpty())
   {
      LoadFilter(filename);
   }
}

void EventTableDialog::LoadFilter(const QString& aFilename)
{
   QFile file(aFilename);
   file.open(QIODevice::ReadOnly);
   QTextStream ts(&file);
   if (file.isOpen())
   {
      ClearRules();
      while (!ts.atEnd())
      {
         int col;
         ts >> col;
         int type;
         ts >> type;
         if (ts.atEnd())
         {
            break;
         }
         QString str;
         ts >> str;
         QListWidgetItem* item = new QListWidgetItem(GetRuleText(col, type, str), mRuleWidgetPtr);
         item->setData(Qt::UserRole, col);
         item->setData(Qt::UserRole + 1, type);
         item->setData(Qt::UserRole + 2, str);
         item->setData(Qt::UserRole + 4, false);
         AddFilterRule(item);
      }
      file.close();
   }
}

void EventTableDialog::RuleContextMenu(const QPoint& aPoint)
{
   QListWidgetItem* item = mRuleWidgetPtr->itemAt(aPoint);
   QMenu* menu = new QMenu(this);
   if (item != nullptr)
   {
      unsigned int id = item->data(Qt::UserRole + 3).toUInt();
      QAction* removeRule = new QAction("Remove Rule", menu);
      connect(removeRule, &QAction::triggered, [this, item]() {RemoveRule(item); });
      menu->addAction(removeRule);
      QAction* editRule = new QAction("Edit Rule", menu);
      connect(editRule, &QAction::triggered, [this, item]() {EditRule(item); });
      menu->addAction(editRule);
   }
   QAction* clearRules = new QAction("Clear Rules", menu);
   connect(clearRules, &QAction::triggered, [this]() {ClearRules(); });
   menu->addAction(clearRules);

   QAction* saveFilter = new QAction("Save Filter", menu);
   connect(saveFilter, &QAction::triggered, [this]() {SaveFilter(); });
   menu->addAction(saveFilter);
   QAction* loadFilter = new QAction("Load Filter", menu);
   connect(loadFilter, &QAction::triggered, [this]() {LoadFilter(); });
   menu->addAction(loadFilter);

   menu->exec(mRuleWidgetPtr->mapToGlobal(aPoint));
}

void EventTableDialog::TableItemContextMenu(const QPoint& aPoint)
{
   QModelIndex idx = mTablePtr->indexAt(aPoint);
   if (mSourceModelPtr)
   {
      QVariant data = mTablePtr->model()->data(idx, Qt::DisplayRole);
      int col = idx.column();

      QMenu* menu = new QMenu(this);
      QString value = "x";
      QString column = "";
      QAbstractItemModel* activeFilter = mTablePtr->model();
      column = activeFilter->headerData(col, Qt::Horizontal, Qt::DisplayRole).toString();
      value = activeFilter->data(idx, Qt::DisplayRole).toString();
      QAction* showOnly = new QAction("Show when column: " + column + " = " + value, menu);
      connect(showOnly, &QAction::triggered, [this, data, col]() {CreateFilter(data, col, EventTableDialog::FilterTypeEqual); });
      menu->addAction(showOnly);
      QAction* hideAll = new QAction("Hide when column: " + column + " = " + value, menu);
      connect(hideAll, &QAction::triggered, [this, data, col]() {CreateFilter(data, col, EventTableDialog::FilterTypeNotEqual); });
      menu->addAction(hideAll);
      QAction* filterAction = new QAction("Create Filter", menu);
      connect(filterAction, &QAction::triggered, [this, data, col]() {CreateFilter(data, col); });
      menu->addAction(filterAction);
      QAction* exportAction = new QAction("Export", menu);
      connect(exportAction, &QAction::triggered, [this]() {Export(); });
      menu->addAction(exportAction);
      menu->exec(mTablePtr->mapToGlobal(aPoint));
   }
}

void EventTableDialog::CreateFilter(const QVariant& aData,
                                    int             aColumn,
                                    int             aFilterType)
{
   if (aFilterType == EventTableDialog::FilterTypeUnset)
   {
      int t = EventTableDialog::FilterTypeEqual;
      QVariant d = aData;
      QDialog::DialogCode dc = GetFilter(aColumn, t, d);

      if (QDialog::Accepted == dc)
      {
         QListWidgetItem* item = new QListWidgetItem(GetRuleText(aColumn, t, d), mRuleWidgetPtr);
         item->setData(Qt::UserRole, aColumn);
         item->setData(Qt::UserRole + 1, t);
         item->setData(Qt::UserRole + 2, d);
         AddFilterRule(item);
      }
   }
   else
   {
      QListWidgetItem* item = new QListWidgetItem(GetRuleText(aColumn, aFilterType, aData), mRuleWidgetPtr);
      int typeEnum = aFilterType;

      item->setData(Qt::UserRole, aColumn);
      item->setData(Qt::UserRole + 1, typeEnum);
      item->setData(Qt::UserRole + 2, aData);
      AddFilterRule(item);
   }
}

QString EventTableDialog::GetTypeText(int aFilterType)
{
   switch (aFilterType)
   {
   case (EventTableDialog::FilterTypeEqual):
      return "=";
   case (EventTableDialog::FilterTypeNotEqual):
      return "!=";
   case (EventTableDialog::FilterTypeGreater):
      return ">";
   case (EventTableDialog::FilterTypeLess):
      return "<";
   case (EventTableDialog::FilterTypeGreaterOrEqual):
      return ">=";
   case (EventTableDialog::FilterTypeLessOrEqual):
      return "<=";
   case (EventTableDialog::FilterTypeContains):
      return "contains";
   case (EventTableDialog::FilterTypeSide):
      return "side = ";
   case (EventTableDialog::FilterTypeType):
      return "is type of ";
   case (EventTableDialog::FilterTypeCategory):
      return "is of category ";
   default:
      return "?";
   }
}

void EventTableDialog::ClearRules()
{
   mTablePtr->setModel(mSourceModelPtr);
   for (auto&& it : mFilterModels)
   {
      delete it.second;
   }
   mFilterModels.clear();
   mRuleWidgetPtr->clear();
}

void EventTableDialog::RemoveRule(QListWidgetItem* aItem)
{
   unsigned int id = aItem->data(Qt::UserRole + 3).toUInt();
   EventFilterModel* rule = mFilterModels[id];
   QAbstractItemModel* childRule = rule->sourceModel();
   // for every filtered rule, if its source if 'rule', set it's source to 'childRule'
   bool found = false;
   for (auto&& it : mFilterModels)
   {
      if (it.second->sourceModel() == rule)
      {
         it.second->setSourceModel(childRule);
         found = true;
      }
   }
   if (!found) // if it wasn't attached to another rule, it must be attached to the table
   {
      mTablePtr->setModel(childRule);
   }
   delete rule;
   mFilterModels.erase(id);
   delete aItem;
}

void EventTableDialog::EditRule(QListWidgetItem* aItem)
{
   unsigned int id = aItem->data(Qt::UserRole + 3).toUInt();
   int col = aItem->data(Qt::UserRole).toInt();
   int comp = aItem->data(Qt::UserRole + 1).toInt();
   QVariant val = aItem->data(Qt::UserRole + 2);
   bool platform = aItem->data(Qt::UserRole + 4).toBool();
   EventFilterModel* rule = mFilterModels[id];
   if (GetFilter(col, comp, val) == QDialog::Accepted)
   {
      rule->SetComparatorAndValue(comp, val);
      aItem->setText(GetRuleText(col, comp, val));
      aItem->setData(Qt::UserRole + 1, comp);
      aItem->setData(Qt::UserRole + 2, val);
   }
}

void EventTableDialog::AddFilterRule(QListWidgetItem* aItem)
{
   unsigned int id = mIdManager++;
   int col = aItem->data(Qt::UserRole).toInt();
   int comp = aItem->data(Qt::UserRole + 1).toInt();
   QVariant val = aItem->data(Qt::UserRole + 2);
   aItem->setData(Qt::UserRole + 3, id);

   EventFilterModel* model = new EventFilterModel(this, col, comp, val);
   if (comp == EventTableDialog::FilterTypeEqual)
   {
      if (mTablePtr->model()->headerData(col, Qt::Horizontal, Qt::DisplayRole).toString() == "type")
      {
         model->SetTypeFiltered(true);
      }
   }
   model->setSourceModel(mTablePtr->model());
   mTablePtr->setModel(model);
   mRuleWidgetPtr->addItem(aItem);
   mFilterModels[id] = model;
}

void EventTableDialog::Export()
{
   QString fname = QFileDialog::getSaveFileName(this, "Export", "", QString("EVT (*.evt);; Text (*.txt);; All Files(*.*)"));
   if (!fname.isEmpty())
   {
      QFile file(fname);
      if (file.open(QIODevice::WriteOnly))
      {
         QAbstractItemModel* activeFilter = mTablePtr->model();

         for (int i = 0; i < activeFilter->rowCount(); ++i)
         {
            QString data = activeFilter->data(activeFilter->index(i, 0), Qt::UserRole).toString();
            file.write(data.toLocal8Bit());
            // write the row
         }
         file.close();
      }
   }
}

void EventTableDialog::SaveFilter()
{
   QString filename = QFileDialog::getSaveFileName(this, "Save Filter", QString(), "*.evt_filter");
   if (!filename.isEmpty())
   {
      if (!filename.endsWith(".evt_filter"))
      {
         filename += ".evt_filter";
      }
      QFile file(filename);
      file.open(QIODevice::WriteOnly);
      QTextStream ts(&file);
      if (file.isOpen())
      {
         for (auto&& filterrule : mFilterModels)
         {
            ts << filterrule.second->GetColumn() << " " << filterrule.second->GetType() << " " << filterrule.second->GetValue().toString() << "\n";
         }
         file.close();
      }
   }
}

QSize EventTableDialog::sizeHint() const
{
   return QSize(800, 600);
}

QDialog::DialogCode EventTableDialog::GetFilter(const int aColumn,
                                                int&      aFilterType,
                                                QVariant& aValue)
{
   QDialog rule;
   rule.setWindowTitle("Filter rule");
   rule.setLayout(new QVBoxLayout(&rule));
   rule.layout()->addWidget(new QLabel("Show if value in column " + QString::number(aColumn + 1)));
   QComboBox* type = new QComboBox(&rule);
   type->addItem(GetTypeText(EventTableDialog::FilterTypeEqual), EventTableDialog::FilterTypeEqual);
   type->addItem(GetTypeText(EventTableDialog::FilterTypeNotEqual), EventTableDialog::FilterTypeNotEqual);
   type->addItem(GetTypeText(EventTableDialog::FilterTypeGreater), EventTableDialog::FilterTypeGreater);
   type->addItem(GetTypeText(EventTableDialog::FilterTypeLess), EventTableDialog::FilterTypeLess);
   type->addItem(GetTypeText(EventTableDialog::FilterTypeGreaterOrEqual), EventTableDialog::FilterTypeGreaterOrEqual);
   type->addItem(GetTypeText(EventTableDialog::FilterTypeLessOrEqual), EventTableDialog::FilterTypeLessOrEqual);
   type->addItem(GetTypeText(EventTableDialog::FilterTypeContains), EventTableDialog::FilterTypeContains);

   type->setCurrentText(GetTypeText(aFilterType));
   rule.layout()->addWidget(type);
   QLineEdit* le = new QLineEdit(&rule);
   le->setText(aValue.toString());
   rule.layout()->addWidget(le);
   QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &rule);
   rule.layout()->addWidget(bb);
   connect(bb, &QDialogButtonBox::accepted, &rule, &QDialog::accept);
   connect(bb, &QDialogButtonBox::rejected, &rule, &QDialog::reject);
   QDialog::DialogCode retval = (QDialog::DialogCode)rule.exec();
   if (retval == QDialog::Accepted)
   {
      aFilterType = type->currentData().toInt();
      aValue = le->text();
   }
   return retval;
}

QString EventTableDialog::GetRuleText(const int       aColumn,
                                      const int       aFilterType,
                                      const QVariant& aValue)
{
   return "C[" + QString::number(aColumn + 1) + "] " + GetTypeText(aFilterType) + " " + aValue.toString();
}

EventFilterModel::EventFilterModel(QObject*        aParent,
                                   int             aColumn,
                                   int             aType,
                                   const QVariant& aValue)
   : QSortFilterProxyModel(aParent)
   , mColumn(aColumn)
   , mType(aType)
   , mValue(aValue)
   , mTypeFiltered(false)
{
}

QVariant EventFilterModel::headerData(int             section,
                                      Qt::Orientation orientation,
                                      int             role) const
{
   if (mTypeFiltered && (orientation == Qt::Horizontal) && (role == Qt::DisplayRole))
   {
      return data(index(0, section), Qt::ToolTipRole);
   }
   return QSortFilterProxyModel::headerData(section, orientation, role);
}

bool EventFilterModel::filterAcceptsRow(int                source_row,
                                        const QModelIndex& source_parent) const
{
   QVariant left = sourceModel()->data(sourceModel()->index(source_row, mColumn), Qt::DisplayRole);
   if (!Compare(left, mType, mValue))
   {
      return false;
   }
   return true;
}

bool EventFilterModel::Compare(const QVariant& aLeft,
                               int             aCompType,
                               const QVariant& aRight) const
{
   switch (aCompType)
   {
   case (EventTableDialog::FilterTypeEqual):
      return aLeft == aRight;
   case (EventTableDialog::FilterTypeNotEqual):
      return aLeft != aRight;
   case (EventTableDialog::FilterTypeGreater):
      return aLeft > aRight;
   case (EventTableDialog::FilterTypeLess):
      return aLeft < aRight;
   case (EventTableDialog::FilterTypeGreaterOrEqual):
      return aLeft >= aRight;
   case (EventTableDialog::FilterTypeLessOrEqual):
      return aLeft <= aRight;
   case (EventTableDialog::FilterTypeContains):
      return aLeft.toString().toLower().contains(aRight.toString().toLower());
   }
   return true;
}
