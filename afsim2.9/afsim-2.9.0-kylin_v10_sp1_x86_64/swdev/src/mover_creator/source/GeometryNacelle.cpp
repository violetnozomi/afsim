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

#include "GeometryNacelle.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QTreeWidget>

#include "P6DofUtils.hpp"
#include "UtMath.hpp"

#include "MoverCreatorLineEdit.hpp"
namespace Designer
{

GeometryNacelle::GeometryNacelle(Vehicle* aVehicle)
   : GeometryBody(aVehicle)
{
   // Overwrite the GeometryBody type
   mGeometryObjectTypeString.assign("GeometryNacelle");
}

GeometryNacelle::~GeometryNacelle()
{
}

QTreeWidgetItem* GeometryNacelle::CreateTreeWidgetItem(QString& aName, QTreeWidget* aTreeWidgetPtr, bool aNoneditableName)
{
   // Create the tree widget item at the GeometryBody level
   QTreeWidgetItem* treeWidgetItem = GeometryObject::CreateTreeWidgetItem(aName, aTreeWidgetPtr, aNoneditableName);

   // Enforce the geometry type
   mGeometryObjectTypeString = "GeometryNacelle";

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
   lengthItem->setToolTip(0, "The length of the nacelle in feet");
   auto lengthLineEdit = new MoverCreatorLineEdit(lengthItem);
   lengthLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   lengthLineEdit->setFrame(false);
   lengthLineEdit->setText(QString::number(mLength_ft));
   aTreeWidgetPtr->setItemWidget(lengthItem, 1, lengthLineEdit);
   aTreeWidgetPtr->setItemWidget(lengthItem, 2, new QLabel(QString::fromStdString("ft")));
   connect(lengthLineEdit, &QLineEdit::textChanged, this, &GeometryNacelle::HandleLengthLineEditChanged);
   connect(lengthLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* heightItem = new QTreeWidgetItem(treeWidgetItem, QStringList("Height"));
   heightItem->setToolTip(0, "The height of the nacelle in feet");
   auto heightLineEdit = new MoverCreatorLineEdit(heightItem);
   heightLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   heightLineEdit->setFrame(false);
   heightLineEdit->setText(QString::number(mHeight_ft));
   aTreeWidgetPtr->setItemWidget(heightItem, 1, heightLineEdit);
   aTreeWidgetPtr->setItemWidget(heightItem, 2, new QLabel(QString::fromStdString("ft")));
   connect(heightLineEdit, &QLineEdit::textChanged, this, &GeometryNacelle::HandleHeightLineEditChanged);
   connect(heightLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mHeightItem = heightItem;

   QTreeWidgetItem* hgtInnerItem = new QTreeWidgetItem(treeWidgetItem, QStringList("Height (Inner)"));
   hgtInnerItem->setToolTip(0, "The inner height of the nacelle in feet");
   auto hgtInnerItemLineEdit = new MoverCreatorLineEdit(hgtInnerItem);
   hgtInnerItemLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   hgtInnerItemLineEdit->setFrame(false);
   hgtInnerItemLineEdit->setText(QString::number(mHeightInner_ft));
   aTreeWidgetPtr->setItemWidget(hgtInnerItem, 1, hgtInnerItemLineEdit);
   aTreeWidgetPtr->setItemWidget(hgtInnerItem, 2, new QLabel(QString::fromStdString("ft")));
   connect(hgtInnerItemLineEdit, &QLineEdit::textChanged, this, &GeometryNacelle::HandleHeightInnerLineEditChanged);
   connect(hgtInnerItemLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mHeightInnerItem = hgtInnerItem;
   mHeightInnerItem->setHidden(true); // This starts hidden

   QTreeWidgetItem* hgtOuterItem = new QTreeWidgetItem(treeWidgetItem, QStringList("Height (Outer)"));
   hgtOuterItem->setToolTip(0, "The outer height of the nacelle in feet");
   auto hgtOuterItemLineEdit = new MoverCreatorLineEdit(hgtOuterItem);
   hgtOuterItemLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   hgtOuterItemLineEdit->setFrame(false);
   hgtOuterItemLineEdit->setText(QString::number(mHeightOuter_ft));
   aTreeWidgetPtr->setItemWidget(hgtOuterItem, 1, hgtOuterItemLineEdit);
   aTreeWidgetPtr->setItemWidget(hgtOuterItem, 2, new QLabel(QString::fromStdString("ft")));
   connect(hgtOuterItemLineEdit, &QLineEdit::textChanged, this, &GeometryNacelle::HandleHeightOuterLineEditChanged);
   connect(hgtOuterItemLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mHeightOuterItem = hgtOuterItem;
   mHeightOuterItem->setHidden(true); // This starts hidden

   QTreeWidgetItem* widthItem = new QTreeWidgetItem(treeWidgetItem, QStringList("Width"));
   widthItem->setToolTip(0, "The width of the nacelle in feet");
   auto widthLineEdit = new MoverCreatorLineEdit(widthItem);
   widthLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   widthLineEdit->setFrame(false);
   widthLineEdit->setText(QString::number(mWidth_ft));
   aTreeWidgetPtr->setItemWidget(widthItem, 1, widthLineEdit);
   aTreeWidgetPtr->setItemWidget(widthItem, 2, new QLabel(QString::fromStdString("ft")));
   connect(widthLineEdit, &QLineEdit::textChanged, this, &GeometryNacelle::HandleWidthLineEditChanged);
   connect(widthLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* thicknessItem = new QTreeWidgetItem(treeWidgetItem, QStringList("Thickness"));
   thicknessItem->setToolTip(0, "The thickness of the nacelle in feet");
   auto thicknessItemLineEdit = new MoverCreatorLineEdit(thicknessItem);
   thicknessItemLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   thicknessItemLineEdit->setFrame(false);
   thicknessItemLineEdit->setText(QString::number(mThickness_ft));
   aTreeWidgetPtr->setItemWidget(thicknessItem, 1, thicknessItemLineEdit);
   aTreeWidgetPtr->setItemWidget(thicknessItem, 2, new QLabel(QString::fromStdString("ft")));
   connect(thicknessItemLineEdit, &QLineEdit::textChanged, this, &GeometryNacelle::HandleThicknessLineEditChanged);
   connect(thicknessItemLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* forwardSweepLength = new QTreeWidgetItem(treeWidgetItem, QStringList("Forward Sweep Length"));
   forwardSweepLength->setToolTip(0, "The length of the nacelle's forward sweep in feet");
   auto forwardSweepLengthLineEdit = new MoverCreatorLineEdit(forwardSweepLength);
   forwardSweepLengthLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   forwardSweepLengthLineEdit->setFrame(false);
   forwardSweepLengthLineEdit->setText(QString::number(mThickness_ft));
   aTreeWidgetPtr->setItemWidget(forwardSweepLength, 1, forwardSweepLengthLineEdit);
   aTreeWidgetPtr->setItemWidget(forwardSweepLength, 2, new QLabel(QString::fromStdString("ft")));
   connect(forwardSweepLengthLineEdit, &QLineEdit::textChanged, this, &GeometryNacelle::HandleForwardSweepLengthLineEditChanged);
   connect(forwardSweepLengthLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mForwardSweepLengthItem = forwardSweepLength;
   mForwardSweepLengthItem->setHidden(true); // This starts hidden

   QTreeWidgetItem* yaw = new QTreeWidgetItem(treeWidgetItem, QStringList("Yaw Angle"));
   yaw->setToolTip(0, "The yaw of the nacelle in degrees");
   auto yawLineEdit = new MoverCreatorLineEdit(yaw);
   yawLineEdit->setFrame(false);
   yawLineEdit->setText(QString::number(mYaw_rad * UtMath::cDEG_PER_RAD));
   aTreeWidgetPtr->setItemWidget(yaw, 1, yawLineEdit);
   aTreeWidgetPtr->setItemWidget(yaw, 2, new QLabel(QString::fromStdString("deg")));
   connect(yawLineEdit, &QLineEdit::textChanged, this, &GeometryNacelle::HandleYawLineEditChanged);
   connect(yawLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* pitch = new QTreeWidgetItem(treeWidgetItem, QStringList("Pitch Angle"));
   pitch->setToolTip(0, "The pitch of the nacelle in degrees");
   auto pitchLineEdit = new MoverCreatorLineEdit(pitch);
   pitchLineEdit->setFrame(false);
   pitchLineEdit->setText(QString::number(mPitch_rad * UtMath::cDEG_PER_RAD));
   aTreeWidgetPtr->setItemWidget(pitch, 1, pitchLineEdit);
   aTreeWidgetPtr->setItemWidget(pitch, 2, new QLabel(QString::fromStdString("deg")));
   connect(pitchLineEdit, &QLineEdit::textChanged, this, &GeometryNacelle::HandlePitchLineEditChanged);
   connect(pitchLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* roll = new QTreeWidgetItem(treeWidgetItem, QStringList("Roll Angle"));
   roll->setToolTip(0, "The roll of the nacelle in degrees");
   auto rollLineEdit = new MoverCreatorLineEdit(roll);
   rollLineEdit->setFrame(false);
   rollLineEdit->setText(QString::number(mRoll_rad * UtMath::cDEG_PER_RAD));
   aTreeWidgetPtr->setItemWidget(roll, 1, rollLineEdit);
   aTreeWidgetPtr->setItemWidget(roll, 2, new QLabel(QString::fromStdString("deg")));
   connect(rollLineEdit, &QLineEdit::textChanged, this, &GeometryNacelle::HandleRollLineEditChanged);
   connect(rollLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* forwardShape = new QTreeWidgetItem(treeWidgetItem, QStringList("Forward Shape"));
   forwardShape->setToolTip(0, "The forward shape of the nacelle, which can be ogive, conical, rounded, or blunt");
   QComboBox* forwardShapeComboBox = new QComboBox();
   QStringList forwardStrings;
   forwardStrings << "Ogive" << "Cone" << "Round" << "Blunt";
   forwardShapeComboBox->addItems(forwardStrings);
   aTreeWidgetPtr->setItemWidget(forwardShape, 1, forwardShapeComboBox);
   aTreeWidgetPtr->setItemWidget(forwardShape, 2, new QLabel(QString::fromStdString("")));
   mForwardShapeItem = forwardShape;
   mForwardShapeItem->setHidden(true); // This starts hidden
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
   connect(forwardShapeLengthLineEdit, &QLineEdit::textChanged, this, &GeometryNacelle::HandleForwardShapeLengthLineEditChanged);
   connect(forwardShapeLengthLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mForwardShapeLengthItem = forwardShapeLength;
   mForwardShapeLengthItem->setHidden(true); // This starts hidden

   QTreeWidgetItem* overallShape = new QTreeWidgetItem(treeWidgetItem, QStringList("Overall Shape"));
   overallShape->setToolTip(0, "The overall shape of the nacelle");
   QComboBox* overallShapeComboBox = new QComboBox();
   QStringList shapeStrings;
   shapeStrings << "Rounded" << "Half-Round-Right" << "Half-Round-Left" << "Half-Round-Top" << "Half-Round-Bottom"
                << "Flat-Sided" << "Flat-Swept-Right" << "Flat-Swept-Left";
   overallShapeComboBox->addItems(shapeStrings);
   aTreeWidgetPtr->setItemWidget(overallShape, 1, overallShapeComboBox);
   aTreeWidgetPtr->setItemWidget(overallShape, 2, new QLabel(QString::fromStdString("")));
   mOverallShapeItem = overallShape;
   connect(overallShapeComboBox, &QComboBox::currentTextChanged, this, [this, aTreeWidgetPtr, overallShape](QString aOverallShape)
   {
      aTreeWidgetPtr->setCurrentItem(overallShape);
      HandleOverallShapeComboBoxChanged(aOverallShape);
   });

   QTreeWidgetItem* aftSection = new QTreeWidgetItem(treeWidgetItem, QStringList("Aft Section Shape"));
   aftSection->setToolTip(0, "The shape of the aft section, which can be either blunt or tapered");
   QComboBox* aftSectionComboBox = new QComboBox();
   QStringList aftSectionStrings;
   aftSectionStrings << "Blunt" << "Tapered";
   aftSectionComboBox->addItems(aftSectionStrings);
   aTreeWidgetPtr->setItemWidget(aftSection, 1, aftSectionComboBox);
   aTreeWidgetPtr->setItemWidget(aftSection, 2, new QLabel(QString::fromStdString("")));
   mAftSectionShapeItem = aftSection;
   connect(aftSectionComboBox, &QComboBox::currentTextChanged, this, [this, aTreeWidgetPtr, aftSection](QString aAftSection)
   {
      aTreeWidgetPtr->setCurrentItem(aftSection);
      HandleAftSectionComboBoxChanged(aAftSection);
   });

   QTreeWidgetItem* aftSectionLength = new QTreeWidgetItem(treeWidgetItem, QStringList("Aft Section Length"));
   aftSectionLength->setToolTip(0, "The length of the aft section in feet");
   auto aftSectionLengthLineEdit = new MoverCreatorLineEdit(aftSectionLength);
   aftSectionLengthLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   aftSectionLengthLineEdit->setFrame(false);
   aftSectionLengthLineEdit->setText(QString::number(mThickness_ft));
   aTreeWidgetPtr->setItemWidget(aftSectionLength, 1, aftSectionLengthLineEdit);
   aTreeWidgetPtr->setItemWidget(aftSectionLength, 2, new QLabel(QString::fromStdString("ft")));
   connect(aftSectionLengthLineEdit, &QLineEdit::textChanged, this, &GeometryNacelle::HandleAftSectionLengthLineEditChanged);
   connect(aftSectionLengthLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mAftSectionLengthItem = aftSectionLength;
   mAftSectionLengthItem->setHidden(true); // This starts hidden

   QTreeWidgetItem* massFactor = new QTreeWidgetItem(treeWidgetItem, QStringList("Mass Factor"));
   massFactor->setToolTip(0, "The mass factor multiplier of the nacelle");
   auto massFactorLineEdit = new MoverCreatorLineEdit(massFactor);
   massFactorLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   massFactorLineEdit->setFrame(false);
   massFactorLineEdit->setText(QString::number(mMassFactor));
   aTreeWidgetPtr->setItemWidget(massFactor, 1, massFactorLineEdit);
   aTreeWidgetPtr->setItemWidget(massFactor, 2, new QLabel(QString::fromStdString("")));
   connect(massFactorLineEdit, &QLineEdit::textChanged, this, &GeometryNacelle::HandleMassFactorLineEditChanged);
   connect(massFactorLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* symmetrical = new QTreeWidgetItem(treeWidgetItem, QStringList("Symmetrical"));
   symmetrical->setToolTip(0, "If checked, the nacelle will use symmetry");
   QCheckBox* symmetricalCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(symmetrical, 1, symmetricalCheckBox);
   aTreeWidgetPtr->setItemWidget(symmetrical, 2, new QLabel(QString::fromStdString("")));
   connect(symmetricalCheckBox, &QCheckBox::stateChanged, this, &GeometryNacelle::HandleSymmetricalCheckBoxChanged);

   return treeWidgetItem;
}

void GeometryNacelle::HandleThicknessLineEditChanged(QString aValue)
{
   mThickness_ft = aValue.toDouble();
   AdjustAndRedraw(false);
}

void GeometryNacelle::HandleGuiElementDependencies()
{
   if ((mOverallShapeString == "Rounded") ||
       (mOverallShapeString == "Half-Round-Right") ||
       (mOverallShapeString == "Half-Round-Left") ||
       (mOverallShapeString == "Half-Round-Top") ||
       (mOverallShapeString == "Half-Round-Bottom"))
   {
      if (mHeightItem != nullptr) { mHeightItem->setHidden(false); }
      if (mHeightInnerItem != nullptr) { mHeightInnerItem->setHidden(true); }
      if (mHeightOuterItem != nullptr) { mHeightOuterItem->setHidden(true); }
      if (mForwardSweepLengthItem != nullptr) { mForwardSweepLengthItem->setHidden(true); }

      if (mAftSectionShapeItem != nullptr) { mAftSectionShapeItem->setHidden(false); }
      if (mAftSectionString == "Blunt") { if (mAftSectionLengthItem != nullptr) { mAftSectionLengthItem->setHidden(true); } }
      else                              { if (mAftSectionLengthItem != nullptr) { mAftSectionLengthItem->setHidden(false); } }
   }
   else if (mOverallShapeString == "Flat-Sided")
   {
      if (mHeightItem != nullptr) { mHeightItem->setHidden(false); }
      if (mHeightInnerItem != nullptr) { mHeightInnerItem->setHidden(true); }
      if (mHeightOuterItem != nullptr) { mHeightOuterItem->setHidden(true); }
      if (mForwardSweepLengthItem != nullptr) { mForwardSweepLengthItem->setHidden(true); }

      // Flat-Sided lacks an aft section
      if (mAftSectionShapeItem != nullptr) { mAftSectionShapeItem->setHidden(true); }
      if (mAftSectionLengthItem != nullptr) { mAftSectionLengthItem->setHidden(true); }
   }
   else // Flat-Swept-Left and Flat-Swept-Right
   {
      if (mHeightItem != nullptr) { mHeightItem->setHidden(true); }
      if (mHeightInnerItem != nullptr) { mHeightInnerItem->setHidden(false); }
      if (mHeightOuterItem != nullptr) { mHeightOuterItem->setHidden(false); }
      if (mForwardSweepLengthItem != nullptr) { mForwardSweepLengthItem->setHidden(false); }

      if (mAftSectionShapeItem != nullptr) { mAftSectionShapeItem->setHidden(false); }
      if (mAftSectionString == "Blunt") { if (mAftSectionLengthItem != nullptr) { mAftSectionLengthItem->setHidden(true); } }
      else                              { if (mAftSectionLengthItem != nullptr) { mAftSectionLengthItem->setHidden(false); } }
   }
}

void GeometryNacelle::HandleOverallShapeComboBoxChanged(QString aValue)
{
   mOverallShapeString = aValue;
   HandleGuiElementDependencies();
   AdjustAndRedraw(true);
}

void GeometryNacelle::HandleAftSectionComboBoxChanged(QString aValue)
{
   mAftSectionString = aValue;
   HandleGuiElementDependencies();
   AdjustAndRedraw(true);
}

void GeometryNacelle::HandleAftSectionLengthLineEditChanged(QString aValue)
{
   mAftSectionLength_ft = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryNacelle::HandleHeightInnerLineEditChanged(QString aValue)
{
   mHeightInner_ft = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryNacelle::HandleHeightOuterLineEditChanged(QString aValue)
{
   mHeightOuter_ft = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryNacelle::HandleForwardSweepLengthLineEditChanged(QString aValue)
{
   mForwardSweepLength_ft = aValue.toDouble();
   AdjustAndRedraw(true);
}

double GeometryNacelle::GetMassArea_ft2()
{
   double avgRadius = (mHeight_ft + mWidth_ft) * 0.25;
   double twoPi = UtMath::cTWO_PI;

   // Note: These are simple calculations and only approximate
   double area_ft2 = 0.0;
   if (mOverallShapeString == "Rounded")
   {
      area_ft2 = mLength_ft * (twoPi * avgRadius);
   }
   else if ((mOverallShapeString == "Half-Round-Right") ||
            (mOverallShapeString == "Half-Round-Left") ||
            (mOverallShapeString == "Half-Round-Top") ||
            (mOverallShapeString == "Half-Round-Bottom"))
   {
      area_ft2 = 0.5 * (mLength_ft * (twoPi * avgRadius));
   }
   else if ((mOverallShapeString == "Flat-Swept-Right") ||
            (mOverallShapeString == "Flat-Swept-Left"))
   {
      double height_ft = 0.5*(mHeightInner_ft + mHeightOuter_ft);
      area_ft2 = mLength_ft * height_ft * mWidth_ft;
   }
   else if (mOverallShapeString == "Flat-Sided")
   {
      area_ft2 = mLength_ft * mHeight_ft * mWidth_ft;
   }

   return area_ft2;
}

void GeometryNacelle::CalcMassProperties(double aTotalMass_lbs)
{
   // Set the mass
   mCalculatedMass_lbs = aTotalMass_lbs;

   // We assume a nacelle is roughly similar to a thin-walled cylinder
   // Ixx = mass * (radius*radius)
   // Iyy = (1/12) * mass * (length*length)
   // Izz = (1/12) * mass * (length*length)

   // A nacelle may use one of several types of overall shape: rounded, half-round,
   // flat-sided, and flat-swept. However, regardless of which overall shape is
   // specified, the mass properties will be based on a thin-walled cylinder.

   // A nacelle may have an aft section -- Blunt or Tapered. We assume that the tapered case
   // may be approximated by a hollow cone, with Ixx = (1/2) * mass * (radius*radius)
   // This results in a reduced Ixx for a tapered aft section as compared to the cylindrical
   // core section of the nacelle.

   // We assume that the aft shapes will not significantly effect the Iyy and Izz, and as a
   // result, we assume a thin-walled cylinder for the entire body for Iyy and Izz. While
   // not true, it is an assumption to simplify calculations.

   // Calc the mass (may be reduced due to symmetry)
   double mass_slugs = mCalculatedMass_lbs * mLbsToSlugs;
   if (mSymmetrical)
   {
      mass_slugs *= 0.5; // Each part is half of the total mass
   }

   // Regardless of whether or not we have symmetry, begin by calculating the base Ixx, Iyy, and Izz:

   double avgDiameter_ft = 0.5 * (mWidth_ft + mHeight_ft);
   double radius = avgDiameter_ft * 0.5;
   double radiusSquared = radius * radius;
   double lengthSquared = mLength_ft * mLength_ft;

   double calculatedIxx = 0.0;
   if (mAftSectionString == "Blunt")
   {
      // This is simply a full cylinder
      calculatedIxx = mass_slugs * radiusSquared;
   }
   else
   {
      // This has a tapered aft section
      double aftFraction = mAftSectionLength_ft / mLength_ft;
      double aftIxx = aftFraction * 0.5 * mass_slugs * radiusSquared;
      double mainIxx = (1.0 - aftFraction) * mass_slugs * radiusSquared;
      calculatedIxx = mainIxx + aftIxx;
   }

   double calculatedIyy = (1.0 / 12.0) * mass_slugs * lengthSquared;
   double calculatedIzz = calculatedIyy;

   // Now we deal with either a single nacelle or two symmetrical nacelles
   if (!mSymmetrical)
   {
      // This is a single nacelle

      // Correct for location, using parallel axis theorem
      double distX = sqrt(mRefPoint_ft.Y() * mRefPoint_ft.Y() + mRefPoint_ft.Z() * mRefPoint_ft.Z());
      double distY = sqrt(mRefPoint_ft.X() * mRefPoint_ft.X() + mRefPoint_ft.Z() * mRefPoint_ft.Z());
      double distZ = sqrt(mRefPoint_ft.X() * mRefPoint_ft.X() + mRefPoint_ft.Y() * mRefPoint_ft.Y());
      double dX2 = distX * distX;
      double dY2 = distY * distY;
      double dZ2 = distZ * distZ;

      mCalculatedIxx = calculatedIxx + (mass_slugs * dX2);
      mCalculatedIyy = calculatedIyy + (mass_slugs * dY2);
      mCalculatedIzz = calculatedIzz + (mass_slugs * dZ2);

      // Calculate the location of the cg -- we assume it is always at the reference point
      mCalculatedCG_ft = mRefPoint_ft;
   }
   else
   {
      // This is two symmetrical nacelles

      // First, we deal with the primary nacelle and correct for location, using parallel axis theorem (PAT)
      double distX = sqrt(mRefPoint_ft.Y() * mRefPoint_ft.Y() + mRefPoint_ft.Z() * mRefPoint_ft.Z());
      double distY = sqrt(mRefPoint_ft.X() * mRefPoint_ft.X() + mRefPoint_ft.Z() * mRefPoint_ft.Z());
      double distZ = sqrt(mRefPoint_ft.X() * mRefPoint_ft.X() + mRefPoint_ft.Y() * mRefPoint_ft.Y());
      double dX2 = distX * distX;
      double dY2 = distY * distY;
      double dZ2 = distZ * distZ;

      // Calc the total inertia (including PAT)
      double primaryNacelleIxx = calculatedIxx + (mass_slugs * dX2);
      double primaryNacelleIyy = calculatedIyy + (mass_slugs * dY2);
      double primaryNacelleIzz = calculatedIzz + (mass_slugs * dZ2);

      // Now, determine location of other (secondary) nacelle
      UtVec3dX refPt = mRefPoint_ft;

      double currentBodyY = mRefPoint_ft.Y();
      double delta = currentBodyY - mSymmetryHorizontalY;
      double otherBodyY = currentBodyY - 2.0 * delta;

      refPt.Set(refPt.X(), otherBodyY, refPt.Z());

      // Correct for location, using parallel axis theorem
      distX = sqrt(refPt.Y() * refPt.Y() + refPt.Z() * refPt.Z());
      distY = sqrt(refPt.X() * refPt.X() + refPt.Z() * refPt.Z());
      distZ = sqrt(refPt.X() * refPt.X() + refPt.Y() * refPt.Y());
      dX2 = distX * distX;
      dY2 = distY * distY;
      dZ2 = distZ * distZ;

      // Calc the secondary nacelle inertia
      double secondaryNacelleIxx = calculatedIxx + (mass_slugs * dX2);
      double secondaryNacelleIyy = calculatedIyy + (mass_slugs * dY2);
      double secondaryNacelleIzz = calculatedIzz + (mass_slugs * dZ2);

      // Calc the total inertia
      mCalculatedIxx = primaryNacelleIxx + secondaryNacelleIxx;
      mCalculatedIyy = primaryNacelleIyy + secondaryNacelleIyy;
      mCalculatedIzz = primaryNacelleIzz + secondaryNacelleIzz;

      // Calculate the location of the cg -- we assume it is always at the reference point,
      // but centered in the Y-axis "between" the two objects
      double centerY = (currentBodyY + otherBodyY) * 0.5;
      mCalculatedCG_ft.Set(mRefPoint_ft.X(), centerY, mRefPoint_ft.Z());
   }
}

QStringList GeometryNacelle::Audit()
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

   if (mWidth_ft < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a width (mWidth_ft is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   if (mThickness_ft < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a thickness (mThickness_ft is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   if ((mOverallShapeString == "Rounded") ||
       (mOverallShapeString == "Half-Round-Right") ||
       (mOverallShapeString == "Half-Round-Left") ||
       (mOverallShapeString == "Half-Round-Top") ||
       (mOverallShapeString == "Half-Round-Bottom") ||
       (mOverallShapeString == "Flat-Sided"))
   {
      if (mHeight_ft < std::numeric_limits<double>::epsilon())
      {
         QString errorString(" requires a height (mHeight_ft is zero)");
         errorString = nameString + errorString;
         errorList.push_back(errorString);
      }
   }
   else if ((mOverallShapeString == "Flat-Swept-Right") ||
            (mOverallShapeString == "Flat-Swept-Left"))
   {
      if (mHeightInner_ft < std::numeric_limits<double>::epsilon())
      {
         QString errorString(" requires an inner height (mHeightInner_ft is zero)");
         errorString = nameString + errorString;
         errorList.push_back(errorString);
      }

      if (mHeightOuter_ft < std::numeric_limits<double>::epsilon())
      {
         QString errorString(" requires an outer height (mHeightOuter_ft is zero)");
         errorString = nameString + errorString;
         errorList.push_back(errorString);
      }

      if (mForwardSweepLength_ft < std::numeric_limits<double>::epsilon())
      {
         QString errorString(" requires a forward sweep length (mForwardSweepLength_ft is zero)");
         errorString = nameString + errorString;
         errorList.push_back(errorString);
      }
   }

   if (mAftSectionString != "Blunt")
   {
      if (mAftSectionLength_ft < std::numeric_limits<double>::epsilon())
      {
         QString errorString(" requires a aft section length (mAftSectionLength_ft is zero)");
         errorString = nameString + errorString;
         errorList.push_back(errorString);
      }
   }

   return errorList;
}

} // namespace Designer
