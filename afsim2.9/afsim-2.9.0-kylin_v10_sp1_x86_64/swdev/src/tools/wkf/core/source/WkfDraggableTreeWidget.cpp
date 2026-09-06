// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfDraggableTreeWidget.hpp"

#include <QApplication>
#include <QMouseEvent>

#include "WkfDefines.hpp"

void wkf::DraggableTreeWidget::mousePressEvent(QMouseEvent* aEvent)
{
   if (mDragEnabled)
   {
      QTreeWidgetItem* item = itemAt(aEvent->pos());
      if (item)
      {
         if (item->data(0, wkf::UPDATER_ROLE).toBool() == true)
         {
            if (aEvent->buttons() & Qt::LeftButton)
            {
               mDragStart    = aEvent->pos();
               mDragPossible = true;
            }
         }
      }
   }
   QTreeWidget::mousePressEvent(aEvent);
}

void wkf::DraggableTreeWidget::mouseMoveEvent(QMouseEvent* aEvent)
{
   if (mDragPossible)
   {
      if (!(aEvent->buttons() & Qt::LeftButton))
      {
         mDragPossible = false;
      }
      else if ((aEvent->pos() - mDragStart).manhattanLength() > QApplication::startDragDistance())
      {
         QTreeWidgetItem* item = itemAt(mDragStart);
         if (item)
         {
            if (item->data(0, wkf::UPDATER_ROLE).toBool() == true)
            {
               emit DragEvent(item);
               mDragPossible = false;
            }
         }
      }
   }
   QTreeWidget::mouseMoveEvent(aEvent);
}
