// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "VaAttachmentWingRibbon.hpp"

#include <osg/Array>
#include <osg/BlendFunc>
#include <osg/BufferObject>
#include <osg/Depth>
#include <osg/Drawable>
#include <osg/GL2Extensions>
#include <osg/Geode>
#include <osg/Quat>
#include <osg/Vec3>

#include "UtoEntity.hpp"
#include "UtoRawShape.hpp"
#include "UtoShaders.hpp"
#include "UtoViewer.hpp"
#include "VaEntity.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"
#include "VaViewerManager.hpp"

namespace vespa
{
osg::Program* VaAttachmentWingRibbon::sOsgProgram          = nullptr;
float         VaAttachmentWingRibbon::sTransparencyOnDeath = 0.5f;

class VaAttachmentWingRibbon_Drawable : public osg::Drawable
{
public:
   static const auto           cVERTEX_SIZE = sizeof(vespa::VaAttachmentWingRibbon::Vertex);
   mutable int                 mBufferSize;
   mutable unsigned int        mBufferId;
   mutable int                 mStateBufferSize;
   VaAttachmentWingRibbon*     mAttachment;
   mutable osg::GL2Extensions* mExt;

   VaAttachmentWingRibbon_Drawable(VaAttachmentWingRibbon* aAttachment)
   {
      mBufferSize       = -1;
      mStateBufferSize  = -1;
      mAttachment       = aAttachment;
      osg::StateSet* ss = getOrCreateStateSet();
      ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
      setSupportsDisplayList(false);
      mExt = nullptr;

      ss->setMode(GL_BLEND, osg::StateAttribute::ON);
      ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
      ss->setAttributeAndModes(new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA));
      ss->setAttributeAndModes(new osg::ColorMask(true, true, true, true));      // ensure alpha channel exists
      ss->setAttributeAndModes(new osg::Depth(osg::Depth::LEQUAL, 0, 1, false)); // turn off writing to depth buffer

