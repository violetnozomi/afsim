// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfAttachmentSensorVolume.hpp"

#include <QColor>
#include <osg/BlendFunc>
#include <osg/CullFace>
#include <osg/Depth>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/PolygonMode>
#include <osg/Stencil>
#include <osgUtil/Tessellator>

#include "UtCast.hpp"
#include "UtoRawShape.hpp"
#include "UtoShaders.hpp"
#include "VaEntity.hpp"
#include "VaViewer.hpp"
#include "WkfObserver.hpp"

float wkf::SensorBeam::msRangeDivisionRate = 370400;  // 200 nautical miles
float wkf::SensorBeam::msCapDivisionRate   = 0.0087F; // ~ 0.5 degrees
float wkf::SensorBeam::msMaxRange          = 6.0E8F;  // beyond the moon

wkf::AttachmentSensorVolume::AttachmentSensorVolume(vespa::VaEntity&   aEntity,
                                                    vespa::VaViewer*   aViewerPtr,
                                                    const std::string& aName)
   : vespa::VaAttachment(aEntity, aViewerPtr, "WkfAttachmentSensorVolume")
   , mModeName("")
   , mShapePtr(nullptr)
{
   SetType<wkf::AttachmentSensorVolume>();
   UtoRawShape rawShape;
   mShapePtr   = dynamic_cast<UtoRawShape*>(AddShapeToParent(aName, rawShape));
   mProjection = aViewerPtr->GetMapProjection();

   mPAT = new osg::MatrixTransform;
   mShapePtr->GetRoot()->addChild(mPAT);
   ConfigureForMapProjection();

   mCallbacks.Add(wkf::Observer::MapProjectionChanged.Connect(&AttachmentSensorVolume::MapProjectionChangedCB, this));
   mCallbacks.Add(
      vespa::VaObserver::EntityMoved.Connect(&AttachmentSensorVolume::EntityMovedCB, this, GetParent().GetUniqueId()));
}

void wkf::AttachmentSensorVolume::SetDisplayData(bool          aShowFaces,
                                                 const QColor& aFaceColor,
                                                 bool          aShowEdges,
                                                 const QColor& aEdgeColor,
                                                 bool          aShowProjections,
                                                 const QColor& aProjectionColor)
{
   mShowFaces       = aShowFaces;
   mShowEdges       = aShowEdges;
   mShowProjections = aShowProjections;
   mFaceColor       = osg::Vec4(aFaceColor.redF(), aFaceColor.greenF(), aFaceColor.blueF(), aFaceColor.alphaF());
   mEdgeColor       = osg::Vec4(aEdgeColor.redF(), aEdgeColor.greenF(), aEdgeColor.blueF(), aEdgeColor.alphaF());
   mProjectionColor =
      osg::Vec4(aProjectionColor.redF(), aProjectionColor.greenF(), aProjectionColor.blueF(), aProjectionColor.alphaF());

   for (auto& it : mBeams)
   {
      it.SetShapeColor(mFaceColor);
      it.SetLineColor(mEdgeColor);
      it.SetProjectionColor(mProjectionColor);
      it.SetProjectionEnabled(aShowProjections);
      it.SetFacesEnabled(aShowFaces);
      it.SetEdgesEnabled(aShowEdges);
   }
}

void wkf::AttachmentSensorVolume::MapProjectionChangedCB(unsigned int aViewerId, const UtoMapProjection* aProjection)
{
   if (aViewerId == GetViewer()->GetUniqueId())
   {
      mProjection = *aProjection;
      ConfigureForMapProjection();
   }
}

void wkf::AttachmentSensorVolume::EntityMovedCB(vespa::VaEntity* aEntityPtr, bool aMoving)
{
   // build out matrix and push it out to the node
   osg::Matrixf mat = aEntityPtr->ModelToWorldMatrix();
   mat.preMult(mPAT->getMatrix());
   mPAT->getOrCreateStateSet()->addUniform(new osg::Uniform("ecefMatrix", mat));
}

void wkf::AttachmentSensorVolume::ConfigureForMapProjection()
{
   if (mProjection.projection == UtoMapProjection::Geocentric)
   {
      mShapePtr->SetReferenceFrame(UtoShape::cFRAME_ENTITY);
      osg::Program* programPtr = UtoShaders::UseProgram("emitterVolume.vert", "emitterVolume.frag");
      for (auto&& it : mBeams)
      {
         it.SetShaders(programPtr, programPtr);
         it.UseFlatStateSet(false);
      }
   }
   else
   {
      mShapePtr->SetReferenceFrame(UtoShape::cFRAME_ENTITY);
      osg::Program* lprogramPtr =
         UtoShaders::UseProgramWithGeometry("emitterVolumeFlatLines.geom", "emitterVolumeFlat.vert", "emitterVolume.frag");
      osg::Program* tprogramPtr =
         UtoShaders::UseProgramWithGeometry("emitterVolumeFlatTris.geom", "emitterVolumeFlat.vert", "emitterVolume.frag");
      mPAT->getOrCreateStateSet()->setAttributeAndModes(lprogramPtr, true);
      for (auto&& it : mBeams)
      {
         it.SetShaders(lprogramPtr, tprogramPtr);
         it.UseFlatStateSet(true);
      }
   }
}

void wkf::AttachmentSensorVolume::AddBeam()
{
   mBeams.emplace_back(mPAT, this);
   ConfigureForMapProjection();
}

void wkf::AttachmentSensorVolume::Articulate(const double aSlew[3], const double aCue[3], const double aTranslate[3])
{
   osg::Matrix partMat;
   partMat.setRotate(osg::Quat(aCue[2], osg::Vec3f(1, 0, 0), aCue[1], osg::Vec3f(0, 1, 0), aCue[0], osg::Vec3f(0, 0, 1)));
   partMat.postMultRotate(
      osg::Quat(aSlew[2], osg::Vec3f(1, 0, 0), aSlew[1], osg::Vec3f(0, 1, 0), aSlew[0], osg::Vec3f(0, 0, 1)));
   partMat.setTrans(aTranslate[0], aTranslate[1], aTranslate[2]);

   mPAT->setMatrix(partMat);
}

wkf::SensorBeam::SensorBeam(osg::MatrixTransform* aParentNode, AttachmentSensorVolume* aParentAttachment)
{
   mLocalPAT = new osg::MatrixTransform();
   aParentNode->addChild(mLocalPAT);

   osg::ref_ptr<osg::Group> standard = new osg::Group;
   mProjFace                         = new osg::Group();
   mProjFill                         = new osg::Group();

   mLocalPAT->addChild(standard);
   mLocalPAT->addChild(mProjFace);
   mLocalPAT->addChild(mProjFill);

   mGeode = new osg::Geode();
   mGeode->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc());
   mProjectionGeode = new osg::Geode();
   mProjectionGeode->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc());
   mLinesGeode = new osg::Geode();
   mLinesGeode->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc());
   mGridGeode = new osg::Geode();
   mGridGeode->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc());

   standard->addChild(mGeode);
   standard->addChild(mLinesGeode);
   standard->addChild(mGridGeode);
   standard->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::LESS, 0.0, 1.0, false));
   standard->getOrCreateStateSet()->setRenderBinDetails(0, "DepthSortedBin");

   mProjFace->addChild(mProjectionGeode);
   mProjFace->getOrCreateStateSet()->setRenderBinDetails(0, "TraversalOrderBin");
   mProjFace->getOrCreateStateSet()->setAttributeAndModes(new osg::ColorMask(false, false, false, false));
   mProjFace->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::LESS, 0.0, 1.0, false));
   osg::ref_ptr<osg::Stencil> stencilFace = new osg::Stencil();
   stencilFace->setFunction(osg::Stencil::ALWAYS, 0, ~0);
   stencilFace->setOperation(osg::Stencil::KEEP, osg::Stencil::KEEP, osg::Stencil::INVERT);
   mProjFace->getOrCreateStateSet()->setAttributeAndModes(stencilFace);

   mProjFill->addChild(mProjectionGeode);
   mProjFill->getOrCreateStateSet()->setRenderBinDetails(0, "TraversalOrderBin");
   mProjFill->getOrCreateStateSet()->setAttributeAndModes(new osg::CullFace(osg::CullFace::BACK));
   mProjFill->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS, 0.0, 1.0, false));
   osg::ref_ptr<osg::Stencil> stencilFill = new osg::Stencil();
   stencilFill->setFunction(osg::Stencil::NOTEQUAL, 0, ~0);
   stencilFill->setOperation(osg::Stencil::ZERO, osg::Stencil::ZERO, osg::Stencil::ZERO);
   mProjFill->getOrCreateStateSet()->setAttributeAndModes(stencilFill);
}

