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

#include "GeometryDish.hpp"

#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QTreeWidget>

#include "MoverCreatorLineEdit.hpp"

#include "P6DofUtils.hpp"
#include "UtMath.hpp"

namespace Designer
{

GeometryDish::GeometryDish(Vehicle* aVehicle)
   : GeometryObject(aVehicle)
{
   // Overwrite the GeometryDish type
   mGeometryObjectTypeString.assign("GeometryDish");
}

GeometryDish::~GeometryDish()
{
}

QTreeWidgetItem* GeometryDish::CreateTreeWidgetItem(QString& aName, QTreeWidget* aTreeWidgetPtr, bool aNoneditableName)
{
   // Create the tree widget item at the GeometryObject level
   QTreeWidgetItem* treeWidgetItem = GeometryObject::CreateTreeWidgetItem(aName, aTreeWidgetPtr, aNoneditableName);

   // Enforce the geometry type
   mGeometryObjectTypeString = "GeometryDish";

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

   QTreeWidgetItem* heightItem = new QTreeWidgetItem(treeWidgetItem, QStringList("Diameter"));
   heightItem->setToolTip(0, "The diameter of the dish in feet");
   auto heightLineEdit = new MoverCreatorLineEdit(heightItem);
   heightLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   heightLineEdit->setFrame(false);
   heightLineEdit->setText(QString::number(mDiameter_ft));
   aTreeWidgetPtr->setItemWidget(heightItem, 1, heightLineEdit);
   aTreeWidgetPtr->setItemWidget(heightItem, 2, new QLabel(QString::fromStdString("ft")));
   connect(heightLineEdit, &QLineEdit::textChanged, this, &GeometryDish::HandleDiameterLineEditChanged);
   connect(heightLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* lengthItem = new QTreeWidgetItem(treeWidgetItem, QStringList("Thickness"));
   lengthItem->setToolTip(0, "The thickness of the dish in feet");
   auto lengthLineEdit = new MoverCreatorLineEdit(lengthItem);
   lengthLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   lengthLineEdit->setFrame(false);
   lengthLineEdit->setText(QString::number(mThickness_ft));
   aTreeWidgetPtr->setItemWidget(lengthItem, 1, lengthLineEdit);
   aTreeWidgetPtr->setItemWidget(lengthItem, 2, new QLabel(QString::fromStdString("ft")));
   connect(lengthLineEdit, &QLineEdit::textChanged, this, &GeometryDish::HandleThicknessLineEditChanged);
   connect(lengthLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* massFactor = new QTreeWidgetItem(treeWidgetItem, QStringList("Mass Factor"));
   massFactor->setToolTip(0, "The mass factor multiplier of the dish");
   auto massFactorLineEdit = new MoverCreatorLineEdit(massFactor);
   massFactorLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   massFactorLineEdit->setFrame(false);
   massFactorLineEdit->setText(QString::number(mMassFactor));
   aTreeWidgetPtr->setItemWidget(massFactor, 1, massFactorLineEdit);
   aTreeWidgetPtr->setItemWidget(massFactor, 2, new QLabel(QString::fromStdString("")));
   connect(massFactorLineEdit, &QLineEdit::textChanged, this, &GeometryDish::HandleMassFactorLineEditChanged);
   connect(massFactorLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   return treeWidgetItem;
}

void GeometryDish::HandleThicknessLineEditChanged(QString aValue)
{
   mThickness_ft = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryDish::HandleDiameterLineEditChanged(QString aValue)
{
   mDiameter_ft = aValue.toDouble();
   AdjustAndRedraw(true);
}

bool GeometryDish::CalcExtents(double& xMin, double& xMax, double& yMin, double& yMax, double& zMin, double& zMax)
{
   UtVec3dX vecRefPt = mRefPoint_ft;
   double xRef = vecRefPt.X();
   double yRef = vecRefPt.Y();
   double zRef = vecRefPt.Z();
   double radius_ft = 0.5 * mDiameter_ft;

   xMin = xRef - radius_ft;
   xMax = xRef + radius_ft;

   yMin = yRef - radius_ft;
   yMax = yRef + radius_ft;

   zMin = zRef - mThickness_ft;
   zMax = zRef + mThickness_ft;

   return true;
}

double GeometryDish::GetMassArea_ft2()
{
   double r = 0.5 * mDiameter_ft;
   return UtMath::cPI * (r * r);
}

void GeometryDish::CalcMassProperties(double aTotalMass_lbs)
{
   // Set the mass
   mCalculatedMass_lbs = aTotalMass_lbs;

   // We assume a dish is roughly similar to a solid cylinder:
   // Ixx = (1/4) * mass * (radius*radius) + (1/12) * mass * (length*length)
   // Iyy = (1/4) * mass * (radius*radius) + (1/12) * mass * (length*length)
   // Izz = (1/2) * mass * (radius*radius)

   double radius = mDiameter_ft * 0.5;
   double mass_slugs = mCalculatedMass_lbs * mLbsToSlugs;
   double radiusSquared = radius * radius;
   double lengthSquared = mThickness_ft * mThickness_ft;

   mCalculatedIxx = (0.25 * mass_slugs * radiusSquared) + ((1.0 / 12.0) * mass_slugs * lengthSquared);
   mCalculatedIyy = (0.25 * mass_slugs * radiusSquared) + ((1.0 / 12.0) * mass_slugs * lengthSquared);
   mCalculatedIzz = 0.5 * mass_slugs * radiusSquared;

   // Correct for location, using parallel axis theorem
   double distX = sqrt(mRefPoint_ft.Y() * mRefPoint_ft.Y() + mRefPoint_ft.Z() * mRefPoint_ft.Z());
   double distY = sqrt(mRefPoint_ft.X() * mRefPoint_ft.X() + mRefPoint_ft.Z() * mRefPoint_ft.Z());
   double distZ = sqrt(mRefPoint_ft.X() * mRefPoint_ft.X() + mRefPoint_ft.Y() * mRefPoint_ft.Y());
   double dX2 = distX * distX;
   double dY2 = distY * distY;
   double dZ2 = distZ * distZ;

   mCalculatedIxx += mass_slugs * dX2;
   mCalculatedIyy += mass_slugs * dY2;
   mCalculatedIzz += mass_slugs * dZ2;

   // Calculate the location of the cg -- we assume it is always at the reference point
   mCalculatedCG_ft = mRefPoint_ft;
}

bool GeometryDish::GetCG_ft(UtVec3dX& aCg_ft)
{
   aCg_ft = mCalculatedCG_ft;
   return true;
}

QStringList GeometryDish::Audit()
{
   QStringList errorList = GeometryObject::Audit();

   QString nameString(mName.c_str());

   if (mDiameter_ft < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a diameter (mDiameter_ft is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   if (mThickness_ft < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a thickness (mThickness_ft is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   return errorList;
}

} // namespace Designer