      // ss->setRenderBinDetails(2, "RenderBin");
   }
   ~VaAttachmentWingRibbon_Drawable() override
   {
      if (mExt)
      {
         mExt->glDeleteBuffers(1, &mBufferId);
      }
   }
   void firstTimeInit(osg::RenderInfo& renderInfo) const
   {
      mExt = osg::GL2Extensions::Get(renderInfo.getContextID(), true);
      mExt->glGenBuffers(1, &mBufferId);
   }
   void WriteBuffer() const
   {
      VaAttachmentWingRibbon& p            = *mAttachment;
      int                     requiredSize = p.mCircular.mBufferSize * sizeof(VaAttachmentWingRibbon::Vertex);
      if (mBufferSize != requiredSize)
      {
         mBufferSize = requiredSize;
         mExt->glBindBuffer(GL_ARRAY_BUFFER_ARB, mBufferId);
         mExt->glBufferData(GL_ARRAY_BUFFER_ARB, p.mCircular.mBufferSize * cVERTEX_SIZE, &p.mVerts[0], GL_DYNAMIC_DRAW);
         mExt->glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
         p.mDirtyIndices.Clear();
      }
      else
      {
         if (!p.mDirtyIndices.mRanges.empty())
         {
            mExt->glBindBuffer(GL_ARRAY_BUFFER_ARB, mBufferId);
            p.mDirtyIndices.Reduce();
            for (size_t i = 0; i < p.mDirtyIndices.mRanges.size(); ++i)
            {
               int start = p.mDirtyIndices.mRanges[i].mStart;
               int end   = p.mDirtyIndices.mRanges[i].mEnd;
               mExt->glBufferSubData(GL_ARRAY_BUFFER_ARB, start * cVERTEX_SIZE, (end - start) * cVERTEX_SIZE, &p.mVerts[start]);
            }
            mExt->glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
            p.mDirtyIndices.Clear();
         }
      }
   }
   void Draw(osg::RenderInfo& renderInfo) const
   {
      VaAttachmentWingRibbon& p = *mAttachment;

      if (p.mCircular.mSize == 0)
      {
         return;
      }

      WriteBuffer();

      mExt->glBindBuffer(GL_ARRAY_BUFFER_ARB, mBufferId);

      glVertexPointer(4,
                      GL_FLOAT,
                      cVERTEX_SIZE,
                      nullptr); // reinterpret_cast<const GLvoid*>(0)); //contains position and time
      glColorPointer(4,
                     GL_FLOAT,
                     cVERTEX_SIZE,
                     reinterpret_cast<const GLvoid*>(4 * sizeof(float))); // contains wingTipA and mainStateVal
      glNormalPointer(GL_FLOAT, cVERTEX_SIZE, reinterpret_cast<const GLvoid*>(8 * sizeof(float))); // contains wintTipB
      glTexCoordPointer(2,
                        GL_FLOAT,
                        cVERTEX_SIZE,
                        reinterpret_cast<const GLvoid*>(11 * sizeof(float))); // contains edgeStateVal and isAlive

      int pointsRemain = std::min(p.mIndexAtCurrentTime + 1, p.mCircular.mSize);
      int firstPoints  = p.mCircular.End1() - p.mCircular.mStart;
      if (pointsRemain < firstPoints)
      {
         firstPoints = pointsRemain;
      }
      pointsRemain -= firstPoints;

      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_COLOR_ARRAY);
      glEnableClientState(GL_NORMAL_ARRAY);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);

      glDrawArrays(GL_LINE_STRIP, p.mCircular.mStart, firstPoints);
      if (p.mCircular.Wraps())
      {
         unsigned int temp[2] = {p.mCircular.End1() - 1u, 0u};
         glDrawElements(GL_LINE_STRIP, 2, GL_UNSIGNED_INT, temp); // span the gap
         glDrawArrays(GL_LINE_STRIP, 0, pointsRemain);
      }
      glDisableClientState(GL_VERTEX_ARRAY);
      glDisableClientState(GL_COLOR_ARRAY);
      glDisableClientState(GL_NORMAL_ARRAY);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);

      mExt->glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
      mExt->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


      if (p.mIndexAtCurrentTime > 0)
      {
         int    endIdx = p.mCircular.IndexToOffset(p.mIndexAtCurrentTime);
         double ecef[3];
         p.GetParent().GetPosition().GetECEF(ecef);
         if (osg::Vec3f(ecef[0], ecef[1], ecef[2]) != p.mVerts[endIdx].mPos)
         {
            glBegin(GL_LINES);
            glColor4fv(reinterpret_cast<float*>(&p.mVerts[endIdx].mWingTipA));
            glNormal3fv(reinterpret_cast<float*>(&p.mVerts[endIdx].mWingTipB._v));
            glTexCoord2f(p.mVerts[endIdx].mStateValEdge, p.mVerts[endIdx].mIsAlive);
            glVertex4fv(reinterpret_cast<float*>(&p.mVerts[endIdx]));
            double coordsWithTime[4] = {ecef[0], ecef[1], ecef[2], p.mCurrentTime};
            glColor4fv(reinterpret_cast<float*>(&p.mVerts[endIdx].mWingTipA));
            glNormal3fv(reinterpret_cast<float*>(&p.mVerts[endIdx].mWingTipB));
            glTexCoord2f(p.mVerts[endIdx].mStateValEdge, p.mVerts[endIdx].mIsAlive);
            glVertex4dv(coordsWithTime);
            glEnd();
         }
      }
   }
   void drawImplementation(osg::RenderInfo& renderInfo) const override
   {
      if (!mExt)
      {
         firstTimeInit(renderInfo);
      }
      osg::State& s = *renderInfo.getState();
      s.disableAllVertexArrays();
      s.setUseModelViewAndProjectionUniforms(true);

      if (mAttachment->mScreenSpaceScaleDirty)
      {
         mAttachment->ReCalculateScreenSpaceScale();
      }
      // s.updateModelViewAndProjectionMatrixUniforms();
      Draw(renderInfo);
   }
   Object* cloneType() const override { return new VaAttachmentWingRibbon_Drawable(*this); }
   Object* clone(const osg::CopyOp&) const override { return new VaAttachmentWingRibbon_Drawable(*this); }
};

void VaAttachmentWingRibbon::RangeSet::AddPoint(int aIndex)
{
   for (size_t i = 0; i < mRanges.size(); ++i)
   {
      Range& r = mRanges[i];
      // is contained in range?
      if (aIndex + 1 >= r.mStart && aIndex <= r.mEnd)
      {
         if (aIndex == r.mStart - 1)
         {
            --r.mStart;
         }
         if (aIndex == r.mEnd)
         {
            ++r.mEnd;
         }
         return;
      }
   }
   // new range
   Range newRange;
   newRange.mStart = aIndex;
   newRange.mEnd   = aIndex + 1;
   mRanges.push_back(newRange);
} // constructor
void VaAttachmentWingRibbon::RangeSet::Reduce()
{
   std::sort(mRanges.begin(), mRanges.end());
   for (int i = -2 + static_cast<int>(mRanges.size()); i >= 0; --i)
   {
      const Range& r1 = mRanges[i];
      const Range& r2 = mRanges[i + 1];
      if (r1.mEnd >= r2.mStart)
      {
         mRanges[i] = Range(r1.mStart, r2.mEnd);
         mRanges.erase(mRanges.begin() + i + 1);
      }
   }
}
void VaAttachmentWingRibbon::RangeSet::Clear()
{
   mRanges.clear();
}

VaAttachmentWingRibbon::VaAttachmentWingRibbon(VaEntity& aEntity, VaViewer* aViewerPtr)
   : VaAttachment(aEntity, aViewerPtr, "wing_ribbon")
   , mIndexAtCurrentTime(0)
   , mShapePtr(nullptr)
   , mIsLoaded(false)
   , mScreenSpaceScaleDirty(true)
   , mCurrentEdgeMaxStateVal(std::numeric_limits<decltype(mCurrentEdgeMaxStateVal)>::lowest())
   , mCurrentEdgeMinStateVal(std::numeric_limits<decltype(mCurrentEdgeMinStateVal)>::max())
   , mCurrentMainMaxStateVal(std::numeric_limits<decltype(mCurrentMainMaxStateVal)>::lowest())
   , mCurrentMainMinStateVal(std::numeric_limits<decltype(mCurrentMainMinStateVal)>::max())
   , mWidthScale(1)

