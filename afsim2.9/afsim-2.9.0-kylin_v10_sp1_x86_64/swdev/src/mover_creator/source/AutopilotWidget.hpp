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

#ifndef AUTOPILOT_WIDGET_HPP
#define AUTOPILOT_WIDGET_HPP

#include <QWidget>
#include <QDoubleValidator>
#include <QJsonObject>
#include <QSettings>
#include <QProcess>

#include "G_LimitSettingsDialog.hpp"
#include "ScriptGeneratorRigidBodySixDOF.hpp"
#include "ScriptGeneratorPointMassSixDOF.hpp"

namespace Ui
{
   class AutopilotWidget;
}

namespace Designer
{
   class AutopilotWidget : public QWidget
   {
         Q_OBJECT
      public:
         explicit AutopilotWidget(QWidget*        aParent,
                                  Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~AutopilotWidget() override;

         void Initialize();

         void LoadPidPrefs();
         void PauseGLAnimation();
         void SaveAutoPilot();
         void showEvent(QShowEvent* aEvent) override;

         void Reset() { }

      public slots:
         void ToggleHardwareAutoPilot();
         void HandleTuneControlsClickedP6DOF();
         void HandleTuneControlsClickedSixDOF(ScriptGeneratorSixDOF* aScriptGenerator);
         void HandleTuneControlsClickedRB6DOF();
         void HandleTuneControlsClickedPM6DOF();
         void HandleHardwareTuneControlsClicked();
         void HandleBrowseButtonClicked();
         void LoadPrefsFromWarlockSettings();
         void UpdatePidPrefsObject();
         void PreviousButtonClicked();
         void NextButtonClicked();
         void HandleCopyDataButtonClicked();
         void HandleHardwareCopyDataButtonClicked();

         void HandleProcessFinished();

         void LaunchPilotsG_LimitsDialog();
         void LaunchHardwareG_LimitsDialog();
         void HandleG_LimitResults();

      private:
         void ToggleG_LimitButtons();

         QJsonObject mPidPrefsObject;
         Ui::AutopilotWidget* mUIPtr;
         QJsonObject mAutopilotJsonObject;
         std::unique_ptr<QSettings> mWarlockSettings;
         QString mTunerFileName = "";
         std::unique_ptr<QProcess> mWarlockProcess;
         QString mCustomIniFile = "";
         std::unique_ptr<G_LimitSettingsDialog> mG_LimitDialogPtr;

         bool mDirty = false;
   };
}
#endif // !AUTOPILOT_WIDGET_HPP
