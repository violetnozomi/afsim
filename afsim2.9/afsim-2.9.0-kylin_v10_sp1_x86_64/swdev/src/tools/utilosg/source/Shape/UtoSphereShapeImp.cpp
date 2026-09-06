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

#include "UtoSphereShapeImp.hpp"

#include <osg/CullFace>
#include <osg/Depth>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LOD>
#include <osg/LineStipple>
#include <osg/LineWidth>

#include "UtMath.hpp"
#include "UtoGeometryUtil.hpp"

// runtime includes
#include <algorithm>
#include <assert.h>
#include <math.h>

#include <limits.h>

UtoSphereShapeImp::UtoSphereShapeImp(const double& rRadius,
                                     unsigned int  nSlices,
                                     unsigned int  nStacks,
                                     const double& rCFactorX,
                                     const double& rCFactorY,
                                     const double& rCFactorZ,
                                     bool          bShowSlices,
                                     bool          bShowStacks)
   : m_dRadius(rRadius)
   , m_dMinimumAltitude(0.0)
   , m_bBoundedMin(false)
   , m_dMaximumAltitude(0.0)
   , m_bBoundedMax(false)
   , m_dStartAngle(0)
   , m_dStopAngle(2 * M_PI)
   , m_dCFactorX(rCFactorX)
   , m_dCFactorY(rCFactorY)
   , m_dCFactorZ(rCFactorZ)
   , m_nStacks(nStacks)
   , m_nSlices(nSlices)
   , m_bStacksVisible(bShowStacks)
   , m_bSlicesVisible(bShowSlices)
   , m_bSphereVisible(true)
   , m_dLineWidth(1.0)
   , m_sStipplePattern((short)0xffff)
   , m_iStippleFactor(1)
   , mLighting(true)
   , m_pLODRoot(nullptr)
   , mLineColor(0.0, 0.0, 0.0, 1.0)
{
   // validate the input parameters
   if (nStacks < 3)
      m_nStacks = 3;
   if (nSlices < 3)
      m_nSlices = 3;
   if (rRadius <= 0)
      m_dRadius = 1.0;
   if (rCFactorX <= 0)
      m_dCFactorX = 1.0;
   if (rCFactorY <= 0)
      m_dCFactorY = 1.0;
   if (rCFactorZ <= 0)
      m_dCFactorZ = 1.0;

   mOrientationTransform = new osg::MatrixTransform;
   AddShapeChildNode(mOrientationTransform);

   // construct the sphere shape
   ConstructSphere();
}

UtoSphereShapeImp::UtoSphereShapeImp(const UtoSphereShapeImp& rSphereImp)
   : UtoShapeImp(rSphereImp)
   , m_dRadius(rSphereImp.m_dRadius)
   , m_dMinimumAltitude(rSphereImp.m_dMinimumAltitude)
   , m_bBoundedMin(rSphereImp.m_bBoundedMin)
   , m_dMaximumAltitude(rSphereImp.m_dMaximumAltitude)
   , m_bBoundedMax(rSphereImp.m_bBoundedMax)
   , m_dCFactorX(rSphereImp.m_dCFactorX)
   , m_dCFactorY(rSphereImp.m_dCFactorY)
   , m_dCFactorZ(rSphereImp.m_dCFactorZ)
   , m_nStacks(rSphereImp.m_nStacks)
   , m_nSlices(rSphereImp.m_nSlices)
   , m_bStacksVisible(rSphereImp.m_bStacksVisible)
   , m_bSlicesVisible(rSphereImp.m_bSlicesVisible)
   , m_bSphereVisible(rSphereImp.m_bSphereVisible)
   , m_dLineWidth(rSphereImp.m_dLineWidth)
   , m_sStipplePattern(rSphereImp.m_sStipplePattern)
   , m_iStippleFactor(rSphereImp.m_iStippleFactor)
   , mLighting(rSphereImp.mLighting)
   , m_pLODRoot(new osg::LOD)
   , mLineColor(rSphereImp.mLineColor)
{
   // stores the vertex pointers to be used by the stack and slice lines
   std::vector<osg::Array*> m_vVertices;

   // set the source and destination LODs
   const osg::LOD* pSrcLOD = rSphereImp.m_pLODRoot.get();
   osg::LOD*       pDesLOD = m_pLODRoot.get();
   m_pLODRoot->getOrCreateStateSet()->setMode(GL_LIGHTING, mLighting ? osg::StateAttribute::ON : osg::StateAttribute::OFF);
   // fill in all the children from slot one
   for (unsigned int i = 0; i < pSrcLOD->getNumChildren(); i++)
   {
      // obtain the child
      const osg::Geode* pGeode = static_cast<const osg::Geode*>(pSrcLOD->getChild(i));
      // obtain the geometry from the vector list of sphere geometry
      const osg::Geometry* pGeometry = rSphereImp.m_vSphereGeometry.at(pDesLOD == m_pLODRoot.get() ? i : i + 1).get();
      // make a copy of this geometry object
      osg::Geometry* pNewGeometry = new osg::Geometry(*pGeometry, osg::CopyOp::DEEP_COPY_ALL);
      // create a new geode for the lod
      osg::Geode* pNewGeode = new osg::Geode;
      // copy the stateset for the geode
      pNewGeode->setStateSet(new osg::StateSet(*pGeode->getStateSet()));
      // insert the geometry into the geode
      pNewGeode->addDrawable(pNewGeometry);
      // insert the geode into the lod
      pDesLOD->addChild(pNewGeode, pSrcLOD->getMinRange(i), pSrcLOD->getMaxRange(i));
      // insert the new geometry into the vector list
      m_vSphereGeometry.push_back(pNewGeometry);
      // add the vertex pointer to the list
      m_vVertices.push_back(pNewGeometry->getVertexArray());

      // switch the source and destination LODs
      if (pDesLOD == m_pLODRoot.get())
      {
         // create a new destination lod
         pDesLOD = new osg::LOD;
         // set the lod node to compute pixel sizes instead of distance from eye
         pDesLOD->setRangeMode(osg::LOD::PIXEL_SIZE_ON_SCREEN);
         // attach the lod node to the main lod node
         m_pLODRoot->addChild(pDesLOD, pSrcLOD->getMaxRange(i), FLT_MAX);
         // switch the src lod
         pSrcLOD = static_cast<const osg::LOD*>(rSphereImp.m_pLODRoot->getChild(1));
         // reset the iterator i
         i = UINT_MAX;
      }
   }

   LineAndVertexCopy(rSphereImp.m_vSliceLines, m_vSliceLines, m_vVertices);
   LineAndVertexCopy(rSphereImp.m_vSliceLinesMinRad, m_vSliceLinesMinRad, m_vVertices);
   LineAndVertexCopy(rSphereImp.m_vStackLines, m_vStackLines, m_vVertices);
   LineAndVertexCopy(rSphereImp.m_vStackLinesMinRad, m_vStackLinesMinRad, m_vVertices);

   mOrientationTransform = new osg::MatrixTransform;
   mOrientationTransform->addChild(m_pLODRoot.get());
   AddShapeChildNode(mOrientationTransform);

   // bind the sphere lines
   BindSphereLines();
}