{
   SetType<VaAttachmentWingRibbon>();
   mCircular.mStart      = 0;
   mCircular.mSize       = 0;
   mCircular.mBufferSize = 100;
   mVerts.resize(mCircular.mBufferSize);
}

/*************************************************************************************************
 * @brief   Copies the recorded points in this ribbon into the the other ribbon. so that the
 *          recorded points in both this ribbon and the passed ribbon will be the exact same.
 * @param   aCopiedRibbon  The ribbon to copy that data into. it is assumed the copied into
 *                         ribbon will mirror this ribbon, but that it was created for a
 *                         different view.
 * @date 7/28/2017
 **************************************************************************************************/
void VaAttachmentWingRibbon::CopyPointsInto(VaAttachmentWingRibbon& aCopiedRibbon) const
{
   // aCopiedRibbon.PruneBefore(std::numeric_limits<float>::max());

   for (CircularIterator iter(0, mCircular); !iter.IsEnd(); ++iter)
   {
      const auto&  vert   = mVerts[iter.GetOffset()];
      const double pos[3] = {vert.mPos.x(), vert.mPos.y(), vert.mPos.z()};
      aCopiedRibbon.AddPointBack(vert.mTime,
                                 pos,
                                 vert.mStateValMain,
                                 vert.mStateValEdge,
                                 vert.mWingTipA,
                                 vert.mWingTipB,
                                 vert.mIsAlive == 1.0f);
   }
}

//! grows the circular buffer of datapoints to the newly specified size
void VaAttachmentWingRibbon::GrowSpace(int aNewSize)
{
   int         arraySize = static_cast<int>(aNewSize * cBUFFER_GROWTH_FACTOR);
   VertexArray newVerts(arraySize);
   int         vertCount1 = (mCircular.End1() - mCircular.mStart);

   memcpy(&newVerts[0], &mVerts[mCircular.mStart], vertCount1 * sizeof(Vertex));
   if (mCircular.Wraps())
   {
      memcpy(&newVerts[vertCount1], &mVerts[0], mCircular.End2() * sizeof(Vertex));
   }

   mCircular.mStart      = 0;
   mCircular.mBufferSize = arraySize;
   mDirtyIndices.Clear();

   std::swap(mVerts, newVerts);
}

//! loops through all the recorded datapoints to determine the current min/max recorded values for the edge state vals
void VaAttachmentWingRibbon::RecalculateEdgeMinMaxLimits()
{
   mCurrentEdgeMaxStateVal = std::numeric_limits<decltype(mCurrentEdgeMaxStateVal)>::lowest();
   mCurrentEdgeMinStateVal = std::numeric_limits<decltype(mCurrentEdgeMinStateVal)>::max();

   CircularIterator iter(0, mCircular);
   for (; !iter.IsEnd(); ++iter)
   {
      auto edgeVal            = mVerts[iter.GetOffset()].mStateValEdge;
      mCurrentEdgeMaxStateVal = std::max(mCurrentEdgeMaxStateVal, edgeVal);
      mCurrentEdgeMinStateVal = std::min(mCurrentEdgeMinStateVal, edgeVal);
   }
   SetHardMaxEdgeLimit(mCurrentEdgeMaxStateVal);
   SetHardMinEdgeLimit(mCurrentEdgeMinStateVal);
}

//! loops through all the recorded datapoints to determine the current min/max recorded values for the main state vals
void VaAttachmentWingRibbon::RecalculateMainMinMaxLimits()
{
   mCurrentMainMaxStateVal = std::numeric_limits<decltype(mCurrentMainMaxStateVal)>::lowest();
   mCurrentMainMinStateVal = std::numeric_limits<decltype(mCurrentMainMinStateVal)>::max();

   CircularIterator iter(0, mCircular);
   for (; !iter.IsEnd(); ++iter)
   {
      auto mainVal            = mVerts[iter.GetOffset()].mStateValMain;
      mCurrentMainMaxStateVal = std::max(mCurrentMainMaxStateVal, mainVal);
      mCurrentMainMinStateVal = std::min(mCurrentMainMinStateVal, mainVal);
   }

   SetHardMaxMainLimit(mCurrentMainMaxStateVal);
   SetHardMinMainLimit(mCurrentMainMinStateVal);
}

void VaAttachmentWingRibbon::shrink_to_fit()
{
   GrowSpace(mCircular.GetSize() / cBUFFER_GROWTH_FACTOR + 1);
}

