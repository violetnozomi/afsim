// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfCommentTextLog.hpp"

#include <QScrollBar>
#include <QTextCursor>

#include "WkfUtils.hpp"

wkf::CommentTextLog::CommentTextLog(QWidget* aParent)
   : QPlainTextEdit(aParent)
{
   setMouseTracking(true);
   setReadOnly(true);
   setTextInteractionFlags(Qt::TextBrowserInteraction);
}

void wkf::CommentTextLog::Write(double aSimTime, const QString& aPlatform, const QString& aComment)
{
   Append(wkf::util::ParseLinks(QString("(%1)[{p:%2}]%3").arg(QString::number(aSimTime, 'f', 2), aPlatform, aComment)));
}

void wkf::CommentTextLog::Append(const QString& aHtml)
{
   auto sb    = verticalScrollBar();
   bool atEnd = sb && (sb->value() == sb->maximum());

   auto cursor = textCursor();
   cursor.movePosition(QTextCursor::End);
   appendHtml(aHtml);

   if (sb && atEnd)
   {
      sb->setValue(sb->maximum());
   }
}

void wkf::CommentTextLog::mousePressEvent(QMouseEvent* aEvent)
{
   QString anchor = anchorAt(aEvent->pos());
   if (!anchor.isEmpty())
   {
      QString errorMsg;
      if (!wkf::util::FollowLink(anchor, errorMsg))
      {
         Append(QString("<font color=#FF6666>%1</font>").arg(errorMsg));
      }
   }
   QPlainTextEdit::mousePressEvent(aEvent);
}

void wkf::CommentTextLog::mouseMoveEvent(QMouseEvent* aEvent)
{
   QString anchor = anchorAt(aEvent->pos());
   if (!anchor.isEmpty())
   {
      viewport()->setCursor(Qt::PointingHandCursor);
   }
   else
   {
      viewport()->setCursor(Qt::IBeamCursor);
   }
   QPlainTextEdit::mouseMoveEvent(aEvent);
}