void wkf::SensorBeam::RebuildPolygonal(float aTilt, float aMinR, float aMaxR, std::vector<std::pair<float, float>>& aPoints)
{
   // NOTE: I think there is a lot of work here to do to get our volumes to match
   // to AFSIM.  Tilt isn't working right.  We can re-add when we are ready to
   // get everything correct.
   // mLocalPAT->setAttitude(osg::Quat(aTilt, osg::Vec3f(0, 1, 0)));
   float maxR           = std::min(msMaxRange, aMaxR);
   int   rangeDivisions = (int)(maxR / msRangeDivisionRate) + 1;
   RebuildDrawables();

   int i;
   int j;
   int wallFaces = aPoints.size();

   // in the following, we calculate the winding of the polygon, so that we can reorient it as necessary
   // since graphics cares about this, but AFSIM does not.
   float sum = 0;
   for (int i = 0; i < wallFaces; ++i)
   {
      int ip1 = (i + 1) % wallFaces;
      sum += (aPoints[ip1].first - aPoints[i].first) * (aPoints[ip1].second + aPoints[i].second);
   }
   int start = 0;
   int end   = wallFaces - 1;
   int incr  = 1;
   if (sum > 0)
   {
      start = wallFaces - 1;
      end   = 0;
      incr  = -1;
   }

   auto* pGeomShape = static_cast<osg::Geometry*>(mGeode->getDrawable(0));
   auto* pGeomLines = static_cast<osg::Geometry*>(mLinesGeode->getDrawable(mLineShapeIdx));

   // find the vertex buffer
   auto* pVertices = static_cast<osg::Vec3Array*>(pGeomShape->getVertexArray());

   if (!pVertices)
   {
      // create and bind a new vertex buffer
      pVertices = new osg::Vec3Array;
      pGeomShape->setVertexArray(pVertices);
      pGeomLines->setVertexArray(pVertices);
   }

   // set the new size of the vertex buffer
   unsigned int nVertSize = wallFaces * rangeDivisions + 1; // DONE, updated for range division

   pVertices->resize(nVertSize);

   // obtain an iterator to the first vertex
   auto itVertex = pVertices->begin();

   pGeomShape->removePrimitiveSet(0, pGeomShape->getNumPrimitiveSets());
   pGeomLines->removePrimitiveSet(0, pGeomLines->getNumPrimitiveSets());

   int farTrack = 0;
   int idx      = 0;
   for (j = 0; j < rangeDivisions; ++j)
   {
      float fRange = (j + 1) * msRangeDivisionRate;
      fRange       = (fRange > maxR) ? maxR : fRange;

      osg::DrawElementsUInt* faces;
      if (j == 0)
      {
         faces = new osg::DrawElementsUInt(GL_TRIANGLE_FAN);
         faces->push_back(nVertSize - 1);
      }
      else
      {
         faces = new osg::DrawElementsUInt(GL_TRIANGLE_STRIP);
      }
      pGeomShape->addPrimitiveSet(faces);
      osg::DrawElementsUInt* loop = new osg::DrawElementsUInt(GL_LINE_LOOP);
      pGeomLines->addPrimitiveSet(loop);
      farTrack = idx;
      for (i = start; i != end + incr; i += incr)
      {
         float az = aPoints[i].first;
         float el = aPoints[i].second;

         float     fCosEl = cos(el);
         float     fX     = fRange * fCosEl * cos(az);
         float     fY     = fRange * fCosEl * sin(az);
         float     fZ     = fRange * sin(el);
         osg::Vec3 xyz(fX, fY, -fZ);
         itVertex->set(xyz);
         ++itVertex;
         loop->push_back(idx);
         if (j != 0)
         {
            faces->push_back(idx - wallFaces);
         }
         faces->push_back(idx);
         ++idx;
      }
      if (j != 0)
      {
         faces->push_back(idx - wallFaces * 2);
      }
      faces->push_back(idx - wallFaces);
   }
   for (i = end; i != start - incr; i -= incr)
   {
      idx                         = i;
      osg::DrawElementsUInt* edge = new osg::DrawElementsUInt(GL_LINE_STRIP);
      pGeomLines->addPrimitiveSet(edge);
      edge->push_back(nVertSize - 1);
      for (j = 0; j < rangeDivisions; ++j)
      {
         edge->push_back(idx);
         idx += wallFaces;
      }
   }

   osgUtil::Tessellator tess;
   tess.beginTessellation();
   tess.beginContour();
   for (i = wallFaces - 1; i >= 0; --i)
   {
      tess.addVertex(&(pVertices->at(farTrack + i)));
   }
   tess.endContour();
   tess.endTessellation();
   auto pl = tess.getPrimList();

   // Build the far face
   int pp = pGeomShape->getNumPrimitiveSets();
   int ii = farTrack + wallFaces;
   for (auto pit : pl)
   {
      pGeomShape->addPrimitiveSet(new osg::DrawElementsUInt(pit->_mode));

      const osgUtil::Tessellator::Prim::VecList& vecs = pit->_vertices;

      osg::DrawElementsUInt* primSet = static_cast<osg::DrawElementsUInt*>(pGeomShape->getPrimitiveSet(pp));
      primSet->resize(vecs.size());
      osg::DrawElementsUInt::iterator itPrim = primSet->begin();

      for (auto vit : vecs)
      {
         *itPrim = vit - &pVertices->front();
         ++itPrim;
         ++ii;
      }
      ++pp;
   }

   itVertex->set(0.0f, 0.0f, 0.0f);

   // force an update to the shape
   pGeomShape->dirtyBound();
   pGeomLines->dirtyBound();
}

