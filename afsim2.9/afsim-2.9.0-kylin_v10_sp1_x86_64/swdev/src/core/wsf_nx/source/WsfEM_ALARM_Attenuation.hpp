// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFEM_ALARM_ATTENUATION_HPP
#define WSFEM_ALARM_ATTENUATION_HPP

#include "WsfPlatform.hpp"
class WsfEM_Xmtr;

//! Interface to the WSF attenuation model from the ALARM clutter model.
//!
//! @note This is only intended to work for interfacing with the clutter model.
//!       The attenuation to/from the target is handled directly by WSF.
namespace WsfEM_ALARM_Attenuation
{
class atmosphere
{
public:
   atmosphere(WsfEM_Xmtr* aXmtrPtr);
   ~atmosphere();
   double attenuation(double elevation, double frequency, double range, double rkfact);

private:
   WsfEM_Xmtr*  mXmtrPtr;
   WsfPlatform* mTempPlatform;
};

double attenuation(atmosphere& atm, double elevation, double frequency, double range, double rkfact);
} // namespace WsfEM_ALARM_Attenuation

#endif
