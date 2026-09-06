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

#ifndef UTQTTABLECELLBUTTONDELEGATE_HPP
#define UTQTTABLECELLBUTTONDELEGATE_HPP

#include "utilqt_export.h"

#include <QRect>
#include <QStyledItemDelegate>
#include <QToolButton>

class UTILQT_EXPORT UtQtTableCellButtonDelegate : public QStyledItemDelegate
{
   Q_OBJECT

public:
   UtQtTableCellButtonDelegate(const QString& aText = "", QObject* aParent = nullptr);
   ~UtQtTableCellButtonDelegate() override;

   void  SetText(const QString& aText) { mButtonText = aText; }
   QRect rect() const { return mButtonRect; }
   QRect screenRect() const { return mScreenRect; }

signals:
   void clicked(const QModelIndex& index);
   void released(const QModelIndex& index);
   void pressed(const QModelIndex& index);

protected:
   // Qt reimplemented
   void  paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
   bool  editorEvent(QEvent*                     event,
                     QAbstractItemModel*         model,
                     const QStyleOptionViewItem& option,
                     const QModelIndex&          index) override;
   QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
   QRect GetButtonRect(const QRect& aCellRect) const;
   void  SetButtonRect(const QRect& aButtonRect) { mButtonRect = aButtonRect; }

   float         mButtonWidth;
   QString       mButtonText;
   QStyle::State mButtonState;
   QRect         mButtonRect;
   QRect         mScreenRect;
};

#endif // UTQTTABLECELLBUTTONDELEGATE_HPP
