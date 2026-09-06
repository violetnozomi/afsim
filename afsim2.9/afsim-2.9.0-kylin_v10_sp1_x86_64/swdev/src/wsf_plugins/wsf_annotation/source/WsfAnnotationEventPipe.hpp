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

#ifndef WSFANNOTATION_EVENTPIPE_HPP
#define WSFANNOTATION_EVENTPIPE_HPP

#include "WsfAnnotationDataTypes.hpp"
#include "WsfEventPipe.hpp"

class WsfEventPipeInterface;

namespace wsf
{
namespace annotation
{
class EventPipe : public WsfEventPipeLogger
{
public:
   static void RegisterEvents(WsfEventPipeExtension& aEventPipeExtension);
   static void RegisterMessages(UtPackSerializer* aSerializerPtr);
   explicit EventPipe(WsfEventPipeInterface* aEventPipeInterfacePtr);

   void AddMessages(const AnnotationData::AnnotationInfo& aAnnotationInfo);

private:
   void Send(double aSimTime, std::unique_ptr<WsfEventPipe::MsgBase> aMessagePtr)
   {
      mEventPipeInterfacePtr->SendImmediate(aSimTime, false, std::move(aMessagePtr));
   }

   void UpdateSubscriptions(const WsfEventPipeOptions& aNewCombinedOptions,
                            const WsfEventPipeOptions& aPreviousCombinedOptions) override
   {
   }

   WsfSimulation*         mSimulationPtr;
   WsfEventPipeInterface* mEventPipeInterfacePtr;
   int                    cANNOTATION;
};


class EventPipeInterface : public WsfSimulationExtension
{
public:
   static EventPipeInterface& Get(const WsfSimulation& aSimulation);
   explicit EventPipeInterface(const AnnotationData::AnnotationInfo& aAnnotationInfo)
      : mEventPipePtr(nullptr)
      , mAnnotationInfo(aAnnotationInfo)
   {
   }
   ~EventPipeInterface() override = default;

   void AddedToSimulation() override;
   void Start() override;

private:
   std::unique_ptr<EventPipe>     mEventPipePtr;
   AnnotationData::AnnotationInfo mAnnotationInfo;
};
} // namespace annotation
} // namespace wsf

#endif
