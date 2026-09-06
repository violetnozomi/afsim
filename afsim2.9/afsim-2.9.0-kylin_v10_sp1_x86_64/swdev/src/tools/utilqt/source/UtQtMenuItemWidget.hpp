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
#ifndef UTQTMENUITEMWIDGET_HPP
#define UTQTMENUITEMWIDGET_HPP

#include "utilqt_export.h"

#include <functional>

#include <QWidgetAction>

class QHBoxLayout;
class QLabel;

//
class UTILQT_EXPORT UtQtMenuItemWidget : public QWidgetAction
{
   // A small form with a label and any other widget
   class UTILQT_EXPORT MenuItemContainer : public QWidget
   {
   public:
      MenuItemContainer(QWidget* aParent, const QString& aLabel, QWidget* aWidgetPtr);

      QHBoxLayout* mLayout;
      QLabel*      mLabelPtr;
      QWidget*     mWidgetPtr;
   };

public:
   // Create a menu item with a label and a custom widget
   // aCreateFn is a function which returns a new widget pointer when called
   UtQtMenuItemWidget(QObject* aParent, QString aLabel, const std::function<QWidget*()>& aCreateFn);

   // Create a menu item with no label and a custom widget
   // aCreateFn is a function which returns a new widget pointer when called
   UtQtMenuItemWidget(QObject* aParent, const std::function<QWidget*()>& aCreateFn);

   QWidget* createWidget(QWidget* aParent) override;

   QString                   mLabel;
   std::function<QWidget*()> mCreateFn;
};


#endif
