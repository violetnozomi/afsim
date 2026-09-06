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

#include "DetectionDialog.hpp"

#include <QDialog>
#include <QMainWindow>

#include "DetectedPlatforms.hpp"
#include "DetectingSensors.hpp"
#include "DetectionType.hpp"
#include "OptionWidget.hpp"
#include "ProxyInterface.hpp"
#include "ReportDialog.hpp"


PostProcessor::DetectionDialog::DetectionDialog(QMainWindow*    aMainWindowPtr,
                                                ProxyInterface* aProxyInterface,
                                                Qt::WindowFlags aFlags /*= Qt::WindowFlags()*/)
   : ReportDialog(aMainWindowPtr, ReportType::cDETECTION, aProxyInterface, aFlags)
   , mDetectingSensorsWidget(new DetectingSensors)
   , mDetectedPlatformsWidget(new DetectedPlatforms)
{
   setWindowTitle("Detection Report Definition");
   Build();
   Connect();
}

void PostProcessor::DetectionDialog::Build()
{
   //// Report Type ----------------------------------------
   DetectionType* reportTypeWidget = new DetectionType();
   QTreeWidgetItem* reportTypeItem = new QTreeWidgetItem();
   reportTypeItem->setText(0, "Report Type");
   reportTypeItem->setData(0, Qt::UserRole, "ReportType");
   AddItem(reportTypeItem);
   AddWidget("ReportType", reportTypeWidget);

   // Connect "Report Options" button
   connect(reportTypeWidget, &DetectionType::ReportOptionsPressed, this, &DetectionDialog::SetWidget);

   //// Report Options -------------------------------------
   std::vector<OptionWidget*> optionWidgets = reportTypeWidget->GetOptionWidgets();
   for (auto option : optionWidgets)
   {
      AddWidget(option->GetName(), option);
      connect(option, &OptionWidget::ReportTypePressed, this, [this]() {SetWidget("ReportType"); });
   }

   // Detecting Sensors
   QTreeWidgetItem* detectingItem = new QTreeWidgetItem();
   detectingItem->setText(0, "Detecting Sensors");
   detectingItem->setData(0, Qt::UserRole, "DetectingSensors");
   AddItem(detectingItem);
   AddWidget("DetectingSensors", mDetectingSensorsWidget); // ownership of mDetectingSensorsWidget is passed to stackedWidget

   // Detected Platforms
   QTreeWidgetItem* detectedItem = new QTreeWidgetItem();
   detectedItem->setText(0, "Detected Platforms");
   detectedItem->setData(0, Qt::UserRole, "DetectedPlatforms");
   AddItem(detectedItem);
   AddWidget("DetectedPlatforms", mDetectedPlatformsWidget); // ownership of mDetectedPlatformsWidget is passed to stackedWidget
}

void PostProcessor::DetectionDialog::Connect()
{
   connect(mProxyInterface, &ProxyInterface::PlatformAdded, mDetectedPlatformsWidget, &DetectedPlatforms::AddPlatforms);
   connect(mProxyInterface, &ProxyInterface::PlatformRemoved, mDetectedPlatformsWidget, &DetectedPlatforms::RemovePlatforms);
   connect(mProxyInterface, &ProxyInterface::SensorAdded, mDetectingSensorsWidget, &DetectingSensors::AddSensors);
   connect(mProxyInterface, &ProxyInterface::SensorRemoved, mDetectingSensorsWidget, &DetectingSensors::RemoveSensors);
}
