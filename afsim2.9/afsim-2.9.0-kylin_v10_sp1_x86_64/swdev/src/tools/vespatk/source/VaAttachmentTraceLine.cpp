// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2017 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "VaAttachmentTraceLine.hpp"

#include <osg/Array>
#include <osg/BlendFunc>
#include <osg/BufferObject>
#include <osg/Camera>
#include <osg/Depth>
#include <osg/Drawable>
#include <osg/GL2Extensions>
#include <osg/Geode>
#include <osg/LineWidth>

#include "UtoRawShape.hpp"
#include "UtoShaders.hpp"
#include "VaEntity.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"

namespace
{
constexpr int cVERTEX_SIZE = sizeof(vespa::VaAttachmentTraceLine::Vertex);
constexpr int cSTATE_SIZE  = sizeof(vespa::VaAttachmentTraceLine::State);
} // namespace
namespace vespa
{
osg::Program* VaAttachmentTraceLine::sOsgProgram = nullptr;

class VaAttachmentTraceLine_LinesDrawable : public osg::Drawable
{
public:
   mutable int                 mBufferSize;
   mutable unsigned int        mBufferId;
   mutable int                 mStateBufferSize;
   mutable unsigned int        mStateBufferId;
   VaAttachmentTraceLine*      mAttachment;
   mutable osg::GL2Extensions* mExt;

