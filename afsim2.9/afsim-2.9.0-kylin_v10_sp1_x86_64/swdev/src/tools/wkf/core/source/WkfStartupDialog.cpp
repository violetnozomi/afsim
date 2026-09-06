// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfStartupDialog.hpp"

wkf::StartupDialogLabel::StartupDialogLabel(const QString& link, const QString& text, QWidget* parent /* = nullptr */)
   : QLabel(parent)
   , mLink(link)
   , mCachedText(text)
{
   setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);

   QFont f = font();
   f.setPointSize(10);
   setFont(f);

   UpdateLink(false);
}

void wkf::StartupDialogLabel::UpdateLink(bool aHovered)
{
   QColor col = palette().color(QPalette::Link);
   if (aHovered)
   {
      col = col.lighter();
   }

   setText(QString(R"(<a href="%1" style="text-decoration:none; color:%2">%3</a>)").arg(mLink, col.name(), mCachedText));
}

void wkf::StartupDialogLabel::enterEvent(QEvent* event)
{
   UpdateLink(true);
}

void wkf::StartupDialogLabel::leaveEvent(QEvent* event)
{
   UpdateLink(false);
}
