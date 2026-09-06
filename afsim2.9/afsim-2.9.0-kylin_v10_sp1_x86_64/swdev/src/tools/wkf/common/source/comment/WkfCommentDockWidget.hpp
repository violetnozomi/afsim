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

#ifndef WKF_COMMENT_DOCK_WIDGET_HPP
#define WKF_COMMENT_DOCK_WIDGET_HPP

#include "wkf_common_export.h"

#include <QDockWidget>

#include "ui_WkfCommentDockWidget.h"

namespace wkf
{
class WKF_COMMON_EXPORT CommentDockWidget : public QDockWidget
{
   Q_OBJECT
public:
   explicit CommentDockWidget(QWidget* aParent);

private:
   Ui::WkfCommentDockWidget mUi;
};
} // namespace wkf

#endif
