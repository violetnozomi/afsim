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

#ifndef WSFCOMMDATALINKLAYER_HPP
#define WSFCOMMDATALINKLAYER_HPP

#include "wsf_export.h"

#include <list>

#include "WsfCommLayer.hpp"
#include "WsfCommQueue.hpp"

namespace wsf
{
namespace comm
{

class WSF_EXPORT DatalinkLayer : public LayerImp
{
public:
   DatalinkLayer()           = default;
   ~DatalinkLayer() override = default;

   //! @name Common infrastructure methods
   //@{
   LayerImp* Clone() const override;
   bool      Initialize(double aSimTime) override { return true; }
   bool      ProcessInput(UtInput& aInput) override;
   //@}

   //! @name Send/Receive methods
   //@{
   bool Receive(double aSimTime, Comm* aXmtrPtr, Message& aMessage, bool& aNotifyNextLayer) override;

   bool Send(double aSimTime, size_t aLayerIndex, Message& aMessage, bool& aNotifyNextLayer) override;
   //@}

   //! @name Comm event methods.
   //@{
   bool ProcessLayerMessage(double aSimTime, layer::Message aLayerMessage, Message* aCommMessagePtr) override;
   //@}

protected:
   DatalinkLayer(const DatalinkLayer& aSrc);

   virtual bool AddToQueue(double aSimTime, Message& aMessage, size_t aPreviousAttempts = 0);

   virtual bool ReadyToTransmit(double aSimTime);

   virtual bool TransmitComplete(double aSimTime, Message* aMessagePtr);

   virtual bool TransmitFailed(double aSimTime, Message* aMessagePtr);

   virtual bool PurgeQueue(double aSimTime);

   Queue  mQueue{};                    //! The queue, where messages coming to this layer are placed.
   double mPurgeInterval{0.0};         //! How long in seconds a message can sit in the queue before being dropped.
   size_t mMaximumTransmitAttempts{0}; //! How many transmissions attempts for each message.
   double mReTransmitDelay{0.0};       //! How long in seconds between retransmission attempts.
   size_t mCurTransmissions{0U}; //! How many messages have been sent to the physical layer and are awaiting feedback.
};

} // namespace comm
} // namespace wsf

#endif
