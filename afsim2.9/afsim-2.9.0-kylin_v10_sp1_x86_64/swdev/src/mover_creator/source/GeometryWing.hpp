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

#ifndef GEOMETRY_WING_HPP
#define GEOMETRY_WING_HPP

#include <string>

#include "GeometrySurface.hpp"

// ****************************************************************************
// GeometryWing is derived from GeometrySurface and provides data for
// the wing, which is a surface object with additional capabilities.
// ****************************************************************************

namespace Designer
{
   class GeometryWing : public GeometrySurface
   {
      public:
         explicit GeometryWing(Vehicle* aVehicle);
         virtual ~GeometryWing();

         double GetAspectRatio() override;
         double GetPlanformArea_ft2() override;

         bool   GetAileronsPresent()                      { return mAileronsPresent; }
         double GetAileronsChordFractionStart()           { return mAileronsChordFractionStart; }
         double GetAileronsChordFractionEnd()             { return mAileronsChordFractionEnd; }
         double GetAileronsSpanFractionStart()            { return mAileronsSpanFractionStart; }
         double GetAileronsSpanFractionEnd()              { return mAileronsSpanFractionEnd; }
         bool   GetAileronsUseExponentialAngleMapping()   { return mAileronsUseExponentialAngleMapping; }
         double GetAileronsControlSurfaceMinAngle_deg()   { return mAileronsControlSurfaceMinAngle_deg; }
         double GetAileronsControlSurfaceMaxAngle_deg()   { return mAileronsControlSurfaceMaxAngle_deg; }
         double GetAileronsActuatorMinRate_dps()          { return mAileronsActuatorMinRate_dps; }
         double GetAileronsActuatorMaxRate_dps()          { return mAileronsActuatorMaxRate_dps; }
         double GetAileronsActuatorMinAngle_deg()         { return mAileronsActuatorMinAngle_deg; }
         double GetAileronsActuatorMaxAngle_deg()         { return mAileronsActuatorMaxAngle_deg; }

         bool   GetDrageronsPresent()                     { return mDrageronsPresent; }
         double GetDrageronsChordFractionStart()          { return mDrageronsChordFractionStart; }
         double GetDrageronsChordFractionEnd()            { return mDrageronsChordFractionEnd; }
         double GetDrageronsSpanFractionStart()           { return mDrageronsSpanFractionStart; }
         double GetDrageronsSpanFractionEnd()             { return mDrageronsSpanFractionEnd; }
         bool   GetDrageronsUseExponentialAngleMapping()  { return mDrageronsUseExponentialAngleMapping; }
         double GetDrageronsControlSurfaceMinAngle_deg()  { return mDrageronsControlSurfaceMinAngle_deg; }
         double GetDrageronsControlSurfaceMaxAngle_deg()  { return mDrageronsControlSurfaceMaxAngle_deg; }
         double GetDrageronsActuatorMinRate_dps()         { return mDrageronsActuatorMinRate_dps; }
         double GetDrageronsActuatorMaxRate_dps()         { return mDrageronsActuatorMaxRate_dps; }
         double GetDrageronsActuatorMinAngle_deg()        { return mDrageronsActuatorMinAngle_deg; }
         double GetDrageronsActuatorMaxAngle_deg()        { return mDrageronsActuatorMaxAngle_deg; }

         bool   GetSpoileronsPresent()                    { return mSpoileronsPresent; }
         double GetSpoileronsChordFractionStart()         { return mSpoileronsChordFractionStart; }
         double GetSpoileronsChordFractionEnd()           { return mSpoileronsChordFractionEnd; }
         double GetSpoileronsSpanFractionStart()          { return mSpoileronsSpanFractionStart; }
         double GetSpoileronsSpanFractionEnd()            { return mSpoileronsSpanFractionEnd; }
         bool   GetSpoileronsUseExponentialAngleMapping() { return mSpoileronsUseExponentialAngleMapping; }
         double GetSpoileronsControlSurfaceMinAngle_deg() { return mSpoileronsControlSurfaceMinAngle_deg; }
         double GetSpoileronsControlSurfaceMaxAngle_deg() { return mSpoileronsControlSurfaceMaxAngle_deg; }
         double GetSpoileronsActuatorMinRate_dps()        { return mSpoileronsActuatorMinRate_dps; }
         double GetSpoileronsActuatorMaxRate_dps()        { return mSpoileronsActuatorMaxRate_dps; }
         double GetSpoileronsActuatorMinAngle_deg()       { return mSpoileronsActuatorMinAngle_deg; }
         double GetSpoileronsActuatorMaxAngle_deg()       { return mSpoileronsActuatorMaxAngle_deg; }

