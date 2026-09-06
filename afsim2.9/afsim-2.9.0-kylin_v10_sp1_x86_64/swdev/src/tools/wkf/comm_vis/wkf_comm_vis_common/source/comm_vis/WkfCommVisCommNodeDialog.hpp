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

#ifndef WKFCOMMVISCOMMNODEDIALOG_HPP
#define WKFCOMMVISCOMMNODEDIALOG_HPP

#include "wkf_comm_vis_common_export.h"

#include <QDialog>
#include <QPointer>

#include "UtQtUiPointer.hpp"
#include "WkfCommVisViewLinksDialog.hpp"
#include "ui_WkfCommVisCommNodeDialog.h"

namespace wkf
{
class CommVisCommGraphicsItem;
class WKF_COMM_VIS_COMMON_EXPORT CommVisCommNodeDialog : public QDialog
{
   Q_OBJECT

public:
   explicit CommVisCommNodeDialog(CommVisCommGraphicsItem* aCommObjPtr,
                                  QWidget*                 aParent = nullptr,
                                  Qt::WindowFlags          aFlags  = Qt::WindowFlags());
   ~CommVisCommNodeDialog() override = default;

protected:
   Ui::CommVisCommNodeDialog                 mUI;
   CommVisCommGraphicsItem*                  mCommObjPtr;
   ut::qt::UiPointer<CommVisViewLinksDialog> mLinksDialog;
};
} // namespace wkf

#endif
