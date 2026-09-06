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
#include <utility>


#include "GeometryWidget.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QDoubleSpinBox>
#include <QFile>
#include <QKeyEvent>
#include <QMenu>
#include <QLabel>
#include <QLineEdit>
#include <QListIterator>

#include "AuditErrorDialog.hpp"
#include "AutopilotWidget.hpp"
#include "ControlsWidget.hpp"
#include "GeometryBody.hpp"
#include "GeometryDish.hpp"
#include "GeometryEngine.hpp"
#include "GeometryFuselage.hpp"
#include "GeometryLandingGear.hpp"
#include "GeometryMassProperties.hpp"
#include "GeometryNacelle.hpp"
#include "GeometryObjFile.hpp"
#include "GeometryPointMass.hpp"
#include "GeometryPropulsionData.hpp"
#include "GeometrySettingsDialog.hpp"
#include "GeometrySpeedBrake.hpp"
#include "GeometrySurface.hpp"
#include "GeometryWing.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorWidget.hpp"
#include "ui_GeometryWidget.h"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtPath.hpp"
#include "Vehicle.hpp"

namespace Designer
{

GeometryWidget::GeometryWidget(QWidget*        aParent,
                               Qt::WindowFlags aFlags /*= Qt::WindowFlags()*/)
   : QWidget(aParent, aFlags)
   , mContextMenu(new QMenu(this))
   , mUIPtr(new Ui::GeometryWidget)
{
   mUIPtr->setupUi(this);
   installEventFilter(this);
}

GeometryWidget::~GeometryWidget()
{
   delete mUIPtr;
}

void GeometryWidget::Initialize()
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   QSettings* settings = mw.GetSettings();

   // Function calls to test treeWidgetItems

   // Region width is 438 px, but only use 410 px to improve appearance when many items are shown
   mUIPtr->mGeometryTreeWidget->setColumnWidth(0, 210);
   mUIPtr->mGeometryTreeWidget->setColumnWidth(1, 150);
   mUIPtr->mGeometryTreeWidget->setColumnWidth(2, 50);

   mUIPtr->mGeometryTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
   mUIPtr->mGeometryTreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
   connect(mUIPtr->mGeometryTreeWidget, &QTreeWidget::customContextMenuRequested, this, &GeometryWidget::ShowGeometryContextMenu);

   // Add control buttons
   connect(mUIPtr->mAddNacelleButton, &QPushButton::clicked, this, &GeometryWidget::CreateNewGeometryNacelle);
   connect(mUIPtr->mAddSpeedBrakeButton, &QPushButton::clicked, this, &GeometryWidget::CreateNewGeometrySpeedBrake);
   connect(mUIPtr->mAddLandingGearButton, &QPushButton::clicked, this, &GeometryWidget::CreateNewGeometryLandingGear);
   connect(mUIPtr->mAddFinButton, &QPushButton::clicked, this, &GeometryWidget::CreateNewGeometrySurface);
   connect(mUIPtr->mAddEnginesButton, &QPushButton::clicked, this, &GeometryWidget::CreateNewGeometryEngine);
   connect(mUIPtr->mAddPodButton, &QPushButton::clicked, this, &GeometryWidget::CreateNewGeometryBody);
   connect(mUIPtr->mAddDishButton, &QPushButton::clicked, this, &GeometryWidget::CreateNewGeometryDish);
   connect(mUIPtr->mAddPointMassButton, &QPushButton::clicked, this, &GeometryWidget::CreateNewGeometryPointMass);
   connect(mUIPtr->mSaveButton, &QPushButton::clicked, this, &GeometryWidget::SaveAeroGeometry);
   connect(mUIPtr->mViewSettingsButton, &QPushButton::clicked, this, &GeometryWidget::ShowViewOptions);
   connect(mUIPtr->mSettingsButton, &QPushButton::clicked, this, &GeometryWidget::ShowSettings);
   connect(mUIPtr->mOutput3dModelButton, &QPushButton::clicked, this, &GeometryWidget::Output3dModel);
   connect(mUIPtr->mNextButton, &QPushButton::clicked, this, &GeometryWidget::NextButtonClicked);
   connect(mUIPtr->mPreviousButton, &QPushButton::clicked, this, &GeometryWidget::PreviousButtonClicked);

   if (settings->contains("UseDefaults"))
   {
      if (settings->value("UseDefaults").toBool())
      {
         settings->setValue("ShowAxis", true);
         settings->setValue("ShowSelectedObjectCG", true);
         settings->setValue("ShowEngines", true);
         settings->setValue("ShowThrustVectors", false);
         settings->setValue("ShowPointMasses", false);
         settings->setValue("ShowFuelTanks", false);
         settings->setValue("ShowVehicleCG", true);
         settings->setValue("ShowLandingGear", true);
         settings->setValue("ShowSpeedBrakes", true);
         settings->setValue("ShowWireframe", false);
         settings->setValue("AeroVectors", "Hide");
         settings->setValue("ShowDebug", false);
      }
   }

   // Connect all the view dialog actions
   InitializeViewSettings();

   QIcon icon;
   QString iconThemeName = QStringLiteral("add");
   icon = QIcon::fromTheme(iconThemeName);

   mUIPtr->mAddFinButton->setIcon(icon);
   mUIPtr->mAddSpeedBrakeButton->setIcon(icon);
   mUIPtr->mAddLandingGearButton->setIcon(icon);
   mUIPtr->mAddEnginesButton->setIcon(icon);
   mUIPtr->mAddNacelleButton->setIcon(icon);
   mUIPtr->mAddDishButton->setIcon(icon);
   mUIPtr->mAddPodButton->setIcon(icon);
   mUIPtr->mAddPointMassButton->setIcon(icon);

   QIcon showIcon;
   QString showIconThemeName = QStringLiteral("show");
   showIcon = QIcon::fromTheme(showIconThemeName);
   mUIPtr->mViewSettingsButton->setIcon(showIcon);

   QIcon settingsIcon;
   QString settingsIconThemeName = QStringLiteral("gears");
   settingsIcon = QIcon::fromTheme(settingsIconThemeName);
   mUIPtr->mSettingsButton->setIcon(settingsIcon);

   connect(mUIPtr->mGeometryTreeWidget, &QTreeWidget::itemChanged, this, &GeometryWidget::ItemChanged);
}

Designer::GeometryGLWidget* GeometryWidget::GetGeometryGLWidget()
{
   return mUIPtr->mGeometryGLWidget;
}

QTreeWidgetItem* GeometryWidget::CreateNewGeometryBody(bool aChecked)
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Vehicle* vehicle = mw.GetMoverCreatorWidget()->GetVehicle();
   QString emptyString;
   QTreeWidgetItem* returnItem = CreateNewGeometryBodyUsingName(vehicle, emptyString, aChecked, false);
   mUIPtr->mGeometryTreeWidget->setCurrentItem(nullptr);
   returnItem->setSelected(true);
   returnItem->setExpanded(true);
   mUIPtr->mGeometryTreeWidget->scrollToBottom();
   Designer::GeometryGLWidget* glObj = GetGeometryGLWidget();
   glObj->Update();
   return returnItem;
}

QTreeWidgetItem* GeometryWidget::CreateNewGeometryFuselage(bool aChecked)
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Vehicle* vehicle = mw.GetMoverCreatorWidget()->GetVehicle();
   QString emptyString;
   QTreeWidgetItem* returnItem = CreateNewGeometryFuselageUsingName(vehicle, emptyString, aChecked, false);
   mUIPtr->mGeometryTreeWidget->setCurrentItem(nullptr);
   returnItem->setSelected(true);
   returnItem->setExpanded(true);
   mUIPtr->mGeometryTreeWidget->scrollToBottom();
   Designer::GeometryGLWidget* glObj = GetGeometryGLWidget();
   glObj->Update();
   return returnItem;
}

QTreeWidgetItem* GeometryWidget::CreateNewGeometryNacelle(bool aChecked)
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Vehicle* vehicle = mw.GetMoverCreatorWidget()->GetVehicle();
   QString emptyString;
   QTreeWidgetItem* returnItem = CreateNewGeometryNacelleUsingName(vehicle, emptyString, aChecked, false);
   mUIPtr->mGeometryTreeWidget->setCurrentItem(nullptr);
   returnItem->setSelected(true);
   returnItem->setExpanded(true);
   mUIPtr->mGeometryTreeWidget->scrollToBottom();
   Designer::GeometryGLWidget* glObj = GetGeometryGLWidget();
   glObj->Update();
   return returnItem;
}

QTreeWidgetItem* GeometryWidget::CreateNewGeometrySurface(bool aChecked)
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Vehicle* vehicle = mw.GetMoverCreatorWidget()->GetVehicle();
   QString emptyString;
   QTreeWidgetItem* returnItem = CreateNewGeometrySurfaceUsingName(vehicle, emptyString, aChecked, false);
   mUIPtr->mGeometryTreeWidget->setCurrentItem(nullptr);
   returnItem->setSelected(true);
   returnItem->setExpanded(true);
   mUIPtr->mGeometryTreeWidget->scrollToBottom();
   Designer::GeometryGLWidget* glObj = GetGeometryGLWidget();
   glObj->Update();
   return returnItem;
}

QTreeWidgetItem* GeometryWidget::CreateNewGeometrySpeedBrake(bool aChecked)
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Vehicle* vehicle = mw.GetMoverCreatorWidget()->GetVehicle();
   QString emptyString;
   QTreeWidgetItem* returnItem = CreateNewGeometrySpeedBrakeUsingName(vehicle, emptyString, aChecked, false);
   mUIPtr->mGeometryTreeWidget->setCurrentItem(nullptr);
   returnItem->setSelected(true);
   returnItem->setExpanded(true);
   mUIPtr->mGeometryTreeWidget->scrollToBottom();
   Designer::GeometryGLWidget* glObj = GetGeometryGLWidget();
   glObj->Update();
   return returnItem;
}

QTreeWidgetItem* GeometryWidget::CreateNewGeometryLandingGear(bool aChecked)
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Vehicle* vehicle = mw.GetMoverCreatorWidget()->GetVehicle();
   QString emptyString;
   QTreeWidgetItem* returnItem = CreateNewGeometryLandingGearUsingName(vehicle, emptyString, aChecked, false);
   mUIPtr->mGeometryTreeWidget->setCurrentItem(nullptr);
   returnItem->setSelected(true);
   returnItem->setExpanded(true);
   mUIPtr->mGeometryTreeWidget->scrollToBottom();
   Designer::GeometryGLWidget* glObj = GetGeometryGLWidget();
   glObj->Update();
   return returnItem;
}

QTreeWidgetItem* GeometryWidget::CreateNewGeometryDish(bool aChecked)
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Vehicle* vehicle = mw.GetMoverCreatorWidget()->GetVehicle();
   QString emptyString;
   QTreeWidgetItem* returnItem = CreateNewGeometryDishUsingName(vehicle, emptyString, aChecked, false);
   mUIPtr->mGeometryTreeWidget->setCurrentItem(nullptr);
   returnItem->setSelected(true);
   returnItem->setExpanded(true);
   mUIPtr->mGeometryTreeWidget->scrollToBottom();
   Designer::GeometryGLWidget* glObj = GetGeometryGLWidget();
   glObj->Update();
   return returnItem;
}

QTreeWidgetItem* GeometryWidget::CreateNewGeometryPointMass(bool aChecked)
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Vehicle* vehicle = mw.GetMoverCreatorWidget()->GetVehicle();
   QString emptyString;
   QTreeWidgetItem* returnItem = CreateNewGeometryPointMassUsingName(vehicle, emptyString, aChecked, false);
   mUIPtr->mGeometryTreeWidget->setCurrentItem(nullptr);
   returnItem->setSelected(true);
   returnItem->setExpanded(true);
   mUIPtr->mGeometryTreeWidget->scrollToBottom();
   Designer::GeometryGLWidget* glObj = GetGeometryGLWidget();
   glObj->Update();
   return returnItem;
}

QTreeWidgetItem* GeometryWidget::CreateNewGeometryEngine(bool aChecked)
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Vehicle* vehicle = mw.GetMoverCreatorWidget()->GetVehicle();
   QString engineType;
   QString engineName;
   QTreeWidgetItem* returnItem = CreateNewGeometryEngineUsingName(vehicle, engineType, engineName, aChecked);
   mUIPtr->mGeometryTreeWidget->setCurrentItem(nullptr);
   returnItem->setSelected(true);
   returnItem->setExpanded(true);
   mUIPtr->mGeometryTreeWidget->scrollToBottom();
   Designer::GeometryGLWidget* glObj = GetGeometryGLWidget();
   glObj->Update();
   return returnItem;
}

QTreeWidgetItem* GeometryWidget::CreateNewGeometryBodyUsingName(Designer::Vehicle* aVehicle, QString& aName, bool aChecked, bool aNonEditableName, bool aMainBody)
{
   // If the name is empty, use a standard name
   if (aName.isEmpty())
   {
      aName = "Pod";
   }

   aName = MakeNameUnique(aName);

   auto newGeometryBody = new GeometryBody(aVehicle);
   mGeometryObjectList.push_back(newGeometryBody);

   auto treeWidgetItem = newGeometryBody->CreateTreeWidgetItem(aName, mUIPtr->mGeometryTreeWidget, aNonEditableName);

   auto index = aVehicle->GetGeometry().AddGeometryObject(newGeometryBody);
   treeWidgetItem->setData(0, Qt::UserRole, QVariant(static_cast<uint>(index)));

   VehicleGeometry& geometry = aVehicle->GetGeometry();

   // If this is the main body, add this to the vehicle propulsion -- this should only be done once
   if (aMainBody)
   {
      if (geometry.AddBody(newGeometryBody) == false)
      {
         throw std::invalid_argument("Cannot have more than one main body in a vehicle geometry");
      }
   }

   auto btnWidget = new QWidget();
   auto btnWidgetLayout = new QHBoxLayout();
   btnWidgetLayout->setSpacing(0);
   btnWidgetLayout->setContentsMargins(0, 0, 0, 0);
   btnWidget->setLayout(btnWidgetLayout);
   auto horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
   auto deleteButton = new QPushButton("");
   QIcon icon;
   QString iconThemeName = QStringLiteral("delete_2");
   icon = QIcon::fromTheme(iconThemeName);
   deleteButton->setIcon(icon);
   deleteButton->setToolTip("Delete Component");
   newGeometryBody->SetDeleteButton(deleteButton);
   connect(deleteButton, &QPushButton::clicked, this, &GeometryWidget::DeleteComponent);
   btnWidgetLayout->addItem(horizontalSpacer);
   btnWidgetLayout->addWidget(deleteButton);
   mUIPtr->mGeometryTreeWidget->setItemWidget(treeWidgetItem, 2, btnWidget);

   SetupEditConnections(treeWidgetItem);

   return treeWidgetItem;
}

QTreeWidgetItem* GeometryWidget::CreateNewGeometryFuselageUsingName(Designer::Vehicle* aVehicle, QString& aName, bool aChecked, bool aNonEditableName)
{
   // If the name is empty, use a standard name
   if (aName.isEmpty())
   {
      aName = "Fuselage";
   }

   aName = MakeNameUnique(aName);

   auto newGeometryFuselage = new GeometryFuselage(aVehicle);
   mGeometryObjectList.push_back(newGeometryFuselage);

   auto treeWidgetItem = newGeometryFuselage->CreateTreeWidgetItem(aName, mUIPtr->mGeometryTreeWidget, aNonEditableName);

   auto index = aVehicle->GetGeometry().AddGeometryObject(newGeometryFuselage);
   treeWidgetItem->setData(0, Qt::UserRole, QVariant(static_cast<uint>(index)));

   VehicleGeometry& geometry = aVehicle->GetGeometry();

   // Add this to the vehicle propulsion -- this should only be done once
   if (geometry.AddFuselage(newGeometryFuselage) == false)
   {
      throw std::invalid_argument("Cannot have more than one fuselage in a vehicle geometry");
   }

   auto btnWidget = new QWidget();
   auto btnWidgetLayout = new QHBoxLayout();
   btnWidgetLayout->setSpacing(0);
   btnWidgetLayout->setContentsMargins(0, 0, 0, 0);
   btnWidget->setLayout(btnWidgetLayout);
   auto horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
   auto deleteButton = new QPushButton("");
   QIcon icon;
   QString iconThemeName = QStringLiteral("delete_2");
   icon = QIcon::fromTheme(iconThemeName);
   deleteButton->setIcon(icon);
   deleteButton->setToolTip("Delete Component");
   newGeometryFuselage->SetDeleteButton(deleteButton);
   connect(deleteButton, &QPushButton::clicked, this, &GeometryWidget::DeleteComponent);
   btnWidgetLayout->addItem(horizontalSpacer);
   btnWidgetLayout->addWidget(deleteButton);
   mUIPtr->mGeometryTreeWidget->setItemWidget(treeWidgetItem, 2, btnWidget);

   SetupEditConnections(treeWidgetItem);

   return treeWidgetItem;
}

QTreeWidgetItem* GeometryWidget::CreateNewGeometryNacelleUsingName(Designer::Vehicle* aVehicle, QString& aName, bool aChecked, bool aNonEditableName)
{
   // If the name is empty, use a standard name
   if (aName.isEmpty())
   {
      aName = "Nacelle";
   }

   aName = MakeNameUnique(aName);

   auto newGeometryNacelle(new GeometryNacelle(aVehicle));
   mGeometryObjectList.push_back(newGeometryNacelle);

   auto treeWidgetItem = newGeometryNacelle->CreateTreeWidgetItem(aName, mUIPtr->mGeometryTreeWidget, aNonEditableName);

   auto index = aVehicle->GetGeometry().AddGeometryObject(newGeometryNacelle);
   treeWidgetItem->setData(0, Qt::UserRole, QVariant(static_cast<uint>(index)));

   //QPushButton* deleteButton = new QPushButton("X");
   auto btnWidget = new QWidget();
   auto btnWidgetLayout = new QHBoxLayout();
   btnWidgetLayout->setSpacing(0);
   btnWidgetLayout->setContentsMargins(0, 0, 0, 0);
   btnWidget->setLayout(btnWidgetLayout);
   auto horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
   auto deleteButton = new QPushButton("");
   QIcon icon;
   QString iconThemeName = QStringLiteral("delete_2");
   icon = QIcon::fromTheme(iconThemeName);
   deleteButton->setIcon(icon);
   deleteButton->setToolTip("Delete Component");
   newGeometryNacelle->SetDeleteButton(deleteButton);
   connect(deleteButton, &QPushButton::clicked, this, &GeometryWidget::DeleteComponent);
   btnWidgetLayout->addItem(horizontalSpacer);
   btnWidgetLayout->addWidget(deleteButton);
   mUIPtr->mGeometryTreeWidget->setItemWidget(treeWidgetItem, 2, btnWidget);

   SetupEditConnections(treeWidgetItem);

   return treeWidgetItem;
}

