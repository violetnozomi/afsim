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

#include "GeometryFuselage.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QTreeWidget>

#include "MoverCreatorLineEdit.hpp"

#include "P6DofUtils.hpp"
#include "UtMath.hpp"

namespace Designer
{

GeometryFuselage::GeometryFuselage(Vehicle* aVehicle)
   : GeometryBody(aVehicle)
{
   // Overwrite the GeometryFuselage type
   mGeometryObjectTypeString.assign("GeometryFuselage");
}

GeometryFuselage::~GeometryFuselage()
{
}

QTreeWidgetItem* GeometryFuselage::CreateTreeWidgetItem(QString& aName, QTreeWidget* aTreeWidgetPtr, bool aNoneditableName)
{
   // Create the tree widget item at the GeometryObject level
   QTreeWidgetItem* treeWidgetItem = GeometryObject::CreateTreeWidgetItem(aName, aTreeWidgetPtr, aNoneditableName);

   // Enforce the geometry type
   mGeometryObjectTypeString = "GeometryFuselage";

   // Set Qt flags
   if (aNoneditableName)
   {
      treeWidgetItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
   }
   else
   {
      treeWidgetItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
   }

   QTreeWidgetItem* lengthItem = new QTreeWidgetItem(treeWidgetItem, QStringList("Length"));
   lengthItem->setToolTip(0, "The length of the fuselage in feet");
   auto lengthLineEdit = new MoverCreatorLineEdit(lengthItem);
   lengthLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   lengthLineEdit->setFrame(false);
   lengthLineEdit->setText(QString::number(mLength_ft));
   aTreeWidgetPtr->setItemWidget(lengthItem, 1, lengthLineEdit);
   aTreeWidgetPtr->setItemWidget(lengthItem, 2, new QLabel(QString::fromStdString("ft")));
   connect(lengthLineEdit, &QLineEdit::textChanged, this, &GeometryFuselage::HandleLengthLineEditChanged);
   connect(lengthLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* heightItem = new QTreeWidgetItem(treeWidgetItem, QStringList("Height"));
   heightItem->setToolTip(0, "The height of the fuselage in feet");
   auto heightLineEdit = new MoverCreatorLineEdit(heightItem);
   heightLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   heightLineEdit->setFrame(false);
   heightLineEdit->setText(QString::number(mHeight_ft));
   aTreeWidgetPtr->setItemWidget(heightItem, 1, heightLineEdit);
   aTreeWidgetPtr->setItemWidget(heightItem, 2, new QLabel(QString::fromStdString("ft")));
   connect(heightLineEdit, &QLineEdit::textChanged, this, &GeometryFuselage::HandleHeightLineEditChanged);
   connect(heightLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* widthItem = new QTreeWidgetItem(treeWidgetItem, QStringList("Width"));
   widthItem->setToolTip(0, "The width of the fuselage in feet");
   auto widthLineEdit = new MoverCreatorLineEdit(widthItem);
   widthLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   widthLineEdit->setFrame(false);
   widthLineEdit->setText(QString::number(mWidth_ft));
   aTreeWidgetPtr->setItemWidget(widthItem, 1, widthLineEdit);
   aTreeWidgetPtr->setItemWidget(widthItem, 2, new QLabel(QString::fromStdString("ft")));
   connect(widthLineEdit, &QLineEdit::textChanged, this, &GeometryFuselage::HandleWidthLineEditChanged);
   connect(widthLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* yaw = new QTreeWidgetItem(treeWidgetItem, QStringList("Yaw Angle"));
   yaw->setToolTip(0, "The yaw of the fuselage in degrees");
   auto yawLineEdit = new MoverCreatorLineEdit(yaw);
   yawLineEdit->setFrame(false);
   yawLineEdit->setText(QString::number(mYaw_rad * UtMath::cDEG_PER_RAD));
   aTreeWidgetPtr->setItemWidget(yaw, 1, yawLineEdit);
   aTreeWidgetPtr->setItemWidget(yaw, 2, new QLabel(QString::fromStdString("deg")));
   connect(yawLineEdit, &QLineEdit::textChanged, this, &GeometryFuselage::HandleYawLineEditChanged);
   connect(yawLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* pitch = new QTreeWidgetItem(treeWidgetItem, QStringList("Pitch Angle"));
   pitch->setToolTip(0, "The pitch of the fuselage in degrees");
   auto pitchLineEdit = new MoverCreatorLineEdit(pitch);
   pitchLineEdit->setFrame(false);
   pitchLineEdit->setText(QString::number(mPitch_rad * UtMath::cDEG_PER_RAD));
   aTreeWidgetPtr->setItemWidget(pitch, 1, pitchLineEdit);
   aTreeWidgetPtr->setItemWidget(pitch, 2, new QLabel(QString::fromStdString("deg")));
   connect(pitchLineEdit, &QLineEdit::textChanged, this, &GeometryFuselage::HandlePitchLineEditChanged);
   connect(pitchLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* roll = new QTreeWidgetItem(treeWidgetItem, QStringList("Roll Angle"));
   roll->setToolTip(0, "The roll of the fuselage in degrees");
   auto rollLineEdit = new MoverCreatorLineEdit(roll);
   rollLineEdit->setFrame(false);
   rollLineEdit->setText(QString::number(mRoll_rad * UtMath::cDEG_PER_RAD));
   aTreeWidgetPtr->setItemWidget(roll, 1, rollLineEdit);
   aTreeWidgetPtr->setItemWidget(roll, 2, new QLabel(QString::fromStdString("deg")));
   connect(rollLineEdit, &QLineEdit::textChanged, this, &GeometryFuselage::HandleRollLineEditChanged);
   connect(rollLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* forwardShape = new QTreeWidgetItem(treeWidgetItem, QStringList("Forward Shape"));
   forwardShape->setToolTip(0, "The forward shape of the fuselage, which can be ogive, conical, rounded, or blunt");
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
   connect(forwardShapeLengthLineEdit, &QLineEdit::textChanged, this, &GeometryFuselage::HandleForwardShapeLengthLineEditChanged);
   connect(forwardShapeLengthLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mForwardShapeLengthItem = forwardShapeLength;

   QTreeWidgetItem* aftShape = new QTreeWidgetItem(treeWidgetItem, QStringList("Aft Shape"));
   aftShape->setToolTip(0, "The aft shape of the fuselage, which can be ogive, conical, rounded, boat tailed or blunt");
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
   connect(aftShapeLengthLineEdit, &QLineEdit::textChanged, this, &GeometryFuselage::HandleAftShapeLengthLineEditChanged);
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
   connect(aftShapeDiameterLineEdit, &QLineEdit::textChanged, this, &GeometryFuselage::HandleAftShapeDiameterLineEditChanged);
   connect(aftShapeDiameterLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mAftShapeDiameterItem = aftShapeDiameter;
   mAftShapeDiameterItem->setHidden(true); // This starts hidden

   QTreeWidgetItem* canopyPresent = new QTreeWidgetItem(treeWidgetItem, QStringList("Canopy Present"));
   canopyPresent->setToolTip(0, "If checked, the canopy will be visible");
   QCheckBox* canopyPresentCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(canopyPresent, 1, canopyPresentCheckBox);
   aTreeWidgetPtr->setItemWidget(canopyPresent, 2, new QLabel(QString::fromStdString("")));
   connect(canopyPresentCheckBox, &QCheckBox::stateChanged, this, &GeometryFuselage::HandleCanopyPresentCheckBoxChanged);

   QTreeWidgetItem* canopyTotalLength = new QTreeWidgetItem(treeWidgetItem, QStringList("Canopy Total Length"));
   canopyTotalLength->setToolTip(0, "The length of the canopy in feet");
   auto canopyTotalLengthLineEdit = new MoverCreatorLineEdit(canopyTotalLength);
   canopyTotalLengthLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   canopyTotalLengthLineEdit->setFrame(false);
   canopyTotalLengthLineEdit->setText(QString::number(mCanopyTotalLength));
   aTreeWidgetPtr->setItemWidget(canopyTotalLength, 1, canopyTotalLengthLineEdit);
   aTreeWidgetPtr->setItemWidget(canopyTotalLength, 2, new QLabel(QString::fromStdString("ft")));
   connect(canopyTotalLengthLineEdit, &QLineEdit::textChanged, this, &GeometryFuselage::HandleCanopyTotalLengthLineEditChanged);
   connect(canopyTotalLengthLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mCanopyTotalLengthWidget = canopyTotalLength;
   mCanopyTotalLengthWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* canopyForwardLength = new QTreeWidgetItem(treeWidgetItem, QStringList("Canopy Forward Length"));
   canopyForwardLength->setToolTip(0, "The length of the canopy forward section in feet");
   auto canopyForwardLengthLineEdit = new MoverCreatorLineEdit(canopyForwardLength);
   canopyForwardLengthLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   canopyForwardLengthLineEdit->setFrame(false);
   canopyForwardLengthLineEdit->setText(QString::number(mCanopyForwardLength));
   aTreeWidgetPtr->setItemWidget(canopyForwardLength, 1, canopyForwardLengthLineEdit);
   aTreeWidgetPtr->setItemWidget(canopyForwardLength, 2, new QLabel(QString::fromStdString("ft")));
   connect(canopyForwardLengthLineEdit, &QLineEdit::textChanged, this, &GeometryFuselage::HandleCanopyForwardLengthLineEditChanged);
   connect(canopyForwardLengthLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mCanopyForwardLengthWidget = canopyForwardLength;
   mCanopyForwardLengthWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* canopyAftLength = new QTreeWidgetItem(treeWidgetItem, QStringList("Canopy Aft Length"));
   canopyAftLength->setToolTip(0, "The length of the canopy aft section in feet");
   auto canopyAftLengthLineEdit = new MoverCreatorLineEdit(canopyAftLength);
   canopyAftLengthLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   canopyAftLengthLineEdit->setFrame(false);
   canopyAftLengthLineEdit->setText(QString::number(mCanopyAftLength));
   aTreeWidgetPtr->setItemWidget(canopyAftLength, 1, canopyAftLengthLineEdit);
   aTreeWidgetPtr->setItemWidget(canopyAftLength, 2, new QLabel(QString::fromStdString("ft")));
   connect(canopyAftLengthLineEdit, &QLineEdit::textChanged, this, &GeometryFuselage::HandleCanopyAftLengthLineEditChanged);
   connect(canopyAftLengthLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mCanopyAftLengthWidget = canopyAftLength;
   mCanopyAftLengthWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* canopyHeight = new QTreeWidgetItem(treeWidgetItem, QStringList("Canopy Height"));
   canopyHeight->setToolTip(0, "The height of the canopy in feet");
   auto canopyHeightLineEdit = new MoverCreatorLineEdit(canopyHeight);
   canopyHeightLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   canopyHeightLineEdit->setFrame(false);
   canopyHeightLineEdit->setText(QString::number(mCanopyHeight));
   aTreeWidgetPtr->setItemWidget(canopyHeight, 1, canopyHeightLineEdit);
   aTreeWidgetPtr->setItemWidget(canopyHeight, 2, new QLabel(QString::fromStdString("ft")));
   connect(canopyHeightLineEdit, &QLineEdit::textChanged, this, &GeometryFuselage::HandleCanopyHeightLineEditChanged);
   connect(canopyHeightLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mCanopyHeightWidget = canopyHeight;
   mCanopyHeightWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* canopyWidth = new QTreeWidgetItem(treeWidgetItem, QStringList("Canopy Width"));
   canopyWidth->setToolTip(0, "The width of the canopy in feet");
   auto canopyWidthLineEdit = new MoverCreatorLineEdit(canopyWidth);
   canopyWidthLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   canopyWidthLineEdit->setFrame(false);
   canopyWidthLineEdit->setText(QString::number(mCanopyWidth));
   aTreeWidgetPtr->setItemWidget(canopyWidth, 1, canopyWidthLineEdit);
   aTreeWidgetPtr->setItemWidget(canopyWidth, 2, new QLabel(QString::fromStdString("ft")));
   connect(canopyWidthLineEdit, &QLineEdit::textChanged, this, &GeometryFuselage::HandleCanopyWidthLineEditChanged);
   connect(canopyWidthLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mCanopyWidthWidget = canopyWidth;
   mCanopyWidthWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* canopyRefX = new QTreeWidgetItem(treeWidgetItem, QStringList("Canopy Ref-X"));
   canopyRefX->setToolTip(0, "The X coordinate of the canopy's reference point");
   auto canopyRefXLineEdit = new MoverCreatorLineEdit(canopyRefX);
   canopyRefXLineEdit->setFrame(false);
   canopyRefXLineEdit->setText(QString::number(mCanopyRefX));
   aTreeWidgetPtr->setItemWidget(canopyRefX, 1, canopyRefXLineEdit);
   aTreeWidgetPtr->setItemWidget(canopyRefX, 2, new QLabel(QString::fromStdString("ft")));
   connect(canopyRefXLineEdit, &QLineEdit::textChanged, this, &GeometryFuselage::HandleCanopyRefX_LineEditChanged);
   connect(canopyRefXLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mCanopyRefX_Widget = canopyRefX;
   mCanopyRefX_Widget->setHidden(true); // Start hidden

   QTreeWidgetItem* canopyRefY = new QTreeWidgetItem(treeWidgetItem, QStringList("Canopy Ref-Y"));
   canopyRefY->setToolTip(0, "The Y coordinate of the canopy's reference point");
   auto canopyRefYLineEdit = new MoverCreatorLineEdit(canopyRefY);
   canopyRefYLineEdit->setFrame(false);
   canopyRefYLineEdit->setText(QString::number(mCanopyRefY));
   aTreeWidgetPtr->setItemWidget(canopyRefY, 1, canopyRefYLineEdit);
   aTreeWidgetPtr->setItemWidget(canopyRefY, 2, new QLabel(QString::fromStdString("ft")));
   connect(canopyRefYLineEdit, &QLineEdit::textChanged, this, &GeometryFuselage::HandleCanopyRefY_LineEditChanged);
   connect(canopyRefYLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mCanopyRefY_Widget = canopyRefY;
   mCanopyRefY_Widget->setHidden(true); // Start hidden

   QTreeWidgetItem* canopyRefZ = new QTreeWidgetItem(treeWidgetItem, QStringList("Canopy Ref-Z"));
   canopyRefZ->setToolTip(0, "The Z coordinate of the canopy's reference point");
   auto canopyRefZLineEdit = new MoverCreatorLineEdit(canopyRefZ);
   canopyRefZLineEdit->setFrame(false);
   canopyRefZLineEdit->setText(QString::number(mCanopyRefZ));
   aTreeWidgetPtr->setItemWidget(canopyRefZ, 1, canopyRefZLineEdit);
   aTreeWidgetPtr->setItemWidget(canopyRefZ, 2, new QLabel(QString::fromStdString("ft")));
   connect(canopyRefZLineEdit, &QLineEdit::textChanged, this, &GeometryFuselage::HandleCanopyRefZ_LineEditChanged);
   connect(canopyRefZLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mCanopyRefZ_Widget = canopyRefZ;
   mCanopyRefZ_Widget->setHidden(true); // Start hidden

   QTreeWidgetItem* massFactor = new QTreeWidgetItem(treeWidgetItem, QStringList("Mass Factor"));
   massFactor->setToolTip(0, "The mass factor multiplier of the fuselage");
   auto massFactorLineEdit = new MoverCreatorLineEdit(massFactor);
   massFactorLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   massFactorLineEdit->setFrame(false);
   massFactorLineEdit->setText(QString::number(mMassFactor));
   aTreeWidgetPtr->setItemWidget(massFactor, 1, massFactorLineEdit);
   aTreeWidgetPtr->setItemWidget(massFactor, 2, new QLabel(QString::fromStdString("")));
   connect(massFactorLineEdit, &QLineEdit::textChanged, this, &GeometryFuselage::HandleMassFactorLineEditChanged);
   connect(massFactorLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* symmetrical = new QTreeWidgetItem(treeWidgetItem, QStringList("Symmetrical"));
   symmetrical->setToolTip(0, "If checked, the fuselage will utilize symmetry");
   QCheckBox* symmetricalCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(symmetrical, 1, symmetricalCheckBox);
   aTreeWidgetPtr->setItemWidget(symmetrical, 2, new QLabel(QString::fromStdString("")));
   connect(symmetricalCheckBox, &QCheckBox::stateChanged, this, &GeometryFuselage::HandleSymmetricalCheckBoxChanged);

   return treeWidgetItem;
}

void GeometryFuselage::HandleCanopyTotalLengthLineEditChanged(QString aValue)
{
   mCanopyTotalLength = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryFuselage::HandleCanopyForwardLengthLineEditChanged(QString aValue)
{
   mCanopyForwardLength = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryFuselage::HandleCanopyAftLengthLineEditChanged(QString aValue)
{
   mCanopyAftLength = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryFuselage::HandleCanopyHeightLineEditChanged(QString aValue)
{
   mCanopyHeight = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryFuselage::HandleCanopyWidthLineEditChanged(QString aValue)
{
   mCanopyWidth = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryFuselage::HandleCanopyRefX_LineEditChanged(QString aValue)
{
   mCanopyRefX = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryFuselage::HandleCanopyRefY_LineEditChanged(QString aValue)
{
   mCanopyRefY = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryFuselage::HandleCanopyRefZ_LineEditChanged(QString aValue)
{
   mCanopyRefZ = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryFuselage::HandleCanopyPresentCheckBoxChanged(int aValue)
{
   if (aValue == 0)
   {
      mCanopyPresent = false;
   }
   else
   {
      mCanopyPresent = true;
   }

   AdjustAndRedraw(true);
}

void GeometryFuselage::AdjustAndRedraw(bool aRecalcMassAndInertia)
{
   if (mCanopyPresent)
   {
      if (mCanopyTotalLengthWidget != nullptr) { mCanopyTotalLengthWidget->setHidden(false); }
      if (mCanopyForwardLengthWidget != nullptr) { mCanopyForwardLengthWidget->setHidden(false); }
      if (mCanopyAftLengthWidget != nullptr) { mCanopyAftLengthWidget->setHidden(false); }
      if (mCanopyHeightWidget != nullptr) { mCanopyHeightWidget->setHidden(false); }
      if (mCanopyWidthWidget != nullptr) { mCanopyWidthWidget->setHidden(false); }
      if (mCanopyRefX_Widget != nullptr) { mCanopyRefX_Widget->setHidden(false); }
      if (mCanopyRefY_Widget != nullptr) { mCanopyRefY_Widget->setHidden(false); }
      if (mCanopyRefZ_Widget != nullptr) { mCanopyRefZ_Widget->setHidden(false); }
   }
   else
   {
      if (mCanopyTotalLengthWidget != nullptr) { mCanopyTotalLengthWidget->setHidden(true); }
      if (mCanopyForwardLengthWidget != nullptr) { mCanopyForwardLengthWidget->setHidden(true); }
      if (mCanopyAftLengthWidget != nullptr) { mCanopyAftLengthWidget->setHidden(true); }
      if (mCanopyHeightWidget != nullptr) { mCanopyHeightWidget->setHidden(true); }
      if (mCanopyWidthWidget != nullptr) { mCanopyWidthWidget->setHidden(true); }
      if (mCanopyRefX_Widget != nullptr) { mCanopyRefX_Widget->setHidden(true); }
      if (mCanopyRefY_Widget != nullptr) { mCanopyRefY_Widget->setHidden(true); }
      if (mCanopyRefZ_Widget != nullptr) { mCanopyRefZ_Widget->setHidden(true); }
   }

   GeometryObject::AdjustAndRedraw(aRecalcMassAndInertia);
}

QStringList GeometryFuselage::Audit()
{
   QStringList errorList = GeometryBody::Audit();

   QString nameString(mName.c_str());

   if (mCanopyPresent)
   {
      if (mCanopyTotalLength < std::numeric_limits<double>::epsilon())
      {
         QString errorString(" requires a canopy total length (mCanopyTotalLength is zero)");
         errorString = nameString + errorString;
         errorList.push_back(errorString);
      }

      if (mCanopyForwardLength < std::numeric_limits<double>::epsilon())
      {
         QString errorString(" requires a canopy forward length (mCanopyForwardLength is zero)");
         errorString = nameString + errorString;
         errorList.push_back(errorString);
      }

      if (mCanopyAftLength < std::numeric_limits<double>::epsilon())
      {
         QString errorString(" requires a canopy aft length (mCanopyAftLength is zero)");
         errorString = nameString + errorString;
         errorList.push_back(errorString);
      }

      if (mCanopyHeight < std::numeric_limits<double>::epsilon())
      {
         QString errorString(" requires a canopy height (mCanopyHeight is zero)");
         errorString = nameString + errorString;
         errorList.push_back(errorString);
      }

      if (mCanopyWidth < std::numeric_limits<double>::epsilon())
      {
         QString errorString(" requires a canopy width (mCanopyWidth is zero)");
         errorString = nameString + errorString;
         errorList.push_back(errorString);
      }
   }

   return errorList;
}

} // namespace Designer
