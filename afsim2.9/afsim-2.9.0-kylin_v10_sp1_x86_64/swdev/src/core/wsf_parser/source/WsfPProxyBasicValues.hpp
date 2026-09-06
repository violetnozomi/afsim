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

#ifndef WSFPPROXYBASICVALUES_HPP
#define WSFPPROXYBASICVALUES_HPP

#include <cassert>
#include <climits>
#include <cstring>
#include <limits>
#include <map>
#include <sstream>

#include "UtAngleR.hpp"
#include "UtAngleRate.hpp"
#include "UtDataSize.hpp"
#include "UtLatPos.hpp"
#include "UtLength.hpp"
#include "UtLonPos.hpp"
#include "UtSpeed.hpp"
#include "UtStringUtil.hpp"
#include "UtTime.hpp"
#include "UtUnitTypes.hpp"
#include "WsfPProxyCommon.hpp"
#include "WsfParseNode.hpp"

class WsfParseRule;

namespace WsfProxy
{
#pragma pack(push, 1)
//! @warning DO NOT MAKE VIRTUAL METHODS INSIDE WsfProxy::BasicValue OR DERIVED CLASSES!
//!          The WsfPProxyBasicType::BasicValueFlags requires that WsfProxy::BasicValue::mBasicValueFlags be at the
//!          front of the internal memory layout of WsfProxy::BasicValue.
class WSF_PARSER_EXPORT BasicValue
{
public:
   BasicValue()
      : mBasicValueFlags(cVALUE_UNSET)
   {
   }
   enum ValueFlags
   {
      cVALUE_INHERITED = WsfProxy::cVALUE_INHERITED,
      cVALUE_UNSET     = WsfProxy::cVALUE_UNSET,
      cVALUE_BITS      = cVALUE_UNSET,
      cCOPY_BITS       = cVALUE_INHERITED | cVALUE_UNSET
   };
   // Returns 'true' if the value is in the unset state
   bool IsUnset() const { return 0 != (mBasicValueFlags & cVALUE_UNSET); }
   //! @return whether the value is set
   bool IsSet() const noexcept { return !IsUnset(); }
   // Sets the value to the 'unset' state
   void SetUnset() { mBasicValueFlags |= cVALUE_UNSET; }
   void ClearUnset() { mBasicValueFlags &= ~cVALUE_UNSET; }
   bool IsInherited() const { return 0 != (mBasicValueFlags & cVALUE_INHERITED); }
   void SetInherited(bool aIsInherited)
   {
      if (aIsInherited)
      {
         mBasicValueFlags |= cVALUE_INHERITED;
      }
      else
      {
         mBasicValueFlags &= ~cVALUE_INHERITED;
      }
   }

protected:
   void          ValueAsssigned() { mBasicValueFlags &= ~(cVALUE_INHERITED | cVALUE_UNSET); }
   unsigned char mBasicValueFlags;
#ifdef WSF_PARSE_DEBUG_MEMORY
   UT_MEMORY_DEBUG_MARKER(cMDB_PROXY_BASIC_VALUE);
#endif
};

class WSF_PARSER_EXPORT Position : public BasicValue
{
public:
   static const WsfProxy::ValueKind cPROXY_TYPE_ID = WsfProxy::cPOSITION_VALUE;
   enum Format
   {
      cLAT_LON = 0,
      cMGRS    = 1
   };

   Position();
   Position(const UtLatPos&, const UtLonPos&);

   std::string ToString() const;
   void        Read(WsfParseNode* aNodePtr);
   std::string Write(WsfParseRule* aRulePtr);
   void        CopyValue(const Position& aRhs);
   bool        Equal(const Position& aRhs) const;
   bool        Less(const Position& aRhs) const;
   bool        SetFromString(const std::string& aString);
   void        SetMGRS(const std::string& aMGRS);