QTreeWidgetItem* GeometryWidget::CreateNewGeometrySurfaceUsingName(Designer::Vehicle* aVehicle, QString& aName, bool aChecked, bool aNonEditableName)
{
   // If the name is empty, use a standard name
   if (aName.isEmpty())
   {
      aName = "Fin";
   }

   aName = MakeNameUnique(aName);

   auto newGeometrySurface = new GeometrySurface(aVehicle);
   mGeometryObjectList.push_back(newGeometrySurface);

   auto treeWidgetItem = newGeometrySurface->CreateTreeWidgetItem(aName, mUIPtr->mGeometryTreeWidget, aNonEditableName);

   auto index = aVehicle->GetGeometry().AddGeometryObject(newGeometrySurface);
   treeWidgetItem->setData(0, Qt::UserRole, QVariant(static_cast<uint>(index)));

   auto btnWidget = new QWidget();
   auto btnWidgetLayout = new QHBoxLayout();
   btnWidgetLayout->setSpacing(0);
   btnWidgetLayout->setContentsMargins(0, 0, 0, 0);
   btnWidget->setLayout(btnWidgetLayout);
   auto horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
   auto deleteButton = new QPushButton("");
   QIcon icon;
   QString iconThemeName = QStringLiteral("delete_2");
   icon = QIcon::fromTheme(iconThemeName);
   deleteButton->setIcon(icon);
   deleteButton->setToolTip("Delete Component");
   newGeometrySurface->SetDeleteButton(deleteButton);
   connect(deleteButton, &QPushButton::clicked, this, &GeometryWidget::DeleteComponent);
   btnWidgetLayout->addItem(horizontalSpacer);
   btnWidgetLayout->addWidget(deleteButton);
   mUIPtr->mGeometryTreeWidget->setItemWidget(treeWidgetItem, 2, btnWidget);

   SetupEditConnections(treeWidgetItem);

   return treeWidgetItem;
}

QTreeWidgetItem* GeometryWidget::CreateNewGeometryWingUsingName(Designer::Vehicle* aVehicle, QString& aName, bool aChecked, bool aNonEditableName)
{
   // If the name is empty, use a standard name
   if (aName.isEmpty())
   {
      aName = "Wing";
   }

   aName = MakeNameUnique(aName);

   auto newGeometryWing = new GeometryWing(aVehicle);
   mGeometryObjectList.push_back(newGeometryWing);

   auto treeWidgetItem = newGeometryWing->CreateTreeWidgetItem(aName, mUIPtr->mGeometryTreeWidget, aNonEditableName);

   auto index = aVehicle->GetGeometry().AddGeometryObject(newGeometryWing);
   treeWidgetItem->setData(0, Qt::UserRole, QVariant(static_cast<uint>(index)));

   VehicleGeometry& geometry = aVehicle->GetGeometry();

   // Add this to the vehicle propulsion -- this should only be done once
   if (geometry.AddWing(newGeometryWing) == false)
   {
      throw std::invalid_argument("Cannot have more than one wing in a vehicle geometry");
   }

   auto btnWidget = new QWidget();
   auto btnWidgetLayout = new QHBoxLayout();
   btnWidgetLayout->setSpacing(0);
   btnWidgetLayout->setContentsMargins(0, 0, 0, 0);
   btnWidget->setLayout(btnWidgetLayout);
   auto horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
   auto deleteButton = new QPushButton("");
   QIcon icon;
   QString iconThemeName = QStringLiteral("delete_2");
   icon = QIcon::fromTheme(iconThemeName);
   deleteButton->setIcon(icon);
   deleteButton->setToolTip("Delete Component");
   newGeometryWing->SetDeleteButton(deleteButton);
   connect(deleteButton, &QPushButton::clicked, this, &GeometryWidget::DeleteComponent);
   btnWidgetLayout->addItem(horizontalSpacer);
   btnWidgetLayout->addWidget(deleteButton);
   mUIPtr->mGeometryTreeWidget->setItemWidget(treeWidgetItem, 2, btnWidget);

   SetupEditConnections(treeWidgetItem);

   return treeWidgetItem;
}

QTreeWidgetItem* GeometryWidget::CreateNewGeometrySpeedBrakeUsingName(Designer::Vehicle* aVehicle, QString& aName, bool aChecked, bool aNonEditableName)
{
   // If the name is empty, use a standard name
   if (aName.isEmpty())
   {
      aName = "SpeedBrake";
   }

   aName = MakeNameUnique(aName);

   auto newSpeedBrake(new GeometrySpeedBrake(aVehicle));
   mGeometryObjectList.push_back(newSpeedBrake);

   auto treeWidgetItem = newSpeedBrake->CreateTreeWidgetItem(aName, mUIPtr->mGeometryTreeWidget, aNonEditableName);

   auto index = aVehicle->GetGeometry().AddGeometryObject(newSpeedBrake);
   treeWidgetItem->setData(0, Qt::UserRole, QVariant(static_cast<uint>(index)));

   auto btnWidget = new QWidget();
   auto btnWidgetLayout = new QHBoxLayout();
   btnWidgetLayout->setSpacing(0);
   btnWidgetLayout->setContentsMargins(0, 0, 0, 0);
   btnWidget->setLayout(btnWidgetLayout);
   auto horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
   auto deleteButton = new QPushButton("");
   QIcon icon;
   QString iconThemeName = QStringLiteral("delete_2");
   icon = QIcon::fromTheme(iconThemeName);
   deleteButton->setIcon(icon);
   deleteButton->setToolTip("Delete Component");
   newSpeedBrake->SetDeleteButton(deleteButton);
   connect(deleteButton, &QPushButton::clicked, this, &GeometryWidget::DeleteComponent);
   btnWidgetLayout->addItem(horizontalSpacer);
   btnWidgetLayout->addWidget(deleteButton);
   mUIPtr->mGeometryTreeWidget->setItemWidget(treeWidgetItem, 2, btnWidget);

   SetupEditConnections(treeWidgetItem);

   return treeWidgetItem;
}

QTreeWidgetItem* GeometryWidget::CreateNewGeometryLandingGearUsingName(Designer::Vehicle* aVehicle, QString& aName, bool aChecked, bool aNonEditableName)
{
   // If the name is empty, use a standard name
   if (aName.isEmpty())
   {
      aName = "LandingGear";
   }

   aName = MakeNameUnique(aName);

   auto newLandingGear(new GeometryLandingGear(aVehicle));
   mGeometryObjectList.push_back(newLandingGear);

   auto treeWidgetItem = newLandingGear->CreateTreeWidgetItem(aName, mUIPtr->mGeometryTreeWidget, aNonEditableName);

   auto index = aVehicle->GetGeometry().AddGeometryObject(newLandingGear);
   treeWidgetItem->setData(0, Qt::UserRole, QVariant(static_cast<uint>(index)));

   auto btnWidget = new QWidget();
   auto btnWidgetLayout = new QHBoxLayout();
   btnWidgetLayout->setSpacing(0);
   btnWidgetLayout->setContentsMargins(0, 0, 0, 0);
   btnWidget->setLayout(btnWidgetLayout);
   auto horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
   auto deleteButton = new QPushButton("");
   QIcon icon;
   QString iconThemeName = QStringLiteral("delete_2");
   icon = QIcon::fromTheme(iconThemeName);
   deleteButton->setIcon(icon);
   deleteButton->setToolTip("Delete Component");
   newLandingGear->SetDeleteButton(deleteButton);
   connect(deleteButton, &QPushButton::clicked, this, &GeometryWidget::DeleteComponent);
   btnWidgetLayout->addItem(horizontalSpacer);
   btnWidgetLayout->addWidget(deleteButton);
   mUIPtr->mGeometryTreeWidget->setItemWidget(treeWidgetItem, 2, btnWidget);

   SetupEditConnections(treeWidgetItem);

   return treeWidgetItem;
}

QTreeWidgetItem* GeometryWidget::CreateNewGeometryDishUsingName(Designer::Vehicle* aVehicle, QString& aName, bool aChecked, bool aNonEditableName)
{
   // If the name is empty, use a standard name
   if (aName.isEmpty())
   {
      aName = "Dish";
   }

   aName = MakeNameUnique(aName);

   auto newDish = new GeometryDish(aVehicle);
   mGeometryObjectList.push_back(newDish);

   auto treeWidgetItem = newDish->CreateTreeWidgetItem(aName, mUIPtr->mGeometryTreeWidget, aNonEditableName);

   auto index = aVehicle->GetGeometry().AddGeometryObject(newDish);
   treeWidgetItem->setData(0, Qt::UserRole, QVariant(static_cast<uint>(index)));

   //QPushButton* deleteButton = new QPushButton("X");
   auto btnWidget = new QWidget();
   auto btnWidgetLayout = new QHBoxLayout();
   btnWidgetLayout->setSpacing(0);
   btnWidgetLayout->setContentsMargins(0, 0, 0, 0);
   btnWidget->setLayout(btnWidgetLayout);
   auto horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
   auto deleteButton = new QPushButton("");
   QIcon icon;
   QString iconThemeName = QStringLiteral("delete_2");
   icon = QIcon::fromTheme(iconThemeName);
   deleteButton->setIcon(icon);
   deleteButton->setToolTip("Delete Component");
   newDish->SetDeleteButton(deleteButton);
   connect(deleteButton, &QPushButton::clicked, this, &GeometryWidget::DeleteComponent);
   btnWidgetLayout->addItem(horizontalSpacer);
   btnWidgetLayout->addWidget(deleteButton);
   mUIPtr->mGeometryTreeWidget->setItemWidget(treeWidgetItem, 2, btnWidget);

   SetupEditConnections(treeWidgetItem);

   return treeWidgetItem;
}

QTreeWidgetItem* GeometryWidget::CreateNewGeometryPointMassUsingName(Designer::Vehicle* aVehicle, QString& aName, bool aChecked, bool aNonEditableName)
{
   // If the name is empty, use a standard name
   if (aName.isEmpty())
   {
      aName = "Point Mass";
   }

   aName = MakeNameUnique(aName);

   auto newMass = new GeometryPointMass(aVehicle);
   mGeometryObjectList.push_back(newMass);

   auto treeWidgetItem = newMass->CreateTreeWidgetItem(aName, mUIPtr->mGeometryTreeWidget, aNonEditableName);

   auto index = aVehicle->GetGeometry().AddGeometryObject(newMass);
   treeWidgetItem->setData(0, Qt::UserRole, QVariant(static_cast<uint>(index)));

   //QPushButton* deleteButton = new QPushButton("X");
   auto btnWidget = new QWidget();
   auto btnWidgetLayout = new QHBoxLayout();
   btnWidgetLayout->setSpacing(0);
   btnWidgetLayout->setContentsMargins(0, 0, 0, 0);
   btnWidget->setLayout(btnWidgetLayout);
   auto horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
   auto deleteButton = new QPushButton("");
   QIcon icon;
   QString iconThemeName = QStringLiteral("delete_2");
   icon = QIcon::fromTheme(iconThemeName);
   deleteButton->setIcon(icon);
   deleteButton->setToolTip("Delete Component");
   newMass->SetDeleteButton(deleteButton);
   connect(deleteButton, &QPushButton::clicked, this, &GeometryWidget::DeleteComponent);
   btnWidgetLayout->addItem(horizontalSpacer);
   btnWidgetLayout->addWidget(deleteButton);
   mUIPtr->mGeometryTreeWidget->setItemWidget(treeWidgetItem, 2, btnWidget);

   SetupEditConnections(treeWidgetItem);

   return treeWidgetItem;
}

QTreeWidgetItem* GeometryWidget::CreateNewGeometryMassPropertiesUsingName(Designer::Vehicle* aVehicle, QString& aName, bool aChecked)
{
   // If the name is empty, use a standard name
   if (aName.isEmpty())
   {
      aName = "Mass Properties";
   }

   aName = MakeNameUnique(aName);

   auto newMass = new GeometryMassProperties(aVehicle);
   mGeometryObjectList.push_back(newMass);

   if (mGeometryMassProperties != nullptr)
   {
      delete mGeometryMassProperties;
   }

   mGeometryMassProperties = newMass;

   auto treeWidgetItem = newMass->CreateTreeWidgetItem(aName, mUIPtr->mGeometryTreeWidget, true);

   auto index = aVehicle->GetGeometry().AddGeometryObject(newMass);
   treeWidgetItem->setData(0, Qt::UserRole, QVariant(static_cast<uint>(index)));

   VehicleGeometry& geometry = aVehicle->GetGeometry();

   // Add this to the vehicle propulsion -- this should only be done once
   if (geometry.AddMassPropertiesObject(newMass) == false)
   {
      throw std::invalid_argument("Cannot have more than one mass properties in a vehicle geometry");
   }

   auto btnWidget = new QWidget();
   auto btnWidgetLayout = new QHBoxLayout();
   btnWidgetLayout->setSpacing(0);
   btnWidgetLayout->setContentsMargins(0, 0, 0, 0);
   btnWidget->setLayout(btnWidgetLayout);
   auto horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
   auto deleteButton = new QPushButton("");
   QIcon icon;
   QString iconThemeName = QStringLiteral("delete_2");
   icon = QIcon::fromTheme(iconThemeName);
   deleteButton->setIcon(icon);
   deleteButton->setToolTip("Delete Component");
   newMass->SetDeleteButton(deleteButton);
   connect(deleteButton, &QPushButton::clicked, this, &GeometryWidget::DeleteComponent);
   btnWidgetLayout->addItem(horizontalSpacer);
   btnWidgetLayout->addWidget(deleteButton);
   mUIPtr->mGeometryTreeWidget->setItemWidget(treeWidgetItem, 2, btnWidget);

   SetupEditConnections(treeWidgetItem);

   return treeWidgetItem;
}

QTreeWidgetItem* GeometryWidget::CreateNewGeometryPropulsionDataUsingName(Designer::Vehicle* aVehicle, QString& aName, bool aChecked)
{
   // If the name is empty, use a standard name
   if (aName.isEmpty())
   {
      aName = "Propulsion Data";
   }

   aName = MakeNameUnique(aName);

   auto newPropulsion = new GeometryPropulsionData(aVehicle);
   mGeometryObjectList.push_back(newPropulsion);

   auto treeWidgetItem = newPropulsion->CreateTreeWidgetItem(aName, mUIPtr->mGeometryTreeWidget, true);

   auto index = aVehicle->GetGeometry().AddGeometryObject(newPropulsion);

   treeWidgetItem->setData(0, Qt::UserRole, QVariant(static_cast<uint>(index)));

   VehicleGeometry& geometry = aVehicle->GetGeometry();

   // Add this to the vehicle geometry -- this should only be done once.
   if (geometry.AddPropulsionData(newPropulsion) == false)
   {
      throw std::invalid_argument("Cannot have more than one vehicle propulsion in a vehicle geometry");
   }

   //QPushButton* deleteButton = new QPushButton("X");
   auto btnWidget = new QWidget();
   auto btnWidgetLayout = new QHBoxLayout();
   btnWidgetLayout->setSpacing(0);
   btnWidgetLayout->setContentsMargins(0, 0, 0, 0);
   btnWidget->setLayout(btnWidgetLayout);
   auto horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
   //QPixmap deletePixmap = QPixmap(QString::fromUtf8(":/icons/%1/64x64/delete_2.png").arg(mw.GetTheme()));
   //QPushButton* deleteButton = new QPushButton(QIcon(deletePixmap), "");
   auto deleteButton = new QPushButton("");
   QIcon icon;
   QString iconThemeName = QStringLiteral("delete_2");
   icon = QIcon::fromTheme(iconThemeName);
   deleteButton->setIcon(icon);
   deleteButton->setToolTip("Delete Component");
   newPropulsion->SetDeleteButton(deleteButton);
   connect(deleteButton, &QPushButton::clicked, this, &GeometryWidget::DeleteComponent);
   btnWidgetLayout->addItem(horizontalSpacer);
   btnWidgetLayout->addWidget(deleteButton);
   mUIPtr->mGeometryTreeWidget->setItemWidget(treeWidgetItem, 2, btnWidget);

   SetupEditConnections(treeWidgetItem);

   return treeWidgetItem;
}

QTreeWidgetItem* GeometryWidget::CreateNewGeometryEngineUsingName(Designer::Vehicle* aVehicle, QString& aType, QString& aName, bool aChecked)
{
   // If the type is empty, set a default of "Jet"
   if (aType.isEmpty())
   {
      aType = "Jet";
   }

   // If the name is empty, use a standard name
   if (aName.isEmpty())
   {
      aName = "Engine";
   }

   aName = MakeNameUnique(aName);

   auto newEngine = new GeometryEngine(aVehicle);
   mGeometryObjectList.push_back(newEngine);

   auto treeWidgetItem = newEngine->CreateTreeWidgetItem(aType, aName, mUIPtr->mGeometryTreeWidget, false);

   auto index = aVehicle->GetGeometry().AddGeometryObject(newEngine);
   treeWidgetItem->setData(0, Qt::UserRole, QVariant(static_cast<uint>(index)));

   auto btnWidget = new QWidget();
   auto btnWidgetLayout = new QHBoxLayout();
   btnWidgetLayout->setSpacing(0);
   btnWidgetLayout->setContentsMargins(0, 0, 0, 0);
   btnWidget->setLayout(btnWidgetLayout);
   auto horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
   auto deleteButton = new QPushButton("");
   QIcon icon;
   QString iconThemeName = QStringLiteral("delete_2");
   icon = QIcon::fromTheme(iconThemeName);
   deleteButton->setIcon(icon);
   deleteButton->setToolTip("Delete Component");
   newEngine->SetDeleteButton(deleteButton);
   connect(deleteButton, &QPushButton::clicked, this, &GeometryWidget::DeleteComponent);
   btnWidgetLayout->addItem(horizontalSpacer);
   btnWidgetLayout->addWidget(deleteButton);
   mUIPtr->mGeometryTreeWidget->setItemWidget(treeWidgetItem, 2, btnWidget);

   SetupEditConnections(treeWidgetItem);

   return treeWidgetItem;
}

