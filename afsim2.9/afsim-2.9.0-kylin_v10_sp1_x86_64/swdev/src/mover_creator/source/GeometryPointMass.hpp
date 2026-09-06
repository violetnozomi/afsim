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

#ifndef GEOMETRY_POINT_MASS_HPP
#define GEOMETRY_POINT_MASS_HPP

#include "GeometryObject.hpp"

#include <string>

// ****************************************************************************
// GeometryPointMass is derived from GeometryObject and provides data
// for a single point mass.
// ****************************************************************************
namespace Designer
{
   class VehicleGeometry;
}

namespace Designer
{
   class GeometryPointMass : public GeometryObject
   {
      public:
         explicit GeometryPointMass(Vehicle* aVehicle);
         virtual ~GeometryPointMass();

         // This is called to load the widget data (GUI functionality)
         QTreeWidgetItem* CreateTreeWidgetItem(QString& aName, QTreeWidget* aTreeWidgetPtr, bool aNoneditableName) override;

         // This calculates the max/min extent of the object in all three directions (x,y,z)
         // Return true if extents are valid (some object do not support "extent").
         bool CalcExtents(double& xMin, double& xMax, double& yMin, double& yMax, double& zMin, double& zMax) override;

         double GetPointMass_lbs() { return mPointMass_lbs; }

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
         void HandleMassLineEditChanged(QString aValue);
         void HandleX_CoordinateLineEditChanged(QString aValue);
         void HandleY_CoordinateLineEditChanged(QString aValue);
         void HandleZ_CoordinateLineEditChanged(QString aValue);

      protected:
         double mPointMass_lbs = 0.0;
   };
}
#endif // ! GEOMETRY_POINT_MASS_HPP

