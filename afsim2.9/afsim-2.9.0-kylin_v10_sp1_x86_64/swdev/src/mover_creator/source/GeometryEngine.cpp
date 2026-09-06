// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "GeometryEngine.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>

#include "GeometryGLWidget.hpp"
#include "MoverCreatorLineEdit.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorWidget.hpp"

Designer::GeometryEngine::GeometryEngine(Vehicle* aVehicle)
   : GeometryObject(aVehicle)
{
   mGeometryObjectTypeString.assign("GeometryEngine");
}

Designer::GeometryEngine::~GeometryEngine()
{
}

void Designer::GeometryEngine::MoveRefPoint(UtVec3dX aMoveDelta_ft)
{
   mSymmetryHorizontalY += aMoveDelta_ft.Y();

   UtVec3dX newRefPoint_ft = mRefPoint_ft + aMoveDelta_ft;
   SetRefPoint(newRefPoint_ft);
}

bool Designer::GeometryEngine::GetCG_ft(UtVec3dX& aCg_ft)
{
   aCg_ft = mCalculatedCG_ft;
   return true;
}

std::string Designer::GeometryEngine::GetEngineTypeString()
{
   switch (mEngineType)
   {
      case Designer::GeometryEngine::EngineType::cJET:
         return "Jet";
      case Designer::GeometryEngine::EngineType::cSOLIDROCKET:
         return "SolidRocket";
      case Designer::GeometryEngine::EngineType::cLIQUIDROCKET:
         return "LiquidRocket";
      case Designer::GeometryEngine::EngineType::cRAMJET:
         return "RamJet";
      default:
         return "Jet";
   }
}