void wkf::SensorBeam::RebuildCircular(float aTilt, float aMinR, float aMaxR, float aHalfAngle)
{
   int circleSampling = 64;
   // NOTE: I think there is a lot of work here to do to get our volumes to match
   // to AFSIM.  Tilt isn't working right.  We can re-add when we are ready to
   // get everything correct.
   //   mLocalPAT->setAttitude(osg::Quat(aTilt, osg::Vec3f(0, 1, 0)));

   float maxR           = std::min(msMaxRange, aMaxR);
   int   rangeDivisions = (int)(maxR / msRangeDivisionRate) + 1;
   int   capDivisions   = (int)(aHalfAngle / msCapDivisionRate) + 1;
   RebuildDrawables();

   int i;
   int j;

   auto* pGeomShape = static_cast<osg::Geometry*>(mGeode->getDrawable(0));
   auto* pGeomLines = static_cast<osg::Geometry*>(mLinesGeode->getDrawable(mLineShapeIdx));

   // find the vertex buffer
   auto* pVertices = static_cast<osg::Vec3Array*>(pGeomShape->getVertexArray());

   if (!pVertices)
   {
      // create and bind a new vertex buffer
      pVertices = new osg::Vec3Array;
      pGeomShape->setVertexArray(pVertices);
      pGeomLines->setVertexArray(pVertices);
   }

   // set the new size of the vertex buffer
   unsigned int nVertSize = circleSampling * (rangeDivisions + capDivisions) + 2; // DONE, updated for range division

   pVertices->resize(nVertSize);

   // obtain an iterator to the first vertex
   auto itVertex = pVertices->begin();

   // obtain indexed geometry for the shape itself
   if ((int)(pGeomShape->getNumPrimitiveSets()) != rangeDivisions + capDivisions) // DONE, updated for range division
   {
      pGeomShape->removePrimitiveSet(0, pGeomShape->getNumPrimitiveSets());
      pGeomShape->addPrimitiveSet(new osg::DrawElementsUInt(GL_TRIANGLE_FAN)); // DONE, updated for range division
      for (i = 1; i < rangeDivisions; ++i)
      {
         pGeomShape->addPrimitiveSet(new osg::DrawElementsUInt(GL_TRIANGLE_STRIP)); // DONE, updated for range division
      }
      for (i = 0; i < capDivisions - 1; ++i)
      {
         pGeomShape->addPrimitiveSet(new osg::DrawElementsUInt(GL_TRIANGLE_STRIP));
      }
      pGeomShape->addPrimitiveSet(new osg::DrawElementsUInt(GL_TRIANGLE_FAN));
   }

   std::vector<osg::DrawElementsUInt*> pPrimSetSWall;
   for (i = 0; i < rangeDivisions; ++i)
   {
      pPrimSetSWall.push_back(
         static_cast<osg::DrawElementsUInt*>(pGeomShape->getPrimitiveSet(i))); // DONE, updated for range divisions
   }
   std::vector<osg::DrawElementsUInt*> pPrimCapDiv;
   for (i = 0; i < capDivisions - 1; ++i)
   {
      pPrimCapDiv.push_back(static_cast<osg::DrawElementsUInt*>(pGeomShape->getPrimitiveSet(rangeDivisions + i)));
   }
   auto* pPrimSetSCap = static_cast<osg::DrawElementsUInt*>(
      pGeomShape->getPrimitiveSet(rangeDivisions + capDivisions - 1)); // DONE, updated for range divisions

   // resize the vectors
   pPrimSetSWall[0]->resize(circleSampling + 1); // back faces
   for (i = 0; i < rangeDivisions; ++i)
   {
      pPrimSetSWall[i]->resize(circleSampling * 2); // back faces
   }
   for (i = 0; i < capDivisions - 1; ++i)
   {
      pPrimCapDiv[i]->resize(circleSampling * 2);
   }
   pPrimSetSCap->resize(circleSampling + 1); // cap back faces

   // obtain the iterators for the shape
   std::vector<osg::DrawElementsUInt::iterator> itSIT;
   for (i = 0; i < rangeDivisions; ++i)
   {
      itSIT.push_back(pPrimSetSWall[i]->begin()); // again, this is an array that will need to be filled
   }
   std::vector<osg::DrawElementsUInt::iterator> itCapDiv;
   for (i = 0; i < capDivisions - 1; ++i)
   {
      itCapDiv.push_back(pPrimCapDiv[i]->begin());
   }
   auto itSIB = pPrimSetSCap->begin();

   // make the first index reference the last vertex element
   unsigned int nLastIndex = nVertSize - 1;

   *itSIT[0] = nLastIndex - 1;
   ++itSIT[0];
   *itSIB = nLastIndex;
   ++itSIB;

   // obtain the indexed geometry for the shape lines
   if ((int)(pGeomLines->getNumPrimitiveSets()) != rangeDivisions + 4)
   {
      pGeomLines->removePrimitiveSet(0, pGeomLines->getNumPrimitiveSets());
      for (i = 0; i < rangeDivisions; ++i)
      {
         pGeomLines->addPrimitiveSet(new osg::DrawElementsUInt(GL_LINE_LOOP));
      }
      pGeomLines->addPrimitiveSet(new osg::DrawElementsUInt(GL_LINE_STRIP));
      pGeomLines->addPrimitiveSet(new osg::DrawElementsUInt(GL_LINE_STRIP));
      pGeomLines->addPrimitiveSet(new osg::DrawElementsUInt(GL_LINE_STRIP));
      pGeomLines->addPrimitiveSet(new osg::DrawElementsUInt(GL_LINE_STRIP));
   }

   std::vector<osg::DrawElementsUInt*>          pPrimSetLines;
   std::vector<osg::DrawElementsUInt::iterator> itLIT;

   auto* pPrimSetLinesUp = static_cast<osg::DrawElementsUInt*>(pGeomLines->getPrimitiveSet(rangeDivisions));
   pPrimSetLinesUp->resize(rangeDivisions + 1);
   auto* pPrimSetLinesDn = static_cast<osg::DrawElementsUInt*>(pGeomLines->getPrimitiveSet(rangeDivisions + 1));
   pPrimSetLinesDn->resize(rangeDivisions + 1);
   auto* pPrimSetLinesLf = static_cast<osg::DrawElementsUInt*>(pGeomLines->getPrimitiveSet(rangeDivisions + 2));
   pPrimSetLinesLf->resize(rangeDivisions + 1);
   auto* pPrimSetLinesRt = static_cast<osg::DrawElementsUInt*>(pGeomLines->getPrimitiveSet(rangeDivisions + 3));
   pPrimSetLinesRt->resize(rangeDivisions + 1);
   auto itLUP = pPrimSetLinesUp->begin();
   auto itLDN = pPrimSetLinesDn->begin();
   auto itLLF = pPrimSetLinesLf->begin();
   auto itLRT = pPrimSetLinesRt->begin();
   *itLUP     = nLastIndex - 1;
   ++itLUP;
   *itLDN = nLastIndex - 1;
   ++itLDN;
   *itLLF = nLastIndex - 1;
   ++itLLF;
   *itLRT = nLastIndex - 1;
   ++itLRT;
   for (i = 0; i < rangeDivisions; ++i)
   {
      pPrimSetLines.push_back(static_cast<osg::DrawElementsUInt*>(pGeomLines->getPrimitiveSet(i)));
      pPrimSetLines[i]->resize(circleSampling);
      itLIT.push_back(pPrimSetLines[i]->begin());
   }

   // spherical coordinates that will be used:
   // x = R * cos( elevation ) * sin( azimuth )
   // y = R * cos( elevation ) * cos( azimuth )
   // z = R * sin( elevation )
   float fRange = maxR;
   bool  ldone[4];

   for (j = 0; j < rangeDivisions; ++j)
   {
      ldone[0] = ldone[1] = ldone[2] = ldone[3] = false;
      fRange                                    = (j + 1) * msRangeDivisionRate;
      fRange                                    = (fRange > maxR) ? maxR : fRange;
      for (i = 0; i < circleSampling; ++i)
      {
         int   idx = i + j * circleSampling;
         float fi  = 1 - i / (float)(circleSampling - 1);

         float circumAng =
            2 * osg::PI * fi; // this is an angle that traces around the sensors direction, called circumscription angle
         float sAng       = sin(circumAng);
         float cAng       = cos(circumAng);
         float angleAtAng = aHalfAngle * aHalfAngle /
                            sqrt(aHalfAngle * aHalfAngle * sAng * sAng + aHalfAngle * aHalfAngle * cAng * cAng);
         float sAngAng = sin(angleAtAng);

         float fX = fRange * cos(angleAtAng);
         float fY = fRange * sAngAng * sAng;
         float fZ = fRange * sAngAng * cAng;

         osg::Vec3 xyz(fX, fY, fZ);
         itVertex->set(xyz);
         ++itVertex;
         auto idxSafe = ut::safe_cast<GLuint, int>(idx);

         // add itLUP, itLDN, itLLF, itLRT
         if ((fi >= 0) && (!ldone[0]))
         {
            ldone[0] = true;
            *itLUP   = idxSafe;
            ++itLUP;
         }
         else if ((fi <= 0.75) && (!ldone[1]))
         {
            ldone[1] = true;
            *itLLF   = idxSafe;
            ++itLLF;
         }
         else if ((fi <= 0.5) && (!ldone[2]))
         {
            *itLRT = idxSafe;
            ++itLRT;
            ldone[2] = true;
         }
         else if ((fi <= 0.25) && (!ldone[3]))
         {
            *itLDN = idxSafe;
            ++itLDN;
            ldone[3] = true;
         }


         *itLIT[j] = idxSafe;
         itLIT[j]++;
         if (j == 0)
         {
            *itSIT[j] = ut::safe_cast<GLuint, int>(circleSampling - 1 - idx);
            itSIT[j]++;
         }
         if (j != 0)
         {
            *itSIT[j] = idxSafe;
            itSIT[j]++;
            *itSIT[j] = ut::safe_cast<GLuint, int>(idx - circleSampling);
            itSIT[j]++;
         }
      }
   }
   float angle = aHalfAngle;
   for (j = 0; j < capDivisions; ++j)
   {
      angle = angle < 0.0F ? 0.0F : angle;
      for (i = 0; i < circleSampling; ++i)
      {
         int   idx = rangeDivisions * circleSampling + i + j * circleSampling;
         float fi  = 1 - i / (float)(circleSampling - 1);

         float circumAng =
            2 * osg::PI * fi; // this is an angle that traces around the sensors direction, called circumscription angle
         float sAng       = sin(circumAng);
         float cAng       = cos(circumAng);
         float angleAtAng = angle * angle / sqrt(angle * angle * (sAng * sAng + cAng * cAng));
         float sAngAng    = sin(angleAtAng);

         float fX = maxR * cos(angleAtAng);
         float fY = maxR * sAngAng * sAng;
         float fZ = maxR * sAngAng * cAng;

         itVertex->set(fX, fY, fZ);
         ++itVertex;

         if (j > 0)
         {
            *itCapDiv[j - 1] = ut::safe_cast<GLuint, int>(idx);
            itCapDiv[j - 1]++;
            *itCapDiv[j - 1] = ut::safe_cast<GLuint, int>(idx - circleSampling);
            itCapDiv[j - 1]++;
         }
         if (j == capDivisions - 1)
         {
            *itSIB = ut::safe_cast<GLuint, int>(idx);
            itSIB++;
         }
      }
      angle -= msCapDivisionRate;
   }

   // insert the center point
   itVertex->set(0.0f, 0.0f, 0.0f);
   ++itVertex;
   itVertex->set(fRange, 0.0f, 0.0f);

   // remove any sections that do not belong
   if ((int)(pGeomShape->getNumPrimitiveSets()) != rangeDivisions + capDivisions)
   {
      pGeomShape->getPrimitiveSetList().resize(rangeDivisions + capDivisions);
   }

   // force an update to the shape
   pGeomShape->dirtyBound();
   pGeomLines->dirtyBound();
}

