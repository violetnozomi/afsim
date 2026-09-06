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

#ifndef WKFCOMMVISLINK_HPP
#define WKFCOMMVISLINK_HPP


#include "wkf_comm_vis_common_export.h"

#include <QGraphicsScene>
#include <QPainterPath>
#include <QPen>

#include "WkfCommVisGraphicsItem.hpp"

namespace wkf
{

class WKF_COMM_VIS_COMMON_EXPORT CommVisLink
{
public:
   CommVisLink(const CommVisGraphicsItem* aFromLink, const CommVisGraphicsItem* aToLink);
   ~CommVisLink() = default;

   void SetLine(const QColor& aColor);
   void SetQuad(const QColor& aColor);

   QPainterPath GetPath() const { return mLinkPath; }
   QPen         GetPen() const { return mLinkPen; }

   const QString& GetFromDeviceName() const { return mFromLink->GetName(); }
   const QString& GetToDeviceName() const { return mToLink->GetName(); }

   const QString& GetFromDeviceAddress() const { return mFromLink->GetAddress(); }
   const QString& GetToDeviceAddress() const { return mToLink->GetAddress(); }

   int     GetFromDeviceHeight() const { return mFromLink->pixmap().height(); }
   int     GetFromDeviceWidth() const { return mFromLink->pixmap().width(); }
   QPointF GetFromDevicePos() const { return mFromLink->pos(); }

   int     GetToDeviceHeight() const { return mToLink->pixmap().height(); }
   int     GetToDeviceWidth() const { return mToLink->pixmap().width(); }
   QPointF GetToDevicePos() const { return mToLink->pos(); }

   void               SetPathItem(QGraphicsPathItem* aPathItem) { mPathItem = aPathItem; }
   QGraphicsPathItem* GetPathItem() const { return mPathItem; }

   bool IsDirectLink() const { return mIsDirectLink; }

private:
   const CommVisGraphicsItem* mFromLink;
   const CommVisGraphicsItem* mToLink;
   bool mIsDirectLink{false}; // Direct link true means this is link connects comm device to comm device as opposed to
                              // comm device to network
   QPainterPath       mLinkPath;
   QPen               mLinkPen;
   QGraphicsPathItem* mPathItem{nullptr};
};

} // namespace wkf

#endif