         bool   GetSpoilersPresent()                      { return mSpoilersPresent; }
         double GetSpoilersChordFractionStart()           { return mSpoilersChordFractionStart; }
         double GetSpoilersChordFractionEnd()             { return mSpoilersChordFractionEnd; }
         double GetSpoilersSpanFractionStart()            { return mSpoilersSpanFractionStart; }
         double GetSpoilersSpanFractionEnd()              { return mSpoilersSpanFractionEnd; }
         bool   GetSpoilersUseExponentialAngleMapping()   { return mSpoilersUseExponentialAngleMapping; }
         double GetSpoilersControlSurfaceMinAngle_deg()   { return mSpoilersControlSurfaceMinAngle_deg; }
         double GetSpoilersControlSurfaceMaxAngle_deg()   { return mSpoilersControlSurfaceMaxAngle_deg; }
         double GetSpoilersActuatorMinRate_dps()          { return mSpoilersActuatorMinRate_dps; }
         double GetSpoilersActuatorMaxRate_dps()          { return mSpoilersActuatorMaxRate_dps; }
         double GetSpoilersActuatorMinAngle_deg()         { return mSpoilersActuatorMinAngle_deg; }
         double GetSpoilersActuatorMaxAngle_deg()         { return mSpoilersActuatorMaxAngle_deg; }

         bool   GetElevonsPresent()                      { return mElevonsPresent; }
         double GetElevonsChordFractionStart()           { return mElevonsChordFractionStart; }
         double GetElevonsChordFractionEnd()             { return mElevonsChordFractionEnd; }
         double GetElevonsSpanFractionStart()            { return mElevonsSpanFractionStart; }
         double GetElevonsSpanFractionEnd()              { return mElevonsSpanFractionEnd; }
         bool   GetElevonsUseExponentialAngleMapping()   { return mElevonsUseExponentialAngleMapping; }
         double GetElevonsControlSurfaceMinAngle_deg()   { return mElevonsControlSurfaceMinAngle_deg; }
         double GetElevonsControlSurfaceMaxAngle_deg()   { return mElevonsControlSurfaceMaxAngle_deg; }
         double GetElevonsActuatorMinRate_dps()          { return mElevonsActuatorMinRate_dps; }
         double GetElevonsActuatorMaxRate_dps()          { return mElevonsActuatorMaxRate_dps; }
         double GetElevonsActuatorMinAngle_deg()         { return mElevonsActuatorMinAngle_deg; }
         double GetElevonsActuatorMaxAngle_deg()         { return mElevonsActuatorMaxAngle_deg; }

         std::vector<GeometrySurface::GeometryPanel> GetPanels() override;

         void CalcAspectRatioAndArea() override;

         bool IsSymmetrical() override { return true; }; // Wings are always symmetrical

         // This is called to load the widget data (GUI functionality)
         QTreeWidgetItem* CreateTreeWidgetItem(QString& aName, QTreeWidget* aTreeWidgetPtr, bool aNoneditableName) override;

         // This is called to calculate a full set of mass properties, given the specified total mass.
         // The calculations include any symmetry effects and represent the combined mass properties for
         // the entire set of subobjects, if appropriate. This function sets mCalculatedMass_lbs and
         // then calculates mCalculatedCG_ft, mCalculatedIxx, mCalculatedIyy, and mCalculatedIzz.
         void CalcMassProperties(double aTotalMass_lbs) override;

