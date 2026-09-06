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
#ifndef RVCOMMVISDIALOG_HPP
#define RVCOMMVISDIALOG_HPP

#include <QDialog>

#include "RvPlugin.hpp"
#include "comm_vis/WkfCommVisDataContainer.hpp"
#include "comm_vis/WkfCommVisDialog.hpp"

namespace RvCommVis
{
class CommVisDialog : public wkf::CommVisDialog
{
   Q_OBJECT
public:
   explicit CommVisDialog(wkf::DataContainer& aCommData,
                          QWidget*            aParent = nullptr,
                          Qt::WindowFlags     aFlags  = Qt::WindowFlags());

private:
   float mCurSimTime;
};
} // namespace RvCommVis
#endif
