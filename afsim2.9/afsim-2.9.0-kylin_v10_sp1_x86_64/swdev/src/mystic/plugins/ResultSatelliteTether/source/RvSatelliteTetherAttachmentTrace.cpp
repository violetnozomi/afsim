// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "RvSatelliteTetherAttachmentTrace.hpp"

#include <osg/Geometry>

#include "RvEnvironment.hpp"
#include "RvEventPipeUtil.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"
#include "UtMath.hpp"
#include "UtRelativeGeometry.hpp"
#include "UtoMemory.hpp"
#include "UtoRawShape.hpp"
#include "UtoShaders.hpp"
#include "VaEntity.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"
#include "VaTimeManager.hpp"
#include "VaViewer.hpp"

RvSatelliteTether::AttachmentTrace::AttachmentTrace(vespa::VaEntity& aParentEntity,
                                                    vespa::VaViewer& aViewer,
                                                    vespa::VaEntity& aRelativeToEntity,
                                                    const QColor&    aColor)
   : wkf::AttachmentSatelliteTetherTrace(aParentEntity, aViewer, aRelativeToEntity, aColor)
{
}

namespace
{
std::pair<float, float> LowDifference(const std::pair<float, float>& p1, const std::pair<float, float>& p2)
{
   if (p1.first < p2.first)
   {
      return std::make_pair(p1.first, (p1.second > p2.first) ? p2.first : p1.second);
   }
   return std::make_pair(-1.0f, -1.0f);
}
std::pair<float, float> HighDifference(const std::pair<float, float>& p1, const std::pair<float, float>& p2)
{
   if (p1.second > p2.second)
   {
      return std::make_pair((p1.first < p2.second) ? p2.second : p1.first, p1.second);
   }
   return std::make_pair(-1.0f, -1.0f);
}
bool Invalid(const std::pair<float, float>& aRange)
{
   if (aRange.first == aRange.second)
   {
      if (aRange.first == -1)
      {
         return true;
      }
   }
   return false;
}
void AddVertex(double                    aSimTime,
               double                    aDispSimTime,
               const rv::MsgEntityState& aFromState,
               const rv::MsgEntityState& aToState,
               osg::Vec3Array*           aVertexArray,
               osg::Vec4Array*           aColorArray,
               const osg::Vec4&          aFutureColor)
{
   UtEntity from;
   UtEntity to;
   rv::BuildUtEntity(aSimTime, aFromState, from);
   rv::BuildUtEntity(aSimTime, aToState, to);
   UtVec3d ric;
   UtRelativeGeometry::CalculateRIC(from, to, ric);
   aVertexArray->push_back(osg::Vec3(ric[0], ric[1], ric[2]));
   osg::Vec4 color(aFutureColor);
   if (aSimTime < aDispSimTime)
   {
      color.a() = 0.8f;
   }
   aColorArray->push_back(color);
}
} // namespace

void RvSatelliteTether::AttachmentTrace::UpdateData()
{
   rv::ResultData*         data            = rvEnv.GetData();
   std::pair<float, float> rangeOfInterest = data->GetCacheRange(); // this is the data we want, ideally
   std::pair<float, float> rangeLoaded     = GetTimeRange();
   std::pair<float, float> loCullRange =
      LowDifference(rangeLoaded,
                    rangeOfInterest); // this is what we will need to remove to make what we have into what we can get
   std::pair<float, float> hiCullRange = HighDifference(rangeLoaded, rangeOfInterest);

   std::pair<float, float> loAddRange =
      LowDifference(rangeOfInterest,
                    rangeLoaded); // this is what we will need to add to make what we have into what we can get
   std::pair<float, float> hiAddRange = HighDifference(rangeOfInterest, rangeLoaded);

   UpdateDataRange(loCullRange.second,
                   (hiCullRange.first == -1) ? rangeOfInterest.second + 1.0f : hiCullRange.first,
                   loAddRange,
                   hiAddRange);

   SetTimeRange(rangeOfInterest);
}

void RvSatelliteTether::AttachmentTrace::Update(double aTime)
{
   float alpha;
   for (auto&& t2i : mTimeToIndex)
   {
      if (t2i.first <= aTime)
      {
         alpha = 0.8f;
      }
      else
      {
         alpha = mFutureAlpha;
      }
      (*mColorArray)[t2i.second] = osg::Vec4(mColor.redF(), mColor.greenF(), mColor.blueF(), alpha);
   }
}