   VaAttachmentTraceLine_LinesDrawable(VaAttachmentTraceLine* aAttachment)
   {
      mBufferSize       = -1;
      mStateBufferSize  = -1;
      mAttachment       = aAttachment;
      osg::StateSet* ss = getOrCreateStateSet();
      ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
      setSupportsDisplayList(false);
      mExt = nullptr;
      ss->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
      osg::Depth* depth = new osg::Depth();
      depth->setWriteMask(false);
      ss->setAttributeAndModes(depth);
      ss->setAttributeAndModes(new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
      ss->setRenderBinDetails(2000, "RenderBin"); // draw it late, so that we can skip the write to the depth buffer
                                                  // with minimal artifacting
      ss->setNestRenderBins(false);
   }
   ~VaAttachmentTraceLine_LinesDrawable() override
   {
      if (mExt)
      {
         mExt->glDeleteBuffers(1, &mBufferId);
         mExt->glDeleteBuffers(1, &mStateBufferId);
      }
   }
   void firstTimeInit(osg::RenderInfo& renderInfo) const
   {
      mExt = osg::GL2Extensions::Get(renderInfo.getContextID(), true);
      mExt->glGenBuffers(1, &mBufferId);
      mExt->glGenBuffers(1, &mStateBufferId);
   }
   void WriteBuffer() const
   {
      VaAttachmentTraceLine& p            = *mAttachment;
      int                    requiredSize = p.mCircular.mBufferSize * sizeof(VaAttachmentTraceLine::Vertex);
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
#ifdef _WIN32
      requiredSize = p.mStateCircular.mBufferSize * sizeof(VaAttachmentTraceLine::State);
      if (mStateBufferSize != requiredSize)
      {
         mStateBufferSize = requiredSize;
         mExt->glBindBuffer(GL_SHADER_STORAGE_BUFFER, mStateBufferId);
         mExt->glBufferData(GL_SHADER_STORAGE_BUFFER, p.mStateCircular.mBufferSize * cSTATE_SIZE, &p.mStates[0], GL_STATIC_DRAW);
         mExt->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
         p.mDirtyStates.Clear();
      }
      else
      {
         if (!p.mDirtyStates.mRanges.empty())
         {
            mExt->glBindBuffer(GL_SHADER_STORAGE_BUFFER, mStateBufferId);
            p.mDirtyStates.Reduce();
            for (size_t i = 0; i < p.mDirtyStates.mRanges.size(); ++i)
            {
               int start = p.mDirtyStates.mRanges[i].mStart;
               int end   = p.mDirtyStates.mRanges[i].mEnd;
               mExt->glBufferSubData(GL_SHADER_STORAGE_BUFFER,
                                     start * cSTATE_SIZE,
                                     (end - start) * cSTATE_SIZE,
                                     &p.mStates[start]);
            }
            mExt->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
            p.mDirtyStates.Clear();
         }
      }
#endif
   }
   void Draw(osg::RenderInfo& renderInfo) const
   {
      VaAttachmentTraceLine& p = *mAttachment;
      if (p.mCircular.mSize == 0)
      {
         return;
      }

      WriteBuffer();
#ifdef _WIN32
      mExt->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, mStateBufferId);
#endif
      mExt->glBindBuffer(GL_ARRAY_BUFFER_ARB, mBufferId);

      // glTexCoordPointer(1, GL_FLOAT, sizeof(GLfloat) * 4, reinterpret_cast<const GLvoid*>(3 * sizeof(float)));
      glVertexPointer(4, GL_FLOAT, sizeof(GLfloat) * 4, nullptr);
      int pointsRemain = std::min(p.mIndexAtCurrentTime + 1, p.mCircular.mSize);
      int firstPoints  = p.mCircular.End1() - p.mCircular.mStart;
      if (pointsRemain < firstPoints)
      {
         firstPoints = pointsRemain;
      }
      pointsRemain -= firstPoints;

      glEnableClientState(GL_VERTEX_ARRAY);

      glDrawArrays(GL_LINE_STRIP, p.mCircular.mStart, firstPoints);

      if (p.mCircular.Wraps())
      {
         unsigned int temp[2] = {p.mCircular.End1() - 1u, 0u};
         glDrawElements(GL_LINE_STRIP, 2, GL_UNSIGNED_INT, temp); // span the gap
         glDrawArrays(GL_LINE_STRIP, 0, pointsRemain);
      }
      glDisableClientState(GL_VERTEX_ARRAY);

      mExt->glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
#ifdef _WIN32
      mExt->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
#endif

      if (p.mIndexAtCurrentTime > 0)
      {
         glBegin(GL_LINE_STRIP);
         int endIdx = p.mCircular.IndexToOffset(p.mIndexAtCurrentTime);
         glVertex4fv((float*)&p.mVerts[endIdx].mData);
         double ecef[4];
         p.GetParent().GetPosition().GetECEF(ecef);
         ecef[3] = p.mCurrentTime;
         glVertex4dv(ecef);
         glEnd();
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
      osg::Matrixd viewMatrix = renderInfo.getCurrentCamera()->getViewMatrix();
      Draw(renderInfo);
   }
   Object* cloneType() const override { return new VaAttachmentTraceLine_LinesDrawable(*this); }
   Object* clone(const osg::CopyOp&) const override { return new VaAttachmentTraceLine_LinesDrawable(*this); }
};

void VaAttachmentTraceLine::RangeSet::AddPoint(int aIndex)
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
void VaAttachmentTraceLine::RangeSet::Reduce()
{
   std::sort(mRanges.begin(), mRanges.end());
   for (int i = -2 + (int)mRanges.size(); i >= 0; --i)
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

VaAttachmentTraceLine::VaAttachmentTraceLine(VaEntity& aEntity, VaViewer* aViewerPtr)
   : VaAttachment(aEntity, aViewerPtr, "trace_line")
   , mLastUpdate(-1.0)
   , mIndexAtCurrentTime(0)
   , mShapePtr(nullptr)
   , mManagingStateBuffer(false)
   , mIsLoaded(false)
{
   SetType<VaAttachmentTraceLine>();
   mCircular.mStart      = 0;
   mCircular.mSize       = 0;
   mCircular.mBufferSize = 100;
   mVerts.resize(mCircular.mBufferSize);
   mStateCircular.mStart      = 0;
   mStateCircular.mSize       = 0;
   mStateCircular.mBufferSize = 100;
   mStates.resize(mStateCircular.mBufferSize);
}

// we need more memory
void VaAttachmentTraceLine::GrowSpace(int aNewSize)
{
   int         arraySize = static_cast<int>(aNewSize * 1.4f); // Grow by ~40%
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

// we need more memory
void VaAttachmentTraceLine::GrowStateSpace(int aNewSize)
{
   int        arraySize = static_cast<int>(aNewSize * 1.4f);
   StateArray newStates(arraySize);
   int        stateCount1 = (mStateCircular.End1() - mStateCircular.mStart);

   memcpy(&newStates[0], &mStates[mStateCircular.mStart], stateCount1 * sizeof(State));
   if (mStateCircular.Wraps())
   {
      memcpy(&newStates[stateCount1], &mStates[0], mStateCircular.End2() * sizeof(State));
   }

   mStateCircular.mStart      = 0;
   mStateCircular.mBufferSize = arraySize;
   mDirtyStates.Clear();
   mShapePtr->SetUniform3i("stateCircular", mStateCircular.mStart, mStateCircular.mSize, mStateCircular.mBufferSize);

   std::swap(mStates, newStates);
}

void VaAttachmentTraceLine::AddPointBack(const float aTime, const double aPosition[3])
{
   int idx              = CirclePushBack();
   mVerts[idx].mData[0] = aPosition[0];
   mVerts[idx].mData[1] = aPosition[1];
   mVerts[idx].mData[2] = aPosition[2]; // .set(aPosition[0], aPosition[1], aPosition[2]);
   mVerts[idx].mData[3] = aTime;
   mDirtyIndices.AddPoint(idx);
}


void VaAttachmentTraceLine::AddPointFront(const float aTime, const double aPosition[3])
{
   int idx = CirclePushFront();
   //      mVerts[idx].mPos.set(aPosition[0], aPosition[1], aPosition[2]);
   //      mVerts[idx].mTime = aTime;

   mVerts[idx].mData[0] = aPosition[0];
   mVerts[idx].mData[1] = aPosition[1];
   mVerts[idx].mData[2] = aPosition[2]; // .set(aPosition[0], aPosition[1], aPosition[2]);
   mVerts[idx].mData[3] = aTime;
   mDirtyIndices.AddPoint(idx);
}

void VaAttachmentTraceLine::AddStateBack(const float aTime, const float aState)
{
   mManagingStateBuffer = true;
   int idx              = StatePushBack();
   mStates[idx].mState  = aState;
   mStates[idx].mTime   = aTime;
   mDirtyStates.AddPoint(idx);
   mShapePtr->SetUniform3i("stateCircular", mStateCircular.mStart, mStateCircular.mSize, mStateCircular.mBufferSize);
}

void VaAttachmentTraceLine::AddStateFront(const float aTime, const float aState)
{
   mManagingStateBuffer = true;
   int idx              = StatePushFront();
   mStates[idx].mState  = aState;
   mStates[idx].mTime   = aTime;
   mDirtyStates.AddPoint(idx);
   // we need to know what happened before this..
   // maybe this is the applications responsibility
   mShapePtr->SetUniform3i("stateCircular", mStateCircular.mStart, mStateCircular.mSize, mStateCircular.mBufferSize);
}

void VaAttachmentTraceLine::PrivateLoad()
{
   if (!mIsLoaded)
   {
      mIsLoaded = true;

      osg::Geode* geode = new osg::Geode;
      geode->setCullingActive(false); // using shaders!
      geode->addDrawable(new VaAttachmentTraceLine_LinesDrawable(this));
      UtoRawShape raw;
      raw.SetReferenceFrame(UtoShape::cFRAME_WORLD);
      mShapePtr = AddShapeToParent(std::string("trace" + std::to_string(GetUniqueId())), raw);
      mShapePtr->GetTree()->addChild(geode);
      mShapePtr->SetUniform3i("stateCircular", mStateCircular.mStart, mStateCircular.mSize, mStateCircular.mBufferSize);

      osg::Uniform* colorArray =
         mShapePtr->GetTree()->getOrCreateStateSet()->getOrCreateUniform("stateColors", osg::Uniform::Type::FLOAT_VEC4, 16U);
      colorArray->setElement(0, osg::Vec4(1.0F, 0.0F, 1.0F, 1.0F));
      colorArray->setElement(1, osg::Vec4(1.0F, 1.0F, 1.0F, 1.0F));
      colorArray->setElement(2, osg::Vec4(0.8F, 0.8F, 0.0F, 1.0F));
      colorArray->setElement(3, osg::Vec4(1.0F, 0.0F, 0.0F, 1.0F));
      colorArray->setElement(4, osg::Vec4(0.0F, 0.0F, 0.0F, 1.0F));
      for (int i = 5; i < 16; ++i)
      {
         colorArray->setElement(i, osg::Vec4(0.0F, 0.0F, 0.0F, 1.0F));
      }

      if (!sOsgProgram)
      {
         std::set<std::string> defset;
         std::string           type = VaEnvironment::Instance().GetRecommendedGLSL_ShaderString();
#ifndef _WIN32
         defset.insert("NO_GEOMETRY");
         sOsgProgram = UtoShaders::UseProgram("traceline" + type + ".vert", "traceline" + type + ".frag", defset);
#else
         sOsgProgram = UtoShaders::UseProgramWithGeometry("traceline.geom",
                                                          "traceline" + type + ".vert",
                                                          "traceline" + type + ".frag",
                                                          defset);
#endif
         sOsgProgram->addBindAttribLocation("trace_Vertex", 0);
      }
      if (sOsgProgram)
      {
         mShapePtr->SetProgram(sOsgProgram, true);
      }
   }
}

void VaAttachmentTraceLine::UpdateCurrentIndex(float aTime)
{
   if (mShapePtr)
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
         if (mVerts[iter.GetOffset()].mData[3] > aTime)
         {
            for (;;)
            {
               if (iter.IsBegin() || mVerts[iter.GetOffset()].mData[3] <= aTime)
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
               if (mVerts[iter.GetOffset()].mData[3] > aTime)
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

void VaAttachmentTraceLine::Update(double aTime)
{
   UpdateCurrentIndex(static_cast<float>(aTime));
}

void VaAttachmentTraceLine::PruneBefore(float aTime)
{
   while (mCircular.mSize > 0 && mVerts[mCircular.mStart].mData[3] < aTime)
   {
      mCircular.PopFront();

      // Update the Current Time Index when we remove an element from the Circular Buffer.
      // Note: mIndexAtCurrentTime = -1 if mCircular.mSize = 0
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
   if (mManagingStateBuffer)
   {
      float frontState = -1.0f;
      while (mStateCircular.mSize > 0 && mStates[mStateCircular.mStart].mTime < aTime)
      {
         frontState = mStates[mStateCircular.mStart].mState;
         mStateCircular.PopFront();
      }
      if (frontState >= 0.0f) // if a state fell off the front end
      {
         // restore it to time "zero"... we don't need to check for space
         // because we are certain something was popped
         int idx             = mStateCircular.PushFront();
         mStates[idx].mTime  = aTime;
         mStates[idx].mState = frontState;
      }
      mShapePtr->SetUniform3i("stateCircular", mStateCircular.mStart, mStateCircular.mSize, mStateCircular.mBufferSize);
   }
}

void VaAttachmentTraceLine::PopFrontState()
{
   if (mStateCircular.GetSize() > 0)
   {
      mStateCircular.PopFront();
      mShapePtr->SetUniform3i("stateCircular", mStateCircular.mStart, mStateCircular.mSize, mStateCircular.mBufferSize);
   }
}

void VaAttachmentTraceLine::PruneAfter(float aTime)
{
   while (mCircular.mSize > 0 && mVerts[mCircular.BackIndex()].mData[3] > aTime)
   {
      mCircular.PopBack();
   }
   if (mManagingStateBuffer)
   {
      while (mStateCircular.mSize > 0 && mStates[mStateCircular.BackIndex()].mTime > aTime)
      {
         mStateCircular.PopBack();
      }
      mShapePtr->SetUniform3i("stateCircular", mStateCircular.mStart, mStateCircular.mSize, mStateCircular.mBufferSize);
   }
}

void VaAttachmentTraceLine::Reset()
{
   mCircular.mStart = 1;
   mCircular.mSize  = 0;
   if (mManagingStateBuffer)
   {
      mStateCircular.mStart = 1;
      mStateCircular.mSize  = 0;
      mShapePtr->SetUniform3i("stateCircular", mStateCircular.mStart, mStateCircular.mSize, mStateCircular.mBufferSize);
   }
}

void VaAttachmentTraceLine::UpdateFrame(double aTime)
{
   UpdateCurrentIndex(static_cast<float>(aTime));
   vespa::VaAttachment::UpdateFrame(aTime);
}

void VaAttachmentTraceLine::SetStaticColor(const UtColor& aColor)
{
   if (mShapePtr)
   {
      mShapePtr->SetUniform4f("staticColor", aColor[0], aColor[1], aColor[2], 1.0);
   }
}

void VaAttachmentTraceLine::SetColorArray(const std::vector<UtColor>& aColors)
{
   osg::Uniform* colorArray =
      mShapePtr->GetTree()->getOrCreateStateSet()->getOrCreateUniform("stateColors", osg::Uniform::Type::FLOAT_VEC4, 16U);
   unsigned int i = 0;
   for (auto& it : aColors)
   {
      if (i < 16)
      {
         float rgba[4];
         it.Get(rgba);
         colorArray->setElement(i, osg::Vec4(rgba[0], rgba[1], rgba[2], rgba[3]));
      }
      ++i;
   }
   for (; i < 16; ++i)
   {
      colorArray->setElement(i, osg::Vec4(0.0F, 0.0F, 0.0F, 1.0F));
   }
}

void VaAttachmentTraceLine::SetStateColoration()
{
   if (mShapePtr)
   {
      mShapePtr->SetUniform4f("staticColor", 0.0, 0.0, 0.0, 0.0);
   }
}

void VaAttachmentTraceLine::SetLineWidth(unsigned int aWidth)
{
   if (mShapePtr)
   {
      osg::StateSet* ss = mShapePtr->GetTree()->getOrCreateStateSet();
      ss->setAttributeAndModes(new osg::LineWidth(aWidth));
   }
}

void VaAttachmentTraceLine::ReportContents(std::ostream& aStream)
{
   aStream << "=== vert buffer ===" << std::endl;
   for (int i = 0; i < mCircular.GetSize(); ++i)
   {
      int idx = (i + mCircular.mStart < mCircular.mBufferSize) ? i + mCircular.mStart :
                                                                 i + mCircular.mStart - mCircular.mBufferSize;
      aStream << mVerts[idx].mData[3] << " " << mVerts[idx].mData[0] << " " << mVerts[idx].mData[1] << " "
              << mVerts[idx].mData[2] << std::endl;
   }
   aStream << "=== state buffer ===" << std::endl;
   for (int i = 0; i < mStateCircular.GetSize(); ++i)
   {
      int idx = (i + mStateCircular.mStart < mStateCircular.mBufferSize) ?
                   i + mStateCircular.mStart :
                   i + mStateCircular.mStart - mStateCircular.mBufferSize;
      aStream << mStates[idx].mTime << " " << mStates[idx].mState << std::endl;
   }
}
} // namespace vespa