void wkf::SensorBeam::RebuildRectangular(float aTilt, float aMinR, float aMaxR, float aMinAz, float aMinEl, float aMaxAz, float aMaxEl)
{
   // NOTE: I think there is a lot of work here to do to get our volumes to match
   // to AFSIM.  Tilt isn't working right.  We can re-add when we are ready to
   // get everything correct.
   //   mLocalPAT->setAttitude(osg::Quat(aTilt, osg::Vec3f(0, 1, 0)));
   float maxR           = std::min(msMaxRange, aMaxR);
   int   rangeDivisions = (int)(maxR / msRangeDivisionRate) + 1;
   RebuildDrawables();

   float azWidth = aMaxAz - aMinAz;
   bool fullAz = ((360 - osg::RadiansToDegrees(azWidth)) * (360 - osg::RadiansToDegrees(azWidth)) < 0.0001) ? true : false;
   if (fullAz)
   {
      BuildFullAzShape(aMinR, maxR, aMinEl, aMaxEl);
      return;
   }
   // if we are within a hundredth of a degree of full 360, consider it full 360
   float elWidth = aMaxEl - aMinEl;
   if ((azWidth == 0) || (elWidth == 0))
   {
      return; // there is nothing to draw
   }

   float sampling;
   float fRange = maxR;
   if ((azWidth < osg::DegreesToRadians(1.0)) && (elWidth < osg::DegreesToRadians(1.0)))
   {
      sampling = osg::DegreesToRadians(0.01);
   }
   else if ((azWidth < osg::DegreesToRadians(10.0)) && (elWidth < osg::DegreesToRadians(10.0)))
   {
      sampling = osg::DegreesToRadians(0.1);
   }
   else
   {
      sampling = osg::DegreesToRadians(1.0);
   }
   unsigned int numStepsAz = (int)(0.5 * azWidth / sampling) * 2 + 1;
   unsigned int numStepsEl = (int)(0.5 * elWidth / sampling) * 2 + 1;
   numStepsAz += 1;
   numStepsEl += 1;
   int i;
   int j;

   auto* pGeomShape  = static_cast<osg::Geometry*>(mGeode->getDrawable(0));
   auto* pGeomLines  = static_cast<osg::Geometry*>(mLinesGeode->getDrawable(mLineShapeIdx));
   auto* pGeomShape2 = static_cast<osg::Geometry*>(mGridGeode->getDrawable(mGridShapeIdx));

   // find the vertex buffer
   auto* pVertices = static_cast<osg::Vec3Array*>(pGeomShape->getVertexArray());

   if (!pVertices)
   {
      // create and bind a new vertex buffer
      pVertices = new osg::Vec3Array;
      pGeomShape->setVertexArray(pVertices);
      pGeomLines->setVertexArray(pVertices);
      pGeomShape2->setVertexArray(pVertices);
   }

   // set the new size of the vertex buffer
   unsigned int nVertSize = (numStepsEl + 1) * (numStepsAz + 1) +             // the cap
                            1 +                                               // the emission point
                            rangeDivisions * (2 * (numStepsAz + numStepsEl)); // intermediate divisions

   pVertices->resize(nVertSize);

   // obtain an iterator to the first vertex
   auto itVertex = pVertices->begin();

   if (pGeomShape->getNumPrimitiveSets() != rangeDivisions + numStepsEl)
   {
      pGeomShape->removePrimitiveSet(0, pGeomShape->getNumPrimitiveSets()); // clear it out
      pGeomShape2->removePrimitiveSet(0, pGeomShape2->getNumPrimitiveSets());
      pGeomShape->addPrimitiveSet(new osg::DrawElementsUInt(GL_TRIANGLE_FAN)); // first step out from range
      for (i = 1; i < rangeDivisions; ++i)
      {
         pGeomShape->addPrimitiveSet(new osg::DrawElementsUInt(GL_TRIANGLE_STRIP)); // every additional range increment
      }
      for (i = 0; i < (int)(numStepsEl); ++i)
      {
         pGeomShape->addPrimitiveSet(new osg::DrawElementsUInt(GL_QUAD_STRIP)); // elevation increments on the cap
         pGeomShape2->addPrimitiveSet(new osg::DrawElementsUInt(GL_QUAD_STRIP));
      }
   }
   std::vector<osg::DrawElementsUInt*> pPrimSetSSides;
   std::vector<osg::DrawElementsUInt*> pPrimSetSCap;
   std::vector<osg::DrawElementsUInt*> pPrimSetSCap2;

   std::vector<osg::DrawElementsUInt::iterator> itSIS;
   std::vector<osg::DrawElementsUInt::iterator> itSIC;
   std::vector<osg::DrawElementsUInt::iterator> itSIC2;

   pPrimSetSSides.push_back(static_cast<osg::DrawElementsUInt*>(pGeomShape->getPrimitiveSet(0))); // triangle fan
   pPrimSetSSides[0]->resize(2 * (numStepsAz + numStepsEl) + 1 + 1); // one to close, and one for the start point
   itSIS.push_back(pPrimSetSSides[0]->begin());
   for (i = 1; i < rangeDivisions; ++i)
   {
      pPrimSetSSides.push_back(static_cast<osg::DrawElementsUInt*>(pGeomShape->getPrimitiveSet(i)));
      pPrimSetSSides[i]->resize(4 * (numStepsAz + numStepsEl) + 2); // two to close
      itSIS.push_back(pPrimSetSSides[i]->begin());
   }
   for (i = 0; i < (int)(numStepsEl); ++i)
   {
      pPrimSetSCap.push_back(static_cast<osg::DrawElementsUInt*>(pGeomShape->getPrimitiveSet(rangeDivisions + i)));
      pPrimSetSCap[i]->resize((numStepsAz + 1) * 2);
      itSIC.push_back(pPrimSetSCap[i]->begin());
      pPrimSetSCap2.push_back(static_cast<osg::DrawElementsUInt*>(pGeomShape2->getPrimitiveSet(i)));
      pPrimSetSCap2[i]->resize((numStepsAz + 1) * 2);
      itSIC2.push_back(pPrimSetSCap2[i]->begin());
   }
   // make the first index reference the last vertex element
   unsigned int nLastIndex = nVertSize - 1;

   *itSIS[0] = nLastIndex;
   ++itSIS[0];

   if ((int)(pGeomLines->getNumPrimitiveSets()) != 4 + rangeDivisions)
   {
      pGeomLines->removePrimitiveSet(0, pGeomLines->getNumPrimitiveSets());
      pGeomLines->addPrimitiveSet(new osg::DrawElementsUInt(GL_LINE_STRIP));
      pGeomLines->addPrimitiveSet(new osg::DrawElementsUInt(GL_LINE_STRIP));
      pGeomLines->addPrimitiveSet(new osg::DrawElementsUInt(GL_LINE_STRIP));
      pGeomLines->addPrimitiveSet(new osg::DrawElementsUInt(GL_LINE_STRIP));
      for (i = 0; i < rangeDivisions; ++i)
      {
         pGeomLines->addPrimitiveSet(new osg::DrawElementsUInt(GL_LINE_LOOP));
      }
   }
   auto* pPrimSetL_A = static_cast<osg::DrawElementsUInt*>(pGeomLines->getPrimitiveSet(0));
   pPrimSetL_A->resize(rangeDivisions + 1);
   auto  itLA        = pPrimSetL_A->begin();
   auto* pPrimSetL_B = static_cast<osg::DrawElementsUInt*>(pGeomLines->getPrimitiveSet(1));
   pPrimSetL_B->resize(rangeDivisions + 1);
   auto  itLB        = pPrimSetL_B->begin();
   auto* pPrimSetL_C = static_cast<osg::DrawElementsUInt*>(pGeomLines->getPrimitiveSet(2));
   pPrimSetL_C->resize(rangeDivisions + 1);
   auto  itLC        = pPrimSetL_C->begin();
   auto* pPrimSetL_D = static_cast<osg::DrawElementsUInt*>(pGeomLines->getPrimitiveSet(3));
   pPrimSetL_D->resize(rangeDivisions + 1);
   auto itLD = pPrimSetL_D->begin();

   std::vector<osg::DrawElementsUInt::iterator> itLE;
   std::vector<osg::DrawElementsUInt*>          pPrimSetL_E;
   for (i = 0; i < rangeDivisions; ++i)
   {
      pPrimSetL_E.push_back(static_cast<osg::DrawElementsUInt*>(pGeomLines->getPrimitiveSet(i + 4)));
      pPrimSetL_E[i]->resize(2 * (numStepsAz + numStepsEl) + 1);
      itLE.push_back(pPrimSetL_E[i]->begin());
   }
   *itLA = nLastIndex;
   ++itLA;
   *itLB = nLastIndex;
   ++itLB;
   *itLC = nLastIndex;
   ++itLC;
   *itLD = nLastIndex;
   ++itLD;

   // spherical coordinates that will be used:
   // x = R * cos( elevation ) * sin( azimuth )
   // y = R * cos( elevation ) * cos( azimuth )
   // z = R * sin( elevation )
   int nIndex = 0;

   for (j = 0; j < rangeDivisions; ++j)
   {
      fRange = (j + 1) * msRangeDivisionRate;
      fRange = (fRange > maxR) ? maxR : fRange;

      if (j == 0)
      {
         unsigned int nAzIndex;
         unsigned int nElIndex;
         GLuint       remember = 0U;

         nAzIndex = numStepsAz;
         for (nElIndex = 0; nElIndex < numStepsEl; ++nElIndex)
         {
            auto nIndexSafe = ut::safe_cast<GLuint, int>(nIndex);

            if (nElIndex == 0)
            {
               *itLA = nIndexSafe;
               ++itLA;
               remember = nIndexSafe;
            }
            *itLE[j] = nIndexSafe;
            ++itLE[j];
            *itSIS[j] = nIndexSafe;
            ++itSIS[j];

            AddVertex(nAzIndex, nElIndex, aMinAz, aMinEl, aMaxAz, aMaxEl, sampling, fRange, itVertex);
            ++nIndex;
         }
         nElIndex = numStepsEl;
         for (nAzIndex = numStepsAz; nAzIndex > 0; --nAzIndex)
         {
            auto nIndexSafe = ut::safe_cast<GLuint, int>(nIndex);

            if (nAzIndex == numStepsAz)
            {
               *itLB = nIndexSafe;
               ++itLB;
            }
            *itLE[j] = nIndexSafe;
            ++itLE[j];
            *itSIS[j] = nIndexSafe;
            ++itSIS[j];

            AddVertex(nAzIndex, nElIndex, aMinAz, aMinEl, aMaxAz, aMaxEl, sampling, fRange, itVertex);
            ++nIndex;
         }
         nAzIndex = 0;
         for (nElIndex = numStepsEl; nElIndex > 0; --nElIndex)
         {
            auto nIndexSafe = ut::safe_cast<GLuint, int>(nIndex);

            if (nElIndex == numStepsEl)
            {
               *itLC = nIndexSafe;
               ++itLC;
            }
            *itLE[j] = nIndexSafe;
            ++itLE[j];
            *itSIS[j] = nIndexSafe;
            ++itSIS[j];

            AddVertex(nAzIndex, nElIndex, aMinAz, aMinEl, aMaxAz, aMaxEl, sampling, fRange, itVertex);
            ++nIndex;
         }
         nElIndex = 0;
         for (nAzIndex = 0; nAzIndex < numStepsAz; ++nAzIndex)
         {
            auto nIndexSafe = ut::safe_cast<GLuint, int>(nIndex);

            if (nAzIndex == 0)
            {
               *itLD = nIndexSafe;
               ++itLD;
            }
            *itLE[j] = nIndexSafe;
            ++itLE[j];
            *itSIS[j] = nIndexSafe;
            ++itSIS[j];

            AddVertex(nAzIndex, nElIndex, aMinAz, aMinEl, aMaxAz, aMaxEl, sampling, fRange, itVertex);
            ++nIndex;
         }
         *itLE[j] = remember;
         ++itLE[j];
         *itSIS[j] = remember;
         ++itSIS[j];
      }
      else
      {
         unsigned int nAzIndex;
         unsigned int nElIndex;
         int          remember   = 0;
         auto         stepsCheck = ut::safe_cast<int, unsigned int>(2U * (numStepsAz + numStepsEl));

         nAzIndex = numStepsAz;
         for (nElIndex = 0; nElIndex < numStepsEl; ++nElIndex)
         {
            auto nIndexSafe = ut::safe_cast<GLuint, int>(nIndex);

            if (nElIndex == 0)
            {
               *itLA = nIndexSafe;
               ++itLA;
               remember = nIndex;
            }
            *itLE[j] = nIndexSafe;
            ++itLE[j];
            *itSIS[j] = ut::safe_cast<GLuint, int>(nIndex - stepsCheck);
            ++itSIS[j];
            *itSIS[j] = nIndexSafe;
            ++itSIS[j];

            AddVertex(nAzIndex, nElIndex, aMinAz, aMinEl, aMaxAz, aMaxEl, sampling, fRange, itVertex);
            ++nIndex;
         }
         nElIndex = numStepsEl;
         for (nAzIndex = numStepsAz; nAzIndex > 0; --nAzIndex)
         {
            auto nIndexSafe = ut::safe_cast<GLuint, int>(nIndex);

            if (nAzIndex == numStepsAz)
            {
               *itLB = nIndexSafe;
               ++itLB;
            }
            *itLE[j] = nIndexSafe;
            ++itLE[j];
            *itSIS[j] = ut::safe_cast<GLuint, int>(nIndex - stepsCheck);
            ++itSIS[j];
            *itSIS[j] = nIndexSafe;
            ++itSIS[j];

            AddVertex(nAzIndex, nElIndex, aMinAz, aMinEl, aMaxAz, aMaxEl, sampling, fRange, itVertex);
            ++nIndex;
         }
         nAzIndex = 0;
         for (nElIndex = numStepsEl; nElIndex > 0; --nElIndex)
         {
            auto nIndexSafe = ut::safe_cast<GLuint, int>(nIndex);

            if (nElIndex == numStepsEl)
            {
               *itLC = nIndexSafe;
               ++itLC;
            }
            *itLE[j] = nIndexSafe;
            ++itLE[j];
            *itSIS[j] = ut::safe_cast<GLuint, int>(nIndex - stepsCheck);
            ++itSIS[j];
            *itSIS[j] = nIndexSafe;
            ++itSIS[j];

            AddVertex(nAzIndex, nElIndex, aMinAz, aMinEl, aMaxAz, aMaxEl, sampling, fRange, itVertex);
            ++nIndex;
         }
         nElIndex = 0;
         for (nAzIndex = 0; nAzIndex < numStepsAz; ++nAzIndex)
         {
            auto nIndexSafe = ut::safe_cast<GLuint, int>(nIndex);

            if (nAzIndex == 0)
            {
               *itLD = nIndexSafe;
               ++itLD;
            }
            *itLE[j] = nIndexSafe;
            ++itLE[j];
            *itSIS[j] = ut::safe_cast<GLuint, int>(nIndex - stepsCheck);
            ++itSIS[j];
            *itSIS[j] = nIndexSafe;
            ++itSIS[j];

            AddVertex(nAzIndex, nElIndex, aMinAz, aMinEl, aMaxAz, aMaxEl, sampling, fRange, itVertex);
            ++nIndex;
         }
         *itLE[j] = ut::safe_cast<GLuint, int>(remember);
         ++itLE[j];
         *itSIS[j] = ut::safe_cast<GLuint, int>(remember - stepsCheck);
         ++itSIS[j];
         *itSIS[j] = ut::safe_cast<GLuint, int>(remember);
         ++itSIS[j];
      }
      if (j == rangeDivisions - 1)
      {
         int nElIndex;
         int nAzIndex;

         for (nElIndex = 0; nElIndex <= ut::safe_cast<int, unsigned int>(numStepsEl); ++nElIndex)
         {
            for (nAzIndex = 0; nAzIndex <= ut::safe_cast<int, unsigned int>(numStepsAz); ++nAzIndex)
            {
               AddVertex(nAzIndex, nElIndex, aMinAz, aMinEl, aMaxAz, aMaxEl, sampling, fRange, itVertex);
               if (nElIndex > 0)
               {
                  auto nIndexSafe = ut::safe_cast<GLuint, int>(nIndex);
                  auto stepsCheck = ut::safe_cast<int, unsigned int>(numStepsAz + 1U);

                  *itSIC[nElIndex - 1] = ut::safe_cast<GLuint, int>(nIndex - stepsCheck);
                  ++itSIC[nElIndex - 1];
                  *itSIC[nElIndex - 1] = nIndexSafe;
                  ++itSIC[nElIndex - 1];

                  *itSIC2[nElIndex - 1] = ut::safe_cast<GLuint, int>(nIndex - stepsCheck);
                  ++itSIC2[nElIndex - 1];
                  *itSIC2[nElIndex - 1] = nIndexSafe;
                  ++itSIC2[nElIndex - 1];
               }
               ++nIndex;
            }
         }
      }
   }
   // insert the center point
   itVertex->set(0.0f, 0.0f, 0.0f);

   // force an update to the shape
   pGeomShape->dirtyBound();
   pGeomLines->dirtyBound();
}

