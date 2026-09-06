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

#ifndef GEOMETRY_OBJECT_HPP
#define GEOMETRY_OBJECT_HPP

#include <string>

#include <QLineEdit>
#include <QObject>
#include <QPushButton>

#include "GeometryWidget.hpp"
#include "UtDCM.hpp"
#include "UtVec3dX.hpp"

// ****************************************************************************
// GeometryObject is the base class of all objects that have geometric
// characteristics such as bodies, fuselages, surfaces, wings, and drag
// producers such as speed brakes, and landing gear.
// ****************************************************************************

class QTreeWidget;
class QTreeWidgetItem;

namespace Ui
{
   class GeometryWidget;
}

namespace Designer
{
   constexpr double mLbsToSlugs = 0.03108095;
   constexpr double mSlugsToLbs = 32.1740486;

   class Vehicle;

   class GeometryObject : public QObject
   {
      public:

         explicit GeometryObject(Vehicle* aVehicle);
         ~GeometryObject();

         std::string GeometryObjectType() { return mGeometryObjectTypeString; }

         virtual std::string GetName();
         virtual void SetName(QString& aName);

         // This is overridable, but not typically
         virtual UtVec3dX GetRefPoint_ft();

         // This allows the reference point to be set
         virtual void SetRefPoint(UtVec3dX aRefPoint_ft);

         // This moves the reference point by the specified amount
         virtual void MoveRefPoint(UtVec3dX aMoveDelta_ft);

         // This is called to load the widget data (GUI functionality)
         virtual QTreeWidgetItem* CreateTreeWidgetItem(QString&     aName,
                                                       QTreeWidget* aTreeWidgetPtr,
                                                       bool         aNoneditableName);

         // Return true if the object may not be deleted via the GUI
         bool MayNotBeDeleted() { return mMayNotBeDeleted; }

         // Call this to set the delete push button
         void SetDeleteButton(QPushButton* aDeleteButton) { mDeleteButton = aDeleteButton; }

         // Returns the associated tree widget item (or nullptr if none exists)
         QTreeWidgetItem* GetTreeWidgetItem() { return mTreeWidgetItemPtr; }

         // Returns the reference point tree widget item (or nullptr if none exists)
         QTreeWidgetItem* GetReferencePointWidget() { return mReferencePointWidget; }

         // Returns the reference point "y" tree widget item (or nullptr if none exists)
         QTreeWidgetItem* GetReferencePointY_TreeWidgetItem() { return mReferencePointY_TreeWidgetItemPtr; }

         // Returns the reference point "x" line edit widget item (or nullptr if none exists)
         QLineEdit*  GetReferencePointX_LineEditPtr() { return mReferencePointX_LineEditPtr; }

         // Returns the reference point "y" line edit widget item (or nullptr if none exists)
         QLineEdit*  GetReferencePointY_LineEditPtr() { return mReferencePointY_LineEditPtr; }

         // Returns the reference point "z" line edit widget item (or nullptr if none exists)
         QLineEdit*  GetReferencePointZ_LineEditPtr() { return mReferencePointZ_LineEditPtr; }

         // This calculates the max/min extent of the object in all three directions (x,y,z)
         // Return true if extents are valid (some object do not support "extent").
         virtual bool CalcExtents(double& xMin, double& xMax, double& yMin, double& yMax, double& zMin, double& zMax);

         // Mass-related functions

         // This returns the mass factor which is a multiplier for the mass of the object
         // as it compares with typical mass for the same type of object. The default is 1.
         double GetMassFactor() { return mMassFactor; }

         // This returns the mass area which is a representation of the appropriate reference
         // area for the object. It is used by GeometryMassProperties to calculate the mass of the object.
         virtual double GetMassArea_ft2() { return 0.0; }

         // This is called to calculate a full set of mass properties, given the specified total mass.
         // The calculations include any symmetry effects and represent the combined mass properties for
         // the entire set of subobjects, if appropriate. This function sets mCalculatedMass_lbs and
         // then calculates mCalculatedCG_ft, mCalculatedIxx, mCalculatedIyy, and mCalculatedIzz.
         virtual void CalcMassProperties(double aTotalMass_lbs);

         // This provides a full set of mass properties. It requires that CalcMassProperties() was called
         // prior to calling this function. The results are corrected for any symmetry effects and
         // represent the combined mass properties for the entire set of subobjects, if appropriate.
         // Returns true if the data is valid. Data should not be used if invalid.
         virtual bool GetCalculatedMassProperties(UtVec3dX& aCgLocation_ft,
                                                  double&   aMass_lbs,
                                                  UtVec3dX& aRotationalInertia);