void GeometryWidget::CheckForDuplicateNames(QTreeWidgetItem* aItem)
{
   QString curItemText  = aItem->text(0);
   QString origItemText = curItemText;

   int curVal = 1;
   bool done  = false;
   while (!done)
   {
      QList<QTreeWidgetItem*> surfaceList = mUIPtr->mGeometryTreeWidget->findItems(curItemText, Qt::MatchExactly, 0);

      // The list should always include the item itself, so we look for more than one
      if (surfaceList.size() > 1)
      {
         // Current name is not unique, so modify it and try again
         curItemText = origItemText;
         curItemText.append(QString::number(curVal));
         ++curVal; // Increment the value for next time
         aItem->setText(0, curItemText); // Set the new value

         // Inform the GeometryObject or EngineInstance of the name change

         // First try for a GeometryObject
         MoverCreatorMainWindow& mainWindow = MoverCreatorMainWindow::GetInstance();
         Vehicle* vehicle = mainWindow.GetMoverCreatorWidget()->GetVehicle();
         VehicleGeometry& vehicleGeometry = vehicle->GetGeometry();

         GeometryObject* geometryObj = vehicleGeometry.GetGeometryObjectWithWidget(aItem);
         if (geometryObj != nullptr)
         {
            geometryObj->SetName(curItemText);
         }
      }
      else
      {
         // The list contains a single item (the target item itself),
         // so set the name and update the GLWidget and we are done.

         MoverCreatorMainWindow& mainWindow = MoverCreatorMainWindow::GetInstance();
         Vehicle* vehicle = mainWindow.GetMoverCreatorWidget()->GetVehicle();
         VehicleGeometry& vehicleGeometry = vehicle->GetGeometry();
         GeometryObject* geometryObj = vehicleGeometry.GetGeometryObjectWithWidget(aItem);
         if (geometryObj != nullptr)
         {
            geometryObj->SetName(curItemText);
            GetGeometryGLWidget()->Update();
         }
         done = true;
      }
   }
}

QString GeometryWidget::MakeNameUnique(QString aName)
{
   MoverCreatorMainWindow& mainWindow = MoverCreatorMainWindow::GetInstance();
   Vehicle* vehicle = mainWindow.GetMoverCreatorWidget()->GetVehicle();
   VehicleGeometry& vehicleGeometry = vehicle->GetGeometry();

   QString currentName = std::move(aName);
   QString originalName = currentName;

   int curVal = 1;
   while (true)
   {
      // Check geometry first
      if (vehicleGeometry.NameIsUnique(currentName.toStdString()))
      {
         return currentName;
      }

      // Current name is not unique, so modify it and try again
      currentName = originalName;
      currentName.append(QString::number(curVal));
      ++curVal; // Increment the value for next time
   }

   return currentName;
}


void GeometryWidget::PopulateTreeWidget(const QJsonObject& aObj, QTreeWidgetItem* aItem)
{
   aItem->setText(0, aObj["objectName"].toString());

   if (aItem->childCount())
   {
      // Loop through the children
      QJsonObject componentObj;
      for (int j = 0; j < aItem->childCount(); ++j)
      {
         QTreeWidgetItem* curChildItem = aItem->child(j);
         QString childStr = curChildItem->text(0);
         if (!aObj[childStr].isNull())
         {
            QString loadedVal = "";
            if (aObj[childStr].isDouble())
            {
               loadedVal = QString::number(aObj[childStr].toDouble());

            }
            else if (aObj[childStr].isBool())
            {
               if (aObj[childStr].toBool())
               {
                  loadedVal = "true";
               }
               else
               {
                  loadedVal = "false";
               }
            }
            else
            {
               loadedVal = aObj[childStr].toString();
            }

            if (curChildItem->childCount() == 0)
            {
               // Should have two fields associated with it, one line edit and one label
               auto valueLineEdit = dynamic_cast<QLineEdit*>(mUIPtr->mGeometryTreeWidget->itemWidget(curChildItem, 1));
               if (valueLineEdit)
               {
                  valueLineEdit->setText(loadedVal);
               }
               else  // See if it's a combobox instead
               {
                  auto valueComboBox = dynamic_cast<QComboBox*>(mUIPtr->mGeometryTreeWidget->itemWidget(curChildItem, 1));
                  if (valueComboBox)
                  {
                     valueComboBox->setCurrentText(loadedVal);
                  }
                  else // See if it's a checkbox instead
                  {
                     auto valueCheckBox = dynamic_cast<QCheckBox*>(mUIPtr->mGeometryTreeWidget->itemWidget(curChildItem, 1));
                     if (valueCheckBox)
                     {
                        if (loadedVal.toStdString() == "true")
                        {
                           valueCheckBox->setChecked(true);
                        }
                        else
                        {
                           valueCheckBox->setChecked(false);
                        }
                     }
                     else // See if it's a double spinbox instead
                     {
                        auto valueDoubleSpinBox = dynamic_cast<QDoubleSpinBox*>(mUIPtr->mGeometryTreeWidget->itemWidget(curChildItem, 1));
                        if (valueDoubleSpinBox)
                        {
                           valueDoubleSpinBox->setValue(loadedVal.toDouble());
                        }
                     }
                  }
               }
            }
            else
            {
               // Specifically for reference points that have 3 children
               QJsonObject refPointObj = aObj["Reference Point"].toObject();

               if (refPointObj.isEmpty())
               {
                  // We do not have "Reference Point", but we may have something else

                  // Try for CG Point instead
                  refPointObj = aObj["CG Point"].toObject();

                  if (refPointObj.isEmpty())
                  {
                     // We still have not found a match, but we may have something else

                     // Try for Fuel Tank CG instead
                     refPointObj = aObj["Fuel Tank CG"].toObject();

                     if (refPointObj.isEmpty())
                     {
                        auto outStream = ut::log::error() << "Unknown 3 child item in GeometryWidget::PopulateTreeWidget().";
                        outStream.AddNote() << aObj["objectName"].toString().toStdString() << ": '" << childStr.toStdString() << "'";
                     }
                  }
               }

               for (int k = 0; k < curChildItem->childCount(); ++k)
               {
                  QTreeWidgetItem* refPointChild = curChildItem->child(k);
                  QString curKey = refPointChild->text(0);

                  // Should have two fields associated with it, one line edit and one label
                  auto valueLineEdit = dynamic_cast<QLineEdit*>(mUIPtr->mGeometryTreeWidget->itemWidget(refPointChild, 1));

                  if (refPointObj[curKey].isDouble())
                  {
                     loadedVal = QString::number(refPointObj[curKey].toDouble());
                  }
                  else if (refPointObj[curKey].isBool())
                  {
                     if (refPointObj[curKey].toBool())
                     {
                        loadedVal = "true";
                     }
                     else
                     {
                        loadedVal = "false";
                     }
                  }
                  else
                  {
                     loadedVal = refPointObj[curKey].toString();
                  }
                  valueLineEdit->setText(loadedVal);
               }
            }
         }
      }
   }
}

void GeometryWidget::ShowGeometryContextMenu(const QPoint& aPoint)
{
   mContextMenu->clear();

   auto addFin = new QAction("Add Fin", mContextMenu);
   connect(addFin, SIGNAL(triggered()), this, SLOT(CreateNewGeometrySurface()));
   mContextMenu->addAction(addFin);
   mAddFinActionPtr = addFin;

   auto addSpeedBrake = new QAction("Add Speed Brake", mContextMenu);
   connect(addSpeedBrake, SIGNAL(triggered()), this, SLOT(CreateNewGeometrySpeedBrake()));
   mContextMenu->addAction(addSpeedBrake);
   mAddSpeedBrakeActionPtr = addSpeedBrake;

   auto addGear = new QAction("Add Landing Gear", mContextMenu);
   connect(addGear, SIGNAL(triggered()), this, SLOT(CreateNewGeometryLandingGear()));
   mContextMenu->addAction(addGear);
   mAddLandingGearActionPtr = addGear;

   auto addEngine = new QAction("Add Engine", mContextMenu);
   connect(addEngine, SIGNAL(triggered()), this, SLOT(CreateNewGeometryEngine()));
   mContextMenu->addAction(addEngine);
   mAddEngineActionPtr = addEngine;

   auto addNacelle = new QAction("Add Nacelle", mContextMenu);
   connect(addNacelle, SIGNAL(triggered()), this, SLOT(CreateNewGeometryNacelle()));
   mContextMenu->addAction(addNacelle);
   mAddNacelleActionPtr = addNacelle;

   auto addPod = new QAction("Add Pod", mContextMenu);
   connect(addPod, SIGNAL(triggered()), this, SLOT(CreateNewGeometryBody()));
   mContextMenu->addAction(addPod);
   mAddPodActionPtr = addPod;

   auto addDish = new QAction("Add Dish", mContextMenu);
   connect(addDish, SIGNAL(triggered()), this, SLOT(CreateNewGeometryDish()));
   mContextMenu->addAction(addDish);
   mAddDishActionPtr = addDish;

   auto addPointMass = new QAction("Add Point Mass", mContextMenu);
   connect(addPointMass, SIGNAL(triggered()), this, SLOT(CreateNewGeometryPointMass()));
   mContextMenu->addAction(addPointMass);
   mAddPointMassActionPtr = addPointMass;

   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Designer::Vehicle* vehicle = mw.GetMoverCreatorWidget()->GetVehicle();
   std::string vehicleTypeString = vehicle->GetVehicleType();

   if (vehicleTypeString == "Aircraft")
   {
      mAddSpeedBrakeActionPtr->setDisabled(false);
      mAddLandingGearActionPtr->setDisabled(false);
   }
   else if (vehicleTypeString == "Weapon")
   {
      mAddSpeedBrakeActionPtr->setDisabled(true);
      mAddLandingGearActionPtr->setDisabled(true);
   }

   mContextMenu->move(mUIPtr->mGeometryTreeWidget->mapToGlobal(aPoint));
   mContextMenu->show();
}

void GeometryWidget::SaveAeroGeometry(bool) // aChecked)
{
   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentVehicleFile();
   if (curDoc == nullptr)
   {
      return;
   }

   QJsonObject mainObj = curDoc->object();
   UpdateGeometryObject();
   mainObj["geometry"] = mGeometryJsonObject;
   mainObj["VehicleControlConfiguration"] = GetVehicle()->GetVehicleControlConfigurationString();
   mainObj["VehicleTurnControl"] = (GetVehicle()->UsesBankToTurnControl() ? "Bank-to-Turn" : "Skid-to-Turn");

   curDoc->setObject(mainObj);

   MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->SaveCurrentVehicleFile();
}

void GeometryWidget::TabTransition(Designer::eMoverCreatorTab aMovingToTab)
{
   // This does not need to be checked for null
   MoverCreatorWidget* mcWidget = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget();

   Vehicle* vehicle = mcWidget->GetVehicle();
   if (vehicle == nullptr)
   {
      MoverCreatorMainWindow().CreateErrorBox("No vehicle is available!");
      return;
   }

   bool outputData = false;
   switch (aMovingToTab)
   {
      case Designer::cSETUP_TAB:
      case Designer::cENGINE_TAB:
      case Designer::cCONTROLS_TAB:
      case Designer::cGEOMETRY_TAB:
      case Designer::cPERFORMANCE_TAB:
      case Designer::cAUTOPILOT_TAB:
      case Designer::cFLIGHT_TEST_TAB:
         // Do nothing
         break;

      case Designer::cAERODYNAMICS_TAB:
         outputData = true;
         break;

      default:
         // Do nothing
         break;
   }

   if (outputData & mDirty)
   {
      // Calculate the aerodynamics
      AskToMoveToCG();
      mcWidget->GetVehicle()->CalculateVehicleAerodynamics();
      mDirty = false;
   }
}

void GeometryWidget::Reset()
{
   mUIPtr->mGeometryGLWidget->SetResettingView(true);
}

void GeometryWidget::NextButtonClicked() // aChecked)
{
   MoverCreatorWidget* mcWidget = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget();
   Vehicle* vehicle = mcWidget->GetVehicle();
   if (vehicle != nullptr)
   {
      QStringList errorList = vehicle->Audit();
      if (!errorList.empty())
      {
         //GeometryErrorDialog errDialog(this);
         AuditErrorDialog errorDialog(this);
         errorDialog.SetErrorLabel("Error: The following items must be addressed before vehicle aerodynamics can be calculated:");
         errorDialog.AddErrors(errorList);
         auto outStream = ut::log::error() << "Audit failed in GeometryWidget::NextButtonClicked().";
         for (auto& item : errorList)
         {
            std::string errorStr = item.toStdString();
            outStream.AddNote() << errorStr;
         }
         errorDialog.exec();
      }

      if (errorList.empty())
      {
         // Calculate the aerodynamics if anything changed
         if (mDirty)
         {
            AskToMoveToCG();
            vehicle->CalculateVehicleAerodynamics();
            mDirty = false;
         }

         // Move to the new tab
         mcWidget->MoveToNextTab();
      }
   }
}

void GeometryWidget::AskToMoveToCG()
{
   UtVec3dX cgLocation_ft = mGeometryMassProperties->GetCg();

   if (cgLocation_ft.Magnitude() > 0.01)
   {
      if (mRememberMoveToCg)
      {
         if (mAlwaysMoveToCg)
         {
            //mGeometryMassProperties->SetOriginalCg(cgLocation_ft);
            HandleMoveToCG_XYZ();
         }
      }
      else
      {
         QString infoText;
         QTextStream infoStream(&infoText);
         infoStream.setNumberFlags(QTextStream::ForcePoint | QTextStream::ForceSign);
         infoStream.setRealNumberNotation(QTextStream::FixedNotation);
         infoStream.setRealNumberPrecision(2);
         infoStream << "Original center of gravity location: ";
         infoStream << "{" << cgLocation_ft.X() << ", " << cgLocation_ft.Y() << ", " << cgLocation_ft.Z() << "} ft";

         QString text;
         QTextStream textStream(&text);
         textStream << "If accepted, rotational data will be produced about the center of mass, instead of about the current reference point. ";
         textStream << "The current reference point will be preserved and output as a script comment, if needed for reference.";

         QCheckBox* rememberBox = new QCheckBox("&Remember this choice for this session");

         QMessageBox queryBox;
         queryBox.setIcon(QMessageBox::Question);
         queryBox.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
         queryBox.setInformativeText(infoText);
         queryBox.setText(text);
         queryBox.setCheckBox(rememberBox);
         queryBox.setWindowTitle("Shift center of mass to reference point?");
         QAbstractButton *approveButton = queryBox.addButton(tr("Accept"), QMessageBox::AcceptRole);

         queryBox.exec();

         bool approved = queryBox.clickedButton() == approveButton;
         bool remembered = queryBox.checkBox()->isChecked();

         mAlwaysMoveToCg = approved;
         mRememberMoveToCg = remembered;

         if (approved)
         {
            mGeometryMassProperties->SetOriginalCg(cgLocation_ft);
            HandleMoveToCG_XYZ();
         }
      }
   }
}

void GeometryWidget::PreviousButtonClicked()
{
   MoverCreatorWidget* mcWidget = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget();
   mcWidget->MoveToPreviousTab();
}

