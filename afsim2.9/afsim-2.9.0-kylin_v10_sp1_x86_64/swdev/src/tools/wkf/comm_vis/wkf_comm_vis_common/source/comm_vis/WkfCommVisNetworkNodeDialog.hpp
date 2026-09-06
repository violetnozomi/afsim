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

#ifndef WKFCOMMVISNETWORKNODEDIALOG_HPP
#define WKFCOMMVISNETWORKNODEDIALOG_HPP

#include "wkf_comm_vis_common_export.h"

#include <QDialog>

#include "ui_WkfCommVisNetworkNodeDialog.h"

namespace wkf
{
class CommVisNetworkGraphicsItem;
class WKF_COMM_VIS_COMMON_EXPORT CommVisNetworkNodeDialog : public QDialog
{
   Q_OBJECT

public:
   explicit CommVisNetworkNodeDialog(CommVisNetworkGraphicsItem* aNetworkPtr,
                                     QWidget*                    aParent = nullptr,
                                     Qt::WindowFlags             aFlags  = Qt::WindowFlags());

   ~CommVisNetworkNodeDialog() override = default;

protected:
   CommVisNetworkGraphicsItem*  mNetworkObjPtr;
   Ui::CommVisNetworkNodeDialog mUI;
};
} // namespace wkf

#endif
