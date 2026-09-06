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

#include "WkfCommVisLink.hpp"

#include "WkfCommVisCommGraphicsItem.hpp"
#include "WkfCommVisNetworkGraphicsItem.hpp"

wkf::CommVisLink::CommVisLink(const CommVisGraphicsItem* aFromLink, const CommVisGraphicsItem* aToLink)
   : mFromLink(aFromLink)
   , mToLink(aToLink)
{
   if (mFromLink->GetDeviceType() == "Comm" && mToLink->GetDeviceType() == "Comm")
   {
      mIsDirectLink = true;
   }
}

void wkf::CommVisLink::SetLine(const QColor& aColor)
{
   int originX = GetFromDevicePos().x() + GetFromDeviceWidth();
   int originY = GetFromDevicePos().y() + (GetFromDeviceHeight() * 0.5);

   int destinationX = GetToDevicePos().x();
   if (mIsDirectLink)
   {
      destinationX += GetToDeviceWidth();
   }

   int destinationY = GetToDevicePos().y() + (GetToDeviceHeight() * 0.5);

   mLinkPath = QPainterPath(QPointF(originX, originY));
   mLinkPath.lineTo(destinationX, destinationY);
   mLinkPen = QPen(aColor);
}

void wkf::CommVisLink::SetQuad(const QColor& aColor)
{
   int originX = GetFromDevicePos().x() + GetFromDeviceWidth();
   int originY = GetFromDevicePos().y() + (GetFromDeviceHeight() * 0.5);

   int destinationX = GetToDevicePos().x();
   if (mIsDirectLink)
   {
      destinationX += GetToDeviceWidth();
   }
   int destinationY = GetToDevicePos().y() + (GetToDeviceHeight() * 0.5);

   int dist     = std::abs(originY - destinationY);
   int halfYPos = dist * 0.5;
   if (originY > destinationY)
   {
      halfYPos += destinationY;
   }
   else
   {
      halfYPos += originY;
   }

   mLinkPath = QPainterPath(QPointF(originX, originY));
   if (GetFromDevicePos().x() == GetToDevicePos().x())
   {
      mLinkPath.quadTo(originX + dist, halfYPos, destinationX, destinationY);
   }
   else
   {
      int distX    = std::abs(destinationX - originX);
      int firstPtX = originX + (0.15 * distX);

      mLinkPath.quadTo(firstPtX, halfYPos, destinationX, destinationY);
   }

   mLinkPen = QPen(aColor);
}
