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

#include "DSV_Dialog.hpp"

#include "GeneralFormat.hpp"

PostProcessor::DSV_Dialog::DSV_Dialog(QMainWindow*    aMainWindowPtr,
                                      ProxyInterface* aProxyInterface,
                                      Qt::WindowFlags aFlags /*= Qt::WindowFlags()*/)
   : ReportDialog(aMainWindowPtr, ReportType::cDSV, aProxyInterface, aFlags)
{
   setWindowTitle("DSV Output Report Definition");
   Build();
}

void PostProcessor::DSV_Dialog::Build()
{
   auto formatWidget = dynamic_cast<GeneralFormat*>(GetWidget("General.Format"));
   if(formatWidget) {
      formatWidget->SetDelimiter("comma");
   }
}
