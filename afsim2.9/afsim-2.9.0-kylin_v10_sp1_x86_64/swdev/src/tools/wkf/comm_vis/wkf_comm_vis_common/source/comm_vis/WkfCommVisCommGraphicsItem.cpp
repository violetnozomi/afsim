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


#include "WkfCommVisCommGraphicsItem.hpp"

#include <QGraphicsScene>
#include <QGraphicsView>

wkf::CommVisCommGraphicsItem::CommVisCommGraphicsItem(const QString& aCommName,
                                                      const QString& aCommAddress,
                                                      const QString& aCommType)
   : mCommName(aCommName)
   , mCommAddress(aCommAddress)
   , mCommType(aCommType)
{
   SetImage("comm");

   QString toolTipStr = "Comm:   " + aCommName;
   SetToolTip(toolTipStr);
   mDeviceType = "Comm";
   SetName(mCommName);
}

void wkf::CommVisCommGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* aEvent)
{
   ToggleSelected();

   if (!mDialogWindowPtr)
   {
      QWidget* parentPtr = nullptr;
      if (scene() && !scene()->views().empty())
      {
         parentPtr = scene()->views()[0]->parentWidget();
      }

      mDialogWindowPtr = QPointer<wkf::CommVisCommNodeDialog>(new wkf::CommVisCommNodeDialog(this, parentPtr));
   }

   mDialogWindowPtr->show();
}


void wkf::CommVisCommGraphicsItem::DialogClosed()
{
   ToggleSelected();
}

void wkf::CommVisCommGraphicsItem::SetOnOffStatus(bool aOn)
{
   mIsTurnedOn = aOn;
   if (mIsTurnedOn)
   {
      TurnOn();
   }
   else
   {
      TurnOff();
   }
}

void wkf::CommVisCommGraphicsItem::RemoveComm()
{
   // Remove this comm and it's text label
   RemoveItem();
}
