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

#include "TrajectoryDialog.hpp"

#include "OutputDataOptions.hpp"
#include "TrajectoryParticipants.hpp"

PostProcessor::TrajectoryDialog::TrajectoryDialog(QMainWindow*    aMainWindowPtr,
                                                  ProxyInterface* aProxyInterface,
                                                  Qt::WindowFlags aFlags /*= Qt::WindowFlags()*/)
   : ReportDialog(aMainWindowPtr, ReportType::cTRAJECTORY, aProxyInterface, aFlags)
{
   setWindowTitle("Trajectory Extractor Report Definition");
   Build();
}

void PostProcessor::TrajectoryDialog::Build()
{
   TrajectoryParticipants* participantsWidget = new TrajectoryParticipants();
   QTreeWidgetItem* participantItem = new QTreeWidgetItem();
   participantItem->setText(0, "Participant Options");
   participantItem->setData(0, Qt::UserRole, "ParticipantOptions");
   AddItem(participantItem);
   AddWidget("ParticipantOptions", participantsWidget); // ownership passes to stackedWidget

   OutputDataOptions* dataOptionsWidget = new OutputDataOptions();
   QTreeWidgetItem* dataOptionsItem = new QTreeWidgetItem();
   dataOptionsItem->setText(0, "Output Data Options");
   dataOptionsItem->setData(0, Qt::UserRole, "OutputDataOptions");
   AddItem(dataOptionsItem);
   AddWidget("OutputDataOptions", dataOptionsWidget); // ownership passes to stackedWidget
}
