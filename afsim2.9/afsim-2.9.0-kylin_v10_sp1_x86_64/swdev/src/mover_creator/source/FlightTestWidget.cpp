// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "FlightTestWidget.hpp"
#include "ui_FlightTestWidget.h"

#include <QProcess>
#include <QChar>

#include "MoverCreatorEnvironment.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorWidget.hpp"
#include "ScriptGeneratorP6DOF.hpp"
#include "ScriptGeneratorRigidBodySixDOF.hpp"
#include "ScriptGeneratorPointMassSixDOF.hpp"
#include "GeometryObjFile.hpp"

namespace Designer
{

FlightTestWidget::FlightTestWidget(QWidget*        aParent,
                                   Qt::WindowFlags aFlags)
   : QWidget(aParent, aFlags)
   , mUIPtr(new Ui::FlightTestWidget)
{
   mUIPtr->setupUi(this);
   connect(mUIPtr->mLaunchTestButton, &QPushButton::clicked, this, &FlightTestWidget::LaunchTestClicked);
   connect(mUIPtr->mPreviousButton, &QPushButton::clicked, this, &FlightTestWidget::PreviousButtonClicked);
}

FlightTestWidget::~FlightTestWidget()
{
   delete mUIPtr;
}

void FlightTestWidget::LaunchTestClicked()
{
   // Get users settings for scenario
   double lat_deg     = mUIPtr->mLatitudeLineEdit->text().toDouble();
   double lon_deg     = mUIPtr->mLongitudeLineEdit->text().toDouble();
   double alt_ft      = mUIPtr->mAltitudeLineEdit->text().toDouble();
   double heading_deg = mUIPtr->mHeadingLineEdit->text().toDouble();
   double speed_fps   = mUIPtr->mSpeedLineEdit->text().toDouble();

   // Create the flight test file

   if (mUIPtr->mMoverComboBox->currentText() == "P6DOF Mover")
   {
      ScriptGeneratorP6DOF scriptGenerator;
      mFlightTestFileName = scriptGenerator.CreateFlightTestFile(lat_deg, lon_deg, alt_ft, heading_deg, speed_fps);
   }
   else if (mUIPtr->mMoverComboBox->currentText() == "RB6DOF Mover")
   {
      ScriptGeneratorRigidBodySixDOF scriptGenerator;
      mFlightTestFileName = scriptGenerator.CreateFlightTestFile(lat_deg, lon_deg, alt_ft, heading_deg, speed_fps);
   }
   else if (mUIPtr->mMoverComboBox->currentText() == "PM6DOF Mover")
   {
      ScriptGeneratorPointMassSixDOF scriptGenerator;
      mFlightTestFileName = scriptGenerator.CreateFlightTestFile(lat_deg, lon_deg, alt_ft, heading_deg, speed_fps);
   }

   // Launch warlock using flight test configuration
   if (mWarlockProcess)
   {
      mWarlockProcess->terminate();
      delete mWarlockProcess;
   }

   // Write out the warlock settings
   if (mWarlockSettings == nullptr)
   {
      // Could not find the warlock settings file, warn the user and prompt them to install warlock
      QMessageBox::warning(this, "Warlock Not Found", "Notice: Warlock needs to be installed in order to Setup/Tune Controls!", QMessageBox::Ok);
      return;
   }
   // Write out the warlock settings
   if (mWarlockSettings)
   {
      mWarlockSettings->beginGroup("P6DOFTuner");
      mWarlockSettings->setValue("moverCreatorFile", mFlightTestFileName);
   }
   mWarlockSettings->endGroup(); // End P6DOFTuner group
   mWarlockSettings->sync(); // Call this to permanently write the settings before launching warlock

   mWarlockProcess = new QProcess();
   connect(mWarlockProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &FlightTestWidget::HandleProcessFinished);

   QStringList warlockArgs;
   // Use the -cf option (not -icf) for Warlock because we do not want the User's default settings to be overwritten
   // with the settings file provided by MoverCreator.
   QString cfStr = "-cf";
   warlockArgs.push_back(cfStr);
   warlockArgs.push_back(mCustomIniFile);
   warlockArgs.push_back(mFlightTestFileName);
   mWarlockProcess->start(MoverCreatorEnvironment::GetEnvironment().GetWarlockDir(), warlockArgs);

   if (!mWarlockProcess->waitForStarted())
   {
      MoverCreatorMainWindow().CreateErrorBox("The Warlock executable could not be launched. Ensure Warlock is installed before attempting to flight test the vehicle.");
   }
}

void FlightTestWidget::showEvent(QShowEvent* aEvent)
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   MoverCreatorWidget* mcw = mw.GetMoverCreatorWidget();
   if (mcw->GenerateRB6DOFMover())
   {
      mUIPtr->mMoverComboBox->addItem("RB6DOF Mover");
   }
   if (mcw->GeneratePM6DOFMover())
   {
       mUIPtr->mMoverComboBox->addItem("PM6DOF Mover");
   }
   if (mcw->GenerateP6DOFMover())
   {
      mUIPtr->mMoverComboBox->addItem("P6DOF Mover");
   }

   // Setup the warlock settings info
   if (mCustomIniFile == "")
   {
      Vehicle* curVehicle = mw.GetMoverCreatorWidget()->GetVehicle();
      QString vehicleName = QString::fromStdString(curVehicle->GetVehicleName());

      QString configFile = MoverCreatorMainWindow::GetDataPath() + "/FlightTestConfig.ini";
      mCustomIniFile = mw.GetDataPath() + "/AFSIM_Scripts/" + vehicleName + "/FlightTestConfig.ini";
      QFile::copy(configFile, mCustomIniFile);
   }

   if (QFileInfo::exists(mCustomIniFile))
   {
      mWarlockSettings = new QSettings(mCustomIniFile, QSettings::IniFormat, this);
   }

   mUIPtr->mAnimatedGLWidget->AdjustViewingDistance(true);
   mUIPtr->mAnimatedGLWidget->StartAnimationTimer();
}

void FlightTestWidget::PauseGLAnimation()
{
   mUIPtr->mAnimatedGLWidget->PauseAnimationTimer();
}

void FlightTestWidget::PreviousButtonClicked()
{
   MoverCreatorWidget* mcWidget = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget();
   mcWidget->MoveToPreviousTab();
}

void FlightTestWidget::HandleProcessFinished()
{
   // The intent of this function is to reset the mWarlockSettings "moverCreatorFile" back to "".
   // Since Warlock has no way of allowing plugins to handle commandline arguments we need to work
   // around by playing with the settings.  Mover creator sets this key/value and the P6DOF tuner
   // plugin looks for it.  If it is set then the plugin knows that the call to warlock came from
   // Mover Creator and will auto save the tuner file to where Mover Creator tells it to.
   // Otherwise, the P6DOF Tuner will ask the user where to save the file.
   mWarlockSettings->beginGroup("P6DOFTuner");
   mWarlockSettings->setValue("moverCreatorFile", "");
   mWarlockSettings->endGroup(); // End P6DOFTuner group
   mWarlockSettings->sync();     // Call this to permanently write the settings before launching warlock
}

} // namespace Designer
