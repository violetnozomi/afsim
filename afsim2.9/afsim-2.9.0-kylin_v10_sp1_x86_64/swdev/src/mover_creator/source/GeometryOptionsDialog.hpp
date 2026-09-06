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

#ifndef GEOMETRY_OPTIONS_DIALOG_HPP
#define GEOMETRY_OPTIONS_DIALOG_HPP

#include <QDialog>
#include <QGroupBox>
#include <QRadioButton>
#include "MoverCreatorMainWindow.hpp"

class QListWidgetItem;
namespace Ui
{
   class GeometryOptionsDialog;
}

namespace Designer
{
   class GeometryOptionsDialog : public QDialog
   {
         Q_OBJECT
      public:
         explicit GeometryOptionsDialog(QWidget*        aParent,
                                        Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~GeometryOptionsDialog();
         void showEvent(QShowEvent* aEvent);

         void SetShowPrimaryAxes(bool aVal);
         void SetShowSelectedCG(bool aVal);
         void SetShowEngines(bool aVal);
         void SetShowThrustVectors(bool aVal);
         void SetShowPointMasses(bool aVal);
         void SetShowFuelTanks(bool aVal);
         void SetShowVehicleCG(bool aVal);
         void SetShowLandingGear(bool aVal);
         void SetShowSpeedBrakes(bool aVal);
         void SetShowWireframe(bool aVal);
         void SetShowDebugData(bool aVal);
         void SetShowUseDefaults(bool aVal);

         void SetTogglePrimaryAxes();
         void SetToggleShowSelectedCG();
         void SetToggleShowEngines();
         void SetToggleShowThrustVectors();
         void SetToggleShowPointMasses();
         void SetToggleShowFuelTanks();
         void SetToggleShowVehicleCG();
         void SetToggleShowLandingGear();
         void SetToggleShowSpeedBrakes();
         void SetHideAeroVectors();
         void SetShowSelectedAeroVectors();
         void SetShowAllAeroVectors();
         void SetToggleShowWireframe();
         void SetToggleShowDebugData();

      private slots:
         void PageChanged(QListWidgetItem* aCurrent, QListWidgetItem* aPrevious);
         void OkClicked();
         void SetUseDefaults();

      signals:
         void RevertOptions();
         void MoveFocusRight();
         void MoveFocusLeft();
         void MoveFocusForward();
         void MoveFocusAft();
         void ResetFocusPoint();
         void TogglePrimaryAxes();
         void ToggleShowSelectedCG();
         void ToggleShowEngines();
         void ToggleShowThrustVectors();
         void ToggleShowPointMasses();
         void ToggleShowFuelTanks();
         void ToggleShowVehicleCG();
         void ToggleShowLandingGear();
         void ToggleShowSpeedBrakes();
         void HideAeroVectors();
         void ShowAllAeroVectors();
         void ShowSelectedAeroVectors();
         void ToggleShowWireframe();
         void ToggleShowDebugData();

      private:
         Ui::GeometryOptionsDialog* mUIPtr;
   };
}
#endif // !GEOMETRY_OPTIONS_DIALOG_HPP


