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

#include "GeometryObject.hpp"

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QString>
#include <QTreeWidgetItem>

#include "MoverCreatorLineEdit.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "GeometryGLWidget.hpp"
#include "GeometryMassProperties.hpp"
#include "MoverCreatorWidget.hpp"
#include "P6DofUtils.hpp"
#include "UtMath.hpp"
#include "Vehicle.hpp"
#include "ui_GeometryWidget.h"

namespace Designer
{

GeometryObject::GeometryObject(Vehicle* aVehicle)
   : mName("UNDEFINED")
   , mGeometryObjectTypeString("UNDEFINED")
   , mRefPoint_ft(0.0, 0.0, 0.0)
   , mVehiclePtr(aVehicle)
{
}

GeometryObject::~GeometryObject()
{
}

std::string GeometryObject::GetName()
{
   return mName;
}

void GeometryObject::SetName(QString& aName)
{
   mName = aName.toStdString();
}

UtVec3dX GeometryObject::GetRefPoint_ft()
{
   return mRefPoint_ft;
}

void GeometryObject::SetRefPoint(UtVec3dX aRefPoint_ft)
{
   mRefPoint_ft = aRefPoint_ft;

   if (mReferencePointX_LineEditPtr != nullptr)
   {
      mReferencePointX_LineEditPtr->setText(QString::number(mRefPoint_ft.X()));
   }

   if (mReferencePointY_LineEditPtr != nullptr)
   {
      mReferencePointY_LineEditPtr->setText(QString::number(mRefPoint_ft.Y()));
   }

   if (mReferencePointZ_LineEditPtr != nullptr)
   {
      mReferencePointZ_LineEditPtr->setText(QString::number(mRefPoint_ft.Z()));
   }
}

void GeometryObject::MoveRefPoint(UtVec3dX aMoveDelta_ft)
{
   UtVec3dX newRefPoint_ft = mRefPoint_ft + aMoveDelta_ft;
   SetRefPoint(newRefPoint_ft);
}

QTreeWidgetItem* GeometryObject::CreateTreeWidgetItem(QString& aName, QTreeWidget* aTreeWidgetPtr, bool aNoneditableName)
{
   QTreeWidgetItem* treeWidgetItem = new QTreeWidgetItem(aTreeWidgetPtr);

   // Save the pointer to this item
   mTreeWidgetItemPtr = treeWidgetItem;

   // Add a means to tell when the tree is clicked (so that the 3D view can be repainted)
   connect(treeWidgetItem->treeWidget(), &QTreeWidget::itemClicked, this, &GeometryObject::HandleTreeItemClicked);

   // Set the name
   treeWidgetItem->setText(0, aName);
   mName = aName.toStdString();

   // Set Qt flags
   if (aNoneditableName)
   {
      treeWidgetItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
   }
   else
   {
      treeWidgetItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
   }

   QTreeWidgetItem* typeItem = new QTreeWidgetItem(treeWidgetItem, QStringList("GeometryObjectType"));
   QLineEdit* typeItemLineEdit = new QLineEdit();
   typeItemLineEdit->setFrame(false);
   typeItemLineEdit->setText(QString(mGeometryObjectTypeString.c_str()));
   aTreeWidgetPtr->setItemWidget(typeItem, 1, typeItemLineEdit);
   aTreeWidgetPtr->setItemWidget(typeItem, 2, new QLabel(QString::fromStdString("")));
   // We hide this so it won't be shown in GUIs -- we need it to still exist for JSON I/O
   typeItem->setHidden(true);

   // Setup the reference point - common to all geometry objects

   QTreeWidgetItem* refPoint = new QTreeWidgetItem(treeWidgetItem, QStringList("Reference Point"));
   refPoint->setToolTip(0, "The reference point of the object in the XYZ plane.");
   mReferencePointWidget = refPoint;

   QTreeWidgetItem* x = new QTreeWidgetItem(refPoint, QStringList("x"));
   auto xLineEdit = new MoverCreatorLineEdit(x);
   mReferencePointX_LineEditPtr = xLineEdit;
   xLineEdit->setFrame(false);
   xLineEdit->setText(QString::number(mRefPoint_ft.X()));
   aTreeWidgetPtr->setItemWidget(x, 1, xLineEdit);
   aTreeWidgetPtr->setItemWidget(x, 2, new QLabel(QString::fromStdString("ft")));
   connect(xLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* y = new QTreeWidgetItem(refPoint, QStringList("y"));
   auto yLineEdit = new MoverCreatorLineEdit(y);
   mReferencePointY_LineEditPtr = yLineEdit;
   mReferencePointY_TreeWidgetItemPtr = y;
   yLineEdit->setFrame(false);
   yLineEdit->setText(QString::number(mRefPoint_ft.Y()));
   aTreeWidgetPtr->setItemWidget(y, 1, yLineEdit);
   aTreeWidgetPtr->setItemWidget(y, 2, new QLabel(QString::fromStdString("ft")));
   connect(yLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* z = new QTreeWidgetItem(refPoint, QStringList("z"));
   auto zLineEdit = new MoverCreatorLineEdit(z);
   mReferencePointZ_LineEditPtr = zLineEdit;
   zLineEdit->setFrame(false);
   zLineEdit->setText(QString::number(mRefPoint_ft.Z()));
   aTreeWidgetPtr->setItemWidget(z, 1, zLineEdit);
   aTreeWidgetPtr->setItemWidget(z, 2, new QLabel(QString::fromStdString("ft")));
   connect(zLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* mayNotBeDeleted = new QTreeWidgetItem(treeWidgetItem, QStringList("May Not Be Deleted"));
   QCheckBox* noDeleteCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(mayNotBeDeleted, 1, noDeleteCheckBox);
   aTreeWidgetPtr->setItemWidget(mayNotBeDeleted, 2, new QLabel(QString::fromStdString("")));
   connect(noDeleteCheckBox, &QCheckBox::stateChanged, this, &GeometryObject::HandleMayNotBeDeletedCheckBoxChanged);
   // We hide this so it won't be shown in GUIs -- we need it to still exist for JSON I/O
   mayNotBeDeleted->setHidden(true);

   if (mUseDefaultConnectLineEdits)
   {
      ConnectLineEdits();
   }

   return treeWidgetItem;
}

void GeometryObject::ConnectLineEdits()
{
   connect(mReferencePointX_LineEditPtr, &QLineEdit::textChanged, this, &GeometryObject::HandleX_CoordinateLineEditChanged);
   connect(mReferencePointY_LineEditPtr, &QLineEdit::textChanged, this, &GeometryObject::HandleY_CoordinateLineEditChanged);
   connect(mReferencePointZ_LineEditPtr, &QLineEdit::textChanged, this, &GeometryObject::HandleZ_CoordinateLineEditChanged);
}

void GeometryObject::HandleX_CoordinateLineEditChanged(QString aValue)
{
   mRefPoint_ft.Set(aValue.toDouble(), mRefPoint_ft.Y(), mRefPoint_ft.Z());
   AdjustAndRedraw(true);
}

void GeometryObject::HandleY_CoordinateLineEditChanged(QString aValue)
{
   mRefPoint_ft.Set(mRefPoint_ft.X(), aValue.toDouble(), mRefPoint_ft.Z());
   AdjustAndRedraw(true);
}

void GeometryObject::HandleZ_CoordinateLineEditChanged(QString aValue)
{
   mRefPoint_ft.Set(mRefPoint_ft.X(), mRefPoint_ft.Y(), aValue.toDouble());
   AdjustAndRedraw(true);
}

void GeometryObject::HandleMayNotBeDeletedCheckBoxChanged(int aValue)
{
   if (aValue == 0)
   {
      mMayNotBeDeleted = false;
   }
   else
   {
      mMayNotBeDeleted = true;
   }

   if (MayNotBeDeleted())
   {
      if (mDeleteButton != nullptr)
      {
         HideDeleteButton();
      }
   }
   else
   {
      if (mDeleteButton != nullptr)
      {
         QIcon icon;
         QString iconThemeName = QStringLiteral("delete_2");
         icon = QIcon::fromTheme(iconThemeName);
         mDeleteButton->setIcon(icon);
         mDeleteButton->setEnabled(true);
      }
   }

   AdjustAndRedraw();
}

void GeometryObject::HandleTreeItemClicked(QTreeWidgetItem* item, int column)
{
   AdjustAndRedraw();
}

void GeometryObject::AdjustViewingDistance()
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Designer::GeometryWidget* geometry = mw.GetMoverCreatorWidget()->GetGeometryWidget();
   if (geometry != nullptr)
   {
      Designer::GeometryGLWidget* glObj = geometry->GetGeometryGLWidget();
      glObj->AdjustViewingDistance();
   }
}

void GeometryObject::Update()
{
   mVehiclePtr->Update();
}

bool GeometryObject::CalcExtents(double& xMin, double& xMax, double& yMin, double& yMax, double& zMin, double& zMax)
{
   // X-Direction
   xMin = mRefPoint_ft.X();
   xMax = mRefPoint_ft.X();

   // Y-Direction
   yMin = mRefPoint_ft.Y();
   yMax = mRefPoint_ft.Y();

   // Z-Direction
   zMin = mRefPoint_ft.Z();
   zMax = mRefPoint_ft.Z();

   return true;
}

Ui::GeometryWidget* GeometryObject::GetUiGeometryWidget()
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Designer::GeometryWidget* geometryWidget = mw.GetMoverCreatorWidget()->GetGeometryWidget();
   if (geometryWidget != nullptr)
   {
      Ui::GeometryWidget* uiPtr = geometryWidget->GetUiGeometryWidget();
      return uiPtr;
   }
   return nullptr;
}

Designer::GeometryWidget* GeometryObject::GetGeometryWidget()
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Designer::GeometryWidget* geometryWidget = mw.GetMoverCreatorWidget()->GetGeometryWidget();
   return geometryWidget;
}

bool GeometryObject::IsAircraft()
{
   // Check to see if we have an aircraft or a weapon
   return mVehiclePtr->IsAircraft();
}

bool GeometryObject::IsWeapon()
{
   // Check to see if we have an aircraft or a weapon
   return mVehiclePtr->IsWeapon();
}

void GeometryObject::AdjustAndRedraw(bool aRecalcMassAndInertia)
{
   Ui::GeometryWidget* uiPtr = GetUiGeometryWidget();
   if (uiPtr != nullptr)
   {
      if (IsAircraft())
      {
         uiPtr->mSaveButton->setText(QString("Save Aircraft"));
      }
      else if (IsWeapon())
      {
         uiPtr->mSaveButton->setText(QString("Save Weapon"));
      }
   }

   if (aRecalcMassAndInertia)
   {
      RecalcMassAndAero();
   }

   QTreeWidgetItem* curItem = GetGeometryWidget()->GetSelectedTreeWidgetItem();
   if (!curItem)
   {
      return;
   }

   if (curItem == mTreeWidgetItemPtr || (mTreeWidgetItemPtr->indexOfChild(curItem) > -1))
   {
      AdjustViewingDistance();
      Update();
   }
   else if (mReferencePointWidget && mReferencePointWidget->indexOfChild(curItem) > -1)
   {
      AdjustViewingDistance();
      Update();
   }
}

void GeometryObject::RecalcMassAndAero()
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   Designer::GeometryWidget* geometry = mw.GetMoverCreatorWidget()->GetGeometryWidget();
   if (geometry == nullptr) { return; }

