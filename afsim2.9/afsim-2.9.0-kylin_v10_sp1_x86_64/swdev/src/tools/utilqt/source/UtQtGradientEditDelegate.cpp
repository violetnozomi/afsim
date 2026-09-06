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

#include "UtQtGradientEditDelegate.hpp"

#include <cassert>

#include <QColorDialog>
#include <QDoubleValidator>
#include <QEvent>
#include <QLineEdit>
#include <QMessageBox>
#include <QMouseEvent>
#include <QToolTip>

UtQtGradientEditDelegate::UtQtGradientEditDelegate(double aMinValue, double aMaxValue, QObject* aParent)
   : QItemDelegate(aParent)
   , mMinValue(aMinValue)
   , mMaxValue(aMaxValue)
{
}

QWidget* UtQtGradientEditDelegate::createEditor(QWidget* aParent,
                                                const QStyleOptionViewItem& /*aStyle*/,
                                                const QModelIndex& /*aIndex*/) const
{
   QLineEdit* lineEdit = new QLineEdit(aParent);
   connect(lineEdit, SIGNAL(textEdited(QString)), SLOT(HandleTextEdited(QString)));
   return lineEdit;
}

//! Checks to see if the item's color icon was double-clicked to be edited, so
//! that a color chooser can be opened to select a new color. If the item is not
//! editable, it is assumed that the item pertains to the default gradient, and
//! the user will be informed that it cannot be edited.
bool UtQtGradientEditDelegate::editorEvent(QEvent*                     aEvent,
                                           QAbstractItemModel*         aModel,
                                           const QStyleOptionViewItem& aStyle,
                                           const QModelIndex&          aIndex)
{
   if (aEvent->type() == QEvent::MouseButtonDblClick)
   {
      if (aModel->flags(aIndex) & Qt::ItemIsEditable)
      {
         QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(aEvent);
         if (mouseEvent->pos().x() <= aStyle.decorationSize.width())
         {
            QColorDialog colorDlg(dynamic_cast<QWidget*>(parent()));
            colorDlg.setCurrentColor(aModel->data(aIndex, Qt::UserRole).value<QColor>());
            if (colorDlg.exec() == QColorDialog::Accepted)
            {
               aModel->setData(aIndex, colorDlg.currentColor(), Qt::UserRole);
            }
            aEvent->accept();
            return true;
         }
      }
      else
      {
         QMessageBox::information(dynamic_cast<QWidget*>(parent()),
                                  "Cannot edit",
                                  "Default gradients cannot be edited. Please add a new gradient.");
         aEvent->ignore();
         return true;
      }
   }
   return QItemDelegate::editorEvent(aEvent, aModel, aStyle, aIndex);
}

void UtQtGradientEditDelegate::setEditorData(QWidget* aEditor, const QModelIndex& aIndex) const
{
   QLineEdit*     lineEdit = static_cast<QLineEdit*>(aEditor);
   const QString& text     = aIndex.model()->data(aIndex, Qt::EditRole).toString();
   lineEdit->setText(text);
}

//! If the user has entered valid input while editing, this will set the model
//! data to the new valid input. If the input is invalid, the input will be
//! rejected, and a message will be displayed to the user.
void UtQtGradientEditDelegate::setModelData(QWidget* aEditor, QAbstractItemModel* aModel, const QModelIndex& aIndex) const
{
   QLineEdit* lineEdit = static_cast<QLineEdit*>(aEditor);
   if (IsTextValid(lineEdit->text()))
   {
      aModel->setData(aIndex, lineEdit->text(), Qt::EditRole);
   }
   else
   {
      QMessageBox::information(aEditor, "Invalid value", InvalidTextMessage());
   }
}

void UtQtGradientEditDelegate::updateEditorGeometry(QWidget*                    aEditor,
                                                    const QStyleOptionViewItem& aStyle,
                                                    const QModelIndex& /*aIndex*/) const
{
   aEditor->setGeometry(aStyle.rect);
}

//! While the user is typing into the text edit and while their input is
//! invalid, this will change the background of the text edit to a light red
//! color and display a tooltip explaining the error.
void UtQtGradientEditDelegate::HandleTextEdited(const QString& aText)
{
   QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(sender());
   assert(lineEdit);
   if (lineEdit)
   {
      if (!IsTextValid(aText))
      {
         assert(lineEdit->parentWidget());
         lineEdit->setStyleSheet("QLineEdit { background-color: pink; }");
         QToolTip::showText(lineEdit->mapToGlobal(lineEdit->rect().bottomLeft()), InvalidTextMessage());
      }
      else
      {
         lineEdit->setStyleSheet("");
      }
   }
}

//! Returns whether or not the given text is a real number that falls between
//! mMinValue and mMaxValue;
bool UtQtGradientEditDelegate::IsTextValid(const QString& aText) const
{
   QDoubleValidator validator;
   validator.setBottom(mMinValue);
   validator.setTop(mMaxValue);
   QString text   = aText;
   int     unused = -1;
   return (validator.validate(text, unused) == QDoubleValidator::Acceptable);
}

QString UtQtGradientEditDelegate::InvalidTextMessage() const
{
   return QString("Values must fall between %1 and %2.").arg(mMinValue).arg(mMaxValue);
}
