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

#include "GeometryWing.hpp"

#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QTreeWidget>

#include "MoverCreatorLineEdit.hpp"
#include "P6DofUtils.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"

namespace Designer
{

GeometryWing::GeometryWing(Vehicle* aVehicle)
   : GeometrySurface(aVehicle)
{
   // Overwrite the GeometrySurface type
   mGeometryObjectTypeString.assign("GeometryWing");
}

GeometryWing::~GeometryWing()
{
}

double GeometryWing::GetAspectRatio()
{
   if (mUseManualAspectRatio)
   {
      return mManualAspectRatio;
   }

   // Note: This can be improved/changed at a later type by using fowler flaps
   return mAspectRatio;
}

double GeometryWing::GetPlanformArea_ft2()
{
   // Note: This can be improved/changed at a later type by using fowler flaps
   return mArea_ft2;
}

std::vector<GeometrySurface::GeometryPanel> GeometryWing::GetPanels()
{
   std::vector<GeometrySurface::GeometryPanel> controlSurfaces;
   if (mAileronsPresent)
   {
      GeometryPanel ailerons;
      ailerons.mChordFractionStart = mAileronsChordFractionStart;
      ailerons.mChordFractionEnd   = mAileronsChordFractionEnd;
      ailerons.mSpanFractionStart  = mAileronsSpanFractionStart;
      ailerons.mSpanFractionEnd    = mAileronsSpanFractionEnd;
      controlSurfaces.push_back(ailerons);
   }
   if (mDrageronsPresent)
   {
      GeometryPanel dragerons;
      dragerons.mChordFractionStart = mDrageronsChordFractionStart;
      dragerons.mChordFractionEnd   = mDrageronsChordFractionEnd;
      dragerons.mSpanFractionStart  = mDrageronsSpanFractionStart;
      dragerons.mSpanFractionEnd    = mDrageronsSpanFractionEnd;
      controlSurfaces.push_back(dragerons);
   }
   if (mSpoileronsPresent)
   {
      GeometryPanel spoilerons;
      spoilerons.mChordFractionStart = mSpoileronsChordFractionStart;
      spoilerons.mChordFractionEnd = mSpoileronsChordFractionEnd;
      spoilerons.mSpanFractionStart = mSpoileronsSpanFractionStart;
      spoilerons.mSpanFractionEnd = mSpoileronsSpanFractionEnd;
      controlSurfaces.push_back(spoilerons);
   }
   if (mSpoilersPresent)
   {
      GeometryPanel spoilers;
      spoilers.mChordFractionStart = mSpoilersChordFractionStart;
      spoilers.mChordFractionEnd   = mSpoilersChordFractionEnd;
      spoilers.mSpanFractionStart  = mSpoilersSpanFractionStart;
      spoilers.mSpanFractionEnd    = mSpoilersSpanFractionEnd;
      controlSurfaces.push_back(spoilers);
   }
   if (mElevonsPresent)
   {
      GeometryPanel elevons;
      elevons.mChordFractionStart = mElevonsChordFractionStart;
      elevons.mChordFractionEnd   = mElevonsChordFractionEnd;
      elevons.mSpanFractionStart  = mElevonsSpanFractionStart;
      elevons.mSpanFractionEnd    = mElevonsSpanFractionEnd;
      controlSurfaces.push_back(elevons);
   }
   return controlSurfaces;
}

void GeometryWing::CalcAspectRatioAndArea()
{
   double effectiveSpan = mSpan_ft;
   double averageChord = 0.5 * (mRootChord_ft + mTipChord_ft);
   double area = effectiveSpan * averageChord;

   mArea_ft2 = area;

   if (area <= 0)
   {
      mAspectRatio = 0.0;
      mCalculatedAspectRatioLineEditPtr->setText(QString::number(mAspectRatio));
      mCalculatedAreaLineEditPtr->setText(QString::number(mArea_ft2));
      return;
   }

   mAspectRatio = (effectiveSpan * effectiveSpan) / area;
   mCalculatedAspectRatioLineEditPtr->setText(QString::number(mAspectRatio));
   mCalculatedAreaLineEditPtr->setText(QString::number(mArea_ft2));
}

QTreeWidgetItem* GeometryWing::CreateTreeWidgetItem(QString& aName, QTreeWidget* aTreeWidgetPtr, bool aNoneditableName)
{
   // Create the tree widget item at the GeometryObject level
   QTreeWidgetItem* treeWidgetItem = GeometrySurface::CreateTreeWidgetItem(aName, aTreeWidgetPtr, false);

   // Enforce the geometry type
   mGeometryObjectTypeString = "GeometryWing";

   // Set Qt flags
   if (aNoneditableName)
   {
      treeWidgetItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
   }
   else
   {
      treeWidgetItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
   }


   // Ailerons section

   QTreeWidgetItem* AileronsPresent = new QTreeWidgetItem(treeWidgetItem, QStringList("Ailerons Present"));
   AileronsPresent->setToolTip(0, "If checked, ailerons will be present on the wing");
   QCheckBox* AileronsPresentCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(AileronsPresent, 1, AileronsPresentCheckBox);
   aTreeWidgetPtr->setItemWidget(AileronsPresent, 2, new QLabel(QString::fromStdString("")));
   connect(AileronsPresentCheckBox, &QCheckBox::stateChanged, this, &GeometryWing::HandleAileronsPresentCheckBoxChanged);
   mAileronsPresentCheckBoxChangedWidget = AileronsPresent;
   
   QTreeWidgetItem* aileronsChordFractionStart = new QTreeWidgetItem(treeWidgetItem, QStringList("Ailerons Chord Fraction, Start"));
   aileronsChordFractionStart->setToolTip(0, "This is the start location of the ailerons, from the leading edge");
   auto aileronsChordFractionStartLineEdit = new MoverCreatorLineEdit(aileronsChordFractionStart);
   aileronsChordFractionStartLineEdit->setFrame(false);
   aileronsChordFractionStartLineEdit->setText(QString::number(mAileronsChordFractionStart));
   aTreeWidgetPtr->setItemWidget(aileronsChordFractionStart, 1, aileronsChordFractionStartLineEdit);
   aTreeWidgetPtr->setItemWidget(aileronsChordFractionStart, 2, new QLabel(QString::fromStdString("")));
   connect(aileronsChordFractionStartLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleAileronsChordFractionStartLineEditChanged);
   connect(aileronsChordFractionStartLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mAileronsChordFractionStartWidget = aileronsChordFractionStart;
   mAileronsChordFractionStartWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* aileronsChordFractionEnd = new QTreeWidgetItem(treeWidgetItem, QStringList("Ailerons Chord Fraction, End"));
   aileronsChordFractionEnd->setToolTip(0, "This is the end location of the ailerons, from the leading edge");
   auto aileronsChordFractionEndLineEdit = new MoverCreatorLineEdit(aileronsChordFractionEnd);
   aileronsChordFractionEndLineEdit->setFrame(false);
   aileronsChordFractionEndLineEdit->setText(QString::number(mAileronsChordFractionEnd));
   aTreeWidgetPtr->setItemWidget(aileronsChordFractionEnd, 1, aileronsChordFractionEndLineEdit);
   aTreeWidgetPtr->setItemWidget(aileronsChordFractionEnd, 2, new QLabel(QString::fromStdString("")));
   connect(aileronsChordFractionEndLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleAileronsChordFractionEndLineEditChanged);
   connect(aileronsChordFractionEndLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mAileronsChordFractionEndWidget = aileronsChordFractionEnd;
   mAileronsChordFractionEndWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* aileronsSpanFractionStart = new QTreeWidgetItem(treeWidgetItem, QStringList("Ailerons Span Fraction, Start"));
   aileronsSpanFractionStart->setToolTip(0, "This is the start location of the ailerons, from the root");
   auto aileronsSpanFractionStartLineEdit = new MoverCreatorLineEdit(aileronsSpanFractionStart);
   aileronsSpanFractionStartLineEdit->setFrame(false);
   aileronsSpanFractionStartLineEdit->setText(QString::number(mAileronsSpanFractionStart));
   aTreeWidgetPtr->setItemWidget(aileronsSpanFractionStart, 1, aileronsSpanFractionStartLineEdit);
   aTreeWidgetPtr->setItemWidget(aileronsSpanFractionStart, 2, new QLabel(QString::fromStdString("")));
   connect(aileronsSpanFractionStartLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleAileronsSpanFractionStartLineEditChanged);
   connect(aileronsSpanFractionStartLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mAileronsSpanFractionStartWidget = aileronsSpanFractionStart;
   mAileronsSpanFractionStartWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* aileronsSpanFractionEnd = new QTreeWidgetItem(treeWidgetItem, QStringList("Ailerons Span Fraction, End"));
   aileronsSpanFractionEnd->setToolTip(0, "This is the end location of the ailerons, from the root");
   auto aileronsSpanFractionEndLineEdit = new MoverCreatorLineEdit(aileronsSpanFractionEnd);
   aileronsSpanFractionEndLineEdit->setFrame(false);
   aileronsSpanFractionEndLineEdit->setText(QString::number(mAileronsSpanFractionEnd));
   aTreeWidgetPtr->setItemWidget(aileronsSpanFractionEnd, 1, aileronsSpanFractionEndLineEdit);
   aTreeWidgetPtr->setItemWidget(aileronsSpanFractionEnd, 2, new QLabel(QString::fromStdString("")));
   connect(aileronsSpanFractionEndLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleAileronsSpanFractionEndLineEditChanged);
   connect(aileronsSpanFractionEndLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mAileronsSpanFractionEndWidget = aileronsSpanFractionEnd;
   mAileronsSpanFractionEndWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* AileronsUseExponentialAngleMapping = new QTreeWidgetItem(treeWidgetItem, QStringList("Ailerons Expo Angles"));
   AileronsUseExponentialAngleMapping->setToolTip(0, "If checked, the ailerons will use exponential angle mapping");
   QCheckBox* AileronsUseExponentialAngleMappingCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(AileronsUseExponentialAngleMapping, 1, AileronsUseExponentialAngleMappingCheckBox);
   aTreeWidgetPtr->setItemWidget(AileronsUseExponentialAngleMapping, 2, new QLabel(QString::fromStdString("")));
   connect(AileronsUseExponentialAngleMappingCheckBox, &QCheckBox::stateChanged, this, &GeometryWing::HandleAileronsUseExponentialAngleMappingCheckBoxChanged);
   AileronsUseExponentialAngleMapping->setHidden(true);
   mAileronsUseExponentialAngleMappingCheckBoxChangedWidget = AileronsUseExponentialAngleMapping;

   QTreeWidgetItem* AileronsControlSurfaceAngleMin = new QTreeWidgetItem(treeWidgetItem, QStringList("Ailerons Surface Ang Min"));
   AileronsControlSurfaceAngleMin->setToolTip(0, "The minimum control surface angle of the ailerons in degrees");
   auto AileronsControlSurfaceAngleMinLineEdit = new MoverCreatorLineEdit(AileronsControlSurfaceAngleMin);
   AileronsControlSurfaceAngleMinLineEdit->setFrame(false);
   AileronsControlSurfaceAngleMinLineEdit->setText(QString::number(mAileronsControlSurfaceMinAngle_deg));
   aTreeWidgetPtr->setItemWidget(AileronsControlSurfaceAngleMin, 1, AileronsControlSurfaceAngleMinLineEdit);
   aTreeWidgetPtr->setItemWidget(AileronsControlSurfaceAngleMin, 2, new QLabel(QString::fromStdString("deg")));
   connect(AileronsControlSurfaceAngleMinLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleAileronsControlSurfaceMinAngleLineEditChanged);
   connect(AileronsControlSurfaceAngleMinLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   AileronsControlSurfaceAngleMin->setHidden(true);
   mAileronsControlSurfaceMinAngleLineEditChangedWidget = AileronsControlSurfaceAngleMin;

   QTreeWidgetItem* AileronsControlSurfaceAngleMax = new QTreeWidgetItem(treeWidgetItem, QStringList("Ailerons Surface Ang Max"));
   AileronsControlSurfaceAngleMax->setToolTip(0, "The maximum control surface angle of the ailerons in degrees");
   auto AileronsControlSurfaceAngleMaxLineEdit = new MoverCreatorLineEdit(AileronsControlSurfaceAngleMax);
   AileronsControlSurfaceAngleMaxLineEdit->setFrame(false);
   AileronsControlSurfaceAngleMaxLineEdit->setText(QString::number(mAileronsControlSurfaceMaxAngle_deg));
   aTreeWidgetPtr->setItemWidget(AileronsControlSurfaceAngleMax, 1, AileronsControlSurfaceAngleMaxLineEdit);
   aTreeWidgetPtr->setItemWidget(AileronsControlSurfaceAngleMax, 2, new QLabel(QString::fromStdString("deg")));
   connect(AileronsControlSurfaceAngleMaxLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleAileronsControlSurfaceMaxAngleLineEditChanged);
   connect(AileronsControlSurfaceAngleMaxLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   AileronsControlSurfaceAngleMax->setHidden(true);
   mAileronsControlSurfaceMaxAngleLineEditChangedWidget = AileronsControlSurfaceAngleMax;

   QTreeWidgetItem* AileronsActuatorAngleMin = new QTreeWidgetItem(treeWidgetItem, QStringList("Ailerons Actuator Ang Min"));
   AileronsActuatorAngleMin->setToolTip(0, "The minimum angle of the aileron actuator in degrees");
   auto AileronsActuatorAngleMinLineEdit = new MoverCreatorLineEdit(AileronsActuatorAngleMin);
   AileronsActuatorAngleMinLineEdit->setFrame(false);
   AileronsActuatorAngleMinLineEdit->setText(QString::number(mAileronsActuatorMinAngle_deg));
   aTreeWidgetPtr->setItemWidget(AileronsActuatorAngleMin, 1, AileronsActuatorAngleMinLineEdit);
   aTreeWidgetPtr->setItemWidget(AileronsActuatorAngleMin, 2, new QLabel(QString::fromStdString("deg")));
   connect(AileronsActuatorAngleMinLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleAileronsActuatorMinAngleLineEditChanged);
   connect(AileronsActuatorAngleMinLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   AileronsActuatorAngleMin->setHidden(true);
   mAileronsActuatorMinAngleLineEditChangedWidget = AileronsActuatorAngleMin;

   QTreeWidgetItem* AileronsActuatorAngleMax = new QTreeWidgetItem(treeWidgetItem, QStringList("Ailerons Actuator Ang Max"));
   AileronsActuatorAngleMax->setToolTip(0, "The maximum angle of the aileron actuator in degrees");
   auto AileronsActuatorAngleMaxLineEdit = new MoverCreatorLineEdit(AileronsActuatorAngleMax);
   AileronsActuatorAngleMaxLineEdit->setFrame(false);
   AileronsActuatorAngleMaxLineEdit->setText(QString::number(mAileronsActuatorMaxAngle_deg));
   aTreeWidgetPtr->setItemWidget(AileronsActuatorAngleMax, 1, AileronsActuatorAngleMaxLineEdit);
   aTreeWidgetPtr->setItemWidget(AileronsActuatorAngleMax, 2, new QLabel(QString::fromStdString("deg")));
   connect(AileronsActuatorAngleMaxLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleAileronsActuatorMaxAngleLineEditChanged);
   connect(AileronsActuatorAngleMaxLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   AileronsActuatorAngleMax->setHidden(true);
   mAileronsActuatorMaxAngleLineEditChangedWidget = AileronsActuatorAngleMax;

   QTreeWidgetItem* AileronsActuatorRateMin = new QTreeWidgetItem(treeWidgetItem, QStringList("Ailerons Actuator Rate Min"));
   AileronsActuatorRateMin->setToolTip(0, "The minimum rate of the aileron actuator in degrees/second");
   auto AileronsActuatorRateMinLineEdit = new MoverCreatorLineEdit(AileronsActuatorRateMin);
   AileronsActuatorRateMinLineEdit->setFrame(false);
   AileronsActuatorRateMinLineEdit->setText(QString::number(mAileronsActuatorMinRate_dps));
   aTreeWidgetPtr->setItemWidget(AileronsActuatorRateMin, 1, AileronsActuatorRateMinLineEdit);
   aTreeWidgetPtr->setItemWidget(AileronsActuatorRateMin, 2, new QLabel(QString::fromStdString("deg/sec")));
   connect(AileronsActuatorRateMinLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleAileronsActuatorMinRateLineEditChanged);
   connect(AileronsActuatorRateMinLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   AileronsActuatorRateMin->setHidden(true);
   mAileronsActuatorMinRateLineEditChangedWidget = AileronsActuatorRateMin;

   QTreeWidgetItem* AileronsActuatorRateMax = new QTreeWidgetItem(treeWidgetItem, QStringList("Ailerons Actuator Rate Max"));
   AileronsActuatorRateMax->setToolTip(0, "The maximum rate of the aileron actuator in degrees/second");
   auto AileronsActuatorRateMaxLineEdit = new MoverCreatorLineEdit(AileronsActuatorRateMax);
   AileronsActuatorRateMaxLineEdit->setFrame(false);
   AileronsActuatorRateMaxLineEdit->setText(QString::number(mAileronsActuatorMaxRate_dps));
   aTreeWidgetPtr->setItemWidget(AileronsActuatorRateMax, 1, AileronsActuatorRateMaxLineEdit);
   aTreeWidgetPtr->setItemWidget(AileronsActuatorRateMax, 2, new QLabel(QString::fromStdString("deg/sec")));
   connect(AileronsActuatorRateMaxLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleAileronsActuatorMaxRateLineEditChanged);
   connect(AileronsActuatorRateMaxLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   AileronsActuatorRateMax->setHidden(true);
   mAileronsActuatorMaxRateLineEditChangedWidget = AileronsActuatorRateMax;

   // Drageron section

   QTreeWidgetItem* DrageronsPresent = new QTreeWidgetItem(treeWidgetItem, QStringList("Dragerons Present"));
   DrageronsPresent->setToolTip(0, "If checked, dragerons will be present on the wing");
   QCheckBox* DrageronsPresentCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(DrageronsPresent, 1, DrageronsPresentCheckBox);
   aTreeWidgetPtr->setItemWidget(DrageronsPresent, 2, new QLabel(QString::fromStdString("")));
   connect(DrageronsPresentCheckBox, &QCheckBox::stateChanged, this, &GeometryWing::HandleDrageronsPresentCheckBoxChanged);
   mDrageronsPresentCheckBoxChangedWidget = DrageronsPresent;

   QTreeWidgetItem* drageronsChordFractionStart = new QTreeWidgetItem(treeWidgetItem, QStringList("Dragerons Chord Fraction, Start"));
   drageronsChordFractionStart->setToolTip(0, "This is the start location of the dragerons, from the leading edge");
   auto drageronsChordFractionStartLineEdit = new MoverCreatorLineEdit(drageronsChordFractionStart);
   drageronsChordFractionStartLineEdit->setFrame(false);
   drageronsChordFractionStartLineEdit->setText(QString::number(mDrageronsChordFractionStart));
   aTreeWidgetPtr->setItemWidget(drageronsChordFractionStart, 1, drageronsChordFractionStartLineEdit);
   aTreeWidgetPtr->setItemWidget(drageronsChordFractionStart, 2, new QLabel(QString::fromStdString("")));
   connect(drageronsChordFractionStartLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleDrageronsChordFractionStartLineEditChanged);
   connect(drageronsChordFractionStartLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mDrageronsChordFractionStartWidget = drageronsChordFractionStart;
   mDrageronsChordFractionStartWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* drageronsChordFractionEnd = new QTreeWidgetItem(treeWidgetItem, QStringList("Dragerons Chord Fraction, End"));
   drageronsChordFractionEnd->setToolTip(0, "This is the end location of the dragerons, from the leading edge");
   auto drageronsChordFractionEndLineEdit = new MoverCreatorLineEdit(drageronsChordFractionEnd);
   drageronsChordFractionEndLineEdit->setFrame(false);
   drageronsChordFractionEndLineEdit->setText(QString::number(mDrageronsChordFractionStart));
   aTreeWidgetPtr->setItemWidget(drageronsChordFractionEnd, 1, drageronsChordFractionEndLineEdit);
   aTreeWidgetPtr->setItemWidget(drageronsChordFractionEnd, 2, new QLabel(QString::fromStdString("")));
   connect(drageronsChordFractionEndLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleDrageronsChordFractionEndLineEditChanged);
   connect(drageronsChordFractionEndLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mDrageronsChordFractionEndWidget = drageronsChordFractionEnd;
   mDrageronsChordFractionEndWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* drageronsSpanFractionStart = new QTreeWidgetItem(treeWidgetItem, QStringList("Dragerons Span Fraction, Start"));
   drageronsSpanFractionStart->setToolTip(0, "This is the start location of the dragerons, from the root");
   auto drageronsSpanFractionStartLineEdit = new MoverCreatorLineEdit(drageronsSpanFractionStart);
   drageronsSpanFractionStartLineEdit->setFrame(false);
   drageronsSpanFractionStartLineEdit->setText(QString::number(mDrageronsSpanFractionStart));
   aTreeWidgetPtr->setItemWidget(drageronsSpanFractionStart, 1, drageronsSpanFractionStartLineEdit);
   aTreeWidgetPtr->setItemWidget(drageronsSpanFractionStart, 2, new QLabel(QString::fromStdString("")));
   connect(drageronsSpanFractionStartLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleDrageronsSpanFractionStartLineEditChanged);
   connect(drageronsSpanFractionStartLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mDrageronsSpanFractionStartWidget = drageronsSpanFractionStart;
   mDrageronsSpanFractionStartWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* drageronsSpanFractionEnd = new QTreeWidgetItem(treeWidgetItem, QStringList("Dragerons Span Fraction, End"));
   drageronsSpanFractionEnd->setToolTip(0, "This is the end location of the dragerons, from the root");
   auto drageronsSpanFractionEndLineEdit = new MoverCreatorLineEdit(drageronsSpanFractionEnd);
   drageronsSpanFractionEndLineEdit->setFrame(false);
   drageronsSpanFractionEndLineEdit->setText(QString::number(mDrageronsSpanFractionStart));
   aTreeWidgetPtr->setItemWidget(drageronsSpanFractionEnd, 1, drageronsSpanFractionEndLineEdit);
   aTreeWidgetPtr->setItemWidget(drageronsSpanFractionEnd, 2, new QLabel(QString::fromStdString("")));
   connect(drageronsSpanFractionEndLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleDrageronsSpanFractionEndLineEditChanged);
   connect(drageronsSpanFractionEndLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mDrageronsSpanFractionEndWidget = drageronsSpanFractionEnd;
   mDrageronsSpanFractionEndWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* drageronsUseExponentialAngleMapping = new QTreeWidgetItem(treeWidgetItem, QStringList("Dragerons Expo Angles"));
   drageronsUseExponentialAngleMapping->setToolTip(0, "If checked, the dragerons will use exponential angle mapping");
   QCheckBox* drageronsUseExponentialAngleMappingCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(drageronsUseExponentialAngleMapping, 1, drageronsUseExponentialAngleMappingCheckBox);
   aTreeWidgetPtr->setItemWidget(drageronsUseExponentialAngleMapping, 2, new QLabel(QString::fromStdString("")));
   connect(drageronsUseExponentialAngleMappingCheckBox, &QCheckBox::stateChanged, this, &GeometryWing::HandleDrageronsUseExponentialAngleMappingCheckBoxChanged);
   drageronsUseExponentialAngleMapping->setHidden(true);
   mDrageronsUseExponentialAngleMappingCheckBoxChangedWidget = drageronsUseExponentialAngleMapping;

   QTreeWidgetItem* drageronsControlSurfaceAngleMin = new QTreeWidgetItem(treeWidgetItem, QStringList("Dragerons Surface Ang Min"));
   drageronsControlSurfaceAngleMin->setToolTip(0, "The minimum control surface angle of the dragerons in degrees");
   auto drageronsControlSurfaceAngleMinLineEdit = new MoverCreatorLineEdit(drageronsControlSurfaceAngleMin);
   drageronsControlSurfaceAngleMinLineEdit->setFrame(false);
   drageronsControlSurfaceAngleMinLineEdit->setText(QString::number(mDrageronsControlSurfaceMinAngle_deg));
   aTreeWidgetPtr->setItemWidget(drageronsControlSurfaceAngleMin, 1, drageronsControlSurfaceAngleMinLineEdit);
   aTreeWidgetPtr->setItemWidget(drageronsControlSurfaceAngleMin, 2, new QLabel(QString::fromStdString("deg")));
   connect(drageronsControlSurfaceAngleMinLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleDrageronsControlSurfaceMinAngleLineEditChanged);
   connect(drageronsControlSurfaceAngleMinLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   drageronsControlSurfaceAngleMin->setHidden(true);
   mDrageronsControlSurfaceMinAngleLineEditChangedWidget = drageronsControlSurfaceAngleMin;

   QTreeWidgetItem* drageronsControlSurfaceAngleMax = new QTreeWidgetItem(treeWidgetItem, QStringList("Dragerons Surface Ang Max"));
   drageronsControlSurfaceAngleMax->setToolTip(0, "The maximum control surface angle of the dragerons in degrees");
   auto drageronsControlSurfaceAngleMaxLineEdit = new MoverCreatorLineEdit(drageronsControlSurfaceAngleMax);
   drageronsControlSurfaceAngleMaxLineEdit->setFrame(false);
   mDrageronsControlSurfaceMaxAngle_deg = 0.0;
   drageronsControlSurfaceAngleMaxLineEdit->setText(QString::number(mDrageronsControlSurfaceMaxAngle_deg));
   aTreeWidgetPtr->setItemWidget(drageronsControlSurfaceAngleMax, 1, drageronsControlSurfaceAngleMaxLineEdit);
   aTreeWidgetPtr->setItemWidget(drageronsControlSurfaceAngleMax, 2, new QLabel(QString::fromStdString("deg")));
   connect(drageronsControlSurfaceAngleMaxLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleDrageronsControlSurfaceMaxAngleLineEditChanged);
   connect(drageronsControlSurfaceAngleMaxLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   drageronsControlSurfaceAngleMax->setHidden(true);
   mDrageronsControlSurfaceMaxAngleLineEditChangedWidget = drageronsControlSurfaceAngleMax;

   QTreeWidgetItem* drageronsActuatorAngleMin = new QTreeWidgetItem(treeWidgetItem, QStringList("Dragerons Actuator Ang Min"));
   drageronsActuatorAngleMin->setToolTip(0, "The minimum angle of the dragerons actuator in degrees");
   auto drageronsActuatorAngleMinLineEdit = new MoverCreatorLineEdit(drageronsActuatorAngleMin);
   drageronsActuatorAngleMinLineEdit->setFrame(false);
   drageronsActuatorAngleMinLineEdit->setText(QString::number(mDrageronsActuatorMinAngle_deg));
   aTreeWidgetPtr->setItemWidget(drageronsActuatorAngleMin, 1, drageronsActuatorAngleMinLineEdit);
   aTreeWidgetPtr->setItemWidget(drageronsActuatorAngleMin, 2, new QLabel(QString::fromStdString("deg")));
   connect(drageronsActuatorAngleMinLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleDrageronsActuatorMinAngleLineEditChanged);
   connect(drageronsActuatorAngleMinLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   drageronsActuatorAngleMin->setHidden(true);
   mDrageronsActuatorMinAngleLineEditChangedWidget = drageronsActuatorAngleMin;

   QTreeWidgetItem* drageronsActuatorAngleMax = new QTreeWidgetItem(treeWidgetItem, QStringList("Dragerons Actuator Ang Max"));
   drageronsActuatorAngleMax->setToolTip(0, "The maximum angle of the dragerons actuator in degrees");
   auto drageronsActuatorAngleMaxLineEdit = new MoverCreatorLineEdit(drageronsActuatorAngleMax);
   drageronsActuatorAngleMaxLineEdit->setFrame(false);
   mDrageronsActuatorMaxAngle_deg = 0.0;
   drageronsActuatorAngleMaxLineEdit->setText(QString::number(mDrageronsActuatorMaxAngle_deg));
   aTreeWidgetPtr->setItemWidget(drageronsActuatorAngleMax, 1, drageronsActuatorAngleMaxLineEdit);
   aTreeWidgetPtr->setItemWidget(drageronsActuatorAngleMax, 2, new QLabel(QString::fromStdString("deg")));
   connect(drageronsActuatorAngleMaxLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleDrageronsActuatorMaxAngleLineEditChanged);
   connect(drageronsActuatorAngleMaxLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   drageronsActuatorAngleMax->setHidden(true);
   mDrageronsActuatorMaxAngleLineEditChangedWidget = drageronsActuatorAngleMax;

   QTreeWidgetItem* drageronsActuatorRateMin = new QTreeWidgetItem(treeWidgetItem, QStringList("Dragerons Actuator Rate Min"));
   drageronsActuatorRateMin->setToolTip(0, "The minimum rate of the dragerons actuator in degrees/second");
   auto drageronsActuatorRateMinLineEdit = new MoverCreatorLineEdit(drageronsActuatorRateMin);
   drageronsActuatorRateMinLineEdit->setFrame(false);
   drageronsActuatorRateMinLineEdit->setText(QString::number(mDrageronsActuatorMinRate_dps));
   aTreeWidgetPtr->setItemWidget(drageronsActuatorRateMin, 1, drageronsActuatorRateMinLineEdit);
   aTreeWidgetPtr->setItemWidget(drageronsActuatorRateMin, 2, new QLabel(QString::fromStdString("deg/sec")));
   connect(drageronsActuatorRateMinLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleDrageronsActuatorMinRateLineEditChanged);
   connect(drageronsActuatorRateMinLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   drageronsActuatorRateMin->setHidden(true);
   mDrageronsActuatorMinRateLineEditChangedWidget = drageronsActuatorRateMin;

   QTreeWidgetItem* drageronsActuatorRateMax = new QTreeWidgetItem(treeWidgetItem, QStringList("Dragerons Actuator Rate Max"));
   drageronsActuatorRateMax->setToolTip(0, "The maximum rate of the dragerons actuator in degrees/second");
   auto drageronsActuatorRateMaxLineEdit = new MoverCreatorLineEdit(drageronsActuatorRateMax);
   drageronsActuatorRateMaxLineEdit->setFrame(false);
   drageronsActuatorRateMaxLineEdit->setText(QString::number(mDrageronsActuatorMaxRate_dps));
   aTreeWidgetPtr->setItemWidget(drageronsActuatorRateMax, 1, drageronsActuatorRateMaxLineEdit);
   aTreeWidgetPtr->setItemWidget(drageronsActuatorRateMax, 2, new QLabel(QString::fromStdString("deg/sec")));
   connect(drageronsActuatorRateMaxLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleDrageronsActuatorMaxRateLineEditChanged);
   connect(drageronsActuatorRateMaxLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   drageronsActuatorRateMax->setHidden(true);
   mDrageronsActuatorMaxRateLineEditChangedWidget = drageronsActuatorRateMax;

   // Spoilerons section

   QTreeWidgetItem* SpoileronsPresent = new QTreeWidgetItem(treeWidgetItem, QStringList("Spoilerons Present"));
   SpoileronsPresent->setToolTip(0, "If checked, spoilerons will be present on the wing");
   QCheckBox* SpoileronsPresentCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(SpoileronsPresent, 1, SpoileronsPresentCheckBox);
   aTreeWidgetPtr->setItemWidget(SpoileronsPresent, 2, new QLabel(QString::fromStdString("")));
   connect(SpoileronsPresentCheckBox, &QCheckBox::stateChanged, this, &GeometryWing::HandleSpoileronsPresentCheckBoxChanged);
   mSpoileronsPresentCheckBoxChangedWidget = SpoileronsPresent;

   QTreeWidgetItem* spoileronsChordFractionStart = new QTreeWidgetItem(treeWidgetItem, QStringList("Spoilerons Chord Fraction, Start"));
   spoileronsChordFractionStart->setToolTip(0, "This is the start location of the spoilerons, from the leading edge");
   auto spoileronsChordFractionStartLineEdit = new MoverCreatorLineEdit(spoileronsChordFractionStart);
   spoileronsChordFractionStartLineEdit->setFrame(false);
   spoileronsChordFractionStartLineEdit->setText(QString::number(mSpoileronsChordFractionStart));
   aTreeWidgetPtr->setItemWidget(spoileronsChordFractionStart, 1, spoileronsChordFractionStartLineEdit);
   aTreeWidgetPtr->setItemWidget(spoileronsChordFractionStart, 2, new QLabel(QString::fromStdString("")));
   connect(spoileronsChordFractionStartLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleSpoileronsChordFractionStartLineEditChanged);
   connect(spoileronsChordFractionStartLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mSpoileronsChordFractionStartWidget = spoileronsChordFractionStart;
   mSpoileronsChordFractionStartWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* spoileronsChordFractionEnd = new QTreeWidgetItem(treeWidgetItem, QStringList("Spoilerons Chord Fraction, End"));
   spoileronsChordFractionEnd->setToolTip(0, "This is the end location of the spoilerons, from the leading edge");
   auto spoileronsChordFractionEndLineEdit = new MoverCreatorLineEdit(spoileronsChordFractionEnd);
   spoileronsChordFractionEndLineEdit->setFrame(false);
   spoileronsChordFractionEndLineEdit->setText(QString::number(mSpoileronsChordFractionStart));
   aTreeWidgetPtr->setItemWidget(spoileronsChordFractionEnd, 1, spoileronsChordFractionEndLineEdit);
   aTreeWidgetPtr->setItemWidget(spoileronsChordFractionEnd, 2, new QLabel(QString::fromStdString("")));
   connect(spoileronsChordFractionEndLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleSpoileronsChordFractionEndLineEditChanged);
   connect(spoileronsChordFractionEndLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mSpoileronsChordFractionEndWidget = spoileronsChordFractionEnd;
   mSpoileronsChordFractionEndWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* spoileronsSpanFractionStart = new QTreeWidgetItem(treeWidgetItem, QStringList("Spoilerons Span Fraction, Start"));
   spoileronsSpanFractionStart->setToolTip(0, "This is the start location of the spoilerons, from the root");
   auto spoileronsSpanFractionStartLineEdit = new MoverCreatorLineEdit(spoileronsSpanFractionStart);
   spoileronsSpanFractionStartLineEdit->setFrame(false);
   spoileronsSpanFractionStartLineEdit->setText(QString::number(mSpoileronsSpanFractionStart));
   aTreeWidgetPtr->setItemWidget(spoileronsSpanFractionStart, 1, spoileronsSpanFractionStartLineEdit);
   aTreeWidgetPtr->setItemWidget(spoileronsSpanFractionStart, 2, new QLabel(QString::fromStdString("")));
   connect(spoileronsSpanFractionStartLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleSpoileronsSpanFractionStartLineEditChanged);
   connect(spoileronsSpanFractionStartLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mSpoileronsSpanFractionStartWidget = spoileronsSpanFractionStart;
   mSpoileronsSpanFractionStartWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* spoileronsSpanFractionEnd = new QTreeWidgetItem(treeWidgetItem, QStringList("Spoilerons Span Fraction, End"));
   spoileronsSpanFractionEnd->setToolTip(0, "This is the end location of the spoilerons, from the root");
   auto spoileronsSpanFractionEndLineEdit = new MoverCreatorLineEdit(spoileronsSpanFractionEnd);
   spoileronsSpanFractionEndLineEdit->setFrame(false);
   spoileronsSpanFractionEndLineEdit->setText(QString::number(mSpoileronsSpanFractionStart));
   aTreeWidgetPtr->setItemWidget(spoileronsSpanFractionEnd, 1, spoileronsSpanFractionEndLineEdit);
   aTreeWidgetPtr->setItemWidget(spoileronsSpanFractionEnd, 2, new QLabel(QString::fromStdString("")));
   connect(spoileronsSpanFractionEndLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleSpoileronsSpanFractionEndLineEditChanged);
   connect(spoileronsSpanFractionEndLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mSpoileronsSpanFractionEndWidget = spoileronsSpanFractionEnd;
   mSpoileronsSpanFractionEndWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* SpoileronsUseExponentialAngleMapping = new QTreeWidgetItem(treeWidgetItem, QStringList("Spoilerons Expo Angles"));
   SpoileronsUseExponentialAngleMapping->setToolTip(0, "If checked, the spoilerons will use exponential angle mapping");
   QCheckBox* SpoileronsUseExponentialAngleMappingCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(SpoileronsUseExponentialAngleMapping, 1, SpoileronsUseExponentialAngleMappingCheckBox);
   aTreeWidgetPtr->setItemWidget(SpoileronsUseExponentialAngleMapping, 2, new QLabel(QString::fromStdString("")));
   connect(SpoileronsUseExponentialAngleMappingCheckBox, &QCheckBox::stateChanged, this, &GeometryWing::HandleSpoileronsUseExponentialAngleMappingCheckBoxChanged);
   SpoileronsUseExponentialAngleMapping->setHidden(true);
   mSpoileronsUseExponentialAngleMappingCheckBoxChangedWidget = SpoileronsUseExponentialAngleMapping;

   QTreeWidgetItem* SpoileronsControlSurfaceAngleMin = new QTreeWidgetItem(treeWidgetItem, QStringList("Spoilerons Surface Ang Min"));
   SpoileronsControlSurfaceAngleMin->setToolTip(0, "The minimum control surface angle of the spoilerons in degrees");
   auto SpoileronsControlSurfaceAngleMinLineEdit = new MoverCreatorLineEdit(SpoileronsControlSurfaceAngleMin);
   SpoileronsControlSurfaceAngleMinLineEdit->setFrame(false);
   SpoileronsControlSurfaceAngleMinLineEdit->setText(QString::number(mSpoileronsControlSurfaceMinAngle_deg));
   aTreeWidgetPtr->setItemWidget(SpoileronsControlSurfaceAngleMin, 1, SpoileronsControlSurfaceAngleMinLineEdit);
   aTreeWidgetPtr->setItemWidget(SpoileronsControlSurfaceAngleMin, 2, new QLabel(QString::fromStdString("deg")));
   connect(SpoileronsControlSurfaceAngleMinLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleSpoileronsControlSurfaceMinAngleLineEditChanged);
   connect(SpoileronsControlSurfaceAngleMinLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   SpoileronsControlSurfaceAngleMin->setHidden(true);
   mSpoileronsControlSurfaceMinAngleLineEditChangedWidget = SpoileronsControlSurfaceAngleMin;

   QTreeWidgetItem* SpoileronsControlSurfaceAngleMax = new QTreeWidgetItem(treeWidgetItem, QStringList("Spoilerons Surface Ang Max"));
   SpoileronsControlSurfaceAngleMax->setToolTip(0, "The maximum control surface angle of the spoilerons in degrees");
   auto SpoileronsControlSurfaceAngleMaxLineEdit = new MoverCreatorLineEdit(SpoileronsControlSurfaceAngleMax);
   SpoileronsControlSurfaceAngleMaxLineEdit->setFrame(false);
   SpoileronsControlSurfaceAngleMaxLineEdit->setText(QString::number(mSpoileronsControlSurfaceMaxAngle_deg));
   aTreeWidgetPtr->setItemWidget(SpoileronsControlSurfaceAngleMax, 1, SpoileronsControlSurfaceAngleMaxLineEdit);
   aTreeWidgetPtr->setItemWidget(SpoileronsControlSurfaceAngleMax, 2, new QLabel(QString::fromStdString("deg")));
   connect(SpoileronsControlSurfaceAngleMaxLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleSpoileronsControlSurfaceMaxAngleLineEditChanged);
   connect(SpoileronsControlSurfaceAngleMaxLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   SpoileronsControlSurfaceAngleMax->setHidden(true);
   mSpoileronsControlSurfaceMaxAngleLineEditChangedWidget = SpoileronsControlSurfaceAngleMax;

   QTreeWidgetItem* SpoileronsActuatorAngleMin = new QTreeWidgetItem(treeWidgetItem, QStringList("Spoilerons Actuator Ang Min"));
   SpoileronsActuatorAngleMin->setToolTip(0, "The minimum angle of the spoileron actuator in degrees");
   auto SpoileronsActuatorAngleMinLineEdit = new MoverCreatorLineEdit(SpoileronsActuatorAngleMin);
   SpoileronsActuatorAngleMinLineEdit->setFrame(false);
   SpoileronsActuatorAngleMinLineEdit->setText(QString::number(mSpoileronsActuatorMinAngle_deg));
   aTreeWidgetPtr->setItemWidget(SpoileronsActuatorAngleMin, 1, SpoileronsActuatorAngleMinLineEdit);
   aTreeWidgetPtr->setItemWidget(SpoileronsActuatorAngleMin, 2, new QLabel(QString::fromStdString("deg")));
   connect(SpoileronsActuatorAngleMinLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleSpoileronsActuatorMinAngleLineEditChanged);
   connect(SpoileronsActuatorAngleMinLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   SpoileronsActuatorAngleMin->setHidden(true);
   mSpoileronsActuatorMinAngleLineEditChangedWidget = SpoileronsActuatorAngleMin;

   QTreeWidgetItem* SpoileronsActuatorAngleMax = new QTreeWidgetItem(treeWidgetItem, QStringList("Spoilerons Actuator Ang Max"));
   SpoileronsActuatorAngleMax->setToolTip(0, "The maximum angle of the spoileron actuator in degrees");
   auto SpoileronsActuatorAngleMaxLineEdit = new MoverCreatorLineEdit(SpoileronsActuatorAngleMax);
   SpoileronsActuatorAngleMaxLineEdit->setFrame(false);
   SpoileronsActuatorAngleMaxLineEdit->setText(QString::number(mSpoileronsActuatorMaxAngle_deg));
   aTreeWidgetPtr->setItemWidget(SpoileronsActuatorAngleMax, 1, SpoileronsActuatorAngleMaxLineEdit);
   aTreeWidgetPtr->setItemWidget(SpoileronsActuatorAngleMax, 2, new QLabel(QString::fromStdString("deg")));
   connect(SpoileronsActuatorAngleMaxLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleSpoileronsActuatorMaxAngleLineEditChanged);
   connect(SpoileronsActuatorAngleMaxLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   SpoileronsActuatorAngleMax->setHidden(true);
   mSpoileronsActuatorMaxAngleLineEditChangedWidget = SpoileronsActuatorAngleMax;

   QTreeWidgetItem* SpoileronsActuatorRateMin = new QTreeWidgetItem(treeWidgetItem, QStringList("Spoilerons Actuator Rate Min"));
   SpoileronsActuatorRateMin->setToolTip(0, "The minimum rate of the spoileron actuator in degrees/second");
   auto SpoileronsActuatorRateMinLineEdit = new MoverCreatorLineEdit(SpoileronsActuatorRateMin);
   SpoileronsActuatorRateMinLineEdit->setFrame(false);
   SpoileronsActuatorRateMinLineEdit->setText(QString::number(mSpoileronsActuatorMinRate_dps));
   aTreeWidgetPtr->setItemWidget(SpoileronsActuatorRateMin, 1, SpoileronsActuatorRateMinLineEdit);
   aTreeWidgetPtr->setItemWidget(SpoileronsActuatorRateMin, 2, new QLabel(QString::fromStdString("deg/sec")));
   connect(SpoileronsActuatorRateMinLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleSpoileronsActuatorMinRateLineEditChanged);
   connect(SpoileronsActuatorRateMinLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   SpoileronsActuatorRateMin->setHidden(true);
   mSpoileronsActuatorMinRateLineEditChangedWidget = SpoileronsActuatorRateMin;

   QTreeWidgetItem* SpoileronsActuatorRateMax = new QTreeWidgetItem(treeWidgetItem, QStringList("Spoilerons Actuator Rate Max"));
   SpoileronsActuatorRateMax->setToolTip(0, "The maximum rate of the spoileron actuator in degrees/second");
   auto SpoileronsActuatorRateMaxLineEdit = new MoverCreatorLineEdit(SpoileronsActuatorRateMax);
   SpoileronsActuatorRateMaxLineEdit->setFrame(false);
   SpoileronsActuatorRateMaxLineEdit->setText(QString::number(mSpoileronsActuatorMaxRate_dps));
   aTreeWidgetPtr->setItemWidget(SpoileronsActuatorRateMax, 1, SpoileronsActuatorRateMaxLineEdit);
   aTreeWidgetPtr->setItemWidget(SpoileronsActuatorRateMax, 2, new QLabel(QString::fromStdString("deg/sec")));
   connect(SpoileronsActuatorRateMaxLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleSpoileronsActuatorMaxRateLineEditChanged);
   connect(SpoileronsActuatorRateMaxLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   SpoileronsActuatorRateMax->setHidden(true);
   mSpoileronsActuatorMaxRateLineEditChangedWidget = SpoileronsActuatorRateMax;

   // Spoilers section

   QTreeWidgetItem* SpoilersPresent = new QTreeWidgetItem(treeWidgetItem, QStringList("Spoilers Present"));
   SpoilersPresent->setToolTip(0, "If checked, spoilers will be present on the wing");
   QCheckBox* SpoilersPresentCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(SpoilersPresent, 1, SpoilersPresentCheckBox);
   aTreeWidgetPtr->setItemWidget(SpoilersPresent, 2, new QLabel(QString::fromStdString("")));
   connect(SpoilersPresentCheckBox, &QCheckBox::stateChanged, this, &GeometryWing::HandleSpoilersPresentCheckBoxChanged);
   mSpoilersPresentCheckBoxChangedWidget = SpoilersPresent;

   QTreeWidgetItem* spoilersChordFractionStart = new QTreeWidgetItem(treeWidgetItem, QStringList("Spoilers Chord Fraction, Start"));
   spoilersChordFractionStart->setToolTip(0, "This is the start location of the spoilers, from the leading edge");
   auto spoilersChordFractionStartLineEdit = new MoverCreatorLineEdit(spoilersChordFractionStart);
   spoilersChordFractionStartLineEdit->setFrame(false);
   spoilersChordFractionStartLineEdit->setText(QString::number(mSpoilersChordFractionStart));
   aTreeWidgetPtr->setItemWidget(spoilersChordFractionStart, 1, spoilersChordFractionStartLineEdit);
   aTreeWidgetPtr->setItemWidget(spoilersChordFractionStart, 2, new QLabel(QString::fromStdString("")));
   connect(spoilersChordFractionStartLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleSpoilersChordFractionStartLineEditChanged);
   connect(spoilersChordFractionStartLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mSpoilersChordFractionStartWidget = spoilersChordFractionStart;
   mSpoilersChordFractionStartWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* spoilersChordFractionEnd = new QTreeWidgetItem(treeWidgetItem, QStringList("Spoilers Chord Fraction, End"));
   spoilersChordFractionEnd->setToolTip(0, "This is the end location of the spoilers, from the leading edge");
   auto spoilersChordFractionEndLineEdit = new MoverCreatorLineEdit(spoilersChordFractionEnd);
   spoilersChordFractionEndLineEdit->setFrame(false);
   spoilersChordFractionEndLineEdit->setText(QString::number(mSpoilersChordFractionStart));
   aTreeWidgetPtr->setItemWidget(spoilersChordFractionEnd, 1, spoilersChordFractionEndLineEdit);
   aTreeWidgetPtr->setItemWidget(spoilersChordFractionEnd, 2, new QLabel(QString::fromStdString("")));
   connect(spoilersChordFractionEndLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleSpoilersChordFractionEndLineEditChanged);
   connect(spoilersChordFractionEndLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mSpoilersChordFractionEndWidget = spoilersChordFractionEnd;
   mSpoilersChordFractionEndWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* spoilersSpanFractionStart = new QTreeWidgetItem(treeWidgetItem, QStringList("Spoilers Span Fraction, Start"));
   spoilersSpanFractionStart->setToolTip(0, "This is the start location of the spoilers, from the root");
   auto spoilersSpanFractionStartLineEdit = new MoverCreatorLineEdit(spoilersSpanFractionStart);
   spoilersSpanFractionStartLineEdit->setFrame(false);
   spoilersSpanFractionStartLineEdit->setText(QString::number(mSpoilersSpanFractionStart));
   aTreeWidgetPtr->setItemWidget(spoilersSpanFractionStart, 1, spoilersSpanFractionStartLineEdit);
   aTreeWidgetPtr->setItemWidget(spoilersSpanFractionStart, 2, new QLabel(QString::fromStdString("")));
   connect(spoilersSpanFractionStartLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleSpoilersSpanFractionStartLineEditChanged);
   connect(spoilersSpanFractionStartLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mSpoilersSpanFractionStartWidget = spoilersSpanFractionStart;
   mSpoilersSpanFractionStartWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* spoilersSpanFractionEnd = new QTreeWidgetItem(treeWidgetItem, QStringList("Spoilers Span Fraction, End"));
   spoilersSpanFractionEnd->setToolTip(0, "This is the end location of the spoilers, from the root");
   auto spoilersSpanFractionEndLineEdit = new MoverCreatorLineEdit(spoilersSpanFractionEnd);
   spoilersSpanFractionEndLineEdit->setFrame(false);
   spoilersSpanFractionEndLineEdit->setText(QString::number(mSpoilersSpanFractionStart));
   aTreeWidgetPtr->setItemWidget(spoilersSpanFractionEnd, 1, spoilersSpanFractionEndLineEdit);
   aTreeWidgetPtr->setItemWidget(spoilersSpanFractionEnd, 2, new QLabel(QString::fromStdString("")));
   connect(spoilersSpanFractionEndLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleSpoilersSpanFractionEndLineEditChanged);
   connect(spoilersSpanFractionEndLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mSpoilersSpanFractionEndWidget = spoilersSpanFractionEnd;
   mSpoilersSpanFractionEndWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* SpoilersUseExponentialAngleMapping = new QTreeWidgetItem(treeWidgetItem, QStringList("Spoilers Expo Angles"));
   SpoilersUseExponentialAngleMapping->setToolTip(0, "If checked, the spoilers will use exponential angle mapping");
   QCheckBox* SpoilersUseExponentialAngleMappingCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(SpoilersUseExponentialAngleMapping, 1, SpoilersUseExponentialAngleMappingCheckBox);
   aTreeWidgetPtr->setItemWidget(SpoilersUseExponentialAngleMapping, 2, new QLabel(QString::fromStdString("")));
   connect(SpoilersUseExponentialAngleMappingCheckBox, &QCheckBox::stateChanged, this, &GeometryWing::HandleSpoilersUseExponentialAngleMappingCheckBoxChanged);
   SpoilersUseExponentialAngleMapping->setHidden(true);
   mSpoilersUseExponentialAngleMappingCheckBoxChangedWidget = SpoilersUseExponentialAngleMapping;

   QTreeWidgetItem* SpoilersControlSurfaceAngleMin = new QTreeWidgetItem(treeWidgetItem, QStringList("Spoilers Surface Ang Min"));
   SpoilersControlSurfaceAngleMin->setToolTip(0, "The minimum control surface angle of the spoilers in degrees");
   auto SpoilersControlSurfaceAngleMinLineEdit = new MoverCreatorLineEdit(SpoilersControlSurfaceAngleMin);
   SpoilersControlSurfaceAngleMinLineEdit->setFrame(false);
   SpoilersControlSurfaceAngleMinLineEdit->setText(QString::number(mSpoilersControlSurfaceMinAngle_deg));
   aTreeWidgetPtr->setItemWidget(SpoilersControlSurfaceAngleMin, 1, SpoilersControlSurfaceAngleMinLineEdit);
   aTreeWidgetPtr->setItemWidget(SpoilersControlSurfaceAngleMin, 2, new QLabel(QString::fromStdString("deg")));
   connect(SpoilersControlSurfaceAngleMinLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleSpoilersControlSurfaceMinAngleLineEditChanged);
   connect(SpoilersControlSurfaceAngleMinLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   SpoilersControlSurfaceAngleMin->setHidden(true);
   mSpoilersControlSurfaceMinAngleLineEditChangedWidget = SpoilersControlSurfaceAngleMin;

   QTreeWidgetItem* SpoilersControlSurfaceAngleMax = new QTreeWidgetItem(treeWidgetItem, QStringList("Spoilers Surface Ang Max"));
   SpoilersControlSurfaceAngleMax->setToolTip(0, "The maximum control surface angle of the spoilers in degrees");
   auto SpoilersControlSurfaceAngleMaxLineEdit = new MoverCreatorLineEdit(SpoilersControlSurfaceAngleMax);
   SpoilersControlSurfaceAngleMaxLineEdit->setFrame(false);
   SpoilersControlSurfaceAngleMaxLineEdit->setText(QString::number(mSpoilersControlSurfaceMaxAngle_deg));
   aTreeWidgetPtr->setItemWidget(SpoilersControlSurfaceAngleMax, 1, SpoilersControlSurfaceAngleMaxLineEdit);
   aTreeWidgetPtr->setItemWidget(SpoilersControlSurfaceAngleMax, 2, new QLabel(QString::fromStdString("deg")));
   connect(SpoilersControlSurfaceAngleMaxLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleSpoilersControlSurfaceMaxAngleLineEditChanged);
   connect(SpoilersControlSurfaceAngleMaxLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   SpoilersControlSurfaceAngleMax->setHidden(true);
   mSpoilersControlSurfaceMaxAngleLineEditChangedWidget = SpoilersControlSurfaceAngleMax;

   QTreeWidgetItem* SpoilersActuatorAngleMin = new QTreeWidgetItem(treeWidgetItem, QStringList("Spoilers Actuator Ang Min"));
   SpoilersActuatorAngleMin->setToolTip(0, "The minimum angle of the spoiler actuator in degrees");
   auto SpoilersActuatorAngleMinLineEdit = new MoverCreatorLineEdit(SpoilersActuatorAngleMin);
   SpoilersActuatorAngleMinLineEdit->setFrame(false);
   SpoilersActuatorAngleMinLineEdit->setText(QString::number(mSpoilersActuatorMinAngle_deg));
   aTreeWidgetPtr->setItemWidget(SpoilersActuatorAngleMin, 1, SpoilersActuatorAngleMinLineEdit);
   aTreeWidgetPtr->setItemWidget(SpoilersActuatorAngleMin, 2, new QLabel(QString::fromStdString("deg")));
   connect(SpoilersActuatorAngleMinLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleSpoilersActuatorMinAngleLineEditChanged);
   connect(SpoilersActuatorAngleMinLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   SpoilersActuatorAngleMin->setHidden(true);
   mSpoilersActuatorMinAngleLineEditChangedWidget = SpoilersActuatorAngleMin;

   QTreeWidgetItem* SpoilersActuatorAngleMax = new QTreeWidgetItem(treeWidgetItem, QStringList("Spoilers Actuator Ang Max"));
   SpoilersActuatorAngleMax->setToolTip(0, "The maximum angle of the spoiler actuator in degrees");
   auto SpoilersActuatorAngleMaxLineEdit = new MoverCreatorLineEdit(SpoilersActuatorAngleMax);
   SpoilersActuatorAngleMaxLineEdit->setFrame(false);
   SpoilersActuatorAngleMaxLineEdit->setText(QString::number(mSpoilersActuatorMaxAngle_deg));
   aTreeWidgetPtr->setItemWidget(SpoilersActuatorAngleMax, 1, SpoilersActuatorAngleMaxLineEdit);
   aTreeWidgetPtr->setItemWidget(SpoilersActuatorAngleMax, 2, new QLabel(QString::fromStdString("deg")));
   connect(SpoilersActuatorAngleMaxLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleSpoilersActuatorMaxAngleLineEditChanged);
   connect(SpoilersActuatorAngleMaxLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   SpoilersActuatorAngleMax->setHidden(true);
   mSpoilersActuatorMaxAngleLineEditChangedWidget = SpoilersActuatorAngleMax;

   QTreeWidgetItem* SpoilersActuatorRateMin = new QTreeWidgetItem(treeWidgetItem, QStringList("Spoilers Actuator Rate Min"));
   SpoilersActuatorRateMin->setToolTip(0, "The minimum rate of the spoiler actuator in degrees/second");
   auto SpoilersActuatorRateMinLineEdit = new MoverCreatorLineEdit(SpoilersActuatorRateMin);
   SpoilersActuatorRateMinLineEdit->setFrame(false);
   SpoilersActuatorRateMinLineEdit->setText(QString::number(mSpoilersActuatorMinRate_dps));
   aTreeWidgetPtr->setItemWidget(SpoilersActuatorRateMin, 1, SpoilersActuatorRateMinLineEdit);
   aTreeWidgetPtr->setItemWidget(SpoilersActuatorRateMin, 2, new QLabel(QString::fromStdString("deg/sec")));
   connect(SpoilersActuatorRateMinLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleSpoilersActuatorMinRateLineEditChanged);
   connect(SpoilersActuatorRateMinLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   SpoilersActuatorRateMin->setHidden(true);
   mSpoilersActuatorMinRateLineEditChangedWidget = SpoilersActuatorRateMin;

   QTreeWidgetItem* SpoilersActuatorRateMax = new QTreeWidgetItem(treeWidgetItem, QStringList("Spoilers Actuator Rate Max"));
   SpoilersActuatorRateMax->setToolTip(0, "The maximum rate of the spoiler actuator in degrees/second");
   auto SpoilersActuatorRateMaxLineEdit = new MoverCreatorLineEdit(SpoilersActuatorRateMax);
   SpoilersActuatorRateMaxLineEdit->setFrame(false);
   SpoilersActuatorRateMaxLineEdit->setText(QString::number(mSpoilersActuatorMaxRate_dps));
   aTreeWidgetPtr->setItemWidget(SpoilersActuatorRateMax, 1, SpoilersActuatorRateMaxLineEdit);
   aTreeWidgetPtr->setItemWidget(SpoilersActuatorRateMax, 2, new QLabel(QString::fromStdString("deg/sec")));
   connect(SpoilersActuatorRateMaxLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleSpoilersActuatorMaxRateLineEditChanged);
   connect(SpoilersActuatorRateMaxLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   SpoilersActuatorRateMax->setHidden(true);
   mSpoilersActuatorMaxRateLineEditChangedWidget = SpoilersActuatorRateMax;

   // Elevons section

   QTreeWidgetItem* ElevonsPresent = new QTreeWidgetItem(treeWidgetItem, QStringList("Elevons Present"));
   ElevonsPresent->setToolTip(0, "If checked, elevons will be present on the wing");
   QCheckBox* ElevonsPresentCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(ElevonsPresent, 1, ElevonsPresentCheckBox);
   aTreeWidgetPtr->setItemWidget(ElevonsPresent, 2, new QLabel(QString::fromStdString("")));
   connect(ElevonsPresentCheckBox, &QCheckBox::stateChanged, this, &GeometryWing::HandleElevonsPresentCheckBoxChanged);
   mElevonsPresentCheckBoxChangedWidget = ElevonsPresent;

   QTreeWidgetItem* elevonsChordFractionStart = new QTreeWidgetItem(treeWidgetItem, QStringList("Elevons Chord Fraction, Start"));
   elevonsChordFractionStart->setToolTip(0, "This is the start location of the elevons, from the leading edge");
   auto elevonsChordFractionStartLineEdit = new MoverCreatorLineEdit(elevonsChordFractionStart);
   elevonsChordFractionStartLineEdit->setFrame(false);
   elevonsChordFractionStartLineEdit->setText(QString::number(mElevonsChordFractionStart));
   aTreeWidgetPtr->setItemWidget(elevonsChordFractionStart, 1, elevonsChordFractionStartLineEdit);
   aTreeWidgetPtr->setItemWidget(elevonsChordFractionStart, 2, new QLabel(QString::fromStdString("")));
   connect(elevonsChordFractionStartLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleElevonsChordFractionStartLineEditChanged);
   connect(elevonsChordFractionStartLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mElevonsChordFractionStartWidget = elevonsChordFractionStart;
   mElevonsChordFractionStartWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* elevonsChordFractionEnd = new QTreeWidgetItem(treeWidgetItem, QStringList("Elevons Chord Fraction, End"));
   elevonsChordFractionEnd->setToolTip(0, "This is the end location of the elevons, from the leading edge");
   auto elevonsChordFractionEndLineEdit = new MoverCreatorLineEdit(elevonsChordFractionEnd);
   elevonsChordFractionEndLineEdit->setFrame(false);
   elevonsChordFractionEndLineEdit->setText(QString::number(mElevonsChordFractionEnd));
   aTreeWidgetPtr->setItemWidget(elevonsChordFractionEnd, 1, elevonsChordFractionEndLineEdit);
   aTreeWidgetPtr->setItemWidget(elevonsChordFractionEnd, 2, new QLabel(QString::fromStdString("")));
   connect(elevonsChordFractionEndLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleElevonsChordFractionEndLineEditChanged);
   connect(elevonsChordFractionEndLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mElevonsChordFractionEndWidget = elevonsChordFractionEnd;
   mElevonsChordFractionEndWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* elevonsSpanFractionStart = new QTreeWidgetItem(treeWidgetItem, QStringList("Elevons Span Fraction, Start"));
   elevonsSpanFractionStart->setToolTip(0, "This is the start location of the elevons, from the root");
   auto elevonsSpanFractionStartLineEdit = new MoverCreatorLineEdit(elevonsSpanFractionStart);
   elevonsSpanFractionStartLineEdit->setFrame(false);
   elevonsSpanFractionStartLineEdit->setText(QString::number(mElevonsSpanFractionStart));
   aTreeWidgetPtr->setItemWidget(elevonsSpanFractionStart, 1, elevonsSpanFractionStartLineEdit);
   aTreeWidgetPtr->setItemWidget(elevonsSpanFractionStart, 2, new QLabel(QString::fromStdString("")));
   connect(elevonsSpanFractionStartLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleElevonsSpanFractionStartLineEditChanged);
   connect(elevonsSpanFractionStartLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mElevonsSpanFractionStartWidget = elevonsSpanFractionStart;
   mElevonsSpanFractionStartWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* elevonsSpanFractionEnd = new QTreeWidgetItem(treeWidgetItem, QStringList("Elevons Span Fraction, End"));
   elevonsSpanFractionEnd->setToolTip(0, "This is the end location of the elevons, from the root");
   auto elevonsSpanFractionEndLineEdit = new MoverCreatorLineEdit(elevonsSpanFractionEnd);
   elevonsSpanFractionEndLineEdit->setFrame(false);
   elevonsSpanFractionEndLineEdit->setText(QString::number(mElevonsSpanFractionEnd));
   aTreeWidgetPtr->setItemWidget(elevonsSpanFractionEnd, 1, elevonsSpanFractionEndLineEdit);
   aTreeWidgetPtr->setItemWidget(elevonsSpanFractionEnd, 2, new QLabel(QString::fromStdString("")));
   connect(elevonsSpanFractionEndLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleElevonsSpanFractionEndLineEditChanged);
   connect(elevonsSpanFractionEndLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mElevonsSpanFractionEndWidget = elevonsSpanFractionEnd;
   mElevonsSpanFractionEndWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* ElevonsUseExponentialAngleMapping = new QTreeWidgetItem(treeWidgetItem, QStringList("Elevons Expo Angles"));
   ElevonsUseExponentialAngleMapping->setToolTip(0, "If checked, the elevons will use exponential angle mapping");
   QCheckBox* ElevonsUseExponentialAngleMappingCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(ElevonsUseExponentialAngleMapping, 1, ElevonsUseExponentialAngleMappingCheckBox);
   aTreeWidgetPtr->setItemWidget(ElevonsUseExponentialAngleMapping, 2, new QLabel(QString::fromStdString("")));
   connect(ElevonsUseExponentialAngleMappingCheckBox, &QCheckBox::stateChanged, this, &GeometryWing::HandleElevonsUseExponentialAngleMappingCheckBoxChanged);
   ElevonsUseExponentialAngleMapping->setHidden(true);
   mElevonsUseExponentialAngleMappingCheckBoxChangedWidget = ElevonsUseExponentialAngleMapping;

   QTreeWidgetItem* ElevonsControlSurfaceAngleMin = new QTreeWidgetItem(treeWidgetItem, QStringList("Elevons Surface Ang Min"));
   ElevonsControlSurfaceAngleMin->setToolTip(0, "The minimum control surface angle of the elevons in degrees");
   auto ElevonsControlSurfaceAngleMinLineEdit = new MoverCreatorLineEdit(ElevonsControlSurfaceAngleMin);
   ElevonsControlSurfaceAngleMinLineEdit->setFrame(false);
   ElevonsControlSurfaceAngleMinLineEdit->setText(QString::number(mElevonsControlSurfaceMinAngle_deg));
   aTreeWidgetPtr->setItemWidget(ElevonsControlSurfaceAngleMin, 1, ElevonsControlSurfaceAngleMinLineEdit);
   aTreeWidgetPtr->setItemWidget(ElevonsControlSurfaceAngleMin, 2, new QLabel(QString::fromStdString("deg")));
   connect(ElevonsControlSurfaceAngleMinLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleElevonsControlSurfaceMinAngleLineEditChanged);
   connect(ElevonsControlSurfaceAngleMinLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   ElevonsControlSurfaceAngleMin->setHidden(true);
   mElevonsControlSurfaceMinAngleLineEditChangedWidget = ElevonsControlSurfaceAngleMin;

   QTreeWidgetItem* ElevonsControlSurfaceAngleMax = new QTreeWidgetItem(treeWidgetItem, QStringList("Elevons Surface Ang Max"));
   ElevonsControlSurfaceAngleMax->setToolTip(0, "The maximum control surface angle of the elevons in degrees");
   auto ElevonsControlSurfaceAngleMaxLineEdit = new MoverCreatorLineEdit(ElevonsControlSurfaceAngleMax);
   ElevonsControlSurfaceAngleMaxLineEdit->setFrame(false);
   ElevonsControlSurfaceAngleMaxLineEdit->setText(QString::number(mElevonsControlSurfaceMaxAngle_deg));
   aTreeWidgetPtr->setItemWidget(ElevonsControlSurfaceAngleMax, 1, ElevonsControlSurfaceAngleMaxLineEdit);
   aTreeWidgetPtr->setItemWidget(ElevonsControlSurfaceAngleMax, 2, new QLabel(QString::fromStdString("deg")));
   connect(ElevonsControlSurfaceAngleMaxLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleElevonsControlSurfaceMaxAngleLineEditChanged);
   connect(ElevonsControlSurfaceAngleMaxLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   ElevonsControlSurfaceAngleMax->setHidden(true);
   mElevonsControlSurfaceMaxAngleLineEditChangedWidget = ElevonsControlSurfaceAngleMax;

   QTreeWidgetItem* ElevonsActuatorAngleMin = new QTreeWidgetItem(treeWidgetItem, QStringList("Elevons Actuator Ang Min"));
   ElevonsActuatorAngleMin->setToolTip(0, "The minimum angle of the elevon actuator in degrees");
   auto ElevonsActuatorAngleMinLineEdit = new MoverCreatorLineEdit(ElevonsActuatorAngleMin);
   ElevonsActuatorAngleMinLineEdit->setFrame(false);
   ElevonsActuatorAngleMinLineEdit->setText(QString::number(mElevonsActuatorMinAngle_deg));
   aTreeWidgetPtr->setItemWidget(ElevonsActuatorAngleMin, 1, ElevonsActuatorAngleMinLineEdit);
   aTreeWidgetPtr->setItemWidget(ElevonsActuatorAngleMin, 2, new QLabel(QString::fromStdString("deg")));
   connect(ElevonsActuatorAngleMinLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleElevonsActuatorMinAngleLineEditChanged);
   connect(ElevonsActuatorAngleMinLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   ElevonsActuatorAngleMin->setHidden(true);
   mElevonsActuatorMinAngleLineEditChangedWidget = ElevonsActuatorAngleMin;

   QTreeWidgetItem* ElevonsActuatorAngleMax = new QTreeWidgetItem(treeWidgetItem, QStringList("Elevons Actuator Ang Max"));
   ElevonsActuatorAngleMax->setToolTip(0, "The maximum angle of the elevon actuator in degrees");
   auto ElevonsActuatorAngleMaxLineEdit = new MoverCreatorLineEdit(ElevonsActuatorAngleMax);
   ElevonsActuatorAngleMaxLineEdit->setFrame(false);
   ElevonsActuatorAngleMaxLineEdit->setText(QString::number(mElevonsActuatorMaxAngle_deg));
   aTreeWidgetPtr->setItemWidget(ElevonsActuatorAngleMax, 1, ElevonsActuatorAngleMaxLineEdit);
   aTreeWidgetPtr->setItemWidget(ElevonsActuatorAngleMax, 2, new QLabel(QString::fromStdString("deg")));
   connect(ElevonsActuatorAngleMaxLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleElevonsActuatorMaxAngleLineEditChanged);
   connect(ElevonsActuatorAngleMaxLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   ElevonsActuatorAngleMax->setHidden(true);
   mElevonsActuatorMaxAngleLineEditChangedWidget = ElevonsActuatorAngleMax;

   QTreeWidgetItem* ElevonsActuatorRateMin = new QTreeWidgetItem(treeWidgetItem, QStringList("Elevons Actuator Rate Min"));
   ElevonsActuatorRateMin->setToolTip(0, "The minimum rate of the elevon actuator in degrees/second");
   auto ElevonsActuatorRateMinLineEdit = new MoverCreatorLineEdit(ElevonsActuatorRateMin);
   ElevonsActuatorRateMinLineEdit->setFrame(false);
   ElevonsActuatorRateMinLineEdit->setText(QString::number(mElevonsActuatorMinRate_dps));
   aTreeWidgetPtr->setItemWidget(ElevonsActuatorRateMin, 1, ElevonsActuatorRateMinLineEdit);
   aTreeWidgetPtr->setItemWidget(ElevonsActuatorRateMin, 2, new QLabel(QString::fromStdString("deg/sec")));
   connect(ElevonsActuatorRateMinLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleElevonsActuatorMinRateLineEditChanged);
   connect(ElevonsActuatorRateMinLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   ElevonsActuatorRateMin->setHidden(true);
   mElevonsActuatorMinRateLineEditChangedWidget = ElevonsActuatorRateMin;

   QTreeWidgetItem* ElevonsActuatorRateMax = new QTreeWidgetItem(treeWidgetItem, QStringList("Elevons Actuator Rate Max"));
   ElevonsActuatorRateMax->setToolTip(0, "The maximum rate of the elevon actuator in degrees/second");
   auto ElevonsActuatorRateMaxLineEdit = new MoverCreatorLineEdit(ElevonsActuatorRateMax);
   ElevonsActuatorRateMaxLineEdit->setFrame(false);
   ElevonsActuatorRateMaxLineEdit->setText(QString::number(mElevonsActuatorMaxRate_dps));
   aTreeWidgetPtr->setItemWidget(ElevonsActuatorRateMax, 1, ElevonsActuatorRateMaxLineEdit);
   aTreeWidgetPtr->setItemWidget(ElevonsActuatorRateMax, 2, new QLabel(QString::fromStdString("deg/sec")));
   connect(ElevonsActuatorRateMaxLineEdit, &QLineEdit::textChanged, this, &GeometryWing::HandleElevonsActuatorMaxRateLineEditChanged);
   connect(ElevonsActuatorRateMaxLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   ElevonsActuatorRateMax->setHidden(true);
   mElevonsActuatorMaxRateLineEditChangedWidget = ElevonsActuatorRateMax;

   // For wings, we hide the symmetry option, since wings *must* be symmetrical
   if (mSymmetricalWidgetItem)
   {
      mSymmetricalWidgetItem->setHidden(true);
   }

   // Hide the reference point "y" (the two symmetrical pieces of a wing "must" touch,
   // hence the reference point y must always be 0 (zero).
   if (mReferencePointY_TreeWidgetItemPtr)
   {
      mReferencePointY_TreeWidgetItemPtr->setHidden(true);
   }

   return treeWidgetItem;
}

void GeometryWing::HandleAileronsPresentCheckBoxChanged(int aValue)
{
   if (aValue == 0)
   {
      mAileronsPresent = false;

      if (mAileronsChordFractionStartWidget != nullptr) { mAileronsChordFractionStartWidget->setHidden(true); }
      if (mAileronsChordFractionEndWidget != nullptr)   { mAileronsChordFractionEndWidget->setHidden(true); }
      if (mAileronsSpanFractionStartWidget != nullptr)  { mAileronsSpanFractionStartWidget->setHidden(true); }
      if (mAileronsSpanFractionEndWidget != nullptr)    { mAileronsSpanFractionEndWidget->setHidden(true); }
      if (mAileronsUseExponentialAngleMappingCheckBoxChangedWidget != nullptr) { mAileronsUseExponentialAngleMappingCheckBoxChangedWidget->setHidden(true); }
      if (mAileronsControlSurfaceMinAngleLineEditChangedWidget != nullptr) { mAileronsControlSurfaceMinAngleLineEditChangedWidget->setHidden(true); }
      if (mAileronsControlSurfaceMaxAngleLineEditChangedWidget != nullptr) { mAileronsControlSurfaceMaxAngleLineEditChangedWidget->setHidden(true); }
      if (mAileronsActuatorMinRateLineEditChangedWidget != nullptr) { mAileronsActuatorMinRateLineEditChangedWidget->setHidden(true); }
      if (mAileronsActuatorMaxRateLineEditChangedWidget != nullptr) { mAileronsActuatorMaxRateLineEditChangedWidget->setHidden(true); }
      if (mAileronsActuatorMinAngleLineEditChangedWidget != nullptr) { mAileronsActuatorMinAngleLineEditChangedWidget->setHidden(true); }
      if (mAileronsActuatorMaxAngleLineEditChangedWidget != nullptr) { mAileronsActuatorMaxAngleLineEditChangedWidget->setHidden(true); }
   }
   else
   {
      mAileronsPresent = true;

      if (mAileronsChordFractionStartWidget != nullptr) { mAileronsChordFractionStartWidget->setHidden(false); }
      if (mAileronsChordFractionEndWidget != nullptr)   { mAileronsChordFractionEndWidget->setHidden(false); }
      if (mAileronsSpanFractionStartWidget != nullptr)  { mAileronsSpanFractionStartWidget->setHidden(false); }
      if (mAileronsSpanFractionEndWidget != nullptr)    { mAileronsSpanFractionEndWidget->setHidden(false); }
      if (mAileronsUseExponentialAngleMappingCheckBoxChangedWidget != nullptr) { mAileronsUseExponentialAngleMappingCheckBoxChangedWidget->setHidden(false); }
      if (mAileronsControlSurfaceMinAngleLineEditChangedWidget != nullptr) { mAileronsControlSurfaceMinAngleLineEditChangedWidget->setHidden(false); }
      if (mAileronsControlSurfaceMaxAngleLineEditChangedWidget != nullptr) { mAileronsControlSurfaceMaxAngleLineEditChangedWidget->setHidden(false); }
      if (mAileronsActuatorMinRateLineEditChangedWidget != nullptr) { mAileronsActuatorMinRateLineEditChangedWidget->setHidden(false); }
      if (mAileronsActuatorMaxRateLineEditChangedWidget != nullptr) { mAileronsActuatorMaxRateLineEditChangedWidget->setHidden(false); }
      if (mAileronsActuatorMinAngleLineEditChangedWidget != nullptr) { mAileronsActuatorMinAngleLineEditChangedWidget->setHidden(false); }
      if (mAileronsActuatorMaxAngleLineEditChangedWidget != nullptr) { mAileronsActuatorMaxAngleLineEditChangedWidget->setHidden(false); }
   }
   AdjustAndRedraw(true);
}

void GeometryWing::HandleAileronsChordFractionStartLineEditChanged(QString aValue)
{
   mAileronsChordFractionStart = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleAileronsChordFractionEndLineEditChanged(QString aValue)
{
   mAileronsChordFractionEnd = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleAileronsSpanFractionStartLineEditChanged(QString aValue)
{
   mAileronsSpanFractionStart = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleAileronsSpanFractionEndLineEditChanged(QString aValue)
{
   mAileronsSpanFractionEnd = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleAileronsUseExponentialAngleMappingCheckBoxChanged(int aValue)
{
   if (aValue == 0)
   {
      mAileronsUseExponentialAngleMapping = false;
   }
   else
   {
      mAileronsUseExponentialAngleMapping = true;
   }

   AdjustAndRedraw(true);
}

void GeometryWing::HandleAileronsControlSurfaceMinAngleLineEditChanged(QString aValue)
{
   mAileronsControlSurfaceMinAngle_deg = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleAileronsControlSurfaceMaxAngleLineEditChanged(QString aValue)
{
   mAileronsControlSurfaceMaxAngle_deg = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleAileronsActuatorMinRateLineEditChanged(QString aValue)
{
   mAileronsActuatorMinRate_dps = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleAileronsActuatorMaxRateLineEditChanged(QString aValue)
{
   mAileronsActuatorMaxRate_dps = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleAileronsActuatorMinAngleLineEditChanged(QString aValue)
{
   mAileronsActuatorMinAngle_deg = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleAileronsActuatorMaxAngleLineEditChanged(QString aValue)
{
   mAileronsActuatorMaxAngle_deg = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleDrageronsPresentCheckBoxChanged(int aValue)
{
   if (aValue == 0)
   {
      mDrageronsPresent = false;

      if (mDrageronsChordFractionStartWidget != nullptr) { mDrageronsChordFractionStartWidget->setHidden(true); }
      if (mDrageronsChordFractionEndWidget != nullptr)   { mDrageronsChordFractionEndWidget->setHidden(true); }
      if (mDrageronsSpanFractionStartWidget != nullptr)  { mDrageronsSpanFractionStartWidget->setHidden(true); }
      if (mDrageronsSpanFractionEndWidget != nullptr)    { mDrageronsSpanFractionEndWidget->setHidden(true); }
      if (mDrageronsUseExponentialAngleMappingCheckBoxChangedWidget != nullptr) { mDrageronsUseExponentialAngleMappingCheckBoxChangedWidget->setHidden(true); }
      if (mDrageronsControlSurfaceMinAngleLineEditChangedWidget != nullptr) { mDrageronsControlSurfaceMinAngleLineEditChangedWidget->setHidden(true); }
      if (mDrageronsControlSurfaceMaxAngleLineEditChangedWidget != nullptr) { mDrageronsControlSurfaceMaxAngleLineEditChangedWidget->setHidden(true); }
      if (mDrageronsActuatorMinRateLineEditChangedWidget != nullptr) { mDrageronsActuatorMinRateLineEditChangedWidget->setHidden(true); }
      if (mDrageronsActuatorMaxRateLineEditChangedWidget != nullptr) { mDrageronsActuatorMaxRateLineEditChangedWidget->setHidden(true); }
      if (mDrageronsActuatorMinAngleLineEditChangedWidget != nullptr) { mDrageronsActuatorMinAngleLineEditChangedWidget->setHidden(true); }
      if (mDrageronsActuatorMaxAngleLineEditChangedWidget != nullptr) { mDrageronsActuatorMaxAngleLineEditChangedWidget->setHidden(true); }
   }
   else
   {
      mDrageronsPresent = true;

      if (mDrageronsChordFractionStartWidget != nullptr) { mDrageronsChordFractionStartWidget->setHidden(false); }
      if (mDrageronsChordFractionEndWidget != nullptr)   { mDrageronsChordFractionEndWidget->setHidden(false); }
      if (mDrageronsSpanFractionStartWidget != nullptr)  { mDrageronsSpanFractionStartWidget->setHidden(false); }
      if (mDrageronsSpanFractionEndWidget != nullptr)    { mDrageronsSpanFractionEndWidget->setHidden(false); }
      if (mDrageronsUseExponentialAngleMappingCheckBoxChangedWidget != nullptr) { mDrageronsUseExponentialAngleMappingCheckBoxChangedWidget->setHidden(false); }
      if (mDrageronsControlSurfaceMinAngleLineEditChangedWidget != nullptr) { mDrageronsControlSurfaceMinAngleLineEditChangedWidget->setHidden(false); }
      if (mDrageronsControlSurfaceMaxAngleLineEditChangedWidget != nullptr) { mDrageronsControlSurfaceMaxAngleLineEditChangedWidget->setHidden(false); }
      if (mDrageronsActuatorMinRateLineEditChangedWidget != nullptr) { mDrageronsActuatorMinRateLineEditChangedWidget->setHidden(false); }
      if (mDrageronsActuatorMaxRateLineEditChangedWidget != nullptr) { mDrageronsActuatorMaxRateLineEditChangedWidget->setHidden(false); }
      if (mDrageronsActuatorMinAngleLineEditChangedWidget != nullptr) { mDrageronsActuatorMinAngleLineEditChangedWidget->setHidden(false); }
      if (mDrageronsActuatorMaxAngleLineEditChangedWidget != nullptr) { mDrageronsActuatorMaxAngleLineEditChangedWidget->setHidden(false); }
   }
   AdjustAndRedraw(true);
}

void GeometryWing::HandleDrageronsChordFractionStartLineEditChanged(QString aValue)
{
   mDrageronsChordFractionStart = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleDrageronsChordFractionEndLineEditChanged(QString aValue)
{
   mDrageronsChordFractionEnd = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleDrageronsSpanFractionStartLineEditChanged(QString aValue)
{
   mDrageronsSpanFractionStart = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleDrageronsSpanFractionEndLineEditChanged(QString aValue)
{
   mDrageronsSpanFractionEnd = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleDrageronsUseExponentialAngleMappingCheckBoxChanged(int aValue)
{
   if (aValue == 0)
   {
      mDrageronsUseExponentialAngleMapping = false;
   }
   else
   {
      mDrageronsUseExponentialAngleMapping = true;
   }

   AdjustAndRedraw(true);
}

void GeometryWing::HandleDrageronsControlSurfaceMinAngleLineEditChanged(QString aValue)
{
   mDrageronsControlSurfaceMinAngle_deg = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleDrageronsControlSurfaceMaxAngleLineEditChanged(QString aValue)
{
   mDrageronsControlSurfaceMaxAngle_deg = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleDrageronsActuatorMinRateLineEditChanged(QString aValue)
{
   mDrageronsActuatorMinRate_dps = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleDrageronsActuatorMaxRateLineEditChanged(QString aValue)
{
   mDrageronsActuatorMaxRate_dps = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleDrageronsActuatorMinAngleLineEditChanged(QString aValue)
{
   mDrageronsActuatorMinAngle_deg = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleDrageronsActuatorMaxAngleLineEditChanged(QString aValue)
{
   mDrageronsActuatorMaxAngle_deg = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleSpoileronsPresentCheckBoxChanged(int aValue)
{
   if (aValue == 0)
   {
      mSpoileronsPresent = false;

      if (mSpoileronsChordFractionStartWidget != nullptr) { mSpoileronsChordFractionStartWidget->setHidden(true); }
      if (mSpoileronsChordFractionEndWidget != nullptr)   { mSpoileronsChordFractionEndWidget->setHidden(true); }
      if (mSpoileronsSpanFractionStartWidget != nullptr)  { mSpoileronsSpanFractionStartWidget->setHidden(true); }
      if (mSpoileronsSpanFractionEndWidget != nullptr)    { mSpoileronsSpanFractionEndWidget->setHidden(true); }
      if (mSpoileronsUseExponentialAngleMappingCheckBoxChangedWidget != nullptr) { mSpoileronsUseExponentialAngleMappingCheckBoxChangedWidget->setHidden(true); }
      if (mSpoileronsControlSurfaceMinAngleLineEditChangedWidget != nullptr) { mSpoileronsControlSurfaceMinAngleLineEditChangedWidget->setHidden(true); }
      if (mSpoileronsControlSurfaceMaxAngleLineEditChangedWidget != nullptr) { mSpoileronsControlSurfaceMaxAngleLineEditChangedWidget->setHidden(true); }
      if (mSpoileronsActuatorMinRateLineEditChangedWidget != nullptr) { mSpoileronsActuatorMinRateLineEditChangedWidget->setHidden(true); }
      if (mSpoileronsActuatorMaxRateLineEditChangedWidget != nullptr) { mSpoileronsActuatorMaxRateLineEditChangedWidget->setHidden(true); }
      if (mSpoileronsActuatorMinAngleLineEditChangedWidget != nullptr) { mSpoileronsActuatorMinAngleLineEditChangedWidget->setHidden(true); }
      if (mSpoileronsActuatorMaxAngleLineEditChangedWidget != nullptr) { mSpoileronsActuatorMaxAngleLineEditChangedWidget->setHidden(true); }
   }
   else
   {
      mSpoileronsPresent = true;

      if (mSpoileronsChordFractionStartWidget != nullptr) { mSpoileronsChordFractionStartWidget->setHidden(false); }
      if (mSpoileronsChordFractionEndWidget != nullptr)   { mSpoileronsChordFractionEndWidget->setHidden(false); }
      if (mSpoileronsSpanFractionStartWidget != nullptr)  { mSpoileronsSpanFractionStartWidget->setHidden(false); }
      if (mSpoileronsSpanFractionEndWidget != nullptr)    { mSpoileronsSpanFractionEndWidget->setHidden(false); }
      if (mSpoileronsUseExponentialAngleMappingCheckBoxChangedWidget != nullptr) { mSpoileronsUseExponentialAngleMappingCheckBoxChangedWidget->setHidden(false); }
      if (mSpoileronsControlSurfaceMinAngleLineEditChangedWidget != nullptr) { mSpoileronsControlSurfaceMinAngleLineEditChangedWidget->setHidden(false); }
      if (mSpoileronsControlSurfaceMaxAngleLineEditChangedWidget != nullptr) { mSpoileronsControlSurfaceMaxAngleLineEditChangedWidget->setHidden(false); }
      if (mSpoileronsActuatorMinRateLineEditChangedWidget != nullptr) { mSpoileronsActuatorMinRateLineEditChangedWidget->setHidden(false); }
      if (mSpoileronsActuatorMaxRateLineEditChangedWidget != nullptr) { mSpoileronsActuatorMaxRateLineEditChangedWidget->setHidden(false); }
      if (mSpoileronsActuatorMinAngleLineEditChangedWidget != nullptr) { mSpoileronsActuatorMinAngleLineEditChangedWidget->setHidden(false); }
      if (mSpoileronsActuatorMaxAngleLineEditChangedWidget != nullptr) { mSpoileronsActuatorMaxAngleLineEditChangedWidget->setHidden(false); }
   }
   AdjustAndRedraw(true);
}

void GeometryWing::HandleSpoileronsChordFractionStartLineEditChanged(QString aValue)
{
   mSpoileronsChordFractionStart = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleSpoileronsChordFractionEndLineEditChanged(QString aValue)
{
   mSpoileronsChordFractionEnd = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleSpoileronsSpanFractionStartLineEditChanged(QString aValue)
{
   mSpoileronsSpanFractionStart = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleSpoileronsSpanFractionEndLineEditChanged(QString aValue)
{
   mSpoileronsSpanFractionEnd = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleSpoileronsUseExponentialAngleMappingCheckBoxChanged(int aValue)
{
   if (aValue == 0)
   {
      mSpoileronsUseExponentialAngleMapping = false;
   }
   else
   {
      mSpoileronsUseExponentialAngleMapping = true;
   }

   AdjustAndRedraw(true);
}

void GeometryWing::HandleSpoileronsControlSurfaceMinAngleLineEditChanged(QString aValue)
{
   mSpoileronsControlSurfaceMinAngle_deg = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleSpoileronsControlSurfaceMaxAngleLineEditChanged(QString aValue)
{
   mSpoileronsControlSurfaceMaxAngle_deg = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleSpoileronsActuatorMinRateLineEditChanged(QString aValue)
{
   mSpoileronsActuatorMinRate_dps = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleSpoileronsActuatorMaxRateLineEditChanged(QString aValue)
{
   mSpoileronsActuatorMaxRate_dps = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleSpoileronsActuatorMinAngleLineEditChanged(QString aValue)
{
   mSpoileronsActuatorMinAngle_deg = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleSpoileronsActuatorMaxAngleLineEditChanged(QString aValue)
{
   mSpoileronsActuatorMaxAngle_deg = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleSpoilersPresentCheckBoxChanged(int aValue)
{
   if (aValue == 0)
   {
      mSpoilersPresent = false;

      if (mSpoilersChordFractionStartWidget != nullptr) { mSpoilersChordFractionStartWidget->setHidden(true); }
      if (mSpoilersChordFractionEndWidget != nullptr)   { mSpoilersChordFractionEndWidget->setHidden(true); }
      if (mSpoilersSpanFractionStartWidget != nullptr)  { mSpoilersSpanFractionStartWidget->setHidden(true); }
      if (mSpoilersSpanFractionEndWidget != nullptr)    { mSpoilersSpanFractionEndWidget->setHidden(true); }
      if (mSpoilersUseExponentialAngleMappingCheckBoxChangedWidget != nullptr) { mSpoilersUseExponentialAngleMappingCheckBoxChangedWidget->setHidden(true); }
      if (mSpoilersControlSurfaceMinAngleLineEditChangedWidget != nullptr) { mSpoilersControlSurfaceMinAngleLineEditChangedWidget->setHidden(true); }
      if (mSpoilersControlSurfaceMaxAngleLineEditChangedWidget != nullptr) { mSpoilersControlSurfaceMaxAngleLineEditChangedWidget->setHidden(true); }
      if (mSpoilersActuatorMinRateLineEditChangedWidget != nullptr) { mSpoilersActuatorMinRateLineEditChangedWidget->setHidden(true); }
      if (mSpoilersActuatorMaxRateLineEditChangedWidget != nullptr) { mSpoilersActuatorMaxRateLineEditChangedWidget->setHidden(true); }
      if (mSpoilersActuatorMinAngleLineEditChangedWidget != nullptr) { mSpoilersActuatorMinAngleLineEditChangedWidget->setHidden(true); }
      if (mSpoilersActuatorMaxAngleLineEditChangedWidget != nullptr) { mSpoilersActuatorMaxAngleLineEditChangedWidget->setHidden(true); }
   }
   else
   {
      mSpoilersPresent = true;

      if (mSpoilersChordFractionStartWidget != nullptr) { mSpoilersChordFractionStartWidget->setHidden(false); }
      if (mSpoilersChordFractionEndWidget != nullptr)   { mSpoilersChordFractionEndWidget->setHidden(false); }
      if (mSpoilersSpanFractionStartWidget != nullptr)  { mSpoilersSpanFractionStartWidget->setHidden(false); }
      if (mSpoilersSpanFractionEndWidget != nullptr)    { mSpoilersSpanFractionEndWidget->setHidden(false); }
      if (mSpoilersUseExponentialAngleMappingCheckBoxChangedWidget != nullptr) { mSpoilersUseExponentialAngleMappingCheckBoxChangedWidget->setHidden(false); }
      if (mSpoilersControlSurfaceMinAngleLineEditChangedWidget != nullptr) { mSpoilersControlSurfaceMinAngleLineEditChangedWidget->setHidden(false); }
      if (mSpoilersControlSurfaceMaxAngleLineEditChangedWidget != nullptr) { mSpoilersControlSurfaceMaxAngleLineEditChangedWidget->setHidden(false); }
      if (mSpoilersActuatorMinRateLineEditChangedWidget != nullptr) { mSpoilersActuatorMinRateLineEditChangedWidget->setHidden(false); }
      if (mSpoilersActuatorMaxRateLineEditChangedWidget != nullptr) { mSpoilersActuatorMaxRateLineEditChangedWidget->setHidden(false); }
      if (mSpoilersActuatorMinAngleLineEditChangedWidget != nullptr) { mSpoilersActuatorMinAngleLineEditChangedWidget->setHidden(false); }
      if (mSpoilersActuatorMaxAngleLineEditChangedWidget != nullptr) { mSpoilersActuatorMaxAngleLineEditChangedWidget->setHidden(false); }
   }
   AdjustAndRedraw(true);
}

void GeometryWing::HandleSpoilersChordFractionStartLineEditChanged(QString aValue)
{
   mSpoilersChordFractionStart = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleSpoilersChordFractionEndLineEditChanged(QString aValue)
{
   mSpoilersChordFractionEnd = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleSpoilersSpanFractionStartLineEditChanged(QString aValue)
{
   mSpoilersSpanFractionStart = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleSpoilersSpanFractionEndLineEditChanged(QString aValue)
{
   mSpoilersSpanFractionEnd = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleSpoilersUseExponentialAngleMappingCheckBoxChanged(int aValue)
{
   if (aValue == 0)
   {
      mSpoilersUseExponentialAngleMapping = false;
   }
   else
   {
      mSpoilersUseExponentialAngleMapping = true;
   }

   AdjustAndRedraw(true);
}

void GeometryWing::HandleSpoilersControlSurfaceMinAngleLineEditChanged(QString aValue)
{
   mSpoilersControlSurfaceMinAngle_deg = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleSpoilersControlSurfaceMaxAngleLineEditChanged(QString aValue)
{
   mSpoilersControlSurfaceMaxAngle_deg = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleSpoilersActuatorMinRateLineEditChanged(QString aValue)
{
   mSpoilersActuatorMinRate_dps = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleSpoilersActuatorMaxRateLineEditChanged(QString aValue)
{
   mSpoilersActuatorMaxRate_dps = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleSpoilersActuatorMinAngleLineEditChanged(QString aValue)
{
   mSpoilersActuatorMinAngle_deg = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleSpoilersActuatorMaxAngleLineEditChanged(QString aValue)
{
   mSpoilersActuatorMaxAngle_deg = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleElevonsPresentCheckBoxChanged(int aValue)
{
   if (aValue == 0)
   {
      mElevonsPresent = false;

      if (mElevonsChordFractionStartWidget != nullptr) { mElevonsChordFractionStartWidget->setHidden(true); }
      if (mElevonsChordFractionEndWidget != nullptr) { mElevonsChordFractionEndWidget->setHidden(true); }
      if (mElevonsSpanFractionStartWidget != nullptr) { mElevonsSpanFractionStartWidget->setHidden(true); }
      if (mElevonsSpanFractionEndWidget != nullptr) { mElevonsSpanFractionEndWidget->setHidden(true); }
      if (mElevonsUseExponentialAngleMappingCheckBoxChangedWidget != nullptr) { mElevonsUseExponentialAngleMappingCheckBoxChangedWidget->setHidden(true); }
      if (mElevonsControlSurfaceMinAngleLineEditChangedWidget != nullptr) { mElevonsControlSurfaceMinAngleLineEditChangedWidget->setHidden(true); }
      if (mElevonsControlSurfaceMaxAngleLineEditChangedWidget != nullptr) { mElevonsControlSurfaceMaxAngleLineEditChangedWidget->setHidden(true); }
      if (mElevonsActuatorMinRateLineEditChangedWidget != nullptr) { mElevonsActuatorMinRateLineEditChangedWidget->setHidden(true); }
      if (mElevonsActuatorMaxRateLineEditChangedWidget != nullptr) { mElevonsActuatorMaxRateLineEditChangedWidget->setHidden(true); }
      if (mElevonsActuatorMinAngleLineEditChangedWidget != nullptr) { mElevonsActuatorMinAngleLineEditChangedWidget->setHidden(true); }
      if (mElevonsActuatorMaxAngleLineEditChangedWidget != nullptr) { mElevonsActuatorMaxAngleLineEditChangedWidget->setHidden(true); }
   }
   else
   {
      mElevonsPresent = true;

      if (mElevonsChordFractionStartWidget != nullptr) { mElevonsChordFractionStartWidget->setHidden(false); }
      if (mElevonsChordFractionEndWidget != nullptr) { mElevonsChordFractionEndWidget->setHidden(false); }
      if (mElevonsSpanFractionStartWidget != nullptr) { mElevonsSpanFractionStartWidget->setHidden(false); }
      if (mElevonsSpanFractionEndWidget != nullptr) { mElevonsSpanFractionEndWidget->setHidden(false); }
      if (mElevonsUseExponentialAngleMappingCheckBoxChangedWidget != nullptr) { mElevonsUseExponentialAngleMappingCheckBoxChangedWidget->setHidden(false); }
      if (mElevonsControlSurfaceMinAngleLineEditChangedWidget != nullptr) { mElevonsControlSurfaceMinAngleLineEditChangedWidget->setHidden(false); }
      if (mElevonsControlSurfaceMaxAngleLineEditChangedWidget != nullptr) { mElevonsControlSurfaceMaxAngleLineEditChangedWidget->setHidden(false); }
      if (mElevonsActuatorMinRateLineEditChangedWidget != nullptr) { mElevonsActuatorMinRateLineEditChangedWidget->setHidden(false); }
      if (mElevonsActuatorMaxRateLineEditChangedWidget != nullptr) { mElevonsActuatorMaxRateLineEditChangedWidget->setHidden(false); }
      if (mElevonsActuatorMinAngleLineEditChangedWidget != nullptr) { mElevonsActuatorMinAngleLineEditChangedWidget->setHidden(false); }
      if (mElevonsActuatorMaxAngleLineEditChangedWidget != nullptr) { mElevonsActuatorMaxAngleLineEditChangedWidget->setHidden(false); }
   }
   AdjustAndRedraw(true);
}

void GeometryWing::HandleElevonsChordFractionStartLineEditChanged(QString aValue)
{
   mElevonsChordFractionStart = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleElevonsChordFractionEndLineEditChanged(QString aValue)
{
   mElevonsChordFractionEnd = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleElevonsSpanFractionStartLineEditChanged(QString aValue)
{
   mElevonsSpanFractionStart = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleElevonsSpanFractionEndLineEditChanged(QString aValue)
{
   mElevonsSpanFractionEnd = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleElevonsUseExponentialAngleMappingCheckBoxChanged(int aValue)
{
   if (aValue == 0)
   {
      mElevonsUseExponentialAngleMapping = false;
   }
   else
   {
      mElevonsUseExponentialAngleMapping = true;
   }

   AdjustAndRedraw(true);
}

void GeometryWing::HandleElevonsControlSurfaceMinAngleLineEditChanged(QString aValue)
{
   mElevonsControlSurfaceMinAngle_deg = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleElevonsControlSurfaceMaxAngleLineEditChanged(QString aValue)
{
   mElevonsControlSurfaceMaxAngle_deg = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleElevonsActuatorMinRateLineEditChanged(QString aValue)
{
   mElevonsActuatorMinRate_dps = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleElevonsActuatorMaxRateLineEditChanged(QString aValue)
{
   mElevonsActuatorMaxRate_dps = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleElevonsActuatorMinAngleLineEditChanged(QString aValue)
{
   mElevonsActuatorMinAngle_deg = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::HandleElevonsActuatorMaxAngleLineEditChanged(QString aValue)
{
   mElevonsActuatorMaxAngle_deg = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryWing::CalcMassProperties(double aTotalMass_lbs)
{
   // Set the mass
   mCalculatedMass_lbs = aTotalMass_lbs;

   // Convert mass to slugs
   double mass_slugs = mCalculatedMass_lbs * mLbsToSlugs;

   // Since we use only a single side of the wing, we use half of the mass
   mass_slugs *= 0.5;

   // In a similar way, we use half of the wing area
   double wingArea_sqft = mArea_ft2 * 0.5;

   // Set nominal ref pt
   UtVec3dX refPoint_ft = mRefPoint_ft;

   // Since we use only a single side of the wing, we use half of the span
   double span_ft = mSpan_ft * 0.5;

   // First, save the dihedral and incidence
   double dihedral_rad       = mDihedralAngle_rad;
   double incidenceAngle_rad = mIncidenceAngle_rad;

   UtVec3dX totalCGMass(0.0, 0.0, 0.0);
   double totalIxx = 0.0;
   double totalIyy = 0.0;
   double totalIzz = 0.0;

   // Now, determine location of other surface
   double currentSurfaceY = mRefPoint_ft.Y();
   double delta = currentSurfaceY - mSymmetryHorizontalY;
   double otherSurfaceY = currentSurfaceY - 2.0 * delta;

   // Clear surface inertia data
   mSurfaceInertiaSet.clear();

   // Current Wing
   GeometrySurfaceInertia surfaceInertiaMain;
   surfaceInertiaMain.SetParent(this);   // Set this surface as the parent
   mDihedralAngle_rad = dihedral_rad;
   mIncidenceAngle_rad = incidenceAngle_rad;
   refPoint_ft.Set(mRefPoint_ft.X(), currentSurfaceY, mRefPoint_ft.Z());
   CalcSingleSurfaceMassProperties(surfaceInertiaMain, mass_slugs, span_ft, wingArea_sqft, refPoint_ft);
   mSurfaceInertiaSet.push_back(surfaceInertiaMain);
   totalIxx += mCalculatedIxx;
   totalIyy += mCalculatedIyy;
   totalIzz += mCalculatedIzz;
   totalCGMass += (mCalculatedCG_ft * mass_slugs);

   // Other Wing
   GeometrySurfaceInertia surfaceInertiaSymmetric;
   surfaceInertiaSymmetric.SetParent(this);   // Set this surface as the parent
   mDihedralAngle_rad = UtMath::cPI - dihedral_rad;
   mIncidenceAngle_rad = -incidenceAngle_rad;
   refPoint_ft.Set(mRefPoint_ft.X(), otherSurfaceY, mRefPoint_ft.Z());
   CalcSingleSurfaceMassProperties(surfaceInertiaSymmetric, mass_slugs, span_ft, wingArea_sqft, refPoint_ft);
   mSurfaceInertiaSet.push_back(surfaceInertiaSymmetric);
   totalIxx += mCalculatedIxx;
   totalIyy += mCalculatedIyy;
   totalIzz += mCalculatedIzz;
   totalCGMass += (mCalculatedCG_ft * mass_slugs);

   mCalculatedCG_ft = totalCGMass / (2.0 * mass_slugs);
   mCalculatedIxx = totalIxx;
   mCalculatedIyy = totalIyy;
   mCalculatedIzz = totalIzz;

   // Restore the dihedral and incidence
   mDihedralAngle_rad  = dihedral_rad;
   mIncidenceAngle_rad = incidenceAngle_rad;
}

double GeometryWing::GetMassArea_ft2()
{
   double averageChord = 0.5 * (mRootChord_ft + mTipChord_ft);
   return 2.0 * (mSpan_ft * 0.5) * averageChord;
}

double GeometryWing::GetFinSpan()
{
   return mSpan_ft * 0.5;
}

bool GeometryWing::CalcExtents(double& xMin, double& xMax, double& yMin, double& yMax, double& zMin, double& zMax)
{
   return CalcExtentsForSurface(xMin, xMax, yMin, yMax, zMin, zMax, mSpan_ft * 0.5, "Horizontal");
}

QStringList GeometryWing::Audit()
{
   QStringList errorList = GeometrySurface::Audit();

   // Ailerons

   if (mAileronsPresent)
   {
      if ((mAileronsChordFractionEnd - mAileronsChordFractionStart) <= std::numeric_limits<double>::epsilon())
      {
         QString errorString("Ailerons require a control chord end fraction aft of control chord start");
         errorList.push_back(errorString);
      }

      if ((mAileronsSpanFractionEnd - mAileronsSpanFractionStart) <= std::numeric_limits<double>::epsilon())
      {
         QString errorString("Ailerons require a control span end fraction outboard of control span start");
         errorList.push_back(errorString);
      }
   }

   // Spoilerons

   if (mSpoileronsPresent)
   {
      if ((mSpoileronsChordFractionEnd - mSpoileronsChordFractionStart) <= std::numeric_limits<double>::epsilon())
      {
         QString errorString("Spoilerons require a control chord end fraction aft of control chord start");
         errorList.push_back(errorString);
      }

      if ((mSpoileronsSpanFractionEnd - mSpoileronsSpanFractionStart) <= std::numeric_limits<double>::epsilon())
      {
         QString errorString("Spoilerons require a control span end fraction outboard of control span start");
         errorList.push_back(errorString);
      }
   }

   // Spoilers

   if (mSpoilersPresent)
   {
      if ((mSpoilersChordFractionEnd - mSpoilersChordFractionStart) <= std::numeric_limits<double>::epsilon())
      {
         QString errorString("Spoilers require a control chord end fraction aft of control chord start");
         errorList.push_back(errorString);
      }

      if ((mSpoilersSpanFractionEnd - mSpoilersSpanFractionStart) <= std::numeric_limits<double>::epsilon())
      {
         QString errorString("Spoilers require a control span end fraction outboard of control span start");
         errorList.push_back(errorString);
      }
   }

   // Dragerons

   if (mDrageronsPresent)
   {
      if ((mDrageronsChordFractionEnd - mDrageronsChordFractionStart) <= std::numeric_limits<double>::epsilon())
      {
         QString errorString("Dragerons require a control chord end fraction aft of control chord start");
         errorList.push_back(errorString);
      }

      if ((mDrageronsSpanFractionEnd - mDrageronsSpanFractionStart) <= std::numeric_limits<double>::epsilon())
      {
         QString errorString("Dragerons require a control span end fraction outboard of control span start");
         errorList.push_back(errorString);
      }
   }

   // Elevons

   if (mElevonsPresent)
   {
      if ((mElevonsChordFractionEnd - mElevonsChordFractionStart) <= std::numeric_limits<double>::epsilon())
      {
         QString errorString("Elevons require a control chord end fraction aft of control chord start");
         errorList.push_back(errorString);
      }

      if ((mElevonsSpanFractionEnd - mElevonsSpanFractionStart) <= std::numeric_limits<double>::epsilon())
      {
         QString errorString("Elevons require a control span end fraction outboard of control span start");
         errorList.push_back(errorString);
      }
   }


   return errorList;
}

} // namespace Designer