UtoSphereShapeImp::~UtoSphereShapeImp()
{
   // clear the vectors
   m_vSliceLines.clear();
   m_vSliceLinesMinRad.clear();
   m_vStackLines.clear();
   m_vStackLinesMinRad.clear();
   m_vSphereGeometry.clear();

   // nullify the pointers
   m_pLODRoot = nullptr;
}

void UtoSphereShapeImp::LineAndVertexCopy(const GeometryVector&    aGeoVec,
                                          GeometryVector&          aNewGeoVec,
                                          std::vector<osg::Array*> aVertices)
{
   // obtain the size of the vector
   unsigned int nVecSize = aGeoVec.size();
   // do a deep copy of the slice vector
   for (unsigned int i = 0; i < nVecSize; i++)
   {
      // copy the geometry unit
      osg::Geometry* pGeometry = new osg::Geometry(*aGeoVec[i], osg::CopyOp::DEEP_COPY_ALL);
      // replace the vertex array with the vertex array from the sphere
      pGeometry->setVertexArray(aVertices.at(i));
      // assign the geode to the slice vector
      aNewGeoVec.push_back(pGeometry);
   }
}

void UtoSphereShapeImp::SetRadius(double rRadius)
{
   // set the new sphere shape values
   Set(rRadius, m_dMinimumRadius, m_nSlices, m_nStacks, m_dCFactorX, m_dCFactorY, m_dCFactorZ);
}

void UtoSphereShapeImp::SetMinimumRadius(double rMinRadius)
{
   Set(m_dRadius, rMinRadius, m_nSlices, m_nStacks, m_dCFactorX, m_dCFactorY, m_dCFactorZ);
}

void UtoSphereShapeImp::SetMinAltitude(double aMinAltitude)
{
   m_dMinimumAltitude = aMinAltitude;
   m_bBoundedMin      = true;
   ConstructSphere();
};

void UtoSphereShapeImp::SetMaxAltitude(double aMaxAltitude)
{
   m_dMaximumAltitude = aMaxAltitude;
   m_bBoundedMax      = true;
   ConstructSphere();
};

void UtoSphereShapeImp::SetOrientationTransform(const UtoOrientation& aOrientation)
{
   mOrientationTransform->setMatrix(osg::Matrix::rotate(aOrientation.yaw,
                                                        osg::Vec3d(0, 0, 1),
                                                        aOrientation.pitch,
                                                        osg::Vec3d(0, 1, 0),
                                                        aOrientation.roll,
                                                        osg::Vec3d(1, 0, 0)));
}

void UtoSphereShapeImp::SetAngleBounds(double aStartAngle, double aStopAngle)
{
   m_dStartAngle = aStartAngle * M_PI / 180;
   m_dStopAngle  = aStopAngle * M_PI / 180;
   ConstructSphere();
}

void UtoSphereShapeImp::SetStacks(unsigned int nStacks)
{
   // set the new sphere shape values
   Set(m_dRadius, m_dMinimumRadius, m_nSlices, nStacks, m_dCFactorX, m_dCFactorY, m_dCFactorZ);
}

void UtoSphereShapeImp::SetSlices(unsigned int nSlices)
{
   // set the new sphere shape values
   Set(m_dRadius, m_dMinimumRadius, nSlices, m_nStacks, m_dCFactorX, m_dCFactorY, m_dCFactorZ);
}

void UtoSphereShapeImp::SetCompressionFactorX(double rFactor)
{
   // set the new sphere shape values
   Set(m_dRadius, m_dMinimumRadius, m_nSlices, m_nStacks, rFactor, m_dCFactorY, m_dCFactorZ);
}

void UtoSphereShapeImp::SetCompressionFactorY(double rFactor)
{
   // set the new sphere shape values
   Set(m_dRadius, m_dMinimumRadius, m_nSlices, m_nStacks, m_dCFactorX, rFactor, m_dCFactorZ);
}

void UtoSphereShapeImp::SetCompressionFactorZ(double rFactor)
{
   // set the new sphere shape values
   Set(m_dRadius, m_dMinimumRadius, m_nSlices, m_nStacks, m_dCFactorX, m_dCFactorY, rFactor);
}

void UtoSphereShapeImp::Set(double       rRadius,
                            double       rMinRadius,
                            unsigned int nSlices,
                            unsigned int nStacks,
                            double       rCFactorX,
                            double       rCFactorY,
                            double       rCFactorZ)
{
   // validate a difference
   if (rRadius != m_dRadius || rMinRadius != m_dMinimumRadius || nSlices != m_nSlices || nStacks != m_nStacks ||
       rCFactorX != m_dCFactorX || rCFactorY != m_dCFactorY || rCFactorZ != m_dCFactorZ)
   {
      // set the member variables
      m_nSlices        = nSlices >= 3 ? nSlices : 3;
      m_nStacks        = nStacks >= 3 ? nStacks : 3;
      m_dRadius        = rRadius > 0.0 ? rRadius : 1.0;
      m_dMinimumRadius = rMinRadius > 0.0 ? rMinRadius : 0.0;
      m_dCFactorX      = rCFactorX > 0.0 ? rCFactorX : 1.0;
      m_dCFactorY      = rCFactorY > 0.0 ? rCFactorY : 1.0;
      m_dCFactorZ      = rCFactorZ > 0.0 ? rCFactorZ : 1.0;
      // construct the sphere
      ConstructSphere();
   }
}

void UtoSphereShapeImp::ShowSliceLines(bool bShow)
{
   // determine if any of the values have changed
   if (bShow != m_bSlicesVisible)
   {
      // indicate the change
      m_bSlicesVisible = bShow;
      // bind the new sphere lines
      ConstructSphere();
   }
}

void UtoSphereShapeImp::ShowStackLines(bool bShow)
{
   // determine if any of the values have changed
   if (bShow != m_bStacksVisible)
   {
      // indicate the change
      m_bStacksVisible = bShow;
      // bind the new sphere lines
      ConstructSphere();
   }
}

void UtoSphereShapeImp::ShowSphereGeometry(bool bShow)
{
   // determine if any of the values have changed
   if (bShow != m_bSphereVisible)
   {
      // indicate the change
      m_bSphereVisible = bShow;
      // bind the new sphere lines
      BindSphereLines();
   }
}

void UtoSphereShapeImp::SetLineWidth(double width)
{
   // determine if any of the values have changed
   if (width != m_dLineWidth)
   {
      // indicate the change
      m_dLineWidth = width;
      // construct the sphere
      ConstructSphere();
   }
}

void UtoSphereShapeImp::SetPattern(short pattern, int factor)
{
   // determine if any of the values have changed
   if (pattern != m_sStipplePattern || factor != m_iStippleFactor)
   {
      // indicate the change
      m_sStipplePattern = pattern;
      m_iStippleFactor  = factor;
      // construct the sphere
      ConstructSphere();
   }
}

