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

#include "UtQtTableCellButtonDelegate.hpp"

#include <QApplication>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QWidget>


UtQtTableCellButtonDelegate::UtQtTableCellButtonDelegate(const QString& aText /*= ""*/, QObject* aParent /*= 0*/)
   : QStyledItemDelegate(aParent)
   , mButtonWidth(20)
   , mButtonText(aText)
   , mButtonState(QStyle::State_Enabled)
{
}

UtQtTableCellButtonDelegate::~UtQtTableCellButtonDelegate() {}

void UtQtTableCellButtonDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   QStyledItemDelegate::paint(painter, option, index);

   QStyleOptionButton button;
   button.rect  = GetButtonRect(option.rect);
   button.text  = mButtonText;
   button.state = mButtonState | option.state;

   QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
}

bool UtQtTableCellButtonDelegate::editorEvent(QEvent*                     event,
                                              QAbstractItemModel*         model,
                                              const QStyleOptionViewItem& option,
                                              const QModelIndex&          index)
{
   if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease)
   {
      QMouseEvent* e      = (QMouseEvent*)event;
      int          mouseX = e->x();
      int          mouseY = e->y();

      mButtonRect = GetButtonRect(option.rect);
      mScreenRect = option.rect;

      if (mouseX > mButtonRect.x() && mouseX < mButtonRect.x() + mButtonRect.width())
      {
         if (mouseY > mButtonRect.y() && mouseY < mButtonRect.y() + mButtonRect.height())
         {
            if (event->type() == QEvent::MouseButtonPress)
            {
               mButtonState = QStyle::State_Sunken;
               emit pressed(index);
            }
            else if (event->type() == QEvent::MouseButtonRelease)
            {
               mButtonState = QStyle::State_Raised;
               emit clicked(index);
               emit released(index);
            }
            e->accept();
            return true;
         }
      }
   }

   mButtonState = QStyle::State_Raised;
   return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QRect UtQtTableCellButtonDelegate::GetButtonRect(const QRect& aCellRect) const
{
   QRect result;

   result.setX(aCellRect.left() + aCellRect.width() - (int)mButtonWidth);
   result.setY(aCellRect.top());
   result.setWidth((int)mButtonWidth);
   result.setHeight(aCellRect.height());

   return result;
}

QSize UtQtTableCellButtonDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   QSize size = QStyledItemDelegate::sizeHint(option, index);
   size.setWidth(size.width() + (int)mButtonWidth);
   return size;
}
