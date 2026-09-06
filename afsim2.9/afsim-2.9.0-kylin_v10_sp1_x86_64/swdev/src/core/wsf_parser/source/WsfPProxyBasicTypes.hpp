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

#ifndef VIPROXYBASICTYPES_HPP
#define VIPROXYBASICTYPES_HPP

#include "wsf_parser_export.h"

#include "WsfPProxyBasicValues.hpp"
#include "WsfParseNode.hpp"

// clang-format off
#include "WsfPProxyValue.hpp"
#include "WsfPProxyType.hpp"
// clang-format on

//
// This file contains many of the basic proxy types.
// Important:
// - Most basic types share the same format as normal c++ types, however, any value stored in the proxy
//   must have the capability to be stored as the 'unset' value.
// - Placement New heavily used here to construct values in a preexisting memory location


// Template class to handle most of the basic type routines.
// T is the C++ data type stored as the proxy value.
// T must support T(), T==T, T<T, T=T

template<typename VALUE_TYPE>
class ProxyTypeAdapterBaseT : public WsfPProxyBasicType
{
public:
   using DataType = VALUE_TYPE;
   ProxyTypeAdapterBaseT(unsigned int aTypeFlags = WsfProxy::cPOD)
      : WsfPProxyBasicType(VALUE_TYPE::cPROXY_TYPE_ID)
   {
      mTypeFlags |= aTypeFlags;
      mDataSize = sizeof(VALUE_TYPE);
   }

protected:
   static VALUE_TYPE& Get(void* p) { return *(VALUE_TYPE*)p; }

public:
   void        Construct(void* p) const override { new (p) VALUE_TYPE(); }
   void        Destroy(void* p) const override { Get(p).~VALUE_TYPE(); }
   void        SetValue(void* p, const std::string& aText) const override { Get(p).SetFromString(aText); }
   std::string ToString(void* p) const override { return Get(p).ToString(); }
   void        Copy(void* dest, void* src, int flags) const override { Get(dest).CopyValue(Get(src)); }
   bool        Equal(void* lhs, void* rhs) const override { return Get(lhs).Equal(Get(rhs)); }
   bool        Less(void* lhs, void* rhs) const override { return Get(lhs).Less(Get(rhs)); }
};

template<typename VALUE_TYPE>
class ProxyTypeAdapterT : public ProxyTypeAdapterBaseT<VALUE_TYPE>
{
public:
   using DataType = VALUE_TYPE;
   ProxyTypeAdapterT(unsigned int aTypeFlags = WsfProxy::cPOD)
      : ProxyTypeAdapterBaseT<VALUE_TYPE>(aTypeFlags)
   {
   }

protected:
   static VALUE_TYPE& Get(void* p) { return *(VALUE_TYPE*)p; }

public:
   void        Read(void* p, WsfParseNode* aNodePtr) const override { Get(p).Read(aNodePtr); }
   std::string Write(void* p, WsfParseRule* aRulePtr) const override { return Get(p).Write(aRulePtr); }
};

template<typename VALUE_TYPE>
class ProxyUnitaryTypeAdapterT : public ProxyTypeAdapterT<VALUE_TYPE>
{
public:
   using UnitaryType = typename VALUE_TYPE::ContainedBasicType;
   ProxyUnitaryTypeAdapterT(unsigned int aTypeFlags = WsfProxy::cPOD)
      : ProxyTypeAdapterT<VALUE_TYPE>(aTypeFlags)
   {
   }


   int GetUnitTypeId() const override { return UnitaryType::UnitType::cUNIT_TYPE_ID; }
};

class WsfPProxyPositionType : public ProxyTypeAdapterT<WsfProxy::Position>
{
};
class WsfPProxyLength2Type : public ProxyTypeAdapterT<WsfProxy::Length2>
{
};
class WsfPProxyLength3Type : public ProxyTypeAdapterT<WsfProxy::Length3>
{
};
class WsfPProxyBoolType : public ProxyTypeAdapterT<WsfProxy::Bool>
{
};
class WsfPProxyRealType : public ProxyTypeAdapterT<WsfProxy::Double>
{
};
class WsfPProxyIntType : public ProxyTypeAdapterT<WsfProxy::Int>
{
};
class WSF_PARSER_EXPORT WsfPProxyStringType : public ProxyTypeAdapterT<WsfProxy::String>
{
public:
   WsfPProxyStringType();
   void          Construct(void* p) const override;
   WsfPProxyHash Hash(void* aDataPtr) const override;
   size_t        MemoryUsage(void* aDataPtr) const override;
   std::string   Write(void* p, WsfParseRule* aRulePtr) const override;
};
class WSF_PARSER_EXPORT WsfPProxyQuotableStringType : public ProxyTypeAdapterT<WsfProxy::QuotableString>
{
public:
   WsfPProxyQuotableStringType();
   void          Construct(void* p) const override;
   WsfPProxyHash Hash(void* aDataPtr) const override;
   size_t        MemoryUsage(void* aDataPtr) const override;
   std::string   Write(void* p, WsfParseRule* aRulePtr) const override;
};