         // This provides a full set of mass properties. It requires that CalcMassProperties() was called
         // prior to calling this function. The results are corrected for any symmetry effects and
         // represent the combined mass properties for the entire set of subobjects, if appropriate.
         // Returns true if the data is valid. Data should not be used if invalid.
         double GetCalculatedMass_lbs() const;

         // This provides the location of the cg. Returns true if valid. This should be overridden to provide
         // a valid cg, since the default is a non-valid cg (for objects lack lack a cg).
         virtual bool GetCG_ft(UtVec3dX& aCg_ft);

         virtual double GetRotationalInertiaIxx() { return mCalculatedIxx; }
         virtual double GetRotationalInertiaIyy() { return mCalculatedIyy; }
         virtual double GetRotationalInertiaIzz() { return mCalculatedIzz; }

         // This should be overridden by derived classes, if appropriate
         virtual bool IsMovable() const { return false; }

         // Returns true if debug mode is active
         bool DebugMode();

         // This performs an "audit" to check that a geometry object is fully
         // defined, identifying any issues that may exist. The function returns a
         // list of QStrings that detail any problems. If no problems are found,
         // the list will be empty.
         virtual QStringList Audit();
         //virtual void AdjustAndRedraw(bool aRecalcMassAndInertia = false);

      protected slots:
         void HandleX_CoordinateLineEditChanged(QString aValue);
         void HandleY_CoordinateLineEditChanged(QString aValue);
         void HandleZ_CoordinateLineEditChanged(QString aValue);
         void HandleMayNotBeDeletedCheckBoxChanged(int aValue);
         void HandleTreeItemClicked(QTreeWidgetItem* item, int column);
         void HandleMassFactorLineEditChanged(QString aValue);

      protected:

         Ui::GeometryWidget* GetUiGeometryWidget();
         Designer::GeometryWidget* GetGeometryWidget();

         // This should be called to connect the x,y,z line edits. This may
         // be overridden to allow the use of different "handler" functions
         virtual void ConnectLineEdits();

         // This should be called to adjust the viewing distance after
         // an object is modified
         virtual void AdjustViewingDistance();

         // This should be called to update the GUI
         virtual void Update();

         // This should be called to adjust the viewing distance and
         // repaint the GUI after an object is modified
         virtual void AdjustAndRedraw(bool aRecalcMassAndInertia = false);

         // This should be called to force recalculating mass properties and aero data
         void RecalcMassAndAero();

         // Returns true if the vehicle is an aircraft
         bool IsAircraft();

         // Returns true if the vehicle is an weapon
         bool IsWeapon();

         void HideDeleteButton();

         std::string        mName;
         std::string        mGeometryObjectTypeString;         // Defines the type of geometry object (used with GUI elements)
         UtVec3dX           mRefPoint_ft;                      // Reference position of object
         bool               mMayNotBeDeleted                   = false;

         double             mCalculatedMass_lbs                = 0.0;
         UtVec3dX           mCalculatedCG_ft;
         double             mCalculatedIxx                     = 0.0;
         double             mCalculatedIyy                     = 0.0;
         double             mCalculatedIzz                     = 0.0;

         // This can be set to false by derived classes to allow the derived class to use its own connections
         // for the reference point line edits
         bool               mUseDefaultConnectLineEdits        = true;

         // The mass factor is a multiplier for the mass of the object as it compares with typical mass
         // for the same type of object. The default is 1. Derived classes and/or instances of classes
         // should adjust the value as necessary.
         double             mMassFactor                        = 1.0;

         QPushButton*       mDeleteButton                      = nullptr;
         QTreeWidgetItem*   mTreeWidgetItemPtr                 = nullptr; // This is the tree widget associated with this object
         QTreeWidgetItem*   mReferencePointWidget              = nullptr;
         QTreeWidgetItem*   mReferencePointY_TreeWidgetItemPtr = nullptr; // This is the reference point "y" widget
         QLineEdit*         mReferencePointX_LineEditPtr       = nullptr;
         QLineEdit*         mReferencePointY_LineEditPtr       = nullptr;
         QLineEdit*         mReferencePointZ_LineEditPtr       = nullptr;
         QLineEdit*         mMassFactorLineEditPtr             = nullptr;

         Designer::Vehicle* mVehiclePtr                        = nullptr;
   };
}
#endif // ! GEOMETRY_OBJECT_HPP
