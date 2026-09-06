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

#ifndef VAATTACHMENTWingRibbon_HPP
#define VAATTACHMENTWingRibbon_HPP

#include <osg/Array>

#include "UtCircularBufferIndex.hpp"
#include "UtColor.hpp"
#include "VaAttachment.hpp"


class UtoRawShape;
namespace osg
{
class Program;
}

namespace vespa
{
class VaAttachmentWingRibbon_Drawable;

class VESPATK_EXPORT VaAttachmentWingRibbon : public VaAttachment
{
public:
   enum InterpolationLimitsMethod
   {
      eHARD_LIMITS = 0, //!< the colors are determined based on clamping state values inside a user defined range
      eCURRENT_INDIVIDUAL_LIMITS =
         1, //!< the colors are determined based on the current min/max values in the state buffer of the wingribbon
      eCURRENT_SCENARIO_LIMITS =
         2, //!< the colors are determined based on min/max value in state buffers across all wingribbons
      eINDEFINATE_INDIVIDUAL_LIMITS = 3, //!< colors determined on min/max value of all states ever added to the
                                         //!< wingribbon (in the time that this LimitsMethod was set)
      eINDEFINATE_SCENARIO_LIMITS = 4,   //!< colors determined on min/max value of all states ever added across all
                                         //!< wingribbons (in the time that this LimitsMethod was set)
   };

   VaAttachmentWingRibbon(VaEntity& aEntity, VaViewer* aViewerPtr);

   void CopyPointsInto(VaAttachmentWingRibbon& aCopiedRibbon) const;

   void AddPointBack(const float       aTime,
                     const double      aPosition[3],
                     const float       aMainStateVal,
                     const float       aEdgeStateVal,
                     const osg::Vec3f& wingTipA,
                     const osg::Vec3f& wingTipB,
                     const bool        aIsAlive);
   void AddPointFront(const float       aTime,
                      const double      aPosition[3],
                      const float       aMainStateVal,
                      const float       aEdgeStateVal,
                      const osg::Vec3f& wingTipA,
                      const osg::Vec3f& wingTipB,
                      const bool        aIsAlive);

   void PruneBefore(float aTime);
   void PruneAfter(float aTime);

   //! @name passthroughs to update glsl uniforms.
   //@{
   void SetMainMaxColor(const UtColor& aMainMaxColor);
   void SetMainMinColor(const UtColor& aMainMinColor);
   void SetEdgeMaxColor(const UtColor& aEdgeMaxColor);
   void SetEdgeMinColor(const UtColor& aEdgeMinColor);
   void SetMainMaxFlippedColor(const UtColor& aMainMaxFlippedColor);
   void SetMainMinFlippedColor(const UtColor& aMainMinFlippedColor);
   void SetEdgeMaxFlippedColor(const UtColor& aEdgeMaxFlippedColor);
   void SetEdgeMinFlippedColor(const UtColor& aEdgeMinFlippedColor);
   void SetScenarioMaxMainLimit(float aScenarioMaxMainLimit);
   void SetScenarioMinMainLimit(float aScenarioMinMainLimit);
   void SetScenarioMaxEdgeLimit(float aScenarioMaxEdgeLimit);
   void SetScenarioMinEdgeLimit(float aScenarioMinEdgeLimit);
   void SetHardEdgeWidth(float aHardEdgeWidth);
   void SetSoftEdgeWidth(float aSoftEdgeWidth);
   //@}

   void SetWidthScale(float aWidthScale);

   void SetVA_ScreenScale(float aVA_ScreenScale);

   static void SetTransparencyOnDeath(float aAlphaOnDeath) { sTransparencyOnDeath = aAlphaOnDeath; }

   //! @name limit functions.
   //! for use in the calculation/usage of the min/max recorded values.
   //@{
   void  SetEdgeLimitType(InterpolationLimitsMethod aEdgeLimitType);
   void  SetMainLimitType(InterpolationLimitsMethod aMainLimitType);
   float GetCurrentMainMaxStateVal();
   float GetCurrentMainMinStateVal();
   float GetCurrentEdgeMaxStateVal();
   float GetCurrentEdgeMinStateVal();
   void  RecalculateEdgeMinMaxLimits();
   void  RecalculateMainMinMaxLimits();
   void  SetHardMaxMainLimit(float aHardMaxMainLimit);
   void  SetHardMinMainLimit(float aHardMinMainLimit);
   void  SetHardMaxEdgeLimit(float aHardMaxEdgeLimit);
   void  SetHardMinEdgeLimit(float aHardMinEdgeLimit);
   //@}

   // memory handling
   void shrink_to_fit();

   bool Highlight(VaViewer* aViewerPtr, const VaAttachmentSubId& aSubpartId, bool aHighlight, const UtColor& aColor) override;

private:
   void UpdateCurrentIndex(float aTime);
   void PrivateLoad() override;

   void PrivateInitializeCallbacks() override;
   void Update(double aTime) override;

   void UpdateFrame(double aTime) override;

   void ReCalculateScreenSpaceScale();

   void AdjustMinMaxLimitsForRemoval(float aEdgeStateValBeingRemoved, float aMainStateValBeingRemoved);
   void AdjustMinMaxLimitsForInsert(float aEdgeStateValBeingInserted, float aMainStateValBeingInserted);


   //! @name Circular buffers
   //@{
   typedef UtCircularBufferIndex CircularBufferIndex;

   static constexpr float cBUFFER_GROWTH_FACTOR = 1.4f;
   int                    CirclePushFront()
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
   //@}

   //! @name Ranges
   //@{
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
      void               Clear();
      std::vector<Range> mRanges;
   };
   //@}

   struct Vertex
   {
      // NOTE: order of members is important, due to calls of gl*Pointer in VaAttachmentWingRibbon_Drawable::draw()
      osg::Vec3f mPos;
      float      mTime;
      osg::Vec3f mWingTipA;
      float      mStateValMain;
      osg::Vec3f mWingTipB;
      float      mStateValEdge;
      float      mIsAlive;
   };
   typedef std::vector<Vertex> VertexArray;

   CircularBufferIndex mCircular;
   RangeSet            mDirtyIndices;

   VertexArray mVerts;

   typedef UtCircularBufferIndex::iterator CircularIterator;

   int                  mIndexAtCurrentTime;
   UtoShape*            mShapePtr;
   float                mCurrentTime;
   static osg::Program* sOsgProgram;
   static float         sTransparencyOnDeath; //!< the alpha value used when a platform is dead

   bool mIsLoaded;
   bool mScreenSpaceScaleDirty;

   // configurable variables
   InterpolationLimitsMethod mEdgeLimitType;
   InterpolationLimitsMethod mMainLimitType;
   float                     mCurrentEdgeMaxStateVal;
   float                     mCurrentEdgeMinStateVal;
   float                     mCurrentMainMaxStateVal;
   float                     mCurrentMainMinStateVal;

   float mWidthScale;
   float mVA_ScreenScale;

   friend class VaAttachmentWingRibbon_Drawable;
};
} // namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaAttachmentWingRibbon)

#endif