         // This calculates the max/min extent of the object in all three directions (x,y,z)
         // Return true if extents are valid (some object do not support "extent").
         bool CalcExtents(double& xMin, double& xMax, double& yMin, double& yMax, double& zMin, double& zMax) override;

         // This returns the mass area which is a representation of the appropriate reference
         // area for the object. It is used by GeometryMassProperties to calculate the mass of the object.
         double GetMassArea_ft2() override;

         // This calculates the single-sided fin span, measured from the x-axis. It only uses a single side,
         // so surfaces with symmetry do not give double values.
         double GetFinSpan() override;

         // This performs an "audit" to check that a geometry object is fully
         // defined, identifying any issues that may exist. The function returns a
         // list of QStrings that detail any problems. If no problems are found,
         // the list will be empty.
         QStringList Audit() override;

      private slots:
         void HandleAileronsPresentCheckBoxChanged(int aValue);
         void HandleAileronsChordFractionStartLineEditChanged(QString aValue);
         void HandleAileronsChordFractionEndLineEditChanged(QString aValue);
         void HandleAileronsSpanFractionStartLineEditChanged(QString aValue);
         void HandleAileronsSpanFractionEndLineEditChanged(QString aValue);
         void HandleAileronsUseExponentialAngleMappingCheckBoxChanged(int aValue);
         void HandleAileronsControlSurfaceMinAngleLineEditChanged(QString aValue);
         void HandleAileronsControlSurfaceMaxAngleLineEditChanged(QString aValue);
         void HandleAileronsActuatorMinRateLineEditChanged(QString aValue);
         void HandleAileronsActuatorMaxRateLineEditChanged(QString aValue);
         void HandleAileronsActuatorMinAngleLineEditChanged(QString aValue);
         void HandleAileronsActuatorMaxAngleLineEditChanged(QString aValue);
         void HandleDrageronsPresentCheckBoxChanged(int aValue);
         void HandleDrageronsChordFractionStartLineEditChanged(QString aValue);
         void HandleDrageronsChordFractionEndLineEditChanged(QString aValue);
         void HandleDrageronsSpanFractionStartLineEditChanged(QString aValue);
         void HandleDrageronsSpanFractionEndLineEditChanged(QString aValue);
         void HandleDrageronsUseExponentialAngleMappingCheckBoxChanged(int aValue);
         void HandleDrageronsControlSurfaceMinAngleLineEditChanged(QString aValue);
         void HandleDrageronsControlSurfaceMaxAngleLineEditChanged(QString aValue);
         void HandleDrageronsActuatorMinRateLineEditChanged(QString aValue);
         void HandleDrageronsActuatorMaxRateLineEditChanged(QString aValue);
         void HandleDrageronsActuatorMinAngleLineEditChanged(QString aValue);
         void HandleDrageronsActuatorMaxAngleLineEditChanged(QString aValue);
         void HandleSpoileronsPresentCheckBoxChanged(int aValue);
         void HandleSpoileronsChordFractionStartLineEditChanged(QString aValue);
         void HandleSpoileronsChordFractionEndLineEditChanged(QString aValue);
         void HandleSpoileronsSpanFractionStartLineEditChanged(QString aValue);
         void HandleSpoileronsSpanFractionEndLineEditChanged(QString aValue);
         void HandleSpoileronsUseExponentialAngleMappingCheckBoxChanged(int aValue);
         void HandleSpoileronsControlSurfaceMinAngleLineEditChanged(QString aValue);
         void HandleSpoileronsControlSurfaceMaxAngleLineEditChanged(QString aValue);
         void HandleSpoileronsActuatorMinRateLineEditChanged(QString aValue);
         void HandleSpoileronsActuatorMaxRateLineEditChanged(QString aValue);
         void HandleSpoileronsActuatorMinAngleLineEditChanged(QString aValue);
         void HandleSpoileronsActuatorMaxAngleLineEditChanged(QString aValue);
         void HandleSpoilersPresentCheckBoxChanged(int aValue);
         void HandleSpoilersChordFractionStartLineEditChanged(QString aValue);
         void HandleSpoilersChordFractionEndLineEditChanged(QString aValue);
         void HandleSpoilersSpanFractionStartLineEditChanged(QString aValue);
         void HandleSpoilersSpanFractionEndLineEditChanged(QString aValue);
         void HandleSpoilersUseExponentialAngleMappingCheckBoxChanged(int aValue);
         void HandleSpoilersControlSurfaceMinAngleLineEditChanged(QString aValue);
         void HandleSpoilersControlSurfaceMaxAngleLineEditChanged(QString aValue);
         void HandleSpoilersActuatorMinRateLineEditChanged(QString aValue);
         void HandleSpoilersActuatorMaxRateLineEditChanged(QString aValue);
         void HandleSpoilersActuatorMinAngleLineEditChanged(QString aValue);
         void HandleSpoilersActuatorMaxAngleLineEditChanged(QString aValue);
         void HandleElevonsPresentCheckBoxChanged(int aValue);
         void HandleElevonsChordFractionStartLineEditChanged(QString aValue);
         void HandleElevonsChordFractionEndLineEditChanged(QString aValue);
         void HandleElevonsSpanFractionStartLineEditChanged(QString aValue);
         void HandleElevonsSpanFractionEndLineEditChanged(QString aValue);
         void HandleElevonsUseExponentialAngleMappingCheckBoxChanged(int aValue);
         void HandleElevonsControlSurfaceMinAngleLineEditChanged(QString aValue);
         void HandleElevonsControlSurfaceMaxAngleLineEditChanged(QString aValue);
         void HandleElevonsActuatorMinRateLineEditChanged(QString aValue);
         void HandleElevonsActuatorMaxRateLineEditChanged(QString aValue);
         void HandleElevonsActuatorMinAngleLineEditChanged(QString aValue);
         void HandleElevonsActuatorMaxAngleLineEditChanged(QString aValue);

