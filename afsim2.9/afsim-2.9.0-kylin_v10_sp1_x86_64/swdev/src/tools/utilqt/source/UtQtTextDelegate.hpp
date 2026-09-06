// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef UTQTTEXTDELEGATE_HPP
#define UTQTTEXTDELEGATE_HPP
#include "utilqt_export.h"

#include <QStyledItemDelegate>
#include <QTextDocument>

//! Uses a QTextDocument to draw styled text in a Qt View
class UTILQT_EXPORT UtQtTextDelegate : public QStyledItemDelegate
{
public:
   UtQtTextDelegate(QWidget* aParentWidget)
      : QStyledItemDelegate(aParentWidget)
      , mParentWidgetPtr(aParentWidget)
   {
   }

   void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

   QWidget*              mParentWidgetPtr;
   mutable QTextDocument mDocument;
};
#endif
