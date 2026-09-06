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

#ifndef GEOMETRY_SURFACE_HPP
#define GEOMETRY_SURFACE_HPP

#include <QComboBox>
#include <QLineEdit>
#include <QTreeWidgetItem>
#include <string>

#include "GeometryObject.hpp"
#include "GeometrySurfaceInertia.hpp"
#include "UtMath.hpp"
#include "UtVec3dX.hpp"

// ****************************************************************************
// GeometrySurface is derived from GeometryObject and provides data for
// any surface such as stabilizers, tails, and fins. Wings are derived from
// surfaces and add additional capabilities.
// ****************************************************************************

namespace Designer
{
   class GeometrySurface : public GeometryObject
   {
      public:

         struct GeometryPanel
         {
            double mChordFractionStart = 0.0;
            double mChordFractionEnd = 0.0;
            double mSpanFractionStart = 0.0;
            double mSpanFractionEnd = 0.0;
         };

         explicit GeometrySurface(Vehicle* aVehicle);
         virtual ~GeometrySurface();

         enum eSurfaceControlMotion
         {
            cControlMotionFixed,
            cControlMotionControlSurface,
            cControlMotionFullFlying
         };

         // This moves the reference point by the specified amount
         void MoveRefPoint(UtVec3dX aMoveDelta_ft) override;

         double GetSpan_ft() { return mSpan_ft; }
         double GetRootChord_ft() { return mRootChord_ft; }
         double GetTipChord_ft() { return mTipChord_ft; }
         double GetSweepAngle_deg() { return mSweepAngle_rad * UtMath::cDEG_PER_RAD; }
         double GetDihedralAngle_deg() { return mDihedralAngle_rad * UtMath::cDEG_PER_RAD; }
         double GetIncidenceAngle_deg() { return mIncidenceAngle_rad * UtMath::cDEG_PER_RAD; }
         double GetThicknessRatio() { return mThicknessRatio; }
         double GetOswaldsEfficiency() { return  mOswaldsEfficiency; }
         double GetFinRefRadius_ft() { return mFinRefRadius_ft; }

         virtual double GetAspectRatio();
         virtual double GetPlanformArea_ft2() { return mArea_ft2; }

         eSurfaceControlMotion GetSurfaceControlMotion() { return mSurfaceControlMotion; }
         double GetControlSurfaceChordFractionStart()    { return mControlSurfaceChordFractionStart; }
         double GetControlSurfaceChordFractionEnd()      { return mControlSurfaceChordFractionEnd; }
         double GetControlSurfaceSpanFractionStart()     { return mControlSurfaceSpanFractionStart; }
         double GetControlSurfaceSpanFractionEnd()       { return mControlSurfaceSpanFractionEnd; }
         double GetControlSurfaceMinAngle_deg()          { return mControlSurfaceMinAngle_deg; }
         double GetControlSurfaceMaxAngle_deg()          { return mControlSurfaceMaxAngle_deg; }
         bool GetUseExponentialAngleMapping()            { return mUseExponentialAngleMapping; }
         double GetActuatorMinRate_dps()                 { return mActuatorMinRate_dps; }
         double GetActuatorMaxRate_dps()                 { return mActuatorMaxRate_dps; }
         double GetActuatorMinAngle_deg()                { return mActuatorMinAngle_deg; }
         double GetActuatorMaxAngle_deg()                { return mActuatorMaxAngle_deg; }

         virtual std::vector<GeometryPanel> GetPanels();

         int GetNumberofAeroSections() { return mNumAeroSections; }

         virtual void CalcAspectRatioAndArea();

         virtual bool IsSymmetrical() { return mSymmetrical; };

         // This is the y-coordinate of horizontal symmetry
         double  GetSymmetryHorizontalY() { return mSymmetryHorizontalY; }

         // This is the z-coordinate of vertical symmetry
         double  GetSymmetryVerticalZ() { return mSymmetryVerticalZ; }

         // The symmetry types include: "Single", "Horizontal", "Vertical", "+ Pattern", and "X Pattern"
         QString GetSymmetryString() { return mSymmetryString; }

         virtual bool QuadControlFins() { return mQuadControlFins; };

         // The quad control fin pattern types include: "+ Pattern" and "X Pattern"
         QString GetQuadControlFinPatternString() { return mQuadControlFinsPatternString; }

         QTreeWidgetItem* GetSymmetricalWidgetItem() { return mSymmetricalWidgetItem; }

         // This is called to load the widget data (GUI functionality)
         QTreeWidgetItem* CreateTreeWidgetItem(QString&     aName,
                                               QTreeWidget* aTreeWidgetPtr,
                                               bool         aMajorControlSurface) override;

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

         // This is called to calculate a full set of mass properties, given the specified total mass.
         // The calculations include any symmetry effects and represent the combined mass properties for
         // the entire set of subobjects, if appropriate. This function sets mCalculatedMass_lbs and
         // then calculates mCalculatedCG_ft, mCalculatedIxx, mCalculatedIyy, and mCalculatedIzz.
         void CalcMassProperties(double aTotalMass_lbs) override;

