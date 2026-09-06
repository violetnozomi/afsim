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

#ifndef WSFCOMMTRANSPORTLAYER_HPP
#define WSFCOMMTRANSPORTLAYER_HPP

#include "wsf_export.h"

// Base class
#include "WsfCommLayer.hpp"

namespace wsf
{
namespace comm
{

class Message;

class WSF_EXPORT TransportLayer : public LayerImp
{
public:
   //! The bit mask values indicate the presence of certain
   //! capabilities usually attributed to a type of protocol as defined in the
   //! transport layer. The mask is stored via mTransportFeature. Presence of
   //! the bit value indicates the protocol incorporates a specific feature,
   //! as described below. Note that at this time, not all of these values
   //! are used but are provided for future or extended capabilities.
   //! NOTE: Most of the below are specific to the mechanics of packet based delivery.
   //! AFSIM comms are message based, and therefore do not split messages into packets.
   //! However, this mode of operation is possible given user extension of layer mechanics,
   //! and some of these features have implications that can be simulated in general
   //! simulation use. For example, reliable transport, can generally be applied to the
   //! message reception itself (such as if a packet went missing during a TCP transmission),
   //! to indicate message transmission failure that can be modeled with a general transmit failure.
   enum
   {
      cRELIABLE_TRANSPORT = 0x00000001, //! ACK/NACK or other confirmation of reception
      cORDERED_DELIVERY   = 0x00000002, //! Correct ordering, usually via segment numbering
      cCHECKSUM           = 0x00000004, //! A checksum value in included, which infers error checking and/or correction
      cMTU                = 0x00000008, //! A maximum transmission unit is defined
      cFLOW_CONTROL       = 0x00000010, //! Flow control mechanism, to avoid overwhelming a receiver
      cCONGESTION_CONTROL = 0x00000020, //! Like flow control, except based on network state (congestion)
      cNAGLE_ALGORITHM    = 0x00000040  //! Algorithm to avoid sending small messages by batching multiple messages
   };

   // Constructor and destructor
   TransportLayer()           = default;
   ~TransportLayer() override = default;
   TransportLayer& operator=(const TransportLayer& aRhs) = delete;

   //! @name Common infrastructure methods.
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

   //! Returns the mask variable regarding which features the transport layer supports.
   unsigned int GetTransportFeatures() const { return mTransportFeature; }

protected:
   // Copy constructor
   TransportLayer(const TransportLayer& aSrc) = default;

private:
   unsigned int mTransportFeature{0};
   unsigned int mMTU{0};      //! In bytes
   unsigned int mChecksum{0}; //! Header checksum size, in bits

   //! Other
};

} // namespace comm
} // namespace wsf

#endif
