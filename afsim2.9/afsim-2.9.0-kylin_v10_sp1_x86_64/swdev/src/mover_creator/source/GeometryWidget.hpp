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

#ifndef GEOMETRY_WIDGET_HPP
#define GEOMETRY_WIDGET_HPP

#include <memory>
#include <QWidget>
#include <QJsonObject>

#include "GeometryOptionsDialog.hpp"
#include "GeometrySettingsDialog.hpp"
#include "MoverCreatorTabs.hpp"

class QMenu;
class QTreeWidgetItem;

namespace Designer
{
   class GeometryMassProperties;
   class GeometryGLWidget;
   class EngineTreeWidgetItem;
   class GeometryObject;
}

namespace Ui
{
   class GeometryWidget;
}

namespace Designer
{
   class Vehicle;

   class GeometryWidget : public QWidget
   {
         Q_OBJECT
      public:
         explicit GeometryWidget(QWidget*        aParent,
                                 Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~GeometryWidget() override;

         void ShowGeometryContextMenu(const QPoint& aPoint);
         void Initialize();

         Designer::GeometryGLWidget* GetGeometryGLWidget();

         Ui::GeometryWidget* GetUiGeometryWidget() { return mUIPtr; }

         void LoadVehicleGeometry();
         void ReloadGeometryEngines();

         // Return pointer to mass properties or null if one does not exist
         GeometryMassProperties* GetGeometryMassProperties() { return mGeometryMassProperties; }

         void LoadEngineLengthDiameterThrustOffset(QTreeWidgetItem* aEngineItem, const QString& aEngineTypeStr, const QString& aEngineModelStr);
         //void ChangeTheme(QString aThemeName);

         QTreeWidgetItem* GetSelectedTreeWidgetItem();
         void DeselectTreeWidget();
         void BlockAllSignals(bool aBlock);

         bool eventFilter(QObject* aObj, QEvent* aEvent) override;

         void SaveAeroGeometry(bool aChecked = false);

         // Returns true if geometry file loading is underway
         bool LoadingInProgress() { return mFileLoadingInProgress; }

         // This should be called when transitioning from the current tab to the "moving to" tab
         void TabTransition(Designer::eMoverCreatorTab aMovingToTab);

         void Reset();

      protected:
         void LoadGeometryMassProperties(QJsonObject& aObject, Designer::Vehicle* aVehicle);
         void LoadGeometryPointMasses(QJsonObject& aObject, Designer::Vehicle* aVehicle);
         void LoadGeometryPropulsion(QJsonObject& aObject, Designer::Vehicle* aVehicle);
         void LoadGeometryFuselages(QJsonObject& aObject, Designer::Vehicle* aVehicle);
         void LoadGeometryBodies(QJsonObject& aObject, Designer::Vehicle* aVehicle);
         void LoadGeometryWings(QJsonObject& aObject, Designer::Vehicle* aVehicle);
         void LoadGeometryMajorSurfaces(QJsonObject& aObject, Designer::Vehicle* aVehicle);
         void LoadGeometryMinorSurfaces(QJsonObject& aObject, Designer::Vehicle* aVehicle);
         void LoadGeometryDishes(QJsonObject& aObject, Designer::Vehicle* aVehicle);
         void LoadGeometryNacelles(QJsonObject& aObject, Designer::Vehicle* aVehicle);
         void LoadGeometrySpeedBrakes(QJsonObject& aObject, Designer::Vehicle* aVehicle);
         void LoadGeometryLandingGear(QJsonObject& aObject, Designer::Vehicle* aVehicle);
         void LoadGeometryEngines(QJsonObject& aObject, Designer::Vehicle* aVehicle);

      private slots:
         QTreeWidgetItem* CreateNewGeometryBody(bool aChecked = false);
         QTreeWidgetItem* CreateNewGeometryFuselage(bool aChecked = false);
         QTreeWidgetItem* CreateNewGeometryNacelle(bool aChecked = false);
         QTreeWidgetItem* CreateNewGeometrySurface(bool aChecked = false);
         QTreeWidgetItem* CreateNewGeometrySpeedBrake(bool aChecked = false);
         QTreeWidgetItem* CreateNewGeometryLandingGear(bool aChecked = false);
         QTreeWidgetItem* CreateNewGeometryDish(bool aChecked = false);
         QTreeWidgetItem* CreateNewGeometryPointMass(bool aChecked = false);
         QTreeWidgetItem* CreateNewGeometryEngine(bool aChecked = false);

         //All slots related to the view settings dialog...
         void HandleRevertOptions();
         void HandleMoveToCG_X();
         void HandleMoveToCG_Z();
         void HandleMoveToCG_XZ();
         void HandleMoveToCG_XYZ();
         void HandleMoveFocusRight();
         void HandleMoveFocusLeft();
         void HandleMoveFocusForward();
         void HandleMoveFocusAft();
         void HandleResetFocusPoint();
         void HandleToggleShowPrimaryAxes();
         void HandleToggleShowSelectedCG();
         void HandleToggleShowEngines();
         void HandleToggleShowThrustVectors();
         void HandleToggleShowPointMasses();
         void HandleToggleShowFuelTanks();
         void HandleToggleShowVehicleCG();
         void HandleToggleShowLandingGear();
         void HandleToggleShowSpeedBrakes();
         void HandleHideAeroVectors();
         void HandleShowAllAeroVectors();
         void HandleShowSelectedAeroVectors();
         void HandleToggleShowWireframe();
         void HandleToggleOutputDebugData();
         void HandleAlphaIncrease();
         void HandleAlphaDecrease();
         void HandleBetaIncrease();
         void HandleBetaDecrease();
         void HandleYawRateIncrease();
         void HandleYawRateDecrease();
         void HandlePitchRateIncrease();
         void HandlePitchRateDecrease();
         void HandleRollRateIncrease();
         void HandleRollRateDecrease();
         void HandleSpeedIncrease();
         void HandleSpeedDecrease();
         void HandleSurfaceControlAngleIncrease();
         void HandleSurfaceControlAngleDecrease();
         void HandleSelectNone();
         void HandleSaveFocusPoint();
         void HandleMoveToFocusPoint(int aKey);
         void HandleSettingsChanged();

         void DeleteComponent(bool aChecked = false);

         void ValueChanged();
         void NextButtonClicked();
         void PreviousButtonClicked();
         void ItemChanged(QTreeWidgetItem* aItem, int aColumn);
         void ShowViewOptions();
         void ShowSettings();
         void Output3dModel();

      private:
         QTreeWidgetItem* CreateNewGeometryBodyUsingName(Designer::Vehicle* aVehicle, QString& aName, bool aChecked, bool aNonEditableName, bool aMainBody = false);
         QTreeWidgetItem* CreateNewGeometryFuselageUsingName(Designer::Vehicle* aVehicle, QString& aName, bool aChecked, bool aNonEditableName);
         QTreeWidgetItem* CreateNewGeometryNacelleUsingName(Designer::Vehicle* aVehicle, QString& aName, bool aChecked, bool aNonEditableName);
         QTreeWidgetItem* CreateNewGeometrySurfaceUsingName(Designer::Vehicle* aVehicle, QString& aName, bool aChecked, bool aNonEditableName);
         QTreeWidgetItem* CreateNewGeometryWingUsingName(Designer::Vehicle* aVehicle, QString& aName, bool aChecked, bool aNonEditableName);
         QTreeWidgetItem* CreateNewGeometrySpeedBrakeUsingName(Designer::Vehicle* aVehicle, QString& aName, bool aChecked, bool aNonEditableName);
         QTreeWidgetItem* CreateNewGeometryLandingGearUsingName(Designer::Vehicle* aVehicle, QString& aName, bool aChecked, bool aNonEditableName);
         QTreeWidgetItem* CreateNewGeometryDishUsingName(Designer::Vehicle* aVehicle, QString& aName, bool aChecked, bool aNonEditableName);
         QTreeWidgetItem* CreateNewGeometryPointMassUsingName(Designer::Vehicle* aVehicle, QString& aName, bool aChecked, bool aNonEditableName);
         QTreeWidgetItem* CreateNewGeometryMassPropertiesUsingName(Designer::Vehicle* aVehicle, QString& aName, bool aChecked);
         QTreeWidgetItem* CreateNewGeometryPropulsionDataUsingName(Designer::Vehicle* aVehicle, QString& aName, bool aChecked);
         QTreeWidgetItem* CreateNewGeometryEngineUsingName(Designer::Vehicle* aVehicle, QString& aType, QString& aName, bool aChecked);

         void InitializeViewSettings();
         Vehicle* GetVehicle();
         void SetDefaultSpeedConditions();

         QMenu*   mContextMenu;
         QAction* mAddFinActionPtr         = nullptr;
         QAction* mAddSpeedBrakeActionPtr  = nullptr;
         QAction* mAddLandingGearActionPtr = nullptr;
         QAction* mAddEngineActionPtr      = nullptr;
         QAction* mAddNacelleActionPtr     = nullptr;
         QAction* mAddPodActionPtr         = nullptr;
         QAction* mAddDishActionPtr        = nullptr;
         QAction* mAddPointMassActionPtr   = nullptr;

         Ui::GeometryWidget* mUIPtr;
         void PopulateTreeWidget(const QJsonObject& aObj, QTreeWidgetItem* aItem);
         void CheckForDuplicateNames(QTreeWidgetItem* aItem);
         QString MakeNameUnique(QString aName);
         void AskToMoveToCG();
         void UpdateGeometryObject();
         void SetupEditConnections(QTreeWidgetItem* aItem);

         QJsonObject                             mGeometryJsonObject;
         GeometryMassProperties*                 mGeometryMassProperties = nullptr;
         std::vector<GeometryObject*>            mGeometryObjectList;
         bool                                    mFileLoadingInProgress  = false;

         std::unique_ptr<GeometryOptionsDialog>  mGeometryOptionsPtr     = nullptr;
         std::unique_ptr<GeometrySettingsDialog> mGeometrySettingsPtr    = nullptr;

         bool                                    mRememberMoveToCg       = true;
         bool                                    mAlwaysMoveToCg         = true;

         bool                                    mDirty                  = true;
   };
}
#endif // !GEOMETRY_WIDGET_HPP
