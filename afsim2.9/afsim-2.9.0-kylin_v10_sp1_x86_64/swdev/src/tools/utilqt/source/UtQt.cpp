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

#include "UtQt.hpp"

#include <cassert>

#include <QApplication>
#include <QGraphicsColorizeEffect>
#include <QMenu>
#include <QMessageBox>
#include <QPalette>
#include <QPropertyAnimation>
#include <QStandardItemModel>
#include <QTabWidget>
#include <QTreeView>
#include <QtCore/QPersistentModelIndex>

#include "UtQtSlider.hpp"

// Removes an item's row from a QStandardItemModel.
void UtQtRemoveItemRow(QStandardItemModel& aModel, const QPersistentModelIndex& aIndex)
{
   QModelIndex parentIndex = aIndex.parent();
   aModel.removeRow(aIndex.row(), parentIndex);
}

// Raise a widget as well as switch to the tab
void UtQtRaiseWidget(QWidget* aWidgetPtr)
{
   // tab widgets have an intermediate 'stacked' widget between them and the tab
   // store 2 levels of children and walk up looking for a tab widget.
   QWidget* c2 = nullptr;
   QWidget* c  = aWidgetPtr;
   while (c)
   {
      QWidget* p = c->parentWidget();
      if (p && c2)
      {
         QTabWidget* tabWidgetPtr = dynamic_cast<QTabWidget*>(p);
         if (tabWidgetPtr)
         {
            tabWidgetPtr->setCurrentWidget(c2);
         }
      }
      c2 = c;
      c  = p;
   }
   aWidgetPtr->raise();
}

/// Animates a widget by slowly fading it in.
void UtQtHighlightWidget(QWidget* widget)
{
   QGraphicsColorizeEffect* effect = new QGraphicsColorizeEffect(widget->parent());
   effect->setColor(QColor(255, 255, 255)); // RGB = 0 to 255

   // Note: QGraphicsItem::setGraphicsEffect(...) documentation states that if
   // there already is an effect installed, then the existing effect will be
   // deleted before installing the new effect. So no risk of memory leak by
   // setting and forgetting.
   widget->setGraphicsEffect(effect);

   QPropertyAnimation* animation = new QPropertyAnimation(effect, "strength");
   animation->setDuration(300);   // msecs
   animation->setStartValue(1.0); // strength = 1.0 (full colorization)
   animation->setEndValue(0.0);   // strength = 0.0 (no effect)
   animation->start(QAbstractAnimation::DeleteWhenStopped);
}

/// Popup a simple error dialog for the user.
void UtQtShowSimpleMsg(const QString& windowTitle, const QString& text, const QString& informativeText, QWidget* parent)
{
   QMessageBox msg(parent);
   msg.setWindowTitle(windowTitle);
   msg.setText(text);
   msg.setInformativeText(informativeText);
   msg.setStandardButtons(QMessageBox::Ok);
   msg.setDefaultButton(QMessageBox::Ok);
   msg.exec();
}

//! Get the set of expanded items from a tree view
void UtQtGetExpandedItems(QTreeView& aTree, const QModelIndex& aIndex, QSet<QPersistentModelIndex>& aExpanded)
{
   if (aTree.isExpanded(aIndex) || aIndex == QModelIndex()) // expanded or is root
   {
      aExpanded.insert(QPersistentModelIndex(aIndex));
      QAbstractItemModel* modelptr = aTree.model();
      assert(modelptr);
      int rowCt = modelptr->rowCount(aIndex);
      for (int i = 0; i < rowCt; ++i)
      {
         QModelIndex idx = modelptr->index(i, 0, aIndex);
         if (idx.isValid())
         {
            UtQtGetExpandedItems(aTree, idx, aExpanded);
         }
      }
   }
}

//! Restore the expand-state of items in a tree view
void UtQtSetExpandedItems(QTreeView& aTree, const QModelIndex& aIndex, const QSet<QPersistentModelIndex>& aExpanded)
{
   bool shouldExpand = aExpanded.contains(aIndex);
   if (shouldExpand != aTree.isExpanded(aIndex))
   {
      if (shouldExpand)
      {
         aTree.expand(aIndex);
      }
      else
      {
         aTree.collapse(aIndex);
      }
   }
   QAbstractItemModel* modelptr = aTree.model();
   int                 rowCt    = modelptr->rowCount(aIndex);
   for (int i = 0; i < rowCt; ++i)
   {
      QModelIndex idx = modelptr->index(i, 0, aIndex);
      if (idx.isValid())
      {
         UtQtSetExpandedItems(aTree, idx, aExpanded);
      }
   }
}

void UtQtTranslucentMenu(QMenu& aMenu)
{
   aMenu.setWindowFlags(aMenu.windowFlags() | Qt::FramelessWindowHint);
   aMenu.setAttribute(Qt::WA_TranslucentBackground);
   QColor bgcolor = aMenu.palette().base().color();
   QColor hlcolor = aMenu.palette().highlight().color();
   bgcolor.setAlphaF(0.75);
   hlcolor.setAlphaF(0.75);
   QString ss = QString("QMenu{background:%1;} QMenu::item:selected{background:%2;}")
                   .arg(bgcolor.name(QColor::HexArgb), hlcolor.name(QColor::HexArgb));
   aMenu.setStyleSheet(ss);
}

void UTILQT_EXPORT UtQtSetTextColor(QWidget* aWidgetPtr, QColor aColor)
{
   QPalette         palette(aWidgetPtr->palette());
   QAbstractButton* buttonPtr = dynamic_cast<QAbstractButton*>(aWidgetPtr);
   if (buttonPtr)
   {
      palette.setColor(QPalette::ButtonText, aColor);
   }
   else
   {
      palette.setColor(QPalette::Text, aColor);
   }
   aWidgetPtr->setPalette(palette);
}

void UTILQT_EXPORT UtQtSetInvalid(QWidget* aWidgetPtr)
{
   UtQtSetTextColor(aWidgetPtr, Qt::red);
}

void UTILQT_EXPORT UtQtSetValid(QWidget* aWidgetPtr)
{
   UtQtSetTextColor(aWidgetPtr, qApp->palette().color(QPalette::Text));
}

void UTILQT_EXPORT UtQtSetValid(QWidget* aWidgetPtr, bool aIsValid)
{
   if (aIsValid)
      UtQtSetValid(aWidgetPtr);
   else
      UtQtSetInvalid(aWidgetPtr);
}