   if (!geometry->LoadingInProgress())
   {
      mVehiclePtr->CalcMassAndInertia();
      mVehiclePtr->GenerateAeroDataFromGeometry();

      // Get the vehicle data

      double vehicleAlpha_deg = 0.0;
      double vehicleBeta_deg = 0.0;
      mVehiclePtr->GetVehicleAlphaBeta(vehicleAlpha_deg, vehicleBeta_deg);

      double vehicleYawRate_dps = 0.0;
      double vehiclePitchRate_dps = 0.0;
      double vehicleRollRate_dps = 0.0;
      mVehiclePtr->GetVehicleYawPitchRollRates(vehicleYawRate_dps, vehiclePitchRate_dps, vehicleRollRate_dps);

      double speed_fps = 0.0;
      double mach = 0.0;
      double KTAS = 0.0;
      double KCAS = 0.0;
      double dynamicPressure_psf = 0.0;
      double airDensity_slugs_ft3 = 0.0;
      mVehiclePtr->GetVehicleSpeedParameters(speed_fps, mach, KTAS, KCAS, dynamicPressure_psf, airDensity_slugs_ft3);

      // Update the vectors
      mVehiclePtr->CalculateAeroAttitudeVectors();
   }
}

void GeometryObject::HideDeleteButton()
{
   // Find the QTreeWidgetItem in the tree that contains this button and hide it.
   auto treeWidget = qobject_cast<QTreeWidget*>(mDeleteButton->parentWidget()->parentWidget()->parentWidget());
   if (treeWidget)
   {
      for (int i = 0; i < treeWidget->topLevelItemCount(); ++i)
      {
         QTreeWidgetItem* curItem = treeWidget->topLevelItem(i);
         auto containingWidget = qobject_cast<QWidget*>(treeWidget->itemWidget(curItem, 2));
         if (containingWidget && (containingWidget->isAncestorOf(mDeleteButton)))
         {
            // Prevent the delete button from being clicked.
            mDeleteButton->setDisabled(true);

            // Make the delete button blend into the background.
            mDeleteButton->setFlat(true);

            // Change to the disabled icon.
            QIcon disabled = QIcon::fromTheme("delete_disabled");
            mDeleteButton->setIcon(disabled);

            // Remove the tooltip.
            mDeleteButton->setToolTip("");
         }
      }
   }
}

