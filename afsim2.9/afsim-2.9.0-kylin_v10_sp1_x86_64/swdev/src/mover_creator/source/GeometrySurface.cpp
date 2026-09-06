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

#include "GeometrySurface.hpp"

#include <QCheckBox>
#include <QDir>
#include <QLabel>
#include <QMessageBox>
#include <QTreeWidget>

#include "AirfoilWidget.hpp"
#include "GeometryTriangle.hpp"
#include "MoverCreatorLineEdit.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "P6DofUtils.hpp"
#include "ui_GeometryWidget.h"
#include "UtDCM.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"

namespace Designer
{

GeometrySurface::GeometrySurface(Vehicle* aVehicle)
   : GeometryObject(aVehicle)
   , mSymmetryString("Single")
   , mSurfaceInertiaSet()
{
   // Overwrite the GeometryObject type
   mGeometryObjectTypeString.assign("GeometrySurface");

   // This will use its own connections for the reference point line edits
   mUseDefaultConnectLineEdits = false;
}

GeometrySurface::~GeometrySurface()
{
}

void GeometrySurface::MoveRefPoint(UtVec3dX aMoveDelta_ft)
{
   mSymmetryHorizontalY += aMoveDelta_ft.Y();
   mSymmetryVerticalZ += aMoveDelta_ft.Z();

   UtVec3dX newRefPoint_ft = mRefPoint_ft + aMoveDelta_ft;
   SetRefPoint(newRefPoint_ft);
}

QTreeWidgetItem* GeometrySurface::CreateTreeWidgetItem(QString&     aName,
                                                       QTreeWidget* aTreeWidgetPtr,
                                                       bool         aMajorControlSurface)
{
   // Create the tree widget item at the GeometryObject level
   QTreeWidgetItem* treeWidgetItem =
      GeometryObject::CreateTreeWidgetItem(aName, aTreeWidgetPtr, aMajorControlSurface);

   // Enforce the geometry type
   mGeometryObjectTypeString = "GeometrySurface";

   // Set Qt flags
   if (aMajorControlSurface)
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

   QTreeWidgetItem* span = new QTreeWidgetItem(treeWidgetItem, QStringList("Span"));
   span->setToolTip(0, "The span of the surface in feet");
   auto spanLineEdit = new MoverCreatorLineEdit(span);
   spanLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   spanLineEdit->setFrame(false);
   spanLineEdit->setText(QString::number(mSpan_ft));
   aTreeWidgetPtr->setItemWidget(span, 1, spanLineEdit);
   aTreeWidgetPtr->setItemWidget(span, 2, new QLabel(QString::fromStdString("ft")));
   connect(spanLineEdit, &QLineEdit::textChanged, this, &GeometrySurface::HandleSpanLineEditChanged);
   connect(spanLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* rootChord = new QTreeWidgetItem(treeWidgetItem, QStringList("Root Chord"));
   rootChord->setToolTip(0, "The root chord of the surface in feet");
   auto rootChordLineEdit = new MoverCreatorLineEdit(rootChord);
   rootChordLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   rootChordLineEdit->setFrame(false);
   rootChordLineEdit->setText(QString::number(mRootChord_ft));
   aTreeWidgetPtr->setItemWidget(rootChord, 1, rootChordLineEdit);
   aTreeWidgetPtr->setItemWidget(rootChord, 2, new QLabel(QString::fromStdString("ft")));
   connect(rootChordLineEdit, &QLineEdit::textChanged, this, &GeometrySurface::HandleRootChordLineEditChanged);
   connect(rootChordLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* tipChord = new QTreeWidgetItem(treeWidgetItem, QStringList("Tip Chord"));
   tipChord->setToolTip(0, "The tip chord of the surface in feet");
   auto tipChordLineEdit = new MoverCreatorLineEdit(tipChord);
   tipChordLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   tipChordLineEdit->setFrame(false);
   tipChordLineEdit->setText(QString::number(mTipChord_ft));
   aTreeWidgetPtr->setItemWidget(tipChord, 1, tipChordLineEdit);
   aTreeWidgetPtr->setItemWidget(tipChord, 2, new QLabel(QString::fromStdString("ft")));
   connect(tipChordLineEdit, &QLineEdit::textChanged, this, &GeometrySurface::HandleTipChordLineEditChanged);
   connect(tipChordLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* calculatedArea = new QTreeWidgetItem(treeWidgetItem, QStringList("Calculated Area"));
   calculatedArea->setToolTip(0, "The calculated area of the surface in square feet");
   auto calculatedAreaLineEdit = new MoverCreatorLineEdit(calculatedArea);
   calculatedAreaLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   calculatedAreaLineEdit->setFrame(false);
   calculatedAreaLineEdit->setText(QString::number(mArea_ft2));
   aTreeWidgetPtr->setItemWidget(calculatedArea, 1, calculatedAreaLineEdit);
   aTreeWidgetPtr->setItemWidget(calculatedArea, 2, new QLabel(QString::fromStdString("sq ft")));
   calculatedAreaLineEdit->setReadOnly(true);
   connect(calculatedAreaLineEdit, &QLineEdit::textChanged, this, &GeometrySurface::HandleCalcAreaLineEditChanged);
   connect(calculatedAreaLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mCalculatedAreaLineEditPtr = calculatedAreaLineEdit;

   QTreeWidgetItem* calculatedAspectRatio = new QTreeWidgetItem(treeWidgetItem, QStringList("Calculated Aspect Ratio"));
   calculatedAspectRatio->setToolTip(0, "The calculated aspect ratio of the surface");
   auto calculatedAspectRatioLineEdit = new MoverCreatorLineEdit(calculatedAspectRatio);
   calculatedAspectRatioLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   calculatedAspectRatioLineEdit->setFrame(false);
   calculatedAspectRatioLineEdit->setText(QString::number(mAspectRatio));
   aTreeWidgetPtr->setItemWidget(calculatedAspectRatio, 1, calculatedAspectRatioLineEdit);
   aTreeWidgetPtr->setItemWidget(calculatedAspectRatio, 2, new QLabel(QString::fromStdString("")));
   calculatedAspectRatioLineEdit->setReadOnly(true);
   connect(calculatedAspectRatioLineEdit, &QLineEdit::textChanged, this, &GeometrySurface::HandleCalcAspectRatioLineEditChanged);
   connect(calculatedAspectRatioLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mCalculatedAspectRatioLineEditPtr = calculatedAspectRatioLineEdit;

   QTreeWidgetItem* useManualAspectRatio = new QTreeWidgetItem(treeWidgetItem, QStringList("Use Manual Aspect Ratio"));
   useManualAspectRatio->setToolTip(0, "If checked, the surface will assume a user-specified aspect ratio");
   QCheckBox* useManualAspectRatioCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(useManualAspectRatio, 1, useManualAspectRatioCheckBox);
   aTreeWidgetPtr->setItemWidget(useManualAspectRatio, 2, new QLabel(QString::fromStdString("")));
   connect(useManualAspectRatioCheckBox, &QCheckBox::stateChanged, this, &GeometrySurface::HandleUseManualAspectRatioCheckBoxChanged);
   mUseManualAspectRatioWidget = useManualAspectRatio;

   QTreeWidgetItem* manualAspectRatio = new QTreeWidgetItem(treeWidgetItem, QStringList("Manual Aspect Ratio"));
   manualAspectRatio->setToolTip(0, "This is a user-specified aspect ratio");
   auto manualAspectRatioLineEdit = new MoverCreatorLineEdit(manualAspectRatio);
   manualAspectRatioLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   manualAspectRatioLineEdit->setFrame(false);
   manualAspectRatioLineEdit->setText(QString::number(mManualAspectRatio));
   aTreeWidgetPtr->setItemWidget(manualAspectRatio, 1, manualAspectRatioLineEdit);
   aTreeWidgetPtr->setItemWidget(manualAspectRatio, 2, new QLabel(QString::fromStdString("")));
   connect(manualAspectRatioLineEdit, &QLineEdit::textChanged, this, &GeometrySurface::HandleManualAspectRatioLineEditChanged);
   connect(manualAspectRatioLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mManualAspectRatioWidget = manualAspectRatio;
   mManualAspectRatioWidget->setHidden(true);

   QTreeWidgetItem* sweepAngle = new QTreeWidgetItem(treeWidgetItem, QStringList("Sweep Angle"));
   sweepAngle->setToolTip(0, "The leading edge sweep angle of the surface in degrees");
   auto sweepAngleLineEdit = new MoverCreatorLineEdit(sweepAngle);
   sweepAngleLineEdit->setFrame(false);
   sweepAngleLineEdit->setText(QString::number(mSweepAngle_rad));
   aTreeWidgetPtr->setItemWidget(sweepAngle, 1, sweepAngleLineEdit);
   aTreeWidgetPtr->setItemWidget(sweepAngle, 2, new QLabel(QString::fromStdString("deg")));
   connect(sweepAngleLineEdit, &QLineEdit::textChanged, this, &GeometrySurface::HandleSweepAngleLineEditChanged);
   connect(sweepAngleLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* dihedralAngle = new QTreeWidgetItem(treeWidgetItem, QStringList("Dihedral Angle"));
   dihedralAngle->setToolTip(0, "The dihedral angle of the surface in degrees");
   auto dihedralAngleLineEdit = new MoverCreatorLineEdit(dihedralAngle);
   mDihedralAngleLineEdit = dihedralAngleLineEdit;
   dihedralAngleLineEdit->setFrame(false);
   dihedralAngleLineEdit->setText(QString::number(mDihedralAngle_rad));
   aTreeWidgetPtr->setItemWidget(dihedralAngle, 1, dihedralAngleLineEdit);
   aTreeWidgetPtr->setItemWidget(dihedralAngle, 2, new QLabel(QString::fromStdString("deg")));
   connect(dihedralAngleLineEdit, &QLineEdit::textChanged, this, &GeometrySurface::HandleDihedralAngleLineEditChanged);
   connect(dihedralAngleLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mDihedralWidget = dihedralAngle;

   QTreeWidgetItem* incidenceAngle = new QTreeWidgetItem(treeWidgetItem, QStringList("Incidence Angle"));
   incidenceAngle->setToolTip(0, "The incidence angle of the surface in degrees");
   auto incidenceAngleLineEdit = new MoverCreatorLineEdit(incidenceAngle);
   incidenceAngleLineEdit->setFrame(false);
   incidenceAngleLineEdit->setText(QString::number(mIncidenceAngle_rad));
   aTreeWidgetPtr->setItemWidget(incidenceAngle, 1, incidenceAngleLineEdit);
   aTreeWidgetPtr->setItemWidget(incidenceAngle, 2, new QLabel(QString::fromStdString("deg")));
   connect(incidenceAngleLineEdit, &QLineEdit::textChanged, this, &GeometrySurface::HandleIncidenceAngleLineEditChanged);
   connect(incidenceAngleLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* thickness = new QTreeWidgetItem(treeWidgetItem, QStringList("Thickness Ratio"));
   thickness->setToolTip(0, "The thickness ratio of the surface");
   auto thicknessLineEdit = new MoverCreatorLineEdit(thickness);
   thicknessLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   thicknessLineEdit->setFrame(false);
   thicknessLineEdit->setText(QString::number(mThicknessRatio));
   aTreeWidgetPtr->setItemWidget(thickness, 1, thicknessLineEdit);
   aTreeWidgetPtr->setItemWidget(thickness, 2, new QLabel(QString::fromStdString("")));
   connect(thicknessLineEdit, &QLineEdit::textChanged, this, &GeometrySurface::HandleThicknessLineEditChanged);
   connect(thicknessLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* airfoil = new QTreeWidgetItem(treeWidgetItem, QStringList("Airfoil"));
   airfoil->setToolTip(0, "The 2D airfoil for this surface");
   mAirfoilComboBox = new QComboBox();
   PopulateAirfoilList();
   mAirfoilComboBox->setFrame(false);
   aTreeWidgetPtr->setItemWidget(airfoil, 1, mAirfoilComboBox);
   connect(mAirfoilComboBox, &QComboBox::currentTextChanged, this, [this, aTreeWidgetPtr, airfoil]()
   {
      aTreeWidgetPtr->setCurrentItem(airfoil);
      AdjustAndRedraw(true);
   });

   QWidget* btnWidget = new QWidget();
   QHBoxLayout* btnWidgetLayout = new QHBoxLayout();
   btnWidgetLayout->setSpacing(0);
   btnWidgetLayout->setContentsMargins(0, 0, 0, 0);
   btnWidget->setLayout(btnWidgetLayout);
   QIcon editIcon;
   QString iconThemeName = QStringLiteral("open_folder");
   editIcon = QIcon::fromTheme(iconThemeName);
   QPushButton* editButton = new QPushButton("");
   editButton->setIcon(editIcon);
   editButton->setToolTip("Browse Airfoils");

   QSpacerItem* horizontalSpacer = new QSpacerItem(100, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
   btnWidgetLayout->addWidget(editButton);
   btnWidgetLayout->addItem(horizontalSpacer);
   aTreeWidgetPtr->setItemWidget(airfoil, 2, btnWidget);
   connect(editButton, static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked), this, &GeometrySurface::HandleOpenAirfoil);

   QTreeWidgetItem* invertAirfoil = new QTreeWidgetItem(treeWidgetItem, QStringList("Invert Airfoil"));
   auto invertAirfoilCheckBox = new QCheckBox();

   // Temporary!
   invertAirfoil->setDisabled(true);
   invertAirfoilCheckBox->setDisabled(true);
   invertAirfoil->setToolTip(0, "Airfoil inversion will be added in the future");

   aTreeWidgetPtr->setItemWidget(invertAirfoil, 1, invertAirfoilCheckBox);
   aTreeWidgetPtr->setItemWidget(invertAirfoil, 2, new QLabel(QString::fromStdString("")));

   QTreeWidgetItem* oswaldEfficiency = new QTreeWidgetItem(treeWidgetItem, QStringList("Oswald's Efficiency Factor"));
   oswaldEfficiency->setToolTip(0, "Oswald's Efficiency Factor for the surface");
   auto oswaldEfficiencyLineEdit = new MoverCreatorLineEdit(oswaldEfficiency);
   oswaldEfficiencyLineEdit->setValidator(new QDoubleValidator(0.0, 1.0, 10));
   oswaldEfficiencyLineEdit->setFrame(false);
   oswaldEfficiencyLineEdit->setText(QString::number(mOswaldsEfficiency));
   aTreeWidgetPtr->setItemWidget(oswaldEfficiency, 1, oswaldEfficiencyLineEdit);
   aTreeWidgetPtr->setItemWidget(oswaldEfficiency, 2, new QLabel(QString::fromStdString("")));
   connect(oswaldEfficiencyLineEdit, &QLineEdit::textChanged, this, &GeometrySurface::HandleOswaldEfficiencyLineEditChanged);
   connect(oswaldEfficiencyLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   if (aMajorControlSurface)
   {
      QTreeWidgetItem* controlMotion = new QTreeWidgetItem(treeWidgetItem, QStringList("Control Motion"));
      controlMotion->setToolTip(0, "The control motion type for the surface. May be either control surface or full-flying.");
      QComboBox* controlMotionComboBox = new QComboBox();
      QStringList comboBoxListStrings;
      comboBoxListStrings << "Control Surface" << "Full-Flying";
      controlMotionComboBox->addItems(comboBoxListStrings);
      aTreeWidgetPtr->setItemWidget(controlMotion, 1, controlMotionComboBox);
      aTreeWidgetPtr->setItemWidget(controlMotion, 2, new QLabel(QString::fromStdString("")));
      connect(controlMotionComboBox, &QComboBox::currentTextChanged, this, [this, aTreeWidgetPtr, controlMotion](QString aControlMotion)
      {
         aTreeWidgetPtr->setCurrentItem(controlMotion);
         HandleSurfaceControlMotionComboBoxChanged(aControlMotion);
      });
      controlMotionComboBox->setCurrentIndex(1); // Start with Full-Flying
      mSurfaceControlMotionWidget = controlMotion;

      QTreeWidgetItem* controlSurfaceChordStartFraction = new QTreeWidgetItem(treeWidgetItem, QStringList("Chord Fraction, Start"));
      controlSurfaceChordStartFraction->setToolTip(0, "This is the start location of the control surface, from the leading edge");
      auto controlSurfaceChordFractionStartLineEdit = new MoverCreatorLineEdit(controlSurfaceChordStartFraction);
      controlSurfaceChordFractionStartLineEdit->setFrame(false);
      controlSurfaceChordFractionStartLineEdit->setText(QString::number(mControlSurfaceChordFractionStart));
      aTreeWidgetPtr->setItemWidget(controlSurfaceChordStartFraction, 1, controlSurfaceChordFractionStartLineEdit);
      aTreeWidgetPtr->setItemWidget(controlSurfaceChordStartFraction, 2, new QLabel(QString::fromStdString("")));
      connect(controlSurfaceChordFractionStartLineEdit, &QLineEdit::textChanged, this, &GeometrySurface::HandleControlSurfaceChordFractionStartLineEditChanged);
      connect(controlSurfaceChordFractionStartLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
      mControlSurfaceChordFractionStartWidget = controlSurfaceChordStartFraction;
      mControlSurfaceChordFractionStartWidget->setHidden(true); // Start hidden

      QTreeWidgetItem* controlSurfaceChordEndFraction = new QTreeWidgetItem(treeWidgetItem, QStringList("Chord Fraction, End"));
      controlSurfaceChordEndFraction->setToolTip(0, "This is the end location of the control surface, from the leading edge");
      auto controlSurfaceChordFractionEndLineEdit = new MoverCreatorLineEdit(controlSurfaceChordEndFraction);
      controlSurfaceChordFractionEndLineEdit->setFrame(false);
      controlSurfaceChordFractionEndLineEdit->setText(QString::number(mControlSurfaceChordFractionEnd));
      aTreeWidgetPtr->setItemWidget(controlSurfaceChordEndFraction, 1, controlSurfaceChordFractionEndLineEdit);
      aTreeWidgetPtr->setItemWidget(controlSurfaceChordEndFraction, 2, new QLabel(QString::fromStdString("")));
      connect(controlSurfaceChordFractionEndLineEdit, &QLineEdit::textChanged, this, &GeometrySurface::HandleControlSurfaceChordFractionEndLineEditChanged);
      connect(controlSurfaceChordFractionEndLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
      mControlSurfaceChordFractionEndWidget = controlSurfaceChordEndFraction;
      mControlSurfaceChordFractionEndWidget->setHidden(true); // Start hidden

      QTreeWidgetItem* controlSurfaceSpanStartFraction = new QTreeWidgetItem(treeWidgetItem, QStringList("Span Fraction, Start"));
      controlSurfaceSpanStartFraction->setToolTip(0, "This is the start location of the control surface, from the root");
      auto controlSurfaceSpanFractionStartLineEdit = new MoverCreatorLineEdit(controlSurfaceSpanStartFraction);
      controlSurfaceSpanFractionStartLineEdit->setFrame(false);
      controlSurfaceSpanFractionStartLineEdit->setText(QString::number(mControlSurfaceSpanFractionStart));
      aTreeWidgetPtr->setItemWidget(controlSurfaceSpanStartFraction, 1, controlSurfaceSpanFractionStartLineEdit);
      aTreeWidgetPtr->setItemWidget(controlSurfaceSpanStartFraction, 2, new QLabel(QString::fromStdString("")));
      connect(controlSurfaceSpanFractionStartLineEdit, &QLineEdit::textChanged, this, &GeometrySurface::HandleControlSurfaceSpanFractionStartLineEditChanged);
      connect(controlSurfaceSpanFractionStartLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
      mControlSurfaceSpanFractionStartWidget = controlSurfaceSpanStartFraction;
      mControlSurfaceSpanFractionStartWidget->setHidden(true); // Start hidden

      QTreeWidgetItem* controlSurfaceSpanEndFraction = new QTreeWidgetItem(treeWidgetItem, QStringList("Span Fraction, End"));
      controlSurfaceSpanEndFraction->setToolTip(0, "This is the end location of the control surface, from the root");
      auto controlSurfaceSpanFractionEndLineEdit = new MoverCreatorLineEdit(controlSurfaceSpanEndFraction);
      controlSurfaceSpanFractionEndLineEdit->setFrame(false);
      controlSurfaceSpanFractionEndLineEdit->setText(QString::number(mControlSurfaceSpanFractionEnd));
      aTreeWidgetPtr->setItemWidget(controlSurfaceSpanEndFraction, 1, controlSurfaceSpanFractionEndLineEdit);
      aTreeWidgetPtr->setItemWidget(controlSurfaceSpanEndFraction, 2, new QLabel(QString::fromStdString("")));
      connect(controlSurfaceSpanFractionEndLineEdit, &QLineEdit::textChanged, this, &GeometrySurface::HandleControlSurfaceSpanFractionEndLineEditChanged);
      connect(controlSurfaceSpanFractionEndLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
      mControlSurfaceSpanFractionEndWidget = controlSurfaceSpanEndFraction;
      mControlSurfaceSpanFractionEndWidget->setHidden(true); // Start hidden

      QTreeWidgetItem* controlSurfaceMinAngle = new QTreeWidgetItem(treeWidgetItem, QStringList("Control Surface Min Angle"));
      controlSurfaceMinAngle->setToolTip(0, "The minimum angle for this control surface in degrees");
      auto controlSurfaceMinAngleLineEdit = new MoverCreatorLineEdit(controlSurfaceMinAngle);
      controlSurfaceMinAngleLineEdit->setFrame(false);
      controlSurfaceMinAngleLineEdit->setText(QString::number(mControlSurfaceMinAngle_deg));
      aTreeWidgetPtr->setItemWidget(controlSurfaceMinAngle, 1, controlSurfaceMinAngleLineEdit);
      aTreeWidgetPtr->setItemWidget(controlSurfaceMinAngle, 2, new QLabel(QString::fromStdString("deg")));
      connect(controlSurfaceMinAngleLineEdit, &QLineEdit::textChanged, this, &GeometrySurface::HandleControlSurfaceMinAngleLineEditChanged);
      connect(controlSurfaceMinAngleLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
      mControlSurfaceMinAngleWidget = controlSurfaceMinAngle;

      QTreeWidgetItem* controlSurfaceMaxAngle = new QTreeWidgetItem(treeWidgetItem, QStringList("Control Surface Max Angle"));
      controlSurfaceMaxAngle->setToolTip(0, "The maximum angle for this control surface in degrees");
      auto controlSurfaceMaxAngleLineEdit = new MoverCreatorLineEdit(controlSurfaceMaxAngle);
      controlSurfaceMaxAngleLineEdit->setFrame(false);
      controlSurfaceMaxAngleLineEdit->setText(QString::number(mControlSurfaceMaxAngle_deg));
      aTreeWidgetPtr->setItemWidget(controlSurfaceMaxAngle, 1, controlSurfaceMaxAngleLineEdit);
      aTreeWidgetPtr->setItemWidget(controlSurfaceMaxAngle, 2, new QLabel(QString::fromStdString("deg")));
      connect(controlSurfaceMaxAngleLineEdit, &QLineEdit::textChanged, this, &GeometrySurface::HandleControlSurfaceMaxAngleLineEditChanged);
      connect(controlSurfaceMaxAngleLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
      mControlSurfaceMaxAngleWidget = controlSurfaceMaxAngle;

      QTreeWidgetItem* expoAngleMapping = new QTreeWidgetItem(treeWidgetItem, QStringList("Enable Expo Angle Mapping"));
      expoAngleMapping->setToolTip(0, "If checked, this surface will use exponential angle mapping");
      QCheckBox* expoAngleMappingCheckBox = new QCheckBox();
      aTreeWidgetPtr->setItemWidget(expoAngleMapping, 1, expoAngleMappingCheckBox);
      aTreeWidgetPtr->setItemWidget(expoAngleMapping, 2, new QLabel(QString::fromStdString("")));
      connect(expoAngleMappingCheckBox, &QCheckBox::stateChanged, this, &GeometrySurface::HandleExponentialAngleMappingCheckBoxChanged);
      mExponentialAngleMappingWidget = expoAngleMapping;
      mExponentialAngleMappingWidget->setHidden(true); // Start hidden

      QTreeWidgetItem* actuatorMinRate = new QTreeWidgetItem(treeWidgetItem, QStringList("Actuator Min Rate"));
      actuatorMinRate->setToolTip(0, "The minimum rate of the surface actuator in degrees/second");
      auto actuatorMinRateLineEdit = new MoverCreatorLineEdit(actuatorMinRate);
      actuatorMinRateLineEdit->setFrame(false);
      actuatorMinRateLineEdit->setText(QString::number(mActuatorMinRate_dps));
      aTreeWidgetPtr->setItemWidget(actuatorMinRate, 1, actuatorMinRateLineEdit);
      aTreeWidgetPtr->setItemWidget(actuatorMinRate, 2, new QLabel(QString::fromStdString("deg/sec")));
      connect(actuatorMinRateLineEdit, &QLineEdit::textChanged, this, &GeometrySurface::HandleActuatorMinRateLineEditChanged);
      connect(actuatorMinRateLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
      mActuatorMinRateWidget = actuatorMinRate;

      QTreeWidgetItem* actuatorMaxRate = new QTreeWidgetItem(treeWidgetItem, QStringList("Actuator Max Rate"));
      actuatorMaxRate->setToolTip(0, "The maximum rate of the surface actuator in degrees/second");
      auto actuatorMaxRateLineEdit = new MoverCreatorLineEdit(actuatorMaxRate);
      actuatorMaxRateLineEdit->setFrame(false);
      actuatorMaxRateLineEdit->setText(QString::number(mActuatorMaxRate_dps));
      aTreeWidgetPtr->setItemWidget(actuatorMaxRate, 1, actuatorMaxRateLineEdit);
      aTreeWidgetPtr->setItemWidget(actuatorMaxRate, 2, new QLabel(QString::fromStdString("deg/sec")));
      connect(actuatorMaxRateLineEdit, &QLineEdit::textChanged, this, &GeometrySurface::HandleActuatorMaxRateLineEditChanged);
      connect(actuatorMaxRateLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
      mActuatorMaxRateWidget = actuatorMaxRate;

      QTreeWidgetItem* actuatorMinAngle = new QTreeWidgetItem(treeWidgetItem, QStringList("Actuator Min Angle"));
      actuatorMinAngle->setToolTip(0, "The minimum angle of the surface actuator in degrees");
      auto actuatorMinAngleLineEdit = new MoverCreatorLineEdit(actuatorMinAngle);
      actuatorMinAngleLineEdit->setFrame(false);
      actuatorMinAngleLineEdit->setText(QString::number(mActuatorMinAngle_deg));
      aTreeWidgetPtr->setItemWidget(actuatorMinAngle, 1, actuatorMinAngleLineEdit);
      aTreeWidgetPtr->setItemWidget(actuatorMinAngle, 2, new QLabel(QString::fromStdString("deg")));
      connect(actuatorMinAngleLineEdit, &QLineEdit::textChanged, this, &GeometrySurface::HandleActuatorMinAngleLineEditChanged);
      connect(actuatorMinAngleLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
      mActuatorMinAngleWidget = actuatorMinAngle;

      QTreeWidgetItem* actuatorMaxAngle = new QTreeWidgetItem(treeWidgetItem, QStringList("Actuator Max Angle"));
      actuatorMaxAngle->setToolTip(0, "The maximum angle of the surface actuator in degrees");
      auto actuatorMaxAngleLineEdit = new MoverCreatorLineEdit(actuatorMaxAngle);
      actuatorMaxAngleLineEdit->setFrame(false);
      actuatorMaxAngleLineEdit->setText(QString::number(mActuatorMaxAngle_deg));
      aTreeWidgetPtr->setItemWidget(actuatorMaxAngle, 1, actuatorMaxAngleLineEdit);
      aTreeWidgetPtr->setItemWidget(actuatorMaxAngle, 2, new QLabel(QString::fromStdString("deg")));
      connect(actuatorMaxAngleLineEdit, &QLineEdit::textChanged, this, &GeometrySurface::HandleActuatorMaxAngleLineEditChanged);
      connect(actuatorMaxAngleLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
      mActuatorMaxAngleWidget = actuatorMaxAngle;
   }

   QTreeWidgetItem* quadControlFins = new QTreeWidgetItem(treeWidgetItem, QStringList("Quad Control Fins"));
   quadControlFins->setToolTip(0, "If checked, this surface will have four control fins");
   QCheckBox* quadControlFinsCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(quadControlFins, 1, quadControlFinsCheckBox);
   aTreeWidgetPtr->setItemWidget(quadControlFins, 2, new QLabel(QString::fromStdString("")));
   connect(quadControlFinsCheckBox, &QCheckBox::stateChanged, this, &GeometrySurface::HandleQuadControlFinsCheckBoxChanged);
   quadControlFins->setHidden(true); // Start hidden

   QTreeWidgetItem* finPattern = new QTreeWidgetItem(treeWidgetItem, QStringList("Quad Control Fins Pattern"));
   finPattern->setToolTip(0, "The pattern of the control fins. May be either a + Pattern or a X Pattern.");
   mQuadControlFinsPatternWidget = finPattern;
   QComboBox* finPatternCB = new QComboBox();
   mQuadControlFinsPatternComboBox = finPatternCB;
   QStringList finPatternStrings;
   finPatternStrings << "+ Pattern" << "X Pattern";
   finPatternCB->addItems(finPatternStrings);
   aTreeWidgetPtr->setItemWidget(finPattern, 1, finPatternCB);
   aTreeWidgetPtr->setItemWidget(finPattern, 2, new QLabel(QString::fromStdString("")));
   connect(finPatternCB, &QComboBox::currentTextChanged, this, [this, aTreeWidgetPtr, finPattern](QString aFinPattern)
   {
      aTreeWidgetPtr->setCurrentItem(finPattern);
      HandleQuadControlFinsPatternComboBoxChanged(aFinPattern);
   });
   mQuadControlFinsPatternWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* symmetryNoChange = new QTreeWidgetItem(treeWidgetItem, QStringList("Symmetry Cannot Be Changed"));
   symmetryNoChange->setToolTip(0, "If checked, the symmetry type of these fins cannot be changed");
   QCheckBox* symmetryNoChangeCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(symmetryNoChange, 1, symmetryNoChangeCheckBox);
   aTreeWidgetPtr->setItemWidget(symmetryNoChange, 2, new QLabel(QString::fromStdString("")));
   connect(symmetryNoChangeCheckBox, &QCheckBox::stateChanged, this, &GeometrySurface::HandleSymmetryCannotBeChangedCheckBoxChanged);
   symmetryNoChange->setHidden(true); // Start hidden

   QTreeWidgetItem* symmetryItem = new QTreeWidgetItem(treeWidgetItem, QStringList("Symmetry Type"));
   symmetryItem->setToolTip(0, "The type of symmetry that the fins utilize. May be horizontal, vertical, + Pattern, or X Pattern.");
   mSymmetricalWidgetItem = symmetryItem;
   QStringList symmetryStrings;
   
   if (aName == "Vertical Tail")
   {
      symmetryStrings << "Single" << "Horizontal";
   }
   else if (aName == "Ventral Tail")
   {
      symmetryStrings << "Single" << "Horizontal";
   }
   else
   {
      symmetryStrings << "Single" << "Horizontal" << "Vertical" << "+ Pattern" << "X Pattern";
   }
   QComboBox* symmetryCB = new QComboBox();
   symmetryCB->addItems(symmetryStrings);
   mSymmetricalStringComboBox = symmetryCB;
   aTreeWidgetPtr->setItemWidget(symmetryItem, 1, symmetryCB);
   aTreeWidgetPtr->setItemWidget(symmetryItem, 2, new QLabel(QString::fromStdString("")));
   connect(symmetryCB, &QComboBox::currentTextChanged, this, [this, aTreeWidgetPtr, symmetryItem](QString aSymmetry)
   {
      aTreeWidgetPtr->setCurrentItem(symmetryItem);
      HandleSymmetricalComboBoxChanged(aSymmetry);
   });


   QTreeWidgetItem* finRefRadius = new QTreeWidgetItem(treeWidgetItem, QStringList("Fin Ref Radius"));
   finRefRadius->setToolTip(0, "The reference radius of the control fins in feet");
   auto finRefRadiusLineEdit = new MoverCreatorLineEdit(finRefRadius);
   finRefRadiusLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   mFinRefRadiusLineEditPtr = finRefRadiusLineEdit;
   finRefRadiusLineEdit->setFrame(false);
   finRefRadiusLineEdit->setText(QString::number(mFinRefRadius_ft));
   aTreeWidgetPtr->setItemWidget(finRefRadius, 1, finRefRadiusLineEdit);
   aTreeWidgetPtr->setItemWidget(finRefRadius, 2, new QLabel(QString::fromStdString("ft")));
   connect(finRefRadiusLineEdit, &QLineEdit::textChanged, this, &GeometrySurface::HandleFinRefRadiusLineEditChanged);
   connect(finRefRadiusLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mFinRefRadiusWidget = finRefRadius;
   mFinRefRadiusWidget->setHidden(true); // Start hidden

   QTreeWidgetItem* massFactor = new QTreeWidgetItem(treeWidgetItem, QStringList("Mass Factor"));
   massFactor->setToolTip(0, "The mass factor multiplier for the surface");
   auto massFactorLineEdit = new MoverCreatorLineEdit(massFactor);
   massFactorLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   massFactorLineEdit->setFrame(false);
   massFactorLineEdit->setText(QString::number(mMassFactor));
   aTreeWidgetPtr->setItemWidget(massFactor, 1, massFactorLineEdit);
   aTreeWidgetPtr->setItemWidget(massFactor, 2, new QLabel(QString::fromStdString("")));
   connect(massFactorLineEdit, &QLineEdit::textChanged, this, &GeometrySurface::HandleMassFactorLineEditChanged);
   connect(massFactorLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   QTreeWidgetItem* numAeroSections = new QTreeWidgetItem(treeWidgetItem, QStringList("Num Aero Sections"));
   auto numAeroSectionsLineEdit = new MoverCreatorLineEdit(numAeroSections);

   // Temporary!
   numAeroSections->setDisabled(true);
   numAeroSectionsLineEdit->setDisabled(true);
   numAeroSections->setToolTip(0, "Multiple aero section will be added in the future");

   numAeroSectionsLineEdit->setValidator(new QIntValidator(1, 100));
   numAeroSectionsLineEdit->setFrame(false);
   numAeroSectionsLineEdit->setText(QString::number(mMassFactor));
   aTreeWidgetPtr->setItemWidget(numAeroSections, 1, numAeroSectionsLineEdit);
   aTreeWidgetPtr->setItemWidget(numAeroSections, 2, new QLabel(QString::fromStdString("")));
   connect(numAeroSectionsLineEdit, &QLineEdit::textChanged, this, &GeometrySurface::HandleNumAeroSectionsLineEditChanged);
   connect(numAeroSectionsLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });

   ConnectLineEdits();

   return treeWidgetItem;
}

double GeometrySurface::GetAspectRatio()
{
   if (mUseManualAspectRatio)
   {
      return mManualAspectRatio;
   }

   return mAspectRatio;
}

std::vector<GeometrySurface::GeometryPanel> GeometrySurface::GetPanels()
{
   std::vector<GeometrySurface::GeometryPanel> controlSurfaces;
   if (mSurfaceControlMotion == GeometrySurface::cControlMotionControlSurface)
   {
      GeometryPanel controlSurface;
      controlSurface.mChordFractionStart = mControlSurfaceChordFractionStart;
      controlSurface.mChordFractionEnd = mControlSurfaceChordFractionEnd;
      controlSurface.mSpanFractionStart = mControlSurfaceSpanFractionStart;
      controlSurface.mSpanFractionEnd = mControlSurfaceSpanFractionEnd;
      controlSurfaces.push_back(controlSurface);
   }
   return controlSurfaces;
}

void GeometrySurface::CalcAspectRatioAndArea()
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

   if ((mSymmetryString == "Vertical") || (mSymmetryString == "Horizontal"))
   {
      // Check spacing
      double spacing = 0.0;

      if (mSymmetryString == "Horizontal")
      {
         spacing = 2.0 * mRefPoint_ft.Y();
      }
      else if (mSymmetryString == "Vertical")
      {
         spacing = 2.0 * mRefPoint_ft.Z();
      }

      if (spacing < mSpan_ft * 0.5)
      {
         // Separation is within aero effect distance (1/2 span)
         area = 2.0 * effectiveSpan * averageChord; // Area is now doubled
         effectiveSpan = 2.0 * mSpan_ft;            // Span is doubled
      }
   }

   mAspectRatio = (effectiveSpan * effectiveSpan) / area;
   mCalculatedAspectRatioLineEditPtr->setText(QString::number(mAspectRatio));
   mCalculatedAreaLineEditPtr->setText(QString::number(mArea_ft2));
}

void GeometrySurface::ConnectLineEdits()
{
   connect(mReferencePointX_LineEditPtr, &QLineEdit::textChanged, this, &GeometrySurface::HandleX_CoordinateLineEditChanged);
   connect(mReferencePointY_LineEditPtr, &QLineEdit::textChanged, this, &GeometrySurface::HandleY_CoordinateLineEditChanged);
   connect(mReferencePointZ_LineEditPtr, &QLineEdit::textChanged, this, &GeometrySurface::HandleZ_CoordinateLineEditChanged);
}

void GeometrySurface::HandleX_CoordinateLineEditChanged(QString aValue)
{
   mRefPoint_ft.Set(aValue.toDouble(), mRefPoint_ft.Y(), mRefPoint_ft.Z());

   CalcAspectRatioAndArea();
   AdjustAndRedraw(true);
}

void GeometrySurface::HandleY_CoordinateLineEditChanged(QString aValue)
{
   mRefPoint_ft.Set(mRefPoint_ft.X(), aValue.toDouble(), mRefPoint_ft.Z());

   CalcAspectRatioAndArea();
   AdjustAndRedraw(true);
}

void GeometrySurface::HandleZ_CoordinateLineEditChanged(QString aValue)
{
   mRefPoint_ft.Set(mRefPoint_ft.X(), mRefPoint_ft.Y(), aValue.toDouble());

   CalcAspectRatioAndArea();
   AdjustAndRedraw(true);
}

void GeometrySurface::HandleSpanLineEditChanged(QString aValue)
{
   mSpan_ft = aValue.toDouble();

   CalcAspectRatioAndArea();
   AdjustAndRedraw(true);
}

void GeometrySurface::HandleRootChordLineEditChanged(QString aValue)
{
   mRootChord_ft = aValue.toDouble();

   CalcAspectRatioAndArea();
   AdjustAndRedraw(true);
}

void GeometrySurface::HandleTipChordLineEditChanged(QString aValue)
{
   mTipChord_ft = aValue.toDouble();

   CalcAspectRatioAndArea();
   AdjustAndRedraw(true);
}

void GeometrySurface::HandleNumAeroSectionsLineEditChanged(QString aValue)
{
   QLineEdit* numSectionsLineEdit = dynamic_cast<QLineEdit*>(QObject::sender());
   if (aValue.size() == 0)
   {
      mNumAeroSections = 0;
      return;
   }
   int intVal = aValue.toInt();
   if (intVal <= 0)
   {
      intVal = 1;
      numSectionsLineEdit->setText(""); //Can't have 0 or fewer sections
   }
   mNumAeroSections = intVal;
}

void GeometrySurface::HandleSweepAngleLineEditChanged(QString aValue)
{
   mSweepAngle_rad = aValue.toDouble() * UtMath::cRAD_PER_DEG;
   AdjustAndRedraw(true);
}

void GeometrySurface::HandleDihedralAngleLineEditChanged(QString aValue)
{
   mDihedralAngle_rad = aValue.toDouble() * UtMath::cRAD_PER_DEG;
   AdjustAndRedraw(true);
}

void GeometrySurface::HandleIncidenceAngleLineEditChanged(QString aValue)
{
   mIncidenceAngle_rad = aValue.toDouble() * UtMath::cRAD_PER_DEG;
   AdjustAndRedraw(true);
}

void GeometrySurface::HandleOswaldEfficiencyLineEditChanged(QString aValue)
{
   mOswaldsEfficiency = aValue.toDouble();
}

void GeometrySurface::HandleThicknessLineEditChanged(QString aValue)
{
   mThicknessRatio = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometrySurface::HandleFinRefRadiusLineEditChanged(QString aValue)
{
   mFinRefRadius_ft = aValue.toDouble();

   AdjustAndRedraw(true);
}

void GeometrySurface::HandleSurfaceControlMotionComboBoxChanged(QString aValue)
{
   mSurfaceControlMotionString = aValue;

   if (mSurfaceControlMotionString == "Fixed")
   {
      mSurfaceControlMotion = GeometrySurface::cControlMotionFixed;

      if (mControlSurfaceChordFractionStartWidget != nullptr) { mControlSurfaceChordFractionStartWidget->setHidden(true); }
      if (mControlSurfaceChordFractionEndWidget != nullptr) { mControlSurfaceChordFractionEndWidget->setHidden(true); }
      if (mControlSurfaceSpanFractionStartWidget != nullptr) { mControlSurfaceSpanFractionStartWidget->setHidden(true); }
      if (mControlSurfaceSpanFractionEndWidget != nullptr) { mControlSurfaceSpanFractionEndWidget->setHidden(true); }
   }
   else if (mSurfaceControlMotionString == "Full-Flying")
   {
      mSurfaceControlMotion = cControlMotionFullFlying;

      if (mControlSurfaceChordFractionStartWidget != nullptr) { mControlSurfaceChordFractionStartWidget->setHidden(true); }
      if (mControlSurfaceChordFractionEndWidget != nullptr)   { mControlSurfaceChordFractionEndWidget->setHidden(true); }
      if (mControlSurfaceSpanFractionStartWidget != nullptr)  { mControlSurfaceSpanFractionStartWidget->setHidden(true); }
      if (mControlSurfaceSpanFractionEndWidget != nullptr)    { mControlSurfaceSpanFractionEndWidget->setHidden(true); }
   }
   else if (mSurfaceControlMotionString == "Control Surface")
   {
      mSurfaceControlMotion = cControlMotionControlSurface;

      if (mControlSurfaceChordFractionStartWidget != nullptr) { mControlSurfaceChordFractionStartWidget->setHidden(false); }
      if (mControlSurfaceChordFractionEndWidget != nullptr)   { mControlSurfaceChordFractionEndWidget->setHidden(false); }
      if (mControlSurfaceSpanFractionStartWidget != nullptr)  { mControlSurfaceSpanFractionStartWidget->setHidden(false); }
      if (mControlSurfaceSpanFractionEndWidget != nullptr)    { mControlSurfaceSpanFractionEndWidget->setHidden(false); }
   }
}

void GeometrySurface::HandleControlSurfaceChordFractionStartLineEditChanged(QString aValue)
{
   mControlSurfaceChordFractionStart = aValue.toDouble();

   if ((mSurfaceControlMotionString == "Control Surface") || (mSurfaceControlMotion == cControlMotionControlSurface))
   {
      if (mControlSurfaceChordFractionStartWidget != nullptr) { mControlSurfaceChordFractionStartWidget->setHidden(false); }
   }

   AdjustAndRedraw(true);
}

void GeometrySurface::HandleControlSurfaceChordFractionEndLineEditChanged(QString aValue)
{
   mControlSurfaceChordFractionEnd = aValue.toDouble();

   if ((mSurfaceControlMotionString == "Control Surface") || (mSurfaceControlMotion == cControlMotionControlSurface))
   {
      if (mControlSurfaceChordFractionEndWidget != nullptr) { mControlSurfaceChordFractionEndWidget->setHidden(false); }
   }

   AdjustAndRedraw(true);
}

void GeometrySurface::HandleControlSurfaceSpanFractionStartLineEditChanged(QString aValue)
{
   mControlSurfaceSpanFractionStart = aValue.toDouble();

   if ((mSurfaceControlMotionString == "Control Surface") || (mSurfaceControlMotion == cControlMotionControlSurface))
   {
      if (mControlSurfaceSpanFractionStartWidget != nullptr) { mControlSurfaceSpanFractionStartWidget->setHidden(false); }
   }

   AdjustAndRedraw(true);
}

void GeometrySurface::HandleControlSurfaceSpanFractionEndLineEditChanged(QString aValue)
{
   mControlSurfaceSpanFractionEnd = aValue.toDouble();

   if ((mSurfaceControlMotionString == "Control Surface") || (mSurfaceControlMotion == cControlMotionControlSurface))
   {
      if (mControlSurfaceSpanFractionEndWidget != nullptr) { mControlSurfaceSpanFractionEndWidget->setHidden(false); }
   }

   AdjustAndRedraw(true);
}

void GeometrySurface::HandleControlSurfaceMinAngleLineEditChanged(QString aValue)
{
   mControlSurfaceMinAngle_deg = aValue.toDouble();
}

void GeometrySurface::HandleControlSurfaceMaxAngleLineEditChanged(QString aValue)
{
   mControlSurfaceMaxAngle_deg = aValue.toDouble();
}

void GeometrySurface::HandleExponentialAngleMappingCheckBoxChanged(int aValue)
{
   if (aValue == 0)
   {
      mUseExponentialAngleMapping = false;
   }
   else
   {
      mUseExponentialAngleMapping = true;
   }
}

void GeometrySurface::HandleActuatorMinRateLineEditChanged(QString aValue)
{
   mActuatorMinRate_dps = aValue.toDouble();
}

void GeometrySurface::HandleActuatorMaxRateLineEditChanged(QString aValue)
{
   mActuatorMaxRate_dps = aValue.toDouble();
}

void GeometrySurface::HandleActuatorMinAngleLineEditChanged(QString aValue)
{
   mActuatorMinAngle_deg = aValue.toDouble();
}

void GeometrySurface::HandleActuatorMaxAngleLineEditChanged(QString aValue)
{
   mActuatorMaxAngle_deg = aValue.toDouble();
}

void GeometrySurface::HandleQuadControlFinsPatternComboBoxChanged(QString aValue)
{
   mQuadControlFinsPatternString = aValue;

   if (mQuadControlFinsPatternString == "+ Pattern")
   {
      if (mSymmetricalStringComboBox != nullptr) { mSymmetricalStringComboBox->setCurrentText(QString("+ Pattern")); }
   }
   else if (mQuadControlFinsPatternString == "X Pattern")
   {
      if (mSymmetricalStringComboBox != nullptr) { mSymmetricalStringComboBox->setCurrentText(QString("X Pattern")); }
   }
}

void GeometrySurface::HandleQuadControlFinsCheckBoxChanged(int aValue)
{
   if (aValue == 0)
   {
      mQuadControlFins = false;
   }
   else
   {
      mQuadControlFins = true;
   }

   AdjustAndRedraw(true);
}

void GeometrySurface::HandleIsMovableCheckBoxChanged(int aValue)
{
   if (aValue == 0)
   {
      mIsMovable = false;

      if (mSurfaceControlMotionWidget        != nullptr)      { mSurfaceControlMotionWidget->setHidden(true); }
      if (mControlSurfaceChordFractionStartWidget != nullptr) { mControlSurfaceChordFractionStartWidget->setHidden(true); }
      if (mControlSurfaceChordFractionEndWidget != nullptr)   { mControlSurfaceChordFractionEndWidget->setHidden(true); }
      if (mControlSurfaceSpanFractionStartWidget != nullptr)  { mControlSurfaceSpanFractionStartWidget->setHidden(true); }
      if (mControlSurfaceSpanFractionEndWidget != nullptr)    { mControlSurfaceSpanFractionEndWidget->setHidden(true); }
      if (mControlSurfaceMinAngleWidget      != nullptr)      { mControlSurfaceMinAngleWidget->setHidden(true); }
      if (mControlSurfaceMaxAngleWidget      != nullptr)      { mControlSurfaceMaxAngleWidget->setHidden(true); }
      if (mExponentialAngleMappingWidget     != nullptr)      { mExponentialAngleMappingWidget->setHidden(true); }
      if (mActuatorMinRateWidget             != nullptr)      { mActuatorMinRateWidget->setHidden(true); }
      if (mActuatorMaxRateWidget             != nullptr)      { mActuatorMaxRateWidget->setHidden(true); }
      if (mActuatorMinAngleWidget            != nullptr)      { mActuatorMinAngleWidget->setHidden(true); }
      if (mActuatorMaxAngleWidget            != nullptr)      { mActuatorMaxAngleWidget->setHidden(true); }
   }
   else
   {
      mIsMovable = true;

      if (mSurfaceControlMotionWidget != nullptr) { mSurfaceControlMotionWidget->setHidden(false); }

      if ((mSurfaceControlMotionString == "Control Surface") || (mSurfaceControlMotion == cControlMotionControlSurface))
      {
         if (mControlSurfaceChordFractionStartWidget != nullptr) { mControlSurfaceChordFractionStartWidget->setHidden(false); }
         if (mControlSurfaceChordFractionEndWidget != nullptr)   { mControlSurfaceChordFractionEndWidget->setHidden(false); }
         if (mControlSurfaceSpanFractionStartWidget != nullptr)  { mControlSurfaceSpanFractionStartWidget->setHidden(false); }
         if (mControlSurfaceSpanFractionEndWidget != nullptr)    { mControlSurfaceSpanFractionEndWidget->setHidden(false); }
      }
      else
      {
         if (mControlSurfaceChordFractionStartWidget != nullptr) { mControlSurfaceChordFractionStartWidget->setHidden(true); }
         if (mControlSurfaceChordFractionEndWidget != nullptr)   { mControlSurfaceChordFractionEndWidget->setHidden(true); }
         if (mControlSurfaceSpanFractionStartWidget != nullptr)  { mControlSurfaceSpanFractionStartWidget->setHidden(true); }
         if (mControlSurfaceSpanFractionEndWidget != nullptr)    { mControlSurfaceSpanFractionEndWidget->setHidden(true); }
      }

      if (mControlSurfaceMinAngleWidget  != nullptr) { mControlSurfaceMinAngleWidget->setHidden(false); }
      if (mControlSurfaceMaxAngleWidget  != nullptr) { mControlSurfaceMaxAngleWidget->setHidden(false); }
      if (mExponentialAngleMappingWidget != nullptr) { mExponentialAngleMappingWidget->setHidden(false); }
      if (mActuatorMinRateWidget         != nullptr) { mActuatorMinRateWidget->setHidden(false); }
      if (mActuatorMaxRateWidget         != nullptr) { mActuatorMaxRateWidget->setHidden(false); }
      if (mActuatorMinAngleWidget        != nullptr) { mActuatorMinAngleWidget->setHidden(false); }
      if (mActuatorMaxAngleWidget        != nullptr) { mActuatorMaxAngleWidget->setHidden(false); }
   }
}

void GeometrySurface::HandleCalcAspectRatioLineEditChanged(QString aValue)
{
   CalcAspectRatioAndArea();
}

void GeometrySurface::HandleCalcAreaLineEditChanged(QString aValue)
{
   CalcAspectRatioAndArea();
}

void GeometrySurface::HandleUseManualAspectRatioCheckBoxChanged(int aValue)
{
   if (aValue == 0)
   {
      mUseManualAspectRatio = false;
   }
   else
   {
      mUseManualAspectRatio = true;
   }

   if (mUseManualAspectRatio)
   {
      mManualAspectRatioWidget->setHidden(false);
   }
   else
   {
      mManualAspectRatioWidget->setHidden(true);
   }
}

void GeometrySurface::HandleSymmetryCannotBeChangedCheckBoxChanged(int aValue)
{
   if (aValue == 0)
   {
      mSymmetryCannotBeChanged = false;
   }
   else
   {
      mSymmetryCannotBeChanged = true;
   }

   AdjustAndRedraw(false);
}

void GeometrySurface::HandleManualAspectRatioLineEditChanged(QString aValue)
{
   mManualAspectRatio = aValue.toDouble();
}

void GeometrySurface::HandleSymmetricalComboBoxChanged(QString aValue)
{
   mSymmetryString = aValue;

   if (mSymmetryString == "Single")
   {
      mSymmetrical = false;

      // Show the reference point and hide the fin ref data
      if (mFinRefRadiusWidget != nullptr) { mFinRefRadiusWidget->setHidden(true); }

      // Be sure that dihedral is not hidden
      if (mDihedralWidget != nullptr) { mDihedralWidget->setHidden(false); }

      if (mName == "Vertical Tail")
      {
         Vehicle::VehicleControlConfig controlConfig = mVehiclePtr->GetVehicleControlConfiguration();
         switch (controlConfig)
         {
         case Designer::Vehicle::cCONFIG_STABS_TWO_TAILS:
            mVehiclePtr->SetVehicleControlConfiguration(Vehicle::cCONFIG_STABS_ONE_TAIL);
            break;
         case Designer::Vehicle::cCONFIG_CANARDS_TWO_TAILS:
            mVehiclePtr->SetVehicleControlConfiguration(Vehicle::cCONFIG_CANARDS_ONE_TAIL);
            break;
         case Designer::Vehicle::cCONFIG_CANARDS_STABS_TWO_TAILS:
            mVehiclePtr->SetVehicleControlConfiguration(Vehicle::cCONFIG_CANARDS_STABS_ONE_TAIL);
            break;
         default:
            break;
         }
      }
      if (mName == "Ventral Tail")
      {
         Vehicle::VehicleControlConfig controlConfig = mVehiclePtr->GetVehicleControlConfiguration();
         if (controlConfig == Vehicle::cCONFIG_STABS_TWO_VENTRAL_TAILS)
         {
            mVehiclePtr->SetVehicleControlConfiguration(Vehicle::cCONFIG_STABS_ONE_VENTRAL_TAIL);
         }
      }

   }
   else if (mSymmetryString == "Horizontal")
   {
      mSymmetrical = true;

      // Show the reference point and hide the fin ref data
      if (mFinRefRadiusWidget != nullptr) { mFinRefRadiusWidget->setHidden(true); }

      // Be sure that dihedral is not hidden
      if (mDihedralWidget != nullptr) { mDihedralWidget->setHidden(false); }

      if (mName == "Vertical Tail")
      {
         Vehicle::VehicleControlConfig controlConfig = mVehiclePtr->GetVehicleControlConfiguration();
         switch (controlConfig)
         {
         case Designer::Vehicle::cCONFIG_STABS_ONE_TAIL:
            mVehiclePtr->SetVehicleControlConfiguration(Vehicle::cCONFIG_STABS_TWO_TAILS);
            break;
         case Designer::Vehicle::cCONFIG_CANARDS_ONE_TAIL:
            mVehiclePtr->SetVehicleControlConfiguration(Vehicle::cCONFIG_CANARDS_TWO_TAILS);
            break;
         case Designer::Vehicle::cCONFIG_CANARDS_STABS_ONE_TAIL:
            mVehiclePtr->SetVehicleControlConfiguration(Vehicle::cCONFIG_CANARDS_STABS_TWO_TAILS);
            break;
         default:
            break;
         }
      }
      if (mName == "Ventral Tail")
      {
         Vehicle::VehicleControlConfig controlConfig = mVehiclePtr->GetVehicleControlConfiguration();
         if (controlConfig == Vehicle::cCONFIG_STABS_ONE_VENTRAL_TAIL)
         {
            mVehiclePtr->SetVehicleControlConfiguration(Vehicle::cCONFIG_STABS_TWO_VENTRAL_TAILS);
         }
      }

   }
   else if (mSymmetryString == "Vertical")
   {
      mSymmetrical = true;

      // Show the reference point and hide the fin ref data
      if (mFinRefRadiusWidget != nullptr) { mFinRefRadiusWidget->setHidden(true); }

      // Be sure that dihedral is not hidden
      if (mDihedralWidget != nullptr) { mDihedralWidget->setHidden(false); }
   }
   else if (mSymmetryString == "+ Pattern")
   {
      mSymmetrical = true;

      // Hide the reference point and show the fin ref data
      if (mFinRefRadiusWidget != nullptr) { mFinRefRadiusWidget->setHidden(false); }

      if (mDihedralAngleLineEdit != nullptr)
      {
         double dihedral_deg = 0.0;
         mDihedralAngleLineEdit->setText(QString::number(dihedral_deg));
         mDihedralAngle_rad = dihedral_deg * UtMath::cRAD_PER_DEG;
      }

      // Be sure that dihedral is hidden
      if (mDihedralWidget != nullptr) { mDihedralWidget->setHidden(true); }

      if (mQuadControlFinsPatternComboBox != nullptr) { mQuadControlFinsPatternComboBox->setCurrentText(QString("+ Pattern")); }
   }
   else if (mSymmetryString == "X Pattern")
   {
      mSymmetrical = true;

      // Hide the reference point and show the fin ref data
      if (mFinRefRadiusWidget != nullptr) { mFinRefRadiusWidget->setHidden(false); }

      if (mDihedralAngleLineEdit != nullptr)
      {
         double dihedral_deg = 45.0;
         mDihedralAngleLineEdit->setText(QString::number(dihedral_deg));
         mDihedralAngle_rad = dihedral_deg * UtMath::cRAD_PER_DEG;
      }

      // Be sure that dihedral is hidden
      if (mDihedralWidget != nullptr) { mDihedralWidget->setHidden(true); }

      if (mQuadControlFinsPatternComboBox != nullptr) { mQuadControlFinsPatternComboBox->setCurrentText(QString("X Pattern")); }
   }
   else
   {
      auto outStream = ut::log::error() << "Unknown symmetry string in GeometrySurface::HandleSymmetricalComboBoxChanged().";
      outStream.AddNote() << "mSymmetryString: " << mSymmetryString.toStdString();
      return;
   }

   AdjustAndRedraw(true);
}

void GeometrySurface::AdjustAndRedraw(bool aRecalcMassAndInertia)
{
   // If the symmetry "cannot be changed", this means that what was
   // contained in template file, should stay "as is", so we will
   // hide the symmetry widget
   if (mSymmetryCannotBeChanged)
   {
      mSymmetricalWidgetItem->setHidden(true);
   }

   // If we have quad control fins, we will show the quad control fins
   // pattern widget and hide the symmetry widget
   if (mQuadControlFins)
   {
      mQuadControlFinsPatternWidget->setHidden(false);
      mSymmetricalWidgetItem->setHidden(true);
   }

   GeometryObject::AdjustAndRedraw(aRecalcMassAndInertia);
}

bool GeometrySurface::CalcExtentsForSurface(double&            aMinX,
                                            double&            aMaxX,
                                            double&            aMinY,
                                            double&            aMaxY,
                                            double&            aMinZ,
                                            double&            aMaxZ,
                                            double             aSpan,
                                            const std::string& aSymmetryString)
{
   if ((aSymmetryString == "X Pattern") || (aSymmetryString == "+ Pattern"))
   {
      double xFrontRoot = mRefPoint_ft.X() + mRootChord_ft * 0.25;
      double xRearRoot  = mRefPoint_ft.X() - mRootChord_ft * 0.75;
      double tipDist    = -aSpan * tan(mSweepAngle_rad);
      double xFrontTip  = mRefPoint_ft.X() + tipDist + mTipChord_ft * 0.25;
      double xRearTip   = mRefPoint_ft.X() + tipDist - mTipChord_ft * 0.75;

      if (xFrontTip > xFrontRoot)
      {
         aMaxX = xFrontTip;
      }
      else
      {
         aMaxX = xFrontRoot;
      }

      if (xRearTip < xRearRoot)
      {
         aMinX = xRearTip;
      }
      else
      {
         aMinX = xRearRoot;
      }

      if (aSymmetryString == "X Pattern")
      {
         double span = (mFinRefRadius_ft + aSpan) * cos(UtMath::cPI_OVER_4);
         aMinY = mRefPoint_ft.Y() - span;
         aMaxY = mRefPoint_ft.Y() + span;
         aMinZ = mRefPoint_ft.Z() - span;
         aMaxZ = mRefPoint_ft.Z() + span;
         return true;
      }
      else if (aSymmetryString == "+ Pattern")
      {
         double span = mFinRefRadius_ft + aSpan;
         aMinY = mRefPoint_ft.Y() - span;
         aMaxY = mRefPoint_ft.Y() + span;
         aMinZ = mRefPoint_ft.Z() - span;
         aMaxZ = mRefPoint_ft.Z() + span;
         return true;
      }
   }

   UtDCM surfaceDihedralDCM(0.0, 0.0, -mDihedralAngle_rad); // For proper angle, use negative dihedral
   UtDCM surfaceIncidenceDCM(0.0, mIncidenceAngle_rad, 0.0);
   UtDCM mDCM_DihedralAndIncidence = surfaceIncidenceDCM * surfaceDihedralDCM;

   UtVec3dX frontRootPt(mRootChord_ft * 0.25, 0.0, 0.0);
   UtVec3dX v1 = mDCM_DihedralAndIncidence.InverseTransform(frontRootPt);
   v1 += mRefPoint_ft;

   UtVec3dX rearRootPt(-mRootChord_ft * 0.75, 0.0, 0.0);
   UtVec3dX v2 = mDCM_DihedralAndIncidence.InverseTransform(rearRootPt);
   v2 += mRefPoint_ft;

   UtVec3dX frontTipPt(mTipChord_ft * 0.25 - aSpan * tan(mSweepAngle_rad), aSpan, 0.0);
   UtVec3dX v3 = mDCM_DihedralAndIncidence.InverseTransform(frontTipPt);
   v3 += mRefPoint_ft;

   UtVec3dX rearTipPt(-mTipChord_ft * 0.75 - aSpan * tan(mSweepAngle_rad), aSpan, 0.0);
   UtVec3dX v4 = mDCM_DihedralAndIncidence.InverseTransform(rearTipPt);
   v4 += mRefPoint_ft;

   aMinX = v1.X();
   if (v2.X() < aMinX) { aMinX = v2.X(); }
   if (v3.X() < aMinX) { aMinX = v3.X(); }
   if (v4.X() < aMinX) { aMinX = v4.X(); }

   aMaxX = v1.X();
   if (v2.X() > aMaxX) { aMaxX = v2.X(); }
   if (v3.X() > aMaxX) { aMaxX = v3.X(); }
   if (v4.X() > aMaxX) { aMaxX = v4.X(); }

   aMinY = v1.Y();
   if (v2.Y() < aMinY) { aMinY = v2.Y(); }
   if (v3.Y() < aMinY) { aMinY = v3.Y(); }
   if (v4.Y() < aMinY) { aMinY = v4.Y(); }

   aMaxY = v1.Y();
   if (v2.Y() > aMaxY) { aMaxY = v2.Y(); }
   if (v3.Y() > aMaxY) { aMaxY = v3.Y(); }
   if (v4.Y() > aMaxY) { aMaxY = v4.Y(); }

   aMinZ = v1.Z();
   if (v2.Z() < aMinZ) { aMinZ = v2.Z(); }
   if (v3.Z() < aMinZ) { aMinZ = v3.Z(); }
   if (v4.Z() < aMinZ) { aMinZ = v4.Z(); }

   aMaxZ = v1.Z();
   if (v2.Z() > aMaxZ) { aMaxZ = v2.Z(); }
   if (v3.Z() > aMaxZ) { aMaxZ = v3.Z(); }
   if (v4.Z() > aMaxZ) { aMaxZ = v4.Z(); }

   if (aSymmetryString == "Horizontal")
   {
      // Now, determine location of other surface
      double currentSurfaceY = mRefPoint_ft.Y();
      double delta = currentSurfaceY - mSymmetryHorizontalY;
      double otherSurfaceY = currentSurfaceY - 2.0 * delta;

      if (otherSurfaceY < currentSurfaceY)
      {
         // Current surface (on right) outer extent
         double outerExtentDeltaY = aMaxY - currentSurfaceY;

         // Other surface is on the left
         aMinY = otherSurfaceY - outerExtentDeltaY;
      }
      else
      {
         // Current surface (on left) outer extent
         double outerExtentDeltaY = aMinY - currentSurfaceY;

         // Other surface is on the right
         aMaxY = otherSurfaceY - outerExtentDeltaY;
      }
   }
   else if (aSymmetryString == "Vertical")
   {
      // Now, determine location of other surface
      double currentSurfaceZ = mRefPoint_ft.Z();
      double delta = currentSurfaceZ - mSymmetryVerticalZ;
      double otherSurfaceZ = currentSurfaceZ - 2.0 * delta;

      if (otherSurfaceZ < currentSurfaceZ)
      {
         // Current surface (on bottom) outer extent
         double outerExtentDeltaZ = aMaxZ - currentSurfaceZ;

         // Other surface is on the top
         aMinZ = otherSurfaceZ - outerExtentDeltaZ;
      }
      else
      {
         // Current surface (on left) outer extent
         double outerExtentDeltaZ = aMinZ - currentSurfaceZ;

         // Other surface is on the right
         aMaxZ = otherSurfaceZ - outerExtentDeltaZ;
      }
   }

   return true;
}

bool GeometrySurface::CalcExtents(double& xMin, double& xMax, double& yMin, double& yMax, double& zMin, double& zMax)
{
   return CalcExtentsForSurface(xMin, xMax, yMin, yMax, zMin, zMax, mSpan_ft, mSymmetryString.toStdString());
}

double GeometrySurface::GetMassArea_ft2()
{
   double averageChord = 0.5 * (mRootChord_ft + mTipChord_ft);
   return mSpan_ft * averageChord;
}

void GeometrySurface::CalcSingleSurfaceMassProperties(GeometrySurfaceInertia& aSurfaceInertiaObject,
                                                      double                  aMass_slugs,
                                                      double                  aSpan_ft,
                                                      double                  aArea_sqft,
                                                      UtVec3dX                aRefPoint_ft)
{
   if ((mSpan_ft < std::numeric_limits<double>::epsilon()) ||
       (mRootChord_ft < std::numeric_limits<double>::epsilon()) ||
       (mTipChord_ft < std::numeric_limits<double>::epsilon()))
   {
      mCalculatedIxx = 0.0;
      mCalculatedIyy = 0.0;
      mCalculatedIzz = 0.0;
      mCalculatedCG_ft.Set(0.0, 0.0, 0.0);
      return;
   }

   // Determine the area for a single surface
   double averageChord = 0.5 * (mRootChord_ft + mTipChord_ft);
   double totalArea = aSpan_ft * averageChord;

   // Break the surface into two triangles (or a single triangle, if tip chord is zero)
   GeometryTriangle triangle1;
   GeometryTriangle triangle2;

   // Check for the tip being zero length. We'll assume anything less than
   // a billionth of a foot is zero, due to numerical precision
   if (mTipChord_ft < 0.000000001)
   {
      // We have only a single triangle

      UtVec3dX FrontRootPt(0.0, 0.0, 0.0);
      UtVec3dX AftRootPt(-mRootChord_ft, 0.0, 0.0);

      UtVec3dX aftVec = AftRootPt - FrontRootPt;
      double length = aftVec.Magnitude();
      length *= 0.25;
      aftVec.Normalize();
      UtVec3dX rootQuarterChordPt = aftVec * length;

      // Determine how far back from the root quarter chord the tip quarter chord exists
      double offsetLength_ft = tan(mSweepAngle_rad) * aSpan_ft;

      UtVec3dX offsetVec(-offsetLength_ft, aSpan_ft, 0.0);
      UtVec3dX TipPt = rootQuarterChordPt + offsetVec;

      // Set up the single triangle
      triangle1.SetPoints(FrontRootPt, AftRootPt, TipPt);

      if (!triangle1.CalcCentroid())
      {
         auto outStream = ut::log::error() << "Unable to calculate a centroid for triangle in AeroSurface::CalcSingleSurfaceMassProperties().";
         outStream.AddNote() << "Surface Name: " << mName;
      }

      // Calculate the location of the cg
      mCalculatedCG_ft = triangle1.GetCentroid_ft();
   }
   else
   {
      // We have two triangles

      UtVec3dX FrontRootPt(0.0, 0.0, 0.0);
      UtVec3dX AftRootPt(-mRootChord_ft, 0.0, 0.0);

      UtVec3dX aftVec = AftRootPt - FrontRootPt;
      double length = aftVec.Magnitude();
      length *= 0.25;
      aftVec.Normalize();
      UtVec3dX rootQuarterChordPt = aftVec * length;

      // Determine how far back from the root quarter chord the tip quarter chord exists
      double offsetLength_ft = tan(mSweepAngle_rad) * aSpan_ft;

      UtVec3dX offsetVec(-offsetLength_ft, aSpan_ft, 0.0);
      UtVec3dX tipQuarterChordPt = rootQuarterChordPt + offsetVec;
      UtVec3dX tipForwardVec(mTipChord_ft * 0.25, 0.0, 0.0);
      UtVec3dX FrontTipPt = tipQuarterChordPt + tipForwardVec;
      UtVec3dX tipAftVec(-mTipChord_ft * 0.75, 0.0, 0.0);
      UtVec3dX AftTipPt = tipQuarterChordPt + tipAftVec;

      // Set up the two triangles
      triangle1.SetPoints(FrontRootPt, AftRootPt, FrontTipPt);
      triangle2.SetPoints(AftTipPt, FrontTipPt, AftRootPt);

      double averageChord2   = 0.5 * mTipChord_ft;
      double area2           = aSpan_ft * averageChord2;
      double fractionalArea2 = area2 / totalArea;

      if (!triangle1.CalcCentroid())
      {
         auto outStream = ut::log::error() << "Unable to calculate a centroid for triangle1 in AeroSurface::CalcSingleSurfaceMassProperties().";
         outStream.AddNote() << "Surface Name: " << mName;
      }

      if (!triangle2.CalcCentroid())
      {
         auto outStream = ut::log::error() << "Unable to calculate a centroid for triangle2 in AeroSurface::CalcSingleSurfaceMassProperties().";
         outStream.AddNote() << "Surface Name: " << mName;
      }

      // Calculate the location of the cg
      UtVec3dX cg1 = triangle1.GetCentroid_ft();
      UtVec3dX cg2 = triangle2.GetCentroid_ft();

      UtVec3dX delCg = cg2 - cg1;
      UtVec3dX cg = cg1 + (delCg * fractionalArea2);

      // Calculate the location of the cg
      mCalculatedCG_ft = cg;
   }

   // Adjust for root quarter chord
   UtVec3dX rootChordAdjustment(mRootChord_ft * 0.25, 0.0, 0.0);
   mCalculatedCG_ft += rootChordAdjustment;

   // Now, adjust for orientation - we only consider dihedral effects
   UtDCM attitudeDCM(0.0, 0.0, -mDihedralAngle_rad);
   mCalculatedCG_ft = attitudeDCM.InverseTransform(mCalculatedCG_ft);

   // Finally, add in ref pt offset
   mCalculatedCG_ft += aRefPoint_ft;

   // Rotational inertia
   double calculatedIxx;
   double calculatedIyy;
   double calculatedIzz;
   aSurfaceInertiaObject.CalculateRotationalInertia(aSpan_ft, aArea_sqft, aMass_slugs, aRefPoint_ft,
                                                    calculatedIxx, calculatedIyy, calculatedIzz);
   mCalculatedIxx = calculatedIxx;
   mCalculatedIyy = calculatedIyy;
   mCalculatedIzz = calculatedIzz;
}

void GeometrySurface::CalcMassProperties(double aTotalMass_lbs)
{
   // Set the mass
   mCalculatedMass_lbs = aTotalMass_lbs;

   // Convert mass to slugs
   double mass_slugs = mCalculatedMass_lbs * mLbsToSlugs;

   // First, we need to consider symmetry effects on mass. If we have symmetry,
   // we need to consider how many pieces are involved, keeping in mind that
   // the total mass is for all pieces
   if (mSymmetrical)
   {
      if ((mSymmetryString == "Horizontal") || (mSymmetryString == "Vertical"))
      {
         // Use half of the total mass
         mass_slugs *= 0.5;
      }
      else if ((mSymmetryString == "+ Pattern") || (mSymmetryString == "X Pattern"))
      {
         // Use a quarter of the total mass
         mass_slugs *= 0.25;
      }
   }

   // Set nominal ref pt
   UtVec3dX refPoint_ft = mRefPoint_ft;

   // Clear surface inertia data
   mSurfaceInertiaSet.clear();

   if (mSymmetryString == "Single")
   {
      GeometrySurfaceInertia surfaceInertia;
      surfaceInertia.SetParent(this);
      CalcSingleSurfaceMassProperties(surfaceInertia, mass_slugs, mSpan_ft, mArea_ft2, refPoint_ft);
      mSurfaceInertiaSet.push_back(surfaceInertia);
   }
   else if (mSymmetryString == "Horizontal")
   {
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

      // Current Fin
      GeometrySurfaceInertia surfaceInertiaFin1;
      surfaceInertiaFin1.SetParent(this);
      mDihedralAngle_rad = dihedral_rad;
      mIncidenceAngle_rad = incidenceAngle_rad;
      refPoint_ft.Set(mRefPoint_ft.X(), currentSurfaceY, mRefPoint_ft.Z());
      CalcSingleSurfaceMassProperties(surfaceInertiaFin1, mass_slugs, mSpan_ft, mArea_ft2, refPoint_ft);
      mSurfaceInertiaSet.push_back(surfaceInertiaFin1);
      totalIxx += mCalculatedIxx;
      totalIyy += mCalculatedIyy;
      totalIzz += mCalculatedIzz;
      totalCGMass += (mCalculatedCG_ft * mass_slugs);

      // Other Fin
      GeometrySurfaceInertia surfaceInertiaFin2;
      surfaceInertiaFin2.SetParent(this);
      mDihedralAngle_rad = UtMath::cPI - dihedral_rad;
      mIncidenceAngle_rad = -incidenceAngle_rad;
      refPoint_ft.Set(mRefPoint_ft.X(), otherSurfaceY, mRefPoint_ft.Z());
      CalcSingleSurfaceMassProperties(surfaceInertiaFin2, mass_slugs, mSpan_ft, mArea_ft2, refPoint_ft);
      mSurfaceInertiaSet.push_back(surfaceInertiaFin2);
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
   else if (mSymmetryString == "Vertical")
   {
      // First, save the dihedral and incidence
      double dihedral_rad       = mDihedralAngle_rad;
      double incidenceAngle_rad = mIncidenceAngle_rad;

      UtVec3dX totalCGMass(0.0, 0.0, 0.0);
      double totalIxx = 0.0;
      double totalIyy = 0.0;
      double totalIzz = 0.0;

      // Now, determine location of other surface
      double currentSurfaceZ = mRefPoint_ft.Z();
      double delta = currentSurfaceZ - mSymmetryVerticalZ;
      double otherSurfaceZ = currentSurfaceZ - 2.0 * delta;

      // Current Fin
      GeometrySurfaceInertia surfaceInertiaFin1;
      surfaceInertiaFin1.SetParent(this);
      mDihedralAngle_rad = dihedral_rad;
      mIncidenceAngle_rad = incidenceAngle_rad;
      refPoint_ft.Set(mRefPoint_ft.X(), mRefPoint_ft.Y(), currentSurfaceZ);
      CalcSingleSurfaceMassProperties(surfaceInertiaFin1, mass_slugs, mSpan_ft, mArea_ft2, refPoint_ft);
      mSurfaceInertiaSet.push_back(surfaceInertiaFin1);
      totalIxx += mCalculatedIxx;
      totalIyy += mCalculatedIyy;
      totalIzz += mCalculatedIzz;
      totalCGMass += (mCalculatedCG_ft * mass_slugs);

      // Other Fin
      GeometrySurfaceInertia surfaceInertiaFin2;
      surfaceInertiaFin2.SetParent(this);
      mDihedralAngle_rad = -dihedral_rad;
      mIncidenceAngle_rad = -incidenceAngle_rad;
      refPoint_ft.Set(mRefPoint_ft.X(), mRefPoint_ft.Y(), otherSurfaceZ);
      CalcSingleSurfaceMassProperties(surfaceInertiaFin2, mass_slugs, mSpan_ft, mArea_ft2, refPoint_ft);
      mSurfaceInertiaSet.push_back(surfaceInertiaFin2);
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
   else if ((mSymmetryString == "+ Pattern") || (mSymmetryString == "X Pattern"))
   {
      UtVec3dX sideOffset(0.0, 0.0, 0.0);

      // These only save the dihedral (not incidence)
      double dihedral_rad       = mDihedralAngle_rad;

      UtVec3dX totalCGMass(0.0, 0.0, 0.0);
      double totalIxx = 0.0;
      double totalIyy = 0.0;
      double totalIzz = 0.0;
      double angle_rad = 0.0;

      if (mSymmetryString == "X Pattern") { angle_rad = UtMath::cPI_OVER_4; }

      // Fin 1
      GeometrySurfaceInertia surfaceInertiaFin1;
      surfaceInertiaFin1.SetParent(this);
      mDihedralAngle_rad = angle_rad;
      sideOffset.Set(0.0, mFinRefRadius_ft * cos(angle_rad), -mFinRefRadius_ft * sin(angle_rad));
      refPoint_ft = mRefPoint_ft + sideOffset;
      CalcSingleSurfaceMassProperties(surfaceInertiaFin1, mass_slugs, mSpan_ft, mArea_ft2, refPoint_ft);
      mSurfaceInertiaSet.push_back(surfaceInertiaFin1);
      totalIxx += mCalculatedIxx;
      totalIyy += mCalculatedIyy;
      totalIzz += mCalculatedIzz;
      totalCGMass += (mCalculatedCG_ft * mass_slugs);

      // Fin 2
      GeometrySurfaceInertia surfaceInertiaFin2;
      surfaceInertiaFin2.SetParent(this);
      angle_rad += UtMath::cPI_OVER_2;
      mDihedralAngle_rad = angle_rad;
      sideOffset.Set(0.0, mFinRefRadius_ft * cos(angle_rad), -mFinRefRadius_ft * sin(angle_rad));
      refPoint_ft = mRefPoint_ft + sideOffset;
      CalcSingleSurfaceMassProperties(surfaceInertiaFin2, mass_slugs, mSpan_ft, mArea_ft2, refPoint_ft);
      mSurfaceInertiaSet.push_back(surfaceInertiaFin2);
      totalIxx += mCalculatedIxx;
      totalIyy += mCalculatedIyy;
      totalIzz += mCalculatedIzz;
      totalCGMass += (mCalculatedCG_ft * mass_slugs);

      // Fin 3
      GeometrySurfaceInertia surfaceInertiaFin3;
      surfaceInertiaFin3.SetParent(this);
      angle_rad += UtMath::cPI_OVER_2;
      mDihedralAngle_rad = angle_rad;
      sideOffset.Set(0.0, mFinRefRadius_ft * cos(angle_rad), -mFinRefRadius_ft * sin(angle_rad));
      refPoint_ft = mRefPoint_ft + sideOffset;
      CalcSingleSurfaceMassProperties(surfaceInertiaFin3, mass_slugs, mSpan_ft, mArea_ft2, refPoint_ft);
      mSurfaceInertiaSet.push_back(surfaceInertiaFin3);
      totalIxx += mCalculatedIxx;
      totalIyy += mCalculatedIyy;
      totalIzz += mCalculatedIzz;
      totalCGMass += (mCalculatedCG_ft * mass_slugs);

      // Fin 4
      GeometrySurfaceInertia surfaceInertiaFin4;
      surfaceInertiaFin4.SetParent(this);
      angle_rad += UtMath::cPI_OVER_2;
      mDihedralAngle_rad = angle_rad;
      sideOffset.Set(0.0, mFinRefRadius_ft * cos(angle_rad), -mFinRefRadius_ft * sin(angle_rad));
      refPoint_ft = mRefPoint_ft + sideOffset;
      CalcSingleSurfaceMassProperties(surfaceInertiaFin4, mass_slugs, mSpan_ft, mArea_ft2, refPoint_ft);
      mSurfaceInertiaSet.push_back(surfaceInertiaFin4);
      totalIxx += mCalculatedIxx;
      totalIyy += mCalculatedIyy;
      totalIzz += mCalculatedIzz;
      totalCGMass += (mCalculatedCG_ft * mass_slugs);

      mCalculatedCG_ft = totalCGMass / (4.0 * mass_slugs);
      mCalculatedIxx = totalIxx;
      mCalculatedIyy = totalIyy;
      mCalculatedIzz = totalIzz;

      // Restore the dihedral
      mDihedralAngle_rad  = dihedral_rad;
   }
}

bool GeometrySurface::GetCG_ft(UtVec3dX& aCg_ft)
{
   aCg_ft = mCalculatedCG_ft;
   return true;
}

double GeometrySurface::GetFinSpan()
{
   if ((mSymmetryString == "+ Pattern") || (mSymmetryString == "X Pattern"))
   {
      return mFinRefRadius_ft + mSpan_ft;
   }

   double y = mRefPoint_ft.Y() + mSpan_ft * cos(mDihedralAngle_rad);
   double z = mRefPoint_ft.Z() - mSpan_ft * sin(mDihedralAngle_rad);
   return sqrt(y * y + z * z);
}

QStringList GeometrySurface::Audit()
{
   QStringList errorList = GeometryObject::Audit();

   QString nameString(mName.c_str());

   if (mSpan_ft < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a span (mSpan_ft is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   if (mRootChord_ft < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a root chord (mRootChord_ft is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   if (mTipChord_ft < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a tip chord (mTipChord_ft is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   if (mThicknessRatio < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a thickness ratio (mThicknessRatio is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   if (mOswaldsEfficiency < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires an Oswalds efficiency value (mOswaldsEfficiency is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   if (mSurfaceControlMotion == cControlMotionControlSurface)
   {
      if ((mControlSurfaceChordFractionEnd - mControlSurfaceChordFractionStart) <= std::numeric_limits<double>::epsilon())
      {
         QString errorString(" requires a control chord end fraction aft of control chord start");
         errorString = nameString + errorString;
         errorList.push_back(errorString);
      }

      if ((mControlSurfaceSpanFractionEnd - mControlSurfaceSpanFractionStart) <= std::numeric_limits<double>::epsilon())
      {
         QString errorString(" requires a control span end fraction outboard of control span start");
         errorString = nameString + errorString;
         errorList.push_back(errorString);
      }
   }

   if (mUseManualAspectRatio)
   {
      if (mManualAspectRatio < std::numeric_limits<double>::epsilon())
      {
         QString errorString(" requires a manual aspect ratio (mManualAspectRatio is zero)");
         errorString = nameString + errorString;
         errorList.push_back(errorString);
      }
   }

   return errorList;
}

void GeometrySurface::HandleOpenAirfoil()
{
   AirfoilWidget afw(this, nullptr);
   afw.setWindowFlags(afw.windowFlags() | Qt::WindowMaximizeButtonHint);
   afw.exec();
}

void GeometrySurface::PopulateAirfoilList()
{
   QString dirPath = MoverCreatorMainWindow::GetDataPath() + "/Airfoils/";
   QDir listDir(dirPath);
   if (listDir.exists())
   {
      for (auto& curItem : listDir.entryInfoList())
      {
         if (curItem.isFile())
         {
            QString fileName = curItem.baseName();
            if (fileName != "." && fileName != "..")
            {
               mAirfoilComboBox->addItem(fileName);
            }
         }
      }
   }
}

QString GeometrySurface::GetAirfoil()
{
   return mAirfoilComboBox->currentText();
}

void GeometrySurface::SetAirfoil(QString aFoilName)
{
   if (mAirfoilComboBox->findText(aFoilName) == -1)
   {
      mAirfoilComboBox->addItem(aFoilName);
   }

   mAirfoilComboBox->setCurrentText(aFoilName);
}

} // namespace Designer
