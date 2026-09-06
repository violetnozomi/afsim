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

#ifndef WSFCOMMNETWORKLAYER_HPP
#define WSFCOMMNETWORKLAYER_HPP

#include "wsf_export.h"

#include "WsfCommAddress.hpp"
#include "WsfCommLayer.hpp"
#include "WsfCommMessage.hpp"

namespace wsf
{
namespace comm
{

// Forward Declarations
class Network;

class WSF_EXPORT NetworkLayer : public LayerImp
{
public:
   // Constructor and destructor
   NetworkLayer()           = default;
   ~NetworkLayer() override = default;
   NetworkLayer& operator=(const NetworkLayer& aRhs) = delete;

   //! @name Common infrastructure methods.
   //@{
   LayerImp* Clone() const override;
   bool      Initialize(double aSimTime) override { return true; }
   bool      Initialize2(double aSimTime) override;
   bool      ProcessInput(UtInput& aInput) override { return false; }
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

   // Unique header for comm messages
   class NetworkLayerMessageHeader : public MessageHeader
   {
   public:
      NetworkLayerMessageHeader()                                      = default;
      ~NetworkLayerMessageHeader() override                            = default;
      NetworkLayerMessageHeader(const NetworkLayerMessageHeader& aSrc) = default;

      MessageHeader* Clone() const override { return new NetworkLayerMessageHeader(*this); }

      unsigned int mNetworkProtocol{0U};
   };

protected:
   // Copy constructor
   NetworkLayer(const NetworkLayer& aSrc) = default;
};

} // namespace comm
} // namespace wsf

#endif
