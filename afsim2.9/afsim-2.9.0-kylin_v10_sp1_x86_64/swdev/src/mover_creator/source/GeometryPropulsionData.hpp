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

#ifndef GEOMETRY_PROPULSION_DATA_HPP
#define GEOMETRY_PROPULSION_DATA_HPP

#include <string>

#include "GeometryObject.hpp"

// ****************************************************************************
// GeometryPropulsionData is derived from GeometryObject and provides data
// for the propulsion system.
// ****************************************************************************

namespace Designer
{
   class GeometryPropulsionData : public GeometryObject
   {
      public:
         explicit GeometryPropulsionData(Vehicle* aVehicle);
         virtual ~GeometryPropulsionData();

         // GeometryPropulsionData uses mFuelTankCG_Location_ft in place of mRefPoint_ft
         UtVec3dX GetRefPoint_ft() override;

         // This allows the reference point to be set. GeometryPropulsionData uses
         // mFuelTankCG_Location_ft in place of mRefPoint_ft
         void SetRefPoint(UtVec3dX aRefPoint_ft) override;

         // This moves the reference point by the specified amount. GeometryPropulsionData
         // uses mFuelTankCG_Location_ft in place of mRefPoint_ft
         void MoveRefPoint(UtVec3dX aMoveDelta_ft) override;

         // This is called to load the widget data (GUI functionality)
         QTreeWidgetItem* CreateTreeWidgetItem(QString&     aName,
                                               QTreeWidget* aTreeWidgetPtr,
                                               bool         aNoneditableName) override;

         // This calculates the max/min extent of the object in all three directions (x,y,z)
         // Return true if extents are valid (some object do not support "extent").
         bool CalcExtents(double& xMin,
                          double& xMax,
                          double& yMin,
                          double& yMax,
                          double& zMin,
                          double& zMax) override;

         // This is called to calculate a full set of mass properties, given the specified total mass.
         // The calculations include any symmetry effects and represent the combined mass properties for
         // the entire set of subobjects, if appropriate. This function sets mCalculatedMass_lbs and
         // then calculates mCalculatedCG_ft, mCalculatedIxx, mCalculatedIyy, and mCalculatedIzz.
         void CalcMassProperties(double aTotalMass_lbs) override;

         // This provides the location of the cg. Returns true if valid.
         bool GetCG_ft(UtVec3dX& aCg_ft) override;

         // This provides a full set of mass properties. It requires that CalcMassProperties() was called
         // prior to calling this function. The results are corrected for any symmetry effects and
         // represent the combined mass properties for the entire set of subobjects, if appropriate.
         // Returns true if the data is valid. Data should not be used if invalid.
         bool GetCalculatedMassProperties(UtVec3dX& aCgLocation_ft,
                                          double&   aMass_lbs,
                                          UtVec3dX& aRotationalInertia) override;

         // This returns the string describing the thrust-vectoring option. Returns true if valid.
         bool GetEnableThrustVectoringString(QString& aString);

         // This returns the minimum deflection, in degrees, typically negative, of the thrust vector
         // relative to the horizontal. Returns true if valid.
         bool GetMinimumThrustVectoringPitchAngle_deg(double& aMinimumThrustVectorPitchAngle_deg);

         // This returns the maximum deflection, in degrees, typically positive, of the thrust vector
         // relative to the horizontal. Returns true if valid.
         bool GetMaximumThrustVectoringPitchAngle_deg(double& aMaximumThrustVectorPitchAngle_deg);

         // This returns the minimum deflection, in degrees, typically negative, of the thrust vector
         // relative to the vertical. Returns true if valid.
         bool GetMinimumThrustVectoringYawAngle_deg(double& aMinimumThrustVectorYawAngle_deg);

         // This returns the maximum deflection, in degrees, typically positive, of the thrust vector
         // relative to the vertical. Returns true if valid.
         bool GetMaximumThrustVectoringYawAngle_deg(double& aMaximumThrustVectorYawAngle_deg);

         // This performs an "audit" to check that a geometry object is fully
         // defined, identifying any issues that may exist. The function returns a
         // list of QStrings that detail any problems. If no problems are found,
         // the list will be empty.
         QStringList Audit() override;

         bool FuelTankPresent() { return mEnableFuelTank; }
         double GetFuelQtyMax() { return mFuelQtyMax; }
         double GetFuelQtyCurrent() { return mFuelQtyCurrent; }

      private slots:
         void HandleX_CoordinateLineEditChanged(QString aValue);
         void HandleY_CoordinateLineEditChanged(QString aValue);
         void HandleZ_CoordinateLineEditChanged(QString aValue);
         void HandleMayNotBeDeletedCheckBoxChanged(int aValue);
         void HandleTreeItemClicked(QTreeWidgetItem* item, int column);
         void HandleEnableFuelTankCheckBoxChanged(int aValue);
         void HandleFuelQtyMaxLineEditChanged(QString aValue);
         void HandleFuelQtyCurrentLineEditChanged(QString aValue);
         void HandleEnableThrustVectoringComboBoxChanged(QString aValue);
         void HandleTVC_PitchAngleMinLineEditChanged(QString aValue);
         void HandleTVC_PitchAngleMaxLineEditChanged(QString aValue);
         void HandleTVC_YawAngleMinLineEditChanged(QString aValue);
         void HandleTVC_YawAngleMaxLineEditChanged(QString aValue);

      protected:
         UtVec3dX         mFuelTankCG_Location_ft; // Reference position of tank CG
         bool             mEnableFuelTank        = false;
         double           mFuelQtyMax            = 0.0;
         double           mFuelQtyCurrent        = 0.0;
         QString          mEnableThrustVectoringString;
         double           mTVC_PitchAngleMin     = 0.0;
         double           mTVC_PitchAngleMax     = 0.0;
         double           mTVC_YawAngleMin       = 0.0;
         double           mTVC_YawAngleMax       = 0.0;

         QTreeWidgetItem* mFuelQtyMaxWidget      = nullptr;
         QTreeWidgetItem* mFuelQtyCurrentWidget  = nullptr;
         QTreeWidgetItem* mFuelTankCG_Widget     = nullptr;
         QTreeWidgetItem* mTVC_PitchMinWidget    = nullptr;
         QTreeWidgetItem* mTVC_PitchMaxWidget    = nullptr;
         QTreeWidgetItem* mTVC_YawMinWidget      = nullptr;
         QTreeWidgetItem* mTVC_YawMaxWidget      = nullptr;

         QLineEdit*       mFuelCG_XLineEdit      = nullptr;
         QLineEdit*       mFuelCG_YLineEdit      = nullptr;
         QLineEdit*       mFuelCG_ZLineEdit      = nullptr;
   };
}
#endif // ! GEOMETRY_PROPULSION_DATA_HPP
