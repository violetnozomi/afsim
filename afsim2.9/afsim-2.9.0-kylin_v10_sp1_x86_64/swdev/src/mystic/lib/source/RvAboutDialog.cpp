// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "RvAboutDialog.hpp"

#include <QApplication>

rv::AboutDialog::AboutDialog(wkf::MainWindow* parent, Qt::WindowFlags f /* = Qt::WindowFlags() */)
   : wkf::AboutDialog(parent, f)
{
   GetInfoLabel()->setText(qApp->applicationName() + QString("\n") + qApp->applicationVersion() + QString("\n"));
}