void UtoSphereShapeImp::SetColor(const UtoColor& rColor, unsigned int nClrMask, unsigned int nReserved)
{
   // call the base class
   UtoShapeImp::SetColor(rColor, nClrMask, nReserved);

   // create a new color value
   osg::Vec4 vColor(m_oBlendColor.RedF(), m_oBlendColor.GreenF(), m_oBlendColor.BlueF(), m_oBlendColor.AlphaF());

   // obtain the main lod
   osg::LOD* pLOD = m_pLODRoot.get();

   // update all the main geodes with the new color values
   for (unsigned int i = 0; i < m_vSphereGeometry.size(); i++)
   {
      // obtain the color array
      osg::Array* pArray = m_vSphereGeometry.at(i)->getColorArray();
      // convert the array
      osg::Vec4Array* pColorArray = static_cast<osg::Vec4Array*>(pArray);
      // set the new color value
      pColorArray->at(0) = vColor;

      // obtain the geode
      osg::Geode* pGeode = static_cast<osg::Geode*>(pLOD->getChild(i == 0 ? i : i - 1));
      // enable or disable blending based on the alpha value
      if (rColor.HasTransparency())
      {
         UtoGeometryUtil::EnableBlend(pGeode);
      }
      else
      {
         UtoGeometryUtil::DisableBlend(pGeode);
      }

      // update the LOD
      if (pLOD == m_pLODRoot.get())
      {
         // obtain the other lod at 1
         pLOD = static_cast<osg::LOD*>(m_pLODRoot->getChild(1));
      }

      // dirty the display lists
      m_vSphereGeometry.at(i)->dirtyDisplayList();
   }
}

void UtoSphereShapeImp::SetLineColor(const UtoColor& aColor)
{
   // sanity check
   assert(m_vStackLines.size() == m_vSliceLines.size());

   mLineColor = aColor;

   auto vColor = osg::Vec4(aColor.RedF(), aColor.GreenF(), aColor.BlueF(), aColor.AlphaF());

   // update all the lines
   for (int i = m_vStackLines.size() - 1; i >= 0; i--)
   {
      // obtain the color array
      osg::Array* pArray = m_vStackLines.at(i)->getColorArray();
      // convert the array
      osg::Vec4Array* pColorArray = static_cast<osg::Vec4Array*>(pArray);
      // set the new color value
      pColorArray->at(0) = vColor;

      // obtain the color array
      pArray = m_vStackLinesMinRad.at(i)->getColorArray();
      // convert the array
      pColorArray = static_cast<osg::Vec4Array*>(pArray);
      // set the new color value
      pColorArray->at(0) = vColor;

      // obtain the color array
      pArray = m_vSliceLines.at(i)->getColorArray();
      // convert the array
      pColorArray = static_cast<osg::Vec4Array*>(pArray);
      // set the new color value
      pColorArray->at(0) = vColor;

      // obtain the color array
      pArray = m_vSliceLinesMinRad.at(i)->getColorArray();
      // convert the array
      pColorArray = static_cast<osg::Vec4Array*>(pArray);
      // set the new color value
      pColorArray->at(0) = vColor;

      // dirty the display lists
      m_vStackLines.at(i)->dirtyDisplayList();
      m_vStackLinesMinRad.at(i)->dirtyDisplayList();
      m_vSliceLines.at(i)->dirtyDisplayList();
      m_vSliceLinesMinRad.at(i)->dirtyDisplayList();
   }
}

void UtoSphereShapeImp::ConstructSphere()
{
   // the LOD algorithm for this sphere is really bad
   // this really needs to be replaced with something else
   // to do this right, you would have to really do this a runtime
   // might be in the best interest of the application to create
   // a special lod node just for the sphere itself.

   // release the vectors
   m_vStackLines.clear();
   m_vStackLinesMinRad.clear();
   m_vSliceLines.clear();
   m_vSliceLinesMinRad.clear();
   m_vSphereGeometry.clear();

   // create a new lod node, and add it to the transform.
   mOrientationTransform->removeChild(m_pLODRoot.get());
   m_pLODRoot = new osg::LOD;
   mOrientationTransform->addChild(m_pLODRoot.get());

   // determine what the scaled radius will be
   double dScaledRadius    = m_dRadius * Scaling();
   double dScaledMinRadius = m_dMinimumRadius * Scaling();

   if (dScaledMinRadius >= dScaledRadius)
   {
      dScaledMinRadius = 0;
   }

   // determine the first set of stacks
   unsigned int nStacks = m_nStacks;
   unsigned int nSlices = m_nSlices;

   // construct the first sphere
   // this sphere will be if the viewer is inside the sphere
   osg::Geode* pSphere =
      CreateSphere(dScaledRadius, dScaledMinRadius, nStacks, nSlices, m_dCFactorX, m_dCFactorY, m_dCFactorZ);
   // obtain the state of opengl
   //   int nFrontFace = 0;
   //   glGetIntegerv(GL_FRONT_FACE, &nFrontFace);
   //   // modify the state set for the node
   /*      pSphere->getOrCreateStateSet()->setAttributeAndModes(new osg::CullFace(nFrontFace == GL_CCW ?
         osg::CullFace::BACK : osg::CullFace::FRONT));*/

   pSphere->getOrCreateStateSet()->setAttributeAndModes(new osg::CullFace(osg::CullFace::FRONT));

   // attach the node to the level of detail node
   m_pLODRoot->addChild(pSphere, 0, dScaledRadius);

   // create a new node to be used by the main lod node
   // this node will use pixel size instead of distance from the view
   osg::LOD* pLOD = new osg::LOD;
   // set the lod node to compute pixel sizes instead of distance from eye
   pLOD->setRangeMode(osg::LOD::PIXEL_SIZE_ON_SCREEN);
   // attach the lod node to the main lod node
   m_pLODRoot->addChild(pLOD, dScaledRadius, FLT_MAX);

   // continue to process the first sphere for the outside
   bool bProcessSphere = true;
   // set the lod min and max values
   // will need a better way to choose the starting min lod value
   // smaller stack and slices will produce spheres that will change quickly
   // need a pay to change the way the multiplier change from the min to max.
   float fLODMin = 500.0, fLODMax = FLT_MAX;

   // add all the scaled nodes to the lod node
   while (bProcessSphere)
   {
      // validate the stacks and slices
      if (nStacks < 3)
         nStacks = 3;
      if (nSlices < 3)
         nSlices = 3;
      // conditional for when to stop
      bProcessSphere = nStacks != 3 && nSlices != 3;
      // create a new sphere
      pSphere = CreateSphere(dScaledRadius, dScaledMinRadius, nStacks, nSlices, m_dCFactorX, m_dCFactorY, m_dCFactorZ);
      // cull out the back faces of the geometry
      /*      pSphere->getOrCreateStateSet()->setAttributeAndModes(new osg::CullFace(nFrontFace == GL_CCW ?
         osg::CullFace::BACK : osg::CullFace::FRONT));*/

      pSphere->getOrCreateStateSet()->setAttributeAndModes(new osg::CullFace(osg::CullFace::FRONT));

      // attach the sphere to the lod node
      pLOD->addChild(pSphere, bProcessSphere ? fLODMin : 0.0f, fLODMax);
      // determine the best location to place the lod
      fLODMax = fLODMin;
      fLODMin = fLODMin * pow(5.2f, -1.0f);
      // reduce the number of stacks and slices by half
      nStacks >>= 1;
      nSlices >>= 1;
   }

   // bind the sphere lines
   BindSphereLines();

   SetLineColor(mLineColor);
}

