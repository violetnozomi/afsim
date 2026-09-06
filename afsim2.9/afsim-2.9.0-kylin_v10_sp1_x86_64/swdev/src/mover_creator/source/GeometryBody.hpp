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

#ifndef GEOMETRY_BODY_HPP
#define GEOMETRY_BODY_HPP

#include <QTreeWidgetItem>
#include <string>

#include "GeometryObject.hpp"
#include "UtMath.hpp"

// ****************************************************************************
// GeometryBody is derived from GeometryObject and provides data for
// any body of revolution such as a fuselage or missile body.
// ****************************************************************************

namespace Designer
{
   class GeometryBody : public GeometryObject
   {
      public:
         explicit GeometryBody(Vehicle* aVehicle);
         virtual ~GeometryBody();

         enum class ForwardShapeType
         {
            cCONE,
            cOGIVE,
            cROUND,
            cBLUNT
         };

         enum class AftShapeType
         {
            cCONE,
            cOGIVE,
            cROUND,
            cBOATTAIL,
            cBLUNT
         };

         // This moves the reference point by the specified amount
         void MoveRefPoint(UtVec3dX aMoveDelta_ft) override;

         double GetLength_ft() { return mLength_ft; }
         double GetHeight_ft() { return mHeight_ft; }
         double GetWidth_ft() { return mWidth_ft; }
         double GetYawAngle_deg() { return mYaw_rad * UtMath::cDEG_PER_RAD; }
         double GetPitchAngle_deg() { return mPitch_rad * UtMath::cDEG_PER_RAD; }
         double GetRollAngle_deg() { return mRoll_rad * UtMath::cDEG_PER_RAD; }

         // Forward shapes include Round, Ogive, Cone, and Blunt
         ForwardShapeType GetForwardShapeType();
         double GetForwardShapeLength_ft() { return mForwardShapeLength_ft; }

         // Aft shapes include Round, Ogive, Cone, BoatTail, and Blunt
         AftShapeType GetAftShapeType();
         double GetAftShapeLength_ft() { return mAftShapeLength_ft; }
         double GetAftShapeDiam_ft() { return mAftShapeDiam_ft; }

         void SetYawAngle(double aYaw_deg) { mYaw_rad = aYaw_deg * UtMath::cRAD_PER_DEG; }
         void SetPitchAngle(double aPitch_deg) { mPitch_rad = aPitch_deg * UtMath::cRAD_PER_DEG; }
         void SetRollAngle(double aRoll_deg) { mRoll_rad = aRoll_deg * UtMath::cRAD_PER_DEG; }

         bool IsSymmetrical() { return mSymmetrical; }

         // This is the y-coordinate of horizontal symmetry
         double  GetSymmetryHorizontalY() { return mSymmetryHorizontalY; }

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

         // This returns the mass area which is a representation of the appropriate reference
         // area for the object. It is used by GeometryMassProperties to calculate the mass of the object.
         double GetMassArea_ft2() override;

         // This returns true if the object is the main body of the vehicle
         bool IsMainBody() { return mIsMainBody; }

         // This is called to calculate a full set of mass properties, given the specified total mass.
         // The calculations include any symmetry effects and represent the combined mass properties for
         // the entire set of subobjects, if appropriate. This function sets mCalculatedMass_lbs and
         // then calculates mCalculatedCG_ft, mCalculatedIxx, mCalculatedIyy, and mCalculatedIzz.
         void CalcMassProperties(double aTotalMass_lbs) override;

         // This provides the location of the cg. Returns true if valid.
         bool GetCG_ft(UtVec3dX& aCg_ft) override;

         // This performs an "audit" to check that a geometry object is fully defined,
         // identifying any issues that may exist. The function returns a list of QStrings
         // that detail any problems. If no problems are found, the list will be empty.
         QStringList Audit() override;

      protected slots:
         void HandleLengthLineEditChanged(QString aValue);
         void HandleHeightLineEditChanged(QString aValue);
         void HandleWidthLineEditChanged(QString aValue);
         void HandleYawLineEditChanged(QString aValue);
         void HandlePitchLineEditChanged(QString aValue);
         void HandleRollLineEditChanged(QString aValue);
         void HandleSymmetricalCheckBoxChanged(int aValue);
         void HandleIsMainBodyCheckBoxChanged(int aValue);
         void HandleForwardShapeComboBoxChanged(QString aValue);
         void HandleForwardShapeLengthLineEditChanged(QString aValue);
         void HandleAftShapeComboBoxChanged(QString aValue);
         void HandleAftShapeLengthLineEditChanged(QString aValue);
         void HandleAftShapeDiameterLineEditChanged(QString aValue);

      protected:
         double            mLength_ft              = 0.0;
         double            mHeight_ft              = 0.0;
         double            mWidth_ft               = 0.0;
         bool              mSymmetrical            = false;
         bool              mIsMainBody             = false;
         double            mYaw_rad                = 0.0;
         double            mPitch_rad              = 0.0;
         double            mRoll_rad               = 0.0;
         QString           mForwardShapeString     = "Ogive";
         double            mForwardShapeLength_ft  = 0.0;
         QString           mAftShapeString         = "Ogive";
         double            mAftShapeLength_ft      = 0.0;
         double            mAftShapeDiam_ft        = 0.0;

         QTreeWidgetItem*  mForwardShapeItem       = nullptr;
         QTreeWidgetItem*  mForwardShapeLengthItem = nullptr;
         QTreeWidgetItem*  mAftShapeItem           = nullptr;
         QTreeWidgetItem*  mAftShapeLengthItem     = nullptr;
         QTreeWidgetItem*  mAftShapeDiameterItem   = nullptr;
         QTreeWidgetItem*  mHeightItem             = nullptr;

         double            mSymmetryHorizontalY    = 0.0;
   };
}

namespace std
{
   // Specialize hash for ForwardShapeType
   template<>
   struct hash<Designer::GeometryBody::ForwardShapeType>
   {
      size_t operator ()(Designer::GeometryBody::ForwardShapeType x) const noexcept
      {
         auto h = std::hash<int>();
         return h(static_cast<int>(x));
      }
   };
}
#endif // ! GEOMETRY_BODY_HPP
