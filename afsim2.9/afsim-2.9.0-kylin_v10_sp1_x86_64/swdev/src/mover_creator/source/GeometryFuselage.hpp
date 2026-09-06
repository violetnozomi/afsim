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

#ifndef GEOMETRY_FUSELAGE_HPP
#define GEOMETRY_FUSELAGE_HPP

#include <QTreeWidgetItem>
#include <string>

#include "GeometryBody.hpp"
#include "UtMath.hpp"

// ****************************************************************************
// GeometryFuselage is derived from GeometryBody and provides data for
// a fuselage of an aircraft.
// ****************************************************************************

namespace Designer
{
   class GeometryFuselage : public GeometryBody
   {
      public:
         explicit GeometryFuselage(Vehicle* aVehicle);
         virtual ~GeometryFuselage();

         bool    CanopyPresent() { return mCanopyPresent; }
         double  CanopyTotalLength() { return mCanopyTotalLength; }
         double  CanopyForwardLength() { return mCanopyForwardLength; }
         double  CanopyAftLength() { return mCanopyAftLength; }
         double  CanopyHeight() { return mCanopyHeight; }
         double  CanopyWidth() { return mCanopyWidth; }
         double  CanopyRefX() { return mCanopyRefX; }
         double  CanopyRefY() { return mCanopyRefY; }
         double  CanopyRefZ() { return mCanopyRefZ; }

         // This is called to load the widget data (GUI functionality)
         QTreeWidgetItem* CreateTreeWidgetItem(QString&     aName,
                                               QTreeWidget* aTreeWidgetPtr,
                                               bool         aNoneditableName) override;

         // Note: GeometryFuselage is derived from GeometryBody and is closely similar.
         // In particular, it is assumed that the cockpit area does not result in a
         // significant change in mass properties, allowing GeometryFuselage to use
         // the inherited CalcMassProperties method from GeometryBody.

         // This performs an "audit" to check that a geometry object is fully
         // defined, identifying any issues that may exist. The function returns a
         // list of QStrings that detail any problems. If no problems are found,
         // the list will be empty.
         QStringList Audit() override;

      protected slots:
         void HandleCanopyTotalLengthLineEditChanged(QString aValue);
         void HandleCanopyForwardLengthLineEditChanged(QString aValue);
         void HandleCanopyAftLengthLineEditChanged(QString aValue);
         void HandleCanopyHeightLineEditChanged(QString aValue);
         void HandleCanopyWidthLineEditChanged(QString aValue);
         void HandleCanopyRefX_LineEditChanged(QString aValue);
         void HandleCanopyRefY_LineEditChanged(QString aValue);
         void HandleCanopyRefZ_LineEditChanged(QString aValue);
         void HandleCanopyPresentCheckBoxChanged(int aValue);

      protected:

         // This should be called to adjust the viewing distance and
         // repaint the GUI after an object is modified
         void AdjustAndRedraw(bool aRecalcMassAndInertia = false) override;

         bool      mCanopyPresent       = false;
         double    mCanopyTotalLength   = 0.0;
         double    mCanopyForwardLength = 0.0;
         double    mCanopyAftLength     = 0.0;
         double    mCanopyHeight        = 0.0;
         double    mCanopyWidth         = 0.0;
         double    mCanopyRefX          = 0.0;
         double    mCanopyRefY          = 0.0;
         double    mCanopyRefZ          = 0.0;

         QTreeWidgetItem* mCanopyTotalLengthWidget   = nullptr;
         QTreeWidgetItem* mCanopyForwardLengthWidget = nullptr;
         QTreeWidgetItem* mCanopyAftLengthWidget     = nullptr;
         QTreeWidgetItem* mCanopyHeightWidget        = nullptr;
         QTreeWidgetItem* mCanopyWidthWidget         = nullptr;
         QTreeWidgetItem* mCanopyRefX_Widget         = nullptr;
         QTreeWidgetItem* mCanopyRefY_Widget         = nullptr;
         QTreeWidgetItem* mCanopyRefZ_Widget         = nullptr;
   };
}
#endif // ! GEOMETRY_FUSELAGE_HPP