void UtoSphereShapeImp::BindSphereLines()
{
   // set the local lod for the function
   osg::LOD* pLOD = m_pLODRoot.get();

   // remove or add all the line drawables from the geode
   for (unsigned int i = 0; i < pLOD->getNumChildren(); i++)
   {
      // obtain the child for the parent
      osg::Geode* pChild = static_cast<osg::Geode*>(pLOD->getChild(i));
      // obtain the stack and slice geometry units
      osg::Geometry* pStackGeometry       = m_vStackLines.at(pLOD == m_pLODRoot.get() ? i : i + 1).get();
      osg::Geometry* pStackGeometryMinRad = m_vStackLinesMinRad.at(pLOD == m_pLODRoot.get() ? i : i + 1).get();
      osg::Geometry* pSliceGeometry       = m_vSliceLines.at(pLOD == m_pLODRoot.get() ? i : i + 1).get();
      osg::Geometry* pSliceGeometryMinRad = m_vSliceLinesMinRad.at(pLOD == m_pLODRoot.get() ? i : i + 1).get();
      osg::Geometry* pSphereGeometry      = m_vSphereGeometry.at(pLOD == m_pLODRoot.get() ? i : i + 1).get();

      // EDIT: Stack and slice geometry is always part of the geode, but some of the
      // primitive sets don't get added to the geometry in CreateSphere(), depending
      // on values of these booleans. The effect is that the equator and meridians are
      // always visible, even if stack and slice lines are "invisible".

      if (m_bStacksVisible)
      {
         // remove the sphere geometry
         pChild->removeDrawable(pSphereGeometry);
         // add the stack geometry to the mix
         pChild->addDrawable(pStackGeometry);
         pChild->addDrawable(pStackGeometryMinRad);
         // add the sphere geometry
         if (m_bSphereVisible)
            pChild->addDrawable(pSphereGeometry);
         // modify the render bin location
         pStackGeometry->getOrCreateStateSet()->setRenderBinDetails(-1, "");
         pStackGeometryMinRad->getOrCreateStateSet()->setRenderBinDetails(-1, "");
      }
      else
      {
         // remove the stack geometry from the mix
         pChild->removeDrawable(pStackGeometry);
         pChild->removeDrawable(pStackGeometryMinRad);
      }

      if (m_bSlicesVisible)
      {
         // remove the sphere geometry
         pChild->removeDrawable(pSphereGeometry);
         // add the stack geometry to the mix
         pChild->addDrawable(pSliceGeometry);
         pChild->addDrawable(pSliceGeometryMinRad);
         // add back the sphere geometry
         if (m_bSphereVisible)
            pChild->addDrawable(pSphereGeometry);
         // modify the render bin location
         pSliceGeometry->getOrCreateStateSet()->setRenderBinDetails(-1, "");
         pSliceGeometry->getOrCreateStateSet()->setRenderBinDetails(-1, "");
      }
      else
      {
         // remove the stack geometry from the mix
         pChild->removeDrawable(pSliceGeometry);
         pChild->removeDrawable(pSliceGeometryMinRad);
      }

      if (pLOD == m_pLODRoot.get())
      {
         // replace the lod
         pLOD = static_cast<osg::LOD*>(m_pLODRoot->getChild(1));
         // reset the iterator i
         i = UINT_MAX;
      }
   }
}

