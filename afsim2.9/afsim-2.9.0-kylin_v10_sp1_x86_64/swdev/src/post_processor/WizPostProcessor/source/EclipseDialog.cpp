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

#include "EclipseDialog.hpp"

#include "ProxyInterface.hpp"

#include "EclipsePlatforms.hpp"
#include "ReportDialog.hpp"

#include <QDialog>
#include <QMainWindow>

//Constructor for the Eclipse Report Dialog
PostProcessor::EclipseDialog::EclipseDialog(QMainWindow* aMainWindowPtr,
                                            ProxyInterface* aProxyInterface,
                                            Qt::WindowFlags aFlags /*= Qt::WindowFlags()*/)
   : ReportDialog(aMainWindowPtr, ReportType::cECLIPSE, aProxyInterface, aFlags)
   , mEclipsePlatformsWidget(new EclipsePlatforms)
{
   setWindowTitle("Eclipse Report Definition");

   Build();
   Connect();
}

//Builds the Eclipse Report Dialog
void PostProcessor::EclipseDialog::Build()
{
   QTreeWidgetItem* detectedItem = new QTreeWidgetItem();
   detectedItem->setText(0, "Platforms");
   detectedItem->setData(0, Qt::UserRole, "MoverPlatforms");
   AddItem(detectedItem);
   AddWidget("MoverPlatforms", mEclipsePlatformsWidget); // ownership of mEclipsePlatformsWidget is passed to stackedWidget
}

//Connects Eclipse Platform filter widget to Eclipse Report Dialog
void PostProcessor::EclipseDialog::Connect()
{
   connect(mProxyInterface, &ProxyInterface::PlatformAdded, mEclipsePlatformsWidget, &EclipsePlatforms::AddPlatforms);
   connect(mProxyInterface, &ProxyInterface::PlatformRemoved, mEclipsePlatformsWidget, &EclipsePlatforms::RemovePlatforms);
}

