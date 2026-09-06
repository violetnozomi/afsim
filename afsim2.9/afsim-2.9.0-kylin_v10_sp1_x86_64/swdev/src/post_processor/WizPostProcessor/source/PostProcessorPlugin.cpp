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

#include "PostProcessorPlugin.hpp"

#include <QMenu>
#include <QMessageBox>
#include <QToolBar>

#include "CommunicationDialog.hpp"
#include "DetectionDialog.hpp"
#include "DSV_Dialog.hpp"
#include "EclipseDialog.hpp"
#include "EngagementDialog.hpp"
#include "Project.hpp"
#include "ProjectWorkspace.hpp"
#include "ReportDialog.hpp"
#include "TrajectoryDialog.hpp"
#include "WkfAction.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(PostProcessor::Plugin, "Post Processor", "A tool to generate custom reports from raw CSV data.", "wizard")

PostProcessor::Plugin::Plugin(const QString& aPluginName,
                              const size_t   aUniqueId)
   : wizard::Plugin(aPluginName, aUniqueId)
   , mCommunicationDialog(new PostProcessor::CommunicationDialog(wkfEnv.GetMainWindow(), &mProxyInterface))
   , mDetectionDialog(new DetectionDialog(wkfEnv.GetMainWindow(), &mProxyInterface))
   , mDSV_Dialog(new DSV_Dialog(wkfEnv.GetMainWindow(), &mProxyInterface))
   , mEclipseDialog(new EclipseDialog(wkfEnv.GetMainWindow(), &mProxyInterface))
   , mEngagementDialog(new EngagementDialog(wkfEnv.GetMainWindow(), &mProxyInterface))
   , mTrajectoryDialog(new TrajectoryDialog(wkfEnv.GetMainWindow(), &mProxyInterface))
{

   //Create the menu
   wkf::MainWindow* mainWindowPtr = wkfEnv.GetMainWindow();
   QMenu* toolsMenu = mainWindowPtr->GetMenuByName("Tools");
   QMenu* postProcessorMenu = new QMenu("Post Processor", mainWindowPtr);
   toolsMenu->addMenu(postProcessorMenu);

   mCommunicationReport = new wkf::Action("Communication Report", mainWindowPtr);
   mDetectionReport = new wkf::Action("Detection Report", mainWindowPtr);
   mDSV_Report = new wkf::Action("DSV Report", mainWindowPtr);
   mEclipseReport = new wkf::Action("Eclipse Report", mainWindowPtr);
   mEngagementReport = new wkf::Action("Engagement Report", mainWindowPtr);
   mTrajectoryReport = new wkf::Action("Trajectory Report", mainWindowPtr);

   postProcessorMenu->addAction(mCommunicationReport);
   postProcessorMenu->addAction(mDetectionReport);
   postProcessorMenu->addAction(mDSV_Report);
   postProcessorMenu->addAction(mEclipseReport);
   postProcessorMenu->addAction(mEngagementReport);
   postProcessorMenu->addAction(mTrajectoryReport);

   connect(wizard::ProjectWorkspace::Instance(), &wizard::ProjectWorkspace::ProjectOpened, this, &PostProcessor::Plugin::ProjectOpened);
   connect(wizard::ProjectWorkspace::Instance(), &wizard::ProjectWorkspace::ProjectClosed, this, &PostProcessor::Plugin::ProjectClosed);
   connect(mCommunicationReport, &QAction::triggered, this, &PostProcessor::Plugin::ShowCommunicationDialog);
   connect(mDetectionReport, &QAction::triggered, this, &PostProcessor::Plugin::ShowDetectionDialog);
   connect(mDSV_Report, &QAction::triggered, this, &PostProcessor::Plugin::ShowDSV_Dialog);
   connect(mEclipseReport, &QAction::triggered, this, &PostProcessor::Plugin::ShowEclipseDialog);
   connect(mEngagementReport, &QAction::triggered, this, &PostProcessor::Plugin::ShowEngagementDialog);
   connect(mTrajectoryReport, &QAction::triggered, this, &PostProcessor::Plugin::ShowTrajectoryDialog);
}

QList<wkf::Action*> PostProcessor::Plugin::GetActions() const
{
   QList<wkf::Action*> actList;
   actList << mCommunicationReport
           << mDetectionReport
           << mDSV_Report
           << mEclipseReport
           << mEngagementReport
           << mTrajectoryReport;
   return actList;
}

void PostProcessor::Plugin::ProjectOpened(wizard::Project* aProjectPtr)
{
   // When a project is opened, we want the report actions to be available
   mCommunicationReport->setEnabled(true);
   mDetectionReport->setEnabled(true);
   mDSV_Report->setEnabled(true);
   mEclipseReport->setEnabled(true);
   mEngagementReport->setEnabled(true);

   //TODO: the trajectory report currently is not implemented, there is no reason to enable this option.
   //Once it does get implemented, change this to true.
   mTrajectoryReport->setEnabled(false);
}

void PostProcessor::Plugin::ProjectClosed(wizard::Project* aProjectPtr)
{
   // When a project is closed, we don't want the report actions to be available
   mCommunicationReport->setEnabled(false);
   mDetectionReport->setEnabled(false);
   mDSV_Report->setEnabled(false);
   mEclipseReport->setEnabled(false);
   mEngagementReport->setEnabled(false);
   mTrajectoryReport->setEnabled(false);

}

void PostProcessor::Plugin::ShowCommunicationDialog()
{
   mCommunicationDialog->show();
   mProxyInterface.Update();
}

void PostProcessor::Plugin::ShowDetectionDialog()
{
   mDetectionDialog->show();
   mProxyInterface.Update();
}

void PostProcessor::Plugin::ShowDSV_Dialog()
{
   mDSV_Dialog->show();
   mProxyInterface.Update();
}

void PostProcessor::Plugin::ShowEclipseDialog()
{
   mEclipseDialog->show();
   mProxyInterface.Update();
}

void PostProcessor::Plugin::ShowEngagementDialog()
{
   mEngagementDialog->show();
   mProxyInterface.Update();
}

void PostProcessor::Plugin::ShowTrajectoryDialog()
{
   mTrajectoryDialog->show();
   mProxyInterface.Update();
}