bool VaAttachmentWingRibbon::Highlight(VaViewer*                aViewerPtr,
                                       const VaAttachmentSubId& aSubpartId,
                                       bool                     aHighlight,
                                       const UtColor&           aColor)
{
   if (IsMyViewer(aViewerPtr))
   {
      if (aHighlight)
      {
         mShapePtr->GetTree()->getOrCreateStateSet()->setDefine("HIGHLIGHTING");
         mShapePtr->SetUniform4f("highlightColor", aColor[0], aColor[1], aColor[2], aColor[3]);
      }
      else
      {
         mShapePtr->GetTree()->getOrCreateStateSet()->removeDefine("HIGHLIGHTING");
      }
      return true;
   }
   return false;
}


/*************************************************************************************************
 * @brief   Recalculates the scale the wing ribbon needs to be to have the same scaling of the
 *          parent model's icon. Also sets the wingSpanScalar uniform in the wing ribbon shaders.
 * @note the need to determine the icon scale of its parent is what prevents a wing ribbon from
 *       being shared across VaViewer's, as I do not know how to implement this math during
 *       rendering without having been supplied a VaViewer.  As a result, we have to create an
 *       entirely new wing ribbon everytime a new viewer is added, thus duplicating the amount of
 *       work needed to record the exact same information.
 * @date 7/14/2017
 **************************************************************************************************/
void VaAttachmentWingRibbon::ReCalculateScreenSpaceScale()
{
   const auto* view    = GetViewer();
   const auto* utoview = view->GetViewer();
   if (!utoview->GetOSGViewer())
   {
      return;
   }
   float VA_IconScale = view->GetIconScale();
   float VA_TrueScale = view->GetTrueScale();

   mScreenSpaceScaleDirty = false;

   if (VA_TrueScale == 0.0f)
   {
      float scalar = 1.0f;
      // mix(min(UTO_Viewport.w / zero.w / VA_IconScale * VA_ScreenScale * 0.1, 1.0), 1.0 / UTO_OrthographicZoom /
      // VA_IconScale * VA_ScreenScale * 0.1, UTO_OrthographicMode);
      if (view->GetMapProjection().projection != 0)
      {
         // orthographic view
         scalar = 1.0f / utoview->GetOrtho()->Zoom() / VA_IconScale * 0.1;
         scalar *= mVA_ScreenScale;
      }
      else
      {
         // perspective view
         double hpr[3];
         double xyz[3];
         GetParent().GetPosition().GetSceneXYZ(xyz, view->GetUniqueId());
         GetParent().GetUtoEntity()->GetOrientationWCS(hpr[0], hpr[1], hpr[2]);
         osg::Quat    q = osg::Quat(hpr[2], osg::Vec3(1, 0, 0), hpr[1], osg::Vec3(0, 1, 0), hpr[0], osg::Vec3(0, 0, 1));
         osg::Matrixd modelMatrix;
         modelMatrix.makeIdentity();
         modelMatrix.preMultTranslate(osg::Vec3d(xyz[0], xyz[1], xyz[2]));
         modelMatrix.preMultRotate(q);

         const osg::Matrixd& viewMatrix = view->GetViewMatrix();
         // the above check for the validity of OSGViewer should confirm the validity of this matrix
         osg::Matrixd projectionMatrix = *utoview->GetProjectionMatrix();
         osg::Matrixd mvp              = modelMatrix * viewMatrix * projectionMatrix;
         osg::Vec4    zero             = mvp * osg::Vec4(0, 0, 0, 1);

         float UTO_Viewport_w = utoview->GetViewport()->height();
         scalar               = UTO_Viewport_w / zero.w() / VA_IconScale * 0.1;
         scalar *= mVA_ScreenScale;
         scalar = std::min(scalar, 1.0f);
      }

      scalar = 1.0 / scalar; // invert value, since scalar will be multiplying instead of dividing in shadar
      scalar = std::max(scalar, 1.0f);
      mShapePtr->SetUniformf("wingSpanScalar", scalar * mWidthScale);
   }
   else
   {
      // using true scale
      mShapePtr->SetUniformf("wingSpanScalar", 1.0f * mWidthScale);
   }
}

/*************************************************************************************************
 * @brief   Adjust the current min/max values of the recorded data to take into account the values being removed
 * @param   aEdgeStateValBeingRemoved  The edge state value being removed.
 * @param   aMainStateValBeingRemoved  The main state value being removed.
 * @date 7/14/2017
 **************************************************************************************************/
void VaAttachmentWingRibbon::AdjustMinMaxLimitsForRemoval(float aEdgeStateValBeingRemoved, float aMainStateValBeingRemoved)
{
   if (mEdgeLimitType == eCURRENT_INDIVIDUAL_LIMITS || mEdgeLimitType == eCURRENT_SCENARIO_LIMITS)
   {
      if (aEdgeStateValBeingRemoved >= mCurrentEdgeMaxStateVal || // theoretically I should only need to do == on all these
          aEdgeStateValBeingRemoved <= mCurrentEdgeMinStateVal)
      {
         RecalculateEdgeMinMaxLimits();
      }
   }
   if (mMainLimitType == eCURRENT_INDIVIDUAL_LIMITS || mMainLimitType == eCURRENT_SCENARIO_LIMITS)
   {
      if (aMainStateValBeingRemoved >= mCurrentMainMaxStateVal || // theoretically I should only need to do == on all these
          aMainStateValBeingRemoved <= mCurrentMainMinStateVal)
      {
         RecalculateMainMinMaxLimits();
      }
   }
}

