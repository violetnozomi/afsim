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

#include "WkfFilterItem.hpp"

#include <QSizePolicy>

#include "UtMemory.hpp"
#include "WkfFilterWidget.hpp"

namespace WkfFilter
{
static bool TakeLayoutItem(QLayout* aLayout, QLayoutItem*& aItem)
{
   if (aLayout != nullptr)
   {
      aItem = aLayout->takeAt(0);
   }
   else
   {
      aItem = nullptr;
   }

   return aItem != nullptr;
}

FilterItem::FilterItem(bool aIsRoot)
{
   mUi.setupUi(this);

   if (aIsRoot)
   {
      mUi.typeComboBox->removeItem(0);
      mUi.removeButton->setEnabled(false);

      GotoGroupView();
   }
   else
   {
      GotoConditionView();
   }

   ConnectUi();
   SetupSizePolicy();
   setContextMenuPolicy(Qt::CustomContextMenu);
}

void FilterItem::SetType(FilterObject::Type aType)
{
   switch (aType)
   {
   case FilterObject::Type::And:
      mUi.typeComboBox->setCurrentText("And");
      GotoGroupView();
      break;
   case FilterObject::Type::Or:
      mUi.typeComboBox->setCurrentText("Or");
      GotoGroupView();
      break;
   case FilterObject::Type::Condition:
   default:
      mUi.typeComboBox->setCurrentText("Condition");
      GotoConditionView();
   }
}

void FilterItem::SetEnabled(bool aEnable)
{
   QSignalBlocker l(mUi.enableCheckBox);
   mUi.enableCheckBox->setChecked(aEnable);
}

void FilterItem::SetInverted(bool aInvert)
{
   QSignalBlocker l(mUi.invertCheckBox);
   mUi.invertCheckBox->setChecked(aInvert);
}

void FilterItem::SetColumn(const QString& aName, int aIndex)
{
   QSignalBlocker l(mUi.columnLineEdit);
   mUi.columnLineEdit->setText(aName);
   if (aIndex >= 0)
   {
      mUi.columnLabel->setText(QString("(col #%1)").arg(aIndex + 1));
   }
   else
   {
      mUi.columnLabel->setText("(no col)");
   }
}

void FilterItem::SetRulesList(const QStringList& aList)
{
   SetRules(aList, mUi.testComboBox->currentText());
}

void FilterItem::SetRules(const QStringList& aList, const QString& aCurrentItem)
{
   QSignalBlocker l(mUi.testComboBox);
   QStringList    currentList;

   for (int i = 0; i < mUi.testComboBox->count(); i++)
   {
      currentList += mUi.testComboBox->itemText(i);
   }

   if (aList != currentList)
   {
      mUi.testComboBox->clear();

      mUi.testComboBox->insertItems(0, aList);

      if (aList.contains(aCurrentItem))
      {
         mUi.testComboBox->setCurrentText(aCurrentItem);
      }
      else
      {
         mUi.testComboBox->setCurrentIndex(0);
      }
   }
   else if (mUi.testComboBox->currentText() != aCurrentItem)
   {
      mUi.testComboBox->setCurrentText(aCurrentItem);
   }
}

void FilterItem::SetValue(const QString& aValue)
{
   QSignalBlocker l(mUi.valueLineEdit);
   mUi.valueLineEdit->setText(aValue);
}

FilterItem* FilterItem::MakeChild()
{
   auto* newChild  = new FilterItem(false); // not root
   auto* newLayout = new QVBoxLayout();
   auto* oldLayout = mUi.subFiltersList->layout();

   QLayoutItem* item;
   while (TakeLayoutItem(oldLayout, item))
   {
      newLayout->addItem(item);
   }

   delete oldLayout;

   newLayout->addWidget(newChild);
   newLayout->setSizeConstraint(QLayout::SetMinimumSize);

   mUi.subFiltersList->setLayout(newLayout);

   return newChild;
}

void FilterItem::RemoveChild(FilterItem* aChild)
{
   auto* oldLayout = mUi.subFiltersList->layout();
   auto* newLayout = new QVBoxLayout();

   QLayoutItem* item;

   while (TakeLayoutItem(oldLayout, item))
   {
      if (dynamic_cast<FilterItem*>(item->widget()) == aChild)
      {
         delete item->widget();
         delete item;
      }
      else
      {
         newLayout->addItem(item);
      }
   }

   delete oldLayout;
   newLayout->setSizeConstraint(QLayout::SetMinimumSize);

   mUi.subFiltersList->setLayout(newLayout);
}

void FilterItem::ConnectUi()
{
   connect(mUi.typeComboBox,
           QOverload<const QString&>::of(&QComboBox::activated),
           [this](const QString& aType)
           {
              if (aType == "And")
              {
                 GotoGroupView();
                 emit TypeChanged(this, FilterObject::Type::And);
              }
              else if (aType == "Or")
              {
                 GotoGroupView();
                 emit TypeChanged(this, FilterObject::Type::Or);
              }
              else
              {
                 GotoConditionView();
                 emit TypeChanged(this, FilterObject::Type::Condition);
              }
           });

   connect(mUi.enableCheckBox, &QCheckBox::toggled, [this](bool aEnable) { emit EnableChanged(this, aEnable); });
   connect(mUi.invertCheckBox, &QCheckBox::toggled, [this](bool aInvert) { emit InvertChanged(this, aInvert); });

   connect(mUi.columnLineEdit,
           &QLineEdit::editingFinished,
           [this]() { emit ColumnChanged(this, mUi.columnLineEdit->text()); });
   connect(mUi.testComboBox,
           QOverload<const QString&>::of(&QComboBox::activated),
           [this](const QString& aTest) { emit TestChanged(this, aTest); });
   connect(mUi.valueLineEdit,
           &QLineEdit::editingFinished,
           [this]() { emit ValueChanged(this, mUi.valueLineEdit->text()); });

   connect(mUi.addButton,
           &QToolButton::clicked,
           [this]()
           {
              mUi.expandCheckBox->setChecked(true);
              emit AddClicked(this);
           });
   connect(mUi.removeButton, &QToolButton::clicked, [this]() { emit RemoveClicked(this); });

   connect(this,
           &FilterItem::customContextMenuRequested,
           [this](const QPoint& aPosition) { emit ContextMenu(this, aPosition); });

   connect(mUi.expandCheckBox, &QCheckBox::toggled, [this](bool aExpand) { mUi.subFiltersList->setVisible(aExpand); });
}

void FilterItem::SetupSizePolicy()
{
   QSizePolicy policy = mUi.columnLineEdit->sizePolicy();
   policy.setRetainSizeWhenHidden(true);
   mUi.columnLineEdit->setSizePolicy(policy);

   policy = mUi.columnLabel->sizePolicy();
   policy.setRetainSizeWhenHidden(true);
   mUi.columnLabel->setSizePolicy(policy);

   policy = mUi.testComboBox->sizePolicy();
   policy.setRetainSizeWhenHidden(true);
   mUi.testComboBox->setSizePolicy(policy);

   policy = mUi.valueLineEdit->sizePolicy();
   policy.setRetainSizeWhenHidden(true);
   mUi.valueLineEdit->setSizePolicy(policy);
}

void FilterItem::GotoConditionView()
{
   mUi.expandCheckBox->hide();
   mUi.subFiltersList->hide();

   mUi.columnLineEdit->show();
   mUi.columnLabel->show();
   mUi.testComboBox->show();
   mUi.valueLineEdit->show();

   mUi.addButton->setEnabled(false);
}

void FilterItem::GotoGroupView()
{
   mUi.expandCheckBox->show();
   mUi.subFiltersList->show();

   mUi.columnLineEdit->hide();
   mUi.columnLabel->hide();
   mUi.testComboBox->hide();
   mUi.valueLineEdit->hide();

   mUi.addButton->setEnabled(true);
}
} // namespace WkfFilter
