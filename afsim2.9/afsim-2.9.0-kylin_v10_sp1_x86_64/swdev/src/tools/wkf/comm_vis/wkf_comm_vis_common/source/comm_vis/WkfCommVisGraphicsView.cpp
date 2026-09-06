// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfCommVisGraphicsView.hpp"

#include <QPoint>
#include <QScrollBar>
#include <QWheelEvent>

#include "UtQtMemory.hpp"
#include "WkfCommVisGraphicsItem.hpp"

wkf::CommVisGraphicsView::CommVisGraphicsView(QWidget* aParentPtr)
{
   mScene = ut::qt::make_qt_ptr<QGraphicsScene>();
   setScene(mScene);
   // Do not show scroll bars
   setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

   mHomePos.setX(viewport()->width() * 0.5);
   mHomePos.setY(viewport()->height() * 0.25);

   // Connect scene update to autoresize
   connect(scene(), &QGraphicsScene::changed, this, &CommVisGraphicsView::AutoComputeSceneSize);
}

void wkf::CommVisGraphicsView::mouseMoveEvent(QMouseEvent* aEvent)
{
   QGraphicsView::mouseMoveEvent(aEvent);
   if (aEvent->buttons() & Qt::LeftButton)
   {
      scene()->update(mapToScene(rect()).boundingRect());
   }

   mViewportPos = aEvent->pos();
   mMousePos    = mapToScene(aEvent->pos());
}

void wkf::CommVisGraphicsView::mousePressEvent(QMouseEvent* aEvent)
{
   if (aEvent->buttons() & Qt::LeftButton)
   {
      setDragMode(QGraphicsView::ScrollHandDrag);
   }

   QGraphicsView::mousePressEvent(aEvent);
}


void wkf::CommVisGraphicsView::mouseReleaseEvent(QMouseEvent* aEvent)
{
   if (dragMode() & QGraphicsView::ScrollHandDrag)
   {
      setDragMode(QGraphicsView::NoDrag);
   }
   QGraphicsView::mouseReleaseEvent(aEvent);
}


void wkf::CommVisGraphicsView::wheelEvent(QWheelEvent* aEvent)
{
   if (aEvent->delta() > 0)
   {
      ZoomIn();
   }
   else
   {
      ZoomOut();
   }

   aEvent->accept();
}


void wkf::CommVisGraphicsView::ZoomIn()
{
   if (mZoomCount <= -12)
   {
      return;
   }
   else
   {
      mZoomCount -= 1;
      scale(1.2, 1.2);
      centerOn(mMousePos);
      QPointF deltaViewportPos = mViewportPos - QPointF(viewport()->width() / 2.0, viewport()->height() / 2.0);
      QPointF viewportCenter   = mapFromScene(mMousePos) - deltaViewportPos;
      centerOn(mapToScene(viewportCenter.toPoint()));
   }
}
void wkf::CommVisGraphicsView::ZoomOut()
{
   if (mZoomCount == 12)
   {
      return;
   }
   else
   {
      mZoomCount += 1;
      scale(1 / 1.2, 1 / 1.2);
      centerOn(mMousePos);
      QPointF deltaViewportPos = mViewportPos - QPointF(viewport()->width() / 2.0, viewport()->height() / 2.0);
      QPointF viewportCenter   = mapFromScene(mMousePos) - deltaViewportPos;
      centerOn(mapToScene(viewportCenter.toPoint()));
   }
}

void wkf::CommVisGraphicsView::AutoComputeSceneSize(const QList<QRectF>& region)
{
   Q_UNUSED(region);

   // Widget viewport rectangle
   QRectF widget_rect_in_scene(mapToScene(-20, -20), mapToScene(rect().bottomRight() + QPoint(20, 20)));

   // Copy the new size from the old one
   QPointF new_top_left(sceneRect().topLeft());
   QPointF new_bottom_right(sceneRect().bottomRight());

   // Check that the scene has a bigger limit in the top side
   if (sceneRect().top() > widget_rect_in_scene.top())
      new_top_left.setY(widget_rect_in_scene.top());

   // Check that the scene has a bigger limit in the bottom side
   if (sceneRect().bottom() < widget_rect_in_scene.bottom())
      new_bottom_right.setY(widget_rect_in_scene.bottom());

   // Check that the scene has a bigger limit in the left side
   if (sceneRect().left() > widget_rect_in_scene.left())
      new_top_left.setX(widget_rect_in_scene.left());

   // Check that the scene has a bigger limit in the right side
   if (sceneRect().right() < widget_rect_in_scene.right())
      new_bottom_right.setX(widget_rect_in_scene.right());

   // Set new scene size
   setSceneRect(QRectF(new_top_left, new_bottom_right));
}

void wkf::CommVisGraphicsView::keyPressEvent(QKeyEvent* aKeyEvent)
{
   if (aKeyEvent->key() == Qt::Key_Home)
   {
      while (mZoomCount < 0)
      {
         ZoomOut();
      }

      while (mZoomCount > 0)
      {
         ZoomIn();
      }
      mMousePos.setX(mHomePos.x());
      mMousePos.setY(mHomePos.y());
      centerOn(mHomePos);
   }
}


void wkf::CommVisGraphicsView::AddItem(CommVisGraphicsItem* aItem)
{
   scene()->addItem(aItem);
   scene()->addItem(aItem->GetNameGraphicsItem());
}
