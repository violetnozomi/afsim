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
#include "WkfTypeVisibilityPrefWidget.hpp"

#include <QKeyEvent>
#include <QPushButton>

#include "WkfTypeVisibilityPrefObject.hpp"

wkf::TypeVisibilityPrefWidget::TypeVisibilityPrefWidget(QWidget* aParent /*= nullptr*/)
   : PrefWidgetT<TypeVisibilityPrefObject>(aParent)
{
   mUI.setupUi(this);

   // Set up other widget settings
   mUI.typeListWidget->installEventFilter(this);
   mUI.typeListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

   // Set up connections
   connect(mUI.addTypePushButton, &QPushButton::clicked, this, &TypeVisibilityPrefWidget::AddFilter);
   connect(mUI.removePushButton, &QPushButton::clicked, this, &TypeVisibilityPrefWidget::RemoveFilters);
}

void wkf::TypeVisibilityPrefWidget::showEvent(QShowEvent* aEvent)
{
   ReadPreferenceData(mPrefObjectPtr->GetPreferences());
}

void wkf::TypeVisibilityPrefWidget::ReadPreferenceData(const TypeVisibilityPrefData& aPrefData)
{
   mUI.typeListWidget->clear();
   for (const auto& type : aPrefData.mHiddenTypes)
   {
      mUI.typeListWidget->addItem(type);
   }
   mUI.typeListWidget->sortItems(Qt::AscendingOrder);
}

void wkf::TypeVisibilityPrefWidget::WritePreferenceData(TypeVisibilityPrefData& aPrefData)
{
   aPrefData.mHiddenTypes.clear();
   for (int i = 0; i < mUI.typeListWidget->count(); ++i)
   {
      aPrefData.mHiddenTypes.insert(mUI.typeListWidget->item(i)->text());
   }
}

void wkf::TypeVisibilityPrefWidget::AddFilter()
{
   QString platformType = mUI.addTypeLineEdit->text();
   if (!platformType.isEmpty() && mUI.typeListWidget->findItems(platformType, Qt::MatchExactly).empty())
   {
      mUI.typeListWidget->addItem(new QListWidgetItem(platformType));
      mUI.typeListWidget->sortItems(Qt::AscendingOrder);
   }
}

void wkf::TypeVisibilityPrefWidget::RemoveFilters()
{
   qDeleteAll(mUI.typeListWidget->selectedItems());
}

bool wkf::TypeVisibilityPrefWidget::eventFilter(QObject* aObject, QEvent* aEvent)
{
   if (aEvent->type() == QEvent::KeyPress)
   {
      QKeyEvent* keyEvent = static_cast<QKeyEvent*>(aEvent);
      if (keyEvent->key() == Qt::Key_Delete || keyEvent->key() == Qt::Key_Backspace)
      {
         RemoveFilters();
         return true;
      }
   }
   return false;
}
