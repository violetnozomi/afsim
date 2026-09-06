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

#ifndef WKFCOMMVISGATEWAYNODEDIALOG_HPP
#define WKFCOMMVISGATEWAYNODEDIALOG_HPP

#include "wkf_comm_vis_common_export.h"

#include <QDialog>

#include "ui_WkfCommVisGatewayNodeDialog.h"

namespace wkf
{
class CommVisGatewayGraphicsItem;
class WKF_COMM_VIS_COMMON_EXPORT CommVisGatewayNodeDialog : public QDialog
{
   Q_OBJECT

public:
   explicit CommVisGatewayNodeDialog(CommVisGatewayGraphicsItem* aGatewayObjPtr,
                                     QWidget*                    aParent = nullptr,
                                     Qt::WindowFlags             aFlags  = Qt::WindowFlags());

   ~CommVisGatewayNodeDialog() override = default;

protected:
   Ui::CommVisGatewayNodeDialog mUI;
   CommVisGatewayGraphicsItem*  mGatewayObjPtr;
};
} // namespace wkf

#endif
