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

#ifndef VAOVERLAYMAPRINGS_HPP
#define VAOVERLAYMAPRINGS_HPP

#include "vespatk_qt_export.h"

#include <osg/Geometry>
#include <osg/MatrixTransform>

#include "UtoRawShape.hpp"
#include "VaCallbackHolder.hpp"
#include "VaOverlay.hpp"

namespace vespa
{

class VESPATK_QT_EXPORT VaOverlayMapRings : public VaOverlay
{
public:
   VaOverlayMapRings();

   VaOverlayMapRings(const std::string& aName);

   ~VaOverlayMapRings() override = default;

   VaOverlayMapRings* Clone() const override;

   bool Initialize() override;

   virtual void SetColor(const float* aColor);
   virtual void SetWidth(const float& aWidth);

private:
   void         CreateLabelNode(const std::string& aText, const double aPosition[3], std::set<size_t>& aToDel);
   virtual void CameraUpdatedCB(VaViewer* aViewerPtr);
   virtual void UnitFormatChangedCB(int aMeasureType, int aUnit);

   void   GenerateRingData(VaCamera* aCameraPtr);
   double ComputeDelta(double aRange) const;

   VaCallbackHolder mCallbacks;

   float  mWidth;
   float  mColor[4];
   int    mFormat;
   bool   mRingDataGenerated;
   double mUserToMeters;

   UtoRawShape*                              mShapePtr;
   osg::ref_ptr<osg::MatrixTransform>        mMatrixPtr;
   osg::ref_ptr<osg::Geometry>               mGeometryPtr;
   osg::ref_ptr<osg::Vec3Array>              mVArrayPtr;
   osg::ref_ptr<osg::Vec4Array>              mCArrayPtr;
   osg::ref_ptr<osg::Group>                  mLabelGroup;
   std::map<size_t, osg::ref_ptr<osg::Node>> mNodeMap;
};

} // namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaOverlayMapRings)
#endif
