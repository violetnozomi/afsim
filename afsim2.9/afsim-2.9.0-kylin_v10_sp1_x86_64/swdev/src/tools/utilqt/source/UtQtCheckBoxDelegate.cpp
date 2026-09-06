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

#include "UtQtCheckBoxDelegate.hpp"

#include <QApplication>
#include <QKeyEvent>
#include <QMouseEvent>

#include "UtQtCenteredCheckBox.hpp"

UtQtCheckBoxDelegate::UtQtCheckBoxDelegate(QObject* aParent /*= 0*/)
   : QStyledItemDelegate(aParent)
{
}

UtQtCheckBoxDelegate::~UtQtCheckBoxDelegate() {}

void UtQtCheckBoxDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   bool     checked;
   QVariant value = index.model()->data(index, Qt::CheckStateRole);
   if (!value.isNull())
   {
      checked = value.toBool();
   }
   else
   {
      checked = true;
   }

   if (index.column() == 0)
   {
      QStyleOptionButton checkboxstyle;
      QRect checkbox_rect = QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator, &checkboxstyle);
      checkboxstyle.rect  = option.rect;
      checkboxstyle.rect.setLeft(
         (int)((option.rect.x() + (option.rect.width() * 0.5) - (checkbox_rect.width() * 0.5)) - 0.5));
      if (checked)
      {
         checkboxstyle.state = QStyle::State_On | QStyle::State_Enabled;
      }
      else
      {
         checkboxstyle.state = QStyle::State_Off | QStyle::State_Enabled;
      }

      QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkboxstyle, painter);
   }
   else
   {
      QStyledItemDelegate::paint(painter, option, index);
   }
}

QWidget* UtQtCheckBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   UtQtCenteredCheckBox* checkBox = new UtQtCenteredCheckBox(parent);
   checkBox->setChecked(true);

   return checkBox;
}

void UtQtCheckBoxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
   bool checked;

   QVariant value = index.model()->data(index, Qt::CheckStateRole);
   if (!value.isNull())
   {
      checked = value.toBool();
   }
   else
   {
      checked = true;
   }

   UtQtCenteredCheckBox* checkBox = static_cast<UtQtCenteredCheckBox*>(editor);
   checkBox->checkBox()->setChecked(checked);
}

void UtQtCheckBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
   UtQtCenteredCheckBox* checkBox = static_cast<UtQtCenteredCheckBox*>(editor);

   model->setData(index, checkBox->checkBox()->isChecked(), Qt::CheckStateRole);
}

void UtQtCheckBoxDelegate::updateEditorGeometry(QWidget*                    editor,
                                                const QStyleOptionViewItem& option,
                                                const QModelIndex&          index) const
{
   editor->setGeometry(option.rect);
}

bool UtQtCheckBoxDelegate::editorEvent(QEvent*                     event,
                                       QAbstractItemModel*         model,
                                       const QStyleOptionViewItem& option,
                                       const QModelIndex&          index)
{
   if ((event->type() == QEvent::MouseButtonRelease) || (event->type() == QEvent::MouseButtonDblClick))
   {
      QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
      if (mouseEvent->button() != Qt::LeftButton || !CheckBoxRect(option).contains(mouseEvent->pos()))
      {
         return false;
      }

      if (event->type() == QEvent::MouseButtonDblClick)
      {
         return true;
      }
   }
   else if (event->type() == QEvent::KeyPress)
   {
      if (static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space && static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select)
      {
         return false;
      }
   }
   else
   {
      return false;
   }

   bool checked = index.model()->data(index, Qt::CheckStateRole).toBool();

   return model->setData(index, !checked, Qt::CheckStateRole);
}

QRect UtQtCheckBoxDelegate::CheckBoxRect(const QStyleOptionViewItem& aOptions)
{
   QStyleOptionButton styleOptions;
   QRect              rect = QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator, &styleOptions);

   QPoint pt((int)(aOptions.rect.x() + aOptions.rect.width() * 0.5 - rect.width() * 0.5),
             (int)(aOptions.rect.y() + aOptions.rect.height() * 0.5 - rect.height() * 0.5));

   return QRect(pt, rect.size());
}
