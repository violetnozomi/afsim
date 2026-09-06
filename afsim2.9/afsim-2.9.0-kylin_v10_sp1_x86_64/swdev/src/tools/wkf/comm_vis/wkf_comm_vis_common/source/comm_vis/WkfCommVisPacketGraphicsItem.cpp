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

#include "WkfCommVisPacketGraphicsItem.hpp"

#include <QGraphicsItemAnimation>
#include <QGraphicsPathItem>
#include <QPropertyAnimation>
#include <QTimeLine>

#include "UtQtMemory.hpp"
#include "WkfCommVisLink.hpp"

wkf::CommVisPacketGraphicsItem::CommVisPacketGraphicsItem(const QColor&   aColor,
                                                          const QString&  aFromAddress,
                                                          const QString&  aToAddress,
                                                          CommVisLink*    aLinkStageOne,
                                                          CommVisLink*    aLinkStageTwo,
                                                          QGraphicsScene* aScenePtr,
                                                          QGraphicsItem*  aParent)
   : QGraphicsEllipseItem(0, 0, 20, 20, aParent)
   , mLinkStageOne(aLinkStageOne)
   , mLinkStageTwo(aLinkStageTwo)
   , mFromAddress(aFromAddress)
   , mToAddress(aToAddress)
{
   setBrush(aColor);
   if (aScenePtr)
   {
      aScenePtr->addItem(this);
   }
}

void wkf::CommVisPacketGraphicsItem::SendPacket()
{
   SendStageOne();
}


void wkf::CommVisPacketGraphicsItem::SendStageOne()
{
   if (mLinkStageOne->GetPathItem())
   {
      mStageOneTimer = ut::qt::make_qt_ptr<QTimeLine>(mSendSpeed);
      mStageOneTimer->setFrameRange(0, 100);

      QPen newPen(Qt::yellow);
      newPen.setWidth(3);
      mLinkStageOne->GetPathItem()->setPen(newPen);
      if (!mLinkStageTwo)
      {
         connect(mStageOneTimer.data(), &QTimeLine::finished, this, &CommVisPacketGraphicsItem::FadePacket);
      }
      else
      {
         if (mLinkStageTwo->GetPathItem())
         {
            connect(mStageOneTimer.data(), &QTimeLine::finished, this, &CommVisPacketGraphicsItem::SendStageTwo);
            mLinkStageTwo->GetPathItem()->setPen(newPen);
         }
      }

      mStageOneAnimation = ut::qt::make_qt_ptr<QGraphicsItemAnimation>();
      mStageOneAnimation->setItem(this);
      mStageOneAnimation->setTimeLine(mStageOneTimer);

      QPointF packetPos;

      bool moveRightToLeft = false;
      if (mFromAddress == mLinkStageOne->GetFromDeviceAddress())
      {
         packetPos = mLinkStageOne->GetPath().pointAtPercent(0);

         if (mLinkStageOne->GetFromDevicePos().x() > mLinkStageOne->GetToDevicePos().x())
         {
            moveRightToLeft = true;
         }
      }
      else if (mFromAddress == mLinkStageOne->GetToDeviceAddress())
      {
         packetPos = mLinkStageOne->GetPath().pointAtPercent(0);

         if (mLinkStageOne->GetToDevicePos().x() >= mLinkStageOne->GetFromDevicePos().x())
         {
            moveRightToLeft = true;
         }
      }

      setPos(packetPos);
      for (double i = 0; i < 1; i = i + 0.1)
      {
         double  pktPercent = moveRightToLeft ? 1 - i : i;
         QPointF interpPos  = mLinkStageOne->GetPath().pointAtPercent(pktPercent).toPoint();
         interpPos.setY(interpPos.y() - 10);
         mStageOneAnimation->setPosAt(i, interpPos);
      }
      mStageOneTimer->start();
   }
}

void wkf::CommVisPacketGraphicsItem::SendStageTwo()
{
   mStageTwoTimer = ut::qt::make_qt_ptr<QTimeLine>(mSendSpeed);
   mStageTwoTimer->setFrameRange(0, 100);

   connect(mStageTwoTimer.data(), &QTimeLine::finished, this, &CommVisPacketGraphicsItem::FadePacket);

   mStageTwoAnimation = ut::qt::make_qt_ptr<QGraphicsItemAnimation>();
   mStageTwoAnimation->setItem(this);
   mStageTwoAnimation->setTimeLine(mStageTwoTimer);

   QPointF packetPos;
   bool    moveRightToLeft = false;
   if (mToAddress == mLinkStageTwo->GetFromDeviceAddress())
   {
      packetPos = mLinkStageTwo->GetPath().pointAtPercent(0);

      if (mLinkStageTwo->GetFromDevicePos().x() > mLinkStageTwo->GetToDevicePos().x())
      {
         moveRightToLeft = true;
      }
   }
   else if (mToAddress == mLinkStageOne->GetToDeviceAddress())
   {
      packetPos = mLinkStageTwo->GetPath().pointAtPercent(0);
      if (mLinkStageOne->GetToDevicePos().x() >= mLinkStageOne->GetFromDevicePos().x())
      {
         moveRightToLeft = true;
      }
   }

   for (double i = 0; i < 1; i = i + 0.1)
   {
      double  pktPercent = moveRightToLeft ? 1 - i : i;
      QPointF interpPos  = mLinkStageTwo->GetPath().pointAtPercent(pktPercent).toPoint();
      interpPos.setY(interpPos.y() - 10);
      mStageTwoAnimation->setPosAt(i, interpPos);
   }
   mStageTwoTimer->start();
}

void wkf::CommVisPacketGraphicsItem::FadePacket()
{
   if (mLinkStageOne->GetPathItem())
   {
      mLinkStageOne->GetPathItem()->setPen(QColor(Qt::darkYellow));
   }

   if (mLinkStageTwo && mLinkStageTwo->GetPathItem())
   {
      mLinkStageTwo->GetPathItem()->setPen(QColor(Qt::darkYellow));
   }

   if (mFade)
   {
      mOpacityAnimation = ut::qt::make_qt_ptr<QPropertyAnimation>(this, "opacity");
      mOpacityAnimation->setDuration(1000);

      mOpacityAnimation->setKeyValueAt(0, 1);
      mOpacityAnimation->setKeyValueAt(1, 0);

      connect(mOpacityAnimation.data(), &QPropertyAnimation::finished, this, &CommVisPacketGraphicsItem::PacketComplete);

      mOpacityAnimation->start();
   }
   else
   {
      emit PacketComplete();
   }
}
