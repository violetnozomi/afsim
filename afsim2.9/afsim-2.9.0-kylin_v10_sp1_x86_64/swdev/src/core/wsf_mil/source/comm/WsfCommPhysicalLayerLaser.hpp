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

#ifndef WSFCOMMPHYSICALLAYERLASER_HPP
#define WSFCOMMPHYSICALLAYERLASER_HPP

// Base class
#include "WsfCommPhysicalLayer.hpp"
class WsfLASER_CommXmtrComponent;

namespace wsf
{
namespace comm
{

class PhysicalLayerLaser : public PhysicalLayer
{
public:
   // Constructor and destructor
   PhysicalLayerLaser()           = default;
   ~PhysicalLayerLaser() override = default;
   PhysicalLayerLaser& operator=(const PhysicalLayerLaser& aRhs) = delete;

   //! @name Common infrastructure methods.
   //@{
   wsf::comm::LayerImp* Clone() const override;
   bool                 Initialize(double aSimTime) override;
   //@}

   double GetTransferRate() const override;

protected:
   // Copy constructor
   PhysicalLayerLaser(const PhysicalLayerLaser& aSrc) = default;

private:
   const WsfLASER_CommXmtrComponent* mLaserXmtr_ComponentPtr{nullptr};
};

} // namespace comm
} // namespace wsf

#endif