//////////////////////////////
void wkf::SensorBeam::BuildFullAzShape(float aMinR, float aMaxR, float aMinEl, float aMaxEl)
{
   // if we are within a hundredth of a degree of full 360, consider it full 360
   float elWidth = aMaxEl - aMinEl;
   if (elWidth == 0)
   {
      return; // there is nothing to draw
   }

   float sampling;
   float maxR   = std::min(msMaxRange, aMaxR);
   float fRange = maxR;
   if (elWidth < osg::DegreesToRadians(1.0))
   {
      sampling = osg::DegreesToRadians(0.1);
   }
   else if (elWidth < osg::DegreesToRadians(10.0))
   {
      sampling = osg::DegreesToRadians(1.0);
   }
   else
   {
      sampling = osg::DegreesToRadians(10.0);
   }
   unsigned int numStepsAz = (int)(osg::PI / sampling) * 2 + 1;
   unsigned int numStepsEl = (int)(0.5 * elWidth / sampling) * 2 + 1;
   numStepsAz += 1;
   numStepsEl += 1;
   int numStepsRng = (int)(maxR / msRangeDivisionRate) + 1;
   int i;
   int j;

   RebuildDrawables();

   auto* pGeomShape  = static_cast<osg::Geometry*>(mGeode->getDrawable(0));
   auto* pGeomLines  = static_cast<osg::Geometry*>(mLinesGeode->getDrawable(mLineShapeIdx));
   auto* pGeomShape2 = static_cast<osg::Geometry*>(mGridGeode->getDrawable(mGridShapeIdx));

   // find the vertex buffer
   auto* pVertices = static_cast<osg::Vec3Array*>(pGeomShape->getVertexArray());

   if (!pVertices)
   {
      // create and bind a new vertex buffer
      pVertices = new osg::Vec3Array;
      pGeomShape->setVertexArray(pVertices);
      pGeomLines->setVertexArray(pVertices);
      pGeomShape2->setVertexArray(pVertices);
   }

   // set the new size of the vertex buffer
   unsigned int nVertSize = (numStepsEl + 1) * (numStepsAz + 1) + // the cap
                            1 +                                   // the emission point
                            numStepsRng * (2 * (numStepsAz));     // intermediate divisions

   pVertices->resize(nVertSize);

   // obtain an iterator to the first vertex
   auto itVertex = pVertices->begin();

   if (pGeomShape->getNumPrimitiveSets() != 2 * numStepsRng + numStepsEl)
   {
      pGeomShape->removePrimitiveSet(0, pGeomShape->getNumPrimitiveSets()); // clear it out
      pGeomShape2->removePrimitiveSet(0, pGeomShape2->getNumPrimitiveSets());
      pGeomShape->addPrimitiveSet(new osg::DrawElementsUInt(GL_TRIANGLE_FAN)); // first step out from range top
      pGeomShape->addPrimitiveSet(new osg::DrawElementsUInt(GL_TRIANGLE_FAN)); // first step out from range bottom
      for (i = 1; i < numStepsRng; ++i)
      {
         pGeomShape->addPrimitiveSet(new osg::DrawElementsUInt(GL_TRIANGLE_STRIP)); // every additional range increment top
         pGeomShape->addPrimitiveSet(new osg::DrawElementsUInt(GL_TRIANGLE_STRIP)); // every additional range increment bottom
      }
      for (i = 0; i < (int)(numStepsEl); ++i)
      {
         pGeomShape->addPrimitiveSet(new osg::DrawElementsUInt(GL_QUAD_STRIP)); // elevation increments on the cap
         pGeomShape2->addPrimitiveSet(new osg::DrawElementsUInt(GL_QUAD_STRIP));
      }
   }
   std::vector<osg::DrawElementsUInt*> pPrimSetSSidesTop;
   std::vector<osg::DrawElementsUInt*> pPrimSetSSidesBottom;
   std::vector<osg::DrawElementsUInt*> pPrimSetSCap;
   std::vector<osg::DrawElementsUInt*> pPrimSetSCap2;

   std::vector<osg::DrawElementsUInt::iterator> itSIT;
   std::vector<osg::DrawElementsUInt::iterator> itSIB;
   std::vector<osg::DrawElementsUInt::iterator> itSIC;
   std::vector<osg::DrawElementsUInt::iterator> itSIC2;

   pPrimSetSSidesTop.push_back(static_cast<osg::DrawElementsUInt*>(pGeomShape->getPrimitiveSet(0)));    // triangle fan
   pPrimSetSSidesBottom.push_back(static_cast<osg::DrawElementsUInt*>(pGeomShape->getPrimitiveSet(1))); // triangle fan
   pPrimSetSSidesTop[0]->resize(numStepsAz + 1 + 1);    // one to close, and one for the start point
   pPrimSetSSidesBottom[0]->resize(numStepsAz + 1 + 1); // one to close, and one for the start point
   itSIT.push_back(pPrimSetSSidesTop[0]->begin());
   itSIB.push_back(pPrimSetSSidesBottom[0]->begin());
   for (i = 1; i < numStepsRng; ++i)
   {
      pPrimSetSSidesTop.push_back(static_cast<osg::DrawElementsUInt*>(pGeomShape->getPrimitiveSet(2 * i)));
      pPrimSetSSidesTop[i]->resize(2 * (numStepsAz) + 2); // two to close
      itSIT.push_back(pPrimSetSSidesTop[i]->begin());
      pPrimSetSSidesBottom.push_back(static_cast<osg::DrawElementsUInt*>(pGeomShape->getPrimitiveSet(2 * i + 1)));
      pPrimSetSSidesBottom[i]->resize(2 * (numStepsAz) + 2); // two to close
      itSIB.push_back(pPrimSetSSidesBottom[i]->begin());
   }
   for (i = 0; i < (int)(numStepsEl); ++i)
   {
      pPrimSetSCap.push_back(static_cast<osg::DrawElementsUInt*>(pGeomShape->getPrimitiveSet(2 * numStepsRng + i)));
      pPrimSetSCap[i]->resize((numStepsAz + 1) * 2);
      itSIC.push_back(pPrimSetSCap[i]->begin());
      pPrimSetSCap2.push_back(static_cast<osg::DrawElementsUInt*>(pGeomShape2->getPrimitiveSet(i)));
      pPrimSetSCap2[i]->resize((numStepsAz + 1) * 2);
      itSIC2.push_back(pPrimSetSCap2[i]->begin());
   }
   // make the first index reference the last vertex element
   unsigned int nLastIndex = nVertSize - 1;

   *itSIT[0] = nLastIndex;
   ++itSIT[0];
   *itSIB[0] = nLastIndex;
   ++itSIB[0];

   if ((int)(pGeomLines->getNumPrimitiveSets()) != 2 * numStepsRng)
   {
      pGeomLines->removePrimitiveSet(0, pGeomLines->getNumPrimitiveSets());
      for (i = 0; i < numStepsRng; ++i)
      {
         pGeomLines->addPrimitiveSet(new osg::DrawElementsUInt(GL_LINE_LOOP));
         pGeomLines->addPrimitiveSet(new osg::DrawElementsUInt(GL_LINE_LOOP));
      }
   }

   std::vector<osg::DrawElementsUInt::iterator> itLE;
   std::vector<osg::DrawElementsUInt*>          pPrimSetL_E;
   std::vector<osg::DrawElementsUInt::iterator> itLF;
   std::vector<osg::DrawElementsUInt*>          pPrimSetL_F;
   for (i = 0; i < numStepsRng; ++i)
   {
      pPrimSetL_E.push_back(static_cast<osg::DrawElementsUInt*>(pGeomLines->getPrimitiveSet(2 * i)));
      pPrimSetL_E[i]->resize(numStepsAz);
      itLE.push_back(pPrimSetL_E[i]->begin());
      pPrimSetL_F.push_back(static_cast<osg::DrawElementsUInt*>(pGeomLines->getPrimitiveSet(2 * i + 1)));
      pPrimSetL_F[i]->resize(numStepsAz);
      itLF.push_back(pPrimSetL_F[i]->begin());
   }

   // spherical coordinates that will be used:
   // x = R * cos( elevation ) * sin( azimuth )
   // y = R * cos( elevation ) * cos( azimuth )
   // z = R * sin( elevation )
   int nIndex = 0;

   for (j = 0; j < numStepsRng; ++j)
   {
      fRange = (j + 1) * msRangeDivisionRate;
      fRange = (fRange > maxR) ? maxR : fRange;

      if (j == 0)
      {
         unsigned int nAzIndex;
         int          remember = 0;

         for (nAzIndex = 0; nAzIndex < numStepsAz; ++nAzIndex)
         {
            auto nIndexSafe = ut::safe_cast<GLuint, int>(nIndex);

            if (nAzIndex == 0)
            {
               remember = nIndex;
            }
            *itLE[j] = nIndexSafe;
            ++itLE[j];
            *itSIT[j] = nIndexSafe;
            ++itSIT[j];
            AddVertex(nAzIndex, 0, -(float)osg::PI, aMinEl, (float)osg::PI, aMaxEl, sampling, fRange, itVertex);
            ++nIndex;

            nIndexSafe = ut::safe_cast<GLuint, int>(nIndex);

            *itLF[j] = nIndexSafe;
            ++itLF[j];
            *itSIB[j] = nIndexSafe;
            ++itSIB[j];
            AddVertex(nAzIndex, numStepsEl, -(float)osg::PI, aMinEl, (float)osg::PI, aMaxEl, sampling, fRange, itVertex);
            ++nIndex;
         }
         *itSIT[j] = ut::safe_cast<GLuint, int>(remember);
         ++itSIT[j];
         *itSIB[j] = ut::safe_cast<GLuint, int>(remember + 1);
         ++itSIB[j];
      }
      else
      {
         unsigned int nAzIndex;
         int          remember   = 0;
         auto         stepsCheck = ut::safe_cast<int, unsigned int>(2U * (numStepsAz));

         for (nAzIndex = 0; nAzIndex < numStepsAz; ++nAzIndex)
         {
            auto nIndexSafe = ut::safe_cast<GLuint, int>(nIndex);

            if (nAzIndex == 0)
            {
               remember = nIndex;
            }
            *itLE[j] = nIndexSafe;
            ++itLE[j];
            *itSIB[j] = ut::safe_cast<GLuint, int>(nIndex - stepsCheck);
            ++itSIB[j];
            *itSIB[j] = nIndexSafe;
            ++itSIB[j];
            AddVertex(nAzIndex, 0, -(float)osg::PI, aMinEl, (float)osg::PI, aMaxEl, sampling, fRange, itVertex);
            ++nIndex;

            nIndexSafe = ut::safe_cast<GLuint, int>(nIndex);

            *itLF[j] = nIndexSafe;
            ++itLF[j];
            *itSIT[j] = ut::safe_cast<GLuint, int>(nIndex - stepsCheck);
            ++itSIT[j];
            *itSIT[j] = nIndexSafe;
            ++itSIT[j];
            AddVertex(nAzIndex, numStepsEl, -(float)osg::PI, aMinEl, (float)osg::PI, aMaxEl, sampling, fRange, itVertex);
            ++nIndex;
         }
         *itSIB[j] = ut::safe_cast<GLuint, int>(remember - stepsCheck);
         ++itSIB[j];
         *itSIB[j] = ut::safe_cast<GLuint, int>(remember);
         ++itSIB[j];

         *itSIT[j] = ut::safe_cast<GLuint, int>(remember + 1 - stepsCheck);
         ++itSIT[j];
         *itSIT[j] = ut::safe_cast<GLuint, int>(remember + 1);
         ++itSIT[j];
      }
      if (j == numStepsRng - 1)
      {
         int          nElIndex;
         unsigned int nAzIndex;
         auto         stepsCheck = ut::safe_cast<int, unsigned int>(numStepsAz + 1);

         for (nElIndex = 0; nElIndex <= (int)(numStepsEl); ++nElIndex)
         {
            for (nAzIndex = 0; nAzIndex <= numStepsAz; ++nAzIndex)
            {
               AddVertex(nAzIndex, nElIndex, -(float)osg::PI, aMinEl, (float)osg::PI, aMaxEl, sampling, fRange, itVertex);
               if (nElIndex > 0)
               {
                  auto nIndexSafe = ut::safe_cast<GLuint, int>(nIndex);

                  *itSIC[nElIndex - 1] = ut::safe_cast<GLuint, int>(nIndex - stepsCheck);
                  ++itSIC[nElIndex - 1];
                  *itSIC[nElIndex - 1] = nIndexSafe;
                  ++itSIC[nElIndex - 1];

                  *itSIC2[nElIndex - 1] = ut::safe_cast<GLuint, int>(nIndex - stepsCheck);
                  ++itSIC2[nElIndex - 1];
                  *itSIC2[nElIndex - 1] = nIndexSafe;
                  ++itSIC2[nElIndex - 1];
               }
               ++nIndex;
            }
         }
      }
   }
   // insert the center point
   itVertex->set(0.0f, 0.0f, 0.0f);
   ++nIndex;

   // force an update to the shape
   pGeomShape->dirtyBound();
   pGeomLines->dirtyBound();
}