#define WSF_PPROXY_DEFINE_BASIC_TYPE(NAME, BASE)             \
   class WsfPProxy##NAME##Type : public BASE<WsfProxy::NAME> \
   {                                                         \
   }

#define WSF_PPROXY_DEFINE_UNITARY_TYPE(NAME) WSF_PPROXY_DEFINE_BASIC_TYPE(NAME, ProxyUnitaryTypeAdapterT)

WSF_PPROXY_DEFINE_BASIC_TYPE(Latitude, ProxyTypeAdapterT);
WSF_PPROXY_DEFINE_BASIC_TYPE(Longitude, ProxyTypeAdapterT);


WSF_PPROXY_DEFINE_UNITARY_TYPE(Length);
WSF_PPROXY_DEFINE_UNITARY_TYPE(Time);
WSF_PPROXY_DEFINE_UNITARY_TYPE(Speed);
WSF_PPROXY_DEFINE_UNITARY_TYPE(DataSize);
WSF_PPROXY_DEFINE_UNITARY_TYPE(Power);
WSF_PPROXY_DEFINE_UNITARY_TYPE(PowerDB);
WSF_PPROXY_DEFINE_UNITARY_TYPE(SolidAngle);
WSF_PPROXY_DEFINE_UNITARY_TYPE(Mass);
WSF_PPROXY_DEFINE_UNITARY_TYPE(Force);
WSF_PPROXY_DEFINE_UNITARY_TYPE(Torque);
WSF_PPROXY_DEFINE_UNITARY_TYPE(Area);
WSF_PPROXY_DEFINE_UNITARY_TYPE(AreaDB);
WSF_PPROXY_DEFINE_UNITARY_TYPE(Volume);
WSF_PPROXY_DEFINE_UNITARY_TYPE(Time2);
WSF_PPROXY_DEFINE_UNITARY_TYPE(Acceleration);
WSF_PPROXY_DEFINE_UNITARY_TYPE(Frequency);
WSF_PPROXY_DEFINE_UNITARY_TYPE(Angle);
WSF_PPROXY_DEFINE_UNITARY_TYPE(AngularRate);
WSF_PPROXY_DEFINE_UNITARY_TYPE(AngularAcceleration);
WSF_PPROXY_DEFINE_UNITARY_TYPE(DataRate);
WSF_PPROXY_DEFINE_UNITARY_TYPE(MassDensity);
WSF_PPROXY_DEFINE_UNITARY_TYPE(MassTransfer);
WSF_PPROXY_DEFINE_UNITARY_TYPE(Energy);
WSF_PPROXY_DEFINE_UNITARY_TYPE(Fluence);
WSF_PPROXY_DEFINE_UNITARY_TYPE(Irradiance);
WSF_PPROXY_DEFINE_UNITARY_TYPE(Ratio);
WSF_PPROXY_DEFINE_UNITARY_TYPE(NoisePressure);
WSF_PPROXY_DEFINE_UNITARY_TYPE(Pressure);
WSF_PPROXY_DEFINE_UNITARY_TYPE(Temperature);
WSF_PPROXY_DEFINE_UNITARY_TYPE(SpecificRange);
WSF_PPROXY_DEFINE_UNITARY_TYPE(AngularInertia);

using WsfPEnumStrMap = std::map<std::string, int>;
using WsfPEnumIntMap = std::map<int, std::string>;

// Enumerations are stored as integers.
class WSF_PARSER_EXPORT WsfPProxyEnumType : public ProxyTypeAdapterT<WsfProxy::Int>
{
   static const int cNOT_SET_VALUE = -INT_MAX + 3;

public:
   WsfPProxyEnumType();

private:
   static WsfProxy::Int& Get(void* p) { return *(WsfProxy::Int*)p; }

public:
   void Construct(void* p) const override;

   void        SetValue(void* p, const std::string& aText) const override;
   void        Read(void* p, WsfParseNode* aNodePtr) const override;
   std::string Write(void* p, WsfParseRule* aRulePtr) const override;
   std::string ToString(void* p) const override;

   bool IsTypeOf(const WsfPProxyType* aOtherType) const override;

   WsfPEnumStrMap mStringToInt;
   WsfPEnumIntMap mIntToString;
};

#endif
