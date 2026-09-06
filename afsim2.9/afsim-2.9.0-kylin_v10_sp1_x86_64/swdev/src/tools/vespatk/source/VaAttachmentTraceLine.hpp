// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2014 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef VAATTACHMENTTRACELINE_HPP
#define VAATTACHMENTTRACELINE_HPP

#include <osg/Array>

#include "UtCircularBufferIndex.hpp"
#include "UtColor.hpp"
#include "VaAttachment.hpp"
#include "VaFwd.hpp"

class UtoMapProjection;
class UtoRawShape;

namespace osg
{
class Program;
}

namespace vespa
{
class VaAttachmentTraceLine_LinesDrawable;
//! A trace-line attached to an entity.  This is different than VaAttachmentTraceLine in that:
//!   -Points can be added or removed from the beginning or end
//!   -Segments are mostly drawn in ECEF coordinates, and do not need to be retransformed
//!   -Other attempts are made at improving efficiency
class VESPATK_EXPORT VaAttachmentTraceLine : public VaAttachment
{
   struct Range
   {
      Range()
         : mStart(-1)
         , mEnd(-1)
      {
      }
      Range(int aStart, int aEnd)
         : mStart(aStart)
         , mEnd(aEnd)
      {
      }
      int  mStart;
      int  mEnd;
      bool operator<(const Range& aRange) const { return mStart < aRange.mStart; }
   };
   struct RangeSet
   {
      void AddPoint(int aIndex);
      // Combine adjacent or intersecting ranges
      void               Reduce();
      void               Clear() { mRanges.clear(); }
      std::vector<Range> mRanges;
   };

public:
   friend class VaAttachmentTraceLine_LinesDrawable;
   VaAttachmentTraceLine(VaEntity& aEntity, VaViewer* aViewerPtr);

   void SetStaticColor(const UtColor& aColor);
   void SetStateColoration();

   void SetColorArray(const std::vector<UtColor>& aColors);

   void SetLineWidth(unsigned int aWidth);

   void AddPointBack(const float aTime, const double aPosition[3]);
   void AddPointFront(const float aTime, const double aPosition[3]);
   void AddStateBack(const float aTime, const float aState);
   void AddStateFront(const float aTime, const float aState);
   void PopFrontState();

   void PruneBefore(float aTime);
   void PruneAfter(float aTime);

   void Reset();

   void ReportContents(std::ostream& aStream);
   struct Vertex
   {
      float mData[4]; // posx, posy, posz, time
                      //            osg::Vec3f              mPos;
                      //            float                   mTime;
   };
   typedef std::vector<Vertex> VertexArray;

   struct State
   {
      float mTime;
      float mState;
   };
   typedef std::vector<State> StateArray;

private:
   void UpdateCurrentIndex(float aTime);
   void PrivateLoad() override;

   void Update(double aTime) override;

   void UpdateFrame(double aTime) override;


   float mLastUpdate;

   ///////////////////////////////////////////////////////////
   // Circular buffers
   typedef UtCircularBufferIndex CircularBufferIndex;


   int CirclePushFront()
   {
      CheckSpace(mCircular.mSize + 1);
      return mCircular.PushFront();
   }
   int CirclePushBack()
   {
      CheckSpace(mCircular.mSize + 1);
      return mCircular.PushBack();
   }
   void CheckSpace(int aNewSize)
   {
      if (aNewSize > mCircular.mBufferSize)
      {
         GrowSpace(aNewSize);
      }
   }
   void GrowSpace(int aNewSize);

   int StatePushFront()
   {
      CheckStateSpace(mStateCircular.mSize + 1);
      return mStateCircular.PushFront();
   }
   int StatePushBack()
   {
      CheckStateSpace(mStateCircular.mSize + 1);
      return mStateCircular.PushBack();
   }
   void CheckStateSpace(int aNewSize)
   {
      if (aNewSize > mStateCircular.mBufferSize)
      {
         GrowStateSpace(aNewSize);
      }
   }
   void GrowStateSpace(int aNewSize);

   CircularBufferIndex mCircular;
   RangeSet            mDirtyIndices;

   VertexArray mVerts;

   typedef UtCircularBufferIndex::iterator CircularIterator;

   int                  mIndexAtCurrentTime;
   UtoShape*            mShapePtr;
   float                mCurrentTime;
   static osg::Program* sOsgProgram;

   bool                mManagingStateBuffer;
   CircularBufferIndex mStateCircular;
   StateArray          mStates;
   RangeSet            mDirtyStates;

   bool mIsLoaded;
};

} // namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaAttachmentTraceLine)
#endif
