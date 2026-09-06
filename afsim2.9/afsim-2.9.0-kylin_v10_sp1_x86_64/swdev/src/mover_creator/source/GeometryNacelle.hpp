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

#ifndef GEOMETRY_NACELLE_HPP
#define GEOMETRY_NACELLE_HPP

#include <string>
#include <QTreeWidgetItem>

#include "GeometryBody.hpp"

// ****************************************************************************
// GeometryNacelle is derived from GeometryBody and provides data for
// engine nacelles.
// ****************************************************************************

namespace Designer
{
   class GeometryNacelle : public GeometryBody
   {
      public:
         explicit GeometryNacelle(Vehicle* aVehicle);
         virtual ~GeometryNacelle();

         double GetThickness_ft() { return mThickness_ft; }
         double GetHeightInner_ft() { return mHeightInner_ft; }
         double GetHeightOuter_ft() { return mHeightOuter_ft; }

         // This is called to load the widget data (GUI functionality)
         QTreeWidgetItem* CreateTreeWidgetItem(QString&     aName,
                                               QTreeWidget* aTreeWidgetPtr,
                                               bool         aNoneditableName) override;

         QString GetOverallShapeString() { return mOverallShapeString; }
         QString GetAftSectionString() { return mAftSectionString; }
         double GetAftSectionLength_ft() { return mAftSectionLength_ft; }
         double GetForwardSweepLength_ft() { return mForwardSweepLength_ft; }

         // This returns the mass area which is a representation of the appropriate reference
         // area for the object. It is used by GeometryMassProperties to calculate the mass of the object.
         double GetMassArea_ft2() override;

         // Since GeometryNacelle has slightly different symmetry options than GeometryBody,
         // it overrides the CalcMassProperties method. These calculations include any
         // symmetry effects and represent the combined mass properties for the entire set of
         // subobjects, if appropriate. This function sets mCalculatedMass_lbs and then
         // calculates mCalculatedCG_ft, mCalculatedIxx, mCalculatedIyy, and mCalculatedIzz.
         void CalcMassProperties(double aTotalMass_lbs) override;

         // This performs an "audit" to check that a geometry object is fully
         // defined, identifying any issues that may exist. The function returns a
         // list of QStrings that detail any problems. If no problems are found,
         // the list will be empty.
         QStringList Audit() override;

      private slots:
         void HandleThicknessLineEditChanged(QString aValue);
         void HandleOverallShapeComboBoxChanged(QString aValue);
         void HandleAftSectionComboBoxChanged(QString aValue);
         void HandleAftSectionLengthLineEditChanged(QString aValue);
         void HandleHeightInnerLineEditChanged(QString aValue);
         void HandleHeightOuterLineEditChanged(QString aValue);
         void HandleForwardSweepLengthLineEditChanged(QString aValue);

         void HandleGuiElementDependencies();

      protected:
         double   mThickness_ft          = 0.0;   // Thickness of the nacelle ring
         QString  mOverallShapeString    = "Rounded";
         QString  mAftSectionString      = "Blunt";
         double   mAftSectionLength_ft   = 0.0;
         double   mHeightInner_ft        = 0.0;
         double   mHeightOuter_ft        = 0.0;
         double   mForwardSweepLength_ft = 0.0;

         QTreeWidgetItem* mOverallShapeItem       = nullptr;
         QTreeWidgetItem* mAftSectionShapeItem    = nullptr;
         QTreeWidgetItem* mAftSectionLengthItem   = nullptr;
         QTreeWidgetItem* mHeightInnerItem        = nullptr;
         QTreeWidgetItem* mHeightOuterItem        = nullptr;
         QTreeWidgetItem* mForwardSweepLengthItem = nullptr;
   };
}
#endif // ! GEOMETRY_NACELLE_HPP
