// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOMMSESSIONLAYER_HPP
#define WSFCOMMSESSIONLAYER_HPP

#include "wsf_export.h"

// Base class
#include "WsfCommLayer.hpp"

// Forward declarations
class WsfPlatform;

namespace wsf
{
namespace comm
{

class WSF_EXPORT SessionLayer : public LayerImp
{
public:
   // Constructor and destructor
   SessionLayer()
      : LayerImp()
   {
   }
   ~SessionLayer() override = default;

   //! @name Common infrastructure methods.
   //@{
   LayerImp* Clone() const override;
   bool      Initialize(double aSimTime) override { return true; }
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

protected:
   // Copy constructor
   SessionLayer(const SessionLayer& aSrc) {}

private:
   //! Prevent use of operator= by declaring, but not defining.
   SessionLayer& operator=(const SessionLayer& aRhs) = delete;
};

} // namespace comm
} // namespace wsf

#endif