/*************************************************************************************************
 * @brief   Adjust the current min/max values of the recorded data to take into account the newly inserted values
 * @param   aEdgeStateValBeingInserted the edge state value being inserted.
 * @param   aMainStateValBeingInserted The main state value being inserted.
 * @date 7/14/2017
 **************************************************************************************************/
void VaAttachmentWingRibbon::AdjustMinMaxLimitsForInsert(float aEdgeStateValBeingInserted, float aMainStateValBeingInserted)
{
   if (mEdgeLimitType != eHARD_LIMITS)
   {
      if (mCurrentEdgeMaxStateVal < aEdgeStateValBeingInserted)
      {
         mCurrentEdgeMaxStateVal = aEdgeStateValBeingInserted;
         SetHardMaxEdgeLimit(aEdgeStateValBeingInserted);
      }
      if (mCurrentEdgeMinStateVal > aEdgeStateValBeingInserted)
      {
         mCurrentEdgeMinStateVal = aEdgeStateValBeingInserted;
         SetHardMinEdgeLimit(aEdgeStateValBeingInserted);
      }
   }
   if (mMainLimitType != eHARD_LIMITS)
   {
      if (mCurrentMainMaxStateVal < aMainStateValBeingInserted)
      {
         mCurrentMainMaxStateVal = aMainStateValBeingInserted;
         SetHardMaxMainLimit(aMainStateValBeingInserted);
      }
      if (mCurrentMainMinStateVal > aMainStateValBeingInserted)
      {
         mCurrentMainMinStateVal = aMainStateValBeingInserted;
         SetHardMinMainLimit(aMainStateValBeingInserted);
      }
   }
}

/*************************************************************************************************
 * @brief   records a datapoint to the back of the wing ribbon (the side connecting to the platform).
 * @param   aTime          The time the data point represents.
 * @param   aPosition      The position.
 * @param   aStateValMain  The state value used in the main/center area of the ribbon.
 * @param   aStateValEdge  The state value used in the edge area of the ribbon.
 * @param   aWingTipA      The (left?) wing tip.
 * @param   aWingTipB      The (right?) wing tip.
 * @param   aIsAlive       True if the parent platform is alive.
 * @date 7/14/2017
 **************************************************************************************************/
void VaAttachmentWingRibbon::AddPointBack(const float       aTime,
                                          const double      aPosition[3],
                                          const float       aStateValMain,
                                          const float       aStateValEdge,
                                          const osg::Vec3f& aWingTipA,
                                          const osg::Vec3f& aWingTipB,
                                          const bool        aIsAlive)
{
   int   idx  = CirclePushBack();
   auto& vert = mVerts[idx];
   vert.mPos.set(aPosition[0], aPosition[1], aPosition[2]);
   vert.mTime         = aTime;
   vert.mStateValMain = aStateValMain;
   vert.mStateValEdge = aStateValEdge;
   vert.mWingTipA     = aWingTipA;
   vert.mWingTipB     = aWingTipB;
   vert.mIsAlive      = aIsAlive ? 1.0f : sTransparencyOnDeath;
   AdjustMinMaxLimitsForInsert(aStateValEdge, aStateValMain);
   mDirtyIndices.AddPoint(idx);
}

/*************************************************************************************************
 * @brief   records a datapoint to the front of the wing ribbon (the oldest side of the ribbon).
 * @param   aTime          The time the data point represents.
 * @param   aPosition      The position.
 * @param   aStateValMain  The state value used in the main/center area of the ribbon.
 * @param   aStateValEdge  The state value used in the edge area of the ribbon.
 * @param   aWingTipA      The (left?) wing tip.
 * @param   aWingTipB      The (right?) wing tip.
 * @param   aIsAlive       True if the parent platform is alive.
 * @date 7/14/2017
 **************************************************************************************************/
void VaAttachmentWingRibbon::AddPointFront(const float       aTime,
                                           const double      aPosition[3],
                                           const float       aStateValMain,
                                           const float       aStateValEdge,
                                           const osg::Vec3f& aWingTipA,
                                           const osg::Vec3f& aWingTipB,
                                           const bool        aIsAlive)
{
   int   idx  = CirclePushFront();
   auto& vert = mVerts[idx];
   vert.mPos.set(aPosition[0], aPosition[1], aPosition[2]);
   vert.mTime         = aTime;
   vert.mStateValMain = aStateValMain;
   vert.mStateValEdge = aStateValEdge;
   vert.mWingTipA     = aWingTipA;
   vert.mWingTipB     = aWingTipB;
   vert.mIsAlive      = aIsAlive ? 1.0f : sTransparencyOnDeath;
   AdjustMinMaxLimitsForInsert(aStateValEdge, aStateValMain);
   mDirtyIndices.AddPoint(idx);
}

