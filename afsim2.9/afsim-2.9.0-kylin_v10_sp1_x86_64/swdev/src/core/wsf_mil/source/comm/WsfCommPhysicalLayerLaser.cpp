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

#include "WsfCommPhysicalLayerLaser.hpp"

#include "WsfCommComponentHW.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfLASER_CommXmtrComponent.hpp"

namespace wsf
{
namespace comm
{

class Comm;
class Result;

// ==========================================================================
LayerImp* PhysicalLayerLaser::Clone() const
{
   return new PhysicalLayerLaser(*this);
}

// ==========================================================================
bool PhysicalLayerLaser::Initialize(double aSimTime)
{
   // Get the data rate from the laser xmtr component.
   ComponentHW* commHW_Ptr = ComponentHW::Find(*Parent());
   auto&        laserXmtr  = *(commHW_Ptr->GetEM_Xmtr(0).GetComponents().GetComponent<WsfLASER_CommXmtrComponent>());
   mLaserXmtr_ComponentPtr = &laserXmtr;

   return true;
}

// ==========================================================================
double PhysicalLayerLaser::GetTransferRate() const
{
   return mLaserXmtr_ComponentPtr->GetDataRate();
}

} // namespace comm
} // namespace wsf
