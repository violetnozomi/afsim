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

#include "WkfCommentDockWidget.hpp"

#include "WkfCommentForwardingService.hpp"
#include "WkfUtils.hpp"

wkf::CommentDockWidget::CommentDockWidget(QWidget* aParent)
   : QDockWidget(aParent)
{
   mUi.setupUi(this);

   connect(wkf::CommentForwardingService::Request(),
           &wkf::CommentForwardingService::CommentReady,
           mUi.textLog,
           &CommentTextLog::Write);

   connect(wkf::CommentForwardingService::Request(),
           &wkf::CommentForwardingService::ClearComments,
           mUi.textLog,
           &CommentTextLog::clear);
}