void GeometryObject::HandleMassFactorLineEditChanged(QString aValue)
{
   mMassFactor = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryObject::CalcMassProperties(double aTotalMass_lbs)
{
   mCalculatedMass_lbs = aTotalMass_lbs;
   mCalculatedCG_ft = mRefPoint_ft;
   mCalculatedIxx = 0.0;
   mCalculatedIyy = 0.0;
   mCalculatedIzz = 0.0;
}

bool GeometryObject::GetCalculatedMassProperties(UtVec3dX& aCgLocation_ft,
                                                 double&   aMass_lbs,
                                                 UtVec3dX& aRotationalInertia)
{
   aCgLocation_ft = mCalculatedCG_ft;
   aMass_lbs = mCalculatedMass_lbs;
   aRotationalInertia.Set(mCalculatedIxx, mCalculatedIyy, mCalculatedIzz);

   return true;
}

double GeometryObject::GetCalculatedMass_lbs() const
{
   return mCalculatedMass_lbs;
}


bool GeometryObject::GetCG_ft(UtVec3dX& aCg_ft)
{
   aCg_ft = mCalculatedCG_ft;
   return false;
}

bool GeometryObject::DebugMode()
{
   return mVehiclePtr->DebugMode();
}

QStringList GeometryObject::Audit()
{
   QStringList errorList;

   // Check various items

   if (mName.empty())
   {
      QString errorString("Object requires a name (mName is empty)");
      errorList.push_back(errorString);
   }

   QString nameString(mName.c_str());

   if (mGeometryObjectTypeString.empty())
   {
      QString errorString(" requires a geometry type (mGeometryObjectTypeString is empty)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   // Note: mCalculatedMass_lbs, mCalculatedIxx, mCalculatedIyy, and mCalculatedIzz should
   // be handled by derived classes

   return errorList;
}

} // namespace Designer
