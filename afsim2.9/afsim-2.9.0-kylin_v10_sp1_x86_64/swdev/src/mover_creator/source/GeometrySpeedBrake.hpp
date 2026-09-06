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

#ifndef GEOMETRY_SPEED_BRAKE_HPP
#define GEOMETRY_SPEED_BRAKE_HPP

#include "GeometryObject.hpp"
#include "UtMath.hpp"

#include <string>

// ****************************************************************************
// GeometrySpeedBrake is derived from GeometryObject and provides aerodynamics
// for speed brakes.
// ****************************************************************************

namespace Designer
{
   class GeometrySpeedBrake : public GeometryObject
   {
      public:
         explicit GeometrySpeedBrake(Vehicle* aVehicle);
         virtual ~GeometrySpeedBrake();

         // This moves the reference point by the specified amount
         void MoveRefPoint(UtVec3dX aMoveDelta_ft) override;

         double GetMaxAngle_deg() { return mMaxAngle_deg; }
         double GetCurrentAngle_deg() { return mCurrentAngle_deg; }
         double GetCdMultiplier() { return mCdMultiplier; }

         void SetCurrentAngle_deg(double aAngle_deg) { mCurrentAngle_deg = aAngle_deg; }

         double GetLength_ft() { return mLength_ft; }
         double GetWidth_ft() { return mWidth_ft; }
         double GetRollAngle_deg() { return mRoll_rad * UtMath::cDEG_PER_RAD; }

         bool IsHorizontallySymmetrical() { return mHorizontallySymmetrical; }
         bool IsVerticallySymmetrical() { return mVerticallySymmetrical; }

         // This is the y-coordinate of horizontal symmetry
         double GetSymmetryHorizontalY() { return mSymmetryHorizontalY; }

         // This is the z-coordinate of vertical symmetry
         double GetSymmetryVerticalZ() { return mSymmetryVerticalZ; }

         // This is called to load the widget data (GUI functionality)
         QTreeWidgetItem* CreateTreeWidgetItem(QString& aName, QTreeWidget* aTreeWidgetPtr, bool aNoneditableName) override;

         // This is called to calculate a full set of mass properties, given the specified total mass.
         // The calculations include any symmetry effects and represent the combined mass properties for
         // the entire set of subobjects, if appropriate. This function sets mCalculatedMass_lbs and
         // then calculates mCalculatedCG_ft, mCalculatedIxx, mCalculatedIyy, and mCalculatedIzz.
         void CalcMassProperties(double aTotalMass_lbs) override;

         // This provides a full set of mass properties. It requires that CalcMassProperties() was called
         // prior to calling this function. The results are corrected for any symmetry effects and
         // represent the combined mass properties for the entire set of subobjects, if appropriate.
         // Returns true if the data is valid. Data should not be used if invalid.
         bool GetCalculatedMassProperties(UtVec3dX& aCgLocation_ft,
                                          double&   aMass_lbs,
                                          UtVec3dX& aRotationalInertia) override;

         // Speed brakes are always movable
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
         void HandleLengthLineEditChanged(QString aValue);
         void HandleWidthLineEditChanged(QString aValue);
         void HandleRollLineEditChanged(QString aValue);
         void HandleSymmetricalComboBoxChanged(QString aValue);
         void HandleMaxRetractionRateLineEditChanged(QString aValue);
         void HandleMaxExtensionRateLineEditChanged(QString aValue);

      protected:
         double            mMaxAngle_deg = 90.0;
         double            mCurrentAngle_deg = 0.0;
         double            mCdMultiplier = 1.0;
         double            mLength_ft = 0.0;
         double            mWidth_ft = 0.0;
         double            mRoll_rad = 0.0;
         bool              mHorizontallySymmetrical = false;
         bool              mVerticallySymmetrical = false;
         QString           mSymmetryString = "Single";
         double            mMaxRetractionRate_dps = -300.00;
         double            mMaxExtensionRate_dps = 300.00;

         QTreeWidgetItem*  mMaxAngleWidget = nullptr;

         double            mSymmetryHorizontalY = 0.0;
         double            mSymmetryVerticalZ = 0.0;
   };
}
#endif // ! GEOMETRY_SPEED_BRAKE_HPP
