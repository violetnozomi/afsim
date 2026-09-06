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

#include "GeometrySpeedBrake.hpp"

#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QTreeWidget>

#include "MoverCreatorLineEdit.hpp"
#include "P6DofUtils.hpp"
#include "UtMath.hpp"

namespace Designer
{

GeometrySpeedBrake::GeometrySpeedBrake(Vehicle* aVehicle)
   : GeometryObject(aVehicle)
{
   // Overwrite the GeometryObject type
   mGeometryObjectTypeString.assign("GeometrySpeedBrake");
}

GeometrySpeedBrake::~GeometrySpeedBrake()
{
}

void GeometrySpeedBrake::MoveRefPoint(UtVec3dX aMoveDelta_ft)
{
   mSymmetryHorizontalY += aMoveDelta_ft.Y();
   mSymmetryVerticalZ += aMoveDelta_ft.Z();

   UtVec3dX newRefPoint_ft = mRefPoint_ft + aMoveDelta_ft;
   SetRefPoint(newRefPoint_ft);
}

QTreeWidgetItem* GeometrySpeedBrake::CreateTreeWidgetItem(QString& aName, QTreeWidget* aTreeWidgetPtr, bool aNoneditableName)
{
   // Create the tree widget item at the GeometryObject level
   QTreeWidgetItem* treeWidgetItem = GeometryObject::CreateTreeWidgetItem(aName, aTreeWidgetPtr, aNoneditableName);

   // Enforce the geometry type
   mGeometryObjectTypeString = "GeometrySpeedBrake";

   // Set Qt flags
   if (aNoneditableName)
   {
      treeWidgetItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
   }
   else
   {
      treeWidgetItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
   }

   QTreeWidgetItem* typeItem = new QTreeWidgetItem(treeWidgetItem, QStringList("Type of Object"));
   typeItem->setToolTip(0, "The type of this object");
   aTreeWidgetPtr->setItemWidget(typeItem, 1, new QLabel(QString::fromStdString(mGeometryObjectTypeString)));
   aTreeWidgetPtr->setItemWidget(typeItem, 2, new QLabel(QString::fromStdString("")));

   QTreeWidgetItem* dragCoefficient = new QTreeWidgetItem(treeWidgetItem, QStringList("Cd Multiplier"));
   dragCoefficient->setToolTip(0, "The drag coefficient multiplier of the speed brake");
   auto dragCoefficientLineEdit = new MoverCreatorLineEdit(dragCoefficient);
   dragCoefficientLineEdit->setFrame(false);
   dragCoefficientLineEdit->setText(QString::number(mCdMultiplier));
   aTreeWidgetPtr->setItemWidget(dragCoefficient, 1, dragCoefficientLineEdit);
   connect(dragCoefficientLineEdit, &QLineEdit::textChanged, this, &GeometrySpeedBrake::HandleDragCoefficientMultiplierLineEditChanged);
   connect(dragCoefficientLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* length = new QTreeWidgetItem(treeWidgetItem, QStringList("Length"));
   length->setToolTip(0, "The length of the speed brake in feet");
   auto lengthLineEdit = new MoverCreatorLineEdit(length);
   lengthLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   lengthLineEdit->setFrame(false);
   lengthLineEdit->setText(QString::number(mLength_ft));
   aTreeWidgetPtr->setItemWidget(length, 1, lengthLineEdit);
   aTreeWidgetPtr->setItemWidget(length, 2, new QLabel(QString::fromStdString("ft")));
   connect(lengthLineEdit, &QLineEdit::textChanged, this, &GeometrySpeedBrake::HandleLengthLineEditChanged);
   connect(lengthLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* width = new QTreeWidgetItem(treeWidgetItem, QStringList("Width"));
   width->setToolTip(0, "The width of the speed brake in feet");
   auto widthLineEdit = new MoverCreatorLineEdit(width);
   widthLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   widthLineEdit->setFrame(false);
   widthLineEdit->setText(QString::number(mWidth_ft));
   aTreeWidgetPtr->setItemWidget(width, 1, widthLineEdit);
   aTreeWidgetPtr->setItemWidget(width, 2, new QLabel(QString::fromStdString("ft")));
   connect(widthLineEdit, &QLineEdit::textChanged, this, &GeometrySpeedBrake::HandleWidthLineEditChanged);
   connect(widthLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* roll = new QTreeWidgetItem(treeWidgetItem, QStringList("Roll Angle"));
   roll->setToolTip(0, "The roll angle of the speed brake in degrees");
   auto rollLineEdit = new MoverCreatorLineEdit(roll);
   rollLineEdit->setFrame(false);
   rollLineEdit->setText(QString::number(mRoll_rad * UtMath::cDEG_PER_RAD));
   aTreeWidgetPtr->setItemWidget(roll, 1, rollLineEdit);
   aTreeWidgetPtr->setItemWidget(roll, 2, new QLabel(QString::fromStdString("deg")));
   connect(rollLineEdit, &QLineEdit::textChanged, this, &GeometrySpeedBrake::HandleRollLineEditChanged);
   connect(rollLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* massFactor = new QTreeWidgetItem(treeWidgetItem, QStringList("Mass Factor"));
   massFactor->setToolTip(0, "The mass factor of the speed brake");
   auto massFactorLineEdit = new MoverCreatorLineEdit(massFactor);
   massFactorLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   massFactorLineEdit->setFrame(false);
   massFactorLineEdit->setText(QString::number(mMassFactor));
   aTreeWidgetPtr->setItemWidget(massFactor, 1, massFactorLineEdit);
   aTreeWidgetPtr->setItemWidget(massFactor, 2, new QLabel(QString::fromStdString("")));
   connect(massFactorLineEdit, &QLineEdit::textChanged, this, &GeometrySpeedBrake::HandleMassFactorLineEditChanged);
   connect(massFactorLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   massFactor->setHidden(true);

   QTreeWidgetItem* symmetryItem = new QTreeWidgetItem(treeWidgetItem, QStringList("Symmetry Type"));
   symmetryItem->setToolTip(0, "If checked, the speed brake will be treated as symmetrical. It can use either horizontal or vertical symmetry.");
   QComboBox* symmetryCB = new QComboBox();
   QStringList symmetryStrings;
   symmetryStrings << "Single" << "Horizontal" << "Vertical";
   symmetryCB->addItems(symmetryStrings);
   aTreeWidgetPtr->setItemWidget(symmetryItem, 1, symmetryCB);
   aTreeWidgetPtr->setItemWidget(symmetryItem, 2, new QLabel(QString::fromStdString("")));
   connect(symmetryCB, &QComboBox::currentTextChanged, this, [this, aTreeWidgetPtr, symmetryItem](QString aSymmetry)
   {
      aTreeWidgetPtr->setCurrentItem(symmetryItem);
      HandleSymmetricalComboBoxChanged(aSymmetry);
   });

   QTreeWidgetItem* maxAngle = new QTreeWidgetItem(treeWidgetItem, QStringList("Max Angle"));
   maxAngle->setToolTip(0, "The maximum angle of the speed brake in degrees");
   auto maxAngleLineEdit = new MoverCreatorLineEdit(maxAngle);
   maxAngleLineEdit->setFrame(false);
   maxAngleLineEdit->setText(QString::number(mMaxAngle_deg));
   aTreeWidgetPtr->setItemWidget(maxAngle, 1, maxAngleLineEdit);
   aTreeWidgetPtr->setItemWidget(maxAngle, 2, new QLabel(QString::fromStdString("deg")));
   connect(maxAngleLineEdit, &QLineEdit::textChanged, this, &GeometrySpeedBrake::HandleMaxAngleLineEditChanged);
   connect(maxAngleLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   // Save pointer to widget
   mMaxAngleWidget = maxAngle;

   QTreeWidgetItem* actuatorMinRate = new QTreeWidgetItem(treeWidgetItem, QStringList("Max Retraction Rate"));
   actuatorMinRate->setToolTip(0, "The maximum retraction rate in degrees/second");
   auto actuatorMinRateLineEdit = new MoverCreatorLineEdit(actuatorMinRate);
   actuatorMinRateLineEdit->setFrame(false);
   actuatorMinRateLineEdit->setText(QString::number(mMaxRetractionRate_dps));
   aTreeWidgetPtr->setItemWidget(actuatorMinRate, 1, actuatorMinRateLineEdit);
   aTreeWidgetPtr->setItemWidget(actuatorMinRate, 2, new QLabel(QString::fromStdString("deg/sec")));
   connect(actuatorMinRateLineEdit, &QLineEdit::textChanged, this, &GeometrySpeedBrake::HandleMaxRetractionRateLineEditChanged);
   connect(actuatorMinRateLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* actuatorMaxRate = new QTreeWidgetItem(treeWidgetItem, QStringList("Max Extension Rate"));
   actuatorMaxRate->setToolTip(0, "The maximum extension rate in degrees/second");
   auto actuatorMaxRateLineEdit = new MoverCreatorLineEdit(actuatorMaxRate);
   actuatorMaxRateLineEdit->setFrame(false);
   actuatorMaxRateLineEdit->setText(QString::number(mMaxExtensionRate_dps));
   aTreeWidgetPtr->setItemWidget(actuatorMaxRate, 1, actuatorMaxRateLineEdit);
   aTreeWidgetPtr->setItemWidget(actuatorMaxRate, 2, new QLabel(QString::fromStdString("deg/sec")));
   connect(actuatorMaxRateLineEdit, &QLineEdit::textChanged, this, &GeometrySpeedBrake::HandleMaxExtensionRateLineEditChanged);
   connect(actuatorMaxRateLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   return treeWidgetItem;
}

void GeometrySpeedBrake::HandleDragCoefficientMultiplierLineEditChanged(QString aValue)
{
   mCdMultiplier = aValue.toDouble();
}

void GeometrySpeedBrake::HandleMaxAngleLineEditChanged(QString aValue)
{
   mMaxAngle_deg = aValue.toDouble();
   AdjustAndRedraw(false);
}

void GeometrySpeedBrake::HandleLengthLineEditChanged(QString aValue)
{
   mLength_ft = aValue.toDouble();
   AdjustAndRedraw(false);
}

void GeometrySpeedBrake::HandleSymmetricalComboBoxChanged(QString aValue)
{
   mSymmetryString = aValue;

   if (mSymmetryString == "Horizontal")
   {
      mHorizontallySymmetrical = true;
      mVerticallySymmetrical = false;
   }
   else if (mSymmetryString == "Vertical")
   {
      mHorizontallySymmetrical = false;
      mVerticallySymmetrical = true;
   }
   else
   {
      mHorizontallySymmetrical = false;
      mVerticallySymmetrical = false;
   }

   AdjustAndRedraw(false);
}

void GeometrySpeedBrake::HandleWidthLineEditChanged(QString aValue)
{
   mWidth_ft = aValue.toDouble();
   AdjustAndRedraw(false);
}

void GeometrySpeedBrake::HandleRollLineEditChanged(QString aValue)
{
   mRoll_rad = aValue.toDouble() * UtMath::cRAD_PER_DEG;
   AdjustAndRedraw(false);
}

void GeometrySpeedBrake::CalcMassProperties(double) // Although a derived function, it ignores aTotalMass_lbs
{
   // Speed brakes lack any mass properties to calculate

   // Set the mass - always zero
   mCalculatedMass_lbs = 0.0;

   // Set inertia - always zero
   mCalculatedIxx = 0.0;
   mCalculatedIyy = 0.0;
   mCalculatedIzz = 0.0;

   // Set cg - always zero
   mCalculatedCG_ft.Set(0.0, 0.0, 0.0);
}

bool GeometrySpeedBrake::GetCalculatedMassProperties(UtVec3dX& aCgLocation_ft,
                                                     double&   aMass_lbs,
                                                     UtVec3dX& aRotationalInertia)
{
   aCgLocation_ft = mCalculatedCG_ft;
   aMass_lbs = mCalculatedMass_lbs;
   aRotationalInertia.Set(mCalculatedIxx, mCalculatedIyy, mCalculatedIzz);

   return false; // Speed brakes lack any mass properties
}

QStringList GeometrySpeedBrake::Audit()
{
   QStringList errorList = GeometryObject::Audit();

   QString nameString(mName.c_str());

   if (mLength_ft < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a length (mLength_ft is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   if (mWidth_ft < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a width (mWidth_ft is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   return errorList;
}

void GeometrySpeedBrake::HandleMaxRetractionRateLineEditChanged(QString aValue)
{
   mMaxRetractionRate_dps = aValue.toDouble();
}

void GeometrySpeedBrake::HandleMaxExtensionRateLineEditChanged(QString aValue)
{
   mMaxExtensionRate_dps = aValue.toDouble();
}

} // namespace Designer
