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

#include "GeometryLandingGear.hpp"

#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QTreeWidget>

#include "MoverCreatorLineEdit.hpp"

#include "P6DofUtils.hpp"
#include "UtMath.hpp"

namespace Designer
{

GeometryLandingGear::GeometryLandingGear(Vehicle* aVehicle)
   : GeometryObject(aVehicle)
{
   // Overwrite the GeometryObject type
   mGeometryObjectTypeString.assign("GeometryLandingGear");
}

GeometryLandingGear::~GeometryLandingGear()
{
}

void GeometryLandingGear::MoveRefPoint(UtVec3dX aMoveDelta_ft)
{
   mSymmetryHorizontalY += aMoveDelta_ft.Y();

   UtVec3dX newRefPoint_ft = mRefPoint_ft + aMoveDelta_ft;
   SetRefPoint(newRefPoint_ft);
}

QTreeWidgetItem* GeometryLandingGear::CreateTreeWidgetItem(QString& aName, QTreeWidget* aTreeWidgetPtr, bool aNoneditableName)
{
   // Create the tree widget item at the GeometryObject level
   QTreeWidgetItem* treeWidgetItem = GeometryObject::CreateTreeWidgetItem(aName, aTreeWidgetPtr, aNoneditableName);

   // Enforce the geometry type
   mGeometryObjectTypeString = "GeometryLandingGear";

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
   typeItem->setToolTip(0, "The type of this object.");
   aTreeWidgetPtr->setItemWidget(typeItem, 1, new QLabel(QString::fromStdString(mGeometryObjectTypeString)));
   aTreeWidgetPtr->setItemWidget(typeItem, 2, new QLabel(QString::fromStdString("")));

   QTreeWidgetItem* dragCoefficient = new QTreeWidgetItem(treeWidgetItem, QStringList("Cd Multiplier"));
   dragCoefficient->setToolTip(0, "The drag coefficient of the landing gear");
   auto dragCoefficientLineEdit = new MoverCreatorLineEdit(dragCoefficient);
   dragCoefficientLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   dragCoefficientLineEdit->setFrame(false);
   dragCoefficientLineEdit->setText(QString::number(mCdMultiplier));
   aTreeWidgetPtr->setItemWidget(dragCoefficient, 1, dragCoefficientLineEdit);
   connect(dragCoefficientLineEdit, &QLineEdit::textChanged, this, &GeometryLandingGear::HandleDragCoefficientMultiplierLineEditChanged);
   connect(dragCoefficientLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* length = new QTreeWidgetItem(treeWidgetItem, QStringList("Uncompressed Length"));
   length->setToolTip(0, "The length of the landing gear when uncompressed in feet");
   auto lengthLineEdit = new MoverCreatorLineEdit(length);
   lengthLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   lengthLineEdit->setFrame(false);
   lengthLineEdit->setText(QString::number(mUncompressedLength_ft));
   aTreeWidgetPtr->setItemWidget(length, 1, lengthLineEdit);
   aTreeWidgetPtr->setItemWidget(length, 2, new QLabel(QString::fromStdString("ft")));
   connect(lengthLineEdit, &QLineEdit::textChanged, this, &GeometryLandingGear::HandleUncompressedLengthLineEditChanged);
   connect(lengthLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* maxCompression = new QTreeWidgetItem(treeWidgetItem, QStringList("Max Compression"));
   maxCompression->setToolTip(0, "The max compression of the landing gear in feet");
   auto maxCompressionLineEdit = new MoverCreatorLineEdit(maxCompression);
   maxCompressionLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   maxCompressionLineEdit->setFrame(false);
   maxCompressionLineEdit->setText(QString::number(mMaxCompression_ft));
   aTreeWidgetPtr->setItemWidget(maxCompression, 1, maxCompressionLineEdit);
   aTreeWidgetPtr->setItemWidget(maxCompression, 2, new QLabel(QString::fromStdString("ft")));
   connect(maxCompressionLineEdit, &QLineEdit::textChanged, this, &GeometryLandingGear::HandleMaxCompressionLineEditChanged);
   connect(maxCompressionLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* strutDiam = new QTreeWidgetItem(treeWidgetItem, QStringList("Strut Diam"));
   strutDiam->setToolTip(0, "The diameter of the landing gear strut in feet");
   auto strutDiamLineEdit = new MoverCreatorLineEdit(strutDiam);
   strutDiamLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   strutDiamLineEdit->setFrame(false);
   strutDiamLineEdit->setText(QString::number(mStrutDiam_ft));
   aTreeWidgetPtr->setItemWidget(strutDiam, 1, strutDiamLineEdit);
   aTreeWidgetPtr->setItemWidget(strutDiam, 2, new QLabel(QString::fromStdString("ft")));
   connect(strutDiamLineEdit, &QLineEdit::textChanged, this, &GeometryLandingGear::HandleStrutDiamLineEditChanged);
   connect(strutDiamLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* tireDiam = new QTreeWidgetItem(treeWidgetItem, QStringList("Tire Diam"));
   tireDiam->setToolTip(0, "The diameter of the tire(s) in feet");
   auto tireDiamLineEdit = new MoverCreatorLineEdit(tireDiam);
   tireDiamLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   tireDiamLineEdit->setFrame(false);
   tireDiamLineEdit->setText(QString::number(mTireDiam_ft));
   aTreeWidgetPtr->setItemWidget(tireDiam, 1, tireDiamLineEdit);
   aTreeWidgetPtr->setItemWidget(tireDiam, 2, new QLabel(QString::fromStdString("ft")));
   connect(tireDiamLineEdit, &QLineEdit::textChanged, this, &GeometryLandingGear::HandleTireDiamLineEditChanged);
   connect(tireDiamLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* tireWidth = new QTreeWidgetItem(treeWidgetItem, QStringList("Tire Width"));
   tireWidth->setToolTip(0, "The width of the tire(s) in feet");
   auto tireWidthLineEdit = new MoverCreatorLineEdit(tireWidth);
   tireWidthLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   tireWidthLineEdit->setFrame(false);
   tireWidthLineEdit->setText(QString::number(mTireWidth_ft));
   aTreeWidgetPtr->setItemWidget(tireWidth, 1, tireWidthLineEdit);
   aTreeWidgetPtr->setItemWidget(tireWidth, 2, new QLabel(QString::fromStdString("ft")));
   connect(tireWidthLineEdit, &QLineEdit::textChanged, this, &GeometryLandingGear::HandleTireWidthLineEditChanged);
   connect(tireWidthLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem*  springConstant = new QTreeWidgetItem(treeWidgetItem, QStringList("Spring Constant"));
   springConstant->setToolTip(0, "The spring constant of the landing gear in lbs/feet");
   auto springConstantLineEdit = new MoverCreatorLineEdit(springConstant);
   springConstantLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   springConstantLineEdit->setFrame(false);
   springConstantLineEdit->setText(QString::number(mSpringConstant));
   aTreeWidgetPtr->setItemWidget(springConstant, 1, springConstantLineEdit);
   aTreeWidgetPtr->setItemWidget(springConstant, 2, new QLabel(QString::fromStdString("lbs/ft")));
   connect(springConstantLineEdit, &QLineEdit::textChanged, this, &GeometryLandingGear::HandleSpringConstantLineEditChanged);
   connect(springConstantLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem*  damperConstant = new QTreeWidgetItem(treeWidgetItem, QStringList("Damper Constant"));
   damperConstant->setToolTip(0, "The damper constant of the landing gear in lbs/fps");
   auto damperConstantLineEdit = new MoverCreatorLineEdit(damperConstant);
   damperConstantLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   damperConstantLineEdit->setFrame(false);
   damperConstantLineEdit->setText(QString::number(mDamperConstant));
   aTreeWidgetPtr->setItemWidget(damperConstant, 1, damperConstantLineEdit);
   aTreeWidgetPtr->setItemWidget(damperConstant, 2, new QLabel(QString::fromStdString("lbs/fps")));
   connect(damperConstantLineEdit, &QLineEdit::textChanged, this, &GeometryLandingGear::HandleDamperConstantLineEditChanged);
   connect(damperConstantLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem*  rollingCoefficient = new QTreeWidgetItem(treeWidgetItem, QStringList("Rolling Friction Coefficient"));
   rollingCoefficient->setToolTip(0, "The friction coefficient of the landing gear when rolling");
   auto rollingCoefficientLineEdit = new MoverCreatorLineEdit(rollingCoefficient);
   rollingCoefficientLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   rollingCoefficientLineEdit->setFrame(false);
   rollingCoefficientLineEdit->setText(QString::number(mRollingFrictionCoefficient));
   aTreeWidgetPtr->setItemWidget(rollingCoefficient, 1, rollingCoefficientLineEdit);
   aTreeWidgetPtr->setItemWidget(rollingCoefficient, 2, new QLabel(QString::fromStdString("")));
   connect(rollingCoefficientLineEdit, &QLineEdit::textChanged, this, &GeometryLandingGear::HandleRollingFrictionCoefficientLineEditChanged);
   connect(rollingCoefficientLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem*  scuffingCoefficient = new QTreeWidgetItem(treeWidgetItem, QStringList("Scuffing Friction Coefficient"));
   scuffingCoefficient->setToolTip(0, "The friction coefficient of the landing gear when scuffing");
   auto scuffingCoefficientLineEdit = new MoverCreatorLineEdit(scuffingCoefficient);
   scuffingCoefficientLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   scuffingCoefficientLineEdit->setFrame(false);
   scuffingCoefficientLineEdit->setText(QString::number(mScuffingFrictionCoefficient));
   aTreeWidgetPtr->setItemWidget(scuffingCoefficient, 1, scuffingCoefficientLineEdit);
   aTreeWidgetPtr->setItemWidget(scuffingCoefficient, 2, new QLabel(QString::fromStdString("")));
   connect(scuffingCoefficientLineEdit, &QLineEdit::textChanged, this, &GeometryLandingGear::HandleScuffingFrictionCoefficientLineEditChanged);
   connect(scuffingCoefficientLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem*  brakingCoefficient = new QTreeWidgetItem(treeWidgetItem, QStringList("Braking Friction Coefficient"));
   brakingCoefficient->setToolTip(0, "The friction coefficient of the landing gear when braking");
   auto brakingCoefficientLineEdit = new MoverCreatorLineEdit(brakingCoefficient);
   brakingCoefficientLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   brakingCoefficientLineEdit->setFrame(false);
   brakingCoefficientLineEdit->setText(QString::number(mBrakingFrictionCoefficient));
   aTreeWidgetPtr->setItemWidget(brakingCoefficient, 1, brakingCoefficientLineEdit);
   aTreeWidgetPtr->setItemWidget(brakingCoefficient, 2, new QLabel(QString::fromStdString("")));
   connect(brakingCoefficientLineEdit, &QLineEdit::textChanged, this, &GeometryLandingGear::HandleBrakingFrictionCoefficientLineEditChanged);
   connect(brakingCoefficientLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* massFactor = new QTreeWidgetItem(treeWidgetItem, QStringList("Mass Factor"));
   massFactor->setToolTip(0, "The mass factor multiplier of the landing gear");
   auto massFactorLineEdit = new MoverCreatorLineEdit(massFactor);
   massFactorLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   massFactorLineEdit->setFrame(false);
   massFactorLineEdit->setText(QString::number(mMassFactor));
   aTreeWidgetPtr->setItemWidget(massFactor, 1, massFactorLineEdit);
   aTreeWidgetPtr->setItemWidget(massFactor, 2, new QLabel(QString::fromStdString("")));
   connect(massFactorLineEdit, &QLineEdit::textChanged, this, &GeometryLandingGear::HandleMassFactorLineEditChanged);
   connect(massFactorLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* symmetrical = new QTreeWidgetItem(treeWidgetItem, QStringList("Symmetrical"));
   symmetrical->setToolTip(0, "If checked, the fuselage will utilize symmetry");
   QCheckBox* symmetricalCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(symmetrical, 1, symmetricalCheckBox);
   aTreeWidgetPtr->setItemWidget(symmetrical, 2, new QLabel(QString::fromStdString("")));
   connect(symmetricalCheckBox, &QCheckBox::stateChanged, this, &GeometryLandingGear::HandleSymmetricalCheckBoxChanged);

   QTreeWidgetItem* maxAngle = new QTreeWidgetItem(treeWidgetItem, QStringList("Max Angle"));
   maxAngle->setToolTip(0, "The maximum angle of the landing gear in degrees");
   auto maxAngleLineEdit = new MoverCreatorLineEdit(maxAngle);
   maxAngleLineEdit->setFrame(false);
   maxAngleLineEdit->setText(QString::number(mMaxAngle_deg));
   aTreeWidgetPtr->setItemWidget(maxAngle, 1, maxAngleLineEdit);
   aTreeWidgetPtr->setItemWidget(maxAngle, 2, new QLabel(QString::fromStdString("deg")));
   connect(maxAngleLineEdit, &QLineEdit::textChanged, this, &GeometryLandingGear::HandleMaxAngleLineEditChanged);
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
   connect(actuatorMinRateLineEdit, &QLineEdit::textChanged, this, &GeometryLandingGear::HandleMaxRetractionRateLineEditChanged);
   connect(actuatorMinRateLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* actuatorMaxRate = new QTreeWidgetItem(treeWidgetItem, QStringList("Max Extension Rate"));
   actuatorMaxRate->setToolTip(0, "The maximum extension rate in degrees/second.");
   auto actuatorMaxRateLineEdit = new MoverCreatorLineEdit(actuatorMaxRate);
   actuatorMaxRateLineEdit->setFrame(false);
   actuatorMaxRateLineEdit->setText(QString::number(mMaxExtensionRate_dps));
   aTreeWidgetPtr->setItemWidget(actuatorMaxRate, 1, actuatorMaxRateLineEdit);
   aTreeWidgetPtr->setItemWidget(actuatorMaxRate, 2, new QLabel(QString::fromStdString("deg/sec")));
   connect(actuatorMaxRateLineEdit, &QLineEdit::textChanged, this, &GeometryLandingGear::HandleMaxExtensionRateLineEditChanged);
   connect(actuatorMaxRateLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   // For landing gear, we hide the max angle widget
   mMaxAngleWidget->setHidden(true);

   return treeWidgetItem;
}

void GeometryLandingGear::HandleDragCoefficientMultiplierLineEditChanged(QString aValue)
{
   mCdMultiplier = aValue.toDouble();
}

void GeometryLandingGear::HandleMaxAngleLineEditChanged(QString aValue)
{
   mMaxAngle_deg = aValue.toDouble();
}

void GeometryLandingGear::HandleUncompressedLengthLineEditChanged(QString aValue)
{
   mUncompressedLength_ft = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryLandingGear::HandleMaxCompressionLineEditChanged(QString aValue)
{
   mMaxCompression_ft = aValue.toDouble();
}

void GeometryLandingGear::HandleStrutDiamLineEditChanged(QString aValue)
{
   mStrutDiam_ft = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryLandingGear::HandleTireDiamLineEditChanged(QString aValue)
{
   mTireDiam_ft = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryLandingGear::HandleTireWidthLineEditChanged(QString aValue)
{
   mTireWidth_ft = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryLandingGear::HandleSymmetricalCheckBoxChanged(int aValue)
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

void GeometryLandingGear::HandleSpringConstantLineEditChanged(QString aValue)
{
   mSpringConstant = aValue.toDouble();
}

void GeometryLandingGear::HandleDamperConstantLineEditChanged(QString aValue)
{
   mDamperConstant = aValue.toDouble();
}

void GeometryLandingGear::HandleRollingFrictionCoefficientLineEditChanged(QString aValue)
{
   mRollingFrictionCoefficient = aValue.toDouble();
}

void GeometryLandingGear::HandleScuffingFrictionCoefficientLineEditChanged(QString aValue)
{
   mScuffingFrictionCoefficient = aValue.toDouble();
}

void GeometryLandingGear::HandleBrakingFrictionCoefficientLineEditChanged(QString aValue)
{
   mBrakingFrictionCoefficient = aValue.toDouble();
}

bool GeometryLandingGear::CalcExtents(double& xMin, double& xMax, double& yMin, double& yMax, double& zMin, double& zMax)
{
   // X-Direction
   xMin = mRefPoint_ft.X();
   xMax = xMin;

   // Y-Direction
   double y1 = 0;
   double y2 = 0;
   if (mSymmetrical)
   {
      double currentGearY = mRefPoint_ft.Y();
      double delta = currentGearY - mSymmetryHorizontalY;
      double otherGearY = currentGearY - 2.0 * delta;

      y1 = currentGearY;
      y2 = otherGearY;
   }
   else
   {
      y1 = mRefPoint_ft.Y();
      y2 = mRefPoint_ft.Y();
   }

   if (y1 < y2)
   {
      yMin = y1;
      yMax = y2;
   }
   else
   {
      yMin = y2;
      yMax = y1;
   }

   // Z-Direction
   zMax = mRefPoint_ft.Z() + mUncompressedLength_ft;
   zMin = mRefPoint_ft.Z();

   return true;
}

double GeometryLandingGear::GetMassArea_ft2()
{
   double pi = UtMath::cPI;
   double strutLength = mUncompressedLength_ft - 0.5 * mTireDiam_ft;
   double strutArea_ft2 = strutLength * (pi * mStrutDiam_ft);
   double tireArea_ft2 = mTireWidth_ft * mTireDiam_ft;
   return strutArea_ft2 + tireArea_ft2;
}

void GeometryLandingGear::CalcMassProperties(double aTotalMass_lbs)
{
   // Set the mass
   mCalculatedMass_lbs = aTotalMass_lbs;

   double mass_slugs = mCalculatedMass_lbs * mLbsToSlugs;

   if (mSymmetrical)
   {
      mass_slugs *= 0.5;  // Use half of the mass for each gear
   }

   // Since there is currently no means to handle changes in rotational inertia for
   // landing gear as they retract/extend, we simply treat landing gear as a point mass.
   mCalculatedIxx = 0.0;
   mCalculatedIyy = 0.0;
   mCalculatedIzz = 0.0;

   // Correct for location, using parallel axis theorem
   double distX = sqrt(mRefPoint_ft.Y() * mRefPoint_ft.Y() + mRefPoint_ft.Z() * mRefPoint_ft.Z());
   double distY = sqrt(mRefPoint_ft.X() * mRefPoint_ft.X() + mRefPoint_ft.Z() * mRefPoint_ft.Z());
   double distZ = sqrt(mRefPoint_ft.X() * mRefPoint_ft.X() + mRefPoint_ft.Y() * mRefPoint_ft.Y());
   double dX2 = distX * distX;
   double dY2 = distY * distY;
   double dZ2 = distZ * distZ;

   double calculatedIxx = mass_slugs * dX2;
   double calculatedIyy = mass_slugs * dY2;
   double calculatedIzz = mass_slugs * dZ2;

   if (!mSymmetrical)
   {
      mCalculatedIxx = calculatedIxx;
      mCalculatedIyy = calculatedIyy;
      mCalculatedIzz = calculatedIzz;

      // Calculate the location of the cg, assumed at the reference point
      mCalculatedCG_ft = mRefPoint_ft;
   }
   else
   {
      double currentGearY = mRefPoint_ft.Y();
      double delta = currentGearY - mSymmetryHorizontalY;
      double otherGearY = currentGearY - 2.0 * delta;

      UtVec3dX refPt = mRefPoint_ft;
      refPt.Set(refPt.X(), otherGearY, refPt.Z());
      distX = sqrt(refPt.Y() * refPt.Y() + refPt.Z() * refPt.Z());
      distY = sqrt(refPt.X() * refPt.X() + refPt.Z() * refPt.Z());
      distZ = sqrt(refPt.X() * refPt.X() + refPt.Y() * refPt.Y());
      dX2 = distX * distX;
      dY2 = distY * distY;
      dZ2 = distZ * distZ;

      double otherCalculatedIxx = mass_slugs * dX2;
      double otherCalculatedIyy = mass_slugs * dY2;
      double otherCalculatedIzz = mass_slugs * dZ2;

      mCalculatedIxx = calculatedIxx + otherCalculatedIxx;
      mCalculatedIyy = calculatedIyy + otherCalculatedIyy;
      mCalculatedIzz = calculatedIzz + otherCalculatedIzz;

      // Calculate the location of the cg -- we assume it is always at the reference point,
      // but centered in the Y-axis "between" the two objects
      double midGearY = (currentGearY + otherGearY) * 0.5;
      mCalculatedCG_ft.Set(mRefPoint_ft.X(), midGearY, mRefPoint_ft.Z());
   }
}

bool GeometryLandingGear::GetCG_ft(UtVec3dX& aCg_ft)
{
   aCg_ft = mCalculatedCG_ft;
   return true;
}

QStringList GeometryLandingGear::Audit()
{
   QStringList errorList = GeometryObject::Audit();

   QString nameString(mName.c_str());

   if (mUncompressedLength_ft < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires an uncompressed length (mUncompressedLength_ft is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   if (mMaxCompression_ft < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a max compression (mMaxCompression_ft is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   if (mStrutDiam_ft < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a strut diameter (mStrutDiam_ft is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   if (mTireDiam_ft < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a tire diameter (mTireDiam_ft is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   if (mTireWidth_ft < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a tire width (mTireWidth_ft is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   return errorList;
}

void GeometryLandingGear::HandleMaxRetractionRateLineEditChanged(QString aValue)
{
   mMaxRetractionRate_dps = aValue.toDouble();
}

void GeometryLandingGear::HandleMaxExtensionRateLineEditChanged(QString aValue)
{
   mMaxExtensionRate_dps = aValue.toDouble();
}

} // namespace Designer
