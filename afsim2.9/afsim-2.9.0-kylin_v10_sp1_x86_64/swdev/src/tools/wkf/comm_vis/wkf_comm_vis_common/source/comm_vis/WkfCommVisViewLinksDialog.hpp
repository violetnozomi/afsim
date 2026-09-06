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

#ifndef WKFCOMMVISVIEWLINKSDIALOG_HPP
#define WKFCOMMVISVIEWLINKSDIALOG_HPP

#include "wkf_comm_vis_common_export.h"

#include <QDialog>

#include "WkfCommVisDataContainer.hpp"
#include "ui_WkfCommVisViewLinksDialog.h"

namespace wkf
{
class WKF_COMM_VIS_COMMON_EXPORT CommVisViewLinksDialog : public QDialog
{
   Q_OBJECT

public:
   explicit CommVisViewLinksDialog(QWidget* aParent = nullptr, Qt::WindowFlags aFlags = Qt::WindowFlags());
   explicit CommVisViewLinksDialog(const std::vector<wkf::CommEvent::LinkInfo>& aLinkInfo,
                                   QWidget*                                     aParent = nullptr,
                                   Qt::WindowFlags                              aFlags  = Qt::WindowFlags());
   ~CommVisViewLinksDialog() override = default;

   const QTableWidget* GetLinksTable() const { return mUI.mLinksTable; }

protected:
   Ui::CommVisViewLinksDialog mUI;
};
} // namespace wkf

#endif
