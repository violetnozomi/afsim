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

#include "UtQtDragDropToolbar.hpp"

#include <QAction>
#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>

UtQtDragDropToolbar::UtQtDragDropToolbar(QWidget* parent)
   : QToolBar(parent)
   , mDragActionPtr(nullptr)
{
}

void UtQtDragDropToolbar::actionEvent(QActionEvent* event)
{
   // Call the base class first to create the tool button from the action, and add to the toolbar
   QToolBar::actionEvent(event);

   if (event->type() == QEvent::ActionAdded)
   {
      // Install event filter on the tool button to receive mouse press events in the toolbar
      widgetForAction(event->action())->installEventFilter(this);
   }
   else if (event->type() == QEvent::ActionRemoved)
   {
      QWidget* widget = widgetForAction(event->action());
      if (widget)
      {
         widget->removeEventFilter(this);
      }
   }
}

bool UtQtDragDropToolbar::eventFilter(QObject* watched, QEvent* event)
{
   if (event->type() == QEvent::MouseButtonPress)
   {
      QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
      mDragStartPos           = mapFromGlobal(mouseEvent->globalPos());
      mDragActionPtr          = actionAt(mDragStartPos);
      return false; // don't filter the event, forward to the tool button
   }
   else
   {
      // standard event processing
      return QToolBar::eventFilter(watched, event);
   }
}

void UtQtDragDropToolbar::mouseMoveEvent(QMouseEvent* event)
{
   // Make sure the left mouse button is down on a tool button
   if (mDragActionPtr && (event->buttons() & Qt::LeftButton))
   {
      int dist = (event->pos() - mDragStartPos).manhattanLength();
      if (dist >= QApplication::startDragDistance())
      {
         // Put the action's text in the MIME data
         QDrag*     drag     = new QDrag(this);
         QMimeData* mimeData = new QMimeData;
         mimeData->setText(mDragActionPtr->text());
         drag->setMimeData(mimeData);

         QPoint hotspot(mDragActionPtr->data().toPoint());
         if (hotspot.isNull())
         {
            hotspot = QPoint(16, 16);
         }

         drag->setHotSpot(hotspot);
         drag->setPixmap(mDragActionPtr->icon().pixmap(32));
         drag->exec();
      }
   }

   QToolBar::mouseMoveEvent(event);
}

void UtQtDragDropToolbar::mousePressEvent(QMouseEvent* event)
{
   // Mouse clicked inside the toolbar but not on a tool button, so reset the drag action
   mDragActionPtr = nullptr;

   QToolBar::mousePressEvent(event);
}
