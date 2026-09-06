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

#ifndef CONTROLS_WIDGET_HPP
#define CONTROLS_WIDGET_HPP

#include <memory>

#include <QDoubleValidator>
#include <QJsonObject>
#include <QWidget>

#include "G_LimitSettingsDialog.hpp"

namespace Ui
{
   class ControlsWidget;
}

class G_LimitSettings;

namespace Designer
{
   class ControlsWidget : public QWidget
   {
         Q_OBJECT
      public:
         explicit ControlsWidget(QWidget*        aParent,
                                 Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~ControlsWidget();

         void Initialize();
         void LoadControls();
         void SaveControls();

         void PauseGLAnimation();
         void showEvent(QShowEvent* aEvent);

         bool IsHardwareAutoPilotAvailable();
         bool IsSyntheticPilotAvailable();
         bool IsManualPilotAvailable();
         bool IsGuidanceSystemAvailable();

         void Reset();

      public slots:
         void ControlSystemTypeChanged(const QString& aText);
         void PilotsControllersChanged();
         void ToggleManualPilotSettings();
         void ToggleHardwareAutoPilot();
         void HandleTurnControlChanged(bool aUsesBankToTurnControl);
         void ValueChanged();

         void PilotSelectionsChanged(int aState);
         void LaunchPilotsG_LimitDialog();
         void LaunchHardwareG_LimitDialog();
         void HandleG_LimitResults();

      private:
         void UpdateControlsObject();
         void UpdateVehicleControls();
         void HandleNextButtonPressed();

         Ui::ControlsWidget*                    mUIPtr;
         QDoubleValidator*                      mCASValidator;
         QJsonObject                            mControlsJsonObject;
         std::unique_ptr<G_LimitSettingsDialog> mG_LimitDialogPtr;
   };
}

#endif // !CONTROLS_WIDGET_HPP