osg::Geode* UtoSphereShapeImp::CreateSphere(double       rRadius,
                                            double       rMinRadius,
                                            unsigned int nSlices,
                                            unsigned int nStacks,
                                            double       rCFactorX,
                                            double       rCFactorY,
                                            double       rCFactorZ)
{
   // pNormals memory leak, why was it here?
   // osg::Vec3Array * pNormals = new osg::Vec3Array;
   // create the required arrays to get the job done
   osg::Vec3Array*           pVertices = new osg::Vec3Array;
   std::vector<unsigned int> pSphereIndices;

   // create a geometry unit for the stack and slice lines
   osg::Geometry* pSliceGeometry       = new osg::Geometry;
   osg::Geometry* pSliceGeometryMinRad = new osg::Geometry;
   osg::Geometry* pStackGeometry       = new osg::Geometry;
   osg::Geometry* pStackGeometryMinRad = new osg::Geometry;
   // allow the lines to have color
   osg::Vec4Array* pSphereColor = new osg::Vec4Array;
   // attach the color attributes
   pSphereColor->push_back(osg::Vec4(m_oBlendColor.RedF(), m_oBlendColor.GreenF(), m_oBlendColor.BlueF(), 1.0f));
   pSliceGeometry->setColorArray(pSphereColor);
   pSliceGeometryMinRad->setColorArray(pSphereColor);
   pStackGeometry->setColorArray(pSphereColor);
   pStackGeometryMinRad->setColorArray(pSphereColor);
   pSliceGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
   pStackGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
   pSliceGeometryMinRad->setColorBinding(osg::Geometry::BIND_OVERALL);
   pStackGeometryMinRad->setColorBinding(osg::Geometry::BIND_OVERALL);
   // attach the vertices to the geometry
   pSliceGeometry->setVertexArray(pVertices);
   pStackGeometry->setVertexArray(pVertices);
   pSliceGeometryMinRad->setVertexArray(pVertices);
   pStackGeometryMinRad->setVertexArray(pVertices);
   // push the geodes into the correct vectors
   m_vSliceLines.push_back(pSliceGeometry);
   m_vSliceLinesMinRad.push_back(pSliceGeometryMinRad);
   m_vStackLines.push_back(pStackGeometry);
   m_vStackLinesMinRad.push_back(pStackGeometryMinRad);
   // change the state set for line thickness and stippling
   osg::LineWidth* pLineWidth = new osg::LineWidth(m_dLineWidth);
   pSliceGeometry->getOrCreateStateSet()->setAttributeAndModes(pLineWidth);
   pStackGeometry->getOrCreateStateSet()->setAttributeAndModes(pLineWidth);
   pSliceGeometryMinRad->getOrCreateStateSet()->setAttributeAndModes(pLineWidth);
   pStackGeometryMinRad->getOrCreateStateSet()->setAttributeAndModes(pLineWidth);
   if (m_sStipplePattern == (short)0xffff)
   {
      pSliceGeometry->getStateSet()->removeAttribute(osg::StateAttribute::LINESTIPPLE);
      pStackGeometry->getStateSet()->removeAttribute(osg::StateAttribute::LINESTIPPLE);
      pSliceGeometryMinRad->getStateSet()->removeAttribute(osg::StateAttribute::LINESTIPPLE);
      pStackGeometryMinRad->getStateSet()->removeAttribute(osg::StateAttribute::LINESTIPPLE);
   }
   else
   {
      osg::LineStipple* pLineStipple = new osg::LineStipple(m_iStippleFactor, m_sStipplePattern);
      pSliceGeometry->getStateSet()->setAttributeAndModes(pLineStipple);
      pStackGeometry->getStateSet()->setAttributeAndModes(pLineStipple);
      pSliceGeometryMinRad->getStateSet()->setAttributeAndModes(pLineStipple);
      pStackGeometryMinRad->getStateSet()->setAttributeAndModes(pLineStipple);
   }

   // push back the very first vertex
   double startZ       = rRadius * rCFactorZ;
   double startZMinRad = rMinRadius * rCFactorZ;
   double endZMinRad   = -rMinRadius * rCFactorZ;
   double endZ         = -rRadius * rCFactorZ;
   bool   twoSpheres   = true;
   int    nextLevel    = nSlices + 1;
   if (m_bBoundedMin && startZ > m_dMinimumAltitude)
   {
      startZ = m_dMinimumAltitude;
   }

   if (m_bBoundedMax && endZ < m_dMaximumAltitude)
   {
      endZ = m_dMaximumAltitude;
   }

   if (m_bBoundedMin && startZMinRad > m_dMinimumAltitude)
   {
      startZMinRad = m_dMinimumAltitude;
   }

   if (m_bBoundedMax && endZMinRad < m_dMaximumAltitude)
   {
      endZMinRad = m_dMaximumAltitude;
   }
   // little sphere draw check
   if (startZ < endZMinRad || endZ > startZMinRad || rMinRadius <= 0)
   {
      twoSpheres = false;
   }
   pVertices->push_back(osg::Vec3(0.0f, 0.0f, startZ));
   if (twoSpheres)
   {
      pVertices->push_back(osg::Vec3(0.0f, 0.0f, startZMinRad));
   }
   // push back the first set of normals
   // pNormals->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));

   // determine the stack and slice delta angles
   float fSliceDelta  = osg::PI * 2.0 / nSlices;
   bool  isFullCircle = true;
   // If not a full circle, drawn slice points must be spaced more to include the center point.
   if (!UtMath::NearlyEqual(m_dStopAngle - m_dStartAngle, 2 * M_PI))
   {
      isFullCircle = false;
      fSliceDelta  = (m_dStopAngle - m_dStartAngle) / (nSlices - 2);
   }
   float fStackDelta = osg::PI / nStacks;

   // create the stack starting angle
   float fStackAngle = fStackDelta;

   // create the local index arrays for the stack and slice lines
   std::vector<unsigned int>              vStackIndices;
   std::vector<unsigned int>              vStackIndicesMinRad;
   std::vector<std::vector<unsigned int>> vSliceIndices;
   std::vector<std::vector<unsigned int>> vSliceIndicesMinRad;

   // resize the slice vector
   vSliceIndices.resize(nSlices);
   vSliceIndicesMinRad.resize(nSlices);

   // create the first set of indices
   for (unsigned int i = 0; i < nSlices; i++)
   {
      // create the sphere indices
      if (startZMinRad != m_dMinimumAltitude || !m_bBoundedMin || !twoSpheres)
      {
         if (i < nSlices - 1 || isFullCircle)
         {
            // draws bottoms without central hole
            pSphereIndices.push_back(ColumnToRowIndexing(0, 0, 1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back((i + 1) % nSlices ? ColumnToRowIndexing(i + 2, 0, 1 + static_cast<int>(twoSpheres)) :
                                                         ColumnToRowIndexing(1, 0, 1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(ColumnToRowIndexing(i + 1, 0, 1 + static_cast<int>(twoSpheres)));
            if (twoSpheres)
            {
               pSphereIndices.push_back(ColumnToRowIndexing(0, 1, 1 + static_cast<int>(twoSpheres)));
               pSphereIndices.push_back(ColumnToRowIndexing(i + 1, 1, 1 + static_cast<int>(twoSpheres)));
               pSphereIndices.push_back((i + 1) % nSlices ?
                                           ColumnToRowIndexing(i + 2, 1, 1 + static_cast<int>(twoSpheres)) :
                                           ColumnToRowIndexing(1, 1, 1 + static_cast<int>(twoSpheres)));
            }
         }
         else if (twoSpheres)
         {
            // draws for bottom gap formed with specified angle
            pSphereIndices.push_back(ColumnToRowIndexing(1, 0, 1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(ColumnToRowIndexing(1, 1, 1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(ColumnToRowIndexing(0, 1, 1 + static_cast<int>(twoSpheres)));

            pSphereIndices.push_back(ColumnToRowIndexing(0, 1, 1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(ColumnToRowIndexing(0, 0, 1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(ColumnToRowIndexing(1, 0, 1 + static_cast<int>(twoSpheres)));

            pSphereIndices.push_back(ColumnToRowIndexing(0, 0, 1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(ColumnToRowIndexing(0, 1, 1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(ColumnToRowIndexing(nSlices, 1, 1 + static_cast<int>(twoSpheres)));

            pSphereIndices.push_back(ColumnToRowIndexing(nSlices, 1, 1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(ColumnToRowIndexing(nSlices, 0, 1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(ColumnToRowIndexing(0, 0, 1 + static_cast<int>(twoSpheres)));
         }
      }
      else
      {
         if (i != 0 || isFullCircle)
         {
            // draws doughnut bottom
            pSphereIndices.push_back(ColumnToRowIndexing(i + 1, 1, 1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(ColumnToRowIndexing(i + 1 % nSlices ? i + 2 : 1, 1, 1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(ColumnToRowIndexing(i + 1 % nSlices ? i + 2 : 1, 0, 1 + static_cast<int>(twoSpheres)));

            pSphereIndices.push_back(ColumnToRowIndexing(i + 1 % nSlices ? i + 2 : 1, 0, 1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(ColumnToRowIndexing(i + 1, 0, 1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(ColumnToRowIndexing(i + 1, 1, 1 + static_cast<int>(twoSpheres)));
         }
      }

      // minimum radius drawn separate if full circle, together if not
      vStackIndices.push_back(ColumnToRowIndexing(i + 1, 0, 1 + static_cast<int>(twoSpheres)));
      if (isFullCircle)
      {
         vStackIndicesMinRad.push_back((i + 1) % nSlices ? ColumnToRowIndexing(i + 2, 0, 1 + static_cast<int>(twoSpheres)) :
                                                           ColumnToRowIndexing(1, 0, 1 + static_cast<int>(twoSpheres)));
      }
      else if (i == nSlices - 1)
      {
         for (int j = nSlices; j >= 1; j--)
         {
            if (twoSpheres)
            {
               vStackIndices.push_back(ColumnToRowIndexing(j, 1, 1 + static_cast<int>(twoSpheres)));
            }
         }
      }
   }

   if (!isFullCircle && !twoSpheres)
   {
      vStackIndices.push_back(ColumnToRowIndexing(0, 0, 1 + static_cast<int>(twoSpheres)));
   }

   // add a geometry primitive for the stack lines
   pStackGeometry->addPrimitiveSet(new osg::DrawElementsUInt(GL_LINE_LOOP, vStackIndices.size(), &vStackIndices[0]));

   if (twoSpheres && isFullCircle)
   {
      pStackGeometryMinRad->addPrimitiveSet(
         new osg::DrawElementsUInt(GL_LINE_LOOP, vStackIndicesMinRad.size(), &vStackIndicesMinRad[0]));
   }

   // loop across all the stacks

   double maxAngle       = std::max(acos(m_dMaximumAltitude / (rRadius * rCFactorZ)), 0.0);
   double minAngle       = std::acos(m_dMinimumAltitude / (rRadius * rCFactorZ));
   double maxAngleMinRad = std::max(acos(m_dMaximumAltitude / (rMinRadius * rCFactorZ)), 0.0);
   double minAngleMinRad = std::acos(m_dMinimumAltitude / (rMinRadius * rCFactorZ));
   if (-m_dMinimumAltitude > m_dRadius)
   {
      minAngle = M_PI;
   }

   for (unsigned int nStack = 1; nStacks > nStack; nStack++)
   {
      // clear the stack indices
      vStackIndices.clear();
      if (isFullCircle)
      {
         vStackIndicesMinRad.clear();
      }

      float stackAngle = fStackAngle;
      if (m_bBoundedMax && fStackAngle > maxAngle)
      {
         stackAngle = maxAngle;
      }
      else if (m_bBoundedMin && fStackAngle < minAngle)
      {
         stackAngle = minAngle;
      }

      float stackAngleMinRad = fStackAngle;
      if (m_bBoundedMax && fStackAngle > maxAngleMinRad)
      {
         stackAngleMinRad = maxAngleMinRad;
      }
      else if (m_bBoundedMin && fStackAngle < minAngleMinRad)
      {
         stackAngleMinRad = minAngleMinRad;
      }

      // calculate the z coordinate
      float fZ       = std::cos(stackAngle) * static_cast<float>(rRadius * rCFactorZ);
      float fZMinRad = std::cos(stackAngleMinRad) * static_cast<float>(rMinRadius * rCFactorZ);

      // create the slice starting angle
      double fSliceAngle = m_dStartAngle;

      // calculate the start index
      unsigned int nStartIndex = ((nStack - 1) * (nSlices + 1)) + 1;

      // loop through all the slices
      for (unsigned int nSlice = 0; nSlices > nSlice; nSlice++)
      {
         // calculate the vertex position
         osg::Vec3 vPos(cos(fSliceAngle) * sin(stackAngle) * (float)(rRadius * rCFactorX),
                        sin(fSliceAngle) * sin(stackAngle) * (float)(rRadius * rCFactorY),
                        fZ);

         osg::Vec3 vPosMinRad(std::cos(fSliceAngle) * std::sin(stackAngleMinRad) *
                                 static_cast<float>(rMinRadius * rCFactorX),
                              std::sin(fSliceAngle) * std::sin(stackAngleMinRad) *
                                 static_cast<float>(rMinRadius * rCFactorY),
                              fZMinRad);


         // calculate the set of x, y, and z coordinates
         pVertices->push_back(vPos);
         if (twoSpheres)
         {
            pVertices->push_back(vPosMinRad);
         }
         // normalize the vector
         vPos.normalize();
         vPosMinRad.normalize();
         // calculate the normals for the vertex
         // pNormals->push_back(vPos);
         // update the slice angle
         if (isFullCircle || nSlice < nSlices - 2)
         {
            fSliceAngle += fSliceDelta;
         }
         // make sure not to create a set of indices for the last vertex
         if (nStack < nStacks - 1)
         {
            if (isFullCircle || nSlice < nSlices - 1)
            {
               pSphereIndices.push_back(ColumnToRowIndexing(nStartIndex + nSlice, 0, 1 + static_cast<int>(twoSpheres)));
               pSphereIndices.push_back(
                  (nSlice + 1) % nSlices ?
                     ColumnToRowIndexing(nStartIndex + nSlice + 1, 0, 1 + static_cast<int>(twoSpheres)) :
                     ColumnToRowIndexing(nStartIndex, 0, 1 + static_cast<int>(twoSpheres)));
               pSphereIndices.push_back(
                  ColumnToRowIndexing(nStartIndex + nSlice + nextLevel, 0, 1 + static_cast<int>(twoSpheres)));

               pSphereIndices.push_back(
                  ColumnToRowIndexing(nStartIndex + nSlice + nextLevel, 0, 1 + static_cast<int>(twoSpheres)));
               pSphereIndices.push_back(
                  (nSlice + 1) % nSlices ?
                     ColumnToRowIndexing(nStartIndex + nSlice + 1, 0, 1 + static_cast<int>(twoSpheres)) :
                     ColumnToRowIndexing(nStartIndex, 0, 1 + static_cast<int>(twoSpheres)));
               pSphereIndices.push_back(
                  (nSlice + 1) % nSlices ?
                     ColumnToRowIndexing(nStartIndex + nSlice + nextLevel + 1, 0, 1 + static_cast<int>(twoSpheres)) :
                     ColumnToRowIndexing(nStartIndex + nextLevel, 0, 1 + static_cast<int>(twoSpheres)));
               if (twoSpheres)
               {
                  // need to be created in oposite direction
                  pSphereIndices.push_back(
                     (nSlice + 1) % nSlices ?
                        ColumnToRowIndexing(nStartIndex + nSlice + nextLevel + 1, 1, 1 + static_cast<int>(twoSpheres)) :
                        ColumnToRowIndexing(nStartIndex + nextLevel, 1, 1 + static_cast<int>(twoSpheres)));
                  pSphereIndices.push_back(
                     (nSlice + 1) % nSlices ?
                        ColumnToRowIndexing(nStartIndex + nSlice + 1, 1, 1 + static_cast<int>(twoSpheres)) :
                        ColumnToRowIndexing(nStartIndex, 1, 1 + static_cast<int>(twoSpheres)));
                  pSphereIndices.push_back(
                     ColumnToRowIndexing(nStartIndex + nSlice + nextLevel, 1, 1 + static_cast<int>(twoSpheres)));
                  pSphereIndices.push_back(
                     ColumnToRowIndexing(nStartIndex + nSlice + nextLevel, 1, 1 + static_cast<int>(twoSpheres)));
                  pSphereIndices.push_back(
                     (nSlice + 1) % nSlices ?
                        ColumnToRowIndexing(nStartIndex + nSlice + 1, 1, 1 + static_cast<int>(twoSpheres)) :
                        ColumnToRowIndexing(nStartIndex, 1, 1 + static_cast<int>(twoSpheres)));
                  pSphereIndices.push_back(ColumnToRowIndexing(nStartIndex + nSlice, 1, 1 + static_cast<int>(twoSpheres)));
               }
            }
            else if (twoSpheres)
            {
               // innerwall due to angle
               pSphereIndices.push_back(ColumnToRowIndexing(nStartIndex, 1, 1 + static_cast<int>(twoSpheres)));
               pSphereIndices.push_back(ColumnToRowIndexing(nStartIndex, 0, 1 + static_cast<int>(twoSpheres)));
               pSphereIndices.push_back(ColumnToRowIndexing(nStartIndex + nextLevel, 1, 1 + static_cast<int>(twoSpheres)));

               pSphereIndices.push_back(ColumnToRowIndexing(nStartIndex + nextLevel, 1, 1 + static_cast<int>(twoSpheres)));
               pSphereIndices.push_back(ColumnToRowIndexing(nStartIndex, 0, 1 + static_cast<int>(twoSpheres)));
               pSphereIndices.push_back(ColumnToRowIndexing(nStartIndex + nextLevel, 0, 1 + static_cast<int>(twoSpheres)));

               // draws inner sphere
               for (int i = nSlices - 2; i >= 0; i--)
               {
                  pSphereIndices.push_back(
                     (i + 1) % nSlices ?
                        ColumnToRowIndexing(nStartIndex + i + nextLevel + 1, 1, 1 + static_cast<int>(twoSpheres)) :
                        ColumnToRowIndexing(nStartIndex + nextLevel, 1, 1 + static_cast<int>(twoSpheres)));
                  pSphereIndices.push_back(
                     (i + 1) % nSlices ? ColumnToRowIndexing(nStartIndex + i + 1, 1, 1 + static_cast<int>(twoSpheres)) :
                                         ColumnToRowIndexing(nStartIndex, 1, 1 + static_cast<int>(twoSpheres)));
                  pSphereIndices.push_back(
                     ColumnToRowIndexing(nStartIndex + i + nextLevel, 1, 1 + static_cast<int>(twoSpheres)));
                  pSphereIndices.push_back(
                     ColumnToRowIndexing(nStartIndex + i + nextLevel, 1, 1 + static_cast<int>(twoSpheres)));
                  pSphereIndices.push_back(
                     (i + 1) % nSlices ? ColumnToRowIndexing(nStartIndex + i + 1, 1, 1 + static_cast<int>(twoSpheres)) :
                                         ColumnToRowIndexing(nStartIndex, 1, 1 + static_cast<int>(twoSpheres)));
                  pSphereIndices.push_back(ColumnToRowIndexing(nStartIndex + i, 1, 1 + static_cast<int>(twoSpheres)));
               }

               // other inner wall
               pSphereIndices.push_back(ColumnToRowIndexing(nStartIndex + nSlice, 0, 1 + static_cast<int>(twoSpheres)));
               pSphereIndices.push_back(ColumnToRowIndexing(nStartIndex + nSlice, 1, 1 + static_cast<int>(twoSpheres)));
               pSphereIndices.push_back(
                  ColumnToRowIndexing(nStartIndex + nextLevel + nSlice, 0, 1 + static_cast<int>(twoSpheres)));

               pSphereIndices.push_back(
                  ColumnToRowIndexing(nStartIndex + nextLevel + nSlice, 0, 1 + static_cast<int>(twoSpheres)));
               pSphereIndices.push_back(ColumnToRowIndexing(nStartIndex + nSlice, 1, 1 + static_cast<int>(twoSpheres)));
               pSphereIndices.push_back(
                  ColumnToRowIndexing(nStartIndex + nextLevel + nSlice, 1, 1 + static_cast<int>(twoSpheres)));
            }
            else
            {
               pSphereIndices.push_back(ColumnToRowIndexing(nStartIndex + nSlices, 0, 1 + static_cast<int>(twoSpheres)));
               pSphereIndices.push_back(
                  ColumnToRowIndexing(nStartIndex + nextLevel + nSlices, 0, 1 + static_cast<int>(twoSpheres)));
               pSphereIndices.push_back(
                  ColumnToRowIndexing(nStartIndex + nSlices - 1, 0, 1 + static_cast<int>(twoSpheres)));

               pSphereIndices.push_back(
                  ColumnToRowIndexing(nStartIndex + nSlices - 1, 0, 1 + static_cast<int>(twoSpheres)));
               pSphereIndices.push_back(
                  ColumnToRowIndexing(nStartIndex + nextLevel + nSlices, 0, 1 + static_cast<int>(twoSpheres)));
               pSphereIndices.push_back(
                  ColumnToRowIndexing(nStartIndex + (nSlices * 2), 0, 1 + static_cast<int>(twoSpheres)));

               //
               pSphereIndices.push_back(ColumnToRowIndexing(nStartIndex, 0, 1 + static_cast<int>(twoSpheres)));
               pSphereIndices.push_back(ColumnToRowIndexing(nStartIndex + nextLevel, 0, 1 + static_cast<int>(twoSpheres)));
               pSphereIndices.push_back(ColumnToRowIndexing(nStartIndex + nSlices, 0, 1 + static_cast<int>(twoSpheres)));

               pSphereIndices.push_back(ColumnToRowIndexing(nStartIndex + nextLevel, 0, 1 + static_cast<int>(twoSpheres)));
               pSphereIndices.push_back(
                  ColumnToRowIndexing(nStartIndex + nextLevel + nSlices, 0, 1 + static_cast<int>(twoSpheres)));
               pSphereIndices.push_back(ColumnToRowIndexing(nStartIndex + nSlices, 0, 1 + static_cast<int>(twoSpheres)));
            }
         }
         // push back the set of indices for the stack lines
         vStackIndices.push_back(ColumnToRowIndexing(nStartIndex + nSlice, 0, 1 + static_cast<int>(twoSpheres)));
         if (isFullCircle && twoSpheres)
         {
            vStackIndicesMinRad.push_back(ColumnToRowIndexing(nStartIndex + nSlice, 1, 1 + static_cast<int>(twoSpheres)));
         }
         else if (nSlice == nSlices - 1)
         {
            if (twoSpheres)
            {
               for (int i = nSlices - 1; i >= 0; i--)
               {
                  vStackIndices.push_back(ColumnToRowIndexing(nStartIndex + i, 1, 1 + static_cast<int>(twoSpheres)));
               }
            }
            else if (!isFullCircle)
            {
               vStackIndices.push_back(ColumnToRowIndexing(nStartIndex + nSlices, 0, 1 + static_cast<int>(twoSpheres)));
            }
         }
         // push back the set of indices for the slice lines
         vSliceIndices.at(nSlice).push_back(ColumnToRowIndexing(nStartIndex + nSlice, 0, 1 + static_cast<int>(twoSpheres)));
         if (twoSpheres)
         {
            vSliceIndicesMinRad.at(nSlice).push_back(
               ColumnToRowIndexing(nStartIndex + nSlice, 1, 1 + static_cast<int>(twoSpheres)));
         }
      }

      if (nStack < nStacks - 1)
      {
         pVertices->push_back(osg::Vec3(0.0f, 0.0f, fZ));
         if (twoSpheres)
         {
            pVertices->push_back(osg::Vec3(0.0f, 0.0f, fZMinRad));
         }
      }
      // update the stack angle
      fStackAngle += fStackDelta;

      // add a geometry primitive for the stack lines
      if (m_bStacksVisible || (nStack == nStacks / 2) || (nStack == nStacks - 1))
      {
         pStackGeometry->addPrimitiveSet(new osg::DrawElementsUInt(GL_LINE_LOOP, vStackIndices.size(), &vStackIndices[0]));
         if (twoSpheres && isFullCircle)
         {
            pStackGeometryMinRad->addPrimitiveSet(
               new osg::DrawElementsUInt(GL_LINE_LOOP, vStackIndicesMinRad.size(), &vStackIndicesMinRad[0]));
         }
      }
   }

   // set the last set of vertices
   pVertices->push_back(osg::Vec3(0.0f, 0.0f, endZ));
   if (twoSpheres)
   {
      pVertices->push_back(osg::Vec3(0.0f, 0.0f, endZMinRad));
   }
   // set the last normal
   // pNormals->push_back(osg::Vec3(0.0f, 0.0f, -1.0f));

   // determine the number of vertices
   unsigned int nVertices = pVertices->size() / 2;
   if (!twoSpheres)
   {
      nVertices = pVertices->size();
   }

   // create the last set of indices
   for (unsigned int i = 0; i < nSlices; i++)
   {
      // create sphere indices
      if ((endZMinRad != m_dMaximumAltitude || !m_bBoundedMax) || !twoSpheres)
      {
         if (isFullCircle || i < nSlices - 1)
         {
            pSphereIndices.push_back(ColumnToRowIndexing(nVertices - nSlices - 1 + i, 0, 1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(
               (i + 1) % nSlices ? ColumnToRowIndexing(nVertices - nSlices + i, 0, 1 + static_cast<int>(twoSpheres)) :
                                   ColumnToRowIndexing(nVertices - nSlices - 1, 0, 1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(ColumnToRowIndexing(nVertices - 1, 0, 1 + static_cast<int>(twoSpheres)));
            if (twoSpheres)
            {
               pSphereIndices.push_back(
                  ColumnToRowIndexing(nVertices - nSlices - 1 + i, 1, 1 + static_cast<int>(twoSpheres)));
               pSphereIndices.push_back(ColumnToRowIndexing(nVertices - 1, 1, 1 + static_cast<int>(twoSpheres)));
               pSphereIndices.push_back(
                  (i + 1) % nSlices ? ColumnToRowIndexing(nVertices - nSlices + i, 1, 1 + static_cast<int>(twoSpheres)) :
                                      ColumnToRowIndexing(nVertices - nSlices - 1, 1, 1 + static_cast<int>(twoSpheres)));
            }
         }
         else if (twoSpheres)
         {
            // top inner wall on angle
            pSphereIndices.push_back(ColumnToRowIndexing(nVertices - 1, 1, 1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(ColumnToRowIndexing(nVertices - 1, 0, 1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(ColumnToRowIndexing(nVertices - 2, 0, 1 + static_cast<int>(twoSpheres)));

            pSphereIndices.push_back(ColumnToRowIndexing(nVertices - 2, 0, 1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(ColumnToRowIndexing(nVertices - 2, 1, 1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(ColumnToRowIndexing(nVertices - 1, 1, 1 + static_cast<int>(twoSpheres)));

            pSphereIndices.push_back(ColumnToRowIndexing(nVertices - nSlices - 1, 1, 1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(ColumnToRowIndexing(nVertices - nSlices - 1, 0, 1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(ColumnToRowIndexing(nVertices - 1, 0, 1 + static_cast<int>(twoSpheres)));

            pSphereIndices.push_back(ColumnToRowIndexing(nVertices - 1, 0, 1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(ColumnToRowIndexing(nVertices - 1, 1, 1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(ColumnToRowIndexing(nVertices - nSlices - 1, 1, 1 + static_cast<int>(twoSpheres)));
         }
      }
      else
      {
         if (i < nSlices - 1 || isFullCircle)
         {
            // upper side doughnut
            pSphereIndices.push_back(ColumnToRowIndexing(nVertices - 1 - nSlices + i, 1, 1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(ColumnToRowIndexing(nVertices - 1 - nSlices + i, 0, 1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(ColumnToRowIndexing((i + 1) % nSlices ? nVertices - nSlices + i : nVertices - 1 - nSlices,
                                                         0,
                                                         1 + static_cast<int>(twoSpheres)));

            pSphereIndices.push_back(ColumnToRowIndexing((i + 1) % nSlices ? nVertices - nSlices + i : nVertices - 1 - nSlices,
                                                         0,
                                                         1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(ColumnToRowIndexing((i + 1) % nSlices ? nVertices - nSlices + i : nVertices - 1 - nSlices,
                                                         1,
                                                         1 + static_cast<int>(twoSpheres)));
            pSphereIndices.push_back(ColumnToRowIndexing(nVertices - 1 - nSlices + i, 1, 1 + static_cast<int>(twoSpheres)));
         }
      }

      if (m_bSlicesVisible || (i == 0) || (i == nSlices / 4) || (i == nSlices / 2) || (i == 3 * nSlices / 4))
      {
         // add a geometry primitive for the slice lines
         pSliceGeometry->addPrimitiveSet(
            new osg::DrawElementsUInt(GL_LINE_STRIP, vSliceIndices.at(i).size(), &vSliceIndices.at(i)[0]));
         if (twoSpheres)
         {
            pSliceGeometryMinRad->addPrimitiveSet(
               new osg::DrawElementsUInt(GL_LINE_STRIP, vSliceIndicesMinRad.at(i).size(), &vSliceIndicesMinRad.at(i)[0]));
         }
      }
   }
   // create a geode for the sphere
   osg::Geode* pSphereGeode = new osg::Geode;
   // create the geometry for the geode
   osg::Geometry* pSphereGeometry = new osg::Geometry;
   // set the attributes for the geometry unit
   pSphereGeometry->setVertexArray(pVertices);
   // allow the lines to have color
   pSphereColor = new osg::Vec4Array;
   // attach the color attributes
   pSphereColor->push_back(
      osg::Vec4(m_oBlendColor.RedF(), m_oBlendColor.GreenF(), m_oBlendColor.BlueF(), m_oBlendColor.AlphaF()));
   pSphereGeometry->setColorArray(pSphereColor);
   pSphereGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
   // add the primitive data type to the geometry
   pSphereGeometry->addPrimitiveSet(new osg::DrawElementsUInt(GL_TRIANGLES, pSphereIndices.size(), &pSphereIndices[0])); //****
   // attach the geometry to the geode
   pSphereGeode->addDrawable(pSphereGeometry);

   // for the sphere geometry, turn off depth buffer writting
   pSphereGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::LEQUAL, 0.0, 1.0, false));
   pSphereGeometry->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
   pSphereGeometry->getOrCreateStateSet()->setNestRenderBins(false);

   // insert the sphere geometry into the vector
   m_vSphereGeometry.push_back(pSphereGeometry);

   // enable / disable blending on the node
   if (m_oBlendColor.HasTransparency() != 0)
      UtoGeometryUtil::EnableBlend(pSphereGeode);
   else
      UtoGeometryUtil::DisableBlend(pSphereGeode);

   // setup names for the corresponding nodes
   pSphereGeode->setName("Sphere Geode");
   pSliceGeometry->setName("Slice Geometry");
   pSliceGeometryMinRad->setName("Inner Radius Slice Geometry");
   pStackGeometry->setName("Stack Geometry");
   pSphereGeometry->setName("Sphere Geometry");

   return pSphereGeode;
}

void UtoSphereShapeImp::EnableLighting(bool aState)
{
   mLighting = aState;

   if (m_pLODRoot)
   {
      m_pLODRoot->getOrCreateStateSet()->setMode(GL_LIGHTING, aState ? osg::StateAttribute::ON : osg::StateAttribute::OFF);
   }
}

int UtoSphereShapeImp::ColumnToRowIndexing(int aRowPos, int aColumnPos, int aColumnSize) const
{
   if (aColumnPos >= aColumnSize)
   {
      return -1;
   }
   return (aRowPos * aColumnSize) + aColumnPos;
}