   UtLatPos GetLatitude() const;
   UtLonPos GetLongitude() const;
   void     SetLatitude(UtLatPos aLat)
   {
      UseLatLon();
      mLatitude  = aLat;
      mLatFormat = aLat.GetFormat();
      ClearUnset();
   }
   void SetLongitude(UtLonPos aLon)
   {
      UseLatLon();
      mLongitude = aLon;
      mLonFormat = aLon.GetFormat();
      ClearUnset();
   }
   bool operator==(const Position& aRhs) const
   {
      if (mFormat != aRhs.mFormat)
      {
         return false;
      }
      if (mFormat == cLAT_LON)
      {
         return mLatitude == aRhs.mLatitude && mLongitude == aRhs.mLongitude;
      }
      else
      {
         return memcmp((void*)mMGRS, (void*)aRhs.mMGRS, 16) == 0;
      }
   }

private:
   std::string PrintLatLon(bool aShowPositionKeyword) const;
   void        UseLatLon()
   {
      if (mFormat != cLAT_LON)
      {
         mFormat   = cLAT_LON;
         mLatitude = mLongitude = 0.0;
      }
   }
   Format mFormat;
   int    mLatFormat;
   int    mLonFormat;
   union
   {
      struct
      {
         double mLatitude;
         double mLongitude;
      };
      char mMGRS[16];
   };
};

class WSF_PARSER_EXPORT Length2 : public BasicValue
{
public:
   static const WsfProxy::ValueKind cPROXY_TYPE_ID = WsfProxy::cLENGTH2_VALUE;

   Length2();

   std::string   ToString() const;
   void          Read(WsfParseNode* aNodePtr);
   std::string   Write(WsfParseRule* /* aRulePtr */) { return ToString(); }
   void          CopyValue(const Length2& aRhs);
   bool          Equal(const Length2& aRhs) const;
   bool          Less(const Length2& aRhs) const;
   bool          SetFromString(const std::string& aString);
   bool          operator==(const Length2& aRhs) const { return Equal(aRhs); }
   bool          operator!=(const Length2& aRhs) const { return !Equal(aRhs); }
   unsigned char mFormat;
   double        mX;
   double        mY;
};

class WSF_PARSER_EXPORT Length3 : public BasicValue
{
public:
   static const WsfProxy::ValueKind cPROXY_TYPE_ID = WsfProxy::cLENGTH3_VALUE;

   Length3();

   std::string   ToString() const;
   void          Read(WsfParseNode* aNodePtr);
   std::string   Write(WsfParseRule* /* aRulePtr */) { return ToString(); }
   void          CopyValue(const Length3& aRhs);
   bool          Equal(const Length3& aRhs) const;
   bool          Less(const Length3& aRhs) const;
   bool          SetFromString(const std::string& aString);
   bool          operator==(const Length3& aRhs) const { return Equal(aRhs); }
   bool          operator!=(const Length3& aRhs) const { return !Equal(aRhs); }
   unsigned char mFormat;
   double        mX;
   double        mY;
   double        mZ;
};

// Booleans are stored as 'char' with 0=false, 1=true, 2=unset
class WSF_PARSER_EXPORT Bool : public BasicValue
{
public:
   static const WsfProxy::ValueKind cPROXY_TYPE_ID = WsfProxy::cBOOL_VALUE;
   Bool()
      : mValue(false)
   {
   }
   Bool(bool aValue)
      : mValue(aValue)
   {
      ClearUnset();
   }

   void SetFromString(const std::string& aText);
   void Read(WsfParseNode* aNodePtr)
   {
      std::string text = WsfParseNode::GetNodeString(aNodePtr);
      if (!text.empty())
      {
         SetFromString(text);
      }
   }
   std::string Write(WsfParseRule* /* aRulePtr */) { return ToString(); }
   std::string ToString() const
   {
      if (IsUnset())
      {
         return "unset";
      }
      if (mValue)
      {
         return "true";
      }
      return "false";
   }