void VaAttachmentWingRibbon::PrivateLoad()
{
   if (!mIsLoaded)
   {
      mIsLoaded = true;

      osg::Geode* geode = new osg::Geode;
      geode->setCullingActive(false); // using shaders!

      geode->addDrawable(new VaAttachmentWingRibbon_Drawable(this));
      UtoRawShape raw;
      raw.SetReferenceFrame(UtoShape::cFRAME_WORLD);
      static int q    = 0;
      auto       name = "wing_ribbon_" + std::to_string(++q);
      mShapePtr       = AddShapeToParent(name, raw);
      mShapePtr->GetTree()->addChild(geode);

      if (nullptr == sOsgProgram)
      {
         std::set<std::string> defset;
         sOsgProgram = UtoShaders::UseProgramWithGeometry("wingribbon.geom", "wingribbon.vert", "wingribbon.frag", defset);
      }
      mShapePtr->SetProgram(sOsgProgram, true);
   }
}

void VaAttachmentWingRibbon::PrivateInitializeCallbacks()
{
   mCallbacks.Add(VaObserver::ViewMatrixChanged.Connect(
      [this](VaViewer* aViewerPtr)
      {
         if (IsMyViewer(aViewerPtr))
         {
            mScreenSpaceScaleDirty = true;
         }
      }));
   mCallbacks.Add(VaObserver::EntityMoved.Connect([this](VaEntity* aEntityPtr, bool isMoving)
                                                  { mScreenSpaceScaleDirty = true; },
                                                  GetParent().GetUniqueId()));
}

//! sets the uTime uniform in the wing ribbons shaders,
//! and determine which datapoint represents that time.
void VaAttachmentWingRibbon::UpdateCurrentIndex(float aTime)
{
   if (nullptr != mShapePtr)
   {
      mShapePtr->SetUniformf("uTime", aTime);

      mCurrentTime = aTime;
   }
   if (mCircular.mSize > 0)
   {
      mIndexAtCurrentTime = std::min(mIndexAtCurrentTime, mCircular.mSize - 1);

      CircularIterator iter(mIndexAtCurrentTime, mCircular);
      if (!iter.IsEnd())
      {
         if (iter.GetOffset() >= 0 && mVerts[iter.GetOffset()].mTime > aTime)
         {
            for (;;)
            {
               if (iter.IsBegin() || mVerts[iter.GetOffset()].mTime <= aTime)
               {
                  mIndexAtCurrentTime = iter.GetIndex();
                  break;
               }
               --iter;
            }
         }
         else // vertTime <= aTime
         {
            while (!iter.IsEnd())
            {
               if (mVerts[iter.GetOffset()].mTime > aTime)
               {
                  break;
               }
               mIndexAtCurrentTime = iter.mIndex;
               ++iter;
            }
         }
      }
   }
}

/// passthrough to UpdateCurrentIndex
void VaAttachmentWingRibbon::Update(double aTime)
{
   UpdateCurrentIndex(static_cast<float>(aTime));
}

/*************************************************************************************************
 * @brief   Removes all data points the were created before the specified time
 * @param   aTime The time.
 * @date 7/14/2017
 **************************************************************************************************/
void VaAttachmentWingRibbon::PruneBefore(float aTime)
{
   while (mCircular.mSize > 0 && mVerts[mCircular.mStart].mTime < aTime)
   {
      float mainStateVal = mVerts[mCircular.mStart].mStateValMain;
      float edgeStateVal = mVerts[mCircular.mStart].mStateValEdge;
      mCircular.PopFront();
      AdjustMinMaxLimitsForRemoval(edgeStateVal, mainStateVal);

      // Update the Current Time Index when we remove an element from the Circular Buffer.
      // Note: mIndexAtCurrentTime = -1 is not a valid index.
      //       Make sure mIndexAtCurrentTime is not less than 0
      mIndexAtCurrentTime--;
      if (mIndexAtCurrentTime < 0)
      {
         mIndexAtCurrentTime = mCircular.mSize - 1;
         if (mIndexAtCurrentTime < 0)
         {
            mIndexAtCurrentTime = 0;
         }
      }
   }

   if (mCircular.mSize == 0)
   {
      mIndexAtCurrentTime = 0;
   }
}

/*************************************************************************************************
 * @brief   Removes all data points that were created after the specified time
 * @param   aTime The time.
 * @date 7/14/2017
 **************************************************************************************************/
void VaAttachmentWingRibbon::PruneAfter(float aTime)
{
   while (mCircular.mSize > 0 && mVerts[mCircular.BackIndex()].mTime > aTime)
   {
      float mainStateVal = mVerts[mCircular.BackIndex()].mStateValMain;
      float edgeStateVal = mVerts[mCircular.BackIndex()].mStateValEdge;
      mCircular.PopBack();
      AdjustMinMaxLimitsForRemoval(edgeStateVal, mainStateVal);
   }
}

