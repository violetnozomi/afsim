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

#include "CommunicationDialog.hpp"

#include <QDialog>
#include <QMainWindow>

#include "CommunicationType.hpp"
#include "EngagementType.hpp"
#include "Networks.hpp"
#include "OptionWidget.hpp"
#include "Receivers.hpp"
#include "ReportDialog.hpp"
#include "Transmitters.hpp"


PostProcessor::CommunicationDialog::CommunicationDialog(QMainWindow*    aMainWindowPtr,
                                                        ProxyInterface* aProxyInterface,
                                                        Qt::WindowFlags aFlags /*= Qt::WindowFlags()*/)
   : ReportDialog(aMainWindowPtr, ReportType::cCOMMUNICATION, aProxyInterface, aFlags)
   , mTransmittersWidget(new PostProcessor::Transmitters())
   , mReceiversWidget(new PostProcessor::Receivers())
   , mNetworksWidget(new PostProcessor::Networks())
{
   setWindowTitle("Communication Report Definition");
   Build();
   Connect();
}

void PostProcessor::CommunicationDialog::Build()
{
   //// Report Type ----------------------------------------
   auto reportTypeWidget = new CommunicationType();
   auto reportTypeItem = new QTreeWidgetItem();
   reportTypeItem->setText(0, "Report Type");
   reportTypeItem->setData(0, Qt::UserRole, "ReportType");
   AddItem(reportTypeItem);
   AddWidget("ReportType", reportTypeWidget);

   // Connect "Report Options" button
   connect(reportTypeWidget, &CommunicationType::ReportOptionsPressed, this, &CommunicationDialog::SetWidget);

   // Report Options -------------------------------------
   auto optionWidgets = reportTypeWidget->GetOptionWidgets();
   for (auto option : optionWidgets)
   {
      AddWidget(option->GetName(), option);
      connect(option, &OptionWidget::ReportTypePressed, this, [this]() {SetWidget("ReportType"); });
   }

   // Transmitters
   auto transmittersItem = new QTreeWidgetItem();
   transmittersItem->setText(0, "Transmitters");
   transmittersItem->setData(0, Qt::UserRole, "Transmitters");
   AddItem(transmittersItem);
   AddWidget("Transmitters", mTransmittersWidget); // ownership passes to stackedWidget

   // Receivers
   auto receiversItem = new QTreeWidgetItem();
   receiversItem->setText(0, "Receivers");
   receiversItem->setData(0, Qt::UserRole, "Receivers");
   AddItem(receiversItem);
   AddWidget("Receivers", mReceiversWidget); // ownership passes to stackedWidget

   // Networks
   auto networksItem = new QTreeWidgetItem();
   networksItem->setText(0, "Networks");
   networksItem->setData(0, Qt::UserRole, "Networks");
   AddItem(networksItem);
   AddWidget("Networks", mNetworksWidget); // ownership passes to stackedWidget
}

void PostProcessor::CommunicationDialog::Connect()
{
   connect(mProxyInterface, &ProxyInterface::TransmitterAdded, mTransmittersWidget, &Transmitters::AddTransmitters);
   connect(mProxyInterface, &ProxyInterface::TransmitterRemoved, mTransmittersWidget, &Transmitters::RemoveTransmitters);

   connect(mProxyInterface, &ProxyInterface::ReceiverAdded, mReceiversWidget, &Receivers::AddReceivers);
   connect(mProxyInterface, &ProxyInterface::ReceiverRemoved, mReceiversWidget, &Receivers::RemoveReceivers);
}
