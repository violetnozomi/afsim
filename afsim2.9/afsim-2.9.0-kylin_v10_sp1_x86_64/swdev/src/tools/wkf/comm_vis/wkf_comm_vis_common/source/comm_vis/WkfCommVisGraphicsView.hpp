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

#ifndef WKFCOMMVISGRAPHICSVIEW_HPP
#define WKFCOMMVISGRAPHICSVIEW_HPP

#include "wkf_comm_vis_common_export.h"

#include <QGraphicsView>

#include "UtQtUiPointer.hpp"
namespace wkf
{
class CommVisGraphicsItem;
class WKF_COMM_VIS_COMMON_EXPORT CommVisGraphicsView : public QGraphicsView
{
   Q_OBJECT
public:
   explicit CommVisGraphicsView(QWidget* aParentPtr = nullptr);
   ~CommVisGraphicsView() override = default;

   void mouseMoveEvent(QMouseEvent* aEvent) override;
   void mousePressEvent(QMouseEvent* aEvent) override;
   void mouseReleaseEvent(QMouseEvent* aEvent) override;
   void keyPressEvent(QKeyEvent* aKeyEvent) override;
   void wheelEvent(QWheelEvent* aEvent) override;
   void ZoomIn();
   void ZoomOut();
   void AutoComputeSceneSize(const QList<QRectF>& region);

   void AddItem(CommVisGraphicsItem* aItem);

private:
   int                               mZoomCount{0};
   ut::qt::UiPointer<QGraphicsScene> mScene;
   QPointF                           mMousePos;
   QPointF                           mViewportPos;
   QPointF                           mHomePos;
};
} // namespace wkf
#endif
