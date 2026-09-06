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

#ifndef WSFCOMMLAYEREVENT_HPP
#define WSFCOMMLAYEREVENT_HPP

#include <memory>

#include "WsfCommLayerMessage.hpp"
#include "WsfCommMessage.hpp"
#include "WsfEvent.hpp"

namespace wsf
{
namespace comm
{

class Comm;

class LayerEvent : public WsfEvent
{
public:
   //! This message is just a wrapper for a generic event, to avoid
   static void ScheduleCommLayerEvent(double         aSimTime,
                                      size_t         aLayerIndex,
                                      Comm*          aXmtrPtr,
                                      layer::Message aMessage,
                                      Message*       aCommMessagePtr = nullptr);

   LayerEvent(double aSimTime, size_t aLayerIndex, Comm* aXmtrPtr, layer::Message aMessage, Message* aCommMessagePtr);

   ~LayerEvent() override = default;

   EventDisposition Execute() override;

   size_t                   mCommLayerIndex;
   size_t                   mXmtrPlatformIndex;
   Comm*                    mXmtrPtr;
   layer::Message           mLayerMessage;
   std::unique_ptr<Message> mCommMessagePtr;
};

} // namespace comm
} // namespace wsf

#endif
