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

#ifndef WSFCOMMPHYSICALLAYER_HPP
#define WSFCOMMPHYSICALLAYER_HPP

#include "wsf_export.h"

#include "WsfCommLayer.hpp"
#include "WsfCommResult.hpp"
#include "WsfRandomVariable.hpp"

namespace wsf
{
namespace comm
{

class WSF_EXPORT PhysicalLayer : public LayerImp
{
public:
   // Constructor and destructor
   PhysicalLayer()           = default;
   ~PhysicalLayer() override = default;

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

   virtual double GetTransferRate() const { return mTransferRate.Draw(); }

protected:
   // Pass-through call for ComponentHW::CanReceiveFrom.
   // Returns true if the related models indicate that
   // a message can be received. Returns false otherwise,
   // or when a hardware component is not being utilized.
   virtual bool CanReceiveFrom(double aSimTime, Comm& aXmtr, Result& aResult);

   // Copy constructor
   PhysicalLayer(const PhysicalLayer& aSrc) = default;

   //! Prevent use of operator= by declaring, but not defining.
   PhysicalLayer& operator=(const PhysicalLayer& aRhs) = delete;

   //! Transfer rate cap for the owning comm model. Any transmission
   //! will not exceed this transfer rate, even if the utilized medium
   //! allows a greater transfer rate. In legacy use cases, this value
   //! is always the limiting factor for transmission rate. Value is
   //! represented in bits/second.
   mutable WsfRandomVariable mTransferRate{0.0, WsfRandomVariable::cPOSITIVE};
};

} // namespace comm
} // namespace wsf

#endif
