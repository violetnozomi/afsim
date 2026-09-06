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


#include "WkfCommVisGraphicsItem.hpp"

#include <QGraphicsScene>
#include <QMessageBox>

#include "UtQtMemory.hpp"

wkf::CommVisGraphicsItem::CommVisGraphicsItem()
{
   mNameTextItem = ut::qt::make_qt_ptr<QGraphicsTextItem>();
}

void wkf::CommVisGraphicsItem::SetImage(const QString& aImageName)
{
   QString imageName = aImageName;
   if (!imageName.endsWith(".png"))
   {
      imageName += ".png";
   }
   setPixmap(QPixmap(":/icons/" + imageName));
   mImageName = imageName;
}

void wkf::CommVisGraphicsItem::SetName(const QString& aName)
{
   mNameTextItem->setPlainText(aName);
   mNameTextItem->setDefaultTextColor(Qt::white);
   mNameTextItem->setPos(scenePos().x(), scenePos().y() + pixmap().height() + 5);
   if (scene())
   {
      scene()->addItem(mNameTextItem);
   }
}

void wkf::CommVisGraphicsItem::RemoveItem()
{
   scene()->removeItem(mNameTextItem);
   scene()->removeItem(this);
}

void wkf::CommVisGraphicsItem::ToggleSelected()
{
   if (!mSelected)
   {
      mSelected            = true;
      int     extensionLoc = mImageName.indexOf(".png");
      QString subStr       = mImageName.left(extensionLoc);
      if (subStr.contains("_off"))
      {
         int offLoc = mImageName.indexOf("_off");
         subStr     = mImageName.left(offLoc);
         subStr += "_selected_off.png";
         SetImage(subStr);
      }
      else
      {
         subStr += "_selected.png";
         SetImage(subStr);
      }
   }
   else
   {
      mSelected            = false;
      int     extensionLoc = mImageName.indexOf("_selected");
      QString subStr       = mImageName.left(extensionLoc);

      if (mImageName.contains("_off"))
      {
         subStr += "_off.png";
         SetImage(subStr);
      }
      else
      {
         subStr += ".png";
         SetImage(subStr);
      }
   }
}

void wkf::CommVisGraphicsItem::TurnOn()
{
   if (mImageName.contains("_off"))
   {
      int     extensionLoc = mImageName.indexOf("_off");
      QString subStr       = mImageName.left(extensionLoc);
      subStr += ".png";
      SetImage(subStr);
   }
}

void wkf::CommVisGraphicsItem::TurnOff()
{
   if (!mImageName.contains("_off"))
   {
      int     extensionLoc = mImageName.indexOf(".png");
      QString subStr       = mImageName.left(extensionLoc);
      subStr += "_off.png";
      SetImage(subStr);
   }
}

void wkf::CommVisGraphicsItem::SetToolTip(const QString& aToolTipString)
{
   setToolTip(aToolTipString);
}


void wkf::CommVisGraphicsItem::SetPos(const QPoint& aPos)
{
   setPos(aPos);

   int halfWidth     = pixmap().width() / 2;
   int halfTextWidth = GetNameGraphicsItem()->boundingRect().width() / 2;
   int diff          = halfWidth - halfTextWidth;
   GetNameGraphicsItem()->setPos(diff + aPos.x(), aPos.y() + pixmap().height() + 2);
}
