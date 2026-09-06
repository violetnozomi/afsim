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

#include "GeometryBody.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QTreeWidget>

#include "MoverCreatorLineEdit.hpp"
#include "P6DofUtils.hpp"
#include "UtMath.hpp"

namespace Designer
{

GeometryBody::GeometryBody(Vehicle* aVehicle)
   : GeometryObject(aVehicle)
{
   // Overwrite the GeometryBody type
   mGeometryObjectTypeString.assign("GeometryBody");
}

GeometryBody::~GeometryBody()
{
}

void GeometryBody::MoveRefPoint(UtVec3dX aMoveDelta_ft)
{
   mSymmetryHorizontalY += aMoveDelta_ft.Y();

   UtVec3dX newRefPoint_ft = mRefPoint_ft + aMoveDelta_ft;
   SetRefPoint(newRefPoint_ft);
}

GeometryBody::ForwardShapeType GeometryBody::GetForwardShapeType()
{
   if (mForwardShapeString == "Round")
   {
      return GeometryBody::ForwardShapeType::cROUND;
   }
   else if (mForwardShapeString == "Ogive")
   {
      return GeometryBody::ForwardShapeType::cOGIVE;
   }
   else if (mForwardShapeString == "Cone")
   {
      return GeometryBody::ForwardShapeType::cCONE;
   }
   else if (mForwardShapeString == "Blunt")
   {
      return GeometryBody::ForwardShapeType::cBLUNT;
   }
   return GeometryBody::ForwardShapeType::cBLUNT;
}

GeometryBody::AftShapeType Designer::GeometryBody::GetAftShapeType()
{
   if (mAftShapeString == "Round")
   {
      return GeometryBody::AftShapeType::cROUND;
   }
   else if (mAftShapeString == "Ogive")
   {
      return GeometryBody::AftShapeType::cOGIVE;
   }
   else if (mAftShapeString == "Cone")
   {
      return GeometryBody::AftShapeType::cCONE;
   }
   else if (mAftShapeString == "BoatTail")
   {
      return GeometryBody::AftShapeType::cBOATTAIL;
   }
   else if (mAftShapeString == "Blunt")
   {
      return GeometryBody::AftShapeType::cBLUNT;
   }
   return GeometryBody::AftShapeType::cBLUNT;
}

QTreeWidgetItem* GeometryBody::CreateTreeWidgetItem(QString& aName, QTreeWidget* aTreeWidgetPtr, bool aNoneditableName)
{
   // Create the tree widget item at the GeometryObject level
   QTreeWidgetItem* treeWidgetItem =
      GeometryObject::CreateTreeWidgetItem(aName, aTreeWidgetPtr, aNoneditableName);

   // Enforce the geometry type
   mGeometryObjectTypeString = "GeometryBody";

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

   QTreeWidgetItem* lengthItem = new QTreeWidgetItem(treeWidgetItem, QStringList("Length"));
   lengthItem->setToolTip(0, "The length of the body in feet");
   auto lengthLineEdit = new MoverCreatorLineEdit(lengthItem);
   lengthLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   lengthLineEdit->setFrame(false);
   lengthLineEdit->setText(QString::number(mLength_ft));
   aTreeWidgetPtr->setItemWidget(lengthItem, 1, lengthLineEdit);
   aTreeWidgetPtr->setItemWidget(lengthItem, 2, new QLabel(QString::fromStdString("ft")));
   connect(lengthLineEdit, &QLineEdit::textChanged, this, &GeometryBody::HandleLengthLineEditChanged);
   connect(lengthLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* heightItem = new QTreeWidgetItem(treeWidgetItem, QStringList("Height"));
   heightItem->setToolTip(0, "The height of the body in feet");
   auto heightLineEdit = new MoverCreatorLineEdit(heightItem);
   heightLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   heightLineEdit->setFrame(false);
   heightLineEdit->setText(QString::number(mHeight_ft));
   aTreeWidgetPtr->setItemWidget(heightItem, 1, heightLineEdit);
   aTreeWidgetPtr->setItemWidget(heightItem, 2, new QLabel(QString::fromStdString("ft")));
   connect(heightLineEdit, &QLineEdit::textChanged, this, &GeometryBody::HandleHeightLineEditChanged);
   connect(heightLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mHeightItem = heightItem;

   QTreeWidgetItem* widthItem = new QTreeWidgetItem(treeWidgetItem, QStringList("Width"));
   widthItem->setToolTip(0, "The width of the body in feet");
   auto widthLineEdit = new MoverCreatorLineEdit(widthItem);
   widthLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   widthLineEdit->setFrame(false);
   widthLineEdit->setText(QString::number(mWidth_ft));
   aTreeWidgetPtr->setItemWidget(widthItem, 1, widthLineEdit);
   aTreeWidgetPtr->setItemWidget(widthItem, 2, new QLabel(QString::fromStdString("ft")));
   connect(widthLineEdit, &QLineEdit::textChanged, this, &GeometryBody::HandleWidthLineEditChanged);
   connect(widthLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* yaw = new QTreeWidgetItem(treeWidgetItem, QStringList("Yaw Angle"));
   yaw->setToolTip(0, "The yaw of the body in degrees");
   auto yawLineEdit = new MoverCreatorLineEdit(yaw);
   yawLineEdit->setFrame(false);
   yawLineEdit->setText(QString::number(mYaw_rad * UtMath::cDEG_PER_RAD));
   aTreeWidgetPtr->setItemWidget(yaw, 1, yawLineEdit);
   aTreeWidgetPtr->setItemWidget(yaw, 2, new QLabel(QString::fromStdString("deg")));
   connect(yawLineEdit, &QLineEdit::textChanged, this, &GeometryBody::HandleYawLineEditChanged);
   connect(yawLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* pitch = new QTreeWidgetItem(treeWidgetItem, QStringList("Pitch Angle"));
   pitch->setToolTip(0, "The pitch of the body in degrees");
   auto pitchLineEdit = new MoverCreatorLineEdit(pitch);
   pitchLineEdit->setFrame(false);
   pitchLineEdit->setText(QString::number(mPitch_rad * UtMath::cDEG_PER_RAD));
   aTreeWidgetPtr->setItemWidget(pitch, 1, pitchLineEdit);
   aTreeWidgetPtr->setItemWidget(pitch, 2, new QLabel(QString::fromStdString("deg")));
   connect(pitchLineEdit, &QLineEdit::textChanged, this, &GeometryBody::HandlePitchLineEditChanged);
   connect(pitchLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* roll = new QTreeWidgetItem(treeWidgetItem, QStringList("Roll Angle"));
   roll->setToolTip(0, "The roll of the body in degrees");
   auto rollLineEdit = new MoverCreatorLineEdit(roll);
   rollLineEdit->setFrame(false);
   rollLineEdit->setText(QString::number(mRoll_rad * UtMath::cDEG_PER_RAD));
   aTreeWidgetPtr->setItemWidget(roll, 1, rollLineEdit);
   aTreeWidgetPtr->setItemWidget(roll, 2, new QLabel(QString::fromStdString("deg")));
   connect(rollLineEdit, &QLineEdit::textChanged, this, &GeometryBody::HandleRollLineEditChanged);
   connect(rollLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* forwardShape = new QTreeWidgetItem(treeWidgetItem, QStringList("Forward Shape"));
   forwardShape->setToolTip(0, "The forward shape of the body, which can be ogive, conical, rounded or blunt");
   QComboBox* forwardShapeComboBox = new QComboBox();
   QStringList forwardStrings;
   forwardStrings << "Ogive" << "Cone" << "Round" << "Blunt";
   forwardShapeComboBox->addItems(forwardStrings);
   aTreeWidgetPtr->setItemWidget(forwardShape, 1, forwardShapeComboBox);
   aTreeWidgetPtr->setItemWidget(forwardShape, 2, new QLabel(QString::fromStdString("")));
   mForwardShapeItem = forwardShape;
   connect(forwardShapeComboBox, &QComboBox::currentTextChanged, this, [this, aTreeWidgetPtr, forwardShape](QString aForwardShape)
   {
      aTreeWidgetPtr->setCurrentItem(forwardShape);
      HandleForwardShapeComboBoxChanged(aForwardShape);
   });

   QTreeWidgetItem* forwardShapeLength = new QTreeWidgetItem(treeWidgetItem, QStringList("Forward Shape Length"));
   forwardShapeLength->setToolTip(0, "The length of the forward shape in feet");
   auto forwardShapeLengthLineEdit = new MoverCreatorLineEdit(forwardShapeLength);
   forwardShapeLengthLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   forwardShapeLengthLineEdit->setFrame(false);
   forwardShapeLengthLineEdit->setText(QString::number(mRoll_rad * UtMath::cDEG_PER_RAD));
   aTreeWidgetPtr->setItemWidget(forwardShapeLength, 1, forwardShapeLengthLineEdit);
   aTreeWidgetPtr->setItemWidget(forwardShapeLength, 2, new QLabel(QString::fromStdString("ft")));
   connect(forwardShapeLengthLineEdit, &QLineEdit::textChanged, this, &GeometryBody::HandleForwardShapeLengthLineEditChanged);
   connect(forwardShapeLengthLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mForwardShapeLengthItem = forwardShapeLength;

   QTreeWidgetItem* aftShape = new QTreeWidgetItem(treeWidgetItem, QStringList("Aft Shape"));
   aftShape->setToolTip(0, "The aft shape of the body, which can be ogive, conical, rounded, boat tailed or blunt");
   QComboBox* aftShapeComboBox = new QComboBox();
   QStringList aftStrings;
   aftStrings << "Ogive" << "Cone" << "Round" << "BoatTail" << "Blunt";
   aftShapeComboBox->addItems(aftStrings);
   aTreeWidgetPtr->setItemWidget(aftShape, 1, aftShapeComboBox);
   aTreeWidgetPtr->setItemWidget(aftShape, 2, new QLabel(QString::fromStdString("")));
   mAftShapeItem = aftShape;
   connect(aftShapeComboBox, &QComboBox::currentTextChanged, this, [this, aTreeWidgetPtr, aftShape](QString aAftShape)
   {
      aTreeWidgetPtr->setCurrentItem(aftShape);
      HandleAftShapeComboBoxChanged(aAftShape);
   });

   QTreeWidgetItem* aftShapeLength = new QTreeWidgetItem(treeWidgetItem, QStringList("Aft Shape Length"));
   aftShapeLength->setToolTip(0, "The length of the aft shape in feet");
   auto aftShapeLengthLineEdit = new MoverCreatorLineEdit(aftShapeLength);
   aftShapeLengthLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   aftShapeLengthLineEdit->setFrame(false);
   aftShapeLengthLineEdit->setText(QString::number(mRoll_rad * UtMath::cDEG_PER_RAD));
   aTreeWidgetPtr->setItemWidget(aftShapeLength, 1, aftShapeLengthLineEdit);
   aTreeWidgetPtr->setItemWidget(aftShapeLength, 2, new QLabel(QString::fromStdString("ft")));
   connect(aftShapeLengthLineEdit, &QLineEdit::textChanged, this, &GeometryBody::HandleAftShapeLengthLineEditChanged);
   connect(aftShapeLengthLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mAftShapeLengthItem = aftShapeLength;

   QTreeWidgetItem* aftShapeDiameter = new QTreeWidgetItem(treeWidgetItem, QStringList("Aft Shape Diameter"));
   aftShapeDiameter->setToolTip(0, "The diameter of the aft shape in feet");
   auto aftShapeDiameterLineEdit = new MoverCreatorLineEdit(aftShapeDiameter);
   aftShapeDiameterLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   aftShapeDiameterLineEdit->setFrame(false);
   aftShapeDiameterLineEdit->setText(QString::number(mRoll_rad * UtMath::cDEG_PER_RAD));
   aTreeWidgetPtr->setItemWidget(aftShapeDiameter, 1, aftShapeDiameterLineEdit);
   aTreeWidgetPtr->setItemWidget(aftShapeDiameter, 2, new QLabel(QString::fromStdString("ft")));
   connect(aftShapeDiameterLineEdit, &QLineEdit::textChanged, this, &GeometryBody::HandleAftShapeDiameterLineEditChanged);
   connect(aftShapeDiameterLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mAftShapeDiameterItem = aftShapeDiameter;
   mAftShapeDiameterItem->setHidden(true); // This starts hidden

   QTreeWidgetItem* massFactor = new QTreeWidgetItem(treeWidgetItem, QStringList("Mass Factor"));
   massFactor->setToolTip(0, "The mass factor multiplier of the body");
   auto massFactorLineEdit = new MoverCreatorLineEdit(massFactor);
   massFactorLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   massFactorLineEdit->setFrame(false);
   massFactorLineEdit->setText(QString::number(mMassFactor));
   aTreeWidgetPtr->setItemWidget(massFactor, 1, massFactorLineEdit);
   aTreeWidgetPtr->setItemWidget(massFactor, 2, new QLabel(QString::fromStdString("")));
   connect(massFactorLineEdit, &QLineEdit::textChanged, this, &GeometryBody::HandleMassFactorLineEditChanged);
   connect(massFactorLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* isMainBody = new QTreeWidgetItem(treeWidgetItem, QStringList("Main Body"));
   isMainBody->setToolTip(0, "If checked, this body will be treated as the main body");
   QCheckBox* isMainBodyCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(isMainBody, 1, isMainBodyCheckBox);
   aTreeWidgetPtr->setItemWidget(isMainBody, 2, new QLabel(QString::fromStdString("")));
   connect(isMainBodyCheckBox, &QCheckBox::stateChanged, this, &GeometryBody::HandleIsMainBodyCheckBoxChanged);
   isMainBody->setHidden(true); // This is always hidden

   QTreeWidgetItem* symmetrical = new QTreeWidgetItem(treeWidgetItem, QStringList("Symmetrical"));
   symmetrical->setToolTip(0, "If checked, this body will utilize symmetry");
   QCheckBox* symmetricalCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(symmetrical, 1, symmetricalCheckBox);
   aTreeWidgetPtr->setItemWidget(symmetrical, 2, new QLabel(QString::fromStdString("")));
   connect(symmetricalCheckBox, &QCheckBox::stateChanged, this, &GeometryBody::HandleSymmetricalCheckBoxChanged);

   return treeWidgetItem;
}

void GeometryBody::HandleLengthLineEditChanged(QString aValue)
{
   mLength_ft = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryBody::HandleHeightLineEditChanged(QString aValue)
{
   mHeight_ft = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryBody::HandleWidthLineEditChanged(QString aValue)
{
   mWidth_ft = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryBody::HandleYawLineEditChanged(QString aValue)
{
   mYaw_rad = aValue.toDouble() * UtMath::cRAD_PER_DEG;
   AdjustAndRedraw(true);
}

void GeometryBody::HandlePitchLineEditChanged(QString aValue)
{
   mPitch_rad = aValue.toDouble() * UtMath::cRAD_PER_DEG;
   AdjustAndRedraw(true);
}

void GeometryBody::HandleRollLineEditChanged(QString aValue)
{
   mRoll_rad = aValue.toDouble() * UtMath::cRAD_PER_DEG;
   AdjustAndRedraw(true);
}

void GeometryBody::HandleIsMainBodyCheckBoxChanged(int aValue)
{
   if (aValue == 0)
   {
      mIsMainBody = false;
   }
   else
   {
      mIsMainBody = true;
   }

   AdjustAndRedraw();
}

void GeometryBody::HandleSymmetricalCheckBoxChanged(int aValue)
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

void GeometryBody::HandleForwardShapeComboBoxChanged(QString aValue)
{
   mForwardShapeString = aValue;

   if (mForwardShapeString == "Blunt")
   {
      if (mForwardShapeLengthItem != nullptr) { mForwardShapeLengthItem->setHidden(true); }
   }
   else
   {
      if (mForwardShapeLengthItem != nullptr) { mForwardShapeLengthItem->setHidden(false); }
   }

   AdjustAndRedraw(true);
}

void GeometryBody::HandleForwardShapeLengthLineEditChanged(QString aValue)
{
   mForwardShapeLength_ft = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryBody::HandleAftShapeComboBoxChanged(QString aValue)
{
   mAftShapeString = aValue;

   if (mAftShapeString == "BoatTail")
   {
      if (mAftShapeLengthItem != nullptr) { mAftShapeLengthItem->setHidden(false); }
      if (mAftShapeDiameterItem != nullptr) { mAftShapeDiameterItem->setHidden(false); }
   }
   else if (mAftShapeString == "Blunt")
   {
      if (mAftShapeLengthItem != nullptr) { mAftShapeLengthItem->setHidden(true); }
      if (mAftShapeDiameterItem != nullptr) { mAftShapeDiameterItem->setHidden(true); }
   }
   else
   {
      if (mAftShapeLengthItem != nullptr) { mAftShapeLengthItem->setHidden(false); }
      if (mAftShapeDiameterItem != nullptr) { mAftShapeDiameterItem->setHidden(true); }
   }

   AdjustAndRedraw(true);
}

void GeometryBody::HandleAftShapeLengthLineEditChanged(QString aValue)
{
   mAftShapeLength_ft = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryBody::HandleAftShapeDiameterLineEditChanged(QString aValue)
{
   mAftShapeDiam_ft = aValue.toDouble();
   AdjustAndRedraw(true);
}

bool GeometryBody::CalcExtents(double& xMin, double& xMax, double& yMin, double& yMax, double& zMin, double& zMax)
{
   double halfLength_ft = 0.5 * mLength_ft;
   double halfWidth_ft = 0.5 * mWidth_ft;
   double halfHeight_ft = 0.5 * mHeight_ft;

   UtDCM DCM(mYaw_rad, mPitch_rad, mRoll_rad);

   UtVec3dX vecRefPt = mRefPoint_ft;

   UtVec3dX v1(halfLength_ft, 0.0, halfHeight_ft);
   UtVec3dX v1Xformed = DCM.InverseTransform(v1);
   v1Xformed += vecRefPt;

   UtVec3dX v2(halfLength_ft, halfWidth_ft, 0.0);
   UtVec3dX v2Xformed = DCM.InverseTransform(v2);
   v2Xformed += vecRefPt;

   UtVec3dX v3(halfLength_ft, 0.0, -halfHeight_ft);
   UtVec3dX v3Xformed = DCM.InverseTransform(v3);
   v3Xformed += vecRefPt;

   UtVec3dX v4(halfLength_ft, -halfWidth_ft, 0.0);
   UtVec3dX v4Xformed = DCM.InverseTransform(v4);
   v4Xformed += vecRefPt;

   UtVec3dX v5(-halfLength_ft, 0.0, halfHeight_ft);
   UtVec3dX v5Xformed = DCM.InverseTransform(v5);
   v5Xformed += vecRefPt;

   UtVec3dX v6(-halfLength_ft, halfWidth_ft, 0.0);
   UtVec3dX v6Xformed = DCM.InverseTransform(v6);
   v6Xformed += vecRefPt;

   UtVec3dX v7(-halfLength_ft, 0.0, -halfHeight_ft);
   UtVec3dX v7Xformed = DCM.InverseTransform(v7);
   v7Xformed += vecRefPt;

   UtVec3dX v8(-halfLength_ft, -halfWidth_ft, 0.0);
   UtVec3dX v8Xformed = DCM.InverseTransform(v8);
   v8Xformed += vecRefPt;

   xMin = v1Xformed.X();
   if (v2Xformed.X() < xMin) { xMin = v2Xformed.X(); }
   if (v3Xformed.X() < xMin) { xMin = v3Xformed.X(); }
   if (v4Xformed.X() < xMin) { xMin = v4Xformed.X(); }
   if (v5Xformed.X() < xMin) { xMin = v5Xformed.X(); }
   if (v6Xformed.X() < xMin) { xMin = v6Xformed.X(); }
   if (v7Xformed.X() < xMin) { xMin = v7Xformed.X(); }
   if (v8Xformed.X() < xMin) { xMin = v8Xformed.X(); }

   xMax = v1Xformed.X();
   if (v2Xformed.X() > xMax) { xMax = v2Xformed.X(); }
   if (v3Xformed.X() > xMax) { xMax = v3Xformed.X(); }
   if (v4Xformed.X() > xMax) { xMax = v4Xformed.X(); }
   if (v5Xformed.X() > xMax) { xMax = v5Xformed.X(); }
   if (v6Xformed.X() > xMax) { xMax = v6Xformed.X(); }
   if (v7Xformed.X() > xMax) { xMax = v7Xformed.X(); }
   if (v8Xformed.X() > xMax) { xMax = v8Xformed.X(); }

   yMin = v1Xformed.Y();
   if (v2Xformed.Y() < yMin) { yMin = v2Xformed.Y(); }
   if (v3Xformed.Y() < yMin) { yMin = v3Xformed.Y(); }
   if (v4Xformed.Y() < yMin) { yMin = v4Xformed.Y(); }
   if (v5Xformed.Y() < yMin) { yMin = v5Xformed.Y(); }
   if (v6Xformed.Y() < yMin) { yMin = v6Xformed.Y(); }
   if (v7Xformed.Y() < yMin) { yMin = v7Xformed.Y(); }
   if (v8Xformed.Y() < yMin) { yMin = v8Xformed.Y(); }

   yMax = v1Xformed.Y();
   if (v2Xformed.Y() > yMax) { yMax = v2Xformed.Y(); }
   if (v3Xformed.Y() > yMax) { yMax = v3Xformed.Y(); }
   if (v4Xformed.Y() > yMax) { yMax = v4Xformed.Y(); }
   if (v5Xformed.Y() > yMax) { yMax = v5Xformed.Y(); }
   if (v6Xformed.Y() > yMax) { yMax = v6Xformed.Y(); }
   if (v7Xformed.Y() > yMax) { yMax = v7Xformed.Y(); }
   if (v8Xformed.Y() > yMax) { yMax = v8Xformed.Y(); }

   zMin = v1Xformed.Z();
   if (v2Xformed.Z() < zMin) { zMin = v2Xformed.Z(); }
   if (v3Xformed.Z() < zMin) { zMin = v3Xformed.Z(); }
   if (v4Xformed.Z() < zMin) { zMin = v4Xformed.Z(); }
   if (v5Xformed.Z() < zMin) { zMin = v5Xformed.Z(); }
   if (v6Xformed.Z() < zMin) { zMin = v6Xformed.Z(); }
   if (v7Xformed.Z() < zMin) { zMin = v7Xformed.Z(); }
   if (v8Xformed.Z() < zMin) { zMin = v8Xformed.Z(); }

   zMax = v1Xformed.Z();
   if (v2Xformed.Z() > zMax) { zMax = v2Xformed.Z(); }
   if (v3Xformed.Z() > zMax) { zMax = v3Xformed.Z(); }
   if (v4Xformed.Z() > zMax) { zMax = v4Xformed.Z(); }
   if (v5Xformed.Z() > zMax) { zMax = v5Xformed.Z(); }
   if (v6Xformed.Z() > zMax) { zMax = v6Xformed.Z(); }
   if (v7Xformed.Z() > zMax) { zMax = v7Xformed.Z(); }
   if (v8Xformed.Z() > zMax) { zMax = v8Xformed.Z(); }

   if (mSymmetrical)
   {
      // Consider the other body
      double currentBodyY = mRefPoint_ft.Y();
      double delta = currentBodyY - mSymmetryHorizontalY;
      double otherBodyY = currentBodyY - 2.0 * delta;

      if (otherBodyY < currentBodyY)
      {
         // Current engine (on right) outer extent
         double outerExtentDeltaY = yMax - currentBodyY;

         // Other engine is on the left
         yMin = otherBodyY - outerExtentDeltaY;
      }
      else
      {
         // Current engine (on left) outer extent
         double outerExtentDeltaY = yMin - currentBodyY;

         // Other engine is on the right
         yMax = otherBodyY - outerExtentDeltaY;
      }
   }

   return true;
}

double GeometryBody::GetMassArea_ft2()
{
   double midLength = mLength_ft - mForwardShapeLength_ft - mAftShapeLength_ft;
   double avgRadius = (mHeight_ft + mWidth_ft) * 0.25;
   double twoPi = UtMath::cTWO_PI;

   // Ensure that mid length is not negative
   if (midLength < 0.0) { midLength = 0.0; }

   // Note: These are simple calculations and only approximate

   double forwardArea_ft = 0.0;
   if (mForwardShapeString == "Blunt")
   {
      forwardArea_ft = mForwardShapeLength_ft * (twoPi * avgRadius);
   }
   else
   {
      forwardArea_ft = mForwardShapeLength_ft * 0.5 * (twoPi * avgRadius);
   }

   double midArea_ft = midLength * (twoPi * avgRadius);

   double aftArea_ft = 0.0;
   if (mAftShapeString == "Blunt")
   {
      aftArea_ft = mAftShapeLength_ft * (twoPi * avgRadius);
   }
   else if (mAftShapeString == "BoatTail")
   {
      double aftRadius = 0.5 * mAftShapeDiam_ft;

      // From Wolfram MathWorld's "Conical Frustum"
      // Area = pi * (R1 + R2) * sqrt((R1-R2)^2 + h^2)
      // Where R1 is the larger radius (avgRadius)
      //       R2 is the smaller radius (aftRadius)
      //       h  is the length of the aft shape (mAftShapeLength_ft)
      aftArea_ft = UtMath::cPI * (avgRadius + aftRadius) *
                   std::sqrt((avgRadius - aftRadius) * (avgRadius - aftRadius) +
                              mAftShapeLength_ft * mAftShapeLength_ft);
   }
   else
   {
      aftArea_ft = mAftShapeLength_ft * 0.5 * (twoPi * avgRadius);
   }

   return forwardArea_ft + midArea_ft + aftArea_ft;
}

void GeometryBody::CalcMassProperties(double aTotalMass_lbs)
{
   const double epsilon        = std::numeric_limits<double>::epsilon();
   const double avgDiameter_ft = 0.5 * (mHeight_ft + mWidth_ft);
   const double avgRadiusOuter_ft = avgDiameter_ft * 0.5;
   const double avgRadiusInner_ft = avgRadiusOuter_ft * 0.8;
   const double pi             = UtMath::cPI;

   double radiusOuterSquared = avgRadiusOuter_ft * avgRadiusOuter_ft;
   double radiusInnerSquared = avgRadiusInner_ft * avgRadiusInner_ft;
   double lengthSquared = mLength_ft * mLength_ft;

   double foreLength_ft   = 0.0;
   double aftLength_ft    = 0.0;
   double foreArea        = 0.0;
   double foreCentroidX   = 0.0;
   double midArea         = 0.0;
   double midCentroidX    = 0.0;
   double aftArea         = 0.0;
   double aftCentroidX    = 0.0;

   // First, determine the fore data
   if (mForwardShapeString == "Round")
   {
      if (mForwardShapeLength_ft > epsilon)
      {
         foreLength_ft = mForwardShapeLength_ft;
         foreArea      = 0.5 * pi * avgRadiusOuter_ft * foreLength_ft;
         foreCentroidX = foreLength_ft * 0.42;
      }
   }
   else if (mForwardShapeString == "Ogive")
   {
      if (mForwardShapeLength_ft > epsilon)
      {
         foreLength_ft = mForwardShapeLength_ft;

         // We assume the area and centroid of the ogive is roughly between a circle and cone
         foreArea      = 0.5 * ((0.5 * pi * avgRadiusOuter_ft * foreLength_ft) + (foreLength_ft * avgRadiusOuter_ft));
         foreCentroidX = foreLength_ft * 0.5 * (0.42 + 0.33333);
      }
   }
   else if (mForwardShapeString == "Cone")
   {
      if (mForwardShapeLength_ft > epsilon)
      {
         foreLength_ft = mForwardShapeLength_ft;
         foreArea      = foreLength_ft * avgRadiusOuter_ft;
         foreCentroidX = foreLength_ft * 0.33333;
      }
   }

   // Next, determine the aft data
   if (mAftShapeString == "Round")
   {
      if (mAftShapeLength_ft > epsilon)
      {
         aftLength_ft = mAftShapeLength_ft;
         aftArea = 0.5 * pi * avgRadiusOuter_ft * aftLength_ft;
         aftCentroidX = aftLength_ft * 0.42;
      }
   }
   else if (mAftShapeString == "Ogive")
   {
      if (mAftShapeLength_ft > epsilon)
      {
         aftLength_ft = mAftShapeLength_ft;

         // We assume the area and centroid of the ogive is roughly between a circle and cone
         aftArea = 0.5 * ((0.5 * pi * avgRadiusOuter_ft * aftLength_ft) + (aftLength_ft * avgRadiusOuter_ft));
         aftCentroidX = aftLength_ft * 0.5 * (0.42 + 0.33333);
      }
   }
   else if (mAftShapeString == "Cone")
   {
      if (mAftShapeLength_ft > epsilon)
      {
         aftLength_ft = mAftShapeLength_ft;
         aftArea = aftLength_ft * avgRadiusOuter_ft;
         aftCentroidX = aftLength_ft * 0.33333;
      }
   }
   else if (mAftShapeString == "Boattail")
   {
      if (mAftShapeLength_ft > epsilon)
      {
         aftLength_ft = mAftShapeLength_ft;
         aftArea      = 0.5 * aftLength_ft * (avgDiameter_ft + mAftShapeDiam_ft);
         aftCentroidX = aftLength_ft * (2.0 * mAftShapeDiam_ft + avgDiameter_ft) / (3.0 * (mAftShapeDiam_ft + avgDiameter_ft));
      }
   }

   // Now, we deal with the mid section, with Mach effects
   double midLength = mLength_ft - foreLength_ft - aftLength_ft;
   midCentroidX = 0.5 * midLength;
   midArea = avgDiameter_ft * midLength;

   // Next, we adjust the centroidX values
   double mostForwardX = mLength_ft * 0.5;
   double mostAftX     = -mLength_ft * 0.5;
   foreCentroidX = (mostForwardX - foreLength_ft) + foreCentroidX;
   midCentroidX  = (mostForwardX - foreLength_ft) - midCentroidX;
   aftCentroidX  = (mostAftX + aftLength_ft) - aftCentroidX;

   double denominator = (foreArea + midArea + aftArea);
   double xOffset_ft  = 0.0;
   if (denominator > 0.0)
   {
      xOffset_ft = ((foreArea * foreCentroidX) + (midArea * midCentroidX) + (aftArea * aftCentroidX)) / denominator;
   }

   UtVec3dX foreAftAdjustment(xOffset_ft, 0.0, 0.0);
   UtVec3dX cgPt = mRefPoint_ft + foreAftAdjustment;

   // We assume a body is roughly similar to a hollow cylinder
   // Ixx = (1/2) * mass * ((radius_inner*radius_inner)+(radius_outer*radius_outer))
   // Iyy = (1/12) * mass * (3*(radius_outer*radius_outer) - 3*(radius_inner*radius_inner) + length*length)
   // Izz = (1/12) * mass * (3*(radius_outer*radius_outer) - 3*(radius_inner*radius_inner) + length*length)
   // We further assume that the inner radius is 80% of the outer radius

   // A body may have forward and aft shapes. Forward shapes include blunt, round, cone, and ogive.
   // Aft shapes include blunt, boat-tail, round, cone, and ogive. We assume that the round, cone,
   // ogive, and boat-tail shapes may be approximated by a hollow cone, with
   // Ixx = (1/2) * mass * (radius_outer*radius_outer)
   // This results in a reduced Ixx for the forward/aft shape as compared to the cylindrical
   // core section of the body.

   // We assume that the forward and aft shapes will not significantly effect the Iyy and Izz, and
   // as a result, we assume a hollow cylinder for the entire body for Iyy and Izz. While not true,
   // it is an assumption to simplify calculations.

   // Regardless of whether or not we have symmetry, begin by calculating the base Ixx, Iyy, and Izz:

   // Set the mass
   mCalculatedMass_lbs = aTotalMass_lbs;

   // Calc the mass (may be reduced due to symmetry)
   double mass_slugs = mCalculatedMass_lbs * mLbsToSlugs;
   if (mSymmetrical)
   {
      mass_slugs *= 0.5; // Each part is half of the total mass
   }

   // Calculate the fore/aft shape lengths

   double calculatedIxx = 0.0;
   if (mForwardShapeString == "Blunt")
   {
      if (mAftShapeString == "Blunt")
      {
         // This is simply a full cylinder
         calculatedIxx = 0.5 * mass_slugs * (radiusInnerSquared + radiusOuterSquared);
      }
      else
      {
         // This is a blunt forward shape and tapered aft shape
         double aftFraction = aftLength_ft / mLength_ft;
         double aftIxx = aftFraction * 0.5 * mass_slugs * radiusOuterSquared;
         double mainIxx = (1.0 - aftFraction) * 0.5 * mass_slugs * (radiusInnerSquared + radiusOuterSquared);
         calculatedIxx = mainIxx + aftIxx;
      }
   }
   else
   {
      if (mAftShapeString == "Blunt")
      {
         // This is a tapered forward shape and a blunt aft shape
         double forwardFraction = foreLength_ft / mLength_ft;
         double forwardIxx = forwardFraction * 0.5 * mass_slugs * radiusOuterSquared;
         double mainIxx = (1.0 - forwardFraction) * 0.5 * mass_slugs * (radiusInnerSquared + radiusOuterSquared);
         calculatedIxx = mainIxx + forwardIxx;
      }
      else
      {
         // This is a tapered forward and aft shape
         double forwardFraction = foreLength_ft / mLength_ft;
         double forwardIxx = forwardFraction * 0.5 * mass_slugs * radiusOuterSquared;
         double aftFraction = aftLength_ft / mLength_ft;
         double aftIxx = aftFraction * 0.5 * mass_slugs * radiusOuterSquared;
         double mainFraction = (mLength_ft - foreLength_ft - aftLength_ft) / mLength_ft;
         double mainIxx = mainFraction * 0.5 * mass_slugs * (radiusInnerSquared + radiusOuterSquared);
         calculatedIxx = mainIxx + forwardIxx + aftIxx;
      }
   }

   double calculatedIyy = (1.0 / 12.0) * mass_slugs * (3.0 * radiusOuterSquared - 3.0 * radiusInnerSquared + lengthSquared);
   double calculatedIzz = calculatedIyy;

   // Now we deal with either a single body or two symmetrical bodies
   if (!mSymmetrical)
   {
      // This is a single body

      // Correct for location, using parallel axis theorem
      double distX = sqrt(cgPt.Y() * cgPt.Y() + cgPt.Z() * cgPt.Z());
      double distY = sqrt(cgPt.X() * cgPt.X() + cgPt.Z() * cgPt.Z());
      double distZ = sqrt(cgPt.X() * cgPt.X() + cgPt.Y() * cgPt.Y());
      double dX2 = distX * distX;
      double dY2 = distY * distY;
      double dZ2 = distZ * distZ;

      mCalculatedIxx = calculatedIxx + (mass_slugs * dX2);
      mCalculatedIyy = calculatedIyy + (mass_slugs * dY2);
      mCalculatedIzz = calculatedIzz + (mass_slugs * dZ2);

      // Calculate the location of the cg -- we assume it is always at the reference point
      mCalculatedCG_ft = cgPt;
   }
   else
   {
      // This is two symmetrical bodies

      // First, we deal with the primary body and correct for location, using parallel axis theorem (PAT)
      double distX = sqrt(cgPt.Y() * cgPt.Y() + cgPt.Z() * cgPt.Z());
      double distY = sqrt(cgPt.X() * cgPt.X() + cgPt.Z() * cgPt.Z());
      double distZ = sqrt(cgPt.X() * cgPt.X() + cgPt.Y() * cgPt.Y());
      double dX2 = distX * distX;
      double dY2 = distY * distY;
      double dZ2 = distZ * distZ;

      // Calc the total inertia (including PAT)
      double primaryBodyIxx = calculatedIxx + (mass_slugs * dX2);
      double primaryBodyIyy = calculatedIyy + (mass_slugs * dY2);
      double primaryBodyIzz = calculatedIzz + (mass_slugs * dZ2);

      // Now, determine location of other (secondary) body
      double currentBodyY = cgPt.Y();
      double delta = currentBodyY - mSymmetryHorizontalY;
      double otherBodyY = currentBodyY - 2.0 * delta;

      // Correct for location, using parallel axis theorem
      UtVec3dX refPt(cgPt.X(), otherBodyY, cgPt.Z());
      distX = sqrt(refPt.Y() * refPt.Y() + refPt.Z() * refPt.Z());
      distY = sqrt(refPt.X() * refPt.X() + refPt.Z() * refPt.Z());
      distZ = sqrt(refPt.X() * refPt.X() + refPt.Y() * refPt.Y());
      dX2 = distX * distX;
      dY2 = distY * distY;
      dZ2 = distZ * distZ;

      // Calc the secondary body inertia
      double secondaryBodyIxx = calculatedIxx + (mass_slugs * dX2);
      double secondaryBodyIyy = calculatedIyy + (mass_slugs * dY2);
      double secondaryBodyIzz = calculatedIzz + (mass_slugs * dZ2);

      // Calc the total inertia
      mCalculatedIxx = primaryBodyIxx + secondaryBodyIxx;
      mCalculatedIyy = primaryBodyIyy + secondaryBodyIyy;
      mCalculatedIzz = primaryBodyIzz + secondaryBodyIzz;

      // Calculate the location of the cg -- we assume it is always at the reference point,
      // but centered in the Y-axis "between" the two objects
      double centerY = (currentBodyY + otherBodyY) * 0.5;
      mCalculatedCG_ft.Set(cgPt.X(), centerY, cgPt.Z());
   }
}

bool GeometryBody::GetCG_ft(UtVec3dX& aCg_ft)
{
   aCg_ft = mCalculatedCG_ft;
   return true;
}

QStringList GeometryBody::Audit()
{
   QStringList errorList = GeometryObject::Audit();

   QString nameString(mName.c_str());

   if (mCalculatedMass_lbs < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a mass (mCalculatedMass_lbs is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   if (mCalculatedIxx < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a rotational inertia about the x-axis (mCalculatedIxx is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   if (mCalculatedIyy < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a rotational inertia about the x-axis (mCalculatedIyy is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   if (mCalculatedIzz < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a rotational inertia about the x-axis (mCalculatedIzz is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   if (mLength_ft < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a length (mLength_ft is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   if (mHeight_ft < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a height (mHeight_ft is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   if (mWidth_ft < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a width (mWidth_ft is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   if (mForwardShapeString != "Blunt")
   {
      if (mForwardShapeLength_ft < std::numeric_limits<double>::epsilon())
      {
         QString errorString(" requires a forward shape length (mForwardShapeLength_ft is zero)");
         errorString = nameString + errorString;
         errorList.push_back(errorString);
      }
   }

   if ((mAftShapeString == "Round") || (mAftShapeString == "Ogive") || (mAftShapeString == "Cone"))
   {
      if (mAftShapeLength_ft < std::numeric_limits<double>::epsilon())
      {
         QString errorString(" requires an aft shape length (mAftShapeLength_ft is zero)");
         errorString = nameString + errorString;
         errorList.push_back(errorString);
      }
   }
   else if (mAftShapeString == "BoatTail")
   {
      if (mAftShapeLength_ft < std::numeric_limits<double>::epsilon())
      {
         QString errorString(" requires an aft shape length (mAftShapeLength_ft is zero)");
         errorString = nameString + errorString;
         errorList.push_back(errorString);
      }
      if (mAftShapeDiam_ft < std::numeric_limits<double>::epsilon())
      {
         QString errorString(" requires an aft shape diameter (mAftShapeDiam_ft is zero)");
         errorString = nameString + errorString;
         errorList.push_back(errorString);
      }
   }

   return errorList;
}

} // namespace Designer