void wkf::SensorBeam::RebuildDrawables()
{
   mGeode->removeDrawables(0, mGeode->getNumDrawables()); // clear out existing drawables
   mProjectionGeode->removeDrawables(0, mProjectionGeode->getNumDrawables());
   mLinesGeode->removeDrawables(0, mLinesGeode->getNumDrawables());
   mGridGeode->removeDrawables(0, mGridGeode->getNumDrawables());
   if (!mGeode->getNumDrawables() && !mProjectionGeode->getNumDrawables() && !mLinesGeode->getNumDrawables() &&
       !mGridGeode->getNumDrawables())
   {
      BuildDrawables();
   }
}

void wkf::SensorBeam::SetVisible(bool aState)
{
   mLocalPAT->setNodeMask(aState ? 0xFFFFFFFF : 0x00000000);
}

// private
void wkf::SensorBeam::BuildDrawables()
{
   // create the new geometry nodes
   osg::Geometry* pGeomShape  = new osg::Geometry;
   osg::Geometry* pGeomLines  = new osg::Geometry;
   osg::Geometry* pGeomShape2 = new osg::Geometry;
   // add to the root node
   mGeode->addDrawable(pGeomShape);
   mProjectionGeode->addDrawable(pGeomShape);
   mLinesGeode->addDrawable(pGeomLines);
   mLineShapeIdx = mLinesGeode->getNumDrawables() - 1;
   mGridGeode->addDrawable(pGeomShape2);
   mGridShapeIdx = mGridGeode->getNumDrawables() - 1;
   // force the shapes to use a display list
   pGeomShape->setUseDisplayList(true);
   pGeomLines->setUseDisplayList(true);
   pGeomShape2->setUseDisplayList(true);
   // force the lines to be drawn first then the shape second
   // this will allow for proper blending of the lines and the shape
   mGeode->getOrCreateStateSet()->setRenderBinDetails(14, "DepthSortedBin");
   mGeode->getOrCreateStateSet()->setNestRenderBins(false);
   mLinesGeode->getOrCreateStateSet()->setRenderBinDetails(15, "DepthSortedBin");
   mLinesGeode->getOrCreateStateSet()->setNestRenderBins(false);
   mGridGeode->getOrCreateStateSet()->setRenderBinDetails(15, "DepthSortedBin");
   mGridGeode->getOrCreateStateSet()->setNestRenderBins(false);

   osg::ref_ptr<osg::BlendFunc> blend = new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   mGeode->getOrCreateStateSet()->setAttributeAndModes(blend.get(), osg::StateAttribute::ON);
   mGeode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   mGeode->getOrCreateStateSet()->setAttributeAndModes(new osg::Light(), osg::StateAttribute::OFF);

   pGeomLines->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::LESS, 0.0, 1.0, false));
   pGeomLines->getOrCreateStateSet()->setRenderBinDetails(2, "DepthSortedBin");
   pGeomLines->getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
   pGeomShape2->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::LESS, 0.0, 1.0, false));
   pGeomShape2->getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
   pGeomShape2->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc(), osg::StateAttribute::ON);
   pGeomShape2->getOrCreateStateSet()->setAttributeAndModes(new osg::CullFace(osg::CullFace::BACK));
   pGeomShape2->getOrCreateStateSet()->setAttributeAndModes(
      new osg::PolygonMode(osg::PolygonMode::FRONT, osg::PolygonMode::LINE));
   pGeomShape2->getOrCreateStateSet()->setRenderBinDetails(3, "DepthSortedBin");
}