        operator bool() { return mValue; }
   bool operator=(bool aRhs)
   {
      mValue = aRhs;
      ValueAsssigned();
      return mValue;
   }

   void CopyValue(const Bool& aRhs)
   {
      mValue           = aRhs.mValue;
      mBasicValueFlags = (mBasicValueFlags & ~cCOPY_BITS) | (cCOPY_BITS & aRhs.mBasicValueFlags);
   }
   bool Equal(const Bool& aRhs) const
   {
      if ((mBasicValueFlags & cVALUE_UNSET & aRhs.mBasicValueFlags) == cVALUE_UNSET)
      {
         return true;
      }
      if ((mBasicValueFlags ^ aRhs.mBasicValueFlags) & cVALUE_UNSET)
      {
         return false;
      }
      return mValue == aRhs.mValue;
   }
   bool Less(const Bool& aRhs) const { return mValue < aRhs.mValue; }
   bool Negate()
   {
      mValue = !mValue;
      return true;
   }
   bool IsTrue() { return mValue; }
   bool operator==(const Bool& aRhs) const { return Equal(aRhs); }
   bool operator!=(const Bool& aRhs) const { return !Equal(aRhs); }

private:
   bool mValue;
};

// Template class to handle most of the basic type routines.
// T is the C++ data type stored as the proxy value.
// T must support T(), T==T, T<T, T=T
template<typename T, WsfProxy::ValueKind PROXY_TYPE_ID>
class BasicValueT : public BasicValue
{
public:
   using ContainedBasicType                        = T;
   static const WsfProxy::ValueKind cPROXY_TYPE_ID = PROXY_TYPE_ID;

   BasicValueT()
      : mValue()
   {
   }
   void SetFromString(const std::string& aText)
   {
      std::stringstream ss(aText);
      ss >> mValue;
      ClearUnset();
   }
   void Read(WsfParseNode* aNodePtr)
   {
      // note: this is pretty inefficient, but it works on most types.
      // this should be re-written for each type to eliminate string copies and memory allocations.
      // Some types may be able to take advantage of the node structure to help read the value.
      std::string text = WsfParseNode::GetNodeString(aNodePtr);
      SetFromString(text);
   }
   std::string Write(WsfParseRule* /* aRulePtr */) { return ToString(); }
   std::string ToString() const
   {
      std::stringstream ss;
      if (!IsUnset())
      {
         ss << mValue;
      }
      else
      {
         ss << "";
      }
      return ss.str();
   }
   void CopyValue(const BasicValueT<T, PROXY_TYPE_ID>& aRhs)
   {
      mValue           = aRhs.mValue;
      mBasicValueFlags = (mBasicValueFlags & ~cCOPY_BITS) | (cCOPY_BITS & aRhs.mBasicValueFlags);
   }