      private:

         bool   mAileronsPresent                      = false;
         double mAileronsChordFractionStart           = 0.8;
         double mAileronsChordFractionEnd             = 1.0;
         double mAileronsSpanFractionStart            = 0.2;
         double mAileronsSpanFractionEnd              = 0.6;
         bool   mAileronsUseExponentialAngleMapping   = false;
         double mAileronsControlSurfaceMinAngle_deg   = -60.0;
         double mAileronsControlSurfaceMaxAngle_deg   = 60.0;
         double mAileronsActuatorMinRate_dps          = -300.00;
         double mAileronsActuatorMaxRate_dps          = 300.00;
         double mAileronsActuatorMinAngle_deg         = -60.0;
         double mAileronsActuatorMaxAngle_deg         = 60.0;

         bool   mDrageronsPresent                     = false;
         double mDrageronsChordFractionStart          = 0.7;
         double mDrageronsChordFractionEnd            = 1.0;
         double mDrageronsSpanFractionStart           = 0.0;
         double mDrageronsSpanFractionEnd             = 1.0;
         bool   mDrageronsUseExponentialAngleMapping  = false;
         double mDrageronsControlSurfaceMinAngle_deg  = -60.0;
         double mDrageronsControlSurfaceMaxAngle_deg  = 0.0;
         double mDrageronsActuatorMinRate_dps         = -300.00;
         double mDrageronsActuatorMaxRate_dps         = 300.00;
         double mDrageronsActuatorMinAngle_deg        = -60.0;
         double mDrageronsActuatorMaxAngle_deg        = 0.0;

         bool   mSpoileronsPresent                    = false;
         double mSpoileronsChordFractionStart         = 0.8;
         double mSpoileronsChordFractionEnd           = 1.0;
         double mSpoileronsSpanFractionStart          = 0.2;
         double mSpoileronsSpanFractionEnd            = 0.6;
         bool   mSpoileronsUseExponentialAngleMapping = false;
         double mSpoileronsControlSurfaceMinAngle_deg = 0.0;
         double mSpoileronsControlSurfaceMaxAngle_deg = 60.0;
         double mSpoileronsActuatorMinRate_dps        = -300.00;
         double mSpoileronsActuatorMaxRate_dps        = 300.00;
         double mSpoileronsActuatorMinAngle_deg       = 0.0;
         double mSpoileronsActuatorMaxAngle_deg       = 60.0;

