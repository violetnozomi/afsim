// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfProxyStyle.hpp"

wkf::ProxyStyle::ProxyStyle(QStyle* aStyle /*= 0*/)
   : QProxyStyle(aStyle)
{
}

QPixmap wkf::ProxyStyle::generatedIconPixmap(QIcon::Mode iconMode, const QPixmap& pixmap, const QStyleOption* opt) const
{
   if (iconMode == QIcon::Disabled)
   {
      QImage tmp = pixmap.toImage();

      for (int y = 0; y < tmp.height(); ++y)
      {
         for (int x = 0; x < tmp.width(); ++x)
         {
            QColor c = tmp.pixelColor(x, y);
            c.setAlpha(c.alpha() * 0.3);
            tmp.setPixelColor(x, y, c);
         }
      }

      return QPixmap::fromImage(tmp);
   }
   return pixmap;
}
