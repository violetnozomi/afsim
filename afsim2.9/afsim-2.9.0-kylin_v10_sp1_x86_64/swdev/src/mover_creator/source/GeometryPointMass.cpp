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

#include "GeometryPointMass.hpp"

#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QTreeWidget>

#include "GeometryBody.hpp"
#include "GeometryPropulsionData.hpp"
#include "GeometryWidget.hpp"
#include "MoverCreatorLineEdit.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorWidget.hpp"
#include "P6DofUtils.hpp"
#include "UtMath.hpp"
#include "VehicleGeometry.hpp"

namespace Designer
{

GeometryPointMass::GeometryPointMass(Vehicle* aVehicle)
   : GeometryObject(aVehicle)
{
   // Overwrite the GeometryObject type
   mGeometryObjectTypeString.assign("GeometryPointMass");
}

GeometryPointMass::~GeometryPointMass()
{
}

QTreeWidgetItem* GeometryPointMass::CreateTreeWidgetItem(QString& aName, QTreeWidget* aTreeWidgetPtr, bool aNoneditableName)
{
   // Create the tree widget item at the GeometryObject level
   QTreeWidgetItem* treeWidgetItem = GeometryObject::CreateTreeWidgetItem(aName, aTreeWidgetPtr, aNoneditableName);

   // Enforce the geometry type
   mGeometryObjectTypeString = "GeometryPointMass";

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

   QTreeWidgetItem* mass_lbs = new QTreeWidgetItem(treeWidgetItem, QStringList("Mass"));
   mass_lbs->setToolTip(0, "The mass of the point mass object in lbs");
   auto mass_lbsLineEdit = new MoverCreatorLineEdit(mass_lbs);
   mass_lbsLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   mass_lbsLineEdit->setFrame(false);
   mass_lbsLineEdit->setText(QString::number(mPointMass_lbs));
   aTreeWidgetPtr->setItemWidget(mass_lbs, 1, mass_lbsLineEdit);
   aTreeWidgetPtr->setItemWidget(mass_lbs, 2, new QLabel(QString::fromStdString("lbs")));
   connect(mass_lbsLineEdit, &QLineEdit::textChanged, this, &GeometryPointMass::HandleMassLineEditChanged);
   connect(mass_lbsLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   return treeWidgetItem;
}

bool GeometryPointMass::CalcExtents(double&, double&, double&, double&, double&, double&)
{
   return false;
}

void GeometryPointMass::HandleMassLineEditChanged(QString aValue)
{
   mPointMass_lbs = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryPointMass::HandleX_CoordinateLineEditChanged(QString aValue)
{
   mRefPoint_ft.Set(aValue.toDouble(), mRefPoint_ft.Y(), mRefPoint_ft.Z());
   AdjustAndRedraw(true);
}

void GeometryPointMass::HandleY_CoordinateLineEditChanged(QString aValue)
{
   mRefPoint_ft.Set(mRefPoint_ft.X(), aValue.toDouble(), mRefPoint_ft.Z());
   AdjustAndRedraw(true);
}

void GeometryPointMass::HandleZ_CoordinateLineEditChanged(QString aValue)
{
   mRefPoint_ft.Set(mRefPoint_ft.X(), mRefPoint_ft.Y(), aValue.toDouble());
   AdjustAndRedraw(true);
}

void GeometryPointMass::CalcMassProperties(double) // Although a derived function, it ignores aTotalMass_lbs
{
   // Set the mass
   mCalculatedMass_lbs = mPointMass_lbs;

   double mass_slugs = mCalculatedMass_lbs * mLbsToSlugs;

   // Point mass has no rotational inertia
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

   mCalculatedIxx += mass_slugs * dX2;
   mCalculatedIyy += mass_slugs * dY2;
   mCalculatedIzz += mass_slugs * dZ2;

   // Calculate the location of the cg -- we assume it is always at the reference point
   mCalculatedCG_ft = mRefPoint_ft;
}

bool GeometryPointMass::GetCG_ft(UtVec3dX& aCg_ft)
{
   aCg_ft = mCalculatedCG_ft;
   return true;
}

QStringList GeometryPointMass::Audit()
{
   QStringList errorList = GeometryObject::Audit();

   QString nameString(mName.c_str());

   if (mPointMass_lbs < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a point mass (mPointMass_lbs is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   return errorList;
}

} // namespace Designer
