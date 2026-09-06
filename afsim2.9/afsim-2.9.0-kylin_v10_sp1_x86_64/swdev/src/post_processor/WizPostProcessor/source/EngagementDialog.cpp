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

#include "EngagementDialog.hpp"
#include "ui_ReportDialog.h"

#include <QFile>
#include <QDialog>
#include <QMainWindow>
#include <QXmlStreamWriter>

#include "ActionTypes.hpp"
#include "EngagementType.hpp"
#include "GeneralFormat.hpp"
#include "GeneralOutput.hpp"
#include "GeneralTime.hpp"
#include "Participants.hpp"


PostProcessor::EngagementDialog::EngagementDialog(QMainWindow*    aMainWindowPtr,
                                                  ProxyInterface* aProxyInterface,
                                                  Qt::WindowFlags aFlags /*= Qt::WindowFlags()*/)
   : ReportDialog(aMainWindowPtr, ReportType::cENGAGEMENT, aProxyInterface, aFlags)
   , mActingPlatformsWidget(new Participants(Participants::ParticipantType::cACTING))
   , mAgainstPlatformsWidget(new Participants(Participants::ParticipantType::cAGAINST))
{
   setWindowTitle("Engagement Report Definition");
   Build();
   Connect();
}

void PostProcessor::EngagementDialog::Build()
{
   // Report Type ----------------------------------------
   EngagementType* reportTypeWidget = new EngagementType();
   QTreeWidgetItem* reportTypeItem = new QTreeWidgetItem();
   reportTypeItem->setText(0, "Report Type");
   reportTypeItem->setData(0, Qt::UserRole, "ReportType");
   AddItem(reportTypeItem);
   AddWidget("ReportType", reportTypeWidget);

   // Connect "Report Options" button
   connect(reportTypeWidget, &EngagementType::ReportOptionsPressed, this, &EngagementDialog::SetWidget);

   // Report Options -------------------------------------
   std::vector<OptionWidget*> optionWidgets = reportTypeWidget->GetOptionWidgets();
   for (auto option : optionWidgets)
   {
      AddWidget(option->GetName(), option);
      connect(option, &OptionWidget::ReportTypePressed, this, [this]() {SetWidget("ReportType"); });
      connect(mProxyInterface, &ProxyInterface::WeaponAdded, option, &OptionWidget::emitWeaponAdded);
      connect(mProxyInterface, &ProxyInterface::WeaponRemoved, option, &OptionWidget::emitWeaponRemoved);
   }

   // Acting Platforms --------------------------------------
   QTreeWidgetItem* actingPlatformsItem = new QTreeWidgetItem();
   actingPlatformsItem->setText(0, "Acting Platforms");
   actingPlatformsItem->setData(0, Qt::UserRole, "ActingPlatforms");
   AddItem(actingPlatformsItem);
   AddWidget("ActingPlatforms", mActingPlatformsWidget); // ownership passes to stackedWidget

   // Against Platforms --------------------------------------
   QTreeWidgetItem* againstPlatformsItem = new QTreeWidgetItem();
   againstPlatformsItem->setText(0, "Against Platforms");
   againstPlatformsItem->setData(0, Qt::UserRole, "AgainstPlatforms");
   AddItem(againstPlatformsItem);
   AddWidget("AgainstPlatforms", mAgainstPlatformsWidget); // ownership passes to stackedWidget

   // Types of Actions -----------------------------------
   QTreeWidgetItem* actionTypesItem = new QTreeWidgetItem();
   actionTypesItem->setText(0, "Types of Actions");
   actionTypesItem->setData(0, Qt::UserRole, "ActionTypes");
   AddItem(actionTypesItem);
   AddWidget("ActionTypes", new PostProcessor::ActionTypes()); // ownership passes to stackedWidget
}

void PostProcessor::EngagementDialog::Connect()
{
   connect(mProxyInterface, &ProxyInterface::PlatformAdded, mActingPlatformsWidget, &Participants::AddPlatforms);
   connect(mProxyInterface, &ProxyInterface::PlatformRemoved, mActingPlatformsWidget, &Participants::RemovePlatforms);

   connect(mProxyInterface, &ProxyInterface::PlatformAdded, mAgainstPlatformsWidget, &Participants::AddPlatforms);
   connect(mProxyInterface, &ProxyInterface::PlatformRemoved, mAgainstPlatformsWidget, &Participants::RemovePlatforms);
}
