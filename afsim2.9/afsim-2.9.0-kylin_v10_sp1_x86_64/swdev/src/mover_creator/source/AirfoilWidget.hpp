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

#ifndef AIRFOIL_WIDGET_HPP
#define AIRFOIL_WIDGET_HPP

#include <memory>
#include <QDialog>

namespace Ui
{
   class AirfoilWidget;
}

namespace Designer
{
   class CubicSpline;
   class GeometrySurface;
   class AeroData;

   class AirfoilWidget : public QDialog
   {
         Q_OBJECT
      public:
         AirfoilWidget(GeometrySurface* aSurfacePtr,
                       QWidget*         aParent,
                       Qt::WindowFlags  aFlags = Qt::WindowFlags());

         ~AirfoilWidget();

         enum PlotType
         {
            cLIFT_VS_ALPHA,
            cDRAG_VS_ALPHA,
            cMOMENT_VS_ALPHA
         };

      private:
         void PopulateAirfoilList();
         void HandleAirfoilChanged();
         void HandleSelectClicked();
         void HandlePlotTypeChanged();
         void UpdatePlot();

         Ui::AirfoilWidget*         mUIPtr;
         std::shared_ptr<AeroData>  mAeroData;
         GeometrySurface*           mGeometrySurfacePtr;
   };
}
#endif // !AIRFOIL_WIDGET_HPP
