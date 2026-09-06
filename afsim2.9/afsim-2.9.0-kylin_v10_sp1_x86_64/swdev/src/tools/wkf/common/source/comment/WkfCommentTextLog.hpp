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

#ifndef WKF_COMMENT_TEXT_LOG_HPP
#define WKF_COMMENT_TEXT_LOG_HPP

#include "wkf_common_export.h"

#include <QPlainTextEdit>

namespace wkf
{
class WKF_COMMON_EXPORT CommentTextLog : public QPlainTextEdit
{
   Q_OBJECT
public:
   explicit CommentTextLog(QWidget* aParent);

   void Write(double aSimTime, const QString& aPlatform, const QString& aComment);

protected:
   void Append(const QString& aHtml);

   void mousePressEvent(QMouseEvent* aEvent) override;
   void mouseMoveEvent(QMouseEvent* aEvent) override;
};
} // namespace wkf

#endif
