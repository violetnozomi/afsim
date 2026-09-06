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

#ifndef GEOMETRY_LANDING_GEAR_HPP
#define GEOMETRY_LANDING_GEAR_HPP

#include "GeometryObject.hpp"
#include "UtMath.hpp"

#include <string>

// ****************************************************************************
// GeometryLandingGear is derived from GeometryObject and provides data
// for landing gear.
// ****************************************************************************

namespace Designer
{
   class GeometryLandingGear : public GeometryObject
   {
      public:
         explicit GeometryLandingGear(Vehicle* aVehicle);
         virtual ~GeometryLandingGear();

         // This moves the reference point by the specified amount
         void MoveRefPoint(UtVec3dX aMoveDelta_ft) override;

         double GetMaxAngle_deg() { return mMaxAngle_deg; }
         double GetCurrentAngle_deg() { return mCurrentAngle_deg; }
         double GetCdMultiplier() { return mCdMultiplier; }

         void SetCurrentAngle_deg(double aAngle_deg) { mCurrentAngle_deg = aAngle_deg; }

         double GetUncompressedLength_ft() { return mUncompressedLength_ft; }
         double GetStrutDiam_ft() { return mStrutDiam_ft; }
         double GetTireDiam_ft() { return mTireDiam_ft; }
         double GetTireWidth_ft() { return mTireWidth_ft; }

         bool IsSymmetrical() { return mSymmetrical; }

         // This is the y-coordinate of horizontal symmetry
         double  GetSymmetryHorizontalY() { return mSymmetryHorizontalY; }

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

         // Landing gear are always movable
         bool IsMovable() const override { return true; }

         // This performs an "audit" to check that a geometry object is fully
         // defined, identifying any issues that may exist. The function returns a
         // list of QStrings that detail any problems. If no problems are found,
         // the list will be empty.
         QStringList Audit() override;

         double GetMaxRetractionRate_dps() { return mMaxRetractionRate_dps; }
         double GetMaxExtensionRate_dps() { return mMaxExtensionRate_dps; }

      private slots:
         void HandleDragCoefficientMultiplierLineEditChanged(QString aValue);
         void HandleMaxAngleLineEditChanged(QString aValue);
         void HandleUncompressedLengthLineEditChanged(QString aValue);
         void HandleMaxCompressionLineEditChanged(QString aValue);
         void HandleStrutDiamLineEditChanged(QString aValue);
         void HandleTireDiamLineEditChanged(QString aValue);
         void HandleTireWidthLineEditChanged(QString aValue);
         void HandleSymmetricalCheckBoxChanged(int aValue);
         void HandleSpringConstantLineEditChanged(QString aValue);
         void HandleDamperConstantLineEditChanged(QString aValue);
         void HandleRollingFrictionCoefficientLineEditChanged(QString aValue);
         void HandleScuffingFrictionCoefficientLineEditChanged(QString aValue);
         void HandleBrakingFrictionCoefficientLineEditChanged(QString aValue);
         void HandleMaxRetractionRateLineEditChanged(QString aValue);
         void HandleMaxExtensionRateLineEditChanged(QString aValue);

      protected:
         double            mMaxAngle_deg = 90.0;
         double            mCurrentAngle_deg = 0.0;
         double            mCdMultiplier = 1.0;
         double            mUncompressedLength_ft = 0.0;
         double            mMaxCompression_ft = 0.0;
         double            mStrutDiam_ft = 0.0;
         double            mTireDiam_ft = 0.0;
         double            mTireWidth_ft = 0.0;
         double            mSpringConstant = 20000.0;
         double            mDamperConstant = 15000.0;
         double            mRollingFrictionCoefficient = 0.001;
         double            mScuffingFrictionCoefficient = 1.2;
         double            mBrakingFrictionCoefficient = 0.8;
         bool              mSymmetrical = false;
         double            mMaxRetractionRate_dps = -300.00;
         double            mMaxExtensionRate_dps = 300.00;

         QTreeWidgetItem*  mMaxAngleWidget = nullptr;

         double            mSymmetryHorizontalY = 0.0;
   };
}
#endif // ! GEOMETRY_LANDING_GEAR_HPP