         // This provides the location of the cg. Returns true if valid.
         bool GetCG_ft(UtVec3dX& aCg_ft) override;

         // This calculates the single-sided fin span, measured from the x-axis. It only uses a single side,
         // so surfaces with symmetry do not give double values.
         virtual double GetFinSpan();

         // This performs an "audit" to check that a geometry object is fully
         // defined, identifying any issues that may exist. The function returns a
         // list of QStrings that detail any problems. If no problems are found,
         // the list will be empty.
         QStringList Audit() override;

         void SetAirfoil(QString aFoilName);
         QString GetAirfoil();

         const std::vector<GeometrySurfaceInertia>& GetSurfaceInertiaSet() const { return mSurfaceInertiaSet; }

         // This should be overridden by derived classes, if appropriate
         bool IsMovable() const override { return mIsMovable; }

      private slots:
         void HandleX_CoordinateLineEditChanged(QString aValue);
         void HandleY_CoordinateLineEditChanged(QString aValue);
         void HandleZ_CoordinateLineEditChanged(QString aValue);
         void HandleSpanLineEditChanged(QString aValue);
         void HandleRootChordLineEditChanged(QString aValue);
         void HandleTipChordLineEditChanged(QString aValue);
         void HandleSweepAngleLineEditChanged(QString aValue);
         void HandleDihedralAngleLineEditChanged(QString aValue);
         void HandleIncidenceAngleLineEditChanged(QString aValue);
         void HandleOswaldEfficiencyLineEditChanged(QString aValue);
         void HandleThicknessLineEditChanged(QString aValue);
         void HandleFinRefRadiusLineEditChanged(QString aValue);
         void HandleQuadControlFinsPatternComboBoxChanged(QString aValue);
         void HandleQuadControlFinsCheckBoxChanged(int aValue);
         void HandleIsMovableCheckBoxChanged(int aValue);
         void HandleSurfaceControlMotionComboBoxChanged(QString aValue);
         void HandleControlSurfaceChordFractionStartLineEditChanged(QString aValue);
         void HandleControlSurfaceChordFractionEndLineEditChanged(QString aValue);
         void HandleControlSurfaceSpanFractionStartLineEditChanged(QString aValue);
         void HandleControlSurfaceSpanFractionEndLineEditChanged(QString aValue);
         void HandleControlSurfaceMinAngleLineEditChanged(QString aValue);
         void HandleControlSurfaceMaxAngleLineEditChanged(QString aValue);
         void HandleExponentialAngleMappingCheckBoxChanged(int aValue);
         void HandleActuatorMinRateLineEditChanged(QString aValue);
         void HandleActuatorMaxRateLineEditChanged(QString aValue);
         void HandleActuatorMinAngleLineEditChanged(QString aValue);
         void HandleActuatorMaxAngleLineEditChanged(QString aValue);
         void HandleCalcAspectRatioLineEditChanged(QString aValue);
         void HandleCalcAreaLineEditChanged(QString aValue);
         void HandleUseManualAspectRatioCheckBoxChanged(int aValue);
         void HandleManualAspectRatioLineEditChanged(QString aValue);
         void HandleSymmetricalComboBoxChanged(QString aValue);
         void HandleSymmetryCannotBeChangedCheckBoxChanged(int aValue);
         void HandleNumAeroSectionsLineEditChanged(QString aValue);
         void HandleOpenAirfoil();

      protected:

         // This should be called to connect the x,y,z line edits. This may
         // be overridden to allow the use of different handlers
         void ConnectLineEdits() override;

         // This should be called to adjust the viewing distance and
         // repaint the GUI after an object is modified
         void AdjustAndRedraw(bool aRecalcMassAndInertia = false) override;

         // This is used to calculate mCalculatedCG_ft, mCalculatedIxx, mCalculatedIyy, and mCalculatedIzz
         void CalcSingleSurfaceMassProperties(GeometrySurfaceInertia& aSurfaceInertiaObject,
                                              double                  aMass_slugs,
                                              double                  aSpan_ft,
                                              double                  aArea_sqft,
                                              UtVec3dX                aRefPoint_ft);

         bool CalcExtentsForSurface(double&            aMinX,
                                    double&            aMaxX,
                                    double&            aMinY,
                                    double&            aMaxY,
                                    double&            aMinZ,
                                    double&            aMaxZ,
                                    double             aSpan,
                                    const std::string& aSymmetryString);

         void PopulateAirfoilList();

