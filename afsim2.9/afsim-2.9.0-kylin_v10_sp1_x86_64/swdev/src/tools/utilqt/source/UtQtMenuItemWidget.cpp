// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2014 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "UtQtMenuItemWidget.hpp"

#include <QHBoxLayout>
#include <QLabel>


UtQtMenuItemWidget::MenuItemContainer::MenuItemContainer(QWidget* aParent, const QString& aLabel, QWidget* aWidgetPtr)
   : QWidget(aParent)
{
   QHBoxLayout* layoutPtr = new QHBoxLayout(this);
   setLayout(layoutPtr);
   layoutPtr->setContentsMargins(0, 0, 0, 0);
   layoutPtr->setSpacing(10);
   mLabelPtr  = new QLabel(aLabel, this);
   mWidgetPtr = aWidgetPtr;
   mWidgetPtr->setParent(this);
   setFocusProxy(mWidgetPtr);
   layout()->addWidget(mLabelPtr);
   layout()->addWidget(mWidgetPtr);

   setFocusPolicy(Qt::StrongFocus);

   if (aLabel.isEmpty())
   {
      mLabelPtr->hide();
   }

   // Note: Font on the label does not match other menu items... not sure how to fix
}

QWidget* UtQtMenuItemWidget::createWidget(QWidget* aParent)
{
   QWidget* valueWidget = mCreateFn();
   QWidget* b           = new MenuItemContainer(aParent, mLabel, valueWidget);
   b->show();
   return b;
}

UtQtMenuItemWidget::UtQtMenuItemWidget(QObject* aParent, QString aLabel, const std::function<QWidget*()>& aCreateFn)
   : QWidgetAction(aParent)
   , mLabel(aLabel)
   , mCreateFn(aCreateFn)
{
}

UtQtMenuItemWidget::UtQtMenuItemWidget(QObject* aParent, const std::function<QWidget*()>& aCreateFn)
   : QWidgetAction(aParent)
   , mCreateFn(aCreateFn)
{
}
