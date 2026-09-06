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

#ifndef MOVERCREATORWIDGET_HPP
#define MOVERCREATORWIDGET_HPP

#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QWidget>

#include "MoverCreatorTabs.hpp"
#include "Vehicle.hpp"

namespace Ui
{
   class MoverCreatorWidget;
}

namespace Designer
{
   class AerodynamicsWidget;
   class AutopilotWidget;
   class CalculateAeroDialog;
   class ControlsWidget;
   class EngineDesignerWidget;
   class FlightTestWidget;
   class GeometryWidget;
   class PerformanceWidget;
   class StartDialog;

   class MoverCreatorWidget : public QWidget
   {
         Q_OBJECT
      public:
         explicit MoverCreatorWidget(QWidget*        aParent,
                                     Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~MoverCreatorWidget() override;

         Vehicle* LoadNewVehicle();
         Vehicle* GetVehicle() {return mVehicle;}

         double GetVehicleMach() { return mVehicleMach; }
         void SetVehicleMach(double aMach) { mVehicleMach = aMach; }

         void Initialize();

         StartDialog* GetStartDialog();
         EngineDesignerWidget* GetEngineDesignerWidget();
         ControlsWidget* GetControlsWidget();
         GeometryWidget* GetGeometryWidget();
         AerodynamicsWidget* GetAerodynamicsWidget();
         PerformanceWidget* GetPerformanceWidget();
         AutopilotWidget* GetAutoPilotWidget();
         FlightTestWidget* GetFlightTestWidget();

         void   SetGenerateMovers(bool aGenerateGM, bool aGenerateP6DOF, bool aGenerateRB6D, bool aGeneratePM6D);

         bool   GenerateGuidedMover() const {return mGenerateGuidedMover;}
         bool   GenerateP6DOFMover() const {return mGenerateP6DOFMover;}
         bool   GenerateRB6DOFMover() const {return mGenerateRB6DOFMover; }
         bool   GeneratePM6DOFMover() const {return mGeneratePM6DOFMover; }
         size_t GetMoverCount() const { return mMoverCount; }


         void SetInitialTab(int aTabIndex);

         void MoveBetweenTabs();
         void MoveToNextTab();
         void MoveToPreviousTab();

         void MoveTabs(int aDelta);

         void SetEnableEngineTab(bool aEnable);

         void DisableTabsAfterCurrent();

         // This enables/disable tabs according to the currently selected tab index
         void EnableDisableTabs(int aIndex);

         const int GetCurrentTab();

         void ChangeTheme(QString aThemeName);

         void ResetAllTabs();

         // Vehicle file functions...
         void SetCurrentVehicleFile(QString aFileName);
         std::shared_ptr<QJsonDocument> GetCurrentVehicleFile() { return mCurrentVehicleFile; }
         void SaveCurrentVehicleFile();
         void SaveTempVehicleFile();

         // Engine file functions...
         void SetCurrentEngineFile(QString aFileName);
         std::shared_ptr<QJsonDocument> GetCurrentEngineFile() { return mCurrentEngineFile; }
         void SaveCurrentEngineFile();
         void SetEngineTypeAndModel(QString aType, QString aModel);

         void keyPressEvent(QKeyEvent* aEvent);

         // This is used to "un-select" any selected object
         void ClearSelections();
         void RevertChanges();

         void ShowCalculatingAeroDialog();
         void ShowCalculatingPerformanceDialog();

      public slots:
         void EndCalculatingPerformanceDialog(int aExitCode, QProcess::ExitStatus aStatus);

      private slots:
         void CurrentTabChanged(int aIndex);

      private:
         void SwapMenuShowHideText(QAction* aActionPtr);
         void SwapMenuEnabledDisabledText(QAction* aActionPtr);

         void ToggleShowPrimaryAxes(QAction* aActionPtr);
         void ToggleShowSelectedCG(QAction* aActionPtr);
         void ToggleShowEngines(QAction* aActionPtr);
         void ToggleShowThrustVectors(QAction* aActionPtr);
         void ToggleShowPointMasses(QAction* aActionPtr);
         void ToggleShowFuelTanks(QAction* aActionPtr);
         void ToggleShowVehicleCG(QAction* aActionPtr);
         void ToggleShowLandingGear(QAction* aActionPtr);
         void ToggleShowSpeedBrakes(QAction* aActionPtr);
         void ToggleShowAeroVectors(QAction* aActionPtr);
         void ToggleShowAeroSurfaces(QAction* aActionPtr);
         void ToggleEnableAeroVectorsOnlyForSelectedObject(QAction* aActionPtr);
         void ToggleOutputDebugData(QAction* aActionPtr);

         void SetDefaultSpeedConditions();

         Vehicle*                         mVehicle                  = nullptr;
         Ui::MoverCreatorWidget*          mUIPtr                    = nullptr;

         // Vehicle
         std::shared_ptr<QJsonDocument>   mCurrentVehicleFile       = nullptr;
         QString                          mCurrentVehicleFilePath;

         // Mover
         bool                             mGenerateRB6DOFMover      = true;
         bool                             mGeneratePM6DOFMover      = false;
         bool                             mGenerateP6DOFMover       = false;
         bool                             mGenerateGuidedMover      = false;
         size_t                           mMoverCount               = 0;

         // Engine
         std::shared_ptr<QJsonDocument>   mCurrentEngineFile        = nullptr;
         QString                          mCurrentEngineFilePath;

         std::shared_ptr<QJsonDocument>   mOriginalVehicleFile      = nullptr;
         QString                          mOriginalVehicleFilePath;

         UtVec3dX                         mOrigCG;
         double                           mVehicleMach              = 0.8;
         CalculateAeroDialog*             mCalculateAeroDialog      = nullptr;
   };
}
#endif // !MOVERCREATORWIDGET_HPP
