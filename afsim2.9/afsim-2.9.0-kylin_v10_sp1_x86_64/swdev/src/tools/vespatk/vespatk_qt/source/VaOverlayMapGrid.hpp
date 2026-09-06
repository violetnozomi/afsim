// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef VAOVERLAYMAPGRID_HPP
#define VAOVERLAYMAPGRID_HPP

#include "vespatk_qt_export.h"

#include <osg/Geometry>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osg/Texture2D>

#include "UtAngle.hpp"
#include "UtoRawShape.hpp"
#include "VaCallbackHolder.hpp"
#include "VaOverlay.hpp"

namespace vespa
{

class VESPATK_QT_EXPORT VaOverlayMapGrid : public VaOverlay
{
public:
   VaOverlayMapGrid();

   VaOverlayMapGrid(const std::string& aName);

   ~VaOverlayMapGrid() override = default;

   VaOverlayMapGrid* Clone() const override;

   bool Initialize() override;

   virtual void SetColor(const float* aColor);
   virtual void SetWidth(const float& aWidth);

   void SetDensityFactor(float aValue);

private:
   void         CreateLabelNode(const std::string& aText, const double aPosition[3], std::set<size_t>& aToDel);
   virtual void CameraUpdatedCB(VaViewer* aViewerPtr);

   void   GenerateGridData(VaCamera* aCameraPtr);
   double ComputeDelta(double aAngle);
   void   GenerateCurvedGrid(VaCamera* aCameraPtr);
   void   GenerateRectangularGrid(VaCamera* aCameraPtr);
   void   GenerateLabels(VaCamera* aCameraPtr);
   void   GenerateLatLines(VaCamera* aCameraPtr, double aMin, double aMax, unsigned int& aVTotal);

   VaCallbackHolder mCallbacks;

   float mWidth;
   float mColor[4];
   float mDensity;

   bool mGridDataGenerated;

   UtAngle::Fmt mLabelFormat;
   double       mMinLat;
   double       mMaxLat;
   double       mDeltaLat;
   unsigned int mNumLatLines;
   double       mMinLon;
   double       mMaxLon;
   double       mDeltaLon;
   unsigned int mNumLonLines;
   bool         mCrossesDateline;
   double       mAltOffset;

   UtoRawShape*                              mShapePtr;
   osg::ref_ptr<osg::MatrixTransform>        mMatrixPtr;
   osg::ref_ptr<osg::Geometry>               mGeometryPtr;
   osg::ref_ptr<osg::Vec3Array>              mVArrayPtr;
   osg::ref_ptr<osg::Vec4Array>              mCArrayPtr;
   osg::ref_ptr<osg::Group>                  mLabelGroup;
   std::map<size_t, osg::ref_ptr<osg::Node>> mNodeMap;
};

} // namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaOverlayMapGrid)
#endif
