// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfTopLevelDropTreeWidget.hpp"

#include <QDropEvent>

wkf::TopLevelDropTreeWidget::TopLevelDropTreeWidget(QWidget* aParent)
   : QTreeWidget(aParent)
{
}

void wkf::TopLevelDropTreeWidget::dragMoveEvent(QDragMoveEvent* event)
{
   // only allow drops into toplevel items, and notify user of this behavior
   bool accepted = false;
   auto item     = itemAt(event->pos());
   if (item && !item->parent())
   {
      DropIndicatorPosition dropIndicator = dropIndicatorPosition();
      if (dropIndicator == DropIndicatorPosition::OnItem)
      {
         accepted = true;
      }
   }
   if (!accepted)
   {
      event->setDropAction(Qt::IgnoreAction);
   }
   event->setAccepted(accepted);
}

void wkf::TopLevelDropTreeWidget::dropEvent(QDropEvent* event)
{
   // Addresses case where dropped item is occasionally dropped above/below a toplevelitem despite indicating it will be
   // dropped inside
   QModelIndex dropIndex = indexAt(event->pos());
   if (!dropIndex.parent().isValid() && dropIndex.row() != -1)
   {
      DropIndicatorPosition dropIndicator = dropIndicatorPosition();
      if (dropIndicator == DropIndicatorPosition::OnItem)
      {
         QTreeWidget::dropEvent(event);
         return;
      }
   }
   event->setDropAction(Qt::IgnoreAction);
   event->setAccepted(false);
}
