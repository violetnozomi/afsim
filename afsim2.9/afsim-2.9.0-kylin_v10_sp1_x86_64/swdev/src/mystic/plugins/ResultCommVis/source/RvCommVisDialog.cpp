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
#include "RvCommVisDialog.hpp"

#include "WkfMainWindow.hpp"

RvCommVis::CommVisDialog::CommVisDialog(wkf::DataContainer& aCommData, QWidget* aParent, Qt::WindowFlags aFlags)
   : wkf::CommVisDialog::CommVisDialog(aCommData, aParent, aFlags)
   , mCurSimTime(0.0)
{
}