void GeometryWidget::LoadVehicleGeometry()
{
   mFileLoadingInProgress = true;

   // Clear out the object and engines list
   for (auto& curObj : mGeometryObjectList)
   {
      delete curObj;
   }
   mGeometryObjectList.clear();
   mGeometryMassProperties = nullptr;

   // Clean out the tree widget
   for (int i = mUIPtr->mGeometryTreeWidget->topLevelItemCount() - 1; i >= 0; --i)
   {
      QTreeWidgetItem* delItem = mUIPtr->mGeometryTreeWidget->takeTopLevelItem(i);
      delete delItem;
   }

   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentVehicleFile();
   if (!curDoc)
   {
      mFileLoadingInProgress = false;
      return;
   }

   QJsonObject mainObj = curDoc->object();
   QString fileName = mainObj["fileName"].toString();

   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();

   std::string vehicleTypeString = mainObj["VehicleType"].toString().toStdString();
   if (vehicleTypeString.empty())
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks a valid type definition!");
      mFileLoadingInProgress = false;
      return;
   }
   else
   {
      Designer::Vehicle* vehicle = mw.GetMoverCreatorWidget()->LoadNewVehicle();
      QString vehicleName = fileName;
      vehicleName.chop(4); //remove ".amc" from the filename string
      vehicle->SetVehicleName(vehicleName.toStdString());

      //vehicle->ReInitialize();
      if (vehicleTypeString == "Aircraft")
      {
         vehicle->SetVehicleType(vehicleTypeString);

         mUIPtr->mAddLandingGearButton->setDisabled(false);
         mUIPtr->mAddSpeedBrakeButton->setDisabled(false);
      }
      else if (vehicleTypeString == "Weapon")
      {
         vehicle->SetVehicleType(vehicleTypeString);

         mUIPtr->mAddLandingGearButton->setDisabled(true);
         mUIPtr->mAddSpeedBrakeButton->setDisabled(false);  // Weapons *can* have speed brakes (like a snake-eye bomb)
      }
      else
      {
         MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks a valid type definition!");
         mFileLoadingInProgress = false;
         return;
      }
   }

   Designer::Vehicle* vehicle = mw.GetMoverCreatorWidget()->GetVehicle();

   std::string vehicleControlEnumString = mainObj["VehicleControlConfiguration"].toString().toStdString();
   if (vehicleControlEnumString.empty())
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks a valid ControlConfiguration definition!");
      mFileLoadingInProgress = false;
      return;
   }
   else
   {
      if (vehicleControlEnumString == "cCONFIG_STABS_ONE_TAIL")
      {
         vehicle->SetVehicleControlConfiguration(Vehicle::VehicleControlConfig::cCONFIG_STABS_ONE_TAIL);
      }
      else if (vehicleControlEnumString == "cCONFIG_STABS_TWO_TAILS")
      {
         vehicle->SetVehicleControlConfiguration(Vehicle::VehicleControlConfig::cCONFIG_STABS_TWO_TAILS);
      }
      else if (vehicleControlEnumString == "cCONFIG_STABS_ONE_VENTRAL_TAIL")
      {
         vehicle->SetVehicleControlConfiguration(Vehicle::VehicleControlConfig::cCONFIG_STABS_ONE_VENTRAL_TAIL);
      }
      else if (vehicleControlEnumString == "cCONFIG_STABS_TWO_VENTRAL_TAILS")
      {
         vehicle->SetVehicleControlConfiguration(Vehicle::VehicleControlConfig::cCONFIG_STABS_TWO_VENTRAL_TAILS);
      }
      else if (vehicleControlEnumString == "cCONFIG_STABS_UPPER_LOWER_TAILS")
      {
         vehicle->SetVehicleControlConfiguration(Vehicle::VehicleControlConfig::cCONFIG_STABS_UPPER_LOWER_TAILS);
      }
      else if (vehicleControlEnumString == "cCONFIG_VTAIL")
      {
         vehicle->SetVehicleControlConfiguration(Vehicle::VehicleControlConfig::cCONFIG_VTAIL);
      }
      else if (vehicleControlEnumString == "cCONFIG_INVERTED_VTAIL")
      {
         vehicle->SetVehicleControlConfiguration(Vehicle::VehicleControlConfig::cCONFIG_INVERTED_VTAIL);
      }
      else if (vehicleControlEnumString == "cCONFIG_CANARDS_ONE_TAIL")
      {
         vehicle->SetVehicleControlConfiguration(Vehicle::VehicleControlConfig::cCONFIG_CANARDS_ONE_TAIL);
      }
      else if (vehicleControlEnumString == "cCONFIG_CANARDS_TWO_TAILS")
      {
         vehicle->SetVehicleControlConfiguration(Vehicle::VehicleControlConfig::cCONFIG_CANARDS_TWO_TAILS);
      }
      else if (vehicleControlEnumString == "cCONFIG_CANARDS_STABS_ONE_TAIL")
      {
         vehicle->SetVehicleControlConfiguration(Vehicle::VehicleControlConfig::cCONFIG_CANARDS_STABS_ONE_TAIL);
      }
      else if (vehicleControlEnumString == "cCONFIG_CANARDS_STABS_TWO_TAILS")
      {
         vehicle->SetVehicleControlConfiguration(Vehicle::VehicleControlConfig::cCONFIG_CANARDS_STABS_TWO_TAILS);
      }
      else if (vehicleControlEnumString == "cCONFIG_TAIL_FINS")
      {
         vehicle->SetVehicleControlConfiguration(Vehicle::VehicleControlConfig::cCONFIG_TAIL_FINS);
      }
      else if (vehicleControlEnumString == "cCONFIG_MID_FINS")
      {
         vehicle->SetVehicleControlConfiguration(Vehicle::VehicleControlConfig::cCONFIG_MID_FINS);
      }
      else if (vehicleControlEnumString == "cCONFIG_CANARD_FINS")
      {
         vehicle->SetVehicleControlConfiguration(Vehicle::VehicleControlConfig::cCONFIG_CANARD_FINS);
      }
      else if (vehicleControlEnumString == "cCONFIG_FLYING_WING")
      {
         vehicle->SetVehicleControlConfiguration(Vehicle::VehicleControlConfig::cCONFIG_FLYING_WING);
      }
      else if (vehicleControlEnumString == "cCONFIG_STABS_FLYING_WING")
      {
         vehicle->SetVehicleControlConfiguration(Vehicle::VehicleControlConfig::cCONFIG_STABS_FLYING_WING);
      }
      else if (vehicleControlEnumString == "cNO_CONTROL")
      {
         vehicle->SetVehicleControlConfiguration(Vehicle::VehicleControlConfig::cNO_CONTROL);
      }
      else
      {
         MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks a valid ControlConfiguration definition!");

         // Since we don't know the control config, set "No Control"
         vehicle->SetVehicleControlConfiguration(Vehicle::VehicleControlConfig::cNO_CONTROL);
      }
   }

   std::string vehicleTurnControlString = mainObj["VehicleTurnControl"].toString().toStdString();

   // --------------------------------------------------------------------------------
   // Note: As of 2.7.0: The following code is to support "legacy" files when the GUI
   // had a combobox to allow the user to change the turn control. If we didn't find
   // an entry for "VehicleTurnControl", we check for the old "mTurnControlComboBox"
   // entry in the controls section of the file. When the file is saved, the new file
   // is written out correctly (it adds a "VehicleTurnControl" and removes the old 
   // "mTurnControlComboBox" entry. Thus, this conversion should only happen once for
   // each vehicle .amc file. This can be taken out at some yet-to-be-determined point
   // in the future when users have had a chance to convert their files.
   if (vehicleTurnControlString.empty())
   {
      if (vehicleControlEnumString != "cNO_CONTROL")
      {
         QJsonObject controls = mainObj["controls"].toObject();
         QString turnControl = controls["mTurnControlComboBox"].toString();
         if (turnControl.isNull())
         {
            MoverCreatorMainWindow().CreateErrorBox("Vehicle lacks a valid Turn Control definition!");
            mFileLoadingInProgress = false;
            return;
         }
         else
         {
            (turnControl == "Bank-to-Turn") ? vehicle->SetBankToTurnControl() : vehicle->SetSkidToTurnControl();
         }
      }
   }
   // --------------------------------------------------------------------------------
   else if (vehicleTurnControlString == "Bank-to-Turn")
   {
      vehicle->SetBankToTurnControl();
   }
   else if (vehicleTurnControlString == "Skid-to-Turn")
   {
      vehicle->SetSkidToTurnControl();
   }
   else
   {
      MoverCreatorMainWindow().CreateErrorBox("Vehicle Turn Control should be either \"Bank-to-Turn\" or \"Skid-to-Turn\"!");
   }

   // Set the default altitude for aero calculations
   double vehicleAltitude_ft = mainObj["aero_testing_altitude"].toDouble();
   if (vehicleAltitude_ft >= 1.0)
   {
      vehicle->SetVehicleAltitude(vehicleAltitude_ft);

      double vehicleTestingMach = mainObj["aero_testing_mach"].toDouble();
      if (vehicleTestingMach >= 0)
      {
         vehicle->SetTestingMach(vehicleTestingMach);
      }

      // Set the maximum Mach for aero calculations
      double vehicleMaxMach = mainObj["aero_max_mach"].toDouble();
      if (vehicleMaxMach >= 0)
      {
         vehicle->SetAeroDataMaxMach(vehicleMaxMach);
      }

      // Set the maximum Beta for aero calculations
      double vehicleMaxBeta = mainObj["aero_max_beta_deg"].toDouble();
      if (vehicleMaxBeta >= 0)
      {
         vehicle->SetAeroDataMaxBeta(vehicleMaxBeta);
      }

      // Set the min/max Alpha for aero calculations
      double vehicleMinAlpha = mainObj["aero_min_alpha_deg"].toDouble();
      double vehicleMaxAlpha = mainObj["aero_max_alpha_deg"].toDouble();
      if (vehicleMinAlpha < vehicleMaxAlpha)
      {
         vehicle->SetAeroDataAlphas(vehicleMinAlpha, vehicleMaxAlpha);
      }
   }

   // Set alpha/beta symmetry
   bool alphaSymmetry = mainObj["aero_alpha_symmetry"].toBool();
   bool betaSymmetry = mainObj["aero_beta_symmetry"].toBool();
   vehicle->SetAeroAlphaBetaSymmetry(alphaSymmetry, betaSymmetry);

   LoadGeometryMassProperties(mainObj, vehicle);
   LoadGeometryPointMasses(mainObj, vehicle);
   LoadGeometryPropulsion(mainObj, vehicle);
   LoadGeometryFuselages(mainObj, vehicle);
   LoadGeometryWings(mainObj, vehicle);
   LoadGeometryMajorSurfaces(mainObj, vehicle);
   LoadGeometryBodies(mainObj, vehicle);
   LoadGeometryMinorSurfaces(mainObj, vehicle);
   LoadGeometryDishes(mainObj, vehicle);
   LoadGeometrySpeedBrakes(mainObj, vehicle);
   LoadGeometryLandingGear(mainObj, vehicle);
   LoadGeometryNacelles(mainObj, vehicle);
   LoadGeometryEngines(mainObj, vehicle);

   mFileLoadingInProgress = false;

   // Need to calc mass and aero data
   vehicle->CalcMassAndInertia();
   vehicle->GenerateAeroDataFromGeometry();
}

void Designer::GeometryWidget::ReloadGeometryEngines()
{
   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentVehicleFile();
   if (curDoc != nullptr)
   {
      QJsonObject mainObj = curDoc->object();

      if (!mainObj["geometry"].isNull())
      {
         QJsonObject geometryObj = mainObj["geometry"].toObject();
         // Loop through each object within the geometry object
         QJsonObject::const_iterator objIter = geometryObj.constBegin();
         QStringList geomKeys = geometryObj.keys();
         int curIdx = 0;
         for (; objIter != geometryObj.constEnd(); ++objIter, ++curIdx)
         {
            if (objIter->isObject())
            {
               QJsonObject curObj = objIter->toObject();
               curObj["objectName"] = geomKeys[curIdx];

               // We're looking for an engine.
               if (!curObj["EngineType"].isNull())
               {
                  // Get the name of the current object (Engine, Engine1, etc)
                  QString objectName = curObj["objectName"].toString();

                  auto items = mUIPtr->mGeometryTreeWidget->findItems(objectName, Qt::MatchExactly);
                  if (items.size() == 1) // There should be exactly one match. If not, do nothing
                  {
                     // Update line edits, etc.
                     LoadEngineLengthDiameterThrustOffset(items.at(0), curObj["EngineType"].toString(), curObj["EngineModel"].toString());
                  }
               }
            }
         }
      }
   }
}

void GeometryWidget::LoadGeometryMassProperties(QJsonObject& aObject, Designer::Vehicle* aVehicle)
{
   if (!aObject["geometry"].isNull())
   {
      QJsonObject geometryObj = aObject["geometry"].toObject();
      // Loop through each object within the geometry object and create the appropriate tree widget item
      QStringList geomKeys = geometryObj.keys();
      int curIdx = 0;
      for (auto objIter = geometryObj.constBegin(); objIter != geometryObj.constEnd(); ++objIter, ++curIdx)
      {
         if (objIter->isObject())
         {
            QJsonObject curObj = objIter->toObject();
            curObj["objectName"] = geomKeys[curIdx];

            // Get the name of the current object
            QString objectName = curObj["objectName"].toString();

            if (!curObj["GeometryObjectType"].isNull()) // Create a new object based on the geometry type
            {
               if (curObj["GeometryObjectType"] == "GeometryMassProperties")
               {
                  PopulateTreeWidget(curObj, CreateNewGeometryMassPropertiesUsingName(aVehicle, objectName, false));
               }
            }
         }
      }
   }
}

void GeometryWidget::LoadGeometryPointMasses(QJsonObject& aObject, Designer::Vehicle* aVehicle)
{
   if (!aObject["geometry"].isNull())
   {
      QJsonObject geometryObj = aObject["geometry"].toObject();
      // Loop through each object within the geometry object and create the appropriate treewidget item
      QStringList geomKeys = geometryObj.keys();
      int curIdx = 0;
      for (auto objIter = geometryObj.constBegin(); objIter != geometryObj.constEnd(); ++objIter, ++curIdx)
      {
         if (objIter->isObject())
         {
            QJsonObject curObj = objIter->toObject();
            curObj["objectName"] = geomKeys[curIdx];

            // Get the name of the current object
            QString objectName = curObj["objectName"].toString();

            if (!curObj["GeometryObjectType"].isNull()) // Create a new object based on the geometry type
            {
               if (curObj["GeometryObjectType"] == "GeometryPointMass")
               {
                  bool nonEditable = false;
                  if (curObj["May Not Be Deleted"].isString())
                  {
                     nonEditable = (curObj["May Not Be Deleted"].toString() == "true") ? true : false;
                  }
                  else if (curObj["May Not Be Deleted"].isBool())
                  {
                     nonEditable = curObj["May Not Be Deleted"].toBool();
                  }
                  PopulateTreeWidget(curObj, CreateNewGeometryPointMassUsingName(aVehicle, objectName, false, nonEditable));
               }
            }
         }
      }
   }
}

void GeometryWidget::LoadGeometryPropulsion(QJsonObject& aObject, Designer::Vehicle* aVehicle)
{
   if (!aObject["geometry"].isNull())
   {
      QJsonObject geometryObj = aObject["geometry"].toObject();
      // Loop through each object within the geometry object and create the appropriate treewidget item
      QStringList geomKeys = geometryObj.keys();
      int curIdx = 0;
      for (auto objIter = geometryObj.constBegin(); objIter != geometryObj.constEnd(); ++objIter, ++curIdx)
      {
         if (objIter->isObject())
         {
            QJsonObject curObj = objIter->toObject();
            curObj["objectName"] = geomKeys[curIdx];

            // Get the name of the current object
            QString objectName = curObj["objectName"].toString();

            if (!curObj["GeometryObjectType"].isNull()) // Create a new object based on the geometry type
            {
               if (curObj["GeometryObjectType"] == "GeometryPropulsionData")
               {
                  PopulateTreeWidget(curObj, CreateNewGeometryPropulsionDataUsingName(aVehicle, objectName, false));
               }
            }
         }
      }
   }
}

void GeometryWidget::LoadGeometryFuselages(QJsonObject& aObject, Designer::Vehicle* aVehicle)
{
   if (!aObject["geometry"].isNull())
   {
      QJsonObject geometryObj = aObject["geometry"].toObject();
      // Loop through each object within the geometry object and create the appropriate treewidget item
      QStringList geomKeys = geometryObj.keys();
      int curIdx = 0;
      for (auto objIter = geometryObj.constBegin(); objIter != geometryObj.constEnd(); ++objIter, ++curIdx)
      {
         if (objIter->isObject())
         {
            QJsonObject curObj = objIter->toObject();
            curObj["objectName"] = geomKeys[curIdx];

            // Get the name of the current object
            QString objectName = curObj["objectName"].toString();

            if (!curObj["GeometryObjectType"].isNull()) // Create a new object based on the geometry type
            {
               if (curObj["GeometryObjectType"] == "GeometryFuselage")
               {
                  bool nonEditable = false;
                  if (curObj["May Not Be Deleted"].isString())
                  {
                     nonEditable = (curObj["May Not Be Deleted"].toString() == "true") ? true : false;
                  }
                  else if (curObj["May Not Be Deleted"].isBool())
                  {
                     nonEditable = curObj["May Not Be Deleted"].toBool();
                  }
                  PopulateTreeWidget(curObj, CreateNewGeometryFuselageUsingName(aVehicle, objectName, false, nonEditable));
               }
            }
         }
      }
   }
}

void GeometryWidget::LoadGeometryBodies(QJsonObject& aObject, Designer::Vehicle* aVehicle)
{
   if (!aObject["geometry"].isNull())
   {
      QJsonObject geometryObj = aObject["geometry"].toObject();
      // Loop through each object within the geometry object and create the appropriate treewidget item
      QStringList geomKeys = geometryObj.keys();
      int curIdx = 0;
      for (auto objIter = geometryObj.constBegin(); objIter != geometryObj.constEnd(); ++objIter, ++curIdx)
      {
         if (objIter->isObject())
         {
            QJsonObject curObj = objIter->toObject();
            curObj["objectName"] = geomKeys[curIdx];

            // Get the name of the current object
            QString objectName = curObj["objectName"].toString();

            if (!curObj["GeometryObjectType"].isNull()) // Create a new object based on the geometry type
            {
               if (curObj["GeometryObjectType"] == "GeometryBody")
               {
                  bool nonEditable = false;
                  if (curObj["May Not Be Deleted"].isString())
                  {
                     nonEditable = (curObj["May Not Be Deleted"].toString() == "true") ? true : false;
                  }
                  else if (curObj["May Not Be Deleted"].isBool())
                  {
                     nonEditable = curObj["May Not Be Deleted"].toBool();
                  }

                  bool mainBody = false;
                  if (curObj["Main Body"].isString())
                  {
                     mainBody = (curObj["Main Body"].toString() == "true") ? true : false;
                  }
                  else if (curObj["Main Body"].isBool())
                  {
                     mainBody = curObj["Main Body"].toBool();
                  }

                  PopulateTreeWidget(curObj, CreateNewGeometryBodyUsingName(aVehicle, objectName, false, nonEditable, mainBody));
               }
            }
         }
      }
   }
}

void GeometryWidget::LoadGeometryWings(QJsonObject& aObject, Designer::Vehicle* aVehicle)
{
   if (!aObject["geometry"].isNull())
   {
      QJsonObject geometryObj = aObject["geometry"].toObject();
      // Loop through each object within the geometry object and create the appropriate treewidget item
      QStringList geomKeys = geometryObj.keys();
      int curIdx = 0;
      for (auto objIter = geometryObj.constBegin(); objIter != geometryObj.constEnd(); ++objIter, ++curIdx)
      {
         if (objIter->isObject())
         {
            QJsonObject curObj = objIter->toObject();
            curObj["objectName"] = geomKeys[curIdx];

            // Get the name of the current object
            QString objectName = curObj["objectName"].toString();

            if (!curObj["GeometryObjectType"].isNull()) // Create a new object based on the geometry type
            {
               if (curObj["GeometryObjectType"] == "GeometryWing")
               {
                  bool nonEditable = false;
                  if (curObj["May Not Be Deleted"].isString())
                  {
                     nonEditable = (curObj["May Not Be Deleted"].toString() == "true") ? true : false;
                  }
                  else if (curObj["May Not Be Deleted"].isBool())
                  {
                     nonEditable = curObj["May Not Be Deleted"].toBool();
                  }
                  PopulateTreeWidget(curObj, CreateNewGeometryWingUsingName(aVehicle, objectName, false, nonEditable));
               }
            }
         }
      }
   }
}

