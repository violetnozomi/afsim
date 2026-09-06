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

#ifndef WKFCOMMVISGRAPHICSPACKETITEM_HPP
#define WKFCOMMVISGRAPHICSPACKETITEM_HPP


#include "wkf_comm_vis_common_export.h"

#include <QGraphicsEllipseItem>
#include <QGraphicsItemAnimation>
#include <QObject>
#include <QPropertyAnimation>
#include <QTimeLine>

#include "UtQtUiPointer.hpp"

namespace wkf
{
class CommVisLink;
class WKF_COMM_VIS_COMMON_EXPORT CommVisPacketGraphicsItem : public QObject, public QGraphicsEllipseItem
{
   Q_OBJECT
   Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
   CommVisPacketGraphicsItem(const QColor&   aColor,
                             const QString&  aFromAddress,
                             const QString&  aToAddress,
                             CommVisLink*    aLinkStageOne,
                             CommVisLink*    aLinkStageTwo = nullptr,
                             QGraphicsScene* aScenePtr     = nullptr,
                             QGraphicsItem*  aParent       = nullptr);

   ~CommVisPacketGraphicsItem() override = default;

   void SendPacket();
   void SetPacketSpeed(unsigned int aSendSpeed) { mSendSpeed = aSendSpeed; }
   void SetFade(bool aFade) { mFade = aFade; }

   const CommVisLink* GetStageOneLink() const { return mLinkStageOne; }
   const CommVisLink* GetStageTwoLink() const { return mLinkStageTwo; }
signals:

   void PacketComplete();

private:
   void FadePacket();
   void SendStageOne();
   void SendStageTwo();

   CommVisLink* mLinkStageOne;
   CommVisLink* mLinkStageTwo;

   QString                                   mFromAddress;
   QString                                   mToAddress;
   bool                                      mFade      = true;
   unsigned int                              mSendSpeed = 1000; // 1000 ms, 1 sec
   ut::qt::UiPointer<QTimeLine>              mStageOneTimer;
   ut::qt::UiPointer<QGraphicsItemAnimation> mStageOneAnimation;
   ut::qt::UiPointer<QTimeLine>              mStageTwoTimer;
   ut::qt::UiPointer<QGraphicsItemAnimation> mStageTwoAnimation;
   ut::qt::UiPointer<QPropertyAnimation>     mOpacityAnimation;
};
} // namespace wkf

#endif
