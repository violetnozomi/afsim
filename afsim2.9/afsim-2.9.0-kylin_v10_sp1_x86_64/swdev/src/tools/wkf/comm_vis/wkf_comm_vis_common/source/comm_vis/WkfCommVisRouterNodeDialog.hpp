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

#ifndef WKFCOMMVISROUTERNODEDIALOG_HPP
#define WKFCOMMVISROUTERNODEDIALOG_HPP

#include "wkf_comm_vis_common_export.h"

#include <QDialog>

#include "ui_WkfCommVisRouterNodeDialog.h"

namespace wkf
{
class CommVisRouterGraphicsItem;
class WKF_COMM_VIS_COMMON_EXPORT CommVisRouterNodeDialog : public QDialog
{
   Q_OBJECT

public:
   explicit CommVisRouterNodeDialog(CommVisRouterGraphicsItem* aRouterObjPtr,
                                    QWidget*                   aParent = nullptr,
                                    Qt::WindowFlags            aFlags  = Qt::WindowFlags());
   ~CommVisRouterNodeDialog() override = default;

protected:
   Ui::CommVisRouterNodeDialog mUI;
   CommVisRouterGraphicsItem*  mRouterObjPtr;

   bool    mAILDefault{false};
   bool    mUseDefaultProtocolDefault{true};
   bool    mUseMulticastDefault{true};
   QString mHopLimitDefault{"64"};
};
} // namespace wkf

#endif
