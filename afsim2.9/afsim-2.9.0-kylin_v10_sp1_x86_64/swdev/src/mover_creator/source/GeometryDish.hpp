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

#ifndef GEOMETRY_DISH_HPP
#define GEOMETRY_DISH_HPP

#include <string>
#include <QTreeWidgetItem>

#include "GeometryObject.hpp"
#include "UtMath.hpp"

// ****************************************************************************
// GeometryDish is derived from GeometryObject and provides data for
// any body of revolution such as an AWACS/AEW radar dish
// ****************************************************************************

namespace Designer
{
   class GeometryDish : public GeometryObject
   {
      public:
         explicit GeometryDish(Vehicle* aVehicle);
         virtual ~GeometryDish();

         double GetDiameter_ft() { return mDiameter_ft; }
         double GetThickness_ft() { return mThickness_ft; }

         // This is called to load the widget data (GUI functionality)
         QTreeWidgetItem* CreateTreeWidgetItem(QString& aName, QTreeWidget* aTreeWidgetPtr, bool aNoneditableName) override;

         // This calculates the max/min extent of the object in all three directions (x,y,z)
         // Return true if extents are valid (some object do not support "extent").
         bool CalcExtents(double& xMin, double& xMax, double& yMin, double& yMax, double& zMin, double& zMax) override;

         // This returns the mass area which is a representation of the appropriate reference
         // area for the object. It is used by GeometryMassProperties to calculate the mass of the object.
         double GetMassArea_ft2() override;

         // This is called to calculate a full set of mass properties, given the specified total mass.
         // The calculations include any symmetry effects and represent the combined mass properties for
         // the entire set of subobjects, if appropriate. This function sets mCalculatedMass_lbs and
         // then calculates mCalculatedCG_ft, mCalculatedIxx, mCalculatedIyy, and mCalculatedIzz.
         void CalcMassProperties(double aTotalMass_lbs) override;

         // This provides the location of the cg. Returns true if valid.
         bool GetCG_ft(UtVec3dX& aCg_ft) override;

         // This performs an "audit" to check that a geometry object is fully
         // defined, identifying any issues that may exist. The function returns a
         // list of QStrings that detail any problems. If no problems are found,
         // the list will be empty.
         QStringList Audit() override;

      private slots:
         void HandleThicknessLineEditChanged(QString aValue);
         void HandleDiameterLineEditChanged(QString aValue);

      protected:

         double  mDiameter_ft = 0.0;
         double  mThickness_ft = 0.0;
   };
}
#endif // ! GEOMETRY_DISH_HPP