void GeometryWidget::LoadGeometryMajorSurfaces(QJsonObject& aObject, Designer::Vehicle* aVehicle)
{
   if (!aObject["geometry"].isNull())
   {
      QJsonObject geometryObj = aObject["geometry"].toObject();
      // Loop through each object within the geometry object and create the appropriate treewidget item
      QStringList geomKeys = geometryObj.keys();
      int curIdx = 0;
      for (auto objIter = geometryObj.constBegin(); objIter != geometryObj.constEnd(); ++objIter, ++curIdx)
      {
         if (objIter->isObject())
         {
            QJsonObject curObj = objIter->toObject();
            curObj["objectName"] = geomKeys[curIdx];

            // Get the name of the current object
            QString objectName = curObj["objectName"].toString();

            if (!curObj["GeometryObjectType"].isNull()) // Create a new object based on the geometry type
            {
               if (curObj["GeometryObjectType"] == "GeometrySurface")
               {
                  bool nonEditable = false;
                  if (curObj["May Not Be Deleted"].isString())
                  {
                     nonEditable = (curObj["May Not Be Deleted"].toString() == "true") ? true : false;
                  }
                  else if (curObj["May Not Be Deleted"].isBool())
                  {
                     nonEditable = curObj["May Not Be Deleted"].toBool();
                  }
                  if (nonEditable)
                  {
                     PopulateTreeWidget(curObj, CreateNewGeometrySurfaceUsingName(aVehicle, objectName, false, true));
                  }
               }
            }
         }
      }
   }
}

void GeometryWidget::LoadGeometryMinorSurfaces(QJsonObject& aObject, Designer::Vehicle* aVehicle)
{
   if (!aObject["geometry"].isNull())
   {
      QJsonObject geometryObj = aObject["geometry"].toObject();
      // Loop through each object within the geometry object and create the appropriate treewidget item
      QStringList geomKeys = geometryObj.keys();
      int curIdx = 0;
      for (auto objIter = geometryObj.constBegin(); objIter != geometryObj.constEnd(); ++objIter, ++curIdx)
      {
         if (objIter->isObject())
         {
            QJsonObject curObj = objIter->toObject();
            curObj["objectName"] = geomKeys[curIdx];

            // Get the name of the current object
            QString objectName = curObj["objectName"].toString();

            if (!curObj["GeometryObjectType"].isNull()) // Create a new object based on the geometry type
            {
               if (curObj["GeometryObjectType"] == "GeometrySurface")
               {
                  bool nonEditable = false;
                  if (curObj["May Not Be Deleted"].isString())
                  {
                     nonEditable = (curObj["May Not Be Deleted"].toString() == "true") ? true : false;
                  }
                  else if (curObj["May Not Be Deleted"].isBool())
                  {
                     nonEditable = curObj["May Not Be Deleted"].toBool();
                  }
                  if (!nonEditable)
                  {
                     PopulateTreeWidget(curObj, CreateNewGeometrySurfaceUsingName(aVehicle, objectName, false, false));
                  }
               }
            }
         }
      }
   }
}

void GeometryWidget::LoadGeometryDishes(QJsonObject& aObject, Designer::Vehicle* aVehicle)
{
   if (!aObject["geometry"].isNull())
   {
      QJsonObject geometryObj = aObject["geometry"].toObject();
      // Loop through each object within the geometry object and create the appropriate treewidget item
      QStringList geomKeys = geometryObj.keys();
      int curIdx = 0;
      for (auto objIter = geometryObj.constBegin(); objIter != geometryObj.constEnd(); ++objIter, ++curIdx)
      {
         if (objIter->isObject())
         {
            QJsonObject curObj = objIter->toObject();
            curObj["objectName"] = geomKeys[curIdx];

            // Get the name of the current object
            QString objectName = curObj["objectName"].toString();

            if (!curObj["GeometryObjectType"].isNull()) // Create a new object based on the geometry type
            {
               if (curObj["GeometryObjectType"] == "GeometryDish")
               {
                  bool nonEditable = false;
                  if (curObj["May Not Be Deleted"].isString())
                  {
                     nonEditable = (curObj["May Not Be Deleted"].toString() == "true") ? true : false;
                  }
                  else if (curObj["May Not Be Deleted"].isBool())
                  {
                     nonEditable = curObj["May Not Be Deleted"].toBool();
                  }
                  PopulateTreeWidget(curObj, CreateNewGeometryDishUsingName(aVehicle, objectName, false, nonEditable));
               }
            }
         }
      }
   }
}

void GeometryWidget::LoadGeometryNacelles(QJsonObject& aObject, Designer::Vehicle* aVehicle)
{
   if (!aObject["geometry"].isNull())
   {
      QJsonObject geometryObj = aObject["geometry"].toObject();
      // Loop through each object within the geometry object and create the appropriate treewidget item
      QStringList geomKeys = geometryObj.keys();
      int curIdx = 0;
      for (auto objIter = geometryObj.constBegin(); objIter != geometryObj.constEnd(); ++objIter, ++curIdx)
      {
         if (objIter->isObject())
         {
            QJsonObject curObj = objIter->toObject();
            curObj["objectName"] = geomKeys[curIdx];

            // Get the name of the current object
            QString objectName = curObj["objectName"].toString();

            if (!curObj["GeometryObjectType"].isNull()) // Create a new object based on the geometry type
            {
               if (curObj["GeometryObjectType"] == "GeometryNacelle")
               {
                  bool nonEditable = false;
                  if (curObj["May Not Be Deleted"].isString())
                  {
                     nonEditable = (curObj["May Not Be Deleted"].toString() == "true") ? true : false;
                  }
                  else if (curObj["May Not Be Deleted"].isBool())
                  {
                     nonEditable = curObj["May Not Be Deleted"].toBool();
                  }
                  PopulateTreeWidget(curObj, CreateNewGeometryNacelleUsingName(aVehicle, objectName, false, nonEditable));
               }
            }
         }
      }
   }
}

void GeometryWidget::LoadGeometrySpeedBrakes(QJsonObject& aObject, Designer::Vehicle* aVehicle)
{
   if (!aObject["geometry"].isNull())
   {
      QJsonObject geometryObj = aObject["geometry"].toObject();
      // Loop through each object within the geometry object and create the appropriate treewidget item
      QStringList geomKeys = geometryObj.keys();
      int curIdx = 0;
      for (auto objIter = geometryObj.constBegin(); objIter != geometryObj.constEnd(); ++objIter, ++curIdx)
      {
         if (objIter->isObject())
         {
            QJsonObject curObj = objIter->toObject();
            curObj["objectName"] = geomKeys[curIdx];

            // Get the name of the current object
            QString objectName = curObj["objectName"].toString();

            if (!curObj["GeometryObjectType"].isNull()) // Create a new object based on the geometry type
            {
               if (curObj["GeometryObjectType"] == "GeometrySpeedBrake")
               {
                  bool nonEditable = false;
                  if (curObj["May Not Be Deleted"].isString())
                  {
                     nonEditable = (curObj["May Not Be Deleted"].toString() == "true") ? true : false;
                  }
                  else if (curObj["May Not Be Deleted"].isBool())
                  {
                     nonEditable = curObj["May Not Be Deleted"].toBool();
                  }
                  PopulateTreeWidget(curObj, CreateNewGeometrySpeedBrakeUsingName(aVehicle, objectName, false, nonEditable));
               }
            }
         }
      }
   }
}

void GeometryWidget::LoadGeometryLandingGear(QJsonObject& aObject, Designer::Vehicle* aVehicle)
{
   if (!aObject["geometry"].isNull())
   {
      QJsonObject geometryObj = aObject["geometry"].toObject();
      // Loop through each object within the geometry object and create the appropriate treewidget item
      QStringList geomKeys = geometryObj.keys();
      int curIdx = 0;
      for (auto objIter = geometryObj.constBegin(); objIter != geometryObj.constEnd(); ++objIter, ++curIdx)
      {
         if (objIter->isObject())
         {
            QJsonObject curObj = objIter->toObject();
            curObj["objectName"] = geomKeys[curIdx];

            // Get the name of the current object
            QString objectName = curObj["objectName"].toString();

            if (!curObj["GeometryObjectType"].isNull()) // Create a new object based on the geometry type
            {
               if (curObj["GeometryObjectType"] == "GeometryLandingGear")
               {
                  bool nonEditable = false;
                  if (curObj["May Not Be Deleted"].isString())
                  {
                     nonEditable = (curObj["May Not Be Deleted"].toString() == "true") ? true : false;
                  }
                  else if (curObj["May Not Be Deleted"].isBool())
                  {
                     nonEditable = curObj["May Not Be Deleted"].toBool();
                  }
                  PopulateTreeWidget(curObj, CreateNewGeometryLandingGearUsingName(aVehicle, objectName, false, nonEditable));
               }
            }
         }
      }
   }
}

void Designer::GeometryWidget::LoadGeometryEngines(QJsonObject& aObject, Designer::Vehicle* aVehicle)
{
   if (!aObject["geometry"].isNull())
   {
      QJsonObject geometryObj = aObject["geometry"].toObject();
      // Loop through each object within the geometry object and create the appropriate treewidget item
      QStringList geomKeys = geometryObj.keys();
      int curIdx = 0;
      for (auto objIter = geometryObj.constBegin(); objIter != geometryObj.constEnd(); ++objIter, ++curIdx)
      {
         if (objIter->isObject())
         {
            QJsonObject curObj = objIter->toObject();
            curObj["objectName"] = geomKeys[curIdx];

            // Get the name of the current object
            QString objectName = curObj["objectName"].toString();

            if (!curObj["EngineType"].isNull()) // Create a new object based on the engine type
            {
               QString type = curObj["EngineType"].toString();
               bool nonEditable = false;
               if (curObj["May Not Be Deleted"].isString())
               {
                  nonEditable = (curObj["May Not Be Deleted"].toString() == "true") ? true : false;
               }
               else if (curObj["May Not Be Deleted"].isBool())
               {
                  nonEditable = curObj["May Not Be Deleted"].toBool();
               }

               QTreeWidgetItem* engineItem = CreateNewGeometryEngineUsingName(aVehicle, type, objectName, nonEditable);
               PopulateTreeWidget(curObj, engineItem);
               LoadEngineLengthDiameterThrustOffset(engineItem, curObj["EngineType"].toString(), curObj["EngineModel"].toString());
            }
         }
      }
   }
}

void GeometryWidget::DeleteComponent(bool aChecked)
{
   auto deleteButton = dynamic_cast<QPushButton*>(QObject::sender());
   QTreeWidgetItemIterator iter(mUIPtr->mGeometryTreeWidget);
   while (*iter)
   {
      if (mUIPtr->mGeometryTreeWidget->itemWidget(*iter, 2) == deleteButton->parentWidget())
      {
         // Remove geometry object from the list
         QTreeWidgetItem* widgetItem = *iter;

         MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();

         Designer::Vehicle* vehicle = mw.GetMoverCreatorWidget()->GetVehicle();

         // Try geometry
         Designer::VehicleGeometry& vehicleGeometry = vehicle->GetGeometry();
         Designer::GeometryObject* geometryObj = vehicleGeometry.GetGeometryObjectWithWidget(widgetItem);
         if (geometryObj != nullptr)
         {
            auto foundObj = std::find(mGeometryObjectList.begin(), mGeometryObjectList.end(), geometryObj);
            if (foundObj != mGeometryObjectList.end())
            {
               mGeometryObjectList.erase(foundObj);
            }

            // Remove component from the vehicle geometry list
            vehicleGeometry.RemoveGeometryObject(geometryObj);

            // Update Mass Properties
            vehicle->CalcMassAndInertia();

            // Update the GLWidget
            Designer::GeometryGLWidget* glObj = GetGeometryGLWidget();
            glObj->Update();

            delete *iter;
            break;
         }
      }
      ++iter;
   }
}

void GeometryWidget::UpdateGeometryObject()
{
   QJsonObject updatedObj;
   // Loop through the treewidget and get each aero component (and its contents)
   for (int i = 0; i < mUIPtr->mGeometryTreeWidget->topLevelItemCount(); ++i)
   {
      QTreeWidgetItem* curItem = mUIPtr->mGeometryTreeWidget->topLevelItem(i);
      QString componentStr = curItem->text(0);

      if (curItem->childCount())
      {
         // Loop through the children
         QJsonObject componentObj;
         for (int j = 0; j < curItem->childCount(); ++j)
         {
            QTreeWidgetItem* curChildItem = curItem->child(j);
            QString valueStr;
            QString childStr = curChildItem->text(0);
            if (curChildItem->childCount() == 0)
            {
               // Should have two fields associated with it, one line edit and one label
               auto valueLineEdit = dynamic_cast<QLineEdit*>(mUIPtr->mGeometryTreeWidget->itemWidget(curChildItem, 1));
               if (valueLineEdit && !valueLineEdit->isReadOnly())
               {
                  valueStr = valueLineEdit->text();
                  bool isDouble = false;
                  double dblValue = valueLineEdit->text().toDouble(&isDouble);
                  if (isDouble)
                  {
                     componentObj[childStr] = dblValue;
                  }
                  else
                  {
                     componentObj[childStr] = valueStr;
                  }
               }
               else
               {
                  auto valueComboBox = dynamic_cast<QComboBox*>(mUIPtr->mGeometryTreeWidget->itemWidget(curChildItem, 1));
                  if (valueComboBox)
                  {
                     valueStr = valueComboBox->currentText();
                     componentObj[childStr] = valueStr;
                  }
                  else
                  {
                     auto valueCheckBox = dynamic_cast<QCheckBox*>(mUIPtr->mGeometryTreeWidget->itemWidget(curChildItem, 1));
                     if (valueCheckBox)
                     {
                        if (valueCheckBox->isChecked())
                        {
                           componentObj[childStr] = true;
                        }
                        else
                        {
                           //valueStr = "false";
                           componentObj[childStr] = false;
                        }
                     }
                     else
                     {
                        auto valueDoubleSpinBox = dynamic_cast<QDoubleSpinBox*>(mUIPtr->mGeometryTreeWidget->itemWidget(curChildItem, 1));
                        if (valueDoubleSpinBox)
                        {
                           componentObj[childStr] = valueDoubleSpinBox->value();
                        }
                     }
                  }
               }
               //componentObj[childStr] = valueStr;
            }
            else
            {
               // This is a reference point that will have three children
               QJsonObject refPointObj;
               for (int k = 0; k < curChildItem->childCount(); ++k)
               {
                  QTreeWidgetItem* refPointChild = curChildItem->child(k);
                  QString curKey = refPointChild->text(0);
                  QString curValue;
                  // Should have two fields associated with it, one line edit and one label
                  auto valueLineEdit = dynamic_cast<QLineEdit*>(mUIPtr->mGeometryTreeWidget->itemWidget(refPointChild, 1));
                  if (valueLineEdit && !valueLineEdit->isReadOnly())
                  {
                     curValue = valueLineEdit->text();
                     bool isDouble = false;
                     double dblValue = valueLineEdit->text().toDouble(&isDouble);
                     if (isDouble)
                     {
                        refPointObj[curKey] = dblValue;
                     }
                     else
                     {
                        refPointObj[curKey] = curValue;
                     }
                  }
                  //refPointObj[curKey] = curValue;
               }
               componentObj[childStr] = refPointObj;
            }
         }
         updatedObj[componentStr] = componentObj;
      }
   }

   mGeometryJsonObject = updatedObj;
}

void GeometryWidget::SetupEditConnections(QTreeWidgetItem* aItem)
{
   if (aItem->childCount())
   {
      // Loop through the children
      QJsonObject componentObj;
      for (int j = 0; j < aItem->childCount(); ++j)
      {
         QTreeWidgetItem* curChildItem = aItem->child(j);
         QString childStr = curChildItem->text(0);
         if (curChildItem->childCount() == 0)
         {
            // Should have two fields associated with it, one line edit and one label
            auto valueLineEdit = dynamic_cast<QLineEdit*>(mUIPtr->mGeometryTreeWidget->itemWidget(curChildItem, 1));
            if (valueLineEdit)
            {
               connect(valueLineEdit, static_cast<void (QLineEdit::*)()>(&QLineEdit::editingFinished), this, &GeometryWidget::ValueChanged);
            }
            else
            {
               auto valueComboBox = dynamic_cast<QComboBox*>(mUIPtr->mGeometryTreeWidget->itemWidget(curChildItem, 1));
               if (valueComboBox)
               {
                  connect(valueComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &GeometryWidget::ValueChanged);
               }
               else
               {
                  auto valueCheckBox = dynamic_cast<QCheckBox*>(mUIPtr->mGeometryTreeWidget->itemWidget(curChildItem, 1));
                  if (valueCheckBox)
                  {
                     connect(valueCheckBox, static_cast<void (QCheckBox::*)(int)>(&QCheckBox::stateChanged), this, &GeometryWidget::ValueChanged);
                  }
               }
            }
         }
         else
         {
            for (int k = 0; k < curChildItem->childCount(); ++k)
            {
               QTreeWidgetItem* refPointChild = curChildItem->child(k);
               QString curKey = refPointChild->text(0);
               // Should have two fields associated with it, one line edit and one label
               auto valueLineEdit = dynamic_cast<QLineEdit*>(mUIPtr->mGeometryTreeWidget->itemWidget(refPointChild, 1));
               connect(valueLineEdit, static_cast<void (QLineEdit::*)()>(&QLineEdit::editingFinished), this, &GeometryWidget::ValueChanged);
            }
         }
      }
   }
}