   bool Equal(const BasicValueT<T, PROXY_TYPE_ID>& aRhs) const
   {
      if ((mBasicValueFlags & cVALUE_UNSET & aRhs.mBasicValueFlags) == cVALUE_UNSET)
      {
         return true;
      }
      if ((mBasicValueFlags ^ aRhs.mBasicValueFlags) & cVALUE_UNSET)
      {
         return false;
      }
      return mValue == aRhs.mValue;
   }
   bool     Less(const BasicValueT<T, PROXY_TYPE_ID>& aRhs) const { return mValue < aRhs.mValue; }
   T&       GetValue() { return mValue; }
   const T& GetValue() const { return mValue; }
   void     SetValue(const T& aValue)
   {
      mValue = aValue;
      ClearUnset();
   }
   bool operator==(const BasicValueT<T, PROXY_TYPE_ID>& aRhs) const { return Equal(aRhs); }
   bool operator!=(const BasicValueT<T, PROXY_TYPE_ID>& aRhs) const { return !Equal(aRhs); }

protected:
   T mValue;
};

template<typename T, WsfProxy::ValueKind PROXY_TYPE_ID>
class NumericValueT : public BasicValueT<T, PROXY_TYPE_ID>
{
public:
   bool Negate(void* p)
   {
      this->mValue = -this->mValue;
      return true;
   }
   NumericValueT()
      : BasicValueT<T, PROXY_TYPE_ID>()
   {
   }
   NumericValueT(const T& aValue) { this->SetValue(aValue); }
                                    operator T() const { return this->GetValue(); }
   NumericValueT<T, PROXY_TYPE_ID>& operator=(const T& aValue)
   {
      this->SetValue(aValue);
      return *this;
   }
};

template<typename T, WsfProxy::ValueKind PROXY_TYPE_ID>
class NumericUnitaryValueT : public NumericValueT<T, PROXY_TYPE_ID>
{
public:
   using UnitaryValueType         = T;
   static const int cUNIT_TYPE_ID = UnitaryValueType::UnitType::cUNIT_TYPE_ID;
   NumericUnitaryValueT()
      : NumericValueT<T, PROXY_TYPE_ID>()
   {
   }
   NumericUnitaryValueT(const T& aValue)
      : NumericValueT<T, PROXY_TYPE_ID>(aValue)
   {
   }
   NumericUnitaryValueT<T, PROXY_TYPE_ID>& operator=(const T& aValue)
   {
      this->SetValue(aValue);
      return *this;
   }
};


using Double = NumericValueT<double, WsfProxy::cDOUBLE_VALUE>;
template class NumericValueT<double, WsfProxy::cDOUBLE_VALUE>;
using Int = NumericValueT<int, WsfProxy::cINT_VALUE>;
template class NumericValueT<int, WsfProxy::cINT_VALUE>;
using Latitude = NumericValueT<UtLatPos, WsfProxy::cLATITUDE_VALUE>;
template class NumericValueT<UtLatPos, WsfProxy::cLATITUDE_VALUE>;
using Longitude = NumericValueT<UtLonPos, WsfProxy::cLONGITUDE_VALUE>;
template class NumericValueT<UtLonPos, WsfProxy::cLONGITUDE_VALUE>;
using Length = NumericUnitaryValueT<UtLengthValue, WsfProxy::cLENGTH_VALUE>;
template class NumericUnitaryValueT<UtLengthValue, WsfProxy::cLENGTH_VALUE>;
using Time = NumericUnitaryValueT<UtTimeValue, WsfProxy::cTIME_VALUE>;
template class NumericUnitaryValueT<UtTimeValue, WsfProxy::cTIME_VALUE>;
using Speed = NumericUnitaryValueT<UtSpeedValue, WsfProxy::cSPEED_VALUE>;
template class NumericUnitaryValueT<UtSpeedValue, WsfProxy::cSPEED_VALUE>;
using DataSize = NumericUnitaryValueT<UtDataSizeValue, WsfProxy::cDATASIZE_VALUE>;
template class NumericUnitaryValueT<UtDataSizeValue, WsfProxy::cDATASIZE_VALUE>;
using Power = NumericUnitaryValueT<UtPowerValue, WsfProxy::cPOWER_VALUE>;
template class NumericUnitaryValueT<UtPowerValue, WsfProxy::cPOWER_VALUE>;
using PowerDB = NumericUnitaryValueT<UtPowerDBValue, WsfProxy::cPOWERDB_VALUE>;
template class NumericUnitaryValueT<UtPowerDBValue, WsfProxy::cPOWERDB_VALUE>;
using SolidAngle = NumericUnitaryValueT<UtSolidAngleValue, WsfProxy::cSOLIDANGLE_VALUE>;
template class NumericUnitaryValueT<UtSolidAngleValue, WsfProxy::cSOLIDANGLE_VALUE>;
using Mass = NumericUnitaryValueT<UtMassValue, WsfProxy::cMASS_VALUE>;
template class NumericUnitaryValueT<UtMassValue, WsfProxy::cMASS_VALUE>;
using Force = NumericUnitaryValueT<UtForceValue, WsfProxy::cFORCE_VALUE>;
template class NumericUnitaryValueT<UtForceValue, WsfProxy::cFORCE_VALUE>;
using Torque = NumericUnitaryValueT<UtTorqueValue, WsfProxy::cTORQUE_VALUE>;
template class NumericUnitaryValueT<UtTorqueValue, WsfProxy::cTORQUE_VALUE>;
using Area = NumericUnitaryValueT<UtAreaValue, WsfProxy::cAREA_VALUE>;
template class NumericUnitaryValueT<UtAreaValue, WsfProxy::cAREA_VALUE>;
using AreaDB = NumericUnitaryValueT<UtAreaDBValue, WsfProxy::cAREADB_VALUE>;
template class NumericUnitaryValueT<UtAreaDBValue, WsfProxy::cAREADB_VALUE>;
using Volume = NumericUnitaryValueT<UtVolumeValue, WsfProxy::cVOLUME_VALUE>;
template class NumericUnitaryValueT<UtVolumeValue, WsfProxy::cVOLUME_VALUE>;
using Time2 = NumericUnitaryValueT<UtTime2Value, WsfProxy::cTIME2_VALUE>;
template class NumericUnitaryValueT<UtTime2Value, WsfProxy::cTIME2_VALUE>;
using Acceleration = NumericUnitaryValueT<UtAccelerationValue, WsfProxy::cACCELERATION_VALUE>;
template class NumericUnitaryValueT<UtAccelerationValue, WsfProxy::cACCELERATION_VALUE>;
using Frequency = NumericUnitaryValueT<UtFrequencyValue, WsfProxy::cFREQUENCY_VALUE>;
template class NumericUnitaryValueT<UtFrequencyValue, WsfProxy::cFREQUENCY_VALUE>;
using Angle = NumericUnitaryValueT<UtAngleValue, WsfProxy::cANGLE_VALUE>;
template class NumericUnitaryValueT<UtAngleValue, WsfProxy::cANGLE_VALUE>;
using AngularRate = NumericUnitaryValueT<UtAngularRateValue, WsfProxy::cANGULARRATE_VALUE>;
template class NumericUnitaryValueT<UtAngularRateValue, WsfProxy::cANGULARRATE_VALUE>;
using AngularAcceleration = NumericUnitaryValueT<UtAngularAccelerationValue, WsfProxy::cANGULARACCELERATION_VALUE>;
template class NumericUnitaryValueT<UtAngularAccelerationValue, WsfProxy::cANGULARACCELERATION_VALUE>;
using DataRate = NumericUnitaryValueT<UtDataRateValue, WsfProxy::cDATARATE_VALUE>;
template class NumericUnitaryValueT<UtDataRateValue, WsfProxy::cDATARATE_VALUE>;
using MassDensity = NumericUnitaryValueT<UtMassDensityValue, WsfProxy::cMASSDENSITY_VALUE>;
template class NumericUnitaryValueT<UtMassDensityValue, WsfProxy::cMASSDENSITY_VALUE>;
using MassTransfer = NumericUnitaryValueT<UtMassTransferValue, WsfProxy::cMASSTRANSFER_VALUE>;
template class NumericUnitaryValueT<UtMassTransferValue, WsfProxy::cMASSTRANSFER_VALUE>;
using Energy = NumericUnitaryValueT<UtEnergyValue, WsfProxy::cENERGY_VALUE>;
template class NumericUnitaryValueT<UtEnergyValue, WsfProxy::cENERGY_VALUE>;
using Fluence = NumericUnitaryValueT<UtFluenceValue, WsfProxy::cFLUENCE_VALUE>;
template class NumericUnitaryValueT<UtFluenceValue, WsfProxy::cFLUENCE_VALUE>;
using Irradiance = NumericUnitaryValueT<UtIrradianceValue, WsfProxy::cIRRADIANCE_VALUE>;
template class NumericUnitaryValueT<UtIrradianceValue, WsfProxy::cIRRADIANCE_VALUE>;
using Ratio = NumericUnitaryValueT<UtRatioValue, WsfProxy::cRATIO_VALUE>;
template class NumericUnitaryValueT<UtRatioValue, WsfProxy::cRATIO_VALUE>;
using NoisePressure = NumericUnitaryValueT<UtNoisePressureValue, WsfProxy::cNOISEPRESSURE_VALUE>;
template class NumericUnitaryValueT<UtNoisePressureValue, WsfProxy::cNOISEPRESSURE_VALUE>;
using Pressure = NumericUnitaryValueT<UtPressureValue, WsfProxy::cPRESSURE_VALUE>;
template class NumericUnitaryValueT<UtPressureValue, WsfProxy::cPRESSURE_VALUE>;
using Temperature = NumericUnitaryValueT<UtTemperatureValue, WsfProxy::cTEMPERATURE_VALUE>;
template class NumericUnitaryValueT<UtTemperatureValue, WsfProxy::cTEMPERATURE_VALUE>;
using SpecificRange = NumericUnitaryValueT<UtSpecificRangeValue, WsfProxy::cSPECIFICRANGE_VALUE>;
template class NumericUnitaryValueT<UtSpecificRangeValue, WsfProxy::cSPECIFICRANGE_VALUE>;
using AngularInertia = NumericUnitaryValueT<UtAngularInertiaValue, WsfProxy::cANGULARINERTIA_VALUE>;
template class NumericUnitaryValueT<UtAngularInertiaValue, WsfProxy::cANGULARINERTIA_VALUE>;

// 'unset' is represented as the empty string.
class WSF_PARSER_EXPORT String : public BasicValueT<std::string, WsfProxy::cSTRING_VALUE>
{
public:
   String() {}
   String(const std::string& aValue)
   {
      mValue = aValue;
      ClearUnset();
   }
   void SetFromString(const std::string& aText)
   {
      mValue = aText;
      ClearUnset();
   }
   void        SetValue(const std::string& aText) { mValue = aText; }
   std::string ToString() const
   {
      if (!IsUnset())
      {
         return mValue;
      }
      else
      {
         return std::string();
      }
   }
   bool Negate() { return false; }
};

// 'unset' is represented as the empty string.
class WSF_PARSER_EXPORT QuotableString : public BasicValueT<std::string, WsfProxy::cQUOTABLE_STRING_VALUE>
{
public:
   QuotableString() = default;
   QuotableString(const std::string& aValue)
   {
      mValue = aValue;
      ClearUnset();
   }
   void Read(WsfParseNode* aNodePtr)
   {
      mValue = WsfParseNode::GetNodeString(aNodePtr);
      ClearUnset();
   }
   void SetFromString(const std::string& aText)
   {
      mValue = aText;
      ClearUnset();
   }
   void        SetValue(const std::string& aText) { mValue = aText; }
   std::string ToString() const
   {
      if (!IsUnset())
      {
         return mValue;
      }
      else
      {
         return std::string();
      }
   }
   bool Negate() { return false; }
};

#pragma pack(pop)
} // namespace WsfProxy

//! Namespace for the World Simulation Framework (WSF)
namespace wsf
{
//! Namespace for the WSF Parser Proxy
namespace proxy
{

//! Denotes altitude reference
enum class AltitudeReferenceEnum
{
   //! denotes the altitude reference defaults to what the symbol (platform or mover) defines
   //! @note If the AltitudeReferenceValue functions return AltitudeReferenceEnum::Default, it is the callee's
   //! responsibility to determine the actual default
   Default,
   MSL, //!< denotes the altitude is measured from mean sea level (MSL)
   AGL  //!< denotes the altitude is measured from above ground level (AGL)
};

} // namespace proxy
} // namespace wsf

#endif
