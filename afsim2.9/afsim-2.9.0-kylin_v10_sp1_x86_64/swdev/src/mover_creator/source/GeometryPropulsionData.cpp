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

#include "GeometryPropulsionData.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QTreeWidget>

#include "MoverCreatorLineEdit.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "P6DofUtils.hpp"
#include "UtMath.hpp"

namespace Designer
{

GeometryPropulsionData::GeometryPropulsionData(Vehicle* aVehicle)
   : GeometryObject(aVehicle)
{
   // Overwrite the GeometryObject type
   mGeometryObjectTypeString.assign("GeometryPropulsionData");
}

GeometryPropulsionData::~GeometryPropulsionData()
{
}

UtVec3dX GeometryPropulsionData::GetRefPoint_ft()
{
   return mFuelTankCG_Location_ft;
}

void GeometryPropulsionData::SetRefPoint(UtVec3dX aRefPoint_ft)
{
   mFuelTankCG_Location_ft = aRefPoint_ft;

   if (mFuelCG_XLineEdit != nullptr)
   {
      mFuelCG_XLineEdit->setText(QString::number(mFuelTankCG_Location_ft.X()));
   }

   if (mFuelCG_YLineEdit != nullptr)
   {
      mFuelCG_YLineEdit->setText(QString::number(mFuelTankCG_Location_ft.Y()));
   }

   if (mFuelCG_ZLineEdit != nullptr)
   {
      mFuelCG_ZLineEdit->setText(QString::number(mFuelTankCG_Location_ft.Z()));
   }
}

void GeometryPropulsionData::MoveRefPoint(UtVec3dX aMoveDelta_ft)
{
   UtVec3dX newRefPoint_ft = mFuelTankCG_Location_ft + aMoveDelta_ft;
   SetRefPoint(newRefPoint_ft);
}

QTreeWidgetItem* GeometryPropulsionData::CreateTreeWidgetItem(QString& aName, QTreeWidget* aTreeWidgetPtr, bool aNoneditableName)
{
   // Enforce the geometry type
   mGeometryObjectTypeString = "GeometryPropulsionData";

   // We do not create the tree widget item at the GeometryObject level,
   // as is typically done -- we create it here instead

   QTreeWidgetItem* treeWidgetItem = new QTreeWidgetItem(aTreeWidgetPtr);

   // Save the pointer to this item
   mTreeWidgetItemPtr = treeWidgetItem;

   // Add a means to tell when the tree is clicked (so that the 3D view can be repainted)
   connect(treeWidgetItem->treeWidget(), &QTreeWidget::itemClicked, this, &GeometryPropulsionData::HandleTreeItemClicked);

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
   auto typeItemLineEdit = new MoverCreatorLineEdit(typeItem);
   typeItemLineEdit->setFrame(false);
   typeItemLineEdit->setText(QString(mGeometryObjectTypeString.c_str()));
   aTreeWidgetPtr->setItemWidget(typeItem, 1, typeItemLineEdit);
   aTreeWidgetPtr->setItemWidget(typeItem, 2, new QLabel(QString::fromStdString("")));
   // We hide this so it won't be shown in GUIs -- we need it to still exist for JSON I/O
   typeItem->setHidden(true);

   QTreeWidgetItem* enableFuelTank = new QTreeWidgetItem(treeWidgetItem, QStringList("Enable Fuel Tank"));
   QCheckBox* enableFuelTankCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(enableFuelTank, 1, enableFuelTankCheckBox);
   aTreeWidgetPtr->setItemWidget(enableFuelTank, 2, new QLabel(QString::fromStdString("")));
   connect(enableFuelTankCheckBox, &QCheckBox::stateChanged, this, &GeometryPropulsionData::HandleEnableFuelTankCheckBoxChanged);
   enableFuelTank->setToolTip(0, QString("If clicked, this allows specification for a single fuel tank.\nIf unchecked, no fuel tank will be included."));

   QTreeWidgetItem* fuelQtyMax = new QTreeWidgetItem(treeWidgetItem, QStringList("Fuel Quantity (Max)"));
   auto fuelQtyMaxLineEdit = new MoverCreatorLineEdit(fuelQtyMax);
   fuelQtyMaxLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   fuelQtyMaxLineEdit->setFrame(false);
   fuelQtyMaxLineEdit->setText(QString::number(mFuelQtyCurrent));
   aTreeWidgetPtr->setItemWidget(fuelQtyMax, 1, fuelQtyMaxLineEdit);
   aTreeWidgetPtr->setItemWidget(fuelQtyMax, 2, new QLabel(QString::fromStdString("lbs")));
   connect(fuelQtyMaxLineEdit, &QLineEdit::textChanged, this, &GeometryPropulsionData::HandleFuelQtyMaxLineEditChanged);
   connect(fuelQtyMaxLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mFuelQtyMaxWidget = fuelQtyMax;
   mFuelQtyMaxWidget->setHidden(true); // Start hidden
   fuelQtyMax->setToolTip(0, QString("This is the maximum capacity of the fuel tank in lbs"));

   QTreeWidgetItem* fuelQtyCurrent = new QTreeWidgetItem(treeWidgetItem, QStringList("Fuel Quantity (Current)"));
   auto fuelQtyCurrentLineEdit = new MoverCreatorLineEdit(fuelQtyCurrent);
   fuelQtyCurrentLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   fuelQtyCurrentLineEdit->setFrame(false);
   fuelQtyCurrentLineEdit->setText(QString::number(mFuelQtyCurrent));
   aTreeWidgetPtr->setItemWidget(fuelQtyCurrent, 1, fuelQtyCurrentLineEdit);
   aTreeWidgetPtr->setItemWidget(fuelQtyCurrent, 2, new QLabel(QString::fromStdString("lbs")));
   connect(fuelQtyCurrentLineEdit, &QLineEdit::textChanged, this, &GeometryPropulsionData::HandleFuelQtyCurrentLineEditChanged);
   connect(fuelQtyCurrentLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mFuelQtyCurrentWidget = fuelQtyCurrent;
   mFuelQtyCurrentWidget->setHidden(true); // Start hidden
   fuelQtyCurrent->setToolTip(0, QString("This is the default/current fuel quantity (it must be less than or equal to the maximum)"));

   // Setup the reference point

   QTreeWidgetItem* refPoint = new QTreeWidgetItem(treeWidgetItem, QStringList("Fuel Tank CG"));
   mFuelTankCG_Widget = refPoint;
   refPoint->setToolTip(0, QString("This is location of the center of mass (center of gravity, cg) of the fuel tank"));

   QTreeWidgetItem* x = new QTreeWidgetItem(refPoint, QStringList("x"));
   auto xLineEdit = new MoverCreatorLineEdit(x);
   xLineEdit->setFrame(false);
   xLineEdit->setText(QString::number(mFuelTankCG_Location_ft.X()));
   aTreeWidgetPtr->setItemWidget(x, 1, xLineEdit);
   aTreeWidgetPtr->setItemWidget(x, 2, new QLabel(QString::fromStdString("ft")));
   connect(xLineEdit, &QLineEdit::textChanged, this, &GeometryPropulsionData::HandleX_CoordinateLineEditChanged);
   connect(xLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mFuelCG_XLineEdit = xLineEdit;

   QTreeWidgetItem* y = new QTreeWidgetItem(refPoint, QStringList("y"));
   auto yLineEdit = new MoverCreatorLineEdit(y);
   mReferencePointY_TreeWidgetItemPtr = y;
   yLineEdit->setFrame(false);
   yLineEdit->setText(QString::number(mFuelTankCG_Location_ft.Y()));
   aTreeWidgetPtr->setItemWidget(y, 1, yLineEdit);
   aTreeWidgetPtr->setItemWidget(y, 2, new QLabel(QString::fromStdString("ft")));
   connect(yLineEdit, &QLineEdit::textChanged, this, &GeometryPropulsionData::HandleY_CoordinateLineEditChanged);
   connect(yLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mFuelCG_YLineEdit = yLineEdit;

   QTreeWidgetItem* z = new QTreeWidgetItem(refPoint, QStringList("z"));
   auto zLineEdit = new MoverCreatorLineEdit(z);
   zLineEdit->setFrame(false);
   zLineEdit->setText(QString::number(mFuelTankCG_Location_ft.Z()));
   aTreeWidgetPtr->setItemWidget(z, 1, zLineEdit);
   aTreeWidgetPtr->setItemWidget(z, 2, new QLabel(QString::fromStdString("ft")));
   connect(zLineEdit, &QLineEdit::textChanged, this, &GeometryPropulsionData::HandleZ_CoordinateLineEditChanged);
   connect(zLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mFuelCG_ZLineEdit = zLineEdit;
   mFuelTankCG_Widget->setHidden(true); // Start hidden


   QTreeWidgetItem* enableTVC = new QTreeWidgetItem(treeWidgetItem, QStringList("Enable Thrust Vectoring"));
   QComboBox* enableTVC_ComboBox = new QComboBox();
   QStringList tvcStrings;
   tvcStrings << "None" << "Pitch Only" << "Yaw Only" << "Pitch and Yaw";
   enableTVC_ComboBox->addItems(tvcStrings);
   aTreeWidgetPtr->setItemWidget(enableTVC, 1, enableTVC_ComboBox);
   aTreeWidgetPtr->setItemWidget(enableTVC, 2, new QLabel(QString::fromStdString("")));
   connect(enableTVC_ComboBox, &QComboBox::currentTextChanged, this, [this, aTreeWidgetPtr, enableTVC](QString aEnableTVC)
   {
      aTreeWidgetPtr->setCurrentItem(enableTVC);
      HandleEnableThrustVectoringComboBoxChanged(aEnableTVC);
   });

   enableTVC->setToolTip(0, QString("This determines if any form of thrust vectoring is present"));

   QTreeWidgetItem* TVC_PitchAngleMin = new QTreeWidgetItem(treeWidgetItem, QStringList("TVC Pitch Angle Min"));
   auto TVC_PitchAngleMinLineEdit = new MoverCreatorLineEdit(TVC_PitchAngleMin);
   TVC_PitchAngleMinLineEdit->setFrame(false);
   TVC_PitchAngleMinLineEdit->setText(QString::number(mTVC_PitchAngleMin));
   aTreeWidgetPtr->setItemWidget(TVC_PitchAngleMin, 1, TVC_PitchAngleMinLineEdit);
   aTreeWidgetPtr->setItemWidget(TVC_PitchAngleMin, 2, new QLabel(QString::fromStdString("deg")));
   connect(TVC_PitchAngleMinLineEdit, &QLineEdit::textChanged, this, &GeometryPropulsionData::HandleTVC_PitchAngleMinLineEditChanged);
   connect(TVC_PitchAngleMinLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mTVC_PitchMinWidget = TVC_PitchAngleMin;
   mTVC_PitchMinWidget->setHidden(true); // Start hidden
   TVC_PitchAngleMin->setToolTip(0, QString("This is the minimum pitch angle that vectoring will support (typically negative)"));

   QTreeWidgetItem* TVC_PitchAngleMax = new QTreeWidgetItem(treeWidgetItem, QStringList("TVC Pitch Angle Max"));
   auto TVC_PitchAngleMaxLineEdit = new MoverCreatorLineEdit(TVC_PitchAngleMax);
   TVC_PitchAngleMaxLineEdit->setFrame(false);
   TVC_PitchAngleMaxLineEdit->setText(QString::number(mTVC_PitchAngleMax));
   aTreeWidgetPtr->setItemWidget(TVC_PitchAngleMax, 1, TVC_PitchAngleMaxLineEdit);
   aTreeWidgetPtr->setItemWidget(TVC_PitchAngleMax, 2, new QLabel(QString::fromStdString("deg")));
   connect(TVC_PitchAngleMaxLineEdit, &QLineEdit::textChanged, this, &GeometryPropulsionData::HandleTVC_PitchAngleMaxLineEditChanged);
   connect(TVC_PitchAngleMaxLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mTVC_PitchMaxWidget = TVC_PitchAngleMax;
   mTVC_PitchMaxWidget->setHidden(true); // Start hidden
   TVC_PitchAngleMax->setToolTip(0, QString("This is the maximum pitch angle that vectoring will support (typically positive)"));

   QTreeWidgetItem* TVC_YawAngleMin = new QTreeWidgetItem(treeWidgetItem, QStringList("TVC Yaw Angle Min"));
   auto TVC_YawAngleMinLineEdit = new MoverCreatorLineEdit(TVC_YawAngleMin);
   TVC_YawAngleMinLineEdit->setFrame(false);
   TVC_YawAngleMinLineEdit->setText(QString::number(mTVC_YawAngleMin));
   aTreeWidgetPtr->setItemWidget(TVC_YawAngleMin, 1, TVC_YawAngleMinLineEdit);
   aTreeWidgetPtr->setItemWidget(TVC_YawAngleMin, 2, new QLabel(QString::fromStdString("deg")));
   connect(TVC_YawAngleMinLineEdit, &QLineEdit::textChanged, this, &GeometryPropulsionData::HandleTVC_YawAngleMinLineEditChanged);
   connect(TVC_YawAngleMinLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mTVC_YawMinWidget = TVC_YawAngleMin;
   mTVC_YawMinWidget->setHidden(true); // Start hidden
   TVC_YawAngleMin->setToolTip(0, QString("This is the minimum yaw angle that vectoring will support (typically negative)"));

   QTreeWidgetItem* TVC_YawAngleMax = new QTreeWidgetItem(treeWidgetItem, QStringList("TVC Yaw Angle Max"));
   auto TVC_YawAngleMaxLineEdit = new MoverCreatorLineEdit(TVC_YawAngleMax);
   TVC_YawAngleMaxLineEdit->setFrame(false);
   TVC_YawAngleMaxLineEdit->setText(QString::number(mTVC_YawAngleMax));
   aTreeWidgetPtr->setItemWidget(TVC_YawAngleMax, 1, TVC_YawAngleMaxLineEdit);
   aTreeWidgetPtr->setItemWidget(TVC_YawAngleMax, 2, new QLabel(QString::fromStdString("deg")));
   connect(TVC_YawAngleMaxLineEdit, &QLineEdit::textChanged, this, &GeometryPropulsionData::HandleTVC_YawAngleMaxLineEditChanged);
   connect(TVC_YawAngleMaxLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mTVC_YawMaxWidget = TVC_YawAngleMax;
   mTVC_YawMaxWidget->setHidden(true); // Start hidden
   TVC_YawAngleMax->setToolTip(0, QString("This is the maximum yaw angle that vectoring will support (typically positive)"));

   QTreeWidgetItem* mayNotBeDeleted = new QTreeWidgetItem(treeWidgetItem, QStringList("May Not Be Deleted"));
   QCheckBox* noDeleteCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(mayNotBeDeleted, 1, noDeleteCheckBox);
   aTreeWidgetPtr->setItemWidget(mayNotBeDeleted, 2, new QLabel(QString::fromStdString("")));
   connect(noDeleteCheckBox, &QCheckBox::stateChanged, this, &GeometryPropulsionData::HandleMayNotBeDeletedCheckBoxChanged);
   // We hide this so it won't be shown in GUIs -- we need it to still exist for JSON I/O
   mayNotBeDeleted->setHidden(true);

   QTreeWidgetItem* massFactor = new QTreeWidgetItem(treeWidgetItem, QStringList("Mass Factor"));
   auto massFactorLineEdit = new MoverCreatorLineEdit(massFactor);
   massFactorLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   massFactorLineEdit->setFrame(false);
   massFactorLineEdit->setText(QString::number(mMassFactor));
   aTreeWidgetPtr->setItemWidget(massFactor, 1, massFactorLineEdit);
   aTreeWidgetPtr->setItemWidget(massFactor, 2, new QLabel(QString::fromStdString("")));
   connect(massFactorLineEdit, &QLineEdit::textChanged, this, &GeometryPropulsionData::HandleMassFactorLineEditChanged);
   connect(massFactorLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   massFactor->setHidden(true);

   return treeWidgetItem;
}

bool GeometryPropulsionData::CalcExtents(double&, double&, double&, double&, double&, double&)
{
   return false;
}

void GeometryPropulsionData::CalcMassProperties(double) // Although a derived function, it ignores aTotalMass_lbs
{
   // Regardless of whether or not a fuel tank is used, we need the empty condition

   // Set the mass - always zero (empty fuel condition)
   mCalculatedMass_lbs = 0.0;

   // For now, fuel is assumed to be a point mass, which has no rotational inertia
   mCalculatedIxx = 0.0;
   mCalculatedIyy = 0.0;
   mCalculatedIzz = 0.0;

   // Calculate the location of the cg -- we assume it is always at the reference point
   mCalculatedCG_ft = mFuelTankCG_Location_ft;
}

bool GeometryPropulsionData::GetCalculatedMassProperties(UtVec3dX& aCgLocation_ft,
                                                         double&   aMass_lbs,
                                                         UtVec3dX& aRotationalInertia)
{
   aCgLocation_ft = mCalculatedCG_ft;
   aMass_lbs = mCalculatedMass_lbs;
   aRotationalInertia.Set(mCalculatedIxx, mCalculatedIyy, mCalculatedIzz);

   return false; // Propulsion lacks any mass properties
}

bool GeometryPropulsionData::GetCG_ft(UtVec3dX& aCg_ft)
{
   aCg_ft = mCalculatedCG_ft;
   return true;
}

bool GeometryPropulsionData::GetEnableThrustVectoringString(QString& aString)
{
   aString = mEnableThrustVectoringString;
   return true;
}

bool GeometryPropulsionData::GetMinimumThrustVectoringPitchAngle_deg(double& aMinimumPitchAngle_deg)
{
   aMinimumPitchAngle_deg = mTVC_PitchAngleMin;
   return true;
}

bool GeometryPropulsionData::GetMaximumThrustVectoringPitchAngle_deg(double& aMaximumPitchAngle_deg)
{
   aMaximumPitchAngle_deg = mTVC_PitchAngleMax;
   return true;
}

bool GeometryPropulsionData::GetMinimumThrustVectoringYawAngle_deg(double& aMinimumYawAngle_deg)
{
   aMinimumYawAngle_deg = mTVC_YawAngleMin;
   return true;
}

bool GeometryPropulsionData::GetMaximumThrustVectoringYawAngle_deg(double& aMaximumYawAngle_deg)
{
   aMaximumYawAngle_deg = mTVC_YawAngleMax;
   return true;
}

void GeometryPropulsionData::HandleX_CoordinateLineEditChanged(QString aValue)
{
   mFuelTankCG_Location_ft.Set(aValue.toDouble(), mFuelTankCG_Location_ft.Y(), mFuelTankCG_Location_ft.Z());
   AdjustAndRedraw(true);
}

void GeometryPropulsionData::HandleY_CoordinateLineEditChanged(QString aValue)
{
   mFuelTankCG_Location_ft.Set(mFuelTankCG_Location_ft.X(), aValue.toDouble(), mFuelTankCG_Location_ft.Z());
   AdjustAndRedraw(true);
}

void GeometryPropulsionData::HandleZ_CoordinateLineEditChanged(QString aValue)
{
   mFuelTankCG_Location_ft.Set(mFuelTankCG_Location_ft.X(), mFuelTankCG_Location_ft.Y(), aValue.toDouble());
   AdjustAndRedraw(true);
}

void GeometryPropulsionData::HandleMayNotBeDeletedCheckBoxChanged(int aValue)
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

   AdjustAndRedraw(false);
}

void GeometryPropulsionData::HandleTreeItemClicked(QTreeWidgetItem* item, int column)
{
   AdjustAndRedraw(false);
}

void GeometryPropulsionData::HandleEnableFuelTankCheckBoxChanged(int aValue)
{
   if (aValue == 0)
   {
      mEnableFuelTank = false;
      if (mFuelQtyMaxWidget     != nullptr) { mFuelQtyMaxWidget->setHidden(true); }
      if (mFuelQtyCurrentWidget != nullptr) { mFuelQtyCurrentWidget->setHidden(true); }
      if (mFuelTankCG_Widget    != nullptr) { mFuelTankCG_Widget->setHidden(true); }
   }
   else
   {
      mEnableFuelTank = true;
      if (mFuelQtyMaxWidget     != nullptr) { mFuelQtyMaxWidget->setHidden(false); }
      if (mFuelQtyCurrentWidget != nullptr) { mFuelQtyCurrentWidget->setHidden(false); }
      if (mFuelTankCG_Widget    != nullptr) { mFuelTankCG_Widget->setHidden(false); }
   }

   AdjustAndRedraw(true);
}

void GeometryPropulsionData::HandleFuelQtyMaxLineEditChanged(QString aValue)
{
   mFuelQtyMax = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryPropulsionData::HandleFuelQtyCurrentLineEditChanged(QString aValue)
{
   mFuelQtyCurrent = aValue.toDouble();
}

void GeometryPropulsionData::HandleEnableThrustVectoringComboBoxChanged(QString aValue)
{
   mEnableThrustVectoringString = aValue;

   if (mEnableThrustVectoringString == "Pitch Only")
   {
      if (mTVC_PitchMinWidget != nullptr) { mTVC_PitchMinWidget->setHidden(false); }
      if (mTVC_PitchMaxWidget != nullptr) { mTVC_PitchMaxWidget->setHidden(false); }
      if (mTVC_YawMinWidget != nullptr) { mTVC_YawMinWidget->setHidden(true); }
      if (mTVC_YawMaxWidget != nullptr) { mTVC_YawMaxWidget->setHidden(true); }
   }
   else if (mEnableThrustVectoringString == "Yaw Only")
   {
      if (mTVC_PitchMinWidget != nullptr) { mTVC_PitchMinWidget->setHidden(true); }
      if (mTVC_PitchMaxWidget != nullptr) { mTVC_PitchMaxWidget->setHidden(true); }
      if (mTVC_YawMinWidget != nullptr) { mTVC_YawMinWidget->setHidden(false); }
      if (mTVC_YawMaxWidget != nullptr) { mTVC_YawMaxWidget->setHidden(false); }
   }
   else if (mEnableThrustVectoringString == "Pitch and Yaw")
   {
      if (mTVC_PitchMinWidget != nullptr) { mTVC_PitchMinWidget->setHidden(false); }
      if (mTVC_PitchMaxWidget != nullptr) { mTVC_PitchMaxWidget->setHidden(false); }
      if (mTVC_YawMinWidget != nullptr) { mTVC_YawMinWidget->setHidden(false); }
      if (mTVC_YawMaxWidget != nullptr) { mTVC_YawMaxWidget->setHidden(false); }
   }
   else
   {
      if (mTVC_PitchMinWidget != nullptr) { mTVC_PitchMinWidget->setHidden(true); }
      if (mTVC_PitchMaxWidget != nullptr) { mTVC_PitchMaxWidget->setHidden(true); }
      if (mTVC_YawMinWidget != nullptr) { mTVC_YawMinWidget->setHidden(true); }
      if (mTVC_YawMaxWidget != nullptr) { mTVC_YawMaxWidget->setHidden(true); }
   }

   AdjustAndRedraw(false);
}

void GeometryPropulsionData::HandleTVC_PitchAngleMinLineEditChanged(QString aValue)
{
   mTVC_PitchAngleMin = aValue.toDouble();
   AdjustAndRedraw(false);
}

void GeometryPropulsionData::HandleTVC_PitchAngleMaxLineEditChanged(QString aValue)
{
   mTVC_PitchAngleMax = aValue.toDouble();
   AdjustAndRedraw(false);
}

void GeometryPropulsionData::HandleTVC_YawAngleMinLineEditChanged(QString aValue)
{
   mTVC_YawAngleMin = aValue.toDouble();
   AdjustAndRedraw(false);
}

void GeometryPropulsionData::HandleTVC_YawAngleMaxLineEditChanged(QString aValue)
{
   mTVC_YawAngleMax = aValue.toDouble();
   AdjustAndRedraw(false);
}

QStringList GeometryPropulsionData::Audit()
{
   QStringList errorList = GeometryObject::Audit();

   QString nameString(mName.c_str());

   if (mEnableFuelTank)
   {
      if (mFuelQtyMax < std::numeric_limits<double>::epsilon())
      {
         QString errorString(" requires a maximum fuel quantity (mFuelQtyMax is zero)");
         errorString = nameString + errorString;
         errorList.push_back(errorString);
      }
   }

   return errorList;
}

} // namespace Designer