void Designer::GeometryEngine::CalcMassProperties(double) // Although a derived function, it ignores aTotalMass_lbs
{
   if (!mSymmetrical)
   {
      {
         // We assume the engine is roughly similar to a solid cylinder
         // Ixx = (1/2) * mass * (radius*radius)
         // Iyy = (1/12) * mass * (3*radius*radius + length*length)
         // Izz = (1/12) * mass * (3*radius*radius + length*length)
         double radius = mDiameter_ft * 0.5;
         double radiusSquared = radius * radius;
         double lengthSquared = mLength_ft * mLength_ft;

         // Correct for location, using parallel axis theorem
         double distX = sqrt(mRefPoint_ft.Y() * mRefPoint_ft.Y() + mRefPoint_ft.Z() * mRefPoint_ft.Z());
         double distY = sqrt(mRefPoint_ft.X() * mRefPoint_ft.X() + mRefPoint_ft.Z() * mRefPoint_ft.Z());
         double distZ = sqrt(mRefPoint_ft.X() * mRefPoint_ft.X() + mRefPoint_ft.Y() * mRefPoint_ft.Y());
         double dX2 = distX * distX;
         double dY2 = distY * distY;
         double dZ2 = distZ * distZ;

         mCalculatedMass_lbs = mMass_lbs - mPropellantMass_lbs;

         double mass_slugs = mCalculatedMass_lbs * mLbsToSlugs;

         mCalculatedIxx = 0.5 * mass_slugs * radiusSquared;
         mCalculatedIyy = (1.0 / 12.0) * mass_slugs * (3.0 * radiusSquared + lengthSquared);
         mCalculatedIzz = (1.0 / 12.0) * mass_slugs * (3.0 * radiusSquared + lengthSquared);
         mCalculatedIxx += mass_slugs * dX2;
         mCalculatedIyy += mass_slugs * dY2;
         mCalculatedIzz += mass_slugs * dZ2;
         mCalculatedCG_ft = mRefPoint_ft;

         mass_slugs = mPropellantMass_lbs * mLbsToSlugs;
         mCalculatedPropellantIxx = 0.5 * mass_slugs * radiusSquared;
         mCalculatedPropellantIyy = (1.0 / 12.0) * mass_slugs * (3.0 * radiusSquared + lengthSquared);
         mCalculatedPropellantIzz = (1.0 / 12.0) * mass_slugs * (3.0 * radiusSquared + lengthSquared);
         mCalculatedPropellantIxx += mass_slugs * dX2;
         mCalculatedPropellantIyy += mass_slugs * dY2;
         mCalculatedPropellantIzz += mass_slugs * dZ2;
         mCalculatedPropellantCG_ft = mRefPoint_ft;

         mass_slugs = mCalculatedMass_lbs * mLbsToSlugs;
         mCalculatedIxx = 0.5 * mass_slugs * radiusSquared;
         mCalculatedIyy = (1.0 / 12.0) * mass_slugs * (3.0 * radiusSquared + lengthSquared);
         mCalculatedIzz = (1.0 / 12.0) * mass_slugs * (3.0 * radiusSquared + lengthSquared);
         mCalculatedIxx += mass_slugs * dX2;
         mCalculatedIyy += mass_slugs * dY2;
         mCalculatedIzz += mass_slugs * dZ2;
         mCalculatedCG_ft = mRefPoint_ft;
      }
   }
   else
   {
      {
         // We assume the engine is roughly similar to a solid cylinder
         double radius = mDiameter_ft * 0.5;
         double radiusSquared = radius * radius;
         double lengthSquared = mLength_ft * mLength_ft;

         // Correct for location, using parallel axis theorem
         UtVec3dX refPt = mRefPoint_ft;
         double distX = sqrt(refPt.Y() * refPt.Y() + refPt.Z() * refPt.Z());
         double distY = sqrt(refPt.X() * refPt.X() + refPt.Z() * refPt.Z());
         double distZ = sqrt(refPt.X() * refPt.X() + refPt.Y() * refPt.Y());
         double dX2 = distX * distX;
         double dY2 = distY * distY;
         double dZ2 = distZ * distZ;

         // Determine location of other engine
         double currentEngineY = mRefPoint_ft.Y();
         double delta = currentEngineY - mSymmetryHorizontalY;
         double otherEngineY = currentEngineY - 2.0 * delta;

         // Two engines, so double the mass
         mPropellantMass_lbs = mPropellantMass_lbs * 2.0;
         mCalculatedMass_lbs = (mMass_lbs - mPropellantMass_lbs) * 2.0;

         // Total Mass

         // First engine

         double mass_slugs = 0.5 * (mCalculatedMass_lbs * mLbsToSlugs);   // We need only a single engine's mass here

         mCalculatedIxx = 0.5 * mass_slugs * radiusSquared;
         mCalculatedIyy = (0.25 * mass_slugs * radiusSquared) + ((1.0 / 12.0) * mass_slugs * lengthSquared);
         mCalculatedIzz = (0.25 * mass_slugs * radiusSquared) + ((1.0 / 12.0) * mass_slugs * lengthSquared);
         mCalculatedIxx += mass_slugs * dX2;
         mCalculatedIyy += mass_slugs * dY2;
         mCalculatedIzz += mass_slugs * dZ2;

         // Second engine

         // Correct for location, using parallel axis theorem
         refPt.Set(mRefPoint_ft.X(), otherEngineY, mRefPoint_ft.Z());
         distX = sqrt(refPt.Y() * refPt.Y() + refPt.Z() * refPt.Z());
         distY = sqrt(refPt.X() * refPt.X() + refPt.Z() * refPt.Z());
         distZ = sqrt(refPt.X() * refPt.X() + refPt.Y() * refPt.Y());
         dX2 = distX * distX;
         dY2 = distY * distY;
         dZ2 = distZ * distZ;

         mCalculatedIxx += 0.5 * mass_slugs * radiusSquared;
         mCalculatedIyy += (0.25 * mass_slugs * radiusSquared) + ((1.0 / 12.0) * mass_slugs * lengthSquared);
         mCalculatedIzz += (0.25 * mass_slugs * radiusSquared) + ((1.0 / 12.0) * mass_slugs * lengthSquared);
         mCalculatedIxx += mass_slugs * dX2;
         mCalculatedIyy += mass_slugs * dY2;
         mCalculatedIzz += mass_slugs * dZ2;

         // Propellant Mass

         // First engine
         mass_slugs = 0.5 * (mPropellantMass_lbs * mLbsToSlugs);   // We need only a single engine's mass here
         mCalculatedPropellantIxx = 0.5 * mass_slugs * radiusSquared;
         mCalculatedPropellantIyy = (0.25 * mass_slugs * radiusSquared) + ((1.0 / 12.0) * mass_slugs * lengthSquared);
         mCalculatedPropellantIzz = (0.25 * mass_slugs * radiusSquared) + ((1.0 / 12.0) * mass_slugs * lengthSquared);
         mCalculatedPropellantIxx += mass_slugs * dX2;
         mCalculatedPropellantIyy += mass_slugs * dY2;
         mCalculatedPropellantIzz += mass_slugs * dZ2;

         // Second engine

         // Correct for location, using parallel axis theorem
         refPt.Set(mRefPoint_ft.X(), otherEngineY, mRefPoint_ft.Z());
         distX = sqrt(refPt.Y() * refPt.Y() + refPt.Z() * refPt.Z());
         distY = sqrt(refPt.X() * refPt.X() + refPt.Z() * refPt.Z());
         distZ = sqrt(refPt.X() * refPt.X() + refPt.Y() * refPt.Y());
         dX2 = distX * distX;
         dY2 = distY * distY;
         dZ2 = distZ * distZ;

         mCalculatedPropellantIxx += 0.5 * mass_slugs * radiusSquared;
         mCalculatedPropellantIyy += (0.25 * mass_slugs * radiusSquared) + ((1.0 / 12.0) * mass_slugs * lengthSquared);
         mCalculatedPropellantIzz += (0.25 * mass_slugs * radiusSquared) + ((1.0 / 12.0) * mass_slugs * lengthSquared);
         mCalculatedPropellantIxx += mass_slugs * dX2;
         mCalculatedPropellantIyy += mass_slugs * dY2;
         mCalculatedPropellantIzz += mass_slugs * dZ2;

         // Empty Mass

         // First engine
         mass_slugs = 0.5 * (mCalculatedMass_lbs * mLbsToSlugs);   // We need only a single engine's mass here
         mCalculatedIxx = 0.5 * mass_slugs * radiusSquared;
         mCalculatedIyy = (0.25 * mass_slugs * radiusSquared) + ((1.0 / 12.0) * mass_slugs * lengthSquared);
         mCalculatedIzz = (0.25 * mass_slugs * radiusSquared) + ((1.0 / 12.0) * mass_slugs * lengthSquared);
         mCalculatedIxx += mass_slugs * dX2;
         mCalculatedIyy += mass_slugs * dY2;
         mCalculatedIzz += mass_slugs * dZ2;

         // Second engine

         // Correct for location, using parallel axis theorem
         refPt.Set(mRefPoint_ft.X(), otherEngineY, mRefPoint_ft.Z());
         distX = sqrt(refPt.Y() * refPt.Y() + refPt.Z() * refPt.Z());
         distY = sqrt(refPt.X() * refPt.X() + refPt.Z() * refPt.Z());
         distZ = sqrt(refPt.X() * refPt.X() + refPt.Y() * refPt.Y());
         dX2 = distX * distX;
         dY2 = distY * distY;
         dZ2 = distZ * distZ;

         mCalculatedIxx += 0.5 * mass_slugs * radiusSquared;
         mCalculatedIyy += (0.25 * mass_slugs * radiusSquared) + ((1.0 / 12.0) * mass_slugs * lengthSquared);
         mCalculatedIzz += (0.25 * mass_slugs * radiusSquared) + ((1.0 / 12.0) * mass_slugs * lengthSquared);
         mCalculatedIxx += mass_slugs * dX2;
         mCalculatedIyy += mass_slugs * dY2;
         mCalculatedIzz += mass_slugs * dZ2;

         // Calculate the location of the cg -- we assume it is always at the reference point,
         // but centered in the Y-axis "between" the two objects
         double centerY = (currentEngineY + otherEngineY) * 0.5;
         mCalculatedCG_ft.Set(mRefPoint_ft.X(), centerY, mRefPoint_ft.Z());
         mCalculatedPropellantCG_ft.Set(mRefPoint_ft.X(), centerY, mRefPoint_ft.Z());
      }
   }
}