         bool   mSpoilersPresent                      = false;
         double mSpoilersChordFractionStart           = 0.5;
         double mSpoilersChordFractionEnd             = 0.8;
         double mSpoilersSpanFractionStart            = 0.2;
         double mSpoilersSpanFractionEnd              = 0.6;
         bool   mSpoilersUseExponentialAngleMapping   = false;
         double mSpoilersControlSurfaceMinAngle_deg   = 0.0;
         double mSpoilersControlSurfaceMaxAngle_deg   = 60.0;
         double mSpoilersActuatorMinRate_dps          = -300.00;
         double mSpoilersActuatorMaxRate_dps          = 300.00;
         double mSpoilersActuatorMinAngle_deg         = 0.0;
         double mSpoilersActuatorMaxAngle_deg         = 60.0;

         bool   mElevonsPresent                      = false;
         double mElevonsChordFractionStart           = 0.8;
         double mElevonsChordFractionEnd             = 1.0;
         double mElevonsSpanFractionStart            = 0.2;
         double mElevonsSpanFractionEnd              = 0.6;
         bool   mElevonsUseExponentialAngleMapping   = false;
         double mElevonsControlSurfaceMinAngle_deg   = 0.0;
         double mElevonsControlSurfaceMaxAngle_deg   = 60.0;
         double mElevonsActuatorMinRate_dps          = -300.00;
         double mElevonsActuatorMaxRate_dps          = 300.00;
         double mElevonsActuatorMinAngle_deg         = 0.0;
         double mElevonsActuatorMaxAngle_deg         = 60.0;

         QTreeWidgetItem* mAileronsPresentCheckBoxChangedWidget                      = nullptr;
         QTreeWidgetItem* mAileronsChordFractionStartWidget                          = nullptr;
         QTreeWidgetItem* mAileronsChordFractionEndWidget                            = nullptr;
         QTreeWidgetItem* mAileronsSpanFractionStartWidget                           = nullptr;
         QTreeWidgetItem* mAileronsSpanFractionEndWidget                             = nullptr;
         QTreeWidgetItem* mAileronsUseExponentialAngleMappingCheckBoxChangedWidget   = nullptr;
         QTreeWidgetItem* mAileronsControlSurfaceMinAngleLineEditChangedWidget       = nullptr;
         QTreeWidgetItem* mAileronsControlSurfaceMaxAngleLineEditChangedWidget       = nullptr;
         QTreeWidgetItem* mAileronsActuatorMinRateLineEditChangedWidget              = nullptr;
         QTreeWidgetItem* mAileronsActuatorMaxRateLineEditChangedWidget              = nullptr;
         QTreeWidgetItem* mAileronsActuatorMinAngleLineEditChangedWidget             = nullptr;
         QTreeWidgetItem* mAileronsActuatorMaxAngleLineEditChangedWidget             = nullptr;

         QTreeWidgetItem* mDrageronsPresentCheckBoxChangedWidget                     = nullptr;
         QTreeWidgetItem* mDrageronsChordFractionStartWidget                         = nullptr;
         QTreeWidgetItem* mDrageronsChordFractionEndWidget                           = nullptr;
         QTreeWidgetItem* mDrageronsSpanFractionStartWidget                          = nullptr;
         QTreeWidgetItem* mDrageronsSpanFractionEndWidget                            = nullptr;
         QTreeWidgetItem* mDrageronsUseExponentialAngleMappingCheckBoxChangedWidget  = nullptr;
         QTreeWidgetItem* mDrageronsControlSurfaceMinAngleLineEditChangedWidget      = nullptr;
         QTreeWidgetItem* mDrageronsControlSurfaceMaxAngleLineEditChangedWidget      = nullptr;
         QTreeWidgetItem* mDrageronsActuatorMinRateLineEditChangedWidget             = nullptr;
         QTreeWidgetItem* mDrageronsActuatorMaxRateLineEditChangedWidget             = nullptr;
         QTreeWidgetItem* mDrageronsActuatorMinAngleLineEditChangedWidget            = nullptr;
         QTreeWidgetItem* mDrageronsActuatorMaxAngleLineEditChangedWidget            = nullptr;