void GeometryWidget::ValueChanged()
{
   mDirty = true;
   UpdateGeometryObject();

   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentVehicleFile();

   QJsonObject mainObj = curDoc->object();
   mainObj["geometry"] = mGeometryJsonObject;
   mainObj["VehicleControlConfiguration"] = GetVehicle()->GetVehicleControlConfigurationString();

   curDoc->setObject(mainObj);

   MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->SaveTempVehicleFile();
}

void GeometryWidget::LoadEngineLengthDiameterThrustOffset(QTreeWidgetItem* aEngineItem, const QString& aEngineTypeStr, const QString& aEngineModelStr)
{
   // Need to load the engine's JSON and read in the length and diameter for it
   QString dirPath = MoverCreatorMainWindow::GetDataPath() + "/Engines/";
   dirPath.append(aEngineTypeStr);
   dirPath.append("/" + aEngineModelStr + ".amc");

   QFile jFile(dirPath);
   if (jFile.open(QIODevice::ReadOnly))
   {
      QByteArray fileData = jFile.readAll();
      QJsonDocument curDoc(QJsonDocument::fromJson(fileData));
      QJsonObject docObj = curDoc.object();

      double length              = docObj["Length"].toDouble();
      double diameter            = docObj["Diameter"].toDouble();
      double thrustOffset        = docObj["ThrustOffset"].toDouble();
      double mass                = docObj["Mass"].toDouble();
      double specificImpulseSL   = docObj["SpecificImpulseSeaLevel"].toDouble();
      double specificImpulseVac  = docObj["SpecificImpulseVacuum"].toDouble();
      double rampUpThrust_lbs    = docObj["RatedThrustInitial"].toDouble();
      double rampDownThrust_lbs  = docObj["RatedThrustFinal"].toDouble();
      double rampUpTime_sec      = docObj["RampUpTime"].toDouble();
      double rampDownTime_sec    = docObj["RampDownTime"].toDouble();
      double burnTime_sec        = docObj["BurnTime"].toDouble();
      QString engine_type        = docObj["engine_type"].toString();
      QString altitudeCondition  = docObj["AltitudeCondition"].toString();
      QString massSpecification  = docObj["MassType"].toString();

      double propellantMass_lbs    = 0.0;
      bool   solidPropellantRocket = false;

      // Special case for solid propellant rocket
      if (engine_type == "SolidRocket")
      {
         solidPropellantRocket = true;

         double isp = specificImpulseSL;
         if (QString::compare(altitudeCondition, "Vacuum") == 0)
         {
            isp = specificImpulseVac;
         }

         double massFlowRampUp = rampUpThrust_lbs / isp;
         double massFlowRampDown = rampDownThrust_lbs / isp;

         propellantMass_lbs = 0.5 * ((massFlowRampUp + massFlowRampDown) *
                                     burnTime_sec - (massFlowRampUp * rampDownTime_sec + massFlowRampDown * rampUpTime_sec));
         double rocketTotalMass_lbs = mass;
         double rocketEmptyMass_lbs = mass;

         if (QString::compare(massSpecification, "Empty") == 0)
         {
            rocketTotalMass_lbs = rocketEmptyMass_lbs + propellantMass_lbs;
         }

         // Note: we need to send the rocketTotalMass_lbs, rather than the "mass", which might be the casing mass
         mass = rocketTotalMass_lbs;
      }

      for (int i = 0; i < aEngineItem->childCount(); ++i)
      {
         QTreeWidgetItem* curChildItem = aEngineItem->child(i);
         QString childStr = curChildItem->text(0);
         if (childStr == "Length")
         {
            auto valueLineEdit = dynamic_cast<QLineEdit*>(mUIPtr->mGeometryTreeWidget->itemWidget(curChildItem, 1));
            if (valueLineEdit && length > 0.0)
            {
               valueLineEdit->setText(QString::number(length));
               valueLineEdit->setReadOnly(true);
               valueLineEdit->setToolTip("Edit this value on the Engine tab");
            }
         }
         else if (childStr == "Diameter" && diameter > 0.0)
         {
            auto valueLineEdit = dynamic_cast<QLineEdit*>(mUIPtr->mGeometryTreeWidget->itemWidget(curChildItem, 1));
            if (valueLineEdit)
            {
               valueLineEdit->setText(QString::number(diameter));
               valueLineEdit->setReadOnly(true);
               valueLineEdit->setToolTip("Edit this value on the Engine tab");
            }
         }
         else if (childStr == "Thrust Offset")
         {
            auto valueLineEdit = dynamic_cast<QLineEdit*>(mUIPtr->mGeometryTreeWidget->itemWidget(curChildItem, 1));
            if (valueLineEdit)
            {
               valueLineEdit->setText(QString::number(thrustOffset));
               valueLineEdit->setReadOnly(true);
               valueLineEdit->setToolTip("Edit this value on the Engine tab");
            }
         }
         else if (childStr == "Mass")
         {
            auto valueLineEdit = dynamic_cast<QLineEdit*>(mUIPtr->mGeometryTreeWidget->itemWidget(curChildItem, 1));
            if (valueLineEdit)
            {
               valueLineEdit->setText(QString::number(mass));
               valueLineEdit->setReadOnly(true);
               valueLineEdit->setToolTip("Edit this value on the Engine tab");
            }
         }
         else if (childStr == "Propellant Mass")
         {
            auto valueLineEdit = dynamic_cast<QLineEdit*>(mUIPtr->mGeometryTreeWidget->itemWidget(curChildItem, 1));
            if (valueLineEdit)
            {
               valueLineEdit->setText(QString::number(propellantMass_lbs));
               valueLineEdit->setReadOnly(true);
               valueLineEdit->setToolTip("Edit this value on the Engine tab");

               // If this is not a solid propellant rocket, then hide it
               if (!solidPropellantRocket)
               {
                  curChildItem->setHidden(true);
               }
            }
         }
      }
      MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
      mw.GetMoverCreatorWidget()->SetCurrentEngineFile(dirPath);
   }
}

void GeometryWidget::ItemChanged(QTreeWidgetItem* aItem, int aColumn)
{
   if (aColumn == 0)
   {
      CheckForDuplicateNames(aItem);
   }

}

void GeometryWidget::DeselectTreeWidget()
{

   for (int i = 0; i < mUIPtr->mGeometryTreeWidget->topLevelItemCount(); ++i)
   {
      mUIPtr->mGeometryTreeWidget->clearSelection();
      QTreeWidgetItem* curItem = mUIPtr->mGeometryTreeWidget->topLevelItem(i);
      if (curItem->text(0) == "Mass Properties")
      {
         mUIPtr->mGeometryTreeWidget->setCurrentItem(curItem);
         return;
      }
   }

}

QTreeWidgetItem* GeometryWidget::GetSelectedTreeWidgetItem()
{
   QTreeWidgetItem* returnItem = mUIPtr->mGeometryTreeWidget->currentItem();
   if (returnItem)
   {
      return returnItem;
   }
   else
   {
      for (int i = 0; i < mUIPtr->mGeometryTreeWidget->topLevelItemCount(); ++i)
      {
         QTreeWidgetItem* curItem = mUIPtr->mGeometryTreeWidget->topLevelItem(i);
         if (curItem->childCount())
         {
            for (int j = 0; j < curItem->childCount(); ++j)
            {
               QTreeWidgetItem* curChildItem = curItem->child(j);
               if (curChildItem->isSelected())
               {
                  return curChildItem;
               }
               else if (curChildItem->childCount())  //possibly a ref point
               {
                  for (int k = 0; k < curChildItem->childCount(); ++k)
                  {
                     QTreeWidgetItem* curGrandChildItem = curChildItem->child(k);
                     if (curGrandChildItem->isSelected())
                     {
                        return curGrandChildItem;
                     }
                  }
               }
            }
         }
      }
   }
   return returnItem;
}

void GeometryWidget::BlockAllSignals(bool aBlock)
{
   QList<QWidget*> treeWidgetlList = mUIPtr->mGeometryTreeWidget->findChildren<QWidget*>();
   for (auto& curWidget : treeWidgetlList)
   {
      curWidget->blockSignals(aBlock);
   }
}

bool GeometryWidget::eventFilter(QObject* aObj, QEvent* aEvent)
{
   bool retval{true};
   if (mUIPtr->mGeometryGLWidget->hasFocus())
   {
      auto isKeyEvent = dynamic_cast<QKeyEvent*>(aEvent);
      if (isKeyEvent != nullptr && isKeyEvent->type() == QKeyEvent::KeyRelease)
      {
         int keyPressed = isKeyEvent->key();
         if (isKeyEvent->modifiers() & Qt::ShiftModifier)
         {
            int keyVal = -1;
            switch (keyPressed)
            {
               case Qt::Key_Exclam:
                  keyVal = 0;
                  break;
               case Qt::Key_At:
                  keyVal = 1;
                  break;
               case Qt::Key_NumberSign:
                  keyVal = 2;
                  break;
               case Qt::Key_Dollar:
                  keyVal = 3;
                  break;
               case Qt::Key_Percent:
                  keyVal = 4;
                  break;
               case Qt::Key_AsciiCircum:
                  keyVal = 5;
                  break;
               case Qt::Key_Ampersand:
                  keyVal = 6;
                  break;
               case Qt::Key_Asterisk:
                  keyVal = 7;
                  break;
               case Qt::Key_ParenLeft:
                  keyVal = 8;
                  break;
               case Qt::Key_ParenRight:
                  keyVal = 9;
                  break;
               default:
                  keyVal = -1;
            }
            if (keyVal >= 0)
            {
               HandleMoveToFocusPoint(keyVal);
            }
         }
         else
         {
            switch (keyPressed)
            {
               case Qt::Key_F1:
                  HandleMoveToCG_X();
                  break;
               case Qt::Key_F2:
                  HandleMoveToCG_Z();
                  break;
               case Qt::Key_F3:
                  HandleMoveToCG_XZ();
                  break;
               case Qt::Key_F4:
                  HandleMoveToCG_XYZ();
                  break;
               case Qt::Key_Right:
                  HandleMoveFocusRight();
                  break;
               case Qt::Key_Left:
                  HandleMoveFocusLeft();
                  break;
               case Qt::Key_Up:
                  HandleMoveFocusForward();
                  break;
               case Qt::Key_Down:
                  HandleMoveFocusAft();
                  break;
               case Qt::Key_R:
                  HandleResetFocusPoint();
                  break;
               case Qt::Key_X:
                  mGeometryOptionsPtr->SetTogglePrimaryAxes();
                  HandleToggleShowPrimaryAxes();
                  break;
               case Qt::Key_S:
                  mGeometryOptionsPtr->SetToggleShowSelectedCG();
                  HandleToggleShowSelectedCG();
                  break;
               case Qt::Key_E:
                  mGeometryOptionsPtr->SetToggleShowEngines();
                  HandleToggleShowEngines();
                  break;
               case Qt::Key_T:
                  mGeometryOptionsPtr->SetToggleShowThrustVectors();
                  HandleToggleShowThrustVectors();
                  break;
               case Qt::Key_M:
                  mGeometryOptionsPtr->SetToggleShowPointMasses();
                  HandleToggleShowPointMasses();
                  break;
               case Qt::Key_F:
                  mGeometryOptionsPtr->SetToggleShowFuelTanks();
                  HandleToggleShowFuelTanks();
                  break;
               case Qt::Key_C:
                  mGeometryOptionsPtr->SetToggleShowVehicleCG();
                  HandleToggleShowVehicleCG();
                  break;
               case Qt::Key_G:
                  mGeometryOptionsPtr->SetToggleShowLandingGear();
                  HandleToggleShowLandingGear();
                  break;
               case Qt::Key_B:
                  mGeometryOptionsPtr->SetToggleShowSpeedBrakes();
                  HandleToggleShowSpeedBrakes();
                  break;
               case Qt::Key_H:
                  mGeometryOptionsPtr->SetHideAeroVectors();
                  HandleHideAeroVectors();
                  break;
               case Qt::Key_A:
                  mGeometryOptionsPtr->SetShowAllAeroVectors();
                  HandleShowAllAeroVectors();
                  break;
               case Qt::Key_V:
                  mGeometryOptionsPtr->SetShowSelectedAeroVectors();
                  HandleShowSelectedAeroVectors();
                  break;
               case Qt::Key_F10:
                  mGeometryOptionsPtr->SetToggleShowDebugData();
                  HandleToggleOutputDebugData();
                  break;
               case Qt::Key_2:
                  HandleAlphaIncrease();
                  break;
               case Qt::Key_8:
                  HandleAlphaDecrease();
                  break;
               case Qt::Key_3:
                  HandleBetaIncrease();
                  break;
               case Qt::Key_1:
                  HandleBetaDecrease();
                  break;
               case Qt::Key_Insert:
                  HandleYawRateIncrease();
                  break;
               case Qt::Key_Delete:
                  HandleYawRateDecrease();
                  break;
               case Qt::Key_Home:
                  HandlePitchRateIncrease();
                  break;
               case Qt::Key_End:
                  HandlePitchRateDecrease();
                  break;
               case Qt::Key_PageUp:
                  HandleRollRateIncrease();
                  break;
               case Qt::Key_PageDown:
                  HandleRollRateDecrease();
                  break;
               case Qt::Key_Plus:
                  HandleSpeedIncrease();
                  break;
               case Qt::Key_Minus:
                  HandleSpeedDecrease();
                  break;
               case Qt::Key_Period:
                  HandleSurfaceControlAngleIncrease();
                  break;
               case Qt::Key_Comma:
                  HandleSurfaceControlAngleDecrease();
                  break;
               case Qt::Key_Escape:
                  HandleSelectNone();
                  break;
               case Qt::Key_Space:
                  HandleSaveFocusPoint();
                  break;
            }
         }
      }
      else
      {
         retval = false;
      }

   }
   return retval;
}

Vehicle* GeometryWidget::GetVehicle()
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   return mw.GetMoverCreatorWidget()->GetVehicle();
}

void GeometryWidget::ShowViewOptions()
{
   if (mGeometryOptionsPtr == nullptr)
   {
      mGeometryOptionsPtr = ut::make_unique<GeometryOptionsDialog>(this);
   }

   mGeometryOptionsPtr->show();
}

void GeometryWidget::ShowSettings()
{
   if (mGeometrySettingsPtr == nullptr)
   {
      mGeometrySettingsPtr = ut::make_unique<GeometrySettingsDialog>(this);
      connect(mGeometrySettingsPtr.get(), &GeometrySettingsDialog::close, this, &GeometryWidget::HandleSettingsChanged);
   }

   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentVehicleFile();
   if (!curDoc)
   {
      return;
   }
   QJsonObject mainObj = curDoc->object();

   double vehicleAltitude_ft = mainObj["aero_testing_altitude"].toDouble();
   mGeometrySettingsPtr->SetTestAltitude(vehicleAltitude_ft);
   double vehicleMach = mainObj["aero_testing_mach"].toDouble();
   mGeometrySettingsPtr->SetTestMach(vehicleMach);
   double vehicleMaxAlpha = mainObj["aero_max_alpha_deg"].toDouble();
   mGeometrySettingsPtr->SetMaxAlpha(vehicleMaxAlpha);
   double vehicleMinAlpha = mainObj["aero_min_alpha_deg"].toDouble();
   mGeometrySettingsPtr->SetMinAlpha(vehicleMinAlpha);
   double vehicleMaxBeta = mainObj["aero_max_beta_deg"].toDouble();
   mGeometrySettingsPtr->SetMaxBeta(vehicleMaxBeta);
   double vehicleMaxMach = mainObj["aero_max_mach"].toDouble();
   mGeometrySettingsPtr->SetMaxMach(vehicleMaxMach);
   bool alphaSymmetry = mainObj["aero_alpha_symmetry"].toBool();
   mGeometrySettingsPtr->SetAlphaSymmetry(alphaSymmetry);
   bool betaSymmetry = mainObj["aero_beta_symmetry"].toBool();
   mGeometrySettingsPtr->SetBetaSymmetry(betaSymmetry);

   mGeometrySettingsPtr->show();
}

void GeometryWidget::Output3dModel()
{
   // Output the 3D model set locally and to the site folder
   GeometryObjFile model3d;
   model3d.Output3dModelSet();
}

void GeometryWidget::HandleSettingsChanged()
{
   mDirty = true;

   double testAlt       = mGeometrySettingsPtr->GetAltitude();
   double testMach      = mGeometrySettingsPtr->GetTestMach();
   double minAlpha      = mGeometrySettingsPtr->GetMinAlpha();
   double maxAlpha      = mGeometrySettingsPtr->GetMaxAlpha();
   double maxMach       = mGeometrySettingsPtr->GetMaxMach();
   double maxBeta       = mGeometrySettingsPtr->GetMaxBeta();
   bool   alphaSymmetry = mGeometrySettingsPtr->GetAlphaSymmetry();
   bool   betaSymmetry  = mGeometrySettingsPtr->GetBetaSymmetry();

   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Designer::Vehicle* vehicle = mw.GetMoverCreatorWidget()->GetVehicle();

   vehicle->SetVehicleAltitude(testAlt);

   if (testMach >= 0)
   {
      vehicle->SetTestingMach(testMach);
   }

   // Set the maximum Mach for aero calculations
   if (maxMach >= 0)
   {
      vehicle->SetAeroDataMaxMach(maxMach);
   }

   // Set the maximum Beta for aero calculations
   if (maxBeta >= 0)
   {
      vehicle->SetAeroDataMaxBeta(maxBeta);
   }

   // Set the min/max Alpha for aero calculations
   if (minAlpha < maxAlpha)
   {
      vehicle->SetAeroDataAlphas(minAlpha, maxAlpha);
   }

   vehicle->SetAeroAlphaBetaSymmetry(alphaSymmetry, betaSymmetry);

   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentVehicleFile();
   if (!curDoc)
   {
      return;
   }
   QJsonObject mainObj = curDoc->object();
   UpdateGeometryObject();
   mainObj["aero_testing_altitude"] = testAlt;
   mainObj["aero_testing_mach"] = testMach;
   mainObj["aero_max_alpha_deg"] = maxAlpha;
   mainObj["aero_min_alpha_deg"] = minAlpha;
   mainObj["aero_max_beta_deg"] = maxBeta;
   mainObj["aero_max_mach"] = maxMach;
   mainObj["aero_alpha_symmetry"] = alphaSymmetry;
   mainObj["aero_beta_symmetry"] = betaSymmetry;

   curDoc->setObject(mainObj);
   MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->SaveCurrentVehicleFile();
}
//void GeometryWidget::ChangeTheme(QString aThemeName)
//{
//   QString viewIconStr(QString(":/icons/%1/64x64/show.png").arg(aThemeName));
//   mUIPtr->mViewSettingsButton->setIcon(QIcon(viewIconStr));
//}

