// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFPPROXYVALUE_INLINE_HPP
#define WSFPPROXYVALUE_INLINE_HPP

#include "WsfPProxyType.hpp"
#include "WsfPProxyValue.hpp"

template<class T>
T* WsfPProxyValue::CastToValue() const
{
   if (mTypePtr && mTypePtr->mTypeStoredKind == T::cPROXY_TYPE_ID)
   {
      return (T*)GetDataPtr();
   }
   return nullptr;
}

// implement conversion and assignment to/from basic values
#define IMPLEMENT_TYPE_OPERATOR(VALUE_TYPE)                                                  \
   inline WsfPProxyValue::operator VALUE_TYPE*() const { return CastToValue<VALUE_TYPE>(); } \
   inline WsfPProxyValue& WsfPProxyValue::operator=(const VALUE_TYPE& aVal)                  \
   {                                                                                         \
      VALUE_TYPE* vPtr = CastToValue<VALUE_TYPE>();                                          \
      if (vPtr)                                                                              \
         *vPtr = aVal;                                                                       \
      return *this;                                                                          \
   }

IMPLEMENT_TYPE_OPERATOR(WsfProxy::String)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::Position)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::Length2)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::Bool)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::Double)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::Int)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::Latitude)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::Longitude)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::Length)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::Time)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::Speed)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::DataSize)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::Power)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::PowerDB)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::SolidAngle)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::Mass)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::Force)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::Torque)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::Area)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::AreaDB)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::Volume)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::Time2)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::Acceleration)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::Frequency)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::Angle)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::AngularRate)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::AngularAcceleration)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::DataRate)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::MassDensity)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::MassTransfer)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::Energy)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::Fluence)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::Irradiance)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::Ratio)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::NoisePressure)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::Pressure)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::Temperature)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::SpecificRange)
IMPLEMENT_TYPE_OPERATOR(WsfProxy::AngularInertia)
#undef IMPLEMENT_TYPE_OPERATOR


#endif