         QTreeWidgetItem* mSpoileronsPresentCheckBoxChangedWidget                    = nullptr;
         QTreeWidgetItem* mSpoileronsChordFractionStartWidget                        = nullptr;
         QTreeWidgetItem* mSpoileronsChordFractionEndWidget                          = nullptr;
         QTreeWidgetItem* mSpoileronsSpanFractionStartWidget                         = nullptr;
         QTreeWidgetItem* mSpoileronsSpanFractionEndWidget                           = nullptr;
         QTreeWidgetItem* mSpoileronsUseExponentialAngleMappingCheckBoxChangedWidget = nullptr;
         QTreeWidgetItem* mSpoileronsControlSurfaceMinAngleLineEditChangedWidget     = nullptr;
         QTreeWidgetItem* mSpoileronsControlSurfaceMaxAngleLineEditChangedWidget     = nullptr;
         QTreeWidgetItem* mSpoileronsActuatorMinRateLineEditChangedWidget            = nullptr;
         QTreeWidgetItem* mSpoileronsActuatorMaxRateLineEditChangedWidget            = nullptr;
         QTreeWidgetItem* mSpoileronsActuatorMinAngleLineEditChangedWidget           = nullptr;
         QTreeWidgetItem* mSpoileronsActuatorMaxAngleLineEditChangedWidget           = nullptr;

         QTreeWidgetItem* mSpoilersPresentCheckBoxChangedWidget                      = nullptr;
         QTreeWidgetItem* mSpoilersChordFractionStartWidget                          = nullptr;
         QTreeWidgetItem* mSpoilersChordFractionEndWidget                            = nullptr;
         QTreeWidgetItem* mSpoilersSpanFractionStartWidget                           = nullptr;
         QTreeWidgetItem* mSpoilersSpanFractionEndWidget                             = nullptr;
         QTreeWidgetItem* mSpoilersUseExponentialAngleMappingCheckBoxChangedWidget   = nullptr;
         QTreeWidgetItem* mSpoilersControlSurfaceMinAngleLineEditChangedWidget       = nullptr;
         QTreeWidgetItem* mSpoilersControlSurfaceMaxAngleLineEditChangedWidget       = nullptr;
         QTreeWidgetItem* mSpoilersActuatorMinRateLineEditChangedWidget              = nullptr;
         QTreeWidgetItem* mSpoilersActuatorMaxRateLineEditChangedWidget              = nullptr;
         QTreeWidgetItem* mSpoilersActuatorMinAngleLineEditChangedWidget             = nullptr;
         QTreeWidgetItem* mSpoilersActuatorMaxAngleLineEditChangedWidget             = nullptr;

         QTreeWidgetItem* mElevonsPresentCheckBoxChangedWidget                       = nullptr;
         QTreeWidgetItem* mElevonsChordFractionStartWidget                           = nullptr;
         QTreeWidgetItem* mElevonsChordFractionEndWidget                             = nullptr;
         QTreeWidgetItem* mElevonsSpanFractionStartWidget                            = nullptr;
         QTreeWidgetItem* mElevonsSpanFractionEndWidget                              = nullptr;
         QTreeWidgetItem* mElevonsUseExponentialAngleMappingCheckBoxChangedWidget    = nullptr;
         QTreeWidgetItem* mElevonsControlSurfaceMinAngleLineEditChangedWidget        = nullptr;
         QTreeWidgetItem* mElevonsControlSurfaceMaxAngleLineEditChangedWidget        = nullptr;
         QTreeWidgetItem* mElevonsActuatorMinRateLineEditChangedWidget               = nullptr;
         QTreeWidgetItem* mElevonsActuatorMaxRateLineEditChangedWidget               = nullptr;
         QTreeWidgetItem* mElevonsActuatorMinAngleLineEditChangedWidget              = nullptr;
         QTreeWidgetItem* mElevonsActuatorMaxAngleLineEditChangedWidget              = nullptr;

   };
}
#endif // !GEOMETRY_WING_HPP