bool Designer::GeometryEngine::GetCalculatedPropellantMassProperties(UtVec3dX& aCgLocation_ft,
                                                                     double&   aMass_lbs,
                                                                     UtVec3dX& aRotationalInertia)
{
   aCgLocation_ft = mCalculatedPropellantCG_ft;
   aMass_lbs = mPropellantMass_lbs;
   aRotationalInertia.Set(mCalculatedPropellantIxx, mCalculatedPropellantIyy, mCalculatedPropellantIzz);

   return true;
}

QTreeWidgetItem* Designer::GeometryEngine::CreateTreeWidgetItem(QString& aType, QString& aName, QTreeWidget* aTreeWidgetPtr, bool aNoneditableName)
{
   // Create the tree widget item at the GeometryObject level
   QTreeWidgetItem* treeWidgetItem = GeometryObject::CreateTreeWidgetItem(aName, aTreeWidgetPtr, aNoneditableName);

   connect(treeWidgetItem->treeWidget(), &QTreeWidget::itemClicked, this, &GeometryEngine::HandleTreeItemClicked);

   // Set the name
   treeWidgetItem->setText(0, aName);
   mName = aName.toStdString();

   // Set Qt flags
   treeWidgetItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

   QTreeWidgetItem* typeItem = new QTreeWidgetItem(treeWidgetItem, QStringList("EngineType"));
   typeItem->setToolTip(0, "The type of the engine");
   mEngineTypeComboBox = new QComboBox();
   mEngineModelComboBox = new QComboBox();

   PopulateEngineTypes();
   HandleEngineTypeChanged(mEngineTypeComboBox->currentText());
   HandleEngineModelChanged(mEngineModelComboBox->currentText());

   aTreeWidgetPtr->setItemWidget(typeItem, 1, mEngineTypeComboBox);
   aTreeWidgetPtr->setItemWidget(typeItem, 2, new QLabel(QString::fromStdString("")));

   QTreeWidgetItem* modelItem = new QTreeWidgetItem(treeWidgetItem, QStringList("EngineModel"));
   modelItem->setToolTip(0, "The model of the engine");

   aTreeWidgetPtr->setItemWidget(modelItem, 1, mEngineModelComboBox);

   QWidget* btnWidget = new QWidget();
   QHBoxLayout* btnWidgetLayout = new QHBoxLayout();
   btnWidgetLayout->setSpacing(0);
   btnWidgetLayout->setContentsMargins(0, 0, 0, 0);
   btnWidget->setLayout(btnWidgetLayout);
   QIcon editIcon;
   QString iconThemeName = QStringLiteral("pencil");
   editIcon = QIcon::fromTheme(iconThemeName);
   QPushButton* editButton = new QPushButton("");
   editButton->setIcon(editIcon);
   editButton->setToolTip("Edit Engine");

   QIcon createIcon;
   QString createIconThemeName = QStringLiteral("add");
   createIcon = QIcon::fromTheme(createIconThemeName);
   QPushButton* createButton = new QPushButton("");
   createButton->setIcon(createIcon);
   createButton->setToolTip("Create Engine");

   btnWidgetLayout->addWidget(editButton);
   btnWidgetLayout->addWidget(createButton);
   aTreeWidgetPtr->setItemWidget(modelItem, 2, btnWidget);

   connect(editButton, static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked), this, &GeometryEngine::EditEngineModelClicked);
   connect(createButton, static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked), this, &GeometryEngine::CreateEngineModelClicked);

   QTreeWidgetItem* yaw = new QTreeWidgetItem(treeWidgetItem, QStringList("Yaw Angle"));
   yaw->setToolTip(0, "The yaw of the engine in degrees");
   auto yawLineEdit = new MoverCreatorLineEdit(yaw);
   yawLineEdit->setFrame(false);
   yawLineEdit->setText(QString::number(mYaw_rad * UtMath::cDEG_PER_RAD));
   aTreeWidgetPtr->setItemWidget(yaw, 1, yawLineEdit);
   aTreeWidgetPtr->setItemWidget(yaw, 2, new QLabel(QString::fromStdString("deg")));
   connect(yawLineEdit, &QLineEdit::textChanged, this, &GeometryEngine::HandleYawLineEditChanged);
   connect(yawLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* pitch = new QTreeWidgetItem(treeWidgetItem, QStringList("Pitch Angle"));
   pitch->setToolTip(0, "The pitch of the engine in degrees");
   auto pitchLineEdit = new MoverCreatorLineEdit(pitch);
   pitchLineEdit->setFrame(false);
   pitchLineEdit->setText(QString::number(mPitch_rad * UtMath::cDEG_PER_RAD));
   aTreeWidgetPtr->setItemWidget(pitch, 1, pitchLineEdit);
   aTreeWidgetPtr->setItemWidget(pitch, 2, new QLabel(QString::fromStdString("deg")));
   connect(pitchLineEdit, &QLineEdit::textChanged, this, &GeometryEngine::HandlePitchLineEditChanged);
   connect(pitchLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* roll = new QTreeWidgetItem(treeWidgetItem, QStringList("Roll Angle"));
   roll->setToolTip(0, "The roll of the engine in degrees");
   auto rollLineEdit = new MoverCreatorLineEdit(roll);
   rollLineEdit->setFrame(false);
   rollLineEdit->setText(QString::number(mRoll_rad * UtMath::cDEG_PER_RAD));
   aTreeWidgetPtr->setItemWidget(roll, 1, rollLineEdit);
   aTreeWidgetPtr->setItemWidget(roll, 2, new QLabel(QString::fromStdString("deg")));
   connect(rollLineEdit, &QLineEdit::textChanged, this, &GeometryEngine::HandleRollLineEditChanged);
   connect(rollLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* length = new QTreeWidgetItem(treeWidgetItem, QStringList("Length"));
   length->setToolTip(0, "The length of the engine in feet");
   auto lengthLineEdit = new MoverCreatorLineEdit(length);
   lengthLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   mLengthLineEditPtr = lengthLineEdit;
   lengthLineEdit->setFrame(false);
   lengthLineEdit->setText(QString::number(mLength_ft));
   aTreeWidgetPtr->setItemWidget(length, 1, lengthLineEdit);
   aTreeWidgetPtr->setItemWidget(length, 2, new QLabel(QString::fromStdString("ft")));
   connect(lengthLineEdit, &QLineEdit::textChanged, this, &GeometryEngine::HandleLengthLineEditChanged);
   connect(lengthLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* diameter = new QTreeWidgetItem(treeWidgetItem, QStringList("Diameter"));
   diameter->setToolTip(0, "The diameter of the engine in feet");
   auto diameterLineEdit = new MoverCreatorLineEdit(diameter);
   mDiameterLineEditPtr = diameterLineEdit;
   diameterLineEdit->setFrame(false);
   diameterLineEdit->setText(QString::number(mDiameter_ft));
   aTreeWidgetPtr->setItemWidget(diameter, 1, diameterLineEdit);
   aTreeWidgetPtr->setItemWidget(diameter, 2, new QLabel(QString::fromStdString("ft")));
   connect(diameterLineEdit, &QLineEdit::textChanged, this, &GeometryEngine::HandleDiameterLineEditChanged);
   connect(diameterLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* thrustOffset = new QTreeWidgetItem(treeWidgetItem, QStringList("Thrust Offset"));
   thrustOffset->setToolTip(0, "The thrust offset of the engine in feet");
   auto thrustOffsetLineEdit = new MoverCreatorLineEdit(thrustOffset);
   mThrustOffsetLineEditPtr = thrustOffsetLineEdit;
   thrustOffsetLineEdit->setFrame(false);
   thrustOffsetLineEdit->setText(QString::number(mThrustOffset_ft));
   aTreeWidgetPtr->setItemWidget(thrustOffset, 1, thrustOffsetLineEdit);
   aTreeWidgetPtr->setItemWidget(thrustOffset, 2, new QLabel(QString::fromStdString("ft")));
   connect(thrustOffsetLineEdit, &QLineEdit::textChanged, this, &GeometryEngine::HandleThrustOffsetLineEditChanged);
   connect(thrustOffsetLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* massItem = new QTreeWidgetItem(treeWidgetItem, QStringList("Mass"));
   massItem->setToolTip(0, "The mass of the engine in lbs");
   auto massLineEdit = new MoverCreatorLineEdit(massItem);
   massLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   mMassLineEditPtr = massLineEdit;
   massLineEdit->setFrame(false);
   massLineEdit->setText(QString::number(mMass_lbs));
   aTreeWidgetPtr->setItemWidget(massItem, 1, massLineEdit);
   aTreeWidgetPtr->setItemWidget(massItem, 2, new QLabel(QString::fromStdString("lbs")));
   connect(massLineEdit, &QLineEdit::textChanged, this, &GeometryEngine::HandleMassLineEditChanged);
   connect(massLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* propellantMassItem = new QTreeWidgetItem(treeWidgetItem, QStringList("Propellant Mass"));
   propellantMassItem->setToolTip(0, "The mass of the propellant in lbs");
   auto propellantMassLineEdit = new MoverCreatorLineEdit(propellantMassItem);
   propellantMassLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   mMassLineEditPtr = propellantMassLineEdit;
   propellantMassLineEdit->setFrame(false);
   propellantMassLineEdit->setText(QString::number(mPropellantMass_lbs));
   aTreeWidgetPtr->setItemWidget(propellantMassItem, 1, propellantMassLineEdit);
   aTreeWidgetPtr->setItemWidget(propellantMassItem, 2, new QLabel(QString::fromStdString("lbs")));
   connect(propellantMassLineEdit, &QLineEdit::textChanged, this, &GeometryEngine::HandlepropellantMassLineEditChanged);
   connect(propellantMassLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* symmetrical = new QTreeWidgetItem(treeWidgetItem, QStringList("Symmetrical"));
   symmetrical->setToolTip(0, "If checked, the engine will utilize symmetry");
   QCheckBox* symmetricalCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(symmetrical, 1, symmetricalCheckBox);
   aTreeWidgetPtr->setItemWidget(symmetrical, 2, new QLabel(QString::fromStdString("")));
   connect(symmetricalCheckBox, &QCheckBox::stateChanged, this, &GeometryEngine::HandleSymmetricalCheckBoxChanged);

   QTreeWidgetItem* ignitionDelayItem = new QTreeWidgetItem(treeWidgetItem, QStringList("Ignition Delay"));
   ignitionDelayItem->setToolTip(0, "The amount of time in seconds between the platform creation and engine ignition");
   auto ignitionDelayLineEdit = new MoverCreatorLineEdit(ignitionDelayItem);
   ignitionDelayLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   mIgnitionDelayEditPtr = ignitionDelayLineEdit;
   aTreeWidgetPtr->setItemWidget(ignitionDelayItem, 1, ignitionDelayLineEdit);
   aTreeWidgetPtr->setItemWidget(ignitionDelayItem, 2, new QLabel(QString::fromStdString("sec")));
   ignitionDelayItem->setHidden(mVehiclePtr->IsAircraft());
   connect(ignitionDelayLineEdit, &QLineEdit::textChanged, this, &GeometryEngine::HandleIgnitionDelayLineEditChanged);

   mLatchFuelInjectionItem = new QTreeWidgetItem(treeWidgetItem, QStringList("Latch Fuel Injection"));
   mLatchFuelInjectionItem->setToolTip(0, "If checked, the engine will use the latch_fuel_injection option");
   QCheckBox* latchFuelInjectionCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(mLatchFuelInjectionItem, 1, latchFuelInjectionCheckBox);
   aTreeWidgetPtr->setItemWidget(mLatchFuelInjectionItem, 2, new QLabel(QString::fromStdString("")));
   connect(latchFuelInjectionCheckBox, &QCheckBox::stateChanged, this, &GeometryEngine::HandleLatchFuelInjectionCheckBoxChanged);

   mUseProportionalThrottleItem = new QTreeWidgetItem(treeWidgetItem, QStringList("Use Proportional Throttle"));
   mUseProportionalThrottleItem->setToolTip(0, "If checked, the engine will use the use_proportional_throttle option");
   QCheckBox* useProportionalThrottleCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(mUseProportionalThrottleItem, 1, useProportionalThrottleCheckBox);
   aTreeWidgetPtr->setItemWidget(mUseProportionalThrottleItem, 2, new QLabel(QString::fromStdString("")));
   connect(useProportionalThrottleCheckBox, &QCheckBox::stateChanged, this, &GeometryEngine::HandleUseProportionalThrottleCheckBoxChanged);

   mMinProportionalThrottleItem = new QTreeWidgetItem(treeWidgetItem, QStringList("Min Proportional Throttle"));
   mMinProportionalThrottleItem->setToolTip(0, "Sets the minimum proportional throttle when proportional throttle is enabled");
   QDoubleSpinBox* minProportionalThrottle = new QDoubleSpinBox();
   minProportionalThrottle->setMinimum(0);
   minProportionalThrottle->setMaximum(1);
   minProportionalThrottle->setDecimals(2);
   minProportionalThrottle->setSingleStep(0.1);
   aTreeWidgetPtr->setItemWidget(mMinProportionalThrottleItem, 1, minProportionalThrottle);
   aTreeWidgetPtr->setItemWidget(mMinProportionalThrottleItem, 2, new QLabel(QString::fromStdString("")));
   mMinProportionalThrottleItem->setDisabled(true);
   minProportionalThrottle->setDisabled(true);
   connect(minProportionalThrottle, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &GeometryEngine::HandleMinProportionalThrottleLineEditChanged);

   connect(mEngineTypeComboBox, &QComboBox::currentTextChanged, this, [this, aTreeWidgetPtr, typeItem](QString aType)
   {
      aTreeWidgetPtr->setCurrentItem(typeItem);
      HandleEngineTypeChanged(aType);
   });

   connect(mEngineModelComboBox, &QComboBox::currentTextChanged, this, [this, aTreeWidgetPtr, modelItem](QString aModel)
   {
      aTreeWidgetPtr->setCurrentItem(modelItem);
      HandleEngineModelChanged(aModel);
   });

   return treeWidgetItem;
}

void Designer::GeometryEngine::AdjustViewingDistance()
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Designer::GeometryWidget* geometry = mw.GetMoverCreatorWidget()->GetGeometryWidget();
   if (geometry != nullptr)
   {
      Designer::GeometryGLWidget* glObj = geometry->GetGeometryGLWidget();
      glObj->AdjustViewingDistance();
   }
}

void Designer::GeometryEngine::PopulateEngineTypes()
{
   QString dirPath = MoverCreatorMainWindow::GetDataPath() + "/Engines/";
   QDir listDir(dirPath);
   if (listDir.exists())
   {
      for (auto& curItem : listDir.entryInfoList())
      {
         if (curItem.isDir())
         {
            QString fileName = curItem.fileName();
            if (fileName != "." && fileName != "..")
            {
               mEngineTypeComboBox->addItem(fileName);
            }
         }
      }
   }
}

void Designer::GeometryEngine::HandleYawLineEditChanged(QString aValue)
{
   mYaw_rad = aValue.toDouble() * UtMath::cRAD_PER_DEG;
   AdjustAndRedraw(true);
}

void Designer::GeometryEngine::HandlePitchLineEditChanged(QString aValue)
{
   mPitch_rad = aValue.toDouble() * UtMath::cRAD_PER_DEG;
   AdjustAndRedraw(true);
}

void Designer::GeometryEngine::HandleRollLineEditChanged(QString aValue)
{
   mRoll_rad = aValue.toDouble() * UtMath::cRAD_PER_DEG;
   AdjustAndRedraw(true);
}

void Designer::GeometryEngine::HandleEngineTypeChanged(QString aType)
{
   if (QString::compare(aType, "Jet") == 0)
   {
      mEngineType = EngineType::cJET;
   }
   else if (QString::compare(aType, "Ramjet") == 0)
   {
      mEngineType = EngineType::cRAMJET;
   }
   else if (QString::compare(aType, "LiquidRocket") == 0)
   {
      mEngineType = EngineType::cLIQUIDROCKET;
   }
   else if (QString::compare(aType, "SolidRocket") == 0)
   {
      mEngineType = EngineType::cSOLIDROCKET;
   }

   mEngineModelComboBox->clear();
   QString dirPath = MoverCreatorMainWindow::GetDataPath() + "/Engines/";
   dirPath.append(aType);
   QDir listDir(dirPath);
   if (listDir.exists())
   {
      for (auto& curFile : listDir.entryInfoList())
      {
         if (curFile.isFile())
         {
            QString fileName = curFile.fileName();
            if (fileName.endsWith(".amc"))
            {
               if (!fileName.startsWith("~"))
               {
                  fileName.truncate(fileName.size() - 4);
                  mEngineModelComboBox->addItem(fileName);
               }
            }
         }
      }
   }

   // Engine Specific Options
   bool isRamjet = mEngineType == EngineType::cRAMJET;
   if (mLatchFuelInjectionItem != nullptr)
   {
      mLatchFuelInjectionItem->setHidden(!isRamjet);
   }
   if (mUseProportionalThrottleItem != nullptr)
   {
      mUseProportionalThrottleItem->setHidden(!isRamjet);
   }
   if (mMinProportionalThrottleItem != nullptr)
   {
      mMinProportionalThrottleItem->setHidden(!isRamjet);
   }
   AdjustAndRedraw(true);
}

void Designer::GeometryEngine::HandleEngineModelChanged(QString aModel)
{
   mEngineModel = aModel.toStdString();

   QString engineTypeString;
   switch (mEngineType)
   {
      case Designer::GeometryEngine::EngineType::cJET:
         engineTypeString = "Jet";
         break;
      case Designer::GeometryEngine::EngineType::cSOLIDROCKET:
         engineTypeString = "SolidRocket";
         break;
      case Designer::GeometryEngine::EngineType::cLIQUIDROCKET:
         engineTypeString = "LiquidRocket";
         break;
      case Designer::GeometryEngine::EngineType::cRAMJET:
         engineTypeString = "Ramjet";
         break;
      default:
         break;
   }

   if (mTreeWidgetItemPtr)
   {
      MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
      mw.GetMoverCreatorWidget()->GetGeometryWidget()->LoadEngineLengthDiameterThrustOffset(mTreeWidgetItemPtr, engineTypeString, QString::fromStdString(mEngineModel));
   }
}

void Designer::GeometryEngine::HandleSymmetricalCheckBoxChanged(int aValue)
{
   if (aValue == 0)
   {
      mSymmetrical = false;
   }
   else
   {
      mSymmetrical = true;
   }

   AdjustAndRedraw(true);
}

void Designer::GeometryEngine::HandleLatchFuelInjectionCheckBoxChanged(int aValue)
{
   if (aValue == 0)
   {
      mLatchFuelInjection = false;
   }
   else
   {
      mLatchFuelInjection = true;
   }
   AdjustAndRedraw(true);
}

void Designer::GeometryEngine::HandleUseProportionalThrottleCheckBoxChanged(int aValue)
{
   if (aValue == 0)
   {
      mUseProportionalThrottle = false;
   }
   else
   {
      mUseProportionalThrottle = true;
   }

   mMinProportionalThrottleItem->setDisabled(!mUseProportionalThrottle);
   QWidget* itemWidget = mMinProportionalThrottleItem->treeWidget()->itemWidget(mMinProportionalThrottleItem, 1);
   if (itemWidget != nullptr)
   {
      itemWidget->setEnabled(mUseProportionalThrottle);
   }
   AdjustAndRedraw(true);
}

void Designer::GeometryEngine::HandleMinProportionalThrottleLineEditChanged(double aValue)
{
   mMinProportionalThrottle = aValue;
   AdjustAndRedraw(true);
}

void Designer::GeometryEngine::HandleLengthLineEditChanged(QString aValue)
{
   mLength_ft = aValue.toDouble();
   AdjustAndRedraw(true);
}

void Designer::GeometryEngine::HandleDiameterLineEditChanged(QString aValue)
{
   mDiameter_ft = aValue.toDouble();
   AdjustAndRedraw(true);
}

void Designer::GeometryEngine::HandleThrustOffsetLineEditChanged(QString aValue)
{
   mThrustOffset_ft = aValue.toDouble();
   AdjustAndRedraw(true);
}

void Designer::GeometryEngine::HandleMassLineEditChanged(QString aValue)
{
   mMass_lbs = aValue.toDouble();
   AdjustAndRedraw(true);
}

void Designer::GeometryEngine::HandlepropellantMassLineEditChanged(QString aValue)
{
   mPropellantMass_lbs = aValue.toDouble();
   AdjustAndRedraw(true);
}

void Designer::GeometryEngine::HandleIgnitionDelayLineEditChanged(QString aValue)
{
   mIgnitionDelay_sec = aValue.toDouble();
   AdjustAndRedraw(true);
}

void Designer::GeometryEngine::HandleTreeItemClicked(QTreeWidgetItem* item, int column)
{
   AdjustAndRedraw();
}

void Designer::GeometryEngine::EditEngineModelClicked()
{
   if (mEditEngineDialog == nullptr)
   {
      mEditEngineDialog = ut::make_unique<EditEngineDialog>(nullptr);
   }
   mEditEngineDialog->show();
   mEditEngineDialog->SetEngineTypeAndModel(mEngineTypeComboBox->currentText(), mEngineModelComboBox->currentText());
}

void Designer::GeometryEngine::CreateEngineModelClicked()
{
   if (mCreateEngineDialog == nullptr)
   {
      mCreateEngineDialog = ut::make_unique<CreateEngineDialog>(nullptr);
   }
   mCreateEngineDialog->show();
   mCreateEngineDialog->SetEngineTypeAndModel(mEngineTypeComboBox->currentText(), mEngineModelComboBox->currentText());

   connect(mCreateEngineDialog.get(), &CreateEngineDialog::EngineCreated, this, &GeometryEngine::HandleEngineCreated);
}

void Designer::GeometryEngine::HandleEngineCreated(QString aModelName)
{
   mEngineModel = aModelName.toStdString();
   mEngineModelComboBox->addItem(aModelName);
   mEngineModelComboBox->setCurrentText(aModelName);
}
