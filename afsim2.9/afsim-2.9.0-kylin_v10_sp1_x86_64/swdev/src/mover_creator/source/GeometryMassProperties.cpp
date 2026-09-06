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

#include "GeometryMassProperties.hpp"

#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QTreeWidget>
#include <QSignalBlocker>

#include "GeometryBody.hpp"
#include "GeometryFuselage.hpp"
#include "GeometryLandingGear.hpp"
#include "GeometryNacelle.hpp"
#include "GeometryPointMass.hpp"
#include "GeometryPropulsionData.hpp"
#include "GeometrySurface.hpp"
#include "GeometryWidget.hpp"
#include "MoverCreatorLineEdit.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorWidget.hpp"
#include "P6DofUtils.hpp"
#include "UtMath.hpp"
#include "VehicleGeometry.hpp"

namespace Designer
{

GeometryMassProperties::GeometryMassProperties(Vehicle* aVehicle)
   : GeometryObject(aVehicle)
   , mVehicleGeometry(aVehicle->GetGeometry())

{
   // Overwrite the GeometryObject type
   mGeometryObjectTypeString.assign("GeometryMassProperties");

   mVehiclePropulsionPtr = mVehiclePtr->GetPropulsion();
}

GeometryMassProperties::~GeometryMassProperties()
{
}

QTreeWidgetItem* GeometryMassProperties::CreateTreeWidgetItem(QString& aName, QTreeWidget* aTreeWidgetPtr, bool aNoneditableName)
{
   // Enforce the geometry type
   mGeometryObjectTypeString = "GeometryMassProperties";

   // We do not create the tree widget item at the GeometryObject level,
   // as is typically done -- we create it here instead

   QTreeWidgetItem* treeWidgetItem = new QTreeWidgetItem(aTreeWidgetPtr);

   // Save the pointer to this item
   mTreeWidgetItemPtr = treeWidgetItem;

   // Add a means to tell when the tree is clicked (so that the 3D view can be repainted)
   connect(treeWidgetItem->treeWidget(), &QTreeWidget::itemClicked, this, &GeometryMassProperties::HandleTreeItemClicked);

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

   QTreeWidgetItem* autoCalcInertia = new QTreeWidgetItem(treeWidgetItem, QStringList("Auto-Calculate Inertia"));
   QCheckBox* autoCalcInertiaCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(autoCalcInertia, 1, autoCalcInertiaCheckBox);
   aTreeWidgetPtr->setItemWidget(autoCalcInertia, 2, new QLabel(QString::fromStdString("")));
   autoCalcInertiaCheckBox->setChecked(mAutoCalcInertia);
   connect(autoCalcInertiaCheckBox, &QCheckBox::stateChanged, this, &GeometryMassProperties::HandleAutoCalcInertiaCheckBoxChanged);
   autoCalcInertia->setToolTip(0, QString("If checked, the rotational inertia will automatically be calculated\nbased on component sizes using typical component mass fractions relative\nto the total mass."));
   mAutoCalcInertia = autoCalcInertia;

   QTreeWidgetItem* emptyMass_lbs = new QTreeWidgetItem(treeWidgetItem, QStringList("Empty Mass (Specified)"));
   auto emptyMass_lbsLineEdit = new MoverCreatorLineEdit(emptyMass_lbs);
   emptyMass_lbsLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   emptyMass_lbsLineEdit->setFrame(false);
   emptyMass_lbsLineEdit->setText(QString::number(mEmptyMass_lbs));
   aTreeWidgetPtr->setItemWidget(emptyMass_lbs, 1, emptyMass_lbsLineEdit);
   aTreeWidgetPtr->setItemWidget(emptyMass_lbs, 2, new QLabel(QString::fromStdString("lbs")));
   connect(emptyMass_lbsLineEdit, &QLineEdit::textChanged, this, &GeometryMassProperties::HandleEmptyMassLineEditChanged);
   connect(emptyMass_lbsLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   emptyMass_lbs->setToolTip(0, QString("This is the empty mass of the vehicle, including engine mass but without\nany fuel or weapons. However, if solid rockets are included, the propellant\nmass will be included since it is integral to the rocket motor casing."));

   QTreeWidgetItem* totalMass_lbs = new QTreeWidgetItem(treeWidgetItem, QStringList("Total Mass (Calculated)"));
   auto totalMassLineEdit = new MoverCreatorLineEdit(totalMass_lbs);
   totalMassLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   totalMassLineEdit->setFrame(false);
   totalMassLineEdit->setText(QString::number(mTotalMass_lbs));
   aTreeWidgetPtr->setItemWidget(totalMass_lbs, 1, totalMassLineEdit);
   aTreeWidgetPtr->setItemWidget(totalMass_lbs, 2, new QLabel(QString::fromStdString("lbs")));
   connect(totalMassLineEdit, &QLineEdit::textChanged, this, &GeometryMassProperties::HandleTotalMassLineEditChanged);
   connect(totalMassLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   totalMass_lbs->setToolTip(0, QString("This is the total mass of the vehicle, including fuel (but not weapons)."));
   mTotalMassLineEdit = totalMassLineEdit;

   // These are always hidden

   QTreeWidgetItem* fuselageMassMultiplier = new QTreeWidgetItem(treeWidgetItem, QStringList("Mass Multiplier - Fuselage"));
   auto fuselageMassMultiplierLineEdit = new MoverCreatorLineEdit(fuselageMassMultiplier);
   fuselageMassMultiplierLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   fuselageMassMultiplierLineEdit->setFrame(false);
   fuselageMassMultiplierLineEdit->setText(QString::number(mFuselageBodyMassMultiplier));
   aTreeWidgetPtr->setItemWidget(fuselageMassMultiplier, 1, fuselageMassMultiplierLineEdit);
   aTreeWidgetPtr->setItemWidget(fuselageMassMultiplier, 2, new QLabel(QString::fromStdString("lbs")));
   connect(fuselageMassMultiplierLineEdit, &QLineEdit::textChanged, this, &GeometryMassProperties::HandleFuselageBodyMassMultiplierLineEditChanged);
   connect(fuselageMassMultiplierLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   fuselageMassMultiplier->setHidden(true); // This is always hidden

   QTreeWidgetItem* podsMassMultiplier = new QTreeWidgetItem(treeWidgetItem, QStringList("Mass Multiplier - Pods"));
   auto podsMassMultiplierLineEdit = new MoverCreatorLineEdit(podsMassMultiplier);
   podsMassMultiplierLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   podsMassMultiplierLineEdit->setFrame(false);
   podsMassMultiplierLineEdit->setText(QString::number(mPodsMassMultiplier));
   aTreeWidgetPtr->setItemWidget(podsMassMultiplier, 1, podsMassMultiplierLineEdit);
   aTreeWidgetPtr->setItemWidget(podsMassMultiplier, 2, new QLabel(QString::fromStdString("")));
   connect(podsMassMultiplierLineEdit, &QLineEdit::textChanged, this, &GeometryMassProperties::HandlePodsMassMultiplierLineEditChanged);
   connect(podsMassMultiplierLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   podsMassMultiplier->setHidden(true); // This is always hidden

   QTreeWidgetItem* nacellesMassMultiplier = new QTreeWidgetItem(treeWidgetItem, QStringList("Mass Multiplier - Nacelles"));
   auto nacellesMassMultiplierLineEdit = new MoverCreatorLineEdit(nacellesMassMultiplier);
   nacellesMassMultiplierLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   nacellesMassMultiplierLineEdit->setFrame(false);
   nacellesMassMultiplierLineEdit->setText(QString::number(mNacellesMassMultiplier));
   aTreeWidgetPtr->setItemWidget(nacellesMassMultiplier, 1, nacellesMassMultiplierLineEdit);
   aTreeWidgetPtr->setItemWidget(nacellesMassMultiplier, 2, new QLabel(QString::fromStdString("")));
   connect(nacellesMassMultiplierLineEdit, &QLineEdit::textChanged, this, &GeometryMassProperties::HandleNacellesMassMultiplierLineEditChanged);
   connect(nacellesMassMultiplierLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   nacellesMassMultiplier->setHidden(true); // This is always hidden

   QTreeWidgetItem* dishesMassMultiplier = new QTreeWidgetItem(treeWidgetItem, QStringList("Mass Multiplier - Dishes"));
   auto dishesMassMultiplierLineEdit = new MoverCreatorLineEdit(dishesMassMultiplier);
   dishesMassMultiplierLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   dishesMassMultiplierLineEdit->setFrame(false);
   dishesMassMultiplierLineEdit->setText(QString::number(mDishesMassMultiplier));
   aTreeWidgetPtr->setItemWidget(dishesMassMultiplier, 1, dishesMassMultiplierLineEdit);
   aTreeWidgetPtr->setItemWidget(dishesMassMultiplier, 2, new QLabel(QString::fromStdString("")));
   connect(dishesMassMultiplierLineEdit, &QLineEdit::textChanged, this, &GeometryMassProperties::HandleDishesMassMultiplierLineEditChanged);
   connect(dishesMassMultiplierLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   dishesMassMultiplier->setHidden(true); // This is always hidden

   QTreeWidgetItem* surfacesMassMultiplier = new QTreeWidgetItem(treeWidgetItem, QStringList("Mass Multiplier - Surfaces"));
   auto surfacesMassMultiplierLineEdit = new MoverCreatorLineEdit(surfacesMassMultiplier);
   surfacesMassMultiplierLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   surfacesMassMultiplierLineEdit->setFrame(false);
   surfacesMassMultiplierLineEdit->setText(QString::number(mSurfacesMassMultiplier));
   aTreeWidgetPtr->setItemWidget(surfacesMassMultiplier, 1, surfacesMassMultiplierLineEdit);
   aTreeWidgetPtr->setItemWidget(surfacesMassMultiplier, 2, new QLabel(QString::fromStdString("")));
   connect(surfacesMassMultiplierLineEdit, &QLineEdit::textChanged, this, &GeometryMassProperties::HandleSurfacesMassMultiplierLineEditChanged);
   connect(surfacesMassMultiplierLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   surfacesMassMultiplier->setHidden(true); // This is always hidden

   QTreeWidgetItem* wingMassMultiplier = new QTreeWidgetItem(treeWidgetItem, QStringList("Mass Multiplier - Wing"));
   auto wingMassMultiplierLineEdit = new MoverCreatorLineEdit(wingMassMultiplier);
   wingMassMultiplierLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   wingMassMultiplierLineEdit->setFrame(false);
   wingMassMultiplierLineEdit->setText(QString::number(mWingMassMultiplier));
   aTreeWidgetPtr->setItemWidget(wingMassMultiplier, 1, wingMassMultiplierLineEdit);
   aTreeWidgetPtr->setItemWidget(wingMassMultiplier, 2, new QLabel(QString::fromStdString("")));
   connect(wingMassMultiplierLineEdit, &QLineEdit::textChanged, this, &GeometryMassProperties::HandleWingMassMultiplierLineEditChanged);
   connect(wingMassMultiplierLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   wingMassMultiplier->setHidden(true); // This is always hidden

   QTreeWidgetItem* landingGearMassMultiplier = new QTreeWidgetItem(treeWidgetItem, QStringList("Mass Multiplier - Landing Gear"));
   auto landingGearMassMultiplierLineEdit = new MoverCreatorLineEdit(landingGearMassMultiplier);
   landingGearMassMultiplierLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   landingGearMassMultiplierLineEdit->setFrame(false);
   landingGearMassMultiplierLineEdit->setText(QString::number(mLandingGearMassMultiplier));
   aTreeWidgetPtr->setItemWidget(landingGearMassMultiplier, 1, landingGearMassMultiplierLineEdit);
   aTreeWidgetPtr->setItemWidget(landingGearMassMultiplier, 2, new QLabel(QString::fromStdString("")));
   connect(landingGearMassMultiplierLineEdit, &QLineEdit::textChanged, this, &GeometryMassProperties::HandleLandingGearMassMultiplierLineEditChanged);
   connect(landingGearMassMultiplierLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   landingGearMassMultiplier->setHidden(true); // This is always hidden

   // These are read only

   QTreeWidgetItem* fuselageMass_lbs = new QTreeWidgetItem(treeWidgetItem, QStringList("Mass - Fuselage/Body"));
   auto fuselageMassLineEdit = new MoverCreatorLineEdit(fuselageMass_lbs);
   fuselageMassLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   fuselageMassLineEdit->setFrame(false);
   fuselageMassLineEdit->setText(QString::number(mFuselageBodyMass_lbs));
   aTreeWidgetPtr->setItemWidget(fuselageMass_lbs, 1, fuselageMassLineEdit);
   aTreeWidgetPtr->setItemWidget(fuselageMass_lbs, 2, new QLabel(QString::fromStdString("lbs")));
   connect(fuselageMassLineEdit, &QLineEdit::textChanged, this, &GeometryMassProperties::HandleFuselageBodyMassLineEditChanged);
   connect(fuselageMassLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   fuselageMass_lbs->setToolTip(0, QString("This is the mass of the fuselage/body of the vehicle"));
   mMassFuselageBodyMassLineEdit = fuselageMassLineEdit;

   QTreeWidgetItem* podsMass_lbs = new QTreeWidgetItem(treeWidgetItem, QStringList("Mass - Pods"));
   auto podsMassLineEdit = new MoverCreatorLineEdit(podsMass_lbs);
   podsMassLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   podsMassLineEdit->setFrame(false);
   podsMassLineEdit->setText(QString::number(mPodsMass_lbs));
   aTreeWidgetPtr->setItemWidget(podsMass_lbs, 1, podsMassLineEdit);
   aTreeWidgetPtr->setItemWidget(podsMass_lbs, 2, new QLabel(QString::fromStdString("lbs")));
   connect(podsMassLineEdit, &QLineEdit::textChanged, this, &GeometryMassProperties::HandlePodsMassLineEditChanged);
   connect(podsMassLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   podsMass_lbs->setToolTip(0, QString("This is the mass of the pods (bodies of revolution) of the vehicle"));
   mMassPodsMassLineEdit = podsMassLineEdit;

   QTreeWidgetItem* nacellesMass_lbs = new QTreeWidgetItem(treeWidgetItem, QStringList("Mass - Nacelles"));
   auto nacellesMassLineEdit = new MoverCreatorLineEdit(nacellesMass_lbs);
   nacellesMassLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   nacellesMassLineEdit->setFrame(false);
   nacellesMassLineEdit->setText(QString::number(mNacellesMass_lbs));
   aTreeWidgetPtr->setItemWidget(nacellesMass_lbs, 1, nacellesMassLineEdit);
   aTreeWidgetPtr->setItemWidget(nacellesMass_lbs, 2, new QLabel(QString::fromStdString("lbs")));
   connect(nacellesMassLineEdit, &QLineEdit::textChanged, this, &GeometryMassProperties::HandleNacellesMassLineEditChanged);
   connect(nacellesMassLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   nacellesMass_lbs->setToolTip(0, QString("This is the mass of the nacelles of the vehicle"));
   mMassNacellesMassLineEdit = nacellesMassLineEdit;

   QTreeWidgetItem* dishesMass_lbs = new QTreeWidgetItem(treeWidgetItem, QStringList("Mass - Dishes"));
   auto dishesMassLineEdit = new MoverCreatorLineEdit(dishesMass_lbs);
   dishesMassLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   dishesMassLineEdit->setFrame(false);
   dishesMassLineEdit->setText(QString::number(mDishesMass_lbs));
   aTreeWidgetPtr->setItemWidget(dishesMass_lbs, 1, dishesMassLineEdit);
   aTreeWidgetPtr->setItemWidget(dishesMass_lbs, 2, new QLabel(QString::fromStdString("lbs")));
   connect(dishesMassLineEdit, &QLineEdit::textChanged, this, &GeometryMassProperties::HandleDishesMassLineEditChanged);
   connect(dishesMassLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   dishesMass_lbs->setToolTip(0, QString("This is the mass of the dishes of the vehicle"));
   mMassDishesMassLineEdit = dishesMassLineEdit;

   QTreeWidgetItem* surfacesMass_lbs = new QTreeWidgetItem(treeWidgetItem, QStringList("Mass - Surfaces"));
   auto surfacesMassLineEdit = new MoverCreatorLineEdit(surfacesMass_lbs);
   surfacesMassLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   surfacesMassLineEdit->setFrame(false);
   surfacesMassLineEdit->setText(QString::number(mSurfacesMass_lbs));
   aTreeWidgetPtr->setItemWidget(surfacesMass_lbs, 1, surfacesMassLineEdit);
   aTreeWidgetPtr->setItemWidget(surfacesMass_lbs, 2, new QLabel(QString::fromStdString("lbs")));
   connect(surfacesMassLineEdit, &QLineEdit::textChanged, this, &GeometryMassProperties::HandleSurfacesMassLineEditChanged);
   connect(surfacesMassLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   surfacesMass_lbs->setToolTip(0, QString("This is the mass of the (non-wing) aerodynamic surfaces of the vehicle"));
   mMassSurfacesMassLineEdit = surfacesMassLineEdit;

   QTreeWidgetItem* wingMass_lbs = new QTreeWidgetItem(treeWidgetItem, QStringList("Mass - Wing"));
   auto wingMassLineEdit = new MoverCreatorLineEdit(wingMass_lbs);
   wingMassLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   wingMassLineEdit->setFrame(false);
   wingMassLineEdit->setText(QString::number(mWingMass_lbs));
   aTreeWidgetPtr->setItemWidget(wingMass_lbs, 1, wingMassLineEdit);
   aTreeWidgetPtr->setItemWidget(wingMass_lbs, 2, new QLabel(QString::fromStdString("lbs")));
   connect(wingMassLineEdit, &QLineEdit::textChanged, this, &GeometryMassProperties::HandleWingMassLineEditChanged);
   connect(wingMassLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   wingMass_lbs->setToolTip(0, QString("This is the mass of the wing of the vehicle"));
   mMassWingMassLineEdit = wingMassLineEdit;

   QTreeWidgetItem* gearMass_lbs = new QTreeWidgetItem(treeWidgetItem, QStringList("Mass - Landing Gear"));
   auto gearMassLineEdit = new MoverCreatorLineEdit(gearMass_lbs);
   gearMassLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   gearMassLineEdit->setFrame(false);
   gearMassLineEdit->setText(QString::number(mLandingGearMass_lbs));
   aTreeWidgetPtr->setItemWidget(gearMass_lbs, 1, gearMassLineEdit);
   aTreeWidgetPtr->setItemWidget(gearMass_lbs, 2, new QLabel(QString::fromStdString("lbs")));
   connect(gearMassLineEdit, &QLineEdit::textChanged, this, &GeometryMassProperties::HandleLandingGearMassLineEditChanged);
   connect(gearMassLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   gearMass_lbs->setToolTip(0, QString("This is the mass of the landing gear of the vehicle"));
   mMassLandingGearMassLineEdit = gearMassLineEdit;

   QTreeWidgetItem* pointMasses_lbs = new QTreeWidgetItem(treeWidgetItem, QStringList("Mass - Point Masses"));
   auto pointMassesLineEdit = new MoverCreatorLineEdit(pointMasses_lbs);
   pointMassesLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   pointMassesLineEdit->setFrame(false);
   pointMassesLineEdit->setText(QString::number(mPointMassesMass_lbs));
   aTreeWidgetPtr->setItemWidget(pointMasses_lbs, 1, pointMassesLineEdit);
   aTreeWidgetPtr->setItemWidget(pointMasses_lbs, 2, new QLabel(QString::fromStdString("lbs")));
   connect(pointMassesLineEdit, &QLineEdit::textChanged, this, &GeometryMassProperties::HandlePointMassesLineEditChanged);
   connect(pointMassesLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   pointMasses_lbs->setToolTip(0, QString("This is the sum of all point masses of the vehicle"));
   mPointMassesLineEdit = pointMassesLineEdit;

   QTreeWidgetItem* inertiaIxxItem = new QTreeWidgetItem(treeWidgetItem, QStringList("Rotational Inertia Ixx"));
   auto inertiaIxxLineEdit = new MoverCreatorLineEdit(inertiaIxxItem);
   inertiaIxxLineEdit->setFrame(false);
   inertiaIxxLineEdit->setText(QString::number(mIxx));
   aTreeWidgetPtr->setItemWidget(inertiaIxxItem, 1, inertiaIxxLineEdit);
   aTreeWidgetPtr->setItemWidget(inertiaIxxItem, 2, new QLabel(QString::fromStdString("slug/ft2")));
   connect(inertiaIxxLineEdit, &QLineEdit::textChanged, this, &GeometryMassProperties::HandleIxxLineEditChanged);
   connect(inertiaIxxLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   inertiaIxxItem->setToolTip(0, QString("This is the rotational inertia about the x-axis, which runs fore/aft (positive forward)"));
   mInertiaIxxLineEdit = inertiaIxxLineEdit;

   QTreeWidgetItem* inertiaIyyItem = new QTreeWidgetItem(treeWidgetItem, QStringList("Rotational Inertia Iyy"));
   auto inertiaIyyLineEdit = new MoverCreatorLineEdit(inertiaIyyItem);
   inertiaIyyLineEdit->setFrame(false);
   inertiaIyyLineEdit->setText(QString::number(mIyy));
   aTreeWidgetPtr->setItemWidget(inertiaIyyItem, 1, inertiaIyyLineEdit);
   aTreeWidgetPtr->setItemWidget(inertiaIyyItem, 2, new QLabel(QString::fromStdString("slug/ft2")));
   connect(inertiaIyyLineEdit, &QLineEdit::textChanged, this, &GeometryMassProperties::HandleIyyLineEditChanged);
   connect(inertiaIyyLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   inertiaIyyItem->setToolTip(0, QString("This is the rotational inertia about the y-axis, which runs side-to-side (positive right)"));
   mInertiaIyyLineEdit = inertiaIyyLineEdit;

   QTreeWidgetItem* inertiaIzzItem = new QTreeWidgetItem(treeWidgetItem, QStringList("Rotational Inertia Izz"));
   auto inertiaIzzLineEdit = new MoverCreatorLineEdit(inertiaIzzItem);
   inertiaIzzLineEdit->setFrame(false);
   inertiaIzzLineEdit->setText(QString::number(mIzz));
   aTreeWidgetPtr->setItemWidget(inertiaIzzItem, 1, inertiaIzzLineEdit);
   aTreeWidgetPtr->setItemWidget(inertiaIzzItem, 2, new QLabel(QString::fromStdString("slug/ft2")));
   connect(inertiaIzzLineEdit, &QLineEdit::textChanged, this, &GeometryMassProperties::HandleIzzLineEditChanged);
   connect(inertiaIzzLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   inertiaIzzItem->setToolTip(0, QString("This is the rotational inertia about the z-axis, which runs vertically (positive down)"));
   mInertiaIzzLineEdit = inertiaIzzLineEdit;

   // Setup the cg point

   QTreeWidgetItem* refPoint = new QTreeWidgetItem(treeWidgetItem, QStringList("CG Point"));
   refPoint->setToolTip(0, QString("This is the location of center-of-mass (center-of-gravity, cg).\nIt will be automatically calculated if 'Auto-Calculate Inertia' (above) is checked."));

   QTreeWidgetItem* x = new QTreeWidgetItem(refPoint, QStringList("x"));
   auto xLineEdit = new MoverCreatorLineEdit(x);
   xLineEdit->setFrame(false);
   xLineEdit->setText(QString::number(mCG_Location_ft.X()));
   aTreeWidgetPtr->setItemWidget(x, 1, xLineEdit);
   aTreeWidgetPtr->setItemWidget(x, 2, new QLabel(QString::fromStdString("ft")));
   connect(xLineEdit, &QLineEdit::textChanged, this, &GeometryMassProperties::HandleX_CoordinateLineEditChanged);
   connect(xLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mCGPointX = xLineEdit;

   QTreeWidgetItem* y = new QTreeWidgetItem(refPoint, QStringList("y"));
   auto yLineEdit = new MoverCreatorLineEdit(y);
   mReferencePointY_TreeWidgetItemPtr = y;
   yLineEdit->setFrame(false);
   yLineEdit->setText(QString::number(mCG_Location_ft.Y()));
   aTreeWidgetPtr->setItemWidget(y, 1, yLineEdit);
   aTreeWidgetPtr->setItemWidget(y, 2, new QLabel(QString::fromStdString("ft")));
   connect(yLineEdit, &QLineEdit::textChanged, this, &GeometryMassProperties::HandleY_CoordinateLineEditChanged);
   connect(yLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mCGPointY = yLineEdit;

   QTreeWidgetItem* z = new QTreeWidgetItem(refPoint, QStringList("z"));
   auto zLineEdit = new MoverCreatorLineEdit(z);
   zLineEdit->setFrame(false);
   zLineEdit->setText(QString::number(mCG_Location_ft.Z()));
   aTreeWidgetPtr->setItemWidget(z, 1, zLineEdit);
   aTreeWidgetPtr->setItemWidget(z, 2, new QLabel(QString::fromStdString("ft")));
   connect(zLineEdit, &QLineEdit::textChanged, this, &GeometryMassProperties::HandleZ_CoordinateLineEditChanged);
   connect(zLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   mCGPointZ = zLineEdit;

   QTreeWidgetItem* mayNotBeDeleted = new QTreeWidgetItem(treeWidgetItem, QStringList("May Not Be Deleted"));
   QCheckBox* noDeleteCheckBox = new QCheckBox();
   aTreeWidgetPtr->setItemWidget(mayNotBeDeleted, 1, noDeleteCheckBox);
   aTreeWidgetPtr->setItemWidget(mayNotBeDeleted, 2, new QLabel(QString::fromStdString("")));
   connect(noDeleteCheckBox, &QCheckBox::stateChanged, this, &GeometryMassProperties::HandleMayNotBeDeletedCheckBoxChanged);
   // We hide this so it won't be shown in GUIs -- we need it to still exist for JSON I/O
   mayNotBeDeleted->setHidden(true);

   QTreeWidgetItem* massFactor = new QTreeWidgetItem(treeWidgetItem, QStringList("Mass Factor"));
   auto massFactorLineEdit = new MoverCreatorLineEdit(massFactor);
   massFactorLineEdit->setValidator(new QDoubleValidator(0.0, DBL_MAX, 10));
   massFactorLineEdit->setFrame(false);
   massFactorLineEdit->setText(QString::number(mMassFactor));
   aTreeWidgetPtr->setItemWidget(massFactor, 1, massFactorLineEdit);
   aTreeWidgetPtr->setItemWidget(massFactor, 2, new QLabel(QString::fromStdString("")));
   connect(massFactorLineEdit, &QLineEdit::textChanged, this, &GeometryMassProperties::HandleMassFactorLineEditChanged);
   connect(massFactorLineEdit, &MoverCreatorLineEdit::clicked, [this]() {AdjustAndRedraw(); });
   massFactor->setHidden(true);

   return treeWidgetItem;
}

bool GeometryMassProperties::CalcExtents(double&, double&, double&, double&, double&, double&)
{
   return false;
}

void GeometryMassProperties::HandleMayNotBeDeletedCheckBoxChanged(int aValue)
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
         QPixmap deletePixmap = QPixmap(QString::fromUtf8(":/icons/%1/64x64/delete_2.png").arg(MoverCreatorMainWindow::GetInstance().GetTheme()));
         mDeleteButton->setIcon(QIcon(deletePixmap));
         mDeleteButton->setEnabled(true);
      }
   }

   AdjustAndRedraw();
}

void GeometryMassProperties::HandleTreeItemClicked(QTreeWidgetItem* item, int column)
{
   AdjustAndRedraw();
}

void GeometryMassProperties::HandleEmptyMassLineEditChanged(QString aValue)
{
   mEmptyMass_lbs = aValue.toDouble();
   AdjustAndRedraw(true);
}

void GeometryMassProperties::HandleTotalMassLineEditChanged(QString aValue)
{
   mTotalMass_lbs = aValue.toDouble();
}

void GeometryMassProperties::HandleAutoCalcInertiaCheckBoxChanged(int aValue)
{
   if (aValue == 0)
   {
      mAutoCalcInertia = false;
      mIxx = mInertiaIxxLineEdit->text().toDouble();
      mIyy = mInertiaIyyLineEdit->text().toDouble();
      mIzz = mInertiaIzzLineEdit->text().toDouble();
   }
   else
   {
      mAutoCalcInertia = true;
   }

   AdjustAndRedraw(true);
}

void GeometryMassProperties::HandleIxxLineEditChanged(QString aValue)
{
   mIxx = aValue.toDouble();
   AdjustAndRedraw(false);
}

void GeometryMassProperties::HandleIyyLineEditChanged(QString aValue)
{
   mIyy = aValue.toDouble();
   AdjustAndRedraw(false);
}

void GeometryMassProperties::HandleIzzLineEditChanged(QString aValue)
{
   mIzz = aValue.toDouble();
   AdjustAndRedraw(false);
}

void GeometryMassProperties::HandleFuselageBodyMassMultiplierLineEditChanged(QString aValue)
{
   mFuselageBodyMassMultiplier = aValue.toDouble();
}

void GeometryMassProperties::HandlePodsMassMultiplierLineEditChanged(QString aValue)
{
   mPodsMassMultiplier = aValue.toDouble();
}

void GeometryMassProperties::HandleNacellesMassMultiplierLineEditChanged(QString aValue)
{
   mNacellesMassMultiplier = aValue.toDouble();
}

void GeometryMassProperties::HandleDishesMassMultiplierLineEditChanged(QString aValue)
{
   mDishesMassMultiplier = aValue.toDouble();
}

void GeometryMassProperties::HandleSurfacesMassMultiplierLineEditChanged(QString aValue)
{
   mSurfacesMassMultiplier = aValue.toDouble();
}

void GeometryMassProperties::HandleWingMassMultiplierLineEditChanged(QString aValue)
{
   mWingMassMultiplier = aValue.toDouble();
}

void GeometryMassProperties::HandleLandingGearMassMultiplierLineEditChanged(QString aValue)
{
   mLandingGearMassMultiplier = aValue.toDouble();
}

void GeometryMassProperties::HandleFuselageBodyMassLineEditChanged(QString aValue)
{
   mFuselageBodyMass_lbs = aValue.toDouble();
}

void GeometryMassProperties::HandlePodsMassLineEditChanged(QString aValue)
{
   mPodsMass_lbs = aValue.toDouble();
}

void GeometryMassProperties::HandleNacellesMassLineEditChanged(QString aValue)
{
   mNacellesMass_lbs = aValue.toDouble();
}

void GeometryMassProperties::HandleDishesMassLineEditChanged(QString aValue)
{
   mDishesMass_lbs = aValue.toDouble();
}

void GeometryMassProperties::HandleSurfacesMassLineEditChanged(QString aValue)
{
   mSurfacesMass_lbs = aValue.toDouble();
}

void GeometryMassProperties::HandleWingMassLineEditChanged(QString aValue)
{
   mWingMass_lbs = aValue.toDouble();
}

void GeometryMassProperties::HandleLandingGearMassLineEditChanged(QString aValue)
{
   mLandingGearMass_lbs = aValue.toDouble();
}

void GeometryMassProperties::HandlePointMassesLineEditChanged(QString aValue)
{
   mPointMassesMass_lbs = aValue.toDouble();
}

void GeometryMassProperties::CalcMassProperties(double) // Although a derived function, it ignores aTotalMass_lbs
{
   // Mass properties itself lacks any mass properties to calculate

   // Set the mass - always zero
   mCalculatedMass_lbs = 0.0;

   // Set inertia - always zero
   mCalculatedIxx = 0.0;
   mCalculatedIyy = 0.0;
   mCalculatedIzz = 0.0;

   // Set cg - always zero
   mCalculatedCG_ft.Set(0.0, 0.0, 0.0);
}

bool GeometryMassProperties::GetCalculatedMassProperties(UtVec3dX& aCgLocation_ft,
                                                         double&   aMass_lbs,
                                                         UtVec3dX& aRotationalInertia)
{
   aCgLocation_ft = mCalculatedCG_ft;
   aMass_lbs = mCalculatedMass_lbs;
   aRotationalInertia.Set(mCalculatedIxx, mCalculatedIyy, mCalculatedIzz);

   return false; // Mass properties itself lacks any mass properties
}

void GeometryMassProperties::CalcMassAndInertiaProperties()
{
   if (mEmptyMass_lbs <= 0) { return; }

   double sumOfMultipliers = mFuselageBodyMassMultiplier +
                             mPodsMassMultiplier +
                             mNacellesMassMultiplier +
                             mDishesMassMultiplier +
                             mSurfacesMassMultiplier +
                             mWingMassMultiplier +
                             mLandingGearMassMultiplier;

   if (sumOfMultipliers <= 0) { return; }

   // Be sure that mass calculations handle symmetry properly
   double fuselageBodyRelativeMass = CalcFuselageBodyRelativeMass();
   double podsRelativeMass         = CalcPodsRelativeMass();
   double nacellesRelativeMass     = CalcNacellesRelativeMass();
   double dishesRelativeMass       = CalcDishesRelativeMass();
   double surfacesRelativeMass     = CalcSurfacesRelativeMass();
   double wingRelativeMass         = CalcWingRelativeMass();
   double landingGearRelativeMass  = CalcLandingGearRelativeMass();

   double totalRelativeMass = fuselageBodyRelativeMass + podsRelativeMass + nacellesRelativeMass
                              + dishesRelativeMass + surfacesRelativeMass + wingRelativeMass
                              + landingGearRelativeMass;

   if (totalRelativeMass <= 0) { return; }

   double totalEngineMass_lbs = CalcEnginesMass_lbs(); // This is similar to the "Call<object>Mass" functions below

   mPointMassesMass_lbs = CalcPointMasses_lbs();

   double structuralWeight_lbs = mEmptyMass_lbs - mPointMassesMass_lbs - totalEngineMass_lbs;

   mFuselageBodyMass_lbs = structuralWeight_lbs * fuselageBodyRelativeMass / totalRelativeMass;
   mPodsMass_lbs         = structuralWeight_lbs * podsRelativeMass / totalRelativeMass;
   mNacellesMass_lbs     = structuralWeight_lbs * nacellesRelativeMass / totalRelativeMass;
   mDishesMass_lbs       = structuralWeight_lbs * dishesRelativeMass / totalRelativeMass;
   mSurfacesMass_lbs     = structuralWeight_lbs * surfacesRelativeMass / totalRelativeMass;
   mWingMass_lbs         = structuralWeight_lbs * wingRelativeMass / totalRelativeMass;
   mLandingGearMass_lbs  = structuralWeight_lbs * landingGearRelativeMass / totalRelativeMass;

   // This is multiplied with each object's relative mass to get actual mass
   double realtiveMassMultiplier_lbs = structuralWeight_lbs / totalRelativeMass;

   // Each object provides its "mass area" via GetMassArea_ft2()
   // Each object provides its "mass factor" via GetMassFactor()
   // GeometryMassProperties has CalcObjectRelativeMass functions -- pass in surface area, mass factor, mass multiplier to get relative mass
   // Sum the relative mass of all objects and then calc normalized mass for each object
   // Should objects maintain their own mass?

   // Temporarily block signals for lineEdits
   const QSignalBlocker massFuselageBlocker(mMassFuselageBodyMassLineEdit);   //should unblock signals once it falls out of scope
   const QSignalBlocker massPodBlocker(mMassPodsMassLineEdit);  //should unblock signals once it falls out of scope
   const QSignalBlocker massNacellesBlocker(mMassNacellesMassLineEdit);  //should unblock signals once it falls out of scope
   const QSignalBlocker massDishesBlocker(mMassDishesMassLineEdit);  //should unblock signals once it falls out of scope
   const QSignalBlocker massSurfacesBlocker(mMassSurfacesMassLineEdit);  //should unblock signals once it falls out of scope
   const QSignalBlocker massWingBlocker(mMassWingMassLineEdit);  //should unblock signals once it falls out of scope
   const QSignalBlocker massLandingGearBlocker(mMassLandingGearMassLineEdit);  //should unblock signals once it falls out of scope
   const QSignalBlocker massPointsBlocker(mPointMassesLineEdit);  //should unblock signals once it falls out of scope
   const QSignalBlocker massTotalBlocker(mTotalMassLineEdit);  //should unblock signals once it falls out of scope

   // Set GUI elements with new mass values
   if (mMassFuselageBodyMassLineEdit != nullptr) { mMassFuselageBodyMassLineEdit->setText(QString::number(mFuselageBodyMass_lbs)); }
   if (mMassPodsMassLineEdit != nullptr) { mMassPodsMassLineEdit->setText(QString::number(mPodsMass_lbs)); }
   if (mMassNacellesMassLineEdit != nullptr) { mMassNacellesMassLineEdit->setText(QString::number(mNacellesMass_lbs)); }
   if (mMassDishesMassLineEdit != nullptr) { mMassDishesMassLineEdit->setText(QString::number(mDishesMass_lbs)); }
   if (mMassSurfacesMassLineEdit != nullptr) { mMassSurfacesMassLineEdit->setText(QString::number(mSurfacesMass_lbs)); }
   if (mMassWingMassLineEdit != nullptr) { mMassWingMassLineEdit->setText(QString::number(mWingMass_lbs)); }
   if (mMassLandingGearMassLineEdit != nullptr) { mMassLandingGearMassLineEdit->setText(QString::number(mLandingGearMass_lbs)); }
   if (mPointMassesLineEdit != nullptr) { mPointMassesLineEdit->setText(QString::number(mPointMassesMass_lbs)); }

   // Calculate total mass, which includes fuel, and set the GUI text
   double fuelMass_lbs = CalcFuelMass_lbs();
   mTotalMass_lbs = mEmptyMass_lbs + fuelMass_lbs;
   if (mTotalMassLineEdit != nullptr) { mTotalMassLineEdit->setText(QString::number(mTotalMass_lbs)); }

   // Need to have all geometry objects include "Calculated Wgt", "Calculated Inertia - Ixx",
   // "Calculated Inertia - Iyy", and "Calculated Inertia - Izz"

   // Be sure that mass calculations handle symmetry properly

   CalcFuselageBodyMass(realtiveMassMultiplier_lbs); // These call CalcMassProperties(double aTotalMass_lbs); ******************************************************
   CalcPodsMass(realtiveMassMultiplier_lbs);
   CalcNacellesMass(realtiveMassMultiplier_lbs);
   CalcDishesMass(realtiveMassMultiplier_lbs);
   CalcSurfacesMass(realtiveMassMultiplier_lbs);
   CalcWingMass(realtiveMassMultiplier_lbs);
   CalcLandingGearMass(realtiveMassMultiplier_lbs);
   CalcPointMassesMass(realtiveMassMultiplier_lbs);
   CalcPropulsionMass(realtiveMassMultiplier_lbs);

   CalculateTotalMassProperties();

   if (mCGPointX != nullptr) { mCGPointX->setText(QString::number(mCG_Location_ft.X())); }
   if (mCGPointY != nullptr) { mCGPointY->setText(QString::number(mCG_Location_ft.Y())); }
   if (mCGPointZ != nullptr) { mCGPointZ->setText(QString::number(mCG_Location_ft.Z())); }
}

double GeometryMassProperties::CalcObjectRelativeMass(double aMassArea_ft2,
                                                      double aObjectectMassFactor,
                                                      double aObjectCategoryMassMultiplier)
{
   return aMassArea_ft2 * aObjectectMassFactor * aObjectCategoryMassMultiplier;
}

double GeometryMassProperties::CalcFuelMass_lbs()
{
   const std::map<size_t, GeometryObject*>& geometryObjMap = mVehicleGeometry.GetGeometryObjectMap();

   double totalFuelMass_lbs = 0.0;

   // Loop through objects, looking for PropulsionData
   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if (geometryType == "GeometryPropulsionData")
      {
         GeometryPropulsionData* propulsion = dynamic_cast<GeometryPropulsionData*>(geometryObject);
         if (propulsion->FuelTankPresent())
         {
            totalFuelMass_lbs += propulsion->GetFuelQtyMax();
         }
      }
      else if (geometryType == "GeometryEngine")
      {
         GeometryEngine* engine = dynamic_cast<GeometryEngine*>(geometryObject);
         totalFuelMass_lbs += engine->GetPropellantMass_lbs();
      }
   }

   return totalFuelMass_lbs;
}

double GeometryMassProperties::CalcPointMasses_lbs()
{
   const std::map<size_t, GeometryObject*>& geometryObjMap = mVehicleGeometry.GetGeometryObjectMap();

   double totalPointMass_lbs = 0.0;

   // Loop through objects, looking for GeometryPointMass
   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if (geometryType == "GeometryPointMass")
      {
         GeometryPointMass* ptMass = dynamic_cast<GeometryPointMass*>(geometryObject);
         double mass_lbs = ptMass->GetPointMass_lbs();
         if (mass_lbs > 0)
         {
            totalPointMass_lbs += mass_lbs;
         }
      }
   }

   return totalPointMass_lbs;
}

double GeometryMassProperties::CalcEnginesMass_lbs()
{
   const std::map<size_t, GeometryObject*>& geometryObjMap = mVehicleGeometry.GetGeometryObjectMap();

   double totalEngineMass_lbs = 0.0;

   // Loop through objects, looking for GeometryEngine
   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if (geometryType == "GeometryEngine")
      {
         geometryObject->CalcMassProperties(0);
         double mass_lbs = 0.0;
         UtVec3dX cgLocation_ft;
         UtVec3dX rotInertia(0.0, 0.0, 0.0);
         if (geometryObject->GetCalculatedMassProperties(cgLocation_ft, mass_lbs, rotInertia))
         {
            totalEngineMass_lbs += mass_lbs;
         }
      }
   }

   return totalEngineMass_lbs;
}

double GeometryMassProperties::CalcFuselageBodyRelativeMass()
{
   const std::map<size_t, GeometryObject*>& geometryObjMap = mVehicleGeometry.GetGeometryObjectMap();

   double totalRelativeMass = 0.0;

   // Loop through objects, looking for fuselages and bodies
   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      bool valid = false;
      bool symmetrical = false;
      if (geometryType == "GeometryFuselage")
      {
         GeometryFuselage* geometryFuselage = dynamic_cast<GeometryFuselage*>(geometryObject);
         valid = true;
         symmetrical = geometryFuselage->IsSymmetrical();
      }
      else if (geometryType == "GeometryBody")
      {
         GeometryBody* geometryBody = dynamic_cast<GeometryBody*>(geometryObject);
         if (geometryBody->IsMainBody())
         {
            valid = true;
         }

         symmetrical = geometryBody->IsSymmetrical();
      }
      if (valid)
      {
         // Add in the relative mass for this object
         double massArea_ft2 = geometryObject->GetMassArea_ft2();
         double massFactor = geometryObject->GetMassFactor();

         if (symmetrical) { massArea_ft2 *= 2.0; }

         totalRelativeMass += CalcObjectRelativeMass(massArea_ft2, massFactor, mFuselageBodyMassMultiplier);
      }
   }

   return totalRelativeMass;
}

double GeometryMassProperties::CalcPodsRelativeMass()
{
   const std::map<size_t, GeometryObject*>& geometryObjMap = mVehicleGeometry.GetGeometryObjectMap();

   double totalRelativeMass = 0.0;

   // Loop through objects, looking for GeometryBody objects which are not IsMainBody()
   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if (geometryType == "GeometryBody")
      {
         GeometryBody* geometryBody = dynamic_cast<GeometryBody*>(geometryObject);
         if (!geometryBody->IsMainBody())
         {
            // Add in the relative mass for this object
            double massArea_ft2 = geometryObject->GetMassArea_ft2();
            double massFactor = geometryObject->GetMassFactor();

            if (geometryBody->IsSymmetrical()) { massArea_ft2 *= 2.0; }

            totalRelativeMass += CalcObjectRelativeMass(massArea_ft2, massFactor, mPodsMassMultiplier);
         }
      }
   }

   return totalRelativeMass;
}

double GeometryMassProperties::CalcNacellesRelativeMass()
{
   const std::map<size_t, GeometryObject*>& geometryObjMap = mVehicleGeometry.GetGeometryObjectMap();

   double totalRelativeMass = 0.0;

   // Loop through objects, looking for GeometryNacelle
   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if (geometryType == "GeometryNacelle")
      {
         GeometryNacelle* geometryNacelle = dynamic_cast<GeometryNacelle*>(geometryObject);

         // Add in the relative mass for this object
         double massArea_ft2 = geometryObject->GetMassArea_ft2();
         double massFactor = geometryObject->GetMassFactor();

         if (geometryNacelle->IsSymmetrical()) { massArea_ft2 *= 2.0; }

         totalRelativeMass += CalcObjectRelativeMass(massArea_ft2, massFactor, mNacellesMassMultiplier);
      }
   }

   return totalRelativeMass;
}

double GeometryMassProperties::CalcDishesRelativeMass()
{
   const std::map<size_t, GeometryObject*>& geometryObjMap = mVehicleGeometry.GetGeometryObjectMap();

   double totalRelativeMass = 0.0;

   // Loop through objects, looking for GeometryDish
   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if (geometryType == "GeometryDish")
      {
         // Add in the relative mass for this object
         double massArea_ft2 = geometryObject->GetMassArea_ft2();
         double massFactor = geometryObject->GetMassFactor();

         // Note -- Dishes do not support symmetry

         totalRelativeMass += CalcObjectRelativeMass(massArea_ft2, massFactor, mDishesMassMultiplier);
      }
   }

   return totalRelativeMass;
}

double GeometryMassProperties::CalcSurfacesRelativeMass()
{
   const std::map<size_t, GeometryObject*>& geometryObjMap = mVehicleGeometry.GetGeometryObjectMap();

   double totalRelativeMass = 0.0;

   // Loop through objects, looking for GeometrySurface
   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if (geometryType == "GeometrySurface")
      {
         GeometrySurface* geometrySurface = dynamic_cast<GeometrySurface*>(geometryObject);

         // Add in the relative mass for this object
         double massArea_ft2 = geometryObject->GetMassArea_ft2();
         double massFactor = geometryObject->GetMassFactor();

         if (geometrySurface->IsSymmetrical())
         {
            std::string symmetryString = geometrySurface->GetSymmetryString().toStdString();

            // No impact for "Single"

            if ((symmetryString == "Horizontal") || (symmetryString == "Vertical"))
            {
               massArea_ft2 *= 2.0;
            }
            else if ((symmetryString == "+ Pattern") || (symmetryString == "X Pattern"))
            {
               massArea_ft2 *= 4.0;
            }
         }

         totalRelativeMass += CalcObjectRelativeMass(massArea_ft2, massFactor, mSurfacesMassMultiplier);
      }
   }

   return totalRelativeMass;
}

double GeometryMassProperties::CalcWingRelativeMass()
{
   const std::map<size_t, GeometryObject*>& geometryObjMap = mVehicleGeometry.GetGeometryObjectMap();

   double totalRelativeMass = 0.0;

   // Loop through objects, looking for GeometryWing
   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if (geometryType == "GeometryWing")
      {
         // Add in the relative mass for this object
         double massArea_ft2 = geometryObject->GetMassArea_ft2();
         double massFactor = geometryObject->GetMassFactor();
         totalRelativeMass += CalcObjectRelativeMass(massArea_ft2, massFactor, mWingMassMultiplier);
      }
   }

   return totalRelativeMass;
}

double GeometryMassProperties::CalcLandingGearRelativeMass()
{
   const std::map<size_t, GeometryObject*>& geometryObjMap = mVehicleGeometry.GetGeometryObjectMap();

   double totalRelativeMass = 0.0;

   // Loop through objects, looking for GeometryLandingGear
   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if (geometryType == "GeometryLandingGear")
      {
         GeometryLandingGear* geometryGear = dynamic_cast<GeometryLandingGear*>(geometryObject);

         // Add in the relative mass for this object
         double massArea_ft2 = geometryObject->GetMassArea_ft2();
         double massFactor = geometryObject->GetMassFactor();

         if (geometryGear->IsSymmetrical()) { massArea_ft2 *= 2.0; }

         totalRelativeMass += CalcObjectRelativeMass(massArea_ft2, massFactor, mLandingGearMassMultiplier);
      }
   }

   return totalRelativeMass;
}

void GeometryMassProperties::CalcFuselageBodyMass(double aRealtiveMassMultiplier_lbs)
{
   const std::map<size_t, GeometryObject*>& geometryObjMap = mVehicleGeometry.GetGeometryObjectMap();

   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      bool valid = false;
      bool symmetrical = false;
      if (geometryType == "GeometryFuselage")
      {
         GeometryFuselage* geometryFuselage = dynamic_cast<GeometryFuselage*>(geometryObject);
         valid = true;
         symmetrical = geometryFuselage->IsSymmetrical();
      }
      else if (geometryType == "GeometryBody")
      {
         GeometryBody* geometryBody = dynamic_cast<GeometryBody*>(geometryObject);
         if (geometryBody->IsMainBody())
         {
            valid = true;
         }

         symmetrical = geometryBody->IsSymmetrical();
      }
      if (valid)
      {
         // Calculate the mass of the object
         double massArea_ft2 = geometryObject->GetMassArea_ft2();
         double massFactor = geometryObject->GetMassFactor();

         if (symmetrical) { massArea_ft2 *= 2.0; }

         double relativeMass = CalcObjectRelativeMass(massArea_ft2, massFactor, mFuselageBodyMassMultiplier);
         double mass_lbs = relativeMass * aRealtiveMassMultiplier_lbs;
         // Set the calculated mass for the object
         geometryObject->CalcMassProperties(mass_lbs);
      }
   }
}

void GeometryMassProperties::CalcPodsMass(double aRealtiveMassMultiplier_lbs)
{
   const std::map<size_t, GeometryObject*>& geometryObjMap = mVehicleGeometry.GetGeometryObjectMap();

   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if (geometryType == "GeometryBody")
      {
         GeometryBody* geometryBody = dynamic_cast<GeometryBody*>(geometryObject);
         if (!geometryBody->IsMainBody())
         {
            // Calculate the mass of the object
            double massArea_ft2 = geometryObject->GetMassArea_ft2();
            double massFactor = geometryObject->GetMassFactor();

            if (geometryBody->IsSymmetrical()) { massArea_ft2 *= 2.0; }

            double relativeMass = CalcObjectRelativeMass(massArea_ft2, massFactor, mPodsMassMultiplier);
            double mass_lbs = relativeMass * aRealtiveMassMultiplier_lbs;
            // Set the calculated mass for the object
            geometryObject->CalcMassProperties(mass_lbs);
         }
      }
   }
}

void GeometryMassProperties::CalcNacellesMass(double aRealtiveMassMultiplier_lbs)
{
   const std::map<size_t, GeometryObject*>& geometryObjMap = mVehicleGeometry.GetGeometryObjectMap();

   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if (geometryType == "GeometryNacelle")
      {
         GeometryNacelle* geometryNacelle = dynamic_cast<GeometryNacelle*>(geometryObject);

         // Calculate the mass of the object
         double massArea_ft2 = geometryObject->GetMassArea_ft2();
         double massFactor = geometryObject->GetMassFactor();

         if (geometryNacelle->IsSymmetrical()) { massArea_ft2 *= 2.0; }

         double relativeMass = CalcObjectRelativeMass(massArea_ft2, massFactor, mNacellesMassMultiplier);
         double mass_lbs = relativeMass * aRealtiveMassMultiplier_lbs;
         // Set the calculated mass for the object
         geometryObject->CalcMassProperties(mass_lbs);
      }
   }
}

void GeometryMassProperties::CalcDishesMass(double aRealtiveMassMultiplier_lbs)
{
   const std::map<size_t, GeometryObject*>& geometryObjMap = mVehicleGeometry.GetGeometryObjectMap();

   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if (geometryType == "GeometryDish")
      {
         // Calculate the mass of the object
         double massArea_ft2 = geometryObject->GetMassArea_ft2();
         double massFactor = geometryObject->GetMassFactor();
         double relativeMass = CalcObjectRelativeMass(massArea_ft2, massFactor, mDishesMassMultiplier);
         double mass_lbs = relativeMass * aRealtiveMassMultiplier_lbs;
         // Set the calculated mass for the object
         geometryObject->CalcMassProperties(mass_lbs);
      }
   }
}

void GeometryMassProperties::CalcSurfacesMass(double aRealtiveMassMultiplier_lbs)
{
   const std::map<size_t, GeometryObject*>& geometryObjMap = mVehicleGeometry.GetGeometryObjectMap();

   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if (geometryType == "GeometrySurface")
      {
         GeometrySurface* geometrySurface = dynamic_cast<GeometrySurface*>(geometryObject);

         // Calculate the mass of the object
         double massArea_ft2 = geometryObject->GetMassArea_ft2();
         double massFactor = geometryObject->GetMassFactor();

         if (geometrySurface->IsSymmetrical())
         {
            std::string symmetryString = geometrySurface->GetSymmetryString().toStdString();

            // No impact for "Single"

            if ((symmetryString == "Horizontal") || (symmetryString == "Vertical"))
            {
               massArea_ft2 *= 2.0;
            }
            else if ((symmetryString == "+ Pattern") || (symmetryString == "X Pattern"))
            {
               massArea_ft2 *= 4.0;
            }
         }

         double relativeMass = CalcObjectRelativeMass(massArea_ft2, massFactor, mSurfacesMassMultiplier);
         double mass_lbs = relativeMass * aRealtiveMassMultiplier_lbs;
         // Set the calculated mass for the object
         geometryObject->CalcMassProperties(mass_lbs);
      }
   }
}

void GeometryMassProperties::CalcWingMass(double aRealtiveMassMultiplier_lbs)
{
   const std::map<size_t, GeometryObject*>& geometryObjMap = mVehicleGeometry.GetGeometryObjectMap();

   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if (geometryType == "GeometryWing")
      {
         // Calculate the mass of the object
         double massArea_ft2 = geometryObject->GetMassArea_ft2();
         double massFactor = geometryObject->GetMassFactor();
         double relativeMass = CalcObjectRelativeMass(massArea_ft2, massFactor, mWingMassMultiplier);
         double mass_lbs = relativeMass * aRealtiveMassMultiplier_lbs;
         // Set the calculated mass for the object
         geometryObject->CalcMassProperties(mass_lbs);
      }
   }
}

void GeometryMassProperties::CalcLandingGearMass(double aRealtiveMassMultiplier_lbs)
{
   const std::map<size_t, GeometryObject*>& geometryObjMap = mVehicleGeometry.GetGeometryObjectMap();

   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if (geometryType == "GeometryLandingGear")
      {
         GeometryLandingGear* geometryGear = dynamic_cast<GeometryLandingGear*>(geometryObject);

         // Calculate the mass of the object
         double massArea_ft2 = geometryObject->GetMassArea_ft2();
         double massFactor = geometryObject->GetMassFactor();

         if (geometryGear->IsSymmetrical()) { massArea_ft2 *= 2.0; }

         double relativeMass = CalcObjectRelativeMass(massArea_ft2, massFactor, mLandingGearMassMultiplier);
         double mass_lbs = relativeMass * aRealtiveMassMultiplier_lbs;
         // Set the calculated mass for the object
         geometryObject->CalcMassProperties(mass_lbs);
      }
   }
}

void GeometryMassProperties::CalcPointMassesMass(double aRealtiveMassMultiplier_lbs)
{
   const std::map<size_t, GeometryObject*>& geometryObjMap = mVehicleGeometry.GetGeometryObjectMap();

   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if (geometryType == "GeometryPointMass")
      {
         double mass_lbs = 0.0;  // Point mass ignores this argument
         geometryObject->CalcMassProperties(mass_lbs);
      }
   }
}

void GeometryMassProperties::CalcPropulsionMass(double aRealtiveMassMultiplier_lbs)
{
   const std::map<size_t, GeometryObject*>& geometryObjMap = mVehicleGeometry.GetGeometryObjectMap();

   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      std::string geometryType = geometryObject->GeometryObjectType();
      if (geometryType == "GeometryPropulsionData")
      {
         double mass_lbs = 0.0;  // Propulsion ignores this argument
         geometryObject->CalcMassProperties(mass_lbs);
      }
   }
}

void GeometryMassProperties::CalculateTotalMassProperties()
{
   const std::map<size_t, GeometryObject*>& geometryObjMap = mVehicleGeometry.GetGeometryObjectMap();

   double totalMass_lbs = 0.0;
   UtVec3dX totalRotInertia(0.0, 0.0, 0.0);
   UtVec3dX weightedCG(0.0, 0.0, 0.0);

   double fuelMass_lbs = 0.0;
   UtVec3dX totalFuelRotInertia(0.0, 0.0, 0.0);
   UtVec3dX weightedFuelCG(0.0, 0.0, 0.0);

   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;
      double mass_lbs = 0.0;
      UtVec3dX cgLocation_ft;
      UtVec3dX rotInertia(0.0, 0.0, 0.0);

      if (geometryObject->GetCalculatedMassProperties(cgLocation_ft, mass_lbs, rotInertia))
      {
         totalMass_lbs += mass_lbs;

         UtVec3dX cgMassProduct = cgLocation_ft * mass_lbs;
         weightedCG += cgMassProduct;
         totalRotInertia += rotInertia;
      }
   }

   if (mVehiclePropulsionPtr != nullptr)
   {
      GeometryPropulsionData* propulsionData = mVehicleGeometry.GetPropulsionData();
      if (propulsionData != nullptr)
      {
         if (propulsionData->FuelTankPresent())
         {
            double tempFuelMass_lbs = propulsionData->GetFuelQtyMax();
            double fuelMass_slugs = tempFuelMass_lbs * mLbsToSlugs;

            UtVec3dX fuelRefPt = propulsionData->GetRefPoint_ft();

            // Correct for location, using parallel axis theorem
            double distX = sqrt(fuelRefPt.Y() * fuelRefPt.Y() + fuelRefPt.Z() * fuelRefPt.Z());
            double distY = sqrt(fuelRefPt.X() * fuelRefPt.X() + fuelRefPt.Z() * fuelRefPt.Z());
            double distZ = sqrt(fuelRefPt.X() * fuelRefPt.X() + fuelRefPt.Y() * fuelRefPt.Y());
            double dX2 = distX * distX;
            double dY2 = distY * distY;
            double dZ2 = distZ * distZ;

            UtVec3dX tempRotInertia(fuelMass_slugs * dX2, fuelMass_slugs * dY2, fuelMass_slugs * dZ2);
            UtVec3dX cgMassProduct = fuelRefPt * tempFuelMass_lbs;

            fuelMass_lbs += tempFuelMass_lbs;
            totalFuelRotInertia += tempRotInertia;
            weightedFuelCG += cgMassProduct;
         }
      }
   }

   // Check for solid propellant
   for (auto& objIter : geometryObjMap)
   {
      GeometryObject* geometryObject = objIter.second;

      if (geometryObject->GeometryObjectType() == "GeometryEngine")
      {
         GeometryEngine* engine = dynamic_cast<GeometryEngine*>(geometryObject);
         {
            double mass_lbs = 0.0;
            UtVec3dX cgLocation_ft;
            UtVec3dX rotInertia(0.0, 0.0, 0.0);

            if (engine->GetCalculatedPropellantMassProperties(cgLocation_ft, mass_lbs, rotInertia))
            {
               fuelMass_lbs += mass_lbs;
               totalFuelRotInertia += rotInertia;

               UtVec3dX cgMassProduct = cgLocation_ft * mass_lbs;
               weightedFuelCG += cgMassProduct;
            }
         }
      }
   }

   UtVec3dX emptyCG = weightedCG / totalMass_lbs;

   if (mAutoCalcInertia)
   {
      mIxx = totalRotInertia.X();
      mIyy = totalRotInertia.Y();
      mIzz = totalRotInertia.Z();
   }
   mCG_Location_ft = emptyCG;

   // Full fuel conditions

   UtVec3dX fueledCG = (weightedCG + weightedFuelCG) / (totalMass_lbs + fuelMass_lbs);

   mFueledIxx = totalRotInertia.X() + totalFuelRotInertia.X();
   mFueledIyy = totalRotInertia.Y() + totalFuelRotInertia.Y();
   mFueledIzz = totalRotInertia.Z() + totalFuelRotInertia.Z();
   mFueledCG_Location_ft = fueledCG;
}

QStringList GeometryMassProperties::Audit()
{
   QStringList errorList = GeometryObject::Audit();

   QString nameString(mName.c_str());

   if (mEmptyMass_lbs < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires an empty mass (mEmptyMass_lbs is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   if (mTotalMass_lbs < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a total mass (mTotalMass_lbs is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   if (mIxx < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a rotational inertia about the x-axis (mIxx is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   if (mIyy < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a rotational inertia about the y-axis (mIyy is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   if (mIzz < std::numeric_limits<double>::epsilon())
   {
      QString errorString(" requires a rotational inertia about the z-axis (mIzz is zero)");
      errorString = nameString + errorString;
      errorList.push_back(errorString);
   }

   return errorList;
}

double GeometryMassProperties::GetFuselageBodyMass_lbs()
{
   return mFuselageBodyMass_lbs;
}

double GeometryMassProperties::GetPodsMass_lbs()
{
   return mPodsMass_lbs;
}

double GeometryMassProperties::GetWingMass_lbs()
{
   return mWingMass_lbs;
}

double GeometryMassProperties::GetSurfacesMass_lbs()
{
   return mSurfacesMass_lbs;
}

double GeometryMassProperties::GetLandingGearMasses_lbs()
{
   return mLandingGearMass_lbs;
}

double GeometryMassProperties::GetNacelleMasses_lbs()
{
   return mNacellesMass_lbs;
}

double GeometryMassProperties::GetDishMasses_lbs()
{
   return mDishesMass_lbs;
}

double GeometryMassProperties::GetPointMasses_lbs()
{
   return mPointMassesMass_lbs;
}

double GeometryMassProperties::GetEngineMasses_lbs()
{
   return mEnginesMass_lbs;
}

} // namespace Designer