void wkf::SensorBeam::AddVertex(int                       aAz,
                                int                       aEl,
                                float                     aMinAz,
                                float                     aMinEl,
                                float                     aMaxAz,
                                float                     aMaxEl,
                                float                     aSampling,
                                float                     aRange,
                                osg::Vec3Array::iterator& aItVertex)
{
   float az = aMinAz + aSampling * aAz;
   az       = (az > aMaxAz) ? aMaxAz : az;
   float el = aMinEl + aSampling * aEl;
   el       = (el > aMaxEl) ? aMaxEl : el;

   float     fCosEl = cos(el);
   osg::Vec3 xyz(aRange * fCosEl * cos(az), aRange * fCosEl * sin(az), aRange * -sin(el));

   // insert the point into the list of vertices
   aItVertex->set(xyz);
   // increase the iterator
   ++aItVertex;
}

bool wkf::SensorBeam::SetShapeColor(const osg::Vec4& aColor)
{
   mGeode->getOrCreateStateSet()->addUniform(new osg::Uniform("uColor", aColor));
   return true;
}

bool wkf::SensorBeam::SetLineColor(const osg::Vec4& aColor)
{
   mLinesGeode->getOrCreateStateSet()->addUniform(new osg::Uniform("uColor", aColor));
   mGridGeode->getOrCreateStateSet()->addUniform(new osg::Uniform("uColor", aColor));
   return true;
}