void GeometryWidget::HandleRevertOptions()
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   QSettings* settings = mw.GetSettings();
   GeometryGLWidget* glWidget = GetGeometryGLWidget();

   settings->setValue("ShowAxis", true);
   glWidget->ShowAxis(true);
   mGeometryOptionsPtr->SetShowPrimaryAxes(true);

   settings->setValue("ShowSelectedObjectCG", true);
   glWidget->ShowSelectedObjectCG(true);
   mGeometryOptionsPtr->SetShowSelectedCG(true);

   settings->setValue("ShowEngines", true);
   glWidget->ShowEngines(true);
   mGeometryOptionsPtr->SetShowEngines(true);

   settings->setValue("ShowThrustVectors", false);
   glWidget->ShowThrustVectors(false);
   mGeometryOptionsPtr->SetShowThrustVectors(false);

   settings->setValue("ShowPointMasses", false);
   glWidget->ShowPointMasses(false);
   mGeometryOptionsPtr->SetShowPointMasses(false);

   settings->setValue("ShowFuelTanks", false);
   glWidget->ShowFuelTanks(false);
   mGeometryOptionsPtr->SetShowFuelTanks(false);

   settings->setValue("ShowVehicleCG", true);
   glWidget->ShowVehicleCG(true);
   mGeometryOptionsPtr->SetShowVehicleCG(true);

   settings->setValue("ShowLandingGear", true);
   glWidget->ShowLandingGear(true);
   mGeometryOptionsPtr->SetShowLandingGear(true);

   settings->setValue("ShowSpeedBrakes", true);
   glWidget->ShowSpeedBrakes(true);
   mGeometryOptionsPtr->SetShowSpeedBrakes(true);

   settings->setValue("ShowWireframe", false);
   glWidget->ShowWireframe(false);
   mGeometryOptionsPtr->SetShowWireframe(false);

   settings->setValue("AeroVectors", "Hide");
   glWidget->HideAeroVectors();
   mGeometryOptionsPtr->SetHideAeroVectors();

   settings->setValue("ShowDebug", false);
   mGeometryOptionsPtr->SetShowDebugData(false);
}

void GeometryWidget::HandleMoveToCG_X()
{
   Vehicle* vehicle = GetVehicle();

   if (vehicle == nullptr) { return; } // Can't do *anything* without a vehicle

   Designer::VehicleGeometry& vehicleGeometry = vehicle->GetGeometry();

   Designer::GeometryMassProperties* massProperties = vehicleGeometry.GetMassProperties();
   if (massProperties == nullptr) { return; } // Can't do *anything* without a mass properties

   // Get current vehicle CG
   UtVec3dX currentCg = massProperties->GetCg();

   // We want to move the current CG, so move everything by the opposite delta
   UtVec3dX moveDelta_ft = currentCg * -1.0;

   // Do not move in Y or Z
   moveDelta_ft.Set(moveDelta_ft.X(), 0.0, 0.0);

   // Block Qt signals while we modify things
   BlockAllSignals(true);

   // Loop though all geometry objects
   for (auto& objIter : vehicleGeometry.GetGeometryObjectMap())
   {
      GeometryObject* curObj = objIter.second;
      curObj->MoveRefPoint(moveDelta_ft);
   }

   // Now, re-calculate the mass and inertia (the CG should now be at the origin)
   vehicle->CalcMassAndInertia();
   vehicle->GenerateAeroDataFromGeometry();
   vehicle->CalculateAeroAttitudeVectors();

   // Update to show the vehicle now centered at the CG
   vehicle->Update();

   // Re-enable Qt signals
   BlockAllSignals(false);
}

void GeometryWidget::HandleMoveToCG_Z()
{

   Designer::Vehicle* vehicle = GetVehicle();
   if (vehicle == nullptr) { return; } // Can't do *anything* without a vehicle

   Designer::VehicleGeometry& vehicleGeometry = vehicle->GetGeometry();

   Designer::GeometryMassProperties* massProperties = vehicleGeometry.GetMassProperties();
   if (massProperties == nullptr) { return; } // Can't do *anything* without a mass properties

   // Get current vehicle CG
   UtVec3dX currentCg = massProperties->GetCg();

   // We want to move the current CG, so move everything by the opposite delta
   UtVec3dX moveDelta_ft = currentCg * -1.0;

   // Do not move in X or Y
   moveDelta_ft.Set(0.0, 0.0, moveDelta_ft.Z());

   // Block Qt signals while we modify things
   BlockAllSignals(true);

   // Loop though all geometry objects
   for (auto& objIter : vehicleGeometry.GetGeometryObjectMap())
   {
      GeometryObject* curObj = objIter.second;
      curObj->MoveRefPoint(moveDelta_ft);
   }

   // Now, re-calculate the mass and inertia (the CG should now be at the origin)
   vehicle->CalcMassAndInertia();
   vehicle->GenerateAeroDataFromGeometry();
   vehicle->CalculateAeroAttitudeVectors();

   // Update to show the vehicle now centered at the CG
   vehicle->Update();

   // Re-enable Qt signals
   BlockAllSignals(false);
}


void GeometryWidget::HandleMoveToCG_XZ()
{
   Designer::Vehicle* vehicle = GetVehicle();
   if (vehicle == nullptr) { return; } // Can't do *anything* without a vehicle

   Designer::VehicleGeometry& vehicleGeometry = vehicle->GetGeometry();

   Designer::GeometryMassProperties* massProperties = vehicleGeometry.GetMassProperties();
   if (massProperties == nullptr) { return; } // Can't do *anything* without a mass properties

   // Get current vehicle CG
   UtVec3dX currentCg = massProperties->GetCg();

   // We want to move the current CG, so move everything by the opposite delta
   UtVec3dX moveDelta_ft = currentCg * -1.0;

   // Do not move in Y
   moveDelta_ft.Set(moveDelta_ft.X(), 0.0, moveDelta_ft.Z());

   // Block Qt signals while we modify things
   BlockAllSignals(true);

   // Loop though all geometry objects
   for (auto& objIter : vehicleGeometry.GetGeometryObjectMap())
   {
      GeometryObject* curObj = objIter.second;
      curObj->MoveRefPoint(moveDelta_ft);
   }

   // Now, re-calculate the mass and inertia (the CG should now be at the origin)
   vehicle->CalcMassAndInertia();
   vehicle->GenerateAeroDataFromGeometry();
   vehicle->CalculateAeroAttitudeVectors();

   // Update to show the vehicle now centered at the CG
   vehicle->Update();

   // Re-enable Qt signals
   BlockAllSignals(false);
}

void GeometryWidget::HandleMoveToCG_XYZ()
{

   Designer::Vehicle* vehicle = GetVehicle();
   if (vehicle == nullptr) { return; } // Can't do *anything* without a vehicle

   Designer::VehicleGeometry& vehicleGeometry = vehicle->GetGeometry();

   Designer::GeometryMassProperties* massProperties = vehicleGeometry.GetMassProperties();
   if (massProperties == nullptr) { return; } // Can't do *anything* without a mass properties

   // Get current vehicle CG
   UtVec3dX currentCg = massProperties->GetCg();

   // We want to move the current CG, so move everything by the opposite delta
   UtVec3dX moveDelta_ft = currentCg * -1.0;

   // Block Qt signals while we modify things
   BlockAllSignals(true);

   // Loop though all geometry objects
   for (auto& objIter : vehicleGeometry.GetGeometryObjectMap())
   {
      GeometryObject* curObj = objIter.second;
      curObj->MoveRefPoint(moveDelta_ft);
   }

   // Now, re-calculate the mass and inertia (the CG should now be at the origin)
   vehicle->CalcMassAndInertia();
   vehicle->GenerateAeroDataFromGeometry();
   vehicle->CalculateAeroAttitudeVectors();

   // Update to show the vehicle now centered at the CG
   vehicle->Update();

   // Re-enable Qt signals
   BlockAllSignals(false);
}

void GeometryWidget::HandleMoveFocusRight()
{
   Designer::GeometryGLWidget* glObj = GetGeometryGLWidget();

   auto stepSize = static_cast<double>(glObj->GetRegionStepSize());
   double currentZ = glObj->GetViewingFocusPointZ();
   glObj->SetViewingFocusPointZ(currentZ + stepSize);
   glObj->ReCalculateEyePoint();
   glObj->Update();
}

void GeometryWidget::HandleMoveFocusLeft()
{
   Designer::GeometryGLWidget* glObj = GetGeometryGLWidget();

   auto stepSize = static_cast<double>(glObj->GetRegionStepSize());
   double currentZ = glObj->GetViewingFocusPointZ();
   glObj->SetViewingFocusPointZ(currentZ - stepSize);
   glObj->ReCalculateEyePoint();
   glObj->Update();
}

void GeometryWidget::HandleMoveFocusForward()
{
   Designer::GeometryGLWidget* glObj = GetGeometryGLWidget();

   auto stepSize = static_cast<double>(glObj->GetRegionStepSize());
   double currentX = glObj->GetViewingFocusPointX();
   glObj->SetViewingFocusPointX(currentX + stepSize);
   glObj->ReCalculateEyePoint();
   glObj->Update();
}

void GeometryWidget::HandleMoveFocusAft()
{
   Designer::GeometryGLWidget* glObj = GetGeometryGLWidget();

   auto stepSize = static_cast<double>(glObj->GetRegionStepSize());
   double currentX = glObj->GetViewingFocusPointX();
   glObj->SetViewingFocusPointX(currentX - stepSize);
   glObj->ReCalculateEyePoint();
   glObj->Update();
}

void GeometryWidget::HandleResetFocusPoint()
{
   Designer::GeometryGLWidget* glObj = GetGeometryGLWidget();

   glObj->SetViewingFocusPointX(0.0);
   glObj->SetViewingFocusPointY(0.0);
   glObj->SetViewingFocusPointZ(0.0);
   glObj->ReCalculateEyePoint();
   glObj->Update();
}

void GeometryWidget::HandleToggleShowPrimaryAxes()
{
   GetGeometryGLWidget()->ToggleAxis();
}

void GeometryWidget::HandleToggleShowSelectedCG()
{
   GetGeometryGLWidget()->ToggleSelectedObjectCG();
}

void GeometryWidget::HandleToggleShowEngines()
{
   GetGeometryGLWidget()->ToggleShowEngines();
}

void GeometryWidget::HandleToggleShowThrustVectors()
{
   GetGeometryGLWidget()->ToggleThrustVectors();
}

void GeometryWidget::HandleToggleShowPointMasses()
{
   GetGeometryGLWidget()->TogglePointMasses();
}

void GeometryWidget::HandleToggleShowFuelTanks()
{
   GetGeometryGLWidget()->ToggleFuelTanks();
}

void GeometryWidget::HandleToggleShowVehicleCG()
{
   GetGeometryGLWidget()->ToggleVehicleCG();
}

void GeometryWidget::HandleToggleShowLandingGear()
{
   GetGeometryGLWidget()->ToggleLandingGear();
}

void GeometryWidget::HandleToggleShowSpeedBrakes()
{
   GetGeometryGLWidget()->ToggleSpeedBrakes();
}

void GeometryWidget::HandleHideAeroVectors()
{
   GetGeometryGLWidget()->HideAeroVectors();
}

void GeometryWidget::HandleShowAllAeroVectors()
{
   GetGeometryGLWidget()->ShowAllAeroVectors();
}

void GeometryWidget::HandleShowSelectedAeroVectors()
{
   GetGeometryGLWidget()->ShowSelectedAeroVectors();
}

void GeometryWidget::HandleToggleShowWireframe()
{
   GetGeometryGLWidget()->ToggleShowWireframe();
}

void GeometryWidget::HandleToggleOutputDebugData()
{
   bool currentDebugSetting = GetVehicle()->DebugMode();
   GetVehicle()->SetDebugMode(!currentDebugSetting);
}

void GeometryWidget::SetDefaultSpeedConditions()
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   double mach        = mw.GetMoverCreatorWidget()->GetVehicleMach();
   double altitude_ft = 20000.0;
   double altitude_m  = altitude_ft * UtMath::cM_PER_FT;

   UtAtmosphere atm = GetVehicle()->GetAtmosphere();

   // Calculate speed, in feet per second
   double speed_fps = atm.CalcFpsFromMach(altitude_m, mach);

   // Calculate speed, in knots (knots true airspeed)
   double ktas = UtMath::cNMPH_PER_FPS * speed_fps;

   // Calculate knots calibrated airspeed
   double kcas = atm.CalcKcasFromFps(altitude_m, speed_fps);

   // Calculate dynamic pressure, in lbf per square foot
   double dynamicPressure_psf = atm.CalcDynamicPressure_psf(altitude_m, speed_fps);

   // Calculate density, in slugs per cubic foot
   double density_kgm3 = atm.Density(altitude_m);
   double density_slugs_ft3 = UtMath::cM_PER_FT * UtMath::cM_PER_FT * UtMath::cM_PER_FT / UtMath::cKG_PER_SLUG * density_kgm3;

   GetVehicle()->SetVehicleSpeedParameters(speed_fps, mach, ktas, kcas, dynamicPressure_psf, density_slugs_ft3);
}

void GeometryWidget::HandleAlphaIncrease()
{
   double alpha_deg = 0.0;
   double beta_deg = 0.0;
   GetVehicle()->GetVehicleAlphaBeta(alpha_deg, beta_deg);

   alpha_deg += 1.0;
   if (alpha_deg > 180.0)
   {
      alpha_deg -= 360.0;
   }
   if (alpha_deg < -180.0)
   {
      alpha_deg += 360.0;
   }

   GetVehicle()->SetVehicleAlphaBeta(alpha_deg, beta_deg);

   // Set default speed conditions
   SetDefaultSpeedConditions();

   // Update the aero vectors
   GetVehicle()->CalculateAeroAttitudeVectors();
   GetGeometryGLWidget()->Update();
}

void GeometryWidget::HandleAlphaDecrease()
{
   double alpha_deg = 0.0;
   double beta_deg = 0.0;
   GetVehicle()->GetVehicleAlphaBeta(alpha_deg, beta_deg);

   alpha_deg -= 1.0;
   if (alpha_deg > 180.0)
   {
      alpha_deg -= 360.0;
   }
   if (alpha_deg < -180.0)
   {
      alpha_deg += 360.0;
   }

   GetVehicle()->SetVehicleAlphaBeta(alpha_deg, beta_deg);

   // Set default speed conditions
   SetDefaultSpeedConditions();

   // Update the aero vectors
   GetVehicle()->CalculateAeroAttitudeVectors();
   GetGeometryGLWidget()->Update();
}

void GeometryWidget::HandleBetaIncrease()
{
   double alpha_deg = 0.0;
   double beta_deg = 0.0;
   GetVehicle()->GetVehicleAlphaBeta(alpha_deg, beta_deg);

   beta_deg += 1.0;
   if (beta_deg > 90.0)
   {
      beta_deg = 90.0;
   }
   if (beta_deg < -90.0)
   {
      beta_deg = -90.0;
   }

   GetVehicle()->SetVehicleAlphaBeta(alpha_deg, beta_deg);

   // Set default speed conditions
   SetDefaultSpeedConditions();

   // Update the aero vectors
   GetVehicle()->CalculateAeroAttitudeVectors();
   GetGeometryGLWidget()->Update();
}

void GeometryWidget::HandleBetaDecrease()
{
   double alpha_deg = 0.0;
   double beta_deg = 0.0;
   GetVehicle()->GetVehicleAlphaBeta(alpha_deg, beta_deg);

   beta_deg -= 1.0;
   if (beta_deg > 90.0)
   {
      beta_deg = 90.0;
   }
   if (beta_deg < -90.0)
   {
      beta_deg = -90.0;
   }

   GetVehicle()->SetVehicleAlphaBeta(alpha_deg, beta_deg);

   // Set default speed conditions
   SetDefaultSpeedConditions();

   // Update the aero vectors
   GetVehicle()->CalculateAeroAttitudeVectors();
   GetGeometryGLWidget()->Update();
}

void GeometryWidget::HandleYawRateIncrease()
{
   double yawRate_dps = 0.0;
   double pitchRate_dps = 0.0;
   double rollRate_dps = 0.0;
   GetVehicle()->GetVehicleYawPitchRollRates(yawRate_dps, pitchRate_dps, rollRate_dps);

   yawRate_dps += 30.0;

   GetVehicle()->SetVehicleYawPitchRollRates(yawRate_dps, pitchRate_dps, rollRate_dps);

   // Set default speed conditions
   SetDefaultSpeedConditions();

   // Update the aero vectors
   GetVehicle()->CalculateAeroAttitudeVectors();
   GetGeometryGLWidget()->Update();
}

void GeometryWidget::HandleYawRateDecrease()
{
   double yawRate_dps = 0.0;
   double pitchRate_dps = 0.0;
   double rollRate_dps = 0.0;
   GetVehicle()->GetVehicleYawPitchRollRates(yawRate_dps, pitchRate_dps, rollRate_dps);

   yawRate_dps -= 30.0;

   GetVehicle()->SetVehicleYawPitchRollRates(yawRate_dps, pitchRate_dps, rollRate_dps);

   // Set default speed conditions
   SetDefaultSpeedConditions();

   // Update the aero vectors
   GetVehicle()->CalculateAeroAttitudeVectors();
   GetGeometryGLWidget()->Update();
}