         double                  mSpan_ft                          = 0.0;
         double                  mRootChord_ft                     = 0.0;
         double                  mTipChord_ft                      = 0.0;
         double                  mThicknessRatio                   = 0.1;
         double                  mSweepAngle_rad                   = 0.0;
         double                  mDihedralAngle_rad                = 0.0;
         double                  mIncidenceAngle_rad               = 0.0;
         double                  mOswaldsEfficiency                = 0.85;
         double                  mAspectRatio                      = 0.0;
         double                  mArea_ft2                         = 0.0;
         double                  mFinRefRadius_ft                  = 0.0;

         int                     mNumAeroSections                  = 1;

         // Angle convention is "trailing edge down" (TED) is positive

         eSurfaceControlMotion   mSurfaceControlMotion              = cControlMotionFixed; // cControlMotionFixed, cControlMotionPanel, or cControlMotionFullFlying
         QString                 mSurfaceControlMotionString;

         bool                    mUseManualAspectRatio              = false;
         double                  mManualAspectRatio                 = 0.0;

         bool                    mSymmetrical                       = false;
         QString                 mSymmetryString;

         bool                    mIsMovable                         = false;

         bool                    mQuadControlFins                   = false;
         QString                 mQuadControlFinsPatternString;

         bool                    mSymmetryCannotBeChanged           = false;

         QLineEdit*              mDihedralAngleLineEdit             = nullptr;

         QTreeWidgetItem*        mSurfaceControlMotionWidget        = nullptr;  // This is used to hide the widget, if desired
         QTreeWidgetItem*        mFinRefRadiusWidget                = nullptr;  // This is used to hide the widget, if desired
         QTreeWidgetItem*        mQuadControlFinsPatternWidget      = nullptr;  // This is used to hide the widget, if desired
         QTreeWidgetItem*        mDihedralWidget                    = nullptr;  // This is used to hide the widget, if desired
         QTreeWidgetItem*        mUseManualAspectRatioWidget        = nullptr;  // This is used to hide the widget, if desired
         QTreeWidgetItem*        mManualAspectRatioWidget           = nullptr;  // This is used to hide the widget, if desired
         QComboBox*              mQuadControlFinsPatternComboBox    = nullptr;
         QLineEdit*              mFinRefRadiusLineEditPtr           = nullptr;
         QLineEdit*              mCalculatedAspectRatioLineEditPtr  = nullptr;
         QLineEdit*              mCalculatedAreaLineEditPtr         = nullptr;
         QTreeWidgetItem*        mSymmetricalWidgetItem             = nullptr;
         QComboBox*              mSymmetricalStringComboBox         = nullptr;
         QComboBox*              mAirfoilComboBox                   = nullptr;
         QString                 mAirfoilFile                       = "";

         double                  mSymmetryHorizontalY               = 0.0;
         double                  mSymmetryVerticalZ                 = 0.0;

         std::vector<GeometrySurfaceInertia>  mSurfaceInertiaSet;

      private:
         double                  mControlSurfaceChordFractionStart = 0.0;
         double                  mControlSurfaceChordFractionEnd = 1.0;
         double                  mControlSurfaceSpanFractionStart = 0.0;
         double                  mControlSurfaceSpanFractionEnd = 1.0;
         double                  mControlSurfaceMinAngle_deg = 0.0;
         double                  mControlSurfaceMaxAngle_deg = 0.0;
         bool                    mUseExponentialAngleMapping = false;
         double                  mActuatorMinRate_dps = -300.00;
         double                  mActuatorMaxRate_dps = 300.00;
         double                  mActuatorMinAngle_deg = 0.0;
         double                  mActuatorMaxAngle_deg = 0.0;

         QTreeWidgetItem*        mControlSurfaceChordFractionStartWidget = nullptr;  // This is used to hide the widget, if desired
         QTreeWidgetItem*        mControlSurfaceChordFractionEndWidget = nullptr;  // This is used to hide the widget, if desired
         QTreeWidgetItem*        mControlSurfaceSpanFractionStartWidget = nullptr;  // This is used to hide the widget, if desired
         QTreeWidgetItem*        mControlSurfaceSpanFractionEndWidget = nullptr;  // This is used to hide the widget, if desired
         QTreeWidgetItem*        mControlSurfaceMinAngleWidget = nullptr;  // This is used to hide the widget, if desired
         QTreeWidgetItem*        mControlSurfaceMaxAngleWidget = nullptr;  // This is used to hide the widget, if desired
         QTreeWidgetItem*        mExponentialAngleMappingWidget = nullptr;  // This is used to hide the widget, if desired
         QTreeWidgetItem*        mActuatorMinRateWidget = nullptr;  // This is used to hide the widget, if desired
         QTreeWidgetItem*        mActuatorMaxRateWidget = nullptr;  // This is used to hide the widget, if desired
         QTreeWidgetItem*        mActuatorMinAngleWidget = nullptr;  // This is used to hide the widget, if desired
         QTreeWidgetItem*        mActuatorMaxAngleWidget = nullptr;  // This is used to hide the widget, if desired
   };
}
#endif // !GEOMETRY_SURFACE_HPP