bool wkf::SensorBeam::SetProjectionColor(const osg::Vec4& aColor)
{
   mProjectionGeode->getOrCreateStateSet()->addUniform(new osg::Uniform("uColor", aColor));
   return true;
}

void wkf::SensorBeam::SetProjectionEnabled(bool aState)
{
   mProjectionGeode->setNodeMask(aState ? 0xFFFFFFFF : 0x00000000);
}

void wkf::SensorBeam::SetFacesEnabled(bool aState)
{
   mGeode->setNodeMask(aState ? 0xFFFFFFFF : 0x00000000);
}

void wkf::SensorBeam::SetEdgesEnabled(bool aState)
{
   mLinesGeode->setNodeMask(aState ? 0xFFFFFFFF : 0x00000000);
   mGridGeode->setNodeMask(aState ? 0xFFFFFFFF : 0x00000000);
}

void wkf::SensorBeam::SetShaders(osg::Program* aLineProgram, osg::Program* aTriProgram)
{
   mGridGeode->getOrCreateStateSet()->setAttributeAndModes(aTriProgram);
   mLinesGeode->getOrCreateStateSet()->setAttributeAndModes(aLineProgram);
   mGeode->getOrCreateStateSet()->setAttributeAndModes(aTriProgram);
   mProjectionGeode->getOrCreateStateSet()->setAttributeAndModes(aTriProgram);
}

void wkf::SensorBeam::UseFlatStateSet(bool aState)
{
   if (aState)
   {
      mProjFace->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::LESS, 0.0, 1.0, false));
      mProjFill->getOrCreateStateSet()->setAttributeAndModes(new osg::CullFace(osg::CullFace::BACK));
   }
   else
   {
      mProjFace->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::GREATER, 0.0, 1.0, false));
      mProjFill->getOrCreateStateSet()->setAttributeAndModes(new osg::CullFace(osg::CullFace::FRONT));
   }
}