void RvSatelliteTether::AttachmentTrace::UpdateDataRange(float            aCullBefore,
                                                         float            aCullAfter,
                                                         const TimeRange& aAddBefore,
                                                         const TimeRange& aAddAfter)
{
   if ((aCullBefore < mTimeRange.first) && (aCullAfter > mTimeRange.second) && (Invalid(aAddBefore)) &&
       (Invalid(aAddAfter)))
   {
      return;
   }

   ResetDrawing();
   auto pset = ut::osg::make_ref<osg::DrawElementsUInt>(GL_LINE_STRIP);
   mGeometryPtr->addPrimitiveSet(pset);

   unsigned int decriment    = 0;
   unsigned int validIndices = 0;
   for (std::map<double, size_t>::iterator it = mTimeToIndex.begin(); it != mTimeToIndex.end();)
   {
      if (it->first <= aCullBefore)
      {
         mVertexArray->erase(mVertexArray->begin());
         mColorArray->erase(mColorArray->begin());
         decriment++;
         it = mTimeToIndex.erase(it);
      }
      else if (it->first < aCullAfter)
      {
         validIndices++;
         it->second -= decriment;
         ++it;
      }
      else
      {
         it = mTimeToIndex.erase(it);
      }
   }
   mVertexArray->resize(validIndices);
   mColorArray->resize(validIndices);

   rv::ResultData* data = rvEnv.GetData();
   if (data)
   {
      double              dispSimTime = data->GetSimTime();
      rv::ResultDb*       db          = data->GetDb();
      rv::ResultPlatform* p1          = db->FindPlatformByName(GetParent().GetName());
      rv::ResultPlatform* p2          = db->FindPlatformByName(mOriginEntityPtr->GetName());
      if (p1 && p2)
      {
         double simTime = 0.0;
         p1->BeginRead();
         p2->BeginRead();
         auto sa1 = p1->GetArray<rv::MsgEntityState>();
         auto sa2 = p2->GetArray<rv::MsgEntityState>();
         if ((sa1 != nullptr) && (sa2 != nullptr))
         {
            auto                         fit         = sa1->FindFirstIteratorAfter(aAddBefore.first);
            auto                         tit         = sa2->FindFirstIteratorAfter(aAddBefore.first);
            unsigned int                 insertLoc   = 0;
            osg::ref_ptr<osg::Vec3Array> inputVArray = new osg::Vec3Array;
            osg::ref_ptr<osg::Vec4Array> inputCArray = new osg::Vec4Array;
            osg::Vec4                    fColor(mColor.redF(), mColor.greenF(), mColor.blueF(), mFutureAlpha);
            while ((fit != sa1->end()) && (tit != sa2->end()))
            {
               auto fromStatePtr = dynamic_cast<rv::MsgEntityState*>(*fit);
               auto toStatePtr   = dynamic_cast<rv::MsgEntityState*>(*tit);
               if ((fromStatePtr->simTime() > aAddBefore.second) || (toStatePtr->simTime() > aAddBefore.second))
               {
                  break;
               }

               // add to start
               if (fromStatePtr->simTime() < toStatePtr->simTime())
               {
                  simTime = fromStatePtr->simTime();
                  if (mTimeToIndex.find(simTime) == mTimeToIndex.end())
                  {
                     AddVertex(simTime, dispSimTime, *fromStatePtr, *toStatePtr, inputVArray, inputCArray, fColor);
                     mTimeToIndex[simTime] = insertLoc;
                     ++insertLoc;
                  }
                  ++fit;
               }
               else
               {
                  simTime = toStatePtr->simTime();
                  if (mTimeToIndex.find(simTime) == mTimeToIndex.end())
                  {
                     AddVertex(simTime, dispSimTime, *fromStatePtr, *toStatePtr, inputVArray, inputCArray, fColor);
                     mTimeToIndex[simTime] = insertLoc;
                     ++insertLoc;
                  }

                  if (fromStatePtr->simTime() == toStatePtr->simTime())
                  {
                     ++fit;
                  }
                  ++tit;
               }
            }
            mVertexArray->insert(mVertexArray->begin(), inputVArray->begin(), inputVArray->end());
            mColorArray->insert(mColorArray->begin(), inputCArray->begin(), inputCArray->end());
            unsigned int i = 0;
            for (auto&& entry : mTimeToIndex)
            {
               if (i >= insertLoc)
               {
                  entry.second += insertLoc;
               }
               ++i;
            } // increment indices

            fit = sa1->FindFirstIteratorAfter(aAddAfter.first);
            tit = sa2->FindFirstIteratorAfter(aAddAfter.first);
            while ((fit != sa1->end()) && (tit != sa2->end()))
            {
               auto fromStatePtr = dynamic_cast<rv::MsgEntityState*>(*fit);
               auto toStatePtr   = dynamic_cast<rv::MsgEntityState*>(*tit);
               if ((fromStatePtr->simTime() > aAddAfter.second) || (toStatePtr->simTime() > aAddAfter.second))
               {
                  break;
               }

               // add to end
               if (fromStatePtr->simTime() < toStatePtr->simTime())
               {
                  simTime = fromStatePtr->simTime();
                  if (mTimeToIndex.find(simTime) == mTimeToIndex.end())
                  {
                     AddVertex(simTime, dispSimTime, *fromStatePtr, *toStatePtr, mVertexArray, mColorArray, fColor);
                     mTimeToIndex[simTime] = mColorArray->size() - 1;
                  }
                  ++fit;
               }
               else
               {
                  simTime = toStatePtr->simTime();
                  if (mTimeToIndex.find(simTime) == mTimeToIndex.end())
                  {
                     AddVertex(simTime, dispSimTime, *fromStatePtr, *toStatePtr, mVertexArray, mColorArray, fColor);
                     mTimeToIndex[simTime] = mColorArray->size() - 1;
                  }
                  if (fromStatePtr->simTime() == toStatePtr->simTime())
                  {
                     ++fit;
                  }
                  ++tit;
               }
            }
         }
         p1->EndRead();
         p2->EndRead();
      }
   }
   for (auto&& it : mTimeToIndex)
   {
      pset->push_back(it.second);
   }
}
