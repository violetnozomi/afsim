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

#ifndef POSTPROCESSORPLUGIN_HPP
#define POSTPROCESSORPLUGIN_HPP

#include <QFileInfo>
#include <QObject>

#include "Plugin.hpp"
#include "ProxyInterface.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"

namespace PostProcessor
{
class     CommunicationDialog;
class     DetectionDialog;
class     DSV_Dialog;
class     EclipseDialog;
class     EngagementDialog;
class     TrajectoryDialog;

class Plugin : public wizard::Plugin
{
      Q_OBJECT
   public:
      Plugin(const QString& aPluginName,
             const size_t   aUniqueId);
      ~Plugin() override = default;

      QList<wkf::Action*> GetActions() const override;

      void ProjectOpened(wizard::Project* aProjectPtr);
      void ProjectClosed(wizard::Project* aProjectPtr);

   private:
      void ShowEngagementDialog();
      void ShowCommunicationDialog();
      void ShowDetectionDialog();
      void ShowDSV_Dialog();
      void ShowEclipseDialog();
      void ShowTrajectoryDialog();

      ProxyInterface       mProxyInterface;

      CommunicationDialog* mCommunicationDialog;
      DetectionDialog*     mDetectionDialog;
      DSV_Dialog*          mDSV_Dialog;
      EclipseDialog*       mEclipseDialog;
      EngagementDialog*    mEngagementDialog;
      TrajectoryDialog*    mTrajectoryDialog;

      wkf::Action*         mCommunicationReport;
      wkf::Action*         mDetectionReport;
      wkf::Action*         mDSV_Report;
      wkf::Action*         mEclipseReport;
      wkf::Action*         mEngagementReport;
      wkf::Action*         mTrajectoryReport;
};
}


#endif
