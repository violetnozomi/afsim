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

#include "WsfAnnotationEventPipe.hpp"

#include "UtPackReflect.hpp"
#include "WsfAnnotation.hpp"
#include "WsfAnnotationEventPipeClasses.hpp"
#include "WsfAnnotationEventPipeClassesRegister.hpp"
#include "WsfAnnotationEventPipeSchema.hpp"
#include "WsfEventPipeClasses.hpp"
#include "WsfSimulation.hpp"

namespace
{
const std::string cANNOTATION_ID = "ANNOTATION";

WsfEventPipe::Vec3f GetColorVector(const UtColor& aColor)
{
   WsfEventPipe::Vec3f color;
   float               r;
   float               g;
   float               b;
   aColor.Get(r, g, b);
   color.x(r);
   color.y(g);
   color.z(b);
   return color;
}
} // namespace

void wsf::annotation::EventPipe::RegisterEvents(WsfEventPipeExtension& aEventPipeExtension)
{
   aEventPipeExtension.AddSchema(cWsfAnnotationEventPipe_SCHEMA);
   aEventPipeExtension.RegisterEventOption(cANNOTATION_ID, true);
   aEventPipeExtension.AddCallback(aEventPipeExtension.RegisterExtensionMessages.Connect(&EventPipe::RegisterMessages));
}

void wsf::annotation::EventPipe::RegisterMessages(UtPackSerializer* aSerializerPtr)
{
   WsfEventPipe::UtPack_register_all_wsf_annotation_events_types(*aSerializerPtr);
}

wsf::annotation::EventPipe::EventPipe(WsfEventPipeInterface* aEventPipeInterfacePtr)
   : mSimulationPtr(&aEventPipeInterfacePtr->GetSimulation())
   , mEventPipeInterfacePtr(aEventPipeInterfacePtr)
{
   cANNOTATION = aEventPipeInterfacePtr->GetEventId(cANNOTATION_ID);
}

void wsf::annotation::EventPipe::AddMessages(const AnnotationData::AnnotationInfo& aAnnotationInfo)
{
   if (mEventPipeInterfacePtr && !mEventPipeInterfacePtr->GetOutputFileName().empty())
   {
      for (const auto& poiIt : aAnnotationInfo.mPois)
      {
         auto poiMsg = ut::make_unique<WsfEventPipe::MsgAnnotationPoi>();
         poiMsg->poiName(poiIt.mPoiName);
         poiMsg->lat(poiIt.mLat);
         poiMsg->lon(poiIt.mLon);
         poiMsg->alt(poiIt.mAlt);
         poiMsg->bullseye(poiIt.mIsBullseye);
         Send(0, std::move(poiMsg));
      }
      for (const auto& decorationIt : aAnnotationInfo.mDecorations)
      {
         auto decorationMsg = ut::make_unique<WsfEventPipe::MsgAnnotationDecoration>();
         decorationMsg->entityName(decorationIt.mEntityName);
         decorationMsg->decorationType(static_cast<int>(decorationIt.mType));
         decorationMsg->decorationInfo(decorationIt.mInfo);
         Send(0, std::move(decorationMsg));
      }
      for (const auto& rangeRingIt : aAnnotationInfo.mRangeRings)
      {
         auto rangeRingMsg = ut::make_unique<WsfEventPipe::MsgAnnotationRangeRing>();
         rangeRingMsg->ringName(rangeRingIt.mRingName);
         rangeRingMsg->referenceName(rangeRingIt.mReferenceName);
         rangeRingMsg->lat(rangeRingIt.mLat);
         rangeRingMsg->lon(rangeRingIt.mLon);
         rangeRingMsg->showRangeLabels(rangeRingIt.mShowRangeLabels);
         rangeRingMsg->rangeLabelColor(GetColorVector(rangeRingIt.mRangeLabelColor));
         rangeRingMsg->ringColor(GetColorVector(rangeRingIt.mRingColor));
         rangeRingMsg->ringWidth(rangeRingIt.mRingWidth);
         rangeRingMsg->centerRadius(rangeRingIt.mCenterRadius);
         rangeRingMsg->additionalRings(rangeRingIt.mAdditionalRings);
         rangeRingMsg->additionalRadii(rangeRingIt.mAdditionalRadii);
         rangeRingMsg->showAngleLabels(rangeRingIt.mShowAngleLabels);
         rangeRingMsg->angleLabelColor(GetColorVector(rangeRingIt.mAngleLabelColor));
         rangeRingMsg->radialColor(GetColorVector(rangeRingIt.mRadialColor));
         rangeRingMsg->radialWidth(rangeRingIt.mRadialWidth);
         rangeRingMsg->centerAngle(rangeRingIt.mCenterAngle);
         rangeRingMsg->angleWidth(rangeRingIt.mAngleWidth);
         rangeRingMsg->radialRate(rangeRingIt.mRadialRate);
         rangeRingMsg->alignPlatform(rangeRingIt.mAlignPlatform);
         Send(0, std::move(rangeRingMsg));
      }
   }
}

void wsf::annotation::EventPipeInterface::Start()
{
   if (mEventPipePtr)
   {
      mEventPipePtr->AddMessages(mAnnotationInfo);
   }
}

wsf::annotation::EventPipeInterface& wsf::annotation::EventPipeInterface::Get(const WsfSimulation& aSimulation)
{
   return static_cast<EventPipeInterface&>(*aSimulation.FindExtension("wsf_annotation_eventpipe"));
}

void wsf::annotation::EventPipeInterface::AddedToSimulation()
{
   WsfEventPipeInterface* eventPipePtr = WsfEventPipeInterface::Find(GetSimulation());
   if (eventPipePtr != nullptr)
   {
      mEventPipePtr = ut::make_unique<EventPipe>(eventPipePtr);
      eventPipePtr->AddLogger(mEventPipePtr.get());
   }
}