void GeometryWidget::HandlePitchRateIncrease()
{
   double yawRate_dps = 0.0;
   double pitchRate_dps = 0.0;
   double rollRate_dps = 0.0;
   GetVehicle()->GetVehicleYawPitchRollRates(yawRate_dps, pitchRate_dps, rollRate_dps);

   pitchRate_dps += 30.0;

   GetVehicle()->SetVehicleYawPitchRollRates(yawRate_dps, pitchRate_dps, rollRate_dps);

   // Set default speed conditions
   SetDefaultSpeedConditions();

   // Update the aero vectors
   GetVehicle()->CalculateAeroAttitudeVectors();
   GetGeometryGLWidget()->Update();
}

void GeometryWidget::HandlePitchRateDecrease()
{
   double yawRate_dps = 0.0;
   double pitchRate_dps = 0.0;
   double rollRate_dps = 0.0;
   GetVehicle()->GetVehicleYawPitchRollRates(yawRate_dps, pitchRate_dps, rollRate_dps);

   pitchRate_dps -= 30.0;

   GetVehicle()->SetVehicleYawPitchRollRates(yawRate_dps, pitchRate_dps, rollRate_dps);

   // Set default speed conditions
   SetDefaultSpeedConditions();

   // Update the aero vectors
   GetVehicle()->CalculateAeroAttitudeVectors();
   GetGeometryGLWidget()->Update();
}


void GeometryWidget::HandleRollRateIncrease()
{
   double yawRate_dps = 0.0;
   double pitchRate_dps = 0.0;
   double rollRate_dps = 0.0;
   GetVehicle()->GetVehicleYawPitchRollRates(yawRate_dps, pitchRate_dps, rollRate_dps);

   rollRate_dps += 30.0;

   GetVehicle()->SetVehicleYawPitchRollRates(yawRate_dps, pitchRate_dps, rollRate_dps);

   // Set default speed conditions
   SetDefaultSpeedConditions();

   // Update the aero vectors
   GetVehicle()->CalculateAeroAttitudeVectors();
   GetGeometryGLWidget()->Update();
}

void GeometryWidget::HandleRollRateDecrease()
{
   double yawRate_dps = 0.0;
   double pitchRate_dps = 0.0;
   double rollRate_dps = 0.0;
   GetVehicle()->GetVehicleYawPitchRollRates(yawRate_dps, pitchRate_dps, rollRate_dps);

   rollRate_dps -= 30.0;

   GetVehicle()->SetVehicleYawPitchRollRates(yawRate_dps, pitchRate_dps, rollRate_dps);

   // Set default speed conditions
   SetDefaultSpeedConditions();

   // Update the aero vectors
   GetVehicle()->CalculateAeroAttitudeVectors();
   GetGeometryGLWidget()->Update();
}

void GeometryWidget::HandleSpeedIncrease()
{
   double speed_fps = 0.0;
   double mach = 0.0;
   double KTAS = 0.0;
   double KCAS = 0.0;
   double dynamicPressure_psf = 0.0;
   double airDensity_slugs_ft3 = 0.0;

   GetVehicle()->GetVehicleSpeedParameters(speed_fps, mach, KTAS, KCAS, dynamicPressure_psf, airDensity_slugs_ft3);

   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   mw.GetMoverCreatorWidget()->SetVehicleMach(mach + 0.1);

   // Set default speed conditions
   SetDefaultSpeedConditions();

   // Update the aero vectors
   GetVehicle()->CalculateAeroAttitudeVectors();
   GetGeometryGLWidget()->Update();
}

void GeometryWidget::HandleSpeedDecrease()
{
   double speed_fps = 0.0;
   double mach = 0.0;
   double KTAS = 0.0;
   double KCAS = 0.0;
   double dynamicPressure_psf = 0.0;
   double airDensity_slugs_ft3 = 0.0;

   GetVehicle()->GetVehicleSpeedParameters(speed_fps, mach, KTAS, KCAS, dynamicPressure_psf, airDensity_slugs_ft3);

   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   mw.GetMoverCreatorWidget()->SetVehicleMach(mach - 0.1);

   // Set default speed conditions
   SetDefaultSpeedConditions();

   // Update the aero vectors
   GetVehicle()->CalculateAeroAttitudeVectors();
   GetGeometryGLWidget()->Update();
}

void GeometryWidget::HandleSurfaceControlAngleIncrease()
{
   // First, determine which surface is active
   VehicleGeometry& vehicleGeometry = GetVehicle()->GetGeometry();
   const std::map<size_t, GeometryObject*>& geometryObjMap = vehicleGeometry.GetGeometryObjectMap();

   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();

      if (geometryType == "GeometrySurface")
      {
         auto surface = dynamic_cast<Designer::GeometrySurface*>(geometryObject);

         if (GetGeometryGLWidget()->TreeWidgetItemSelected(surface->GetTreeWidgetItem()))
         {
            // We have the selected surface object

            VehicleAero* vehicleAero = GetVehicle()->GetAero();
            const std::vector<AeroObject*> aeroObjList = vehicleAero->GetAeroObjectsGeneratedFrom(surface->GetName());
            for (auto& aeroObj : aeroObjList)
            {
               double angle_deg = aeroObj->GetControlAngle_deg();
               angle_deg += 2.0;
               aeroObj->SetControlAngle_deg(angle_deg);
            }

            GetVehicle()->CalculateAeroAttitudeVectors();
            GetGeometryGLWidget()->Update();
         }
      }
   }
}

void GeometryWidget::HandleSurfaceControlAngleDecrease()
{
   // First, determine which surface is active
   VehicleGeometry& vehicleGeometry = GetVehicle()->GetGeometry();
   const std::map<size_t, GeometryObject*>& geometryObjMap = vehicleGeometry.GetGeometryObjectMap();

   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();

      if (geometryType == "GeometrySurface")
      {
         auto surface = dynamic_cast<Designer::GeometrySurface*>(geometryObject);

         if (GetGeometryGLWidget()->TreeWidgetItemSelected(surface->GetTreeWidgetItem()))
         {
            // We have the selected surface object

            VehicleAero* vehicleAero = GetVehicle()->GetAero();
            const std::vector<AeroObject*> aeroObjList = vehicleAero->GetAeroObjectsGeneratedFrom(surface->GetName());
            for (auto& aeroObj : aeroObjList)
            {
               double angle_deg = aeroObj->GetControlAngle_deg();
               angle_deg -= 2.0;
               aeroObj->SetControlAngle_deg(angle_deg);
            }

            GetVehicle()->CalculateAeroAttitudeVectors();
            GetGeometryGLWidget()->Update();
         }
      }
   }
}

void GeometryWidget::HandleSelectNone()
{
   DeselectTreeWidget();
   GetGeometryGLWidget()->Update();
}

void GeometryWidget::HandleSaveFocusPoint()
{
   GetGeometryGLWidget()->SaveCurrentFocusPoint();
   GetGeometryGLWidget()->Update();
}

void GeometryWidget::HandleMoveToFocusPoint(int aKey)
{
   GetGeometryGLWidget()->MoveToFocusPoint(aKey);
   GetGeometryGLWidget()->ReCalculateEyePoint();
   GetGeometryGLWidget()->Update();
}

void GeometryWidget::InitializeViewSettings()
{

   mGeometryOptionsPtr = ut::make_unique<GeometryOptionsDialog>(this);
   mGeometrySettingsPtr = ut::make_unique<GeometrySettingsDialog>(this);
   connect(mGeometryOptionsPtr.get(), &GeometryOptionsDialog::RevertOptions, this, &GeometryWidget::HandleRevertOptions);
   connect(mGeometrySettingsPtr.get(), &GeometrySettingsDialog::MoveToCG_X, this, &GeometryWidget::HandleMoveToCG_X);
   connect(mGeometrySettingsPtr.get(), &GeometrySettingsDialog::MoveToCG_Z, this, &GeometryWidget::HandleMoveToCG_Z);
   connect(mGeometrySettingsPtr.get(), &GeometrySettingsDialog::MoveToCG_XZ, this, &GeometryWidget::HandleMoveToCG_XZ);
   connect(mGeometrySettingsPtr.get(), &GeometrySettingsDialog::MoveToCG_XYZ, this, &GeometryWidget::HandleMoveToCG_XYZ);
   connect(mGeometryOptionsPtr.get(), &GeometryOptionsDialog::MoveFocusRight, this, &GeometryWidget::HandleMoveFocusRight);
   connect(mGeometryOptionsPtr.get(), &GeometryOptionsDialog::MoveFocusLeft, this, &GeometryWidget::HandleMoveFocusLeft);
   connect(mGeometryOptionsPtr.get(), &GeometryOptionsDialog::MoveFocusForward, this, &GeometryWidget::HandleMoveFocusForward);
   connect(mGeometryOptionsPtr.get(), &GeometryOptionsDialog::MoveFocusAft, this, &GeometryWidget::HandleMoveFocusAft);
   connect(mGeometryOptionsPtr.get(), &GeometryOptionsDialog::ResetFocusPoint, this, &GeometryWidget::HandleResetFocusPoint);
   connect(mGeometryOptionsPtr.get(), &GeometryOptionsDialog::TogglePrimaryAxes, this, &GeometryWidget::HandleToggleShowPrimaryAxes);
   connect(mGeometryOptionsPtr.get(), &GeometryOptionsDialog::ToggleShowSelectedCG, this, &GeometryWidget::HandleToggleShowSelectedCG);
   connect(mGeometryOptionsPtr.get(), &GeometryOptionsDialog::ToggleShowEngines, this, &GeometryWidget::HandleToggleShowEngines);
   connect(mGeometryOptionsPtr.get(), &GeometryOptionsDialog::ToggleShowThrustVectors, this, &GeometryWidget::HandleToggleShowThrustVectors);
   connect(mGeometryOptionsPtr.get(), &GeometryOptionsDialog::ToggleShowPointMasses, this, &GeometryWidget::HandleToggleShowPointMasses);
   connect(mGeometryOptionsPtr.get(), &GeometryOptionsDialog::ToggleShowFuelTanks, this, &GeometryWidget::HandleToggleShowFuelTanks);
   connect(mGeometryOptionsPtr.get(), &GeometryOptionsDialog::ToggleShowVehicleCG, this, &GeometryWidget::HandleToggleShowVehicleCG);
   connect(mGeometryOptionsPtr.get(), &GeometryOptionsDialog::ToggleShowLandingGear, this, &GeometryWidget::HandleToggleShowLandingGear);
   connect(mGeometryOptionsPtr.get(), &GeometryOptionsDialog::ToggleShowSpeedBrakes, this, &GeometryWidget::HandleToggleShowSpeedBrakes);
   connect(mGeometryOptionsPtr.get(), &GeometryOptionsDialog::HideAeroVectors, this, &GeometryWidget::HandleHideAeroVectors);
   connect(mGeometryOptionsPtr.get(), &GeometryOptionsDialog::ShowAllAeroVectors, this, &GeometryWidget::HandleShowAllAeroVectors);
   connect(mGeometryOptionsPtr.get(), &GeometryOptionsDialog::ShowSelectedAeroVectors, this, &GeometryWidget::HandleShowSelectedAeroVectors);
   connect(mGeometryOptionsPtr.get(), &GeometryOptionsDialog::ToggleShowWireframe, this, &GeometryWidget::HandleToggleShowWireframe);
   connect(mGeometryOptionsPtr.get(), &GeometryOptionsDialog::ToggleShowDebugData, this, &GeometryWidget::HandleToggleOutputDebugData);
   connect(mGeometrySettingsPtr.get(), &GeometrySettingsDialog::AlphaIncrease, this, &GeometryWidget::HandleAlphaIncrease);
   connect(mGeometrySettingsPtr.get(), &GeometrySettingsDialog::AlphaDecrease, this, &GeometryWidget::HandleAlphaDecrease);
   connect(mGeometrySettingsPtr.get(), &GeometrySettingsDialog::BetaIncrease, this, &GeometryWidget::HandleBetaIncrease);
   connect(mGeometrySettingsPtr.get(), &GeometrySettingsDialog::BetaDecrease, this, &GeometryWidget::HandleBetaDecrease);
   connect(mGeometrySettingsPtr.get(), &GeometrySettingsDialog::YawRateIncrease, this, &GeometryWidget::HandleYawRateIncrease);
   connect(mGeometrySettingsPtr.get(), &GeometrySettingsDialog::YawRateDecrease, this, &GeometryWidget::HandleYawRateDecrease);
   connect(mGeometrySettingsPtr.get(), &GeometrySettingsDialog::PitchRateIncrease, this, &GeometryWidget::HandlePitchRateIncrease);
   connect(mGeometrySettingsPtr.get(), &GeometrySettingsDialog::PitchRateDecrease, this, &GeometryWidget::HandlePitchRateDecrease);
   connect(mGeometrySettingsPtr.get(), &GeometrySettingsDialog::RollRateIncrease, this, &GeometryWidget::HandleRollRateIncrease);
   connect(mGeometrySettingsPtr.get(), &GeometrySettingsDialog::RollRateDecrease, this, &GeometryWidget::HandleRollRateDecrease);
   connect(mGeometrySettingsPtr.get(), &GeometrySettingsDialog::SpeedIncrease, this, &GeometryWidget::HandleSpeedIncrease);
   connect(mGeometrySettingsPtr.get(), &GeometrySettingsDialog::SpeedDecrease, this, &GeometryWidget::HandleSpeedDecrease);
   connect(mGeometrySettingsPtr.get(), &GeometrySettingsDialog::SurfaceControlAngleIncrease, this, &GeometryWidget::HandleSurfaceControlAngleIncrease);
   connect(mGeometrySettingsPtr.get(), &GeometrySettingsDialog::SurfaceControlAngleDecrease, this, &GeometryWidget::HandleSurfaceControlAngleDecrease);
   connect(mGeometrySettingsPtr.get(), &GeometrySettingsDialog::accepted, this, &GeometryWidget::HandleSettingsChanged);

   //load the settings
   MoverCreatorMainWindow& mainWindow = MoverCreatorMainWindow::GetInstance();
   QSettings* settings = mainWindow.GetSettings();
   GeometryGLWidget* glWidget = GetGeometryGLWidget();

   if (settings->contains("ShowAxis"))
   {
      bool showAxis = settings->value("ShowAxis").toBool();
      mGeometryOptionsPtr->SetShowPrimaryAxes(showAxis);
      glWidget->ShowAxis(showAxis);
   }

   if (settings->contains("ShowSelectedObjectCG"))
   {
      bool showObjectCG = settings->value("ShowSelectedObjectCG").toBool();
      mGeometryOptionsPtr->SetShowSelectedCG(showObjectCG);
      glWidget->ShowSelectedObjectCG(showObjectCG);
   }

   if (settings->contains("ShowEngines"))
   {
      bool showEngines = settings->value("ShowEngines").toBool();
      mGeometryOptionsPtr->SetShowEngines(showEngines);
      glWidget->ShowEngines(showEngines);
   }

   if (settings->contains("ShowThrustVectors"))
   {
      bool showThrustVectors = settings->value("ShowThrustVectors").toBool();
      mGeometryOptionsPtr->SetShowThrustVectors(showThrustVectors);
      glWidget->ShowThrustVectors(showThrustVectors);
   }

   if (settings->contains("ShowPointMasses"))
   {
      bool showPointMasses = settings->value("ShowPointMasses").toBool();
      mGeometryOptionsPtr->SetShowPointMasses(showPointMasses);
      glWidget->ShowPointMasses(showPointMasses);
   }

   if (settings->contains("ShowFuelTanks"))
   {
      bool showFuelTanks = settings->value("ShowFuelTanks").toBool();
      mGeometryOptionsPtr->SetShowFuelTanks(showFuelTanks);
      glWidget->ShowFuelTanks(showFuelTanks);
   }

   if (settings->contains("ShowVehicleCG"))
   {
      bool showVehicleCG = settings->value("ShowVehicleCG").toBool();
      mGeometryOptionsPtr->SetShowVehicleCG(showVehicleCG);
      glWidget->ShowVehicleCG(showVehicleCG);
   }

   if (settings->contains("ShowLandingGear"))
   {
      bool showLandingGear = settings->value("ShowLandingGear").toBool();
      mGeometryOptionsPtr->SetShowLandingGear(showLandingGear);
      glWidget->ShowLandingGear(showLandingGear);
   }

   if (settings->contains("ShowSpeedBrakes"))
   {
      bool showSpeedBrakes = settings->value("ShowSpeedBrakes").toBool();
      mGeometryOptionsPtr->SetShowSpeedBrakes(showSpeedBrakes);
      glWidget->ShowSpeedBrakes(showSpeedBrakes);
   }

   if (settings->contains("AeroVectors"))
   {
      QString aeroVectors = settings->value("AeroVectors").toString();
      if (aeroVectors == "Hide")
      {
         mGeometryOptionsPtr->SetHideAeroVectors();
         HandleHideAeroVectors();
      }
      else if (aeroVectors == "ShowAll")
      {
         mGeometryOptionsPtr->SetShowAllAeroVectors();
         HandleShowAllAeroVectors();
      }
      else if (aeroVectors == "ShowSelected")
      {
         mGeometryOptionsPtr->SetShowSelectedAeroVectors();
         HandleShowSelectedAeroVectors();
      }
   }

   if (settings->contains("ShowWireframe"))
   {
      bool showWireframe = settings->value("ShowWireframe").toBool();
      if (showWireframe)
      {
         mGeometryOptionsPtr->SetShowWireframe(showWireframe);
         HandleToggleShowWireframe();
      }
   }

   if (settings->contains("ShowDebug"))
   {
      bool showDebug = settings->value("ShowDebug").toBool();
      if (showDebug)
      {
         mGeometryOptionsPtr->SetShowDebugData(showDebug);
         HandleToggleOutputDebugData();
      }
   }

   if (settings->contains("UseDefaults"))
   {
      bool useDefaults = settings->value("UseDefaults").toBool();
      mGeometryOptionsPtr->SetShowUseDefaults(useDefaults);
   }
}

} // namespace Designer
