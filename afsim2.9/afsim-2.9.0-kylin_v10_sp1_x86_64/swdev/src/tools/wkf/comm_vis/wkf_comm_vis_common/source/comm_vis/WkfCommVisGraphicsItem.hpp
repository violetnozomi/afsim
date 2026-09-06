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

#ifndef WKFCOMMVISGRAPHICSITEM_HPP
#define WKFCOMMVISGRAPHICSITEM_HPP

#include "wkf_comm_vis_common_export.h"

#include <QDialog>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QObject>

#include "UtQtUiPointer.hpp"

namespace wkf
{
class WKF_COMM_VIS_COMMON_EXPORT CommVisGraphicsItem : public QObject, public QGraphicsPixmapItem
{
public:
   CommVisGraphicsItem();
   ~CommVisGraphicsItem() override = default;

   void         mouseDoubleClickEvent(QGraphicsSceneMouseEvent* aEvent) override{};
   void         SetImage(const QString& aImageName);
   virtual void SetName(const QString& aName);
   void         ToggleSelected();
   void         TurnOff();
   void         TurnOn();
   void         SetToolTip(const QString& aToolTipString);
   void         SetPos(const QPoint& aPos);
   void         RemoveItem();

   virtual const QString& GetName() const    = 0;
   virtual const QString& GetAddress() const = 0;
   const QString&         GetDeviceType() const { return mDeviceType; }
   QGraphicsTextItem*     GetNameGraphicsItem() { return mNameTextItem; }
   virtual void           DialogClosed() = 0;

protected:
   QString mDeviceType;

private:
   bool                                 mSelected{false};
   QString                              mImageName;
   ut::qt::UiPointer<QGraphicsTextItem> mNameTextItem;
};

} // namespace wkf

#endif