void VaAttachmentWingRibbon::UpdateFrame(double aTime)
{
   UpdateCurrentIndex(static_cast<float>(aTime));
   vespa::VaAttachment::UpdateFrame(aTime);
}
/// sets the mainMaxColor uniform of the wing ribbon shaders
void VaAttachmentWingRibbon::SetMainMaxColor(const UtColor& aMainMaxColor)
{
   float r, g, b, a;
   aMainMaxColor.Get(r, g, b, a);
   mShapePtr->SetUniform4f("mainMaxColor", r, g, b, a);
}
/// sets the mainMinColor uniform of the wing ribbon shaders
void VaAttachmentWingRibbon::SetMainMinColor(const UtColor& aMainMinColor)
{
   float r, g, b, a;
   aMainMinColor.Get(r, g, b, a);
   mShapePtr->SetUniform4f("mainMinColor", r, g, b, a);
}
/// sets the edgeMaxColor uniform of the wing ribbon shaders
void VaAttachmentWingRibbon::SetEdgeMaxColor(const UtColor& aEdgeMaxColor)
{
   float r, g, b, a;
   aEdgeMaxColor.Get(r, g, b, a);
   mShapePtr->SetUniform4f("edgeMaxColor", r, g, b, a);
}
/// sets the edgeMinColor uniform of the wing ribbon shaders
void VaAttachmentWingRibbon::SetEdgeMinColor(const UtColor& aEdgeMinColor)
{
   float r, g, b, a;
   aEdgeMinColor.Get(r, g, b, a);
   mShapePtr->SetUniform4f("edgeMinColor", r, g, b, a);
}
/// sets the mainMaxFlippedColor uniform of the wing ribbon shaders
void VaAttachmentWingRibbon::SetMainMaxFlippedColor(const UtColor& aMainMaxFlippedColor)
{
   float r, g, b, a;
   aMainMaxFlippedColor.Get(r, g, b, a);
   mShapePtr->SetUniform4f("mainMaxFlippedColor", r, g, b, a);
}
/// sets the mainMinFlippedColor uniform of the wing ribbon shaders
void VaAttachmentWingRibbon::SetMainMinFlippedColor(const UtColor& aMainMinFlippedColor)
{
   float r, g, b, a;
   aMainMinFlippedColor.Get(r, g, b, a);
   mShapePtr->SetUniform4f("mainMinFlippedColor", r, g, b, a);
}
/// sets the edgeMaxFlippedColor uniform of the wing ribbon shaders
void VaAttachmentWingRibbon::SetEdgeMaxFlippedColor(const UtColor& aEdgeMaxFlippedColor)
{
   float r, g, b, a;
   aEdgeMaxFlippedColor.Get(r, g, b, a);
   mShapePtr->SetUniform4f("edgeMaxFlippedColor", r, g, b, a);
}
/// sets the edgeMinFlippedColor uniform of the wing ribbon shaders
void VaAttachmentWingRibbon::SetEdgeMinFlippedColor(const UtColor& aEdgeMinFlippedColor)
{
   float r, g, b, a;
   aEdgeMinFlippedColor.Get(r, g, b, a);
   mShapePtr->SetUniform4f("edgeMinFlippedColor", r, g, b, a);
}

