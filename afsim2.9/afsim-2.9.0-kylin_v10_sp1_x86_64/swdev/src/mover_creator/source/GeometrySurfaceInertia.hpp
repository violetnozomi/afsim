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

#ifndef GEOMETRY_SURFACE_INERTIA_HPP
#define GEOMETRY_SURFACE_INERTIA_HPP

#include <string>

#include <QComboBox>
#include <QLineEdit>
#include <QTreeWidgetItem>

#include "GeometryObject.hpp"
#include "UtMath.hpp"
#include "UtVec3dX.hpp"

// ****************************************************************************
// GeometrySurfaceInertia is used to calculate the moments of inertia
// (rotational inertia) of aerodynamic surfaces.
// ****************************************************************************

namespace Designer
{
   class GeometrySurface;

   class GeometrySurfaceInertia
   {
      public:
         GeometrySurfaceInertia();
         virtual ~GeometrySurfaceInertia();

         void SetParent(GeometrySurface* aParent);

         // This is called by the parent GeometrySurface to calculate the rotational inertia
         // for a single surface object (each piece of a two piece "symmetrical object"
         // should be calculated separately.
         void CalculateRotationalInertia(double   aSpan_ft,
                                         double   aArea_sqft,
                                         double   aMass_slugs,
                                         UtVec3dX aRefPoint_ft,
                                         double&  aCalculatedIxx,
                                         double&  aCalculatedIyy,
                                         double&  aCalculatedIzz);

         // This provides a set of data that is useful for graphical display.
         // It provides data for the main object, unless aSymetricalObject is true.
         // It returns true if the data is valid.
         bool GetInertiaCompositionData(UtVec3dX&                           aRootFrontPt,
                                        UtVec3dX&                           aRootRearPt,
                                        UtVec3dX&                           aTipRearPt,
                                        UtVec3dX&                           aTipFrontPt,
                                        std::vector<std::vector<UtVec3dX>>& aRectSet) const;

      protected:
         GeometrySurface*                    mParent           = nullptr;
         bool                                mDataInitialized  = false;
         UtVec3dX                            mRootFrontPt;
         UtVec3dX                            mRootRearPt;
         UtVec3dX                            mTipRearPt;
         UtVec3dX                            mTipFrontPt;
         std::vector<std::vector<UtVec3dX>>  mRectSet;
   };
}
#endif // !GEOMETRY_SURFACE_INERTIA_HPP
