// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOMMPHYSICALLAYERJTIDS_HPP
#define WSFCOMMPHYSICALLAYERJTIDS_HPP

// Base class
#include "WsfCommPhysicalLayer.hpp"

namespace wsf
{
namespace comm
{

class PhysicalLayerJTIDS : public PhysicalLayer
{
public:
   // Constructor and destructor
   PhysicalLayerJTIDS()           = default;
   ~PhysicalLayerJTIDS() override = default;
   ;

   //! @name Common infrastructure methods.
   //@{
   wsf::comm::LayerImp* Clone() const override;
   bool                 Initialize(double aSimTime) override { return true; }
   bool                 ProcessInput(UtInput& aInput) override { return false; }
   //@}

   //! @name Send/Receive methods
   //@{
   bool Receive(double aSimTime, Comm* aXmtrPtr, Message& aMessage, bool& aNotifyNextLayer) override;
   bool Send(double aSimTime, size_t aLayerIndex, Message& aMessage, bool& aNotifyNextLayer) override;
   //@}

   //! @name Comm event methods.
   //@{
   bool ProcessLayerMessage(double aSimTime, layer::Message aMessage, Message* aCommMessagePtr) override;
   //@}

protected:
   // Copy constructor
   PhysicalLayerJTIDS(const PhysicalLayerJTIDS& aSrc) = default;

private:
   //! Prevent use of operator= by declaring, but not defining.
   PhysicalLayerJTIDS& operator=(const PhysicalLayerJTIDS& aRhs) = delete;
};

} // namespace comm
} // namespace wsf

#endif