//! sets the edge limit type to determine if the attachment needs to keep track of the current
//! min/max values that it has recorded for the edge state vals
void VaAttachmentWingRibbon::SetEdgeLimitType(InterpolationLimitsMethod aEdgeLimitType)
{
   mEdgeLimitType = aEdgeLimitType;
   if (aEdgeLimitType != eHARD_LIMITS)
   {
      RecalculateEdgeMinMaxLimits();
   }
}
//! sets the edge limit type to determine if the attachment needs to keep track of the current
//! min/max values that it has recorded for the main state vals.
void VaAttachmentWingRibbon::SetMainLimitType(InterpolationLimitsMethod aMainLimitType)
{
   mMainLimitType = aMainLimitType;
   if (aMainLimitType != eHARD_LIMITS)
   {
      RecalculateMainMinMaxLimits();
   }
}
//! sets the currently tracked max main state val and the maxMainLimit uniform of the wing ribbon shaders
void VaAttachmentWingRibbon::SetHardMaxMainLimit(float aHardMaxMainLimit)
{
   mCurrentMainMaxStateVal = aHardMaxMainLimit;
   if (aHardMaxMainLimit == mCurrentMainMinStateVal)
   {
      aHardMaxMainLimit += .0001f;
   } // prevent division by zero in shader
   mShapePtr->SetUniformf("maxMainLimit", aHardMaxMainLimit);
}
//! sets the currently tracked min main state val and the minMinLimit uniform of the wing ribbon shaders
void VaAttachmentWingRibbon::SetHardMinMainLimit(float aHardMinMainLimit)
{
   mCurrentMainMinStateVal = aHardMinMainLimit;
   if (aHardMinMainLimit == mCurrentMainMaxStateVal)
   {
      aHardMinMainLimit -= 0.0001f;
   } // prevent division by zero in shader
   mShapePtr->SetUniformf("minMainLimit", aHardMinMainLimit);
}
//! sets the currently tracked max edge state val and  the maxEdgeLimit uniform of the wing ribbon shaders
void VaAttachmentWingRibbon::SetHardMaxEdgeLimit(float aHardMaxEdgeLimit)
{
   mCurrentEdgeMaxStateVal = aHardMaxEdgeLimit;
   if (aHardMaxEdgeLimit == mCurrentEdgeMinStateVal)
   {
      aHardMaxEdgeLimit += 0.0001f;
   } // prevent division by zero in shader
   mShapePtr->SetUniformf("maxEdgeLimit", aHardMaxEdgeLimit);
}
//! sets the currently tracked min edge state val and the minEdgeLimit uniform of the wing ribbon shaders
void VaAttachmentWingRibbon::SetHardMinEdgeLimit(float aHardMinEdgeLimit)
{
   mCurrentEdgeMinStateVal = aHardMinEdgeLimit;
   if (aHardMinEdgeLimit == mCurrentEdgeMaxStateVal)
   {
      aHardMinEdgeLimit -= 0.0001f;
   } // prevent division by zero in shader
   mShapePtr->SetUniformf("minEdgeLimit", aHardMinEdgeLimit);
}
/// sets the maxMainLimit uniform of the wing ribbon shaders
void VaAttachmentWingRibbon::SetScenarioMaxMainLimit(float aScenarioMaxMainLimit)
{
   if (aScenarioMaxMainLimit == mCurrentMainMinStateVal)
   {
      aScenarioMaxMainLimit += .0001f;
   } // prevent division by zero in shader
   mShapePtr->SetUniformf("maxMainLimit", aScenarioMaxMainLimit);
}
/// sets the minMainLimit uniform of the wing ribbon shaders
void VaAttachmentWingRibbon::SetScenarioMinMainLimit(float aScenarioMinMainLimit)
{
   if (aScenarioMinMainLimit == mCurrentMainMaxStateVal)
   {
      aScenarioMinMainLimit -= 0.0001f;
   } // prevent division by zero in shader
   mShapePtr->SetUniformf("minMainLimit", aScenarioMinMainLimit);
}
/// sets the maxEdgeLimit uniform of the wing ribbon shaders
void VaAttachmentWingRibbon::SetScenarioMaxEdgeLimit(float aScenarioMaxEdgeLimit)
{
   if (aScenarioMaxEdgeLimit == mCurrentEdgeMinStateVal)
   {
      aScenarioMaxEdgeLimit += 0.0001f;
   } // prevent division by zero in shader
   mShapePtr->SetUniformf("maxEdgeLimit", aScenarioMaxEdgeLimit);
}
/// sets the minEdgeLimit uniform of the wing ribbon shaders
void VaAttachmentWingRibbon::SetScenarioMinEdgeLimit(float aScenarioMinEdgeLimit)
{
   if (aScenarioMinEdgeLimit == mCurrentEdgeMaxStateVal)
   {
      aScenarioMinEdgeLimit -= 0.0001f;
   } // prevent division by zero in shader
   mShapePtr->SetUniformf("minEdgeLimit", aScenarioMinEdgeLimit);
}

void VaAttachmentWingRibbon::SetWidthScale(float aWidthScale)
{
   mWidthScale            = aWidthScale;
   mScreenSpaceScaleDirty = true;
}
/// sets the hardEdgeWidth uniform of the wing ribbon shaders
void VaAttachmentWingRibbon::SetHardEdgeWidth(float aHardEdgeWidth)
{
   mShapePtr->SetUniformf("hardEdgeWidth", aHardEdgeWidth);
}
/// sets the softEdgeWidth uniform of the wing ribbon shaders
void VaAttachmentWingRibbon::SetSoftEdgeWidth(float aSoftEdgeWidth)
{
   mShapePtr->SetUniformf("softEdgeWidth", aSoftEdgeWidth);
}
/// set the screen scale to match the screen scale of the parent's icon
void VaAttachmentWingRibbon::SetVA_ScreenScale(float aVA_ScreenScale)
{
   mVA_ScreenScale        = aVA_ScreenScale;
   mScreenSpaceScaleDirty = true;
}

float VaAttachmentWingRibbon::GetCurrentMainMaxStateVal()
{
   return mCurrentMainMaxStateVal;
}

float VaAttachmentWingRibbon::GetCurrentMainMinStateVal()
{
   return mCurrentMainMinStateVal;
}

float VaAttachmentWingRibbon::GetCurrentEdgeMaxStateVal()
{
   return mCurrentEdgeMaxStateVal;
}

float VaAttachmentWingRibbon::GetCurrentEdgeMinStateVal()
{
   return mCurrentEdgeMinStateVal;
}
} // namespace vespa
